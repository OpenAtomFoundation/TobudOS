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

#include "nrf_cli.h"

#include "cli_utils.h"
#include "raw.h"


/* =============================================================================================
 *                               Raw utility functions
 * =============================================================================================
 */


static void print_hex(uint8_t * p_data, size_t size);


/** @brief Callback function to print hex data using currently selected format.
 *  @param p_data  Pointer to data to print.
 *  @param size    Number of bytes to print.
 */
typedef void (*print_fn_t)(uint8_t * p_data, size_t size);


/** @brief Function that prints hex in currently selected format.
 */
print_fn_t m_print_fn = print_hex;


/** @brief Prints in raw hex format. @see print_fn_t.
 */
static void print_hex(uint8_t * p_data, size_t size)
{
    nrf_cli_t const * p_cli = cli_get();
    size_t            i;
    size_t            col = 0;
    for (i = 0; i < size; i++)
    {
        col = i & 0xF;
        if (col == 0)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "    ");
        }
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_NORMAL,
                        "%02X%s",
                        p_data[i],
                        (col < 0xF ? " " : ""));
        if (col == 0xF)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\n");
        }
    }
    if (col != 0xF)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\n");
    }
}


/** @brief Prints in C format. @see print_fn_t.
 */
static void print_c(uint8_t * p_data, size_t size)
{
    nrf_cli_t const * p_cli = cli_get();
    size_t            i;
    size_t            col = 0;
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "{\n");
    for (i = 0; i < size; i++)
    {
        col = i & 0xF;
        if (col == 0)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "    ");
        }
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_NORMAL,
                        "0x%02X,%s",
                        p_data[i],
                        (col < 0xF ? " " : ""));
        if (col == 0xF)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\n");
        }
    }
    if (col != 0xF)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\n");
    }
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "}\n");
}


/** @brief Prints in CLI format. @see print_fn_t.
 */
static void print_cli(uint8_t * p_data, size_t size)
{
    nrf_cli_t const * p_cli = cli_get();
    size_t            i;
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\"");
    for (i = 0; i < size; i++)
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_NORMAL,
                        "%02X%s",
                        p_data[i],
                        (i < (size - 1) ? " " : ""));
    }
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\"\n");
}


void raw_print_hex(uint8_t * p_data, size_t size)
{
    m_print_fn(p_data, size);
}


/** @brief Returns true if data inside p_var is printable.
 */
static bool is_printable(var_t * p_var)
{
    size_t i;
    for (i = 0; i < p_var->data_size; i++)
    {
        if ((VAR_DATA(p_var)[i] < ' ') || (VAR_DATA(p_var)[i] >= '\x7F'))
        {
            return false;
        }
    }
    return true;
}


/* =============================================================================================
 *                               Raw type definition
 * =============================================================================================
 */


/** @brief Callback function to print raw variables. @see var_print_fn_t.
 */
static void raw_print(var_t * p_var, bool short_info)
{
    size_t            i;
    nrf_cli_t const * p_cli     = cli_get();
    bool              printable = is_printable(p_var);

    if (short_info)
    {
        if (printable)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Printable ASCII");
        }
        else
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Binary");
        }
    }
    else
    {
        raw_print_hex(VAR_DATA(p_var), p_var->data_size);
        if (printable)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "    \"");
            for (i = 0; i < p_var->data_size; i++)
            {
                nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%c", VAR_DATA(p_var)[i]);
            }
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\"\n");
        }
    }
}


const var_type_t g_raw_type =
{
    .p_description = "Raw data buffer",
    .free_fn = NULL,
    .print_fn = raw_print,
    .p_tag = NULL,
};


/* =============================================================================================
 *                           Sub commands used in arguments
 * =============================================================================================
 */


VAR_REGISTER_SUB_COMMANDS(m_subcmd_var, NULL);

static void subcmd_var_all_get(size_t idx, nrf_cli_static_entry_t * p_static);

NRF_CLI_CREATE_DYNAMIC_CMD(m_subcmd_var_all, subcmd_var_all_get);

static void subcmd_var_all_get(size_t idx, nrf_cli_static_entry_t * p_static)
{
    var_dynamic_cmd(&m_subcmd_var_all, idx, p_static);
}


