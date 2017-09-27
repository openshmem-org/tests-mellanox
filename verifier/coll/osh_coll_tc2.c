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

#include "osh_coll_tests.h"

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


#define WAIT_COUNT  5
#define TYPE_VALUE  int64_t
#define FUNC_VALUE  shmem_broadcast64
#define SIZE_VALUE  sizeof(TYPE_VALUE)
#define MAX_VALUE   (((unsigned long long)1 << ( 8 * SIZE_VALUE - 1)) - 1)
#define BASE_VALUE  8
#define DEFAULT_VALUE  (-1)


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
int osh_coll_tc2(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    rc = __parse_opt(node, argc, argv);

    if (rc == TC_PASS)
    {
        pSync = shmalloc(sizeof(*pSync) * _SHMEM_COLLECT_SYNC_SIZE);
        if (!pSync)
        {
            rc = TC_SETUP_FAIL;
        }
    }

    /* Every PE put the single value as symmetric data object to itself
     * The data is not copied to the target address on the PE specified by PE_root.
     */
    if (rc == TC_PASS)
    {
        rc = test_item1();
        log_item(node, 1, rc);
        shmem_barrier_all();
    }

    /* PE#0 put defined single value to all */
    if (rc == TC_PASS)
    {
        rc = test_item2();
        log_item(node, 2, rc);
        shmem_barrier_all();
    }

    /* Random PE put defined single value to all */
    if (rc == TC_PASS)
    {
        rc = test_item3();
        log_item(node, 3, rc);
        shmem_barrier_all();
    }

    /* PE#0 put defined single value to even PEs */
    if (rc == TC_PASS)
    {
        rc = test_item4();
        log_item(node, 4, rc);
        shmem_barrier_all();
    }

    /* PE#0 put dynamic buffer to even PEs */
    if (rc == TC_PASS)
    {
        rc = test_item5();
        log_item(node, 5, rc);
        shmem_barrier_all();
    }

    /* PE#0 put static buffer to even PEs */
    if (rc == TC_PASS)
    {
        rc = test_item6();
        log_item(node, 6, rc);
        shmem_barrier_all();
    }

    /* broadcast calls in loop with alternating multiple pSync arrays (without barrrier synchronization between iterations) */
    if (rc == TC_PASS)
    {
        rc = test_item7();
        log_item(node, 7, rc);
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
    TYPE_VALUE* shmem_addr = NULL;
    TYPE_VALUE* send_addr = NULL;
    TYPE_VALUE my_value = 0;
    TYPE_VALUE peer_value = 0;
    TYPE_VALUE expect_value = 0;
    int my_proc = 0;
    int root_proc = 0;

    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    send_addr = shmalloc(sizeof(*send_addr));
    if (shmem_addr && send_addr)
    {
        TYPE_VALUE value = DEFAULT_VALUE;
        int j = 0;

        /* Set my value */
        my_value = DEFAULT_VALUE;
        *shmem_addr = my_value;

        /* Define peer and it value */
        peer_value = BASE_VALUE;
        *send_addr = peer_value;

        /* Set root */
        root_proc = my_proc;

        /* Define expected value */
        expect_value = DEFAULT_VALUE;

        /* This guarantees that PE set initial value before peer change one */
        for ( j = 0; j < _SHMEM_COLLECT_SYNC_SIZE; j++ )
        {
            pSync[j] = _SHMEM_SYNC_VALUE;
        }
        shmem_barrier_all();

        /* Put value to peer */
        FUNC_VALUE(shmem_addr, send_addr, 1, root_proc, root_proc, 0, 1, pSync);

        /* Get value put by peer:
         * These routines start the remote transfer and may return before the data
         * is delivered to the remote PE
         */
        shmem_barrier_all();
        {
            int wait = WAIT_COUNT;

            while (wait--)
            {
                sleep(1);
            }
            value = *shmem_addr;
        }

        rc = (expect_value == value ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my#%d root(#%d:%lld) expected = %lld actual = %lld\n",
                           my_proc, root_proc, (INT64_TYPE)peer_value, (INT64_TYPE)expect_value, (INT64_TYPE)value);
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (send_addr)
    {
        shfree(send_addr);
    }

    if (shmem_addr)
    {
        shfree(shmem_addr);
    }

    return rc;
}


static int test_item2(void)
{
    int rc = TC_PASS;
    TYPE_VALUE* shmem_addr = NULL;
    TYPE_VALUE* send_addr = NULL;
    TYPE_VALUE my_value = 0;
    TYPE_VALUE peer_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    int root_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    send_addr = shmalloc(sizeof(*send_addr));
    if (shmem_addr && send_addr)
    {
        TYPE_VALUE value = DEFAULT_VALUE;
        int j = 0;

        /* Set my value */
        my_value = DEFAULT_VALUE;
        *shmem_addr = my_value;

        /* Define peer and it value */
        peer_value = BASE_VALUE;
        *send_addr = peer_value;

        /* Set root */
        root_proc = 0;

        /* Define expected value */
        expect_value = (my_proc == root_proc ? DEFAULT_VALUE : BASE_VALUE);

        /* This guarantees that PE set initial value before peer change one */
        for ( j = 0; j < _SHMEM_COLLECT_SYNC_SIZE; j++ )
        {
            pSync[j] = _SHMEM_SYNC_VALUE;
        }
        shmem_barrier_all();

        /* Put value to peer */
        FUNC_VALUE(shmem_addr, send_addr, 1, root_proc, 0, 0, num_proc, pSync);

        /* Get value put by peer:
         * These routines start the remote transfer and may return before the data
         * is delivered to the remote PE
         */
        shmem_barrier_all();
        {
            int wait = WAIT_COUNT;

            while (wait--)
            {
                value = *shmem_addr;
                if (expect_value == value) break;
                sleep(1);
            }
        }

        rc = (expect_value == value ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my#%d root(#%d:%lld) expected = %lld actual = %lld\n",
                           my_proc, root_proc, (INT64_TYPE)peer_value, (INT64_TYPE)expect_value, (INT64_TYPE)value);
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (send_addr)
    {
        shfree(send_addr);
    }

    if (shmem_addr)
    {
        shfree(shmem_addr);
    }

    return rc;
}


static int test_item3(void)
{
    int rc = TC_PASS;
    TYPE_VALUE* shmem_addr = NULL;
    TYPE_VALUE* send_addr = NULL;
    TYPE_VALUE my_value = 0;
    TYPE_VALUE peer_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    int root_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    send_addr = shmalloc(sizeof(*send_addr));
    if (shmem_addr && send_addr)
    {
        TYPE_VALUE value = DEFAULT_VALUE;
        int j = 0;

        /* Set my value */
        my_value = DEFAULT_VALUE;
        *shmem_addr = my_value;

        /* Define peer and it value */
        peer_value = BASE_VALUE;
        *send_addr = peer_value;

        /* Set root */
        root_proc = num_proc / 2;

        /* Define expected value */
        expect_value = (my_proc == root_proc ? DEFAULT_VALUE : BASE_VALUE);

        /* This guarantees that PE set initial value before peer change one */
        for ( j = 0; j < _SHMEM_COLLECT_SYNC_SIZE; j++ )
        {
            pSync[j] = _SHMEM_SYNC_VALUE;
        }
        shmem_barrier_all();

        /* Put value to peer */
        FUNC_VALUE(shmem_addr, send_addr, 1, root_proc, 0, 0, num_proc, pSync);

        /* Get value put by peer:
         * These routines start the remote transfer and may return before the data
         * is delivered to the remote PE
         */
        shmem_barrier_all();
        {
            int wait = WAIT_COUNT;

            while (wait--)
            {
                value = *shmem_addr;
                if (expect_value == value) break;
                sleep(1);
            }
        }

        rc = (expect_value == value ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my#%d root(#%d:%lld) expected = %lld actual = %lld\n",
                           my_proc, root_proc, (INT64_TYPE)peer_value, (INT64_TYPE)expect_value, (INT64_TYPE)value);
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (send_addr)
    {
        shfree(send_addr);
    }

    if (shmem_addr)
    {
        shfree(shmem_addr);
    }

    return rc;
}


static int test_item4(void)
{
    int rc = TC_PASS;
    TYPE_VALUE* shmem_addr = NULL;
    TYPE_VALUE* send_addr = NULL;
    TYPE_VALUE my_value = 0;
    TYPE_VALUE peer_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    int root_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    send_addr = shmalloc(sizeof(*send_addr));
    if (shmem_addr && send_addr)
    {
        TYPE_VALUE value = DEFAULT_VALUE;
        int j = 0;

        /* Set my value */
        my_value = DEFAULT_VALUE;
        *shmem_addr = my_value;

        /* Define peer and it value */
        peer_value = BASE_VALUE;
        *send_addr = peer_value;

        /* Set root */
        root_proc = 0;

        /* Define expected value */
        expect_value = (((my_proc % 2) == 0) && (my_proc != 0) ? BASE_VALUE : DEFAULT_VALUE);

        /* This guarantees that PE set initial value before peer change one */
        for ( j = 0; j < _SHMEM_COLLECT_SYNC_SIZE; j++ )
        {
            pSync[j] = _SHMEM_SYNC_VALUE;
        }
        shmem_barrier_all();

        /* Put value to peer */
        if ((my_proc % 2) == 0)
        {
            FUNC_VALUE(shmem_addr, send_addr, 1, root_proc, 0, 1, ((num_proc / 2) + (num_proc % 2)), pSync);
        }

        /* Get value put by peer:
         * These routines start the remote transfer and may return before the data
         * is delivered to the remote PE
         */
        shmem_barrier_all();
        {
            int wait = WAIT_COUNT;

            while (wait--)
            {
                value = *shmem_addr;
                if (expect_value == value) break;
                sleep(1);
            }
        }

        rc = (expect_value == value ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my#%d root(#%d:%lld) expected = %lld actual = %lld\n",
                           my_proc, root_proc, (INT64_TYPE)peer_value, (INT64_TYPE)expect_value, (INT64_TYPE)value);
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (send_addr)
    {
        shfree(send_addr);
    }

    if (shmem_addr)
    {
        shfree(shmem_addr);
    }

    return rc;
}


static int test_item5(void)
{
    int rc = TC_PASS;
    TYPE_VALUE* shmem_addr = NULL;
    TYPE_VALUE* send_addr = NULL;
    TYPE_VALUE my_value = 0;
    TYPE_VALUE peer_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    int root_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = (TYPE_VALUE*)shmalloc(sizeof(*shmem_addr) * __max_buffer_size);
    send_addr = (TYPE_VALUE*)shmalloc(sizeof(*send_addr) * __max_buffer_size);
    if (shmem_addr && send_addr)
    {
        TYPE_VALUE value = DEFAULT_VALUE;
        int i = 0;
        int j = 0;
        long cur_buf_size = 0;

        for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++)
        {
            cur_buf_size = sys_max(1, (i + 1) * __max_buffer_size / __cycle_count);

            /* Set my value */
            my_value = DEFAULT_VALUE;
            fill_buffer((void *)shmem_addr, cur_buf_size, (void *)&my_value, sizeof(my_value));

            /* Give some time to all PE for setting their values */
            shmem_barrier_all();

            /* Define peer and it value */
            peer_value = (i * (MAX_VALUE / __cycle_count));
            fill_buffer((void *)send_addr, cur_buf_size, (void *)&peer_value, sizeof(peer_value));

            /* Set root */
            root_proc = 0;

            /* Define expected value */
            expect_value = (((my_proc % 2) == 0) && (my_proc != root_proc) ? peer_value : DEFAULT_VALUE);

            /* This guarantees that PE set initial value before peer change one */
            for ( j = 0; j < _SHMEM_COLLECT_SYNC_SIZE; j++ )
            {
                pSync[j] = _SHMEM_SYNC_VALUE;
            }
            shmem_barrier_all();

            /* Put value to peer */
            if ((my_proc % 2) == 0)
            {
                FUNC_VALUE(shmem_addr, send_addr, cur_buf_size, root_proc, 0, 1, ((num_proc / 2) + (num_proc % 2)), pSync);
            }

            /* Get value put by peer:
             * These routines start the remote transfer and may return before the data
             * is delivered to the remote PE
             */
            shmem_barrier_all();
            {
                int wait = WAIT_COUNT;

                while (wait--)
                {
                    value = *shmem_addr;
                    if (expect_value == value) break;
                    sleep(1);
                }
            }

            rc = (!compare_buffer_with_const(shmem_addr, cur_buf_size, &expect_value, sizeof(expect_value)) ? TC_PASS : TC_FAIL);

            log_debug(OSH_TC, "my#%d root(#%d:%lld) expected = %lld actual = %lld buffer size = %lld\n",
                               my_proc, root_proc, (INT64_TYPE)peer_value, (INT64_TYPE)expect_value, (INT64_TYPE)value, (INT64_TYPE)cur_buf_size);

            if (rc)
            {
                TYPE_VALUE* check_addr = shmem_addr;
                int odd_index = compare_buffer_with_const(check_addr, cur_buf_size, &expect_value, sizeof(expect_value));
                int show_index = (odd_index > 1 ? odd_index - 2 : 0);
                int show_size = sizeof(*check_addr) * sys_min(3, cur_buf_size - show_index);

                log_debug(OSH_TC, "index of incorrect value: 0x%08X (%d)\n", odd_index - 1, odd_index - 1);
                log_debug(OSH_TC, "buffer interval: 0x%08X - 0x%08X\n", show_index, show_index + show_size);
                show_buffer(check_addr + show_index, show_size);
            }
        }
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (send_addr)
    {
        shfree(send_addr);
    }

    if (shmem_addr)
    {
        shfree(shmem_addr);
    }

    return rc;
}


static int test_item6(void)
{
    int rc = TC_PASS;
    static TYPE_VALUE shmem_addr[MAX_BUFFER_SIZE];
    static TYPE_VALUE send_addr[MAX_BUFFER_SIZE];
    TYPE_VALUE my_value = 0;
    TYPE_VALUE peer_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    int root_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    {
        TYPE_VALUE value = DEFAULT_VALUE;
        int i = 0;
        int j = 0;
        long cur_buf_size = 0;

        for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++)
        {
            cur_buf_size = sys_max(1, (i + 1) * MAX_BUFFER_SIZE / __cycle_count);

            /* Set my value */
            my_value = DEFAULT_VALUE;
            fill_buffer((void *)shmem_addr, cur_buf_size, (void *)&my_value, sizeof(my_value));

            /* Give some time to all PE for setting their values */
            shmem_barrier_all();

            /* Define peer and it value */
            peer_value = (i * (MAX_VALUE / __cycle_count));
            fill_buffer((void *)send_addr, cur_buf_size, (void *)&peer_value, sizeof(peer_value));

            /* Set root */
            root_proc = 0;

            /* Define expected value */
            expect_value = (((my_proc % 2) == 0) && (my_proc != root_proc) ? peer_value : DEFAULT_VALUE);

            /* This guarantees that PE set initial value before peer change one */
            for ( j = 0; j < _SHMEM_COLLECT_SYNC_SIZE; j++ )
            {
                pSync[j] = _SHMEM_SYNC_VALUE;
            }
            shmem_barrier_all();

            /* Put value to peer */
            if ((my_proc % 2) == 0)
            {
                FUNC_VALUE(shmem_addr, send_addr, cur_buf_size, root_proc, 0, 1, ((num_proc / 2) + (num_proc % 2)), pSync);
            }

            /* Get value put by peer:
             * These routines start the remote transfer and may return before the data
             * is delivered to the remote PE
             */
            shmem_barrier_all();
            {
                int wait = WAIT_COUNT;

                while (wait--)
                {
                    value = *shmem_addr;
                    if (expect_value == value) break;
                    sleep(1);
                }
            }

            rc = (!compare_buffer_with_const(shmem_addr, cur_buf_size, &expect_value, sizeof(expect_value)) ? TC_PASS : TC_FAIL);

            log_debug(OSH_TC, "my#%d root(#%d:%lld) expected = %lld actual = %lld buffer size = %lld\n",
                               my_proc, root_proc, (INT64_TYPE)peer_value, (INT64_TYPE)expect_value, (INT64_TYPE)value, (INT64_TYPE)cur_buf_size);

            if (rc)
            {
                TYPE_VALUE* check_addr = shmem_addr;
                int odd_index = compare_buffer_with_const(check_addr, cur_buf_size, &expect_value, sizeof(expect_value));
                int show_index = (odd_index > 1 ? odd_index - 2 : 0);
                int show_size = sizeof(*check_addr) * sys_min(3, cur_buf_size - show_index);

                log_debug(OSH_TC, "index of incorrect value: 0x%08X (%d)\n", odd_index - 1, odd_index - 1);
                log_debug(OSH_TC, "buffer interval: 0x%08X - 0x%08X\n", show_index, show_index + show_size);
                show_buffer(check_addr + show_index, show_size);
            }
        }
    }

    return rc;
}


static int test_item7(void)
{
    int rc = TC_PASS;
    static TYPE_VALUE shmem_addr[MAX_BUFFER_SIZE * 2];
    static TYPE_VALUE send_addr[MAX_BUFFER_SIZE * 2];
    TYPE_VALUE my_value = 0;
    TYPE_VALUE peer_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    int root_proc = 0;
    long* pSyncMult = NULL;
    int pSyncNum = 2;

    num_proc = _num_pes();
    my_proc = _my_pe();

    pSyncMult = shmalloc(sizeof(*pSyncMult) * pSyncNum * _SHMEM_COLLECT_SYNC_SIZE);
    if (!pSyncMult)
    {
        rc = TC_SETUP_FAIL;
    }

    if (rc == TC_PASS)
    {
        int i = 0;
        int j = 0;

        for ( j = 0; j < pSyncNum * _SHMEM_COLLECT_SYNC_SIZE; j++ )
        {
            pSyncMult[j] = _SHMEM_SYNC_VALUE;
        }

        /* Give some time to all PE for setting their values */
        shmem_barrier_all();

        /* Set root */
        root_proc = 0;

        my_value = DEFAULT_VALUE;
        peer_value = MAX_VALUE;
        expect_value = (my_proc == root_proc ? DEFAULT_VALUE : peer_value);

        fill_buffer((void *)send_addr, MAX_BUFFER_SIZE * 2, (void *)&peer_value, sizeof(peer_value));
        fill_buffer((void *)shmem_addr, MAX_BUFFER_SIZE * 2, (void *)&my_value, sizeof(my_value));
        shmem_barrier_all();
        for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++)
        {
            /* Put value to peer */
            FUNC_VALUE(shmem_addr + (i % 2) * MAX_BUFFER_SIZE, send_addr + (i % 2) * MAX_BUFFER_SIZE, MAX_BUFFER_SIZE, root_proc, 0, 0, num_proc, pSyncMult + (i % pSyncNum) * _SHMEM_COLLECT_SYNC_SIZE);
            rc = (!compare_buffer_with_const(shmem_addr + (i % 2) * MAX_BUFFER_SIZE, MAX_BUFFER_SIZE, &expect_value, sizeof(expect_value)) ? TC_PASS : TC_FAIL);

            log_debug(OSH_TC, "my#%d root(#%d:%lld) expected = %lld actual = %lld buffer size = %lld\n",
                               my_proc, root_proc, (INT64_TYPE)peer_value, (INT64_TYPE)expect_value, (INT64_TYPE)(*shmem_addr), (INT64_TYPE)MAX_BUFFER_SIZE);

            if (rc)
            {
                TYPE_VALUE* check_addr = shmem_addr + (i % 2) * MAX_BUFFER_SIZE;
                int odd_index = compare_buffer_with_const(check_addr, MAX_BUFFER_SIZE, &expect_value, sizeof(expect_value));
                int show_index = (odd_index > 1 ? odd_index - 2 : 0);
                int show_size = sizeof(*check_addr) * sys_min(3, MAX_BUFFER_SIZE - show_index);

                log_debug(OSH_TC, "index of incorrect value: 0x%08X (%d)\n", odd_index - 1, odd_index - 1);
                log_debug(OSH_TC, "buffer interval: 0x%08X - 0x%08X\n", show_index, show_index + show_size);
                show_buffer(check_addr + show_index, show_size);
            }
                fill_buffer((void *)(send_addr + (i % 2) * MAX_BUFFER_SIZE), MAX_BUFFER_SIZE, (void *)&peer_value, sizeof(peer_value));
                fill_buffer((void *)(shmem_addr + (i % 2) * MAX_BUFFER_SIZE ), MAX_BUFFER_SIZE, (void *)&my_value, sizeof(my_value));
        }
    }

    if (pSyncMult)
    {
        shfree(pSyncMult);
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
