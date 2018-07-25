#ifndef __ERIS_BUFFER_H__
#define __ERIS_BUFFER_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Operator buffer.
 **
 ******************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"


/** Eris-buffer context */
struct eris_buffer_s {
    eris_size_t   alloc;    /** Alloced size  */
    eris_size_t   size;     /** Data size     */
    eris_uchar_t *data;     /** Buffer data   */
};


/**
 * @Brief: Eris-buffer context init.
 *
 * @Param: __buffer, Eris-buffer context.
 * @Param: __cap,    Frist capacity size.
 * @Param: __log,    Happen log and dump there.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_buffer_init( eris_buffer_t *__buffer, eris_size_t __cap, eris_log_t *__log);


/**
 * @Brief: Get buffer data size.
 *
 * @Param: __buffer, Eris-buffer context.
 * @Param: __log,    Happen log and dump there.
 *
 * @Return: Buffer size.
 **/
extern eris_size_t eris_buffer_size( const eris_buffer_t *__buffer, eris_log_t *__log);


/**
 * @Brief: Get buffer capacity.
 *
 * @Param: __buffer, Eris-buffer context.
 * @Param: __log,    Happen log and dump there.
 *
 * @Return: Buffer capacity.
 **/
extern eris_size_t eris_buffer_capacity( const eris_buffer_t *__buffer, eris_log_t *__log);


/**
 * @Brief: Get buffer data.
 *
 * @Param: __buffer, Eris-buffer context.
 * @Param: __log,    Happen log and dump there.
 *
 * @Return: Buffer data pointer.
 **/
extern const eris_void_t *eris_buffer_data( const eris_buffer_t *__buffer, eris_log_t *__log);


/**
 * @Brief: Cleanup buffer data.
 *
 * @Param: __buffer, Eris-buffer context.
 * @Param: __log,    Happen log and dump there.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_buffer_cleanup( eris_buffer_t *__buffer, eris_log_t *__log);


/**
 * @Brief: Free buffer.
 *
 * @Param: __buffer, Eris-buffer context.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_buffer_free( eris_buffer_t *__buffer);


/**
 * @Brief: Append pointer data to buffer tail.
 *
 * @Param: __buffer, Eris-buffer context.
 * @Param: __ptr,    Pointer input data.
 * @Param: __size,   Input data size.
 * @Param: __log,    Happen log and dump there.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_buffer_append( eris_buffer_t *__buffer, const eris_void_t *__ptr, eris_size_t __size, eris_log_t *__log);


/**
 * @Brief: Append a charactor data to buffer tail.
 *
 * @Param: __buffer, Eris-buffer context.
 * @Param: __c,      A charactor.
 * @Param: __log,    Happen log and dump there.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_buffer_append_c( eris_buffer_t *__buffer, const eris_uchar_t __c, eris_log_t *__log);


/**
 * @Brief: Append pointer data to buffer tail of eris buffer.
 *
 * @Param: __src, Source buffer.
 * @Param: __dst, Dest buffer.
 * @Param: __log, Happen log and dump there.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_buffer_append_es( eris_buffer_t *__src, const eris_buffer_t *__dst, eris_log_t *__log);


/**
 * @Brief: Copy source buffer data to destination.
 *
 * @Param: __dst, Destination buffer.
 * @Param: __src, Source buffer.
 * @Param: __log, Happen log and dump there.
 *
 * @Return: Ok is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_buffer_copy( eris_buffer_t *__dst, const eris_buffer_t *__src, eris_log_t *__log);


/**
 * @Brief: Copy source buffer data to destinationa at index to end.
 *
 * @Param: __dst, Destination buffer.
 * @Param: __src, Source buffer.
 * @Param: __at , At pointer of source buffer.
 * @Param: __log, Happen log and dump there.
 *
 * @Return: Ok is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_buffer_copy_at( eris_buffer_t *__dst, const eris_buffer_t *__src, eris_size_t __at, eris_log_t *__log);


/**
 * @Brief: Destroy eris buffer context.
 *
 * @Param: __buffer, Eris-buffer context.
 *
 * @Return: Nothing.
 **/
extern eris_none_t eris_buffer_destroy( eris_buffer_t *__buffer);


#ifdef __cplusplus
}
#endif

#endif /** __ERIS_BUFFER_H__ */


