/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2021 THL A29 Limited, a Tencent company.All rights reserved.
 *
 * Licensed under the MIT License(the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file qcloud_iot_system.h
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-07-24
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-07-24 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2022-01-26 <td>1.1     <td>hubert    <td>add serverip
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_SYSTEM_H_
#define IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_SYSTEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "qcloud_iot_common.h"

#include "utils_json.h"

/**
 * @brief Get time from system topic
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] time time from system topic
 * @return @see IotReturnCode
 */
int IOT_Sys_GetTime(void *client, uint32_t *time);

/**
 * @brief Get ntp time and set to system.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
int IOT_Sys_SyncNTPTime(void *client);

/**
 * @brief Get serverip from system result topic
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] server_ip serverip from system result topic
 * @return @see IotReturnCode
 */
int IOT_Sys_GetServerIp(void *client, char *server_ip);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_SYSTEM_H_
