/*
 * Copyright (c) 2014      Mellanox Technologies, Inc.
 *                         All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "osh_def.h"
#include "osh_cmn.h"
#include "osh_log.h"

#include "shmem.h"

#include "osh_coll_tests.h"

int osh_coll_tc8(const TE_NODE *node, int argc, const char *argv[])
{
  /* General initialisations			*/
  int rc = TC_PASS;
  int kk = 0;
  int ii, numprocs;
  int32_t *source, *target;
  long *pSync;

  UNREFERENCED_PARAMETER(node);
  UNREFERENCED_PARAMETER(argc);
  UNREFERENCED_PARAMETER(argv);

  numprocs = _num_pes();

  source = NULL;
  target = NULL;
  pSync = NULL;

  if (numprocs == 1)
  {
    log_debug(OSH_TC,"Using more than 1 CPU makes the tests of this program more interesting\n");
    return TC_SETUP_FAIL;
  }

  source = shmalloc(sizeof(int32_t) * numprocs);
  for (ii = 0; ii < numprocs; ii++)
    source[ii] = ii;
  target = shmalloc(sizeof(int32_t) * (numprocs * (numprocs / 2)));
  for (ii = 0; ii < (numprocs * numprocs / 2); ii++)
    target[ii] = 0;

  pSync = shmalloc(sizeof(long) *_SHMEM_COLLECT_SYNC_SIZE);
  for (ii=0; ii < _SHMEM_COLLECT_SYNC_SIZE; ii++)
    pSync[ii] = _SHMEM_SYNC_VALUE;

  shmem_barrier_all();		/* Wait for all CPUs to initialize pSync */

  /* Fcollect function				*/

  if (_my_pe()%2 == 1)
    shmem_fcollect32( target, source, numprocs, 1, 1,
                    (numprocs / 2), pSync );

  if (_my_pe()%2 == 1)
    for (ii = 0; ii < (numprocs * (numprocs / 2)); ii++)
    {
      kk = ( ii % (numprocs) ? kk + 1 : 0);
      if (target[ii] != kk)
      {
        rc = TC_FAIL;
        break;
      }
    }

  /* Finalizes					*/
  shfree(source);
  shfree(target);
  shfree(pSync);

  return rc;
}
