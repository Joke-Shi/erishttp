#ifndef __ERIS_LIST_H__
#define __ERIS_LIST_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This is list of erishttp project, design apis eg:
 **        ::    init
 **        ::    size
 **        ::    push
 **        ::    push_back
 **        ::    append
 **        ::    pop
 **        ::    pop_back
 **        ::    find
 **        ::    rfind
 **        ::    erase
 **        ::    lock
 **        ::    unlock
 **        ::    cleanup
 **        ::    destroy
 **           And so on...
 **
 ******************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"



/** Eris list node data structure */
typedef struct eris_list_node_s eris_list_node_t;
struct eris_list_node_s {
    eris_int_t        prio;          /** Default is 0  */
    eris_void_t      *data;          /** Data pointer  */
    eris_list_node_t *prev;          /** Prev node     */
    eris_list_node_t *next;          /** Next node     */
};


/** Eris list iterator */
typedef struct eris_list_iter_s eris_list_iter_t;
struct eris_list_iter_s {
    eris_list_node_t *iter;          /** List iterator  */
};


/** Eris list context structure */
//typedef struct eris_list_s eris_list_t;
struct eris_list_s {
    eris_atomic_t     lock;          /** Safe-thread    */
    eris_pid_t        pid;           /** User pid       */
    eris_long_t       nprocs;        /** N processors   */
    eris_size_t       size;          /** List size      */
    eris_bool_t       enable_prio;   /** Used priority  */
    eris_list_node_t *head;          /** List head node */
    eris_list_node_t *tail;          /** List tail node */
    eris_log_t       *log;           /** Log context    */
};


/** Eris list compare callback type */
typedef eris_int_t ( *eris_list_cmp_cb_t)( eris_void_t *__data, eris_arg_t __arg);

/** Eris list free callback type */
typedef eris_none_t (*eris_list_free_cb_t)(eris_void_t *__data);


/**
 * @Brief: Init eris list context.
 *
 * @Param: __list,        Eris list context.
 * @Param: __enable_prio, Enable priority set true, default is false.
 * @Param: __log,         Dump log context.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_list_init( eris_list_t *__list, eris_bool_t __enable_prio, eris_log_t *__log);


/**
 * @Brief: Get list elements count.
 *
 * @Param: __list, Eris list context.
 * 
 * @Return: size of eris list.
 **/
extern eris_size_t eris_list_size( const eris_list_t *__list);


/**
 * @Brief: Push data into list, if enable priority and compare of priority.
 *
 * @Param: __list, Eris list context.
 * @Param: __data, Push source data pointer.
 * @Param: __prio, Enable and using.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_list_push( eris_list_t *__list, eris_void_t *__data, eris_int_t __prio);


/**
 * @Brief: Push data into list back, if enable priority and compare of priority.
 *
 * @Param: __list, Eris list context.
 * @Param: __data, Push source data pointer.
 * @Param: __prio, Enable and using.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_list_push_back( eris_list_t *__list, eris_void_t *__data, eris_int_t __prio);


/**
 * @Brief: Push data into list back, if enable priority and compare of priority.
 *
 * @Param: __list, Eris list context.
 * @Param: __data, Push source data pointer.
 * @Param: __prio, Enable and using.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_list_append( eris_list_t *__list, eris_void_t *__data, eris_int_t __prio);


/**
 * @Brief: Pop data from list head.
 *
 * @Param: __list, Eris list context.
 *
 * @Return: Pointer to data.
 **/
extern eris_void_t *eris_list_pop( eris_list_t *__list);


/**
 * @Brief: Pop data from list tail.
 *
 * @Param: __list, Eris list context.
 *
 * @Return: Pointer to data.
 **/
extern eris_void_t *eris_list_pop_back( eris_list_t *__list);


