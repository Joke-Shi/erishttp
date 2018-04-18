#include "core/eris.core.h"
#include "eris/eris.version.h"


eris_int_t hello_service( const eris_module_t *__mcontext, eris_http_t *__http, eris_log_t *__log)
{
    eris_int_t rc = 0;

    const eris_char_t *hello_erishttp_s = 
          "<!DOCTYPE html><html><head><title>erishttp"
          "</title></head><body><h2 align=\"center\">"
          "Hello "ERIS_VERSION_DESC
          "</h2></body></html>";
    rc = eris_http_response_set_body( __http, (eris_void_t *)hello_erishttp_s,  strlen( hello_erishttp_s));
    if ( 0 == rc) {
        rc = eris_http_response_set_header( __http, "Content-Type",  "text/html");
        if ( 0 == rc) {
            eris_http_response_set_status( __http, ERIS_HTTP_200);
        } else {
            rc = EERIS_ERROR;

            eris_http_response_set_status( __http, ERIS_HTTP_500);
        }
    } else {
        rc = EERIS_ERROR;

        eris_http_response_set_status( __http, ERIS_HTTP_500);
    }


    return rc;
}



