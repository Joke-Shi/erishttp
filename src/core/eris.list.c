/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This is list of erishttp project, design apis eg:
 **        ::    init
 **        ::    size
 **        ::    push
 **        ::    push_back
 **        ::    append
 **        ::    pop
 **        ::    pop_back
 **        ::    find
 **        ::    rfind
 **        ::    erase
 **        ::    lock
 **        ::    unlock
 **        ::    cleanup
 **        ::    destroy
 **           And so on...
 **
 ******************************************************************************/

#include "core/eris.core.h"


/** Create list node. */
static eris_list_node_t *eris_list_create_node( eris_list_t *__list);

/** Release list node. */
static eris_none_t eris_list_delete_node( eris_list_t *__list, eris_list_node_t **__pnode);



/**
 * @Brief: Init eris list context.
 *
 * @Param: __list, Eris list context.
 * @Param: __enable_prio, Enable priority set true, default is false.
 * @Param: __log,  Dump log context.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_list_init( eris_list_t *__list, eris_bool_t __enable_prio, eris_log_t *__log)
{
    eris_int_t rc = 0;

    if ( __list) {
        /** Init */
        {
            __list->lock        = 0;
            __list->size        = 0;
            __list->pid         = eris_get_pid();
            __list->nprocs      = eris_get_nprocessor();
            __list->enable_prio = __enable_prio;
            __list->head        = NULL;
            __list->tail        = NULL;
            __list->log         = __log;
        }
    } else {
        rc = EERIS_ERROR;

        if ( __log) {
            eris_log_dump( __log, ERIS_LOG_CORE, "Input list context is invalid");
        }
    }

    return rc;
}/// eris_list_init


/**
 * @Brief: Get list elements count.
 *
 * @Param: __list, Eris list context.
 * 
 * @Return: size of eris list.
 **/
eris_size_t eris_list_size( const eris_list_t *__list)
{
    if ( __list) {
        /** List size */
        return (__list->size);
    }

    return 0;
}/// eris_list_size


/**
 * @Brief: Push data into list, if enable priority and compare of priority.
 *
 * @Param: __list, Eris list context.
 * @Param: __data, Push source data pointer.
 * @Param: __prio, Enable and using.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_list_push( eris_list_t *__list, eris_void_t *__data, eris_int_t __prio)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __list) {
        if ( __data) {
            eris_list_node_t *new_node = eris_list_create_node( __list);
            if ( new_node) {
                /** Set data */
                new_node->data = __data;

                if ( __list->enable_prio) {
                    new_node->prio = __prio;

                    /** Frist element node */
                    if ( NULL == __list->head) {
                        __list->head = new_node;
                        __list->tail = new_node;

                    } else {
                        eris_list_node_t *cur_node = __list->head;

                        /** Compare prio */
                        do {
                            if ( cur_node->prio < __prio) {
                                break;

                            } else {
                                cur_node = cur_node->next;
                            }
                        } while ( cur_node);

                        if ( cur_node) {
                            /** prev <--> new <--> current */
                            new_node->prev = cur_node->prev;

                            cur_node->prev ? (cur_node->prev->next = new_node) : 0;
                            cur_node->prev = new_node;

                            new_node->next = cur_node;

                            /** If current is head and reset it */
                            if ( cur_node == __list->head) {
                                __list->head = new_node;
                            }
                        } else {
                            new_node->prev     = __list->tail;
                            __list->tail->next = new_node;
                            __list->tail       = new_node;
                        }
                    }

                    /** Ok */
                    __list->size++;

                } else {
                    new_node->next = __list->head;
                    __list->head ? (__list->head->prev = new_node) : 0;
                    __list->head   = new_node;

                    if ( 0 == __list->size) {
                        __list->tail = new_node;
                    }

                    /** Ok */
                    __list->size++;
                }
            } else {
                rc = EERIS_ERROR;

                if ( __list->log) {
                    eris_log_dump( __list->log, ERIS_LOG_CORE, "Create node failed, errno.<%d>", errno);
                }
            }
        }/// else nothing
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_list_push


