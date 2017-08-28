/*
 * Copyright (c) 2014      Mellanox Technologies, Inc.
 *                         All rights reserved.
 * $COPYRIGHT$
 *
 * This test is based on US patent 7426684 
 * "Lost-cycle measurement using cycle counter" by David C. P. LaFrance-Linden
 * Topic:
 * A method of lost-cycle measurement using a cycle counter. In some embodiments, 
 * the lost-cycle measurement method comprises: getting a current cycle counter value; 
 * finding a number of elapsed cycles between the current cycle counter value and a 
 * preceding cycle counter value; determining whether the number of elapsed cycles is 
 * indicative of lost cycles; and repeating.
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

#define REPORT_WAIT      3
#define REPORT_DURATION  60
#define REPORT_THRESHOLD 80

#define REPORT_OUTPUT(fmt, ...)  printf(fmt, ##__VA_ARGS__)

static long __report_threshold = REPORT_THRESHOLD;
static int64_t __report_cycles = 0;
static long __report_warmup = 100;

/*
 * List of supported test case options.
 */
static const AOPT_DESC  self_opt_desc[] =
{
    {
        'd', AOPT_ARG,      aopt_set_literal( 0 ),    aopt_set_string( "duration" ),
            "Duration measurement in cycles or as period of time with ended sec or min (default: " STR(REPORT_DURATION) "sec )."
    },
    {
        't', AOPT_ARG,      aopt_set_literal( 0 ),    aopt_set_string( "threshold" ),
            "Threshold value that approximates a minimum number of cycles needed for a loop iteration (default: " STR(REPORT_THRESHOLD) " )."
    },
	{ 0, AOPT_NOARG, aopt_set_literal( 0 ), aopt_set_string( NULL ), NULL }
};


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_analysis_tc1(const TE_NODE *node, int argc, const char *argv[])
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
    int num_proc = 0;
    int my_proc = 0;
    int64_t fast_loops = 0;
    int64_t slow_loops = 0;
    int64_t total_cycles = 0;
    int64_t fast_cycles = 0;
    int64_t slow_cycles = 0;
    int64_t cycles_per_loop = 0;
    int64_t got_cycles = 0;
    int64_t cur_cycles = 0;
    int64_t last_cycles = 0;
    int64_t diff_cycles = 0;
    int64_t diff_max_cycles = 0;
    int64_t diff_min_cycles = 0;
    int64_t diff_avg_cycles = 0;
    long i = 0;
    static float report_result = -1.0;

    num_proc = _num_pes();
    my_proc = _my_pe();

#if 0 /* this section set the same __report_cycles for all PEs */
    if (my_proc == 0)
    {
        int peer_proc = 0;

        for (peer_proc = 1; peer_proc < num_proc; peer_proc++)
        {
            shmem_putmem(&__report_cycles, &__report_cycles, sizeof(__report_cycles), peer_proc);
        }
    }
    shmem_barrier_all();
#endif 

    /* Warmup */
    last_cycles = sys_rdtsc();
    while (!rc && (i < __report_warmup))
    {
        cur_cycles = sys_rdtsc();
        diff_cycles = cur_cycles - last_cycles;
        if (diff_cycles < 0)
        {
            break;
        }
        last_cycles = cur_cycles;

        if (i)
        {
            diff_avg_cycles += diff_cycles;
            diff_max_cycles = sys_max(diff_max_cycles, diff_cycles);
            diff_min_cycles = sys_min(diff_min_cycles, diff_cycles);
        }
        else
        {
            diff_avg_cycles = diff_cycles;
            diff_max_cycles = diff_cycles;
            diff_min_cycles = diff_cycles;
        }
        i++;
    }

    shmem_barrier_all();

    /* Main section */
    last_cycles = sys_rdtsc();
    while (total_cycles < __report_cycles)
    {
        cur_cycles = sys_rdtsc();
        diff_cycles = cur_cycles - last_cycles;
        if (diff_cycles < 0)
        {
            rc = TC_FAIL;
            break;
        }
        last_cycles = cur_cycles;

        total_cycles += diff_cycles;
        if (diff_cycles < __report_threshold)
        {
            fast_loops++;
            fast_cycles += diff_cycles;
        }
        else
        {
            slow_loops++;
            slow_cycles += diff_cycles;
        }
    }

    /* Result calculation */
    if (fast_loops && total_cycles)
    {
        cycles_per_loop = fast_cycles / fast_loops;
        got_cycles = fast_cycles + slow_loops * cycles_per_loop;
        report_result = 100 - (float)(got_cycles * 100) / (float)total_cycles;
    }
    else
    {
        REPORT_OUTPUT("Set another threshold\n");
        rc = TC_FAIL;
    }

    log_debug(OSH_TE, "Lost Cycles (%%): "
                      "%0.2f\t"
                      "(total: %lld threshold: %lld diff: %lld(max) %lld(min) %lld(avg))\n",
        report_result,
        __report_cycles, 
        __report_threshold,
        diff_max_cycles,
        diff_min_cycles,
        (__report_warmup ? diff_avg_cycles / __report_warmup : 0)
        );

    sleep(REPORT_WAIT);
    shmem_barrier_all();

    if (my_proc == 0)
    {
        int peer_proc = 0;
        float value = 0;
        float report_max_result = report_result;
        float report_min_result = report_result;
        float report_avg_result = report_result;

        for (peer_proc = 1; peer_proc < num_proc; peer_proc++)
        {
            value = shmem_float_g(&report_result, peer_proc);
            if (value < 0)
            {
                rc = TC_FAIL;
                break;
            }
            report_avg_result += value;
            report_max_result = sys_max(report_max_result, value);
            report_min_result = sys_min(report_min_result, value);
        }

        if (!rc)
        {
            report_avg_result = report_avg_result / num_proc;

            REPORT_OUTPUT( "Lost Cycles (%%): "
                              "max %0.2f  "
                              "min %0.2f  "
                              "avg %0.2f  "
                              "(total: %lld threshold: %ld)\n",
                              report_max_result,
                              report_min_result,
                              report_avg_result,
                              (long long int)__report_cycles, 
                              __report_threshold);
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
        static long __report_duration = REPORT_DURATION;

        __report_cycles = 0;
        if ( !status && aopt_check(self_opt_obj, 'd') ) 
        {
            const char* optarg = aopt_value(self_opt_obj, 'd');
            if (optarg) 
            {
                errno = 0;
                char *str_end = NULL;
                long value = sys_strtol(optarg, &str_end, 0);
                if ( !errno && (value > 0) ) 
                {
                    if ( !str_end || !(*str_end) ) 
                    {
                        __report_cycles = value;
                    }
                    if ( !str_end || !sys_strcmp(str_end, "sec") || (str_end[0]=='s') ) 
                    {
                        __report_duration = value;
                    }
                    else if ( !sys_strcmp(str_end, "min") || (str_end[0]=='m') ) 
                    {
                        __report_duration = value * 60;
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

        /* It is case when duration is set as time or by default */
        if (__report_cycles == 0)
        {
            int64_t cur_cycles = 0;
            int64_t last_cycles = 0;

            cur_cycles = sys_rdtsc();
            sleep(__report_duration);
            last_cycles = sys_rdtsc();
            __report_cycles = last_cycles - cur_cycles;

            if (__report_cycles < 0)
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
                long value = sys_strtol(optarg, NULL, 0);
                if ( !errno && (value > 0) ) 
                {
                    __report_threshold = value;
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
