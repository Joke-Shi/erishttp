/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This design to do string of the erishttp reference sds, apis eg:
 **        :    eris_string_alloc
 **        :    eris_string_create
 **        :    eris_string_create_n
 **        :    eris_string_assign
 **        :    eris_string_at
 **        :    eris_string_append
 **        :    eris_string_cleanup
 **        :    eris_string_copy
 **        :    eris_string_copy_n
 **        :    eris_string_complare
 **        :    eris_string_empty
 **        :    eris_string_erase
 **        :    eris_string_clone
 **        :    eris_string_find
 **        :    eris_string_rfind
 **        :    eris_string_replace
 **        :    eris_string_printf
 **        :    eris_string_vprintf
 **        :    eris_string_reverse
 **        :    eris_string_rotation
 **        :    eris_string_strip
 **        :    eris_string_free
 **        :    ...
 **        : and so on
 **
 ******************************************************************************/

#include "core/eris.core.h"


/** The npos of eris string */
const eris_size_t ERIS_STRING_NPOS = -1;


/** All type max size number */
#define ERIS_STRING_8_MSIZE  (0xffUL)
#define ERIS_STRING_16_MSIZE (0xffffUL)
#define ERIS_STRING_32_MSIZE (0xfffffff0UL)
#define ERIS_STRING_64_MSIZE (0xffffffffffffffe0UL)



/**
 * @Brief: Set string to eris string object.
 *
 * @Param: __esp, Eris string objects.
 * @Param: __s  , Value string.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_string_set( eris_string_t *__esp, const eris_char_t *__s)
{
    eris_int_t rc = 0;

    if ( __esp) {
        eris_string_cleanup( *__esp);

        /** Set value */
        if ( '\0' != __s[0]) {
            rc = eris_string_append( __esp, __s);
        }
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_string_set


/**
 * @Brief: Set string to eris string object.
 *
 * @Param: __esp, Eris string objects.
 * @Param: __es,  Value eris string.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_string_set_eris( eris_string_t *__esp, const eris_string_t __es)
{
    eris_int_t rc = 0;

    if ( __esp) {
        eris_string_cleanup( *__esp);

        /** Set value */
        rc = eris_string_append_eris( __esp, __es);
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_string_set_eris


/**
 * @Brief: Set a charactor at eris string index.
 *
 * @Param: __es, Eris string context.
 * @Param: __at, At index, number.
 * @Param: __c , A charactor.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_string_set_at_c( eris_string_t __es, eris_size_t __at, const eris_char_t __c)
{
    eris_int_t rc = 0;

    if ( __es) {
        if ( __at < eris_string_size( __es)) {
            __es[ __at] = __c;

            if ( '\0' == __c) {
                switch( ERIS_STRING_TYPE( __es)) {
                    case ERIS_STRING_TYPE_5 : 
                    case ERIS_STRING_TYPE_8 : 
                        { 
                            ERIS_STRING_VARP( __es,8) {
                                eris_string_varp->size = __at;
                            }
                        }break;
                    case ERIS_STRING_TYPE_16: 
                        { 
                            ERIS_STRING_VARP(__es,16) {
                                eris_string_varp->size = __at;
                            }
                        }break;
                    case ERIS_STRING_TYPE_32: 
                        { 
                            ERIS_STRING_VARP(__es,32) {
                                eris_string_varp->size = __at;
                            }
                        }break;
                    case ERIS_STRING_TYPE_64: 
                        { 
                            ERIS_STRING_VARP(__es,64) {
                                eris_string_varp->size = __at;
                            }
                        }break;
                    default : break;
                }
            }
        }
    }

    return rc;
}/// eris_string_set_at_c


/**
 * @Brief: Alloc empty eris string object.
 *
 * @Param: _n, The alloc of size n bits.
 *
 * @Return: Successed return eris string object, other, is NULL.
 **/
eris_string_t eris_string_alloc( eris_size_t __n)
{
    eris_string_t es_p = NULL;

    __n += 1;

    if ( __n < ERIS_STRING_8_MSIZE) {
        eris_string_8_t *tp = (eris_string_8_t *)eris_memory_alloc( __n + sizeof( eris_string_8_t));
        if ( tp) {
            tp->flags.type = ERIS_STRING_TYPE_8;
            tp->flags.zero = 0;
            tp->size       = 0;
            tp->alloc      = __n;

            es_p = tp->estring;
        }
    } else if ( __n < ERIS_STRING_16_MSIZE) {
        eris_string_16_t *tp = (eris_string_16_t *)eris_memory_alloc( __n + sizeof( eris_string_16_t));
        if ( tp) {
            tp->flags.type = ERIS_STRING_TYPE_16;
            tp->flags.zero = 0;
            tp->size       = 0;
            tp->alloc      = __n;

            es_p = tp->estring;
        }
    } else if ( __n < ERIS_STRING_32_MSIZE) {
        eris_string_32_t *tp = (eris_string_32_t *)eris_memory_alloc( __n + sizeof( eris_string_32_t));
        if ( tp) {
            tp->flags.type = ERIS_STRING_TYPE_32;
            tp->flags.zero = 0;
            tp->size       = 0;
            tp->alloc      = __n;

            es_p = tp->estring;
        }
    } else {
        if ( ERIS_STRING_64_MSIZE < __n) {
            __n = ERIS_STRING_64_MSIZE;
        }

        eris_string_64_t *tp = (eris_string_64_t *)eris_memory_alloc( __n + sizeof( eris_string_64_t));
        if ( tp) {
            tp->flags.type = ERIS_STRING_TYPE_64;
            tp->flags.zero = 0;
            tp->size       = 0;
            tp->alloc      = __n;

            es_p = tp->estring;
        }
    }

    /** Clean up the eris string buffer */
    if ( es_p) { memset( es_p, 0x0, __n); }

    return es_p;
}/// eris_string_alloc


/**
 * @Brief: Realloc eris string object, old->size + __n.
 *
 * @Param: __es, The old source eris string.
 * @Param: __n,  The alloc of size n bits.
 *
 * @Return: Successed return new eris string object, other, is NULL.
 **/
eris_string_t eris_string_realloc( eris_string_t __es, eris_size_t __n)
{
    eris_string_t rc_es = NULL;

    if ( (0 < __n) && __es) {
        eris_size_t es_size  = eris_string_size( __es);
        eris_size_t es_alloc = eris_string_count( __es);

        /** If es_size range out of ERIS_STRING_64_SIZE, happen error; then create new */
        if ( es_alloc != __n) {
            rc_es = eris_string_alloc( __n);
            if ( rc_es) {
                if ( 0 < es_size) {
                    /** Copy data */
                    (eris_void_t )eris_string_copy_eris( rc_es, __es);
                }

                /** Release old eris string */
                (eris_void_t )eris_string_free( __es);
            }
        } else { rc_es = __es; }
    } else { rc_es = __es; }

    return rc_es;
}/// eris_string_realloc


/**
 * @Brief: From eris string to interger.
 *
 * @Param: __es, The eris string.
 *
 * @Return: A result of interger value.
 **/
eris_int_t eris_string_atoi( const eris_string_t __es)
{
    eris_int_t value = 0;
    eris_int_t sign  = 1;

    if ( __es) {
        eris_string_t dup_es = __es;
#if 0
        /** Dup eris string */
        eris_string_t dup_es = eris_string_clone( __es);
        if ( dup_es) {
            eris_string_strip( dup_es);
#endif

            eris_size_t size = eris_string_size( dup_es);
            eris_size_t i    = 0;

            /** Check sign */
            if ( '-' == dup_es[ i]) {
                sign = -1; 

				i++;
            } else {
                if ( '+' == dup_es[ i]) {
                    i++;
                }
            }

            for ( ; i < size; i++) {
                if ( ('0' <= dup_es[ i]) && ('9' >= dup_es[ i])) {
                    value = value * 10 + (dup_es[i] - '0');

                    if ( 0 > value) {
                        /** Max sign int value */
                        value = 2147483647;

                        break;
                    }
                } else { break; }
            }

#if 0
            eris_string_free( dup_es);
            dup_es = NULL;
        }
#endif
    }/// fi ( __es)

    return (value * sign);
}/// eris_string_atoi


/**
 * @Brief: From eris string to unsinged interger.
 *
 * @Param: __es, The eris string.
 *
 * @Return: A result of unsigned interger value.
 **/
eris_uint_t eris_string_atou( const eris_string_t __es)
{
    eris_int_t  sign  = 1;
    eris_uint_t value = 0;

    if ( __es) {
        eris_string_t dup_es = __es;
#if 0
        /** Dup eris string */
        eris_string_t dup_es = eris_string_clone( __es);
        if ( dup_es) {
            eris_string_strip( dup_es);
#endif

            eris_size_t size = eris_string_size( dup_es);
            eris_size_t i    = 0;

            /** Check sign */
            if ( '-' == dup_es[ i]) {
                sign = -1; 
				
				i++;
            } else {
                if ( '+' == dup_es[ i]) {
                    i++;
                }
            }

            for ( ; i < size; i++) {
                if ( ('0' <= dup_es[ i]) && ('9' >= dup_es[ i])) {
                    eris_uint_t old_value = value;

                    value = value * 10 + (dup_es[i] - '0');
                    if ( old_value > value) {
                        value = 4294967295;

                        break;
                    }
                } else { break; }
            }

#if 0
            eris_string_free( dup_es);
            dup_es = NULL;
        }
#endif
    }

    return (value * sign);
}/// eris_string_atou


/**
 * @Brief: From eris string to long value.
 *
 * @Param: __es, The eris string.
 *
 * @Return: A result of long value.
 **/
eris_long_t eris_string_atol( const eris_string_t __es)
{
    eris_long_t value = 0;
    eris_long_t sign  = 1;

    if ( __es) {
        eris_string_t dup_es = __es;
#if 0
        /** Dup eris string */
        eris_string_t dup_es = eris_string_clone( __es);
        if ( dup_es) {
            eris_string_strip( dup_es);
#endif

            eris_size_t size = eris_string_size( dup_es);
            eris_size_t i    = 0;

            /** Check sign */
            if ( '-' == dup_es[ i]) {
                sign = -1; 
				
				i++;
            } else {
                if ( '+' == dup_es[ i]) {
                    i++;
                }
            }

            for ( ; i < size; i++) {
                if ( ('0' <= dup_es[ i]) && ('9' >= dup_es[ i])) {
                    value = value * 10 + (dup_es[i] - '0');

                    if ( 0 > value) {
                        /** Max sign long value */
                        if ( 4 == sizeof( long)) {
                            value = 2147483647L;
                        } else {
                            value = 9223372036854775807L;
                        }

                        break;
                    }
                } else { break; }
            }

#if 0
            eris_string_free( dup_es);
            dup_es = NULL;
        }
#endif
    }

    return (value * sign);
}/// eris_string_atol


/**
 * @Brief: From eris string to unsigned long value.
 *
 * @Param: __es, The eris string.
 *
 * @Return: A result of unsigned long value.
 **/
eris_ulong_t eris_string_atoul( const eris_string_t __es)
{
    eris_long_t  sign  = 1;
    eris_ulong_t value = 0;

    if ( __es) {
        eris_string_t dup_es = __es;
#if 0
        /** Dup eris string */
        eris_string_t dup_es = eris_string_clone( __es);
        if ( dup_es) {
            eris_string_strip( dup_es);
#endif

            eris_size_t size = eris_string_size( dup_es);
            eris_size_t i    = 0;

            /** Check sign */
            if ( '-' == dup_es[ i]) {
                sign = -1; 
				
				i++;
            } else {
                if ( '+' == dup_es[ i]) {
                    i++;
                }
            }

            for ( ; i < size; i++) {
                if ( ('0' <= dup_es[ i]) && ('9' >= dup_es[ i])) {
                    eris_ulong_t old_value = value;

                    value = value * 10 + (dup_es[i] - '0');
                    if ( old_value > value) {
                        if ( 4 == sizeof( eris_ulong_t)) {
                            value = 4294967295UL;
                        } else {
                            value = 18446744073709551615UL;
                        }

                        break;
                    }
                } else { break; }
            }

#if 0
            eris_string_free( dup_es);
            dup_es = NULL;
        }
#endif
    }

    return (value * sign);
}/// eris_string_atoul


/**
 * @Brief: From eris string to long long value.
 *
 * @Param: __es, The eris string.
 *
 * @Return: A result of long long value.
 **/
eris_llong_t eris_string_atoll( const eris_string_t __es)
{
    eris_llong_t value = 0;
    eris_llong_t sign  = 1;

    if ( __es) {
        eris_string_t dup_es = __es;
#if 0
        /** Dup eris string */
        eris_string_t dup_es = eris_string_clone( __es);
        if ( dup_es) {
            eris_string_strip( dup_es);
#endif

            eris_size_t size  = eris_string_size( dup_es);
            eris_size_t i     = 0;

            /** Check sign */
            if ( '-' == dup_es[ i]) {
                sign = -1; 
				
				i++;
            } else {
                if ( '+' == dup_es[ i]) {
                    i++;
                }
            }

            for ( ; i < size; i++) {
                if ( ('0' <= dup_es[ i]) && ('9' >= dup_es[ i])) {
                    value = value * 10 + (dup_es[i] - '0');

                    if ( 0 > value) {
                        /** Max sign long long value */
                        value = 9223372036854775807LL;

                        break;
                    }
                } else { break; }
            }

#if 0
            eris_string_free( dup_es);
            dup_es = NULL;
        }
#endif
    }

    return (value * sign);
}/// eris_string_atoll


/**
 * @Brief: From eris string to unsigned long long value.
 *
 * @Param: __es, The eris string.
 *
 * @Return: A result of unsigned long long value.
 **/
eris_ullong_t eris_string_atoull( const eris_string_t __es)
{
    eris_llong_t  sign  = 1;
    eris_ullong_t value = 0;

    if ( __es) {
        /** Dup eris string */
        eris_string_t dup_es = eris_string_clone_eris( __es);
        eris_string_strip( dup_es);

        eris_size_t size  = eris_string_size( dup_es);
        eris_size_t i     = 0;

        /** Check sign */
        if ( '-' == dup_es[ i]) {
            sign = -1; 
			
			i++;
        } else {
            if ( '+' == dup_es[ i]) {
                i++;
            }
        }

        for ( ; i < size; i++) {
            if ( ('0' <= dup_es[ i]) && ('9' >= dup_es[ i])) {
                eris_ullong_t old_value = value;

                value = value * 10 + (dup_es[i] - '0');
                if ( old_value > value) {
                    value = 18446744073709551615ULL;

                    break;
                }
            } else { break; }
        }
    }

    return (value * sign);
}/// eris_string_atoull


/**
 * @Brief: From int to eris string.
 *
 * @Param: __es, The output eris string.
 * @Param: __v,  The value number.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_itoa( eris_string_t *__esp, eris_int_t __v)
{
    eris_int_t  rc = 0;
    eris_char_t buf[ 32] = {0};

    eris_uint_t v = (0 > __v) ? (-__v) : __v;

    eris_char_t *p = buf;
    do {
        *p++ = '0' + (v % 10);
        v   /= 10;
    } while ( v);

    if ( 0 > __v) { 
        *p++ = '-';
    }

    *p = '\0';

    if ( __esp) {
        if ( !(*__esp)) {
            /** Create new eris string */
            *__esp = eris_string_alloc( 32);
        }

        if ( *__esp) {
            (eris_void_t )eris_string_cleanup( *__esp);

            /** Append n size */
            rc = eris_string_append_n( __esp, buf, (p - buf));
            if ( 0 == rc) {
                (eris_void_t )eris_string_reverse( *__esp);
            }
        }
    }

    return rc;
}/// eris_string_itoa


