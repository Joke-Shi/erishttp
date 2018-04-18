/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This design event: select, poll, epoll, kqueue, devpoll and so on.
 **
 ******************************************************************************/

#include "event/eris.event.h"

#include "core/eris.core.h"


/**
 * @Brief: Init event context of event attributes.
 *
 * @Param: __event, Event context.
 * @Param: __attrs, Specify attributes.
 *
 * @Return: Ok is 0, Other is -1.
 **/
eris_int_t eris_event_init( eris_event_t *__event, eris_event_attr_t *__attrs)
{
    eris_int_t rc = 0;

    if ( __event ) {
        eris_event_attr_t tmp_attrs; {
            tmp_attrs.iot        = ERIS_EVENT_IO_SELECT;
            tmp_attrs.timeout    = 20;
            tmp_attrs.keepalive  = 60;
            tmp_attrs.max_events = 1024;
            tmp_attrs.send_buffer_size = 8192;
            tmp_attrs.recv_buffer_size = 8192;
            tmp_attrs.tcp_nodelay      = 0;
            tmp_attrs.tcp_nopush       = 0;
            tmp_attrs.zero             = 0;
            tmp_attrs.log              = NULL;
        }

        eris_event_attr_t *pattrs = &tmp_attrs;
        if ( __attrs) {
            pattrs = __attrs;
        }

        rc = pthread_mutex_init( &(__event->mutex), NULL);
        if ( 0 == rc) {
            __event->iot              = pattrs->iot;
            __event->timeout          = pattrs->timeout;
            __event->keepalive        = pattrs->keepalive;
            __event->max_events       = pattrs->max_events;
            __event->send_buffer_size = pattrs->send_buffer_size;
            __event->recv_buffer_size = pattrs->send_buffer_size;
            __event->tcp_nodelay      = pattrs->tcp_nodelay;
            __event->tcp_nopush       = pattrs->tcp_nopush;
            __event->zero             = pattrs->zero;
            __event->pid              = eris_get_pid();
            __event->nprocs           = eris_get_nprocessor();
            __event->log              = pattrs->log;
            __event->accept_nodes     = NULL;

            __event->nodes_size       = __event->max_events / 16;
            if ( 64 >= __event->nodes_size) {
                __event->nodes_size = 64;
            }
            __event->nodes_count = 0;

            __event->nodes = (eris_event_node_t **)eris_memory_alloc( sizeof( eris_event_node_t *) * __event->nodes_size);
            if ( __event->nodes) {
                eris_int_t i = 0; 

                for ( i = 0; i < __event->nodes_size; i++) {
                    __event->nodes[ i] = NULL;
                }

                switch ( __event->iot) {
#if (ERIS_HAVE_POLL_H)
                    case ERIS_EVENT_IO_POLL :
                        {
                            /** Poll callback */
                            __event->init_handler     = eris_poll_init;
                            __event->add_handler      = eris_poll_add;
                            __event->modify_handler   = eris_poll_modify;
                            __event->delete_handler   = eris_poll_delete;
                            __event->dispatch_handler = eris_poll_dispatch;
                            __event->over_handler     = eris_poll_over;
                            __event->destroy_handler  = eris_poll_destroy;

                        } break;
#endif

#if (ERIS_HAVE_SYS_EPOLL_H)
                    case ERIS_EVENT_IO_EPOLL :
                        {
                            /** Epoll callback */
                            __event->init_handler     = eris_epoll_init;
                            __event->add_handler      = eris_epoll_add;
                            __event->modify_handler   = eris_epoll_modify;
                            __event->delete_handler   = eris_epoll_delete;
                            __event->dispatch_handler = eris_epoll_dispatch;
                            __event->over_handler     = eris_epoll_over;
                            __event->destroy_handler  = eris_epoll_destroy;
                        } break;
#endif

#if (ERIS_HAVE_SYS_EVENT_H)
                    case ERIS_EVENT_IO_KQUEUE :
                        {
                            /** Kqueue callback */
                            __event->init_handler     = eris_kqueue_init;
                            __event->add_handler      = eris_kqueue_add;
                            __event->modify_handler   = eris_kqueue_modify;
                            __event->delete_handler   = eris_kqueue_delete;
                            __event->dispatch_handler = eris_kqueue_dispatch;
                            __event->over_handler     = eris_kqueue_over;
                            __event->destroy_handler  = eris_kqueue_destroy;
                        } break;
#endif

#if (ERIS_HAVE_SYS_DEVPOLL_H)
                    case ERIS_EVENT_IO_DEVPOLL :
                        {
                            /** devpoll callback */
                            __event->init_handler     = eris_devpoll_init;
                            __event->add_handler      = eris_devpoll_add;
                            __event->modify_handler   = eris_devpoll_modify;
                            __event->delete_handler   = eris_devpoll_delete;
                            __event->dispatch_handler = eris_devpoll_dispatch;
                            __event->over_handler     = eris_devpoll_over;
                            __event->destroy_handler  = eris_devpoll_destroy;
                        } break;
#endif
                    case ERIS_EVENT_IO_SELECT :
                    default: 
                        {
                            /** Default select-I/O */
                            __event->init_handler     = eris_select_init;
                            __event->add_handler      = eris_select_add;
                            __event->modify_handler   = eris_select_modify;
                            __event->delete_handler   = eris_select_delete;
                            __event->dispatch_handler = eris_select_dispatch;
                            __event->over_handler     = eris_select_over;
                            __event->destroy_handler  = eris_select_destroy;
                        } break; 
                }/// switch ( __event->iot)

                /** Do-init */
                rc = __event->init_handler( __event);
            } else { rc = EERIS_ERROR; }
        } else { rc = EERIS_ERROR; }
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_event_init


/**
 * @Brief: Add socket element in event context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is -1.
 **/
eris_int_t eris_event_add( eris_event_t *__event, eris_event_elem_t *__elem)
{
    eris_int_t rc = 0;

    if ( __event) {
        pthread_cleanup_push( (eris_cleanup_routine_t)pthread_mutex_unlock, (eris_void_t *)&(__event->mutex));
        /** Heppen cancel? */

        /** Must safe-thread: lock */
        pthread_mutex_lock( &(__event->mutex));

        /** Has over dispatch */
        if ( 0 == __event->over) {
            if ( __elem && (0 < __elem->sock)) {
                if ( ERIS_EVENT_ACCEPT & __elem->events) {
                    eris_event_node_t *cur_node = __event->accept_nodes;

                    while ( cur_node) {
                        if ( cur_node->elem.sock == __elem->sock) {
                            break;
                        }

                        cur_node = cur_node->next;
                    }

                    if ( !cur_node) {
                        cur_node = (eris_event_node_t *)eris_memory_alloc( sizeof( eris_event_node_t));
                        if ( cur_node) {
                            cur_node->elem.sock   = __elem->sock;
                            cur_node->elem.events = __elem->events;
                            eris_time_get( cur_node->start);

                            cur_node->next = __event->accept_nodes;
                            cur_node->prev = NULL;

                            if ( __event->accept_nodes) {
                                __event->accept_nodes->prev = cur_node;
                            }

                            __event->accept_nodes = cur_node;
                        } else {
                            rc = EERIS_ERROR;

                            if ( __event->log) {
                                eris_log_dump( __event->log, ERIS_LOG_CORE, "Create event node failed, errno.<%d>", errno);
                            }
                        }
                    }
                } else {
                    /** Calc table index with save */
                    eris_int_t x = (eris_int_t )(__elem->sock) % __event->nodes_size;

                    eris_event_node_t *cur_node = __event->nodes[ x];
                    if ( cur_node) {
                        /** It is existed? */
                        do {
                            if ( cur_node->elem.sock == __elem->sock) {
                                break;
                            }

                            /** Not found and goto next node */
                            cur_node = cur_node->next;
                        } while ( cur_node);

                        if ( cur_node) {
                            /** Has existed */
                            cur_node->elem.events = __elem->events;

                            eris_time_get( cur_node->start);
                        } else {
                            /** Not found and new node */
                            cur_node = (eris_event_node_t *)eris_memory_alloc( sizeof( eris_event_node_t));
                            if ( cur_node) {
                                cur_node->next = __event->nodes[ x];
                                cur_node->prev = NULL;
                                cur_node->elem.sock   = __elem->sock;
                                cur_node->elem.events = __elem->events;

                                eris_time_get( cur_node->start);

                                /** Add to table link */
                                if ( __event->nodes[ x]) {
                                    __event->nodes[ x]->prev = cur_node;
                                }
                                __event->nodes[ x] = cur_node;

                                __event->nodes_count++;
                            } else {
                                rc = EERIS_ERROR;

                                if ( __event->log) {
                                    eris_log_dump( __event->log, ERIS_LOG_CORE, "Alloced memory for new event node failed.");
                                }
                            }
                        }/// else cur_node is NULL
                    } else {
                        __event->nodes[ x] = (eris_event_node_t *)eris_memory_alloc( sizeof( eris_event_node_t));
                        if ( __event->nodes[ x]) {
                            __event->nodes[ x]->elem.sock   = __elem->sock;
                            __event->nodes[ x]->elem.events = __elem->events;
                            __event->nodes[ x]->next = NULL;
                            __event->nodes[ x]->prev = NULL;

                            eris_time_get( __event->nodes[ x]->start);

                            __event->nodes_count++;
                        } else {
                            rc = EERIS_ERROR;

                            if ( __event->log) {
                                eris_log_dump( __event->log, ERIS_LOG_CORE, "Alloced memory for new event node failed.");
                            }
                        }
                    }/// else cur_node pointer index table is NULL
                }/// else if not monitor accept event

            } else {
                rc = EERIS_ERROR;

                if ( __event->log) {
                    eris_log_dump( __event->log, ERIS_LOG_CORE, "Unspecify element parameters.");
                }
            }

            /** Successed -> call */
            if ( 0 == rc) { __event->add_handler( __event, __elem); }
        } else { 
            rc = EERIS_ERROR;

            if ( __event->log) {
                eris_log_dump( __event->log, ERIS_LOG_CORE, "Dispatch has over.");
            }
        }

        /** Unlock */
        pthread_mutex_unlock( &(__event->mutex));

        pthread_cleanup_pop( 0);

    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_event_add


/**
 * @Brief: Modify socket element in event context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is -1.
 **/
eris_int_t eris_event_modify( eris_event_t *__event, eris_event_elem_t *__elem)
{
    eris_int_t rc = 0;

    if ( __event) {
        pthread_cleanup_push( (eris_cleanup_routine_t)pthread_mutex_unlock, (eris_void_t *)&(__event->mutex));
        /** Heppen cancel? */

        /** Must safe-thread: lock */
        pthread_mutex_lock( &(__event->mutex));

        /** Has over dispatch */
        if ( 0 == __event->over) {
            if ( __elem && (0 <= __elem->sock)) {
                /** Calc table index with save */
                eris_int_t x = (eris_int_t )(__elem->sock) % __event->nodes_size;

                eris_event_node_t *cur_node = __event->nodes[ x];
                if ( cur_node) {
                    /** It is existed? */
                    do {
                        if ( cur_node->elem.sock == __elem->sock) {
                            break;
                        }

                        /** Not found and goto next node */
                        cur_node = cur_node->next;
                    } while ( cur_node);

                    if ( cur_node) {
                        /** Has existed */
                        cur_node->elem.events = __elem->events;

                        eris_time_get( cur_node->start);
                    } else {
                        /** Not found and new node */
                        cur_node = (eris_event_node_t *)eris_memory_alloc( sizeof( eris_event_node_t));
                        if ( cur_node) {
                            cur_node->next = __event->nodes[ x];
                            cur_node->prev = NULL;
                            cur_node->elem.sock   = __elem->sock;
                            cur_node->elem.events = __elem->events;

                            eris_time_get( cur_node->start);

                            /** Add to table link */
                            if ( __event->nodes[ x]) {
                                __event->nodes[ x]->prev = cur_node;
                            }
                            __event->nodes[ x] = cur_node;

                            __event->nodes_count++;
                        } else {
                            rc = EERIS_ERROR;

                            if ( __event->log) {
                                eris_log_dump( __event->log, ERIS_LOG_CORE, "Alloced memory for new event node failed.");
                            }
                        }
                    }/// else cur_node is NULL
                } else {
                    __event->nodes[ x] = (eris_event_node_t *)eris_memory_alloc( sizeof( eris_event_node_t));
                    if ( __event->nodes[ x]) {
                        __event->nodes[ x]->elem.sock   = __elem->sock;
                        __event->nodes[ x]->elem.events = __elem->events;
                        __event->nodes[ x]->next = NULL;
                        __event->nodes[ x]->prev = NULL;

                        eris_time_get( __event->nodes[ x]->start);

                        __event->nodes_count++;
                    } else {
                        rc = EERIS_ERROR;

                        if ( __event->log) {
                            eris_log_dump( __event->log, ERIS_LOG_CORE, "Alloced memory for new event node failed.");
                        }
                    }
                }/// else cur_node pointer index table is NULL

            } else {
                rc = EERIS_ERROR;

                if ( __event->log) {
                    eris_log_dump( __event->log, ERIS_LOG_CORE, "Unspecify element parameters.");
                }
            }

            /** Successed -> call */
            if ( 0 == rc) { __event->modify_handler( __event, __elem); }
        } else { 
            rc = EERIS_ERROR;

            if ( __event->log) {
                eris_log_dump( __event->log, ERIS_LOG_CORE, "Dispatch has over.");
            }
        }

        /** Unlock */
        pthread_mutex_unlock( &(__event->mutex));

        pthread_cleanup_pop(0);
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_event_modify


/**
 * @Brief: Delete socket element in event context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is -1.
 **/
eris_int_t eris_event_delete( eris_event_t *__event, eris_event_elem_t *__elem)
{
    eris_int_t rc = 0;

    if ( __event) {
        pthread_cleanup_push( (eris_cleanup_routine_t)pthread_mutex_unlock, (eris_void_t *)&(__event->mutex));
        /** Heppen cancel? */

        /** Must safe-thread: lock */
        pthread_mutex_lock( &(__event->mutex));

        /** Has over dispatch */
        if ( 0 == __event->over) {
            if ( __elem && (0 <= __elem->sock)) {
                /** Calc table index with save */
                eris_int_t x = (eris_int_t )(__elem->sock) % __event->nodes_size;

                eris_event_node_t *cur_node = __event->nodes[ x];
                if ( cur_node) {
                    /** It is existed? */
                    do {
                        if ( cur_node->elem.sock == __elem->sock) {

                            break;
                        }

                        /** Not found and goto next node */
                        cur_node = cur_node->next;
                    } while ( cur_node);

                    if ( cur_node) {
                        if ( cur_node == __event->nodes[ x]) {
                            __event->nodes[ x] = cur_node->next;
                        }

                        cur_node->prev ? (cur_node->prev->next = cur_node->next): 0;
                        cur_node->next ? (cur_node->next->prev = cur_node->prev): 0;
                        cur_node->prev = NULL; 
                        cur_node->next = NULL;

                        eris_memory_free( cur_node);

                        __event->nodes_count--;
                    }
                }

                /** Successed -> call */
                __event->delete_handler( __event, __elem);
            } else {
                rc = EERIS_ERROR;

                if ( __event->log) {
                    eris_log_dump( __event->log, ERIS_LOG_CORE, "Unspecify element parameters.");
                }
            }
        } else { 
            rc = EERIS_ERROR;

            if ( __event->log) {
                eris_log_dump( __event->log, ERIS_LOG_CORE, "Dispatch has over.");
            }
        }

        /** Unlock */
        pthread_mutex_unlock( &(__event->mutex));

        pthread_cleanup_pop(0);
    } else { rc = EERIS_ERROR; }


    return rc;
}/// eris_event_delete


/**
 * @Brief: Monitor all sockets and dispatch.
 *
 * @Param: __event, Event context.
 * @Param: __cb,    Happen event and doing.
 * @Param: __arg,   Global callback argument.
 *
 * @Return: Os is 0, Other is -1.
 **/
eris_int_t eris_event_dispatch( eris_event_t *__event, eris_event_cb_t __cb, eris_arg_t __arg)
{
    eris_int_t rc = 0;

    if ( __event) {
        /** Specify I/O type dispatch */
        if ( 0 == __event->over) {
            rc = __event->dispatch_handler( __event, __cb, __arg);

            pthread_cleanup_push( (eris_cleanup_routine_t)pthread_mutex_unlock, (eris_void_t *)&(__event->mutex));
            /** Heppen cancel? */

            pthread_mutex_lock( &(__event->mutex));

            /** Dispatch over */
            {
                __event->over++;
            }

            pthread_mutex_unlock( &(__event->mutex));

             pthread_cleanup_pop(0);
        } else { 
            /** Has Over, Thanks used!!! */
            if ( __event->log) {
                eris_log_dump( __event->log, ERIS_LOG_CORE, "Event over flag has been set.");
            }
        }
    } else { rc = EERIS_ERROR; }


    return rc;
}/// eris_over_dispatch


/**
 * @Brief: Set over with dipatch.
 *
 * @Param: __event, Event context.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_event_over( eris_event_t *__event)
{
    if ( __event) {
        pthread_cleanup_push( (eris_cleanup_routine_t)pthread_mutex_unlock, (eris_void_t *)&(__event->mutex));
        /** Heppen cancel? */

        pthread_mutex_lock( &(__event->mutex));

        /** First must is 0 */
        {
            __event->over++;

            /** Call over callback */
            __event->over_handler( __event);
        }

        pthread_mutex_unlock( &(__event->mutex));

        pthread_cleanup_pop(0);
    }
}/// eris_event_over


/**
 * @Brief: Destroy event context.
 *
 * @Param: __event, Event context.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_event_destroy( eris_event_t *__event)
{
    if ( __event) {
        /** Over++ and  Dispatch++ */
        while ( 1 >= __event->over) {
            eris_run_sched_yield();
        }

        /** Destroy I/O context */
        __event->destroy_handler( __event);

        pthread_cleanup_push( (eris_cleanup_routine_t)pthread_mutex_unlock, (eris_void_t *)&(__event->mutex));
        /** Heppen cancel? */

        pthread_mutex_lock( &(__event->mutex));
        {
            eris_event_node_t *cur_node = __event->accept_nodes;
            while ( cur_node) {
                __event->accept_nodes = cur_node->next;

                if ( 0 <= cur_node->elem.sock) {
                    /** Close */
                    (eris_none_t )eris_socket_close( cur_node->elem.sock);
                }

                /** Recover to cache */
                eris_memory_free( cur_node);

                cur_node = __event->accept_nodes;
            }
        }

        /** Destroy event context */
        eris_int_t i = 0;
        for ( i = 0; i < __event->nodes_size; i++) {
            eris_event_node_t *cur_node = __event->nodes[ i];

            while ( cur_node) {
                __event->nodes[ i] = __event->nodes[ i]->next;

                if ( 0 <= cur_node->elem.sock) {
                    /** Close */
                    (eris_none_t )eris_socket_close( cur_node->elem.sock);
                }

                /** Recover to cache */
                eris_memory_free( cur_node);

                cur_node = __event->nodes[ i];
            }
        }
        __event->nodes_size  = 0;
        __event->nodes_count = 0;

        pthread_mutex_unlock( &(__event->mutex));

        pthread_cleanup_pop(0);
        pthread_mutex_destroy( &(__event->mutex));
    }
}/// eris_event_destroy


/**
 * @Brief: Clean up all timeout socket fd.
 *
 * @Param: __event, Event context object.
 * @Param: __cb,    Happen event and doing.
 * @Param: __arg,   Global callback argument.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_event_filing( eris_event_t *__event, eris_event_cb_t __cb, eris_arg_t __arg)
{
    if ( __event) {
        pthread_cleanup_push( (eris_cleanup_routine_t)pthread_mutex_unlock, (eris_void_t *)&(__event->mutex));
        /** Heppen cancel? */

        /** Safe-thread: lock */
        pthread_mutex_lock( &(__event->mutex));

        eris_time_t now_time = 0;
        eris_time_get( now_time);

        eris_int_t i     = 0;
        eris_int_t count = 0;

        /** Clean up timeout socket fd */
        for ( i = 0; i < __event->nodes_size; i++) {
            if ( count > __event->nodes_count) {
                break;
            }

            eris_event_node_t *cur_node  = __event->nodes[ i];
            eris_event_node_t *next_node = NULL;

            while( cur_node) {
                count++;

                next_node = cur_node->next;

                /** Timeout of not events */
                if ( (__event->keepalive < (now_time - cur_node->start)) || (ERIS_EVENT_NONE == cur_node->elem.events) ) {
                    eris_event_elem_t ev_elem; {
                        ev_elem.sock   = cur_node->elem.sock;
                        ev_elem.events = ERIS_EVENT_TIMEOUT;
                    }

                    if ( cur_node == __event->nodes[ i]) {
                        __event->nodes[ i] = cur_node->next;
                    }

                    cur_node->prev ? cur_node->prev->next = cur_node->next : 0;
                    cur_node->next ? cur_node->next->prev = cur_node->prev : 0;
                    cur_node->prev = NULL;
                    cur_node->next = NULL;

                    /** Successed -> call */
                    __event->delete_handler( __event, &(cur_node->elem)); 

                    eris_memory_free( cur_node);

                    __event->nodes_count--;

                    if ( __cb) { __cb( &ev_elem, __arg); } 
                    else { eris_socket_close( ev_elem.sock); }
                }

                cur_node = next_node;
            }
        }

        /** Unlock */
        pthread_mutex_unlock( &(__event->mutex));

        pthread_cleanup_pop(0);
    }
}/// eris_event_filing


