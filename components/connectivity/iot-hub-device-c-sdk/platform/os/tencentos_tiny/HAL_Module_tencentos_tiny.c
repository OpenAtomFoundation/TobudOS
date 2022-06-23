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

#include "esp8266_tencent_firmware.h"

#define YOUR_WIFI_SSID "WIFI_SSID"
#define YOUR_WIFI_PWD  "WIFI_PASSWORD"

/**
 * @brief Init at module.
 *
 * @return 0 for success
 */
int HAL_Module_Init(void)
{
    return esp8266_tencent_firmware_sal_init(HAL_UART_PORT_2);
}

/**
 * @brief Deinit at module.
 *
 */
void HAL_Module_Deinit(void)
{
    esp8266_tencent_firmware_sal_deinit();
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
    return esp8266_tencent_firmware_at_cmd_exec_until(at_cmd, at_expect, timeout_ms);
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
    return esp8266_tencent_firmware_at_cmd_exec_recv(at_cmd, at_expect, recv_buf, recv_len, timeout_ms);
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
    return esp8266_tencent_firmware_raw_data_send(data, data_len);
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
    return esp8266_tencent_firmware_set_urc(urc, urc_handler);
}

/**
 * @brief connect network
 *
 * @return int 0 for success
 */
int HAL_Module_ConnectNetwork(void)
{
    return esp8266_tencent_firmware_join_ap(YOUR_WIFI_SSID, YOUR_WIFI_PWD);
}
