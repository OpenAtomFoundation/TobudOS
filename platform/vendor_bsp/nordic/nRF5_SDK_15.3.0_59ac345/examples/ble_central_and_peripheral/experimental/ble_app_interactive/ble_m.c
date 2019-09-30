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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "ble_m.h"
#include "cli_m.h"
#include "bsp_btn_ble.h"
#include "ble_bas.h"
#include "ble.h"
#include "app_util.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_conn_state.h"
#include "nrf_log.h"
#include "ble_advertising.h"
#include "ble_hci.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_balloc.h"
#include "nfc_central_m.h"
#include "nfc_ble_oob_advdata_parser.h"
#include "nrf_ble_scan.h"

#define APP_BLE_CONN_CFG_TAG           1 /**< Tag that identifies the SoftDevice BLE configuration. */

#define CENTRAL_SCANNING_LED           BSP_BOARD_LED_0
#define CENTRAL_CONNECTED_LED          BSP_BOARD_LED_1
#define PERIPHERAL_ADVERTISING_LED     BSP_BOARD_LED_2
#define PERIPHERAL_CONNECTED_LED       BSP_BOARD_LED_3


#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(5000)                  /**< Time from initiating an event (connect or start of notification) to the first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(30000)                 /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT   3                                      /**< Max count of connection parameter update. */

#define APP_ADV_DURATION               18000                                  /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define APP_FEATURE_NOT_SUPPORTED      BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2   /**< Reply when unsupported features are requested. */
#define CCCD_DESCRIPTOR_UUID           BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG /**< UUID of characteristic CCCD descriptors. */
#define L2CAP_HDR_LEN                  4                                      /**< Length of a L2CAP header, in bytes. */
#define BATTERY_INITIAL_LVL            100                                    /**< Battery initial level. */
#define UUID_STRING_LEN                5                                      /**< UUID uint16_t string length. */

/**@brief   Priority of the application BLE event handler.
 * @note    There is no need to modify this value.
 */
#define APP_BLE_OBSERVER_PRIO     1

BLE_BAS_DEF(m_bas);
NRF_BLE_GATT_DEF(m_gatt);           /**< GATT module instance. */
BLE_ADVERTISING_DEF(m_advertising); /**< Advertising module instance. */
NRF_BLE_SCAN_DEF(m_scan);           /**< Scanning Module instance. */

// Structure storing data of all discovered services.
typedef struct
{
    ble_gattc_service_t services[MAX_SERVICE_COUNT]; /**< Data of the services found. */
    uint8_t             count;                       /**< Count of the services found. */
} device_srv_t;


typedef struct
{
    ble_uuid_t uuid;              /**< UUID of the characteristic. */
    uint16_t   decl_handle;       /**< Handle of the characteristic declaration. */
    uint16_t   value_handle;      /**< Handle of the characteristic value. */
    uint16_t   cccd_desc_handle;  /**< Handle of the CCCD descriptors. */
    bool       notify;            /**< True when notification of the value permitted. */
} char_data_t;

// Structure storing the data of all discovered characteristics.
typedef struct
{
    char_data_t char_data[MAX_CHARACTERISTIC_COUNT]; /**< Characteristics data. */
    uint8_t     count;                               /**< Characteristics count. */
} srv_char_t;

scanned_device_t   m_device[DEVICE_TO_FIND_MAX];                 /**< Stores device info from scan data. */
char               m_addr_str_for_connection[ADDR_STRING_LEN];   /**< Stores device address as string for establishing a connection. */
srv_char_t         m_srv_char;                                   /**< Stores all the characteristics data from one service to allow further operations. */
uint16_t           m_desc_handle;                                /**< The CCCD descriptor handle found. */
device_srv_t     * mp_device_srv[NRF_BLE_LINK_COUNT];            /**< Pointers to the allocated memory needed to discover the services on the server. */
static bool        m_numeric_match_requested = false;            /**< Numeric match request. */
static uint16_t    m_num_comp_conn_handle;                       /**< Numeric comparison connection handle. */
static conn_peer_t m_connected_peers[NRF_BLE_LINK_COUNT];        /**< Connected devices data. */
bool               m_scanning = false;                           /**< Variable that informs about the ongoing scanning. True when scan is ON. */
bool               m_vendor_uuid_read = false;                   /**< Variable that informs about the read request for a 128-bit service UUID. */
uint8_t            m_uuid_attr_handle;

NRF_BALLOC_DEF(m_srv_pool, sizeof(device_srv_t), NRF_BLE_LINK_COUNT);

/**@brief UUIDs that the central application scans for if the name above is set to an empty string,
 * and that are advertised by the peripherals.
 */
ble_uuid_t m_adv_uuids[] = { { BLE_UUID_BATTERY_SERVICE, BLE_UUID_TYPE_BLE },
                             { BLE_UUID_RUNNING_SPEED_AND_CADENCE, BLE_UUID_TYPE_BLE } };

static char * mp_roles_str[] =
{
    "INVALID_ROLE",
    "CENTRAL",
    "PERIPHERAL",
};


/**@brief Connection parameters requested for connection.
 */
static ble_gap_conn_params_t const m_connection_param =
{
    MIN_CONNECTION_INTERVAL,
    MAX_CONNECTION_INTERVAL,
    SLAVE_LATENCY,
    SUPERVISION_TIMEOUT
};


/**@brief Function for handling asserts in the SoftDevice.
 *
 * @details This function is called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and is not meant for the final product. You need to analyze
 *          how your product is supposed to react in case of an assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num     Line number of the failing assert call.
 * @param[in] p_file_name  File name of the failing assert call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    ASSERT(p_file_name);

    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}


/**@brief Function for handling errors from the Connection Parameters module.
 *
 * @param[in] nrf_error  Error code that contains information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


bool is_scanning(void)
{
    return m_scanning;
}


void addr_store_value_set(char const * const p_address_str)
{
    ASSERT(p_address_str);

    strcpy(m_addr_str_for_connection, p_address_str);
}


scanned_device_t * scan_device_info_get(void)
{
    return m_device;
}


void scan_device_info_clear(void)
{
    memset(m_device, 0, sizeof(m_device));
}


bool is_numeric_match_requested(void)
{
    return m_numeric_match_requested;
}


void numeric_match_request_clear(void)
{
    m_numeric_match_requested = false;
}


uint16_t numeric_match_request_conn_handle_get(void)
{
    return m_num_comp_conn_handle;
}


void preferred_phy_set(ble_gap_phys_t const * const p_phy, uint16_t conn_handle)
{
    ASSERT(p_phy);

    ret_code_t err_code;

    err_code = sd_ble_gap_phy_update(conn_handle, p_phy);
    APP_ERROR_CHECK(err_code);
}


uint16_t cccd_descriptors_handle_get(char const * const p_char_uuid_str)
{
    ASSERT(p_char_uuid_str);

    char uuid_str[UUID_STRING_LEN] = {0};

    // Searching for characteristic.
    for (uint8_t i = 0; i < m_srv_char.count; i++)
    {

        sprintf(uuid_str, "%X", m_srv_char.char_data[i].uuid.uuid);

        if ((!strcmp(uuid_str, p_char_uuid_str)) && m_srv_char.char_data[i].notify)
        {
           return m_srv_char.char_data[i].cccd_desc_handle;
        }
    }

    return BLE_GATT_HANDLE_INVALID;
}


/**@brief Function for searching for the CCCD of a characteristic.
 *
 * @param[in] char_uuid       UUID of the characteristic.
 * @param[in] conn_handle     The connection handle that identifies the connection to perform this procedure on.
 */

