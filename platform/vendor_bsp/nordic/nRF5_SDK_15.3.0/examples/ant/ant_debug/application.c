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
/**
 * @defgroup application Test Application
 * @brief Example of an application being debugged using debug messages.
 */

/**
 * @file application.c
 * @brief Example application source file
 * @ingroup application
 */


#include "application.h"
#include <stdint.h>
#include "string.h"
#include "ant_interface.h"
#include "ant_parameters.h"
#include "app_error.h"
#include "boards.h"
#include "bsp.h"
#include "sdk_config.h"
#include "ant_channel_config.h"
#include "debug.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"

// Data Page Numbers
#define DIGITALIO_DATA_PID              1u                          /**< Page number: digital data. */

#define APP_ANT_OBSERVER_PRIO           1                           /**< Application's ANT observer priority. You shouldn't need to modify this value. */

static uint8_t m_broadcast_data[ANT_STANDARD_DATA_PAYLOAD_SIZE];    /**< Primary data transmit buffer. */
static uint8_t m_rx_input_pin_state = 0xFF;                         /**< State of received digital data, from the other node. */
static uint8_t m_tx_input_pin_state = 0xFF;                         /**< State of digital inputs in this node, for transmission. */


/**@brief Encode current state of buttons
 *
 * Configure bitfield encoding the state of the buttons
 * Bit 0 = 0 if BSP_BUTTON_0 is pressed
 * Bit 1 = 0 if BSP_BUTTON_1 is pressed
 * Bit 2 = 0 if BSP_BUTTON_2 is pressed
 * Bit 3 = 0 if BSP_BUTTON_3 is pressed
 * Bit 4-7 = 1 (unused)
 *
 * The state of each button is also transmitted in the
 * debug channel, with 0 = button pressed.
 */
static void button_state_encode(void)
{
    m_tx_input_pin_state = 0xFF;

    #if DEBUG_CHANNEL_INCLUDED
    for (uint8_t i = ANT_DEBUG_FIELD_BUTTON_A_STATUS; i <= ANT_DEBUG_FIELD_BUTTON_D_STATUS; i++)
    {
        ad_debug_field_set(i, 1);
    }
    #endif // DEBUG_CHANNEL_INCLUDED

    if (bsp_button_is_pressed(0))
    {
        m_tx_input_pin_state &= 0xFE;
#if DEBUG_CHANNEL_INCLUDED
        ad_debug_field_set(ANT_DEBUG_FIELD_BUTTON_A_STATUS,0);
#endif // DEBUG_CHANNEL_INCLUDED
    }

    if (bsp_button_is_pressed(1))
    {
        m_tx_input_pin_state &= 0xFD;
#if DEBUG_CHANNEL_INCLUDED
        ad_debug_field_set(ANT_DEBUG_FIELD_BUTTON_B_STATUS,0);
#endif // DEBUG_CHANNEL_INCLUDED
    }

    if (bsp_button_is_pressed(2))
    {
        m_tx_input_pin_state &= 0xFB;
#if DEBUG_CHANNEL_INCLUDED
        ad_debug_field_set(ANT_DEBUG_FIELD_BUTTON_C_STATUS,0);
#endif // DEBUG_CHANNEL_INCLUDED
    }

    if (bsp_button_is_pressed(3))
    {
        m_tx_input_pin_state &= 0xF7;
#if DEBUG_CHANNEL_INCLUDED
        ad_debug_field_set(ANT_DEBUG_FIELD_BUTTON_D_STATUS,0);
#endif // DEBUG_CHANNEL_INCLUDED
    }
}


/**@brief Formats page with current button state and sends data
 * Byte 0 = Page number (Digital I/O Data)
 * Byte 1-6 = Reserved
 * Byte 7 = State of digital inputs
 */
static void transmit_handle()
{
    uint32_t err_code;

    button_state_encode();

    m_broadcast_data[0] = DIGITALIO_DATA_PID;
    m_broadcast_data[1] = 0xFF;
    m_broadcast_data[2] = 0xFF;
    m_broadcast_data[3] = 0xFF;
    m_broadcast_data[4] = 0xFF;
    m_broadcast_data[5] = 0xFF;
    m_broadcast_data[6] = 0xFF;
    m_broadcast_data[7] = m_tx_input_pin_state;

    err_code = sd_ant_broadcast_message_tx(ANT_CHANNEL_NUM,
                                           ANT_STANDARD_DATA_PAYLOAD_SIZE,
                                           m_broadcast_data);
    APP_ERROR_CHECK(err_code);
}


/**@brief Turns on LEDs according to the contents of the received data page.
 */
