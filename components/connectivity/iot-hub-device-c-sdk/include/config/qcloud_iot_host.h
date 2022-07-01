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
 * @file qcloud_iot_host.h
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-28
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-28 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2021-07-12 <td>1.1     <td>fancyxu   <td>change port to str format
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_INCLUDE_CONFIG_QCLOUD_IOT_HOST_H_
#define IOT_HUB_DEVICE_C_SDK_INCLUDE_CONFIG_QCLOUD_IOT_HOST_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief IoT C-SDK APPID.
 *
 */
#define QCLOUD_IOT_DEVICE_SDK_APPID     "21010406"
#define QCLOUD_IOT_DEVICE_SDK_APPID_LEN (sizeof(QCLOUD_IOT_DEVICE_SDK_APPID) - 1)

/**
 * @brief MQTT server domain.
 *
 */
#define QCLOUD_IOT_MQTT_DIRECT_DOMAIN "iotcloud.tencentdevices.com"
#define MQTT_SERVER_PORT_TLS          "8883"
#define MQTT_SERVER_PORT_NO_TLS       "1883"

/**
 * @brief Server domain for dynamic registering device.
 *
 */
#define DYN_REG_SERVER_URL      "ap-guangzhou.gateway.tencentdevices.com"
#define DYN_REG_URI_PATH        "/device/register"
#define DYN_REG_SERVER_PORT     "80"
#define DYN_REG_SERVER_PORT_TLS "443"

/**
 * @brief URL for doing log upload.
 *
 */
#define LOG_UPLOAD_SERVER_URL  "ap-guangzhou.gateway.tencentdevices.com"
#define LOG_UPLOAD_URI_PATH    "/device/reportlog"
#define LOG_UPLOAD_SERVER_PORT "80"

/**
 * @brief (old)URL for doing log upload.
 * Compatible with old versions, new users are recommended to use up
 * 
 */
#define LOG_UPLOAD_OLD_SERVER_URL  "http://devicelog.iot.cloud.tencent.com/cgi-bin/report-log"
#define LOG_UPLOAD_OLD_SERVER_PORT "80"

/**
 * @brief Max size of a host name.
 *
 */
#define HOST_STR_LENGTH 64

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_INCLUDE_CONFIG_QCLOUD_IOT_HOST_H_
