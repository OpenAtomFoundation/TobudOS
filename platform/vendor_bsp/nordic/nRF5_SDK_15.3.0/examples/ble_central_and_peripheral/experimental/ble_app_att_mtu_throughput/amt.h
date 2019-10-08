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

#ifndef AMT_H__
#define AMT_H__


#include "ble.h"
#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_db_discovery.h"
#include "nrf_ble_gatt.h"

/**@cond To Make Doxygen skip documentation generation for this file.
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

// UUID = bb4aff4f-ad03-415d-a96c-9d6cddda8304

#define SERVICE_UUID_BASE   {0xBB, 0x4A, 0xFF, 0x4F, 0xAD, 0x03, 0x41, 0x5D, \
                             0xA9, 0x6C, 0x9D, 0x6C, 0xDD, 0xDA, 0x83, 0x04}

#define AMT_SERVICE_UUID                    0x1523
#define AMTS_CHAR_UUID                      0x1524
#define AMT_RCV_BYTES_CNT_CHAR_UUID         0x1525

#define AMT_RCV_BYTES_CNT_MAX_LEN           4
#define AMT_BYTE_TRANSFER_CNT               (1024 * 1024)


/**
 * @defgroup nrf_ble_amt ATT MTU Throughput (AMT) Service Client
 * @{
 * @ingroup ble_sdk_srv
 *
 * @details  This module contains the APIs and types exposed by the ATT MTU Throughput
 *           Service Client module. These APIs and types can be used by the application to perform
 *           discovery of ATT MTU Throughput Service at the peer and interact with it.
 *
 * @note     The application must propagate BLE stack events to this module by calling
 *           nrf_ble_amt_c_on_ble_evt().
 */

#define BLE_AMTC_BLE_OBSERVER_PRIO   3


/**@brief Structure containing the handles found on the peer. */
typedef struct
{
    uint16_t amt_cccd_handle;               //!<  Handle of the CCCD . */
    uint16_t amt_handle;                    //!<  Handle of the characteristic as provided by the SoftDevice. */
    uint16_t amt_rbc_handle;                //!<  Handle of the Number of received bytes  characteristic as provided by the SoftDevice. */
} nrf_ble_amtc_db_t;

/**@brief AMT Client event type. */
typedef enum
{
    NRF_BLE_AMT_C_EVT_DISCOVERY_COMPLETE,   //!<  Event indicating that the peer throughput Service has been discovered at the peer. */
    NRF_BLE_AMT_C_EVT_NOTIFICATION,         //!<  Event indicating that a notification has been received from the peer. */
    NRF_BLE_AMT_C_EVT_RBC_READ_RSP          //!<  Event indicating that a Number of received bytes notification has been received from the peer. */
} nrf_ble_amtc_evt_type_t;


/**@brief AMT Notification structure. */
typedef struct
{
    uint16_t notif_len;                     //!<  Length of the received notification.*/
    uint32_t bytes_sent;                    //!<  Decoded number of bytes sent by the peer.*/
    uint32_t bytes_rcvd;                    //!<  Number of bytes received from the peer since the beggining of the transfer.*/
} nrf_ble_amtc_notif_t;


/**@brief AMT Event structure. */
typedef struct
{
    nrf_ble_amtc_evt_type_t evt_type;       //!<  Type of the event. */
    uint16_t                conn_handle;    //!<  Connection handle on which the event  occurred.*/
    union
    {
        nrf_ble_amtc_db_t    peer_db;       //!<  Handles found on the peer device. This will be filled if the evt_type is @ref NRF_BLE_AMT_C_EVT_DISCOVERY_COMPLETE.*/
        nrf_ble_amtc_notif_t hvx;           //!<  Notification data. This will be filled if the evt_type is @ref NRF_BLE_AMT_C_EVT_NOTIFICATION.*/
        uint32_t             rcv_bytes_cnt; //!<  Number of received bytes by the peer. This will be filled if the evt_type is @ref NRF_BLE_AMT_C_EVT_RBC_NOTIFICATION.*/
    } params;
} nrf_ble_amtc_evt_t;


// Forward declaration of the nrf_ble_amtc_t type.
struct nrf_ble_amtc_t;

/**@brief AMT Client module event handler type.
 * The AMT Client module will call this function when a matching server has been found on the peer and for each notification.
*/
typedef void (*nrf_ble_amtc_evt_handler_t) (struct nrf_ble_amtc_t * p_ctx,
                                            nrf_ble_amtc_evt_t    * p_evt);

/**@brief AMT_C structure.
 * @details This structure contains status information for the AMT client module. */
