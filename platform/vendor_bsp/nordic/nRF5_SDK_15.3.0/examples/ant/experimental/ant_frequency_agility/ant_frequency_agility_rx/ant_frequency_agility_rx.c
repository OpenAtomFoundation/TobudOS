/**
 * This software is subject to the ANT+ Shared Source License
 * www.thisisant.com/swlicenses
 * Copyright (c) Garmin Canada Inc. 2015
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

#include "ant_frequency_agility_rx.h"
#include <stdint.h>
#include "string.h"
#include "sdk_config.h"
#include "bsp.h"
#include "nrf_soc.h"
#include "ant_interface.h"
#include "ant_parameters.h"
#include "ant_channel_config.h"
#include "ant_search_config.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"
#include "app_error.h"
#include "ant_error.h"

//ANT Channels
#define ANT_CHANNEL_NUMBER              0x00                    /**< ANT Channel 0. */
#define EXT_ASSIGN                      EXT_PARAM_FREQUENCY_AGILITY   /**< ANT Ext Assign. */
#define ANT_CHANNEL_DEFAULT_NETWORK     0x00                    /**< ANT Network (default public network). */

// Data Page Numbers
#define DIGITALIO_DATA_PID              1u                      /**< Page number: digital data. */

// Static variables and buffers.
static uint8_t m_broadcast_data[ANT_STANDARD_DATA_PAYLOAD_SIZE];    /**< Primary data transmit buffer. */
static uint8_t m_rx_input_pin_state = 0xFF;                         /**< State of received digital data, from the other node. */
static uint8_t m_tx_input_pin_state = 0xFF;                         /**< State of digital inputs in this node, for transmission. */


/**@brief Encode current state of buttons
 *
 * Configure bitfield encoding the state of the buttons
 * Bit 0 = 0 if button 0 is pressed
 * Bit 1 = 0 if button 1 is pressed
 * Bit 2 = 0 if button 2 is pressed
 * Bit 3 = 0 if button 3 is pressed
 * Bit 4-7 = 1 (unused)
 */
static void encode_button_state(void)
{
    m_tx_input_pin_state = 0xFF;

    if (bsp_button_is_pressed(0))
    {
        m_tx_input_pin_state &= 0xFE;
    }

    if (bsp_button_is_pressed(1))
    {
        m_tx_input_pin_state &= 0xFD;
    }

    if (bsp_button_is_pressed(2))
    {
        m_tx_input_pin_state &= 0xFB;
    }

    if (bsp_button_is_pressed(3))
    {
        m_tx_input_pin_state &= 0xF7;
    }
}


/**@brief Formats page with current button state and sends data
 * Byte 0   = Page number (Digital I/O Data)
 * Byte 1-6 = Reserved
 * Byte 7   = State of digital inputs
 */
static void handle_transmit()
{
    uint32_t err_code;

    encode_button_state();

    m_broadcast_data[0] = DIGITALIO_DATA_PID;
    m_broadcast_data[1] = 0xFF;
    m_broadcast_data[2] = 0xFF;
    m_broadcast_data[3] = 0xFF;
    m_broadcast_data[4] = 0xFF;
    m_broadcast_data[5] = 0xFF;
    m_broadcast_data[6] = 0xFF;
    m_broadcast_data[7] = m_tx_input_pin_state;

    err_code = sd_ant_broadcast_message_tx(ANT_CHANNEL_NUMBER, ANT_STANDARD_DATA_PAYLOAD_SIZE, m_broadcast_data);
    APP_ERROR_CHECK(err_code);
}

/**@brief Turns on LEDs according to the contents of the received data page.
 */
