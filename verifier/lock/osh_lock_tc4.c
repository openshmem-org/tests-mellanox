/*
 * Copyright (c) 2014      Mellanox Technologies, Inc.
 *                         All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include <pthread.h>
#include "osh_def.h"
#include "osh_cmn.h"
#include "osh_log.h"

#include "shmem.h"

#include "osh_lock_tests.h"

/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/

/*
  The following code is taken from oshmem/mca/atomic/basic/atomic_basic_module.c
  This is a bad practice but I did not find a quick way to test it directly
*/

static char *atomic_lock_sync;
static int *atomic_lock_turn;
static char *local_lock_sync;
static int *local_lock_turn;

enum 
{
    ATOMIC_LOCK_IDLE = 0, 
    ATOMIC_LOCK_WAITING = 1, 
    ATOMIC_LOCK_ACTIVE = 2
};

#define OSHMEM_SUCCESS 0
#define OSHMEM_ERR_OUT_OF_RESOURCE 1

/*
 * Initial query function that is invoked during initialization, allowing
 * this module to indicate what level of thread support it provides.
 */
static int mca_atomic_basic_init(int enable_progress_threads,
                          int enable_threads)
{
    int rc = OSHMEM_SUCCESS;
    void* ptr = NULL;
    int num_pe = _num_pes();

    UNREFERENCED_PARAMETER(enable_progress_threads);
    UNREFERENCED_PARAMETER(enable_threads);

    ptr = shmalloc(num_pe * sizeof(char));
    if(rc == OSHMEM_SUCCESS)
    {
        atomic_lock_sync = (char*)ptr;
        memset(atomic_lock_sync, ATOMIC_LOCK_IDLE, sizeof(char) * num_pe);

        ptr = shmalloc(sizeof(int));
        if(rc == OSHMEM_SUCCESS)
        {
            atomic_lock_turn = (int*)ptr;
            *atomic_lock_turn = 0;
            if(rc == OSHMEM_SUCCESS)
            {
                local_lock_sync = (char*)malloc(num_pe * sizeof(char));
                local_lock_turn = (int*)malloc(sizeof(int));
                if (!local_lock_sync || !local_lock_turn)
                {
                    rc = OSHMEM_ERR_OUT_OF_RESOURCE;
                }
                else
                {
                    memcpy((void*)local_lock_sync, (void*)atomic_lock_sync, sizeof(char) * num_pe);
                    *local_lock_turn = *atomic_lock_turn;
                }
            }
        }
    }

    return rc;
}


static int mca_atomic_basic_finalize(void)
{
    void* ptr = NULL;

    ptr = (void*)atomic_lock_sync;
    shfree(ptr);
    atomic_lock_sync = NULL;

    ptr = (void*)atomic_lock_turn;
    shfree(ptr);
    atomic_lock_turn = NULL;

    if (local_lock_sync)
    {
        free((void*)local_lock_sync);
        local_lock_sync = NULL;
    }

    if (local_lock_turn)
    {
        free((void*)local_lock_turn);
        local_lock_turn = NULL;
    }

    return OSHMEM_SUCCESS;
}

static void atomic_basic_lock(int pe)
{
    int index = -1;
    int me = shmem_my_pe();
    int num_pe = _num_pes();
    char lock_required = ATOMIC_LOCK_WAITING;
    char lock_active = ATOMIC_LOCK_ACTIVE;
    int root_pe = pe;

    do
    {
        /* announce that we need the resource */
        do
        {
            shmem_putmem((void*)(atomic_lock_sync + me), (void*)&lock_required, sizeof(lock_required), root_pe);
            shmem_fence(); /* quiet */
            shmem_getmem((void*)local_lock_sync, (void*)atomic_lock_sync, num_pe * sizeof(*atomic_lock_sync), root_pe);
        } while (local_lock_sync[me] != lock_required);

        shmem_getmem((void*)&index, (void*)atomic_lock_turn, sizeof(index), root_pe);
        while (index != me)
        {
            if (local_lock_sync[index] != ATOMIC_LOCK_IDLE) 
            {
                shmem_getmem((void*)&index, (void*)atomic_lock_turn, sizeof(index), root_pe);
                shmem_getmem((void*)local_lock_sync, (void*)atomic_lock_sync, num_pe * sizeof(*atomic_lock_sync), root_pe);
            }
            else
            {
                index = (index + 1) % num_pe;
            }
        }

        /* now tentatively claim the resource */
        do 
        {
            shmem_putmem((void*)(atomic_lock_sync + me), (void*)&lock_active, sizeof(lock_active), root_pe);
            shmem_fence(); /* quiet */
            shmem_getmem((void*)local_lock_sync, (void*)atomic_lock_sync, num_pe * sizeof(*atomic_lock_sync), root_pe);
        } while (local_lock_sync[me] != lock_active);

        index = 0;
        while((index < num_pe)&&
             ((index == me)||(local_lock_sync[index] != ATOMIC_LOCK_ACTIVE)))
        {
            index = index + 1;
        }

        shmem_getmem((void*)local_lock_turn, (void*)atomic_lock_turn, sizeof(*atomic_lock_turn), root_pe);
    } while (!((index >= num_pe) && 
               ((*local_lock_turn == me)||(local_lock_sync[*local_lock_turn] == ATOMIC_LOCK_IDLE))));

    shmem_putmem((void*)atomic_lock_turn, (void*)&me, sizeof(me), root_pe);
    shmem_fence(); /* quiet */
}


