/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This design event - epoll I/O, and support api:
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

#include "event/eris.epoll.h"
#include "event/eris.event.h"

#include "core/eris.core.h"


/**
 * @Brief: Init epoll context handler.
 *
 * @Param: __event, Event context handler.
 * 
 * @Return: Ok is 0, Other is -1.
 **/
eris_int_t eris_epoll_init( eris_event_t *__event)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __event) {
#if (ERIS_HAVE_SYS_EPOLL_H)
        __event->context.epoll.size = __event->max_events / 2;
        if ( 64 > __event->context.epoll.size) {
            __event->context.epoll.size = 64;
        }

        /** Create epoll fd */
        __event->context.epoll.fd = epoll_create( __event->max_events);
        if ( -1 != __event->context.epoll.fd) {
            /** Alloced memory for fds cache */
            __event->context.epoll.fds = (eris_epollfd_t *)eris_memory_alloc( __event->context.epoll.size * sizeof( eris_epollfd_t));
            if ( __event->context.epoll.fds) {
                eris_int_t i = 0;

                for ( i = 0; i < __event->context.epoll.size; i++) {
                    __event->context.epoll.fds[ i].data.fd = -1;
                    __event->context.epoll.fds[ i].events  = 0;
                }
            } else { 
                /** Alloc pollfd buffer failed */
                rc = EERIS_ERROR; 

                if ( __event->log) {
                    eris_log_dump( __event->log, ERIS_LOG_CORE, "Alloced memory for pollfds cache failed.");
                }
            }
        } else { 
            /** Create epoll fd failed */
            rc = EERIS_ERROR; 

            if ( __event->log) {
                eris_log_dump( __event->log, ERIS_LOG_CORE, "Create epoll fd failed, errno.<%d>", errno);
            }
        }
#endif /** ERIS_HAVE_SYS_EPOLL_H */

    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_epoll_init


/**
 * @Brief: Add socket element in epoll context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is -1.
 **/
eris_int_t eris_epoll_add( eris_event_t *__event, eris_event_elem_t *__elem)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __event) {
#if (ERIS_HAVE_SYS_EPOLL_H)
        if ( 0 <= __event->context.epoll.fd) {
            if ( __elem && (0 <= __elem->sock)) {
                eris_epollfd_t tmp_ev; {
                    tmp_ev.data.fd = __elem->sock;
                    tmp_ev.events  = 0;
                }

                if ( (ERIS_EVENT_ACCEPT & __elem->events) ||
                     (ERIS_EVENT_READ   & __elem->events)) {
                    /** Set readable */
                    //tmp_ev.events |= EPOLLIN | EPOLLET;
                    tmp_ev.events |= EPOLLIN ;
                }

                if ( ERIS_EVENT_WRITE & __elem->events) {
                    /** Set write */
                    //tmp_ev.events |= EPOLLOUT | EPOLLET;
                    tmp_ev.events |= EPOLLOUT ;
                }

                rc = epoll_ctl( __event->context.epoll.fd, EPOLL_CTL_ADD, __elem->sock, &tmp_ev);
                if ( 0 != rc) {
                    if ( EEXIST == errno) {
                        /** Ok-> it is existed */
                        rc = 0;

                    } else {
                        /** Error and set error log dump out */
                        rc = EERIS_ERROR;

                        if ( __event->log) {
                            eris_log_dump( __event->log, ERIS_LOG_CORE, "Add socket fd event failed, errno.<%d>", errno);
                        }
                    }
                }
            }
        } else {
            rc = EERIS_ERROR;

            if ( __event->log) {
                eris_log_dump( __event->log, ERIS_LOG_CORE, "Epoll fd is -1 invalid");
            }
        }
#endif /** ERIS_HAVE_SYS_EPOLL_H */

    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_epoll_add


/**
 * @Brief: Modify socket element in epoll context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is -1.
 **/
eris_int_t eris_epoll_modify( eris_event_t *__event, eris_event_elem_t *__elem)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __event) {
#if (ERIS_HAVE_SYS_EPOLL_H)
        if ( 0 <= __event->context.epoll.fd) {
            if ( __elem && (0 <= __elem->sock)) {
                eris_epollfd_t tmp_ev; {
                    tmp_ev.data.fd = __elem->sock;
                    tmp_ev.events  = 0;
                }

                if ( (ERIS_EVENT_ACCEPT & __elem->events) ||
                     (ERIS_EVENT_READ   & __elem->events)) {
                    /** Set readable */
                    //tmp_ev.events |= EPOLLIN | EPOLLET;
                    tmp_ev.events |= EPOLLIN;
                }

                if ( ERIS_EVENT_WRITE & __elem->events) {
                    /** Set write */
                    //tmp_ev.events |= EPOLLOUT | EPOLLET;
                    tmp_ev.events |= EPOLLOUT ;
                }

                rc = epoll_ctl( __event->context.epoll.fd, EPOLL_CTL_MOD, __elem->sock, &tmp_ev);
                if ( 0 != rc) {
                    if ( ENOENT == errno) {
                        /** Ok-> it is existed */
                        rc = eris_epoll_add( __event, __elem);

                    } else {
                        /** Error and set error log dump out */
                        rc = EERIS_ERROR;

                        if ( __event->log) {
                            eris_log_dump( __event->log, ERIS_LOG_CORE, "Modify socket fd event failed, errno.<%d>", errno);
                        }
                    }
                }
            }
        } else {
            rc = EERIS_ERROR;

            if ( __event->log) {
                eris_log_dump( __event->log, ERIS_LOG_CORE, "Epoll fd is -1 invalid");
            }
        }
#endif /** ERIS_HAVE_SYS_EPOLL_H */

    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_epoll_modify


/**
 * @Brief: Delete socket element in epoll context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is -1.
 **/
eris_int_t eris_epoll_delete( eris_event_t *__event, eris_event_elem_t *__elem)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __event) {
#if (ERIS_HAVE_SYS_EPOLL_H)
        if ( 0 <= __event->context.epoll.fd) {
            if ( __elem && (0 <= __elem->sock)) {
                eris_epollfd_t tmp_ev; {
                    tmp_ev.data.fd = __elem->sock;
                    tmp_ev.events  = EPOLLIN | EPOLLOUT | EPOLLET;
                }

                rc = epoll_ctl( __event->context.epoll.fd, EPOLL_CTL_DEL, __elem->sock, &tmp_ev);
                if ( 0 != rc) {
                    if ( ENOENT == errno) {
                        /** Ok-> it is not registered */
                        rc = 0;

                    } else {
                        /** Error and set error log dump out */
                        rc = EERIS_ERROR;

                        if ( __event->log) {
                            eris_log_dump( __event->log, ERIS_LOG_CORE, "Delete socket fd event failed, errno.<%d>", errno);
                        }
                    }
                }
            }
        } else {
            rc = EERIS_ERROR;

            if ( __event->log) {
                eris_log_dump( __event->log, ERIS_LOG_CORE, "Epoll fd is -1 invalid");
            }
        }
#endif /** ERIS_HAVE_SYS_EPOLL_H */

    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_epoll_delete


/**
 * @Brief: epoll dispatcher.
 *
 * @Param: __event,    Event context handler.
 * @Param: __event_cb, Happen event and callback.
 * @Param: __arg,      Input callback argument.
 *
 * @Return: Ok is 0, other is -1.
 **/
eris_int_t eris_epoll_dispatch( eris_event_t *__event, eris_event_cb_t __event_cb, eris_arg_t __arg)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __event) {
#if (ERIS_HAVE_SYS_EPOLL_H)
        if ( 0 <= __event->context.epoll.fd) {
            do {
                eris_int_t ready_n = 0;

                /** Dispatcher over */
                if ( 0 < __event->over) {
                    break;
                }

                if ( __event->log) {
                    eris_log_dump( __event->log, ERIS_LOG_DEBUG, "Has event nodes count.%d", __event->nodes_count);
                }

                ready_n = epoll_wait( __event->context.epoll.fd, 
                                      __event->context.epoll.fds,
                                      __event->context.epoll.size,
                                      __event->timeout * 1000 );
                if ( 0 < ready_n) {
                    eris_int_t i = 0;

                    /** Ok, has ready_n socket objects */
                    for ( i = 0; i < ready_n; i++) {
                        eris_bool_t is_accept_sock = false;

                        eris_event_elem_t cur_elem; {
                            cur_elem.sock   = __event->context.epoll.fds[ i].data.fd;
                            cur_elem.events = ERIS_EVENT_NONE;
                        }

                        /** Check is accept socket object? */
                        {
                            eris_event_node_t *cur_node = __event->accept_nodes;

                            for ( ; (NULL != cur_node); cur_node = cur_node->next) {
                                if ( cur_elem.sock == cur_node->elem.sock) {
                                    is_accept_sock = true;

                                    break;
                                }
                            }
                        }

                        if ( is_accept_sock) {
                            eris_sock_t client_sock = eris_event_accept( __event, cur_elem.sock);
                            if ( -1 != client_sock) {
                                eris_event_elem_t ev_client_elem; {
                                    ev_client_elem.sock   = client_sock;
                                    ev_client_elem.events = ERIS_EVENT_READ;
                                }

                                if ( __event->log) {
                                    eris_socket_host_t client_host;

                                    if ( 0 == eris_socket_host( client_sock, &client_host)) {
                                        eris_log_dump( __event->log, ERIS_LOG_INFO, 
                                                       "Connect from client.(%s:%d) fd.%d", 
                                                       client_host.ipv4, 
                                                       client_host.port,
                                                       client_sock);
                                    }
                                }

                                /** Busy??? */
                                if ( (__event->nodes_count + 1) < __event->max_events) {
                                    rc = eris_event_add( __event, &ev_client_elem);
                                    if ( 0 != rc) {
                                        eris_socket_close( client_sock);
                                    }
                                } else {
                                    ev_client_elem.events = ERIS_EVENT_BUSY;

                                    /** Call back */
                                    if ( __event_cb) {
                                        __event_cb( &ev_client_elem, __arg); 

                                    } else { 
                                        eris_socket_close( client_sock); 
                                    }
                                }
                            }
                        } else {
                            /** Has readable */
                            if ( EPOLLIN & __event->context.epoll.fds[ i].events) {
                                cur_elem.events |= ERIS_EVENT_READ;
                            }

                            /** Has writeable */
                            if ( EPOLLOUT & __event->context.epoll.fds[ i].events) {
                                cur_elem.events |= ERIS_EVENT_WRITE;
                            }

                            /** Has urgent data */
                            if ( EPOLLPRI & __event->context.epoll.fds[ i].events) {
                                cur_elem.events = ERIS_EVENT_OOB;
                            }

                            /** Happen error */
                            if ( EPOLLERR & __event->context.epoll.fds[ i].events) {
                                cur_elem.events = ERIS_EVENT_ERROR;
                            }

                            /** Close of peer */
                            if ( ( EPOLLHUP   & __event->context.epoll.fds[ i].events) || 
                                 ( EPOLLRDHUP & __event->context.epoll.fds[ i].events) ) {
                                cur_elem.events |= ERIS_EVENT_CLOSE;
                            }

                            if ( ERIS_EVENT_NONE != cur_elem.events) {
                                eris_event_elem_t del_elem; {
                                    del_elem.sock   = cur_elem.sock;
                                    del_elem.events = ERIS_EVENT_READ | ERIS_EVENT_WRITE;
                                }

                                eris_event_delete( __event, &del_elem);

                                /** Call back */
                                if ( __event_cb) { __event_cb( &cur_elem, __arg); }
                            }
                        }
                    }/// for ( i = 0; i < read_n; i++)

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
                eris_log_dump( __event->log, ERIS_LOG_CORE, "Epoll fd is -1 invalid");
            }
        }

        /** Record dispatch will over */
        {
            pthread_mutex_lock( &(__event->mutex));

            __event->over++;

            pthread_mutex_unlock( &(__event->mutex));
        }
#endif /** ERIS_HAVE_SYS_EPOLL_H */

    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_epoll_dispatch


/**
 * @Brief: Terminate diapatcher.
 *
 * @Param: __event, Event context handler.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_epoll_over( eris_event_t *__event)
{
    /** Nothing */
    return;
}/// eris_epoll_over


/**
 * @Brief: Destroy context handler.
 *
 * @Param: __event, Event context handler.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_epoll_destroy( eris_event_t *__event)
{
    if ( __event) {
#if (ERIS_HAVE_SYS_EPOLL_H)

        if ( 0 <= __event->context.epoll.fd) {
#define eris_epoll_close_fd(fd) close(fd)
            eris_epoll_close_fd( __event->context.epoll.fd);
#undef eris_epoll_close_fd

            __event->context.epoll.fd = -1;
        }

        /** Dispatch is over, and then destroy context */
        eris_memory_free( __event->context.poll.fds);

        __event->context.poll.fds = NULL;

#endif /** ERIS_HAVE_SYS_EPOLL_H*/

    }
}/// eris_epoll_destory



