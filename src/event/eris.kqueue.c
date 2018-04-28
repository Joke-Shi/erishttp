/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This design event - kqueue I/O, and support api:
 **        :     create
 **        :     add
 **        :     modify 
 **        :     delete
 **        :     dispatch 
 **        :     over 
 **        :     destroy 
 **
 ******************************************************************************/

#include "eris.config.h"

#include "event/eris.kqueue.h"
#include "event/eris.event.h"

#include "core/eris.core.h"


/**
 * @Brief: Init kqueue context handler.
 *
 * @Param: __event, Event context handler.
 * 
 * @Return: Ok is 0, Other is -1.
 **/
eris_int_t eris_kqueue_init( eris_event_t *__event)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __event) {
#if (ERIS_HAVE_SYS_EVENT_H)
        __event->context.kqueue.size = __event->max_events;

        __event->context.kqueue.kevents = NULL;

        __event->context.kqueue.fd = kqueue();
        if ( -1 != __event->context.kqueue.fd) {
            eris_size_t kevents_size = (sizeof( eris_kevent_t) * __event->context.kqueue.size);

            /** Create event cache */
            __event->context.kqueue.kevents = (eris_kevent_t *)eris_memory_alloc( kevents_size);
            if ( __event->context.kqueue.kevents) {
                eris_memory_cleanup( __event->context.kqueue.kevents, kevents_size);

            } else {
                rc = EERIS_ERROR;

#define eris_kqueue_close_fd(fd) close(fd)
                eris_kqueue_close_fd( __event->context.kqueue.fd);
#undef eris_kqueue_close_fd
                __event->context.kqueue.fd = -1;

                if ( __event->log) {
                    eris_log_dump( __event->log, ERIS_LOG_CORE, "Create kqueue failed, errno.<%d>", errno);
                }
            }
        } else {
            rc = EERIS_ERROR;

            if ( __event->log) {
                eris_log_dump( __event->log, ERIS_LOG_CORE, "Create kqueue failed, errno.<%d>", errno);
            }
        }

#endif /** ERIS_HAVE_SYS_EVENT_H */
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_kqueue_init


/**
 * @Brief: Add socket element in kqueue context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is -1.
 **/
eris_int_t eris_kqueue_add( eris_event_t *__event, eris_event_elem_t *__elem)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __event) {
#if (ERIS_HAVE_SYS_EVENT_H)
        if ( 0 <= __event->context.kqueue.fd) {
            if ( __elem && (0 <= __elem->sock)) {
                struct kevent kev;

                /** Add read event */
                if ( (ERIS_EVENT_ACCEPT & __elem->events) || 
                     (ERIS_EVENT_READ   & __elem->events)) {
                    EV_SET( &kev, __elem->sock, EVFILT_READ, EV_ADD, 0, 0, NULL);
                    
                    rc = kevent( __event->context.kqueue.fd, &kev, 1, NULL, 0, NULL);
                    if ( (-1 == rc) || (EV_ERROR & kev.flags)) {
                        rc = EERIS_ERROR;

                        if ( __event->log) {
                            eris_log_dump( __event->log, ERIS_LOG_CORE, "Add socket fd.<%d> failed, errno.<%d>", __elem->sock, errno);
                        }
                    }
                }

                /** Ok, and then add output event */
                if ( 0 == rc) {
                    if ( ERIS_EVENT_WRITE & __elem->events) {
                        EV_SET( &kev, __elem->sock, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
                        
                        rc = kevent( __event->context.kqueue.fd, &kev, 1, NULL, 0, NULL);
                        if ( (-1 == rc) || (EV_ERROR & kev.flags)) {
                            rc = EERIS_ERROR;

                            /** Rollback */
                            EV_SET( &kev, __elem->sock, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                            (eris_none_t)kevent( __event->context.kqueue.fd, &kev, 1, NULL, 0, NULL);

                            if ( __event->log) {
                                eris_log_dump( __event->log, ERIS_LOG_CORE, "Add socket fd.<%d> failed, errno.<%d>", __elem->sock, errno);
                            }
                        }
                    }
                }
            }
        } else {
            rc = EERIS_ERROR;

            if ( __event->log) {
                eris_log_dump( __event->log, ERIS_LOG_CORE, "kqueue fd is -1 invalid");
            }
        }
#endif /** ERIS_HAVE_SYS_EVENT_H */
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_kqueue_add


/**
 * @Brief: Modify socket element in kqueue context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is -1.
 **/
eris_int_t eris_kqueue_modify( eris_event_t *__event, eris_event_elem_t *__elem)
{
    eris_int_t rc = 0;

    if ( __event) {
#if (ERIS_HAVE_SYS_EVENT_H)
        if ( 0 <= __event->context.kqueue.fd) {
            if ( __elem && (0 <= __elem->sock)) {
                eris_event_elem_t del_elem; {
                    del_elem.sock   = __elem->sock;
                    del_elem.events = ERIS_EVENT_READ | ERIS_EVENT_WRITE;
                }

                /** Delete all */
                (eris_none_t )eris_kqueue_delete( __event, &del_elem);

                /** Then and new events */
                rc = eris_kqueue_add( __event, __elem);
            }
        } else {
            rc = EERIS_ERROR;

            if ( __event->log) {
                eris_log_dump( __event->log, ERIS_LOG_CORE, "Epoll fd is -1 invalid");
            }
        }
#endif /** ERIS_HAVE_SYS_EVENT_H */
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_kqueue_modify


/**
 * @Brief: Delete socket element in kqueue context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is -1.
 **/
eris_int_t eris_kqueue_delete( eris_event_t *__event, eris_event_elem_t *__elem)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __event) {
#if (ERIS_HAVE_SYS_EVENT_H)
        if ( 0 <= __event->context.kqueue.fd) {
            if ( __elem && (0 <= __elem->sock)) {
                struct kevent kev;

                /** Delete read event */
                EV_SET( &kev, __elem->sock, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                (eris_none_t )kevent( __event->context.kqueue.fd, &kev, 1, NULL, 0, NULL);

                /** Delete write event */
                EV_SET( &kev, __elem->sock, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
                (eris_none_t )kevent( __event->context.kqueue.fd, &kev, 1, NULL, 0, NULL);
            }
        } else {
            rc = EERIS_ERROR;

            if ( __event->log) {
                eris_log_dump( __event->log, ERIS_LOG_CORE, "Epoll fd is -1 invalid");
            }
        }
#endif /** ERIS_HAVE_SYS_EVENT_H */
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_kqueue_delete


/**
 * @Brief: kqueue dispatcher.
 *
 * @Param: __event,    Event context handler.
 * @Param: __event_cb, Happen event and callback.
 * @Param: __arg,      Input callback argument.
 *
 * @Return: Ok is 0, other is -1.
 **/
eris_int_t eris_kqueue_dispatch( eris_event_t *__event, eris_event_cb_t __event_cb, eris_arg_t __arg)
{
    eris_int_t rc = 0;

    if ( __event) {
#if (ERIS_HAVE_SYS_EVENT_H)
        if ( 0 <= __event->context.kqueue.fd) {
            do {
                eris_int_t ready_n = 0;

                /** Set over of dispatcher */
                if ( 0 < __event->over) {
                    break;
                }

                if ( __event->log) {
                    eris_log_dump( __event->log, ERIS_LOG_DEBUG, "Has event nodes count.%d", __event->nodes_count);
                }

                eris_timespec_t ts; {
                    ts.tv_sec  = __event->timeout;
                    ts.tv_nsec = 0;
                }

                ready_n = kevent( __event->context.kqueue.fd, 
                                  NULL,
                                  0,
                                  __event->context.kqueue.kevents,
                                  __event->context.kqueue.size,
                                  &ts);
                if ( 0 < ready_n) {
                    eris_int_t i = 0;

                    /** Has ready_n socket object happen event */
                    for ( i = 0; i < ready_n; i++) {
                        eris_bool_t is_accept_sock = false;

                        eris_event_elem_t ev_elem; {
                            ev_elem.sock   = -1;
                            ev_elem.events = ERIS_EVENT_NONE;
                        }

                        ev_elem.sock = (eris_sock_t )(__event->context.kqueue.kevents[ i].ident);

                        eris_int16_t  ev_filter = __event->context.kqueue.kevents[ i].filter;
                        eris_uint16_t ev_flags  = __event->context.kqueue.kevents[ i].flags;

                        /** Check is accept socket object? */
                        {
                            eris_event_node_t *cur_node = __event->accept_nodes;

                            for ( ; (NULL != cur_node); cur_node = cur_node->next) {
                                if ( ev_elem.sock == cur_node->elem.sock) {
                                    is_accept_sock = true;

                                    break;
                                }
                            }
                        }

                        if ( is_accept_sock) {
                            /** Busy??? */
                            if ( (__event->nodes_count + 1) < __event->max_events) {
                                eris_sock_t client_sock = eris_event_accept( __event, ev_elem.sock);
                                if ( -1 != client_sock) {
                                    eris_event_elem_t ev_client_elem; {
                                        ev_client_elem.sock   = client_sock;
                                        ev_client_elem.events = ERIS_EVENT_READ;
                                    }

                                    if ( __event->log) {
                                        eris_socket_host_t client_host;

                                        if ( 0 == eris_socket_host( client_sock, &client_host)) {
                                            eris_log_dump( __event->log, ERIS_LOG_INFO, 
                                                           "Pid.%d - connect from client.(%s:%d) fd.%d",
                                                           eris_get_pid(),
                                                           client_host.ipv4, 
                                                           client_host.port,
                                                           client_sock);
                                        }
                                    }

                                    rc = eris_event_add( __event, &ev_client_elem);
                                    if ( 0 != rc) {
                                        eris_socket_close( client_sock);
                                    }
                                }
                            }
                        } else {
                            if ( EV_ERROR & ev_flags ) {
                                ev_elem.events = ERIS_EVENT_ERROR;

                            } else {
                                if ( EVFILT_READ == ev_filter) {
                                    ev_elem.events |= ERIS_EVENT_READ;
                                }

                                if ( EVFILT_WRITE == ev_filter) {
                                    ev_elem.events |= ERIS_EVENT_WRITE;
                                }
                            }

                            if ( ERIS_EVENT_NONE != ev_elem.events) {
                                eris_event_elem_t del_elem; {
                                    del_elem.sock   = ev_elem.sock;
                                    del_elem.events = ERIS_EVENT_READ | ERIS_EVENT_WRITE;
                                }

                                eris_event_delete( __event, &del_elem);

                                /** Do event callback output */
                                if ( __event_cb) { __event_cb( &ev_elem, __arg); }
                            }
                        }
                    }/// for ( i = 0; i < ready_n; i++)

                } else {
                    /** Timer event heppen */
                    if ( 0 == ready_n) {
                        eris_event_elem_t timer_elem; {
                            timer_elem.sock   = -1;
                            timer_elem.events = ERIS_EVENT_TIMER;
                        }

                        /** Doing callback */
                        if ( __event_cb) { __event_cb( &timer_elem, __arg); }
                    }
                }

                /** Clean up timeout socket object */
                eris_event_filing( __event, __event_cb, __arg);

            } while ( 1);
        } else {
            rc = EERIS_ERROR;

            if ( __event->log) {
                eris_log_dump( __event->log, ERIS_LOG_CORE, "kevent fd is -1 invalid");
            }
        }

        /** Record dispatch will over */
        {
            pthread_mutex_lock( &(__event->mutex));

            __event->over++;

            pthread_mutex_unlock( &(__event->mutex));
        }
#endif /** ERIS_HAVE_SYS_EVENT_H */
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_kqueue_dispatch


/**
 * @Brief: Terminate diapatcher.
 *
 * @Param: __event, Event context handler.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_kqueue_over( eris_event_t *__event)
{
    if ( __event) {
#if (ERIS_HAVE_SYS_EVENT_H)

#endif /** ERIS_HAVE_SYS_EVENT_H */
    }

}/// eris_kqueue_over


/**
 * @Brief: Destroy context handler.
 *
 * @Param: __event, Event context handler.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_kqueue_destroy( eris_event_t *__event)
{
    if ( __event) {
#if (ERIS_HAVE_SYS_EVENT_H)

        if ( 0 <= __event->context.kqueue.fd) {
#define eris_kqueue_close_fd(fd) close(fd)
            eris_kqueue_close_fd( __event->context.kqueue.fd);
#undef eris_kqueue_close_fd
            __event->context.kqueue.fd = -1;
        }

        if ( __event->context.kqueue.kevents) {
            eris_memory_free( __event->context.kqueue.kevents);
        }

#endif /** ERIS_HAVE_SYS_EVENT_H */
    }

}/// eris_kqueue_destroy



