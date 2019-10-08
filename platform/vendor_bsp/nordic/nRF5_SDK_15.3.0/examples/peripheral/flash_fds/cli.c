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
#include "fds.h"
#include "nrf_soc.h"
#include "sdk_config.h"
#include "fds_example.h"


#define PRINT_HELP  "print records\n"                                                             \
                    "usage: print all|config"

#define PRINT_ALL_HELP  "print all records\n"                                                     \
                        "usage: print all"

#define PRINT_CFG_HELP  "print configuration\n"                                                   \
                        "usage: print config"

#define WRITE_HELP  "write a record\n"                                                            \
                    "usage: write file_id key \"data\"\n"                                         \
                    "- file_id:\tfile ID, in HEX\n"                                               \
                    "- key:\trecord key, in HEX\n"                                                \
                    "- data:\trecord contents"

#define DELETE_HELP "delete a record\n"                                                           \
                    "usage: delete file_id key\n"                                                 \
                    "- file_id:\tfile ID, in HEX\n"                                               \
                    "- key:\trecord key, in HEX\n"                                                \

#define DELETE_ALL_HELP "delete all records\n"                                                    \
                        "usage: delete_all"                                                         \

#define UPDATE_HELP "update configuration\n"                                                      \
                    "usage: update on|off on|off boot_count device_name"                            \

#define STAT_HELP   "print statistics\n"                                                          \
                    "usage: stat"

#define GC_HELP     "run garbage collection\n"                                                    \
                    "usage: gc"


NRF_CLI_UART_DEF(cli_uart, 0, 64, 16);
NRF_CLI_DEF(m_cli_uart, "fds example:~$ ", &cli_uart.transport, '\r', 4);


/* Defined in main.c */
extern char const * fds_err_str[];


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


static void cli_wrong_param_count_help(nrf_cli_t const * p_cli, char const * p_cmd)
{
    nrf_cli_fprintf(p_cli,
                    NRF_CLI_ERROR,
                    "%s: wrong parameter count.\n"
                    "Try '%s -h' for help.\n",
                    p_cmd,
                    p_cmd);
}


static void record_write(nrf_cli_t const * p_cli,
                         uint32_t fid,
                         uint32_t key,
                         void const * p_data,
                         uint32_t len)
{
    fds_record_t const rec =
    {
        .file_id           = fid,
        .key               = key,
        .data.p_data       = p_data,
        .data.length_words = (len + 3) / sizeof(uint32_t)
    };

    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                    "writing record to flash...\n"
                    "file: 0x%x, key: 0x%x, \"%s\", len: %u bytes\n",
                    fid, key, p_data, len);

    ret_code_t rc = fds_record_write(NULL, &rec);
    if (rc != FDS_SUCCESS)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                        "error: fds_record_write() returned %s.\n",
                        fds_err_str[rc]);
    }
}


static void record_update(nrf_cli_t const * p_cli, configuration_t const * p_cfg)
{
    fds_record_desc_t desc = {0};
    fds_find_token_t  ftok = {0};

    if (fds_record_find(CONFIG_FILE, CONFIG_REC_KEY, &desc, &ftok) == FDS_SUCCESS)
    {
        fds_record_t const rec =
        {
            .file_id           = CONFIG_FILE,
            .key               = CONFIG_REC_KEY,
            .data.p_data       = p_cfg,
            .data.length_words = (sizeof(configuration_t) + 3) / sizeof(uint32_t)
        };

        ret_code_t rc = fds_record_update(&desc, &rec);
        if (rc != FDS_SUCCESS)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                            "error: fds_record_update() returned %s.\n",
                            fds_err_str[rc]);
        }
    }
    else
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "error: could not find config file.\n");
    }
}


