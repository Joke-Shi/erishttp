/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief: 
 **       : Files operator.
 **
 ******************************************************************************/

#include "os/unix/eris.files.h"


/**
 * @Brief: Initialization file context.
 *
 * @Param: __fcontext, Files context.
 * @Param: __name, File name string.
 * @Param: __log,  Log context.
 *
 * @Return: Ok is 0, Other is -1.
 **/
eris_int_t eris_files_init( eris_files_t *__fcontext, const eris_char_t *__name, eris_log_t *__log)
{
    eris_int_t rc = 0;

    if ( __fcontext) {
        __fcontext->name   = NULL;
        __fcontext->fd     = -1;
        __fcontext->offset = 0;
        __fcontext->log    = __log;

        /** Specify file name and clone */
        if ( __name) {
            __fcontext->name = eris_string_clone( __name);
            if ( !(__fcontext->name)) {
                /** Clone error */
                rc = EERIS_ERROR;

                if ( __log) { eris_log_dump( __log, ERIS_LOG_CORE, "Call eris_string_clone failed"); }
            }
        }
    } else { 
        rc = EERIS_ERROR; 

        if ( __log) { eris_log_dump( __log, ERIS_LOG_CORE, "Input \"__fcontext\" is invalid"); }
    }

    return rc;
}/// eris_files_init


/**
 * @Brief: Create file.
 *
 * @Param: __fcontext, Files context,
 * @Param: __access,   Set the file access.
 *
 * @Return: Ok is 0, Other is -1.
 **/
eris_int_t eris_files_create( eris_files_t *__fcontext, eris_mode_t __access)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __fcontext) {
        if ( (0 > __fcontext->fd) && __fcontext->name) {
            if ( 0 == __access) {
                __access = ERIS_ATTRFS_ACCESS_DEFAULT;
            }

#ifdef __CYGWIN__
            __fcontext->fd = open( __fcontext->name, ERIS_ATTRFS_RDWR | ERIS_ATTRFS_CREAT | O_BINARY, __access);
#else
            __fcontext->fd = open( __fcontext->name, ERIS_ATTRFS_RDWR | ERIS_ATTRFS_CREAT, __access);
#endif
            if ( -1 == __fcontext->fd) {
                rc = EERIS_ERROR;

                /** Has open of fcontext */
                if ( __fcontext->log) {
                       eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Create file.<%s> failed, errno.<%d>",
                                        __fcontext->name,
                                        errno); 
                }
            }

        } else { 
            rc = EERIS_ERROR; 

            if ( -1 != __fcontext->fd) {
                /** Has open of fcontext */
                if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Has used fd"); }

            } else {
                /** Unspecify file name */
                if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Unspecify file name"); }
            }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_files_create


/**
 * @Brief: Open file and return file description context.
 *
 * @Param: __fcontext, File context.
 * @Param: __flags,    File flags.
 * @Param: __access,   Set the file access.
 *
 * @Return: Ok is 0, Other is -1.
 **/
eris_int_t eris_files_open( eris_files_t *__fcontext, eris_flag_t __flags, eris_mode_t __access)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __fcontext) {
        /** Do open failed */
        if ( (0 > __fcontext->fd) && __fcontext->name) {
            if ( 0 == __flags) {
                /** Default readable and writeable flags */
                __flags = ERIS_ATTRFS_RDWR;
            }

            if ( 0 == __access) {
                __access = ERIS_ATTRFS_ACCESS_DEFAULT;
            }

#ifdef __CYGWIN__
            __fcontext->fd = open( __fcontext->name, __flags | O_BINARY, __access);
#else
            __fcontext->fd = open( __fcontext->name, __flags, __access);
#endif
            if ( -1 == __fcontext->fd) {
                rc = EERIS_ERROR;

                /** Has open of fcontext */
                if ( __fcontext->log) {
                       eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Create file.<%s> failed, errno.<%d>",
                                        __fcontext->name,
                                        errno); 
                }
            }

        } else { 
            rc = EERIS_ERROR; 

            if ( -1 != __fcontext->fd) {
                /** Has open of fcontext */
                if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Has used fd"); }

            } else {
                /** Unspecify file name */
                if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Unspecify file name"); }
            }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_files_open


/**
 * @Brief: Read file content with offset.
 *
 * @Param: __fcontext, File context.
 * @Param: __buf,      Input data buffer.
 * @Param: __size,     The buffer size.
 * @Param: __offset,   Offset value. 
 *
 * @Return: Read content size, Happen error is -1.
 **/
eris_ssize_t eris_files_read( eris_files_t *__fcontext, eris_uchar_t *__buf, eris_size_t __size, eris_off_t __offset)
{
    eris_int_t   tmp_errno = errno;
    eris_ssize_t r_size    = 0;

    if ( __fcontext) {
        if ( 0 <= __fcontext->fd) {
#if (ERIS_HAVE_PREAD)
            r_size = pread( __fcontext->fd, __buf, __size, __offset);
#else
            /** Seek */
            if ( __offset != __fcontext->offset) {
                if ( -1 != lseek( __fcontext->fd, __offset, SEEK_SET)) {
                    __fcontext->offset = __offset;

                } else {
                    r_size = -1;

                    if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "lseek failed, errno.<%d>", errno); }
                }
            }

            if ( -1 != r_size) {
                while ( r_size < __size) {
                    /** Read */
                    eris_ssize_t r_count = read( __fcontext->fd, __buf, __size - r_size);
                    if ( 0 < r_count) {
                        r_size += r_count;

                    } else if ( 0 == r_count) {
                        break;

                    } else {
                        if ( EINTR == errno) {
                            continue;

                        } else {
                            r_size = -1;

                            if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Read failed, errno.<%d>", errno); }
                        }
                    }
                }

                /** Reset file offset */
                if ( -1 != r_size) { __fcontext->offset += r_size; }
            }
#endif
        } else {
            r_size = -1; 

            if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Invalid fd"); }
        }
    } else { r_size = -1; }

    errno = tmp_errno;

    return r_size;
}/// eris_files_read


