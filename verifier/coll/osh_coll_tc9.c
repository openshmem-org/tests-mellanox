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

int osh_coll_tc9(const TE_NODE *node, int argc, const char *argv[])
{
  /* General initialisations			*/

  int rc = TC_PASS;

  int ii, numprocs, count, d, nlong;
  int32_t *source, *target, *displ;
  long *pSync;

  UNREFERENCED_PARAMETER(node);
  UNREFERENCED_PARAMETER(argc);
  UNREFERENCED_PARAMETER(argv);

  numprocs = _num_pes();

  nlong = _my_pe() + 1;

  source = NULL;
  displ = NULL;
  target = NULL;
  pSync = NULL;

  if (numprocs == 1)
  {
    log_debug(OSH_TC, "Using more than 1 CPU makes the tests of this program more interesting\n");
    return TC_SETUP_FAIL;
  }

  displ = malloc(sizeof(int) * numprocs);

  count = 0;
  for (ii = 0; ii < numprocs; ii++)
  {
    displ[ii] = count;
    count = count + ii + 1;
  }

  pSync = shmalloc(sizeof(long) *_SHMEM_COLLECT_SYNC_SIZE);
  for (ii=0; ii < _SHMEM_COLLECT_SYNC_SIZE; ii++)
    pSync[ii] = _SHMEM_SYNC_VALUE;

  target = shmalloc(sizeof(int) * count);
  for (ii = 0; ii < count; ii++)
    target[ii] = 0;

  source = shmalloc(sizeof(int) * numprocs);
  for (ii = 0; ii < nlong; ii++)
    source[ii] = ii;

  shmem_barrier_all();		/* Wait for all CPUs to initialize pSync */

  /* Collect function				*/

  shmem_collect32( target, source, nlong, 0, 0,
                 numprocs, pSync );

  ii = d = 0;
  while (ii < numprocs)
  {
    for(count = 0; count <= ii; count++)
      if (target[d + count] != count)
        rc = TC_FAIL;
    d = displ[count];
    ii++;
  }

  /* Finalizes					*/
  shfree(source);
  shfree(target);
  shfree(pSync);
  free(displ);

  return rc;
}
