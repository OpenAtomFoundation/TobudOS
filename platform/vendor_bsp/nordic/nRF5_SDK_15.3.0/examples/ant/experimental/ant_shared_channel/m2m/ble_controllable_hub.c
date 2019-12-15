/**
 * This software is subject to the ANT+ Shared Source License
 * www.thisisant.com/swlicenses
 * Copyright (c) Garmin Canada Inc. 2014
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 *    1) Redistributions of source code must retain the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer.
 *
 *    2) Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 *    3) Neither the name of Garmin nor the names of its
 *       contributors may be used to endorse or promote products
 *       derived from this software without specific prior
 *       written permission.
 *
 * The following actions are prohibited:
 *
 *    1) Redistribution of source code containing the ANT+ Network
 *       Key. The ANT+ Network Key is available to ANT+ Adopters.
 *       Please refer to http://thisisant.com to become an ANT+
 *       Adopter and access the key. 
 *
 *    2) Reverse engineering, decompilation, and/or disassembly of
 *       software provided in binary form under this license.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE HEREBY
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; DAMAGE TO ANY DEVICE, LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE. SOME STATES DO NOT ALLOW 
 * THE EXCLUSION OF INCIDENTAL OR CONSEQUENTIAL DAMAGES, SO THE
 * ABOVE LIMITATIONS MAY NOT APPLY TO YOU.
 *
 */
#include "ble_controllable_hub.h"
#include <string.h>
#include <stdbool.h>
#include "nrf_error.h"
#include "ble.h"
#include "app_util.h"
#include "asc_pages.h"
#include "asc_events.h"

/**
 * @brief Verify module's initialization status. Returns NRF_ERROR_INVALID_STATE it is not.
 */
#define VERIFY_MODULE_INITIALIZED()                                                               \
        do                                                                                        \
        {                                                                                         \
            if (!m_module_initialized)                                                            \
            {                                                                                     \
                 return (NRF_ERROR_INVALID_STATE);                                                \
            }                                                                                     \
        } while (0)

/**
 * @brief Verify NULL parameters are not passed to API.
 */
#define VERIFY_NOT_NULL_PARAM(PARAM)                                                              \
        if ((PARAM) == NULL)                                                                      \
        {                                                                                         \
            return (NRF_ERROR_NULL);                                                              \
        }
/**@} */


/**@brief 128-bit UUID base List. */
static const ble_uuid128_t m_base_uuid128 =
{
   {
       0x29, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15,
       0xBC, 0xEF, 0x12, 0x24, 0x00, 0x00, 0x00, 0x00
   }
};


#define BLE_CCCD_VALUE_LEN            2                                             /**< The length of a CCCD value. */
#define COMPANY_IDENTIFIER            0x0059                                        /**< Company identifier for Nordic Semiconductor ASA as per www.bluetooth.org. */

#define SERIAL_NUMBER_ADDRESS           ((uint32_t) 0x10000060)     // FICR + 60
#define HUB_DEVICE_NUMBER               (*(uint16_t*) SERIAL_NUMBER_ADDRESS)

#define LOCAL_SERVICE_UUID            0x1525                                        /**< Proprietary UUID for local service. */
#define LOCAL_CHAR_UUID               0x1526                                        /**< Proprietary UUID for local characteristic. */
#define LOCAL_CHAR_CTRLPT_UUID        0x1527                                        /**< Proprietary UUID for local control point characteristic. */
#define APP_REPORT_CHAR_LEN           8                                             /**< Size of the characteristic value being notified (in bytes). */
#define APP_CTRLPT_CHAR_LEN           8                                             /**< Size of the control point characteristic value being notified (in bytes). */
static uint16_t                       m_service_handle;                             /**< Handle of local service (as provided by the BLE stack). */
static uint8_t                        m_char_value[APP_REPORT_CHAR_LEN];            /**< Value of the characteristic that will be sent as a notification to the central. */
static uint8_t                        m_char_ctrlpt_value[APP_CTRLPT_CHAR_LEN];     /**< Value of the control point characteristic that will be sent as a notification to the central. */
static ble_gatts_char_handles_t       m_char_handles;                               /**< Handles of the characteristic (as provided by the BLE stack). */
static ble_gatts_char_handles_t       m_char_ctrlpt_handles;                        /**< Handles of control point characteristic (as provided by the BLE stack). */
static bool                           m_module_initialized = false;                 /**< Value to indicate if module has been initialized. */
static uint32_t                       m_event_flags;                                /**< Event bitfield allowing the application to determine when a command has been received */
static asc_command_data_t             m_last_received_command;                      /**< The last command received over the Bluetooth channel. */

