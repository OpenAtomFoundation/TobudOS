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
 * @defgroup
 * @{
 * @ingroup
 *
 * @brief
 */

#include "asc_pages.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "asc_parameters.h"
#include "ant_parameters.h"
#include "nrf_error.h"

#ifdef TWO_BYTE_SHARED_ADDRESS
void asc_encode_set_shared_address(uint16_t shared_address, uint8_t * p_tx_buffer)
{
    memset(p_tx_buffer, RESERVED_ZEROS, ANT_STANDARD_DATA_PAYLOAD_SIZE);
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE1] = (uint8_t) shared_address;
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE2] = (uint8_t) (shared_address >> 8);
    p_tx_buffer[ENCODE_PAGE_ID_BYTE] = SET_SLAVE_ADDRESS_PID;
}
#else
void asc_encode_set_shared_address(uint8_t shared_address, uint8_t * p_tx_buffer)
{
    memset(p_tx_buffer, RESERVED_ZEROS, ANT_STANDARD_DATA_PAYLOAD_SIZE);
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE] = shared_address;
    p_tx_buffer[ENCODE_PAGE_ID_BYTE] = SET_SLAVE_ADDRESS_PID;
}
#endif


void asc_encode_address_available_page(asc_addr_available_paramters_t params, uint8_t * p_tx_buffer)
{
    memset(p_tx_buffer, RESERVED_ONES, ANT_STANDARD_DATA_PAYLOAD_SIZE);
    p_tx_buffer[ENCODE_PAGE_ID_BYTE] = ADDRESS_AVAILABLE_PID;

#ifdef TWO_BYTE_SHARED_ADDRESS
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE1] = (uint8_t) SEARCH_ADDRESS;
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE2] = (uint8_t) (SEARCH_ADDRESS >> 8);
#else
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE] = SEARCH_ADDRESS;
#endif

    p_tx_buffer[3] = 0x0F | (params.channel_period << 4);
    p_tx_buffer[5] = params.backoff_range;
    p_tx_buffer[6] = (uint8_t) params.data_timeout >> 1; //divide data timeout seconds by two for transmission
    p_tx_buffer[7] = ADDRESS_AVAILABLE_MASK & params.is_address_available;
}


void asc_encode_request_address_page(uint32_t serial_num, uint8_t * p_tx_buffer)
{
    memset(p_tx_buffer, RESERVED_ONES, ANT_STANDARD_DATA_PAYLOAD_SIZE);
    p_tx_buffer[ENCODE_PAGE_ID_BYTE] = REQUEST_ADDRESS_PID;

#ifdef TWO_BYTE_SHARED_ADDRESS
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE1] = (uint8_t) SEARCH_ADDRESS;
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE2] = (uint8_t) (SEARCH_ADDRESS >> 8);

    p_tx_buffer[5] = (uint8_t) serial_num;
    p_tx_buffer[6] = (uint8_t) (serial_num >> 8);
    p_tx_buffer[7] = (uint8_t) (serial_num >> 16);

#else
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE] = SEARCH_ADDRESS;

    p_tx_buffer[4] = (uint8_t) serial_num;
    p_tx_buffer[5] = (uint8_t) (serial_num >> 8);
    p_tx_buffer[6] = (uint8_t) (serial_num >> 16);
    p_tx_buffer[7] = (uint8_t) (serial_num >> 24);
#endif
}


#ifdef TWO_BYTE_SHARED_ADDRESS
void asc_encode_busy_acquiring_page(uint16_t next_address, uint32_t serial_num, uint8_t * p_tx_buffer)
{
    memset(p_tx_buffer, RESERVED_ONES, ANT_STANDARD_DATA_PAYLOAD_SIZE);
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE1] = (uint8_t) REGISTER_ADDRESS;
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE2] = (uint8_t) (REGISTER_ADDRESS >> 8);
    p_tx_buffer[ENCODE_PAGE_ID_BYTE] = BUSY_ACQUIRING_PID;

    p_tx_buffer[3] = (uint8_t) next_address;
    p_tx_buffer[4] = (uint8_t) (next_address >> 8);
    p_tx_buffer[5] = (uint8_t) serial_num;  //Serial number LSB
    p_tx_buffer[6] = (uint8_t) (serial_num >> 8);  //Serial number
    p_tx_buffer[7] = (uint8_t) (serial_num >> 16);  //Serial number MSB
}
#else
void asc_encode_busy_acquiring_page(uint8_t next_address, uint32_t serial_num, uint8_t * p_tx_buffer)
{
    memset(p_tx_buffer, RESERVED_ONES, ANT_STANDARD_DATA_PAYLOAD_SIZE);
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE] = REGISTER_ADDRESS;
    p_tx_buffer[ENCODE_PAGE_ID_BYTE] = BUSY_ACQUIRING_PID;

    p_tx_buffer[3] = next_address;
    p_tx_buffer[4] = (uint8_t) serial_num;  //Serial number LSB
    p_tx_buffer[5] = (uint8_t) (serial_num >> 8);  //Serial number
    p_tx_buffer[6] = (uint8_t) (serial_num >> 16);  //Serial number
    p_tx_buffer[7] = (uint8_t) (serial_num >> 24);  //Serial number MSB
}
#endif


