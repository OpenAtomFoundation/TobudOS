#include "tos_k.h"
#include "tos_hal.h"
#include "stm32f4xx_hal.h"
#include "sdio.h"

__API__ int tos_hal_sd_init(hal_sd_t *sd)
{
    if (!sd) {
        return -1;
    }

    sd->private_sd = &hsd;
    MX_SDIO_SD_Init();
    return 0;
}

__API__ int tos_hal_sd_read(hal_sd_t *sd, uint8_t *buf, uint32_t blk_addr, uint32_t blk_num, uint32_t timeout)
{
    HAL_StatusTypeDef hal_status;
    SD_HandleTypeDef *sd_handle;

    if (!sd || !buf) {
        return -1;
    }

    if (!sd->private_sd) {
        return -1;
    }

    sd_handle = sd->private_sd;
    hal_status = HAL_SD_ReadBlocks(sd_handle, buf, blk_addr, blk_num, timeout);
    if (hal_status != HAL_OK) {
        return -1;
    }

    while (HAL_SD_GetCardState(sd_handle) != HAL_SD_CARD_TRANSFER) {
        ;
    }
    return 0;
}

__API__ int tos_hal_sd_write(hal_sd_t *sd, const uint8_t *buf, uint32_t blk_addr, uint32_t blk_num, uint32_t timeout)
{
    HAL_StatusTypeDef hal_status;
    SD_HandleTypeDef *sd_handle;

    if (!sd || !buf) {
        return -1;
    }

    if (!sd->private_sd) {
        return -1;
    }

    sd_handle = sd->private_sd;
    hal_status = HAL_SD_WriteBlocks(sd_handle, (uint8_t *)buf, blk_addr, blk_num, timeout);
    if (hal_status != HAL_OK) {
        return -1;
    }

    while (HAL_SD_GetCardState(sd_handle) != HAL_SD_CARD_TRANSFER) {
        ;
    }
    return 0;
}

__API__ int tos_hal_sd_read_dma(hal_sd_t *sd, uint8_t *buf, uint32_t blk_addr, uint32_t blk_num)
{
    HAL_StatusTypeDef hal_status;
    SD_HandleTypeDef *sd_handle;

    if (!sd || !buf) {
        return -1;
    }

    if (!sd->private_sd) {
        return -1;
    }

    sd_handle = sd->private_sd;
    hal_status = HAL_SD_ReadBlocks_DMA(sd_handle, buf, blk_addr, blk_num);
    if (hal_status != HAL_OK) {
        return -1;
    }
    return 0;
}

__API__ int tos_hal_sd_write_dma(hal_sd_t *sd, const uint8_t *buf, uint32_t blk_addr, uint32_t blk_num)
{
    HAL_StatusTypeDef hal_status;
    SD_HandleTypeDef *sd_handle;

    if (!sd || !buf) {
        return -1;
    }

    if (!sd->private_sd) {
        return -1;
    }

    sd_handle = sd->private_sd;
    hal_status = HAL_SD_WriteBlocks_DMA(sd_handle, (uint8_t *)buf, blk_addr, blk_num);
    if (hal_status != HAL_OK) {
        return -1;
    }
    return 0;
}

__API__ int tos_hal_sd_erase(hal_sd_t *sd, uint32_t blk_add_start, uint32_t blk_addr_end)
{
    HAL_StatusTypeDef hal_status;
    SD_HandleTypeDef *sd_handle;

    if (!sd || !sd->private_sd) {
        return -1;
    }

    sd_handle = sd->private_sd;
    hal_status = HAL_SD_Erase(sd_handle, blk_add_start, blk_addr_end);
    if (hal_status != HAL_OK) {
        return -1;
    }

    while (HAL_SD_GetCardState(sd_handle) != HAL_SD_CARD_TRANSFER) {
        ;
    }
    return 0;
}

__API__ int tos_hal_sd_info_get(hal_sd_t *sd, hal_sd_info_t *info)
{
    HAL_StatusTypeDef hal_status;
    SD_HandleTypeDef *sd_handle;
    HAL_SD_CardInfoTypeDef hal_card_info;

    if (!sd || !info) {
        return -1;
    }

    if (!sd->private_sd) {
        return -1;
    }

    sd_handle = sd->private_sd;

    hal_status = HAL_SD_GetCardInfo(sd_handle, &hal_card_info);
    if (hal_status != HAL_OK) {
        return -1;
    }

    info->card_type             = hal_card_info.CardType;
    info->card_version          = hal_card_info.CardVersion;
    info->class                 = hal_card_info.Class;
    info->relative_card_addr    = hal_card_info.RelCardAdd;
    info->blk_num               = hal_card_info.BlockNbr;
    info->blk_size              = hal_card_info.BlockSize;
    info->logical_blk_num       = hal_card_info.LogBlockNbr;
    info->logical_blk_size      = hal_card_info.LogBlockSize;

    return 0;
}

__API__ int tos_hal_sd_state_get(hal_sd_t *sd, hal_sd_state_t *state)
{
    int ret = 0;
    SD_HandleTypeDef *sd_handle;
    HAL_SD_StateTypeDef sd_state;

    if (!sd || !state) {
        return -1;
    }

    if (!sd->private_sd) {
        return -1;
    }

    sd_handle = sd->private_sd;

    sd_state = HAL_SD_GetState(sd_handle);
    switch (sd_state) {
        case HAL_SD_STATE_RESET:
            *state = HAL_SD_STAT_RESET;
            break;

        case HAL_SD_STATE_READY:
            *state = HAL_SD_STAT_READY;
            break;

        case HAL_SD_STATE_TIMEOUT:
            *state = HAL_SD_STAT_TIMEOUT;
            break;

        case HAL_SD_STATE_BUSY:
            *state = HAL_SD_STAT_BUSY;
            break;

        case HAL_SD_STATE_PROGRAMMING:
            *state = HAL_SD_STAT_PROGRAMMING;
            break;

        case HAL_SD_STATE_RECEIVING:
            *state = HAL_SD_STAT_RECEIVING;
            break;

        case HAL_SD_STATE_TRANSFER:
            *state = HAL_SD_STAT_TRANSFER;
            break;

        case HAL_SD_STATE_ERROR:
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
    HAL_StatusTypeDef hal_status;
    SD_HandleTypeDef *sd_handle;

    if (!sd || !sd->private_sd) {
        return -1;
    }

    sd_handle = sd->private_sd;
    hal_status = HAL_SD_DeInit(sd_handle);
    if (hal_status != HAL_OK) {
        return -1;
    }
    return 0;
}

