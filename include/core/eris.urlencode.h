#ifndef __ERIS_URLENCODE_H__
#define __ERIS_URLENCODE_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Doing encode or decode of urlencode data.
 **
 ******************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"



/**
 * @Brief: Encode to urlencode of input data.
 *
 * @Param: __out_es, Encode result output urlencode string.
 * @Param: __in_s,   Input data string.
 * @Param: __size,   Input data string size.
 * @param: __log  ,  Dump log context if set.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_urlencode_encode( eris_string_t *__out_es, const eris_char_t *__in_s, eris_size_t __size, eris_log_t *__log);


/**
 * @Brief: Encode to urlencode of input data.
 *
 * @Param: __out_es, Encode result output urlencode string.
 * @Param: __in_es,  Input data string.
 * @param: __log  ,  Dump log context if set.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_urlencode_encode_eris( eris_string_t *__out_es, const eris_string_t __in_es, eris_log_t *__log);


/**
 * @Brief: Encode to urlencode of input data.
 *
 * @Param: __out_es, Encode result output urlencode string.
 * @Param: __in_buf,  Input data buffer.
 * @param: __log  ,  Dump log context if set.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_urlencode_encode_buffer( eris_string_t *__out_es, const eris_buffer_t *__in_buf, eris_log_t *__log);


/**
 * @Brief: Decode urlencode data to stream data.
 *
 * @Param: __out_es, Output decode urlencode string.
 * @Param: __in_es,  Input urlencoded data string.
 * @Param: __size,   Input urlencoded data string size.
 * @param: __log ,   Dump log context if set.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_urlencode_decode( eris_string_t *__out_es, const eris_char_t *__in_s, eris_size_t __size, eris_log_t *__log);


/**
 * @Brief: Decode urlencode data to stream data.
 *
 * @Param: __out_es, Output decode urlencode string.
 * @Param: __in_es,  Input urlencoded data string.
 * @param: __log ,   Dump log context if set.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_urlencode_decode_eris( eris_string_t *__out_es, const eris_string_t __in_es, eris_log_t *__log);


/**
 * @Brief: Decode urlencode data to stream data.
 *
 * @Param: __out_buf,Output decode urlencode buffer..
 * @Param: __in_es,  Input urlencoded data string.
 * @param: __log ,   Dump log context if set.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_urlencode_decode_buffer( eris_buffer_t *__out_buf, const eris_string_t __in_es, eris_log_t *__log);



#ifdef __cplusplus
}
#endif

#endif /** __ERIS_URLENCODE_H__ */


