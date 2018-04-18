/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Parse or pack x-www-form-urlencode data string.
 **
 ******************************************************************************/

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.core.h"



/** Compare key with input arg. */
static eris_int_t eris_xform_erase_compare( eris_void_t *__elem, eris_arg_t __arg);

/** Release elem memory. */
static eris_none_t eris_xform_elem_delete( eris_void_t *__elem);



/**
 * @Brief: Init eris xform context for parse application/x-www-form-urlencoded data.
 *
 * @Param: __xform, Eris xform context.
 * @Param_ __log  , Dump log context.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_xform_init( eris_xform_t *__xform, eris_log_t *__log)
{
    eris_int_t rc = 0;

    if ( __xform) {
        __xform->log = __log;

        /** Init xform list context */
        rc = eris_list_init( &(__xform->xform), false, __log);
        if ( 0 != rc) {
            rc = EERIS_ERROR;

            if ( __log) {
                eris_log_dump( __log, ERIS_LOG_CORE, "Init xform list context is failed");
            }
        }
    } else {
        rc = EERIS_ERROR;

        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input __xform context is invalid");
        }
    }

    return rc;
}/// eris_xform_init


/**
 * @Brief: Parse application/x-www-form-urlencoded query string.
 *
 * @Param: __xform,    Eris xform context.
 * @Param: __query_es, Format of application/x-www-form-urlencoded eris string.
 *
 * @Return: Ok is 0, Input query data invalid is 1, Other is EERIS_ERROR.
 **/
