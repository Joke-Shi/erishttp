#ifndef __ERIS_TIME_H__
#define __ERIS_TIME_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Operator time.
 **
 ******************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"


/** Get time value */
#define eris_time_get(t) time(&(t))


/**
 * @Brief: Get now time to gmt format string.
 *
 * @Param: __out_gmt, Output gmt time format string.
 *
 * @Return: __out_gmt pointer eris string.
 **/
extern eris_string_t eris_time_gmt( eris_string_t *__out_gmt);


/**
 * @Brief: Convet gmt time format string to tm structure.
 *
 * @Param: __out_tm, Output tm value.
 * @Param: __in_gmt, Input gmt time format string.
 *
 * @Return: Ok is 0, Other is errno.
 **/
extern eris_int_t eris_time_gmt2tm( eris_tm_t *__out_tm, const eris_string_t __in_gmt);



#ifdef __cplusplus
}
#endif

#endif /** __ERIS_TIME_H__ */


