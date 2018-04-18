/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Operator buffer.
 **
 ******************************************************************************/

#include "core/eris.core.h"


/**
 * @Brief: Eris-buffer context init.
 *
 * @Param: __buffer, Eris-buffer context.
 * @Param: __cap,    Frist capacity size.
 * @Param: __log,    Happen log and dump there.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_buffer_init( eris_buffer_t *__buffer, eris_size_t __cap, eris_log_t *__log)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __buffer) {
        /** Init */
        __buffer->alloc = 0;
        __buffer->size  = 0;
        __buffer->data  = NULL;

        if ( 0 < __cap) {
            __buffer->data  = (eris_uchar_t *)eris_memory_alloc( __cap);
            if ( __buffer->data) {
                __buffer->alloc = __cap;

                eris_memory_cleanup( __buffer->data, __buffer->alloc);
            } else {
                rc = EERIS_ERROR;

                if ( __log) {
                    eris_log_dump( __log, ERIS_LOG_CORE, "Create buffer failed, errno.<%d>", errno);
                }
            }
        }
    } else {
        rc = EERIS_ERROR;

        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input buffer context invalid.");
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_buffer_init


/**
 * @Brief: Get buffer data size.
 *
 * @Param: __buffer, Eris-buffer context.
 * @Param: __log,    Happen log and dump there.
 *
 * @Return: Buffer size.
 **/
eris_size_t eris_buffer_size( const eris_buffer_t *__buffer, eris_log_t *__log)
{
    eris_size_t size = 0;

    if ( __buffer) {
        size = __buffer->size;

    } else {
        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input buffer context invalid.");
        }
    }

    return size;
}/// eris_buffer_size


/**
 * @Brief: Get buffer capacity.
 *
 * @Param: __buffer, Eris-buffer context.
 * @Param: __log,    Happen log and dump there.
 *
 * @Return: Buffer capacity.
 **/
eris_size_t eris_buffer_capacity( const eris_buffer_t *__buffer, eris_log_t *__log)
{
    eris_size_t capacity = 0;

    if ( __buffer) {
        if ( (__buffer->size + 1) < __buffer->alloc) {
            capacity = (__buffer->alloc - __buffer->size);

        }
    } else {
        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input buffer context invalid.");
        }
    }

    return capacity;
}/// eris_buffer_capacity


/**
 * @Brief: Get buffer data.
 *
 * @Param: __buffer, Eris-buffer context.
 * @Param: __log,    Happen log and dump there.
 *
 * @Return: Buffer data pointer.
 **/
const eris_void_t *eris_buffer_data( const eris_buffer_t *__buffer, eris_log_t *__log)
{
    if ( __buffer) {
        return __buffer->data;

    } else {
        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input buffer context invalid.");
        }
    }

    return NULL;
}/// eris_buffer_data


/**
 * @Brief: Cleanup buffer data.
 *
 * @Param: __buffer, Eris-buffer context.
 * @Param: __log,    Happen log and dump there.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_buffer_cleanup( eris_buffer_t *__buffer, eris_log_t *__log)
{
    if ( __buffer) {
        if ( 0 < __buffer->alloc) {
            /** Cleanup */
            __buffer->size = 0;

            ((eris_char_t *)(__buffer->data))[0] = '\0';

        } else {
            __buffer->alloc = 0;
            __buffer->size  = 0;
            __buffer->data  = NULL;
        }
    }
}/// eris_buffer_cleanup


