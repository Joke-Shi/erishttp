/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Doing encode or decode of urlencode data.
 **
 ******************************************************************************/

#include "core/eris.core.h"



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
eris_int_t eris_urlencode_encode( eris_string_t *__out_es, const eris_char_t *__in_s, eris_size_t __size, eris_log_t *__log)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __out_es && __in_s) {
        const eris_char_t *hexs = "0123456789ABCDEF";

        eris_string_cleanup( *__out_es);

        /** Encode data to urlencode string */
        eris_size_t i    = 0;

        for ( i = 0; i < __size; i++) {
            eris_uchar_t c = (eris_uchar_t)__in_s[ i];

            if ( ' ' == c) {
                rc = eris_string_append_c( __out_es, '+');
                if ( 0 != rc) {
                    rc = EERIS_ERROR;

                    if ( __log) {
                        eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '+' failed, errno.<%d>", errno);
                    }
                }
            } else if ( isalnum( c) ||
                       ('.' == c)   ||
                       ('-' == c)   ||
                       ('_' == c)   ||
                       ('~' == c)
                     ) {
                rc = eris_string_append_c( __out_es, c);
                if ( 0 != rc) {
                    rc = EERIS_ERROR;

                    if ( __log) {
                        eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '%c' failed, errno.<%d>", c, errno);
                    }
                }
            } else {
                eris_char_t encode_tmp[ 4] = {0}; {
                    encode_tmp[0] = '%';
                    encode_tmp[1] = hexs[ 0x0F & (c >> 4)];
                    encode_tmp[2] = hexs[ 0x0F & c];
                    encode_tmp[3] = '\0';
                }

                rc = eris_string_append( __out_es, encode_tmp);
                if ( 0 != rc) {
                    rc = EERIS_ERROR;

                    if ( __log) {
                        eris_log_dump( __log, ERIS_LOG_CORE, "Save urlencode '%s' string failed, errno.<%d>", encode_tmp, errno);
                    }
                }
            }

            /** Happen error */
            if ( 0 != rc) { break; }
        }
    } else {
        rc = EERIS_ERROR;

        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input string or data is invalid");
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_urlencode_encode


/**
 * @Brief: Encode to urlencode of input data.
 *
 * @Param: __out_es, Encode result output urlencode string.
 * @Param: __in_es,  Input data string.
 * @param: __log  ,  Dump log context if set.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_urlencode_encode_eris( eris_string_t *__out_es, const eris_string_t __in_es, eris_log_t *__log)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __out_es && __in_es) {
        const eris_char_t *hexs = "0123456789ABCDEF";

        eris_string_cleanup( *__out_es);

        /** Encode data to urlencode string */
        eris_size_t i    = 0;
        eris_size_t size = eris_string_size( __in_es);

        for ( i = 0; i < size; i++) {
            eris_uchar_t c = (eris_uchar_t)eris_string_at( __in_es, i);

            if ( ' ' == c) {
                rc = eris_string_append_c( __out_es, '+');
                if ( 0 != rc) {
                    rc = EERIS_ERROR;

                    if ( __log) {
                        eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '+' failed, errno.<%d>", errno);
                    }
                }
            } else if ( isalnum( c) ||
                       ('.' == c)   ||
                       ('-' == c)   ||
                       ('_' == c)   ||
                       ('~' == c)
                     ) {
                rc = eris_string_append_c( __out_es, c);
                if ( 0 != rc) {
                    rc = EERIS_ERROR;

                    if ( __log) {
                        eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '%c' failed, errno.<%d>", c, errno);
                    }
                }
            } else {
                eris_char_t encode_tmp[ 4] = {0}; {
                    encode_tmp[0] = '%';
                    encode_tmp[1] = hexs[ 0x0F & (c >> 4)];
                    encode_tmp[2] = hexs[ 0x0F & c];
                    encode_tmp[3] = '\0';
                }

                rc = eris_string_append( __out_es, encode_tmp);
                if ( 0 != rc) {
                    rc = EERIS_ERROR;

                    if ( __log) {
                        eris_log_dump( __log, ERIS_LOG_CORE, "Save urlencode '%s' string failed, errno.<%d>", encode_tmp, errno);
                    }
                }
            }

            /** Happen error */
            if ( 0 != rc) { break; }
        }
    } else {
        rc = EERIS_ERROR;

        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input string or data is invalid");
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_urlencode_encode


/**
 * @Brief: Encode to urlencode of input data.
 *
 * @Param: __out_es, Encode result output urlencode string.
 * @Param: __in_buf,  Input data buffer.
 * @param: __log  ,  Dump log context if set.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_urlencode_encode_buffer( eris_string_t *__out_es, const eris_buffer_t *__in_buf, eris_log_t *__log)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __out_es && __in_buf) {
        const eris_char_t *hexs = "0123456789ABCDEF";

        eris_string_cleanup( *__out_es);

        /** Encode data to urlencode string */
        eris_size_t i    = 0;
        eris_size_t size = __in_buf->size;

        for ( i = 0; i < size; i++) {
            eris_uchar_t c = ((eris_uchar_t *)(__in_buf->data))[ i];

            if ( ' ' == c) {
                rc = eris_string_append_c( __out_es, '+');
                if ( 0 != rc) {
                    rc = EERIS_ERROR;

                    if ( __log) {
                        eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '+' failed, errno.<%d>", errno);
                    }
                }
            } else if ( isalnum( c) ||
                       ('.' == c)   ||
                       ('-' == c)   ||
                       ('_' == c)   ||
                       ('~' == c)
                     ) {
                rc = eris_string_append_c( __out_es, c);
                if ( 0 != rc) {
                    rc = EERIS_ERROR;

                    if ( __log) {
                        eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '%c' failed, errno.<%d>", c, errno);
                    }
                }
            } else {
                eris_char_t encode_tmp[ 4] = {0}; {
                    encode_tmp[0] = '%';
                    encode_tmp[1] = hexs[ 0x0F & (c >> 4)];
                    encode_tmp[2] = hexs[ 0x0F & c];
                    encode_tmp[3] = '\0';
                }

                rc = eris_string_append( __out_es, encode_tmp);
                if ( 0 != rc) {
                    rc = EERIS_ERROR;

                    if ( __log) {
                        eris_log_dump( __log, ERIS_LOG_CORE, "Save urlencode '%s' string failed, errno.<%d>", encode_tmp, errno);
                    }
                }
            }

            /** Happen error */
            if ( 0 != rc) { break; }
        }
    } else {
        rc = EERIS_ERROR;

        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input string or data is invalid");
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_urlencode_encode_buffer


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
eris_int_t eris_urlencode_decode( eris_string_t *__out_es, const eris_char_t *__in_s, eris_size_t __size, eris_log_t *__log)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __out_es && __in_s) {
        eris_string_cleanup( *__out_es);

        /** Urlencode states of decode doing */
        enum {
            ERIS_URLENCODE_START   = 0,
            ERIS_URLENCODE_ALNUM   ,
            ERIS_URLENCODE_SIGN    ,
            ERIS_URLENCODE_PERCENT ,
            ERIS_URLENCODE_HEX1    ,
            ERIS_URLENCODE_HEX2    ,
        } eris_urlencode_state_v = ERIS_URLENCODE_START;

        eris_uchar_t b64_2_c = 0;
        eris_size_t  i       = 0;

        /** Decode urluncode data */
        for ( i = 0; i < __size; i++) {
            eris_uchar_t c = (eris_uchar_t )__in_s[ i];

            switch ( eris_urlencode_state_v) {
                case ERIS_URLENCODE_START :
                    {
                        if ( '%' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_PERCENT;

                        } else if ( '+' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_SIGN;

                            rc = eris_string_append_c( __out_es, ' ');
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char ' ' failed, errno.<%d>", errno);
                                }
                            }
                        } else {
                            eris_urlencode_state_v = ERIS_URLENCODE_ALNUM;

                            rc = eris_string_append_c( __out_es, c);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '%c' failed, errno.<%d>", c, errno);
                                }
                            }
                        }
                    } break;
                case ERIS_URLENCODE_ALNUM :
                    {
                        if ( '%' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_PERCENT;

                        } else if ( '+' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_SIGN;

                            rc = eris_string_append_c( __out_es, ' ');
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char ' ' failed, errno.<%d>", errno);
                                }
                            }
                        } else {
                            rc = eris_string_append_c( __out_es, c);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '%c' failed, errno.<%d>", c, errno);
                                }
                            }
                        }
                    } break;
                case ERIS_URLENCODE_SIGN :
                    {
                        if ( '%' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_PERCENT;

                        } else if ( '+' == c) {
                            rc = eris_string_append_c( __out_es, ' ');
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char ' ' failed, errno.<%d>", errno);
                                }
                            }
                        } else {
                            eris_urlencode_state_v = ERIS_URLENCODE_ALNUM;

                            rc = eris_string_append_c( __out_es, c);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '%c' failed, errno.<%d>", c, errno);
                                }
                            }
                        }
                    } break;
                case ERIS_URLENCODE_PERCENT :
                    {
                        if ( ERIS_CHAR_IS_HEX(c)) {
                            eris_urlencode_state_v = ERIS_URLENCODE_HEX1;

                            if ( ('0' <= c) && ('9' >= c)) {
                                b64_2_c = (0xF0 & ((c - '0') << 4));

                            } else if ( ('a' <= c) && ('z' >= c)) {
                                b64_2_c = (0xF0 & ((c - 'a' + 10) << 4));

                            } else {
                                b64_2_c = (0xF0 & ((c - 'A' + 10) << 4));
                            }
                        } else {
                            rc = EERIS_ERROR;

                            if ( __log) {
                                eris_log_dump( __log, ERIS_LOG_CORE, "Save a '%c' is not urlencode hex charactor", c);
                            }
                        }
                    } break;
                case ERIS_URLENCODE_HEX1 :
                    {
                        if ( ERIS_CHAR_IS_HEX(c)) {
                            eris_urlencode_state_v = ERIS_URLENCODE_HEX2;

                            if ( ('0' <= c) && ('9' >= c)) {
                                b64_2_c |= (0x0F & (c - '0'));

                            } else if ( ('a' <= c) && ('f' >= c)) {
                                b64_2_c |= (0x0F & (c - 'a' + 10));

                            } else {
                                b64_2_c |= (0x0F & (c - 'A' + 10));
                            }

                            rc = eris_string_append_c( __out_es, b64_2_c);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '%c' failed, errno.<%d>", b64_2_c, errno);
                                }
                            }

                            /** Reset */
                            b64_2_c = 0;
                        } else {
                            rc = EERIS_ERROR;

                            if ( __log) {
                                eris_log_dump( __log, ERIS_LOG_CORE, "Save a '%c' is not urlencode hex charactor", c);
                            }
                        }
                    } break;
                case ERIS_URLENCODE_HEX2 :
                    {
                        if ( '%' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_PERCENT;

                        } else if ( '+' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_SIGN;

                            rc = eris_string_append_c( __out_es, ' ');
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char ' ' failed, errno.<%d>", errno);
                                }
                            }
                        } else {
                            eris_urlencode_state_v = ERIS_URLENCODE_ALNUM;

                            rc = eris_string_append_c( __out_es, c);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '%c' failed, errno.<%d>", c, errno);
                                }
                            }
                        }
                    } break;
                default : break;
            }/// switch ( eris_urlencode_state_v)

            /** Happen error */
            if ( 0 != rc) { break; }

        }/// for ( i = 0; i < __size; i++)
    } else {
        rc = EERIS_ERROR;

        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input string or data is invalid");
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_urlencode_decode


/**
 * @Brief: Decode urlencode data to stream data.
 *
 * @Param: __out_es, Output decode urlencode string.
 * @Param: __in_es,  Input urlencoded data string.
 * @param: __log ,   Dump log context if set.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_urlencode_decode_eris( eris_string_t *__out_es, const eris_string_t __in_es, eris_log_t *__log)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __out_es && __in_es) {
        eris_string_cleanup( *__out_es);

        /** Urlencode states of decode doing */
        enum {
            ERIS_URLENCODE_START   = 0,
            ERIS_URLENCODE_ALNUM   ,
            ERIS_URLENCODE_SIGN    ,
            ERIS_URLENCODE_PERCENT ,
            ERIS_URLENCODE_HEX1    ,
            ERIS_URLENCODE_HEX2    ,
        } eris_urlencode_state_v = ERIS_URLENCODE_START;

        eris_uchar_t b64_2_c = 0;
        eris_size_t  i       = 0;

        eris_size_t data_size = eris_string_size( __in_es);

        /** Decode urluncode data */
        for ( i = 0; i < data_size; i++) {
            eris_uchar_t c = (eris_uchar_t )eris_string_at( __in_es, i);

            switch ( eris_urlencode_state_v) {
                case ERIS_URLENCODE_START :
                    {
                        if ( '%' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_PERCENT;

                        } else if ( '+' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_SIGN;

                            rc = eris_string_append_c( __out_es, ' ');
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char ' ' failed, errno.<%d>", errno);
                                }
                            }
                        } else {
                            eris_urlencode_state_v = ERIS_URLENCODE_ALNUM;

                            rc = eris_string_append_c( __out_es, c);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '%c' failed, errno.<%d>", c, errno);
                                }
                            }
                        }
                    } break;
                case ERIS_URLENCODE_ALNUM :
                    {
                        if ( '%' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_PERCENT;

                        } else if ( '+' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_SIGN;

                            rc = eris_string_append_c( __out_es, ' ');
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char ' ' failed, errno.<%d>", errno);
                                }
                            }
                        } else {
                            rc = eris_string_append_c( __out_es, c);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '%c' failed, errno.<%d>", c, errno);
                                }
                            }
                        }
                    } break;
                case ERIS_URLENCODE_SIGN :
                    {
                        if ( '%' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_PERCENT;

                        } else if ( '+' == c) {
                            rc = eris_string_append_c( __out_es, ' ');
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char ' ' failed, errno.<%d>", errno);
                                }
                            }
                        } else {
                            eris_urlencode_state_v = ERIS_URLENCODE_ALNUM;

                            rc = eris_string_append_c( __out_es, c);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '%c' failed, errno.<%d>", c, errno);
                                }
                            }
                        }
                    } break;
                case ERIS_URLENCODE_PERCENT :
                    {
                        if ( ERIS_CHAR_IS_HEX(c)) {
                            eris_urlencode_state_v = ERIS_URLENCODE_HEX1;

                            if ( ('0' <= c) && ('9' >= c)) {
                                b64_2_c = (0xF0 & ((c - '0') << 4));

                            } else if ( ('a' <= c) && ('z' >= c)) {
                                b64_2_c = (0xF0 & ((c - 'a' + 10) << 4));

                            } else {
                                b64_2_c = (0xF0 & ((c - 'A' + 10) << 4));
                            }
                        } else {
                            rc = EERIS_ERROR;

                            if ( __log) {
                                eris_log_dump( __log, ERIS_LOG_CORE, "Save a '%c' is not urlencode hex charactor", c);
                            }
                        }
                    } break;
                case ERIS_URLENCODE_HEX1 :
                    {
                        if ( ERIS_CHAR_IS_HEX(c)) {
                            eris_urlencode_state_v = ERIS_URLENCODE_HEX2;

                            if ( ('0' <= c) && ('9' >= c)) {
                                b64_2_c |= (0x0F & (c - '0'));

                            } else if ( ('a' <= c) && ('f' >= c)) {
                                b64_2_c |= (0x0F & (c - 'a' + 10));

                            } else {
                                b64_2_c |= (0x0F & (c - 'A' + 10));
                            }

                            rc = eris_string_append_c( __out_es, b64_2_c);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '%c' failed, errno.<%d>", b64_2_c, errno);
                                }
                            }

                            /** Reset */
                            b64_2_c = 0;
                        } else {
                            rc = EERIS_ERROR;

                            if ( __log) {
                                eris_log_dump( __log, ERIS_LOG_CORE, "Save a '%c' is not urlencode hex charactor", c);
                            }
                        }
                    } break;
                case ERIS_URLENCODE_HEX2 :
                    {
                        if ( '%' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_PERCENT;

                        } else if ( '+' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_SIGN;

                            rc = eris_string_append_c( __out_es, ' ');
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char ' ' failed, errno.<%d>", errno);
                                }
                            }
                        } else {
                            eris_urlencode_state_v = ERIS_URLENCODE_ALNUM;

                            rc = eris_string_append_c( __out_es, c);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '%c' failed, errno.<%d>", c, errno);
                                }
                            }
                        }
                    } break;
                default : break;
            }/// switch ( eris_urlencode_state_v)

            /** Happen error */
            if ( 0 != rc) { break; }

        }/// for ( i = 0; i < __size; i++)
    } else {
        rc = EERIS_ERROR;

        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input string or data is invalid");
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_urlencode_decode_eris


/**
 * @Brief: Decode urlencode data to stream data.
 *
 * @Param: __out_buf,Output decode urlencode buffer..
 * @Param: __in_es,  Input urlencoded data string.
 * @param: __log ,   Dump log context if set.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_urlencode_decode_buffer( eris_buffer_t *__out_buf, const eris_string_t __in_es, eris_log_t *__log)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __out_buf && __in_es) {
        eris_buffer_cleanup( __out_buf, __log);

        /** Urlencode states of decode doing */
        enum {
            ERIS_URLENCODE_START   = 0,
            ERIS_URLENCODE_ALNUM   ,
            ERIS_URLENCODE_SIGN    ,
            ERIS_URLENCODE_PERCENT ,
            ERIS_URLENCODE_HEX1    ,
            ERIS_URLENCODE_HEX2    ,
        } eris_urlencode_state_v = ERIS_URLENCODE_START;

        eris_uchar_t b64_2_c = 0;
        eris_size_t  i       = 0;

        eris_size_t data_size = eris_string_size( __in_es);

        /** Decode urluncode data */
        for ( i = 0; i < data_size; i++) {
            eris_uchar_t c = (eris_uchar_t )eris_string_at( __in_es, i);

            switch ( eris_urlencode_state_v) {
                case ERIS_URLENCODE_START :
                    {
                        if ( '%' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_PERCENT;

                        } else if ( '+' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_SIGN;

                            rc = eris_buffer_append_c( __out_buf, ' ', __log);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char ' ' failed, errno.<%d>", errno);
                                }
                            }
                        } else {
                            eris_urlencode_state_v = ERIS_URLENCODE_ALNUM;

                            rc = eris_buffer_append_c( __out_buf, c, __log);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '%c' failed, errno.<%d>", c, errno);
                                }
                            }
                        }
                    } break;
                case ERIS_URLENCODE_ALNUM :
                    {
                        if ( '%' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_PERCENT;

                        } else if ( '+' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_SIGN;

                            rc = eris_buffer_append_c( __out_buf, ' ', __log);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char ' ' failed, errno.<%d>", errno);
                                }
                            }
                        } else {
                            rc = eris_buffer_append_c( __out_buf, c, __log);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '%c' failed, errno.<%d>", c, errno);
                                }
                            }
                        }
                    } break;
                case ERIS_URLENCODE_SIGN :
                    {
                        if ( '%' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_PERCENT;

                        } else if ( '+' == c) {
                            rc = eris_buffer_append_c( __out_buf, ' ', __log);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char ' ' failed, errno.<%d>", errno);
                                }
                            }
                        } else {
                            eris_urlencode_state_v = ERIS_URLENCODE_ALNUM;

                            rc = eris_buffer_append_c( __out_buf, c, __log);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '%c' failed, errno.<%d>", c, errno);
                                }
                            }
                        }
                    } break;
                case ERIS_URLENCODE_PERCENT :
                    {
                        if ( ERIS_CHAR_IS_HEX(c)) {
                            eris_urlencode_state_v = ERIS_URLENCODE_HEX1;

                            if ( ('0' <= c) && ('9' >= c)) {
                                b64_2_c = (0xF0 & ((c - '0') << 4));

                            } else if ( ('a' <= c) && ('z' >= c)) {
                                b64_2_c = (0xF0 & ((c - 'a' + 10) << 4));

                            } else {
                                b64_2_c = (0xF0 & ((c - 'A' + 10) << 4));
                            }
                        } else {
                            rc = EERIS_ERROR;

                            if ( __log) {
                                eris_log_dump( __log, ERIS_LOG_CORE, "Save a '%c' is not urlencode hex charactor", c);
                            }
                        }
                    } break;
                case ERIS_URLENCODE_HEX1 :
                    {
                        if ( ERIS_CHAR_IS_HEX(c)) {
                            eris_urlencode_state_v = ERIS_URLENCODE_HEX2;

                            if ( ('0' <= c) && ('9' >= c)) {
                                b64_2_c |= (0x0F & (c - '0'));

                            } else if ( ('a' <= c) && ('f' >= c)) {
                                b64_2_c |= (0x0F & (c - 'a' + 10));

                            } else {
                                b64_2_c |= (0x0F & (c - 'A' + 10));
                            }

                            rc = eris_buffer_append_c( __out_buf, b64_2_c, __log);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '%c' failed, errno.<%d>", b64_2_c, errno);
                                }
                            }

                            /** Reset */
                            b64_2_c = 0;
                        } else {
                            rc = EERIS_ERROR;

                            if ( __log) {
                                eris_log_dump( __log, ERIS_LOG_CORE, "Save a '%c' is not urlencode hex charactor", c);
                            }
                        }
                    } break;
                case ERIS_URLENCODE_HEX2 :
                    {
                        if ( '%' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_PERCENT;

                        } else if ( '+' == c) {
                            eris_urlencode_state_v = ERIS_URLENCODE_SIGN;

                            rc = eris_buffer_append_c( __out_buf, ' ', __log);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char ' ' failed, errno.<%d>", errno);
                                }
                            }
                        } else {
                            eris_urlencode_state_v = ERIS_URLENCODE_ALNUM;

                            rc = eris_buffer_append_c( __out_buf, c, __log);
                            if ( 0 != rc) {
                                rc = EERIS_ERROR;

                                if ( __log) {
                                    eris_log_dump( __log, ERIS_LOG_CORE, "Save a char '%c' failed, errno.<%d>", c, errno);
                                }
                            }
                        }
                    } break;
                default : break;
            }/// switch ( eris_urlencode_state_v)

            /** Happen error */
            if ( 0 != rc) { break; }

        }/// for ( i = 0; i < __size; i++)
    } else {
        rc = EERIS_ERROR;

        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input string or data is invalid");
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_urlencode_decode_buffer


