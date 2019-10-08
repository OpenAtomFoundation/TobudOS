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


/* =============================================================================================
 *                           Hash name utility functions
 * =============================================================================================
 */


/** @brief Gets info structure from the provided hash name.
 *
 * @param p_name  Name of the hash algorithm.
 * @return        Pointer to the info structure, or NULL if hash is not available.
 */
static nrf_crypto_hash_info_t const * hash_info_get(char const * p_name)
{
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA256)
    if (strcmp(p_name, "sha256") == 0)
    {
        return &g_nrf_crypto_hash_sha256_info;
    }
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA512)
    if (strcmp(p_name, "sha512") == 0)
    {
        return &g_nrf_crypto_hash_sha512_info;
    }
#endif

    nrf_cli_fprintf(cli_get(),
                    NRF_CLI_WARNING,
                    "Invalid name '%s'. Available hash algorithms: \n",
                    p_name);

#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA256)
    nrf_cli_fprintf(cli_get(), NRF_CLI_WARNING, "  sha256\n");
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA512)
    nrf_cli_fprintf(cli_get(), NRF_CLI_WARNING, "  sha512\n");
#endif

    return NULL;
}


/* =============================================================================================
 *                           Sub commands used in arguments
 * =============================================================================================
 */


/** @brief Subcommand handler to list all available hash algorithms.
 */
static void subcmd_hash_get(size_t idx, nrf_cli_static_entry_t * p_static)
{
    static char const * table[] =
    {
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA256)
        "sha256",
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA512)
        "sha512",
#endif
        NULL,
    };
    p_static->p_syntax = table[idx];
    p_static->handler  = NULL;
    p_static->p_subcmd = NULL;
    p_static->p_help   = NULL;
}


VAR_REGISTER_SUB_COMMANDS(m_subcmd_var, NULL);
VAR_REGISTER_SUB_COMMANDS(m_subcmd_var_var, &m_subcmd_var);
CLI_CREATE_DYNAMIC_CMD_WITH_SUBCMD(m_subcmd_hash_var, subcmd_hash_get, &m_subcmd_var);
NRF_CLI_CREATE_DYNAMIC_CMD(m_subcmd_hash, subcmd_hash_get);


/* =============================================================================================
 *                               Hash CLI type definition
 * =============================================================================================
 */


/** @brief Returns true if hash context in p_var was already finalized.
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


/** @brief Callback function for deallocating hash context variables.
 */
static bool hash_context_free(var_t * p_var)
{
    ret_code_t                      ret;
    nrf_crypto_hash_sha512_digest_t digest;
    size_t                          digest_size = sizeof(digest);

    if (!context_is_finalized(p_var))
    {
        ret = nrf_crypto_hash_finalize((nrf_crypto_hash_context_t *)VAR_DATA(p_var),
                                       digest,
                                       &digest_size);
        context_finalize(p_var);
        cli_error_print(ret);
    }

    return true;
}


/** @brief Callback function for printing hash context variables.
 */
static void hash_context_print(var_t * p_var, bool short_info)
{
    if (short_info)
    {
        nrf_cli_fprintf(cli_get(),
                        NRF_CLI_NORMAL,
                        context_is_finalized(p_var) ? "Finalized.\n" : "Ready.\n");
    }
}


/** @brief Variable type for hash contexts.
 */
static const var_type_t m_hash_context_type =
{
    .p_description = "Hash context",
    .free_fn       = hash_context_free,
    .print_fn      = hash_context_print,
    .p_tag         = NULL,
};


/* =============================================================================================
 *                                        Hash commands
 * =============================================================================================
 */


/** @brief Command to initialize hash context. See command help for details.
 */
