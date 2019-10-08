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

#include <stdint.h>
#include <stdbool.h>

#include "nrf_cli.h"
#include "vars.h"
#include "raw.h"
#include "cli_utils.h"
#include "nrf_crypto_hash.h"
#include "nrf_crypto_hmac.h"
#include "nrf_crypto_hkdf.h"


/* =============================================================================================
 *                           HMAC name utility functions
 * =============================================================================================
 */


/** @brief Gets info structure from provided hash name.
 *
 * @param p_name  Name of the HMAC algorithm.
 * @return        Pointer to info structure, or NULL if hash is not available.
 */
static nrf_crypto_hmac_info_t const * hmac_info_get(char const * p_name)
{
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HMAC_SHA256)
    if (strcmp(p_name, "sha256") == 0)
    {
        return &g_nrf_crypto_hmac_sha256_info;
    }
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HMAC_SHA512)
    if (strcmp(p_name, "sha512") == 0)
    {
        return &g_nrf_crypto_hmac_sha512_info;
    }
#endif

    nrf_cli_fprintf(cli_get(),
                    NRF_CLI_WARNING,
                    "Invalid name '%s'. Available hash algorithms for HMAC: \n",
                    p_name);

#if NRF_MODULE_ENABLED(NRF_CRYPTO_HMAC_SHA256)
    nrf_cli_fprintf(cli_get(), NRF_CLI_WARNING, "  sha256\n");
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HMAC_SHA512)
    nrf_cli_fprintf(cli_get(), NRF_CLI_WARNING, "  sha512\n");
#endif

    return NULL;
}


/* =============================================================================================
 *                           Sub commands used in arguments
 * =============================================================================================
 */


/** @brief Subcommand handler to list all available HMAC algorithms.
 */
static void subcmd_hmac_get(size_t idx, nrf_cli_static_entry_t * p_static)
{
    static char const * table[] =
    {
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HMAC_SHA256)
        "sha256",
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HMAC_SHA512)
        "sha512",
#endif
        NULL,
    };
    p_static->p_syntax = table[idx];
    p_static->handler  = NULL;
    p_static->p_subcmd = NULL;
    p_static->p_help   = NULL;
}


/** @brief Subcommand handler to list all available HMAC modes.
 */
static void subcmd_mode_get(size_t idx, nrf_cli_static_entry_t * p_static)
{
    static char const * table[] =
    {
        "expand_only",
        "normal",
        NULL,
    };
    p_static->p_syntax = table[idx];
    p_static->handler  = NULL;
    p_static->p_subcmd = NULL;
    p_static->p_help   = NULL;
}


/** @brief Subcommand handler to list suggested key sizes.
 */
static void subcmd_key_size_get(size_t idx, nrf_cli_static_entry_t * p_static)
{
    static char const * table[] =
    {
        "128bit",
        "192bit",
        "256bit",
        NULL,
    };
    p_static->p_syntax = table[idx];
    p_static->handler  = NULL;
    p_static->p_subcmd = NULL;
    p_static->p_help   = NULL;
}


VAR_REGISTER_SUB_COMMANDS(m_subcmd_var, NULL);
VAR_REGISTER_SUB_COMMANDS(m_subcmd_var_var, &m_subcmd_var);
CLI_CREATE_DYNAMIC_CMD_WITH_SUBCMD(m_subcmd_hmac_var, subcmd_hmac_get, &m_subcmd_var);
NRF_CLI_CREATE_DYNAMIC_CMD(m_subcmd_hmac, subcmd_hmac_get);
NRF_CLI_CREATE_DYNAMIC_CMD(m_subcmd_key_size, subcmd_key_size_get);
VAR_REGISTER_SUB_COMMANDS(m_subcmd_var_key_size, &m_subcmd_key_size);
VAR_REGISTER_SUB_COMMANDS(m_subcmd_var_var_key_size, &m_subcmd_var_key_size);
VAR_REGISTER_SUB_COMMANDS(m_subcmd_var_var_var_key_size, &m_subcmd_var_var_key_size);
CLI_CREATE_DYNAMIC_CMD_WITH_SUBCMD(m_subcmd_mode_var_var_var_key_size,
                                   subcmd_mode_get,
                                   &m_subcmd_var_var_var_key_size);
