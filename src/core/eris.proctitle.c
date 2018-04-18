/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Modify processor title.
 **
 ******************************************************************************/

#include "core/eris.core.h"


extern char **environ;


/** Pointer argv last pos */
static eris_char_t **p_eris_argv;
static eris_char_t  *p_eris_argv_last;
static eris_char_t  *p_new_environ;


/**
 * @Brief: Init proctitle.
 *
 * @Param: __argv, Os argv.
 * @Param: __log,   Dump log context.
 * 
 * @Return: Ok is EERIS_OK, Other (EERIS_ALLOC | EERIS_INVALID)
 **/
eris_int_t eris_proctitle_init( char **__argv, eris_log_t *__log)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc    = 0;
    p_eris_argv      = NULL;
    p_eris_argv_last = NULL;
    p_new_environ    = NULL;

    if ( __argv) {
        eris_int_t   i         = 0;
        eris_size_t  env_size  = 0;
        eris_char_t *p_new_env = NULL;

        for ( i = 0; environ[ i]; i++) {
            env_size += (strlen( environ[ i]) + 1);
        }

        /** Alloce save move environ */
        p_new_env = (eris_char_t *)eris_memory_alloc( env_size);
        if ( p_new_env) {
            p_new_environ    = p_new_env;
            p_eris_argv      = __argv;
            p_eris_argv_last = __argv[0];

            for ( i = 0; __argv[ i]; i++) {
                if ( p_eris_argv_last == __argv[ i]) {
                    env_size = strlen( __argv[ i]) + 1;

                    p_eris_argv_last = __argv[ i] + env_size;
                }
            }

            /** Copy environ */
            for ( i = 0; environ[ i]; i++) {
                if ( p_eris_argv_last == environ[ i]) {
                    env_size = strlen( environ[ i]) + 1;
                    p_eris_argv_last = environ[ i] + env_size;

                    eris_memory_copy( p_new_env, environ[ i], env_size);
                    environ[ i] = (eris_char_t *)p_new_env;

                    p_new_env += env_size;
                }
            }

            p_eris_argv_last--;
            *p_eris_argv_last = '\0';

        } else {
            rc = EERIS_ALLOC;

            if ( __log) {
                eris_log_dump( __log, ERIS_LOG_CORE, "Alloced memory for save environ failed, errno.<%d>", errno);
            }
        }
    } else {
        rc = EERIS_INVALID;

        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input argv is invalid");
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_proctitle_init


/**
 * @Brief: Set processor title of format.
 *
 * @Param: __title, Processor titile.
 * @Param: __index, Processor index.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_proctitle_set( const eris_char_t *__title, eris_int_t __index)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    eris_string_t proc_name_es;
    eris_string_init( proc_name_es);

    if ( p_eris_argv && p_eris_argv_last) {
        if ( __title) {
            rc = eris_string_printf( &proc_name_es, "erishttpd: %s processor - %i", __title, __index);

        } else {
            rc = eris_string_printf( &proc_name_es, "erishttpd: server processor - %i", __index);
        }

        if ( 0 == rc) {
            p_eris_argv[ 1] = NULL;

            eris_size_t copy_size = eris_string_size( proc_name_es);
            if ( copy_size >= (p_eris_argv_last - p_eris_argv[ 0])) {
                copy_size = (p_eris_argv_last - p_eris_argv[0]) - 1;
            }

            eris_memory_copy( p_eris_argv[0], proc_name_es, copy_size);
            p_eris_argv[0][ copy_size] = '\0';
        }
    }

    eris_string_free( proc_name_es);
    eris_string_init( proc_name_es);

    errno = tmp_errno;
}/// eris_proctitle_set


/**
 * @Brief: Release new environ pointer memory.
 *
 * @Param: None.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_proctitle_destroy( eris_none_t)
{
    if ( p_new_environ) {
        eris_memory_free( p_new_environ);
    }
}/// eris_proctitle_destroy



