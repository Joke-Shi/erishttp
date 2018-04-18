#include "core/eris.core.h"
#include "eris/eris.version.h"


eris_int_t test_service( const eris_module_t *__mcontext, eris_http_t *__http, eris_log_t *__log)
{
    //eris_log_dump( __log, ERIS_LOG_NOTICE, "%s", __func__);

    eris_int_t rc = 0;

    const eris_buffer_t *p_body = eris_http_request_get_body( __http);

    /** Set body data */
    if ( p_body && (0 < p_body->size)) {
        rc = eris_http_response_set_body( __http, p_body->data, p_body->size);
        if ( 0 == rc) {
            /** Set Content-Type */
            rc = eris_http_response_set_header( __http, "Content-Type", eris_http_request_get_header( __http, "Content-Type"));
            if ( 0 == rc) {
                /** Set status */
                eris_http_response_set_status( __http, ERIS_HTTP_200);

            } else {
                rc = EERIS_ERROR;

                eris_http_response_set_status( __http, ERIS_HTTP_500);
            }
        } else {
            rc = EERIS_ERROR;

            eris_http_response_set_status( __http, ERIS_HTTP_500);
        }
    } else {
        rc = eris_http_response_set_body( __http, (const eris_void_t *)"Erishttp test service", 21);
        if ( 0 == rc) {
            /** Set Content-Type */
            rc = eris_http_response_set_header( __http, "Content-Type", "text/plain");
            if ( 0 == rc) {
                /** Set status */
                eris_http_response_set_status( __http, ERIS_HTTP_200);

            } else {
                rc = EERIS_ERROR;

                eris_http_response_set_status( __http, ERIS_HTTP_500);
            }
        } else {
            rc = EERIS_ERROR;

            eris_http_response_set_status( __http, ERIS_HTTP_500);
        }
    }


    return rc;
}




