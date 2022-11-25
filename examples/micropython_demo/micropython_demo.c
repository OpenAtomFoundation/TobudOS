#include "tos_k.h"
#include "mpmain.h"
#include "mpconfigboard.h"

#ifdef MP_USING_VFS
#include "tos_vfs.h"
#include "ff.h"
#endif /* MP_USING_VFS */

#ifdef MP_USING_QSPI_FLASH
#include "w25qxx.h"
#endif /* MP_USING_QSPI_FLASH */

#define MP_TASK_PRIO        (3)
#ifndef MP_TASK_STACK_SIZE
#define MP_TASK_STACK_SIZE  (4 * 1024)
#endif

// MicroPython task stack
__STATIC__ k_stack_t mp_task_stack[MP_TASK_STACK_SIZE];
__STATIC__ k_task_t mp_task;

void mp_entry(void *arg) {
  
    #ifdef MP_USING_VFS
    extern vfs_blkdev_ops_t sd_dev;
    extern vfs_fs_ops_t fatfs_ops;

    tos_vfs_block_device_register("/dev/sd", &sd_dev);
    tos_vfs_fs_register("fatfs_sd", &fatfs_ops);

    if (tos_vfs_fs_mount("/dev/sd", "/fs", "fatfs_sd") != 0) {
        printf("mount failed!\n");
    }
    #endif /* MP_USING_VFS */
    
    for (;;) {
        mp_main();
        tos_task_delay(100);
    }
}

void application_entry(void *arg) {

    #ifdef MP_USING_QSPI_FLASH
    w25qxx_init();
    w25qxx_memory_mapped();
    #endif /* MP_USING_QSPI_FLASH */

    tos_task_create(&mp_task, "micropython", mp_entry,
                        K_NULL, MP_TASK_PRIO, mp_task_stack,
                        MP_TASK_STACK_SIZE, 0);
}
