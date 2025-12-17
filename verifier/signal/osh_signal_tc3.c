/*
 * Copyright (c) 2024      NVIDIA Corporation.
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

#include "osh_signal_tests.h"

/****************************************************************************
 * Test Case can consist of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
static int test_item1(void);
static int test_item2(void);


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_signal_tc3(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    rc = test_item1();
    log_item(node, 1, rc);
    shmem_barrier_all();

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
    uint64_t* signal_addr = NULL;
    uint64_t test_value = 0xFEEDFACEULL;
    uint64_t fetched_value = 0;
    int my_proc = 0;

    my_proc = _my_pe();

    signal_addr = (uint64_t*)shmalloc(sizeof(*signal_addr));

    if (!signal_addr)
    {
        rc = TC_SETUP_FAIL;
        goto cleanup;
    }

    *signal_addr = test_value;
    shmem_barrier_all();

    fetched_value = shmem_signal_fetch(signal_addr);

    if (fetched_value != test_value)
    {
        rc = TC_FAIL;
        log_debug(OSH_TC, "PE #%d: Expected signal = 0x%llx, got = 0x%llx\n",
                 my_proc, (unsigned long long)test_value, (unsigned long long)fetched_value);
    }

cleanup:
    if (signal_addr)
        shfree(signal_addr);

    return rc;
}


static int test_item2(void)
{
    int rc = TC_PASS;
    uint64_t* data_addr = NULL;
    uint64_t* signal_addr = NULL;
    int my_proc = 0;
    int num_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    if (num_proc < 2)
        return TC_PASS;

    data_addr = (uint64_t*)shmalloc(sizeof(*data_addr));
    signal_addr = (uint64_t*)shmalloc(sizeof(*signal_addr));

    if (!data_addr || !signal_addr)
    {
        rc = TC_SETUP_FAIL;
        goto cleanup;
    }

    uint64_t test_data = 0x12345678ULL;
    uint64_t signal_value = 0xAAAABBBBULL;

    *data_addr = 0;
    *signal_addr = 0;

    shmem_barrier_all();

    if (my_proc == 0)
    {
        shmem_putmem_signal(data_addr, &test_data, sizeof(uint64_t),
                           signal_addr, signal_value, SHMEM_SIGNAL_SET, 1);
    }

    shmem_barrier_all();

    if (my_proc == 1)
    {
        uint64_t fetched_signal = 0;

        shmem_wait_until(signal_addr, SHMEM_CMP_NE, 0);
        fetched_signal = shmem_signal_fetch(signal_addr);

        if (fetched_signal != signal_value)
        {
            rc = TC_FAIL;
            log_debug(OSH_TC, "PE #%d: Expected signal = 0x%llx, got = 0x%llx\n",
                     my_proc, (unsigned long long)signal_value, (unsigned long long)fetched_signal);
            goto cleanup;
        }

        if (*data_addr != test_data)
        {
            rc = TC_FAIL;
            log_debug(OSH_TC, "PE #%d: Expected data = 0x%llx, got = 0x%llx\n",
                     my_proc, (unsigned long long)test_data, (unsigned long long)*data_addr);
        }
    }

cleanup:
    if (data_addr)
        shfree(data_addr);
    if (signal_addr)
        shfree(signal_addr);

    return rc;
}

