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
 * Test Case can consitis of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
static int test_item1(void);
static int test_item2(void);


#define WAIT_SEC      1
#define WAIT_MAX_SEC 20
#define CYCLE_COUNT 100

/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_basic_tc6(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    /* Barrier for all, version with timings */
    rc = test_item1();
    log_item(node, 1, rc);
    shmem_barrier_all();

    /* Barrier for all, version with shmem_int_p(...) routine */
    if (rc == TC_PASS)
    {
        rc = test_item2();
        log_item(node, 2, rc);
        shmem_barrier_all();
    }

    return rc;
}


/****************************************************************************
 * Place for Test Item functions
 ***************************************************************************/
static int test_item1(void)
{
    int rc = TC_PASS;
    time_t start;
    time_t finish;
    time_t wait;
    time_t expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    wait = sys_min(my_proc * WAIT_SEC, WAIT_MAX_SEC);
    expect_value = ( (num_proc - my_proc - 1) > 0 ? (num_proc - my_proc - 1) * WAIT_SEC - 2 : 0 );

    shmem_barrier_all();
    osh_sleep(wait);
    start = time(NULL);
    shmem_barrier_all();
    finish = time(NULL);

    rc = (((finish - start) >= expect_value) ? TC_PASS : TC_FAIL);

    log_debug(OSH_TC, "my_proc = %d wait = %ld limit = %ld actual wait = %ld\n",
                       my_proc, (unsigned long)wait, (unsigned long)expect_value, (unsigned long)(finish - start));

    return rc;
}


static int test_item2(void)
{
    int rc = TC_PASS;
    int num_proc = 0;
    int my_proc = 0;
    int i = 0;
    int* shmem_indx = NULL, *shmem_array = NULL;

    num_proc = _num_pes();
    my_proc = _my_pe();

    if (num_proc < 2)
    {
        rc = TC_SETUP_FAIL;
        return rc;
    }

    shmem_indx = (int*)shmalloc(sizeof(*shmem_indx));
    shmem_array = shmalloc(sizeof(*shmem_array) * (num_proc - 1) * CYCLE_COUNT);

    if ( !shmem_indx || !shmem_array )
    {
        rc = TC_SETUP_FAIL;
        return rc;
    }

    (*shmem_indx) = 0;
    shmem_barrier_all();

    for (i = 0; i < (num_proc - 1) * CYCLE_COUNT; i++)
    {
        if (i % (num_proc -1) == my_proc)
        {
            (*shmem_indx) = shmem_int_g(shmem_indx, num_proc - 1);
            shmem_int_p(shmem_indx, i + 1, num_proc - 1);
            shmem_int_p(shmem_array + (*shmem_indx), i, num_proc - 1);
        }
        shmem_barrier_all();
    }

    if (my_proc == num_proc - 1)
    {
        for (i = 0; i < (num_proc - 1) * CYCLE_COUNT; i++)
        {
            if (shmem_array[i] != i)
            {
                rc = TC_FAIL;
            }
        }
    }

    if (shmem_indx)
    {
        shfree(shmem_indx);
    }
    if (shmem_array)
    {
        shfree(shmem_array);
    }
    return rc;
}
