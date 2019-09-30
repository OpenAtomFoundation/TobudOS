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
#include "ant_search_sharing.h"
#include <stdint.h>
#include "ant_interface.h"
#include "ant_parameters.h"
#include "app_error.h"
#include "boards.h"
#include "sdk_config.h"
#include "ant_channel_config.h"
#include "ant_search_config.h"
#include "nrf_soc.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"


// Miscellaneous defines.
#define ANT_CHANNEL_NUM_0           0       /**< ANT Channel Number for channel 0. */
#define ANT_CHANNEL_NUM_1           1       /**< ANT Channel Number for channel 1. */

#define APP_ANT_OBSERVER_PRIO       1       /**< Application's ANT observer priority. You shouldn't need to modify this value. */

bool m_rx_first_ch0;                        /**< Received first message on channel 0. */
bool m_rx_first_ch1;                        /**< Received first message on channel 1. */


void ant_search_sharing_setup(void)
{
    uint32_t err_code;

    ant_channel_config_t channel_config =
    {
        .channel_type   = CHANNEL_TYPE_SLAVE,
        .ext_assign     = 0x00,
        .device_number  = 0x00,     // Wild card
        .network_number = ANT_NETWORK_NUM,
    };

    ant_search_config_t search_config =
    {
        .low_priority_timeout  = SEARCH_TIMEOUT,
        .high_priority_timeout = ANT_HIGH_PRIORITY_SEARCH_DISABLE,
        .search_sharing_cycles = SEARCH_CYCLES,
        .search_priority       = ANT_SEARCH_PRIORITY_DEFAULT,
        .waveform              = ANT_WAVEFORM_FAST,     // Use fast waveform to limit impact on acquisition time from searching on multiple frequencies
    };

    m_rx_first_ch0 = false;
    m_rx_first_ch1 = false;

    // Configure channel parameters for channel 0
    channel_config.channel_number    = ANT_CHANNEL_NUM_0;
    channel_config.device_type       = CH0_CHAN_ID_DEV_TYPE;
    channel_config.transmission_type = CH0_CHAN_ID_TRANS_TYPE;
    channel_config.channel_period    = CH0_CHAN_PERIOD;
    channel_config.rf_freq           = CH0_RF_FREQ;
    err_code                         = ant_channel_init(&channel_config);
    APP_ERROR_CHECK(err_code);

    // Configure search parameters for channel 0
    search_config.channel_number = ANT_CHANNEL_NUM_0;
    err_code                     = ant_search_init(&search_config);
    APP_ERROR_CHECK(err_code);

    // Configure channel parameters for channel 1
    channel_config.channel_number    = ANT_CHANNEL_NUM_1;
    channel_config.device_type       = CH1_CHAN_ID_DEV_TYPE;
    channel_config.transmission_type = CH1_CHAN_ID_TRANS_TYPE;
    channel_config.channel_period    = CH1_CHAN_PERIOD;
    channel_config.rf_freq           = CH1_RF_FREQ;
    err_code                         = ant_channel_init(&channel_config);
    APP_ERROR_CHECK(err_code);

    // Configure search parameters for channel 1
    search_config.channel_number = ANT_CHANNEL_NUM_1;
    err_code                     = ant_search_init(&search_config);
    APP_ERROR_CHECK(err_code);

    // Open both channels
    err_code = sd_ant_channel_open(ANT_CHANNEL_NUM_0);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ant_channel_open(ANT_CHANNEL_NUM_1);
    APP_ERROR_CHECK(err_code);

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
        case EVENT_RX:
            switch (p_ant_evt->channel)
            {
                case ANT_CHANNEL_NUM_0:
                    if (!m_rx_first_ch0)
                    {
                        m_rx_first_ch0 = true;
                        bsp_board_led_on(BSP_BOARD_LED_0);
                    }
                    break;

                case ANT_CHANNEL_NUM_1:
                    if (!m_rx_first_ch1)
                    {
                        m_rx_first_ch1 = true;
                        bsp_board_led_on(BSP_BOARD_LED_1);
                    }
                    break;
            }
            break;

        default:
            break; // No implementation needed
    }
}

NRF_SDH_ANT_OBSERVER(m_ant_observer, APP_ANT_OBSERVER_PRIO, ant_evt_handler, NULL);
