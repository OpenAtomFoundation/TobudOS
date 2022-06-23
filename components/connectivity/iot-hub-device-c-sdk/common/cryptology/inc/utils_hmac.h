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
 * @file utils_hmac.h
 * @brief header file for utils_hmac
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-31
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-31 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_COMMON_CRYPTOLOGY_INC_UTILS_HMAC_H_
#define IOT_HUB_DEVICE_C_SDK_COMMON_CRYPTOLOGY_INC_UTILS_HMAC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "utils_sha1.h"

/**
 * @brief Sha1 digest size.
 *
 */
#define SHA1_DIGEST_SIZE 20

/**
 * @brief Get digest of hmac-sha1.
 *
 * @param[in] msg message to hmac-sha1
 * @param[in] msg_len message len
 * @param[in] key key using in hmac-sha1
 * @param[in] key_len key len
 * @param[out] digest digest to calculate
 * @return 0 for success
 */
int utils_hmac_sha1(const char *msg, int msg_len, const uint8_t *key, int key_len, char *digest);

/**
 * @brief Get digest hex of hmac-sha1. Output hexstr format.
 *
 * @param[in] msg message to hmac-sha1
 * @param[in] msg_len message len
 * @param[in] key key using in hmac-sha1
 * @param[in] key_len key len
 * @param[out] digest digest to calculate
 * @return 0 for success
 */
int utils_hmac_sha1_hex(const char *msg, int msg_len, const uint8_t *key, int key_len, char *digest);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_COMMON_CRYPTOLOGY_INC_UTILS_HMAC_H_
