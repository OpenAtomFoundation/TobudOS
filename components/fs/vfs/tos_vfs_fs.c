#include "tos_vfs.h"

#if TOS_CFG_VFS_EN > 0u

extern k_list_t k_vfs_fsmap_list;
TOS_LIST_DEFINE(k_vfs_fsmap_list);

__STATIC__ vfs_fsmap_t *vfs_fsmap_get(const char *fs_name)
{
    char *name = K_NULL;
    vfs_fsmap_t *fsmap = K_NULL;

    TOS_LIST_FOR_EACH_ENTRY(fsmap, vfs_fsmap_t, list, &k_vfs_fsmap_list) {
        name = (char *)fsmap->name;
        if (strlen(name) == strlen(fs_name) &&
            strncmp(name, fs_name, strlen(name)) == 0) {
            return fsmap;
        }
    }
    return K_NULL;
}

__STATIC_INLINE__ void vfs_fsmap_set(vfs_fsmap_t *fsmap, const char *fs_name, vfs_fs_ops_t *ops)
{
    fsmap->name = fs_name;
    fsmap->ops  = ops;
    tos_list_init(&fsmap->list);
    tos_list_add(&fsmap->list, &k_vfs_fsmap_list);
}

__STATIC_INLINE__ void vfs_fsmap_free(vfs_fsmap_t *fsmap)
{
    tos_list_del(&fsmap->list);
    tos_mmheap_free((void *)fsmap);
}

/*
    Actually the "section table hack" is much more cool and elegant, but with that
    we must deel with the link script, that will introduce coding complication,
    especially develop with IDE. So, linked list with dynamic memory is a compromised
    solution here.
 */
__API__ vfs_err_t tos_vfs_fs_register(const char *fs_name, vfs_fs_ops_t *ops)
{
    vfs_fsmap_t *fsmap = K_NULL;

    if (!fs_name || !ops) {
        return VFS_ERR_PARA_INVALID;
    }

    fsmap = vfs_fsmap_get(fs_name);
    if (fsmap) {
        return VFS_ERR_FS_ALREADY_REGISTERED;
    }

    fsmap = (vfs_fsmap_t *)tos_mmheap_calloc(1, sizeof(vfs_fsmap_t));
    if (!fsmap) {
        return VFS_ERR_OUT_OF_MEMORY;
    }

    vfs_fsmap_set(fsmap, fs_name, ops);
    return VFS_ERR_NONE;
}

__API__ vfs_err_t tos_vfs_fs_unregister(const char *fs_name)
{
    vfs_fsmap_t *fsmap = K_NULL;

    if (!fs_name) {
        return VFS_ERR_PARA_INVALID;
    }

    fsmap = vfs_fsmap_get(fs_name);
    if (!fsmap) {
        return VFS_ERR_FS_NOT_REGISTERED;
    }

    vfs_fsmap_free(fsmap);
    return VFS_ERR_NONE;
}

/*
    tos_vfs_mount("/dev/block/nand", "/fs/yaffs", "yaffs");
    we mount a filesystem named "yaffs" to directory "fs/yaffs", using device "/dev/block/nand"
 */
__API__ vfs_err_t tos_vfs_fs_mount(const char *device_path, const char *dir, const char *fs_name)
{
    vfs_fsmap_t *fsmap = K_NULL;
    vfs_inode_t *device_inode = K_NULL;
    vfs_inode_t *fs_inode = K_NULL;

    fsmap = vfs_fsmap_get(fs_name);
    if (!fsmap) {
        return VFS_ERR_FS_NOT_REGISTERED;
    }

    device_inode = vfs_inode_find(device_path, K_NULL);
    if (!device_inode) {
        return VFS_ERR_DEVICE_NOT_REGISTERED;
    }

    if (!VFS_INODE_IS_DEVICE(device_inode)) {
        return VFS_ERR_INODE_INVALID;
    }

    fs_inode = vfs_inode_find(dir, K_NULL);
    if (fs_inode) {
        if (VFS_INODE_IS_FILESYSTEM(fs_inode)) {
            return VFS_ERR_FS_ALREADY_MOUNTED;
        } else {
            return VFS_ERR_INODE_INAVALIABLE;
        }
    }

    fs_inode = vfs_inode_alloc(dir);
    if (!fs_inode) {
        return VFS_ERR_INODE_CREATE_FAILED;
    }

    VFS_INODE_TYPE_SET(fs_inode, VFS_INODE_TYPE_FILESYSTEM);
    fs_inode->ops.fs_ops = fsmap->ops;

    if (!fs_inode->ops.fs_ops->bind) {
        return VFS_ERR_OPS_NULL;
    }

    if (fs_inode->ops.fs_ops->bind(fs_inode, device_inode) != 0) {
        return VFS_ERR_OPS_FAILED;
    }

    return VFS_ERR_NONE;
}

__API__ vfs_err_t tos_vfs_fs_umount(const char *dir)
{
    vfs_inode_t *inode = K_NULL;

    inode = vfs_inode_find(dir, K_NULL);
    if (!inode) {
        return VFS_ERR_FS_NOT_MOUNT;
    }

    if (!VFS_INODE_IS_FILESYSTEM(inode)) {
        return VFS_ERR_INODE_INAVALIABLE;
    }

    if (vfs_inode_is_busy(inode)) {
        return VFS_ERR_INODE_BUSY;
    }

    if (!inode->ops.fs_ops->unbind) {
        return VFS_ERR_OPS_NULL;
    }

    if (inode->ops.fs_ops->unbind(inode) != 0) {
        return VFS_ERR_OPS_FAILED;
    }

    return VFS_ERR_NONE;
}

__API__ vfs_err_t tos_vfs_fs_mkfs(const char *device_path, const char *fs_name, int opt, unsigned long arg)
{
    vfs_fsmap_t *fsmap = K_NULL;
    vfs_inode_t *device_inode = K_NULL;
    vfs_inode_t *fs_inode = K_NULL;

    fsmap = vfs_fsmap_get(fs_name);
    if (!fsmap) {
        return VFS_ERR_FS_NOT_REGISTERED;
    }

    device_inode = vfs_inode_find(device_path, K_NULL);
    if (!device_inode) {
        return VFS_ERR_DEVICE_NOT_REGISTERED;
    }

    if (!VFS_INODE_IS_DEVICE(device_inode)) {
        return VFS_ERR_INODE_INVALID;
    }

    fs_inode->ops.fs_ops = fsmap->ops;

    if (!fsmap->ops->mkfs) {
        return VFS_ERR_OPS_NULL;
    }

    if (fsmap->ops->mkfs(device_inode, opt, arg) != 0) {
        return VFS_ERR_OPS_FAILED;
    }

    return VFS_ERR_NONE;
}

#endif /* TOS_CFG_VFS_EN */

