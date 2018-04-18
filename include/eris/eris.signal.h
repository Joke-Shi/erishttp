#ifndef __ERIS_SIGNAL_H__
#define __ERIS_SIGNAL_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Signal doing.
 **
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"


/** eris signal struct type */
typedef struct eris_signal_s eris_signal_t;
struct eris_signal_s {
    eris_int_t    signo;
    eris_char_t  *signame;
    eris_none_t (*sighandler)( eris_int_t __signo);
};


/**
 * @Brief: Init signals.
 *
 * @Param: __log, Dump log message context.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_signal_init( eris_log_t *__log);



#ifdef __cplusplus
}
#endif

#endif /** __ERIS_SIGNAL_H__ */


