/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This design to parser the path string, eg:
 **        :    http.max_connections
 **        :    http.service[1].name
 **        : and so on.
 **
 ******************************************************************************/

#include "core/eris.core.h"


/**
 * @Brief: Clear-init.
 *
 * @Param: __petree, The path element tree context.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_petree_init( eris_petree_t *__petree)
{
    if ( __petree) {
        __petree->count = 0;
        __petree->level = 0;
        __petree->head  = NULL;
    }
}/// eris_petree_cleanup


/**
 * @Brief: Parser the path element string, eg: http.backlog.
 *
 * @Param: __petree, The path element tree context.
 * @Param: __pe_str, The path element string.
 * @Param: __errno,  If happen error and set it.
 *
 * @Return: Ok->0, other return EERIS_ERROR, and set errno: (EERIS_INVALID | EERIS_DATA | EERIS_MEMORY).
 **/
eris_int_t eris_petree_parser( eris_petree_t *__petree, const eris_char_t *__pe_str, eris_errno_t *__errno)
{
    eris_int_t rc = 0;

    if ( __petree) {
        /** Check the input path string, eg: http.backlog */
        if ( __pe_str ) {
            /** Clean up old */
            (eris_void_t )eris_petree_cleanup( __petree);

            eris_petree_e_t **current_pos = &(__petree->head);

            enum {
                ERIS_PESTATE_START = 0,
                ERIS_PESTATE_KEY      ,
                ERIS_PESTATE_DOT      ,
                ERIS_PESTATE_BRACKET_L,
                ERIS_PESTATE_BRACKET_R,
                ERIS_PESTATE_FINISH   ,
            } eris_pestate_v = ERIS_PESTATE_START;

            eris_bool_t back_flag  = false;
            eris_bool_t index_star_flag = false;
            eris_int_t  key_size   = 0;
            eris_int_t  index_size = 0;
            eris_char_t index[32]  = {0};
            eris_char_t key[ ERIS_PETREE_KSIZE] = {0};

            /** Do parser the path string */
            const eris_char_t *s = __pe_str;

            while ( *s ) {
                eris_char_t c = *s++;

                /** Match all state */
                switch ( eris_pestate_v) {
                    case ERIS_PESTATE_START :
                        {
                            if ( '.' == c) {
                                /** Dot */
                                eris_pestate_v = ERIS_PESTATE_DOT;
                            } else if ( '[' == c) {
                                /** The left bracket */
                                eris_pestate_v = ERIS_PESTATE_BRACKET_L;
                            } else if ( ']' == c) {
                                /** Error */
                                rc = EERIS_ERROR;

                                __errno ? (*__errno = EERIS_DATA) : 0;
                            } else if ( ('_' == c) || ('-' == c) || isalnum( c)) {
                                key[ key_size++] = c;

                                eris_pestate_v = ERIS_PESTATE_KEY;
                            } else if ( (' ' == c) || ('\t' == c)) {
                                /** Nothing */
                            } else {
                                rc = EERIS_ERROR; __errno ? (*__errno = EERIS_DATA) : 0;

                                eris_pestate_v = ERIS_PESTATE_FINISH;
                            }
                        } break;
                    case ERIS_PESTATE_KEY   :
                        {
                            if ( '.' == c) {
                                /** Create new node */
                                if ( 0 < key_size) {
                                    *current_pos = (eris_petree_e_t *)eris_memory_alloc( sizeof( eris_petree_e_t) + key_size + 1);
                                    if ( *current_pos) {
                                        (*current_pos)->type  = ERIS_PETREE_STRING;
                                        (*current_pos)->index = 0;
                                        (*current_pos)->child = NULL;

                                        memcpy( (*current_pos)->key, key, key_size);
                                        (*current_pos)->key[ key_size] = '\0';

                                        /** Position to child */
                                        current_pos = &(*current_pos)->child;
                                        __petree->count++;
                                        __petree->level++;
                                    } else {
                                        rc = EERIS_ERROR; __errno ? (*__errno = EERIS_ALLOC) : 0;

                                        eris_pestate_v = ERIS_PESTATE_FINISH;
                                    }

                                    back_flag = false;
                                    key_size  = 0;
                                    memset( key, 0x0, sizeof( key));
                                }

                                /** Dot */
                                eris_pestate_v = ERIS_PESTATE_DOT;
                            } else if ( '[' == c) {
                                /** Create new node */
                                if ( 0 < key_size) {
                                    *current_pos = (eris_petree_e_t *)eris_memory_alloc( sizeof( eris_petree_e_t) + key_size + 1);
                                    if ( *current_pos) {
                                        (*current_pos)->type  = ERIS_PETREE_STRING;
                                        (*current_pos)->index = 0;
                                        (*current_pos)->child = NULL;

                                        memcpy( (*current_pos)->key, key, key_size);
                                        (*current_pos)->key[ key_size] = '\0';

                                        /** Position to child */
                                        current_pos = &(*current_pos)->child;
                                        __petree->count++;
                                        __petree->level++;
                                    }

                                    back_flag = false;
                                    key_size  = 0;
                                    memset( key, 0x0, sizeof( key));
                                } else {
                                    rc = EERIS_ERROR; __errno ? (*__errno = EERIS_ALLOC) : 0;

                                    eris_pestate_v = ERIS_PESTATE_FINISH;
                                }

                                /** The left bracket */
                                eris_pestate_v = ERIS_PESTATE_BRACKET_L;

                            } else if ( ']' == c) {
                                /** Error */
                                rc = EERIS_ERROR; __errno ? (*__errno = EERIS_DATA) : 0;

                                eris_pestate_v = ERIS_PESTATE_FINISH;
                            } else if ( ('_' == c) || ('-' == c) || isalnum( c)) {
                                if ( !back_flag ) {
                                    if ( key_size < (sizeof( key) - 1)) {
                                        key[ key_size++] = c;
                                    }
                                } else {
                                    rc = EERIS_ERROR; __errno ? (*__errno = EERIS_DATA) : 0;

                                    eris_pestate_v = ERIS_PESTATE_FINISH;
                                }
                            } else if ( (' ' == c) || ('\t' == c)) {
                                if ( 0 < key_size) { 
                                    back_flag = true; 
                                }
                            } else {
                                rc = EERIS_ERROR; __errno ? (*__errno = EERIS_DATA) : 0;

                                eris_pestate_v = ERIS_PESTATE_FINISH;
                            }
                        } break;
                    case ERIS_PESTATE_DOT   :
                        {
                            if ( '.' == c) {
                                eris_pestate_v = ERIS_PESTATE_DOT;

                            } else if ( ('_' == c) || ('-' == c) || isalnum( c)) {
                                if ( key_size < (sizeof( key) - 1)) {
                                    key[ key_size++] = c;
                                }

                                eris_pestate_v = ERIS_PESTATE_KEY;
                            } else if ( (' ' == c) || ('\t' == c)) {
                                /** Nothing */
                            } else {
                                rc = EERIS_ERROR; __errno ? (*__errno = EERIS_DATA) : 0;

                                eris_pestate_v = ERIS_PESTATE_FINISH;
                            }
                        } break;
                    case ERIS_PESTATE_BRACKET_L:
                        {
                            if ( isdigit( c)) {
                                if ( index_size < (sizeof( index) - 1)) {
                                    index[ index_size++] = c;
                                }
                            } else if ( ']' == c) {
                                if ( index_star_flag) {
                                    if ( 0 == index_size) {
                                        *current_pos = (eris_petree_e_t *)eris_memory_alloc( sizeof( eris_petree_e_t));
                                        if ( *current_pos) {
                                            (*current_pos)->type  = ERIS_PETREE_INTEGER;
                                            (*current_pos)->index = EERIS_ERROR;
                                            (*current_pos)->child = NULL;

                                            /** Position to child */
                                            current_pos = &(*current_pos)->child;
                                            __petree->count++;
                                            __petree->level++;
                                        } else {
                                            rc = EERIS_ERROR; __errno ? (*__errno = EERIS_ALLOC) : 0;

                                            eris_pestate_v = ERIS_PESTATE_FINISH;
                                        }
                                    } else {
                                        /** Error */
                                        rc = EERIS_ERROR; __errno ? (*__errno = EERIS_DATA) : 0;

                                        eris_pestate_v = ERIS_PESTATE_FINISH;
                                    }

                                    index_star_flag = false;
                                } else {
                                    /** Is digit index */
                                    *current_pos = (eris_petree_e_t *)eris_memory_alloc( sizeof( eris_petree_e_t));
                                    if ( *current_pos) {
                                        (*current_pos)->type  = ERIS_PETREE_INTEGER;
                                        (*current_pos)->index = atoi( index);
                                        (*current_pos)->child = NULL;

                                        /** Position to child */
                                        current_pos = &(*current_pos)->child;
                                        __petree->count++;
                                        __petree->level++;
                                    } else {
                                        rc = EERIS_ERROR; __errno ? (*__errno = EERIS_ALLOC) : 0;

                                        eris_pestate_v = ERIS_PESTATE_FINISH;
                                    }

                                    index_size = 0;
                                    memset( index, 0x0, sizeof( index));
                                }

                                back_flag = false;
                                eris_pestate_v = ERIS_PESTATE_BRACKET_R;

                            } else if ( '*' == c) {
                                /** Is star index */
                                index_star_flag = true;

                            } else if ( (' ' == c) || ('\t' == c)) {
                                if ( 0 < index_size ) {
                                    back_flag = true;
                                }
                            } else {
                                if ( (0 == index_size) && (('-' == c) || ('+' == c))) {
                                    index[ index_size++] = c;
                                } else {
                                    rc = EERIS_ERROR; __errno ? (*__errno = EERIS_DATA) : 0;

                                    eris_pestate_v = ERIS_PESTATE_FINISH;
                                }
                            }
                        } break;
                    case ERIS_PESTATE_BRACKET_R:
                        {
                            if ( (' ' == c) || ('\t' == c)) {
                                /** Nothing */
                            } else if ( '.' == c) {
                                /** Goto dot state */
                                eris_pestate_v = ERIS_PESTATE_DOT;

                            } else {
                                rc = EERIS_ERROR; __errno ? (*__errno = EERIS_DATA) : 0;

                                eris_pestate_v = ERIS_PESTATE_FINISH;
                            }
                        } break;
                    case ERIS_PESTATE_FINISH :
                    default : break;
                }/// switch( c)

                if ( ERIS_PESTATE_FINISH == eris_pestate_v) { break; }
            }/// while ( *s)

            /** Create new node */
            if ( (0 == rc) && (0 < key_size)) {
                *current_pos = (eris_petree_e_t *)eris_memory_alloc( sizeof( eris_petree_e_t) + key_size + 1);
                if ( *current_pos) {
                    (*current_pos)->type  = ERIS_PETREE_STRING;
                    (*current_pos)->index = 0;
                    (*current_pos)->child = NULL;

                    memcpy( (*current_pos)->key, key, key_size);
                    (*current_pos)->key[ key_size] = '\0';

                    /** Position to child */
                    current_pos = &(*current_pos)->child;
                    __petree->count++;
                    __petree->level++;
                }

                key_size = 0;
                memset( key, 0x0, sizeof( key));
            }

        } else { rc = EERIS_ERROR; __errno ? (*__errno = EERIS_DATA) : 0; }
    } else { rc = EERIS_ERROR; __errno ? (*__errno = EERIS_INVALID) : 0; }

    /** Happen error */
    if ( 0 != rc) { (eris_void_t )eris_petree_cleanup( __petree); }

    return rc;
}/// eris_petree_parser


