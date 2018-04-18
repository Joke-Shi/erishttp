/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Eris spinlock acquire and release.
 **        :
 ******************************************************************************/

#include "core/eris.core.h"


/**
 * @Brief: Get lock.
 *
 * @Param: __lock, A lock pointer.
 * @Param: __v,    Set lock value.
 * @Param: __ncpu, Cpu numbers.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_spinlock_acquire( eris_atomic_t *__lock, eris_atomic_int_t __v, eris_long_t __ncpu)
{
    eris_uint_t i    = 0;
    eris_uint_t spin = 2048;
    eris_uint_t idle = 0;

    for ( ;; ) {
        if ( (0 == *__lock) && (eris_atomic_cmp_set( __lock, 0, __v)) ) {
            /** Get lock ok */
            return;
        }

        if ( 1 < __ncpu) {
            /** idle * 2 */
            for ( idle = 1; idle < spin; idle <<= 1) {
                /** cpu pause */
                for ( i = 0; i < idle; i++) {
                    eris_atomic_cpu_pause();
                }

                /** Get lock */
                if ( (0 == *__lock) && (eris_atomic_cmp_set( __lock, 0, __v)) ) {
                    /** Get lock ok */
                    return;
                }
            }
        }

        /** process sched */
        eris_run_sched_yield();
    }
}/// eris_spinlock_acquire


/**
 * @Brief: Release lock.
 *
 * @Param: __lock, A lock pointer.
 * @Param: __v,    Old lock value.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_spinlock_release( eris_atomic_t *__lock, eris_atomic_int_t __v)
{
    for ( ;; ) {
        /** (*__lock) == __v and unlock */
        if ( __v == (*__lock)) {
            if ( eris_atomic_cmp_set( __lock, __v, 0)) {
                /** Unlock ok */
                return;
            }
        } else { return; }
    }
}/// eris_spinlock_release