typedef struct nrf_ble_amtc_t
{
    uint16_t                   conn_handle;      //!<  Connection handle as provided by the SoftDevice. */
    nrf_ble_amtc_db_t          peer_db;          //!<  Handles on the peer*/
    nrf_ble_amtc_evt_handler_t evt_handler;      //!<  Application event handler to be called when there is an event related to this AMT Client Module. */
    uint8_t                    uuid_type;        //!<  UUID type. */
    uint32_t                   bytes_rcvd_cnt;   //!<  Number of bytes received.*/
} nrf_ble_amtc_t;


/**@brief      Function for initializing the ATT MTU Throughput client module.
 *
 * @details    This function will initialize the module and set up Database Discovery to discover
 *             the ATT MTU Throughput Service. After calling this function, call @ref ble_db_discovery_start
 *             to start discovery once a link with a peer has been established.
 *
 * @param[out] p_ctx       Pointer to the AMT client structure.
 * @param[in]  evt_handler Event handler.
 *
 * @retval     NRF_SUCCESS      Operation success.
 * @retval     NRF_ERROR_NULL   A parameter is NULL.
 *                              Otherwise, an error code returned by @ref ble_db_discovery_evt_register.
 */
ret_code_t nrf_ble_amtc_init(nrf_ble_amtc_t * p_ctx, nrf_ble_amtc_evt_handler_t evt_handler);


/**@brief     Function for handling BLE events from the SoftDevice.
 *
 * @details   This function will handle the BLE events received from the SoftDevice. If a BLE
 *            event is relevant to the AMT Client module, then it uses it to update
 *            interval variables and, if necessary, send events to the application.
 *
 * @param[in] p_ble_evt     Pointer to the BLE event.
 * @param[in] p_context     Pointer to the AMT client structure.
 */
void nrf_ble_amtc_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);


/**@brief   Function for enabling CCCD on the peer.
 *
 * @details This function will enable to notification of AMT at the peer
 *          by writing to the CCCD of the AMT Characteristic.
 *
 * @param   p_ctx Pointer to the AMT client structure.
 *
 * @retval  NRF_SUCCESS If the SoftDevice has been requested to write to the CCCD of the peer.
 *                      Otherwise, an error code. This function propagates the error code returned
 *                      by the SoftDevice API @ref sd_ble_gattc_write.
 */
ret_code_t nrf_ble_amtc_notif_enable(nrf_ble_amtc_t * p_ctx);


/**@brief     Function for handling events from the database discovery module.
 *
 * @details   Call this function when getting a callback event from the DB discovery modue.
 *            This function will handle an event from the database discovery module, and determine
 *            if it relates to the discovery of AMT service at the peer. If so, it will
 *            call the application's event handler indicating that the AMT service has been
 *            discovered at the peer. It also populates the event with the service related
 *            information before providing it to the application.
 *
 * @param     p_ctx      Pointer to the AMT client structure.
 * @param[in] p_evt Pointer to the event received from the database discovery module.
 *
 */
void nrf_ble_amtc_on_db_disc_evt(nrf_ble_amtc_t * p_ctx, const ble_db_discovery_evt_t * p_evt);


/**@brief     Function for assigning handles to a this instance of rscs_c.
 *
 * @details   Call this function when a link has been established with a peer to
 *            associate this link to this instance of the module. This makes it
 *            possible to handle several link and associate each link to a particular
 *            instance of this module. The connection handle and attribute handles will be
 *            provided from the discovery event @ref NRF_BLE_AMT_C_EVT_DISCOVERY_COMPLETE.
 *
 * @param     p_ctx          Pointer to the AMT client structure.
 * @param[in] conn_handle    Connection handle to associated with the given AMT Instance.
 * @param[in] p_peer_handles Attribute handles on the AMT server that you want this AMT client to
 *                           interact with.
 */
ret_code_t nrf_ble_amtc_handles_assign(nrf_ble_amtc_t    * p_ctx,
                                       uint16_t            conn_handle,
                                       nrf_ble_amtc_db_t * p_peer_handles);


/**@brief     Function for reading the Bytes Received Count (RCB) characteristic.
 *
 * @details   Call this function to read the RCB characteristic value on the peer.
 *
 * @param     p_ctx          Pointer to the AMT client structure.
 */
ret_code_t nrf_ble_amtc_rcb_read(nrf_ble_amtc_t * p_ctx);



/** @} */ // End tag for the Client.


/**************************************************************************************************/
/**************************************************************************************************/

/**
 * @defgroup nrf_ble_amt ATT MTU Throughput (AMT) Service Server
 * @{
 * @ingroup ble_sdk_srv
 *
 * @details  This module contains the APIs and types exposed by the ATT MTU Throughput
 *           Service Server module.
 *
 * @note     The application must propagate BLE stack events to this module by calling
 *           amt_on_ble_evt().
 */

