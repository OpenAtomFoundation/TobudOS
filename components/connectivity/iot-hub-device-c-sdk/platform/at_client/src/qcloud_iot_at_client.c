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
 * @file qcloud_iot_at_client.c
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

#include "qcloud_iot_at_client.h"

/**
 * @brief Only one client is support.
 *
 */
static QcloudATClient sg_client;

/**
 * @brief Init at client.
 *
 * @param[in] max_at_size max at buffer size
 * @param[in] at_send_func send at function
 * @return @see QCloudATErrCode
 */
int qcloud_iot_at_client_init(int max_at_size, QcloudATSendDataFunc at_send_func)
{
    if (QCLOUD_AT_STATUS_INIT == sg_client.status) {
        return QCLOUD_AT_RET_INITTED;
    }
    sg_client.status = QCLOUD_AT_STATUS_INIT;

    int rc;

    QcloudATClient *client = &sg_client;
    client->at_send_func   = at_send_func;

    client->recv_pool = HAL_Malloc(max_at_size);
    if (!client->recv_pool) {
        goto exit;
    }

    client->recv_queue = HAL_MailQueueInit(client->recv_pool, 1, max_at_size);
    if (!client->recv_queue) {
        goto exit;
    }
    client->recv_buf = HAL_Malloc(max_at_size);
    if (!client->recv_buf) {
        goto exit;
    }
    client->recv_buf_size = max_at_size;
    client->recv_len      = 0;

    client->urc_table      = NULL;
    client->urc_table_size = 0;

    client->urc_recv_pool = HAL_Malloc(sizeof(QcloudATUrcRecv) * QCLOUD_AT_MAX_URC_QUEUE_LEN);
    if (!client->urc_recv_pool) {
        goto exit;
    }

    client->urc_recv_queue =
        HAL_MailQueueInit(client->urc_recv_pool, sizeof(QcloudATUrcRecv), QCLOUD_AT_MAX_URC_QUEUE_LEN);
    if (!client->urc_recv_queue) {
        goto exit;
    }

    client->resp.expect = NULL;
    client->resp.status = QCLOUD_AT_RESP_STATUS_IDLE;
    client->resp_lock   = HAL_MutexCreate();
    if (!client->resp_lock) {
        goto exit;
    }
    client->resp_sem = HAL_SemaphoreCreate();
    if (!client->resp_sem) {
        goto exit;
    }

    static ThreadParams sg_qcloud_at_parse_thread_params = {0};
    static void        *sg_qcloud_at_parse_thread_stack;
    if (!sg_qcloud_at_parse_thread_stack) {
        sg_qcloud_at_parse_thread_stack = HAL_Malloc(QCLOUD_AT_PARSE_THREAD_STACK_SIZE);
        if (!sg_qcloud_at_parse_thread_stack) {
            goto exit;
        }
    }
    sg_qcloud_at_parse_thread_params.user_arg    = client;
    sg_qcloud_at_parse_thread_params.stack_base  = sg_qcloud_at_parse_thread_stack;
    sg_qcloud_at_parse_thread_params.stack_size  = QCLOUD_AT_PARSE_THREAD_STACK_SIZE;
    sg_qcloud_at_parse_thread_params.thread_name = QCLOUD_AT_PARSE_THREAD_NAME;
    sg_qcloud_at_parse_thread_params.priority    = QCLOUD_AT_PARSE_THREAD_PRIORITY;
    sg_qcloud_at_parse_thread_params.thread_func = qcloud_iot_at_client_parser;

    rc = HAL_ThreadCreate(&sg_qcloud_at_parse_thread_params);
    if (rc) {
        goto exit;
    }

    static ThreadParams sg_qcloud_at_urc_thread_params = {0};
    static void        *sg_qcloud_at_urc_thread_stack;
    if (!sg_qcloud_at_urc_thread_stack) {
        sg_qcloud_at_urc_thread_stack = HAL_Malloc(QCLOUD_AT_URC_THREAD_STACK_SIZE);
        if (!sg_qcloud_at_urc_thread_stack) {
            goto exit;
        }
    }
    sg_qcloud_at_urc_thread_params.user_arg    = client;
    sg_qcloud_at_urc_thread_params.stack_base  = sg_qcloud_at_urc_thread_stack;
    sg_qcloud_at_urc_thread_params.stack_size  = QCLOUD_AT_URC_THREAD_STACK_SIZE;
    sg_qcloud_at_urc_thread_params.thread_name = QCLOUD_AT_URC_THREAD_NAME;
    sg_qcloud_at_urc_thread_params.priority    = QCLOUD_AT_URC_THREAD_PRIORITY;
    sg_qcloud_at_urc_thread_params.thread_func = qcloud_iot_at_urc_handle;

    rc = HAL_ThreadCreate(&sg_qcloud_at_urc_thread_params);
    if (rc) {
        HAL_ThreadDestroy(sg_qcloud_at_parse_thread_params.thread_id);
        goto exit;
    }
    return QCLOUD_AT_RET_SUCCESS;
exit:
    HAL_MailQueueDeinit(client->recv_queue);
    HAL_Free(client->recv_pool);
    HAL_Free(client->recv_buf);
    HAL_MutexDestroy(client->resp_lock);
    HAL_SemaphoreDestroy(client->resp_sem);
    HAL_MailQueueDeinit(client->urc_recv_queue);
    HAL_Free(client->urc_recv_pool);
    memset(client, 0, sizeof(QcloudATClient));
    return QCLOUD_AT_ERR_FAILURE;
}

