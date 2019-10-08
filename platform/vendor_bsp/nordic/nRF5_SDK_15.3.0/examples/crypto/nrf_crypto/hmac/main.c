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
/** @file
 *
 * @defgroup nrf_crypto_example_hmac main.c
 * @{
 * @ingroup nrf_crypto_example
 * 
 * @brief Hash-based message authentication (HMAC) Example Application main file.
 *
 * @details This file contains the source code for a sample application that demonstrates using the
 * @lib_crypto_hmac library to do HMAC calculations. Different backends can be used by adjusting
 * @ref sdk_config accordingly.
 *
 */

#include <stdint.h>
#include "app_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_crypto.h"


static uint8_t m_data[] = {0x6d, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65};
static uint8_t m_key[] = {0x6b, 0x65, 0x79};
static uint8_t m_digest[NRF_CRYPTO_HASH_SIZE_SHA256] = {0};
static nrf_crypto_hmac_context_t m_context;

static uint8_t m_expected_digest[NRF_CRYPTO_HASH_SIZE_SHA256] =
{
    0x6e, 0x9e, 0xf2, 0x9b, 0x75, 0xff, 0xfc, 0x5b, 0x7a, 0xba, 0xe5, 0x27, 0xd5, 0x8f, 0xda, 0xdb,
    0x2f, 0xe4, 0x2e, 0x72, 0x19, 0x01, 0x19, 0x76, 0x91, 0x73, 0x43, 0x06, 0x5f, 0x58, 0xed, 0x4a
};

static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


int main(void)
{
    uint32_t err_code = NRF_SUCCESS;
    size_t digest_len = sizeof(m_digest);

    log_init();
    err_code = nrf_crypto_init();
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("HMAC example started.");

    // Initialize frontend (which also initializes backend).
    err_code = nrf_crypto_hmac_init(&m_context,
                                    &g_nrf_crypto_hmac_sha256_info,
                                    m_key,
                                    sizeof(m_key));
    APP_ERROR_CHECK(err_code);

    // Push all data in one go (could be done repeatedly)
    err_code = nrf_crypto_hmac_update(&m_context, m_data, sizeof(m_data));
    APP_ERROR_CHECK(err_code);

    // Finish calculation
    err_code = nrf_crypto_hmac_finalize(&m_context, m_digest, &digest_len);
    APP_ERROR_CHECK(err_code);

    // Print digest (result).
    NRF_LOG_INFO("Calculated HMAC (length %u:)", digest_len)
    NRF_LOG_HEXDUMP_INFO(m_digest, digest_len);

    // Compare calculated digest with the expected digest.
    if (memcmp(m_digest, m_expected_digest, digest_len) == 0)
    {
        NRF_LOG_INFO("HMAC example executed successfully.");
    }
    else
    {
        NRF_LOG_ERROR("HMAC example failed!!!");
        while(1);
    }

    for (;;)
    {
    }
}

/**
 *@}
 **/
