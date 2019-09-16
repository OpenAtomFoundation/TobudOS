/**
 * Copyright (c) 2016 - 2017, Nordic Semiconductor ASA
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
#include "nrf_ble_dfu.h"

#include <stddef.h>
#include "boards.h"
#include "sdk_common.h"
#include "nrf_dfu_transport.h"
#include "nrf_dfu_req_handler.h"
#include "nrf_dfu_handling_error.h"
#include "nrf_dfu_mbr.h"
#include "nrf_bootloader_info.h"
#include "ble_conn_params.h"
#include "ble_hci.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_log.h"
#include "nrf_delay.h"
#include "app_timer.h"
#include "nrf_dfu_svci.h"
#include "nrf_dfu_settings.h"


#define ADVERTISING_LED_PIN_NO              BSP_LED_0                                               /**< Is on when device is advertising. */
#define CONNECTED_LED_PIN_NO                BSP_LED_1                                               /**< Is on when device has connected. */

#define DEVICE_NAME                         "DfuTarg"                                               /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME                   "NordicSemiconductor"                                   /**< Manufacturer. Will be passed to Device Information Service. */

#define MIN_CONN_INTERVAL                   (uint16_t)(MSEC_TO_UNITS(7.5, UNIT_1_25_MS))            /**< Minimum acceptable connection interval. */
#define MAX_CONN_INTERVAL                   (uint16_t)(MSEC_TO_UNITS(30, UNIT_1_25_MS))             /**< Maximum acceptable connection interval. */
#define SLAVE_LATENCY                       0                                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                    (4 * 100)                                               /**< Connection supervisory timeout (4 seconds). */

#define MAX_ADV_DATA_LENGTH                 20                                                      /**< Maximum length of advertising data. */

#define APP_ADV_INTERVAL                    MSEC_TO_UNITS(25, UNIT_0_625_MS)                        /**< The advertising interval (25 ms.). */
#define APP_ADV_TIMEOUT_IN_SECONDS          BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED                   /**< The advertising timeout in units of seconds. This is set to @ref BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED so that the advertisement is done as long as there there is a call to @ref dfu_transport_close function.*/

#define APP_FEATURE_NOT_SUPPORTED           BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2                    /**< Reply when unsupported features are requested. */

#define SEC_PARAM_BOND                      0                                                       /**< Perform bonding. */
#define SEC_PARAM_MITM                      0                                                       /**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES           BLE_GAP_IO_CAPS_NONE                                    /**< No I/O capabilities. */
#define SEC_PARAM_OOB                       0                                                       /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE              7                                                       /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE              16                                                      /**< Maximum encryption key size. */

#define MAX_DFU_PKT_LEN                     (20)                                                    /**< Maximum length (in bytes) of the DFU Packet characteristic. */
#define PKT_CREATE_PARAM_LEN                (6)                                                     /**< Length (in bytes) of the parameters for Create Object request. */
#define PKT_SET_PRN_PARAM_LEN               (3)                                                     /**< Length (in bytes) of the parameters for Set Packet Receipt Notification request. */
#define PKT_READ_OBJECT_INFO_PARAM_LEN      (2)                                                     /**< Length (in bytes) of the parameters for Read Object Info request. */
#define MAX_RESPONSE_LEN                    (17)                                                    /**< Maximum length (in bytes) of the response to a Control Point command. */

#define DFU_BLE_FLAG_NONE                   (0)
#define DFU_BLE_FLAG_SERVICE_INITIALIZED    (1 << 0)                                                /**< Flag to check if the DFU service was initialized by the application.*/
#define DFU_BLE_FLAG_USE_ADV_NAME           (1 << 1)                                                /**< Flag to indicate that advertisement name is to be used. */
#define DFU_BLE_RESETTING_SOON              (1 << 2)                                                /**< Flag to indicate that the device will reset soon. */

#define BLE_OBSERVER_PRIO                   2                                                       /**< BLE observer priority. Controls the priority for BLE event handler. */

DFU_TRANSPORT_REGISTER(nrf_dfu_transport_t const dfu_trans) =
{
    .init_func  = ble_dfu_transport_init,
    .close_func = ble_dfu_transport_close
};

static ble_dfu_t m_dfu;                                                                             /**< Structure used to identify the Device Firmware Update service. */
static uint16_t  m_pkt_notif_target;                                                                /**< Number of packets of firmware data to be received before transmitting the next Packet Receipt Notification to the DFU Controller. */
static uint16_t  m_pkt_notif_target_cnt;                                                            /**< Number of packets of firmware data received after sending last Packet Receipt Notification or since the receipt of a @ref BLE_DFU_PKT_RCPT_NOTIF_ENABLED event from the DFU service, which ever occurs later.*/
static uint16_t  m_conn_handle = BLE_CONN_HANDLE_INVALID;                                           /**< Handle of the current connection. */
static uint32_t  m_flags;
static uint8_t   m_notif_buffer[MAX_RESPONSE_LEN];                                                  /**< Buffer used for sending notifications to peer. */

#if defined(NRF_DFU_BLE_REQUIRES_BONDS) && (NRF_DFU_BLE_REQUIRES_BONDS == 1)
static nrf_dfu_peer_data_t         m_peer_data;
static ble_gap_addr_t      const * m_whitelist[1];                                                  /**< List of peers in whitelist (only one) */
static ble_gap_id_key_t    const * m_gap_ids[1];
#else
static nrf_dfu_adv_name_t          m_adv_name;
#endif


/**@brief     Function for the Advertising functionality initialization.
 *
 * @details   Encodes the required advertising data and passes it to the stack.
 *            The advertising data encoded here is specific for DFU.
 *            Setting advertising data can by done by calling @ref ble_advdata_set.
 */
