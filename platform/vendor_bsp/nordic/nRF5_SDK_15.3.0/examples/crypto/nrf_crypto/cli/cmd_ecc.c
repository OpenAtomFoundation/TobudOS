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
#include "nrf_crypto_ecc.h"
#include "nrf_crypto_ecdh.h"
#include "nrf_crypto_ecdsa.h"
#include "nrf_crypto_eddsa.h"


/* =============================================================================================
 *                           Curve name utility functions
 * =============================================================================================
 */


typedef struct
{
    nrf_crypto_ecc_curve_info_t const * p_info;
    char                        const * p_name;
} curve_info_name_t;


static const curve_info_name_t m_curve_info_name_table[] =
{
#if NRF_CRYPTO_ECC_BP256R1_ENABLED
    { &g_nrf_crypto_ecc_bp256r1_curve_info, "bp256r1" },
#endif
#if NRF_CRYPTO_ECC_BP384R1_ENABLED
    { &g_nrf_crypto_ecc_bp384r1_curve_info, "bp384r1" },
#endif
#if NRF_CRYPTO_ECC_BP512R1_ENABLED
    { &g_nrf_crypto_ecc_bp512r1_curve_info, "bp512r1" },
#endif
#if NRF_CRYPTO_ECC_CURVE25519_ENABLED
    { &g_nrf_crypto_ecc_curve25519_curve_info, "curve25519" },
#endif
#if NRF_CRYPTO_ECC_ED25519_ENABLED
    { &g_nrf_crypto_ecc_ed25519_curve_info, "ed25519" },
#endif
#if NRF_CRYPTO_ECC_SECP160K1_ENABLED
    { &g_nrf_crypto_ecc_secp160k1_curve_info, "secp160k1" },
#endif
#if NRF_CRYPTO_ECC_SECP160R1_ENABLED
    { &g_nrf_crypto_ecc_secp160r1_curve_info, "secp160r1" },
#endif
#if NRF_CRYPTO_ECC_SECP160R2_ENABLED
    { &g_nrf_crypto_ecc_secp160r2_curve_info, "secp160r2" },
#endif
#if NRF_CRYPTO_ECC_SECP192K1_ENABLED
    { &g_nrf_crypto_ecc_secp192k1_curve_info, "secp192k1" },
#endif
#if NRF_CRYPTO_ECC_SECP192R1_ENABLED
    { &g_nrf_crypto_ecc_secp192r1_curve_info, "secp192r1" },
#endif
#if NRF_CRYPTO_ECC_SECP224K1_ENABLED
    { &g_nrf_crypto_ecc_secp224k1_curve_info, "secp224k1" },
#endif
#if NRF_CRYPTO_ECC_SECP224R1_ENABLED
    { &g_nrf_crypto_ecc_secp224r1_curve_info, "secp224r1" },
#endif
#if NRF_CRYPTO_ECC_SECP256K1_ENABLED
    { &g_nrf_crypto_ecc_secp256k1_curve_info, "secp256k1" },
#endif
#if NRF_CRYPTO_ECC_SECP256R1_ENABLED
    { &g_nrf_crypto_ecc_secp256r1_curve_info, "secp256r1" },
#endif
#if NRF_CRYPTO_ECC_SECP384R1_ENABLED
    { &g_nrf_crypto_ecc_secp384r1_curve_info, "secp384r1" },
#endif
#if NRF_CRYPTO_ECC_SECP521R1_ENABLED
    { &g_nrf_crypto_ecc_secp521r1_curve_info, "secp521r1" },
#endif
    { NULL, NULL },
};


static nrf_crypto_ecc_curve_info_t const * curve_info_get(char const * p_name)
{
    curve_info_name_t const * item = &m_curve_info_name_table[0];
    while (item->p_info)
    {
        if (strcmp(p_name, item->p_name) == 0)
        {
            return item->p_info;
        }
        item++;
    }
    nrf_cli_fprintf(cli_get(),
                    NRF_CLI_WARNING,
                    "Invalid curve name '%s'. Available curves:\n",
                    p_name);
    item = &m_curve_info_name_table[0];
    while (item->p_info)
    {
        nrf_cli_fprintf(cli_get(), NRF_CLI_INFO, "  %s\n", item->p_name);
        item++;
    }
    return NULL;
}


static char const * curve_name_get(nrf_crypto_ecc_curve_info_t const * p_info)
{
    curve_info_name_t const * item = &m_curve_info_name_table[0];
    while (item->p_info)
    {
        if (p_info == item->p_info)
        {
            return item->p_name;
        }
        item++;
    }
    return "INVALID";
}


/* =============================================================================================
 *                           Sub commands used in arguments
 * =============================================================================================
 */


static void subcmd_curve_get(size_t idx, nrf_cli_static_entry_t * p_static)
{
    p_static->p_syntax = m_curve_info_name_table[idx].p_name;
    p_static->handler  = NULL;
    p_static->p_subcmd = NULL;
    p_static->p_help   = NULL;
}


VAR_REGISTER_SUB_COMMANDS(m_subcmd_var, NULL);
VAR_REGISTER_SUB_COMMANDS(m_subcmd_var_var, &m_subcmd_var);
VAR_REGISTER_SUB_COMMANDS(m_subcmd_var_var_var, &m_subcmd_var_var);
CLI_CREATE_DYNAMIC_CMD_WITH_SUBCMD(m_subcmd_curve_var, subcmd_curve_get, &m_subcmd_var);
CLI_CREATE_DYNAMIC_CMD_WITH_SUBCMD(m_subcmd_curve_var_var, subcmd_curve_get, &m_subcmd_var_var);
NRF_CLI_CREATE_DYNAMIC_CMD(m_subcmd_curve, subcmd_curve_get);


/* =============================================================================================
 *                               ECC CLI types definition
 * =============================================================================================
 */


static void ecc_key_print(var_t * p_var, bool short_info)
{
    ret_code_t                          ret;
    char                        const * p_name;
    nrf_crypto_ecc_curve_info_t const * p_info = NULL;

    ret = nrf_crypto_ecc_curve_info_get(VAR_DATA(p_var), &p_info);
    cli_error_print(ret);
    p_name = curve_name_get(p_info);

    if (short_info)
    {
        nrf_cli_fprintf(cli_get(), NRF_CLI_NORMAL, "Curve %s.", p_name);
    }
}


static bool ecc_private_key_free(var_t * p_var)
{
    ret_code_t ret;
    ret = nrf_crypto_ecc_private_key_free((nrf_crypto_ecc_private_key_t *)VAR_DATA(p_var));
    cli_error_print(ret);
    return true;
}


