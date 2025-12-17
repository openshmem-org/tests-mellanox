/*
 * Copyright (c) 2024      NVIDIA Corporation.
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
#include "osh_signal_tests.h"


const TE_NODE signal_tcs[] =
{
#ifdef HAVE_SIGNAL_PUT
    { NULL, osh_signal_tc1,   "put_signal_set",     aopt_set_string( "tc1" ),  "shmem_putmem_signal(): Copy data with SHMEM_SIGNAL_SET.", TEST_SIGNAL_PUT_SIGNAL},
    { NULL, osh_signal_tc2,   "put_signal_add",     aopt_set_string( "tc2" ),  "shmem_putmem_signal(): Copy data with SHMEM_SIGNAL_ADD.", TEST_SIGNAL_PUT_SIGNAL},
#endif
#ifdef HAVE_SIGNAL_FETCH
    { NULL, osh_signal_tc3,   "signal_fetch",       aopt_set_string( "tc3" ),  "shmem_signal_fetch(): Fetch signal value.", TEST_SIGNAL_FETCH},
#endif
    { NULL, NULL,             NULL,                 aopt_set_string( NULL ),   NULL, TEST_IGNORE}
};


/*
 * List of supported test case options.
 */
static const AOPT_DESC  self_opt_desc[] =
{
	{ 0, AOPT_NOARG, aopt_set_literal( 0 ), aopt_set_string( NULL ), NULL }
};


int proc_tst_signal( const TE_NODE *node, int argc, const char **argv )
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
        /* Destroy option objects */
        aopt_exit((AOPT_OBJECT*)self_opt_obj);
    }

    return status;
}

