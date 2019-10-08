/**
 * Copyright (c) 2017 - 2019, Nordic Semiconductor ASA
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
 * @brief Immediate Alert Application main file.
 *
 * This file contains source code for a sample application using the
 * Immediate Alert Service.
 *
 * This application accepts pairing requests from any peer device.
 */

#include <stdint.h>
#include <string.h>

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_ias.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "app_timer.h"
#include "bsp_btn_ble.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "nrf_fstorage.h"
#include "fds.h"
#include "ble_conn_state.h"
#include "nrf_ble_gatt.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_ble_scan.h"
#include "sensorsim.h"
#include "ble_bas.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


#define APP_BLE_OBSERVER_PRIO           3                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_SOC_OBSERVER_PRIO           1                                       /**< Applications' SoC observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                       /**< A tag identifying the SoftDevice BLE configuration. */

#define CENTRAL_SCANNING_LED            BSP_BOARD_LED_0                         /**< LED to indicate that scanning is active. */
#define CENTRAL_CONNECTED_LED           BSP_BOARD_LED_1                         /**< LED to indicate that the device has established at least one connection. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(500, UNIT_1_25_MS)        /**< Minimum acceptable connection interval (0.5 seconds).  */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(1000, UNIT_1_25_MS)       /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                   0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)         /**< Connection supervisory timeout (4 seconds). */

#define SCAN_DURATION_WITELIST          3000                                    /**< Duration of the scanning in units of 10 milliseconds. */

#define SEC_PARAM_BOND                  1                                       /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                       /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                       /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                       /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                    /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                       /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                       /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                      /**< Maximum encryption key size. */

#define TARGET_UUID                     BLE_UUID_IMMEDIATE_ALERT_SERVICE        /**< Target device name that the application is looking for. */

#define DEAD_BEEF                       0xDEADBEEF                              /**< Value used as error code on stack dump. Can be used to identify stack location on stack unwind. */

#define BATTERY_LEVEL_MEAS_INTERVAL     APP_TIMER_TICKS(10000)                  /**< Battery level measurement interval (ticks). */
#define BATTERY_NOTIFY_DELAY            APP_TIMER_TICKS(3500)                   /**< Battery notification delay time. */
#define MIN_BATTERY_LEVEL               50                                      /**< Minimum simulated battery level. */
#define MAX_BATTERY_LEVEL               100                                     /**< Maximum simulated 7battery level. */
#define BATTERY_LEVEL_INCREMENT         1                                       /**< Increment between each simulated battery level measurement. */
#define MAX_BAS_BONDED_CLIENT           NRF_SDH_BLE_TOTAL_LINK_COUNT            /**< The maximum number of clients for which battery data will be stored. */
#define INVALID_BATTERY_LEVEL           255

APP_TIMER_DEF(m_battery_timer_id);                        /**< Battery timer. */
APP_TIMER_DEF(m_battery_notify_timer_id);                 /**< Battery notification delay timer. */
NRF_BLE_GATT_DEF(m_gatt);                                 /**< GATT module instance. */
BLE_IAS_DEF(m_ias, NRF_SDH_BLE_TOTAL_LINK_COUNT);         /**< Structure used to identify the Immediate Alert Service. */
NRF_BLE_SCAN_DEF(m_scan);                                 /**< Scanning Module instance. */
BLE_BAS_DEF(m_bas);                                       /**< Structure used to identify the Battery Service. */

static bool                  m_whitelist_disabled;        /**< True if whitelist has been temporarily disabled. */
static sensorsim_cfg_t       m_battery_sim_cfg;           /**< Battery Level sensor simulator configuration. */
static sensorsim_state_t     m_battery_sim_state;         /**< Battery Level sensor simulator state. */
static ble_gap_scan_params_t m_scan_param =               /**< Scan parameters requested for scanning and connection. */
{
    .active        = 0x01,
    .interval      = NRF_BLE_SCAN_SCAN_INTERVAL,
    .window        = NRF_BLE_SCAN_SCAN_WINDOW,
    .filter_policy = BLE_GAP_SCAN_FP_WHITELIST,
    .timeout       = SCAN_DURATION_WITELIST,
    .scan_phys     = BLE_GAP_PHY_1MBPS,
};


static struct
{
    struct
    {
        uint16_t conn_handle;         /*< Connection handle. */
        uint16_t peer_id;             /*< Peer Id. Identifies bonded peer. */
        uint8_t  last_battery_lvl;    /*< Last battery value for bonded peer. Filled during disconnection. */
    } devices[MAX_BAS_BONDED_CLIENT];
    uint8_t counter;                  /*< Count of bonded clients. */
} m_bas_bonded_clients;