eris_int_t eris_xform_parse( eris_xform_t *__xform, const eris_string_t __query_es)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __xform) {
        /** Clean up old content */
        eris_xform_cleanup( __xform);

        if ( __query_es) {
            /** Split query string by '&' */
            eris_string_array_t xform_array = eris_string_split_eris( __query_es, "&");
            if ( xform_array) {
                eris_size_t i = 0;
                eris_size_t xform_array_size = eris_string_array_size( xform_array);

                for ( i = 0; i < xform_array_size; i++) {
                    const eris_string_t xform_eris_elt  = eris_string_array_at( xform_array, i);

                    /** Split key=value string */
                    eris_string_array_t xform_array_elt = eris_string_split_eris( xform_eris_elt, "=");
                    if ( xform_array_elt) {
                        if ( (0 < eris_string_array_size( xform_array_elt)) &&
                                (2 >= eris_string_array_size( xform_array_elt))
                           ) {
                            const eris_string_t key_es   = eris_string_array_at( xform_array_elt, 0);
                            const eris_string_t value_es = eris_string_array_at( xform_array_elt, 1);

                            /** New xform element */
                            eris_xform_elem_t *new_xform_elem = (eris_xform_elem_t *)eris_memory_alloc( sizeof( eris_xform_elem_t));
                            if ( new_xform_elem) {
                                new_xform_elem->key   = NULL;
                                new_xform_elem->value = NULL;

                                /** Decode urlencode, save key string */
                                rc = eris_urlencode_decode_eris( &new_xform_elem->key, key_es, __xform->log);
                                if ( 0 == rc) {
                                    rc = eris_urlencode_decode_eris( &new_xform_elem->value, value_es, __xform->log);
                                    if ( 0 == rc) {
                                        /** Save new xform element */
                                        rc = eris_list_append( &(__xform->xform), new_xform_elem, 0);
                                        if ( 0 != rc) {
                                            if ( __xform->log) {
                                                eris_log_dump( __xform->log, ERIS_LOG_CORE, "Save xform element failed, errno.<%d>", errno);
                                            }
                                        }

                                    } else {
                                        rc = EERIS_ERROR;

                                        if ( __xform->log) {
                                            eris_log_dump( __xform->log, ERIS_LOG_CORE, "Decode value string failed, errno.<%d>", errno);
                                        }
                                    }
                                } else {
                                    rc = EERIS_ERROR;

                                    if ( __xform->log) {
                                        eris_log_dump( __xform->log, ERIS_LOG_CORE, "Decode key string failed, errno.<%d>", errno);
                                    }
                                }

                                /** Failed and free xform element */
                                if ( 0 != rc) {
                                    eris_xform_elem_delete( new_xform_elem);
                                    new_xform_elem = NULL;
                                }
                            } else {
                                rc = EERIS_ERROR;

                                if ( __xform->log) {
                                    eris_log_dump( __xform->log, ERIS_LOG_CORE, "Alloced memory for xform element failed, errno.<%d>", errno);
                                }
                            }
                        } else {
                            rc = 1;

                            if ( __xform->log) {
                                eris_log_dump( __xform->log, ERIS_LOG_CORE, "Split query eris string failed, errno.<%d>", errno);
                            }
                        }

                        /** Release tmp xform eris string elements array */
                        eris_string_array_free( xform_array_elt); xform_array_elt = NULL;

                    } else {
                        rc = EERIS_ERROR;

                        if ( __xform->log) {
                            eris_log_dump( __xform->log, ERIS_LOG_CORE, "Split query eris string failed, errno.<%d>", errno);
                        }
                    }

                    /** Happend error */
                    if ( 0 != rc) { break; }

                }/// for ( i = 0; i < xform_array_size; i++)

                /** Release tmp xform eris string array */
                eris_string_array_free( xform_array); xform_array = NULL;

            } else {
                rc = EERIS_ERROR;

                if ( __xform->log) {
                    eris_log_dump( __xform->log, ERIS_LOG_CORE, "Split query eris string failed, errno.<%d>", errno);
                }
            }
        }/// fi ( __query_es)

        /** Happend error and cleanup */
        if ( 0 != rc) { eris_xform_cleanup( __xform); }

    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_xform_parse


/**
 * @Param: Pack xform to application/x-www-form-urlencoded string.
 *
 * @Param: __xform,  Eris xform context.
 * @Param: __out_es, Output application/x-www-form-urlencoded eris string.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_xform_pack( eris_xform_t *__xform, eris_string_t *__out_es)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __xform) {
        eris_string_cleanup( *(__out_es));

        if ( 0 < eris_list_size( &(__xform->xform))) {
            eris_xform_iter_t xform_iter; {
                eris_xform_iter_init( __xform, &xform_iter);
            }

            eris_string_t tmp_key;   eris_string_init( tmp_key);
            eris_string_t tmp_value; eris_string_init( tmp_value);

            const eris_xform_elem_t *xform_elem_ptr = NULL;
            for ( ; (NULL != (xform_elem_ptr = eris_xform_iter_next( &xform_iter))); ) {
                /** Encode urlencode of key */
                rc = eris_urlencode_encode_eris( &tmp_key, xform_elem_ptr->key, __xform->log);
                if ( 0 == rc) {
                    /** Encode urlencode of value */
                    rc = eris_urlencode_encode_eris( &tmp_value, xform_elem_ptr->value, __xform->log);
                    if ( 0 == rc) {
                        /** Merge key=value */
                        rc = eris_string_printf_append( __out_es, "%S=%S&", xform_elem_ptr->key, xform_elem_ptr->value);
                        if ( 0 != rc) {
                            rc = EERIS_ERROR;

                            if ( __xform->log) {
                                eris_log_dump( __xform->log, ERIS_LOG_CORE, 
                                                    "Append key.<%s> value.<%s> failed, errno.<%d>", 
                                                    xform_elem_ptr->key, 
                                                    xform_elem_ptr->value, 
                                                    errno);
                            }

                            break;
                        }
                    } else {
                        rc = EERIS_ERROR;

                        if ( __xform->log) {
                            eris_log_dump( __xform->log, ERIS_LOG_CORE, 
                                                "Encode urlencode value.<%s> failed, errno.<%d>", 
                                                xform_elem_ptr->value, 
                                                errno);
                        }

                        break;
                    }
                } else {
                    rc = EERIS_ERROR;

                    if ( __xform->log) {
                        eris_log_dump( __xform->log, ERIS_LOG_CORE, 
                                            "Encode urlencode key.<%s> failed, errno.<%d>", 
                                            xform_elem_ptr->key, 
                                            errno);
                    }

                    break;
                }
            }/// for ( xform_elem_iter... )

            eris_string_free( tmp_key);   
            eris_string_init( tmp_key);
            eris_string_free( tmp_value); 
            eris_string_init( tmp_value);

            /** OK */
            if ( 0 == rc) {
                eris_size_t out_es_size = eris_string_size( *(__out_es));

                /** Clean up end '&' */
                if ( 0 < out_es_size) {
                    eris_string_cleanup_as( *(__out_es), (out_es_size - 1), out_es_size);
                }
            } else {
                eris_string_free( *(__out_es)); 
                *(__out_es) = NULL;
            }
        }/// else nothing.
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_xform_pack


/**
 * @Brief: Get value of input key string.
 *
 * @Param: __xform, Eris xform context.
 * @Param: __key,   Query of key string.
 *
 * @Return: Ok is value string, Other is NULL - (not found).
 **/
