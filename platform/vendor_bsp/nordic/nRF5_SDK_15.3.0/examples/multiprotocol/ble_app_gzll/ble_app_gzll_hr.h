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
/** @file
 *
 * @defgroup ble_sdk_app_gzll_bluetooth_part Bluetooth part of Multiprotocol Application
 * @{
 * @ingroup ble_sdk_app_gzll
 * @brief Heart rate demo application used in the multiprotocol application.

 * This file contains the source code for the Bluetooth part of multiprotocol
 * sample application using the @ref ble_sdk_srv_hrs (and also @ref ble_sdk_srv_bas and
 * @ref ble_sdk_srv_dis). This application uses the @ref ble_conn_params module.
 */

#ifndef BLE_APP_GZLL_HR_H__
#define BLE_APP_GZLL_HR_H__

#ifdef __cplusplus
extern "C" {
#endif

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
void ble_stack_start(void);

/**@brief Function for disabling the BLE stack.
 */
void ble_stack_stop(void);

/**@brief Function for initializing used services and starting the Bluetooth Heart rate application.
 *
 * @details This function initializes the Heart Rate service, the Battery service and the Device
 *          information, setup the advertising data and GAP database and then start advertising.
 */
void ble_hrs_app_start(void);

/**@brief Function for stopping the Bluetooth Heart rate application.
 *
 * @details This function stops all timers used by the Bluetooth Heart rate application.
 */
void ble_hrs_app_stop(void);


#ifdef __cplusplus
}
#endif

#endif // BLE_APP_GZLL_HR_H__
/** @} */
