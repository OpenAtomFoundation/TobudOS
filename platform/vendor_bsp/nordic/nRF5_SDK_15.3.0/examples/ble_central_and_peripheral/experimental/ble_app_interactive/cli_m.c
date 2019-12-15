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

#include <ctype.h>
#include "cli_m.h"
#include "nrf_cli.h"
#include "nrf_ble_gatt.h"
#include "nrf_log.h"
#include "ble_m.h"
#include "ble_hci.h"
#include "ble_conn_params.h"
#include "peer_manager.h"
#include "nfc_m.h"
#include "nfc_central_m.h"
#include "adafruit_pn532.h"

#define UNKNOWN_PARAMETER     "unknown parameter: "
#define WRONG_PARAMETER_COUNT "wrong parameter count \n"
#define KEY_PASSKEY_LEN       (6U)

#define UNIT_TO_MSEC(uint, resolution) (uint16_t)(((uint) * (resolution)) / 1000)            /**< Macro for converting ticks into milliseconds. */

#define SEC_PARAM_CENTRAL_DEFAULT                           \
    {                                                       \
        .bond           = 1,                                \
        .mitm           = 0,                                \
        .lesc           = 0,                                \
        .keypress       = 0,                                \
        .io_caps        = BLE_GAP_IO_CAPS_KEYBOARD_DISPLAY, \
        .oob            = 0,                                \
        .min_key_size   = BLE_SEC_PARAM_MIN_KEY_SIZE,       \
        .max_key_size   = BLE_SEC_PARAM_MAX_KEY_SIZE,       \
        .kdist_own.enc  = 1,                                \
        .kdist_own.id   = 1,                                \
        .kdist_peer.enc = 1,                                \
        .kdist_peer.id  = 1,                                \
    }

typedef union
{
    char connect_addr_cmd_buffer[DEVICE_TO_FIND_MAX][CLI_MAX_CMD_LEN]; /**< Device address from scan buffer. */
    char connected_cmd_buffer[NRF_BLE_LINK_COUNT][CLI_MAX_CMD_LEN];    /**< Connected device address buffer. */
    char bond_cmd_buffer[BOND_DEVICE_MAX][CLI_MAX_CMD_LEN];            /**< Bonded device address buffer. */
} cli_cmd_buffer_t;

typedef struct
{
    cli_cmd_buffer_t buffer_type; /**< Dynamic command buffer type. */
    uint8_t          cmd_counter; /**< Dynamic command counter. */
} cli_cmd_connect_addr_t;

typedef enum
{
    CLI_CMD_CONNECT_ADDR,  /**< Device address from scan. */
    CLI_CMD_CONNECTED,     /**< Connected device address. */
    CLI_CMD_BONDED,        /**< Bonded device address. */
} cli_cmd_buffer_type_t;

bool                   m_key_request_event = false;         /**< Flag informing about a key request event. */
char                   m_device_name[DEVICE_NAME_MAX_SIZE]; /**< Device name. */
uint16_t               m_conn_handle;                       /**< Connection handle for pairing and key request. */
cli_cmd_connect_addr_t m_cli_connect_addr;                  /**< List of BLE address strings for all devices found during scanning using as CLI commands. */
cli_cmd_connect_addr_t m_cli_connected;                     /**< List of connected devices address string using as CLI commands. */
cli_cmd_connect_addr_t m_cli_bonded;                        /**< List of bonded devices address string using as CLI commands. */
ble_gap_sec_params_t   m_sec_params_central = SEC_PARAM_CENTRAL_DEFAULT;
ble_gap_conn_params_t  m_params_update      =
{
    .min_conn_interval = MIN_CONNECTION_INTERVAL,
    .max_conn_interval = MAX_CONNECTION_INTERVAL,
    .slave_latency     = SLAVE_LATENCY,
    .conn_sup_timeout  = SUPERVISION_TIMEOUT
};


/**@Function for verification connection handle.
 */
static inline bool conn_handle_is_valid(uint16_t conn_handle, nrf_cli_t const * p_cli)
{
    if (conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Invalid device address\n");
        return false;
    }

    return true;
}


/**@Function for verification of the error code.
 */
static inline void ret_code_verify(ret_code_t err_code)
{
    if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_INVALID_STATE))
    {
        APP_ERROR_CHECK(err_code);
    }
}


/* Function required by qsort */
static int string_cmp(void const * p_a, void const * p_b)
{
    ASSERT(p_a);
    ASSERT(p_b);
    return strcmp( (char const *)p_a, (char const *)p_b);
}


/**@brief Function for adding BLE address string as command to the dynamic command buffer.
 *
 * @param[in]   p_data           BLE address to remove.
 * @param[in]   p_cli_buffer     Dynamic command buffer.
 * @param[in]   type             Type of the dynamic command buffer.
 */
static void cli_addr_string_to_command_add(uint8_t          const * p_data,
                                           cli_cmd_connect_addr_t * p_cli_buffer,
                                           cli_cmd_buffer_type_t    type)
{
    uint8_t   idx;
    uint8_t   size;
    uint8_t * p_cmd_counter = &p_cli_buffer->cmd_counter;
    char      (*p_buffer)[CLI_MAX_CMD_LEN];

    switch (type)
    {
        case CLI_CMD_CONNECT_ADDR:
            size = DEVICE_TO_FIND_MAX;
            p_buffer = p_cli_buffer->buffer_type.connect_addr_cmd_buffer;
        break;

        case CLI_CMD_CONNECTED:
            size = NRF_BLE_LINK_COUNT;
            p_buffer = p_cli_buffer->buffer_type.connected_cmd_buffer;
        break;

        case CLI_CMD_BONDED:
            size = BOND_DEVICE_MAX;
            p_buffer = p_cli_buffer->buffer_type.bond_cmd_buffer;
        break;

        default:
            return;

    }

    char string_buf[ADDR_STRING_LEN];

    memset(string_buf, 0, ADDR_STRING_LEN);

    // Address conversion to string.
    int_addr_to_hex_str(string_buf, BLE_GAP_ADDR_LEN, p_data);

    // If no place in buffer then return.
    if (*p_cmd_counter >= size)
    {
        return;
    }

    // Check if the address is repeated.
    for (idx = 0; idx < size; idx++)
    {
        if (!strcmp(p_buffer[idx], string_buf))
        {
            return;
        }
    }

    //Add address to buffer.
    sprintf(p_buffer[(*p_cmd_counter)++], "%s", string_buf);
    qsort(p_buffer,
          *p_cmd_counter,
          sizeof(p_buffer[0]),
          string_cmp);
}


/**@brief Function for removing a BLE address string from the dynamic command buffer.
 *
 * @param[in]   p_data           BLE address to remove.
 * @param[in]   p_cli_buffer     Dynamic command buffer.
 * @param[in]   type             Type of the dynamic command buffer.
 */
