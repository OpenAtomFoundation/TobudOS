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
 * @file utils_md5.h
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-10-20
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-10-20 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_COMMON_CRYPTOLOGY_INC_UTILS_MD5_H_
#define IOT_HUB_DEVICE_C_SDK_COMMON_CRYPTOLOGY_INC_UTILS_MD5_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "qcloud_iot_config.h"

#ifdef AUTH_WITH_NO_TLS

typedef struct {
    uint32_t total[2];   /**< number of bytes processed  */
    uint32_t state[4];   /**< intermediate digest state  */
    uint8_t  buffer[64]; /**< data block being processed */
    char     md5sum[33]; /**< md5sum result in hex string */
} IotMd5Context;

#else
#include "mbedtls/md5.h"
typedef struct {
    mbedtls_md5_context ctx;
    char                md5sum[33]; /**< md5sum result in hex string */
} IotMd5Context;
#endif

/**
 * @brief Reset MD5 context.
 *
 * @param[in,out] ctx MD5 context
 */
void utils_md5_reset(IotMd5Context *ctx);

/**
 * @brief MD5 update.
 *
 * @param[in,out] ctx MD5 context
 * @param[in] input input data
 * @param[in] ilen data length
 */
void utils_md5_update(IotMd5Context *ctx, const uint8_t *input, size_t ilen);

/**
 * @brief Finish MD5 calculation, result will store in md5sum.
 *
 * @param[in,out] ctx MD5 context
 */
void utils_md5_finish(IotMd5Context *ctx);

/**
 * @brief Compare md5sum with context.
 *
 * @param[in,out] ctx MD5 context
 * @param[in] md5sum md5sum to compare
 * @return 0 for the same
 */
int utils_md5_compare(IotMd5Context *ctx, const char md5sum[33]);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_COMMON_CRYPTOLOGY_INC_UTILS_MD5_H_
