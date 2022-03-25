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
 * @file qcloud_iot_variables.h
 * @brief variables can be set depend on user resource.
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-28
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-28 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_INCLUDE_CONFIG_QCLOUD_IOT_VARIABLES_H_
#define IOT_HUB_DEVICE_C_SDK_INCLUDE_CONFIG_QCLOUD_IOT_VARIABLES_H_

#ifdef __cplusplus
extern "C" {
#endif

// Variables are dependant on user situation (network status/device memory/application context). Adjust the default
// value to meet your requirement

/**
 * @brief default MQTT timeout value when connect/pub/sub (unit: ms)
 *
 */
#define QCLOUD_IOT_MQTT_COMMAND_TIMEOUT (5 * 1000)

/**
 * @brief default MQTT timeout value when wait server ack
 *
 */
#define QCLOUD_IOT_MQTT_WAIT_ACK_TIMEOUT (5 * 1000)

/**
 * @brief default MQTT keep alive interval (unit: ms)
 *
 */
#define QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL (240 * 1000)

/**
 * @brief default MQTT Tx buffer size, MAX: 16*1024
 *
 */
#define QCLOUD_IOT_MQTT_TX_BUF_LEN (4096)

/**
 * @brief default MQTT Rx buffer size, MAX: 16*1024
 *
 */
#define QCLOUD_IOT_MQTT_RX_BUF_LEN (4096)

/**
 * @brief default MQTT timeout value when yield
 *
 */
#define QCLOUD_IOT_MQTT_YIELD_TIMEOUT (500)

/**
 * @brief MAX MQTT reconnect interval (unit: ms)
 *
 */
#define MAX_RECONNECT_WAIT_INTERVAL (60 * 1000)

/**
 * @brief MAX valid time when connect to MQTT server. 0: always valid.
 * Use this only if the device has accurate UTC time. Otherwise, set to 0.
 *
 */
#define MAX_ACCESS_EXPIRE_TIMEOUT (0)

// Log upload related params, which will affect the size of device memory/disk consumption Log upload related params,
// which will affect the size of device memory/disk consumption

/**
 * @brief size of buffer for log upload
 *
 */
#define LOG_UPLOAD_BUFFER_SIZE 5000

/**
 * @brief Max size of one http log upload. Should not larger than 5000
 *
 */
#define MAX_HTTP_LOG_POST_SIZE 5000

/**
 * @brief MAX size for saving log into NVS (files/FLASH) after upload fail
 *
 */
#define MAX_LOG_SAVE_SIZE (3 * LOG_UPLOAD_BUFFER_SIZE)

/**
 * @brief interval of log upload (unit: ms) Decrease this value if LOG_UPLOAD_BUFFER_SIZE is small
 *
 */
#define LOG_UPLOAD_INTERVAL_MS 1800000

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_INCLUDE_CONFIG_QCLOUD_IOT_VARIABLES_H_
