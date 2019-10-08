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
/**@cond To Make Doxygen skip documentation generation for this file.
 * @{
 */
#include "sdk_config.h"
#include "ble_db_discovery.h"
#include "ble_types.h"
#include "ble_srv_common.h"
#include "ble_gattc.h"
#include "sdk_common.h"
#include "amt.h"

#define NRF_LOG_MODULE_NAME AMTC
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define TX_BUFFER_MASK         0x07                  /**< TX Buffer mask, must be a mask of continuous zeroes, followed by continuous sequence of ones: 000...111. */
#define TX_BUFFER_SIZE         (TX_BUFFER_MASK + 1)  /**< Size of send buffer, which is 1 higher than the mask. */

#define WRITE_MESSAGE_LENGTH   BLE_CCCD_VALUE_LEN    /**< Length of the write message for CCCD. */

typedef enum
{
    READ_REQ,  /**< Type identifying that this tx_message is a read request. */
    WRITE_REQ  /**< Type identifying that this tx_message is a write request. */
} tx_request_t;

/**@brief Structure for writing a message to the peer, i.e. CCCD.
 */
typedef struct
{
    uint8_t                  gattc_value[WRITE_MESSAGE_LENGTH];  /**< The message to write. */
    ble_gattc_write_params_t gattc_params;                       /**< GATTC parameters for this message. */
} write_params_t;

/**@brief Structure for holding data to be transmitted to the connected central.
 */
typedef struct
{
    uint16_t     conn_handle;  /**< Connection handle to be used when transmitting this message. */
    tx_request_t type;         /**< Type of this message, i.e. read or write message. */
    union
    {
        uint16_t       read_handle;  /**< Read request message. */
        write_params_t write_req;    /**< Write request message. */
    } req;
} tx_message_t;


static tx_message_t   m_tx_buffer[TX_BUFFER_SIZE];  /**< Transmit buffer for messages to be transmitted to the central. */
static uint32_t       m_tx_insert_index = 0;        /**< Current index in the transmit buffer where the next message should be inserted. */
static uint32_t       m_tx_index = 0;               /**< Current index in the transmit buffer from where the next message to be transmitted resides. */


/**@brief Function for passing any pending request from the buffer to the stack.
 */
static void tx_buffer_process(void)
{
    if (m_tx_index != m_tx_insert_index)
    {
        uint32_t err_code;

        if (m_tx_buffer[m_tx_index].type == READ_REQ)
        {
            err_code = sd_ble_gattc_read(m_tx_buffer[m_tx_index].conn_handle,
                                         m_tx_buffer[m_tx_index].req.read_handle,
                                         0);
        }
        else
        {
            err_code = sd_ble_gattc_write(m_tx_buffer[m_tx_index].conn_handle,
                                          &m_tx_buffer[m_tx_index].req.write_req.gattc_params);
        }

        if (err_code == NRF_SUCCESS)
        {
            m_tx_index++;
            m_tx_index &= TX_BUFFER_MASK;
        }
        else
        {
            NRF_LOG_DEBUG("SD Read/Write API returns error, will retry later.");
        }
    }
}


/**@brief     Function for handling Handle Value Notification received from the SoftDevice.
 *
 * @details   This function will uses the Handle Value Notification received from the SoftDevice
 *            and checks if it is a notification of the AMT from the peer.
 *            If it is, this function will send an event to the peer.
 *
 * @param[in] p_ctx        Pointer to the AMT Client structure.
 * @param[in] p_ble_evt    Pointer to the BLE event received.
 */
