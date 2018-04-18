#ifndef __ERIS_HTTP_H__
#define __ERIS_HTTP_H__

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


#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"



/** ERIS HTTP command */
#define ERIS_HTTP_COMMAND(CX) \
    CX(0 ,  UNKNOW  , UNKNOW  ) \
    CX(1 ,  GET     , GET     ) \
    CX(2 ,  HEAD    , HEAD    ) \
    CX(3 ,  POST    , POST    ) \
    CX(4 ,  PUT     , PUT     ) \
    CX(5 ,  DELETE  , DELETE  ) \
    CX(6 ,  OPTIONS , OPTIONS ) \
    CX(7 ,  TRACE   , TRACE   ) \
    CX(8 ,  PATCH   , PATCH   ) \
    CX(9 ,  MOVE    , MOVE    ) \
    CX(10,  COPY    , COPY    ) \
    CX(11,  LINK    , LINK    ) \
    CX(12,  UNLINK  , UNLINK  ) \
    CX(13,  PURGE   , PURGE   ) \
    CX(14,  LOCK    , LOCK    ) \
    CX(15,  UNLOCK  , UNLOCK  ) \
    CX(17,  PROPFIND, PROPFIND) \
    CX(18,  VIEW    , VIEW    ) \
    CX(19,  CMAX    , CMAX    ) \


enum eris_http_command_e {
#define CX(n, name, s) ERIS_HTTP_##name = n,
    ERIS_HTTP_COMMAND(CX)
#undef CX
};
typedef enum eris_http_command_e eris_http_command_t;


#define eris_http_command_v(c)   ERIS_HTTP_##c


/** HTTP version string */
#define ERIS_HTTP_V09_S "HTTP/0.9"
#define ERIS_HTTP_V10_S "HTTP/1.0"
#define ERIS_HTTP_V11_S "HTTP/1.1"
#define ERIS_HTTP_VERSION_SIZE  8


enum eris_http_version_e {
    ERIS_HTTP_V00 = 0,     /** None     */
    ERIS_HTTP_V09 = 9,     /** HTTP/0.9 */
    ERIS_HTTP_V10 = 10,    /** HTTP/1.0 */
    ERIS_HTTP_V11 = 11,    /** HTTP/1.1 */
};
typedef enum eris_http_version_e eris_http_version_t;

#define eris_http_version_tos(v) ERIS_HTTP_V##v##_S