/**@brief Strings to display alert level information */
static char const * const m_alerts[] =
{
    "No Alert",
    "Mild Alert",
    "High Alert"
};

static void on_ias_evt(ble_ias_t * p_ias, ble_ias_evt_t * p_evt);
static void scan_start(void);


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of an assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for adding a bonded client.
 *
 * @param[in] p_evt Pointer to peer manager event structure.
 */
static void bonded_client_add(pm_evt_t const * p_evt)
{
    uint16_t   conn_handle = p_evt->conn_handle;
    uint16_t   peer_id     = p_evt->peer_id;
    uint8_t  * p_counter   = &m_bas_bonded_clients.counter;

    if (*p_counter == MAX_BAS_BONDED_CLIENT)
    {
        return;
    }

    for (uint8_t i = 0; i < *p_counter; i++)
    {
        // If the device is already on the list, update conn_handle.
        if (m_bas_bonded_clients.devices[i].peer_id == peer_id)
        {
            m_bas_bonded_clients.devices[i].conn_handle = conn_handle;
            return;
        }
    }

    m_bas_bonded_clients.devices[*p_counter].conn_handle      = conn_handle;
    m_bas_bonded_clients.devices[*p_counter].peer_id          = peer_id;
    m_bas_bonded_clients.devices[*p_counter].last_battery_lvl = INVALID_BATTERY_LEVEL;

    (*p_counter)++;
}


/**@brief Function for removing all bonded clients.
 */
inline static void bonded_client_remove_all(void)
{
    memset(&m_bas_bonded_clients, 0, sizeof(m_bas_bonded_clients));
}


/**@brief Function for triggering notification with battery level
 *        after bonded client reconnection.
 *
 * @param[in] p_evt Pointer to peer manager event structure.
 */
static void on_bonded_peer_reconnection_lvl_notify(pm_evt_t const * p_evt)
{
    ret_code_t        err_code;
    static uint16_t   peer_id   = PM_PEER_ID_INVALID;

    peer_id = p_evt->peer_id;

    // Notification must be sent later to allow the client to
    // perform a database discovery first
    err_code = app_timer_start(m_battery_notify_timer_id,
                                       BATTERY_NOTIFY_DELAY,
                                       &peer_id);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for saving last battery value for bonded client upon disconnection.
 *
 * @param[in] conn_handle Connection handle.
 */
static void on_disconnect_battery_lvl_store(uint16_t conn_handle)
{
    uint8_t counter = m_bas_bonded_clients.counter;

    for (uint8_t i = 0; i < counter; i++)
    {
        // Store the last battery value.
        if (m_bas_bonded_clients.devices[i].conn_handle == conn_handle)
        {
            m_bas_bonded_clients.devices[i].last_battery_lvl = m_bas.battery_level_last;

            return;
        }
    }
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
        case PM_EVT_BONDED_PEER_CONNECTED:
            bonded_client_add(p_evt);
            on_bonded_peer_reconnection_lvl_notify(p_evt);
            break;

        case PM_EVT_CONN_SEC_SUCCEEDED:
            bonded_client_add(p_evt);
            break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            bonded_client_remove_all();

            // Bonds are deleted. Start scanning.
            scan_start();
            break;

        default:
            break;
    }
}


/**@brief Function for performing battery measurement and updating the Battery Level characteristic
 *        in Battery Service.
 */
static void battery_level_update(void)
{
    ret_code_t err_code;
    uint8_t  battery_level;

    battery_level = (uint8_t)sensorsim_measure(&m_battery_sim_state, &m_battery_sim_cfg);

    err_code = ble_bas_battery_level_update(&m_bas, battery_level, BLE_CONN_HANDLE_ALL);
    if ((err_code != NRF_SUCCESS) &&
        (err_code != NRF_ERROR_INVALID_STATE) &&
        (err_code != NRF_ERROR_RESOURCES) &&
        (err_code != NRF_ERROR_BUSY) &&
        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
       )
    {
        APP_ERROR_HANDLER(err_code);
    }
}


/**@brief Function for handling Battery notification delay timer tomeout.
 *
 * @details This function send notification to bonded client after reconnection,
 *          if current value is different from the value during disconnection.
 */