/**
 * @Brief: From unsigned int to eris string.
 *
 * @Param: __es, The output eris string.
 * @Param: __v,  The value number.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_utoa( eris_string_t *__esp, eris_uint_t __v)
{
    eris_int_t  rc = 0;
    eris_char_t buf[ 32] = {0};

    eris_uint_t v = __v;

    eris_char_t *p = buf;
    do {
        *p++ = '0' + (v % 10);
        v   /= 10;
    } while ( v);

    *p = '\0';

    if ( __esp) {
        if ( !(*__esp)) {
            /** Create new eris string */
            *__esp = eris_string_alloc( 32);
        }

        if ( *__esp) {
            (eris_void_t )eris_string_cleanup( *__esp);

            /** Append n size */
            rc = eris_string_append_n( __esp, buf, (p - buf));
            if ( 0 == rc) {
                (eris_void_t )eris_string_reverse( *__esp);
            }
        }
    }

    return rc;
}/// eris_string_utoa


/**
 * @Brief: From long to eris string.
 *
 * @Param: __es, The output eris string.
 * @Param: __v,  The value number.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_ltoa( eris_string_t *__esp, eris_long_t __v)
{
    eris_int_t  rc = 0;
    eris_char_t buf[ 32] = {0};

    eris_long_t v = (0 > __v) ? (-__v) : __v;

    eris_char_t *p = buf;
    do {
        *p++ = '0' + (v % 10);
        v   /= 10;
    } while ( v);

    if ( 0 > __v) { 
        *p++ = '-';
    }

    *p = '\0';

    if ( __esp) {
        if ( !(*__esp)) {
            /** Create new eris string */
            *__esp = eris_string_alloc( 32);
        }

        if ( *__esp) {
            (eris_void_t )eris_string_cleanup( *__esp);

            /** Append n size */
            rc = eris_string_append_n( __esp, buf, (p - buf));
            if ( 0 == rc) {
                (eris_void_t )eris_string_reverse( *__esp);
            }
        }
    }

    return rc;
}/// eris_string_ltoa


/**
 * @Brief: From unsigned long to eris string.
 *
 * @Param: __es, The output eris string.
 * @Param: __v,  The value number.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_ultoa( eris_string_t *__esp, eris_ulong_t __v)
{
    eris_int_t  rc = 0;
    eris_char_t buf[ 32] = {0};

    eris_ulong_t v = __v;

    eris_char_t *p = buf;
    do {
        *p++ = '0' + (v % 10);
        v   /= 10;
    } while ( v);

    *p = '\0';

    if ( __esp) {
        if ( !(*__esp)) {
            /** Create new eris string */
            *__esp = eris_string_alloc( 32);
        }

        if ( *__esp) {
            (eris_void_t )eris_string_cleanup( *__esp);

            /** Append n size */
            rc = eris_string_append_n( __esp, buf, (p - buf));
            if ( 0 == rc) {
                (eris_void_t )eris_string_reverse( *__esp);
            }
        }
    }

    return rc;
}/// eris_string_ultoa


/**
 * @Brief: From long long to eris string.
 *
 * @Param: __es, The output eris string.
 * @Param: __v,  The value number.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_lltoa( eris_string_t *__esp, eris_llong_t __v)
{
    eris_int_t  rc = 0;
    eris_char_t buf[ 32] = {0};

    eris_llong_t v = (0 > __v) ? (-__v) : __v;

    eris_char_t *p = buf;
    do {
        *p++ = '0' + (v % 10);
        v   /= 10;
    } while ( v);

    if ( 0 > __v) { 
        *p++ = '-';
    }

    *p = '\0';

    if ( __esp) {
        if ( !(*__esp)) {
            /** Create new eris string */
            *__esp = eris_string_alloc( 32);
        }

        if ( *__esp) {
            (eris_void_t )eris_string_cleanup( *__esp);

            /** Append n size */
            rc = eris_string_append_n( __esp, buf, (p - buf));
            if ( 0 == rc) {
                (eris_void_t )eris_string_reverse( *__esp);
            }
        }
    }

    return rc;
}/// eris_string_lltoa


/**
 * @Brief: From unsigned long long to eris string.
 *
 * @Param: __es, The output eris string.
 * @Param: __v,  The value number.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_ulltoa( eris_string_t *__esp, eris_ullong_t __v)
{
    eris_int_t  rc = 0;
    eris_char_t buf[ 32] = {0};

    eris_ullong_t v = __v;

    eris_char_t *p = buf;
    do {
        *p++ = '0' + (v % 10);
        v   /= 10;
    } while ( v);

    *p = '\0';

    if ( __esp) {
        if ( !(*__esp)) {
            /** Create new eris string */
            *__esp = eris_string_alloc( 32);
        }

        if ( *__esp) {
            (eris_void_t )eris_string_cleanup( *__esp);

            /** Append n size */
            rc = eris_string_append_n( __esp, buf, (p - buf));
            if ( 0 == rc) {
                (eris_void_t )eris_string_reverse( *__esp);
            }
        }
    }

    return rc;
}/// eris_string_ulltoa


/**
 * @Brief: Hex string to interger.
 *
 * @Param: __es, The output eris string.
 *
 * @Return: Convert result interger.
 **/
eris_size_t eris_string_hextov( const eris_string_t __es)
{
    eris_size_t rc_v = 0;

    if ( __es) {
        eris_bool_t is_end   = false;
        eris_size_t base     = 1;
        eris_size_t i        = 0;
        eris_size_t size     = eris_string_size( __es);

        for ( i = 0; i < size; i++) {
            switch ( __es[ i]) {
                case '0' :
                    {
                        if ( (0 != i) && (0 != rc_v)) {
                            rc_v = (rc_v * base) + 0;

                            base *= 16;
                        }
                    } break;
                case '1' :
                    {
                        rc_v  = (rc_v * base) + 1;
                        base *= 16;
                    } break;
                case '2' :
                    {
                        rc_v  = (rc_v * base) + 2;
                        base *= 16;
                    } break;
                case '3' :
                    {
                        rc_v  = (rc_v * base) + 3;
                        base *= 16;
                    } break;
                case '4' :
                    {
                        rc_v  = (rc_v * base) + 4;
                        base *= 16;
                    } break;
                case '5' :
                    {
                        rc_v  = (rc_v * base) + 5;
                        base *= 16;
                    } break;
                case '6' :
                    {
                        rc_v  = (rc_v * base) + 6;
                        base *= 16;
                    } break;
                case '7' :
                    {
                        rc_v  = (rc_v * base) + 7;
                        base *= 16;
                    } break;
                case '8' :
                    {
                        rc_v  = (rc_v * base) + 8;
                        base *= 16;
                    } break;
                case '9' :
                    {
                        rc_v  = (rc_v * base) + 9;
                        base *= 16;
                    } break;
                case 'a' :
                case 'A' :
                    {
                        rc_v  = (rc_v * base) + 10;
                        base *= 16;
                    } break;
                case 'b' :
                case 'B' :
                    {
                        rc_v  = (rc_v * base) + 11;
                        base *= 16;
                    } break;
                case 'c' :
                case 'C' :
                    {
                        rc_v  = (rc_v * base) + 12;
                        base *= 16;
                    } break;
                case 'd' :
                case 'D' :
                    {
                        rc_v  = (rc_v * base) + 13;
                        base *= 16;
                    } break;
                case 'e' :
                case 'E' :
                    {
                        rc_v  = (rc_v * base) + 14;
                        base *= 16;
                    } break;
                case 'f' :
                case 'F' :
                    {
                        rc_v  = (rc_v * base) + 15;
                        base *= 16;
                    } break;
                case 'x' :
                case 'X' :
                    {
                        /** Hex format is invalid */
                        if ( 1 != i) {
                            is_end = true;
                        }
                    } break;
                default : 
                    {
                        /** Is not hex char and end */
                        is_end = true;
                    } break;
            }

            /** Convert end */
            if ( is_end) { break; }
        }
    }

    return rc_v;
}/// eris_string_hextov


/**
 * @Brief: From type size value to eris string.
 *
 * @Param: __esp, The output eris string.
 * @Param: __v,   The value number.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_vtohex( eris_string_t *__esp, eris_size_t __v)
{
    eris_int_t  rc = 0;
    eris_char_t buf[ 32] = {0};

    eris_size_t v = __v;

    eris_char_t *p = buf;
    do {
        eris_size_t m = v % 16;

        if ( 9 >= m) {
            *p++ = '0' + m;

        } else {
            *p++ = 'A' + m;

        }

        v /= 16;
    } while ( v);

    *p = '\0';

    if ( __esp) {
        if ( !(*__esp)) {
            /** Create new eris string */
            *__esp = eris_string_alloc( 32);
        }

        if ( *__esp) {
            (eris_void_t )eris_string_cleanup( *__esp);

            /** Append n size */
            rc = eris_string_append_n( __esp, buf, (p - buf));
            if ( 0 == rc) {
                (eris_void_t )eris_string_reverse( *__esp);
            }
        }
    }

    return rc;
}/// eris_string_vtohex


/**
 * @Brief: Create the eris string.
 *
 * @Param: __s, The source string.
 *
 * @Return: Successed return eris string object, other, is NULL.
 **/
eris_string_t eris_string_create( const eris_char_t *__s)
{
    eris_string_t rc_es = NULL;

    eris_size_t s_size = 0;

    if ( __s) {
        s_size = strlen( __s);
    }

    rc_es = eris_string_alloc( s_size);
    if ( rc_es) {
        /** Copy string */
        (eris_void_t )eris_string_copy_n( rc_es, __s, s_size);
    }

    return rc_es;
}/// eris_string_create


/**
 * @Brief: Create the eris string of size.
 *
 * @Param: __s,  The source string.
 * @Param: __n, Make the size of the eris string object.
 *
 * @Return: Successed return eris string object. other is NULL.
 **/
eris_string_t eris_string_create_n( const eris_char_t *__s, eris_size_t __n)
{
    eris_string_t rc_es = NULL;

    eris_size_t copy_n = __n;
    eris_size_t s_size = 0;

    if ( __s) {
        s_size = strlen( __s);
    }

    if ( __n > s_size) {
        copy_n = s_size;
    }

    rc_es = eris_string_alloc( __n);
    if ( rc_es) {
        /** Copy string */
        (eris_void_t )eris_string_copy_n( rc_es, __s, copy_n);
    }

    return rc_es;
}/// eris_string_create_n


/**
 * @Brief: Assign eris string and output sub eris string.
 *
 * @Param: __es, The eris string object.
 * @Param: __b, The begin index.
 * @Param: __e, The end index.
 *
 * @Return: Successed, return assign sub eris string. other is NULL.
 **/
eris_string_t eris_string_assign( const eris_string_t __es, eris_size_t __b, eris_size_t __e)
{
    eris_string_t rc_es = NULL;

    if ( __es) {
        eris_size_t es_size = eris_string_size( __es);
        if ( es_size < __e) {
            __e = es_size;
        }

        if (( __b < es_size) && ( __b <= __e)) {
            eris_size_t assign_n = 1;
            if ( __b < __e) {
                assign_n = __e - __b;
            }

            /** Create new eris string */
            rc_es = eris_string_alloc( assign_n);
            if ( rc_es) {
                /** Copy */
                (eris_void_t )eris_string_copy_n( rc_es, ((eris_char_t *)__es + __b), assign_n);
            }
        } else {
            rc_es = eris_string_alloc( 8);
            if ( rc_es) {
                eris_string_cleanup( rc_es);
            }
        }
    }

    return rc_es;
}/// eris_string_assign


/**
 * @Brief: Get a char of eris string.
 *
 * @Param: __es, The eris string object.
 * @Param: __at, Specify at of index.
 *
 * @Return: If not range out of eris string, return a char; other return '\0'.
 **/
eris_char_t eris_string_at( const eris_string_t __es, eris_size_t __at)
{
    eris_char_t rc_c = '\0';

    if ( __es) {
        eris_size_t es_size = eris_string_size( __es);
    
        if ( __at < es_size) { rc_c = __es[ __at]; }
    }

    return rc_c;
}/// eris_string_at


/**
 * @Brief: Get the eris string capacity.
 *
 * @Param: __es, The eris string.
 *
 * @Return: The eris string capacity.
 **/
eris_size_t eris_string_capacity( const eris_string_t __es)
{
    /** alloc - size */
    if ( __es) {
        return (eris_string_count( __es) - eris_string_size( __es));
    }

    return 0;
}/// eris_string_capacity


/**
 * @Brief: The source eris string append tmp eris string.
 *
 * @Param: __es, The source eris string.
 * @Param: __s,  The tmp string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_append( eris_string_t *__esp, const eris_char_t *__s)
{
    eris_int_t rc = 0;

    if ( __s) {
        eris_size_t s_size = strlen( __s);

        if ( __esp ) {
            if ( !*__esp) {
                *__esp = eris_string_alloc( s_size);
            } else {
                eris_size_t es_capacity = eris_string_capacity( *__esp);

                if ( s_size > es_capacity) {
                    eris_size_t alloc_count = eris_string_size( *__esp) + s_size + 1;

                    /** Realloc */
                    eris_string_t tmp_es = eris_string_realloc( *__esp, alloc_count);
                    if ( tmp_es) {
                        *__esp = tmp_es;
                    }
                }
            }

            if ( *__esp) {
                /** append */
                eris_size_t es_size = eris_string_size( *__esp);

                memcpy( (eris_char_t *)(*__esp) + es_size, __s, s_size);
                (*__esp)[ es_size + s_size] = '\0';

                switch( ERIS_STRING_TYPE( *__esp)) {
                    case ERIS_STRING_TYPE_5 : 
                    case ERIS_STRING_TYPE_8 : 
                        { 
                            ERIS_STRING_VARP( *__esp,8) {
                                eris_string_varp->size += s_size;
                            }
                        }break;
                    case ERIS_STRING_TYPE_16: 
                        { 
                            ERIS_STRING_VARP(*__esp,16) {
                                eris_string_varp->size += s_size;
                            }
                        }break;
                    case ERIS_STRING_TYPE_32: 
                        { 
                            ERIS_STRING_VARP(*__esp,32) {
                                eris_string_varp->size += s_size;
                            }
                        }break;
                    case ERIS_STRING_TYPE_64: 
                        { 
                            ERIS_STRING_VARP(*__esp,64) {
                                eris_string_varp->size += s_size;
                            }
                        }break;
                    default : break;
                }
            } else { rc = EERIS_ERROR; }
        } else { rc = EERIS_ERROR; }

    }/// fi ( __s)

    return rc;
}/// eris_string_append


