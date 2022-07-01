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
 * @file qcloud_iot_at_client_parse.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2022-04-27
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-04-27 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "qcloud_iot_at_client.h"

#include <string.h>

/**
 * @brief Max recv timeout.
 *
 */
#define MAX_RECV_TIMEOUT 0xffffffffUL

/**
 * @brief Get expect len from expect.
 *
 * @param[in] client pointer to QcloudATClient
 */
static void _at_client_get_expect_len(QcloudATClient *client, uint32_t *expect_len)
{
    int len = 0;
    if (sscanf(client->recv_buf, client->resp.expect, &len) == 1 &&
        client->recv_buf[client->recv_len - 1] == client->resp.expect[strlen(client->resp.expect) - 1]) {
        *expect_len           = len + client->recv_len;
        client->resp.recv_len = len;
    }
}

/**
 * @brief Read one line for at recv queue.
 *
 * @param[in] client pointer to QcloudATClient
 * @return @see QCloudATErrCode
 */
static int _at_client_readline(QcloudATClient *client)
{
    int      rc, read_len = 0;
    char     ch = 0, last_ch = 0;
    size_t   read_size  = 1;
    uint32_t expect_len = 0;

    memset(client->recv_buf, 0, client->recv_buf_size);
    client->recv_len = 0;

    while (1) {
        rc = HAL_MailQueueRecv(client->recv_queue, &ch, &read_size, MAX_RECV_TIMEOUT);
        if (rc) {
            Log_e("recv from queue fail, %d", rc);
            return QCLOUD_AT_ERR_BUFF_SHORT;
        }

        if (read_len >= client->recv_buf_size) {
            Log_e("read line failed. The line data length is out of buffer size(%d)!", client->recv_buf_size);
            client->recv_len = 0;
            return QCLOUD_AT_ERR_BUFF_SHORT;
        }

        client->recv_buf[read_len++] = ch;
        client->recv_len             = read_len;

        // check if using expect string to get length
        if (client->resp.recv_buf) {
            if (!client->resp.recv_len) {
                _at_client_get_expect_len(client, &expect_len);
            }

            if (expect_len) {
                if (expect_len == client->recv_len) {
                    break;
                }
                continue;
            }
        }

        if ((ch == '\n' && last_ch == '\r')) {
            break;
        }
        last_ch = ch;
    }
    client->recv_buf[read_len] = '\0';
    return QCLOUD_AT_RET_SUCCESS;
}

/**
 * @brief Check urc table.
 *
 * @param[in] client pointer to QcloudATClient
 * @return @see QCloudATErrCode
 */
static int _at_client_check_urc(QcloudATClient *client)
{
    int          i;
    size_t       urc_len;
    QcloudATUrc *urc;

    for (i = 0; i < client->urc_table_size; i++) {
        urc     = &client->urc_table[i];
        urc_len = strlen(urc->urc_prefix);

        if (client->recv_len < urc_len) {
            continue;
        }

        if (!strncmp(urc->urc_prefix, client->recv_buf, urc_len)) {
            QcloudATUrcRecv urc_recv;
            urc_recv.urc            = urc;
            urc_recv.urc_recv_len   = client->recv_len;
            urc_recv.urc_handle_buf = HAL_Malloc(client->recv_len + 1);
            if (!urc_recv.urc_handle_buf) {
                Log_e("malloc urc_handle_buf error.");
                return QCLOUD_AT_ERR_FAILURE;
            }
            memcpy(urc_recv.urc_handle_buf, client->recv_buf, client->recv_len);
            urc_recv.urc_handle_buf[client->recv_len] = '\0';
            return HAL_MailQueueSend(client->urc_recv_queue, &urc_recv, sizeof(QcloudATUrcRecv));
        }
    }
    return QCLOUD_AT_ERR_FAILURE;
}

/**
 * @brief Check at response.
 *
 * @param[in] client pointer to QcloudATClient
 */
static int _at_client_check_common_resp(QcloudATClient *client)
{
    if (!strncmp(client->recv_buf, "OK", 2) && !client->resp.expect) {
        client->resp.status = QCLOUD_AT_RESP_STATUS_OK;
        return QCLOUD_AT_RET_SUCCESS;
    }

    if (strstr(client->recv_buf, "ERROR") || !strncmp(client->recv_buf, "FAIL", 4)) {
        client->resp.status = QCLOUD_AT_RESP_STATUS_ERROR;
        return QCLOUD_AT_ERR_RESP_FAIL;
    }
    return QCLOUD_AT_ERR_FAILURE;
}

/**
 * @brief Check expect response.
 *
 * @param[in] client pointer to QcloudATClient
 * @return @see QCloudATErrCode
 */
static int _at_client_check_expect(QcloudATClient *client)
{
    if (!client->resp.expect) {
        return QCLOUD_AT_ERR_FAILURE;
    }

    if (client->resp.recv_buf && client->resp.recv_len) {
        memcpy(client->resp.recv_buf, client->recv_buf + client->recv_len - client->resp.recv_len,
               client->resp.recv_len);
        client->resp.status = QCLOUD_AT_RESP_STATUS_OK;
        return QCLOUD_AT_RET_SUCCESS;
    }

    if (strncmp(client->resp.expect, client->recv_buf, strlen(client->resp.expect))) {
        return QCLOUD_AT_ERR_FAILURE;
    }

    client->resp.status = QCLOUD_AT_RESP_STATUS_OK;
    return QCLOUD_AT_RET_SUCCESS;
}

/**
 * @brief Parse data recv from at module.
 *
 * @param[in] client pointer to QcloudATClient
 */
void qcloud_iot_at_client_parser(void *client)
{
    int rc;

    QcloudATClient *at_client = (QcloudATClient *)client;
    while (1) {
        rc = _at_client_readline(at_client);
        if (rc) {
            continue;
        }
        printf("AT recv <--:%.*s\r\n", at_client->recv_len, at_client->recv_buf);

        rc = _at_client_check_urc(at_client);
        if (!rc) {
            continue;
        }

        if (QCLOUD_AT_RESP_STATUS_WAIT != at_client->resp.status) {
            continue;
        }

        rc = _at_client_check_expect(at_client);
        if (!rc) {
            HAL_SemaphorePost(at_client->resp_sem);
            continue;
        }

        rc = _at_client_check_common_resp(at_client);
        switch (rc) {
            case QCLOUD_AT_RET_SUCCESS:
                // if expect, ignore "OK"
                if (at_client->resp.expect) {
                    break;
                }
            case QCLOUD_AT_ERR_RESP_FAIL:
                HAL_SemaphorePost(at_client->resp_sem);
                break;
            default:
                // Log_w("ignore %.*s", at_client->recv_len, at_client->recv_buf);
                break;
        }
    }
}

/**
 * @brief Recv urc and callback.
 *
 * @param[in] client pointer to QcloudATClient
 */
void qcloud_iot_at_urc_handle(void *client)
{
    QcloudATClient *at_client = (QcloudATClient *)client;
    QcloudATUrcRecv urc_recv;
    size_t          recv_size = sizeof(QcloudATUrcRecv);
    while (1) {
        int rc = HAL_MailQueueRecv(at_client->urc_recv_queue, &urc_recv, &recv_size, MAX_RECV_TIMEOUT);
        if (rc) {
            continue;
        }
        urc_recv.urc->urc_handle(urc_recv.urc_handle_buf, urc_recv.urc_recv_len);
        HAL_Free(urc_recv.urc_handle_buf);
    }
}
