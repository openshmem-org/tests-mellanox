/*
 * Copyright (c) 2014      Mellanox Technologies, Inc.
 *                         All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
/**
 * @file osh_def.h
 *
 * @brief Place to define common types.
 *
 **/
#ifndef _OSH_DEF_H_
#define _OSH_DEF_H_

#include "config.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>   /* printf PRItn */
#include <ctype.h>
#include <float.h>
#include <time.h>
#include <math.h>
#include <complex.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef HAVE_UNISTD_H
    #include <unistd.h>
    #include <errno.h>
    #include <stdint.h>
    #include <sys/time.h>
    #include <pthread.h>
#else
    #include <windows.h>
    #include <winsock.h>
    #include <process.h>
    #define unlink  _unlink     /* to press warning C4996: 'unlink': The POSIX name for this item is deprecated*/
    #define fileno  _fileno     /* to press warning C4996: 'fileno': The POSIX name for this item is deprecated*/
    #define sleep(x)   Sleep(x*1000)
#endif


#ifdef __cplusplus
extern "C" {
#endif


#ifndef TRUE
#define TRUE (1 == 1)
#endif /* TRUE */

#ifndef FALSE
#define FALSE (1 == 0)
#endif /* FALSE */

#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#ifndef BOOL
#define BOOL    int
#endif


/**
 * @name Global types
 * @details
 *      If any of the integral types below : UINT8, UINT16, UINT32
 *      have already been defined elsewhere, the conflicts must be manually
 *      resolved to ensure the existing definitions produce
 *      data types of the proper size and sign.  Note that
 *      this check may not work under all compilers.
 */
/** @{ */
typedef uint8_t   UINT8_TYPE;                   /**< 8-bit  unsigned integer */
typedef int8_t    INT8_TYPE;                    /**< 8-bit  signed integer */
typedef uint16_t  UINT16_TYPE;                  /**< 16-bit unsigned integer */
typedef int16_t   INT16_TYPE;                   /**< 16-bit signed integer */
typedef uint32_t  UINT32_TYPE;                  /**< 32-bit unsigned integer */
typedef int32_t   INT32_TYPE;                   /**< 32-bit signed integer */
typedef uint64_t  UINT64_TYPE;                  /**< 64-bit unsigned types */
typedef int64_t   INT64_TYPE;                   /**< 64-bit signed types */

#pragma pack( push, 1 )
typedef struct _DATA128_TYPE
{
   uint64_t    field1;
   uint64_t    field2;
}  DATA128_TYPE;
#pragma pack( pop )
/** @} */

#define INLINE  __inline

/* This macros should be used in printf to display uintptr_t */
#ifndef PRIXPTR
#define PRIXPTR    "lX"
#endif

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P) ((void)P)
#endif


#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)

#ifndef VERSION
#define VERSION unknown
#endif

#define MODULE_NAME "oshmem_test"
#define MODULE_COPYRIGHT	"Copyright (C) 2014 Mellanox Technologies Ltd." \
	"\nThis tool is open source software," \
    "\nsee http://www.mellanox.com/ and http://www.openshmem.org"


/**
 * @enum OSH_ERROR
 * @brief List of supported error codes.
 */
typedef enum
{
    OSH_ERR_NONE  =  0x0,  /**< the function completed */
    OSH_ERR_BAD_ARGUMENT,  /**< incorrect parameter */
    OSH_ERR_INCORRECT,     /**< incorrect internal object */
    OSH_ERR_UNSUPPORTED,   /**< this function is not supported */
    OSH_ERR_NOT_EXIST,     /**< requested object does not exist */
    OSH_ERR_NO_MEMORY,     /**< dynamic memory error */
    OSH_ERR_FATAL,         /**< system fatal error */
    OSH_ERR_TIMEOUT,       /**< the time limit expires */
    OSH_ERR_UNKNOWN,       /**< general error */
    OSH_ERR_TEST_FAILED    /**< test failed */
} OSH_ERROR;


/**
 * @struct _OSH_CODE
 * @brief This type defines code values based ID and name.
 */
typedef struct _OSH_CODE
{
    int       id;     /**< significant value */
    char*     name;   /**< description */
} OSH_CODE;


/* sleep() that is uninterruptible by signal */
static inline void osh_sleep(unsigned secs)
{
    struct timespec req, rem;
    int err;

    req.tv_sec  = secs;
    req.tv_nsec = 0;

    do {
        err = nanosleep(&req, &rem);
        if (err == 0 || errno != EINTR)
            break;
        memcpy(&req, &rem, sizeof(req));
    } while(1);
} 

#ifdef __cplusplus
}
#endif


#endif /* _OSH_DEF_H_ */

