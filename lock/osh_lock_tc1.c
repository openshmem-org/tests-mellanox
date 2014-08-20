/*
 * Copyright (c) 2014      Mellanox Technologies, Inc.
 *                         All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include <pthread.h>
#include "osh_def.h"
#include "osh_cmn.h"
#include "osh_log.h"

#include "shmem.h"

#include "osh_lock_tests.h"

/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_lock_tc1(const TE_NODE *node, int argc, const char *argv[])
{
    //static long test_variable = 0;
    long *test_variable = shmalloc(sizeof(long));
    int *remote_pe = shmalloc(sizeof(int));
    int number_of_iterations = 1000;
    int number_of_checks = 10;
    int i = 0, j = 0;
    int my_pe = _my_pe();
    int status = TC_PASS;

    UNREFERENCED_PARAMETER(node);
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    *test_variable = 0;
    shmem_barrier_all();

    for (i = 0; i < number_of_iterations; i++)
    {
        shmem_set_lock(test_variable);
        shmem_int_p(remote_pe, my_pe, 0);
        shmem_quiet();
        for (j = 0; j < number_of_checks; j++)
        {
            int remote_value = 0;
            shmem_int_get(&remote_value, remote_pe, 1, 0);
            if (my_pe == remote_value)
            {
                break;
            }
            usleep(1);
        }
        for (j = 0; j < number_of_checks; j++)
        { 
            int real_pe = 0;
            shmem_int_get(&real_pe, remote_pe, 1, 0);
            if (my_pe != real_pe)
            {
                status = TC_FAIL;
                break;
            }
            //usleep(1);
        }
        shmem_clear_lock(test_variable);
    }

    shmem_barrier_all();
    shfree(test_variable);
    shfree(remote_pe);

    return status;
}