/**
 * @Brief: Find element from list head.
 *
 * @Param: __list,   Eris list context.
 * @Param; __cmp_cb, The compare callback.
 * @Param: __arg,    Input callback parameters.
 *
 * @Return: Pointer to find ok data, Other is NULL.
 **/
extern eris_void_t *eris_list_find( eris_list_t *__list, eris_list_cmp_cb_t __cmp_cb, eris_arg_t __arg);


/**
 * @Brief: Find element from list tail.
 *
 * @Param: __list,   Eris list context.
 * @Param; __cmp_cb, The compare callback.
 * @Param: __arg,    Input callback parameters.
 *
 * @Return: Pointer to find ok data, Other is NULL.
 **/
extern eris_void_t *eris_list_rfind( eris_list_t *__list, eris_list_cmp_cb_t __cmp_cb, eris_arg_t __arg);


/**
 * @Brief: Remove element from list.
 *
 * @Param: __list,    Eris list context.
 * @Param; __cmp_cb,  The compare callback.
 * @Param: __arg,     Input callback parameters.
 *
 * @Return: Pointer erase data, Other is NULL.
 **/
extern eris_void_t *eris_list_remove( eris_list_t *__list, eris_list_cmp_cb_t __cmp_cb, eris_arg_t __arg);


/**
 * @Brief: Erase element from list.
 *
 * @Param: __list,    Eris list context.
 * @Param: __free_cb, If set and call free data.
 * @Param; __cmp_cb,  The compare callback.
 * @Param: __arg,     Input callback parameters.
 *
 * @Return: Pointer erase data, Other is NULL.
 **/
extern eris_none_t eris_list_erase( eris_list_t *__list, eris_list_free_cb_t __free_cb, eris_list_cmp_cb_t __cmp_cb, eris_arg_t __arg);


/**
 * @Brief: If do safe-thread lock.
 *
 * @Param: __list, Eris list context.
 * 
 * @Return: Nothing
 **/
extern eris_void_t eris_list_lock( eris_list_t *__list);


/**
 * @Brief: If do safe-thread lock, then unlock this.
 *
 * @Param: __list, Eris list context.
 * 
 * @Return: Nothing
 **/
extern eris_none_t eris_list_unlock( eris_list_t *__list);


/**
 * @Brief: Cleanup eris list context.
 *
 * @Param: __list,    Eris list context.
 * @Param: __free_cb, If set and call free data.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_list_cleanup( eris_list_t *__list, eris_list_free_cb_t __free_cb);


/**
 * @Brief: Destroy eris list context.
 *
 * @Param: __list,    Eris list context.
 * @Param: __free_cb, If set and call free data.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_list_destroy( eris_list_t *__list, eris_list_free_cb_t __free_cb);


/**
 * @Brief: Init eris list iterator.
 *
 * @Param: __list, Eris list context.
 * @Param: __iter, Eris list iterator pointer.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_list_iter_init( eris_list_t *__list, eris_list_iter_t *__iter);


/**
 * @Brief: Get current element and iter pointer next.
 *
 * @Param: __iter, Eris list iterator context.
 *
 * @Return: Data of list current iter node.
 **/
extern eris_void_t *eris_list_iter_next( eris_list_iter_t *__iter);


/**
 * @Brief: Get current element and iter pointer prev.
 *
 * @Param: __iter, Eris list iterator context.
 *
 * @Return: Data of list current iter node.
 **/
extern eris_void_t *eris_list_iter_prev( eris_list_iter_t *__iter);


/**
 * @Brief: Reset eris list iterator.
 *
 * @Param: __list, Eris list context.
 * @Param: __iter, Eris list iterator pointer.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_list_iter_reset( eris_list_t *__list, eris_list_iter_t *__iter);


/**
 * @Brief: Cleanup current iter context.
 *
 * @Param: __iter, Eris list iterator context.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_list_iter_cleanup( eris_list_iter_t *__iter);



#ifdef __cplusplus
}
#endif

#endif /** __ERIS_LIST_H__ */

