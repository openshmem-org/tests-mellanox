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
static int test_item3(void);
static int test_shmem_ptr(void);
static int test_shmem_accessible(void);


#ifdef QUICK_TEST
#define LOOP_COUNT  100
#else
#define LOOP_COUNT  1000
#endif


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_basic_tc2(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    rc = test_item1();
    log_item(node, 1, rc);

    if (rc == TC_PASS)
    {
        rc = test_item2();
        log_item(node, 2, rc);
    }

    if (rc == TC_PASS)
    {
        rc = test_item3();
        log_item(node, 3, rc);
    }

    if (rc == TC_PASS)
    {
        rc = test_shmem_ptr();
        log_item(node, 4, rc);
    }

    if (rc == TC_PASS) 
    {
        rc = test_shmem_accessible();
        log_item(node, 5, rc);

    }

    return rc;
}


/****************************************************************************
 * Place for Test Item functions
 ***************************************************************************/
static int test_item1(void)
{
    int rc = TC_PASS;
    int num_proc = 0;
    int my_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    rc = (( my_proc < num_proc ) && (my_proc >= 0) ? TC_PASS : TC_FAIL);

    log_debug(OSH_TC, "PE#%d of %d\n", my_proc, num_proc);


    return rc;
}


static int test_item2(void)
{
    int rc = TC_PASS;
    int num_proc = 0;
    int count = LOOP_COUNT;

    num_proc = _num_pes();
    rc = ( num_proc > 0  ? TC_PASS : TC_FAIL);

    while (!rc && --count)
    {
        if (num_proc != _num_pes())
        {
            rc = TC_FAIL;
            break;
        }
    }

    log_debug(OSH_TC, "PE#%d num_proc=%d count=%d\n", _my_pe(), num_proc, count);

    return rc;
}


static int test_item3(void)
{
    int rc = TC_PASS;
    int my_proc = 0;
    int count = LOOP_COUNT;

    my_proc = _my_pe();
    rc = ( my_proc >= 0  ? TC_PASS : TC_FAIL);

    while (!rc && --count)
    {
        if (my_proc != _my_pe())
        {
            rc = TC_FAIL;
            break;
        }
    }

    log_debug(OSH_TC, "PE#%d num_proc=%d count=%d\n", my_proc, _num_pes(), count);

    return rc;
}

static int test_shmem_ptr()
{
    int *ptr;
    static int foo = 0xdeadbeef;

    ptr = shmem_ptr(&foo, 0);

    log_debug(OSH_TC, "%d: ptr %p foo %p val %x\n", _my_pe(), ptr, &foo, ptr ? *ptr : 0);

    return (ptr == NULL || *ptr == 0xdeadbeef) ? TC_PASS : TC_FAIL;
}

static int test_shmem_accessible()
{
    int my_pe = _my_pe();
    int n_pes = _num_pes();

    if (!shmem_pe_accessible(my_pe))
        return TC_FAIL;

    if (!shmem_pe_accessible((my_pe + 1) % n_pes))
        return TC_FAIL;

    if (shmem_pe_accessible(-10))
        return TC_FAIL;

    if (shmem_pe_accessible(n_pes + 10))
        return TC_FAIL;

    return TC_PASS;
}
