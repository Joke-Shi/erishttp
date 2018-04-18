/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This design event queue for event store, support api:
 **        :     init
 **        :     get
 **        :     put
 **        :     destroy 
 **
 ******************************************************************************/

#include "eris.config.h"

#include "core/eris.memory.h"

#include "event/eris.event_queue.h"



/**
 * @Brief: Eris init event queue.
 *
 * @Param: __event_queue, Eris event queue context.
 * @Param: __max,         Max event queue elements.
 * 
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_event_queue_init( eris_event_queue_t *__event_queue, eris_int_t __max)
{
    eris_int_t rc = 0;

    __max = (0 < __max) ? __max : 1024;

    if ( __event_queue) {
        __event_queue->max   = __max;
        __event_queue->count = 0;
        __event_queue->front = 0;
        __event_queue->near  = 0;

        rc = pthread_mutex_init( &(__event_queue->mutex), NULL);
        if ( 0 == rc) {
            rc = pthread_cond_init( &(__event_queue->cond_put), NULL);
            if ( 0 == rc) {
                rc = pthread_cond_init( &(__event_queue->cond_get), NULL);
                if ( 0 == rc) {
                    /** Create event queue pool */
                    __event_queue->events = (eris_event_elem_t *)eris_memory_alloc( sizeof( eris_event_elem_t) * __event_queue->max);
                    if ( __event_queue->events) {
                        eris_int_t i = 0;

                        for ( i = 0; i < __event_queue->max; i++) {
                            __event_queue->events[ i].sock   = -1;
                            __event_queue->events[ i].events = ERIS_EVENT_NONE;

                        }
                    } else {
                        rc = EERIS_ERROR;

                        pthread_cond_destroy( &(__event_queue->cond_get));
                        pthread_cond_destroy( &(__event_queue->cond_put));
                        pthread_mutex_destroy(&(__event_queue->mutex));
                    }
                } else { 
                    rc = EERIS_ERROR; 

                    pthread_cond_destroy( &(__event_queue->cond_put));
                    pthread_mutex_destroy(&(__event_queue->mutex));
                }
            } else { 
                rc = EERIS_ERROR; 

                pthread_mutex_destroy(&(__event_queue->mutex));
            }
        } else { rc = EERIS_ERROR; }
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_event_queue_init


/**
 * @Brief: Get event queue capacity.
 *
 * @Param: __event_queue, Eris event queue context.
 *
 * @Return: Ok is queue capacity, other is EERIS_ERROR.
 **/
