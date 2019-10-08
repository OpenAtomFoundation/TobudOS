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

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <ctype.h>


#include "nrf.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

#include "nrf_drv_power.h"
#include "app_timer.h"

#include "app_error.h"
#include "app_util.h"

#include "nrf_cli.h"
#include "nrf_cli_libuarte.h"
#include "nrf_cli_rtt.h"
#include "nrf_crypto.h"

#include "boards.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "vars.h"
#include "raw.h"
#include "cli_utils.h"

NRF_CLI_LIBUARTE_DEF(m_cli_uart_transport, 256, 256);
NRF_CLI_DEF(m_cli_uart,
            "nrf_crypto$ ",
            &m_cli_uart_transport.transport,
            '\r',
            8);

NRF_CLI_RTT_DEF(m_cli_rtt_transport);
NRF_CLI_DEF(m_cli_rtt,
            "nrf_crypto$ ",
            &m_cli_rtt_transport.transport,
            '\n',
            8);


static void cli_init(void)
{
    ret_code_t            ret;
    cli_libuarte_config_t libuarte_config;

    libuarte_config.tx_pin   = TX_PIN_NUMBER;
    libuarte_config.rx_pin   = RX_PIN_NUMBER;
    libuarte_config.baudrate = NRF_UARTE_BAUDRATE_115200;
    libuarte_config.parity   = NRF_UARTE_PARITY_EXCLUDED;
    libuarte_config.hwfc     = NRF_UARTE_HWFC_DISABLED;

    ret = nrf_cli_init(&m_cli_uart, &libuarte_config, true, true, NRF_LOG_SEVERITY_INFO);
    APP_ERROR_CHECK(ret);

    ret = nrf_cli_init(&m_cli_rtt, NULL, true, true, NRF_LOG_SEVERITY_INFO);
    APP_ERROR_CHECK(ret);
}


static void cli_start(void)
{
    ret_code_t ret;
    ret = nrf_cli_start(&m_cli_uart);
    APP_ERROR_CHECK(ret);
    ret = nrf_cli_start(&m_cli_rtt);
    APP_ERROR_CHECK(ret);
}


int main(void)
{
    ret_code_t ret;

    ret = NRF_LOG_INIT(app_timer_cnt_get);
    APP_ERROR_CHECK(ret);

    ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret);
    nrf_drv_clock_lfclk_request(NULL);

    ret = app_timer_init();
    APP_ERROR_CHECK(ret);

    cli_init();

    ret = nrf_crypto_init();
    APP_ERROR_CHECK(ret);

    var_init();

    NRF_LOG_RAW_INFO("Command Line Interface for nrf_crypto started.\n");
    NRF_LOG_RAW_INFO("Press Tab to see all available commands.\n");

    cli_start();

    while (true)
    {
        UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
        nrf_cli_process(&m_cli_rtt);
        nrf_cli_process(&m_cli_uart);
    }
}

