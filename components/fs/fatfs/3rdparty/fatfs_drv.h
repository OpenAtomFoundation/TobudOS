#ifndef _FATFS_DRV_H_
#define _FATFS_DRV_H_

#include "tos_hal.h"

int MMC_disk_initialize(void);

int MMC_disk_read(uint8_t *buff, uint32_t sector, uint32_t count);

int MMC_disk_write(const uint8_t *buff, uint32_t sector, uint32_t count);

int MMC_disk_status(hal_sd_state_t *sd_state);

int MMC_disk_ioctl(BYTE cmd, void *buff);

#endif /*_FATFS_DRV_H_*/
