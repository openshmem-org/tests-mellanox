/*
 * Copyright (c) 2014-2016 Mellanox Technologies, Inc.
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

#include "osh_tests.h"

extern const TE_NODE atomic_tcs[];
extern const TE_NODE basic_tcs[];
extern const TE_NODE data_tcs[];
extern const TE_NODE coll_tcs[];
extern const TE_NODE lock_tcs[];
extern const TE_NODE reduce_tcs[];
extern const TE_NODE sync_tcs[];
extern const TE_NODE strided_tcs[];
#ifdef ENABLE_MPI
extern const TE_NODE mix_tcs[];
#endif
extern const TE_NODE analysis_tcs[];
extern const TE_NODE nbi_tcs[];
extern const TE_NODE misc_tcs[];



const TE_NODE exec_tst[] =
{
   { atomic_tcs,    proc_tst_atomic,    "atomic",   aopt_set_string( "at" ),    "Run " MODULE_NAME " ATOMIC test suite.", TEST_RUN},
   { basic_tcs,     NULL,               "basic",    aopt_set_string( "bs" ),    "Run " MODULE_NAME " BASIC test suite.", TEST_RUN},
   { coll_tcs,      proc_tst_coll,      "coll",     aopt_set_string( "cl" ),    "Run " MODULE_NAME " COLLECTIVE Transfer test suite.", TEST_RUN},
   { data_tcs,      proc_tst_data,      "data",     aopt_set_string( "dt" ),    "Run " MODULE_NAME " DATA Transfer test suite.", TEST_RUN},
   { lock_tcs,      proc_tst_lock,      "lock",     aopt_set_string( "lc" ),    "Run " MODULE_NAME " LOCK test suite.", TEST_RUN},
   { reduce_tcs,    proc_tst_reduce,    "reduce",   aopt_set_string( "rd" ),    "Run " MODULE_NAME " REDUCTION test suite.", TEST_RUN},
   { sync_tcs,      proc_tst_sync,      "sync",     aopt_set_string( "sc" ),    "Run " MODULE_NAME " SYNCHRONIZATION test suite.", TEST_RUN},
   { strided_tcs,   proc_tst_strided,   "stride",   aopt_set_string( "st" ),    "Run " MODULE_NAME " STRIDED test suite.", TEST_RUN},
#ifdef ENABLE_MPI
   { mix_tcs,       proc_tst_mix,       "mix",      aopt_set_string( "mx" ),    "Run " MODULE_NAME " MIX test suite.", TEST_RUN},
#endif
   { analysis_tcs,  proc_tst_analysis,  "analysis", aopt_set_string( "an" ),    "Run " MODULE_NAME " ANALYSIS test suite.", TEST_IGNORE},
   { nbi_tcs,       proc_tst_nbi,       "nbi",      aopt_set_string( "nb" ),    "Run " MODULE_NAME " NB DATA Transfer test suite.", TEST_RUN},
   { misc_tcs,      proc_tst_misc,      "misc",     aopt_set_string( "ms" ),    "Run " MODULE_NAME " AUX test suite.", TEST_RUN},
   { NULL,          NULL,               NULL,       aopt_set_string( NULL ),    NULL }
};


/****************************************************************************
 * Static Function Declarations
 ***************************************************************************/
static int __do_exec( const TE_NODE *, const AOPT_OBJECT*, int, const char **);
static int __do_info( const TE_NODE *, const AOPT_OBJECT*, int, const char **);


/*
 * List of supported general options.
 */
static const AOPT_DESC  self_opt_desc[] =
{
    {
        't', AOPT_REPEAT,   aopt_set_literal( 't' ),    aopt_set_string( "task" ),
            "Define test to be launched in format [tsuite]|[tsuite:tcase]|all."
    },
    {
        'i', AOPT_OPTARG,   aopt_set_literal( 0 ),    aopt_set_string( "info" ),
            "Display list of supported tasks [suite|case] (default: suite)."
    },
#if defined(__LINUX__)
    {
        'c', AOPT_NOARG,   aopt_set_literal( 0 ),    aopt_set_string( "no-colour" ),
            "Define output in monochrome view."
    },
#endif
    { 0, AOPT_NOARG, aopt_set_literal( 0 ), aopt_set_string( NULL ), NULL }
};

