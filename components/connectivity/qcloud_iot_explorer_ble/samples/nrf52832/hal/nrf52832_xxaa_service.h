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

#ifndef __NRF52832_XXAA_SERVICE_H
#define __NRF52832_XXAA_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "ble_qiot_export.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"

#define BLE_PRIV_DEF(_name)      \
    static ble_priv_cfg_s _name; \
    NRF_SDH_BLE_OBSERVER(_name##_obs, BLE_LBS_BLE_OBSERVER_PRIO, iot_on_ble_evt, &_name)

typedef struct _ble_priv_cfg_s ble_priv_cfg_s;

typedef void (*indi_resp_cb)(uint8_t res);

// save BLE service configuration, such as handle, callback function, and so on, you can edit it as your requirements
struct _ble_priv_cfg_s {
    uint16_t service_handle;
    uint8_t  uuid_type;  // UUID type for the service.

    ble_gatts_char_handles_t device_info_char_handle;
    ble_gatts_char_handles_t data_char_handle;
    ble_gatts_char_handles_t event_char_handle;

    // characteristic write callback function
    ble_on_write_cb iot_device_info_write_handler;
    ble_on_write_cb iot_data_write_handler;

    uint16_t conn_handle; /**< Handle of the current connection (as provided by the BLE stack, is
                             BLE_CONN_HANDLE_INVALID if not in a connection). */
};

ble_priv_cfg_s *ble_get_priv_cfg(void);
nrf_ble_gatt_t *ble_get_gatt_instance(void);
void            iot_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context);
void            ble_services_add(const qiot_service_init_s *p_service);

#ifdef __cplusplus
}
#endif

#endif
