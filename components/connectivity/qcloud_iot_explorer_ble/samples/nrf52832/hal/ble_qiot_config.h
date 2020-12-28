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

#ifndef QCLOUD_BLE_QIOT_CONFIG_H
#define QCLOUD_BLE_QIOT_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

#define BLE_QIOT_SDK_VERSION "1.2.0"  // sdk version
#define BLE_QIOT_SDK_DEBUG   0        // sdk debug switch

// the device broadcast is controlled by the user, but we provide a mechanism to help the device save more power.
// if you want broadcast is triggered by something like press a button instead of all the time, and the broadcast
// stopped automatically in a few minutes if the device is not bind, define BLE_QIOT_BUTTON_BROADCAST is 1 and
// BLE_QIOT_BIND_TIMEOUT is the period that broadcast stopped.
// if the device in the bound state, broadcast dose not stop automatically.
#define BLE_QIOT_BUTTON_BROADCAST 1
#if (1 == BLE_QIOT_BUTTON_BROADCAST)
#define BLE_QIOT_BIND_TIMEOUT (2 * 60 * 1000)  // unit: ms
#endif

// some data like integer need to be transmitted in a certain byte order, defined it according to your device
#define __ORDER_LITTLE_ENDIAN__ 1234
#define __ORDER_BIG_ENDIAN__    4321
#define __BYTE_ORDER__          __ORDER_LITTLE_ENDIAN__

// the following definition will affect the stack that LLSync used，the minimum value tested is
// 2048（BLE_QIOT_EVENT_MAX_SIZE is 128, BLE_QIOT_EVENT_BUF_SIZE is 23 ） the max length that llsync event data, depends
// on the length of user data reported to Tencent Lianlian at a time
#define BLE_QIOT_EVENT_MAX_SIZE (512)
// the minimum between BLE_QIOT_EVENT_MAX_SIZE and mtu
#define BLE_QIOT_EVENT_BUF_SIZE (256)

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

// in some BLE stack ble_qiot_log_hex() maybe not work, user can use there own hexdump function
#define BLE_QIOT_USER_DEFINE_HEDUMP 1

#if (1 == BLE_QIOT_USER_DEFINE_HEDUMP)
#define ble_qiot_log_hex(level, hex_name, data, data_len)                          \
    do {                                                                           \
        NRF_LOG_INFO("\r\nble qiot dump: %s, length: %d\r\n", hex_name, data_len); \
        NRF_LOG_RAW_HEXDUMP_INFO(data, data_len);                                  \
    } while (0)
#endif  // BLE_QIOT_USER_DEFINE_HEDUMP

// Macro for logging a formatted string, the function must printf raw string without any color, prefix, newline or
// timestamp
#define BLE_QIOT_LOG_PRINT(...) NRF_LOG_RAW_INFO(__VA_ARGS__)

// nrf52832xxAA Flash size is 512KB, nrf52832xxAB Flash size is 512KB, be carefol of the address!
#define BLE_QIOT_RECORD_FLASH_ADDR     0x7e000  // qiot data storage address
#define BLE_QIOT_RECORD_FLASH_PAGESIZE 4096     // flash page size, see chip datasheet
#define BLE_QIOT_RECORD_FLASH_PAGENUM  2        // how many pages qiot use

#define APP_BLE_OBSERVER_PRIO 3 /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG  1 /**< A tag identifying the SoftDevice BLE configuration. */

#define APP_ADV_INTERVAL 64 /**< The advertising interval (in units of 0.625 ms; this value corresponds to 40 ms). */
#define APP_ADV_DURATION                                                                                              \
    BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED /**< The advertising time-out (in units of seconds). When set to 0, we will \
                                             never time out. */

#ifdef __cplusplus
}
#endif

#endif  // QCLOUD_BLE_QIOT_CONFIG_H
