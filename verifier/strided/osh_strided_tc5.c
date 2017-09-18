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
#include <pthread.h>
#include "shmem.h"

#include "osh_strided_tests.h"

/****************************************************************************
 * Test Case can consitis of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
static int test_item1(void);




#define TYPE_VALUE  double
#define FUNC_VALUE  shmem_double_iget
#define SIZE_VALUE  sizeof(TYPE_VALUE)
#ifdef QUICK_TEST
#define COUNT_VALUE 10
#else
#define COUNT_VALUE 100
#endif
#define MAX_ARRAY_SIZE  1000


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_strided_tc5(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    /* Get strided values from rank+1%size */
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
    TYPE_VALUE* local_addr = NULL;
    TYPE_VALUE my_value = 0;
    TYPE_VALUE peer_value = 0;
    TYPE_VALUE* expect_value = NULL;
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;
	int tst, sst;
    int max_stride = MAX_ARRAY_SIZE/2-1;

    num_proc = _num_pes();
    my_proc = _my_pe();
    shmem_addr = shmalloc(sizeof(*shmem_addr)*MAX_ARRAY_SIZE);
    local_addr = malloc(sizeof(*local_addr)*MAX_ARRAY_SIZE);
    expect_value = malloc(sizeof(*expect_value)*MAX_ARRAY_SIZE);
    if (shmem_addr)
    {
        INT64_TYPE i = 0;
        INT64_TYPE j = 0;
        int num_to_get;
        my_value = 0;
        size_t odd_pos;
        for (i = 0; (i < COUNT_VALUE) && (rc == TC_PASS); i++)
        {
            tst = (i < max_stride) ? i+1 : max_stride;
            sst = tst;
            num_to_get = MAX_ARRAY_SIZE/tst;
            /* Set my value */
            my_value = (TYPE_VALUE)(my_proc + 1);
            memset(local_addr,0,MAX_ARRAY_SIZE*SIZE_VALUE);
            memset(expect_value,0,MAX_ARRAY_SIZE*SIZE_VALUE);
            for (j = 0; j < MAX_ARRAY_SIZE; j++)
                shmem_addr[j] = my_value;


            /* Define peer and it value */
            peer_proc = (my_proc + 1) % num_proc;
            peer_value = (TYPE_VALUE)(peer_proc + 1);


            /* Define expected value */
            for (j=0; j<num_to_get; j++)
                expect_value[j*tst] = peer_value;
            /* Wait is set instead of barrier to give some time to all PE for setting their values */
            shmem_barrier_all();

            /* Get value from peer */
            FUNC_VALUE(local_addr, shmem_addr,tst,sst,num_to_get,peer_proc);

            if (rc == TC_PASS)
            {
                rc = (compare_buffer((unsigned char*)local_addr, (unsigned char*)expect_value, MAX_ARRAY_SIZE, &odd_pos) ? TC_PASS : TC_FAIL);
            }
            log_debug(OSH_TC, "my(#%d:%lld) peer(#%d:%lld) expected = %lld vs got = %lld\n",
                               my_proc, (INT64_TYPE)my_value, peer_proc, (INT64_TYPE)peer_value, (INT64_TYPE)expect_value[0], (INT64_TYPE)local_addr[0]);

            /* Wait is set instead of barrier to give some time to all PE for setting their values */
            shmem_barrier_all();
        }
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (local_addr)
    {
        free(local_addr);
    }
    if (expect_value)
    {
        free(expect_value);
    }
    if (shmem_addr)
    {
        shfree(shmem_addr);
    }

    return rc;
}
