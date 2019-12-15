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
#include "ble_m.h"
#include "nordic_common.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "peer_manager.h"
#include "nfc_pair_m.h"
#include "fds.h"
#include "nrf_fstorage.h"
#include "ble_db_discovery.h"
#include "ble_hrs_c.h"
#include "ble_bas_c.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_scan.h"

#define NRF_LOG_MODULE_NAME BLE_M
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define APP_BLE_CONN_CFG_TAG        1                                   /**< Tag for the configuration of the BLE stack. */
#define APP_BLE_OBSERVER_PRIO       3                                   /**< BLE observer priority of the application. There is no need to modify this value. */
#define APP_SOC_OBSERVER_PRIO       1                                   /**< SoC observer priority of the application. There is no need to modify this value. */

#define DEV_NAME_LEN                ((BLE_GAP_ADV_SET_DATA_SIZE_MAX + 1) - \
                                    AD_DATA_OFFSET)                     /**< Determines the device name length. */

BLE_HRS_C_DEF(m_hrs_c);                                                 /**< Heart Rate Service client module instance. */
BLE_BAS_C_DEF(m_bas_c);                                                 /**< Battery Service client module instance. */
NRF_BLE_GATT_DEF(m_gatt);                                               /**< GATT module instance. */
BLE_DB_DISCOVERY_DEF(m_db_discovery);                                   /**< Database Discovery module instance. */
NRF_BLE_SCAN_DEF(m_scan);                                               /**< Scanning Module instance. */

static bool     m_is_connected              = false;                    /**< Flag to keep track of the BLE connections with peripheral devices. */
static uint16_t m_conn_handle               = BLE_CONN_HANDLE_INVALID;  /**< Current connection handle. */
static bool     m_memory_access_in_progress = false;                    /**< Flag to keep track of the ongoing operations on persistent memory. */
static bool     m_hrs_notif_enabled         = false;                    /**< Flag indicating that HRS notification has been enabled. */


/**@brief Function for handling database discovery events.
 *
 * @details This function is a callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function forwards the events
 *          to their respective services.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 */
static void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{
    ble_hrs_on_db_disc_evt(&m_hrs_c, p_evt);
    ble_bas_on_db_disc_evt(&m_bas_c, p_evt);
}


bool ble_is_connected(void)
{
    return m_is_connected;
}


uint16_t ble_get_conn_handle(void)
{
    return m_conn_handle;
}


void ble_disconnect(void)
{
    ret_code_t err_code;

    if (m_is_connected)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        if (err_code != NRF_ERROR_INVALID_STATE)
        {
            APP_ERROR_CHECK(err_code);
        }
    }
}


