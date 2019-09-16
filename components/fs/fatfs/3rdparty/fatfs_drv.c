#include "ff.h"
#include "diskio.h"
#include "tos_hal.h"

static hal_sd_t sd;
static hal_sd_info_t sd_info;

DWORD get_fattime(void)
{
    return 0;
}

int MMC_disk_initialize(void)
{
    if (tos_hal_sd_init(&sd) != 0) {
        return -1;
    }
    return tos_hal_sd_info_get(&sd, &sd_info);
}

static uint8_t sdio_aligned_buffer[512] __ALIGNED__(4);

int MMC_disk_read(uint8_t *buff, uint32_t sector, uint32_t count)
{
    TOS_CPU_CPSR_ALLOC();
    int ret = 0;
    uint32_t i;
    uint64_t sector_addr = sector << 9;

    TOS_CPU_INT_DISABLE();
    if ((cpu_addr_t)buff % 4 != 0) {
        for (i = 0; i < count; ++i) {
            ret = tos_hal_sd_read(&sd, sdio_aligned_buffer, sector_addr + 512 * i, 1, 2000);
            memcpy(buff, sdio_aligned_buffer, 512);
            buff += 512;
        }
    } else {
        ret = tos_hal_sd_read(&sd, buff, sector_addr, count, 2000);
    }
    TOS_CPU_INT_ENABLE();
    return ret;
}

int MMC_disk_write(const uint8_t *buff, uint32_t sector, uint32_t count)
{
    TOS_CPU_CPSR_ALLOC();
    int ret = 0;
    uint32_t i;
    uint64_t sector_addr = sector << 9;

    TOS_CPU_INT_DISABLE();
    if ((cpu_addr_t)buff % 4 !=0) {
        for (i = 0; i < count; ++i) {
            memcpy(sdio_aligned_buffer, buff, 512);
            ret = tos_hal_sd_write(&sd, sdio_aligned_buffer, sector_addr + 512 * i, 1, 2000);
            buff += 512;
        }
    } else {
        ret = tos_hal_sd_write(&sd, buff, sector_addr, count, 2000);
    }
    TOS_CPU_INT_ENABLE();
    return ret;
}

int MMC_disk_status(hal_sd_state_t *sd_state)
{
    return tos_hal_sd_state_get(&sd, sd_state);
}

int MMC_disk_ioctl(BYTE cmd, void *buff)
{
    int ret = 0;

    if (cmd != CTRL_SYNC && !buff) {
        return -1;
    }

    switch (cmd) {
        case CTRL_SYNC:
            break;

        case GET_SECTOR_SIZE:
            *(DWORD *)buff = sd_info.blk_size;
            break;

        case GET_BLOCK_SIZE:
            *(WORD *)buff = sd_info.blk_size;
            break;

        case GET_SECTOR_COUNT:
            *(DWORD *)buff = sd_info.blk_num;
            break;

        default:
            ret = -1;
            break;
    }

    return ret;
}

