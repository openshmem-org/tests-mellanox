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

#include "osh_tests.h"
#include "osh_coll_tests.h"

const TE_NODE coll_tcs[] =
{
    { NULL, osh_coll_tc1,  "broadcast32",   aopt_set_string( "tc1" ), 	"shmem_broadcast32(): Copy a data object to a target location on all PEs of the active set.", TEST_RUN},
    { NULL, osh_coll_tc2,  "broadcast64",   aopt_set_string( "tc2" ), 	"shmem_broadcast64(): Copy a data object to a target location on all PEs of the active set.", TEST_RUN},
    { NULL, osh_coll_tc3,  "bcast10int",    aopt_set_string( "tc3" ), 	"shmem_broadcast32(): From PE == 1 broadcast 10 integers to all (pe%2 == 1) and check", TEST_RUN},
    { NULL, osh_coll_tc4,  "collect32",     aopt_set_string( "tc4" ), 	"shmem_collect32(): Concatenates blocks of data from multiple processing elements (PEs) to an array in every PE.", TEST_RUN},
    { NULL, osh_coll_tc5,  "collect64",     aopt_set_string( "tc5" ), 	"shmem_collect64(): Concatenates blocks of data from multiple processing elements (PEs) to an array in every PE.", TEST_RUN},
    { NULL, osh_coll_tc6,  "fcollect32",    aopt_set_string( "tc6" ), 	"shmem_fcollect32(): Concatenates blocks of data from multiple processing elements (PEs) to an array in every PE.", TEST_RUN},
    { NULL, osh_coll_tc7,  "fcollect64",    aopt_set_string( "tc7" ), 	"shmem_fcollect64(): Concatenates blocks of data from multiple processing elements (PEs) to an array in every PE.", TEST_RUN},
    { NULL, osh_coll_tc8,  "fcollect32_2",  aopt_set_string( "tc8" ), 	"shmem_fcollect32(): Concatenates blocks of data from multiple processing elements (PEs) to an array in every PE - Cray test smatestC2.c.", TEST_RUN},
    { NULL, osh_coll_tc9,  "collect32_2",   aopt_set_string( "tc9" ), 	"shmem_collect32(): Concatenates blocks of data from multiple processing elements (PEs) to an array in every PE - Cray test smatestC3.c.", TEST_RUN},
    { NULL, osh_coll_tc10, "broadcast",     aopt_set_string( "tc10" ), 	"shmem_broadcast(): Copy a data object to a target location on all PEs of the active set.", TEST_RUN},
#ifdef HAVE_ALLTOALL
    { NULL, osh_coll_tc11, "alltoall",      aopt_set_string( "tc11" ),  "shmem_alltoall(): Each PE exchanges a fixed amount of data with all other PEs.", TEST_RUN},
    { NULL, osh_coll_tc12, "alltoalls",     aopt_set_string( "tc12" ),  "shmem_alltoalls(): Each PE exchanges a fixed amount of strided data with all other PEs.", TEST_RUN},
#endif
    { NULL, NULL,           NULL,	        aopt_set_string( NULL ),    NULL, TEST_IGNORE}
};


/*
 * List of supported test case options.
 */
static const AOPT_DESC  self_opt_desc[] =
{
	{ 0, AOPT_NOARG, aopt_set_literal( 0 ), aopt_set_string( NULL ), NULL }
};


int proc_tst_coll( const TE_NODE *node, int argc, const char **argv )
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
