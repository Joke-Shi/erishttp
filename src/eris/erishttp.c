/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : The erishttp major c file.
 **
 ******************************************************************************/

#include "eris/erishttp.h"


/** Define erishttp block macro */
#define ERISHTTP_BLOCK_BEGIN  {
#define ERISHTTP_BLOCK_END    }


/** Print erishttp help usage message. */
static eris_none_t erishttp_usage( eris_none_t);

/** Print erishttp version message. */
static eris_none_t erishttp_version( eris_none_t);

/** Get config options from cftree. */
static eris_int_t erishttp_config_get( const eris_char_t *__key_path, eris_string_t *__value, eris_log_t *__log);

/** Init erishttp config attributes. */
static eris_int_t erishttp_attrs_init( eris_none_t);

/** Free mime type element. */
static eris_none_t erishttp_mime_types_free( erishttp_mime_type_t *__mt);

/** erishttp mime-types callback. */
static eris_none_t erishttp_mime_types_exec( const eris_string_t __mime, const eris_string_t __types);

/** Init erishttp mime-types . */
static eris_none_t erishttp_mime_types_init( eris_none_t);

/** Destroy erishttp config attributes. */
static eris_none_t erishttp_attrs_destroy( eris_none_t);

/** Init erishttp context logs context. */
static eris_int_t erishttp_logs_init( eris_none_t);

/** Init modules of erishttp server. */
static eris_int_t erishttp_module_init( eris_none_t);

/** Create erishttp server listen socket. */
static eris_int_t erishttp_socket_create( eris_none_t);

/** Create erishttp admin listen socket. */
static eris_int_t erishttp_admin_socket_create( eris_none_t);

/** Slave worker instance. */
static eris_none_t erishttp_slave_main( eris_int_t __index);

/** Spawn worker progressors */
static eris_int_t erishttp_spawn_slaves( eris_none_t);

/** Slave enent dispatch execute */
static eris_none_t erishttp_slave_event_exec( eris_event_elem_t *__elem, eris_arg_t __arg);

/** Admin event dispatch execute. */
static eris_none_t erishttp_admin_event_exec( eris_event_elem_t *__elem, eris_arg_t __arg);



/****************************************************************//*
 ** Global erishttp context 
 ******************************************************************/
static erishttp_t erishttp_context;
erishttp_t *const p_erishttp_context = &(erishttp_context);

