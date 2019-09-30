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
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <ctype.h>


#include "nrf.h"
#include "nrf_drv_clock.h"
#include "nrf_delay.h"

#include "nrf_drv_power.h"

#include "app_error.h"
#include "app_util.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "boards.h"

#include "nrf_crypto.h"
#include "nrf_crypto_error.h"
#include "mem_manager.h"


#if NRF_MODULE_ENABLED(NRF_CRYPTO)
/**@file
 * @defgroup CHACHA_POLY_example main.c
 *
 * @{
 *
 */

#define NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE    (100)

#define AES_ERROR_CHECK(error)  \
    do {            \
        if (error)  \
        {           \
            NRF_LOG_RAW_INFO("\r\nError = 0x%x\r\n%s\r\n",           \
                             (error),                                \
                             nrf_crypto_error_string_get(error));    \
            return; \
        }           \
    } while (0);



/* Maximum allowed key = 256 bit */
static uint8_t m_key[32] = {'N', 'O', 'R', 'D', 'I', 'C',
                            'S', 'E', 'M', 'I', 'C', 'O', 'N', 'D', 'U', 'C', 'T', 'O', 'R',
                            'C', 'H', 'A', 'C', 'H', 'A', 'P', 'O', 'L', 'Y', 'K', 'E', 'Y'};

/* Below text is used as plain text for encryption, decryption and MAC calculation. */
static char m_plain_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE] =
{
    "Example string used to demonstrate ChaCha-Poly example."
};
static char m_encrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];
static char m_decrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];

static void text_print(char const* p_label, char const * p_text, size_t len)
{
    NRF_LOG_RAW_INFO("----%s (length: %u) ----\r\n", p_label, len);
    NRF_LOG_FLUSH();
    for(size_t i = 0; i < len; i++)
    {
        NRF_LOG_RAW_INFO("%c", p_text[i]);
        NRF_LOG_FLUSH();
    }
    NRF_LOG_RAW_INFO("\r\n");
    NRF_LOG_RAW_INFO("---- %s end ----\r\n\r\n", p_label);
    NRF_LOG_FLUSH();
}

static void hex_text_print(char const* p_label, char const * p_text, size_t len)
{
    NRF_LOG_RAW_INFO("---- %s (length: %u) ----\r\n", p_label, len);
    NRF_LOG_FLUSH();

    // Handle partial line (left)
    for (size_t i = 0; i < len; i++)
    {
        if (((i & 0xF) == 0) && (i > 0))
        {
            NRF_LOG_RAW_INFO("\r\n");
            NRF_LOG_FLUSH();
        }

        NRF_LOG_RAW_INFO("%02x ", p_text[i]);
        NRF_LOG_FLUSH();
    }
    NRF_LOG_RAW_INFO("\r\n");
    NRF_LOG_RAW_INFO("---- %s end ----\r\n\r\n", p_label);
    NRF_LOG_FLUSH();
}

static void plain_text_print(void)
{
    size_t len = strlen(m_plain_text);

    text_print("Plain text", m_plain_text, len);
    hex_text_print("Plain text (hex)", m_plain_text, len);
}

static void encrypted_text_print(char const * p_text, size_t encrypted_len)
{
    hex_text_print("Encrypted text (hex)", p_text, encrypted_len);
}

static void decrypted_text_print(char const * p_text, size_t decrypted_len)
{
    text_print("Decrypted text", p_text, decrypted_len);
    hex_text_print("Decrypted text (hex)", p_text, decrypted_len);
}

static void mac_print(uint8_t const * p_buff, uint8_t mac_size)
{
    hex_text_print("MAC (hex)", (char const*)p_buff, mac_size);
}

static void crypt_chacha_poly(void)
{
    uint32_t    len;
    ret_code_t  ret_val;
    static uint8_t     mac[NRF_CRYPTO_CHACHA_POLY_MAC_SIZE];
    static uint8_t     nonce[NRF_CRYPTO_CHACHA_POLY_NONCE_SIZE];
    static uint8_t     adata[] = {0xAA, 0xBB, 0xCC, 0xDD};

    static nrf_crypto_aead_context_t chacha_poly_ctx;

    memset(mac,   0, sizeof(mac));
    memset(nonce, 0, sizeof(nonce));

    plain_text_print();

    len = strlen((char const *)m_plain_text);

    /* Init encrypt and decrypt context */
    ret_val = nrf_crypto_aead_init(&chacha_poly_ctx,
                                   &g_nrf_crypto_chacha_poly_256_info,
                                   m_key);
    AES_ERROR_CHECK(ret_val);

    /* encrypt and tag text */
    ret_val = nrf_crypto_aead_crypt(&chacha_poly_ctx,
                                    NRF_CRYPTO_ENCRYPT,
                                    nonce,
                                    sizeof(nonce),
                                    adata,
                                    sizeof(adata),
                                    (uint8_t *)m_plain_text,
                                    len,
                                    (uint8_t *)m_encrypted_text,
                                    mac,
                                    sizeof(mac));
    AES_ERROR_CHECK(ret_val);

    encrypted_text_print(m_encrypted_text, len);
    mac_print(mac, sizeof(mac));

    /* decrypt text */
    ret_val = nrf_crypto_aead_crypt(&chacha_poly_ctx,
                                    NRF_CRYPTO_DECRYPT,
                                    nonce,
                                    sizeof(nonce),
                                    adata,
                                    sizeof(adata),
                                    (uint8_t *)m_encrypted_text,
                                    len,
                                    (uint8_t *)m_decrypted_text,
                                    mac,
                                    sizeof(mac));
    AES_ERROR_CHECK(ret_val);

    ret_val = nrf_crypto_aead_uninit(&chacha_poly_ctx);
    AES_ERROR_CHECK(ret_val);

    decrypted_text_print(m_decrypted_text, len);

    if (memcmp(m_plain_text, m_decrypted_text, len) == 0)
    {
        NRF_LOG_RAW_INFO("chacha-poly example executed successfully.\r\n");
    }
    else
    {
        NRF_LOG_RAW_INFO("chacha-poly example failed!!!\r\n");
    }
}

int main(void)
{
    ret_code_t ret;

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_RAW_INFO("chacha-poly example started.\r\n");
    NRF_LOG_FLUSH();

    ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret);
    nrf_drv_clock_lfclk_request(NULL);

    ret = nrf_crypto_init();
    APP_ERROR_CHECK(ret);

    ret = nrf_mem_init();
    APP_ERROR_CHECK(ret);

    crypt_chacha_poly();
    while (true)
    {
        NRF_LOG_FLUSH();
        UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
    }
}

/** @} */
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO)

