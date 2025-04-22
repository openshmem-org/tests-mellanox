/*
 * Copyright (c) 2025      Nvidia Corporation.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#ifndef __OSH_TEAM_H__
#define __OSH_TEAM_H__

#include <shmem.h>

int validate_pe_translation(shmem_team_t team);
int validate_team_npes(shmem_team_t team, int expected_npes, const char *team_name);

int test_team_split_strided(const TE_NODE *node, int argc, const char *argv[]);
int test_team_split_2d(const TE_NODE *node, int argc, const char *argv[]);

#endif
