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
static int test_item4(void);
static int test_item5(void);
static int test_item6(void);


#ifdef QUICK_TEST
#define COUNT_VALUE		1000
#else
#define COUNT_VALUE		100000
#endif
#define BUFFER_COUNT	0x1000
#define SHMEM_SYNC_INVALID 	(-77)

static  int static_shmem_addr[BUFFER_COUNT];
static  int static_check_addr[BUFFER_COUNT];


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_data_tc40(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    /* Remote Write: In a Remote Write operation (PUT),
        the initiating (active side) PE is the source and the
        remote (passive side) PE is the target. he active
        side PE speciies the local (active side) PE’s (source)
        memory from which the data will be sent, and
        the remote (passive side) PE’s (target) memory to
        which the data will be written. MP-SHMEM, LCSHMEM, Q-SHMEM, and C-SHMEM all provide the semantic that ater shmem_put returns,
        the data speciied for transfer has been bufered;
        this local completion on the active side does not
        equate to remote completion of the put on the
        passive side. LC-SHMEM, Q-SHMEM, and C-
        SHMEM also provide a non-blocking PUT operation shmem_put_nb which may return before the
        data is bufered for transfer. Local completion of
        non-blocking remote (RMA) writes are guaranteed either when shmem_test_nb returns success or
        shmem_wait_nb returns. Unlike MPI requests, it is
        invalid to wait on a non-blocking operation ater test
        returns success.
    */
    /* destination dynamic address & source local address */
    rc = test_item1();
    log_item(node, 1, rc);
    shmem_barrier_all();

    /* destination static address & source local address */
    if (rc == TC_PASS)
    {
        rc = test_item2();
        log_item(node, 2, rc);
        shmem_barrier_all();
    }

    /* destination dynamic address & source static address */
    if (rc == TC_PASS)
    {
        rc = test_item3();
        log_item(node, 3, rc);
        shmem_barrier_all();
    }

    /* static dynamic address & source static address */
    if (rc == TC_PASS)
    {
        rc = test_item4();
        log_item(node, 4, rc);
        shmem_barrier_all();
    }

    /* destination dynamic address & source dynamic address */
    if (rc == TC_PASS)
    {
        rc = test_item5();
        log_item(node, 5, rc);
        shmem_barrier_all();
    }

    /* static dynamic address & source dynamic address */
    if (rc == TC_PASS)
    {
        rc = test_item6();
        log_item(node, 6, rc);
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
    int* shmem_addr = NULL;
    int* check_addr = NULL;
    int local_check_addr[BUFFER_COUNT];
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;
    int i = 0;
    int count = COUNT_VALUE;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr) * BUFFER_COUNT);
    check_addr = local_check_addr;

    shmem_barrier_all();
    if (shmem_addr)
    {
        memset(check_addr, 0, sizeof(*check_addr) * BUFFER_COUNT);
        memset(shmem_addr, 0, sizeof(*shmem_addr) * BUFFER_COUNT);
        peer_proc = my_proc + 1;
        if ((my_proc == (num_proc - 1)) && !(my_proc % 2))
        {
            /* do nothing */
        }
        else if (my_proc % 2)
        {
            i = 0;
            while ((0 <= *check_addr) && (*check_addr < count))
            {
                shmem_int_get(check_addr, shmem_addr, 1, my_proc);
                i++;
            }
            rc = (*check_addr == count ? TC_PASS : TC_FAIL);
        }
        else
        {
            for (i = 0; i < count; i++)
            {
                *check_addr = i;
                shmem_int_put(shmem_addr, check_addr, BUFFER_COUNT, peer_proc);
                *check_addr = SHMEM_SYNC_INVALID;
                usleep(my_proc % 10 + 1);
            }
            shmem_fence();
            *check_addr = count;
            shmem_int_put(shmem_addr, check_addr, BUFFER_COUNT, peer_proc);
        }
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    log_debug(OSH_TC, "my#%d peer#%d value=%d expected=%d i=%d\n", my_proc, peer_proc, *check_addr, count, i);

    shmem_barrier_all();

    if (shmem_addr)
    {
        shfree(shmem_addr);
    }

    return rc;
}