#define BLE_AMTS_BLE_OBSERVER_PRIO   3


/**@brief AMT Server event type. */
typedef enum
{
    NRF_BLE_AMTS_EVT_NOTIF_ENABLED,
    NRF_BLE_AMTS_EVT_NOTIF_DISABLED,
    NRF_BLE_AMTS_EVT_TRANSFER_1KB,
    NRF_BLE_AMTS_EVT_TRANSFER_FINISHED,
} nrf_ble_amts_evt_type_t;


/**@brief AMTS Event structure. */
typedef struct
{
    nrf_ble_amts_evt_type_t evt_type;               //!< Type of the event. */
    uint32_t                bytes_transfered_cnt;   //!< Number of bytes sent during the transfer*/
} nrf_ble_amts_evt_t;


/**@brief AMTS module event handler type.
 * The AMTS module will call this function when notifications have been enabled/disabled, for each Kilobytes sent and at the end of the tranfer.
*/
typedef void (*amts_evt_handler_t)(nrf_ble_amts_evt_t);


/**@brief AMTS structure.
 * @details This structure contains status information for the AMTS module. */
typedef struct
{
    uint16_t                 conn_handle;            //!< Connection handle as provided by the SoftDevice. */
    uint8_t                  uuid_type;              //!< UUID type. */
    ble_gatts_char_handles_t amts_char_handles;      //!< AMT characteristic handles */
    ble_gatts_char_handles_t amt_rbc_char_handles;   //!< Received Bytes Count Characteristic handles. */
    amts_evt_handler_t       evt_handler;            //!< Application event handler to be called when there is an event related to the AMTS module. */
    bool                     busy;                   //!< busy flag, indicates that the hvx function returned busy and that there are still data to be transfered. */
    uint16_t                 max_payload_len;        //!< Maximum number of bytes which can be sent in one notification. */
    uint32_t                 kbytes_sent;            //!< Number of Kilobytes sent. */
    uint32_t                 bytes_sent;             //!< Number of bytes sent. */
} nrf_ble_amts_t;



/**@brief   Function for handling BLE events from the SoftDevice.
 *
 * @details This function will handle the BLE events received from the SoftDevice.
 *          If a BLE event is relevant to the AMTS module, then it uses it to update
 *          interval variables and, if necessary, send events to the application.
 *
 * @param[in] p_ble_evt     Pointer to the BLE event.
 * @param     p_context     Pointer to the AMTS structure.
 */
void nrf_ble_amts_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);


/**@brief   Function for initializing the ATT MTU Throughput Service module.
 *
 * @details This function will initialize the module and set up the ATT MTU Throughput Service.
 *
 * @param[out] p_ctx        Pointer to the AMTS structure.
 * @param[in]  evt_handler  Event handler.
 *
 * @retval     NRF_SUCCESS      Operation success.
 * @retval     NRF_ERROR_NULL   A parameter is NULL.
 */
void nrf_ble_amts_init(nrf_ble_amts_t * p_ctx, amts_evt_handler_t evt_handler);


/**@brief   Function for sending AMT_BYTE_TRANSFER_CNT bytes via notifications.
 *
 * @details Call this function to start sending notifications until AMT_BYTE_TRANSFER_CNT bytes
 *          have been sent. Each notification will contain @p p_ctx::max_payload_len bytes of
 *          ATT payload.
 *
 * @param   p_ctx   Pointer to the AMTS structure.
 */
void nrf_ble_amts_notif_spam(nrf_ble_amts_t * p_ctx);


/**@brief   Function for setting the the number of bytes received.
 *
 * @details Call this function to update the number of bytes received.
 *
 * @param       p_ctx     Pointer to the AMTS structure.
 * @param[in]   byte_cnt  Number of bytes received.
 */
void nrf_ble_amts_rbc_set(nrf_ble_amts_t * p_ctx, uint32_t byte_cnt);


/**@brief   Function for handling the GATT module's events.
 *
 * @details Handles all events from the GATT module of interest to the AMT Service.
 *
 * @param       p_ctx         Pointer to the AMTS structure.
 * @param[in]   p_gatt_evt    Event received from the GATT module.
 */
void nrf_ble_amts_on_gatt_evt(nrf_ble_amts_t * p_ctx, nrf_ble_gatt_evt_t const * p_gatt_evt);

/** @} */ // End tag for the Server.


#ifdef __cplusplus
}
#endif

#endif // AMT_H__

/** @}
 *  @endcond
 */
