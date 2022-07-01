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
 * @file qcloud_iot_log_upload.h
 * @brief
 * @author hubertxxu (hubertxxu@tencent.com)
 * @version 1.0
 * @date 2022-01-05
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-01-05 <td>1.0     <td>hubertxxu   <td>first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_LOG_UPLOAD_H_
#define IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_LOG_UPLOAD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "qcloud_iot_common.h"

#include "utils_json.h"

/**
 * @brief User callback for saving/reading logs into/from NVS(files/FLASH) after upload fail/recover
 *
 */
// callback for saving logs into NVS(files/FLASH) after upload fail
typedef size_t (*LogSaveFunc)(const char *filename, const void *buf, size_t write_len, size_t offset);
// callback for reading logs from NVS(files/FLASH) when upload ready
typedef size_t (*LogReadFunc)(const char *filename, void *buf, size_t read_len, size_t offset);
// callback for deleting logs in NVS(files/FLASH). return 0 when success
typedef int (*LogDelFunc)(const char *filename);
// callback for reading the size of logs in NVS(files/FLASH). return 0 when nothing exist
typedef size_t (*LogGetSizeFunc)(const char *filename);

/**
 * @brief Data structure to init feature of log upload.
 *
 */
typedef struct {
    /* device info */
    const char *product_id;
    const char *device_name;
    /* auth key, use device secret for PSK device and cert file path for cert device */
    const char *sign_key;
    /* http connect domain */
    const char *host;
    uint32_t    log_buffer_size;
    /* user callback saving/reading logs into/from NVS(files/FLASH) */
    const char    *save_log_filename;
    LogSaveFunc    save_func;
    LogReadFunc    read_func;
    LogDelFunc     del_func;
    LogGetSizeFunc get_size_func;
} LogUploadInitParams;

/**
 * @brief Default params.
 *
 */
#define DEFAULT_LOG_UPLOAD_INIT_PARAMS                                               \
    {                                                                                \
        NULL, NULL, NULL, NULL, LOG_UPLOAD_BUFFER_SIZE, NULL, NULL, NULL, NULL, NULL \
    }

/**
 * @brief Init log upload previously.
 *
 * @param[in] init_params @see LogUploadInitParams
 * @return @see IotReturnCode
 */
int IOT_Log_Upload_InitPre(const LogUploadInitParams *init_params);

/**
 * @brief Init log upload module.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
int IOT_Log_Upload_Init(void *client);

/**
 * @brief Stop log upload add release resources.
 *
 * @return @see IotReturnCode
 */
int IOT_Log_Upload_Deinit(void);

/**
 * @brief Append need report log to log upload buffer.
 *
 * @param[in] log_level @see LogLevel
 * @param[in] log_content data of need to report
 */
void IOT_Log_Upload_AppendToUploadBuffer(LogLevel log_level, const char *log_content);

/**
 * @brief Do log upload.
 *
 * @param[in] force_upload force upload when error
 * @return @see IotReturnCode
 */
int IOT_Log_Upload(bool force_upload);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_LOG_UPLOAD_H_
