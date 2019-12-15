/**
 * Copyright (c) 2013 - 2019, Nordic Semiconductor ASA
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
 * @defgroup ble_sdk_6lowpan_eval_main 6LoWPAN Adaptation Layer.
 * @{
 * @ingroup ble_sdk_6lowpan_eval
 * @brief 6LoWPAN Adaptation Layer Example.
 */

#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "nordic_common.h"
#include "nrf_soc.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "ble_advdata.h"
#include "ble_srv_common.h"
#include "ble_ipsp.h"
#include "iot_common.h"
#include "mem_manager.h"
#include "sdk_config.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define APP_IPSP_TAG                        35                                                      /**< Identifier for L2CAP configuration with the softdevice. */
#define APP_IPSP_ACCEPTOR_PRIO              1                                                       /**< Priority with the SDH on receiving events from the softdevice. */
#define ADVERTISING_LED                     BSP_LED_0_MASK                                          /**< Is on when device is advertising. */
#define CONNECTED_LED                       BSP_LED_1_MASK                                          /**< Is on when device is connected. */

#define APP_ADV_TIMEOUT                     0                                                       /**< Time for which the device must be advertising in non-connectable mode (in seconds). 0 disables timeout. */
#define APP_ADV_ADV_INTERVAL                MSEC_TO_UNITS(100, UNIT_0_625_MS)                       /**< The advertising interval. This value can vary between 100ms to 10.24s). */

#define DEVICE_NAME                         "IPSPAcceptor."
#define DEAD_BEEF                           0xDEADBEEF                                              /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define APP_ENABLE_LOGS                     1                                                       /**< Enable logs in the application. */

#if (APP_ENABLE_LOGS == 1)

#define APPL_LOG  NRF_LOG_INFO
#define APPL_DUMP NRF_LOG_RAW_HEXDUMP_INFO
#define APPL_ADDR IPV6_ADDRESS_LOG

#else // APP_ENABLE_LOGS

#define APPL_LOG(...)
#define APPL_DUMP(...)
#define APPL_ADDR(...)

#endif // APP_ENABLE_LOGS

#define MAX_IPSP_DATA_LEN   CEIL_DIV(BLE_IPSP_MTU, sizeof(uint32_t))
static uint32_t             ipsp_data[MAX_IPSP_DATA_LEN];
static uint32_t             ipsp_data_len = BLE_IPSP_MTU;
static ble_ipsp_handle_t    m_handle;

static ble_gap_addr_t       m_my_addr;                                                              /**< Local GAP address. */
static ble_gap_adv_params_t m_adv_params;                                                           /**< Parameters to be passed to the stack when starting advertising. */
static uint8_t              m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];                           /**< Buffer for storing an encoded advertising set. */
static uint8_t              m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;                          /**< Advertising handle used to identify an advertising set. */

/**@brief Struct that contains pointers to the encoded advertising data. */
static ble_gap_adv_data_t m_adv_data =
{
    .adv_data =
    {
        .p_data = m_enc_advdata,
        .len    = BLE_GAP_ADV_SET_DATA_SIZE_MAX
    },
    .scan_rsp_data =
    {
        .p_data = NULL,
        .len    = 0

    }
};

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for the LEDs initialization.
 *
 * @details Initializes all LEDs used by this application.
 */
static void leds_init(void)
{
    // Configure leds.
    LEDS_CONFIGURE((ADVERTISING_LED | CONNECTED_LED));

    // Set leds off.
    LEDS_OFF((ADVERTISING_LED | CONNECTED_LED));
}


/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void)
{
    uint32_t        err_code;
    ble_advdata_t   advdata;
    uint8_t         flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_addr_get(&m_my_addr);
    APP_ERROR_CHECK(err_code);

    m_my_addr.addr[5]   = 0x00;
    m_my_addr.addr[4]   = 0xEE;
    m_my_addr.addr[3]   = 0xDD;
    m_my_addr.addr[2]   = 0xCC;
    m_my_addr.addr[1]   = 0xBB;
    m_my_addr.addr[0]   = 0xAA;

    m_my_addr.addr_type = BLE_GAP_ADDR_TYPE_PUBLIC;

    err_code = sd_ble_gap_addr_set(&m_my_addr);
    APP_ERROR_CHECK(err_code);

    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.flags                   = flags;

//    err_code = ble_advdata_set(&advdata, NULL);
//    APP_ERROR_CHECK(err_code);

    err_code = ble_advdata_encode(&advdata, m_adv_data.adv_data.p_data, &m_adv_data.adv_data.len);
    APP_ERROR_CHECK(err_code);

    // Initialize advertising parameters (used when starting advertising).
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
    m_adv_params.p_peer_addr     = NULL;                                              // Undirected advertisement.
    m_adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval        = APP_ADV_ADV_INTERVAL;
    m_adv_params.duration        = APP_ADV_TIMEOUT;

    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, &m_adv_params);
    APP_ERROR_CHECK(err_code);
}