static void cccd_descriptors_search(uint16_t char_uuid, uint16_t conn_handle)
{
    uint8_t                  i;
    ret_code_t               err_code;
    ble_gattc_handle_range_t handle_range;
    uint16_t                 start_handle = 0;
    uint16_t                 end_handle   = 0;

    // Searching for the handle of the initial characteristic.
    for (i = 0; i < m_srv_char.count; i++)
    {
        if (m_srv_char.char_data[i].uuid.uuid == char_uuid)
        {
            start_handle = m_srv_char.char_data[i].value_handle + 1;
            break;
        }
    }

    // Searching for the handle of the final characteristic if the characteristic is not the last one in the service.
    if ((i < (m_srv_char.count - 1)) && (m_srv_char.char_data[i + 1].uuid.uuid != 0))
    {
        // If the characteristic is not the last one, start a handle equal to the declaration handle of the next characteristic's declaration handle.
        end_handle = m_srv_char.char_data[i + 1].decl_handle - 1;
    }
    // Searching for the handle of the final characteristic. If the characteristic is the last one in the service, the end handle is equal to the service end handle.
    else
    {
        for (uint8_t j = 0; j < mp_device_srv[conn_handle]->count; j++)
        {
            if ((start_handle >
                 mp_device_srv[conn_handle]->services[j].handle_range.start_handle) &&
                (start_handle < mp_device_srv[conn_handle]->services[j].handle_range.end_handle))
            {
                end_handle = mp_device_srv[conn_handle]->services[j].handle_range.end_handle;
                break;
            }
        }
    }

    handle_range.start_handle = start_handle;
    handle_range.end_handle   = end_handle;

    // Discovery descriptors inside, the found handle range.
    err_code = sd_ble_gattc_descriptors_discover(conn_handle, &handle_range);
    APP_ERROR_CHECK(err_code);
}


uint16_t handle_value_search(char const * const p_service_uuid_str)
{
    ASSERT(p_service_uuid_str);

    char buffer[UUID_STRING_LEN] = {0};

    for (uint8_t i = 0; i < m_srv_char.count; i++)
    {
        sprintf(buffer, "%X", m_srv_char.char_data[i].uuid.uuid);

        if (!strcmp(buffer, p_service_uuid_str))
        {
            return m_srv_char.char_data[i].value_handle;
        }
    }

    return BLE_GATT_HANDLE_INVALID;
}


ble_gattc_handle_range_t * handle_range_search(char const * const p_service_uuid_str,
                                               uint16_t           conn_handle)
{
    ASSERT(p_service_uuid_str);

    char buffer[UUID_STRING_LEN] = {0};

    for (uint8_t i = 0; i < mp_device_srv[conn_handle]->count; i++)
    {

        sprintf(buffer, "%X", mp_device_srv[conn_handle]->services[i].uuid.uuid);
        if (!strcmp(buffer, p_service_uuid_str))
        {
            return &mp_device_srv[conn_handle]->services[i].handle_range;
        }
    }

    return NULL;
}


void link_layer_data_length_set(char * p_data_length, uint16_t conn_handle)
{
    ASSERT(p_data_length);

    ret_code_t err_code;
    uint8_t    value;
    uint16_t   mtu;

    // Get actual MTU.
    mtu = nrf_ble_gatt_eff_mtu_get(&m_gatt, conn_handle);

    value = atoi(p_data_length);

    // Check that new data length has a correct value.
    if ((value > (mtu + L2CAP_HDR_LEN)) || (value < (BLE_GATT_ATT_MTU_DEFAULT + L2CAP_HDR_LEN)))
    {
        NRF_LOG_RAW_INFO("Data Length value must be less than %d and bigger than %d\r\n", 
                         (mtu + L2CAP_HDR_LEN), 
                         (BLE_GATT_ATT_MTU_DEFAULT + L2CAP_HDR_LEN));
        return;
    }

    // Set data length.
    err_code = nrf_ble_gatt_data_length_set(&m_gatt, conn_handle, value);
    APP_ERROR_CHECK(err_code);
}


void gatt_mtu_set(char const * const p_mtu_value)
{
    ASSERT(p_mtu_value);

    ret_code_t err_code;
    uint16_t   mtu_val;

    mtu_val = atoi(p_mtu_value);

    // Checking the new MTU value.
    if ((mtu_val < BLE_GATT_ATT_MTU_DEFAULT) ||
        (mtu_val > NRF_SDH_BLE_GATT_MAX_MTU_SIZE))
    {
        NRF_LOG_RAW_INFO("%s %d - %d \r\n",
                         "MTU value must be between",
                         BLE_GATT_ATT_MTU_DEFAULT,
                         NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
        return;
    }

    // New maxiumum MTU must be set before a connection is established.
    err_code = nrf_ble_gatt_att_mtu_central_set(&m_gatt, mtu_val);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, mtu_val);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_RAW_INFO("MTU will be changed for next connection. Processing MTU change.\r\n");
}


char const * phy_str(ble_gap_phys_t const * const p_phys)
{
    ASSERT(p_phys);

    static char const * p_str[] =
    {
        "1 Mbps",
        "2 Mbps",
        "Coded",
        "Unknown"
    };

    switch (p_phys->tx_phys)
    {
        case BLE_GAP_PHY_1MBPS:
            return p_str[0];

        case BLE_GAP_PHY_2MBPS:
        case (BLE_GAP_PHY_2MBPS | BLE_GAP_PHY_1MBPS):
        case (BLE_GAP_PHY_2MBPS | BLE_GAP_PHY_1MBPS | BLE_GAP_PHY_CODED):
            return p_str[1];

        case BLE_GAP_PHY_CODED:
            return p_str[2];

        default:
            return p_str[3];
    }
}


