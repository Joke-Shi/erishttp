#ifndef  __ERIS_TYPES_H__
#define  __ERIS_TYPES_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief: 
 **       : All types of eris program.
 **
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.macro.h"


/** Convenience eris types */
typedef void               eris_void_t;
typedef void               eris_none_t;
typedef void              *eris_arg_t;
typedef char               eris_char_t;
typedef unsigned char      eris_uchar_t;
typedef short              eris_short_t;
typedef unsigned short     eris_ushort_t;
typedef int                eris_int_t;
typedef unsigned int       eris_uint_t;
typedef long               eris_long_t;
typedef unsigned long      eris_ulong_t;
typedef long long          eris_llong_t;
typedef unsigned long long eris_ullong_t;

/** Fiexed-size and underlying types depend on word size and compiler */
typedef int8_t             eris_int8_t;
typedef int16_t            eris_int16_t;
typedef int32_t            eris_int32_t;
typedef int64_t            eris_int64_t;

typedef uint8_t            eris_uint8_t;
typedef uint16_t           eris_uint16_t;
typedef uint32_t           eris_uint32_t;
typedef uint64_t           eris_uint64_t;

typedef size_t             eris_size_t;
typedef ssize_t            eris_ssize_t;

/** eris socket and other type */
typedef int                eris_fd_t;
typedef int                eris_flag_t;
typedef mode_t             eris_mode_t;
typedef int                eris_sock_t;
typedef socklen_t          eris_socklen_t;

typedef key_t              eris_ipckey_t;
typedef dev_t              eris_dev_t;
typedef ino_t              eris_inode_t;
typedef int64_t            eris_ino64_t;
typedef mode_t             eris_mode_t;
typedef nlink_t            eris_nlink_t;
typedef blksize_t          eris_blksize_t;
typedef blkcnt_t           eris_blkcnt_t;
typedef off_t              eris_off_t;
typedef int64_t            eris_off64_t;

typedef uid_t              eris_uid_t;
typedef gid_t              eris_gid_t;
typedef pid_t              eris_pid_t;

typedef rlim_t             eris_rlim_t;
typedef int64_t            eris_rlim64_t;

typedef time_t             eris_time_t;
typedef useconds_t         eris_useconds_t;
typedef suseconds_t        eris_suseconds_t;


/** eris structure type */
typedef struct stat        eris_stat_t;
typedef struct tm          eris_tm_t;
typedef struct timespec    eris_timespec_t;
typedef struct timeval     eris_timeval_t;
typedef struct timezone    eris_timezone_t;
typedef struct flock       eris_flock_t;

/** FILE and dirent */
typedef DIR                eris_dir_t;
typedef FILE               eris_file_t;
typedef struct dirent      eris_dirent_t;

/** Atomic types */
typedef int32_t                     eris_atomic_int_t;
typedef uint32_t                    eris_atomic_uint_t;
typedef volatile eris_atomic_uint_t eris_atomic_t;

#if ERIS_HAVE_STDINT_H
#include <stdint.h>
#endif

#ifdef ERIS_HAVE_STDBOOL_H
#include <stdbool.h>
typedef bool eris_bool_t;
#else 
typedef enum { false = 0, true = 1 } eris_bool_t;
#endif

typedef pthread_t       eris_thread_t;
typedef pthread_mutex_t eris_mutex_t;
typedef pthread_cond_t  eris_cond_t;

#if (ERIS_HAVE_SYS_EVENT_H)
typedef struct kevent      eris_kevent_t;
#endif

#if (ERIS_HAVE_SIGACTION)
typedef struct sigaction   eris_sigaction_t;
#endif

/** The type of eris string */
typedef eris_char_t   *eris_string_t;
typedef eris_string_t *eris_string_array_t;

/** Erishttp tyoes */
typedef struct eris_petree_s  eris_petree_t;     /** eris petree  */
typedef struct eris_cftree_s  eris_cftree_t;     /** eris cftree  */
typedef struct eris_buffer_s  eris_buffer_t;     /** eris buffer  */
typedef struct eris_http_s    eris_http_t;       /** eris http    */
typedef struct eris_list_s    eris_list_t;       /** eris list    */
typedef struct eris_log_s     eris_log_t;        /** eris log     */
typedef struct eris_regex_s   eris_regex_t;      /** eris regex   */
typedef struct eris_task_s    eris_task_t;       /** eris task    */
typedef struct eris_xform_s   eris_xform_t;      /** eris xform   */
typedef struct eris_module_s  eris_module_t;     /** eris service */


/** Eris pthread cleanup push routine */
typedef eris_none_t (*eris_cleanup_routine_t)(eris_void_t *);


#ifdef __cplusplus
}
#endif

#endif /** __ERIS_TYPES_H__ */

