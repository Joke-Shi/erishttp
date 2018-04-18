#ifndef __ERIS_ATOMIC_H__
#define __ERIS_ATOMIC_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief: 
 **       : Atomic of eris program.
 **
 ******************************************************************************/

#include "eris.config.h"

#include "core/eris.types.h"


#if defined(__GNUC__)
#define eris_inline inline
#else
#define eris_inline
#endif

#define ERIS_HAVE_SMP 1

/** The GCC4.1 greater version supported inline atomic */
#if (__GNUC__ > 3) && (__GNUC_MINOR__ > 0)

#define eris_atomic_cmp_set(lock, o, v)  __sync_bool_compare_and_swap((lock), (o), (v))
#define eris_atomic_fetch_add(v, add)    __sync_fetch_and_add((v), (add))
#define eris_atomic_fetch_sub(v, sub)    __sync_fetch_and_sub((v), (sub))
#define eris_atomic_fetch_inc(v)         __sync_fetch_and_add((v), 1)
#define eris_atomic_fetch_dec(v)         __sync_fetch_and_sub((v), 1)
#define eris_atomic_barrier()            __sync_synchronize()

#if defined(__i386__) || defined(__x86_64__) || defined(__amd32__) || defined(__amd64__)
#define eris_atomic_cpu_pause() __asm__("pause")
#else
#define eris_atomic_cpu_pause()
#endif

/** x86 */
#elif defined(__i386__) || defined(__x86_64__)
#include "os/unix/eris.atomic_gcc_x86.h"

/** amd64 */
#elif defined(__amd32__) || defined(__amd64__) || defined(__AMD64__)
#include "os/unix/eris.atomic_gcc_amd.h"

/** POWERPC */
#elif defined(__powerpc__) || defined(__powerpc64__)
#include "os/unix/eris.atomic_gcc_powerpc.h"

/** Not atomic */
#else
#include "os/unix/eris.atomic_none.h"

#endif


#endif /** __ERIS_ATOMIC_H__ */


