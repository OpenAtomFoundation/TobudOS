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

#ifndef _OTA_ERR_H_
#define  _OTA_ERR_H_

typedef enum ota_err_en {
    OTA_ERR_NONE = 0u,
    
    OTA_ERR_FLASH_INIT_FAIL = 1u,
    
    OTA_ERR_PARTITION_READ_FAIL = 2u,
    OTA_ERR_PARTITION_MAGIC_NOT_SAME,
    OTA_ERR_PARTITION_ADDR_FAIL,
    OTA_ERR_PARTITION_NOT_ALIGN,
    OTA_ERR_PARTITION_CRC_FAIL,
    
    OTA_ERR_UPDT_TYPE_UNKOWN,
    
    OTA_ERR_KV_FAIL
} ota_err_t;

#endif /* _OTA_ERR_H_ */

