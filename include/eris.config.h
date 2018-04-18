#ifndef  __ERIS_CONFIG_H__
#define  __ERIS_CONFIG_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief:
 **       : Description of platform configuration information.
 **
 ******************************************************************************/

#ifdef __cplusplus 
extern "C" {
#endif


/** Check sigaction */
/* #undef ERIS_HAVE_SIGACTION */

/** Check getime_r */
#define ERIS_HAVE_GMTIME_R      1

/** Check truncate */
#define ERIS_HAVE_TRUNCATE      1

/** Check pread */
#define ERIS_HAVE_PREAD         1

/** Check pwrite */
#define ERIS_HAVE_PWRITE        1 

/** Check getaddrinfo */
#define ERIS_HAVE_GETADDRINFO   1 

/** Check stdint.h */
#define ERIS_HAVE_STDINT_H      1

/** Check stdbool.h */
#define ERIS_HAVE_STDBOOL_H     1

/** Check malloc.h */
#define ERIS_HAVE_MALLOC_H      1

/** Check limits.h */
#define ERIS_HAVE_LIMITS_H      1

/** Check unistd.h */
#define ERIS_HAVE_UNISTD_H      1

/** Check sched.h */
#define ERIS_HAVE_SCHED_H       1

/** Check crypt.h */
#define ERIS_HAVE_CRYPT_H       1

/** Check sys/mount.h */
#define ERIS_HAVE_SYS_MOUNT_H   1

/** Check sys/param.h */
#define ERIS_HAVE_SYS_PARAM_H   1

/** Check sys/statvfs.h */
#define ERIS_HAVE_SYS_STATVFS_H 1

/** Check sys/fileio.h */
/* #undef ERIS_HAVE_SYS_FILEIO_H */

/** Check poll.h */
#define ERIS_HAVE_POLL_H        1

/** Check sys/epoll.h */
#define ERIS_HAVE_SYS_EPOLL_H   1

/** Check sys/event.h */
/* #undef ERIS_HAVE_SYS_EVENT_H */

/** Check sys/devpoll.h */
/* #undef ERIS_HAVE_SYS_DEVPOLL_H */

/** Check aio.h */
#define ERIS_HAVE_AIO_H         1

/** Check pcre.h */
#define ERIS_HAVE_PCRE_H        1



#ifdef __cplusplus 
}
#endif

#endif /** __ERIS_CONFIG_H__ */

