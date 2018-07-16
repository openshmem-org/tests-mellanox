/*
 * Copyright (c) 2014-2016 Mellanox Technologies, Inc.
 *                         All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef __OSH_TESTS_H__
#define __OSH_TESTS_H__

int proc_tst_atomic( const TE_NODE *node, int argc, const char **argv );
int proc_tst_basic( const TE_NODE *node, int argc, const char **argv );
int proc_tst_coll( const TE_NODE *node, int argc, const char **argv );
int proc_tst_data( const TE_NODE *node, int argc, const char **argv );
int proc_tst_lock( const TE_NODE *node, int argc, const char **argv );
int proc_tst_reduce( const TE_NODE *node, int argc, const char **argv );
int proc_tst_strided( const TE_NODE *node, int argc, const char **argv );
int proc_tst_sync( const TE_NODE *node, int argc, const char **argv );
int proc_tst_mix( const TE_NODE *node, int argc, const char **argv );
int proc_tst_analysis( const TE_NODE *node, int argc, const char **argv );
int proc_tst_nbi( const TE_NODE *node, int argc, const char **argv );
int proc_tst_misc( const TE_NODE *node, int argc, const char **argv );

#if HAVE_DECL_SHMEM_UINT_ATOMIC_FETCH_AND
#  define TEST_INT_FETCH_AND TEST_RUN
#else
#  define TEST_INT_FETCH_AND TEST_SKIP
#endif

#if HAVE_DECL_SHMEM_ULONG_ATOMIC_FETCH_AND
#  define TEST_LONG_FETCH_AND TEST_RUN
#else
#  define TEST_LONG_FETCH_AND TEST_SKIP
#endif

#if HAVE_DECL_SHMEM_ULONGLONG_ATOMIC_FETCH_AND
#  define TEST_LONGLONG_FETCH_AND TEST_RUN
#else
#  define TEST_LONGLONG_FETCH_AND TEST_SKIP
#endif

#if HAVE_DECL_SHMEM_UINT_ATOMIC_FETCH_OR
#  define TEST_INT_FETCH_OR TEST_RUN
#else
#  define TEST_INT_FETCH_OR TEST_SKIP
#endif

#if HAVE_DECL_SHMEM_ULONG_ATOMIC_FETCH_OR
#  define TEST_LONG_FETCH_OR TEST_RUN
#else
#  define TEST_LONG_FETCH_OR TEST_SKIP
#endif

#if HAVE_DECL_SHMEM_ULONGLONG_ATOMIC_FETCH_OR
#  define TEST_LONGLONG_FETCH_OR TEST_RUN
#else
#  define TEST_LONGLONG_FETCH_OR TEST_SKIP
#endif

#if HAVE_DECL_SHMEM_UINT_ATOMIC_FETCH_XOR
#  define TEST_INT_FETCH_XOR TEST_RUN
#else
#  define TEST_INT_FETCH_XOR TEST_SKIP
#endif

#if HAVE_DECL_SHMEM_ULONG_ATOMIC_FETCH_XOR
#  define TEST_LONG_FETCH_XOR TEST_RUN
#else
#  define TEST_LONG_FETCH_XOR TEST_SKIP
#endif

#if HAVE_DECL_SHMEM_ULONGLONG_ATOMIC_FETCH_XOR
#  define TEST_LONGLONG_FETCH_XOR TEST_RUN
#else
#  define TEST_LONGLONG_FETCH_XOR TEST_SKIP
#endif

#if HAVE_DECL_SHMEM_UINT_ATOMIC_AND
#  define TEST_INT_AND TEST_RUN
#else
#  define TEST_INT_AND TEST_SKIP
#endif

#if HAVE_DECL_SHMEM_ULONG_ATOMIC_AND
#  define TEST_LONG_AND TEST_RUN
#else
#  define TEST_LONG_AND TEST_SKIP
#endif

#if HAVE_DECL_SHMEM_ULONGLONG_ATOMIC_AND
#  define TEST_LONGLONG_AND TEST_RUN
#else
#  define TEST_LONGLONG_AND TEST_SKIP
#endif

#if HAVE_DECL_SHMEM_UINT_ATOMIC_OR
#  define TEST_INT_OR TEST_RUN
#else
#  define TEST_INT_OR TEST_SKIP
#endif

#if HAVE_DECL_SHMEM_ULONG_ATOMIC_OR
#  define TEST_LONG_OR TEST_RUN
#else
#  define TEST_LONG_OR TEST_SKIP
#endif

#if HAVE_DECL_SHMEM_ULONGLONG_ATOMIC_OR
#  define TEST_LONGLONG_OR TEST_RUN
#else
#  define TEST_LONGLONG_OR TEST_SKIP
#endif

#if HAVE_DECL_SHMEM_UINT_ATOMIC_XOR
#  define TEST_INT_XOR TEST_RUN
#else
#  define TEST_INT_XOR TEST_SKIP
#endif

#if HAVE_DECL_SHMEM_ULONG_ATOMIC_XOR
#  define TEST_LONG_XOR TEST_RUN
#else
#  define TEST_LONG_XOR TEST_SKIP
#endif

#if HAVE_DECL_SHMEM_ULONGLONG_ATOMIC_XOR
#  define TEST_LONGLONG_XOR TEST_RUN
#else
#  define TEST_LONGLONG_XOR TEST_SKIP
#endif

#endif
