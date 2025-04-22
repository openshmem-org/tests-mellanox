/*
 * Copyright (c) 2025      Nvidia Corporation.  All rights reserved.
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
#include "osh_team_tests.h"

const TE_NODE team_tcs[] =
{
    { NULL, test_team_split_strided, "team_split_strided",  aopt_set_string( "team_split_strided" ), "Strided splitting of a team: A correctness test.", TEST_RUN},
    { NULL, test_team_split_2d,      "team_split_2d",       aopt_set_string( "team_split_2d" ),      "2D splitting of a team: A correctness test.", TEST_RUN},
    { NULL, NULL,                    NULL,	                aopt_set_string( NULL ),                 NULL, TEST_IGNORE}
};

/*
 * List of supported test case options.
 */
static const AOPT_DESC  self_opt_desc[] =
{
	{ 0, AOPT_NOARG, aopt_set_literal( 0 ), aopt_set_string( NULL ), NULL }
};


int proc_tst_team( const TE_NODE *node, int argc, const char **argv )
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

/**
 * Validate that the pe translation back to the global team is correct.
 * This is a sanity check to ensure that the team split is done correctly.
 * returns TC_PASS if the translation is correct, TC_FAIL otherwise.
 */
int validate_pe_translation(shmem_team_t team)
{
    int expected_global_pe = shmem_my_pe();
    int team_pe = shmem_team_my_pe(team);
    int global_pe = shmem_team_translate_pe(team, team_pe, SHMEM_TEAM_WORLD);

    // PE is might not part of the team, so we return TC_PASS in that case
    if (team_pe == -1) {
        return TC_PASS;
    }

    if (global_pe != expected_global_pe) {
        log_error(OSH_TC, "Error: failed to translate pe back to global pe: global_pe %d != %d\n", global_pe, expected_global_pe);
        return TC_FAIL;
    }

    return TC_PASS;
}

int validate_team_npes(shmem_team_t team, int expected_npes, const char *team_name)
{
    int npes = shmem_team_n_pes(team);
    if (npes != expected_npes) {
        log_error(OSH_TC, "Error: %s npes size mismatch: %d != %d\n", team_name, npes, expected_npes);
        return TC_FAIL;
    }

    return TC_PASS;
}