/** ERIS HTTP status */
#define ERIS_HTTP_STATUS(SX) \
    SX(000, 000, None                           ) \
    SX(100, 100, Continue                       ) \
    SX(101, 101, Switching Protocols            ) \
    SX(200, 200, Ok                             ) \
    SX(201, 201, Created                        ) \
    SX(202, 202, Accepted                       ) \
    SX(203, 203, Non-Authoritative Information  ) \
    SX(204, 204, No Content                     ) \
    SX(205, 205, Reset Content                  ) \
    SX(206, 206, Partial Content                ) \
    SX(300, 300, Multiple Choices               ) \
    SX(301, 301, Moved Permanently              ) \
    SX(302, 302, Found                          ) \
    SX(303, 303, See Other                      ) \
    SX(304, 304, Not Modified                   ) \
    SX(305, 305, Use Proxy                      ) \
    SX(306, 306, Unused                         ) \
    SX(307, 307, Temporary Redirect             ) \
    SX(400, 400, Bad Request                    ) \
    SX(401, 401, Unauthorized                   ) \
    SX(402, 402, Payment Required               ) \
    SX(403, 403, ForBidden                      ) \
    SX(404, 404, Not Found                      ) \
    SX(405, 405, Method Not Allowed             ) \
    SX(406, 406, Not Acceptable                 ) \
    SX(407, 407, Proxy Authentication Required  ) \
    SX(408, 408, Request Timeout                ) \
    SX(409, 409, Conflict                       ) \
    SX(410, 410, Gone                           ) \
    SX(411, 411, Length Required                ) \
    SX(412, 412, Precondition Failed            ) \
    SX(413, 413, Request Entity Too Large       ) \
    SX(414, 414, Request URI Too Long           ) \
    SX(415, 415, Unsupported Media Type         ) \
    SX(416, 416, Requested range not satisfiable) \
    SX(417, 417, Expectation Failed             ) \
    SX(418, 418, Reserved                       ) \
    SX(419, 419, Reserved                       ) \
    SX(420, 420, Reserved                       ) \
    SX(421, 421, Too Many Connections           ) \
    SX(422, 422, Unprocessable Entity           ) \
    SX(423, 423, Locked                         ) \
    SX(424, 424, Failed Dependency              ) \
    SX(425, 425, Reserved                       ) \
    SX(426, 426, Change TLS/1.0                 ) \
    SX(427, 427, Reserved                       ) \
    SX(428, 428, Precondintion Required         ) \
    SX(429, 429, Too many Requests              ) \
    SX(430, 430, Reserved                       ) \
    SX(431, 431, Request Header Fields Too Large) \
    SX(449, 449, MS Precondintion Required      ) \
    SX(500, 500, Internal Server Error          ) \
    SX(501, 501, Not Implemented                ) \
    SX(502, 502, Bad Gateway                    ) \
    SX(503, 503, Service Unavailable            ) \
    SX(504, 504, Gateway Timeout                ) \
    SX(505, 505, HTTP Version Not Supported     ) \
    SX(506, 506, Variant Also Negotiates        ) \
    SX(507, 507, Insufficient Storage           ) \
    SX(508, 508, Loop Detected                  ) \
    SX(509, 509, Bandwidth Limit Exceeded       ) \
    SX(510, 510, Not Extended                   ) \
    SX(511, 511, Network Authentication Required) \


/** Code reason phrace */
#define ERIS_HTTP_000_RP "None"
#define ERIS_HTTP_100_RP "Continue"
#define ERIS_HTTP_101_RP "Switching Protocols"
#define ERIS_HTTP_200_RP "Ok"
#define ERIS_HTTP_201_RP "Created"
#define ERIS_HTTP_202_RP "Accepted"
#define ERIS_HTTP_203_RP "Non-Authoritative Information"
#define ERIS_HTTP_204_RP "No Content"
#define ERIS_HTTP_205_RP "Reset Content"
#define ERIS_HTTP_206_RP "Partial Content"
#define ERIS_HTTP_300_RP "Multiple Choices"
#define ERIS_HTTP_301_RP "Moved Permanently"
#define ERIS_HTTP_302_RP "Found"
#define ERIS_HTTP_303_RP "See Other"
#define ERIS_HTTP_304_RP "Not Modified"
#define ERIS_HTTP_305_RP "Use Proxy"
#define ERIS_HTTP_306_RP "Unused"
#define ERIS_HTTP_307_RP "Temporary Redirect"
#define ERIS_HTTP_400_RP "Bad Request"
#define ERIS_HTTP_401_RP "Unauthorized"
#define ERIS_HTTP_402_RP "Payment Required"
#define ERIS_HTTP_403_RP "ForBidden"
#define ERIS_HTTP_404_RP "Not Found"
#define ERIS_HTTP_405_RP "Method Not Allowed"
#define ERIS_HTTP_406_RP "Not Acceptable"
#define ERIS_HTTP_407_RP "Proxy Authentication Required"
#define ERIS_HTTP_408_RP "Request Timeout"
#define ERIS_HTTP_409_RP "Conflict"
#define ERIS_HTTP_410_RP "Gone"
#define ERIS_HTTP_411_RP "Length Required"
#define ERIS_HTTP_412_RP "Precondition Failed"
#define ERIS_HTTP_413_RP "Request Entity Too Large"
#define ERIS_HTTP_414_RP "Request URI Too Long"
#define ERIS_HTTP_415_RP "Unsupported Media Type"
#define ERIS_HTTP_416_RP "Requested range not satisfiable"
#define ERIS_HTTP_417_RP "Expectation Failed"
#define ERIS_HTTP_418_RP "Reserved"
#define ERIS_HTTP_419_RP "Reserved"
#define ERIS_HTTP_420_RP "Reserved"
#define ERIS_HTTP_421_RP "Too Many Connections"
#define ERIS_HTTP_422_RP "Unprocessable Entity"
#define ERIS_HTTP_423_RP "Locked"
#define ERIS_HTTP_424_RP "Failed Dependency"
#define ERIS_HTTP_425_RP "Reserved"
#define ERIS_HTTP_426_RP "Change TLS/1.0"
#define ERIS_HTTP_427_RP "Reserved"
#define ERIS_HTTP_428_RP "Precondintion Required"
#define ERIS_HTTP_429_RP "Too many Requests"
#define ERIS_HTTP_430_RP "Reserved"
#define ERIS_HTTP_431_RP "Request Header Fields Too Large"
#define ERIS_HTTP_449_RP "MS Precondintion Required"
#define ERIS_HTTP_500_RP "Internal Server Error"
#define ERIS_HTTP_501_RP "Not Implemented"
#define ERIS_HTTP_502_RP "Bad Gateway"
#define ERIS_HTTP_503_RP "Service Unavailable"
#define ERIS_HTTP_504_RP "Gateway Timeout"
#define ERIS_HTTP_505_RP "HTTP Version Not Supported"
#define ERIS_HTTP_506_RP "Variant Also Negotiates"
#define ERIS_HTTP_507_RP "Insufficient Storage"
#define ERIS_HTTP_508_RP "Loop Detected"
#define ERIS_HTTP_509_RP "Bandwidth Limit Exceeded"
#define ERIS_HTTP_510_RP "Not Extended"
#define ERIS_HTTP_511_RP "Network Authentication Required"

