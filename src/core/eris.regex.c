/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Operator regex.
 **
 ******************************************************************************/

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.core.h"


/**
 * @Brief: Init regex context.
 *
 * @Param: __regex,   Eris regex context.
 * @Param: __pattern, Pattern eris string.
 * @Param: __log,     Pointer log context.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_regex_init( eris_regex_t *__regex, const eris_string_t __pattern, eris_log_t *__log)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __regex) {
        __regex->comp_ok = false;
        __regex->log     = __log;

        if ( __pattern) {
#if (ERIS_HAVE_PCRE_H)
            /** Using pcre regex compile */
            eris_int_t error_offset  = 0;
            const eris_char_t *error = NULL;

            __regex->pcre_regex_ctx = pcre_compile( __pattern, 0, &error, &error_offset, NULL);
            if ( __regex->pcre_regex_ctx) {
                /** Compile Ok... */
                __regex->comp_ok = true;

            } else {
                rc = EERIS_ERROR;

                if ( (eris_size_t) error_offset == eris_string_size( __pattern)) {
                    if ( __log) {
                        eris_log_dump( __log, ERIS_LOG_CORE, "Compile failed: \"%s\" in \"%s\" pattern string", \
                                            error, __pattern);
                    }
                } else {
                    if ( __log) {
                        eris_log_dump( __log, ERIS_LOG_CORE, "Compile failed: \"%s\" in \"%s\" pattern string at \"%s\"", \
                                            error, __pattern, (__pattern + error_offset));
                    }
                }
            }
#else
            /** Using c regex compile */
            rc = regcomp( &(__regex->c_regex_ctx), __pattern, REG_EXTENDED);
            if ( 0 == rc) {
                /** Compile Ok... */
                __regex->comp_ok = true;

            } else {
                if ( __log) {
                    eris_log_dump( __log, ERIS_LOG_CORE, "Compile failed in \"%s\" pattern string, errno.<%d>", __pattern, rc);
                }

                rc = EERIS_ERROR;
            }
#endif
        } else {
            rc = EERIS_ERROR; 

            if ( __log) {
                eris_log_dump( __log, ERIS_LOG_CORE, "Input pattern string is invalid");
            }
        }
    } else { 
        rc = EERIS_ERROR; 

        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input regex context is invalid");
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_regex_init


/**
 * @Brief: Match of input source string.
 * 
 * @Param: __regex, Eris regex context.
 * @Param: __src,   Input source string data.
 * @Param: __size,  Input source string data size.
 *
 * @Return: Has matched is true, Other is false.
 **/
eris_bool_t eris_regex_match( eris_regex_t *__regex, const eris_char_t *__src, eris_size_t __size)
{
    eris_bool_t rc_match = false;

    if ( __regex) {
        if ( __src && (0 < __size)) {
            eris_int_t rc = 0;

#if (ERIS_HAVE_PCRE_H)
            eris_int_t vector_idx[ 1] = {0};

            rc = pcre_exec( __regex->pcre_regex_ctx, NULL, __src, __size, 0, 0, vector_idx, 1);
            if ( 0 == rc) {
                /** Match */
                rc_match = true;
            }
#else
            regmatch_t match_arr_v[ 1];
            rc = regexec( &(__regex->c_regex_ctx), __src, 1, match_arr_v, 0);
            if ( 0 == rc) {
                /** Ok, match */
                rc_match = true;
            }
#endif
        }
    }

    return rc_match;
}/// eris_regex_match


/**
 * @Brief: Match of input eris source string.
 * 
 * @Param: __regex,  Eris regex context.
 * @Param: __src_es, Input source eris string data.
 *
 * @Return: Has matched is true, Other is false.
 **/
eris_bool_t eris_regex_match_eris( eris_regex_t *__regex, const eris_string_t __src_es)
{
    eris_bool_t rc_match = false;

    if ( __regex) {
        if ( __src_es) {
            /** Call common match */
            rc_match = eris_regex_match( __regex, __src_es, eris_string_size( __src_es));
        }
    }

    return rc_match;
}/// eris_regex_match_eris


/**
 * @Brief: Destroy regex context.
 *
 * @Parm: __regex, Eris regex context.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_regex_destroy( eris_regex_t *__regex)
{
    if ( __regex) {
        __regex->log     = NULL;

        if ( __regex->comp_ok) {
#if (ERIS_HAVE_PCRE_H)
            /** Release pcre regex context */
            if ( __regex->pcre_regex_ctx) {
                pcre_free( __regex->pcre_regex_ctx);
                __regex->pcre_regex_ctx = NULL;
            }
#else
            /** Release c regex context */
            regfree( &(__regex->c_regex_ctx));

#endif
        }

        __regex->comp_ok = false;
    }
}/// eris_regex_destroy



