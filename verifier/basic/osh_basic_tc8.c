// Copyright (c) 2014      Mellanox Technologies, Inc.
//                         All rights reserved
// $COPYRIGHT$
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <shmem.h>

#include "osh_def.h"
#include "osh_cmn.h"
#include "osh_log.h"
#include "rnd_mt.h"
#include "osh_basic_tests.h"

#ifdef QUICK_TEST
#define TABLE_LENGTH                        (10)
#define ITERATIONS_CONST                    (2)
#else
#define TABLE_LENGTH                        (100)
#define ITERATIONS_CONST                    (10)
#endif
#define PRETEST_CONST                       (4)
#define POTENTIAL_MAX_SIZE(heap_size)       (heap_size/2)
#define SHMEMALIGN_BOUNDRY                  (2)
#define INITIAL_SIZE_FOR_SHREALLOC          (16)
#define MAX_ALLOC_CONST                     (10)
#define HEAP_PERCENT                        (0.8)
#define MIN_ALLOC_SIZE_IN_BYTES             (8)
enum errors {
    ERROR_SHMALLOC          = 1,
    ERROR_SHREALLOC         = 1 << 1,
    ERROR_SHMEMALIGN        = 1 << 2,
    ERROR_TABLESHMALLOC     = 1 << 3,
    ERROR_TABLESHREALLOC    = 1 << 4,
    ERROR_TABLESHMEMALIGN   = 1 << 5,
    ERROR_MAXALLOC          = 1 << 6,
    ERROR_LAST              = 1 << 7
};

random_mt_state_t rnd;
static void error_handler (int err)
{
    log_error(OSH_TC, "The following functions result in an error:\n");
    if ((err & ERROR_SHMALLOC) == ERROR_SHMALLOC) {
        log_error(OSH_TC, "testing_shmalloc\n");
    }
    if ((err & ERROR_SHREALLOC) == ERROR_SHREALLOC) {
        log_error(OSH_TC, "testing_shrealloc\n");
    }
    if ((err & ERROR_SHMEMALIGN) == ERROR_SHMEMALIGN) {
        log_error(OSH_TC, "testing_shmemalign\n ");
    }
    if ((err & ERROR_TABLESHMALLOC) == ERROR_TABLESHMALLOC) {
        log_error(OSH_TC, "table_shmalloc\n ");
    }
    if ((err & ERROR_TABLESHREALLOC) == ERROR_TABLESHREALLOC) {
        log_error(OSH_TC, "table_shrealloc\n ");
    }
    if ((err & ERROR_TABLESHMEMALIGN) == ERROR_TABLESHMEMALIGN) {
        log_error(OSH_TC, "table_shmemalign.\n ");
    }
    if ((err & ERROR_MAXALLOC) == ERROR_MAXALLOC) {
        log_error(OSH_TC, "Maximal allocation size is not the heap size.\n");
    }
}

static long int align_size (long int size)
{
    long int res;
    int count = -1, sum = 0;
    while (0 != size) {
        sum += size & 1;
        size >>= 1;
        count++;
    }
    if (sum != 1) {
        res = 1 << (count + 1);
    } else {
        res = 1 << count;
    }
    return res;
}

static long int binary_search_size (long int max_alloc_size)
{
    void *p;
    long int max_size, mid_val, low, high;
    max_size = max_alloc_size;
    low = max_size;
    high = max_size;
    p = shmalloc (max_size);
    while (p != NULL) {
        shfree (p);
        low = max_size;
        max_size = 2 * max_size;
        high = max_size;
        p = shmalloc (max_size);
    }
    while (high > low + 1) {
        mid_val = (long)floor ((double)((high + low) / 2));
        p = shmalloc (mid_val);
        if (p == NULL) {
            high = mid_val;
        } else {
            shfree (p);
            low = mid_val;
        }
    }
    return low;
}

static void free_alloc_table (void *allocTable[])
{
    size_t k;
    for (k = 0; k < TABLE_LENGTH; k++) {
        if (allocTable[k] != NULL) {
            shfree (allocTable[k]);
            allocTable[k] = NULL;
        }
    }
}


