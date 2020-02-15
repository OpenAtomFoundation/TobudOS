#include "tos_vfs.h"

#if TOS_CFG_VFS_EN > 0u

__STATIC__ TOS_LIST_DEFINE(k_vfs_inode_list);

/*
    1. filesystem
    assume that we have registered a yaffs and mount it on /fs/yaffs,
    we wanna find the inode of file /fs/yaffs/somedir/thefile,
    we will return the inode of /fs/yaffs, and the relative_path will be somedir/thefile
    2. device
    if we are find a device's inode, like /dev/block/nand, the path must just be the path
    of the device(/dev/block/nand), but on this condition, we would return a relative_path,
    because device operation does not need a relative path.
 */
/*
    I don't think an IOT system will have quit a lot of inodes, a linear search algorithm
    should just be OK(mainly because it's simple ^_^).
 */
__STATIC__ vfs_inode_t *vfs_inode_search(const char *path, const char **relative_path)
{
    char *name = K_NULL;
    int path_len, name_len;
    vfs_inode_t *inode = K_NULL;

    path_len = strlen(path);

    TOS_LIST_FOR_EACH_ENTRY(inode, vfs_inode_t, list, &k_vfs_inode_list) {
        name = (char *)inode->name;
        name_len = strlen(name);

        /* eg. name is /dev/nand, path is /dev/na */
        if (path_len < name_len) {
            continue;
        }

        /* 1. name is /dev/nand, path is /dev/nand
           2. name is /dev/nand, path is /dev/nand/invalid
           3. name is /dev/nand, path is /dev/nand_tail
           4. name is /fs/ramfs, path is /fs/ramfs
           5. name is /fs/ramfs, path is /fs/ramfs/somefile
           6. name is /fs/ramfs, path is /fs/ramfs_tail
         */
        if (VFS_INODE_IS_DEVICE(inode)) {
            /* for a device, name and the path should just be the same, situation 1 */
            if (path_len == name_len &&
                strncmp(name, path, name_len) == 0) {
                return inode;
            }
        } else if (VFS_INODE_IS_FILESYSTEM(inode)) {
            if (path_len == name_len &&
                strncmp(name, path, name_len) == 0) {
                /* for a filesystem, situation 4 */
                if (relative_path) {
                    *relative_path = K_NULL;
                }
                return inode;
            } else if (path_len > name_len &&
                        strncmp(name, path, name_len) == 0 &&
                        path[name_len] == '/') {
                /* for a filesystem, situation 5 */
                if (relative_path) {
                    *relative_path = &path[name_len];
                }
                return inode;
            }
        }
    }
    return K_NULL;
}

__STATIC__ vfs_inode_t *vfs_inode_create(const char *name, int name_len)
{
    vfs_inode_t *inode = K_NULL;

    inode = (vfs_inode_t *)tos_mmheap_calloc(1, VFS_INODE_SIZE(name_len));
    if (!inode) {
        return K_NULL;
    }

    strncpy(inode->name, name, name_len);
    VFS_INODE_TYPE_SET(inode, VFS_INODE_TYPE_UNUSED);
    tos_list_init(&inode->list);
    tos_list_add(&inode->list, &k_vfs_inode_list);

    return inode;
}

__KNL__ void vfs_inode_free(vfs_inode_t *inode)
{
    tos_list_del(&inode->list);
    tos_mmheap_free((void *)inode);
}

__KNL__ void vfs_inode_refinc(vfs_inode_t *inode)
{
    if (inode->refs < VFS_INODE_REFS_MAX) {
        ++inode->refs;
    }
}

__KNL__ vfs_inode_t *vfs_inode_find(const char *path, const char **relative_path)
{
    vfs_inode_t *inode = K_NULL;

    inode = vfs_inode_search(path, relative_path);
    if (!inode) {
        return K_NULL;
    }

    vfs_inode_refinc(inode);

    return inode;
}

__KNL__ int vfs_inode_is_busy(vfs_inode_t *inode)
{
    return inode->refs > 0;
}

__KNL__ vfs_inode_t *vfs_inode_alloc(const char *name)
{
    int name_len;
    vfs_inode_t *inode = K_NULL;

    if (!name) {
        return K_NULL;
    }

    name_len = strlen(name);
    if (name_len > VFS_INODE_NAME_MAX) {
        return K_NULL;
    }

    inode = vfs_inode_create(name, name_len);
    if (!inode) {
        return K_NULL;
    }

    ++inode->refs;

    return inode;
}

__STATIC__ void vfs_inode_refdec(vfs_inode_t *inode)
{
    if (inode->refs) {
        --inode->refs;
    }
}

__KNL__ void vfs_inode_release(vfs_inode_t *inode)
{
    vfs_inode_refdec(inode);

    if (inode->refs == 0) {
        vfs_inode_free(inode);
    }
}

#endif /* TOS_CFG_VFS_EN */

