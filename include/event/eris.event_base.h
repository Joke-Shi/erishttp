#ifndef __ERIS_EVENT_BASE_H__
#define __ERIS_EVENT_BASE_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This design event used all base data strunct type.
 **
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"
#include "os/unix/eris.socket.h"

#include "core/eris.types.h"
#include "core/eris.errno.h"
#include "core/eris.spinlock.h"
#include "core/eris.string.h"
#include "core/eris.log.h"
#include "core/eris.time.h"


/** Event module types */
typedef enum   eris_event_iot_e  eris_event_iot_t;
typedef struct eris_event_attr_s eris_event_attr_t;
typedef struct eris_event_elem_s eris_event_elem_t;
typedef struct eris_event_node_s eris_event_node_t;
typedef struct eris_event_s      eris_event_t;

/** Event callback handler */
typedef eris_none_t (*eris_event_cb_t)( eris_event_elem_t *__elem, eris_arg_t __arg);

/** ERIS-select context type */
typedef struct eris_select_s eris_select_t;
struct eris_select_s {
    eris_event_node_t *event_nodes;   /** Monitor events */
    eris_sock_t        max_sock;      /** Max socket fd  */
    fd_set             readable;      /** Readable  set  */
    fd_set             writeable;     /** Writeable set  */
};


/** ERIS-devpoll context type */
typedef struct eris_devpoll_s eris_devpoll_t;
struct eris_devpoll_s {
#if (ERIS_HAVE_SYS_DEVPOLL_H)
#endif
};


/** ERIS-epoll context type */
#if (ERIS_HAVE_SYS_EPOLL_H)
typedef struct epoll_event eris_epollfd_t;
#endif

typedef struct eris_epoll_s eris_epoll_t;
struct eris_epoll_s {
#if (ERIS_HAVE_SYS_EPOLL_H)
    eris_fd_t       fd;      /** epoll_create */
    eris_int_t      size;    /** epoll_create */
    eris_epollfd_t *fds;     /** events cache */
#endif
};


/** ERIS-kqueue context type */
typedef struct eris_kqueue_s eris_kqueue_t;
struct eris_kqueue_s {
#if (ERIS_HAVE_SYS_EVENT_H)
    eris_int_t      fd;      /** Kqueue fd   */
    eris_int_t      size;    /** Evlist size */
    eris_kevent_t  *kevents; /** Event list  */
#endif
};


/** pollfd type */
#if (ERIS_HAVE_POLL_H)
typedef struct pollfd eris_pollfd_t;
#endif

/** ERIS-poll context type */
typedef struct eris_poll_s eris_poll_t;
struct eris_poll_s {
#if (ERIS_HAVE_POLL_H)
    eris_int32_t   size;           /** fds pool size */
    eris_int32_t   current;        /** current fds   */
    eris_pollfd_t *fds;            /** Pointer fds   */
#endif
};


/** Event I/O type values */
enum eris_event_iot_e {
    ERIS_EVENT_IO_SELECT = 0,       /** select  I/O */
    ERIS_EVENT_IO_POLL   ,          /** poll    I/O */
    ERIS_EVENT_IO_EPOLL  ,          /** epoll   I/O */
    ERIS_EVENT_IO_KQUEUE ,          /** kqueue  I/O */
    ERIS_EVENT_IO_DEVPOLL,          /** devpoll I/O */
};


/** Events type flags */
enum {
    ERIS_EVENT_NONE    = 0x0,       /** Nothing     */
    ERIS_EVENT_ACCEPT  = 0x1 <<  0, /** Accept      */
    ERIS_EVENT_READ    = 0x1 <<  1, /** Read        */
    ERIS_EVENT_WRITE   = 0x1 <<  2, /** Write       */
    ERIS_EVENT_CLOSE   = 0x1 <<  3, /** Close       */
    ERIS_EVENT_BUSY    = 0x1 <<  4, /** Busy        */
    ERIS_EVENT_TIMER   = 0x1 <<  5, /** Timer       */
    ERIS_EVENT_ERROR   = 0x1 <<  6, /** Error       */
    ERIS_EVENT_TIMEOUT = 0x1 <<  7, /** Timeout     */
    ERIS_EVENT_OOB     = 0x1 << 30, /** Out of band */
};


