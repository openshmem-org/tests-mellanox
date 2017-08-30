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

#include "osh_basic_tests.h"

/****************************************************************************
 * Test Case can consitis of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
static int test_item1(void);
static int test_item2(void);
static int test_item3(void);
static int test_item4(void);

static int test_64bit_align(void);
static int test_memalign(void);
static int test_realloc(void);
static int test_addr_accessible(void);
static int test_shmem_ptr();
static int test_allocation_size(void);
static int test_global_vars(void);
static int test_max_size(void);

#define LOOP_COUNT  1000

enum {
    MEMHEAP_ALLOC_UNKNOWN,
    MEMHEAP_ALLOC_BUDDY,
    MEMHEAP_ALLOC_PTMALLOC,
    MEMHEAP_ALLOC_UNDEFINED
};

static int memheap_type(void)
{
    static int memheap_type = MEMHEAP_ALLOC_UNKNOWN;
    char *p;

    if (memheap_type != MEMHEAP_ALLOC_UNKNOWN)
        return memheap_type;

    p = getenv("SHMEM_SYMMETRIC_HEAP_ALLOCATOR");
    if (p)
    {
        log_debug(OSH_TC, "heap allocator is %s\n", p);
        if (strcmp(p, "buddy") == 0)
            memheap_type = MEMHEAP_ALLOC_BUDDY;
        else if (strcmp(p, "ptmalloc") == 0)
            memheap_type = MEMHEAP_ALLOC_PTMALLOC;
        else
            memheap_type = MEMHEAP_ALLOC_UNDEFINED;
    }
    else
    {
        log_debug(OSH_TC, "can not find heap allocator type\n");
        memheap_type = MEMHEAP_ALLOC_UNDEFINED;
    }

    return memheap_type;
}

/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/

int osh_basic_tc3(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);


    rc = test_max_size();
    log_item(node, 1, rc);

    if (rc == TC_PASS) {
        rc = test_allocation_size();
        log_item(node, 2, rc);
    }
    if (rc == TC_PASS)
    {
        rc = test_item1();
        log_item(node, 3, rc);
    }
    if (rc == TC_PASS)
    {
        rc = test_item2();
        log_item(node, 4, rc);
    }

    if (rc == TC_PASS)
    {
        rc = test_item3();
        log_item(node, 5, rc);
    }

    if (rc == TC_PASS)
    {
        rc = test_64bit_align();
        log_item(node, 6, rc);
    }

    if (rc == TC_PASS)
    {
        rc = test_memalign();
        log_item(node, 7, rc);
    }

    if (rc == TC_PASS)
    {
        rc = test_realloc();
        log_debug(OSH_TC, "realloc test exit with %s\n", rc == TC_PASS ? "pass" : "fail");
        log_item(node, 8, rc);
    }

    if (rc == TC_PASS)
    {
        rc = test_addr_accessible();
        log_item(node, 9, rc);
    }

    if (rc == TC_PASS)
    {
        rc = test_global_vars();
        log_item(node, 10, rc);
    }

    if (rc == TC_PASS)
    {
        rc = test_item4();
        log_item(node, 11, rc);
    }

    if (rc == TC_PASS)
    {
        rc = test_shmem_ptr();
        log_item(node, 12, rc);
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

    shmem_addr = shmalloc(sizeof(int));

    rc = (shmem_addr ? TC_PASS : TC_FAIL);

    log_debug(OSH_TC, "addr = %p\n", shmem_addr);

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
    int value = _my_pe();

    shmem_addr = shmalloc(sizeof(int));

    if (shmem_addr)
    {
        *shmem_addr = value;
        rc = (*shmem_addr == value ? TC_PASS : TC_FAIL);
    }
    else
    {
        rc = TC_FAIL;
    }

    log_debug(OSH_TC, "addr = %p value = %d\n", shmem_addr, value);

    if (shmem_addr)
    {
        shfree(shmem_addr);
    }

    return rc;
}


static int test_item3(void)
{
    int rc = TC_PASS;
    int* shmem_addr[LOOP_COUNT];
    int i = 0;

    sys_memset(shmem_addr, 0 , sizeof(shmem_addr));

    for (i = 0; i < LOOP_COUNT; i++)
    {
        shmem_addr[i] = shmalloc(sizeof(int));
        if (!shmem_addr[i])
        {
            rc = TC_FAIL;
            log_debug(OSH_TC, "Failure on step %d\n", i);
            break;
        }
    }

    for (i = 0; i < LOOP_COUNT; i++)
    {
        if (shmem_addr[i])
        {
            shfree(shmem_addr[i]);
        }
    }

    return rc;
}



static int test_item4(void)
{
    int rc = TC_PASS;
    int* shmem_addr = NULL;
    int value = _my_pe();
    int i = 0;

    shmem_addr = shmalloc(sizeof(int));

    if (shmem_addr)
    {
        *shmem_addr = value;
        rc = (*shmem_addr == value ? TC_PASS : TC_FAIL);
    }
    else
    {
        rc = TC_FAIL;
    }


    for (i = 0; (!rc) && (i < LOOP_COUNT); i++)
    {
        value = _my_pe() * 100000 + random_value(1, 100000);
        *shmem_addr = value;
        if (*shmem_addr != value)
        {
            rc = TC_FAIL;
            log_debug(OSH_TC, "Failure case: i = %d addr = %p value = %d\n", i, shmem_addr, value);
            break;
        }
    }

    if (shmem_addr)
    {
        shfree(shmem_addr);
    }

    return rc;
}

static int test_64bit_align(void)
{
    int rc = TC_PASS;
    char *p[LOOP_COUNT];
    int i;

    for (i = 0; i < LOOP_COUNT; i++)
    {
        p[i] = shmalloc(2*i+3);
        if (!p[i])
        {
            rc = TC_FAIL;
            log_debug(OSH_TC, "alloc fail: got %p size %d\n", p[i], 2*i+3);
        }
        if ((unsigned long)p[i] & 0x7)
        {
            rc = TC_FAIL;
            log_debug(OSH_TC, "Align failure: got %p size %d - not aligned to 64 bit\n", p[i], 2*i+3);
        }
    }
    for (i = 0; i < LOOP_COUNT; i++)
    {
        shfree(p[i]);
    }
    return rc;
}

static int test_memalign(void)
{
    int align;
    char *p;

    /* test that we get desired alignments */
    for (align = 1; align <= 4096; align *= 2)
    {
       p = shmemalign(align, 37);
       if (!p)
       {
           log_debug(OSH_TC, "failed to alloc with align = %d\n", align);
           return TC_FAIL;
       }
       if ((unsigned long)p & (align-1))
       {
           log_debug(OSH_TC, "failed to alloc with align = %d\n", align);
           return TC_FAIL;
       }
       shfree(p);
    }

    /* test some corner cases */
    p = shmemalign(0, 100);
    if (p) {
        log_debug(OSH_TC, "align zero returnded valid ptr!!!\n");
        return TC_FAIL;
    }

    p = shmemalign(3, 100);
    if (p) {
        log_debug(OSH_TC, "align non power of two returnded valid ptr!!!\n");
        return TC_FAIL;
    }

    return TC_PASS;
}

