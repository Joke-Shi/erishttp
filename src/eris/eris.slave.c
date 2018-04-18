/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Worker slave :
 **        :    Get request data and parse protocol, and then execute user service,
 **        :    and result data send response to client.
 **
 ******************************************************************************/


#include "eris.config.h"

#include "os/unix/eris.unix.h"
#include "core/eris.core.h"
#include "event/eris.event.h"

#include "eris/erishttp.h"


/** eris slave request input callback of http parse. */
static eris_int_t 
eris_slave_request_incb( eris_buffer_t *__out_buf, eris_size_t __max_size, eris_arg_t __arg, eris_log_t *__log);

/** eris slave response output cb of http response pack. */
static eris_int_t 
eris_slave_response_outcb( eris_buffer_t *__in_buf, eris_size_t __in_size, eris_arg_t __arg, eris_log_t *__log);

/** Set http response base headers. */
static eris_int_t eris_slave_set_response_headers( eris_http_t *__http_context, eris_bool_t __keep_alive);

/** Get file of specify request url. */
static eris_int_t eris_slave_get_file( eris_http_t *__http);

/** Dump log of http request state. */
static eris_none_t eris_slave_log_dump( eris_sock_t __sock, const eris_http_t *__http, const eris_char_t *__tail);



/**
 * @Brief: Eris slave handler of task callback.
 *
 * @Param: __arg, It is eris http context.
 *
 * @Return: Nothing is NULL.
 **/
