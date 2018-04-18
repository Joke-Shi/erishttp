#ifndef __ERIS_SPINLOCK_H__
#define __ERIS_SPINLOCK_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Eris spinlock acquire and release.
 **        :
 ******************************************************************************/

#include "eris.config.h"

#include "os/unix/eris.unix.h"
#include "os/unix/eris.atomic.h"

#include "core/eris.types.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @Brief: Get lock.
 *
 * @Param: __lock, A lock pointer.
 * @Param: __v,    Set lock value.
 * @Param: __ncpu, Cpu numbers.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_spinlock_acquire( eris_atomic_t *__lock, eris_atomic_int_t __v, eris_long_t __ncpu);


/**
 * @Brief: Release lock.
 *
 * @Param: __lock, A lock pointer.
 * @Param: __v,    Old lock value.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_spinlock_release( eris_atomic_t *__lock, eris_atomic_int_t __v);



#ifdef __cplusplus
}
#endif


#endif /** __ERIS_SPINLOCK_H__ */

