/**
 * Copyright (c) 2016 - 2019 Nordic Semiconductor ASA and Luxoft Global Operations Gmbh.
 *
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef FSM_H_INCLUDED
#define FSM_H_INCLUDED

#include <stdint.h>
#include "mac_mlme_pib.h"
#include "mac_mcps_data.h"

typedef uint8_t                           sequence_number_t;
#define MAX_APP_SEQUENCE_NUMBER_SIZE      sizeof(sequence_number_t)
#define MAX_MSDU_SIZE                     (PHY_MAX_PACKET_SIZE - MAC_MEMORY_RESERVE - MAX_APP_SEQUENCE_NUMBER_SIZE)
#define PAYLOAD_START_POSITION            (MAC_MAX_MHR_SIZE + MAX_APP_SEQUENCE_NUMBER_SIZE)
#define COUNTER_POSITION                  (MAC_MAX_MHR_SIZE)

typedef enum
{
    E_START_CONFIG,
    E_SET_DONE,
    E_RADIO_RX_DONE,
    E_RADIO_TX_DONE,
    E_UART_RX_DONE,
    E_UART_TX_DONE,
    E_SECURITY_SET,
    EVENTS_AMOUNT
} fsm_events_t;

typedef union
{
    mlme_set_conf_t  * mlme_set;
    mcps_data_ind_t  * mcps_data_ind;
    mcps_data_conf_t * mcps_data_conf;
} fsm_event_data_t;


void fsm_init(void);
void fsm_event_post(fsm_events_t event, fsm_event_data_t * p_data);
void fsm_event_scheduler_run(void);

#endif /* FSM_H_INCLUDED */
