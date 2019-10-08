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
#ifndef CLI_UTILS_H__
#define CLI_UTILS_H__

#include "nrf_cli.h"
#include "nrf_section.h"
#include "vars.h"

/** @brief Prints a message regarding invalid parameter and returns false if provided condition is false.
 */
#define CLI_ARGS_CHECK(...) if (!(__VA_ARGS__)) \
    { \
        nrf_cli_fprintf(cli_get(), \
                        NRF_CLI_ERROR, \
                        "Invalid arguments. Run the command with -h option to get more help.\r\n");\
        return false; \
    }

/** @brief Returns false if provided condition is false.
 */
#define CLI_CHECK(...) if (!(__VA_ARGS__)) \
    { \
        return false; \
    }

/** @brief Prints nrf_crypto error message and returns false if provided error code is different than NRF_SUCCESS.
 */
#define CLI_ERROR_CHECK(code) if ((code) != NRF_SUCCESS) \
    { \
        cli_error_print((code)); \
        return false; \
    }

/** @brief Utility function to create CLI dynamic command with specified subcommands.
 */
#define CLI_CREATE_DYNAMIC_CMD_WITH_SUBCMD(name, func, subcmd) \
    static void _##name##_get(size_t idx, nrf_cli_static_entry_t * p_static) \
    { \
        func(idx, p_static); \
        if (p_static->p_syntax != NULL) \
        { \
            p_static->p_subcmd = subcmd; \
        } \
    } \
    NRF_CLI_CREATE_DYNAMIC_CMD(name, _##name##_get)

/** @brief Macro to register the nrf_crypto CLI command.
 *
 *  Macro creates a new function that handles common tasks performed by all nrf_crypto CLI commands:
 *  printing help text when requested, call @ref cli_begin_command and @ref cli_end_command.
 *
 *  @param p_syntax   CLI command name.
 *  @param p_subcmd   Subcommand of this command.
 *  @param p_handler  Command handler function. It is the same as normal CLI command handler except
 *                    it returns bool indicating whether the command was successful.
 *  @param p_help     Help string that is to be printed when user requests help for this command.
 */
#define CLI_CMD_REGISTER(p_syntax, p_subcmd, p_handler, p_help) \
    static void _##p_syntax##_cmd_int(nrf_cli_t const * p_cli, size_t argc, char ** argv) \
    { \
        if (nrf_cli_help_requested(p_cli)) \
        { \
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\r\n%s\r\n", p_help); \
        } \
        else \
        { \
            cli_begin_command(p_cli, argc, argv); \
            bool ok = (p_handler)(p_cli, argc, argv); \
            cli_end_command(ok); \
        } \
    } \
    NRF_CLI_CMD_REGISTER(p_syntax, p_subcmd, "", _##p_syntax##_cmd_int);

/** @brief Macro to register the variable converter.
 *
 *  @param name          Unique name of the converter.
 *  @param cli_name      String that contains the name as it is to be in CLI. There can be multiple converters
 *                       with the same CLI name.
 *  @param from          Pointer to the variable type that defines source type of this converter, or NULL
 *                       if any type is acceptable by this converter.
 *  @param to            Pointer to the variable type that defines destination type of this converter, or NULL
 *                       if it is to be set by test function.
 *  @param conversion    Conversion function. @see cli_conversion_fn_t.
 *  @param test          Test function. @see cli_conversion_test_fn_t.
 *  @param converter_tag Transparent pointer that can be used by callback functions.
 */
#define CLI_CONVERTER_REGISTER(name, cli_name, from, to, conversion, test, converter_tag) \
    static uint32_t _##name##_flags = 0; \
    NRF_SECTION_ITEM_REGISTER(crypto_cli_conv, static cli_converter_t const name) = \
    { \
        .p_name        = (cli_name), \
        .p_from_type   = (from), \
        .p_to_type     = (to), \
        .conversion_fn = (conversion), \
        .test_fn       = (test), \
        .p_flags       = &_##name##_flags, \
        .p_tag         = (converter_tag), \
    };

struct cli_converter_s;

/** @brief Conversion callback function.
 *  @param p_converter  Pointer to the registered converter structure.
 *  @param p_input      Variable to convert.
 *  @param pp_output    Output parameter where new variable is to be stored.
 *  @return             true if conversion was successful.
 */
typedef bool (*cli_conversion_fn_t)(struct cli_converter_s const * p_converter,
                                    var_t                        * p_input,
                                    var_t                       ** pp_output);

/** @brief Callback function to test whether the converter can convert specific variable.
 *  @param p_converter  Pointer to the registered converter structure.
 *  @param p_input      Variable to check.
 *  @param pp_type      Input/output parameter that contains the destination variable type.
 *                      On input, it is the same as in @ref CLI_CONVERTER_REGISTER.
 *                      It can be optionally changed.
 *  @return             true if convesion is possible.
 */
typedef bool (*cli_conversion_test_fn_t)(struct cli_converter_s const * p_converter,
                                         var_t                        * p_input,
                                         var_type_t            const ** pp_type);

/** @brief Structure containing the converter definition.
 */
typedef struct cli_converter_s
{
    const char              * p_name;        ///< CLI name of the converter.
    var_type_t        const * p_from_type;   ///< Input variable type, or NULL if all types are acceptable.
    var_type_t        const * p_to_type;     ///< Output variable type, or NULL if type is to be determined by test_fn.
    cli_conversion_fn_t       conversion_fn; ///< Conversion function callback.
    cli_conversion_test_fn_t  test_fn;       ///< Test function callback.
    uint32_t                * p_flags;       ///< Pointer to the variable that stores flags that are to be used during converter matching functions.
    void                    * p_tag;         ///< Transparent pointer that can be used by callback functions.
} cli_converter_t;

NRF_SECTION_DEF(crypto_cli_conv, cli_converter_t);


/** @brief Function that must be called before each nrf_crypto CLI command. @see CLI_CMD_REGISTER.
 */
void cli_begin_command(nrf_cli_t const * p_cli, size_t argc, char ** argv);

/** @brief Function that must be called after each nrf_crypto CLI command. @see CLI_CMD_REGISTER.
 *  It informs user about the result of the command, prints all command output arguments, and does
 *  final cleanup.
 */
void cli_end_command(bool ok);

/** @brief Returns handle to nrf_cli that was saved by @ref cli_begin_command.
 */
nrf_cli_t const * cli_get(void);

/** @brief Prints massage based on nrf_crypto error code, or nothing if the return value is NRF_SUCCESS.
 */
void cli_error_print(ret_code_t ret);


/** @brief Gets the command's input argument as a variable.
 *
 *  All conversions and type checking is done inside this function. Temporary variables
 *  are also created, if required. In case of errors or warnings proper messages are printed, so
 *  the caller does not have to print anything in case or error.
 *
 *  @param arg_number  Command argument number.
 *  @param type        Expected type, or NULL if all types are acceptable.
 *  @return            Argument as pointer to variable, or NULL if argument is invalid.
 */
var_t * cli_input_arg_get(size_t arg_number, var_type_t const * type);


/** @brief Gets the command's input argument as a variable with multiple acceptable types.
 *
 *  @see cli_input_arg_get
 *
 * @param arg_number  Command argument number.
 * @param types       NULL-terminated array of expected types.
 * @return            Argument as pointer to variable, or NULL if argument is invalid.
 */
var_t * cli_input_arg_multi_get(size_t arg_number, var_type_t const * const * types);


/** @brief Gets the command's output argument as a variable.
 *
 *  Function creates a temporary variable to store command result.
 *
 *  If command was successful, this temporary variable is renamed to name provided by user.
 *  If the variable already exists, a warning is displayed and the variable is overridden.
 *  The output variable is printed out if user does not add '>' prefix in the argument.
 *
 *  If command fails, all temporary variables are removed, so that state is the same as before
 *  the execution of the command. This prevents from partial command results on failure.
 *
 *  @param arg_number  Command argument number.
 *  @param type        Type of the new variable created.
 *  @param size        Initial size of newly created variable.
 *  @return            The new variable created.
 */
var_t * cli_output_arg_get(size_t arg_number, var_type_t const * type, size_t size);


#endif // CLI_UTILS_H__
