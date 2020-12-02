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
#define BLE_QIOT_BUTTON_BROADCAST 0
#if (1 == BLE_QIOT_BUTTON_BROADCAST)
#define BLE_QIOT_BIND_TIMEOUT (2 * 60 * 1000)  // unit: ms
#endif

// some data like integer need to be transmitted in a certain byte order, defined it according to your device
#define __ORDER_LITTLE_ENDIAN__ 1234
#define __ORDER_BIG_ENDIAN__    4321
#define __BYTE_ORDER__          __ORDER_LITTLE_ENDIAN__

// in some BLE stack ble_qiot_log_hex() maybe not work, user can use there own hexdump function
#define BLE_QIOT_USER_DEFINE_HEDUMP 0

#if (1 == BLE_QIOT_USER_DEFINE_HEDUMP)
// add your code here like this
// #define ble_qiot_log_hex(level, hex_name, data, data_len) \
//    do { \
//        MY_RAW_LOG("\r\nble qiot dump: %s, length: %d\r\n", hex_name, data_len); \
//        MY_RAW_HEXDUMP_(data, data_len); \
//    } while(0)

// or use your own hexdump function with same definition
// void ble_qiot_log_hex(e_ble_qiot_log_level level, const char *hex_name, const char *data, uint32_t data_len);
#endif  // BLE_QIOT_USER_DEFINE_HEDUMP

// Macro for logging a formatted string, the function must printf raw string without any color, prefix, newline or
// timestamp
#define BLE_QIOT_LOG_PRINT(...) printf(__VA_ARGS__)

// nrf52832xxAA Flash size is 512KB, nrf52832xxAB Flash size is 512KB, be carefol of the address!
#define BLE_QIOT_RECORD_FLASH_ADDR     0x7e000  // qiot data storage address
#define BLE_QIOT_RECORD_FLASH_PAGESIZE 4096     // flash page size, see chip datasheet
#define BLE_QIOT_RECORD_FLASH_PAGENUM  2        // how many pages qiot use

// the following definition will affect the stack that LLSync used, the minimum value tested is
// 2048 (BLE_QIOT_EVENT_MAX_SIZE is 128, BLE_QIOT_EVENT_BUF_SIZE is 23) the max length that llsync event data, depends
// on the length of user data reported to Tencent Lianlian at a time
#define BLE_QIOT_EVENT_MAX_SIZE (128)
// the minimum between BLE_QIOT_EVENT_MAX_SIZE and mtu
#define BLE_QIOT_EVENT_BUF_SIZE (23)

#define BLE_QIOT_INCLUDE_PROPERTY

#ifdef __cplusplus
}
#endif

#endif  // QCLOUD_BLE_QIOT_CONFIG_H
