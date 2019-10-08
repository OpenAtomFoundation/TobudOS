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
 * @defgroup ble_sdk_app_bps_main main.c
 * @{
 * @ingroup ble_sdk_app_bps
 * @brief Blood Pressure Service Sample Application main file.
 *
 * This file contains the source code for a sample application using the Blood pressure service.
 * This file also contains the code for initializing and using the Battery Service and the Device
 * Information Service. Furthermore, it demonstrates the use of the @ref srvlib_conn_params module.
 */

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
#include "ble_advertising.h"
#include "ble_bas.h"
#include "ble_bps.h"
#include "ble_dis.h"
#include "ble_conn_params.h"
#include "sensorsim.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "app_timer.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "bsp_btn_ble.h"
#include "fds.h"
#include "ble_conn_state.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


#define DEVICE_NAME                     "Nordic_BPS"                            /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "NordicSemiconductor"                   /**< Manufacturer. Will be passed to Device Information Service. */
#define MODEL_NUM                       "NS-BPS-EXAMPLE"                        /**< Model number. Will be passed to Device Information Service. */
#define MANUFACTURER_ID                 0x1122334455                            /**< Manufacturer ID, part of System ID. Will be passed to Device Information Service. */
#define ORG_UNIQUE_ID                   0x667788                                /**< Organizational Unique ID, part of System ID. Will be passed to Device Information Service. */

#define APP_BLE_OBSERVER_PRIO           3                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                       /**< A tag identifying the SoftDevice BLE configuration. */

#define APP_ADV_INTERVAL                40                                      /**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */

#define APP_ADV_DURATION                18000                                   /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define NUM_SIM_MEAS_VALUES             4                                       /**< Number of simulated measurements to cycle through. */

#define SIM_MEAS_1_SYSTOLIC             117                                     /**< Simulated measurement value for systolic pressure. */
#define SIM_MEAS_1_DIASTOLIC            76                                      /**< Simulated measurement value for diastolic pressure. */
#define SIM_MEAS_1_MEAN_AP              103                                     /**< Simulated measurement value for mean arterial pressure. */
#define SIM_MEAS_1_PULSE_RATE           60                                      /**< Simulated measurement value for pulse rate. */

#define SIM_MEAS_2_SYSTOLIC             121                                     /**< Simulated measurement value for systolic pressure. */
#define SIM_MEAS_2_DIASTOLIC            81                                      /**< Simulated measurement value for diastolic pressure. */
#define SIM_MEAS_2_MEAN_AP              106                                     /**< Simulated measurement value for mean arterial pressure. */
#define SIM_MEAS_2_PULSE_RATE           72                                      /**< Simulated measurement value for pulse rate. */

#define SIM_MEAS_3_SYSTOLIC             138                                     /**< Simulated measurement value for systolic pressure. */
#define SIM_MEAS_3_DIASTOLIC            88                                      /**< Simulated measurement value for diastolic pressure. */
#define SIM_MEAS_3_MEAN_AP              120                                     /**< Simulated measurement value for mean arterial pressure. */
#define SIM_MEAS_3_PULSE_RATE           105                                     /**< Simulated measurement value for pulse rate. */

#define SIM_MEAS_4_SYSTOLIC             145                                     /**< Simulated measurement value for systolic pressure. */
#define SIM_MEAS_4_DIASTOLIC            100                                     /**< Simulated measurement value for diastolic pressure. */
#define SIM_MEAS_4_MEAN_AP              131                                     /**< Simulated measurement value for mean arterial pressure. */
#define SIM_MEAS_4_PULSE_RATE           125                                     /**< Simulated measurement value for pulse rate. */

#define BATTERY_LEVEL_MEAS_INTERVAL     APP_TIMER_TICKS(2000)                   /**< Battery level measurement interval (ticks). */
#define MIN_BATTERY_LEVEL               81                                      /**< Minimum battery level as returned by the simulated measurement function. */
#define MAX_BATTERY_LEVEL               100                                     /**< Maximum battery level as returned by the simulated measurement function. */
#define BATTERY_LEVEL_INCREMENT         1                                       /**< Value by which the battery level is incremented/decremented for each call to the simulated measurement function. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(500, UNIT_1_25_MS)        /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(1000, UNIT_1_25_MS)       /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                   0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)         /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                   /**< Time from initiating event (connect or start of indication) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                       /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                  1                                       /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                       /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                       /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                       /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                    /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                       /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                       /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                      /**< Maximum encryption key size. */

#define DEAD_BEEF                       0xDEADBEEF                              /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */


/**@brief Structure for a simulated blood pressure measurment. An instance of this struct is
          filled out before sending a notification to the peer with ble_bps_measurement_send.
 */
typedef struct
{
    ieee_float16_t systolic;
    ieee_float16_t diastolic;
    ieee_float16_t mean_arterial;
    ieee_float16_t pulse_rate;
} bps_meas_sim_value_t;


APP_TIMER_DEF(m_battery_timer_id);                                      /**< Battery timer. */
BLE_BAS_DEF(m_bas);                                                     /**< Structure used to identify the battery service. */
BLE_BPS_DEF(m_bps);                                                     /**< Structure used to identify the blood pressure service. */
NRF_BLE_GATT_DEF(m_gatt);                                               /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                 /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);                                     /**< Advertising module instance. */