void int_addr_to_hex_str(char * p_result, uint8_t result_len, uint8_t const * const p_addr)
{
    ASSERT(p_result);
    ASSERT(p_addr);

    if (result_len > BLE_GAP_ADDR_LEN)
    {
        return;
    }

    char buffer[BLE_GAP_ADDR_LEN] = {0};

    memset(p_result, 0, result_len);

    for (uint8_t i = 0; i < result_len; ++i)
    {
        sprintf(buffer, "%.2X", p_addr[result_len - (i + 1)]);
        strcat(p_result, buffer);

        if (i < (result_len - 1))
        {
            strcat(p_result, ":");
        }
    }
}


/**@brief Function for printing the UUID for each service.
 *
 * @param[in] conn_handle    The connection handle identifying the connection to perform this procedure on.
 * @param[in] service_p      Pointer to ble_gattc_service_t.
 */
static void uuid_print(uint16_t conn_handle, ble_gattc_service_t const * p_service)
{
    NRF_LOG_RAW_INFO("Found service UUIDs: \r\n");

    for (uint8_t i = 0; i < mp_device_srv[conn_handle]->count; i++)
    {
        NRF_LOG_RAW_INFO("%s: %X %s: 0x%X\r\n", 
                         "UUID", 
                         p_service[i].uuid.uuid,
                         "type", 
                         p_service[i].uuid.type);
    }
}


bool is_address_compare(ble_gap_addr_t const * const p_connected_addr, char const * const p_addr)
{
    ASSERT(p_connected_addr);
    ASSERT(p_addr);

    char string_addr_buf[ADDR_STRING_LEN] = {0};

    int_addr_to_hex_str(string_addr_buf,
                        BLE_GAP_ADDR_LEN,
                        p_connected_addr->addr);

    return (memcmp(string_addr_buf, p_addr, sizeof(string_addr_buf))) ? false : true;
}


uint16_t addr_string_to_conn_handle(char const * const p_addr)
{
    ASSERT(p_addr);

    uint16_t conn_handle = 0;
    uint8_t  idx;

    for (idx = 0; idx < NRF_BLE_LINK_COUNT; idx++)
    {
        if (is_address_compare(&m_connected_peers[idx].address, p_addr))
        {
            conn_handle = idx;
            return conn_handle;
        }
    }

    return BLE_CONN_HANDLE_INVALID;
}


/**@brief Function for searching for a device name and adding it to a dynamic command.
 *
 * @details Use this function to parse the received advertising data and to find a given
 * name in them either as 'complete_local_name' or as 'short_local_name'.
 *
 * @param[in]   p_adv_report   Advertising data to parse.
 */
static void device_to_list_add(ble_gap_evt_adv_report_t const * p_adv_report)
{
    uint8_t  idx             = 0;
    uint16_t dev_name_offset = 0;
    uint16_t field_len;
    data_t   adv_data;


    // Initialize advertisement report for parsing
    adv_data.p_data   = (uint8_t *)p_adv_report->data.p_data;
    adv_data.data_len = p_adv_report->data.len;

    for ( idx = 0; idx < DEVICE_TO_FIND_MAX; idx++)
    {
        // If address is duplicated, then return.
        if (memcmp(p_adv_report->peer_addr.addr,
                   m_device[idx].addr,
                   sizeof(p_adv_report->peer_addr.addr)) == 0)
        {
            return;
        }
    }

    // Add device data if an empty record is found.
    for (idx = 0; idx < DEVICE_TO_FIND_MAX; idx++)
    {
        if (!m_device[idx].is_not_empty)
        {
            memcpy(m_device[idx].addr,
                   p_adv_report->peer_addr.addr,
                   sizeof(p_adv_report->peer_addr.addr));

            m_device[idx].is_not_empty = true;

            // Search for advertising names.
            field_len = ble_advdata_search(adv_data.p_data,
                                           adv_data.data_len,
                                           &dev_name_offset,
                                           BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME);

            if (field_len == 0)
            {
                field_len = ble_advdata_search(adv_data.p_data,
                                               adv_data.data_len,
                                               &dev_name_offset,
                                               BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME);
                // If name is not found, then return.
                if (field_len == 0)
                {
                    return;
                }
            }
            memcpy(m_device[idx].dev_name, &adv_data.p_data[dev_name_offset], field_len);
            m_device[idx].dev_name[field_len] = 0;

            return;
        }
    }
}


/**@brief Function for displaying an address in HEX format.
 */
static void hex_addr_display(uint8_t const * p_addr, size_t size)
{
    NRF_LOG_RAW_INFO("Connected to address: ");

    for (uint8_t i = 0; i < size; ++i)
    {
        NRF_LOG_RAW_INFO("%.2X ", p_addr[size - (i + 1)]);
    }

    NRF_LOG_RAW_INFO("\r\n");
}


/**@brief Function for parsing data from NFC central and connecting if data matches.
 *
 * @param[in]   p_gap_evt        Pointer to GAP data event.
 * @param[in]   p_peer_addr      Pointer to adv_report address.
 */
static void nfc_central_connect(ble_gap_evt_t const        * p_gap_evt,
                                ble_gap_addr_t const * const p_peer_addr)
{
    ret_code_t err_code;
    uint8_t  * p_adv_data;
    uint16_t   data_len;
    uint16_t   dev_name_offset = 0;
    uint16_t   field_len;
    char       dev_name[32];

    // Initialize advertisement report for parsing.
    p_adv_data = (uint8_t *)p_gap_evt->params.adv_report.data.p_data;
    data_len   = p_gap_evt->params.adv_report.data.len;

    // Search for advertising names.
    field_len = ble_advdata_search(p_adv_data,
                                   data_len,
                                   &dev_name_offset,
                                   BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME);

    if (field_len == 0)
    {
        // Look for the short local name if it was not found as complete.
        field_len = ble_advdata_search(p_adv_data,
                                       data_len,
                                       &dev_name_offset,
                                       BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME);

        if (field_len == 0)
        {
            // If data cannot be parsed, then exit.
            return;
        }
    }

    memcpy(dev_name, &p_adv_data[dev_name_offset], field_len);
    dev_name[field_len] = 0;

    NRF_LOG_DEBUG("Found advertising device: %s", nrf_log_push( (char *)dev_name));

    // Check if device address is the same as the address taken from the NFC tag.
    if (nfc_oob_pairing_tag_match(p_peer_addr))
    {
        // If the address is correct, initiate a connection with the peripheral device.
        err_code = sd_ble_gap_connect(p_peer_addr, &m_scan.scan_params,
                                      &m_connection_param,
                                      APP_BLE_CONN_CFG_TAG);
        APP_ERROR_CHECK(err_code);
    }
}


