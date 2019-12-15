/**
 * Copyright (c) 2014 - 2019, Nordic Semiconductor ASA
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
 * @defgroup flashwrite_example_main main.c
 * @{
 * @ingroup flashwrite_example
 *
 * @brief This file contains the source code for a sample application using the Flash Write Application.
 *a
 */

#include <stdbool.h>
#include <stdio.h>
#include "nrf.h"
#include "bsp.h"
#include "app_error.h"
#include "nrf_nvmc.h"
#include "nordic_common.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "app_timer.h"
#include "nrf_drv_clock.h"

#include "nrf_cli.h"
#include "nrf_cli_uart.h"

#define FLASHWRITE_EXAMPLE_MAX_STRING_LEN       (62u)
#define FLASHWRITE_EXAMPLE_BLOCK_VALID          (0xA55A5AA5)
#define FLASHWRITE_EXAMPLE_BLOCK_INVALID        (0xA55A0000)
#define FLASHWRITE_EXAMPLE_BLOCK_NOT_INIT       (0xFFFFFFFF)

NRF_CLI_UART_DEF(m_cli_uart_transport, 0, 64, 16);
NRF_CLI_DEF(m_cli_uart, "uart_cli:~$ ", &m_cli_uart_transport.transport, '\r', 4);

typedef struct
{
   uint32_t magic_number;
   uint32_t buffer[FLASHWRITE_EXAMPLE_MAX_STRING_LEN + 1]; // + 1 for end of string
} flashwrite_example_flash_data_t;

typedef struct
{
    uint32_t addr;
    uint32_t pg_size;
    uint32_t pg_num;
    flashwrite_example_flash_data_t * m_p_flash_data;
} flashwrite_example_data_t;

static flashwrite_example_data_t m_data;

static ret_code_t clock_config(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_clock_init();
    if (err_code != NRF_SUCCESS && err_code != NRF_ERROR_MODULE_ALREADY_INITIALIZED)
    {
        return err_code;
    }

    nrf_drv_clock_lfclk_request(NULL);

    return NRF_SUCCESS;
}

static void flash_page_init(void)
{
    m_data.pg_num = NRF_FICR->CODESIZE - 1;
    m_data.pg_size = NRF_FICR->CODEPAGESIZE;
    m_data.addr = (m_data.pg_num * m_data.pg_size);

    m_data.m_p_flash_data = (flashwrite_example_flash_data_t *)m_data.addr;

    while (1)
    {
        if (m_data.m_p_flash_data->magic_number == FLASHWRITE_EXAMPLE_BLOCK_VALID)
        {
            return;
        }

        if (m_data.m_p_flash_data->magic_number == FLASHWRITE_EXAMPLE_BLOCK_INVALID)
        {
            ++m_data.m_p_flash_data;
            continue;
        }

        nrf_nvmc_page_erase(m_data.addr);
        return;
    }
}
/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));

    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = clock_config();
    APP_ERROR_CHECK(err_code);

    flash_page_init();

    nrf_drv_uart_config_t uart_config = NRF_DRV_UART_DEFAULT_CONFIG;
    uart_config.pseltxd = TX_PIN_NUMBER;
    uart_config.pselrxd = RX_PIN_NUMBER;
    uart_config.hwfc    = NRF_UART_HWFC_DISABLED;
    err_code = nrf_cli_init(&m_cli_uart, &uart_config, true, true, NRF_LOG_SEVERITY_INFO);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_cli_start(&m_cli_uart);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_RAW_INFO("Flashwrite example started.\r\n");
    NRF_LOG_RAW_INFO("Execute: <flash -h> for more information "
                     "or press the Tab button to see all available commands.\r\n");

    while (true)
    {
        UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
        nrf_cli_process(&m_cli_uart);
    }
}

static void flash_string_write(uint32_t address, const char * src, uint32_t num_words)
{
    uint32_t i;

    // Enable write.
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
    }

    for (i = 0; i < num_words; i++)
    {
        /* Only full 32-bit words can be written to Flash. */
        ((uint32_t*)address)[i] = 0x000000FFUL & (uint32_t)((uint8_t)src[i]);
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
        {
        }
    }

    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
    }
}


