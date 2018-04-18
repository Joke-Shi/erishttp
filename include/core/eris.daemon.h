#ifndef __ERIS_DAEMON_H__
#define __ERIS_DAEMON_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This design change the progress state to daemon, apis eg:
 **        :    eris_daemon_create
 **        :
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"


/**
 * @Brief: Change the progress state to daemon.
 *
 * @Param: __dir, Working directory, default "/tmp".
 *
 * @Return: Ok is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_daemon_create( const eris_char_t *__dir);



#ifdef __cplusplus
}
#endif


#endif /** __ERIS_DAEMON_H__ */

