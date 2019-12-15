/**
 * Copyright (c) 2018 - 2019, Nordic Semiconductor ASA
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

#include "pm_m.h"
#include "ble_conn_state.h"
#include "ble_m.h"
#include "cli_m.h"
#include "fds.h"
#include "sdk_errors.h"
#include "sdk_config.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "nfc_pair_lib_m.h"

#include "nrf_log.h"


/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    ret_code_t err_code;

    pm_handler_on_pm_evt(p_evt);
    pm_handler_flash_clean(p_evt);

    switch (p_evt->evt_id)
    {
        case PM_EVT_CONN_SEC_SUCCEEDED:
        {
            /* Restore default Peer Manager configuration */
            ble_gap_sec_params_t sec_params;
            memset(&sec_params, 0, sizeof(sec_params));

            sec_params.bond           = BLE_SEC_PARAM_BOND;
            sec_params.mitm           = BLE_SEC_PARAM_MITM;
            sec_params.lesc           = BLE_SEC_PARAM_LESC;
            sec_params.keypress       = BLE_SEC_PARAM_KEYPRESS;
            sec_params.io_caps        = BLE_SEC_PARAM_IO_CAPS;
            sec_params.oob            = BLE_SEC_PARAM_OOB;
            sec_params.min_key_size   = BLE_SEC_PARAM_MIN_KEY_SIZE;
            sec_params.max_key_size   = BLE_SEC_PARAM_MAX_KEY_SIZE;
            sec_params.kdist_own.enc  = BLE_SEC_PARAM_KDIST_OWN_ENC;
            sec_params.kdist_own.id   = BLE_SEC_PARAM_KDIST_OWN_ID;
            sec_params.kdist_peer.enc = BLE_SEC_PARAM_KDIST_PEER_ENC;
            sec_params.kdist_peer.id  = BLE_SEC_PARAM_KDIST_PEER_ID;

            err_code = pm_sec_params_set(&sec_params);
            APP_ERROR_CHECK(err_code);
        }
        break;

        case PM_EVT_CONN_SEC_FAILED:
        {
            if (p_evt->params.conn_sec_failed.error == PM_CONN_SEC_ERROR_PIN_OR_KEY_MISSING)
            {
                // Rebond if one party has lost its keys.
                err_code = pm_conn_secure(p_evt->conn_handle, true);

                if (err_code != NRF_ERROR_BUSY)
                {
                    APP_ERROR_CHECK(err_code);
                }
            }
        }
        break;

        /** @snippet [NFC Pairing Lib usage_1] */
        case PM_EVT_CONN_SEC_PARAMS_REQ:
        {
            // Send event to the NFC BLE pairing library as it may dynamically alternate
            // security parameters to achieve the highest possible security level.
            err_code = nfc_ble_pair_on_pm_params_req(p_evt);
            APP_ERROR_CHECK(err_code);
        }
        break;
        /** @snippet [NFC Pairing Lib usage_1] */

        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
        {
            bond_get();
        }
        break;

        default:
            break;
    }
}


void peer_manager_init(void)
{
    ble_gap_sec_params_t sec_params;
    ret_code_t           err_code;


    err_code = pm_init();
    APP_ERROR_CHECK(err_code);
    memset(&sec_params, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for peripheral security procedures.
    sec_params.bond           = BLE_SEC_PARAM_BOND;
    sec_params.mitm           = BLE_SEC_PARAM_MITM;
    sec_params.lesc           = BLE_SEC_PARAM_LESC;
    sec_params.keypress       = BLE_SEC_PARAM_KEYPRESS;
    sec_params.io_caps        = BLE_SEC_PARAM_IO_CAPS;
    sec_params.oob            = BLE_SEC_PARAM_OOB;
    sec_params.min_key_size   = BLE_SEC_PARAM_MIN_KEY_SIZE;
    sec_params.max_key_size   = BLE_SEC_PARAM_MAX_KEY_SIZE;
    sec_params.kdist_own.enc  = BLE_SEC_PARAM_KDIST_OWN_ENC;
    sec_params.kdist_own.id   = BLE_SEC_PARAM_KDIST_OWN_ID;
    sec_params.kdist_peer.enc = BLE_SEC_PARAM_KDIST_PEER_ENC;
    sec_params.kdist_peer.id  = BLE_SEC_PARAM_KDIST_PEER_ID;

    err_code = pm_sec_params_set(&sec_params);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}


