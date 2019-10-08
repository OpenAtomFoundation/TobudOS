/**
 * Copyright (c) 2017 - 2019, Nordic Semiconductor ASA
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
 * @defgroup nrf_crypto_hash main.c
 * @{
 * @ingroup nrf_crypto_hash
 * @brief Cryptographic Hash Example Application main file.
 *
 * This file contains the source code for a sample application that demonstrates using the
 * nrf_crypto library to do hash calculations. Different backends can be used by adjusting
 * @ref sdk_config.h accordingly.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "nrf_assert.h"
#include "nrf_log_default_backends.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_crypto.h"
#include "nrf_crypto_hash.h"

static uint8_t m_message[] = "abc";
const size_t  m_message_len = 3; // Skipping null termination
nrf_crypto_hash_sha256_digest_t     m_digest;
nrf_crypto_hash_sha256_digest_t     m_digest_swapped;
const size_t m_digest_len = NRF_CRYPTO_HASH_SIZE_SHA256;

nrf_crypto_hash_sha512_digest_t     m_digest512;
const size_t m_digest_len512 = NRF_CRYPTO_HASH_SIZE_SHA512;


static uint8_t m_expected_digest[NRF_CRYPTO_HASH_SIZE_SHA256] =
{
    0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea, 0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
    0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c, 0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
};

nrf_crypto_hash_context_t   hash_context;

static void print_array(const uint8_t *string, size_t len)
{
  #if NRF_LOG_ENABLED
  for(size_t i = 0; i < len; i++)
  {
    NRF_LOG_RAW_INFO("%02x", string[i]);
  }
  #endif // NRF_LOG_ENABLED
}

#define PRINT_HEX(msg, res, len)    \
do                                  \
{                                   \
    NRF_LOG_RAW_INFO(msg);          \
    NRF_LOG_RAW_INFO("\r\n");       \
    print_array(res, len);          \
    NRF_LOG_RAW_INFO("\r\n")        \
} while(0)

/**
 * @brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    ret_code_t err_code = NRF_SUCCESS;
    size_t digest_len = m_digest_len;

    log_init();

    NRF_LOG_INFO("Hash example started.\r\n");

    // Initialize crypto subsystem
    err_code = nrf_crypto_init();
    APP_ERROR_CHECK(err_code);


    NRF_LOG_INFO("Calculating SHA-256 hash with init/update/finalize");

    // Initialize the hash context
    err_code = nrf_crypto_hash_init(&hash_context, &g_nrf_crypto_hash_sha256_info);
    APP_ERROR_CHECK(err_code);

    // Run the update function (this can be run multiples of time if the data is accessible
    // in smaller chunks, e.g. when received on-air.
    err_code = nrf_crypto_hash_update(&hash_context, m_message, m_message_len);
    APP_ERROR_CHECK(err_code);

    // Run the finalize when all data has been fed to the update function.
    // this gives you the result
    err_code = nrf_crypto_hash_finalize(&hash_context, m_digest, &digest_len);
    APP_ERROR_CHECK(err_code);

    PRINT_HEX("Calculated SHA-256 digest: ", m_digest, digest_len);

    if(digest_len != NRF_CRYPTO_HASH_SIZE_SHA256)
    {
        NRF_LOG_ERROR("Invalid size of hash: 0x%04x, expected: %0x%04x", digest_len, NRF_CRYPTO_HASH_SIZE_SHA256);
        while(1);
    }

    if (memcmp(m_digest, m_expected_digest, NRF_CRYPTO_HASH_SIZE_SHA256) == 0)
    {
        NRF_LOG_INFO("SHA-256 hash calculation was correct\r\n");
    }
    else
    {
        NRF_LOG_ERROR("SHA-256 hash calculation failed\r\n");
        while(1);
    }

    NRF_LOG_INFO("Calculating SHA-256 hash with nrf_crypto_hash_compute (integrated version)");

  // Integrated version
    err_code = nrf_crypto_hash_calculate(&hash_context,                     // Context or NULL to allocate internally
                                         &g_nrf_crypto_hash_sha256_info,    // Info structure configures hash mode
                                         m_message,                         // Input buffer
                                         m_message_len,                     // Input size
                                         m_digest,                          // Result buffer
                                         &digest_len);                      // Result size
    APP_ERROR_CHECK(err_code);

    PRINT_HEX("Calculated SHA-256 digest: ", m_digest, digest_len);

    if (memcmp(m_digest, m_expected_digest, NRF_CRYPTO_HASH_SIZE_SHA256) == 0)
    {
        NRF_LOG_INFO("Integrated SHA-256 hash calculation was correct\r\n");
    }
    else
    {
        NRF_LOG_ERROR("Integrated SHA-256 hash calculation failed\r\n");
        while(1);
    }

    NRF_LOG_INFO("Hash example executed successfully.");

    for (;;)
    {
    }
}

/**
 *@}
 **/
