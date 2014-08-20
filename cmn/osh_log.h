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
 * @file osh_log.h
 *
 * @brief This file contains implemnation of logging capacity.
 *
 **/
#ifndef _OSH_LOG_H_
#define _OSH_LOG_H_


#ifdef __cplusplus
extern "C" {
#endif


extern int log_level;


/**
 * @name Names of predefined components
 * @{
 */
#define OSH_STD         "std."
#define OSH_TE          "te.."
#define OSH_TC          "tc.."
/** @}*/


/**
 * @enum LOG_LEVEL
 * @brief Predefined levels of priorities.
 */
typedef enum
{
    LOG_NONE =    0x00,     /**< silent mode */
    LOG_FATAL,              /**< critical conditions */
    LOG_ERR,                /**< error conditions */
    LOG_WARN,               /**< warning conditions */
    LOG_INFO,               /**< informational */
    LOG_TRACE,              /**< normal but significant condition */
    LOG_DEBUG,              /**< debug-level messages */
} LOG_LEVEL;


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
extern int log_init( void );


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
extern int log_exit( void );


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
extern int log_send( const char* name, 
                      LOG_LEVEL priority,
                      const char* file_name,
                      const int line_no,
                      const char* func_name,
                      const char* format,
                      ...);


/**
 * log_fatal
 *
 * @brief
 *    Log a message with LOG_FATAL priority.
 *
 * @param[in]    category       Category name.
 * @param[in]    format         Format specifier for the string to write to media.
 * @param[in]    ...            The arguments for format.
 *
 * @return @a none
 ***************************************************************************/
#define log_fatal( category, format, ... ) \
    log_send(category, LOG_FATAL, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)


/**
 * log_error
 *
 * @brief
 *    Log a message with LOG_ERROR priority.
 *
 * @param[in]    category       Category name.
 * @param[in]    format         Format specifier for the string to write to media.
 * @param[in]    ...            The arguments for format.
 *
 * @return @a none
 ***************************************************************************/
#define log_error( category, format, ... ) \
    log_send(category, LOG_ERR, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)


/**
 * log_warn
 *
 * @brief
 *    Log a message with LOG_WARN priority.
 *
 * @param[in]    category       Category name.
 * @param[in]    format         Format specifier for the string to write to media.
 * @param[in]    ...            The arguments for format.
 *
 * @return @a none
 ***************************************************************************/
#define log_warn( category, format, ... ) \
    log_send(category, LOG_WARN, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)


/**
 * log_info
 *
 * @brief
 *    Log a message with LOG_INFO priority.
 *
 * @param[in]    category       Category name.
 * @param[in]    format         Format specifier for the string to write to media.
 * @param[in]    ...            The arguments for format.
 *
 * @return @a none
 ***************************************************************************/
#define log_info( category, format, ... ) \
    log_send(category, LOG_INFO, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)


/**
 * log_trace
 *
 * @brief
 *    Log a message with LOG_TRACE priority.
 *
 * @param[in]    category       Category name.
 * @param[in]    format         Format specifier for the string to write to media.
 * @param[in]    ...            The arguments for format.
 *
 * @return @a none
 ***************************************************************************/
#define log_trace( category, format, ... ) \
    log_send(category, LOG_TRACE, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)


/**
 * log_debug
 *
 * @brief
 *    Log a message with LOG_DEBUG priority.
 *
 * @param[in]    category       Category name.
 * @param[in]    format         Format specifier for the string to write to media.
 * @param[in]    ...            The arguments for format.
 *
 * @return @a none
 ***************************************************************************/
#define log_debug( category, format, ... ) \
    log_send(category, LOG_DEBUG, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)


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
void log_help(const char* format, ...);


/**
 * log_help
 *
 * @brief
 *    Output information about test item and synchronizes results between all pPEs.
 *
 * @param[in]    node           Node related Test Case.
 * @param[in]    id             Test Item Identifier.
 * @param[in]    rc             Result code.
 *
 * @return @a none
 ***************************************************************************/
#define log_item( node, id, rc ) \
    { \
        _shmem_sync_result(rc); \
        log_trace(OSH_TC, "...TC: %s TI #%d result: %s\n", (node && node->name ? node->name : "unknown" ), (id), ( !(rc) ? "PASS" : "FAIL" ) ); \
    }


#ifdef __cplusplus
}
#endif


#endif /* _OSH_LOG_H_ */

