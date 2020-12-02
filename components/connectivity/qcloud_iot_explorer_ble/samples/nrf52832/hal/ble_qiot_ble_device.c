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
#include "nrf52832_xxaa_service.h"

// add std head file here
#include <stdint.h>

// add ble qiot head file here
#include "ble_qiot_import.h"

// add sdk head file here
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_err.h"
#include "ble_gap.h"
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
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "ble_qiot_config.h"

#include "flash_storage.h"

// divece info which defined in explorer platform
#define PRODUCT_ID  "LR5NRSX9F5"
#define DEVICE_NAME "ble01"
#define SECRET_KEY  "uG2TRl0hnCNsuLgls/65Bg=="

static uint8_t m_adv_handle =
    BLE_GAP_ADV_SET_HANDLE_NOT_SET; /**< Advertising handle used to identify an advertising set. */
static uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX]; /**< Buffer for storing an encoded advertising set. */
static uint8_t m_enc_scan_response_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX]; /**< Buffer for storing an encoded scan data. */

/**@brief Struct that contains pointers to the encoded advertising data. */
static ble_gap_adv_data_t m_adv_data = {
    .adv_data      = {.p_data = m_enc_advdata, .len = BLE_GAP_ADV_SET_DATA_SIZE_MAX},
    .scan_rsp_data = {.p_data = m_enc_scan_response_data, .len = BLE_GAP_ADV_SET_DATA_SIZE_MAX

    }};

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
    uint32_t       err_code;
    ble_gap_addr_t mac_info;

    err_code = sd_ble_gap_addr_get(&mac_info);
    if (NRF_SUCCESS != err_code) {
        NRF_LOG_ERROR("Get MAC error, ret %d", err_code);
        return err_code;
    }
    mac[0] = mac_info.addr[5];
    mac[1] = mac_info.addr[4];
    mac[2] = mac_info.addr[3];
    mac[3] = mac_info.addr[2];
    mac[4] = mac_info.addr[1];
    mac[5] = mac_info.addr[0];

    return 0;
}

int ble_write_flash(uint32_t flash_addr, const char *write_buf, uint16_t write_len)
{
    return fstorage_write(flash_addr, write_len, write_buf);
}

int ble_read_flash(uint32_t flash_addr, char *read_buf, uint16_t read_len)
{
    return fstorage_read(flash_addr, read_len, read_buf);
}

ble_qiot_ret_status_t ble_advertising_start(adv_info_s *adv)
{
    ret_code_t err_code;

    ble_gap_adv_params_t adv_params;
    ble_advdata_t        advdata;

    // set Scan Response data, here set service uuid
    ble_advdata_t srdata;
    memset(&srdata, 0, sizeof(srdata));
    ble_uuid_t adv_uuids[1];
    adv_uuids[0].uuid              = adv->uuid_info.uuids[0];
    adv_uuids[0].type              = BLE_UUID_TYPE_BLE;
    srdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    srdata.uuids_complete.p_uuids  = adv_uuids;
    err_code = ble_advdata_encode(&srdata, m_adv_data.scan_rsp_data.p_data, &m_adv_data.scan_rsp_data.len);
    APP_ERROR_CHECK(err_code);

    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));
    advdata.name_type = BLE_ADVDATA_FULL_NAME;
    advdata.flags     = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    // set Advertising Data
    ble_advdata_manuf_data_t manf_data;
    memset(&manf_data, 0, sizeof(manf_data));
    manf_data.company_identifier  = adv->manufacturer_info.company_identifier;
    manf_data.data.size           = adv->manufacturer_info.adv_data_len;
    manf_data.data.p_data         = adv->manufacturer_info.adv_data;
    advdata.p_manuf_specific_data = &manf_data;

    err_code = ble_advdata_encode(&advdata, m_adv_data.adv_data.p_data, &m_adv_data.adv_data.len);
    APP_ERROR_CHECK(err_code);

    // Set advertising parameters.
    memset(&adv_params, 0, sizeof(adv_params));

    adv_params.primary_phy     = BLE_GAP_PHY_1MBPS;
    adv_params.duration        = APP_ADV_DURATION;
    adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
    adv_params.p_peer_addr     = NULL;
    adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;
    adv_params.interval        = APP_ADV_INTERVAL;

    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, &adv_params);
    if (NRF_SUCCESS != err_code) {
        NRF_LOG_ERROR("sd_ble_gap_adv_set_configure error, ret %d", err_code);
        return BLE_QIOT_RS_ERR;
    }

    err_code = sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CONN_CFG_TAG);
    if (NRF_SUCCESS != err_code) {
        NRF_LOG_ERROR("sd_ble_gap_adv_start error, ret %d", err_code);
        return BLE_QIOT_RS_ERR;
    }

    return BLE_QIOT_RS_OK;
}

