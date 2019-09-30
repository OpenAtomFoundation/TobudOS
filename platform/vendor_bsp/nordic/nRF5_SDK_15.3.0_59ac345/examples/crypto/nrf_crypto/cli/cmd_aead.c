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
#include "nrf_crypto_aead.h"
#include "nordic_common.h"

#include "raw.h"
#include "vars.h"
#include "cli_utils.h"


/* =============================================================================================
 *                           AEAD utility definitions
 * =============================================================================================
 */


/** @brief Type used to index key size of @ref mode_data_t
 */
typedef enum
{
    AEAD_KEY_SIZE_128   = 0, ///< 128-bit AEAD key
    AEAD_KEY_SIZE_192   = 1, ///< 192-bit AEAD key
    AEAD_KEY_SIZE_256   = 2, ///< 256-bit AEAD key
    AEAD_KEY_SIZE_COUNT = 3, ///< Number of possible AES key sizes
} aead_key_size_t;


/** @brief Holds information of specific AEAD mode
 */
typedef struct
{
    char                   const * p_name;
    nrf_crypto_aead_info_t const * p_infos[AEAD_KEY_SIZE_COUNT];
} aead_mode_data_t;


/** @brief Contains all available modes and its info structures
 */
static const aead_mode_data_t all_infos[] = {
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM)
    {
        .p_name = "ccm",
        .p_infos = {
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM_128)
            &g_nrf_crypto_aes_ccm_128_info,
#else
            NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM_192)
            &g_nrf_crypto_aes_ccm_192_info,
#else
            NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM_256)
            &g_nrf_crypto_aes_ccm_256_info,
#else
            NULL,
#endif
        },
    },
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM_STAR)
    {
        .p_name = "ccm*",
        .p_infos = {
            &g_nrf_crypto_aes_ccm_star_128_info,
            NULL,
            NULL,
        },
    },
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_EAX)
    {
        .p_name = "eax",
        .p_infos = {
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_EAX_128)
            &g_nrf_crypto_aes_eax_128_info,
#else
            NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_EAX_192)
            &g_nrf_crypto_aes_eax_192_info,
#else
            NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_EAX_256)
            &g_nrf_crypto_aes_eax_256_info,
#else
            NULL,
#endif
        },
    },
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_GCM)
    {
        .p_name = "gcm",
        .p_infos = {
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_GCM_128)
            &g_nrf_crypto_aes_gcm_128_info,
#else
            NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_GCM_192)
            &g_nrf_crypto_aes_gcm_192_info,
#else
            NULL,
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_GCM_256)
            &g_nrf_crypto_aes_gcm_256_info,
#else
            NULL,
#endif
        },
    },
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_CHACHA_POLY)
    {
        .p_name = "chacha-poly",
        .p_infos = {
            NULL,
            NULL,
            &g_nrf_crypto_chacha_poly_256_info,
        },
    },
#endif
};


