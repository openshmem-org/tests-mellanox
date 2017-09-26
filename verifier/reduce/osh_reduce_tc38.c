/*
 * Copyright (c) 2014      Mellanox Technologies, Inc.
 *                         All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "osh_def.h"
#include "osh_cmn.h"
#include "osh_log.h"

#include "shmem.h"

#include "osh_reduce_tests.h"

/****************************************************************************
 * Test Case can consitis of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
static int test_item1(void);
static int test_item2(void);
static int test_item3(void);
static int test_item4(void);
static int test_item5(void);
static int test_item6(void);
static int test_item7(void);
static int test_item8(void);


#ifdef QUICK_TEST
#define WAIT_COUNT  1
#else
#define WAIT_COUNT  5
#endif
#define TYPE_VALUE  long
#define FUNC_VALUE  shmem_long_prod_to_all
#define SIZE_VALUE  sizeof(TYPE_VALUE)
#define MAX_VALUE   (((unsigned long long)1 << ( 8 * SIZE_VALUE - 1)) - 1)
#define BASE_VALUE  0x0F
#define DEFAULT_VALUE  (-1)
#define OVERFLOW_FACTORIAL_LIMIT (9) /* (9 is for long)
                                        to avoid data overflow case for n! (1, 1, 2, 6, 24, 120, 720, 5040, 40320,) */


/*
 * Parse command line options specific for test case.
 */
static int __parse_opt( const TE_NODE *, int, const char ** );

#ifdef QUICK_TEST
#define COUNT_VALUE 10
#else
#define COUNT_VALUE 100
#endif
#define MAX_BUFFER_SIZE 8096

static long __max_buffer_size = MAX_BUFFER_SIZE;
static long __cycle_count = COUNT_VALUE;
static long* pSync = NULL;
static TYPE_VALUE* pWrk = NULL;


/*
 * List of supported test case options.
 */
static const AOPT_DESC  self_opt_desc[] =
{
    {
        'm', AOPT_ARG,      aopt_set_literal( 0 ),    aopt_set_string( "max-buffer-size" ),
            "Define maximum buffer size (default: " STR(MAX_BUFFER_SIZE) " )."
    },
    {
        'c', AOPT_ARG,      aopt_set_literal( 0 ),    aopt_set_string( "cycle-count" ),
            "Number of different buffers used in the test (default: " STR(COUNT_VALUE) " )."
    },
	{ 0, AOPT_NOARG, aopt_set_literal( 0 ), aopt_set_string( NULL ), NULL }
};


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_reduce_tc38(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    rc = __parse_opt(node, argc, argv);

    if (rc == TC_PASS)
    {
        pSync = shmalloc(sizeof(*pSync) * _SHMEM_REDUCE_SYNC_SIZE);
        if (!pSync)
        {
            rc = TC_SETUP_FAIL;
        }
    } else {
        rc = TC_SETUP_FAIL;
    }

    /* Every PE does reduction of the single value as symmetric data object to itself */
    if (rc == TC_PASS)
    {
        rc = test_item1();
        log_item(node, 1, rc);
        shmem_barrier_all();
    }

    /* All PEs reduce the single value */
    if (rc == TC_PASS)
    {
        rc = test_item2();
        log_item(node, 2, rc);
        shmem_barrier_all();
    }

    /* Every PE does reduction of the single value as symmetric data object to itself
     * (target and source are the same array)
     */
    if (rc == TC_PASS)
    {
        rc = test_item3();
        log_item(node, 3, rc);
        shmem_barrier_all();
    }

    /* All PEs reduce the single value
     * (target and source are the same array)
     */
    if (rc == TC_PASS)
    {
        rc = test_item4();
        log_item(node, 4, rc);
        shmem_barrier_all();
    }

    /* Every PE does reduction of the buffer as symmetric data object to itself */
    if (rc == TC_PASS)
    {
        rc = test_item5();
        log_item(node, 5, rc);
        shmem_barrier_all();
    }

    /* All PEs reduce the buffer */
    if (rc == TC_PASS)
    {
        rc = test_item6();
        log_item(node, 6, rc);
        shmem_barrier_all();
    }

    /* Even PEs reduce the buffer */
    if (rc == TC_PASS)
    {
        rc = test_item7();
        log_item(node, 7, rc);
        shmem_barrier_all();
    }

    /* reduce calls in loop with alternating multiple pSync and pWrk arrays (without barrrier synchronization between iterations) */
    if (rc == TC_PASS)
    {
        rc = test_item8();
        log_item(node, 8, rc);
        shmem_barrier_all();
    }

    if (pSync)
    {
        shfree(pSync);
    }

    return rc;
}


