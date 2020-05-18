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

#include "osh_sync_tests.h"

static int test_item1(void);

/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_sync_tc8(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;
    int ri = TC_PASS;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    ri = test_item1();
    log_item(node, 1, ri);
    shmem_barrier_all();
    if (rc == TC_PASS)
    {
        rc = ri;
    }

    return rc;
}

static int test_item1(void)
{
    long me = _my_pe();
    int num_pe = _num_pes();
    int rc = TC_PASS;
    const int number_of_iterations = 10;
    const int number_of_write_attempts = 1000;
    int i, j;
    char zero = 0;
    static long test_variable = 0;

    char *test_array = shmalloc(num_pe * sizeof(char));

    if (num_pe <= 1)
        return TC_SETUP_FAIL;

    for (i = 0; i < number_of_iterations; i++)
    {
        for (j = 0; j < number_of_write_attempts; j++)
        {
            char test_value = (char)(i*j);
            shmem_char_put(test_array + me, &zero, 1, 1);
            shmem_fence();
            shmem_char_put(test_array + me, &test_value, 1, 1);
            shmem_fence();
            shmem_barrier_all();
            if (me != 1)
                shmem_char_get(test_array, test_array, num_pe, 1);
            if (test_array[me] != test_value)
            {
                unsigned char got = test_array[me];
                unsigned char set = test_value;
                rc = TC_FAIL;
                log_debug(OSH_TC, "(3) fence failed at size 1 got = %x expected = %x\n", got, set);
            }
        }

        shmem_long_put(&test_variable, &me, 1, 1);
    }

    shfree(test_array);

    return rc;
}
