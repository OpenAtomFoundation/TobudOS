#include "tos_k.h"
#include "tos_hal.h"
#include "ff.h"
#include "tos_vfs.h"
#include "tos_fatfs_drv.h"
#include "tos_fatfs_vfs.h"

char buf[512];

void application_entry(void *arg)
{
    int fd, ret;
    extern vfs_blkdev_ops_t sd_dev;
    extern vfs_fs_ops_t fatfs_ops;

    tos_vfs_block_device_register("/dev/sd", &sd_dev);
    tos_vfs_fs_register("fatfs_sd", &fatfs_ops);

    if (tos_vfs_fs_mkfs("/dev/sd", "fatfs_sd", FM_ANY, 0) != 0) {
        printf("mkfs failed!\n");
    }

    if (tos_vfs_fs_mount("/dev/sd", "/fs/fatfs_sd", "fatfs_sd") != 0) {
        printf("mount failed!\n");
    }

    fd = tos_vfs_open("/fs/fatfs_sd/test_file.txt", VFS_OFLAG_CREATE_ALWAYS | VFS_OFLAG_WRITE);
    if (fd < 0) {
        printf("open failed!\n");
    }

    ret = tos_vfs_write(fd, "fatfs sample content", strlen("fatfs sample content"));
    if (ret >= 0) {
        printf("write ok\n");
        printf("write data:\n%s\n", "fatfs sample content");
    } else {
        printf("write error: %d\n", ret);
    }
    ret = tos_vfs_close(fd);
    if (ret < 0) {
        printf("close failed!\n");
    }

    fd = tos_vfs_open("/fs/fatfs_sd/test_file.txt", VFS_OFLAG_EXISTING | VFS_OFLAG_READ);
    if (fd < 0) {
        printf("open file error: %d\n", ret);
    }

    memset(buf, 0, sizeof(buf));
    ret = tos_vfs_read(fd, buf, sizeof(buf));
    if (ret >= 0) {
        printf("read ok: %d\n", ret);
        printf("read data:\n%s\n", buf);
    } else {
        printf("read error: %d\n", ret);
    }

    ////////////////////////////////////////////////
    ret = tos_vfs_lseek(fd, 2, VFS_SEEK_CUR);
    if (ret < 0) {
        printf("lseek error\n");
    }

    memset(buf, 0, sizeof(buf));
    ret = tos_vfs_read(fd, buf, sizeof(buf));
    if (ret >= 0) {
        printf("read ok: %d\n", ret);
        printf("read data:\n%s\n", buf);
    } else {
        printf("read error: %d\n", ret);
    }
    /////////////////////////////////////////////////

    tos_vfs_close(fd);
}

