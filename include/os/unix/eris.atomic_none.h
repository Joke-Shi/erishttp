#ifndef __ERIS_ATOMIC_NONE_H__
#define __ERIS_ATOMIC_NONE_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief: 
 **       : Unatomic of eris program.
 **
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

/** A global mutex lock */
static pthread_mutex_t eris_atomic_lock = PTHREAD_MUTEX_INITIALIZER;



/**
 * @Brief: Compare and set of atomic with c syntax :
 *       : if ( __o == *__lock) {
 *       :     *__lock = _v;
 *       :     rc = 1;
 *       : } else {
 *       :     rc = 0;
 *       : }
 *       : GCC 2.7 does not support "+r", used "=a" eax output 
 * 
 * @Param: __lock,Lock pointer.
 * @Param: __o,   Old value.
 * @Param: __v,   Update to this value.
 *
 * @Return: The old source value.
 **/
static eris_inline eris_atomic_uint_t 
eris_atomic_cmp_set( eris_atomic_t *__lock, eris_atomic_uint_t __o, eris_atomic_uint_t __v)
{
    eris_atomic_uint_t rc = 0;

    pthread_cleanup_push( (void (*routine)(void *))pthread_mutex_unlock, (eris_void_t *)&(eris_atomic_lock));
    /** Heppen cancel? */

    pthread_mutex_lock( &eris_atomic_lock);
    if ( __lock) {
        if ( (*__lock) == __o) {
            *__lock = __v;

            rc = 1;
        }
    }
    pthread_mutex_unlock( &eris_atomic_lock);

    pthread_cleanup_pop(0);

    return rc;
}/// eris_atomic_cmp_set


/**
 * @Brief: Sub a specify value by source value of atomic.
 *       : {
 *       :     old   = *__v;
 *       :     *__v += __sub;
 *       : } 
 *       : GCC 2.7 does not support "+r", used "=a" eax output 
 * 
 * @Param: __v,   Pointer the source value.
 * @Param: __add, A add number.
 *
 * @Return: The old source value.
 **/
static eris_inline eris_atomic_int_t 
eris_atomic_fetch_add( eris_atomic_t *__v, eris_atomic_int_t __add)
{
    eris_atomic_int_t old = 0;

    pthread_cleanup_push( (void (*routine)(void *))pthread_mutex_unlock, (eris_void_t *)&(eris_atomic_lock));
    /** Heppen cancel? */

    pthread_mutex_lock( &eris_atomic_lock);
    if ( __v) {
        old   = *__v;
        *__v += __add;
    }
    pthread_mutex_unlock( &eris_atomic_lock);

    pthread_cleanup_pop(0);

    return old;
}/// eris_atomic_fetch_add


/**
 * @Brief: Sub a specify value by source value of atomic.
 *       : {
 *       :     old   = *__v;
 *       :     *__v -= __sub;
 *       : } 
 *       : GCC 2.7 does not support "+r", used "=a" eax output 
 * 
 * @Param: __v,   Pointer the source value.
 * @Param: __sub, A sub number.
 *
 * @Return: The old source value.
 **/
static eris_inline eris_atomic_int_t 
eris_atomic_fetch_sub( eris_atomic_t *__v, eris_atomic_int_t __sub)
{
    eris_atomic_int_t old = 0;

    pthread_cleanup_push( (void (*routine)(void *))pthread_mutex_unlock, (eris_void_t *)&(eris_atomic_lock));
    /** Heppen cancel? */

    pthread_mutex_lock( &eris_atomic_lock);
    if ( __v) {
        old   = *__v;
        *__v -= __sub;
    }
    pthread_mutex_unlock( &eris_atomic_lock);

    pthread_cleanup_pop(0);

    return old;
}/// eris_atomic_fetch_sub


/**
 * @Brief: Incement a input value of atomic.
 *       : {
 *       :     (*__v)++;
 *       : }
 *
 * @Param: __v, Pointer a value.
 *
 * @Return: Nothing.
 **/
static eris_inline void eris_atomic_fetch_inc( eris_atomic_t *__v)
{
    pthread_cleanup_push( (void (*routine)(void *))pthread_mutex_unlock, (eris_void_t *)&(eris_atomic_lock));
    /** Heppen cancel? */

    pthread_mutex_lock( &eris_atomic_lock);
    if ( __v) {
        *__v += 1;
    }
    pthread_mutex_unlock( &eris_atomic_lock);

    pthread_cleanup_pop(0);
}/// eris_atomic_fetch_inc


/**
 * @Brief: Decrement a intput value of atomic.
 *       : {
 *       :     (*__v)--;
 *       : }
 *
 * @Param: __v, Pointer a value.
 *
 * @Return: Nothing.
 **/
static eris_inline void eris_atomic_fetch_dec( eris_atomic_t *__v)
{
    pthread_cleanup_push( (void (*routine)(void *))pthread_mutex_unlock, (eris_void_t *)&(eris_atomic_lock));
    /** Heppen cancel? */

    pthread_mutex_lock( &eris_atomic_lock);
    if ( __v) {
        *__v -= 1;
    }
    pthread_mutex_unlock( &eris_atomic_lock);

    pthread_cleanup_pop(0);
}/// eris_atomic_fetch_dec


#define eris_atomic_barrier()
#define eris_atomic_cpu_pause()

#ifdef __cplusplus
}
#endif

#endif /** __ERIS_ATOMIC_NONE_H__ */


