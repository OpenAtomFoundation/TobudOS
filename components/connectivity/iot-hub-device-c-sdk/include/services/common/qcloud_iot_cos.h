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
 * @file qcloud_iot_cos.h
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-10-26
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-10-26 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_COS_H_
#define IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_COS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "qcloud_iot_common.h"

/**
 * @brief Cos download params.
 *
 */
typedef struct {
    const char *url;              /**< cos url, user space */
    uint32_t    offset;           /**< download offset */
    uint32_t    file_size;        /**< download file size */
    int         is_fragmentation; /**< http fragmentation support */
    int         is_https_enabled; /**< TODO:https support */
} IotCosDownloadParams;

/**
 * @brief Init cos download handle.
 *
 * @param[in] params @see IotCosDownloadParams
 * @return pointer to cos download handle
 */
void *IOT_COS_DownloadInit(IotCosDownloadParams *params);

/**
 * @brief Fetch data from cos.
 *
 * @param[in,out] handle pointer to cos download handle, @see HTTPCosDownloadHandle
 * @param[out] buf buffer to store data
 * @param[in] buf_len buffer length
 * @param timeout_ms timeout for fetching
 * @return >= 0 for recv data len. others @see IotReturnCode
 */
int IOT_COS_DownloadFetch(void *handle, uint8_t *buf, uint32_t buf_len, uint32_t timeout_ms);

/**
 * @brief Is download finished.
 *
 * @param[in,out] handle pointer to cos download handle, @see HTTPCosDownloadHandle
 * @return true for finished
 */
int IOT_COS_DownloadIsFinished(void *handle);

/**
 * @brief Deinit cos download.
 *
 * @param[in,out] handle pointer to cos download handle, @see HTTPCosDownloadHandle
 */
void IOT_COS_DownloadDeinit(void *handle);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_COS_H_
