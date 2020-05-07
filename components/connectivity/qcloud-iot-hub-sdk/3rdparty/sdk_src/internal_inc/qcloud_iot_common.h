/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef QCLOUD_IOT_COMMON_H_
#define QCLOUD_IOT_COMMON_H_

/* IoT C-SDK APPID */
#define QCLOUD_IOT_DEVICE_SDK_APPID "21010406"

/* MQTT server domain */
#define QCLOUD_IOT_MQTT_DIRECT_DOMAIN "iotcloud.tencentdevices.com"
#define MQTT_SERVER_PORT_TLS          8883
#define MQTT_SERVER_PORT_NOTLS        1883

/* CoAP server domain */
#define QCLOUD_IOT_COAP_DEIRECT_DOMAIN "iotcloud.tencentdevices.com"
#define COAP_SERVER_PORT               5684

/* server domain for dynamic registering device */
#define DYN_REG_SERVER_URL      "gateway.tencentdevices.com"
#define DYN_REG_SERVER_PORT     80
#define DYN_REG_SERVER_PORT_TLS 443

/* URL for doing log upload */
#define LOG_UPLOAD_SERVER_URL    "http://devicelog.iot.cloud.tencent.com/cgi-bin/report-log"
#define LOG_UPLOAD_SERVER_DOMAIN "devicelog.iot.cloud.tencent.com"
#define LOG_UPLOAD_SERVER_PORT   80

/* Max size of a host name */
#define HOST_STR_LENGTH 64

/* Max size of base64 encoded PSK = 64, after decode: 64/4*3 = 48*/
#define DECODE_PSK_LENGTH 48

#endif /* QCLOUD_IOT_COMMON_H_ */
