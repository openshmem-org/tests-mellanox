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
 * @file osh_cmn.h
 *
 * @brief Additional utilities.
 *
 **/

#ifndef _OSH_CMN_H_
#define _OSH_CMN_H_


#include "aopt.h"


/**
 * @name ANSI C Library
 * @brief Standart ANSI C library functions
 */
/** @{*/
#define sys_malloc      malloc
#define sys_free        free
#define sys_memcpy      memcpy
#define sys_memset      memset
#define sys_memcmp      memcmp
#define sys_strlen      strlen
#define sys_strcpy      strcpy
#define sys_strncpy     strncpy
#define sys_strcmp      strcmp
#define sys_strchr      strchr
#define sys_sprintf     sprintf
#define sys_strtol      strtol


/* Minimum and maximum macros */
#define sys_max(a,b)  (((a) > (b)) ? (a) : (b))
#define sys_min(a,b)  (((a) < (b)) ? (a) : (b))
#define sys_fcompare(a,b)  (fabs((double)((a) - (b))) <= (0.00001 * fabs((double)(a))))
#define sys_ccompare(a,b)  (cabs((double complex)((a) - (b))) <= (0.00001 * cabs((double complex)(a))))

#if defined(_MSC_VER)
    #ifndef snprintf
        #define snprintf _snprintf
    #endif
#endif
/** @} */

static INLINE double sys_gettime(void)
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (double)(tv.tv_sec * 1000000 + tv.tv_usec);
}


static INLINE int sys_time(struct timeval *tv)
{
    int status = 0;

#if defined(__LINUX__)
    status = gettimeofday(tv, NULL);
#else
    time_t t = time(NULL);
    tv->tv_sec = (long)t;
    tv->tv_usec = 0; /*clock() % CLOCKS_PER_SEC*/
#endif

    return status;
}


static INLINE uint64_t sys_rdtsc(void)
{
    unsigned long long int result=0;

#if defined(__LINUX__)
    #if defined(__i386__)
        __asm volatile(".byte 0x0f, 0x31" : "=A" (result) : );

    #elif defined(__x86_64__)
        unsigned hi, lo;
        __asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
        result = hi;
        result = result<<32;
        result = result|lo;

    #elif defined(__powerpc__)
        unsigned long int hi, lo, tmp;
        __asm volatile(
                    "0:                 \n\t"
                    "mftbu   %0         \n\t"
                    "mftb    %1         \n\t"
                    "mftbu   %2         \n\t"
                    "cmpw    %2,%0      \n\t"
                    "bne     0b         \n"
                    : "=r"(hi),"=r"(lo),"=r"(tmp)
                    );
        result = hi;
        result = result<<32;
        result = result|lo;

    #endif
#endif /* __LINUX__ */

    return (result);
}


#define sys_strdup(str) \
    sys_strcpy((char *)sys_malloc(sys_strlen(str) + 1), str)