/**
 * @Brief: Write content to file with offset.
 *
 * @Param: __fcontext, File context.
 * @Param: __buf,      Buffer store output data.
 * @Param: __size,     The buffer size.
 * @Param: __offset,   Offset value. 
 *
 * @Return: Write content size, Happen error is -1.
 **/
eris_ssize_t eris_files_write( eris_files_t *__fcontext, const eris_uchar_t *__buf, eris_size_t __size, eris_off_t __offset)
{
    eris_int_t   tmp_errno = errno;
    eris_ssize_t w_size    = 0;

    if ( __fcontext) {
        if ( 0 <= __fcontext->fd) {
            eris_ssize_t w_count = 0;
#if (ERIS_HAVE_PWRITE)
            while ( (eris_size_t )w_size < __size ) {
                w_count = pwrite( __fcontext->fd, __buf + w_size, __size - (eris_size_t )w_size, __offset);
                if ( -1 == w_count) {
                    if ( 0 == w_size) {
                        w_size = -1;
                    }

                    /** Error */
                    break;
                }

                /** Write ok */
                __fcontext->offset += w_count;
                w_size += w_count;

                __offset += w_count;
            }
#else
            /** Seek */
            if ( __offset != __fcontext->offset) {
                if ( -1 != lseek( __fcontext->fd, __offset, SEEK_SET)) {
                    __fcontext->offset = __offset;

                } else {
                    w_size = -1;

                    if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "lseek failed, errno.<%d>", errno); }
                }
            }

            if ( -1 != w_size) {
                while ( w_size < __size ) {
                    w_count = write( __fcontext->fd, __buf + w_size, __size - (eris_size_t )w_size);
                    if ( -1 == w_count) {
                        if ( 0 == w_size) {
                            w_size = -1;
                        }

                        /** Error */
                        break;
                    }

                    /** Write ok */
                    __fcontext->offset += w_count;
                    w_size += w_count;
                }
            }
#endif
        } else {
            w_size = -1; 

            if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Invalid fd"); }
        }
    } else { w_size = -1; }

    errno = tmp_errno;

    return w_size;
}/// eris_files_write