static uint32_t ach_report_char_add(const uint8_t uuid_type)
{
    // Report characteristic
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_md_t attr_md;

    ble_uuid_t          char_uuid;
    ble_gatts_attr_t    attr_char_value;


    // Controlpoint characteristic
    ble_gatts_char_md_t char_ctrlpt_md;
    ble_gatts_attr_md_t cccd_ctrlpt_md;
    ble_gatts_attr_md_t attr_ctrlpt_md;

    ble_uuid_t          char_ctrlpt_uuid;
    ble_gatts_attr_t    attr_char_ctrlpt_value;


    memset(&cccd_md, 0, sizeof(cccd_md));

    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;

    char_uuid.type = uuid_type;
    char_uuid.uuid = LOCAL_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);

    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));
//lint --e{545}
    memset(&m_char_value, 0, APP_REPORT_CHAR_LEN);

    attr_char_value.p_uuid    = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = APP_REPORT_CHAR_LEN;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = APP_REPORT_CHAR_LEN;
    attr_char_value.p_value   = m_char_value;

    uint32_t err_code;
    err_code = sd_ble_gatts_characteristic_add(m_service_handle,
                                               &char_md,
                                               &attr_char_value,
                                               &m_char_handles);

    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    memset(&cccd_ctrlpt_md, 0, sizeof(cccd_ctrlpt_md));

    cccd_ctrlpt_md.vloc = BLE_GATTS_VLOC_STACK;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_ctrlpt_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_ctrlpt_md.write_perm);

    memset(&char_ctrlpt_md, 0, sizeof(char_ctrlpt_md));

    char_ctrlpt_md.char_props.notify   = 1;
    char_ctrlpt_md.char_props.write    = 1;
    char_ctrlpt_md.p_char_user_desc    = NULL;
    char_ctrlpt_md.p_char_pf           = NULL;
    char_ctrlpt_md.p_user_desc_md      = NULL;
    char_ctrlpt_md.p_cccd_md           = &cccd_ctrlpt_md;
    char_ctrlpt_md.p_sccd_md           = NULL;

    char_ctrlpt_uuid.type = uuid_type;
    char_ctrlpt_uuid.uuid = LOCAL_CHAR_CTRLPT_UUID;

    memset(&attr_ctrlpt_md, 0, sizeof(attr_ctrlpt_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_ctrlpt_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_ctrlpt_md.write_perm);

    attr_ctrlpt_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_ctrlpt_md.rd_auth = 0;
    attr_ctrlpt_md.wr_auth = 1;
    attr_ctrlpt_md.vlen    = 0;

    memset(&attr_char_ctrlpt_value, 0, sizeof(attr_char_ctrlpt_value));
//lint --e{545}
    memset(&m_char_ctrlpt_value, 0, APP_CTRLPT_CHAR_LEN);

    attr_char_ctrlpt_value.p_uuid    = &char_ctrlpt_uuid;
    attr_char_ctrlpt_value.p_attr_md = &attr_ctrlpt_md;
    attr_char_ctrlpt_value.init_len  = APP_CTRLPT_CHAR_LEN;
    attr_char_ctrlpt_value.init_offs = 0;
    attr_char_ctrlpt_value.max_len   = APP_CTRLPT_CHAR_LEN;
    attr_char_ctrlpt_value.p_value   = m_char_ctrlpt_value;

    err_code = sd_ble_gatts_characteristic_add(m_service_handle,
                                               &char_ctrlpt_md,
                                               &attr_char_ctrlpt_value,
                                               &m_char_ctrlpt_handles);

    return err_code;
}

uint32_t ble_achs_init(ble_achs_t * p_achs, const ble_achs_init_t * p_achs_init)
{
    VERIFY_NOT_NULL_PARAM(p_achs);
    VERIFY_NOT_NULL_PARAM(p_achs_init);

    uint32_t   err_code;
    ble_uuid_t service_uuid;

    // Invalidate any previous existing connection handle.
    p_achs->conn_handle = BLE_CONN_HANDLE_INVALID;

    service_uuid.uuid = LOCAL_SERVICE_UUID;

    err_code = sd_ble_uuid_vs_add(&m_base_uuid128, &service_uuid.type);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service_uuid, &m_service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code = ach_report_char_add(service_uuid.type);

    if (err_code == NRF_SUCCESS)
    {
        m_module_initialized = true;
    }

    return err_code;
}