static void on_hvx(nrf_ble_amtc_t * p_ctx, ble_evt_t const * p_ble_evt)
{
    // Check if the event if on the link for this instance
    if (p_ctx->conn_handle != p_ble_evt->evt.gattc_evt.conn_handle)
    {
        return;
    }

    // Check if this is a AMT notification.
    if (p_ble_evt->evt.gattc_evt.params.hvx.handle == p_ctx->peer_db.amt_handle)
    {
        nrf_ble_amtc_evt_t amt_c_evt;
        p_ctx->bytes_rcvd_cnt           += p_ble_evt->evt.gattc_evt.params.hvx.len;
        amt_c_evt.evt_type              = NRF_BLE_AMT_C_EVT_NOTIFICATION;
        amt_c_evt.conn_handle           = p_ble_evt->evt.gattc_evt.conn_handle;
        amt_c_evt.params.hvx.notif_len  = p_ble_evt->evt.gattc_evt.params.hvx.len;
        amt_c_evt.params.hvx.bytes_sent = uint32_decode(p_ble_evt->evt.gattc_evt.params.hvx.data);
        amt_c_evt.params.hvx.bytes_rcvd = p_ctx->bytes_rcvd_cnt;
        p_ctx->evt_handler(p_ctx, &amt_c_evt);
    }
}


/**@brief     Function for handling read response event received from the SoftDevice.
 *
 * @details   This function will uses the read response received from the SoftDevice
 *            and checks if it is a read response of the AMT from the peer.
 *            If it is, this function will send an event to the peer.
 *
 * @param[in] p_ctx        Pointer to the AMT Client structure.
 * @param[in] p_ble_evt    Pointer to the BLE event received.
 */
static void on_read_response(nrf_ble_amtc_t * p_ctx, ble_evt_t const * p_ble_evt)
{
    // Check if the event if on the link for this instance
    if (p_ctx->conn_handle != p_ble_evt->evt.gattc_evt.conn_handle)
    {
        return;
    }

    // Check if this is a AMT RCB read response.
    if (p_ble_evt->evt.gattc_evt.params.read_rsp.handle == p_ctx->peer_db.amt_rbc_handle)
    {
        nrf_ble_amtc_evt_t amt_c_evt;
        amt_c_evt.evt_type             = NRF_BLE_AMT_C_EVT_RBC_READ_RSP;
        amt_c_evt.conn_handle          = p_ble_evt->evt.gattc_evt.conn_handle;
        amt_c_evt.params.rcv_bytes_cnt = uint32_decode(p_ble_evt->evt.gattc_evt.params.read_rsp.data);
        p_ctx->evt_handler(p_ctx, &amt_c_evt);
    }
}


/**@brief     Function for handling write response event received from the SoftDevice.
 *
 * @details   This function will uses the read response received from the SoftDevice
 *            and checks if it is a write response of the AMT from the peer.
 *
 * @param[in] p_ctx        Pointer to the AMT Client structure.
 * @param[in] p_ble_evt    Pointer to the BLE event received.
 */
static void on_write_response(nrf_ble_amtc_t * p_ctx, ble_evt_t const * p_ble_evt)
{
    // Check if the event if on the link for this instance
    if (p_ctx->conn_handle != p_ble_evt->evt.gattc_evt.conn_handle)
    {
        return;
    }

    // Check if this is a write response on the CCCD.
    if (p_ble_evt->evt.gattc_evt.params.write_rsp.handle == p_ctx->peer_db.amt_cccd_handle)
    {
        NRF_LOG_DEBUG("CCCD configured.");
    }
}


