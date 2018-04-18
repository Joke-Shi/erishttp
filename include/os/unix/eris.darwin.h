#ifndef __ERIS_DARWIN_H__
#define __ERIS_DARWIN_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief: 
 **       : Config of Mac os.
 **
 ******************************************************************************/

#include "eris.config.h"

#include "core/eris.macro.h"


#ifdef __cplusplus
extern "C" {
#endif


#define _BSD_SOURCE

#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>               /** tm, timespec */
#include <stdarg.h>
#include <stddef.h>             /** offsetof */
#include <string.h>
#include <regex.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <glob.h>
#include <pthread.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sched.h>

/** eris_run_sched_yield */
#define eris_run_sched_yield() sched_yield()


#if (ERIS_HAVE_LIMITS_H)
#include <limits.h>
#endif

#if (ERIS_HAVE_CRYPT_H)
#include <crypt.h>
#endif

/** System */
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>          /** FIONBIO */
#include <sys/resource.h>
#include <sys/mount.h>          /** statfs  */
#include <sys/utsname.h>        /** uname   */

#if (ERIS_HAVE_POLL_H)
#include <poll.h>               /** poll */
#endif

#if (ERIS_HAVE_SYS_EVENT_H)
#include <sys/event.h>          /** kqueue */
#endif

#if (ERIS_HAVE_AIO_H)
#include <aio.h>                /** aio_xxx */
typedef struct aiocb  eris_aiocb_t;
#endif

#if (ERIS_HAVE_PCRE_H)
#include <pcre.h>
#endif

/** socket */
#include <sys/socket.h>         /** socket      */
#include <netinet/in.h>         /** sockaddr_in, htons */
#include <arpa/inet.h>          /** inet_addr   */
#include <netinet/tcp.h>        /** TCP_NODELAY */
#include <netdb.h>              /** getaddrinfo */
#include <sys/un.h>             /** sockaddr_un */


#define ERIS_CMSG_SPACE         CMSG_SPACE
#define ERIS_CMSG_LEN           CMSG_LEN
#define ERIS_CMSG_DATA          CMSG_DATA

/** Get ncpu */
#define eris_get_nprocessor()   sysconf( _SC_NPROCESSORS_ONLN)

/** ENV */
extern char **environ;


#ifdef __cplusplus
}
#endif /** __cplusplus */

#endif /** __ERIS_DARWIN_H__ */


