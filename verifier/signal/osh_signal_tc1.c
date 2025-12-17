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
int osh_signal_tc1(const TE_NODE *node, int argc, const char *argv[])
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
    uint64_t* data_addr = NULL;
    uint64_t* signal_addr = NULL;
    uint64_t expected_data = 0;
    uint64_t expected_signal = 0;
    int my_proc = 0;
    int peer_proc = 0;
    int num_proc = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    data_addr = (uint64_t*)shmalloc(sizeof(*data_addr));
    signal_addr = (uint64_t*)shmalloc(sizeof(*signal_addr));

    if (!data_addr || !signal_addr)
    {
        rc = TC_SETUP_FAIL;
        goto cleanup;
    }

    *data_addr = 0;
    *signal_addr = 0;
    expected_data = 0x1234567890ABCDEFULL + my_proc;
    expected_signal = 0xDEADBEEFULL;

    shmem_barrier_all();

    peer_proc = (my_proc + 1) % num_proc;

    shmem_putmem_signal(data_addr, &expected_data, sizeof(uint64_t),
                       signal_addr, expected_signal, SHMEM_SIGNAL_SET,
                       peer_proc);

    shmem_wait_until(signal_addr, SHMEM_CMP_EQ, expected_signal);

    if (*data_addr != (0x1234567890ABCDEFULL + ((my_proc - 1 + num_proc) % num_proc)))
    {
        rc = TC_FAIL;
        log_debug(OSH_TC, "PE #%d: Expected data = 0x%llx, got = 0x%llx\n",
                 my_proc, (unsigned long long)(0x1234567890ABCDEFULL + ((my_proc - 1 + num_proc) % num_proc)),
                 (unsigned long long)*data_addr);
        goto cleanup;
    }

    if (*signal_addr != expected_signal)
    {
        rc = TC_FAIL;
        log_debug(OSH_TC, "PE #%d: Expected signal = 0x%llx, got = 0x%llx\n",
                 my_proc, (unsigned long long)expected_signal, (unsigned long long)*signal_addr);
    }

cleanup:
    if (data_addr)
        shfree(data_addr);
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

    data_addr = (uint64_t*)shmalloc(sizeof(*data_addr) * 4);
    signal_addr = (uint64_t*)shmalloc(sizeof(*signal_addr));

    if (!data_addr || !signal_addr)
    {
        rc = TC_SETUP_FAIL;
        goto cleanup;
    }

    uint64_t test_data[4] = {0x1111, 0x2222, 0x3333, 0x4444};
    uint64_t signal_val = 0x5555AAAA;

    for (int i = 0; i < 4; i++)
        data_addr[i] = 0;
    *signal_addr = 0;

    shmem_barrier_all();

    if (my_proc == 0 && num_proc > 1)
    {
        shmem_putmem_signal(data_addr, test_data, sizeof(test_data),
                           signal_addr, signal_val, SHMEM_SIGNAL_SET, 1);
    }

    shmem_barrier_all();

    if (my_proc == 1)
    {
        shmem_wait_until(signal_addr, SHMEM_CMP_EQ, signal_val);

        for (int i = 0; i < 4; i++)
        {
            if (data_addr[i] != test_data[i])
            {
                rc = TC_FAIL;
                log_debug(OSH_TC, "PE #%d: data[%d] expected = 0x%llx, got = 0x%llx\n",
                         my_proc, i, (unsigned long long)test_data[i], (unsigned long long)data_addr[i]);
                goto cleanup;
            }
        }

        if (*signal_addr != signal_val)
        {
            rc = TC_FAIL;
            log_debug(OSH_TC, "PE #%d: Expected signal = 0x%llx, got = 0x%llx\n",
                     my_proc, (unsigned long long)signal_val, (unsigned long long)*signal_addr);
        }
    }

cleanup:
    if (data_addr)
        shfree(data_addr);
    if (signal_addr)
        shfree(signal_addr);

    return rc;
}

