#ifndef __ERIS_ATOMIC_GCC_AMD_H__
#define __ERIS_ATOMIC_GCC_AMD_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief: 
 **       : Atomic of gcc AMD.
 **
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


#if (ERIS_HAVE_SMP)
#define ERIS_SMP_LOCK "lock \n"
#else
#define ERIS_SMP_LOCK 
#endif


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
    unsigned char rc = 0;

    if ( __lock) {
        __asm__ volatile (
            ERIS_SMP_LOCK
            "cmpxchgq %3, %1 \n"
            "sete     %0     \n"

            : "=a"(rc) 
            : "m"( *__lock), "a" (__o), "r"(__v)
            : "cc", "memory"
        );
    }

    return rc;
}/// eris_atomic_cmp_set


/**
 * @Brief: Add a specify value by source value of atomic.
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
    if ( __v) {
        __asm__ volatile (
            ERIS_SMP_LOCK
            "addq %0, %1 \n"
            
            : "+r"(__add)
            : "m"(*__v)
            : "cc", "memory"
        );
    }

    return __add;
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
    if ( __v) {
        __asm__ volatile (
            ERIS_SMP_LOCK
            "subq %0, %1 \n"
            
            : "+r"(__sub)
            : "m"(*__v)
            : "cc", "memory"
        );
    }

    return __sub;
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
    if ( __v) {
        __asm__ volatile (
            ERIS_SMP_LOCK
            "incq %0 \n"

            :"+m"(*__v)
        );
    }
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
    if ( __v) {
        __asm__ volatile (
            ERIS_SMP_LOCK
            "decq %0 \n"

            :"+m"(*__v)
        );
    }
}/// eris_atomic_fetch_dec


/** Disable the gcc reorder optimizations of write operation go in a program order */
#define eris_atomic_barrier()    __asm__ volatile ("":::"memory") 
#define eris_atomic_cpu_pause()  __asm__("pause")


#ifdef __cplusplus
extern "C" {
#endif

#endif /** __ERIS_ATOMIC_GCC_AMD_H__ */