void nrf_ble_amtc_on_db_disc_evt(nrf_ble_amtc_t * p_ctx, ble_db_discovery_evt_t const * p_evt)
{
    // Check if the AMT service was discovered.
    if (   (p_evt->evt_type != BLE_DB_DISCOVERY_COMPLETE)
        || (p_evt->params.discovered_db.srv_uuid.uuid != AMT_SERVICE_UUID)
        || (p_evt->params.discovered_db.srv_uuid.type != p_ctx->uuid_type))
    {
        return;
    }

    nrf_ble_amtc_evt_t evt;
    evt.conn_handle = p_evt->conn_handle;

    // Find the CCCD Handle of the AMT characteristic.
    for (uint32_t i = 0; i < p_evt->params.discovered_db.char_count; i++)
    {
        ble_uuid_t uuid = p_evt->params.discovered_db.charateristics[i].characteristic.uuid;

        if ((uuid.uuid == AMTS_CHAR_UUID) && (uuid.type == p_ctx->uuid_type))
        {
            // Found AMT characteristic. Store handles.
            evt.params.peer_db.amt_cccd_handle =
                p_evt->params.discovered_db.charateristics[i].cccd_handle;
            evt.params.peer_db.amt_handle =
                p_evt->params.discovered_db.charateristics[i].characteristic.handle_value;
        }

        if ((uuid.uuid == AMT_RCV_BYTES_CNT_CHAR_UUID) && (uuid.type == p_ctx->uuid_type))
        {
            // Found AMT Number of received bytes characteristic. Store handles.
            evt.params.peer_db.amt_rbc_handle =
                p_evt->params.discovered_db.charateristics[i].characteristic.handle_value;
        }
    }

    NRF_LOG_DEBUG("AMT service discovered at peer.");

    //If the instance has been assigned prior to db_discovery, assign the db_handles.
    if (p_ctx->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        if (   (p_ctx->peer_db.amt_cccd_handle == BLE_GATT_HANDLE_INVALID)
            && (p_ctx->peer_db.amt_handle == BLE_GATT_HANDLE_INVALID))
        {
            p_ctx->peer_db = evt.params.peer_db;
        }
    }

    p_ctx->bytes_rcvd_cnt = 0;

    evt.evt_type = NRF_BLE_AMT_C_EVT_DISCOVERY_COMPLETE;
    p_ctx->evt_handler(p_ctx, &evt);
}


ret_code_t nrf_ble_amtc_init(nrf_ble_amtc_t * p_ctx, nrf_ble_amtc_evt_handler_t evt_handler)
{
    VERIFY_PARAM_NOT_NULL(p_ctx);
    VERIFY_PARAM_NOT_NULL(evt_handler);

    ble_uuid128_t base_uuid = {SERVICE_UUID_BASE};
    ble_uuid_t    amt_uuid;

    ret_code_t err_code = sd_ble_uuid_vs_add(&base_uuid, &p_ctx->uuid_type);
    VERIFY_SUCCESS(err_code);

    amt_uuid.type = p_ctx->uuid_type;
    amt_uuid.uuid = AMT_SERVICE_UUID;

    p_ctx->evt_handler             = evt_handler;
    p_ctx->bytes_rcvd_cnt          = 0;
    p_ctx->conn_handle             = BLE_CONN_HANDLE_INVALID;
    p_ctx->peer_db.amt_cccd_handle = BLE_GATT_HANDLE_INVALID;
    p_ctx->peer_db.amt_handle      = BLE_GATT_HANDLE_INVALID;
    p_ctx->conn_handle             = BLE_CONN_HANDLE_INVALID;
    p_ctx->peer_db.amt_rbc_handle  = BLE_GATT_HANDLE_INVALID;

    return ble_db_discovery_evt_register(&amt_uuid);
}


ret_code_t nrf_ble_amtc_handles_assign(nrf_ble_amtc_t   * p_ctx,
                                      uint16_t            conn_handle,
                                      nrf_ble_amtc_db_t * p_peer_handles)
{
    VERIFY_PARAM_NOT_NULL(p_ctx);
    p_ctx->conn_handle = conn_handle;
    if (p_peer_handles != NULL)
    {
        p_ctx->peer_db = *p_peer_handles;
    }

    return NRF_SUCCESS;
}


/**@brief     Function for handling Disconnected event received from the SoftDevice.
 *
 * @details   This function check if the disconnect event is happening on the link
 *            associated with the current instance of the module, if so it will set its
 *            conn_handle to invalid.
 *
 * @param[in] p_ctx       Pointer to the AMT Client structure.
 * @param[in] p_ble_evt   Pointer to the BLE event received.
 */
