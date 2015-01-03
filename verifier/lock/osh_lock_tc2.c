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

#include "shmem.h"

#include "osh_lock_tests.h"

/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_lock_tc2(const TE_NODE *node, int argc, const char *argv[])
{
    //This is a stress test which makes sure the distributed locking is not hanging
    long *test_variable = shmalloc(sizeof(long));
    int number_of_iterations = 50;
    int i = 0;

    UNREFERENCED_PARAMETER(node);
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    *test_variable = 0;
    shmem_barrier_all();
    for (i = 0; i < number_of_iterations; i++)
    {
        shmem_set_lock(test_variable);
        shmem_clear_lock(test_variable);
    }
    shmem_barrier_all();

    shfree(test_variable);

    return TC_PASS;
}

