#ifndef __ERIS_REGEX_H__
#define __ERIS_REGEX_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Operator regex.
 **
 ******************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"


/** Eris-buffer context */
// typedef struct eris_regex_s eris_regex_t;
struct eris_regex_s {
    eris_bool_t comp_ok;       /** Compile ok flag    */
#if (ERIS_HAVE_PCRE_H)
    pcre *pcre_regex_ctx;      /** Pcre regex context */
#else
    regex_t c_regex_ctx;       /** Libc regex context */
#endif
    eris_log_t *log;           /** Dump log context   */
};


/**
 * @Brief: Init regex context.
 *
 * @Param: __regex,   Eris regex context.
 * @Param: __pattern, Pattern eris string.
 * @Param: __log,     Dump log message context.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_regex_init( eris_regex_t *__regex, const eris_string_t __pattern, eris_log_t *__log);


/**
 * @Brief: Match of input source string.
 * 
 * @Param: __regex, Eris regex context.
 * @Param: __src,   Input source string data.
 * @Param: __size,  Input source string data size.
 *
 * @Return: Has matched is true, Other is false.
 **/
extern eris_bool_t eris_regex_match( eris_regex_t *__regex, const eris_char_t *__src, eris_size_t __size);


/**
 * @Brief: Match of input eris source string.
 * 
 * @Param: __regex,  Eris regex context.
 * @Param: __src_es, Input source eris string data.
 *
 * @Return: Has matched is true, Other is false.
 **/
extern eris_bool_t eris_regex_match_eris( eris_regex_t *__regex, const eris_string_t __src_es);


/**
 * @Brief: Destroy regex context.
 *
 * @Parm: __regex, Eris regex context.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_regex_destroy( eris_regex_t *__regex);



#ifdef __cplusplus
}
#endif

#endif /** __ERIS_REGEX_H__ */


