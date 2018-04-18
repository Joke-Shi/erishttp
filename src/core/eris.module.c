/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This is eris user modules manager. add a module and dispatch it.
 **
 ******************************************************************************/

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.core.h"


/** Create module element. */
static eris_module_elem_t *eris_module_elem_create( const eris_module_info_t *__info, eris_log_t *__log);

/** Compare name with input arg. */
static eris_int_t eris_module_erase_compare( eris_module_elem_t *__elem, eris_arg_t __arg);

/** Release module element. */
static eris_none_t eris_module_elem_delete( eris_module_elem_t *__elem);



/**
 * @Brief: Eris module init context.
 *
 * @Param: __module, Eris module context.
 * @Param: __cftree, Eris http config information tree.
 * @Param: __log,    Dump log context.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_module_init( eris_module_t *__module, eris_cftree_t *__cftree, eris_log_t *__log)
{
    eris_int_t rc = 0;

    if ( __module) {
        __module->cftree = __cftree;
        __module->log    = __log;

        rc = eris_list_init( &(__module->context), true, __log);
        if ( 0 != rc) {
            if ( __log) {
                eris_log_dump( __log, ERIS_LOG_CORE, "Init module list context failed");
            }
        }
    } else {
        rc = EERIS_ERROR;

        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Invalid module context");
        }
    }

    return rc;
}/// eris_module_init


/**
 * @Brief: Add module and specify it information.
 *
 * @Param: __module, Eris module context.
 * @Param: __info,   Eris module attributes information.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_module_add( eris_module_t *__module, const eris_module_info_t *__info)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __module && __info) {
        eris_module_elem_t *new_module_elem = eris_module_elem_create( __info, __module->log);
        if ( new_module_elem) {
            /** Insert new element into module list context manager */
            eris_int_t prio = (eris_int_t )eris_string_size( __info->pattern);

            /** Lock */
            eris_list_lock( &(__module->context)); {
                __module->update = 1;

                while ( 0 < __module->used) {
                    eris_atomic_cpu_pause();
                }
            }

            rc = eris_list_push( &(__module->context), new_module_elem, prio);
            if ( 0 != rc) {
                if ( __module->log) {
                    eris_log_dump( __module->log, ERIS_LOG_CORE, "Save a module element failed, errno.<%d>", errno);
                }
            }

            /** Clean update flag */
            {
                __module->update = 0;
            }

            /** Unlock */
            eris_list_unlock( &(__module->context));

        } else {
            rc = EERIS_ERROR;

            if ( __module->log) {
                eris_log_dump( __module->log, ERIS_LOG_CORE, "Create module element failed");
            }
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_module_add


/**
 * @Brief: Update module and specify it information.
 *
 * @Param: __module, Eris module context.
 * @Param: __info,   Eris module attributes information.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_module_update( eris_module_t *__module, const eris_module_info_t *__info)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __module && __info) {
        /** Erase with name */
        eris_module_erase_eris( __module, __info->name);

        /** Add new */
        rc = eris_module_add( __module, __info);
        if ( 0 != rc) {
            if ( __module->log) {
                eris_log_dump( __module->log, ERIS_LOG_CORE, "Update a module element failed");
            }
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_module_update


/**
 * @Brief: Execute service in eris module context.
 *
 * @Param: __module, Eris module context.
 * @Param: __service_path_s, Execute service path string, eg: /xxx/yyy/zzz/hello_service.
 * @Param: __http,   Eris http context, it is request input and output response.
 *
 * @Return: Ok is EERIS_OK,
 *        : Other codes (EERIS_INVALID, EERIS_ALLOC, EERIS_NOTFOUND, EERIS_SERVICE, EERIS_UNSUPPORT).
 **/
eris_int_t eris_module_exec( eris_module_t *__module, const eris_char_t *__service_path_s, eris_http_t *__http)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = EERIS_OK;

    if ( __module && __service_path_s && __http) {
        /** Lock */
        eris_list_lock( &(__module->context)); {
            while ( __module->update) {
                eris_atomic_cpu_pause();
            }

            __module->used += 1;
        }
        eris_list_unlock( &(__module->context)); 

        /** Do service... */
        eris_list_iter_t elem_iter; {
            eris_list_iter_init( &(__module->context), &elem_iter);
        }

        eris_size_t service_path_s_size = strlen( __service_path_s);

        eris_module_elem_t *cur_elem = NULL;
        for ( ; (NULL != (cur_elem = eris_list_iter_next( &elem_iter))); ) {
            if ( __module->log) { 
                eris_log_dump( __module->log, ERIS_LOG_DEBUG, "Current service module.<%s>", cur_elem->name); 
            }

            if ( eris_regex_match( &(cur_elem->regex), __service_path_s, service_path_s_size) ) {
                /** Match ok */

                break;
            }
        }

        if ( cur_elem) {
            /** Get service name, Service name is "hello_service" of service path string */
            if ( (ERIS_MODULE_C == cur_elem->lgg) || (ERIS_MODULE_CPP == cur_elem->lgg) ) {
                const eris_char_t *s = __service_path_s + service_path_s_size;

                while ( s >= __service_path_s) {
                    if ( '/' == *s) {
                        /** Found and break out loop */

                        break;
                    } else {

                        s--;
                    }
                } s++;

                /** Call service */
                if ( '\0' != s) {
                    (eris_none_t )dlerror();
                    
#if (RTLD_DEEPBIND)
                    eris_int_t open_libfile_mode = (RTLD_LAZY | RTLD_GLOBAL | RTLD_DEEPBIND);
#else
                    eris_int_t open_libfile_mode = (RTLD_LAZY | RTLD_GLOBAL);
#endif
                    eris_void_t *libfile_context = dlopen( cur_elem->libfile, open_libfile_mode);
                    if ( libfile_context) {
                        /** Convert service function */
                        eris_module_service_t service_func = (eris_module_service_t)dlsym( libfile_context, s);
                        if ( service_func) {
                            rc = service_func( __module, __http, __module->log);
                            if ( 0 == rc) {
                                /** Ok */
                                rc = EERIS_OK;

                                if ( __module->log) {
                                    eris_log_dump( __module->log, ERIS_LOG_NOTICE, "Execute service \"%s\" ok", __service_path_s);
                                }
                            } else {
                                rc = EERIS_SERVICE;

                                if ( __module->log) {
                                    eris_log_dump( __module->log, ERIS_LOG_NOTICE, "Execute Service \"%s\" happend error", __service_path_s);
                                }
                            }
                        } else {
                            rc = EERIS_NOTFOUND;

                            if ( __module->log) {
                                eris_log_dump( __module->log, ERIS_LOG_NOTICE, "Dlopen service \"%s\" failed, error.<%s>", __service_path_s, (const char *)dlerror());
                            }
                        }

                        /** Dlclose do */
                        {
                            dlclose( libfile_context); 
                            libfile_context = NULL;
                        }

                    } else {
                        rc = EERIS_NOSUCH;

                        if ( __module->log) {
                            eris_log_dump( __module->log, ERIS_LOG_NOTICE, "Libfile \"%s\" no such file", cur_elem->libfile);
                        }
                    }
                } else {
                    rc = EERIS_NOTFOUND;

                    if ( __module->log) {
                        eris_log_dump( __module->log, ERIS_LOG_NOTICE, "Service \"%s\" not found", __service_path_s);
                    }
                }

                s = NULL;
            } else if ( ERIS_MODULE_LUA == cur_elem->lgg) {
                rc = EERIS_UNSUPPORT;

                if ( __module->log) {
                    eris_log_dump( __module->log, ERIS_LOG_NOTICE, "Service \"lua\" language unsupport of server");
                }
            } else {
                rc = EERIS_UNSUPPORT;

                if ( __module->log) {
                    eris_log_dump( __module->log, ERIS_LOG_NOTICE, "Service \"unknow\" language unsupport of server");
                }
            }
        } else {
            rc = EERIS_NOTFOUND;

            if ( __module->log) {
                eris_log_dump( __module->log, ERIS_LOG_NOTICE, "Service \"%s\" not found", __service_path_s);
            }
        }

        /** Clean up */
        cur_elem = NULL;
        eris_list_iter_cleanup( &(elem_iter));

        eris_list_lock( &(__module->context)); {
            __module->used -= 1;
        }
        eris_list_unlock( &(__module->context)); 

    } else { rc = EERIS_INVALID; }

    errno = tmp_errno;

    return rc;
}/// eris_module_exec


/**
 * @Brief: Execute service in eris module context.
 *
 * @Param: __module, Eris module context.
 * @Param: __service_path_es, Execute eris service path string, eg: /xxx/yyy/zzz/hello_service.
 * @Param: __http,   Eris http context, it is request input and output response.
 *
 * @Return: Ok is EERIS_OK,
 *        : Other codes (EERIS_INVALID, EERIS_ALLOC, EERIS_NOTFOUND, EERIS_SERVICE, EERIS_UNSUPPORT).
 **/
eris_int_t eris_module_exec_eris( eris_module_t *__module, const eris_string_t __service_path_es, eris_http_t *__http)
{
    eris_int_t rc = 0;

    if ( __module && __service_path_es) {
        /** Lock */
        eris_list_lock( &(__module->context)); {
            while ( __module->update) {
                eris_atomic_cpu_pause();
            }

            __module->used += 1;
        }
        eris_list_unlock( &(__module->context)); 

        /** Do service... */
        eris_list_iter_t elem_iter; {
            eris_list_iter_init( &(__module->context), &elem_iter);
        }

        eris_module_elem_t *cur_elem = NULL;
        for ( ; (NULL != (cur_elem = eris_list_iter_next( &elem_iter))); ) {
            if ( eris_regex_match_eris( &(cur_elem->regex), __service_path_es) ) {
                /** Match ok */

                break;
            }
        }

        if ( cur_elem) {
            /** Get service name, Service name is "hello_service" of service path string */
            if ( (ERIS_MODULE_C == cur_elem->lgg) || (ERIS_MODULE_CPP == cur_elem->lgg) ) {
                eris_size_t service_path_es_size = eris_string_size( __service_path_es);

                const eris_char_t *s = __service_path_es + service_path_es_size;

                while ( s >= __service_path_es) {
                    if ( '/' == *s) {
                        /** Found and break out loop */

                        break;
                    } else {

                        s--;
                    }
                } s++;

                /** Call service */
                if ( '\0' != s) {
                    (eris_none_t )dlerror();
                    
#if (RTLD_DEEPBIND)
                    eris_int_t open_libfile_mode = (RTLD_LAZY | RTLD_GLOBAL | RTLD_DEEPBIND);
#else
                    eris_int_t open_libfile_mode = (RTLD_LAZY | RTLD_GLOBAL);
#endif
                    eris_void_t *libfile_context = dlopen( cur_elem->libfile, open_libfile_mode);
                    if ( libfile_context) {
                        /** Convert service function */
                        eris_module_service_t service_func = (eris_module_service_t)dlsym( libfile_context, s);
                        if ( service_func) {
                            rc = service_func( __module, __http, __module->log);
                            if ( 0 != rc) {
                                /** Ok */
                                rc = EERIS_OK;

                                if ( __module->log) {
                                    eris_log_dump( __module->log, ERIS_LOG_NOTICE, "Execute service \"%s\" ok", __service_path_es);
                                }
                            } else {
                                rc = EERIS_SERVICE;

                                if ( __module->log) {
                                    eris_log_dump( __module->log, ERIS_LOG_NOTICE, "Execute Service \"%s\" happend errro", __service_path_es);
                                }
                            }
                        } else {
                            rc = EERIS_NOTFOUND;

                            if ( __module->log) {
                                eris_log_dump( __module->log, ERIS_LOG_NOTICE, "Dlopen service \"%s\" failed, error.<%s>", __service_path_es, (const char *)dlerror());
                            }
                        }

                        /** Dlclose do */
                        {
                            dlclose( libfile_context); 
                            libfile_context = NULL;
                        }

                    } else {
                        rc = EERIS_NOSUCH;

                        if ( __module->log) {
                            eris_log_dump( __module->log, ERIS_LOG_NOTICE, "Libfile \"%s\" no such file", cur_elem->libfile);
                        }
                    }
                } else {
                    rc = EERIS_NOTFOUND;

                    if ( __module->log) {
                        eris_log_dump( __module->log, ERIS_LOG_NOTICE, "Service \"%s\" not found", __service_path_es);
                    }
                }

                s = NULL;
            } else if ( ERIS_MODULE_LUA == cur_elem->lgg) {
                rc = EERIS_UNSUPPORT;

                if ( __module->log) {
                    eris_log_dump( __module->log, ERIS_LOG_NOTICE, "Service \"lua\" language unsupport of server");
                }
            } else {
                rc = EERIS_UNSUPPORT;

                if ( __module->log) {
                    eris_log_dump( __module->log, ERIS_LOG_NOTICE, "Service \"unknow\" language unsupport of server");
                }
            }
        } else {
            rc = EERIS_NOTFOUND;

            if ( __module->log) {
                eris_log_dump( __module->log, ERIS_LOG_NOTICE, "Service \"%s\" not found", __service_path_es);
            }
        }

        /** Clean up */
        cur_elem = NULL;
        eris_list_iter_cleanup( &(elem_iter));

        eris_list_lock( &(__module->context)); {
            __module->used -= 1;
        }
        eris_list_unlock( &(__module->context)); 

    } else { rc = EERIS_INVALID; }

    return rc;
}/// eris_module_exec_eris


/**
 * @Brief: Erase a module from eris module context with name.
 *
 * @Param: __module,  Eris module context.
 * @Param: __mname_s, Module name of string.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_module_erase( eris_module_t *__module, const eris_char_t *__name_s)
{
    if ( __module && __name_s) {
        /** Lock */
        eris_list_lock( &(__module->context)); {
            __module->update = 1;

            while ( 0 < __module->used) {
                eris_atomic_cpu_pause();
            }
        }

        /** Erase */
        eris_list_erase( &(__module->context), 
                         (eris_list_free_cb_t )eris_module_elem_delete,
                         (eris_list_cmp_cb_t  )eris_module_erase_compare,
                         (eris_arg_t )__name_s
                       );
        /** Clean update flag */
        {
            __module->update = 0;
        }

        /** Unlock */
        eris_list_unlock( &(__module->context));
    }
}/// eris_module_erase


