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

#include "osh_atomic_tests.h"

/****************************************************************************
 * Test Case can consitis of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
static int test_item1(void);

#ifdef QUICK_TEST
#define COUNT_VALUE 30
#else
#define COUNT_VALUE 300
#endif
#define CHECK_COUNT_VALUE 1

/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_atomic_tc22(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    if (rc == TC_PASS)
    {
        rc = test_item1();
        log_item(node, 1, rc);
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
    long* shmem_addr = NULL;
    int my_proc = 0;
    int root = 0;
    int i = 0, j = 0;
    long original_value, prev_value, new_value, check_value;
    long *lock_value = 0;

    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    lock_value = shmalloc(sizeof(*lock_value));
    *lock_value = 0;

    shmem_barrier_all();
    if (my_proc < (int)sizeof(long))
    {
        for (i = 0; ((i < COUNT_VALUE) && (rc == TC_PASS)); i++)
        {
            shmem_long_get(&original_value, shmem_addr, 1, root);
            shmem_set_lock(lock_value);
            new_value = my_proc;
            prev_value = shmem_long_cswap(shmem_addr, original_value, new_value, root);
            for (j = 0; j < CHECK_COUNT_VALUE; j++)
            {
                shmem_long_get(&check_value, shmem_addr, 1, root);

                if (check_value != new_value && check_value != prev_value)
                {
                    rc = TC_FAIL;
                    break;
                }
            }
            shmem_clear_lock(lock_value);
        }
    }

    if (shmem_addr)
    {
        shfree(shmem_addr);
    }

    shfree(lock_value);

    return rc;
}