static bool is_cccd_configured(uint16_t conn_handle)
{
    uint32_t          err_code;
    uint8_t           cccd_value_buf[BLE_CCCD_VALUE_LEN];
    ble_gatts_value_t value;
    bool              is_ctrlpt_notif_enabled = false;

    value.len     = BLE_CCCD_VALUE_LEN;
    value.offset  = 0;
    value.p_value = cccd_value_buf;

    err_code = sd_ble_gatts_value_get(conn_handle,
                                      m_char_ctrlpt_handles.cccd_handle,
                                      &value);

    // TODO: Error codes should be sent back to application indicating that the
    // read of CCCD did not work. No application error handler is currently
    // implemented.
    (void)err_code;

    uint16_t cccd_value = uint16_decode(cccd_value_buf);
    is_ctrlpt_notif_enabled = ((cccd_value & BLE_GATT_HVX_NOTIFICATION) != 0);

    return is_ctrlpt_notif_enabled;
}


static void on_ctrlpt_write(ble_achs_t                  * p_achs,
                            ble_gatts_evt_write_t const * p_evt_write)
{
    ble_gatts_rw_authorize_reply_params_t auth_reply;

    auth_reply.type                 = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
    auth_reply.params.write.update  = 1;
    auth_reply.params.write.offset  = 0;
    auth_reply.params.write.len     = 0;
    auth_reply.params.write.p_data  = NULL;

    if (is_cccd_configured(p_achs->conn_handle))
    {
        auth_reply.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;
        uint32_t err_code = sd_ble_gatts_rw_authorize_reply(p_achs->conn_handle, &auth_reply);

        (void)err_code;
    }
    else
    {
        auth_reply.params.write.gatt_status = ACH_CTRLPT_NACK_CCCD_IMPROPERLY_CONFIGURED;
        uint32_t err_code = sd_ble_gatts_rw_authorize_reply(p_achs->conn_handle, &auth_reply);
        (void)err_code;
        return;
    }

    uint8_t op_code = p_evt_write->data[0];
//lint --e{415}
    uint8_t command = p_evt_write->data[1];

    if (op_code == ACH_OP_COMMAND)
    {
        switch (command)
        {
            case ACH_CMD_REPORTING_MODE_OFF:
                {
                    //Load up the last received command structure so the application
                    //gets recent data when it asks for it
                    m_last_received_command.command = REPORTING_MODE_OFF;
                    m_last_received_command.master_id = EVERY_MASTER_ID;

                    //Set the command received flag so the application will know to ask for
                    //the most recent command.
                    asc_event_set(&m_event_flags, EVENT_ASC_COMMAND_RECEIVED);
                }
                break;

            case ACH_CMD_REPORTING_MODE_ON:
                {
                    //Load up the last received command structure so the application
                    //gets recent data when it asks for it
                    m_last_received_command.command = REPORTING_MODE_ON;
                    m_last_received_command.master_id = EVERY_MASTER_ID;

                    //Set the command received flag so the application will know to ask for
                    //the most recent command.
                    asc_event_set(&m_event_flags, EVENT_ASC_COMMAND_RECEIVED);
                }
                break;

            case ACH_CMD_REPORTING_MODE_WARNINGS:
                {
                    /* this command is not currently supported by this demo
                    //Load up the last received command structure so the application
                    //gets recent data when it asks for it
                    m_last_received_command.command = REPORTING_MODE_WARNINGS;
                    m_last_received_command.master_id = EVERY_MASTER_ID;

                    //Set the command received flag so the application will know to ask for
                    //the most recent command.
                    asc_event_set(&m_event_flags, EVENT_ASC_COMMAND_RECEIVED);
                    */
                }
                break;

            case ACH_CMD_PERI_ON:
                {
                    //Load up the last received command structure so the application
                    //gets recent data when it asks for it
                    m_last_received_command.command = TURN_ON;
//lint --e{415}
//lint --e{416}
                    m_last_received_command.shared_address = p_evt_write->data[2];
//lint --e{415}
//lint --e{416}
                    m_last_received_command.master_id = uint16_decode(&p_evt_write->data[4]);
                    m_last_received_command.group_number = NO_GROUPS;/**< @todo can the demo actually use groups? Double check this everywhere*/
                    //Set the command received flag so the application will know to ask for
                    //the most recent command.
                    asc_event_set(&m_event_flags, EVENT_ASC_COMMAND_RECEIVED);
                }
                break;

            case ACH_CMD_PERI_OFF:
                {
                    //Load up the last received command structure so the application
                    //gets recent data when it asks for it
                    m_last_received_command.command = TURN_OFF;
//lint --e{416}
                    m_last_received_command.shared_address = p_evt_write->data[2];
//lint --e{416}
                    m_last_received_command.master_id = uint16_decode(&p_evt_write->data[4]);
                    m_last_received_command.group_number = NO_GROUPS;

                    //Set the command received flag so the application will know to ask for
                    //the most recent command.
                    asc_event_set(&m_event_flags, EVENT_ASC_COMMAND_RECEIVED);
                }
                break;

            case ACH_CMD_GROUP_ASSIGN:
                {
                    //Load up the last received command structure so the application
                    //gets recent data when it asks for it
                    m_last_received_command.command = ASSIGN_TO_GROUP;
//lint --e{416}
                    m_last_received_command.shared_address = p_evt_write->data[2];
//lint --e{416}
                    m_last_received_command.master_id = uint16_decode(&p_evt_write->data[4]);
                    m_last_received_command.group_number = p_evt_write->data[3];

                    //Set the command received flag so the application will know to ask for
                    //the most recent command.
                    asc_event_set(&m_event_flags, EVENT_ASC_COMMAND_RECEIVED);

                }
                break;

            case ACH_CMD_GROUP_ON:
                {
                    //Load up the last received command structure so the application
                    //gets recent data when it asks for it
                    m_last_received_command.command = TURN_ON;
                    m_last_received_command.shared_address = EVERY_DEVICE_ADDRESS;
//lint --e{416}
                    m_last_received_command.master_id = uint16_decode(&p_evt_write->data[3]);
                    m_last_received_command.group_number = p_evt_write->data[2];

                    asc_event_set(&m_event_flags, EVENT_ASC_COMMAND_RECEIVED);
                }
                break;

            case ACH_CMD_GROUP_OFF:
                {
                    //Load up the last received command structure so the application
                    //gets recent data when it asks for it
                    m_last_received_command.command = TURN_OFF;
                    m_last_received_command.shared_address = EVERY_DEVICE_ADDRESS;
//lint --e{416}
                    m_last_received_command.master_id = uint16_decode(&p_evt_write->data[3]);
                    m_last_received_command.group_number = p_evt_write->data[2];

                    asc_event_set(&m_event_flags, EVENT_ASC_COMMAND_RECEIVED);
                }
                break;

            default:
                break;

        } // end switch (command)
    }
}