/**
 * @Brief: Erase a module from eris module context with name.
 *
 * @Param: __module,   Eris module context.
 * @Param: __mname_es, Module name of eris string.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_module_erase_eris( eris_module_t *__module, const eris_string_t __name_es)
{
    if ( __module && __name_es) {
        /** Lock */
        eris_list_lock( &(__module->context)); {
            __module->update = 1;

            while ( 0 < __module->used) {
                eris_atomic_cpu_pause();
            }
        }

        /** Erase */
        eris_list_erase( &(__module->context), 
                         (eris_list_free_cb_t )eris_module_elem_delete,
                         (eris_list_cmp_cb_t  )eris_module_erase_compare,
                         (eris_arg_t )__name_es
                       );
        /** Clean update flag */
        {
            __module->update = 0;
        }

        /** Unlock */
        eris_list_unlock( &(__module->context));
    }
}/// eris_module_erase_eris


/**
 * @Brief: Cleanup eris module context.
 *
 * @Param: __module, Eris module context.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_module_cleanup( eris_module_t *__module)
{
    if ( __module) {
        /** Cleanup */
        eris_list_cleanup( &(__module->context), (eris_list_free_cb_t )eris_module_elem_delete);
    }
}/// eris_module_cleanup


/**
 * @Brief: Destroy eris module context.
 *
 * @Param: __module, Eris module context.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_module_destroy( eris_module_t *__module)
{
    if ( __module) {
        __module->cftree = NULL;
        __module->log    = NULL;

        /** Destroy */
        eris_list_destroy( &(__module->context), (eris_list_free_cb_t )eris_module_elem_delete);
    }
}/// eris_module_destroy


