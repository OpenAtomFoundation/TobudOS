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

#include <stdbool.h>

#include "nrf_cli.h"
#include "nrf_crypto_aes.h"
#include "nordic_common.h"

#include "raw.h"
#include "vars.h"
#include "cli_utils.h"


/* =============================================================================================
 *                           AES utility definitions
 * =============================================================================================
 */


#define AES_TWO_BLOCKS_SIZE 32  ///< Size of two AES blocks (used to define correct size for buffers)


/** @brief Type used to index padding type of @ref mode_data_t
 */
typedef enum
{
    AES_PADDING_NONE  = 0, ///< No padding
    AES_PADDING_PKCS7 = 1, ///< PKCS7 padding
    AES_PADDING_COUNT = 2, ///< Number of possible padding types
} aes_padding_t;


/** @brief Type used to index key size of @ref mode_data_t
 */
typedef enum
{
    AES_KEY_SIZE_128   = 0, ///< 128-bit AES key
    AES_KEY_SIZE_192   = 1, ///< 192-bit AES key
    AES_KEY_SIZE_256   = 2, ///< 256-bit AES key
    AES_KEY_SIZE_COUNT = 3, ///< Number of possible AES key sizes
} aes_key_size_t;


/** @brief Holds information of specific AES mode
 */
typedef struct
{
    char                  const * p_name;                                  ///< Name of the mode
    nrf_crypto_aes_info_t const * p_infos[AES_PADDING_COUNT][AES_KEY_SIZE_COUNT];  ///< Array containing info for all available variants of this mode
} aes_mode_data_t;


/** @brief Contains all available modes and its info structures
 */
static const aes_mode_data_t all_infos[] = {
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC)
    {
        .p_name = "cbc",
        .p_infos = {
            {
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_128)
                &g_nrf_crypto_aes_cbc_128_info,
#else
                NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_192)
                &g_nrf_crypto_aes_cbc_192_info,
#else
                NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_256)
                &g_nrf_crypto_aes_cbc_256_info,
#else
                NULL,
#endif
            },
            {
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_128)
                &g_nrf_crypto_aes_cbc_128_pad_pkcs7_info,
#else
                NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_192)
                &g_nrf_crypto_aes_cbc_192_pad_pkcs7_info,
#else
                NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_256)
                &g_nrf_crypto_aes_cbc_256_pad_pkcs7_info,
#else
                NULL,
#endif
            },
        },
    },
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_MAC)
    {
        .p_name = "cbc-mac",
        .p_infos = {
            {
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_MAC_128)
                &g_nrf_crypto_aes_cbc_mac_128_info,
#else
                NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_MAC_192)
                &g_nrf_crypto_aes_cbc_mac_192_info,
#else
                NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_MAC_256)
                &g_nrf_crypto_aes_cbc_mac_256_info,
#else
                NULL,
#endif
            },
            {
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_MAC_128)
                &g_nrf_crypto_aes_cbc_mac_128_pad_pkcs7_info,
#else
                NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_MAC_192)
                &g_nrf_crypto_aes_cbc_mac_192_pad_pkcs7_info,
#else
                NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_MAC_256)
                &g_nrf_crypto_aes_cbc_mac_256_pad_pkcs7_info,
#else
                NULL,
#endif
            },
        },
    },
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CFB)
    {
        .p_name = "cfb",
        .p_infos = {
            {
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CFB_128)
                &g_nrf_crypto_aes_cfb_128_info,
#else
                NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CFB_192)
                &g_nrf_crypto_aes_cfb_192_info,
#else
                NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CFB_256)
                &g_nrf_crypto_aes_cfb_256_info,
#else
                NULL,
#endif
            },
            {
                NULL,
                NULL,
                NULL,
            },
        },
    },
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CMAC)
    {
        .p_name = "cmac",
        .p_infos = {
            {
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CMAC_128)
                &g_nrf_crypto_aes_cmac_128_info,
#else
                NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CMAC_192)
                &g_nrf_crypto_aes_cmac_192_info,
#else
                NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CMAC_256)
                &g_nrf_crypto_aes_cmac_256_info,
#else
                NULL,
#endif
            },
            {
                NULL,
                NULL,
                NULL,
            },
        },
    },
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CTR)
    {
        .p_name = "ctr",
        .p_infos = {
            {
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CTR_128)
                &g_nrf_crypto_aes_ctr_128_info,
#else
                NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CTR_192)
                &g_nrf_crypto_aes_ctr_192_info,
#else
                NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CTR_256)
                &g_nrf_crypto_aes_ctr_256_info,