static int test_item2(void)
{
    int rc = TC_PASS;
    int* shmem_addr = NULL;
    int* check_addr = NULL;
    int local_check_addr[BUFFER_COUNT];
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;
    int i = 0;
    int count = COUNT_VALUE;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = static_shmem_addr;
    check_addr = local_check_addr;

    shmem_barrier_all();
    if (shmem_addr)
    {
        memset(check_addr, 0, sizeof(*check_addr) * BUFFER_COUNT);
        memset(shmem_addr, 0, sizeof(*shmem_addr) * BUFFER_COUNT);
        peer_proc = my_proc + 1;
        if ((my_proc == (num_proc - 1)) && !(my_proc % 2))
        {
            /* do nothing */
        }
        else if (my_proc % 2)
        {
            i = 0;
            while ((0 <= *check_addr) && (*check_addr < count))
            {
                shmem_int_get(check_addr, shmem_addr, 1, my_proc);
                i++;
            }
            rc = (*check_addr == count ? TC_PASS : TC_FAIL);
        }
        else
        {
            for (i = 0; i < count; i++)
            {
                *check_addr = i;
                shmem_int_put(shmem_addr, check_addr, BUFFER_COUNT, peer_proc);
                *check_addr = SHMEM_SYNC_INVALID;
                usleep(my_proc % 10 + 1);
            }
            shmem_fence();
            *check_addr = count;
            shmem_int_put(shmem_addr, check_addr, BUFFER_COUNT, peer_proc);
        }
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    log_debug(OSH_TC, "my#%d peer#%d value=%d expected=%d i=%d\n", my_proc, peer_proc, *check_addr, count, i);

    shmem_barrier_all();

    return rc;
}


static int test_item3(void)
{
    int rc = TC_PASS;
    int* shmem_addr = NULL;
    int* check_addr = NULL;
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;
    int i = 0;
    int count = COUNT_VALUE;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr) * BUFFER_COUNT);
    check_addr = static_check_addr;

    shmem_barrier_all();
    if (shmem_addr)
    {
        memset(check_addr, 0, sizeof(*check_addr) * BUFFER_COUNT);
        memset(shmem_addr, 0, sizeof(*shmem_addr) * BUFFER_COUNT);
        peer_proc = my_proc + 1;
        if ((my_proc == (num_proc - 1)) && !(my_proc % 2))
        {
            /* do nothing */
        }
        else if (my_proc % 2)
        {
            i = 0;
            while ((0 <= *check_addr) && (*check_addr < count))
            {
                shmem_int_get(check_addr, shmem_addr, 1, my_proc);
                i++;
            }
            rc = (*check_addr == count ? TC_PASS : TC_FAIL);
        }
        else
        {
            for (i = 0; i < count; i++)
            {
                *check_addr = i;
                shmem_int_put(shmem_addr, check_addr, BUFFER_COUNT, peer_proc);
                *check_addr = SHMEM_SYNC_INVALID;
                usleep(my_proc % 10 + 1);
            }
            shmem_fence();
            *check_addr = count;
            shmem_int_put(shmem_addr, check_addr, BUFFER_COUNT, peer_proc);
        }
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    log_debug(OSH_TC, "my#%d peer#%d value=%d expected=%d i=%d\n", my_proc, peer_proc, *check_addr, count, i);

    shmem_barrier_all();

    if (shmem_addr)
    {
        shfree(shmem_addr);
    }

    return rc;
}