static void led_state_set()
{
    uint8_t led_state_field = ~m_rx_input_pin_state;

    if (led_state_field & 1)
    {
        bsp_board_led_on(BSP_BOARD_LED_0);
    }
    else
    {
        bsp_board_led_off(BSP_BOARD_LED_0);
    }

    if (led_state_field & 2)
    {
        bsp_board_led_on(BSP_BOARD_LED_1);
    }
    else
    {
        bsp_board_led_off(BSP_BOARD_LED_1);
    }

    if (led_state_field & 4)
    {
        bsp_board_led_on(BSP_BOARD_LED_2);
    }
    else
    {
        bsp_board_led_off(BSP_BOARD_LED_2);
    }

    if (led_state_field & 8)
    {
        bsp_board_led_on(BSP_BOARD_LED_3);
    }
    else
    {
        bsp_board_led_off(BSP_BOARD_LED_3);
    }

    #if DEBUG_CHANNEL_INCLUDED
    // Example use for Fast Debug Byte
    // Check the value actually written to LED A is correct by outputting to Fast Debug Byte (1 for on, 0 for off)
    ad_fast_debug_byte_set(bsp_board_led_state_get(BSP_BOARD_LED_0) ? 1 : 0);
    #endif // DEBUG_CHANNEL_INCLUDED
}


void app_channel_setup(void)
{
    uint32_t err_code;

    #if DEBUG_CHANNEL_INCLUDED
    // Init debug fields
    ad_debug_field_set(ANT_DEBUG_FIELD_TX_TOTAL_CH0, 0);
    ad_debug_field_set(ANT_DEBUG_FIELD_RX_TOTAL_CH0, 0);
    ad_debug_field_set(ANT_DEBUG_FIELD_COLLISIONS_CH0, 0);
    #endif // DEBUG_CHANNEL_INCLUDED

    ant_channel_config_t channel_config =
    {
        .channel_number    = ANT_CHANNEL_NUM,
        .channel_type      = CHANNEL_TYPE_MASTER,
        .ext_assign        = 0x00,
        .rf_freq           = RF_FREQ,
        .transmission_type = CHAN_ID_TRANS_TYPE,
        .device_type       = CHAN_ID_DEV_TYPE,
        .device_number     = (uint16_t) NRF_FICR->DEVICEID[0],
        .channel_period    = CHAN_PERIOD,
        .network_number    = ANT_NETWORK_NUM,
    };

    // Configure channel parameters
    err_code = ant_channel_init(&channel_config);
    APP_ERROR_CHECK(err_code);

    // Open channel.
    err_code = sd_ant_channel_open(ANT_CHANNEL_NUM);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling a ANT stack event.
 *
 * @param[in] p_ant_evt  ANT stack event.
 * @param[in] p_context  Context.
 */
static void ant_evt_handler(ant_evt_t * p_ant_evt, void * p_context)
{
    if (p_ant_evt->channel != ANT_CHANNEL_NUM)
    {
        return;
    }

    switch (p_ant_evt->event)
    {
        case EVENT_RX:
            if (p_ant_evt->message.ANT_MESSAGE_aucPayload[0] == DIGITALIO_DATA_PID)
            {
                // Set LEDs according to Received Digital IO Data Page
                m_rx_input_pin_state = p_ant_evt->message.ANT_MESSAGE_aucPayload[7];
                led_state_set();
            }
            #if DEBUG_CHANNEL_INCLUDED
            ad_debug_field_increment(ANT_DEBUG_FIELD_RX_TOTAL_CH0);
            #endif // DEBUG_CHANNEL_INCLUDED
            break;

        case EVENT_TX:
            // Transmit data on the reverse direction every channel period
            transmit_handle();
            #if DEBUG_CHANNEL_INCLUDED
            ad_debug_field_increment(ANT_DEBUG_FIELD_TX_TOTAL_CH0);
            #endif // DEBUG_CHANNEL_INCLUDED
            break;

        case EVENT_CHANNEL_COLLISION:
            #if DEBUG_CHANNEL_INCLUDED
            ad_debug_field_increment(ANT_DEBUG_FIELD_COLLISIONS_CH0);
            #endif // DEBUG_CHANNEL_INCLUDED
            break;

        default:
            break;
    }
}

NRF_SDH_ANT_OBSERVER(m_ant_observer, APP_ANT_OBSERVER_PRIO, ant_evt_handler, NULL);

#if DEBUG_CHANNEL_INCLUDED
void app_custom_debug_command_handler(uint8_t const * const p_command)
{
    // Example of a custom command sent over the debug channel which causes the device to turn all its LEDs on.
    // Do not define any custom commands which begin with the debug message indicator: 0xF9 - they will be misinterpreted as a filter message.
    if (p_command[0] == 0x42)
    {
        bsp_board_leds_on();
    }
    else
    {
        bsp_board_leds_off();
    }
}
#endif // DEBUG_CHANNEL_INCLUDED
