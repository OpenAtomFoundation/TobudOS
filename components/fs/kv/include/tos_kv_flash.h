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

#ifndef _TOS_KV_FLASH_H_
#define  _TOS_KV_FLASH_H_

typedef enum kv_flash_program_type_en {
    KV_FLASH_PROGRAM_TYPE_BYTE,         /*!< Program byte (8-bit) at a specified address           */
    KV_FLASH_PROGRAM_TYPE_HALFWORD,     /*!< Program a half-word (16-bit) at a specified address   */
    KV_FLASH_PROGRAM_TYPE_WORD,         /*!< Program a word (32-bit) at a specified address        */
    KV_FLASH_PROGRAM_TYPE_DOUBLEWORD,   /*!< Program a double word (64-bit) at a specified address */
} kv_flash_pgm_type_t;

typedef int (*kv_flash_write_t)(uint32_t addr, const void *buf, size_t len);
typedef int (*kv_flash_read_t)(uint32_t addr, void *buf, size_t len);
typedef int (*kv_flash_erase_t)(uint32_t addr, size_t len);

typedef struct kv_flash_drv_st {
    kv_flash_write_t    write;
    kv_flash_read_t     read;
    kv_flash_erase_t    erase;
} kv_flash_drv_t;

typedef struct kv_flash_property_st {
    uint8_t             sector_size_log2;
    kv_flash_pgm_type_t pgm_type;
    uint32_t            flash_start;
    uint32_t            flash_size;
} kv_flash_prop_t;

#endif /* _TOS_KV_FLASH_H_ */

