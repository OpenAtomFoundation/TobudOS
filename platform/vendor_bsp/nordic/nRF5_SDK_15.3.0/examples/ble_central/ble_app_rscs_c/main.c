/**
 * Copyright (c) 2014 - 2019, Nordic Semiconductor ASA
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
/**
 * @brief BLE Running Speed and Cadence Collector application main file.
 *
 * This file contains the source code for a sample Running Speed and Cadence collector application.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf_sdm.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_db_discovery.h"
#include "ble_srv_common.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "nrf_pwr_mgmt.h"
#include "app_util.h"
#include "app_error.h"
#include "ble_dis_c.h"
#include "ble_rscs_c.h"
#include "app_util.h"
#include "app_timer.h"
#include "bsp_btn_ble.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "fds.h"
#include "nrf_fstorage.h"
#include "ble_conn_state.h"
#include "nrf_ble_gatt.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_ble_scan.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


#define APP_BLE_CONN_CFG_TAG        1                                   /**< Tag that identifies the BLE configuration of the SoftDevice. */
#define APP_BLE_OBSERVER_PRIO       3                                   /**< BLE observer priority of the application. There is no need to modify this value. */
#define APP_SOC_OBSERVER_PRIO       1                                   /**< SoC observer priority of the application. There is no need to modify this value. */

#define SEC_PARAM_BOND              1                                   /**< Perform bonding. */
#define SEC_PARAM_MITM              0                                   /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC              0                                   /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS          0                                   /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES   BLE_GAP_IO_CAPS_NONE                /**< No I/O capabilities. */
#define SEC_PARAM_OOB               0                                   /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE      7                                   /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE      16                                  /**< Maximum encryption key size. */

#define SCAN_INTERVAL               0x00A0                              /**< Determines scan interval in units of 0.625 millisecond. */
#define SCAN_WINDOW                 0x0050                              /**< Determines scan window in units of 0.625 millisecond. */

#define SCAN_DURATION           0x0000                              /**< Duration of the scanning in units of 10 milliseconds. If set to 0x0000, scanning continues until it is explicitly disabled. */
#define SCAN_DURATION_WITELIST  3000                                /**< Duration of the scanning in units of 10 milliseconds. */

#define MIN_CONNECTION_INTERVAL     MSEC_TO_UNITS(7.5, UNIT_1_25_MS)    /**< Determines minimum connection interval in milliseconds. */
#define MAX_CONNECTION_INTERVAL     MSEC_TO_UNITS(30, UNIT_1_25_MS)     /**< Determines maximum connection interval in milliseconds. */
#define SLAVE_LATENCY               0                                   /**< Determines slave latency in counts of connection events. */
#define SUPERVISION_TIMEOUT         MSEC_TO_UNITS(4000, UNIT_10_MS)     /**< Determines supervision timeout in units of 10 millisecond. */

#define TARGET_UUID                 BLE_UUID_RUNNING_SPEED_AND_CADENCE  /**< Target UUID that the application looks for. */

/**@brief Variable length data encapsulation in terms of length and pointer to data. */
typedef struct
{
    uint8_t * p_data;   /**< Pointer to data. */
    uint16_t  data_len; /**< Length of data. */
} data_t;


/**@brief Strings to display data retrieved by Device Information Service client module. */
static char const * const m_dis_char_names[] =
{
    "Manufacturer Name String",
    "Model Number String     ",
    "Serial Number String    ",
    "Hardware Revision String",
    "Firmware Revision String",
    "Software Revision String",
    "System ID",
    "IEEE 11073-20601 Regulatory Certification Data List",
    "PnP ID"
};


BLE_DIS_C_DEF(m_ble_dis_c);                                 /**< Device Information Service client instance. */
BLE_RSCS_C_DEF(m_rscs_c);                                   /**< Running Speed and Cadence Service client instance. */
NRF_BLE_GATT_DEF(m_gatt);                                   /**< GATT module instance. */
BLE_DB_DISCOVERY_DEF(m_db_disc);                            /**< Database discovery module instance. */
NRF_BLE_SCAN_DEF(m_scan);                                   /**< Scanning Module instance. */

static uint16_t              m_conn_handle;                 /**< Current connection handle. */
static bool                  m_whitelist_disabled;          /**< True if the whitelist is temporarily disabled. */
static bool                  m_memory_access_in_progress;   /**< Flag to keep track of ongoing operations on persistent memory. */
static bool                  m_rscs_log_enabled;            /**< Flag to enable logs related with RSCS measurements. */

