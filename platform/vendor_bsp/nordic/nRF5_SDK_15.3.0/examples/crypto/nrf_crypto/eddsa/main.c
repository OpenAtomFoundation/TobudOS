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
/**
 * @defgroup nrf_crypto_eddsa_example
 * @{
 * @ingroup nrf_crypto_eddsa
 * @brief EdDSA Example Application main file.
 *
 * This file contains the source code for a sample application that demonstrates the usage of the
 * nrf_crypto library to generate and verify an EdDSA signature. Different backends can be
 * used by adjusting @ref sdk_config.h.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nrf_assert.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_crypto.h"

static uint8_t                                      m_message[] = "Authentic message";
const size_t                                        m_message_len = sizeof(m_message) - 1; // Skipping null termination
static nrf_crypto_ecc_key_pair_generate_context_t   m_key_pair_generate_context;
static nrf_crypto_eddsa_sign_context_t              m_sign_context;
static nrf_crypto_eddsa_verify_context_t            m_verify_context;


/** @brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/** @brief Function for the application main entry.
 */
int main(void)
{

    static nrf_crypto_ecc_private_key_t priv_key;
    static nrf_crypto_ecc_public_key_t  pub_key;
    static uint8_t                      raw_priv_key[NRF_CRYPTO_ECC_ED25519_RAW_PRIVATE_KEY_SIZE];
    static uint8_t                      raw_pub_key[NRF_CRYPTO_ECC_ED25519_RAW_PUBLIC_KEY_SIZE];
    static uint8_t                      signature[NRF_CRYPTO_EDDSA_ED25519_SIGNATURE_SIZE];
    size_t                              signature_size = sizeof(signature);
    size_t                              raw_priv_key_size = sizeof(raw_priv_key);
    size_t                              raw_pub_key_size = sizeof(raw_pub_key);
    ret_code_t                          err_code;

    log_init();

    NRF_LOG_INFO("EdDSA example started.");

    err_code = nrf_crypto_init();
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("Message:")
    NRF_LOG_HEXDUMP_INFO(m_message, m_message_len);

    // Generate key pair.
    err_code = nrf_crypto_ecc_key_pair_generate(&m_key_pair_generate_context,
                                                &g_nrf_crypto_ecc_ed25519_curve_info,
                                                &priv_key,
                                                &pub_key);
    APP_ERROR_CHECK(err_code);

    // Print private key.
    err_code = nrf_crypto_ecc_private_key_to_raw(&priv_key,
                                                 raw_priv_key,
                                                 &raw_priv_key_size);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("Private key:")
    NRF_LOG_HEXDUMP_INFO(raw_priv_key, raw_priv_key_size);

    // Print public key.
    err_code = nrf_crypto_ecc_public_key_to_raw(&pub_key,
                                                raw_pub_key,
                                                &raw_pub_key_size);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("Public key:")
    NRF_LOG_HEXDUMP_INFO(raw_pub_key, raw_pub_key_size);

    // Sign message.
    err_code = nrf_crypto_eddsa_sign(&m_sign_context,
                                     &priv_key,
                                     m_message,
                                     m_message_len,
                                     signature,
                                     &signature_size);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("Signature:")
    NRF_LOG_HEXDUMP_INFO(signature, signature_size);

    // Verify message.
    err_code = nrf_crypto_eddsa_verify(&m_verify_context,
                                       &pub_key,
                                       m_message,
                                       m_message_len,
                                       signature,
                                       sizeof(signature));

    if (err_code == NRF_SUCCESS)
    {
        NRF_LOG_INFO("Signature is valid. Message is authentic.");
    }
    else if (err_code == NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE)
    {
        NRF_LOG_ERROR("Signature is invalid. Message is not authentic.");
        while(1);
    }
    else
    {
        APP_ERROR_CHECK(err_code);
    }

    NRF_LOG_INFO("EdDSA example executed successfully.");

    for (;;)
    {
    }
}


/** @}
 */