static void record_delete(nrf_cli_t const * p_cli, uint32_t fid, uint32_t key)
{
    fds_find_token_t tok   = {0};
    fds_record_desc_t desc = {0};

    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                    "deleting record...\n"
                    "file: 0x%x, key: 0x%x\n",
                    fid,
                    key);

    if (fds_record_find(fid, key, &desc, &tok) == FDS_SUCCESS)
    {
        ret_code_t rc = fds_record_delete(&desc);
        if (rc != FDS_SUCCESS)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                            "error: fds_record_delete() returned %s.\n", fds_err_str[rc]);

            return;
        }

        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "record id: 0x%x\n", desc.record_id);
    }
    else
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "error: record not found!\n");
    }
}


bool record_delete_next(void)
{
    fds_find_token_t  tok   = {0};
    fds_record_desc_t desc  = {0};

    if (fds_record_iterate(&desc, &tok) == FDS_SUCCESS)
    {
        ret_code_t rc = fds_record_delete(&desc);
        if (rc != FDS_SUCCESS)
        {
            return false;
        }

        return true;
    }
    else
    {
        /* No records left to delete. */
        return false;
    }
}


static void print_cmd(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else if (argc != 2)
    {
        cli_wrong_param_count_help(p_cli, "print");
    }
    else
    {
        cli_unknown_param_help(p_cli, argv[1], "print");
    }
}


static void print_cfg_cmd(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    fds_record_desc_t desc = {0};
    fds_find_token_t  tok  = {0};

    while (fds_record_find(CONFIG_FILE, CONFIG_REC_KEY, &desc, &tok) == FDS_SUCCESS)
    {
        ret_code_t rc;
        fds_flash_record_t frec = {0};

        rc = fds_record_open(&desc, &frec);
        switch (rc)
        {
            case FDS_SUCCESS:
                break;

            case FDS_ERR_CRC_CHECK_FAILED:
                nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "error: CRC check failed!\n");
                continue;

            case FDS_ERR_NOT_FOUND:
                nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "error: record not found!\n");
                continue;

            default:
            {
                nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                                "error: unexpecte error %s.\n",
                                fds_err_str[rc]);

                continue;
            }
        }

        configuration_t * p_cfg = (configuration_t *)(frec.p_data);

        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                        "config1:\t%s\n"
                        "config2:\t%s\n"
                        "boot count:\t%u\n"
                        "device name:\t%s\n",
                        p_cfg->config1_on ? "on" : "off",
                        p_cfg->config2_on ? "on" : "off",
                        p_cfg->boot_count,
                        p_cfg->device_name);

        rc = fds_record_close(&desc);
        APP_ERROR_CHECK(rc);
    }
}


static void print_all_cmd(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    fds_find_token_t tok   = {0};
    fds_record_desc_t desc = {0};

    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                    "rec. id\t"
                    "\tfile id\t"
                    "\trec. key"
                    "\tlength\n");

    while (fds_record_iterate(&desc, &tok) != FDS_ERR_NOT_FOUND)
    {
        ret_code_t rc;
        fds_flash_record_t frec = {0};

        rc = fds_record_open(&desc, &frec);
        switch (rc)
        {
            case FDS_SUCCESS:
                break;

            case FDS_ERR_CRC_CHECK_FAILED:
                nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "error: CRC check failed!\n");
                continue;

            case FDS_ERR_NOT_FOUND:
                nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "error: record not found!\n");
                continue;

            default:
            {
                nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                                "error: unexpecte error %s.\n",
                                fds_err_str[rc]);

                continue;
            }
        }

        uint32_t const len = frec.p_header->length_words * sizeof(uint32_t);

        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                        " 0x%04x\t"
                        "\t 0x%04x\t"
                        "\t 0x%04x\t"
                        "\t %4u bytes\n",
                        frec.p_header->record_id,
                        frec.p_header->file_id,
                        frec.p_header->record_key,
                        len);

        rc = fds_record_close(&desc);
        APP_ERROR_CHECK(rc);
    }
}


