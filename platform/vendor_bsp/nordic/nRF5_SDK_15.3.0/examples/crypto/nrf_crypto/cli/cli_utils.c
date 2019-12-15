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

#include "nrf_crypto_error.h"
#include "vars.h"
#include "raw.h"

#include "cli_utils.h"


#define CONVERTER_GET(i)  NRF_SECTION_ITEM_GET(crypto_cli_conv, cli_converter_t, (i)) ///< @brief Utility macro to retrieve converter from nrf_section.
#define CONVERTERS_COUNT  NRF_SECTION_ITEM_COUNT(crypto_cli_conv, cli_converter_t)    ///< @brief Utility macro to count number of converters in nrf_section.


/* =============================================================================================
 *                           Static state of the cli_utils
 * =============================================================================================
 */


/** @brief Holds information about single output argument.
 *
 *  Each time command gets output argument, a structure of this type is created to know how to handle
 *  output argument at the end of command (in @ref cli_end_command).
 */
typedef struct
{
    uint32_t     arg_number; ///< @brief Command argument number.
    var_t      * p_var;      ///< @brief Pointer to temporary variable.
    bool         hidden;     ///< @brief true if user added '>' prefix in the argument.
    char const * p_name;     ///< @brief Final name of the variable as user requested.
} output_info_t;


static nrf_cli_t   const * mp_cli             = NULL;  ///< @brief Pointer to the nrf_cli instance provided to the command that is being executed.
static size_t              m_argc             = 0;     ///< @brief Number of arguments provided to the command that is being executed.
static char             ** m_argv             = NULL;  ///< @brief Arguments provided to the command that is being executed.
static output_info_t     * m_output_info      = NULL;  ///< @brief Array of output arguments that is to be handled at the end of command.
static size_t              m_output_info_size = 0;     ///< @brief Number of elements of @ref m_output_info array.


/* =============================================================================================
 *                               Forward declarations
 * =============================================================================================
 */


static var_t * input_arg_internal_get(char * p_str, size_t arg_number);


/* =============================================================================================
 *                           CLI Commands helper functions
 * =============================================================================================
 */


void cli_begin_command(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (mp_cli != NULL)
    {
        cli_end_command(false);
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Unexpected termination of previous command\n");
    }
    mp_cli = p_cli;
    m_argc = argc;
    m_argv = argv;
}


void cli_end_command(bool ok)
{
    size_t i;
    if (ok)
    {
        nrf_cli_fprintf(mp_cli, NRF_CLI_INFO, "OK\n");
        for (i = 0; i < m_output_info_size; i++)
        {
            if (strcmp(m_output_info[i].p_name, "$") != 0)
            {
                UNUSED_RETURN_VALUE(var_rename(m_output_info[i].p_var->name,
                                               m_output_info[i].p_name,
                                               true));
            }
            if (!m_output_info[i].hidden)
            {
                var_show(m_output_info[i].p_var);
            }
        }
    }
    else
    {
        nrf_cli_fprintf(mp_cli, NRF_CLI_ERROR, "ERROR\n");
    }
    var_all_temp_delete();
    mp_cli = NULL;
    m_argc = 0;
    m_argv = NULL;
    if (m_output_info != NULL)
    {
        var_free(m_output_info);
    }
    m_output_info = NULL;
    m_output_info_size = 0;
}


nrf_cli_t const * cli_get(void)
{
    return mp_cli;
}


void cli_error_print(ret_code_t ret)
{
    if (ret != NRF_SUCCESS)
    {
        char const * message = nrf_crypto_error_string_get(ret);
        nrf_cli_fprintf(cli_get(), NRF_CLI_ERROR, "Error: (0x%0X) %s\n", ret, message);
    }
}


/* =============================================================================================
 *                      Input arguments and converters handling functions
 * =============================================================================================
 */


/** @brief Converts the provided hex string into bytes.
 *
 *  Hex input is very flexible. All non-alphanumeric characters are acceptable and they are
 *  treated as separators. Each separated block of hex characters can be prefixed with "0x" or "0X".
 *
 *  @param p_str       Input hex string.
 *  @param p_output    Pointer to where to store converted data, or NULL to discard data and get only the size.
 *  @param arg_number  Argument number that is to be used for printing messages in case of error.
 *  @return            Number of bytes converted from hex string.
 */
