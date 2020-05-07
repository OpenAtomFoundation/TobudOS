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

#ifndef IOT_OTA_CLIENT_H_
#define IOT_OTA_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Specify the maximum characters of version */
#define OTA_MAX_TOPIC_LEN (64)

#define TYPE_FIELD     "type"
#define MD5_FIELD      "md5sum"
#define VERSION_FIELD  "version"
#define URL_FIELD      "url"
#define FILESIZE_FIELD "file_size"
#define RESULT_FIELD   "result_code"

#define REPORT_VERSION_RSP "report_version_rsp"
#define UPDATE_FIRMWARE    "update_firmware"

enum { MQTT_CHANNEL, COAP_CHANNEL };

typedef void (*OnOTAMessageCallback)(void *pcontext, const char *msg, uint32_t msgLen);

void *qcloud_osc_init(const char *productId, const char *deviceName, void *channel, OnOTAMessageCallback callback,
                      void *context);

int qcloud_osc_deinit(void *handle);

int qcloud_osc_report_progress(void *handle, const char *msg);

int qcloud_osc_report_version(void *handle, const char *msg);

int qcloud_osc_report_upgrade_result(void *handle, const char *msg);

#ifdef __cplusplus
}
#endif

#endif /* IOT_OTA_CLIENT_H_ */
