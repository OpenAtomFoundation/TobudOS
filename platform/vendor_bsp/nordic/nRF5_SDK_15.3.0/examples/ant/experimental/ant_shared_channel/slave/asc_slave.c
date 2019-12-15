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
 * @defgroup asc_slave module
 * @{
 * @ingroup ant_auto_shared_channel
 *
 * @brief ANT Auto Shared Channel (ASC) Slave implementations.
 */

#include "asc_slave.h"
#include <stdint.h>
#include <string.h>
#include "ant_interface.h"
#include "ant_parameters.h"
#include "app_error.h"
#include "asc_events.h"
#include "asc_parameters.h"
#include "nrf_soc.h"
#include "nrf.h"
#include "asc_pages.h"

// Private Definitions
#define ANT_SEARCH_TIMEOUT          ((uint8_t) 120)         /**< The ANT channel search timeout that will be used when discovering an ASC master. */

#define MODEL_NUMBER                ((uint16_t) 1)          /**< The model number of this ASC Slave. */
#define HW_REVISION                 ((uint8_t) 1)           /**< The hardware revision number of this ASC Slave. */
#define SW_REVISION                 ((uint8_t) 1)           /**< The software revision number of this ASC Slave. */

#define WAITING_TIMEOUT             ((uint8_t) 3)           /**< The maximum amount of time to stay in the WAITING state before falling back to the SEARCHING state. */
#define CONFIRMING_TIMEOUT          ((uint8_t) 2)           /**< The maximum amount of time to stay in the CONFIRMING state before falling back to the SEARCHING state. */
#define DEFAULT_BACKOFF_RANGE       ((uint8_t) 100)         /**< The maximum backoff value that an ASC Slave will employ if no valid backoff range is given by the ASC Master. */
#define MIN_BACKOFF                 ((uint8_t) 2)           /**< The minimum number of message periods to wait until attempting to register with a master. */


// Private Variables
static uint32_t                 m_timeout_counter = 0;      /**< The counter used to determine timeouts. Incremented using @link ascs_increment_timer. */
static uint32_t                 m_serial_number;            /**< The serial number of this slave device. */
static uint8_t                  m_tx_buffer[ANT_STANDARD_DATA_PAYLOAD_SIZE] = {0};   /**< The buffer used for normal transmission. */
static uint8_t                  m_response_buffer[ANT_STANDARD_DATA_PAYLOAD_SIZE] = {0};   /**< The buffer used for responding to data page requests. */
static uint8_t                  m_backoff_range = 0;        /**< The maximum number of message periods to wait until attempting to register with a master (changes depending on how long the device has been waiting to register). */
static uint8_t                  m_backoff = 0;              /**< The number of message periods the slave device. */
                                                            /**< should backoff for before attempting to register with a master. */
static uint8_t                  m_data_timeout;             /**< Data timeout specified by master. */

#ifdef TWO_BYTE_SHARED_ADDRESS
    static uint16_t             m_shared_address;           /**< The shared address assigned to the slave device (two byte version). */
#else
    static uint8_t              m_shared_address;           /**< The shared address assigned to the slave device (one byte version). */
#endif
static uint16_t                 m_groups_assigned = NO_GROUPS;  /**< Bitfield for storing groups currently assigned to. */

static uint8_t                  m_channel_number;           /**< The channel number that the ASC Slave is to use. */
static ascs_states_t            m_state = OFF;              /**< The current state of the ASC Slave. */
static asc_slave_states_t       m_light_state = SLAVE_STATE_OFF;    /**< The current state of the ASC Slave's light. */
static asc_request_data_t       m_last_received_request;    /**< The details of the last request that the ASC Slave has received. */
static uint32_t                 m_asc_event_flags = 0;      /**< Bitfield describing the current events that may need attention by the main application. */


// Private Functions
/**@brief Sets the state machine to the specified state.
 */
static void ascs_set_state(ascs_states_t new_state)
{
    uint32_t err_code;

    if (m_state != new_state)
    {
        switch (new_state)
        {
            case OFF:
            {
                m_state = new_state;
                err_code = sd_ant_channel_close(m_channel_number);
                APP_ERROR_CHECK(err_code);
                break;
            }
            case SEARCHING:
            {
                //Check if we need to re-open the channel
                if (m_state == OFF)
                {
                    //Open slave channel
                    err_code = sd_ant_channel_open(m_channel_number);
                    APP_ERROR_CHECK(err_code);
                }

                m_state = new_state;
                m_groups_assigned = NO_GROUPS;
                m_shared_address = SEARCH_ADDRESS;
                asc_encode_set_shared_address(m_shared_address, m_tx_buffer);
                err_code = sd_ant_broadcast_message_tx (m_channel_number,  ANT_STANDARD_DATA_PAYLOAD_SIZE, m_tx_buffer);
                APP_ERROR_CHECK(err_code);
                break;
            }
            case REQUESTING:
            {
                if (m_state != SEARCHING)
                {
                    asc_event_set(&m_asc_event_flags, EVENT_ASC_DEVICE_IN_WRONG_STATE);
                    return;
                }
                m_state = new_state;
                asc_encode_request_address_page(m_serial_number, m_tx_buffer);
                err_code = sd_ant_acknowledge_message_tx(m_channel_number,  ANT_STANDARD_DATA_PAYLOAD_SIZE, m_tx_buffer);
                APP_ERROR_CHECK(err_code);

                break;
            }
            case WAITING:
            {
                if (m_state != REQUESTING)
                {
                    asc_event_set(&m_asc_event_flags, EVENT_ASC_DEVICE_IN_WRONG_STATE);
                    return;
                }
                m_state = new_state;
                m_shared_address = REGISTER_ADDRESS;
                asc_encode_set_shared_address(m_shared_address, m_tx_buffer);
                err_code = sd_ant_broadcast_message_tx (m_channel_number,  ANT_STANDARD_DATA_PAYLOAD_SIZE, m_tx_buffer);
                APP_ERROR_CHECK(err_code);
                break;
            }
            case CONFIRMING:
            {
                if (m_state != WAITING)
                {
                    asc_event_set(&m_asc_event_flags, EVENT_ASC_DEVICE_IN_WRONG_STATE);
                    return;
                }
                m_state = new_state;
                m_timeout_counter = 0;
                asc_confirm_acquire_paramters_t params = {
                    .serial_number = m_serial_number,
                    .model_number = MODEL_NUMBER,
                    .hw_revision = HW_REVISION,
                    .sw_revision = SW_REVISION
                };
                asc_encode_confirm_acquire_page(params, m_tx_buffer);
                err_code = sd_ant_acknowledge_message_tx(m_channel_number,  ANT_STANDARD_DATA_PAYLOAD_SIZE, m_tx_buffer);
                APP_ERROR_CHECK(err_code);
                break;
            }
            case ASSIGNED:
            {
                if (m_state != CONFIRMING)
                {
                    asc_event_set(&m_asc_event_flags, EVENT_ASC_DEVICE_IN_WRONG_STATE);
                    return;
                }
                ascs_states_t previous_state = m_state;
                m_state = new_state;
                m_timeout_counter = 0;
                if (previous_state == CONFIRMING)
                {
                    asc_encode_set_shared_address(m_shared_address, m_tx_buffer);
                    err_code = sd_ant_broadcast_message_tx (m_channel_number,  ANT_STANDARD_DATA_PAYLOAD_SIZE, m_tx_buffer);
                    APP_ERROR_CHECK(err_code);
                }
                break;
            }
            default:
            {
                asc_event_set(&m_asc_event_flags, EVENT_ASC_DEVICE_IN_WRONG_STATE);
                return;
            }
        }
        asc_event_set(&m_asc_event_flags, EVENT_ASC_STATE_CHANGED);
    }
}


/** @brief Function to decode and handle a command message.
*/
static void ascs_process_command(uint8_t * p_event_message_buffer)
{
    uint32_t err_code;

    asc_command_data_t command_data;
    err_code = asc_decode_command_page(&command_data, p_event_message_buffer);
    APP_ERROR_CHECK(err_code);

    switch (command_data.command)
    {
        case TURN_OFF:
        {
            if (command_data.group_number == NO_GROUPS ||
               (GROUP_SET << command_data.group_number) & m_groups_assigned)
            {
                m_light_state = SLAVE_STATE_OFF;
            }
            break;
        }
        case TURN_ON:
        {
            if (command_data.group_number == NO_GROUPS ||
               (GROUP_SET << command_data.group_number) & m_groups_assigned)
            {
                m_light_state = SLAVE_STATE_ON;
            }
            break;
        }
        case ASSIGN_TO_GROUP:
        {
            m_groups_assigned = GROUP_SET << command_data.group_number;
            break;
        }
        case ASSIGN_TO_ADDITIONAL_GROUP:
        {
            m_groups_assigned |= GROUP_SET << command_data.group_number;
            break;
        }
        case ASSIGN_TO_ALL_GROUPS:
        {
            m_groups_assigned = ALL_GROUPS;
            break;
        }
        case CLEAR_FROM_GROUP:
        {
//lint --e{502}
            m_groups_assigned &= ~(GROUP_SET << command_data.group_number);
            break;
        }
        case CLEAR_FROM_ALL_GROUPS:
        {
            m_groups_assigned = NO_GROUPS;
            break;
        }
        default:
        {
            break;
        }
    }

    asc_event_set(&m_asc_event_flags, EVENT_ASC_LIGHT_STATE_CHANGED);
}

/**@brief Processes a ANT messages while in the off state.
 *
 * @param[in] Pointer to the raw ant message received.
 */
static void ascs_process_message_off(uint8_t event, uint8_t * p_event_message_buffer)
{
    //Do nothing
}


/** @brief Processes a ANT messages while in the searching state.
 *
 * @param[in] Pointer to the raw ant message received.
 */
static void ascs_process_message_searching(uint8_t event, uint8_t * p_event_message_buffer)
{
    uint32_t err_code;
    switch (event)
    {
        case EVENT_RX:
        {
            //Check if we are set to backoff
            if (m_backoff == 0)
            {
                //Process by page
                if (p_event_message_buffer[DECODE_PAGE_ID_BYTE] == ADDRESS_AVAILABLE_PID)
                {
                    asc_addr_available_paramters_t addr_avail_params;
                    err_code = asc_decode_address_available_page(&addr_avail_params, p_event_message_buffer);
                    APP_ERROR_CHECK(err_code);

                    if (addr_avail_params.is_address_available)
                    {
                        m_backoff_range = addr_avail_params.backoff_range;
                        if (m_backoff_range == INVALID_BACKOFF_RANGE)
                        {
                            m_backoff_range = DEFAULT_BACKOFF_RANGE;
                        }

                        //Match the message period of the hub
                        err_code = sd_ant_channel_period_set(m_channel_number, asc_get_counts_from_period_enum(addr_avail_params.channel_period));
                        APP_ERROR_CHECK(err_code);

                        //Multiply data timeout by 2 to get total timeout in seconds
                        m_data_timeout = addr_avail_params.data_timeout;

                        ascs_set_state(REQUESTING);
                    }
                    else
                    {
                        /**@todo decide what to do in this case? off state? */
                    }
                }
            }
            else
            {
                m_backoff--;
            }
            break;
        }
        case EVENT_CHANNEL_CLOSED:
        {
            ascs_set_state(OFF);
            break;
        }
        default:
        {
            break;
        }
    }
}


/** @brief Processes a ANT messages while in the requesting state.
 *
 * @param[in] Pointer to the raw ant message received.
 */
static void ascs_process_message_requesting(uint8_t event, uint8_t * p_event_message_buffer)
{
    uint32_t err_code;
    switch (event)
    {
        case EVENT_TRANSFER_TX_FAILED:
        {
            //Randomly get a backoff between MIN_BACKOFF to m_backoff_range
            uint8_t num_rand_bytes_available;
            err_code = sd_rand_application_bytes_available_get(&num_rand_bytes_available);
            APP_ERROR_CHECK(err_code);

            //update the backoff if there is a randon number available
            if (num_rand_bytes_available > 0)
            {
                uint8_t rand_number;
                err_code = sd_rand_application_vector_get(&rand_number, 1);
                APP_ERROR_CHECK(err_code);

                m_backoff = (uint8_t) (rand_number % m_backoff_range + MIN_BACKOFF);
            }

            ascs_set_state(SEARCHING);
            break;
        }
        case EVENT_TRANSFER_TX_COMPLETED:
        {
            ascs_set_state(WAITING);
            break;
        }
        case EVENT_CHANNEL_CLOSED:
        case EVENT_RX: //Intentional fallthrough
        default:
        {
            break;
        }
    }
}


/**@brief Processes a ANT messages while in the waiting state.
 *
 * @param[in] Pointer to the raw ant message received.
 */
static void ascs_process_message_waiting(uint8_t event, uint8_t * p_event_message_buffer)
{
    uint32_t err_code;

    switch (event)
    {
        case EVENT_RX:
        {
            //Process by page
            switch (p_event_message_buffer[DECODE_PAGE_ID_BYTE])
            {
                case BUSY_ACQUIRING_PID:
                {
                    bool do_serial_numbers_match;
                    err_code = asc_decode_busy_acquiring_page(m_serial_number, &do_serial_numbers_match, &m_shared_address, p_event_message_buffer);
                    APP_ERROR_CHECK(err_code);

                    if (do_serial_numbers_match)
                    {
                        ascs_set_state(CONFIRMING);
                    }
                    else //There was no match
                    {
                        ascs_set_state(SEARCHING);
                    }
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        case EVENT_CHANNEL_CLOSED:
        default:
        {
            break;
        }
    }
}


/** @brief Processes a ANT messages while in the confirming state.
 *
 * @param[in] Pointer to the raw ant message received.
 */
static void ascs_process_message_confirming(uint8_t event, uint8_t * p_event_message_buffer)
{
    switch (event)
    {
        case EVENT_TRANSFER_TX_FAILED:
        {
            int32_t err_code;
            //Re-send the Confirm Acquire page (should still be loaded in the tx buffer).
            err_code = sd_ant_acknowledge_message_tx(m_channel_number,  ANT_STANDARD_DATA_PAYLOAD_SIZE, m_tx_buffer);
            APP_ERROR_CHECK(err_code);
            break;
        }
        case EVENT_TRANSFER_TX_COMPLETED:
        {
            ascs_set_state(ASSIGNED);
            break;
        }
        case EVENT_CHANNEL_CLOSED:
        case EVENT_RX: //Intentional fallthrough
        default:
        {
            break;
        }
    }
}


/**@brief Processes a ANT messages while in the assigned state.
 *
 * @param[in] Pointer to the raw ant message received.
 */
static void ascs_process_message_assigned(uint8_t event, uint8_t * p_event_message_buffer)
{
    uint32_t err_code;
    switch (event)
    {
        case EVENT_RX:
        {
            m_timeout_counter = 0;
            switch (p_event_message_buffer[DECODE_PAGE_ID_BYTE])
            {
                case REQUEST_DATA_PID:
                {
                    err_code = asc_decode_request_data_page(&m_last_received_request, p_event_message_buffer);
                    APP_ERROR_CHECK(err_code);
                    //if the request is for the update data page, handle it automatically.
                    //Otherwise, notify the application that the request came in
                    if (m_last_received_request.page_id_requested == UPDATE_DATA_PID)
                    {
                        asc_update_data_t update = {
                            .shared_address = m_shared_address,
                            .state = m_light_state
                        };
                        asc_encode_update_data_page(update, m_response_buffer);
                        err_code = sd_ant_broadcast_message_tx(m_channel_number,  ANT_STANDARD_DATA_PAYLOAD_SIZE, m_response_buffer);
                        APP_ERROR_CHECK(err_code);
                    }
                    else
                    {
                        asc_event_set(&m_asc_event_flags, EVENT_ASC_REQUEST_RECEIVED);
                    }
                    break;
                }
                case COMMAND_PID:
                {
                   ascs_process_command(p_event_message_buffer);
                    break;
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
}


// Public Functions
void ascs_init(const asc_ant_params_t * const ant_parameters)
{
    uint32_t    err_code;

    m_asc_event_flags = 0;
    m_channel_number = ant_parameters->channel_number;
    m_serial_number = (uint32_t)NRF_FICR->DEVICEID[0];
    m_serial_number &= (NRF_FICR->DEVICEID[0] << 8);

    err_code = sd_ant_channel_assign(m_channel_number,
                                     CHANNEL_TYPE_SHARED_SLAVE,
                                     ant_parameters->network_number,
                                     ASC_ANT_EXT_ASSIGNMENT);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ant_channel_id_set(m_channel_number,
                                     *(ant_parameters->p_device_number),
                                     ant_parameters->device_type,
                                     ant_parameters->tx_type);
    APP_ERROR_CHECK(err_code);

    //Assign slave channel frequency
    err_code = sd_ant_channel_radio_freq_set(m_channel_number, ant_parameters->rf_frequency);
    APP_ERROR_CHECK(err_code);

    //Assign slave channel message period
    err_code = sd_ant_channel_period_set(m_channel_number,
                                        asc_get_counts_from_period_enum(ant_parameters->channel_period));
    APP_ERROR_CHECK(err_code);

    //Set the transmission power
    err_code = sd_ant_channel_radio_tx_power_set(m_channel_number, ant_parameters->tx_power, ASC_ANT_CUSTOM_PWR);
    APP_ERROR_CHECK(err_code);

    //Assign the search timeout
    err_code = sd_ant_channel_rx_search_timeout_set(m_channel_number, ANT_SEARCH_TIMEOUT);
    APP_ERROR_CHECK(err_code);

    // Configure proximity search
    // @todo: JP added trailin 0 paramter
    err_code = sd_ant_prox_search_set(m_channel_number, PROXIMITY_THRESHOLD_5, 0);
    APP_ERROR_CHECK(err_code);
}


void ascs_turn_on(void)
{
    ascs_set_state(SEARCHING);
}


void ascs_handle_ant_event(uint8_t event, uint8_t * p_event_message_buffer)
{
    switch (m_state)
    {
        case OFF:
            ascs_process_message_off(event, p_event_message_buffer);
            break;
        case SEARCHING:
            ascs_process_message_searching(event, p_event_message_buffer);
            break;
        case REQUESTING:
            ascs_process_message_requesting(event, p_event_message_buffer);
            break;
        case WAITING:
            ascs_process_message_waiting(event, p_event_message_buffer);
            break;
        case CONFIRMING:
            ascs_process_message_confirming(event, p_event_message_buffer);
            break;
        case ASSIGNED:
            ascs_process_message_assigned(event, p_event_message_buffer);
            break;
        default:
            break;
    }
}


void ascs_send_data(uint8_t * p_data)
{
}


ascs_states_t ascs_state_get(void)
{
    return m_state;
}


asc_slave_states_t ascs_light_state_get(void)
{
    return m_light_state;
}


uint32_t ascs_events_get(void)
{
    return m_asc_event_flags;
}


void ascs_event_clear(uint32_t event)
{
   asc_event_clear(&m_asc_event_flags, event);
}


void ascs_increment_timer(void)
{
    //Check the timeout counter based on current state
    switch (m_state)
    {
        case REQUESTING:
        {
            break;
        }
        case WAITING:
        {
            if (++m_timeout_counter >= WAITING_TIMEOUT)
            {
                ascs_set_state(SEARCHING);
            }
            break;
        }
        case CONFIRMING:
        {
            if (++m_timeout_counter >= CONFIRMING_TIMEOUT)
            {
                ascs_set_state(SEARCHING);
            }
            break;
        }
        case ASSIGNED:
        {
            if (m_data_timeout != DATA_TIMEOUT_DISABLED && ++m_timeout_counter >= m_data_timeout)
            {
                ascs_set_state(SEARCHING);
            }
            break;
        }
        case SEARCHING:
        case OFF: //Intentional fallthroughs
        default:
        {
            break;
        }
    }
}


asc_request_data_t ascs_get_last_request(void)
{
    return m_last_received_request;
}


void ascs_send_response(uint8_t * response_data_buffer)
{
    uint32_t err_code;
    uint32_t array_size = sizeof(response_data_buffer) / sizeof(response_data_buffer[0]);


    if (array_size != ANT_STANDARD_DATA_PAYLOAD_SIZE)
    {
        err_code = NRF_ERROR_INVALID_PARAM;
    }
    else
    {
        memcpy(m_response_buffer, response_data_buffer, ANT_STANDARD_DATA_PAYLOAD_SIZE);
        err_code = sd_ant_broadcast_message_tx(m_channel_number, ANT_STANDARD_DATA_PAYLOAD_SIZE, m_response_buffer);
    }

    APP_ERROR_CHECK(err_code);
}

/** @} */