static uint32_t advertising_init(uint8_t adv_flags)
{
    uint32_t err_code;
    uint16_t len_advdata                = 9;
    uint16_t max_device_name_length     = MAX_ADV_DATA_LENGTH - len_advdata;
    uint16_t actual_device_name_length  = max_device_name_length;

    uint8_t p_encoded_advdata[MAX_ADV_DATA_LENGTH];

    // Encode flags.
    p_encoded_advdata[0] = 0x2;
    p_encoded_advdata[1] = BLE_GAP_AD_TYPE_FLAGS;
    p_encoded_advdata[2] = adv_flags;

    // Encode 'more available' UUID list.
    p_encoded_advdata[3] = 0x3;
    p_encoded_advdata[4] = BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE;
    p_encoded_advdata[5] = LSB_16(BLE_DFU_SERVICE_UUID);
    p_encoded_advdata[6] = MSB_16(BLE_DFU_SERVICE_UUID);

    // Get GAP device name and length.
    err_code = sd_ble_gap_device_name_get(&p_encoded_advdata[9], &actual_device_name_length);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Set GAP device in advertising data.
    if (actual_device_name_length <= max_device_name_length)
    {
        p_encoded_advdata[7] = actual_device_name_length + 1; // (actual_length + ADV_AD_TYPE_FIELD_SIZE(1))
        p_encoded_advdata[8] = BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME;
        len_advdata += actual_device_name_length;
    }
    else
    {
        // Must use a shorter advertising name than the actual name of the device.
        p_encoded_advdata[7] = max_device_name_length + 1; // (length + ADV_AD_TYPE_FIELD_SIZE(1))
        p_encoded_advdata[8] = BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME;
        len_advdata = MAX_ADV_DATA_LENGTH;
    }

    return sd_ble_gap_adv_data_set(p_encoded_advdata, len_advdata, NULL, 0);
}


/**@brief Function for starting advertising.
 */
static uint32_t advertising_start(void)
{
    uint32_t err_code;
    ble_gap_adv_params_t adv_params =
    {
        .type        = BLE_GAP_ADV_TYPE_ADV_IND,
        .p_peer_addr = NULL,
        .fp          = BLE_GAP_ADV_FP_ANY,
        .interval    = APP_ADV_INTERVAL,
        .timeout     = APP_ADV_TIMEOUT_IN_SECONDS,
    };
    uint8_t  adv_flag = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

#if defined(NRF_DFU_BLE_REQUIRES_BONDS) && (NRF_DFU_BLE_REQUIRES_BONDS == 1)
    ble_gap_irk_t empty_irk = {{0}};

    NRF_LOG_DEBUG("##### Setting adv with peer data ####");
    if (memcmp(m_peer_data.ble_id.id_info.irk, empty_irk.irk, sizeof(ble_gap_irk_t)) == 0)
    {
        NRF_LOG_DEBUG("##### No IRK, General discovery");
    }
    else
    {
        NRF_LOG_DEBUG("##### IRK Found. Setting whitelist ####");

        // API versions lower than 3 are not supported.
        STATIC_ASSERT(NRF_SD_BLE_API_VERSION >= 3);

        m_whitelist[0] = &m_peer_data.ble_id.id_addr_info;
        err_code = sd_ble_gap_whitelist_set(m_whitelist, 1);
        VERIFY_SUCCESS(err_code);

        m_gap_ids[0] = &m_peer_data.ble_id;
        sd_ble_gap_device_identities_set(m_gap_ids, NULL, 1);
        VERIFY_SUCCESS(err_code);

        adv_flag      = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
        adv_params.fp = BLE_GAP_ADV_FP_FILTER_CONNREQ;
    }
#else
    {
        NRF_LOG_DEBUG("#### Advertising NO BONDING ####");
    }
#endif // NRF_DFU_BLE_REQUIRES_BONDS

    err_code = advertising_init(adv_flag);
    VERIFY_SUCCESS(err_code);

    (void) sd_ble_gap_adv_stop();
    err_code = sd_ble_gap_adv_start(&adv_params, BLE_CONN_CFG_TAG_DEFAULT);
    VERIFY_SUCCESS(err_code);

    nrf_gpio_pin_clear(ADVERTISING_LED_PIN_NO);
    nrf_gpio_pin_set(CONNECTED_LED_PIN_NO);

    return NRF_SUCCESS;
}


/**@brief Function for stopping advertising.
 */
static void advertising_stop(void)
{
    (void)sd_ble_gap_adv_stop();
    nrf_gpio_pin_set(ADVERTISING_LED_PIN_NO);

}


static bool is_cccd_configured(ble_dfu_t * p_dfu)
{
    uint8_t  cccd_val_buf[BLE_CCCD_VALUE_LEN];
    ble_gatts_value_t gatts_value = {0};

    gatts_value.len     = BLE_CCCD_VALUE_LEN;
    gatts_value.p_value = cccd_val_buf;

    // Check the CCCD Value of DFU Control Point.
    uint32_t err_code = sd_ble_gatts_value_get(m_conn_handle,
                                               p_dfu->dfu_ctrl_pt_handles.cccd_handle,
                                               &gatts_value);
    VERIFY_SUCCESS(err_code);

    return ble_srv_is_notification_enabled(cccd_val_buf);
}


static uint32_t send_hvx(uint16_t conn_handle, uint16_t value_handle, uint16_t len)
{
    ble_gatts_hvx_params_t hvx_params = {0};

    hvx_params.handle = value_handle;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
    hvx_params.p_len  = &len;
    hvx_params.p_data = m_notif_buffer;

    return sd_ble_gatts_hvx(conn_handle, &hvx_params);
}