static void cli_addr_from_command_remove(char             * const p_data,
                                         cli_cmd_connect_addr_t * p_cli_buffer,
                                         cli_cmd_buffer_type_t    type)
{
    uint8_t   size;
    uint8_t * p_cmd_counter = &p_cli_buffer->cmd_counter;
    char      (*p_buffer)[CLI_MAX_CMD_LEN];

    switch (type)
    {
        case CLI_CMD_CONNECT_ADDR:
            size = DEVICE_TO_FIND_MAX;
            p_buffer = p_cli_buffer->buffer_type.connect_addr_cmd_buffer;
        break;

        case CLI_CMD_CONNECTED:
            size = NRF_BLE_LINK_COUNT;
            p_buffer = p_cli_buffer->buffer_type.connected_cmd_buffer;
        break;

        case CLI_CMD_BONDED:
            size = BOND_DEVICE_MAX;
            p_buffer = p_cli_buffer->buffer_type.bond_cmd_buffer;
        break;

        default:
            return;

    }
    for (uint8_t idx = 0; idx < *p_cmd_counter; idx++)
    {
        if (!strcmp(p_buffer[idx], p_data))
        {
            if (idx == size - 1)
            {
                p_buffer[idx][0] = '\0';
            }
            else
            {
                memmove(p_buffer[idx],
                        p_buffer[idx + 1],
                        sizeof(p_buffer[idx]) * (*p_cmd_counter - idx));
            }

            --(*p_cmd_counter);

            return;
        }
    }
}


void connected_to_cmd_add(conn_peer_t * p_connected_peers, uint16_t conn_handle)
{
    ASSERT(p_connected_peers);

    cli_addr_string_to_command_add(p_connected_peers[conn_handle].address.addr,
                                   &m_cli_connected,
                                   CLI_CMD_CONNECTED);
}


void connected_to_cmd_remove(conn_peer_t * p_connected_peers, uint16_t conn_handle)
{
    ASSERT(p_connected_peers);

    char string_addr_buf[ADDR_STRING_LEN];

    // Convert address to format XX:XX:XX:XX:XX:XX.
    int_addr_to_hex_str(string_addr_buf,
                        BLE_GAP_ADDR_LEN,
                        p_connected_peers[conn_handle].address.addr);

    // Remove address from command buffer.
    cli_addr_from_command_remove(string_addr_buf,
                                 &m_cli_connected,
                                 CLI_CMD_CONNECTED);
}


void address_to_cmd_add(ble_gap_addr_t const * p_connected_addr)
{
    ASSERT(p_connected_addr);
    // Add address to command buffer.
    cli_addr_string_to_command_add(p_connected_addr->addr,
                                   &m_cli_connect_addr,
                                   CLI_CMD_CONNECT_ADDR);
}


void connect_addr_clear(void)
{
    /* Erasing the dynamically created command (address, name) and commands counters */
    memset(&m_cli_connect_addr, 0, sizeof(m_cli_connect_addr));
}


void bond_get(void)
{
    ret_code_t   err_code;
    pm_peer_id_t current_peer_id = pm_next_peer_id_get(PM_PEER_ID_INVALID);

    memset(&m_cli_bonded, 0, sizeof(m_cli_bonded));

    // Reading all peer`s bonding data.
    while (current_peer_id != PM_PEER_ID_INVALID)
    {
        pm_peer_data_bonding_t p_data;

        err_code = pm_peer_data_bonding_load(current_peer_id, &p_data);

        if (err_code == NRF_SUCCESS)
        {
            // Add bonded device address to CLI commands.
            cli_addr_string_to_command_add(p_data.peer_ble_id.id_addr_info.addr,
                                           &m_cli_bonded,
                                           CLI_CMD_BONDED);
        }

        current_peer_id = pm_next_peer_id_get(current_peer_id);
    }
}


void bonds_delete(void)
{
    ret_code_t err_code;

    // Delete all data stored for all peers.
    err_code = pm_peers_delete();
    APP_ERROR_CHECK(err_code);
    memset(&m_cli_bonded, 0, sizeof(m_cli_bonded) );
}


void key_request_set(void)
{
    m_key_request_event = true;
}


/**@brief Function for setting the PHY.
 *
 * @param[in] value            New PHY value.
 * @param[in] conn_handle      The connection handle identifying the connection to perform this procedure on.
 */
static void phy_set(nrf_cli_t const * p_cli, uint8_t value, uint16_t conn_handle)
{
    ble_gap_phys_t phy =
    {
        .rx_phys = value,
        .tx_phys = value,
    };

    // Set new PHY value.
    preferred_phy_set(&phy, conn_handle);
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Preferred PHY set to %s.\n", phy_str(&phy));
}


/**@brief Function for restoring the default connection parameters.
 *
 * @param[out] Pointer to conn_params.
 */
static void default_con_param(ble_gap_conn_params_t * p_params_update)
{
    p_params_update->min_conn_interval = MIN_CONNECTION_INTERVAL;
    p_params_update->max_conn_interval = MAX_CONNECTION_INTERVAL;
    p_params_update->slave_latency     = SLAVE_LATENCY;
    p_params_update->conn_sup_timeout  = SUPERVISION_TIMEOUT;
}


/**@brief Function for printing the connectable devices.
 *
 *@details Function print list of available to connect device address and name (if exist).
 *         Display format: XX:XX:XX:XX:XX:XX device_name .
 *
 * @param[in] p_cli       Pointer to the instance of the command line module.
 * @param[in] device      Pointer to the struct storing the scanned devices.
 */
static void device_list_print(nrf_cli_t const * p_cli, scanned_device_t * p_device)
{
    for (uint8_t i = 0; i < DEVICE_TO_FIND_MAX; i++)
    {
        if (p_device[i].is_not_empty)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Device ");

            char buffer[ADDR_STRING_LEN];
            int_addr_to_hex_str(buffer, BLE_GAP_ADDR_LEN, p_device[i].addr);

            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%s %s\n", buffer,  p_device[i].dev_name);
        }
    }
}


/**@brief Function for setting the CCCD Descriptor value.
 *
 * @param[in] p_cli          Instance of the command line.
 * @param[in] cccd           New CCCD value.
 * @param[in] addr           Device address in string format.
 * @param[in] uuid           Characteristic UUID in string format.
 */
static void cccd_set(nrf_cli_t const * p_cli, uint16_t cccd, char * p_addr, char * p_uuid)
{
    uint16_t                 conn_handle;
    uint16_t                 desc_handle;
    ret_code_t               err_code;
    ble_gattc_write_params_t write_params;

    // Search for connection handle.
    conn_handle = addr_string_to_conn_handle(p_addr);

    if(!conn_handle_is_valid(conn_handle, p_cli))
    {
        return;
    }

    memset(&write_params, 0, sizeof(write_params));

    desc_handle = cccd_descriptors_handle_get(p_uuid);

    if (desc_handle == BLE_GATT_HANDLE_INVALID)
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "Wrong characteristic UUID or the CCCD descriptor has not been found yet\n");
        return;
    }

    uint16_t cccd_val    = cccd;
    uint8_t  data_buf[2] = {LSB_16(cccd_val), MSB_16(cccd_val)};
    uint8_t  data_len    = sizeof(data_buf);

    write_params.write_op = BLE_GATT_OP_WRITE_REQ;
    write_params.handle   = desc_handle;
    write_params.len      = data_len;
    write_params.p_value  = data_buf;
    write_params.offset   = 0;
    write_params.flags    = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_WRITE;

    // Set CCCD descriptor value.
    err_code = sd_ble_gattc_write(conn_handle, &write_params);

    if (err_code != NRF_SUCCESS)
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "wrong characteristic UUID or the CCCD descriptor has not been found yet\n");
    }
}


