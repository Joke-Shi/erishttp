/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief: 
 **       : Socket operator.
 **
 ******************************************************************************/

#include "os/unix/eris.socket.h"



/** Parse listen eris string. */
static eris_int_t eris_socket_listen_parse( 
    const eris_string_t __listen_es,
    eris_string_t      *__host_es,
    eris_string_t      *__port_es,
    eris_log_t         *__log
);



/**
 * @Brief: Create TCP protocol socket context.
 *
 * @Param: Nothing.
 *
 * @Return: Ok->Socket context, Other->-1.
 **/
eris_sock_t eris_socket_tcp( void)
{
    /** TCP socket */
    return (eris_sock_t )socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
}/// eris_socket_tcp


/**
 * @Brief: Create UDP protocol socket context.
 *
 * @Param: Nothing.
 *
 * @Return: Ok->Socket context, Other->-1.
 **/
eris_sock_t eris_socket_udp( void)
{
    /** UDP socket */
    return (eris_sock_t )socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}/// eris_socket_udp


/**
 * @Brief: Create local protocol socket context.
 *
 * @Param: Nothing.
 *
 * @Return: Ok->Socket context, Other->-1.
 **/
eris_sock_t eris_socket_local( void)
{
    /** Local socket */
#if ERIS_HAVE_AF_LOCAL
    return (eris_sock_t )socket( AF_LOCAL, SOCK_STREAM, 0);
#else
    return (eris_sock_t )socket( AF_UNIX,  SOCK_STREAM, 0); 
#endif
}/// eris_socket_local


/**
 * @Brief: Set socket blocking.
 *
 * @Param: __sock, Socket context.
 *
 * @Return: Ok->0, Other->-1.
 **/
