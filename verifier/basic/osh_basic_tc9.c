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


#define WAIT_SEC  5


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_basic_tc9(const TE_NODE *node, int argc, const char *argv[])
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

/* todo: use relative number calculated from heap size, like 80% from heap */
#define HEAP_USAGE_PERCENT (0.8)
#define MAX_SIZE (1024 * 1024)

static int test_item1(void)
{
    int rc = TC_PASS;
    int num_proc = 0;
    int my_proc = 0;
    int peer;
    int size;
    char *buf;
    int test_byte, expected_val;
    int max_heap_size_per_proc;

    num_proc = _num_pes();
    my_proc = _my_pe();
    peer = (my_proc + 1) % num_proc;

    max_heap_size_per_proc = 1L << (sys_log2((memheap_size() * HEAP_USAGE_PERCENT)/ num_proc) - 1);
    max_heap_size_per_proc = (max_heap_size_per_proc > MAX_SIZE) ? MAX_SIZE : max_heap_size_per_proc;
    buf = (char *)shmalloc(max_heap_size_per_proc * num_proc);
    if (!buf)
    {
        log_error(OSH_TC, "shmalloc(%d)\n", max_heap_size_per_proc * num_proc);
        return TC_SETUP_FAIL;
    }

    size = 1L << sys_log2(num_proc);
    size = ((size - 2) > 0) ? size : 4;
    log_debug(OSH_TC, "%d: buf = %p size=%d\n", my_proc, buf, size);
    for (; size <= max_heap_size_per_proc; size *=2)
    {
        expected_val = 1 + my_proc % (size - 2);
        memset(buf + max_heap_size_per_proc * my_proc, expected_val, max_heap_size_per_proc);
        log_debug(OSH_TC, "\n%d: b4 barrier size = %d\n", my_proc, size);
        shmem_barrier_all();
        log_debug(OSH_TC, "%d: b4 putmem size = %d  %p -> %p\n", my_proc, size,
                buf+max_heap_size_per_proc*my_proc, buf + max_heap_size_per_proc * my_proc);
        shmem_putmem(buf+max_heap_size_per_proc*my_proc, buf+max_heap_size_per_proc*my_proc, size, peer);
        shmem_fence();
        test_byte = 0;
        log_debug(OSH_TC, "%d: b4 getmem size = %d\n %p <- %p ", my_proc, size,
                &test_byte,
                buf+max_heap_size_per_proc*peer + size - 1
                );
        shmem_getmem(&test_byte, buf+max_heap_size_per_proc*my_proc + size - 1, 1, peer);

        log_debug(OSH_TC, "%d: after getmem size = %d result=%x\n", my_proc, size, test_byte);
        if (test_byte != (expected_val & 0xff))
        {
            log_error(OSH_TC, "fence failed at size %d got = %x expected = %x\n", size, test_byte, (expected_val & 0xff));
            rc = TC_FAIL;
        }

    }

    shfree(buf);
    log_debug(OSH_TC, rc == TC_PASS? "passed" : "failed");
    return rc;
}