static void on_disconnected(nrf_ble_amtc_t * p_ctx, ble_evt_t const * p_ble_evt)
{
    if (p_ctx->conn_handle != p_ble_evt->evt.gap_evt.conn_handle)
    {
        return;
    }

    p_ctx->conn_handle             = BLE_CONN_HANDLE_INVALID;
    p_ctx->peer_db.amt_cccd_handle = BLE_GATT_HANDLE_INVALID;
    p_ctx->peer_db.amt_handle      = BLE_GATT_HANDLE_INVALID;
    p_ctx->peer_db.amt_rbc_handle  = BLE_GATT_HANDLE_INVALID;
    p_ctx->bytes_rcvd_cnt          = 0;
}


void nrf_ble_amtc_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    nrf_ble_amtc_t * p_ctx = (nrf_ble_amtc_t *)p_context;

    if ((p_ctx == NULL) || (p_ble_evt == NULL))
    {
        return;
    }

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GATTC_EVT_HVX:
            on_hvx(p_ctx, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnected(p_ctx, p_ble_evt);
            break;

        case BLE_GATTC_EVT_WRITE_RSP:
            on_write_response(p_ctx, p_ble_evt);
            break;

        case BLE_GATTC_EVT_READ_RSP:
            on_read_response(p_ctx, p_ble_evt);
            break;

        default:
            break;
    }
    // Check if the event if on the link for this instance
    if (p_ctx->conn_handle != p_ble_evt->evt.gattc_evt.conn_handle)
    {
        return;
    }
    // Check if there is any message to be sent across to the peer and send it.
    tx_buffer_process();

}


/**@brief Function for creating a message for writing to the CCCD.
 */
static ret_code_t cccd_configure(uint16_t conn_handle, uint16_t handle_cccd, bool enable)
{
    NRF_LOG_DEBUG("Configuring CCCD. CCCD Handle = %d, Connection Handle = %d",
        handle_cccd, conn_handle);

    tx_message_t * p_msg;
    uint16_t       cccd_val = enable ? BLE_GATT_HVX_NOTIFICATION : 0;

    p_msg              = &m_tx_buffer[m_tx_insert_index++];
    m_tx_insert_index &= TX_BUFFER_MASK;

    p_msg->req.write_req.gattc_params.handle   = handle_cccd;
    p_msg->req.write_req.gattc_params.len      = WRITE_MESSAGE_LENGTH;
    p_msg->req.write_req.gattc_params.p_value  = p_msg->req.write_req.gattc_value;
    p_msg->req.write_req.gattc_params.offset   = 0;
    p_msg->req.write_req.gattc_params.write_op = BLE_GATT_OP_WRITE_REQ;
    p_msg->req.write_req.gattc_value[0]        = LSB_16(cccd_val);
    p_msg->req.write_req.gattc_value[1]        = MSB_16(cccd_val);
    p_msg->conn_handle                         = conn_handle;
    p_msg->type                                = WRITE_REQ;

    tx_buffer_process();

    return NRF_SUCCESS;
}


ret_code_t nrf_ble_amtc_notif_enable(nrf_ble_amtc_t * p_ctx)
{
    VERIFY_PARAM_NOT_NULL(p_ctx);

    if (p_ctx->conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    return cccd_configure(p_ctx->conn_handle, p_ctx->peer_db.amt_cccd_handle, true);
}


ret_code_t nrf_ble_amtc_rcb_read(nrf_ble_amtc_t * p_ctx)
{
    VERIFY_PARAM_NOT_NULL(p_ctx);

    if (p_ctx->conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    tx_message_t * p_msg = &m_tx_buffer[m_tx_insert_index++];

    p_msg->req.read_handle = p_ctx->peer_db.amt_rbc_handle;
    p_msg->conn_handle     = p_ctx->conn_handle;
    p_msg->type            = READ_REQ;

    m_tx_insert_index &= TX_BUFFER_MASK;
    tx_buffer_process();

    return NRF_SUCCESS;
}

/** @}
 *  @endcond
 */