static int __verify(char *p, int size, int pattern)
{
    for (--size;size >= 0; size--)
    {
        if ((uint8_t)p[size] != (uint8_t)pattern)
        {
            log_debug(OSH_TC, "verifey idx=%d got %x != %x\n", size, (uint8_t)p[size], pattern);
            return TC_FAIL;
        }
    }
    return TC_PASS;
}

static int test_realloc()
{
    char *p1, *p2;

    p1 = shmalloc(999);
    if (!p1)
        return TC_FAIL;

    memset(p1, 0xEF, 999);

    p2 = shrealloc(p1, 42);
    if (!p2)
    {
        log_debug(OSH_TC, "Failed to realloc\n");
        return TC_FAIL;
    }

    if (__verify(p2, 42, 0xEF) == TC_FAIL)
    {
        log_debug(OSH_TC, "Failed to verify from 999 to 42\n");
        return TC_FAIL;
    }

    p1 = shrealloc(p2, 1717);
    if (!p1)
    {
        log_debug(OSH_TC, "Failed to realloc from 42 to 1717\n");
        return TC_FAIL;
    }
    if (__verify(p1, 42, 0xEF) == TC_FAIL)
    {
        log_debug(OSH_TC, "Failed to verify from 42 to 1717\n");
        return TC_FAIL;
    }

    /* corner cases */
    p2 = shrealloc(p1, 0); /* works as shfree() */
    if (p2)
    {
        log_debug(OSH_TC, "failed shrealloc as shfree()\n");
        return TC_FAIL;
    }

    p1 = shrealloc(0, 333); /* works as malloc() */
    if (!p1)
    {
        log_debug(OSH_TC, "failed shrealloc as shmalloc()\n");
        return TC_FAIL;
    }
    shfree(p1);

    /* should not be able to realloc non valid ptr */
    p2 = shrealloc(p1, 666);
    if (p2) {
        log_debug(OSH_TC, "failed shrealloc with non valid ptr\n");
        return TC_FAIL;
    }

    /* should be able to realloc if old_size + new_size > heap_size && new_size <= heap_size */
    p1 = shmalloc(16);
    if (!p1) {
        log_debug(OSH_TC, "shmalloc(16) failed");
        return TC_FAIL;
    }

    p2 = shrealloc(p1, memheap_size());
    if (!p2) {
        log_debug(OSH_TC, "failed shrealloc to %d bytes\n", memheap_size());
        return TC_FAIL;
    }
    shfree(p2);

    return TC_PASS;
}