static void app_ipsp_data_send(void)
{
    memset(ipsp_data, (ipsp_data_len%255), ipsp_data_len);

    APPL_LOG("[0x%04X]:[0x%04X] Sending data on IPSP Channel. Pattern 0x%02X, length 0x%08lX.",
             m_handle.conn_handle, m_handle.cid, ((uint8_t *)ipsp_data)[0], ipsp_data_len);

    uint32_t err_code = ble_ipsp_send(&m_handle, (uint8_t *)ipsp_data, ipsp_data_len);

     APPL_LOG("[0x%04X]:[0x%04X] IPSP Data Send result 0x%08lX.",
              m_handle.conn_handle, m_handle.cid, err_code);

    ipsp_data_len--;
    if (ipsp_data_len == 0)
    {
        ipsp_data_len = BLE_IPSP_MTU;
    }
}


/**@brief IPSP event handler.
 * @param[in] p_handle Identifies the connection and channel on which the event occurred.
 * @param[in] p_evt    Event and related parameters (if any).
 *
 * @returns    NRF_SUCCESS.
 */
static uint32_t app_ipsp_event_handler(ble_ipsp_handle_t const * p_handle,
                                       ble_ipsp_evt_t const    * p_evt)
{
    switch (p_evt->evt_id)
    {
        case BLE_IPSP_EVT_CHANNEL_CONNECTED:
        {
            APPL_LOG("[0x%04X]:[0x%04X] BLE_IPSP_EVT_CHANNEL_CONNECTED Event. Result 0x%08lX",
                     p_handle->conn_handle, p_handle->cid, p_evt->evt_result);

            if (p_evt->evt_result == BLE_L2CAP_CH_STATUS_CODE_SUCCESS)
            {
                ipsp_data_len = BLE_IPSP_MTU;
                m_handle = (*p_handle);
                app_ipsp_data_send();
            }
            break;
        }
        case BLE_IPSP_EVT_CHANNEL_DISCONNECTED:
        {
            APPL_LOG("[0x%04X]:[0x%04X] BLE_IPSP_EVT_CHANNEL_DISCONNECTED Event. "
                      "Result 0x%08lX", p_handle->conn_handle, p_handle->cid, p_evt->evt_result);
            break;
        }
        case BLE_IPSP_EVT_CHANNEL_DATA_RX:
        {
            APPL_LOG("[0x%04X]:[0x%04X] BLE_IPSP_EVT_CHANNEL_DATA_RX Event. "
                      "Result 0x%08lX", p_handle->conn_handle, p_handle->cid, p_evt->evt_result);
            app_ipsp_data_send();
            break;
        }
        case BLE_IPSP_EVT_CHANNEL_DATA_TX_COMPLETE:
        {
            APPL_LOG("[0x%04X]:[0x%04X] BLE_IPSP_EVT_CHANNEL_DATA_TX_COMPLETE Event. "
                      "Result 0x%08lX", p_handle->conn_handle, p_handle->cid, p_evt->evt_result);
            break;
        }
        default:
        {
            APPL_LOG("[0x%04X]:[0x%04X] Unknown Event 0x%04X. "
                      "Result 0x%08lX", p_evt->evt_id, p_handle->conn_handle, p_handle->cid, p_evt->evt_result);
            break;
        }
    }
    return NRF_SUCCESS;
}


/**@brief Function for initializing services that will be used by the application.
 *
 * @details Initialize the Heart Rate, Battery and Device Information services.
 */
static void services_init(void)
{
    ble_ipsp_init_t init_param;
    init_param.evt_handler = app_ipsp_event_handler;

    uint32_t err_code = ble_ipsp_init(&init_param);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    uint32_t err_code;

    err_code = sd_ble_gap_adv_start(m_adv_handle, APP_IPSP_TAG);
    APP_ERROR_CHECK(err_code);

    LEDS_ON(ADVERTISING_LED);
}


/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t const * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            APPL_LOG("Connected.");

            LEDS_ON(CONNECTED_LED);
            LEDS_OFF(ADVERTISING_LED);

            break;
        case BLE_GAP_EVT_DISCONNECTED:
            APPL_LOG("Disconnected.");

            LEDS_OFF(CONNECTED_LED);

            advertising_start();
            break;
        default:
            break;
    }
}