eris_int_t eris_event_queue_capacity( eris_event_queue_t *__event_queue)
{
    eris_int_t rc = 0;

    if ( __event_queue) {
        pthread_cleanup_push( (eris_cleanup_routine_t)pthread_mutex_unlock, (eris_void_t *)&(__event_queue->mutex));
        /** Heppen cancel? */

        rc = pthread_mutex_lock( &(__event_queue->mutex));
        if ( 0 == rc) {
            rc = __event_queue->max - __event_queue->count;

            /** Unlock */
            pthread_mutex_unlock( &(__event_queue->mutex));

        } else { rc = EERIS_ERROR; }

        pthread_cleanup_pop( 0);

    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_event_queue_capacity


/**
 * @Brief: Get event queue count.
 *
 * @Param: __event_queue, Eris event queue context.
 *
 * @Return: Ok is queue count, other is EERIS_ERROR.
 **/
eris_int_t eris_event_queue_count( eris_event_queue_t *__event_queue)
{
    eris_int_t rc = 0;

    if ( __event_queue) {
        pthread_cleanup_push( (eris_cleanup_routine_t)pthread_mutex_unlock, (eris_void_t *)&(__event_queue->mutex));
        /** Heppen cancel? */

        rc = pthread_mutex_lock( &(__event_queue->mutex));
        if ( 0 == rc) {
            rc = __event_queue->count;

            /** Unlock */
            pthread_mutex_unlock( &(__event_queue->mutex));

        } else { rc = EERIS_ERROR; }

        pthread_cleanup_pop( 0);

    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_event_queue_count


/**
 * @Brief: Check event queue is full?
 *
 * @Param: __event_queue, Eris event queue context.
 *
 * @Return: Is full is true, other is false.
 **/
eris_bool_t eris_event_queue_isfull( eris_event_queue_t *__event_queue)
{
    eris_bool_t rc = false;

    if ( __event_queue) {
        pthread_cleanup_push( (eris_cleanup_routine_t)pthread_mutex_unlock, (eris_void_t *)&(__event_queue->mutex));
        /** Heppen cancel? */

        /** Lock ok? */
        if ( 0 == pthread_mutex_lock( &(__event_queue->mutex))) {
            if ( __event_queue->max <= __event_queue->count) {
                rc = true;
            }

            /** Unlock */
            pthread_mutex_unlock( &(__event_queue->mutex));
        }

        pthread_cleanup_pop(0);
    }

    return rc;
}/// eris_event_queue_isfull


/**
 * @Brief: Check event queue is empty?
 *
 * @Param: __event_queue, Eris event queue context.
 *
 * @Return: Is empty is true, other is false.
 **/
eris_bool_t eris_event_queue_isempty( eris_event_queue_t *__event_queue)
{
    eris_bool_t rc = false;

    if ( __event_queue) {
        pthread_cleanup_push( (eris_cleanup_routine_t)pthread_mutex_unlock, (eris_void_t *)&(__event_queue->mutex));
        /** Heppen cancel? */

        /** Lock ok? */
        if ( 0 == pthread_mutex_lock( &(__event_queue->mutex))) {
            if ( 0 >= __event_queue->count) {
                rc = true;
            }

            /** Unlock */
            pthread_mutex_unlock( &(__event_queue->mutex));
        }

        pthread_cleanup_pop(0);
    }

    return rc;
}/// eris_event_queue_isempty


/**
 * @Brief: Put event element into queue.
 *
 * @Param: __event_queue, Eris event queue context.
 * @Param: __in_elem,     Input event element pointer.
 * @Param: __wait,        If true is wait, other is not wait.
 *
 * @Return: Ok is EERIS_OK, 
 *        : Other error code (EERIS_ERROR, EERIS_INVALID, EERIS_FULL).
 **/
eris_int_t eris_event_queue_put( eris_event_queue_t *__event_queue, const eris_event_elem_t *__in_elem, eris_bool_t __wait)
{
    eris_int_t rc = 0;

    if ( __event_queue) {
        if ( __in_elem) {
            pthread_cleanup_push( (eris_cleanup_routine_t)pthread_mutex_unlock, (eris_void_t *)&(__event_queue->mutex));
            /** Heppen cancel? */

            /** Safe-thread lock */
            rc = pthread_mutex_lock( &(__event_queue->mutex));
            if ( 0 == rc) {
                while ( __event_queue->max <= __event_queue->count) {
                    if ( __wait) {
                        /** Wait, and continue */
                        pthread_cond_wait( &(__event_queue->cond_put), &(__event_queue->mutex));

                    } else {
                        /** No wait */
                        rc = EERIS_FULL;

                        break;
                    }
                }

                /** Has vacant position */
                if ( 0 == rc) {
                    __event_queue->events[ __event_queue->near].sock   = __in_elem->sock;
                    __event_queue->events[ __event_queue->near].events = __in_elem->events;

                    __event_queue->near = (__event_queue->near + 1) % __event_queue->max;
                    __event_queue->count++;
                }

                /** Notify other monitor condition object */
                {
                    pthread_mutex_unlock( &(__event_queue->mutex));
                    pthread_cond_signal( &(__event_queue->cond_put));
                    pthread_cond_signal( &(__event_queue->cond_get));
                }
            } else { rc = EERIS_ERROR; }

            pthread_cleanup_pop(0);
        }
    } else { rc = EERIS_INVALID; }

    return rc;
}/// eris_event_queue_put


/**
 * @Brief: Get event element from queue.
 *
 * @Param: __event_queue, Eris event queue context.
 * @Param: __out_elem,    Output event element pointer.
 * @Param: __wait,        If true is wait, other is not wait.
 *
 * @Return: Ok is EERIS_OK, 
 *        : Other error code (EERIS_ERROR, EERIS_INVALID, EERIS_EMPTY).
 **/
eris_int_t eris_event_queue_get( eris_event_queue_t *__event_queue, eris_event_elem_t *__out_elem, eris_bool_t __wait)
{
    eris_int_t rc = 0;

    if ( __event_queue) {
        if ( __out_elem) {
            pthread_cleanup_push( (eris_cleanup_routine_t)pthread_mutex_unlock, (eris_void_t *)&(__event_queue->mutex));
            /** Heppen cancel? */

            /** Safe-thread lock */
            rc = pthread_mutex_lock( &(__event_queue->mutex));
            if ( 0 == rc) {
                while ( 0 == __event_queue->count) {
                    if ( __wait) {
                        /** Wait, and continue */
                        pthread_cond_wait( &(__event_queue->cond_get), &(__event_queue->mutex));

                    } else {
                        /** No wait */
                        rc = EERIS_EMPTY;

                        break;
                    }
                }

                /** Has event element, and get output */
                if ( 0 == rc) {
                    __out_elem->sock   = __event_queue->events[ __event_queue->front].sock;
                    __out_elem->events = __event_queue->events[ __event_queue->front].events;

                    __event_queue->front = (__event_queue->front + 1) % __event_queue->max;
                    __event_queue->count--;
                }

                /** Notify other monitor condition object */
                {
                    pthread_mutex_unlock( &(__event_queue->mutex));
                    pthread_cond_signal( &(__event_queue->cond_put));
                    pthread_cond_signal( &(__event_queue->cond_get));
                }
            } else { rc = EERIS_ERROR; }

            pthread_cleanup_pop(0);
        }
    } else { rc = EERIS_INVALID; }

    return rc;
}/// eris_event_queue_get


/**
 * @Brief: Destroy eris event queue.
 *
 * @Param: __event_queue, Eris event queue context.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_event_queue_destroy( eris_event_queue_t *__event_queue)
{
    if ( __event_queue) {
        __event_queue->front = 0;
        __event_queue->near  = 0;

        /** Destory mutex, cond, and events pool */
        pthread_mutex_lock( &(__event_queue->mutex)); {
            eris_int_t i = __event_queue->front;

            for ( ; (__event_queue->count > 0); ) {
                eris_socket_close( __event_queue->events[ i].sock);

                __event_queue->events[ i].sock   = -1;
                __event_queue->events[ i].events = ERIS_EVENT_NONE;

                i++; __event_queue->count--;

                if ( i >= __event_queue->max) {
                    i = 0;
                }
            }

            __event_queue->max   = 0;
            __event_queue->count = 0;

            /** Release ans set NULL */
            {
                eris_memory_free( __event_queue->events);

                pthread_cond_destroy( &(__event_queue->cond_put));
                pthread_cond_destroy( &(__event_queue->cond_get));
            }
        }

        pthread_mutex_unlock( &(__event_queue->mutex));
        pthread_mutex_destroy( &(__event_queue->mutex));
    }
}/// eris_event_queue_destroy


