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

#ifndef QCLOUD_IOT_UTILS_BASE64_H_
#define QCLOUD_IOT_UTILS_BASE64_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "qcloud_iot_export_error.h"
#include "qcloud_iot_export_log.h"

int qcloud_iot_utils_base64encode(unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen);

int qcloud_iot_utils_base64decode(unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen);

#ifdef __cplusplus
}
#endif
#endif /* QCLOUD_IOT_UTILS_BASE64_H_ */
