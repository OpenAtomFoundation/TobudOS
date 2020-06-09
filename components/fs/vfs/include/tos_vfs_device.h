/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef _TOS_VFS_DEVICE_H_
#define  _TOS_VFS_DEVICE_H_

typedef struct vfs_inode_st vfs_inode_t;

typedef struct vfs_char_device_operations_st {
    int     (*open)     (vfs_file_t *file);
    int     (*close)    (vfs_file_t *file);
    ssize_t (*read)     (vfs_file_t *file, void *buf, size_t count);
    ssize_t (*write)    (vfs_file_t *file, const void *buf, size_t count);
    vfs_off_t   (*lseek)    (vfs_file_t *file, vfs_off_t offset, vfs_whence_t whence);
    int     (*ioctl)    (vfs_file_t *file, int cmd, unsigned long arg);

    int     (*suspend)  (void);
    int     (*resume)   (void);
} vfs_chrdev_ops_t;

typedef struct vfs_block_device_geometry_st {
    int         is_available;
    uint32_t    sector_size;
    uint32_t    nsectors;
} vfs_blkdev_geo_t;

typedef struct vfs_block_device_operations_st {
    int     (*open)     (vfs_inode_t *dev);
    int     (*close)    (vfs_inode_t *dev);
    ssize_t (*read)     (vfs_inode_t *dev, void *buf, size_t start_sector, unsigned int nsectors);
    ssize_t (*write)    (vfs_inode_t *dev, const unsigned char *buf, size_t start_sector, unsigned int nsectors);
    int     (*geometry) (vfs_inode_t *dev, vfs_blkdev_geo_t *geo);
    int     (*erase)    (vfs_inode_t *dev, size_t start_sector, size_t nsectors);
    int     (*ioctl)    (vfs_inode_t *dev, int cmd, unsigned long arg);

    int     (*suspend)  (void);
    int     (*resume)   (void);
} vfs_blkdev_ops_t;

__API__ vfs_err_t tos_vfs_block_device_register(const char *device_name, vfs_blkdev_ops_t *ops);

__API__ vfs_err_t tos_vfs_block_device_unregister(const char *device_name);

__API__ vfs_err_t tos_vfs_char_device_register(const char *device_name, vfs_chrdev_ops_t *ops);

__API__ vfs_err_t tos_vfs_char_device_unregister(const char *device_name);

#endif /* _TOS_VFS_DEVICE_H_ */

