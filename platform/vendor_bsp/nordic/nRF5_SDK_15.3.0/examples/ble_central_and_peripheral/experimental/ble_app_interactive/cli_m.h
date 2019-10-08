/**
 * Copyright (c) 2018 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
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

#ifndef CLI_H__
#define CLI_H__

#include <stdint.h>
#include "nrf_cli.h"
#include "nrf_cli_uart.h"
#include "ble_gap.h"


typedef struct
{
    ble_gap_addr_t address; /**< Device address. */
} conn_peer_t;


/**@brief Function for adding a connected device address to a dynamic command list.
 *
 * @param[in]    p_connected_peers   Pointer to a struct with information about the connected devices.
 * @param[in]    conn_handle         Connection handle identifying the connection
 *                                   to perform this procedure on.
 */
void connected_to_cmd_add(conn_peer_t * p_connected_peers, uint16_t conn_handle);


/**@brief Function for removing a device address from a dynamic command list on BLE_EVT_DISCONNECTED.
 *
 * @param[in]    p_connected_peers  Pointer to struct with information about the connected devices.
 * @param[in]    conn_handle        The connection handle identifying the connection
 *                                  to perform this procedure on.
 */
void connected_to_cmd_remove(conn_peer_t * p_connected_peers, uint16_t conn_handle);


/**@brief Function for adding a device address to the dynamic command buffer.
 *
 * @param[in]    p_connected_adr Pointer to device address struct.
 */
void address_to_cmd_add(ble_gap_addr_t const * p_connected_adr);


/**@brief Function for bonding information writing to a dynamic command buffer.
 */
void bond_get(void);


/**@brief Function for setting key_request.
 */
void key_request_set(void);


/**@brief Function for deleting all data stored for all peers.
 */
void bonds_delete(void);


/**@brief Function for clearing a device's address from scan.
 */
void connect_addr_clear(void);


#endif // CLI_H__