CLI_CREATE_DYNAMIC_CMD_WITH_SUBCMD(m_subcmd_hmac_mode_var_var_var_key_size,
                                   subcmd_hmac_get,
                                   &m_subcmd_mode_var_var_var_key_size);


/* =============================================================================================
 *                               HMAC CLI type definition
 * =============================================================================================
 */


/** @brief Returns true if HMAC context in p_var is already finalized.
 */
static bool context_is_finalized(var_t * p_var)
{
    size_t  i;
    uint8_t is_ready = 0;
    for (i = 0; i < p_var->data_size; i++)
    {
        is_ready |= VAR_DATA(p_var)[i];
    }
    return !is_ready;
}


/** @brief Marks that context in p_var is already finalized.
 */
static void context_finalize(var_t * p_var)
{
    memset(VAR_DATA(p_var), 0, p_var->data_size);
}


/** @brief Callback function for deallocating HMAC context variables.
 */
static bool hmac_context_free(var_t * p_var)
{
    ret_code_t                      ret;
    nrf_crypto_hash_sha512_digest_t digest;
    size_t                          digest_size = sizeof(digest);

    if (!context_is_finalized(p_var))
    {
        ret = nrf_crypto_hmac_finalize((nrf_crypto_hmac_context_t *)VAR_DATA(p_var),
                                       digest,
                                       &digest_size);
        context_finalize(p_var);
        cli_error_print(ret);
    }

    return true;
}


/** @brief Callback function for printing HMAC context variables.
 */
static void hmac_context_print(var_t * p_var, bool short_info)
{
    if (short_info)
    {
        nrf_cli_fprintf(cli_get(),
                        NRF_CLI_NORMAL,
                        context_is_finalized(p_var) ? "Finalized.\n" : "Ready.\n");
    }
}


/** @brief Variable type for HMAC contexts.
 */
static const var_type_t m_hmac_context_type =
{
    .p_description = "HMAC context",
    .free_fn       = hmac_context_free,
    .print_fn      = hmac_context_print,
    .p_tag         = NULL,
};


/* =============================================================================================
 *                                        HMAC commands
 * =============================================================================================
 */


/** @brief Command to initialize HMAC context. See command help for details.
 */
