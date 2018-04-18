#ifndef __ERIS_SLAVE_H__
#define __ERIS_SLAVE_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Worker slave :
 **        :    Get request data and parse protocol, and then execute user service,
 **        :    and result data send response to client.
 **
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.core.h"



/**
 * @Brief: Eris slave handler of task callback.
 *
 * @Param: __arg, It is eris http context.
 *
 * @Return: Nothing is NULL.
 **/
extern eris_void_t *eris_slave_handler( eris_void_t *__arg);



#ifdef __cplusplus
}
#endif

#endif /** __ERIS_SLAVE_H__ */


