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

#ifndef IOT_OTA_FETCH_H_
#define IOT_OTA_FETCH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void *ofc_Init(const char *url, uint32_t offset, uint32_t size);

int32_t qcloud_ofc_connect(void *handle);

int32_t qcloud_ofc_fetch(void *handle, char *buf, uint32_t buf_len, uint32_t timeout_s);

int qcloud_ofc_deinit(void *handle);

#ifdef __cplusplus
}
#endif

#endif /* IOT_OTA_FETCH_H_ */
