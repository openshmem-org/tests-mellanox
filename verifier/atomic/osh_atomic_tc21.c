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

#include "osh_atomic_tests.h"

/****************************************************************************
 * Test Case can consitis of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
static int test_item1(void);
static int test_item2(void);
static int test_item3(void);


#ifdef QUICK_TEST
#define WAIT_COUNT  1
#else
#define WAIT_COUNT  5
#endif
#define TYPE_VALUE  long long
#define FUNC_VALUE  shmem_longlong_inc
#define DEFAULT_VALUE  (-1)
#ifdef QUICK_TEST
#define COUNT_VALUE 10
#else
#define COUNT_VALUE 100
#endif

static long __cycle_count = COUNT_VALUE;


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_atomic_tc21(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    if (rc == TC_PASS)
    {
        rc = test_item1();
        log_item(node, 1, rc);
        shmem_barrier_all();
    }

    if (rc == TC_PASS)
    {
        rc = test_item2();
        log_item(node, 2, rc);
        shmem_barrier_all();
    }

    if (rc == TC_PASS)
    {
        rc = test_item3();
        log_item(node, 3, rc);
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
    TYPE_VALUE* shmem_addr = NULL;
    TYPE_VALUE my_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    int root_proc = 0;
    int i = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    if (shmem_addr)
    {
        TYPE_VALUE value = 0;

        /* Store my value */
        my_value = (TYPE_VALUE)1;
        *shmem_addr = 0;

        /* Define expected value */
        expect_value = ( my_proc == root_proc ? num_proc * __cycle_count : 0);

        shmem_barrier_all();
        for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++)
        {
            FUNC_VALUE(shmem_addr, root_proc);
        }
        shmem_barrier_all();

        value = *shmem_addr;
        rc = (expect_value == value ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my(#%d:%lld) expected = %lld vs got = %lld\n",
                           my_proc, (INT64_TYPE)my_value, (INT64_TYPE)expect_value, (INT64_TYPE)value);
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (shmem_addr)
    {
        shfree(shmem_addr);
    }

    return rc;
}


static int test_item2(void)
{
    int rc = TC_PASS;
    TYPE_VALUE* shmem_addr = NULL;
    TYPE_VALUE my_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;
    int i = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    if (shmem_addr)
    {
        TYPE_VALUE value = 0;

        /* Store my value */
        my_value = (TYPE_VALUE)1;
        *shmem_addr = 0;

        /* Define peer */
        peer_proc = (my_proc + 1) % num_proc;

        /* Define expected value */
        expect_value = __cycle_count;

        shmem_barrier_all();
        for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++)
        {
            FUNC_VALUE(shmem_addr, peer_proc);
        }
        shmem_barrier_all();

        value = *shmem_addr;
        rc = (expect_value == value ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my(#%d:%lld) expected = %lld vs got = %lld\n",
                           my_proc, (INT64_TYPE)my_value, (INT64_TYPE)expect_value, (INT64_TYPE)value);
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (shmem_addr)
    {
        shfree(shmem_addr);
    }

    return rc;
}


static int test_item3(void)
{
    int rc = TC_PASS;
    static TYPE_VALUE shmem_value = 0;
    TYPE_VALUE* shmem_addr = &shmem_value;
    TYPE_VALUE my_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;
    int i = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    {
        TYPE_VALUE value = 0;

        /* Store my value */
        my_value = (TYPE_VALUE)1;
        *shmem_addr = 0;

        /* Define peer */
        peer_proc = (my_proc + 1) % num_proc;

        /* Define expected value */
        expect_value = __cycle_count;

        shmem_barrier_all();
        for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++)
        {
            FUNC_VALUE(shmem_addr, peer_proc);
        }
        shmem_barrier_all();

        value = *shmem_addr;
        rc = (expect_value == value ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my(#%d:%lld) expected = %lld vs got = %lld\n",
                           my_proc, (INT64_TYPE)my_value, (INT64_TYPE)expect_value, (INT64_TYPE)value);
    }

    return rc;
}
