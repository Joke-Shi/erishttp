#ifndef __ERIS_MODULE_H__
#define __ERIS_MODULE_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This is eris user modules manager. add a module and dispatch it.
 **
 ******************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"


/** Eris module language numbers */
enum eris_module_lgg_e {
    ERIS_MODULE_C   = 0,         /** Default C module  */
    ERIS_MODULE_CPP   ,          /** Cplusplus module  */
    ERIS_MODULE_LUA   ,          /** Lua module        */
    ERIS_MODULE_UNKNOW,          /** Unsupported       */
};
typedef enum eris_module_lgg_e eris_module_lgg_t;

/** Eris module attribute */
typedef struct eris_module_info_s eris_module_info_t;
struct eris_module_info_s {
    eris_string_t     name;      /** Module name       */
    eris_string_t     language;  /** Module language   */
    eris_string_t     pattern;   /** Pattern string    */
    eris_string_t     libfile;   /** Module libfile    */
};

#define eris_module_info_init(mfo) \
    do { \
        eris_string_init( mfo.name);     \
        eris_string_init( mfo.language); \
        eris_string_init( mfo.pattern);  \
        eris_string_init( mfo.libfile);  \
    } while (0) \

#define eris_module_info_cleanup(mfo) \
    do { \
        eris_string_cleanup( mfo.name);     \
        eris_string_cleanup( mfo.language); \
        eris_string_cleanup( mfo.pattern);  \
        eris_string_cleanup( mfo.libfile);  \
    } while (0) \

#define eris_module_info_destroy(mfo) \
    do { \
        eris_string_free( mfo.name);     \
        eris_string_free( mfo.language); \
        eris_string_free( mfo.pattern);  \
        eris_string_free( mfo.libfile);  \
        eris_string_init( mfo.name);     \
        eris_string_init( mfo.language); \
        eris_string_init( mfo.pattern);  \
        eris_string_init( mfo.libfile);  \
    } while (0) \


/** Eris module element type */
typedef struct eris_module_elem_s eris_module_elem_t;
struct eris_module_elem_s {
    eris_regex_t      regex;     /** Pattern map regex */
    eris_module_lgg_t lgg;       /** Language specify  */
    eris_string_t     name;      /** Module name       */
    eris_string_t     pattern;   /** Pattern string    */
    eris_string_t     libfile;   /** Module libfile    */
};


/** Eris module context type */
//typedef struct eris_module_s eris_module_t;
struct eris_module_s {
    eris_list_t       context;   /** Modules table     */
    eris_uint32_t     update:1;  /** Update flag       */
    eris_uint32_t     used:31;   /** Update flag       */
    eris_cftree_t    *cftree;    /** Config info - get */
    eris_log_t       *log;       /** Dump log context  */
};


/** Eris module service function type */
typedef eris_int_t ( *eris_module_service_t)( const eris_module_t *__mcontext, eris_http_t *__http, eris_log_t *__log);



/**
 * @Brief: Eris module init context.
 *
 * @Param: __module, Eris module context.
 * @Param: __cftree, Eris http config information tree.
 * @Param: __log,    Dump log context.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_module_init( eris_module_t *__module, eris_cftree_t *__cftree, eris_log_t *__log);


/**
 * @Brief: Add module and specify it information.
 *
 * @Param: __module, Eris module context.
 * @Param: __info,   Eris module attributes information.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_module_add( eris_module_t *__module, const eris_module_info_t *__info);


/**
 * @Brief: Update module and specify it information.
 *
 * @Param: __module, Eris module context.
 * @Param: __info,   Eris module attributes information.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_module_update( eris_module_t *__module, const eris_module_info_t *__info);


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
extern eris_int_t eris_module_exec( eris_module_t *__module, const eris_char_t *__service_path_s, eris_http_t *__http);


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
extern eris_int_t eris_module_exec_eris( eris_module_t *__module, const eris_string_t __service_path_es, eris_http_t *__http);


/**
 * @Brief: Erase a module from eris module context with name.
 *
 * @Param: __module, Eris module context.
 * @Param: __name_s, Module name of string.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_module_erase( eris_module_t *__module, const eris_char_t *__name_s);


/**
 * @Brief: Erase a module from eris module context with name.
 *
 * @Param: __module,  Eris module context.
 * @Param: __name_es, Module name of eris string.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_module_erase_eris( eris_module_t *__module, const eris_string_t __name_es);


/**
 * @Brief: Cleanup eris module context.
 *
 * @Param: __module, Eris module context.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_module_cleanup( eris_module_t *__module);


/**
 * @Brief: Destroy eris module context.
 *
 * @Param: __module, Eris module context.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_module_destroy( eris_module_t *__module);



#ifdef __cplusplus
}
#endif

#endif /** __ERIS_MODULE_H__ */