/**
 * @Brief: The source eris string append a char.
 *
 * @Param: __esp, The source eris string.
 * @Param: __c,   A tmp char.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_append_c( eris_string_t *__esp, eris_char_t __c)
{
    eris_int_t  rc = 0;
    eris_size_t s_size = 1;

    if ( __esp ) {
        if ( !*__esp) {
            *__esp = eris_string_alloc( s_size);

        } else {
            eris_size_t es_capacity = eris_string_capacity( *__esp);

            if ( s_size > es_capacity) {
                eris_size_t alloc_count = eris_string_size( *__esp) + s_size + 1;

                /** Realloc */
                eris_string_t tmp_es = eris_string_realloc( *__esp, alloc_count);
                if ( tmp_es) {
                    *__esp = tmp_es;
                }
            }
        }

        if ( *__esp) {
            /** append */
            eris_size_t es_size = eris_string_size( *__esp);

            (*__esp)[ es_size] = __c;
            (*__esp)[ es_size + s_size] = '\0';

            if ( '\0' != __c) {
                switch( ERIS_STRING_TYPE( *__esp)) {
                    case ERIS_STRING_TYPE_5 : 
                    case ERIS_STRING_TYPE_8 : 
                        { 
                            ERIS_STRING_VARP( *__esp,8) {
                                eris_string_varp->size += s_size;
                            }
                        }break;
                    case ERIS_STRING_TYPE_16: 
                        { 
                            ERIS_STRING_VARP(*__esp,16) {
                                eris_string_varp->size += s_size;
                            }
                        }break;
                    case ERIS_STRING_TYPE_32: 
                        { 
                            ERIS_STRING_VARP(*__esp,32) {
                                eris_string_varp->size += s_size;
                            }
                        }break;
                    case ERIS_STRING_TYPE_64: 
                        { 
                            ERIS_STRING_VARP(*__esp,64) {
                                eris_string_varp->size += s_size;
                            }
                        }break;
                    default : break;
                }
            }
        } else { rc = EERIS_ERROR; }
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_string_append_c


/**
 * @Brief: The source eris string append tmp eris string.
 *
 * @Param: __esp, The source eris string.
 * @Param: __s  , The tmp string.
 * @Param: __n  , The size with append tmp eris string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_append_n( eris_string_t *__esp, const eris_char_t *__s, eris_size_t __n)
{
    eris_int_t rc = 0;

    if ( __s) {
        eris_size_t s_size = strlen( __s);
        s_size = (__n > s_size) ? s_size : __n;

        if ( __esp ) {
            if ( !*__esp) {
                *__esp = eris_string_alloc( s_size);
            } else {
                eris_size_t es_capacity = eris_string_capacity( *__esp);

                if ( s_size > es_capacity) {
                    eris_size_t alloc_count = eris_string_size( *__esp) + s_size + 1;

                    /** Realloc */
                    eris_string_t tmp_es = eris_string_realloc( *__esp, alloc_count);
                    if ( tmp_es) {
                        *__esp = tmp_es;
                    }
                }
            }

            if ( *__esp) {
                /** append */
                eris_size_t es_size = eris_string_size( *__esp);

                memcpy( (eris_char_t *)(*__esp) + es_size, __s, s_size);
                (*__esp)[ es_size + s_size] = '\0';

                switch( ERIS_STRING_TYPE( *__esp)) {
                    case ERIS_STRING_TYPE_5 : 
                    case ERIS_STRING_TYPE_8 : 
                        { 
                            ERIS_STRING_VARP( *__esp,8) {
                                eris_string_varp->size += s_size;
                            }
                        }break;
                    case ERIS_STRING_TYPE_16: 
                        { 
                            ERIS_STRING_VARP(*__esp,16) {
                                eris_string_varp->size += s_size;
                            }
                        }break;
                    case ERIS_STRING_TYPE_32: 
                        { 
                            ERIS_STRING_VARP(*__esp,32) {
                                eris_string_varp->size += s_size;
                            }
                        }break;
                    case ERIS_STRING_TYPE_64: 
                        { 
                            ERIS_STRING_VARP(*__esp,64) {
                                eris_string_varp->size += s_size;
                            }
                        }break;
                    default : break;
                }
            } else { rc = EERIS_ERROR; }
        } else { rc = EERIS_ERROR; }

    }/// fi ( __s)

    return rc;
}/// eris_string_append_n


/**
 * @Brief: The source eris string append tmp eris string.
 *
 * @Param: __esp, The source eris string.
 * @Param: __et,  The tmp eris string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_append_eris( eris_string_t *__esp, const eris_string_t __et)
{
    eris_int_t rc = 0;

    if ( __et) {
        eris_size_t et_size = eris_string_size( __et);

        if ( __esp ) {
            if ( !*__esp) {
                *__esp = eris_string_alloc( et_size);
            } else {
                eris_size_t es_capacity = eris_string_capacity( *__esp);

                if ( et_size > es_capacity) {
                    eris_size_t alloc_count = eris_string_size( *__esp) + et_size + 1;

                    /** Realloc */
                    eris_string_t tmp_es = eris_string_realloc( *__esp, alloc_count);
                    if ( tmp_es) {
                        *__esp = tmp_es;
                    }
                }
            }

            if ( *__esp) {
                /** append */
                eris_size_t eet_size = eris_string_size( *__esp);

                memcpy( (eris_char_t *)(*__esp) + eet_size, __et, et_size);
                (*__esp)[ eet_size + et_size] = '\0';

                switch( ERIS_STRING_TYPE( *__esp)) {
                    case ERIS_STRING_TYPE_5 : 
                    case ERIS_STRING_TYPE_8 : 
                        { 
                            ERIS_STRING_VARP( *__esp,8) {
                                eris_string_varp->size += et_size;
                            }
                        }break;
                    case ERIS_STRING_TYPE_16: 
                        { 
                            ERIS_STRING_VARP(*__esp,16) {
                                eris_string_varp->size += et_size;
                            }
                        }break;
                    case ERIS_STRING_TYPE_32: 
                        { 
                            ERIS_STRING_VARP(*__esp,32) {
                                eris_string_varp->size += et_size;
                            }
                        }break;
                    case ERIS_STRING_TYPE_64: 
                        { 
                            ERIS_STRING_VARP(*__esp,64) {
                                eris_string_varp->size += et_size;
                            }
                        }break;
                    default : break;
                }
            } else { rc = EERIS_ERROR; }
        } else { rc = EERIS_ERROR; }

    }/// fi ( __s)

    return rc;
}/// eris_string_append_eris


/**
 * @Brief: Clean up the eris string.
 *
 * @Param: __es, The eris string source.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_string_cleanup( eris_string_t __es)
{
    if ( __es) {
        switch( ERIS_STRING_TYPE(__es)) {
            case ERIS_STRING_TYPE_5 : 
            case ERIS_STRING_TYPE_8 : 
                { 
                    ERIS_STRING_VARP(__es,8) {
                        eris_string_varp->size = 0;
                    }
                }break;
            case ERIS_STRING_TYPE_16: 
                { 
                    ERIS_STRING_VARP(__es,16) {
                        eris_string_varp->size = 0;
                    }
                }break;
            case ERIS_STRING_TYPE_32: 
                { 
                    ERIS_STRING_VARP(__es,32) {
                        eris_string_varp->size = 0;
                    }
                }break;
            case ERIS_STRING_TYPE_64: 
                { 
                    ERIS_STRING_VARP(__es,64) {
                        eris_string_varp->size = 0;
                    }
                }break;
            default : break;
        }

        eris_size_t alloc_count = eris_string_count( __es);
        if ( 0 < alloc_count ) { 
            memset( __es, 0x0, alloc_count); 
        }
    }
}/// eris_string_cleanup


/**
 * @Brief: Clear as the eris string.
 *
 * @Param: __es, The eris string source.
 * @Param: __b,  The begin index.
 * @Param: __e,  The end index.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_string_cleanup_as( eris_string_t __es, eris_size_t __b, eris_size_t __e)
{
    if ( __es && (__b <= __e)) {
        eris_size_t clear_size = __e - __b;
        eris_size_t es_size    = eris_string_size( __es);
        
        if ( __b < es_size) {
            clear_size = (clear_size > es_size) ? (es_size - __b) : clear_size;

            eris_size_t alloc_count = eris_string_count( __es);
            if ( clear_size >= es_size) {
                if ( 0 < alloc_count ) { 
                    memset( __es, 0x0, alloc_count); 
                }
            } else {
                if ( __e > es_size) { __e = es_size; }

                while ( __es[__e]) {
                    __es[__b] = __es[ __e];

                    __b++; __e++;
                }

                while ( __b < alloc_count) {
                    __es[__b] = '\0'; __b++;
                }
            }

            switch( ERIS_STRING_TYPE(__es)) {
                case ERIS_STRING_TYPE_5 : 
                case ERIS_STRING_TYPE_8 : 
                    { 
                        ERIS_STRING_VARP(__es,8) {
                            eris_string_varp->size -= clear_size;
                        }
                    }break;
                case ERIS_STRING_TYPE_16: 
                    { 
                        ERIS_STRING_VARP(__es,16) {
                            eris_string_varp->size -= clear_size;
                        }
                    }break;
                case ERIS_STRING_TYPE_32: 
                    { 
                        ERIS_STRING_VARP(__es,32) {
                            eris_string_varp->size -= clear_size;
                        }
                    }break;
                case ERIS_STRING_TYPE_64: 
                    { 
                        ERIS_STRING_VARP(__es,64) {
                            eris_string_varp->size -= clear_size;
                        }
                    }break;
                default : break;
            }
        }
    }
}/// eris_string_cleanup_as


/**
 * @Brief: Copy from tmp eris string to source.
 *
 * @Param: __es, The source eris string.
 * @Param: __et, The tmp string.
 *
 * @Return: The result of copy finish source eris string.
 **/
eris_string_t eris_string_copy( eris_string_t __es, const eris_char_t *__s)
{
    if ( __es && __s) {
        /** Copy */
        eris_size_t s_size = strlen( __s);

        return eris_string_copy_n( __es, __s, s_size);
    }

     return __es;
}/// eris_string_copy


/**
 * @Brief: Copy from tmp eris string to source with size.
 *
 * @Param: __es, The source eris string.
 * @Param: __et, The tmp eris string.
 * @Param: __n , Copy size of the tmp eris string.
 *
 * @Return: The result of copy finish source eris string.
 **/
eris_string_t eris_string_copy_n( eris_string_t __es, const eris_char_t *__s, eris_size_t __n)
{
    if ( __es && __s && (0 < __n)) {
        eris_size_t es_size     = eris_string_size( __es);
        eris_size_t es_capacity = eris_string_capacity( __es);

        __n = (__n > es_capacity) ? es_capacity : __n;

        memcpy( (eris_char_t *)__es + es_size, __s, __n);

        switch( ERIS_STRING_TYPE(__es)) {
            case ERIS_STRING_TYPE_5 : 
            case ERIS_STRING_TYPE_8 : 
                { 
                    ERIS_STRING_VARP(__es,8) {
                        eris_string_varp->size += __n;
                        eris_string_varp->estring[ eris_string_varp->size] = '\0';
                    }
                }break;
            case ERIS_STRING_TYPE_16: 
                { 
                    ERIS_STRING_VARP(__es,16) {
                        eris_string_varp->size += __n;
                        eris_string_varp->estring[ eris_string_varp->size] = '\0';
                    }
                }break;
            case ERIS_STRING_TYPE_32: 
                { 
                    ERIS_STRING_VARP(__es,32) {
                        eris_string_varp->size += __n;
                        eris_string_varp->estring[ eris_string_varp->size] = '\0';
                    }
                }break;
            case ERIS_STRING_TYPE_64: 
                { 
                    ERIS_STRING_VARP(__es,64) {
                        eris_string_varp->size += __n;
                        eris_string_varp->estring[ eris_string_varp->size] = '\0';
                    }
                }break;
            default : break;
        }
    }

    return __es;
}/// eris_string_copy_n


/**
 * @Brief: Copy from tmp eris string to source.
 *
 * @Param: __es, The source eris string.
 * @Param: __et, The tmp eris string.
 *
 * @Return: The result of copy finish source eris string.
 **/
eris_string_t eris_string_copy_eris( eris_string_t __es, const eris_string_t __et)
{
    if ( __es && __et) {
        /** Copy et_size */
        return eris_string_copy_n( __es, (eris_char_t *)__et, eris_string_size( __et));
    }

    return __es;
}/// eris_string_copy_eris


/**
 * @Brief: Copy n size from tmp eris string to source.
 *
 * @Param: __es, The source eris string.
 * @Param: __et, The tmp eris string.
 * @Param: __n , Copy size of the tmp eris string.
 *
 * @Return: The result of copy finish source eris string.
 **/
eris_string_t eris_string_copy_n_eris( eris_string_t __es, const eris_string_t __et, eris_size_t __n)
{
    if ( __es && __et) {
        eris_size_t es_size = eris_string_size( __et);
        __n = (es_size > __n) ? __n : es_size;

        /** Copy et_size */
        return eris_string_copy_n( __es, (eris_char_t *)__et, __n);
    }

    return __es;
}/// eris_string_copy_n_eris


/**
 * @Brief: Compare eris string 1 vs othe string.
 *
 * @Param: __es1,  The eris string one.
 * @Param: __s,    The other string.
 * @Param: __case, If true ignore case, and vice versa.
 *
 * @Return: If is equal 0, other lt is EERIS_ERROR, gt is 1.
 **/
eris_int_t eris_string_compare( const eris_string_t __es, const eris_char_t *__s, eris_bool_t __case)
{
    eris_int_t rc_compare = 0;

    if ( __es && __s) {
        eris_size_t i = 0;
        eris_size_t es_size = eris_string_size( __es);

        while ( i <= es_size) {
            if ( __s[ i] && __es[ i]) {
                eris_char_t es_c = __es[ i];
                eris_char_t s_c  = __s[ i];

                if ( __case) {
                    es_c = tolower( es_c);
                    s_c  = tolower( s_c);
                }

                if ( es_c != s_c) {
                    if ( es_c < s_c) {
                        rc_compare = EERIS_ERROR;
                    } else {
                        rc_compare = 1;
                    }

                    break;
                }
            } else { 
                if ( ('\0' == __s[i]) && ('\0' == __es[ i])) {
                    rc_compare = 0;
                } else {
                    if ( '\0' == __es[i]) {
                        rc_compare = EERIS_ERROR;
                    } else {
                        rc_compare = 1;
                    }
                }

                break;
            }

            /** compare a next charactor */
            i++;
        }
    } else {
        if ( !__es && !__s) {
            rc_compare = 0;
        } else {
            if ( !__es) {
                rc_compare = EERIS_ERROR;
            } else {
                rc_compare = 1;
            }
        }
    }

    return rc_compare;
}/// eris_string_compare


/**
 * @Brief: Compare eris string 1 vs other string with size.
 *
 * @Param: __es1,  The eris string one.
 * @Param: __s ,   The other string.
 * @Param: __n ,   Compare size of eris string vs other string.
 * @Param: __case, If true ignore case, and vice versa.
 *
 * @Return: If is equal 0, other lt is EERIS_ERROR, gt is 1.
 **/
eris_int_t eris_string_compare_n( const eris_string_t __es, const eris_char_t *__s, eris_size_t __n, eris_bool_t __case)
{
    eris_int_t rc_compare = 0;

    /** If 0 == n, is equal */
    if ( 0 < __n) {
        if ( __es && __s) {
            eris_size_t i = 0;

            while ( i <= __n) {
                if ( __s[ i] && __es[ i]) {
                    eris_char_t es_c = __es[ i];
                    eris_char_t s_c  = __s[ i];

                    if ( __case) {
                        es_c = tolower( es_c);
                        s_c  = tolower( s_c);
                    }

                    if ( es_c != s_c) {
                        if ( es_c < s_c) {
                            rc_compare = EERIS_ERROR;
                        } else {
                            rc_compare = 1;
                        }

                        break;
                    }
                } else { 
                    if ( ('\0' == __s[i]) && ('\0' == __es[ i])) {
                        rc_compare = 0;
                    } else {
                        if ( '\0' == __es[i]) {
                            rc_compare = EERIS_ERROR;
                        } else {
                            rc_compare = 1;
                        }
                    }

                    break;
                }

                /** compare a next charactor */
                i++;
            }
        } else {
            if ( !__es && !__s) {
                rc_compare = 0;
            } else {
                if ( !__es) {
                    rc_compare = EERIS_ERROR;
                } else {
                    rc_compare = 1;
                }
            }
        }
    }/// fi ( 0 < __n)

    return rc_compare;
}/// eris_string_compare_n