#define sys_snprintf_safe(buf, size, fmt, ...) \
	sys_min(snprintf(buf, size, fmt,  ##__VA_ARGS__), (int)size)

#define sys_array_size(a) (sizeof(a)/sizeof(a[0]))


#define PE_LIST_SIZE    ((int)(8 * sizeof(unsigned long long) - 2))    /**< one bit is reserved for all case (INVALID_PE) */
#define PE_LIST_CHECK(pe, pe_list)  (((pe) < PE_LIST_SIZE) && ((1ULL << (pe)) & (pe_list)))
#define PE_LIST_SET(pe, pe_list)  ((pe_list) |= (1ULL << (pe)))

#define INVALID_PE  (-1)
#define ROOT_PE     (0)
#define DEFAULT_PE  ROOT_PE



/**
 * @enum TC_RESULT
 * @brief Test Case result code.
 */
typedef enum
{
    TC_PASS        =  0x0,    /**< Success */
    TC_FAIL        =  0x1,    /**< Failure */
    TC_SETUP_FAIL  =  0x2,    /**< Test Setup Failure */
    TC_NONE        =  0x3,    /**< Not implemented */
} TC_RESULT;

enum {
    TEST_SKIP = 0,
    TEST_IGNORE,
    TEST_RUN
};

/**
 * @struct _TE_MEM
 * @brief
 *      Description of Test Engine Memory status.
 */
typedef struct _TE_MEM
{
    int64_t vm_size;                                          /**< Virtual Memory Size */
    int64_t vm_rss;                                           /**< Virtual Memory Resident */
} TE_MEM;


/**
 * @struct _TE_NODE
 * @brief
 *      Description of Test Engine node (mode, test suite, test case).
 */
typedef struct _TE_NODE
{
    const struct _TE_NODE * child;
    int (*func)(const struct _TE_NODE *, int, const char **); 	    /**< proc function */
    const char* name;   					                        /**< name to use from command line as an argument */
    const char* const shorts[4];   		                            /**< short name */
    const char* note;                                               /**< help description */
    const int   active_test;                                        /**< 1 if test is ready, 0 if it can be skipped in the "all" test run*/
} TE_NODE;


/**
 * @struct _CONFIG
 * @brief
 *      Configuration common structure.
 */
typedef struct _CONFIG
{
    int    active;
    char   hostname[256];
    int    my_pe;
    int    num_pe;
    int*   rc;
    unsigned timer;
    struct _TE_MODE
    {
        unsigned long long     log_pe_list;
        int     out_level;            /**< terminal output level */
        FILE*   out_file;
        int     log_level;            /**< dump output level */
        FILE*   log_file;
        char*   log_file_name;
    } exec_mode;
    TE_MEM  mstat;
} CONFIG;


extern const AOPT_DESC  common_opt_desc[];
extern CONFIG osh_config;


#define _shmem_init(x)   \
    { \
        if (osh_config.active == 0) \
        { \
            get_mstat(&(osh_config.mstat)); \
            start_pes(0); \
            osh_config.my_pe = _my_pe(); \
            osh_config.num_pe = _num_pes(); \
            /*osh_config.rc = (int*)shmalloc(sizeof(int));*/ \
            *osh_config.rc = 0; \
            osh_config.active = 1; \
        } \
        else if (osh_config.active == 2) \
        { \
            log_error(OSH_TC, "OpenSHMEM was already started\n" ); \
        } \
        else \
        { \
            osh_config.active++; \
            shmem_barrier_all(); \
        } \
    }

#define _shmem_finalize(x)   \
    { \
        if (osh_config.active == 1) \
        { \
            /*shfree(osh_config.rc);*/ \
            *osh_config.rc = 0; \
            shmem_finalize(); \
            osh_config.active = 0; \
        } \
        else if (osh_config.active == 0) \
        { \
            log_error(OSH_TC, "OpenSHMEM was already finalized\n" ); \
        } \
        else \
        { \
            osh_config.active--; \
            shmem_barrier_all(); \
        } \
    }

#define _shmem_sync_result(_x_)   \
    { \
        if (osh_config.active) \
        { \
            *osh_config.rc = (_x_); \
            shmem_barrier_all(); \
            if ((_x_) == 0) \
            { \
                int i = 0; \
                for (i = 0; i < osh_config.num_pe; i++) \
                { \
                    _x_ = shmem_int_g(osh_config.rc, i); \
                    if (_x_ != 0) break; \
                } \
            } \
            shmem_barrier_all(); \
            *osh_config.rc = 0; \
        } \
        else \
        { \
            log_error(OSH_TC, "OpenSHMEM has been not started\n" ); \
        } \
    }

#define _shmem_result_pe_list(_x_)   \
    { \
        if (osh_config.active) \
        { \
            if (osh_config.exec_mode.log_pe_list != INVALID_PE) \
            { \
                *osh_config.rc = (_x_); \
            } \
            shmem_barrier_all(); \
            if (((_x_) == 0) && (PE_LIST_CHECK(osh_config.my_pe, osh_config.exec_mode.log_pe_list))) \
            { \
                int i = 0; \
                for (i = 0; i < osh_config.num_pe; i++) \
                { \
                    _x_ = shmem_int_g(osh_config.rc, i); \
                    if (_x_ != 0) break; \
                } \
            } \
            *osh_config.rc = 0; \
        } \
        else \
        { \
            log_error(OSH_TC, "OpenSHMEM has been not started\n" ); \
        } \
    }


const TE_NODE* find_node(const TE_NODE*, const char *);
char* display_opt(const TE_NODE *, char *, size_t);

enum{
    PUT_IN_PROGRESS,
    PUT_COMPLETED
};

static inline void do_progress(void)
{
#if defined(HAVE_OPAL_PROGRESS)
	extern void opal_progress(void);
	opal_progress();
#endif
}

void wait_for_put_completion(int pe, double timeout);
BOOL compare_buffer(unsigned char *buf1, unsigned char *buf2, size_t size, size_t* odd_pos);
BOOL compare_longdouble_buffers(long double *buf1, long double *buf2, size_t size, size_t* odd_pos);
void random_buffer(unsigned char *buf, size_t size);
void show_buffer(void *buf, size_t size);
int compare_buffer_with_const(void *buf, size_t buf_elements, void *value, size_t value_size);
int compare_buffer_with_const_longdouble(long double *buf, size_t buf_elements, long double value);
void fill_buffer(void *buf, size_t buf_elements, void *value, size_t value_size);

unsigned long long random_value(unsigned long long min_value, unsigned long long max_value);
unsigned long long set_pe_list(const char * pe_list);

void spin_wait(unsigned long usec);

void get_mstat(TE_MEM *);
char * get_rc_string(int rc);
char * get_rc_color(int rc);
int get_heap_size();

#define MEMHEAP_BASE_MIN_SIZE (1ULL<<21)
static INLINE unsigned long long memheap_align(unsigned long top) {
    return ((top + MEMHEAP_BASE_MIN_SIZE - 1) & ~(MEMHEAP_BASE_MIN_SIZE - 1));
}
unsigned long long memheap_size(void);

static INLINE unsigned sys_log2(unsigned long long val)
{
    /* add 1 if val is NOT a power of 2 (to do the ceil) */
    unsigned int count = (val & (val - 1) ? 1 : 0);

    while (val > 0) {
        val = val >> 1;
        count++;
    }

    return count > 0 ? count - 1 : 0;
}
#endif /* _OSH_CMN_H_ */