enum eris_http_status_e {
#define SX(n, name, s) ERIS_HTTP_##name = n,
    ERIS_HTTP_STATUS(SX)
#undef SX
};
typedef enum eris_http_status_e eris_http_status_t;

#define eris_http_status_v(c)    ERIS_HTTP_##c
#define eris_http_status_desc(c) ERIS_HTTP_##c##_RP

#define ERIS_HTTP_CRLF           "\r\n"
#define ERIS_HTTP_CRLF_SIZE      2


/** Type: request or response */
enum eris_http_type_e {
    ERIS_HTTP_REQUEST = 0,
    ERIS_HTTP_RESPONSE,
};
typedef enum eris_http_type_e eris_http_type_t; 


/** Chunked state of parse */
enum eris_http_chunk_state_e {
    /** Chunked */
    ERIS_HTTP_CHUNKED            = 0,
    ERIS_HTTP_CHUNK_SIZE         ,
    ERIS_HTTP_CHUNK_EXT_NAME     ,
    ERIS_HTTP_CHUNK_EXT_VALUE    ,
    ERIS_HTTP_CHUNK_CR1          ,
    ERIS_HTTP_CHUNK_LF1          ,
    ERIS_HTTP_CHUNK_CR2          ,
    ERIS_HTTP_CHUNK_LF2          ,
    ERIS_HTTP_CHUNK_TRAILER_NAME ,
    ERIS_HTTP_CHUNK_TRAILER_VALUE,
    ERIS_HTTP_CHUNK_DATA         ,
    ERIS_HTTP_CHUNK_DATA_CR      , 
    ERIS_HTTP_CHUNK_DATA_LF      ,
    ERIS_HTTP_CHUNKED_FINISH     ,
};
typedef enum eris_http_chunk_state_e eris_http_chunk_state_t;


/** Status map reason phrace */
typedef struct eris_http_status_map_reason_s eris_http_status_map_reason_t;
struct eris_http_status_map_reason_s {
    eris_http_status_t  status;
    eris_char_t        *status_s;
    eris_char_t        *reason_phrace;
};