/**
 * @Brief: Compare eris string 1 vs eris string 2.
 *
 * @Param: __es1,  The eris string one.
 * @Param: __es2,  The eris string two.
 * @Param: __case, If true ignore case, and vice versa.
 *
 * @Return: If is equal 0, other lt is EERIS_ERROR, gt is 1.
 **/
eris_int_t eris_string_compare_eris( const eris_string_t __es1, const eris_string_t __es2, eris_bool_t __case)
{
    eris_int_t rc_compare = 0;

    if ( __es1 && __es2) {
        eris_size_t es1_size = eris_string_size( __es1);
        eris_size_t es2_size = eris_string_size( __es2);

        if ( es1_size == es2_size) {
            eris_size_t i = 0;

            while ( i <= es1_size) {
                if ( __es1[ i] && __es2[ i]) {
                    eris_char_t es1_c = __es1[ i];
                    eris_char_t es2_c = __es2[ i];

                    /** ignore case */
                    if ( __case) {
                        es1_c = tolower( es1_c);
                        es2_c = tolower( es2_c);
                    }

                    if ( es1_c != es2_c) {
                        if ( es1_c < es2_c) {
                            rc_compare = EERIS_ERROR;
                        } else {
                            rc_compare = 1;
                        }

                        break;
                    }
                } else { 
                    if ( ('\0' == __es1[i]) && ('\0' == __es2[ i])) {
                        rc_compare = 0;
                    } else {
                        if ( '\0' == __es1[i]) {
                            rc_compare = EERIS_ERROR;
                        } else {
                            rc_compare = 1;
                        }
                    }

                    break;
                }

                /** compare a next charactor */
                i++;
            }
        } else { rc_compare = (es1_size < es2_size) ? EERIS_ERROR : 1; }
    } else {
        if ( !__es1 && !__es2) {
            rc_compare = 0;
        } else {
            if ( !__es1) {
                rc_compare = EERIS_ERROR;
            } else {
                rc_compare = 1;
            }
        }
    }

    return rc_compare;
}/// eris_string_compare_eris


/**
 * @Brief: Clone the source eris string.
 *
 * @Param: __s, The source string.
 *
 * @Return: The result eris string by clone.
 **/
eris_string_t eris_string_clone( const eris_char_t *__s) 
{
    eris_string_t rc_es = NULL;

    if ( __s) {
        eris_size_t s_size = strlen( __s);

        /** Create new eris string */
        rc_es = eris_string_alloc( s_size + 1);
        if ( rc_es) {
            if ( 0 < s_size) {
                (eris_void_t )eris_string_copy_n( rc_es, __s, s_size);

            } else {
                rc_es[0] = '\0';
            }
        }
    }

    return rc_es;
}/// eris_string_clone


/**
 * @Brief: Clone the source eris string.
 *
 * @Param: __s, The source string.
 * @Param: __n , Clone size of the source eris string.
 *
 * @Return: The result eris string by clone.
 **/
eris_string_t eris_string_clone_n( const eris_char_t *__s, eris_size_t __n) 
{
    eris_string_t rc_es = NULL;

    if ( __s) {
        eris_size_t s_size = strlen( __s);
        s_size = (s_size > __n) ? __n : s_size;

        /** Create new eris string */
        rc_es = eris_string_alloc( s_size + 1);
        if ( rc_es) {
            if ( 0 < s_size) {
                (eris_void_t )eris_string_copy_n( rc_es, __s, s_size);

            } else {
                rc_es[0] = '\0';
            }
        }
    }

    return rc_es;
}/// eris_string_clone_n


/**
 * @Brief: Clone the source eris string.
 *
 * @Param: __s, The source eris string.
 *
 * @Return: The result eris string by clone.
 **/
eris_string_t eris_string_clone_eris( const eris_string_t __es) 
{
    eris_string_t rc_es = NULL;

    if ( __es) {
        eris_size_t es_size = eris_string_size( __es);

        /** Create new eris string */
        rc_es = eris_string_alloc( es_size + 1);
        if ( rc_es) {
            if ( 0 < es_size) {
                (eris_void_t )eris_string_copy_eris( rc_es, __es);

            } else {
                rc_es[0] = '\0';
            }
        }
    }

    return rc_es;
}/// eris_string_clone_eris


/**
 * @Brief: Get the eris string alloc count.
 *
 * @Param: __es, The eris string.
 *
 * @Return: The eris string alloc count.
 **/
eris_size_t eris_string_count( const eris_string_t __es)
{
    eris_size_t rc_count = 0;
    
    if ( __es) {
        switch( ERIS_STRING_TYPE(__es)) {
            case ERIS_STRING_TYPE_5 : 
            case ERIS_STRING_TYPE_8 : 
                { 
                    ERIS_STRING_VARP(__es,8) {
                        rc_count = eris_string_varp->alloc - 1;
                    }
                }break;
            case ERIS_STRING_TYPE_16: 
                { 
                    ERIS_STRING_VARP(__es,16) {
                        rc_count = eris_string_varp->alloc - 1;
                    }
                }break;
            case ERIS_STRING_TYPE_32: 
                { 
                    ERIS_STRING_VARP(__es,32) {
                        rc_count = eris_string_varp->alloc - 1;
                    }
                }break;
            case ERIS_STRING_TYPE_64: 
                { 
                    ERIS_STRING_VARP(__es,64) {
                        rc_count = eris_string_varp->alloc - 1;
                    }
                }break;
            default : break;
        }
    }/// fi ( __es)

    return rc_count;
}/// eris_string_count


/**
 * @Brief: Check the eris string has empty?
 * 
 * @Param: __es, The eris string.
 *
 * @Return: If empty is true, and vice versa is false.
 **/
eris_bool_t eris_string_empty( const eris_string_t __es)
{
    return (0 == eris_string_size( __es)) ? true : false;
}/// eris_string_empty


/**
 * @Brief: Erase substring from eris string.
 *
 * @Param: __es, The eris string.
 * @Param: __substr, The dest of string.
 *
 * @Param: Nothing.
 **/
eris_void_t eris_string_erase( eris_string_t __es, const eris_char_t *__substr)
{
    if ( __es && __substr) {
        eris_size_t at  = 0;
        eris_size_t pos = 0;
        eris_size_t substr_size = strlen( __substr);

        do {
            pos = eris_string_find_at( __es, at, __substr);
            if ( ERIS_STRING_NPOS != pos) {
                at += pos;

                /** Clear */
                (eris_void_t )eris_string_cleanup_as( __es, pos, pos + substr_size);
            }
        } while ( ERIS_STRING_NPOS != pos);
    }
}/// eris_string_erase


/**
 * @Brief: Erase substring from eris string.
 *
 * @Param: __es, The eris string.
 * @Param: __at, Begin of this index.
 * @Param: __substr, The dest of string.
 *
 * @Param: Nothing.
 **/
eris_void_t eris_string_erase_at( eris_string_t __es, eris_size_t __at, const eris_char_t *__substr)
{
    if ( __es && __substr) {
        eris_size_t at  = __at;
        eris_size_t pos = 0;
        eris_size_t substr_size = strlen( __substr);

        do {
            pos = eris_string_find_at( __es, at, __substr);
            if ( ERIS_STRING_NPOS != pos) {
                at += pos;

                /** Clear */
                (eris_void_t )eris_string_cleanup_as( __es, pos, pos + substr_size);
            }
        } while ( ERIS_STRING_NPOS != pos);
    }
}/// eris_string_erase


/**
 * @Brief: Erase substring from eris string.
 *
 * @Param: __es, The eris string.
 * @Param: __esubstr, The dest of eris substring.
 *
 * @Param: Nothing.
 **/
eris_void_t eris_string_erase_eris( eris_string_t __es, const eris_string_t __esubstr)
{
    if ( __es && __esubstr) {
        eris_size_t at  = 0;
        eris_size_t pos = 0;
        eris_size_t esubstr_size = eris_string_size( __esubstr);

        do {
            pos = eris_string_find_at_eris( __es, at, __esubstr);
            if ( ERIS_STRING_NPOS != pos) {
                at += pos;

                /** Clear */
                (eris_void_t )eris_string_cleanup_as( __es, pos, pos + esubstr_size);
            } else { break; }
        } while ( ERIS_STRING_NPOS != pos);
    }
}/// eris_string_erase_eris


/**
 * @Brief: Erase substring from eris string.
 *
 * @Param: __es, The eris string.
 * @Param: __at, Begin of this index.
 * @Param: __esubstr, The dest of eris substring.
 *
 * @Param: Nothing.
 **/
eris_void_t eris_string_erase_at_eris( eris_string_t __es, eris_size_t __at, const eris_string_t __esubstr)
{
    if ( __es && __esubstr) {
        eris_size_t at  = __at;
        eris_size_t pos = 0;
        eris_size_t esubstr_size = eris_string_size( __esubstr);

        do {
            pos = eris_string_find_at_eris( __es, at, __esubstr);
            if ( ERIS_STRING_NPOS != pos) {
                at += pos;

                /** Clear */
                (eris_void_t )eris_string_cleanup_as( __es, pos, pos + esubstr_size);
            } else { break; }
        } while ( ERIS_STRING_NPOS != pos);
    }
}/// eris_string_erase_at_eris


/**
 * @Brief: Find a sub string of the source eris string.
 *
 * @Param: __es, The source eris string.
 * @Param: __substr, The sub string of dest.
 *
 * @Return: If existed result index of the source eris string, Not found is ERIS_STRING_NPOS.
 **/
eris_size_t eris_string_find( const eris_string_t __es, const eris_char_t *__substr)
{
    eris_size_t rc_index = ERIS_STRING_NPOS;

    if ( __es && __substr) {
        eris_size_t es_size = eris_string_size( __es);

        /** have sub string? */
        if ( *__substr && (0 < es_size) ) {
            const eris_char_t *s = (const eris_char_t *) __es;
            const eris_char_t *d = (const eris_char_t *)__substr;

            while ( *s) {
                while ( *d) {
                    if ( *s == *d) {
                        s++; d++;
                    } else {
                        s++; break;
                    }
                }

                if ( *s && *d) {
                    while ( *s) {
                        if ( *s == *d) {
                            s = s - (d - __substr);
                            d = __substr;

                            break;
                        } else { s++; }
                    }/// while ( *s)
                } else { break; }
            }/// while ( *s)

            if ( !*d) { 
                rc_index = (eris_size_t )((s - (d - __substr)) - __es); 
            }
        }
    }

    return rc_index;
}/// eris_string_find


/**
 * @Brief: Find a sub string of the source eris string.
 *
 * @Param: __es, The source eris string.
 * @Param: __at, The index at the source eris string.
 * @Param: __substr, The sub string of dest.
 *
 * @Return: If existed result index of the source eris string, Not found is ERIS_STRING_NPOS.
 **/
eris_size_t eris_string_find_at( const eris_string_t __es, eris_size_t __at, const eris_char_t *__substr)
{
    eris_size_t rc_index = ERIS_STRING_NPOS;

    if ( __es && __substr) {
        eris_size_t es_size = eris_string_size( __es);

        /** have sub string? */
        if ( *__substr && (__at < es_size) ) {
            const eris_char_t *s = (const eris_char_t *) __es + __at;
            const eris_char_t *d = (const eris_char_t *)__substr;

            while ( *s) {
                while ( *d) {
                    if ( *s == *d) {
                        s++; d++;
                    } else {
                        s++; break;
                    }
                }

                if ( *s && *d) {
                    while ( *s) {
                        if ( *s == *d) {
                            s = s - (d - __substr);
                            d = __substr;

                            break;
                        } else { s++; }
                    }/// while ( *s)
                } else { break; }
            }/// while ( *s)

            if ( !*d) { 
                rc_index = (eris_size_t )((s - (d - __substr)) - __es); 
            }
        }
    }

    return rc_index;
}/// eris_string_find_at


/**
 * @Brief: Find a sub string of the source eris string.
 *
 * @Param: __es, The source eris string.
 * @Param: __substr, The sub string of dest.
 *
 * @Return: If existed result index of the source eris string, Not found is ERIS_STRING_NPOS.
 **/
eris_size_t eris_string_find_eris( const eris_string_t __es, const eris_string_t __esubstr)
{
    eris_size_t rc_index = ERIS_STRING_NPOS;

    if ( __es && __esubstr) {
        eris_size_t es_size = eris_string_size( __es);
        eris_size_t esubstr_size = eris_string_size( __esubstr);

        /** have sub string? */
        if ( (0 < esubstr_size) && (esubstr_size <= es_size) ) {
            const eris_char_t *s = (const eris_char_t *)__es;
            const eris_char_t *d = (const eris_char_t *)__esubstr;

            while ( *s) {
                while ( *d) {
                    if ( *s == *d) {
                        s++; d++;
                    } else {
                        s++; break;
                    }
                }

                if ( *s && *d) {
                    while ( *s) {
                        if ( *s == *d) {
                            s = s - (d - __esubstr);
                            d = (const eris_char_t *)__esubstr;

                            break;
                        } else { s++; }
                    }/// while ( *s)
                } else { break; }
            }/// while ( *s)

            if ( !*d) { 
                rc_index = (eris_size_t )((s - esubstr_size) - __es); 
            }
        }
    }

    return rc_index;
}/// eris_string_find_eris


/**
 * @Brief: Find a sub string of the source eris string.
 *
 * @Param: __es, The source eris string.
 * @Param: __at, The index at the source eris string.
 * @Param: __esubstr, The sub string of dest.
 *
 * @Return: If existed result index of the source eris string, Not found is ERIS_STRING_NPOS.
 **/
eris_size_t eris_string_find_at_eris( const eris_string_t __es, eris_size_t __at, const eris_string_t __esubstr)
{
    eris_size_t rc_index = ERIS_STRING_NPOS;

    if ( __es && __esubstr) {
        eris_size_t es_size = eris_string_size( __es);
        eris_size_t esubstr_size = eris_string_size( __esubstr);

        /** have sub string? */
        if ( (0 < esubstr_size) && (__at < es_size) && (esubstr_size < es_size) ) {
            const eris_char_t *s = (const eris_char_t *)__es + __at;
            const eris_char_t *d = (const eris_char_t *)__esubstr;

            while ( *s) {
                while ( *d) {
                    if ( *s == *d) {
                        s++; d++;
                    } else {
                        s++; break;
                    }
                }

                if ( *s && *d) {
                    while ( *s) {
                        if ( *s == *d) {
                            s = s - (d - __esubstr);
                            d = (const eris_char_t *)__esubstr;

                            break;
                        } else { s++; }
                    }/// while ( *s)
                } else { break; }
            }/// while ( *s)

            if ( !*d) { 
                rc_index = (eris_size_t )((s - esubstr_size) - __es); 
            }
        }
    }

    return rc_index;
}/// eris_string_find_at_eris