/**@brief Command handler for displaying the connected devices.
 */
static void cmd_connected_display(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    ret_code_t         err_code;
    uint16_t           conn_handle;
    ble_gap_conn_sec_t security_params;
    memset(&security_params, 0, sizeof(security_params));

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Connected devices:\n");

    for (uint8_t i = 0; i < m_cli_connected.cmd_counter; i++)
    {
        conn_handle = addr_string_to_conn_handle(
            m_cli_connected.buffer_type.connected_cmd_buffer[i]);

        // Get connection security level.
        err_code = sd_ble_gap_conn_sec_get(conn_handle, &security_params);

        if (err_code != NRF_SUCCESS)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Security level reading has failed\n");

            return;
        }
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_NORMAL,
                        "%d. %s Security level: %d \n",
                        i,
                        m_cli_connected.buffer_type.connected_cmd_buffer[i],
                        security_params.sec_mode.lv);
    }
}


static void cmd_privacy(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    UNUSED_VARIABLE(argv);

    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s%s\n", UNKNOWN_PARAMETER, argv[1]);

}

/** @brief Command handler for setting privacy.
 */
static void cmd_privacy_on(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    UNUSED_VARIABLE(p_cli);
    UNUSED_VARIABLE(argc);
    UNUSED_VARIABLE(argv);

    ret_code_t               err_code;
    ble_gap_privacy_params_t privacy_params;

    // Privacy settings cannot be changed while advertising, scanning, or creating a connection.
    scan_stop();
    adv_stop();
    err_code = sd_ble_gap_connect_cancel();
    ret_code_verify(err_code);

    memset(&privacy_params, 0, sizeof(privacy_params));

    // Privacy setting.
    privacy_params.privacy_mode         = BLE_GAP_PRIVACY_MODE_DEVICE_PRIVACY;
    privacy_params.private_addr_type    = BLE_GAP_ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE;
    privacy_params.private_addr_cycle_s = PRIVATE_ADDRESS_INTERVAL;
    privacy_params.p_device_irk         = NULL;

    // Set privacy.
    err_code = sd_ble_gap_privacy_set(&privacy_params);
    APP_ERROR_CHECK(err_code);

    // Set device indentities list.
    err_code = sd_ble_gap_device_identities_set(NULL, NULL, NRF_SDH_BLE_PERIPHERAL_LINK_COUNT);
    APP_ERROR_CHECK(err_code);
}

/**@brief Command handler for disabling privacy.
 */
static void cmd_privacy_off(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    UNUSED_VARIABLE(p_cli);
    UNUSED_VARIABLE(argc);
    UNUSED_VARIABLE(argv);

    ret_code_t               err_code;
    ble_gap_privacy_params_t privacy_params;

     // Privacy settings cannot be changed while advertising, scanning, or creating a connection. 
    scan_stop();
    adv_stop();

    err_code = sd_ble_gap_connect_cancel();
    ret_code_verify(err_code);

    memset(&privacy_params, 0, sizeof(privacy_params));
    privacy_params.privacy_mode         = BLE_GAP_PRIVACY_MODE_OFF;
    privacy_params.private_addr_type    = BLE_GAP_ADDR_TYPE_RANDOM_STATIC;
    privacy_params.private_addr_cycle_s = BLE_GAP_DEFAULT_PRIVATE_ADDR_CYCLE_INTERVAL_S;
    privacy_params.p_device_irk         = NULL;

    // Set privacy.
    err_code = sd_ble_gap_privacy_set(&privacy_params);
    APP_ERROR_CHECK(err_code);
}


static void cmd_numeric(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s\n", UNKNOWN_PARAMETER, argv[1]);
}


/**@brief Command handler for accepting a numeric value.
 */
static void cmd_numeric_accept(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    UNUSED_VARIABLE(argc);
    UNUSED_VARIABLE(argv);

    ret_code_t err_code;

    if (is_numeric_match_requested())
    {
        // If numeric match request and keys on the both devices are the same then accept.
        err_code = sd_ble_gap_auth_key_reply(numeric_match_request_conn_handle_get(),
                                             BLE_GAP_AUTH_KEY_TYPE_PASSKEY, NULL); // Accept
        APP_ERROR_CHECK(err_code);

        // Clear numeric match flag.
        numeric_match_request_clear();
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Numeric Match\n");
    }
    else
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "This command can be used only with numeric comparison pairing\n");
    }
}


/**@brief Command handler for rejecting a numeric value.
 */
static void cmd_numeric_reject(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    UNUSED_VARIABLE(argc);
    UNUSED_VARIABLE(argv);

    ret_code_t err_code;

    if (is_numeric_match_requested())
    {
        // If numeric match request and keys on both devices are not the same, then reject.
        err_code = sd_ble_gap_auth_key_reply(numeric_match_request_conn_handle_get(),
                                             BLE_GAP_AUTH_KEY_TYPE_NONE, NULL); // Reject
        APP_ERROR_CHECK(err_code);

        // Clear a numeric match flag.
        numeric_match_request_clear();
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Numeric Reject\n");
    }
    else
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "This command can be used only with numeric comparison pairing\n");
    }
}


#if NRF_MODULE_ENABLED(ADAFRUIT_SHIELD)
static void cmd_nfc_reader(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }
    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s%s\n", UNKNOWN_PARAMETER, argv[1]);
}


/**@brief Command handler for pairing by NFC in central role.
 */
static void cmd_nfc_reader_on(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    UNUSED_VARIABLE(argv);

    if (argc != 1)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, WRONG_PARAMETER_COUNT);
        return;
    }

    ret_code_t err_code;

    // In order to avoid a mistake when reusing the command.
    nfc_pair_stop();
    err_code = adafruit_pn532_field_off();
    ret_code_verify(err_code);

    // Turn on the Adafruit tag reader.
    err_code = adafruit_pn532_field_on();
    APP_ERROR_CHECK(err_code);

    nfc_pair_start();
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "NFC reader ON\n");
}


/**@brief Command handler for disabling pairing by NFC in central role.
 */
static void cmd_nfc_reader_off(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    UNUSED_VARIABLE(argv);

    ret_code_t err_code;

    if (argc != 1)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, WRONG_PARAMETER_COUNT);
        return;
    }

    nfc_pair_stop();
    err_code = adafruit_pn532_field_off();
    ret_code_verify(err_code);

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "NFC reader OFF\n");
}


