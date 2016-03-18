/*
 * Copyright (c) 2016      Mellanox Technologies, Inc.
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
#include "mpi.h"

#include "osh_nbi_tests.h"

/****************************************************************************
 * Test Case can consist of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
static int test_item1(void);

#define WAIT_COUNT  5
#define TYPE_VALUE  unsigned char
#define FUNC_VALUE  shmem_putmem_nbi
#define SIZE_VALUE  sizeof(TYPE_VALUE)
#define MAX_VALUE   (((unsigned long long)1 << ( 8 * SIZE_VALUE - 1)) - 1)


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_nbi_tc2(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    rc = test_item1();
    log_item(node, 1, rc);
    shmem_barrier_all();

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
    TYPE_VALUE peer_value = 0;
    TYPE_VALUE expect_value = 0;
    int my_proc = 0;
    int peer_proc = 0;

    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    if (shmem_addr)
    {
        /* Set my value */
        my_value = (-1);
        *shmem_addr = my_value;

        /* Define peer and it value */
        peer_proc = my_proc;
        peer_value = (TYPE_VALUE)(((double)rand() / (double)RAND_MAX) * MAX_VALUE);

        /* Define expected value */
        expect_value = peer_value;

        /* This guarantees that PE set initial value before peer change one */
        shmem_barrier_all();

        /* Put value to peer */
        FUNC_VALUE(shmem_addr, &peer_value, 1, peer_proc);
        shmem_quiet();

        /* Get value put by peer:
         * These routines start the remote transfer and may return before the data
         * is delivered to the remote PE
         */
        wait_for_put_completion(peer_proc, 10 /* wait for 10 secs */);

        rc = (expect_value == *shmem_addr ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my(#%d:%lld) peer(#%d:%lld) expected = %lld buffer size = %lld\n",
                           my_proc, (INT64_TYPE)my_value, peer_proc, (INT64_TYPE)peer_value, (INT64_TYPE)expect_value, (INT64_TYPE)1);
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
