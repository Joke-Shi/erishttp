#ifndef __ERIS_PETREE_H__
#define __ERIS_PETREE_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This design to parser the path string, eg:
 **        :    http.max_connections
 **        :    http.service[1].name
 **        : and so on.
 **
 ******************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif


#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"



/** There define the key max size */
#define ERIS_PETREE_KSIZE 256


/** The element type flags */
enum eris_petree_type_e { 
    ERIS_PETREE_INTEGER = 0, 
    ERIS_PETREE_STRING  = 1, 
};


/** Redefined eris petree element */
typedef struct eris_petree_e_s eris_petree_e_t;

/** Redefined eris petree iter */
typedef struct eris_petree_iter_s eris_petree_iter_t;


/** The eris petree element */
struct eris_petree_e_s {
    eris_int_t       type;
    eris_int_t       index;
    eris_petree_e_t *child;
    eris_char_t      key[];
};

/** The eris petree iter type */
struct eris_petree_iter_s {
    eris_int_t       pos;
    eris_petree_e_t *current;
};


/** The eris petree structure */
//typedef struct eris_petree_s eris_petree_t;
struct eris_petree_s {
    eris_int_t       count;
    eris_int_t       level;
    eris_petree_e_t *head;
};



/**
 * @Brief: Clear-init.
 *
 * @Param: __petree, The path element tree context.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_petree_init( eris_petree_t *__petree);


/**
 * @Brief: Parser the path element string, eg: http.backlog.
 *
 * @Param: __petree, The path element tree context.
 * @Param: __pe_str, The path element string.
 * @Param: __errno,  If happen error and set it.
 *
 * @Return: Ok->0, other return EERIS_ERROR, and set errno: (EERIS_INVALID | EERIS_DATA | EERIS_MEMORY).
 **/
extern eris_int_t  eris_petree_parser( eris_petree_t *__petree, const eris_char_t *__pe_str, eris_errno_t *__errno);


/**
 * @Brief: Dump the petree to stdout output.
 *
 * @Param: __petree, The petree context.
 * @Param: __result, The dump result output, When dit not use, please free it.
 * @Param: __errno,  If happen error and set error code, (EERIS_INVALID | EERIS_ALLOC).
 *
 * @Return: Nothing.
 **/
extern eris_int_t eris_petree_dump( eris_petree_t *__petree, eris_char_t **__result, eris_errno_t *__errno);


/**
 * @Brief: Dump the petree to stdout output.
 *
 * @Param: __petree, The petree context.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_petree_dumpout( eris_petree_t *__petree);


/**
 * @Brief: Clean up the petree.
 *
 * @Param: __petree, The petree context.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_petree_cleanup( eris_petree_t *__petree);


/**
 * @Brief: Init-iter
 *
 * @Param: __petree, The petree context.
 * @Param: __iter, Pointer to iter objects.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_petree_iter( const eris_petree_t *__petree, eris_petree_iter_t *__iter);


/**
 * @Brief: Used iter goto next.
 *
 * @Param: __iter, Pointer to iter objects.
 *
 * @Return: The current element node.
 **/
extern eris_petree_e_t *eris_petree_next( eris_petree_iter_t *__iter);


/**
 * @Brief: Reset-iter
 *
 * @Param: __petree, The petree context.
 * @Param: __iter, Pointer to iter objects.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_petree_reset( const eris_petree_t *__petree, eris_petree_iter_t *__iter);


/**
 * @Brief: Destroy the petree.
 *
 * @Param: __petree, The petree context.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_petree_destroy( eris_petree_t *__petree);



#ifdef __cplusplus
}
#endif

#endif /** __ERIS_PETREE_H__ */


