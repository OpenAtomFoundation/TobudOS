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
 * @file HAL_AT_Linux.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2022-04-21
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-04-21 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "qcloud_iot_platform.h"

#include "qcloud_iot_at_client.h"
#include "tos_hal.h"

#define YOUR_WIFI_SSID "youga_wifi"
#define YOUR_WIFI_PWD  "Iot@2018"

static hal_uart_t sg_uart;

/**
 * @brief At send function.
 *
 * @return 0 for success
 */
static int qcloud_iot_usr_at_send(const void *data, size_t data_len)
{
    return tos_hal_uart_write(&sg_uart, data, data_len, 0xffffffff);
}

static int _esp8266_init(void)
{
    const char *at_cmd_resp[][2] = {
        {"AT+RESTORE\r\n", "ready"},
        {"ATE0\r\n", NULL},
        {"AT+CWMODE=1\r\n", NULL},
        {"AT+CIPMODE=0\r\n", NULL},
    };
    int rc;
    for (int i = 0; i < sizeof(at_cmd_resp) / sizeof(const char *) / 2; i++) {
        int retry = 3;
        do {
            rc = HAL_Module_SendAtCmdWaitResp(at_cmd_resp[i][0], at_cmd_resp[i][1], 5000);
        } while (rc && retry-- > 0);
        if (rc) {
            break;
        }
    }
    return rc;
}

/**
 * @brief Init at module.
 *
 * @return 0 for success
 */
int HAL_Module_Init(void)
{
    // init at client
    int rc = qcloud_iot_at_client_init(2048, qcloud_iot_usr_at_send);
    if (rc < 0) {
        Log_e("at client init fail");
        return rc;
    }
    // init uart
    rc = tos_hal_uart_init(&sg_uart, HAL_UART_PORT_2);
    if (rc) {
        return rc;
    }

    // init esp8266
    return _esp8266_init();
}

/**
 * @brief Deinit at module.
 *
 */
void HAL_Module_Deinit(void)
{
    return;
}

/**
 * @brief Send at cmd to at module and wait for resp.
 *
 * @param[in] at_cmd at cmd
 * @param[in] at_expect expect resp
 * @param[in] timeout_ms wait timeout
 * @return 0 for success
 */
int HAL_Module_SendAtCmdWaitResp(const char *at_cmd, const char *at_expect, uint32_t timeout_ms)
{
    Log_d("at_cmd:%s", at_cmd);
    return qcloud_iot_at_client_send_at_util_expect(at_cmd, at_expect, NULL, 0, timeout_ms);
}

/**
 * @brief Send at cmd and waif for data.
 *
 * @param[in] at_cmd at cmd
 * @param[in] at_expect expect resp
 * @param[out] recv_buf recv data buffer
 * @param[out] recv_len recv data length
 * @param[in] timeout_ms wait timeout
 * @return 0 for success
 */
int HAL_Module_SendAtCmdWaitRespWithData(const char *at_cmd, const char *at_expect, void *recv_buf, uint32_t *recv_len,
                                         uint32_t timeout_ms)
{
    return qcloud_iot_at_client_send_at_util_expect(at_cmd, at_expect, recv_buf, recv_len, timeout_ms);
}

/**
 * @brief Send date to at module.
 *
 * @param[in] data data to send
 * @param[in] data_len data length
 * @return 0 for success
 */
int HAL_Module_SendAtData(const void *data, int data_len)
{
    return qcloud_iot_at_client_send_data(data, data_len);
}

/**
 * @brief Set urc.
 *
 * @param[in] urc irc string
 * @param[in] urc_handler urc handler
 * @return 0 for success
 */
int HAL_Module_SetUrc(const char *urc, OnUrcHandler urc_handler)
{
    static QcloudATUrc sg_urc_table[10];
    static int         sg_urc_table_count;

    sg_urc_table[sg_urc_table_count].urc_prefix = urc;
    sg_urc_table[sg_urc_table_count].urc_handle = urc_handler;
    sg_urc_table_count++;
    qcloud_iot_at_client_set_urc(sg_urc_table, sg_urc_table_count);
    return 0;
}

/**
 * @brief connect network
 *
 * @return int 0 for success
 */
int HAL_Module_ConnectNetwork(void)
{
    return HAL_Module_SendAtCmdWaitResp("AT+CWJAP=\"" YOUR_WIFI_SSID "\",\"" YOUR_WIFI_PWD "\"\r\n", "WIFI GOT IP",
                                        10000);
}