eris_void_t *eris_slave_handler( eris_void_t *__arg)
{
    eris_int_t   rc = 0;
    eris_http_t *http_context = (eris_http_t *)__arg;

    enum {
        ERIS_SLAVE_GET_WAIT      = 0,
        ERIS_SLAVE_IS_OOB        ,
        ERIS_SLAVE_HTTP_PARSE    ,
        ERIS_SLAVE_HTTP_SHAKE    ,
        ERIS_SLAVE_EXEC_SERVICE  ,
        ERIS_SLAVE_HTTP_HEAD     ,
        ERIS_SLAVE_HTTP_GET      ,
        ERIS_SLAVE_HTTP_2XX      ,
        ERIS_SLAVE_HTTP_3XX      ,
        ERIS_SLAVE_HTTP_4XX      ,
        ERIS_SLAVE_HTTP_5XX      ,
        ERIS_SLAVE_HTTP_PACK     ,
        ERIS_SLAVE_CONN_KEEPALIVE,
        ERIS_SLAVE_CONN_CLOSE    ,
    } eris_slave_state_v = ERIS_SLAVE_GET_WAIT;

    eris_event_elem_t ev_elt; {
        ev_elt.sock   = -1;
        ev_elt.events = 0;
    }

    eris_bool_t is_keepalive     = false;
    eris_size_t content_length_v = 0;

    do {
        switch ( eris_slave_state_v) {
            case ERIS_SLAVE_GET_WAIT :
                {
                    is_keepalive     = false;
                    content_length_v = 0;
                    eris_http_cleanup( http_context);

                    /** Cleanup ev_elt */
                    {
                        ev_elt.sock   = -1;
                        ev_elt.events = 0;
                    }

                    /** Get event element */
                    rc = eris_event_queue_get( &(p_erishttp_context->svc_event_queue), &ev_elt, true);
                    if ( 0 == rc) {
                        if ( 0 < ev_elt.sock ) {
                            if ( ERIS_EVENT_READ & ev_elt.events) {
                                eris_slave_state_v = ERIS_SLAVE_HTTP_PARSE;

                            } else if ( ERIS_EVENT_OOB & ev_elt.events) {
                                eris_slave_state_v = ERIS_SLAVE_IS_OOB;

                            } else { eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE; }
                        }
                    }
                } break;
            case ERIS_SLAVE_IS_OOB :
                {
                    /** Receive a char data */
                    eris_uchar_t c     = 0;
                    eris_int_t   times = 0;
                    do {
                        times++;

                        eris_ssize_t recv_n = recv( ev_elt.sock, &c, sizeof( c), 0);
                        if ( sizeof( c) == recv_n) {
                            break;
                        } else if ( 0 == recv_n) {
                            continue;
                        } else {
                            break;
                        }
                    } while ( 3 < times);

                    if ( ERIS_EVENT_READ & ev_elt.events) {
                        eris_slave_state_v = ERIS_SLAVE_HTTP_PARSE;

                    } else { eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE; }
                } break;
            case ERIS_SLAVE_HTTP_PARSE :
                {
                    /** Get data and parse */
                    rc = eris_http_request_parse( http_context, eris_slave_request_incb, &ev_elt);
                    if ( 0 == rc) {
                        if ( eris_http_request_keep_alive( http_context) ) {
                            is_keepalive = true;
                        }

                        if ( eris_http_request_shake( http_context) ) {
                            /** Get Content-Length */
                            eris_string_t content_length_es = eris_http_request_get_header( http_context, "Content-Length");
                            if ( content_length_es) {
                                content_length_v = eris_string_atol( content_length_es);
                                if ( 0 == content_length_v) {
                                    eris_slave_state_v = ERIS_SLAVE_HTTP_SHAKE;

                                } else { 
                                    eris_http_response_set_status( http_context, ERIS_HTTP_400);

                                    eris_slave_state_v = ERIS_SLAVE_HTTP_4XX; 
                                }
                            } else { eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE; }
                        } else { eris_slave_state_v = ERIS_SLAVE_EXEC_SERVICE; }
                    } else {
                        if ( EERIS_4XX == rc) {
                            eris_slave_state_v = ERIS_SLAVE_HTTP_4XX;

                        } else { eris_slave_state_v = ERIS_SLAVE_HTTP_5XX; }
                    }
                } break;
            case ERIS_SLAVE_HTTP_SHAKE :
                {
                    /** response shake */
                    eris_http_response_pack( http_context, eris_slave_response_outcb, &ev_elt);
                    eris_http_response_set_status( http_context, ERIS_HTTP_000);

                    eris_slave_state_v = ERIS_SLAVE_EXEC_SERVICE;

                    /** Receive data continue */
                    eris_int_t try_times = 0;

                    do {
                        rc = eris_socket_ready_r( ev_elt.sock, 1);
                        if ( 1 == rc) {
                            try_times = 0;

                            eris_uchar_t recv_buffer[4096] = {0};
                            eris_ssize_t recv_n = recv( ev_elt.sock, recv_buffer, sizeof( recv_buffer), 0);
                            if ( 0 < recv_n) {
                                /** Save data */
                                rc = eris_http_response_set_body_append( http_context, recv_buffer, recv_n);
                                if ( 0 != rc) {
                                    eris_http_response_set_status( http_context, ERIS_HTTP_500);

                                    eris_slave_state_v = ERIS_SLAVE_HTTP_5XX;

                                    break;
                                }
                            } else {
                                /** error */
                                if ( -1 == recv_n) {
                                    eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE;

                                    break;
                                }
                            }

                        } else if (0 == rc) {
                            try_times++;

                            /** Timeout */
                            if ( try_times > p_erishttp_context->attrs.timeout) {
                                eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE;

                                break;
                            }
                        } else {
                            eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE;

                            break;
                        }
                    } while ( eris_http_request_get_body_size( http_context) < content_length_v);
                } break;
            case ERIS_SLAVE_EXEC_SERVICE :
                {
                    eris_http_response_set_status( http_context, ERIS_HTTP_200);

                    if ( (ERIS_HTTP_GET    == http_context->request.command) ||
                         (ERIS_HTTP_DELETE == http_context->request.command) ||
                         (ERIS_HTTP_HEAD   == http_context->request.command) ||
                         (ERIS_HTTP_PATCH  == http_context->request.command) ||
                         (ERIS_HTTP_POST   == http_context->request.command) ||
                         (ERIS_HTTP_PUT    == http_context->request.command) ) {
                        /** Call module service */
                        rc = eris_module_exec( &(p_erishttp_context->mcontext), 
                                               eris_http_request_get_url( http_context), 
                                               http_context);
                        if ( 0 == rc) {
                            eris_int_t resp_status = eris_http_response_get_status( http_context);

                            if ( ERIS_HTTP_000 == resp_status ) {
                                eris_http_response_set_status( http_context, ERIS_HTTP_200);

                                eris_slave_state_v = ERIS_SLAVE_HTTP_2XX;
                            } else {
                                /** Is valid status */
                                if ( !eris_http_status_valid( resp_status)) {
                                    eris_http_response_set_status( http_context, ERIS_HTTP_200);

                                    eris_slave_state_v = ERIS_SLAVE_HTTP_2XX;
                                } else {
                                    if ( (ERIS_HTTP_200 <= resp_status) && (ERIS_HTTP_206 >= resp_status)) {
                                        eris_slave_state_v = ERIS_SLAVE_HTTP_2XX;

                                    } else if ( (ERIS_HTTP_300 <= resp_status) && (ERIS_HTTP_307 >= resp_status)) {
                                        eris_slave_state_v = ERIS_SLAVE_HTTP_3XX;

                                    } else if ( (ERIS_HTTP_400 <= resp_status) && (ERIS_HTTP_449 >= resp_status)) {
                                        eris_slave_state_v = ERIS_SLAVE_HTTP_4XX;

                                    } else if ( (ERIS_HTTP_500 <= resp_status) && (ERIS_HTTP_511 >= resp_status)) {
                                        eris_slave_state_v = ERIS_SLAVE_HTTP_5XX;

                                    } else {
                                        /** nothing */
                                        eris_slave_state_v = ERIS_SLAVE_HTTP_2XX;
                                    }
                                }
                            }
                        } else {
                            /** Call service failed */
                            if ( EERIS_NOTFOUND == rc) {
                                if ( ERIS_HTTP_GET  == http_context->request.command) {
                                    /** Get static file??? */
                                    eris_slave_state_v = ERIS_SLAVE_HTTP_GET;

                                } else if (ERIS_HTTP_HEAD == http_context->request.command ) {
                                    /** Get static file??? */
                                    eris_slave_state_v = ERIS_SLAVE_HTTP_HEAD;

                                } else {
                                    eris_slave_state_v = ERIS_SLAVE_HTTP_4XX;

                                    eris_http_response_set_status( http_context, ERIS_HTTP_404);
                                }
                            } else if (EERIS_UNSUPPORT == rc ) {
                                eris_slave_state_v = ERIS_SLAVE_HTTP_4XX;

                                eris_http_response_set_status( http_context, ERIS_HTTP_406);

                            } else {
                                eris_slave_state_v = ERIS_SLAVE_HTTP_5XX;

                                eris_http_response_set_status( http_context, ERIS_HTTP_500);
                            }
                        }
                    } else {
                        eris_slave_state_v = ERIS_SLAVE_HTTP_4XX;

                        eris_http_response_set_status( http_context, ERIS_HTTP_405);
                    }
                } break;
            case ERIS_SLAVE_HTTP_GET :
            case ERIS_SLAVE_HTTP_HEAD:
                {
                    /** Get file */
                    rc = eris_slave_get_file( http_context);
                    if ( 0 == rc) {
                        /** Get file content ok */
                        eris_slave_state_v = ERIS_SLAVE_HTTP_2XX;

                        eris_http_response_set_status( http_context, ERIS_HTTP_200);

                    } else if ( EERIS_NOTFOUND == rc) {
                        /** Not found */
                        eris_slave_state_v = ERIS_SLAVE_HTTP_4XX;

                        eris_http_response_set_status( http_context, ERIS_HTTP_404);
                    } else if ( EERIS_NOACCESS == rc ) {
                        /** Not access */
                        eris_slave_state_v = ERIS_SLAVE_HTTP_4XX;

                        eris_http_response_set_status( http_context, ERIS_HTTP_406);
                    } else {
                        /** server error */
                        eris_slave_state_v = ERIS_SLAVE_HTTP_5XX;

                        eris_http_response_set_status( http_context, ERIS_HTTP_500);
                    }
                } break;
            case ERIS_SLAVE_HTTP_2XX :
                {
                    eris_slave_state_v = ERIS_SLAVE_HTTP_PACK;

                    eris_http_response_set_version( http_context, eris_http_request_get_version( http_context));

                    rc = eris_slave_set_response_headers( http_context, is_keepalive);
                    if ( 0 == rc) {
                        /** Set content-xxx */
                        eris_string_t resp_size_es;
                        eris_string_init( resp_size_es);

                        eris_size_t resp_size = eris_http_response_get_body_size( http_context);
                        rc = eris_string_ultoa( &resp_size_es, resp_size);
                        if ( 0 == rc) {
                            rc = eris_http_response_set_header( http_context, "Content-Length", resp_size_es);
                            if ( 0 != rc) {
                                eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE;

                            }
                        } else { eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE; }

                        eris_string_free( resp_size_es);
                        eris_string_init( resp_size_es);

                    } else { eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE; }
                } break;
            case ERIS_SLAVE_HTTP_3XX :
                {
                    eris_slave_state_v = ERIS_SLAVE_HTTP_PACK;

                    eris_http_response_set_version( http_context, eris_http_request_get_version( http_context));

                    rc = eris_slave_set_response_headers( http_context, is_keepalive);
                    if ( 0 == rc) {
                        /** Set content-xxx */
                        eris_string_t resp_size_es;
                        eris_string_init( resp_size_es);

                        eris_size_t resp_size = eris_http_response_get_body_size( http_context);
                        rc = eris_string_ultoa( &resp_size_es, resp_size);
                        if ( 0 == rc) {
                            rc = eris_http_response_set_header( http_context, "Content-Length", resp_size_es);
                            if ( 0 != rc) {
                                eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE;

                            }
                        } else { eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE; }

                        eris_string_free( resp_size_es);
                        eris_string_init( resp_size_es);

                    } else { eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE; }
                } break;
            case ERIS_SLAVE_HTTP_4XX :
            case ERIS_SLAVE_HTTP_5XX :
                {
                    rc = 0;

                    eris_slave_state_v = ERIS_SLAVE_HTTP_PACK;

                    eris_http_response_set_version( http_context, eris_http_request_get_version( http_context));

                    /** Set content-xxx */
                    eris_string_t resp_size_es;
                    eris_string_init( resp_size_es);

                    /** Set content-type and error message */
                    eris_size_t resp_size = eris_http_response_get_body_size( http_context);
                    if ( 0 == resp_size) {
                        eris_int_t resp_status = eris_http_response_get_status( http_context);

                        const eris_char_t *p_reason_phrace = eris_http_status_reason_phrace( resp_status); 

                        eris_string_t error_msg_es;
                        eris_string_init( error_msg_es);

                        rc = eris_string_printf( &error_msg_es, 
                                                 "<!DOCTYPE html><html><head><title>%i</title></head>"
                                                 "<body><h2>Error: %i %s</h2></body></html>", 
                                                 resp_status,
                                                 resp_status,
                                                 p_reason_phrace);
                        if ( 0 == rc) {
                            rc = eris_http_response_set_body( http_context, error_msg_es, eris_string_size( error_msg_es));
                            if ( 0 == rc) {
                                rc = eris_http_response_set_header( http_context, "Content-Type", "text/html");
                                if ( 0 == rc) {
                                    rc = eris_slave_set_response_headers( http_context, is_keepalive);
                                    if ( 0 != rc) {

                                        eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE;
                                    }
                                } else { eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE; }
                            } else { eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE; }
                        } else { eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE; }

                        eris_string_free( error_msg_es);
                        eris_string_init( error_msg_es);
                    }/// fi ( 0 == resp_size)

                    if ( ERIS_SLAVE_HTTP_PACK == eris_slave_state_v) {
                        /** Set content-length */
                        resp_size = eris_http_response_get_body_size( http_context);
                        rc = eris_string_ultoa( &resp_size_es, resp_size);
                        if ( 0 == rc) {
                            rc = eris_http_response_set_header( http_context, "Content-Length", resp_size_es);
                            if ( 0 != rc) {
                                eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE;

                            }
                        } else { eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE; }
                    }

                    eris_string_free( resp_size_es);
                    eris_string_init( resp_size_es);
                } break;
            case ERIS_SLAVE_HTTP_PACK :
                {
                    /** output response to client */
                    rc = eris_http_response_pack( http_context, eris_slave_response_outcb, &ev_elt);
                    if ( 0 == rc) {
                        if ( is_keepalive) {
                            eris_slave_state_v = ERIS_SLAVE_CONN_KEEPALIVE;

                        } else { eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE; }
                    } else { eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE; }
                } break;
            case ERIS_SLAVE_CONN_KEEPALIVE :
                {
                    /** Resave into event context monitor */
                    ev_elt.events = ERIS_EVENT_READ;

                    rc = eris_event_modify( &(p_erishttp_context->svc_event), &ev_elt);
                    if ( 0 == rc) {
                        eris_slave_state_v = ERIS_SLAVE_GET_WAIT;

                        eris_slave_log_dump( ev_elt.sock, http_context, "ok");
                    } else { 
                        eris_slave_state_v = ERIS_SLAVE_CONN_CLOSE; 

                        eris_slave_log_dump( ev_elt.sock, http_context, "failed");
                    }
                } break;
            case ERIS_SLAVE_CONN_CLOSE :
                {
                    {
                        eris_event_elem_t del_ev_elt; {
                            del_ev_elt.sock   = ev_elt.sock;
                            del_ev_elt.events = ev_elt.events;
                        }
                        eris_event_delete( &(p_erishttp_context->svc_event), &del_ev_elt);
                    }

                    eris_socket_close( ev_elt.sock);

                    eris_slave_state_v = ERIS_SLAVE_GET_WAIT;
                } break;
            default : break;
        }/// switch ( eris_slave_state_v)

    } while ( 1);

    return NULL;
}/// eris_slave_handler


