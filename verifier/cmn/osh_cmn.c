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
#include <math.h>

#include "shmem.h"

static int _put_completion_flag = PUT_IN_PROGRESS;

void wait_for_put_completion(int pe, double timeout)
{
    double time_start = sys_gettime();
    shmem_fence();
    shmem_int_p(&_put_completion_flag,PUT_COMPLETED,pe);
    while (PUT_COMPLETED != _put_completion_flag &&
           (sys_gettime()-time_start < timeout*1e6)){
        do_progress();
    }

    /* reset completion flag for next usages */
    _put_completion_flag = PUT_IN_PROGRESS;
}

const TE_NODE* find_node(const TE_NODE* node_set, const char *name)
{
    const TE_NODE* node = NULL;
    const TE_NODE* cur_node = node_set;

    while (name && cur_node && cur_node->name)
    {
        if (!sys_strcmp(cur_node->name, name))
        {
            node = cur_node;
        }
        else
        {
            int j = 0;
            for ( j = 0; cur_node->shorts[j] != NULL; j++ )
            {
                if (!sys_strcmp(cur_node->shorts[j], name))
                {
                    node = cur_node;
                }
            }
        }
        if (node) break;
        cur_node++;
    }

    return node;
}


char* display_opt( const TE_NODE *tst, char *buf, size_t buf_size )
{
    char *cur_ptr = buf;

	if ( buf && buf_size ) {
        int cur_len = 0;
        int ret = 0;
        int j = 0;

        ret = snprintf( (cur_ptr + cur_len), (buf_size - cur_len), "%s", tst->name );
        cur_len += (ret < 0 ? 0 : ret );

	    for ( j = 0; (ret >= 0) && (tst->shorts[j] != NULL); j++ ) {
	    	if (j == 0) {
	    		ret = snprintf( (cur_ptr + cur_len), (buf_size - cur_len), " (%s", tst->shorts[j] );
	    	}
	    	else {
	    		ret = snprintf( (cur_ptr + cur_len), (buf_size - cur_len), " ,%s", tst->shorts[j] );
	    	}
	    	cur_len += (ret < 0 ? 0 : ret );
	    }

	    if ((j > 0) && (ret >= 0)) {
	        ret = snprintf( (cur_ptr + cur_len), (buf_size - cur_len), ")" );
	        cur_len += (ret < 0 ? 0 : ret );
	    }
	}

	return (cur_ptr ? cur_ptr : (char *)"");
}


BOOL compare_buffer(unsigned char *buf1, unsigned char *buf2, size_t size, size_t* odd_pos)
{
    size_t i = 0;

    if ( !buf1 || !buf2 )
    {
        return FALSE;
    }

    for ( i = 0; i < size; i++ )
    {
        if ( buf1[i] != buf2[i] )
        {
            if (odd_pos)
            {
                *odd_pos = i;
            }
            return FALSE;
        }
    }

    return TRUE;
}

BOOL compare_longdouble_buffers(long double *buf1, long double *buf2, size_t size, size_t* odd_pos)
{
    size_t i = 0;
    long double comparison_precision = 0.00001;
    if ( !buf1 || !buf2 )
    {
        return FALSE;
    }

    for ( i = 0; i < size; i++ )
    {
        if ((long double)fabs((double)(buf1[i]-buf2[i])) > comparison_precision)
        {
            if (odd_pos)
            {
                *odd_pos = i;
            }
            return FALSE;
        }
    }

    return TRUE;
}

int compare_buffer_with_const_longdouble(long double *buf, size_t buf_elements, long double value)
{
   int result = 0;
   long double comparison_precision = 0.00001;
   if (buf)
    {
        size_t i = 0;
        for (i = 0; i < buf_elements; i++)
        {
            if ((long double)fabs((double)(buf[i]-value)) > comparison_precision)
            {
                result = i + 1;
                break;
            }
        }
    }

    return result;
}
int compare_buffer_with_const(void *buf, size_t buf_elements, void *value, size_t value_size)
{
    int result = 0;

    if (buf && value)
    {
        long i = 0;
        for (i = 0; i < (long)buf_elements; i++)
        {
            if (sys_memcmp((void *)((unsigned char *)buf + i * value_size), value, value_size))
            {
                result = i + 1;
                break;
            }
        }
    }

    return result;
}