static nrf_crypto_aead_info_t const * aead_info_get(char const * p_mode_name, size_t key_size)
{
    size_t                         i;
    aead_mode_data_t       const * p_mode_data;
    aead_key_size_t                key_size_index;
    nrf_crypto_aead_info_t const * p_result = NULL;

    if (key_size == 128 / 8)
    {
        key_size_index = AEAD_KEY_SIZE_128;
    }
    else if (key_size == 192 / 8)
    {
        key_size_index = AEAD_KEY_SIZE_192;
    }
    else if (key_size == 256 / 8)
    {
        key_size_index = AEAD_KEY_SIZE_256;
    }
    else
    {
        nrf_cli_fprintf(cli_get(),
                        NRF_CLI_ERROR,
                        "%d bit key size is not supported.\n",
                        8 * key_size);
        return NULL;
    }

    for (i = 0; i < ARRAY_SIZE(all_infos); i++)
    {
        p_mode_data = &all_infos[i];
        if (strcmp(p_mode_data->p_name, p_mode_name) == 0)
        {
            p_result = p_mode_data->p_infos[key_size_index];
            if (p_result == NULL)
            {
                nrf_cli_fprintf(cli_get(),
                                NRF_CLI_ERROR,
                                "Selected key size is not supported by this mode.\n");
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
static void subcmd_size_get(size_t idx, nrf_cli_static_entry_t * p_static)
{
    static char const * table[] =
    {
        "128",
        "160",
        "192",
        "224",
        "256",
        "384",
        "512",
        NULL,
    };
    p_static->p_syntax = table[idx];
    p_static->handler  = NULL;
    p_static->p_subcmd = NULL;
    p_static->p_help   = NULL;
}


NRF_CLI_CREATE_DYNAMIC_CMD(m_subcmd_var_size, subcmd_size_get);
VAR_REGISTER_SUB_COMMANDS(m_subcmd_var_var_size, &m_subcmd_var_size);
VAR_REGISTER_SUB_COMMANDS(m_subcmd_var_var_var_size, &m_subcmd_var_var_size);
VAR_REGISTER_SUB_COMMANDS(m_subcmd_var_var_var_var_size, &m_subcmd_var_var_var_size);
CLI_CREATE_DYNAMIC_CMD_WITH_SUBCMD(m_subcmd_mode_var_var_var_var_size,
                                   subcmd_aes_mode_get,
                                   &m_subcmd_var_var_var_var_size);

VAR_REGISTER_SUB_COMMANDS(m_subcmd_var, NULL);
VAR_REGISTER_SUB_COMMANDS(m_subcmd_var_var, &m_subcmd_var);
VAR_REGISTER_SUB_COMMANDS(m_subcmd_var_var_var, &m_subcmd_var_var);
VAR_REGISTER_SUB_COMMANDS(m_subcmd_var_var_var_var, &m_subcmd_var_var_var);
VAR_REGISTER_SUB_COMMANDS(m_subcmd_var_var_var_var_var, &m_subcmd_var_var_var_var);
CLI_CREATE_DYNAMIC_CMD_WITH_SUBCMD(m_subcmd_mode_var_var_var_var_var,
                                   subcmd_aes_mode_get,
                                   &m_subcmd_var_var_var_var_var);


/* =============================================================================================
 *                                        AEAD commands
 * =============================================================================================
 */


/** @brief Command to do AEAD encrypt. See command help for details.
 */
static bool cmd_aead_encrypt(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                     ret;
    nrf_crypto_aead_info_t const * p_info;

    UNUSED_PARAMETER(p_cli);

    CLI_ARGS_CHECK(argc == 9);

    var_t * p_key_var = cli_input_arg_get(2, &g_raw_type);
    CLI_CHECK(p_key_var != NULL);

    p_info        = aead_info_get(argv[1], p_key_var->data_size);
    CLI_CHECK(p_info);

    var_t * p_nonce_var = cli_input_arg_get(3, &g_raw_type);
    CLI_CHECK(p_nonce_var);

    var_t * p_adata_var = cli_input_arg_get(4, &g_raw_type);
    CLI_CHECK(p_adata_var);

    var_t * p_data_in_var = cli_input_arg_get(5, &g_raw_type);
    CLI_CHECK(p_data_in_var);

    size_t mac_size = 0;

    char * p_arg_end = NULL;
    mac_size = strtol(argv[6], &p_arg_end, 0);
    CLI_ARGS_CHECK(p_arg_end[0] == '\0');

    var_t * p_mac_var = cli_output_arg_get(7, &g_raw_type, mac_size);
    CLI_CHECK(p_mac_var);

    var_t * p_data_out_var = cli_output_arg_get(8, &g_raw_type, p_data_in_var->data_size);
    CLI_CHECK(p_data_out_var);

    nrf_crypto_aead_context_t * p_ctx =
        (nrf_crypto_aead_context_t *)var_temp_alloc(sizeof(nrf_crypto_aead_context_t));
    CLI_CHECK(p_ctx != NULL);

    ret = nrf_crypto_aead_init(p_ctx, p_info, VAR_DATA(p_key_var));
    CLI_ERROR_CHECK(ret);

    ret = nrf_crypto_aead_crypt(p_ctx,
                                NRF_CRYPTO_ENCRYPT,
                                VAR_DATA(p_nonce_var),
                                p_nonce_var->data_size,
                                VAR_DATA(p_adata_var),
                                p_adata_var->data_size,
                                VAR_DATA(p_data_in_var),
                                p_data_in_var->data_size,
                                VAR_DATA(p_data_out_var),
                                VAR_DATA(p_mac_var),
                                p_mac_var->data_size);

    (void)nrf_crypto_aead_uninit(p_ctx);

    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(aead_encrypt, &m_subcmd_mode_var_var_var_var_size, cmd_aead_encrypt,
    "Calculate AEAD in one step.\n"
    "Usage:\n"
    "  aead_encrypt mode key nonce adata data_in mac_size mac data_out\n"
    "Arguments:\n"
    "  IN  mode      AES block cipher mode (ccm, ccm*, eax, gcm, chacha-poly)\n"
    "  IN  key       AES key (must be 128, 192 or 256 bit long)\n"
    "  IN  nonce     Nonce\n"
    "  IN  adata     Additional authenticated data\n"
    "  IN  data_in   Data to process\n"
    "  IN  mac_size  Expected size of the MAC\n"
    "  OUT mac       Resulting MAC\n"
    "  OUT data_out  Resulting data\n"
    );


/** @brief Command to do AEAD decrypt. See command help for details.
 */
static bool cmd_aead_decrypt(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                     ret;
    nrf_crypto_aead_info_t const * p_info;

    UNUSED_PARAMETER(p_cli);

    CLI_ARGS_CHECK(argc == 8);

    var_t * p_key_var = cli_input_arg_get(2, &g_raw_type);
    CLI_CHECK(p_key_var != NULL);

    p_info        = aead_info_get(argv[1], p_key_var->data_size);
    CLI_CHECK(p_info);

    var_t * p_nonce_var = cli_input_arg_get(3, &g_raw_type);
    CLI_CHECK(p_nonce_var);

    var_t * p_adata_var = cli_input_arg_get(4, &g_raw_type);
    CLI_CHECK(p_adata_var);

    var_t * p_data_in_var = cli_input_arg_get(5, &g_raw_type);
    CLI_CHECK(p_data_in_var);

    var_t * p_mac_var = cli_input_arg_get(6, &g_raw_type);
    CLI_CHECK(p_mac_var);

    var_t * p_data_out_var = cli_output_arg_get(7, &g_raw_type, p_data_in_var->data_size);
    CLI_CHECK(p_data_out_var);

    nrf_crypto_aead_context_t * p_ctx =
        (nrf_crypto_aead_context_t *)var_temp_alloc(sizeof(nrf_crypto_aead_context_t));
    CLI_CHECK(p_ctx != NULL);

    ret = nrf_crypto_aead_init(p_ctx, p_info, VAR_DATA(p_key_var));
    CLI_ERROR_CHECK(ret);

    ret = nrf_crypto_aead_crypt(p_ctx,
                                NRF_CRYPTO_DECRYPT,
                                VAR_DATA(p_nonce_var),
                                p_nonce_var->data_size,
                                VAR_DATA(p_adata_var),
                                p_adata_var->data_size,
                                VAR_DATA(p_data_in_var),
                                p_data_in_var->data_size,
                                VAR_DATA(p_data_out_var),
                                VAR_DATA(p_mac_var),
                                p_mac_var->data_size);

    (void)nrf_crypto_aead_uninit(p_ctx);

    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(aead_decrypt, &m_subcmd_mode_var_var_var_var_var, cmd_aead_decrypt,
    "Calculate AEAD in one step.\n"
    "Usage:\n"
    "  aead_decrypt mode key nonce adata data_in mac_size mac data_out\n"
    "Arguments:\n"
    "  IN  mode      AES block cipher mode (ccm, ccm*, eax, gcm, chacha-poly)\n"
    "  IN  key       AES key (must be 128, 192 or 256 bit long)\n"
    "  IN  nonce     Nonce\n"
    "  IN  adata     Additional authenticated data\n"
    "  IN  data_in   Data to process\n"
    "  IN  mac       MAC to check\n"
    "  OUT data_out  Resulting data\n"
    );