#define erishttp_access_log_dump(level,fmt,...) \
    eris_log_exec(&(erishttp_context.access_log),(level), __FILE__, __func__, __LINE__, (fmt), ##__VA_ARGS__)

#define erishttp_errors_log_dump(level,fmt,...) \
    eris_log_exec(&(erishttp_context.errors_log),(level), __FILE__, __func__, __LINE__, (fmt), ##__VA_ARGS__)

#define erishttp_module_log_dump(level,fmt,...) \
    eris_log_exec(&(erishttp_context.module_log),(level), __FILE__, __func__, __LINE__, (fmt), ##__VA_ARGS__)


/*********************************************************************************************//*
 ************************************ MAIN-INSTANCE *********************************************
 ***********************************************************************************************/
int main( int argc, char **argv, char **env)
{
    eris_int_t  rc = 0;
    eris_bool_t goto_next = false;

    /** Init all */
    ERISHTTP_BLOCK_BEGIN
        eris_string_init( erishttp_context.prefix);
        eris_string_init( erishttp_context.cfile);

        erishttp_context.svc_sock   = -1;
        erishttp_context.admin_sock = -1;
        erishttp_context.slave_pids = NULL;
        erishttp_context.tasks_http = NULL;
    ERISHTTP_BLOCK_END


#define erishttp_stdout_print(fmt,...) fprintf( stdout, (fmt), ##__VA_ARGS__)
#define erishttp_stderr_print(fmt,...) fprintf( stderr, (fmt), ##__VA_ARGS__)
    /** Parse input options */
    ERISHTTP_BLOCK_BEGIN
    eris_int_t opt_c = 0;

    eris_bool_t bl_show_help        = false;
    eris_bool_t bl_show_version     = false;
    eris_bool_t bl_test_config      = false;
    eris_bool_t bl_test_dump_config = false;
    eris_bool_t bl_set_config       = false;
    eris_bool_t bl_set_outfile      = false;
    eris_bool_t bl_set_prefix       = false;


    eris_string_t test_config_file;
    eris_string_t test_dump_config_file;
    eris_string_t config_file;
    eris_string_t prefix_path;
    eris_string_t output_file;

    /** Init all */
    ERISHTTP_BLOCK_BEGIN
        eris_string_init( test_config_file);
        eris_string_init( test_dump_config_file);
        eris_string_init( config_file);
        eris_string_init( prefix_path);
        eris_string_init( output_file);
    ERISHTTP_BLOCK_END

    while ( -1 != (opt_c = getopt( argc, argv, "?hvt:T:c:p:o:"))) {
        switch ( opt_c) {
            case 't' :
                {
                    bl_test_config = true;

                    rc = eris_string_set( &test_config_file, optarg);
                    if ( 0 != rc) {
                        erishttp_stderr_print( "[ERROR]: Save test config file.<%s> failed, errno.<%d>\n", optarg, errno);
                    }
                } break;
            case 'T' :
                {
                    bl_test_dump_config = true;

                    rc = eris_string_set( &test_dump_config_file, optarg);
                    if ( 0 != rc) {
                        erishttp_stderr_print( "[ERROR]: Save test dump config file.<%s> failed, errno.<%d>\n", optarg, errno);
                    }
                } break;
            case 'c' :
                {
                    bl_set_config = true;

                    rc = eris_string_set( &config_file, optarg);
                    if ( 0 != rc) {
                        erishttp_stderr_print( "[ERROR]: Save config file.<%s> failed, errno.<%d>\n", optarg, errno);
                    }
                } break;
            case 'p' :
                {
                    bl_set_prefix = true;
                    eris_char_t prefix_path_buf[ PATH_MAX] = {0};

                    rc = eris_string_set( &prefix_path, realpath( optarg, prefix_path_buf));
                    if ( 0 == rc) {
                        eris_attrfs_t rc_attrfs = eris_attrfs_get( prefix_path);
                        if ( ERIS_ATTRFS_DIR == rc_attrfs) {
                            rc = 0;

                            erishttp_stderr_print( "[NOTICE]: prefix path.<%s>\n", prefix_path);

                        } else {
                            rc = EERIS_ERROR;

                            erishttp_stderr_print( "[ERROR]: prefix path.<%s> is no directory\n", prefix_path);
                        }
                    } else {
                        erishttp_stderr_print( "[ERROR]: Save prefix path.<%s> failed, errno.<%d>\n", optarg, errno);
                    }
                } break;
            case 'o' :
                {
                    bl_set_outfile = true;

                    rc = eris_string_set( &output_file, optarg);
                    if ( 0 != rc) {
                        erishttp_stderr_print( "[ERROR]: Save dump output config file.<%s> failed, errno.<%d>\n", optarg, errno);
                    }
                } break;
            case 'v' :
                {
                    bl_show_version = true;
                } break;
            case 'h': 
            case '?':
            default :
                {
                    bl_show_help = true;
                } break;
        }

        /** Heppen error */
        if ( 0 != rc) { break; }
    }/// while ( getopt ... ) 

    if ( 0 == rc) {
        /** How can i do, please see next */
        if ( bl_show_help) {
            erishttp_usage();

        } else if ( bl_show_version) {
            erishttp_version();

        } else if ( bl_test_dump_config) {
            /** Test and dump */
            eris_cftree_t test_dump_cftree;
            eris_cftree_init( &test_dump_cftree);

            rc = eris_cftree_parse( &test_dump_cftree, test_dump_config_file, NULL);
            if ( 0 == rc) {
                if ( bl_set_outfile) {
                    eris_file_t *fp = fopen( output_file, "w+");
                    if ( fp) {
                        /** Dump output file */
                        eris_cftree_dumpout( &test_dump_cftree, fp);

                        /** Release file pointer */
                        fclose( fp); fp = NULL;
                    } else {
                        erishttp_stderr_print( "[ERROR]: Open dump output file.<%s> failed, errno.<%d>\n", test_dump_config_file, errno);
                    }
                } else {
                    /** Dump ouput stderr */
                    eris_cftree_dumpout( &test_dump_cftree, stderr);
                }
            } else {
                erishttp_stderr_print( "[ERROR]: Parse config file.<%s> failed, errno.<%d>\n", test_dump_config_file, errno);
            }

            eris_cftree_destroy( &test_dump_cftree);

        } else if ( bl_test_config) {
            /** Test */
            eris_string_t config_content;
            eris_string_init( config_content);

            rc = eris_cftree_test( &config_content, test_config_file, NULL);
            if ( 0 == rc) {
                erishttp_stderr_print( "[Ok]: Config file.<%s> content is ok\n", test_config_file);

            } else {
                erishttp_stderr_print( "[ERROR]: Config file.<%s> content is invalid, error.<%s>\n", test_config_file, config_content);
            }

            eris_string_free( config_content);
            eris_string_init( config_content);

        } else {
            goto_next = true;

            /** Set working prefix path */
            if ( bl_set_prefix) {
                erishttp_context.prefix = eris_string_clone_eris( prefix_path);
                if ( !erishttp_context.prefix) {
                    goto_next = false;

                    erishttp_stderr_print( "[ERROR]: Save working prefix path.<%s> , errno.<%d>\n", prefix_path, errno);
                }
            } else {
                const char *p_erishttp_env = getenv( "ERISHTTP");
                if ( p_erishttp_env) {
                    rc = eris_string_set( &(erishttp_context.prefix), p_erishttp_env);
                    if ( 0 != rc) {
                        goto_next = false;

                        erishttp_stderr_print( "[ERROR]: Save working prefix path.<%s> , errno.<%d>\n", p_erishttp_env, errno);
                    }
                } else {
                    goto_next = false;

                    erishttp_stderr_print( "[ERROR]: Unset \"$ERISHTTP\" envirament variable, please set it\n");
                }
            }

            /** Is not test, then goto next */
            if ( 0 < eris_string_size( erishttp_context.prefix)) {
                if ( bl_set_config) {
                    erishttp_context.cfile = eris_string_clone_eris( config_file);
                    if ( !erishttp_context.cfile) {
                        goto_next = false;

                        erishttp_stderr_print( "[ERROR]: Save config file.<%s> , errno.<%d>\n", config_file, errno);
                    }
                } else {
                    rc = eris_string_set( &(erishttp_context.cfile), erishttp_context.prefix);
                    if ( 0 == rc) {
                        rc = eris_string_append( &(erishttp_context.cfile), "/config/erishttp.conf");
                        if ( 0 != rc) {
                            goto_next = false;

                            erishttp_stderr_print( "[ERROR]: Save config file path suffix.</config/erishttp.conf> , errno.<%d>\n", errno);
                        }
                    } else {
                        goto_next = false;

                        erishttp_stderr_print( "[ERROR]: Save config file path prefix.<%s> , errno.<%d>\n", erishttp_context.prefix, errno);
                    }
                }
            }
        }
    }
    
    
    /** Release all eris string */
    {
        eris_string_free( test_config_file);
        eris_string_free( test_dump_config_file);
        eris_string_free( config_file);
        eris_string_free( prefix_path);
        eris_string_free( output_file);

        /** Reset all */
        {
            eris_string_init( test_config_file);
            eris_string_init( test_dump_config_file);
            eris_string_init( config_file);
            eris_string_init( prefix_path);
            eris_string_init( output_file);
        }
    }

    if ( !goto_next) { 
        eris_string_free( erishttp_context.prefix);
        eris_string_init( erishttp_context.prefix);

        eris_string_free( erishttp_context.cfile);
        eris_string_init( erishttp_context.cfile);

        /** Exit processor */
        exit( rc); 
    }
    erishttp_stdout_print( "[NOTICE]: Set config file and prefix ok, rc.<%d>.\n", rc);
    ERISHTTP_BLOCK_END


    /** Get config info from config */
    ERISHTTP_BLOCK_BEGIN
        goto_next = true;

        eris_cftree_init( &(erishttp_context.cftree));
        rc = eris_cftree_parse( &(erishttp_context.cftree), erishttp_context.cfile, NULL);
        if ( 0 == rc) {
            rc = erishttp_attrs_init();
            if ( 0 != rc) {
                goto_next = false;

                erishttp_attrs_destroy();

                erishttp_stderr_print( "[ERROR]: Init attributes failed, errno.<%d>\n", errno);
            }
        } else {
            goto_next = false;

            erishttp_stderr_print( "[ERROR]: Parse config file.<%s>, please test and check it\n", erishttp_context.cfile);
        }

        if ( !goto_next) { 
            eris_string_free( erishttp_context.prefix);
            eris_string_init( erishttp_context.prefix);

            eris_string_free( erishttp_context.cfile);
            eris_string_init( erishttp_context.cfile);

            eris_cftree_destroy( &(erishttp_context.cftree));

            /** Exit processor */
            exit( rc); 
        }
        erishttp_stdout_print( "[NOTICE]: Parse and init configuration ok, rc.<%d>.\n", rc);
    ERISHTTP_BLOCK_END


    /** Init access/errors/module log context */
    ERISHTTP_BLOCK_BEGIN
        goto_next = true;

        rc = erishttp_logs_init();
        if ( 0 != rc) {
            goto_next = false;
        }

        if ( !goto_next) { 
            eris_string_free( erishttp_context.prefix);
            eris_string_init( erishttp_context.prefix);

            eris_string_free( erishttp_context.cfile);
            eris_string_init( erishttp_context.cfile);

            eris_cftree_destroy( &(erishttp_context.cftree));

            erishttp_attrs_destroy();

            /** Exit processor */
            exit( rc); 
        }
        erishttp_stdout_print( "[NOTICE]: Init all logs context ok, rc.<%d>.\n", rc);
    ERISHTTP_BLOCK_END


    /** Set daemon? */
    ERISHTTP_BLOCK_BEGIN
        if ( 1 == erishttp_context.attrs.daemon) {
            eris_daemon_create( erishttp_context.prefix);
        }

        /** Set rlimit nofile */
        struct rlimit rno; {
            rno.rlim_cur = erishttp_context.attrs.rlimit_nofile;
            rno.rlim_max = erishttp_context.attrs.rlimit_nofile;
        }
        (eris_none_t)setrlimit( RLIMIT_NOFILE, &rno);

        erishttp_context.ppid = eris_get_pid();
        rc = eris_util_dump_pid( erishttp_context.attrs.pidfile, erishttp_context.ppid);
        if ( 0 != rc) {
            if ( EERIS_EXISTED == rc) {
                erishttp_errors_log_dump( ERIS_LOG_ERROR, "erishttpd server progress is running, please check pidfile.<%s>",
                                          erishttp_context.attrs.pidfile);

            } else {
                erishttp_errors_log_dump( ERIS_LOG_ERROR, "Dump erishttpd server progress pid.<%d> failed", erishttp_context.ppid);
            }

            goto_next = false;
        }

        if ( !goto_next) { 
            eris_string_free( erishttp_context.prefix);
            eris_string_init( erishttp_context.prefix);

            eris_string_free( erishttp_context.cfile);
            eris_string_init( erishttp_context.cfile);

            eris_cftree_destroy( &(erishttp_context.cftree));

            eris_log_destroy( &(erishttp_context.access_log));
            eris_log_destroy( &(erishttp_context.errors_log));
            eris_log_destroy( &(erishttp_context.module_log));

            erishttp_attrs_destroy();

            /** Exit processor */
            exit( rc); 
        }
    ERISHTTP_BLOCK_END


    /** Set signal */
    ERISHTTP_BLOCK_BEGIN
        goto_next = true;
        rc = eris_signal_init( &(erishttp_context.errors_log));
        if ( 0 != rc) {
            goto_next = false;
        }

        if ( !goto_next) { 
            eris_string_free( erishttp_context.prefix);
            eris_string_init( erishttp_context.prefix);

            eris_string_free( erishttp_context.cfile);
            eris_string_init( erishttp_context.cfile);

            eris_cftree_destroy( &(erishttp_context.cftree));

            eris_log_destroy( &(erishttp_context.access_log));
            eris_log_destroy( &(erishttp_context.errors_log));
            eris_log_destroy( &(erishttp_context.module_log));

            erishttp_attrs_destroy();

            /** Exit processor */
            exit( rc); 
        }
    ERISHTTP_BLOCK_END


    /** Init mime-types */
    ERISHTTP_BLOCK_BEGIN
        erishttp_mime_types_init();
    ERISHTTP_BLOCK_END


    /** Proctitle init */
    ERISHTTP_BLOCK_BEGIN
        eris_proctitle_init( argv, &(erishttp_context.errors_log));
        eris_proctitle_set( "master", 0);
    ERISHTTP_BLOCK_END


    /** Init module context, load all modules */
    ERISHTTP_BLOCK_BEGIN
        goto_next = true;
        rc = erishttp_module_init();
        if ( 0 != rc) {
            goto_next = false;
        }

        if ( !goto_next) { 
            eris_string_free( erishttp_context.prefix);
            eris_string_init( erishttp_context.prefix);

            eris_string_free( erishttp_context.cfile);
            eris_string_init( erishttp_context.cfile);

            eris_cftree_destroy( &(erishttp_context.cftree));

            eris_log_destroy( &(erishttp_context.access_log));
            eris_log_destroy( &(erishttp_context.errors_log));
            eris_log_destroy( &(erishttp_context.module_log));

            erishttp_attrs_destroy();
            eris_list_destroy( &(erishttp_context.mime_types), (eris_list_free_cb_t )erishttp_mime_types_free);

            /** Exit processor */
            exit( rc); 
        }
    ERISHTTP_BLOCK_END

    
    /** Create server socket object */
    ERISHTTP_BLOCK_BEGIN
        goto_next = true;
        rc = erishttp_socket_create();
        if ( 0 != rc) {
            goto_next = false;
        }

        if ( !goto_next) { 
            eris_string_free( erishttp_context.prefix);
            eris_string_init( erishttp_context.prefix);

            eris_string_free( erishttp_context.cfile);
            eris_string_init( erishttp_context.cfile);

            eris_cftree_destroy( &(erishttp_context.cftree));
            eris_module_destroy( &(erishttp_context.mcontext));

            eris_log_destroy( &(erishttp_context.access_log));
            eris_log_destroy( &(erishttp_context.errors_log));
            eris_log_destroy( &(erishttp_context.module_log));

            erishttp_attrs_destroy();
            eris_list_destroy( &(erishttp_context.mime_types), (eris_list_free_cb_t )erishttp_mime_types_free);

            /** Exit processor */
            exit( rc); 
        }
    ERISHTTP_BLOCK_END

    
    /** Create admin server socket object */
    ERISHTTP_BLOCK_BEGIN
        goto_next = true;
        rc = erishttp_admin_socket_create();
        if ( 0 != rc) {
            goto_next = false;
        }

        if ( !goto_next) { 
            eris_string_free( erishttp_context.prefix);
            eris_string_init( erishttp_context.prefix);

            eris_string_free( erishttp_context.cfile);
            eris_string_init( erishttp_context.cfile);

            eris_socket_close( erishttp_context.svc_sock);
            erishttp_context.svc_sock = -1;

            eris_cftree_destroy( &(erishttp_context.cftree));
            eris_module_destroy( &(erishttp_context.mcontext));

            eris_log_destroy( &(erishttp_context.access_log));
            eris_log_destroy( &(erishttp_context.errors_log));
            eris_log_destroy( &(erishttp_context.module_log));

            erishttp_attrs_destroy();
            eris_list_destroy( &(erishttp_context.mime_types), (eris_list_free_cb_t )erishttp_mime_types_free);

            /** Exit processor */
            exit( rc); 
        }
    ERISHTTP_BLOCK_END


    /** Spawn worker processors */
    ERISHTTP_BLOCK_BEGIN
        goto_next = true;
        rc = erishttp_spawn_slaves();
        if ( 0 != rc) {
            goto_next = false;
        }

        eris_socket_close( erishttp_context.svc_sock);
        erishttp_context.svc_sock = -1;

        if ( !goto_next) { 
            erishttp_destroy( false);

            /** Exit processor */
            exit( rc); 
        }
    ERISHTTP_BLOCK_END

    /** Init admin server evnet */
    ERISHTTP_BLOCK_BEGIN
        goto_next = true;

        eris_event_attr_t evt_attrs; {
            evt_attrs.iot              = erishttp_context.attrs.event_iot;
            evt_attrs.timeout          = erishttp_context.attrs.timeout;;
            evt_attrs.keepalive        = erishttp_context.attrs.keepalive;;
            evt_attrs.max_events       = erishttp_context.attrs.max_connections;
            evt_attrs.recv_buffer_size = erishttp_context.attrs.recv_buffer_size;
            evt_attrs.send_buffer_size = erishttp_context.attrs.send_buffer_size;
            evt_attrs.tcp_nodelay      = erishttp_context.attrs.tcp_nodelay;
            evt_attrs.tcp_nopush       = erishttp_context.attrs.tcp_nopush;
            evt_attrs.zero             = erishttp_context.attrs.zero;
            evt_attrs.log              = &(erishttp_context.errors_log);
        }

        rc = eris_event_init( &(erishttp_context.admin_event), &evt_attrs);
        if ( 0 == rc) {
            {
                eris_event_elem_t admin_svc_elt; {
                    admin_svc_elt.sock   = erishttp_context.admin_sock;
                    admin_svc_elt.events = ERIS_EVENT_ACCEPT;
                }
                rc = eris_event_add( &(erishttp_context.admin_event), &admin_svc_elt);
            }

            /** Dispatch */
            if ( 0 == rc) {
                rc = eris_event_dispatch( &(erishttp_context.admin_event), 
                                          erishttp_admin_event_exec,
                                          &(erishttp_context.errors_log));

                erishttp_errors_log_dump( ERIS_LOG_NOTICE, "Dispatch over, rc.<%d>", rc);
            }

            /** End destroy event */
            eris_event_destroy( &erishttp_context.admin_event);
        } else {
            erishttp_errors_log_dump( ERIS_LOG_ERROR, "Init admin server event context, errno.<%d>", errno);
        }
    ERISHTTP_BLOCK_END

    /** End and destroy */
    erishttp_destroy( false);

    exit( rc);
}/// End->func: main



/*********************************************************************************************//*
 ************************************ Static function ******************************************
 ***********************************************************************************************/
/**
 * @Brief: Print erishttp help usage message.
 *
 * @Param: None
 *
 * @Return: Nothing.
 **/
static eris_none_t erishttp_usage( eris_none_t)
{
    (eris_none_t )erishttp_version();

#define ERISHTTP_STDERR_OUTUSAGE(fmt) fprintf( stderr, (fmt))
#define ERISHTTP_LF "\n"

    ERISHTTP_STDERR_OUTUSAGE(
        ERISHTTP_LF
        "Usage: " 
        ERISHTTP_LF
        "     : erishttpd [-tTc filename] [-p prefix] [-o outfile] [-?hv]" 
        ERISHTTP_LF
        ERISHTTP_LF
        "Options:" 
        ERISHTTP_LF
        "     -t filename  : Test configuration file and exit." 
        ERISHTTP_LF
        "     -T filename  : Test configuration file, dump it and exit." 
        ERISHTTP_LF
        "     -c filename  : Load configuration from file. (default: $ERISHTTP/config/erishttp.conf)"
        ERISHTTP_LF
        "     -p prefix    : Working prefix path. (default: $ERISHTTP)" 
        ERISHTTP_LF
        "     -o outfile   : Dump output file with the option \"-T\". (default: /dev/stderr)"
        ERISHTTP_LF
        "     -?,-h        : Print this help message." 
        ERISHTTP_LF
        "     -v           : Show version message and exit."
        ERISHTTP_LF
        ERISHTTP_LF
        "Notice: "
        ERISHTTP_LF
        "      After running, please set \"$ERISHTTP\" environment variable, or set option \"-p\","
        ERISHTTP_LF
        "      priority level option \"-p\" greater than \"$ERISHTTP\" env. thanks!!!"
        ERISHTTP_LF
        ERISHTTP_LF
    );

#undef  ERISHTTP_LF
#undef  ERISHTTP_STDERR_OUTUSAGE

}/// erishttp_usage


/**
 * @Brief: Print erishttp version message.
 *
 * @Param: None
 *
 * @Return: Nothing.
 **/
static eris_none_t erishttp_version( eris_none_t)
{
#define ERISHTTP_STDERR_OUTVERSION(fmt) fprintf( stderr, (fmt))
#define ERISHTTP_LF "\n"

    ERISHTTP_STDERR_OUTVERSION(
        "erishttp version: "
        ERIS_VERSION_DESC
        ERISHTTP_LF
    );

#undef  ERISHTTP_LF
#undef  ERISHTTP_STDERR_OUTVERSION
}/// erishttp_version


/**
 * @Brief: Get config options from cftree.
 *
 * @Param: __key_path, The key path string.
 * @Param: __value,    Output value.
 * @Param: __log,      Dump log context.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR | EERIS_NOTFOUND.
 **/
static eris_int_t erishttp_config_get( const eris_char_t *__key_path, eris_string_t *__value, eris_log_t *__log)
{
    eris_int_t rc = 0;

    eris_string_t abs_key_path_es;
    eris_string_init( abs_key_path_es);

    rc = eris_string_set( &abs_key_path_es, "http");
    if ( 0 == rc) {
        if ( eris_cftree_existed( &(erishttp_context.cftree), abs_key_path_es, __log)) {
            /** http.xxxx.yyyy */
            rc = eris_string_printf_append( &abs_key_path_es, ".%s", __key_path);

        } else {
            /** xxxx.yyyy */
            rc = eris_string_set( &abs_key_path_es, __key_path);
        }

        if ( 0 == rc) {
            if ( eris_cftree_existed( &(erishttp_context.cftree), abs_key_path_es, __log)) {
                /** Get value of key path */
                rc = eris_cftree_get( &(erishttp_context.cftree), abs_key_path_es, __value, __log);

            } else { rc = EERIS_NOTFOUND; }
        } else { rc = EERIS_ERROR; }
    } else { rc = EERIS_ERROR; }

    eris_string_free( abs_key_path_es);
    eris_string_init( abs_key_path_es);

    return rc;
}/// erishttp_config_get


/**
 * @Brief: Init erishttp config attributes.
 *
 * @Param: None.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
static eris_int_t erishttp_attrs_init( eris_none_t)
{
    eris_int_t rc = 0;

    /** Eris init */
    ERISHTTP_BLOCK_BEGIN
        eris_string_init( erishttp_context.attrs.user);
        eris_string_init( erishttp_context.attrs.group);
        eris_string_init( erishttp_context.attrs.pidfile);

        erishttp_context.attrs.daemon      = 1;
        erishttp_context.attrs.tcp_nodelay = 0;
        erishttp_context.attrs.tcp_nopush  = 0;
        erishttp_context.attrs.log_level   = 2;
        erishttp_context.attrs.zero        = 0;

        erishttp_context.attrs.worker_n      = 1;
        erishttp_context.attrs.worker_task_n = 8;
        erishttp_context.attrs.rlimit_nofile = 4096;
        erishttp_context.attrs.cpuset_enable = 0;
        erishttp_context.attrs.cpuset        = NULL;

        eris_string_init( erishttp_context.attrs.listen);
        eris_string_init( erishttp_context.attrs.admin_listen);
        erishttp_context.attrs.backlog         = 64;
        erishttp_context.attrs.max_connections = 4096;
        erishttp_context.attrs.event_iot       = ERIS_EVENT_IO_SELECT;

        erishttp_context.attrs.timeout   = 20;
        erishttp_context.attrs.keepalive = 60;

        erishttp_context.attrs.send_buffer_size = 8192;
        erishttp_context.attrs.recv_buffer_size = 8192;

        erishttp_context.attrs.header_cache_size = 4096UL;
        erishttp_context.attrs.body_cache_size   = 8192UL;
        erishttp_context.attrs.url_max_size      = 1024UL;
        erishttp_context.attrs.header_max_size   = 4096UL;
        erishttp_context.attrs.body_max_size     = 10 * 1024 * 1024UL;

        erishttp_context.attrs.log_max_size = 64 * 1024 * 1024UL;
        eris_string_init( erishttp_context.attrs.log_path);
        eris_string_init( erishttp_context.attrs.doc_root);
    ERISHTTP_BLOCK_END

    eris_string_t tmp_es = eris_string_alloc( 64);
    eris_string_cleanup( tmp_es);

    /** Get user and group */
    if ( 0 == rc)
    ERISHTTP_BLOCK_BEGIN
        rc = eris_string_set( &(erishttp_context.attrs.user),  "nobody");
        if ( 0 == rc)
        ERISHTTP_BLOCK_BEGIN
        rc = eris_string_set( &(erishttp_context.attrs.group), "nobody");
        ERISHTTP_BLOCK_END

        if ( 0 == rc)
        ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "user", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            eris_string_array_t p_user_array = eris_string_split( tmp_es, ":");
            if ( p_user_array) {
                eris_int_t i = 0;

                for ( i = 0; i < eris_string_array_size( p_user_array); i++) {
                    if ( 0 == i) {
                        rc = eris_string_set_eris( &(erishttp_context.attrs.user), p_user_array[ i]);

                    } else if ( 1 == i) {
                        rc = eris_string_set_eris( &(erishttp_context.attrs.group), p_user_array[ i]);

                    } else { break; }

                    if ( 0 != rc) { 
                        erishttp_stderr_print( "[ERROR]: Save user:group option failed, errno.<%d>\n", errno);

                        break; 
                    }
                }

                /** Release user array */
                eris_string_array_free( p_user_array);
            }
        }
        ERISHTTP_BLOCK_END
        else
        ERISHTTP_BLOCK_BEGIN
            erishttp_stderr_print( "[ERROR]: Set default user:group option failed, errno.<%d>\n", errno);
        ERISHTTP_BLOCK_END

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: user, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get pidfile path */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "pidfile", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es))) {
            if ( '/' == tmp_es[0]) {
                erishttp_context.attrs.pidfile = eris_string_clone_eris( tmp_es);
                if ( !erishttp_context.attrs.pidfile) {
                    rc = EERIS_ERROR;

                    erishttp_stderr_print( "[ERROR]: Merge pidfile failed, errno.<%d>\n", errno);
                }
            } else {
                rc = eris_string_printf( &(erishttp_context.attrs.pidfile), "%S/%S", erishttp_context.prefix, tmp_es);
                if ( 0 != rc) {
                    rc = EERIS_ERROR;

                    erishttp_stderr_print( "[ERROR]: Merge pidfile failed, errno.<%d>\n", errno);
                }
            }
        } else {
            if ( EERIS_NOTFOUND == rc) {
                erishttp_stdout_print( "[NOTICE]: Not found a config option: pidfile, rc.<%d>\n", rc);

                rc = eris_string_printf( &(erishttp_context.attrs.pidfile), "%S/var/run/erishttp.pid", erishttp_context.prefix);
                if ( 0 != rc) {
                    rc = EERIS_ERROR;

                    erishttp_stderr_print( "[ERROR]: Merge pidfile failed, errno.<%d>\n", errno);
                }
            } else { erishttp_stdout_print( "[ERROR]: Get pidfile option failed, errno.<%d>\n", errno); }
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get daemon flag */
    if ( 0 == rc)
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "daemon", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            if ( eris_string_isequal( tmp_es, "off", true)) {
                erishttp_context.attrs.daemon = 0;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: daemon, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get tcp_nodelay flag */
    if ( 0 == rc)
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "tcp_nodelay", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            if ( eris_string_isequal( tmp_es, "on", true)) {
                erishttp_context.attrs.tcp_nodelay = 1;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: tcp_nodelay, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get tcp_nopush flag */
    if ( 0 == rc)
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "tcp_nopush", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            if ( eris_string_isequal( tmp_es, "on", true)) {
                erishttp_context.attrs.tcp_nopush = 1;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: tcp_nopush, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get log level */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "log_level", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size(tmp_es)) ) {
            eris_int_t tmp_log_level = eris_string_atoi( tmp_es);
            if ( (0 <= tmp_log_level) && (ERIS_LOG_CORE >= tmp_log_level)) {
                /** Reset log level */
                erishttp_context.attrs.log_level = (eris_uint32_t )tmp_log_level;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: log_level, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get worker_n number */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "worker_n", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            eris_int_t tmp_worker_n = eris_string_atoi( tmp_es);
            if ( 0 < tmp_worker_n) {
                erishttp_context.attrs.worker_n = tmp_worker_n;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: worker_n, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get worker_task_n number */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "worker_task_n", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            eris_int_t tmp_worker_task_n = eris_string_atoi( tmp_es);
            if ( 0 < tmp_worker_task_n) {
                erishttp_context.attrs.worker_task_n = tmp_worker_task_n;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: worker_task_n, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get rlimit_nofile number */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "rlimit_nofile", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            eris_int_t tmp_rlimit_nofile = eris_string_atoi( tmp_es);
            if ( 0 < tmp_rlimit_nofile) {
                erishttp_context.attrs.rlimit_nofile = tmp_rlimit_nofile;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: rlimit_nofile, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get cpuset map */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "cpuset", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            erishttp_context.attrs.cpuset = eris_util_cpuset_parse( tmp_es, erishttp_context.attrs.worker_n);
            erishttp_context.attrs.cpuset_enable = 1;

        } else {

            erishttp_context.attrs.cpuset = eris_util_cpuset_parse( NULL, erishttp_context.attrs.worker_n);
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: cpuset, rc.<%d>\n", rc);

            rc = 0;
        }

        if ( !erishttp_context.attrs.cpuset) {
            rc = EERIS_ERROR;

            erishttp_stderr_print( "[ERROR]: Create cpuset map array failed, errno.<%d>\n", EERIS_ALLOC);
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get listen path */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "listen", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es))) {
            erishttp_context.attrs.listen = eris_string_clone_eris( tmp_es);
            if ( !erishttp_context.attrs.listen) {
                rc = EERIS_ERROR;

                erishttp_stderr_print( "[ERROR]: Save listen failed, errno.<%d>\n", errno);
            }
        } else {
            if ( EERIS_NOTFOUND == rc) {
                erishttp_stdout_print( "[NOTICE]: Not found a config option: listen, rc.<%d>\n", rc);

                rc = eris_string_set( &(erishttp_context.attrs.listen), "0.0.0.0:9432");
                if ( 0 != rc) {
                    rc = EERIS_ERROR;

                    erishttp_stderr_print( "[ERROR]: Set default listen failed, errno.<%d>\n", errno);
                }
            } else { erishttp_stdout_print( "[ERROR]: Get listen option failed, errno.<%d>\n", errno); }
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get backlog number */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "backlog", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            eris_int_t tmp_backlog = eris_string_atoi( tmp_es);
            if ( 0 < tmp_backlog) {
                erishttp_context.attrs.backlog = tmp_backlog;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: backlog, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get max_connections number */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "max_connections", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            eris_int_t tmp_max_connections = eris_string_atoi( tmp_es);
            if ( 0 < tmp_max_connections) {
                erishttp_context.attrs.max_connections = tmp_max_connections;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: max_connections, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get admin_listen path */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "admin_listen", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es))) {
            erishttp_context.attrs.admin_listen = eris_string_clone_eris( tmp_es);
            if ( !erishttp_context.attrs.admin_listen) {
                rc = EERIS_ERROR;

                erishttp_stderr_print( "[ERROR]: Save admin_listen failed, errno.<%d>\n", errno);
            }
        } else {
            if ( EERIS_NOTFOUND == rc) {
                erishttp_stdout_print( "[NOTICE]: Not found a config option: admin_listen, rc.<%d>\n", rc);

                rc = eris_string_set( &(erishttp_context.attrs.admin_listen), "0.0.0.0:9432");
                if ( 0 != rc) {
                    rc = EERIS_ERROR;

                    erishttp_stderr_print( "[ERROR]: Set default admin_listen failed, errno.<%d>\n", errno);
                }
            } else { erishttp_stdout_print( "[ERROR]: Get admin_listen option failed, errno.<%d>\n", errno); }
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get event_type flag */
    if ( 0 == rc)
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "event_type", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            if ( eris_string_isequal( tmp_es, "poll", true)) {
                erishttp_context.attrs.event_iot = ERIS_EVENT_IO_POLL;

            } else if ( eris_string_isequal( tmp_es, "epoll", true)) {
                erishttp_context.attrs.event_iot = ERIS_EVENT_IO_EPOLL;

            } else if ( eris_string_isequal( tmp_es, "kqueue", true) ||
                        eris_string_isequal( tmp_es, "kevent", true)) {
                erishttp_context.attrs.event_iot = ERIS_EVENT_IO_KQUEUE;

            } else {
                erishttp_context.attrs.event_iot = ERIS_EVENT_IO_SELECT;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: event_type, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get timeout number */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "timeout", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            eris_int_t tmp_timeout = eris_string_atoi( tmp_es);
            if ( 0 < tmp_timeout) {
                erishttp_context.attrs.timeout = tmp_timeout;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: timeout, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get keepalive number */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "keepalive", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            eris_int_t tmp_keepalive = eris_string_atoi( tmp_es);
            if ( 0 < tmp_keepalive) {
                erishttp_context.attrs.keepalive = tmp_keepalive;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: keepalive, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get send_buffer_size */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "send_buffer_size", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            eris_int64_t tmp_size = eris_util_unit_toi64( tmp_es);
            if ( 0 < tmp_size) {
                erishttp_context.attrs.send_buffer_size = (eris_int_t )tmp_size;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: send_buffer_size, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get recv_buffer_size */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "recv_buffer_size", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            eris_int64_t tmp_size = eris_util_unit_toi64( tmp_es);
            if ( 0 < tmp_size) {
                erishttp_context.attrs.recv_buffer_size = (eris_int_t )tmp_size;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: recv_buffer_size, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get header_cache_size */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "header_cache_size", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            eris_int64_t tmp_size = eris_util_unit_toi64( tmp_es);
            if ( 0 < tmp_size) {
                erishttp_context.attrs.header_cache_size = (eris_size_t )tmp_size;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: header_cache_size, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get url_max_size */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "url_max_size", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            eris_int64_t tmp_size = eris_util_unit_toi64( tmp_es);
            if ( 0 < tmp_size) {
                erishttp_context.attrs.url_max_size = (eris_size_t )tmp_size;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: url_max_size, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get body_cache_size */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "body_cache_size", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            eris_int64_t tmp_size = eris_util_unit_toi64( tmp_es);
            if ( 0 < tmp_size) {
                erishttp_context.attrs.body_cache_size = (eris_size_t )tmp_size;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: body_cache_size, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get url_max_size */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "url_max_size", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            eris_int64_t tmp_size = eris_util_unit_toi64( tmp_es);
            if ( 0 < tmp_size) {
                erishttp_context.attrs.url_max_size = (eris_size_t )tmp_size;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: url_max_size, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get header_max_size */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "header_max_size", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            eris_int64_t tmp_size = eris_util_unit_toi64( tmp_es);
            if ( 0 < tmp_size) {
                erishttp_context.attrs.header_cache_size = (eris_size_t )tmp_size;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: header_max_size, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get body_max_size */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "body_max_size", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            eris_int64_t tmp_size = eris_util_unit_toi64( tmp_es);
            if ( 0 < tmp_size) {
                erishttp_context.attrs.body_cache_size = (eris_size_t )tmp_size;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: body_max_size, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get log_max_size */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "log_max_size", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es)) ) {
            eris_int64_t tmp_size = eris_util_unit_toi64( tmp_es);
            if ( 0 < tmp_size) {
                erishttp_context.attrs.log_max_size = (eris_size_t )tmp_size;
            }
        }

        if ( EERIS_NOTFOUND == rc) {
            erishttp_stdout_print( "[NOTICE]: Not found a config option: log_max_size, rc.<%d>\n", rc);

            rc = 0;
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get log_path */
    if ( 0 == rc ) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "log_path", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es))) {
            if ( '/' == tmp_es[0]) {
                erishttp_context.attrs.log_path = eris_string_clone_eris( tmp_es);
                if ( !erishttp_context.attrs.log_path) {
                    rc = EERIS_ERROR;

                    erishttp_stderr_print( "[ERROR]: Merge log_path failed, errno.<%d>\n", errno);
                }
            } else {
                rc = eris_string_printf( &(erishttp_context.attrs.log_path), "%S/%S", erishttp_context.prefix, tmp_es);
                if ( 0 != rc) {
                    rc = EERIS_ERROR;

                    erishttp_stderr_print( "[ERROR]: Merge log_path failed, errno.<%d>\n", errno);
                }
            }
        } else {
            if ( EERIS_NOTFOUND == rc) {
                erishttp_stdout_print( "[NOTICE]: Not found a config option: log_path, rc.<%d>\n", rc);

                rc = eris_string_printf( &(erishttp_context.attrs.log_path), "%S/var/log", erishttp_context.prefix);
                if ( 0 != rc) {
                    rc = EERIS_ERROR;

                    erishttp_stderr_print( "[ERROR]: Merge log_path failed, errno.<%d>\n", errno);
                }
            } else { erishttp_stdout_print( "[ERROR]: Get log_path option failed, errno.<%d>\n", errno); }
        }
    ERISHTTP_BLOCK_END
    eris_string_cleanup( tmp_es);


    /** Get doc_root path */
    if ( 0 == rc) 
    ERISHTTP_BLOCK_BEGIN
        rc = erishttp_config_get( "doc_root", &tmp_es, NULL);
        if ( (0 == rc) && (0 < eris_string_size( tmp_es))) {
            if ( '/' == tmp_es[0]) {
                erishttp_context.attrs.doc_root = eris_string_clone_eris( tmp_es);
                if ( !erishttp_context.attrs.doc_root) {
                    rc = EERIS_ERROR;

                    erishttp_stderr_print( "[ERROR]: Merge doc_root failed, errno.<%d>\n", errno);
                }
            } else {
                rc = eris_string_printf( &(erishttp_context.attrs.doc_root), "%S/%S", erishttp_context.prefix, tmp_es);
                if ( 0 != rc) {
                    rc = EERIS_ERROR;

                    erishttp_stderr_print( "[ERROR]: Merge doc_root failed, errno.<%d>\n", errno);
                }
            }
        } else {
            if ( EERIS_NOTFOUND == rc) {
                erishttp_stdout_print( "[NOTICE]: Not found a config option: doc_root, rc.<%d>\n", rc);

                rc = eris_string_printf( &(erishttp_context.attrs.doc_root), "%S/var/www/htdoc", erishttp_context.prefix);
                if ( 0 != rc) {
                    rc = EERIS_ERROR;

                    erishttp_stderr_print( "[ERROR]: Merge doc_root failed, errno.<%d>\n", errno);
                }
            } else { erishttp_stdout_print( "[ERROR]: Get log_path option failed, errno.<%d>\n", errno); }
        }
    ERISHTTP_BLOCK_END
    
    eris_string_free( tmp_es);
    eris_string_init( tmp_es);

    return rc;
}/// erishttp_attrs_init


/**
 * @Brief: Free mime type element.
 *
 * @Param: __mt, MIME-type element pointer.
 *
 * @Return: Nothing.
 **/
static eris_none_t erishttp_mime_types_free( erishttp_mime_type_t *__mt)
{
    eris_string_free( __mt->mime);
    eris_string_free( __mt->types);

    eris_memory_free( __mt);
}/// erishttp_mime_types_free


/**
 * @Brief: erishttp mime-types callback.
 *
 * @Param: None.
 *
 * @Return: Nothing.
 **/
static eris_none_t erishttp_mime_types_exec( const eris_string_t __mime, const eris_string_t __types)
{
    if ( __mime && __types) {
        eris_int_t rc = 0;

        erishttp_mime_type_t *new_mime_types = (erishttp_mime_type_t *)eris_memory_alloc( sizeof( erishttp_mime_type_t));
        if ( new_mime_types) {
            new_mime_types->mime  = eris_string_clone_eris( __mime);
            new_mime_types->types = eris_string_clone_eris( __types);

            eris_string_append_c( &(new_mime_types->types), ' ');

            rc = eris_list_push( &(erishttp_context.mime_types), new_mime_types, 0);
            if (0 != rc) {
                erishttp_mime_types_free( new_mime_types);
            }
        }
    }
}/// erishttp_mime_types_exec


/**
 * @Brief: Init erishttp mime-types. 
 *
 * @Param: None.
 *
 * @Return: Nothing.
 **/
static eris_none_t erishttp_mime_types_init( eris_none_t)
{
    eris_list_init( &(erishttp_context.mime_types), false, &(erishttp_context.errors_log));

    const eris_char_t *p_mt_key_path = "http.mime-types";
    if ( eris_cftree_existed( &(erishttp_context.cftree), p_mt_key_path, &(erishttp_context.errors_log)) ) {
        /** Each all mime-types */
        eris_cftree_each( &(erishttp_context.cftree), p_mt_key_path, erishttp_mime_types_exec, &(erishttp_context.errors_log));

    } else {
        p_mt_key_path = "mime-types";

        if ( eris_cftree_existed( &(erishttp_context.cftree), p_mt_key_path, &(erishttp_context.errors_log)) ) {
            /** Each all mime-types */
            eris_cftree_each( &(erishttp_context.cftree), p_mt_key_path, erishttp_mime_types_exec, &(erishttp_context.errors_log));

        }
    }
}/// erishttp_mime_types_init


/**
 * @Brief: Destroy erishttp config attributes.
 *
 * @Param: None.
 *
 * @Return: Nothing.
 **/
static eris_none_t erishttp_attrs_destroy( eris_none_t)
{
    /** Eris init */
    ERISHTTP_BLOCK_BEGIN
        /** Release attributes */
        eris_string_free( erishttp_context.attrs.user);
        eris_string_free( erishttp_context.attrs.group);
        eris_string_free( erishttp_context.attrs.pidfile);
        eris_string_free( erishttp_context.attrs.listen);
        eris_string_free( erishttp_context.attrs.admin_listen);
        eris_string_free( erishttp_context.attrs.log_path);
        eris_string_free( erishttp_context.attrs.doc_root);

        eris_memory_free( erishttp_context.attrs.cpuset);
        erishttp_context.attrs.cpuset = NULL;

        /** Reinit */
        eris_string_init( erishttp_context.attrs.user);
        eris_string_init( erishttp_context.attrs.group);
        eris_string_init( erishttp_context.attrs.pidfile);
        eris_string_init( erishttp_context.attrs.listen);
        eris_string_init( erishttp_context.attrs.admin_listen);
        eris_string_init( erishttp_context.attrs.log_path);
        eris_string_init( erishttp_context.attrs.doc_root);
    ERISHTTP_BLOCK_END
}/// erishttp_attrs_destroy


/**
 * @Brief: Init erishttp context logs context.
 *
 * @Param: None.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
static eris_int_t erishttp_logs_init( eris_none_t )
{
    eris_int_t       rc = 0;
    eris_log_level_t log_level    = erishttp_context.attrs.log_level;
    eris_size_t      log_max_size = erishttp_context.attrs.log_max_size;

    eris_string_t access_log_es;
    eris_string_t errors_log_es;
    eris_string_t module_log_es;

    eris_string_init( access_log_es);
    eris_string_init( errors_log_es);
    eris_string_init( module_log_es);

    /** Make all logs */
    rc = eris_string_printf( &access_log_es, "%S/erishttp.access.log", erishttp_context.attrs.log_path);
    if ( 0 == rc) {
        rc = eris_string_printf( &errors_log_es, "%S/erishttp.errors.log", erishttp_context.attrs.log_path);
        if ( 0 == rc) {
            rc = eris_string_printf( &module_log_es, "%S/erishttp.module.log", erishttp_context.attrs.log_path);
            if ( 0 == rc) {
                /** Init all log context */
                rc = eris_log_init( &(erishttp_context.access_log), access_log_es, log_level, log_max_size);
                if ( 0 == rc) {
                    rc = eris_log_init( &(erishttp_context.errors_log), errors_log_es, log_level, log_max_size);
                    if ( 0 == rc) {
                        rc = eris_log_init( &(erishttp_context.module_log), module_log_es, log_level, log_max_size);
                        if ( 0 != rc) {
                            eris_log_destroy( &(erishttp_context.access_log));
                            eris_log_destroy( &(erishttp_context.errors_log));

                            erishttp_stderr_print( "[ERROR]: Init module log context failed, errno.<%d>\n", errno);
                        }
                    } else { 
                        eris_log_destroy( &(erishttp_context.access_log));

                        erishttp_stderr_print( "[ERROR]: Init errors log context failed, errno.<%d>\n", errno); 
                    }
                } else { erishttp_stderr_print( "[ERROR]: Init access log context failed, errno.<%d>\n", errno); }
            } else { erishttp_stderr_print( "[ERROR]: Merge module log file string failed, errno.<%d>\n", errno); }
        } else { erishttp_stderr_print( "[ERROR]: Merge errors log file string failed, errno.<%d>\n", errno); }
    } else { erishttp_stderr_print( "[ERROR]: Merge access log file string failed, errno.<%d>\n", errno); }

    /** Release all eris string */
    eris_string_free( access_log_es);
    eris_string_free( errors_log_es);
    eris_string_free( module_log_es);

    eris_string_init( access_log_es);
    eris_string_init( errors_log_es);
    eris_string_init( module_log_es);

    return rc;
}/// erishttp_logs_init
#undef erishttp_stdout_print
#undef erishttp_stderr_print


/**
 * @Brief: Init modules of erishttp server.
 *
 * @Param: None.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
static eris_int_t erishttp_module_init( eris_none_t)
{
    eris_int_t rc = 0;

    eris_string_t tmp_libfile_es;
    eris_string_t module_path_es;
    eris_module_info_t mod_info; 

    eris_string_init( tmp_libfile_es);
    eris_string_init( module_path_es);
    eris_module_info_init( mod_info);

    enum {
        ERISHTTP_MODULE_PATTERN = 0,
        ERISHTTP_MODULE_NAME    ,
        ERISHTTP_MODULE_LANGUAGE,
        ERISHTTP_MODULE_LIBFILE ,
        ERISHTTP_MODULE_ADD     ,
        ERISHTTP_MODULE_FINISH  ,
        ERISHTTP_MODULE_ERROR   ,
    } erishttp_module_state_v = ERISHTTP_MODULE_PATTERN;

    /** Init eris module context */
    rc = eris_module_init( &(erishttp_context.mcontext), &(erishttp_context.cftree), &(erishttp_context.module_log));
    if ( 0 == rc) {
        eris_int_t module_count = eris_cftree_esize( &(erishttp_context.cftree), "http.module", &(erishttp_context.errors_log));
        if ( 0 >= module_count) {
            module_count = eris_cftree_esize( &(erishttp_context.cftree), "module", &(erishttp_context.errors_log));
        }

        erishttp_module_log_dump( ERIS_LOG_DEBUG, "erishttp service modules count.<%d>", module_count);

        /** Get all modules configuration */
        eris_int_t i = 0;
        for ( i = 0; i < module_count; i++) {
            eris_module_info_cleanup( mod_info);

            rc = eris_string_printf( &module_path_es, "module[%i]", i);
            if ( 0 == rc) {
                do {
                    switch ( erishttp_module_state_v) {
                        case ERISHTTP_MODULE_PATTERN: 
                            {
                                erishttp_module_state_v = ERISHTTP_MODULE_NAME;

                                /** Get pattern */
                                rc = erishttp_config_get( module_path_es, &(mod_info.pattern), &(erishttp_context.errors_log));
                                if ( !((0 == rc) && (0 < eris_string_size( mod_info.pattern))) ) {
                                    rc = eris_string_set( &(mod_info.pattern), "/");
                                    if ( 0 != rc) {
                                        erishttp_module_state_v = ERISHTTP_MODULE_ERROR;

                                        erishttp_module_log_dump( ERIS_LOG_ERROR, "Set module pattern value failed, errno.<%d>", errno);

                                        break;
                                    }
                                }
                            } break;
                        case ERISHTTP_MODULE_NAME :
                            {
                                erishttp_module_state_v = ERISHTTP_MODULE_LANGUAGE;

                                /** Get module name */
                                rc = eris_string_printf( &module_path_es, "module[%i].name", i);
                                if ( 0 == rc) {
                                    rc = erishttp_config_get( module_path_es, &(mod_info.name), &(erishttp_context.errors_log));
                                    if ( (0 != rc) || (0 == eris_string_size( mod_info.name)) ) {
                                        erishttp_module_state_v = ERISHTTP_MODULE_ERROR;

                                        erishttp_module_log_dump( ERIS_LOG_ERROR, "Get module name.<%s> failed, rc.<%d>", module_path_es, rc);
                                    }

                                } else {
                                    erishttp_module_state_v = ERISHTTP_MODULE_ERROR;

                                    erishttp_module_log_dump( ERIS_LOG_ERROR, "Make module name key path failed, errno.<%d>", errno);
                                }
                            } break;
                        case ERISHTTP_MODULE_LANGUAGE :
                            {
                                erishttp_module_state_v = ERISHTTP_MODULE_LIBFILE;

                                /** Get module language */
                                rc = eris_string_printf( &module_path_es, "module[%i].language", i);
                                if ( 0 == rc) {
                                    rc = erishttp_config_get( module_path_es, &(mod_info.language), &(erishttp_context.errors_log));
                                    if ( 0 != rc) {
                                        rc = eris_string_set( &(mod_info.pattern), "c");
                                        if ( 0 != rc) {
                                            erishttp_module_state_v = ERISHTTP_MODULE_ERROR;

                                            erishttp_module_log_dump( ERIS_LOG_ERROR, "Set module language.<%s> failed, errno.<%d>", module_path_es, errno);
                                        }
                                    }

                                } else {
                                    erishttp_module_state_v = ERISHTTP_MODULE_ERROR;

                                    erishttp_module_log_dump( ERIS_LOG_ERROR, "Make module language key path failed, errno.<%d>", errno);
                                }
                            } break;
                        case ERISHTTP_MODULE_LIBFILE :
                            {
                                erishttp_module_state_v = ERISHTTP_MODULE_ADD;

                                /** Get module libfile */
                                rc = eris_string_printf( &module_path_es, "module[%i].libfile", i);
                                if ( 0 == rc) {
                                    eris_string_t tmp_libfile_es;
                                    eris_string_init( tmp_libfile_es);

                                    rc = erishttp_config_get( module_path_es, &tmp_libfile_es, &(erishttp_context.errors_log));
                                    if ( (0 == rc) && (0 < eris_string_size( tmp_libfile_es)) ) {
                                        if ( '/' == eris_string_at( tmp_libfile_es, 0)) {
                                            mod_info.libfile = eris_string_clone_eris( tmp_libfile_es);

                                        } else {
                                            eris_string_printf( &(mod_info.libfile), "%S/%S", 
                                                                erishttp_context.prefix, 
                                                                tmp_libfile_es);
                                        }

                                        if ( !mod_info.libfile) {
                                            erishttp_module_state_v = ERISHTTP_MODULE_ERROR;

                                            erishttp_module_log_dump( ERIS_LOG_ERROR, "Merge module libfile.<%s> failed, rc.<%d>", module_path_es, rc);
                                        }

                                    } else {
                                        erishttp_module_state_v = ERISHTTP_MODULE_ERROR;

                                        erishttp_module_log_dump( ERIS_LOG_ERROR, "Get module libfile.<%s> failed, rc.<%d>", module_path_es, rc);
                                    }

                                    eris_string_free( tmp_libfile_es);
                                    eris_string_init( tmp_libfile_es);

                                } else {
                                    erishttp_module_state_v = ERISHTTP_MODULE_ERROR;

                                    erishttp_module_log_dump( ERIS_LOG_ERROR, "Make module libfile key path failed, errno.<%d>", errno);
                                }
                            } break;
                        case ERISHTTP_MODULE_ADD :
                            {
                                erishttp_module_state_v = ERISHTTP_MODULE_FINISH;

                                /** Add new module into mcontext */
                                rc = eris_module_add( &(erishttp_context.mcontext), &mod_info);
                                if ( 0 != rc) {
                                    erishttp_module_state_v = ERISHTTP_MODULE_ERROR;

                                    erishttp_module_log_dump( ERIS_LOG_ERROR, "Add a module to mcontext failed, errno.<%d>", errno);
                                }
                            } break;
                        case ERISHTTP_MODULE_FINISH:
                            {
                                rc = 0;

                                erishttp_module_state_v = ERISHTTP_MODULE_PATTERN;
                            } break;
                        case ERISHTTP_MODULE_ERROR :
                        default : break;
                    }/// switch ( erishttp_module_state_v)

                    if ( ERISHTTP_MODULE_ERROR == erishttp_module_state_v) {
                        rc = EERIS_ERROR; break;
                    }
                    
                    if ( ERISHTTP_MODULE_FINISH == erishttp_module_state_v) {
                        break;
                    }
                } while ( 1); /** Get module attributes */

                if ( 0 != rc) { break; }
            } else { 
                erishttp_module_log_dump( ERIS_LOG_ERROR, "Make module key path failed, errno.<%d>", errno);

                break;
            }
        }/// for ( i = 0; i < module_count; i++);

        /** Happen error */
        if ( 0 != rc) {
            eris_module_destroy( &(erishttp_context.mcontext));
        }

        eris_string_free( tmp_libfile_es);
        eris_string_free( module_path_es);
        eris_string_init( tmp_libfile_es);
        eris_string_init( module_path_es);

        eris_module_info_destroy( mod_info);
    }/// if module context init ok

    return rc;
}/// erishttp_module_init


/**
 * @Brief: Create erishttp server listen socket.
 *
 * @Param: None.
 *
 * @Return: Ok is 0, Other is -1.
 **/
static eris_int_t erishttp_socket_create( eris_none_t)
{
    eris_int_t rc = 0;

    eris_socket_attr_t sock_attrs;
    ERISHTTP_BLOCK_BEGIN
        sock_attrs.nonblock     = 1;
        sock_attrs.backlog      = erishttp_context.attrs.backlog;
        sock_attrs.tcp_nodelay  = erishttp_context.attrs.tcp_nodelay;
        sock_attrs.tcp_nopush   = erishttp_context.attrs.tcp_nopush;
        sock_attrs.linger       = 0;
        sock_attrs.zero         = 0;
        sock_attrs.sendbuf_size = erishttp_context.attrs.send_buffer_size;
        sock_attrs.recvbuf_size = erishttp_context.attrs.recv_buffer_size;
    ERISHTTP_BLOCK_END

    /** Create server socket context */
    erishttp_context.svc_sock = eris_socket_tcp_listen( erishttp_context.attrs.listen, 
                                                        &sock_attrs, 
                                                        &(erishttp_context.errors_log));
    if ( -1 == erishttp_context.svc_sock) {
        rc = EERIS_ERROR;

        erishttp_errors_log_dump( ERIS_LOG_ERROR, "Create server socket object failed, errno.<%d>", errno);
    }

    return rc;
}/// erishttp_socket_create


/**
 * @Brief: Create erishttp admin listen socket.
 *
 * @Param: None.
 *
 * @Return: Ok is 0, Other is -1.
 **/
static eris_int_t erishttp_admin_socket_create( eris_none_t)
{
    eris_int_t rc = 0;

    eris_socket_attr_t sock_attrs;
    ERISHTTP_BLOCK_BEGIN
        sock_attrs.nonblock     = 1;
        sock_attrs.backlog      = erishttp_context.attrs.backlog;
        sock_attrs.tcp_nodelay  = erishttp_context.attrs.tcp_nodelay;
        sock_attrs.tcp_nopush   = erishttp_context.attrs.tcp_nopush;
        sock_attrs.linger       = 0;
        sock_attrs.zero         = 0;
        sock_attrs.sendbuf_size = erishttp_context.attrs.send_buffer_size;
        sock_attrs.recvbuf_size = erishttp_context.attrs.recv_buffer_size;
    ERISHTTP_BLOCK_END

    /** Create admin socket context */
    erishttp_context.admin_sock = eris_socket_tcp_listen( erishttp_context.attrs.admin_listen, 
                                                          &sock_attrs, 
                                                          &(erishttp_context.errors_log));
    if ( -1 == erishttp_context.admin_sock) {
        rc = EERIS_ERROR;

        erishttp_errors_log_dump( ERIS_LOG_ERROR, "Create admin server socket object failed, errno.<%d>", errno);
    }

    return rc;
}/// erishttp_admin_socket_create


/**
 * @Brief: Slave worker instance.
 *
 * @Param: __index, Processor index number.
 *
 * @Return: Nothing.
 **/
static eris_none_t erishttp_slave_main( eris_int_t __index)
{
    ERISHTTP_BLOCK_BEGIN
        eris_proctitle_set( "slave", __index);
        erishttp_errors_log_dump( ERIS_LOG_NOTICE, 
                                  "Slave progress index.<%d> - pid.<%d> ok",
                                  __index, 
                                  eris_get_pid());
    ERISHTTP_BLOCK_END

    /** Enabile/Disable cpu affinity */
    if ( 1 == erishttp_context.attrs.cpuset_enable) {
        eris_cpuset_bind( erishttp_context.attrs.cpuset[ __index + 1], 0);
    }

    eris_int_t rc = 0;
    rc = eris_signal_init( &(erishttp_context.errors_log));

    if ( 0 == rc)
    ERISHTTP_BLOCK_BEGIN
        eris_socket_close( erishttp_context.admin_sock);
        erishttp_context.admin_sock = -1;

        /** Init event queue */
        rc = eris_event_queue_init( &(erishttp_context.svc_event_queue), 
                                    erishttp_context.attrs.max_connections);
        if ( 0 == rc) {
            /** Create all task */
            erishttp_context.tasks_http = (eris_http_t *)eris_memory_alloc( sizeof( eris_http_t) *
                                                                            erishttp_context.attrs.worker_task_n);
            if ( erishttp_context.tasks_http) {
                eris_int_t i = 0;
                eris_int_t k = 0;

                ERISHTTP_BLOCK_BEGIN
                eris_http_attr_t http_attrs; {
                    http_attrs.url_max_size      = erishttp_context.attrs.url_max_size;
                    http_attrs.header_max_size   = erishttp_context.attrs.header_max_size;
                    http_attrs.body_max_size     = erishttp_context.attrs.body_max_size;
                    http_attrs.header_cache_size = erishttp_context.attrs.header_cache_size;
                    http_attrs.body_cache_size   = erishttp_context.attrs.body_cache_size;
                }


                for ( k = 0; k < erishttp_context.attrs.worker_task_n; k++) {
                    rc = eris_http_init( &(erishttp_context.tasks_http[ k]), &http_attrs, &(erishttp_context.errors_log));
                    if ( 0 != rc) {
                        erishttp_errors_log_dump( ERIS_LOG_ERROR, "Init http context of task woring failed, errno.<%d>", errno);

                        break;
                    }
                }
                ERISHTTP_BLOCK_END

                /** Create tasks http context ok */
                if ( 0 == rc) {
                    eris_task_init( &(erishttp_context.tasks), 0);

                    for ( k = 0; k < erishttp_context.attrs.worker_task_n; k++) {
                        rc = eris_task_create( &(erishttp_context.tasks), 
                                                eris_slave_handler, 
                                               &(erishttp_context.tasks_http[ k]), 
                                               &(erishttp_context.errors_log));
                        if ( 0 != rc) {
                            /** Create thread error */
                            k = erishttp_context.attrs.worker_task_n;

                            break;
                        }
                    }

                    /** Create tasks thread ok */
                    if ( 0 == rc) {
                        ERISHTTP_BLOCK_BEGIN
                        eris_event_attr_t evt_attrs; {
                            evt_attrs.iot              = erishttp_context.attrs.event_iot;
                            evt_attrs.timeout          = erishttp_context.attrs.timeout;;
                            evt_attrs.keepalive        = erishttp_context.attrs.keepalive;;
                            evt_attrs.max_events       = erishttp_context.attrs.max_connections;
                            evt_attrs.recv_buffer_size = erishttp_context.attrs.recv_buffer_size;
                            evt_attrs.send_buffer_size = erishttp_context.attrs.send_buffer_size;
                            evt_attrs.tcp_nodelay      = erishttp_context.attrs.tcp_nodelay;
                            evt_attrs.tcp_nopush       = erishttp_context.attrs.tcp_nopush;
                            evt_attrs.zero             = erishttp_context.attrs.zero;
                            evt_attrs.log              = &(erishttp_context.errors_log);
                        }

                        /** Init server evnet */
                        rc = eris_event_init( &(erishttp_context.svc_event), &evt_attrs);
                        ERISHTTP_BLOCK_END

                        if ( 0 == rc) {
                            ERISHTTP_BLOCK_BEGIN
                            eris_event_elem_t svc_elt; {
                                svc_elt.sock   = erishttp_context.svc_sock;
                                svc_elt.events = ERIS_EVENT_ACCEPT;
                            }
                            rc = eris_event_add( &(erishttp_context.svc_event), &svc_elt);
                            ERISHTTP_BLOCK_END

                            /** Dispatch */
                            if ( 0 == rc) {
                                erishttp_errors_log_dump( ERIS_LOG_DEBUG, "Dispatch...");

                                rc = eris_event_dispatch( &(erishttp_context.svc_event), 
                                                          erishttp_slave_event_exec,
                                                          &(erishttp_context.errors_log));

                                erishttp_errors_log_dump( ERIS_LOG_NOTICE, "Dispatch over, rc.<%d>", rc);
                            }

                            /** End destroy event */
                            eris_event_destroy( &erishttp_context.svc_event);
                        } else {
                            erishttp_errors_log_dump( ERIS_LOG_ERROR, "Init server event context, errno.<%d>", errno);
                        }
                    } else {
                        erishttp_errors_log_dump( ERIS_LOG_ERROR, "Create slave task failed, errno.<%d>", errno);
                    }
                }

                /** Destroy http context */
                for ( i = 0; i < k; i++) {
                    eris_http_destroy( &(erishttp_context.tasks_http[ i]));
                }

                eris_memory_free( erishttp_context.tasks_http);
                eris_task_destroy( &erishttp_context.tasks);

            } else {
                erishttp_errors_log_dump( ERIS_LOG_ERROR, "Create http context pool failed, errno.<%d>", errno);
            }

            /** End destroy event queue */
            eris_event_queue_destroy( &(erishttp_context.svc_event_queue));
        } else {
            erishttp_errors_log_dump( ERIS_LOG_ERROR, "Init event queue failed, errno.<%d>", errno);
        }

        /** Close svc_sock exit */
        eris_socket_close( erishttp_context.svc_sock);
        erishttp_context.svc_sock = -1;
    ERISHTTP_BLOCK_END
}/// erishttp_slave_main


/**
 * @Brief: Spawn worker progressors.
 *
 * @Param: None.
 *
 * @return: Ok is 0, Other is -1.
 **/
static eris_int_t erishttp_spawn_slaves( eris_none_t)
{
    eris_int_t rc = 0;

    ERISHTTP_BLOCK_BEGIN
        /** Create slave pids cache */
        erishttp_context.slave_pids = (eris_pid_t *)eris_memory_alloc
                                      (sizeof( eris_pid_t) * erishttp_context.attrs.worker_n);
        if ( erishttp_context.slave_pids) {
            /** Init pids cache */
            eris_int_t i = 0;

            for ( i = 0; i < erishttp_context.attrs.worker_n; i++) {
                erishttp_context.slave_pids[ i] = -1;
            }

            /** Create all slave pids */
            for ( i = 0; i < erishttp_context.attrs.worker_n; i++) {
                eris_pid_t a_slave_pid = fork();
                if ( 0 == a_slave_pid) {
                    /** Execute slave pid */
                    (eris_none_t )erishttp_slave_main( i);

                    exit( 0);
                } else if ( 0 < a_slave_pid) {
                    /** Save a slave pid */
                    erishttp_context.slave_pids[ i] = a_slave_pid;

                } else {
                    /** error of fork */
                    rc = EERIS_ERROR;

                    erishttp_errors_log_dump( ERIS_LOG_ERROR, "Create a slave index.<%d> failed, errno.<%d>", i, errno);

                    break;
                }
            }

            erishttp_errors_log_dump( ERIS_LOG_NOTICE, "Create slave pids cache ok");
        } else { 
            /** Alloced memory failed */
            rc = EERIS_ERROR;

            erishttp_errors_log_dump( ERIS_LOG_NOTICE, "Create slave pids cache failed, errno.<%d>", errno);
        }
    ERISHTTP_BLOCK_END

    return rc;
}/// erishttp_spawn_slaves


/**
 * @Brief: Slave event dispatch execute. 
 *
 * @Param: __elem, Heppen input event element.
 * @Param: __arg , Input args.
 *
 * @Return: Nothing.
 **/
static eris_none_t erishttp_slave_event_exec( eris_event_elem_t *__elem, eris_arg_t __arg)
{
    if ( 0 < __elem->sock) {
        eris_event_elem_t ev_elem; {
            ev_elem.sock   = __elem->sock;
            ev_elem.events = 0;
        }

        if ( ERIS_EVENT_OOB & __elem->events) {
            ev_elem.events = ERIS_EVENT_OOB;

            /** Put oob event */
            eris_event_queue_put( &(erishttp_context.svc_event_queue), &ev_elem, true);
        }

        if ( ERIS_EVENT_READ & __elem->events) {
            ev_elem.events = ERIS_EVENT_READ;

            /** Put read event */
            eris_event_queue_put( &(erishttp_context.svc_event_queue), &ev_elem, true);
        }

        if ( ERIS_EVENT_WRITE & __elem->events) {
            ev_elem.events = ERIS_EVENT_WRITE;

            /** Put write event */
            eris_event_queue_put( &(erishttp_context.svc_event_queue), &ev_elem, true);
        }

        if ( ERIS_EVENT_BUSY & __elem->events) {
            ev_elem.events = ERIS_EVENT_BUSY;

            /** Put write event */
            eris_event_queue_put( &(erishttp_context.svc_event_queue), &ev_elem, true);
        }

        if (( ERIS_EVENT_ERROR   & __elem->events) ||
            ( ERIS_EVENT_CLOSE   & __elem->events) ||
            ( ERIS_EVENT_TIMEOUT & __elem->events) ) {
            ev_elem.events = ERIS_EVENT_CLOSE;

            /** Put write event */
            eris_event_queue_put( &(erishttp_context.svc_event_queue), &ev_elem, true);
        }

    } else { 
        erishttp_errors_log_dump( ERIS_LOG_DEBUG, "erishttp event health timer."); 
    }
}/// erishttp_slave_event_exec


/**
 * @Brief: Admin event dispatch execute. 
 *
 * @Param: __elem, Heppen input event element.
 * @Param: __arg , Input args.
 *
 * @Return: Nothing.
 **/
static eris_none_t erishttp_admin_event_exec( eris_event_elem_t *__elem, eris_arg_t __arg)
{
    if ( 0 < __elem->sock) {
        /** Accept */
        eris_socket_close( __elem->sock);

    } else { 
        erishttp_errors_log_dump( ERIS_LOG_DEBUG, "erishttp admin event health timer..."); 
    }
}/// erishttp_admin_event_exec


/**
 * @Brief: Destroy by master init objects.
 *
 * @Param: __slave, Slave process doing.
 *
 * @Return: Nothing.
 **/
eris_none_t erishttp_destroy( eris_bool_t __slave)
{
    ERISHTTP_BLOCK_BEGIN
        /** Unlink pidfile */
        (eris_none_t )unlink( erishttp_context.attrs.pidfile);

        /** Slave progress doing */
        if ( __slave) {
            eris_task_destroy( &(erishttp_context.tasks));

            eris_int_t i = 0;
            for ( i = 0; i < erishttp_context.attrs.worker_task_n; i++) {
                eris_http_destroy( &(erishttp_context.tasks_http[ i]));
            }

            eris_memory_free( erishttp_context.tasks_http);

            eris_event_destroy( &(erishttp_context.svc_event));
            eris_event_queue_destroy( &(erishttp_context.svc_event_queue));
        }

        if ( erishttp_context.slave_pids) {
            if ( !__slave) {
                eris_int_t i = 0;

                for ( i = 0; i < erishttp_context.attrs.worker_n; i++) {
                    if ( 0 < erishttp_context.slave_pids[ i]) {
                        kill( erishttp_context.slave_pids[ i], SIGQUIT);
                    }
                }
            }

            eris_memory_free( erishttp_context.slave_pids);
        }

        eris_string_free( erishttp_context.prefix);
        eris_string_init( erishttp_context.prefix);

        eris_string_free( erishttp_context.cfile);
        eris_string_init( erishttp_context.cfile);

        /** close socket object */
        if ( 0 <= erishttp_context.svc_sock) {
            eris_socket_close( erishttp_context.svc_sock);
            erishttp_context.svc_sock = -1;
        }

        if ( 0 <= erishttp_context.admin_sock) {
            eris_socket_close( erishttp_context.admin_sock);
            erishttp_context.admin_sock = -1;
        }

        /** Free slave pids handler */
        eris_cftree_destroy( &(erishttp_context.cftree));
        eris_module_destroy( &(erishttp_context.mcontext));

        eris_log_destroy( &(erishttp_context.access_log));
        eris_log_destroy( &(erishttp_context.errors_log));
        eris_log_destroy( &(erishttp_context.module_log));

        erishttp_attrs_destroy();
        eris_list_destroy( &(erishttp_context.mime_types), (eris_list_free_cb_t )erishttp_mime_types_free);

        eris_proctitle_destroy();
    ERISHTTP_BLOCK_END
}/// erishttp_destroy



