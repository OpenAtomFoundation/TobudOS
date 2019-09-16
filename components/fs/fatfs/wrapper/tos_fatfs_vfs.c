#include "tos_ff.h"
#include "tos_vfs.h"

static BYTE fatfs_translate_oflag2mode(vfs_oflag_t flags)
{
    BYTE mode = 0;

    if (flags & VFS_OFLAG_READ) {
        mode |= FA_READ;
    }
    if (flags & VFS_OFLAG_WRITE) {
        mode |= FA_WRITE;
    }
    if (flags & VFS_OFLAG_EXISTING) {
        mode |= FA_OPEN_EXISTING;
    }
    if (flags & VFS_OFLAG_CREATE_NEW) {
        mode |= FA_CREATE_NEW;
    }
    if (flags & VFS_OFLAG_CREATE_ALWAYS) {
        mode |= FA_CREATE_ALWAYS;
    }
    if (flags & VFS_OFLAG_OPEN_ALWAYS) {
        mode |= FA_OPEN_ALWAYS;
    }
    if (flags & VFS_OFLAG_OPEN_APPEND) {
        mode |= FA_OPEN_APPEND;
    }
    return mode;
}

static int fatfs_open(vfs_file_t *file, const char *pathname, vfs_oflag_t flags)
{
    FIL *fp = NULL;
    FATFS *fatfs = NULL;
    FRESULT res;
    BYTE mode = 0;

    fp = tos_mmheap_alloc(sizeof(FIL));
    if (!fp) {
        return -1;
    }

    file->private = fp;
    fatfs = (FATFS *)file->inode->private;
    mode = fatfs_translate_oflag2mode(flags);

    res = tos_f_open(fatfs, fp, pathname, mode);
    if (res != FR_OK) {
        return -1;
    }

    return 0;
}

static ssize_t fatfs_read(vfs_file_t *file, void *buf, size_t count)
{
    FIL *fp = NULL;
    uint32_t length;
    FRESULT res;

    fp = (FIL *)file->private;

    res = tos_f_read(fp, buf, count, &length);
    if (res != FR_OK) {
        return -1;
    }

    return length;
}

static ssize_t fatfs_write(vfs_file_t *file, const void *buf, size_t count)
{
    FIL *fp = NULL;
    uint32_t length;
    FRESULT res;

    fp = (FIL *)file->private;

    res = tos_f_write(fp, buf, count, &length);
    if (res != FR_OK) {
        return -1;
    }

    return length;
}

static int fatfs_close(vfs_file_t *file)
{
    FIL *fp = NULL;
    FRESULT res;

    fp = (FIL *)file->private;

    res = tos_f_close(fp);
    if (res != FR_OK) {
        return -1;
    }

    tos_mmheap_free(fp);
    file->private = NULL;

    return 0;
}

static vfs_off_t fatfs_lseek(vfs_file_t *file, vfs_off_t offset, vfs_whence_t whence)
{
    FIL *fp = NULL;
    FRESULT res;

    fp = (FIL *)file->private;

    res = tos_f_lseek(fp, offset);
    if (res != FR_OK) {
        return -1;
    }

    return offset;
}

static int fatfs_sync(vfs_file_t *file)
{
    FIL *fp = NULL;
    FRESULT res;

    fp = (FIL *)file->private;

    res = tos_f_sync(fp);
    if (res != FR_OK) {
        return -1;
    }

    return 0;
}

static int fatfs_truncate(vfs_file_t *file, vfs_off_t length)
{
    FIL *fp = NULL;
    FRESULT res;

    fp = (FIL *)file->private;

    res = tos_f_truncate(fp);
    if (res != FR_OK) {
        return -1;
    }

    return 0;
}

static int fatfs_opendir(vfs_dir_t *dir, const char *pathname)
{
    DIR *dp = NULL;
    FATFS *fatfs = NULL;
    FRESULT res;

    dp = tos_mmheap_alloc(sizeof(DIR));
    if (!dp) {
        return -1;
    }

    dir->private = dp;
    fatfs = (FATFS *)dir->inode->private;

    res = tos_f_opendir(fatfs, dp, pathname);
    if (res != FR_OK) {
        return -1;
    }

    return 0;
}


static int fatfs_closedir(vfs_dir_t *dir)
{
    DIR *dp = NULL;
    FRESULT res;

    dp = (DIR *)dir->private;

    res = tos_f_closedir(dp);
    if (res != FR_OK) {
        return -1;
    }

    tos_mmheap_free(dp);
    dir->private = NULL;

    return 0;
}