static void bettery_notify_send_handler(void * p_context)
{
    ret_code_t     err_code  = NRF_SUCCESS;
    uint8_t      * p_counter = &m_bas_bonded_clients.counter;
    pm_peer_id_t * p_peer_id = (pm_peer_id_t *)p_context;

    for (uint8_t i = 0; i < *p_counter; i++)
    {
        if (m_bas_bonded_clients.devices[i].peer_id == *p_peer_id)
        {
            if (m_bas_bonded_clients.devices[i].last_battery_lvl != m_bas.battery_level_last)
            {
                err_code = ble_bas_battery_lvl_on_reconnection_update(&m_bas,
                                                                      m_bas_bonded_clients.devices[i].conn_handle);
            }
        }
    }

    // These errors can be ignored because the application may
    // try to send notification earlier.
    if (err_code != NRF_SUCCESS &&
        err_code != NRF_ERROR_INVALID_STATE &&
        err_code != NRF_ERROR_RESOURCES)
    {
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling the Battery measurement timer timeout.
 *
 * @details This function will be called each time the battery level measurement timer expires.
 *
 * @param[in] p_context  Pointer used for passing some arbitrary information (context) from the
 *                       app_start_timer() call to the timeout handler.
 */
static void battery_level_meas_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);
    battery_level_update();
}


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    ret_code_t err_code;

    // Initialize timer module.
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_battery_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                battery_level_meas_timeout_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_battery_notify_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                bettery_notify_send_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting application timers.
 */