static void ecc_private_key_print(var_t * p_var, bool short_info)
{
    ecc_key_print(p_var, short_info);

    if (!short_info)
    {
        ret_code_t                         ret;
        nrf_crypto_ecc_private_key_t     * p_pub;
        nrf_crypto_ecc_raw_private_key_t   raw;
        size_t                             size;

        p_pub = (nrf_crypto_ecc_private_key_t *)VAR_DATA(p_var);

        size = sizeof(raw);
        ret = nrf_crypto_ecc_private_key_to_raw(p_pub, raw, &size);

        if (ret == NRF_SUCCESS)
        {
            nrf_cli_fprintf(cli_get(), NRF_CLI_NORMAL, "Raw form:\n");
            raw_print_hex(raw, size);
        }
        else if (ret != NRF_ERROR_CRYPTO_FEATURE_UNAVAILABLE)
        {
            cli_error_print(ret);
        }
    }
}


static const var_type_t m_ecc_private_key_type =
{
    .p_description = "ECC Private Key",
    .free_fn       = ecc_private_key_free,
    .print_fn      = ecc_private_key_print,
    .p_tag         = NULL,
};


static bool ecc_public_key_free(var_t * p_var)
{
    ret_code_t ret;
    ret = nrf_crypto_ecc_public_key_free((nrf_crypto_ecc_public_key_t *)VAR_DATA(p_var));
    cli_error_print(ret);
    return true;
}


static void ecc_public_key_print(var_t * p_var, bool short_info)
{
    ecc_key_print(p_var, short_info);

    if (!short_info)
    {
        ret_code_t                        ret;
        nrf_crypto_ecc_public_key_t     * p_pub;
        nrf_crypto_ecc_raw_public_key_t   raw;
        size_t                            size;

        p_pub = (nrf_crypto_ecc_public_key_t *)VAR_DATA(p_var);

        size = sizeof(raw);
        ret = nrf_crypto_ecc_public_key_to_raw(p_pub, raw, &size);

        if (ret == NRF_SUCCESS)
        {
            nrf_cli_fprintf(cli_get(), NRF_CLI_NORMAL, "Raw form:\n");
            raw_print_hex(raw, size);
        }
        else if (ret != NRF_ERROR_CRYPTO_FEATURE_UNAVAILABLE)
        {
            cli_error_print(ret);
        }
    }
}


static const var_type_t m_ecc_public_key_type =
{
    .p_description = "ECC Public Key",
    .free_fn       = ecc_public_key_free,
    .print_fn      = ecc_public_key_print,
    .p_tag         = NULL,
};


/* =============================================================================================
 *                               ECC Key generation commands
 * =============================================================================================
 */


/** @brief Command to generate key pair. See command help for details.
 */