/**
 * @Brief: If monitor accept event and execute.
 *
 * @Param: __event, Event object context.
 * @Param: __svc_sock, Server socket object.
 *
 * @Return: Ok is client socket object.
 **/
eris_sock_t eris_event_accept( eris_event_t *__event, eris_sock_t __svc_sock) 
{
    eris_sock_t client_sock = -1;

    if ( 0 < __svc_sock) {
        eris_socklen_t client_addr_size = sizeof( struct sockaddr_in);

        struct sockaddr_in client_addr;
        eris_memory_cleanup( &client_addr, client_addr_size);

        /** Accept client connection */
        client_sock = accept( __svc_sock, (struct sockaddr *)&client_addr, &client_addr_size);
        if ( -1 != client_sock) {
            eris_int_t rc = eris_socket_nonblocking( client_sock);
            if ( 0 == rc) {
                rc = eris_socket_reuseaddr( client_sock);
                if ( 0 == rc) {
                    /** set recv/send buffer size */
                    eris_socket_set_recvbuf( client_sock, __event->recv_buffer_size);
                    eris_socket_set_sendbuf( client_sock, __event->send_buffer_size);

                    if ( 1 == __event->tcp_nodelay) {
                        eris_socket_tcp_nodelay( client_sock);

                    } else {
                        if ( 1 == __event->tcp_nopush) {
                            eris_socket_tcp_nopush( client_sock);
                        }
                    }
                } else {
                    if ( __event->log) {
                        eris_log_dump( __event->log, ERIS_LOG_ERROR, "Set SO_REUSEADDR of socket fd.<%d> failed, errno.<%d>", client_sock, errno);
                    }

                    {
                        eris_socket_close( client_sock);
                        client_sock = -1;
                    }

                }
            } else {
                if ( __event->log) {
                    eris_log_dump( __event->log, ERIS_LOG_ERROR, "Set nonblocking of socket fd.<%d>, errno.<%d>", client_sock, errno);
                }

                {
                    eris_socket_close( client_sock);
                    client_sock = -1;
                }
            }
        }
    }

    return client_sock;
}/// eris_eris_event_accept


