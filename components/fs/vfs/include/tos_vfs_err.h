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

#ifndef _TOS_VFS_ERR_H_
#define  _TOS_VFS_ERR_H_

typedef enum vfs_err_en {
    VFS_ERR_NONE,
    VFS_ERR_BUFFER_NULL,
    VFS_ERR_DEVICE_NOT_REGISTERED,
    VFS_ERR_DEVICE_ALREADY_REGISTERED,
    VFS_ERR_FILE_NO_AVAILABLE,
    VFS_ERR_FILE_NOT_OPEN,
    VFS_ERR_FS_ALREADY_MOUNTED,
    VFS_ERR_FS_ALREADY_REGISTERED,
    VFS_ERR_FS_NOT_REGISTERED,
    VFS_ERR_FS_NOT_MOUNT,
    VFS_ERR_OPS_NULL,
    VFS_ERR_OPS_FAILED,
    VFS_ERR_INODE_NAME_TOO_LONG,
    VFS_ERR_INODE_CREATE_FAILED,
    VFS_ERR_INODE_NOT_FOUND,
    VFS_ERR_INODE_INVALID,
    VFS_ERR_INODE_BUSY,
    VFS_ERR_INODE_INAVALIABLE,
    VFS_ERR_OPEN_DIR,
    VFS_ERR_OUT_OF_MEMORY,
    VFS_ERR_PARA_INVALID,
    VFS_ERR_PATH_TOO_LONG,
} vfs_err_t;

#endif /* _TOS_VFS_ERR_H_ */