static bool cmd_ecc_key_pair_generate(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                           ret;
    var_t                              * p_prv_var;
    var_t                              * p_pub_var;
    nrf_crypto_ecc_curve_info_t  const * p_info;
    nrf_crypto_ecc_private_key_t       * p_prv;
    nrf_crypto_ecc_public_key_t        * p_pub;

    UNUSED_PARAMETER(p_cli);

    CLI_ARGS_CHECK(argc == 4);

    p_info = curve_info_get(argv[1]);
    CLI_CHECK(p_info != NULL);

    p_prv_var = cli_output_arg_get(2, &m_ecc_private_key_type, p_info->private_key_size);
    p_pub_var = cli_output_arg_get(3, &m_ecc_public_key_type, p_info->public_key_size);
    CLI_CHECK((p_prv_var != NULL) && (p_pub_var != NULL));

    p_prv = (nrf_crypto_ecc_private_key_t *)VAR_DATA(p_prv_var);
    p_pub = (nrf_crypto_ecc_public_key_t *)VAR_DATA(p_pub_var);

    ret = nrf_crypto_ecc_key_pair_generate(NULL, p_info, p_prv, p_pub);
    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(ecc_key_pair_generate, &m_subcmd_curve, cmd_ecc_key_pair_generate,
    "Generate ECC key pair.\n"
    "Usage:\n"
    "  ecc_key_pair_generate curve private_key public_key\n"
    "Arguments:\n"
    "  IN  curve        Curve to use.\n"
    "  IN  private_key  Source private key.\n"
    "  OUT public_key   Generated public key.\n"
    );


/** @brief Command to calculate public key from private key. See command help for details.
 */
static bool cmd_ecc_public_key_calculate(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                           ret;
    var_t                              * p_prv_var;
    var_t                              * p_pub_var;
    nrf_crypto_ecc_curve_info_t  const * p_info;
    nrf_crypto_ecc_private_key_t       * p_prv;
    nrf_crypto_ecc_public_key_t        * p_pub;

    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK(argc == 3);

    p_prv_var = cli_input_arg_get(1, &m_ecc_private_key_type);
    CLI_CHECK(p_prv_var != NULL);

    p_prv = (nrf_crypto_ecc_private_key_t *)VAR_DATA(p_prv_var);

    ret = nrf_crypto_ecc_curve_info_get(p_prv, &p_info);
    CLI_ERROR_CHECK(ret);

    p_pub_var = cli_output_arg_get(2, &m_ecc_public_key_type, p_info->public_key_size);
    CLI_CHECK(p_pub_var != NULL);

    p_pub = (nrf_crypto_ecc_public_key_t *)VAR_DATA(p_pub_var);

    ret = nrf_crypto_ecc_public_key_calculate(NULL, p_prv, p_pub);
    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(ecc_public_key_calculate, &m_subcmd_var, cmd_ecc_public_key_calculate,
    "Generate ECC public key from private key.\n"
    "Usage:\n"
    "  ecc_public_key_calculate private_key public_key\n"
    "Arguments:\n"
    "  IN  private_key  Source private key.\n"
    "  OUT public_key   Generated public key\n"
    );


/** @brief Command to list all ECC curves types. See command help for details.
 */
static bool cmd_ecc_curves(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    curve_info_name_t const * item = &m_curve_info_name_table[0];

    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK(argc == 1);

    while (item->p_info)
    {
        nrf_cli_fprintf(cli_get(), NRF_CLI_NORMAL, "%s\n", item->p_name);
        item++;
    }

    return true;
}


CLI_CMD_REGISTER(ecc_curves, NULL, cmd_ecc_curves,
    "Show all available ECC curve types.\n"
    "Usage:\n"
    "  ecc_curves\n"
    );


/* =============================================================================================
 *                               ECC Key conversion commands
 * =============================================================================================
 */


/** @brief Command to convert public key from raw data. See command help for details.
 */
static bool cmd_ecc_public_key_from_raw(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                          ret;
    nrf_crypto_ecc_curve_info_t const * p_info;
    var_t                             * p_raw_var;
    var_t                             * p_key_var;

    UNUSED_PARAMETER(p_cli);

    CLI_ARGS_CHECK(argc == 4);

    p_info = curve_info_get(argv[1]);
    CLI_CHECK(p_info != NULL);

    p_raw_var = cli_input_arg_get(2, &g_raw_type);
    p_key_var = cli_output_arg_get(3, &m_ecc_public_key_type, p_info->public_key_size);
    CLI_CHECK((p_raw_var != NULL) && (p_key_var != NULL));

    ret = nrf_crypto_ecc_public_key_from_raw(p_info,
                                             (nrf_crypto_ecc_public_key_t *)VAR_DATA(p_key_var),
                                             VAR_DATA(p_raw_var),
                                             p_raw_var->data_size);
    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(ecc_public_key_from_raw, &m_subcmd_curve_var, cmd_ecc_public_key_from_raw,
    "Create ECC public key from raw data.\n"
    "Usage:\n"
    "  ecc_public_key_from_raw curve raw_data public_key\n"
    "Arguments:\n"
    "  IN  curve        Curve name. Command 'ecc_curves' lists all supported curves.\n"
    "  IN  raw_data     Raw representation of the key\n"
    "  OUT public_key   Converted key\n"
    );


/** @brief Command to convert private key from raw data. See command help for details.
 */
static bool cmd_ecc_private_key_from_raw(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                          ret;
    nrf_crypto_ecc_curve_info_t const * p_info;
    var_t                             * p_raw_var;
    var_t                             * p_key_var;

    UNUSED_PARAMETER(p_cli);

    CLI_ARGS_CHECK(argc == 4);

    p_info = curve_info_get(argv[1]);
    CLI_CHECK(p_info != NULL);

    p_raw_var = cli_input_arg_get(2, &g_raw_type);
    p_key_var = cli_output_arg_get(3, &m_ecc_private_key_type, p_info->private_key_size);
    CLI_CHECK((p_raw_var != NULL) && (p_key_var != NULL));

    ret = nrf_crypto_ecc_private_key_from_raw(p_info,
                                              (nrf_crypto_ecc_private_key_t *)VAR_DATA(p_key_var),
                                              VAR_DATA(p_raw_var),
                                              p_raw_var->data_size);
    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(ecc_private_key_from_raw, &m_subcmd_curve_var, cmd_ecc_private_key_from_raw,
    "Create ECC private key from raw data.\n"
    "Usage:\n"
    "  ecc_private_key_from_raw curve raw_data private_key\n"
    "Arguments:\n"
    "  IN  curve        Curve name. Command 'ecc_curves' lists all supported curves.\n"
    "  IN  raw_data     Raw representation of the key\n"
    "  OUT private_key  Converted key\n"
    );


/** @brief Command to convert public key to raw data. See command help for details.
 */
static bool cmd_ecc_public_key_to_raw(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                           ret;
    var_t                              * p_raw_var;
    var_t                              * p_key_var;
    nrf_crypto_ecc_curve_info_t  const * p_info;
    nrf_crypto_ecc_public_key_t        * p_key;

    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK(argc == 3);

    p_key_var = cli_input_arg_get(1, &m_ecc_public_key_type);
    CLI_CHECK(p_key_var != NULL);

    p_key = (nrf_crypto_ecc_public_key_t *)VAR_DATA(p_key_var);

    ret = nrf_crypto_ecc_curve_info_get(p_key, &p_info);
    CLI_ERROR_CHECK(ret);

    p_raw_var = cli_output_arg_get(2, &g_raw_type, p_info->raw_public_key_size);
    CLI_CHECK(p_raw_var != NULL);

    ret = nrf_crypto_ecc_public_key_to_raw(p_key, VAR_DATA(p_raw_var), &p_raw_var->data_size);
    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(ecc_public_key_to_raw, &m_subcmd_var, cmd_ecc_public_key_to_raw,
    "Get raw data from ECC public key.\n"
    "Usage:\n"
    "  ecc_public_key_to_raw public_key raw_data\n"
    "Arguments:\n"
    "  IN  public_key   Key to convert\n"
    "  OUT raw_data     Raw representation of the key\n"
    );


/** @brief Command to convert private key to raw data. See command help for details.
 */
static bool cmd_ecc_private_key_to_raw(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                           ret;
    var_t                              * p_raw_var;
    var_t                              * p_key_var;
    nrf_crypto_ecc_curve_info_t  const * p_info;
    nrf_crypto_ecc_private_key_t       * p_key;

    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK(argc == 3);

    p_key_var = cli_input_arg_get(1, &m_ecc_private_key_type);
    CLI_CHECK(p_key_var != NULL);

    p_key = (nrf_crypto_ecc_private_key_t *)VAR_DATA(p_key_var);

    ret = nrf_crypto_ecc_curve_info_get(p_key, &p_info);
    CLI_ERROR_CHECK(ret);

    p_raw_var = cli_output_arg_get(2, &g_raw_type, p_info->raw_private_key_size);
    CLI_CHECK(p_raw_var != NULL);

    ret = nrf_crypto_ecc_private_key_to_raw(p_key, VAR_DATA(p_raw_var), &p_raw_var->data_size);
    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(ecc_private_key_to_raw, &m_subcmd_var, cmd_ecc_private_key_to_raw,
    "Get raw data from ECC private key.\n"
    "Usage:\n"
    "  ecc_private_key_to_raw private_key raw_data\n"
    "Arguments:\n"
    "  IN  private_key  Key to convert\n"
    "  OUT raw_data     Raw representation of the key\n"
    );


/* =============================================================================================
 *                                    ECDH Commands
 * =============================================================================================
 */


/** @brief Command to compute ECDH. See command help for details.
 */
static bool cmd_ecdh_compute(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                        ret;
    var_t                           * p_prv_var;
    var_t                           * p_pub_var;
    var_t                           * p_shared_secret_var;
    nrf_crypto_ecc_private_key_t    * p_prv;
    nrf_crypto_ecc_public_key_t     * p_pub;
    nrf_crypto_ecdh_shared_secret_t   shared_secret;
    size_t                            size;

    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK(argc == 4);

    p_prv_var = cli_input_arg_get(1, &m_ecc_private_key_type);
    p_pub_var = cli_input_arg_get(2, &m_ecc_public_key_type);
    CLI_CHECK((p_prv_var != NULL) && (p_pub_var != NULL));

    p_prv = (nrf_crypto_ecc_private_key_t *)VAR_DATA(p_prv_var);
    p_pub = (nrf_crypto_ecc_public_key_t *)VAR_DATA(p_pub_var);

    size = sizeof(shared_secret);
    ret  = nrf_crypto_ecdh_compute(NULL, p_prv, p_pub, shared_secret, &size);
    CLI_ERROR_CHECK(ret);

    p_shared_secret_var = cli_output_arg_get(3, &g_raw_type, size);
    CLI_CHECK(p_shared_secret_var != NULL);

    memcpy(VAR_DATA(p_shared_secret_var), shared_secret, size);

    return true;
}


CLI_CMD_REGISTER(ecdh_compute, &m_subcmd_var_var, cmd_ecdh_compute,
    "Compute Elliptic-curve Diffie-Hellman shared secret.\n"
    "Usage:\n"
    "  ecdh_compute private_key public_key shared_secret\n"
    "Arguments:\n"
    "  IN  private_key   Local private key\n"
    "  IN  public_key    Remote public key\n"
    "  OUT shared_secret Created shared secret\n"
    );


/* =============================================================================================
 *                                   ECDSA Commands
 * =============================================================================================
 */


/** @brief Command to sign with ECDSA. See command help for details.
 */
static bool cmd_ecdsa_sign(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                     ret;
    var_t                        * p_prv_var;
    var_t                        * p_hash_var;
    var_t                        * p_signature_var;
    nrf_crypto_ecc_private_key_t * p_key;
    nrf_crypto_ecdsa_signature_t   signature;
    size_t                         size;

    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK(argc == 4);

    p_prv_var  = cli_input_arg_get(1, &m_ecc_private_key_type);
    p_hash_var = cli_input_arg_get(2, &g_raw_type);
    CLI_CHECK((p_prv_var != NULL) && (p_hash_var != NULL));

    p_key = (nrf_crypto_ecc_private_key_t *)VAR_DATA(p_prv_var);

    size = sizeof(signature);
    ret  = nrf_crypto_ecdsa_sign(NULL,
                                 p_key,
                                 VAR_DATA(p_hash_var),
                                 p_hash_var->data_size,
                                 signature,
                                 &size);
    CLI_ERROR_CHECK(ret);

    p_signature_var = cli_output_arg_get(3, &g_raw_type, size);
    CLI_CHECK(p_signature_var != NULL);

    memcpy(VAR_DATA(p_signature_var), signature, size);

    return true;
}


CLI_CMD_REGISTER(ecdsa_sign, &m_subcmd_var_var, cmd_ecdsa_sign,
    "Sign a message with ECC private key.\n"
    "Usage:\n"
    "  ecdsa_sign private_key hash signature\n"
    "Arguments:\n"
    "  IN  private_key  Private key\n"
    "  IN  hash         Hash of the message to sign\n"
    "  OUT signature    Signature to create\n"
    );


/** @brief Command to verify with ECDSA. See command help for details.
 */
static bool cmd_ecdsa_verify(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                    ret;
    var_t                       * p_pub_var;
    var_t                       * p_hash_var;
    var_t                       * p_signature_var;
    nrf_crypto_ecc_public_key_t * p_key;

    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK(argc == 4);

    p_pub_var       = cli_input_arg_get(1, &m_ecc_public_key_type);
    p_hash_var      = cli_input_arg_get(2, &g_raw_type);
    p_signature_var = cli_input_arg_get(3, &g_raw_type);
    CLI_CHECK((p_pub_var != NULL) && (p_hash_var != NULL) && (p_signature_var != NULL));

    p_key = (nrf_crypto_ecc_public_key_t *)VAR_DATA(p_pub_var);

    ret = nrf_crypto_ecdsa_verify(NULL,
                                  p_key,
                                  VAR_DATA(p_hash_var),
                                  p_hash_var->data_size,
                                  VAR_DATA(p_signature_var),
                                  p_signature_var->data_size);
    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(ecdsa_verify, &m_subcmd_var_var_var, cmd_ecdsa_verify,
    "Verify a hash with ECC public key.\n"
    "Usage:\n"
    "  ecdsa_verify public_key hash signature\n"
    "Arguments:\n"
    "  IN  public_key   Public key\n"
    "  IN  hash         Hash of the message to verify\n"
    "  IN  signature    Signature to check\n"
    );


/* =============================================================================================
 *                                   EdDSA Commands
 * =============================================================================================
 */


/** @brief Command to sign with EdDSA. See command help for details.
 */
static bool cmd_eddsa_sign(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                     ret;
    var_t                        * p_prv_var;
    var_t                        * p_message_var;
    var_t                        * p_signature_var;
    nrf_crypto_ecc_private_key_t * p_key;
    nrf_crypto_ecdsa_signature_t   signature;
    size_t                         size;

    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK(argc == 4);

    p_prv_var     = cli_input_arg_get(1, &m_ecc_private_key_type);
    p_message_var = cli_input_arg_get(2, &g_raw_type);
    CLI_CHECK((p_prv_var != NULL) && (p_message_var != NULL));

    p_key = (nrf_crypto_ecc_private_key_t *)VAR_DATA(p_prv_var);

    size = sizeof(signature);
    ret  = nrf_crypto_eddsa_sign(NULL,
                                 p_key,
                                 VAR_DATA(p_message_var),
                                 p_message_var->data_size,
                                 signature,
                                 &size);
    CLI_ERROR_CHECK(ret);

    p_signature_var = cli_output_arg_get(3, &g_raw_type, size);
    CLI_CHECK(p_signature_var != NULL);

    memcpy(VAR_DATA(p_signature_var), signature, size);

    return true;
}


CLI_CMD_REGISTER(eddsa_sign, &m_subcmd_var_var, cmd_eddsa_sign,
    "Sign a message with ECC private key using EdDSA.\n"
    "Usage:\n"
    "  ecdsa_sign private_key message signature\n"
    "Arguments:\n"
    "  IN  private_key  Private key\n"
    "  IN  message      Message to sign\n"
    "  OUT signature    Signature to create\n"
    );


/** @brief Command to verify with EdDSA. See command help for details.
 */
static bool cmd_eddsa_verify(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                    ret;
    var_t                       * p_pub_var;
    var_t                       * p_hash_var;
    var_t                       * p_signature_var;
    nrf_crypto_ecc_public_key_t * p_key;

    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK(argc == 4);

    p_pub_var       = cli_input_arg_get(1, &m_ecc_public_key_type);
    p_hash_var      = cli_input_arg_get(2, &g_raw_type);
    p_signature_var = cli_input_arg_get(3, &g_raw_type);
    CLI_CHECK((p_pub_var != NULL) && (p_hash_var != NULL) && (p_signature_var != NULL));

    p_key = (nrf_crypto_ecc_public_key_t *)VAR_DATA(p_pub_var);

    ret = nrf_crypto_eddsa_verify(NULL,
                                  p_key,
                                  VAR_DATA(p_hash_var),
                                  p_hash_var->data_size,
                                  VAR_DATA(p_signature_var),
                                  p_signature_var->data_size);
    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(eddsa_verify, &m_subcmd_var_var_var, cmd_eddsa_verify,
    "Verify a hash with ECC public key using EdDSA.\n"
    "Usage:\n"
    "  ecdsa_verify public_key message signature\n"
    "Arguments:\n"
    "  IN  public_key   Public key\n"
    "  IN  message      Message to verify\n"
    "  IN  signature    Signature to check\n"
    );


/* =============================================================================================
 *                                   Utility ECC Commands
 * =============================================================================================
 */


/** @brief Command to invert byte order of a raw ECC data. See command help for details.
 */
static bool cmd_ecc_byte_order_invert(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t                           ret;
    var_t                              * p_in_var;
    var_t                              * p_out_var;
    nrf_crypto_ecc_curve_info_t  const * p_info;
    int                                  vars_first;

    UNUSED_PARAMETER(p_cli);

    CLI_ARGS_CHECK(argc == 3 || argc == 4);

    if (argc == 3)
    {
        p_info = NULL;
        vars_first = 1;
    }
    else
    {
        p_info = curve_info_get(argv[1]);
        CLI_CHECK(p_info != NULL);
        vars_first = 2;
    }

    p_in_var = cli_input_arg_get(vars_first, &g_raw_type);
    CLI_CHECK(p_in_var != NULL);
    p_out_var = cli_output_arg_get(vars_first + 1, &g_raw_type, p_in_var->data_size);
    CLI_CHECK(p_out_var != NULL);

    ret = nrf_crypto_ecc_byte_order_invert(p_info,
                                           VAR_DATA(p_in_var),
                                           VAR_DATA(p_out_var),
                                           p_in_var->data_size);
    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(ecc_byte_order_invert, &m_subcmd_curve_var_var, cmd_ecc_byte_order_invert,
    "Invert byte order of a raw ECC data.\n"
    "Usage:\n"
    "  ecc_byte_order_invert [curve] input output\n"
    "Arguments:\n"
    "  IN  curve   Optional curve that defines how to invert fields.\n"
    "              If it is skipped all bytes are inverted.\n"
    "  IN  input   Source raw data.\n"
    "  OUT output  Raw data containing inverted byte order.\n"
    );


/* =============================================================================================
 *                               ECC Key converters
 * =============================================================================================
 */


/** @brief Converter from public key to raw data. @see cli_conversion_fn_t.
 */
static bool public_to_raw_conv(struct cli_converter_s const * p_converter,
                               var_t                        * p_input,
                               var_t                       ** pp_output)
{
    ret_code_t                      ret;
    nrf_crypto_ecc_raw_public_key_t raw;
    size_t                          size;
    nrf_crypto_ecc_public_key_t   * p_key = (nrf_crypto_ecc_public_key_t *)VAR_DATA(p_input);

    UNUSED_PARAMETER(p_converter);

    size = sizeof(raw);
    ret = nrf_crypto_ecc_public_key_to_raw(p_key, raw, &size);
    CLI_ERROR_CHECK(ret);

    *pp_output = var_temp_create(&g_raw_type, size, true);
    CLI_CHECK(*pp_output != NULL);

    memcpy(VAR_DATA(*pp_output), raw, size);

    return true;
}


/*lint -esym(528,public_to_raw) */

CLI_CONVERTER_REGISTER(public_to_raw,
                 "raw",
                 &m_ecc_public_key_type,
                 &g_raw_type,
                 public_to_raw_conv,
                 NULL,
                 0);


/** @brief Converter from private key to raw data. @see cli_conversion_fn_t.
 */
static bool private_to_raw_conv(struct cli_converter_s const * p_converter,
                                var_t                        * p_input,
                                var_t                       ** pp_output)
{
    ret_code_t                       ret;
    nrf_crypto_ecc_raw_private_key_t raw;
    size_t                           size;
    nrf_crypto_ecc_private_key_t   * p_key = (nrf_crypto_ecc_private_key_t *)VAR_DATA(p_input);

    UNUSED_PARAMETER(p_converter);

    size = sizeof(raw);
    ret = nrf_crypto_ecc_private_key_to_raw(p_key, raw, &size);
    CLI_ERROR_CHECK(ret);

    *pp_output = var_temp_create(&g_raw_type, size, true);
    CLI_CHECK(*pp_output != NULL);

    memcpy(VAR_DATA(*pp_output), raw, size);

    return true;
}


/*lint -esym(528,private_to_raw) */

CLI_CONVERTER_REGISTER(private_to_raw,
                 "raw",
                 &m_ecc_private_key_type,
                 &g_raw_type,
                 private_to_raw_conv,
                 NULL,
                 0);


/** @brief Converter from raw data to private key. @see cli_conversion_fn_t.
 */
static bool raw_to_private_conv(struct cli_converter_s const * p_converter,
                                var_t                        * p_input,
                                var_t                       ** pp_output)
{
    ret_code_t ret;
    nrf_crypto_ecc_curve_info_t const * p_info =
        (nrf_crypto_ecc_curve_info_t const *)p_converter->p_tag;

    *pp_output = var_temp_create(&m_ecc_private_key_type, p_info->private_key_size, true);
    CLI_CHECK(*pp_output);

    nrf_crypto_ecc_private_key_t * p_key = (nrf_crypto_ecc_private_key_t *)VAR_DATA(*pp_output);

    ret = nrf_crypto_ecc_private_key_from_raw(p_info, p_key, VAR_DATA(p_input), p_input->data_size);
    CLI_ERROR_CHECK(ret);

    return true;
}


/** @brief Checks if conversion from raw data to private key is possible. @see cli_conversion_test_fn_t.
 */
static bool raw_to_private_test(struct cli_converter_s const * p_converter,
                                var_t                        * p_input,
                                var_type_t            const ** pp_type)
{
    nrf_crypto_ecc_curve_info_t * p_info = (nrf_crypto_ecc_curve_info_t *)p_converter->p_tag;
    if (p_input->data_size != p_info->raw_private_key_size)
    {
        return false;
    }
    return true;
}


/** @brief Converter from raw data to public key. @see cli_conversion_fn_t.
 */
static bool raw_to_public_conv(struct cli_converter_s const * p_converter,
                               var_t                        * p_input,
                               var_t                       ** pp_output)
{
    ret_code_t ret;
    nrf_crypto_ecc_curve_info_t const * p_info =
        (nrf_crypto_ecc_curve_info_t const *)p_converter->p_tag;

    *pp_output = var_temp_create(&m_ecc_public_key_type, p_info->public_key_size, true);
    CLI_CHECK(*pp_output);

    nrf_crypto_ecc_public_key_t * p_key = (nrf_crypto_ecc_public_key_t *)VAR_DATA(*pp_output);

    ret = nrf_crypto_ecc_public_key_from_raw(p_info, p_key, VAR_DATA(p_input), p_input->data_size);
    CLI_ERROR_CHECK(ret);

    return true;
}


/** @brief Checks if conversion from raw data to public key is possible. @see cli_conversion_test_fn_t.
 */
static bool raw_to_public_test(struct cli_converter_s const * p_converter,
                               var_t                        * p_input,
                               var_type_t            const ** pp_type)
{
    nrf_crypto_ecc_curve_info_t * p_info = (nrf_crypto_ecc_curve_info_t *)p_converter->p_tag;
    if (p_input->data_size != p_info->raw_public_key_size)
    {
        return false;
    }
    return true;
}


/** @brief Converter that inverts byte order in ecc raw data. @see cli_conversion_fn_t.
 */
static bool ecc_byte_invert_conv(struct cli_converter_s const * p_converter,
                                 var_t                        * p_input,
                                 var_t                       ** pp_output)
{
    ret_code_t ret;
    nrf_crypto_ecc_curve_info_t const * p_info =
        (nrf_crypto_ecc_curve_info_t const *)p_converter->p_tag;

    *pp_output = var_temp_create(&g_raw_type, p_input->data_size, true);
    CLI_CHECK(*pp_output);

    ret = nrf_crypto_ecc_byte_order_invert(p_info,
                                           VAR_DATA(p_input),
                                           VAR_DATA(*pp_output),
                                           p_input->data_size);
    CLI_ERROR_CHECK(ret);

    return true;
}


/** @brief Checks if specific curve supports inverting byte order in provided ecc raw data. @see cli_conversion_fn_t.
 */
static bool ecc_byte_invert_test(struct cli_converter_s const * p_converter,
                                 var_t                        * p_input,
                                 var_type_t            const ** pp_type)
{
    nrf_crypto_ecc_curve_info_t * p_info = (nrf_crypto_ecc_curve_info_t *)p_converter->p_tag;
    if ((p_info != NULL) && (p_input->data_size % p_info->raw_private_key_size != 0))
    {
        return false;
    }
    return true;
}


/*lint -esym(528,raw_to_private_*) */
/*lint -esym(528,raw_to_public_*) */
/*lint -esym(528,ecc_byte_invert_*) */

#if NRF_CRYPTO_ECC_BP256R1_ENABLED
CLI_CONVERTER_REGISTER(raw_to_private_bp256r1,
                 "private_bp256r1",
                 &g_raw_type,
                 &m_ecc_private_key_type,
                 raw_to_private_conv,
                 raw_to_private_test,
                 (void *)&g_nrf_crypto_ecc_bp256r1_curve_info);
CLI_CONVERTER_REGISTER(raw_to_public_bp256r1,
                 "public_bp256r1",
                 &g_raw_type,
                 &m_ecc_public_key_type,
                 raw_to_public_conv,
                 raw_to_public_test,
                 (void *)&g_nrf_crypto_ecc_bp256r1_curve_info);
CLI_CONVERTER_REGISTER(ecc_byte_invert_bp256r1,
                 "invert_bp256r1",
                 &g_raw_type,
                 &g_raw_type,
                 ecc_byte_invert_conv,
                 ecc_byte_invert_test,
                 (void *)&g_nrf_crypto_ecc_bp256r1_curve_info);
#endif

#if NRF_CRYPTO_ECC_BP384R1_ENABLED
CLI_CONVERTER_REGISTER(raw_to_private_bp384r1,
                 "private_bp384r1",
                 &g_raw_type,
                 &m_ecc_private_key_type,
                 raw_to_private_conv,
                 raw_to_private_test,
                 (void *)&g_nrf_crypto_ecc_bp384r1_curve_info);
CLI_CONVERTER_REGISTER(raw_to_public_bp384r1,
                 "public_bp384r1",
                 &g_raw_type,
                 &m_ecc_public_key_type,
                 raw_to_public_conv,
                 raw_to_public_test,
                 (void *)&g_nrf_crypto_ecc_bp384r1_curve_info);
CLI_CONVERTER_REGISTER(ecc_byte_invert_bp384r1,
                 "invert_bp384r1",
                 &g_raw_type,
                 &g_raw_type,
                 ecc_byte_invert_conv,
                 ecc_byte_invert_test,
                 (void *)&g_nrf_crypto_ecc_bp384r1_curve_info);
#endif

#if NRF_CRYPTO_ECC_BP512R1_ENABLED
CLI_CONVERTER_REGISTER(raw_to_private_bp512r1,
                 "private_bp512r1",
                 &g_raw_type,
                 &m_ecc_private_key_type,
                 raw_to_private_conv,
                 raw_to_private_test,
                 (void *)&g_nrf_crypto_ecc_bp512r1_curve_info);
CLI_CONVERTER_REGISTER(raw_to_public_bp512r1,
                 "public_bp512r1",
                 &g_raw_type,
                 &m_ecc_public_key_type,
                 raw_to_public_conv,
                 raw_to_public_test,
                 (void *)&g_nrf_crypto_ecc_bp512r1_curve_info);
CLI_CONVERTER_REGISTER(ecc_byte_invert_bp512r1,
                 "invert_bp512r1",
                 &g_raw_type,
                 &g_raw_type,
                 ecc_byte_invert_conv,
                 ecc_byte_invert_test,
                 (void *)&g_nrf_crypto_ecc_bp512r1_curve_info);
#endif

#if NRF_CRYPTO_ECC_CURVE25519_ENABLED
CLI_CONVERTER_REGISTER(raw_to_private_curve25519,
                 "private_curve25519",
                 &g_raw_type,
                 &m_ecc_private_key_type,
                 raw_to_private_conv,
                 raw_to_private_test,
                 (void *)&g_nrf_crypto_ecc_curve25519_curve_info);
CLI_CONVERTER_REGISTER(raw_to_public_curve25519,
                 "public_curve25519",
                 &g_raw_type,
                 &m_ecc_public_key_type,
                 raw_to_public_conv,
                 raw_to_public_test,
                 (void *)&g_nrf_crypto_ecc_curve25519_curve_info);
CLI_CONVERTER_REGISTER(ecc_byte_invert_curve25519,
                 "invert_curve25519",
                 &g_raw_type,
                 &g_raw_type,
                 ecc_byte_invert_conv,
                 ecc_byte_invert_test,
                 (void *)&g_nrf_crypto_ecc_curve25519_curve_info);
#endif

#if NRF_CRYPTO_ECC_ED25519_ENABLED
CLI_CONVERTER_REGISTER(raw_to_private_ed25519,
                 "private_ed25519",
                 &g_raw_type,
                 &m_ecc_private_key_type,
                 raw_to_private_conv,
                 raw_to_private_test,
                 (void *)&g_nrf_crypto_ecc_ed25519_curve_info);
CLI_CONVERTER_REGISTER(raw_to_public_ed25519,
                 "public_ed25519",
                 &g_raw_type,
                 &m_ecc_public_key_type,
                 raw_to_public_conv,
                 raw_to_public_test,
                 (void *)&g_nrf_crypto_ecc_ed25519_curve_info);
CLI_CONVERTER_REGISTER(ecc_byte_invert_ed25519,
                 "invert_ed25519",
                 &g_raw_type,
                 &g_raw_type,
                 ecc_byte_invert_conv,
                 ecc_byte_invert_test,
                 (void *)&g_nrf_crypto_ecc_ed25519_curve_info);
#endif

#if NRF_CRYPTO_ECC_SECP160K1_ENABLED
CLI_CONVERTER_REGISTER(raw_to_private_secp160k1,
                 "private_secp160k1",
                 &g_raw_type,
                 &m_ecc_private_key_type,
                 raw_to_private_conv,
                 raw_to_private_test,
                 (void *)&g_nrf_crypto_ecc_secp160k1_curve_info);
CLI_CONVERTER_REGISTER(raw_to_public_secp160k1,
                 "public_secp160k1",
                 &g_raw_type,
                 &m_ecc_public_key_type,
                 raw_to_public_conv,
                 raw_to_public_test,
                 (void *)&g_nrf_crypto_ecc_secp160k1_curve_info);
CLI_CONVERTER_REGISTER(ecc_byte_invert_secp160k1,
                 "invert_secp160k1",
                 &g_raw_type,
                 &g_raw_type,
                 ecc_byte_invert_conv,
                 ecc_byte_invert_test,
                 (void *)&g_nrf_crypto_ecc_secp160k1_curve_info);
#endif

#if NRF_CRYPTO_ECC_SECP160R1_ENABLED
CLI_CONVERTER_REGISTER(raw_to_private_secp160r1,
                 "private_secp160r1",
                 &g_raw_type,
                 &m_ecc_private_key_type,
                 raw_to_private_conv,
                 raw_to_private_test,
                 (void *)&g_nrf_crypto_ecc_secp160r1_curve_info);
CLI_CONVERTER_REGISTER(raw_to_public_secp160r1,
                 "public_secp160r1",
                 &g_raw_type,
                 &m_ecc_public_key_type,
                 raw_to_public_conv,
                 raw_to_public_test,
                 (void *)&g_nrf_crypto_ecc_secp160r1_curve_info);
CLI_CONVERTER_REGISTER(ecc_byte_invert_secp160r1,
                 "invert_secp160r1",
                 &g_raw_type,
                 &g_raw_type,
                 ecc_byte_invert_conv,
                 ecc_byte_invert_test,
                 (void *)&g_nrf_crypto_ecc_secp160r1_curve_info);
#endif

#if NRF_CRYPTO_ECC_SECP160R2_ENABLED
CLI_CONVERTER_REGISTER(raw_to_private_secp160r2,
                 "private_secp160r2",
                 &g_raw_type,
                 &m_ecc_private_key_type,
                 raw_to_private_conv,
                 raw_to_private_test,
                 (void *)&g_nrf_crypto_ecc_secp160r2_curve_info);
CLI_CONVERTER_REGISTER(raw_to_public_secp160r2,
                 "public_secp160r2",
                 &g_raw_type,
                 &m_ecc_public_key_type,
                 raw_to_public_conv,
                 raw_to_public_test,
                 (void *)&g_nrf_crypto_ecc_secp160r2_curve_info);
CLI_CONVERTER_REGISTER(ecc_byte_invert_secp160r2,
                 "invert_secp160r2",
                 &g_raw_type,
                 &g_raw_type,
                 ecc_byte_invert_conv,
                 ecc_byte_invert_test,
                 (void *)&g_nrf_crypto_ecc_secp160r2_curve_info);
#endif

#if NRF_CRYPTO_ECC_SECP192K1_ENABLED
CLI_CONVERTER_REGISTER(raw_to_private_secp192k1,
                 "private_secp192k1",
                 &g_raw_type,
                 &m_ecc_private_key_type,
                 raw_to_private_conv,
                 raw_to_private_test,
                 (void *)&g_nrf_crypto_ecc_secp192k1_curve_info);
CLI_CONVERTER_REGISTER(raw_to_public_secp192k1,
                 "public_secp192k1",
                 &g_raw_type,
                 &m_ecc_public_key_type,
                 raw_to_public_conv,
                 raw_to_public_test,
                 (void *)&g_nrf_crypto_ecc_secp192k1_curve_info);
CLI_CONVERTER_REGISTER(ecc_byte_invert_secp192k1,
                 "invert_secp192k1",
                 &g_raw_type,
                 &g_raw_type,
                 ecc_byte_invert_conv,
                 ecc_byte_invert_test,
                 (void *)&g_nrf_crypto_ecc_secp192k1_curve_info);
#endif

#if NRF_CRYPTO_ECC_SECP192R1_ENABLED
CLI_CONVERTER_REGISTER(raw_to_private_secp192r1,
                 "private_secp192r1",
                 &g_raw_type,
                 &m_ecc_private_key_type,
                 raw_to_private_conv,
                 raw_to_private_test,
                 (void *)&g_nrf_crypto_ecc_secp192r1_curve_info);
CLI_CONVERTER_REGISTER(raw_to_public_secp192r1,
                 "public_secp192r1",
                 &g_raw_type,
                 &m_ecc_public_key_type,
                 raw_to_public_conv,
                 raw_to_public_test,
                 (void *)&g_nrf_crypto_ecc_secp192r1_curve_info);
CLI_CONVERTER_REGISTER(ecc_byte_invert_secp192r1,
                 "invert_secp192r1",
                 &g_raw_type,
                 &g_raw_type,
                 ecc_byte_invert_conv,
                 ecc_byte_invert_test,
                 (void *)&g_nrf_crypto_ecc_secp192r1_curve_info);
#endif

#if NRF_CRYPTO_ECC_SECP224K1_ENABLED
CLI_CONVERTER_REGISTER(raw_to_private_secp224k1,
                 "private_secp224k1",
                 &g_raw_type,
                 &m_ecc_private_key_type,
                 raw_to_private_conv,
                 raw_to_private_test,
                 (void *)&g_nrf_crypto_ecc_secp224k1_curve_info);
CLI_CONVERTER_REGISTER(raw_to_public_secp224k1,
                 "public_secp224k1",
                 &g_raw_type,
                 &m_ecc_public_key_type,
                 raw_to_public_conv,
                 raw_to_public_test,
                 (void *)&g_nrf_crypto_ecc_secp224k1_curve_info);
CLI_CONVERTER_REGISTER(ecc_byte_invert_secp224k1,
                 "invert_secp224k1",
                 &g_raw_type,
                 &g_raw_type,
                 ecc_byte_invert_conv,
                 ecc_byte_invert_test,
                 (void *)&g_nrf_crypto_ecc_secp224k1_curve_info);
#endif

#if NRF_CRYPTO_ECC_SECP224R1_ENABLED
CLI_CONVERTER_REGISTER(raw_to_private_secp224r1,
                 "private_secp224r1",
                 &g_raw_type,
                 &m_ecc_private_key_type,
                 raw_to_private_conv,
                 raw_to_private_test,
                 (void *)&g_nrf_crypto_ecc_secp224r1_curve_info);
CLI_CONVERTER_REGISTER(raw_to_public_secp224r1,
                 "public_secp224r1",
                 &g_raw_type,
                 &m_ecc_public_key_type,
                 raw_to_public_conv,
                 raw_to_public_test,
                 (void *)&g_nrf_crypto_ecc_secp224r1_curve_info);
CLI_CONVERTER_REGISTER(ecc_byte_invert_secp224r1,
                 "invert_secp224r1",
                 &g_raw_type,
                 &g_raw_type,
                 ecc_byte_invert_conv,
                 ecc_byte_invert_test,
                 (void *)&g_nrf_crypto_ecc_secp224r1_curve_info);
#endif

#if NRF_CRYPTO_ECC_SECP256K1_ENABLED
CLI_CONVERTER_REGISTER(raw_to_private_secp256k1,
                 "private_secp256k1",
                 &g_raw_type,
                 &m_ecc_private_key_type,
                 raw_to_private_conv,
                 raw_to_private_test,
                 (void *)&g_nrf_crypto_ecc_secp256k1_curve_info);
CLI_CONVERTER_REGISTER(raw_to_public_secp256k1,
                 "public_secp256k1",
                 &g_raw_type,
                 &m_ecc_public_key_type,
                 raw_to_public_conv,
                 raw_to_public_test,
                 (void *)&g_nrf_crypto_ecc_secp256k1_curve_info);
CLI_CONVERTER_REGISTER(ecc_byte_invert_secp256k1,
                 "invert_secp256k1",
                 &g_raw_type,
                 &g_raw_type,
                 ecc_byte_invert_conv,
                 ecc_byte_invert_test,
                 (void *)&g_nrf_crypto_ecc_secp256k1_curve_info);
#endif

#if NRF_CRYPTO_ECC_SECP256R1_ENABLED
CLI_CONVERTER_REGISTER(raw_to_private_secp256r1,
                 "private_secp256r1",
                 &g_raw_type,
                 &m_ecc_private_key_type,
                 raw_to_private_conv,
                 raw_to_private_test,
                 (void *)&g_nrf_crypto_ecc_secp256r1_curve_info);
CLI_CONVERTER_REGISTER(raw_to_public_secp256r1,
                 "public_secp256r1",
                 &g_raw_type,
                 &m_ecc_public_key_type,
                 raw_to_public_conv,
                 raw_to_public_test,
                 (void *)&g_nrf_crypto_ecc_secp256r1_curve_info);
CLI_CONVERTER_REGISTER(ecc_byte_invert_secp256r1,
                 "invert_secp256r1",
                 &g_raw_type,
                 &g_raw_type,
                 ecc_byte_invert_conv,
                 ecc_byte_invert_test,
                 (void *)&g_nrf_crypto_ecc_secp256r1_curve_info);
#endif

#if NRF_CRYPTO_ECC_SECP384R1_ENABLED
CLI_CONVERTER_REGISTER(raw_to_private_secp384r1,
                 "private_secp384r1",
                 &g_raw_type,
                 &m_ecc_private_key_type,
                 raw_to_private_conv,
                 raw_to_private_test,
                 (void *)&g_nrf_crypto_ecc_secp384r1_curve_info);
CLI_CONVERTER_REGISTER(raw_to_public_secp384r1,
                 "public_secp384r1",
                 &g_raw_type,
                 &m_ecc_public_key_type,
                 raw_to_public_conv,
                 raw_to_public_test,
                 (void *)&g_nrf_crypto_ecc_secp384r1_curve_info);
CLI_CONVERTER_REGISTER(ecc_byte_invert_secp384r1,
                 "invert_secp384r1",
                 &g_raw_type,
                 &g_raw_type,
                 ecc_byte_invert_conv,
                 ecc_byte_invert_test,
                 (void *)&g_nrf_crypto_ecc_secp384r1_curve_info);
#endif

#if NRF_CRYPTO_ECC_SECP521R1_ENABLED
CLI_CONVERTER_REGISTER(raw_to_private_secp521r1,
                 "private_secp521r1",
                 &g_raw_type,
                 &m_ecc_private_key_type,
                 raw_to_private_conv,
                 raw_to_private_test,
                 (void *)&g_nrf_crypto_ecc_secp521r1_curve_info);
CLI_CONVERTER_REGISTER(raw_to_public_secp521r1,
                 "public_secp521r1",
                 &g_raw_type,
                 &m_ecc_public_key_type,
                 raw_to_public_conv,
                 raw_to_public_test,
                 (void *)&g_nrf_crypto_ecc_secp521r1_curve_info);
CLI_CONVERTER_REGISTER(ecc_byte_invert_secp521r1,
                 "invert_secp521r1",
                 &g_raw_type,
                 &g_raw_type,
                 ecc_byte_invert_conv,
                 ecc_byte_invert_test,
                 (void *)&g_nrf_crypto_ecc_secp521r1_curve_info);
#endif

CLI_CONVERTER_REGISTER(ecc_byte_invert_generic,
                 "invert",
                 &g_raw_type,
                 &g_raw_type,
                 ecc_byte_invert_conv,
                 ecc_byte_invert_test,
                 NULL);