#if defined(NRF_DFU_BLE_REQUIRES_BONDS) && (NRF_DFU_BLE_REQUIRES_BONDS == 1)
static uint32_t service_changed_send(void)
{
    uint32_t err_code;

    NRF_LOG_DEBUG("Sending Service Changed indication");

    err_code = sd_ble_gatts_sys_attr_set(m_conn_handle,
                                         m_peer_data.sys_serv_attr,
                                         sizeof(m_peer_data.sys_serv_attr),
                                         BLE_GATTS_SYS_ATTR_FLAG_SYS_SRVCS);
    VERIFY_SUCCESS(err_code);


    err_code = sd_ble_gatts_sys_attr_set(m_conn_handle,
                                         NULL,
                                         0,
                                         BLE_GATTS_SYS_ATTR_FLAG_USR_SRVCS);

    VERIFY_SUCCESS(err_code);

    err_code = sd_ble_gatts_service_changed(m_conn_handle, m_dfu.service_handle, 0xFFFF);

    if ((err_code == BLE_ERROR_INVALID_CONN_HANDLE) ||
        (err_code == NRF_ERROR_INVALID_STATE) ||
        (err_code == NRF_ERROR_BUSY))
    {
        // Those errors can be expected when sending trying to send Service Changed indication
        // if the CCCD is not set to indicate. Thus set the returning error code to success.
        NRF_LOG_WARNING("Client did not have the Service Changed indication set to enabled. Error: 0x%08x", err_code);
        err_code = NRF_SUCCESS;
    }

    return err_code;
}
#endif


static uint32_t response_send(ble_dfu_t          * p_dfu,
                              uint8_t              op_code,
                              nrf_dfu_res_code_t   resp_val)
{
    uint16_t index = 0;

    NRF_LOG_DEBUG("Sending Response: [0x%01x, 0x%01x]", op_code, resp_val);

    ASSERT(p_dfu != NULL);

    if ((m_conn_handle == BLE_CONN_HANDLE_INVALID) || (m_flags & DFU_BLE_FLAG_SERVICE_INITIALIZED) == 0)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    m_notif_buffer[index++] = BLE_DFU_OP_CODE_RESPONSE;

    // Encode the Request Op code.
    m_notif_buffer[index++] = op_code;

    // Encode the Response Value.
    m_notif_buffer[index++] = (uint8_t)resp_val;

    // If the error was an extended error code, add the error to the response.
    if (resp_val == NRF_DFU_RES_CODE_EXT_ERROR)
    {
        m_notif_buffer[index++] = ext_error_get();
        // Clear the last extended error code.
        (void) ext_error_set(NRF_DFU_EXT_ERROR_NO_ERROR);
    }

    return send_hvx(m_conn_handle, p_dfu->dfu_ctrl_pt_handles.value_handle, index);
}


static uint32_t response_crc_cmd_send(ble_dfu_t * p_dfu,
                                      uint32_t    offset,
                                      uint32_t    crc)
{
    uint16_t index = 0;

    NRF_LOG_DEBUG("Sending CRC: [0x60, 0x03, 0x01, 0:x%08x, CRC:0x%08x]", offset, crc);

    ASSERT(p_dfu != NULL);

    if ((m_conn_handle == BLE_CONN_HANDLE_INVALID) || (m_flags & DFU_BLE_FLAG_SERVICE_INITIALIZED) == 0)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    m_notif_buffer[index++] = BLE_DFU_OP_CODE_RESPONSE;

    // Encode the Request Op code
    m_notif_buffer[index++] = BLE_DFU_OP_CODE_CALCULATE_CRC;

    // Encode the Response Value.
    m_notif_buffer[index++] = (uint8_t)NRF_DFU_RES_CODE_SUCCESS;

    // Encode the Offset Value.
    index += uint32_encode(offset, &m_notif_buffer[index]);

    // Encode the Crc Value.
    index += uint32_encode(crc, &m_notif_buffer[index]);

    return send_hvx(m_conn_handle, p_dfu->dfu_ctrl_pt_handles.value_handle, index);
}


static uint32_t response_select_object_cmd_send(ble_dfu_t * p_dfu,
                                                uint32_t    max_size,
                                                uint32_t    offset,
                                                uint32_t    crc)
{
    uint16_t index = 0;

    NRF_LOG_DEBUG("Sending Object Info: [0x60, 0x06, 0x01 max: 0:x%08x 0:x%08x, CRC:0x%08x]",
                  max_size, offset, crc);

    ASSERT(p_dfu != NULL);

    if ((m_conn_handle == BLE_CONN_HANDLE_INVALID) || (m_flags & DFU_BLE_FLAG_SERVICE_INITIALIZED) == 0)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    m_notif_buffer[index++] = BLE_DFU_OP_CODE_RESPONSE;

    // Encode the Request Op code.
    m_notif_buffer[index++] = BLE_DFU_OP_CODE_SELECT_OBJECT;

    // Encode the Success Response Value.
    m_notif_buffer[index++] = (uint8_t)NRF_DFU_RES_CODE_SUCCESS;

    // Encode the Max Size Value.
    index += uint32_encode(max_size, &m_notif_buffer[index]);

    // Encode the Offset Value.
    index += uint32_encode(offset, &m_notif_buffer[index]);

    // Encode the CRC Value.
    index += uint32_encode(crc, &m_notif_buffer[index]);

    return send_hvx(m_conn_handle, p_dfu->dfu_ctrl_pt_handles.value_handle, index);
}


