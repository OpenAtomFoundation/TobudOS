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
#include "ant_search_config.h"
#include "ant_multi_channels_rx.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"


#define APP_ANT_OBSERVER_PRIO   1   /**< Application's ANT observer priority. You shouldn't need to modify this value. */


// Private variables
static bool m_tracking_channels[NRF_SDH_ANT_TOTAL_CHANNELS_ALLOCATED] = { false };     /**< Array to keep track of which channels are currently tracking */

/**@brief Function to display the bottom nibble of the input byte on the board's LEDs
 *
 */
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


/**@brief Function for handling ANT GO_TO_SEARCH events
 *
 */
static void goto_search_handler(uint8_t channel)
{
    m_tracking_channels[channel] = false;
    ant_scaleable_display_num_tracking_channels();
}


/**@brief Function for handling ANT RX events
 *
 */
static void message_received_handler(uint8_t ant_channel, ant_evt_t * p_ant_evt)
{
    switch (p_ant_evt->message.ANT_MESSAGE_ucMesgID)
    {
        // Broadcast data received
        case MESG_BROADCAST_DATA_ID:
            m_tracking_channels[ant_channel] = true;
            ant_scaleable_display_num_tracking_channels();
            break;

        default:
            break;
    }
}


void ant_scaleable_display_num_tracking_channels()
{
    uint8_t  num_tracking_channels = 0;
    uint32_t i;

    for (i = 0; i < NRF_SDH_ANT_TOTAL_CHANNELS_ALLOCATED; i++)
    {
        if (m_tracking_channels[i])
        {
            num_tracking_channels++;
        }
    }
    ant_scaleable_display_byte_on_leds(num_tracking_channels);
}


void ant_scaleable_channel_rx_broadcast_setup(void)
{
    uint32_t err_code;
    uint32_t i = 0;

    ant_channel_config_t channel_config =
    {
        .channel_number     = i,
        .channel_type       = CHANNEL_TYPE_SLAVE,
        .ext_assign         = 0x00,
        .rf_freq            = RF_FREQ,
        .transmission_type  = CHAN_ID_TRANS_TYPE,
        .device_type        = CHAN_ID_DEV_TYPE,
        .device_number      = i + 1,
        .channel_period     = CHAN_PERIOD,
        .network_number     = ANT_NETWORK_NUM,
    };

    ant_search_config_t ant_search_config   = DEFAULT_ANT_SEARCH_CONFIG(i);
    ant_search_config.high_priority_timeout = ANT_HIGH_PRIORITY_TIMEOUT_DISABLE;

    for (i = 0; i < NRF_SDH_ANT_TOTAL_CHANNELS_ALLOCATED; i++)
    {
        // Configure channel
        channel_config.channel_number = i;
        channel_config.device_number  = i + 1;

        err_code = ant_channel_init(&channel_config);
        APP_ERROR_CHECK(err_code);

        // Set search timeout
        ant_search_config.channel_number = i;
        err_code = ant_search_init(&ant_search_config);
        APP_ERROR_CHECK(err_code);

        // Open channel
        err_code = sd_ant_channel_open(i);
        APP_ERROR_CHECK(err_code);
    }
}

/**@brief Function for handling a ANT stack event.
 *
 * @param[in] p_ant_evt  ANT stack event.
 * @param[in] p_context  Context.
 */
void ant_evt_handler(ant_evt_t * p_ant_evt, void * p_context)
{
    switch (p_ant_evt->event)
    {
        case EVENT_RX_FAIL_GO_TO_SEARCH:
            goto_search_handler(p_ant_evt->channel);
            break;

        case EVENT_RX:
            message_received_handler(p_ant_evt->channel, p_ant_evt);
            break;

        default:
            break;
    }
}

NRF_SDH_ANT_OBSERVER(m_ant_observer, APP_ANT_OBSERVER_PRIO, ant_evt_handler, NULL);
