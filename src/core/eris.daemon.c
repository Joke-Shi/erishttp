/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This design change the progress state to daemon, apis eg:
 **        :    eris_daemon_create
 **        :
 ******************************************************************************/

#include "core/eris.core.h"


/**
 * @Brief: Change the progress state to daemon.
 *
 * @Param: __dir, Working directory, default "/tmp".
 *
 * @Return: Ok is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_daemon_create( const eris_char_t *__dir)
{
    eris_int_t rc  = 0;
    eris_pid_t pid = 0;

    /** Create a child progress */
    pid = fork();
    if ( 0 > pid) {
        rc = EERIS_ERROR;
    } else if ( 0 < pid) {
        /** Exit the parent */
        exit( 0);
    } else {
        /** The child doing */
        pid = setsid();
        if ( 0 < pid) {
            pid = fork();
            if ( 0 > pid ) {
                /** Error happen */
                rc = EERIS_ERROR;
            } else if ( 0 < pid) {
                /** Parent exit */
                exit( 0);
            } else {
                /** Ok ok ok... */
                if ( __dir) {
                    rc = chdir( __dir);
                    if ( 0 != rc) {
                        chdir( "/tmp");

                        rc = 0;
                    }
                } else {
                    chdir( "/tmp");
                }

                umask( 0);
                
                (void )close( STDIN_FILENO);
                (void )close( STDOUT_FILENO);
                (void )close( STDERR_FILENO);

                /** Dup 0,1,2 fd to /dev/null */
                eris_fd_t fd_0 = open( "/dev/null", O_RDWR);
                if ( -1 != fd_0) {
                    (void )dup2( fd_0, STDIN_FILENO);
                    (void )dup2( fd_0, STDOUT_FILENO);
                    (void )dup2( fd_0, STDERR_FILENO);

                    if ( 2 < fd_0) { close( fd_0); }
                } else { rc = EERIS_ERROR; }
            }/// else to childres
        } else { rc = EERIS_ERROR; }
    }/// else to children

    return rc;
}/// eris_daemon_create


