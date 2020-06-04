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
static int test_item2(const int chunk, const int number_of_iterations);

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

    ri = test_item2(1000, 4000);
    log_item(node, 2, ri);
    shmem_barrier_all();
    if (rc == TC_PASS)
    {
        rc = ri;
    }

    ri = test_item2(4000, 1000);
    log_item(node, 3, ri);
    shmem_barrier_all();
    if (rc == TC_PASS)
    {
        rc = ri;
    }

    ri = test_item2(100000, 100);
    log_item(node, 4, ri);
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
    const int number_of_iterations = 100000;
    int i;
    char zero = 0;

    char *test_array = shmalloc(num_pe * sizeof(char));

    if (num_pe <= 1)
        return TC_SETUP_FAIL;

    for (i = 0; i < number_of_iterations; i++)
    {
        char test_value = (char)i;
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
            log_debug(OSH_TC, "(1) fence failed at size 1 got = %x expected = %x\n", got, set);
        }
    }

    shfree(test_array);

    return rc;
}

static int test_item2(const int chunk, const int number_of_iterations)
{
    long me = _my_pe();
    int num_pe = _num_pes();
    int rc = TC_PASS;
    int i, j;
    char zero[chunk];

    memset(zero, 0, chunk);
    char *test_array = shmalloc(num_pe * sizeof(char) * chunk);

    if (num_pe <= 1)
        return TC_SETUP_FAIL;

    for (i = 0; i < number_of_iterations; i++)
    {
        char test_value[chunk];

        memset(test_value, (char)i, chunk);
        shmem_putmem(test_array + me * chunk, &zero, chunk, 1);
        shmem_fence();
        shmem_putmem(test_array + me * chunk, &test_value, chunk, 1);
        shmem_fence();
        shmem_barrier_all();
        if (me != 1)
            shmem_getmem(test_array, test_array, num_pe * chunk, 1);
        for (j = 0; j < chunk; j++) {
            if (test_array[me * chunk + j] != test_value[j])
            {
                unsigned char got = test_array[me * chunk + j];
                unsigned char set = test_value[j];
                rc = TC_FAIL;
                log_debug(OSH_TC, "(2) fence failed at size %d got = %x expected = %x\n", chunk, got, set);
            }
        }
    }

    shfree(test_array);

    return rc;
}