/**@brief     Function for handling a Write event on the Control Point characteristic.
 *
 * @param[in] p_dfu             DFU Service Structure.
 * @param[in] p_ble_write_evt   Pointer to the write event received from BLE stack.
 *
 * @return    NRF_SUCCESS on successful processing of control point write. Otherwise an error code.
 */
static uint32_t on_ctrl_pt_write(ble_dfu_t * p_dfu, ble_gatts_evt_write_t const * p_ble_write_evt)
{
    nrf_dfu_res_code_t  res_code;
    nrf_dfu_req_t       dfu_req;
    nrf_dfu_res_t       dfu_res = {{{0}}};

    memset(&dfu_req, 0, sizeof(nrf_dfu_req_t));

    switch (p_ble_write_evt->data[0])
    {
        case BLE_DFU_OP_CODE_CREATE_OBJECT:

            if (p_ble_write_evt->len != PKT_CREATE_PARAM_LEN)
            {
                return response_send(p_dfu,
                                     BLE_DFU_OP_CODE_CREATE_OBJECT,
                                     NRF_DFU_RES_CODE_INVALID_PARAMETER);
            }

            NRF_LOG_DEBUG("Received create object");

            // Reset the packet receipt notification on create object
            m_pkt_notif_target_cnt = m_pkt_notif_target;

            // Get type parameter
            //lint -save -e415
            dfu_req.obj_type =  p_ble_write_evt->data[1];
            //lint -restore

            // Get length value
            //lint -save -e416
            dfu_req.object_size = uint32_decode(&(p_ble_write_evt->data[2]));
            //lint -restore

            // Set req type
            dfu_req.req_type = NRF_DFU_OBJECT_OP_CREATE;

            res_code = nrf_dfu_req_handler_on_req(NULL, &dfu_req, &dfu_res);
            return response_send(p_dfu, BLE_DFU_OP_CODE_CREATE_OBJECT, res_code);

        case BLE_DFU_OP_CODE_EXECUTE_OBJECT:
            NRF_LOG_DEBUG("Received execute object");

            // Set req type
            dfu_req.req_type =  NRF_DFU_OBJECT_OP_EXECUTE;

            res_code = nrf_dfu_req_handler_on_req(NULL, &dfu_req, &dfu_res);
            return response_send(p_dfu, BLE_DFU_OP_CODE_EXECUTE_OBJECT, res_code);

        case BLE_DFU_OP_CODE_SET_RECEIPT_NOTIF:
            NRF_LOG_DEBUG("Set receipt notif");
            if (p_ble_write_evt->len != PKT_SET_PRN_PARAM_LEN)
            {
                return (response_send(p_dfu,
                                      BLE_DFU_OP_CODE_SET_RECEIPT_NOTIF,
                                      NRF_DFU_RES_CODE_INVALID_PARAMETER));
            }

            //lint -save -e415
            m_pkt_notif_target = uint16_decode(&(p_ble_write_evt->data[1]));
            //lint -restore
            m_pkt_notif_target_cnt = m_pkt_notif_target;

            return response_send(p_dfu, BLE_DFU_OP_CODE_SET_RECEIPT_NOTIF, NRF_DFU_RES_CODE_SUCCESS);

        case BLE_DFU_OP_CODE_CALCULATE_CRC:
            NRF_LOG_DEBUG("Received calculate CRC");

            dfu_req.req_type =  NRF_DFU_OBJECT_OP_CRC;

            res_code = nrf_dfu_req_handler_on_req(NULL, &dfu_req, &dfu_res);
            if (res_code == NRF_DFU_RES_CODE_SUCCESS)
            {
                return response_crc_cmd_send(p_dfu, dfu_res.offset, dfu_res.crc);
            }
            else
            {
                return response_send(p_dfu, BLE_DFU_OP_CODE_CALCULATE_CRC, res_code);
            }

        case BLE_DFU_OP_CODE_SELECT_OBJECT:

            NRF_LOG_DEBUG("Received select object");
            if (p_ble_write_evt->len != PKT_READ_OBJECT_INFO_PARAM_LEN)
            {
                return response_send(p_dfu,
                                     BLE_DFU_OP_CODE_SELECT_OBJECT,
                                     NRF_DFU_RES_CODE_INVALID_PARAMETER);
            }

            // Set object type to read info about
            //lint -save -e415
            dfu_req.obj_type = p_ble_write_evt->data[1];
            //lint -restore

            dfu_req.req_type = NRF_DFU_OBJECT_OP_SELECT;

            res_code = nrf_dfu_req_handler_on_req(NULL, &dfu_req, &dfu_res);
            if (res_code == NRF_DFU_RES_CODE_SUCCESS)
            {
                return response_select_object_cmd_send(p_dfu, dfu_res.max_size, dfu_res.offset, dfu_res.crc);
            }
            else
            {
                return response_send(p_dfu, BLE_DFU_OP_CODE_SELECT_OBJECT, res_code);
            }

        default:
            NRF_LOG_WARNING("Received unsupported OP code");
            // Unsupported op code.
            return response_send(p_dfu,
                                 p_ble_write_evt->data[0],
                                 NRF_DFU_RES_CODE_INVALID_PARAMETER);
    }
}


