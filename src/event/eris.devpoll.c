/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This design event - devpoll I/O, and support api:
 **        :     create
 **        :     add
 **        :     modify 
 **        :     delete
 **        :     dispatch 
 **        :     over 
 **        :     destroy 
 **
 ******************************************************************************/

#include "eris.config.h"

#include "event/eris.devpoll.h"

#include "core/eris.core.h"


/**
 * @Brief: Init devpoll context handler.
 *
 * @Param: __event, Event context handler.
 * 
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_devpoll_init( eris_event_t *__event)
{
    eris_int_t rc = 0;

    if ( __event) {
#if (ERIS_HAVE_SYS_DEVPOLL_H)

#endif /** ERIS_HAVE_SYS_DEVPOLL_H */
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_devpoll_init


/**
 * @Brief: Add socket element in devpoll context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_devpoll_add( eris_event_t *__event, eris_event_elem_t *__elem)
{
    eris_int_t rc = 0;

    if ( __event) {
#if (ERIS_HAVE_SYS_DEVPOLL_H)

#endif /** ERIS_HAVE_SYS_DEVPOLL_H */
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_devpoll_add


/**
 * @Brief: Modify socket element in devpoll context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_devpoll_modify( eris_event_t *__event, eris_event_elem_t *__elem)
{
    eris_int_t rc = 0;

    if ( __event) {
#if (ERIS_HAVE_SYS_DEVPOLL_H)

#endif /** ERIS_HAVE_SYS_DEVPOLL_H */
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_devpoll_modify


/**
 * @Brief: Delete socket element in devpoll context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_devpoll_delete( eris_event_t *__event, eris_event_elem_t *__elem)
{
    eris_int_t rc = 0;

    if ( __event) {
#if (ERIS_HAVE_SYS_DEVPOLL_H)

#endif /** ERIS_HAVE_SYS_DEVPOLL_H */
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_devpoll_delete


/**
 * @Brief: devpoll dispatcher.
 *
 * @Param: __event,    Event context handler.
 * @Param: __event_cb, Happen event and callback.
 * @Param: __arg,      Input callback argument.
 *
 * @Return: Ok is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_devpoll_dispatch( eris_event_t *__event, eris_event_cb_t __event_cb, eris_arg_t __arg)
{
    eris_int_t rc = 0;

    if ( __event) {
#if (ERIS_HAVE_SYS_DEVPOLL_H)

#endif /** ERIS_HAVE_SYS_DEVPOLL_H */
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_devpoll_dispatch


/**
 * @Brief: Terminate diapatcher.
 *
 * @Param: __event, Event context handler.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_devpoll_over( eris_event_t *__event)
{
    if ( __event) {
#if (ERIS_HAVE_SYS_DEVPOLL_H)

#endif /** ERIS_HAVE_SYS_DEVPOLL_H */
    }
}/// eris_devpoll_over


/**
 * @Brief: Destroy context handler.
 *
 * @Param: __event, Event context handler.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_devpoll_destroy( eris_event_t *__event)
{
    if ( __event) {
#if (ERIS_HAVE_SYS_DEVPOLL_H)

#endif /** ERIS_HAVE_SYS_DEVPOLL_H */
    }
}/// eris_devpoll_destroy


