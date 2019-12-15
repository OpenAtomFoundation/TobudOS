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
 * @defgroup ble_sdk_app_pwr_mgmt_main main.c
 * @{
 * @ingroup ble_sdk_app_pwr_mgmt
 * @brief Power profiling sample application main file.
 *
 * This file contains the source code for a sample application to demonstrate/measure the power
 * consumption by the nRF5 IC while sending notifications for a given duration and while
 * advertising in non-connectable mode for a given duration. The values of macros that begin
 * with APP_CFG_ prefix can be changed to alter the power consumption of the application.
 *
 * @ref srvlib_conn_params module.
 */

#include <stdint.h>
#include <string.h>
#include "nrf.h"
#include "nrf_soc.h"
#include "ble.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "bsp.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "app_timer.h"
#include "app_error.h"
#include "nordic_common.h"
#include "nrf_pwr_mgmt.h"

// User-modifiable configuration parameters.
//      The following values shall be altered when doing power profiling.

#define APP_CFG_NON_CONN_ADV_TIMEOUT    30                                            /**< Time for which the device must be advertising in non-connectable mode (in seconds). */
#define APP_CFG_CHAR_NOTIF_TIMEOUT      5000                                          /**< Time for which the device must continue to send notifications once connected to central (in milli seconds). */
#define APP_CFG_ADV_DATA_LEN            31                                            /**< Required length of the complete advertisement packet. This should be atleast 8 in order to accommodate flag field and other mandatory fields and one byte of manufacturer specific data. */
#define APP_CFG_CONNECTION_INTERVAL     20                                            /**< Connection interval used by the central (in milli seconds). This application will be sending one notification per connection interval. A repeating timer will be started with timeout value equal to this value and one notification will be sent everytime this timer expires. */
#define APP_CFG_CHAR_LEN                20                                            /**< Size of the characteristic value being notified (in bytes). */

// Fixed configuration parameters:
//      The following parameters are not meant to be changed while using this application for power
//      profiling.

#define NOTIF_BUTTON_ID                 0                                             /**< Button used for initializing the application in connectable mode. */
#define NON_CONN_ADV_BUTTON_ID          1                                             /**< Button used for initializing the application in non-connectable mode. */

#define DEVICE_NAME                     "Nordic_Power_Mgmt"                           /**< Name of device. Will be included in the advertising data. */
#define COMPANY_IDENTIFIER              0x0059                                        /**< Company identifier for Nordic Semiconductor ASA as per www.bluetooth.org. */

#define APP_BLE_OBSERVER_PRIO           3                                             /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                             /**< A tag identifying the SoftDevice BLE configuration. */

#define LOCAL_SERVICE_UUID              0x1523                                        /**< Proprietary UUID for local service. */
#define LOCAL_CHAR_UUID                 0x1524                                        /**< Proprietary UUID for local characteristic. */

#define DEAD_BEEF                       0xDEADBEEF                                    /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define CHAR_NOTIF_TIMEOUT_IN_TKS       APP_TIMER_TICKS(APP_CFG_CHAR_NOTIF_TIMEOUT)   /**< Time for which the device must continue to send notifications once connected to central (in ticks). */

#define CONNECTABLE_ADV_INTERVAL        MSEC_TO_UNITS(20, UNIT_0_625_MS)              /**< The advertising interval for connectable advertisement (20 ms). This value can vary between 20ms to 10.24s. */
#define NON_CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(100, UNIT_0_625_MS)             /**< The advertising interval for non-connectable advertisement (100 ms). This value can vary between 100ms to 10.24s). */

#define APP_ADV_DURATION                3000                                          /**< The advertising duration (30 seconds) in units of 10 milliseconds. */

#define SLAVE_LATENCY                   0                                             /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)               /**< Connection supervisory timeout (4 seconds). */

#define ADV_ENCODED_AD_TYPE_LEN         1                                             /**< Length of encoded ad type in advertisement data. */
#define ADV_ENCODED_AD_TYPE_LEN_LEN     1                                             /**< Length of the 'length field' of each ad type in advertisement data. */
#define ADV_FLAGS_LEN                   1                                             /**< Length of flags field that will be placed in advertisement data. */
#define ADV_ENCODED_FLAGS_LEN           (ADV_ENCODED_AD_TYPE_LEN +       \
                                        ADV_ENCODED_AD_TYPE_LEN_LEN +   \
                                        ADV_FLAGS_LEN)                               /**< Length of flags field in advertisement packet. (1 byte for encoded ad type plus 1 byte for length of flags plus the length of the flags itself). */
