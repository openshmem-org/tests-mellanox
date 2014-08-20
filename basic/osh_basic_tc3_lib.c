/*
 * Copyright (c) 2014      Mellanox Technologies, Inc.
 *                         All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

//#include "oshmem_config.h"

static int foo;

#define OSHMEM_DECLSPEC
OSHMEM_DECLSPEC int _oshmem_lib_global = 0xCAFEBABE;

OSHMEM_DECLSPEC int *oshmem_get_int_buf(void);
OSHMEM_DECLSPEC int *oshmem_get_int_buf(void)
{
    return &foo;
}
