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
#ifndef ES_APP_CONFIG_H__
#define ES_APP_CONFIG_H__

#include "es.h"
#include "boards.h"

/**
 * @file
 * @defgroup eddystone_app_config Application configuration
 * @brief Configuration settings for the application.
 * @details These configuration settings are defined in the application.
 * You can find the configuration file at
 * <tt>examples\ble_peripheral\ble_app_eddystone\es_app_config.h</tt>.
 * @ingroup eddystone
 * @{
 */

// EID security
#define MAC_RANDOMIZED //!< Configuration option to specify whether the BLE address should be randomized when advertising EIDs.

#define APP_CONFIG_LOCK_CODE {0xFF, 0xFF, 0xFF, 0xFF, \
                              0xFF, 0xFF, 0xFF, 0xFF, \
                              0xFF, 0xFF, 0xFF, 0xFF, \
                              0xFF, 0xFF, 0xFF, 0xFF} //!< Beacon lock code. @warning This lock code must be changed before going to production.

#ifdef NRF52_SERIES
#define APP_CONFIG_CALIBRATED_RANGING_DATA {-49, -39, -29, -24, -19, -14, -9, -7, -5}   //!< Calibrated TX power at 0 m. See the nRF52 Product Specification for corresponding TX values.
#elif NRF51
#define APP_CONFIG_CALIBRATED_RANGING_DATA {-39, -26, -23, -18, -13, -12, -9, -2}       //!< Calibrated TX power at 0 m. See the nRF51 Product Specification for corresponding TX values.
#else
#error MISSING CALIBRATED DATA
#endif

#define APP_CONFIG_TLM_TEMP_VBATT_UPDATE_INTERVAL_SECONDS   10                          //!< How often should the data in the TLM frame be updated.
#define APP_CONFIG_TLM_ADV_INTERLEAVE_RATIO                 5                           //!< How often should the TLM frame be advertised.


#define APP_CONFIG_ADV_INTERVAL_MS_MAX                      16384                       //!< Maximum allowed advertisement interval. Can be undefined without int
#define APP_CONFIG_ADV_FRAME_SPACING_MS_MIN                 500                         //!< Minimum time between advertisement frames. Imposes limit on minumum accepted advertisement interval.
#ifdef NRF52_SERIES
#define APP_CONFIG_ADV_FRAME_ETLM_SPACING_MS                300                         //!< The time that is reqired for preparing an eTLM slot. Imposes limit on minimum accepted advertisement interval.
#else
#error MISSING ETLM DELAY TIMING
#endif

#define ES_STOPWATCH_MAX_USERS                              4                           //!< Maximum amount of users that can be registered with the es_stopwatch module.


#define DEBUG_TIMING_INIT_VALUE                             65280                       //!< Initial time (as recommended by Google) to test the TK rollover behavior.
#define APP_CONFIG_TIMING_INIT_VALUE                        DEBUG_TIMING_INIT_VALUE     //!< Initializing value for the timing value of security slots.

#define APP_CONFIG_TLM_TEMP_INTERVAL_SECONDS                (30)                        //!< How often should the temperature of the beacon be updated when TLM slot is configured.
#define APP_CONFIG_TLM_VBATT_INTERVAL_SECONDS               (30)                        //!< How often should the battery voltage of the beacon be updated when TLM slot is configured.

// HW CONFIGS
#define BUTTON_REGISTRATION                                 BUTTON_1                    //!< Button to push when putting the beacon in registration mode.
#define USE_ECB_ENCRYPT_HW                                  1                           //!< Configuration option to use the hardware peripheral (1) or the software library (0) for ECB encryption (decryption always uses the software library).


// BLE CONFIGS
#define APP_DEVICE_NAME                                     "nRF5x_Eddystone"           //!< Advertised device name in the scan response when in connectable mode.
#define IS_SRVC_CHANGED_CHARACT_PRESENT                     0                           //!< Information whether the service changed characteristic is available. If it is not enabled, the server's database cannot be changed for the lifetime of the device.
#define MAX_ADV_INTERVAL                                    (10240)                     //!< Maximum connection interval (in ms).
#define MIN_CONN_ADV_INTERVAL                               (20)                        //!< Minimum connection interval (in ms).
#define MIN_NON_CONN_ADV_INTERVAL                           (100)                       //!< Minimum advertisement interval for non-connectable advertisements (in ms).

#define CENTRAL_LINK_COUNT                                  0                           //!< Number of central links used by the application. When changing this number, remember to adjust the RAM settings.
#define PERIPHERAL_LINK_COUNT                               1                           //!< Number of peripheral links used by the application. When changing this number, remember to adjust the RAM settings.

#define APP_CFG_NON_CONN_ADV_TIMEOUT                        0                           //!< Time for which the device must be advertising in non-connectable mode (in seconds). 0 disables the time-out.
#define APP_CFG_NON_CONN_ADV_INTERVAL_MS                    1000                        //!< The advertising interval for non-connectable advertisement (in milliseconds). This value can vary between 100 ms and 10.24 s.
#define APP_CFG_CONNECTABLE_ADV_TIMEOUT                     6000                        //!< Time for which the device must be advertising in connectable mode (in milliseconds). 0 disables the time-out.
#define APP_CFG_CONNECTABLE_ADV_INTERVAL_MS                 100                         //!< The advertising interval for connectable advertisement (in milliseconds). This value can vary between 20 ms and 10.24 s.

#define APP_CFG_DEFAULT_RADIO_TX_POWER                      0x00                        //!< Default TX power of the radio.