/**@brief     Function for handling the @ref BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST event from the
 *            SoftDevice.
 *
 * @param[in] p_dfu     DFU Service Structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static bool on_rw_authorize_req(ble_dfu_t * p_dfu, ble_evt_t const * p_ble_evt)
{
    uint32_t err_code;
    ble_gatts_rw_authorize_reply_params_t auth_reply = {0};

    ble_gatts_evt_rw_authorize_request_t const * p_authorize_request;
    ble_gatts_evt_write_t                const * p_ble_write_evt;

    p_authorize_request = &(p_ble_evt->evt.gatts_evt.params.authorize_request);
    p_ble_write_evt     = &(p_ble_evt->evt.gatts_evt.params.authorize_request.request.write);

    if ((p_authorize_request->type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)                           &&
        (p_authorize_request->request.write.handle == p_dfu->dfu_ctrl_pt_handles.value_handle)  &&
        (p_authorize_request->request.write.op != BLE_GATTS_OP_PREP_WRITE_REQ)                  &&
        (p_authorize_request->request.write.op != BLE_GATTS_OP_EXEC_WRITE_REQ_NOW)              &&
        (p_authorize_request->request.write.op != BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL)            )
    {
        auth_reply.type                = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
        auth_reply.params.write.update = 1;
        auth_reply.params.write.offset = p_ble_write_evt->offset;
        auth_reply.params.write.len    = p_ble_write_evt->len;
        auth_reply.params.write.p_data = p_ble_write_evt->data;

        if (!is_cccd_configured(p_dfu))
        {
            // Send an error response to the peer indicating that the CCCD is improperly configured.
            auth_reply.params.write.gatt_status = BLE_GATT_STATUS_ATTERR_CPS_CCCD_CONFIG_ERROR;

            // Ignore response of auth reply
            (void)sd_ble_gatts_rw_authorize_reply(m_conn_handle, &auth_reply);
            return false;
        }

        auth_reply.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;

        err_code = sd_ble_gatts_rw_authorize_reply(m_conn_handle, &auth_reply);
        return err_code  == NRF_SUCCESS ? true: false;
    }
    else
    {
        return false;
    }
}


/**@brief     Function for handling the @ref BLE_GATTS_EVT_WRITE event from the SoftDevice.
 *
 * @param[in] p_dfu     DFU Service Structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_write(ble_dfu_t * p_dfu, ble_evt_t const * p_ble_evt)
{
    if (p_ble_evt->evt.gatts_evt.params.write.handle == p_dfu->dfu_pkt_handles.value_handle)
    {
        nrf_dfu_res_code_t  res_code;
        nrf_dfu_req_t       dfu_req;
        nrf_dfu_res_t       dfu_res = {{{0}}};

        memset(&dfu_req, 0, sizeof(nrf_dfu_req_t));

        // Set req type
        dfu_req.req_type = NRF_DFU_OBJECT_OP_WRITE;

        // Set data and length
        dfu_req.p_req   = (uint8_t*)p_ble_evt->evt.gatts_evt.params.write.data;
        dfu_req.req_len = p_ble_evt->evt.gatts_evt.params.write.len;

        res_code = nrf_dfu_req_handler_on_req(NULL, &dfu_req, &dfu_res);
        if (res_code != NRF_DFU_RES_CODE_SUCCESS)
        {
            NRF_LOG_ERROR("Failure to run packet write");
        }

        // Check if a packet receipt notification is needed to be sent.
        if (m_pkt_notif_target != 0 && --m_pkt_notif_target_cnt == 0)
        {
            (void)response_crc_cmd_send(p_dfu, dfu_res.offset, dfu_res.crc);

            // Reset the counter for the number of firmware packets.
            m_pkt_notif_target_cnt = m_pkt_notif_target;
        }
    }
}

/**@brief Function for handling the HVC event.
 *
 * @details Handles HVC events from the BLE stack.
 *
 * @param[in] p_dfu      DFU Service structure.
 * @param[in] p_ble_evt  Event received from the BLE stack.
 */
static void on_hvc(ble_dfu_t * p_dfu, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_hvc_t const * p_hvc = &p_ble_evt->evt.gatts_evt.params.hvc;

    if (p_hvc->handle == BLE_UUID_GATT_CHARACTERISTIC_SERVICE_CHANGED)
    {
        NRF_LOG_DEBUG("on_hvx: service_changed handle");
    }
    else
    {
        NRF_LOG_DEBUG("on_hvx: Handle: 0x%04x", p_hvc->handle);
    }
}