/**
 * @Brief: Create module element.
 *
 * @Param: __info, Eris module information.
 * @Param: __log,  Dump log message.
 *
 * @Return: Successed pointer to new module element, Other is NULL.
 **/
static eris_module_elem_t *eris_module_elem_create( const eris_module_info_t *__info, eris_log_t *__log)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;
    eris_module_elem_t *rc_elem = NULL;

    if ( __info) {
        rc_elem = (eris_module_elem_t *)eris_memory_alloc( sizeof( eris_module_elem_t));
        if ( rc_elem) {
            rc_elem->lgg = ERIS_MODULE_UNKNOW;

            eris_string_init( rc_elem->name);
            eris_string_init( rc_elem->pattern);
            eris_string_init( rc_elem->libfile);

            /** Init regex of input pattern */
            rc = eris_regex_init( &(rc_elem->regex), __info->pattern, __log); 
            if ( 0 == rc) {
                if ( eris_string_isequal( __info->language, "c", true)) {
                    rc_elem->lgg = ERIS_MODULE_C;

                } else if ( eris_string_isequal( __info->language, "c++", true) ||
                            eris_string_isequal( __info->language, "cpp", true) ||
                            eris_string_isequal( __info->language, "cxx", true) ||
                            eris_string_isequal( __info->language, "cc",  true)) {

                    rc_elem->lgg = ERIS_MODULE_CPP;
                } else if ( eris_string_isequal( __info->language, "lua", true)) {

                    rc_elem->lgg = ERIS_MODULE_LUA;
                } else {

                    rc_elem->lgg = ERIS_MODULE_UNKNOW;
                }

                /** Module name */
                if ( __info->name) {
                    rc_elem->name = eris_string_clone( __info->name);
                    if ( !rc_elem->name) {
                        rc = EERIS_ERROR;

                        if ( __log) {
                            eris_log_dump( __log, ERIS_LOG_CORE, "Clone module name.<%s> failed, errno.<%d>", __info->name, errno);
                        }
                    }
                }

                /** Module pattern */
                if ( (0 == rc) && __info->pattern) {
                    rc_elem->pattern = eris_string_clone( __info->pattern);
                    if ( !rc_elem->pattern) {
                        rc = EERIS_ERROR;

                        if ( __log) {
                            eris_log_dump( __log, ERIS_LOG_CORE, "Clone module pattern.<%s> failed, errno.<%d>", __info->pattern, errno);
                        }
                    }
                }

                /** Module libfile */
                if ( (0 == rc) && __info->libfile) {
                    rc_elem->libfile = eris_string_clone( __info->libfile);
                    if ( !rc_elem->libfile) {
                        rc = EERIS_ERROR;

                        if ( __log) {
                            eris_log_dump( __log, ERIS_LOG_CORE, "Clone module libfile.<%s> failed, errno.<%d>", __info->libfile, errno);
                        }
                    }
                }

                /** Happen error */
                if ( 0 != rc) {
                    eris_module_elem_delete( rc_elem); rc_elem = NULL;
                }
            } else {
                /** Error of regex init */
                eris_module_elem_delete( rc_elem); rc_elem = NULL;

                if ( __log) {
                    eris_log_dump( __log, ERIS_LOG_CORE, "Regex init failed");
                }
            }
        }
    }

    errno = tmp_errno;

    return rc_elem;
}/// eris_module_elem_create


