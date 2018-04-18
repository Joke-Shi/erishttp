/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This design to parser the config options, like this eg:
 **        : http {
 **        :     listen 0.0.0.0:9436
 **        :     max_conntions   10240
 **        :     max_flow_limits 10240
 **        :     ...
 **        : }
 **
 ******************************************************************************/

#include "core/eris.core.h"


/** Get element pointer with specify key path of the cftree. */
static const eris_cftree_e_t *eris_cftree_get_e( 
    eris_cftree_e_t *__cftree_cur_e, 
    eris_petree_e_t *__petree_parent_e, 
    eris_petree_e_t *__petree_move_e
);

/** Set value withc petree into cftree. */
static eris_int_t  eris_cftree_set_e( 
    eris_cftree_e_t *__cftree_parent_e, 
    eris_cftree_e_t **__cftree_cur_e, 
    eris_petree_e_t *__petree_parent_e, 
    eris_petree_e_t *__petree_move_e, 
    const eris_char_t *__value
);

/** Cleanup all element of parent. */
static eris_void_t eris_cftree_cleanup_e( eris_cftree_e_t **__parent_e_ptr);

/** Dumpout cftree content of begin specify current element. */
static eris_int_t  eris_cftree_dump_e( 
    eris_string_t *__result, 
    const eris_cftree_e_t *__cur_e, 
    eris_uint_t __level, 
    eris_log_t *__log
);

/** Dumpout cftree begin specify current element. */
static eris_void_t eris_cftree_dumpout_e( 
    eris_file_t *__fp, 
    const eris_cftree_e_t *__cur_e, 
    eris_uint_t __level
);

/** Get element counts with specify key path of the cftree. */
static eris_int_t  eris_cftree_esize_e( 
    eris_cftree_e_t *__cftree_cur_e, 
    eris_petree_e_t *__petree_parent_e, 
    eris_petree_e_t *__petree_move_e
);

/** Check petree is existed of the cftree. */
static eris_bool_t eris_cftree_existed_e( 
    eris_cftree_e_t *__cftree_cur_e, 
    eris_petree_e_t *__petree_parent_e, 
    eris_petree_e_t *__petree_move_e
);



/**
 * @Brief: Clear-init.
 *
 * @Param: __cftree, The config tree context.
 *
 * @Return: Nothing.
 **/
void eris_cftree_init( eris_cftree_t *__cftree)
{
    if ( __cftree) {
        __cftree->count = 0;
        __cftree->head.key    = NULL;
        __cftree->head.value  = NULL;
        __cftree->head.parent = NULL;
        __cftree->head.child  = NULL;
        __cftree->head.next   = NULL;
    }
}/// eris_cftree_clear


/**
 * @Brief: Parse the path element string, eg: http.backlog.
 *
 * @Param: __cftree, The path element tree context.
 * @Param: __pe_str, The path element string.
 * @Param: __log   , Record log message out.
 *
 * @Return: Ok->0, other return EERIS_ERROR.
 **/
