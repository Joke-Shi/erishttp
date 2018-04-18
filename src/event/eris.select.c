/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This design event - select I/O, and support api:
 **        :     create
 **        :     add
 **        :     modify 
 **        :     delete
 **        :     dispatch 
 **        :     over 
 **        :     destroy 
 **
 ******************************************************************************/

#include "event/eris.select.h"
#include "event/eris.event.h"

#include "core/eris.core.h"


/** regfds of select. */
static eris_none_t eris_select_regfds( eris_event_t *__event);
    


/**
 * @Brief: Init select context handler.
 *
 * @Param: __event, Event context handler.
 * 
 * @Return: Ok is 0, Other is -1.
 **/
eris_int_t eris_select_init( eris_event_t *__event)
{
    eris_int_t rc = 0;

    if ( __event) {
        __event->context.select.event_nodes = NULL;
        __event->context.select.max_sock    = -1;

        FD_ZERO( &(__event->context.select.readable));
        FD_ZERO( &(__event->context.select.writeable));

    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_select_init


/**
 * @Brief: Add socket element in select context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is -1.
 **/
eris_int_t eris_select_add( eris_event_t *__event, eris_event_elem_t *__elem)
{
    return 0;
}/// eris_select_add


/**
 * @Brief: Modify socket element in select context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is -1.
 **/
eris_int_t eris_select_modify( eris_event_t *__event, eris_event_elem_t *__elem)
{
    return 0;
}/// eris_select_modify


/**
 * @Brief: Delete socket element in select context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is -1.
 **/
eris_int_t eris_select_delete( eris_event_t *__event, eris_event_elem_t *__elem)
{
    return 0;
}/// eris_select_delete


/**
 * @Brief: select dispatcher.
 *
 * @Param: __event,    Event context handler.
 * @Param: __event_cb, Happen event and callback.
 * @Param: __arg,      Input callback argument.
 *
 * @Return: Ok is 0, other is -1.
 **/
eris_int_t eris_select_dispatch( eris_event_t *__event, eris_event_cb_t __event_cb, eris_arg_t __arg)
{
    eris_int_t rc = 0;

    if ( __event) {
        do {
            eris_select_regfds( __event);

            eris_int_t ready_n = 0;

            /** Has over flag set and jump out */
            if ( 0 < __event->over) { 
                break; 
            }

            if ( __event->log) {
                eris_log_dump( __event->log, ERIS_LOG_DEBUG, "Has event nodes count.%d", __event->nodes_count);
            }

            fd_set *p_r_set = &(__event->context.select.readable);
            fd_set *p_w_set = &(__event->context.select.writeable);
            fd_set  errorable_set; { FD_ZERO( &errorable_set); }

            eris_timeval_t tv_timeout; {
                tv_timeout.tv_sec  = __event->timeout;
                tv_timeout.tv_usec = 0;
            }

            /** Do monitor of select I/O */
            ready_n = select( __event->context.select.max_sock + 1, 
                              p_r_set,
                              p_w_set, 
                              &errorable_set, 
                              &tv_timeout );
            if ( 0 < ready_n ) {
                eris_int_t do_count = 0;

                eris_event_node_t *cur_node = __event->accept_nodes;
                while ( cur_node) {
                    if ( FD_ISSET( cur_node->elem.sock, p_r_set)) {
                        eris_sock_t client_sock = eris_event_accept( __event, cur_node->elem.sock);
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

                        do_count++;
                    }

                    if ( do_count >= ready_n) {
                        break;
                    }

                    cur_node = cur_node->next;
                }

                if ( do_count < ready_n) {
                    eris_event_node_t *cur_node = __event->context.select.event_nodes;

                    while ( cur_node) {
                        eris_event_elem_t cur_elem; {
                            cur_elem.sock   = cur_node->elem.sock;
                            cur_elem.events = ERIS_EVENT_NONE;
                        }

                        /** Read event happen */
                        if ( FD_ISSET( cur_node->elem.sock, p_r_set)) {
                            /** Connect of client socket fd */
                            cur_elem.events |= ERIS_EVENT_READ;
                        }

                        /** Write event happen */
                        if ( FD_ISSET( cur_node->elem.sock, p_w_set)) {
                            /** Connect of client socket fd */
                            cur_elem.events |= ERIS_EVENT_WRITE;
                        }

                        /** Error event happen */
                        if ( FD_ISSET( cur_node->elem.sock, &errorable_set)) {
                            /** Connect of client socket fd */
                            cur_elem.events = ERIS_EVENT_ERROR;
                        }

                        if ( ERIS_EVENT_NONE != cur_elem.events) {
                            //eris_event_delete( __event, &cur_elem);

                            do_count++;

                            /** Call back */
                            if ( __event_cb) { __event_cb( &cur_elem, __arg); }
                        }

                        if ( do_count >= ready_n) { break; }

                        cur_node = cur_node->next;
                    }/// whil ( cur_node)
                }
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

            /** Check has timeout socket object? */
            eris_event_filing( __event, __event_cb, __arg);

        } while( 1);

        /** Record dispatch will over */
        {
            pthread_mutex_lock( &(__event->mutex));

            __event->over++;

            pthread_mutex_unlock( &(__event->mutex));
        }
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_select_dispatch


/**
 * @Brief: Terminate diapatcher.
 *
 * @Param: __event, Event context handler.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_select_over( eris_event_t *__event)
{
    /** Nothing */
    return;
}/// eris_select_over


/**
 * @Brief: Destroy context handler.
 *
 * @Param: __event, Event context handler.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_select_destroy( eris_event_t *__event)
{
    if ( __event) {
        __event->context.select.max_sock = -1;

        eris_event_node_t *del_elem = __event->context.select.event_nodes;
        while ( del_elem) {
            __event->context.select.event_nodes = del_elem->next;

            eris_socket_close( del_elem->elem.sock);

            eris_memory_free( del_elem);

            del_elem = __event->context.select.event_nodes;
        }

        FD_ZERO( &(__event->context.select.readable));
        FD_ZERO( &(__event->context.select.writeable));
    }
}/// eris_select_destroy


/**
 * @Brief: regfds of select.
 *
 * @Param: __event, Event context object.
 *
 * @Return: Nothing.
 **/
static eris_none_t eris_select_regfds( eris_event_t *__event)
{
    if ( __event) {
        eris_event_node_t *del_elem = __event->context.select.event_nodes;
        while ( del_elem) {
            __event->context.select.event_nodes = del_elem->next;

            eris_memory_free( del_elem);

            del_elem = __event->context.select.event_nodes;
        }


        __event->context.select.max_sock = -1;

        FD_ZERO( &(__event->context.select.readable));
        FD_ZERO( &(__event->context.select.writeable));

        eris_time_t now_time = 0;
        eris_time_get( now_time);

        /** Register accept socket object */
        {
            eris_event_node_t *cur_node = __event->accept_nodes;
            while ( cur_node) {
                FD_SET( cur_node->elem.sock, &(__event->context.select.readable));

                if ( __event->context.select.max_sock < cur_node->elem.sock) {
                    __event->context.select.max_sock = cur_node->elem.sock;
                }

                cur_node = cur_node->next;
            }
        }

        eris_int_t i     = 0;
        eris_int_t count = 0;

        pthread_cleanup_push( (eris_cleanup_routine_t)pthread_mutex_unlock, (eris_void_t *)&(__event->mutex));
        /** Heppen cancel? */

        /** Safe-thread: lock */
        pthread_mutex_lock( &(__event->mutex));

        /** Clean up timeout socket fd */
        for ( i = 0; i < __event->nodes_size; i++) {
            if ( count > __event->nodes_count) {
                break;
            }

            eris_event_node_t *cur_node  = __event->nodes[ i];
            eris_event_node_t *next_node = NULL;

            while( cur_node) {
                next_node = cur_node->next;

                if ( (0 < cur_node->elem.sock) ||
                     (ERIS_EVENT_READ  & cur_node->elem.events) ||
                     (ERIS_EVENT_WRITE & cur_node->elem.events) ) {

                    eris_event_node_t *ev_new_elem = (eris_event_node_t *)eris_memory_alloc( sizeof( eris_event_node_t));
                    if ( ev_new_elem) {
                        ev_new_elem->elem.sock   = cur_node->elem.sock;
                        ev_new_elem->elem.events = cur_node->elem.events;
                        ev_new_elem->start       = now_time;

                        ev_new_elem->next = __event->context.select.event_nodes;
                        if ( __event->context.select.event_nodes) {
                            __event->context.select.event_nodes->prev = ev_new_elem;
                        }

                        __event->context.select.event_nodes = ev_new_elem;

                        /** Register client socket objects */
                        if ( __event->context.select.max_sock < cur_node->elem.sock) {
                            __event->context.select.max_sock = cur_node->elem.sock;
                        }

                        if (ERIS_EVENT_READ & cur_node->elem.events ) {
                            FD_SET( cur_node->elem.sock, &(__event->context.select.readable));
                        }

                        if ( ERIS_EVENT_WRITE & cur_node->elem.events) {
                            FD_SET( cur_node->elem.sock, &(__event->context.select.writeable));
                        }

                        count++;
                    }
                }

                cur_node = next_node;
            }
        }

        /** Unlock */
        pthread_mutex_unlock( &(__event->mutex));

        pthread_cleanup_pop(0);
    }
}/// eris_select_regfds



