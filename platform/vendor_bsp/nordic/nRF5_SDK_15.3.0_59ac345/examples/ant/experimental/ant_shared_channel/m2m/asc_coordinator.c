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
#include <stdint.h>
#include <string.h>
#include "asc_coordinator.h"
#include "asc_events.h"
#include "asc_parameters.h"
#include "asc_master_to_master.h"
#include "asc_master.h"
#include "ant_phone_connection.h"
#include "ant_interface.h"
#include "ant_parameters.h"
#include "app_error.h"
#include "leds.h"
#include "boards.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_ant.h"

#ifdef BLE_STACK_SUPPORT_REQD
    #include "ble_controllable_hub.h"
    #include "ble_phone_connection.h"
#endif

#if defined(BOARD_N5DK1)
    #define LED_CONNECT_STATUS   LED_B
#else
    #define LED_CONNECT_STATUS   BSP_BOARD_LED_1
#endif


//Generic Channel properties
#define ANT_PUBLIC_NETWORK_NUMBER       0x00u                       /**< ANT Public/Default Network Key. */
#define ANT_CUSTOM_TRANSMIT_POWER       0u                          /**< ANT Custom Transmit Power (Invalid/Not Used). */
#define SERIAL_NUMBER_ADDRESS           ((uint32_t) 0x10000060)     // FICR + 60
#define DEVICE_NUMBER                   (*(uint16_t*) SERIAL_NUMBER_ADDRESS)

// ASCM Channel configuration.
#define ASCM_CHANNEL                    0x00u                       /**< ASC Master channel. */
#define ASCM_RF_FREQUENCY               0x42u                       /**< 2400 + 66Mhz. */
#define ASCM_CHANNEL_PERIOD             MSG_PERIOD_4HZ              /**< ASC Master channel period. */
#define ASCM_CHANNEL_TYPE               CHANNEL_TYPE_MASTER         /**< ASC Master channel type. */
#ifdef TWO_BYTE_SHARED_ADDRESS
    #define ASCM_TRANS_TYPE             ANT_TRANS_TYPE_2_BYTE_SHARED_ADDRESS    /**< Two byte shared address transmission type. */
#else
    #define ASCM_TRANS_TYPE             ANT_TRANS_TYPE_1_BYTE_SHARED_ADDRESS    /**< One byte shared address transmission type. */
#endif
#define ASCM_DEVICE_TYPE                0x02u                       /**< Device type. */
#define DEFAULT_RETRIES                 2u                          /**< The default number of retries that this demo will tell an ASC Master to attempt when sending a command to all slaves. */

// Phone Channel configuration.
#define PHONE_CHANNEL                   0x01u                       /**< Phone connection channel. */
#define PHONE_RF_FREQUENCY              0x42u                       /**< Phone connection rf frequency offset. */
#define PHONE_CHANNEL_PERIOD            MSG_PERIOD_4HZ              /**< Phone connection channel period. */
#define PHONE_CHANNEL_TYPE              CHANNEL_TYPE_MASTER         /**< Phone connection channel type. */
#define PHONE_TRANS_TYPE                0x05u                       /**< Phone connection transmission type. */
#define PHONE_DEVICE_TYPE               0x03u                       /**< Phone connection device type. */

// Master to Master Channel configuration.
#define ASCMM_DISCOVERY_CHANNEL         0x02u                       /**< Master to Master device discovery channel. */
#define ASCMM_CHANNEL                   0x03u                       /**< Master to Master channel. */
#define ASCMM_RF_FREQUENCY              0x42u                       /**< Master to Master rf frequency offset (same as PHONE_RF_FREQUENCY since the phone connection channel is used for discovery). */
#define ASCMM_CHANNEL_PERIOD            MSG_PERIOD_4HZ              /**< Master to Master channel period (same as PHONE_CHANNEL_PERIDO since the phone connection channel is used for discovery). */
#define ASCMM_CHANNEL_TYPE              CHANNEL_TYPE_SLAVE          /**< Master to Master channel type. */
#define ASCMM_TRANS_TYPE                0x05u                       /**< Master to Master  transmission type (same as PHONE_TRANS_TYPE since the phone connection channel is used for discovery). */
#define ASCMM_DEVICE_TYPE               0x04u                       /**< Master to Master device type (same as PHONE_DEVICE_TYPE since the phone connection channel is used for discovery). */
#define ASCMM_RETRIES                   0x02                        /**< Number of messages retries that will be sent when relaying messages. */


