/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Http protocol operators, eg: 
 **        :    set   header
 **        :    get   header
 **        :    pack  request
 **        :    pack  response
 **        :    parse request
 **        :    parse response
 **        : and so on.
 **
 ******************************************************************************/

#include "core/eris.core.h"


/** Command map to string */
const eris_char_t *eris_http_command_arrs[] = {
#define CX(n, name, s) #s,
    ERIS_HTTP_COMMAND(CX)
    NULL
#undef CX
};

/** Status map reason phrace string */
const eris_http_status_map_reason_t eris_http_status_map_reason[] = {
#define SX(n, name, s) {n, #name, #s},
    ERIS_HTTP_STATUS(SX)
#undef SX
};


/*
 * @Brief: Eris-http init.
 *
 * @Param: __http,  Eris http context.
 * @Param: __attrs, Specify attributes of http limit info.
 * @Param: __log,   Log context.
 * 
 * @Return: Ok is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_http_init( eris_http_t *__http, eris_http_attr_t *__attrs, eris_log_t *__log)
{
    eris_int_t rc = 0;

    if ( __http) {
        __http->chunked        = false;
        __http->chunked_end    = false;
        __http->chunk_size     = 0;
        __http->tmp_chunk_size = 0;

        /** Init all attrs */
        if ( __attrs) {
            __http->attrs.url_max_size      = __attrs->url_max_size      ? __attrs->url_max_size      : 1024;
            __http->attrs.header_max_size   = __attrs->header_max_size   ? __attrs->header_max_size   : 4096;
            __http->attrs.body_max_size     = __attrs->body_max_size     ? __attrs->body_max_size     : 10485760;
            __http->attrs.header_cache_size = __attrs->header_cache_size ? __attrs->header_cache_size : 4096;
            __http->attrs.body_cache_size   = __attrs->body_cache_size   ? __attrs->body_cache_size   : 16384;
        } else {
            /** Default */
            __http->attrs.url_max_size      = 1024;       /** 1KB  */
            __http->attrs.header_max_size   = 4096;       /** 4KB  */
            __http->attrs.body_max_size     = 10485760;   /** 10MB */
            __http->attrs.header_cache_size = 4096;       /** 4KB  */
            __http->attrs.body_cache_size   = 16384;      /** 16KB */
        }

        /** Init request handler */
        {
            __http->request.shake      = false;
            __http->request.command    = ERIS_HTTP_UNKNOW;
            __http->request.url        = NULL;
            __http->request.version    = ERIS_HTTP_V00;
            __http->request.headers    = NULL;
            __http->request.parameters = NULL;
            __http->request.query      = NULL;
            __http->request.fragment   = NULL;
            
            rc = eris_buffer_init( &(__http->request.body), 0, __log);
            if ( 0 != rc) {
                if ( __log) {
                    eris_log_dump( __log, ERIS_LOG_CORE, "Init request body buffer failed, errno.<%d>", errno);
                }
            }
        }

        /** Init Request handler */
        if ( 0 == rc) {
            __http->response.version   = ERIS_HTTP_V00;
            __http->response.status    = ERIS_HTTP_000;
            __http->response.reason    = NULL;
            __http->response.headers   = NULL;
            
            rc = eris_buffer_init( &(__http->response.body), 0, __log);
            if ( 0 != rc) {
                /** Destroy request body buffer */
                eris_buffer_destroy( &(__http->request.body));

                if ( __log) {
                    eris_log_dump( __log, ERIS_LOG_CORE, "Init response body buffer failed, errno.<%d>", errno);
                }
            }
        }

        if ( 0 == rc) {
            rc = eris_buffer_init( &(__http->hbuffer), __http->attrs.header_cache_size, __log);
            if ( 0 == rc) {
                rc = eris_buffer_init( &(__http->bbuffer), __http->attrs.body_cache_size, __log);
                if ( 0 != rc) {
                    /** Destroy request body buffer */
                    eris_buffer_destroy( &(__http->request.body));

                    /** Destroy response body buffer */
                    eris_buffer_destroy( &(__http->response.body));

                    /** Destroy header buffer */
                    eris_buffer_destroy( &(__http->hbuffer));

                    if ( __log) {
                        eris_log_dump( __log, ERIS_LOG_CORE, "Init body buffer failed, errno.<%d>", errno);
                    }
                }
            } else {
                /** Destroy request body buffer */
                eris_buffer_destroy( &(__http->request.body));

                /** Destroy response body buffer */
                eris_buffer_destroy( &(__http->response.body));

                if ( __log) {
                    eris_log_dump( __log, ERIS_LOG_CORE, "Init header buffer failed, errno.<%d>", errno);
                }
            }
        }

        /** Specify log context */
        __http->log = __log;
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_http_init


/**
 * @Brief: Read http-request data package from callback input.
 *
 * @Param: __http, Eris http context.
 * @Param: __incb, Read http-request data from input callback.
 * @Param: __arg,  Call callback and input argument.
 *
 * @Return: Ok is 0, Internal error is EERIS_ERROR or EERIS_4XX.
 **/
