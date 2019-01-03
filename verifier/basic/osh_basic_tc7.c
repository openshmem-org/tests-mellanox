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

#include "osh_basic_tests.h"

/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int test_variable = 0;
int osh_basic_tc7(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;
    int me = _my_pe();
    int num_of_pes = _num_pes();
    //static int test_variable = 0;
    int value_to_set = 1;

    UNREFERENCED_PARAMETER(node);
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    //just in case
    shmem_barrier_all();

    if (num_of_pes < 2)
    {
        return TC_SETUP_FAIL;
    }

    if (0 == me)
    {
        int wait_time = 0;
        //wait for pe #1 to change my variable
        //and do some important work in the while
        while (shmem_int_test(&test_variable, SHMEM_CMP_EQ, 0) && wait_time < 5000000)
        {
            usleep(1000);
            wait_time += 1000;
            do_progress();
        }

        if (!test_variable)
        {
            rc = TC_FAIL;
        }
    }
    else if (1 == me)
    {
        shmem_int_put(&test_variable, &value_to_set, 1, 0);
    }

    shmem_barrier_all();

    return rc;
}