/**
 * @Brief: Find a sub string from the source eris string end.
 *
 * @Param: __es, The source eris string.
 * @Param: __substr, The sub string of dest.
 *
 * @Return: If existed result index of the source eris string, Not found is ERIS_STRING_NPOS.
 **/
eris_size_t eris_string_rfind( const eris_string_t __es, const eris_char_t *__substr)
{
    eris_size_t rc_index = ERIS_STRING_NPOS;

    if ( __es && __substr) {
        eris_size_t es_size = eris_string_size( __es);
        eris_size_t substr_size = strlen( __substr);

        if ( (0 < substr_size) && (substr_size <= es_size) ) {
            const eris_char_t *s = (const eris_char_t *)__es + es_size - 1;
            const eris_char_t *d = (const eris_char_t *)__substr + substr_size - 1;

            eris_size_t i = es_size - 1;

            while ( (s >= __es) && (d >= __substr) ) {
                if ( *s == *d) {
                    s--; d--;

                } else {
                    s = (const eris_char_t *)__es + i - 1;
                    d = (const eris_char_t *)__substr + substr_size - 1;

                    if ( 0 < i) { i--; } else { break; }
                }
            }

            if ( d < __substr) { rc_index = (eris_size_t )((++s - __es)); }
        }
    }

    return rc_index;
}/// eris_string_rfind


/**
 * @Brief: From end to begin find at a sub string of the source eris string.
 *
 * @Param: __es, The source eris string.
 * @Param: __at, The index at the source eris string.
 * @Param: __substr, The sub string of dest.
 *
 * @Return: If existed result index of the source eris string, Not found is ERIS_STRING_NPOS.
 **/
eris_size_t eris_string_rfind_at( const eris_string_t __es, eris_size_t __at, const eris_char_t *__substr)
{
    eris_size_t rc_index = ERIS_STRING_NPOS;

    if ( __es && __substr) {
        eris_size_t es_size = eris_string_size( __es);
        eris_size_t substr_size = strlen( __substr);

        if ( (0 < substr_size) && (substr_size <= es_size) && 
             (__at < es_size)   && (substr_size < (es_size - __at))
        ) {
            const eris_char_t *s = (const eris_char_t *)__es + es_size - 1;
            const eris_char_t *d = (const eris_char_t *)__substr + substr_size - 1;

            eris_size_t i = es_size - 1;

            while ( (s >= (__es + __at)) && (d >= __substr) ) {
                if ( *s == *d) {
                    s--; d--;

                } else {
                    s = (const eris_char_t *)__es + i - 1;
                    d = (const eris_char_t *)__substr + substr_size - 1;

                    if ( 0 < i) { i--; } else { break; }
                }
            }

            if ( d < __substr) { rc_index = (eris_size_t )((++s - __es)); }
        }
    }

    return rc_index;
}/// eris_string_rfind_at


/**
 * @Brief: From end to begin find a sub string of the source eris string.
 *
 * @Param: __es, The source eris string.
 * @Param: __substr, The sub string of dest.
 *
 * @Return: If existed result index of the source eris string, Not found is ERIS_STRING_NPOS.
 **/
eris_size_t eris_string_rfind_eris( const eris_string_t __es, const eris_string_t __esubstr)
{
    eris_size_t rc_index = ERIS_STRING_NPOS;

    if ( __es && __esubstr) {
        eris_size_t es_size = eris_string_size( __es);
        eris_size_t esubstr_size = eris_string_size( __esubstr);

        if ( (0 < esubstr_size) && (esubstr_size <= es_size)) {
            const eris_char_t *s = (const eris_char_t *)__es + es_size - 1;
            const eris_char_t *d = (const eris_char_t *)__esubstr + esubstr_size - 1;

            eris_size_t i = es_size - 1;

            while ( (s >= __es) && (d >= __esubstr) ) {
                if ( *s == *d) {
                    s--; d--;

                } else {
                    s = (const eris_char_t *)__es + i - 1;
                    d = (const eris_char_t *)__esubstr + esubstr_size - 1;

                    if ( 0 < i) { i--; } else { break; }
                }
            }

            if ( d < __esubstr) { rc_index = (eris_size_t )((++s - __es)); }
        }
    }

    return rc_index;
}/// eris_string_rfind_eris


/**
 * @Brief: From end to begin find at a sub string of the source eris string.
 *
 * @Param: __es, The source eris string.
 * @Param: __at, The index at the source eris string.
 * @Param: __substr, The sub string of dest.
 *
 * @Return: If existed result index of the source eris string, Not found is ERIS_STRING_NPOS.
 **/
eris_size_t eris_string_rfind_at_eris( const eris_string_t __es, eris_size_t __at, const eris_string_t __esubstr)
{
    eris_size_t rc_index = ERIS_STRING_NPOS;

    if ( __es && __esubstr) {
        eris_size_t es_size = eris_string_size( __es);
        eris_size_t esubstr_size = eris_string_size( __esubstr);

        if ( (0 < esubstr_size) && (esubstr_size <= es_size) && 
             (__at < es_size)   && (esubstr_size < (es_size - __at))
        ) {
            const eris_char_t *s = (const eris_char_t *)__es + es_size - 1;
            const eris_char_t *d = (const eris_char_t *)__esubstr + esubstr_size - 1;

            eris_size_t i = es_size - 1;

            while ( (s >= (__es + __at)) && (d >= __esubstr) ) {
                if ( *s == *d) {
                    s--; d--;

                } else {
                    s = (const eris_char_t *)__es + i - 1;
                    d = (const eris_char_t *)__esubstr + esubstr_size - 1;

                    if ( 0 < i) { i--; } else { break; }
                }
            }

            if ( d < __esubstr) { rc_index = (eris_size_t )((++s - __es)); }
        }
    }

    return rc_index;
}/// eris_string_rfind_at_eris


/**
 * @Brief: Compare eris string 1 vs othe string.
 *
 * @Param: __es,  The eris string.
 * @Param: __s,    The other string.
 * @Param: __case, If true ignore case, and vice versa.
 *
 * @Return: If equal is true, and vice versa is false.
 **/
eris_bool_t eris_string_isequal( const eris_string_t __es, const eris_char_t *__s, eris_bool_t __case)
{
    /** Do compare */
    return (0 == eris_string_compare( __es, __s, __case)) ? true : false;
}/// eris_string_isequal


/**
 * @Brief: Compare eris string 1 vs other string with size.
 *
 * @Param: __es,  The eris string.
 * @Param: __s ,   The other string.
 * @Param: __n ,   Compare size of eris string vs other string.
 * @Param: __case, If true ignore case, and vice versa.
 *
 * @Return: If equal is true, and vice versa is false.
 **/
eris_bool_t eris_string_isequal_n( const eris_string_t __es, const eris_char_t *__s, eris_size_t __n, eris_bool_t __case)
{
    /** Do compare n */
    return (0 == eris_string_compare_n( __es, __s, __n, __case));
}/// eris_string_isequal_n


/**
 * @Brief: Compare eris string 1 vs eris string 2.
 *
 * @Param: __es1,  The eris string one.
 * @Param: __es2,  The eris string two.
 * @Param: __case, If true ignore case, and vice versa.
 *
 * @Return: If equal is true, and vice versa is false.
 **/
eris_bool_t eris_string_isequal_eris( const eris_string_t __es1, const eris_string_t __es2, eris_bool_t __case)
{
    /** Do compare eris string */
    return (0 == eris_string_compare_eris( __es1, __es2, __case));
}/// eris_string_isequal_eris


/**
 * @Brief: Replace the source eris string sub string to the replace string.
 *
 * @Param: __esp, The source eris string.
 * @Param: __substr, The sub string.
 * @Param: __repstr, The replace string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_replace( eris_string_t *__esp, const eris_char_t *__substr, const eris_char_t *__repstr)
{
    int rc = 0;

    if ( __esp && *__esp && __substr && __repstr) {
        eris_string_t rc_es = *__esp;

        eris_size_t es_size     = eris_string_size( *__esp);
        eris_size_t es_capacity = eris_string_capacity( *__esp);
        eris_size_t substr_size = strlen( __substr);
        eris_size_t repstr_size = strlen( __repstr);

        eris_size_t pos = 0;

        pos = eris_string_find( *__esp, __substr);
        if ( pos != ERIS_STRING_NPOS) {
            /** Replace in the source eris string */
            if ( (repstr_size <= substr_size)) {
                eris_size_t i  = 0;
                while ( i < repstr_size) {
                    (*__esp)[ pos + i] = __repstr[ i];

                    i++;
                }

                eris_size_t j = pos + substr_size;
                while ( (*__esp)[ j]) {
                    (*__esp)[ pos + i] = (*__esp)[ j];

                    i++; j++;
                }

                while ( (pos + i) < es_size) {
                    (*__esp)[ pos + i] = '\0'; i++;
                }

                switch( ERIS_STRING_TYPE(rc_es)) {
                    case ERIS_STRING_TYPE_5 : 
                    case ERIS_STRING_TYPE_8 : 
                        { 
                            ERIS_STRING_VARP(rc_es,8) {
                                eris_string_varp->size -= (substr_size - repstr_size);
                            }
                        }break;
                    case ERIS_STRING_TYPE_16: 
                        { 
                            ERIS_STRING_VARP(rc_es,16) {
                                eris_string_varp->size -= (substr_size - repstr_size);
                            }
                        }break;
                    case ERIS_STRING_TYPE_32: 
                        { 
                            ERIS_STRING_VARP(rc_es,32) {
                                eris_string_varp->size -= (substr_size - repstr_size);
                            }
                        }break;
                    case ERIS_STRING_TYPE_64: 
                        { 
                            ERIS_STRING_VARP(rc_es,64) {
                                eris_string_varp->size -= (substr_size - repstr_size);
                            }
                        }break;
                    default : break;
                }

            } else {
                eris_size_t step = repstr_size - substr_size;
                if ( step <= es_capacity) {
                    eris_char_t *ep = (eris_char_t *)(*__esp) + es_size;
                    eris_char_t *tp = ep + step;

                    while ( ((eris_char_t *)(*__esp) + pos + substr_size) <= ep) {
                        *tp-- = *ep--;
                    }

                    eris_size_t i = 0;
                    while ( i < repstr_size) {
                        (*__esp)[ pos + i] = __repstr[ i];

                        i++;
                    }

                    switch( ERIS_STRING_TYPE(rc_es)) {
                        case ERIS_STRING_TYPE_5 : 
                        case ERIS_STRING_TYPE_8 : 
                            { 
                                ERIS_STRING_VARP(rc_es,8) {
                                    eris_string_varp->size += step;
                                }
                            }break;
                        case ERIS_STRING_TYPE_16: 
                            { 
                                ERIS_STRING_VARP(rc_es,16) {
                                    eris_string_varp->size += step;
                                }
                            }break;
                        case ERIS_STRING_TYPE_32: 
                            { 
                                ERIS_STRING_VARP(rc_es,32) {
                                    eris_string_varp->size += step;
                                }
                            }break;
                        case ERIS_STRING_TYPE_64: 
                            { 
                                ERIS_STRING_VARP(rc_es,64) {
                                    eris_string_varp->size += step;
                                }
                            }break;
                        default : break;
                    }
                } else {
                    /** Create new eris string */
                    rc_es = eris_string_alloc( es_size + repstr_size);
                    if ( rc_es) {
                        /** Copy */
                        (eris_void_t )eris_string_copy_n_eris( rc_es, *__esp, pos);
                        (eris_void_t )eris_string_copy( rc_es, __repstr);
                        (eris_void_t )eris_string_copy( rc_es, (const eris_char_t *)(*__esp) + pos + substr_size);

                        /** Free old */
                        (eris_void_t )eris_string_free( *__esp);
                        *__esp = rc_es;
                    } else {
                        rc = EERIS_ERROR;
                    }
                }
            }
        }
    }

    return rc;
}/// eris_string_replace


/**
 * @Brief: Replace at begin of the source eris string sub string to the replace string.
 *
 * @Param: __esp, The source eris string.
 * @Param: __at, At the begin index.
 * @Param: __substr, The sub string.
 * @Param: __repstr, The replace string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_replace_at( eris_string_t *__esp, eris_size_t __at, const eris_char_t *__substr, const eris_char_t *__repstr)
{
    eris_int_t rc = 0;

    if ( __esp && (*__esp) && __substr && __repstr) {
        eris_string_t rc_es = *__esp;

        eris_size_t es_size     = eris_string_size( *__esp);
        eris_size_t es_capacity = eris_string_capacity( *__esp);
        eris_size_t substr_size = strlen( __substr);
        eris_size_t repstr_size = strlen( __repstr);

        eris_size_t pos = 0;

        pos = eris_string_find_at( (*__esp), __at, __substr);
        if ( pos != ERIS_STRING_NPOS) {
            /** Replace in the source eris string */
            if ( (repstr_size <= substr_size)) {
                eris_size_t i  = 0;
                while ( i < repstr_size) {
                    (*__esp)[ pos + i] = __repstr[ i];

                    i++;
                }

                eris_size_t j = pos + substr_size;
                while ( (*__esp)[ j]) {
                    (*__esp)[ pos + i] = (*__esp)[ j];

                    i++; j++;
                }

                while ( (pos + i) < es_size) {
                    (*__esp)[ pos + i] = '\0'; i++;
                }

                switch( ERIS_STRING_TYPE(rc_es)) {
                    case ERIS_STRING_TYPE_5 : 
                    case ERIS_STRING_TYPE_8 : 
                        { 
                            ERIS_STRING_VARP(rc_es,8) {
                                eris_string_varp->size -= (substr_size - repstr_size);
                            }
                        }break;
                    case ERIS_STRING_TYPE_16: 
                        { 
                            ERIS_STRING_VARP(rc_es,16) {
                                eris_string_varp->size -= (substr_size - repstr_size);
                            }
                        }break;
                    case ERIS_STRING_TYPE_32: 
                        { 
                            ERIS_STRING_VARP(rc_es,32) {
                                eris_string_varp->size -= (substr_size - repstr_size);
                            }
                        }break;
                    case ERIS_STRING_TYPE_64: 
                        { 
                            ERIS_STRING_VARP(rc_es,64) {
                                eris_string_varp->size -= (substr_size - repstr_size);
                            }
                        }break;
                    default : break;
                }

            } else {
                eris_size_t step = repstr_size - substr_size;
                if ( step <= es_capacity) {
                    eris_char_t *ep = (eris_char_t *)(*__esp) + es_size;
                    eris_char_t *tp = ep + step;

                    while ( ((eris_char_t *)(*__esp) + pos + substr_size) <= ep) {
                        *tp-- = *ep--;
                    }

                    eris_size_t i = 0;
                    while ( i < repstr_size) {
                        (*__esp)[ pos + i] = __repstr[ i];

                        i++;
                    }

                    switch( ERIS_STRING_TYPE(rc_es)) {
                        case ERIS_STRING_TYPE_5 : 
                        case ERIS_STRING_TYPE_8 : 
                            { 
                                ERIS_STRING_VARP(rc_es,8) {
                                    eris_string_varp->size += step;
                                }
                            }break;
                        case ERIS_STRING_TYPE_16: 
                            { 
                                ERIS_STRING_VARP(rc_es,16) {
                                    eris_string_varp->size += step;
                                }
                            }break;
                        case ERIS_STRING_TYPE_32: 
                            { 
                                ERIS_STRING_VARP(rc_es,32) {
                                    eris_string_varp->size += step;
                                }
                            }break;
                        case ERIS_STRING_TYPE_64: 
                            { 
                                ERIS_STRING_VARP(rc_es,64) {
                                    eris_string_varp->size += step;
                                }
                            }break;
                        default : break;
                    }
                } else {
                    /** Create new eris string */
                    rc_es = eris_string_alloc( es_size + repstr_size);
                    if ( rc_es) {
                        /** Copy */
                        (eris_void_t )eris_string_copy_n_eris( rc_es, (*__esp), pos);
                        (eris_void_t )eris_string_copy( rc_es, __repstr);
                        (eris_void_t )eris_string_copy( rc_es, (const eris_char_t *)(*__esp) + pos + substr_size);

                        /** Free old */
                        (eris_void_t )eris_string_free( *__esp);
                        *__esp = rc_es;
                    } else { rc = EERIS_ERROR; }
                }
            }
        }
    }

    return rc;
}/// eris_string_replace_at


