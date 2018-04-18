#ifndef __ERIS_ERRNO_H__
#define __ERIS_ERRNO_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Defined all eris errno number, and support api get message.
 **
 ******************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"


/** The eris errno code numbers */
//typedef enum eris_errno_e eris_errno_t;
enum eris_errno_e {
    EERIS_ERROR     = -1,        /** Internal error    */
    EERIS_OK        = 0,         /** Ok                */
    EERIS_INVALID   = 10000,     /** Input invalid     */
    EERIS_ALLOC     = 10001,     /** Alloced failed    */
    EERIS_DATA      = 10002,     /** Data is invalid   */
    EERIS_SMALL     = 10003,     /** Object Too small  */
    EERIS_LARGE     = 10004,     /** Object Too large  */
    EERIS_NOTFOUND  = 10005,     /** Not found object  */
    EERIS_NOSUCH    = 10006,     /** No such object    */
    EERIS_SERVICE   = 10007,     /** Service rc error  */
    EERIS_EXISTED   = 10008,     /** Object is existed */
    EERIS_UNSUPPORT = 10009,     /** Object unsupport  */
    EERIS_FULL      = 10010,     /** Object is full    */
    EERIS_EMPTY     = 10011,     /** Object is empty   */
	EERIS_NOACCESS  = 10012,     /** No access perm    */
    EERIS_4XX       = 10013,     /** Http 4xx error    */
    EERIS_5XX       = 10014,     /** Internal error 5xx*/
};
typedef enum eris_errno_e eris_errno_t;


/**
 * @Brief: Get the error message with eris errno.
 *
 * @Param: __content, The output error message content.
 * @Param: __errno  , Specify error code.
 *
 * @Return: Ok is errno string, Other is NULL.
 **/
extern eris_string_t eris_errno_string( eris_string_t *__content, eris_errno_t __errno);


#ifdef __cplusplus
}
#endif

#endif /** __ERIS_ERRNO_H__ */


