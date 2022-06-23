/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2022 THL A29 Limited, a Tencent company.All rights reserved.
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
 * @file qcloud_iot_at_client.h
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2022-05-06
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-05-06 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_PLATFORM_AT_CLIENT_INC_QCLOUD_IOT_AT_CLIENT_H_
#define IOT_HUB_DEVICE_C_SDK_PLATFORM_AT_CLIENT_INC_QCLOUD_IOT_AT_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_platform.h"

#define QCLOUD_AT_PARSE_THREAD_NAME       "at_parse"
#define QCLOUD_AT_PARSE_THREAD_STACK_SIZE (4096)
#define QCLOUD_AT_PARSE_THREAD_PRIORITY   THREAD_PRIORITY_HIGH
#define QCLOUD_AT_MAX_URC_QUEUE_LEN       (16)

typedef enum {
    QCLOUD_AT_RET_SUCCESS    = 0,
    QCLOUD_AT_RET_INITTED    = 1,
    QCLOUD_AT_ERR_FAILURE    = -1,
    QCLOUD_AT_ERR_NO_INIT    = -2,
    QCLOUD_AT_ERR_TIMEOUT    = -3,
    QCLOUD_AT_ERR_SEND_FAIL  = -4,
    QCLOUD_AT_ERR_RESP_FAIL  = -5,
    QCLOUD_AT_ERR_BUFF_SHORT = -5,
} QCloudATErrCode;

/**
 * @brief AT urc.
 *
 */
typedef struct {
    const char  *urc_prefix;
    OnUrcHandler urc_handle;
} QcloudATUrc;

/**
 * @brief Send data to at module.
 *
 */
typedef int (*QcloudATSendDataFunc)(const void *data, size_t data_len);

/**
 * @brief AT init status.
 *
 */
typedef enum {
    QCLOUD_AT_STATUS_NO_INIT = 0,
    QCLOUD_AT_STATUS_INIT    = 1,
} QcloudATStatus;

/**
 * @brief AT response status.
 *
 */
typedef enum {
    QCLOUD_AT_RESP_STATUS_IDLE  = 0,  /* no change resp status */
    QCLOUD_AT_RESP_STATUS_WAIT  = 1,  /* AT response wait */
    QCLOUD_AT_RESP_STATUS_OK    = 2,  /* AT response OK */
    QCLOUD_AT_RESP_STATUS_ERROR = -1, /* AT response ERROR */
} QcloudATRespStatus;

/**
 * @brief AT response.
 *
 */
typedef struct {
    const char        *expect;
    uint8_t           *recv_buf; /* recv data from at buffer */
    uint32_t           recv_len; /* recv data length */
    QcloudATRespStatus status;
} QcloudATResp;

/**
 * @brief AT URC Recv handle.
 *
 */
typedef struct {
    QcloudATUrc *urc;
    char        *urc_handle_buf;
    uint32_t     urc_recv_len;
} QcloudATUrcRecv;

/**
 * @brief AT client.
 *
 */
typedef struct {
    QcloudATStatus       status;
    QcloudATSendDataFunc at_send_func;
    // recv data
    void    *recv_queue;
    char    *recv_buf;
    uint32_t recv_buf_size;
    uint32_t recv_len;
    // urc
    QcloudATUrc *urc_table;
    uint32_t     urc_table_size;
    void        *urc_recv_queue;
    void        *urc_lock;
    // resp
    QcloudATResp resp;
    void        *resp_lock;
    void        *resp_sem;
} QcloudATClient;

/**
 * @brief Parse data recv from at module.
 *
 * @param[in] client pointer to QcloudATClient
 */
void qcloud_iot_at_client_parser(void *client);

/**
 * @brief Recv urc and callback.
 *
 * @param[in] client pointer to QcloudATClient
 */
void qcloud_iot_at_urc_handle(void *client);

/**
 * @brief Init at client.
 *
 * @param[in] max_at_size max at buffer size
 * @param[in] at_send_func send at function
 * @return @see QCloudATErrCode
 */
int qcloud_iot_at_client_init(int max_at_size, QcloudATSendDataFunc at_send_func);

/**
 * @brief Set at urc.
 *
 * @param[in] urc_table urc table
 * @param[in] urc_table_size table size
 */
void qcloud_iot_at_client_set_urc(QcloudATUrc *urc_table, uint32_t urc_table_size);

/**
 * @brief Send at command and wait for receiving expect response or data.
 *
 * @param[in] at_cmd at command
 * @param[in] at_expect expect response or data length format
 * @param[out] recv_buf recv data buffer
 * @param[out] recv_len recv data length
 * @param[in] timeout_ms wait timeout
 * @return @see QCloudATErrCode
 */
int qcloud_iot_at_client_send_at_util_expect(const char *at_cmd, const char *at_expect, void *recv_buf,
                                             uint32_t *recv_len, uint32_t timeout_ms);

/**
 * @brief Send data to at module.
 *
 * @param[in] data
 * @param[in] data_len
 * @return 0 for success
 */
int qcloud_iot_at_client_send_data(const void *data, size_t data_len);

/**
 * @brief Push data to at client queue.
 *
 * @param[in] recv_data recv data from uart
 * @param[in] data_len recv data length
 * @return 0 for success.
 */
int qcloud_iot_at_client_push_data(const void *recv_data, size_t data_len);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_PLATFORM_AT_CLIENT_INC_QCLOUD_IOT_AT_CLIENT_H_
