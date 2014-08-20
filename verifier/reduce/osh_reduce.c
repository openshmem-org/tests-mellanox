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
#include "osh_reduce_tests.h"

const TE_NODE reduce_tcs[] =
{
    { NULL, osh_reduce_tc1,     "and_short",    aopt_set_string( "tc1" ), 	"shmem_short_and_to_all(): Performs a bitwise AND operation on symmetric arrays of 'short' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc2,     "and_int",      aopt_set_string( "tc2" ), 	"shmem_int_and_to_all(): Performs a bitwise AND operation on symmetric arrays of 'int' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc3,     "and_long",     aopt_set_string( "tc3" ), 	"shmem_long_and_to_all(): Performs a bitwise AND operation on symmetric arrays of 'long' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc4,     "and_llong",    aopt_set_string( "tc4" ), 	"shmem_longlong_and_to_all(): Performs a bitwise AND operation on symmetric arrays of 'long long' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc5,     "or_short",     aopt_set_string( "tc5" ), 	"shmem_short_or_to_all(): Performs a bitwise OR operation on symmetric arrays of 'short' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc6,     "or_int",       aopt_set_string( "tc6" ), 	"shmem_int_or_to_all(): Performs a bitwise OR operation on symmetric arrays of 'int' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc7,     "or_long",      aopt_set_string( "tc7" ), 	"shmem_long_or_to_all(): Performs a bitwise OR operation on symmetric arrays of 'long' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc8,     "or_llong",     aopt_set_string( "tc8" ), 	"shmem_longlong_or_to_all(): Performs a bitwise OR operation on symmetric arrays of 'long long' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc9,     "xor_short",    aopt_set_string( "tc9" ), 	"shmem_short_xor_to_all(): Performs a bitwise XOR operation on symmetric arrays of 'short' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc10,    "xor_int",      aopt_set_string( "tc10" ), 	"shmem_int_xor_to_all(): Performs a bitwise XOR operation on symmetric arrays of 'int' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc11,    "xor_long",     aopt_set_string( "tc11" ), 	"shmem_long_xor_to_all(): Performs a bitwise XOR operation on symmetric arrays of 'long' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc12,    "xor_llong",    aopt_set_string( "tc12" ), 	"shmem_longlong_xor_to_all(): Performs a bitwise XOR operation on symmetric arrays of 'long long' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc13,    "max_short",    aopt_set_string( "tc13" ), 	"shmem_short_max_to_all(): Defines MAX on symmetric arrays of 'short' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc14,    "max_int",      aopt_set_string( "tc14" ), 	"shmem_int_max_to_all(): Defines MAX on symmetric arrays of 'int' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc15,    "max_long",     aopt_set_string( "tc15" ), 	"shmem_long_max_to_all(): Defines MAX on symmetric arrays of 'long' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc16,    "max_llong",    aopt_set_string( "tc16" ), 	"shmem_longlong_max_to_all(): Defines MAX on symmetric arrays of 'long long' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc17,    "max_float",    aopt_set_string( "tc17" ), 	"shmem_float_max_to_all(): Defines MAX on symmetric arrays of 'float' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc18,    "max_double",   aopt_set_string( "tc18" ), 	"shmem_double_max_to_all(): Defines MAX on symmetric arrays of 'double' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc19,    "max_ldouble",  aopt_set_string( "tc19" ), 	"shmem_longdouble_max_to_all(): Defines MAX on symmetric arrays of 'long double' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc20,    "min_short",    aopt_set_string( "tc20" ), 	"shmem_short_min_to_all(): Defines MIN on symmetric arrays of 'short' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc21,    "min_int",      aopt_set_string( "tc21" ), 	"shmem_int_min_to_all(): Defines MIN on symmetric arrays of 'int' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc22,    "min_long",     aopt_set_string( "tc22" ), 	"shmem_long_min_to_all(): Defines MIN on symmetric arrays of 'long' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc23,    "min_llong",    aopt_set_string( "tc23" ), 	"shmem_longlong_min_to_all(): Defines MIN on symmetric arrays of 'long long' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc24,    "min_float",    aopt_set_string( "tc24" ), 	"shmem_float_min_to_all(): Defines MIN on symmetric arrays of 'float' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc25,    "min_double",   aopt_set_string( "tc25" ), 	"shmem_double_min_to_all(): Defines MIN on symmetric arrays of 'double' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc26,    "min_ldouble",  aopt_set_string( "tc26" ), 	"shmem_longdouble_min_to_all(): Defines MIN on symmetric arrays of 'long double' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc27,    "sum_short",    aopt_set_string( "tc27" ), 	"shmem_short_sum_to_all(): Calculates SUM on symmetric arrays of 'short' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc28,    "sum_int",      aopt_set_string( "tc28" ), 	"shmem_int_sum_to_all(): Calculates SUM on symmetric arrays of 'int' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc29,    "sum_long",     aopt_set_string( "tc29" ), 	"shmem_long_sum_to_all(): Calculates SUM on symmetric arrays of 'long' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc30,    "sum_llong",    aopt_set_string( "tc30" ), 	"shmem_longlong_sum_to_all(): Calculates SUM on symmetric arrays of 'long long' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc31,    "sum_float",    aopt_set_string( "tc31" ), 	"shmem_float_sum_to_all(): Calculates SUM on symmetric arrays of 'float' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc32,    "sum_double",   aopt_set_string( "tc32" ), 	"shmem_double_sum_to_all(): Calculates SUM on symmetric arrays of 'double' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc33,    "sum_ldouble",  aopt_set_string( "tc33" ), 	"shmem_longdouble_sum_to_all(): Calculates SUM on symmetric arrays of 'long double' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc34,    "sum_complexf", aopt_set_string( "tc34" ), 	"shmem_complexf_sum_to_all(): Calculates SUM on symmetric arrays of 'float complex' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc35,    "sum_complexd", aopt_set_string( "tc35" ), 	"shmem_complexd_sum_to_all(): Calculates SUM on symmetric arrays of 'double complex' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc36,    "prod_short",   aopt_set_string( "tc36" ), 	"shmem_short_prod_to_all(): Calculates PROD on symmetric arrays of 'short' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc37,    "prod_int",     aopt_set_string( "tc37" ), 	"shmem_int_prod_to_all(): Calculates PROD on symmetric arrays of 'int' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc38,    "prod_long",    aopt_set_string( "tc38" ), 	"shmem_long_prod_to_all(): Calculates PROD on symmetric arrays of 'long' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc39,    "prod_llong",   aopt_set_string( "tc39" ), 	"shmem_longlong_prod_to_all(): Calculates PROD on symmetric arrays of 'long long' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc40,    "prod_float",   aopt_set_string( "tc40" ), 	"shmem_float_prod_to_all(): Calculates PROD on symmetric arrays of 'float' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc41,    "prod_double",  aopt_set_string( "tc41" ), 	"shmem_double_prod_to_all(): Calculates PROD on symmetric arrays of 'double' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc42,    "prod_ldouble", aopt_set_string( "tc42" ), 	"shmem_longdouble_prod_to_all(): Calculates PROD on symmetric arrays of 'long double' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc43,    "prod_fcomplex",aopt_set_string( "tc43" ), 	"shmem_complexf_prod_to_all(): Calculates PROD on symmetric arrays of 'float complex' datatype over the active set of PEs.", TEST_RUN},
    { NULL, osh_reduce_tc44,    "prod_dcomplex",aopt_set_string( "tc44" ), 	"shmem_complexd_prod_to_all(): Calculates PROD on symmetric arrays of 'double complex' datatype over the active set of PEs.", TEST_RUN},
    { NULL, NULL,           NULL,	        aopt_set_string( NULL ),    NULL, TEST_IGNORE}
};


/*
 * List of supported test case options.
 */
static const AOPT_DESC  self_opt_desc[] =
{
	{ 0, AOPT_NOARG, aopt_set_literal( 0 ), aopt_set_string( NULL ), NULL }
};


int proc_tst_reduce( const TE_NODE *node, int argc, const char **argv )
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
