// Copyright (c) 2014      Mellanox Technologies, Inc.
//                         All rights reserved
// $COPYRIGHT$
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <shmem.h>
#include <mpi.h>

#include "osh_def.h"
#include "osh_cmn.h"
#include "osh_log.h"

#include "osh_mix.h"

#define ROOT                    (0)
enum errors {
    ERROR_MPI_SEND               = 1,
    ERROR_MPI_RECV               = 1 << 1,
    ERROR_MPI_ISEND              = 1 << 2,
    ERROR_MPI_IRECV              = 1 << 3,
    ERROR_MPI_REDUCE             = 1 << 4,
    ERROR_MPI_BCAST              = 1 << 5,
    ERROR_MPI_ALLGATHER          = 1 << 6,
    ERROR_SHMEM_SHMALLOC         = 1 << 7,
    ERROR_SHMEM_SWAP             = 1 << 8,
    ERROR_SHMEM_START            = 1 << 9,
    ERROR_SHMEM_END              = 1 << 10,
    ERROR_ROOT_VAL               = 1 << 11,
    ERROR_MPI_START              = 1 << 12,
    ERROR_MPI_END                = 1 << 13,
    ERROR_LAST                   = 1 << 14
};

static void error_handler (int err)
{
    if ((err & ERROR_ROOT_VAL) == ERROR_ROOT_VAL) {
        log_error
            (OSH_TC, "The root value must be lower then the total number of PEs. Exiting... \n");
        return;
    }
    log_error (OSH_TC, "There was an error in:\n");
    if ((err & ERROR_MPI_SEND) == ERROR_MPI_SEND) {
        log_error (OSH_TC, "MPI_Send\n");
    }
    if ((err & ERROR_MPI_RECV) == ERROR_MPI_RECV) {
        log_error (OSH_TC, "MPI_Recv\n");
    }
    if ((err & ERROR_MPI_ISEND) == ERROR_MPI_ISEND) {
        log_error (OSH_TC, "MPI_Isend\n");
    }
    if ((err & ERROR_MPI_IRECV) == ERROR_MPI_IRECV) {
        log_error (OSH_TC, "MPI_Irecv\n");
    }
    if ((err & ERROR_MPI_REDUCE) == ERROR_MPI_REDUCE) {
        log_error (OSH_TC, "MPI_Reduce\n");
    }
    if ((err & ERROR_MPI_BCAST) == ERROR_MPI_BCAST) {
        log_error (OSH_TC, "MPI_Bcast\n");
    }
    if ((err & ERROR_MPI_ALLGATHER) == ERROR_MPI_ALLGATHER) {
        log_error (OSH_TC, "MPI_Allgather\n");
    }
    if ((err & ERROR_SHMEM_SHMALLOC) == ERROR_SHMEM_SHMALLOC) {
        log_error (OSH_TC, "shmalloc\n");
    }
    if ((err & ERROR_SHMEM_SWAP) == ERROR_SHMEM_SWAP) {
        log_error (OSH_TC, "shmem_int_swap\n");
    }
    if ((err & ERROR_SHMEM_START) == ERROR_SHMEM_START) {
        log_error (OSH_TC, "shmem test - part 1\n");
    }
    if ((err & ERROR_SHMEM_END) == ERROR_SHMEM_END) {
        log_error (OSH_TC, "shmem test - part 2\n");
    }
    if ((err & ERROR_MPI_START) == ERROR_MPI_START) {
        log_error (OSH_TC, "MPI test - part 1\n");
    }
    if ((err & ERROR_MPI_END) == ERROR_MPI_END) {
        log_error (OSH_TC, "MPI test - part 2\n");
    }
}

/* A mix test for testing MPI and SHMEM routines.
 *
 * MPI Part - The PEs form a ring, each pe sends his id to the
 * next one. Every pe sends what he received to the next pe once again.
 * The root sums the values of all the PEs and sends it to all of them.
 * Every pe sends the sum of his receiving buffer to all other PEs.
 *
 * SHMEM Part - Each pe receives a certain id.
 * The PEs send these id's to each other until every one has its
 * true id in his receiving buffer.
 * All the PEs then send their id to the pe who is: (numprocs - myid - 1)
 * and so together they sum to the total number of PEs minus one.
 * The root sends the total number of processes to all the other PEs
 * using broadcast.
 */
