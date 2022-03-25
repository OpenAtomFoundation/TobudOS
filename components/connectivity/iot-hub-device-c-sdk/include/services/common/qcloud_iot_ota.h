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
 * @file qcloud_iot_ota.h
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-10-11
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-10-11 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_OTA_H_
#define IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_OTA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "qcloud_iot_common.h"

#include "utils_json.h"

/**
 * @brief OTA report type.
 *
 */
typedef enum {
    IOT_OTA_REPORT_TYPE_DOWNLOADING = 0,
    IOT_OTA_REPORT_TYPE_UPGRADE_BEGIN,
    IOT_OTA_REPORT_TYPE_UPGRADE_SUCCESS,
    IOT_OTA_REPORT_TYPE_DOWNLOAD_TIMEOUT,
    IOT_OTA_REPORT_TYPE_FILE_NOT_EXIST,
    IOT_OTA_REPORT_TYPE_AUTH_FAIL,
    IOT_OTA_REPORT_TYPE_MD5_NOT_MATCH,
    IOT_OTA_REPORT_TYPE_UPGRADE_FAIL,
} IotOTAReportType;

/**
 * @brief Callback of OTA.
 *
 */
typedef struct {
    void (*update_firmware_callback)(UtilsJsonValue version, UtilsJsonValue url, UtilsJsonValue md5sum,
                                     uint32_t file_size, void *usr_data);
    void (*report_version_reply_callback)(int result_code, void *usr_data);
} IotOTAUpdateCallback;

/**
 * @brief OTA init, subscribe update topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] callback @see IotOTAUpdateCallback
 * @param[in] usr_data usr data used in callback
 * @return 0 for success, or err code (<0) @see IotReturnCode
 */
int IOT_OTA_Init(void *client, IotOTAUpdateCallback callback, void *usr_data);

/**
 * @brief OTA deinit, unsubscribe update topic.
 *
 * @param[in,out] client pointer to mqtt client
 */
void IOT_OTA_Deinit(void *client);

/**
 * @brief Report upgrade progress.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] buf publish message buffer
 * @param[in] buf_len buffer len
 * @param[in] report_type @see IotOTAReportType
 * @param[in] progress progress using in IOT_OTA_REPORT_TYPE_DOWNLOADING
 * @param[in] version update firmware version
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_OTA_ReportProgress(void *client, char *buf, int buf_len, IotOTAReportType report_type, int progress,
                           const char *version);

/**
 * @brief Report current firmware version.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] buf publish message buffer
 * @param[in] buf_len buffer len
 * @param[in] version current firmware version
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_OTA_ReportVersion(void *client, char *buf, int buf_len, const char *version);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_OTA_H_
