#include "tos_vfs.h"
#include "tos_hal.h"
#include "tos_ff.h"
#include "tos_diskio.h"

DWORD tos_get_fattime(void)
{
    return 0;
}

static hal_sd_t sd;
static hal_sd_info_t sd_info;

static int sd_open(vfs_inode_t *dev)
{
    if (tos_hal_sd_init(&sd) != 0) {
        return -1;
    }
    return tos_hal_sd_info_get(&sd, &sd_info);
}

static int sd_close(vfs_inode_t *dev)
{
    return tos_hal_sd_deinit(&sd);
}

static uint8_t sdio_aligned_buffer[512] __ALIGNED__(4);

static ssize_t sd_read(vfs_inode_t *dev, void *buf, size_t start_sector, unsigned int nsectors)
{
    TOS_CPU_CPSR_ALLOC();
    int rc = 0;
    uint32_t i;
    uint8_t *buff = (uint8_t *)buf;
    uint64_t sector_addr = start_sector << 9;

    TOS_CPU_INT_DISABLE();
    if ((cpu_addr_t)buff % 4 != 0) {
        for (i = 0; i < nsectors; ++i) {
            rc = tos_hal_sd_read(&sd, sdio_aligned_buffer, sector_addr + 512 * i, 1, 2000);
            memcpy(buff, sdio_aligned_buffer, 512);
            buff += 512;
        }
    } else {
        rc = tos_hal_sd_read(&sd, buff, sector_addr, nsectors, 2000);
    }
    TOS_CPU_INT_ENABLE();
    return rc;
}

static ssize_t sd_write(vfs_inode_t *dev, const unsigned char *buf, size_t start_sector, unsigned int nsectors)
{
    TOS_CPU_CPSR_ALLOC();
    int rc = 0;
    uint32_t i;
    uint8_t *buff = (uint8_t *)buf;
    uint64_t sector_addr = start_sector << 9;

    TOS_CPU_INT_DISABLE();
    if ((cpu_addr_t)buff % 4 != 0) {
        for (i = 0; i < nsectors; ++i) {
            memcpy(sdio_aligned_buffer, buff, 512);
            rc = tos_hal_sd_write(&sd, sdio_aligned_buffer, sector_addr + 512 * i, 1, 2000);
            buff += 512;
        }
    } else {
        rc = tos_hal_sd_write(&sd, buff, sector_addr, nsectors, 2000);
    }
    TOS_CPU_INT_ENABLE();
    return rc;
}

static int sd_ioctl(vfs_inode_t *dev, int cmd, unsigned long arg)
{
    int rc = 0;
    void *buff = (void *)arg;

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
            rc = -1;
            break;
    }

    return rc;
}

static int sd_geometry(vfs_inode_t *dev, vfs_blkdev_geo_t *geo)
{
    hal_sd_info_t sd_info;
    hal_sd_state_t sd_state;

    if (!dev || !geo) {
        return -1;
    }

    if (tos_hal_sd_info_get(&sd, &sd_info) != 0) {
        return -1;
    }

    if (tos_hal_sd_state_get(&sd, &sd_state) != 0) {
        return -1;
    }

    geo->is_available   = (sd_state != HAL_SD_STAT_ERROR);
    geo->sector_size    = sd_info.blk_size;
    geo->nsectors       = sd_info.blk_num;
    return 0;
}


vfs_blkdev_ops_t sd_dev = {
    .open       = sd_open,
    .close      = sd_close,
    .read       = sd_read,
    .write      = sd_write,
    .ioctl      = sd_ioctl,
    .geometry   = sd_geometry,
};