static void timers_start(void)
{
    ret_code_t err_code;

    err_code = app_timer_start(m_battery_timer_id, BATTERY_LEVEL_MEAS_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    ret_code_t err_code;

    // Turn off LEDs
    bsp_board_led_off(CENTRAL_CONNECTED_LED);
    bsp_board_led_off(CENTRAL_SCANNING_LED);

    err_code = bsp_indication_set(BSP_INDICATE_ALERT_OFF);
    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Immediate Alert Service.
 */
static void ias_init(void)
{
    ret_code_t     err_code;
    ble_ias_init_t ias_init_obj;

    memset(&ias_init_obj, 0, sizeof(ias_init_obj));
    ias_init_obj.evt_handler  = on_ias_evt;

    ias_init_obj.alert_wr_sec = SEC_JUST_WORKS;

    err_code = ble_ias_init(&m_ias, &ias_init_obj);
    APP_ERROR_CHECK(err_code);
}


static void bas_init(void)
{
    ret_code_t     err_code;
    ble_bas_init_t bas_init_obj;

    memset(&bas_init_obj, 0, sizeof(bas_init_obj));

    bas_init_obj.support_notification = true;
    bas_init_obj.initial_batt_level   = 100;

    // Here the sec level for the Battery Service can be changed/increased.
    bas_init_obj.bl_rd_sec        = SEC_OPEN;
    bas_init_obj.bl_cccd_wr_sec   = SEC_OPEN;
    bas_init_obj.bl_report_rd_sec = SEC_OPEN;

    err_code = ble_bas_init(&m_bas, &bas_init_obj);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the services that will be used by the application.
 */
static void services_init(void)
{
    ias_init();
    bas_init();
}


/**@brief Function for initializing the sensor simulators.
 */
static void sensor_simulator_init(void)
{
    m_battery_sim_cfg.min          = MIN_BATTERY_LEVEL;
    m_battery_sim_cfg.max          = MAX_BATTERY_LEVEL;
    m_battery_sim_cfg.incr         = BATTERY_LEVEL_INCREMENT;
    m_battery_sim_cfg.start_at_max = true;

    sensorsim_init(&m_battery_sim_state, &m_battery_sim_cfg);
}


/**@brief Function for the Signals alert event from the Immediate Alert Service.
 *
 * @param[in] alert_level  Requested alert level.
 */
static void alert_signal(uint8_t alert_level)
{
    ret_code_t err_code;

    if (alert_level < ARRAY_SIZE(m_alerts))
    {
        NRF_LOG_INFO("%s.", m_alerts[alert_level]);
    }

    switch (alert_level)
    {
        case BLE_CHAR_ALERT_LEVEL_NO_ALERT:
            err_code = bsp_indication_set(BSP_INDICATE_ALERT_OFF);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_CHAR_ALERT_LEVEL_MILD_ALERT:
            err_code = bsp_indication_set(BSP_INDICATE_ALERT_0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_CHAR_ALERT_LEVEL_HIGH_ALERT:
            err_code = bsp_indication_set(BSP_INDICATE_ALERT_3);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for determining current alert level.
 *
 * @details This function will be called for @ref BLE_IAS_EVT_ALERT_LEVEL_UPDATED event which is
 *          passed to the application. It will determine the current alert level by finding
 *          the maximum one among connected peers.
 */
static uint8_t alert_level_resolve(void)
{
    ret_code_t                        err_code;
    uint8_t                           alert_level;
    uint8_t                           max_alert_level = BLE_CHAR_ALERT_LEVEL_NO_ALERT;
    ble_conn_state_conn_handle_list_t conn_handles    = ble_conn_state_conn_handles();

    // Iterate over all valid connection handles.
    for (uint32_t i = 0; i < conn_handles.len; i++)
    {
        if (ble_conn_state_status(conn_handles.conn_handles[i]) == BLE_CONN_STATUS_CONNECTED)
        {
            err_code = ble_ias_alert_level_get(&m_ias, conn_handles.conn_handles[i], &alert_level);
            APP_ERROR_CHECK(err_code);

            max_alert_level = (alert_level > max_alert_level) ? alert_level : max_alert_level;
        }
    }
    return max_alert_level;
}


/**@brief Function for handling Immediate Alert events.
 *
 * @details This function will be called for all Immediate Alert events which are passed to the
 *          application.
 *
 * @param[in] p_ias  Immediate Alert structure.
 * @param[in] p_evt  Event received from the Immediate Alert Service.
 */
static void on_ias_evt(ble_ias_t * p_ias, ble_ias_evt_t * p_evt)
{
    NRF_LOG_DEBUG("Received IAS event from conn_handle: 0x%04X.", p_evt->conn_handle);

    switch (p_evt->evt_type)
    {
        case BLE_IAS_EVT_ALERT_LEVEL_UPDATED:
        {
            uint8_t alert_level;

            if (p_evt->p_link_ctx != NULL)
            {
                NRF_LOG_INFO("New alert level: %s from conn_handle: 0x%04X.",
                             m_alerts[p_evt->p_link_ctx->alert_level],
                             p_evt->conn_handle);
            }
            else
            {
                NRF_LOG_ERROR("No alert context for conn_handle: 0x%04X", p_evt->conn_handle);
            }

            // Update common alert level.
            alert_level = alert_level_resolve();
            alert_signal(alert_level);
        } break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t err_code = NRF_SUCCESS;

    pm_handler_secure_on_connection(p_ble_evt);

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
        {
            NRF_LOG_INFO("Connected using conn_handle: 0x%04X.",
                         p_ble_evt->evt.gap_evt.conn_handle);

            // Handle LEDs.
            bsp_board_led_on(CENTRAL_CONNECTED_LED);
            bsp_board_led_off(CENTRAL_SCANNING_LED);
            scan_start();
        } break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected using conn_handle: 0x%04X.",
                         p_ble_evt->evt.gap_evt.conn_handle);

            on_disconnect_battery_lvl_store(p_ble_evt->evt.gap_evt.conn_handle);

            //Handle LEDs.
            if (ble_conn_state_central_conn_count() == 0)
            {
                bsp_board_led_off(CENTRAL_CONNECTED_LED);
            }

            if (p_ble_evt->evt.gap_evt.params.disconnected.reason ==
                BLE_HCI_LOCAL_HOST_TERMINATED_CONNECTION)
            {
                bsp_board_led_off(CENTRAL_SCANNING_LED);
                nrf_ble_scan_stop();
            }
            else
            {
                scan_start();
            }
            break;

        case BLE_GAP_EVT_TIMEOUT:
        {
            if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN)
            {
                NRF_LOG_INFO("Connection Request timed out.");
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

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
            // Accepting parameters requested by peer.
            err_code = sd_ble_gap_conn_param_update(p_ble_evt->evt.gap_evt.conn_handle,
                                                    &p_ble_evt->evt.gap_evt.params.conn_param_update_request.conn_params);
            APP_ERROR_CHECK(err_code);
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

    // Register a handler for BLE and SoC events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}


/**@brief Function for the Peer Manager initialization.
 */
static void peer_manager_init(void)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    err_code = pm_init();
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

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for disabling the use of whitelist and starting scanning.
 */
static void whitelist_disable_and_scan(void)
{
    if (!m_whitelist_disabled)
    {
        NRF_LOG_INFO("Whitelist temporarily disabled.");
        m_whitelist_disabled = true;
    }

    scan_start();
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated when button is pressed.
 */
static void bsp_event_handler(bsp_event_t event)
{
    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break;

        case BSP_EVENT_WHITELIST_OFF:
            whitelist_disable_and_scan();
            break;

        default:
            break;
    }
}


/**@brief Function for retrieving a list of peer manager peer IDs.
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


/**@brief Function for loading the whitelist.
 */
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

    // Set up the list of device identities.
    // Some SoftDevices do not support this feature.
    ret = pm_device_identities_list_set(peers, peer_cnt);
    if (ret != NRF_ERROR_NOT_SUPPORTED)
    {
        APP_ERROR_CHECK(ret);
    }
}


static void on_whitelist_req(void)
{
    ret_code_t     err_code;

    // Whitelist buffers.
    ble_gap_addr_t whitelist_addrs[8];
    ble_gap_irk_t  whitelist_irks[8];

    memset(whitelist_addrs, 0x00, sizeof(whitelist_addrs));
    memset(whitelist_irks,  0x00, sizeof(whitelist_irks));

    uint32_t addr_cnt = (sizeof(whitelist_addrs) / sizeof(ble_gap_addr_t));
    uint32_t irk_cnt  = (sizeof(whitelist_irks)  / sizeof(ble_gap_irk_t));

    // Reload the whitelist and whitelist all peers.
    whitelist_load();

    // Get the whitelist previously set using pm_whitelist_set().
    err_code = pm_whitelist_get(whitelist_addrs, &addr_cnt,
                                whitelist_irks,  &irk_cnt);
    APP_ERROR_CHECK(err_code);

    if (((addr_cnt == 0) && (irk_cnt == 0)) ||
        (m_whitelist_disabled))
    {
        // Don't use whitelist
        err_code = nrf_ble_scan_params_set(&m_scan, NULL);
        APP_ERROR_CHECK(err_code);
        NRF_LOG_INFO("Starting scan.");
    }
    else
    {
        // Use whitelist.
        NRF_LOG_INFO("Starting scan with whitelist.");
    }

    bsp_board_led_on(CENTRAL_SCANNING_LED);
}


/**@brief Function for handling events from the Scanning Module.
 *
 * @param[in]   event   Event generated.
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


/**@brief Function for starting the scanning.
 */
static void scan_start(void)
{
    ret_code_t err_code;

    bsp_board_led_off(CENTRAL_SCANNING_LED);

    // Check whether there is an available connection slot.
    if (ble_conn_state_central_conn_count() >= NRF_SDH_BLE_CENTRAL_LINK_COUNT)
    {
        NRF_LOG_DEBUG("Maximum number of connections: %d has been reached. Scanning cannot be restarted",
                      NRF_SDH_BLE_CENTRAL_LINK_COUNT);
        return;
    }

    err_code = nrf_ble_scan_params_set(&m_scan, &m_scan_param);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_scan_start(&m_scan);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing buttons and LEDs.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init(bool * p_erase_bonds)
{
    ret_code_t  err_code;
    bsp_event_t startup_event;

    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}


/**@brief Function for initializing the nrf_log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing scanning.
 */
static void scan_init(void)
{
    ret_code_t          err_code;
    nrf_ble_scan_init_t init_scan;
    ble_uuid_t uuid =
    {
        .uuid = TARGET_UUID,
        .type = BLE_UUID_TYPE_BLE,
    };

    memset(&init_scan, 0, sizeof(init_scan));

    init_scan.connect_if_match = true;
    init_scan.conn_cfg_tag     = APP_BLE_CONN_CFG_TAG;
    init_scan.p_scan_param     = &m_scan_param;

    err_code = nrf_ble_scan_init(&m_scan, &init_scan, scan_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_scan_filter_set(&m_scan, SCAN_UUID_FILTER, &uuid);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_scan_filters_enable(&m_scan,
                                           NRF_BLE_SCAN_UUID_FILTER,
                                           false);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the idle state (main loop).
 *
 * @details Handle any pending log operation(s), then sleep until the next event occurs.
 */
static void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}


/**@brief Clear bond information from persistent storage.
 */
static void delete_bonds(void)
{
    ret_code_t err_code;

    NRF_LOG_INFO("Erase bonds!");

    err_code = pm_peers_delete();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting a scan, or instead trigger it from peer manager (after
 *        deleting bonds).
 *
 * @param[in] p_erase_bonds Pointer to a bool to determine if bonds will be deleted before scanning.
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


/**@brief Function for application main entry.
 */
int main(void)
{
    bool erase_bonds;

    // Initialize.
    log_init();
    timers_init();
    buttons_leds_init(&erase_bonds);
    power_management_init();
    ble_stack_init();
    gatt_init();
    peer_manager_init();
    timers_start();
    services_init();
    scan_init();
    sensor_simulator_init();

    // Start execution.
    NRF_LOG_INFO("Immediate Alert example started.");
    scanning_start(&erase_bonds);

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }
}


/**
 * @}
 */
