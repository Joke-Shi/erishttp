#ifndef __ERIS_KQUEUE_H__
#define __ERIS_KQUEUE_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This design event - kqueue I/O, and support api:
 **        :     create
 **        :     add
 **        :     modify 
 **        :     delete
 **        :     dispatch 
 **        :     over 
 **        :     destroy 
 **
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "event/eris.event_base.h"


/**
 * @Brief: Init kqueue context handler.
 *
 * @Param: __event, Event context handler.
 * 
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_kqueue_init( eris_event_t *__event);


/**
 * @Brief: Add socket element in kqueue context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_kqueue_add( eris_event_t *__event, eris_event_elem_t *__elem);


/**
 * @Brief: Modify socket element in kqueue context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_kqueue_modify( eris_event_t *__event, eris_event_elem_t *__elem);


/**
 * @Brief: Delete socket element in kqueue context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_kqueue_delete( eris_event_t *__event, eris_event_elem_t *__elem);


/**
 * @Brief: kqueue dispatcher.
 *
 * @Param: __event,    Event context handler.
 * @Param: __event_cb, Happen event and callback.
 * @Param: __arg,      Input callback argument.
 *
 * @Return: Ok is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_kqueue_dispatch( eris_event_t *__event, eris_event_cb_t __event_cb, eris_arg_t __arg);


/**
 * @Brief: Terminate diapatcher.
 *
 * @Param: __event, Event context handler.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_kqueue_over( eris_event_t *__event);


/**
 * @Brief: Destroy context handler.
 *
 * @Param: __event, Event context handler.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_kqueue_destroy( eris_event_t *__event);



#ifdef __cplusplus
}
#endif

#endif /** __ERIS_KQUEUE_H__ */

