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
#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "ble_qiot_export.h"
#include "ble_qiot_service.h"
#include "ble_qiot_import.h"

// add std head file here
#include <stdint.h>

// add ble qiot head file here
#include "ble_qiot_import.h"

#include "ble_qiot_config.h"

#include "esp_bt_device.h"
#include "esp_spi_flash.h"

// divece info which defined in explorer platform
#define PRODUCT_ID  "1NBPCQ7L2O"
#define DEVICE_NAME "123"
#define SECRET_KEY  "q/WQFemC2ejNZTNWMqHerA=="


int ble_get_product_id(char *product_id)
{
    memcpy(product_id, PRODUCT_ID, strlen(PRODUCT_ID));

    return 0;
}

int ble_get_device_name(char *device_name)
{
    memcpy(device_name, DEVICE_NAME, strlen(DEVICE_NAME));

    return strlen(DEVICE_NAME);
}

int ble_get_psk(char *psk)
{
    memcpy(psk, SECRET_KEY, strlen(SECRET_KEY));

    return 0;
}

int ble_get_mac(char *mac)
{
    char *address = (char *)esp_bt_dev_get_address();
    memcpy(mac, address, 6);

    return 0;
}

int ble_write_flash(uint32_t flash_addr, const char *write_buf, uint16_t write_len)
{
    int ret = spi_flash_erase_range(flash_addr, BLE_QIOT_RECORD_FLASH_PAGESIZE);
    ret = spi_flash_write(flash_addr, write_buf, write_len);

    return ret == ESP_OK ? write_len : ret;
}

int ble_read_flash(uint32_t flash_addr, char *read_buf, uint16_t read_len)
{
    int ret = spi_flash_read(flash_addr, read_buf, read_len);

    return ret == ESP_OK ? read_len : ret;
}

ble_timer_t ble_timer_create(uint8_t type, ble_timer_cb timeout_handle)
{
    return NULL;
}

ble_qiot_ret_status_t ble_timer_start(ble_timer_t timer_id, uint32_t period)
{
    return BLE_QIOT_RS_OK;
}

ble_qiot_ret_status_t ble_timer_stop(ble_timer_t timer_id)
{
    return BLE_QIOT_RS_OK;
}

ble_qiot_ret_status_t ble_timer_delete(ble_timer_t timer_id)
{
    return BLE_QIOT_RS_OK;
}

// should return ATT_MTU - 3
uint16_t ble_get_user_data_mtu_size(void)
{
    // esp32 can not get mtu，use default mtu
    return 20;
}

#ifdef __cplusplus
}
#endif