/****************************************************************************
 * Place for Test Item functions
 ***************************************************************************/
static int test_item1(void)
{
    int rc = TC_PASS;
    TYPE_VALUE* target_addr = NULL;
    TYPE_VALUE* source_addr = NULL;
    TYPE_VALUE source_value = 0;
    TYPE_VALUE expect_value = 0;
    int my_proc = 0;

    my_proc = _my_pe();

    pWrk = shmalloc(sizeof(*pWrk) * sys_max(1/2 + 1, _SHMEM_REDUCE_MIN_WRKDATA_SIZE));
    if (pWrk)
    {
        target_addr = shmalloc(sizeof(*target_addr));
        source_addr = shmalloc(sizeof(*source_addr));
    }

    if (target_addr && source_addr)
    {
        TYPE_VALUE value = DEFAULT_VALUE;
        int j = 0;

        /* Set initial target value */
        *target_addr = DEFAULT_VALUE;

        /* Set my value */
        source_value = (TYPE_VALUE)(BASE_VALUE + my_proc);
        *source_addr = source_value;

        /* Define expected value */
        expect_value = source_value;

        /* This guarantees that PE set initial value before peer change one */
        for ( j = 0; j < _SHMEM_REDUCE_SYNC_SIZE; j++ )
        {
            pSync[j] = _SHMEM_SYNC_VALUE;
        }
        shmem_barrier_all();

        /* Put value to peer */
        FUNC_VALUE(target_addr, source_addr, 1, my_proc, 0, 1, pWrk, pSync);

        /* Get value put by peer:
         * These routines start the remote transfer and may return before the data
         * is delivered to the remote PE
         */
        shmem_barrier_all();
        {
            int total_wait = 0;
            while (*target_addr == DEFAULT_VALUE && total_wait < 1000 * WAIT_COUNT)
            {
                total_wait++;
                usleep(1);
            }
            value = *target_addr;
        }

        rc = (expect_value == value ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my#%d source = %lld expected = %lld actual = %lld\n",
                           my_proc, (INT64_TYPE)source_value, (INT64_TYPE)expect_value, (INT64_TYPE)value);
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (source_addr)
    {
        shfree(source_addr);
    }

    if (target_addr)
    {
        shfree(target_addr);
    }

    if (pWrk)
    {
        shfree(pWrk);
        pWrk = NULL;
    }

    return rc;
}


static int test_item2(void)
{
    int rc = TC_PASS;
    TYPE_VALUE* target_addr = NULL;
    TYPE_VALUE* source_addr = NULL;
    TYPE_VALUE source_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();


    pWrk = shmalloc(sizeof(*pWrk) * sys_max(1/2 + 1, _SHMEM_REDUCE_MIN_WRKDATA_SIZE));
    if (pWrk)
    {
        target_addr = shmalloc(sizeof(*target_addr));
        source_addr = shmalloc(sizeof(*source_addr));
    }

    if (target_addr && source_addr)
    {
        TYPE_VALUE value = DEFAULT_VALUE;
        int j = 0;

        /* Set initial target value */
        *target_addr = DEFAULT_VALUE;

        /* Set my value */
        source_value = ( my_proc < OVERFLOW_FACTORIAL_LIMIT ? (TYPE_VALUE)(my_proc + 1) : 1);
        *source_addr = source_value;

        /* Define expected value */
        expect_value = 1;
        {
            int k = ( num_proc <= OVERFLOW_FACTORIAL_LIMIT ? num_proc : OVERFLOW_FACTORIAL_LIMIT);
            while (k) expect_value *= k--;
        }

        /* This guarantees that PE set initial value before peer change one */
        for ( j = 0; j < _SHMEM_REDUCE_SYNC_SIZE; j++ )
        {
            pSync[j] = _SHMEM_SYNC_VALUE;
        }
        shmem_barrier_all();

        /* Put value to peer */
        FUNC_VALUE(target_addr, source_addr, 1, 0, 0, num_proc, pWrk, pSync);

        /* Get value put by peer:
         * These routines start the remote transfer and may return before the data
         * is delivered to the remote PE
         */
        shmem_barrier_all();
        {
            int total_wait = 0;
            while (*target_addr == DEFAULT_VALUE && total_wait < 1000 * WAIT_COUNT)
            {
                total_wait++;
                usleep(1);
            }
            value = *target_addr;
        }

        rc = (expect_value == value ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my#%d source = %lld expected = %lld actual = %lld\n",
                           my_proc, (INT64_TYPE)source_value, (INT64_TYPE)expect_value, (INT64_TYPE)value);
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (source_addr)
    {
        shfree(source_addr);
    }

    if (target_addr)
    {
        shfree(target_addr);
    }

    if (pWrk)
    {
        shfree(pWrk);
        pWrk = NULL;
    }

    return rc;
}


static int test_item3(void)
{
    int rc = TC_PASS;
    TYPE_VALUE* target_addr = NULL;
    TYPE_VALUE* source_addr = NULL;
    TYPE_VALUE source_value = 0;
    TYPE_VALUE expect_value = 0;
    int my_proc = 0;

    my_proc = _my_pe();

    pWrk = shmalloc(sizeof(*pWrk) * sys_max(1/2 + 1, _SHMEM_REDUCE_MIN_WRKDATA_SIZE));
    if (pWrk)
    {
        source_addr = shmalloc(sizeof(*source_addr));
        target_addr = source_addr;
    }

    if (target_addr && source_addr)
    {
        TYPE_VALUE value = DEFAULT_VALUE;
        int j = 0;

        /* Set my value */
        source_value = (TYPE_VALUE)(BASE_VALUE + my_proc);
        *source_addr = source_value;

        /* Define expected value */
        expect_value = source_value;

        /* This guarantees that PE set initial value before peer change one */
        for ( j = 0; j < _SHMEM_REDUCE_SYNC_SIZE; j++ )
        {
            pSync[j] = _SHMEM_SYNC_VALUE;
        }
        shmem_barrier_all();

        /* Put value to peer */
        FUNC_VALUE(target_addr, source_addr, 1, my_proc, 0, 1, pWrk, pSync);

        /* Get value put by peer:
         * These routines start the remote transfer and may return before the data
         * is delivered to the remote PE
         */
        shmem_barrier_all();
        {
            int total_wait = 0;
            while (*target_addr == DEFAULT_VALUE && total_wait < 1000 * WAIT_COUNT)
            {
                total_wait++;
                usleep(1);
            }
            value = *target_addr;
        }

        rc = (expect_value == value ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my#%d source = %lld expected = %lld actual = %lld\n",
                           my_proc, (INT64_TYPE)source_value, (INT64_TYPE)expect_value, (INT64_TYPE)value);
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (source_addr)
    {
        shfree(source_addr);
    }

    if (pWrk)
    {
        shfree(pWrk);
        pWrk = NULL;
    }

    return rc;
}


static int test_item4(void)
{
    int rc = TC_PASS;
    TYPE_VALUE* target_addr = NULL;
    TYPE_VALUE* source_addr = NULL;
    TYPE_VALUE source_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();


    pWrk = shmalloc(sizeof(*pWrk) * sys_max(1/2 + 1, _SHMEM_REDUCE_MIN_WRKDATA_SIZE));
    if (pWrk)
    {
        source_addr = shmalloc(sizeof(*source_addr));
        target_addr = source_addr;
    }

    if (target_addr && source_addr)
    {
        TYPE_VALUE value = DEFAULT_VALUE;
        int j = 0;

        /* Set my value */
        source_value = ( my_proc < OVERFLOW_FACTORIAL_LIMIT ? (TYPE_VALUE)(my_proc + 1) : 1);
        *source_addr = source_value;

        /* Define expected value */
        expect_value = 1;
        {
            int k = ( num_proc <= OVERFLOW_FACTORIAL_LIMIT ? num_proc : OVERFLOW_FACTORIAL_LIMIT);
            while (k) expect_value *= k--;
        }

        /* This guarantees that PE set initial value before peer change one */
        for ( j = 0; j < _SHMEM_REDUCE_SYNC_SIZE; j++ )
        {
            pSync[j] = _SHMEM_SYNC_VALUE;
        }
        shmem_barrier_all();

        /* Put value to peer */
        FUNC_VALUE(target_addr, source_addr, 1, 0, 0, num_proc, pWrk, pSync);

        /* Get value put by peer:
         * These routines start the remote transfer and may return before the data
         * is delivered to the remote PE
         */
        shmem_barrier_all();
        {
            int total_wait = 0;
            while (*target_addr == DEFAULT_VALUE && total_wait < 1000 * WAIT_COUNT)
            {
                total_wait++;
                usleep(1);
            }
            value = *target_addr;
        }

        rc = (expect_value == value ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my#%d source = %lld expected = %lld actual = %lld\n",
                           my_proc, (INT64_TYPE)source_value, (INT64_TYPE)expect_value, (INT64_TYPE)value);
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (source_addr)
    {
        shfree(source_addr);
    }

    if (pWrk)
    {
        shfree(pWrk);
        pWrk = NULL;
    }

    return rc;
}


static int test_item5(void)
{
    int rc = TC_PASS;
    TYPE_VALUE* target_addr = NULL;
    TYPE_VALUE* source_addr = NULL;
    TYPE_VALUE source_value = 0;
    TYPE_VALUE expect_value = 0;
    int my_proc = 0;

    my_proc = _my_pe();

    target_addr = (TYPE_VALUE*)shmalloc(sizeof(*target_addr) * __max_buffer_size);
    source_addr = (TYPE_VALUE*)shmalloc(sizeof(*source_addr) * __max_buffer_size);
    if (target_addr && source_addr)
    {
        TYPE_VALUE value = DEFAULT_VALUE;
        int i = 0;
        int j = 0;
        long cur_buf_size = 0;

        for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++)
        {
            cur_buf_size = sys_max(1, (i + 1) * __max_buffer_size / __cycle_count);
            pWrk = shmalloc(sizeof(*pWrk) * sys_max(cur_buf_size/2 + 1, _SHMEM_REDUCE_MIN_WRKDATA_SIZE));
            if (pWrk)
            {
                /* Set initial target value */
                value = DEFAULT_VALUE;
                fill_buffer((void *)target_addr, cur_buf_size, (void *)&value, sizeof(value));

                /* Give some time to all PE for setting their values */
                shmem_barrier_all();

                /* Set my value */
                source_value = (TYPE_VALUE)(BASE_VALUE + my_proc);
                fill_buffer((void *)source_addr, cur_buf_size, (void *)&source_value, sizeof(source_value));

                /* Define expected value */
                expect_value = source_value;

                /* This guarantees that PE set initial value before peer change one */
                for ( j = 0; j < _SHMEM_REDUCE_SYNC_SIZE; j++ )
                {
                    pSync[j] = _SHMEM_SYNC_VALUE;
                }
                shmem_barrier_all();

                /* Put value to peer */
                FUNC_VALUE(target_addr, source_addr, cur_buf_size, my_proc, 0, 1, pWrk, pSync);

                /* Get value put by peer:
                 * These routines start the remote transfer and may return before the data
                 * is delivered to the remote PE
                 */
                shmem_barrier_all();
                {
                    int wait = WAIT_COUNT;

                    while (wait--)
                    {
                        value = *target_addr;
                        if (expect_value == value) break;
                        sleep(1);
                    }
                }

                rc = (!compare_buffer_with_const(target_addr, cur_buf_size, &expect_value, sizeof(expect_value)) ? TC_PASS : TC_FAIL);

                log_debug(OSH_TC, "my#%d source = %lld expected = %lld actual = %lld buffer size = %lld\n",
                                   my_proc, (INT64_TYPE)source_value, (INT64_TYPE)expect_value, (INT64_TYPE)value, (INT64_TYPE)cur_buf_size);

                if (rc)
                {
                    TYPE_VALUE* check_addr = target_addr;
                    int odd_index = compare_buffer_with_const(check_addr, cur_buf_size, &expect_value, sizeof(expect_value));
                    int show_index = (odd_index > 1 ? odd_index - 2 : 0);
                    int show_size = sizeof(*check_addr) * sys_min(3, cur_buf_size - odd_index - 1);

                    log_debug(OSH_TC, "index of incorrect value: 0x%08X (%d)\n", odd_index - 1, odd_index - 1);
                    log_debug(OSH_TC, "buffer interval: 0x%08X - 0x%08X\n", show_index, show_index + show_size);
                    show_buffer(check_addr + show_index, show_size);
                }

                shfree(pWrk);
            } else {
                rc = TC_SETUP_FAIL;
            }
        }
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (source_addr)
    {
        shfree(source_addr);
    }

    if (target_addr)
    {
        shfree(target_addr);
    }

    return rc;
}


static int test_item6(void)
{
    int rc = TC_PASS;
    TYPE_VALUE* target_addr = NULL;
    TYPE_VALUE* source_addr = NULL;
    TYPE_VALUE source_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    target_addr = (TYPE_VALUE*)shmalloc(sizeof(*target_addr) * __max_buffer_size);
    source_addr = (TYPE_VALUE*)shmalloc(sizeof(*source_addr) * __max_buffer_size);
    if (target_addr && source_addr)
    {
        TYPE_VALUE value = DEFAULT_VALUE;
        int i = 0;
        int j = 0;
        long cur_buf_size = 0;

        for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++)
        {
            cur_buf_size = sys_max(1, (i + 1) * __max_buffer_size / __cycle_count);
            pWrk = shmalloc(sizeof(*pWrk) * sys_max(cur_buf_size/2 + 1, _SHMEM_REDUCE_MIN_WRKDATA_SIZE));
            if (pWrk)
            {
                /* Set initial target value */
                value = DEFAULT_VALUE;
                fill_buffer((void *)target_addr, cur_buf_size, (void *)&value, sizeof(value));

                /* Give some time to all PE for setting their values */
                shmem_barrier_all();

                /* Set my value */
                source_value = ( my_proc < OVERFLOW_FACTORIAL_LIMIT ? (TYPE_VALUE)(my_proc + 1) : 1);
                fill_buffer((void *)source_addr, cur_buf_size, (void *)&source_value, sizeof(source_value));

                /* Define expected value */
                expect_value = 1;
                {
                    int k = ( num_proc <= OVERFLOW_FACTORIAL_LIMIT ? num_proc : OVERFLOW_FACTORIAL_LIMIT);
                    while (k) expect_value *= k--;
                }

                /* This guarantees that PE set initial value before peer change one */
                for ( j = 0; j < _SHMEM_REDUCE_SYNC_SIZE; j++ )
                {
                    pSync[j] = _SHMEM_SYNC_VALUE;
                }
                shmem_barrier_all();

                /* Put value to peer */
                FUNC_VALUE(target_addr, source_addr, cur_buf_size, 0, 0, num_proc, pWrk, pSync);

                /* Get value put by peer:
                 * These routines start the remote transfer and may return before the data
                 * is delivered to the remote PE
                 */
                shmem_barrier_all();
                {
                    int wait = WAIT_COUNT;

                    while (wait--)
                    {
                        value = *target_addr;
                        if (expect_value == value) break;
                        sleep(1);
                    }
                }

                rc = (!compare_buffer_with_const(target_addr, cur_buf_size, &expect_value, sizeof(expect_value)) ? TC_PASS : TC_FAIL);

                log_debug(OSH_TC, "my#%d source = %lld expected = %lld actual = %lld buffer size = %lld\n",
                                   my_proc, (INT64_TYPE)source_value, (INT64_TYPE)expect_value, (INT64_TYPE)value, (INT64_TYPE)cur_buf_size);

                if (rc)
                {
                    TYPE_VALUE* check_addr = target_addr;
                    int odd_index = compare_buffer_with_const(check_addr, cur_buf_size, &expect_value, sizeof(expect_value));
                    int show_index = (odd_index > 1 ? odd_index - 2 : 0);
                    int show_size = sizeof(*check_addr) * sys_min(3, cur_buf_size - odd_index - 1);

                    log_debug(OSH_TC, "index of incorrect value: 0x%08X (%d)\n", odd_index - 1, odd_index - 1);
                    log_debug(OSH_TC, "buffer interval: 0x%08X - 0x%08X\n", show_index, show_index + show_size);
                    show_buffer(check_addr + show_index, show_size);
                }

                shfree(pWrk);
            } else {
                rc = TC_SETUP_FAIL;
            }
        }
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (source_addr)
    {
        shfree(source_addr);
    }

    if (target_addr)
    {
        shfree(target_addr);
    }

    return rc;
}


static int test_item7(void)
{
    int rc = TC_PASS;
    TYPE_VALUE* target_addr = NULL;
    TYPE_VALUE* source_addr = NULL;
    TYPE_VALUE source_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    target_addr = (TYPE_VALUE*)shmalloc(sizeof(*target_addr) * __max_buffer_size);
    source_addr = (TYPE_VALUE*)shmalloc(sizeof(*source_addr) * __max_buffer_size);
    if (target_addr && source_addr)
    {
        TYPE_VALUE value = DEFAULT_VALUE;
        int i = 0;
        int j = 0;
        long cur_buf_size = 0;

        for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++)
        {
            cur_buf_size = sys_max(1, (i + 1) * __max_buffer_size / __cycle_count);
            pWrk = shmalloc(sizeof(*pWrk) * sys_max(cur_buf_size/2 + 1, _SHMEM_REDUCE_MIN_WRKDATA_SIZE));
            if (pWrk)
            {
                /* Set initial target value */
                value = DEFAULT_VALUE;
                fill_buffer((void *)target_addr, cur_buf_size, (void *)&value, sizeof(value));

                /* Give some time to all PE for setting their values */
                shmem_barrier_all();

                /* Set my value */
                source_value = ( my_proc < OVERFLOW_FACTORIAL_LIMIT ? (TYPE_VALUE)(my_proc + 1) : 1);
                fill_buffer((void *)source_addr, cur_buf_size, (void *)&source_value, sizeof(source_value));

                /* Define expected value */
                expect_value = 1;
                if (my_proc % 2)    expect_value = DEFAULT_VALUE;
                else
                {
                    int k = ( num_proc <= OVERFLOW_FACTORIAL_LIMIT ? num_proc : OVERFLOW_FACTORIAL_LIMIT);
                    while (k)
                    {
                        if (k % 2)  expect_value *= k;
                        k--;
                    }
                }

                /* This guarantees that PE set initial value before peer change one */
                for ( j = 0; j < _SHMEM_REDUCE_SYNC_SIZE; j++ )
                {
                    pSync[j] = _SHMEM_SYNC_VALUE;
                }
                shmem_barrier_all();

                /* Put value to peer */
                FUNC_VALUE(target_addr, source_addr, cur_buf_size, 0, 1, ((num_proc / 2) + (num_proc % 2)), pWrk, pSync);

                /* Get value put by peer:
                 * These routines start the remote transfer and may return before the data
                 * is delivered to the remote PE
                 */
                shmem_barrier_all();
                {
                    int wait = WAIT_COUNT;

                    while (wait--)
                    {
                        value = *target_addr;
                        if (expect_value == value) break;
                        sleep(1);
                    }
                }

                rc = (!compare_buffer_with_const(target_addr, cur_buf_size, &expect_value, sizeof(expect_value)) ? TC_PASS : TC_FAIL);

                log_debug(OSH_TC, "my#%d source = %lld expected = %lld actual = %lld buffer size = %lld\n",
                                   my_proc, (INT64_TYPE)source_value, (INT64_TYPE)expect_value, (INT64_TYPE)value, (INT64_TYPE)cur_buf_size);

                if (rc)
                {
                    TYPE_VALUE* check_addr = target_addr;
                    int odd_index = compare_buffer_with_const(check_addr, cur_buf_size, &expect_value, sizeof(expect_value));
                    int show_index = (odd_index > 1 ? odd_index - 2 : 0);
                    int show_size = sizeof(*check_addr) * sys_min(3, cur_buf_size - odd_index - 1);

                    log_debug(OSH_TC, "index of incorrect value: 0x%08X (%d)\n", odd_index - 1, odd_index - 1);
                    log_debug(OSH_TC, "buffer interval: 0x%08X - 0x%08X\n", show_index, show_index + show_size);
                    show_buffer(check_addr + show_index, show_size);
                }

                shfree(pWrk);
            } else {
                rc = TC_SETUP_FAIL;
            }
        }
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (source_addr)
    {
        shfree(source_addr);
    }

    if (target_addr)
    {
        shfree(target_addr);
    }

    return rc;
}


static int test_item8(void)
{
    int rc = TC_PASS;
    static TYPE_VALUE target_addr[MAX_BUFFER_SIZE * 2];
    static TYPE_VALUE source_addr[MAX_BUFFER_SIZE * 2];
    TYPE_VALUE source_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    long* pSyncMult = NULL;
    TYPE_VALUE* pWrkMult = NULL;
    int pSyncNum = 2;
    int pWrkNum = 2;

    num_proc = _num_pes();
    my_proc = _my_pe();

    pSyncMult = shmalloc(sizeof(*pSyncMult) * pSyncNum * _SHMEM_REDUCE_SYNC_SIZE);
    if (pSyncMult)
    {
        TYPE_VALUE value = DEFAULT_VALUE;
        int i = 0;
        int j = 0;
        long cur_buf_size = 0;

        for ( j = 0; j < pSyncNum * _SHMEM_REDUCE_SYNC_SIZE; j++ )
        {
            pSyncMult[j] = _SHMEM_SYNC_VALUE;
        }

        /* Give some time to all PE for setting their values */
        shmem_barrier_all();

        pWrkMult = shmalloc(sizeof(*pWrkMult) * pWrkNum * sys_max(MAX_BUFFER_SIZE, _SHMEM_REDUCE_MIN_WRKDATA_SIZE));
        if (pWrkMult)
        {
            value = DEFAULT_VALUE;
            source_value = ( my_proc < OVERFLOW_FACTORIAL_LIMIT ? (TYPE_VALUE)(my_proc + 1) : 1);
            fill_buffer((void *)source_addr, MAX_BUFFER_SIZE * 2, (void *)&source_value, sizeof(source_value));
            fill_buffer((void *)target_addr, MAX_BUFFER_SIZE * 2, (void *)&value, sizeof(value));
            shmem_barrier_all();
            for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++)
            {
                cur_buf_size = sys_max(1, (i + 1) * MAX_BUFFER_SIZE / __cycle_count);
                /* Set initial target value */
                value = DEFAULT_VALUE;

                /* Set my value */
                source_value = ( my_proc < OVERFLOW_FACTORIAL_LIMIT ? (TYPE_VALUE)(my_proc + 1) : 1);

                /* Define expected value */
                expect_value = 1;
                if (my_proc % 2)    expect_value = DEFAULT_VALUE;
                else
                {
                    int k = ( num_proc <= OVERFLOW_FACTORIAL_LIMIT ? num_proc : OVERFLOW_FACTORIAL_LIMIT);
                    while (k)
                    {
                        if (k % 2)  expect_value *= k;
                        k--;
                    }
                }

                /* Put value to peer */
                FUNC_VALUE(target_addr + (i % 2) * MAX_BUFFER_SIZE, source_addr + (i % 2) * MAX_BUFFER_SIZE, cur_buf_size, 0, 1, ((num_proc / 2) + (num_proc % 2)), pWrkMult + (i % pWrkNum) * sys_max(MAX_BUFFER_SIZE, _SHMEM_REDUCE_MIN_WRKDATA_SIZE),  pSyncMult + (i % pSyncNum) * _SHMEM_REDUCE_SYNC_SIZE);
                rc = (!compare_buffer_with_const(target_addr + (i % 2) * MAX_BUFFER_SIZE, cur_buf_size, &expect_value, sizeof(expect_value)) ? TC_PASS : TC_FAIL);

                log_debug(OSH_TC, "my#%d source = %lld expected = %lld actual = %lld buffer size = %lld\n",
                                   my_proc, (INT64_TYPE)source_value, (INT64_TYPE)expect_value, (INT64_TYPE)value, (INT64_TYPE)cur_buf_size);

                if (rc)
                {
                    TYPE_VALUE* check_addr = target_addr + (i % 2) * MAX_BUFFER_SIZE;
                    int odd_index = compare_buffer_with_const(check_addr, cur_buf_size, &expect_value, sizeof(expect_value));
                    int show_index = (odd_index > 1 ? odd_index - 2 : 0);
                    int show_size = sizeof(*check_addr) * sys_min(3, cur_buf_size - odd_index - 1);

                    log_debug(OSH_TC, "index of incorrect value: 0x%08X (%d)\n", odd_index - 1, odd_index - 1);
                    log_debug(OSH_TC, "buffer interval: 0x%08X - 0x%08X\n", show_index, show_index + show_size);
                    show_buffer(check_addr + show_index, show_size);
                }
                fill_buffer((void *)(source_addr + (i % 2) * MAX_BUFFER_SIZE), cur_buf_size, (void *)&source_value, sizeof(source_value));
                fill_buffer((void *)(target_addr + (i % 2) * MAX_BUFFER_SIZE ), cur_buf_size, (void *)&value, sizeof(value));
            }
            shfree(pWrkMult);
        } else {
            rc = TC_SETUP_FAIL;
        }
        shfree(pSyncMult);
    } else {
        rc = TC_SETUP_FAIL;
    }

    return rc;
}


static int __parse_opt( const TE_NODE *node, int argc, const char *argv[] )
{
    OSH_ERROR status = OSH_ERR_NONE;
	const AOPT_OBJECT* self_opt_obj = NULL;

	/* Load supported option and create option objects */
	{
		int temp_argc = 0;

		temp_argc = argc;
		self_opt_obj = aopt_init(&temp_argc, (const char **)argv, self_opt_desc);
	}

    /* Parse specific options */
	if (!status && self_opt_obj)
    {
        if ( !status && aopt_check(self_opt_obj, 'm') )
        {
            const char* optarg = aopt_value(self_opt_obj, 'm');
            if (optarg)
            {
                errno = 0;
                long value = sys_strtol(optarg, NULL, 0);
                if ( !errno && (value > 0) )
                {
                    __max_buffer_size = (int)value;
                }
                else
                {
                    status = OSH_ERR_BAD_ARGUMENT;
                }
            }
            else
            {
                status = OSH_ERR_BAD_ARGUMENT;
            }
        }

        if ( !status && aopt_check(self_opt_obj, 'c') )
        {
            const char* optarg = aopt_value(self_opt_obj, 'c');
            if (optarg)
            {
                errno = 0;
                long value = sys_strtol(optarg, NULL, 0);
                if ( !errno && (value > 0) )
                {
                    __cycle_count = (int)value;
                }
                else
                {
                    status = OSH_ERR_BAD_ARGUMENT;
                }
            }
            else
            {
                status = OSH_ERR_BAD_ARGUMENT;
            }
        }
	}

    if (status)
    {
        /* Display help information */
        const char* help_str = NULL;
        char temp_buf[30];

        log_help("%s: %s\n", display_opt(node, temp_buf, sizeof(temp_buf)), node->note);
        log_help("\n");
        log_help("Options:\n");
        help_str = aopt_help(self_opt_desc);
        if (help_str)
        {
            log_help("%s\n", help_str);
            sys_free((void*)help_str);
        }
    }

    aopt_exit((AOPT_OBJECT*)self_opt_obj);

    return status;
}
