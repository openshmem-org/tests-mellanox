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


#ifdef QUICK_TEST
#define WAIT_COUNT  1
#else
#define WAIT_COUNT  5
#endif


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_basic_tc4(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    rc = test_item1();
    log_item(node, 1, rc);

    return rc;
}


/****************************************************************************
 * Place for Test Item functions
 ***************************************************************************/
static int test_item1(void)
{
    int rc = TC_PASS;
    int* shmem_addr = NULL;
    int my_value = 0;
    int peer_value = 0;
    int expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    if (shmem_addr)
    {
        int value = 0;

        /* Set my value */
        my_value = my_proc;
        *shmem_addr = my_value;

        /* Define peer and it value */
        peer_proc = (my_proc + 1) % num_proc;
        peer_value = peer_proc;

        /* Define expected value */
        expect_value = peer_value;

        /* Wait is set instead of barrier to give some time to all PE for setting their values */
        shmem_barrier_all();

        /* Get value from peer */
        {
            int wait = WAIT_COUNT;

            while (wait--)
            {
                value = shmem_int_g(shmem_addr, peer_proc);
                if (value == expect_value) break;
                sleep(1);
            }
        }
        rc = (expect_value == value ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my(#%d:%d) peer(#%d:%d) expected = %d vs got = %d\n",
                           my_proc, my_value, peer_proc, peer_value, expect_value, value);
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
