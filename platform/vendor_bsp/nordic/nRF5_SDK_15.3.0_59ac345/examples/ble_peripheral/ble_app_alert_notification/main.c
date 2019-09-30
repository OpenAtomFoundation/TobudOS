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
/** @file
 *
 * @defgroup ble_sdk_alert_notification_main main.c
 * @{
 * @ingroup ble_sdk_app_alert_notification
 * @brief Alert Notification Client Sample Application main file.
 *
 * This file contains the source code for a sample application using the Alert Notification Profile
 * Client. This application uses the @ref srvlib_conn_params module.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble_hci.h"
#include "ble_gap.h"
#include "ble_err.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "ble_srv_common.h"
#include "ble_conn_params.h"
#include "ble_db_discovery.h"
#include "ble_ans_c.h"
#include "app_timer.h"
#include "bsp_btn_ble.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "fds.h"
#include "ble_conn_state.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


#define DEVICE_NAME                     "Nordic_Alert_Notif."                       /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "NordicSemiconductor"                       /**< Manufacturer. Will be passed to Device Information Service. */

#define APP_BLE_OBSERVER_PRIO           3                                           /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                           /**< A tag identifying the SoftDevice BLE configuration. */

#define APP_ADV_FAST_INTERVAL           40                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
#define APP_ADV_SLOW_INTERVAL           3200                                        /**< Slow advertising interval (in units of 0.625 ms. This value corresponds to 2 seconds). */

#define APP_ADV_FAST_DURATION           3000                                        /**< The advertising duration of fast advertising in units of 10 milliseconds. */
#define APP_ADV_SLOW_DURATION           18000                                       /**< The advertising duration of slow advertising in units of 10 milliseconds. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(500, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(1000, UNIT_1_25_MS)           /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */

#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define MESSAGE_BUFFER_SIZE             18                                          /**< Size of buffer holding optional messages in notifications. */
#define BLE_ANS_NB_OF_CATEGORY_ID       10                                          /**< Number of categories. */

#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                           /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */


typedef enum
{
    ALERT_NOTIFICATION_DISABLED, /**< Alert Notifications has been disabled. */
    ALERT_NOTIFICATION_ENABLED,  /**< Alert Notifications has been enabled. */
    ALERT_NOTIFICATION_ON,       /**< Alert State is on. */
} ble_ans_c_alert_state_t;


BLE_ANS_C_DEF(m_ans_c);                                                             /**< Structure used to identify the Alert Notification Service Client. */
NRF_BLE_GATT_DEF(m_gatt);                                                           /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                             /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);                                                 /**< Advertising module instance. */
BLE_DB_DISCOVERY_DEF(m_ble_db_discovery);                                           /**< DB discovery module instance. */

static uint16_t m_cur_conn_handle = BLE_CONN_HANDLE_INVALID;                        /**< Handle of the current connection. */

static uint8_t m_alert_message_buffer[MESSAGE_BUFFER_SIZE];                         /**< Message buffer for optional notify messages. */
static ble_ans_c_alert_state_t m_new_alert_state    = ALERT_NOTIFICATION_DISABLED;  /**< State that holds the current state of New Alert Notifications, i.e. Enabled, Alert On, Disabled. */
static ble_ans_c_alert_state_t m_unread_alert_state = ALERT_NOTIFICATION_DISABLED;  /**< State that holds the current state of Unread Alert Notifications, i.e. Enabled, Alert On, Disabled. */

/**@brief String literals for the iOS notification categories. used then printing to UART. */
static char const * lit_catid[BLE_ANS_NB_OF_CATEGORY_ID] =
{
    "Simple alert",
    "Email",
    "News",
    "Incoming call",
    "Missed call",
    "SMS/MMS",
    "Voice mail",
    "Schedule",
    "High prioritized alert",
    "Instant message"
};


