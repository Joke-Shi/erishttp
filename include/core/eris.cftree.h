#ifndef __ERIS_CFTREE_H__
#define __ERIS_CFTREE_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This design to parser the config options, like this eg:
 **        : http {
 **        :     listen 0.0.0.0:9436
 **        :     max_conntions   10240
 **        :     max_flow_limits 10240
 **        :     ...
 **        : }
 **
 ******************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"



/** Redefined eris_cftree_t */
typedef struct eris_cftree_s eris_cftree_t;

/** Redefined eris cftree element type */
typedef struct eris_cftree_e_s eris_cftree_e_t;


/** The eris cftree element */
struct eris_cftree_e_s {
    eris_string_t    key;
    eris_string_t    value;

    /** Pointer parent */
    eris_cftree_e_t *parent;

    /** Pointer child */
    eris_cftree_e_t *child;

    /** Pointer next brother */
    eris_cftree_e_t *next;
};


/** The eris cftree structrure */
//typedef struct eris_cftree_s eris_cftree_t;
struct eris_cftree_s {
    eris_int_t      count;

    /** Cftree head */
    eris_cftree_e_t head;
};

/** Eris cftree each cb type */
typedef eris_none_t (*eris_cftree_each_cb_t)( const eris_string_t __key, const eris_string_t __name);



/**
 * @Brief: Clear-init.
 *
 * @Param: __cftree, The config tree context.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_cftree_init( eris_cftree_t *__cftree);


/**
 * @Brief: Parse the path element string, eg: http.backlog.
 *
 * @Param: __cftree, The path element tree context.
 * @Param: __pe_str, The path element string.
 * @Param: __log,      Record log message out.
 *
 * @Return: Ok->0, other return EERIS_ERROR.
 **/
extern eris_int_t eris_cftree_parse( eris_cftree_t *__cftree, const eris_char_t *__cfile, eris_log_t *__log);


/**
 * @Brief: Get the value of the key path string, eg: http.backlog
 *
 * @Param: __cftree,   The config tree context.
 * @Param: __key_path, The key path string like this: http.backlog or http.service[2].name
 * @Param: __value,    If successed and set output value string.
 * @Param: __log,      Record log message out.
 *
 * @Return: Ok->0, other is EERIS_ERROR | EERIS_NOTFOUND.
 **/
extern eris_int_t eris_cftree_get( const eris_cftree_t *__cftree, const eris_char_t *__key_path, eris_string_t *__value, eris_log_t *__log);


/**
 * @Brief: Each elemets of key path, and depth is 1.
 *
 * @Param: __cftree,   The config tree context.
 * @Param: __key_path, The key path string like this: http.backlog or http.service[2].name
 * @Param: __each_cb,  Each callback.
 * @Param: __log,      Record log message out.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_cftree_each( const eris_cftree_t *__cftree, const eris_char_t *__key_path, eris_cftree_each_cb_t __each_cb, eris_log_t *__log);


/**
 * @Brief: Set the value with the key path string, eg: http.backlog
 *
 * @Param: __cftree,   The config tree context.
 * @Param: __key_path, The key path string like this: http.backlog or http.service[2].name
 * @Param: __value,    The key path value string.
 * @Param: __log,      Record log message out.
 *
 * @Return: Ok->0, other return EERIS_ERROR.
 **/
extern eris_int_t eris_cftree_set( eris_cftree_t *__cftree, const eris_char_t *__key_path, const eris_char_t *__value, eris_log_t *__log);


/**
 * @Brief: Get element count of key path string specify, eg: http.service
 *
 * @Param: __cftree,   The config tree context.
 * @Param: __key_path, The key path string like this: http.backlog or http.service
 * @Param: __log,      Record log message out.

 * @Return: Ok is count, other is EERIS_ERROR.
 **/
extern eris_int_t eris_cftree_esize( const eris_cftree_t *__cftree, const eris_char_t *__key_path, eris_log_t *__log);


/**
 * @Brief: Test element node is existed of key path string specify, eg: http.service
 *
 * @Param: __cftree,   The config tree context.
 * @Param: __key_path, The key path string like this: http.backlog or http.service
 * @Param: __log,      Record log message out.

 * @Return: If existed is true, other is false.
 **/
extern eris_bool_t eris_cftree_existed( const eris_cftree_t *__cftree, const eris_char_t *__key_path, eris_log_t *__log);


/**
 * @Brief: Dump the cftree to stdout output.
 *
 * @Param: __cftree, The config tree context.
 * @Param: __result, The dump result output, When dit not use, please free it.
 * @Param: __log,    Record log message out.
 *
 * @Return: Ok->0, other return EERIS_ERROR.
 **/
extern eris_int_t eris_cftree_dump( const eris_cftree_t *__cftree, eris_string_t *__result, eris_log_t *__log);


/**
 * @Brief: Dump the cftree to stdout output.
 *
 * @Param: __cftree, The cftree context.
 * @Param: __out_fp, Dump cftree content out there.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_cftree_dumpout( const eris_cftree_t *__cftree, eris_file_t *__out_fp);


/**
 * @Brief: Clean up the cftree.
 *
 * @Param: __cftree_ptr, The cftree context pointer.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_cftree_cleanup( eris_cftree_t *__cftree);


/**
 * @Brief: Test the config file is ok?
 *
 * @Param: __content, The output result content, if error and set error message.
 * @Param: __cfile  , The config file.
 * @Param: __log    , The log context.
 *
 * @Return: If ok 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_cftree_test( eris_string_t *__content, const eris_char_t *__cfile, eris_log_t *__log);


/**
 * @Brief: Get the config content from config file.
 *
 * @Param: __content, The output result content, if error and set error message.
 * @Param: __cfile  , The config file path name.
 * @Param: __errno  , If happen error and set it.
 *
 * @Return: Success is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_cftree_content( eris_string_t *__content, const eris_char_t *__cfile, eris_errno_t *__errno);


/**
 * @Brief: Destroy the cftree.
 *
 * @Param: __cftree_ptr, The cftree context pointer.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_cftree_destroy( eris_cftree_t *__cftree);


#ifdef __cplusplus
}
#endif

#endif /** __ERIS_CFTREE_H__ */