#define ADV_ENCODED_COMPANY_ID_LEN      2                                            /**< Length of the encoded Company Identifier in the Manufacturer Specific Data part of the advertisement data. */
#define ADV_ADDL_MANUF_DATA_LEN         (APP_CFG_ADV_DATA_LEN -                \
                                        (                                     \
                                            ADV_ENCODED_FLAGS_LEN +           \
                                            (                                 \
                                                ADV_ENCODED_AD_TYPE_LEN +     \
                                                ADV_ENCODED_AD_TYPE_LEN_LEN + \
                                                ADV_ENCODED_COMPANY_ID_LEN    \
                                            )                                 \
                                        )                                     \
                                        )                                             /**< Length of Manufacturer Specific Data field that will be placed on the air during advertisement. This is computed based on the value of APP_CFG_ADV_DATA_LEN (required advertisement data length). */


#if APP_CFG_ADV_DATA_LEN > BLE_GAP_ADV_SET_DATA_SIZE_MAX
    #error "The required advertisement data size (APP_CFG_ADV_DATA_LEN) is greater than the value allowed by stack (BLE_GAP_ADV_MAX_SIZE). Reduce the value of APP_CFG_ADV_DATA_LEN and recompile."
#endif

// Check whether the maximum characteristic length + opcode length (1) + handle length (2) is not
// greater than default MTU size.
#if (APP_CFG_CHAR_LEN + 1 + 2) > BLE_GATT_ATT_MTU_DEFAULT
    #error "The APP_CFG_CHAR_LEN is too large for the maximum MTU size."
#endif

#if ADV_ADDL_MANUF_DATA_LEN < 1
    #error "The required length of additional manufacturer specific data computed based on the user configured values is computed to be less than 1. Consider increasing the value of APP_CFG_ADV_DATA_LEN."
#endif


NRF_BLE_GATT_DEF(m_gatt);                                                           /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                             /**< Context for the Queued Write module.*/
APP_TIMER_DEF(m_conn_int_timer_id);                                                 /**< Connection interval timer. */
APP_TIMER_DEF(m_notif_timer_id);                                                    /**< Notification timer. */

static ble_gap_adv_params_t     m_adv_params;                                       /**< Parameters to be passed to the stack when starting advertising. */
static uint8_t                  m_char_value[APP_CFG_CHAR_LEN];                     /**< Value of the characteristic that will be sent as a notification to the central. */
static uint8_t                  m_addl_adv_manuf_data[ADV_ADDL_MANUF_DATA_LEN];     /**< Value of the additional manufacturer specific data that will be placed in air (initialized to all zeros). */
static ble_gatts_char_handles_t m_char_handles;                                     /**< Handles of local characteristic (as provided by the BLE stack).*/
static uint16_t                 m_conn_handle = BLE_CONN_HANDLE_INVALID;            /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).*/
static uint16_t                 m_service_handle;                                   /**< Handle of local service (as provided by the BLE stack).*/
static bool                     m_is_notifying_enabled = false;                     /**< Variable to indicate whether the notification is enabled by the peer.*/

static bool    m_is_notification_mode    = false;                                   /**< Flag to keep track of if the application is connectable and will have a service with notifications, or if it will advertise in non-connectable mode. */
static bool    m_is_non_connectable_mode = false;                                   /**< Flag to keep track of if the application is connectable and will have a service with notifications, or if it will advertise in non-connectable mode. */

static uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;                       /**< Advertising handle used to identify an advertising set. */
static uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];                        /**< Buffer for storing an encoded advertising set. */

/**@brief Struct that contains pointers to the encoded advertising data. */
static ble_gap_adv_data_t m_adv_data =
{
    .adv_data =
    {
        .p_data = m_enc_advdata,
        .len    = BLE_GAP_ADV_SET_DATA_SIZE_MAX
    },
    .scan_rsp_data =
    {
        .p_data = NULL,
        .len    = 0

    }
};

/**@brief 128-bit UUID base List. */
static ble_uuid128_t const m_base_uuid128 =
{
   {
       0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15,
       0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00
   }
};


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for the Characteristic notification.
 *
 * @details Sends one characteristic value notification to peer if connected to it and the
 *          notification is enabled.
 */