const eris_string_t eris_xfrom_get( eris_xform_t *__xform, const eris_char_t *__key)
{
    eris_string_t rc_value = NULL;

    if ( __xform && __key) {
        eris_xform_iter_t xform_iter; {
            eris_xform_iter_init( __xform, &xform_iter);
        }

        const eris_xform_elem_t *xform_elem_ptr = NULL;

        for ( ; (NULL != (xform_elem_ptr = eris_xform_iter_next( &xform_iter))); ) {
            if ( eris_string_isequal( xform_elem_ptr->key, __key, false)) {
                /** Found */
                rc_value = xform_elem_ptr->value;

                break;
            }
        }/// for ( xform_elem_ptr... )
    }/// fi ( __xform && __key)

    return rc_value;
}/// eris_xform_get


/**
 * @Brief: Set key-value into xform, If key is existed and update it.
 *
 * @Param: __xform, Eris xform context.
 * @Param: __key,   Set specify key string.
 * @Param: __value, Set specify value string.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
const eris_int_t eris_xfrom_set( eris_xform_t *__xform, const eris_char_t *__key, const eris_char_t *__value)
{
    eris_int_t rc = 0;

    if ( __xform) {
        if ( __key) {
            eris_bool_t key_is_existed = false;

            eris_xform_iter_t xform_iter; {
                eris_xform_iter_init( __xform, &xform_iter);
            }

            eris_xform_elem_t *xform_elem_ptr = NULL;

            for ( ; (NULL != (xform_elem_ptr = eris_xform_iter_next( &xform_iter))); ) {
                if ( eris_string_isequal( xform_elem_ptr->key, __key, false)) {
                    /** Is existed and update */
                    key_is_existed = true;

                    rc = eris_string_set( &(xform_elem_ptr->value), __value);

                    break;
                }
            }

            /** Is not existed key */
            if ( !key_is_existed) {
                eris_xform_elem_t *new_xform_elem = (eris_xform_elem_t *)eris_memory_alloc( sizeof( eris_xform_elem_t));
                if ( new_xform_elem) {
                    eris_string_init( new_xform_elem->key);
                    eris_string_init( new_xform_elem->value);

                    rc = eris_urlencode_encode( &(new_xform_elem->key), __key, strlen( __key), __xform->log);
                    if ( 0 == rc) {
                        rc = eris_urlencode_encode( &(new_xform_elem->value), __value, strlen( __key), __xform->log);
                        if ( 0 == rc) {
                            rc = eris_list_append( &(__xform->xform), new_xform_elem, 0);
                            if ( 0 != rc) {
                                /** Append ok */
                                rc = EERIS_ERROR;

                                if ( __xform->log) {
                                    eris_log_dump( __xform->log, ERIS_LOG_CORE, "Encode value to urlencode failed, errno.<%d>", errno);
                                }
                            }
                        } else { 
                            rc = EERIS_ERROR;

                            if ( __xform->log) {
                                eris_log_dump( __xform->log, ERIS_LOG_CORE, "Encode value to urlencode failed, errno.<%d>", errno);
                            }
                        }
                    } else {
                        rc = EERIS_ERROR;

                        if ( __xform->log) {
                            eris_log_dump( __xform->log, ERIS_LOG_CORE, "Encode key to urlencode failed, errno.<%d>", errno);
                        }
                    }

                    /** Failed free new xform element node */
                    if ( 0 != rc) { 
                        eris_xform_elem_delete( new_xform_elem);
                        new_xform_elem = NULL;
                    }
                } else {
                    rc = EERIS_ERROR;

                    if ( __xform->log) {
                        eris_log_dump( __xform->log, ERIS_LOG_CORE, "Alloced new xform element failed, errno.<%d>", errno);
                    }
                }
            }
        } else {
            rc = EERIS_ERROR;

            if ( __xform->log) {
                eris_log_dump( __xform->log, ERIS_LOG_CORE, "Unspecify key, please set it");
            }
        }
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_xform_set


/**
 * @Brief: Erase a key-value in xform.
 *
 * @Param: __xform, Eris xform context.
 * @Param: __key, Key string.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_xform_erase( eris_xform_t *__xform, eris_char_t *__key)
{
    if ( __xform && __key) {
        /** Erase */
        eris_list_erase( &(__xform->xform), 
                         (eris_list_free_cb_t )eris_xform_elem_delete,
                         (eris_list_cmp_cb_t  )eris_xform_erase_compare,
                         (eris_arg_t )__key
                       );
    }
}/// eris_xform_erase