static void flashwrite_erase_cmd(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    nrf_nvmc_page_erase(m_data.addr);

    m_data.m_p_flash_data = (flashwrite_example_flash_data_t *)m_data.addr;
}

static void flashwrite_read_cmd(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    flashwrite_example_flash_data_t * p_data = (flashwrite_example_flash_data_t *)m_data.addr;
    char string_buff[FLASHWRITE_EXAMPLE_MAX_STRING_LEN + 1]; // + 1 for end of string

    if ((p_data == m_data.m_p_flash_data) &&
        (p_data->magic_number != FLASHWRITE_EXAMPLE_BLOCK_VALID))
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_WARNING, "Please write something first.\r\n");
        return;
    }

    while (p_data <= m_data.m_p_flash_data)
    {
        if ((p_data->magic_number != FLASHWRITE_EXAMPLE_BLOCK_VALID) &&
            (p_data->magic_number != FLASHWRITE_EXAMPLE_BLOCK_INVALID))
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_WARNING, "Corrupted data found.\r\n");
            return;
        }
        uint8_t i;
        for (i = 0 ; i <= FLASHWRITE_EXAMPLE_MAX_STRING_LEN; i++)
        {
            string_buff[i] = (char)p_data->buffer[i];
        }

        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%s\r\n", string_buff);
        ++p_data;
    }
}

static void flashwrite_write_cmd(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    static uint16_t const page_size = 4096;

    if (argc < 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s:%s", argv[0], " bad parameter count\r\n");
        return;
    }
    if (argc > 2)
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_WARNING,
                        "%s:%s",
                        argv[0],
                        " bad parameter count - please use quotes\r\n");
        return;
    }

    uint32_t len = strlen(argv[1]);
    if (len > FLASHWRITE_EXAMPLE_MAX_STRING_LEN)
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "Too long string. Please limit entered string to %d chars.\r\n",
                        FLASHWRITE_EXAMPLE_MAX_STRING_LEN);
        return;
    }

    if ((m_data.m_p_flash_data->magic_number != FLASHWRITE_EXAMPLE_BLOCK_NOT_INIT) &&
        (m_data.m_p_flash_data->magic_number != FLASHWRITE_EXAMPLE_BLOCK_VALID))
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Flash corrupted, please errase it first.");
        return;
    }

    if (m_data.m_p_flash_data->magic_number == FLASHWRITE_EXAMPLE_BLOCK_VALID)
    {
        uint32_t new_end_addr = (uint32_t)(m_data.m_p_flash_data + 2);
        uint32_t diff = new_end_addr - m_data.addr;
        if (diff > page_size)
        {
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_WARNING,
                            "Not enough space - please erase flash first.\r\n");
            return;
        }
        nrf_nvmc_write_word((uint32_t)&m_data.m_p_flash_data->magic_number,
                            FLASHWRITE_EXAMPLE_BLOCK_INVALID);
        ++m_data.m_p_flash_data;
    }

    //++len -> store also end of string '\0'
    flash_string_write((uint32_t)m_data.m_p_flash_data->buffer, argv[1], ++len);
    nrf_nvmc_write_word((uint32_t)&m_data.m_p_flash_data->magic_number,
                        FLASHWRITE_EXAMPLE_BLOCK_VALID);
}

static void flashwrite_cmd(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s:%s%s\r\n", argv[0], " unknown parameter: ", argv[1]);
}

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_flash)
{
    NRF_CLI_CMD(erase, NULL, "Erase flash.",          flashwrite_erase_cmd),
    NRF_CLI_CMD(read,  NULL, "Read data from flash.", flashwrite_read_cmd),
    NRF_CLI_CMD(write, NULL, "Write data to flash.\n"
                             "Limitations:\n"
                             "- maximum 16 entries,\n"
                             "- each entry is maximum 62 chars long.",
                                                      flashwrite_write_cmd),
    NRF_CLI_SUBCMD_SET_END
};
NRF_CLI_CMD_REGISTER(flash, &m_sub_flash, "Flash access command.", flashwrite_cmd);

/** @} */