int __global_foo = 4242;

static int test_addr_accessible()
{
    int my_pe = _my_pe();
    int n_pes = _num_pes();
    int pe = (my_pe + 1) % n_pes;
    static int foo;
    char *p1;

    if (shmem_addr_accessible(0, pe))
       return TC_FAIL;

   /* next two are not going to work for now */
   if  (shmem_addr_accessible(&foo, pe))
       return TC_PASS;

   if (shmem_addr_accessible(&__global_foo, pe))
       return TC_PASS;

   p1 = (char *)shmalloc(1234);
   if (!shmem_addr_accessible(p1, pe))
   {
       shfree(p1);
       return TC_FAIL;
   }
   shfree(p1);
   return TC_PASS;

}


static int test_shmem_ptr()
{
    int my_pe = _my_pe();
    int n_pes = _num_pes();
    static int foo;
    int *ptr;
    int *pdata;
    int i;

    pdata = (int *)shmalloc(5678);
    foo = my_pe;
    *pdata = my_pe;
    shmem_barrier_all();

    /* if shmem_ptr is implemented it must return a pointer to
     * the remote variable which holds a remote pe number
     */
    for (i = 0; i < n_pes; i++) {
        ptr = shmem_ptr(&foo, i);
        if (ptr) {
            log_debug(OSH_TC, "%d: shmem_ptr(static=%p, dst=%d) = %p, val=%d\n", my_pe, &foo, i, ptr, *ptr);
            if (*ptr != i)
                goto fail;
        }
        ptr = shmem_ptr(pdata, i);
        if (ptr) {
            log_debug(OSH_TC, "%d: shmem_ptr(heap=%p, dst=%d) = %p, val=%d\n", my_pe, pdata, i, ptr, *ptr);
            if (*ptr != i)
                goto fail;
        }
    }

    shfree(pdata);
    return TC_PASS;
fail:
    shfree(pdata);
    return TC_FAIL;
}

static int test_max_size(void)
{
#if 0
    char *p;
    int size = 256 * 1024 * 1024; // this is default symmetric heap size

    p = shmalloc(size);
    if (!p) {
        log_debug(OSH_TC, "Failed to use all(%d) symmetric heap memory", size);
        return TC_FAIL;
    }
    shfree(p);
#endif
    return TC_PASS;
}


static int test_allocation_size(void)
{
    int rc = TC_PASS;
    char *p1, *p2;
    int size = 0;
    int cur_size = 0;

    for (size = 4 * 1024; (size <= 1024 * 1024) && (rc == TC_PASS); size *= 2)
    {
        p1 = (char *)shmalloc(size);
        p2 = (char *)shmalloc(size);

        if (p1 && p2)
        {
            log_debug(OSH_TC, "p1=%p p2=%p size = %d delta=%ld\n", p1, p2, size, p2-p1);
            if (memheap_type() == MEMHEAP_ALLOC_BUDDY && (((unsigned long)p1 & (size-1)) || ((unsigned long)p2 & (size-1))))
            {
                log_debug(OSH_TC, "shmalloc returned not aligned buffer!!!\n");
                shfree(p1);
                shfree(p2);

                return TC_FAIL;
            }

            if (p2 > p1 && p2 - p1 < size)
            {
                log_debug(OSH_TC, "shmalloc logic fail\n");
                shfree(p1);
                shfree(p2);

                return TC_FAIL;
            }
            if (p1 > p2 && p1 - p2 < size)
            {
                log_debug(OSH_TC, "shmalloc logic fail\n");
                shfree(p1);
                shfree(p2);

                return TC_FAIL;
            }

            shfree(p1);
            shfree(p2);
        }
        else
        {
            rc = TC_SETUP_FAIL;
        }
    }

    for (cur_size = 1024; (cur_size <= 1024 * 1024) && (rc == TC_PASS); cur_size *= 2)
    {
        size = cur_size - 1;
        p1 = (char *)shmalloc(size);
        p2 = (char *)shmalloc(size);

        if (p1 && p2)
        {
            log_debug(OSH_TC, "p1=%p p2=%p size = %d delta=%ld\n", p1, p2, size, p2-p1);

            if (memheap_type() == MEMHEAP_ALLOC_BUDDY && (((unsigned long)p1 & (size-1)) || ((unsigned long)p2 & (size-1))))
            {
                log_debug(OSH_TC, "shmalloc returned not aligned buffer!!!\n");
                shfree(p1);
                shfree(p2);

                return TC_FAIL;
            }

            if (p2 > p1 && p2 - p1 < size)
            {
                log_debug(OSH_TC, "shmalloc logic fail\n");
                shfree(p1);
                shfree(p2);

                return TC_FAIL;
            }
            if (p1 > p2 && p1 - p2 < size)
            {
                log_debug(OSH_TC, "shmalloc logic fail\n");
                shfree(p1);
                shfree(p2);

                return TC_FAIL;
            }

            shfree(p1);
            shfree(p2);
        }
        else
        {
            rc = TC_SETUP_FAIL;
        }
    }

    for (cur_size = 1024; (cur_size <= 1024 * 1024) && (rc == TC_PASS); cur_size *= 2)
    {
        size = cur_size + 1;
        p1 = (char *)shmalloc(size);
        p2 = (char *)shmalloc(size);

        if (p1 && p2)
        {
            log_debug(OSH_TC, "p1=%p p2=%p size = %d delta=%ld\n", p1, p2, size, p2-p1);
            if (memheap_type() == MEMHEAP_ALLOC_BUDDY && (((unsigned long)p1 & (size-1)) || ((unsigned long)p2 & (size-1))))
            {
                log_debug(OSH_TC, "shmalloc returned not aligned buffer!!!\n");
                shfree(p1);
                shfree(p2);

                return TC_FAIL;
            }

            if (p2 > p1 && p2 - p1 < size)
            {
                log_debug(OSH_TC, "shmalloc logic fail\n");
                shfree(p1);
                shfree(p2);

                return TC_FAIL;
            }
            if (p1 > p2 && p1 - p2 < size)
            {
                log_debug(OSH_TC, "shmalloc logic fail\n");
                shfree(p1);
                shfree(p2);

                return TC_FAIL;
            }

            shfree(p1);
            shfree(p2);
        }
        else
        {
            rc = TC_SETUP_FAIL;
        }
    }

    return rc;
}