void asc_encode_confirm_acquire_page(asc_confirm_acquire_paramters_t params, uint8_t * p_tx_buffer)
{
    memset(p_tx_buffer, RESERVED_ONES, ANT_STANDARD_DATA_PAYLOAD_SIZE);
    p_tx_buffer[ENCODE_PAGE_ID_BYTE] = CONFIRM_ACQUIRE_PID;

#ifdef TWO_BYTE_SHARED_ADDRESS
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE1] = (uint8_t) REGISTER_ADDRESS;
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE2] = (uint8_t) (REGISTER_ADDRESS >> 8);
#else
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE] = REGISTER_ADDRESS;
#endif

    p_tx_buffer[3] = (uint8_t) params.serial_number;
    p_tx_buffer[4] = (uint8_t) params.model_number;
    p_tx_buffer[5] = (uint8_t) (params.model_number >> 8);
    p_tx_buffer[6] = params.hw_revision;
    p_tx_buffer[7] = params.sw_revision;
}


void asc_encode_command_page(asc_command_data_t command_data, uint8_t * p_tx_buffer)
{
    memset(p_tx_buffer, RESERVED_ONES, ANT_STANDARD_DATA_PAYLOAD_SIZE);
    p_tx_buffer[ENCODE_PAGE_ID_BYTE] = COMMAND_PID;

#ifdef TWO_BYTE_SHARED_ADDRESS
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE1] = (uint8_t) command_data.shared_address;
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE2] = (uint8_t) (command_data.shared_address >> 8);
#else
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE] = command_data.shared_address;
#endif

    p_tx_buffer[3] = (uint8_t) (command_data.group_number << 4);
    p_tx_buffer[7] = (uint8_t) command_data.command;
}


void asc_encode_phone_command_page(asc_command_data_t command_data, uint8_t * p_tx_buffer)
{
    memset(p_tx_buffer, RESERVED_ONES, ANT_STANDARD_DATA_PAYLOAD_SIZE);
    p_tx_buffer[0] = PHONE_COMMAND_PID;

#ifdef TWO_BYTE_SHARED_ADDRESS
    p_tx_buffer[1] = (uint8_t) command_data.shared_address;
    p_tx_buffer[2] = (uint8_t) (command_data.shared_address >> 8);
#else
    p_tx_buffer[1] = command_data.shared_address;
#endif

    p_tx_buffer[3] = (uint8_t) command_data.master_id;
    p_tx_buffer[4] = (uint8_t) (command_data.master_id >> 8);
    p_tx_buffer[5] = (uint8_t) (command_data.group_number << 4);
    p_tx_buffer[7] = (uint8_t) command_data.command;
}


void asc_encode_update_data_page(asc_update_data_t update_data, uint8_t * p_tx_buffer)
{
    memset(p_tx_buffer, RESERVED_ONES, ANT_STANDARD_DATA_PAYLOAD_SIZE);
    p_tx_buffer[ENCODE_PAGE_ID_BYTE] = UPDATE_DATA_PID;

#ifdef TWO_BYTE_SHARED_ADDRESS
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE1] = (uint8_t) update_data.shared_address;
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE2] = (uint8_t) (update_data.shared_address >> 8);
#else
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE] = update_data.shared_address;
#endif

    p_tx_buffer[7] = (uint8_t) update_data.state;
}

