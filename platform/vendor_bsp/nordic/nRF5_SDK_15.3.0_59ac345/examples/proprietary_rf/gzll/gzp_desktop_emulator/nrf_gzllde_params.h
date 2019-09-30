/**
 * Copyright (c) 2012 - 2019, Nordic Semiconductor ASA
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
#ifndef COMMON_PARAMS_H__
#define COMMON_PARAMS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief Common parameters for nRF51 and nRF24LU1 desktop emulator projects.
 *
 * nRFready Desktop style parameters are prefixed with NRFR.
 *
 * @{
 * @ingroup gzll_desktop_device_emulator_example
 */

/******************************************************************************/
/** @name Addresses and channels
*  @{ */
/******************************************************************************/

#define NRF_GZLLDE_CHANNEL_TAB      {3, 23, 40, 61, 75}  ///< Redefine channel table for example.
#define NRF_GZLLDE_CHANNEL_TAB_SIZE 5                    ///< Redefine channel table size.

#define NRF_GZLLDE_ADDRESS_PIPE0    {1, 3, 6, 9, 12}     ///< Redefine addresses for example.
#define NRF_GZLLDE_ADDRESS_PIPE1    {2, 6, 8, 11, 20}    ///< Redefine addresses for example.
#define NRF_GZLLDE_ADDRESS_PIPE2    3
#define NRF_GZLLDE_ADDRESS_PIPE3    4
#define NRF_GZLLDE_ADDRESS_PIPE4    5
#define NRF_GZLLDE_ADDRESS_PIPE5    6
#define NRF_GZLLDE_ADDRESS_PIPE6    7
#define NRF_GZLLDE_ADDRESS_PIPE7    8

/** @} */


/******************************************************************************/
/** @name Timing parameters
*@{*/
/******************************************************************************/

#define NRF_GZLLDE_RXPERIOD_DIV_2                  504  ///< RXPERIOD/2 on LU1 = timeslot period on nRF51
#define NRF_GZLLDE_DEVICE_CHANNEL_SELECTION_POLICY NRF_GZLL_DEVICE_CHANNEL_SELECTION_POLICY_USE_SUCCESSFUL
///< Use last succesful channel. If timeslot_period
/** @} */


/******************************************************************************/
/** @name USB endpoint and Gzll pipe definitions
 *  @{ */
/******************************************************************************/

#define NRFR_KEYBOARD_EP 1    ///< Keyboard USB endpoint and pipe
#define NRFR_MOUSE_EP    2    ///< Mouse USB endpoint and pipe
/** @} */

/******************************************************************************/
/** @name Keyboard packet definition
 *  @{ */
/******************************************************************************/

#define NRFR_KEYBOARD_MOD           0                             ///< Byte 0: Modifier keys
#define NRFR_KEYBOARD_RESERVED      (NRFR_KEYBOARD_MOD + 1)       ///< Byte 1: Reserved
#define NRFR_KEYBOARD_KEYS          (NRFR_KEYBOARD_RESERVED + 1)  ///< Byte 2-7; Keyboard data
#define NRFR_KEYBOARD_PACKET_LENGTH (NRFR_KEYBOARD_KEYS + 6)      ///< Length: 8
/** @} */

/******************************************************************************/
/** @name Mouse packet definition
 *  @{ */
/******************************************************************************/

#define NRFR_REPORT_ID                   0                                   ///< All packets byte 0: Report ID
#define NRFR_MOUSE_BUTTONS_REPORT_ID     1                                   ///< Mouse buttons -> Report ID 1
#define NRFR_MOUSE_MOV_REPORT_ID         2                                   ///< Mouse movement -> Report ID 2
#define NRFR_ADVANCED_REPORT_ID          3                                   ///< Multimedia buttons -> Report ID 3

#define NRFR_MOUSE_BUTTONS               (NRFR_REPORT_ID + 1)                ///< NRFR_MOUSE_BUTTONS_REPORT_ID byte 1: Buttons
#define NRFR_MOUSE_WHEEL                 (NRFR_MOUSE_BUTTONS + 1)            ///< NRFR_MOUSE_BUTTONS_REPORT_ID byte 2: Scroll wheel
#define NRFR_MOUSE_PAN                   (NRFR_MOUSE_WHEEL + 1)              ///< NRFR_MOUSE_BUTTONS_REPORT_ID byte 3: Mouse pan
#define NRFR_MOUSE_BUTTONS_PACKET_LENGTH (NRFR_MOUSE_PAN + 1)                ///< NRFR_MOUSE_BUTTONS_REPORT_ID packet legth = 4

#define NRFR_MOUSE_MOV_XLSBYTE           (NRFR_REPORT_ID + 1)                ///< NRFR_MOUSE_MOV_REPORT_ID byte 1: X direction LSB
#define NRFR_MOUSE_MOV_YLSNIB_XMSNIB     (NRFR_MOUSE_MOV_XLSBYTE + 1)        ///< NRFR_MOUSE_MOV_REPORT_ID byte 2: Y direction LSB
#define NRFR_MOUSE_MOV_YMSBYTE           (NRFR_MOUSE_MOV_YLSNIB_XMSNIB + 1)  ///< NRFR_MOUSE_MOV_REPORT_ID byte 3: Y and X LS nibbles
#define NRFR_MOUSE_MOV_PACKET_LENGTH     (NRFR_MOUSE_MOV_YMSBYTE + 1)        ///< NRFR_MOUSE_MOV_REPORT_ID packet length = 4

#define NRFR_ADVANCED_BUTTONS            (NRFR_REPORT_ID + 1)                ///< NRFR_ADVANCED_REPORT_ID byte 1: Advanced mouse buttons
#define NRFR_ADVANCED_PACKET_LENGTH      (NRFR_ADVANCED_BUTTONS + 1)         ///< NRFR_ADVANCED_REPORT_ID packet length = 2
/** @} */

/** @} */


#ifdef __cplusplus
}
#endif

#endif