/**
 * @brief Set at urc.
 *
 * @param[in] urc_table urc table
 * @param[in] urc_table_size table size
 */
void qcloud_iot_at_client_set_urc(QcloudATUrc *urc_table, uint32_t urc_table_size)
{
    sg_client.urc_table      = urc_table;
    sg_client.urc_table_size = urc_table_size;
}

/**
 * @brief Send at command and wait for receiving expect response.
 *
 * @param[in] at_cmd at command
 * @param[in] at_expect expect response
 * @param[in] timeout_ms wait timeout
 * @return @see QCloudATErrCode
 */

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
                                             uint32_t *recv_len, uint32_t timeout_ms)
{
    QcloudATClient *client = &sg_client;
    if (QCLOUD_AT_STATUS_NO_INIT == client->status) {
        return QCLOUD_AT_ERR_NO_INIT;
    }
    int rc;

    HAL_MutexLock(client->resp_lock);
    client->resp.expect   = at_expect;
    client->resp.recv_buf = recv_buf;
    client->resp.recv_len = 0;
    client->resp.status   = QCLOUD_AT_RESP_STATUS_WAIT;

    rc = client->at_send_func(at_cmd, strlen(at_cmd));
    if (rc) {
        rc = QCLOUD_AT_ERR_SEND_FAIL;
        goto exit;
    }

    rc = HAL_SemaphoreWait(client->resp_sem, timeout_ms);
    if (rc) {
        rc = QCLOUD_AT_ERR_TIMEOUT;
        goto exit;
    }

    if (client->resp.status != QCLOUD_AT_RESP_STATUS_OK) {
        rc = QCLOUD_AT_ERR_RESP_FAIL;
    }
    *recv_len = client->resp.recv_len;
exit:
    client->resp.expect = NULL;
    client->resp.status = QCLOUD_AT_RESP_STATUS_IDLE;
    HAL_MutexUnlock(client->resp_lock);
    return rc;
}

/**
 * @brief Send data to at module.
 *
 * @param[in] data
 * @param[in] data_len
 * @return 0 for success
 */
int qcloud_iot_at_client_send_data(const void *data, size_t data_len)
{
    QcloudATClient *client = &sg_client;
    if (QCLOUD_AT_STATUS_NO_INIT == client->status) {
        return QCLOUD_AT_ERR_NO_INIT;
    }
    return client->at_send_func(data, data_len);
}

/**
 * @brief Push data to at client queue.
 *
 * @param[in] recv_data recv data from uart
 * @param[in] data_len recv data length
 * @return 0 for success.
 */
int qcloud_iot_at_client_push_data(const void *recv_data, size_t data_len)
{
    int             rc     = 0;
    QcloudATClient *client = &sg_client;
    if (QCLOUD_AT_STATUS_NO_INIT == client->status) {
        return QCLOUD_AT_ERR_NO_INIT;
    }
    for (size_t i = 0; i < data_len; i++) {
        rc = HAL_MailQueueSend(client->recv_queue, (uint8_t *)recv_data + i, 1);
        if (rc) {
            return QCLOUD_AT_ERR_FAILURE;
        }
    }
    return QCLOUD_AT_RET_SUCCESS;
}