#endif // ADAFRUIT_SHIELD

static void cmd_gatt(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s%s\n", UNKNOWN_PARAMETER, argv[1]);
}



/**@brief Command handler for services discovery.
 */
static void cmd_services_discovery(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    ret_code_t err_code;
    uint16_t   conn_handle;

    // Start handle to look for services.
    uint16_t handle = 0x0001;
    conn_handle = addr_string_to_conn_handle(argv[1]);
    if(!conn_handle_is_valid(conn_handle, p_cli))
    {
        return;
    }

    // Discovery primary services.
    err_code = sd_ble_gattc_primary_services_discover(conn_handle, handle, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Command handler for characteristics discovery.
 */
static void cmd_char_get(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    uint16_t   conn_handle;
    ret_code_t err_code;

    // Search for a connection handle.
    conn_handle = addr_string_to_conn_handle(argv[1]);

    if(!conn_handle_is_valid(conn_handle, p_cli))
    {
        return;
    }

    // Look for start handle range.
    ble_gattc_handle_range_t * p_handle_range = handle_range_search(argv[2], conn_handle);

    // Characteristic discovery.
    err_code = sd_ble_gattc_characteristics_discover(conn_handle, p_handle_range);

    if (err_code != NRF_SUCCESS)
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "incorrect service UUID or service does not exist\n");
    }
}



/**@brief Command handler for reading a characteristic value.
 */
static void cmd_char_read(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    uint16_t   conn_handle;
    uint16_t   val_handle;
    ret_code_t err_code;

    // Search for a connection handle.
    conn_handle = addr_string_to_conn_handle(argv[1]);

    if(!conn_handle_is_valid(conn_handle, p_cli))
    {
        return;
    }

    // Search for a Characterisctic Value Handle.
    val_handle = handle_value_search(argv[2]);
    if (val_handle == BLE_GATT_HANDLE_INVALID)
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "%s%s\n",
                        "The selected service does not contain the characteristic - ",
                        argv[2]);
        return;
    }

    // Read characteristic data.
    err_code = sd_ble_gattc_read(conn_handle, val_handle, 0);
    APP_ERROR_CHECK(err_code);
}


/**@brief Command handler for writing a value to the characteristic.
 */
static void cmd_char_write(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{

    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    uint16_t                 conn_handle;
    ret_code_t               err_code;
    ble_gattc_write_params_t write_params;
    uint8_t                  data_buf[GATT_DATA_WRITE_SIZE];
    uint16_t                 val_handle;

    // Calculate data length, first 4 argv are commands, the rest of argv are data.
    uint16_t data_len = argc - 4;

    memset(&write_params, 0, sizeof(write_params));

    if (!strcmp(argv[1], "command"))
    {
        write_params.write_op = BLE_GATT_OP_WRITE_CMD;
    }
    else if (!strcmp(argv[1], "request"))
    {
        write_params.write_op = BLE_GATT_OP_WRITE_REQ;
    }
    else
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s%s\n", UNKNOWN_PARAMETER, argv[1]);
        return;
    }

    // Search for a connection handle.
    conn_handle = addr_string_to_conn_handle(argv[2]);

    if(!conn_handle_is_valid(conn_handle, p_cli))
    {
        return;
    }

    // Search for a Characteristic Value Handle.
    val_handle = handle_value_search(argv[3]);

    if (val_handle == BLE_GATT_HANDLE_INVALID)
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "%s%s\n",
                        "The selected service does not contain the characteristic - ",
                        argv[3]);
        return;
    }

    write_params.handle  = val_handle;
    write_params.len     = data_len;
    write_params.p_value = data_buf;
    write_params.offset  = 0;
    write_params.flags   = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_WRITE;

    for (uint8_t i = 0; i < data_len; i++)
    {
        data_buf[i] = atoi(argv[i + 4]);
    }

    // Write data to characteristic.
    err_code = sd_ble_gattc_write(conn_handle, &write_params);
    APP_ERROR_CHECK(err_code);
}


static void cmd_notify(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s%s\n", UNKNOWN_PARAMETER, argv[1]);
}

/**@brief Command handler for enabling a notification.
 */
static void cmd_notify_on(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    UNUSED_VARIABLE(argc);

    // In order to enable the notification, the CCCD descriptor should take the value 1.
    cccd_set(p_cli, 0x01, argv[1], argv[2]);
}


/**@brief Command handler for disabling a notification.
 */
static void cmd_notify_off(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    UNUSED_VARIABLE(argc);
    // In order to disable the notification, the CCCD descriptor should take the value 0.
    cccd_set(p_cli, 0x00, argv[1], argv[2]);
}


static void cmd_ind(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s%s\n", UNKNOWN_PARAMETER, argv[1]);

}


/**@brief Command handler for enabling an indication.
 */
static void cmd_ind_on(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    UNUSED_VARIABLE(argc);

    // In order to enable the indication, the CCCD descriptor should take the value 2.
    cccd_set(p_cli, 0x02, argv[1], argv[2]);
}


/**@brief Command handler for disabling an indication.
 */
static void cmd_ind_off(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    UNUSED_VARIABLE(argc);

    // In order to disable the indication, the CCCD descriptor should take the value 0.
    cccd_set(p_cli, 0x00, argv[1], argv[2]);
}


/**@brief Command handler for setting link layer data length.
 */
static void cmd_data_length_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (argc != 3)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, WRONG_PARAMETER_COUNT);
        return;
    }

    uint16_t conn_handle;

    for (uint8_t i = 0; i < strlen(argv[2]); i++)
    {
        if (!isdigit((int) argv[2][i]))
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "data length must be a number\n");
            return;
        }
    }

    // Search for connection handle.
    conn_handle = addr_string_to_conn_handle(argv[1]);

    if(!conn_handle_is_valid(conn_handle, p_cli))
    {
        return;
    }

    // Set new link layer data length.
    link_layer_data_length_set(argv[2], conn_handle);
}


/**@brief Command handler for setting the PHY to 1 MBPS.
 */
static void cmd_phy_1m_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    UNUSED_VARIABLE(argc);

    uint16_t conn_handle;

    // Search for a connection handle.
    conn_handle = addr_string_to_conn_handle(argv[1]);

    if(!conn_handle_is_valid(conn_handle, p_cli))
    {
        return;
    }

    // PHY value set.
    phy_set(p_cli, BLE_GAP_PHY_1MBPS, conn_handle);
}



/**@brief Command handler for setting the PHY to 2 MBPS.
 */
static void cmd_phy_2m_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    UNUSED_VARIABLE(argc);

    uint16_t conn_handle;

    // Search for a connection handle.
    conn_handle = addr_string_to_conn_handle(argv[1]);

    if(!conn_handle_is_valid(conn_handle, p_cli))
    {
        return;
    }

    // PHY value set.
    phy_set(p_cli, BLE_GAP_PHY_2MBPS, conn_handle);
}