static ble_gap_scan_params_t m_scan_param =                 /**< Scan parameters requested for scanning and connection. */
{
    .active        = 0x01,
    .interval      = NRF_BLE_SCAN_SCAN_INTERVAL,
    .window        = NRF_BLE_SCAN_SCAN_WINDOW,
    .filter_policy = BLE_GAP_SCAN_FP_WHITELIST,
    .timeout       = SCAN_DURATION_WITELIST,
    .scan_phys     = BLE_GAP_PHY_1MBPS,
    .extended      = 1,
};

static void scan_start(void);


/**@brief Function for handling asserts in the SoftDevice.
 *
 * @details This function is called in case of an assert in the SoftDevice.
 *
 * @warning This handler is only an example and is not meant for the final product. You need to analyze
 *          how your product is supposed to react in case of an assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num     Line number of the failing assert call.
 * @param[in] p_file_name  File name of the failing assert call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}


/**@brief Function for handling database discovery events.
 *
 * @details This function is a callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function forwards the events
 *          to their respective services.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 *
 */
static void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{
    ble_rscs_on_db_disc_evt(&m_rscs_c, p_evt);
    ble_dis_c_on_db_disc_evt(&m_ble_dis_c, p_evt);
}


/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    pm_handler_on_pm_evt(p_evt);
    pm_handler_disconnect_on_sec_failure(p_evt);
    pm_handler_flash_clean(p_evt);

    switch (p_evt->evt_id)
    {
        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            scan_start();
            break;

        default:
            break;
    }
}


/**
 * @brief Function for handling shutdown events.
 *
 * @param[in]   event       Shutdown type.
 */
static bool shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    ret_code_t err_code;

    err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);

    switch (event)
    {
        case NRF_PWR_MGMT_EVT_PREPARE_WAKEUP:
            // Prepare wakeup buttons.
            err_code = bsp_btn_ble_sleep_mode_prepare();
            APP_ERROR_CHECK(err_code);
            break;

        default:
            break;
    }

    return true;
}

NRF_PWR_MGMT_HANDLER_REGISTER(shutdown_handler, APP_SHUTDOWN_HANDLER_PRIORITY);

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t            err_code;
    ble_gap_evt_t const * p_gap_evt = &p_ble_evt->evt.gap_evt;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
        {
            // Discover the peer's services.
            err_code = ble_db_discovery_start(&m_db_disc, p_ble_evt->evt.gap_evt.conn_handle);
            APP_ERROR_CHECK(err_code);

            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);

            if (ble_conn_state_central_conn_count() < NRF_SDH_BLE_CENTRAL_LINK_COUNT)
            {
                scan_start();
            }
        } break;

        case BLE_GAP_EVT_TIMEOUT:
        {
            if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN)
            {
                NRF_LOG_DEBUG("Connection Request timed out.");
            }
        } break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
        {
            // Accepting parameters requested by peer.
            err_code = sd_ble_gap_conn_param_update(p_gap_evt->conn_handle,
                                                    &p_gap_evt->params.conn_param_update_request.conn_params);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_DISCONNECTED:
        {
            NRF_LOG_INFO("Disconnected. conn_handle: 0x%x, reason: 0x%x",
                         p_gap_evt->conn_handle,
                         p_gap_evt->params.disconnected.reason);

            if (ble_conn_state_central_conn_count() < NRF_SDH_BLE_CENTRAL_LINK_COUNT)
            {
                scan_start();
            }
        } break;

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
        {
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTS_EVT_TIMEOUT:
        {
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
        } break;

        default:
            break;
    }
}


/**
 * @brief SoftDevice SoC event handler.
 *
 * @param[in] evt_id    SoC event.
 * @param[in] p_context Context.
 */
static void soc_evt_handler(uint32_t evt_id, void * p_context)
{
    switch (evt_id)
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


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
  */
static void ble_stack_init(void)
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
}

/**@brief Function for disabling the use of the whitelist for scanning.
 */