/**@brief Function for the Application's SoftDevice event handler.
 *
 * @param[in] p_ble_evt SoftDevice event.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    uint32_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
        {
            nrf_gpio_pin_clear(CONNECTED_LED_PIN_NO);
            nrf_gpio_pin_set(ADVERTISING_LED_PIN_NO);

            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
        } break;

        case BLE_GAP_EVT_DISCONNECTED:
        {
            // Restart advertising so that the DFU Controller can reconnect if possible.
            nrf_gpio_pin_set(CONNECTED_LED_PIN_NO);

            if ((m_flags & DFU_BLE_RESETTING_SOON) == 0)
            {
                err_code = advertising_start();
                APP_ERROR_CHECK(err_code);
            }

            m_conn_handle = BLE_CONN_HANDLE_INVALID;
        } break;

        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
        {
            nrf_dfu_req_handler_reset_if_dfu_complete();
        } break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
        {
            NRF_LOG_DEBUG("Received: BLE_GAP_EVT_SEC_PARAMS_REQUEST");
            uint16_t cccd;
            ble_gatts_value_t value =
            {
                .len     = BLE_CCCD_VALUE_LEN,
                .offset  = 0,
                .p_value = (uint8_t*)&cccd
            };

            err_code = sd_ble_gatts_value_get(m_conn_handle, BLE_UUID_GATT_CHARACTERISTIC_SERVICE_CHANGED, &value);
            APP_ERROR_CHECK(err_code);

            NRF_LOG_DEBUG("CCCD for the service changed is 0x%04x", cccd);

            ble_gap_sec_keyset_t * p_keys = NULL;
            ble_gap_sec_params_t * p_sec_params = NULL;

            err_code = sd_ble_gap_sec_params_reply(m_conn_handle,
                                                   BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP,
                                                   p_sec_params,
                                                   p_keys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE:
        {
            NRF_LOG_DEBUG("Received: BLE_GAP_EVT_CONN_PARAM_UPDATE");
            NRF_LOG_DEBUG("conn_sup_timeout: %d\r\nmax_conn_interval: %d\r\nmin_conn_interval: %d\r\nslave_latency %d",
            p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params.conn_sup_timeout,
            p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params.max_conn_interval,
            p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params.min_conn_interval,
            p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params.slave_latency);
        } break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("Received: BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST");
            err_code = sd_ble_gap_conn_param_update(m_conn_handle, &p_ble_evt->evt.gap_evt.params.conn_param_update_request.conn_params);
            if(err_code != NRF_SUCCESS)
            {
                NRF_LOG_ERROR("Failure to update connection parameter request: 0x%x", err_code);
            }

            APP_ERROR_CHECK(err_code);
        } break;

#if defined(S132)
        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;
#endif

        case BLE_GATTS_EVT_TIMEOUT:
        {
            if (p_ble_evt->evt.gatts_evt.params.timeout.src == BLE_GATT_TIMEOUT_SRC_PROTOCOL)
            {
                err_code = sd_ble_gap_disconnect(m_conn_handle,
                                                 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
                APP_ERROR_CHECK(err_code);
            }
        } break;

        case BLE_EVT_USER_MEM_REQUEST:
        {
            err_code = sd_ble_user_mem_reply(m_conn_handle, NULL);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            if (p_ble_evt->evt.gatts_evt.params.authorize_request.type
                != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
            {
                if (on_rw_authorize_req(&m_dfu, p_ble_evt))
                {
                    err_code = on_ctrl_pt_write(&m_dfu,
                           &(p_ble_evt->evt.gatts_evt.params.authorize_request.request.write));
#ifdef NRF_DFU_DEBUG_VERSION
                    if (err_code != NRF_SUCCESS)
                    {
                        NRF_LOG_ERROR("Could not handle on_ctrl_pt_write. err_code: 0x%04x", err_code);
                    }
#else
                    // Swallow result
                    (void) err_code;
#endif
                }
            }
        } break;

        case BLE_GAP_EVT_SEC_INFO_REQUEST:
        {
            NRF_LOG_DEBUG("== conn sec update request");
            ble_gap_enc_info_t * p_enc_info = NULL;
            ble_gap_irk_t * p_id_info = NULL;

#if defined(NRF_DFU_BLE_REQUIRES_BONDS) && (NRF_DFU_BLE_REQUIRES_BONDS == 1)
            // If there is a match in diversifier, then set the correct keys.
            if (p_ble_evt->evt.gap_evt.params.sec_info_request.master_id.ediv ==
                m_peer_data.enc_key.master_id.ediv)
            {
                p_enc_info = &m_peer_data.enc_key.enc_info;
            }
            p_id_info = &m_peer_data.ble_id.id_info;
#endif
            err_code = sd_ble_gap_sec_info_reply(p_ble_evt->evt.gap_evt.conn_handle,
                                                 p_enc_info,
                                                 p_id_info,
                                                 NULL);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_CONN_SEC_UPDATE:
        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
        {
#if defined(NRF_DFU_BLE_REQUIRES_BONDS) && (NRF_DFU_BLE_REQUIRES_BONDS == 1)
            err_code = service_changed_send();
#else
            err_code = sd_ble_gatts_sys_attr_set(p_ble_evt->evt.gap_evt.conn_handle, NULL, 0, 0);
#endif
            APP_ERROR_CHECK(err_code);
            NRF_LOG_DEBUG("== We are finished handling conn sec update");
        } break;

        case BLE_GATTS_EVT_WRITE:
        {
            on_write(&m_dfu, p_ble_evt);
        } break;

#if (NRF_SD_BLE_API_VERSION >= 3)
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
        {
            err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                       BLE_GATT_ATT_MTU_DEFAULT);
            APP_ERROR_CHECK(err_code);
        } break; // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST

        case BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST:
        {
            ble_gap_data_length_params_t const dlp =
            {
                .max_rx_octets = BLE_GAP_DATA_LENGTH_AUTO,
                .max_tx_octets = BLE_GAP_DATA_LENGTH_AUTO,
            };

            err_code = sd_ble_gap_data_length_update(p_ble_evt->evt.gatts_evt.conn_handle,
                                                     &dlp, NULL);
            APP_ERROR_CHECK(err_code);
        } break; // BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST
#endif

        case BLE_GATTS_EVT_HVC:
            on_hvc(&m_dfu, p_ble_evt);
            break;

        case BLE_GATTS_EVT_SC_CONFIRM:
            NRF_LOG_DEBUG("Service Changed was handled");
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief       Function for the LEDs initialization.
 *
 * @details     Initializes all LEDs used by this application.
 */
static void leds_init(void)
{
    nrf_gpio_cfg_output(ADVERTISING_LED_PIN_NO);
    nrf_gpio_cfg_output(CONNECTED_LED_PIN_NO);
    nrf_gpio_pin_set(ADVERTISING_LED_PIN_NO);
    nrf_gpio_pin_set(CONNECTED_LED_PIN_NO);
}