static void advertising_start(bool erase_bonds);

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    pm_handler_on_pm_evt(p_evt);
    pm_handler_flash_clean(p_evt);

    switch (p_evt->evt_id)
    {
        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            advertising_start(false);
            break;

        default:
            break;
    }
}


/**@brief Function for setup of alert notifications in central.
 *
 * @details This function will be called when a successful connection has been established.
 */
static void alert_notification_setup(void)
{
    ret_code_t err_code;

    err_code = ble_ans_c_enable_notif_new_alert(&m_ans_c);
    APP_ERROR_CHECK(err_code);

    m_new_alert_state = ALERT_NOTIFICATION_ENABLED;
    NRF_LOG_INFO("New Alert State: Enabled.");

    err_code = ble_ans_c_enable_notif_unread_alert(&m_ans_c);
    APP_ERROR_CHECK(err_code);

    m_unread_alert_state = ALERT_NOTIFICATION_ENABLED;
    NRF_LOG_INFO("Unread Alert State: Enabled.");

    NRF_LOG_DEBUG("Notifications enabled.");
}


/**@brief Function for setup of alert notifications in central.
 *
 * @details This function will be called when supported alert notification and
 *          supported unread alert notifications has been fetched.
 *
 * @param[in] p_evt  Event containing the response with supported alert types.
 */
