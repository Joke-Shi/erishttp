/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : The erishttp major c file.
 **
 ******************************************************************************/


#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.core.h"

#include "eris/eris.util.h"


/** A char to digit value */
#define eris_util_c2v(c) ((c) - '0')


/**
 * @Brief: Unit string to interger64, eg: 4k -> 4096.
 *
 * @Param: __unit_es, Unit eris string, eg: 4k.
 *
 * @Return: convert result interger64, is input is NULL, then is 0.
 **/
eris_int64_t eris_util_unit_toi64( const eris_string_t __unit_es)
{
    eris_bool_t error = false;

    eris_int64_t rv   = 0;
    eris_int64_t sign = 1;
    eris_int64_t base = 1;

    if ( __unit_es) {
        enum {
            ERIS_UTIL_START  = 0,
            ERIS_UTIL_DIGIT  ,
            ERIS_UTIL_XB     ,
            ERIS_UTIL_FINISH ,
        } eris_util_state_v = ERIS_UTIL_START;

        eris_size_t i    = 0;
        eris_size_t size = eris_string_size( __unit_es);

        for ( i = 0; i < size; i++) {
            eris_char_t c = eris_string_at( __unit_es, i);

            switch ( eris_util_state_v) {
                case ERIS_UTIL_START :
                    {
                        if ( isdigit( c)) {
                            rv    = eris_util_c2v(c);
                            base *= 10;

                            eris_util_state_v = ERIS_UTIL_DIGIT;
                        } else if ('-' == c) {
                            sign = -1;

                            eris_util_state_v = ERIS_UTIL_DIGIT;
                        } else if ('+' == c) {
                            sign = 1;
                                
                            eris_util_state_v = ERIS_UTIL_DIGIT;
                        } else {
                            error = true;
                        }
                    } break;
                case ERIS_UTIL_DIGIT :
                    {
                        if ( isdigit( c)) {
                            rv    = (rv * base) + eris_util_c2v(c);
                            base *= 10LL;

                        } else if ( ('B' == c) || ('b' == c)) {
                            eris_util_state_v = ERIS_UTIL_FINISH;

                        } else if ( ('K' == c) || ('k' == c)) {
                            rv *= 1024LL;

                            eris_util_state_v = ERIS_UTIL_XB;

                        } else if ( ('M' == c) || ('m' == c)) {
                            rv *= (1024 * 1024LL);

                            eris_util_state_v = ERIS_UTIL_XB;

                        } else if ( ('G' == c) || ('g' == c)) {
                            rv *= (1024 * 1024 * 1024LL);

                            eris_util_state_v = ERIS_UTIL_XB;

                        } else if ( ('T' == c) || ('T' == c)) {
                            rv *= (1024 * 1024 * 1024 * 1024LL);

                            eris_util_state_v = ERIS_UTIL_XB;

                        } else {
                            error = true;
                        }
                    } break;
                case ERIS_UTIL_XB :
                    {
                        eris_util_state_v = ERIS_UTIL_FINISH;
                    } break;
                case ERIS_UTIL_FINISH :
                default : break;
            }

            if ( ERIS_UTIL_FINISH == eris_util_state_v) { break; }
            if ( error) { break; }
        }/// for ( i = 0; i < size; i++)
    }

    return (rv * sign);
}/// eris_util_unit_toi64


/**
 * @Brief: Parse cpuset string and return map array of cpuids.
 *       : cpuset is "1:0 2:1 3:2 4:0" string.
 *
 * @Param: __cpuset_es, cpuset eris string.
 * @Param: __slave_n  , N slave processors.
 *
 * @Return: Ok pointer indexs array, Other is NULL. index 0 is array size.
 **/
