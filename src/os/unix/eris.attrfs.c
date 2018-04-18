/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief: 
 **       : File system attributes.
 **
 ******************************************************************************/

#include "os/unix/eris.attrfs.h"


/**
 * @Brief: Get type of path object.
 *
 * @Param: __path, Path string of file system.
 *
 * @Return: Is type of path object.
 **/
eris_attrfs_t eris_attrfs_get( const eris_char_t *__path)
{
    eris_attrfs_t rc_attrfs = ERIS_ATTRFS_NONE;

    if ( __path) {
        eris_stat_t info_fs;

        /** Get object info */
        eris_int_t rc = stat( __path, &info_fs);
        if ( 0 == rc) {
            /** Check attributes flags */
            if ( eris_attrfs_isfile( info_fs)) {
                /** Is regular file */
                rc_attrfs = ERIS_ATTRFS_REG;

            } else if ( eris_attrfs_isdir( info_fs)) {
                /** Is directory */
                rc_attrfs = ERIS_ATTRFS_DIR;

            } else if ( eris_attrfs_islink( info_fs)) {
                /** Is link file */
                rc_attrfs = ERIS_ATTRFS_LINK;

            } else if ( eris_attrfs_ischar( info_fs)) {
                /** Is char device file */
                rc_attrfs = ERIS_ATTRFS_CHAR;

            } else if ( eris_attrfs_isblock( info_fs)) {
                /** Is block device file */
                rc_attrfs = ERIS_ATTRFS_BLOCK;

            } else if ( eris_attrfs_isfifo( info_fs)) {
                /** Is fifo file */
                rc_attrfs = ERIS_ATTRFS_FIFO;

            } else if ( eris_attrfs_issock( info_fs)) {
                /** Is socket file */
                rc_attrfs = ERIS_ATTRFS_SOCK;

            } else {
                /** Nothing, but not in there. */
            }
        }
    }

    return rc_attrfs;
}/// eris_attrfs_get