int _global_one = 0xDEADBEEF;

static int test_global_vars()
{
    static int foo;
    int num_proc;
    int my_pe;
    int val;
    int peer;
    extern int _oshmem_lib_global;
    extern int *oshmem_get_int_buf(void);
    int *p;

    num_proc = _num_pes();
    my_pe = _my_pe();
    peer = (my_pe + 1) % num_proc;
    p = oshmem_get_int_buf();

    /* test get */
    val = shmem_int_g(&_global_one, peer);
    if ((unsigned int)val != 0xDEADBEEF)
    {
        log_debug(OSH_TC, "expected 0xDEADBEEF got %d\n", val);
        return TC_FAIL;
    }

    /* static vars are set to 0 !*/
    val = shmem_int_g(&foo, peer);
    if (val != 0)
    {
        log_debug(OSH_TC, "expected 0 got %d\n", val);
        return TC_FAIL;
    }

    /* next two are globals in lib */
    /* test get */
    log_debug(OSH_TC, "**** testing get to lib globals **** %p\n", &_oshmem_lib_global);
    val = shmem_int_g(&_oshmem_lib_global, peer);
    if ((unsigned int)val != 0xCAFEBABE) /* this is global default value */
    {
        log_debug(OSH_TC, "expected 0xCAFEBABE got %d\n", val);
        return TC_FAIL;
    }

    /* static vars are set to 0 !*/
    log_debug(OSH_TC, "**** testing get to lib static **** %p\n", p);
    val = shmem_int_g(p, peer);
    if (val != 0)
    {
        log_debug(OSH_TC, "expected 0 got %d\n", val);
        return TC_FAIL;
    }


    /* test put */
    shmem_int_p(&_global_one, 0xabcd, peer);
    shmem_int_wait(&_global_one, 0xDEADBEEF);
    if (_global_one != 0xabcd)
    {
        log_debug(OSH_TC, "expected 0xABCD got %d\n", _global_one);
        return TC_FAIL;
    }

    shmem_int_p(&foo, 0xFEEDBEEF, peer);
    shmem_int_wait(&foo, 0);
    if ((unsigned int)foo != 0xFEEDBEEF)
    {
        log_debug(OSH_TC, "expected 0xABCD got %d\n", foo);
        return TC_FAIL;
    }

    /* lib vars */
    log_debug(OSH_TC, "**** testing put to lib globals **** %p\n", &_oshmem_lib_global);
    shmem_int_p(&_oshmem_lib_global, 0xabcd, peer);
    shmem_int_wait(&_oshmem_lib_global, 0xCAFEBABE);
    if (_oshmem_lib_global != 0xabcd)
    {
        log_debug(OSH_TC, "expected 0xABCD got %d\n", _oshmem_lib_global);
        return TC_FAIL;
    }

    log_debug(OSH_TC, "**** testing put to lib static **** %p\n", p);
    shmem_int_p(p, 0xFEEDBEEF, peer);
    shmem_int_wait(p, 0);
    if ((unsigned int)*p != 0xFEEDBEEF)
    {
        log_debug(OSH_TC, "expected 0xABCD got %d\n", *p);
        return TC_FAIL;
    }

    return TC_PASS;
}