static bool cmd_hmac_init(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                     ret;
    nrf_crypto_hmac_info_t const * p_info;
    var_t                        * p_context_var;
    var_t                        * p_key_var;

    UNUSED_PARAMETER(p_cli);

    CLI_ARGS_CHECK(argc == 4);

    p_info        = hmac_info_get(argv[1]);
    p_key_var     = cli_input_arg_get(2, &g_raw_type);
    p_context_var = cli_output_arg_get(3, &m_hmac_context_type, sizeof(nrf_crypto_hmac_context_t));
    CLI_CHECK((p_info != NULL) && (p_key_var != NULL) && (p_context_var != NULL));

    ret = nrf_crypto_hmac_init((nrf_crypto_hmac_context_t *)VAR_DATA(p_context_var),
                               p_info,
                               VAR_DATA(p_key_var),
                               p_key_var->data_size);
    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(hmac_init, &m_subcmd_hmac, cmd_hmac_init,
    "Initialize HMAC context.\n"
    "Usage:\n"
    "  hmac_init hash_type key context\n"
    "Arguments:\n"
    "  IN  hmac_type  Algorithm used to calculate HMAC (sha256 or sha512)\n"
    "  IN  key        Key data\n"
    "  OUT context    Newly initialized context\n"
    );


/** @brief Command to update HMAC. See command help for details.
 */
static bool cmd_hmac_update(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t   ret;
    var_t      * p_context_var;
    var_t      * p_data_var;

    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK(argc == 3);

    p_context_var = cli_input_arg_get(1, &m_hmac_context_type);
    p_data_var    = cli_input_arg_get(2, &g_raw_type);
    CLI_CHECK((p_context_var != NULL) && (p_data_var != NULL));

    ret = nrf_crypto_hmac_update((nrf_crypto_hmac_context_t *)VAR_DATA(p_context_var),
                                 VAR_DATA(p_data_var),
                                 p_data_var->data_size);
    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(hmac_update, &m_subcmd_var_var, cmd_hmac_update,
    "Update HMAC calculation with a new data.\n"
    "Usage:\n"
    "  hmac_update context data\n"
    "Arguments:\n"
    "  IN  context  HMAC context to update\n"
    "  IN  data     Data input\n"
    );


/** @brief Command to finalize HMAC. See command help for details.
 */
static bool cmd_hmac_finalize(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                       ret;
    var_t                          * p_context_var;
    var_t                          * p_digest_var;
    nrf_crypto_hash_sha512_digest_t  digest;
    size_t                           digest_size = sizeof(digest);

    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK(argc == 3);

    p_context_var = cli_input_arg_get(1, &m_hmac_context_type);
    CLI_CHECK(p_context_var != NULL);

    ret = nrf_crypto_hmac_finalize((nrf_crypto_hmac_context_t *)VAR_DATA(p_context_var),
                                   digest,
                                   &digest_size);
    context_finalize(p_context_var);
    CLI_ERROR_CHECK(ret);

    p_digest_var = cli_output_arg_get(2, &g_raw_type, digest_size);
    CLI_CHECK(p_digest_var != NULL);
    memcpy(VAR_DATA(p_digest_var), digest, p_digest_var->data_size);

    return true;
}


CLI_CMD_REGISTER(hmac_finalize, &m_subcmd_var, cmd_hmac_finalize,
    "Finalize HAMC calculation.\n"
    "Usage:\n"
    "  hmac_finalize context digest\n"
    "Arguments:\n"
    "  IN  context  HMAC context to finalize\n"
    "  OUT digest   Calculated digest\n"
    );


/** @brief Command to calculate HMAC. See command help for details.
 */
static bool cmd_hmac_calculate(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                       ret;
    nrf_crypto_hmac_info_t   const * p_info;
    var_t                          * p_key_var;
    var_t                          * p_digest_var;
    var_t                          * p_data_var;
    nrf_crypto_hash_sha512_digest_t  digest;
    size_t                           digest_size = sizeof(digest);

    UNUSED_PARAMETER(p_cli);

    CLI_ARGS_CHECK(argc == 5);

    p_info     = hmac_info_get(argv[1]);
    p_key_var  = cli_input_arg_get(2, &g_raw_type);
    p_data_var = cli_input_arg_get(3, &g_raw_type);
    CLI_CHECK((p_info != NULL) && (p_key_var != NULL) && (p_data_var != NULL));

    ret = nrf_crypto_hmac_calculate(NULL,
                                    p_info,
                                    digest,
                                    &digest_size,
                                    VAR_DATA(p_key_var),
                                    p_key_var->data_size,
                                    VAR_DATA(p_data_var),
                                    p_data_var->data_size);
    CLI_ERROR_CHECK(ret);

    p_digest_var = cli_output_arg_get(4, &g_raw_type, digest_size);
    CLI_CHECK(p_digest_var != NULL);
    memcpy(VAR_DATA(p_digest_var), digest, digest_size);

    return true;
}


CLI_CMD_REGISTER(hmac_calculate, &m_subcmd_hmac_var, cmd_hmac_calculate,
    "Calculate HMAC in one step.\n"
    "Usage:\n"
    "  hmac_calculate hash_type key data digest\n"
    "Arguments:\n"
    "  IN  hmac_type  Algorithm used to calculate hmac (sha256 or sha512)\n"
    "  IN  key        Key data\n"
    "  IN  data       Data input\n"
    "  OUT digest     Calculated digest\n"
    );


/* =============================================================================================
 *                                        HKDF commands
 * =============================================================================================
 */


/** @brief Command to calculate HKDF. See command help for details.
 */
static bool cmd_hkdf_calculate(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                       ret;
    nrf_crypto_hmac_info_t   const * p_info;
    var_t                          * p_input_key_var;
    var_t                          * p_output_key_var;
    void                           * p_salt;
    size_t                           salt_size;
    void                           * p_ainfo;
    size_t                           ainfo_size;
    char                           * p_arg_end;
    nrf_crypto_hkdf_mode_t           mode = NRF_CRYPTO_HKDF_EXTRACT_AND_EXPAND;
    size_t                           output_key_size;

    UNUSED_PARAMETER(p_cli);

    CLI_ARGS_CHECK(argc == 8);

    p_info          = hmac_info_get(argv[1]);

    if (strcmp(argv[2], "normal") == 0)
    {
        mode = NRF_CRYPTO_HKDF_EXTRACT_AND_EXPAND;
    }
    else if (strcmp(argv[2], "expand_only") == 0)
    {
        mode = NRF_CRYPTO_HKDF_EXPAND_ONLY;
    }
    else
    {
        CLI_ARGS_CHECK(false);
    }

    p_input_key_var = cli_input_arg_get(3, &g_raw_type);

    CLI_CHECK((p_info != NULL) && (p_input_key_var != NULL));

    if (strcmp(argv[4], "-") != 0)
    {
        var_t * p_salt_var = cli_input_arg_get(4, &g_raw_type);
        CLI_CHECK(p_salt_var);
        p_salt = VAR_DATA(p_salt_var);
        salt_size = p_salt_var->data_size;
    }
    else
    {
        p_salt = NULL;
        salt_size = 0;
    }
    if (strcmp(argv[5], "-") != 0)
    {
        var_t * p_ainfo_var = cli_input_arg_get(5, &g_raw_type);
        CLI_CHECK(p_ainfo_var);
        p_ainfo = VAR_DATA(p_ainfo_var);
        ainfo_size = p_ainfo_var->data_size;
    }
    else
    {
        p_ainfo = NULL;
        ainfo_size = 0;
    }

    output_key_size = (size_t)strtol(argv[6], &p_arg_end, 0);
    if ((strcmp(p_arg_end, "bits") == 0) || (strcmp(p_arg_end, "bit") == 0))
    {
        if ((output_key_size % 8) != 0)
        {
            nrf_cli_fprintf(cli_get(), NRF_CLI_ERROR, "Number of bits must be multiple of 8.\n");
            return false;
        }
        output_key_size = output_key_size / 8;
    }
    else
    {
        CLI_ARGS_CHECK(p_arg_end[0] == '\0');
    }

    p_output_key_var = cli_output_arg_get(7, &g_raw_type, output_key_size);
    CLI_CHECK(p_output_key_var != NULL);

    ret = nrf_crypto_hkdf_calculate(NULL,
                                    p_info,
                                    VAR_DATA(p_output_key_var),
                                    &output_key_size,
                                    VAR_DATA(p_input_key_var),
                                    p_input_key_var->data_size,
                                    p_salt,
                                    salt_size,
                                    p_ainfo,
                                    ainfo_size,
                                    mode);
    CLI_ERROR_CHECK(ret);

    p_output_key_var->data_size = output_key_size;

    return true;
}


CLI_CMD_REGISTER(hkdf_calculate, &m_subcmd_hmac_mode_var_var_var_key_size, cmd_hkdf_calculate,
    "Calculate HKDF.\n"
    "Usage:\n"
    "  hkdf_calculate hash_type mode input_key salt ainfo output_key_size output_key\n"
    "Arguments:\n"
    "  IN  hmac_type  Algorithm used to calculate HKDF (sha256 or sha512)\n"
    "  IN  mode       HKDF mode (normal or expand_only)\n"
    "  IN  input_key  Key\n"
    "  IN  salt       Salt data or \"-\"\n"
    "  IN  ainfo      Application specific information or \"-\"\n"
    "  IN  key_size   Output key size in bytes or bits if \"bit\" postfix is added\n"
    "  OUT key        Calculated key\n"
    );