static int convert_hex(const char * p_str, uint8_t * p_output, size_t arg_number)
{
    int     size        = 0;
    bool    token_start = true;
    size_t  hex_size    = 0;
    uint8_t hex_data    = 0;

    while (*p_str)
    {
        char c = *p_str;
        p_str++;
        if (token_start && (c == '0') && ((p_str[0] == 'x') || (p_str[0] == 'X')))
        {
            p_str++;
            token_start = false;
        }
        else if ((c >= '0') && (c <= '9'))
        {
            hex_data <<= 4;
            hex_data |= (uint8_t)(c - '0');
            hex_size += 4;
            token_start = false;
        }
        else if ((c >= 'a') && (c <= 'f'))
        {
            hex_data <<= 4;
            hex_data |= (uint8_t)(c - 'a' + 10);
            hex_size += 4;
            token_start = false;
        }
        else if ((c >= 'A') && (c <= 'F'))
        {
            hex_data <<= 4;
            hex_data |= (uint8_t)(c - 'A' + 10);
            hex_size += 4;
            token_start = false;
        }
        else if (((c >= 'G') && (c <= 'Z')) || ((c >= 'g') && (c <= 'z')))
        {
            nrf_cli_fprintf(mp_cli,
                            NRF_CLI_ERROR,
                            "Invalid character '%c' in hex argument %d.\n",
                            c,
                            arg_number);
            return -1;
        }
        else
        {
            token_start = true;
        }

        if (hex_size == 8)
        {
            if (p_output != NULL)
            {
                *p_output = hex_data;
                p_output++;
            }
            hex_data = 0;
            hex_size = 0;
            size++;
        }
    }

    if (hex_size != 0)
    {
        nrf_cli_fprintf(mp_cli,
                        NRF_CLI_ERROR,
                        "Number of hex characters is not even in argument %d.\n",
                        arg_number);
        return -1;
    }

    return size;
}


/** @brief Checks whether type is on the list.
 *
 * @param p_type  Type to search.
 * @param p_list  NULL-terminated array that is to be searched.
 * @return        true if type is on the list or list is empty, false otherwise.
 */
static bool type_in_list(var_type_t const * p_type, var_type_t const * const * p_list)
{
    var_type_t const * const * p_item;
    p_item = p_list;
    if (*p_item == NULL)
    {
        return true;
    }
    while (*p_item)
    {
        if (p_type == *p_item)
        {
            return true;
        }
        p_item++;
    }
    return false;
}


/** @brief Checks whether converter can be applied to specific input variable and output types.
 *
 *  @param p_converter  Converter to check.
 *  @param p_input      Input variable to check whether it can be converted by this converter.
 *  @param p_types      NULL-terminated array of expected types.
 *  @return             true if converter can be applied, false otherwise.
 */
static bool converter_match(cli_converter_t          * p_converter,
                            var_t                    * p_input,
                            var_type_t const * const * p_types)
{
    if ((p_converter->p_from_type != NULL) && (p_converter->p_from_type != p_input->header.p_type))
    {
        return false;
    }

    if ((p_converter->p_to_type != NULL) && !type_in_list(p_converter->p_to_type, p_types))
    {
        return false;
    }

    if (p_converter->test_fn != NULL)
    {
        var_type_t const * p_expected_type = p_converter->p_to_type;
        bool ok = p_converter->test_fn(p_converter, p_input, &p_expected_type);
        if (!ok || !type_in_list(p_expected_type, p_types))
        {
            return false;
        }
    }

    return true;
}


/** @brief Gets input argument prefixed by a converter name.
 *
 *  @param p_str       Argument string starting with converter name in brackets.
 *  @param arg_number  Argument number just for message printing.
 *  @return            New variable that contains converted data, or NULL on failure.
 */
static var_t * input_arg_with_converter(char * p_str, size_t arg_number)
{
    size_t            i;
    size_t            name_length;
    var_t           * p_source_var = NULL;
    var_t           * p_dest_var = NULL;
    cli_converter_t * p_converter;
    char            * p_converter_name;

    p_str++;
    // Trim leading non-printable characters
    while ((*p_str > 0) && (*p_str <= ' '))
    {
        p_str++;
    }
    p_converter_name = p_str;
    while (((*p_str >= 'a') && (*p_str <= 'z')) || ((*p_str >= 'A') && (*p_str <= 'Z'))
           || ((*p_str >= '0') && (*p_str <= '9')) || (*p_str == '_'))
    {
        p_str++;
    }
    name_length = p_str - p_converter_name;
    while ((*p_str > 0) && (*p_str <= ' '))
    {
        p_str++;
    }
    if (*p_str != ')')
    {
        nrf_cli_fprintf(mp_cli,
                        NRF_CLI_ERROR,
                        "Invalid converter name in argument %d.\n",
                        arg_number);
        return NULL;
    }
    p_str++;
    p_source_var = input_arg_internal_get(p_str, arg_number);
    if (p_source_var == NULL)
    {
        return NULL;
    }
    var_type_t const * p_types[] = { NULL };

    p_converter = NULL;
    for (i = 0; i < CONVERTERS_COUNT; i++)
    {
        p_converter = CONVERTER_GET(i);
        if ((strlen(p_converter->p_name) == name_length)
            && memcmp(p_converter->p_name, p_converter_name, name_length) == 0
            && converter_match(p_converter, p_source_var, p_types))
        {
            break;
        }
        p_converter = NULL;
    }

    if (p_converter == NULL)
    {
        nrf_cli_fprintf(mp_cli,
                        NRF_CLI_ERROR,
                        "Cannot find matching converter for argument %d.\n",
                        arg_number);
        return NULL;
    }

    bool ok = p_converter->conversion_fn(p_converter, p_source_var, &p_dest_var);

    if (var_is_temp(p_source_var))
    {
        UNUSED_RETURN_VALUE(var_delete(p_source_var->name, false));
    }

    if (!ok)
    {
        return NULL;
    }

    return p_dest_var;
}


