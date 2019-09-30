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

#ifndef NFC_CENTRAL_M_H__
#define NFC_CENTRAL_M_H__

#include <stdbool.h>
#include "ble_advdata.h"
#include "sdk_errors.h"
#include "ble_gap.h"

/**@brief Function for initializing the NFC part of the application.
 */
void nfc_central_init(void);

/**
 * @brief Function for reading the NFC tag content.
 *
 * @details If an NFC tag has been read, the device disables the NFC reader and initiates
 *          BLE scanning to find and connect to the device whose tag has been found.
 */
void nfc_tag_process(void);

/**@brief Function for checking if the given BLE device address matches the one that
 *        was found in the Connection Handover NDEF message.
 *
 * @param[in] p_peer_addr BLE device address to check.
 *
 * @retval    true        If the device addresses match.
 * @retval    false       Otherwise.
 */
bool nfc_oob_pairing_tag_match(ble_gap_addr_t const * const p_peer_addr);

/**@brief Function for notifying the NFC part of the application that the read Connection Handover
 *        NDEF Message is no longer valid.
 */
void nfc_oob_pairing_tag_invalidate(void);

/**@brief Function for acquiring the Temporary Key value.
 *
 * @param[out] pp_tk_value   Pointer to pointer to the Temporary Key value.
 *
 * @retval    NRF_SUCCESS         If the Temporary Key was found.
 * @retval    NRF_ERROR_NOT_FOUND Otherwise.
 */
ret_code_t nfc_tk_value_get(ble_advdata_tk_value_t ** pp_tk_value);

/**
 * @brief Function for enabling the NFC reader and BLE pairing with a peripheral device.
 */
void nfc_pair_start(void);

/**@brief Function for disabling the NFC reader and BLE pairing with a peripheral device.
 */
void nfc_pair_stop(void);

/**@brief Function for getting the nfc_central flag value.
 */
bool is_nfc_central_get(void);

/**@brief Function for setting the nfc_central flag.
 *
 * @param[in] value      nfc_central flag value to set.
 */
void is_nfc_central_set(bool value);

/**@brief Function for acquiring the LESC OOB peer data.
 *
 * @return    Pointer to lesc_oob_peer_data.
 */
ble_gap_lesc_oob_data_t * get_lesc_oob_peer_data(void);

#endif // NFC_CENTRAL_M_H__
