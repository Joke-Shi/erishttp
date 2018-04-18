#ifndef __ERISHTTP_H__
#define __ERISHTTP_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : The erishttp major header file, and it defined erishttp context.
 **
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"
#include "os/unix/eris.atomic.h"
#include "os/unix/eris.attrfs.h"
#include "os/unix/eris.files.h"
#include "os/unix/eris.socket.h"

#include "core/eris.core.h"

#include "event/eris.event.h"

#include "eris/eris.version.h"
#include "eris/eris.util.h"
#include "eris/eris.signal.h"
#include "eris/eris.slave.h"


/** Mime-type structure */
typedef struct erishttp_mime_type_s erishttp_mime_type_t;
struct erishttp_mime_type_s {
	eris_string_t mime;
	eris_string_t types;
};


/** Erishttp attribute type */
typedef struct erishttp_attrs_s erishttp_attrs_t;
struct erishttp_attrs_s {
    eris_string_t    user;                 /** default: nobody   */
    eris_string_t    group;                /** default: nobody   */
    eris_string_t    pidfile;              /** $prefix/var/run/erishttp.pid */

    eris_uint32_t    daemon:1;             /** default: 1        */
    eris_uint32_t    tcp_nodelay:1;        /** default: 0        */
    eris_uint32_t    tcp_nopush:1;         /** default: 0        */
    eris_uint32_t    log_level:8;          /** default: 2        */
    eris_uint32_t    cpuset_enable;        /** 1/0 enable/disable*/
    eris_uint32_t    zero:20;              /** zero nothing      */

    eris_int_t       worker_n;             /** default: 1        */
    eris_int_t       worker_task_n;        /** default: 8        */
    eris_int_t       rlimit_nofile;        /** default: 4096     */
    eris_int_t      *cpuset;               /** Cpuset map array  */


    eris_string_t    listen;               /** 0.0.0.0:9432      */
    eris_string_t    admin_listen;         /** 127.0.0.1:9433    */
    eris_int_t       backlog;              /** default: 64       */
    eris_int_t       max_connections;      /** default: 4096     */
    eris_event_iot_t event_iot;            /** default: select   */

    eris_int_t       timeout;              /** default: 20       */
    eris_int_t       keepalive;            /** default: 60       */

    eris_int_t       send_buffer_size;     /** default: 8k       */
    eris_int_t       recv_buffer_size;     /** default: 8k       */

    eris_size_t      header_cache_size;    /** default: 4k       */
    eris_size_t      body_cache_size;      /** default: 8k       */
    eris_size_t      url_max_size;         /** default: 1k       */
    eris_size_t      header_max_size;      /** default: 4k       */
    eris_size_t      body_max_size;        /** default: 10m      */

    eris_size_t      log_max_size;         /** default: 64m      */
    eris_string_t    log_path;             /** $prefix/var/log   */
    eris_string_t    doc_root;             /** $prefix/var/www/htdoc */
};


/** Erishttp context type */
typedef struct erishttp_s erishttp_t;
struct erishttp_s {
    eris_pid_t          ppid;              /** Parent pid        */
    eris_string_t       cfile;             /** config file       */
    eris_string_t       prefix;            /** work prefix       */

    eris_cftree_t       cftree;            /** config tree       */
    eris_module_t       mcontext;          /** module context    */

    eris_sock_t         svc_sock;          /** server socket     */
    eris_sock_t         admin_sock;        /** admin socket      */

    eris_pid_t         *slave_pids;        /** workers pids      */
    eris_http_t        *tasks_http;        /** Tasks http context*/
    eris_task_t         tasks;             /** worker tasks      */
    eris_event_t        svc_event;         /** server event      */
    eris_event_t        admin_event;       /** server event      */
    eris_event_queue_t  svc_event_queue;   /** event queue       */

    eris_log_t          access_log;        /** access log ctx    */
    eris_log_t          errors_log;        /** errors log ctx    */
    eris_log_t          module_log;        /** mudule log ctx    */

    erishttp_attrs_t    attrs;             /** erishttp info     */
	eris_list_t         mime_types;        /** mime-types list   */
};


/** Global erishttp context */
extern erishttp_t *const p_erishttp_context;

/** Destroy by master init objects. */
extern eris_none_t erishttp_destroy( eris_bool_t __slave);



#ifdef __cplusplus
}
#endif

#endif /** __ERISHTTP_H__ */

