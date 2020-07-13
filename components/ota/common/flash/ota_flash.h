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

#ifndef _OTA_FLASH_H_
#define  _OTA_FLASH_H_

#include "stdint.h"
#include "stdlib.h"
#include "string.h"

#define OTA_FLASH_SECTOR_SIZE_LOG2                  (flash_ctl.sector_size_log2)
#define OTA_FLASH_SECTOR_SIZE                       (1 << OTA_FLASH_SECTOR_SIZE_LOG2)
#define OTA_FLASH_WRITE_ALIGN                       (flash_ctl.flash_write_align)

#define OTA_FLASH_WRITE                             ((ota_flash_write_t)(flash_ctl.flash_drv.write))
#define OTA_FLASH_READ                              ((ota_flash_read_t)(flash_ctl.flash_drv.read))
#define OTA_FLASH_ERASE                             ((ota_flash_erase_t)(flash_ctl.flash_drv.erase))

#define OTA_ALIGN_UP(v, align)                      (((v) + ((align) - 1)) & ~((align) - 1))
#define OTA_IS_ALINGED(v, align)                    ((v) % (align) == 0)

typedef uint8_t     ota_byte_t;         // byte
typedef uint16_t    ota_hword_t;        // half word
typedef uint32_t    ota_word_t;         // word
typedef uint64_t    ota_dword_t;        // double word

typedef enum ota_flash_program_type_en {
    OTA_FLASH_PROGRAM_TYPE_BYTE,            /*!< Program byte (8-bit) at a specified address           */
    OTA_FLASH_PROGRAM_TYPE_HALFWORD,        /*!< Program a half-word (16-bit) at a specified address   */
    OTA_FLASH_PROGRAM_TYPE_WORD,            /*!< Program a word (32-bit) at a specified address        */
    OTA_FLASH_PROGRAM_TYPE_DOUBLEWORD,      /*!< Program a double word (64-bit) at a specified address */
} ota_flash_pgm_type_t;

typedef int (*ota_flash_write_t)(uint32_t addr, const void *buf, size_t len);
typedef int (*ota_flash_read_t)(uint32_t addr, void *buf, size_t len);
typedef int (*ota_flash_erase_t)(uint32_t addr, size_t len);

typedef struct ota_flash_drv_st {
    ota_flash_write_t       write;
    ota_flash_read_t        read;
    ota_flash_erase_t       erase;
} ota_flash_drv_t;

typedef struct ota_flash_property_st {
    uint8_t                 sector_size_log2;
    ota_flash_pgm_type_t    pgm_type;
} ota_flash_prop_t;

typedef struct ota_flash_control_st {
    uint8_t         sector_size_log2;
    uint8_t         flash_write_align;

    ota_flash_drv_t flash_drv;
} ota_flash_ctl_t;

int ota_flash_init(ota_flash_drv_t *flash_drv, ota_flash_prop_t *flash_prop);

int ota_flash_read(uint32_t addr, void *buf, size_t buf_size);

int ota_flash_write(uint32_t addr, void *buf, size_t buf_size);

int ota_flash_erase(uint32_t start, size_t len);

int ota_flash_erase_blocks(uint32_t start, size_t len);

size_t ota_flash_write_size_aligned_get(size_t len);

int ota_flash_size_is_aligned(size_t len);

#endif /* _OTA_FLASH_H_ */

