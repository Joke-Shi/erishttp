
/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Operator time.
 **
 ******************************************************************************/


#include "core/eris.core.h"


/** Gmt time format string */
#define ERIS_TIME_GMTFORMAT ("%a, %d %b %Y %T GMT")


/**
 * @Brief: Get now time to gmt format string.
 *
 * @Param: __out_gmt, Output gmt time format string.
 *
 * @Return: __out_gmt pointer eris string.
 **/
eris_string_t eris_time_gmt( eris_string_t *__out_gmt)
{
    if ( __out_gmt) {
        eris_char_t gmt_buf[ 64] = {0};
        
        eris_time_t t = 0;
        eris_tm_t   tm;

        (eris_none_t )time( &t);

#if (ERIS_HAVE_GMTIME_R)
        (eris_none_t )gmtime_r( &t, &tm);
#else
        tm = *((eris_tm_t *)gmtime( &t));
#endif

        eris_size_t rc = strftime( gmt_buf, sizeof( gmt_buf), ERIS_TIME_GMTFORMAT, &tm);
        if ( 0 < rc) {
            /** Not must */
            if ( rc < sizeof( gmt_buf) ) {
                gmt_buf[ rc] = '\0';
            } else {
                gmt_buf[ sizeof( gmt_buf) - 1] = '\0';
            }

            (eris_none_t)eris_string_set( __out_gmt, gmt_buf);
        }

        return *(__out_gmt);
    }

    return NULL;
}/// eris_time_gmt


/**
 * @Brief: Convet gmt time format string to tm structure.
 *
 * @Param: __out_tm, Output tm value.
 * @Param: __in_gmt, Input gmt time format string.
 *
 * @Return: Ok is 0, Other is EERIS_INVALID or errno.
 **/
eris_int_t eris_time_gmt2tm( eris_tm_t *__out_tm, const eris_string_t __in_gmt)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __out_tm && __in_gmt) {
        if ( !strptime( __in_gmt, ERIS_TIME_GMTFORMAT, __out_tm)) {
            rc = errno;
        }
    } else { rc = EERIS_INVALID; }

    errno = tmp_errno;

    return rc;
}/// eris_time_gmt2tm


