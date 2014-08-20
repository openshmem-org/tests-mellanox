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

int osh_coll_tc3(const TE_NODE *node, int argc, const char *argv[])
{
  /* General initialisations            */

  int ii, numprocs, master;
  static int32_t source[10] = { 1, 2, 3, 4, 5,
                      6, 7, 8, 9, 10 };
  static int32_t target[10];

  int nlong;
  long *pSync = 0;

  int status = TC_PASS;

  UNREFERENCED_PARAMETER(node);
  UNREFERENCED_PARAMETER(argc);
  UNREFERENCED_PARAMETER(argv);

  numprocs = _num_pes();

  master = 1;
  nlong = 10;

  if (numprocs == 1)
  {
    log_debug(OSH_TC, "Using more than 1 CPU makes the tests of this program more interesting\n");
    return TC_SETUP_FAIL;
  }

  for (ii = 0; ii < nlong; ii++)
    target[ii] = 0;

  pSync = NULL;

  pSync = shmalloc(sizeof(long) *_SHMEM_COLLECT_SYNC_SIZE);

  for (ii=0; ii < _SHMEM_COLLECT_SYNC_SIZE; ii++) {
    pSync[ii] = _SHMEM_SYNC_VALUE;
  }

  shmem_barrier_all();      /* Wait for all CPUs to initialize pSync */

  /* Broadcast function             */

  if (_my_pe()%2 == 1)
    shmem_broadcast32(target, source, nlong, 0, 1, 1,
                    numprocs/2, pSync);     /* local master CPU = 0 */

  if ((_my_pe()%2 == 1) && (_my_pe() != master))
    for (ii = 0; ii < nlong; ii++)
      if (target[ii] != (ii + 1))
        status = TC_FAIL;

  if (pSync) {
    shfree(pSync);
  }
  return status;
}