/** @brief Gets input argument without automatic conversion.
 *
 *  @param p_str       Input argument string.
 *  @param arg_number  Argument number just for message printing.
 *  @return            New or existing variable that contains input argument, or NULL on failure.
 */
static var_t * input_arg_internal_get(char * p_str, size_t arg_number)
{
    int     size;
    var_t * p_var = NULL;

    // Trim leading non-printable characters
    while (((*p_str > 0) && (*p_str <= ' ')) || (*p_str == '>'))
    {
        p_str++;
    }

    switch (p_str[0])
    {
        case '$': // variable
            p_var = var_get(p_str, true);
            return p_var;

        case '@': // raw string
            p_var = var_temp_create(&g_raw_type, strlen(p_str) - 1, true);
            if (p_var == NULL)
            {
                return NULL;
            }
            memcpy(VAR_DATA(p_var), &p_str[1], p_var->data_size);
            return p_var;

        case '(': // converter
            return input_arg_with_converter(p_str, arg_number);

        default: // raw hex data
            size = convert_hex(p_str, NULL, arg_number);
            if (size < 0)
            {
                return NULL;
            }
            p_var = var_temp_create(&g_raw_type, size, true);
            if (p_var == NULL)
            {
                return NULL;
            }
            UNUSED_RETURN_VALUE(convert_hex(p_str, VAR_DATA(p_var), arg_number));
            return p_var;
    }
}


/** @brief Prints converter details.
 *  @param p_converter  Converter to print.
 */
static void print_converter(cli_converter_t * p_converter)
{
    nrf_cli_fprintf(mp_cli,
        NRF_CLI_NORMAL,
        "  (%s)\n    From: %s\n    To: %s\n",
        p_converter->p_name,
        p_converter->p_from_type ? p_converter->p_from_type->p_description : "ANY",
        p_converter->p_to_type ? p_converter->p_to_type->p_description : "ANY");
}


/** @brief Prints list of all converters that have non-zero flags.
 */
static void list_flagged_converters()
{
    size_t            i;
    cli_converter_t * p_converter;
    for (i = 0; i < CONVERTERS_COUNT; i++)
    {
        p_converter = CONVERTER_GET(i);
        if (*p_converter->p_flags != 0)
        {
            print_converter(p_converter);
        }
    }
}


