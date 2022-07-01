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
 * @file qcloud_iot_broadcast.h
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-07-18
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-07-18 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_HUB_QCLOUD_IOT_BROADCAST_H_
#define IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_HUB_QCLOUD_IOT_BROADCAST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "qcloud_iot_common.h"

/**
 * @brief Callback when broadcast message arrived.
 *
 */
typedef void (*OnBroadcastArrivedCallback)(void *client, const char *msg, int msg_len, void *usr_data);

/**
 * @brief Subscribe broadcast topic with callback.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] callback callback to handle message
 * @param[in] usr_data usr data using in callback
 * @return @see IotReturnCode
 */
int IOT_Broadcast_Init(void *client, OnBroadcastArrivedCallback callback, void *usr_data);

/**
 * @brief Unsubscribe broadcast topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
int IOT_Broadcast_Deinit(void *client);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_HUB_QCLOUD_IOT_BROADCAST_H_
