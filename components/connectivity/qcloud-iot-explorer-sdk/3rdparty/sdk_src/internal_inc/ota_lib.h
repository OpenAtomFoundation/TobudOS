/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef IOT_OTA_LIB_H_
#define IOT_OTA_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <stdint.h>

#include "qcloud_iot_export_ota.h"

void *qcloud_otalib_md5_init(void);

void qcloud_otalib_md5_update(void *md5, const char *buf, size_t buf_len);

void qcloud_otalib_md5_finalize(void *md5, char *output_str);

void qcloud_otalib_md5_deinit(void *md5);

int qcloud_otalib_get_firmware_type(const char *json, char **type);

int qcloud_otalib_get_report_version_result(const char *json);

/**
 * @brief Parse firmware info from JSON string
 *
 * @param json          source JSON string
 * @param type          parsed type
 * @param url           parsed url
 * @param version       parsed version
 * @param md5           parsed MD5
 * @param fileSize      parsed file size
 * @return              QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int qcloud_otalib_get_params(const char *json, char **type, char **url, char **version, char *md5,
                     uint32_t *fileSize);


/**
 * @brief Generate firmware info from id and version
 *
 * @param buf       output buffer
 * @param bufLen    size of buffer
 * @param id        firmware id
 * @param version   firmware version
 * @return          QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int qcloud_otalib_gen_info_msg(char *buf, size_t bufLen, uint32_t id, const char *version);


/**
 * @brief Generate firmware report
 *
 * @param buf           output buffer
 * @param bufLen        size of buffer
 * @param id            firmware id
 * @param version       firmware version
 * @param progress      download progress
 * @param reportType    report type
 * @return              QCLOUD_RET_SUCCESS for success, or err code for failure
 */
int qcloud_otalib_gen_report_msg(char *buf, size_t bufLen, uint32_t id, const char *version, int progress, IOT_OTAReportType reportType);

#ifdef __cplusplus
}
#endif

#endif /* IOT_OTA_LIB_H_ */