int proc_mode_exec( const TE_NODE *node, int argc, const char **argv );

int proc_mode_exec( const TE_NODE *node, int argc, const char **argv )
{
    OSH_ERROR status = OSH_ERR_NONE;
    const AOPT_OBJECT *common_obj = NULL;
    const AOPT_OBJECT* self_opt_obj = NULL;

    if (!node)
    {
        return OSH_ERR_FATAL;
    }

    /* Load supported option and create option objects */
    {
        int valid_argc = 0;
        int temp_argc = 0;

        temp_argc = argc;
        common_obj = aopt_init(&temp_argc, (const char **)argv, common_opt_desc);
        valid_argc += temp_argc;
        temp_argc = argc;
        self_opt_obj = aopt_init(&temp_argc, (const char **)argv, self_opt_desc);
        valid_argc += temp_argc;

        /* We do not check for valid_argc > 0 because of in this case
           it should work as --task=all
        */
        if (!common_obj || !self_opt_obj)
        {
            status = OSH_ERR_FATAL;
        }
    }

    if (!status && !aopt_check(common_obj, 'h') && self_opt_obj)
    {
        /* Parse specific options */
        if ( !status && aopt_check(self_opt_obj, 'i') )
        {
            status = __do_info(node, self_opt_obj, argc, argv);
        }
        /* Run test defined test case or all test cases */
        else
        {
            status = __do_exec(node, self_opt_obj, argc, argv);
        }
    }

    if ((status != OSH_ERR_NONE && status != OSH_ERR_TEST_FAILED) || aopt_check(common_obj, 'h'))
    {
        /* Display help information */
        const char* help_str = NULL;
        char temp_buf[30];

        log_help("%s: %s\n", display_opt(node, temp_buf, sizeof(temp_buf)), node->note);
        log_help("\n");
        log_help("Usage: " MODULE_NAME " %s [options] ...\n", node->name);
        log_help("\n");
        log_help("Options:\n");
        help_str = aopt_help(common_opt_desc);
        if (help_str)
        {
            log_help("%s\n", help_str);
            sys_free((void*)help_str);
        }
        printf("Valid arguments:\n");
        help_str = aopt_help(self_opt_desc);
        if (help_str)
        {
            printf("%s", help_str);
            free((void*)help_str);
            printf("\n");
        }
    }

    /* Destroy option objects */
    aopt_exit((AOPT_OBJECT*)common_obj);
    aopt_exit((AOPT_OBJECT*)self_opt_obj);

    return status;
}


