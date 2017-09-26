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
#define TYPE_VALUE  short
#define FUNC_VALUE  shmem_short_p
#define SIZE_VALUE  sizeof(TYPE_VALUE)
#define MAX_VALUE   (((unsigned long long)1 << ( 8 * SIZE_VALUE - 1)) - 1)
#ifdef QUICK_TEST
#define COUNT_VALUE 10
#else
#define COUNT_VALUE 100
#endif
#define STEP_VALUE  (MAX_VALUE / COUNT_VALUE)


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_data_tc8(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    /* Put random value to itself */
    rc = test_item1();
    log_item(node, 1, rc);
    shmem_barrier_all();

    /* Put defined value to peer */
    if (rc == TC_PASS)
    {
        rc = test_item2();
        log_item(node, 2, rc);
        shmem_barrier_all();
    }

    /* Put range of possible values */
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
    TYPE_VALUE peer_value = 0;
    TYPE_VALUE expect_value = 0;
    int my_proc = 0;
    int peer_proc = 0;

    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    if (shmem_addr)
    {
        TYPE_VALUE value = -1;

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

        /* Write value to peer */
        FUNC_VALUE(shmem_addr, peer_value, peer_proc);

        /* Get value put by peer:
         * These routines start the remote transfer and may return before the data
         * is delivered to the remote PE
         */
        wait_for_put_completion(peer_proc,10 /* wait for 10 secs */);
        value = *shmem_addr;
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
    TYPE_VALUE* shmem_addr = NULL;
    TYPE_VALUE my_value = 0;
    TYPE_VALUE peer_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    if (shmem_addr)
    {
        TYPE_VALUE value = -1;

        /* Set my value */
        my_value = (-1);
        *shmem_addr = my_value;

        /* Define peer and it value */
        peer_proc = (my_proc + 1) % num_proc;
        peer_value = (TYPE_VALUE)my_proc;

        /* Define expected value */
        expect_value = (TYPE_VALUE)(my_proc ? (my_proc - 1) : (num_proc - 1));

        /* This guarantees that PE set initial value before peer change one */
        shmem_barrier_all();

        /* Write value to peer */
        FUNC_VALUE(shmem_addr, peer_value, peer_proc);

        /* Get value put by peer:
         * These routines start the remote transfer and may return before the data
         * is delivered to the remote PE
         */
        wait_for_put_completion(peer_proc,10 /* wait for 10 secs */);
        value = *shmem_addr;

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
    TYPE_VALUE* shmem_addr = NULL;
    TYPE_VALUE my_value = 0;
    TYPE_VALUE peer_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    if (shmem_addr)
    {
        TYPE_VALUE value = -1;
        INT64_TYPE i = 0;

        /* Set my value */
        my_value = (-1);
        *shmem_addr = my_value;
        for (i = 0; i < COUNT_VALUE; i++)
        {
            /* Define peer and it value */
            peer_proc = (my_proc + 1) % num_proc;
            peer_value = (peer_proc % 2 ? 1 : -1) * (i * STEP_VALUE);

            /* Define expected value */
            expect_value = (my_proc % 2 ? 1 : -1) * (i * STEP_VALUE);

            /* This guarantees that PE set initial value before peer change one */
            shmem_barrier_all();

            /* Write value to peer */
            FUNC_VALUE(shmem_addr, peer_value, peer_proc);

            /* Get value put by peer:
             * These routines start the remote transfer and may return before the data
             * is delivered to the remote PE
             */
            wait_for_put_completion(peer_proc,10 /* wait for 10 secs */);
            value = *shmem_addr;

            rc = (expect_value == value ? TC_PASS : TC_FAIL);

            log_debug(OSH_TC, "my(#%d:%lld) peer(#%d:%lld) expected = %lld vs got = %lld\n",
                               my_proc, (INT64_TYPE)my_value, peer_proc, (INT64_TYPE)peer_value, (INT64_TYPE)expect_value, (INT64_TYPE)value);
        }
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