/**
 * @Brief: Push data into list back, if enable priority and compare of priority.
 *
 * @Param: __list, Eris list context.
 * @Param: __data, Push source data pointer.
 * @Param: __prio, Enable and using.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_list_push_back( eris_list_t *__list, eris_void_t *__data, eris_int_t __prio)
{
    eris_int_t rc = 0;

    if ( __list) {
        if ( __data) {
            eris_list_node_t *new_node = eris_list_create_node( __list);
            if ( new_node) {
                /** Set data */
                new_node->data = __data;

                if ( __list->enable_prio) {
                    new_node->prio = __prio;

                    /** Frist element node */
                    if ( 0 == __list->size) {
                        __list->head = new_node;
                        __list->tail = new_node;

                    } else {
                        eris_list_node_t *cur_node = __list->tail;

                        /** Compare prio */
                        do {
                            if ( cur_node->prio >= __prio) {
                                break;

                            } else {
                                cur_node = cur_node->prev;
                            }
                        } while ( cur_node);

                        if ( cur_node) {
                            /** current <--> new <--> next */
                            new_node->next = cur_node->next;

                            cur_node->next ? (cur_node->next->prev = new_node) : 0;
                            cur_node->next = new_node;

                            new_node->prev = cur_node;

                            /** If current is head and reset it */
                            if ( cur_node == __list->tail) {
                                __list->tail = new_node;
                            }
                        } else {
                            new_node->next     = __list->head;
                            __list->head->prev = new_node;
                            __list->head       = new_node;
                        }
                    }
                } else {
                    new_node->prev = __list->tail;
                    __list->tail ? (__list->tail->next = new_node) : 0;
                    __list->tail   = new_node;

                    if ( 0 == __list->size) {
                        __list->head = __list->tail;
                    }
                }

                /** Ok */
                __list->size++;
            } else {
                rc = EERIS_ERROR;

                if ( __list->log) {
                    eris_log_dump( __list->log, ERIS_LOG_CORE, "Create node failed, errno.<%d>", errno);
                }
            }
        }/// else nothing
    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_list_push_back


/**
 * @Brief: Push data into list back, if enable priority and compare of priority.
 *
 * @Param: __list, Eris list context.
 * @Param: __data, Push source data pointer.
 * @Param: __prio, Enable and using.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
eris_int_t eris_list_append( eris_list_t *__list, eris_void_t *__data, eris_int_t __prio)
{
    eris_int_t rc = 0;

    if ( __list) {
        /** Append is push back */
        rc = eris_list_push_back( __list, __data, __prio);

    } else { rc = EERIS_ERROR; }

    return rc;
}/// eris_list_append


/**
 * @Brief: Pop data from list head.
 *
 * @Param: __list, Eris list context.
 *
 * @Return: Pointer to data.
 **/
eris_void_t *eris_list_pop( eris_list_t *__list)
{
    eris_void_t *rc_ptr = NULL;

    if ( __list) {
        if ( __list->head) {
            /** head node */
            rc_ptr = __list->head->data;

            if ( 1 == __list->size) {
                eris_list_delete_node( __list, &(__list->head));

                __list->tail = NULL;
            } else {
                __list->head = __list->head->next;

                eris_list_delete_node( __list, &(__list->head->prev));
            }

            __list->size--;
        }
    }

    return rc_ptr;
}/// eris_list_pop


/**
 * @Brief: Pop data from list tail.
 *
 * @Param: __list, Eris list context.
 *
 * @Return: Pointer to data.
 **/
eris_void_t *eris_list_pop_back( eris_list_t *__list)
{
    eris_void_t *rc_ptr = NULL;

    if ( __list) {
        if ( __list->tail) {
            /** head node */
            rc_ptr = __list->tail->data;

            if ( 1 == __list->size) {
                eris_list_delete_node( __list, &(__list->tail));

                __list->tail = NULL;
            } else {
                __list->tail = __list->tail->prev;

                eris_list_delete_node( __list, &(__list->tail->next));
            }

            __list->size--;
        }
    }

    return rc_ptr;
}/// eris_list_pop_back


/**
 * @Brief: Find element from list head.
 *
 * @Param: __list,   Eris list context.
 * @Param; __cmp_cb, The compare callback.
 * @Param: __arg,    Input callback parameters.
 *
 * @Return: Pointer to find ok data, Other is NULL.
 **/
eris_void_t *eris_list_find( eris_list_t *__list, eris_list_cmp_cb_t __cmp_cb, eris_arg_t __arg)
{
    eris_void_t *rc_ptr = NULL;

    if ( __list && (0 < __list->size)) {
        if ( __cmp_cb) {
            eris_list_node_t *cur_node = __list->head;

            do {
                if ( 0 == __cmp_cb( cur_node->data, __arg)) {
                    rc_ptr = cur_node->data;

                    break;
                } else { cur_node = cur_node->next; }

            } while ( cur_node);
        }
    }

    return rc_ptr;
}/// eris_list_find


/**
 * @Brief: Find element from list tail.
 *
 * @Param: __list,   Eris list context.
 * @Param; __cmp_cb, The compare callback.
 * @Param: __arg,    Input callback parameters.
 *
 * @Return: Pointer to find ok data, Other is NULL.
 **/
