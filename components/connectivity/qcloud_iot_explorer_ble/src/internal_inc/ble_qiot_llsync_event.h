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
#ifndef QCLOUD_BLE_QIOT_LLSYNC_EVENT_H
#define QCLOUD_BLE_QIOT_LLSYNC_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "ble_qiot_config.h"
#include "ble_qiot_export.h"

enum {
    BLE_QIOT_EVENT_NO_SLICE   = 0,
    BLE_QIOT_EVENT_SLICE_HEAD = 1,
    BLE_QIOT_EVENT_SLICE_BODY = 2,
    BLE_QIOT_EVENT_SLICE_FOOT = 3,
};

#define BLE_QIOT_EVENT_FIXED_HEADER_LEN (3)

// the bit 15 - 14 is slice flag, bit 13 - 0 is tlv length
#define BLE_QIOT_IS_SLICE_PACKAGE(_C) ((_C)&0XC0)
#define BLE_QIOT_IS_SLICE_HEADER(_C)  (((_C)&0XC0) == 0X40)
#define BLE_QIOT_IS_SLICE_BODY(_C)    (((_C)&0XC0) == 0X80)
#define BLE_QIOT_IS_SLICE_TAIL(_C)    (((_C)&0XC0) == 0XC0)

ble_qiot_ret_status_t ble_event_notify(uint8_t type, uint8_t *header, uint8_t header_len, const char *buf,
                                       uint16_t buf_len);

#ifdef __cplusplus
}
#endif

#endif  // QCLOUD_BLE_QIOT_LLSYNC_EVENT_H