/**
 * @Brief: Dump the petree to stdout output.
 *
 * @Param: __petree, The petree context.
 * @Param: __result, The dump result output, When dit not use, please free it.
 * @Param: __errno,  If happen error and set error code, (EERIS_INVALID | EERIS_ALLOC).
 *
 * @Return: Nothing.
 **/
eris_int_t eris_petree_dump( eris_petree_t *__petree, eris_char_t **__result, eris_errno_t *__errno)
{
    eris_int_t rc = 0;

    if ( __petree && __result) {
        eris_string_t tmp_es = eris_string_alloc( 128);
        if ( tmp_es) {
            eris_petree_e_t *current_e = __petree->head;
        
            /** From begin to end element */
            for ( ; current_e; current_e = current_e->child) {
                if ( ERIS_PETREE_STRING == current_e->type) {
                    /** Append key */
                    rc = eris_string_append( &tmp_es, current_e->key);
                    if ( tmp_es) {
                        if ( current_e->child && (ERIS_PETREE_STRING == current_e->child->type)) {
                            rc = eris_string_append_c( &tmp_es, '.');
                        }
                    }
                    
                    /** Append error */
                    if ( 0 != rc) { __errno ? (*__errno = EERIS_ALLOC): 0; break; }
                } else if ( ERIS_PETREE_INTEGER == current_e->type) {
                    /** Is index */
                    if ( 0 <= current_e->index) {
                        rc = eris_string_printf_append( &tmp_es, "[%i]", current_e->index);

                        if ( 0 != rc) { __errno ? (*__errno = EERIS_ALLOC): 0; break; }
                    } else {
                        /** Append [*] string */
                        rc = eris_string_append( &tmp_es, "[*]");

                        if ( 0 != rc) { __errno ? (*__errno = EERIS_ALLOC): 0; break; }
                    }

                    if ( current_e->child && (ERIS_PETREE_STRING == current_e->child->type)) {
                        rc = eris_string_append_c( &tmp_es, '.');
                    }

                    /** Alloc error and break */
                    if ( !tmp_es) { __errno ? (*__errno = EERIS_ALLOC): 0; break; }
                } else { /** Nothing */ }
            }/// for ( ; current_e; current_e = current_e->child)

            /** Happen error */
            if ( 0 == rc) { 
                eris_size_t tmp_es_size = eris_string_size( tmp_es);
                *__result = (eris_char_t *)eris_memory_alloc( tmp_es_size + 1);

                memcpy( *__result, tmp_es, tmp_es_size);
                (*__result)[ tmp_es_size] = '\0';
            }
            
            /** Release tmp eris string */
            if ( tmp_es) { (eris_void_t )eris_string_free( tmp_es); tmp_es = NULL; }
        /** Happen alloced error */
        } else { rc = EERIS_ERROR; __errno ? (*__errno = EERIS_ALLOC): 0; }
    } else { rc = EERIS_ERROR; __errno ? (*__errno = EERIS_INVALID) : 0; }

    return rc;
}/// eris_petree_dump


