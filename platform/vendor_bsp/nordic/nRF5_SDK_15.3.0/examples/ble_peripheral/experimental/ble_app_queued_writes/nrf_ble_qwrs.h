/**
 * Copyright (c) 2016 - 2019, Nordic Semiconductor ASA
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
/** @cond To make doxygen skip this file */

#ifndef BLE_QWRS_H__
#define BLE_QWRS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_ble_qwr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NRF_BLE_QWRS_MAX_RCV_SIZE   128

/**@brief Queued Write Example Service event types. */
typedef enum
{
    BLE_QWRS_CHECK_RCVD_DATA,                        /* On this event, the application shall only verify if the data are correct.*/
    BLE_QWRS_NEW_DATA_RCVD,                          /* On this event, the application can act upon the received data*/
} nrf_ble_qwrs_evt_type;


/**@brief Queued Write Example Service event. */
typedef struct
{
    nrf_ble_qwrs_evt_type evt_type;                        //!< Type of event.
    uint16_t              rcv_length;
    uint8_t               rcvd_data[NRF_BLE_QWRS_MAX_RCV_SIZE];
} nrf_ble_qwrs_evt_t;


// Forward declaration of the nrf_ble_qwrs_t type.
struct nrf_ble_qwrs_t;


/**@brief Queued Write Example Service event handler type. returns a BLE_GATT_STATUS_CODES */
typedef uint16_t (*nrf_ble_qwrs_evt_handler_t) (struct nrf_ble_qwrs_t * p_qwrs,
                                                nrf_ble_qwrs_evt_t    * p_evt);


typedef struct
{
    nrf_ble_qwrs_evt_handler_t   evt_handler;       //!< Event handler to be called for handling events in the Queued Write Example  Service.
    ble_srv_error_handler_t      error_handler;     //!< Function to be called in case of an error.
    nrf_ble_qwr_t              * p_qwr_ctx;         //!< pointer to the initialized queued write context
} nrf_ble_qwrs_init_t;


typedef struct nrf_ble_qwrs_t
{
    uint8_t                    uuid_type;               //!< UUID type.
    uint16_t                   service_handle;          //!< Handle of Queued Write Example  Service (as provided by the BLE stack).
    uint16_t                   conn_handle;             //!< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).
    nrf_ble_qwrs_evt_handler_t evt_handler;             //!< Event handler to be called for handling events in the Queued Write Example  Service.
    ble_srv_error_handler_t    error_handler;           //!< Function to be called in case of an error.
    ble_gatts_char_handles_t   long_charact_handles;    //!< Handles related to the Queued Write Example long characteristic.
    ble_gatts_char_handles_t   charact_handles;         //!< Handles related to the Queued Write Example characteristic.
} nrf_ble_qwrs_t;


/**@brief Function for initializing the Queued Write Example Service.
 *
 * @details This call allows the application to initialize the Queued Write Example Service.
 *
 * @param[in]   p_qwrs_init  Information needed to initialize the service.
 * @param[out]  p_qwrs       Queued Write Example Service structure.

 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t nrf_ble_qwrs_init(nrf_ble_qwrs_init_t *p_qwrs_init, nrf_ble_qwrs_t *p_qwrs);


/**@brief event handler function for handling event from the queued write module.
 *
 * @param[in]  p_qwrs     Queued Write Example Service structure.
 * @param[in]  p_qwr      Queued Write structure.
 * @param[in]  p_evt      event received from the QWR module.
 *
 * @return      BLE_GATT_STATUS_SUCCESS if the received data are accepted, error code otherwise.
 */
uint16_t nrf_ble_qwrs_on_qwr_evt(nrf_ble_qwrs_t *p_qwrs,
                                 nrf_ble_qwr_t * p_qwr,
                                 nrf_ble_qwr_evt_t * p_evt);


#ifdef __cplusplus
}
#endif

#endif // BLE_QWRS_H__

/** @} */

/** @endcond */