static int mix_mpi_shmem_test (void)
{
    int dest, source, prev, next;
    int tag = 0, tag1 = 0;
    int err = 0, res, count, sum_recv_buf = 0;
    int i, numprocs, myid, total_sum=0;

    MPI_Request reqs[2];
    MPI_Status stats;
    numprocs = _num_pes ();
    if (ROOT >= numprocs) {
        err |= ERROR_ROOT_VAL;
    } else {
        int mpi_sendbuf[2];
        int mpi_recvbuf[numprocs + 3];

        myid = _my_pe ();

        for (i = 0; i < 2; i++) {
            mpi_sendbuf[i] = 0;
        }
        for (i = 0; i < numprocs + 3; i++) {
            mpi_recvbuf[i] = 0;
        }

        mpi_sendbuf[0] = myid;
        prev = myid - 1;
        next = myid + 1;
        if (0 == myid) {
            prev = numprocs - 1;
        }
        if (myid == (numprocs - 1)) {
            next = 0;
        }
        dest = next;
        source = prev;

              /** MPI PART **/

        res =
            MPI_Send (&mpi_sendbuf[0], 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
        if (res != MPI_SUCCESS) {
            err |= ERROR_MPI_SEND;
            return err;
        }
        res =
            MPI_Recv (&mpi_recvbuf[0], 1, MPI_INT, source, tag,
                      MPI_COMM_WORLD, &stats);
        if (res != MPI_SUCCESS) {
            err |= ERROR_MPI_RECV;
            return err;
        }

        res =
            MPI_Isend (&mpi_recvbuf[0], 1, MPI_INT, next, tag1,
                       MPI_COMM_WORLD, &reqs[0]);
        if (res != MPI_SUCCESS) {
            err |= ERROR_MPI_ISEND;
            return err;
        }
        MPI_Wait (&reqs[0], &stats);
        if (reqs[0] != MPI_REQUEST_NULL) {
            err |= ERROR_MPI_ISEND;
            return err;
        }
        res =
            MPI_Irecv (&mpi_recvbuf[1], 1, MPI_INT, prev, tag1,
                       MPI_COMM_WORLD, &reqs[1]);
        if (reqs[0] != MPI_REQUEST_NULL) {
            err |= ERROR_MPI_IRECV;
            return err;
        }
        MPI_Wait (&reqs[1], &stats);
        MPI_Get_count (&stats, MPI_INT, &count);
        if (reqs[1] != MPI_REQUEST_NULL) {
            err |= ERROR_MPI_IRECV;
            return err;
        }

        /* Checking logical correctness */
        if (numprocs > 1) {
            if (myid > 1) {
                if ((mpi_recvbuf[0] != (myid - 1))
                    || (mpi_recvbuf[1] != (myid - 2))) {
                    err |= ERROR_MPI_START;
                    return err;
                }
            } else {
                if ((0 == myid)
                    && (((mpi_recvbuf[0] != (numprocs - 1))
                         || (mpi_recvbuf[1] != (numprocs - 2))))) {
                    err |= ERROR_MPI_START;
                    return err;
                } else {
                    if ((1 == myid)
                        && (((mpi_recvbuf[0] != (myid - 1))
                             || (mpi_recvbuf[1] != (numprocs - 1))))) {
                        err |= ERROR_MPI_START;
                        return err;
                    }
                }
            }
        } else {
            if ((0 != mpi_recvbuf[0]) || (0 != mpi_recvbuf[1])) {
                err |= ERROR_MPI_START;
                return err;
            }
        }

        res =
            MPI_Reduce (&mpi_sendbuf[0], &mpi_recvbuf[2], 1, MPI_INT, MPI_SUM,
                        ROOT, MPI_COMM_WORLD);
        if (res != MPI_SUCCESS) {
            err |= ERROR_MPI_REDUCE;
            return err;
        }

        for (i = 0; i < numprocs; i++) {
            sum_recv_buf += mpi_recvbuf[i];
        }
        mpi_sendbuf[1] = sum_recv_buf;

        res = MPI_Bcast (&mpi_recvbuf[2], 1, MPI_INT, ROOT, MPI_COMM_WORLD);
        if (res != MPI_SUCCESS) {
            err |= ERROR_MPI_BCAST;
            return err;
        }

        /* Checking logical correctness */
        for (i = 0; i < numprocs; i++) {
            total_sum += i;
        }
        if (mpi_recvbuf[2] != total_sum) {
            err |= ERROR_MPI_END;
            return err;
        }

        res = MPI_Allgather (&mpi_sendbuf[1], 1, MPI_INT, &mpi_recvbuf[3], 1,
                             MPI_INT, MPI_COMM_WORLD);
        if (res != MPI_SUCCESS) {
            err |= ERROR_MPI_ALLGATHER;
            return err;
        }

                                                                                                                                                                                                                     /** SHMEM PART **/
        int *shm_sendarr = (int *) shmalloc (2 * sizeof (int));
        if (NULL == shm_sendarr) {
            err |= ERROR_SHMEM_SHMALLOC;
            return err;
        }
        int *shm_recvarr = (int *) shmalloc (2 * sizeof (int));
        if (NULL == shm_recvarr) {
            err |= ERROR_SHMEM_SHMALLOC;
            shfree (shm_sendarr);
            return err;
        }
        if (!(err & ERROR_SHMEM_SHMALLOC)) {
            shm_sendarr[0] = myid + (numprocs / 2);
            /* Placing the proper id to the matching pe */
            if (0 == numprocs % 2) {
                if (myid < (numprocs / 2)) {
                    shmem_int_iput (&shm_recvarr[0], &shm_sendarr[0], 1, 1, 1,
                                    myid + (numprocs / 2));
                } else {
                    shmem_int_iput (&shm_recvarr[0], &shm_sendarr[0], 1, 1, 1,
                                    myid - (numprocs / 2));
                }
                shmem_barrier_all ();
                if (myid >= (numprocs / 2))
                    shmem_int_add (&shm_recvarr[0], -numprocs,
                                   myid - (numprocs / 2));
            } else {
                if (myid <= (numprocs / 2)) {
                    shmem_int_iput (&shm_recvarr[0], &shm_sendarr[0], 1, 1, 1,
                                    myid + (numprocs / 2));
                } else {
                    shmem_int_iput (&shm_recvarr[0], &shm_sendarr[0], 1, 1, 1,
                                    myid - (1 + (numprocs / 2)));
                }
                shmem_barrier_all ();
                if (myid > (numprocs / 2)) {
                    shmem_int_add (&shm_recvarr[0], -numprocs,
                                   myid - (numprocs / 2) - 1);
                }
            }
            shmem_barrier_all ();
            /* Checking logical correctness */
            if (shm_recvarr[0] != myid) {
                err |= ERROR_SHMEM_START;
                shfree (shm_sendarr);
                shfree (shm_recvarr);
                return err;
            }

            shmem_int_swap (&shm_recvarr[1], shm_recvarr[0],
                            numprocs - myid - 1);
            shmem_barrier_all ();
            /* Checking logical correctness */
            if ((shm_recvarr[0] + shm_recvarr[1]) != (numprocs - 1)) {
                err |= ERROR_SHMEM_SWAP;
                shfree (shm_sendarr);
                shfree (shm_recvarr);
                return err;
            }

            long int *pSync =
                (long *) shmalloc (_SHMEM_BCAST_SYNC_SIZE *
                                   sizeof (long int));
            for (i = 0; i < _SHMEM_BCAST_SYNC_SIZE; i++) {
                pSync[i] = _SHMEM_SYNC_VALUE;
            }

            if (myid == ROOT) {
                shm_recvarr[0] = numprocs;
            }
            /* Sending the numprocs to all PEs */
            shmem_broadcast32 (&shm_sendarr[1], &shm_recvarr[0], 1, ROOT, 0,
                               0, numprocs, pSync);
            shmem_barrier_all ();

            if (myid == ROOT) {
                if (0 == ROOT) {
                    shmem_int_get (&shm_sendarr[1], &shm_sendarr[1], 1,
                                   numprocs - 1);
                } else {
                    shmem_int_get (&shm_sendarr[1], &shm_sendarr[1], 1,
                                   ROOT - 1);
                }
            }
            /* Checking logical correctness */
            if ((numprocs != 1)) {
                if (shm_sendarr[1] != numprocs) {
                    err |= ERROR_SHMEM_END;
                }
            }

            shfree (shm_sendarr);
            shfree (shm_recvarr);
            shfree (pSync);
        }
    }
    return err;
}

int osh_mix_tc3 (const TE_NODE * node, int argc, const char *argv[])
{
    int rc = TC_PASS;
    UNREFERENCED_PARAMETER (argc);
    UNREFERENCED_PARAMETER (argv);

   // MPI_Init (&argc, &bb);
   // shmem_init ();

    rc = mix_mpi_shmem_test ();
    log_item (node, 1, rc);

    if (rc != TC_PASS) {
        error_handler (rc);
        return TC_FAIL;
    }

   // MPI_Finalize ();
    return rc;
}
