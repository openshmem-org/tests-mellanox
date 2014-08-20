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

#include <signal.h>
#include <unistd.h>
#include <setjmp.h>

#include "osh_sync_tests.h"

static sigjmp_buf error_env;
/****************************************************************************
 * Test Case can consitis of different number of separate items
 * it is recommended to form every item as function
 ***************************************************************************/
static int test_short_wait_until(void);
static int test_int_wait_until(void);
static int test_long_wait_until(void);
static int test_longlong_wait_until(void);

#define MAX_COUNT 512

#define TYPE short
#define FUNC_WAIT  shmem_short_wait_until
#define FUNC_PUT   shmem_short_p
#define TEST_NAME  test_short_wait_until

#include "osh_wait_until_template.c"

#define TYPE int
#define FUNC_WAIT  shmem_int_wait_until
#define FUNC_PUT   shmem_int_p
#define TEST_NAME  test_int_wait_until

#include "osh_wait_until_template.c"

#define TYPE long
#define FUNC_WAIT  shmem_long_wait_until
#define FUNC_PUT   shmem_long_p
#define TEST_NAME  test_long_wait_until

#include "osh_wait_until_template.c"

#define TYPE long long
#define FUNC_WAIT  shmem_longlong_wait_until
#define FUNC_PUT   shmem_longlong_p
#define TEST_NAME  test_longlong_wait_until

#include "osh_wait_until_template.c"

#define WAIT_SEC  5


static void test_fail(int signum)
{
    UNREFERENCED_PARAMETER(signum);

    printf("Timeout received - test aborted\n");
    longjmp(error_env, 1);
}

/****************************************************************************
 * Test Case processing procedure
 ***************************************************************************/
int osh_sync_tc3(const TE_NODE *node, int argc, const char *argv[])
{
    int rc = TC_PASS;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    signal(SIGALRM, test_fail);
    rc = test_short_wait_until();
    log_item(node, 1, rc);
    if (rc == TC_FAIL)
        return rc;

    rc = test_int_wait_until();
    log_item(node, 2, rc);
    if (rc == TC_FAIL)
        return rc;

    rc = test_long_wait_until();
    log_item(node, 3, rc);
    if (rc == TC_FAIL)
        return rc;

    rc = test_longlong_wait_until();
    log_item(node, 4, rc);
    if (rc == TC_FAIL)
        return rc;

    return rc;
}


/****************************************************************************
 * Place for Test Item functions
 ***************************************************************************/






