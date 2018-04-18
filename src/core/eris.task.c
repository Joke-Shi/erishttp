/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : The tasks manager of progress.
 **        :
 ******************************************************************************/

#include "core/eris.core.h"


/**
 * @Brief: Initialization task context.
 *
 * @Param: __task,  The task handler.
 * @Param: __count, Create count of tasks.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
eris_int_t eris_task_init( eris_task_t *__task, eris_int_t __count)
{
    eris_int_t rc = 0;

    if ( __task) {
        __task->count  = (__count >= 0) ? __count : 0;
        __task->head   = NULL;
        __task->tail   = NULL;

        /** Create task table */
        eris_int_t i = 0;

        for ( i = 0; i < __task->count; i++) {
            if ( 0 == i) {
                __task->head = (eris_task_node_t *)eris_memory_alloc( sizeof( eris_task_node_t));
                if ( __task->head) {
                    __task->head->state = ERIS_TASK_NONE;

                    /** Clear */
                    memset( &(__task->head->tid), 0x0, sizeof( __task->head->tid));

                    __task->head->next = NULL;

                    /** Set tail */
                    __task->tail = __task->head;
                } else {
                    /** Alloced failed */
                    rc = EERIS_ERROR;

                    break;
                }
            } else {
                __task->tail->next = (eris_task_node_t *)eris_memory_alloc( sizeof( eris_task_node_t));
                if ( __task->tail->next) {
                    __task->tail        = __task->tail->next;
                    __task->tail->state = ERIS_TASK_NONE;

                    /** Clear */
                    memset( &(__task->tail->tid), 0x0, sizeof( __task->tail->tid));

                    __task->tail->next = NULL;
                } else {
                    /** Alloced failed */
                    rc = EERIS_ERROR;

                    break;
                }
            }
        }
    }

    /** Failed and destroy */
    if ( 0 != rc) { eris_task_destroy( __task); }

    return rc;
}/// eris_task_init


/**
 * @Brief: Create a task job.
 *
 * @Param: __task,     The task handler.
 * @Param: __start_cb, The start handler.
 * @Param: __arg,      The task input argument.
 * @Param: __log,      Dump log.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
eris_int_t eris_task_create( eris_task_t *__task, eris_task_handler_t __start_cb, eris_void_t *__arg, eris_log_t *__log)
{
    eris_int_t rc = 0;

    if ( __task && __start_cb) {
        pthread_attr_t task_attr;

        rc = pthread_attr_init( &task_attr);
        if ( 0 == rc) {
            /** Set detach state */
            (void )pthread_attr_setdetachstate( &task_attr, PTHREAD_CREATE_DETACHED);

            eris_task_node_t *cur_task = (eris_task_node_t *)eris_memory_alloc( sizeof( eris_task_node_t));
            if ( cur_task) {
                __task->count++;

                cur_task->state = ERIS_TASK_NONE;
                cur_task->next  = NULL;

                if ( __task->tail ) {
                    __task->tail->next = cur_task;
                    __task->tail       = cur_task;
                } else {
                    __task->head = cur_task;
                    __task->tail = cur_task;
                }

                /** Create a task job */
                rc = pthread_create( &cur_task->tid, &task_attr, __start_cb, __arg);
                if ( 0 == rc) {
                    cur_task->state = ERIS_TASK_RUNNING;

                } else {
                    rc = EERIS_ERROR;

                    if ( __log) {
                        (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Create a task failed, eris_errno: %d", rc);
                    }
                }
            } else {
                rc = EERIS_ERROR;

                if ( __log) {
                    (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Initialization task attributes failed, eris_errno: %d", errno);
                }
            }

            /** Destroy task attribute */
            pthread_attr_destroy( &task_attr);
        } else {
            rc = EERIS_ERROR;

            if ( __log) {
                (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Initialization task attributes failed, eris_errno: %d", errno);
            }
        }
    } else { 
        rc = EERIS_ERROR; 

        if ( __log) {
            (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Input parameters is invalid, eris_errno: %d", EERIS_INVALID);
        }
    }

    return rc;
}/// eris_task_create


/**
 * @Brief: If tasks is stopped, then continue running.
 *
 * @Param: __task, The task handler.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_task_continue( eris_task_t *__task)
{
    if ( __task) {
        eris_task_node_t *cur_task = __task->head;

        /** Continue running all */
        while ( cur_task) {
            if ( ERIS_TASK_STOPPED == cur_task->state) {
                /** Continue running */
                pthread_kill( cur_task->tid, SIGCONT);

                cur_task->state = ERIS_TASK_RUNNING;
            }

            cur_task = cur_task->next;
        }
    }
}/// eris_task_continue


