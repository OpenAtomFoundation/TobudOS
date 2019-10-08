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
#include "app_button.h"
#include "app_timer.h"
#include "sdk_config.h"
#include "mem_manager.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define APP_IPSP_TAG                        35                                                      /**< Identifier for L2CAP configuration with the softdevice. */
#define APP_IPSP_INITIATOR_PRIO             1                                                       /**< Priority with the SDH on receiving events from the softdevice. */
#define SCANNING_LED                        BSP_LED_0_MASK                                          /**< Is on when device is scanning. */
#define CONNECTED_LED                       BSP_LED_1_MASK                                          /**< Is on when device is connected. */
#define BUTTON_DETECTION_DELAY              APP_TIMER_TICKS(50)
#define SCAN_INTERVAL                       0x00A0                                                  /**< Determines scan interval in units of 0.625 millisecond. */
#define SCAN_WINDOW                         0x0050                                                  /**< Determines scan window in units of 0.625 millisecond. */
#define MIN_CONNECTION_INTERVAL             MSEC_TO_UNITS(30, UNIT_1_25_MS)                         /**< Determines maximum connection interval in millisecond. */
#define MAX_CONNECTION_INTERVAL             MSEC_TO_UNITS(60, UNIT_1_25_MS)                         /**< Determines maximum connection interval in millisecond. */
#define SLAVE_LATENCY                       0                                                       /**< Determines slave latency in counts of connection events. */
#define SUPERVISION_TIMEOUT                 MSEC_TO_UNITS(4000, UNIT_10_MS)                         /**< Determines supervision time-out in units of 10 millisecond. */
#define DEAD_BEEF                           0xDEADBEEF                                              /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define APP_ENABLE_LOGS                 1                                                           /**< Enable logs in the application. */

#if (APP_ENABLE_LOGS == 1)

#define APPL_LOG  NRF_LOG_INFO
#define APPL_DUMP NRF_LOG_RAW_HEXDUMP_INFO
#define APPL_ADDR IPV6_ADDRESS_LOG

#else // APP_ENABLE_LOGS

#define APPL_LOG(...)
#define APPL_DUMP(...)
#define APPL_ADDR(...)

#endif // APP_ENABLE_LOGS

static ble_ipsp_handle_t    m_handle;
static uint16_t             m_conn_handle = BLE_CONN_HANDLE_INVALID;
static const ble_gap_addr_t m_peer_addr =
{
    .addr_type = BLE_GAP_ADDR_TYPE_PUBLIC,
    .addr = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x00}
};


/**
 * @brief Scan parameters requested for scanning and connection.
 */
static const ble_gap_scan_params_t m_scan_param =
{
     .active         = 0,                          // Passive scanning.
     .filter_policy  = BLE_GAP_SCAN_FP_ACCEPT_ALL, // Do not use whitelist.
     .interval       = (uint16_t)SCAN_INTERVAL,    // Scan interval.
     .window         = (uint16_t)SCAN_WINDOW,      // Scan window.
     .timeout        = 0,                          // Never stop scanning unless explicit asked to.
     .scan_phys      = BLE_GAP_PHY_AUTO            // Automatic PHY selection.
};


/**
 * @brief Connection parameters requested for connection.
 */
static const ble_gap_conn_params_t m_connection_param =
{
    .min_conn_interval = (uint16_t)MIN_CONNECTION_INTERVAL,   // Minimum connection
    .max_conn_interval = (uint16_t)MAX_CONNECTION_INTERVAL,   // Maximum connection
    .slave_latency     = 0,                                   // Slave latency
    .conn_sup_timeout  = (uint16_t)SUPERVISION_TIMEOUT        // Supervision time-out
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
    // Configure LEDs.
    LEDS_CONFIGURE((SCANNING_LED | CONNECTED_LED));

    // Set LEDs off.
    LEDS_OFF((SCANNING_LED | CONNECTED_LED));
}

/**@brief Function for handling button events.
 *
 * @param[in]   pin_no         The pin number of the button pressed.
 * @param[in]   button_action  The action performed on button.
 */
