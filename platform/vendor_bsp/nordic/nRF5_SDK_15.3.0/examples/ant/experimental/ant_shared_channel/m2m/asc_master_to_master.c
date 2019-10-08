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
/**@file
 * @defgroup asc_multimaster module
 * @{
 * @ingroup ant_auto_shared_channel
 *
 * @brief ANT Auto Shared Channel (ASC) Master implementation.
 */


#include "asc_master_to_master.h"
#include <stdint.h>
#include <string.h>
#include "ant_interface.h"
#include "ant_parameters.h"
#include "app_error.h"
#include "asc_events.h"
#include "asc_parameters.h"
#include "nrf_soc.h"

#define ANT_WILDCARD_DEVICE_NUMBER          ((uint16_t) 0)
#define DISCOVERY_SEARCH_TIMEOUT            ((uint8_t) 0)   // Disable high priority search to avoid channel collisions
#define DISCOVERY_LP_SEARCH_TIMEOUT         ((uint8_t) 8)   // 20 seconds

#define ASCMM_SEARCH_TIMEOUT                ((uint8_t) 0)   // Disable high priority search to avoid channel collisions
#define ASCMM_LP_SEARCH_TIMEOUT             ((uint8_t) 12)  // 30 secondss

typedef struct
{
    uint16_t    device_number;
    uint8_t     page_id;
    uint16_t    payload;
} ascmm_neighbor_update_t;