static int stressing_shmalloc_test (void)
{
    int i, j, victim;
    int err = 0;
    long int size, max_possible_alloc, heap_size, total_alloc_size = 0;
    void *allocTable[TABLE_LENGTH];
    void *ptr = NULL;
    void *newp = NULL;
    enum allocType { ALLOC_SHMALLOC, ALLOC_SHREALLOC, ALLOC_SHMEMALIGN,
        ALLOC_LAST
    };

    for (i = 0; i < TABLE_LENGTH; i++) {
        allocTable[i] = NULL;
    }
    max_possible_alloc = 0;
    heap_size = memheap_size ();

    max_possible_alloc = binary_search_size (POTENTIAL_MAX_SIZE (heap_size));
#if 0
    if (max_possible_alloc != heap_size) {
#else
    /*
     * In the assumption, that smalloc can allocate memory that
     * is greater than set in environment variable by an user.
     */
    printf("max_possible_alloc = %ld\n", max_possible_alloc);
    printf("heap_size = %ld\n", heap_size);
    if (max_possible_alloc < heap_size) {
#endif
        log_error(OSH_TC, "Maximum allocation size is %ld . exiting...\n",
                max_possible_alloc);
        err |= ERROR_MAXALLOC;
    } else {
        for (i = 0; i < ALLOC_LAST; i++) {
            j = PRETEST_CONST;
            switch (i) {
            case ALLOC_SHMALLOC:
                while (j >= 1 && !(err & ERROR_SHMALLOC)) {
                    ptr = shmalloc (heap_size);
                    if (NULL == ptr) {
                        err |= ERROR_SHMALLOC;
                    } else {
                        shfree (ptr);
                    }
                    j--;
                }
                break;
            case ALLOC_SHREALLOC:
                while (j >= 1 && !(err & ERROR_SHREALLOC)) {
                    ptr = shmalloc (INITIAL_SIZE_FOR_SHREALLOC);
                    if (NULL == ptr) {
                        log_error(OSH_TC, "Failed in shmalloc in testing_shrealloc\n");
                        err |= ERROR_SHREALLOC;
                    } else {
                        newp = shrealloc (ptr, heap_size);
                        if (NULL == newp) {
                            log_error(OSH_TC, "Failed in shrealloc in testing_shrealloc\n");
                            err |= ERROR_SHREALLOC;
                        } else {
                            ptr = newp;
                        }
                        shfree (ptr);
                    }
                    j--;
                }
                break;
            case ALLOC_SHMEMALIGN:
                while (j >= 1 && !(err & ERROR_SHMEMALIGN)) {
                    ptr = shmemalign (SHMEMALIGN_BOUNDRY, heap_size);
                    if (NULL == ptr) {
                        err |= ERROR_SHMEMALIGN;
                    } else {
                        shfree (ptr);
                    }
                    j--;
                }
                break;
            }
        }
        for (i = 0; i < ALLOC_LAST; i++) {
            switch (i) {
            case ALLOC_SHMALLOC:
                for (j = 0; (!(err & ERROR_TABLESHMALLOC))
                     && j < (TABLE_LENGTH * ITERATIONS_CONST); j++) {
                    size = MIN_ALLOC_SIZE_IN_BYTES + rnd_mt_next(&rnd) % (heap_size / MAX_ALLOC_CONST);
                    size = align_size (size);
                    total_alloc_size += size;
                    do {
                        if (total_alloc_size > (heap_size * HEAP_PERCENT)) {
                            free_alloc_table (allocTable);
                            total_alloc_size = 0;
                            size =
                                MIN_ALLOC_SIZE_IN_BYTES + rnd_mt_next(&rnd) % (heap_size / MAX_ALLOC_CONST);
                            size = align_size (size);
                            total_alloc_size += size;
                        }
                    } while (total_alloc_size > (heap_size * HEAP_PERCENT));
                    victim = rnd_mt_next(&rnd) % TABLE_LENGTH;
                    if (NULL != allocTable[victim]) {
                        shfree (allocTable[victim]);
                        allocTable[victim] = NULL;
                    }
                    allocTable[victim] = shmalloc (size);
                    if (NULL == allocTable[victim]) {
                        err |= ERROR_TABLESHMALLOC;
                    }
                }
                break;
            case ALLOC_SHREALLOC:
                for (j = 0; (!(err & ERROR_TABLESHREALLOC))
                     && j < (TABLE_LENGTH * ITERATIONS_CONST); j++) {
                    victim = rnd_mt_next(&rnd) % TABLE_LENGTH;
                    if (NULL != allocTable[victim]) {
                        shfree (allocTable[victim]);
                        allocTable[victim] = NULL;
                    }
                    allocTable[victim] =
                        shmalloc (INITIAL_SIZE_FOR_SHREALLOC);
                    if (NULL == allocTable[victim]) {
                        err |= ERROR_TABLESHREALLOC;
                    } else {
                        size = MIN_ALLOC_SIZE_IN_BYTES + rnd_mt_next(&rnd) % (heap_size / MAX_ALLOC_CONST);
                        size = align_size (size);
                        total_alloc_size = total_alloc_size + size;
                        do {
                            if (total_alloc_size >=
                                (heap_size * HEAP_PERCENT)) {
                                free_alloc_table (allocTable);
                                total_alloc_size = 0;
                                size =
                                    MIN_ALLOC_SIZE_IN_BYTES +
                                    rnd_mt_next(&rnd) % (heap_size / MAX_ALLOC_CONST);
                                size = align_size (size);
                                total_alloc_size += size;
                            }
                        } while (total_alloc_size >=
                                 (heap_size * HEAP_PERCENT));

                        if (NULL == allocTable[victim]) {
                            allocTable[victim] =
                                shmalloc (INITIAL_SIZE_FOR_SHREALLOC);
                        }


                        if (NULL != allocTable[victim]) {
                            newp = shrealloc (allocTable[victim], size);
                            if (NULL == newp) {
                                err |= ERROR_TABLESHREALLOC;
                            } else {
                                allocTable[victim] = newp;
                            }
                        } else {
                            err |= ERROR_TABLESHREALLOC;
                        }


                    }
                }
                break;
            case ALLOC_SHMEMALIGN:
                for (j = 0; (!(err & ERROR_TABLESHMEMALIGN))
                     && j < (TABLE_LENGTH * ITERATIONS_CONST); j++) {

                    size = MIN_ALLOC_SIZE_IN_BYTES + rnd_mt_next(&rnd) % (heap_size / MAX_ALLOC_CONST);
                    size = align_size (size);
                    total_alloc_size += size;
                    do {
                        if (total_alloc_size >= (heap_size * HEAP_PERCENT)) {
                            free_alloc_table (allocTable);
                            total_alloc_size = 0;

                            size =
                                MIN_ALLOC_SIZE_IN_BYTES + rnd_mt_next(&rnd) % (heap_size / MAX_ALLOC_CONST);
                            size = align_size (size);
                            total_alloc_size += size;
                        }
                    } while (total_alloc_size >= (heap_size * HEAP_PERCENT));

                    victim = rnd_mt_next(&rnd) % TABLE_LENGTH;
                    if (NULL != allocTable[victim]) {
                        shfree (allocTable[victim]);
                        allocTable[victim] = NULL;
                    }

                    allocTable[victim] =
                        shmemalign (SHMEMALIGN_BOUNDRY, size);
                    if (NULL == allocTable[victim]) {
                        err |= ERROR_TABLESHMEMALIGN;
                    }

                }
                break;

            }

        }

        free_alloc_table (allocTable);
    }
    return err;
}

int osh_basic_tc8 (const TE_NODE * node, int argc, const char *argv[])
{
    int rc = TC_PASS;
    UNREFERENCED_PARAMETER (argc);
    UNREFERENCED_PARAMETER (argv);

    //shmem_init ();
    initialize_mt_generator(111,&rnd);

    rc = stressing_shmalloc_test ();
    log_item (node, 1, rc);

    if (rc != TC_PASS) {
        error_handler (rc);
        return TC_FAIL;
    }

    return rc;
}
