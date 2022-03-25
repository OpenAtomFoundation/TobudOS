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
 * @file mqtt_client.h
 * @brief mqtt client internel api
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-31
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-31 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2021-07-08 <td>1.1     <td>fancyxu   <td>fix code standard of IotReturnCode and QcloudIotClient
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_SERVICES_COMMON_LOG_UPLOAD_INC_LOG_UPLOAD_H_
#define IOT_HUB_DEVICE_C_SDK_SERVICES_COMMON_LOG_UPLOAD_INC_LOG_UPLOAD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_log_upload.h"

#include "utils_list.h"
#include "utils_base64.h"
#include "utils_hmac.h"

/**
 * @brief To check log http server return msg or not
 *
 */
#define LOG_CHECK_HTTP_RET_CODE

/**
 * @brief enable log upload debug or not
 *
 */
// #define LOG_UPLOAD_DEBUG

/**
 * @brief http recv length
 *
 */
#define HTTP_RET_JSON_LENGTH 256

/**
 * @brief wait http recv time out
 *
 */
#define HTTP_WAIT_RET_TIMEOUT_MS 1000

/**
 * @brief do immediate log update if buffer is lower than this threshold (about two max log item)
 *
 */
#define LOG_LOW_BUFFER_THRESHOLD (LOG_UPLOAD_BUFFER_SIZE / 4)

/**
 * @brief sing key length
 *
 */
#define SIGN_KEY_SIZE (24)

#ifdef LOG_UPLOAD_DEBUG
#define UPLOAD_DBG(fmt, ...) HAL_Printf(">>LOG-DBG>>%s(%d): " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define UPLOAD_DBG(...)
#endif
#define UPLOAD_ERR(fmt, ...) HAL_Printf(">>LOG-ERR>>%s(%d): " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

/**
 * @brief clear log upload buffer
 *
 */
void log_upload_clear_buffer(void);

/**
 * @brief set log upload in comm err state
 *
 * @param state true or false
 */
void log_upload_set_upload_log_in_comm_err(bool state);

/**
 * @brief change log upload level
 *
 * @param level
 */
void log_upload_set_log_upload_level(LogLevel level);

/**
 * @brief get log upload level
 *
 * @return LogLevel
 */
LogLevel log_upload_get_log_upload_level(void);

/**
 * @brief log upload buffer init
 *
 * @param init_params @see LogUploadInitParams
 * @return void* if success return @see LogUploadBuffer
 */
void *log_upload_buffer_init(LogUploadInitParams *init_params);
/**
 * @brief deinit log buffer handle
 *
 * @param upload_buffer_handle
 */
void log_upload_buffer_deinit(void *upload_buffer_handle);
/**
 * @brief Get the log buffer write index object
 *
 * @param log_upload_buffer_handle
 * @return uint32_t
 */
uint32_t get_log_buffer_write_index(void *log_upload_buffer_handle);
/**
 * @brief Get the log buffer head len object
 *
 * @param log_upload_buffer_handle
 * @return uint32_t
 */
uint32_t get_log_buffer_head_len(void *log_upload_buffer_handle);
/**
 * @brief check log buffer is empty or not
 *
 * @param log_upload_buffer_handle
 * @return true empty
 * @return false not empty
 */
bool have_data_need_update(void *log_upload_buffer_handle);
/**
 * @brief When a new log appears during the reporting period, move the log to the specified position in the buffer
 *
 */
void remove_log_from_buffer(void *log_upload_buffer_handle, uint32_t need_remove_size);
/**
 * @brief copy log buffer head
 *
 * @param[out] dst Destination address
 * @param log_upload_buffer_handle
 */
void copy_log_buffer_header(char *dst, void *log_upload_buffer_handle);

/**
 * @brief lock log buffer
 *
 */
void log_buffer_lock(void);

/**
 * @brief unlock log buffer
 *
 */
void log_buffer_unlock(void);

/**
 * @brief post data to tencent cloud over http
 *
 * @param[in] post_buf  need post data buffer
 * @param[in] post_size need post data buffer length
 * @return @see IotReturnCode
 */
int post_one_http_to_server(void *handle, char *post_buf, size_t post_size);

/**
 * @brief Get the log buffer object
 *
 * @param log_upload_buffer_handle
 * @return char*
 */
char *get_log_buffer(void *log_upload_buffer_handle);

/**
 * @brief push data to log buffer
 *
 * @param[in] log_content need push data
 * @return int if 0 success else QCLOUD_ERR_FAILURE
 */
int append_data_to_log_buffer(const char *log_content);
/**
 * @brief init log upload module
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
int log_upload_init(void *client);
/**
 * @brief Get the log buffer size object
 *
 * @return uint32_t log buffer size
 */
uint32_t get_log_buffer_size(void);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_SERVICES_COMMON_LOG_UPLOAD_INC_LOG_UPLOAD_H_
