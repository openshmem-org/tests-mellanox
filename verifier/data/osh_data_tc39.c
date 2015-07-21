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

#include "osh_data_tests.h"


/****************************************************************************
 * Test Case can consitis of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
static int test_item1(void);
static int test_item2(void);
static int test_item3(void);


#define WAIT_COUNT  5


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_data_tc39(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    /* direct access by address */
    rc = test_item1();
    log_item(node, 1, rc);
    shmem_barrier_all();

    /* progress */
    if (rc == TC_PASS)
    {
        rc = test_item2();
        log_item(node, 2, rc);
        shmem_barrier_all();
    }

    /* access using get function */
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
        my_value = -1;
        *shmem_addr = my_value;

        /* Define peer and it value */
        peer_proc = (my_proc + 1) % num_proc;
        peer_value = my_proc;

        /* Define expected value */
        expect_value = (my_proc ? (my_proc - 1) : (num_proc - 1));

        /* Wait is set instead of barrier to give some time to all PE for setting their values */
        shmem_barrier_all();

        /* Put value to peer */
        shmem_int_p(shmem_addr, peer_value, peer_proc);

        shmem_barrier_all();
        {
            int wait = WAIT_COUNT;
            while (wait--)
            {
                value = *shmem_addr;

                if (expect_value == value) break;
                sleep(1);
            }
        }
        shmem_barrier_all();

        rc = (expect_value == value ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my(#%d:%lld) peer(#%d:%lld) expected = %lld vs got = %lld\n",
                           my_proc, (INT64_TYPE)my_value, peer_proc, (INT64_TYPE)peer_value, (INT64_TYPE)expect_value, (INT64_TYPE)value);
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
        my_value = -1;
        *shmem_addr = my_value;

        /* Define peer and it value */
        peer_proc = (my_proc + 1) % num_proc;
        peer_value = my_proc;

        /* Define expected value */
        expect_value = (my_proc ? (my_proc - 1) : (num_proc - 1));

        /* Wait is set instead of barrier to give some time to all PE for setting their values */
        shmem_barrier_all();

        /* Put value to peer */
        shmem_int_p(shmem_addr, peer_value, peer_proc);

        shmem_barrier_all();
        {
            int wait = WAIT_COUNT;
            while (wait--)
            {
                value = *shmem_addr;
                do_progress();

                if (expect_value == value) break;
                sleep(1);
            }
        }
        shmem_barrier_all();

        rc = (expect_value == value ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my(#%d:%lld) peer(#%d:%lld) expected = %lld vs got = %lld\n",
                           my_proc, (INT64_TYPE)my_value, peer_proc, (INT64_TYPE)peer_value, (INT64_TYPE)expect_value, (INT64_TYPE)value);
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
        my_value = -1;
        *shmem_addr = my_value;

        /* Define peer and it value */
        peer_proc = (my_proc + 1) % num_proc;
        peer_value = my_proc;

        /* Define expected value */
        expect_value = (my_proc ? (my_proc - 1) : (num_proc - 1));

        /* Wait is set instead of barrier to give some time to all PE for setting their values */
        shmem_barrier_all();

        /* Put value to peer */
        shmem_int_p(shmem_addr, peer_value, peer_proc);

        shmem_barrier_all();
        {
            int wait = WAIT_COUNT;
            while (wait--)
            {
                value = shmem_int_g(shmem_addr, my_proc);

                if (expect_value == value) break;
                sleep(1);
            }
        }
        shmem_barrier_all();

        rc = (expect_value == value ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my(#%d:%lld) peer(#%d:%lld) expected = %lld vs got = %lld\n",
                           my_proc, (INT64_TYPE)my_value, peer_proc, (INT64_TYPE)peer_value, (INT64_TYPE)expect_value, (INT64_TYPE)value);
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
