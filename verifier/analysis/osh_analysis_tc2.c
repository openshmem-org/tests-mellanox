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


/*
 * Parse command line options specific for test case.
 */
static int __parse_opt( const TE_NODE *, int, const char ** );
static int __do_warmup( int );

#define REPORT_USEC_IN_SEC  ((double)1.0e6)
#define REPORT_WAIT         3
#define REPORT_DURATION     1000

#define REPORT_OUTPUT(fmt, ...)  printf(fmt, ##__VA_ARGS__)

static long __report_cycles = REPORT_DURATION;

/*
 * List of supported test case options.
 */
static const AOPT_DESC  self_opt_desc[] =
{
    {
        'd', AOPT_ARG,      aopt_set_literal( 0 ),    aopt_set_string( "duration" ),
            "Duration measurement in cycles (default: " STR(REPORT_DURATION) "cycles )."
    },
	{ 0, AOPT_NOARG, aopt_set_literal( 0 ), aopt_set_string( NULL ), NULL }
};


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_analysis_tc2(const TE_NODE *node, int argc, const char *argv[])
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
    int my_proc = 0;
    long i = 0;
    double report_start = 0;
    double report_end = 0;
    static double report_result = -1.0;

    my_proc = _my_pe();

    /* Warmup */
    rc = __do_warmup( 100 );
    shmem_barrier_all();

    /* Main section */
    i = __report_cycles;
    report_start = sys_gettime();
    while (i--)
    {
        shmem_barrier_all();
    }
    report_end = sys_gettime();

    /* Result calculation */
    sleep(REPORT_WAIT);
    shmem_barrier_all();

    report_result = report_end - report_start;

    log_debug(OSH_TE, "Barrier performance (sec/op): "
                      "%9.3e "
                      "(total: %ld ops time: %9.3e wall secs)\n",
        report_result / (__report_cycles * REPORT_USEC_IN_SEC),
        __report_cycles, 
        report_result / REPORT_USEC_IN_SEC
        );


    if (my_proc == 0)
    {
        REPORT_OUTPUT( 
            "Barrier performance: \n"
            "duration = %ld \n"
            "%12s %12s \n",
            __report_cycles,
            "op/sec", "wall sec"
            );
        REPORT_OUTPUT( "%12.3e %12.3e\n",
            report_result / (__report_cycles * REPORT_USEC_IN_SEC),
            report_result / REPORT_USEC_IN_SEC
            );
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