void asc_encode_phone_update_data_page(asc_update_data_t update_data, uint8_t * p_tx_buffer)
{
    memset(p_tx_buffer, RESERVED_ONES, ANT_STANDARD_DATA_PAYLOAD_SIZE);
    p_tx_buffer[ENCODE_PHONE_PAGE_ID_BYTE] = UPDATE_DATA_PID;

#ifdef TWO_BYTE_SHARED_ADDRESS
    p_tx_buffer[ENCODE_PHONE_SHARED_ADDRESS_BYTE1] = (uint8_t) update_data.shared_address;
    p_tx_buffer[ENCODE_PHONE_SHARED_ADDRESS_BYTE2] = (uint8_t) (update_data.shared_address >> 8);
#else
    p_tx_buffer[ENCODE_PHONE_SHARED_ADDRESS_BYTE1] = update_data.shared_address;
#endif

    p_tx_buffer[3] = (uint8_t) update_data.master_id;
    p_tx_buffer[4] = (uint8_t) (update_data.master_id >> 8);
    p_tx_buffer[7] = (uint8_t) update_data.state;
}

void asc_encode_request_data_page(asc_request_data_t request_data, uint8_t * p_tx_buffer)
{
    memset(p_tx_buffer, RESERVED_ONES, ANT_STANDARD_DATA_PAYLOAD_SIZE);
    p_tx_buffer[ENCODE_PAGE_ID_BYTE] = REQUEST_DATA_PID;

#ifdef TWO_BYTE_SHARED_ADDRESS
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE1] = (uint8_t) request_data.shared_address;
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE2] = (uint8_t) (request_data.shared_address >> 8);
#else
    p_tx_buffer[ENCODE_SHARED_ADDRESS_BYTE] = request_data.shared_address;
#endif

    p_tx_buffer[6] = request_data.page_id_requested;
    p_tx_buffer[7] = (uint8_t) 0x01;
}


void asc_encode_device_availability_page(uint16_t neighbor_id, uint8_t * p_tx_buffer)
{
    memset(p_tx_buffer, RESERVED_ONES, ANT_STANDARD_DATA_PAYLOAD_SIZE);
    p_tx_buffer[0] = DEVICE_AVAILABILITY_PID;
    p_tx_buffer[1] = (uint8_t) neighbor_id;
    p_tx_buffer[2] = (uint8_t) (neighbor_id >> 8);
}


uint32_t asc_decode_address_available_page(asc_addr_available_paramters_t * p_params, uint8_t * p_rx_buffer)
{
    if (p_rx_buffer[DECODE_PAGE_ID_BYTE] == ADDRESS_AVAILABLE_PID)
    {
        p_params->channel_period = (asc_message_periods_t) ( p_rx_buffer[ANT_MESSAGE_DATA3_OFFSET] >> 4);
        p_params->is_address_available = p_rx_buffer[ANT_MESSAGE_DATA7_OFFSET] & ADDRESS_AVAILABLE_MASK;
        p_params->backoff_range = p_rx_buffer[ANT_MESSAGE_DATA5_OFFSET];
        p_params->data_timeout = p_rx_buffer[ANT_MESSAGE_DATA6_OFFSET] << 1; //Multiply data timeout by 2 to get total timeout in seconds

        return NRF_SUCCESS;
    }
    else
    {
        return NRF_ERROR_INVALID_PARAM;
    }
}


uint32_t asc_decode_request_address_page(uint32_t * serial_number, uint8_t * p_rx_buffer)
{
    if (p_rx_buffer[DECODE_PAGE_ID_BYTE] == REQUEST_ADDRESS_PID)
    {
#ifdef TWO_BYTE_SHARED_ADDRESS
        *serial_number =
        (uint32_t) p_rx_buffer[ANT_MESSAGE_DATA5_OFFSET] |
        ((uint32_t) p_rx_buffer[ANT_MESSAGE_DATA6_OFFSET] << 8) |
        ((uint32_t) p_rx_buffer[ANT_MESSAGE_DATA7_OFFSET] << 16) ;
#else
        *serial_number =
        (uint32_t) p_rx_buffer[ANT_MESSAGE_DATA4_OFFSET] |
        ((uint32_t) p_rx_buffer[ANT_MESSAGE_DATA5_OFFSET] << 8) |
        ((uint32_t) p_rx_buffer[ANT_MESSAGE_DATA6_OFFSET] << 16) |
        ((uint32_t) p_rx_buffer[ANT_MESSAGE_DATA7_OFFSET] << 24);
#endif
        return NRF_SUCCESS;
    }
    else
    {
        return NRF_ERROR_INVALID_PARAM;
    }
}


