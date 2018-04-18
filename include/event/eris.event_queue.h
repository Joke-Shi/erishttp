#ifndef __ERIS_EVENT_QUEUE_H__
#define __ERIS_EVENT_QUEUE_H__

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

#ifdef __cplusplus
extern "C" {
#endif

#include "event/eris.event_base.h"


/**
 * @Brief: Eris init event queue.
 *
 * @Param: __event_queue, Eris event queue context.
 * @Param: __max,         Max event queue elements.
 * 
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_event_queue_init( eris_event_queue_t *__event_queue, eris_int_t __max);


/**
 * @Brief: Get event queue capacity.
 *
 * @Param: __event_queue, Eris event queue context.
 *
 * @Return: Ok is queue capacity, other is EERIS_ERROR.
 **/
extern eris_int_t eris_event_queue_capacity( eris_event_queue_t *__event_queue);


/**
 * @Brief: Get event queue count.
 *
 * @Param: __event_queue, Eris event queue context.
 *
 * @Return: Ok is queue count, other is EERIS_ERROR.
 **/
extern eris_int_t eris_event_queue_count( eris_event_queue_t *__event_queue);


/**
 * @Brief: Check event queue is full?
 *
 * @Param: __event_queue, Eris event queue context.
 *
 * @Return: Is full is true, other is false.
 **/
extern eris_bool_t eris_event_queue_isfull( eris_event_queue_t *__event_queue);


/**
 * @Brief: Check event queue is empty?
 *
 * @Param: __event_queue, Eris event queue context.
 *
 * @Return: Is empty is true, other is false.
 **/
extern eris_bool_t eris_event_queue_isempty( eris_event_queue_t *__event_queue);


/**
 * @Brief: Put event element into queue.
 *
 * @Param: __event_queue, Eris event queue context.
 * @Param: __in_elem,     Input event element pointer.
 * @Param: __wait,        If true is wait, other is not wait.
 *
 * @Return: Ok is EERIS_OK, 
 *        : Other error code (EERIS_INVALID, EERIS_FULL).
 *        : Other error code (EERIS_ERROR, EERIS_INVALID, EERIS_FULL).
 **/
extern eris_int_t eris_event_queue_put( eris_event_queue_t *__event_queue, const eris_event_elem_t *__in_elem, eris_bool_t __wait);


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
extern eris_int_t eris_event_queue_get( eris_event_queue_t *__event_queue, eris_event_elem_t *__out_elem, eris_bool_t __wait);


/**
 * @Brief: Destroy eris event queue.
 *
 * @Param: __event_queue, Eris event queue context.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_event_queue_destroy( eris_event_queue_t *__event_queue);



#ifdef __cplusplus
}
#endif

#endif /** __ERIS_EVENT_QUEUE_H__ */


