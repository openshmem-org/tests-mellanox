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
static int test_item2(void);
static int test_item3(void);


#define TYPE_VALUE  double
#define FUNC_VALUE  shmem_double_swap
#define DEFAULT_VALUE  (-1)
#ifdef QUICK_TEST
#define COUNT_VALUE 10
#else
#define COUNT_VALUE 100
#endif

static long __cycle_count = COUNT_VALUE;


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_atomic_tc6(const TE_NODE *node, int argc, const char *argv[])
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

    if (rc == TC_PASS)
    {
        rc = test_item2();
        log_item(node, 2, rc);
        shmem_barrier_all();
    }

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
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;
    int i = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    if (shmem_addr)
    {
        TYPE_VALUE value = 0;

        /* Store my value */
        my_value = (TYPE_VALUE)my_proc;
        *shmem_addr = DEFAULT_VALUE;

        /* Define peer */
        peer_proc = (my_proc + 1) % num_proc;

        /* Define expected value */
        expect_value = (TYPE_VALUE)(( my_proc == 0 ? (num_proc - 1) : (my_proc - 1) ) + (__cycle_count - 1));

        shmem_barrier_all();
        for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++)
        {
            value = FUNC_VALUE(shmem_addr, (my_value + i), peer_proc);
            if  ( ((i >0 ) && (!sys_fcompare(value, my_value + i - 1))) || ((i == 0) && (!sys_fcompare(value, DEFAULT_VALUE))) )
            {
                break;
            }
        }
        shmem_barrier_all();

        value = *shmem_addr;
        rc = (sys_fcompare(expect_value, value) ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my(#%d:%Lf) expected = %Lf vs got = %Lf\n",
                           my_proc, (long double)my_value, (long double)expect_value, (long double)value);
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
    static TYPE_VALUE shmem_value = 0;
    TYPE_VALUE* shmem_addr = &shmem_value;
    TYPE_VALUE my_value = 0;
    TYPE_VALUE expect_value = 0;
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;
    int i = 0;

    num_proc = _num_pes();
    my_proc = _my_pe();

    {
        TYPE_VALUE value = 0;

        /* Store my value */
        my_value = (TYPE_VALUE)my_proc;
        *shmem_addr = DEFAULT_VALUE;

        /* Define peer */
        peer_proc = (my_proc + 1) % num_proc;

        /* Define expected value */
        expect_value = (TYPE_VALUE)(( my_proc == 0 ? (num_proc - 1) : (my_proc - 1) ) + (__cycle_count - 1));

        shmem_barrier_all();
        for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++)
        {
            value = FUNC_VALUE(shmem_addr, (my_value + i), peer_proc);
            if  ( ((i >0 ) && (!sys_fcompare(value, my_value + i - 1))) || ((i == 0) && (!sys_fcompare(value, DEFAULT_VALUE))) )
            {
                break;
            }
        }
        shmem_barrier_all();

        value = *shmem_addr;
        rc = (sys_fcompare(expect_value, value) ? TC_PASS : TC_FAIL);

        log_debug(OSH_TC, "my(#%d:%Lf) expected = %Lf vs got = %Lf\n",
                           my_proc, (long double)my_value, (long double)expect_value, (long double)value);
    }

    return rc;
}

static int test_item3(void)
{
    int rc = TC_PASS;
    TYPE_VALUE* shmem_addr = NULL;
    TYPE_VALUE my_value = 0;
    TYPE_VALUE* check_arr = NULL;
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    int flag = 0;
    int missed_values = 0;
    static long* pSync = NULL;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr));
    check_arr = shmalloc(sizeof(*check_arr) * num_proc);

    pSync = shmalloc(sizeof(*pSync) * _SHMEM_COLLECT_SYNC_SIZE);
    for (i = 0; i < _SHMEM_COLLECT_SYNC_SIZE; i++) {
        pSync[i] = _SHMEM_SYNC_VALUE;
    }

    if (shmem_addr && pSync && check_arr)
    {
        static TYPE_VALUE value = 0;

        /* Store my value */
        my_value = (TYPE_VALUE)my_proc;
        *shmem_addr = DEFAULT_VALUE;

        shmem_barrier_all();
        for (i = 0; (i < __cycle_count) && (rc == TC_PASS); i++)
        {
            missed_values = 0;
            my_value = (TYPE_VALUE)my_proc;

            value = FUNC_VALUE(shmem_addr, my_value, peer_proc);

            shmem_barrier_all();
            shmem_collect32(check_arr, &value, (sizeof(value) + 3 ) / 4, 0, 0, num_proc, pSync);
            shmem_barrier_all();
            for (j = 0; j < num_proc ; j++)
            {
                flag = 0;
                for (k = 0; k < num_proc; k++)
                {
                    if (sys_fcompare(check_arr[k], j))
                    {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 0)
                {
                    missed_values++;
                }
                if (missed_values > 1)
                {
                    rc = TC_FAIL;
                    break;
                }
            }
        }
        shmem_barrier_all();

        log_debug(OSH_TC, "my(#%d:%lld) missed_values expected = 1 vs missed_values = %d\n",
                           my_proc, (INT64_TYPE)my_value, missed_values);
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (shmem_addr)
    {
        shfree(shmem_addr);
    }
    if (pSync)
    {
        shfree(pSync);
    }

    return rc;
}
