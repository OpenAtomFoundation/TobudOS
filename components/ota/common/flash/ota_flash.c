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

#include "ota_flash.h"

static ota_flash_ctl_t flash_ctl;

int ota_flash_init(ota_flash_drv_t *flash_drv, ota_flash_prop_t *flash_prop)
{
    if (!flash_drv || !flash_prop) {
        return -1;
    }

    memcpy(&flash_ctl.flash_drv, flash_drv, sizeof(ota_flash_drv_t));

    OTA_FLASH_SECTOR_SIZE_LOG2  = flash_prop->sector_size_log2;

    if (flash_prop->pgm_type == OTA_FLASH_PROGRAM_TYPE_BYTE) {
        OTA_FLASH_WRITE_ALIGN   = sizeof(ota_byte_t);
    } else if (flash_prop->pgm_type == OTA_FLASH_PROGRAM_TYPE_HALFWORD) {
        OTA_FLASH_WRITE_ALIGN   = sizeof(ota_hword_t);
    } else if (flash_prop->pgm_type == OTA_FLASH_PROGRAM_TYPE_WORD) {
        OTA_FLASH_WRITE_ALIGN   = sizeof(ota_word_t);
    } else if (flash_prop->pgm_type == OTA_FLASH_PROGRAM_TYPE_DOUBLEWORD) {
        OTA_FLASH_WRITE_ALIGN   = sizeof(ota_dword_t);
    }

    return 0;
}

int ota_flash_read(uint32_t addr, void *buf, size_t buf_size)
{
    if (OTA_FLASH_READ(addr, buf, buf_size) < 0) {
        return -1;
    }
    return 0;
}

int ota_flash_write(uint32_t addr, void *buf, size_t buf_size)
{
    if (OTA_FLASH_WRITE(addr, buf, buf_size) < 0) {
        return -1;
    }
    return 0;
}

int ota_flash_erase(uint32_t start, size_t len)
{
    if (OTA_FLASH_ERASE(start, len) < 0) {
        return -1;
    }
    return 0;
}

int ota_flash_erase_blocks(uint32_t start, size_t len)
{
    return ota_flash_erase(start, OTA_ALIGN_UP(len, OTA_FLASH_SECTOR_SIZE));
}

size_t ota_flash_write_size_aligned_get(size_t len)
{
    return OTA_ALIGN_UP(len, OTA_FLASH_WRITE_ALIGN);
}

int ota_flash_size_is_aligned(size_t len)
{
    return OTA_IS_ALINGED(len, OTA_FLASH_WRITE_ALIGN);
}

