/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Defined all eris errno number, and support api get message.
 **
 ******************************************************************************/

#include "core/eris.core.h"


/**
 * @Brief: Get the error message with eris errno.
 *
 * @Param: __content, The output error message content.
 * @Param: __errno  , Specify error code.
 *
 * @Return: Ok is errno string, Other is NULL.
 **/
eris_string_t eris_errno_string( eris_string_t *__content, eris_errno_t __errno)
{
    eris_int_t rc = 0;

    if ( __content) {
        eris_string_cleanup( *__content);

        /** Match error code */
        switch ( __errno) {
            case EERIS_ERROR :
                {
                    rc = eris_string_set( __content, "Internal error");
                } break;
            case EERIS_OK :
                {
                    rc = eris_string_set( __content, "Successed");
                } break;
            case EERIS_INVALID :
                {
                    rc = eris_string_set( __content, "Input objects is invalid");
                } break;
            case EERIS_ALLOC :
                {
                    rc = eris_string_set( __content, "Unenough memory with alloced");
                } break;
            case EERIS_DATA :
                {
                    rc = eris_string_set( __content, "Input data is bad");
                } break;
            case EERIS_SMALL :
                {
                    rc = eris_string_set( __content, "The object is too small");
                } break;
            case EERIS_LARGE :
                {
                    rc = eris_string_set( __content, "The object is too larger");
                } break;
            case EERIS_NOTFOUND :
                {
                    rc = eris_string_set( __content, "Not found object");
                } break;
            case EERIS_SERVICE :
                {
                    rc = eris_string_set( __content, "User service internal error");
                } break;
            case EERIS_NOSUCH :
                {
                    rc = eris_string_set( __content, "Not such object");
                } break;
            case EERIS_EXISTED :
                {
                    rc = eris_string_set( __content, "The object is existed");
                } break;
            case EERIS_UNSUPPORT:
                {
                    rc = eris_string_set( __content, "Object service unsupport");
                } break;
            case EERIS_FULL:
                {
                    rc = eris_string_set( __content, "Object is full");
                }
            case EERIS_EMPTY:
                {
                    rc = eris_string_set( __content, "Object is empty");
                }
            case EERIS_NOACCESS:
                {
                    rc = eris_string_set( __content, "No access permission");
                }
            case EERIS_4XX:
                {
                    rc = eris_string_set( __content, "Http request 4xx error");
                }
            case EERIS_5XX:
                {
                    rc = eris_string_set( __content, "Server internal error 5xx");
                }
            default : 
                {
                    const eris_char_t *p = strerror( __errno);
                    rc = eris_string_append( __content, p);
                } break;
        } /// switch( __errno)

        /** Store error message */
    } else { rc = EERIS_ERROR; }

    return (0 == rc) ? (*__content) : NULL;
}/// eris_errno_content