#ifdef NRF52840_XXAA
/**@brief Command handler for setting the PHY to coded.
 */
static void cmd_phy_coded_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    UNUSED_VARIABLE(argc);

    uint16_t conn_handle;

    // Search for a connection handle.
    conn_handle = addr_string_to_conn_handle(argv[1]);

    if(!conn_handle_is_valid(conn_handle, p_cli))
    {
        return;
    }

    // PHY value set.
    phy_set(p_cli, BLE_GAP_PHY_CODED, conn_handle);
}


#endif

static void cmd_phy(nrf_cli_t const * p_cli, size_t argc, char * * argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s%s\n", UNKNOWN_PARAMETER, argv[1]);

}


static void cmd_parameters(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s%s\n", UNKNOWN_PARAMETER, argv[1]);
}


/**@brief Command handler for setting the MTU.
 */
static void cmd_mtu_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (argc != 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, WRONG_PARAMETER_COUNT);
        return;
    }

    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    for (uint8_t i = 0; i < strlen(argv[1]); i++)
    {
        if (!isdigit((int) argv[1][i]))
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "MTU must be a number\n");
            return;
        }
    }

    // Set a new MTU value.
    gatt_mtu_set(argv[1]);
}


/**@brief Command handler for setting the Minimum Connection Interval.
 */
static void cmd_min_conn_interval_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    uint16_t value;

    if (argc != 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, WRONG_PARAMETER_COUNT);
        return;
    }

    for (uint8_t i = 0; i < strlen(argv[1]); i++)
    {
        if (!isdigit((int) argv[1][i]))
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "min_conn_interval must be a number\n");
            return;
        }
    }

    value = (uint16_t)MSEC_TO_UNITS(atoi(argv[1]), UNIT_1_25_MS);

    // Checking the value is within the required range.
    if ((value < MSEC_TO_UNITS(BLE_GAP_CP_MIN_CONN_INTVL_MIN, UNIT_1_25_MS)) || 
        (value > MSEC_TO_UNITS(BLE_GAP_CP_MIN_CONN_INTVL_MAX, UNIT_1_25_MS)))
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s %d and %d.\n",
                        "Minimum connection interval must be between",
                        BLE_GAP_CP_MIN_CONN_INTVL_MIN,
                        BLE_GAP_CP_MIN_CONN_INTVL_MAX);
        return;
    }

    m_params_update.min_conn_interval = value;
}


/**@brief Command handler for setting Maximum Connection Interval.
 */
static void cmd_max_conn_interval_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    uint16_t value;

    if (argc != 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, WRONG_PARAMETER_COUNT);
        return;
    }

    for (uint8_t i = 0; i < strlen(argv[1]); i++)
    {
        if (!isdigit((int) argv[1][i]))
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "max_conn_interval must be a number.\n");
            return;
        }
    }

    value = (uint16_t)MSEC_TO_UNITS(atoi(argv[1]), UNIT_1_25_MS);

    // Checking whether the value is within the required range.
    if ((value < MSEC_TO_UNITS(BLE_GAP_CP_MAX_CONN_INTVL_MIN, UNIT_1_25_MS)) || 
        (value > MSEC_TO_UNITS(BLE_GAP_CP_MAX_CONN_INTVL_MAX, UNIT_1_25_MS)))
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s %d and %d.\n",
                        "Maximum connection interval must be between",
                        BLE_GAP_CP_MAX_CONN_INTVL_MIN,
                        BLE_GAP_CP_MAX_CONN_INTVL_MAX);
        return;
    }

    m_params_update.max_conn_interval = value;
}


/**@brief Command handler for setting Slave Latency.
 */
static void cmd_slave_latency_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    uint16_t value;

    if (argc != 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, WRONG_PARAMETER_COUNT);
        return;
    }

    for (uint8_t i = 0; i < strlen(argv[1]); i++)
    {
        if (!isdigit((int) argv[1][i]))
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Slave latency must be a number\n");
            return;
        }
    }

    value = atoi(argv[1]);

    if (value > BLE_GAP_CP_SLAVE_LATENCY_MAX)
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "%s%d.\n",
                        "Slave latency should be less than: ",
                        BLE_GAP_CP_SLAVE_LATENCY_MAX);
        return;
    }

    m_params_update.slave_latency = value;
}


/**@brief Command handler for setting Supervisory time-out.
 */
static void cmd_sup_timeout_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    uint16_t value;
    uint16_t check;
    uint16_t valid;

    if (argc != 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, WRONG_PARAMETER_COUNT);
        return;
    }

    for (uint8_t i = 0; i < strlen(argv[1]); i++)
    {
        if (!isdigit((int) argv[1][i]))
        {
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_ERROR,
                            "Supervision time-out must be a number.\n");
            return;
        }
    }

    check = atoi(argv[1]);
    value = (uint16_t)MSEC_TO_UNITS(check, UNIT_10_MS);
    valid = (1 + m_params_update.slave_latency) * UNIT_TO_MSEC(m_params_update.max_conn_interval,
                                                               UNIT_1_25_MS) * 2;
    // Checking whether the value is within the required range.
    if ((value < MSEC_TO_UNITS(BLE_GAP_CP_CONN_SUP_TIMEOUT_MIN, UNIT_10_MS)) ||
        (value > MSEC_TO_UNITS(BLE_GAP_CP_CONN_SUP_TIMEOUT_MAX, UNIT_10_MS)))
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "%s  %d and %d.\n",
                        "Supervision time-out must be between",
                        BLE_GAP_CP_CONN_SUP_TIMEOUT_MIN,
                        BLE_GAP_CP_CONN_SUP_TIMEOUT_MAX);
        return;
    }

    if (check <= valid)
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "%s \n%s%d\n",
                        "Supervision_Timeout in milliseconds shall be larger than:",
                        "(1 + Slave_Latency) * Conn_Interval_Max * 2 - ",
                        valid);
        return;
    }

    m_params_update.conn_sup_timeout = value;
}


/**@brief Command handler for setting connection parameters.
 */
static void cmd_connection_params_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (argc != 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, WRONG_PARAMETER_COUNT);
        return;
    }

    ret_code_t err_code;
    uint16_t   conn_handle;

    // Search for a connection handle.
    conn_handle = addr_string_to_conn_handle(argv[1]);

    if(!conn_handle_is_valid(conn_handle, p_cli))
    {
        return;
    }

    // Set connection parameters for the connection.
    err_code = sd_ble_gap_conn_param_update(conn_handle, &m_params_update);

    if (err_code == NRF_SUCCESS)
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_NORMAL,
                        "Preferred connection parameters changed successfully\n");

        default_con_param(&m_params_update);
    }
    else
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "Connection parameters could not be changed\n");
    }
}


/**@brief Command handler for displaying connectable devices.
 */
