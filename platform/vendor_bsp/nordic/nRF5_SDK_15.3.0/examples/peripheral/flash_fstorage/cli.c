/**
 * Copyright (c) 2017 - 2019, Nordic Semiconductor ASA
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

#include <string.h>

#include "app_error.h"
#include "boards.h"
#include "nrf_cli.h"
#include "nrf_cli_uart.h"
#include "nrf_drv_uart.h"
#include "nrf_fstorage.h"
#include "nrf_soc.h"
#include "nrf_strerror.h"
#include "sdk_config.h"


#define READ_HELP   "read bytes from flash\n"                                                     \
                    "usage: read hex|str addr len"

#define READ_HEX_HELP   "read bytes from flash in HEX format\n"                                   \
                        "usage: read hex addr len\n"                                              \
                        "- addr: the address to read from, in HEX\n"                              \
                        "- len: number of bytes to read"

#define READ_STR_HELP   "read bytes from flash in ASCII format\n"                                 \
                        "usage: read str addr len\n"                                              \
                        "- addr: the address to read from, in HEX\n"                              \
                        "- len: number of bytes to read"

#define WRITE_HELP  "write ASCII bytes to flash\n"                                                \
                    "usage: write addr data\n"                                                    \
                    "- addr: address to write to, in HEX\n"                                       \
                    "- data: data to be written; use quotes for strings"

#define ERASE_HELP  "erase flash pages\n"                                                         \
                    "usage: erase addr pages\n"                                                   \
                    "- addr: address of the page to begin erasing from, in HEX\n"                 \
                    "- pages: number of pages to erase"

#define FLASHAREA_HELP  "print or set the boundaries of the flash\n"                              \
                        "usage: flasharea print\n"                                                \
                        "usage: flasharea set begin end"

#define FLASHAREA_PRINT_HELP    "print the boundaries of the flash\n"                             \
                                "usage: flasharea print"

#define FLASHAREA_SET_HELP  "set the boundaries of the flash\n"                                   \
                            "usage: flasharea set begin end\n"                                    \
                            "- begin: address of the beginning of the flash area, in HEX\n"       \
                            "- end: address of end of the flash area, in HEX"


typedef enum
{
    DATA_FMT_HEX = 'h',
    DATA_FMT_STR = 's'
} data_fmt_t;


extern nrf_fstorage_t fstorage;                                                                     /**< The fstorage instance, defined in main.c. */
extern void wait_for_flash_ready(nrf_fstorage_t const *);                                           /**< Wait for flash operations to complete. Defined in main.c */


NRF_CLI_UART_DEF(cli_uart, 0, 64, 128);
NRF_CLI_DEF(m_cli_uart, "fstorage example:~$ ", &cli_uart.transport, '\r', 4);


void cli_init(void)
{
    nrf_drv_uart_config_t uart_config = NRF_DRV_UART_DEFAULT_CONFIG;
    uart_config.pseltxd               = TX_PIN_NUMBER;
    uart_config.pselrxd               = RX_PIN_NUMBER;
    uart_config.hwfc                  = NRF_UART_HWFC_DISABLED;

    ret_code_t rc = nrf_cli_init(&m_cli_uart, &uart_config, true, true, NRF_LOG_SEVERITY_INFO);
    APP_ERROR_CHECK(rc);
}


void cli_start(void)
{
    ret_code_t rc = nrf_cli_start(&m_cli_uart);
    APP_ERROR_CHECK(rc);
}


void cli_process(void)
{
    nrf_cli_process(&m_cli_uart);
}


static void cli_unknown_param_help(nrf_cli_t const * p_cli,
                                   char const * p_param,
                                   char const * p_cmd)
{
    nrf_cli_fprintf(p_cli,
                    NRF_CLI_ERROR,
                    "%s: unknown parameter '%s'\n"
                    "Try '%s -h' for help.\n",
                    p_cmd,
                    p_param,
                    p_cmd);
}


static void cli_missing_param_help(nrf_cli_t const * p_cli, char const * p_cmd)
{
    nrf_cli_fprintf(p_cli,
                    NRF_CLI_ERROR,
                    "%s: missing parameter.\n"
                    "Try '%s -h' for help.\n",
                    p_cmd,
                    p_cmd);
}


static void fstorage_read(nrf_cli_t const * p_cli, uint32_t addr, uint32_t len, data_fmt_t fmt)
{
    ret_code_t rc;
    uint8_t    data[256] = {0};

    if (len > sizeof(data))
    {
        len = sizeof(data);
    }

    /* Read data. */
    rc = nrf_fstorage_read(&fstorage, addr, data, len);
    if (rc != NRF_SUCCESS)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "nrf_fstorage_read() returned: %s\n",
                        nrf_strerror_get(rc));
        return;
    }

    switch (fmt)
    {
        case DATA_FMT_HEX:
        {
            /* Print bytes. */
            for (uint32_t i = 0; i < len; i++)
            {
                nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "0x%x ", data[i]);
            }
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\n");
        } break;

        case DATA_FMT_STR:
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%s\n", data);
        } break;

        default:
            break;
    }
}


