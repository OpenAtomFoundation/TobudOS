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
 * @defgroup nrf_crypto_example_hkdf main.c
 * @{
 * @ingroup nrf_crypto_example
 *
 * @brief HMAC based Key Derivation Function (HKDF) Example Application main file.
 *
 * @details This file contains the source code for a sample application that demonstrates using the
 * nrf_crypto library to do HKDF calculations. Different backends can be used by configuring the
 * HMAC backend in @c sdk_config.h accordingly.
 *
 */

#include <stdint.h>
#include "app_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_crypto.h"


// Test data from RFC 5869 Test Case 1 (https://tools.ietf.org/html/rfc5869)
// Input key material.
static uint8_t m_ikm[] =
{
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b
};

// Salt.
static uint8_t m_salt[] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c
};

// Additional info material optionally used to increase entropy.
static uint8_t m_ainfo[] =
{
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9
};

// Expected output key material (result of HKDF operation).
static uint8_t m_expected_okm[] =
{
    0x3c, 0xb2, 0x5f, 0x25, 0xfa, 0xac, 0xd5, 0x7a, 0x90, 0x43, 0x4f, 0x64, 0xd0, 0x36, 0x2f, 0x2a,
    0x2d, 0x2d, 0x0a, 0x90, 0xcf, 0x1a, 0x5a, 0x4c, 0x5d, 0xb0, 0x2d, 0x56, 0xec, 0xc4, 0xc5, 0xbf,
    0x34, 0x00, 0x72, 0x08, 0xd5, 0xb8, 0x87, 0x18, 0x58, 0x65
};

static uint8_t                          m_okm[sizeof(m_expected_okm)];
static nrf_crypto_hmac_context_t m_context;

static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


int main(void)
{
    uint32_t err_code = NRF_SUCCESS;
    size_t okm_len = sizeof(m_okm);

    log_init();
    err_code = nrf_crypto_init();
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("HKDF example started.");

    err_code = nrf_crypto_hkdf_calculate(&m_context,
                                         &g_nrf_crypto_hmac_sha256_info,
                                         m_okm,
                                         &okm_len,
                                         m_ikm,
                                         sizeof(m_ikm),
                                         m_salt,
                                         sizeof(m_salt),
                                         m_ainfo,
                                         sizeof(m_ainfo),
                                         NRF_CRYPTO_HKDF_EXTRACT_AND_EXPAND);
    APP_ERROR_CHECK(err_code);

    if (memcmp(m_okm, m_expected_okm, sizeof(m_expected_okm)) == 0)
    {
        NRF_LOG_INFO("HKDF example executed successfully.");
    }
    else
    {
        NRF_LOG_ERROR("HKDF example failed!!!");
    }

    for (;;)
    {
    }
}

/**
 *@}
 **/
