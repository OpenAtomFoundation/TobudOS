/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef QLCOUD_IOT_EXPORT_DYNREG_H_ 
#define QLCOUD_IOT_EXPORT_DYNREG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_export.h"

/**
 * @brief Do dynamic register/create device 
 *
 * @param pDevInfo  In:     device info with [ProductId, ProductKey, DeviceName]
 *                  Out:    device info with [ProductId, DeviceName, DeviceSecret or Device cert/key file]
 *
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int IOT_DynReg_Device(DeviceInfo *pDevInfo);


#ifdef __cplusplus
}
#endif

#endif //QLCOUD_IOT_EXPORT_DYNREG_H_