#if !defined(NRF_DFU_BLE_REQUIRES_BONDS) || (NRF_DFU_BLE_REQUIRES_BONDS == 0)
static uint32_t gap_address_change(void)
{
    uint32_t       err_code;
    ble_gap_addr_t addr;

#if (NRF_SD_BLE_API_VERSION < 3)
    err_code = sd_ble_gap_address_get(&addr);
#else
    err_code = sd_ble_gap_addr_get(&addr);
#endif

    VERIFY_SUCCESS(err_code);

    // Increase the BLE address by one when advertising openly.
    addr.addr[0] += 1;

#if (NRF_SD_BLE_API_VERSION < 3)
    err_code = sd_ble_gap_address_set(BLE_GAP_ADDR_CYCLE_MODE_NONE, &addr);
#else
    err_code = sd_ble_gap_addr_set(&addr);
#endif

    VERIFY_SUCCESS(err_code);

    return NRF_SUCCESS;
}
#endif


/**@brief     Function for GAP initialization.
 *
 * @details   This function will set up all necessary GAP (Generic Access Profile) parameters of
 *            the device. It also sets the permissions and appearance.
 */
static uint32_t gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params = {0};
    ble_gap_conn_sec_mode_t sec_mode;
    uint8_t const *         device_name;
    uint32_t                name_len;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

#if !defined(NRF_DFU_BLE_REQUIRES_BONDS) || (NRF_DFU_BLE_REQUIRES_BONDS == 0)

    err_code = gap_address_change();
    VERIFY_SUCCESS(err_code);

    if ((m_flags & DFU_BLE_FLAG_USE_ADV_NAME) != 0)
    {
        NRF_LOG_DEBUG("Setting adv name: %s, length: %d", (uint32_t)m_adv_name.name, m_adv_name.len);
        device_name = m_adv_name.name;
        name_len = m_adv_name.len;
    }
    else
#endif
    {
        NRF_LOG_DEBUG("Regular adv name");
        device_name = (uint8_t const *)DEVICE_NAME;
        name_len = strlen(DEVICE_NAME);
    }

    err_code = sd_ble_gap_device_name_set(&sec_mode, device_name, name_len);
    VERIFY_SUCCESS(err_code);

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    return err_code;
}


static uint32_t ble_stack_init(bool init_softdevice)
{
    uint32_t  err_code;
    uint32_t  ram_start = 0;
    ble_cfg_t ble_cfg   = {{0}};

    #if defined(NRF_DFU_BLE_REQUIRES_BONDS) && (NRF_DFU_BLE_REQUIRES_BONDS == 1)
    ble_cfg_t           ble_gatts_cfg_service_changed   = {{0}};
    #endif

    if (init_softdevice)
    {
        err_code = nrf_dfu_mbr_init_sd();
        VERIFY_SUCCESS(err_code);
    }

    NRF_LOG_DEBUG("vector table: 0x%08x", BOOTLOADER_START_ADDR);
    err_code = sd_softdevice_vector_table_base_set(BOOTLOADER_START_ADDR);
    VERIFY_SUCCESS(err_code);
    NRF_LOG_DEBUG("vector table: 0x%08x", BOOTLOADER_START_ADDR);

    NRF_LOG_DEBUG("Error code - sd_softdevice_vector_table_base_set: 0x%08x", err_code);

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Fetch the start address of the application RAM.
    err_code = nrf_sdh_ble_app_ram_start_get(&ram_start);
    VERIFY_SUCCESS(err_code);

    // Configure the maximum number of connections.
    memset(&ble_cfg, 0, sizeof(ble_cfg));
    ble_cfg.gap_cfg.role_count_cfg.periph_role_count  = 1;
    ble_cfg.gap_cfg.role_count_cfg.central_role_count = 0;
    ble_cfg.gap_cfg.role_count_cfg.central_sec_count  = 0;
    err_code = sd_ble_cfg_set(BLE_GAP_CFG_ROLE_COUNT, &ble_cfg, ram_start);

    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("Error code - sd_ble_cfg_set: 0x%08x", err_code);
        return err_code;
    }

#if defined(NRF_DFU_BLE_REQUIRES_BONDS) && (NRF_DFU_BLE_REQUIRES_BONDS == 1)
    NRF_LOG_DEBUG("Running Service Changed config");

    ble_gatts_cfg_service_changed.gatts_cfg.service_changed.service_changed = 1;
    err_code = sd_ble_cfg_set(BLE_GATTS_CFG_SERVICE_CHANGED, &ble_gatts_cfg_service_changed, ram_start);
    VERIFY_SUCCESS(err_code);

    NRF_LOG_DEBUG("Finished running Service Changed config");
#endif

    NRF_LOG_DEBUG("Enabling SoftDevice.");

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("Failed softdevice_enable: 0x%08x", err_code);
    }
    else
    {
        NRF_LOG_DEBUG("SoftDevice enabled.");
    }

    NRF_SDH_BLE_OBSERVER(m_ble_evt_observer, BLE_OBSERVER_PRIO, ble_evt_handler, NULL);

    return err_code;
}


/**@brief       Function for adding DFU Packet characteristic to the BLE Stack.
 *
 * @param[in]   p_dfu DFU Service structure.
 *
 * @return      NRF_SUCCESS on success. Otherwise an error code.
 */
static uint32_t dfu_pkt_char_add(ble_dfu_t * const p_dfu)
{
    ble_gatts_char_md_t char_md         = {{0}};
    ble_gatts_attr_t    attr_char_value = {0};
    ble_gatts_attr_md_t attr_md         = {{0}};
    ble_uuid_t          char_uuid;

    char_md.char_props.write_wo_resp = 1;

    char_uuid.type = p_dfu->uuid_type;
    char_uuid.uuid = BLE_DFU_PKT_CHAR_UUID;

    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.read_perm);

#if defined(NRF_DFU_BLE_REQUIRES_BONDS) && (NRF_DFU_BLE_REQUIRES_BONDS == 1)
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&attr_md.write_perm);
#else
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
#endif

    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.vlen    = 1;

    attr_char_value.p_uuid    = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.max_len   = MAX_DFU_PKT_LEN;
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(p_dfu->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_dfu->dfu_pkt_handles);
}


