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
#ifndef QCLOUD_BLE_QIOT_LLSYNC_DATA_H
#define QCLOUD_BLE_QIOT_LLSYNC_DATA_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <ble_qiot_export.h>

#define BLE_QIOT_CONTROL_DATA_TYPE          (0x00)
#define BLE_QIOT_GET_STATUS_REPLY_DATA_TYPE (0x22)

#define BLE_QIOT_GET_STATUS_REPLY_HEADER_LEN (4)
#define BLE_QIOT_DATA_FIXED_HEADER_LEN       (3)

// handle property request
ble_qiot_ret_status_t ble_lldata_property_request_handle(const char *in_buf, int buf_len);
// handle property reply
ble_qiot_ret_status_t ble_lldata_property_reply_handle(uint8_t type, const char *in_buf, int buf_len);

// handle event data
ble_qiot_ret_status_t ble_lldata_event_handle(uint8_t id, const char *in_buf, int len);

// handle action data
ble_qiot_ret_status_t ble_lldata_action_handle(uint8_t id, const char *in_buf, int len);

ble_qiot_ret_status_t ble_user_property_get_report_data(void);
#ifdef __cplusplus
}
#endif
#endif  // QCLOUD_BLE_QIOT_LLSYNC_DATA_H
