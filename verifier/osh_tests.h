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

#endif
