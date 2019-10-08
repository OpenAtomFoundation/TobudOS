/**
 * Copyright (c) 2016 - 2019 Nordic Semiconductor ASA and Luxoft Global Operations Gmbh.
 *
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
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
#include "config.h"

#include "fsm.h"
#include "uart.h"
#include "sys_debug.h"
#include "sys_utils.h"
#include "boards.h"


static char m_tx_buffer[CONFIG_UART_TX_BUFFER_SIZE];
static char m_rx_buffer[CONFIG_UART_RX_BUFFER_SIZE];
static hal_uart_config_t m_uart_config =
{
    .module         = CONFIG_UART_MODULE,
    .tx_pin         = CONFIG_UART_TX_PIN,
    .rx_pin         = CONFIG_UART_RX_PIN,
    .cts_pin        = CONFIG_UART_CTS_PIN,
    .rts_pin        = CONFIG_UART_RTS_PIN,
    .baudrate       = CONFIG_UART_BAUDRATE,
    .parity         = CONFIG_UART_PARITY,
    .char_size      = HAL_UART_EIGHT_BITS_CHAR,
    .flow_control   = HAL_UART_FLOW_CONTROL_ENABLED,
};

static void uart_read_handler(uint32_t channel, size_t char_count);
static void uart_write_handler(uint32_t channel, size_t char_count);
static void uart_error_handler(uint32_t channel, hal_uart_error_t error_description);

hal_uart_descriptor_t g_uart_descr =
{
    .write_handler     = uart_write_handler,
    .write_buffer_ptr  = m_tx_buffer,
    .write_buffer_size = ARRAY_SIZE(m_tx_buffer),
    .read_handler      = uart_read_handler,
    .read_buffer_ptr   = m_rx_buffer,
    .read_buffer_size  = ARRAY_SIZE(m_rx_buffer),
    .error_handler     = uart_error_handler,
};


void uart_init(void)
{
    g_uart_descr.uart_config = m_uart_config;

    hal_uart_open(&m_uart_config, &g_uart_descr);
}

static void uart_read_handler(uint32_t channel, size_t char_count)
{
    fsm_event_post(E_UART_RX_DONE, NULL);
}

static void uart_write_handler(uint32_t channel, size_t char_count)
{
    fsm_event_post(E_UART_TX_DONE, NULL);
    LEDS_OFF(BIT(CONFIG_DOWNSTREAM_PIN));
}

static void uart_error_handler(uint32_t channel, hal_uart_error_t error_description)
{
    LEDS_ON(BIT(CONFIG_ERROR_PIN));
    ASSERT_INFO(error_description == HAL_UART_ERROR_NONE,
                "Unexpected error code %d\n",
                error_description);
    ASSERT(false);
}

