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

#include "osh_data_tests.h"

/****************************************************************************
 * Test Case can consitis of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
static int test_item1(void);
static int test_item2(void);
static int test_item3(void);
static int test_item4(void);
static int test_item5(void);


#define WAIT_COUNT  5
#define TYPE_VALUE  long long
#define FUNC_VALUE  shmem_longlong_get
#define SIZE_VALUE  sizeof(TYPE_VALUE)
#define MAX_VALUE   (((unsigned long long)1 << ( 8 * SIZE_VALUE - 1)) - 1)


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
int osh_data_tc21(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    rc = __parse_opt(node, argc, argv);

    /* Get random the single value from itself */
    if (rc == TC_PASS)
    {
        rc = test_item1();
        log_item(node, 1, rc);
        shmem_barrier_all();
    }

    /* Get defined single value from peer */
    if (rc == TC_PASS)
    {
        rc = test_item2();
        log_item(node, 2, rc);
        shmem_barrier_all();
    }

    /* Get buffer from itself */
    if (rc == TC_PASS)
    {
        rc = test_item3();
        log_item(node, 3, rc);
        shmem_barrier_all();
    }

    /* Get dynamic buffer from peer */
    if (rc == TC_PASS)
    {
        rc = test_item4();
        log_item(node, 4, rc);
        shmem_barrier_all();
    }

    /* Get static buffer from peer */
    if (rc == TC_PASS)
    {
        rc = test_item5();
        log_item(node, 5, rc);
        shmem_barrier_all();
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
    TYPE_VALUE my_value = 0;
    TYPE_VALUE peer_value = 0;
    TYPE_VALUE expect_value = 0;
    int my_proc = 0;
    int peer_proc = 0;

    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    if (shmem_addr)
    {
        TYPE_VALUE value = -1;

        /* Set my value */
        my_value = (TYPE_VALUE)(((double)rand() / (double)RAND_MAX) * MAX_VALUE);
        *shmem_addr = my_value;

        /* Define peer and it value */
        peer_proc = my_proc;
        peer_value = my_value;

        /* Define expected value */
        expect_value = peer_value;

        /* Wait is set instead of barrier to give some time to all PE for setting their values */
        shmem_barrier_all();

        /* Get value from peer */
        FUNC_VALUE(&value, shmem_addr, 1, peer_proc);

        rc = (expect_value == value ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my(#%d:%lld) peer(#%d:%lld) expected = %lld buffer size = %lld\n",
                           my_proc, (INT64_TYPE)my_value, peer_proc, (INT64_TYPE)peer_value, (INT64_TYPE)expect_value, (INT64_TYPE)1);
    }
    else
    {
        rc = TC_SETUP_FAIL;
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
    TYPE_VALUE my_value = 0;
    TYPE_VALUE peer_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    if (shmem_addr)
    {
        TYPE_VALUE value = -1;

        /* Set my value */
        my_value = (TYPE_VALUE)my_proc;
        *shmem_addr = my_value;

        /* Define peer and it value */
        peer_proc = (my_proc + 1) % num_proc;
        peer_value = (TYPE_VALUE)peer_proc;

        /* Define expected value */
        expect_value = peer_value;

        /* Wait is set instead of barrier to give some time to all PE for setting their values */
        shmem_barrier_all();

        /* Get value from peer */
        FUNC_VALUE(&value, shmem_addr, 1, peer_proc);

        rc = (expect_value == value ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my(#%d:%lld) peer(#%d:%lld) expected = %lld buffer size = %lld\n",
                           my_proc, (INT64_TYPE)my_value, peer_proc, (INT64_TYPE)peer_value, (INT64_TYPE)expect_value, (INT64_TYPE)1);
    }
    else
    {
        rc = TC_SETUP_FAIL;
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
    TYPE_VALUE* recv_addr = NULL;
    TYPE_VALUE my_value = 0;
    TYPE_VALUE peer_value = 0;
    TYPE_VALUE expect_value = 0;
    int my_proc = 0;
    int peer_proc = 0;

    my_proc = _my_pe();

    shmem_addr = (TYPE_VALUE*)shmalloc(sizeof(*shmem_addr) * __max_buffer_size);
    recv_addr = (TYPE_VALUE*)sys_malloc(sizeof(*recv_addr) * __max_buffer_size);
    if (shmem_addr && recv_addr)
    {
        INT64_TYPE i = 0;
        long cur_buf_size = 0;

        my_value = 0;
        for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++)
        {
            /* Set my value */
            my_value = (my_proc % 2 ? 1 : -1) * (i * (MAX_VALUE / __cycle_count));
            cur_buf_size = sys_max(1, (i + 1) * __max_buffer_size / __cycle_count);
            fill_buffer((void *)shmem_addr, cur_buf_size, (void *)&my_value, sizeof(my_value));

            /* Give some time to all PE for setting their values */
            shmem_barrier_all();

            /* Define peer and it value */
            peer_proc = my_proc;
            peer_value = my_value;

            /* Define expected value */
            expect_value = peer_value;

            /* Get value from peer */
            FUNC_VALUE(recv_addr, shmem_addr, cur_buf_size, peer_proc);

            rc = (!compare_buffer_with_const(recv_addr, cur_buf_size, &expect_value, sizeof(expect_value)) ? TC_PASS : TC_FAIL);

            log_debug(OSH_TC, "my(#%d:%lld) peer(#%d:%lld) expected = %lld buffer size = %lld\n",
                               my_proc, (INT64_TYPE)my_value, peer_proc, (INT64_TYPE)peer_value, (INT64_TYPE)expect_value, (INT64_TYPE)cur_buf_size);

            if (rc)
            {
                TYPE_VALUE* check_addr = recv_addr;
                int odd_index = compare_buffer_with_const(check_addr, cur_buf_size, &expect_value, sizeof(expect_value));
                int show_index = (odd_index > 1 ? odd_index - 2 : 0);
                int show_size = sizeof(*check_addr) * sys_min(3, cur_buf_size - odd_index - 1);

                log_debug(OSH_TC, "index of incorrect value: 0x%08X (%d)\n", odd_index - 1, odd_index - 1);
                log_debug(OSH_TC, "buffer interval: 0x%08X - 0x%08X\n", show_index, show_index + show_size);
                show_buffer(check_addr + show_index, show_size);
            }

            shmem_barrier_all();
        }
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (recv_addr)
    {
        sys_free(recv_addr);
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
    TYPE_VALUE* recv_addr = NULL;
    TYPE_VALUE my_value = 0;
    TYPE_VALUE peer_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = (TYPE_VALUE*)shmalloc(sizeof(*shmem_addr) * __max_buffer_size);
    recv_addr = (TYPE_VALUE*)sys_malloc(sizeof(*recv_addr) * __max_buffer_size);
    if (shmem_addr && recv_addr)
    {
        INT64_TYPE i = 0;
        long cur_buf_size = 0;

        my_value = 0;
        for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++)
        {
            /* Set my value */
            my_value = (my_proc % 2 ? 1 : -1) * (i * (MAX_VALUE / __cycle_count));
            cur_buf_size = sys_max(1, (i + 1) * __max_buffer_size / __cycle_count);
            fill_buffer((void *)shmem_addr, cur_buf_size, (void *)&my_value, sizeof(my_value));

            /* Give some time to all PE for setting their values */
            shmem_barrier_all();

            /* Define peer and it value */
            peer_proc = (my_proc + 1) % num_proc;
            peer_value = (peer_proc % 2 ? 1 : -1) * (i * (MAX_VALUE / __cycle_count));

            /* Define expected value */
            expect_value = peer_value;

            /* Get value from peer */
            FUNC_VALUE(recv_addr, shmem_addr, cur_buf_size, peer_proc);

            rc = (!compare_buffer_with_const(recv_addr, cur_buf_size, &expect_value, sizeof(expect_value)) ? TC_PASS : TC_FAIL);

            log_debug(OSH_TC, "my(#%d:%lld) peer(#%d:%lld) expected = %lld buffer size = %lld\n",
                               my_proc, (INT64_TYPE)my_value, peer_proc, (INT64_TYPE)peer_value, (INT64_TYPE)expect_value, (INT64_TYPE)cur_buf_size);

            if (rc)
            {
                TYPE_VALUE* check_addr = recv_addr;
                int odd_index = compare_buffer_with_const(check_addr, cur_buf_size, &expect_value, sizeof(expect_value));
                int show_index = (odd_index > 1 ? odd_index - 2 : 0);
                int show_size = sizeof(*check_addr) * sys_min(3, cur_buf_size - odd_index - 1);

                log_debug(OSH_TC, "index of incorrect value: 0x%08X (%d)\n", odd_index - 1, odd_index - 1);
                log_debug(OSH_TC, "buffer interval: 0x%08X - 0x%08X\n", show_index, show_index + show_size);
                show_buffer(check_addr + show_index, show_size);
            }

            shmem_barrier_all();
        }
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (recv_addr)
    {
        sys_free(recv_addr);
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
    static TYPE_VALUE shmem_addr[MAX_BUFFER_SIZE];
    static TYPE_VALUE recv_addr[MAX_BUFFER_SIZE];
    TYPE_VALUE my_value = 0;
    TYPE_VALUE peer_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    {
        INT64_TYPE i = 0;
        long cur_buf_size = 0;

        my_value = 0;
        for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++)
        {
            /* Set my value */
            my_value = (my_proc % 2 ? 1 : -1) * (i * (MAX_VALUE / __cycle_count));
            cur_buf_size = sys_max(1, (i + 1) * MAX_BUFFER_SIZE / __cycle_count);
            fill_buffer((void *)shmem_addr, cur_buf_size, (void *)&my_value, sizeof(my_value));

            /* Give some time to all PE for setting their values */
            shmem_barrier_all();

            /* Define peer and it value */
            peer_proc = (my_proc + 1) % num_proc;
            peer_value = (peer_proc % 2 ? 1 : -1) * (i * (MAX_VALUE / __cycle_count));

            /* Define expected value */
            expect_value = peer_value;

            /* Get value from peer */
            FUNC_VALUE(recv_addr, shmem_addr, cur_buf_size, peer_proc);

            rc = (!compare_buffer_with_const(recv_addr, cur_buf_size, &expect_value, sizeof(expect_value)) ? TC_PASS : TC_FAIL);

            log_debug(OSH_TC, "my(#%d:%lld) peer(#%d:%lld) expected = %lld buffer size = %lld\n",
                               my_proc, (INT64_TYPE)my_value, peer_proc, (INT64_TYPE)peer_value, (INT64_TYPE)expect_value, (INT64_TYPE)cur_buf_size);

            if (rc)
            {
                TYPE_VALUE* check_addr = recv_addr;
                int odd_index = compare_buffer_with_const(check_addr, cur_buf_size, &expect_value, sizeof(expect_value));
                int show_index = (odd_index > 1 ? odd_index - 2 : 0);
                int show_size = sizeof(*check_addr) * sys_min(3, cur_buf_size - odd_index - 1);

                log_debug(OSH_TC, "index of incorrect value: 0x%08X (%d)\n", odd_index - 1, odd_index - 1);
                log_debug(OSH_TC, "buffer interval: 0x%08X - 0x%08X\n", show_index, show_index + show_size);
                show_buffer(check_addr + show_index, show_size);
            }

            shmem_barrier_all();
        }
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