#define APP_BLE_CONN_CFG_TAG            1                           /**< A tag that refers to the BLE stack configuration we set with @ref sd_ble_cfg_set. Default tag is @ref BLE_CONN_CFG_TAG_DEFAULT. */
#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                           /**< Whether or not to include the service_changed characteristic. If not enabled, the server's database cannot be changed for the lifetime of the device */

// Static variables and buffers.
static uint16_t         m_neighbor_id = INVALID_NEIGHBOUR_ID;
static bool             m_is_reporting_mode_on = true;
static uint8_t          m_tx_buffer[ANT_STANDARD_DATA_PAYLOAD_SIZE] = {0};

const asc_ant_params_t  m_asc_parameters = {
    ASCM_CHANNEL,
    ANT_PUBLIC_NETWORK_NUMBER,
    (uint16_t*) SERIAL_NUMBER_ADDRESS,
    ASCM_DEVICE_TYPE,
    ASCM_TRANS_TYPE,
    ASCM_CHANNEL_PERIOD,
    ASCM_CHANNEL_TYPE,
    ASCM_RF_FREQUENCY,
    RADIO_TX_POWER_LVL_3
}; /**< Structure containing setup parameters for the auto shared master. */

const asc_ant_params_t  m_ascmm_discovery_parameters = {
    ASCMM_DISCOVERY_CHANNEL,
    ANT_PUBLIC_NETWORK_NUMBER,
    (uint16_t*) SERIAL_NUMBER_ADDRESS,
    PHONE_DEVICE_TYPE,
    PHONE_TRANS_TYPE,
    PHONE_CHANNEL_PERIOD,
    CHANNEL_TYPE_SLAVE,
    PHONE_RF_FREQUENCY,
    RADIO_TX_POWER_LVL_3
}; /**< Structure containing setup parameters for the discovery portion of the master-to-master connection.
        In this case, it searches for the phone connection channel. */

const asc_ant_params_t  m_ascmm_connection_parameters = {
    ASCMM_CHANNEL,
    ANT_PUBLIC_NETWORK_NUMBER,
    (uint16_t*) SERIAL_NUMBER_ADDRESS,
    ASCMM_DEVICE_TYPE,
    ASCMM_TRANS_TYPE,
    ASCMM_CHANNEL_PERIOD,
    ASCMM_CHANNEL_TYPE,
    ASCMM_RF_FREQUENCY,
    RADIO_TX_POWER_LVL_3
}; /**< Structure containing setup parameters for the final master-to-master connection. */

const asc_ant_params_t  m_phone_parameters = {
    PHONE_CHANNEL,
    ANT_PUBLIC_NETWORK_NUMBER,
    (uint16_t*) SERIAL_NUMBER_ADDRESS,
    PHONE_DEVICE_TYPE,
    PHONE_TRANS_TYPE,
    PHONE_CHANNEL_PERIOD,
    PHONE_CHANNEL_TYPE,
    PHONE_RF_FREQUENCY,
    RADIO_TX_POWER_LVL_3
}; /**< Structure containing setup parameters for the phone connection. */



#if defined(BLE_STACK_SUPPORT_REQD) && defined (TWO_BYTE_SHARED_ADDRESS)
#error "BLE Demo is incompatible with two byte shared addressing."
#endif


// Private Functions

/**@brief  Function for checking event flags and handling them.
 */
