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

#ifndef _TOS_VFS_INODE_H_
#define  _TOS_VFS_INODE_H_

// why your inode's name so long?
#define VFS_INODE_NAME_MAX              31

typedef union vfs_inode_operations_un {
    vfs_chrdev_ops_t    *cd_ops;
    vfs_blkdev_ops_t    *bd_ops;
    vfs_fs_ops_t        *fs_ops;
} vfs_inode_ops_t;

#define VFS_INODE_TYPE_CHAR_DEVICE          0x00000000
#define VFS_INODE_TYPE_BLOCK_DEVICE         0x00000001
#define VFS_INODE_TYPE_FILESYSTEM           0x00000002
#define VFS_INODE_TYPE_UNUSED               0x00000003

#define VFS_INODE_TYPE_SET(inode, _type) \
    ((inode)->type = _type)

#define VFS_INODE_IS_TYPE(inode, _type) \
    ((inode)->type == _type)

#define VFS_INODE_IS_CHAR_DEVICE(inode)     VFS_INODE_IS_TYPE(inode, VFS_INODE_TYPE_CHAR_DEVICE)
#define VFS_INODE_IS_BLOCK_DEVICE(inode)    VFS_INODE_IS_TYPE(inode, VFS_INODE_TYPE_BLOCK_DEVICE)
#define VFS_INODE_IS_FILESYSTEM(inode)      VFS_INODE_IS_TYPE(inode, VFS_INODE_TYPE_FILESYSTEM)
#define VFS_INODE_IS_DEVICE(inode)          (VFS_INODE_IS_BLOCK_DEVICE(inode) || VFS_INODE_IS_CHAR_DEVICE(inode))

#define VFS_INODE_REFS_MAX                  (0x3F)

typedef struct vfs_inode_st {
    uint8_t             type : 2; /*< now we only support three kind of inode */
    uint8_t             refs : 6; /*< max 0x3F */
    vfs_inode_ops_t     ops;
    void               *private;
    k_list_t            list;
    char                name[1];
} vfs_inode_t;

#define VFS_INODE_SIZE(name_len)    (sizeof(vfs_inode_t) + name_len)

__KNL__ int vfs_inode_is_busy(vfs_inode_t *inode);

__KNL__ void vfs_inode_refinc(vfs_inode_t *inode);

__KNL__ vfs_inode_t *vfs_inode_find(const char *path, const char **relative_path);

__KNL__ vfs_inode_t *vfs_inode_alloc(const char *name);

__KNL__ void vfs_inode_free(vfs_inode_t *inode);

__KNL__ void vfs_inode_release(vfs_inode_t *inode);

#endif /* _TOS_VFS_INODE_H_ */

