/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#ifndef __AT_DEVICE_ESP8266_H__
#define __AT_DEVICE_ESP8266_H__

#define AT_RESP_TIMEOUT_MS				(5000)
#define ESP8266_SEND_MAX_LEN_ONCE   	(2048)
#define ESP8266_MAX_SOCKET_NUM			(5)

#define WIFI_SSID						"youga_wifi"//"Honor 8"//
#define WIFI_PASSWORD					"Iot@2018"//"xy123123"//

int at_device_esp8266_init(void);

#endif /* __AT_DEVICE_ESP8266_H__ */