static void write_cmd(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{

    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else if (argc != 4)
    {
        cli_wrong_param_count_help(p_cli, "write");
    }
    else
    {
        /* Must be statically allocated, because it is going to be written in flash. */
        static uint8_t m_data[256];

        uint16_t const fid  = strtol(argv[1], NULL, 16);
        uint16_t const key  = strtol(argv[2], NULL, 16);

        uint32_t const len  = strlen(argv[3]) < sizeof(m_data) ?
                              strlen(argv[3]) : sizeof(m_data);

        /* Copy data to static variable. */
        memset(m_data, 0x00, sizeof(m_data));
        memcpy(m_data, argv[3], len);

        record_write(p_cli, fid, key, m_data, len);
    }
}


static void update_cmd(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else if (argc != 5)
    {
        cli_wrong_param_count_help(p_cli, "update");
    }
    else
    {
        /* Must be statically allocated, because it is going to be written in flash. */
        static configuration_t cfg = {0};

        memset(&cfg, 0x00, sizeof(configuration_t));

        cfg.config1_on = !strcmp(argv[1], "on");
        cfg.config2_on = !strcmp(argv[2], "on");
        cfg.boot_count = strtol(argv[3], NULL, 10);

        uint16_t const len = strlen(argv[4]) < sizeof(cfg.device_name) ?
                             strlen(argv[4]) : sizeof(cfg.device_name);

        memcpy(cfg.device_name, argv[4], len);

        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
            "updating configuration: %s, %s, boot_count=%u, device_name=\"%s\"\n",
            cfg.config1_on ? "on" : "off",
            cfg.config2_on ? "on" : "off",
            cfg.boot_count,
            cfg.device_name);

        record_update(p_cli, &cfg);
    }
}


static void delete_cmd(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else if (argc < 3)
    {
        cli_wrong_param_count_help(p_cli, "delete");
    }
    else
    {
        uint32_t const fid  = strtol(argv[1], NULL, 16);
        uint32_t const key  = strtol(argv[2], NULL, 16);

        record_delete(p_cli, fid, key);
    }
}


static void delete_all_cmd(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else if (argc != 1)
    {
        cli_wrong_param_count_help(p_cli, "delete_all");
    }
    else
    {
        delete_all_begin();
    }
}


static void stat_cmd(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else
    {
        /* Print fds_stat(). */
        fds_stat_t stat = {0};

        ret_code_t rc = fds_stat(&stat);
        APP_ERROR_CHECK(rc);

        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                        "total pages:\t%u\n"
                        "total records:\t%u\n"
                        "valid records:\t%u\n"
                        "dirty records:\t%u\n"
                        "largest contig:\t%u\n"
                        "freeable words:\t%u (%u bytes)\n",
                        stat.pages_available,
                        stat.valid_records + stat.dirty_records,
                        stat.valid_records,
                        stat.dirty_records,
                        stat.largest_contig,
                        stat.freeable_words,
                        stat.freeable_words * sizeof(uint32_t));
    }
}


static void gc_cmd(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t rc = fds_gc();
    switch (rc)
    {
        case FDS_SUCCESS:
            break;

        default:
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                            "error: garbage collection returned %s\n", fds_err_str[rc]);
            break;
    }
}


NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_print)
{
    NRF_CLI_CMD(all,    NULL, PRINT_ALL_HELP, print_all_cmd),
    NRF_CLI_CMD(config, NULL, PRINT_CFG_HELP, print_cfg_cmd),
    NRF_CLI_SUBCMD_SET_END
};


NRF_CLI_CMD_REGISTER(print,      &m_print, PRINT_HELP,      print_cmd);
NRF_CLI_CMD_REGISTER(write,      NULL,     WRITE_HELP,      write_cmd);
NRF_CLI_CMD_REGISTER(update,     NULL,     UPDATE_HELP,     update_cmd);
NRF_CLI_CMD_REGISTER(delete,     NULL,     DELETE_HELP,     delete_cmd);
NRF_CLI_CMD_REGISTER(delete_all, NULL,     DELETE_ALL_HELP, delete_all_cmd);
NRF_CLI_CMD_REGISTER(gc,         NULL,     GC_HELP,         gc_cmd);
NRF_CLI_CMD_REGISTER(stat,       NULL,     STAT_HELP,       stat_cmd);
