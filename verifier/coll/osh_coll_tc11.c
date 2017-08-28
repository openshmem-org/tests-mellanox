/*
 * Copyright (c) 2014-2016 Mellanox Technologies, Inc.
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

#include "osh_coll_tests.h"

/****************************************************************************
 * Test Case can consists of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
static int test_item1(void);
static int test_item2(void);


static long* pSync = NULL;

/* Tranlate a group PE index to a global PE rank. */
static int index_to_pe(int group_pe, int pe_start, int pe_stride, int pe_size)
{
    int stride = 1 << pe_stride;

    return group_pe >= pe_size ? -1 : pe_start + group_pe * stride;
}


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_coll_tc11(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    if (rc == TC_PASS)
    {
        pSync = shmalloc(sizeof(*pSync) * SHMEM_ALLTOALL_SYNC_SIZE);
        if (!pSync)
        {
            rc = TC_SETUP_FAIL;
        }
    }

    if (rc == TC_PASS)
    {
        rc = test_item1();
        log_item(node, 1, rc);
        shmem_barrier_all();
    }

    if (rc == TC_PASS)
    {
        rc = test_item2();
        log_item(node, 2, rc);
        shmem_barrier_all();
    }

    if (pSync)
    {
        shfree(pSync);
    }

    return rc;
}


/****************************************************************************
 * Place for Test Item functions
 ***************************************************************************/
static int test_item1(void)
{
    int rc = TC_PASS;
    int32_t *source, *dest;
    int32_t expect_value;
    int i, count, pe;
    int num_proc = 0;
    int my_proc = 0;

    num_proc = shmem_n_pes();
    my_proc = shmem_my_pe();

    count = 2;
    dest = (int32_t *)shmem_malloc(count * num_proc * sizeof(*dest));
    source = (int32_t *)shmem_malloc(count * num_proc * sizeof(*source));

    /* assign source values */
    for (pe = 0; pe < num_proc; pe++) {
        for (i = 0; i < count; i++) {
            source[(pe * count) + i] = my_proc + i;
            dest[(pe * count) + i] = 9999;
        }
    }

    /* This guarantees that PE set initial value before peer change one */
    for ( i = 0; i < SHMEM_ALLTOALL_SYNC_SIZE; i++ )
    {
        pSync[i] = _SHMEM_SYNC_VALUE;
    }
    shmem_barrier_all();

    /* alltoall on all PES */
    shmem_alltoall32(dest, source, count, 0, 0, num_proc, pSync);
    /* verify results */
    for (pe = 0; pe < num_proc; pe++) {
        for (i = 0; i < count; i++) {
            expect_value = i + index_to_pe(pe, 0, 0, num_proc);
            if (dest[(pe * count) + i] != expect_value) {
                rc = TC_FAIL;
                log_debug(OSH_TC, "my#%d ERROR: dest[%d]=%ld, should be %d\n",
                        my_proc, (pe * count) + i, dest[(pe * count) + i],
                        expect_value);
            }
        }
    }
    shmem_barrier_all();
    shmem_free(dest);
    shmem_free(source);

    return rc;
}


static int test_item2(void)
{
    int rc = TC_PASS;
    int64_t *source, *dest;
    int64_t expect_value;
    int i, count, pe;
    int num_proc = 0;
    int my_proc = 0;

    num_proc = shmem_n_pes();
    my_proc = shmem_my_pe();

    count = 2;
    dest = (int64_t *)shmem_malloc(count * num_proc * sizeof(*dest));
    source = (int64_t *)shmem_malloc(count * num_proc * sizeof(*source));

    /* assign source values */
    for (pe = 0; pe < num_proc; pe++) {
        for (i = 0; i < count; i++) {
            source[(pe * count) + i] = my_proc + i;
            dest[(pe * count) + i] = 9999;
        }
    }

    /* This guarantees that PE set initial value before peer change one */
    for ( i = 0; i < SHMEM_ALLTOALL_SYNC_SIZE; i++ )
    {
        pSync[i] = _SHMEM_SYNC_VALUE;
    }
    shmem_barrier_all();

    /* alltoall on all PES */
    shmem_alltoall64(dest, source, count, 0, 0, num_proc, pSync);
    /* verify results */
    for (pe = 0; pe < num_proc; pe++) {
        for (i = 0; i < count; i++) {
            expect_value = i + index_to_pe(pe, 0, 0, num_proc);
            if (dest[(pe * count) + i] != expect_value) {
                rc = TC_FAIL;
                log_debug(OSH_TC, "my#%d ERROR: dest[%d]=%ld, should be %d\n",
                        my_proc, (pe * count) + i, dest[(pe * count) + i],
                        expect_value);
            }
        }
    }
    shmem_barrier_all();
    shmem_free(dest);
    shmem_free(source);

    return rc;
}