void check_and_handle_ascm_flags(void)
{
    uint32_t asc_flags = ascm_events_get();
    uint32_t registry_flags = ascm_get_device_registry_events();

    do
    {
        //service asc master events
        if (asc_flags & EVENT_ASC_STATE_CHANGED)
        {
            ascm_event_clear(EVENT_ASC_STATE_CHANGED);

            //determine which state the master has moved into and handle it
            ascm_states_t state = ascm_state_get();
            switch (state)
            {
                case SENDING_COMMAND:
                case ASCM_OFF:
                case ADDRESS_AVAILABLE:
                case HANDSHAKING:
                case POLLING:
                default:
                    break;
            }
        }

        if (asc_flags & EVENT_ASC_DEVICE_IN_WRONG_STATE)
        {
            ascm_event_clear(EVENT_ASC_DEVICE_IN_WRONG_STATE);
            //Do Nothing
        }

        if (asc_flags & EVENT_ASC_UPDATE_RECEIVED)
        {
            ascm_event_clear(EVENT_ASC_UPDATE_RECEIVED);

            if (m_is_reporting_mode_on)
            {
                //extract last update and relay the update accross the master to master channel
                asc_update_data_t update = ascm_get_last_recevied_update();
                update.master_id = DEVICE_NUMBER;
                asc_encode_phone_update_data_page(update, m_tx_buffer);
                ascmm_relay_message(m_tx_buffer, DEFAULT_RETRIES);

                //also send this update across the phone channel
                phc_transmit_message(m_tx_buffer, DEFAULT_RETRIES);
#ifdef BLE_STACK_SUPPORT_REQD
                cntrldevice_process_update(update);
#endif
            }
        }

        //service device registry events
        if (registry_flags & DEVICEREGISTRY_EVENT_DEVICE_ADDED)
        {
            ascm_clear_device_registry_event(DEVICEREGISTRY_EVENT_DEVICE_ADDED);
        }

        if (registry_flags & DEVICEREGISTRY_EVENT_DEVICE_REMOVED)
        {
            ascm_clear_device_registry_event(DEVICEREGISTRY_EVENT_DEVICE_REMOVED);
        }

        //update the local events bitfields
        asc_flags = ascm_events_get();
        registry_flags = ascm_get_device_registry_events();
    }
    while (asc_flags != 0 && registry_flags !=0);

}


void check_and_handle_ascmm_flags(void)
{
    uint32_t ascmm_flags = ascmm_events_get();

    do
    {
        //service master to master events
        if (ascmm_flags & EVENT_ASC_STATE_CHANGED)
        {
            ascmm_event_clear(EVENT_ASC_STATE_CHANGED);
            m_neighbor_id = ascmm_get_neighbor_id();
            phc_set_neighbor_id(m_neighbor_id);

            ascmm_states_t state = ascmm_state_get();
            switch (state)
            {
                case CONNECTED:
                {
                    led_on(LED_CONNECT_STATUS);
                    break;
                }
                default:
                {
                    led_off(LED_CONNECT_STATUS);
                    break;
                }
            }
        }

        if (ascmm_flags & EVENT_ASC_DEVICE_IN_WRONG_STATE)
        {
            ascmm_event_clear(EVENT_ASC_DEVICE_IN_WRONG_STATE);
            //Do nothing
        }

        if (ascmm_flags & EVENT_ASC_COMMAND_RECEIVED)
        {
            ascmm_event_clear(EVENT_ASC_COMMAND_RECEIVED);

            asc_command_data_t command_data =  ascmm_get_last_command();

            switch (command_data.command)
            {
                case REPORTING_MODE_OFF:
                {
                    m_is_reporting_mode_on = false;
                    break;
                }
                case REPORTING_MODE_ON:
                {
                    m_is_reporting_mode_on = true;
                    break;
                }
                default:
                {
//lint --e{534}
                    ascm_send_command(command_data, DEFAULT_RETRIES);
                    break;
                }
            }
        }

        if (ascmm_flags & EVENT_ASC_UPDATE_RECEIVED)
        {
            ascmm_event_clear(EVENT_ASC_UPDATE_RECEIVED);

            if (m_is_reporting_mode_on)
            {
                //Get the received update and relay it to the phone
                asc_update_data_t update = ascmm_get_last_update();
                asc_encode_phone_update_data_page(update, m_tx_buffer);
                phc_transmit_message(m_tx_buffer, DEFAULT_RETRIES);
#ifdef BLE_STACK_SUPPORT_REQD
                cntrldevice_process_update(update);
#endif
            }
        }

        //update the local events bitfields
        ascmm_flags = ascmm_events_get();
    }
    while (ascmm_flags != 0);
}


void handle_received_command(asc_command_data_t command_data)
{
    switch (command_data.command)
    {
        case REPORTING_MODE_OFF:
        {
            m_is_reporting_mode_on = false;
            asc_encode_phone_command_page(command_data, m_tx_buffer);
            ascmm_relay_message(m_tx_buffer, ASCMM_RETRIES);
            break;
        }
        case REPORTING_MODE_ON:
        {
            m_is_reporting_mode_on = true;
            asc_encode_phone_command_page(command_data, m_tx_buffer);
            ascmm_relay_message(m_tx_buffer, ASCMM_RETRIES);
            break;
        }
        default:
        {
            if (command_data.master_id == DEVICE_NUMBER)
            {
//lint --e{534}
                ascm_send_command(command_data, DEFAULT_RETRIES);
            }
            else if (command_data.master_id == m_neighbor_id && m_neighbor_id != INVALID_NEIGHBOUR_ID)
            {
                asc_encode_phone_command_page(command_data, m_tx_buffer);
                ascmm_relay_message(m_tx_buffer, ASCMM_RETRIES);
            }
            else if (command_data.master_id == EVERY_MASTER_ID)
            {
//lint --e{534}
                ascm_send_command(command_data, DEFAULT_RETRIES);
                asc_encode_phone_command_page(command_data, m_tx_buffer);
                ascmm_relay_message(m_tx_buffer, ASCMM_RETRIES);
            }
            break;
        }
    }
}