/**@brief       Function for adding DFU Control Point characteristic to the BLE Stack.
 *
 * @param[in]   p_dfu DFU Service structure.
 *
 * @return      NRF_SUCCESS on success. Otherwise an error code.
 */
static uint32_t dfu_ctrl_pt_add(ble_dfu_t * const p_dfu)
{
    ble_gatts_char_md_t char_md         = {{0}};
    ble_gatts_attr_t    attr_char_value = {0};
    ble_gatts_attr_md_t attr_md         = {{0}};
    ble_uuid_t          char_uuid;

    char_md.char_props.write  = 1;
    char_md.char_props.notify = 1;

    char_uuid.type = p_dfu->uuid_type;
    char_uuid.uuid = BLE_DFU_CTRL_PT_UUID;

    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.read_perm);

#if defined(NRF_DFU_BLE_REQUIRES_BONDS) && (NRF_DFU_BLE_REQUIRES_BONDS == 1)
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&attr_md.write_perm);
#else
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
#endif

    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.wr_auth = 1;
    attr_md.vlen    = 1;

    attr_char_value.p_uuid    = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.max_len   = BLE_GATT_ATT_MTU_DEFAULT;
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(p_dfu->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_dfu->dfu_ctrl_pt_handles);
}


/**@brief     Function for checking if the CCCD of DFU Control point is configured for Notification.
 *
 * @details   This function checks if the CCCD of DFU Control Point characteristic is configured
 *            for Notification by the DFU Controller.
 *
 * @param[in] p_dfu DFU Service structure.
 *
 * @return    True if the CCCD of DFU Control Point characteristic is configured for Notification.
 *            False otherwise.
 */
uint32_t ble_dfu_init(ble_dfu_t * p_dfu)
{
    ble_uuid_t service_uuid;
    uint32_t   err_code;

    ASSERT(p_dfu != NULL);

    m_conn_handle = BLE_CONN_HANDLE_INVALID;

    BLE_UUID_BLE_ASSIGN(service_uuid, BLE_DFU_SERVICE_UUID);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &service_uuid,
                                        &(p_dfu->service_handle));
    VERIFY_SUCCESS(err_code);

    ble_uuid128_t const base_uuid128 =
    {
        {
            0x50, 0xEA, 0xDA, 0x30, 0x88, 0x83, 0xB8, 0x9F,
            0x60, 0x4F, 0x15, 0xF3,  0x00, 0x00, 0xC9, 0x8E
        }
    };

    err_code = sd_ble_uuid_vs_add(&base_uuid128, &p_dfu->uuid_type);
    VERIFY_SUCCESS(err_code);

    err_code = dfu_pkt_char_add(p_dfu);
    VERIFY_SUCCESS(err_code);

    err_code = dfu_ctrl_pt_add(p_dfu);
    VERIFY_SUCCESS(err_code);

    m_flags |= DFU_BLE_FLAG_SERVICE_INITIALIZED;

    return NRF_SUCCESS;
}


uint32_t ble_dfu_transport_init(void)
{
    uint32_t err_code;

    NRF_LOG_DEBUG("Initializing BLE DFU transport");

    leds_init();

#if defined(NRF_DFU_BLE_REQUIRES_BONDS) && (NRF_DFU_BLE_REQUIRES_BONDS == 1)
    // Copy out the peer data if bonds are required
    if (nrf_dfu_settings_peer_data_is_valid())
    {
        NRF_LOG_DEBUG("Copying peer data");
        (void)nrf_dfu_settings_peer_data_copy(&m_peer_data);

    }
    else
    {
        APP_ERROR_HANDLER(NRF_ERROR_INTERNAL);
    }
#endif

    err_code = ble_stack_init(true);
    VERIFY_SUCCESS(err_code);

#if !defined(NRF_DFU_BLE_REQUIRES_BONDS) || (NRF_DFU_BLE_REQUIRES_BONDS == 0)
    // Copy out the new advertisement name when bonds are not required and name is set.
    if (nrf_dfu_settings_adv_name_is_valid())
    {
        (void)nrf_dfu_settings_adv_name_copy(&m_adv_name);

        // Set flags for advertisement name that is to be used
        m_flags |= DFU_BLE_FLAG_USE_ADV_NAME;

        NRF_LOG_DEBUG("nrf_dfu_settings_adv_name_is_valid TRUE");
    }
    else
    {
        NRF_LOG_DEBUG("nrf_dfu_settings_adv_name_is_valid FALSE");
    }
#endif

    err_code = gap_params_init();
    VERIFY_SUCCESS(err_code);

    // Initialize the Device Firmware Update Service.
    err_code = ble_dfu_init(&m_dfu);
    VERIFY_SUCCESS(err_code);

    err_code = advertising_start();
    VERIFY_SUCCESS(err_code);

    NRF_LOG_DEBUG("Finished initializing BLE DFU transport");

    return NRF_SUCCESS;
}


uint32_t ble_dfu_transport_close(void)
{
    uint32_t err_code = NRF_SUCCESS;

    NRF_LOG_DEBUG("Shutting down BLE transport.");

    if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        NRF_LOG_DEBUG("Disconnecting.");

        // Set flag to prevent advertisement from starting
        m_flags |= DFU_BLE_RESETTING_SOON;

        // Disconnect from peer.
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        VERIFY_SUCCESS(err_code);

        // Wait a bit for the disconnect event to be sent on air.
        nrf_delay_ms(200);
    }
    else
    {
        advertising_stop();
    }

    NRF_LOG_DEBUG("BLE transport shut down.");

    return err_code;
}