eris_int_t eris_http_request_parse( eris_http_t *__http, eris_http_cb_t __incb, eris_arg_t __arg)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __http) {
        /** Clean up http request context */
        eris_http_cleanup_request( __http);
        eris_buffer_cleanup( &(__http->hbuffer), NULL);
        eris_buffer_cleanup( &(__http->bbuffer), NULL);

        __http->chunk_state    = ERIS_HTTP_CHUNKED;
        __http->chunked        = false;
        __http->chunked_end    = false;
        __http->chunk_size     = 0;
        __http->tmp_chunk_size = 0;

        if ( __incb) {
            eris_bool_t header_finish_flag = false;

            /** HTTP request protocol state */
            enum {
                ERIS_HTTP_SREQ_START       = 0,
                ERIS_HTTP_SREQ_COMMAND     ,
                ERIS_HTTP_SREQ_URL_START   ,
                ERIS_HTTP_SREQ_URL         ,
                ERIS_HTTP_SREQ_PARAMETERS  ,
                ERIS_HTTP_SREQ_QUERY       ,
                ERIS_HTTP_SREQ_FRAGMENT    ,
                ERIS_HTTP_SREQ_H           ,
                ERIS_HTTP_SREQ_HT          ,
                ERIS_HTTP_SREQ_HTT         ,
                ERIS_HTTP_SREQ_HTTP        ,
                ERIS_HTTP_SREQ_SLASH       ,
                ERIS_HTTP_SREQ_MAJOR       ,
                ERIS_HTTP_SREQ_DOT         ,
                ERIS_HTTP_SREQ_MINOR       ,
                ERIS_HTTP_SREQ_TITLE_CR    ,
                ERIS_HTTP_SREQ_TITLE_LF    ,
                ERIS_HTTP_SREQ_HEADER_NAME ,
                ERIS_HTTP_SREQ_HEADER_COLON,
                ERIS_HTTP_SREQ_HEADER_VALUE,
                ERIS_HTTP_SREQ_HEADER_CR1  ,
                ERIS_HTTP_SREQ_HEADER_LF1  ,
                ERIS_HTTP_SREQ_HEADER_CR2  ,
                ERIS_HTTP_SREQ_HEADER_LF2  ,
                ERIS_HTTP_SREQ_FINISH             ,
            } eris_http_sreq_v = ERIS_HTTP_SREQ_START;

            /** Do parse header */
            eris_string_t tmp_s  = eris_string_alloc( 32);
            eris_string_t hname  = eris_string_alloc( 32);
            eris_string_t hvalue = eris_string_alloc( 128);

            eris_size_t header_size = 0;

            do {
                if ( !header_finish_flag && (0 == rc)) {
                    /** Get header content from callback */
                    rc = __incb( &(__http->hbuffer), __http->attrs.header_cache_size, __arg, __http->log);
                    if ( EERIS_ERROR != rc) {
                        eris_size_t ph_size   = __http->hbuffer.size;
                        const eris_char_t *ph = (const eris_char_t *)__http->hbuffer.data;

                        /** Has data and parse */
                        if ( 0 < ph_size) {
                            eris_size_t i = 0;

                            for ( i = 0; i < ph_size; i++) {
                                header_size++;

                                /** Check header size is up limit max? */
                                if ( header_size > __http->attrs.header_max_size) {
                                    rc = EERIS_4XX;

                                    /** Internal server error */
                                    __http->response.status  = ERIS_HTTP_431;
                                    __http->response.version = ERIS_HTTP_V10;
                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 431));

                                    if ( __http->log) {
                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Request header content too large");
                                    }

                                    break;
                                }

                                /** End of header parser */
                                if ( header_finish_flag ) { break; }

                                switch ( eris_http_sreq_v) {
                                    case ERIS_HTTP_SREQ_START :
                                        {
                                            if ( 0 == eris_string_append_c( &tmp_s, ph[ i])) {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_COMMAND;

                                            } else {
                                                rc = EERIS_ERROR;

                                                eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                /** Internal server error */
                                                __http->response.status  = ERIS_HTTP_500;
                                                __http->response.version = ERIS_HTTP_V10;
                                                eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a command charactor failed, errno.<%d>", errno);
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_COMMAND :
                                        {
                                            if ( ' ' != ph[ i]) {
                                                if ( 0 != eris_string_append_c( &tmp_s, ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Server internal error */
                                                    __http->response.status  = ERIS_HTTP_500;
                                                    __http->response.version = ERIS_HTTP_V10;

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a command charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_URL_START;

                                                /** End command string */
                                                if ( eris_string_isequal( tmp_s, "GET", true)) {
                                                    __http->request.command = ERIS_HTTP_GET;

                                                } else if ( eris_string_isequal( tmp_s, "HEAD",    true)) {
                                                    __http->request.command = ERIS_HTTP_HEAD;

                                                } else if ( eris_string_isequal( tmp_s, "POST",    true)) {
                                                    __http->request.command = ERIS_HTTP_POST;

                                                } else if ( eris_string_isequal( tmp_s, "PUT",     true)) {
                                                    __http->request.command = ERIS_HTTP_PUT;

                                                } else if ( eris_string_isequal( tmp_s, "DELETE",  true)) {
                                                    __http->request.command = ERIS_HTTP_DELETE;

                                                } else if ( eris_string_isequal( tmp_s, "OPTIONS", true)) {
                                                    __http->request.command = ERIS_HTTP_OPTIONS;

                                                } else if ( eris_string_isequal( tmp_s, "TRACE",   true)) {
                                                    __http->request.command = ERIS_HTTP_TRACE;

                                                } else if ( eris_string_isequal( tmp_s, "PATCH",   true)) {
                                                    __http->request.command = ERIS_HTTP_PATCH;

                                                } else if ( eris_string_isequal( tmp_s, "MOVE",    true)) {
                                                    __http->request.command = ERIS_HTTP_MOVE;

                                                } else if ( eris_string_isequal( tmp_s, "COPY",    true)) {
                                                    __http->request.command = ERIS_HTTP_COPY;

                                                } else if ( eris_string_isequal( tmp_s, "LINK",    true)) {
                                                    __http->request.command = ERIS_HTTP_LINK;

                                                } else if ( eris_string_isequal( tmp_s, "UNLINK",  true)) {
                                                    __http->request.command = ERIS_HTTP_UNLINK;

                                                } else if ( eris_string_isequal( tmp_s, "PURGE",   true)) {
                                                    __http->request.command = ERIS_HTTP_PURGE;

                                                } else if ( eris_string_isequal( tmp_s, "LOCK",    true)) {
                                                    __http->request.command = ERIS_HTTP_LOCK;

                                                } else if ( eris_string_isequal( tmp_s, "UNLOCK",  true)) {
                                                    __http->request.command = ERIS_HTTP_UNLOCK;

                                                } else if ( eris_string_isequal( tmp_s, "PROPFIND",true)) {
                                                    __http->request.command = ERIS_HTTP_PROPFIND;

                                                } else if ( eris_string_isequal( tmp_s, "VIEW",    true)) {
                                                    __http->request.command = ERIS_HTTP_VIEW;

                                                } else {
                                                    rc = EERIS_DATA;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Request method not allowed */
                                                    __http->response.status  = ERIS_HTTP_405;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 405));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a command charactor failed, errno.<%d>", errno);
                                                    }
                                                }

                                                eris_string_cleanup( tmp_s);
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_URL_START :
                                        {
                                            if ( '/' == ph[ i]) {
                                                /** Store a url charactor */
                                                if ( 0 == eris_string_append_c( &(__http->request.url), ph[ i])) {
                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_URL;

                                                } else {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_500;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a url charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_4XX;

                                                eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                /** Bad request */
                                                __http->response.status  = ERIS_HTTP_400;
                                                __http->response.version = ERIS_HTTP_V10;
                                                eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad request url");
                                                }
                                            }    
                                        } break;
                                    case ERIS_HTTP_SREQ_URL :
                                        {
                                            if ( (' ' == ph[ i]) || (';' == ph[ i]) || ('?' == ph[ i]) || ('#' == ph[ i])) {
                                                if ( ' ' == ph[ i]) {
                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_H;

                                                } else if ( ';' == ph[ i]) {
                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_PARAMETERS;

                                                } else if ( '?' == ph[ i]) {
                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_QUERY;

                                                } else { 
                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FRAGMENT;
                                                }
                                            } else if ( '\r' == ph[ i]) {
                                                /** Is HTTP/0.9? check request method it is GET? */
                                                if ( ERIS_HTTP_GET == __http->request.command) {
                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_TITLE_CR;
                                                    
                                                    __http->request.version = ERIS_HTTP_V09;
                                                } else {
                                                    rc = EERIS_4XX;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_405;
                                                    __http->response.version = ERIS_HTTP_V09;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 405));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Request version is HTTP/0.9, method is not GET");
                                                    }
                                                }
                                            } else {
                                                /** Store a url charactor */
                                                if ( 0 != eris_string_append_c( &(__http->request.url), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_500;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a url charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            }

                                            /** Check url size */
                                            if ( 0 == rc) {
                                                eris_size_t url_size = eris_string_size( __http->request.url);
                                                if ( url_size > __http->attrs.url_max_size) {
                                                    rc = EERIS_4XX;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Request url too long */
                                                    __http->response.status  = ERIS_HTTP_414;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 414));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Request URI too long");
                                                    }
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_PARAMETERS :
                                        {
                                            if ( (' ' == ph[ i]) || ('?' == ph[ i]) || ('#' == ph[ i])) {
                                                if ( ' ' == ph[ i]) {
                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_H;

                                                } else if ( '?' == ph[ i]) {
                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_QUERY;

                                                } else { 
                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FRAGMENT;
                                                }

                                            } else if ( '\r' == ph[ i]) {
                                                /** Is HTTP/0.9? check request method it is GET? */
                                                if ( ERIS_HTTP_GET == __http->request.command) {
                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_TITLE_CR;
                                                    
                                                } else {
                                                    rc = EERIS_4XX;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_405;
                                                    __http->response.version = ERIS_HTTP_V09;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 405));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Request version is HTTP/0.9, method is not GET");
                                                    }
                                                }
                                            } else {
                                                /** Store a parameters charactor */
                                                if ( 0 != eris_string_append_c( &(__http->request.parameters), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_500;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a uri parameter charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_QUERY :
                                        {
                                            if ( (' ' == ph[ i]) || (';' == ph[i]) || ('#' == ph[ i])) {
                                                if ( ' ' == ph[ i]) {
                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_H;

                                                } else if ( ';' == ph[ i]) {
                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_PARAMETERS;

                                                } else { 
                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FRAGMENT;
                                                }

                                            } else if ( '\r' == ph[ i]) {
                                                /** Is HTTP/0.9? check request method it is GET? */
                                                if ( ERIS_HTTP_GET == __http->request.command) {
                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_TITLE_CR;
                                                    
                                                } else {
                                                    rc = EERIS_4XX;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_405;
                                                    __http->response.version = ERIS_HTTP_V09;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 405));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Request version is HTTP/0.9, method is not GET");
                                                    }
                                                }
                                            } else {
                                                /** Store a query charactor */
                                                if ( 0 != eris_string_append_c( &(__http->request.query), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_500;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a uri query charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_FRAGMENT :
                                        {
                                            if ( ' ' == ph[ i]) {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_H;

                                            } else if ( '\r' == ph[ i]) {
                                                /** Is HTTP/0.9? check request method it is GET? */
                                                if ( ERIS_HTTP_GET == __http->request.command) {
                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_TITLE_CR;
                                                    
                                                } else {
                                                    rc = EERIS_4XX;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_405;
                                                    __http->response.version = ERIS_HTTP_V09;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 405));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Request version is HTTP/0.9, method is not GET");
                                                    }
                                                }
                                            } else {
                                                /** Store a query charactor */
                                                if ( 0 != eris_string_append_c( &(__http->request.fragment), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_500;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a uri query charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_H :
                                        {
                                            if ( 'H' == ph[ i]) {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_HT;

                                                eris_string_cleanup( tmp_s);

                                                /** Store a 'H' charactor */
                                                if ( 0 != eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_500;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a version 'H' charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_4XX;

                                                eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                /** Bad request version */
                                                __http->response.status  = ERIS_HTTP_400;
                                                __http->response.version = ERIS_HTTP_V10;
                                                eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad request version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_HT :
                                        {
                                            if ( 'T' == ph[ i]) {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_HTT;

                                                /** Store a 'T' charactor */
                                                if ( 0 != eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_500;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a version 'T' charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_4XX;

                                                eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                /** Bad request version */
                                                __http->response.status  = ERIS_HTTP_400;
                                                __http->response.version = ERIS_HTTP_V10;
                                                eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad request version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_HTT :
                                        {
                                            if ( 'T' == ph[ i]) {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_HTTP;

                                                /** Store a 'T' charactor */
                                                if ( 0 != eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_500;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a version 'T' charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_4XX;

                                                eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                /** Bad request version */
                                                __http->response.status  = ERIS_HTTP_400;
                                                __http->response.version = ERIS_HTTP_V10;
                                                eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad request version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_HTTP :
                                        {
                                            if ( 'P' == ph[ i]) {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_SLASH;

                                                /** Store a 'P' charactor */
                                                if ( 0 != eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_500;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a version 'P' charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_4XX;

                                                eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                /** Bad request version */
                                                __http->response.status  = ERIS_HTTP_400;
                                                __http->response.version = ERIS_HTTP_V10;
                                                eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad request version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_SLASH :
                                        {
                                            if ( '/' == ph[ i]) {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_MAJOR;

                                                if ( 0 != eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_500;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a version '/' charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_4XX;

                                                eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                /** Bad request version */
                                                __http->response.status  = ERIS_HTTP_400;
                                                __http->response.version = ERIS_HTTP_V10;
                                                eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad request version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_MAJOR :
                                        {
                                            if ( isdigit( ph[ i]) ) {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_DOT;

                                                if ( 0 != eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_500;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a major version charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_4XX;

                                                eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                /** Bad request version */
                                                __http->response.status  = ERIS_HTTP_400;
                                                __http->response.version = ERIS_HTTP_V10;
                                                eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad request version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_DOT :
                                        {
                                            if ( '.' == ph[ i]) {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_MINOR;

                                                if ( 0 != eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_500;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a version dot charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_4XX;

                                                eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                /** Bad request version */
                                                __http->response.status  = ERIS_HTTP_400;
                                                __http->response.version = ERIS_HTTP_V10;
                                                eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad request version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_MINOR :
                                        {
                                            if ( isdigit( ph[ i]) ) {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_TITLE_CR;

                                                if ( 0 == eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    if ( eris_string_isequal( tmp_s, ERIS_HTTP_V09_S, false)) {
                                                        __http->request.version = ERIS_HTTP_V09;

                                                    } else if ( eris_string_isequal( tmp_s, ERIS_HTTP_V10_S, false)) {
                                                        __http->request.version = ERIS_HTTP_V10;

                                                    } else if ( eris_string_isequal( tmp_s, ERIS_HTTP_V11_S, false)) {
                                                        __http->request.version = ERIS_HTTP_V11;

                                                    } else {
                                                        __http->request.version = ERIS_HTTP_V00;

                                                        rc = EERIS_5XX;

                                                        eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                        /** Internal server error */
                                                        __http->response.status  = ERIS_HTTP_505;
                                                        __http->response.version = ERIS_HTTP_V10;
                                                        eris_string_set( &(__http->response.reason), eris_http_status_desc( 505));
                                                    }
                                                } else {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_500;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a minor version charactor failed, errno.<%d>", errno);
                                                    }
                                                }

                                                /** Clean up tmp string */
                                                eris_string_cleanup( tmp_s);
                                            } else {
                                                rc = EERIS_4XX;

                                                eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                /** Bad request version */
                                                __http->response.status  = ERIS_HTTP_400;
                                                __http->response.version = ERIS_HTTP_V10;
                                                eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad request version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_TITLE_CR :
                                        {
                                            if ( '\r' == ph[ i] ) {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_TITLE_LF;

                                            } else {
                                                rc = EERIS_4XX;

                                                eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                /** Bad request version */
                                                __http->response.status  = ERIS_HTTP_400;
                                                __http->response.version = ERIS_HTTP_V10;
                                                eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad request title end CR");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_TITLE_LF :
                                        {
                                            if ( '\n' == ph[ i] ) {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_HEADER_NAME;

                                                /** Cleanup name and value string object */
                                                eris_string_cleanup( hname);
                                                eris_string_cleanup( hvalue);

                                            } else {
                                                rc = EERIS_4XX;

                                                eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                /** Bad request version */
                                                __http->response.status  = ERIS_HTTP_400;
                                                __http->response.version = ERIS_HTTP_V10;
                                                eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad request title end LF");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_HEADER_NAME :
                                        {
                                            if ( ':' == ph[ i]) {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_HEADER_COLON;

                                            } else if ( isalnum( ph[ i]) || ('-' == ph[ i]) || ('_' == ph[i]) ) {
                                                /** Is name string? */
                                                if ( 0 != eris_string_append_c( &( hname), ph[ i]) ){
                                                    rc = EERIS_ERROR;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_500;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a header name charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_4XX;

                                                eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                __http->response.status  = ERIS_HTTP_400;
                                                __http->response.version = ERIS_HTTP_V10;
                                                eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad request header name");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_HEADER_COLON :
                                        {
                                            if ( ' ' == ph[ i]) {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_HEADER_VALUE;

                                            } else {
                                                rc = EERIS_4XX;

                                                eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                /** Bad http header */
                                                __http->response.status  = ERIS_HTTP_400;
                                                __http->response.version = ERIS_HTTP_V10;
                                                eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad header name:x-value charactor");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_HEADER_VALUE :
                                        {
                                            if ( '\r' == ph[ i]) {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_HEADER_CR1;

                                            } else {
                                                if ( 0 != eris_string_append_c( &( hvalue), ph[ i]) ){
                                                    rc = EERIS_ERROR;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_500;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a header value charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_HEADER_CR1 :
                                        {
                                            if ( '\n' == ph[ i]) {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_HEADER_LF1;

                                            } else {
                                                rc = EERIS_4XX;

                                                eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                /** Bad http header */
                                                __http->response.status  = ERIS_HTTP_400;
                                                __http->response.version = ERIS_HTTP_V10;
                                                eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad header line end charactor");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_HEADER_LF1 :
                                        {
                                            if ( ' ' == ph[ i]) {
                                                /** New line value */
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_HEADER_VALUE;

                                            } else {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_HEADER_CR2;

                                                /** Set name-value header */
                                                eris_http_hnode_t *new_hnode = (eris_http_hnode_t *)eris_memory_alloc(sizeof( eris_http_hnode_t));
                                                if ( new_hnode) {
                                                    new_hnode->name  = eris_string_clone_eris( hname);
                                                    new_hnode->value = eris_string_clone_eris( hvalue);
                                                    new_hnode->next  = NULL;

                                                    /** Link header */
                                                    if ( new_hnode->name && new_hnode->value) {
                                                        new_hnode->next         = __http->request.headers;
                                                        __http->request.headers = new_hnode;

                                                    } else {
                                                        eris_string_free( new_hnode->name);  new_hnode->name  = NULL;
                                                        eris_string_free( new_hnode->value); new_hnode->value = NULL;

                                                        eris_memory_free( new_hnode); new_hnode = NULL;

                                                        rc = EERIS_ERROR;

                                                        eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                        /** Internal server error */
                                                         __http->response.status  = ERIS_HTTP_500;
                                                         __http->response.version = ERIS_HTTP_V10;
                                                        eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                        if ( __http->log) {
                                                            eris_log_dump( __http->log, ERIS_LOG_CORE, "Set header node name-value failed, errno.<%d>", errno);
                                                        }
                                                    }
                                                } else {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                    /** Internal server error */
                                                    __http->response.status  = ERIS_HTTP_500;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Create header node failed, errno.<%d>", errno);
                                                    }
                                                }

                                                eris_string_cleanup( hname);
                                                eris_string_cleanup( hvalue);

                                                if ( 0 == rc) {
                                                    if ( '\r' == ph[ i]) {
                                                        /** To end of header package */
                                                        eris_http_sreq_v = ERIS_HTTP_SREQ_HEADER_CR2;

                                                    } else {
                                                        /** To end of header package */
                                                        eris_http_sreq_v = ERIS_HTTP_SREQ_HEADER_NAME;

                                                        if ( 0 != eris_string_append_c( &( hname), ph[ i]) ) {
                                                            rc = EERIS_ERROR;

                                                            eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                            /** Internal server error */
                                                            __http->response.status  = ERIS_HTTP_500;
                                                            __http->response.version = ERIS_HTTP_V10;
                                                            eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                            if ( __http->log) {
                                                                eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a header name charactor failed, errno.<%d>", errno);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_HEADER_CR2 :
                                        {
                                            header_finish_flag = true;

                                            if ( '\n' == ph[ i]) {
                                                eris_http_sreq_v = ERIS_HTTP_SREQ_HEADER_LF2;

                                                /** Ok */
                                                if ( (i + 1) < ph_size) {
                                                    rc = eris_buffer_copy_at( &(__http->bbuffer), &(__http->hbuffer), (i + 1), __http->log);
                                                    if ( 0 != rc) {
                                                        rc = EERIS_ERROR;

                                                        eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                        /** Copy error: 500 */
                                                        __http->response.status  = ERIS_HTTP_500;
                                                        __http->response.version = ERIS_HTTP_V10;
                                                        eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                        if ( __http->log) {
                                                            eris_log_dump( __http->log, ERIS_LOG_CORE, "Copy end of body content from header buffer failed, errno.<%d>", errno);
                                                        }
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_4XX;

                                                eris_http_sreq_v = ERIS_HTTP_SREQ_FINISH;

                                                /** Bad request header */
                                                __http->response.status  = ERIS_HTTP_400;
                                                __http->response.version = ERIS_HTTP_V10;
                                                eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad request header end");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SREQ_HEADER_LF2 :
                                    case ERIS_HTTP_SREQ_FINISH :
                                    default :
                                        {
                                            /** Happen error */
                                            header_finish_flag = true;
                                        } break;
                                }/// switch (ph[ i])

                                /** Happen error and end of parser */
                                if ( 0 != rc) { break; }

                            }/// for ( i = 0; i < ph_size; i++)
                        } else { break; /** ph_size is equal 0, Not data or error */ }
                    } else {
                        rc = EERIS_ERROR;

                        __http->response.status  = ERIS_HTTP_408;
                        __http->response.version = ERIS_HTTP_V10;
                        eris_string_set( &(__http->response.reason), eris_http_status_desc( 408));

                        /** Happen error */
                        if ( __http->log) {
                            eris_log_dump( __http->log, ERIS_LOG_CORE, "Get data from callback failed.");
                        }

                        break;
                    }
                } else { break; }

                /** Clean up header buffer */
                eris_buffer_cleanup( &(__http->hbuffer), NULL);

            } while ( 1);
            /** End parser header */

            /** Clean up header buffer */
            eris_buffer_cleanup( &(__http->hbuffer), NULL);

            /** Request header content ok */
            if ( (0 == rc) && (header_finish_flag)) {
                /** Methods: PUT or POST or PATCH has content */
                if ( (ERIS_HTTP_PUT   == __http->request.command) ||
                     (ERIS_HTTP_POST  == __http->request.command) ||
                     (ERIS_HTTP_PATCH == __http->request.command) ) {
                    /** Is chunked transfer encoding */
                    const eris_string_t chunked_value_s = eris_http_request_get_header( __http, "Transfer-Encoding"); 
                       if ( chunked_value_s && 
                         (eris_string_isequal( chunked_value_s, "chunked", true)) 
                       ) {
                        __http->chunked = true;
                    }

                    /** HTTP/1.1 shake hands */
                    const eris_string_t expect_100_continue_s = eris_http_request_get_header( __http, "Expect");
                    if ( expect_100_continue_s &&
                         (eris_string_isequal( chunked_value_s, "100-continue", true)) 
                       ) {
                        /** 100-continue */
                        __http->request.shake = true;

                    }

                    if ( __http->request.shake) {
                        __http->response.status  = 100;
                        __http->response.version = ERIS_HTTP_V11;
                        eris_string_set( &(__http->response.reason), eris_http_status_desc( 100));

                        /** Get content-length */
                        const eris_string_t content_length_s = eris_http_request_get_header( __http, "Content-Length");
                        if ( content_length_s) {
                            eris_int64_t content_length_v = (eris_size_t )eris_string_atol( content_length_s);
                            if ( content_length_v <= __http->attrs.body_max_size) {
                                if ( 0 > content_length_v) {
                                    /** Bad content_length */
                                    rc = EERIS_4XX;

                                    __http->response.status  = ERIS_HTTP_400;
                                    __http->response.version = ERIS_HTTP_V10;
                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));

                                    if ( __http->log) {
                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad request Content-Length");
                                    }
                                }
                            } else {
                                /** Request entity too large */
                                rc = EERIS_4XX;

                                __http->response.status  = ERIS_HTTP_413;
                                __http->response.version = ERIS_HTTP_V10;
                                eris_string_set( &(__http->response.reason), eris_http_status_desc( 413));

                                if ( __http->log) {
                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Request content data too large");
                                }
                            }
                        } else {
                            /** Not found Content-Length */
                            rc = EERIS_4XX;

                            __http->response.status  = ERIS_HTTP_411;
                            __http->response.version = ERIS_HTTP_V10;
                            eris_string_set( &(__http->response.reason), eris_http_status_desc( 411));

                            if ( __http->log) {
                                eris_log_dump( __http->log, ERIS_LOG_CORE, "Unspecify Content-Length header");
                            }
                        }
                    } else {
                        if ( __http->chunked ) {
                            do {
                                if ( 0 == __http->bbuffer.size) {
                                    eris_size_t get_data_size = __http->bbuffer.alloc;

                                    /** Call back get chunked data */
                                    rc = __incb( &(__http->bbuffer), get_data_size, __arg, __http->log);
                                }

                                rc = eris_http_chunk_parse( __http, ERIS_HTTP_REQUEST, &(__http->bbuffer));
                                if ( __http->chunked_end) {

                                    break;
                                }

                                if ( 0 != rc) { 
                                    __http->response.status  = ERIS_HTTP_408;
                                    __http->response.version = ERIS_HTTP_V10;
                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 408));

                                    break; 
                                }

                                /** Clean up body buffer */
                                eris_buffer_cleanup( &(__http->bbuffer), NULL);

                            } while ( 1);

                        } else {
                            /** Copy body content */
                            rc = eris_buffer_copy( &(__http->request.body), &(__http->bbuffer), __http->log);
                            if ( 0 == rc) {
                                /** Get content-length */
                                const eris_string_t content_length_s = eris_http_request_get_header( __http, "Content-Length");
                                if ( content_length_s) {
                                    eris_int64_t content_length_v = (eris_size_t )eris_string_atol( content_length_s);
                                    if ( content_length_v <= __http->attrs.body_max_size) {
                                        if ( 0 <= content_length_v) {
                                            /** Body buffer size is not equal Content-Length */
                                            while ( __http->request.body.size < (eris_size_t ) content_length_v) {
                                                eris_buffer_cleanup( &(__http->bbuffer), __http->log);

                                                eris_size_t get_data_size = ((eris_size_t )content_length_v - __http->request.body.size);
                                                if ( get_data_size > __http->bbuffer.alloc ) {
                                                    get_data_size = __http->bbuffer.alloc;
                                                }

                                                /** Get body content data */
                                                rc = __incb( &(__http->bbuffer), get_data_size, __arg, __http->log);
                                                if ( 0 == rc) {
                                                    /** Append request body data */
                                                    rc = eris_buffer_append_es( &(__http->request.body), &(__http->bbuffer), __http->log);
                                                    if ( 0 != rc) {
                                                        rc = EERIS_ERROR;

                                                        __http->response.status  = ERIS_HTTP_500;
                                                        __http->response.version = ERIS_HTTP_V10;
                                                        eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                                        break;
                                                    }
                                                } else {
                                                    rc = EERIS_4XX;

                                                    /** Heppen error */
                                                    __http->response.status  = ERIS_HTTP_408;
                                                    __http->response.version = ERIS_HTTP_V10;
                                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 408));

                                                    break;
                                                }/// else callback error.
                                            }/// while ( __http->request.body.size < content_length_v)
                                        } else {
                                            /** Bad content_length */
                                            rc = EERIS_4XX;

                                            __http->response.status  = ERIS_HTTP_400;
                                            __http->response.version = ERIS_HTTP_V10;
                                            eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));

                                            if ( __http->log) {
                                                eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad request Content-Length");
                                            }
                                        }
                                    } else {
                                        /** Request entity too large */
                                        rc = EERIS_4XX;

                                        __http->response.status  = ERIS_HTTP_413;
                                        __http->response.version = ERIS_HTTP_V10;
                                        eris_string_set( &(__http->response.reason), eris_http_status_desc( 413));

                                        if ( __http->log) {
                                            eris_log_dump( __http->log, ERIS_LOG_CORE, "Request content data too large");
                                        }
                                    }
                                } else {
                                    /** Not found Content-Length */
                                    rc = EERIS_4XX;

                                    __http->response.status  = ERIS_HTTP_411;
                                    __http->response.version = ERIS_HTTP_V10;
                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 411));

                                    if ( __http->log) {
                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Unspecify Content-Length header");
                                    }
                                }
                            } else {
                                rc = EERIS_ERROR;

                                __http->response.status  = ERIS_HTTP_500;
                                __http->response.version = ERIS_HTTP_V10;
                                eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));

                                if ( __http->log) {
                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Copy buffer failed, errno.<%d>", errno);
                                }
                            }
                        }/// else is not chunked data
                    }/// else not 100-continue
                }/// fi command is PUT or POST or PATCH
            }/// fi ( (0 == rc) && header_finish_flag )

            eris_string_free( tmp_s);  tmp_s  = NULL;
            eris_string_free( hname);  hname  = NULL;
            eris_string_free( hvalue); hvalue = NULL;

        } else {
            rc = EERIS_ERROR;

            if ( __http->log) {
                eris_log_dump( __http->log, ERIS_LOG_CORE, "Unspecify callback, please set it.");
            }
        }

        eris_buffer_cleanup( &(__http->hbuffer), NULL);
        eris_buffer_cleanup( &(__http->bbuffer), NULL);
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_http_request_parse


/**
 * @Brief: The client request want to shake hands.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: yes is true, other is false.
 **/
eris_bool_t eris_http_request_shake( eris_http_t *__http)
{
    if ( __http) {
        return __http->request.shake;
    }

    return false;
}/// eris_http_request_shake


/**
 * @Brief: Write http-request data package to callback output.
 *
 * @Param: __http, Eris http context.
 * @Param: __outcb,Write http-request data to output callback.
 * @Param: __arg,  Call callback and input argument.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_http_request_pack( eris_http_t *__http, eris_http_cb_t __outcb, eris_arg_t __arg)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __http) {
        if ( __outcb) {
            /** Clean up header/body buffer */
            eris_buffer_cleanup( &(__http->hbuffer), NULL);

            if ( (ERIS_HTTP_UNKNOW >= __http->request.command) || 
                 (eris_http_command_arrs_size <= __http->request.command)) {
                __http->request.command = ERIS_HTTP_GET;
            }

            rc = eris_buffer_append( &(__http->hbuffer), 
                                     eris_http_command_arrs[ __http->request.command], 
                                     strlen( eris_http_command_arrs[ __http->request.command]), 
                                     __http->log
                                   );
            /** Append command ok */
            if ( 0 == rc) {
                /** Append a empty Separator */
                rc = eris_buffer_append_c( &(__http->hbuffer), ' ', __http->log);
            }
            
            /** Append request url */
            if ( 0 == rc) {
                rc = eris_buffer_append( &(__http->hbuffer), 
                                         __http->request.url, 
                                         eris_string_size( __http->request.url), 
                                         __http->log
                                       );
            }

            /** Append request parameters? */
            if ( (0 == rc) && ( 0 < eris_string_size( __http->request.parameters)) ) {
                rc = eris_buffer_append_c( &(__http->hbuffer), ';', __http->log);
                if ( 0 == rc) {
                    rc = eris_buffer_append( &(__http->hbuffer), 
                                             __http->request.parameters, 
                                             eris_string_size( __http->request.parameters), 
                                             __http->log
                                           );
                }
            }

            /** Append request query? */
            if ( (0 == rc) && ( 0 < eris_string_size( __http->request.query)) ) {
                rc = eris_buffer_append_c( &(__http->hbuffer), '?', __http->log);
                if ( 0 == rc) {
                    rc = eris_buffer_append( &(__http->hbuffer), 
                                             __http->request.query, 
                                             eris_string_size( __http->request.query), 
                                             __http->log
                                           );
                }
            }

            /** Append request fragment? */
            if ( (0 == rc) && ( 0 < eris_string_size( __http->request.fragment)) ) {
                rc = eris_buffer_append_c( &(__http->hbuffer), '#', __http->log);
                if ( 0 == rc) {
                    rc = eris_buffer_append( &(__http->hbuffer), 
                                             __http->request.fragment, 
                                             eris_string_size( __http->request.fragment), 
                                             __http->log
                                           );
                }
            }

            if ( 0 == rc) {
                /** Append a empty Separator */
                rc = eris_buffer_append_c( &(__http->hbuffer), ' ', __http->log);
            }
            
            /** Append request version */
            if ( 0 == rc) {
                switch ( __http->request.version) {
                    case ERIS_HTTP_V11 :
                        {
                            /** HTTP/1.1 */
                            rc = eris_buffer_append( &(__http->hbuffer), eris_http_version_tos(11), ERIS_HTTP_VERSION_SIZE, __http->log);
                        } break;
                    case ERIS_HTTP_V09 :
                        {
                            /** HTTP/0.9 */
                            rc = eris_buffer_append( &(__http->hbuffer), eris_http_version_tos(09), ERIS_HTTP_VERSION_SIZE, __http->log);
                        } break;
                    case ERIS_HTTP_V10 :
                    default :
                        {
                            /** Default HTTP/1.0 */
                            __http->request.version = ERIS_HTTP_V10;

                            rc = eris_buffer_append( &(__http->hbuffer), eris_http_version_tos(10), ERIS_HTTP_VERSION_SIZE, __http->log);
                        } break;
                }/// switch
            }

            /** Append CRLF end */
            if ( 0 == rc) {
                rc = eris_buffer_append( &(__http->hbuffer), ERIS_HTTP_CRLF, ERIS_HTTP_CRLF_SIZE, __http->log);
            }

            /** Append all headers */
            if ( 0 == rc) {
                eris_http_hnode_t *cur_hnode = __http->request.headers;

                for ( ; cur_hnode; cur_hnode = cur_hnode->next) {
                    /** Append name */
                    rc = eris_buffer_append( &(__http->hbuffer), cur_hnode->name, eris_string_size( cur_hnode->name), __http->log);
                    if ( 0 == rc) {
                        rc = eris_buffer_append( &(__http->hbuffer), ": ", 2, __http->log);
                        if ( 0 == rc) {
                            /** Append value */
                            rc = eris_buffer_append( &(__http->hbuffer), cur_hnode->value, eris_string_size( cur_hnode->value), __http->log);
                            if ( 0 == rc) {
                                rc = eris_buffer_append( &(__http->hbuffer), ERIS_HTTP_CRLF, ERIS_HTTP_CRLF_SIZE, __http->log);
                            }
                        }
                    }

                    /** Heppen error */
                    if ( 0 != rc) { break; }
                }

                cur_hnode = NULL;
            }

            /** Append CRLF_CRLF header end */
            if ( 0 == rc) {
                rc = eris_buffer_append( &(__http->hbuffer), ERIS_HTTP_CRLF, ERIS_HTTP_CRLF_SIZE, __http->log);
            }

            /** Call back output request content package */
            if ( 0 == rc) {
                (eris_void_t )__outcb( &(__http->hbuffer), __http->hbuffer.size, __arg, __http->log);

                if ( (ERIS_HTTP_PUT   == __http->request.command) || 
                     (ERIS_HTTP_POST  == __http->request.command) ||
                     (ERIS_HTTP_PATCH == __http->request.command) ) {
                    /** PUT, POST, PATCH has body */
                    (eris_void_t )__outcb( &(__http->request.body), __http->request.body.size, __arg, __http->log);
                }
            }
        } else {
            rc = EERIS_ERROR;

            if ( __http->log) {
                eris_log_dump( __http->log, ERIS_LOG_CORE, "Unspecify callback, please set it.");
            }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_http_request_pack


/**
 * @Brief: Read http-response data package from callback input.
 *
 * @Param: __http, Eris http context.
 * @Param: __incb, Read http-response data from input callback.
 * @Param: __arg,  Call callback and input argument.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR or EERIS_DATA.
 **/
eris_int_t eris_http_response_parse( eris_http_t *__http, eris_http_cb_t __incb, eris_arg_t __arg)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __http) {
        /** Clean up http response context */
        eris_http_cleanup_response( __http);
        eris_buffer_cleanup( &(__http->hbuffer), NULL);
        eris_buffer_cleanup( &(__http->bbuffer), NULL);

        __http->chunk_state    = ERIS_HTTP_CHUNKED;
        __http->chunked        = false;
        __http->chunked_end    = false;
        __http->chunk_size     = 0;
        __http->tmp_chunk_size = 0;

        if ( __incb) {
            eris_bool_t header_finish_flag = false;

            /** HTTP response protocol state */
            enum {
                ERIS_HTTP_SRESP_H           = 0,
                ERIS_HTTP_SRESP_HT          ,
                ERIS_HTTP_SRESP_HTT         ,
                ERIS_HTTP_SRESP_HTTP        ,
                ERIS_HTTP_SRESP_SLASH       ,
                ERIS_HTTP_SRESP_MAJOR       ,
                ERIS_HTTP_SRESP_DOT         ,
                ERIS_HTTP_SRESP_MINOR       ,
                ERIS_HTTP_SRESP_SEP1        ,
                ERIS_HTTP_SRESP_STATUS_1    ,
                ERIS_HTTP_SRESP_STATUS_2    ,
                ERIS_HTTP_SRESP_STATUS_3    ,
                ERIS_HTTP_SRESP_SEP2        ,
                ERIS_HTTP_SRESP_REASON      ,
                ERIS_HTTP_SRESP_TITLE_CR    ,
                ERIS_HTTP_SRESP_TITLE_LF    ,
                ERIS_HTTP_SRESP_HEADER_NAME ,
                ERIS_HTTP_SRESP_HEADER_COLON,
                ERIS_HTTP_SRESP_HEADER_VALUE,
                ERIS_HTTP_SRESP_HEADER_CR1  ,
                ERIS_HTTP_SRESP_HEADER_LF1  ,
                ERIS_HTTP_SRESP_HEADER_CR2  ,
                ERIS_HTTP_SRESP_HEADER_LF2  ,
                ERIS_HTTP_SRESP_FINISH      ,
            } eris_http_sresp_v = ERIS_HTTP_SRESP_H;

            /** Do parse header */
            eris_string_t tmp_s  = eris_string_alloc( 32);
            eris_string_t hname  = eris_string_alloc( 32);
            eris_string_t hvalue = eris_string_alloc( 128);

            /** Do parse */
            do {
                if ( !header_finish_flag && (0 == rc)) {
                    /** Get response content from input callback */
                    rc = __incb( &(__http->hbuffer), __http->attrs.header_cache_size, __arg, __http->log);
                    if ( EERIS_ERROR != rc) {
                        eris_size_t ph_size   = __http->hbuffer.size;
                        const eris_char_t *ph = (const eris_char_t *)__http->hbuffer.data;

                        /** Has data and parse */
                        if ( 0 < ph_size) {
                            eris_size_t i = 0;

                            for ( i = 0; i < ph_size; i++) {
                                if ( header_finish_flag) {
                                    break;
                                }

                                switch ( eris_http_sresp_v) {
                                    case ERIS_HTTP_SRESP_H :
                                        {
                                            if ( 'H' == ph[ i]) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_HT;

                                                eris_string_cleanup( tmp_s);

                                                /** Store a 'H' charactor */
                                                if ( 0 != eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a version 'H' charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad repsonse version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_HT :
                                        {
                                            if ( 'T' == ph[ i]) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_HTT;

                                                /** Store a 'T' charactor */
                                                if ( 0 != eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a version 'T' charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad response version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_HTT :
                                        {
                                            if ( 'T' == ph[ i]) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_HTTP;

                                                /** Store a 'T' charactor */
                                                if ( 0 != eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a version 'T' charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad response version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_HTTP :
                                        {
                                            if ( 'P' == ph[ i]) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_SLASH;

                                                /** Store a 'P' charactor */
                                                if ( 0 != eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a version 'P' charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad response version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_SLASH :
                                        {
                                            if ( '/' == ph[ i]) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_MAJOR;

                                                if ( 0 != eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a version '/' charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad response version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_MAJOR :
                                        {
                                            if ( isdigit( ph[ i]) ) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_DOT;

                                                if ( 0 != eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a major version charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad response version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_DOT :
                                        {
                                            if ( '.' == ph[ i]) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_MINOR;

                                                if ( 0 != eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a version dot charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad request version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_MINOR :
                                        {
                                            if ( isdigit( ph[ i]) ) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_SEP1;

                                                if ( 0 == eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    if ( eris_string_isequal( tmp_s, ERIS_HTTP_V09_S, false)) {
                                                        __http->response.version = ERIS_HTTP_V09;

                                                    } else if ( eris_string_isequal( tmp_s, ERIS_HTTP_V10_S, false)) {
                                                        __http->response.version = ERIS_HTTP_V10;

                                                    } else if ( eris_string_isequal( tmp_s, ERIS_HTTP_V11_S, false)) {
                                                        __http->response.version = ERIS_HTTP_V11;

                                                    } else {
                                                        rc = EERIS_DATA;

                                                        eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;
                                                    }
                                                } else {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a minor version charactor failed, errno.<%d>", errno);
                                                    }
                                                }

                                                /** Clean up tmp string */
                                                eris_string_cleanup( tmp_s);
                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad response version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_SEP1 :
                                        {
                                            if ( ' ' == ph[ i]) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_STATUS_1;

                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad response version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_STATUS_1 :
                                        {
                                            if ( isdigit( ph[ i])) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_STATUS_2;

                                                if ( 0 != eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a status charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad response version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_STATUS_2 :
                                        {
                                            if ( isdigit( ph[ i])) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_STATUS_3;

                                                if ( 0 != eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a status charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad response version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_STATUS_3 :
                                        {
                                            if ( isdigit( ph[ i])) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_SEP2;

                                                if ( 0 == eris_string_append_c( &( tmp_s), ph[ i])) {
                                                    eris_size_t k = 0;

                                                    for ( k = 1; k < eris_http_status_map_reason_size; k++) {
                                                        if ( eris_string_isequal( tmp_s, eris_http_status_map_reason[ k].status_s, false)) {
                                                            __http->response.status = eris_http_status_map_reason[ k].status;

                                                            break;
                                                        }
                                                    }

                                                    if ( k >= eris_http_status_map_reason_size) {
                                                        rc = EERIS_DATA;

                                                        eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                        if ( __http->log) {
                                                            eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad response version");
                                                        }
                                                    }
                                                } else {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a status charactor failed, errno.<%d>", errno);
                                                    }
                                                }

                                                /** Clean up tmp string cache */
                                                eris_string_cleanup( tmp_s);
                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad response version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_SEP2 :
                                        {
                                            if ( ' ' == ph[ i]) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_REASON;

                                                eris_string_cleanup( __http->response.reason);
                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad response version");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_REASON :
                                        {
                                            if ( '\r' == ph[ i]) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_TITLE_CR;

                                            } else {
                                                rc = eris_string_append_c( &(__http->response.reason), ph[ i]);
                                                if ( 0 != rc) {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a response reason phrace char failed, errno.<%d>", errno);
                                                    }
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_TITLE_CR :
                                        {
                                            if ( '\n' == ph[ i] ) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_TITLE_LF;

                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad response title end CR");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_TITLE_LF :
                                        {
                                            eris_string_cleanup( hname);
                                            eris_string_cleanup( hvalue);

                                            if ( isalnum( ph[ i]) || ('-' == ph[ i]) || ('_' == ph[i]) ) {
                                                /** Is name string? */
                                                if ( 0 != eris_string_append_c( &( hname), ph[ i]) ){
                                                    rc = EERIS_ERROR;

                                                    eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a header name charactor failed, errno.<%d>", errno);
                                                    }
                                                }

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_HEADER_NAME;
                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad response header name");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_HEADER_NAME :
                                        {
                                            if ( ':' == ph[ i]) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_HEADER_COLON;

                                            } else if ( isalnum( ph[ i]) || ('-' == ph[ i]) || ('_' == ph[i]) ) {
                                                /** Is name string? */
                                                if ( 0 != eris_string_append_c( &( hname), ph[ i]) ){
                                                    rc = EERIS_ERROR;

                                                    eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a header name charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad response header name");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_HEADER_COLON :
                                        {
                                            if ( ' ' == ph[ i]) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_HEADER_VALUE;

                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad header name:x-value charactor");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_HEADER_VALUE :
                                        {
                                            if ( '\r' == ph[ i]) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_HEADER_CR1;

                                            } else {
                                                if ( 0 != eris_string_append_c( &( hvalue), ph[ i]) ){
                                                    rc = EERIS_ERROR;

                                                    eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a header value charactor failed, errno.<%d>", errno);
                                                    }
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_HEADER_CR1 :
                                        {
                                            if ( '\n' == ph[ i]) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_HEADER_LF1;

                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad header line end charactor");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_HEADER_LF1 :
                                        {
                                            if ( ' ' == ph[ i]) {
                                                /** New line value */
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_HEADER_VALUE;

                                            } else {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_HEADER_CR2;

                                                /** Set name-value header */
                                                eris_http_hnode_t *new_hnode = (eris_http_hnode_t *)eris_memory_alloc( sizeof( eris_http_hnode_t));
                                                if ( new_hnode) {
                                                    new_hnode->name  = eris_string_clone_eris( hname);
                                                    new_hnode->value = eris_string_clone_eris( hvalue);
                                                    new_hnode->next  = NULL;

                                                    /** Link header */
                                                    if ( new_hnode->name && new_hnode->value) {
                                                        new_hnode->next         = __http->response.headers;
                                                        __http->response.headers = new_hnode;

                                                    } else {
                                                        eris_string_free( new_hnode->name);  new_hnode->name = NULL;
                                                        eris_string_free( new_hnode->value); new_hnode->next = NULL;

                                                        eris_memory_free( new_hnode); new_hnode = NULL;

                                                        rc = EERIS_ERROR;

                                                        eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                        if ( __http->log) {
                                                            eris_log_dump( __http->log, ERIS_LOG_CORE, "Set header node name-value failed, errno.<%d>", errno);
                                                        }
                                                    }
                                                } else {
                                                    rc = EERIS_ERROR;

                                                    eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                    if ( __http->log) {
                                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Create header node failed, errno.<%d>", errno);
                                                    }
                                                }

                                                eris_string_cleanup( hname);
                                                eris_string_cleanup( hvalue);

                                                if ( 0 == rc) {
                                                    if ( '\r' == ph[ i]) {
                                                        /** To end of header package */
                                                        eris_http_sresp_v = ERIS_HTTP_SRESP_HEADER_CR2;

                                                    } else {
                                                        /** To end of header package */
                                                        eris_http_sresp_v = ERIS_HTTP_SRESP_HEADER_NAME;

                                                        if ( 0 != eris_string_append_c( &( hname), ph[ i]) ) {
                                                            rc = EERIS_ERROR;

                                                            eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                            if ( __http->log) {
                                                                eris_log_dump( __http->log, ERIS_LOG_CORE, "Store a header name charactor failed, errno.<%d>", errno);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_HEADER_CR2 :
                                        {
                                            if ( '\n' == ph[ i]) {
                                                eris_http_sresp_v = ERIS_HTTP_SRESP_HEADER_LF2;

                                                /** Ok */
                                                if ( (i + 1) < ph_size) {
                                                    rc = eris_buffer_copy_at( &(__http->bbuffer), &(__http->hbuffer), (i + 1), __http->log);
                                                    if ( 0 != rc) {
                                                        rc = EERIS_ERROR;

                                                        eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                        if ( __http->log) {
                                                            eris_log_dump( __http->log, ERIS_LOG_CORE, "Copy end of body content from header buffer failed, errno.<%d>", errno);
                                                        }
                                                    }
                                                }

                                                header_finish_flag = true;
                                            } else {
                                                rc = EERIS_DATA;

                                                eris_http_sresp_v = ERIS_HTTP_SRESP_FINISH;

                                                if ( __http->log) {
                                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad response header end");
                                                }
                                            }
                                        } break;
                                    case ERIS_HTTP_SRESP_HEADER_LF2 :
                                    case ERIS_HTTP_SRESP_FINISH :
                                    default :
                                        {
                                            header_finish_flag = true;
                                        } break;
                                }/// witch( eris_http_sresp_v)


                                if ( 0 != rc) { break; }
                            }/// for ( i = 0; i <= ph_size; i++);
                        } else { break; /** Not response data from callback */ }
                    } else {
                        /** Happen error */
                        if ( __http->log) {
                            eris_log_dump( __http->log, ERIS_LOG_CORE, "Get data from callback failed.");
                        }

                        break;
                    }
                } else { break; }

                /** Clean up header buffer */
                eris_buffer_cleanup( &(__http->hbuffer), NULL);

            } while ( 1); /// Do parer header and data from callback

            /** Clean up header buffer */
            eris_buffer_cleanup( &(__http->hbuffer), NULL);

            if ( (0 == rc) && (header_finish_flag)) {
                /** Is chunked transfer encoding of response */
                const eris_string_t chunked_value_s = eris_http_response_get_header( __http, "Transfer-Encoding"); 
                if ( chunked_value_s && 
                     (eris_string_isequal( chunked_value_s, "chunked", true)) 
                   ) {
                    __http->chunked = true;
                }

                if ( __http->chunked) {
                    do {
                        if ( 0 == __http->bbuffer.size) {
                            eris_size_t get_data_size = __http->bbuffer.alloc;

                            /** Call back get chunked data */
                            rc = __incb( &(__http->bbuffer), get_data_size, __arg, __http->log);
                        }

                        rc = eris_http_chunk_parse( __http, ERIS_HTTP_RESPONSE, &(__http->bbuffer));
                        if ( __http->chunked_end) {
                            /** Get chunked data end */

                            break;
                        }

                        /** Happen error and break */
                        if ( 0 != rc) { 
                            eris_buffer_cleanup( &(__http->response.body), NULL);

                            break; 
                        }

                        /** Clean up body buffer */
                        eris_buffer_cleanup( &(__http->bbuffer), NULL);

                    } while ( 1);

                } else {
                    const eris_string_t content_length_s = eris_http_request_get_header( __http, "Content-Length");
                    if ( content_length_s) {
                        /** Response data length */
                        eris_int64_t content_length_v = (eris_size_t )eris_string_atol( content_length_s);
                        if ( 0 < content_length_v) {
                            rc = eris_buffer_copy( &(__http->response.body), &(__http->bbuffer), __http->log);
                            if ( 0 == rc) {
                                /** Body buffer size is not equal Content-Length */
                                while ( __http->response.body.size < (eris_size_t )content_length_v) {
                                    eris_buffer_cleanup( &(__http->bbuffer), __http->log);

                                    eris_size_t get_data_size = ((eris_size_t )content_length_v - __http->response.body.size);
                                    if ( get_data_size > __http->bbuffer.alloc ) {
                                        get_data_size = __http->bbuffer.alloc;
                                    }

                                    /** Get body content data */
                                    rc = __incb( &(__http->bbuffer), get_data_size, __arg, __http->log);
                                    if ( 0 == rc) {
                                        /** Append response body data */
                                        rc = eris_buffer_append_es( &(__http->response.body), &(__http->bbuffer), __http->log);
                                        if ( 0 != rc) {
                                            /** Happen error and get out */
                                            rc = EERIS_ERROR;

                                            eris_buffer_cleanup( &(__http->response.body), NULL);

                                            if ( __http->log) {
                                                eris_log_dump( __http->log, ERIS_LOG_CORE, "Store response data from upstream failed, errno.<%d>", errno);
                                            }

                                            break;
                                        }
                                    } else {
                                        rc = EERIS_DATA;

                                        /** Heppen error */
                                        eris_buffer_cleanup( &(__http->response.body), NULL);

                                        if ( __http->log) {
                                            eris_log_dump( __http->log, ERIS_LOG_CORE, "Upstream service unavailable response");
                                        }

                                        break;
                                    }/// else callback error.
                                }/// while ( __http->request.body.size < content_length_v)
                            } else {
                                rc = EERIS_ERROR;

                                eris_buffer_cleanup( &(__http->response.body), NULL);

                                if ( __http->log) {
                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Store response data from upstream failed, errno.<%d>", errno);
                                }
                            }
                        }/// else nothing
                    }/// else nothing
                }/// else set Content-Length?
            }/// else happen error

            eris_string_free( tmp_s);  tmp_s  = NULL;
            eris_string_free( hname);  hname  = NULL;
            eris_string_free( hvalue); hvalue = NULL;

        } else {
            rc = EERIS_ERROR;

            if ( __http->log) {
                eris_log_dump( __http->log, ERIS_LOG_CORE, "Unspecify callback, please set it.");
            }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_http_response_parse


/**
 * @Brief: Write http-response data package to callback output.
 *
 * @Param: __http, Eris http context.
 * @Param: __outcb,Write http-response data to output callback.
 * @Param: __arg,  Call callback and input argument.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_http_response_pack( eris_http_t *__http, eris_http_cb_t __outcb, eris_arg_t __arg)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __http) {
        if ( __outcb) {
            /** Clean up header/body buffer */
            eris_buffer_cleanup( &(__http->hbuffer), NULL);

            /** Append request version */
            switch ( __http->response.version) {
                case ERIS_HTTP_V11 :
                    {
                        /** HTTP/1.1 */
                        rc = eris_buffer_append( &(__http->hbuffer), eris_http_version_tos(11), ERIS_HTTP_VERSION_SIZE, __http->log);
                    } break;
                case ERIS_HTTP_V09 :
                    {
                        /** HTTP/0.9 */
                        rc = eris_buffer_append( &(__http->hbuffer), eris_http_version_tos(09), ERIS_HTTP_VERSION_SIZE, __http->log);
                    } break;
                case ERIS_HTTP_V10 :
                default :
                    {
                        /** Default HTTP/1.0 */
                        __http->request.version = ERIS_HTTP_V10;

                        rc = eris_buffer_append( &(__http->hbuffer), eris_http_version_tos(10), ERIS_HTTP_VERSION_SIZE, __http->log);
                    } break;
            }/// switch

            if ( 0 == rc) {
                /** Append a empty Separator */
                rc = eris_buffer_append_c( &(__http->hbuffer), ' ', __http->log);
            }
            
            /** Append status and reason phrace */
            if ( 0 == rc) {
                if ( 0 < eris_string_size( __http->response.reason)) {
                    eris_string_t tmp_status_reason_es;
                    eris_string_init( tmp_status_reason_es);

                    rc = eris_string_printf( &tmp_status_reason_es, "%i %S", __http->response.status, __http->response.reason);
                    if ( 0 == rc) {
                        rc = eris_buffer_append( &(__http->hbuffer), tmp_status_reason_es, eris_string_size( tmp_status_reason_es), __http->log);
                        
                    } else {
                        rc = EERIS_ERROR;

                        if ( __http->log) {
                               eris_log_dump( __http->log, ERIS_LOG_CORE, "Make status and reason phrace failed, errno.<%d>", errno);
                        }
                    }

                    eris_string_free( tmp_status_reason_es);
                    eris_string_init( tmp_status_reason_es);
                } else {
                    eris_int_t i = 0;

                    for ( i = 1; i < eris_http_status_map_reason_size; i++) {
                        if ( eris_http_status_map_reason[i].status == __http->response.status) {
                            break;
                        }
                    }

                    if ( i < eris_http_status_map_reason_size) {
                        rc = eris_buffer_append( &(__http->hbuffer), eris_http_status_map_reason[i].status_s, 3, __http->log);
                        if ( 0 == rc) {
                            /** Append a empty Separator */
                            rc = eris_buffer_append_c( &(__http->hbuffer), ' ', __http->log);
                        }

                        if ( 0 == rc) {
                            rc = eris_buffer_append( &(__http->hbuffer), 
                                                     eris_http_status_map_reason[i].reason_phrace, 
                                                     strlen( eris_http_status_map_reason[i].reason_phrace),
                                                     __http->log);
                        }
                    } else {
                        rc = EERIS_ERROR;

                        if ( __http->log) {
                               eris_log_dump( __http->log, ERIS_LOG_CORE, "Status value is invalid.");
                        }
                    }
                }
            }

            /** Append CRLF end */
            if ( 0 == rc) {
                rc = eris_buffer_append( &(__http->hbuffer), ERIS_HTTP_CRLF, ERIS_HTTP_CRLF_SIZE, __http->log);
            }

            /** Append all headers */
            if ( 0 == rc) {
                eris_http_hnode_t *cur_hnode = __http->response.headers;

                for ( ;cur_hnode; cur_hnode = cur_hnode->next) {
                    /** Append name */
                    rc = eris_buffer_append( &(__http->hbuffer), cur_hnode->name, eris_string_size( cur_hnode->name), __http->log);
                    if ( 0 == rc) {
                        rc = eris_buffer_append( &(__http->hbuffer), ": ", 2, __http->log);
                        if ( 0 == rc) {
                            /** Append value */
                            rc = eris_buffer_append( &(__http->hbuffer), cur_hnode->value, eris_string_size( cur_hnode->value), __http->log);
                            if ( 0 == rc) {
                                rc = eris_buffer_append( &(__http->hbuffer), ERIS_HTTP_CRLF, ERIS_HTTP_CRLF_SIZE, __http->log);
                            }
                        }
                    }

                    /** Heppen error */
                    if ( 0 != rc) { break; }
                }

                cur_hnode = NULL;
            }

            /** Append CRLF_CRLF header end */
            if ( 0 == rc) {
                rc = eris_buffer_append( &(__http->hbuffer), ERIS_HTTP_CRLF, ERIS_HTTP_CRLF_SIZE, __http->log);
            }

            /** Call back output request content package */
            if ( 0 == rc) {
                rc = __outcb( &(__http->hbuffer), __http->hbuffer.size, __arg, __http->log);
                if ( 0 == rc) {
                   if (ERIS_HTTP_HEAD != __http->request.command ) {
                       if ( 0 < __http->response.body.size) {
                            rc = __outcb( &(__http->response.body), __http->response.body.size, __arg, __http->log);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                eris_log_dump( __http->log, ERIS_LOG_CORE, "Do output body failed, errno.<%d>", errno);
                            }
                       }
                   }
                } else {
                    rc = EERIS_ERROR;

                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Do output header failed, errno.<%d>", errno);
                }
            } else { rc = EERIS_ERROR; }
        } else {
            rc = EERIS_ERROR;

            if ( __http->log) {
                eris_log_dump( __http->log, ERIS_LOG_CORE, "Unspecify callback, please set it.");
            }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_http_response_pack


/**
 * @Brief: The client request connection is keep alive.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Keep-alive is true, other is false.
 **/
eris_bool_t eris_http_request_keep_alive( eris_http_t *__http)
{
    eris_bool_t rc = false;

    if ( __http) {
        const eris_string_t proxy_connection_es = eris_http_request_get_header( __http, "Proxy-Connection");
        if ( proxy_connection_es) {
            /** Proxy-Connecton is keep-alive */
            if ( eris_string_isequal( proxy_connection_es, "keep-alive", true)) {

                rc = true;
            }
        } else {
            const eris_string_t connection_es = eris_http_request_get_header( __http, "Connection");
            if ( connection_es) {
                /** Connection: keep-alive */
                if ( eris_string_isequal( connection_es, "keep-alive", true)) {

                    rc = true;
                }
            }
        }
    }

    return rc;
}/// eris_http_request_keep_alive


/**
 * @Brief: The response connection is keep alive.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Keep-alive is true, other is false.
 **/
eris_bool_t eris_http_response_keep_alive( eris_http_t *__http)
{
    eris_bool_t rc = false;

    if ( __http) {
        const eris_string_t proxy_connection_es = eris_http_response_get_header( __http, "Proxy-Connection");
        if ( proxy_connection_es) {
            /** Proxy-Connecton is keep-alive */
            if ( eris_string_isequal( proxy_connection_es, "keep-alive", true)) {

                rc = true;
            }
        } else {
            const eris_string_t connection_es = eris_http_response_get_header( __http, "Connection");
            if ( connection_es) {
                /** Connection: keep-alive */
                if ( eris_string_isequal( connection_es, "keep-alive", true)) {

                    rc = true;
                }
            }
        }
    }

    return rc;
}/// eris_http_response_keep_alive


/**
 * @Brief: Check request entity content is x-www-form-urlencoded format.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Yes is true, other is false.
 **/
eris_bool_t eris_http_request_is_x_www_form_urlencoded( eris_http_t *__http)
{
    eris_bool_t rc = false;

    if ( __http) {
        const eris_string_t content_type_vs = eris_http_request_get_header( __http, "Content-Type");
        if ( content_type_vs) {
            /** Content-Type: application/x-www-form-urlencoded */
            if ( ERIS_STRING_NPOS != eris_string_find( content_type_vs, "application/x-www-form-urlencoded")) {

                rc = true;
            }
        }
    }

    return rc;
}/// eris_http_request_is_x_www_form_urlencoded


/**
 * @Brief: Check response entity content is x-www-form-urlencoded format.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Yes is true, other is false.
 **/
eris_bool_t eris_http_response_is_x_www_form_urlencoded( eris_http_t *__http)
{
    eris_bool_t rc = false;

    if ( __http) {
        const eris_string_t content_type_vs = eris_http_response_get_header( __http, "Content-Type");
        if ( content_type_vs) {
            /** Content-Type: application/x-www-form-urlencoded */
            if ( ERIS_STRING_NPOS != eris_string_find( content_type_vs, "application/x-www-form-urlencoded")) {

                rc = true;
            }
        }
    }

    return rc;
}/// eris_http_response_is_x_www_form_urlencoded


/**
 * @Brief: Check request entity content is json format.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Yes is true, other is false.
 **/
eris_bool_t eris_http_request_is_json( eris_http_t *__http)
{
    eris_bool_t rc = false;

    if ( __http) {
        const eris_string_t content_type_vs = eris_http_request_get_header( __http, "Content-Type");
        if ( content_type_vs) {
            /** Content-Type: application/json */
            if ( ERIS_STRING_NPOS != eris_string_find( content_type_vs, "application/json")) {

                rc = true;
            }
        }
    }

    return rc;
}/// eris_http_request_is_json


/**
 * @Brief: Check response entity content is json format.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Yes is true, other is false.
 **/
eris_bool_t eris_http_response_is_json( eris_http_t *__http)
{
    eris_bool_t rc = false;

    if ( __http) {
        const eris_string_t content_type_vs = eris_http_response_get_header( __http, "Content-Type");
        if ( content_type_vs) {
            /** Content-Type: application/json */
            if ( ERIS_STRING_NPOS != eris_string_find( content_type_vs, "application/json")) {

                rc = true;
            }
        }
    }

    return rc;
}/// eris_http_response_is_json


/**
 * @Brief: Check http context is chunked.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: It is has chunked data is true, Other is false.
 **/
eris_bool_t eris_http_is_chunked( eris_http_t *__http)
{
    if ( __http) {
        return __http->chunked;
    }

    return false;
}/// eris_http_is_chunked


/**
 * @Brief: Check http context parse is chunked ok.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: It is parse chunked data end is true, Other is false.
 **/
eris_bool_t eris_http_is_chunked_end( eris_http_t *__http)
{
    if ( __http) {
        return __http->chunked_end;
    }

    return false;
}/// eris_http_is_chunked_end


/**
 * @Brief: Parse chunk data.
 *
 * @Param: __http,     Eris http context.
 * @Param: __type,     ERIS_HTTP_REQUEST or ERIS_HTTP_RESPONSE.
 * @Param: __inbuffer, Input data buffer.
 *
 * @Return: Ok is 0, Internal error is EERIS_ERROR or EERIS_DATA.
 **/
eris_int_t eris_http_chunk_parse( eris_http_t *__http, eris_http_type_t __type, const eris_buffer_t *__inbuffer)
{
    eris_int_t rc = 0;

    if ( __http && __inbuffer) {
        if ( 0 < __inbuffer->size) {
            eris_string_t chunk_size_s = NULL;
            const eris_char_t *pb = (const eris_char_t *)(__inbuffer->data);

            eris_size_t i = 0;
            for ( i = 0; i < __inbuffer->size; i++) {
                if ( ERIS_HTTP_REQUEST == __type) {
                    if ( __http->request.body.size > __http->attrs.body_max_size) {
                        /** Request entity too large */
                        rc = EERIS_DATA;

                        if ( ERIS_HTTP_REQUEST == __type) {
                            __http->response.status  = ERIS_HTTP_413;
                            __http->response.version = ERIS_HTTP_V10;
                            eris_string_set( &(__http->response.reason), eris_http_status_desc( 413));
                        }

                        if ( __http->log) {
                            eris_log_dump( __http->log, ERIS_LOG_CORE, "Request content data too large");
                        }

                        break;
                    }
                }

                switch ( __http->chunk_state) {
                    case ERIS_HTTP_CHUNKED :
                        {
                            /** Must hexadecimal char */
                            if ( ERIS_CHAR_IS_HEX( pb[ i])) {
                                __http->chunk_state = ERIS_HTTP_CHUNK_SIZE;

                                rc = eris_string_append_c( &chunk_size_s, (eris_char_t )pb[ i]);
                                if ( 0 != rc) {
                                    rc = EERIS_ERROR;

                                    if ( ERIS_HTTP_REQUEST == __type) {
                                        __http->response.status  = ERIS_HTTP_500;
                                        __http->response.version = ERIS_HTTP_V10;
                                        eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));
                                    }
                                }
                            } else {
                                rc = EERIS_DATA;

                                if ( ERIS_HTTP_REQUEST == __type) {
                                    __http->response.status  = ERIS_HTTP_400;
                                    __http->response.version = ERIS_HTTP_V10;
                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));
                                }

                                if ( __http->log) {
                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "A chunk size char is not hexadecimal");
                                }
                            }
                        } break;
                    case ERIS_HTTP_CHUNK_SIZE :
                        {
                            /** Must hexadecimal char */
                            if ( ERIS_CHAR_IS_HEX( pb[ i])) {
                                __http->chunk_state = ERIS_HTTP_CHUNK_SIZE;

                                rc = eris_string_append_c( &chunk_size_s, (eris_char_t )pb[i] );
                                if ( 0 != rc) {
                                    rc = EERIS_ERROR;

                                    if ( ERIS_HTTP_REQUEST == __type) {
                                        __http->response.status  = ERIS_HTTP_500;
                                        __http->response.version = ERIS_HTTP_V10;
                                        eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));
                                    }
                                }
                            } else if ( ';'  == pb[ i]) {
                                __http->chunk_state = ERIS_HTTP_CHUNK_EXT_NAME;

                            } else if ( '\r' == pb[ i]) {
                                __http->chunk_state = ERIS_HTTP_CHUNK_CR1;

                            } else {
                                rc = EERIS_DATA;

                                if ( ERIS_HTTP_REQUEST == __type) {
                                    __http->response.status  = ERIS_HTTP_400;
                                    __http->response.version = ERIS_HTTP_V10;
                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));
                                }

                                if ( __http->log) {
                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "A chunk size char is not hexadecimal");
                                }
                            }
                        } break;
                    case ERIS_HTTP_CHUNK_EXT_NAME :
                        {
                            /** Pass extension name */
                            if ( '=' == pb[ i]) {
                                __http->chunk_state = ERIS_HTTP_CHUNK_EXT_VALUE;

                            } else {
                                if ( ('_' == pb[ i]) || (isalnum( pb[i]) || ('-' == pb[i])) ) {
                                    /** Pass */

                                } else {
                                    rc = EERIS_DATA;

                                    if ( ERIS_HTTP_REQUEST == __type) {
                                        __http->response.status  = ERIS_HTTP_400;
                                        __http->response.version = ERIS_HTTP_V10;
                                        eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));
                                    }

                                    if ( __http->log) {
                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Chunk extension name is invalid");
                                    }
                                }
                            }
                        } break;
                    case ERIS_HTTP_CHUNK_EXT_VALUE :
                        {
                            /** Pass extension name */
                            if ( ';' == pb[ i]) {
                                __http->chunk_state = ERIS_HTTP_CHUNK_EXT_NAME;

                            } else if ( '\r' == pb[ i]) {
                                __http->chunk_state = ERIS_HTTP_CHUNK_CR1;

                            } else {
                                if ( ('_' == pb[ i]) || (isalnum( pb[i]) || ('-' == pb[i])) ) {
                                    /** Pass */

                                } else {
                                    rc = EERIS_DATA;

                                    if ( ERIS_HTTP_REQUEST == __type) {
                                        __http->response.status  = ERIS_HTTP_400;
                                        __http->response.version = ERIS_HTTP_V10;
                                        eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));
                                    }

                                    if ( __http->log) {
                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Chunk extension value is invalid");
                                    }
                                }
                            }
                        } break;
                    case ERIS_HTTP_CHUNK_CR1 :
                        {
                            if ( '\n' == pb[ i]) {
                                __http->chunk_size = eris_string_hextov( chunk_size_s );

                                eris_string_cleanup( chunk_size_s);

                                __http->chunk_state = ERIS_HTTP_CHUNK_LF1;
                            } else {
                                rc = EERIS_DATA;

                                if ( ERIS_HTTP_REQUEST == __type) {
                                    __http->response.status  = ERIS_HTTP_400;
                                    __http->response.version = ERIS_HTTP_V10;
                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));
                                }

                               if ( __http->log) {
                                   eris_log_dump( __http->log, ERIS_LOG_CORE, "Chunk extension value is invalid");
                               }
                            }
                        } break;
                    case ERIS_HTTP_CHUNK_LF1 :
                        {
                            if ( '\r' == pb[ i]) {
                                __http->chunk_state = ERIS_HTTP_CHUNK_CR2;

                            } else {
                                if ( 0 == __http->chunk_size) {
                                    __http->chunk_state = ERIS_HTTP_CHUNK_TRAILER_NAME;

                                } else {
                                    if ( ERIS_HTTP_REQUEST == __type) {
                                        /** Request chunked */
                                        rc = eris_buffer_append_c( &(__http->request.body), pb[ i], __http->log);

                                    } else {
                                        /** Response chunked */
                                        rc = eris_buffer_append_c( &(__http->response.body), pb[ i], __http->log);
                                    }

                                    if ( 0 != rc) {
                                        /** Append error */
                                        rc = EERIS_ERROR;

                                        if ( ERIS_HTTP_REQUEST == __type) {
                                            __http->response.status  = ERIS_HTTP_500;
                                            __http->response.version = ERIS_HTTP_V10;
                                            eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));
                                        }
                                    }

                                    __http->tmp_chunk_size++;
                                    __http->chunk_state = ERIS_HTTP_CHUNK_DATA;
                                }
                            }
                        } break;
                    case ERIS_HTTP_CHUNK_CR2 :
                        {
                            if ( '\n' == pb[ i]) {
                                __http->chunked_end = true;

                                __http->chunk_state = ERIS_HTTP_CHUNK_LF2;
                            } else {
                                rc = EERIS_DATA;

                                if ( ERIS_HTTP_REQUEST == __type) {
                                    __http->response.status  = ERIS_HTTP_400;
                                    __http->response.version = ERIS_HTTP_V10;
                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));
                                }

                                if ( __http->log) {
                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Chunk stream is invalid");
                                }
                            }
                        } break;
                    case ERIS_HTTP_CHUNK_LF2 :
                        {
                            __http->chunked_end = true;

                        } break;
                    case ERIS_HTTP_CHUNK_TRAILER_NAME :
                        {
                            if ( ':' == pb[ i]) {
                                __http->chunk_state = ERIS_HTTP_CHUNK_TRAILER_VALUE;

                            } else {
                                /** pass */
                                if ( ('_' == pb[ i]) || (isalnum( pb[i]) || ('-' == pb[i])) ) {
                                    /** Pass */

                                } else {
                                    rc = EERIS_DATA;

                                    if ( ERIS_HTTP_REQUEST == __type) {
                                        __http->response.status  = ERIS_HTTP_400;
                                        __http->response.version = ERIS_HTTP_V10;
                                        eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));
                                    }

                                    if ( __http->log) {
                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Chunk trailer name is invalid");
                                    }
                                }
                            }
                        } break;
                    case ERIS_HTTP_CHUNK_TRAILER_VALUE :
                        {
                            if ( '\r' == pb[ i]) {
                                __http->chunk_state = ERIS_HTTP_CHUNK_CR1;

                            } else {
                                /** pass */
                                if ( ('_' == pb[ i]) || (isalnum( pb[i]) || ('-' == pb[i])) ) {
                                    /** Pass */

                                } else {
                                    rc = EERIS_DATA;

                                    if ( ERIS_HTTP_REQUEST == __type) {
                                        __http->response.status  = ERIS_HTTP_400;
                                        __http->response.version = ERIS_HTTP_V10;
                                        eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));
                                    }

                                    if ( __http->log) {
                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Chunk trailer name is invalid");
                                    }
                                }
                            }
                        } break;
                    case ERIS_HTTP_CHUNK_DATA :
                        {
                            if ( '\r' == pb[ i]) {
                                __http->chunk_state = ERIS_HTTP_CHUNK_DATA_CR;

                                if ( __http->tmp_chunk_size != __http->chunk_size ) {
                                    rc = EERIS_DATA;

                                    if ( ERIS_HTTP_REQUEST == __type) {
                                        __http->response.status  = ERIS_HTTP_400;
                                        __http->response.version = ERIS_HTTP_V10;
                                        eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));
                                    }

                                    if ( __http->log) {
                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Chunk data is unequal chunk size");
                                    }
                                }

                                __http->chunk_size     = 0;
                                __http->tmp_chunk_size = 0;
                            } else {
                                if ( __http->tmp_chunk_size < __http->chunk_size) {
                                    if ( ERIS_HTTP_REQUEST == __type) {
                                        /** Request chunked */
                                        rc = eris_buffer_append_c( &(__http->request.body), pb[ i], __http->log);

                                    } else {
                                        /** Response chunked */
                                        rc = eris_buffer_append_c( &(__http->response.body), pb[ i], __http->log);
                                    }

                                    if ( 0 != rc) {
                                        /** Append error */
                                        rc = EERIS_ERROR;

                                        if ( ERIS_HTTP_REQUEST == __type) {
                                            __http->response.status  = ERIS_HTTP_500;
                                            __http->response.version = ERIS_HTTP_V10;
                                            eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));
                                        }
                                    }

                                    __http->tmp_chunk_size++;
                                } else {
                                    rc = EERIS_DATA;

                                    if ( ERIS_HTTP_REQUEST == __type) {
                                        __http->response.status  = ERIS_HTTP_400;
                                        __http->response.version = ERIS_HTTP_V10;
                                        eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));
                                    }

                                    if ( __http->log) {
                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Chunk data is unequal chunk size");
                                    }
                                }
                            }
                        } break;
                    case ERIS_HTTP_CHUNK_DATA_CR :
                        {
                            if ( '\n' == pb[ i]) {
                                __http->chunk_state = ERIS_HTTP_CHUNK_DATA_LF;

                            } else {
                                rc = EERIS_DATA;

                                if ( ERIS_HTTP_REQUEST == __type) {
                                    __http->response.status  = ERIS_HTTP_400;
                                    __http->response.version = ERIS_HTTP_V10;
                                    eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));
                                }

                                if ( __http->log) {
                                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Bad chunk data end");
                                }
                            }
                        } break;
                    case ERIS_HTTP_CHUNK_DATA_LF :
                        {
                            if ( '\r' == pb[ i]) {
                                __http->chunk_state = ERIS_HTTP_CHUNK_CR2;

                            } else {
                                /** Must hexadecimal char */
                                if ( ERIS_CHAR_IS_HEX( pb[ i])) {
                                    __http->chunk_state = ERIS_HTTP_CHUNK_SIZE;

                                    rc = eris_string_append_c( &chunk_size_s, (eris_char_t )pb[ i]);
                                    if ( 0 != rc) {
                                        rc = EERIS_ERROR;

                                        if ( ERIS_HTTP_REQUEST == __type) {
                                            __http->response.status  = ERIS_HTTP_500;
                                            __http->response.version = ERIS_HTTP_V10;
                                            eris_string_set( &(__http->response.reason), eris_http_status_desc( 500));
                                        }
                                    }
                                } else {
                                    rc = EERIS_DATA;

                                    if ( ERIS_HTTP_REQUEST == __type) {
                                        __http->response.status  = ERIS_HTTP_400;
                                        __http->response.version = ERIS_HTTP_V10;
                                        eris_string_set( &(__http->response.reason), eris_http_status_desc( 400));
                                    }

                                    if ( __http->log) {
                                        eris_log_dump( __http->log, ERIS_LOG_CORE, "A chunk size char is not hexadecimal");
                                    }
                                }
                            }
                        } break;
                    default :
                        {
                            __http->chunked_end = true;
                        } break;
                }/// switch ( __http->chunk_state)

                /** Happen error or end */
                if ( (0 != rc) || ( __http->chunked_end)) {
                    break;
                }
            }/// for ( i = 0; i < __inbuffer.size; i++)

            if ( chunk_size_s ) {
                eris_string_free( chunk_size_s);
                chunk_size_s = NULL;
            }
        } 

        if ( (0 != rc) || ( __http->chunked_end)) {
            __http->chunk_state    = ERIS_HTTP_CHUNKED;
            __http->chunk_size     = 0;
            __http->tmp_chunk_size = 0;
        }

    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_http_chunk_parse


/**
 * @Brief: Set request command, if user is client.
 *
 * @Param: __http,    Eris http context.
 * @Param: __command, Specify request command.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_http_request_set_command( eris_http_t *__http, eris_http_command_t __command)
{
    eris_int_t rc = 0;

    if ( __http) {
        /** Set command */
        __http->request.command = __command;

    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_http_request_set_command


/**
 * @Brief: Set request version, if user is client.
 *
 * @Param: __http,    Eris http context.
 * @Param: __version, Specify version.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_http_request_set_version( eris_http_t *__http, eris_http_version_t __version)
{
    eris_int_t rc = 0;

    if ( __http) {
        /** Set version */
        __http->request.version = __version;

    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_http_request_set_version


/**
 * @Brief: Set request url, if user is client.
 *
 * @Param: __http,  Eris http context.
 * @Param: __url,   Url string.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_http_request_set_url( eris_http_t *__http, const eris_char_t *__url)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __http) {
        if ( __url) {
            /** Set fragment */
            rc = eris_string_set( &(__http->request.url), __url);
            if ( 0 != rc) {
                if ( __http->log) {
                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Set url failed, errno.<%d>", errno);
                }
            }

        } else {
            rc = EERIS_ERROR;

            if ( __http->log) {
                eris_log_dump( __http->log, ERIS_LOG_CORE, "Input url is invalid.");
            }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_http_request_set_url


/**
 * @Brief: Set request parameters , if user is client.
 *
 * @Param: __http,  Eris http context.
 * @Param: __parameters, If GET command specify and set.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_http_request_set_parameters( eris_http_t *__http, const eris_char_t *__parameters)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __http) {
        if ( __parameters) {
            /** Set parameters */
            rc = eris_string_set( &(__http->request.parameters), __parameters);
            if ( 0 != rc) {
                if ( __http->log) {
                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Set paramenters failed, errno.<%d>", errno);
                }
            }

        } else {
            rc = EERIS_ERROR;

            if ( __http->log) {
                eris_log_dump( __http->log, ERIS_LOG_CORE, "Input parameters is invalid.");
            }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_http_request_set_paramters


/**
 * @Brief: Set request query, if user is client.
 *
 * @Param: __http,  Eris http context.
 * @Param: __query, If GET command specify and set, default is NULL.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_http_request_set_query( eris_http_t *__http, const eris_char_t *__query)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __http) {
        if ( __query) {
            /** Set query */
            rc = eris_string_set( &(__http->request.query), __query);
            if ( 0 != rc) {
                if ( __http->log) {
                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Set query failed, errno.<%d>", errno);
                }
            }

        } else {
            rc = EERIS_ERROR;

            if ( __http->log) {
                eris_log_dump( __http->log, ERIS_LOG_CORE, "Input query is invalid.");
            }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_http_request_set_query


/**
 * @Brief: Set request fragment, if user is client.
 *
 * @Param: __http,  Eris http context.
 * @Param: __fragment, If GET command specify and set, default is nothing.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_http_request_set_fragment( eris_http_t *__http, const eris_char_t *__fragment)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __http) {
        if ( __fragment) {
            /** Set fragment */
            rc = eris_string_set( &(__http->request.fragment), __fragment);
            if ( 0 != rc) {
                if ( __http->log) {
                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Set fragment failed, errno.<%d>", errno);
                }
            }

        } else {
            rc = EERIS_ERROR;

            if ( __http->log) {
                eris_log_dump( __http->log, ERIS_LOG_CORE, "Input fragment is invalid.");
            }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_http_request_set_fragment


/**
 * @Brief: Set http header name-value, if user is client.
 *
 * @Param: __http,  Eris http context.
 * @Param: __name,  HTTP protocol header name.
 * @Param: __value, HTTP protocol header value. 
 * 
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_http_request_set_header( eris_http_t *__http, const eris_char_t *__name, const eris_char_t *__value)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __http) {
        if ( __name && __value) {
            eris_http_hnode_t *cur_hnode = __http->request.headers;

            while ( cur_hnode) {
                if ( eris_string_isequal( cur_hnode->name, __name, true)) {
                    /** Is existed */
                    break;
                } else {
                    /** Goto next hnode */
                    cur_hnode = cur_hnode->next;
                }
            }

            /** Is existed... */
            if ( cur_hnode) {
                /** Set new value */
                rc = eris_string_set( &(cur_hnode->value), __value);
                if ( 0 != rc) {
                    if ( __http->log) {
                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Set header value failed, errno.<%d>", errno);
                    }
                }
            } else {
                cur_hnode = (eris_http_hnode_t *)eris_memory_alloc( sizeof( eris_http_hnode_t));
                if ( cur_hnode) {
                    eris_string_init( cur_hnode->name);
                    eris_string_init( cur_hnode->value);
                    cur_hnode->next = NULL;

                    /** Set name */
                    rc = eris_string_set( &(cur_hnode->name), __name);
                    if ( 0 == rc) {
                        /** Set value */
                        rc = eris_string_set( &(cur_hnode->value), __value);
                        if ( 0 == rc) {
                            /** Ok, link */
                            cur_hnode->next = __http->request.headers;

                            __http->request.headers = cur_hnode;

                        } else {
                            if ( __http->log) {
                                eris_log_dump( __http->log, ERIS_LOG_CORE, "Set header value failed, errno.<%d>", errno);
                            }
                        }
                    } else {
                        if ( __http->log) {
                            eris_log_dump( __http->log, ERIS_LOG_CORE, "Set header name failed, errno.<%d>", errno);
                        }
                    }

                    /** Set name or value failed */
                    if ( 0 != rc) {
                        eris_string_free( cur_hnode->name) ; cur_hnode->name = NULL;
                        eris_string_free( cur_hnode->value); cur_hnode->value= NULL;

                        /** Release */
                        eris_memory_free( cur_hnode);
                    }
                
                } else {
                    rc = EERIS_ERROR;

                    if ( __http->log) {
                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Create new header node failed, errno.<%d>", errno);
                    }
                }
            }
        } else {
            rc = EERIS_ERROR;

            if ( __http->log) {
                eris_log_dump( __http->log, ERIS_LOG_CORE, "Input header name-value is invalid.");
            }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_http_request_set_header


/**
 * @Brief: Set http request body, if user is client.
 *
 * @Param: __http, Eris http context.
 * @Param: __body, Body data.
 * @Param: __size, Body data size.
 * 
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_http_request_set_body( eris_http_t *__http, const eris_void_t *__body, eris_size_t __size)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __http) {
        if ( __body && (0 < __size)) {
            eris_buffer_cleanup( &(__http->response.body), __http->log);

            /** Set body data into buffer */
            rc = eris_buffer_append( &(__http->response.body), __body, __size, __http->log);
            if ( 0 != rc) {
                if ( __http->log) {
                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Set body failed, errno.<%d>", errno);
                }
            }

        } else {
            rc = EERIS_ERROR;

            if ( __http->log) {
                eris_log_dump( __http->log, ERIS_LOG_CORE, "Input body content is invalid");
            }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_http_request_set_body


/**
 * @Brief: Set http request body and append, if user is client.
 *
 * @Param: __http, Eris http context.
 * @Param: __body, Body data.
 * @Param: __size, Body data size.
 * 
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_http_request_set_body_append( eris_http_t *__http, const eris_void_t *__body, eris_size_t __size)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __http) {
        if ( __body && (0 < __size)) {
            /** Append body data */
            rc = eris_buffer_append( &(__http->response.body), __body, __size, __http->log);
            if ( 0 != rc) {
                if ( __http->log) {
                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Set body failed, errno.<%d>", errno);
                }
            }

        } else {
            rc = EERIS_ERROR;

            if ( __http->log) {
                eris_log_dump( __http->log, ERIS_LOG_CORE, "Input body content is invalid");
            }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_http_request_set_body


/**
 * @Brief: Get request command, if user is server and after parse request package.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Eris http commad.
 **/
eris_http_command_t eris_http_request_get_command( const eris_http_t *__http)
{
    eris_http_command_t rc_command = ERIS_HTTP_UNKNOW;

    if ( __http) {
        /** Request command */
        rc_command = __http->request.command;
    }

    return rc_command;
}/// eris_http_request_get_command


/**
 * @Brief: Get request version, if user is server and after parse request package.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Eris http version.
 **/
eris_http_version_t eris_http_request_get_version( const eris_http_t *__http)
{
    eris_http_version_t rc_version = ERIS_HTTP_V00;

    if ( __http) {
        /** Request version */
        rc_version = __http->request.version;
    }

    return rc_version;
}/// eris_http_request_get_version


/**
 * @Brief: Get request url, if user is server and after parse request package.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Url string.
 **/
const eris_string_t eris_http_request_get_url( const eris_http_t *__http)
{
    if ( __http) {
        /** Request url */
        return __http->request.url;
    }

    return NULL;
}/// eris_http_request_get_url


/**
 * @Brief: Get request parameters, if user is server and after parse request package.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Parameters string.
 **/
const eris_string_t eris_http_request_get_parameters( const eris_http_t *__http)
{
    if ( __http) {
        /** Request parameters string */
        return __http->request.parameters;
    }

    return NULL;
}/// eris_http_request_get_parameters


/**
 * @Brief: Get request query, if user is server and after parse request package.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Qeury string.
 **/
const eris_string_t eris_http_request_get_query( const eris_http_t *__http)
{
    if ( __http) {
        /** Request query string */
        return __http->request.query;
    }

    return NULL;
}/// eris_http_request_get_query


/**
 * @Brief: Get request fragment, if user is server and after parse request package.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Fragment string.
 **/
const eris_string_t eris_http_request_get_fragment( const eris_http_t *__http)
{
    if ( __http) {
        /** Request fragment string */
        return __http->request.fragment;
    }

    return NULL;
}/// eris_http_request_get_fragment


/**
 * @Brief: Get http header name-value, if user is server.
 *
 * @Param: __http,  Eris http context.
 * @Param: __name,  HTTP protocol header name.
 * 
 * @Return: Name -> value string.
 **/
const eris_string_t eris_http_request_get_header( const eris_http_t *__http, const eris_char_t *__name)
{
    if ( __http && __name) {
        eris_http_hnode_t *cur_hnode = __http->request.headers;

        while ( cur_hnode) {
            /** Found name? */
            if ( eris_string_isequal( cur_hnode->name, __name, true)) {
                /** Ok, Has found name-value and return */
#if 0
                eris_log_dump( __http->log, ERIS_LOG_CORE, "%s: %s", cur_hnode->name, cur_hnode->value);
#endif

                return cur_hnode->value;
            } else {
                /** Goto next */
                cur_hnode = cur_hnode->next;
            }
        }
    }

    return NULL;
}/// eris_http_request_get_header


/**
 * @Brief: Get http request body buffer object, if user is server.
 *
 * @Param: __http, Eris http context.
 * 
 * @Return: Body buffer object.
 **/
const eris_buffer_t *eris_http_request_get_body( const eris_http_t *__http)
{
    if ( __http) {
        /** Request body buffer */
        return &(__http->request.body);
    }

    return NULL;
}/// eris_http_request_get_body


/**
 * @Brief: Get http request body data, if user is server.
 *
 * @Param: __http, Eris http context.
 * 
 * @Return: Body data.
 **/
const eris_void_t *eris_http_request_get_body_data( const eris_http_t *__http)
{
    if ( __http) {
        /** Request body buffer data */
        return eris_buffer_data( &(__http->request.body), __http->log);
    }

    return NULL;
}/// eris_http_request_get_body_data


/**
 * @Brief: Get http request body size, if user is server.
 *
 * @Param: __http, Eris http context.
 * 
 * @Return: Body data size.
 **/
eris_size_t eris_http_request_get_body_size( const eris_http_t *__http)
{
    if ( __http) {
        /** Response body buffer size */
        return eris_buffer_size( &(__http->request.body), __http->log);
    }

    return 0;
}/// eris_http_request_get_body_size


/**
 * @Brief: Set response version, if user is server.
 *
 * @Param: __http,    Eris http context.
 * @Param: __version, Specify version.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_http_response_set_version( eris_http_t *__http, eris_http_version_t __version)
{
    eris_int_t rc = 0;

    if ( __http) {
        /** Set version */
        __http->response.version = __version;

    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_http_response_set_version


/**
 * @Brief: Set response status, if user is server.
 *
 * @Param: __http,   Eris http context.
 * @Param: __status, Specify status code.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_http_response_set_status( eris_http_t *__http, eris_http_status_t __status)
{
    eris_int_t rc = 0;

    if ( __http) {
        __http->response.status = __status;

    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_http_response_set_status


/**
 * @Brief: Set response reason phrace, if user is server.
 *
 * @Param: __http,   Eris http context.
 * @Param: __reason, Reason phrace string.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_http_response_set_reason( eris_http_t *__http, const eris_char_t *__reason)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;
    
    if ( __http) {
        /** Set reason phrace */
        rc = eris_string_set( &(__http->response.reason), __reason);
        if ( 0 != rc) {
            if ( __http->log) {
                eris_log_dump( __http->log, ERIS_LOG_CORE, "Set reason phrace failed, errno.<%d>", errno);
            }
        }

    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_http_response_set_reason


/**
 * @Brief: Set http header name-value, if user is server.
 *
 * @Param: __http,  Eris http context.
 * @Param: __name,  HTTP protocol header name.
 * @Param: __value, HTTP protocol header value. 
 * 
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_http_response_set_header( eris_http_t *__http, const eris_char_t *__name, const eris_char_t *__value)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __http) {
        if ( __name && __value) {
            eris_http_hnode_t *cur_hnode = __http->response.headers;

            while ( cur_hnode) {
                if ( eris_string_isequal( cur_hnode->name, __name, true)) {
                    /** Is existed */
                    break;
                } else {
                    /** Goto next hnode */
                    cur_hnode = cur_hnode->next;
                }
            }

            /** Is existed... */
            if ( cur_hnode) {
                printf( "3----%s:%s\n", __name, __value);
                /** Set new value */
                rc = eris_string_set( &(cur_hnode->value), __value);
                if ( 0 != rc) {
                    if ( __http->log) {
                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Set header value failed, errno.<%d>", errno);
                    }
                }
            } else {
                cur_hnode = (eris_http_hnode_t *)eris_memory_alloc( sizeof( eris_http_hnode_t));
                if ( cur_hnode) {
                    eris_string_init( cur_hnode->name);
                    eris_string_init( cur_hnode->value);
                    cur_hnode->next = NULL;

                    /** Set name */
                    rc = eris_string_set( &(cur_hnode->name), __name);
                    if ( 0 == rc) {
                        /** Set value */
                        rc = eris_string_set( &(cur_hnode->value), __value);
                        if ( 0 == rc) {
                            /** Ok, link */
                            cur_hnode->next = __http->response.headers;

                            __http->response.headers = cur_hnode;

                        } else {
                            if ( __http->log) {
                                eris_log_dump( __http->log, ERIS_LOG_CORE, "Set header value failed, errno.<%d>", errno);
                            }
                        }
                    } else {
                        if ( __http->log) {
                            eris_log_dump( __http->log, ERIS_LOG_CORE, "Set header name failed, errno.<%d>", errno);
                        }
                    }

                    /** Set name or value failed */
                    if ( 0 != rc) {
                        eris_string_free( cur_hnode->name) ; cur_hnode->name = NULL;
                        eris_string_free( cur_hnode->value); cur_hnode->value= NULL;

                        /** Release */
                        eris_memory_free( cur_hnode);
                    }
                
                } else {
                    rc = EERIS_ERROR;

                    if ( __http->log) {
                        eris_log_dump( __http->log, ERIS_LOG_CORE, "Create new header node failed, errno.<%d>", errno);
                    }
                }
            }
        } else {
            rc = EERIS_ERROR;

            if ( __http->log) {
                eris_log_dump( __http->log, ERIS_LOG_CORE, "Input header name-value is invalid.");
            }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_http_response_set_header


/**
 * @Brief: Set http response body, if user is server.
 *
 * @Param: __http, Eris http context.
 * @Param: __body, Body data.
 * @Param: __size, Body data size.
 * 
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_http_response_set_body( eris_http_t *__http, const eris_void_t *__body, eris_size_t __size)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __http) {
        if ( __body && (0 < __size)) {
            eris_buffer_cleanup( &(__http->response.body), __http->log);

            /** Set body data into buffer */
            rc = eris_buffer_append( &(__http->response.body), __body, __size, __http->log);
            if ( 0 != rc) {
                if ( __http->log) {
                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Set body failed, errno.<%d>", errno);
                }
            }

        } else {
            rc = EERIS_ERROR;

            if ( __http->log) {
                eris_log_dump( __http->log, ERIS_LOG_CORE, "Input body content is invalid");
            }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_http_response_set_body


/**
 * @Brief: Set http response body and append, if user is server.
 *
 * @Param: __http, Eris http context.
 * @Param: __body, Body data.
 * @Param: __size, Body data size.
 * 
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_http_response_set_body_append( eris_http_t *__http, const eris_void_t *__body, eris_size_t __size)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __http) {
        if ( __body && (0 < __size)) {
            /** Append body data */
            rc = eris_buffer_append( &(__http->response.body), __body, __size, __http->log);
            if ( 0 != rc) {
                if ( __http->log) {
                    eris_log_dump( __http->log, ERIS_LOG_CORE, "Set body failed, errno.<%d>", errno);
                }
            }

        } else {
            rc = EERIS_ERROR;

            if ( __http->log) {
                eris_log_dump( __http->log, ERIS_LOG_CORE, "Input body content is invalid");
            }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_http_response_set_body_append


/**
 * @Brief: Get response version, if user is client.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Version value.
 **/
eris_http_version_t eris_http_response_get_version( const eris_http_t *__http)
{
    eris_http_version_t rc_version = ERIS_HTTP_V00;

    if ( __http) {
        rc_version = __http->response.version;
    }

    return rc_version;
}/// eris_http_response_get_version


/**
 * @Brief: Get response status, if user is client.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: response status.
 **/
eris_http_status_t eris_http_response_get_status( const eris_http_t *__http)
{
    eris_http_status_t rc_status = ERIS_HTTP_000;

    if ( __http) {
        rc_status = __http->response.status;
    }

    return rc_status;
}/// eris_http_response_get_status


/**
 * @Brief: Get response reason phrace, if user is client.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Reason phrace string.
 **/
const eris_string_t eris_http_response_get_reason( const eris_http_t *__http)
{
    if ( __http) {
        /** Return response reason phrace */
        return __http->response.reason;
    }

    return NULL;
}/// eris_http_response_get_reason


/**
 * @Brief: Get http header name-value, if user is client.
 *
 * @Param: __http,  Eris http context.
 * @Param: __name,  HTTP protocol header name.
 * 
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
const eris_string_t eris_http_response_get_header( const eris_http_t *__http, const eris_char_t *__name)
{
    if ( __http && __name) {
        eris_http_hnode_t *cur_hnode = __http->response.headers;

        while ( cur_hnode) {
            /** Found name? */
            if ( eris_string_isequal( cur_hnode->name, __name, true)) {
                /** Ok, Has found name-value and return */

                return cur_hnode->value;
            } else {
                /** Goto next */
                cur_hnode = cur_hnode->next;
            }
        }
    }

    return NULL;
}/// eris_http_response_get_header


/**
 * @Brief: Get http response body object, if user is client
 *
 * @Param: __http, Eris http context.
 * 
 * @Return: Body buffer object.
 **/
const eris_buffer_t *eris_http_response_get_body( const eris_http_t *__http)
{
    if ( __http) {
        /** Response body buffer */
        return &(__http->response.body);
    }

    return NULL;
}/// eris_http_response_get_body


/**
 * @Brief: Get http response body data, if user is client
 *
 * @Param: __http, Eris http context.
 * 
 * @Return: Body data.
 **/
const eris_void_t *eris_http_response_get_body_data( const eris_http_t *__http)
{
    if ( __http) {
        /** Body buffer data */
        return eris_buffer_data( &(__http->response.body), __http->log);
    }

    return NULL;
}/// eris_http_response_get_body_data


/**
 * @Brief: Get http response body size, if user is client
 *
 * @Param: __http, Eris http context.
 * 
 * @Return: Body data size.
 **/
eris_size_t eris_http_response_get_body_size( const eris_http_t *__http)
{
    if ( __http) {
        /** Response body buffer */
        return eris_buffer_size( &(__http->response.body), __http->log);
    }

    return 0;
}/// eris_http_response_get_body_size


/**
 * @Brief: Clean up eris http context.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_http_cleanup( eris_http_t *__http)
{
    if ( __http) {
        /** Clean up request */
        {
            __http->request.command = ERIS_HTTP_UNKNOW;

            eris_string_free( __http->request.url);
            __http->request.url = NULL;

            __http->request.version = ERIS_HTTP_V00;

            /** Release all headers nodes */
            eris_http_hnode_t *del_hnode = __http->request.headers;
            while ( del_hnode) {
                __http->request.headers = __http->request.headers->next;

                if ( del_hnode->name) {
                    eris_string_free( del_hnode->name);
                    del_hnode->name  = NULL;
                }

                if ( del_hnode->value) {
                    eris_string_free( del_hnode->value);
                    del_hnode->value = NULL;
                }

                del_hnode->next  = NULL;

                eris_memory_free( del_hnode);

                /** Goto next */
                del_hnode = __http->request.headers;
            }

            eris_string_free( __http->request.query);
            __http->request.query = NULL;

            eris_buffer_free( &(__http->request.body));
        }/// End: clean up request

        /** Clean up response */
        {
            __http->response.version = ERIS_HTTP_V00;
            __http->response.status  = ERIS_HTTP_000;

            eris_string_free( __http->response.reason);
            __http->response.reason = NULL;

            /** Release all headers nodes */
            eris_http_hnode_t *del_hnode = __http->response.headers;
            while ( del_hnode) {
                __http->response.headers = __http->response.headers->next;

                if ( del_hnode->name) {
                    eris_string_free( del_hnode->name);
                    del_hnode->name  = NULL;
                }

                if ( del_hnode->value) {
                    eris_string_free( del_hnode->value);
                    del_hnode->value = NULL;
                }

                del_hnode->next  = NULL;

                eris_memory_free( del_hnode);

                /** Goto next */
                del_hnode = __http->response.headers;
            }

            eris_buffer_free( &(__http->response.body));
        }/// End: clean up response

        /** Cleanup header/body buffer */
        eris_buffer_cleanup( &(__http->hbuffer), NULL);
        eris_buffer_cleanup( &(__http->bbuffer), NULL);

        __http->chunk_state    = ERIS_HTTP_CHUNKED;
        __http->chunked        = false;
        __http->chunked_end    = false;
        __http->chunk_size     = 0;
        __http->tmp_chunk_size = 0;
    }
}/// eris_http_cleanup


/**
 * @Brief: Clean up eris http request handler.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_http_cleanup_request( eris_http_t *__http)
{
    if ( __http) {
        __http->request.shake   = false;
        __http->request.command = ERIS_HTTP_UNKNOW;

        eris_string_free( __http->request.url);
        __http->request.url = NULL;

        __http->request.version = ERIS_HTTP_V00;

        /** Release all headers nodes */
        eris_http_hnode_t *del_hnode = __http->request.headers;
        while ( del_hnode) {
            del_hnode->next  = NULL;
            __http->request.headers = __http->request.headers->next;

            if ( del_hnode->name) {
                eris_string_free( del_hnode->name);
                del_hnode->name = NULL;
            }

            if ( del_hnode->value) {
                eris_string_free( del_hnode->value);
                del_hnode->value = NULL;
            }

            eris_memory_free( del_hnode);

            /** Goto next */
            del_hnode = __http->request.headers;
        }

        eris_string_free( __http->request.query);
        __http->request.query = NULL;

        eris_buffer_free( &(__http->request.body));
    }
}/// eris_http_cleanup_request


/**
 * @Brief: Clean up eris http response handler.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_http_cleanup_response( eris_http_t *__http)
{
    if ( __http) {
        /** Clean up response */
        __http->response.version = ERIS_HTTP_V00;
        __http->response.status  = ERIS_HTTP_000;

        eris_string_free( __http->response.reason);
        __http->response.reason = NULL;

        /** Release all headers nodes */
        eris_http_hnode_t *del_hnode = __http->response.headers;
        while ( del_hnode) {
            __http->response.headers = __http->response.headers->next;

            if ( del_hnode->name) {
                eris_string_free( del_hnode->name);
                del_hnode->name  = NULL;
            }

            if ( del_hnode->value) {
                eris_string_free( del_hnode->value);
                del_hnode->value = NULL;
            }

            del_hnode->next  = NULL;

            eris_memory_free( del_hnode);

            /** Goto next */
            del_hnode = __http->response.headers;
        }

        eris_buffer_free( &(__http->response.body));
    }
}/// eris_http_cleanup_response


/**
 * @Brief: Check status is valid.
 *
 * @Parma: __status, Input status code.
 *
 * @Return: Is valid is true, Other is false.
 **/
eris_bool_t eris_http_status_valid( eris_int_t __status)
{
    eris_bool_t is_valid = false;

    eris_int_t i = 0;

    for ( i = 0; i < eris_http_status_map_reason_size; i++) {
        if ( __status == eris_http_status_map_reason[ i].status) {
            is_valid = true;

            break;
        }
    }

    return is_valid;
}/// eris_eris_http_status_valid


/**
 * @Brief: Get status string
 *
 * @Param: __status, Status code.
 *
 * @Return: status string.
 **/
const eris_char_t *eris_http_status_string( eris_int_t __status)
{
    eris_int_t i = 0;

    for ( i = 0; i < eris_http_status_map_reason_size; i++) {
        if ( __status == eris_http_status_map_reason[ i].status) {

            return eris_http_status_map_reason[ i].status_s;
        }
    }

    return NULL;
}/// eris_http_status_reason_phrace


/**
 * @Brief: Get status reason phrace.
 *
 * @Param: __status, Status code.
 *
 * @Return: reason phrace string.
 **/
const eris_char_t *eris_http_status_reason_phrace( eris_int_t __status)
{
    eris_int_t i = 0;

    for ( i = 0; i < eris_http_status_map_reason_size; i++) {
        if ( __status == eris_http_status_map_reason[ i].status) {

            return eris_http_status_map_reason[ i].reason_phrace;
        }
    }

    return NULL;
}/// eris_http_status_reason_phrace


/**
 * @Brief: Destroy eris http context.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_http_destroy( eris_http_t *__http)
{
    if ( __http) {
        eris_http_cleanup( __http);

        /** Destroy request/response buffer */
        eris_buffer_destroy( &(__http->request.body));
        eris_buffer_destroy( &(__http->response.body));

        /** Destroy header/body buffer */
        eris_buffer_destroy( &(__http->hbuffer));
        eris_buffer_destroy( &(__http->bbuffer));

        {
            __http->log   = NULL;
        }
    }
}/// eris_http_destroy