eris_int_t eris_socket_blocking( eris_sock_t __sock)
{
    eris_int_t rc = 0;

    if ( 0 <= __sock) {
#if ERIS_HAVE_SYS_FILEIO_H
        eris_ulong_t flag = 0;

        rc = (eris_int_t )ioctl( __sock, FIONBIO, &flag);
#else
        (eris_none_t )fcntl( __sock, F_SETFL, 0);

#endif
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_socket_blocking


/**
 * @Brief: Set socket nonblocking.
 *
 * @Param: __sock, Socket context.
 *
 * @Return: Ok->0, Other->-1.
 **/
eris_int_t eris_socket_nonblocking( eris_sock_t __sock)
{
    eris_int_t rc = 0;

    if ( 0 <= __sock) {
#if ERIS_HAVE_SYS_FILEIO_H
        eris_ulong_t flag = 1;

        rc = (eris_int_t )ioctl( __sock, FIONBIO, &flag);
#else
        eris_int_t flags = fcntl( __sock, F_GETFL);
        if ( -1 != flags) {
            flags = (eris_int_t )fcntl( __sock, F_SETFL, flags & O_NONBLOCK);
            if ( -1 == flags) {
                rc = EERIS_ERROR;
            }
        }
#endif
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_socket_nonblocking


/**
 * @Brief: Enable TCP_NODELAY.
 *
 * @Param: __sock, Socket context.
 *
 * @Return: Ok->0, Other->-1.
 **/
eris_int_t eris_socket_tcp_nodelay( eris_sock_t __sock)
{
    eris_int_t rc = 0;

    if ( 0 <= __sock) {
        int flag = 1;

#if defined(__linux__) || (__linux)
        rc = (eris_int_t )setsockopt( __sock, IPPROTO_TCP, TCP_NODELAY, (const void *)&flag, sizeof( int));

#elif defined(__FreeBSD__) || defined(__DragonFly__) 
        rc = (eris_int_t )setsockopt( __sock, IPPROTO_TCP, TCP_NODELAY, (const void *)&flag, sizeof( int));

#elif defined(__Darwin__) || defined( __APPLE__)
        rc = (eris_int_t )setsockopt( __sock, IPPROTO_TCP, TCP_NODELAY, (const void *)&flag, sizeof( int));

#else
    /** Nothing */
#endif
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_socket_tcp_nodelay


/**
 * @Brief: Enable TCP_NOPUSH or TCP_CORK.
 *
 * @Param: __sock, Socket context.
 *
 * @Return: Ok->0, Other->-1.
 **/
eris_int_t eris_socket_tcp_nopush( eris_sock_t __sock)
{
    eris_int_t rc = 0;

    if ( 0 <= __sock) {
        int flag = 1;

#if defined(__linux__) || (__linux)
        rc = (eris_int_t )setsockopt( __sock, IPPROTO_TCP, TCP_CORK, (const void *)&flag, sizeof( int));

#elif defined(__FreeBSD__) || defined(__DragonFly__) 
        rc = (eris_int_t )setsockopt( __sock, IPPROTO_TCP, TCP_NOPUSH, (const void *)&flag, sizeof( int));

#elif defined(__Darwin__) || defined( __APPLE__)
        rc = (eris_int_t )setsockopt( __sock, IPPROTO_TCP, TCP_NOPUSH, (const void *)&flag, sizeof( int));

#else
    /** Nothing */
#endif
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_socket_tcp_nopush


/**
 * @Brief: Set linger timeout.
 *
 * @Param: __sock,   Socket context.
 * @Param: __linger, Linger value.
 *
 * @Return: Ok->0, Other->-1.
 **/
eris_int_t eris_socket_linger( eris_sock_t __sock, eris_int_t __linger)
{
    eris_int_t rc = 0;

    if ( 0 <= __sock) {
        /** Init linger */
        struct linger lg = {
            .l_onoff  = 1,
            .l_linger = __linger
        };

        rc = (eris_int_t )setsockopt( __sock, SOL_SOCKET, SO_LINGER, (const void *)&lg, sizeof( struct linger));
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_socket_linger


/**
 * @Brief: Disable linger timeout.
 *
 * @Param: __sock, Socket context.
 *
 * @Return: Ok->0, Other->-1.
 **/
eris_int_t eris_socket_nolinger( eris_sock_t __sock)
{
    eris_int_t rc = 0;

    if ( 0 <= __sock) {
        /** Init linger */
        struct linger lg = {
            .l_onoff  = 0,
            .l_linger = 0 
        };

        rc = (eris_int_t )setsockopt( __sock, SOL_SOCKET, SO_LINGER, (const void *)&lg, sizeof( struct linger));
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_socket_linger


/**
 * @Brief: Set reuse address with socket context.
 *
 * @Param: __sock, Socket context.
 *
 * @Return: Ok->0, Other->-1.
 **/
eris_int_t eris_socket_reuseaddr( eris_sock_t __sock)
{
    eris_int_t rc = 0;

    if ( 0 <= __sock) {
        int enable = 1;

        rc = (eris_int_t )setsockopt( __sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&enable, sizeof( int));
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_socket_reuseaddr


/**
 * @Brief: Set socket receive buffer size.
 *
 * @Param: __sock, Socket context.
 * @Param: __size, Buffer size, default 8k.
 *
 * @Return: Ok->0, Other->-1.
 **/
eris_int_t eris_socket_set_recvbuf( eris_sock_t __sock, eris_int_t __size)
{
    eris_int_t rc = 0;

    if ( 0 <= __sock) {
        int def_size = 8192;
        if ( 0 < __size) {
            def_size = (int )__size;
        }

        rc = (eris_int_t )setsockopt( __sock, SOL_SOCKET, SO_RCVBUF, (const void *)&def_size, sizeof( int));
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_socket_set_recvbuf


/**
 * @Brief: Set socket send buffer size.
 *
 * @Param: __sock, Socket context.
 * @Param: __size, Buffer size, default 8k.
 *
 * @Return: Ok->0, Other->-1.
 **/
eris_int_t eris_socket_set_sendbuf( eris_sock_t __sock, eris_int_t __size)
{
    eris_int_t rc = 0;

    if ( 0 <= __sock) {
        int def_size = 8192;
        if ( 0 < __size) {
            def_size = (int )__size;
        }

        rc = (eris_int_t )setsockopt( __sock, SOL_SOCKET, SO_SNDBUF, (const void *)&def_size, sizeof( int));
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_socket_set_sendbuf


/**
 * @Brief: Check socket object has ready data read.
 *
 * @Param: __sock, Socket context.
 * @Param: __timeout, Second timeout.
 *
 * @Return: Ok is 1, timeout is 0, other is -1.
 **/
eris_int_t eris_socket_ready_r( eris_sock_t __sock, eris_int_t __timeout)
{
    eris_int_t rc = 0;

    if ( 0 <= __sock) {
        fd_set read_set;

        FD_ZERO( &read_set);

        FD_SET ( __sock, &read_set);

        eris_timeval_t tv; {
            tv.tv_sec  = __timeout;
            tv.tv_usec = 0;
        }

        eris_int_t fds_n = select( __sock + 1, &read_set, NULL, NULL, &tv);
        if ( 0 < fds_n) {
            if ( FD_ISSET( __sock, &read_set)) {
                rc = 1;
            }
        } else if ( 0 == fds_n) {
            rc = 0;

        } else {
            rc = -1;
        }
    }

    return rc;
}/// eris_socket_ready_r


/**
 * @Brief: Check socket object has ready output.
 *
 * @Param: __sock, Socket context.
 * @Param: __timeout, Second timeout.
 *
 * @Return: Ok is 1, timeout is 0, other is -1.
 **/
eris_int_t eris_socket_ready_w( eris_sock_t __sock, eris_int_t __timeout)
{
    eris_int_t rc = 0;

    if ( 0 <= __sock) {
        fd_set write_set;

        FD_ZERO( &write_set);

        FD_SET ( __sock, &write_set);

        eris_timeval_t tv; {
            tv.tv_sec  = __timeout;
            tv.tv_usec = 0;
        }

        eris_int_t fds_n = select( __sock + 1, NULL, &write_set, NULL, &tv);
        if ( 0 < fds_n) {
            if ( FD_ISSET( __sock, &write_set)) {
                rc = 1;
            }

        } else if ( 0 == fds_n) {
            rc = 0;

        } else {
            rc = -1;
        }
    }

    return rc;
}/// eris_socket_ready_w


/**
 * @Brief: Close socket context.
 *
 * @Param: __sock, Socket context.
 *
 * @Return: Ok->0, Other->-1.
 **/
eris_int_t eris_socket_close( eris_sock_t __sock)
{
    eris_int_t rc = 0;
    if ( __sock) {
        rc = (eris_int_t )close( __sock);

    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_socket_close


/**
 * @Brief: Close socket context read channel.
 *
 * @Param: __sock, Socket context.
 *
 * @Return: Ok->0, Other->-1.
 **/
eris_int_t eris_socket_close_r( eris_sock_t __sock)
{
    eris_int_t rc = 0;

    if ( 0 <= __sock) {
        /** Close read anble */
        rc = (eris_int_t )shutdown( __sock, SHUT_RD);

    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_socket_close_r


/**
 * @Brief: Close socket context write channel.
 *
 * @Param: __sock, Socket context.
 *
 * @Return: Ok->0, Other->-1.
 **/
eris_int_t eris_socket_close_w( eris_sock_t __sock)
{
    eris_int_t rc = 0;

    if ( 0 <= __sock) {
        /** Close write anble */
        rc = (eris_int_t )shutdown( __sock, SHUT_WR);

    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_socket_close_w


/**
 * @Brief: Create tcp socket and bind listen.
 *
 * @Param: __listen_es, Listen eris string, default: "127.0.0.1:9432".
 * @Param: __attrs,     Socket listen set attributes..
 * @Param: __log,       Dump log message context.
 *
 * @Return: Ok is socket fd object, Other is -1.
 **/
eris_sock_t eris_socket_tcp_listen( const eris_string_t __listen_es, eris_socket_attr_t *__attrs, eris_log_t *__log)
{
    eris_int_t  tmp_errno = errno;
    eris_int_t  rc      = 0;
    eris_sock_t sock_fd = -1;

    eris_string_t host; eris_string_init( host);
    eris_string_t port; eris_string_init( port);

    /** Init attributes */
    eris_socket_attr_t attrs;
    if ( __attrs) {
        attrs = *__attrs;
        
        if ( 0 == attrs.backlog) {
            attrs.backlog = 64;
        }
    } else {
        attrs.nonblock     = 1 ;
        attrs.backlog      = 64;
        attrs.tcp_nodelay  = 0 ;
        attrs.tcp_nopush   = 0 ;
        attrs.linger       = 0 ;
        attrs.zero         = 0 ;
        attrs.sendbuf_size = -1;
        attrs.recvbuf_size = -1;
    }

    /** Parse listen "127.0.0.1:9432" string */
    rc = eris_socket_listen_parse( __listen_es, &host, &port, __log);
    if ( 0 == rc) {
        if ( !host || (0 == eris_string_size( host)) ) { 
            eris_string_set( &host, "0.0.0.0"); 
        }

        if ( !port || (0 == eris_string_size( port)) ) { 
            eris_string_set( &port, "9432");
        }

#if (ERIS_HAVE_GETADDRINFO)
        struct addrinfo  ht;
        struct addrinfo *rp     = NULL;
        struct addrinfo *result = NULL;
        eris_memory_cleanup( &ht, sizeof( struct addrinfo));
        {
            ht.ai_flags     = AI_PASSIVE | AI_NUMERICSERV;
            ht.ai_family    = AF_INET;
            ht.ai_socktype  = SOCK_STREAM;
            ht.ai_protocol  = 0;
            ht.ai_addrlen   = 0;
            ht.ai_canonname = NULL;
            ht.ai_addr      = NULL;
            ht.ai_next      = NULL;
        }

        rc = getaddrinfo( host, port, &ht, &result);
        if ( 0 == rc) {
            for ( rp = result; (NULL != rp); rp = rp->ai_next) {
#if (ERIS_LINIX_OS)
                sock_fd = socket( rp->ai_family, rp->ai_socktype | SOCK_NONBLOCK, rp->ai_protocol);
#else
                sock_fd = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol);
#endif
                if ( -1 != sock_fd) {
                    /** Set socket options */
                    eris_socket_reuseaddr( sock_fd);

                    if ( 1 == attrs.nonblock) {
                        eris_socket_nonblocking( sock_fd);
                    }

                    if ( 1 == attrs.tcp_nodelay) {
                        eris_socket_tcp_nodelay( sock_fd);
                    } else {
                        if ( 1 == attrs.tcp_nopush) {
                            eris_socket_tcp_nopush( sock_fd);
                        }
                    }

                    if ( 0 != attrs.linger) {
                        eris_socket_linger( sock_fd, (eris_int_t )attrs.linger);
                    }

                    /** Try bind */
                    rc = bind( sock_fd, rp->ai_addr, rp->ai_addrlen);
                    if ( 0 == rc) {
                        rc = listen( sock_fd, (eris_int_t )attrs.backlog);
                        if ( 0 == rc) {
                            /** Successed */
                            break;

                        } else {
                            eris_socket_close( sock_fd); sock_fd = -1;
                        }
                    } else {
                        eris_socket_close( sock_fd); sock_fd = -1;
                    }
                } else {
                    if ( __log) {
                        eris_log_dump( __log, ERIS_LOG_ERROR, "Create socket of tcp protocol failed, errno.<%d>", errno);
                    }
                }
            }/// for 

            /** Release addrinfo */
            freeaddrinfo( result);
        } else {
            if ( __log) {
                eris_log_dump( __log, ERIS_LOG_ERROR, "Get host by name failed, error.<%s> - errno.<%d>", gai_strerror( errno), errno);
            }
        }
#else
        extern int h_errno;
        struct hostent *ph = gethostbyname( host);
        if ( ph) {
            rc = -1;

            eris_char_t ipv4_host[ INET_ADDRSTRLEN] = {0};

            const char *pph = ph->h_addr_list;
            for ( ; (NULL != *pph); pph++) {
                switch ( ph->h_addrtype) {
                    case AF_INET :
                        if ( inet_ntop( ph->h_addrtype, *pph, ipv4_host, sizeof( ipv4_host))) {
                            rc = 0;

                            /** Ok */
                            break;
                        } break;
                    default : { rc = -1; } break;
                }

                if ( 0 == rc) { break; }
            }

            /** Ok, and create tcp socket */
            if ( 0 == rc) {
                sock_fd = eris_socket_tcp();
                if ( -1 != sock_fd) {
                    eris_int_t svc_port = eris_string_atoi( port);
                    if ( (0 >= svc_port) || (65535 <= svc_port)) {
                        svc_port = 9432;
                    }

                    struct sockaddr_in svc_addr; {
                        svc_addr.sin_family = AF_INET;
                        svc_addr.sin_port   = htons( svc_port);
                        svc_addr.sin_addr.s_addr = inet_addr( ipv4_host);
                    }

                    /** Set socket options */
                    eris_socket_reuseaddr( sock_fd);

                    if ( 1 == attrs.nonblock) {
                        eris_socket_nonblocking( sock_fd);
                    }

                    if ( 1 == attrs.tcp_nodelay) {
                        eris_socket_tcp_nodelay( sock_fd);
                    } else {
                        if ( 1 == attrs.tcp_nopush) {
                            eris_socket_tcp_nopush( sock_fd);
                        }
                    }

                    if ( 0 != attrs.linger) {
                        eris_socket_linger( sock_fd, (eris_int_t )attrs.linger);
                    }

                    /** Try bind */
                    rc = bind( sock_fd, (struct sockaddr *)&svc_addr, sizeof( struct sockaddr_in));
                    if ( 0 == rc) {
                        rc = listen( sock_fd, (eris_int_t )attrs.backlog);
                        if ( 0 == rc) {
                            /** Successed */
                            break;

                        } else {
                            eris_socket_close( sock_fd); sock_fd = -1;
                        }
                    } else {
                        eris_socket_close( sock_fd); sock_fd = -1;
                    }
                } else {
                    if ( __log) {
                        eris_log_dump( __log, ERIS_LOG_ERROR, "Create socket of tcp protocol failed, errno.<%d>", errno);
                    }
                }
            } else {
                if ( __log) {
                    eris_log_dump( __log, ERIS_LOG_ERROR, "Host.<%s> is invalid", host);
                }
            }
        } else {
            if ( __log) {
                eris_log_dump( __log, ERIS_LOG_ERROR, "Get host by name failed, error.<%s> - errno.<%d>", hstrerror( h_errno), h_errno);
            }
        }
#endif
    }

    eris_string_free( host); eris_string_init( host);
    eris_string_free( port); eris_string_init( port);

    errno = tmp_errno;

    return sock_fd;
}/// eris_socket_tcp_listen


/**
 * @Brief: Create tcp socket connect..
 *
 * @Param: __conn_es, Connect service eris string, default: "127.0.0.1:9432".
 * @Param: __attrs,   Socket connect set attributes..
 * @Param: __log,     Dump log message context.
 *
 * @Return: Ok is socket fd object, Other is -1.
 **/
eris_sock_t eris_socket_tcp_connect( const eris_string_t __conn_es, eris_socket_attr_t *__attrs, eris_log_t *__log)
{
    eris_int_t  tmp_errno = errno;
    eris_int_t  rc      = 0;
    eris_sock_t sock_fd = -1;

    eris_string_t host; eris_string_init( host);
    eris_string_t port; eris_string_init( port);

    /** Init attributes */
    eris_socket_attr_t attrs;
    if ( __attrs) {
        attrs = *__attrs;
        
        if ( 0 == attrs.backlog) {
            attrs.backlog = 64;
        }
    } else {
        attrs.nonblock     = 1 ;
        attrs.backlog      = 64;
        attrs.tcp_nodelay  = 0 ;
        attrs.tcp_nopush   = 0 ;
        attrs.linger       = 0 ;
        attrs.zero         = 0 ;
        attrs.sendbuf_size = -1;
        attrs.recvbuf_size = -1;
    }

    /** Parse listen "127.0.0.1:9432" string */
    rc = eris_socket_listen_parse( __conn_es, &host, &port, __log);
    if ( 0 == rc) {
        if ( !host || (0 == eris_string_size( host)) ) { 
            eris_string_set( &host, "0.0.0.0"); 
        }

        if ( !port || (0 == eris_string_size( port)) ) { 
            eris_string_set( &port, "9432");
        }

#if (ERIS_HAVE_GETADDRINFO)
        struct addrinfo  ht;
        struct addrinfo *rp     = NULL;
        struct addrinfo *result = NULL;
        eris_memory_cleanup( &ht, sizeof( struct addrinfo));
        {
            ht.ai_flags     = AI_NUMERICSERV;
            ht.ai_family    = AF_INET;
            ht.ai_socktype  = SOCK_STREAM;
            ht.ai_protocol  = 0;
            ht.ai_addrlen   = 0;
            ht.ai_canonname = NULL;
            ht.ai_addr      = NULL;
            ht.ai_next      = NULL;
        }

        rc = getaddrinfo( host, port, &ht, &result);
        if ( 0 == rc) {
            for ( rp = result; (NULL != rp); rp = rp->ai_next) {
                sock_fd = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol);
                if ( -1 != sock_fd) {
                    /** Set socket options */
                    eris_socket_reuseaddr( sock_fd);

                    if ( 1 == attrs.nonblock) {
                        eris_socket_nonblocking( sock_fd);
                    }

                    if ( 1 == attrs.tcp_nodelay) {
                        eris_socket_tcp_nodelay( sock_fd);
                    } else {
                        if ( 1 == attrs.tcp_nopush) {
                            eris_socket_tcp_nopush( sock_fd);
                        }
                    }

                    if ( 0 != attrs.linger) {
                        eris_socket_linger( sock_fd, (eris_int_t )attrs.linger);
                    }

                    /** Set send and recv buffer size */
                    eris_socket_set_recvbuf( sock_fd, attrs.recvbuf_size);
                    eris_socket_set_sendbuf( sock_fd, attrs.sendbuf_size);

                    /** Try bind */
                    rc = connect( sock_fd, rp->ai_addr, rp->ai_addrlen);
                    if ( 0 == rc) {
                        /** Successed */
                        break;

                    } else {
                        if ( errno == EINPROGRESS) {
                            /** Nothing */
                            eris_int_t     error_flag = 1;
                            eris_socklen_t error_flag_size = sizeof( eris_int_t);
                            (eris_none_t )getsockopt( sock_fd,  SOL_SOCKET, SO_ERROR ,&error_flag, &error_flag_size);

                        } else {
                            eris_socket_close( sock_fd); sock_fd = -1;

                            if ( __log) {
                                eris_log_dump( __log, ERIS_LOG_ERROR, "Connect to server failed, errno.<%d>", errno);
                            }
                        }
                    }
                } else {
                    if ( __log) {
                        eris_log_dump( __log, ERIS_LOG_ERROR, "Create socket of tcp protocol failed, errno.<%d>", errno);
                    }
                }
            }

            /** Release addrinfo */
            freeaddrinfo( result);
        } else {
            if ( __log) {
                eris_log_dump( __log, ERIS_LOG_ERROR, "Get addrinfo failed, error.<%s> - errno.<%d>", gai_strerror( errno), errno);
            }
        }
#else
        extern int h_errno;
        struct hostent *ph = gethostbyname( host);
        if ( ph) {
            rc = -1;

            eris_char_t ipv4_host[ INET_ADDRSTRLEN] = {0};

            const char *pph = ph->h_addr_list;
            for ( ; (NULL != *pph); pph++) {
                switch ( ph->h_addrtype) {
                    case AF_INET :
                        if ( inet_ntop( ph->h_addrtype, *pph, ipv4_host, sizeof( ipv4_host))) {
                            rc = 0;

                            /** Ok */
                            break;
                        } break;
                    default : { rc = -1; } break;
                }

                if ( 0 == rc) { break; }
            }

            /** Ok, and create tcp socket */
            if ( 0 == rc) {
                sock_fd = eris_socket_tcp();
                if ( -1 != sock_fd) {
                    eris_int_t svc_port = eris_string_atoi( port);
                    if ( (0 >= svc_port) || (65535 <= svc_port)) {
                        svc_port = 9432;
                    }

                    struct sockaddr_in svc_addr; {
                        svc_addr.sin_family = AF_INET;
                        svc_addr.sin_port   = htons( svc_port);
                        svc_addr.sin_addr.s_addr = inet_addr( ipv4_host);
                    }

                    /** Set socket options */
                    eris_socket_reuseaddr( sock_fd);

                    if ( 1 == attrs.nonblock) {
                        eris_socket_nonblocking( sock_fd);
                    }

                    if ( 1 == attrs.tcp_nodelay) {
                        eris_socket_tcp_nodelay( sock_fd);
                    } else {
                        if ( 1 == attrs.tcp_nopush) {
                            eris_socket_tcp_nopush( sock_fd);
                        }
                    }

                    if ( 0 != attrs.linger) {
                        eris_socket_linger( sock_fd, (eris_int_t )attrs.linger);
                    }

                    /** Set send and recv buffer size */
                    eris_socket_set_recvbuf( sock_fd, attrs.recvbuf_size);
                    eris_socket_set_sendbuf( sock_fd, attrs.sendbuf_size);

                    /** Connect to server... */
                    rc = connect( sock_fd, (struct sockaddr *)&svc_addr, sizeof( struct sockaddr_in));
                    if ( 0 != rc) {
                        if ( errno == EINPROGRESS) {
                            /** Nothing */
                            eris_int_t     error_flag = 1;
                            eris_socklen_t error_flag_size = sizeof( eris_int_t);
                            (eris_none_t )getsockopt( sock_fd,  SOL_SOCKET, SO_ERROR ,&error_flag, &error_flag_size);

                        } else {
                            eris_socket_close( sock_fd); sock_fd = -1;

                            if ( __log) {
                                eris_log_dump( __log, ERIS_LOG_ERROR, "Connect to server failed, errno.<%d>", errno);
                            }
                        }
                    }
                } else {
                    if ( __log) {
                        eris_log_dump( __log, ERIS_LOG_ERROR, "Create socket of tcp protocol failed, errno.<%d>", errno);
                    }
                }
            } else {
                if ( __log) {
                    eris_log_dump( __log, ERIS_LOG_ERROR, "Host.<%s> is invalid", host);
                }
            }
        } else {
            if ( __log) {
                eris_log_dump( __log, ERIS_LOG_ERROR, "Get addrinfo failed, error.<%s> - errno.<%d>", hstrerror( h_errno), h_errno);
            }
        }
#endif
    }

    eris_string_free( host); eris_string_init( host);
    eris_string_free( port); eris_string_init( port);

    errno = tmp_errno;

    return sock_fd;
}/// eris_socket_tcp_connect


/**
 * @Brief: Get IPv4 addresss of socket object.
 *
 * @Param: __sock, Socket object.
 * @Param: __host, Output ipv4 and port.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_socket_host( eris_sock_t __sock, eris_socket_host_t *__host)
{
    eris_int_t rc = 0;

    if ( 0 <= __sock) {
        eris_socklen_t sock_addr_size = sizeof( struct sockaddr_in);
        struct sockaddr_in sock_addr;

        eris_memory_cleanup( &sock_addr, sock_addr_size);

        rc = getpeername( __sock, (struct sockaddr *)&sock_addr, &sock_addr_size);
        if ( 0 == rc) {
            if ( __host) {
                eris_memory_cleanup( __host->ipv4, sizeof( __host->ipv4));
                __host->port = ntohs( sock_addr.sin_port);

                (eris_void_t )inet_ntop( AF_INET, &(sock_addr.sin_addr), __host->ipv4, sizeof( __host->ipv4));

            }
        } else { rc = EERIS_ERROR; }
    } else { rc = EERIS_ERROR; }


    return rc;
}/// eris_socket_ipv4



/**
 * @Brief: Parse listen eris string.
 *
 * @Param: __listen_es, Listen eris string, eg: "127.0.0.1:9432".
 * @Param: __host_es,   Output host eris string, eg: "127.0.0.1".
 * @Param: __host_es,   Output host eris string, eg: "127.0.0.1".
 * @Param: __log,       Dump log context.
 *
 * @Return: Ok is 0, Other is EERIS_DATA or EERIS_ALLOC or EERIS_INVALID.
 **/
static eris_int_t eris_socket_listen_parse( 
    const eris_string_t __listen_es,
    eris_string_t      *__host_es,
    eris_string_t      *__port_es,
    eris_log_t         *__log )
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    enum {
        ERIS_SOCKET_LISTEN_START = 0,
        ERIS_SOCKET_LISTEN_HOST  ,
        ERIS_SOCKET_LISTEN_COLON ,
        ERIS_SOCKET_LISTEN_PORT  ,
    } eris_socket_listen_state_v = ERIS_SOCKET_LISTEN_START;

    if ( __listen_es) {
        eris_size_t i = 0;
        eris_size_t listen_es_size = eris_string_size( __listen_es);

        for ( i = 0; i < listen_es_size; i++) {
            eris_char_t c = eris_string_at( __listen_es, i);

            switch ( eris_socket_listen_state_v) {
                case ERIS_SOCKET_LISTEN_START :
                    {
                        if ( ':' == c) {
                            eris_socket_listen_state_v = ERIS_SOCKET_LISTEN_COLON;

                        } else {
                            /** Save a host char */
                            if ( isalnum( c) || ('_' == c) || ('.' == c)) {
                                eris_socket_listen_state_v = ERIS_SOCKET_LISTEN_HOST;

                                rc = eris_string_append_c( __host_es, c);
                                if ( 0 != rc) {
                                    rc = EERIS_ALLOC;

                                    if ( __log) {
                                        eris_log_dump( __log, ERIS_LOG_ERROR, "Store a host charactor.<%c> failed, errno.<%d>", c, errno);
                                    }
                                }
                            } else {
                                /** Invalid host a char */
                                rc = EERIS_DATA;

                                   if ( __log) {
                                       eris_log_dump( __log, ERIS_LOG_ERROR, "Invalid a host charactor.<%c> failed, errno.<%d>", c, errno);
                                   }
                            }
                        }
                    } break;
                case ERIS_SOCKET_LISTEN_HOST :
                    {
                        if ( ':' == c) {
                            eris_socket_listen_state_v = ERIS_SOCKET_LISTEN_COLON;

                        } else {
                            /** Save a host char */
                            if ( isalnum( c) || ('_' == c) || ('.' == c)) {
                                rc = eris_string_append_c( __host_es, c);
                                if ( 0 != rc) {
                                    rc = EERIS_ALLOC;

                                    if ( __log) {
                                        eris_log_dump( __log, ERIS_LOG_ERROR, "Store a host charactor.<%c> failed, errno.<%d>", c, errno);
                                    }
                                }
                            } else {
                                /** Invalid host a char */
                                rc = EERIS_DATA;

                                   if ( __log) {
                                       eris_log_dump( __log, ERIS_LOG_ERROR, "Invalid a host charactor.<%c> failed, errno.<%d>", c, errno);
                                   }
                            }
                        }
                    } break;
                case ERIS_SOCKET_LISTEN_COLON :
                    {
                        if ( isdigit( c)) {
                            eris_socket_listen_state_v = ERIS_SOCKET_LISTEN_PORT;

                            rc = eris_string_append_c( __port_es, c);
                            if ( 0 != rc) {
                                rc = EERIS_ALLOC;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_ERROR, "Store a port charactor.<%c> failed, errno.<%d>", c, errno);
                                }
                            }
                        } else {
                            /** Invalid a port char */
                            rc = EERIS_DATA;

                               if ( __log) {
                                   eris_log_dump( __log, ERIS_LOG_ERROR, "Invalid a port charactor.<%c> failed, errno.<%d>", c, errno);
                               }
                        }
                    } break;
                case ERIS_SOCKET_LISTEN_PORT :
                    {
                        if ( isdigit( c)) {
                            rc = eris_string_append_c( __port_es, c);
                            if ( 0 != rc) {
                                rc = EERIS_ALLOC;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_ERROR, "Store a port charactor.<%c> failed, errno.<%d>", c, errno);
                                }
                            }
                        } else {
                            /** Invalid a port char */
                            rc = EERIS_DATA;

                               if ( __log) {
                                   eris_log_dump( __log, ERIS_LOG_ERROR, "Invalid a port charactor.<%c> failed, errno.<%d>", c, errno);
                               }
                        }
                    } break;
                default : { rc = EERIS_DATA; } break;
            }/// switch ( eris_socket_listen_state_v)

            /** Haha, state is not used of error */
            if ( 0 != rc) { break; }

        }/// for ( i = 0; i < listen_es_size; i++)
    } else { rc = EERIS_INVALID; }

    errno = tmp_errno;

    return rc;
}/// eris_socket_listen_parse