static void char_notify(void)
{
    ret_code_t err_code;
    uint16_t len = APP_CFG_CHAR_LEN;

    // Send value if connected and notifying.
    if ((m_conn_handle != BLE_CONN_HANDLE_INVALID) && m_is_notifying_enabled)
    {
        ble_gatts_hvx_params_t hvx_params;

        memset(&hvx_params, 0, sizeof(hvx_params));
        len = sizeof(uint8_t);

        hvx_params.handle = m_char_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &len;
        hvx_params.p_data = m_char_value;

        err_code = sd_ble_gatts_hvx(m_conn_handle, &hvx_params);
        if ((err_code != NRF_SUCCESS) &&
            (err_code != NRF_ERROR_INVALID_STATE) &&
            (err_code != NRF_ERROR_RESOURCES) &&
            (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
        )
        {
            APP_ERROR_HANDLER(err_code);
        }
    }
}


/**@brief Function for the GAP initialization.
 *
 * @details This function shall be used to setup all the necessary GAP (Generic Access Profile)
 *          parameters of the device. It also sets the permissions.
 *
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

    // Set GAP Peripheral Preferred Connection Parameters (converting connection interval from
    // milliseconds to required unit of 1.25ms).
    gap_conn_params.min_conn_interval = (4 * APP_CFG_CONNECTION_INTERVAL) / 5;
    gap_conn_params.max_conn_interval = (4 * APP_CFG_CONNECTION_INTERVAL) / 5;
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


/**@brief Function for initializing the Queued Write module.
 */
static void qwr_init(void)
{
    ret_code_t         err_code;
    nrf_ble_qwr_init_t qwr_init_obj = {0};

    // Initialize Queued Write Module.
    qwr_init_obj.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init_obj);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the connectable advertisement parameters.
 *
 * @details This function initializes the advertisement parameters to values that will put
 *          the application in connectable mode.
 *
 */
static void connectable_adv_init(void)
{
    // Initialize advertising parameters (used when starting advertising).
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
    m_adv_params.duration        = APP_ADV_DURATION;

    m_adv_params.p_peer_addr   = NULL;
    m_adv_params.filter_policy = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval      = CONNECTABLE_ADV_INTERVAL;
    m_adv_params.primary_phy   = BLE_GAP_PHY_1MBPS;
}


/**@brief Function for initializing the non-connectable advertisement parameters.
 *
 * @details This function initializes the advertisement parameters to values that will put
 *          the application in non-connectable mode.
 *
 */
static void non_connectable_adv_init(void)
{
    // Initialize advertising parameters (used when starting advertising).
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.properties.type = BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED; // BLE_GAP_ADV_TYPE_CONNECTABLE_NONSCANNABLE_DIRECTED; //
    m_adv_params.duration        = APP_ADV_DURATION;
    m_adv_params.p_peer_addr     = NULL;
    m_adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval        = NON_CONNECTABLE_ADV_INTERVAL;
    m_adv_params.primary_phy     = BLE_GAP_PHY_1MBPS;
}


/**@brief Function for initializing the Advertisement packet.
 *
 * @details This function initializes the data that is to be placed in an advertisement packet in
 *          both connectable and non-connectable modes.
 *
 */
static void advertising_init(void)
{
    ret_code_t               err_code;
    ble_advdata_t            advdata;
    ble_advdata_manuf_data_t manuf_data;
    uint8_t                  flags = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;

    APP_ERROR_CHECK_BOOL(sizeof(flags) == ADV_FLAGS_LEN);  // Assert that these two values of the same.

    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));

    manuf_data.company_identifier = COMPANY_IDENTIFIER;
    manuf_data.data.size          = ADV_ADDL_MANUF_DATA_LEN;
    manuf_data.data.p_data        = m_addl_adv_manuf_data;
    advdata.flags                 = flags;
    advdata.p_manuf_specific_data = &manuf_data;

    err_code = ble_advdata_encode(&advdata, m_adv_data.adv_data.p_data, &m_adv_data.adv_data.len);
    APP_ERROR_CHECK(err_code);

    if(m_is_non_connectable_mode)
    {
        err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, NULL, &m_adv_params);
        APP_ERROR_CHECK(err_code);
    }
    else
    {
        err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, &m_adv_params);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for adding the Characteristic.
 *
 * @details This function adds the characteristic to the local db.
 *
 * @param[in] uuid_type Type of service UUID assigned by the SoftDevice.
 *
 */
static void char_add(const uint8_t uuid_type)
{
    ret_code_t          err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          char_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;

    char_uuid.type = uuid_type;
    char_uuid.uuid = LOCAL_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);

    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = APP_CFG_CHAR_LEN;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = APP_CFG_CHAR_LEN;
    attr_char_value.p_value   = m_char_value;

    err_code = sd_ble_gatts_characteristic_add(m_service_handle,
                                               &char_md,
                                               &attr_char_value,
                                               &m_char_handles);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for adding the Service.
 *
 * @details This function adds the service and the characteristic within it to the local db.
 *
 */
static void service_add(void)
{
    ble_uuid_t service_uuid;
    ret_code_t err_code;

    service_uuid.uuid = LOCAL_SERVICE_UUID;

    err_code = sd_ble_uuid_vs_add(&m_base_uuid128, &service_uuid.type);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service_uuid, &m_service_handle);
    APP_ERROR_CHECK(err_code);

    // Add characteristics
    char_add(service_uuid.type);
}


/**@brief Function for handling the Notification timeout.
 *
 * @details This function will be called when the notification timer expires. This will stop the
 *          timer for connection interval and disconnect from the peer.
 *
 * @param[in]   p_context   Pointer used for passing some arbitrary information (context) from the
 *                          app_start_timer() call to the timeout handler.
 */
