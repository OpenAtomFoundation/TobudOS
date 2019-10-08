/**
 * This software is subject to the ANT+ Shared Source License
 * www.thisisant.com/swlicenses
 * Copyright (c) Garmin Canada Inc. 2013
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
/**@file
 * @defgroup nrf_ant_relay_demo ANT Relay Example
 * @{
 * @ingroup nrf_ant_relay_demo
 *
 * @brief Example of ANT Relay implementation.
 *
 * Before compiling this example for NRF52, complete the following steps:
 * - Download the S212 SoftDevice from <a href="https://www.thisisant.com/developer/components/nrf52832" target="_blank">thisisant.com</a>.
 * - Extract the downloaded zip file and copy the S212 SoftDevice headers to <tt>\<InstallFolder\>/components/softdevice/s212/headers</tt>.
 * If you are using Keil packs, copy the files into a @c headers folder in your example folder.
 * - Make sure that @ref ANT_LICENSE_KEY in @c nrf_sdm.h is uncommented.
 */

#include <string.h>
#include "app_error.h"
#include "app_util.h"
#include "ant_interface.h"
#include "ant_parameters.h"
#include "nrf_soc.h"
#include "nrf.h"
#include "bsp.h"
#include "hardfault.h"
#include "app_timer.h"
#include "nrf_pwr_mgmt.h"
#include "ant_channel_config.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

// Global channel parameters
#define ANT_RELAY_MAIN_PAGE                 ((uint8_t) 1)               /**< Main status page for relay interface channel. */
#define ANT_MOBILE_ANT_OBSERVER_PRIO        1                           /**< ANT observer priority. */
#define ANT_RELAY_MASTER_ANT_OBSERVER_PRIO  1                           /**< Master ANT observer priority. */
#define ANT_RELAY_SLAVE_ANT_OBSERVER_PRIO   1                           /**< Slave ANT observer priority. */

typedef enum{
    ANT_LED_STATE_OFF = 0,
    ANT_LED_STATE_ON  = 1
} ant_led_state_t;

typedef enum
{
    ANT_PAGE_NUMBER_FIELD_OFFSET    = 0,
    ANT_LED_STATUS_FIELD_OFFSET     = 1,
    ANT_COUNTER_FIELD_OFFSET        = 2,
    ANT_STATE_FIELD_OFFSET          = 7
} ant_message_field_offset_t;

typedef enum
{
    ANT_MOBILE_MAIN_PAGE    = 1,    /**< Main status page for mobile interface channel. */
    ANT_MOBILE_COMMAND_PAGE = 2     /**< Command page for mobile interface (from mobile to device). */
} ant_mobile_command_t;

typedef enum
{
    ANT_COMMAND_PAIRING = 1,
    ANT_COMMAND_ON      = 2,
    ANT_COMMAND_OFF     = 3
} ant_state_command_t;

// Static variables and buffers.
static uint32_t  m_led_change_counter = 0;

/**@brief Decode and handle the main relay page.
 *  set the LED if required.
 *
 * @param[in] p_payload ANT message 8-byte payload.
 */
void ant_relay_main_page_handle(uint8_t* p_payload)
{
    uint32_t counter =  uint32_decode(p_payload + ANT_COUNTER_FIELD_OFFSET);

    // If counter changed, set the led to what
    // we received in the message.
    if (counter > m_led_change_counter)
    {
        uint8_t led_state = p_payload[ANT_LED_STATUS_FIELD_OFFSET];
        if (led_state == ANT_LED_STATE_ON)
        {
            bsp_board_led_on(BSP_BOARD_LED_0);
        }
        else
        {
            bsp_board_led_off(BSP_BOARD_LED_0);
        }

        m_led_change_counter = counter;
    }
}

/**@brief Function for creating and sending main
 * relay data page.
 *
 * @param[in] channel ANT channel on which to send this message.
 */
void ant_relay_main_message_assemble(uint8_t channel)
{
    uint8_t status;
    uint32_t err_code = sd_ant_channel_status_get(ANT_RELAY_SLAVE_CHANNEL_NUMBER, &status);
    APP_ERROR_CHECK(err_code);

    uint8_t broadcast_data[ANT_STANDARD_DATA_PAYLOAD_SIZE];
    memset(broadcast_data, 0xFF, sizeof(broadcast_data));
    broadcast_data[ANT_PAGE_NUMBER_FIELD_OFFSET]  = ANT_RELAY_MAIN_PAGE;
    broadcast_data[ANT_LED_STATUS_FIELD_OFFSET]   = ( bsp_board_led_state_get(BSP_BOARD_LED_0) )?
                                                      ANT_LED_STATE_ON : ANT_LED_STATE_OFF;
    broadcast_data[ANT_STATE_FIELD_OFFSET]        = status & STATUS_CHANNEL_STATE_MASK;
    UNUSED_RETURN_VALUE(uint32_encode(m_led_change_counter, broadcast_data + ANT_COUNTER_FIELD_OFFSET));

    err_code = sd_ant_broadcast_message_tx(channel, ANT_STANDARD_DATA_PAYLOAD_SIZE, broadcast_data);
    APP_ERROR_CHECK(err_code);
}