static void whitelist_disable(void)
{
    if (!m_whitelist_disabled)
    {
        NRF_LOG_INFO("Whitelist temporarily disabled.");
        m_whitelist_disabled = true;
        nrf_ble_scan_stop();
        scan_start();
    }
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
void bsp_event_handler(bsp_event_t event)
{
    ret_code_t err_code;

    switch (event)
    {
        case BSP_EVENT_SLEEP:
            nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
            break;

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BSP_EVENT_WHITELIST_OFF:
            whitelist_disable();
            break;

        default:
            break;
    }
}


/**@brief Running Speed and Cadence collector handler.
 */
static void rscs_c_evt_handler(ble_rscs_c_t * p_rsc_c, ble_rscs_c_evt_t * p_rsc_c_evt)
{
    ret_code_t err_code;

    switch (p_rsc_c_evt->evt_type)
    {
        case BLE_RSCS_C_EVT_DISCOVERY_COMPLETE:
            // Initiate bonding.
            err_code = ble_rscs_c_handles_assign(&m_rscs_c,
                                                 p_rsc_c_evt->conn_handle,
                                                 &p_rsc_c_evt->params.rscs_db);
            APP_ERROR_CHECK(err_code);

            err_code = pm_conn_secure(p_rsc_c_evt->conn_handle, false);
            if (err_code != NRF_ERROR_BUSY)
            {
                APP_ERROR_CHECK(err_code);
            }

            // Running Speed and Cadence Service discovered. Enable Running Speed and Cadence notifications.
            err_code = ble_rscs_c_rsc_notif_enable(p_rsc_c);
            APP_ERROR_CHECK(err_code);

            NRF_LOG_INFO("Running Speed and Cadence Service discovered.");
            break;

        case BLE_RSCS_C_EVT_RSC_NOTIFICATION:
        {
            if (m_rscs_log_enabled)
            {
                NRF_LOG_INFO("");
                NRF_LOG_DEBUG("RSC Measurement received %d ",
                         p_rsc_c_evt->params.rsc.inst_speed);

                NRF_LOG_INFO("Instantanious Speed   = %d", p_rsc_c_evt->params.rsc.inst_speed);
                if (p_rsc_c_evt->params.rsc.is_inst_stride_len_present)
                {
                    NRF_LOG_INFO("Stride Length         = %d",
                           p_rsc_c_evt->params.rsc.inst_stride_length);
                }
                if (p_rsc_c_evt->params.rsc.is_total_distance_present)
                {
                    NRF_LOG_INFO("Total Distance = %u",
                           (unsigned int)p_rsc_c_evt->params.rsc.total_distance);
                }
                NRF_LOG_INFO("Instantanious Cadence = %d", p_rsc_c_evt->params.rsc.inst_cadence);
                NRF_LOG_INFO("Flags");
                NRF_LOG_INFO("  Stride Length Present = %d",
                       p_rsc_c_evt->params.rsc.is_inst_stride_len_present);
                NRF_LOG_INFO("  Total Distance Present= %d",
                       p_rsc_c_evt->params.rsc.is_total_distance_present);
                NRF_LOG_INFO("  Is Running            = %d", p_rsc_c_evt->params.rsc.is_running);
            }
            break;
        }

        default:
            break;
    }
}


/**@brief Function for reading all characteristics that can be present in DIS.
 *
 * @return    The number of discovered characteristics.
 */
static uint32_t ble_dis_c_all_chars_read(void)
{
    ret_code_t err_code;
    uint32_t   disc_char_num = 0;

    for (ble_dis_c_char_type_t char_type = (ble_dis_c_char_type_t) 0;
         char_type < BLE_DIS_C_CHAR_TYPES_NUM;
         char_type++)
    {
        err_code = ble_dis_c_read(&m_ble_dis_c, char_type);

        // The NRF_ERROR_INVALID_STATE error code means that the characteristic is not present in DIS.
        if (err_code != NRF_ERROR_INVALID_STATE)
        {
            APP_ERROR_CHECK(err_code);
            disc_char_num++;
        }
    }

    return disc_char_num;
}


/**@brief Function for logging string characteristics that can be present in Device Information Service.
 *
 * @param[in] char_type Type of string characteristic.
 * @param[in] p_string  Response data of the characteristic that has been read.
 */
static void ble_dis_c_string_char_log(ble_dis_c_char_type_t            char_type,
                                      ble_dis_c_string_t const * const p_string)
{
    char response_data_string[BLE_DIS_C_STRING_MAX_LEN] = {0};

    if (sizeof(response_data_string) > p_string->len)
    {
        memcpy(response_data_string, p_string->p_data, p_string->len);
        NRF_LOG_INFO("%s: %s",
                     m_dis_char_names[char_type],
                     nrf_log_push((char *) response_data_string));
    }
    else
    {
        NRF_LOG_ERROR("String buffer for DIS characteristics is too short.")
    }
}


/**@brief Function for logging System ID characteristic data that can be present in Device Information Service.
 *
 * @param[in] p_sys_id Pointer to structure that describes the content of the System ID characteristic.
 */
static void ble_dis_c_system_id_log(ble_dis_sys_id_t const * const p_sys_id)
{
    NRF_LOG_INFO("%s:", m_dis_char_names[BLE_DIS_C_SYS_ID]);
    NRF_LOG_INFO(" Manufacturer Identifier:            0x%010X", p_sys_id->manufacturer_id);
    NRF_LOG_INFO(" Organizationally Unique Identifier: 0x%06X", p_sys_id->organizationally_unique_id);
}


/**@brief Function for logging IEEE 11073-20601 Regulatory Certification Data List characteristic
 *        data that can be present in DIS.
 *
 * @param[in] p_cert_list  Pointer to structure that describes the content of Certification Data List
 *                         characteristic.
 */
static void ble_dis_c_cert_list_log(ble_dis_reg_cert_data_list_t const * const p_cert_list)
{
    NRF_LOG_INFO("%s:", m_dis_char_names[BLE_DIS_C_CERT_LIST]);
    NRF_LOG_HEXDUMP_INFO(p_cert_list->p_list, p_cert_list->list_len);
}


/**@brief Function for logging PnP ID characteristic data that can be present in Device Information Service.
 *
 * @param[in] p_pnp_id Pointer to structure that describes the content of PnP ID characteristic.
 */
static void ble_dis_c_pnp_id_log(ble_dis_pnp_id_t const * const p_pnp_id)
{
    NRF_LOG_INFO("%s:", m_dis_char_names[BLE_DIS_C_PNP_ID]);
    NRF_LOG_INFO(" Vendor ID Source: 0x%02X", p_pnp_id->vendor_id_source);
    NRF_LOG_INFO(" Vendor ID:        0x%04X", p_pnp_id->vendor_id);
    NRF_LOG_INFO(" Product ID:       0x%04X", p_pnp_id->product_id);
    NRF_LOG_INFO(" Product Version:  0x%04X", p_pnp_id->product_version);
}


/**@brief Device Information Service client handler.
 */
static void ble_dis_c_evt_handler(ble_dis_c_t * p_ble_dis_c, ble_dis_c_evt_t const * p_ble_dis_evt)
{
    ret_code_t      err_code;
    static uint32_t disc_chars_num     = 0;
    static uint32_t disc_chars_handled = 0;

    switch (p_ble_dis_evt->evt_type)
    {
        case BLE_DIS_C_EVT_DISCOVERY_COMPLETE:
            err_code = ble_dis_c_handles_assign(p_ble_dis_c,
                                                p_ble_dis_evt->conn_handle,
                                                p_ble_dis_evt->params.disc_complete.handles);
            APP_ERROR_CHECK(err_code);

            disc_chars_num     = ble_dis_c_all_chars_read();
            disc_chars_handled = 0;
            NRF_LOG_INFO("Device Information Service discovered.");
            break;

        case BLE_DIS_C_EVT_DIS_C_READ_RSP:
        {
            ble_dis_c_evt_read_rsp_t const * p_read_rsp = &p_ble_dis_evt->params.read_rsp;

            //Print header log.
            if ((disc_chars_handled == 0) && (disc_chars_num != 0))
            {
                NRF_LOG_INFO("");
                NRF_LOG_INFO("Device Information:");
            }

            switch (p_read_rsp->char_type)
            {
                case BLE_DIS_C_MANUF_NAME:
                case BLE_DIS_C_MODEL_NUM:
                case BLE_DIS_C_SERIAL_NUM:
                case BLE_DIS_C_HW_REV:
                case BLE_DIS_C_FW_REV:
                case BLE_DIS_C_SW_REV:
                    ble_dis_c_string_char_log(p_read_rsp->char_type, &p_read_rsp->content.string);
                    break;

                case BLE_DIS_C_SYS_ID:
                    ble_dis_c_system_id_log(&p_read_rsp->content.sys_id);
                    break;

                case BLE_DIS_C_CERT_LIST:
                    ble_dis_c_cert_list_log(&p_read_rsp->content.cert_list);
                    break;

                case BLE_DIS_C_PNP_ID:
                    ble_dis_c_pnp_id_log(&p_read_rsp->content.pnp_id);
                    break;

                default:
                    break;
            }

            disc_chars_handled++;
            if(disc_chars_handled == disc_chars_num)
            {
                NRF_LOG_INFO("");
                disc_chars_handled = 0;
                disc_chars_num     = 0;
                m_rscs_log_enabled = true;
            }
         }
         break;

        case BLE_DIS_C_EVT_DIS_C_READ_RSP_ERROR:
            NRF_LOG_ERROR("Read request for: %s characteristic failed with gatt_status: 0x%04X.",
                          m_dis_char_names[p_ble_dis_evt->params.read_rsp.char_type],
                          p_ble_dis_evt->params.read_rsp_err.gatt_status);
            break;

        case BLE_DIS_C_EVT_DISCONNECTED:
            break;
    }
}


/**
 * @brief Running Speed and Cadence collector initialization.
 */
static void rscs_c_init(void)
{
    ble_rscs_c_init_t rscs_c_init_obj;

    rscs_c_init_obj.evt_handler = rscs_c_evt_handler;

    ret_code_t err_code = ble_rscs_c_init(&m_rscs_c, &rscs_c_init_obj);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Device Information Service client. */
static void dis_c_init(void)
{
    ret_code_t       err_code;
    ble_dis_c_init_t init;

    memset(&init, 0, sizeof(ble_dis_c_init_t));
    init.evt_handler = ble_dis_c_evt_handler;

    err_code = ble_dis_c_init(&m_ble_dis_c, &init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Peer Manager.
 *
 * @param[in] erase_bonds  Indicates whether the bonding information must be cleared from
 *                         persistent storage during the initialization of the Peer Manager.
 */
static void peer_manager_init()
{
    ret_code_t           err_code;
    ble_gap_sec_params_t sec_param;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));
    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);
}


/**@brief Clear bonding information from the persistent storage.
 */
static void delete_bonds(void){
    ret_code_t err_code;

    NRF_LOG_INFO("Erase bonds");
    err_code = pm_peers_delete();
    APP_ERROR_CHECK(err_code);
}



/**
 * @brief Database discovery collector initialization.
 */
static void db_discovery_init(void)
{
    ret_code_t err_code = ble_db_discovery_init(db_disc_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Retrive a list of peer manager peer IDs.
 *
 * @param[inout] p_peers   The buffer where to store the list of peer IDs.
 * @param[inout] p_size    In: The size of the @p p_peers buffer.
 *                         Out: The number of peers copied in the buffer.
 */
static void peer_list_get(pm_peer_id_t * p_peers, uint32_t * p_size)
{
    pm_peer_id_t peer_id;
    uint32_t     peers_to_copy;

    peers_to_copy = (*p_size < BLE_GAP_WHITELIST_ADDR_MAX_COUNT) ?
                     *p_size : BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

    peer_id = pm_next_peer_id_get(PM_PEER_ID_INVALID);
    *p_size = 0;

    while ((peer_id != PM_PEER_ID_INVALID) && (peers_to_copy--))
    {
        p_peers[(*p_size)++] = peer_id;
        peer_id = pm_next_peer_id_get(peer_id);
    }
}


static void whitelist_load()
{
    ret_code_t   ret;
    pm_peer_id_t peers[8];
    uint32_t     peer_cnt;

    memset(peers, PM_PEER_ID_INVALID, sizeof(peers));
    peer_cnt = (sizeof(peers) / sizeof(pm_peer_id_t));

    // Load all peers from the flash and whitelist them.
    peer_list_get(peers, &peer_cnt);

    ret = pm_whitelist_set(peers, peer_cnt);
    APP_ERROR_CHECK(ret);

    // Setup the list of device identities.
    // Some SoftDevices do not support this feature.
    ret = pm_device_identities_list_set(peers, peer_cnt);
    if (ret != NRF_ERROR_NOT_SUPPORTED)
    {
        APP_ERROR_CHECK(ret);
    }
}


static void on_whitelist_req(void)
{
    // Whitelist buffers.
    ble_gap_addr_t whitelist_addrs[8];
    ble_gap_irk_t  whitelist_irks[8];

    memset(whitelist_addrs, 0x00, sizeof(whitelist_addrs));
    memset(whitelist_irks,  0x00, sizeof(whitelist_irks));

    uint32_t addr_cnt = (sizeof(whitelist_addrs) / sizeof(ble_gap_addr_t));
    uint32_t irk_cnt  = (sizeof(whitelist_irks)  / sizeof(ble_gap_irk_t));

    // Reload the whitelist and whitelist all peers.
    whitelist_load();

    ret_code_t ret;

    // Get the whitelist previously set with pm_whitelist_set().
    ret = pm_whitelist_get(whitelist_addrs, &addr_cnt,
                           whitelist_irks,  &irk_cnt);

    if (((addr_cnt == 0) && (irk_cnt == 0)) ||
        (m_whitelist_disabled))
    {
        m_scan_param.filter_policy = BLE_GAP_SCAN_FP_ACCEPT_ALL;

        ret = nrf_ble_scan_params_set(&m_scan, &m_scan_param);
        APP_ERROR_CHECK(ret);
    }
}


/**@brief Function for handling Scanning Module events.
 */
static void scan_evt_handler(scan_evt_t const * p_scan_evt)
{
    ret_code_t err_code;
    switch(p_scan_evt->scan_evt_id)
    {
        case NRF_BLE_SCAN_EVT_WHITELIST_REQUEST:
        {
            on_whitelist_req();
            m_whitelist_disabled = false;
        } break;

        case NRF_BLE_SCAN_EVT_CONNECTING_ERROR:
        {
            err_code = p_scan_evt->params.connecting_err.err_code;
            APP_ERROR_CHECK(err_code);
        } break;

        case NRF_BLE_SCAN_EVT_SCAN_TIMEOUT:
        {
            NRF_LOG_INFO("Scan timed out.");
            scan_start();
        } break;

        case NRF_BLE_SCAN_EVT_FILTER_MATCH:
            break;
        case NRF_BLE_SCAN_EVT_WHITELIST_ADV_REPORT:
            break;

        default:
          break;
    }
}


/**@brief Function for initializing the scanning and setting the filters.
 */
static void scan_init(void)
{
    ret_code_t          err_code;
    nrf_ble_scan_init_t init_scan;

    ble_uuid_t uuid =
    {
        .type = BLE_UUID_TYPE_BLE,
        .uuid = TARGET_UUID,
    };

    memset(&init_scan, 0, sizeof(init_scan));

    init_scan.connect_if_match = true;
    init_scan.conn_cfg_tag     = APP_BLE_CONN_CFG_TAG;
    init_scan.p_scan_param     = &m_scan_param;

    err_code = nrf_ble_scan_init(&m_scan, &init_scan, scan_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_scan_filter_set(&m_scan, SCAN_UUID_FILTER, &uuid);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_scan_filters_enable(&m_scan, NRF_BLE_SCAN_UUID_FILTER, false);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting scanning.
 */
static void scan_start(void)
{
    ret_code_t err_code;

    // If there is any pending write to flash, defer scanning until it completes.
    if (nrf_fstorage_is_busy(NULL))
    {
        m_memory_access_in_progress = true;
        return;
    }

    NRF_LOG_INFO("Starting scan.");

    m_scan_param.filter_policy = BLE_GAP_SCAN_FP_WHITELIST;

    err_code = nrf_ble_scan_params_set(&m_scan, &m_scan_param);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_scan_start(&m_scan);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_indication_set(BSP_INDICATE_SCANNING);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing buttons and LEDs.
 *
 * @param[out] p_erase_bonds  True if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init(bool * p_erase_bonds)
{
    ret_code_t err_code;
    bsp_event_t startup_event;

    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}


/**@brief Function for initializing logging. */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for initializing the timer. */
static void timer_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the GATT module. */
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the idle state (main loop).
 *
 * @details Handles any pending log operations, then sleeps until the next event occurs.
 */
static void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}


/**@brief Function for starting a scan, or instead triggering it from the peer manager (after
 *        deleting bonds).
 *
 * @param[in] p_erase_bonds Pointer to a bool to determine whether bonds are to be deleted before scanning.
 */
void scanning_start(bool * p_erase_bonds)
{
    // Start scanning for peripherals and initiate connection
    // with devices that advertise GATT Service UUID.
    if (*p_erase_bonds == true)
    {
        // Scan is started by the PM_EVT_PEERS_DELETE_SUCCEEDED event.
        delete_bonds();
    }
    else
    {
        scan_start();
    }
}


int main(void)
{
    bool erase_bonds;

    // Initialize.
    log_init();
    timer_init();
    buttons_leds_init(&erase_bonds);
    power_management_init();
    ble_stack_init();
    gatt_init();
    peer_manager_init();
    db_discovery_init();
    rscs_c_init();
    dis_c_init();
    scan_init();

    whitelist_load();

    // Start execution.
    NRF_LOG_INFO("Running Speed Collector example started.");
    scanning_start(&erase_bonds);


    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }
}