/**
 * @Brief: Create temp file.
 *
 * @Param: __fcontext, File context.
 * @Param: __pers,     Create temp file is persistent.
 * @Param: __access,   Access permission.
 *
 * @Return: Ok is 0; Other is -1.
 **/
eris_int_t eris_files_create_tf( eris_files_t *__fcontext, eris_bool_t __pers, eris_mode_t __access)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __fcontext) {
        if ( (0 > __fcontext->fd) && __fcontext->name) {
            if ( 0 == __access) {
                __access = ERIS_ATTRFS_ACCESS_DEFAULT;
            }

#ifdef __CYGWIN__
            __fcontext->fd = open( __fcontext->name, O_RDWR | O_CREAT | O_EXCL | 0_BINARY, __access);
#else
            __fcontext->fd = open( __fcontext->name, O_RDWR | O_CREAT | O_EXCL, __access);
#endif
            if ( (-1 != __fcontext->fd) && !(__pers)) {
                /** Unpersistent and delete */
                (void )unlink( __fcontext->name);
            }

            if ( -1 == __fcontext->fd) {
                rc = EERIS_ERROR;

                /** Create failed */
                eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Create temp file.<%s> failed, errno.<%d>", 
                                    __fcontext->name,
                                    errno);
            }
        } else { 
            rc = EERIS_ERROR; 

            if ( -1 != __fcontext->fd) {
                /** Has open of fcontext */
                if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Has used fd"); }

            } else {
                /** Unspecify file name */
                if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Unspecify file name"); }
            }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_files_craete_tf


/**
 * @Brief: Get file size.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: file size.
 **/
eris_size_t eris_files_size( eris_files_t *__fcontext)
{
    eris_size_t rc_size = 0;

    if ( __fcontext) {
        if ( __fcontext->name) {
            eris_stat_t file_info;

            /** Get file information */
            if ( -1 != stat( __fcontext->name, &file_info)) {
                rc_size = eris_attrfs_size( file_info);
            }
        } else {
            if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Unspecify file name"); }
        }
    }

    return rc_size;
}/// eris_files_size


/**
 * @Brief: Get file mtime.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: file mtime.
 **/
eris_time_t eris_files_mtime( eris_files_t *__fcontext)
{
    eris_time_t rc_mtime = 0;

    if ( __fcontext) {
        if ( __fcontext->name) {
            eris_stat_t file_info;

            /** Get file information */
            if ( -1 != stat( __fcontext->name, &file_info)) {
                rc_mtime = eris_attrfs_mtime( file_info);
            }
        } else {
            if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Unspecify file name"); }
        }
    }

    return rc_mtime;
}/// eris_files_mtime


/**
 * @Brief: Get file inode.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: file inode.
 **/
eris_inode_t eris_files_inode( eris_files_t *__fcontext)
{
    eris_inode_t rc_inode = 0;

    if ( __fcontext) {
        if ( __fcontext->name) {
            eris_stat_t file_info;

            /** Get file information */
            if ( -1 != stat( __fcontext->name, &file_info)) {
                rc_inode = eris_attrfs_inode( file_info);
            }
        } else {
            if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Unspecify file name"); }
        }
    }

    return rc_inode;
}/// eris_files_inode


/**
 * @Brief: Set time of file.
 *
 * @Param: __fcontext, File context.
 * @Param: __time, time value.
 *
 * @Return: Ok->0, Other->-1.
 **/
eris_int_t eris_files_settime( eris_files_t *__fcontext, eris_time_t __time)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __fcontext) {
        if ( __fcontext->name) {
            eris_timeval_t tvs[2]; {
                tvs[0].tv_sec  = __time;
                tvs[0].tv_usec = 0;

                tvs[1].tv_sec  = __time;
                tvs[2].tv_usec = 0;

            }

            /** Update access and modify time */
            rc = utimes( __fcontext->name, tvs);
            if ( 0 != rc) {
                rc = EERIS_ERROR;

                /** Update time failed */
                eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Update time of file.<%s> failed, errno.<%d>", 
                                    __fcontext->name,
                                    errno);
            }
        } else {
            rc = EERIS_ERROR;

            if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Unspecify file name"); }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_files_settime


/**
 * @Brief: Rename old file name to new.
 *
 * @Param: __fcontext, File context.
 * @Param: __new,      New file name.
 *
 * @Return: Ok->0, Other->-1.
 **/
eris_int_t eris_files_rename( eris_files_t *__fcontext, const eris_char_t *__new)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __fcontext) {
        if ( __new) {
            if ( __fcontext->name) {
                /** Rename */
                rc = rename( __fcontext->name, __new);
                if ( 0 == rc) {
                    eris_string_free( __fcontext->name);

                    __fcontext->name = eris_string_clone( __new);
                    if ( !(__fcontext->name)) {
                        eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Clone new file name failed, errno.<%d>", errno);

                    }
                } else {
                    /** Error */
                    if ( __fcontext->log) {
                        eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Rename old name.<%s> to new name.<%s> failed, errno.<%d>", 
                                            __fcontext->name,
                                            __new,
                                            errno);
                    }
                }
            }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_files_rename


/**
 * @Brief: Get file information.
 *
 * @Param: __fcontext, File context.
 * @Param: __info,     Output file information.
 *
 * @Return: Ok->0, Other->-1.
 **/
eris_int_t eris_files_info( eris_files_t *__fcontext, eris_stat_t *__info)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = 0;

    if ( __fcontext) {
        if ( __fcontext->name) {
            rc = stat( __fcontext->name, __info);
            if ( 0 != rc) {
                if ( __fcontext->log) {
                    eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Access file.<%s> failed, errno.<%d>", 
                                        __fcontext->name,
                                        errno);
                }
            }
        } else {
            if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Unspecify file name"); }
        }
    } else { rc = EERIS_ERROR; }

    errno = tmp_errno;

    return rc;
}/// eris_files_info


