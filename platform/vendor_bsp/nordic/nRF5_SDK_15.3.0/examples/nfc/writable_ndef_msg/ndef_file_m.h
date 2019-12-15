/**
 * Copyright (c) 2017 - 2019, Nordic Semiconductor ASA
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

#ifndef NDEF_FILE_M_H__
#define NDEF_FILE_M_H__

/** @file
 *
 * @defgroup nfc_writable_ndef_msg_example_ndef_file_m ndef_file_m.h
 * @{
 * @ingroup nfc_writable_ndef_msg_example
 * @brief The FDS file handler for the NFC writable NDEF message example.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "compiler_abstraction.h"
#include "sdk_errors.h"
#include "nrf.h"

/**
 * @brief   Function for initializing the FDS module.
 *
 * @return  NRF_SUCCESS when module has been set up properly,
 *          error code otherwise.
 */
ret_code_t ndef_file_setup(void);

/**
 * @brief   Function for updating NDEF message in the flash file.
 *
 * @details FDS update operation is performed asynchronously,
 *          operation status is reported through events.
 *
 * @param[in] p_buff Pointer to the NDEF message to be stored in flash.
 * @param[in] size   Size of NDEF message.
 *
 * @return  NRF_SUCCESS when update request has been added to the queue.
 *          Otherwise, FDS error code.
 */
ret_code_t ndef_file_update(uint8_t const * p_buff, uint32_t size);

/**
 * @brief   Function for loading NDEF message from the flash file.
 *
 * @details If the flash file does not exist, the default NDEF message
 *          is created and stored in flash.
 *
 * @param[out] p_buff Pointer to the buffer for NDEF message.
 * @param[in]  size   Size of the buffer.
 *
 * @return  NRF_SUCCESS when NDEF message has been retrieved properly.
 *          Otherwise, FDS error code.
 */
ret_code_t ndef_file_load(uint8_t * p_buff, uint32_t size);

/**
 * @brief  Function for creating the default NDEF message: URL "nordicsemi.com".
 *
 * @param[out]    p_buff Pointer to the NDEF message buffer.
 * @param[inout]  p_size Pointer to the variable holding buffer size as input,
 *                       size of the created message as output.
 *
 * @return NRF_SUCCESS if the message has been created, error code otherwise.
 */
ret_code_t ndef_file_default_message(uint8_t * p_buff, uint32_t * p_size);

/** @} */

#endif // NDEF_FILE_M_H__