/**
 * @Brief: Free buffer.
 *
 * @Param: __buffer, Eris-buffer context.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_buffer_free( eris_buffer_t *__buffer)
{
    if ( __buffer) {
        if ( (0 < __buffer->alloc) && __buffer->data) {
            eris_memory_free( __buffer->data);
        }

        /** Reset */
        {
            __buffer->alloc = 0;
            __buffer->size  = 0;
            __buffer->data  = NULL;
        }
    }
}/// eris_buffer_free


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
eris_int_t eris_buffer_append( eris_buffer_t *__buffer, const eris_void_t *__ptr, eris_size_t __size, eris_log_t *__log)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __buffer) {
        if ( __ptr && (0 < __size)) {
            /** Have data and append to buffer */
            if ( __size  >= (__buffer->alloc - __buffer->size)) {
                eris_size_t realloc_size = __buffer->alloc + __size;

                __buffer->data = (eris_uchar_t *)eris_memory_realloc( __buffer->data, realloc_size);
                if ( __buffer->data) {
                    __buffer->alloc = realloc_size;

                } else {
                    __buffer->alloc = 0;
                    __buffer->size  = 0;

                    rc = EERIS_ERROR;

                    if ( __log) {
                        eris_log_dump( __log, ERIS_LOG_CORE, "Realloc memory for buffer failed, errno.<%d>", errno);
                    }
                }
            }

            /** Up ok and copy append */
            if ( 0 == rc) {
                eris_memory_copy( (eris_uchar_t *)__buffer->data + __buffer->size, __ptr, __size);

                __buffer->size += __size;
                ((eris_uchar_t *)__buffer->data)[ __buffer->size] = '\0';
            }
        }
    } else {
        rc = EERIS_ERROR;

        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input buffer context is invalid.");
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_buffer_append


/**
 * @Brief: Append a charactor data to buffer tail.
 *
 * @Param: __buffer, Eris-buffer context.
 * @Param: __c,      A charactor.
 * @Param: __log,    Happen log and dump there.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_buffer_append_c( eris_buffer_t *__buffer, const eris_uchar_t __c, eris_log_t *__log)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __buffer) {
        /** Append to buffer tail */
        if ( 1 >= (__buffer->alloc - __buffer->size)) {
            eris_size_t realloc_size = __buffer->alloc + 8;

            __buffer->data = (eris_uchar_t *)eris_memory_realloc( __buffer->data, realloc_size);
            if ( __buffer->data) {
                __buffer->alloc = realloc_size;

            } else {
                __buffer->alloc = 0;
                __buffer->size  = 0;

                rc = EERIS_ERROR;

                if ( __log) {
                    eris_log_dump( __log, ERIS_LOG_CORE, "Realloc memory for buffer failed, errno.<%d>", errno);
                }
            }
        }

        /** Up ok and copy append */
        if ( 0 == rc) {
            ((eris_uchar_t *)__buffer->data)[ __buffer->size] = __c;

            __buffer->size += 1;
            __buffer->data[ __buffer->size] = '\0';
        }
    } else {
        rc = EERIS_ERROR;

        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input buffer context is invalid.");
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_buffer_append_c


/**
 * @Brief: Append pointer data to buffer tail of eris buffer.
 *
 * @Param: __src, Source buffer.
 * @Param: __dst, Dest buffer.
 * @Param: __log, Happen log and dump there.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_buffer_append_eris( eris_buffer_t *__src, const eris_buffer_t *__dst, eris_log_t *__log)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __src) {
        if ( __dst && (0 < __dst->size)) {
            /** Have data and append to buffer */
            if ( __dst->size > (__src->alloc - __src->size)) {
                eris_size_t realloc_size = __src->alloc + __dst->size;

                __src->data = (eris_uchar_t *)eris_memory_realloc( __src->data, realloc_size);
                if ( __src->data) {
                    __src->alloc = realloc_size;

                } else {
                    __src->alloc = 0;
                    __src->alloc = 0;

                    rc = EERIS_ERROR;

                    if ( __log) {
                        eris_log_dump( __log, ERIS_LOG_CORE, "Realloc memory for buffer failed, errno.<%d>", errno);
                    }
                }
            }

            /** Up ok and copy append */
            if ( 0 == rc) {
                eris_memory_copy( (eris_uchar_t *)__src->data + __src->size, __dst->data, __dst->size);

                __src->size += __dst->size;
                ((eris_uchar_t *)__src->data)[ __src->size] = '\0';
            }
        }
    } else {
        rc = EERIS_ERROR;

        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input buffer context is invalid.");
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_buffer_append_eris


/**
 * @Brief: Copy source buffer data to destination.
 *
 * @Param: __dst, Destination buffer.
 * @Param: __src, Source buffer.
 * @Param: __log, Happen log and dump there.
 *
 * @Return: Ok is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_buffer_copy( eris_buffer_t *__dst, const eris_buffer_t *__src, eris_log_t *__log)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __src && __dst) {
        if ( 0 < __src->size) {
            /** Copy */
            if ( (__dst->alloc > __src->size) && __dst->data) {
                eris_memory_copy( __dst->data, __src->data, __src->size);

                __dst->size = __src->size;
                ((eris_uchar_t *)__dst->data)[ __dst->size] = '\0';

            } else {
                if ( __dst->data && (0 < __dst->alloc)) {
                    /** Realloc for dst buffer */
                    __dst->data = (eris_uchar_t *)eris_memory_realloc( __dst->data, __src->alloc);
                    if ( __dst->data) {
                        __dst->alloc = __src->alloc;

                    } else {
                        rc = EERIS_ERROR;
                        
                        if ( __log) {
                            eris_log_dump( __log, ERIS_LOG_CORE, "Realloced memory for buffer failed, errno.<%d>", errno);
                        }
                    }
                } else {
                    /** Alloc for dst buffer */
                    __dst->data = (eris_uchar_t *)eris_memory_alloc( __src->alloc);
                    if ( __dst->data) {
                        __dst->alloc = __src->alloc;

                    } else {
                        rc = EERIS_ERROR;
                        
                        if ( __log) {
                            eris_log_dump( __log, ERIS_LOG_CORE, "Realloced memory for buffer failed, errno.<%d>", errno);
                        }
                    }
                }

                /** Copy memory data */
                if ( 0 == rc) {
                    eris_memory_copy( __dst->data, __src->data, __src->size);

                    __dst->size = __src->size;
                    ((eris_uchar_t *)__dst->data)[ __dst->size] = '\0';
                }
            }
        } else { 
            /** Nothing copy */
            __dst->size  = 0; 
        }
    } else {
        rc = EERIS_ERROR;

        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input source or destiantion context is invalid");
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_buffer_copy


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
eris_int_t eris_buffer_copy_at( eris_buffer_t *__dst, const eris_buffer_t *__src, eris_size_t __at, eris_log_t *__log)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __src && __dst) {
        if ( __src->size > __at) {
            eris_size_t copy_size = __src->size - __at;
            if ( copy_size < __dst->alloc) {
                eris_memory_copy( __dst->data, ((eris_char_t *)__src->data) + __at, copy_size);

                __dst->size = copy_size;
                ((eris_uchar_t *)__dst->data)[ __dst->size] = '\0';

            } else {
                if ( (0 == __dst->alloc) && !(__dst->data)) {
                    __dst->data = (eris_uchar_t *)eris_memory_alloc( copy_size + 1);
                    if ( __dst->data) {
                        __dst->alloc = copy_size + 1;

                        eris_memory_copy( __dst->data, (eris_uchar_t *)__src->data + __at, copy_size);

                        __dst->size  = copy_size;
                        ((eris_uchar_t *)__dst->data)[ __dst->size] = '\0';
                    } else {
                        __dst->alloc = 0;
                        __dst->size  = 0;

                        rc = EERIS_ERROR;

                        if ( __log) {
                            eris_log_dump( __log, ERIS_LOG_CORE, "Alloced memory for buffer failed, errno.<%d>", errno);
                        }
                    }
                } else {
                    eris_memory_free( __dst->data);

                    __dst->data = (eris_uchar_t *)eris_memory_alloc( copy_size + 1);
                    if ( __dst->data) {
                        eris_memory_copy( __dst->data, (eris_uchar_t *)__src->data + __at, copy_size);

                        __dst->alloc = copy_size + 1;
                        __dst->size  = copy_size;

                        ((eris_uchar_t *)__dst->data)[ __dst->size] = '\0';
                    } else {
                        rc = EERIS_ERROR;

                        __dst->alloc = 0;
                        __dst->size  = 0;

                        if ( __log) {
                            eris_log_dump( __log, ERIS_LOG_CORE, "Alloced memory for buffer failed, errno.<%d>", errno);
                        }
                    }
                }
            }
        } else {
            /** Nothing copy */
            __dst->size = 0;
        }
    } else {
        rc = EERIS_ERROR;

        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input source or destiantion context is invalid");
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_buffer_copy_at




/**
 * @Brief: Destroy eris buffer context.
 *
 * @Param: __buffer, Eris-buffer context.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_buffer_destroy( eris_buffer_t *__buffer)
{
    if ( __buffer) {
        if ( (0 < __buffer->alloc) && __buffer->data) {
            /** Release memory */
            eris_memory_free( __buffer->data);
        }

        /** Destroy */
        __buffer->alloc = 0;
        __buffer->size  = 0;
        __buffer->data  = NULL;
    }
}/// eris_buffer_destroy