eris_int_t *eris_util_cpuset_parse( const eris_string_t __cpuset_es, eris_int_t __slave_n)
{
    eris_int_t  rc = 0;
    eris_int_t *p_cpuset_array = NULL;

    eris_int_t array_size = (0 < __slave_n) ? (__slave_n + 1) : 1;

    p_cpuset_array = (eris_int_t *)eris_memory_alloc( sizeof( eris_int_t) * array_size);
    if ( p_cpuset_array) {
        eris_int_t       i = 0;
        p_cpuset_array[ 0] = array_size;

        for ( i = 1; i < array_size; i++) {
            p_cpuset_array[ i] = 0;
        }

        if ( __cpuset_es) {
            enum {
                ERIS_UTIL_CPUSET_PIDX  = 0,
                ERIS_UTIL_CPUSET_COLON,
                ERIS_UTIL_CPUSET_CPUID,
                ERIS_UTIL_CPUSET_JUMP ,
            } eris_util_cpuset_state_v = ERIS_UTIL_CPUSET_PIDX;

            eris_int_t base  = 1;
            eris_int_t pidx  = 0;
            eris_int_t cpuid = 0;

            for ( i = 0 ; i < eris_string_size( __cpuset_es); i++) {
                eris_char_t c = eris_string_at( __cpuset_es, i);

                switch ( eris_util_cpuset_state_v) {
                    case ERIS_UTIL_CPUSET_PIDX :
                        {
                            if ( isdigit( c)) {
                                pidx  = pidx * base + eris_util_c2v(c);
                                base *= 10;

                            } else if ( ':' == c) {
                                base = 1;

                                eris_util_cpuset_state_v = ERIS_UTIL_CPUSET_COLON;

                            } else {
                                if ( (0 < pidx) && (array_size > pidx)) {
                                    p_cpuset_array[ pidx] = cpuid;
                                }

                                base  = 1;
                                pidx  = 0;
                                cpuid = 0;

                                if ( (' ' == c) || ('\t' == c) || ('\v' == c)) {
                                    /** On now state */
                                } else {
                                    eris_util_cpuset_state_v = ERIS_UTIL_CPUSET_JUMP;
                                }
                            }
                        } break;
                    case ERIS_UTIL_CPUSET_COLON :
                        {
                            if ( isdigit( c) ) {
                                eris_util_cpuset_state_v = ERIS_UTIL_CPUSET_PIDX;

                                cpuid = cpuid * base + eris_util_c2v(c);
                                base *= 10;
                            } else {
                                if ( (0 < pidx) && (array_size > pidx)) {
                                    p_cpuset_array[ pidx] = cpuid;
                                }

                                base  = 1;
                                pidx  = 0;
                                cpuid = 0;

                                if ( (' ' == c) || ('\t' == c) || ('\v' == c)) {
                                    eris_util_cpuset_state_v = ERIS_UTIL_CPUSET_PIDX;
                                } else {
                                    eris_util_cpuset_state_v = ERIS_UTIL_CPUSET_JUMP;
                                }
                            }
                        } break;
                    case ERIS_UTIL_CPUSET_CPUID :
                        {
                            if ( isdigit( c) ) {
                                cpuid = cpuid * base + eris_util_c2v(c);
                                base *= 10;
                            } else {
                                if ( (0 < pidx) && (array_size > pidx)) {
                                    p_cpuset_array[ pidx] = cpuid;
                                }

                                base  = 1;
                                pidx  = 0;
                                cpuid = 0;

                                if ( (' ' == c) || ('\t' == c) || ('\v' == c)) {
                                    eris_util_cpuset_state_v = ERIS_UTIL_CPUSET_PIDX;
                                } else {
                                    eris_util_cpuset_state_v = ERIS_UTIL_CPUSET_JUMP;
                                }
                            }
                        } break;
                    case ERIS_UTIL_CPUSET_JUMP :
                        {
                            if ( (' ' == c) || ('\t' == c) || ('\v' == c)) {
                                eris_util_cpuset_state_v = ERIS_UTIL_CPUSET_PIDX;
                            }
                        } break;
                    default : break;
                }

                /** Happen error */
                if ( 0 != rc) { break; }
            }
        }
    } else { /** error */ }

    return p_cpuset_array;
}/// eris_util_cpuset_parse



/**
 * @Brief: Dump pid number to pidfile.
 *
 * @Param: __pidfile, Pid file path.
 * @Param: __pid,     Pid number.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR or EERIS_INVALID or EERIS_EXISTED.
 **/
eris_int_t eris_util_dump_pid( const eris_char_t *__pidfile, eris_pid_t __pid)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __pidfile) {
        eris_fd_t fd = open( __pidfile, O_RDWR | O_CREAT | O_EXCL, 0644);
        if ( -1 != fd) {
            eris_char_t pid_cache[16] = {0};
            
            eris_int_t pid_cache_size = snprintf( pid_cache, sizeof( pid_cache), "%d", __pid);

            /** Dump */
            (eris_none_t )write( fd, pid_cache, pid_cache_size);

            //unlink( __pidfile);
            close( fd);
        } else {
            if ( EEXIST == errno) {
                rc = EERIS_EXISTED;

            } else { rc = EERIS_ERROR; }
        }
    } else { rc = EERIS_INVALID; }

    errno = tmp_errno;

    return rc;
}/// eris_util_dump_pid