static uint16_t             m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */
static bps_meas_sim_value_t m_bps_meas_sim_val[NUM_SIM_MEAS_VALUES];    /**< Blood Pressure simulated measurements. */
static bool                 m_bps_meas_ind_conf_pending = false;        /**< Flag to keep track of when an indication confirmation is pending. */
static sensorsim_cfg_t      m_battery_sim_cfg;                          /**< Battery Level sensor simulator configuration. */
static sensorsim_state_t    m_battery_sim_state;                        /**< Battery Level sensor simulator state. */

static ble_uuid_t m_adv_uuids[] =                                       /**< Universally unique service identifiers. */
{
    {BLE_UUID_BLOOD_PRESSURE_SERVICE,     BLE_UUID_TYPE_BLE},
    {BLE_UUID_BATTERY_SERVICE,            BLE_UUID_TYPE_BLE},
    {BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE}
};


static void advertising_start(bool erase_bonds);
static void blood_pressure_measurement_send(void);

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


/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    ret_code_t err_code;
    bool       is_indication_enabled;

    pm_handler_on_pm_evt(p_evt);
    pm_handler_flash_clean(p_evt);

    switch (p_evt->evt_id)
    {
        case PM_EVT_CONN_SEC_SUCCEEDED:
            // Send a single blood pressure measurement if indication is enabled.
            // NOTE: For this to work, make sure ble_bps_on_ble_evt() is called before
            // ble_bondmngr_on_ble_evt() in ble_evt_dispatch().
            err_code = ble_bps_is_indication_enabled(&m_bps, &is_indication_enabled);
            APP_ERROR_CHECK(err_code);

            if (is_indication_enabled)
            {
                blood_pressure_measurement_send();
            }
            break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            advertising_start(false);
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
        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
       )
    {
        APP_ERROR_HANDLER(err_code);
    }
}


/**@brief Function for handling the Battery measurement timer timeout.
 *
 * @details This function will be called each time the battery level measurement timer expires.
 *
 * @param[in]   p_context   Pointer used for passing some arbitrary information (context) from the
 *                          app_start_timer() call to the timeout handler.
 */
static void battery_level_meas_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);
    battery_level_update();
}


/**@brief Function for populating simulated blood pressure measurements.
 */