/** Eris-http init attrs */
typedef struct eris_http_attr_s eris_http_attr_t;
struct eris_http_attr_s {
    eris_size_t url_max_size;        /** URL max size      */
    eris_size_t header_max_size;     /** Header max size   */
    eris_size_t body_max_size;       /** Body max size     */
    eris_size_t header_cache_size;   /** Header cache size */
    eris_size_t body_cache_size;     /** Body cache size   */
};

/** Eris-http header node type */
typedef struct eris_http_hnode_s eris_http_hnode_t;
struct eris_http_hnode_s {
    eris_string_t      name;         /** Header name       */
    eris_string_t      value;        /** Header value      */
    eris_http_hnode_t *next;         /** Pointer next      */
};

/** Eris-http request type */
typedef struct eris_http_request_s eris_http_request_t;
struct eris_http_request_s {
    eris_bool_t          shake;      /** 100-continue set  */
    eris_http_command_t  command;    /** Request command   */
    eris_string_t        url;        /** Request url       */
    eris_http_version_t  version;    /** Http version      */
    eris_http_hnode_t   *headers;    /** Request headers   */
    eris_string_t        parameters; /** parameters string */
    eris_string_t        query;      /** Query string      */
    eris_string_t        fragment;   /** Fragment string   */
    eris_buffer_t        body;       /** Request body data */
};

/** Eris-http response type */
typedef struct eris_http_response_s eris_http_response_t;
struct eris_http_response_s {
    eris_http_version_t version;     /** Http version      */
    eris_http_status_t  status;      /** Response status   */
    eris_string_t       reason;      /** reason phrace     */
    eris_http_hnode_t  *headers;     /** response headers  */
    eris_buffer_t       body;        /** Response body data*/
};

/** Eris-http context type */
//typedef struct eris_http_s eris_http_t;
struct eris_http_s {
    eris_http_attr_t        attrs;          /** Attributes info  */
    eris_http_request_t     request;        /** Request info     */
    eris_http_response_t    response;       /** Response info    */
    eris_buffer_t           hbuffer;        /** Header buffer    */
    eris_buffer_t           bbuffer;        /** Body buffer      */
    eris_http_chunk_state_t chunk_state;    
    eris_bool_t             chunked;        /** Is chunked data  */
    eris_bool_t             chunked_end;    /** Parse ok and end */
    eris_size_t             chunk_size;     /** A chunk size     */
    eris_size_t             tmp_chunk_size; /** ................ */
    eris_log_t             *log;            /** Log context      */
};


/** Command map to string array */
extern const eris_char_t *eris_http_command_arrs[];

#define eris_http_command_arrs_size ERIS_HTTP_CMAX

/** Status map reason phrace string */
extern const eris_http_status_map_reason_t eris_http_status_map_reason[];

#define eris_http_status_map_reason_size (sizeof(eris_http_status_map_reason) / sizeof(eris_http_status_map_reason_t))


/** Eris-http read callback type, return: ok is 0, other is EERIS_ERROR */
typedef eris_int_t (*eris_http_cb_t)( eris_buffer_t *__buffer, eris_size_t __size, eris_arg_t __arg, eris_log_t *__log);


/**
 * @Brief: Eris-http init.
 *
 * @Param: __http,  Eris http context.
 * @Param: __attrs, Specify attributes of http limit info.
 * @Param: __log,   Log context.
 * 
 * @Return: Ok is 0, Internal error is EERIS_ERROR, Other request data invalid is 1.
 **/
extern eris_int_t eris_http_init( eris_http_t *__http, eris_http_attr_t *__attrs, eris_log_t *__log);


/**
 * @Brief: Read http-request data package from callback input.
 *
 * @Param: __http, Eris http context.
 * @Param: __incb, Read http-request data from input callback.
 * @Param: __arg,  Call callback and input argument.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR or EERIS_4XX.
 **/
extern eris_int_t eris_http_request_parse( eris_http_t *__http, eris_http_cb_t __incb, eris_arg_t __arg);


/**
 * @Brief: The client request want to shake hands.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: yes is true, other is false.
 **/
