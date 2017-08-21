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

#include "osh_analysis.h"

/****************************************************************************
 * Test Case can consitis of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
static int test_item1(void);

#define TYPE_VALUE  long
#define FUNC_VALUE  shmem_long_sum_to_all
#define SIZE_VALUE  sizeof(TYPE_VALUE)


/*
 * Parse command line options specific for test case.
 */
static int __parse_opt( const TE_NODE *, int, const char ** );
static int __do_warmup( int );
static double __scale_value( double, const char ** );

#define REPORT_USEC_IN_SEC  ((double)1.0e6)
#define REPORT_WAIT         3
#define REPORT_DURATION     1000
#define REPORT_MSIZE        8096

#define REPORT_OUTPUT(fmt, ...)  printf(fmt, ##__VA_ARGS__)

static long __report_cycles = REPORT_DURATION;
static long __report_msize = REPORT_MSIZE;
static long* pSync = NULL;
static TYPE_VALUE* pWrk = NULL;

/*
 * List of supported test case options.
 */
static const AOPT_DESC  self_opt_desc[] =
{
    {
        'd', AOPT_ARG,      aopt_set_literal( 0 ),    aopt_set_string( "duration" ),
            "Duration measurement in cycles (default: " STR(REPORT_DURATION) "cycles )."
    },
    {
        'm', AOPT_ARG,      aopt_set_literal( 0 ),    aopt_set_string( "msize" ),
            "Maximum message size in B,KB,MB,GB (default: " STR(REPORT_MSIZE) "bytes )."
    },
	{ 0, AOPT_NOARG, aopt_set_literal( 0 ), aopt_set_string( NULL ), NULL }
};


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_analysis_tc4(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    rc = __parse_opt(node, argc, argv);

    if (rc == TC_PASS)
    {
        pSync = shmalloc(sizeof(*pSync) * _SHMEM_REDUCE_SYNC_SIZE);
        if (!pSync)
        {
            rc = TC_SETUP_FAIL;
        }
    }

    if (rc == TC_PASS)
    {
        rc = test_item1();
        log_item(node, 1, rc);
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
    int num_proc = 0;
    int my_proc = 0;
    int root_proc = 0;
    double report_start = 0;
    double report_end = 0;
    static double report_result = -1.0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    /* Warmup */
    rc = __do_warmup( 100 );
    shmem_barrier_all();

    /* Main section */
    if (rc == TC_PASS)
    {
        shmem_addr = (TYPE_VALUE*)shmalloc(__report_msize);
        send_addr = (TYPE_VALUE*)shmalloc(__report_msize);
        if (shmem_addr && send_addr)
        {
            TYPE_VALUE expect_value = 0;
            TYPE_VALUE source_value = 0;
            long nelems = 0;
            int i = 0;

            /* cycle for number of elements having data type TYPE_VALUE */
            for (nelems = 1; (nelems <= (__report_msize / (long)SIZE_VALUE)) && (rc == TC_PASS); nelems<<=1)
            {
                pWrk = shmalloc(sizeof(*pWrk) * sys_max(nelems/2 + 1, _SHMEM_REDUCE_MIN_WRKDATA_SIZE));
                if (pWrk)
                {
                    source_value = (TYPE_VALUE)nelems;
                    expect_value = source_value * num_proc;
                    fill_buffer((void *)send_addr, nelems, (void *)&source_value, SIZE_VALUE);

                    for ( i = 0; i < _SHMEM_REDUCE_SYNC_SIZE; i++ )
                    {
                        pSync[i] = _SHMEM_SYNC_VALUE;
                    }
                    shmem_barrier_all();

                    i = __report_cycles;
                    report_start = sys_gettime();
                    while (i--)
                    {
                        root_proc %= num_proc;
                        send_addr[0] = (TYPE_VALUE)root_proc;
                        FUNC_VALUE(shmem_addr, send_addr, nelems, 0, 0, num_proc, pWrk, pSync);
                        shmem_barrier_all();
                    }
                    report_end = sys_gettime();

                    rc = (!compare_buffer_with_const(
                                    (shmem_addr + 1),
                                    (nelems - 1),
                                    &expect_value,
                                    SIZE_VALUE) ? TC_PASS : TC_FAIL);
                    rc = (rc ? rc : (shmem_addr[0] == (root_proc * num_proc) ? TC_PASS : TC_FAIL));

                    report_result = report_end - report_start;

                    if (rc == TC_PASS)
                    {
                        if (my_proc == 0)
                        {
                            const char *str_scale1 = NULL;
                            const char *str_scale2 = NULL;
                            const char *str_scale3 = NULL;
                            double result_value1;
                            double result_value2;
                            double result_value3;

                            result_value1 = __scale_value(nelems * SIZE_VALUE, &str_scale1);
                            result_value2 = __scale_value((__report_cycles * nelems * SIZE_VALUE * REPORT_USEC_IN_SEC) / (report_result), &str_scale2);
                            result_value3 = __scale_value(((num_proc - 1) * __report_cycles * nelems * SIZE_VALUE * REPORT_USEC_IN_SEC) / (report_result), &str_scale3);

                            if (nelems == 1)
                            {
                                REPORT_OUTPUT(
                                    "Reduce performance: \n"
                                    "duration = %ld \n"
                                    "%10s %12s %12s %16s %16s \n",
                                    __report_cycles,
                                    "msize", "op/sec", "wall sec", "yield /PE", "yield aggregate"
                                    );
                            }
                            REPORT_OUTPUT( "%7.0lf %s %12.3e %12.3e %9.2lf %2s/sec %9.2lf %2s/sec\n",
                                result_value1, str_scale1,
                                report_result / (__report_cycles * REPORT_USEC_IN_SEC),
                                report_result / REPORT_USEC_IN_SEC,
                                result_value2, str_scale2,
                                result_value3, str_scale3
                                );
                        }
                    }
                    else
                    {
                        log_debug(OSH_TC, "my#%d root#%d expected = %lld buffer size = %lld\n",
                                           my_proc, root_proc, (INT64_TYPE)expect_value, (INT64_TYPE)(nelems * SIZE_VALUE));

                        if (nelems > 1)
                        {
                            TYPE_VALUE* check_addr = shmem_addr;
                            int odd_index = compare_buffer_with_const(
                                    (shmem_addr + 1),
                                    (nelems - 1),
                                    &expect_value,
                                    SIZE_VALUE) + 1;
                            int show_index = (odd_index > 1 ? odd_index - 2 : 0);
                            int show_size = sizeof(*check_addr) * sys_min(3, nelems - odd_index - 1);

                            log_debug(OSH_TC, "index of incorrect value: 0x%08X (%d)\n", odd_index - 1, odd_index - 1);
                            log_debug(OSH_TC, "buffer interval: 0x%08X - 0x%08X\n", show_index, show_index + show_size);
                            show_buffer(check_addr + show_index, show_size);
                        }
                        REPORT_OUTPUT("ERROR: Data integrity failure\n");
                    }

                    shfree(pWrk);
                }
                else
                {
                    rc = TC_SETUP_FAIL;
                }
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
        if ( !status && aopt_check(self_opt_obj, 'd') )
        {
            const char* optarg = aopt_value(self_opt_obj, 'd');
            if (optarg)
            {
                errno = 0;
                long value = sys_strtol(optarg, NULL, 0);
                if ( !errno && (value > 0) )
                {
                    __report_cycles = value;
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

        if ( !status && aopt_check(self_opt_obj, 'm') )
        {
            const char* optarg = aopt_value(self_opt_obj, 'm');
            if (optarg)
            {
                errno = 0;
                char *str_end = NULL;
                long value = sys_strtol(optarg, &str_end, 0);
                if ( !errno && (value > 0) )
                {
                    if ( !str_end || !(*str_end) )
                    {
                        __report_msize = value;
                    }
                    else if ( !sys_strcmp(str_end, "bytes") || (str_end[0]=='B') )
                    {
                        __report_msize = value;
                    }
                    else if ( !sys_strcmp(str_end, "KB") )
                    {
                        __report_msize = value * 1024;
                    }
                    else if ( !sys_strcmp(str_end, "MB") )
                    {
                        __report_msize = value * 1024 * 1024;
                    }
                    else if ( !sys_strcmp(str_end, "GB") )
                    {
                        __report_msize = value * 1024 * 1024 * 1024;
                    }

                    if (__report_msize < (long)SIZE_VALUE)
                    {
                        status = OSH_ERR_BAD_ARGUMENT;
                    }
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

static int __do_warmup( int cycles )
{
    int rc = TC_PASS;

    while (cycles--)
    {
        shmem_barrier_all();
    }

    return rc;
}


static double __scale_value( double value, const char **scale )
{
    const char * const str_unit[] = {" B", "KB", "MB", "GB"};

    if (scale)
    {
        int i = 0;
        while ( scale && (value >= 1024.0) && (i < (int)(sys_array_size(str_unit) - 1)) )
        {
            value /= 1024.0;
            i++;
        }

        *scale = str_unit[i];
    }

    return value;
}