/**
 * @Brief: Dump the petree to stdout output.
 *
 * @Param: __petree, The petree context.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_petree_dumpout( eris_petree_t *__petree)
{
    if ( __petree) {
        eris_petree_e_t *current_e = __petree->head;
        
        /** From begin to end element */
        for ( ; current_e; current_e = current_e->child) {
            if ( ERIS_PETREE_STRING == current_e->type) {
                /** Key */
                printf( "%s", current_e->key);

                if ( current_e->child && (ERIS_PETREE_STRING == current_e->child->type) ) {
                    printf( ".");
                }
            } else if ( ERIS_PETREE_INTEGER == current_e->type) {
                /** index */
                if ( 0 <= current_e->index) {
                    printf( "[%d]", current_e->index);
                } else {
                    printf( "[*]");
                }

                if ( current_e->child && (ERIS_PETREE_STRING == current_e->child->type) ) {
                    printf( ".");
                }
            } else {
                /** Key */
                printf( "%s", current_e->key);

                if ( current_e->child && (ERIS_PETREE_STRING == current_e->child->type) ) {
                    printf( ".");
                }
            }
        }/// for ( ; current_e; current_e = current_e->next)

        /** Flush */
        printf( "\n");
    }/// fi ( __petree)
}/// eris_petree_dumpout