extern eris_bool_t eris_http_request_shake( eris_http_t *__http);


/**
 * @Brief: Write http-request data package to callback output.
 *
 * @Param: __http, Eris http context.
 * @Param: __outcb,Write http-request data to output callback.
 * @Param: __arg,  Call callback and input argument.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_http_request_pack( eris_http_t *__http, eris_http_cb_t __outcb, eris_arg_t __arg);


/**
 * @Brief: Read http-response data package from callback input.
 *
 * @Param: __http, Eris http context.
 * @Param: __incb, Read http-response data from intput callback.
 * @Param: __arg,  Call callback and input argument.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR or EERIS_DATA.
 **/
extern eris_int_t eris_http_response_parse( eris_http_t *__http, eris_http_cb_t __incb, eris_arg_t __arg);


/**
 * @Brief: Write http-response data package to callback output.
 *
 * @Param: __http, Eris http context.
 * @Param: __outcb,Write http-response data to output callback.
 * @Param: __arg,  Call callback and input argument.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_http_response_pack( eris_http_t *__http, eris_http_cb_t __outcb, eris_arg_t __arg);


/**
 * @Brief: The client request connection is keep alive.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Keep-alive is true, other is false.
 **/
extern eris_bool_t eris_http_request_keep_alive( eris_http_t *__http);


/**
 * @Brief: The response connection is keep alive.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Keep-alive is true, other is false.
 **/
extern eris_bool_t eris_http_response_keep_alive( eris_http_t *__http);


/**
 * @Brief: Check request entity content is x-www-form-urlencoded format.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Yes is true, other is false.
 **/
extern eris_bool_t eris_http_request_is_x_www_form_urlencoded( eris_http_t *__http);


/**
 * @Brief: Check response entity content is x-www-form-urlencoded format.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Yes is true, other is false.
 **/
extern eris_bool_t eris_http_response_is_x_www_form_urlencoded( eris_http_t *__http);


/**
 * @Brief: Check request entity content is json format.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Yes is true, other is false.
 **/
extern eris_bool_t eris_http_request_is_json( eris_http_t *__http);


/**
 * @Brief: Check response entity content is json format.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Yes is true, other is false.
 **/
extern eris_bool_t eris_http_response_is_json( eris_http_t *__http);


/**
 * @Brief: Check http context is chunked.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: It is has chunked data is true, Other is false.
 **/
extern eris_bool_t eris_http_is_chunked( eris_http_t *__http);


/**
 * @Brief: Check http context parse is chunked ok.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: It is parse chunked data end is true, Other is false.
 **/
extern eris_bool_t eris_http_is_chunked_end( eris_http_t *__http);


/**
 * @Brief: Parse chunk data.
 *
 * @Param: __http,     Eris http context.
 * @Param: __type,     ERIS_HTTP_REQUEST or ERIS_HTTP_RESPONSE.
 * @Param: __inbuffer, Input data buffer.
 *
 * @Return: Ok is 0, Internal error is EERIS_ERROR, Other request data invalid is 1.
 **/
extern eris_int_t eris_http_chunk_parse( eris_http_t *__http, eris_http_type_t __type, const eris_buffer_t *__inbuffer);