#else
                NULL,
#endif
            },
            {
                NULL,
                NULL,
                NULL,
            },
        },
    },
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_ECB)
    {
        .p_name = "ecb",
        .p_infos = {
            {
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_ECB_128)
                &g_nrf_crypto_aes_ecb_128_info,
#else
                NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_ECB_192)
                &g_nrf_crypto_aes_ecb_192_info,
#else
                NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_ECB_256)
                &g_nrf_crypto_aes_ecb_256_info,
#else
                NULL,
#endif
            },
            {
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_ECB_128)
                &g_nrf_crypto_aes_ecb_128_pad_pkcs7_info,
#else
                NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_ECB_192)
                &g_nrf_crypto_aes_ecb_192_pad_pkcs7_info,
#else
                NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_ECB_256)
                &g_nrf_crypto_aes_ecb_256_pad_pkcs7_info,
#else
                NULL,
#endif
            },
        },
    },
#endif
};


/* =============================================================================================
 *                           Sub commands used in arguments
 * =============================================================================================
 */


/** @brief Subcommand handler to list all available AES modes.
 */
static void subcmd_aes_mode_get(size_t idx, nrf_cli_static_entry_t * p_static)
{
    p_static->p_syntax = (idx < ARRAY_SIZE(all_infos)) ? all_infos[idx].p_name : NULL;
    p_static->handler  = NULL;
    p_static->p_subcmd = NULL;
    p_static->p_help   = NULL;
}


/** @brief Subcommand handler to list all available operations.
 */
static void subcmd_op_get(size_t idx, nrf_cli_static_entry_t * p_static)
{
    static char const * table[] =
    {
        "decrypt",
        "encrypt",
        "mac",
        NULL,
    };
    p_static->p_syntax = table[idx];
    p_static->handler  = NULL;
    p_static->p_subcmd = NULL;
    p_static->p_help   = NULL;
}


/** @brief Subcommand handler to list all available paddings.
 */
static void subcmd_pad_get(size_t idx, nrf_cli_static_entry_t * p_static)
{
    static char const * table[] =
    {
        "none",
        "pkcs7",
    };
    if (idx < ARRAY_SIZE(table))
    {
        p_static->p_syntax = table[idx];
        p_static->handler  = NULL;
        p_static->p_subcmd = NULL;
        p_static->p_help   = NULL;
    }
    else
    {
        var_dynamic_cmd(NULL, idx - ARRAY_SIZE(table), p_static);
    }
}


VAR_REGISTER_SUB_COMMANDS(m_subcmd_var, NULL);
VAR_REGISTER_SUB_COMMANDS(m_subcmd_var_var, &m_subcmd_var);
VAR_REGISTER_SUB_COMMANDS(m_subcmd_var_var_var, &m_subcmd_var_var);
CLI_CREATE_DYNAMIC_CMD_WITH_SUBCMD(m_subcmd_pad_var_var, subcmd_pad_get, &m_subcmd_var_var);
CLI_CREATE_DYNAMIC_CMD_WITH_SUBCMD(m_subcmd_op_pad_var_var, subcmd_op_get, &m_subcmd_pad_var_var);
CLI_CREATE_DYNAMIC_CMD_WITH_SUBCMD(m_subcmd_mode_op_pad_var_var,
                                   subcmd_aes_mode_get,
                                   &m_subcmd_op_pad_var_var);
CLI_CREATE_DYNAMIC_CMD_WITH_SUBCMD(m_subcmd_pad_var_var_var, subcmd_pad_get, &m_subcmd_var_var_var);
CLI_CREATE_DYNAMIC_CMD_WITH_SUBCMD(m_subcmd_op_pad_var_var_var,
                                   subcmd_op_get,
                                   &m_subcmd_pad_var_var_var);
CLI_CREATE_DYNAMIC_CMD_WITH_SUBCMD(m_subcmd_mode_op_pad_var_var_var,
                                   subcmd_aes_mode_get,
                                   &m_subcmd_op_pad_var_var_var);


/* =============================================================================================
 *                               AES CLI type definition
 * =============================================================================================
 */


/** @brief Structure that is put inside AES context CLI variables. Holds AES context and
 *         additional information.
 */
