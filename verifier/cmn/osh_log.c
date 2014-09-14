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


/****************************************************************************
 * Static Function Declarations
 ***************************************************************************/
static const char* __log_priority2str( int priority );


/**
 * log_init
 *
 * @brief
 *    The function is used to initialize internal object.
 *    This function should be called at least once before using any other function.
 *    Special structured data can be passed to set initial logging parameters.
 *    In case user defined configuartion is not passed logging sets default parameters.
 *
 * @param none
 *
 * @retval zero - on success
 * @retval non-zero - on failure
 ***************************************************************************/
int log_init( void )
{
    OSH_ERROR status = OSH_ERR_NONE;

    if( osh_config.exec_mode.out_level > LOG_NONE ) 
    {
        if (!osh_config.exec_mode.out_file)
        {
            osh_config.exec_mode.out_file = stderr;
        }
    }

    if( osh_config.exec_mode.log_level > LOG_NONE ) 
    {
        if (osh_config.exec_mode.log_file_name && !osh_config.exec_mode.log_file)
        {
            osh_config.exec_mode.log_file = fopen(osh_config.exec_mode.log_file_name, "w");
            if( osh_config.exec_mode.log_file == NULL )
            {
                status = OSH_ERR_NOT_EXIST;
            }
        }
    }

    return status;
}


/**
 * log_exit
 *
 * @brief
 *    The function is used as a destructor.
 *
 * @param none
 *
 * @retval zero - on success
 * @retval non-zero - on failure
 ***************************************************************************/
int log_exit( void )
{
    OSH_ERROR status = OSH_ERR_NONE;

    if (!osh_config.exec_mode.out_file)
    {
        osh_config.exec_mode.out_file = NULL;
    }

    if (osh_config.exec_mode.log_file)
    {
        fclose(osh_config.exec_mode.log_file);
        osh_config.exec_mode.log_file = NULL;
    }

    return status;
}


/**
 * log_send
 *
 * @brief
 *    Entry point for all logging functions.
 *
 * @param[in]    name           Category name.
 * @param[in]    priority       Information priority.
 * @param[in]    file_name      Name of file where call is located.
 * @param[in]    line_no        Line in the file where call is located.
 * @param[in]    func_name      Function name where call is located.
 *
 * @retval zero - on success
 * @retval non-zero - on failure
 ***************************************************************************/
int log_send( const char* name, 
                      LOG_LEVEL priority,
                      const char* file_name,
                      const int line_no,
                      const char* func_name,
                      const char* format,
                      ...)
{
    OSH_ERROR status = OSH_ERR_NONE;
    char *buf = NULL;
    int buf_size = 256;
    BOOL cr = FALSE;

    /* Current state check */
    if ( !osh_config.active || 
        (osh_config.my_pe == INVALID_PE) || 
        (osh_config.exec_mode.log_pe_list == (unsigned long long)INVALID_PE) )
    {
         return status;
    }

    /* Logging on the defined PE only */
    if ( (!PE_LIST_CHECK(osh_config.my_pe, osh_config.exec_mode.log_pe_list)) )
    {
         return status;
    }

    if ( ((unsigned)osh_config.exec_mode.log_level < priority) && 
         ((unsigned)osh_config.exec_mode.out_level < priority))
    {
        return status;
    }

    buf = (char*)sys_malloc(buf_size);
    if (!buf)
    {
        status = OSH_ERR_NO_MEMORY;
    }

    {
        va_list va;
        int n = 0;

        while (status == OSH_ERR_NONE) 
        {
            va_start(va, format);
            n = vsnprintf(buf, buf_size, format, va);
            va_end(va);

            /* If that worked, return */
            if (n > -1 && n < buf_size)
            {
                break;
            }

            /* Else try again with more space. */
            if (n > -1)     /* ISO/IEC 9899:1999 */
                buf_size = n + 1;
            else            /* twice the old size */
                buf_size *= 2;

            sys_free(buf);
            buf = (char*)sys_malloc(buf_size);
            if (!buf)
            {
                status = OSH_ERR_NO_MEMORY;
            }
        }

        if (status == OSH_ERR_NONE)
        {
            /* get length of data */
            buf_size = (int)sys_strlen(buf);

            /* cut off ended '\n' */
            if ((buf_size > 0) && (buf[buf_size-1] == '\n'))
            {
                buf[buf_size-1] = '\0';
                buf_size--;
                cr = TRUE;
            }
        }
    }

    if ( status == OSH_ERR_NONE )
    {
        UNREFERENCED_PARAMETER(file_name);
        UNREFERENCED_PARAMETER(func_name);
        UNREFERENCED_PARAMETER(line_no);

        /* Output into display */
        if (osh_config.exec_mode.out_file && ((unsigned)osh_config.exec_mode.out_level >= priority))
        {
            if (name && !sys_strcmp(OSH_STD, name))
            {
                fprintf(osh_config.exec_mode.out_file, "%s%s", 
                    buf,
                    (cr ? "\n" : "")
                    );
            }
            else
            {
                fprintf(osh_config.exec_mode.out_file, "[%s] %s: #%02d %s%s", 
                    __log_priority2str(priority),
                    (name ? name : ""),
                    osh_config.my_pe,
                    buf,
                    (cr ? "\n" : "")
                    );
            }
            fflush(osh_config.exec_mode.out_file);
        }

        /* Output into log */
        if (osh_config.exec_mode.log_file && ((unsigned)osh_config.exec_mode.log_level >= priority))
        {
            fprintf(osh_config.exec_mode.log_file, "[%s] %s: #%02d %s%s", 
                __log_priority2str(priority),
                (name ? name : ""),
                osh_config.my_pe,
                buf,
                (cr ? "\n" : "")
                );
             fflush(osh_config.exec_mode.log_file);
       }
    }

    if (buf)
    {
        sys_free(buf);
    }

    return status;
}


/**
 * log_help
 *
 * @brief
 *    Output help message.
 *
 * @param[in]    format         Format specifier for the string to write to media.
 * @param[in]    ...            The arguments for format.
 *
 * @return @a none
 ***************************************************************************/
void log_help(const char* format, ...)
{
    va_list args;

    {
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
    }
}


static const char* __log_priority2str( int priority )
{
    static OSH_CODE priority_list[]=
    {
        {LOG_FATAL,    "fatal"},
        {LOG_ERR,      "error"},
        {LOG_WARN,     "warn "},
        {LOG_INFO,     "info "},
        {LOG_DEBUG,    "debug"},
        {LOG_TRACE,    "trace"},
    };
    static int priority_total = sizeof(priority_list) / sizeof(priority_list[0]);
    int i = 0;

    for (i=0; i<priority_total; i++)
    {
        if (priority_list[i].id == priority)
        {
            return priority_list[i].name;
        }
    }
    
    return ("");
}