// Private Variables
static const uint8_t            ascmm_idle_page[ANT_STANDARD_DATA_PAYLOAD_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint16_t                 m_device_number;            //used to perform the comparison between this device and the found neighbor
static uint8_t                  m_discovery_channel_number;
static uint16_t                 m_connection_device_number;
static ascmm_neighbor_update_t  m_neighbor;
static asc_ant_params_t         m_ant_connected_parameters;
static ascmm_states_t           m_state;
static uint32_t                 m_asc_event_flags = 0;
static uint8_t                  m_tx_buffer[ANT_STANDARD_DATA_PAYLOAD_SIZE] = {0};
static asc_command_data_t       m_last_received_command;
static asc_update_data_t        m_last_received_update;
static uint8_t                  m_retries;


// Private Functions

/**@brief Sets the state machine to the specified state.
 */
static void ascmm_set_state(ascmm_states_t new_state)
{
    uint32_t err_code;

    if (new_state != m_state)
    {
        switch (new_state)
        {
            case ASCMM_OFF:
            {
                //Close the channel
                err_code = sd_ant_channel_close(m_discovery_channel_number);
                APP_ERROR_CHECK(err_code);
                break;
            }
            case DISCOVERY:
            {
                if (m_state != ASCMM_OFF)
                {
                    asc_event_set(&m_asc_event_flags, EVENT_ASC_DEVICE_IN_WRONG_STATE);
                    return;
                }

                m_state = new_state;
                //Open the discovery channel
                err_code = sd_ant_channel_open(m_discovery_channel_number);

                break;
            }
            case CONNECTED:
            {
                if (m_state != DISCOVERY)
                {
                    asc_event_set(&m_asc_event_flags, EVENT_ASC_DEVICE_IN_WRONG_STATE);
                    return;
                }
                m_state = new_state;

                //Close the channel
                err_code = sd_ant_channel_close(m_discovery_channel_number);
                APP_ERROR_CHECK(err_code);


                //Assign the connection channel
                err_code = sd_ant_channel_assign(m_ant_connected_parameters.channel_number,
                                                 m_ant_connected_parameters.channel_type,
                                                 m_ant_connected_parameters.network_number, 0);
                APP_ERROR_CHECK(err_code);

                //Set channel id
                err_code = sd_ant_channel_id_set (m_ant_connected_parameters.channel_number,
                                                  m_connection_device_number,
                                                  m_ant_connected_parameters.device_type,
                                                  m_ant_connected_parameters.tx_type);
                APP_ERROR_CHECK(err_code);

                //Set radio frequency
                err_code = sd_ant_channel_radio_freq_set (m_ant_connected_parameters.channel_number,
                                                          m_ant_connected_parameters.rf_frequency);
                APP_ERROR_CHECK(err_code);

                if (m_ant_connected_parameters.channel_type == CHANNEL_TYPE_SLAVE)
                {
                    //Configure search timeout for slave channel
                    err_code = sd_ant_channel_rx_search_timeout_set(m_ant_connected_parameters.channel_number,
                                                                    ASCMM_SEARCH_TIMEOUT);
                    APP_ERROR_CHECK(err_code);

                    err_code = sd_ant_channel_low_priority_rx_search_timeout_set(m_ant_connected_parameters.channel_number,
                                                                                 ASCMM_LP_SEARCH_TIMEOUT);
                    APP_ERROR_CHECK(err_code);
                }
                else
                {
                    // Configure Tx power for master chanel
                    err_code = sd_ant_channel_radio_tx_power_set(m_ant_connected_parameters.channel_number,
                                                                 m_ant_connected_parameters.tx_power, 0);
                    APP_ERROR_CHECK(err_code);
                }

                err_code = sd_ant_channel_open(m_ant_connected_parameters.channel_number);
                APP_ERROR_CHECK(err_code);

                break;
            }
            default:
            {
                //if the new state was not recognized, return to avoid setting the state changed event
                asc_event_set(&m_asc_event_flags, EVENT_ASC_DEVICE_IN_WRONG_STATE);
                return;
            }
        }
        asc_event_set(&m_asc_event_flags, EVENT_ASC_STATE_CHANGED);
    }
//lint --e{438}
}


/**@brief Processes a ANT messages while in the off state.
 *
 * @param[in] Pointer to the raw ant message received.
 */
static void process_message_off(uint8_t * p_ant_message)
{
    //There should be no messages to process
}


/**@brief Processes a ANT messages while in the discovery state.
 *
 * @param[in] Pointer to the raw ant message received.
 */
static void process_message_discovery(uint8_t * p_ant_message)
{
    switch (p_ant_message[ANT_MESSAGE_ID_OFFSET])
    {
        case MESG_BROADCAST_DATA_ID:
            //fall-through
        case MESG_ACKNOWLEDGED_DATA_ID:
        {
            //determine whether this device is to be a master or a slave, and continue to the connected state
            if (p_ant_message[ANT_MESSAGE_FLAG_OFFSET] == ANT_EXT_MESG_BITFIELD_DEVICE_ID)
            {
                //Fill the temporary neighbor with the device number and rssi value from the received message
                //page number and payload are not relevant at the moment, so ignore them
                m_neighbor.device_number = p_ant_message[ANT_MESSAGE_EXTENDED0_OFFSET] | (((uint16_t) p_ant_message[ANT_MESSAGE_EXTENDED1_OFFSET]) << 8);

                if (m_neighbor.device_number < m_device_number)
                {
                    m_connection_device_number = (m_neighbor.device_number & 0xFF) | ((m_device_number & 0xFF) << 8);
                    m_ant_connected_parameters.channel_type = CHANNEL_TYPE_MASTER;
                }
                else
                {
                    m_connection_device_number = (m_device_number & 0xFF) | ((m_neighbor.device_number & 0xFF) << 8);
                    m_ant_connected_parameters.channel_type = CHANNEL_TYPE_SLAVE;
                }

                ascmm_set_state(CONNECTED);
            }
            break;
        }
        default:
            break;
    }
}


/**@brief Processes a ANT messages while in the connected state.
 *
 * @param[in] Pointer to the raw ant message received.
 */
static void process_message_connected(uint8_t * p_ant_message)
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
                        //If this is a slave channel, we will need to controll the reverse-direction communication.
                        //Otherwise, the last message loaded into the tx buffer will continue to be transmitted anyway
                        if ( m_retries-- > 0)
                        {
                            err_code = sd_ant_broadcast_message_tx(m_ant_connected_parameters.channel_number, ANT_STANDARD_DATA_PAYLOAD_SIZE, m_tx_buffer);
                            APP_ERROR_CHECK(err_code);
                        }
                        else if (m_ant_connected_parameters.channel_type == CHANNEL_TYPE_MASTER)
                        {
                            memcpy(m_tx_buffer, ascmm_idle_page, ANT_STANDARD_DATA_PAYLOAD_SIZE);
                            err_code = sd_ant_broadcast_message_tx(m_ant_connected_parameters.channel_number, ANT_STANDARD_DATA_PAYLOAD_SIZE, m_tx_buffer);
                            APP_ERROR_CHECK(err_code);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
            break;
        }
        case MESG_BROADCAST_DATA_ID:
            //fall-through
        case MESG_ACKNOWLEDGED_DATA_ID:
        {
            //process by page
            switch (p_ant_message[DECODE_PAGE_ID_BYTE_PHONE_FORMAT])
            {
                case PHONE_COMMAND_PID:
                {
//lint --e{534}
                    asc_decode_phone_command_page(&m_last_received_command, p_ant_message);
                    asc_event_set(&m_asc_event_flags, EVENT_ASC_COMMAND_RECEIVED);
                    break;
                }
                case UPDATE_DATA_PID:
                {
//lint --e{534}
                    asc_decode_phone_update_data_page(&m_last_received_update, p_ant_message);
                    asc_event_set(&m_asc_event_flags, EVENT_ASC_UPDATE_RECEIVED);
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        default:
            break;
    }
}



// Public Functions
void ascmm_init(const asc_ant_params_t * const p_ant_discovery_parameters, const asc_ant_params_t * const p_ant_connection_parameters, uint16_t device_number)
{
    uint32_t err_code;
    m_discovery_channel_number = p_ant_discovery_parameters->channel_number;
    m_device_number = device_number;

    //Assign the slave channel
    err_code = sd_ant_channel_assign(m_discovery_channel_number,
                                     p_ant_discovery_parameters->channel_type,
                                     p_ant_discovery_parameters->network_number,
                                     EXT_PARAM_ALWAYS_SEARCH);
    APP_ERROR_CHECK(err_code);

    //Assign slave channel id.
    err_code = sd_ant_channel_id_set(m_discovery_channel_number,
                                     ANT_WILDCARD_DEVICE_NUMBER,
                                     p_ant_discovery_parameters->device_type,
                                     p_ant_discovery_parameters->tx_type);
    APP_ERROR_CHECK(err_code);

    //Assign slave channel frequency.
    err_code = sd_ant_channel_radio_freq_set(m_discovery_channel_number,
                                             p_ant_discovery_parameters->rf_frequency);
    APP_ERROR_CHECK(err_code);

    //Set the transmission power.
    err_code = sd_ant_channel_radio_tx_power_set(m_discovery_channel_number,
                                                 p_ant_discovery_parameters->tx_power,
                                                 ASC_ANT_CUSTOM_PWR);
    APP_ERROR_CHECK(err_code);

    //Configure search timeouts
    err_code = sd_ant_channel_rx_search_timeout_set(m_discovery_channel_number,
                                                    DISCOVERY_SEARCH_TIMEOUT);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ant_channel_low_priority_rx_search_timeout_set(m_discovery_channel_number,
                                                                 DISCOVERY_LP_SEARCH_TIMEOUT);
    APP_ERROR_CHECK(err_code);

    // Configure proximity search
    // @todo: JP added trailing 0 parameter
    err_code = sd_ant_prox_search_set(m_discovery_channel_number, PROXIMITY_THRESHOLD_2, 0);
    APP_ERROR_CHECK(err_code);

    //Enable extended messages
    err_code = sd_ant_lib_config_set(ANT_LIB_CONFIG_MESG_OUT_INC_DEVICE_ID);
    APP_ERROR_CHECK(err_code);

    //Initialize the connection parameters struct for later use
    //Many of these values are just defaults for now.
    //Ex. channel type will be determined before switching to the connected state)
    m_ant_connected_parameters.channel_number =     p_ant_connection_parameters->channel_number;
    m_ant_connected_parameters.network_number =     p_ant_connection_parameters->network_number;
    m_ant_connected_parameters.p_device_number =    &(m_connection_device_number); //This will change over the course of discovery
    m_ant_connected_parameters.device_type =        p_ant_connection_parameters->device_type;
    m_ant_connected_parameters.tx_type =            p_ant_connection_parameters->tx_type;
    m_ant_connected_parameters.channel_period =     p_ant_connection_parameters->channel_period;
    m_ant_connected_parameters.channel_type =       p_ant_connection_parameters->channel_type;
    m_ant_connected_parameters.rf_frequency =       p_ant_connection_parameters->rf_frequency;
    m_ant_connected_parameters.tx_power =           p_ant_connection_parameters->tx_power;
}


void ascmm_turn_on(void)
{
    ascmm_set_state(DISCOVERY);
}


void ascmm_handle_ant_event(uint8_t event, uint8_t * p_event_message_buffer)
{
    switch (m_state)
    {
        case ASCMM_OFF:
        {
            process_message_off(p_event_message_buffer);
            break;
        }
        case DISCOVERY:
        {
            process_message_discovery(p_event_message_buffer);
            break;
        }
        case CONNECTED:
        {
            process_message_connected(p_event_message_buffer);
            break;
        }
        default:
            break;
    }
}


void ascmm_relay_message(uint8_t * p_tx_buffer, uint8_t retries)
{
    if (m_state == CONNECTED)
    {
        uint32_t err_code;

        m_retries = retries;
        memcpy(m_tx_buffer, p_tx_buffer, ANT_STANDARD_DATA_PAYLOAD_SIZE);

        err_code = sd_ant_broadcast_message_tx(m_ant_connected_parameters.channel_number, ANT_STANDARD_DATA_PAYLOAD_SIZE, m_tx_buffer);
        APP_ERROR_CHECK(err_code);
    }
}


ascmm_states_t ascmm_state_get(void)
{
    return m_state;
}


uint32_t ascmm_events_get(void)
{
    return m_asc_event_flags;
}


void ascmm_event_clear(uint32_t event)
{
    asc_event_clear(&m_asc_event_flags, event);
}


uint16_t ascmm_get_neighbor_id(void)
{
    return m_neighbor.device_number;
}

asc_command_data_t ascmm_get_last_command(void)
{
    return m_last_received_command;
}


asc_update_data_t ascmm_get_last_update(void)
{
    return m_last_received_update;
}
/** @} **/