/**
 * @Brief: Replace all the source eris string sub string to the replace string.
 *
 * @Param: __esp, The source eris string.
 * @Param: __substr, The sub string.
 * @Param: __repstr, The replace string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_replace_all( eris_string_t *__esp, const eris_char_t *__substr, const eris_char_t *__repstr)
{
    eris_int_t rc = 0;

    if ( __esp && (*__esp) && __substr && __repstr) {
        eris_size_t pos = 0;
        eris_size_t at  = 0;
        eris_size_t repstr_size = strlen( __repstr);

        do { 
            pos = eris_string_find_at( *__esp, at, __substr);
            if ( pos != ERIS_STRING_NPOS) {
                /** replace at */
                rc = eris_string_replace_at( __esp, pos, __substr, __repstr);
                if ( 0 != rc) {
                    break;
                }

                at = (pos + repstr_size);
            } else { break; }
        } while ( pos != ERIS_STRING_NPOS);
    }

    return rc;
}/// eris_string_replace_all


/**
 * @Brief: Replace all the source eris string sub string to the replace string.
 *
 * @Param: __esp, The source eris string.
 * @Param: __at, At the begin index.
 * @Param: __substr, The sub string.
 * @Param: __repstr, The replace string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_replace_all_at( eris_string_t *__esp, eris_size_t __at, const eris_char_t *__substr, const eris_char_t *__repstr)
{
    eris_int_t rc = 0;

    if ( __esp && (*__esp) && __substr && __repstr) {
        eris_size_t pos = 0;
        eris_size_t at  = __at;
        eris_size_t repstr_size = strlen( __repstr);

        do { 
            pos = eris_string_find_at( *__esp, at, __substr);
            if ( pos != ERIS_STRING_NPOS) {
                /** replace at */
                rc = eris_string_replace_at( __esp, pos, __substr, __repstr);
                if ( 0 != rc) {
                    break;
                }

                at = (pos + repstr_size);
            } else { break; }
        } while ( pos != ERIS_STRING_NPOS);
    }

    return rc;
}/// eris_string_replace_all_at


/**
 * @Brief: Replace the source eris string sub string to the replace string.
 *
 * @Param: __es, The source eris string.
 * @Param: __substr, The eris sub string.
 * @Param: __repstr, The eris replace string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_replace_eris( eris_string_t *__esp, const eris_string_t __substr, const eris_string_t __repstr)
{
    eris_int_t rc = 0;

    if ( __esp && (*__esp) && __substr && __repstr) {
        eris_string_t rc_es = *__esp;

        eris_size_t es_size     = eris_string_size( *__esp);
        eris_size_t es_capacity = eris_string_capacity( *__esp);
        eris_size_t substr_size = eris_string_size( __substr);
        eris_size_t repstr_size = eris_string_size( __repstr);

        eris_size_t pos = 0;

        pos = eris_string_find_eris( *__esp, __substr);
        if ( pos != ERIS_STRING_NPOS) {
            /** Replace in the source eris string */
            if ( (repstr_size <= substr_size)) {
                eris_size_t i  = 0;
                while ( i < repstr_size) {
                    (*__esp)[ pos + i] = __repstr[ i];

                    i++;
                }

                eris_size_t j = pos + substr_size;
                while ( (*__esp)[ j]) {
                    (*__esp)[ pos + i] = (*__esp)[ j];

                    i++; j++;
                }

                while ( (pos + i) < es_size) {
                    (*__esp)[ pos + i] = '\0'; i++;
                }

                switch( ERIS_STRING_TYPE(rc_es)) {
                    case ERIS_STRING_TYPE_5 : 
                    case ERIS_STRING_TYPE_8 : 
                        { 
                            ERIS_STRING_VARP(rc_es,8) {
                                eris_string_varp->size -= (substr_size - repstr_size);
                            }
                        }break;
                    case ERIS_STRING_TYPE_16: 
                        { 
                            ERIS_STRING_VARP(rc_es,16) {
                                eris_string_varp->size -= (substr_size - repstr_size);
                            }
                        }break;
                    case ERIS_STRING_TYPE_32: 
                        { 
                            ERIS_STRING_VARP(rc_es,32) {
                                eris_string_varp->size -= (substr_size - repstr_size);
                            }
                        }break;
                    case ERIS_STRING_TYPE_64: 
                        { 
                            ERIS_STRING_VARP(rc_es,64) {
                                eris_string_varp->size -= (substr_size - repstr_size);
                            }
                        }break;
                    default : break;
                }

            } else {
                eris_size_t step = repstr_size - substr_size;
                if ( step <= es_capacity) {
                    eris_char_t *ep = (eris_char_t *)(*__esp) + es_size;
                    eris_char_t *tp = ep + step;

                    while ( ((eris_char_t *)(*__esp) + pos + substr_size) <= ep) {
                        *tp-- = *ep--;
                    }

                    eris_size_t i = 0;
                    while ( i < repstr_size) {
                        (*__esp)[ pos + i] = __repstr[ i];

                        i++;
                    }

                    switch( ERIS_STRING_TYPE(rc_es)) {
                        case ERIS_STRING_TYPE_5 : 
                        case ERIS_STRING_TYPE_8 : 
                            { 
                                ERIS_STRING_VARP(rc_es,8) {
                                    eris_string_varp->size += step;
                                }
                            }break;
                        case ERIS_STRING_TYPE_16: 
                            { 
                                ERIS_STRING_VARP(rc_es,16) {
                                    eris_string_varp->size += step;
                                }
                            }break;
                        case ERIS_STRING_TYPE_32: 
                            { 
                                ERIS_STRING_VARP(rc_es,32) {
                                    eris_string_varp->size += step;
                                }
                            }break;
                        case ERIS_STRING_TYPE_64: 
                            { 
                                ERIS_STRING_VARP(rc_es,64) {
                                    eris_string_varp->size += step;
                                }
                            }break;
                        default : break;
                    }
                } else {
                    /** Create new eris string */
                    rc_es = eris_string_alloc( es_size + repstr_size);
                    if ( rc_es) {
                        /** Copy */
                        (eris_void_t )eris_string_copy_n_eris( rc_es, (*__esp), pos);
                        (eris_void_t )eris_string_copy_eris( rc_es, __repstr);
                        (eris_void_t )eris_string_copy_n( rc_es, (const eris_char_t *)(*__esp) + pos + substr_size, (es_size - (pos + substr_size)));

                        /** Free old */
                        (eris_void_t )eris_string_free( *__esp);
                        *__esp = rc_es;
                    } else { rc = EERIS_ERROR; }
                }
            }
        }
    }

    return rc;
}/// eris_string_replace_eris


/**
 * @Brief: Replace the source eris string sub string to the replace string.
 *
 * @Param: __es, The source eris string.
 * @Param: __at, At the begin index.
 * @Param: __substr, The eris sub string.
 * @Param: __repstr, The eris replace string.
 *
 * @Return: The result of replace eris string, if error is NULL.
 **/
eris_int_t eris_string_replace_at_eris( eris_string_t *__esp, eris_size_t __at, const eris_string_t __substr, const eris_string_t __repstr)
{
    eris_int_t rc = 0;

    if ( __esp && (*__esp) && __substr && __repstr) {
        eris_string_t rc_es = *__esp;

        eris_size_t es_size     = eris_string_size( *__esp);
        eris_size_t es_capacity = eris_string_capacity( *__esp);
        eris_size_t substr_size = eris_string_size( __substr);
        eris_size_t repstr_size = eris_string_size( __repstr);

        eris_size_t pos = 0;

        pos = eris_string_find_at_eris( *__esp, __at, __substr);
        if ( pos != ERIS_STRING_NPOS) {
            /** Replace in the source eris string */
            if ( (repstr_size <= substr_size)) {
                eris_size_t i  = 0;
                while ( i < repstr_size) {
                    (*__esp)[ pos + i] = __repstr[ i];

                    i++;
                }

                eris_size_t j = pos + substr_size;
                while ( (*__esp)[ j]) {
                    (*__esp)[ pos + i] = (*__esp)[ j];

                    i++; j++;
                }

                while ( (pos + i) < es_size) {
                    (*__esp)[ pos + i] = '\0'; i++;
                }

                switch( ERIS_STRING_TYPE(rc_es)) {
                    case ERIS_STRING_TYPE_5 : 
                    case ERIS_STRING_TYPE_8 : 
                        { 
                            ERIS_STRING_VARP(rc_es,8) {
                                eris_string_varp->size -= (substr_size - repstr_size);
                            }
                        }break;
                    case ERIS_STRING_TYPE_16: 
                        { 
                            ERIS_STRING_VARP(rc_es,16) {
                                eris_string_varp->size -= (substr_size - repstr_size);
                            }
                        }break;
                    case ERIS_STRING_TYPE_32: 
                        { 
                            ERIS_STRING_VARP(rc_es,32) {
                                eris_string_varp->size -= (substr_size - repstr_size);
                            }
                        }break;
                    case ERIS_STRING_TYPE_64: 
                        { 
                            ERIS_STRING_VARP(rc_es,64) {
                                eris_string_varp->size -= (substr_size - repstr_size);
                            }
                        }break;
                    default : break;
                }

            } else {
                eris_size_t step = repstr_size - substr_size;
                if ( step <= es_capacity) {
                    eris_char_t *ep = (eris_char_t *)(*__esp) + es_size;
                    eris_char_t *tp = ep + step;

                    while ( ((eris_char_t *)(*__esp) + pos + substr_size) <= ep) {
                        *tp-- = *ep--;
                    }

                    eris_size_t i = 0;
                    while ( i < repstr_size) {
                        (*__esp)[ pos + i] = __repstr[ i];

                        i++;
                    }

                    switch( ERIS_STRING_TYPE(rc_es)) {
                        case ERIS_STRING_TYPE_5 : 
                        case ERIS_STRING_TYPE_8 : 
                            { 
                                ERIS_STRING_VARP(rc_es,8) {
                                    eris_string_varp->size += step;
                                }
                            }break;
                        case ERIS_STRING_TYPE_16: 
                            { 
                                ERIS_STRING_VARP(rc_es,16) {
                                    eris_string_varp->size += step;
                                }
                            }break;
                        case ERIS_STRING_TYPE_32: 
                            { 
                                ERIS_STRING_VARP(rc_es,32) {
                                    eris_string_varp->size += step;
                                }
                            }break;
                        case ERIS_STRING_TYPE_64: 
                            { 
                                ERIS_STRING_VARP(rc_es,64) {
                                    eris_string_varp->size += step;
                                }
                            }break;
                        default : break;
                    }
                } else {
                    /** Create new eris string */
                    rc_es = eris_string_alloc( es_size + repstr_size);
                    if ( rc_es) {
                        /** Copy */
                        (eris_void_t )eris_string_copy_n_eris( rc_es, (*__esp), pos);
                        (eris_void_t )eris_string_copy_eris( rc_es, __repstr);
                        (eris_void_t )eris_string_copy_n( rc_es, (const eris_char_t *)(*__esp) + pos + substr_size, (es_size - (pos + substr_size)));

                        /** Free old */
                        (eris_void_t )eris_string_free( *__esp);
                        *__esp = rc_es;
                    } else { rc = EERIS_ERROR; }
                }
            }
        }
    }

    return rc;
}/// eris_string_replace_at_eris


/**
 * @Brief: Replace all the source eris string sub string to the replace string.
 *
 * @Param: __esp, The source eris string.
 * @Param: __substr, The eris sub string.
 * @Param: __repstr, The eris replace string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_replace_all_eris( eris_string_t *__esp, const eris_string_t __substr, const eris_string_t __repstr)
{
    eris_int_t rc = 0;

    if ( __esp && (*__esp) && __substr && __repstr) {
        eris_size_t pos = 0;
        eris_size_t at  = 0;
        eris_size_t repstr_size = eris_string_size( __repstr);

        do { 
            pos = eris_string_find_at_eris( *__esp, at, __substr);
            if ( pos != ERIS_STRING_NPOS) {
                /** replace at */
                rc = eris_string_replace_at_eris( __esp, pos, __substr, __repstr);
                if ( 0 != rc) {
                    break;
                }

                at = (pos + repstr_size);
            } else { break; }
        } while ( pos != ERIS_STRING_NPOS);
    }

    return rc;
}/// eris_string_replace_all_eris


/**
 * @Brief: Replace all the source eris string sub string to the replace string.
 *
 * @Param: __esp, The source eris string.
 * @Param: __at, At the begin index.
 * @Param: __substr, The eris sub string.
 * @Param: __repstr, The eris replace string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_replace_all_at_eris( eris_string_t *__esp, eris_size_t __at, const eris_string_t __substr, const eris_string_t __repstr)
{
    eris_int_t rc = 0;

    if ( __esp && (*__esp) && __substr && __repstr) {
        eris_size_t pos = 0;
        eris_size_t at  = __at;
        eris_size_t repstr_size = eris_string_size( __repstr);

        do { 
            pos = eris_string_find_at_eris( *__esp, at, __substr);
            if ( pos != ERIS_STRING_NPOS) {
                /** replace at */
                rc = eris_string_replace_at_eris( __esp, pos, __substr, __repstr);
                if ( 0 != rc) {
                    break;
                }

                at = (pos + repstr_size);
            } else { break; }
        } while ( pos != ERIS_STRING_NPOS);
    }

    return rc;
}/// eris_string_replace_all_at_eris


/**
 * @Brief: Get the eris string size.
 *
 * @Param: __es, The eris string object.
 *
 * @Return: The eris string size.
 **/
eris_size_t eris_string_size( const eris_string_t __es)
{
    eris_size_t rc_size = 0;
    
    if ( __es) {
        switch( ERIS_STRING_TYPE(__es)) {
            case ERIS_STRING_TYPE_5 : 
            case ERIS_STRING_TYPE_8 : 
                { 
                    ERIS_STRING_VARP(__es,8) {
                        rc_size = eris_string_varp->size;
                    }
                }break;
            case ERIS_STRING_TYPE_16: 
                { 
                    ERIS_STRING_VARP(__es,16) {
                        rc_size = eris_string_varp->size;
                    }
                }break;
            case ERIS_STRING_TYPE_32: 
                { 
                    ERIS_STRING_VARP(__es,32) {
                        rc_size = eris_string_varp->size;
                    }
                }break;
            case ERIS_STRING_TYPE_64: 
                { 
                    ERIS_STRING_VARP(__es,64) {
                        rc_size = eris_string_varp->size;
                    }
                }break;
            default : break;
        }
    }/// fi ( __es)

    return rc_size;
}/// eris_string_size


