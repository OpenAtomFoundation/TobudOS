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

#include "tos_vfs.h"

vfs_err_t tos_vfs_block_device_register(const char *device_name, vfs_blkdev_ops_t *ops)
{
    int path_len = 0;
    vfs_inode_t *inode = K_NULL;

    path_len = strlen(device_name);
    if (path_len > TOS_CFG_VFS_PATH_MAX) {
        return VFS_ERR_PATH_TOO_LONG;
    }

    inode = vfs_inode_find(device_name, K_NULL);
    if (inode) {
        if (VFS_INODE_IS_BLOCK_DEVICE(inode)) {
            return VFS_ERR_DEVICE_ALREADY_REGISTERED;
        } else {
            return VFS_ERR_INODE_INAVALIABLE;
        }
    }

    inode = vfs_inode_alloc(device_name);
    if (!inode) {
        return VFS_ERR_INODE_CREATE_FAILED;
    }

    VFS_INODE_TYPE_SET(inode, VFS_INODE_TYPE_BLOCK_DEVICE);
    inode->ops.bd_ops = ops;

    return VFS_ERR_NONE;
}

vfs_err_t tos_vfs_block_device_unregister(const char *device_name)
{
    int path_len = 0;
    vfs_inode_t *inode = K_NULL;

    path_len = strlen(device_name);
    if (path_len > TOS_CFG_VFS_PATH_MAX) {
        return VFS_ERR_PATH_TOO_LONG;
    }

    inode = vfs_inode_find(device_name, K_NULL);
    if (!inode) {
        return VFS_ERR_DEVICE_NOT_REGISTERED;
    }

    if (!VFS_INODE_IS_BLOCK_DEVICE(inode)) {
        return VFS_ERR_INODE_INVALID;
    }

    if (vfs_inode_is_busy(inode)) {
        return VFS_ERR_INODE_BUSY;
    }

    vfs_inode_free(inode);
    return VFS_ERR_NONE;
}

vfs_err_t tos_vfs_char_device_register(const char *device_name, vfs_chrdev_ops_t *ops)
{
    int path_len = 0;
    vfs_inode_t *inode = K_NULL;

    path_len = strlen(device_name);
    if (path_len > TOS_CFG_VFS_PATH_MAX) {
        return VFS_ERR_PATH_TOO_LONG;
    }

    inode = vfs_inode_find(device_name, K_NULL);
    if (inode) {
        if (VFS_INODE_IS_CHAR_DEVICE(inode)) {
            return VFS_ERR_DEVICE_ALREADY_REGISTERED;
        } else {
            return VFS_ERR_INODE_INAVALIABLE;
        }
    }

    inode = vfs_inode_alloc(device_name);
    if (!inode) {
        return VFS_ERR_INODE_CREATE_FAILED;
    }

    VFS_INODE_TYPE_SET(inode, VFS_INODE_TYPE_CHAR_DEVICE);
    inode->ops.cd_ops = ops;

    return VFS_ERR_NONE;
}

vfs_err_t tos_vfs_char_device_unregister(const char *device_name)
{
    int path_len = 0;
    vfs_inode_t *inode = K_NULL;

    path_len = strlen(device_name);
    if (path_len > TOS_CFG_VFS_PATH_MAX) {
        return VFS_ERR_PATH_TOO_LONG;
    }

    inode = vfs_inode_find(device_name, K_NULL);
    if (!inode) {
        return VFS_ERR_DEVICE_NOT_REGISTERED;
    }

    if (!VFS_INODE_IS_CHAR_DEVICE(inode)) {
        return VFS_ERR_INODE_INVALID;
    }

    if (vfs_inode_is_busy(inode)) {
        return VFS_ERR_INODE_BUSY;
    }

    vfs_inode_free(inode);
    return VFS_ERR_NONE;
}

