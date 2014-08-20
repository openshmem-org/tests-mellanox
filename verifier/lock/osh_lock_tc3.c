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
int osh_lock_tc3(const TE_NODE *node, int argc, const char *argv[])
{
    static long test_variable = 0;
    //long *test_variable = shmalloc(sizeof(long));
    //*test_variable = 0;

    UNREFERENCED_PARAMETER(node);
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    shmem_barrier_all();
    if (!shmem_test_lock(&test_variable))
    {
        shmem_clear_lock(&test_variable);
    }
    shmem_barrier_all();

    //shfree(test_variable);
    return TC_PASS;
}