static void button_event_handler(uint8_t pin_no, uint8_t button_action)
{
    uint32_t err_code;
    if (button_action == APP_BUTTON_PUSH)
    {
        switch (pin_no)
        {
            case BSP_BUTTON_0:
            {
                if (m_conn_handle == BLE_CONN_HANDLE_INVALID)
                {
                    err_code = sd_ble_gap_connect(&m_peer_addr,
                                                  &m_scan_param,
                                                  &m_connection_param,
                                                  APP_IPSP_TAG);

                    if (err_code != NRF_SUCCESS)
                    {
                        APPL_LOG("Connection Request Failed, reason 0x%08lX", err_code);
                    }
                    APP_ERROR_CHECK(err_code);
                }
                else
                {
                     APPL_LOG("Connection exists with peer");
                }

                break;
            }
            case BSP_BUTTON_1:
            {
                if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
                {
                    ble_ipsp_handle_t ipsp_handle;
                    ipsp_handle.conn_handle = m_conn_handle;
                    err_code = ble_ipsp_connect(&ipsp_handle);
                    APP_ERROR_CHECK_BOOL((err_code == NRF_SUCCESS) ||
                                         (err_code == NRF_ERROR_BLE_IPSP_CHANNEL_ALREADY_EXISTS));
                }
                else
                {
                     APPL_LOG("No physical link exists with peer");
                }
                break;
            }
            case BSP_BUTTON_2:
            {
                err_code = ble_ipsp_disconnect(&m_handle);
                APPL_LOG("ble_ipsp_disconnect result %08lX", err_code);
                break;
            }
            case BSP_BUTTON_3:
            {
                if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
                {
                    err_code = sd_ble_gap_disconnect(m_conn_handle, 0x13);
                    APPL_LOG("sd_ble_gap_disconnect result %08lX", err_code);
                }
                else
                {
                    APPL_LOG("No physical link exists with peer");
                }
                break;
            }
            default:
                break;
        }
    }
}


/**@brief Function for the Button initialization.
 *
 * @details Initializes all Buttons used by this application.
 */
