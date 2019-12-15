/**
 * Copyright (c) 2016 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "nrf_ble_qwrs.h"
#include <string.h>
#include "ble_srv_common.h"
#include "nrf_ble_qwr.h"

#define NRF_LOG_MODULE_NAME BLE_QWRS
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define BLE_UUID_QWRS_SERVICE             0x0001                      /**< The UUID of the QWRS Service. */
#define BLE_UUID_QWRS_LONG_CHARACTERISTIC 0x0002                      /**< The UUID of the QWRS Long Characteristic. */

#define BLE_QWRS_MAX_LONG_CHAR_LEN        NRF_BLE_QWRS_MAX_RCV_SIZE   /**< Maximum length of the QWRS Long Characteristic (in bytes). */
#define BLE_QWRS_MAX_CHAR_LEN             20                          /**< Maximum length of the QWRS Characteristic (in bytes). */

#define QWRS_BASE_UUID                  {{0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x00, 0x00, 0x40, 0x6E}} /**< Used vendor specific UUID. */

uint16_t nrf_ble_qwrs_on_qwr_evt(nrf_ble_qwrs_t *p_qwrs,
                                 nrf_ble_qwr_t * p_qwr,
                                 nrf_ble_qwr_evt_t * p_evt)
{
    if (p_qwrs->evt_handler != NULL)
    {
        nrf_ble_qwrs_evt_t cur_evt;
        cur_evt.rcv_length = NRF_BLE_QWRS_MAX_RCV_SIZE;
        ret_code_t err_code = nrf_ble_qwr_value_get(p_qwr, p_evt->attr_handle, cur_evt.rcvd_data, &cur_evt.rcv_length);
        if (err_code != NRF_SUCCESS)
        {
            NRF_LOG_ERROR("nrf_ble_qwr_value_get failed.");
            return BLE_GATT_STATUS_ATTERR_INSUF_AUTHORIZATION;
        }
        if (p_evt->evt_type == NRF_BLE_QWR_EVT_AUTH_REQUEST)
        {
            cur_evt.evt_type = BLE_QWRS_CHECK_RCVD_DATA;
            return p_qwrs->evt_handler(p_qwrs, &(cur_evt));
        }
        else if (p_evt->evt_type == NRF_BLE_QWR_EVT_EXECUTE_WRITE)
        {
            if (p_qwrs->evt_handler != NULL)
            {
                cur_evt.evt_type = BLE_QWRS_NEW_DATA_RCVD;
                return p_qwrs->evt_handler(p_qwrs, &(cur_evt));
            }
        }
    }
    return BLE_GATT_STATUS_SUCCESS;
}

uint32_t nrf_ble_qwrs_init(nrf_ble_qwrs_init_t *p_qwrs_init, nrf_ble_qwrs_t *p_qwrs)
{
    ret_code_t    err_code;
    ble_uuid_t    ble_uuid;

    // Initialize service structure.
    p_qwrs->evt_handler   = p_qwrs_init->evt_handler;
    p_qwrs->error_handler = p_qwrs_init->error_handler;
    p_qwrs->conn_handle   = BLE_CONN_HANDLE_INVALID;

    // Add service.
    ble_uuid128_t base_uuid = QWRS_BASE_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_qwrs->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_qwrs->uuid_type;
    ble_uuid.uuid = BLE_UUID_QWRS_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &p_qwrs->service_handle);
    VERIFY_SUCCESS(err_code);

    //Add Long characteristic
    ble_add_char_params_t add_char_params;

    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid             = BLE_UUID_QWRS_LONG_CHARACTERISTIC;
    add_char_params.uuid_type        = p_qwrs->uuid_type;
    add_char_params.max_len          = BLE_QWRS_MAX_LONG_CHAR_LEN;
    add_char_params.init_len         = 0;
    add_char_params.char_props.write = true;
    add_char_params.write_access     = SEC_OPEN;
    add_char_params.is_defered_write = true;

    err_code = characteristic_add(p_qwrs->service_handle,
                              &add_char_params,
                              &p_qwrs->long_charact_handles);
    VERIFY_SUCCESS(err_code);

    if (p_qwrs_init->p_qwr_ctx != NULL)
    {
        err_code = nrf_ble_qwr_attr_register(p_qwrs_init->p_qwr_ctx,
                                             p_qwrs->long_charact_handles.value_handle);
        VERIFY_SUCCESS(err_code);
    }
    return err_code;
}
