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
 * @file ota_downloader.h
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

#ifndef IOT_HUB_DEVICE_C_SDK_APP_OTA_OTA_DOWNLOADER_H_
#define IOT_HUB_DEVICE_C_SDK_APP_OTA_OTA_DOWNLOADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "qcloud_iot_common.h"
#include "utils_downloader.h"

/**
 * @brief Firmware version size.
 *
 */
#define MAX_SIZE_OF_FW_VERSION 32

/**
 * @brief OTA firmware info.
 *
 */
typedef struct {
    char     version[MAX_SIZE_OF_FW_VERSION + 4];
    uint32_t file_size;
    char     md5sum[33];
} OTAFirmwareInfo;

/**
 * @brief Downloader status.
 *
 */
typedef enum {
    OTA_DOWNLOADER_STATUS_NO_INITTED = 0,
    OTA_DOWNLOADER_STATUS_INITTED,
    OTA_DOWNLOADER_STATUS_DOWNLOADING,
    OTA_DOWNLOADER_STATUS_FINISHED,
} OTADownloaderStatus;

/**
 * @brief Init ota downloader.
 *
 * @param[in,out] client pointer to mqtt client
 * @return 0 for success.
 */
int ota_downloader_init(void* client);

/**
 * @brief Set download info of ota firmware.
 *
 * @param[in] firmware_info pointer to firmware info
 * @param[in] url url of cos download
 * @param[in] url_len download length
 */
void ota_downloader_info_set(OTAFirmwareInfo* firmware_info, const char* url, int url_len);

/**
 * @brief Process ota download.
 *
 * @return @see OTADownloaderStatus
 */
OTADownloaderStatus ota_downloader_process(void);

/**
 * @brief Deinit ota downloader.
 *
 */
void ota_downloader_deinit(void);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_APP_OTA_OTA_DOWNLOADER_H_
