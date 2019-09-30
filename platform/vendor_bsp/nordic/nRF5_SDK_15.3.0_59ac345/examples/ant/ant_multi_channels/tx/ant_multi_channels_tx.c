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
#include <stdint.h>
#include "ant_interface.h"
#include "ant_parameters.h"
#include "app_error.h"
#include "boards.h"
#include "sdk_config.h"
#include "ant_channel_config.h"
#include "ant_multi_channels_tx.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"


#define APP_ANT_OBSERVER_PRIO   1   /**< Application's ANT observer priority. You shouldn't need to modify this value. */


// Private variables
static uint8_t m_counter[NRF_SDH_ANT_TOTAL_CHANNELS_ALLOCATED]  = {0};  /**< Counters to increment the ANT broadcast data payload. */
static uint8_t m_broadcast_data[ANT_STANDARD_DATA_PAYLOAD_SIZE] = {0};  /**< Primary data transmit buffers. */
static uint8_t m_num_open_channels                              = 0;    /**< Number of channels open */


/**@brief Function to display the bottom nibble of the input byte */
static void ant_scaleable_display_byte_on_leds(uint8_t byte_to_display)
{
    bsp_board_leds_off();

    if ((byte_to_display & 0x01) == 1)
    {
        bsp_board_led_on(BSP_BOARD_LED_3);
    }
    if (((byte_to_display >> 1) & 0x01) == 1)
    {
        bsp_board_led_on(BSP_BOARD_LED_2);
    }
    if (((byte_to_display >> 2) & 0x01) == 1)
    {
        bsp_board_led_on(BSP_BOARD_LED_1);
    }
    if (((byte_to_display >> 3) & 0x01) == 1)
    {
        bsp_board_led_on(BSP_BOARD_LED_0);
    }
}


void ant_scaleable_channel_tx_broadcast_setup(void)
{
    uint32_t err_code;
    uint32_t i = 0;

    ant_channel_config_t channel_config =
    {
        .channel_number     = i,
        .channel_type       = CHANNEL_TYPE_MASTER,
        .ext_assign         = 0x00,
        .rf_freq            = RF_FREQ,
        .transmission_type  = CHAN_ID_TRANS_TYPE,
        .device_type        = CHAN_ID_DEV_TYPE,
        .device_number      = i + 1,
        .channel_period     = CHAN_PERIOD,
        .network_number     = ANT_NETWORK_NUM,
    };

    for (i = 0; i < NRF_SDH_ANT_TOTAL_CHANNELS_ALLOCATED; i++)
    {
        // Configure channel
        channel_config.channel_number = i;
        channel_config.device_number  = i + 1;

        err_code = ant_channel_init(&channel_config);
        APP_ERROR_CHECK(err_code);

        // Open channel
        err_code = sd_ant_channel_open(i);
        APP_ERROR_CHECK(err_code);

        m_num_open_channels++;
        ant_scaleable_display_byte_on_leds(m_num_open_channels);
    }
}


/**@brief Function for handling a ANT stack event.
 *
 * @param[in] p_ant_evt  ANT stack event.
 * @param[in] p_context  Context.
 */
void ant_evt_handler(ant_evt_t * p_ant_evt, void * p_context)
{
    uint32_t err_code;

    switch (p_ant_evt->event)
    {
        // ANT broadcast success.
        // Increment the counter and send a new broadcast.
        case EVENT_TX:
            // Increment the counter.
            m_counter[p_ant_evt->channel]++;

            // Assign a new value to the broadcast data.
            m_broadcast_data[ANT_STANDARD_DATA_PAYLOAD_SIZE - 1] = m_counter[p_ant_evt->channel];

            // Broadcast the data.
            err_code = sd_ant_broadcast_message_tx(p_ant_evt->channel,
                                                   ANT_STANDARD_DATA_PAYLOAD_SIZE,
                                                   m_broadcast_data);
            APP_ERROR_CHECK(err_code);

            break;

        default:
            break;
    }
}

NRF_SDH_ANT_OBSERVER(m_ant_observer, APP_ANT_OBSERVER_PRIO, ant_evt_handler, NULL);
