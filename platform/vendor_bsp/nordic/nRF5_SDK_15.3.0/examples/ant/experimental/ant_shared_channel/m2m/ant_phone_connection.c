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
#include "ant_phone_connection.h"
#include <stdbool.h>
#include "asc_pages.h"
#include "string.h"
#include "app_error.h"
#include "ant_parameters.h"
#include "asc_events.h"
#include "asc_parameters.h"
#include "asc_pages.h"
#include "nrf.h"

typedef enum
{
    PHONE_OFF = 0,
    PHONE_ON,
} connection_states_t;


////////////////////////////////////////////////////////////////////////////////
// Private Variables
////////////////////////////////////////////////////////////////////////////////
static uint8_t              m_channel_number;
static uint16_t             m_device_number;
static asc_command_data_t   m_last_command;
static bool                 m_is_update_available = false;
static uint8_t              m_retries;
static uint8_t              m_retry_counter = 0;
static uint8_t              m_tx_buffer[ANT_STANDARD_DATA_PAYLOAD_SIZE] = {0};
//static uint8_t              m_rx_buffer[ANT_STANDARD_DATA_PAYLOAD_SIZE] = {0};
//static uint8_t              m_tx_counter = 0;
static connection_states_t  m_state;
static uint16_t             m_neighbor_id = INVALID_NEIGHBOUR_ID;
static uint32_t             m_asc_events;

////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////
/**@brief ANT message processor for the current <STATE>
 *
 * @param  pucANTMessage - pointer to a buffer with an ANT event
 */
