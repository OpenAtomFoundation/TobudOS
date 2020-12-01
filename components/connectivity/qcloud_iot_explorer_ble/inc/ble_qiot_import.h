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
#ifndef QCLOUD_BLE_QIOT_IMPORT_H
#define QCLOUD_BLE_QIOT_IMPORT_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include "ble_qiot_export.h"

// 16 bits service UUIDs list, use advertising type 0x02 or 0x03
typedef struct
{
    uint8_t uuid_num;
    uint16_t *uuids;
} uuid_list_s;

// advertise manufacture specific data, use advertising type 0xFF
typedef struct 
{
    uint16_t company_identifier;
    uint8_t *adv_data;
    uint8_t  adv_data_len;
} manufacturer_data_s;

typedef struct {
    uuid_list_s uuid_info;
    manufacturer_data_s manufacturer_info;
} adv_info_s;

/**
 * @brief  get device product id
 * @param  product_id  the buf storage product id, 10 bytes permanent
 * @return 0 is success, other is error
 */
int ble_get_product_id(char *product_id);

/**
 * @brief  get device name
 * @param  device_name     the buf storage device name, the max length of the device name is 48 bytes
 * @return length of device name, 0 is error
 */
int ble_get_device_name(char *device_name);

/**
 * @brief  get device secret
 * @param  psk         the buf storage secret, 24 bytes permanent
 * @return 0 is success, other is error
 */
int ble_get_psk(char *psk);

/**
 * @brief  get mac address
 * @param  mac     the buf storage mac, 6 bytes permanent
 * @return 0 is success, other is error
 */
int ble_get_mac(char *mac);

/**
 * @brief write data to flash
 * @param flash_addr write address in flash
 * @param write_buf  point to write buf
 * @param write_len  length of data to write
 * @return write_len is success, other is error
 */
int ble_write_flash(uint32_t flash_addr, const char *write_buf, uint16_t write_len);

/**
 * @brief read data from flash
 * @param flash_addr read address from flash
 * @param read_buf   point to read buf
 * @param read_len   length of data to read
 * @return read_len is success, other is error
 */
int ble_read_flash(uint32_t flash_addr, char *read_buf, uint16_t read_len);

/**
 * @brief add llsync services to ble stack
 * @param qiot_service_init_s llsync service
 * @return none
 */
void ble_services_add(const qiot_service_init_s *p_service);

/**
 * @brief start llsync advertising
 * @param adv a pointer point to advertising data
 * @return BLE_QIOT_RS_OK is success, other is error
 */
ble_qiot_ret_status_t ble_advertising_start(adv_info_s *adv);

/**
 * @brief stop advertising
 * @return BLE_QIOT_RS_OK is success, other is error
 */
ble_qiot_ret_status_t ble_advertising_stop(void);

/**
 * @brief send a notification to host, use characteristic IOT_BLE_UUID_EVENT
 * @param buf a pointer point to indication information
 * @param len indication information length
 * @return BLE_QIOT_RS_OK is success, other is error
 */
ble_qiot_ret_status_t ble_send_notify(uint8_t *buf, uint8_t len);

/**
 * @brief get the max size of data that user can used, normally is ATT_MTU - 3
 * @return the value
 */
uint16_t ble_get_user_data_mtu_size(void);

// timer type
enum {
    BLE_TIMER_ONE_SHOT_TYPE = 0,
    BLE_TIMER_PERIOD_TYPE,
    BLE_TIMER_BUTT,
};
typedef void *ble_timer_t;

// timer callback prototype
typedef void (*ble_timer_cb)(void *param);

/**
 * @brief create a timer
 * @param type timer type
 * @param timeout_handle timer callback
 * @return timer identifier is return, NULL is error
 */
ble_timer_t ble_timer_create(uint8_t type, ble_timer_cb timeout_handle);

/**
 * @brief start a timer
 * @param timer_id Timer identifier
 * @param period timer period(unit: ms)
 * @return BLE_QIOT_RS_OK is success, other is error
 */
ble_qiot_ret_status_t ble_timer_start(ble_timer_t timer_id, uint32_t period);

/**
 * @brief stop a timer
 * @param timer_id Timer identifier
 * @return BLE_QIOT_RS_OK is success, other is error
 */
ble_qiot_ret_status_t ble_timer_stop(ble_timer_t timer_id);

/**
 * @brief delete a timer
 * @param timer_id Timer identifier
 * @return BLE_QIOT_RS_OK is success, other is error
 */
ble_qiot_ret_status_t ble_timer_delete(ble_timer_t timer_id);

#if defined(__cplusplus)
}
#endif

#endif  // QCLOUD_BLE_QIOT_IMPORT_H
