/*
 * Copyright (c) 2018      Mellanox Technologies, Inc.
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

#if HAVE_DECL_SHMEM_LONGLONG_ATOMIC_OR
#  define TEST_ENABLED 1
#else
#  define TEST_ENABLED 0
#endif

/****************************************************************************
 * Test Case can consitis of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
#if TEST_ENABLED
static int test_item1(void);
static int test_item2(void);
static int test_item3(void);

#define TYPE_VALUE  long long
#define FUNC_VALUE  shmem_longlong_atomic_or
#define PROC_IN_TEST(proc) ((proc) < 4) /* up to 4 procs are in test */
#define ROOT_PROC_VALUE(proc, iter) (PROC_IN_TEST(proc) ? (1L << (((proc) % 4) * 2 + ((iter) % 2))) : DEFAULT_VALUE)
#define PROC_VALUE(proc) (1L << (proc))
#define DEFAULT_VALUE  0
#define COUNT_VALUE 4

static long __cycle_count = COUNT_VALUE;
#endif


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_atomic_tc40(const TE_NODE *node, int argc, const char *argv[])
{
#if TEST_ENABLED
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
#else
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);
    UNREFERENCED_PARAMETER(node);
    return TC_NONE;
#endif
}


#if TEST_ENABLED
/****************************************************************************
 * Place for Test Item functions
 ***************************************************************************/
/* OP to root, allocated */
static int test_item1(void)
{
    int rc = TC_PASS;
    TYPE_VALUE* shmem_addr = NULL;
    TYPE_VALUE expect_value = DEFAULT_VALUE;
    int num_proc = 0;
    int my_proc = 0;
    int root_proc = 0;
    int i;
    int j;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    if (shmem_addr)
    {
        TYPE_VALUE value = 0;

        /* Store my value */
        *shmem_addr = DEFAULT_VALUE;

        /* Define expected value */
        if (my_proc == root_proc) {
            /* if root proc */
            for (j = 0; j < num_proc; j++) {
                for (i = 0; i < __cycle_count; i++) {
                    expect_value |= ROOT_PROC_VALUE(j, i);
                }
            }
        }

        shmem_barrier_all();
        for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++)
        {
            FUNC_VALUE(shmem_addr, ROOT_PROC_VALUE(my_proc, i), root_proc);
        }
        shmem_barrier_all();

        value = *shmem_addr;
        rc = (expect_value == value ? TC_PASS : TC_FAIL);
        log_debug(OSH_TC, "my(#%d) expected = %lld vs got = %lld\n",
                           my_proc, (INT64_TYPE)expect_value, (INT64_TYPE)value);
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


/* OP to neighbour, allocated */
static int test_item2(void)
{
    int rc = TC_PASS;
    TYPE_VALUE* shmem_addr = NULL;
    TYPE_VALUE my_value = DEFAULT_VALUE;
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;
    int i = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();
    peer_proc = (my_proc + 1) % num_proc;

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    if (shmem_addr)
    {
        TYPE_VALUE value = 0;

        /* Store my value */
        *shmem_addr = DEFAULT_VALUE;

        for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++) {
            my_value |= PROC_VALUE(i + my_proc + 1);
        }

        shmem_barrier_all();
        for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++) {
            FUNC_VALUE(shmem_addr, PROC_VALUE(i + peer_proc + 1), peer_proc);
        }
        shmem_barrier_all();

        value = *shmem_addr;
        rc = (my_value == value ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my(#%d) expected = %lld vs got = %lld\n",
                           my_proc, (INT64_TYPE)my_value, (INT64_TYPE)value);
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


/* OP to neighbour, static */
static int test_item3(void)
{
    int rc = TC_PASS;
    static TYPE_VALUE shmem_value = 0;
    TYPE_VALUE* shmem_addr = &shmem_value;
    TYPE_VALUE my_value = DEFAULT_VALUE;
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;
    int i = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();
    peer_proc = (my_proc + 1) % num_proc;

    TYPE_VALUE value = 0;

    /* Store my value */
    *shmem_addr = DEFAULT_VALUE;

    for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++) {
        my_value |= PROC_VALUE(i + my_proc + 1);
    }

    shmem_barrier_all();
    for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++) {
        FUNC_VALUE(shmem_addr, PROC_VALUE(i + peer_proc + 1), peer_proc);
    }
    shmem_barrier_all();

    value = *shmem_addr;
    rc = (my_value == value ? TC_PASS : TC_FAIL);

    log_debug(OSH_TC, "my(#%d) expected = %lld vs got = %lld\n",
                       my_proc, (INT64_TYPE)my_value, (INT64_TYPE)value);

    return rc;
}
#endif
