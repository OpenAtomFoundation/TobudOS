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

/**@brief BLE interactive app example application main file.
 *
 * @detail This application demonstrates how to control a BLE connection using the command line interface.
 *
 * @note This application requires the use of an external ECC library
 *       for public key and shared secret calculation.
 *       Refer to the application's documentation for more details.
 *
 */

#include "sdk_config.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <ctype.h>
#include "nordic_common.h"
#include "app_timer.h"

#include "nrf_pwr_mgmt.h"

#include "nrf_cli.h"
#include "nrf_cli_uart.h"
#include "nrf_cli_rtt.h"

#include "boards.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_ble_lesc.h"

#include "ble_m.h"
#include "cli_m.h"
#include "pm_m.h"
#include "nfc_m.h"
#include "nfc_central_m.h"

#define FOUND_DEVICE_REFRESH_TIME APP_TIMER_TICKS(SCAN_LIST_REFRESH_INTERVAL) /**< Time after which the device list is clean and refreshed. */


/**@brief Command line interface instance.
 */
NRF_CLI_UART_DEF(m_cli_uart_transport, 0, 64, 16);
NRF_CLI_DEF(m_cli_uart, "uart_cli:~$ ", &m_cli_uart_transport.transport, '\r', CLI_LOG_QUEUE_SIZE);


/**@brief Function for starting a command line interface working on the UART transport layer.
 */
static void cli_uart_start(void)
{
    ret_code_t ret;

    ret = nrf_cli_start(&m_cli_uart);
    APP_ERROR_CHECK(ret);
}


#if NRF_MODULE_ENABLED(CLI_RTT)
NRF_CLI_RTT_DEF(m_cli_rtt_transport);
NRF_CLI_DEF(m_cli_rtt, "rtt_cli:~$ ", &m_cli_rtt_transport.transport, '\n', CLI_LOG_QUEUE_SIZE);

/**@brief Function for starting a command line interface working on the RTT transport layer.
 */
static void cli_rtt_start(void)
{
    ret_code_t ret;

    ret = nrf_cli_init(&m_cli_rtt, NULL, true, true, NRF_LOG_SEVERITY_INFO);
    APP_ERROR_CHECK(ret);

    ret = nrf_cli_start(&m_cli_rtt);
    APP_ERROR_CHECK(ret);
}


#endif // CLI_RTT

/**@brief Function for configuring UART for CLI.
 */
static void cli_uart_cfg(void)
{
    ret_code_t            err_code;
    nrf_drv_uart_config_t uart_config = NRF_DRV_UART_DEFAULT_CONFIG;

    uart_config.pseltxd = TX_PIN_NUMBER;
    uart_config.pselrxd = RX_PIN_NUMBER;
    uart_config.hwfc    = NRF_UART_HWFC_DISABLED;
    err_code            = nrf_cli_init(&m_cli_uart, &uart_config, true, true, NRF_LOG_SEVERITY_INFO);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the adv_list_timer event, which refreshes the connectable devices.
 */
static void adv_list_timer_handle(void * p_context)
{
    if (is_scanning())
    {
        connect_addr_clear();
        scan_device_info_clear();
    }
}


/**@brief Function for initializing logging.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(app_timer_cnt_get);

    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the timer.
 */
static void timer_init(void)
{
    ret_code_t err_code = app_timer_init();

    APP_ERROR_CHECK(err_code);

    // Timer for refreshing scanned devices data.
    APP_TIMER_DEF(adv_list_timer);
    err_code = app_timer_create(&adv_list_timer, APP_TIMER_MODE_REPEATED, adv_list_timer_handle);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(adv_list_timer, FOUND_DEVICE_REFRESH_TIME, NULL);
    APP_ERROR_CHECK(err_code);
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
static void idle_state_handler(void)
{
    ret_code_t err_code;
    err_code = nrf_ble_lesc_request_handler();
    APP_ERROR_CHECK(err_code);
    
#if defined(ADAFRUIT_SHIELD) && (ADAFRUIT_SHIELD == 1)
    nfc_tag_process();
#endif // ADAFRUIT_SHIELD

    nrf_cli_process(&m_cli_uart);
    
#if NRF_MODULE_ENABLED(CLI_RTT)
    nrf_cli_process(&m_cli_rtt);
#endif // CLI_RTT
    
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}


int main(void)
{
    power_management_init();
    log_init();
    timer_init();
    // Console start
    cli_uart_cfg();
    cli_uart_start();
#if NRF_MODULE_ENABLED(CLI_RTT)
    cli_rtt_start();
#endif // CLI_RTT
    ble_m_init();
    peer_manager_init();
    nfc_pairing_init();
#if NRF_MODULE_ENABLED(ADAFRUIT_SHIELD)
    nfc_central_init();
#endif // ADAFRUIT_SHIELD
    bond_get();

    NRF_LOG_RAW_INFO("BLE app with command line interface example started.\r\n");
    NRF_LOG_RAW_INFO("Press Tab to view all available commands.\r\n");

    for (;;)
    {
        idle_state_handler();
    }
}


