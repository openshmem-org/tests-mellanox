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

#include "osh_basic_tests.h"

/****************************************************************************
 * Test Case can consitis of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
static int test_item1(void);


/*
 * Parse command line options specific for test case.
 */
static int __parse_opt( const TE_NODE *, int, const char ** );

static long __memory_unit_size = (1024 * 1024);
static const char * __memory_unit_size_str = "MB";


/*
 * List of supported test case options.
 */
static const AOPT_DESC  self_opt_desc[] =
{
    {
        'u', AOPT_ARG,      aopt_set_literal( 0 ),    aopt_set_string( "mem-unit" ),
            "Memory unit B,KB,MB,GB (default: MB )."
    },
	{ 0, AOPT_NOARG, aopt_set_literal( 0 ), aopt_set_string( NULL ), NULL }
};


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_basic_tc1(const TE_NODE *node, int argc, const char *argv[])
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
    TE_MEM mstat;

    sys_memcpy(&mstat, &(osh_config.mstat), sizeof(osh_config.mstat));
    get_mstat(&(osh_config.mstat));

    log_trace(OSH_TE, "Memory Usage (%s):\t"
                      "before start_pes(): %lld resident: %lld\t"
                      "after  start_pes(): %lld resident: %lld\t"
                      "difference: %+lld resident: %+lld\n",
        __memory_unit_size_str,
        mstat.vm_size / __memory_unit_size, 
        mstat.vm_rss / __memory_unit_size, 
        osh_config.mstat.vm_size / __memory_unit_size, 
        osh_config.mstat.vm_rss / __memory_unit_size,
        (osh_config.mstat.vm_size - mstat.vm_size) / __memory_unit_size, 
        (osh_config.mstat.vm_rss - mstat.vm_rss) / __memory_unit_size
        );

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
        if ( !status && aopt_check(self_opt_obj, 'u') ) 
        {
            const char* optarg = aopt_value(self_opt_obj, 'u');
            if (optarg) 
            {
                errno = 0;
                if ( !sys_strcmp(optarg, "B") ) 
                {
                    __memory_unit_size = 1;
                    __memory_unit_size_str = "B";
                }
                else if ( !sys_strcmp(optarg, "KB") ) 
                {
                    __memory_unit_size = 1024;
                    __memory_unit_size_str = "KB";
                }
                else if ( !sys_strcmp(optarg, "MB") ) 
                {
                    __memory_unit_size = 1024 * 1024;
                    __memory_unit_size_str = "MB";
                }
                else if ( !sys_strcmp(optarg, "GB") ) 
                {
                    __memory_unit_size = 1024 * 1024 * 1024;
                    __memory_unit_size_str = "GB";
                }
                else
                {
                    __memory_unit_size = 1;
                    __memory_unit_size_str = "B";
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