static void set_led_state()
{
    uint8_t led_state_field = ~m_rx_input_pin_state;

    if (led_state_field & 1)
        bsp_board_led_on(BSP_BOARD_LED_0);
    else
        bsp_board_led_off(BSP_BOARD_LED_0);

    if (led_state_field & 2)
        bsp_board_led_on(BSP_BOARD_LED_1);
    else
        bsp_board_led_off(BSP_BOARD_LED_1);

    if (led_state_field & 4)
        bsp_board_led_on(BSP_BOARD_LED_2);
    else
        bsp_board_led_off(BSP_BOARD_LED_2);

    if (led_state_field & 8)
        bsp_board_led_on(BSP_BOARD_LED_3);
    else
        bsp_board_led_off(BSP_BOARD_LED_3);

}


void ant_freq_ag_setup(void)
{
    uint32_t err_code;

    ant_channel_config_t channel_config =
    {
        .channel_number     = ANT_CHANNEL_NUMBER,
        .channel_type       = CHANNEL_TYPE_SLAVE,
        .ext_assign         = EXT_ASSIGN,
        .rf_freq            = RF_FREQUENCY_A,
        .transmission_type  = CHAN_ID_TRANS_TYPE,
        .device_type        = CHAN_ID_DEV_TYPE,
        .device_number      = CHAN_ID_DEV_NUM,
        .channel_period     = CHAN_PERIOD,
        .network_number     = ANT_CHANNEL_DEFAULT_NETWORK,
    };


    ant_search_config_t search_config  = DEFAULT_ANT_SEARCH_CONFIG(ANT_CHANNEL_NUMBER);
    search_config.high_priority_timeout = 12;
    search_config.low_priority_timeout = ANT_LOW_PRIORITY_SEARCH_DISABLE;

    // Configure channel parameters
    err_code = ant_channel_init(&channel_config);
    APP_ERROR_CHECK(err_code);

    // Configure search
    err_code = ant_search_init(&search_config);
    APP_ERROR_CHECK(err_code);

    // Configure the frequency agility hop table.
    err_code = sd_ant_auto_freq_hop_table_set(ANT_CHANNEL_NUMBER, RF_FREQUENCY_A, RF_FREQUENCY_B, RF_FREQUENCY_C);
    APP_ERROR_CHECK(err_code);

    // Open channel.
    err_code = sd_ant_channel_open(ANT_CHANNEL_NUMBER);
    APP_ERROR_CHECK(err_code);
}


static void ant_freq_ag_event_handler(ant_evt_t * p_ant_evt, void * p_context)
{
    uint32_t err_code;
    uint8_t channel_status = 0;
    ANT_MESSAGE * p_message = (ANT_MESSAGE*) p_ant_evt->message.aucMessage;

    switch (p_ant_evt->event)
    {
        case EVENT_RX:
           if (p_message->ANT_MESSAGE_aucPayload[0] == DIGITALIO_DATA_PID)
           {
               //Set LEDs according to Received Digital IO Data Page
               m_rx_input_pin_state = p_message->ANT_MESSAGE_aucPayload[7];
               set_led_state();
           }
           // Transmit data on the reverse direction every channel period
           handle_transmit();
           break;

        case EVENT_TX:
        case EVENT_TRANSFER_TX_COMPLETED:
        case EVENT_TRANSFER_TX_FAILED:
          // Transmit data on the reverse direction every channel period
          handle_transmit();
          break;

        case EVENT_CHANNEL_CLOSED:
            // Re -open the channel
            err_code = sd_ant_channel_status_get (ANT_CHANNEL_NUMBER, &channel_status);
            APP_ERROR_CHECK(err_code);

            if ((channel_status & STATUS_CHANNEL_STATE_MASK) == STATUS_ASSIGNED_CHANNEL)
            {
                err_code = sd_ant_channel_open(ANT_CHANNEL_NUMBER);
                APP_ERROR_CHECK(err_code);
            }
            break;

        case EVENT_RX_SEARCH_TIMEOUT:
        default:
          break;
    }
}

NRF_SDH_ANT_OBSERVER(m_ant_observer, 1, ant_freq_ag_event_handler, NULL);


