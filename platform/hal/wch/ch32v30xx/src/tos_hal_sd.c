#include "tos_k.h"
#include "tos_hal.h"
#include "ch32v30x.h"
#include "sdio.h"

__API__ int tos_hal_sd_init(hal_sd_t *sd)
{
    (void)sd;

    return SD_Init() == SD_OK ? 0 : -1;
}

__API__ int tos_hal_sd_read(hal_sd_t *sd, uint8_t *buf, uint32_t blk_addr, uint32_t blk_num, uint32_t timeout)
{
    uint8_t err;

    (void)sd;

    if (!buf) {
        return -1;
    }

    err = SD_ReadDisk(buf, blk_addr, blk_num);

    return err == SD_OK ? 0 : -1;
}

__API__ int tos_hal_sd_write(hal_sd_t *sd, const uint8_t *buf, uint32_t blk_addr, uint32_t blk_num, uint32_t timeout)
{
    uint8_t err;

    (void)sd;

    if (!buf) {
        return -1;
    }

    err = SD_WriteDisk((uint8_t *)buf, blk_addr, blk_num);

    return err == SD_OK ? 0 : -1;
}

__API__ int tos_hal_sd_read_dma(hal_sd_t *sd, uint8_t *buf, uint32_t blk_addr, uint32_t blk_num)
{
    return -1;
}

__API__ int tos_hal_sd_write_dma(hal_sd_t *sd, const uint8_t *buf, uint32_t blk_addr, uint32_t blk_num)
{
    return -1;
}

__API__ int tos_hal_sd_erase(hal_sd_t *sd, uint32_t blk_add_start, uint32_t blk_addr_end)
{
    return 0;
}

__API__ int tos_hal_sd_info_get(hal_sd_t *sd, hal_sd_info_t *info)
{
    SD_Error err;
    SD_CardInfo card_info;

    (void)sd;

    if (!sd || !info) {
        return -1;
    }

    err = SD_GetCardInfo(&card_info);
    if (err != SD_OK) {
        return -1;
    }

    info->card_type             = card_info.CardType;
    info->blk_num               = card_info.CardCapacity / card_info.CardBlockSize;
    info->blk_size              = card_info.CardBlockSize;

    return 0;
}

__API__ int tos_hal_sd_state_get(hal_sd_t *sd, hal_sd_state_t *state)
{
    int ret = 0;
    SDCardState sd_state;

    (void)sd;

    if (!sd || !state) {
        return -1;
    }

    sd_state = SD_GetState();
    switch (sd_state) {
        case SD_CARD_READY:
            *state = HAL_SD_STAT_READY;
            break;

        case SD_CARD_SENDING:
            *state = HAL_SD_STAT_PROGRAMMING;
            break;

        case SD_CARD_RECEIVING:
            *state = HAL_SD_STAT_RECEIVING;
            break;

        case SD_CARD_TRANSFER:
            *state = HAL_SD_STAT_TRANSFER;
            break;

        case SD_CARD_ERROR:
            *state = HAL_SD_STAT_ERROR;
            break;

        default:
            ret = -1;
            break;
    }
    return ret;
}

__API__ int tos_hal_sd_deinit(hal_sd_t *sd)
{
    (void)sd;
    return 0;
}

