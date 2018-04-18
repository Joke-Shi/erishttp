#ifndef __ERIS_MEMORY_H__
#define __ERIS_MEMORY_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This is memory mananger module, which specify api eg:
 **        ::    alloc
 **        ::    realloc
 **        ::    calloc
 **        ::    free
 **
 ******************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"


/** The memory function */
#define eris_memory_alloc(s)     malloc(s)
#define eris_memory_realloc(p,s) realloc((p),(s))
#define eris_memory_calloc(n,s)  calloc((n),(s))
#define eris_memory_free(p) \
do { \
    if ( p) { \
        free((p)); (p) = NULL; \
    } \
} while (0)
#define eris_memory_release      free


#define eris_memory_copy(d,s,l)  memcpy( (d), (s), (l))
#define eris_memory_set(p,c,l)   memset( (p), (c), (l))
#define eris_memory_cleanup(p,l) memset( (p), 0x0, (l))



#ifdef __cplusplus
}
#endif

#endif /** __ERIS_MEMORY_H__ */