typedef struct
{
    char                     const * p_name;
    size_t                           key_size;
    bool                             pad_pkcs7;
    bool                             initialized;
    nrf_crypto_aes_context_t         context;
    nrf_crypto_aes_info_t    const * p_info;
    nrf_crypto_operation_t           operation;
} aes_cli_context_t;


/** @brief Frees AES context CLI variable. @see var_free_fn_t.
 */
static bool aes_context_free(var_t * p_var)
{
    aes_cli_context_t * p_aes_cli_context = (aes_cli_context_t *)VAR_DATA(p_var);

    if (p_aes_cli_context->initialized)
    {
        ret_code_t ret;
        uint8_t    buffer[AES_TWO_BLOCKS_SIZE];
        size_t     size = sizeof(buffer);
        ret = nrf_crypto_aes_finalize(&p_aes_cli_context->context, buffer, 0, buffer, &size);
        cli_error_print(ret);
        p_aes_cli_context->initialized = false;
    }

    return true;
}


/** @brief Prints AES context CLI variable. @see var_print_fn_t.
 */
static void aes_context_print(var_t * p_var, bool short_info)
{
    if (short_info)
    {
        aes_cli_context_t * p_aes_cli_context = (aes_cli_context_t *)VAR_DATA(p_var);
        nrf_cli_fprintf(cli_get(),
                        NRF_CLI_NORMAL,
                        "%s %s%s, %d-bit, %s.",
                        p_aes_cli_context->operation == NRF_CRYPTO_DECRYPT ? "Decrypt" :
                            p_aes_cli_context->operation == NRF_CRYPTO_ENCRYPT ? "Encrypt" :
                            "Calculate MAC",
                        p_aes_cli_context->p_name,
                        p_aes_cli_context->pad_pkcs7 ? " with pkcs7 padding" : "",
                        p_aes_cli_context->key_size * 8,
                        p_aes_cli_context->initialized ? "ready" : "finalized");
    }
}


/** @brief Defines CLI type for AES context variables
 */
static const var_type_t m_aes_context_type =
{
    .p_description = "AES context",
    .free_fn       = aes_context_free,
    .print_fn      = aes_context_print,
    .p_tag         = NULL,
};


/** @brief Gets AES info structure based on provided parameters.
 *
 *  @param p_mode_name   Requested AES mode.
 *  @param pad_pkcs7     Set to true if padding is required.
 *  @param key_size      AES key size in bits.
 *  @param pp_mode_name  Output parameter that will contain pointer to static mode name string.
 *  @return              Pointer to AES info structure or NULL on error. In case of error adequate
 *                       information are printed.
 */
static nrf_crypto_aes_info_t const * aes_info_get(char    const * p_mode_name,
                                                  bool            pad_pkcs7,
                                                  size_t          key_size,
                                                  char   const ** pp_mode_name)
{
    size_t                        i;
    aes_mode_data_t       const * p_mode_data;
    aes_key_size_t                key_size_index;
    aes_padding_t                 pad_index;
    nrf_crypto_aes_info_t const * p_result = NULL;

    if (key_size == 128 / 8)
    {
        key_size_index = AES_KEY_SIZE_128;
    }
    else if (key_size == 192 / 8)
    {
        key_size_index = AES_KEY_SIZE_192;
    }
    else if (key_size == 256 / 8)
    {
        key_size_index = AES_KEY_SIZE_256;
    }
    else
    {
        nrf_cli_fprintf(cli_get(),
                        NRF_CLI_ERROR,
                        "%d bit key size is not supported.\n",
                        8 * key_size);
        return NULL;
    }

    pad_index = pad_pkcs7 ? AES_PADDING_PKCS7 : AES_PADDING_NONE;

    for (i = 0; i < ARRAY_SIZE(all_infos); i++)
    {
        p_mode_data = &all_infos[i];
        if (strcmp(p_mode_data->p_name, p_mode_name) == 0)
        {
            *pp_mode_name = p_mode_data->p_name;
            p_result = p_mode_data->p_infos[pad_index][key_size_index];
            if (p_result == NULL)
            {
                nrf_cli_fprintf(cli_get(),
                                NRF_CLI_ERROR,
                                "Selected key size or padding is not supported by this mode.\n");
            }
            return p_result;
        }
    }

    nrf_cli_fprintf(cli_get(),
                    NRF_CLI_ERROR,
                    "Mode %s is not supported.\n",
                    p_mode_name);

    return NULL;
}


/* =============================================================================================
 *                                        AES commands
 * =============================================================================================
 */


