#ifndef __ERIS_UNIX_H__
#define __ERIS_UNIX_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief: 
 **       : Config of unix os.
 **
 ******************************************************************************/

#include "eris.config.h"

#ifdef __cplusplus
extern "C" {
#endif


#if defined(__linux__) || defined( __linux)
/** Ubuntu, RedHat, Centos */
#define  ERIS_LINUX_OS 1
#include "os/unix/eris.linux.h"


#elif defined(__Darwin__) || defined( __APPLE__)
/** Mac OS */
#define  ERIS_DARWIN_OS 1
#include "os/unix/eris.darwin.h"


#elif defined(__FreeBSD__) || defined(__DragonFly__)
/** FreeBSD and DragonFly */
#define  ERIS_FREEBSD_OS 1
#include "os/unix/eris.freebsd.h"


#elif defined(_POSIX_VERSION)
/** Suppport posix OS */
#define  ERIS_POSIX_OS 1
#include "os/unix/eris.posix.h"


#else
/** Unsupport now */
    #error "Unsupported compiler OS."
#endif /** Check OS */


/** Check D_TYPE */
#ifdef D_TYPE
#define ERIS_HAVE_D_TYPE 1
#endif

/** Check AF_LOCAL */
#ifdef AF_LOCAL
#define ERIS_HAVE_AF_LOCAL 1
#endif


#define eris_get_pid() getpid()


#ifdef __cplusplus
}
#endif /** __cplusplus */


#endif /** __ERIS_UNIX_H__ */