static void cmd_devices_display(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (argc >= 2)
    {
        if (nrf_cli_help_requested(p_cli))
        {
            nrf_cli_help_print(p_cli, NULL, 0);
            return;
        }
        else
        {
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_ERROR,
                            "%s:%s%s\n",
                            argv[0],
                            " bad parameter ",
                            argv[1]);
            return;
        }
    }

    // Print connectable devices from scan data.
    scanned_device_t * p_device_list = scan_device_info_get();
    device_list_print(p_cli, p_device_list);
}


static void cmd_remove(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s%s\n", UNKNOWN_PARAMETER, argv[1]);
}


/**@brief Command handler for displaying bonded devices.
 */
static void cmd_bonded_devices_display(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (argc > 1)
    {
        if (nrf_cli_help_requested(p_cli))
        {
            nrf_cli_help_print(p_cli, NULL, 0);
            return;
        }

        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, WRONG_PARAMETER_COUNT);
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Peers count: %d \n", pm_peer_count());

    pm_peer_id_t current_peer_id = pm_next_peer_id_get(PM_PEER_ID_INVALID);

    // Display all bonded devices.
    while (current_peer_id != PM_PEER_ID_INVALID)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "ID: %d  address: ", current_peer_id);

        ret_code_t             err_code;
        pm_peer_data_bonding_t data;
        char                   string_addr_buf[ADDR_STRING_LEN];

        // Get bonding data.
        err_code = pm_peer_data_bonding_load(current_peer_id, &data);
        APP_ERROR_CHECK(err_code);
        current_peer_id = pm_next_peer_id_get(current_peer_id);

        // Convert device address to string.
        int_addr_to_hex_str(string_addr_buf,
                            BLE_GAP_ADDR_LEN,
                            data.peer_ble_id.id_addr_info.addr);

        // Display bonded device.
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%s\n", string_addr_buf);
    }
}


/**@brief Command handler for pairing with a device.
 */
static void cmd_device_pair(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (!strcmp(argv[1], "LESC"))
    {
        m_sec_params_central.lesc = 1;
    }
    else if (!strcmp(argv[1], "LEGACY"))
    {
        m_sec_params_central.lesc = 0;
    }
    else
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s:%s%s\n", argv[0], " wrong command ", argv[1]);
        return;
    }

    ret_code_t err_code;

    // Search for connection handle.
    m_conn_handle = addr_string_to_conn_handle(argv[2]);

    if(!conn_handle_is_valid(m_conn_handle, p_cli))
    {
        return;
    }

    if (!strcmp(argv[3], "mitm"))
    {
        m_sec_params_central.mitm    = 1;
        m_sec_params_central.io_caps = BLE_GAP_IO_CAPS_KEYBOARD_DISPLAY;
    }
    else
    {
        m_sec_params_central.mitm    = 0;
        m_sec_params_central.io_caps = BLE_GAP_IO_CAPS_NONE;
    }

    if (!strcmp(argv[3], "oob"))
    {
        m_sec_params_central.mitm    = 1;
        m_sec_params_central.oob     = 1;
        m_sec_params_central.io_caps = BLE_GAP_IO_CAPS_NONE;
    }

    // Based on command parameters, set security parameters.
    err_code = pm_sec_params_set(&m_sec_params_central);
    APP_ERROR_CHECK(err_code);

    // Establishing encryption on a connection, and optionally establishing a bond.
    err_code = pm_conn_secure(m_conn_handle, false);

    if (err_code != NRF_SUCCESS)
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "Device is not connected or is already paired\n");
    }
    else
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Start pairing\n");
    }
}


/**@brief Command handler for reply to BLE_GAP_EVT_AUTH_KEY_REQUEST.
 */
static void cmd_key_reply(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (argc > 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, WRONG_PARAMETER_COUNT);
        return;
    }

    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (!m_key_request_event)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "No key request\n");
        return;
    }

    if (strlen(argv[1]) != KEY_PASSKEY_LEN)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s", "invalid key length!\n");
        return;
    }

    ret_code_t err_code;
    uint8_t    key[KEY_PASSKEY_LEN];

    for (uint8_t i = 0; i < KEY_PASSKEY_LEN; i++)
    {
        if (!isdigit((int) argv[1][i]))
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "the key must consist of digits\n");
            return;
        }

        key[i] = argv[1][i];
    }

    // Reply with an authentication key. 
    err_code = sd_ble_gap_auth_key_reply(m_conn_handle,
                                         BLE_GAP_AUTH_KEY_TYPE_PASSKEY,
                                         key);
    m_key_request_event = false;

    if (err_code != NRF_SUCCESS)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Error sending key\n");
    }
    else
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Key sending success\n");
    }
}


/**@brief Command handler for setting a device name.
 */
static void cmd_device_name_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    ret_code_t              err_code;
    ble_gap_conn_sec_mode_t sec_mode;
    char                    buf[DEVICE_NAME_MAX_SIZE];

    memset(buf, 0, DEVICE_NAME_MAX_SIZE);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    for (uint8_t i = 1; i < argc; i++)
    {
        strcat(buf, argv[i]);

        if (i == (argc - 1))
        {
            break;
        }

        strcat(buf, " ");
    }

    // Set new device name.
    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)buf,
                                          strlen(buf));

    //Re-initialization of advertising with new device name.
    advertising_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Command handler for enabling advertising.
 */
static void cmd_advertise_on(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t err_code;

    if (argc > 1)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, WRONG_PARAMETER_COUNT);
        return;
    }

    adv_stop();
    err_code = nfc_ble_pair_stop();
    ret_code_verify(err_code);
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Advertising enabled\n");
    adv_start();
    err_code = nfc_ble_pair_start();
    APP_ERROR_CHECK(err_code);
}


/**@brief Command handler for disabling advertising.
 */
static void cmd_advertise_off(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t err_code;

    if (argc > 1)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, WRONG_PARAMETER_COUNT);
        return;
    }

    adv_stop();
    err_code = nfc_ble_pair_stop();
    ret_code_verify(err_code);
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Advertising disabled\n");
}


static void cmd_advertise(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s%s\n", UNKNOWN_PARAMETER, argv[1]);
}


/**@brief Command handler for disabling scanning.
 */
static void cmd_scan_off(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (argc != 1)
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "%s %s: %s",
                        argv[-1],
                        argv[0],
                        WRONG_PARAMETER_COUNT);
        return;
    }

    scan_stop();
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Scan stopped\n");
}


/**@brief Command handler for enabling scanning.
 */
static void cmd_scan_on(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (argc != 1)
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "%s %s: %s",
                        argv[-1],
                        argv[0],
                        WRONG_PARAMETER_COUNT);
        return;
    }

    scan_start();
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Scan started\n");
}


static void cmd_scan(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "%s%s\n",
                        UNKNOWN_PARAMETER,
                        argv[1]);
}


/**@brief Command handler for establishing a connection.
 */