/**@brief Process ANT message on ANT relay master channel
 *
 * @param[in] p_ant_evt  ANT stack event.
 * @param[in] p_context  Context.
 */
void ant_relay_master_process(ant_evt_t * p_ant_evt, void * p_context)
{
    ANT_MESSAGE* p_ant_message = (ANT_MESSAGE*)&p_ant_evt->message;
    switch (p_ant_evt->event)
    {
        case EVENT_RX:
            switch (p_ant_message->ANT_MESSAGE_aucPayload[0])
            {
                case ANT_RELAY_MAIN_PAGE:
                    ant_relay_main_page_handle(p_ant_message->ANT_MESSAGE_aucPayload);
                    break;

            }
            break;

        case EVENT_TX:
            ant_relay_main_message_assemble(ANT_RELAY_MASTER_CHANNEL_NUMBER);
            break;

        default:
            break;

    }
}

NRF_SDH_ANT_OBSERVER(m_master_observer, ANT_RELAY_MASTER_ANT_OBSERVER_PRIO, ant_relay_master_process, NULL);

/**@brief Process ANT message on ANT slave relay channel
 *
 * @param[in] p_ant_evt  ANT stack event.
 * @param[in] p_context  Context.
 */
void ant_relay_slave_process(ant_evt_t * p_ant_evt, void * p_context)
{
    static bool first_recieved = false;
    if (p_ant_evt->channel != ANT_RELAY_SLAVE_CHANNEL_NUMBER)
    {
        return;
    }

    ANT_MESSAGE* p_ant_message = (ANT_MESSAGE*)&p_ant_evt->message;


    switch (p_ant_evt->event)
    {
        case EVENT_RX:
        {
            switch (p_ant_message->ANT_MESSAGE_aucPayload[0])
            {
                case ANT_RELAY_MAIN_PAGE:
                    ant_relay_main_page_handle(p_ant_message->ANT_MESSAGE_aucPayload);
                    break;

            }

            bsp_board_led_on(BSP_BOARD_LED_1);

            if (first_recieved)
            {
                break;
            }
            else
            {
                first_recieved = true;
            }
        }
        // fall-through

        case EVENT_TX:
        {
            ant_relay_main_message_assemble(ANT_RELAY_SLAVE_CHANNEL_NUMBER);
            break;
        }
        case EVENT_RX_SEARCH_TIMEOUT:
        {
            // Channel has closed.
            // Re-initialize proximity search settings.
            uint32_t err_code = sd_ant_prox_search_set(ANT_RELAY_SLAVE_CHANNEL_NUMBER, RELAY_PROXIMITY_BIN, 0);
            APP_ERROR_CHECK(err_code);
            bsp_board_led_off(BSP_BOARD_LED_1);
            break;
        }
        default:
        {
            break;
        }

    }
}

NRF_SDH_ANT_OBSERVER(m_slave_observer, ANT_RELAY_SLAVE_ANT_OBSERVER_PRIO, ant_relay_slave_process, NULL);

/**@brief Process ANT message on ANT mobile interface channel
 *
 * @details   This function handles all events on the mobile interface channel.
 *            On EVENT_TX an ANT_MOBILE_MAIN_PAGE message is queue. The format is:
 *            byte[0]   = page (1 = ANT_MOBILE_MAIN_PAGE)
 *            byte[1]   = led state (1 = 0N, 0 = OFF)
 *            byte[2-6] = reserved (0xFF)
 *            byte[7]   = relay slave channel status (0 = unnassigned, 1 = assigned, 2 = searching, 3 = tracking)
 *
 *            On EVENT_RX the function will decode an ANT_MOBILE_COMMAND_PAGE. The format is:
 *            byte[0]   = page (2 = ANT_MOBILE_COMMAND_PAGE)
 *            byte[1]   = reserved (Set to 0xFF)
 *            byte[2]   = command (1 = pairing, 2 = led on, 3 = led off)
 *            byte[3-7] = reserved (Set to 0xFF)
 *
 * @param[in] p_ant_evt  ANT stack event.
 * @param[in] p_context  Context.
 */