/**
 * @Brief: Clean up the petree.
 *
 * @Param: __petree, The petree context.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_petree_cleanup( eris_petree_t *__petree)
{
    if ( __petree) {
        __petree->count = 0;
        __petree->level = 0;

        /** Clean up all */
        eris_petree_e_t *cleanup_e = __petree->head;
        while ( cleanup_e) {
            __petree->head = cleanup_e->child;

            /** Free element */
            eris_memory_free( cleanup_e);

            cleanup_e = __petree->head;
        }
    }
}/// eris_petree_cleanup


/**
 * @Brief: Init-iter
 *
 * @Param: __petree, The petree context.
 * @Param: __iter, Pointer to iter objects.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_petree_iter( const eris_petree_t *__petree, eris_petree_iter_t *__iter)
{
    if ( __petree && __iter) {
        __iter->pos     = 0;
        __iter->current = __petree->head;
    } else {
        if ( __iter) {
            __iter->pos     = 0;
            __iter->current = NULL;
        }
    }
}/// eris_petree_iter


/**
 * @Brief: Used iter goto next.
 *
 * @Param: __iter, Pointer to iter objects.
 *
 * @Return: The current element node.
 **/
eris_petree_e_t *eris_petree_next( eris_petree_iter_t *__iter)
{
    eris_petree_e_t *current_e_ptr = NULL;

    if ( __iter && __iter->current) {
        current_e_ptr = __iter->current;

        __iter->current = current_e_ptr->child;
        __iter->pos++;
    }

    return current_e_ptr;
}/// eris_petree_next


/**
 * @Brief: Reset-iter
 *
 * @Param: __petree, The petree context.
 * @Param: __iter, Pointer to iter objects.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_petree_reset( const eris_petree_t *__petree, eris_petree_iter_t *__iter)
{
    if ( __petree && __iter) {
        __iter->pos     = 0;
        __iter->current = __petree->head;
    } else {
        if ( __iter) {
            __iter->pos     = 0;
            __iter->current = NULL;
        }
    }
}/// eris_petree_reset


/**
 * @Brief: Destroy the petree.
 *
 * @Param: __petree, The petree context.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_petree_destroy( eris_petree_t *__petree)
{
    if ( __petree) {
        __petree->count = 0;
        __petree->level = 0;

        /** Clean up all */
        eris_petree_e_t *cleanup_e = __petree->head;
        while ( cleanup_e) {
            __petree->head = cleanup_e->child;

            /** Free element */
            eris_memory_free( cleanup_e);

            cleanup_e = __petree->head;
        }

        __petree->head = NULL;
    }
}/// eris_petree_destroy