static void bps_sim_measurement(ble_bps_meas_t * p_meas)
{
    static ble_date_time_t s_time_stamp = { 2012, 12, 5, 11, 05, 03 };
    static uint8_t         s_ndx        = 0;

    p_meas->blood_pressure_units_in_kpa = false;
    p_meas->time_stamp_present          = (s_ndx == 0) || (s_ndx == 2);
    p_meas->pulse_rate_present          = (s_ndx == 0) || (s_ndx == 1);
    p_meas->user_id_present             = false;
    p_meas->measurement_status_present  = false;

    p_meas->blood_pressure_systolic.mantissa = m_bps_meas_sim_val[s_ndx].systolic.mantissa;
    p_meas->blood_pressure_systolic.exponent = m_bps_meas_sim_val[s_ndx].systolic.exponent;

    p_meas->blood_pressure_diastolic.mantissa = m_bps_meas_sim_val[s_ndx].diastolic.mantissa;
    p_meas->blood_pressure_diastolic.exponent = m_bps_meas_sim_val[s_ndx].diastolic.exponent;

    p_meas->mean_arterial_pressure.mantissa = m_bps_meas_sim_val[s_ndx].mean_arterial.mantissa;
    p_meas->mean_arterial_pressure.exponent = m_bps_meas_sim_val[s_ndx].mean_arterial.exponent;

    p_meas->time_stamp = s_time_stamp;

    p_meas->pulse_rate.mantissa = m_bps_meas_sim_val[s_ndx].pulse_rate.mantissa;
    p_meas->pulse_rate.exponent = m_bps_meas_sim_val[s_ndx].pulse_rate.exponent;

    // Update index to simulated measurements.
    s_ndx++;
    if (s_ndx == NUM_SIM_MEAS_VALUES)
    {
        s_ndx = 0;
    }

    // Update simulated time stamp.
    s_time_stamp.seconds += 27;
    if (s_time_stamp.seconds > 59)
    {
        s_time_stamp.seconds -= 60;

        s_time_stamp.minutes++;
        if (s_time_stamp.minutes > 59)
        {
            s_time_stamp.minutes = 0;
        }
    }
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

    // Create timers.
    err_code = app_timer_create(&m_battery_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                battery_level_meas_timeout_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the GAP initialization.
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

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_BLOOD_PRESSURE);
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


/**@brief Function for simulating and sending one Blood Pressure Measurement.
 */
static void blood_pressure_measurement_send(void)
{
    ble_bps_meas_t simulated_meas;
    uint32_t       err_code;
    bool           is_indication_enabled;

    err_code = ble_bps_is_indication_enabled(&m_bps, &is_indication_enabled);
    APP_ERROR_CHECK(err_code);

    if (is_indication_enabled && !m_bps_meas_ind_conf_pending)
    {
        bps_sim_measurement(&simulated_meas);

        err_code = ble_bps_measurement_send(&m_bps, &simulated_meas);

        switch (err_code)
        {
            case NRF_SUCCESS:
                // Measurement was successfully sent, wait for confirmation.
                m_bps_meas_ind_conf_pending = true;
                break;

            case NRF_ERROR_INVALID_STATE:
                // Ignore error.
                break;

            default:
                APP_ERROR_HANDLER(err_code);
                break;
        }
    }
}


/**@brief Function for handling the Blood Pressure Service events.
 *
 * @details This function will be called for all Blood Pressure Service events which are passed to
 *          the application.
 *
 * @param[in]   p_bps   Blood Pressure Service structure.
 * @param[in]   p_evt   Event received from the Blood Pressure Service.
 */
static void on_bps_evt(ble_bps_t * p_bps, ble_bps_evt_t * p_evt)
{
    switch (p_evt->evt_type)
    {
        case BLE_BPS_EVT_INDICATION_ENABLED:
            // Indication has been enabled, send a single blood pressure measurement.
            blood_pressure_measurement_send();
            break;

        case BLE_BPS_EVT_INDICATION_CONFIRMED:
            m_bps_meas_ind_conf_pending = false;
            break;

        default:
            // No implementation needed.
            break;
    }
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


/**@brief Function for initializing services that will be used by the application.
 *
 * @details Initialize the Blood Pressure, Battery, and Device Information services.
 */
static void services_init(void)
{
    uint32_t           err_code;
    ble_bps_init_t     bps_init;
    ble_bas_init_t     bas_init;
    ble_dis_init_t     dis_init;
    ble_dis_sys_id_t   sys_id;
    nrf_ble_qwr_init_t qwr_init = {0};

    // Initialize Queued Write Module
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Blood Pressure Service.
    memset(&bps_init, 0, sizeof(bps_init));

    bps_init.evt_handler = on_bps_evt;
    bps_init.feature     = BLE_BPS_FEATURE_BODY_MOVEMENT_BIT |
                           BLE_BPS_FEATURE_MEASUREMENT_POSITION_BIT;

    // Here the sec level for the Blood Pressure Service can be changed/increased.
    bps_init.bp_cccd_wr_sec    = SEC_JUST_WORKS;
    bps_init.bp_feature_rd_sec = SEC_OPEN;

    err_code = ble_bps_init(&m_bps, &bps_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Battery Service.
    memset(&bas_init, 0, sizeof(bas_init));

    // Here the sec level for the Battery Service can be changed/increased.
    bas_init.bl_rd_sec        = SEC_OPEN;
    bas_init.bl_cccd_wr_sec   = SEC_OPEN;
    bas_init.bl_report_rd_sec = SEC_OPEN;

    bas_init.evt_handler          = NULL;
    bas_init.support_notification = true;
    bas_init.p_report_ref         = NULL;
    bas_init.initial_batt_level   = 100;

    err_code = ble_bas_init(&m_bas, &bas_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Device Information Service.
    memset(&dis_init, 0, sizeof(dis_init));

    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, MANUFACTURER_NAME);
    ble_srv_ascii_to_utf8(&dis_init.model_num_str, MODEL_NUM);

    sys_id.manufacturer_id            = MANUFACTURER_ID;
    sys_id.organizationally_unique_id = ORG_UNIQUE_ID;
    dis_init.p_sys_id                 = &sys_id;

    dis_init.dis_char_rd_sec = SEC_OPEN;

    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);
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

    // Simulated measurement #1.
    m_bps_meas_sim_val[0].systolic.mantissa      = SIM_MEAS_1_SYSTOLIC;
    m_bps_meas_sim_val[0].systolic.exponent      = 0;
    m_bps_meas_sim_val[0].diastolic.mantissa     = SIM_MEAS_1_DIASTOLIC;
    m_bps_meas_sim_val[0].diastolic.exponent     = 0;
    m_bps_meas_sim_val[0].mean_arterial.mantissa = SIM_MEAS_1_MEAN_AP;
    m_bps_meas_sim_val[0].mean_arterial.exponent = 0;
    m_bps_meas_sim_val[0].pulse_rate.mantissa    = SIM_MEAS_1_PULSE_RATE;
    m_bps_meas_sim_val[0].pulse_rate.exponent    = 0;

    // Simulated measurement #2.
    m_bps_meas_sim_val[1].systolic.mantissa      = SIM_MEAS_2_SYSTOLIC;
    m_bps_meas_sim_val[1].systolic.exponent      = 0;
    m_bps_meas_sim_val[1].diastolic.mantissa     = SIM_MEAS_2_DIASTOLIC;
    m_bps_meas_sim_val[1].diastolic.exponent     = 0;
    m_bps_meas_sim_val[1].mean_arterial.mantissa = SIM_MEAS_2_MEAN_AP;
    m_bps_meas_sim_val[1].mean_arterial.exponent = 0;
    m_bps_meas_sim_val[1].pulse_rate.mantissa    = SIM_MEAS_2_PULSE_RATE;
    m_bps_meas_sim_val[1].pulse_rate.exponent    = 0;

    // Simulated measurement #3.
    m_bps_meas_sim_val[2].systolic.mantissa      = SIM_MEAS_3_SYSTOLIC;
    m_bps_meas_sim_val[2].systolic.exponent      = 0;
    m_bps_meas_sim_val[2].diastolic.mantissa     = SIM_MEAS_3_DIASTOLIC;
    m_bps_meas_sim_val[2].diastolic.exponent     = 0;
    m_bps_meas_sim_val[2].mean_arterial.mantissa = SIM_MEAS_3_MEAN_AP;
    m_bps_meas_sim_val[2].mean_arterial.exponent = 0;
    m_bps_meas_sim_val[2].pulse_rate.mantissa    = SIM_MEAS_3_PULSE_RATE;
    m_bps_meas_sim_val[2].pulse_rate.exponent    = 0;

    // Simulated measurement #4.
    m_bps_meas_sim_val[3].systolic.mantissa      = SIM_MEAS_4_SYSTOLIC;
    m_bps_meas_sim_val[3].systolic.exponent      = 0;
    m_bps_meas_sim_val[3].diastolic.mantissa     = SIM_MEAS_4_DIASTOLIC;
    m_bps_meas_sim_val[3].diastolic.exponent     = 0;
    m_bps_meas_sim_val[3].mean_arterial.mantissa = SIM_MEAS_4_MEAN_AP;
    m_bps_meas_sim_val[3].mean_arterial.exponent = 0;
    m_bps_meas_sim_val[3].pulse_rate.mantissa    = SIM_MEAS_4_PULSE_RATE;
    m_bps_meas_sim_val[3].pulse_rate.exponent    = 0;
}


/**@brief Function for starting application timers.
 */
static void application_timers_start(void)
{
    ret_code_t err_code;

    // Start application timers.
    err_code = app_timer_start(m_battery_timer_id, BATTERY_LEVEL_MEAS_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the Connection Parameter events.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail configuration parameter, but instead we use the
 *                event handler mechanism to demonstrate its use.
 *
 * @param[in]   p_evt   Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    ret_code_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
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
    ble_conn_params_init_t connection_params_init;

    memset(&connection_params_init, 0, sizeof(connection_params_init));

    connection_params_init.p_conn_params                  = NULL;
    connection_params_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    connection_params_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    connection_params_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    connection_params_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    connection_params_init.disconnect_on_fail             = false;
    connection_params_init.evt_handler                    = on_conn_params_evt;
    connection_params_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&connection_params_init);
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
        case BLE_ADV_EVT_FAST:
            NRF_LOG_INFO("Fast advertising");
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
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
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected");
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected");
            m_conn_handle               = BLE_CONN_HANDLE_INVALID;
            m_bps_meas_ind_conf_pending = false;
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
            err_code = sd_ble_gap_disconnect(m_conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BSP_EVENT_WHITELIST_OFF:
            if (m_conn_handle == BLE_CONN_HANDLE_INVALID)
            {
                err_code = ble_advertising_restart_without_whitelist(&m_advertising);
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }
            }
            break;

        case BSP_EVENT_KEY_0:
            if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
            {
                blood_pressure_measurement_send();
            }
            break;

        default:
            break;
    }
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
    ret_code_t             err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = m_adv_uuids;

    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  = APP_ADV_DURATION;

    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
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
    else{
        ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);

        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for application main entry.
 */
int main(void)
{
    bool     erase_bonds;

    // Initialize.
    log_init();
    timers_init();
    buttons_leds_init(&erase_bonds);
    power_management_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    advertising_init();
    services_init();
    sensor_simulator_init();
    conn_params_init();
    peer_manager_init();

    // Start execution.
    NRF_LOG_INFO("Blood Pressure example started.");
    application_timers_start();

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