static uint32_t round_up_u32(uint32_t len)
{
    if (len % sizeof(uint32_t))
    {
        return (len + sizeof(uint32_t) - (len % sizeof(uint32_t)));
    }

    return len;
}


static void fstorage_write(nrf_cli_t const * p_cli, uint32_t addr, void const * p_data)
{
    /* The following code snippet make sure that the length of the data we are writing to flash
     * is a multiple of the program unit of the flash peripheral (4 bytes).
     *
     * In case of non-string piece of data, use the sizeof operator instead of strlen.
     */
    uint32_t len = round_up_u32(strlen(p_data));

    ret_code_t rc = nrf_fstorage_write(&fstorage, addr, p_data, len, NULL);
    if (rc != NRF_SUCCESS)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "nrf_fstorage_write() returned: %s\n",
                        nrf_strerror_get(rc));
    }
}


static void fstorage_erase(nrf_cli_t const * p_cli, uint32_t addr, uint32_t pages_cnt)
{
    ret_code_t rc = nrf_fstorage_erase(&fstorage, addr, pages_cnt, NULL);
    if (rc != NRF_SUCCESS)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "nrf_fstorage_erase() returned: %s\n",
                        nrf_strerror_get(rc));
    }
}


static void read_cmd(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else if (argc == 1)
    {
        cli_missing_param_help(p_cli, "read");
    }
    else
    {
        cli_unknown_param_help(p_cli, argv[1], "read");
    }
}


static void read_cmd_hex(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else if (argc != 3)
    {
        cli_missing_param_help(p_cli, "read hex");
    }
    else
    {
        uint32_t const addr = strtol(argv[1], NULL, 16);
        uint32_t const len  = strtol(argv[2], NULL, 10);

        fstorage_read(p_cli, addr, len, DATA_FMT_HEX);
    }
}


static void read_cmd_str(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else if (argc != 3)
    {
        cli_missing_param_help(p_cli, "read str");
    }
    else
    {
        uint32_t const addr = strtol(argv[1], NULL, 16);
        uint32_t const len  = strtol(argv[2], NULL, 10);

        fstorage_read(p_cli, addr, len, DATA_FMT_STR);
    }
}


static void write_cmd(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    static uint8_t m_data[256];

    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else if (argc != 3)
    {
        cli_missing_param_help(p_cli, "write");
    }
    else
    {
        uint32_t const addr = strtol(argv[1], NULL, 16);
        uint32_t const len  = strlen(argv[2]) < sizeof(m_data) ?
                              strlen(argv[2]) : sizeof(m_data);

        /* Copy data to a static variable. */
        memset(m_data, 0x00, sizeof(m_data));
        memcpy(m_data, argv[2], len);

        fstorage_write(p_cli, addr, m_data);
    }

}


static void erase_cmd(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else if (argc != 3)
    {
        cli_missing_param_help(p_cli, argv[0]);
    }
    else
    {
        uint32_t const addr      = strtol(argv[1], NULL, 16);
        uint32_t const pages_cnt = strtol(argv[2], NULL, 10);

        fstorage_erase(p_cli, addr, pages_cnt);
    }
}


static void flasharea_cmd(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else if (argc == 1)
    {
        cli_missing_param_help(p_cli, "flasharea");
    }
    else
    {
        cli_unknown_param_help(p_cli, argv[1], "flasharea");
    }
}


static void flasharea_cmd_print(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "begin: %x, end: %x\n",
                        fstorage.start_addr, fstorage.end_addr);
    }
}


static void flasharea_cmd_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else if (argc != 3)
    {
        cli_missing_param_help(p_cli, "flasharea set");
    }
    else
    {
        fstorage.start_addr = strtol(argv[1], NULL, 16);
        fstorage.end_addr   = strtol(argv[2], NULL, 16);

        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "begin: %x, end: %x\n",
                        fstorage.start_addr, fstorage.end_addr);
    }
}


NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_read_cmd)
{
    NRF_CLI_CMD(hex, NULL, READ_HEX_HELP, read_cmd_hex),
    NRF_CLI_CMD(str, NULL, READ_STR_HELP, read_cmd_str),
    NRF_CLI_SUBCMD_SET_END
};


NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_flasharea_cmd)
{
    NRF_CLI_CMD(print, NULL, FLASHAREA_PRINT_HELP, flasharea_cmd_print),
    NRF_CLI_CMD(set,   NULL, FLASHAREA_SET_HELP,   flasharea_cmd_set),
    NRF_CLI_SUBCMD_SET_END
};


NRF_CLI_CMD_REGISTER(read,      &m_read_cmd,        READ_HELP,      read_cmd);
NRF_CLI_CMD_REGISTER(write,     NULL,               WRITE_HELP,     write_cmd);
NRF_CLI_CMD_REGISTER(erase,     NULL,               ERASE_HELP,     erase_cmd);
NRF_CLI_CMD_REGISTER(flasharea, &m_flasharea_cmd,   FLASHAREA_HELP, flasharea_cmd);
