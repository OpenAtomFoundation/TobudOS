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

#include "nrf52832_xxaa_service.h"

#include <stdint.h>
#include <string.h>

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "boards.h"
#include "app_timer.h"
#include "app_button.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "ble_qiot_service.h"
#include "ble_qiot_import.h"

BLE_PRIV_DEF(priv_cfg);
NRF_BLE_GATT_DEF(m_gatt); /**< GATT module instance. */

#define ATTRIBUTE_VALUE_MAX_LEN 256

ble_priv_cfg_s *ble_get_priv_cfg(void)
{
    return &priv_cfg;
}

nrf_ble_gatt_t *ble_get_gatt_instance(void)
{
    return &m_gatt;
}

// on_write event
static void on_write(ble_priv_cfg_s *p_cfg, ble_evt_t const *p_ble_evt)
{
    ble_gatts_evt_write_t const *p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    // print raw data for debug
    NRF_LOG_INFO("dump write data, len %d", p_evt_write->len);
    NRF_LOG_RAW_HEXDUMP_INFO(p_evt_write->data, p_evt_write->len);

    if ((p_evt_write->handle == p_cfg->device_info_char_handle.value_handle) &&
        (p_cfg->iot_device_info_write_handler != NULL)) {
        p_cfg->iot_device_info_write_handler(p_evt_write->data, p_evt_write->len);
    } else if ((p_evt_write->handle == p_cfg->data_char_handle.value_handle) &&
               (p_cfg->iot_data_write_handler != NULL)) {
        p_cfg->iot_data_write_handler(p_evt_write->data, p_evt_write->len);
    }
}

