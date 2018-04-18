#ifndef __ERIS_VERSION_H__
#define __ERIS_VERSION_H__


/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief:
 **       : Description erishttp version information.
 **
 ******************************************************************************/

#ifdef __cplusplus 
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"


#if   (ERIS_LINUX_OS)
#define ERIS_VERSION_DESC "erishttpd/0.1.0 (Linux)"

#elif (ERIS_DARWIN_OS)
#define ERIS_VERSION_DESC "erishttpd/0.1.0 (Darwin)"

#elif (ERIS_FREEBSD_OS)
#define ERIS_VERSION_DESC "erishttpd/0.1.0 (FreeBSD)"

#else
#define ERIS_VERSION_DESC "erishttpd/0.1.0 (Posix)"

#endif


#ifdef __cplusplus 
}
#endif

#endif /** __ERIS_VERSION_H__ */