static int test_item4(void)
{
    int rc = TC_PASS;
    int* shmem_addr = NULL;
    int* check_addr = NULL;
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;
    int i = 0;
    int count = COUNT_VALUE;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = static_shmem_addr;
    check_addr = static_check_addr;

    shmem_barrier_all();
    if (shmem_addr)
    {
        memset(check_addr, 0, sizeof(*check_addr) * BUFFER_COUNT);
        memset(shmem_addr, 0, sizeof(*shmem_addr) * BUFFER_COUNT);
        peer_proc = my_proc + 1;
        if ((my_proc == (num_proc - 1)) && !(my_proc % 2))
        {
            /* do nothing */
        }
        else if (my_proc % 2)
        {
            i = 0;
            while ((0 <= *check_addr) && (*check_addr < count))
            {
                shmem_int_get(check_addr, shmem_addr, 1, my_proc);
                i++;
            }
            rc = (*check_addr == count ? TC_PASS : TC_FAIL);
        }
        else
        {
            for (i = 0; i < count; i++)
            {
                *check_addr = i;
                shmem_int_put(shmem_addr, check_addr, BUFFER_COUNT, peer_proc);
                *check_addr = SHMEM_SYNC_INVALID;
                usleep(my_proc % 10 + 1);
            }
            shmem_fence();
            *check_addr = count;
            shmem_int_put(shmem_addr, check_addr, BUFFER_COUNT, peer_proc);
        }
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    log_debug(OSH_TC, "my#%d peer#%d value=%d expected=%d i=%d\n", my_proc, peer_proc, *check_addr, count, i);

    shmem_barrier_all();

    return rc;
}


static int test_item5(void)
{
    int rc = TC_PASS;
    int* shmem_addr = NULL;
    int* check_addr = NULL;
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;
    int i = 0;
    int count = COUNT_VALUE;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = shmalloc(sizeof(*shmem_addr) * BUFFER_COUNT);
    check_addr = shmalloc(sizeof(*check_addr) * BUFFER_COUNT);

    shmem_barrier_all();
    if (shmem_addr && check_addr)
    {
        memset(check_addr, 0, sizeof(*check_addr) * BUFFER_COUNT);
        memset(shmem_addr, 0, sizeof(*shmem_addr) * BUFFER_COUNT);
        peer_proc = my_proc + 1;
        if ((my_proc == (num_proc - 1)) && !(my_proc % 2))
        {
            /* do nothing */
        }
        else if (my_proc % 2)
        {
            i = 0;
            while ((0 <= *check_addr) && (*check_addr < count))
            {
                shmem_int_get(check_addr, shmem_addr, 1, my_proc);
                i++;
            }
            rc = (*check_addr == count ? TC_PASS : TC_FAIL);
        }
        else
        {
            for (i = 0; i < count; i++)
            {
                *check_addr = i;
                shmem_int_put(shmem_addr, check_addr, BUFFER_COUNT, peer_proc);
                *check_addr = SHMEM_SYNC_INVALID;
                usleep(my_proc % 10 + 1);
            }
            shmem_fence();
            *check_addr = count;
            shmem_int_put(shmem_addr, check_addr, BUFFER_COUNT, peer_proc);
        }
        log_debug(OSH_TC, "my#%d peer#%d value=%d expected=%d i=%d\n", my_proc, peer_proc, *check_addr, count, i);
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }


    shmem_barrier_all();

    if (shmem_addr)
    {
        shfree(shmem_addr);
    }

    if (check_addr)
    {
        shfree(check_addr);
    }

    return rc;
}


static int test_item6(void)
{
    int rc = TC_PASS;
    int* shmem_addr = NULL;
    int* check_addr = NULL;
    int num_proc = 0;
    int my_proc = 0;
    int peer_proc = 0;
    int i = 0;
    int count = COUNT_VALUE;

    num_proc = _num_pes();
    my_proc = _my_pe();

    shmem_addr = static_shmem_addr;
    check_addr = shmalloc(sizeof(*check_addr) * BUFFER_COUNT);

    shmem_barrier_all();
    if (shmem_addr && check_addr)
    {
        memset(check_addr, 0, sizeof(*check_addr) * BUFFER_COUNT);
        memset(shmem_addr, 0, sizeof(*shmem_addr) * BUFFER_COUNT);
        peer_proc = my_proc + 1;
        if ((my_proc == (num_proc - 1)) && !(my_proc % 2))
        {
            /* do nothing */
        }
        else if (my_proc % 2)
        {
            i = 0;
            while ((0 <= *check_addr) && (*check_addr < count))
            {
                shmem_int_get(check_addr, shmem_addr, 1, my_proc);
                i++;
            }
            rc = (*check_addr == count ? TC_PASS : TC_FAIL);
        }
        else
        {
            for (i = 0; i < count; i++)
            {
                *check_addr = i;
                shmem_int_put(shmem_addr, check_addr, BUFFER_COUNT, peer_proc);
                *check_addr = SHMEM_SYNC_INVALID;
                usleep(my_proc % 10 + 1);
            }
            shmem_fence();
            *check_addr = count;
            shmem_int_put(shmem_addr, check_addr, BUFFER_COUNT, peer_proc);
        }
        log_debug(OSH_TC, "my#%d peer#%d value=%d expected=%d i=%d\n", my_proc, peer_proc, *check_addr, count, i);
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }


    shmem_barrier_all();

    if (check_addr)
    {
        shfree(check_addr);
    }

    return rc;
}
