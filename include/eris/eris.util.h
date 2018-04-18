#ifndef __ERIS_UTIL_H__
#define __ERIS_UTIL_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : utils
 **
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"


/**
 * @Brief: Unit string to interger64, eg: 4k -> 4096.
 *
 * @Param: __unit_es, Unit eris string, eg: 4k.
 *
 * @Return: convert result interger64, is input is NULL, then is 0.
 **/
extern eris_int64_t eris_util_unit_toi64( const eris_string_t __unit_es);


/**
 * @Brief: Parse cpuset string and return map array of cpuids.
 *       : cpuset is "1:0 2:1 3:2 4:0" string.
 *
 * @Param: __cpuset_es, cpuset eris string.
 * @Param: __slave_n  , N slave processors.
 *
 * @Return: Ok pointer indexs array, Other is NULL. index 0 is array size.
 **/
extern eris_int_t *eris_util_cpuset_parse( const eris_string_t __cpuset_es, eris_int_t __slave_n);



/**
 * @Brief: Dump pid number to pidfile.
 *
 * @Param: __pidfile, Pid file path.
 * @Param: __pid,     Pid number.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR or EERIS_INVALID or EERIS_EXISTED.
 **/
extern eris_int_t eris_util_dump_pid( const eris_char_t *__pidfile, eris_pid_t __pid);




#ifdef __cplusplus
}
#endif

#endif /** __ERIS_UTIL_H__ */


