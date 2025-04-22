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


int _test_team_split_2d(int xrange)
{
  int status = TC_PASS;
  int npes = shmem_n_pes();
  int expected_yrange = npes / xrange;
  shmem_team_t x_team, y_team;

  status = shmem_team_split_2d(SHMEM_TEAM_WORLD, xrange, NULL, 0, &x_team, 
                               NULL, 0, &y_team);
  if (status ||
      (x_team == SHMEM_TEAM_INVALID) ||
      (y_team == SHMEM_TEAM_INVALID)) {
      log_error(OSH_TC, "Error: shmem_team_split_2d failed\n");
      return TC_FAIL;
  }

  if (validate_pe_translation(x_team) != TC_PASS ||
      validate_pe_translation(y_team) != TC_PASS) {
      log_error(OSH_TC, "Error: x_team or y_team pe translation failed\n");
      status = TC_FAIL;
      goto out_free_teams;
  }

  if (validate_team_npes(x_team, xrange, "x_team") != TC_PASS ||
      validate_team_npes(y_team, expected_yrange, "y_team") != TC_PASS) {
      log_error(OSH_TC, "Error: x_team or y_team npes size not as expected\n");
      status = TC_FAIL;
      goto out_free_teams;
  }

out_free_teams:
  shmem_team_destroy(x_team);
  shmem_team_destroy(y_team);
  return status;
}

int test_team_split_2d(const TE_NODE *node, int argc, const char *argv[])
{
  int status = TC_PASS;
  int npes = shmem_n_pes();
  int xrange;

  UNREFERENCED_PARAMETER(node);
  UNREFERENCED_PARAMETER(argc);
  UNREFERENCED_PARAMETER(argv);

  for (xrange = 2; xrange <= npes; ++xrange) {
    status = _test_team_split_2d(xrange);
    if (status != TC_PASS) {
      log_error(OSH_TC, "Error: test_team_split_2d failed for xrange %d\n", xrange);
      return status;
    }
  }

  return status;
}
