/*
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

#ifndef QCLOUD_BLE_QIOT_DEVICE_H
#define QCLOUD_BLE_QIOT_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

void property_power_switch(const char *data, uint16_t len);
void report_reply_blink(void);
void action_led_blink(int ms);
#ifdef __cplusplus
}
#endif

#endif  // QCLOUD_BLE_QIOT_DEVICE_H