static void cmd_device_connect(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc == 2)
    {
        //Connect by peer ID if davice was bonded and using privacy.
        if (strlen(argv[1]) < (ADDR_STRING_LEN - 1))
        {
            uint16_t pm_id;
            pm_id = atoi(argv[1]);
            private_connect(&pm_id);
            return;
        }

        // If address or peer ID have incorrect format, then return.
        if (strlen(argv[1]) != (ADDR_STRING_LEN - 1))
        {
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_ERROR,
                            "wrong address length - Correct format: XX:XX:XX:XX:XX:XX\n");
            return;
        }

        //Connect by device address.
        addr_store_value_set(argv[1]);
    }
    else
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, WRONG_PARAMETER_COUNT);
    }
}


/**@brief Command handler for disconnecting from a device.
 */
static void cmd_device_disconnect(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc > 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: %s", argv[0], WRONG_PARAMETER_COUNT);
        return;
    }

    if (strlen(argv[1]) != (ADDR_STRING_LEN - 1))
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "wrong address length - Correct format: XX:XX:XX:XX:XX:XX\n");
        return;
    }

    uint16_t   conn_handle;
    ret_code_t err_code;

    // Search for a connection handle.
    conn_handle = addr_string_to_conn_handle(argv[1]);

    if(!conn_handle_is_valid(conn_handle, p_cli))
    {
        return;
    }

    // Disconnect from a device.
    err_code = sd_ble_gap_disconnect(conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    ret_code_verify(err_code);
}


static void cmd_all_bonds_delete(nrf_cli_t const * p_cli, size_t argc, char * * argv)
{
	if (argc == 1)
	{
	    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Bonding data deleted\n");
	    bonds_delete();
	    return;
	}

	if (nrf_cli_help_requested(p_cli))
	{
	    nrf_cli_help_print(p_cli, NULL, 0);
	    return;
	}

	nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s%s\n", UNKNOWN_PARAMETER, argv[1]);
}


/**@brief Command handler for removing device bonding data.
 */
static void cmd_device_remove(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    ret_code_t   err_code;
    pm_peer_id_t current_peer_id = pm_next_peer_id_get(PM_PEER_ID_INVALID);

    // Search for device bondind data.
    while (current_peer_id != PM_PEER_ID_INVALID)
    {
        pm_peer_data_bonding_t p_data;

        err_code = pm_peer_data_bonding_load(current_peer_id, &p_data);
        APP_ERROR_CHECK(err_code);

        if (is_address_compare(&p_data.peer_ble_id.id_addr_info, argv[1]))
        {
            // If bonding data was found, then remove.
            err_code = pm_peer_delete(current_peer_id);
            APP_ERROR_CHECK(err_code);

            // Remove device from bonded device command buffer.
            cli_addr_from_command_remove(argv[1],
                                         &m_cli_bonded,
                                         CLI_CMD_BONDED);
        }

        current_peer_id = pm_next_peer_id_get(current_peer_id);
    }
}


/* Dynamic command creation */
static void connected_get(size_t idx, nrf_cli_static_entry_t * p_static)
{
    // Must be sorted alphabetically to ensure correct CLI completion.
    p_static->p_syntax = (idx < m_cli_connected.cmd_counter) ?
                         m_cli_connected.buffer_type.connected_cmd_buffer[idx] : NULL;
    p_static->handler  = NULL;
    p_static->p_subcmd = NULL;
    p_static->p_help   = 0;
}


/* Dynamic command creation */
static void connect_addr_get(size_t idx, nrf_cli_static_entry_t * p_static)
{
    // Must be sorted alphabetically to ensure correct CLI completion.
    p_static->p_syntax = (idx < m_cli_connect_addr.cmd_counter) ?
                         m_cli_connect_addr.buffer_type.connect_addr_cmd_buffer[idx] : NULL;
    p_static->handler  = NULL;
    p_static->p_subcmd = NULL;
    p_static->p_help   = "Connect with address.";
}


/* Dynamic command creation */
static void delete_bond_get(size_t idx, nrf_cli_static_entry_t * p_static)
{
    // Must be sorted alphabetically to ensure correct CLI completion.
    p_static->p_syntax = (idx < m_cli_bonded.cmd_counter) ?
                         m_cli_bonded.buffer_type.bond_cmd_buffer[idx] : NULL;
    p_static->handler  = NULL;
    p_static->p_subcmd = NULL;
    p_static->p_help   = "Delete device bond by address.";
}





/* Creating subcommands (level 1) for command "LESC and Legacy */
NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_pairing_opt)
{
    NRF_CLI_CMD(mitm, NULL, "MITM protection", NULL),
    NRF_CLI_CMD(oob, NULL, "OOB pair mode", NULL),
    NRF_CLI_SUBCMD_SET_END
};


/* Dynamic command creation */
static void pairing_get(size_t idx, nrf_cli_static_entry_t * p_static)
{
    // Must be sorted alphabetically to ensure correct CLI completion.
    p_static->p_syntax = (idx < m_cli_connected.cmd_counter) ?
                         m_cli_connected.buffer_type.connected_cmd_buffer[idx] : NULL;
    p_static->handler  = NULL;
    p_static->p_subcmd = &m_sub_pairing_opt;
    p_static->p_help   = NULL;
}


/* Creating dynamic subcommands (level 2) */
NRF_CLI_CREATE_DYNAMIC_CMD(m_sub_connect_addr_collection, connect_addr_get);
NRF_CLI_CREATE_DYNAMIC_CMD(m_sub_pairing_set, pairing_get);

/* Creating dynamic subcommands (level 1) */
NRF_CLI_CREATE_DYNAMIC_CMD(m_sub_connected_collection, connected_get);
NRF_CLI_CREATE_DYNAMIC_CMD(m_sub_delete_bond_collection, delete_bond_get);

/* Creating subcommands (level 2) for command "phy" */
NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_phy)
{
    NRF_CLI_CMD(1M,
                &m_sub_connected_collection,
                "Set preferred PHY to 1 Mbps.",
                cmd_phy_1m_set),
    NRF_CLI_CMD(2M,
                &m_sub_connected_collection,
                "Set preferred PHY to 2 Mbps.",
                cmd_phy_2m_set),
#ifdef NRF52840_XXAA
    NRF_CLI_CMD(coded,
                &m_sub_connected_collection,
                "Set preferred PHY to Coded.",
                cmd_phy_coded_set),
#endif
    NRF_CLI_SUBCMD_SET_END
};

/* Creating subcommands (level 2)*/
NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_write)
{
    NRF_CLI_CMD(command,
                &m_sub_connected_collection,
                "<char_uuid> <value> Write characteristic value(request).",
                NULL),
    NRF_CLI_CMD(request,
                &m_sub_connected_collection,
                "<char_uuid> <value> Write characteristic value(command).",
                NULL),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_notify)
{
    NRF_CLI_CMD(off, &m_sub_connected_collection, "Notifications off.", cmd_notify_off),
    NRF_CLI_CMD(on, &m_sub_connected_collection, "Notifications on.", cmd_notify_on),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_ind)
{
    NRF_CLI_CMD(off, &m_sub_connected_collection, "Indications off.", cmd_ind_off),
    NRF_CLI_CMD(on, &m_sub_connected_collection, "Indications on.", cmd_ind_on),
    NRF_CLI_SUBCMD_SET_END
};

