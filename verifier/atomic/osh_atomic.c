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
#include "osh_atomic_tests.h"

const TE_NODE atomic_tcs[] =
{
    { NULL, osh_atomic_tc1,  "int_swap",       aopt_set_string( "tc1" ), 	"shmem_int_swap(): A simple test.",        TEST_RUN},
    { NULL, osh_atomic_tc2,  "long_swap",      aopt_set_string( "tc2" ), 	"shmem_long_swap(): A simple test.",       TEST_RUN},
    { NULL, osh_atomic_tc3,  "swap",           aopt_set_string( "tc3" ), 	"shmem_swap(): A simple test.",            TEST_RUN},
    { NULL, osh_atomic_tc4,  "longlong_swap",  aopt_set_string( "tc4" ), 	"shmem_longlong_swap(): A simple test.",   TEST_RUN},
    { NULL, osh_atomic_tc5,  "float_swap",     aopt_set_string( "tc5" ), 	"shmem_float_swap(): A simple test.",      TEST_RUN},
    { NULL, osh_atomic_tc6,  "double_swap",    aopt_set_string( "tc6" ), 	"shmem_double_swap(): A simple test.",     TEST_RUN},
    { NULL, osh_atomic_tc7,  "int_cswap",      aopt_set_string( "tc7" ), 	"shmem_int_cswap(): A simple test.",       TEST_RUN},
    { NULL, osh_atomic_tc8,  "long_cswap",     aopt_set_string( "tc8" ), 	"shmem_long_cswap(): A simple test.",      TEST_RUN},
    { NULL, osh_atomic_tc9,  "longlong_cswap", aopt_set_string( "tc9" ), 	"shmem_longlong_cswap(): A simple test.",  TEST_RUN},
    { NULL, osh_atomic_tc10, "int_fadd",       aopt_set_string( "tc10" ), 	"shmem_int_fadd(): A simple test.",        TEST_RUN},
    { NULL, osh_atomic_tc11, "long_fadd",      aopt_set_string( "tc11" ), 	"shmem_long_fadd(): A simple test.",       TEST_RUN},
    { NULL, osh_atomic_tc12, "longlong_fadd",  aopt_set_string( "tc12" ), 	"shmem_longlong_fadd(): A simple test.",   TEST_RUN},
    { NULL, osh_atomic_tc13, "int_finc",       aopt_set_string( "tc13" ), 	"shmem_int_finc(): A simple test.",        TEST_RUN},
    { NULL, osh_atomic_tc14, "long_finc",      aopt_set_string( "tc14" ), 	"shmem_long_finc(): A simple test.",       TEST_RUN},
    { NULL, osh_atomic_tc15, "longlong_finc",  aopt_set_string( "tc15" ), 	"shmem_longlong_finc(): A simple test.",   TEST_RUN},
    { NULL, osh_atomic_tc16, "int_add",        aopt_set_string( "tc16" ), 	"shmem_int_add(): A simple test.",         TEST_RUN},
    { NULL, osh_atomic_tc17, "long_add",       aopt_set_string( "tc17" ), 	"shmem_long_add(): A simple test.",        TEST_RUN},
    { NULL, osh_atomic_tc18, "longlong_add",   aopt_set_string( "tc18" ), 	"shmem_longlong_add(): A simple test.",    TEST_RUN},
    { NULL, osh_atomic_tc19, "int_inc",        aopt_set_string( "tc19" ), 	"shmem_int_inc(): A simple test.",         TEST_RUN},
    { NULL, osh_atomic_tc20, "long_inc",       aopt_set_string( "tc20" ), 	"shmem_long_inc(): A simple test.",        TEST_RUN},
    { NULL, osh_atomic_tc21, "longlong_inc",   aopt_set_string( "tc21" ), 	"shmem_longlong_inc(): A simple test.",    TEST_RUN},
    { NULL, osh_atomic_tc22, "stress",         aopt_set_string( "tc22" ), 	"Atomic stress test.",                     TEST_RUN},
    { NULL, osh_atomic_tc23, "int_xor",        aopt_set_string( "tc23" ), 	"shmem_int_xor(): A simple test.",         TEST_INT_XOR},
    { NULL, osh_atomic_tc24, "long_xor",       aopt_set_string( "tc24" ), 	"shmem_long_xor(): A simple test.",        TEST_LONG_XOR},
    { NULL, osh_atomic_tc25, "longlong_xor",   aopt_set_string( "tc25" ), 	"shmem_longlong_xor(): A simple test.",    TEST_LONGLONG_XOR},
    { NULL, osh_atomic_tc26, "int_fxor",       aopt_set_string( "tc26" ), 	"shmem_int_fxor(): A simple test.",        TEST_INT_FXOR},
    { NULL, osh_atomic_tc27, "long_fxor",      aopt_set_string( "tc27" ), 	"shmem_long_fxor(): A simple test.",       TEST_LONG_FXOR},
    { NULL, osh_atomic_tc28, "longlong_fxor",  aopt_set_string( "tc28" ), 	"shmem_longlong_fxor(): A simple test.",   TEST_LONGLONG_FXOR},
    { NULL, osh_atomic_tc29, "int_fand",       aopt_set_string( "tc29" ), 	"shmem_int_fand(): A simple test.",        TEST_INT_FAND},
    { NULL, osh_atomic_tc30, "long_fand",      aopt_set_string( "tc30" ), 	"shmem_long_fand(): A simple test.",       TEST_LONG_FAND},
    { NULL, osh_atomic_tc31, "longlong_fand",  aopt_set_string( "tc31" ), 	"shmem_longlong_fand(): A simple test.",   TEST_LONGLONG_FAND},
    { NULL, osh_atomic_tc32, "int_and",        aopt_set_string( "tc32" ), 	"shmem_int_and(): A simple test.",         TEST_INT_AND},
    { NULL, osh_atomic_tc33, "long_and",       aopt_set_string( "tc33" ), 	"shmem_long_and(): A simple test.",        TEST_LONG_AND},
    { NULL, osh_atomic_tc34, "longlong_and",   aopt_set_string( "tc34" ), 	"shmem_longlong_and(): A simple test.",    TEST_LONGLONG_AND},
    { NULL, osh_atomic_tc35, "int_for",        aopt_set_string( "tc35" ), 	"shmem_int_for(): A simple test.",         TEST_INT_FOR},
    { NULL, osh_atomic_tc36, "long_for",       aopt_set_string( "tc36" ), 	"shmem_long_for(): A simple test.",        TEST_LONG_FOR},
    { NULL, osh_atomic_tc37, "longlong_for",   aopt_set_string( "tc37" ), 	"shmem_longlong_for(): A simple test.",    TEST_LONGLONG_FOR},
    { NULL, osh_atomic_tc38, "int_or",         aopt_set_string( "tc38" ), 	"shmem_int_or(): A simple test.",          TEST_INT_OR},
    { NULL, osh_atomic_tc39, "long_or",        aopt_set_string( "tc39" ), 	"shmem_long_or(): A simple test.",         TEST_LONG_OR},
    { NULL, osh_atomic_tc40, "longlong_or",    aopt_set_string( "tc40" ), 	"shmem_longlong_or(): A simple test.",     TEST_LONGLONG_OR},
    { NULL, NULL,            NULL,	           aopt_set_string( NULL ),      NULL, TEST_IGNORE}
};


/*
 * List of supported test case options.
 */
static const AOPT_DESC  self_opt_desc[] =
{
	{ 0, AOPT_NOARG, aopt_set_literal( 0 ), aopt_set_string( NULL ), NULL }
};


int proc_tst_atomic( const TE_NODE *node, int argc, const char **argv )
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