/**
 * @Brief: eris slave request input callback of http parse, 
         : receive data from connect socket.
 *
 * @Param: __out_buf, Get data from connect socket, and save in there output.
 * @Param: __max_size,Get data max size.
 * @Param: __arg,     Input argument, it is connect socket context.
 * @Param: __log,     Happen something and dump log meesage by it.
 *
 * @Return: Ok is 0, Other is -1.
 */
static eris_int_t 
eris_slave_request_incb( eris_buffer_t *__out_buf, eris_size_t __max_size, eris_arg_t __arg, eris_log_t *__log)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    eris_buffer_cleanup( __out_buf, __log);

    eris_event_elem_t *p_ev_elt = (eris_event_elem_t *)__arg;

    rc = eris_socket_ready_r( p_ev_elt->sock, p_erishttp_context->attrs.timeout);
    if ( 1 == rc) {
        rc = 0;
        do {
            eris_uchar_t recv_buffer[4096] = {0};

            eris_ssize_t recv_n = recv( p_ev_elt->sock, recv_buffer, sizeof( recv_buffer), 0);
            if ( 0 < recv_n) {
                /** Save data */
                rc = eris_buffer_append( __out_buf, recv_buffer, recv_n, __log);
                if ( 0 != rc) {
                    rc = -1;
                }

                break;
            } else if ( 0 == recv_n) {

                break;
            } else {
                if ( EINTR == errno) { 
                    continue;
                } else { rc = -1; break; }
            }
        } while ( 0);
    } else { rc = -1; }

    errno = tmp_errno;

    return rc;
}/// eris_slave_request_incb


