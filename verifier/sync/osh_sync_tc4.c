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

/****************************************************************************
 * Test Case can consitis of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
#ifdef BAD_TEST
static int test_item1(void);
static int test_item2(void);
#endif
static int test_item3(void);


#define WAIT_SEC      1
#define WAIT_MAX_SEC 20
#define COUNT_VALUE 300

static long* pSync = NULL;


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_sync_tc4(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    pSync = shmalloc(sizeof(*pSync) * _SHMEM_BARRIER_SYNC_SIZE);
    if (!pSync)
    {
        rc = TC_SETUP_FAIL;
    }

#ifdef BAD_TEST
    /* Equal to shmem_barrier_all */
    if (rc == TC_PASS)
    {
        rc = test_item1();
        log_item(node, 1, rc);
        shmem_barrier_all();
    }

    /* Barrier for all excluding PE#0 */
    if (rc == TC_PASS)
    {
        rc = test_item2();
        log_item(node, 2, rc);
        shmem_barrier_all();
    }
#endif

    /* Massive barrier */
    if (rc == TC_PASS)
    {
        rc = test_item3();
        log_item(node, 3, rc);
        shmem_barrier_all();
    }

    if (pSync)
    {
        shfree(pSync);
    }

    return rc;
}

#ifdef BAD_TEST

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
    int j = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    wait = sys_min(my_proc * WAIT_SEC, WAIT_MAX_SEC);
    expect_value = ( (num_proc - my_proc - 1) > 0 ? (num_proc - my_proc - 1) * WAIT_SEC - 2 : 0 );

    /* This guarantees that PE set initial value before peer change one */
    for ( j = 0; j < _SHMEM_BARRIER_SYNC_SIZE; j++ )
    {
        pSync[j] = _SHMEM_SYNC_VALUE;
    }
    shmem_barrier_all();

    osh_sleep(wait);
    start = time(NULL);

    shmem_barrier(0, 0, num_proc, pSync);

    finish = time(NULL);

    rc = (((finish - start) >= expect_value) ? TC_PASS : TC_FAIL);

    log_trace(OSH_TC, "my_proc = %d wait = %ld limit = %ld actual wait = %ld\n",
                       my_proc, (unsigned long)wait, (unsigned long)expect_value, (unsigned long)(finish - start));

    return rc;
}


static int test_item2(void)
{
    int rc = TC_PASS;
    time_t start;
    time_t finish;
    time_t wait;
    time_t expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    int j = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    if (my_proc > 0)
    {
        wait = sys_min(my_proc * WAIT_SEC, WAIT_MAX_SEC);
        expect_value = ( (num_proc - my_proc - 1) > 0 ? (num_proc - my_proc - 1) * WAIT_SEC - 2 : 0 );
    }
    else
    {
        wait = 0;
        expect_value = 1;
    }

    /* This guarantees that PE set initial value before peer change one */
    for ( j = 0; j < _SHMEM_BARRIER_SYNC_SIZE; j++ )
    {
        pSync[j] = _SHMEM_SYNC_VALUE;
    }
    shmem_barrier_all();

    osh_sleep(wait);
    start = time(NULL);

    shmem_barrier(1, 0, (num_proc - 1), pSync);

    finish = time(NULL);

    if (my_proc > 0)
    {
        rc = (((finish - start) >= expect_value) ? TC_PASS : TC_FAIL);
    }
    else
    {
        rc = (((finish - start) <= expect_value) ? TC_PASS : TC_FAIL);
    }

    log_trace(OSH_TC, "my_proc = %d wait = %ld limit = %ld actual wait = %ld\n",
                       my_proc, (unsigned long)wait, (unsigned long)expect_value, (unsigned long)(finish - start));

    return rc;
}
#endif

static int test_item3(void)
{
    int rc = TC_PASS;
    int* shmem_addr = NULL;
    int my_value = 0;
    int peer_value = 0;
    int expect_value = 0;
    int peer_proc = 0;
    int num_proc = 0;
    int my_proc = 0;
    int i = 0;
    int count_per_pair = 0;
    int j = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    if (shmem_addr)
    {
        peer_proc = ((my_proc == (num_proc - 1)) && (num_proc % 2) ? my_proc : (my_proc % 2 ? my_proc - 1 : my_proc + 1));
        count_per_pair = (my_proc / 2 + 1) * COUNT_VALUE;
        expect_value = count_per_pair;

        /* If the pSync array is initialized at run time, be sure to use some type of synchronization,
            for example, a call to shmem_barrier_all, before calling shmem_barrier for the first time
         */
        for ( j = 0; j < _SHMEM_BARRIER_SYNC_SIZE; j++ )
        {
            pSync[j] = _SHMEM_SYNC_VALUE;
        }
        shmem_barrier_all();

        for (i = 0; i <= count_per_pair; i++)
        {
            *shmem_addr = i;
            shmem_barrier((my_proc / 2 * 2), 0, ((my_proc == (num_proc - 1)) && (num_proc % 2) ? 1 : 2), pSync);
        }

        my_value = *shmem_addr;
        peer_value = shmem_int_g(shmem_addr, peer_proc);
        rc = (expect_value == peer_value ? TC_PASS : TC_FAIL);

        log_trace(OSH_TC, "my(#%d:%d) peer(#%d:%d) expected = %d\n",
                           my_proc, my_value, peer_proc, peer_value, expect_value);
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
