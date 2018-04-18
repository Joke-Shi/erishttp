#ifndef __ERIS_FILES_H__
#define __ERIS_FILES_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief: 
 **       : Files operator.
 **
 ******************************************************************************/

#include "eris.config.h"

#include "os/unix/eris.unix.h"
#include "os/unix/eris.attrfs.h"

#include "core/eris.core.h"


#ifdef __cplusplus
extern "C" {
#endif


/** Files context type */
typedef struct eris_files_s {
    eris_string_t    name;
    eris_fd_t        fd; 
    eris_off_t       offset;
    eris_log_t      *log;
} eris_files_t;


/** File mapping type */
typedef struct eris_files_map_s {
    eris_string_t    name;
    eris_fd_t        fd;
    eris_size_t      size;
    eris_void_t     *ptr;
    eris_log_t      *log;
} eris_files_map_t;



/**
 * @Brief: Initialization file context.
 *
 * @Param: __fcontext, Files context.
 * @Param: __name, File name string.
 * @Param: __log,  Log context.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_files_init( eris_files_t *__fcontext, const eris_char_t *__name, eris_log_t *__log);


/**
 * @Brief: Create file.
 *
 * @Param: __fcontext, Files context,
 * @Param: __access,   Set the file access.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_files_create( eris_files_t *__fcontext, eris_mode_t __access);


/**
 * @Brief: Open file and return file description context.
 *
 * @Param: __fcontext, File context.
 * @Param: __flags,    File flags.
 * @Param: __access,   Set the file access.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_files_open( eris_files_t *__fcontext, eris_flag_t __flags, eris_mode_t __access);


/**
 * @Brief: Read file content with offset.
 *
 * @Param: __fcontext, File context.
 * @Param: __buf,      Input data buffer.
 * @Param: __size,     The buffer size.
 * @Param: __offset,   Offset value. 
 *
 * @Return: Read content size, Happen error is EERIS_ERROR.
 **/
extern eris_ssize_t eris_files_read( eris_files_t *__fcontext, eris_uchar_t *__buf, eris_size_t __size, eris_off_t __offset);


/**
 * @Brief: Write content to file with offset.
 *
 * @Param: __fcontext, File context.
 * @Param: __buf,      Buffer store output data.
 * @Param: __size,     The buffer size.
 * @Param: __offset,   Offset value. 
 *
 * @Return: Write content size, Happen error is EERIS_ERROR.
 **/
extern eris_ssize_t eris_files_write( eris_files_t *__fcontext, const eris_uchar_t *__buf, eris_size_t __size, eris_off_t __offset);


/**
 * @Brief: Create temp file.
 *
 * @Param: __fcontext, File context.
 * @Param: __pers,     Create temp file is persistent.
 * @Param: __access,   Access permission.
 *
 * @Return: Ok is 0; Other is EERIS_ERROR.
 **/
extern eris_int_t eris_files_create_tf( eris_files_t *__fcontext, eris_bool_t __pers, eris_mode_t __access);


/**
 * @Brief: Get file size.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: file size.
 **/
extern eris_size_t eris_files_size( eris_files_t *__fcontext);


/**
 * @Brief: Get file mtime.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: file mtime.
 **/
extern eris_time_t eris_files_mtime( eris_files_t *__fcontext);


/**
 * @Brief: Get file inode.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: file inode.
 **/
extern eris_inode_t eris_files_inode( eris_files_t *__fcontext);


/**
 * @Brief: Set time of file.
 *
 * @Param: __fcontext, File context.
 * @Param: __time, time value.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
extern eris_int_t eris_files_settime( eris_files_t *__fcontext, eris_time_t __time);


/**
 * @Brief: Rename old file name to new.
 *
 * @Param: __fcontext, File context.
 * @Param: __new,      New file name.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
extern eris_int_t eris_files_rename( eris_files_t *__fcontext, const eris_char_t *__new);


/**
 * @Brief: Get file information.
 *
 * @Param: __fcontext, File context.
 * @Param: __info,     Output file information.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
extern eris_int_t eris_files_info( eris_files_t *__fcontext, eris_stat_t *__info);


/**
 * @Brief: Get lock of file context.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: Ok is true, Other is false.
 **/
extern eris_bool_t eris_files_lock( eris_files_t *__fcontext);


/**
 * @Brief: Try lock of file context.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: Ok is true, Other is false.
 **/
extern eris_bool_t eris_files_trylock( eris_files_t *__fcontext);


/**
 * @Brief: Unlock of file context.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: Ok is true, Other is false.
 **/
extern eris_bool_t eris_files_unlock( eris_files_t *__fcontext);


/**
 * @Brief: Check file is readable.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: If file readable is true, Other is false.
 **/
extern eris_bool_t eris_files_readable( eris_files_t *__fcontext);


/**
 * @Brief: Check file is writeable.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: If file writeable is true, Other is false.
 **/
extern eris_bool_t eris_files_writeable( eris_files_t *__fcontext);


/**
 * @Brief: Check file is existd.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: If file existd is true, Other is false.
 **/
extern eris_bool_t eris_files_existed( eris_files_t *__fcontext);


/**
 * @Brief: Close file context.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: Ok->0, Other->EERIS_ERROR.
 **/
extern eris_int_t eris_files_close( eris_files_t *__fcontext);


/**
 * @Brief: remove file.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_files_remove( eris_files_t *__fcontext);


/**
 * @Brief: Release file context.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_files_destroy( eris_files_t *__fcontext);


#ifdef __cplusplus
}
#endif /** __cplusplus */


#endif /** __ERIS_FILES_H__ */


