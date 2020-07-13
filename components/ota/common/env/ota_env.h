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

#ifndef _OTA_ENV_H_
#define  _OTA_ENV_H_

#include "stdint.h"
#include "ota_flash.h"
#include "ota_partition.h"
#include "ota_err.h"

ota_err_t ota_env_init(ota_updt_type_t updt_type, uint32_t partition_addr, ota_flash_drv_t *flash_drv, ota_flash_prop_t *flash_prop);

#endif /* _OTA_ENV_H_ */