var_t * cli_input_arg_multi_get(size_t arg_number, var_type_t const * const * p_types)
{
    size_t            i;
    var_t           * p_var = NULL;
    var_t           * p_converted = NULL;
    cli_converter_t * p_converter;

    if (arg_number >= m_argc)
    {
        nrf_cli_fprintf(mp_cli, NRF_CLI_ERROR, "Invalid number of arguments\n");
        return NULL;
    }

    p_var = input_arg_internal_get(m_argv[arg_number], arg_number);

    if (p_var == NULL)
    {
        return NULL;
    }

    if (!type_in_list(p_var->header.p_type, p_types))
    {
        size_t matching_converters = 0;
        cli_converter_t * p_selected = NULL;
        for (i = 0; i < CONVERTERS_COUNT; i++)
        {
            p_converter = CONVERTER_GET(i);
            *p_converter->p_flags = 0;
            if (converter_match(p_converter, p_var, p_types))
            {
                matching_converters++;
                *p_converter->p_flags = 1;
                p_selected = p_converter;
            }
        }

        if (matching_converters != 1)
        {
            nrf_cli_fprintf(mp_cli, NRF_CLI_ERROR, "Invalid type of argument %d.\n", arg_number);
            nrf_cli_fprintf(mp_cli, NRF_CLI_INFO, "Provided type:\n");
            nrf_cli_fprintf(mp_cli, NRF_CLI_INFO, "  %s\n", p_var->header.p_type->p_description);
            nrf_cli_fprintf(mp_cli, NRF_CLI_INFO, "Following types are acceptable:\n");
            var_type_t const * const * p_item;
            p_item = p_types;
            while (*p_item)
            {
                nrf_cli_fprintf(mp_cli, NRF_CLI_INFO, "  %s\n", (*p_item)->p_description);
                p_item++;
            }
        }

        if (matching_converters == 0)
        {
            nrf_cli_fprintf(mp_cli,
                            NRF_CLI_INFO,
                            "No converter found to do the conversion of argument %d.\n",
                            arg_number);
            return NULL;
        }
        else if (matching_converters > 1)
        {
            nrf_cli_fprintf(mp_cli, NRF_CLI_INFO, "Following converters can be used:\n");
            list_flagged_converters();
            return NULL;
        }
        nrf_cli_fprintf(mp_cli,
                        NRF_CLI_WARNING,
                        "Implicit conversion of argument %d:\n",
                        arg_number);
        print_converter(p_selected);

        bool ok = p_selected->conversion_fn(p_selected, p_var, &p_converted);

        if (var_is_temp(p_var))
        {
            UNUSED_PARAMETER(var_delete(p_var->name, false));
        }

        if (!ok)
        {
            return NULL;
        }

        return p_converted;
    }

    return p_var;
}


var_t * cli_input_arg_get(size_t arg_number, var_type_t const * p_type)
{
    var_type_t const * p_types[2] = { p_type, NULL };
    return cli_input_arg_multi_get(arg_number, p_types);
}


/* =============================================================================================
 *                                Output arguments functions
 * =============================================================================================
 */


/** @brief Internal part of the @ref cli_output_arg_get.
 *
 *  @param p_str       Output argument string.
 *  @param arg_number  Argument number just for message printing.
 *  @param type        Type of the new variable created.
 *  @param size        Initial size of the variable.
 *  @return            New variable.
 */
static var_t * output_arg_internal_get(char * p_str,
                                       size_t arg_number,
                                       var_type_t const * type,
                                       size_t size)
{
    size_t   i;
    var_t  * p_var;

    // Trim leading non-printable characters
    while ((*p_str > 0) && (*p_str <= ' '))
    {
        p_str++;
    }

    switch (p_str[0])
    {
        case '$': // variable
            p_var = var_temp_create(type, size, true);
            if (p_var != NULL)
            {
                m_output_info_size++;
                m_output_info = var_realloc(m_output_info,
                                            m_output_info_size * sizeof(output_info_t));
                if (m_output_info != NULL)
                {
                    m_output_info[m_output_info_size - 1].arg_number = arg_number;
                    m_output_info[m_output_info_size - 1].p_var = p_var;
                    m_output_info[m_output_info_size - 1].hidden = false;
                    m_output_info[m_output_info_size - 1].p_name = p_str;
                }
                else
                {
                    m_output_info_size = 0;
                    nrf_cli_fprintf(mp_cli,
                                    NRF_CLI_ERROR,
                                    "Out of memory. Output may be invalid.\n");
                }
            }
            return p_var;

        case '>': // hidden output
            p_var = output_arg_internal_get(p_str + 1, arg_number, type, size);
            for (i = 0; i < m_output_info_size; i++)
            {
                if (m_output_info[i].p_var == p_var)
                {
                    m_output_info[i].hidden = true;
                    break;
                }
            }
            return p_var;

        default:
            nrf_cli_fprintf(mp_cli,
                            NRF_CLI_ERROR,
                            "Output argument %d is invalid.\n",
                            arg_number);
            return NULL;
    }
}


var_t * cli_output_arg_get(size_t arg_number, var_type_t const * type, size_t size)
{
    return output_arg_internal_get(m_argv[arg_number], arg_number, type, size);
}


/* =============================================================================================
 *                                    CLI helper commands
 * =============================================================================================
 */


/** @brief Command to show all defined converters. See command help for details.
 */
static bool cmd_converters(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    size_t            i;
    cli_converter_t * p_converter;
    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(argv);
    CLI_ARGS_CHECK(argc == 1);
    for (i = 0; i < CONVERTERS_COUNT; i++)
    {
        p_converter = CONVERTER_GET(i);
        print_converter(p_converter);
    }
    return true;
}


CLI_CMD_REGISTER(converters, NULL, cmd_converters,
        "List all defined converters.\n"
        "Usage:\n"
        "  converters\n"
        );