/**
 * @Brief: eris slave response output callback of http response pack, 
         : for send data to client.
 *
 * @Param: __in_buf, Input data buffer, and send to client.
 * @Param: __in_size,Input data buffer size.
 * @Param: __arg,    Input argument, it is connect socket context.
 * @Param: __log,    Happen something and dump log meesage by it.
 *
 * @Return: Ok is 0, Other is -1.
 **/
static eris_int_t 
eris_slave_response_outcb( eris_buffer_t *__in_buf, eris_size_t __in_size, eris_arg_t __arg, eris_log_t *__log)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    eris_event_elem_t *p_ev_elt = (eris_event_elem_t *)__arg;

    if ( 0 < __in_size) {
        rc = eris_socket_ready_w( p_ev_elt->sock, p_erishttp_context->attrs.timeout);
        if ( 1 == rc) {
            rc = 0;

            eris_size_t send_count = 0;
            do {
                eris_ssize_t send_n = send( p_ev_elt->sock, __in_buf->data + send_count, __in_size - send_count, 0);
                if ( 0 < send_n) {
                    send_count += send_n;

                }
                
                if ( 0 == send_n) {
                    continue;
                } 
                
                if ( 0 > send_n ){
                    if ( EINTR == errno) {
                        continue;
                    } else { rc = -1; break; }
                }
            } while ( send_count < __in_size);
        } else { rc = -1; /** Timeout is bad request */}
    }

    errno = tmp_errno;

    return rc;
}/// eris_slave_response_outcb


