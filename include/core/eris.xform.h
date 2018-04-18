#ifndef __ERIS_XFORM_H__
#define __ERIS_XFORM_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Parse or pack x-www-form-urlencode data string.
 **
 ******************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"


/** x-www-form-urlencode element */
typedef struct eris_xform_elem_s eris_xform_elem_t;
struct eris_xform_elem_s {
    eris_string_t key;      /** xform key    */
    eris_string_t value;    /** xform value  */
};


/** xfrom iterator */
typedef struct eris_xform_iter_s eris_xform_iter_t;
struct eris_xform_iter_s {
    eris_void_t  *iter;    /** xform iterator*/
};


/** x-www-form-urlencode context type */
//typedef struct eris_xform_s eris_xform_t;
struct eris_xform_s {
    eris_list_t   xform;   /** xform handler */
    eris_log_t   *log;     /** Log handler   */
};


/**
 * @Brief: Init eris xform context for parse application/x-www-form-urlencoded data.
 *
 * @Param: __xform, Eris xform context.
 * @Param_ __log  , Dump log context.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_xform_init( eris_xform_t *__xform, eris_log_t *__log);


/**
 * @Brief: Parse application/x-www-form-urlencoded query string.
 *
 * @Param: __xform,    Eris xform context.
 * @Param: __query_es, Format of application/x-www-form-urlencoded eris string.
 *
 * @Return: Ok is 0, is query data invalid is 1, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_xform_parse( eris_xform_t *__xform, const eris_string_t __query_es);


/**
 * @Param: Pack xform to application/x-www-form-urlencoded string.
 *
 * @Param: __xform,  Eris xform context.
 * @Param: __out_es, Output application/x-www-form-urlencoded eris string.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_xform_pack( eris_xform_t *__xform, eris_string_t *__out_es);


/**
 * @Brief: Get value of input key string.
 *
 * @Param: __xform, Eris xform context.
 * @Param: __key,   Query of key string.
 *
 * @Return: Ok is value string, Other is NULL - (not found).
 **/
extern const eris_string_t eris_xfrom_get( eris_xform_t *__xform, const eris_char_t *__key);


/**
 * @Brief: Set key-value into xform, If key is existed and update it.
 *
 * @Param: __xform, Eris xform context.
 * @Param: __key,   Set specify key string.
 * @Param: __value, Set specify value string.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern const eris_int_t eris_xfrom_set( eris_xform_t *__xform, const eris_char_t *__key, const eris_char_t *__value);


/**
 * @Brief: Erase a key-value in xform.
 *
 * @Param: __xform, Eris xform context.
 * @Param: __key, Key string.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_xform_erase( eris_xform_t *__xform, eris_char_t *__key);


/**
 * @Brief: Cleanup eris xform context.
 *
 * @Param: __xform, Eris xform context.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_xform_cleanup( eris_xform_t *__form);


/**
 * @Brief: Destroy eris xform context.
 *
 * @Param: __xform, Eris xform context.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_xfrom_destroy( eris_xform_t *__form);


/**
 * @Brief: Init eris xform iterator.
 *
 * @Param: __xform, Eris xform context.
 * @Param: __iter, Eris xform iterator pointer.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_xform_iter_init( eris_xform_t *__xform, eris_xform_iter_t *__iter);


/**
 * @Brief: Get current element and iter pointer next.
 *
 * @Param: __iter, Eris xform iterator context.
 *
 * @Return: Data of xform current iter node.
 **/
extern eris_xform_elem_t *eris_xform_iter_next( eris_xform_iter_t *__iter);


/**
 * @Brief: Get current element and iter pointer prev.
 *
 * @Param: __iter, Eris xform iterator context.
 *
 * @Return: Data of xform current iter node.
 **/
extern eris_xform_elem_t *eris_xform_iter_prev( eris_xform_iter_t *__iter);


/**
 * @Brief: Reset eris xform iterator.
 *
 * @Param: __xform, Eris xform context.
 * @Param: __iter, Eris xform iterator pointer.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_xform_iter_reset( eris_xform_t *__xform, eris_xform_iter_t *__iter);


/**
 * @Brief: Cleanup current iter context.
 *
 * @Param: __iter, Eris xform iterator context.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_xform_iter_cleanup( eris_xform_iter_t *__iter);



#ifdef __cplusplus
}
#endif

#endif /** __ERIS_TIME_H__ */


