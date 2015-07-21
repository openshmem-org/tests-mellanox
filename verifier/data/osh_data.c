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
#include "osh_data_tests.h"


const TE_NODE data_tcs[] =
{
    { NULL, osh_data_tc1,   "short_g",	        aopt_set_string( "tc1" ), 	"shmem_short_g(): Retrieve basic type 'short'.", TEST_RUN},
    { NULL, osh_data_tc2,   "int_g",	        aopt_set_string( "tc2" ), 	"shmem_int_g(): Retrieve basic type 'int'.", TEST_RUN},
    { NULL, osh_data_tc3,   "long_g",	        aopt_set_string( "tc3" ), 	"shmem_long_g(): Retrieve basic type 'long'.", TEST_RUN},
    { NULL, osh_data_tc4,   "float_g",	        aopt_set_string( "tc4" ), 	"shmem_float_g(): Retrieve basic type 'float'.", TEST_RUN},
    { NULL, osh_data_tc5,   "double_g",	        aopt_set_string( "tc5" ), 	"shmem_double_g(): Retrieve basic type 'double'.", TEST_RUN},
    { NULL, osh_data_tc6,   "longlong_g",	    aopt_set_string( "tc6" ), 	"shmem_longlong_g(): Retrieve basic type 'long long'.", TEST_RUN},
    { NULL, osh_data_tc7,   "longdouble_g",	    aopt_set_string( "tc7" ), 	"shmem_longdouble_g(): Retrieve basic type 'long double'.", TEST_RUN},
    { NULL, osh_data_tc8,   "short_p",	        aopt_set_string( "tc8" ), 	"shmem_short_p(): Write basic type 'short'.", TEST_RUN},
    { NULL, osh_data_tc9,   "int_p",	        aopt_set_string( "tc9" ), 	"shmem_int_p(): Write basic type 'int'.", TEST_RUN},
    { NULL, osh_data_tc10,  "long_p",	        aopt_set_string( "tc10" ), 	"shmem_long_p(): Write basic type 'long'.", TEST_RUN},
    { NULL, osh_data_tc11,  "float_p",	        aopt_set_string( "tc11" ), 	"shmem_float_p(): Write basic type 'float'.", TEST_RUN},
    { NULL, osh_data_tc12,  "double_p",	        aopt_set_string( "tc12" ), 	"shmem_double_p(): Write basic type 'double'.", TEST_RUN},
    { NULL, osh_data_tc13,  "longlong_p",	    aopt_set_string( "tc13" ), 	"shmem_longlong_p(): Write basic type 'long long'.", TEST_RUN},
    { NULL, osh_data_tc14,  "longdouble_p",	    aopt_set_string( "tc14" ), 	"shmem_longdouble_p(): Write basic type 'long double'.", TEST_RUN},
    { NULL, osh_data_tc15,  "char_get",         aopt_set_string( "tc15" ), 	"shmem_char_get(): Retrieve contiguous data of 'char' type.", TEST_RUN},
    { NULL, osh_data_tc16,  "short_get",        aopt_set_string( "tc16" ), 	"shmem_short_get(): Retrieve contiguous data of 'short' type.", TEST_RUN},
    { NULL, osh_data_tc17,  "int_get",	        aopt_set_string( "tc17" ), 	"shmem_int_get(): Retrieve contiguous data of 'int' type.", TEST_RUN},
    { NULL, osh_data_tc18,  "long_get",         aopt_set_string( "tc18" ), 	"shmem_long_get(): Retrieve contiguous data of 'long' type.", TEST_RUN},
    { NULL, osh_data_tc19,  "float_get",        aopt_set_string( "tc19" ), 	"shmem_float_get(): Retrieve contiguous data of 'float' type.", TEST_RUN},
    { NULL, osh_data_tc20,  "double_get",       aopt_set_string( "tc20" ), 	"shmem_double_get(): Retrieve contiguous data of 'double' type.", TEST_RUN},
    { NULL, osh_data_tc21,  "longlong_get",     aopt_set_string( "tc21" ), 	"shmem_longlong_get(): Retrieve contiguous data of 'long long' type.", TEST_RUN},
    { NULL, osh_data_tc22,  "longdouble_get",   aopt_set_string( "tc22" ), 	"shmem_longdouble_get(): Retrieve contiguous data of 'long double' type.", TEST_RUN},
    { NULL, osh_data_tc23,  "char_put",         aopt_set_string( "tc23" ), 	"shmem_char_put(): Copy contiguous data of 'char' type.", TEST_RUN},
    { NULL, osh_data_tc24,  "short_put",        aopt_set_string( "tc24" ), 	"shmem_short_put(): Copy contiguous data of 'short' type.", TEST_RUN},
    { NULL, osh_data_tc25,  "int_put",	        aopt_set_string( "tc25" ), 	"shmem_int_put(): Copy contiguous data of 'int' type.", TEST_RUN},
    { NULL, osh_data_tc26,  "long_put",         aopt_set_string( "tc26" ), 	"shmem_long_put(): Copy contiguous data of 'long' type.", TEST_RUN},
    { NULL, osh_data_tc27,  "float_put",        aopt_set_string( "tc27" ), 	"shmem_float_put(): Copy contiguous data of 'float' type.", TEST_RUN},
    { NULL, osh_data_tc28,  "double_put",       aopt_set_string( "tc28" ), 	"shmem_double_put(): Copy contiguous data of 'double' type.", TEST_RUN},
    { NULL, osh_data_tc29,  "longlong_put",     aopt_set_string( "tc29" ), 	"shmem_longlong_put(): Copy contiguous data of 'long long' type.", TEST_RUN},
    { NULL, osh_data_tc30,  "longdouble_put",   aopt_set_string( "tc30" ), 	"shmem_longdouble_put(): Copy contiguous data of 'long double' type.", TEST_RUN},
    { NULL, osh_data_tc31,  "getmem",           aopt_set_string( "tc31" ), 	"shmem_getmem(): Retrieve any data type.", TEST_RUN},
    { NULL, osh_data_tc32,  "putmem",	        aopt_set_string( "tc32" ), 	"shmem_putmem(): Writes any data type.", TEST_RUN},
    { NULL, osh_data_tc33,  "get32",            aopt_set_string( "tc33" ), 	"shmem_get32(): Reads contiguous 32-bit elements from a remote PE.", TEST_RUN},
    { NULL, osh_data_tc34,  "get64",	        aopt_set_string( "tc34" ), 	"shmem_get64(): Reads contiguous 64-bit elements from a remote PE.", TEST_RUN},
    { NULL, osh_data_tc35,  "get128",           aopt_set_string( "tc35" ), 	"shmem_get128(): Reads contiguous 128-bit elements from a remote PE.", TEST_RUN},
    { NULL, osh_data_tc36,  "put32",            aopt_set_string( "tc36" ), 	"shmem_put32(): Writes contiguous 32-bit elements to a remote PE.", TEST_RUN},
    { NULL, osh_data_tc37,  "put64",	        aopt_set_string( "tc37" ), 	"shmem_put64(): Writes contiguous 64-bit elements to a remote PE.", TEST_RUN},
    { NULL, osh_data_tc38,  "put128",           aopt_set_string( "tc38" ), 	"shmem_put128(): Writes contiguous 128-bit elements to a remote PE.", TEST_RUN},
    { NULL, osh_data_tc39,  "put_get",          aopt_set_string( "tc39" ), 	"read data written by peer after barrier directly by address and by get operation", TEST_RUN},
    { NULL, osh_data_tc40,  "put_blocking",     aopt_set_string( "tc40" ), 	"blocking PUT operation (data specified for transfer should be buffered)", TEST_RUN},
    { NULL, NULL,           NULL,	aopt_set_string( NULL ),    NULL, TEST_IGNORE}
};


/*
 * List of supported test case options.
 */
static const AOPT_DESC  self_opt_desc[] =
{
	{ 0, AOPT_NOARG, aopt_set_literal( 0 ), aopt_set_string( NULL ), NULL }
};


int proc_tst_data( const TE_NODE *node, int argc, const char **argv )
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