/**
 * @Brief: Set http response base headers.
 *
 * @Param: __http_context, Http object context.
 * @Param: __keepalive,    Keep alive flags.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
static eris_int_t eris_slave_set_response_headers( eris_http_t *__http_context, eris_bool_t __keep_alive)
{
    eris_int_t rc = 0;

    eris_string_t date_es;
    eris_string_init( date_es);

    if ( eris_time_gmt( &date_es) ) {
        /** Set base response headers */
        rc = eris_http_response_set_header( __http_context, "Server", ERIS_VERSION_DESC);
        if ( 0 == rc) {
            rc = eris_http_response_set_header( __http_context, "Date"  , date_es);
        }
        
        if ( 0 == rc) {
            if ( __keep_alive) {
                rc = eris_http_response_set_header( __http_context, "Connection", "keep-alive");

            } else {
                rc = eris_http_response_set_header( __http_context, "Connection", "close");
            }
        }
    
        if ( 0 == rc) {
            if ( !eris_http_response_get_header( __http_context, "Content-Type")) {
                rc = eris_http_response_set_header( __http_context, "Content-Type", "application/octet-stream");
            }
        }

        /** 405 */
        if ( 0 == rc ) {
            if ( ERIS_HTTP_405 == __http_context->response.status) {
                rc = eris_http_response_set_header( __http_context, "Allow", "GET DELETE HEAD PATCH POST PUT");
            }
        }
    } else { rc = -1; }

    eris_string_free( date_es);
    eris_string_init( date_es);

    return rc;
}/// eris_slave_set_response_headers


