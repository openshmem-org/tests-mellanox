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

#include "osh_tests.h"
#include "osh_sync_tests.h"

const TE_NODE sync_tcs[] =
{
    { NULL, osh_sync_tc1,  "barrier_all",   aopt_set_string( "tc1" ), 	"Barrier All Synchronization Routine.", TEST_RUN},
    { NULL, osh_sync_tc2,  "wait",          aopt_set_string( "tc2" ), 	"Wait Synchronization Routine.", TEST_RUN},
    { NULL, osh_sync_tc3,  "wait_until",    aopt_set_string( "tc3" ), 	"Wait Until Synchronization Routine.", TEST_RUN},
    { NULL, osh_sync_tc4,  "barrier",       aopt_set_string( "tc4" ), 	"Barrier Synchronization Routine.", TEST_RUN},
    { NULL, osh_sync_tc5,  "quiet",         aopt_set_string( "tc5" ), 	"Quiet.", TEST_RUN},
    { NULL, osh_sync_tc6,  "barrier_stress",aopt_set_string( "tc6" ), 	"Barrier All Stress Test.", TEST_RUN},
    { NULL, osh_sync_tc7,  "fence_stress",  aopt_set_string( "tc7" ), 	"Fence Stress Test.", TEST_RUN},
    { NULL, osh_sync_tc8,  "fence",         aopt_set_string( "tc8" ), 	"Fence ordering.", TEST_RUN},
    { NULL, NULL,           NULL,	        aopt_set_string( NULL ),    NULL, TEST_IGNORE}
};


/*
 * List of supported test case options.
 */
static const AOPT_DESC  self_opt_desc[] =
{
	{ 0, AOPT_NOARG, aopt_set_literal( 0 ), aopt_set_string( NULL ), NULL }
};


int proc_tst_sync( const TE_NODE *node, int argc, const char **argv )
{
    OSH_ERROR status = OSH_ERR_NONE;
	const AOPT_OBJECT* self_opt_obj = NULL;

    UNREFERENCED_PARAMETER(argc);

    if (!node)
    {
        return OSH_ERR_FATAL;
    }

	/* Load supported option and create option objects */
	{
		int temp_argc = 0;

		temp_argc = argc;
		self_opt_obj = aopt_init(&temp_argc, (const char **)argv, self_opt_desc);
        if (!self_opt_obj)
        {
            status = OSH_ERR_FATAL;
        }
    }

	if (!status && self_opt_obj) 
    {
        /* Parse specific options */
    }

	if (status) 
    {
        /* Display help information */
	    const char* help_str = NULL;
	    char temp_buf[30];

		log_help("%s: %s\n", display_opt(node, temp_buf, sizeof(temp_buf)), node->note);
		log_help("\n");
		log_help("Valid arguments:\n");
		help_str = aopt_help(self_opt_desc);
	    if (help_str)
	    {
	        log_help("%s", help_str);
	        sys_free((void*)help_str);
		    log_help("\n");
	    }
	}

	/* Destroy option objects */
	aopt_exit((AOPT_OBJECT*)self_opt_obj);

    return status;
}