#ifdef TWO_BYTE_SHARED_ADDRESS
uint32_t asc_decode_busy_acquiring_page(uint32_t serial_number, bool * p_is_serial_match, uint16_t * next_address, uint8_t * p_rx_buffer)
{
    if (p_rx_buffer[DECODE_PAGE_ID_BYTE] == BUSY_ACQUIRING_PID)
    {
        //Check to see that the received serial number in the message buffer matches the serial number argument.
        uint32_t received_serial_number = ((uint32_t) p_rx_buffer[ANT_MESSAGE_DATA7_OFFSET] << 16 |
                                           (uint32_t) p_rx_buffer[ANT_MESSAGE_DATA6_OFFSET] << 8 |
                                           (uint32_t) p_rx_buffer[ANT_MESSAGE_DATA5_OFFSET]);

        if ((serial_number & 0xFFF) == (received_serial_number & 0xFFF))
        {
            *p_is_serial_match = true;
            *next_address = (p_rx_buffer[ANT_MESSAGE_DATA4_OFFSET] << 8 |
                             p_rx_buffer[ANT_MESSAGE_DATA3_OFFSET]);
        }
        else
        {
            *p_is_serial_match = false;
        }
        return NRF_SUCCESS;
    }
    else
    {
        return NRF_ERROR_INVALID_PARAM;
    }
}
#else
uint32_t asc_decode_busy_acquiring_page(uint32_t serial_number, bool * p_is_serial_match, uint8_t * next_address, uint8_t * p_rx_buffer)
{
    if (p_rx_buffer[DECODE_PAGE_ID_BYTE] == BUSY_ACQUIRING_PID)
    {
        //Check to see that the received serial number in the message buffer matches the serial number argument.
        uint32_t received_serial_number = ((uint32_t) p_rx_buffer[ANT_MESSAGE_DATA7_OFFSET] << 24 |
                                             (uint32_t) p_rx_buffer[ANT_MESSAGE_DATA6_OFFSET] << 16 |
                                             (uint32_t) p_rx_buffer[ANT_MESSAGE_DATA5_OFFSET] << 8  |
                                             (uint32_t) p_rx_buffer[ANT_MESSAGE_DATA4_OFFSET]);

        if (received_serial_number == serial_number)
        {
            *p_is_serial_match = true;
            *next_address = p_rx_buffer[ANT_MESSAGE_DATA3_OFFSET];
        }
        else
        {
            *p_is_serial_match = false;
        }
        return NRF_SUCCESS;
    }
    else
    {
        return NRF_ERROR_INVALID_PARAM;
    }
}
#endif


uint32_t asc_decode_confirm_acquire_page(uint32_t serial_number,
                                                bool * p_is_serial_match,
                                                asc_confirm_acquire_paramters_t * p_params,
                                                uint8_t * p_rx_buffer)
{
    if (p_rx_buffer[DECODE_PAGE_ID_BYTE] == CONFIRM_ACQUIRE_PID)
    {
         //Confirm that it is the correct device
        if (((uint8_t) serial_number) == p_rx_buffer[ANT_MESSAGE_DATA3_OFFSET])
        {
            *p_is_serial_match = true;
            p_params->model_number = p_rx_buffer[ANT_MESSAGE_DATA5_OFFSET] << 8 |
                                     p_rx_buffer[ANT_MESSAGE_DATA4_OFFSET] ;
            p_params->hw_revision = p_rx_buffer[ANT_MESSAGE_DATA6_OFFSET];
            p_params->sw_revision = p_rx_buffer[ANT_MESSAGE_DATA7_OFFSET];
        }
        else
        {
            *p_is_serial_match = false;
        }

        return NRF_SUCCESS;
    }
    else
    {
        return NRF_ERROR_INVALID_PARAM;
    }
}


uint32_t asc_decode_command_page(asc_command_data_t * p_command_data, uint8_t * p_rx_buffer)
{
    if (p_rx_buffer[DECODE_PAGE_ID_BYTE] == COMMAND_PID)
    {
        p_command_data->group_number = (p_rx_buffer[ANT_MESSAGE_DATA3_OFFSET] >> 4);
        p_command_data->command = (asc_commands_t) p_rx_buffer[ANT_MESSAGE_DATA7_OFFSET];
        return NRF_SUCCESS;
    }
    else
    {
        return NRF_ERROR_INVALID_PARAM;
    }
}