ble_qiot_ret_status_t ble_advertising_stop(void)
{
    ret_code_t err_code;

    err_code = sd_ble_gap_adv_stop(m_adv_handle);
    if (NRF_SUCCESS != err_code) {
        NRF_LOG_ERROR("sd_ble_gap_adv_stop error, ret %d", err_code);
        return BLE_QIOT_RS_ERR;
    }

    return BLE_QIOT_RS_OK;
}

// define a static timer used in bind for example, a good way is used malloc
APP_TIMER_DEF(ble_bind_timer);

ble_timer_t ble_timer_create(uint8_t type, ble_timer_cb timeout_handle)
{
    app_timer_create((app_timer_id_t const *)&ble_bind_timer,
                     type == BLE_TIMER_ONE_SHOT_TYPE ? APP_TIMER_MODE_SINGLE_SHOT : APP_TIMER_MODE_REPEATED,
                     (app_timer_timeout_handler_t)timeout_handle);
    NRF_LOG_INFO("create timer id %p", (ble_timer_t)ble_bind_timer);

    return (ble_timer_t)ble_bind_timer;
}

ble_qiot_ret_status_t ble_timer_start(ble_timer_t timer_id, uint32_t period)
{
    ret_code_t ret = 0;

    ret = app_timer_start((app_timer_id_t)timer_id, APP_TIMER_TICKS(period), NULL);
    NRF_LOG_INFO("start timer id %p, tick %d, ret %d", timer_id, APP_TIMER_TICKS(period), ret);

    return 0 == ret ? BLE_QIOT_RS_OK : BLE_QIOT_RS_ERR;
}

ble_qiot_ret_status_t ble_timer_stop(ble_timer_t timer_id)
{
    ret_code_t ret = 0;

    ret = app_timer_stop(timer_id);
    NRF_LOG_INFO("stop timer id %p, ret %d", timer_id, ret);

    return 0 == ret ? BLE_QIOT_RS_OK : BLE_QIOT_RS_ERR;
}

ble_qiot_ret_status_t ble_timer_delete(ble_timer_t timer_id)
{
    // do nothing
    return BLE_QIOT_RS_OK;
}

ble_qiot_ret_status_t ble_send_notify(uint8_t *buf, uint8_t len)
{
    ret_code_t             err_code;
    ble_gatts_hvx_params_t params;
    uint16_t               data_len = len;
    ble_priv_cfg_s *       p_cfg;

    p_cfg = ble_get_priv_cfg();

    memset(&params, 0, sizeof(params));
    params.type   = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_cfg->event_char_handle.value_handle;  // handle, corresponds to the same characteristic
    params.p_data = buf;
    params.p_len  = &data_len;  // do not large than length set in characteristic

    err_code = sd_ble_gatts_hvx(p_cfg->conn_handle, &params);
    if (NRF_SUCCESS != err_code) {
        NRF_LOG_ERROR("sd_ble_gatts_hvx error, ret %d", err_code);
        return BLE_QIOT_RS_ERR;
    }

    return BLE_QIOT_RS_OK;
}

// should return ATT_MTU - 3
uint16_t ble_get_user_data_mtu_size(void)
{
    ble_priv_cfg_s *p_cfg;
    nrf_ble_gatt_t *p_gatt;

    p_cfg  = ble_get_priv_cfg();
    p_gatt = ble_get_gatt_instance();

    return nrf_ble_gatt_eff_mtu_get(p_gatt, p_cfg->conn_handle) - 3;
}

void property_power_switch(const char *data, uint16_t len)
{
    if (data[0]) {
        bsp_board_led_on(BSP_BOARD_LED_0);
        NRF_LOG_INFO("Received LED ON!");
    } else {
        bsp_board_led_off(BSP_BOARD_LED_0);
        NRF_LOG_INFO("Received LED OFF!");
    }
    return;
}

void action_led_blink(int ms)
{
    bsp_board_led_on(BSP_BOARD_LED_1);
    nrf_delay_ms(ms);
    bsp_board_led_off(BSP_BOARD_LED_1);
    nrf_delay_ms(ms);
    bsp_board_led_on(BSP_BOARD_LED_1);
    nrf_delay_ms(ms);
    bsp_board_led_off(BSP_BOARD_LED_1);
    nrf_delay_ms(ms);
    bsp_board_led_on(BSP_BOARD_LED_1);
    nrf_delay_ms(ms);
    bsp_board_led_off(BSP_BOARD_LED_1);
}

void report_reply_blink(void)
{
    bsp_board_led_on(BSP_BOARD_LED_2);
    nrf_delay_ms(200);
    bsp_board_led_off(BSP_BOARD_LED_2);
    nrf_delay_ms(200);
    bsp_board_led_on(BSP_BOARD_LED_2);
    nrf_delay_ms(200);
    bsp_board_led_off(BSP_BOARD_LED_2);
}

#ifdef __cplusplus
}
#endif
