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
#include "ant_async_tx.h"
#include <stdint.h>
#include "ant_interface.h"
#include "ant_parameters.h"
#include "app_timer.h"
#include "app_error.h"
#include "boards.h"
#include "sdk_config.h"
#include "ant_channel_config.h"
#include "ant_search_config.h"
#include "nrf_soc.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ant.h"

// Application specific commands
#define COMMAND_PAGE                   0x01                               /**< Page number for command data */
#define COMMAND_0                      0x10                               /**< Command associated with pressing BSP_BUTTON_0 */
#define COMMAND_1                      0x11                               /**< Command associated with pressing BSP_BUTTON_1 */

// Retry configuration
#define MAX_RETRIES                    8                                  /**< Maximum number of retries for sending a command */
#define MIN_BACKOFF_TIME               20                                 /**< Minimum number of ms to wait before next retry attempt. The maximum back off time is 255 + MIN_BACKOFF_TIME */

#define APP_ANT_OBSERVER_PRIO          1                                  /**< Application's ANT observer priority. You shouldn't need to modify this value. */

static uint8_t  m_cmd_tx_buffer[ANT_STANDARD_DATA_PAYLOAD_SIZE];          /**< Command data transmit buffer */
static uint8_t  m_retries;                                                /**< Number of remaining retries */
static uint8_t  m_sequence_number;                                        /**< Sequence number */

APP_TIMER_DEF(m_backoff_timer);                                           /**< Back off timer */


/**@brief Perform message retries
 *
 */
static void retry_perform(void)
{
    if (m_retries > 0)
    {
        uint32_t err_code = sd_ant_acknowledge_message_tx(ANT_CHANNEL_NUM,
                                                          ANT_STANDARD_DATA_PAYLOAD_SIZE,
                                                          m_cmd_tx_buffer);
        if (err_code != TRANSFER_IN_PROGRESS)
        {
            // Transfer in progress means that a message is waiting to be transmitted by ANT as an ACK.
            // Ignore this error; attempts to initiate a burst transfer while already busy handling one
            // will be ignored.
            APP_ERROR_CHECK(err_code);
        }

        m_retries--;
    }
}


/**@brief Send command over async channel
 *
 * @param[in] command   Command to send
 */
static void async_command_send(uint8_t command)
{
    m_cmd_tx_buffer[0] = COMMAND_PAGE;
    m_cmd_tx_buffer[1] = 0xFF;
    m_cmd_tx_buffer[2] = 0xFF;
    m_cmd_tx_buffer[3] = 0xFF;
    m_cmd_tx_buffer[4] = 0xFF;
    m_cmd_tx_buffer[5] = 0xFF;
    m_cmd_tx_buffer[6] = m_sequence_number;
    m_cmd_tx_buffer[7] = command;
    m_retries          = MAX_RETRIES;
    m_sequence_number++;
    retry_perform();
}


/**@brief Handler for backoff timer event
 *
 * @param[in] p_context   Pointer to context
 */
static void backoff_timer_evt_handler(void * p_context)
{
    retry_perform();
}


/**@brief Function for generating a random number using the RNG
 * The number generated is between [MIN_BACKOFF_TIME, 255 + MIN_BACKOFF_TIME] (inclusive)
 *
 */
static uint16_t rnd_backoff_generate(void)
{
    uint8_t  rand_bytes[1]   = {0};
    uint8_t  bytes_available = 0;
    uint32_t err_code;
    uint16_t rand_value;

    while (bytes_available < 1)
    {
        err_code = sd_rand_application_bytes_available_get(&bytes_available);
        APP_ERROR_CHECK(err_code);
    }

    err_code = sd_rand_application_vector_get(rand_bytes, 1);
    APP_ERROR_CHECK(err_code);

    rand_value = (uint16_t) (rand_bytes[0]) + MIN_BACKOFF_TIME;

    return rand_value;
}


void ant_async_tx_setup(void)
{
    uint32_t err_code;

    ant_channel_config_t channel_config =
    {
        .channel_number    = ANT_CHANNEL_NUM,
        .channel_type      = CHANNEL_TYPE_MASTER,
        .ext_assign        = EXT_PARAM_ASYNC_TX_MODE | EXT_PARAM_FAST_INITIATION_MODE,
        .rf_freq           = RF_FREQ,
        .transmission_type = CHAN_ID_TRANS_TYPE,
        .device_type       = CHAN_ID_DEV_TYPE,
        .device_number     = (uint16_t) (NRF_FICR->DEVICEID[0]),
        .channel_period    = CHAN_PERIOD,
        .network_number    = ANT_NETWORK_NUM,
    };

    err_code = ant_channel_init(&channel_config);
    APP_ERROR_CHECK(err_code);

    m_retries         = 0;
    m_sequence_number = 0;

    err_code = app_timer_create(&m_backoff_timer,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                backoff_timer_evt_handler);
    APP_ERROR_CHECK(err_code);
}


void ant_async_tx_bsp_evt_handler(bsp_event_t evt)
{
    switch (evt)
    {
        case BSP_EVENT_KEY_0:
            bsp_board_led_invert(BSP_BOARD_LED_0);
            async_command_send(COMMAND_0);
            break;

        case BSP_EVENT_KEY_1:
            bsp_board_led_invert(BSP_BOARD_LED_0);
            async_command_send(COMMAND_1);
            break;

        default:
            return; // no implementation needed
    }
}


void ant_async_tx_event_handler(ant_evt_t * p_ant_evt, void * p_context)
{
    switch (p_ant_evt->event)
    {
        case EVENT_TRANSFER_TX_COMPLETED:
            // The message was successfully transmitted
            m_retries = 0;
            break;

        case EVENT_TRANSFER_TX_FAILED:
            // The message failed; attempt a retry after a random backoff period
            if (m_retries > 0)
            {
                uint32_t backoff_time = (uint32_t) rnd_backoff_generate();

                uint32_t err_code =
                    app_timer_start(m_backoff_timer,
                                    APP_TIMER_TICKS(backoff_time),
                                    NULL);
                APP_ERROR_CHECK(err_code);
            }
            break;

        default:
            break;
    }
}

NRF_SDH_ANT_OBSERVER(m_ant_observer, APP_ANT_OBSERVER_PRIO, ant_async_tx_event_handler, NULL);
