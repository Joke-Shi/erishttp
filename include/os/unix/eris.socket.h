#ifndef __ERIS_SOCKET_H__
#define __ERIS_SOCKET_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief: 
 **       : Socket operator.
 **
 ******************************************************************************/

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.core.h"


#ifdef __cplusplus
extern "C" {
#endif


/** IPv4 address */
typedef struct eris_socket_host_s eris_socket_host_t;
struct eris_socket_host_s {
    eris_char_t ipv4[32];           /** IPv4 address */
    eris_int_t  port;               /** Port number  */
};


/** Eris socket attributes */
typedef struct eris_socket_attr_s eris_socket_attr_t;
struct eris_socket_attr_s {
    eris_uint32_t nonblock   :1;    /** Nonblocking   */
    eris_uint32_t backlog    :16;   /** Backlog size  */
    eris_uint32_t tcp_nodelay:1;    /** 1 or 0 enable */
    eris_uint32_t tcp_nopush :1;    /** 1 or 0 enable */
    eris_uint32_t linger     :8;    /** Second close  */
    eris_uint32_t zero       :7;    /** Zero          */
    eris_int_t    sendbuf_size;     /** Send buf size */
    eris_int_t    recvbuf_size;     /** Recv buf size */
};



/**
 * @Brief: Create TCP protocol socket context.
 *
 * @Param: Nothing.
 *
 * @Return: Ok->Socket context, Other->EERIS_ERROR.
 **/
extern eris_sock_t eris_socket_tcp( void);


/**
 * @Brief: Create UDP protocol socket context.
 *
 * @Param: Nothing.
 *
 * @Return: Ok->Socket context, Other->EERIS_ERROR.
 **/
extern eris_sock_t eris_socket_udp( void);


/**
 * @Brief: Create local protocol socket context.
 *
 * @Param: Nothing.
 *
 * @Return: Ok->Socket context, Other->EERIS_ERROR.
 **/
extern eris_sock_t eris_socket_local( void);


/**
 * @Brief: Set socket blocking.
 *
 * @Param: __sock, Socket context.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
extern eris_int_t eris_socket_blocking( eris_sock_t __sock);


/**
 * @Brief: Set socket nonblocking.
 *
 * @Param: __sock, Socket context.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
extern eris_int_t eris_socket_nonblocking( eris_sock_t __sock);


/**
 * @Brief: Enable TCP_NODELAY.
 *
 * @Param: __sock, Socket context.
 *
 * @Return: Ok->0, Other->-1.
 **/
extern eris_int_t eris_socket_tcp_nodelay( eris_sock_t __sock);


/**
 * @Brief: Disable TCP_PUSH or TCP_CORK.
 *
 * @Param: __sock, Socket context.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
extern eris_int_t eris_socket_tcp_nopush( eris_sock_t __sock);


/**
 * @Brief: Set linger timeout.
 *
 * @Param: __sock,   Socket context.
 * @Param: __linger, Linger value.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
extern eris_int_t eris_socket_linger( eris_sock_t __sock, eris_int_t __linger);


/**
 * @Brief: Disable linger timeout.
 *
 * @Param: __sock, Socket context.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
extern eris_int_t eris_socket_nolinger( eris_sock_t __sock);


/**
 * @Brief: Set reuse address with socket context.
 *
 * @Param: __sock, Socket context.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
extern eris_int_t eris_socket_reuseaddr( eris_sock_t __sock);


/**
 * @Brief: Set socket receive buffer size.
 *
 * @Param: __sock, Socket context.
 * @Param: __size, Buffer size, default 8k.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
extern eris_int_t eris_socket_set_recvbuf( eris_sock_t __sock, eris_int_t __size);


/**
 * @Brief: Set socket send buffer size.
 *
 * @Param: __sock, Socket context.
 * @Param: __size, Buffer size, default 8k.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
extern eris_int_t eris_socket_set_sendbuf( eris_sock_t __sock, eris_int_t __size);


/**
 * @Brief: Check socket object has ready data read.
 *
 * @Param: __sock, Socket context.
 * @Param: __timeout, Second timeout.
 *
 * @Return: Ok is 1, timeout is 0, other is -1.
 **/
extern eris_int_t eris_socket_ready_r( eris_sock_t __sock, eris_int_t __timeout);


/**
 * @Brief: Check socket object has ready output.
 *
 * @Param: __sock, Socket context.
 * @Param: __timeout, Second timeout.
 *
 * @Return: Ok is 1, timeout is 0, other is -1.
 **/
extern eris_int_t eris_socket_ready_w( eris_sock_t __sock, eris_int_t __timeout);


/**
 * @Brief: Close socket context.
 *
 * @Param: __sock, Socket context.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
extern eris_int_t eris_socket_close( eris_sock_t __sock);


/**
 * @Brief: Close socket context read channel.
 *
 * @Param: __sock, Socket context.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
extern eris_int_t eris_socket_close_r( eris_sock_t __sock);


/**
 * @Brief: Close socket context write channel.
 *
 * @Param: __sock, Socket context.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
extern eris_int_t eris_socket_close_w( eris_sock_t __sock);


/**
 * @Brief: Create tcp socket and bind listen.
 *
 * @Param: __listen_es, Listen eris string, default: "127.0.0.1:9432".
 * @Param: __attrs,     Socket listen set attributes..
 * @Param: __log,       Dump log message context.
 *
 * @Return: Ok is socket fd object, Other is -1.
 **/
extern eris_sock_t eris_socket_tcp_listen( const eris_string_t __listen_es, eris_socket_attr_t *__attrs, eris_log_t *__log);


/**
 * @Brief: Create tcp socket connect..
 *
 * @Param: __conn_es, Connect service eris string, default: "127.0.0.1:9432".
 * @Param: __attrs,   Socket connect set attributes..
 * @Param: __log,     Dump log message context.
 *
 * @Return: Ok is socket fd object, Other is -1.
 **/
extern eris_sock_t eris_socket_tcp_connect( const eris_string_t __conn_es, eris_socket_attr_t *__attrs, eris_log_t *__log);


/**
 * @Brief: Get IPv4 addresss of socket object.
 *
 * @Param: __sock, Socket object.
 * @Param: __host, Output ipv4 and port.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_socket_host( eris_sock_t __sock, eris_socket_host_t *__host);



#ifdef __cplusplus
}
#endif /** __cplusplus */


#endif /** __ERIS_SOCKET_H__ */


