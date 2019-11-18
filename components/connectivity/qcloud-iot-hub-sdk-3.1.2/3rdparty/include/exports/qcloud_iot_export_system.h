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

#ifndef QCLOUD_IOT_EXPORT_SYSTEM_H_
#define QCLOUD_IOT_EXPORT_SYSTEM_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get system timestamp from MQTT server
 *
 * @param pClient           MQTTClient pointer
 * @param time              timestamp value return from server
 * @return                  QCLOUD_RET_SUCCESS for success
 *                          otherwise, failure
 */
int IOT_Get_SysTime(void* pClient, long* time);


#ifdef __cplusplus
}
#endif

#endif /* QCLOUD_IOT_EXPORT_COAP_H_ */