/**
 * @Brief: Get lock of file context.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: Ok is true, Other is false.
 **/
eris_bool_t eris_files_lock( eris_files_t *__fcontext)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = EERIS_ERROR;

    if ( __fcontext) {
        if ( 0 <= __fcontext->fd) {
            eris_flock_t flck; {
                flck.l_type   = F_WRLCK;
                flck.l_start  = 0;
                flck.l_whence = SEEK_SET;
                flck.l_len    = 0;
                flck.l_pid    = 0;
            }

            /** Lock */
            rc = fcntl( __fcontext->fd, F_SETLKW, &flck);
        } else {
            if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Invalid fd"); }
        }
    }

    errno = tmp_errno;

    return (rc) ? false : true;
}/// eris_files_lock


/**
 * @Brief: Try lock of file context.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: Ok is true, Other is false.
 **/
eris_bool_t eris_files_trylock( eris_files_t *__fcontext)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = EERIS_ERROR;

    if ( __fcontext) {
        if ( 0 <= __fcontext->fd) {
            eris_flock_t flck; {
                flck.l_type   = F_WRLCK;
                flck.l_start  = 0;
                flck.l_whence = SEEK_SET;
                flck.l_len    = 0;
                flck.l_pid    = 0;
            }

            /** Lock */
            rc = fcntl( __fcontext->fd, F_SETLK, &flck);
        } else {
            if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Invalid fd"); }
        }
    }

    errno = tmp_errno;

    return (rc) ? false : true;
}/// eris_files_trylock


/**
 * @Brief: Unlock of file context.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: Ok is true, Other is false.
 **/
