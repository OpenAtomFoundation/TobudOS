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

#ifndef QCLOUD_IOT_EXPORT_VARIABLES_H_
#define QCLOUD_IOT_EXPORT_VARIABLES_H_

/*
 * Below variables are dependant on user situation (network status/device memory/application context)
 * Adjust the default value to meet your requirement
 */
 
/* default MQTT/CoAP timeout value when connect/pub/sub (unit: ms) */
#define QCLOUD_IOT_MQTT_COMMAND_TIMEOUT                             (5 * 1000)

/* default MQTT keep alive interval (unit: ms) */
#define QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL                         (240 * 1000)

/* default MQTT Tx buffer size, MAX: 16*1024 */
#define QCLOUD_IOT_MQTT_TX_BUF_LEN                                  (2048)

/* default MQTT Rx buffer size, MAX: 16*1024 */
#define QCLOUD_IOT_MQTT_RX_BUF_LEN                                  (2048)

/* default COAP Tx buffer size, MAX: 1*1024 */
#define COAP_SENDMSG_MAX_BUFLEN                                     (512)

/* default COAP Rx buffer size, MAX: 1*1024 */
#define COAP_RECVMSG_MAX_BUFLEN                                     (512)

/* MAX MQTT reconnect interval (unit: ms) */
#define MAX_RECONNECT_WAIT_INTERVAL                                 (60 * 1000)

/* MAX valid time when connect to MQTT server. 0: always valid */
/* Use this only if the device has accurate UTC time. Otherwise, set to 0 */
#define MAX_ACCESS_EXPIRE_TIMEOUT                                   (0)



/* log print/upload related variables */
/* MAX size of log buffer for one log item including header and content */
#define MAX_LOG_MSG_LEN 			(511)

#if defined(__linux__)
	#undef  MAX_LOG_MSG_LEN
	#define MAX_LOG_MSG_LEN         (1023)
#endif

/* 
 * Log upload related params, which will affect the size of device memory/disk consumption
 * the default value can be changed for different user situation
 */
// size of buffer for log upload
#define LOG_UPLOAD_BUFFER_SIZE      5000

// Max size of one http log upload. Should not larger than 5000
#define MAX_HTTP_LOG_POST_SIZE      3000

// MAX size for saving log into NVS (files/FLASH) after upload fail
#define MAX_LOG_SAVE_SIZE           (3*LOG_UPLOAD_BUFFER_SIZE)

// interval of log upload (unit: ms) Decrease this value if LOG_UPLOAD_BUFFER_SIZE is small
#define LOG_UPLOAD_INTERVAL_MS      2000


#endif /* QCLOUD_IOT_EXPORT_VARIABLES_H_ */
