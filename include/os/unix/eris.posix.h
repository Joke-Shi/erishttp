#ifndef __ERIS_POSIX_H__
#define __ERIS_POSIX_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief: 
 **       : Config of posix.
 **
 ******************************************************************************/
#include "eris.config.h"

#include "core/eris.macro.h"


#ifdef __cplusplus
extern "C" {
#endif


#define _POSIX_SOURCE

#include <sys/types.h>
#include <sys/time.h>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>             /** offsetof */
#include <time.h>               /** tm, timespec */
#include <string.h>
#include <regex.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <glob.h>
#include <ctype.h>
#include <pthread.h>
#include <dlfcn.h>
#include <fcntl.h>

#if (ERIS_HAVE_SCHED_H)
#include <sched.h>
#define eris_run_sched_yield() sched_yield()

#elif (ERIS_HAVE_UNISTD_H)
#include <unistd.h>
#define eris_run_sched_yield() usleep(1)

#else
#define eris_run_sched_yield()
#endif


#if (ERIS_HAVE_LIMITS_H)
#include <limits.h>
#endif

#if (ERIS_HAVE_MALLOC_H)
#include <malloc.h>             /** memalign */
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
#include <sys/select.h>         /** select  */

#if (ERIS_HAVE_SYS_MOUNT_H)
#include <sys/mount.h>
#endif


#if (ERIS_HAVE_SYS_PARAM_H)     /** statfs */
#include <sys/param.h>
#endif

#if (ERIS_HAVE_SYS_STATVFS_H)   /** fstatvfs and fstatvfs */
#include <sys/statvfs.h>
#endif

#if (ERIS_HAVE_SYS_FILEIO_H)    /** FIONBIO */
#include <sys/fileio.h>
#endif

#if (ERIS_HAVE_POLL_H)
#include <poll.h>               /** poll */
#endif

#if (ERIS_HAVE_SYS_EPOLL_H)
#include <sys/epoll.h>          /** epoll_xxx */
#endif

#if (ERIS_HAVE_SYS_EVENT_H)
#include <sys/event.h>          /** kqueue */
#endif

#if (ERIS_HAVE_SYS_DEVPOLL_H)
#include <sys/devpoll.h>        /** devpoll */
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


#if defined(__FreeBSD__) || defined(__DragonFly__)
#define ERIS_CMSG_SPACE(x)      (ALIGN(sizeof( struct cmsghdr)) + ALIGN(x))
#define ERIS_CMSG_LEN(x)        (ALIGN(sizeof( struct cmsghdr)) + (x))
#define ERIS_CMSG_DATA(v)       ((unsigned char *)(v) + ALIGN(sizeof( struct cmsghdr)))

#else
#define ERIS_CMSG_SPACE         CMSG_SPACE
#define ERIS_CMSG_LEN           CMSG_LEN
#define ERIS_CMSG_DATA          CMSG_DATA

#endif


/** Get ncpu */
#define eris_get_nprocessor()   sysconf( _SC_NPROCESSORS_ONLN)


/** ENV */
extern char **environ;


#ifdef __cplusplus
}
#endif /** __cplusplus */

#endif /** __ERIS_POSIX_H__ */


