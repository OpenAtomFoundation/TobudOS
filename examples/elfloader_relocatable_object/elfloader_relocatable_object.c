#include "tos_k.h"
#include "tos_hal.h"
#include "ff.h"
#include "tos_vfs.h"
#include "tos_fatfs_drv.h"
#include "tos_fatfs_vfs.h"

#include "tos_elfloader.h"

int d_e_a = 9;

int f_e_a(int a)
{
    /* a = d_e_a + d_g_a = d_e_a + 15 = 24 */
    printf("f_e_a:  %d\n", a);
    return 0;
}

const el_symbol_t el_symbols[] = {
    { "d_e_a", &d_e_a },
    { "f_e_a", f_e_a },
    { K_NULL, K_NULL },
};

/*
 step1: create 1.c

 extern int d_e_a;

 int d_g_a = 3;

 static int d_s_a = 5;

 extern int f_e_a(int);

 static int f_s_a(void) {
     d_s_a += 7; // d_s_a = 12
 }

 int f_g_a(void) {
     f_s_a();

     d_g_a += d_s_a; // d_g_a = 15

     d_e_a += d_g_a; // + 15

     f_e_a(d_e_a);
 }

 step2: compile 1.c to 1.o
 arm-linux-gnueabihf-gcc -fno-builtin -nostdlib -mthumb -mthumb-interwork -mcpu=cortex-m4 -c 1.c -o 1.o
 */

void application_entry(void *arg)
{
    int fd;
    el_module_t module;

    extern vfs_blkdev_ops_t sd_dev;
    extern vfs_fs_ops_t fatfs_ops;

    if (tos_vfs_block_device_register("/dev/sd", &sd_dev) != VFS_ERR_NONE) {
        return;
    }

    if (tos_vfs_fs_register("fatfs_sd", &fatfs_ops) != VFS_ERR_NONE) {
        return;
    }

    if (tos_vfs_fs_mount("/dev/sd", "/fs/fatfs_sd", "fatfs_sd") != VFS_ERR_NONE) {
        printf("mount failed!\n");
        return;
    }

    fd = tos_vfs_open("/fs/fatfs_sd/1.o", VFS_OFLAG_READ | VFS_OFLAG_EXISTING);
    if (fd < 0) {
        return;
    }

    if (tos_elfloader_load(&module, fd) != ELFLOADER_ERR_NONE) {
        return;
    }

    void *addr = tos_elfloader_find_symbol(&module, "f_g_a");
    if (!addr) {
        printf("symbol NOT FOUND: %s\n", "f_g_a");
        return;
    }

    printf("addr: %x\n", addr);

    typedef int (*fp_t)(void);
    /* call f_g_a in 1.o */
    ((fp_t)addr)();

    tos_elfloader_unload(&module);

    tos_vfs_close(fd);
}

