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

#define TYPE_VALUE  int64_t
#define FUNC_VALUE  shmem_put64
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
#define REPORT_TSIZE        0

#define REPORT_OUTPUT(fmt, ...)  \
    {   \
        fprintf(stdout, fmt, ##__VA_ARGS__); \
        fflush(stdout); \
    }

static long __report_cycles = REPORT_DURATION;
static long __report_msize = REPORT_MSIZE;
static long __report_tsize = REPORT_TSIZE;

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
    {
        't', AOPT_ARG,      aopt_set_literal( 0 ),    aopt_set_string( "tsize" ),
            "Table size in B,KB,MB,GB (default: np x " STR(REPORT_MSIZE) "bytes )."
    },
	{ 0, AOPT_NOARG, aopt_set_literal( 0 ), aopt_set_string( NULL ), NULL }
};


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_analysis_tc5(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    rc = __parse_opt(node, argc, argv);

    if (rc == TC_PASS)
    {
        rc = test_item1();
        log_item(node, 1, rc);
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
    int pe_proc = 0;
    double report_start = 0;
    double report_end = 0;
    static double report_result = -1.0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    __report_tsize = ( __report_tsize ? __report_tsize : (__report_msize * num_proc));

    /* Warmup */
    rc = __do_warmup( 100 );
    shmem_barrier_all();

    /* Main section */
    if (rc == TC_PASS)
    {
        shmem_addr = (TYPE_VALUE*)shmalloc(__report_tsize);
        send_addr = (TYPE_VALUE*)malloc(__report_msize);
        if (shmem_addr && send_addr)
        {
            TYPE_VALUE expect_value = 0;
            TYPE_VALUE source_value = 0;
            long msize_in_elements = 0;
            long tsize_in_elements = 0;
            int i = 0;
            int j = 0;

            /* cycle for number of elements having data type TYPE_VALUE */
            for (msize_in_elements = 1; (msize_in_elements <= (__report_msize / (long)SIZE_VALUE)) && (rc == TC_PASS); msize_in_elements<<=1)
            {
                tsize_in_elements = __report_tsize / (long)SIZE_VALUE;
                tsize_in_elements = tsize_in_elements - (tsize_in_elements % (msize_in_elements * num_proc));

                source_value = (TYPE_VALUE)my_proc;
                fill_buffer((void *)send_addr, msize_in_elements, (void *)&source_value, SIZE_VALUE);

                shmem_barrier_all();

                i = __report_cycles;
                report_start = sys_gettime();
                while (i--)
                {
                    for (j = 0; j < tsize_in_elements; j += (msize_in_elements * num_proc))
                    {
                        for (pe_proc = 0; pe_proc < num_proc; pe_proc++)
                        {
                            FUNC_VALUE((shmem_addr + j + my_proc * msize_in_elements), send_addr, msize_in_elements, pe_proc);
                        }
                    }
                }
                shmem_barrier_all();
                report_end = sys_gettime();
                /* Check result */
                {
                    pe_proc = 0;
                    for (j = 0; j < tsize_in_elements; j += msize_in_elements)
                    {
                        pe_proc %= num_proc;
                        expect_value = (TYPE_VALUE)pe_proc;
                        if (compare_buffer_with_const((shmem_addr + j), msize_in_elements, &expect_value, sizeof(*shmem_addr)))
                        {
                            rc = TC_FAIL;
                            break;
                        }
                        pe_proc++;
                    }
                }

                report_result = report_end - report_start;

                if (rc == TC_PASS)
                {
                    if (my_proc == 0)
                    {
                        const char *str_scale1 = NULL;
                        const char *str_scale2 = NULL;
                        double result_value1;
                        double result_value2;

                        result_value1 = __scale_value(msize_in_elements * SIZE_VALUE, &str_scale1);
                        result_value2 = __scale_value((__report_cycles * tsize_in_elements * SIZE_VALUE * REPORT_USEC_IN_SEC) / (report_result), &str_scale2);

                        if (msize_in_elements == 1)
                        {
                            REPORT_OUTPUT(
                                "All2All performance: \n"
                                "duration = %ld \n"
                                "%10s %12s %12s %16s\n",
                                __report_cycles,
                                "msize", "op/sec", "wall sec", "yield"
                                );
                        }
                        REPORT_OUTPUT( "%7.0lf %s %12.3e %12.3e %9.2lf %2s/sec\n",
                            result_value1, str_scale1,
                            report_result / (__report_cycles * REPORT_USEC_IN_SEC),
                            report_result / REPORT_USEC_IN_SEC,
                            result_value2, str_scale2
                            );
                    }
                }
                else
                {
                    REPORT_OUTPUT("ERROR: Data integrity failure\n");
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
        free(send_addr);
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

        if ( !status && aopt_check(self_opt_obj, 't') )
        {
            const char* optarg = aopt_value(self_opt_obj, 't');
            if (optarg)
            {
                errno = 0;
                char *str_end = NULL;
                long value = sys_strtol(optarg, &str_end, 0);
                if ( !errno && (value > 0) )
                {
                    if ( !str_end || !(*str_end) )
                    {
                        __report_tsize = value;
                    }
                    else if ( !sys_strcmp(str_end, "bytes") || (str_end[0]=='B') )
                    {
                        __report_tsize = value;
                    }
                    else if ( !sys_strcmp(str_end, "KB") )
                    {
                        __report_tsize = value * 1024;
                    }
                    else if ( !sys_strcmp(str_end, "MB") )
                    {
                        __report_tsize = value * 1024 * 1024;
                    }
                    else if ( !sys_strcmp(str_end, "GB") )
                    {
                        __report_tsize = value * 1024 * 1024 * 1024;
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
