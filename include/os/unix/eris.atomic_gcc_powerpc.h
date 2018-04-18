#ifndef __ERIS_ATOMIC_GCC_POWERPC_H__
#define __ERIS_ATOMIC_GCC_POWERPC_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief: 
 **       : Atomic of gcc POWERPC.
 **
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
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
    eris_atomic_uint_t rc = 0;
    eris_atomic_uint_t t  = 0;

    if ( __lock) {
        __asm__ volatile (
            "    eieio            \n"    /** Write barrier  */
            "0:                   \n"
            "    lwarx  %1, 0, %2 \n"    /** t = *__lock    */
            "    cmpw   %1, %3    \n"    /** cmp( t == _o)  */
            "    bne    1f        \n"    /** Not equal      */
            "    stwcx  %4, 0, %2 \n"    /** *lock = __v    */
            "    bne    0b        \n"    /** Reload running */
            "    isync            \n"    /** Read barrier   */
            "    li %0, 1         \n"    /** Set true ret   */
            "1:                   \n"    /** nop            */

            : "=&r" (rc), "&=r" (t)
            : "r" (__lock), "r" (__o), "r" (__v)
            : "cc", "memory"
        );
    }

    return rc;
}/// eris_atomic_cmp_set


/**
 * @Brief: Add a specify value by source value of atomic with c syntax :
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
    eris_atomic_int_t t   = 0;

    if ( __v) {
        __asm__ volatile (
            "    eieio              \n"    /** write barrier   */
            "0:  lwarx   %0, 0, %2  \n"    /** old = *__v      */
            "    add     %1, %0, %3 \n"    /** t = old + __add */
            "    stwcx.  %1, 0, %2  \n"    /** *__v = t        */
            "    bne     0b         \n"    /** Reload running  */
            "    isync              \n"    /** Read barrier    */

            : "=&r" (old), "=&r" (t)
            : "r" (__v), "r" (__add)
            : "cc", "memory"
        );
    }

    return old;
}/// eris_atomic_fetch_add


/**
 * @Brief: Sub a specify value by source value of atomic with c syntax:
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
    eris_atomic_int_t t   = 0;

    if ( __v) {
        __asm__ volatile (
            "    eieio              \n"    /** write barrier   */
            "0:  lwarx   %0, 0, %2  \n"    /** old = *__v      */
            "    sub     %1, %0, %3 \n"    /** t = old - __sub */
            "    stwcx.  %1, 0, %2  \n"    /** *__v = t        */
            "    bne     0b         \n"    /** Reload running  */
            "    isync              \n"    /** Read barrier    */

            : "=&r" (old), "=&r" (t)
            : "r" (__v), "r" (__sub)
            : "cc", "memory"
        );
    }

    return old;
}/// eris_atomic_fetch_sub


/**
 * @Brief: Incement a input value of atomic with c syntax :
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
    eris_atomic_int_t old = 0;
    eris_atomic_int_t t   = 0;

    if ( __v) {
        __asm__ volatile (
            "    eieio              \n"    /** write barrier   */
            "0:  lwarx   %0, 0, %2  \n"    /** old = *__v      */
            "    add     %1, 1, %0  \n"    /** t = old + 1     */
            "    stwcx.  %1, 0, %2  \n"    /** *__v = t        */
            "    bne     0b         \n"    /** Reload running  */
            "    isync              \n"    /** Read barrier    */

            : "=&r" (old), "=&r" (t)
            : "r" (__v)
            : "cc", "memory"
        );
    }
}/// eris_atomic_fetch_inc


/**
 * @Brief: Decrement a intput value of atomic with c syntax :
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
    eris_atomic_int_t old = 0;
    eris_atomic_int_t t   = 0;

    if ( __v) {
        __asm__ volatile (
            "    eieio              \n"    /** write barrier   */
            "0:  lwarx   %0, 0, %2  \n"    /** old = *__v      */
            "    sub     %1, 1, %0  \n"    /** t = old - 1     */
            "    stwcx.  %1, 0, %2  \n"    /** *__v = t        */
            "    bne     0b         \n"    /** Reload running  */
            "    isync              \n"    /** Read barrier    */

            : "=&r" (old), "=&r" (t)
            : "r" (__v)
            : "cc", "memory"
        );
    }
}/// eris_atomic_fetch_dec


/** Disable the gcc reorder optimizations of write operation go in a program order */
#if (ERIS_HAVE_SMP)
#define eris_atomic_barrier()    __asm__ volatile ("isync    \n eieio    \n":::"memory") 
#else
#define eris_atomic_barrier()    __asm__ volatile ("":::"memory") 
#endif

/** Nothing */
#define eris_atomic_cpu_pause()


#ifdef __cplusplus
}
#endif

#endif /** __ERIS_ATOMIC_GCC_POWERPC_H__ */