void scan_start(void)
{
    ret_code_t err_code;

    // If there is any pending write to flash, defer scanning until it completes.
    if (nrf_fstorage_is_busy(NULL))
    {
        m_memory_access_in_progress = true;
        return;
    }

    err_code = nrf_ble_scan_start(&m_scan);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the advertising report BLE event.
 *
 * @param[in] p_adv_report  Advertising report from the SoftDevice.
 */
static void on_adv_report(ble_gap_evt_adv_report_t const * p_adv_report)
{
    ret_code_t  err_code;
    uint8_t   * p_adv_data;
    uint16_t    data_len;
    uint16_t    field_len;
    uint16_t    dev_name_offset = 0;
    char        dev_name[DEV_NAME_LEN];

    // Initialize advertisement report for parsing.
    p_adv_data = (uint8_t *)p_adv_report->data.p_data;
    data_len   = p_adv_report->data.len;

    // Search for advertising names.
    field_len = ble_advdata_search(p_adv_data, 
                                   data_len, 
                                   &dev_name_offset, 
                                   BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME);
    if (field_len == 0)
    {
        // Look for the short local name if it was not found as complete.
        field_len = ble_advdata_search(p_adv_data, 
                                       data_len,
                                       &dev_name_offset,
                                       BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME);
        if (field_len == 0)
        {
            // Exit if the data cannot be parsed.
            return;
        }
    }

    memcpy(dev_name, &p_adv_data[dev_name_offset], field_len);
    dev_name[field_len] = 0;

    NRF_LOG_DEBUG("Found advertising device: %s", nrf_log_push((char *)dev_name));

    // Check if the device address is the same as address taken from the NFC tag.
    if (nfc_oob_pairing_tag_match(&p_adv_report->peer_addr))
    {
        // If the address is correct, stop scanning and initiate a connection with the peripheral device.
        nrf_ble_scan_stop();

        err_code = sd_ble_gap_connect(&p_adv_report->peer_addr,
                                      &m_scan.scan_params,
                                      &m_scan.conn_params,
                                      APP_BLE_CONN_CFG_TAG);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t err_code;
    ble_gap_evt_t const * p_gap_evt = &p_ble_evt->evt.gap_evt;

    switch (p_ble_evt->header.evt_id)
    {
        // Upon connection, initiate secure bonding.
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected.");

            // Discover the peer services.
            err_code = ble_db_discovery_start(&m_db_discovery,
                                              p_ble_evt->evt.gap_evt.conn_handle);
            APP_ERROR_CHECK(err_code);

            m_is_connected = true;
            m_conn_handle  = p_ble_evt->evt.gap_evt.conn_handle;
            break;

        // Upon disconnection, reset the connection handle of the peer that disconnected
        // and invalidate data taken from the NFC tag.
        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected. conn_handle: 0x%x, reason: 0x%x",
                         p_gap_evt->conn_handle,
                         p_gap_evt->params.disconnected.reason);

            m_conn_handle       = BLE_CONN_HANDLE_INVALID;
            m_is_connected      = false;
            m_hrs_notif_enabled = false;
            nfc_oob_pairing_tag_invalidate();
            break;

        case BLE_GAP_EVT_ADV_REPORT:
            on_adv_report(&p_gap_evt->params.adv_report);
            break;

        case BLE_GAP_EVT_TIMEOUT:
            if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN)
            {
                NRF_LOG_INFO("Connection Request timed out.");
            }
            break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
            // Accept parameters requested by the the peer.
            err_code = sd_ble_gap_conn_param_update(p_gap_evt->conn_handle,
                                        &p_gap_evt->params.conn_param_update_request.conn_params);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_AUTH_STATUS:
            NRF_LOG_INFO("BLE_GAP_EVT_AUTH_STATUS");
            if (p_ble_evt->evt.gap_evt.params.auth_status.auth_status == BLE_GAP_SEC_STATUS_SUCCESS)
            {
                NRF_LOG_INFO("Authorization succeeded!");
            }
            else
            {
                NRF_LOG_INFO("Authorization failed with code: %u!",
                             p_ble_evt->evt.gap_evt.params.auth_status.auth_status);
            }
            break;

        case BLE_GAP_EVT_CONN_SEC_UPDATE:
            NRF_LOG_INFO("BLE_GAP_EVT_CONN_SEC_UPDATE");
            NRF_LOG_INFO("Security mode: %u. Security level: %u",
                         p_ble_evt->evt.gap_evt.params.conn_sec_update.conn_sec.sec_mode.sm,
                         p_ble_evt->evt.gap_evt.params.conn_sec_update.conn_sec.sec_mode.lv);
            if (p_ble_evt->evt.gap_evt.params.conn_sec_update.conn_sec.sec_mode.lv >= SECURITY_LEVEL_THR)
            {
                NRF_LOG_INFO("Security level high enough to enable HRS notifications.");
            }
            else
            {
                NRF_LOG_INFO("Security level too low to enable HRS notifications.");
            }
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for handling the system events of the application.
 *
 * @param[in]   sys_evt   System event.
 */
static void soc_evt_handler(uint32_t sys_evt, void * p_context)
{
    switch (sys_evt)
    {
        case NRF_EVT_FLASH_OPERATION_SUCCESS:
            /* fall through */

        case NRF_EVT_FLASH_OPERATION_ERROR:
            if (m_memory_access_in_progress)
            {
                m_memory_access_in_progress = false;
                scan_start();
            }
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for handling Heart Rate Collector events.
 *
 * @param[in] p_hrs_c       Pointer to Heart Rate Client structure.
 * @param[in] p_hrs_c_evt   Pointer to event structure.
 */
static void hrs_c_evt_handler(ble_hrs_c_t * p_hrs_c, ble_hrs_c_evt_t * p_hrs_c_evt)
{
    ret_code_t err_code;

    switch (p_hrs_c_evt->evt_type)
    {
        case BLE_HRS_C_EVT_DISCOVERY_COMPLETE:
            err_code = ble_hrs_c_handles_assign(p_hrs_c ,
                                                p_hrs_c_evt->conn_handle,
                                                &p_hrs_c_evt->params.peer_db);
            APP_ERROR_CHECK(err_code);

            // Initiate bonding.
            err_code = pm_conn_secure(p_hrs_c_evt->conn_handle, false);
            if (err_code != NRF_ERROR_BUSY)
            {
                APP_ERROR_CHECK(err_code);
            }

            NRF_LOG_DEBUG("Heart Rate Service discovered ");
            break;

        case BLE_HRS_C_EVT_HRM_NOTIFICATION:
            NRF_LOG_INFO("Heart Rate = %d", p_hrs_c_evt->params.hrm.hr_value);
            m_hrs_notif_enabled = true;
            break;

        default:
            break;
    }
}


/**@brief Function for handling Battery Level Collector events.
 *
 * @param[in] p_bas_c       Pointer to Battery Service Client structure.
 * @param[in] p_bas_c_evt   Pointer to event structure.
 */
static void bas_c_evt_handler(ble_bas_c_t * p_bas_c, ble_bas_c_evt_t * p_bas_c_evt)
{
    ret_code_t err_code;

    switch (p_bas_c_evt->evt_type)
    {
        case BLE_BAS_C_EVT_DISCOVERY_COMPLETE:
            err_code = ble_bas_c_handles_assign(p_bas_c,
                                                p_bas_c_evt->conn_handle,
                                                &p_bas_c_evt->params.bas_db);
            APP_ERROR_CHECK(err_code);

            // Battery Service discovered. Enable notification of Battery Level.
            NRF_LOG_DEBUG("Battery Service discovered. Reading Battery Level.");

            err_code = ble_bas_c_bl_read(p_bas_c);
            APP_ERROR_CHECK(err_code);

            NRF_LOG_DEBUG("Enabling Battery Level Notification. ");
            err_code = ble_bas_c_bl_notif_enable(p_bas_c);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_BAS_C_EVT_BATT_NOTIFICATION:
            NRF_LOG_DEBUG("Battery Level received %d %%", p_bas_c_evt->params.battery_level);
            if (!m_hrs_notif_enabled)
            {
                // Enable notifications of Heart Rate Measurement.
                err_code = ble_hrs_c_hrm_notif_enable(&m_hrs_c);
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BLE_BAS_C_EVT_BATT_READ_RESP:
            NRF_LOG_INFO("Battery Level read as %d %%", p_bas_c_evt->params.battery_level);
            break;

        default:
            break;
    }
}


static void scan_evt_handler(scan_evt_t const * p_scan_evt)
{
    switch(p_scan_evt->scan_evt_id)
    {
        case NRF_BLE_SCAN_EVT_SCAN_TIMEOUT:
            NRF_LOG_DEBUG("Scan timed out.");
            scan_start();

            break;

        default:
            break;
    }

}


/**
 * @brief Function for initializing the Heart Rate Collector.
 */
static void hrs_c_init(void)
{
    ble_hrs_c_init_t hrs_c_init_obj;

    hrs_c_init_obj.evt_handler = hrs_c_evt_handler;

    ret_code_t err_code = ble_hrs_c_init(&m_hrs_c, &hrs_c_init_obj);
    APP_ERROR_CHECK(err_code);
}


/**
 * @brief Function for initializing the Battery Level Collector.
 */
static void bas_c_init(void)
{
    ble_bas_c_init_t bas_c_init_obj;

    bas_c_init_obj.evt_handler = bas_c_evt_handler;

    ret_code_t err_code = ble_bas_c_init(&m_bas_c, &bas_c_init_obj);
    APP_ERROR_CHECK(err_code);
}


/**
 * @brief Function for initializing the Database Discovery Collector.
 */
static void db_discovery_init(void)
{
    ret_code_t err_code = ble_db_discovery_init(db_disc_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing scanning.
 */
static void scan_init(void)
{
    ret_code_t err_code;

    err_code = nrf_ble_scan_init(&m_scan, NULL, scan_evt_handler);
    APP_ERROR_CHECK(err_code);
}


void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register handlers for BLE and SoC events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
    NRF_SDH_SOC_OBSERVER(m_soc_observer, APP_SOC_OBSERVER_PRIO, soc_evt_handler, NULL);

    gatt_init();
    db_discovery_init();
    hrs_c_init();
    bas_c_init();
    scan_init();
}
