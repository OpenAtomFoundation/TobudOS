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
 * @file mqtt_client_common.c
 * @brief common api
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-28
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-28 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2021-07-08 <td>1.1     <td>fancyxu   <td>fix code standard of IotReturnCode and QcloudIotClient
 * </table>
 */

#include "mqtt_client.h"

/**
 * @brief Get the next packet id object.
 *
 * @param[in,out] client pointer to mqtt client
 * @return packet id
 */
uint16_t get_next_packet_id(QcloudIotClient *client)
{
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

    HAL_MutexLock(client->lock_generic);
    client->next_packet_id = (uint16_t)((MAX_PACKET_ID == client->next_packet_id) ? 1 : (client->next_packet_id + 1));
    HAL_MutexUnlock(client->lock_generic);

    IOT_FUNC_EXIT_RC(client->next_packet_id);
}

/**
 * @brief Get the next conn id object.
 *
 * @param[out] conn_id buffer to save conn id
 */
void get_next_conn_id(char *conn_id)
{
    int i;
    srand(HAL_Timer_CurrentSec());
    for (i = 0; i < MAX_CONN_ID_LEN - 1; i++) {
        int flag = rand() % 3;
        switch (flag) {
            case 0:
                conn_id[i] = (rand() % 26) + 'a';
                break;
            case 1:
                conn_id[i] = (rand() % 26) + 'A';
                break;
            case 2:
                conn_id[i] = (rand() % 10) + '0';
                break;
        }
    }

    conn_id[MAX_CONN_ID_LEN - 1] = '\0';
}

/**
 * @brief Set the client conn state object.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] connected connect status, @see ConnStatus
 */
void set_client_conn_state(QcloudIotClient *client, uint8_t connected)
{
    HAL_MutexLock(client->lock_generic);
    client->is_connected = connected;
    HAL_MutexUnlock(client->lock_generic);
}

/**
 * @brief Get the client conn state object.
 *
 * @param[in,out] client
 * @return @see ConnStatus
 */
uint8_t get_client_conn_state(QcloudIotClient *client)
{
    IOT_FUNC_ENTRY;
    uint8_t is_connected = 0;
    HAL_MutexLock(client->lock_generic);
    is_connected = client->is_connected;
    HAL_MutexUnlock(client->lock_generic);
    IOT_FUNC_EXIT_RC(is_connected);
}

/**
 * @brief Send mqtt packet, timeout = command_timeout_ms.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] length length of data to be sent, data is saved in client write_buf
 * @return @see IotReturnCode
 */
int send_mqtt_packet(QcloudIotClient *client, size_t length)
{
    IOT_FUNC_ENTRY;

    int rc = QCLOUD_RET_SUCCESS;

    size_t sent_len = 0;

    if (length >= client->write_buf_size) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    rc = client->network_stack.write(&(client->network_stack), client->write_buf, length, client->command_timeout_ms,
                                     &sent_len);
    rc = QCLOUD_ERR_TCP_WRITE_TIMEOUT == rc ? QCLOUD_ERR_MQTT_REQUEST_TIMEOUT : rc;
    IOT_FUNC_EXIT_RC(rc);
}