/**
 * @Brief: Do format of eris string. However this function only handles an incompatible subset of 
 *         printf alike format sepcifiers:
 *         %c - C character.
 *         %s - C string.
 *         %S - The eris string.
 *         %i - Signed int.
 *         %u - Unsigned int.
 *         %l - Signed long int.
 *         %L - Unsigned long int.
 *         %I - Signed long long.
 *         %U  Unsigned long long.
 *         %% - Verbatim "%" character.
 *
 * @Param: __es,  The source eris string.
 * @Param: __fmt, The print format string.
 * @Param: ...,   The other input arguments.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_printf( eris_string_t *__esp, const eris_char_t *__fmt, ...)
{
    eris_int_t rc = 0;

    if ( __esp && __fmt) {
        va_list vap;
        va_start( vap, __fmt);

        /** Do vprintf */
        rc = eris_string_vprintf( __esp, __fmt, vap);

        va_end( vap);
    }

    return rc;
}/// eris_string_printf


/**
 * @Brief: Append format of eris string. However this function only handles an incompatible subset of 
 *         printf alike format sepcifiers:
 *         %c - C character.
 *         %s - C string.
 *         %S - The eris string.
 *         %i - Signed int.
 *         %u - Unsigned int.
 *         %l - Signed long int.
 *         %L - Unsigned long int.
 *         %I - Signed long long.
 *         %U  Unsigned long long.
 *         %% - Verbatim "%" character.
 *
 * @Param: __es,  The source eris string.
 * @Param: __fmt, The print format string.
 * @Param: ...,   The other input arguments.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_printf_append( eris_string_t *__esp, const eris_char_t *__fmt, ...)
{
    eris_int_t rc = 0;

    if ( __esp && __fmt) {
        va_list vap;
        va_start( vap, __fmt);

        eris_string_t tmp_es = NULL;

        /** Do vprintf */
        rc = eris_string_vprintf( &tmp_es, __fmt, vap);
        if ( 0 == rc) {
            /** Append */
            rc = eris_string_append_eris( __esp, tmp_es);
        }

        if ( tmp_es) { (eris_void_t )eris_string_free( tmp_es); tmp_es = NULL; }

        va_end( vap);
    }

    return rc;
}/// eris_string_printf


/**
 * @Brief: Do format var list of eris string. However this function only handles an incompatible subset of 
 *         printf alike format sepcifiers:
 *         %c - C character.
 *         %s - C string.
 *         %S - The eris string.
 *         %i - Signed int.
 *         %u - Unsigned int.
 *         %l - Signed long int.
 *         %L - Unsigned long int.
 *         %I - Signed long long.
 *         %U - Unsigned long long.
 *         %% - Verbatim "%" character.
 *
 * @Param: __es,  The source eris string.
 * @Param: __fmt, The print format string.
 * @Param: __vap  The other input arguments list.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_vprintf( eris_string_t *__esp, const eris_char_t *__fmt, va_list __vap)
{
    eris_int_t rc = 0;

    if ( __esp && __fmt) {
        (eris_void_t )eris_string_cleanup( *__esp);
        const eris_char_t *p = __fmt;

        while ( *p) {
            eris_char_t next_c = 0;
            
            /** Match is '%'? */
            switch ( *p) {
                case '%':
                    {
                        next_c = *(p + 1); p++;
                        switch ( next_c) {
                            case 'c' :
                                {
                                    eris_char_t tmp_c = (eris_char_t )va_arg( __vap, eris_int_t );

                                    /** Save append */
                                    rc = eris_string_append_c( __esp, tmp_c);
                                } break;
                            case 's' :
                                {
                                    const eris_char_t *tmp_s = va_arg( __vap, const eris_char_t *);

                                    /** Save append */
                                    rc = eris_string_append( __esp, tmp_s);
                                } break;
                            case 'S' :
                                {
                                    eris_string_t tmp_es = va_arg( __vap, eris_string_t );

                                    /** Save append */
                                    rc = eris_string_append_eris( __esp, tmp_es);
                                } break;
                            case 'i' :
                                {
                                    eris_int_t v = va_arg( __vap, eris_int_t );

                                    eris_string_t tmp_es = NULL;
                                    rc = eris_string_itoa( &tmp_es, v);
                                    if ( 0 == rc) {
                                        /** Save append */
                                        rc = eris_string_append_eris( __esp, tmp_es);

                                        (eris_void_t )eris_string_free( tmp_es); tmp_es = NULL;
                                    }
                                } break;
                            case 'u' :
                                {
                                    eris_uint_t v = va_arg( __vap, eris_uint_t );

                                    eris_string_t tmp_es = NULL;
                                    rc = eris_string_utoa( &tmp_es, v);
                                    if ( 0 == rc) {
                                        /** Save append */
                                        rc = eris_string_append_eris( __esp, tmp_es);

                                        (eris_void_t )eris_string_free( tmp_es); tmp_es = NULL;
                                    }
                                } break;
                            case 'l' :
                                {
                                    eris_long_t v = va_arg( __vap, eris_long_t );

                                    eris_string_t tmp_es = NULL;
                                    rc = eris_string_ltoa( &tmp_es, v);
                                    if ( 0 == rc) {
                                        /** Save append */
                                        rc = eris_string_append_eris( __esp, tmp_es);

                                        (eris_void_t )eris_string_free( tmp_es); tmp_es = NULL;
                                    }
                                } break;
                            case 'L' :
                                {
                                    eris_ulong_t v = va_arg( __vap, eris_ulong_t );

                                    eris_string_t tmp_es = NULL;
                                    rc = eris_string_ultoa( &tmp_es, v);
                                    if ( 0 == rc) {
                                        /** Save append */
                                        rc = eris_string_append_eris( __esp, tmp_es);

                                        (eris_void_t )eris_string_free( tmp_es); tmp_es = NULL;
                                    }
                                } break;
                            case 'I' :
                                {
                                    eris_llong_t v = va_arg( __vap, eris_llong_t );

                                    eris_string_t tmp_es = NULL;
                                    rc = eris_string_lltoa( &tmp_es, v);
                                    if ( 0 == rc) {
                                        /** Save append */
                                        rc = eris_string_append_eris( __esp, tmp_es);

                                        (eris_void_t )eris_string_free( tmp_es); tmp_es = NULL;
                                    }
                                } break;
                            case 'U' :
                                {
                                    eris_ullong_t v = va_arg( __vap, eris_ullong_t );

                                    eris_string_t tmp_es = NULL;
                                    rc = eris_string_ulltoa( &tmp_es, v);
                                    if ( 0 == rc) {
                                        /** Save append */
                                        rc = eris_string_append_eris( __esp, tmp_es);

                                        (eris_void_t )eris_string_free( tmp_es); tmp_es = NULL;
                                    }
                                } break;
                            case '%' :
                                {
                                    /** Save append */
                                    rc = eris_string_append_c( __esp, '%');
                                } break;
                            default :
                                {
                                    /** Save append */
                                    rc = eris_string_append_c( __esp, next_c);
                                } break;

                        }/// switch ( next_c)
                    } break;
                default :
                    {
                        /** Save append */
                        rc = eris_string_append_c( __esp, *p);
                    } break;
            }/// switch ( *p)

            if ( 0 != rc) { break; }

            /** Next char */
            p++;
        }/// while ( *p)
    }

    return rc;
}/// eris_string_vprintf


/**
 * @Brief: Append format var list of eris string. However this function only handles an incompatible subset of 
 *         printf alike format sepcifiers:
 *         %c - C character.
 *         %s - C string.
 *         %S - The eris string.
 *         %i - Signed int.
 *         %u - Unsigned int.
 *         %l - Signed long int.
 *         %L - Unsigned long int.
 *         %I - Signed long long.
 *         %U - Unsigned long long.
 *         %% - Verbatim "%" character.
 *
 * @Param: __es,  The source eris string.
 * @Param: __fmt, The print format string.
 * @Param: __vap  The other input arguments list.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_string_vprintf_append( eris_string_t *__esp, const eris_char_t *__fmt, va_list __vap)
{
    eris_int_t rc = 0;

    if ( __esp && __fmt) {
        eris_string_t tmp_es = NULL;

        /** Do vprintf */
        rc = eris_string_vprintf( &tmp_es, __fmt, __vap);
        if ( 0 == rc) {
            /** Append end */
            rc = eris_string_append_eris( __esp, tmp_es);
        }

        if ( tmp_es) { (eris_void_t )eris_string_free( tmp_es); tmp_es = NULL; }
    }

    return rc;
}/// eris_string_vprintf_append


/**
 * @Brief: Reverse the eris string.
 *
 * @Param: __es, The source eris string.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_string_reverse( eris_string_t __es)
{
    if ( __es) {
        eris_size_t es_size = eris_string_size( __es);

        if ( 1 < es_size) {
            eris_char_t *bp = (eris_char_t *)__es;
            eris_char_t *ep = (eris_char_t *)__es + es_size - 1;

            while ( bp < ep) {
                eris_char_t c = *bp;
                *bp++ = *ep;
                *ep-- = c;
            }
        }
    }
}/// eris_string_reverse


/**
 * @Brief: Rotation the eris string.
 *
 * @Param: __es, The source eris string.
 * @Param: __pos,The pos of rotation.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_string_rotation( eris_string_t __es, eris_size_t __pos)
{
    if ( __es) {
        eris_size_t es_size = eris_string_size( __es);

        if ( (1 < es_size) && (__pos < es_size)) {
            eris_char_t *bp  = (eris_char_t *)__es;
            eris_char_t *mp  = (eris_char_t *)__es + __pos;
            eris_char_t *ep  = (eris_char_t *)__es + es_size - 1;

            if ( 0 == __pos) {
                mp++;

                while ( mp < ep) {
                    eris_char_t c = *mp;
                    *mp++  = *ep;
                    *ep--  = c;
                }
            } else if ( (__pos) == (es_size - 1)) {
                mp--;

                while ( bp < mp ) {
                    eris_char_t c = *bp;
                    *bp++  = *mp;
                    *mp--  = c;
                }
            } else {
                mp--;

                while ( bp < mp ) {
                    eris_char_t c = *bp;
                    *bp++  = *mp;
                    *mp--  = c;
                }

                /** Reset mp pointer */
                mp = (eris_char_t *)__es + __pos;
                while ( mp < ep) {
                    eris_char_t c = *mp;
                    *mp++  = *ep;
                    *ep--  = c;
                }
            }

            /** reverse */
            bp = (eris_char_t *)__es;
            ep = (eris_char_t *)__es + es_size - 1;
            while ( bp < ep) {
                eris_char_t c = *bp;
                *bp++  = *ep;
                *ep--  = c;
            }
        }
    }
}/// eris_string_rotation


/**
 * @Brief: Strip the right and left blank character.
 *
 * @Param: __es, The eris string object.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_string_strip( eris_string_t __es)
{
    /** First is right */
    (eris_void_t )eris_string_rstrip( __es);

    /** Second is left */
    (eris_void_t )eris_string_lstrip( __es);
}/// eris_string_strip


/**
 * @Brief: Strip the left blank character.
 *
 * @Param: __es, The eris string object.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_string_lstrip( eris_string_t __es)
{
    if ( __es) {
        eris_size_t es_size = eris_string_size( __es);

        if ( 0 < es_size) {
            eris_size_t tmp_size = 0;
            eris_char_t *s = (eris_char_t *)__es;
            eris_char_t *p = (eris_char_t *)__es;

            while ( *p && isspace( *p)) { p++; tmp_size++; };

            while ( *p ) {
                *s++= *p++;
            }

            while ( s < ((eris_char_t *)__es + es_size)) {
                *s++ = '\0';
            }

            switch( ERIS_STRING_TYPE(__es)) {
                case ERIS_STRING_TYPE_5 : 
                case ERIS_STRING_TYPE_8 : 
                    { 
                        ERIS_STRING_VARP(__es,8) {
                            eris_string_varp->size -= tmp_size;
                        }
                    }break;
                case ERIS_STRING_TYPE_16: 
                    { 
                        ERIS_STRING_VARP(__es,16) {
                            eris_string_varp->size -= tmp_size;
                        }
                    }break;
                case ERIS_STRING_TYPE_32: 
                    { 
                        ERIS_STRING_VARP(__es,32) {
                            eris_string_varp->size -= tmp_size;
                        }
                    }break;
                case ERIS_STRING_TYPE_64: 
                    { 
                        ERIS_STRING_VARP(__es,64) {
                            eris_string_varp->size -= tmp_size;
                        }
                    }break;
                default : break;
            }
        }
    }
}/// eris_string_lstrip


/**
 * @Brief: Strip the right blank character.
 *
 * @Param: __es, The eris string object.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_string_rstrip( eris_string_t __es)
{
    if ( __es) {
        eris_size_t es_size = eris_string_size( __es);

        if ( 0 < es_size) {
            eris_char_t *p = (eris_char_t *)__es + es_size - 1;

            eris_size_t tmp_size = 0;
            while ( p >= __es) {
                if ( isspace( *p)) {
                    *p-- = '\0';
                    tmp_size++;

                } else { 
                    break; 
                }
            }

            switch( ERIS_STRING_TYPE(__es)) {
                case ERIS_STRING_TYPE_5 : 
                case ERIS_STRING_TYPE_8 : 
                    { 
                        ERIS_STRING_VARP(__es,8) {
                            eris_string_varp->size -= tmp_size;
                        }
                    }break;
                case ERIS_STRING_TYPE_16: 
                    { 
                        ERIS_STRING_VARP(__es,16) {
                            eris_string_varp->size -= tmp_size;
                        }
                    }break;
                case ERIS_STRING_TYPE_32: 
                    { 
                        ERIS_STRING_VARP(__es,32) {
                            eris_string_varp->size -= tmp_size;
                        }
                    }break;
                case ERIS_STRING_TYPE_64: 
                    { 
                        ERIS_STRING_VARP(__es,64) {
                            eris_string_varp->size -= tmp_size;
                        }
                    }break;
                default : break;
            }
        }
    }
}/// eris_string_rstrip


/**
 * @Brief: Split the eris string.
 *
 * @Param: __s,  The source string.
 * @Param: __sp, The splitators string.
 *
 * @Return: If soucessed reuslt of eris string array, other is NULL.
 **/
eris_string_array_t eris_string_split( const eris_char_t *__s, const eris_char_t *__sp)
{
    eris_string_array_t rc_es_array = NULL;

    if ( __s) {
        /** Create eris string */
        eris_string_t tmp_es = eris_string_create( __s);

        rc_es_array = eris_string_split_eris( tmp_es, __sp);

        if ( tmp_es) {
            (eris_void_t )eris_string_free( tmp_es);

            tmp_es = NULL;
        }

    }

    return rc_es_array;
}/// eris_string_split


/**
 * @Brief: Split the eris string.
 *
 * @Param: __es, The eris string.
 * @Param: __sp, The splitators string.
 *
 * @Return: If soucessed reuslt of eris string array, other is NULL.
 **/
