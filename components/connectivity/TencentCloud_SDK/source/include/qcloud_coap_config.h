/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _QCLOUD_COAP_CONFIG_H_
#define  _QCLOUD_COAP_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

static const char *qcloud_coap_server_domain = "iotcloud.tencentdevices.com";

/* 判断COAP消息是否为空 */
#define COAP_MSG_IS_EMPTY(message)                 					(((message)->code_class == 0) && ((message)->code_detail == 0))

/* 判断COAP消息是否为空 ACK*/
#define COAP_MSG_IS_EMPTY_ACK(message)                 					(((message)->code_class == 2) && ((message)->code_detail == 3))

/* 判断COAP消息是否为RESP */
#define COAP_MSG_IS_EMPTY_RSP(message)                 					(((message)->code_class == 2) && ((message)->code_detail == 5))

/* COAP协议版本号  */
#define COAP_VERSION                        (0x01)

/* token最大长度 */
#define COAP_MSG_TOKEN_MAX              (8)

/* Message id最大值 */
#define COAP_MSG_ID_MAX                 ((1 << 16) - 1)

/* COAP 消息最大code class */
#define COAP_MSG_CODE_CLASS_MAX             (7)

/* COAP 消息最大code detail */
#define COAP_MSG_CODE_DETAIL_MAX            (31)

#define QCLOUD_COAP_COMMAND_TIMEOUT             (15 * 1000)

/* CoAP 最大链接ID的长度 */
#define QCLOUD_COAP_CONNECT_ID_MAX                              (6)

/* COAP 发送消息buffer大小，最大支持64*1024字节 */
#define QCLOUD_COAP_CLIENT_TX_BUF_LEN   (512)

/* COAP 接收消息buffer大小，最大支持64*1024字节 */
#define QCLOUD_COAP_CLIENT_RX_BUF_LEN   (512)

/* 设备鉴权URI，该URI唯一，禁止设备创建该URI topic */
#define QCLOUD_COAP_AUTH_URI												("txauth9w0BAQsFA")

/* Topic最大长度 */
#define QCLOUD_COAP_URI_MAX             (128)

#define QCLOUD_COAP_AUTH_ID_MAX     (sizeof(QCLOUD_APPID) + QCLOUD_COAP_CONNECT_ID_MAX + 2)

#define QCLOUD_COAP_AUTH_URI_MAX    (QCLOUD_DEVICE_PRODUCT_ID_MAX + QCLOUD_DEVICE_DEVICE_NAME_MAX + sizeof(QCLOUD_COAP_AUTH_URI) + 4)

#ifdef __cplusplus
}
#endif

#endif