eris_int_t eris_cftree_parse( eris_cftree_t *__cftree, const eris_char_t *__cfile, eris_log_t *__log)
{
    eris_int_t rc = 0;

    if ( __cftree && __cfile) {
        eris_string_t cf_content = NULL;

        /** Get the config file content */
        rc = eris_cftree_content( &cf_content, __cfile, NULL);
        if ( 0 ==  rc) {
            /** Cleanup all old element */
            (void )eris_cftree_cleanup( __cftree);

            /** Parser cf_content and make cftree */
            enum {
                CF_PARSER_START = 0,
                CF_PARSER_KEY_DQUOT1,
                CF_PARSER_KEY_DQUOT2,
                CF_PARSER_KEY   ,
                CF_PARSER_EMPTY ,
                CF_PARSER_VALUE_DQUOT1,
                CF_PARSER_VALUE_DQUOT2,
                CF_PARSER_VALUE ,
                CF_PARSER_NEXT  ,
                CF_PARSER_LMARK ,
                CF_PARSER_RMARK ,
            } cf_parser_state = CF_PARSER_START;

            /** chain element */
            eris_cftree_e_t  *parent_e_ptr = &(__cftree->head);
            eris_cftree_e_t **chain_e_ptr  = &(__cftree->head.child);

            eris_string_t key_es   = eris_string_alloc( 64);
            eris_string_t value_es = eris_string_alloc( 128);

            for ( eris_size_t i = 0; i < eris_string_size( cf_content); i++) {
                eris_char_t c = cf_content[ i];

                /** State machine */
                switch ( cf_parser_state) {
                    case CF_PARSER_START :
                        {
                            if ( ('\n' == c) || (';' == c) || (' ' == c)) {
                                /** Nothing */
                            } else if ( '{' == c) {
                                cf_parser_state = CF_PARSER_LMARK;

                            } else if ( '\"' == c) { 
                                cf_parser_state = CF_PARSER_KEY_DQUOT1;

                            } else {
                                rc = eris_string_append_c( &key_es, c);
                                if ( 0 != rc) {
                                    rc = EERIS_ERROR; if ( __log) { (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory append a key character failed"); }
                                }

                                cf_parser_state = CF_PARSER_KEY;
                            }
                        } break;
                    case CF_PARSER_KEY_DQUOT1 :
                        {
                            if ( '\"' == c) {
                                cf_parser_state = CF_PARSER_KEY_DQUOT2;
                            } else {
                                rc = eris_string_append_c( &key_es, c);
                                if ( 0 != rc) {
                                    rc = EERIS_ERROR; if ( __log) { (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory append a key character failed"); }
                                }
                            }
                        } break;
                    case CF_PARSER_KEY_DQUOT2 :
                        {
                            if ( ' ' == c) {
                                cf_parser_state = CF_PARSER_EMPTY;

                            } else if ( '\"' == c) {
                                cf_parser_state = CF_PARSER_VALUE_DQUOT1;

                            } else if ( '{' == c ) {
                                (void )eris_string_strip( key_es);
                                (void )eris_string_strip( value_es);

                                *chain_e_ptr = (eris_cftree_e_t *)eris_memory_alloc( sizeof( eris_cftree_e_t));
                                if ( (*chain_e_ptr)) {
                                    __cftree->count++;

                                    (*chain_e_ptr)->parent = parent_e_ptr;
                                    (*chain_e_ptr)->child  = NULL;
                                    (*chain_e_ptr)->next   = NULL;
                                    (*chain_e_ptr)->value  = NULL;
                                    (*chain_e_ptr)->value  = (0 < eris_string_size( value_es)) ? eris_string_clone_eris( value_es) : NULL;
                                    (*chain_e_ptr)->key    = (0 < eris_string_size( key_es)) ? eris_string_clone_eris( key_es) : NULL;
                                } else {
                                    rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory save a key-value into cftree failed");
                                }

                                if ( 0 == rc) { 
                                    parent_e_ptr = *chain_e_ptr;
                                    chain_e_ptr  = &((*chain_e_ptr)->child);

                                    cf_parser_state = CF_PARSER_LMARK;
                                }

                                (void )eris_string_cleanup( key_es);
                                (void )eris_string_cleanup( value_es);
                            } else if ( (';' == c) || ('\n' == c) || ('}' == c) ) {
                                (void )eris_string_strip( key_es);
                                (void )eris_string_strip( value_es);

                                if ( 0 < eris_string_size( key_es)) {
                                    *chain_e_ptr = (eris_cftree_e_t *)eris_memory_alloc( sizeof( eris_cftree_e_t));
                                    if ( *chain_e_ptr) {
                                        __cftree->count++;

                                        (*chain_e_ptr)->parent = parent_e_ptr;
                                        (*chain_e_ptr)->child  = NULL;
                                        (*chain_e_ptr)->next   = NULL;
                                        (*chain_e_ptr)->value  = (0 < eris_string_size( value_es)) ? eris_string_clone_eris( value_es) : NULL;
                                        (*chain_e_ptr)->key    = eris_string_clone_eris( key_es);
                                        if ( !(*chain_e_ptr)->key) {
                                            rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Clone memory save a key into cftree failed");
                                        }
                                    } else {
                                        rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory save a key-value into cftree failed");
                                    }

                                    /** Create node ok */
                                    if ( 0 == rc) {
                                        if ( '}' == c) {
                                            chain_e_ptr  = &(parent_e_ptr->next);
                                            parent_e_ptr = parent_e_ptr->parent;

                                            cf_parser_state = CF_PARSER_RMARK;
                                        } else {
                                            chain_e_ptr  = &((*chain_e_ptr)->next);

                                            cf_parser_state = CF_PARSER_KEY;
                                        }
                                    }

                                    (void )eris_string_cleanup( key_es);
                                    (void )eris_string_cleanup( value_es);
                                } else {
                                    if ( '}' == c) {
                                        chain_e_ptr  = &(parent_e_ptr->next);
                                        parent_e_ptr = parent_e_ptr->parent;

                                        cf_parser_state = CF_PARSER_RMARK;
                                    }
                                }
                            } else {
                                rc = eris_string_append_c( &key_es, c);
                                if ( 0 != rc) {
                                    rc = EERIS_ERROR; if ( __log) { (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory append a key character failed"); }
                                }
                            }
                        } break;
                    case CF_PARSER_KEY :
                        {
                            if ( ' ' == c) {
                                if ( 0 < eris_string_size( key_es)) {
                                    cf_parser_state = CF_PARSER_EMPTY;
                                } else {
                                    /** Nothing */
                                }
                            } else if ( '\"' == c) {
                                if ( 0 < eris_string_size( key_es)) {
                                    cf_parser_state = CF_PARSER_VALUE_DQUOT1;
                                } else { 
                                    cf_parser_state = CF_PARSER_KEY_DQUOT2;
                                }
                            } else if ( '{' == c ) {
                                (void )eris_string_strip( key_es);
                                (void )eris_string_strip( value_es);

                                *chain_e_ptr = (eris_cftree_e_t *)eris_memory_alloc( sizeof( eris_cftree_e_t));
                                if ( (*chain_e_ptr)) {
                                    __cftree->count++;

                                    (*chain_e_ptr)->parent = parent_e_ptr;
                                    (*chain_e_ptr)->child  = NULL;
                                    (*chain_e_ptr)->next   = NULL;
                                    (*chain_e_ptr)->value  = NULL;
                                    (*chain_e_ptr)->value  = (0 < eris_string_size( value_es)) ? eris_string_clone_eris( value_es) : NULL;
                                    (*chain_e_ptr)->key    = (0 < eris_string_size( key_es)) ? eris_string_clone_eris( key_es) : NULL;
                                } else {
                                    rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory save a key-value into cftree failed");
                                }

                                if ( 0 == rc) { 
                                    parent_e_ptr = *chain_e_ptr;
                                    chain_e_ptr  = &((*chain_e_ptr)->child);

                                    cf_parser_state = CF_PARSER_LMARK;
                                }

                                (void )eris_string_cleanup( key_es);
                                (void )eris_string_cleanup( value_es);
                            } else if ( (';' == c) || ('\n' == c) || ('}' == c) ) {
                                (void )eris_string_strip( key_es);
                                (void )eris_string_strip( value_es);

                                if ( 0 < eris_string_size( key_es)) {
                                    *chain_e_ptr = (eris_cftree_e_t *)eris_memory_alloc( sizeof( eris_cftree_e_t));
                                    if ( *chain_e_ptr) {
                                        __cftree->count++;

                                        (*chain_e_ptr)->parent = parent_e_ptr;
                                        (*chain_e_ptr)->child  = NULL;
                                        (*chain_e_ptr)->next   = NULL;
                                        (*chain_e_ptr)->value  = (0 < eris_string_size( value_es)) ? eris_string_clone_eris( value_es) : NULL;
                                        (*chain_e_ptr)->key    = eris_string_clone_eris( key_es);
                                        if ( !(*chain_e_ptr)->key) {
                                            rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Clone memory save a key into cftree failed");
                                        }
                                    } else {
                                        rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory save a key-value into cftree failed");
                                    }

                                    /** Create node ok */
                                    if ( 0 == rc) {
                                        if ( '}' == c) {
                                            chain_e_ptr  = &(parent_e_ptr->next);
                                            parent_e_ptr = parent_e_ptr->parent;

                                            cf_parser_state = CF_PARSER_RMARK;
                                        } else {
                                            chain_e_ptr  = &((*chain_e_ptr)->next);

                                            cf_parser_state = CF_PARSER_KEY;
                                        }
                                    }

                                    (void )eris_string_cleanup( key_es);
                                    (void )eris_string_cleanup( value_es);
                                } else {
                                    if ( '}' == c) {
                                        chain_e_ptr  = &(parent_e_ptr->next);
                                        parent_e_ptr = parent_e_ptr->parent;

                                        cf_parser_state = CF_PARSER_RMARK;
                                    }
                                }
                            } else {
                                rc = eris_string_append_c( &key_es, c);
                                if ( 0 != rc) {
                                    rc = EERIS_ERROR; if ( __log) { (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory append a key character failed"); }
                                }
                            }
                        } break;
                    case CF_PARSER_EMPTY :
                        {
                            if ( ' ' == c) {
                                /** Continue empty state */
                            } else if ( '\"' == c) {
                                cf_parser_state = CF_PARSER_VALUE_DQUOT1;

                            } else if ( '{' == c ) {
                                (void )eris_string_strip( key_es);
                                (void )eris_string_strip( value_es);

                                *chain_e_ptr = (eris_cftree_e_t *)eris_memory_alloc( sizeof( eris_cftree_e_t));
                                if ( (*chain_e_ptr)) {
                                    __cftree->count++;

                                    (*chain_e_ptr)->parent = parent_e_ptr;
                                    (*chain_e_ptr)->child  = NULL;
                                    (*chain_e_ptr)->next   = NULL;
                                    (*chain_e_ptr)->value  = NULL;
                                    (*chain_e_ptr)->value  = (0 < eris_string_size( value_es)) ? eris_string_clone_eris( value_es) : NULL;
                                    (*chain_e_ptr)->key    = (0 < eris_string_size( key_es)) ? eris_string_clone_eris( key_es) : NULL;
                                } else {
                                    rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory save a key-value into cftree failed");
                                }

                                if ( 0 == rc) { 
                                    parent_e_ptr = *chain_e_ptr;
                                    chain_e_ptr  = &((*chain_e_ptr)->child);

                                    cf_parser_state = CF_PARSER_LMARK;
                                }

                                (void )eris_string_cleanup( key_es);
                                (void )eris_string_cleanup( value_es);
                            } else if ( (';' == c) || ('\n' == c) || ('}' == c) ) {
                                (void )eris_string_strip( key_es);
                                (void )eris_string_strip( value_es);

                                if ( 0 < eris_string_size( key_es)) {
                                    *chain_e_ptr = (eris_cftree_e_t *)eris_memory_alloc( sizeof( eris_cftree_e_t));
                                    if ( *chain_e_ptr) {
                                        __cftree->count++;

                                        (*chain_e_ptr)->parent = parent_e_ptr;
                                        (*chain_e_ptr)->child  = NULL;
                                        (*chain_e_ptr)->next   = NULL;
                                        (*chain_e_ptr)->value  = (0 < eris_string_size( value_es)) ? eris_string_clone_eris( value_es) : NULL;
                                        (*chain_e_ptr)->key    = eris_string_clone_eris( key_es);
                                        if ( !(*chain_e_ptr)->key) {
                                            rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Clone memory save a key into cftree failed");
                                        }
                                    } else {
                                        rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory save a key-value into cftree failed");
                                    }

                                    /** Create node ok */
                                    if ( 0 == rc) {
                                        if ( '}' == c) {
                                            chain_e_ptr  = &(parent_e_ptr->next);
                                            parent_e_ptr = parent_e_ptr->parent;

                                            cf_parser_state = CF_PARSER_RMARK;
                                        } else {
                                            chain_e_ptr  = &((*chain_e_ptr)->next);

                                            cf_parser_state = CF_PARSER_KEY;
                                        }
                                    }

                                    (void )eris_string_cleanup( key_es);
                                    (void )eris_string_cleanup( value_es);
                                } else {
                                    if ( '}' == c) {
                                        chain_e_ptr  = &(parent_e_ptr->next);
                                        parent_e_ptr = parent_e_ptr->parent;

                                        cf_parser_state = CF_PARSER_RMARK;
                                    }
                                }
                            } else {
                                rc = eris_string_append_c( &value_es, c);
                                if ( 0 != rc) {
                                    rc = EERIS_ERROR; if ( __log) { (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory append a value character failed"); }
                                }

                                cf_parser_state = CF_PARSER_VALUE;
                            }
                        } break;
                    case CF_PARSER_VALUE_DQUOT1 :
                        {
                            if ( '\"' == c) {
                                cf_parser_state = CF_PARSER_VALUE_DQUOT2;
                            } else {
                                rc = eris_string_append_c( &value_es, c);
                                if ( 0 != rc) {
                                    rc = EERIS_ERROR; if ( __log) { (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory append a value character failed"); }
                                }
                            }
                        } break;
                    case CF_PARSER_VALUE_DQUOT2 :
                        {
                            (void )eris_string_strip( key_es);
                            (void )eris_string_strip( value_es);

                            if ( '{' == c ) {
                                (*chain_e_ptr) = (eris_cftree_e_t *)eris_memory_alloc( sizeof( eris_cftree_e_t));
                                if ( *chain_e_ptr) {
                                    __cftree->count++;

                                    (*chain_e_ptr)->parent = parent_e_ptr;
                                    (*chain_e_ptr)->child  = NULL;
                                    (*chain_e_ptr)->next   = NULL;
                                    (*chain_e_ptr)->value  = (0 < eris_string_size( value_es)) ? eris_string_clone_eris( value_es) : NULL;
                                    (*chain_e_ptr)->key    = (0 < eris_string_size( key_es)) ? eris_string_clone_eris( key_es) : NULL;
                                } else {
                                    rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory save a key-value into cftree failed");
                                }

                                if ( 0 == rc) { 
                                    parent_e_ptr = *chain_e_ptr;
                                    chain_e_ptr  = &((*chain_e_ptr)->child);

                                    cf_parser_state = CF_PARSER_LMARK;
                                }

                                (void )eris_string_cleanup( key_es);
                                (void )eris_string_cleanup( value_es);
                            } else {
                                if ( 0 < eris_string_size( key_es)) {
                                    *chain_e_ptr = (eris_cftree_e_t *)eris_memory_alloc( sizeof( eris_cftree_e_t));
                                    if ( *chain_e_ptr) {
                                        __cftree->count++;

                                        (*chain_e_ptr)->parent = parent_e_ptr;
                                        (*chain_e_ptr)->child  = NULL;
                                        (*chain_e_ptr)->next   = NULL;
                                        (*chain_e_ptr)->value  = (0 < eris_string_size( value_es)) ? eris_string_clone_eris( value_es) : NULL;
                                        (*chain_e_ptr)->key    = eris_string_clone_eris( key_es);
                                        if ( !(*chain_e_ptr)->key) {
                                            rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Clone memory save a key into cftree failed");
                                        }
                                    } else {
                                        rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory save a key-value into cftree failed");
                                    }


                                    /** Clear key eris string */
                                    (void )eris_string_cleanup( key_es);
                                    (void )eris_string_cleanup( value_es);

                                    /** Create node ok */
                                    if ( 0 == rc) {
                                        if ( '}' == c) {
                                            chain_e_ptr  = &(parent_e_ptr->next);
                                            parent_e_ptr = parent_e_ptr->parent;

                                            cf_parser_state = CF_PARSER_RMARK;
                                        } else {
                                            chain_e_ptr  = &((*chain_e_ptr)->next);

                                            if ( !((';' == c) || ('\n' == c) || (' ' == c)) ) {
                                                rc = eris_string_append_c( &key_es, c);
                                                if ( 0 != rc) {
                                                    rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory save a key failed");
                                                }
                                            }

                                            cf_parser_state = CF_PARSER_KEY;
                                        }
                                    }
                                } else {
                                    if ( '}' == c) {
                                        chain_e_ptr  = &(parent_e_ptr->next);
                                        parent_e_ptr = parent_e_ptr->parent;

                                        cf_parser_state = CF_PARSER_RMARK;
                                    }
                                }

                                /** Clear key eris string */
                                (void )eris_string_cleanup( key_es);
                                (void )eris_string_cleanup( value_es);
                            }
                        } break;
                    case CF_PARSER_VALUE :
                        {
                            if ( '{' == c ) {
                                (void )eris_string_strip( key_es);
                                (void )eris_string_strip( value_es);

                                *chain_e_ptr = (eris_cftree_e_t *)eris_memory_alloc( sizeof( eris_cftree_e_t));
                                if ( (*chain_e_ptr)) {
                                    __cftree->count++;

                                    (*chain_e_ptr)->parent = parent_e_ptr;
                                    (*chain_e_ptr)->child  = NULL;
                                    (*chain_e_ptr)->next   = NULL;
                                    (*chain_e_ptr)->value  = NULL;
                                    (*chain_e_ptr)->value  = (0 < eris_string_size( value_es)) ? eris_string_clone_eris( value_es) : NULL;
                                    (*chain_e_ptr)->key    = (0 < eris_string_size( key_es)) ? eris_string_clone_eris( key_es) : NULL;
                                } else {
                                    rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory save a key-value into cftree failed");
                                }

                                if ( 0 == rc) { 
                                    parent_e_ptr = *chain_e_ptr;
                                    chain_e_ptr  = &((*chain_e_ptr)->child);

                                    cf_parser_state = CF_PARSER_LMARK;
                                }

                                (void )eris_string_cleanup( key_es);
                                (void )eris_string_cleanup( value_es);
                            } else if ( (';' == c) || ('\n' == c) || ('}' == c) ) {
                                (void )eris_string_strip( key_es);
                                (void )eris_string_strip( value_es);

                                if ( 0 < eris_string_size( key_es)) {
                                    *chain_e_ptr = (eris_cftree_e_t *)eris_memory_alloc( sizeof( eris_cftree_e_t));
                                    if ( *chain_e_ptr) {
                                        __cftree->count++;

                                        (*chain_e_ptr)->parent = parent_e_ptr;
                                        (*chain_e_ptr)->child  = NULL;
                                        (*chain_e_ptr)->next   = NULL;
                                        (*chain_e_ptr)->value  = (0 < eris_string_size( value_es)) ? eris_string_clone_eris( value_es) : NULL;
                                        (*chain_e_ptr)->key    = eris_string_clone_eris( key_es);
                                        if ( !(*chain_e_ptr)->key) {
                                            rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Clone memory save a key into cftree failed");
                                        }
                                    } else {
                                        rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory save a key-value into cftree failed");
                                    }

                                    (void )eris_string_strip( key_es);
                                    (void )eris_string_strip( value_es);

                                    /** Create node ok */
                                    if ( 0 == rc) {
                                        if ( '}' == c) {
                                            chain_e_ptr  = &(parent_e_ptr->next);
                                            parent_e_ptr = parent_e_ptr->parent;

                                            cf_parser_state = CF_PARSER_RMARK;
                                        } else {
                                            chain_e_ptr  = &((*chain_e_ptr)->next);

                                            cf_parser_state = CF_PARSER_KEY;
                                        }
                                    }

                                    /** Clear key eris string */
                                    (void )eris_string_cleanup( key_es);
                                    (void )eris_string_cleanup( value_es);
                                } else {
                                    if ( '}' == c) {
                                        chain_e_ptr  = &(parent_e_ptr->next);
                                        parent_e_ptr = parent_e_ptr->parent;

                                        cf_parser_state = CF_PARSER_RMARK;
                                    }
                                }
                            } else {
                                rc = eris_string_append_c( &value_es, c);
                                if ( 0 != rc) {
                                    rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory save a value failed");
                                }
                            }
                        } break;
                    case CF_PARSER_NEXT  :
                        {
                        } break;
                    case CF_PARSER_LMARK :
                    case CF_PARSER_RMARK :
                        {
                            if ( '{' == c) {
                                (void )eris_string_strip( key_es);
                                (void )eris_string_strip( value_es);

                                *chain_e_ptr = (eris_cftree_e_t *)eris_memory_alloc( sizeof( eris_cftree_e_t));
                                if ( (*chain_e_ptr)) {
                                    __cftree->count++;

                                    (*chain_e_ptr)->parent = parent_e_ptr;
                                    (*chain_e_ptr)->child  = NULL;
                                    (*chain_e_ptr)->next   = NULL;
                                    (*chain_e_ptr)->value  = NULL;
                                    (*chain_e_ptr)->value  = (0 < eris_string_size( value_es)) ? eris_string_clone_eris( value_es) : NULL;
                                    (*chain_e_ptr)->key    = (0 < eris_string_size( key_es)) ? eris_string_clone_eris( key_es) : NULL;
                                } else {
                                    rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory save a key-value into cftree failed");
                                }

                                if ( 0 == rc) { 
                                    parent_e_ptr = *chain_e_ptr;
                                    chain_e_ptr  = &((*chain_e_ptr)->child);

                                    cf_parser_state = CF_PARSER_LMARK;
                                }

                                (void )eris_string_cleanup( key_es);
                                (void )eris_string_cleanup( value_es);
                            } else if ( '}' == c) {
                                chain_e_ptr  = &(parent_e_ptr->next);
                                parent_e_ptr = parent_e_ptr->parent;

                                cf_parser_state = CF_PARSER_RMARK;

                            } else if ( '\"' == c ) {
                                cf_parser_state = CF_PARSER_KEY_DQUOT1;

                            } else if ( ('\n' == c) || (';' == c) || (' ' == c)) {
                                cf_parser_state = CF_PARSER_KEY;

                            } else {
                                rc = eris_string_append_c( &key_es, c);
                                if ( 0 != rc) {
                                    rc = EERIS_ERROR; if ( __log) (void )eris_log_dump( __log, ERIS_LOG_ERROR, "Alloced memory save a key failed");
                                }

                                cf_parser_state = CF_PARSER_KEY;
                            }
                        } break;
                    default : break;
                }/// switch ( cf_content[ i])

                if ( 0 != rc) { (void )eris_cftree_cleanup( __cftree); break; }
            }/// for ( i < eris_string_size( cf_content))

            /** Release key and value */
            {
                (void )eris_string_free( key_es);   key_es   = NULL;
                (void )eris_string_free( value_es); value_es = NULL;
            }
        } else { rc = EERIS_ERROR; if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, (const eris_char_t *)cf_content); } }

        /** Release cf_content memory */
        (void )eris_string_free( cf_content); cf_content = NULL;

    } else { rc = EERIS_ERROR; if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Input parameters invalid"); } }

    return rc;
}/// eris_cftree_parse


/**
 * @Brief: Get the value of the key path string, eg: http.backlog.
 *
 * @Param: __cftree,   The config tree context.
 * @Param: __key_path, The key path string like this: http.backlog or http.service[2].name
 * @Param: __value,    If successed and set output value string.
 * @Param: __log   , Record log message out.
 *
 * @Return: Ok->0, other is EERIS_ERROR | EERIS_NOTFOUND.
 **/
eris_int_t eris_cftree_get( const eris_cftree_t *__cftree, const eris_char_t *__key_path, eris_string_t *__value, eris_log_t *__log)
{
    eris_int_t rc = 0;

    if ( __cftree && __key_path && __value) {
        if ( __cftree->head.child) {
            eris_errno_t  eris_errno_v = EERIS_OK;
            (void )eris_string_cleanup( *__value);

            eris_petree_t petree_context; {
                (void )eris_petree_init( &petree_context);
            }

            /** Parser path string make syntax tree */
            rc = eris_petree_parser( &petree_context, __key_path, &eris_errno_v);
            if ( 0 == rc) {
                const eris_cftree_e_t *tmp_e_ptr = eris_cftree_get_e( __cftree->head.child, petree_context.head, petree_context.head);
                if ( tmp_e_ptr) {
                    /** Is existed of cfree, A ha!!! */
                    rc = eris_string_append_eris( __value, tmp_e_ptr->value);
                    if ( 0 == rc) {
                        if ( __log) { 
                            eris_log_dump( __log, ERIS_LOG_DEBUG, "Get specify key path.<%s> ok, value.|%s|", __key_path, tmp_e_ptr->value); 
                        } 

                    } else { if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Resave key-value into output alloced failed"); } }
                } else { rc = EERIS_NOTFOUND; if ( __log) { eris_log_dump( __log, ERIS_LOG_INFO, "Get specify key path.<%s> not found", __key_path); } }
            } else { rc = EERIS_ERROR; if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Parser key path string failed, errno.%d", eris_errno_v); } }

            /** Release resources of petree context */
            (void )eris_petree_cleanup( &petree_context);

        } else { rc = EERIS_ERROR; if ( __log) { eris_log_dump( __log, ERIS_LOG_NOTICE, "The cftree has't child elements"); } }
    } else { rc = EERIS_ERROR; if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Input parameters invalid"); } }

    return rc;
}/// eris_cftree_get


/**
 * @Brief: Each elemets of key path, and depth is 1.
 *
 * @Param: __cftree,   The config tree context.
 * @Param: __key_path, The key path string like this: http.backlog or http.service[2].name
 * @Param: __each_cb,  Each callback.
 * @Param: __log,      Record log message out.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_cftree_each( const eris_cftree_t *__cftree, const eris_char_t *__key_path, eris_cftree_each_cb_t __each_cb, eris_log_t *__log)
{
    if ( __cftree && __key_path) {
        if ( __each_cb) {
            eris_int_t rc = 0;

            if ( __cftree->head.child) {
                eris_errno_t  eris_errno_v = EERIS_OK;

                eris_petree_t petree_context; {
                    (void )eris_petree_init( &petree_context);
                }

                /** Parser path string make syntax tree */
                rc = eris_petree_parser( &petree_context, __key_path, &eris_errno_v);
                if ( 0 == rc) {
                    const eris_cftree_e_t *tmp_e_ptr = eris_cftree_get_e( __cftree->head.child, petree_context.head, petree_context.head);
                    if ( tmp_e_ptr) {
                        tmp_e_ptr = tmp_e_ptr->child;

                        for ( ; (NULL != tmp_e_ptr); tmp_e_ptr = tmp_e_ptr->next) {
                            /** Each callback */
                            __each_cb( tmp_e_ptr->key, tmp_e_ptr->value);
                        }

                    } else { if ( __log) { eris_log_dump( __log, ERIS_LOG_INFO, "Get specify key path.<%s> not found", __key_path); } }
                } else { if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Parser key path string failed, errno.%d", eris_errno_v); } }

                /** Release resources of petree context */
                (void )eris_petree_cleanup( &petree_context);

            } else { if ( __log) { eris_log_dump( __log, ERIS_LOG_NOTICE, "The cftree has't child elements"); } }
        }
    } else { if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Input parameters invalid"); } }
}/// eris_cftree_each


/**
 * @Brief: Set the value with the key path string, eg: http.backlog.
 *
 * @Param: __cftree,   The config tree context.
 * @Param: __key_path, The key path string like this: http.backlog or http.service[2].name
 * @Param: __value,    The key path value string.
 * @Param: __log   , Record log message out.
 *
 * @Return: Ok->0, other return EERIS_ERROR.
 **/
eris_int_t eris_cftree_set( eris_cftree_t *__cftree, const eris_char_t *__key_path, const eris_char_t *__value, eris_log_t *__log)
{
    eris_int_t rc = 0;

    if ( __cftree && __key_path) {
        eris_int_t rc = 0;
        eris_errno_t  eris_errno_v = EERIS_OK;

        eris_petree_t petree_context; {
            (void )eris_petree_init( &petree_context);
        }

        /** Parser path string make syntax tree */
        rc = eris_petree_parser( &petree_context, __key_path, &eris_errno_v);
        if ( 0 == rc) {
            rc = eris_cftree_set_e( &(__cftree->head), &(__cftree->head.child), petree_context.head, petree_context.head, __value);
            if ( 0 == rc) {
                if ( __log) { 
                    eris_log_dump( __log, ERIS_LOG_DEBUG, "Get specify key path.<%s> value.|%s| ok", __key_path, __value); 
                } 

            } else { rc = EERIS_ERROR; if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Set specify key path.<%s> alloced failed", __key_path); } }
        } else { rc = EERIS_ERROR; if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Parser key path string failed, errno.%d", eris_errno_v); } }

        /** Release resources of petree context */
        (void )eris_petree_cleanup( &petree_context);

    } else { rc = EERIS_ERROR; if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Input parameters invalid"); } }

    return rc;
}/// eris_cftree_set


/**
 * @Brief: Get element count of key path string specify, eg: http.service
 *
 * @Param: __cftree,   The config tree context.
 * @Param: __key_path, The key path string like this: http.backlog or http.service
 * @Param: __log,      Record log message out.

 * @Return: Ok is count, other is EERIS_ERROR.
 **/
eris_int_t eris_cftree_esize( const eris_cftree_t *__cftree, const eris_char_t *__key_path, eris_log_t *__log)
{
    eris_int_t rc_esize = 0;

    if ( __cftree && __key_path) {
        if ( __cftree->head.child) {
            eris_int_t rc = 0;
            eris_errno_t  eris_errno_v = EERIS_OK;

            eris_petree_t petree_context; {
                (void )eris_petree_init( &petree_context);
            }

            /** Parser path string make syntax tree */
            rc = eris_petree_parser( &petree_context, __key_path, &eris_errno_v);
            if ( 0 == rc) {
                /** Do found and check is existed */
                rc_esize = eris_cftree_esize_e( __cftree->head.child, petree_context.head, petree_context.head);
                if ( 0 < rc_esize ) {
                    if ( __log) { eris_log_dump( __log, ERIS_LOG_DEBUG, "The specify key path.<%s> esize.%d", __key_path, rc_esize); }
                } else {
                    /** Not existed */
                    if ( __log) { eris_log_dump( __log, ERIS_LOG_DEBUG, "The specify key path.<%s> is not found", __key_path); }
                }
            } else { 
                /** Error of parser key path */
                rc_esize = EERIS_ERROR;

                if ( __log) { 
                    if ( EERIS_DATA == eris_errno_v) {
                        eris_log_dump( __log, ERIS_LOG_ERROR, "The specify key path.<%s> string is invalid, please check it", __key_path); 
                    } else {
                        eris_log_dump( __log, ERIS_LOG_ERROR, "Parser key path alloced failed"); 
                    }
                } 
            }

            /** Release resources of petree context */
            (void )eris_petree_cleanup( &petree_context);

        } else { if ( __log) { eris_log_dump( __log, ERIS_LOG_DEBUG, "The cftree has't child elements"); } }
    } else { if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Input parameters invalid"); } }

    return rc_esize;
}/// eris_cftree_esize


/**
 * @Brief: Test element node is existed of key path string specify, eg: http.service
 *
 * @Param: __cftree,   The config tree context.
 * @Param: __key_path, The key path string like this: http.backlog or http.service
 * @Param: __log,      Record log message out.

 * @Return: If existed is true, other is false.
 **/
eris_bool_t eris_cftree_existed( const eris_cftree_t *__cftree, const eris_char_t *__key_path, eris_log_t *__log)
{
    eris_bool_t rc_existed = false;

    if ( __cftree && __key_path) {
        if ( __cftree->head.child) {
            eris_int_t rc = 0;
            eris_errno_t  eris_errno_v = EERIS_OK;

            eris_petree_t petree_context; {
                (void )eris_petree_init( &petree_context);
            }

            /** Parser path string make syntax tree */
            rc = eris_petree_parser( &petree_context, __key_path, &eris_errno_v);
            if ( 0 == rc) {
                /** Do found and check is existed */
                rc_existed = eris_cftree_existed_e( __cftree->head.child, petree_context.head, petree_context.head);
                if ( rc_existed) {
                    if ( __log) { eris_log_dump( __log, ERIS_LOG_DEBUG, "The specify key path.<%s> is existed", __key_path); }

                } else {
                    if ( __log) { eris_log_dump( __log, ERIS_LOG_DEBUG, "The specify key path.<%s> is not existed", __key_path); }
                }
            } else { 
                if ( __log) { 
                    if ( EERIS_DATA == eris_errno_v) {
                        eris_log_dump( __log, ERIS_LOG_ERROR, "The specify key path.<%s> string is invalid, please check it", __key_path); 
                    } else {
                        eris_log_dump( __log, ERIS_LOG_ERROR, "Parser key path alloced failed"); 
                    }
                } 
            }

            /** Release resources of petree context */
            (void )eris_petree_cleanup( &petree_context);

        } else { if ( __log) { eris_log_dump( __log, ERIS_LOG_DEBUG, "The cftree has't child elements"); } }
    } else { if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Input parameters invalid"); } }

    return rc_existed;
}/// eris_cftree_existed


/**
 * @Brief: Dump the cftree to stdout output.
 *
 * @Param: __cftree, The config tree context.
 * @Param: __result, The dump result output, When dit not use, please free it.
 * @Param: __log   , Record log message out.
 *
 * @Return: Ok->0, other return EERIS_ERROR.
 **/
eris_int_t eris_cftree_dump( const eris_cftree_t *__cftree, eris_string_t *__result, eris_log_t *__log)
{
    eris_int_t rc = 0;

    if ( __cftree && __result) {
        eris_bool_t alloc_result = false;

        /** Make cache */
        if ( !(*__result)) {
            *__result = eris_string_alloc( 16000U);
            if ( *__result) {
                alloc_result = true;
            }
        } else {
            (void )eris_string_cleanup( *__result);
        }

        (void )eris_string_printf( __result, "#********************************Count: %i*******************************#\n", __cftree->count);

        /** Dump out */
        rc = eris_cftree_dump_e( __result, __cftree->head.child, 0, __log);

        if ( (0 != rc) && alloc_result ) { 
            (void )eris_string_free( *__result); *__result = NULL;
        }
    } else { rc = EERIS_ERROR; if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Input parameters invalid"); } }

    return rc;
}/// eris_cftree_dump


/**
 * @Brief: Dump the cftree to stdout output.
 *
 * @Param: __cftree, The cftree context.
 * @Param: __out_fp, Dump cftree content out there.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_cftree_dumpout( const eris_cftree_t *__cftree, eris_file_t *__out_fp)
{
    if ( __cftree ) {
        (void )fprintf( __out_fp, "#********************************Count: %d*******************************#\n", __cftree->count);

        /** Begin dump out */
        (void )eris_cftree_dumpout_e( __out_fp,  __cftree->head.child, 0);
    }
}/// eris_cftree_dumpout


/**
 * @Brief: Clean up the cftree.
 *
 * @Param: __cftree_ptr, The cftree context pointer.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_cftree_cleanup( eris_cftree_t *__cftree)
{
    if ( __cftree) {
        /** Clean all elements */
        (void )eris_cftree_cleanup_e( &(__cftree->head.child));

        __cftree->count = 0;
    }
}/// eris_cftree_cleanup


/**
 * @Brief: Test the config file is ok?
 *
 * @Param: __content, The output result content, if error and set error message.
 * @Param: __cfile  , The config file.
 * @Param: __log    , The log context.
 *
 * @Return: If ok 0, other is EERIS_ERROR.
 **/
eris_int_t eris_cftree_test( eris_string_t *__content, const eris_char_t *__cfile, eris_log_t *__log)
{
    eris_int_t rc = 0;

    if ( __cfile) {
        eris_errno_t eris_errno_v = EERIS_OK;

        /** The user want to get content or error message??? */
        if ( __content) {
            if ( !(*__content)) {
                *__content = eris_string_alloc( 8192);
            }

            /** Get config file content */
            rc = eris_cftree_content( __content, __cfile, &eris_errno_v);
            if ( 0 == rc) {
                /** Ok */
                if ( __log) { 
                    eris_log_dump( __log, ERIS_LOG_ERROR, "The config syntax is ok"); 
                }

            } else {
                if ( __log) { 
                    eris_log_dump( __log, ERIS_LOG_ERROR, (const eris_char_t *)(*__content)); 
                }
            }
        } else {
            /** Get config file content */
            eris_string_t tmp_content = eris_string_alloc( 9182);

            rc = eris_cftree_content( &tmp_content, __cfile, &eris_errno_v);
            if ( 0 == rc ) {
                /** Ok */
                if ( __log) { 
                    eris_log_dump( __log, ERIS_LOG_ERROR, "The config syntax is ok"); 
                }

            } else {
                if ( __log) { 
                    eris_log_dump( __log, ERIS_LOG_ERROR, (const eris_char_t *)(tmp_content)); 
                }
            }

            /** Release content memory */
            if ( tmp_content) { (void )eris_string_free( tmp_content); tmp_content = NULL; }
        }/// else->Make tmp content and get config content test
    } else { rc = EERIS_ERROR; if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Input parameters invalid"); } }

    return rc;
}/// eris_cftree_test


/**
 * @Brief: Get the config content from config file.
 *
 * @Param: __content, The output result content. if error and set error message.
 * @Param: __cfile  , The config file path name.
 * @Param: __errno  , If happen error and set it.
 *
 * @Return: Success is 0, other is EERIS_ERROR.
 **/
eris_int_t eris_cftree_content( eris_string_t *__content, const eris_char_t *__cfile, eris_errno_t *__errno)
{
    eris_int_t    rc    = 0;
    eris_size_t lines = 1;
    eris_int_t    tmp_errno = 0;

    if ( __content && __cfile) {
        /** Path string */
        eris_string_t path_es = eris_string_create( __cfile);
        if ( path_es) {
            eris_size_t pos = eris_string_rfind( path_es, "/");
            if ( pos == ERIS_STRING_NPOS) {
                (void )eris_string_cleanup( path_es);

                /** Do current path */
                rc = eris_string_append( &path_es, "./");
                if ( 0 != rc) {
                    __errno ? (*__errno = EERIS_ALLOC) : 0;
                    tmp_errno = EERIS_ALLOC;
                }
            } else {
                /** Clear end file name */
                (void )eris_string_cleanup_as( path_es, (pos + 1), EERIS_ERROR);
            }

            eris_string_t sub_file_name = NULL;
            if ( 0 == rc) {
                sub_file_name = eris_string_alloc( 256);
                if ( !sub_file_name) {
                    rc = EERIS_ERROR; __errno ? (*__errno = EERIS_ALLOC) : 0;
                    tmp_errno = EERIS_ALLOC;
                }
            }

            if ( 0 == rc) {
                /** Check file is existed */
                eris_file_t *fp = fopen( __cfile, "r");
                if ( fp) {
                    enum {
                        CF_CONTENT_START   = 0,
                        CF_CONTENT_CONTENT ,
                        CF_CONTENT_DQUOT1  ,
                        CF_CONTENT_DQUOT2  ,
                        CF_CONTENT_COMMENT ,
                        CF_CONTENT_INC_I   ,
                        CF_CONTENT_INC_N   ,
                        CF_CONTENT_INC_C   ,
                        CF_CONTENT_INC_L   ,
                        CF_CONTENT_INC_U   ,
                        CF_CONTENT_INC_D   ,
                        CF_CONTENT_INC_E   ,
                        CF_CONTENT_INCLUDE ,
                        CF_CONTENT_INC_MARK1,
                        CF_CONTENT_INC_MARK2,
                        CF_CONTENT_BLANK   ,
                        CF_CONTENT_TTAB    ,
                        CF_CONTENT_VTAB    ,
                        CF_CONTENT_SEMICOLON,
                        CF_CONTENT_NEWLINE ,
                        CF_CONTENT_EMPTY   ,
                    } cf_content_state = CF_CONTENT_START;

                    eris_int_t brace_mark_pairs = 0;
                    do {
                        eris_char_t c = 0;
                        eris_size_t read_n = fread( &c, sizeof( eris_char_t), 1, fp);
                        if ( 1 == read_n) {
                            switch ( cf_content_state) {
                                case CF_CONTENT_START :
                                    {
                                        if ( '#' == c) {
                                            cf_content_state = CF_CONTENT_COMMENT;

                                        } else if ( '\n' == c) {
                                            cf_content_state = CF_CONTENT_NEWLINE;

                                        } else if ( ';' == c) {
                                            cf_content_state = CF_CONTENT_SEMICOLON;

                                        } else if ( 'i' == c) {
                                            cf_content_state = CF_CONTENT_INC_I;

                                        } else if ( '\t' == c || '\v' == c || ' ' == c) { 
                                            cf_content_state = CF_CONTENT_EMPTY;

                                        } else { 
                                            rc = eris_string_append_c( __content, c);
                                            if ( 0 != rc) {
                                                __errno ? (*__errno = EERIS_ALLOC) : 0;
                                                tmp_errno = EERIS_ALLOC;
                                            } else {
                                                if ( '{' == c) { brace_mark_pairs++; }
                                                if ( '}' == c) { 
                                                    brace_mark_pairs--;

                                                    if ( 0 > brace_mark_pairs) {
                                                        rc = EERIS_ERROR;
                                                        __errno ? (*__errno = EERIS_DATA) : 0;
                                                        tmp_errno = EERIS_DATA;

                                                        /** A invalid '}' eris_char_t */
                                                        (void )eris_string_printf( __content, "A invalid \'}\' eris_char_t in %s line:%L", __cfile, lines);
                                                    }
                                                }

                                                if ( '\"' == c) {
                                                    cf_content_state = CF_CONTENT_DQUOT1;
                                                } else { 
                                                    cf_content_state = CF_CONTENT_CONTENT; 
                                                }
                                            }
                                        }
                                    } break;
                                case CF_CONTENT_CONTENT :
                                    {
                                        if ( '#' == c) {
                                            cf_content_state = CF_CONTENT_COMMENT;

                                        } else {
                                            if ( '\t' == c || '\v' == c) { 
                                                c = ' ';  
                                            }

                                            /** Append a eris_char_t into content */
                                            rc = eris_string_append_c( __content, c);
                                            if ( 0 != rc) {
                                                __errno ? (*__errno = EERIS_ALLOC) : 0;
                                                tmp_errno = EERIS_ALLOC;
                                            }

                                            if ( '\n' == c ) {
                                                cf_content_state = CF_CONTENT_NEWLINE;
                                            } else if ( ';' == c) {
                                                cf_content_state = CF_CONTENT_SEMICOLON;
                                            } else if ( '\"' == c) {
                                                cf_content_state = CF_CONTENT_DQUOT1;
                                            } else {
                                                if ( '{' == c) { brace_mark_pairs++; }
                                                if ( '}' == c) { 
                                                    brace_mark_pairs--;

                                                    if ( 0 > brace_mark_pairs) {
                                                        rc = EERIS_ERROR;
                                                        __errno ? (*__errno = EERIS_DATA) : 0;
                                                        tmp_errno = EERIS_DATA;

                                                        /** A invalid '}' eris_char_t */
                                                        (void )eris_string_printf( __content, "A invalid \'}\' eris_char_t in %s line:%L", __cfile, lines);
                                                    } 
                                                }
                                            }
                                        }
                                    } break;
                                case CF_CONTENT_DQUOT1 :
                                    {
                                        if ( '\t' == c || '\v' == c) { 
                                            c = ' ';  
                                        }

                                        /** Append a eris_char_t into content */
                                        rc = eris_string_append_c( __content, c);
                                        if ( 0 != rc) {
                                            __errno ? (*__errno = EERIS_ALLOC) : 0;
                                            tmp_errno = EERIS_ALLOC;
                                        }

                                        switch ( c) {
                                            case '\"' :
                                                {
                                                    cf_content_state = CF_CONTENT_CONTENT;
                                                } break;
                                            case '\n' :
                                            case ';'  :
                                            case '{'  :
                                            case '}'  :
                                                {
                                                    rc = EERIS_ERROR;
                                                    __errno ? (*__errno = EERIS_DATA) : 0;
                                                    tmp_errno = EERIS_DATA;

                                                    (void )eris_string_printf( __content, "Missing a \'\"\' eris_char_t in %s line:%L", __cfile, lines);
                                                } break;
                                            default : break;
                                        }
                                    } break;
                                case CF_CONTENT_DQUOT2 :
                                    {
                                    } break;
                                case CF_CONTENT_COMMENT :
                                    {
                                        if ( '\n' == c) {
                                            /** Append a eris_char_t into content */
                                            rc = eris_string_append_c( __content, c);
                                            if ( 0 != rc) {
                                                __errno ? (*__errno = EERIS_ALLOC) : 0;
                                                tmp_errno = EERIS_ALLOC;
                                            }

                                            cf_content_state = CF_CONTENT_NEWLINE;
                                        } 
                                    } break;
                                case CF_CONTENT_INC_I :
                                    {
                                        if ( 'n' == c) {
                                            cf_content_state = CF_CONTENT_INC_N;
                                        } else if ( '#' == c) {
                                            cf_content_state = CF_CONTENT_COMMENT;

                                        } else {
                                            /** Append "in" chars into content */
                                            rc = eris_string_append( __content, "i");
                                            if ( 0 != rc) {
                                                __errno ? (*__errno = EERIS_ALLOC) : 0;
                                                tmp_errno = EERIS_ALLOC;
                                            } else {
                                                if ( '\t' == c || '\v' == c) { c = ' '; }

                                                /** Append a eris_char_t into content */
                                                rc = eris_string_append_c( __content, c);
                                                if ( 0 != rc) {
                                                    __errno ? (*__errno = EERIS_ALLOC) : 0;
                                                    tmp_errno = EERIS_ALLOC;
                                                } else {
                                                    if ( '\n' == c ) {
                                                        cf_content_state = CF_CONTENT_NEWLINE;
                                                    } else if ( ';' == c) {
                                                        cf_content_state = CF_CONTENT_SEMICOLON;
                                                    } else if ( '\"' == c) {
                                                        cf_content_state = CF_CONTENT_DQUOT1;
                                                    } else {
                                                        if ( '{' == c) { brace_mark_pairs++; }
                                                        if ( '}' == c) { 
                                                            brace_mark_pairs--;

                                                            if ( 0 > brace_mark_pairs) {
                                                                rc = EERIS_ERROR;
                                                                __errno ? (*__errno = EERIS_DATA) : 0;
                                                                tmp_errno = EERIS_DATA;

                                                                /** A invalid '}' eris_char_t */
                                                                (void )eris_string_printf( __content, "A invalid \'}\' eris_char_t in %s line:%L", __cfile, lines);
                                                            }
                                                        }

                                                        cf_content_state = CF_CONTENT_CONTENT;
                                                    }
                                                }
                                            }
                                        }
                                    } break;
                                case CF_CONTENT_INC_N :
                                    {
                                        if ( 'c' == c) {
                                            cf_content_state = CF_CONTENT_INC_C;
                                        } else if ( '#' == c) {
                                            cf_content_state = CF_CONTENT_COMMENT;

                                        } else {
                                            /** Append "in" chars into content */
                                            rc = eris_string_append( __content, "in");
                                            if ( 0 != rc) {
                                                __errno ? (*__errno = EERIS_ALLOC) : 0;
                                                tmp_errno = EERIS_ALLOC;
                                            } else {
                                                if ( '\t' == c || '\v' == c) { c = ' '; }

                                                /** Append a eris_char_t into content */
                                                rc = eris_string_append_c( __content, c);
                                                if ( 0 != rc) {
                                                    __errno ? (*__errno = EERIS_ALLOC) : 0;
                                                    tmp_errno = EERIS_ALLOC;
                                                } else {
                                                    if ( '\n' == c ) {
                                                        cf_content_state = CF_CONTENT_NEWLINE;
                                                    } else if ( ';' == c) {
                                                        cf_content_state = CF_CONTENT_SEMICOLON;
                                                    } else if ( '\"' == c) {
                                                        cf_content_state = CF_CONTENT_DQUOT1;
                                                    } else {
                                                        if ( '{' == c) { brace_mark_pairs++; }
                                                        if ( '}' == c) { 
                                                            brace_mark_pairs--;

                                                            if ( 0 > brace_mark_pairs) {
                                                                rc = EERIS_ERROR;
                                                                __errno ? (*__errno = EERIS_DATA) : 0;
                                                                tmp_errno = EERIS_DATA;

                                                                /** A invalid '}' eris_char_t */
                                                                (void )eris_string_printf( __content, "A invalid \'}\' eris_char_t in %s line:%L", __cfile, lines);
                                                            }
                                                        } 
                                                    }

                                                    cf_content_state = CF_CONTENT_CONTENT;
                                                }
                                            }
                                        }
                                    } break;
                                case CF_CONTENT_INC_C :
                                    {
                                        if ( 'l' == c) {
                                            cf_content_state = CF_CONTENT_INC_L;

                                        } else if ( '#' == c) {
                                            cf_content_state = CF_CONTENT_COMMENT;

                                        } else {
                                            /** Append "in" chars into content */
                                            rc = eris_string_append( __content, "inc");
                                            if ( 0 != rc) {
                                                __errno ? (*__errno = EERIS_ALLOC) : 0;
                                                tmp_errno = EERIS_ALLOC;
                                            } else {
                                                if ( '\n' == c ) {
                                                    cf_content_state = CF_CONTENT_NEWLINE;
                                                } else if ( ';' == c) {
                                                    cf_content_state = CF_CONTENT_SEMICOLON;
                                                } else if ( '\"' == c) {
                                                    cf_content_state = CF_CONTENT_DQUOT1;
                                                } else {
                                                    if ( '{' == c) { brace_mark_pairs++; }
                                                    if ( '}' == c) { 
                                                        brace_mark_pairs--;

                                                        if ( 0 > brace_mark_pairs) {
                                                            rc = EERIS_ERROR;
                                                            __errno ? (*__errno = EERIS_DATA) : 0;
                                                            tmp_errno = EERIS_DATA;

                                                            /** A invalid '}' eris_char_t */
                                                            (void )eris_string_printf( __content, "A invalid \'}\' eris_char_t in %s line:%L", __cfile, lines);
                                                        } 
                                                    }

                                                    cf_content_state = CF_CONTENT_CONTENT;
                                                }
                                            }
                                        }
                                    } break;
                                case CF_CONTENT_INC_L :
                                    {
                                        if ( 'u' == c) {
                                            cf_content_state = CF_CONTENT_INC_U;

                                        } else if ( '#' == c) {
                                            cf_content_state = CF_CONTENT_COMMENT;

                                        } else {
                                            /** Append "in" chars into content */
                                            rc = eris_string_append( __content, "incl");
                                            if ( 0 != rc) {
                                                __errno ? (*__errno = EERIS_ALLOC) : 0;
                                                tmp_errno = EERIS_ALLOC;
                                            } else {
                                                if ( '\n' == c ) {
                                                    cf_content_state = CF_CONTENT_NEWLINE;
                                                } else if ( ';' == c) {
                                                    cf_content_state = CF_CONTENT_SEMICOLON;
                                                } else if ( '\"' == c) {
                                                    cf_content_state = CF_CONTENT_DQUOT1;
                                                } else {
                                                    if ( '{' == c) { brace_mark_pairs++; }
                                                    if ( '}' == c) { 
                                                        brace_mark_pairs--;

                                                        if ( 0 > brace_mark_pairs) {
                                                            rc = EERIS_ERROR;
                                                            __errno ? (*__errno = EERIS_DATA) : 0;
                                                            tmp_errno = EERIS_DATA;

                                                            /** A invalid '}' eris_char_t */
                                                            (void )eris_string_printf( __content, "A invalid \'}\' eris_char_t in %s line:%L", __cfile, lines);
                                                        }
                                                    } 
                                                    cf_content_state = CF_CONTENT_CONTENT; 
                                                }
                                            }
                                        }
                                    } break;
                                case CF_CONTENT_INC_U :
                                    {
                                        if ( 'd' == c) {
                                            cf_content_state = CF_CONTENT_INC_D;

                                        } else if ( '#' == c) {
                                            cf_content_state = CF_CONTENT_COMMENT;

                                        } else {
                                            /** Append "in" chars into content */
                                            rc = eris_string_append( __content, "inclu");
                                            if ( 0 != rc) {
                                                __errno ? (*__errno = EERIS_ALLOC) : 0;
                                                tmp_errno = EERIS_ALLOC;
                                            } else {
                                                if ( '\n' == c ) {
                                                    cf_content_state = CF_CONTENT_NEWLINE;
                                                } else if ( ';' == c) {
                                                    cf_content_state = CF_CONTENT_SEMICOLON;
                                                } else if ( '\"' == c) {
                                                    cf_content_state = CF_CONTENT_DQUOT1;
                                                } else {
                                                    if ( '{' == c) { brace_mark_pairs++; }
                                                    if ( '}' == c) { 
                                                        brace_mark_pairs--;

                                                        if ( 0 > brace_mark_pairs) {
                                                            rc = EERIS_ERROR;
                                                            __errno ? (*__errno = EERIS_DATA) : 0;
                                                            tmp_errno = EERIS_DATA;

                                                            /** A invalid '}' eris_char_t */
                                                            (void )eris_string_printf( __content, "A invalid \'}\' eris_char_t in %s line:%L", __cfile, lines);
                                                        }
                                                    } 

                                                    cf_content_state = CF_CONTENT_CONTENT;
                                                }
                                            }
                                        }
                                    } break;
                                case CF_CONTENT_INC_D :
                                    {
                                        if ( 'e' == c) {
                                            cf_content_state = CF_CONTENT_INC_E;

                                        } else if ( '#' == c) {
                                            cf_content_state = CF_CONTENT_COMMENT;

                                        } else {
                                            /** Append "in" chars into content */
                                            rc = eris_string_append( __content, "includ");
                                            if ( 0 != rc) {
                                                __errno ? (*__errno = EERIS_ALLOC) : 0;
                                                tmp_errno = EERIS_ALLOC;
                                            } else {
                                                if ( '\n' == c ) {
                                                    cf_content_state = CF_CONTENT_NEWLINE;
                                                } else if ( ';' == c) {
                                                    cf_content_state = CF_CONTENT_SEMICOLON;
                                                } else if ( '\"' == c) {
                                                    cf_content_state = CF_CONTENT_DQUOT1;
                                                } else {
                                                    if ( '{' == c) { brace_mark_pairs++; }
                                                    if ( '}' == c) { 
                                                        brace_mark_pairs--;

                                                        if ( 0 > brace_mark_pairs) {
                                                            rc = EERIS_ERROR;
                                                            __errno ? (*__errno = EERIS_DATA) : 0;
                                                            tmp_errno = EERIS_DATA;

                                                            /** A invalid '}' eris_char_t */
                                                            (void )eris_string_printf( __content, "A invalid \'}\' eris_char_t in %s line:%L", __cfile, lines);
                                                        }
                                                    } 

                                                    cf_content_state = CF_CONTENT_CONTENT;
                                                }
                                            }
                                        }
                                    } break;
                                case CF_CONTENT_INC_E :
                                    {
                                        if ( ' ' == c || '\t' == c || '\v' == c) {
                                            cf_content_state = CF_CONTENT_BLANK;

                                        } else if ( '\"' == c) {
                                            cf_content_state = CF_CONTENT_INC_MARK1;

                                        } else if ( '#' == c) {
                                            cf_content_state = CF_CONTENT_COMMENT;

                                        } else {
                                            /** Append "in" chars into content */
                                            rc = eris_string_append( __content, "include");
                                            if ( 0 != rc) {
                                                __errno ? (*__errno = EERIS_ALLOC) : 0;
                                                tmp_errno = EERIS_ALLOC;
                                            } else {
                                                if ( '\n' == c ) {
                                                    cf_content_state = CF_CONTENT_NEWLINE;
                                                } else if ( ';' == c) {
                                                    cf_content_state = CF_CONTENT_SEMICOLON;
                                                } else {
                                                    if ( '{' == c) { brace_mark_pairs++; }
                                                    if ( '}' == c) { 
                                                        brace_mark_pairs--;

                                                        if ( 0 > brace_mark_pairs) {
                                                            rc = EERIS_ERROR;
                                                            __errno ? (*__errno = EERIS_DATA) : 0;
                                                            tmp_errno = EERIS_DATA;

                                                            /** A invalid '}' eris_char_t */
                                                            (void )eris_string_printf( __content, "A invalid \'}\' eris_char_t in %s line:%L", __cfile, lines);
                                                        } 
                                                    }

                                                    cf_content_state = CF_CONTENT_CONTENT;
                                                }
                                            }
                                        }
                                    } break;
                                case CF_CONTENT_INCLUDE :
                                    {
                                        eris_string_t tmp_sub_file_path = NULL;
                                        if ( '/' == sub_file_name[0]) {
                                            /** CLone */
                                            tmp_sub_file_path = eris_string_clone( sub_file_name);
                                            if ( !tmp_sub_file_path) {
                                                __errno ? (*__errno = EERIS_ALLOC) : 0; 
                                                tmp_errno = EERIS_ALLOC;
                                            }
                                        } else {
                                            rc = eris_string_append_eris( &tmp_sub_file_path, path_es);
                                            if ( 0 == rc) {
                                                rc = eris_string_append_eris( &tmp_sub_file_path, sub_file_name);
                                                if ( 0 != rc) {
                                                    __errno ? (*__errno = EERIS_ALLOC) : 0; 
                                                    tmp_errno = EERIS_ALLOC;
                                                }
                                            } else { 
                                                __errno ? (*__errno = EERIS_ALLOC) : 0; 
                                                tmp_errno = EERIS_ALLOC;
                                            }  
                                        }

                                        if ( 0 == rc) {
                                            (void )eris_string_strip( tmp_sub_file_path);

                                            /** Goto get next sub file content */
                                            rc = eris_cftree_content( __content, (const eris_char_t *)tmp_sub_file_path, __errno);
                                        }

                                        (void )eris_string_free( tmp_sub_file_path); 
                                        tmp_sub_file_path = NULL;

                                        (void )eris_string_cleanup( sub_file_name);

                                        /** Restart */
                                        if ( '#' == c) {
                                            cf_content_state = CF_CONTENT_COMMENT;

                                        } else if ( '\n' == c) {
                                            cf_content_state = CF_CONTENT_NEWLINE;

                                        } else if ( ';' == c) {
                                            cf_content_state = CF_CONTENT_SEMICOLON;

                                        } else if ( 'i' == c) {
                                            cf_content_state = CF_CONTENT_INC_I;

                                        } else {
                                            if ( '\t' == c || '\v' == c) { c = ' '; }

                                            /** Append a eris_char_t into content */
                                            rc = eris_string_append_c( __content, c);
                                            if ( 0 != rc) {
                                                __errno ? (*__errno = EERIS_ALLOC) : 0;
                                                tmp_errno = EERIS_ALLOC;
                                            } else { 
                                                if ( '{' == c) { brace_mark_pairs++; }
                                                if ( '}' == c) { 
                                                    brace_mark_pairs--;

                                                    if ( 0 > brace_mark_pairs) {
                                                        rc = EERIS_ERROR;
                                                        __errno ? (*__errno = EERIS_DATA) : 0;
                                                        tmp_errno = EERIS_DATA;

                                                        /** A invalid '}' eris_char_t */
                                                        (void )eris_string_printf( __content, "A invalid \'}\' eris_char_t in %s line:%L", __cfile, lines);
                                                    }
                                                }

                                                if ( '\"' == c) { 
                                                    cf_content_state = CF_CONTENT_DQUOT1;
                                                } else {
                                                    cf_content_state = CF_CONTENT_CONTENT;
                                                }
                                            }
                                        }
                                    } break;
                                case CF_CONTENT_INC_MARK1:
                                    {
                                        if ( '\"' == c) {
                                            cf_content_state = CF_CONTENT_INCLUDE;

                                        } else if ( '#' == c) {
                                            cf_content_state = CF_CONTENT_COMMENT;

                                            rc = EERIS_ERROR; __errno ? (*__errno = EERIS_DATA) : 0;
                                            tmp_errno = EERIS_DATA;

                                            (void )eris_string_printf( __content, "Invalid in %s line:%L", __cfile, lines);
                                        } else if ( ';' == c) {
                                            cf_content_state = CF_CONTENT_COMMENT;

                                            rc = EERIS_ERROR; __errno ? (*__errno = EERIS_DATA) : 0;
                                            tmp_errno = EERIS_DATA;

                                            (void )eris_string_printf( __content, "Invalid in %s line:%L", __cfile, lines);
                                        } else if ( '{' == c) {
                                            cf_content_state = CF_CONTENT_COMMENT;

                                            rc = EERIS_ERROR; __errno ? (*__errno = EERIS_DATA) : 0;
                                            tmp_errno = EERIS_DATA;

                                            (void )eris_string_printf( __content, "Invalid in %s line:%L", __cfile, lines);
                                        } else if ( '}' == c) {
                                            cf_content_state = CF_CONTENT_COMMENT;

                                            rc = EERIS_ERROR; __errno ? (*__errno = EERIS_DATA) : 0;
                                            tmp_errno = EERIS_DATA;

                                            (void )eris_string_printf( __content, "Invalid in %s line:%L", __cfile, lines);
                                        } else if ( '\n' == c) {
                                            cf_content_state = CF_CONTENT_NEWLINE;

                                            rc = EERIS_ERROR; __errno ? (*__errno = EERIS_DATA) : 0;
                                            tmp_errno = EERIS_DATA;

                                            (void )eris_string_printf( __content, "Invalid in %s line:%L", __cfile, lines);
                                        } else {
                                            if ( '\t' == c || '\v' == c) { c = ' '; }

                                            rc = eris_string_append_c( &sub_file_name, c);
                                            if ( 0 != rc) {
                                                __errno ? (*__errno = EERIS_ALLOC) : 0;
                                                tmp_errno = EERIS_ALLOC;
                                            }
                                        }
                                    } break;
                                case CF_CONTENT_BLANK :
                                case CF_CONTENT_TTAB :
                                case CF_CONTENT_VTAB :
                                    {
                                        if ( '\"' == c) {
                                            cf_content_state = CF_CONTENT_INC_MARK1;

                                        } else if ( '#' == c) {
                                            cf_content_state = CF_CONTENT_COMMENT;

                                            (void )eris_string_cleanup( sub_file_name);
                                        } else if ( '\n' == c) {
                                            cf_content_state = CF_CONTENT_NEWLINE;

                                            (void )eris_string_cleanup( sub_file_name);
                                        } else if ( (' ' == c) || ('\t' == c) || ('\v' == c)) {
                                            /** Nothing */
                                        } else {
                                            rc = EERIS_ERROR; __errno ? (*__errno = EERIS_DATA) : 0;
                                            tmp_errno = EERIS_DATA;

                                            (void )eris_string_printf( __content, "Invalid in %s line:%L", __cfile, lines);
                                        }
                                    } break;
                                case CF_CONTENT_SEMICOLON :
                                    {
                                        if ( '#' == c) {
                                            cf_content_state = CF_CONTENT_COMMENT;

                                        } else if ( 'i' == c) {
                                            cf_content_state = CF_CONTENT_INC_I;

                                        } else {
                                            if ( '\t' == c || '\v' == c) { c = ' '; }

                                            /** Append a eris_char_t into content */
                                            rc = eris_string_append_c( __content, c);
                                            if ( 0 != rc) {
                                                __errno ? (*__errno = EERIS_ALLOC) : 0;
                                                tmp_errno = EERIS_ALLOC;
                                            } else { 
                                                if ( '\n' == c ) {
                                                    cf_content_state = CF_CONTENT_NEWLINE;
                                                } else if ( ';' == c) {
                                                    cf_content_state = CF_CONTENT_SEMICOLON;
                                                } else if ( ' ' == c) {
                                                    cf_content_state = CF_CONTENT_EMPTY;
                                                } else if ( '\"' == c) {
                                                    cf_content_state = CF_CONTENT_DQUOT1;
                                                } else {
                                                    if ( '{' == c) { brace_mark_pairs++; }
                                                    if ( '}' == c) { 
                                                        brace_mark_pairs--;

                                                        if ( 0 > brace_mark_pairs) {
                                                            rc = EERIS_ERROR;
                                                            __errno ? (*__errno = EERIS_DATA) : 0;
                                                            tmp_errno = EERIS_DATA;

                                                            /** A invalid '}' eris_char_t */
                                                            (void )eris_string_printf( __content, "A invalid \'}\' eris_char_t in %s line:%L", __cfile, lines);
                                                        }
                                                    }

                                                    cf_content_state = CF_CONTENT_CONTENT;
                                                }
                                            }
                                        }
                                    } break;
                                case CF_CONTENT_NEWLINE :
                                    {
                                        lines++;

                                        if ( '#' == c) {
                                            cf_content_state = CF_CONTENT_COMMENT;

                                        } else if ( 'i' == c) {
                                            cf_content_state = CF_CONTENT_INC_I;

                                        } else {
                                            if ( '\t' == c || '\v' == c) { c = ' '; }

                                            /** Append a eris_char_t into content */
                                            rc = eris_string_append_c( __content, c);
                                            if ( 0 != rc) {
                                                __errno ? (*__errno = EERIS_ALLOC) : 0;
                                                tmp_errno = EERIS_ALLOC;
                                            } else {
                                                if ( '\n' == c ) {
                                                    cf_content_state = CF_CONTENT_NEWLINE;
                                                } else if ( ';' == c) {
                                                    cf_content_state = CF_CONTENT_SEMICOLON;
                                                } else if ( ' ' == c) {
                                                    cf_content_state = CF_CONTENT_EMPTY;
                                                } else if ( '\"' == c) {
                                                    cf_content_state = CF_CONTENT_DQUOT1;
                                                } else {
                                                    if ( '{' == c) { brace_mark_pairs++; }
                                                    if ( '}' == c) { 
                                                        brace_mark_pairs--;

                                                        if ( 0 > brace_mark_pairs) {
                                                            rc = EERIS_ERROR;
                                                            __errno ? (*__errno = EERIS_DATA) : 0;
                                                            tmp_errno = EERIS_DATA;

                                                            /** A invalid '}' eris_char_t */
                                                            (void )eris_string_printf( __content, "A invalid \'}\' eris_char_t in %s line:%L", __cfile, lines);
                                                        }
                                                    }

                                                    cf_content_state = CF_CONTENT_CONTENT;
                                                }
                                            }
                                        }
                                    } break;
                                case CF_CONTENT_EMPTY :
                                    {
                                        if ( '#' == c) {
                                            cf_content_state = CF_CONTENT_COMMENT;

                                        } else if ( 'i' == c) {
                                            cf_content_state = CF_CONTENT_INC_I;

                                        } else {
                                            if ( '\t' == c || '\v' == c) { c = ' '; }

                                            /** Append a eris_char_t into content */
                                            rc = eris_string_append_c( __content, c);
                                            if ( 0 != rc) {
                                                __errno ? (*__errno = EERIS_ALLOC) : 0;
                                                tmp_errno = EERIS_ALLOC;
                                            } else {
                                                if ( '\n' == c ) {
                                                    cf_content_state = CF_CONTENT_NEWLINE;
                                                } else if ( ';' == c) {
                                                    cf_content_state = CF_CONTENT_SEMICOLON;
                                                } else if ( ' ' == c) {
                                                    cf_content_state = CF_CONTENT_EMPTY;
                                                } else if ( '\"' == c) {
                                                    cf_content_state = CF_CONTENT_DQUOT1;
                                                } else {
                                                    if ( '{' == c) { brace_mark_pairs++; }
                                                    if ( '}' == c) { 
                                                        brace_mark_pairs--;

                                                        if ( 0 > brace_mark_pairs) {
                                                            rc = EERIS_ERROR;
                                                            __errno ? (*__errno = EERIS_DATA) : 0;
                                                            tmp_errno = EERIS_DATA;

                                                            /** A invalid '}' eris_char_t */
                                                            (void )eris_string_printf( __content, "A invalid \'}\' eris_char_t in %s line:%L", __cfile, lines);
                                                        }
                                                    }

                                                    cf_content_state = CF_CONTENT_CONTENT;
                                                }
                                            }
                                        }
                                    } break;
                                default: break;
                            }
                        } else { break; }

                        /** Jump out of while loop */
                        if ( 0 != rc) { break; }
                    } while ( !feof( fp));

                    /** Close file pointer */
                    fclose( fp); fp = NULL;

                    /** Invalid config file no pair {} */
                    if ( (0 == rc) && (0 < brace_mark_pairs)) {
                        rc = EERIS_ERROR; __errno ? (*__errno = EERIS_DATA) : 0; 
                        tmp_errno = EERIS_DATA;

                        (void )eris_string_printf( __content, "Missing a \'}\' character in %s file end", __cfile);
                    }
                } else { 
                    rc = EERIS_ERROR; __errno ? (*__errno = EERIS_NOSUCH) : 0; 
                    (void )eris_string_printf( __content, "%s: no such config file", __cfile);
                }
            } else { rc = EERIS_ERROR; }

            /** Release eris string path */
            (void )eris_string_free( path_es);
            (void )eris_string_free( sub_file_name);

        } else { rc = EERIS_ERROR; __errno ? (*__errno = EERIS_ALLOC) : 0; }
    } else { rc = EERIS_ERROR; __errno ? (*__errno = EERIS_INVALID) : 0; }

    /** Record error message */
    if ( 0 != rc) {
        if ( EERIS_ALLOC == tmp_errno) { (void )eris_string_append( __content, "Alloced memory failed"); }
    } else { __errno ? (*__errno = EERIS_OK) : 0; }

    return rc;
}/// eris_cftree_content    


/**
 * @Brief: Destroy the cftree.
 *
 * @Param: __cftree_ptr, The cftree context pointer.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_cftree_destroy( eris_cftree_t *__cftree)
{
    if ( __cftree) {
        /** Clean all elements */
        (void )eris_cftree_cleanup_e( &(__cftree->head.child));

        __cftree->count       = 0;
        __cftree->head.parent = NULL;
        __cftree->head.child  = NULL;
        __cftree->head.next   = NULL;
    }
}/// eris_cftree_destroy


/**
 * @Brief: Cleanup all element of parent.
 *
 * @Param: __parent_e_ptr, The parent element pointer.
 * 
 * @Return: Nothing.
 **/
static eris_void_t eris_cftree_cleanup_e( eris_cftree_e_t **__parent_e_ptr)
{
    if ( __parent_e_ptr && *__parent_e_ptr) {
        (void )eris_string_free( (*__parent_e_ptr)->key);
        (void )eris_string_free( (*__parent_e_ptr)->value);

        /** Goto childrens */
        (void )eris_cftree_cleanup_e( &(*__parent_e_ptr)->child);

        /** Goto nexts */
        (void )eris_cftree_cleanup_e( &(*__parent_e_ptr)->next);

        /** Reset parent */
        (*__parent_e_ptr)->parent = NULL;

        eris_memory_free( *__parent_e_ptr);
        *__parent_e_ptr = NULL;
    }
}/// eris_cftree_cleanup_e


/**
 * @Brief: Dumpout cftree content of begin specify current element.
 *
 * @Param; __result, The output result content.
 * @Param: __cur_e , The current element pointer.
 * @Param: __level , The level of cftree.
 * @Param: __log   , If happen error and dump log of there.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
static eris_int_t eris_cftree_dump_e( eris_string_t *__result, const eris_cftree_e_t *__cur_e, eris_uint_t __level, eris_log_t *__log)
{
    eris_int_t rc = 0;

    if ( __cur_e) {
        for ( eris_uint_t i = 0; i < ( 4 * __level); i++) {
            /** Append a preffix empty character */
            rc = eris_string_append_c( __result, ' ');
            if ( 0 != rc) {
                break;
            }
        }

        if ( 0 == rc) {
            /** Dump key */
            rc = eris_string_printf_append( __result, "%S %s", __cur_e->key, __cur_e->value ? __cur_e->value : "");
            if ( 0 == rc) {
                if ( __cur_e->child ) {
                    rc = eris_string_append( __result, "{\n");
                    if ( 0 == rc) {
                        /** Goto child */
                        (void )eris_cftree_dump_e( __result, __cur_e->child, (__level + 1), __log);

                        for ( eris_uint_t i = 0; i < ( 4 * __level); i++) {
                            /** Append a preffix empty character */
                            rc = eris_string_append_c( __result, ' ');
                            if ( 0 != rc) {
                                rc = EERIS_ERROR; if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Append a preffix space character alloced failed"); }
                            }
                        }

                        if ( 0 == rc) {
                            rc = eris_string_append( __result, "}\n");
                            if ( 0 != rc) {
                                rc = EERIS_ERROR; if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Append a \'{\' and a space alloced failed"); }
                            }
                        }

                    } else { rc = EERIS_ERROR; if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Append a \'{\' and a space alloced failed"); } }
                } else { 
                    /** End of {} */
                    rc = eris_string_append( __result, "\n");
                    if ( 0 != rc) {
                        rc = EERIS_ERROR; if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Append a \'{\' and a space alloced failed"); }
                    }
                }

                /** Goto next element node */
                if ( 0 == rc) {
                    (void )eris_cftree_dump_e( __result, __cur_e->next, __level, __log);
                }
            } else { rc = EERIS_ERROR; if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Dump a key-value string alloced failed"); } }
        } else { rc = EERIS_ERROR; if ( __log) { eris_log_dump( __log, ERIS_LOG_ERROR, "Append a preffix space character alloced failed"); } }
    }

    return rc;
}/// end->Func: eris_cftree_dump_e


/**
 * @Brief: Dumpout stdout cftree of begin specify current element.
 *
 * @Param: __fp  , Dump out this file pointer.
 * @Param: __cur_e, The current element pointer.
 * @Param: __level, The level of cftree.
 *
 * @Return: Nothing.
 **/
static eris_void_t eris_cftree_dumpout_e( eris_file_t *__fp, const eris_cftree_e_t *__cur_e, eris_uint_t __level)
{
    if ( __cur_e) {
        eris_file_t *tmp_fp = __fp;
        if ( !tmp_fp) { 
            tmp_fp = stdout;
        }

        for ( eris_uint_t i = 0; i < ( 4 * __level); i++) {
            fprintf( tmp_fp, " ");
        }

        (void )fprintf( tmp_fp, "%s %s", __cur_e->key ? __cur_e->key : "", __cur_e->value ? __cur_e->value : "");

        /** Goto child */
        if ( __cur_e->child) {
            (void )fprintf( tmp_fp, " {\n");

            (void )eris_cftree_dumpout_e( tmp_fp, __cur_e->child, __level + 1);

            for ( eris_uint_t i = 0; i < ( 4 * __level); i++) {
                (void )fprintf( tmp_fp, " ");
            }

            (void )fprintf( tmp_fp, "}\n");

        } else { (void )fprintf( tmp_fp, "\n"); }

        /** Goto next */
        (void )eris_cftree_dumpout_e( tmp_fp, __cur_e->next,  __level);
    }
}/// end->Func: eris_cftree_dumpout_e


/**
 * @Brief: Get element pointer with specify key path of the cftree.
 *
 * @Param: __cftree_cur_e,    The current element node of each.
 * @Param: __petree_parent_e, The petree move element node parent.
 * @Param: __petree_move_e,   The petree move current node of each.
 *
 * @Return: If is existed and then element pointer, other is NULL.
 **/
static const eris_cftree_e_t *eris_cftree_get_e( eris_cftree_e_t *__cftree_cur_e, eris_petree_e_t *__petree_parent_e, eris_petree_e_t *__petree_move_e)
{
    const eris_cftree_e_t *rc_e_ptr = NULL;

    if ( __cftree_cur_e && __petree_move_e) {
        if ( ERIS_PETREE_STRING == __petree_move_e->type) {
            /** Is key string */
            eris_cftree_e_t *tmp_cftree_e = __cftree_cur_e;

            do {
                if ( eris_string_isequal( tmp_cftree_e->key, __petree_move_e->key, false) ) {
                    /** Goto next and match */
                    if ( ( __petree_move_e->child) && 
                         ( ERIS_PETREE_STRING == (__petree_move_e->child)->type)
                     ) {
                        /** Match child */
                        rc_e_ptr = eris_cftree_get_e( tmp_cftree_e->child, __petree_move_e, __petree_move_e->child);

                    } else {
                        if ( __petree_move_e->child) {
                            /** Match petree child index */
                            rc_e_ptr = eris_cftree_get_e( tmp_cftree_e, __petree_move_e, __petree_move_e->child);

                        } else {
                            /** A ha!!! */
                            rc_e_ptr = tmp_cftree_e;
                        }
                    }

                    /** Out of while */
                    break;
                } else { tmp_cftree_e = tmp_cftree_e->next; }
            } while ( tmp_cftree_e);
        } else {
            /** Is key index number */
            eris_cftree_e_t *pre_cftree_e = NULL;
            eris_cftree_e_t *tmp_cftree_e = __cftree_cur_e;

            eris_int_t tmp_index = __petree_move_e->index;
            if ( 0 <= tmp_index) {
                /** Goto index element */
                do {
                    if ( eris_string_isequal( tmp_cftree_e->key, __petree_parent_e->key, false)) {
                        tmp_index--;
                    }

                    if ( 0 > tmp_index) { break; }
                    else { tmp_cftree_e = tmp_cftree_e->next; }
                } while ( tmp_cftree_e);

                pre_cftree_e = tmp_cftree_e;
            } else {
                /** Goto tail of equal move element key value */
                do {
                    if ( eris_string_isequal( tmp_cftree_e->key, __petree_parent_e->key, false)) {
                        pre_cftree_e = tmp_cftree_e;
                    }

                    tmp_cftree_e = tmp_cftree_e->next;
                } while ( tmp_cftree_e);
            }

            /** Match ok, and goto next */
            if ( pre_cftree_e) {
                if ( ( __petree_move_e->child) && 
                     ( ERIS_PETREE_STRING == (__petree_move_e->child)->type)
                ) {
                    /** Match to child */
                    rc_e_ptr = eris_cftree_get_e( pre_cftree_e->child, __petree_move_e, __petree_move_e->child);
                } else {
                    if ( __petree_move_e->child) {
                        /** Match child */
                        rc_e_ptr = eris_cftree_get_e( pre_cftree_e, __petree_move_e, __petree_move_e->child);

                    } else {
                        /** A ha!!! */
                        rc_e_ptr = pre_cftree_e;
                    }
                }
            }
        }
    }

    return rc_e_ptr;
}/// eris_cftree_get_e


/**
 * @Brief: Set value withc petree into cftree.
 *
 * @Param: __cftree_cur_e_ptr,The current element node of each pointer. 
 * @Param: __petree_parent_e, The petree move element node parent.
 * @Param: __petree_move_e,   The petree move current node of each.
 * @Param: __value,           Set the node value.
 *
 * @Return: If successed is 0, other is false.
 **/
static eris_int_t eris_cftree_set_e( eris_cftree_e_t   *__cftree_parent_e,
                              eris_cftree_e_t  **__cftree_cur_e_ptr, 
                              eris_petree_e_t   *__petree_parent_e, 
                              eris_petree_e_t   *__petree_move_e,
                              const eris_char_t *__value
)
{
    eris_int_t rc = 0;

    if ( __cftree_cur_e_ptr && __petree_move_e) {
        if ( *__cftree_cur_e_ptr) {
            if ( ERIS_PETREE_STRING == __petree_move_e->type) {
                eris_bool_t not_found = true;
                /** Is key string */
                do {
                    if ( eris_string_isequal( (*__cftree_cur_e_ptr)->key, __petree_move_e->key, false) ) {
                        not_found = false;

                        /** Goto next and match */
                        if ( __petree_move_e->child) {
                            if ( ERIS_PETREE_STRING == (__petree_move_e->child)->type) {
                                rc = eris_cftree_set_e( (*__cftree_cur_e_ptr), &(*__cftree_cur_e_ptr)->child, __petree_move_e, __petree_move_e->child, __value);

                            } else {
                                /** The next is index element */
                                rc = eris_cftree_set_e( __cftree_parent_e, __cftree_cur_e_ptr, __petree_move_e, __petree_move_e->child, __value);
                            }
                        } else {
                            if ( __value) {
                                (*__cftree_cur_e_ptr)->value = eris_string_clone( __value);
                                if ( !(*__cftree_cur_e_ptr)->value) {
                                    /** Clone value failed */
                                    rc = EERIS_ERROR;
                                }
                            }
                        }

                        break;
                    } else { __cftree_cur_e_ptr = &(*__cftree_cur_e_ptr)->next; }
                } while ( *__cftree_cur_e_ptr);

                if ( not_found) {
                    /** Create a new node */
                    *__cftree_cur_e_ptr = (eris_cftree_e_t *)eris_memory_alloc( sizeof( eris_cftree_e_t));
                    if ( *__cftree_cur_e_ptr) {
                        (*__cftree_cur_e_ptr)->parent = __cftree_parent_e;
                        (*__cftree_cur_e_ptr)->child  = NULL;
                        (*__cftree_cur_e_ptr)->next   = NULL;
                        (*__cftree_cur_e_ptr)->value  = NULL;
                        (*__cftree_cur_e_ptr)->key    = eris_string_clone( __petree_move_e->key);
                        if ( (*__cftree_cur_e_ptr)->key) {
                            /** To do make child */
                            if ( __petree_move_e->child) {
                                if (  ERIS_PETREE_STRING == (__petree_move_e->child)->type) {
                                    rc = eris_cftree_set_e( (*__cftree_cur_e_ptr), &(*__cftree_cur_e_ptr)->child, __petree_move_e, __petree_move_e->child, __value);
                                } else {
                                    rc = eris_cftree_set_e( __cftree_parent_e, &(*__cftree_cur_e_ptr)->next, __petree_move_e, __petree_move_e->child, __value);
                                }
                            } else {
                                if ( __value) {
                                    (*__cftree_cur_e_ptr)->value = eris_string_clone( __value);
                                    if ( !(*__cftree_cur_e_ptr)->value) {
                                        /** Clone value failed */
                                        rc = EERIS_ERROR;
                                    }
                                }
                            }
                        } else { rc = EERIS_ERROR; }
                    } else { rc = EERIS_ERROR; }
                }
            } else {
                /** Is key index number */
                eris_bool_t not_found = true;
                eris_int_t tmp_index  = __petree_move_e->index;
                if ( 0 <= tmp_index) {
                    /** Goto index element */
                    do {
                        if ( eris_string_isequal( (*__cftree_cur_e_ptr)->key, __petree_parent_e->key, false)) {
                            tmp_index--;
                        }

                        if ( 0 > tmp_index) { 
                            not_found = false; break; 
                        } else { 
                            __cftree_cur_e_ptr = &(*__cftree_cur_e_ptr)->next; 
                        }

                    } while ( *__cftree_cur_e_ptr);
                } else {
                    /** Goto tail of equal move element key value */
                    do {
                        if ( eris_string_isequal( (*__cftree_cur_e_ptr)->key, __petree_parent_e->key, false)) {
                            not_found = false;

                            break;
                        } else { __cftree_cur_e_ptr = &(*__cftree_cur_e_ptr)->next; }
                    } while ( *__cftree_cur_e_ptr);
                }

                /** Not existed */
                if ( not_found) {
                    /** Create a new node */
                    *__cftree_cur_e_ptr = (eris_cftree_e_t *)eris_memory_alloc( sizeof( eris_cftree_e_t));
                    if ( *__cftree_cur_e_ptr) {
                        (*__cftree_cur_e_ptr)->parent = __cftree_parent_e;
                        (*__cftree_cur_e_ptr)->child  = NULL;
                        (*__cftree_cur_e_ptr)->next   = NULL;
                        (*__cftree_cur_e_ptr)->value  = NULL;
                        (*__cftree_cur_e_ptr)->key    = eris_string_clone( __petree_parent_e->key);
                        if ( (*__cftree_cur_e_ptr)->key) {
                            rc = 0;
                        } else { rc = EERIS_ERROR; }
                    } else { rc = EERIS_ERROR; }
                }

                /** Match ok, and goto next */
                if ( 0 == rc) {
                    /** To do make child */
                    if ( __petree_move_e->child) {
                        if (  ERIS_PETREE_STRING == (__petree_move_e->child)->type) {
                            rc = eris_cftree_set_e( (*__cftree_cur_e_ptr), &(*__cftree_cur_e_ptr)->child, __petree_move_e, __petree_move_e->child, __value);
                        } else {
                            rc = eris_cftree_set_e( __cftree_parent_e, &(*__cftree_cur_e_ptr)->next, __petree_move_e, __petree_move_e->child, __value);
                        }
                    } else {
                        if ( __value) {
                            (*__cftree_cur_e_ptr)->value = eris_string_clone( __value);
                            if ( !(*__cftree_cur_e_ptr)->value) {
                                /** Clone value failed */
                                rc = EERIS_ERROR;
                            }
                        }
                    }
                }
            }
        } else {
            if ( ERIS_PETREE_STRING == __petree_move_e->type) {
                /** Create a new node */
                *__cftree_cur_e_ptr = (eris_cftree_e_t *)eris_memory_alloc( sizeof( eris_cftree_e_t));
                if ( *__cftree_cur_e_ptr) {
                    (*__cftree_cur_e_ptr)->parent = __cftree_parent_e;
                    (*__cftree_cur_e_ptr)->child  = NULL;
                    (*__cftree_cur_e_ptr)->next   = NULL;
                    (*__cftree_cur_e_ptr)->value  = NULL;
                    (*__cftree_cur_e_ptr)->key    = eris_string_clone( __petree_move_e->key);
                    if ( (*__cftree_cur_e_ptr)->key) {
                        /** To do make child */
                        if ( __petree_move_e->child) {
                            if (  ERIS_PETREE_STRING == (__petree_move_e->child)->type) {
                                rc = eris_cftree_set_e( (*__cftree_cur_e_ptr), &(*__cftree_cur_e_ptr)->child, __petree_move_e, __petree_move_e->child, __value);
                            } else {
                                rc = eris_cftree_set_e( __cftree_parent_e, &(*__cftree_cur_e_ptr)->next, __petree_move_e, __petree_move_e->child, __value);
                            }
                        } else {
                            if ( __value) {
                                (*__cftree_cur_e_ptr)->value = eris_string_clone( __value);
                                if ( !(*__cftree_cur_e_ptr)->value) {
                                    /** Clone value failed */
                                    rc = EERIS_ERROR;
                                }
                            }
                        }
                    } else { rc = EERIS_ERROR; }
                } else { rc = EERIS_ERROR; }
            } else {
                /** Make 1-vector */
                eris_int_t tmp_index = __petree_move_e->index;
                if ( 0 > tmp_index ) {
                    /** Create a new node */
                    *__cftree_cur_e_ptr = (eris_cftree_e_t *)eris_memory_alloc( sizeof( eris_cftree_e_t));
                    if ( *__cftree_cur_e_ptr) {
                        (*__cftree_cur_e_ptr)->parent = __cftree_parent_e;
                        (*__cftree_cur_e_ptr)->child  = NULL;
                        (*__cftree_cur_e_ptr)->next   = NULL;
                        (*__cftree_cur_e_ptr)->value  = NULL;
                        (*__cftree_cur_e_ptr)->key    = eris_string_clone( __petree_parent_e->key);
                        if ( (*__cftree_cur_e_ptr)->key) {
                            /** To do make child */
                            if ( __petree_move_e->child) {
                                if (  ERIS_PETREE_STRING == (__petree_move_e->child)->type) {
                                    rc = eris_cftree_set_e( (*__cftree_cur_e_ptr), &(*__cftree_cur_e_ptr)->child, __petree_move_e, __petree_move_e->child, __value);
                                } else {
                                    rc = eris_cftree_set_e( __cftree_parent_e, &(*__cftree_cur_e_ptr)->next, __petree_move_e, __petree_move_e->child, __value);
                                }
                            } else {
                                if ( __value) {
                                    (*__cftree_cur_e_ptr)->value = eris_string_clone( __value);
                                    if ( !(*__cftree_cur_e_ptr)->value) {
                                        /** Clone value failed */
                                        rc = EERIS_ERROR;
                                    }
                                }
                            }
                        } else { rc = EERIS_ERROR; }
                    } else { rc = EERIS_ERROR; }
                } else {
                    /** Make index + 1 count elements */
                    do {
                        tmp_index--;

                        /** Create a new node */
                        *__cftree_cur_e_ptr = (eris_cftree_e_t *)eris_memory_alloc( sizeof( eris_cftree_e_t));
                        if ( *__cftree_cur_e_ptr) {
                            (*__cftree_cur_e_ptr)->parent = __cftree_parent_e;
                            (*__cftree_cur_e_ptr)->child  = NULL;
                            (*__cftree_cur_e_ptr)->next   = NULL;
                            (*__cftree_cur_e_ptr)->value  = NULL;
                            (*__cftree_cur_e_ptr)->key    = eris_string_clone( __petree_parent_e->key);
                            if ( (*__cftree_cur_e_ptr)->key) {
                                /** To do make child */
                                if ( !(__petree_move_e->child)) {
                                    if ( __value) {
                                        (*__cftree_cur_e_ptr)->value = eris_string_clone( __value);
                                        if ( !(*__cftree_cur_e_ptr)->value) {
                                            /** Clone value failed */
                                            rc = EERIS_ERROR;
                                        }
                                    }
                                }

                            } else { rc = EERIS_ERROR; break; }
                        } else { rc = EERIS_ERROR; break; }

                        /** index >= 1 */
                        if ( (0 == rc) && (0 < tmp_index) ) {
                            __cftree_cur_e_ptr = &(*__cftree_cur_e_ptr)->next;
                        } else { break; }
                    } while ( (0 == rc) && (0 <= tmp_index));


                    /** Make 1-vector nodes ok? */
                    if ( 0 == rc) {
                        if ( __petree_move_e->child) {
                            if (  ERIS_PETREE_STRING == (__petree_move_e->child)->type) {
                                rc = eris_cftree_set_e( (*__cftree_cur_e_ptr), &(*__cftree_cur_e_ptr)->child, __petree_move_e, __petree_move_e->child, __value);
                            } else {
                                rc = eris_cftree_set_e( __cftree_parent_e, &(*__cftree_cur_e_ptr)->next, __petree_move_e, __petree_move_e->child, __value);
                            }
                        }
                    }
                }
            }/// Make 1-vector
        }/// else __petree_move_e->type is number
    }/// else create new nodes 

    return rc;
}/// eris_cftree_set_e


/**
 * @Brief: Get element counts with specify key path of the cftree.
 *
 * @Param: __cftree_cur_e,    The current element node of each.
 * @Param: __petree_parent_e, The petree move element node parent.
 * @Param: __petree_move_e,   The petree move current node of each.
 *
 * @Return: The count of specify key path.
 **/
static eris_int_t eris_cftree_esize_e( eris_cftree_e_t *__cftree_cur_e, eris_petree_e_t *__petree_parent_e, eris_petree_e_t *__petree_move_e)
{
    eris_int_t rc_esize = 0;

    if ( __cftree_cur_e && __petree_move_e) {
        if ( ERIS_PETREE_STRING == __petree_move_e->type) {
            /** Is key string */
            eris_cftree_e_t *tmp_cftree_e = __cftree_cur_e;

            do {
                if ( eris_string_isequal( tmp_cftree_e->key, __petree_move_e->key, false) ) {
                    /** Goto next and match */
                    if ( ( __petree_move_e->child) && 
                         ( ERIS_PETREE_STRING == (__petree_move_e->child)->type)
                     ) {
                        /** Match child */
                        rc_esize = eris_cftree_esize_e( tmp_cftree_e->child, __petree_move_e, __petree_move_e->child);

                    } else {
                        if ( __petree_move_e->child) {
                            /** Match petree child index */
                            rc_esize = eris_cftree_esize_e( tmp_cftree_e, __petree_move_e, __petree_move_e->child);

                        } else {
                            /** A ha!!! */
                            rc_esize++;

                            tmp_cftree_e = tmp_cftree_e->next;

                            while ( tmp_cftree_e ) {
                                if ( eris_string_isequal( tmp_cftree_e->key, __petree_move_e->key, false) ) {
                                    /** Found other one */
                                    rc_esize++;
                                }

                                tmp_cftree_e = tmp_cftree_e->next;
                            }
                        }
                    }

                    /** Out of while */
                    break;
                } else { tmp_cftree_e = tmp_cftree_e->next; }
            } while ( tmp_cftree_e);
        } else {
            /** Is key index number */
            eris_cftree_e_t *pre_cftree_e = NULL;
            eris_cftree_e_t *tmp_cftree_e = __cftree_cur_e;

            eris_int_t tmp_index = __petree_move_e->index;
            if ( 0 <= tmp_index) {
                /** Goto index element */
                do {
                    if ( eris_string_isequal( tmp_cftree_e->key, __petree_parent_e->key, false)) {
                        tmp_index--;
                    }

                    if ( 0 > tmp_index) { break; }
                    else { tmp_cftree_e = tmp_cftree_e->next; }
                } while ( tmp_cftree_e);

                pre_cftree_e = tmp_cftree_e;
            } else {
                /** Goto tail of equal move element key value */
                do {
                    if ( eris_string_isequal( tmp_cftree_e->key, __petree_parent_e->key, false)) {
                        pre_cftree_e = tmp_cftree_e;
                    }

                    tmp_cftree_e = tmp_cftree_e->next;
                } while ( tmp_cftree_e);
            }

            /** Match ok, and goto next */
            if ( pre_cftree_e) {
                if ( ( __petree_move_e->child) && 
                     ( ERIS_PETREE_STRING == (__petree_move_e->child)->type)
                ) {
                    /** Match to child */
                    rc_esize = eris_cftree_esize_e( pre_cftree_e->child, __petree_move_e, __petree_move_e->child);
                } else {
                    if ( __petree_move_e->child) {
                        /** Match child */
                        rc_esize = eris_cftree_esize_e( pre_cftree_e, __petree_move_e, __petree_move_e->child);

                    } else {
                        /** A ha!!! */
                        rc_esize++;
                    }
                }
            }
        }
    }

    return rc_esize;
}/// eris_cftree_esize_e


/**
 * @Brief: Check petree is existed of the cftree.
 *
 * @Param: __cftree_cur_e,    The current element node of each.
 * @Param: __petree_parent_e, The petree move element node parent.
 * @Param: __petree_move_e,   The petree move current node of each.
 *
 * @Return: If existed is true, other is false.
 **/
static eris_bool_t eris_cftree_existed_e( eris_cftree_e_t *__cftree_cur_e, eris_petree_e_t *__petree_parent_e, eris_petree_e_t *__petree_move_e)
{
    eris_bool_t rc_existed_e = false;

    if ( __cftree_cur_e && __petree_move_e) {
        if ( ERIS_PETREE_STRING == __petree_move_e->type) {
            /** Is key string */
            eris_cftree_e_t *tmp_cftree_e = __cftree_cur_e;

            do {
                if ( eris_string_isequal( tmp_cftree_e->key, __petree_move_e->key, false) ) {
                    /** Goto next and match */
                    if ( ( __petree_move_e->child) && 
                         ( ERIS_PETREE_STRING == (__petree_move_e->child)->type)
                     ) {
                        rc_existed_e = eris_cftree_existed_e( tmp_cftree_e->child, __petree_move_e, __petree_move_e->child);
                    } else {
                        rc_existed_e = eris_cftree_existed_e( tmp_cftree_e, __petree_move_e, __petree_move_e->child);
                    }

                    break;
                } else { tmp_cftree_e = tmp_cftree_e->next; }
            } while ( tmp_cftree_e);
        } else {
            /** Is key index number */
            eris_cftree_e_t *pre_cftree_e = NULL;
            eris_cftree_e_t *tmp_cftree_e = __cftree_cur_e;

            eris_int_t tmp_index = __petree_move_e->index;
            if ( 0 <= tmp_index) {
                /** Goto index element */
                do {
                    if ( eris_string_isequal( tmp_cftree_e->key, __petree_parent_e->key, false)) {
                        tmp_index--;
                    }

                    if ( 0 > tmp_index) { break; }
                    else { tmp_cftree_e = tmp_cftree_e->next; }
                } while ( tmp_cftree_e);

                pre_cftree_e = tmp_cftree_e;
            } else {
                /** Goto tail of equal move element key value */
                do {
                    if ( eris_string_isequal( tmp_cftree_e->key, __petree_parent_e->key, false)) {
                        pre_cftree_e = tmp_cftree_e;
                    }

                    tmp_cftree_e = tmp_cftree_e->next;
                } while ( tmp_cftree_e);
            }

            /** Match ok, and goto next */
            if ( pre_cftree_e) {
                if ( ( __petree_move_e->child) && 
                     ( ERIS_PETREE_STRING == (__petree_move_e->child)->type)
                ) {
                    rc_existed_e = eris_cftree_existed_e( pre_cftree_e->child, __petree_move_e, __petree_move_e->child);
                } else {
                    rc_existed_e = eris_cftree_existed_e( pre_cftree_e,  __petree_move_e, __petree_move_e->child);
                }
            }
        }
    } else {
        /** The element is existed */
        if ( __cftree_cur_e) {
            rc_existed_e = true;
        } else {
            if ( !__cftree_cur_e && !__petree_move_e) {
                rc_existed_e = true;
            }
        }
    }

    return rc_existed_e;
}/// eris_cftree_existed_e


