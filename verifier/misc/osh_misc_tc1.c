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

#include "osh_misc_tests.h"

/****************************************************************************
 * Test Case can consitis of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
static int test_item1(void);

#define ARRAY_SIZE 10000
#define TRY_SIZE 100

/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_misc_tc1(const TE_NODE *node, int argc, const char *argv[])
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
    int myPe = shmem_my_pe();
	int myPeer = myPe + ( myPe % 2 ? -1 : 1 ) ;
    int nPe = shmem_n_pes();
    int remainderPe = nPe - (nPe % 2);

    static int statArray[ARRAY_SIZE];
    int* dynamicArray = shmalloc( ARRAY_SIZE * sizeof(int) );

    int iterate;
    for (iterate = 0; iterate < ARRAY_SIZE; iterate++)
    {
        if (myPe != remainderPe)
        {
            int tryIterate;
            int putNum, getNum;

            for (tryIterate = 0; tryIterate < TRY_SIZE; tryIterate++)
            {
                putNum = iterate + myPe;
                shmem_int_put(&statArray[iterate], &putNum, 1, myPeer);
                shmem_int_put(&dynamicArray[iterate], &putNum, 1, myPeer);
            }

            shmem_fence();
            shmem_int_get(&getNum, &statArray[iterate], 1, myPeer);
            if (getNum != putNum)
            {
                rc = TC_FAIL;
            }

            shmem_int_get(&getNum, &dynamicArray[iterate], 1, myPeer);
            if (getNum != putNum)
            {
                rc = TC_FAIL;
            }
        }

        shmem_barrier_all();
    }

    shfree(dynamicArray);
    return rc;
}
