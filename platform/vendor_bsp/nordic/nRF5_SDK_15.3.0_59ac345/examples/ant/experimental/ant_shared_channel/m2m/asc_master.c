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
 * @defgroup asc_master module
 * @{
 * @ingroup ant_auto_shared_channel
 *
 * @brief ANT Auto Shared Channel (ASC) Master implementation.
 */


#include "asc_master.h"
#include <stdint.h>
#include "ant_interface.h"
#include "ant_parameters.h"
#include "app_error.h"
#include "asc_events.h"
#include "asc_parameters.h"
#include "nrf_soc.h"
#include "asc_pages.h"
#include "asc_device_registry.h"

// Handshaking and timeout definitions
#define BACKOFF_RANGE                       ((uint8_t) 10)  /**< Upper limit of the number of address available pages that an ASC Slave should wait for before retrying an address request. */
#define DATA_TIMEOUT                        ((uint8_t) 15)  /**< Length of time (in seconds) that an ASC Slave should wait since the last message it specifically received before clearing its shared address. */
#define MAX_MISSED_POLL_MESSAGES            ((uint8_t) 8)   /**< Upper limit of the number of retries that an ASC Master should attempt while polling before considering an ASC Slave to no longer be present. */
#define MIN_POLLS                           ((uint8_t) 1)   /**< Minimum number of acknowledgements that an ACS Master must receive from an ASC Slave before progressing through the POLLING state. */
#define POLLING_REQUEST_NUMBER_RESPONSES    ((uint8_t) 1)   /**< Number of responses requested by the ASC Master when polling a slave. */
#define MAX_TX_MESSAGES_ADDRESS_AVAILABLE   ((uint8_t) 10)  /**< Maximum number of address available messages to send before switching to the POLLING state (if there are any registered ASC Slaves). */
#define MAX_TX_MESSAGES_HANDSHAKING         ((uint8_t) 12)  /**< Maximum number of busy aquiring pages to send before returning to the ADDRESS AVAILABLE state. This is used to avoid being stuck in the HANDSHAKING state for any reason. */
#define MAX_TX_MESSAGES_SENDING_COMMAND     ((uint8_t) 2)   /**< Maximum number of acknowledgements to the command page that the ASC Master must receive before returning to the ADDRESS AVAILABLE state, unless the maximum number of retries is reached first. */

// Private Variables
static uint8_t                  m_channel_number;                                   /**< The channel number that the ASC Master is to use. */
static asc_message_periods_t    m_channel_period;                                   /**< The channel period to be used by the ASC Master. This is an enumeration value and is not in units of clock ticks. */
static asc_device_registry_t    m_device_registry;                                  /**< Keeps a record of the slave devices that are currently registered with the master. */
static asc_device_registry_t*   pm_device_registry = &m_device_registry;            /**< Pointer to m_device_registry. */
static asc_device_t             m_handshaking_device;                               /**< Stores the info of the device we are currently handshaking with. */
static bool                     m_is_address_available;                             /**< Indicates whether or not the ASC Master can accept more ASC Slaves. */
static ascm_states_t            m_state = ASCM_OFF;                                 /**< Indicates the current state of the ASC Master. */
static ascm_states_t            m_previous_state = ASCM_OFF;                        /**< Indicates the state that the master should return to after servicing a command. */
static uint8_t                  m_msg_counter_address_available = 0;                /**< Tx Message counter for the ADDRESS AVAILABLE state. */
static uint8_t                  m_msg_counter_handshaking = 0;                      /**< Tx Message counter for the HANDSHAKING state. */
static uint8_t                  m_msg_counter_polling = 0;                          /**< Tx Message counter for the POLLING state. */
static uint8_t                  m_msg_counter_sending_command = 0;                  /**< Tx Message counter for the SENDING_COMMAND state. */
static asc_device_t*            pm_device;                                          /**< Temporary device pointer. */
static asc_update_data_t        m_last_received_udpate;                             /**< */
static uint8_t                  m_retries;                                          /**< */
static bool                     m_send_command_as_ack;                              /**< */
static asc_command_data_t       m_current_command_data;                                  /**< The current shared address and command being sent.*/
#ifdef TWO_BYTE_SHARED_ADDRESS
static uint16_t                 m_current_shared_address_polling;                   /**< The current shared address being polled. */
#else
static uint8_t                  m_current_shared_address_polling;                   /**< The current shared address being polled. */
#endif
static uint32_t                 m_asc_event_flags = 0;                              /**< Bitfield used to indicate that an event has occured. */
static uint8_t                  m_tx_buffer[ANT_STANDARD_DATA_PAYLOAD_SIZE] = {0};  /**< Buffer to use when setting ANT transmission values. Defined at module scope to allow retrying the same data easily if needed. */
static bool                     m_is_command_pending = false;                       /**< Flag to indicate that a command is waiting to be serviced while in the polling state. */

// Private Functions
/**@brief Encodes and transmits the Address Available page.
 */
static __INLINE void ascm_send_address_available_page(void)
{
    uint32_t err_code;

    asc_addr_available_paramters_t addr_avail_params = {
        .channel_period = m_channel_period,
        .backoff_range = BACKOFF_RANGE,
        .data_timeout = DATA_TIMEOUT,
        .is_address_available = m_is_address_available
    };
    asc_encode_address_available_page(addr_avail_params, m_tx_buffer);

    err_code = sd_ant_broadcast_message_tx(m_channel_number,
                                            ANT_STANDARD_DATA_PAYLOAD_SIZE,
                                            m_tx_buffer);
    APP_ERROR_CHECK(err_code);
}


/**@brief Encodes and transmits the Busy Acquiring page.
 */
static __INLINE void ascm_send_busy_acquiring_page(void)
{
    uint32_t err_code;

    asc_encode_busy_acquiring_page(m_handshaking_device.shared_address,
                                   m_handshaking_device.serial_number,
                                   m_tx_buffer);

    err_code = sd_ant_broadcast_message_tx(m_channel_number,
                                            ANT_STANDARD_DATA_PAYLOAD_SIZE,
                                            m_tx_buffer);
    APP_ERROR_CHECK(err_code);
}

static __INLINE void send_polling_message(void)
{
    uint32_t err_code;
    asc_request_data_t request = {
        m_current_shared_address_polling,
        UPDATE_DATA_PID
    };

    asc_encode_request_data_page(request, m_tx_buffer);

    err_code = sd_ant_acknowledge_message_tx(m_channel_number, ANT_STANDARD_DATA_PAYLOAD_SIZE, m_tx_buffer);
    APP_ERROR_CHECK(err_code);
}

/**@brief Sets the state machine to the specified state.
 */
static void ascm_set_state(ascm_states_t new_state)
{
    uint32_t err_code;

    if (new_state != m_state)
    {
        switch (new_state)
        {
            case ASCM_OFF:
            {
                //Close the channel
                m_previous_state = m_state;
                m_state = new_state;
                err_code = sd_ant_channel_close(m_channel_number);
                APP_ERROR_CHECK(err_code);
                break;
            }
            case ADDRESS_AVAILABLE:
            {
                if (m_state == ASCM_OFF)
                {
                    //Open the channel
                    err_code = sd_ant_channel_open(m_channel_number);
                    APP_ERROR_CHECK(err_code);
                }
                m_previous_state = m_state;
                m_state = new_state;
                m_is_address_available = !deviceregistry_is_full(pm_device_registry);
                ascm_send_address_available_page();
                break;
            }
            case HANDSHAKING:
            {
                if (m_state != ADDRESS_AVAILABLE)
                {
                    asc_event_set(&m_asc_event_flags, EVENT_ASC_DEVICE_IN_WRONG_STATE);
                    return;
                }

                m_previous_state = m_state;
                m_state = new_state;
                //there is no need to check for validity of this shared address, as there must be a free address available
                //to be in the handshaking state at all.
                m_handshaking_device.shared_address = deviceregistry_get_next_free_shared_address(pm_device_registry);
                m_msg_counter_handshaking = 0;
                ascm_send_busy_acquiring_page();
                break;
            }
            case POLLING:
            {
                if (m_state != ADDRESS_AVAILABLE && m_state != SENDING_COMMAND)
                {
                    asc_event_set(&m_asc_event_flags, EVENT_ASC_DEVICE_IN_WRONG_STATE);
                    return;
                }

                m_previous_state = m_state;
                m_state = new_state;

                //If the previous state was not SENDING_COMMAND, start from the beginning of the polling cycle.
                //Otherwise, allow the process_message handler to finish the cycle
                if (m_previous_state != SENDING_COMMAND)
                {
                    m_current_shared_address_polling = deviceregistry_get_first_registered_shared_address(pm_device_registry);
                    if (m_current_shared_address_polling != INVALID_SHARED_ADDRESS)
                    {
                        m_msg_counter_polling = 0;
                        send_polling_message();
                    }
                    else
                    {
                        //no devices are registered, go back to address available
                        ascm_set_state(ADDRESS_AVAILABLE);
                    }
                }
                break;
            }
            case SENDING_COMMAND:
            {
                //Only allow sending commands if the device is in the
                //Address available or polling states
                if (m_state != ADDRESS_AVAILABLE && m_state != POLLING)
                {
                    asc_event_set(&m_asc_event_flags, EVENT_ASC_DEVICE_IN_WRONG_STATE);
                    return;
                }

                m_previous_state = m_state;
                m_state = new_state;
                m_msg_counter_sending_command = 0;

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
}



/**@brief Processes a device update message and sets the update received event flag.
 *
 * @param[in] Pointer to the raw ant message received.
 */
static void process_device_update(uint8_t * p_ant_message)
{
    uint32_t err_code;

    err_code = asc_decode_update_data_page(&m_last_received_udpate, p_ant_message);
    APP_ERROR_CHECK(err_code);

    asc_event_set(&m_asc_event_flags, EVENT_ASC_UPDATE_RECEIVED);
}


/**@brief Processes an ANT messages while in the off state.
 *
 * @param[in] Pointer to the raw ant message received.
 */
static void process_message_off(uint8_t * p_ant_message)
{
    //There should be no messages to process
}


/**@brief Processes an ANT messages while in the address available state.
 *
 * @param[in] Pointer to the raw ant message received.
 */
static void process_message_address_available(uint8_t * p_ant_message)
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
                        if (m_is_command_pending == true)
                        {
                            ascm_set_state(SENDING_COMMAND);
                        }
                        else if (m_device_registry.count > 0 && ++m_msg_counter_address_available > MAX_TX_MESSAGES_ADDRESS_AVAILABLE)
                        {
                            m_msg_counter_address_available = 0;
                            ascm_set_state(POLLING);
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
            switch (p_ant_message[DECODE_PAGE_ID_BYTE])
            {
                case REQUEST_ADDRESS_PID:
                {
                    if (m_is_address_available)
                    {
                        uint32_t* p_serial_number = &(m_handshaking_device.serial_number);

                        err_code = asc_decode_request_address_page(p_serial_number, p_ant_message);
                        APP_ERROR_CHECK(err_code);

                        ascm_set_state(HANDSHAKING);
                    }
                    break;
                }
                case UPDATE_DATA_PID:
                {
                    process_device_update(p_ant_message);
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


/**@brief Processes an ANT messages while in the handshaking state.
 *
 * @param[in] Pointer to the raw ant message received.
 */
static void process_message_handshaking(uint8_t * p_ant_message)
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
                    if (++m_msg_counter_handshaking > MAX_TX_MESSAGES_HANDSHAKING)
                    {
                        m_msg_counter_handshaking = 0;
//lint --e{534}
                        deviceregistry_remove_device(pm_device_registry, m_handshaking_device.shared_address);
                        ascm_set_state(ADDRESS_AVAILABLE);
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        }
        case MESG_BROADCAST_DATA_ID:
            //fall-through
        case MESG_ACKNOWLEDGED_DATA_ID:
            //Process by page
            switch (p_ant_message[DECODE_PAGE_ID_BYTE])
            {
                case CONFIRM_ACQUIRE_PID:
                {
                    asc_confirm_acquire_paramters_t params;
                    bool is_from_handshaking_device;
                    err_code = asc_decode_confirm_acquire_page(m_handshaking_device.serial_number,
                                                               &is_from_handshaking_device,
                                                               &params,
                                                               p_ant_message);
                    APP_ERROR_CHECK(err_code);

                    //Confirm that it is the correct device
                    if (is_from_handshaking_device)
                    {
                        m_handshaking_device.serial_number = params.serial_number;
                        m_handshaking_device.model_number = params.model_number;
                        m_handshaking_device.hw_revision = params.hw_revision;
                        m_handshaking_device.sw_revision = params.sw_revision;
//lint --e{534}
                        deviceregistry_add_device(pm_device_registry, &m_handshaking_device);

                        ascm_set_state(ADDRESS_AVAILABLE); //This ensures that after requesting the page we are returned back to Address Available
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        default:
            break;
    }
}


/**@brief Processes an ANT messages while in the handshaking state.
 *
 * @param[in] Pointer to the raw ant message received.
 */
static void process_message_polling(uint8_t* p_ant_message)
{
    switch (p_ant_message[ANT_MESSAGE_ID_OFFSET])
    {
        case MESG_RESPONSE_EVENT_ID:
        {
            if (p_ant_message[ANT_MESSAGE_DATA0_OFFSET] == MESG_EVENT_ID)   //Channel event
            {
                switch (p_ant_message[ANT_MESSAGE_DATA1_OFFSET])
                {
                    case EVENT_TX:
                        //Intentional fallthrough to allow the master to continue polling once it has returned from the SENDING_COMMAND state
                    case EVENT_TRANSFER_TX_COMPLETED:
                    {
                        pm_device = deviceregistry_get_device(&m_device_registry, m_current_shared_address_polling);
                        pm_device->missed_polls = 0;

                        if (++m_msg_counter_polling >= MIN_POLLS)
                        {
                            //If a command is waiting to be serviced, this is the opportunity to service that command
                            //the sending command state will automatically return the master to the polling state when
                            //the command is complete
                            if (m_is_command_pending == true)
                            {
                                ascm_set_state(SENDING_COMMAND);
                            }
                            else
                            {
                                //The slave as acknowledged the polling message, so move on to the next slave in the registry.
                                m_current_shared_address_polling = deviceregistry_get_next_registered_shared_address(&m_device_registry, m_current_shared_address_polling);
                                //Check if we have finished polling over all devices
                                if (m_current_shared_address_polling == INVALID_SHARED_ADDRESS)
                                {
                                    //Switch back to Address Available
                                    ascm_set_state(ADDRESS_AVAILABLE);
                                }
                                else
                                {
                                    m_msg_counter_polling = 0;
                                    send_polling_message();
                                }
                            }
                        }
                        else
                        {
                            send_polling_message();
                        }
                        break;
                    }
                    case EVENT_TRANSFER_TX_FAILED:
                    {
                        //Increment the missed poll cycle counter for the device
                        pm_device = deviceregistry_get_device(&m_device_registry, m_current_shared_address_polling);
                        //Check if the device has timed out and we need to de-register it
                        if (++(pm_device->missed_polls) > MAX_MISSED_POLL_MESSAGES)
                        {
//lint --e{534}
                            deviceregistry_remove_device(&m_device_registry, m_current_shared_address_polling);

                            m_current_shared_address_polling = deviceregistry_get_next_registered_shared_address(&m_device_registry, m_current_shared_address_polling);
                            //Check if we have finished polling over all devices
                            if (m_current_shared_address_polling == INVALID_SHARED_ADDRESS)
                            {
                                //Switch back to Address Available
                                ascm_set_state(ADDRESS_AVAILABLE);
                                break;
                            }
                        }
                        send_polling_message();

                        break;
                    }
                    default:
                        break;
                }
            }
            break;
        }
        case MESG_ACKNOWLEDGED_DATA_ID:
            //fall-through
        case MESG_BROADCAST_DATA_ID:
        {
            if (p_ant_message[ANT_MESSAGE_DATA0_OFFSET] == pm_device->shared_address)
            {
                pm_device->missed_polls = 0;
            }

            //Process by page
            switch (p_ant_message[DECODE_PAGE_ID_BYTE])
            {
                case UPDATE_DATA_PID:
                {
                    process_device_update(p_ant_message);
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
        {
            break;
        }
    }
}


/**@brief Processes an ANT messages while in the sending command state.
 *
 * @param[in] Pointer to the raw ant message received.
 */
static void process_message_sending_command(uint8_t* p_ant_message)
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
                        //This event will be received if broadcast messages were used
                        //Automatically retry to increase the chances of a successful transmission

                        asc_encode_command_page(m_current_command_data, m_tx_buffer);
                        if (m_send_command_as_ack)
                        {
                            err_code = sd_ant_acknowledge_message_tx(m_channel_number, ANT_STANDARD_DATA_PAYLOAD_SIZE, m_tx_buffer);
                        }
                        else
                        {
                            err_code = sd_ant_broadcast_message_tx(m_channel_number, ANT_STANDARD_DATA_PAYLOAD_SIZE, m_tx_buffer);
                        }
                        APP_ERROR_CHECK(err_code);

                        if (++m_msg_counter_sending_command > m_retries)
                        {
                            m_is_command_pending = false;
                            ascm_set_state(m_previous_state);
                        }
                        break;
                    }
                    case EVENT_TRANSFER_TX_COMPLETED:
                    {
                        //The message was successfully received. No need to resend
                        m_is_command_pending = false;
                        ascm_set_state(m_previous_state);
                        break;
                    }
                    case EVENT_TRANSFER_TX_FAILED:
                    {
                        asc_encode_command_page(m_current_command_data, m_tx_buffer);
                        err_code = sd_ant_acknowledge_message_tx(m_channel_number, ANT_STANDARD_DATA_PAYLOAD_SIZE, m_tx_buffer);
                        APP_ERROR_CHECK(err_code);

                        //retry untitl success, or until too many retries have occured
                        if (++m_msg_counter_sending_command > m_retries)
                        {
                            m_is_command_pending = false;
                            ascm_set_state(m_previous_state);
                        }
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
            break;
        }
        case MESG_ACKNOWLEDGED_DATA_ID:
            //fall-through
        case MESG_BROADCAST_DATA_ID:
        {
            //Process by page
            switch (p_ant_message[DECODE_PAGE_ID_BYTE])
            {
                case UPDATE_DATA_PID:
                {
                    process_device_update(p_ant_message);
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
        {
            break;
        }
    }
}


// Public Functions
void ascm_init(const asc_ant_params_t * const ant_parameters)
{
    uint32_t err_code;
    m_channel_number = ant_parameters->channel_number;
    m_channel_period = ant_parameters->channel_period;
    deviceregistry_setup(pm_device_registry);

    err_code = sd_ant_channel_assign(m_channel_number,
                                     CHANNEL_TYPE_SHARED_MASTER,
                                     ant_parameters->network_number,
                                     ASC_ANT_EXT_ASSIGNMENT);
    APP_ERROR_CHECK(err_code);

    //Assign slave channel id.
    err_code = sd_ant_channel_id_set(m_channel_number,
                                     *(ant_parameters->p_device_number),
                                     ant_parameters->device_type,
                                     ant_parameters->tx_type);
    APP_ERROR_CHECK(err_code);

    //Assign slave channel frequency.
    err_code = sd_ant_channel_radio_freq_set(m_channel_number, ant_parameters->rf_frequency);
    APP_ERROR_CHECK(err_code);

    //Assign slave channel message period.
    err_code = sd_ant_channel_period_set(m_channel_number,
                                         asc_get_counts_from_period_enum(m_channel_period));
    APP_ERROR_CHECK(err_code);

    //Set the transmission power.
    err_code = sd_ant_channel_radio_tx_power_set(m_channel_number,
                                                 ant_parameters->tx_power,
                                                 ASC_ANT_CUSTOM_PWR);
    APP_ERROR_CHECK(err_code);
}


void ascm_turn_on(void)
{
    ascm_set_state(ADDRESS_AVAILABLE);
}


void ascm_handle_ant_event(uint8_t event, uint8_t * p_event_message_buffer)
{
    switch (m_state)
    {
        case ASCM_OFF:
        {
            process_message_off(p_event_message_buffer);
            break;
        }
        case ADDRESS_AVAILABLE:
        {
            process_message_address_available(p_event_message_buffer);
            break;
        }
        case HANDSHAKING:
        {
            process_message_handshaking(p_event_message_buffer);
            break;
        }
        case POLLING:
        {
            process_message_polling(p_event_message_buffer);
            break;
        }
        case SENDING_COMMAND:
        {
            process_message_sending_command(p_event_message_buffer);
            break;
        }
        default:
            break;
    }
}


ascm_command_status_t ascm_send_command(asc_command_data_t command_data, uint8_t retries)
{
    if (m_state != ADDRESS_AVAILABLE && m_state != POLLING)
    {
        return FAIL_BUSY;
    }
    else if (command_data.shared_address == EVERY_DEVICE_ADDRESS)
    {
        m_send_command_as_ack = false;
    }
    else if (deviceregistry_is_device_registered(pm_device_registry, command_data.shared_address))
    {
        m_send_command_as_ack = true;
    }
    else
    {
        return FAIL_UNREGISTERED_ADDRESS;
    }

    m_retries = retries;
    m_current_command_data = command_data;
    m_is_command_pending = true;

    return COMMAND_SENT;
}


ascm_states_t ascm_state_get(void)
{
    return m_state;
}

uint32_t ascm_events_get(void)
{
    return m_asc_event_flags;
}


asc_update_data_t ascm_get_last_recevied_update(void)
{
    return m_last_received_udpate;
}


void ascm_event_clear(uint32_t event)
{
    asc_event_clear(&m_asc_event_flags, event);
}


uint32_t ascm_get_device_registry_events(void)
{
    return m_device_registry.event_flags;
}


void ascm_clear_device_registry_event(uint32_t event)
{
    deviceregistry_clear_event(pm_device_registry, event);
}

/** @} **/