/* =============================================================================================
 *                            Raw data manipulation functions
 * =============================================================================================
 */


/** @brief Command to create new raw variable. See command help for details.
 */
static bool cmd_raw(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    size_t     i;
    size_t     total_size;
    var_t    * p_output_var;
    var_t   ** pp_args;

    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK(argc >= 2);

    pp_args = (var_t **)var_temp_alloc(sizeof(var_t *) * (argc - 2));
    CLI_CHECK(pp_args != NULL);

    total_size = 0;

    for (i = 1; i < argc - 1; i++)
    {
        pp_args[i - 1] = cli_input_arg_get(i, &g_raw_type);
        CLI_CHECK(pp_args[i - 1] != NULL);
        total_size += pp_args[i - 1]->data_size;
    }

    p_output_var = cli_output_arg_get(argc - 1, &g_raw_type, total_size);
    CLI_CHECK(p_output_var != NULL);

    total_size = 0;

    for (i = 1; i < argc - 1; i++)
    {
        memcpy(&(VAR_DATA(p_output_var)[total_size]),
               VAR_DATA(pp_args[i - 1]),
               pp_args[i - 1]->data_size);
        total_size += pp_args[i - 1]->data_size;
    }

    return true;
}


CLI_CMD_REGISTER(raw, &m_subcmd_var_all, cmd_raw,
    "Create variable with raw data.\n"
    "Usage:\n"
    "  raw [data1 [data2 ...]] output\n"
    "Arguments:\n"
    "  IN  dataN   Multiple raw data arguments that will be concatenated.\n"
    "  OUT output  Newly created raw data.\n"
    );


/** @brief Command to add data to raw variable. @see cmd_prepend, @see cmd_append.
 */
static bool cmd_data_add(nrf_cli_t const * p_cli, size_t argc, char ** argv, bool append)
{
    size_t    i;
    var_t  ** pp_args = NULL;

    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK(argc >= 2);

    size_t total_size = 0;

    pp_args = var_temp_alloc(sizeof(var_t *) * (argc - 2));
    CLI_CHECK(pp_args != NULL);

    for (i = 1; i < argc - 1; i++)
    {
        pp_args[i - 1] = cli_input_arg_get(i, &g_raw_type);
        CLI_CHECK(pp_args[i - 1] != NULL);
        total_size += pp_args[i - 1]->data_size;
    }

    var_t * p_var = cli_input_arg_get(argc - 1, &g_raw_type);

    if (p_var == NULL)
    {
        nrf_cli_fprintf(p_cli,
            NRF_CLI_WARNING,
            "Variable does not exists or has different type. Creating new one.\n");
    }
    else
    {
        total_size += p_var->data_size;
    }

    var_t * p_output_var = cli_output_arg_get(argc - 1, &g_raw_type, total_size);
    CLI_CHECK(p_output_var != NULL);

    total_size = 0;

    if (append && (p_var != NULL))
    {
        memcpy(&(VAR_DATA(p_output_var)[total_size]), VAR_DATA(p_var), p_var->data_size);
        total_size += p_var->data_size;
    }

    for (i = 0; i < argc - 2; i++)
    {
        memcpy(&(VAR_DATA(p_output_var)[total_size]), VAR_DATA(pp_args[i]), pp_args[i]->data_size);
        total_size += pp_args[i]->data_size;
    }

    if (!append && (p_var != NULL))
    {
        memcpy(&(VAR_DATA(p_output_var)[total_size]), VAR_DATA(p_var), p_var->data_size);
    }

    if (p_var != NULL)
    {
        UNUSED_RETURN_VALUE(var_delete(p_var->name, false));
    }

    return true;
}


/** @brief Command to prepend data to raw variable. See command help for details.
 */
static bool cmd_prepend(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    return cmd_data_add(p_cli, argc, argv, false);
}


CLI_CMD_REGISTER(prepend, &m_subcmd_var_all, cmd_prepend,
    "Prepend raw data to a raw variable.\n"
    "Usage:\n"
    "  prepend [data1 [data2 ...]] variable\n"
    "Arguments:\n"
    "  IN     dataN     Multiple raw data arguments that are to be concatenated.\n"
    "  IN/OUT variable  Variable where data is to be prepended.\n"
    );