/**
 * @Brief: Cleanup eris xform context.
 *
 * @Param: __xform, Eris xform context.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_xform_cleanup( eris_xform_t *__xform)
{
    if ( __xform) {
        /** Destroy */
        eris_list_destroy( &(__xform->xform), (eris_list_free_cb_t )eris_xform_elem_delete);

    }
}/// eris_xform_cleanup


/**
 * @Brief: Destroy eris xform context.
 *
 * @Param: __xform, Eris xform context.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_xfrom_destroy( eris_xform_t *__xform)
{
    if ( __xform) {
        /** Destroy */
        eris_list_destroy( &(__xform->xform), (eris_list_free_cb_t )eris_xform_elem_delete);

        __xform->log = NULL;
    }
}/// eris_xform_destroy


/**
 * @Brief: Init eris xform iterator.
 *
 * @Param: __xform, Eris xform context.
 * @Param: __iter, Eris xform iterator pointer.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_xform_iter_init( eris_xform_t *__xform, eris_xform_iter_t *__iter)
{
    if ( __xform) {
        if ( __iter) {
            (__iter->iter) = __xform->xform.head;
        }
    } else {
        if ( __iter) {
            __iter->iter = NULL;
        }
    }
}/// eris_xform_iter_init


/**
 * @Brief: Get current element and iter pointer next.
 *
 * @Param: __iter, Eris xform iterator context.
 *
 * @Return: Data of xform current iter node.
 **/
eris_xform_elem_t *eris_xform_iter_next( eris_xform_iter_t *__iter)
{
    eris_xform_elem_t *rc_ptr = NULL;

    if ( __iter) {
        if ( __iter->iter) {
            rc_ptr = (eris_xform_elem_t *)((eris_list_node_t *)(__iter->iter))->data;

            __iter->iter = ((eris_list_node_t *)(__iter->iter))->next;
        }
    }

    return rc_ptr;
}/// eris_xform_iter_next


/**
 * @Brief: Get current element and iter pointer prev.
 *
 * @Param: __iter, Eris xform iterator context.
 *
 * @Return: Data of xform current iter node.
 **/
eris_xform_elem_t *eris_xform_iter_prev( eris_xform_iter_t *__iter)
{
    eris_xform_elem_t *rc_ptr = NULL;

    if ( __iter) {
        if ( __iter->iter) {
            rc_ptr = (eris_xform_elem_t *)((eris_list_node_t *)(__iter->iter))->data;

            __iter->iter = ((eris_list_node_t *)(__iter->iter))->prev;
        }
    }

    return rc_ptr;
}/// eris_xform_iter_prev


/**
 * @Brief: Reset eris xform iterator.
 *
 * @Param: __xform, Eris xform context.
 * @Param: __iter, Eris xform iterator pointer.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_xform_iter_reset( eris_xform_t *__xform, eris_xform_iter_t *__iter)
{
    if ( __xform) {
        if ( __iter) {
            (__iter->iter) = __xform->xform.head;
        }
    } else {
        if ( __iter) {
            __iter->iter = NULL;
        }
    }
}/// eris_xform_iter_reset


/**
 * @Brief: Cleanup current iter context.
 *
 * @Param: __iter, Eris xform iterator context.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_xform_iter_cleanup( eris_xform_iter_t *__iter)
{
    if ( __iter) {
        __iter->iter = NULL;
    }
}/// eris_xform_iter_cleanup


/**
 * @Brief: Compare key with input arg.
 *
 * @Param: __elem, The key in xform.
 * @Param: __arg,  Dst key string.
 *
 * @Return: Is equal is 0, Other is 1.
 **/
static eris_int_t eris_xform_erase_compare( eris_void_t *__elem, eris_arg_t __arg)
{
    eris_int_t rc = 1;
    eris_xform_elem_t *pelem = (eris_xform_elem_t *)__elem;

    if ( pelem && __arg) {
        if ( eris_string_isequal( pelem->key, (const eris_char_t *)__arg, false)) {
            rc = 0;
        }
    }

    return rc;
}/// eris_xform_erase_compare


/**
 * @Brief: Release elem memory.
 *
 * @Param: __elem, Element pointer.
 *
 * @Return: Nothing.
 **/
static eris_none_t eris_xform_elem_delete( eris_void_t *__elem)
{
    eris_xform_elem_t *pelem = (eris_xform_elem_t *)__elem;
    if ( pelem) {
        if ( pelem->key) {
            eris_string_free( pelem->key);
            pelem->key = NULL;
        }

        if ( pelem->value) {
            eris_string_free( pelem->value);
            pelem->value = NULL;
        }

        eris_memory_free( pelem);
    }
}/// eris_xform_elem_delete



