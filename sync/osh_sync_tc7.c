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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "shmem.h"

#include "osh_sync_tests.h"

/****************************************************************************
 * Test Case can consitis of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
static int test_item1(void);

#define NITER 1000
#define NPUTS 10000
#define NNUM  100

/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_sync_tc7(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    rc = test_item1();
    log_item(node, 1, rc);

    return rc;
}


/****************************************************************************
 * Place for Test Item functions
 ***************************************************************************/

static int test_item1(void)
{
    int rc = TC_PASS;
    int num_proc = 0;
    int my_proc = 0;
    int i = 0, j = 0, k = 0;
    int peer = 0;
    long *test_array = 0;
    long *local_array = 0;

    srand( (unsigned)time( NULL ) );

    num_proc = _num_pes();
    my_proc = _my_pe();
    test_array = shmalloc(sizeof(long) * NNUM);
    local_array = malloc(sizeof(long) * NNUM);

    for (k = 0; k < NNUM; k++)
    {
        local_array[k] = k*num_proc + my_proc;
    }

    for (i = 0; i < NITER; i++)
    {
        for (j = 0; j < NPUTS; j++)
        {
            peer = rand() % num_proc;
            shmem_put(test_array, local_array, NNUM, peer);
        }
        shmem_fence();
        shmem_barrier_all();
    }

    shfree(test_array);  
    free(local_array);

    log_debug(OSH_TC, "passed");
    return rc;
}