static void control_point_setup(ble_ans_c_evt_t * p_evt)
{
    uint32_t                err_code;
    ble_ans_control_point_t setting;

    if (p_evt->uuid.uuid == BLE_UUID_SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR)
    {
        setting.command  = ANS_ENABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION;
        setting.category = (ble_ans_category_id_t)p_evt->data.alert.alert_category;
        NRF_LOG_DEBUG("Unread status notification enabled for received categories.");
    }
    else if (p_evt->uuid.uuid == BLE_UUID_SUPPORTED_NEW_ALERT_CATEGORY_CHAR)
    {
        setting.command  = ANS_ENABLE_NEW_INCOMING_ALERT_NOTIFICATION;
        setting.category = (ble_ans_category_id_t)p_evt->data.alert.alert_category;
        NRF_LOG_DEBUG("New incoming notification enabled for received categories.");
    }
    else
    {
        return;
    }

    err_code = ble_ans_c_control_point_write(&m_ans_c, &setting);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for reading supported alert notifications in central.
 *
 * @details This function will be called when a connection has been established.
 */
static void supported_alert_notification_read(void)
{
    NRF_LOG_DEBUG("Read supported Alert Notification characteristics on the connected peer.");

    ret_code_t err_code;

    err_code = ble_ans_c_new_alert_read(&m_ans_c);
    APP_ERROR_CHECK(err_code);

    err_code = ble_ans_c_unread_alert_read(&m_ans_c);
    APP_ERROR_CHECK(err_code);

}


/**@brief Function for handling the key of the New Alert Notification button.
 *
 * @details This function check the current state of new alert and then, enable, disable or clear
 *          the alert depending on the state.
 */
static void new_alert_state_toggle(void)
{
    ret_code_t err_code = NRF_SUCCESS;

    if (m_new_alert_state == ALERT_NOTIFICATION_ON)
    {
        m_new_alert_state = ALERT_NOTIFICATION_ENABLED;
        err_code          = bsp_indication_set(BSP_INDICATE_ALERT_OFF);
        NRF_LOG_INFO("New Alert State: Off.");
    }
    else if (m_new_alert_state == ALERT_NOTIFICATION_ENABLED)
    {
        m_new_alert_state = ALERT_NOTIFICATION_DISABLED;
        err_code          = ble_ans_c_disable_notif_new_alert(&m_ans_c);
        NRF_LOG_INFO("New Alert State: Disabled.");
    }
    else
    {
        m_new_alert_state = ALERT_NOTIFICATION_ENABLED;
        err_code          = ble_ans_c_enable_notif_new_alert(&m_ans_c);
        NRF_LOG_INFO("New Alert State: Enabled.");
    }

    // If the user presses the button while we are not connected,
    // we will have NRF_ERROR_INVALID_STATE, thus we just ignore the error code.
    if (err_code != NRF_SUCCESS && err_code != NRF_ERROR_INVALID_STATE)
    {
        APP_ERROR_HANDLER(err_code);
    }
}


/**@brief Function for handling the key of the Unread Alert button.
 *
 * @details This function check the current state of unread alert and then, enable, disable or
 *          clear the alert depending on the state.
 */
static void unread_alert_state_toggle(void)
{
    ret_code_t err_code = NRF_SUCCESS;

    if (m_unread_alert_state == ALERT_NOTIFICATION_ON)
    {
        m_unread_alert_state = ALERT_NOTIFICATION_ENABLED;
        err_code             = bsp_indication_set(BSP_INDICATE_ALERT_OFF);
        NRF_LOG_INFO("Unread Alert State: Off.");
    }
    else if (m_unread_alert_state == ALERT_NOTIFICATION_ENABLED)
    {
        m_unread_alert_state = ALERT_NOTIFICATION_DISABLED;
        err_code             = ble_ans_c_disable_notif_unread_alert(&m_ans_c);
        NRF_LOG_INFO("Unread Alert State: Disabled.");
    }
    else
    {
        m_unread_alert_state = ALERT_NOTIFICATION_ENABLED;
        err_code             = ble_ans_c_enable_notif_unread_alert(&m_ans_c);
        NRF_LOG_INFO("Unread Alert State: Enabled.");
    }

    // If the user presses the button while we are not connected,
    // we will have NRF_ERROR_INVALID_STATE, thus we just ignore the error code.
    if (err_code != NRF_SUCCESS && err_code != NRF_ERROR_INVALID_STATE)
    {
        APP_ERROR_HANDLER(err_code);
    }
}


/**@brief Function for handling key presses of the All Alert Notify button.
 *
 * @details This function check the current state of the alert notifications and based on the state
 *          it will request the central to resend current alert counters.
 */
static void all_alert_notify_request(void)
{
    ret_code_t err_code = NRF_SUCCESS;

    if (m_unread_alert_state == ALERT_NOTIFICATION_ON ||
        m_unread_alert_state == ALERT_NOTIFICATION_ENABLED
       )
    {
        err_code = ble_ans_c_unread_alert_notify(&m_ans_c, ANS_TYPE_ALL_ALERTS);

        if (err_code != NRF_SUCCESS && err_code != NRF_ERROR_INVALID_STATE)
        {
            APP_ERROR_HANDLER(err_code);
        }
        NRF_LOG_INFO("Notify the Unread Alert characteristic for all categories.");
    }

    if (m_new_alert_state == ALERT_NOTIFICATION_ON ||
        m_new_alert_state == ALERT_NOTIFICATION_ENABLED
       )
    {
        err_code = ble_ans_c_new_alert_notify(&m_ans_c, ANS_TYPE_ALL_ALERTS);

        if (err_code != NRF_SUCCESS && err_code != NRF_ERROR_INVALID_STATE)
        {
            APP_ERROR_HANDLER(err_code);
        }
        NRF_LOG_INFO("Notify the New Alert characteristic for all categories.");
    }
}


/**@brief Function for lighting up the LED corresponding to the notification received.
 *
 * @param[in]   p_evt   Event containing the notification.
 */
static void handle_alert_notification(ble_ans_c_evt_t * p_evt)
{
    ret_code_t err_code;

    if (p_evt->uuid.uuid == BLE_UUID_UNREAD_ALERT_CHAR)
    {
        if (m_unread_alert_state == ALERT_NOTIFICATION_ENABLED)
        {
            err_code = bsp_indication_set(BSP_INDICATE_ALERT_1);
            APP_ERROR_CHECK(err_code);
            m_unread_alert_state = ALERT_NOTIFICATION_ON;
            NRF_LOG_INFO("Unread Alert state: On.");
            NRF_LOG_INFO("  Category:                 %s",
                         (uint32_t)lit_catid[p_evt->data.alert.alert_category]);
            NRF_LOG_INFO("  Number of unread alerts:  %d",
                         p_evt->data.alert.alert_category_count);
        }
    }
    else if (p_evt->uuid.uuid == BLE_UUID_NEW_ALERT_CHAR)
    {
        if (m_new_alert_state == ALERT_NOTIFICATION_ENABLED)
        {
            err_code = bsp_indication_set(BSP_INDICATE_ALERT_0);
            APP_ERROR_CHECK(err_code);
            m_new_alert_state = ALERT_NOTIFICATION_ON;
            NRF_LOG_INFO("New Alert state: On.");
            NRF_LOG_INFO("  Category:                 %s",
                         (uint32_t)lit_catid[p_evt->data.alert.alert_category]);
            NRF_LOG_INFO("  Number of new alerts:     %d",
                         p_evt->data.alert.alert_category_count);
            NRF_LOG_INFO("  Text String Information:  %s",
                         (uint32_t)p_evt->data.alert.p_alert_msg_buf);
        }
    }
    else
    {
        // Only Unread and New Alerts exists, thus do nothing.
    }
}


/**@brief Function for initializing the timer module.
 */
static void timers_init(void)
{
    // Initialize timer module, making it use the scheduler.
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the Alert Notification Service Client.
 *
 * @details This function will be called for all events in the Alert Notification Client which
 *          are passed to the application.
 *
 * @param[in]   p_evt   Event received from the Alert Notification Service Client.
 */
static void on_ans_c_evt(ble_ans_c_evt_t * p_evt)
{
    ret_code_t err_code;

    switch (p_evt->evt_type)
    {
        case BLE_ANS_C_EVT_NOTIFICATION:
            handle_alert_notification(p_evt);
            NRF_LOG_DEBUG("Alert Notification received from server, UUID: %X.", p_evt->uuid.uuid);
            break; // BLE_ANS_C_EVT_NOTIFICATION

        case BLE_ANS_C_EVT_DISCOVERY_COMPLETE:
            NRF_LOG_DEBUG("Alert Notification Service discovered on the server.");
            err_code = ble_ans_c_handles_assign(&m_ans_c,
                                                p_evt->conn_handle,
                                                &p_evt->data.service);
            APP_ERROR_CHECK(err_code);
            supported_alert_notification_read();
            alert_notification_setup();
            break; // BLE_ANS_C_EVT_DISCOVERY_COMPLETE

        case BLE_ANS_C_EVT_READ_RESP:
            NRF_LOG_DEBUG("Alert Setup received from server, UUID: %X.", p_evt->uuid.uuid);
            control_point_setup(p_evt);
            break; // BLE_ANS_C_EVT_READ_RESP

        case BLE_ANS_C_EVT_DISCONN_COMPLETE:
            m_new_alert_state    = ALERT_NOTIFICATION_DISABLED;
            m_unread_alert_state = ALERT_NOTIFICATION_DISABLED;

            err_code = bsp_indication_set(BSP_INDICATE_ALERT_OFF);
            APP_ERROR_CHECK(err_code);
            break; // BLE_ANS_C_EVT_DISCONN_COMPLETE

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for handling the Alert Notification Service Client errors.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void alert_notification_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initialization of the Alert Notification Service Client.
 */
static void alert_notification_init()
{
    uint32_t           err_code;
    ble_ans_c_init_t   ans_init_obj;
    nrf_ble_qwr_init_t qwr_init = {0};

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // Initialize the Alert Notification Service Client.
    memset(&ans_init_obj, 0, sizeof(ans_init_obj));
    memset(m_alert_message_buffer, 0, MESSAGE_BUFFER_SIZE);

    ans_init_obj.evt_handler         = on_ans_c_evt;
    ans_init_obj.message_buffer_size = MESSAGE_BUFFER_SIZE;
    ans_init_obj.p_message_buffer    = m_alert_message_buffer;
    ans_init_obj.error_handler       = alert_notification_error_handler;

    err_code = ble_ans_c_init(&m_ans_c, &ans_init_obj);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling database discovery events.
 *
 * @details This function is callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function should forward the events
 *          to their respective services.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 */
static void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{
    ble_ans_c_on_db_disc_evt(&m_ans_c, p_evt);
}


/** @brief Database discovery module initialization.
 */
static void db_discovery_init(void)
{
    ret_code_t err_code = ble_db_discovery_init(db_disc_handler);

    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = true;
    cp_init.evt_handler                    = NULL;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    ret_code_t err_code = bsp_indication_set(BSP_INDICATE_IDLE);

    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    ret_code_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_DIRECTED_HIGH_DUTY:
            NRF_LOG_INFO("Directed advertising");
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_DIRECTED);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_FAST:
            NRF_LOG_INFO("Fast advertising");
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_SLOW:
            NRF_LOG_INFO("Slow advertising");
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_SLOW);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();
            break;

        default:
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

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected.");
            // LED indication will be changed when advertising starts.
            break;

        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected.");
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_cur_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = ble_db_discovery_start(&m_ble_db_discovery, m_cur_conn_handle);
            APP_ERROR_CHECK(err_code);
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_cur_conn_handle);
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
            m_cur_conn_handle = BLE_CONN_HANDLE_INVALID;
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

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in] event  Event generated by button press.
 */
static void bsp_event_handler(bsp_event_t event)
{
    ret_code_t err_code;

    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break;

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_cur_conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BSP_EVENT_KEY_0:
            NRF_LOG_DEBUG("Button 1 pushed.");
            if (m_ans_c.conn_handle != BLE_CONN_HANDLE_INVALID)
            {
                new_alert_state_toggle();
            }
            break;

        case BSP_EVENT_KEY_1:
            NRF_LOG_DEBUG("Button 2 pushed.");
            if (m_ans_c.conn_handle != BLE_CONN_HANDLE_INVALID)
            {
                unread_alert_state_toggle();
            }
            break;

        case BSP_EVENT_KEY_2:
            NRF_LOG_DEBUG("Button 3 pushed.");
            if (m_ans_c.conn_handle != BLE_CONN_HANDLE_INVALID)
            {
                all_alert_notify_request();
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

    // Register a handler for BLE events.
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


/**@brief Clear bond information from persistent storage.
 */
static void delete_bonds(void)
{
    ret_code_t err_code;

    NRF_LOG_INFO("Erase bonds!");

    err_code = pm_peers_delete();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    ret_code_t    err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = 0;
    init.advdata.uuids_complete.p_uuids  = NULL;

    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = APP_ADV_FAST_INTERVAL;
    init.config.ble_adv_fast_timeout  = APP_ADV_FAST_DURATION;
    init.config.ble_adv_slow_enabled  = true;
    init.config.ble_adv_slow_interval = APP_ADV_SLOW_INTERVAL;
    init.config.ble_adv_slow_timeout  = APP_ADV_SLOW_DURATION;

    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the
 *                            application up.
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


/**@brief Function for initializing the nrf log module.
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


/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}


/**@brief Function for starting advertising.
 */
static void advertising_start(bool erase_bonds)
{
    if (erase_bonds == true)
    {
        delete_bonds();
        // Advertising is started by PM_EVT_PEERS_DELETE_SUCCEEDED event.
    }
    else
    {
        ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
        APP_ERROR_CHECK(err_code);
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
    gap_params_init();
    gatt_init();
    advertising_init();
    db_discovery_init();
    alert_notification_init();
    conn_params_init();
    peer_manager_init();

    // Start execution.
    NRF_LOG_INFO("Alert Notification client example started.");
    advertising_start(erase_bonds);

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }
}


/**
 * @}
 */