/**
 * @Brief: Get file of specify request url.
 *
 * @Param: __http, Eris http context handler.
 *
 * @Return: OK is 0, 
 *        : Not found is EERIS_NOTFOUND, no access is EERIS_NOACCESS,
 *        : other is EERIS_ERROR.
 **/
static eris_int_t eris_slave_get_file( eris_http_t *__http)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    eris_string_t file_path_es;
    eris_string_init( file_path_es);

    eris_string_t clone_url_es = eris_string_clone_eris( __http->request.url);
    if ( clone_url_es) {
        if ( eris_string_isequal( clone_url_es, "/", false)) {
            rc = eris_string_set( &clone_url_es, "/index.html");
        }

        if ( 0 == rc) {
            rc = eris_string_printf( &file_path_es, "%S%S", p_erishttp_context->attrs.doc_root, clone_url_es);
            if ( 0 == rc) {
                /** Dump get file */
                if ( ERIS_HTTP_GET == __http->request.command) {
                    eris_log_dump( &(p_erishttp_context->module_log), ERIS_LOG_INFO, "Get.<%s>", file_path_es);

                } else {
                    eris_log_dump( &(p_erishttp_context->module_log), ERIS_LOG_INFO, "HEAD.<%s>", file_path_es);
                }

                if ( 0 == access( file_path_es, F_OK)) {
                    eris_attrfs_t file_type = eris_attrfs_get( file_path_es);
                    if ( ERIS_ATTRFS_REG == file_type) {
                        eris_files_t fcontext;
                        rc = eris_files_init( &fcontext, file_path_es, &(p_erishttp_context->errors_log));
                        if ( 0 == rc) {
                            rc = eris_files_open( &fcontext, ERIS_ATTRFS_RDONLY, 0);
                            if ( 0 == rc) {
                                //eris_buffer_init( &(__http->response.body), NULL);

                                eris_size_t  r_count = 0;
                                eris_uchar_t r_cache[ 512] = {0};

                                do {
                                    /** Read file... */
                                    eris_ssize_t r_size = eris_files_read( &fcontext, r_cache, sizeof( r_cache), r_count);
                                    if ( 0 < r_size) {
                                        r_count += r_size;

                                        rc = eris_http_response_set_body_append( __http, (const eris_void_t *)r_cache, r_size);
                                        if ( 0 != rc) {
                                            rc = EERIS_ERROR;

                                            break;
                                        }
                                    } else if (0 == r_size) {
                                        /** End */
                                        break;
                                    } else {
                                        if ( EINTR == errno) {
                                            continue;
                                        } else {
                                            rc = EERIS_ERROR;

                                            break;
                                        }
                                    }
                                } while ( 1);

                                /** Read data ok */
                                if ( 0 == rc) {
                                    /** Get file suffix */
                                    eris_size_t suffix_idx = eris_string_rfind( file_path_es, ".");
                                    if ( ERIS_STRING_NPOS != suffix_idx) {
                                        eris_string_t suffix_es = eris_string_assign( file_path_es, (suffix_idx + 1), -1);
                                        if ( suffix_es) {
                                            (eris_none_t )eris_string_append_c( &suffix_es, ' ');

                                            eris_list_iter_t mt_iter; {
                                                eris_list_iter_init( &(p_erishttp_context->mime_types), &mt_iter);
                                            }

                                            /** Found Content-Type */
                                            erishttp_mime_type_t *p_mt = NULL;
                                            for ( ; (NULL != (p_mt = eris_list_iter_next( &mt_iter))); ) {
                                                if ( ERIS_STRING_NPOS != eris_string_find_eris( p_mt->types, suffix_es)) {
                                                    /** Set MIME-type */
                                                    eris_http_response_set_header( __http, "Content-Type", p_mt->mime);

                                                    break;
                                                }
                                            }

                                            eris_string_free( suffix_es);
                                            eris_string_init( suffix_es);
                                        }
                                    }
                                }
                            } else {
                                if ( 0 != access( file_path_es, F_OK)) {
                                    rc = EERIS_NOTFOUND;

                                } else {
                                    rc = EERIS_NOACCESS;
                                }
                            }

                            /** Destroy file context */
                            eris_files_destroy( &fcontext);
                        } else { rc = EERIS_ERROR; }
                    } else { rc = EERIS_NOACCESS; }
                } else { rc = EERIS_NOTFOUND; }
            } else { rc = EERIS_ERROR; }
        } else { rc = EERIS_ERROR; }

        eris_string_free( clone_url_es);
        eris_string_init( clone_url_es);

    } else { rc = EERIS_ERROR; }

    eris_string_free( file_path_es);
    eris_string_init( file_path_es);

    errno = tmp_errno;

    return rc;
}/// eris_slave_get_file


