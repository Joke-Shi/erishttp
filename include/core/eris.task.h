#ifndef __ERIS_TASK_H__
#define __ERIS_TASK_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : The tasks manager of progress.
 **        :
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"



/** The task callback */
typedef eris_void_t *( *eris_task_handler_t)( eris_void_t *);


/** The task states */
enum eris_task_state_e {
    ERIS_TASK_NONE    = 0,    /** None         */
    ERIS_TASK_STOPPED    ,    /** Stopped      */
    ERIS_TASK_RUNNING    ,    /** Running      */
    ERIS_TASK_TERMINATED ,    /** Terminated   */
};
typedef enum eris_task_state_e eris_task_state_t;


/** Task attrs node */
typedef struct eris_task_node_s eris_task_node_t;
struct eris_task_node_s {
    eris_task_state_t  state; /** Now state    */
    eris_thread_t      tid;   /** Task id      */
    eris_task_node_t  *next;  /** Next pointer */
};


/** The task handler */
//typedef struct eris_task_s eris_task_t;
struct eris_task_s {
    eris_int_t        count;  /** Task count   */
    eris_task_node_t *head;   /** Task head    */
    eris_task_node_t *tail;   /** Task tail    */
};



/**
 * @Brief: Initialization task context.
 *
 * @Param: __task,  The task handler.
 * @Param: __count, Create count of tasks.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
extern eris_int_t eris_task_init( eris_task_t *__task, eris_int_t __count);


/**
 * @Brief: Create a tasks and manager.
 *
 * @Param: __task,     The task handler.
 * @Param: __start_cb, The start handler.
 * @Param: __arg,      The task input argument.
 * @Param: __log,      Dump log.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
extern eris_int_t eris_task_create( eris_task_t *__task, eris_task_handler_t __start_cb, eris_void_t *__arg, eris_log_t *__log);


/**
 * @Brief: If tasks is stopped, then continue running.
 *
 * @Param: __task, The task handler.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_task_continue( eris_task_t *__task);


/**
 * @Brief: If tasks is running and stop all.
 *
 * @Param: __task, The task handler.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_task_stop( eris_task_t *__task);


/**
 * @Brief: If tasks is running and terminate.
 *
 * @Param: __task, The task handler.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_task_terminate( eris_task_t *__task);


/**
 * @Brief: Create a task jobs pool.
 *
 * @Param: __task,     The task handler.
 * @Param: __start_cb, The start handler.
 * @Param: __arg,      The task input argument.
 * @Param: __log,      Dump log.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
extern eris_int_t eris_task_pool( eris_task_t *__task, eris_task_handler_t __start_cb, eris_void_t *__arg, eris_log_t *__log);


/**
 * @Brief: Destroy task handler.
 *
 * @Param: __task, The task handler.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_task_destroy( eris_task_t *__task);



#ifdef __cplusplus
}
#endif


#endif /** __ERIS_TASK_H__ */

