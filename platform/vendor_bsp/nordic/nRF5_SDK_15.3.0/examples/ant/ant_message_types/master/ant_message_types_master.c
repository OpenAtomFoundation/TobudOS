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
#include "ant_message_types_master.h"
#include <stdint.h>
#include "string.h"
#include "ant_interface.h"
#include "ant_parameters.h"
#include "app_error.h"
#include "boards.h"
#include "sdk_config.h"
#include "ant_channel_config.h"
#include "nrf_delay.h"
#include "nrf_sdh_ant.h"

// I/O configuration
#define LED_BROADCAST                   BSP_BOARD_LED_0
#define LED_ACKNOWLEDGED                BSP_BOARD_LED_1
#define LED_BURST                       BSP_BOARD_LED_2

// Channel configuration.
#define ANT_CUSTOM_TRANSMIT_POWER       0u      /**< ANT Custom Transmit Power (Invalid/Not Used). */
#define BROADCAST_DATA_BUFFER_SIZE      8u      /**< Size of the broadcast data buffer. */
#define BURST_BLOCK_SIZE                32u     /**< Size of data block transmitted via burst. Size must be divisible by 8. */

#define APP_ANT_OBSERVER_PRIO           1       /**< Application's ANT observer priority. You shouldn't need to modify this value. */

// Static variables and buffers.
static uint8_t m_tx_buffer[BROADCAST_DATA_BUFFER_SIZE]; /**< Primary data (Broadcast/Acknowledged) transmit buffer. */
static uint8_t m_counter = 1u;                               /**< Counter to increment the ANT broadcast data payload. */
static uint8_t m_burst_data[BURST_BLOCK_SIZE];               /**< Burst data transmit buffer. */

// State Machine
enum MESSAGE_TYPES_MASTER_STATES
{
    BROADCAST,
    ACKNOWLEDGED,
    BURST
} state_message_types;

void ant_message_types_master_setup(void)
{
    uint32_t err_code;

    ant_channel_config_t channel_config =
    {
        .channel_number    = ANT_CHANNEL_NUM,
        .channel_type      = CHANNEL_TYPE_MASTER,
        .ext_assign        = 0x00,
        .rf_freq           = RF_FREQ,
        .transmission_type = CHAN_ID_TRANS_TYPE,
        .device_type       = CHAN_ID_DEV_TYPE,
        .device_number     = (uint16_t) (NRF_FICR->DEVICEID[0]),
        .channel_period    = CHAN_PERIOD,
        .network_number    = ANT_NETWORK_NUM,
    };

    err_code = ant_channel_init(&channel_config);
    APP_ERROR_CHECK(err_code);

    //Set Tx Power
    err_code = sd_ant_channel_radio_tx_power_set(ANT_CHANNEL_NUM,
                                                 RADIO_TX_POWER_LVL_3,
                                                 ANT_CUSTOM_TRANSMIT_POWER);
    APP_ERROR_CHECK(err_code);

    // Open channel.
    err_code = sd_ant_channel_open(ANT_CHANNEL_NUM);
    APP_ERROR_CHECK(err_code);

    // Write counter value to last byte of the broadcast data.
    // The last byte is chosen to get the data more visible in the end of an printout
    // on the recieving end.
    memset(m_tx_buffer, 0, BROADCAST_DATA_BUFFER_SIZE);
    m_tx_buffer[BROADCAST_DATA_BUFFER_SIZE - 1] = m_counter;

    // Configure the initial payload of the broadcast data
    err_code = sd_ant_broadcast_message_tx(ANT_CHANNEL_NUM,
                                           BROADCAST_DATA_BUFFER_SIZE,
                                           m_tx_buffer);
    APP_ERROR_CHECK(err_code);

    //Set state to broadcasting
    state_message_types = BROADCAST;
}


void ant_message_types_master_bsp_evt_handler(bsp_event_t evt)
{
    switch (evt)
    {
        case BSP_EVENT_KEY_0:
            state_message_types = BROADCAST;
            break;

        case BSP_EVENT_KEY_1:
            state_message_types = ACKNOWLEDGED;
            break;

        case BSP_EVENT_KEY_2:
            state_message_types = BURST;
            break;

        default:
            break; // no implementation needed
    }
}

/**@brief Function for handling a ANT stack event.
 *
 * @param[in] p_ant_evt  ANT stack event.
 * @param[in] p_context  Context.
 */
static void ant_evt_handler(ant_evt_t * p_ant_evt, void * p_context)
{
    uint32_t err_code;
    uint32_t led_output = LED_BROADCAST;

    switch (p_ant_evt->event)
    {
        // ANT broadcast/Acknowledged/Burst Success
        // Send the next message according to the current state and increment the counter.
        case EVENT_TX:                      // Intentional fall through
        case EVENT_TRANSFER_TX_COMPLETED:   // Intentional fall through
        case EVENT_TRANSFER_TX_FAILED:
            bsp_board_leds_off();
            m_tx_buffer[BROADCAST_DATA_BUFFER_SIZE - 1] = m_counter;

            if (state_message_types == BROADCAST)
            {
                // Send as broadcast
                err_code = sd_ant_broadcast_message_tx(ANT_CHANNEL_NUM,
                                                       BROADCAST_DATA_BUFFER_SIZE,
                                                       m_tx_buffer);
                APP_ERROR_CHECK(err_code);

                led_output = LED_BROADCAST;
            }
            else if (state_message_types == ACKNOWLEDGED)
            {
                // Send as acknowledged
                err_code = sd_ant_acknowledge_message_tx(ANT_CHANNEL_NUM,
                                                         BROADCAST_DATA_BUFFER_SIZE,
                                                         m_tx_buffer);
                APP_ERROR_CHECK(err_code);

                led_output = LED_ACKNOWLEDGED;
            }
            else if (state_message_types == BURST)
            {
                // If this is a new message, populate the burst buffer
                // with new dummy data.  Otherwise, will retry sending the
                // same content.
                if (p_ant_evt->event != EVENT_TRANSFER_TX_FAILED)
                {
                    for (uint32_t i = 0; i < BURST_BLOCK_SIZE; i++)
                    {
                        m_burst_data[i] = m_counter;
                        m_counter++;
                    }
                }

                // Queue a Burst Transfer.  Since this is a small burst, queue entire burst.
                err_code = sd_ant_burst_handler_request(ANT_CHANNEL_NUM,
                                                        BURST_BLOCK_SIZE,
                                                        m_burst_data,
                                                        (BURST_SEGMENT_START | BURST_SEGMENT_END));
                APP_ERROR_CHECK(err_code);

                led_output = LED_BURST;
            }
            // Activate LED for 20ms
            bsp_board_led_on(led_output);
            nrf_delay_ms(20);
            bsp_board_led_off(led_output);
            m_counter++;
            break;

        case TRANSFER_IN_PROGRESS:              //Intentional fall through
        case TRANSFER_SEQUENCE_NUMBER_ERROR:    //Intentional fall through
        case TRANSFER_IN_ERROR:                 //Intentional fall through
        case TRANSFER_BUSY:
            // Ignore these events; will retry burst transfer when we get the EVENT_TRANSFER_TX_FAILED event.
            break;

        default:
            break; // No implementation needed

    }
}

NRF_SDH_ANT_OBSERVER(m_ant_observer, APP_ANT_OBSERVER_PRIO, ant_evt_handler, NULL);
