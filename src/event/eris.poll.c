/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This design event - poll, and support api:
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

#include "event/eris.poll.h"
#include "event/eris.event.h"

#include "core/eris.core.h"


/** Register all fds. */
static eris_none_t eris_poll_regfds( eris_event_t *__event);



/**
 * @Brief: Init poll context handler.
 *
 * @Param: __event, Event context handler.
 * 
 * @Return: Ok is 0, Other is -1.
 **/
eris_int_t eris_poll_init( eris_event_t *__event)
{
    eris_int_t rc = 0;

    if ( __event) {
#if (ERIS_HAVE_POLL_H)
        __event->context.poll.size = __event->max_events;

        __event->context.poll.current = 0;

        __event->context.poll.fds = (eris_pollfd_t *)eris_memory_alloc( __event->context.poll.size * sizeof( eris_pollfd_t));
        if ( __event->context.poll.fds) {
            eris_int_t i = 0;

            for ( i = 0; i < __event->context.poll.size; i++) {
                __event->context.poll.fds[ i].fd      = -1;
                __event->context.poll.fds[ i].events  = 0;
                __event->context.poll.fds[ i].revents = 0;
            }
        } else { 
            /** Alloc pollfd buffer failed */
            rc = EERIS_ERROR; 

            if ( __event->log) {
                eris_log_dump( __event->log, ERIS_LOG_CORE, "Alloced memory for pollfds cache failed.");
            }
        }
#endif /** ERIS_HAVE_POLL_H */

    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_poll_init


/**
 * @Brief: Add socket element in poll context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is -1.
 **/
eris_int_t eris_poll_add( eris_event_t *__event, eris_event_elem_t *__elem)
{
    /** Nothing */
    return 0;
}/// eris_poll_add


/**
 * @Brief: Modify socket element in poll context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is -1.
 **/
eris_int_t eris_poll_modify( eris_event_t *__event, eris_event_elem_t *__elem)
{
    /** Nothing */
    return 0;
}/// eris_poll_modify


/**
 * @Brief: Delete socket element in poll context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is -1.
 **/
eris_int_t eris_poll_delete( eris_event_t *__event, eris_event_elem_t *__elem)
{
    /** Nothing */
    return 0;
}/// eris_poll_delete


/**
 * @Brief: Poll dispatcher.
 *
 * @Param: __event,    Event context handler.
 * @Param: __event_cb, Happen event and callback.
 * @Param: __arg,      Input callback argument.
 *
 * @Return: Ok is 0, other is -1.
 **/
eris_int_t eris_poll_dispatch( eris_event_t *__event, eris_event_cb_t __event_cb, eris_arg_t __arg)
{
    eris_int_t rc = 0;

    if ( __event) {
#if (ERIS_HAVE_POLL_H)

        eris_int_t ready_n = 0;

        do {
            /** Has over flag set and jump out */
            if ( 0 < __event->over) { break; }

            /** Register fds */
            eris_poll_regfds( __event);

            if ( __event->log) {
                eris_log_dump( __event->log, ERIS_LOG_DEBUG, "Has event nodes count.%d", __event->nodes_count);
            }

            /** Do monitor of poll I/O */
            ready_n = poll( __event->context.poll.fds, __event->context.poll.current, __event->timeout * 1000);
            if ( 0 < ready_n) {
                eris_int_t i = 0;
                eris_int_t doing_count = 0;

                /** Has ready_n number ready */
                for ( i = 0; i < __event->context.poll.current; i++) {
                    if ( doing_count >= ready_n) {
                        break;
                    }

                    eris_bool_t is_accept_sock = false;

                    eris_event_elem_t cur_elem; {
                        cur_elem.sock   = __event->context.poll.fds[ i].fd;
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
                        if ( (POLLIN     & __event->context.poll.fds[ i].revents) ||
                             (POLLRDNORM & __event->context.poll.fds[ i].revents) ){
                            doing_count++;

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
                        }
                    } else {
                        //eris_event_delete( __event, &cur_elem);

                        /** Other client socket fd */
                        if ( (POLLIN     & __event->context.poll.fds[ i].revents) ||
                             (POLLRDNORM & __event->context.poll.fds[ i].revents) ){
                            cur_elem.events |= ERIS_EVENT_READ;
                        }

                        if ( (POLLOUT    & __event->context.poll.fds[ i].revents) ||
                             (POLLWRNORM & __event->context.poll.fds[ i].revents) ) {
                            cur_elem.events |= ERIS_EVENT_WRITE;
                        }

                        if ( POLLPRI & __event->context.poll.fds[ i].revents) {
                            cur_elem.events |= ERIS_EVENT_OOB;
                        }

                        if ( (POLLERR  & __event->context.poll.fds[ i].revents) &&
                             (POLLNVAL & __event->context.poll.fds[ i].revents) ) {
                            cur_elem.events = ERIS_EVENT_ERROR;
                        }

                        if ( POLLHUP & __event->context.poll.fds[ i].revents) {
                            cur_elem.events |= ERIS_EVENT_CLOSE;
                        }

                        if ( ERIS_EVENT_NONE != cur_elem.events) {
                            eris_event_elem_t del_elem; {
                                del_elem.sock   = cur_elem.sock;
                                del_elem.events = ERIS_EVENT_READ | ERIS_EVENT_WRITE;
                            }

                            eris_event_delete( __event, &del_elem);

                            doing_count++;

                            /** Doing callback */
                            if ( __event_cb) { 
                                __event_cb( &cur_elem, __arg); 
                            }
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
            } /** End: do monitor poll I/O */

            /** Check has timeout socket object? */
            eris_event_filing( __event, __event_cb, __arg);

        } while (1);

        /** Record dispatch will over */
        {
            pthread_mutex_lock( &(__event->mutex));

            __event->over++;

            pthread_mutex_unlock( &(__event->mutex));
        }
#endif /** ERIS_HAVE_POLL_H */

    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_poll_dispatch


/**
 * @Brief: Terminate diapatcher.
 *
 * @Param: __event, Event context handler.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_poll_over( eris_event_t *__event)
{
    /** Nothng */
    return;
}/// eris_poll_over


/**
 * @Brief: Destroy context handler.
 *
 * @Param: __event, Event context handler.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_poll_destroy( eris_event_t *__event)
{
    if ( __event) {
#if (ERIS_HAVE_POLL_H)
        /** Dispatch is over, and then destroy context */
        if ( __event->context.poll.fds) {
            eris_memory_free( __event->context.poll.fds);

            __event->context.poll.fds = NULL;
        }

        __event->context.poll.current = 0;
#endif /** ERIS_HAVE_POLL_H */

    }
}/// eris_poll_destroy


/**
 * @Brief: Register all fds.
 *
 * @Param: __event, Event context handler.
 *
 * @Return: Nothing.
 **/
static eris_none_t eris_poll_regfds( eris_event_t *__event)
{
#if (ERIS_HAVE_POLL_H)
    eris_int_t reg_count = 0;
    {
        eris_event_node_t *cur_node = __event->accept_nodes;

        while ( cur_node) {
            if ( 0 <= cur_node->elem.sock) {
                __event->context.poll.fds[ reg_count].fd = cur_node->elem.sock;
                __event->context.poll.fds[ reg_count].events  = POLLIN | POLLRDNORM;
                __event->context.poll.fds[ reg_count].revents = 0;

                reg_count++;

                __event->context.poll.current = reg_count;
            }

            cur_node = cur_node->next;
        }
    }

    pthread_cleanup_push( (eris_cleanup_routine_t)pthread_mutex_unlock, (eris_void_t *)&(__event->mutex));
   /** Heppen cancel? */

    /** Lock */
    pthread_mutex_lock( &(__event->mutex));

    /** Add to poll and monitor */
    eris_int_t i = 0;

    for ( i = 0; i < __event->nodes_size; i++) {
        eris_event_node_t *cur_node = __event->nodes[ i];

        while ( cur_node) {
            if ( reg_count >= __event->context.poll.size) {
                break;
            }

            /** valid socket fd context */
            if ( 0 < cur_node->elem.sock) {
                /** Is alive??? */
                if ( 1 == eris_socket_ready_w( cur_node->elem.sock, 0)) {
                    __event->context.poll.fds[ reg_count].fd = cur_node->elem.sock;
                    __event->context.poll.fds[ reg_count].events  = 0;
                    __event->context.poll.fds[ reg_count].revents = 0;

                    /** Register read event */
                    if ( (ERIS_EVENT_READ  & cur_node->elem.events) ||
                         (ERIS_EVENT_WRITE & cur_node->elem.events) ) {

                        if ( ERIS_EVENT_READ & cur_node->elem.events) {
                            __event->context.poll.fds[ reg_count].events |= (POLLIN | POLLRDNORM);
                        }

                        /** Register write event */
                        if ( ERIS_EVENT_WRITE & cur_node->elem.events) {
                            __event->context.poll.fds[ reg_count].events |= (POLLOUT | POLLRDNORM);
                        }

                        reg_count++;
                    }
                }else {
                    cur_node->start = 0;
                }
            }

            cur_node = cur_node->next;
        }

        __event->context.poll.current = reg_count;

        if ( reg_count >= __event->context.poll.size) {
            break;
        }
    }

    /** Unlock */
    pthread_mutex_unlock( &(__event->mutex));

    pthread_cleanup_pop(0);
#endif /** ERIS_HAVE_POLL_H */

}/// eris_poll_regfds