static void notif_timeout_handler(void * p_context)
{
    ret_code_t err_code;

    UNUSED_PARAMETER(p_context);

    // Stop all notifications (by stopping the timer for connection interval that triggers
    // notifications and disconnecting from peer).
    err_code = app_timer_stop(m_conn_int_timer_id);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the Connection interval timeout.
 *
 * @details This function will be called when the connection interval timer expires. This will
 *          trigger another characteristic notification to the peer.
 *
 * @param[in]   p_context   Pointer used for passing some arbitrary information (context) from the
 *                          app_start_timer() call to the timeout handler.
 */
static void connection_interval_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);

    // Into next connection interval. Send one notification.
    char_notify();
}


/**@brief Function for starting application timers.
 *
 * @details This function will be start two timers - one for the time duration for which
 *          notifications have to be sent to the peer and another for the interval between two
 *          notifications (which is also the connection interval).
 */
static void application_timers_start(void)
{
    ret_code_t err_code;

    // Start connection interval timer.
    err_code = app_timer_start(m_conn_int_timer_id,
                               APP_TIMER_TICKS(APP_CFG_CONNECTION_INTERVAL),
                               NULL);
    APP_ERROR_CHECK(err_code);

    // Start characteristic notification timer.
    err_code = app_timer_start(m_notif_timer_id, CHAR_NOTIF_TIMEOUT_IN_TKS, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for Stopping application timers.
 */
static void application_timers_stop(void)
{
    ret_code_t err_code;

    err_code = app_timer_stop(m_notif_timer_id);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_stop(m_conn_int_timer_id);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    ret_code_t err_code;

    err_code = sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the Timer initialization.
 *
* @details Initializes the timer module.
*/
static void timers_init(void)
{
    ret_code_t err_code;

    // Initialize timer module
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    // Create timers
    err_code = app_timer_create(&m_conn_int_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                connection_interval_timeout_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_notif_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                notif_timeout_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing buttons.
 */
static void buttons_init(void)
{
    ret_code_t err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for evaluating the value written in CCCD
 *
 * @details This shall be called when there is a write event received from the stack. This
 *          function will evaluate whether or not the peer has enabled notifications and
 *          start/stop notifications accordingly.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_write(ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if ((p_evt_write->handle == m_char_handles.cccd_handle) && (p_evt_write->len == 2))
    {
        // CCCD written. Start notifications
        m_is_notifying_enabled = ble_srv_is_notification_enabled(p_evt_write->data);

        if (m_is_notifying_enabled)
        {
            application_timers_start();
            char_notify();
        }
        else
        {
            application_timers_stop();
        }
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

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            application_timers_stop();
            // Go to system-off mode
            err_code = sd_power_system_off();
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle,
                                                 NULL,
                                                 0,
                                                 BLE_GATTS_SYS_ATTR_FLAG_SYS_SRVCS | BLE_GATTS_SYS_ATTR_FLAG_USR_SRVCS);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_ADV_SET_TERMINATED:
            if (p_ble_evt->evt.gap_evt.params.adv_set_terminated.reason == BLE_GAP_EVT_ADV_SET_TERMINATED_REASON_TIMEOUT)
            {
                // Go to system-off mode (this function will not return; wakeup will cause a reset).
                err_code = sd_power_system_off();
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_ble_evt);
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
 * @details Sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
    nrf_pwr_mgmt_run();
}


/**@brief Function for application main entry.
 */
int main(void)
{
    timers_init();
    buttons_init();
    power_management_init();

#if BUTTONS_NUMBER > 2
    // Check button states.
    // Notification Start button.
    m_is_notification_mode = bsp_board_button_state_get(NOTIF_BUTTON_ID);

    // Non-connectable advertisement start button.
    if (!m_is_notification_mode)
    {
        m_is_non_connectable_mode = bsp_board_button_state_get(NON_CONN_ADV_BUTTON_ID);
    }
    else
    {
        // Un-configured button.
    }
#else
    m_is_notification_mode = true;
#endif

    // Initialize SoftDevice.
    ble_stack_init();

    if (!m_is_notification_mode && !m_is_non_connectable_mode)
    {
        // The startup was not because of button presses. This is the first start.
        // Go into System-Off mode. Button presses will wake the chip up.
        nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
    }

    // If we reach this point, the application was woken up
    // by pressing one of the two configured buttons.
    gap_params_init();
    gatt_init();
    qwr_init();

    if (m_is_notification_mode)
    {
        // Notification button is pressed. Start connectable advertisement.
        connectable_adv_init();
        service_add();
    }
    else
    {
        non_connectable_adv_init();
    }

    advertising_init();
    advertising_start();

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }
}

/**
 * @}
 */