void random_buffer(unsigned char *buf, size_t size)
{
    unsigned long random_num = 0;
    size_t i = 0;

    if ( buf )
    {
        for ( i = 0; i < size; i++ )
        {
            random_num = random_value('A', 'z');
            buf[i] = (unsigned char)(random_num & 0xFF);
        }
    }
}


void show_buffer(void *ptr, size_t size)
{
    size_t i, j;
    unsigned char *buf = (unsigned char *)ptr;

    for (i = 0; i < size; i += 16)
    {
        log_debug(OSH_TE, "%06x: ", i);
        for (j = 0; j < 16; j++)
        if ( i+j < size)
            log_debug(OSH_STD, "%02x ", buf[i+j]);
        else
            log_debug(OSH_STD, "   ");
        log_debug(OSH_STD, " ");
        for (j=0; j<16; j++)
            if (i+j < size)
                log_debug(OSH_STD, "%c", isprint(buf[i+j]) ? buf[i+j] : '.');
        log_debug(OSH_STD, "\n");
    }
}


void fill_buffer(void *buf, size_t buf_elements, void *value, size_t value_size)
{
    if (buf && value)
    {
        int64_t i = 0;
        for (i = 0; i < (int64_t)buf_elements; i++)
        {
            sys_memcpy((void *)((unsigned char *)buf + i * value_size), value, value_size);
        }
    }
}


unsigned long long random_value(unsigned long long min_value, unsigned long long max_value)
{
   return ( (min_value >= max_value) ? min_value : min_value + (rand() % (max_value - min_value + 1)));
}


unsigned long long set_pe_list(const char * str_pe_list)
{
    OSH_ERROR status = OSH_ERR_NONE;
    unsigned long long pe_list = 0;

    if (str_pe_list && str_pe_list[0])
    {
        long pe_from = -1;
        long pe_cur = 0;
        char * buf = sys_strdup(str_pe_list);
        char * cur_buf = buf;
        char * cur_ptr = buf;
        char * end_ptr = NULL;

        /* Parse pe list */
        while (cur_buf) {
            if (*cur_ptr == '\0')
            {
                errno = 0;
                pe_cur = sys_strtol(cur_buf, &end_ptr, 0);
                if ( (errno != 0) || (cur_buf == end_ptr) )
                {
                    status = OSH_ERR_BAD_ARGUMENT;
                    break;
                }
                cur_buf = NULL;
            }
            else if (*cur_ptr == ',')
            {
                *cur_ptr = '\0';
                errno = 0;
                pe_cur = sys_strtol(cur_buf, &end_ptr, 0);
                if ( (errno != 0) || (cur_buf == end_ptr) )
                {
                    status = OSH_ERR_BAD_ARGUMENT;
                    break;
                }
                cur_buf = cur_ptr + 1;
                cur_ptr++;
            }
            else if (*cur_ptr == '-') {
                *cur_ptr = '\0';
                errno = 0;
                pe_from = strtol(cur_buf, &end_ptr, 0);
                if ( (errno != 0) || (cur_buf == end_ptr) )
                {
                    status = OSH_ERR_BAD_ARGUMENT;
                    break;
                }
                cur_buf = cur_ptr + 1;
                cur_ptr++;
                continue;
            }
            else
            {
                cur_ptr++;
                continue;
            }

            if ((pe_from <= pe_cur) && (pe_cur < PE_LIST_SIZE))
            {
                if (pe_from == -1) pe_from = pe_cur;

                while ((pe_from <= pe_cur))
                {
                    PE_LIST_SET(pe_from, pe_list);
                    pe_from++;
                }
                pe_from = -1;
            }
            else
            {
                status = OSH_ERR_BAD_ARGUMENT;
                break;
            }
        }

        if ( status != OSH_ERR_NONE )
        {
            log_debug(OSH_TE, "Invalid argument: %s\n", str_pe_list);
            pe_list = 0;
        }

        if (buf)
        {
            sys_free(buf);
        }
    }

    return pe_list;
}


