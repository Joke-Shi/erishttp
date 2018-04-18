#ifndef __ERIS_EVENT_H__
#define __ERIS_EVENT_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This design event: select, poll, epoll, kqueue, devpoll and so on.
 **
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "event/eris.event_base.h"

/** Default select I/O */
#include "event/eris.select.h"

/** devpoll I/O */
#if (ERIS_HAVE_SYS_DEVPOLL_H)
#include "event/eris.devpoll.h"
#endif

/** epoll I/O */
#if (ERIS_HAVE_SYS_EPOLL_H)
#include "event/eris.epoll.h"
#endif

/** kqueue I/O */
#if (ERIS_HAVE_SYS_EVENT_H)
#include "event/eris.kqueue.h"
#endif

/** poll I/O */
#if (ERIS_HAVE_POLL_H)
#include "event/eris.poll.h"
#endif

#include "event/eris.event_queue.h"



/**
 * @Brief: Init event context of event attributes.
 *
 * @Param: __event, Event context.
 * @Param: __attrs, Specify attributes.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_event_init( eris_event_t *__event, eris_event_attr_t *__attrs);


/**
 * @Brief: Add socket element in event context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_event_add( eris_event_t *__event, eris_event_elem_t *__elem);


/**
 * @Brief: Modify socket element in event context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_event_modify( eris_event_t *__event, eris_event_elem_t *__elem);


/**
 * @Brief: Delete socket element in event context monitor.
 *
 * @Param: __event, Event context.
 * @Param: __elem,  Socket element.
 *
 * @Return: Os is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_event_delete( eris_event_t *__event, eris_event_elem_t *__elem);


/**
 * @Brief: Monitor all sockets and dispatch.
 *
 * @Param: __event, Event context.
 * @Param: __cb,    Happen event and doing.
 * @Param: __arg,   Global callback argument.
 *
 * @Return: Os is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_event_dispatch( eris_event_t *__event, eris_event_cb_t __cb, eris_arg_t __arg);


/**
 * @Brief: Set over with dipatch.
 *
 * @Param: __event, Event context.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_event_over( eris_event_t *__event);


/**
 * @Brief: Destroy event context.
 *
 * @Param: __event, Event context.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_event_destroy( eris_event_t *__event);


/**
 * @Brief: Clean up all timeout socket fd.
 *
 * @Param: __event, Event context object.
 * @Param: __cb,    Happen event and doing.
 * @Param: __arg,   Global callback argument.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_event_filing( eris_event_t *__event, eris_event_cb_t __cb, eris_arg_t __arg);


/**
 * @Brief: If monitor accept event and execute.
 *
 * @Param: __event, Event object context.
 * @Param: __svc_sock, Server socket object.
 *
 * @Return: Ok is client socket object.
 **/
extern eris_sock_t eris_event_accept( eris_event_t *__event, eris_sock_t __svc_sock);



#ifdef __cplusplus
}
#endif

#endif  /** __ERIS_EVENT_H__ */

