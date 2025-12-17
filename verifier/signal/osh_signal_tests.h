/*
 * Copyright (c) 2024      NVIDIA Corporation.
 *                         All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef __OSH_SIGNAL_TESTS_H__
#define __OSH_SIGNAL_TESTS_H__

#ifdef HAVE_SIGNAL_PUT
int osh_signal_tc1( const TE_NODE *, int, const char ** );
int osh_signal_tc2( const TE_NODE *, int, const char ** );
#endif

#ifdef HAVE_SIGNAL_FETCH
int osh_signal_tc3( const TE_NODE *, int, const char ** );
#endif

#endif