static int __do_exec( const TE_NODE* node, const AOPT_OBJECT* opt_obj, int argc, const char *argv[])
{
    int  status = OSH_ERR_NONE;
    int  test_status = OSH_ERR_NONE;
    const TE_NODE* tst_node = NULL;
    const TE_NODE* tcs_node = NULL;
    const TE_NODE* cur_tst_node = NULL;
    const TE_NODE* cur_tcs_node = NULL;
    int index = 0;

    if (!status)
    {
        time_t  start_time;
        time_t  end_time;
        int num_tests = 0;
        int num_passes = 0;
        int num_skipped = 0;
        int num_ignored = 0;
        int num_crippled = 0;

        /* Initialize OpenSHMEM */
        _shmem_init();

        /* Display head */
        {
            log_info(OSH_STD, "\nOpenSHMEM Verification Tool ver.%s\n", STR(VERSION));
            log_info(OSH_STD, "**********************************\n");
            log_info(OSH_STD, "* Log file: %s\n", (osh_config.exec_mode.log_level && osh_config.exec_mode.log_file_name ? osh_config.exec_mode.log_file_name : "none"));
            log_info(OSH_STD, "* Host: %s\n", osh_config.hostname);
            log_info(OSH_STD, "* Output level: %d\n", osh_config.exec_mode.out_level);
            log_info(OSH_STD, "* Log level: %d\n", osh_config.exec_mode.log_level);
            log_info(OSH_STD, "**********************************\n");
            log_info(OSH_STD, "\n");
        }

        /* Seed the random-number generator with current time so that
         * the numbers will be different every time we run.
         */
        start_time = time( NULL );
        srand( (unsigned int)start_time );

        while (!status)
        {
            int all_tests = 1;
            tst_node = NULL;
            tcs_node = NULL;
            cur_tst_node = NULL;
            cur_tcs_node = NULL;

            /* Process --task option */
            if ( opt_obj && aopt_check(opt_obj, 't') )
            {
                const char* optarg = aopt_value_ex(opt_obj, 't', index);
                if ( !optarg )
                {
                    break;
                }
                else
                {
                    const char* value = optarg;

                    index++;
                    if (value && !(all_tests = !sys_strcmp("all", value)))
                    {
                        char* tst_name = (char*)value;
                        char* tcs_name = sys_strchr(value, ':');

                        if (tcs_name)
                        {
                            *tcs_name = '\0';
                            tcs_name++;
                        }

                        tst_node = find_node(node->child, tst_name);
                        if (tst_node)
                        {
                            if (tcs_name)
                            {
                                tcs_node = find_node(tst_node->child, tcs_name);
                            }
                        }
                        else
                        {
                            status = OSH_ERR_BAD_ARGUMENT;
                            break;
                        }
                    }
                }
            }

            cur_tst_node = (tst_node ? tst_node : node->child);
            while (cur_tst_node && cur_tst_node->name)
            {
                cur_tcs_node = (tcs_node ? tcs_node : cur_tst_node->child);
                while (cur_tcs_node && cur_tcs_node->name)
                {
                    int rc = OSH_ERR_NONE;
                    int skipped = (cur_tst_node->active_test == TEST_SKIP  || cur_tcs_node->active_test == TEST_SKIP );
                    int ignored = all_tests && (cur_tst_node->active_test == TEST_IGNORE || cur_tcs_node->active_test == TEST_IGNORE);
                    TE_MEM mstat;
                    long memory_unit_size = (1024 * 1024);
                    const char * memory_unit_size_str = "MB";

                    log_trace(OSH_TE, "TEST SUITE: %s (%s)\n",
                                     cur_tst_node->name, (cur_tst_node->note ? cur_tst_node->note : "no info"));
                    log_trace(OSH_TE, "TEST CASE: %s (%s)\n",
                                     cur_tcs_node->name, (cur_tcs_node->note ? cur_tcs_node->note : "no info"));

                    _shmem_init();

                    /* save previous memory footprint */
                    sys_memcpy(&mstat, &(osh_config.mstat), sizeof(osh_config.mstat));

                    if (!rc && cur_tst_node->func && !skipped)
                    {
                        rc = cur_tst_node->func( cur_tst_node, argc, argv );
                    }

                    if (!rc && cur_tcs_node->func && !skipped)
                    {
                        rc = cur_tcs_node->func( cur_tcs_node, argc, argv );
                    }

                    /* update memory footprint */
                    get_mstat(&(osh_config.mstat));

                    _shmem_finalize();

                    _shmem_sync_result(rc);

                    num_tests++;
                    if (!ignored && !skipped)
                    {
                        if (TC_PASS == rc) {
                            num_passes += 1;
                        } else if (TC_SETUP_FAIL == rc) {
                            num_crippled += 1;
                        } else {
                            test_status = OSH_ERR_TEST_FAILED;
						}
                    }
                    else
                    {
                        if (ignored)
                        {
                            num_ignored++;
                        }
                        else
                        {
                            num_skipped++;
                        }
                    }
                    log_info(OSH_STD, "%s%-6.6s%s %-10.10s %-14.14s %s\n",
#if defined(__LINUX__)
                                            ( opt_obj && aopt_check(opt_obj, 'c') ? "" : (!ignored && !skipped ? get_rc_color(rc) : ( !skipped ? "\e[33m" : "\e[34m")) ),
#else
    ""
#endif
                                            (!ignored && !skipped ? get_rc_string(rc) : ( !skipped ? "IGNORE" : "SKIP")),
#if defined(__LINUX__)
                                            ( opt_obj && aopt_check(opt_obj, 'c') ? "" : "\e[0m" ),
#else
    ""
#endif
                                            cur_tst_node->name,
                                            cur_tcs_node->name,
                                            cur_tcs_node->note
                                            );

                    log_trace(OSH_TE, "Memory Usage (%s):\t"
                                      "before: %lld resident: %lld\t"
                                      "after: %lld resident: %lld\t"
                                      "difference: %+lld resident: %+lld\n",
                        memory_unit_size_str,
                        mstat.vm_size / memory_unit_size,
                        mstat.vm_rss / memory_unit_size,
                        osh_config.mstat.vm_size / memory_unit_size,
                        osh_config.mstat.vm_rss / memory_unit_size,
                        (osh_config.mstat.vm_size - mstat.vm_size) / memory_unit_size,
                        (osh_config.mstat.vm_rss - mstat.vm_rss) / memory_unit_size
                        );

                    if (cur_tcs_node == tcs_node) break;
                    cur_tcs_node++;
                }

                if (cur_tst_node == tst_node) break;
                cur_tst_node++;
            }

            /* Check if there is not --task option in command line
               In this case it should launch all test suites and exit
             */
            if ( opt_obj && !aopt_check(opt_obj, 't') )
            {
                break;
            }
        }

        end_time = time( NULL );

        /* Display foot */
        {
            log_info(OSH_STD, "\n");
            log_info(OSH_STD, "**********************************\n");
            log_info(OSH_STD, "* Passed: %d\n", num_passes);
            log_info(OSH_STD, "* Failed: %d\n", (num_tests - num_passes - num_skipped - num_ignored - num_crippled));
            log_info(OSH_STD, "* Skipped: %d\n", num_skipped);
            log_info(OSH_STD, "* Ignored: %d\n", num_ignored);
            log_info(OSH_STD, "* Incompat:%d\n", num_crippled);
            log_info(OSH_STD, "* Total: %d\n", num_tests);
            log_info(OSH_STD, "* Start time: %02d-%02d-%04d %02d:%02d:%02d\n",
                                            (localtime(&start_time))->tm_mday,
                                            (localtime(&start_time))->tm_mon + 1,
                                            (localtime(&start_time))->tm_year + 1900,
                                            (localtime(&start_time))->tm_hour,
                                            (localtime(&start_time))->tm_min,
                                            (localtime(&start_time))->tm_sec);
            log_info(OSH_STD, "* Finish time: %02d-%02d-%04d %02d:%02d:%02d\n",
                                            (localtime(&end_time))->tm_mday,
                                            (localtime(&end_time))->tm_mon + 1,
                                            (localtime(&end_time))->tm_year + 1900,
                                            (localtime(&end_time))->tm_hour,
                                            (localtime(&end_time))->tm_min,
                                            (localtime(&end_time))->tm_sec);
            log_info(OSH_STD, "* Duration: %d:%02d:%02d\n",
                                            (end_time - start_time) / 3600,
                                            ((end_time - start_time) / 60) % 60,
                                            (end_time - start_time) % 60);
            log_info(OSH_STD, "**********************************\n");
            log_info(OSH_STD, "\n");
        }

        /* Initialize OpenSHMEM */
        _shmem_finalize();
    }

    return ( status ? status : test_status);
}