/**
 * @Brief: Set request command, if user is client.
 *
 * @Param: __http,    Eris http context.
 * @Param: __command, Specify request command.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_http_request_set_command( eris_http_t *__http, eris_http_command_t __command);


/**
 * @Brief: Set request version, if user is client.
 *
 * @Param: __http,    Eris http context.
 * @Param: __version, Specify version.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_http_request_set_version( eris_http_t *__http, eris_http_version_t __version);


/**
 * @Brief: Set request url, if user is client.
 *
 * @Param: __http,  Eris http context.
 * @Param: __url,   Url string.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_http_request_set_url( eris_http_t *__http, const eris_char_t *__url);


/**
 * @Brief: Set request parameters , if user is client.
 *
 * @Param: __http,  Eris http context.
 * @Param: __parameters, If GET command specify and set.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_http_request_set_parameters( eris_http_t *__http, const eris_char_t *__parameters);


/**
 * @Brief: Set request query, if user is client.
 *
 * @Param: __http,  Eris http context.
 * @Param: __query, If GET command specify and set, default is NULL.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_http_request_set_query( eris_http_t *__http, const eris_char_t *__query);


/**
 * @Brief: Set request fragment, if user is client.
 *
 * @Param: __http,  Eris http context.
 * @Param: __fragment, If GET command specify and set, default is nothing.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_http_request_set_fragment( eris_http_t *__http, const eris_char_t *__fragment);


/**
 * @Brief: Set http header name-value, if user is client.
 *
 * @Param: __http,  Eris http context.
 * @Param: __name,  HTTP protocol header name.
 * @Param: __value, HTTP protocol header value. 
 * 
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_http_request_set_header( eris_http_t *__http, const eris_char_t *__name, const eris_char_t *__value);


/**
 * @Brief: Set http request body, if user is client.
 *
 * @Param: __http, Eris http context.
 * @Param: __body, Body data.
 * @Param: __size, Body data size.
 * 
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_http_request_set_body( eris_http_t *__http, const eris_void_t *__body, eris_size_t __size);


/**
 * @Brief: Set http request body and append, if user is client.
 *
 * @Param: __http, Eris http context.
 * @Param: __body, Body data.
 * @Param: __size, Body data size.
 * 
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_http_request_set_body_append( eris_http_t *__http, const eris_void_t *__body, eris_size_t __size);


/**
 * @Brief: Get request command, if user is server and after parse request package.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Eris http commad.
 **/
extern eris_http_command_t eris_http_request_get_command( const eris_http_t *__http);


/**
 * @Brief: Get request version, if user is server and after parse request package.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Eris http version.
 **/
extern eris_http_version_t eris_http_request_get_version( const eris_http_t *__http);


/**
 * @Brief: Get request url, if user is server and after parse request package.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Url string.
 **/
extern const eris_string_t eris_http_request_get_url( const eris_http_t *__http);


/**
 * @Brief: Get request parameters, if user is server and after parse request package.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Parameters string.
 **/
extern const eris_string_t eris_http_request_get_parameters( const eris_http_t *__http);


/**
 * @Brief: Get request query, if user is server and after parse request package.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Qeury string.
 **/
extern const eris_string_t eris_http_request_get_query( const eris_http_t *__http);


/**
 * @Brief: Get request fragment, if user is server and after parse request package.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Fragment string.
 **/
extern const eris_string_t eris_http_request_get_fragment( const eris_http_t *__http);


/**
 * @Brief: Get http header name-value, if user is server.
 *
 * @Param: __http,  Eris http context.
 * @Param: __name,  HTTP protocol header name.
 * 
 * @Return: Name -> value string.
 **/
extern const eris_string_t eris_http_request_get_header( const eris_http_t *__http, const eris_char_t *__name);


/**
 * @Brief: Get http request body buffer object, if user is server.
 *
 * @Param: __http, Eris http context.
 * 
 * @Return: Body buffer object.
 **/
extern const eris_buffer_t *eris_http_request_get_body( const eris_http_t *__http);


/**
 * @Brief: Get http request body data, if user is server.
 *
 * @Param: __http, Eris http context.
 * 
 * @Return: Body data.
 **/
extern const eris_void_t *eris_http_request_get_body_data( const eris_http_t *__http);


/**
 * @Brief: Get http request body size, if user is server.
 *
 * @Param: __http, Eris http context.
 * 
 * @Return: Body data size.
 **/
extern eris_size_t eris_http_request_get_body_size( const eris_http_t *__http);