eris_void_t *eris_list_rfind( eris_list_t *__list, eris_list_cmp_cb_t __cmp_cb, eris_arg_t __arg)
{
    eris_void_t *rc_ptr = NULL;

    if ( __list && (0 < __list->size)) {
        if ( __cmp_cb) {
            eris_list_node_t *cur_node = __list->tail;

            do {
                if ( 0 == __cmp_cb( cur_node->data, __arg)) {
                    rc_ptr = cur_node->data;

                    break;
                } else { cur_node = cur_node->prev; }

            } while ( cur_node);
        }
    }

    return rc_ptr;
}/// eris_list_rfind


/**
 * @Brief: Remove element from list.
 *
 * @Param: __list,    Eris list context.
 * @Param; __cmp_cb,  The compare callback.
 * @Param: __arg,     Input callback parameters.
 *
 * @Return: Pointer erase data, Other is NULL.
 **/
eris_void_t *eris_list_remove( eris_list_t *__list, eris_list_cmp_cb_t __cmp_cb, eris_arg_t __arg)
{
    eris_void_t *rc_ptr = NULL;

    if ( __list && (0 < __list->size)) {
        if ( __cmp_cb) {
            eris_list_node_t *cur_node = __list->tail;

            do {
                if ( 0 == __cmp_cb( cur_node->data, __arg)) {
                    rc_ptr = cur_node->data;
                    cur_node->data = NULL;

                    /** Erase cur_node */
                    if (cur_node == __list->head) { 
                        __list->head = __list->head->next; 

                        if ( __list->head) {
                            __list->head->prev = NULL;
                        }
                    }

                    if (cur_node == __list->tail) { 
                        __list->tail = __list->tail->prev; 

                        if ( __list->tail) {
                            __list->tail->next = NULL;
                        }
                    }

                    eris_list_delete_node( __list, &cur_node);

                    __list->size--;

                    /** Remove Ok... */
                    break;
                } else { cur_node = cur_node->prev; }

            } while ( cur_node);
        }
    }

    return rc_ptr;
}/// eris_list_remove


/**
 * @Brief: Erase element from list.
 *
 * @Param: __list,    Eris list context.
 * @Param: __free_cb, If set and call free data.
 * @Param; __cmp_cb,  The compare callback.
 * @Param: __arg,     Input callback parameters.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_list_erase( eris_list_t *__list, eris_list_free_cb_t __free_cb, eris_list_cmp_cb_t __cmp_cb, eris_arg_t __arg)
{
    if ( __list && (0 < __list->size)) {
        if ( __cmp_cb) {
            eris_list_node_t *cur_node = __list->tail;

            do {
                if ( 0 == __cmp_cb( cur_node->data, __arg)) {
                    if ( __free_cb) {
                        __free_cb( cur_node->data);

                        cur_node->data = NULL;
                    }

                    /** Erase cur_node */
                    if (cur_node == __list->head) { 
                        __list->head = __list->head->next; 

                        if ( __list->head) {
                            __list->head->prev = NULL;
                        }
                    }

                    if (cur_node == __list->tail) { 
                        __list->tail = __list->tail->prev; 

                        if ( __list->tail) {
                            __list->tail->next = NULL;
                        }
                    }

                    eris_list_delete_node( __list, &cur_node);

                    __list->size--;

                    /** Erase Ok... */
                    break;
                } else { cur_node = cur_node->prev; }

            } while ( cur_node);
        }
    }
}/// eris_list_erase


/**
 * @Brief: If do safe-thread lock.
 *
 * @Param: __list, Eris list context.
 * 
 * @Return: Nothing
 **/
eris_none_t eris_list_lock( eris_list_t *__list)
{
    if ( __list) {
        /** Lock */
        eris_spinlock_acquire( &(__list->lock), (eris_int_t )__list->pid, __list->nprocs);
    }
}/// eris_list_lock


/**
 * @Brief: If do safe-thread lock, then unlock this.
 *
 * @Param: __list, Eris list context.
 * 
 * @Return: Nothing.
 **/
eris_none_t eris_list_unlock( eris_list_t *__list)
{
    if ( __list) {
        /** Unlock */
        eris_spinlock_release( &(__list->lock), (eris_int_t )__list->pid);
    }
}/// eris_list_unlock