static int __do_info( const TE_NODE* node, const AOPT_OBJECT* opt_obj, int argc, const char *argv[])
{
    int  status = OSH_ERR_NONE;
    const TE_NODE* cur_tst_node = NULL;
    const TE_NODE* cur_tcs_node = NULL;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    if (opt_obj)
    {
        if ( !status && aopt_check(opt_obj, 'i') )
        {
            const char* optarg = aopt_value(opt_obj, 'i');
            const char* value = ( optarg ? optarg : "suite" );
            if (value)
            {
                int info_type = ( sys_strcmp("case", value) ? 0 : 1);
                int i = 0;

                cur_tst_node = node->child;
                while (cur_tst_node && cur_tst_node->name)
                {
                    char tmp_buf[25];

                    log_help("   %-24.24s\t%-s\n", display_opt(&cur_tst_node[i], tmp_buf, sizeof(tmp_buf)), cur_tst_node[i].note);

                    cur_tcs_node = ( info_type ? cur_tst_node->child : NULL);
                    while (cur_tcs_node && cur_tcs_node->name)
                    {
                        log_help("      %-24.24s\t%-s\n", display_opt(&cur_tcs_node[i], tmp_buf, sizeof(tmp_buf)), cur_tcs_node[i].note);
                        cur_tcs_node++;
                    }
                    cur_tst_node++;
                }
            }
        }
    }

    return status;
}