static bool cmd_hash_init(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                     ret;
    nrf_crypto_hash_info_t const * p_info;
    var_t                        * p_context_var;

    UNUSED_PARAMETER(p_cli);

    CLI_ARGS_CHECK(argc == 3);

    p_info        = hash_info_get(argv[1]);
    p_context_var = cli_output_arg_get(2, &m_hash_context_type, sizeof(nrf_crypto_hash_context_t));
    CLI_CHECK((p_info != NULL) && (p_context_var != NULL));

    ret = nrf_crypto_hash_init((nrf_crypto_hash_context_t *)VAR_DATA(p_context_var), p_info);
    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(hash_init, &m_subcmd_hash, cmd_hash_init,
    "Initialize hash context.\n"
    "Usage:\n"
    "  hash_init hash_type context\n"
    "Arguments:\n"
    "  IN  hash_type  Algorithm used to calculate hash (sha256 or sha512)\n"
    "  OUT context    Newly initialized context\n"
    );


/** @brief Command to update hash. See command help for details.
 */
static bool cmd_hash_update(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t   ret;
    var_t      * p_context_var;
    var_t      * p_data_var;

    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK(argc == 3);

    p_context_var = cli_input_arg_get(1, &m_hash_context_type);
    p_data_var    = cli_input_arg_get(2, &g_raw_type);
    CLI_CHECK((p_context_var != NULL) && (p_data_var != NULL));

    ret = nrf_crypto_hash_update((nrf_crypto_hash_context_t *)VAR_DATA(p_context_var),
                                 VAR_DATA(p_data_var),
                                 p_data_var->data_size);
    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(hash_update, &m_subcmd_var_var, cmd_hash_update,
    "Update hash function with new data.\n"
    "Usage:\n"
    "  hash_update context data\n"
    "Arguments:\n"
    "  IN  context  Hash context to update\n"
    "  IN  data     Data input\n"
    );


/** @brief Command to finalize hash calculation. See command help for details.
 */
static bool cmd_hash_finalize(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                       ret;
    var_t                          * p_context_var;
    var_t                          * p_digest_var;
    nrf_crypto_hash_sha512_digest_t  digest;
    size_t                           digest_size = sizeof(digest);

    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK(argc == 3);

    p_context_var = cli_input_arg_get(1, &m_hash_context_type);
    CLI_CHECK(p_context_var != NULL);

    ret = nrf_crypto_hash_finalize((nrf_crypto_hash_context_t *)VAR_DATA(p_context_var),
                                   digest,
                                   &digest_size);
    context_finalize(p_context_var);
    CLI_ERROR_CHECK(ret);

    p_digest_var = cli_output_arg_get(2, &g_raw_type, digest_size);
    CLI_CHECK(p_digest_var != NULL);
    memcpy(VAR_DATA(p_digest_var), digest, p_digest_var->data_size);

    return true;
}


CLI_CMD_REGISTER(hash_finalize, &m_subcmd_var, cmd_hash_finalize,
    "Finalize hash calculation.\n"
    "Usage:\n"
    "  hash_finalize context digest\n"
    "Arguments:\n"
    "  IN  context  Hash context to finalize\n"
    "  OUT digest   Calculated digest\n"
    );


/** @brief Command to calculate hash. See command help for details.
 */
static bool cmd_hash_calculate(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                       ret;
    nrf_crypto_hash_info_t   const * p_info;
    nrf_crypto_hash_context_t      * p_context;
    var_t                          * p_digest_var;
    var_t                          * p_data_var;
    nrf_crypto_hash_sha512_digest_t  digest;
    size_t                           digest_size = sizeof(digest);

    UNUSED_PARAMETER(p_cli);

    CLI_ARGS_CHECK(argc == 4);

    p_info     = hash_info_get(argv[1]);
    p_data_var = cli_input_arg_get(2, &g_raw_type);
    CLI_CHECK((p_info != NULL) && (p_data_var != NULL));

    p_context = (nrf_crypto_hash_context_t *)var_temp_alloc(sizeof(nrf_crypto_hash_context_t));
    CLI_CHECK(p_context != NULL);

    ret = nrf_crypto_hash_calculate(p_context, // NRF_CRYPTO_TODO: Replace by NULL when NULL pointer issue is fixed in hash frontend
                                    p_info,
                                    VAR_DATA(p_data_var),
                                    p_data_var->data_size,
                                    digest,
                                    &digest_size);
    CLI_ERROR_CHECK(ret);

    p_digest_var = cli_output_arg_get(3, &g_raw_type, digest_size);
    CLI_CHECK(p_digest_var != NULL);
    memcpy(VAR_DATA(p_digest_var), digest, digest_size);

    return true;
}


CLI_CMD_REGISTER(hash_calculate, &m_subcmd_hash_var, cmd_hash_calculate,
    "Calculate hash in one step.\n"
    "Usage:\n"
    "  hash_calculate hash_type data digest\n"
    "Arguments:\n"
    "  IN  hash_type  Algorithm used to calculate hash (sha256 or sha512)\n"
    "  IN  data       Data input\n"
    "  OUT digest     Calculated digest\n"
    );


/* =============================================================================================
 *                               Hash converters
 * =============================================================================================
 */


/** @brief Callback function used to convert raw data to its digest.
 */
static bool hash_calculate_conv(struct cli_converter_s const * p_converter,
                                var_t                        * p_input,
                                var_t                       ** pp_output)
{
    ret_code_t                        ret;
    nrf_crypto_hash_context_t       * p_context;
    nrf_crypto_hash_info_t    const * p_info = (nrf_crypto_hash_info_t const *)p_converter->p_tag;
    nrf_crypto_hash_sha512_digest_t   digest;
    size_t                            digest_size = sizeof(digest);

    p_context = (nrf_crypto_hash_context_t *)var_temp_alloc(sizeof(nrf_crypto_hash_context_t));
    CLI_CHECK(p_context != NULL);

    ret = nrf_crypto_hash_calculate(p_context, // NRF_CRYPTO_TODO: Replace by NULL when NULL pointer issue is fixed in hash frontend
                                    p_info,
                                    VAR_DATA(p_input),
                                    p_input->data_size,
                                    digest,
                                    &digest_size);
    CLI_ERROR_CHECK(ret);

    *pp_output = var_temp_create(&g_raw_type, digest_size, true);
    CLI_CHECK(*pp_output != NULL);
    memcpy(VAR_DATA(*pp_output), digest, digest_size);

    return true;
}


/*lint -esym(528,sha256_converter) */
/*lint -esym(528,sha512_converter) */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA256)
CLI_CONVERTER_REGISTER(sha256_converter,
                 "sha256",
                 &g_raw_type,
                 &g_raw_type,
                 hash_calculate_conv,
                 NULL,
                 (void *)&g_nrf_crypto_hash_sha256_info);
#endif

#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA512)
CLI_CONVERTER_REGISTER(sha512_converter,
                 "sha512",
                 &g_raw_type,
                 &g_raw_type,
                 hash_calculate_conv,
                 NULL,
                 (void *)&g_nrf_crypto_hash_sha512_info);
#endif