static void fatfs_translate_filinfo2dirent(FILINFO *info, vfs_dirent_t *dirent)
{
    switch (info->fattrib) {
        case AM_DIR:
            dirent->type = VFS_TYPE_DIRECTORY;
            break;

        case AM_ARC:
            dirent->type = VFS_TYPE_FILE;
            break;

        default:
            dirent->type = VFS_TYPE_OTHER;
            break;
    }

    dirent->size = info->fsize;
    dirent->date = info->fdate;
    dirent->time = info->ftime;
    strncpy(dirent->name, info->fname, VFS_PATH_MAX);
    dirent->name[VFS_PATH_MAX - 1] = '\0';
}

static int fatfs_readdir(vfs_dir_t *dir, vfs_dirent_t *dirent)
{
    FILINFO info;
    DIR *dp = NULL;
    FRESULT res;

    dp = (DIR *)dir->private;
    res = tos_f_readdir(dp, &info);
    if (res != FR_OK) {
        return -1;
    }

    fatfs_translate_filinfo2dirent(&info, dirent);

    return 0;
}

static int fatfs_unlink(vfs_inode_t *fs, const char *pathname)
{
    FATFS *fatfs = NULL;
    FRESULT res;

    fatfs = (FATFS *)fs->private;

    res = tos_f_unlink(fatfs, pathname);
    if (res != FR_OK) {
        return -1;
    }
    return 0;
}

static int fatfs_mkdir(vfs_inode_t *fs, const char *pathname)
{
    FATFS *fatfs = NULL;
    FRESULT res;

    fatfs = (FATFS *)fs->private;

    res = tos_f_mkdir(fatfs, pathname);
    if (res != FR_OK) {
        return -1;
    }
    return 0;
}

static int fatfs_rename(vfs_inode_t *fs, const char *oldpath, const char *newpath)
{
    FATFS *fatfs = NULL;
    FRESULT res;

    fatfs = (FATFS *)fs->private;

    res = tos_f_rename(fatfs, oldpath, newpath);
    if (res != FR_OK) {
        return -1;
    }
    return 0; 
}

static void fatfs_translate_filinfo2fstat(FILINFO *info, vfs_fstat_t *buf)
{
    switch (info->fattrib) {
        case AM_DIR:
            buf->type = VFS_TYPE_DIRECTORY;
            break;
    
        case AM_ARC:
            buf->type = VFS_TYPE_FILE;
            break;
    
        default:
            buf->type = VFS_TYPE_OTHER;
            break;
    }

    buf->size   = info->fsize;
    buf->mtime  = info->ftime;
}

static int fatfs_stat(vfs_inode_t *fs, const char *pathname, vfs_fstat_t *buf)
{
    FATFS *fatfs = NULL;
    FILINFO info;
    FRESULT res;

    fatfs = (FATFS *)fs->private;

    res = tos_f_stat(fatfs, pathname, &info);
    if (res != FR_OK) {
        return -1;
    }

    fatfs_translate_filinfo2fstat(&info, buf);

    return 0;
}

static int fatfs_bind(vfs_inode_t *fs, vfs_inode_t *dev)
{
    FATFS *fatfs = NULL;
    FRESULT res;

    fatfs = tos_mmheap_alloc(sizeof(FATFS));
    if (!fatfs) {
        return -1;
    }

    fatfs->pdrv = dev;
    fs->private = (void *)fatfs;

    res = tos_f_mount(fatfs, 1);
    if (res != FR_OK) {
        return -1;
    }

    return 0;
}

static char workbuf[4096];

static int fatfs_mkfs(vfs_inode_t *dev, int opt, unsigned long arg)
{
    FRESULT res;

    res = tos_f_mkfs(dev, opt, arg, workbuf, sizeof(workbuf));
    if (res != FR_OK) {
        return -1;
    }

    return 0;
}

vfs_fs_ops_t fatfs_ops = {
    .open       = fatfs_open,
    .close      = fatfs_close,
    .read       = fatfs_read,
    .write      = fatfs_write,

    .lseek      = fatfs_lseek,
    .truncate   = fatfs_truncate,
    .sync       = fatfs_sync,
    .opendir    = fatfs_opendir,
    .closedir   = fatfs_closedir,
    .readdir    = fatfs_readdir,
    .mkdir      = fatfs_mkdir,
    .unlink     = fatfs_unlink,
    .rename     = fatfs_rename,
    .stat       = fatfs_stat,

    .bind       = fatfs_bind,
    .mkfs       = fatfs_mkfs,
};

