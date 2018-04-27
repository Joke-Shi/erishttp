/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Signal doing.
 **
 ******************************************************************************/

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "eris/erishttp.h"


static eris_none_t eris_signal_handler( eris_int_t __signo);


/** eris signal struct type */
static eris_signal_t eris_signals_array[] = {
    { SIGHUP , "SIGHUP" , eris_signal_handler},
    { SIGUSR1, "SIGUSR1", eris_signal_handler},
    { SIGUSR2, "SIGUSR2", eris_signal_handler},
    { SIGQUIT, "SIGQUIT", eris_signal_handler},
    { SIGTERM, "SIGTERM", eris_signal_handler},
    { SIGSEGV, "SIGSEGV", eris_signal_handler},
    { SIGCHLD, "SIGCHLD", eris_signal_handler},
    { SIGSYS , "SIGSYS" , SIG_IGN            },
    { SIGPIPE, "SIGPIPE", SIG_IGN            },
    { SIGFPE , "SIGFPE" , SIG_IGN            },
    { 0      , NULL     , NULL               },
};



/**
 * @Brief: Init signals.
 *
 * @Param: __log, Dump log message context.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_signal_init( eris_log_t *__log)
{
    eris_int_t rc = 0;

    eris_signal_t *p_sigs = eris_signals_array;

#if (ERIS_HAVE_SIGACTION)
    eris_sigaction_t sa;

    for ( ; (0 != p_sigs->signo); p_sigs++) {
        eris_memory_cleanup( &sa, sizeof( eris_sigaction_t));

        sa.sa_handler = p_sigs->sighandler;
        sigemptyset( &(sa.sa_mask));

        if ( -1 == sigaction( p_sigs->signo, &sa, NULL)) {
            rc = EERIS_ERROR;

            if ( __log) {
                eris_log_dump( __log, ERIS_LOG_CORE, "sigaction set \"%s\" failed, errno.<%d>", p_sigs->signame, errno);
            }

            break;
        }
    }
#else
    for ( ; (0 != p_sigs->signo); p_sigs++) {
        if ( SIG_ERR == signal( p_sigs->signo, p_sigs->sighandler)) {
            rc = EERIS_ERROR;

            if ( __log) {
                eris_log_dump( __log, ERIS_LOG_CORE, "signal set \"%s\" failed, errno.<%d>", p_sigs->signame, errno);
            }

            break;
        }
    }
#endif

    return rc;
}/// eris_signal_init


/**
 * @Brief: Signal handler.
 *
 * @Param: __signo, Happen signo.
 *
 * @Return: Nothing.
 **/
static eris_none_t eris_signal_handler( eris_int_t __signo)
{
    eris_log_dump( &(p_erishttp_context->errors_log), ERIS_LOG_WARN, "Happen signal.<%d>", __signo);

    switch ( __signo) {
        case SIGHUP  : 
            { /** nothing */} break;
        case SIGUSR1 : 
            { /** nothing */} break;
        case SIGUSR2 : 
            { /** nothing */} break;
        case SIGCHLD : 
            { /** Nothing */} break;
        case SIGQUIT : 
        case SIGTERM : 
        case SIGSEGV : 
                { 
                    if ( eris_get_pid() != p_erishttp_context->ppid) {

                        erishttp_destroy( true);

                    } else {
                        erishttp_destroy( false);
                    }


                    eris_log_dump( &(p_erishttp_context->errors_log), ERIS_LOG_NOTICE, "Happen signal.<%d> and exit", __signo);

                    exit( 0);
                } break;
        default : break;

    }
}/// eris_signal_handler



