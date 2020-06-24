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

#ifndef _OTA_RECOVERY_H_
#define  _OTA_RECOVERY_H_

#include "ota_env.h"
#include "ota_info.h"
#include "ota_flash.h"
#include "ota_image.h"
#include "ota_patch.h"
#include "ota_err.h"

ota_err_t ota_recovery(void);

int ota_recovery_xip(void);

#endif /* _OTA_RECOVERY_H_ */