/**
 * @Brief: Set response version, if user is server.
 *
 * @Param: __http,    Eris http context.
 * @Param: __version, Specify version.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_http_response_set_version( eris_http_t *__http, eris_http_version_t __version);


/**
 * @Brief: Set response status, if user is server.
 *
 * @Param: __http,   Eris http context.
 * @Param: __status, Specify status code.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_http_response_set_status( eris_http_t *__http, eris_http_status_t __status);


/**
 * @Brief: Set response reason phrace, if user is server.
 *
 * @Param: __http,   Eris http context.
 * @Param: __reason, Reason phrace string.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_http_response_set_reason( eris_http_t *__http, const eris_char_t *__reason);


/**
 * @Brief: Set http header name-value, if user is server.
 *
 * @Param: __http,  Eris http context.
 * @Param: __name,  HTTP protocol header name.
 * @Param: __value, HTTP protocol header value. 
 * 
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_http_response_set_header( eris_http_t *__http, const eris_char_t *__name, const eris_char_t *__value);


/**
 * @Brief: Set http response body, if user is server.
 *
 * @Param: __http, Eris http context.
 * @Param: __body, Body data.
 * @Param: __size, Body data size.
 * 
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_http_response_set_body( eris_http_t *__http, const eris_void_t *__body, eris_size_t __size);


/**
 * @Brief: Set http response body and append, if user is server.
 *
 * @Param: __http, Eris http context.
 * @Param: __body, Body data.
 * @Param: __size, Body data size.
 * 
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_http_response_set_body_append( eris_http_t *__http, const eris_void_t *__body, eris_size_t __size);


/**
 * @Brief: Get response version, if user is client.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Version value.
 **/
extern eris_http_version_t eris_http_response_get_version( const eris_http_t *__http);


/**
 * @Brief: Get response status, if user is client.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: response status.
 **/
extern eris_http_status_t eris_http_response_get_status( const eris_http_t *__http);


/**
 * @Brief: Get response reason phrace, if user is client.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Reason phrace string.
 **/
extern const eris_string_t eris_http_response_get_reason( const eris_http_t *__http);


/**
 * @Brief: Get http header name-value, if user is client.
 *
 * @Param: __http,  Eris http context.
 * @Param: __name,  HTTP protocol header name.
 * 
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern const eris_string_t eris_http_response_get_header( const eris_http_t *__http, const eris_char_t *__name);


/**
 * @Brief: Get http response body object, if user is client
 *
 * @Param: __http, Eris http context.
 * 
 * @Return: Body buffer object.
 **/
extern const eris_buffer_t *eris_http_response_get_body( const eris_http_t *__http);


/**
 * @Brief: Get http response body data, if user is client
 *
 * @Param: __http, Eris http context.
 * 
 * @Return: Body data.
 **/
extern const eris_void_t *eris_http_response_get_body_data( const eris_http_t *__http);


/**
 * @Brief: Get http response body size, if user is client
 *
 * @Param: __http, Eris http context.
 * 
 * @Return: Body data size.
 **/
extern eris_size_t eris_http_response_get_body_size( const eris_http_t *__http);


/**
 * @Brief: Clean up eris http context.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_http_cleanup( eris_http_t *__http);


/**
 * @Brief: Clean up eris http request handler
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_http_cleanup_request( eris_http_t *__http);


/**
 * @Brief: Clean up eris http response
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_http_cleanup_response( eris_http_t *__http);


/**
 * @Brief: Check status is valid.
 *
 * @Parma: __status, Input status code.
 *
 * @Return: Is valid is true, Other is false.
 **/
extern eris_bool_t eris_http_status_valid( eris_int_t __status);


/**
 * @Brief: Get status string
 *
 * @Param: __status, Status code.
 *
 * @Return: status string.
 **/
extern const eris_char_t *eris_http_status_string( eris_int_t __status);


/**
 * @Brief: Get status reason phrace.
 *
 * @Param: __status, Status code.
 *
 * @Return: reason phrace string.
 **/
extern const eris_char_t *eris_http_status_reason_phrace( eris_int_t __status);


/**
 * @Brief: Destroy eris http context.
 *
 * @Param: __http, Eris http context.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_http_destroy( eris_http_t *__http);



#ifdef __cplusplus
}
#endif

#endif /** __ERIS_HTTP_H__ */