/* Creating subcommands (level 1)*/
NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_privacy)
{
    NRF_CLI_CMD(off, NULL, "Privacy advertising mode off.", cmd_privacy_off),
    NRF_CLI_CMD(on, NULL, "Privacy advertising mode on.", cmd_privacy_on),
    NRF_CLI_SUBCMD_SET_END
};
NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_numeric)
{
    NRF_CLI_CMD(accept, NULL, "Accepts pin value.", cmd_numeric_accept),
    NRF_CLI_CMD(reject, NULL, "Reject pin value.", cmd_numeric_reject),
    NRF_CLI_SUBCMD_SET_END
};

#if NRF_MODULE_ENABLED(ADAFRUIT_SHIELD)
NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_nfc_reader)
{
    NRF_CLI_CMD(off, NULL, "NFC reader off.", cmd_nfc_reader_off),
    NRF_CLI_CMD(on, NULL, "NFC reader on.", cmd_nfc_reader_on),
    NRF_CLI_SUBCMD_SET_END
};
#endif // ADAFRUIT_SHIELD
NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_gatt)
{
    NRF_CLI_CMD(characteristics,
                &m_sub_connected_collection,
                "<address> <serv_uuid> Get information about the contents of a service.",
                cmd_char_get),
    NRF_CLI_CMD(indication,
                &m_sub_ind,
                "<sub_cmd> <address> <char_uuid> Change indication state.",
                cmd_ind),
    NRF_CLI_CMD(notification,
                &m_sub_notify,
                "<sub_cmd> <address> <char_uuid> Change notification state.",
                cmd_notify),
    NRF_CLI_CMD(read,
                &m_sub_connected_collection,
                "<address> <char_uuid> Read characteristic value.",
                cmd_char_read),
    NRF_CLI_CMD(services,
                &m_sub_connected_collection,
                "<address> Download list of services.",
                cmd_services_discovery),
    NRF_CLI_CMD(write,
                &m_sub_write,
                "<sub_cmd> <address> <char_uuid> <value> Write characteristic value.",
                cmd_char_write),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_parameters)
{
    NRF_CLI_CMD(apply,
                &m_sub_connected_collection,
                "<address> Change connection parameters for the device.",
                cmd_connection_params_set),
    NRF_CLI_CMD(data_length,
                &m_sub_connected_collection,
                "<address> <value> Data length change.",
                cmd_data_length_set),
    NRF_CLI_CMD(max_conn_interval,
                NULL,
                "<value> Set maximum connection interval.",
                cmd_max_conn_interval_set),
    NRF_CLI_CMD(min_conn_interval,
                NULL,
                "<value> Set minimum connection interval.",
                cmd_min_conn_interval_set),
    NRF_CLI_CMD(mtu, NULL, "<value> Change MTU value.", cmd_mtu_set),
    NRF_CLI_CMD(phy, &m_sub_phy, "<subcmd> <address> PHY change.  ", cmd_phy),
    NRF_CLI_CMD(slave_latency, NULL, "<value> Set slave latency.", cmd_slave_latency_set),
    NRF_CLI_CMD(sup_timeout, NULL, "<value> Set connection supervision time-out.", cmd_sup_timeout_set),
    NRF_CLI_SUBCMD_SET_END
};
NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_remove)
{
    NRF_CLI_CMD(all, NULL, "Remove all bonded devices.", cmd_all_bonds_delete),
    NRF_CLI_CMD(device,
                &m_sub_delete_bond_collection,
                "<address> Remove one bonded device.",
                cmd_device_remove),
    NRF_CLI_SUBCMD_SET_END
};
NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_pairing)
{
    NRF_CLI_CMD(LEGACY, &m_sub_pairing_set, "Legacy pairing mode.", NULL),
    NRF_CLI_CMD(LESC, &m_sub_pairing_set, "LESC pairing mode.", NULL),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_advertise)
{
    NRF_CLI_CMD(off, NULL, "Advertising off.", cmd_advertise_off),
    NRF_CLI_CMD(on, NULL, "Advertising on.", cmd_advertise_on),
	NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_scan)
{
    NRF_CLI_CMD(off, NULL, "Stop scanning.", cmd_scan_off),
    NRF_CLI_CMD(on, NULL, "Start scanning.", cmd_scan_on),
    NRF_CLI_SUBCMD_SET_END
};

/* Creating root (level 0) command */
NRF_CLI_CMD_REGISTER(connected_devices,
                     NULL,
                     "Display connected devices and information about security level.",
                     cmd_connected_display);
NRF_CLI_CMD_REGISTER(privacy, &m_sub_privacy, "Set privacy settings.", cmd_privacy);
NRF_CLI_CMD_REGISTER(numeric,
                     &m_sub_numeric,
                     "Confirm or reject the value of the pin code.",
                     cmd_numeric);
#if NRF_MODULE_ENABLED(ADAFRUIT_SHIELD)
NRF_CLI_CMD_REGISTER(nfc_read,
                     &m_sub_nfc_reader,
                     "Turn on NFC for reading <on/off>.",
                     cmd_nfc_reader);
#endif // ADAFRUIT_SHIELD
NRF_CLI_CMD_REGISTER(gatt, &m_sub_gatt, "Gatt options.", cmd_gatt);
NRF_CLI_CMD_REGISTER(parameters,
                     &m_sub_parameters,
                     "<subcmd> Change connection and link layer parameters.",
                     cmd_parameters);
NRF_CLI_CMD_REGISTER(bonded_devices, 
                     NULL,
                     "List bonded devices (The devices are identified by public address).",
                     cmd_bonded_devices_display);
NRF_CLI_CMD_REGISTER(devices, NULL, "List available devices.", cmd_devices_display);
NRF_CLI_CMD_REGISTER(remove_bond, &m_sub_remove, "<subcmd> Remove bonded device.", cmd_remove);
NRF_CLI_CMD_REGISTER(key_reply, NULL, "<key> Enter key from another device.", cmd_key_reply);
NRF_CLI_CMD_REGISTER(pair,
                     &m_sub_pairing,
                     "<subcmd> <address> <option> Start pairing with a evice.",
                     cmd_device_pair);
NRF_CLI_CMD_REGISTER(device_name, NULL, "<name> Set device name.", cmd_device_name_set);
NRF_CLI_CMD_REGISTER(advertise, &m_sub_advertise, "Turn advertising <on/off>.", cmd_advertise);
NRF_CLI_CMD_REGISTER(scan, &m_sub_scan, "Scan options", cmd_scan);
NRF_CLI_CMD_REGISTER(connect,
                     &m_sub_connect_addr_collection,
                     "<address/peer_id> Connect with a device.",
                     cmd_device_connect);
NRF_CLI_CMD_REGISTER(disconnect,
                     &m_sub_connected_collection,
                     "<address> Disconnect from a device.",
                     cmd_device_disconnect);