void ant_mobile_process(ant_evt_t * p_ant_evt, void * p_context)
{
    if (p_ant_evt->channel != ANT_MOBILE_CHANNEL_NUMBER)
    {
        return;
    }

    ANT_MESSAGE* p_ant_message = (ANT_MESSAGE*)&p_ant_evt->message;

    switch (p_ant_evt->event)
    {
        case EVENT_RX:
            switch (p_ant_message->ANT_MESSAGE_aucPayload[0])
            {
                case ANT_MOBILE_COMMAND_PAGE:
                    switch (p_ant_message->ANT_MESSAGE_aucPayload[2])
                    {
                        case ANT_COMMAND_ON:
                            bsp_board_led_on(BSP_BOARD_LED_0);
                            m_led_change_counter++;
                            break;

                        case ANT_COMMAND_OFF:
                            bsp_board_led_off(BSP_BOARD_LED_0);
                            m_led_change_counter++;
                            break;

                        case ANT_COMMAND_PAIRING:
                        {
                            uint8_t channel_status;
                            uint32_t err_code = sd_ant_channel_status_get(ANT_RELAY_SLAVE_CHANNEL_NUMBER,
                                                                          &channel_status);
                            APP_ERROR_CHECK(err_code);

                            if ((channel_status & STATUS_CHANNEL_STATE_MASK) == STATUS_ASSIGNED_CHANNEL)
                            {
                                err_code = sd_ant_channel_open(ANT_RELAY_SLAVE_CHANNEL_NUMBER);
                                APP_ERROR_CHECK(err_code);
                            }
                            break;
                        }
                    }
                    break;

                default:
                    break;
            }
            break;

        case EVENT_TX:
        {
            uint8_t status;

            uint32_t err_code = sd_ant_channel_status_get(ANT_RELAY_SLAVE_CHANNEL_NUMBER, &status);
            APP_ERROR_CHECK(err_code);

            uint8_t broadcast_data[ANT_STANDARD_DATA_PAYLOAD_SIZE];
            memset(broadcast_data, 0xFF, sizeof(broadcast_data));
            broadcast_data[0] = ANT_MOBILE_MAIN_PAGE;
            broadcast_data[1] = ( bsp_board_led_state_get(BSP_BOARD_LED_0) )? ANT_LED_STATE_ON : ANT_LED_STATE_OFF;
            broadcast_data[7] = status & STATUS_CHANNEL_STATE_MASK;
            err_code          = sd_ant_broadcast_message_tx(ANT_MOBILE_CHANNEL_NUMBER,
                                                            ANT_STANDARD_DATA_PAYLOAD_SIZE,
                                                            broadcast_data);
            APP_ERROR_CHECK(err_code);

            break;
        }
        default:
            break;

    }
}

NRF_SDH_ANT_OBSERVER(m_mobile_observer, ANT_MOBILE_ANT_OBSERVER_PRIO, ant_mobile_process, NULL);

/**@brief Function for handling bsp events.
 */
void bsp_evt_handler(bsp_event_t evt)
{
    switch (evt)
    {
        case BSP_EVENT_KEY_0:
            // Toggle the state of the LED
            m_led_change_counter++;

            bsp_board_led_invert(BSP_BOARD_LED_0);
            break;

        case BSP_EVENT_KEY_1:
        {
            // Open slave channel
            uint8_t channel_status;
            uint32_t err_code = sd_ant_channel_status_get(ANT_RELAY_SLAVE_CHANNEL_NUMBER,
                                                          &channel_status);
            APP_ERROR_CHECK(err_code);

            if ((channel_status & STATUS_CHANNEL_STATE_MASK) == STATUS_ASSIGNED_CHANNEL)
            {
                err_code = sd_ant_channel_open(ANT_RELAY_SLAVE_CHANNEL_NUMBER);
                APP_ERROR_CHECK(err_code);
            }
            break;
        }

        default:
            return; // no implementation needed
    }
}

/**@brief Function for setting up all things not directly associated with ANT stack/protocol.
 */