eris_string_array_t eris_string_split_eris( const eris_string_t __es, const eris_char_t *__sp)
{
    eris_int_t rc = 0;
    struct eris_string_array_s *rc_es_array = NULL;

    if ( __es) {
        struct eris_string_range_s {
            eris_size_t start;
            eris_size_t end;
        };

        eris_list_t list_range_context; {
            rc = eris_list_init( &list_range_context, false, NULL);
        }

        if ( 0 == rc) {
            eris_size_t start   = 0;
            eris_size_t end     = 0;
            eris_size_t at      = 0;
            eris_size_t sp_size = strlen( __sp);
            eris_size_t es_size = eris_string_size( __es);

            do {
                eris_size_t pos = eris_string_find_at( __es, at, __sp);
                if ( ERIS_STRING_NPOS != pos ) {
                    at  = (pos + sp_size);
                    end = pos;

                    if ( start < end) {
                        struct eris_string_range_s *range_elt = (struct eris_string_range_s *) \
                                                                eris_memory_alloc( sizeof( struct eris_string_range_s));
                        if ( range_elt) {
                            range_elt->start = start;
                            range_elt->end   = end;

                            rc = eris_list_push_back( &list_range_context, range_elt, 0);
                            if ( 0 != rc) {
                                eris_memory_free( range_elt);

                                break;
                            }
                        } else {
                            /** Happen error */
                            rc = EERIS_ERROR;

                            break;
                        }
                    }

                    start = at;
                } else {
                    /** Unmacth or end */
                    if ( (start + 1) < es_size) {
                        struct eris_string_range_s *range_elt = (struct eris_string_range_s *) \
                                                                eris_memory_alloc( sizeof( struct eris_string_range_s));
                        if ( range_elt) {
                            range_elt->start = start;
                            range_elt->end   = es_size;

                            rc = eris_list_push_back( &list_range_context, range_elt, 0);
                            if ( 0 != rc) {
                                eris_memory_free( range_elt);

                                break;
                            }
                        } else {
                            /** Happen error */
                            rc = EERIS_ERROR;

                            break;
                        }
                    }

                    break;
                }

                if ( 0 != rc) { break; }
            } while ( 1);
        }

        /** Get all index pairs ok */
        if ( 0 == rc) {
            eris_size_t count = eris_list_size( &list_range_context);
            if ( 0 < count) {
                rc_es_array = (struct eris_string_array_s *)eris_memory_alloc( sizeof( struct eris_string_array_s) + (sizeof( eris_string_t) * (count + 1)));
                if ( rc_es_array) {
                    rc_es_array->size = count;

                    eris_size_t k = 0;
                    for ( k = 0; k < count; k++) {
                        rc_es_array->array[ k] = NULL;
                    }

                    /** Ok */
                    eris_list_iter_t list_iter_range; {
                        eris_list_iter_init( &list_range_context, &list_iter_range);

                        const struct eris_string_range_s *range_elt_ptr = NULL;

                        eris_size_t i = 0;

                        /** Iter */
                        for ( ; (NULL != (range_elt_ptr = eris_list_iter_next( &list_iter_range))); ) {
                            rc_es_array->array[ i] = eris_string_assign( __es, range_elt_ptr->start, range_elt_ptr->end);
                            if ( rc_es_array->array[ i]) {
                                /** Assign ok */
                                i++;

                            } else {
                                rc = EERIS_ERROR;

                                /** Happend error */
                                eris_string_array_free( (eris_string_array_t )rc_es_array->array);
                                rc_es_array = NULL;

                                break;
                            }
                        }
                    }
                }
            }
        }

        /** Destory */
        eris_list_destroy( &list_range_context, eris_memory_release);
    }

    return (rc_es_array ? rc_es_array->array : NULL);
}/// eris_string_split_eris


/**
 * @Brief: Get at the array element.
 *
 * @Param: __esa, The eris string array.
 * @Param: __at,  At the array index.
 *
 * @Return: The eris string at the array.
 **/
const eris_string_t eris_string_array_at( const eris_string_array_t __esa, eris_size_t __at)
{
    /** The valid eris string array context */
    if ( __esa) {
        struct eris_string_array_s *p = (eris_void_t *)((eris_void_t *)__esa - sizeof( struct eris_string_array_s));
        
        if ( __at < p->size) {
            return (eris_string_t )p->array[ __at];
        }
    }

    return NULL;
}/// eris_string_array_at


/**
 * @Brief: Get the eris string array size.
 *
 * @Param: __esa, The eris string array.
 *
 * @Return: The eris string array size.
 **/
eris_size_t eris_string_array_size( const eris_string_array_t __esa)
{
    /** The valid eris string array context */
    if ( __esa) {
        struct eris_string_array_s *p = (eris_void_t *)((eris_void_t *)__esa - sizeof( struct eris_string_array_s));
        
        return p->size;
    }

    return 0;
}/// eris_string_size


/**
 * @Brief: Release the eris string array.
 *
 * @Param: __es, The eris string objects.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_string_array_free( eris_string_array_t __esa)
{
    /** The valid eris string array context */
    if ( __esa) {
        struct eris_string_array_s *p = (eris_void_t *)((eris_void_t *)__esa - sizeof( struct eris_string_array_s));
        
        for ( eris_size_t i = 0; (i < p->size); i++) {
            /** Eris string free */
            if ( p->array[ i]) {
                (eris_void_t )eris_string_free( p->array[ i]);

                p->array[ i] = NULL;
            }
        }/// for ( i < p->size)

        eris_memory_free( p); p = NULL;
    }/// fi ( __esa)
}/// eris_string_array_free


/**
 * @Brief: To lower with eris string.
 *
 * @Param: __es, The eris string.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_string_tolower( eris_string_t __es)
{
    if ( __es) {
        eris_size_t es_size = eris_string_size( __es);
        if ( 0 < es_size) {
            for ( eris_size_t i = 0; i < es_size; i++) {
                if ( ('A' <= __es[ i]) && ('Z' >= __es[ i])) {
                    __es[ i] |= 0x20;
                }
            }
        }
    }
}/// eris_string_tolower


/**
 * @Brief: To lower with at begin of the eris string.
 *
 * @Param: __es, The eris string.
 * @param: __at, At the begin of eris string.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_string_tolower_at( eris_string_t __es, eris_size_t __at)
{
    if ( __es) {
        eris_size_t es_size = eris_string_size( __es);
        if ( __at < es_size) {
            for ( eris_size_t i = __at; i < es_size; i++) {
                if ( ('A' <= __es[ i]) && ('Z' >= __es[ i])) {
                    __es[ i] |= 0x20;
                }
            }
        }
    }
}/// eris_string_tolower_at


/**
 * @Brief: To lower with eris string.
 *
 * @Param: __es, The eris string.
 * @param: __n,  To lower n charactors.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_string_tolower_n( eris_string_t __es, eris_size_t __n)
{
    if ( __es) {
        eris_size_t es_size = eris_string_size( __es);

        if ( __n > es_size) {
            __n = es_size;
        }

        if ( 0 < __n) {
            for ( eris_size_t i = 0; i < __n; i++) {
                if ( ('A' <= __es[ i]) && ('Z' >= __es[ i])) {
                    __es[ i] |= 0x20;
                }
            }
        }
    }
}/// eris_string_tolower_n


/**
 * @Brief: To lower with at begin of the eris string n size.
 *
 * @Param: __es, The eris string.
 * @param: __at, At the begin of eris string.
 * @param: __n,  To lower n charactors.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_string_tolower_at_n( eris_string_t __es, eris_size_t __at, eris_size_t __n)
{
    if ( __es && (0 < __n)) {
        eris_size_t es_size = eris_string_size( __es);

        if ( __n > es_size) {
            __n = es_size;
        }

        if ( (0 < __n) && (__at < es_size) ) {
            eris_size_t m = 0;

            for ( eris_size_t i = __at; (i < es_size) && (m < __n); i++, m++) {
                if ( ('A' <= __es[ i]) && ('Z' >= __es[ i])) {
                    __es[ i] |= 0x20;
                }
            }
        }
    }
}/// eris_string_tolower_at_n


/**
 * @Brief: To upper with eris string.
 *
 * @Param: __es, The eris string.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_string_toupper( eris_string_t __es)
{
    if ( __es) {
        eris_size_t es_size = eris_string_size( __es);
        if ( 0 < es_size) {
            for ( eris_size_t i = 0; i < es_size; i++) {
                if ( ('a' <= __es[ i]) && ('z' >= __es[ i])) {
                    __es[ i] &= ~(0x20);
                }
            }
        }
    }
}/// eris_string_toupper


/**
 * @Brief: To upper with at begin of the eris string.
 *
 * @Param: __es, The eris string.
 * @param: __at, At the begin of eris string.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_string_toupper_at( eris_string_t __es, eris_size_t __at)
{
    if ( __es) {
        eris_size_t es_size = eris_string_size( __es);
        if ( __at < es_size) {
            for ( eris_size_t i = __at; i < es_size; i++) {
                if ( ('a' <= __es[ i]) && ('z' >= __es[ i])) {
                    __es[ i] &= ~(0x20);
                }
            }
        }
    }
}/// eris_string_toupper_at


/**
 * @Brief: To upper with eris string.
 *
 * @Param: __es, The eris string.
 * @param: __n,  To lower n charactors.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_string_toupper_n( eris_string_t __es, eris_size_t __n)
{
    if ( __es) {
        eris_size_t es_size = eris_string_size( __es);

        if ( __n > es_size) {
            __n = es_size;
        }

        if ( 0 < __n) {
            for ( eris_size_t i = 0; i < __n; i++) {
                if ( ('A' <= __es[ i]) && ('Z' >= __es[ i])) {
                    __es[ i] &= ~(0x20);
                }
            }
        }
    }
}/// eris_string_toupper_n


/**
 * @Brief: To upper with at begin of the eris string n size.
 *
 * @Param: __es, The eris string.
 * @param: __at, At the begin of eris string.
 * @param: __n,  To upper n charactors.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_string_toupper_at_n( eris_string_t __es, eris_size_t __at, eris_size_t __n)
{
    if ( __es && (0 < __n)) {
        eris_size_t es_size = eris_string_size( __es);

        if ( __n > es_size) {
            __n = es_size;
        }

        if ( (0 < __n) && (__at < es_size) ) {
            eris_size_t m = 0;

            for ( eris_size_t i = __at; (i < es_size) && (m < __n); i++, m++) {
                if ( ('a' <= __es[ i]) && ('z' >= __es[ i])) {
                    __es[ i] &= ~(0x20);
                }
            }
        }
    }
}/// eris_string_toupper_at_n


/**
 * @Brief: Check the eris string start with the dest string.
 * 
 * @Param: __es, The eris string.
 * @Param: __s,  The dest string.
 *
 * @Return: If start with is true, other is false.
 **/
eris_bool_t eris_string_start_with( const eris_string_t __es, const eris_char_t *__s)
{
    eris_bool_t rc_bool = false;

    if ( __es && __s) {
        const eris_char_t *es = (const eris_char_t *)__es;
        const eris_char_t *s  = (const eris_char_t *)__s;

        eris_size_t es_size = eris_string_size( __es);
        if ( *es && *s) {
            do {
                if ( *es == *s) {
                    es++; s++;
                } else {
                    break; 
                }
            } while ( *s && (es < (__es + es_size)));

            /** Start with s? */
            if ( !*s) { rc_bool = true; }
        } else {
            if ( !(*es) && !(*s)) {
                rc_bool = true;
            }
        }
    }

    return rc_bool;
}/// eris_string_start_with


/**
 * @Brief: Check the eris string start with the dest string.
 * 
 * @Param: __es, The eris string.
 * @Param: __et, The dest eris string.
 *
 * @Return: If start with is true, other is false.
 **/
eris_bool_t eris_string_start_with_eris( const eris_string_t __es, const eris_string_t __et)
{
    eris_bool_t rc_bool = false;

    if ( __es && __et) {
        const eris_char_t *es = (const eris_char_t *)__es;
        const eris_char_t *et = (const eris_char_t *)__et;

        eris_size_t es_size = eris_string_size( __es);
        eris_size_t et_size = eris_string_size( __et);

        if ( es_size >= et_size) {
            if ( *es && *et) {
                do {
                    if ( *es == *et) {
                        es++; et++;
                    } else {
                        break; 
                    }
                } while ( (et < (__et + et_size)) && (es < (__es + es_size)));

                /** Start with et string? */
                if ( et == (__et + et_size)) { rc_bool = true; }
            } else {
                if ( !(*es) && !(*et)) {
                    rc_bool = true;
                }
            }
        }
    }

    return rc_bool;
}/// eris_string_start_with_eris


/**
 * @Brief: Check the eris string end with the dest string.
 * 
 * @Param: __es, The eris string.
 * @Param: __s,  The dest string.
 *
 * @Return: If end with is true, other is false.
 **/
eris_bool_t eris_string_end_with( const eris_string_t __es, const eris_char_t *__s)
{
    eris_bool_t rc_bool = false;

    if ( __es && __s) {
        eris_size_t es_size = eris_string_size( __es);
        eris_size_t s_size  = strlen( __s);

        if ( (0 < s_size) && (s_size <= es_size)) {
            const eris_char_t *es = (const eris_char_t *)__es + es_size - 1;
            const eris_char_t *s  = (const eris_char_t *)__s + s_size - 1;

            do {
                if ( *es == *s) {
                    es--; s--;
                } else {
                    break; 
                }
            } while ( s >= __s);

            /** End with s? */
            if ( s < __s) { rc_bool = true; }
        } else {
            if ( !(*__es) && !(*__s)) {
                rc_bool = true;
            }
        }
    }

    return rc_bool;
}/// eris_string_end_with


/**
 * @Brief: Check the eris string end with the dest string.
 * 
 * @Param: __es, The eris string.
 * @Param: __et, The dest eris string.
 *
 * @Return: If end with is true, other is false.
 **/
eris_bool_t eris_string_end_with_eris( const eris_string_t __es, const eris_string_t __et)
{
    eris_bool_t rc_bool = false;

    if ( __es && __et) {
        eris_size_t es_size = eris_string_size( __es);
        eris_size_t et_size = eris_string_size( __et);

        if ( (0 < et_size) && (et_size <= es_size)) {
            const eris_char_t *es = (const eris_char_t *)__es + es_size - 1;
            const eris_char_t *et = (const eris_char_t *)__et + et_size - 1;

            do {
                if ( *es == *et) {
                    es--; et--;
                } else {
                    break; 
                }
            } while ( et >= __et);

            /** End with s? */
            if ( et < __et) { rc_bool = true; }
        } else {
            if ( !(*__es) && !(*__et)) {
                rc_bool = true;
            }
        }
    }

    return rc_bool;
}/// eris_string_end_with_eris


/**
 * @Brief: Release the eris string.
 *
 * @Param: __es, The eris string objects.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_string_free( eris_string_t __es)
{
    /** The valid eris string context */
    if ( __es) {
        eris_char_t *p = NULL;

        switch ( ERIS_STRING_TYPE( __es)) {
            case ERIS_STRING_TYPE_5 :
            case ERIS_STRING_TYPE_8 :
                {
                    /** Base 8bits */
                    p = (eris_void_t *)ERIS_STRING_BASE( __es, 8);
                } break;
            case ERIS_STRING_TYPE_16:
                {
                    /** Base 16bits */
                    p = (eris_void_t *)ERIS_STRING_BASE( __es, 16);
                } break;
            case ERIS_STRING_TYPE_32:
                {
                    /** Base 32bits */
                    p = (eris_void_t *)ERIS_STRING_BASE( __es, 32);
                } break;
            case ERIS_STRING_TYPE_64:
                {
                    /** Base 64bits */
                    p = (eris_void_t *)ERIS_STRING_BASE( __es, 64);
                } break;
            default : { p = (eris_void_t *)__es; } break;
        }/// switch( ERIS_STRING_TYPE( __es))

        /** Release */
        if ( p) { eris_memory_free( p); p = NULL; }
    }/// fi ( __es)
}/// eris_string_free


