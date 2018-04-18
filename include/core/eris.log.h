#ifndef __ERIS_LOG_H__
#define __ERIS_LOG_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** Brief  :
 **        : This design to dump log message output to specify log file, and 
 **        : support safe-thread api, support level :
 **        :     debug
 **        :     info
 **        :     notice
 **        :     warn
 **        :     error
 **        :     fatal
 **        :     crit
 **        :     emerg
 **        : and so on...
 **
 ******************************************************************************/

#if __cplusplus 
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"



/** All log level number */
typedef enum eris_log_level_e {
    ERIS_LOG_DEBUG = 0,   /** debug  */
    ERIS_LOG_INFO   ,     /** info   */
    ERIS_LOG_NOTICE ,     /** notice */
    ERIS_LOG_WARN   ,     /** warn   */
    ERIS_LOG_ERROR  ,     /** error  */
    ERIS_LOG_FATAL  ,     /** fatal  */
    ERIS_LOG_CRIT   ,     /** crit   */
    ERIS_LOG_ALERT  ,     /** alert  */
    ERIS_LOG_EMERG  ,     /** emerg  */
    ERIS_LOG_CORE   ,     /** core   */
    ERIS_LOG_LVLMAX
} eris_log_level_t;


/** The eris log content type */
//typedef struct eris_log_s eris_log_t;
struct eris_log_s
{
    /** Dump log min level */
    eris_log_level_t min_level;

    /** Dump log max msize */
    eris_size_t      max_size;

    /** Dump into this file */
    eris_string_t    log_file;
};


/** Do macro dump out log message */
#define eris_log_dump(log,level,fmt,...) eris_log_exec((log),(level), __FILE__, __func__, __LINE__, (fmt), ##__VA_ARGS__)


/**
 * @brief: Init-log context, Set level, max log message and log file object.
 *
 * @param: __log_file,  Dump log message out it.
 * @Param: __min_level, Dump out gt this level log message.
 * @Param: __max_size,  Dump out max size log message.
 *
 * @return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_log_init( eris_log_t *__log, const eris_char_t *__log_file, eris_log_level_t __min_level, eris_size_t __max_size);


/**
 * @brief: Dump out the info level log message.
 *
 * @param: __log,   Log context...
 * @Param: __level, Dump out level log message.
 * @Param: __file,  Record execute file name.
 * @Param: __func,  Record execute function done.
 * @Param: __line,  Record done line number.
 * @param: __fmt,   The format of the log message out.
 * @param: ...,     Others parameters...
 *
 * @return: Nothing
 **/
extern eris_void_t eris_log_exec( eris_log_t *__log, eris_log_level_t __level, const eris_char_t *__file, const eris_char_t *__func, eris_int_t __line,  const eris_char_t *__fmt, ...);


/**
 * @Brief: Clean up log context - release object.
 *
 * @Param: __log, Log context.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_log_cleanup( eris_log_t *__log);


/**
 * @Brief: Destroy log context.
 *
 * @Param: __log, Log context.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_log_destroy( eris_log_t *__log);




#if __cplusplus 
}
#endif

#endif /** __ERIS_LOG_H__ */