/**
 * @Brief: Cleanup eris list context.
 *
 * @Param: __list,    Eris list context.
 * @Param: __free_cb, If set and call free data.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_list_cleanup( eris_list_t *__list, eris_list_free_cb_t __free_cb)
{
    if ( __list) {
        if ( __list->head) {
            /** Cleanup all element nodes */
            eris_list_node_t *cur_node = __list->head;

            do {
                __list->head = cur_node->next;

                if ( __free_cb) {
                    __free_cb( cur_node->data);
                }
                cur_node->data = NULL;

                eris_list_delete_node( __list, &cur_node);

                cur_node = __list->head;
            } while ( cur_node);
        }

        __list->head = NULL;
        __list->tail = NULL;
        __list->size = 0;
    }
}/// eris_list_cleanup


/**
 * @Brief: Destroy eris list context.
 *
 * @Param: __list,    Eris list context.
 * @Param: __free_cb, If set and call free data.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_list_destroy( eris_list_t *__list, eris_list_free_cb_t __free_cb)
{
    if ( __list) {
        eris_list_cleanup( __list, __free_cb);

        /** Clear all */
        {
            __list->pid    = 0;
            __list->nprocs = 0;
            __list->enable_prio = false;
            __list->log    = NULL;
        }
    }
}/// eris_list_destroy


/**
 * @Brief: Create list node.
 *
 * @Param: __list, Eris list context.
 *
 * @Return: Successed pointer new node address, Other is NULL.
 **/
static eris_list_node_t *eris_list_create_node( eris_list_t *__list)
{
    eris_list_node_t *rc_node = NULL;

    if ( __list) {
        rc_node = (eris_list_node_t *)eris_memory_alloc( sizeof( eris_list_node_t));
        if ( rc_node) {
            rc_node->prio = 0;
            rc_node->data = NULL;
            rc_node->next = NULL;
            rc_node->prev = NULL;
        }
    }

    return rc_node;
}/// eris_list_create_node


/**
 * @Brief: Release list node.
 *
 * @Param: __list, Eris list context.
 *
 * @Return: Nothing.
 **/
static eris_none_t eris_list_delete_node( eris_list_t *__list, eris_list_node_t **__pnode)
{
    if ( __list && __pnode) {
       if ( *__pnode) {
            eris_memory_free( *__pnode);

            *__pnode = NULL;
       }
    }
}/// eris_list_delete_node


/**
 * @Brief: Init eris list iterator.
 *
 * @Param: __list, Eris list context.
 * @Param: __iter, Eris list iterator pointer.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_list_iter_init( eris_list_t *__list, eris_list_iter_t *__iter)
{
    if ( __list) {
        if ( __iter) {
            __iter->iter = __list->head;
        }
    } else {
        if ( __iter) {
            __iter->iter = NULL;
        }
    }
}/// eris_list_iter_init


/**
 * @Brief: Get current element and iter pointer next.
 *
 * @Param: __iter, Eris list iterator context.
 *
 * @Return: Data of list current iter node.
 **/
eris_void_t *eris_list_iter_next( eris_list_iter_t *__iter)
{
    eris_void_t *rc_ptr = NULL;

    if ( __iter) {
        if ( __iter->iter) {
            /** Pointer to data */
            rc_ptr = __iter->iter->data;

            __iter->iter = __iter->iter->next;
        }
    }

    return rc_ptr;
}/// eris_list_iter_next


/**
 * @Brief: Get current element and iter pointer prev.
 *
 * @Param: __iter, Eris list iterator context.
 *
 * @Return: Data of list current iter node.
 **/
eris_void_t *eris_list_iter_prev( eris_list_iter_t *__iter)
{
    eris_void_t *rc_ptr = NULL;

    if ( __iter) {
        if ( __iter->iter) {
            /** Pointer to data */
            rc_ptr = __iter->iter->data;

            __iter->iter = __iter->iter->next;
        }
    }

    return rc_ptr;
}/// eris_list_iter_prev


/**
 * @Brief: Reset eris list iterator.
 *
 * @Param: __list, Eris list context.
 * @Param: __iter, Eris list iterator pointer.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_list_iter_reset( eris_list_t *__list, eris_list_iter_t *__iter)
{
    if ( __list) {
        if ( __iter) {
            __iter->iter = __list->head;
        }
    } else {
        if ( __iter) {
            __iter->iter = NULL;
        }
    }
}/// eris_list_iter_reset


/**
 * @Brief: Cleanup current iter context.
 *
 * @Param: __iter, Eris list iterator context.
 *
 * @Return: Nothing.
 **/
eris_none_t eris_list_iter_cleanup( eris_list_iter_t *__iter)
{
    if ( __iter) {
        __iter->iter = NULL;
    }
}/// eris_list_iter_cleanup


