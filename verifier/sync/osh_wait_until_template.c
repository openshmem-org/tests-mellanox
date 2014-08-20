// Copyright (c) 2014      Mellanox Technologies, Inc.
//                         All rights reserved
// $COPYRIGHT$

#define DO_TEST(buf, cond, val, putval) \
    do { \
        TYPE old_val = buf[0]; \
        shmem_barrier_all(); \
        /* update neighbour */ \
        log_debug(OSH_TC, "write %d to peer %d\n", putval, peer); \
        FUNC_PUT(buf, putval, peer); \
        /* now wait 4 our update */ \
        alarm(5); \
        FUNC_WAIT(buf, cond, val); \
        alarm(0); \
        log_debug(OSH_TC, "val=%d buf=%d\n", val, buf[0]); \
        if (buf[0] != putval || buf[0] == old_val) \
        { \
            log_fatal(OSH_TC, "Cond %d failed on val=%d buf[0]=%d\n", cond, val, buf[0]); \
            shfree(buf); \
            return TC_FAIL; \
        } \
    }  while (0)

static int TEST_NAME(void)
{
    int rc = TC_PASS;
    int num_proc = 0;
    int my_proc = 0;
    int peer;
    TYPE val, putval;
    TYPE *buf;
    
    num_proc = _num_pes();
    my_proc = _my_pe();
    peer = (my_proc + 1) % num_proc;
    buf = (TYPE *)shmalloc(sizeof(TYPE));
    if (!buf)
    {
        log_error(OSH_TC, "shmalloc\n");
        return TC_FAIL;
    }
    log_debug(OSH_TC, "%d: buf = %p\n", my_proc, buf);

    val = 0;
    buf[0] = val-1;
    if (setjmp(error_env))
    {
        log_fatal(OSH_TC, "Test failed on val=%d buf[0]=%d\n", val, buf[0]);
        shfree(buf);
        return TC_FAIL;
    }

    /* test conditions */

    /* wait till equal */
    putval = val = 0xABCD; buf[0] = 0;
    DO_TEST(buf, SHMEM_CMP_EQ, val, putval);

    /* wait not equal */
    putval = 0xB; val = buf[0] = 0xA;
    DO_TEST(buf, SHMEM_CMP_NE, val, putval);

    /* wait gt */
    putval = 20000; val = 10000 ; buf[0] = 1000; 
    DO_TEST(buf, SHMEM_CMP_GT, val, putval);

    /* wait le */
    putval = 10; val = 100 ; buf[0] = 1000; 
    DO_TEST(buf, SHMEM_CMP_LE, val, putval);

    /* wait ge */
    putval = 20000; val = 10000 ; buf[0] = 1000; 
    DO_TEST(buf, SHMEM_CMP_GE, val, putval);

    /* wait lt */
    putval = 10; val = 100 ; buf[0] = 1000; 
    DO_TEST(buf, SHMEM_CMP_LT, val, putval);

    shfree(buf);
    log_debug(OSH_TC, "passed");
    return rc;
}

#undef TYPE
#undef FUNC_PUT
#undef FUNC_WAIT
#undef TEST_NAME
#undef DO_TEST