static void utils_setup(void)
{
    // Initialize and start a single continuous mode timer, which is used to update the event time
    // on the main data page.
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS,
                                 bsp_evt_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for ANT stack initialization.
 *
 * @details Initializes the SoftDevice and the ANT event interrupt.
 */
static void softdevice_setup(void)
{
    ret_code_t err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    ASSERT(nrf_sdh_is_enabled());

    err_code = nrf_sdh_ant_enable();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for setting up the ANT channels
 *
 */
static void ant_channel_setup(void)
{
    uint32_t err_code;

    // !! CONFIGURE MOBILE CHANNEL !! //

    /* The purpose of the mobile channel is to provide an independent link
       to a mobile platform using ANT. The mobile channel will report the
       status of the LED as well as the status of the slave relay channel
       (closed/open/searching). The mobile channel will also accept commands
       which emulate the function of the development board. For example, to
       set the the set the state of the led or to put the device into pairing
       mode.
    */

    ant_channel_config_t mobile_channel_config =
    {
        .channel_number     = ANT_MOBILE_CHANNEL_NUMBER,
        .channel_type       = CHANNEL_TYPE_MASTER,
        .ext_assign         = 0x00,
        .rf_freq            = MOBILE_RF_FREQ,
        .transmission_type  = MOBILE_CHAN_ID_TRANS_TYPE,
        .device_type        = MOBILE_CHAN_ID_DEV_TYPE,
        .device_number      = (uint16_t) (NRF_FICR->DEVICEID[0]),
        .channel_period     = MOBILE_CHAN_PERIOD,
        .network_number     = ANT_NETWORK_NUM,
    };

    err_code = ant_channel_init(&mobile_channel_config);
    APP_ERROR_CHECK(err_code);

    // Open channel right away.
    err_code = sd_ant_channel_open(ANT_MOBILE_CHANNEL_NUMBER);
    APP_ERROR_CHECK(err_code);

    // !! CONFIGURE RELAY MASTER CHANNEL !! //

    /* The relay master channel is always on and transmits the
       status of the LED and the status of the slave relay channel
       (open/closed/searching). It is 100% bi-directional once
       a slave connects to it (status updates from the slave are
       sent on every message period)
    */

    ant_channel_config_t relay_master_channel_config =
    {
        .channel_number     = ANT_RELAY_MASTER_CHANNEL_NUMBER,
        .channel_type       = CHANNEL_TYPE_MASTER,
        .ext_assign         = 0x00,
        .rf_freq            = RELAY_RF_FREQ,
        .transmission_type  = RELAY_CHAN_ID_TRANS_TYPE,
        .device_type        = RELAY_CHAN_ID_DEV_TYPE,
        .device_number      = (uint16_t) (NRF_FICR->DEVICEID[0]),
        .channel_period     = RELAY_CHAN_PERIOD,
        .network_number     = ANT_NETWORK_NUM,
    };

    err_code = ant_channel_init(&relay_master_channel_config);
    APP_ERROR_CHECK(err_code);

    // Open channel right away.
    err_code = sd_ant_channel_open(ANT_RELAY_MASTER_CHANNEL_NUMBER);
    APP_ERROR_CHECK(err_code);

    // !! CONFIGURE RELAY SLAVE CHANNEL !! //

    /* The purpose of the relay slave channel is to find and synchronize
       to another devices master really channel. The slave channel is only
       opened on a button press and uses proximity pairing to connect to a
       master channel. Once tracking a master the slave channel will send status
       message back to the master 100% of the time.
    */

    ant_channel_config_t relay_slave_channel_config =
    {
        .channel_number     = ANT_RELAY_SLAVE_CHANNEL_NUMBER,
        .channel_type       = CHANNEL_TYPE_SLAVE,
        .ext_assign         = 0x00,
        .rf_freq            = RELAY_RF_FREQ,
        .transmission_type  = RELAY_CHAN_ID_TRANS_TYPE,
        .device_type        = RELAY_CHAN_ID_DEV_TYPE,
        .device_number      = 0x00,                     // Wildcard
        .channel_period     = RELAY_CHAN_PERIOD,
        .network_number     = ANT_NETWORK_NUM,
    };

    err_code = ant_channel_init(&relay_slave_channel_config);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ant_prox_search_set(ANT_RELAY_SLAVE_CHANNEL_NUMBER, RELAY_PROXIMITY_BIN, 0);
    APP_ERROR_CHECK(err_code);

    // DO NOT OPEN THE SLAVE RIGHT AWAY - IT OPENS ON BUTTON PRESS
    // OR MESSAGE FROM MOBILE PHONE
}


/**@brief Function for initializing logging.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for application main entry. Does not return.
 */
int main(void)
{
    log_init();
    utils_setup();
    softdevice_setup();
    ant_channel_setup();

    NRF_LOG_INFO("ANT Relay example started.");

    // Main loop.
    for (;;)
    {
        NRF_LOG_FLUSH();
        nrf_pwr_mgmt_run();
    }
}
