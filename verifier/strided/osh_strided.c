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
#include "osh_strided_tests.h"

const TE_NODE strided_tcs[] =
{
    { NULL, osh_strided_tc1,   "short_iget",	        aopt_set_string( "tc1" ), 	"shmem_short_iget(): Retrieve strided data  of 'short' type.", TEST_RUN},
    { NULL, osh_strided_tc2,   "int_iget",	        aopt_set_string( "tc2" ), 	"shmem_int_iget(): Retrieve strided data  of 'int' type.", TEST_RUN},
    { NULL, osh_strided_tc3,   "long_iget",	        aopt_set_string( "tc3" ), 	"shmem_long_iget(): Retrieve strided data  of 'long' type.", TEST_RUN},
    { NULL, osh_strided_tc4,   "float_iget",	        aopt_set_string( "tc4" ), 	"shmem_float_iget(): Retrieve strided data  of 'float' type.", TEST_RUN},
    { NULL, osh_strided_tc5,   "double_iget",	        aopt_set_string( "tc5" ), 	"shmem_double_iget(): Retrieve strided data  of 'double' type.", TEST_RUN},
    { NULL, osh_strided_tc6,   "longlong_iget",	    aopt_set_string( "tc6" ), 	"shmem_longlong_iget(): Retrieve strided data  of 'longlong' type.", TEST_RUN},
    { NULL, osh_strided_tc7,   "longdouble_iget",	    aopt_set_string( "tc7" ), 	"shmem_longdouble_iget(): Retrieve strided data  of 'longdouble' type.", TEST_RUN},
    { NULL, osh_strided_tc8,   "iget32",	        aopt_set_string( "tc8" ), 	"shmem_iget32(): Retrieve strided data  with 32-bit element size.", TEST_RUN},
    { NULL, osh_strided_tc9,   "iget64",	        aopt_set_string( "tc9" ), 	"shmem_iget64(): Retrieve strided data  with 64-bit element size.", TEST_RUN},
    { NULL, osh_strided_tc10,  "iget128",	        aopt_set_string( "tc10" ), 	"shmem_iget128(): Retrieve strided data  with 128-bit element size.", TEST_RUN},
    { NULL, osh_strided_tc11,  "short_iput",	        aopt_set_string( "tc11" ), 	"shmem_short_iput(): Write strided data  of 'short' type.", TEST_RUN},
    { NULL, osh_strided_tc12,  "int_iput",	        aopt_set_string( "tc12" ), 	"shmem_int_iput(): Write strided data  of 'int' type.", TEST_RUN},
    { NULL, osh_strided_tc13,  "long_iput",	    aopt_set_string( "tc13" ), 	"shmem_long_iput(): Write strided data  of 'long' type.", TEST_RUN},
    { NULL, osh_strided_tc14,  "float_iput",	    aopt_set_string( "tc14" ), 	"shmem_float_iput(): Write strided data  of 'float' type.", TEST_RUN},
    { NULL, osh_strided_tc15,  "double_iput",        aopt_set_string( "tc15" ), 	"shmem_double_iput(): Write strided data  of 'double' type.", TEST_RUN},
    { NULL, osh_strided_tc16,  "longlong_iput",       aopt_set_string( "tc16" ), 	"shmem_longlong_iput(): Write strided data  of 'longlong' type.", TEST_RUN},
    { NULL, osh_strided_tc17,  "longdouble_iput",	        aopt_set_string( "tc17" ), 	"shmem_longdouble_iput(): Write strided data  of 'longdouble' type.", TEST_RUN},
    { NULL, osh_strided_tc18,  "iput32",        aopt_set_string( "tc18" ), 	"shmem_iput32(): Write strided data  with 32-bit element size.", TEST_RUN},
    { NULL, osh_strided_tc19,  "iput64",       aopt_set_string( "tc19" ), 	"shmem_iput64(): Write strided data  with 64-bit element size.", TEST_RUN},
    { NULL, osh_strided_tc20,  "iput128",      aopt_set_string( "tc20" ), 	"shmem_iput128(): Write strided data  with 128-bit element size.", TEST_RUN},
    { NULL, NULL,           NULL,	aopt_set_string( NULL ),    NULL, TEST_IGNORE}
};


/*
 * List of supported test case options.
 */
static const AOPT_DESC  self_opt_desc[] =
{
	{ 0, AOPT_NOARG, aopt_set_literal( 0 ), aopt_set_string( NULL ), NULL }
};


int proc_tst_strided( const TE_NODE *node, int argc, const char **argv )
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
    }

//	if (!status && self_opt_obj) 
    {
        /* Parse specific options */
        if (!self_opt_obj)
        {
            status = OSH_ERR_FATAL;
        }
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
