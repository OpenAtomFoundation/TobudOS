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
#include "tos_elfloader.h"

/*
 ATTENTION:
    if you wanna load the so/obj laying on your flash, you should implement "elfloader_fd_read"
    with flash driver read/write operation.

    very easy to use whether your module is on a file system or raw flash.
 */
__KNL__ __WEAK__ el_err_t elfloader_fd_read(int fd, uint32_t offset, void *buf, size_t len)
{
    if (tos_vfs_lseek(fd, (vfs_off_t)offset, VFS_SEEK_SET) < 0) {
        return ELFLOADER_ERR_FD_READ_FAILED;
    }

    if (tos_vfs_read(fd, buf, len) < 0) {
        return ELFLOADER_ERR_FD_READ_FAILED;
    }

    return ELFLOADER_ERR_NONE;
}