/** Event init-attrs */
struct eris_event_attr_s {
    eris_event_iot_t iot;          /** I/O type     */
    eris_int_t       timeout;      /** Unit seconds */
    eris_int_t       keepalive;    /** Keepalive    */
    eris_int_t       max_events;   /** max events   */
    eris_int_t       send_buffer_size;
    eris_int_t       recv_buffer_size;
    eris_int_t       tcp_nodelay:1; 
    eris_int_t       tcp_nopush:1; 
    eris_int_t       zero:30; 
    eris_log_t      *log;          /** Log context  */
};


/** Event element */
struct eris_event_elem_s {
    eris_sock_t  sock;           /** Socket fd      */
    eris_int32_t events;         /** Speicfy events */
};


/** elem1<--->elem2<--->elem3... */
struct eris_event_node_s {
    eris_event_elem_t  elem;     /** Socket element */
    eris_time_t        start;    /** Start monitor  */
    eris_event_node_t *next;     /** Pointer next   */
    eris_event_node_t *prev;     /** Pointer prev   */
};


/** Event init */
typedef eris_int_t (*eris_event_init_t )( eris_event_t *__event);

/** Event add  */
typedef eris_int_t (*eris_event_add_t )( eris_event_t *__event, eris_event_elem_t *__elem);

/** Event modify  */
typedef eris_int_t (*eris_event_modify_t )( eris_event_t *__event, eris_event_elem_t *__elem);

/** Event delete  */
typedef eris_int_t (*eris_event_delete_t )( eris_event_t *__event, eris_event_elem_t *__elem);

/** Event dispatch*/
typedef eris_int_t (*eris_event_dispatch_t )( eris_event_t *__event, eris_event_cb_t __cb, eris_arg_t __arg);

/** Set over flag */
typedef eris_none_t (*eris_event_over_t )( eris_event_t *__event);

/** Event destroy */
typedef eris_none_t (*eris_event_destroy_t )( eris_event_t *__event);


/** Event context handler */
struct eris_event_s {
    eris_mutex_t     mutex;          /** Event mutex       */
    eris_event_iot_t iot;            /** Specify I/O type  */
    eris_atomic_t    over;           /** Dispatch over     */
    eris_int_t       timeout;        /** Default: 20s      */
    eris_int_t       pid;            /** Processor id      */
    eris_int_t       nprocs;         /** Nprocessors       */
    eris_int_t       keepalive;      /** Default: 60s      */
    eris_int_t       max_events;     /** Max events.       */
    eris_int_t       send_buffer_size;
    eris_int_t       recv_buffer_size;
    eris_int_t       tcp_nodelay:1; 
    eris_int_t       tcp_nopush:1; 
    eris_int_t       zero:30; 

    union {
        eris_select_t   select;      /** Event-select  ... */
        eris_poll_t     poll;        /** Event-poll    ... */
        eris_epoll_t    epoll;       /** Event-epoll   ... */
        eris_kqueue_t   kqueue;      /** Event-kqueue  ... */
        eris_devpoll_t  devpoll;     /** Event-devpoll ... */
    } context;

    /** Handlers */
    eris_event_init_t     init_handler;       /** init     */
    eris_event_add_t      add_handler;        /** add      */
    eris_event_modify_t   modify_handler;     /** modify   */
    eris_event_delete_t   delete_handler;     /** delete   */
    eris_event_dispatch_t dispatch_handler;   /** dispatch */
    eris_event_over_t     over_handler;       /** over     */
    eris_event_destroy_t  destroy_handler;    /** destroy  */

    eris_event_node_t    *accept_nodes;   /** accept event */

    /** Element table */
    eris_int_t            nodes_size;     /** Table size   */
    eris_atomic_t         nodes_count;      /** Currents     */
    eris_event_node_t   **nodes;          /** Nodes table  */

    eris_log_t *log;                /** Dump log context   */
};


/** Event element queue */
typedef struct eris_event_queue_s eris_event_queue_t;
struct eris_event_queue_s {
    eris_mutex_t       mutex;              /** Get   mutex */
    eris_cond_t        cond_put;           /** Cond  put   */
    eris_cond_t        cond_get;           /** Cond  get   */
    eris_int_t         max;                /** Queue max   */
    eris_int_t         count;              /** Queue count */
    eris_int_t         front;              /** Front pos   */
    eris_int_t         near;               /** Near  pos   */
    eris_event_elem_t *events;             /** Event pool  */
};



#ifdef __cplusplus
}
#endif

#endif  /** __ERIS_EVENT_BASE_H__ */