/**
 * @Brief: Compare name with input arg.
 *
 * @Param: __elem, The key in module.
 * @Param: __arg,  Dst key string.
 *
 * @Return: Is equal is 0, Other is 1.
 **/
static eris_int_t eris_module_erase_compare( eris_module_elem_t *__elem, eris_arg_t __arg)
{
    eris_int_t rc = 1;

    if ( __elem && __arg) {
        if ( eris_string_isequal( __elem->name, (const eris_char_t *)__arg, false)) {

            rc = 0;
        }
    }

    return rc;
}/// eris_module_erase_compare


/**
 * @Brief: Release module element.
 *
 * @Param: __elem, Eris module element.
 *
 * @Return: Nothing.
 **/
static eris_none_t eris_module_elem_delete( eris_module_elem_t *__elem)
{
    if ( __elem) {
        /** Destroy regex */
        eris_regex_destroy( &(__elem->regex));

        __elem->lgg = ERIS_MODULE_UNKNOW;

        /** Release name */
        {
            eris_string_free( __elem->name);
            eris_string_init( __elem->name);
        }

        /** Release pattern */
        {
            eris_string_free( __elem->pattern);
            eris_string_init( __elem->pattern);
        }

        /** Release libfile */
        {
            eris_string_free( __elem->libfile);
            eris_string_init( __elem->libfile);
        }

        eris_memory_free( __elem);
    }
}/// eris_module_elem_delete