/** @brief Command to initialize AES context. See command help for details.
 */
static bool cmd_aes_init(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                    ret;
    nrf_crypto_aes_info_t const * p_info;
    var_t                       * p_context_var;
    size_t                        key_arg_index = 3;
    bool                          pad_pkcs7     = false;
    char                  const * p_mode_name;
    nrf_crypto_operation_t        operation     = NRF_CRYPTO_ENCRYPT;
    var_t                       * p_iv_var      = NULL;
    var_t                       * p_key_var;

    UNUSED_PARAMETER(p_cli);

    CLI_ARGS_CHECK((5 <= argc) && (argc <= 7));

    if (strcmp(argv[2], "encrypt") == 0)
    {
        operation = NRF_CRYPTO_ENCRYPT;
    }
    else if (strcmp(argv[2], "decrypt") == 0)
    {
        operation = NRF_CRYPTO_DECRYPT;
    }
    else if (strcmp(argv[2], "mac") == 0)
    {
        operation = NRF_CRYPTO_MAC_CALCULATE;
    }
    else
    {
        CLI_ARGS_CHECK(false);
    }

    if (strcmp(argv[3], "pkcs7") == 0)
    {
        pad_pkcs7 = true;
        key_arg_index++;
    }
    else if (strcmp(argv[3], "none") == 0)
    {
        pad_pkcs7 = false;
        key_arg_index++;
    }

    p_key_var = cli_input_arg_get(key_arg_index, &g_raw_type);
    CLI_CHECK(p_key_var != NULL);

    p_info = aes_info_get(argv[1], pad_pkcs7, p_key_var->data_size, &p_mode_name);
    CLI_CHECK(p_info);

    if (argc > key_arg_index + 2)
    {
        p_iv_var = cli_input_arg_get(key_arg_index + 1, &g_raw_type);
        CLI_CHECK(p_iv_var != NULL);
    }

    p_context_var = cli_output_arg_get(argc - 1, &m_aes_context_type, sizeof(aes_cli_context_t));
    CLI_CHECK(p_context_var != NULL);
    aes_cli_context_t * p_aes_cli_context = (aes_cli_context_t *)VAR_DATA(p_context_var);

    p_aes_cli_context->initialized = false;
    p_aes_cli_context->key_size    = p_key_var->data_size;
    p_aes_cli_context->p_name      = p_mode_name;
    p_aes_cli_context->p_info      = p_info;
    p_aes_cli_context->pad_pkcs7   = pad_pkcs7;
    p_aes_cli_context->operation   = operation;

    ret = nrf_crypto_aes_init(&p_aes_cli_context->context, p_info, operation);
    CLI_ERROR_CHECK(ret);

    p_aes_cli_context->initialized = true;

    ret = nrf_crypto_aes_key_set(&p_aes_cli_context->context, VAR_DATA(p_key_var));
    CLI_ERROR_CHECK(ret);

    if (p_iv_var != NULL)
    {
        ret = nrf_crypto_aes_iv_set(&p_aes_cli_context->context, VAR_DATA(p_iv_var));
        CLI_ERROR_CHECK(ret);
    }

    return true;
}


CLI_CMD_REGISTER(aes_init, &m_subcmd_mode_op_pad_var_var, cmd_aes_init,
    "Initialize AES context.\n"
    "Usage:\n"
    "  aes_init mode operation [padding] key [iv] context\n"
    "Arguments:\n"
    "  IN  mode      AES block cipher mode (cbc, cbc-mac, cfb, cmac, ctr, ecb)\n"
    "  IN  operation Operation to perform (encrypt, decrypt, mac)\n"
    "  IN  padding   Optional padding algorithm (pkcs7 or none)\n"
    "  IN  key       AES key (must be 128, 192, 256 bit long)\n"
    "  IN  iv        Optional initialization vector\n"
    "  OUT context   Newly initialized context\n"
    );


/** @brief Command to update AES context. See command help for details.
 */