void private_connect(pm_peer_id_t const * p_peers_id)
{
    ASSERT(p_peers_id);

    ret_code_t     err_code;
    ble_gap_addr_t public_addr;

    memset(&public_addr, 0, sizeof(public_addr));
    pm_peer_data_bonding_t data;
    memset(&data, 0, sizeof(data));

    // Load device bonding data.
    err_code = pm_peer_data_bonding_load(*p_peers_id, &data);

    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("Try again with correct peer ID.");
        return;
    }

    memcpy(&public_addr,
           &data.peer_ble_id.id_addr_info,
           sizeof(public_addr));

     // Stop scanning.
    scan_stop();

    // Set device identities list.
    err_code = pm_device_identities_list_set(p_peers_id, 1);
    APP_ERROR_CHECK(err_code);

    scan_start();

    // Connect with device using privacy by public address.
    err_code = sd_ble_gap_connect(&public_addr, &m_scan.scan_params,
                                  &m_connection_param,
                                  APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling primary service discovery response.
 *
 * @details This function will handle the primary service discovery response.
 *
 * @param[in] p_ble_gattc_evt   Pointer to the GATT Client event.
 */
static void on_primary_srv_discovery_rsp(ble_gattc_evt_t const * p_ble_gattc_evt)
{
    uint16_t        count;
    uint16_t        bytes_to_copy;
    uint16_t        conn_handle;
    static uint16_t offset = 0;
    ret_code_t      err_code;

    // For readability.
    ble_gattc_evt_prim_srvc_disc_rsp_t const * p_prim_serv =
        &(p_ble_gattc_evt->params.prim_srvc_disc_rsp);
    conn_handle = p_ble_gattc_evt->conn_handle;
    ble_gattc_service_t * const p_service =
        mp_device_srv[conn_handle]->services;

    // Number of discovered services.
    count = p_prim_serv->count;

    // If no more services are found.
    if ((count != 0) && 
        (p_ble_gattc_evt->gatt_status == BLE_GATT_STATUS_SUCCESS))
    {
        if ((count + offset) > MAX_SERVICE_COUNT)
        {
            bytes_to_copy = MAX_SERVICE_COUNT - offset;
        }
        else
        {
            bytes_to_copy = count;
        }

        // Save services data.
        memcpy((p_service + offset),
               p_prim_serv->services,
               bytes_to_copy * sizeof(ble_gattc_service_t));

        offset += count;

        // If the last service has not been reached, this function must be called again with a new start handle.
        err_code = sd_ble_gattc_primary_services_discover(
            conn_handle,
            p_prim_serv->services[count - 1].handle_range.end_handle + 1,
            NULL);
        APP_ERROR_CHECK(err_code);
    }
    else
    {
        mp_device_srv[conn_handle]->count = offset;

        // If service UUID type is unknown, then look for a 128-bit UUID.
        // Only the first one is searched for here, the rest is searched for in the @ref on_read_rsp.
        for (uint8_t i = 0; i < offset; i++)
        {
            if (p_service[i].uuid.type == BLE_UUID_TYPE_UNKNOWN)
            {
                m_vendor_uuid_read = true;
                // Read service 128-bit UUID.
                err_code = sd_ble_gattc_read(conn_handle, p_service[i].handle_range.start_handle, 0);
                APP_ERROR_CHECK(err_code);
                offset = 0;

                return;
            }
        }

        NRF_LOG_INFO("Services count: %d", offset);
        uuid_print(p_ble_gattc_evt->conn_handle, p_service);

        offset = 0;
    }
}


/**@brief Function for starting a discovery of CCCD descriptors.
 *
 * @details If characteristics can be notified, then look for CCCD descriptors in all 
 *          characteristics inside the service.
 *
 * @param[in] p_ble_gattc_evt 
 */
static void cccd_descriptors_discovery(ble_gattc_evt_t const * p_ble_gattc_evt)
{
    for (uint8_t i = 0; i < m_srv_char.count; i++)
    {
        // If it is possible to enable notification.
        if (m_srv_char.char_data[i].notify)
        {
            // Search for CCCD descriptor handle
            cccd_descriptors_search(m_srv_char.char_data[i].uuid.uuid, p_ble_gattc_evt->conn_handle);
        }
    }
}


/**@brief Function for handling a characteristic discovery response.
 *
 * @param[in] p_ble_gattc_evt   Pointer to the GATT Client event.
 */
static void on_characteristics_discovery_rsp(ble_gattc_evt_t const * p_ble_gattc_evt)
{
    uint16_t        count;
    static uint16_t offset = 0;;
    uint16_t        bytes_to_copy;
    ret_code_t      err_code;
    uint16_t        conn_handle = p_ble_gattc_evt->conn_handle;

    // For readability.
    count = p_ble_gattc_evt->params.char_disc_rsp.count;
    ble_gattc_evt_char_disc_rsp_t const * p_char_disc_rsp_evt;

    p_char_disc_rsp_evt = &(p_ble_gattc_evt->params.char_disc_rsp);

    if (p_ble_gattc_evt->gatt_status == BLE_GATT_STATUS_SUCCESS)
    {
        if ((count + offset) > MAX_CHARACTERISTIC_COUNT)
        {
            bytes_to_copy = MAX_CHARACTERISTIC_COUNT - offset;
            NRF_LOG_RAW_INFO("Too many characteristics discovered\r\n");
        }
        else
        {
            bytes_to_copy = count;
        }

        // Save characteristics data.
       for (uint8_t i = offset; i < bytes_to_copy; i++)
       {
           m_srv_char.char_data[i].decl_handle      = p_char_disc_rsp_evt->chars[i].handle_decl;
           m_srv_char.char_data[i].value_handle     = p_char_disc_rsp_evt->chars[i].handle_value;
           m_srv_char.char_data[i].uuid             = p_char_disc_rsp_evt->chars[i].uuid;
           m_srv_char.char_data[i].notify           = p_char_disc_rsp_evt->chars[i].char_props.notify;
           m_srv_char.char_data[i].cccd_desc_handle = 0;

           offset++;
       }

       // Display characteristics data.
       for (uint8_t i = 0; i < offset; i++)
       {
           ble_gatt_char_props_t char_param =
                p_ble_gattc_evt->params.char_disc_rsp.chars[i].char_props;

           NRF_LOG_RAW_INFO("Characteristic UUID: %X\r\n",
                            p_ble_gattc_evt->params.char_disc_rsp.chars[i].uuid.uuid);
           NRF_LOG_RAW_INFO("Parameters:\r\n");
           NRF_LOG_RAW_INFO("broadcast: %d ", char_param.broadcast);
           NRF_LOG_RAW_INFO("read: %d ", char_param.read);
           NRF_LOG_RAW_INFO("write_wo_resp: %d ", char_param.write_wo_resp);
           NRF_LOG_RAW_INFO("write: %d ", char_param.write);
           NRF_LOG_RAW_INFO("notify: %d\r\n", char_param.notify);
           NRF_LOG_RAW_INFO("indicate: %d ", char_param.indicate);
           NRF_LOG_RAW_INFO("auth_signed_wr: %d\r\n", char_param.auth_signed_wr);
       }
    }
    // If the last characteristic has not been reached, look for a new handle range.
    ble_gattc_handle_range_t handle_range;

    handle_range.start_handle = m_srv_char.char_data[offset - 1].value_handle + 1;

    // Search for end handle.
    for (uint8_t j = 0; j < mp_device_srv[conn_handle]->count; j++)
    {
        if (handle_range.start_handle >
             mp_device_srv[conn_handle]->services[j].handle_range.start_handle)
        {
            handle_range.end_handle =
                mp_device_srv[conn_handle]->services[j].handle_range.end_handle;
            break;
        }
    }

    // Handle value of the characteristic being discovered is less than the end handle of
    // the service being discovered. There is no possibility of more characteristics being
    // present.
    if ((m_srv_char.char_data[offset - 1].value_handle >= handle_range.end_handle) ||
        (offset == MAX_CHARACTERISTIC_COUNT) ||
        (p_ble_gattc_evt->gatt_status != BLE_GATT_STATUS_SUCCESS))
    {
        NRF_LOG_RAW_INFO("Number of characteristics: %d\r\n", offset);
        m_srv_char.count = offset;
        offset           = 0;

        // Search for the CCCD descriptors.
        cccd_descriptors_discovery(p_ble_gattc_evt);

        return;
    }

    // If the last Characteristic has not been reached, this function must be called again with new handle range.
    err_code = sd_ble_gattc_characteristics_discover(p_ble_gattc_evt->conn_handle, &handle_range);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling descriptor discovery response.
 *
 * @param[in] p_ble_gattc_evt   Pointer to the GATT Client event.
 */
static void on_descriptor_discovery_rsp(const ble_gattc_evt_t * const p_ble_gattc_evt)
{
    uint16_t   count;
    uint16_t   conn_handle = p_ble_gattc_evt->conn_handle;
    ret_code_t err_code;

    count = p_ble_gattc_evt->params.desc_disc_rsp.count;

    // If no more descriptors are found.
    if (count == 0)
    {
        NRF_LOG_RAW_INFO("No descriptors for characteristic, ",
                         "notifications and indications not possible\r\n");
        return;
    }

    if (p_ble_gattc_evt->gatt_status != BLE_GATT_STATUS_SUCCESS)
    {
        return;
    }

    // If the descriptor was found, connect it to the characteristic.
    for (uint8_t i = 0; i < count; i++)
    {
        if (p_ble_gattc_evt->params.desc_disc_rsp.descs[i].uuid.uuid == CCCD_DESCRIPTOR_UUID)
        {
            for (uint8_t j = 0; j < m_srv_char.count; j++)
            {
                if (m_srv_char.char_data[j].cccd_desc_handle == 0 && m_srv_char.char_data[j].notify)
                {
                    m_srv_char.char_data[j].cccd_desc_handle =
                        p_ble_gattc_evt->params.desc_disc_rsp.descs[i].handle;
                }
            }
            return;
        }
    }

    // If the last descriptors have not been found, look for a new handle range to search for.
    ble_gattc_handle_range_t handle_range;
    memset(&handle_range, 0, sizeof(handle_range));

    handle_range.start_handle = p_ble_gattc_evt->params.desc_disc_rsp.descs[count - 1].handle + 1;

    for (uint8_t i = 0; i < m_srv_char.count; i++)
    {
        // Search for end handle, to descriptors searching.
        if ((handle_range.start_handle >= m_srv_char.char_data[i].value_handle) &&
            (i < (m_srv_char.count - 1)))
        {
            // If current characteristic is not last in service
            handle_range.end_handle = m_srv_char.char_data[i + 1].decl_handle - 1;
            break;
        }
        // If the characteristic is last into service, search for service end handle.
        if (i == (m_srv_char.count - 1))
        {
            for (uint8_t j = 0; j < mp_device_srv[conn_handle]->count; j++)
            {
                if ((handle_range.start_handle >
                     mp_device_srv[conn_handle]->services[j].handle_range.start_handle) &&
                    (handle_range.start_handle <
                     mp_device_srv[conn_handle]->services[j].handle_range.end_handle))
                {
                    handle_range.end_handle =
                        mp_device_srv[conn_handle]->services[j].handle_range.end_handle;
                    break;
                }
            }
        }
    }

    // If the last descriptors have not been reached, this function must be called again with a new handle range.
    err_code = sd_ble_gattc_descriptors_discover(conn_handle, &handle_range);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling a read response.
 *
 * @param[in] p_ble_gattc_evt   Pointer to the GATT Client event.
 */
static void on_read_rsp(const ble_gattc_evt_t * const p_ble_gattc_evt)
{
    ret_code_t                       err_code;
    uint16_t                         conn_handle = p_ble_gattc_evt->conn_handle;
    ble_uuid128_t                    uuid;
    const ble_gattc_evt_read_rsp_t * p_read_rsp = &(p_ble_gattc_evt->params.read_rsp);

    if (m_vendor_uuid_read)
    {
        for (uint8_t i = 0; i < mp_device_srv[conn_handle]->count; i++)
        {
            if (p_read_rsp->handle ==
                mp_device_srv[conn_handle]->services[i].handle_range.start_handle)
            {
                //lint -save -e420
                memcpy(uuid.uuid128, p_read_rsp->data, sizeof(uuid.uuid128));
                //lint -restore
                err_code = sd_ble_uuid_vs_add(&uuid, &mp_device_srv[conn_handle]->services[i].uuid.type);
                APP_ERROR_CHECK(err_code);
                break;
            }
        }

        // If service UUID type is unknown, then try to search for the 128-bit UUID.
        for (uint8_t i = 0; i < mp_device_srv[conn_handle]->count; i++)
        {
            if (mp_device_srv[conn_handle]->services[i].uuid.type == BLE_UUID_TYPE_UNKNOWN)
            {
                m_vendor_uuid_read = true;
                // Look for service 128-bit UUID.
                err_code = sd_ble_gattc_read(conn_handle, mp_device_srv[conn_handle]->services[i].handle_range.start_handle, 0);
                APP_ERROR_CHECK(err_code);
                return;
            }

            // If service is last.
            if (i == (mp_device_srv[conn_handle]->count - 1))
            {
                NRF_LOG_INFO("Services count: %d", mp_device_srv[conn_handle]->count);
                m_vendor_uuid_read = false;
                // Print services UUID. When you first discover services, the 128-bit UUIDs may not be displayed.
                // In such case, you should rediscover the services.
                uuid_print(p_ble_gattc_evt->conn_handle, mp_device_srv[conn_handle]->services);
            }
        }

        return;
    }

    NRF_LOG_RAW_INFO("Read data:\r\n");

    for (uint8_t i = 0; i < p_read_rsp->len; i++)
    {
        NRF_LOG_RAW_INFO("0x%X ", p_read_rsp->data[i]);
    }

    NRF_LOG_RAW_INFO("\r\n");

    // The application only supports reading data of length less than MTU which was set for this connection.
}


/**@brief Function for handling a write response.
 *
 * @param[in] p_ble_gattc_evt   Pointer to the GATT Client event.
 */
static void on_write_rsp(const ble_gattc_evt_t * const p_ble_gattc_evt)
{
    // For readability.
    const ble_gattc_evt_write_rsp_t * p_write_rsp = &(p_ble_gattc_evt->params.write_rsp);

    // Display gattc write status.
    NRF_LOG_RAW_INFO("Type of write operation: 0x%x\r\n", p_write_rsp->write_op);
    NRF_LOG_RAW_INFO("Data was written to the server \r\n");

    for (uint8_t i = 0; i < p_write_rsp->len; i++)
    {
        NRF_LOG_RAW_INFO("%d ", p_write_rsp->data[i]);
    }

    if (p_write_rsp->len > 0)
    {
        NRF_LOG_RAW_INFO("\r\n");
    }

}


static void on_ble_evt(uint16_t conn_handle, ble_evt_t const * p_ble_evt)
{
    ret_code_t err_code;
    char       passkey[BLE_GAP_PASSKEY_LEN + 1];
    uint16_t   role = ble_conn_state_role(conn_handle);

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
        {
            // Store device data to dynamic command buffer and for futher operations.
            m_connected_peers[conn_handle].address =
                p_ble_evt->evt.gap_evt.params.connected.peer_addr;
            connected_to_cmd_add(m_connected_peers, conn_handle);
            // Display device address.
            hex_addr_display(p_ble_evt->evt.gap_evt.params.connected.peer_addr.addr,
                             BLE_GAP_ADDR_LEN);
            // Allocation memory for services data.
            mp_device_srv[conn_handle] = (device_srv_t *)nrf_balloc_alloc(&m_srv_pool);
        }
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            // Clearing device data.
            connected_to_cmd_remove(m_connected_peers, conn_handle);
            memset(&m_connected_peers[conn_handle], 0x00, sizeof(m_connected_peers[0]));
            // Release of allocated memory.
            nrf_balloc_free(&m_srv_pool, mp_device_srv[conn_handle]);
            break;

        case BLE_GAP_EVT_PASSKEY_DISPLAY:
            memcpy(passkey,
                   p_ble_evt->evt.gap_evt.params.passkey_display.passkey,
                   BLE_GAP_PASSKEY_LEN);
            passkey[BLE_GAP_PASSKEY_LEN] = 0x00;
            NRF_LOG_INFO("%s: BLE_GAP_EVT_PASSKEY_DISPLAY: passkey=%s match_req=%d",
                         nrf_log_push(mp_roles_str[role]),
                         nrf_log_push(passkey),
                         p_ble_evt->evt.gap_evt.params.passkey_display.match_request);

            if (p_ble_evt->evt.gap_evt.params.passkey_display.match_request)
            {
                NRF_LOG_INFO("Type /numeric accept/ to confirm, /numeric reject/ to reject");
                m_num_comp_conn_handle    = conn_handle;
                m_numeric_match_requested = true;
            }

            break;

        case BLE_GAP_EVT_AUTH_KEY_REQUEST:
            key_request_set();
            NRF_LOG_DEBUG("%s: BLE_GAP_EVT_AUTH_KEY_REQUEST", nrf_log_push(mp_roles_str[role]));
            break;

        case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
            NRF_LOG_DEBUG("%s: BLE_GAP_EVT_LESC_DHKEY_REQUEST", nrf_log_push(mp_roles_str[role]));
            break;

        case BLE_GAP_EVT_AUTH_STATUS:
            NRF_LOG_INFO(
                "%s: BLE_GAP_EVT_AUTH_STATUS: status=0x%x bond=0x%x lv4: %d",
                nrf_log_push(mp_roles_str[role]),
                p_ble_evt->evt.gap_evt.params.auth_status.auth_status,
                p_ble_evt->evt.gap_evt.params.auth_status.bonded,
                p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv4);
            NRF_LOG_DEBUG("kdist_own:0x%x kdist_peer:0x%x",
                          *( (uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_own),
                          *( (uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer));
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            ble_gap_evt_t const * p_gap_evt = &p_ble_evt->evt.gap_evt;
            NRF_LOG_RAW_INFO("PHY update request\r\n");
            ble_gap_phys_t phy_param =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_gap_evt->conn_handle, &phy_param);
            APP_ERROR_CHECK(err_code);
        }
        break;

        case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:
        {
            on_primary_srv_discovery_rsp(&(p_ble_evt->evt.gattc_evt));
        }
        break;

        case BLE_GATTC_EVT_CHAR_DISC_RSP:
        {
            on_characteristics_discovery_rsp(&(p_ble_evt->evt.gattc_evt));
        }
        break;

        case BLE_GATTC_EVT_DESC_DISC_RSP:
        {
            on_descriptor_discovery_rsp(&(p_ble_evt->evt.gattc_evt));
        }
        break;

        case BLE_GATTC_EVT_READ_RSP:
        {
            on_read_rsp(&(p_ble_evt->evt.gattc_evt));
        }
        break;

        case BLE_GATTC_EVT_WRITE_RSP:
        {
            on_write_rsp(&(p_ble_evt->evt.gattc_evt));
        }
        break;

        case BLE_GATTC_EVT_HVX:
        {
            // If server sends an indication, then send a Handle Value Confirmation to the GATT Server.
            if (p_ble_evt->evt.gattc_evt.params.hvx.type == BLE_GATT_HVX_INDICATION)
            {
                err_code = sd_ble_gattc_hv_confirm(p_ble_evt->evt.gattc_evt.conn_handle,
                                                   p_ble_evt->evt.gattc_evt.params.hvx.handle);
                APP_ERROR_CHECK(err_code);
            }

            uint8_t data_len = p_ble_evt->evt.gattc_evt.params.hvx.len;
            NRF_LOG_RAW_INFO(
                "%s data: ",
                (p_ble_evt->evt.gattc_evt.params.hvx.type !=
                 BLE_GATT_HVX_NOTIFICATION) ? "Indication" : "Notification");

            // Display notifications or indication data.
            for (uint8_t i = 0; i < data_len; i++)
            {
                NRF_LOG_RAW_INFO("%d ", p_ble_evt->evt.gattc_evt.params.hvx.data[i]);
            }

            NRF_LOG_RAW_INFO("\r\n");
        }
        break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for handling BLE Stack events that concern the central application.
 *
 * @details This function keeps the connection handles of the central application up-to-date. It
 * parses scanning reports, initiating a connection attempt to peripherals,
 * and manages connection parameter update requests. Additionally, it updates the status
 * of LEDs used to report the central application's activity.
 *
 * @note        Since this function updates the connection handles, @ref BLE_GAP_EVT_DISCONNECTED events
 *              should be dispatched to the target application before invoking this function.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_ble_central_evt(ble_evt_t const * p_ble_evt)
{
    ret_code_t err_code;
    // For readability
    ble_gap_evt_t const        * p_gap_evt   = &p_ble_evt->evt.gap_evt;
    ble_gap_addr_t const * const p_peer_addr = &p_gap_evt->params.adv_report.peer_addr;

    switch (p_ble_evt->header.evt_id)
    {
        // Upon connection, update LEDs status.
        case BLE_GAP_EVT_CONNECTED:
        {
            NRF_LOG_INFO("CENTRAL: Connected, handle: %d.", p_gap_evt->conn_handle);
            memset(m_addr_str_for_connection, 0, sizeof(m_addr_str_for_connection));

            // Update LEDs status.
            bsp_board_led_off(CENTRAL_SCANNING_LED);
            bsp_board_led_on(CENTRAL_CONNECTED_LED);
        }
        break; // BLE_GAP_EVT_CONNECTED

        // Upon disconnection, reset the connection handle of the peer which disconnected, update
        // the LEDs status.
        case BLE_GAP_EVT_DISCONNECTED:
        {
            NRF_LOG_INFO("CENTRAL: Disconnected, handle: %d, reason: 0x%x",
                         p_gap_evt->conn_handle,
                         p_gap_evt->params.disconnected.reason);

            // Update LEDs status.
            bsp_board_led_off(CENTRAL_CONNECTED_LED);
        }
        break; // BLE_GAP_EVT_DISCONNECTED

        case BLE_GAP_EVT_ADV_REPORT:
        {
            if (is_address_compare(&p_gap_evt->params.adv_report.peer_addr,
                                   m_addr_str_for_connection))
            {
                memset(m_addr_str_for_connection, 0, sizeof(m_addr_str_for_connection));

                // Initiate connection.
                NRF_LOG_INFO("CENTRAL: Connecting...");
                err_code = sd_ble_gap_connect(&p_gap_evt->params.adv_report.peer_addr,
                                              &m_scan.scan_params, 
                                              &m_connection_param,
                                              APP_BLE_CONN_CFG_TAG);

                if (err_code != NRF_SUCCESS)
                {
                    NRF_LOG_DEBUG("Connection Request Failed, reason %d", err_code);
                    return;
                }
            }

            // If pairing request in NFC central role, then compare data and connect.
            nfc_central_connect(p_gap_evt, p_peer_addr);

            // Add device address and name (if exists) from the scan report to dynamic CLI command.
            device_to_list_add(&p_gap_evt->params.adv_report);
            address_to_cmd_add(&p_gap_evt->params.adv_report.peer_addr);
        }
        break; // BLE_GAP_ADV_REPORT

        case BLE_GAP_EVT_TIMEOUT:
        {
            // Time-out for scanning has not been specified, so only connection attemps can time out.
            if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN)
            {
                NRF_LOG_DEBUG("CENTRAL: Connection Request timed out.");
            }
        }
        break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE:
        {
            NRF_LOG_RAW_INFO("Connection parameters update success\r\n");
        }
        break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
        {
            // Accept parameters requested by the peer.
            err_code = sd_ble_gap_conn_param_update(
                p_gap_evt->conn_handle,
                &p_gap_evt->params.conn_param_update_request.
                conn_params);
            APP_ERROR_CHECK(err_code);
        }
        break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client time-out event.
            NRF_LOG_DEBUG("CENTRAL: GATT Client Time-out.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server time-out event.
            NRF_LOG_DEBUG("CENTRAL: GATT Server Time-out.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTC_EVT_EXCHANGE_MTU_RSP:
            NRF_LOG_RAW_INFO("Current MTU: %d\r\n",
                             p_ble_evt->evt.gattc_evt.params.exchange_mtu_rsp.server_rx_mtu);

            break;

        case BLE_GAP_EVT_PHY_UPDATE:
        {
            ble_gap_evt_phy_update_t const * p_phy_evt = &p_ble_evt->evt.gap_evt.params.phy_update;

            if (p_phy_evt->status == BLE_HCI_STATUS_CODE_LMP_ERROR_TRANSACTION_COLLISION)
            {
                // Ignore LL collisions.
                NRF_LOG_DEBUG("LL transaction collision during PHY update.");
                break;
            }

            ble_gap_phys_t phys = { 0 };
            phys.tx_phys = p_phy_evt->tx_phy;
            phys.rx_phys = p_phy_evt->rx_phy;

            NRF_LOG_INFO("PHY update %s. PHY set to %s.",
                         (p_phy_evt->status == BLE_HCI_STATUS_CODE_SUCCESS) ?
                         "accepted" : "rejected",
                         phy_str(&phys));
        }
        break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for handling BLE Stack events involving peripheral applications. Manages the
 * LEDs used to report the status of the peripheral applications.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void on_ble_peripheral_evt(ble_evt_t const * p_ble_evt)
{
    ret_code_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("PERIPHERAL: Connected, handle %d.", p_ble_evt->evt.gap_evt.conn_handle);
            bsp_board_led_off(PERIPHERAL_ADVERTISING_LED);
            bsp_board_led_on(PERIPHERAL_CONNECTED_LED);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("PERIPHERAL: Disconnected, handle %d, reason 0x%x.",
                         p_ble_evt->evt.gap_evt.conn_handle,
                         p_ble_evt->evt.gap_evt.params.disconnected.reason);
            bsp_board_led_off(PERIPHERAL_CONNECTED_LED);
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client time-out event.
            NRF_LOG_DEBUG("PERIPHERAL: GATT Client Time-out.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server time-out event.
            NRF_LOG_DEBUG("PERIPHERAL: GATT Server Time-out.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_EVT_USER_MEM_REQUEST:
            err_code = sd_ble_user_mem_reply(p_ble_evt->evt.gap_evt.conn_handle, NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t  req;
            ble_gatts_rw_authorize_reply_params_t auth_reply;

            req = p_ble_evt->evt.gatts_evt.params.authorize_request;

            if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
            {
                if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL))
                {
                    if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
                    }
                    else
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
                    }

                    auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
                    err_code                            = sd_ble_gatts_rw_authorize_reply(
                        p_ble_evt->evt.gatts_evt.conn_handle,
                        &auth_reply);
                    APP_ERROR_CHECK(err_code);
                }
            }
        }
        break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

        case BLE_GAP_EVT_PHY_UPDATE:
        {
            ble_gap_evt_phy_update_t const * p_phy_evt = &p_ble_evt->evt.gap_evt.params.phy_update;

            if (p_phy_evt->status == BLE_HCI_STATUS_CODE_LMP_ERROR_TRANSACTION_COLLISION)
            {
                // Ignore LL collisions.
                NRF_LOG_INFO("LL transaction collision during PHY update.");
                break;
            }

            ble_gap_phys_t phys = {0};
            phys.tx_phys = p_phy_evt->tx_phy;
            phys.rx_phys = p_phy_evt->rx_phy;

            NRF_LOG_INFO("PHY update %s. PHY set to %s.",
                         (p_phy_evt->status == BLE_HCI_STATUS_CODE_SUCCESS) ?
                         "accepted" : "rejected",
                         phy_str(&phys));
        }
        break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for handling advertising events.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static inline void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            bsp_board_led_on(PERIPHERAL_ADVERTISING_LED);
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    uint16_t conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    uint16_t role        = ble_conn_state_role(conn_handle);

    on_ble_evt(conn_handle, p_ble_evt);

    if (role == BLE_GAP_ROLE_PERIPH)
    {
        // Manages peripheral LEDs.
        on_ble_peripheral_evt(p_ble_evt);
    }
    else if ((role == BLE_GAP_ROLE_CENTRAL) ||
             (p_ble_evt->header.evt_id == BLE_GAP_EVT_ADV_REPORT))
    {
        on_ble_central_evt(p_ble_evt);
    }
    else
    {
    }
}


/**@brief Function for initiating scanning.
 */
void scan_start(void)
{
    ret_code_t err_code;

    //Clear the current device address.
    memset(m_addr_str_for_connection, 0, sizeof(m_addr_str_for_connection));

    connect_addr_clear();

    err_code = nrf_ble_scan_start(&m_scan);
    APP_ERROR_CHECK(err_code);

    bsp_board_led_on(CENTRAL_SCANNING_LED);

    NRF_LOG_INFO("Scanning");
    m_scanning = true;
}


/**@brief Function for stopping scanning.
 */
void scan_stop(void)
{
    nrf_ble_scan_stop();
    bsp_board_led_off(CENTRAL_SCANNING_LED);
    m_scanning = false;
}


/**@brief Function for initiating advertising.
 */
void adv_start(void)
{
    ret_code_t err_code;

    // Turn on the LED to signal advertising.
    bsp_board_led_on(PERIPHERAL_ADVERTISING_LED);

    // Start advertising.
    err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);

    if (err_code == NRF_ERROR_CONN_COUNT)
    {
        NRF_LOG_INFO("Maximum connection count exceeded.\r\n");
        return;
    }

    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("Advertising");
}


/**@brief Function for disabling advertising and scanning.
 */
void adv_stop(void)
{
    ret_code_t err_code;

    // Turn off the LED to signal advertising.
    bsp_board_led_off(PERIPHERAL_ADVERTISING_LED);
    err_code = sd_ble_gap_adv_stop(m_advertising.adv_handle);

    if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_INVALID_STATE) && (err_code != BLE_ERROR_INVALID_ADV_HANDLE))
    {
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupts.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
static void bsp_event_handler(bsp_event_t event)
{
    ret_code_t err_code;

    switch (event)
    {
        case BSP_EVENT_KEY_0:
            err_code = ble_bas_battery_level_update(&m_bas, 87, BLE_CONN_HANDLE_ALL);

            if ((err_code != NRF_SUCCESS) &&
                (err_code != NRF_ERROR_INVALID_STATE) &&
                (err_code != NRF_ERROR_RESOURCES) &&
                (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
               )
            {
                APP_ERROR_HANDLER(err_code);
            }

            break;

        default:
            break;
    }
}


/**@brief Function for initializing buttons and LEDs.
 */
static inline void buttons_leds_init(void)
{
    ret_code_t err_code;

    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_event_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONNECTION_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONNECTION_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = SUPERVISION_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling BLE stack GATT events.
 */
static void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
    switch (p_evt->evt_id)
    {
        case NRF_BLE_GATT_EVT_ATT_MTU_UPDATED:

            NRF_LOG_RAW_INFO("MTU changed successfully\r\n");

            break;

        case NRF_BLE_GATT_EVT_DATA_LENGTH_UPDATED:
            NRF_LOG_INFO("Data length updated to %u bytes.", p_evt->params.data_length);
            break;

        default:
            break;
    }
}


/**@brief Function for initializing the GATT module.
 */
static inline void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);

    APP_ERROR_CHECK(err_code);
}


/**@brief   Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    ret_code_t err_code;
    uint16_t   conn_handle = p_evt->conn_handle;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_CONN_HANDLE_INVALID; // Start upon connection.
    cp_init.disconnect_on_fail             = true;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


void advertising_init(void)
{
    ret_code_t             err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = m_adv_uuids;

    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  = APP_ADV_DURATION;

    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}


/**@brief Function for initialization Scanning Module.
 */
static void scan_init(void)
{
    ret_code_t err_code;

    err_code = nrf_ble_scan_init(&m_scan, NULL, NULL);
    APP_ERROR_CHECK(err_code);
}


/**
 * @brief Function for initializing a block allocator.
 */
static inline void balloc_init(void)
{
    ret_code_t err_code;

    err_code = nrf_balloc_init(&m_srv_pool);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Battery service. */
static void bas_init(void)
{
    ret_code_t     err_code;
    ble_bas_init_t bas_init_struct;
    uint8_t        initial_batt_lvl;

    // Initialize the Battery Service.
    initial_batt_lvl = BATTERY_INITIAL_LVL;

    memset(&bas_init_struct, 0, sizeof(bas_init_struct));

    bas_init_struct.evt_handler          = NULL;
    bas_init_struct.support_notification = true;
    bas_init_struct.p_report_ref         = NULL;
    bas_init_struct.initial_batt_level   = initial_batt_lvl;

    // Require LESC with MITM (Numeric Comparison)
    bas_init_struct.bl_cccd_wr_sec   = SEC_MITM;
    bas_init_struct.bl_report_rd_sec = SEC_MITM;

    err_code = ble_bas_init(&m_bas, &bas_init_struct);
    APP_ERROR_CHECK(err_code);
}


void ble_m_init(void)
{
    // Initialization of required BLE components.
    balloc_init();
    buttons_leds_init();
    ble_stack_init();
    scan_init();
    gap_params_init();
    gatt_init();
    bas_init();
    conn_params_init();
    advertising_init();
}


