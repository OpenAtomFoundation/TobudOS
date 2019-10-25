/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef _TOS_HAL_SD_H_
#define  _TOS_HAL_SD_H_

typedef enum hal_sd_state_en {
    HAL_SD_STAT_RESET,         /*< not yet initialized or disabled */
    HAL_SD_STAT_READY,         /*< initialized and ready for use   */
    HAL_SD_STAT_TIMEOUT,       /*< timeout state                   */
    HAL_SD_STAT_BUSY,          /*< process ongoing                 */
    HAL_SD_STAT_PROGRAMMING,   /*< programming state               */
    HAL_SD_STAT_RECEIVING,     /*< receinving state                */
    HAL_SD_STAT_TRANSFER,      /*< transfert state                 */
    HAL_SD_STAT_ERROR,         /*< error state                     */
} hal_sd_state_t;

typedef struct hal_sd_info_st {
    uint32_t card_type;             /*< card type                       */
    uint32_t card_version;          /*< card version                    */
    uint32_t class;                 /*< card class                      */
    uint32_t relative_card_addr;    /*< relative card address           */
    uint32_t blk_num;               /*< card capacity in blocks         */
    uint32_t blk_size;              /*< one block size in bytes         */
    uint32_t logical_blk_num;       /*< card logical capacity in blocks */
    uint32_t logical_blk_size;      /*< logical block size in bytes     */
} hal_sd_info_t;

typedef struct hal_sd_st {
    void           *private_sd;
} hal_sd_t;

__API__ int tos_hal_sd_init(hal_sd_t *sd);

__API__ int tos_hal_sd_read(hal_sd_t *sd, uint8_t *buf, uint32_t blk_addr, uint32_t blk_num, uint32_t timeout);

__API__ int tos_hal_sd_write(hal_sd_t *sd, const uint8_t *buf, uint32_t blk_addr, uint32_t blk_num, uint32_t timeout);

__API__ int tos_hal_sd_read_dma(hal_sd_t *sd, uint8_t *buf, uint32_t blk_addr, uint32_t blk_num);

__API__ int tos_hal_sd_write_dma(hal_sd_t *sd, const uint8_t *buf, uint32_t blk_addr, uint32_t blk_num);

__API__ int tos_hal_sd_erase(hal_sd_t *sd, uint32_t blk_add_start, uint32_t blk_addr_end);

__API__ int tos_hal_sd_info_get(hal_sd_t *sd, hal_sd_info_t *info);

__API__ int tos_hal_sd_state_get(hal_sd_t *sd, hal_sd_state_t *state);

__API__ int tos_hal_sd_deinit(hal_sd_t *sd);

#endif