/**
 * @Brief: If tasks is running and stop all.
 *
 * @Param: __task, The task handler.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_task_stop( eris_task_t *__task)
{
    if ( __task) {
        eris_task_node_t *cur_task = __task->head;

        /** Stop all */
        while ( cur_task) {
            if ( ERIS_TASK_RUNNING == cur_task->state) {
                /** Stop */
                pthread_kill( cur_task->tid, SIGSTOP);

                cur_task->state = ERIS_TASK_STOPPED;
            }

            cur_task = cur_task->next;
        }
    }
}/// eris_task_stop


/**
 * @Brief: If tasks is running and terminate.
 *
 * @Param: __task, The task handler.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_task_terminate( eris_task_t *__task)
{
    if ( __task) {
        eris_task_node_t *cur_task = __task->head;

        /** Terminate all */
        while ( cur_task) {
            if ( ERIS_TASK_RUNNING == cur_task->state) {
                /** Terminate */
                pthread_cancel( cur_task->tid);

                cur_task->state = ERIS_TASK_TERMINATED;
            }

            cur_task = cur_task->next;
        }
    }
}/// eris_task_terminate


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
eris_int_t eris_task_pool( eris_task_t *__task, eris_task_handler_t __start_cb, eris_void_t *__arg, eris_log_t *__log)
{
    eris_int_t rc = 0;

    if ( __task && __start_cb) {
        pthread_attr_t task_attr;

        rc = pthread_attr_init( &task_attr);
        if ( 0 == rc) {
            /** Set detach state */
            (void )pthread_attr_setdetachstate( &task_attr, PTHREAD_CREATE_DETACHED);

            eris_task_node_t *cur_task = __task->head;
            while ( cur_task) {
                if ( (ERIS_TASK_NONE == cur_task->state) || 
                     (ERIS_TASK_TERMINATED == cur_task->state)) {
                    rc = pthread_create( &cur_task->tid, &task_attr, __start_cb, __arg);
                    if ( 0 != rc) {
                        if ( __log) {
                            (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Create a task failed, eris_errno: %d", rc);
                        }

                        /** Failed and end create */
                        rc = EERIS_ERROR; break;
                    } else {
                        /** Ok and next */
                        cur_task->state = ERIS_TASK_RUNNING;

                        cur_task = cur_task->next;
                    }
                } else { cur_task = cur_task->next; }
            }/// while( cur_task)

            /** Destroy task attribute */
            pthread_attr_destroy( &task_attr);
        } else {
            rc = EERIS_ERROR;

            if ( __log) {
                (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Initialization task attributes failed, eris_errno: %d", errno);
            }
        }
    } else { 
        rc = EERIS_ERROR; 

        if ( __log) {
            (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Input parameters is invalid, eris_errno: %d", EERIS_INVALID);
        }
    }

    return rc;
}/// eris_task_pool


/**
 * @Brief: Destroy task handler.
 *
 * @Param: __task, The task handler.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_task_destroy( eris_task_t *__task)
{
    if ( __task) {
        /** Terminate tasks */
        eris_task_terminate( __task);

        /** Release memory */
        eris_task_node_t *cur_task = __task->head;
        while ( cur_task) {
            __task->head = cur_task->next;


            /** Free */
            eris_memory_free( cur_task);
            
            cur_task = __task->head;
        }

        __task->count  = 0;
        __task->head   = NULL;
        __task->tail   = NULL;
    }
}/// eris_task_destroy