void spin_wait(unsigned long usec)
{
    double now = 0;
    double deadline = 0;;

    now = sys_gettime();
    deadline = now + (double)usec;
    while (sys_gettime() < deadline)
        ;
}


void get_mstat(TE_MEM *mstat)
{
    FILE *f = NULL;
    char temp_buf[256];

    if (mstat)
    {
        snprintf(temp_buf, sizeof(temp_buf), "/proc/%d/statm", getpid());

        f = fopen(temp_buf, "r");
        if(!f)
        {
            log_fatal(OSH_TE, "Can not open: %s\n", temp_buf);
        }
        else
        {
            if(2 != fscanf(f, "%" PRId64 " %" PRId64 "", &(mstat->vm_size), &(mstat->vm_rss)))
            {
                log_fatal(OSH_TE, "Can not read: %s\n", temp_buf);
            }
            else
            {
                mstat->vm_size *= sysconf(_SC_PAGE_SIZE);
                mstat->vm_rss *= sysconf(_SC_PAGE_SIZE);
            }
            fclose(f);
        }
    }
}

char * get_rc_string(int rc){
    switch (rc) {
        case TC_PASS:
            return "PASS";
        case TC_FAIL:
            return "FAIL";
        case TC_SETUP_FAIL:
            return "WARN";
        default:
            return "NONE";
    }
}

char * get_rc_color(int rc){
    switch (rc) {
        case TC_PASS:
            return "\e[32m";
        case TC_FAIL:
            return "\e[31m";
        case TC_SETUP_FAIL:
            return "\e[35m";
        default:
            return "\e[37m";
    }
}

#define DEFAULT_SYMMETRIC_HEAP_SIZE         (256)
#define SIZE_IN_MEGA_BYTES(size_in_mb)      (size_in_mb * 1024 * 1024)
unsigned long long memheap_size (void) {
    char *p;
    unsigned long long factor;
    int idx;
    unsigned long long size;

    p = getenv("SHMEM_HEAP_SIZE");
    if (!p) {
        return (SIZE_IN_MEGA_BYTES (DEFAULT_SYMMETRIC_HEAP_SIZE));
    }
    idx = strlen (p) - 1;
    if (p[idx] == 'k' || p[idx] == 'K') {
        factor = 1024;
    } else if (p[idx] == 'm' || p[idx] == 'M') {
        factor = 1024 * 1024;
    } else if (p[idx] == 'g' || p[idx] == 'G') {
        factor = 1024 * 1024 * 1024;
    } else if (p[idx] == 't' || p[idx] == 'T') {
        factor = 1024UL * 1024UL * 1024UL * 1024UL;
    } else {
        factor = 1;
    }
    size = atoll (p);
    if (!size) {
        log_error(OSH_TC, "Incorrect symmetric heap size %s. Using default heap size\n",
                p);
        return (SIZE_IN_MEGA_BYTES (DEFAULT_SYMMETRIC_HEAP_SIZE));
    }
    return (size * factor);
}

int check_within_active_set(int PE_start, int logPE_stride, int PE_size, int my_pe, int num_pes)
{
#ifdef CHECK_ACTIVE_SET
    int shmem_err_check_active_stride = 1 << logPE_stride;
    if (PE_start < 0 || logPE_stride < 0 || PE_size < 0 || \
        PE_start + (PE_size - 1) * shmem_err_check_active_stride > num_pes) {
        return 0;
    }
    if (! (my_pe >= PE_start && \
           my_pe <= PE_start + (PE_size-1) * shmem_err_check_active_stride && \
           (my_pe - PE_start) % shmem_err_check_active_stride == 0)) {
        return 0;
    }
    return 1;
#else
    return 1;
#endif
}
