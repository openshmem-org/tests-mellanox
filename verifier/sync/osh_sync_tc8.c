/*
 * Copyright (c) 2020      Mellanox Technologies, Inc.
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

static int test_item1(const int size1, const int size2,
                      const int number_of_iterations);

/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_sync_tc8(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;
    int ri = TC_PASS;
    int size, ti;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    ri = test_item1(1, 1, 100000);
    log_item(node, 1, ri);
    shmem_barrier_all();
    if (rc == TC_PASS)
    {
        rc = ri;
    }

    ri = test_item1(1000, 1000, 4000);
    log_item(node, 2, ri);
    shmem_barrier_all();
    if (rc == TC_PASS)
    {
        rc = ri;
    }

    ri = test_item1(4000, 4000, 1000);
    log_item(node, 3, ri);
    shmem_barrier_all();
    if (rc == TC_PASS)
    {
        rc = ri;
    }

    ri = test_item1(100000, 100000, 100);
    log_item(node, 4, ri);
    shmem_barrier_all();
    if (rc == TC_PASS)
    {
        rc = ri;
    }

    for (size = 1000000, ti = 5; size >= 1; size /= 10, ++ti) {
        ri = test_item1(size, 1, 10);
        log_item(node, ti, ri);
        shmem_barrier_all();
        if (rc == TC_PASS)
        {
            rc = ri;
        }
    }

    return rc;
}

static int test_item1(const int size1, const int size2, const int number_of_iterations)
{
    long me = _my_pe();
    int num_pe = _num_pes();
    int rc = TC_PASS;
    int max_size = sys_max(size1, size2);
    int i, j;
    char *zero, *test_array;

    if (num_pe <= 1)
        return TC_SETUP_FAIL;

    zero = calloc(size1, sizeof(char));
    test_array = shmalloc(num_pe * sizeof(char) * max_size);
    if ((zero == NULL) || (test_array == NULL))
        return TC_SETUP_FAIL;

    for (i = 0; i < number_of_iterations; i++)
    {
        char test_value[size2];

        memset(test_value, (char)i, size2);
        shmem_putmem(test_array + me * max_size, zero, size1, 1);
        shmem_fence();
        shmem_putmem(test_array + me * max_size, &test_value, size2, 1);
        shmem_fence();
        shmem_barrier_all();
        if (me != 1)
            shmem_getmem(test_array, test_array, num_pe * max_size, 1);
        for (j = 0; j < size2; j++) {
            if (test_array[me * max_size + j] != test_value[j])
            {
                unsigned char got = test_array[me * max_size + j];
                unsigned char set = test_value[j];
                rc = TC_FAIL;
                log_debug(OSH_TC, "(2) fence failed at size %d got = %x expected = %x\n",
                          max_size, got, set);
            }
        }
    }

    free(zero);
    shfree(test_array);

    return rc;
}