uint32_t asc_decode_update_data_page(asc_update_data_t * p_update_data, uint8_t * p_rx_buffer)
{
    if (p_rx_buffer[DECODE_PAGE_ID_BYTE] == UPDATE_DATA_PID)
    {
#ifdef TWO_BYTE_SHARED_ADDRESS
        p_update_data->shared_address = p_rx_buffer[DECODE_SHARED_ADDRESS_BYTE2] << 8 |
                                        p_rx_buffer[DECODE_SHARED_ADDRESS_BYTE1];
#else
        p_update_data->shared_address = p_rx_buffer[DECODE_SHARED_ADDRESS_BYTE];
#endif
        p_update_data->state = (asc_slave_states_t) p_rx_buffer[ANT_MESSAGE_DATA7_OFFSET];
        return NRF_SUCCESS;
    }
    else
    {
        return NRF_ERROR_INVALID_PARAM;
    }
}


uint32_t asc_decode_phone_update_data_page(asc_update_data_t * p_update_data, uint8_t * p_rx_buffer)
{
    if (p_rx_buffer[DECODE_PHONE_PAGE_ID_BYTE] == UPDATE_DATA_PID)
    {
#ifdef TWO_BYTE_SHARED_ADDRESS
        p_update_data->shared_address = p_rx_buffer[DECODE_PHONE_SHARED_ADDRESS_BYTE2] << 8 |
                                        p_rx_buffer[DECODE_PHONE_SHARED_ADDRESS_BYTE1];
#else
        p_update_data->shared_address = p_rx_buffer[DECODE_PHONE_SHARED_ADDRESS_BYTE1];
#endif
        p_update_data->master_id = p_rx_buffer[ANT_MESSAGE_DATA4_OFFSET] << 8 |
                                   p_rx_buffer[ANT_MESSAGE_DATA3_OFFSET];
        p_update_data->state = (asc_slave_states_t) p_rx_buffer[ANT_MESSAGE_DATA7_OFFSET];
        return NRF_SUCCESS;
    }
    else
    {
        return NRF_ERROR_INVALID_PARAM;
    }
}


uint32_t asc_decode_request_data_page(asc_request_data_t * p_request_data,  uint8_t * p_rx_buffer)
{
    if (p_rx_buffer[DECODE_PAGE_ID_BYTE] == REQUEST_DATA_PID)
    {
#ifdef TWO_BYTE_SHARED_ADDRESS
        p_request_data->shared_address = p_rx_buffer[DECODE_SHARED_ADDRESS_BYTE2] << 8 |
                                         p_rx_buffer[DECODE_SHARED_ADDRESS_BYTE1];
#else
        p_request_data->shared_address = p_rx_buffer[DECODE_SHARED_ADDRESS_BYTE];
#endif
        p_request_data->page_id_requested = p_rx_buffer[ANT_MESSAGE_DATA6_OFFSET];
        return NRF_SUCCESS;
    }
    else
    {
        return NRF_ERROR_INVALID_PARAM;
    }
}


uint32_t asc_decode_phone_command_page(asc_command_data_t * p_command_data,  uint8_t * p_rx_buffer)
{

    if (p_rx_buffer[ANT_MESSAGE_DATA0_OFFSET] == PHONE_COMMAND_PID)
    {
#ifdef TWO_BYTE_SHARED_ADDRESS
        p_command_data->shared_address = p_rx_buffer[ANT_MESSAGE_DATA2_OFFSET] << 8 |
                                         p_rx_buffer[ANT_MESSAGE_DATA1_OFFSET];
#else
        p_command_data->shared_address = p_rx_buffer[ANT_MESSAGE_DATA1_OFFSET];
#endif
        p_command_data->master_id = (p_rx_buffer[ANT_MESSAGE_DATA4_OFFSET] << 8) |
                                    p_rx_buffer[ANT_MESSAGE_DATA3_OFFSET];

        p_command_data->group_number =  (p_rx_buffer[ANT_MESSAGE_DATA5_OFFSET] >> 4);
        p_command_data->command = (asc_commands_t) p_rx_buffer[ANT_MESSAGE_DATA7_OFFSET];
        return NRF_SUCCESS;
    }
    else
    {
        return NRF_ERROR_INVALID_PARAM;
    }
}

/** @} */
