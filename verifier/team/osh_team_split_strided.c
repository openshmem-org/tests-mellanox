/*
 * Copyright (c) 2025      Nvidia Corporation.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include <stdio.h>

#include <shmem.h>
#include <shmemx.h>

#include "osh_def.h"
#include "osh_cmn.h"
#include "osh_log.h"

#include "osh_team_tests.h"


static int _test_team_split_strided(int start, int stride)
{
  int status = TC_PASS;
  int npes = shmem_n_pes();
  int expected_npes = (npes - start) / stride;
  shmem_team_t new_team;

  status = shmem_team_split_strided(SHMEM_TEAM_WORLD, start, stride, expected_npes, NULL, 0,
                                    &new_team);
  if (status || (new_team == SHMEM_TEAM_INVALID)) {
    log_error(OSH_TC, "Error: shmem_team_split_strided failed\n");
    return TC_FAIL;
  }

  if (validate_pe_translation(new_team) != TC_PASS) {
    log_error(OSH_TC, "Error: pe translation to global team failed\n");
    status = TC_FAIL;
    goto out;
  }

  if (validate_team_npes(new_team, expected_npes, "strided_team") != TC_PASS) {
    status = TC_FAIL;
    goto out;
  }

out:
  shmem_team_destroy(new_team);
  return status;
}


int test_team_split_strided(const TE_NODE *node, int argc, const char *argv[])
{
  int status = TC_PASS;
  int npes = shmem_n_pes();
  int stride, start;

  UNREFERENCED_PARAMETER(node);
  UNREFERENCED_PARAMETER(argc);
  UNREFERENCED_PARAMETER(argv);

  for (stride = 2; stride <= npes / 2; ++stride) {
    for (start = 0; start < (npes / stride); ++start) {
      status = _test_team_split_strided(start, stride);
      if (status != TC_PASS) {
        log_error(OSH_TC, "Error: test_team_split_strided failed for stride %d, start %d\n", stride, start);
        return status;
      }
    }
  }

  return status;
}
