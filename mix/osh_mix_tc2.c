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
#include "mpi.h"

#include "osh_mix.h"

/****************************************************************************
 * Test Case can consitis of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
static int test_item1(void);


/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_mix_tc2(const TE_NODE *node, int argc, const char *argv[])
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
    #define MESSAGE_ALIGNMENT 64
    #define MAX_MSG_SIZE (1<<22)
    #define MYBUFSIZE (MAX_MSG_SIZE + MESSAGE_ALIGNMENT)

    static char s_buf_original[MYBUFSIZE];
    static char r_buf_original[MYBUFSIZE];

    int skip = 1000;
    int loop = 10000;
    int skip_large = 10;
    int loop_large = 100;
    int large_message_size = 8192;

    int myid, i;
    int size;
    char *sh_buf;
    MPI_Status reqstat;
    char *s_buf, *r_buf;
    int align_size;
    int type = 2;
    double t_start = 0.0, t_end = 0.0;

    myid = _my_pe();

    align_size = MESSAGE_ALIGNMENT;

    /* Allocate a variable from the symmetric heap */
    sh_buf = (char *)shmalloc(MYBUFSIZE);
    if (sh_buf)
    {
        s_buf =
            (char *) (((unsigned long) s_buf_original + (align_size - 1)) /
                      align_size * align_size);
        r_buf =
            (char *) (((unsigned long) r_buf_original + (align_size - 1)) /
                      align_size * align_size);

        while (type--)
        {
            if (myid == 0)
            {
                log_debug(OSH_TC, "\n");
                log_debug(OSH_TC, "# OSU Latency Test (Version 2.2)\n");
                log_debug(OSH_TC, "Used Method: %s\n", (type == 1 ? "OSHMEM" : "OMPI"));
                log_debug(OSH_TC, "# Size\t\tLatency (us) \n");
                log_debug(OSH_TC, "-------------------------------\n");
            }

            for (size = 2; size < MAX_MSG_SIZE;  size = (size ? size * 2 : size + 1))
            {
                for (i = 0; i < size; i++) {
                    s_buf[i] = 'a';
                    r_buf[i] = 'b';
                }

                if (size > large_message_size) {
                    loop = loop_large;
                    skip = skip_large;
                }

                if (type == 1)  /* OSHMEM part */
                {
                    shmem_barrier_all();

                    if (myid == 0) {
                        for (i = 0; i < loop + skip; i++) {
                            if (i == skip)
                                t_start = MPI_Wtime();
                            shmem_putmem(sh_buf, s_buf, size, 1);
                            shmem_short_wait((short*)(sh_buf + size - 1), 0);
                            shmem_getmem(r_buf, sh_buf, size, 1);
                        }
                        t_end = MPI_Wtime();


                    } else if (myid == 1) {
                        for (i = 0; i < loop + skip; i++) {
                            shmem_short_wait((short*)(sh_buf + size - 1), 0);
                            shmem_getmem(r_buf, sh_buf, size, 0);
                            shmem_putmem(sh_buf, s_buf, size, 0);
                        }
                    }
                }
                else  /* OMPI part */
                {
                    MPI_Barrier(MPI_COMM_WORLD);

                    if (myid == 0) {
                        for (i = 0; i < loop + skip; i++) {
                            if (i == skip)
                                t_start = MPI_Wtime();
                            MPI_Send(s_buf, size, MPI_SHORT, 1, 1, MPI_COMM_WORLD);
                            MPI_Recv(r_buf, size, MPI_SHORT, 1, 1, MPI_COMM_WORLD, &reqstat);
                        }
                        t_end = MPI_Wtime();

                    } else if (myid == 1) {
                        for (i = 0; i < loop + skip; i++) {
                            MPI_Recv(r_buf, size, MPI_SHORT, 0, 1, MPI_COMM_WORLD, &reqstat);
                            MPI_Send(s_buf, size, MPI_SHORT, 0, 1, MPI_COMM_WORLD);
                        }
                    }
                }

                if (myid == 0)
                {
                    double latency;
                    latency = (t_end - t_start) * 1.0e6 / (2.0 * loop);
                    log_debug(OSH_TC, "%10d\t\t%10.2f\n", size, latency);
                }
            }
        }
    }
    else
    {
        rc = TC_SETUP_FAIL;
    }

    if (sh_buf)
    {
        shfree(sh_buf);
    }

    return rc;
}