static void process_message_on(uint8_t* p_ant_message)
{
    uint32_t err_code;

    switch (p_ant_message[ANT_MESSAGE_ID_OFFSET])
    {
        case MESG_RESPONSE_EVENT_ID:
        {
            if (p_ant_message[ANT_MESSAGE_DATA0_OFFSET] == MESG_EVENT_ID)   //Channel event
            {
                switch (p_ant_message[ANT_MESSAGE_DATA1_OFFSET])
                {
                    case EVENT_TX:
                    {
                        if (m_is_update_available && ++m_retry_counter >= m_retries)
                        {
                            m_is_update_available = false;
                            m_retry_counter = 0;
                            asc_encode_device_availability_page(m_neighbor_id, m_tx_buffer);
                            err_code = sd_ant_broadcast_message_tx (m_channel_number, ANT_STANDARD_DATA_PAYLOAD_SIZE, m_tx_buffer);
                            APP_ERROR_CHECK(err_code);
                        }
                        break;
                    }
                }
            }
            break;
        }
        case MESG_BROADCAST_DATA_ID:
            //fall-through
        case MESG_ACKNOWLEDGED_DATA_ID:
        {
            //Process by page
            switch (p_ant_message[ANT_MESSAGE_DATA0_OFFSET])
            {
                case PHONE_COMMAND_PID:
                {
                    //Store command info
//lint --e{534}
                    asc_decode_phone_command_page(&m_last_command, p_ant_message);
                    asc_event_set(&m_asc_events, EVENT_ASC_COMMAND_RECEIVED);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}

/**@brief Sets the state machine to the applicable state
 */
static void set_state(connection_states_t new_state)
{
    uint32_t err_code;

    if (m_state != new_state)
    {
        switch (new_state)
        {
            case PHONE_OFF:
            {
                uint8_t channel_status;
                err_code = sd_ant_channel_status_get(m_channel_number, &channel_status);
                APP_ERROR_CHECK(err_code);

                m_state = new_state;

                switch (channel_status)
                {
                    case STATUS_SEARCHING_CHANNEL:
                        //Intentional fallthrough
                    case STATUS_TRACKING_CHANNEL:
                    {
                        err_code = sd_ant_channel_close(m_channel_number);
                        APP_ERROR_CHECK(err_code);
                    } //Intentional fallthrough
                    case STATUS_ASSIGNED_CHANNEL:
                    {
                        err_code = sd_ant_channel_unassign(m_channel_number);
                        APP_ERROR_CHECK(err_code);
                        break;
                    }
                    default:
                    {
                        return;
                    }
                }
                break;
            }
            case PHONE_ON:
            {
                //uint32_t err_code;
                if (m_state == PHONE_OFF)
                {
                    //Open the channel
                    err_code = sd_ant_channel_open(m_channel_number);
                    APP_ERROR_CHECK(err_code);
                }

                m_state = new_state;

                //Prepare the buffer
                asc_encode_device_availability_page(m_neighbor_id, m_tx_buffer);
                err_code = sd_ant_broadcast_message_tx (m_channel_number, ANT_STANDARD_DATA_PAYLOAD_SIZE, m_tx_buffer);
                APP_ERROR_CHECK(err_code);
                break;
            }
            default:
            {
                return;
            }
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////

/**@brief Initializes controllable device. Sets up channel but does not open it.
 */
void phc_init(const asc_ant_params_t * const p_ant_params)
{
    uint32_t err_code;

    if (m_state == PHONE_OFF)
    {
        m_channel_number = p_ant_params->channel_number;
        m_device_number = *(p_ant_params->p_device_number);

        //Assign the slave channel
        err_code = sd_ant_channel_assign(m_channel_number, CHANNEL_TYPE_MASTER, p_ant_params->network_number, ASC_ANT_EXT_ASSIGNMENT);
        APP_ERROR_CHECK(err_code);

        //Assign channel id
        err_code = sd_ant_channel_id_set (m_channel_number, m_device_number, p_ant_params->device_type , p_ant_params->tx_type);
        APP_ERROR_CHECK(err_code);

        //Assign channel frequency
        err_code = sd_ant_channel_radio_freq_set (m_channel_number, p_ant_params->rf_frequency);
        APP_ERROR_CHECK(err_code);

        //Assign channel message period
        err_code = sd_ant_channel_period_set (m_channel_number, asc_get_counts_from_period_enum(p_ant_params->channel_period));
        APP_ERROR_CHECK(err_code);

        //Set the hub's transmission power
        err_code = sd_ant_channel_radio_tx_power_set (m_channel_number, p_ant_params->tx_power, 0);
        APP_ERROR_CHECK(err_code);
    }

}

/**@brief Turns the phone connection on and opens the channel.
 */
void phc_turn_on(void)
{
    set_state(PHONE_ON);
}

/**@brief Handles ANT events on the controllable device's channel
 *
 * @param[in] event The ANT event type that was received.
 *
 * @param[in] p_event_message_buffer The received message buffer.
 */
void phc_handle_ant_event(uint8_t event, uint8_t* p_ant_message)
{
    if (p_ant_message)
    {
        //Process message
        switch (m_state)
        {
            case PHONE_OFF:
                //No messages can be processed while off
                break;
            case PHONE_ON:
                process_message_on(p_ant_message);
                break;
        }
    }
}

void phc_transmit_message(uint8_t * p_tx_buffer, uint8_t retries)
{
    uint32_t err_code;

    //This could benefit from a small message queue, as repeated calls to this function will overwrite
    //the data from previous calls.
    m_is_update_available = true;
    m_retries = retries;
    memcpy(m_tx_buffer, p_tx_buffer, ANT_STANDARD_DATA_PAYLOAD_SIZE);
    err_code = sd_ant_broadcast_message_tx (m_channel_number, ANT_STANDARD_DATA_PAYLOAD_SIZE, m_tx_buffer);
    APP_ERROR_CHECK(err_code);
}

void phc_set_neighbor_id(uint16_t neighbor_id)
{
    uint32_t err_code;

    m_neighbor_id = neighbor_id;

    asc_encode_device_availability_page(m_neighbor_id, m_tx_buffer);
    err_code = sd_ant_broadcast_message_tx (m_channel_number, ANT_STANDARD_DATA_PAYLOAD_SIZE, m_tx_buffer);
    APP_ERROR_CHECK(err_code);
}


uint32_t phc_events_get(void)
{
    return m_asc_events;
}


void phc_event_clear(uint32_t event)
{
    asc_event_clear(&m_asc_events, event);
}


asc_command_data_t phc_get_last_command(void)
{
    return m_last_command;
}