static void atomic_basic_unlock(int pe)
{
    int index = -1;
    int me = shmem_my_pe();
    int num_pe = _num_pes();
    char lock_idle = ATOMIC_LOCK_IDLE;
    int root_pe = pe;

    shmem_getmem((void*)local_lock_sync, (void*)atomic_lock_sync, num_pe * sizeof(*atomic_lock_sync), root_pe);
    shmem_getmem((void*)&index, (void*)atomic_lock_turn, sizeof(index), root_pe);

    do
    {
        index = (index + 1) % num_pe;
    } while (local_lock_sync[index] == ATOMIC_LOCK_IDLE);

    shmem_putmem((void*)atomic_lock_turn, (void*)&index, sizeof(index), root_pe);

    do
    {
        shmem_putmem((void*)(atomic_lock_sync + me), (void*)&lock_idle, sizeof(lock_idle), root_pe);
        shmem_fence(); /* quiet */
        shmem_getmem((void*)local_lock_sync, (void*)atomic_lock_sync, num_pe * sizeof(*atomic_lock_sync), root_pe);
    } while (local_lock_sync[me] != lock_idle);
}

static int test_item1(void);
static int test_item2(void);
//static int test_item3(void);

int osh_lock_tc4(const TE_NODE *node, int argc, const char *argv[])
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
/*
    if (rc == TC_PASS)
    {
        rc = test_item3();
        log_item(node, 3, rc);
        shmem_barrier_all();
    }
*/
    return rc;
}


static int test_item1()
{
    int me = shmem_my_pe();
    int num_pe = _num_pes();
    int res = TC_PASS;
    int writer = 0;
    const int number_of_iterations = 2; //num_pe - 1;
    const int number_of_write_attempts = 1;//00;
    int *test_variable = shmalloc(sizeof(int) * 1);

    mca_atomic_basic_init(0, 0);

    if (num_pe >= 2)
    {
        for (writer = 1; writer <= number_of_iterations; writer++)
        {
            if (writer == me)
            {
                atomic_basic_lock(0);

                shmem_int_put(test_variable, &me, 1, 0);
                shmem_quiet();
                //usleep(10000);
                shmem_int_get(test_variable, test_variable, 1, 0);
                if (res == TC_PASS && *test_variable != me)
                {
                    res = TC_FAIL;
                }
                atomic_basic_unlock(0);
            }
            else
            {
                int iter = 0;
                for (iter = 0; iter < number_of_write_attempts; iter++)
                {
                    atomic_basic_lock(0);
                    shmem_int_put(test_variable, &me, 1, 0);
                    atomic_basic_unlock(0);
                }
            }

            shmem_barrier_all();
        }
    }

    shfree(test_variable);

    mca_atomic_basic_finalize();

    return res;
}

static int test_item2()
{
    int *remote_pe = shmalloc(sizeof(int));
    int number_of_iterations = 5;
    int number_of_checks = 2;
    int i = 0, j = 0;
    int my_pe = _my_pe();
    int status = TC_PASS;

    mca_atomic_basic_init(0, 0);

    shmem_barrier_all();

    for (i = 0; i < number_of_iterations; i++)
    {

        atomic_basic_lock(0);
        shmem_int_p(remote_pe, my_pe, 0);
        shmem_quiet();
        for (j = 0; j < number_of_checks; j++)
        {
            int remote_value = 0;
            shmem_int_get(&remote_value, remote_pe, 1, 0);
            if (my_pe == remote_value)
            {
                break;
            }
            //usleep(1);
        }
        for (j = 0; j < number_of_checks; j++)
        { 
            int real_pe = 0;
            shmem_int_get(&real_pe, remote_pe, 1, 0);
            if (my_pe != real_pe)
            {
                status = TC_FAIL;
                break;
            }
            //usleep(1);
        }
        atomic_basic_unlock(0);
    }

    shmem_barrier_all();
    shfree(remote_pe);
    mca_atomic_basic_finalize();

    return status;
}
/*
static int test_item3(void)
{
    int rc = TC_PASS;
    long* shmem_addr = NULL;
    int num_proc = 0;
    int my_proc = 0;
    int root = 0;
    int i = 0, j = 0;
    long original_value, prev_value, new_value, check_value;
    long *lock_value = 0;

    num_proc = _num_pes();
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
                    printf("WE HAVE ERROR HERE!\n");
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
*/
