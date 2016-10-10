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
static int test_item2(void);
static int test_item3(void);

/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_sync_tc5(const TE_NODE *node, int argc, const char *argv[])
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

    ri = test_item2();
    log_item(node, 2, ri);
    shmem_barrier_all();
    if (rc == TC_PASS)
    {
        rc = ri;
    }

    ri = test_item3();
    log_item(node, 3, ri);
    shmem_barrier_all();
    if (rc == TC_PASS)
    {
        rc = ri;
    }

    return rc;
}

static int test_item1(void)
{
    int me = _my_pe();
    int num_pe = _num_pes();
    int rc = TC_PASS;
    const int number_of_iterations = 10;
    const int number_of_write_attempts = 1000;
    int i, j;
    int zero = 0;

    int *test_array = shmalloc(num_pe * sizeof(int));

    for (i = 0; i < number_of_iterations; i++)
    {
        for (j = 0; j < number_of_write_attempts; j++)
        {
            int test_value = i * j;
            shmem_int_put(test_array + me, &(test_value), 1, 0);
            shmem_quiet();
            if (me != 0)
                shmem_int_get(test_array, test_array, num_pe, 0);
            if (test_array[me] != test_value)
            {
                rc = TC_FAIL;
                log_error(OSH_TC, "(1) quiet failed at size 1 got = %x expected = %x\n", test_array[me], test_value);
            }
            shmem_quiet();
            shmem_int_put(test_array + me, &zero, 1, 0);
            shmem_quiet();
        }
    }

    shfree(test_array);

    return rc;
}

static int test_item2(void)
{
    int me = _my_pe();
    int num_pe = _num_pes();
    int rc = TC_PASS;
    const int number_of_iterations = 10;
    const int number_of_write_attempts = 1000;
    int i, j;
    char zero = 0;

    char *test_array = shmalloc(num_pe * sizeof(char));

    for (i = 0; i < number_of_iterations; i++)
    {
        for (j = 0; j < number_of_write_attempts; j++)
        {
            char test_value = (char)(i*j);
            shmem_char_put(test_array + me, &(test_value), 1, 0);
            shmem_quiet();
            if (me != 0)
                shmem_char_get(test_array, test_array, num_pe, 0);
            if (test_array[me] != test_value)
            {
                unsigned char got = test_array[me];
                unsigned char set = test_value;
                rc = TC_FAIL;
                log_error(OSH_TC, "(2) quiet failed at size 1 got = %x expected = %x\n", got, set);
            }
            shmem_quiet();
            shmem_char_put(test_array + me, &zero, 1, 0);
            shmem_quiet();
        }
    }

    shfree(test_array);

    return rc;
}

static int test_item3(void)
{
    long me = _my_pe();
    int num_pe = _num_pes();
    int rc = TC_PASS;
    const int number_of_iterations = 10;
    const int number_of_write_attempts = 10000;
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
            shmem_char_put(test_array + me, &test_value, 1, 1);
            shmem_quiet();
            if (me != 1)
                shmem_char_get(test_array, test_array, num_pe, 1);
            if (test_array[me] != test_value)
            {
                unsigned char got = test_array[me];
                unsigned char set = test_value;
                rc = TC_FAIL;
                log_debug(OSH_TC, "(3) quiet failed at size 1 got = %x expected = %x\n", got, set);
            }
            shmem_quiet();
            shmem_char_put(test_array + me, &zero, 1, 1);
            shmem_quiet();
        }

        shmem_long_put(&test_variable, &me, 1, 1);
    }

    shfree(test_array);

    return rc;
}