#define MIN_CONN_INTERVAL                   MSEC_TO_UNITS(50, UNIT_1_25_MS)             //!< Minimum acceptable connection interval (20 ms). The connection interval uses 1.25 ms units.
#define MAX_CONN_INTERVAL                   MSEC_TO_UNITS(90, UNIT_1_25_MS)             //!< Maximum acceptable connection interval (75 ms). The connection interval uses 1.25 ms units.
#define SLAVE_LATENCY                       0                                           //!< Slave latency.
#define CONN_SUP_TIMEOUT                    MSEC_TO_UNITS(4000, UNIT_10_MS)             //!< Connection supervision time-out (4 seconds). The supervision time-out uses 10 ms units.
#define FIRST_CONN_PARAMS_UPDATE_DELAY      APP_TIMER_TICKS(5000)                       //!< Time from initiating an event (connection or start of notification) to the first time @ref sd_ble_gap_conn_param_update is called (5 seconds).
#define NEXT_CONN_PARAMS_UPDATE_DELAY       APP_TIMER_TICKS(30000)                      //!< Time between each call to @ref sd_ble_gap_conn_param_update after the first call (30 seconds).
#define MAX_CONN_PARAMS_UPDATE_COUNT        3                                           //!< Number of attempts before giving up the connection parameter negotiation.


// ES CONFIGS
#define APP_MAX_ADV_SLOTS                   5                                           //!< Maximum number of advertisement slots.
#define APP_MAX_EID_SLOTS                   APP_MAX_ADV_SLOTS                           /**< @brief Maximum number of EID slots.
                                                                                         * @note The maximum number of EID slots must be equal to the maximum number of advertisement slots (@ref APP_MAX_ADV_SLOTS). If your application
                                                                                         * does not adhere to this convention, you must modify the @ref eddystone_security module, because the security module maps the security slots'
                                                                                         * slot numbers 1 to 1 to the slots'. */

// Broadcast Capabilities
#define APP_IS_VARIABLE_ADV_SUPPORTED       ESCS_BROADCAST_VAR_ADV_SUPPORTED_No         //!< Information whether variable advertisement intervals are supported.
#define APP_IS_VARIABLE_TX_POWER_SUPPORTED  ESCS_BROADCAST_VAR_TX_POWER_SUPPORTED_Yes   //!< Information whether variable advertisement TX power is supported.

#define APP_IS_UID_SUPPORTED                ESCS_FRAME_TYPE_UID_SUPPORTED_Yes           //!< Information whether the UID frame is supported.
#define APP_IS_URL_SUPPORTED                ESCS_FRAME_TYPE_URL_SUPPORTED_Yes           //!< Information whether the URL frame is supported.
#define APP_IS_TLM_SUPPORTED                ESCS_FRAME_TYPE_TLM_SUPPORTED_Yes           //!< Information whether the TLM frame is supported.
#define APP_IS_EID_SUPPORTED                ESCS_FRAME_TYPE_EID_SUPPORTED_Yes           //!< Information whether the EID frame is supported.

// Remain connectable
#define APP_IS_REMAIN_CONNECTABLE_SUPPORTED ESCS_FUNCT_REMAIN_CONNECTABLE_SUPPORTED_Yes //!< Information whether the 'remain connectable' option is supported.

// Eddystone common data
#define APP_ES_UUID                         0xFEAA                                      //!< UUID for Eddystone beacons according to specification.

// Eddystone UID data
#define APP_ES_UID_FRAME_TYPE               ES_FRAME_TYPE_UID                           //!< UID frame type (fixed at 0x00).
#define APP_ES_UID_NAMESPACE                0xAA, 0xAA, 0xBB, 0xBB, \
                                            0xCC, 0xCC, 0xDD, 0xDD, \
                                            0xEE, 0xEE                                  //!< Mock values for 10-byte Eddystone UID ID namespace.
#define APP_ES_UID_ID                       0x01, 0x02, 0x03, 0x04, \
                                            0x05, 0x06                                  //!< Mock values for 6-byte Eddystone UID ID instance.
#define APP_ES_UID_RFU                      0x00, 0x00                                  //!< Reserved for future use according to specification.

// Eddystone URL data
#define APP_ES_URL_FRAME_TYPE               ES_FRAME_TYPE_URL                           //!< URL Frame type (fixed at 0x10).
#define APP_ES_URL_SCHEME                   0x01                                        //!< URL prefix scheme according to specification (0x01 = "https://www").
#define APP_ES_URL_URL                      0x6e, 0x6f, 0x72, 0x64, \
                                            0x69, 0x63, 0x73, 0x65, \
                                            0x6d,0x69, 0x00                             //!< "nordicsemi.com". Last byte suffix 0x00 = ".com" according to specification.

#define DEFAULT_FRAME_TYPE                  APP_ES_URL_FRAME_TYPE                       //!< Frame type of default frame.
#define DEFAULT_FRAME_TX_POWER              0x00                                        //!< Default frame TX power.

/** @brief This value should mimic the data that would be written to the RW ADV Slot characteristic (for example, no RSSI for UID). */
#define DEFAULT_FRAME_DATA                  {DEFAULT_FRAME_TYPE, DEFAULT_FRAME_TX_POWER, APP_ES_URL_SCHEME, APP_ES_URL_URL}
#define DEFAULT_FRAME_LENGTH                14                                          //!< 1 - Frame Type, 1 - TX - power 1 - URL Scheme, URL - 11 = 14

// SCHEDULER CONFIGS
#define SCHED_MAX_EVENT_DATA_SIZE           APP_TIMER_SCHED_EVENT_DATA_SIZE             //!< Maximum size of the scheduler event data.
#define SCHED_QUEUE_SIZE                    10                                          //!< Size of the scheduler queue.


/**
 * @}
 */

#endif // ES_APP_CONFIG_H__
