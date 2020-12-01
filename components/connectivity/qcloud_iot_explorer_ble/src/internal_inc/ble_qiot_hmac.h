/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef QCLOUD_BLE_QIOT_HMAC_H
#define QCLOUD_BLE_QIOT_HMAC_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <string.h>

#define SHA1_DIGEST_SIZE 20

void utils_hmac_sha1(const char *msg, int msg_len, char *digest, const char *key, int key_len);

#if defined(__cplusplus)
}
#endif
#endif  // QCLOUD_BLE_QIOT_HMAC_H