static bool cmd_aes_update(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t          ret;
    var_t             * p_context_var;
    var_t             * p_data_in_var;
    aes_cli_context_t * p_aes_cli_context;
    uint8_t           * p_data_out = NULL;

    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK((3 <= argc) && (argc <= 4));

    p_context_var = cli_input_arg_get(1, &m_aes_context_type);
    CLI_CHECK(p_context_var);

    p_aes_cli_context = (aes_cli_context_t *)VAR_DATA(p_context_var);
    CLI_CHECK(p_aes_cli_context->initialized);

    p_data_in_var = cli_input_arg_get(2, &g_raw_type);
    CLI_CHECK(p_data_in_var);

    if (argc >= 4)
    {
        var_t * p_data_out_in_var  = NULL;
        var_t * p_data_out_out_var = NULL;
        size_t  size               = 0;

        p_data_out_in_var = cli_input_arg_get(3, &g_raw_type);

        if (p_data_out_in_var == NULL)
        {
            nrf_cli_fprintf(p_cli,
                NRF_CLI_WARNING,
                "Variable does not exists or has different type. Creating new one.\n");
        }
        else
        {
            size += p_data_out_in_var->data_size;
        }

        p_data_out_out_var = cli_output_arg_get(3, &g_raw_type, size + p_data_in_var->data_size);

        p_data_out = VAR_DATA(p_data_out_out_var);

        if (p_data_out_in_var != NULL)
        {
            memcpy(p_data_out, VAR_DATA(p_data_out_in_var), size);
        }

        p_data_out = &p_data_out[size];
    }
    else
    {
        p_data_out = (uint8_t *)var_temp_alloc(p_data_in_var->data_size);
        CLI_CHECK(p_data_out != NULL);
    }

    ret = nrf_crypto_aes_update(&p_aes_cli_context->context,
                                VAR_DATA(p_data_in_var),
                                p_data_in_var->data_size,
                                p_data_out);
    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(aes_update, &m_subcmd_var_var_var, cmd_aes_update,
    "Update AES with a new data.\n"
    "Usage:\n"
    "  aes_update context data_in [data_out]\n"
    "Arguments:\n"
    "  IN     context  AES context\n"
    "  IN     data_in  Input data\n"
    "                  For all modes except CFB it must be multiple of 16 bytes\n"
    "  IN/OUT data_out Optional result of the update\n"
    "                  If it is not present then data will be discarded\n"
    "                  If it is an existing raw variable then data will be appended\n"
    "                  If is is non-existing variable then it will be created\n"
    );


/** @brief Command to finalize AES context. See command help for details.
 */
static bool cmd_aes_finalize(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t          ret;
    var_t             * p_context_var;
    aes_cli_context_t * p_aes_cli_context;
    var_t             * p_data_in_var;
    uint8_t           * p_data_out         = NULL;
    var_t             * p_data_out_out_var = NULL;
    size_t              data_old_size      = 0;
    size_t              buffer_size        = 0;

    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK((3 <= argc) && (argc <= 4));

    p_context_var = cli_input_arg_get(1, &m_aes_context_type);
    CLI_CHECK(p_context_var);

    p_aes_cli_context = (aes_cli_context_t *)VAR_DATA(p_context_var);
    CLI_CHECK(p_aes_cli_context->initialized);

    p_data_in_var = cli_input_arg_get(2, &g_raw_type);
    CLI_CHECK(p_data_in_var);

    if (argc >= 4)
    {
        var_t * p_data_out_in_var = NULL;

        p_data_out_in_var = cli_input_arg_get(3, &g_raw_type);

        if (p_data_out_in_var == NULL)
        {
            nrf_cli_fprintf(p_cli,
                NRF_CLI_WARNING,
                "Variable does not exists or has different type. Creating new one.\n");
        }
        else
        {
            data_old_size += p_data_out_in_var->data_size;
        }

        p_data_out_out_var = cli_output_arg_get(3,
            &g_raw_type,
            data_old_size + p_data_in_var->data_size + AES_TWO_BLOCKS_SIZE);
        buffer_size = p_data_in_var->data_size + AES_TWO_BLOCKS_SIZE;

        p_data_out = VAR_DATA(p_data_out_out_var);

        if (p_data_out_in_var != NULL)
        {
            memcpy(p_data_out, VAR_DATA(p_data_out_in_var), data_old_size);
        }

        p_data_out = &p_data_out[data_old_size];
    }
    else
    {
        p_data_out = (uint8_t *)var_temp_alloc(p_data_in_var->data_size + AES_TWO_BLOCKS_SIZE);
        CLI_CHECK(p_data_out != NULL);
        buffer_size = p_data_in_var->data_size + AES_TWO_BLOCKS_SIZE;
    }

    p_aes_cli_context->initialized = false;

    ret = nrf_crypto_aes_finalize(&p_aes_cli_context->context,
                                  VAR_DATA(p_data_in_var),
                                  p_data_in_var->data_size,
                                  p_data_out,
                                  &buffer_size);
    CLI_ERROR_CHECK(ret);

    if (p_data_out_out_var != NULL)
    {
        p_data_out_out_var->data_size = data_old_size + buffer_size;
    }

    return true;
}


CLI_CMD_REGISTER(aes_finalize, &m_subcmd_var_var_var, cmd_aes_finalize,
    "Finalize AES.\n"
    "Usage:\n"
    "  aes_finalize context data_in [data_out]\n"
    "Arguments:\n"
    "  IN     context  AES context\n"
    "  IN     data_in  Last part of input data\n"
    "  IN/OUT data_out Optional result data of the finalization\n"
    "                  If it is not present then data will be discarded\n"
    "                  If it is an existing raw variable then data will be appended\n"
    "                  If is is non-existing variable then it will be created\n"
    );


/** @brief Command to do AES in one step. See command help for details.
 */
static bool cmd_aes_crypt(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                    ret;
    nrf_crypto_aes_info_t const * p_info;
    size_t                        key_arg_index = 3;
    bool                          pad_pkcs7     = false;
    char                  const * p_mode_name;
    nrf_crypto_operation_t        operation     = NRF_CRYPTO_ENCRYPT;
    var_t                       * p_key_var;
    uint8_t                     * p_iv          = NULL;
    var_t                       * p_data_in_var;
    var_t                       * p_data_out_var;

    UNUSED_PARAMETER(p_cli);

    CLI_ARGS_CHECK((6 <= argc) && (argc <= 8));

    if (strcmp(argv[2], "encrypt") == 0)
    {
        operation = NRF_CRYPTO_ENCRYPT;
    }
    else if (strcmp(argv[2], "decrypt") == 0)
    {
        operation = NRF_CRYPTO_DECRYPT;
    }
    else if (strcmp(argv[2], "mac") == 0)
    {
        operation = NRF_CRYPTO_MAC_CALCULATE;
    }
    else
    {
        CLI_ARGS_CHECK(false);
    }

    if (strcmp(argv[3], "pkcs7") == 0)
    {
        pad_pkcs7 = true;
        key_arg_index++;
    }
    else if (strcmp(argv[3], "none") == 0)
    {
        pad_pkcs7 = false;
        key_arg_index++;
    }

    p_key_var = cli_input_arg_get(key_arg_index, &g_raw_type);
    CLI_CHECK(p_key_var != NULL);

    p_info        = aes_info_get(argv[1], pad_pkcs7, p_key_var->data_size, &p_mode_name);
    CLI_CHECK(p_info);

    if (argc > key_arg_index + 3)
    {
        var_t * p_iv_var = NULL;
        p_iv_var = cli_input_arg_get(key_arg_index + 1, &g_raw_type);
        CLI_CHECK(p_iv_var != NULL);
        p_iv = VAR_DATA(p_iv_var);
    }

    p_data_in_var = cli_input_arg_get(argc - 2, &g_raw_type);
    CLI_CHECK(p_data_in_var);

    p_data_out_var = cli_output_arg_get(argc - 1,
                                        &g_raw_type,
                                        p_data_in_var->data_size + AES_TWO_BLOCKS_SIZE);
    CLI_CHECK(p_data_out_var);

    ret = nrf_crypto_aes_crypt(NULL,
                               p_info,
                               operation,
                               VAR_DATA(p_key_var),
                               p_iv,
                               VAR_DATA(p_data_in_var),
                               p_data_in_var->data_size,
                               VAR_DATA(p_data_out_var),
                               &p_data_out_var->data_size);
    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(aes_crypt, &m_subcmd_mode_op_pad_var_var_var, cmd_aes_crypt,
    "Calculate AES in one step.\n"
    "Usage:\n"
    "  aes_crypt mode operation [padding] key [iv] data_in data_out\n"
    "Arguments:\n"
    "  IN  mode      AES block cipher mode (cbc, cbc-mac, cfb, cmac, ctr, ecb)\n"
    "  IN  operation Operation to perform (encrypt, decrypt, mac)\n"
    "  IN  padding   Optional padding algorithm (pkcs7 or none)\n"
    "  IN  key       AES key (must be 128, 192, 256 bit long)\n"
    "  IN  iv        Optional initialization vector\n"
    "  IN  data_in   Data to process\n"
    "  OUT data_out  Resulting data\n"
    );
