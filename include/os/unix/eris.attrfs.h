#ifndef __ERIS_ATTRFS_H__
#define __ERIS_ATTRFS_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief: 
 **       : File system attributes.
 **
 ******************************************************************************/

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"


#ifdef __cplusplus
extern "C" {
#endif


/** File system attribute */
typedef enum eris_attrfs_e {
    ERIS_ATTRFS_NONE  = 0,
    ERIS_ATTRFS_REG   ,
    ERIS_ATTRFS_DIR   ,
    ERIS_ATTRFS_LINK  ,
    ERIS_ATTRFS_CHAR  ,
    ERIS_ATTRFS_BLOCK ,
    ERIS_ATTRFS_FIFO  ,
    ERIS_ATTRFS_SOCK  ,
} eris_attrfs_t;


/** Mode flags */
#define ERIS_ATTRFS_CREAT          O_CREAT
#define ERIS_ATTRFS_RDONLY         O_RDONLY
#define ERIS_ATTRFS_WRONLY         O_WRONLY
#define ERIS_ATTRFS_RDWR           O_RDWR
#define ERIS_ATTRFS_TRUNC          O_TRUNC
#define ERIS_ATTRFS_APPEND         O_WRONLY | O_APPEND
#define ERIS_ATTRFS_NONBLOCK       O_NONBLOCK

/** Access flags */
#define ERIS_ATTRFS_ACCESS_DEFAULT 0644
#define ERIS_ATTRFS_ACCESS_OWNER   0600


/** Check file attributes */
#define eris_attrfs_isfile(ss)     (S_ISREG((ss).st_mode))
#define eris_attrfs_isdir(ss)      (S_ISDIR((ss).st_mode))
#define eris_attrfs_islink(ss)     (S_ISLNK((ss).st_mode))
#define eris_attrfs_ischar(ss)     (S_ISCHR((ss).st_mode))
#define eris_attrfs_isblock(ss)    (S_ISBLK((ss).st_mode))
#define eris_attrfs_isfifo(ss)     (S_ISFIFO((ss).st_mode))
#define eris_attrfs_issock(ss)     (S_ISSOCK((ss).st_mode))
#define eris_attrfs_isexec(ss)     (S_IXUSR == ((ss).st_mode & S_IXUSR))

#define eris_attrfs_size(ss)       ((ss).st_size)
#define eris_attrfs_mtime(ss)      ((ss).st_mtime)
#define eris_attrfs_inode(ss)      ((ss).st_ino)


/**
 * @Brief: Get type of path object.
 *
 * @Param: __path, Path string of file system.
 *
 * @Return: Is type of path object.
 **/
extern eris_attrfs_t eris_attrfs_get( const eris_char_t *__path);



#ifdef __cplusplus
}
#endif /** __cplusplus */


#endif /** __ERIS_ATTRFS_H__ */