void iot_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context)
{
    ret_code_t      err_code;
    ble_priv_cfg_s *p_cfg = (ble_priv_cfg_s *)p_context;

    NRF_LOG_INFO("evt_id %d", p_ble_evt->header.evt_id);
    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            // on_connect(p_hrs, p_ble_evt);
            NRF_LOG_INFO("Connected");
            p_cfg->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            ble_gap_connect_cb();
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            // on_disconnect(p_hrs, p_ble_evt);
            NRF_LOG_INFO("Disconnected");
            p_cfg->conn_handle = BLE_CONN_HANDLE_INVALID;
            ble_gap_disconnect_cb();

            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_cfg, p_ble_evt);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(p_cfg->conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            // NRF_LOG_INFO("err_code %d", err_code);
            break;

        case BLE_GATTS_EVT_HVC:
            NRF_LOG_INFO("indication response %d", p_ble_evt->evt.gatts_evt.params.hvn_tx_complete.count);
            break;
#if defined(S132)
        case BLE_GAP_EVT_PHY_UPDATE_REQUEST: {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys = {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;
#endif

        default:
            // No implementation needed.
            break;
    }
}

// add services and characteristic
void ble_services_add(const qiot_service_init_s *p_service)
{
    ret_code_t err_code;

    ble_uuid_t    service_uuid;
    ble_uuid128_t ble_uuid128;

    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          char_uuid;
    ble_gatts_attr_md_t attr_md;
    ble_gatts_attr_md_t user_descr_attr_md;
    ble_gatts_attr_md_t cccd_md;

    ble_priv_cfg_s *p_cfg;

    p_cfg = ble_get_priv_cfg();

    /*************************************************************************/
    // if it's necessary to save those configuration in private configuration
    p_cfg->iot_device_info_write_handler = p_service->device_info.on_write;
    p_cfg->iot_data_write_handler        = p_service->data.on_write;

    /*************************************************************************/
    // add service

    // set uuid128 to ble stack, return uuid type
    memcpy(ble_uuid128.uuid128, p_service->service_uuid128, sizeof(ble_uuid128.uuid128));
    err_code = sd_ble_uuid_vs_add(&ble_uuid128, &p_cfg->uuid_type);

    // set service uuid
    service_uuid.type = p_cfg->uuid_type;
    service_uuid.uuid = IOT_BLE_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service_uuid, &p_cfg->service_handle);
    if (err_code != NRF_SUCCESS) {
        NRF_LOG_ERROR("err at file %s, line %d, err_code %d \n", __FILE__, __LINE__, err_code);
        return;
    }

    /*************************************************************************/
    // set device info characteristic, uuid 0xFEE1
    memset(&char_md, 0, sizeof(char_md));
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    memset(&char_uuid, 0, sizeof(char_uuid));
    memset(&attr_md, 0, sizeof(attr_md));
    memset(&user_descr_attr_md, 0, sizeof(user_descr_attr_md));
    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    // BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&cccd_md.write_perm);
    cccd_md.rd_auth = 0;
    cccd_md.wr_auth = 0;
    cccd_md.vlen    = 1;
    cccd_md.vloc    = BLE_GATTS_VLOC_STACK;

    char_md.char_props.broadcast      = ((p_service->device_info.gatt_char_props & GATT_CHAR_BROADCAST) ? 1 : 0);
    char_md.char_props.read           = ((p_service->device_info.gatt_char_props & GATT_CHAR_READ) ? 1 : 0);
    char_md.char_props.write_wo_resp  = ((p_service->device_info.gatt_char_props & GATT_CHAR_WRITE_WO_RESP) ? 1 : 0);
    char_md.char_props.write          = ((p_service->device_info.gatt_char_props & GATT_CHAR_WRITE) ? 1 : 0);
    char_md.char_props.notify         = ((p_service->device_info.gatt_char_props & GATT_CHAR_NOTIFY) ? 1 : 0);
    char_md.char_props.indicate       = ((p_service->device_info.gatt_char_props & GATT_CHAR_INDICATE) ? 1 : 0);
    char_md.char_props.auth_signed_wr = ((p_service->device_info.gatt_char_props & GATT_CHAR_AUTH_SIGNED_WR) ? 1 : 0);

    char_md.p_char_pf      = NULL;
    char_md.p_user_desc_md = NULL;
    char_md.p_cccd_md      = &cccd_md;
    char_md.p_sccd_md      = NULL;

    char_uuid.type = p_cfg->uuid_type;
    char_uuid.uuid = p_service->device_info.uuid16;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;

    attr_char_value.p_uuid    = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(uint8_t) * ATTRIBUTE_VALUE_MAX_LEN;
    attr_char_value.p_value   = NULL;

    err_code = sd_ble_gatts_characteristic_add(p_cfg->service_handle, &char_md, &attr_char_value,
                                               &p_cfg->device_info_char_handle);
    if (err_code != NRF_SUCCESS) {
        NRF_LOG_ERROR("err at file %s, line %d, err_code %d \n", __FILE__, __LINE__, err_code);
        return;
    }

    /*************************************************************************/
    // set data characteristic, uuid 0xFEE2
    memset(&char_md, 0, sizeof(char_md));
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    memset(&char_uuid, 0, sizeof(char_uuid));
    memset(&attr_md, 0, sizeof(attr_md));
    memset(&user_descr_attr_md, 0, sizeof(user_descr_attr_md));
    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    // BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&cccd_md.write_perm);
    cccd_md.rd_auth = 0;
    cccd_md.wr_auth = 0;
    cccd_md.vlen    = 1;
    cccd_md.vloc    = BLE_GATTS_VLOC_STACK;

    char_md.char_props.broadcast      = ((p_service->data.gatt_char_props & GATT_CHAR_BROADCAST) ? 1 : 0);
    char_md.char_props.read           = ((p_service->data.gatt_char_props & GATT_CHAR_READ) ? 1 : 0);
    char_md.char_props.write_wo_resp  = ((p_service->data.gatt_char_props & GATT_CHAR_WRITE_WO_RESP) ? 1 : 0);
    char_md.char_props.write          = ((p_service->data.gatt_char_props & GATT_CHAR_WRITE) ? 1 : 0);
    char_md.char_props.notify         = ((p_service->data.gatt_char_props & GATT_CHAR_NOTIFY) ? 1 : 0);
    char_md.char_props.indicate       = ((p_service->data.gatt_char_props & GATT_CHAR_INDICATE) ? 1 : 0);
    char_md.char_props.auth_signed_wr = ((p_service->data.gatt_char_props & GATT_CHAR_AUTH_SIGNED_WR) ? 1 : 0);

    char_md.p_char_pf      = NULL;
    char_md.p_user_desc_md = NULL;
    char_md.p_cccd_md      = &cccd_md;
    char_md.p_sccd_md      = NULL;

    char_uuid.type = p_cfg->uuid_type;
    char_uuid.uuid = p_service->data.uuid16;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;

    attr_char_value.p_uuid    = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(uint8_t) * ATTRIBUTE_VALUE_MAX_LEN;
    attr_char_value.p_value   = NULL;

    err_code =
        sd_ble_gatts_characteristic_add(p_cfg->service_handle, &char_md, &attr_char_value, &p_cfg->data_char_handle);
    if (err_code != NRF_SUCCESS) {
        NRF_LOG_ERROR("err at file %s, line %d, err_code %d \n", __FILE__, __LINE__, err_code);
        return;
    }

    /*************************************************************************/
    // set event characteristic, uuid 0xFEE3
    memset(&char_md, 0, sizeof(char_md));
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    memset(&char_uuid, 0, sizeof(char_uuid));
    memset(&attr_md, 0, sizeof(attr_md));
    memset(&user_descr_attr_md, 0, sizeof(user_descr_attr_md));
    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    // BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&cccd_md.write_perm);
    cccd_md.rd_auth = 0;
    cccd_md.wr_auth = 0;
    cccd_md.vlen    = 1;
    cccd_md.vloc    = BLE_GATTS_VLOC_STACK;

    char_md.char_props.broadcast      = ((p_service->event.gatt_char_props & GATT_CHAR_BROADCAST) ? 1 : 0);
    char_md.char_props.read           = ((p_service->event.gatt_char_props & GATT_CHAR_READ) ? 1 : 0);
    char_md.char_props.write_wo_resp  = ((p_service->event.gatt_char_props & GATT_CHAR_WRITE_WO_RESP) ? 1 : 0);
    char_md.char_props.write          = ((p_service->event.gatt_char_props & GATT_CHAR_WRITE) ? 1 : 0);
    char_md.char_props.notify         = ((p_service->event.gatt_char_props & GATT_CHAR_NOTIFY) ? 1 : 0);
    char_md.char_props.indicate       = ((p_service->event.gatt_char_props & GATT_CHAR_INDICATE) ? 1 : 0);
    char_md.char_props.auth_signed_wr = ((p_service->event.gatt_char_props & GATT_CHAR_AUTH_SIGNED_WR) ? 1 : 0);

    char_md.p_char_pf      = NULL;
    char_md.p_user_desc_md = NULL;
    char_md.p_cccd_md      = &cccd_md;
    char_md.p_sccd_md      = NULL;

    char_uuid.type = p_cfg->uuid_type;
    char_uuid.uuid = p_service->event.uuid16;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;

    attr_char_value.p_uuid    = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;
    // attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_len  = sizeof(uint8_t) * 10;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(uint8_t) * ATTRIBUTE_VALUE_MAX_LEN;
    attr_char_value.p_value   = "hello";

    err_code =
        sd_ble_gatts_characteristic_add(p_cfg->service_handle, &char_md, &attr_char_value, &p_cfg->event_char_handle);
    if (err_code != NRF_SUCCESS) {
        NRF_LOG_ERROR("err at file %s, line %d, err_code %d \n", __FILE__, __LINE__, err_code);
        return;
    }
}