void check_and_handle_phc_flags(void)
{
    uint32_t phone_connection_flags = phc_events_get();

    do
    {
        //service phone connection events
        if (phone_connection_flags & EVENT_ASC_COMMAND_RECEIVED)
        {
            phc_event_clear(EVENT_ASC_COMMAND_RECEIVED);
            asc_command_data_t command_data = phc_get_last_command();

            handle_received_command(command_data);
        }

        phone_connection_flags = phc_events_get();
    }
    while (phone_connection_flags != 0);
}


#ifdef BLE_STACK_SUPPORT_REQD
void check_and_handle_ble_flags(void)
{
    uint32_t ble_flags = ble_achs_events_get();

    do
    {
        //service phone connection events
        if (ble_flags & EVENT_ASC_COMMAND_RECEIVED)
        {
            ble_achs_event_clear(EVENT_ASC_COMMAND_RECEIVED);
            asc_command_data_t command_data = ble_achs_get_last_command();

            handle_received_command(command_data);
        }

        ble_flags = ble_achs_events_get();
    }
    while (ble_flags != 0);
}


static void ble_evt_dispatch(ble_evt_t const * p_ble_evt, void * p_context)
{
    cntrldevice_event_handler(p_ble_evt);
}


/**@brief Function for handling a ANT stack event.
 *
 * @param[in] p_ant_evt  ANT stack event.
 * @param[in] p_context  Context.
 */
void ant_evt_handler(ant_evt_t * p_ant_evt, void * p_context)
{
    switch (p_ant_evt->channel)
    {
        case ASCM_CHANNEL:
        {
            ascm_handle_ant_event(p_ant_evt->event, p_ant_evt->message.aucMessage);
            break;
        }
        case ASCMM_DISCOVERY_CHANNEL:
            //intentional fallthrough
        case ASCMM_CHANNEL:
        {
            ascmm_handle_ant_event(p_ant_evt->event, p_ant_evt->message.aucMessage);
            break;
        }
        case PHONE_CHANNEL:
        {
            phc_handle_ant_event(p_ant_evt->event, p_ant_evt->message.aucMessage);
            break;
        }
        default:
        {
            break;
        }
    }

    check_and_handle_ascm_flags();
    check_and_handle_ascmm_flags();
    check_and_handle_phc_flags();

#ifdef BLE_STACK_SUPPORT_REQD
    check_and_handle_ble_flags();
#endif
}


/**@brief BLE stack initialization.
 *
 * @details Initializes the SoftDevice for BLE stack.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;
    uint32_t   ram_start = 0;

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_dispatch, NULL);
}
#endif


/**@brief BLE + ANT stack initialization.
 *
 * @details Initializes the SoftDevice and the stack event interrupt.
 */
static void softdevice_setup(void)
{
    ret_code_t err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    ASSERT(nrf_sdh_is_enabled());

#ifdef BLE_STACK_SUPPORT_REQD
    ble_stack_init();
#endif

    err_code = nrf_sdh_ant_enable();
    APP_ERROR_CHECK(err_code);

    // Register a handler for ANT events.
    NRF_SDH_ANT_OBSERVER(m_ant_observer, APP_ANT_OBSERVER_PRIO, ant_evt_handler, NULL);
}


// Public Functions

void ascc_init(void)
{
    softdevice_setup();

#ifdef BLE_STACK_SUPPORT_REQD
    // Initialize the ble phone connection.
    cntrldevice_initialize();
#endif

    // Initialise and start the asc, ascmm, and phone modules.
    ascm_init(&m_asc_parameters);
    ascmm_init(&m_ascmm_discovery_parameters, &m_ascmm_connection_parameters, DEVICE_NUMBER);
    phc_init(&m_phone_parameters);

    ascm_turn_on();
    ascmm_turn_on();
    phc_turn_on();
}