/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the BLE Stack event interrupt handler after a BLE stack
 *          event has been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t const * p_ble_evt, void * p_context)
{
    UNUSED_PARAMETER(p_context);
    ble_ipsp_evt_handler(p_ble_evt);
    on_ble_evt(p_ble_evt);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t     err_code;
    uint32_t     ram_start = 0;
    ble_cfg_t    ble_cfg;

    err_code = nrf_sdh_enable_request();

    if (err_code == NRF_SUCCESS)
    {
        // Fetch the start address of the application RAM.
        err_code = nrf_sdh_ble_app_ram_start_get(&ram_start);
    }

    if (err_code == NRF_SUCCESS)
    {
        // Configure the maximum number of connections.
        memset(&ble_cfg, 0, sizeof(ble_cfg));
        ble_cfg.gap_cfg.role_count_cfg.periph_role_count  = BLE_IPSP_MAX_CHANNELS;
        ble_cfg.gap_cfg.role_count_cfg.central_role_count = 0;
        ble_cfg.gap_cfg.role_count_cfg.central_sec_count  = 0;
        err_code = sd_ble_cfg_set(BLE_GAP_CFG_ROLE_COUNT, &ble_cfg, ram_start);
    }

    if (err_code == NRF_SUCCESS)
    {
        memset(&ble_cfg, 0, sizeof(ble_cfg));

        // Configure total number of connections.
        ble_cfg.conn_cfg.conn_cfg_tag                     = APP_IPSP_TAG;
        ble_cfg.conn_cfg.params.gap_conn_cfg.conn_count   = BLE_IPSP_MAX_CHANNELS;
        ble_cfg.conn_cfg.params.gap_conn_cfg.event_length = BLE_GAP_EVENT_LENGTH_DEFAULT;
        err_code = sd_ble_cfg_set(BLE_CONN_CFG_GAP, &ble_cfg, ram_start);

    }

    if (err_code ==  NRF_SUCCESS)
    {
        memset(&ble_cfg, 0, sizeof(ble_cfg));

         // Configure the number of custom UUIDS.
        ble_cfg.common_cfg.vs_uuid_cfg.vs_uuid_count = 0;
        err_code = sd_ble_cfg_set(BLE_COMMON_CFG_VS_UUID, &ble_cfg, ram_start);
    }

    if (err_code == NRF_SUCCESS)
    {
        memset(&ble_cfg, 0, sizeof(ble_cfg));

        // Set L2CAP channel configuration

        // @note The TX MPS and RX MPS of initiator and acceptor are not symmetrically set.
        // This will result in effective MPS of 50 in reach direction when using the initiator and
        // acceptor example against each other. In the IPSP, the TX MPS is set to a higher value
        // as Linux which is the border router for 6LoWPAN examples uses default RX MPS of 230
        // bytes. Setting TX MPS of 212 in place of 50 results in better credit and hence bandwidth
        // utilization.
        ble_cfg.conn_cfg.conn_cfg_tag                        = APP_IPSP_TAG;
        ble_cfg.conn_cfg.params.l2cap_conn_cfg.rx_mps        = BLE_IPSP_RX_MPS;
        ble_cfg.conn_cfg.params.l2cap_conn_cfg.rx_queue_size = BLE_IPSP_RX_BUFFER_COUNT;
        ble_cfg.conn_cfg.params.l2cap_conn_cfg.tx_mps        = BLE_IPSP_TX_MPS;
        ble_cfg.conn_cfg.params.l2cap_conn_cfg.tx_queue_size = 1;
        ble_cfg.conn_cfg.params.l2cap_conn_cfg.ch_count      = 1; // One L2CAP channel per link.
        err_code = sd_ble_cfg_set(BLE_CONN_CFG_L2CAP, &ble_cfg, ram_start);
    }

    if (err_code == NRF_SUCCESS)
    {
        memset(&ble_cfg, 0, sizeof(ble_cfg));

        // Set the ATT table size.
        ble_cfg.gatts_cfg.attr_tab_size.attr_tab_size = 256;
        err_code = sd_ble_cfg_set(BLE_GATTS_CFG_ATTR_TAB_SIZE, &ble_cfg, ram_start);
    }

    if (err_code ==  NRF_SUCCESS)
    {
        err_code = nrf_sdh_ble_enable(&ram_start);
    }

    NRF_SDH_BLE_OBSERVER(m_ble_evt_observer, APP_IPSP_ACCEPTOR_PRIO, ble_evt_dispatch, NULL);

    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    // Initialize.
    log_init();
    leds_init();
    power_management_init();
    ble_stack_init();
    advertising_init();
    services_init();

    APPL_LOG("ble_app_ipsp_acceptor started.");
    APPL_LOG("Advertising.");

    // Start execution.
    advertising_start();

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }
}

/**
 * @}
 */
