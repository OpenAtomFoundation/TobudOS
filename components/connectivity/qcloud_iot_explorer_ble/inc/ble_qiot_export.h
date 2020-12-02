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
#ifndef QCLOUD_BLE_QIOT_EXPORT_H
#define QCLOUD_BLE_QIOT_EXPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "ble_qiot_common.h"

// Tencent Company ID
#define TENCENT_COMPANY_IDENTIFIER  0xFEE7
#define TENCENT_COMPANY_IDENTIFIER2 0xFEBA

#define IOT_BLE_UUID_BASE                                                                              \
    {                                                                                                  \
        0xe2, 0xa4, 0x1b, 0x54, 0x93, 0xe4, 0x6a, 0xb5, 0x20, 0x4e, 0xd0, 0x65, 0x00, 0x00, 0x00, 0x00 \
    }

// llsync services uuid
#define IOT_BLE_UUID_SERVICE 0xFFE0

// characteristics uuid
#define IOT_BLE_UUID_DEVICE_INFO 0xFFE1  // used to connection and identity authentication
#define IOT_BLE_UUID_DATA        0xFFE2  // used to send data to the device from server
#define IOT_BLE_UUID_EVENT       0xFFE3  // used to send data to the server from device

typedef enum {
    GATT_CHAR_BROADCAST      = (1 << 0),  // Broadcasting of the value permitted.
    GATT_CHAR_READ           = (1 << 1),  // Reading the value permitted.
    GATT_CHAR_WRITE_WO_RESP  = (1 << 2),  // Writing the value with Write Command permitted.
    GATT_CHAR_WRITE          = (1 << 3),  // Writing the value with Write Request permitted.
    GATT_CHAR_NOTIFY         = (1 << 4),  // Notification of the value permitted.
    GATT_CHAR_INDICATE       = (1 << 5),  // Indications of the value permitted.
    GATT_CHAR_AUTH_SIGNED_WR = (1 << 6),  // Writing the value with Signed Write Command permitted.
} char_props_s;

// the callback function prototype definition for the characteristics
typedef void (*ble_on_write_cb)(const uint8_t *buf, uint16_t len);

// the characteristics attributes
typedef struct {
    uint16_t        uuid16;
    uint8_t         gatt_char_props;
    ble_on_write_cb on_write;
} qiot_char_s;

// the service attributes
typedef struct {
    uint16_t service_uuid16;
    uint8_t  service_uuid128[16];
    uint16_t gatt_max_mtu;

    qiot_char_s device_info;
    qiot_char_s data;
    qiot_char_s event;
} qiot_service_init_s;

typedef enum {
    BLE_QIOT_RS_OK             = 0,   // success
    BLE_QIOT_RS_ERR            = -1,  // normal error
    BLE_QIOT_RS_ERR_FLASH      = -2,  // flash error
    BLE_QIOT_RS_ERR_PARA       = -3,  // parameters error
    BLE_QIOT_RS_VALID_SIGN_ERR = -4,
} ble_qiot_ret_status_t;

/**
 * @brief get llsync services context
 *
 * @return llsync services struct
 */
const qiot_service_init_s *ble_get_qiot_services(void);

/**
 * @brief llsync sdck initialize
 * @note  you should called it before any other sdk api
 * @return BLE_QIOT_RS_OK is success, other is error
 */
ble_qiot_ret_status_t ble_qiot_explorer_init(void);

/**
 * @brief  get property of the device from the server
 * @note   the property will be received from IOT_BLE_UUID_DATA if success
 * @return BLE_QIOT_RS_OK is success, other is error. if success, the data from server will come to
 */
ble_qiot_ret_status_t ble_event_get_status(void);

/**
 * @brief  report property of the device to the server
 * @note   the reply will be received from IOT_BLE_UUID_DATA if success
 * @return BLE_QIOT_RS_OK is success, other is error
 */
ble_qiot_ret_status_t ble_event_report_property(void);

/**
 * @brief  report mtu of the device to the server
 * @note   report mtu to the server for optimizing bandwidth usage. this mtu equals (ATT_MTU - 3)
 * @return BLE_QIOT_RS_OK is success, other is error
 */
ble_qiot_ret_status_t ble_event_report_device_info(void);

/**
 * @brief  post event to the server
 * @param  event_id id of the event
 * @note   the reply will be received from IOT_BLE_UUID_DATA if success
 * @return BLE_QIOT_RS_OK is success, other is error
 */
ble_qiot_ret_status_t ble_event_post(uint8_t event_id);

/**
 * @brief  start llsync advertising
 * @note   broadcast data according to the device bind state, reference to llsync protocol
 * @return BLE_QIOT_RS_OK is success, other is error
 */
ble_qiot_ret_status_t ble_qiot_advertising_start(void);

/**
 * @brief  stop advertising
 * @return BLE_QIOT_RS_OK is success, other is error
 */
ble_qiot_ret_status_t ble_qiot_advertising_stop(void);

/**
 * @brief device info write callbcak, call the function when characteristic IOT_BLE_UUID_DEVICE_INFO received data
 * @param buf a pointer point to the data
 * @param len data length
 * @return none
 */
void ble_device_info_write_cb(const uint8_t *buf, uint16_t len);

/**
 * @brief data write callback, call the function when characteristic IOT_BLE_UUID_DATA received data
 * @param buf a pointer point to the data
 * @param len data length
 * @return none
 */
void ble_lldata_write_cb(const uint8_t *buf, uint16_t len);

/**
 * @brief gap event connect call-back, when gap get ble connect event, use this function
 *       tell qiot ble sdk
 * @return none
 */
void ble_gap_connect_cb(void);

/**
 * @brief gap event disconnect call-back, when gap get ble disconnect event, use this function
 *       tell qiot ble sdk
 * @return none
 */
void ble_gap_disconnect_cb(void);

#ifdef __cplusplus
}
#endif
#endif  // QCLOUD_BLE_QIOT_EXPORT_H
