#ifndef __ERIS_CPUSET_H__
#define __ERIS_CPUSET_H__


/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Bind a processor to cpu affinity.
 **
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"


/**
 * @Brief: Bind a processor to cpu affinity.
 *
 * @Param: __cpu, Cpu id number.
 * @Param: __pid, A processor id.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR or some errno.
 **/
extern eris_int_t eris_cpuset_bind( eris_int_t __cpu, eris_pid_t __pid);



#ifdef __cplusplus
}
#endif

#endif /** __ERIS_CPUSET_H__ */



