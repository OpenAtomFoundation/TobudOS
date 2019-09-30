/**
 * This software is subject to the ANT+ Shared Source License
 * www.thisisant.com/swlicenses
 * Copyright (c) Garmin Canada Inc. 2014
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 *    1) Redistributions of source code must retain the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer.
 *
 *    2) Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 *    3) Neither the name of Garmin nor the names of its
 *       contributors may be used to endorse or promote products
 *       derived from this software without specific prior
 *       written permission.
 *
 * The following actions are prohibited:
 *
 *    1) Redistribution of source code containing the ANT+ Network
 *       Key. The ANT+ Network Key is available to ANT+ Adopters.
 *       Please refer to http://thisisant.com to become an ANT+
 *       Adopter and access the key. 
 *
 *    2) Reverse engineering, decompilation, and/or disassembly of
 *       software provided in binary form under this license.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE HEREBY
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; DAMAGE TO ANY DEVICE, LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE. SOME STATES DO NOT ALLOW 
 * THE EXCLUSION OF INCIDENTAL OR CONSEQUENTIAL DAMAGES, SO THE
 * ABOVE LIMITATIONS MAY NOT APPLY TO YOU.
 *
 */
#include "ble_phone_connection.h"
#include "ble_gap.h"
#include "ble_advdata.h"
#include "boards.h"
#include "nrf_soc.h"

#include "ble_controllable_hub.h"
#include "asc_pages.h"
#include "app_util.h"
#include "app_error.h"
#include "leds.h"

#ifdef BOARD_N5DK1
    #define ADVERTISING_LED_PIN_NO          LED_A                                   /**< Is on when device is advertising. */
    #define CONNECTED_LED_PIN_NO            LED_B                                   /**< Is on when device has connected. */
#else
    #define ADVERTISING_LED_PIN_NO          BSP_BOARD_LED_0                         /**< Is on when device is advertising. */
    #define CONNECTED_LED_PIN_NO            BSP_BOARD_LED_1                         /**< Is on when device has connected. */
#endif

#define DEVICE_NAME                     "LightDemo"                                  /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "NordicSemiconductor"                        /**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL                1600                                         /**< The advertising interval (in units of 0.625 ms. This value corresponds to 1000 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      0                                            /**< Disabled. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(300, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (250 ms). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(300, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (250 ms). */
#define SLAVE_LATENCY                   0                                            /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(3000, UNIT_10_MS)               /**< Connection supervisory timeout (3 seconds), Supervision Timeout uses 10 ms units. */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)   /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000)   /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                            /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_TIMEOUT               30                                           /**< Timeout for Pairing Request or Security Request (in seconds). */
#define SEC_PARAM_BOND                  1                                            /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                            /**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                         /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                            /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                            /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                           /**< Maximum encryption key size. */

#define DEAD_BEEF                       0xDEADBEEF                                   /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;     /**< Handle of the current connection. */
static uint8_t                          m_adv_handle;                                /**< Advertising handle. */
static ble_achs_t                       m_achs;                                      /**< Structure used to identify the Controllable Hub service. */

/**@brief Start advertising.
 */
static void advertising_start(void)
{
    uint32_t err_code;

    err_code = sd_ble_gap_adv_start(m_adv_handle, CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
}


/**@brief GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode, (uint8_t *)DEVICE_NAME, strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**@brief Advertising functionality initialization.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void)
{
    uint32_t             err_code;
    ble_gap_adv_params_t adv_params;
    ble_gap_adv_data_t   advdata_enc;
    ble_advdata_t        advdata;
    static uint8_t       advdata_buff[BLE_GAP_ADV_SET_DATA_SIZE_MAX];
    uint16_t             advdata_buff_len = BLE_GAP_ADV_SET_DATA_SIZE_MAX;

    ble_uuid_t adv_uuids[] =
    {
        {0x9998, BLE_UUID_TYPE_BLE},
        {0x9999, BLE_UUID_TYPE_BLE}
    };

    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = false;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = adv_uuids;

    err_code = ble_advdata_encode(&advdata, advdata_buff, &advdata_buff_len);
    APP_ERROR_CHECK(err_code);

    m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;

    memset(&advdata_enc, 0, sizeof(advdata_enc));

    advdata_enc.adv_data.p_data = advdata_buff;
    advdata_enc.adv_data.len    = advdata_buff_len;

    // Initialise advertising parameters (used when starting advertising)
    memset(&adv_params, 0, sizeof(adv_params));

    adv_params.primary_phy     = BLE_GAP_PHY_1MBPS;
    adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
    adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;
    adv_params.interval        = APP_ADV_INTERVAL;
    adv_params.duration        = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &advdata_enc, &adv_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Application's Stack BLE event handler.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t const * p_ble_evt)
{
    uint32_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            led_on(CONNECTED_LED_PIN_NO);
            led_off(ADVERTISING_LED_PIN_NO);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

            break;

        case BLE_GAP_EVT_DISCONNECTED:
            led_off(CONNECTED_LED_PIN_NO);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            advertising_start();
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle,
                                                   BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP,
                                                   NULL,
                                                   NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_ADV_SET_TERMINATED:
            led_off(ADVERTISING_LED_PIN_NO);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle,
                                                 NULL,
                                                 0,
                                                 (BLE_GATTS_SYS_ATTR_FLAG_SYS_SRVCS | BLE_GATTS_SYS_ATTR_FLAG_USR_SRVCS));
            APP_ERROR_CHECK(err_code);
            break;

        default:
            // No implementation needed.
            break;
    }
}

///**@brief Dispatches a stack event to all modules with a stack BLE event handler.
// *
// * @details This function is called from the Stack event interrupt handler after a stack BLE
// *          event has been received.
// *
// * @param[in]   p_ble_evt   Stack Bluetooth event.
// */
//static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
//{
////    ble_hrs_on_ble_evt(&m_hrs, p_ble_evt);
////    ble_conn_params_on_ble_evt(p_ble_evt);
//}


/**@brief Initialize services that will be used by the application.
 *
 * @details Initialize the Heart Rate and Device Information services.
 */
static void services_init(void)
{
    uint32_t        err_code;
    ble_achs_init_t achs_init;

    memset(&achs_init, 0, sizeof(ble_achs_init_t));

    achs_init.evt_handler = NULL;

    err_code = ble_achs_init(&m_achs, &achs_init);
    APP_ERROR_CHECK(err_code);
}


void cntrldevice_initialize(void)
{
    // Initialize Bluetooth stack parameters.
    gap_params_init();

    advertising_init();

    services_init();

    advertising_start();
}


void cntrldevice_process_update(asc_update_data_t update)
{
    //Process a message from the queue if one exists and we are not already in the process of handling one

    //Pass the non-shared format of page along to the remote controls
    //Prepare the buffer
    ble_achs_report_t report;

    report.page_num = UPDATE_DATA_PID;
    report.shared_address = update.shared_address;
    report.master_id = update.master_id;
    report.value    = update.state;

    uint32_t err_code = ble_achs_report_send(&m_achs, &report);
    if (err_code == NRF_ERROR_INVALID_STATE)
    {
        // ignore invalid state, as we do not maintain a flag indicating if CCCD is enabled or not.
    }
    else
    {
        APP_ERROR_CHECK(err_code);
    }
}


void cntrldevice_event_handler(ble_evt_t const * p_ble_evt)
{
    on_ble_evt(p_ble_evt);
    ble_achs_on_ble_evt(&m_achs, p_ble_evt);
}
