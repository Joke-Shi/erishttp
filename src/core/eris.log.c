/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief:
 **       :  This design to dump log message output to specify log file, and 
 **       :  support safe-thread apis:
 **       :     debug
 **       :     info
 **       :     notice
 **       :     warn
 **       :     error
 **       :     fatal
 **       :     crit
 **       :     emerg
 **
 ******************************************************************************/

#include "core/eris.core.h"


/** Truncate the log file to zero size. */
static eris_void_t eris_log_truncate( eris_log_t *__log);


/**
 * @brief: Init-log context, Set level, max log message and log file object.
 *
 * @param: __log_file,  Dump log message out it.
 * @Param: __min_level, Dump out gt this level log message.
 * @Param: __max_size,  Dump out max size log message.
 *
 * @return: If successed is 0, other is -1.
 **/
eris_int_t eris_log_init( eris_log_t *__log, const eris_char_t *__log_file, eris_log_level_t __min_level, eris_size_t __max_size)
{
    eris_int_t rc = 0;
    if ( __log) {
        if ( __log_file) {
            __log->log_file = eris_string_create( __log_file);
            if ( !(__log->log_file)) {
                /** Create failed */
                rc = -1;
            }
        } else { __log->log_file = NULL; }

        if ( (ERIS_LOG_DEBUG <= __min_level) && (ERIS_LOG_LVLMAX > __min_level)) {
            __log->min_level = __min_level;
        } else { __log->min_level = ERIS_LOG_DEBUG; }

        { __log->max_size = __max_size; }
    }

    return rc;
}/// eris_log_init


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
eris_void_t eris_log_exec( eris_log_t *__log, eris_log_level_t __level, const eris_char_t *__file, const eris_char_t *__func, eris_int_t __line, const eris_char_t *__fmt, ...)
{
    if ( __level >= __log->min_level) {
        /** Check log context */
        if ( __log && (__log->log_file) && __fmt) {
            eris_int_t rc = 0;

            /** Append */
            FILE *log_file_context = NULL;
            log_file_context = fopen( __log->log_file, "a+");
            if ( log_file_context) {
                long file_pos = ftell( log_file_context);
                if ( 0 > file_pos) { file_pos = 0; }

                const eris_char_t *level_arr[ 11] = {
                    "DEBUG " ,
                    "INFO  " ,
                    "NOTICE" ,
                    "WARN  " ,
                    "ERROR " ,
                    "FATAL " ,
                    "CRIT  " ,
                    "ATERT " ,
                    "EMERG " ,
                    "E-CORE" ,
                    NULL
                };

                eris_char_t buffer_time[64] = {0};
                eris_time_t time_v  = time( NULL);
                eris_tm_t   tm_v    = *( gmtime( &time_v));

                /** Get GMT format string */
                (void )strftime( buffer_time, sizeof( buffer_time), "%F %H:%M:%S", &tm_v);

                eris_char_t log_content[ 4096] = {0}; {
                    va_list vap;
                    va_start( vap, __fmt);
                    (void )vsnprintf( log_content, (eris_long_t )sizeof( log_content), __fmt, vap);
                    va_end( vap);
                }

                /** Lock log file */
                eris_flock_t lock_w; {
                    lock_w.l_type   = F_WRLCK;
                    lock_w.l_start  = 0;
                    lock_w.l_whence = SEEK_SET;
                    lock_w.l_len    = 0;
                    lock_w.l_pid    = getpid();
                }

                rc = fcntl( fileno( log_file_context), F_SETLKW, &lock_w);
                if ( 0 == rc) {
                    /** Truncate??? */
                    if ( (0 < __log->max_size) && (__log->max_size < (eris_size_t )file_pos)) {
                        (void )eris_log_truncate( __log);
                    }

                    if ( ERIS_LOG_DEBUG == __log->min_level) {
                        fprintf( log_file_context, "%s [%s]: File.{%s->:%s->:%d} - %s\n",
                                buffer_time,
                                level_arr[ __level],
                                __file,
                                __func,
                                __line,
                                log_content
                        );
                    } else {
                        fprintf( log_file_context, "%s [%s]: %s\n",
                                buffer_time,
                                level_arr[ __level],
                                log_content
                        );
                    }

                    /** Flush log message */
                    (void)fflush( log_file_context);

                    /** Unlock */
                    {
                        lock_w.l_type   = F_UNLCK;
                        lock_w.l_start  = 0;
                        lock_w.l_whence = 0;
                        lock_w.l_len    = 0;
                        lock_w.l_pid    = 0;
                        (void )fcntl( fileno( log_file_context), F_SETLK, &lock_w);
                    }
                }

                /** Close log file */
                (void )fclose( log_file_context); log_file_context = NULL;
            }
        }
    }
}/// eris_log_exec


/**
 * @Brief: Clean up log context - release object.
 *
 * @Param: __log, Log context.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_log_cleanup( eris_log_t *__log)
{
    if ( __log) {
        /** Free log file memory */
        eris_string_free( __log->log_file);
        eris_string_init( __log->log_file);

        __log->min_level = ERIS_LOG_DEBUG;
        __log->max_size  = 0;
    }
}/// eris_log_cleanup


/**
 * @Brief: Destroy log context.
 *
 * @Param: __log, Log context.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_log_destroy( eris_log_t *__log)
{
    if ( __log) {
        /** Free log file memory */
        eris_string_free( __log->log_file);
        eris_string_init( __log->log_file);

        __log->min_level = ERIS_LOG_DEBUG;
        __log->max_size  = 0;
    }
}/// eris_log_destroy


/**
 * @Brief: Truncate the log file to zero size.
 *
 * @Param: __log, Log context.
 *
 * @Return: Nothing.
 **/
static eris_void_t eris_log_truncate( eris_log_t *__log)
{
    eris_file_t *tmp_file_context = fopen( __log->log_file, "w+");
    if ( tmp_file_context) {
        (void )fclose( tmp_file_context);

        tmp_file_context = NULL;
    }
}/// eris_log_truncate


