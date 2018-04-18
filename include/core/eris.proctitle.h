#ifndef __ERIS_PROCTITLE_H__
#define __ERIS_PROCTITLE_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Modify processor title.
 **
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"



/**
 * @Brief: Init proctitle.
 *
 * @Param: __argv, Os argv.
 * @Param: __log,   Dump log context.
 * 
 * @Return: Ok is EERIS_OK, Other (EERIS_ALLOC | EERIS_INVALID)
 **/
extern eris_int_t eris_proctitle_init( char **__argv, eris_log_t *__log);


/**
 * @Brief: Set processor title of format.
 *
 * @Param: __title, Processor titile.
 * @Param: __index, Processor index.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_proctitle_set( const eris_char_t *__title, eris_int_t __index);


/**
 * @Brief: Release new environ pointer memory.
 *
 * @Param: None.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_proctitle_destroy( eris_none_t);



#ifdef __cplusplus
}
#endif

#endif /** __ERIS_PROCTITLE_H__ */


