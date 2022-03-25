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
 * @file qcloud_iot_hub.h
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-28
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-28 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2021-07-18 <td>1.1     <td>fancyxu   <td>support broadcast
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_INCLUDE_QCLOUD_IOT_HUB_H_
#define IOT_HUB_DEVICE_C_SDK_INCLUDE_QCLOUD_IOT_HUB_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief IoT C-SDK version info
 *
 */
#define QCLOUD_IOT_DEVICE_HUB_SDK_VERSION "4.0.0"

// service header file
#include "qcloud_iot_broadcast.h"

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_INCLUDE_QCLOUD_IOT_HUB_H_