/** @brief Command to append data to raw variable. See command help for details.
 */
static bool cmd_append(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    return cmd_data_add(p_cli, argc, argv, true);
}


CLI_CMD_REGISTER(append, &m_subcmd_var_all, cmd_append,
    "Append raw data to a raw variable.\n"
    "Usage:\n"
    "  append [data1 [data2 ...]] variable\n"
    "Arguments:\n"
    "  IN     dataN     Multiple raw data arguments that are to be concatenated.\n"
    "  IN/OUT variable  Variable where data is to be appended.\n"
    );


/** @brief Command to get portion of a raw variable. See command help for details.
 */
static bool cmd_sub(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    var_t * p_src = NULL;
    char  * p_arg_end;

    CLI_ARGS_CHECK(argc == 5);

    p_src = cli_input_arg_get(1, &g_raw_type);
    CLI_CHECK(p_src != NULL);

    int from = (int)strtol(argv[2], &p_arg_end, 0);
    int count;
    if (p_arg_end[0] != '\0')
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_WARNING, "Number at argument 2 is invalid.\n");
        return false;
    }

    if (from < 0)
    {
        from = p_src->data_size + from;
    }

    if ((from < 0) || (from > (int)p_src->data_size))
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_WARNING, "Starting point outside the data.\n");
        return false;
    }

    if (strcmp(argv[3], "-") == 0)
    {
        count = p_src->data_size - from;
    }
    else
    {
        count = (int)strtol(argv[3], &p_arg_end, 0);
        if (p_arg_end[0] != '\0')
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_WARNING, "Number at argument 3 is invalid.\n");
            return false;
        }
    }

    if (count < 0)
    {
        count = p_src->data_size + count - from;
    }

    if ((count < 0) || (from + count > (int)p_src->data_size))
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_WARNING, "Endpoint outside the data.\n");
        return false;
    }

    var_t * p_output_var = cli_output_arg_get(4, &g_raw_type, count);
    CLI_CHECK(p_output_var != NULL);

    memcpy(VAR_DATA(p_output_var), &(VAR_DATA(p_src)[from]), count);

    return true;
}


CLI_CMD_REGISTER(sub, &m_subcmd_var, cmd_sub,
    "Create variable from part of a raw data.\n"
    "Usage:\n"
    "  sub source start_index bytes_count output\n"
    "Arguments:\n"
    "  IN  source       Source raw data to cut.\n"
    "  IN  start_index  Index of byte in the source where cut starts.\n"
    "                   May be negative to count from the end of data.\n"
    "  IN  bytes_count  Number of bytes to cut. Negative value indicate where cut\n"
    "                   is to end, counting from the end. Value \"-\" cuts to the end.\n"
    "  OUT output       Variable where portion of the source is to be placed.\n"
    );


/** @brief Command to change format of hex printing. See command help for details.
 */
static bool cmd_format(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    UNUSED_PARAMETER(p_cli);

    CLI_ARGS_CHECK(argc == 2);

    if (strcmp(argv[1], "hex") == 0)
    {
        m_print_fn = print_hex;
    }
    else if (strcmp(argv[1], "c") == 0)
    {
        m_print_fn = print_c;
    }
    else if (strcmp(argv[1], "cli") == 0)
    {
        m_print_fn = print_cli;
    }
    else
    {
        CLI_ARGS_CHECK(false);
    }

    return true;
}


NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_format)
{
    NRF_CLI_CMD(hex, NULL, "", NULL),
    NRF_CLI_CMD(c,   NULL, "", NULL),
    NRF_CLI_CMD(cli, NULL, "", NULL),
    NRF_CLI_SUBCMD_SET_END
};


CLI_CMD_REGISTER(format, &m_sub_format, cmd_format,
    "Change printing format of the raw data.\n"
    "Usage:\n"
    "  format type\n"
    "Arguments:\n"
    "  IN  type   Source raw data to cut. Following values are allowed:\n"
    "             hex - raw hex format (default),\n"
    "             c   - C source code,\n"
    "             cli - string that can be copy-pasted back to CLI.\n"
    );