static void buttons_init(void)
{
    uint32_t err_code;

    static app_button_cfg_t buttons[] =
    {
        {BSP_BUTTON_0, false, BUTTON_PULL, button_event_handler},
        {BSP_BUTTON_1, false, BUTTON_PULL, button_event_handler},
        {BSP_BUTTON_2, false, BUTTON_PULL, button_event_handler},
        {BSP_BUTTON_3, false, BUTTON_PULL, button_event_handler}
    };

    err_code = app_button_init(buttons, ARRAY_SIZE(buttons), BUTTON_DETECTION_DELAY);
    APP_ERROR_CHECK(err_code);

    err_code = app_button_enable();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module.
 */
static void timers_init(void)
{
    // Initialize timer module.
    APP_ERROR_CHECK(app_timer_init());

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
            APPL_LOG("Connected, handle 0x%04X.", p_ble_evt->evt.gap_evt.conn_handle);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

            LEDS_ON(CONNECTED_LED);
            LEDS_OFF(SCANNING_LED);

            break;
        case BLE_GAP_EVT_DISCONNECTED:
            APPL_LOG("Disconnected.");

            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            LEDS_OFF(CONNECTED_LED);
            break;
        default:
            break;
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
            APPL_LOG("[0x%04X]:[0x%04X] BLE_IPSP_EVT_CHANNEL_CONNECTED Event. Result "
                     "0x%08lX", p_handle->conn_handle, p_handle->cid, p_evt->evt_result);
            if (p_evt->evt_result == NRF_SUCCESS)
            {
                m_handle = (*p_handle);
            }
            break;
        }
        case BLE_IPSP_EVT_CHANNEL_DISCONNECTED:
        {
            APPL_LOG("[0x%04X]:[0x%04X] BLE_IPSP_EVT_CHANNEL_DISCONNECTED Event. Result "
                     "0x%08lX", p_handle->conn_handle, p_handle->cid, p_evt->evt_result);
            break;
        }
        case BLE_IPSP_EVT_CHANNEL_DATA_RX:
        {
            const uint16_t received_sdu_length = MIN(p_evt->p_evt_param->p_l2cap_evt->params.rx.sdu_buf.len,
                                                     p_evt->p_evt_param->p_l2cap_evt->params.rx.sdu_len);

            APPL_LOG("[0x%04X]:[0x%04X] BLE_IPSP_EVT_CHANNEL_DATA_RX Event. Result 0x%08lX,"
                     " Data Length 0x%04X", p_handle->conn_handle, p_handle->cid,
                     p_evt->evt_result,  p_evt->p_evt_param->p_l2cap_evt->params.rx.sdu_buf.len);

            uint8_t * p_send_buffer = nrf_malloc(received_sdu_length);

            if (p_send_buffer != NULL)
            {
                // Make a copy of the buffer as this buffer is recycled once the callback returns.
                // ble_ipsp_send requires the memory to be resident until the event
                // BLE_IPSP_EVT_CHANNEL_DATA_TX_COMPLETE is notified with the buffer.
                memcpy (p_send_buffer,
                        p_evt->p_evt_param->p_l2cap_evt->params.rx.sdu_buf.p_data,
                        received_sdu_length);

                //Echo back the data received.
                uint32_t err_code = ble_ipsp_send(p_handle,
                                                  p_send_buffer,
                                                  received_sdu_length);

                APPL_LOG("[0x%04X]:[0x%04X]:[%p] Echo data back. Result %08lX",
                         p_handle->conn_handle, p_handle->cid, p_send_buffer, err_code);
            }
            else
            {
                 APPL_LOG("[0x%04X]:[0x%04X] No memory to echo data back.",
                          p_handle->conn_handle, p_handle->cid);
            }

            break;
        }
        case BLE_IPSP_EVT_CHANNEL_DATA_TX_COMPLETE:
        {
            APPL_LOG ("[0x%04X]:[0x%04X]:[%p] BLE_IPSP_EVT_CHANNEL_DATA_TX_COMPLETE Event."
                      "Result 0x%08lX\r\n", p_handle->conn_handle, p_handle->cid,
                      p_evt->p_evt_param->p_l2cap_evt->params.tx.sdu_buf.p_data, p_evt->evt_result);
            nrf_free(p_evt->p_evt_param->p_l2cap_evt->params.tx.sdu_buf.p_data);
            break;
        }
        default:
        {
            APPL_LOG("[0x%04X]:[0x%04X] Unknown Event 0x%04X. Result 0x%08lX",
                     p_evt->evt_id, p_handle->conn_handle, p_handle->cid, p_evt->evt_result);
            break;
        }
    }
    return NRF_SUCCESS;
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
        ble_cfg.gap_cfg.role_count_cfg.periph_role_count  = 0;
        ble_cfg.gap_cfg.role_count_cfg.central_role_count = BLE_IPSP_MAX_CHANNELS;
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

    NRF_SDH_BLE_OBSERVER(m_ble_evt_observer, APP_IPSP_INITIATOR_PRIO, ble_evt_dispatch, NULL);

    APP_ERROR_CHECK(err_code);
}


static void services_init()
{
    ble_ipsp_init_t init_param;
    init_param.evt_handler = app_ipsp_event_handler;

    uint32_t err_code = ble_ipsp_init(&init_param);
    APP_ERROR_CHECK(err_code);

    ble_gap_addr_t m_my_addr;

    m_my_addr.addr[5]   = 0x00;
    m_my_addr.addr[4]   = 0x11;
    m_my_addr.addr[3]   = 0x22;
    m_my_addr.addr[2]   = 0x33;
    m_my_addr.addr[1]   = 0x44;
    m_my_addr.addr[0]   = 0x55;
    m_my_addr.addr_type = BLE_GAP_ADDR_TYPE_PUBLIC;

    err_code = sd_ble_gap_addr_set(&m_my_addr);
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
 * @details Handle any pending log operation(s), then sleep until the next event occurs.
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
    timers_init();
    buttons_init();
    leds_init();
    power_management_init();
    ble_stack_init();
    services_init();

    uint32_t err_code = nrf_mem_init();
    APP_ERROR_CHECK(err_code);

    APPL_LOG("ble_app_ipsp_initiator started.");
    APPL_LOG("    Press button 1 to create physical link.");
    APPL_LOG("    Press button 2 to create IPSP link.");
    APPL_LOG("    Press button 3 to tear down IPSP link.");
    APPL_LOG("    Press button 4 to tear down physical link.");

    // Start execution.
    //connectable_mode_enter();

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }
}

/**
 * @}
 */