static void on_rw_authorize_request(ble_achs_t * p_achs, ble_gatts_evt_t const * p_gatts_evt)
{
    ble_gatts_evt_rw_authorize_request_t const * p_auth_req = &p_gatts_evt->params.authorize_request;

    on_ctrlpt_write(p_achs, &p_auth_req->request.write);
}


void ble_achs_on_ble_evt(ble_achs_t * p_achs, ble_evt_t const * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            p_achs->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

            // If we are connected start reporting, push a fake reporting mode on command
            m_last_received_command.command = REPORTING_MODE_ON;
            m_last_received_command.master_id = EVERY_MASTER_ID;
            m_last_received_command.group_number = NO_GROUPS;
            m_last_received_command.shared_address = EVERY_DEVICE_ADDRESS;

            asc_event_set(&m_event_flags, EVENT_ASC_COMMAND_RECEIVED);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            p_achs->conn_handle = BLE_CONN_HANDLE_INVALID;
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
            on_rw_authorize_request(p_achs, &p_ble_evt->evt.gatts_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}


uint32_t ble_achs_report_send(ble_achs_t * p_achs, ble_achs_report_t * p_report)
{
    VERIFY_MODULE_INITIALIZED();

    // Send value if connected and notifying
    if (p_achs->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        uint16_t hvx_len = APP_REPORT_CHAR_LEN;

        m_char_value[0] = p_report->page_num;
#ifdef TWO_BYTE_SHARED_ADDRESS
        (void)uint16_encode(p_report->shared_address, &m_char_value[1]);
        m_char_value[3] = p_report->value;
        (void)uint16_encode(p_report->master_id, &m_char_value[4]);
#else
        m_char_value[1] = p_report->shared_address;
        m_char_value[2] = p_report->value;
        (void)uint16_encode(p_report->master_id, &m_char_value[3]);
#endif

        ble_gatts_hvx_params_t hvx_params =
        {
            .handle = m_char_handles.value_handle,
            .type   = BLE_GATT_HVX_NOTIFICATION,
            .p_len  = &hvx_len,
            .p_data = m_char_value
        };

        uint32_t err_code = sd_ble_gatts_hvx(p_achs->conn_handle, &hvx_params);
        if ((err_code == NRF_SUCCESS) && (hvx_len != APP_REPORT_CHAR_LEN))
        {
            err_code = NRF_ERROR_DATA_SIZE;
        }
//lint --e{438}
    }
    else
    {
        return NRF_ERROR_INVALID_STATE;
    }

    return NRF_SUCCESS;
}


uint32_t ble_achs_events_get(void)
{
    return m_event_flags;
}


void ble_achs_event_clear(uint32_t event)
{
    asc_event_clear(&m_event_flags, event);
}


asc_command_data_t ble_achs_get_last_command(void)
{
    return m_last_received_command;
}
