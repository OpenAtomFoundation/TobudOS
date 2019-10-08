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
 * @brief ANT Auto Shared Channel (ASC) Slave implementation.
 */

#ifndef ASC_SLAVE_H__
#define ASC_SLAVE_H__

#include <stdint.h>
#include "asc_parameters.h"

#ifdef __cplusplus
extern "C" {
#endif


/**@brief Enumeration defining the possible states of an ASC Slave.
 */
typedef enum
{
    OFF = 0,                /**< ASC Slave state: OFF. The shared channel should be off when in this state. */
    SEARCHING,              /**< ASC Slave state: SEARCHING. The ASC Slave is searching for an ASC Master. */
    REQUESTING,             /**< ASC Slave state: REQUESTING. The ASC Slave is requesting a shared address from an ASC Master. */
    WAITING,                /**< ASC Slave state: WAITING. The ASC Slave is waiting for a response from an ASC Master. */
    CONFIRMING,             /**< ASC Slave state: CONFIRMING. The ASC Slave is confirming its shared address assignment with an ASC Master. */
    ASSIGNED,               /**< ASC Slave state: ASSIGNED. The ASC Slave has been assigned a shared address and is participating in a shared channel network. */
} ascs_states_t;


/**@brief Function for initializing the ASC Slave.
 *
 * @note  This function ignores the channel type member of the ant parameters argument. An ASC Slave channel will always be set up as a shared slave.
 *
 * @param p_ant_parameters Pointer to the ANT channel parameters used to assign and configure the shared channel.
 */
void ascs_init(const asc_ant_params_t * const p_ant_parameters);


/**@brief Function to turn on the ASC Slave and begin the registration process.
 */
void ascs_turn_on(void);


/**@brief Function to handle received ANT messages on the ASC Slave channel.
 *
 *@param event The ANT event type that was received.
 *
 *@param p_event_message_buffer The received message buffer.
 */
void ascs_handle_ant_event(uint8_t event, uint8_t * p_event_message_buffer);


/**@brief Function to send data back to the ASC master using ANT broadcast messages.
 *
 * @param[in] p_data 7 byte data payload.
 */
void ascs_send_data(uint8_t * p_data);


/**@brief Function to get the current ASC Slave state.
 *
 * @return A copy of the current ASC Slave state.
 */
ascs_states_t ascs_state_get(void);


/**@brief Function to get the current ASC Slave light state.
 *
 * @return A copy of the current ASC Slave light state.
 */
asc_slave_states_t ascs_light_state_get(void);


/**@brief Function to get the ASC Slave's event bitfield.
 *
 * @note After using this function and checking for an event, be sure to clear that event immediately.
 *
 * @return A copy of the current event bitfield.
 */
uint32_t ascs_events_get(void);


/**@brief Clears the specified event from the ASC Slave's event bitfield.
 *
 * @param[in] event The ASC event to clear from the bitfield.
 */
void ascs_event_clear(uint32_t event);


/**@brief Increments the internal counter used to control timeouts.
 *
 * @note This function should be called by a 1 second timer tick for accurate results.
 */
void ascs_increment_timer(void);


/**@brief Function to get the last request received from an ASC Master.
 *
 * @return A copy of the most recently received request page data.
 */
asc_request_data_t ascs_get_last_request(void);


/**@brief Function to send a response to a page request.
 *
 * @note This function assumes that the response_data_buffer is formatted properly
 *       including the correct size shared addres and properly located page id.
 *
 * @param[in] response_data_buffer The response page to be sent back to the ASC Master.
 */
void ascs_send_response(uint8_t * response_data_buffer);



#ifdef __cplusplus
}
#endif

#endif /* ASC_SLAVE_H__ */

/** @} */