/**
 * @Brief: Dump log of http request state.
 *
 * @Param: __sock, Client socket object.
 * @Param: __http, Http context handler.
 * @Param: __tail, "ok" or "failed" send data to client.
 *
 * @Return: Nothing.
 **/
static eris_none_t eris_slave_log_dump( eris_sock_t __sock, const eris_http_t *__http, const eris_char_t *__tail)
{
    const eris_char_t   *p_req_command  = NULL;
    const eris_char_t   *p_resp_version = NULL;
    const eris_string_t  req_url_es     = eris_http_request_get_url( __http); 
    const eris_string_t  req_user_agent = eris_http_request_get_header( __http, "User-Agent");
    eris_int_t           resp_status    = eris_http_response_get_status( __http);
    eris_size_t          resp_body_size = eris_http_response_get_body_size( __http);


    /** Get request command */
    eris_http_command_t req_command = eris_http_request_get_command( __http);

    if ( (0 < req_command) && (req_command < eris_http_command_arrs_size)) {
        p_req_command = eris_http_command_arrs[ req_command];
    } else {
        p_req_command = "Unknow";
    }

    /** Get response version */
    eris_http_version_t resp_version = eris_http_response_get_version( __http);
    switch ( resp_version) {
        case ERIS_HTTP_V11 :
            {
                p_resp_version = eris_http_version_tos(11);
            } break;
        case ERIS_HTTP_V10 :
            {
                p_resp_version = eris_http_version_tos(10);
            } break;
        case ERIS_HTTP_V09 :
            {
                p_resp_version = eris_http_version_tos(09);
            } break;
        default :
            {
                p_resp_version = eris_http_version_tos(10);
            } break;
    }

    eris_socket_host_t client_host;

    if ( 0 == eris_socket_host( __sock, &client_host)) {
        eris_log_dump( &(p_erishttp_context->access_log), ERIS_LOG_NOTICE, 
                       "(%s:%d-%d) - \"%s %s %s\" - \"%d %lu\" \"%s\" - %s", 
                       client_host.ipv4, 
                       client_host.port,
                       __sock,
                       p_req_command,
                       req_url_es,
                       p_resp_version,
                       resp_status,
                       resp_body_size,
                       req_user_agent,
                       __tail);
    }
}/// eris_slave_log_dump