eris_bool_t eris_files_unlock( eris_files_t *__fcontext)
{
    eris_int_t tmp_errno = errno;
    eris_int_t rc = EERIS_ERROR;

    if ( __fcontext) {
        if ( 0 <= __fcontext->fd) {
            eris_flock_t flck; {
                flck.l_type   = F_UNLCK;
                flck.l_start  = 0;
                flck.l_whence = SEEK_SET;
                flck.l_len    = 0;
                flck.l_pid    = 0;
            }

            /** Lock */
            rc = fcntl( __fcontext->fd, F_SETLKW, &flck);
        } else {
            if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Invalid fd"); }
        }
    }

    errno = tmp_errno;

    return (rc) ? false : true;
}/// eris_files_unlock


/**
 * @Brief: Check file is readable.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: If file readable is true, Other is false.
 **/
eris_bool_t eris_files_readable( eris_files_t *__fcontext)
{
    eris_int_t  tmp_errno = errno;
    eris_bool_t rc = false;

    if ( __fcontext) {
        if ( __fcontext->name) {
            if ( 0 == access( __fcontext->name, R_OK)) {
                rc = true;

            } else {
                if ( __fcontext->log) {
                    eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Access file.<%s> failed, errno.<%d>", 
                                        __fcontext->name,
                                        errno);
                }
            }
        } else {
            if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Unspecify file name"); }
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_files_readable


/**
 * @Brief: Check file is writeable.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: If file writeable is true, Other is false.
 **/
eris_bool_t eris_files_writeable( eris_files_t *__fcontext)
{
    eris_int_t  tmp_errno = errno;
    eris_bool_t rc = false;

    if ( __fcontext) {
        if ( __fcontext->name) {
            if ( 0 == access( __fcontext->name, W_OK)) {
                rc = true;
            } else {
                if ( __fcontext->log) {
                    eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Access file.<%s> failed, errno.<%d>", 
                                        __fcontext->name,
                                        errno);
                }
            }
        } else {
            if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Unspecify file name"); }
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_files_writeable


/**
 * @Brief: Check file is existd.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: If file existd is true, Other is false.
 **/
eris_bool_t eris_files_existed( eris_files_t *__fcontext)
{
    eris_int_t  tmp_errno = errno;
    eris_bool_t rc = false;

    if ( __fcontext) {
        if ( __fcontext->name) {
            if ( 0 == access( __fcontext->name, F_OK)) {
                rc = true;

            } else {
                if ( __fcontext->log) {
                    eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Access file.<%s> failed, errno.<%d>", 
                                        __fcontext->name,
                                        errno);
                }
            }
        } else {
            if ( __fcontext->log) { eris_log_dump( __fcontext->log, ERIS_LOG_CORE, "Unspecify file name"); }
        }
    }

    errno = tmp_errno;

    return rc;
}/// eris_files_existed


/**
 * @Brief: Close file context.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: Ok->0, Other->-1.
 **/
eris_int_t eris_files_close( eris_files_t *__fcontext)
{
    eris_int_t rc = 0;

    if ( __fcontext) {
        if ( 0 <= __fcontext->fd) {
            eris_int_t times = 3;

            do {
                rc = close( __fcontext->fd);
                if ( 0 == rc) {
                    __fcontext->fd = -1;

                    break;
                }

                --times;
            } while ( 0 < times);
        }
    }

    return rc;
}/// eris_files_close


/**
 * @Brief: remove file.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_files_remove( eris_files_t *__fcontext)
{
    if ( __fcontext) {
        (void )eris_files_close( __fcontext);

        if ( __fcontext->name) {
            /** Delete file */
            (void )unlink( __fcontext->name);
        }
    }
}/// eris_files_remove


/**
 * @Brief: Release file context.
 *
 * @Param: __fcontext, File context.
 *
 * @Return: Nothing.
 **/
eris_void_t eris_files_destroy( eris_files_t *__fcontext)
{
    if ( __fcontext) {
        if ( __fcontext->name) {
            eris_string_free( __fcontext->name);
            eris_string_init( __fcontext->name);
        }

        if ( 0 <= __fcontext->fd) {
            close( __fcontext->fd);
        }

        eris_string_init( __fcontext->name);
        __fcontext->fd     = -1;
        __fcontext->offset = 0;
        __fcontext->log    = NULL;
    }
}/// eris_files_destroy


