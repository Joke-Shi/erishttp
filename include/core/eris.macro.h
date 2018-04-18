#ifndef __ERIS_MACRO_H__
#define __ERIS_MACRO_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : The macro of eris project.
 **
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"


#define ERIS_IS_ALIGN(A, n)     (0 == ((A) & ((n) - 1)))
#define ERIS_ALIGN_DOWN(A, n)   ((A) & ~((n) - 1))
#define ERIS_ALIGN_UP(A, n)     (((A) + ((n) - 1)) & ~((n) - 1))

/** ERIS_ALIGN_OFFSET */
#define ERIS_ALIGN_OFFSET(A, n) ((0 == ((A) & ((n) - 1))) ? 0 : \
                                 (((n) - ((n) - ((A) & ((n) - 1))) & ((n) - 1) \
                                )

/** Check is hex char */
#define ERIS_CHAR_IS_HEX(c) ((('0' <= (c)) && ('9' >= (c))) || \
                             (('a' <= (c)) && ('f' >= (c))) || \
                             (('A' <= (c)) && ('F' >= (c))) )



/** End */
#ifdef __cplusplus
}
#endif

#endif /** __ERIS_MACRO_H__ */

