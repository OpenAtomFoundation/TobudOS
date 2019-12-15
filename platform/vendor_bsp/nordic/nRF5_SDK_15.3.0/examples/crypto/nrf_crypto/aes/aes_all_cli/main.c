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
#include "app_timer.h"

#include "app_error.h"
#include "app_util.h"

#include "nrf_cli.h"
#include "nrf_cli_uart.h"
#include "nrf_cli_types.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "boards.h"

#include "nrf_crypto.h"
#include "nrf_crypto_error.h"
#include "mem_manager.h"


#if NRF_MODULE_ENABLED(NRF_CRYPTO)
/**@file
 * @defgroup AES_example main.c
 *
 * @{
 *
 */

#define HELP_CMD_HELP           "Encrypts, decrypts or calculates MAC of plain text with " \
                                "configured settings."
#define HELP_CMD_CBC            "Encrypt & decrypt text with AES CBC mode."
#define HELP_CMD_CBC_MAC        "Caclculate MAC with AES CBC_MAC mode."
#define HELP_CMD_CCM            "Encrypt & decrypt text with AES CCM mode."
#define HELP_CMD_CCM_STAR       "Encrypt & decrypt text with AES CCM_STAR (CCM*) mode."
#define HELP_CMD_CFB            "Encrypt & decrypt text with AES CFB mode."
#define HELP_CMD_CMAC           "Caclculate MAC with AES CMAC mode."
#define HELP_CMD_CTR            "Encrypt & decrypt text with AES CTR mode."
#define HELP_CMD_EAX            "Encrypt & decrypt text with AES EAX mode."
#define HELP_CMD_ECB            "Encrypt & decrypt text with AES ECB mode."
#define HELP_CMD_GCM            "Encrypt & decrypt text with AES GCM mode."
#define HELP_CMD_AEAD           "Encrypt and decrypt plain text with specified AES AEAD mode."
#define HELP_CMD_CRYPT          "Encrypt and decrypt plain text with specified AES mode."
#define HELP_CMD_MAC_CALC       "Calculate specified MAC on plain text."
#define HELP_CMD_KEY_SET        "Set key size for crypt and mac operation."
#define HELP_CMD_KEY128         "128 bit key size"
#define HELP_CMD_KEY192         "192 bit key size"
#define HELP_CMD_KEY256         "256 bit key size"


#define AES_ERROR_CHECK(p_cli, error)  \
    do {            \
        if (error)  \
        {           \
            nrf_cli_fprintf((p_cli),                                \
                            NRF_CLI_ERROR,                          \
                            "\r\nError = 0x%x\r\n%s\r\n",           \
                            (error),                                \
                            nrf_crypto_error_string_get(error));    \
            return; \
        }           \
    } while (0);

#define NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE    (100)
#define CLI_EXAMPLE_LOG_QUEUE_SIZE              (4)
NRF_CLI_UART_DEF(m_cli_uart_transport, 0, 128, 128);
NRF_CLI_DEF(m_cli_uart,
            "uart_cli:~$ ",
            &m_cli_uart_transport.transport,
            '\r',
            CLI_EXAMPLE_LOG_QUEUE_SIZE);


/* Maximum allowed key = 256 bit */
static uint8_t m_key[32] = {'N', 'O', 'R', 'D', 'I', 'C', ' ',
                            'S', 'E', 'M', 'I', 'C', 'O', 'N', 'D', 'U', 'C', 'T', 'O', 'R',
                            'A', 'E', 'S', '&', 'M', 'A', 'C', ' ', 'T', 'E', 'S', 'T'};
static nrf_crypto_key_size_id_t m_key_size = NRF_CRYPTO_KEY_SIZE_128;

/* Below text is used as plain text for encryption, decryption and MAC calculation. Its length must
   be multiple of 16 bytes to use NRF_CRYPTO_AES library. */
static char const * mp_default_text = "AES example demonstrating basic usage of nrf_crypto."
                                      " Test start.";
static char m_plain_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];

static void plain_text_print(nrf_cli_t const * p_cli)
{
    size_t len = strlen(m_plain_text);

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Plain text:\r\n%s\r\n", m_plain_text);
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Plain text - ASCII:");
    for (size_t i = 0; i < len; i++)
    {
        if ((i & 0xF) == 0)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\r\n");
        }
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%2x ", m_plain_text[i]);
    }
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\r\n");
}

static void encrypted_text_print(nrf_cli_t const * p_cli, char const * p_text)
{
    size_t len = strlen(m_plain_text);

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Encrypted text:\r\n");
    for (size_t i = 0; i < len; i++)
    {
        char c = p_text[i];
        if (isprint((int)c))
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%c", c);
        }
        else
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, ".");
        }
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\r\nEncrypted text - ASCII:");
    for (size_t i = 0; i < len; i++)
    {
        if ((i & 0xF) == 0)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "\r\n");
        }
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%02x ", p_text[i]);
    }
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\r\n");
}

static void decrypted_text_print(nrf_cli_t const * p_cli, char const * p_text)
{
    size_t len = strlen(p_text);

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Decrypted text:\r\n%s\r\n", p_text);
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Decrypted text - ASCII:");
    for (size_t i = 0; i < len; i++)
    {
        if ((i & 0xF) == 0)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "\r\n");
        }
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%02x ", p_text[i]);
    }
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\r\n");
}

static void mac_print(nrf_cli_t const * p_cli, uint8_t const * p_buff, uint8_t mac_size)
{
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Calculated MAC:\r\n");
    for (size_t i = 0; i < mac_size; i++)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%02x ", p_buff[i]);
    }
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\r\n");
}

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM)
static void nrf_cli_cmd_ccm(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    uint32_t    len;
    ret_code_t  ret_val;
    uint8_t     mac[16];
    uint8_t     nonce[13];
    uint8_t     adata[] = {0xAA, 0xBB, 0xCC, 0xDD};
    char        encrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];
    char        decrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];

    nrf_crypto_aead_info_t const * p_ccm_info;
    nrf_crypto_aead_context_t      ccm_encr_ctx;
    nrf_crypto_aead_context_t      ccm_decr_ctx;

    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    memset(mac,             0,  sizeof(mac));
    memset(nonce,           0,  sizeof(nonce));
    memset(encrypted_text,  0,  sizeof(encrypted_text));
    memset(decrypted_text,  0,  sizeof(encrypted_text));
    memset(&ccm_encr_ctx,   0,  sizeof(ccm_encr_ctx));
    memset(&ccm_decr_ctx,   0,  sizeof(ccm_decr_ctx));

    len = strlen((char const *)m_plain_text);

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES mode: CCM | ");

    switch (m_key_size)
    {
        case NRF_CRYPTO_KEY_SIZE_128:
            p_ccm_info = &g_nrf_crypto_aes_ccm_128_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 128 bit\r\n");
            break;

#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS_AES_CCM)
        case NRF_CRYPTO_KEY_SIZE_192:
            p_ccm_info = &g_nrf_crypto_aes_ccm_192_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 192 bit\r\n");
            break;

        case NRF_CRYPTO_KEY_SIZE_256:
            p_ccm_info = &g_nrf_crypto_aes_ccm_256_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 256 bit\r\n");
            break;
#endif

        default:
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_ERROR,
                            "AES key: %d bit key size.\r\n"
                            "Key size not supported for this backend.\r\n",
                            m_key_size);
            return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Nonce: ");
    for (uint8_t i = 0; i < ARRAY_SIZE(nonce); i++)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "0x%02x ", nonce[i]);
    }
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\r\n");

    /* Init encrypt and decrypt context */
    ret_val = nrf_crypto_aead_init(&ccm_encr_ctx,
                                   p_ccm_info,
                                   m_key);
    AES_ERROR_CHECK(p_cli, ret_val);
    ret_val = nrf_crypto_aead_init(&ccm_decr_ctx,
                                   p_ccm_info,
                                   m_key);
    AES_ERROR_CHECK(p_cli, ret_val);
    /* encrypt and tag text */
    ret_val = nrf_crypto_aead_crypt(&ccm_encr_ctx,
                                    NRF_CRYPTO_ENCRYPT,
                                    nonce,
                                    sizeof(nonce),
                                    adata,
                                    sizeof(adata),
                                    (uint8_t *)m_plain_text,
                                    len,
                                    (uint8_t *)encrypted_text,
                                    mac,
                                    sizeof(mac));
    AES_ERROR_CHECK(p_cli, ret_val);

    /* decrypt text */
    ret_val = nrf_crypto_aead_crypt(&ccm_decr_ctx,
                                    NRF_CRYPTO_DECRYPT,
                                    nonce,
                                    sizeof(nonce),
                                    adata,
                                    sizeof(adata),
                                    (uint8_t *)encrypted_text,
                                    len,
                                    (uint8_t *)decrypted_text,
                                    mac,
                                    sizeof(mac));
    AES_ERROR_CHECK(p_cli, ret_val);

    ret_val = nrf_crypto_aead_uninit(&ccm_encr_ctx);
    AES_ERROR_CHECK(p_cli, ret_val);
    ret_val = nrf_crypto_aead_uninit(&ccm_decr_ctx);
    AES_ERROR_CHECK(p_cli, ret_val);

    plain_text_print(p_cli);
    encrypted_text_print(p_cli, encrypted_text);
    mac_print(p_cli, mac, sizeof(mac));
    decrypted_text_print(p_cli, decrypted_text);
}
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM_STAR)
static void nrf_cli_cmd_ccm_star(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    uint32_t    len;
    uint8_t     mac[16];
    ret_code_t  ret_val;
    uint8_t     adata[] = {0xAA, 0xBB, 0xCC, 0xDD};
    char        encrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];
    char        decrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];
    uint8_t     nonce_star[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 7};

    nrf_crypto_aead_info_t const * p_ccm_star_info;
    nrf_crypto_aead_context_t      ccm_star_encr_ctx;
    nrf_crypto_aead_context_t      ccm_star_decr_ctx;

    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    memset(mac,                 0,  sizeof(mac));
    memset(encrypted_text,      0,  sizeof(encrypted_text));
    memset(decrypted_text,      0,  sizeof(encrypted_text));
    memset(&ccm_star_encr_ctx,  0,  sizeof(ccm_star_encr_ctx));
    memset(&ccm_star_decr_ctx,  0,  sizeof(ccm_star_decr_ctx));

    len = strlen((char const *)m_plain_text);

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES mode: CCM_STAR (CCM*) | ");

    switch (m_key_size)
    {
        case NRF_CRYPTO_KEY_SIZE_128:
            p_ccm_star_info = &g_nrf_crypto_aes_ccm_star_128_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 128 bit\r\n");
            break;

        default:
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_ERROR,
                            "AES key: %d bit key size.\r\n"
                            "Key size not supported for this backend.\r\n",
                            m_key_size);
            return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Nonce: ");
    for (uint8_t i = 0; i < ARRAY_SIZE(nonce_star); i++)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "0x%02x ", nonce_star[i]);
    }
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\r\n");

    /* Init encrypt and decrypt context */
    ret_val = nrf_crypto_aead_init(&ccm_star_encr_ctx,
                                   p_ccm_star_info,
                                   m_key);
    AES_ERROR_CHECK(p_cli, ret_val);
    ret_val = nrf_crypto_aead_init(&ccm_star_decr_ctx,
                                   p_ccm_star_info,
                                   m_key);
    AES_ERROR_CHECK(p_cli, ret_val);
    /* encrypt and tag text */
    ret_val = nrf_crypto_aead_crypt(&ccm_star_encr_ctx,
                                    NRF_CRYPTO_ENCRYPT,
                                    nonce_star,
                                    sizeof(nonce_star),
                                    adata,
                                    sizeof(adata),
                                    (uint8_t *)m_plain_text,
                                    len,
                                    (uint8_t *)encrypted_text,
                                    mac,
                                    sizeof(mac));
    AES_ERROR_CHECK(p_cli, ret_val);
    /* decrypt text */
    ret_val = nrf_crypto_aead_crypt(&ccm_star_decr_ctx,
                                    NRF_CRYPTO_DECRYPT,
                                    nonce_star,
                                    sizeof(nonce_star),
                                    adata,
                                    sizeof(adata),
                                    (uint8_t *)encrypted_text,
                                    len,
                                    (uint8_t *)decrypted_text,
                                    mac,
                                    sizeof(mac));
    AES_ERROR_CHECK(p_cli, ret_val);

    ret_val = nrf_crypto_aead_uninit(&ccm_star_encr_ctx);
    AES_ERROR_CHECK(p_cli, ret_val);
    ret_val = nrf_crypto_aead_uninit(&ccm_star_decr_ctx);
    AES_ERROR_CHECK(p_cli, ret_val);

    plain_text_print(p_cli);
    encrypted_text_print(p_cli, encrypted_text);
    mac_print(p_cli, mac, sizeof(mac));
    decrypted_text_print(p_cli, decrypted_text);
}
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM_STAR)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_EAX)
static void nrf_cli_cmd_eax(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    uint32_t    len;
    ret_code_t  ret_val;
    uint8_t     mac[16];
    uint8_t     nonce[15];
    uint8_t     adata[] = {0xAA, 0xBB, 0xCC, 0xDD};
    char        encrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];
    char        decrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];

    nrf_crypto_aead_info_t const * p_eax_info;
    nrf_crypto_aead_context_t      eax_encr_ctx;
    nrf_crypto_aead_context_t      eax_decr_ctx;

    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    memset(mac,             0,  sizeof(mac));
    memset(nonce,           0,  sizeof(nonce));
    memset(encrypted_text,  0,  sizeof(encrypted_text));
    memset(decrypted_text,  0,  sizeof(encrypted_text));
    memset(&eax_encr_ctx,   0,  sizeof(eax_encr_ctx));
    memset(&eax_decr_ctx,   0,  sizeof(eax_decr_ctx));

    len = strlen((char const *)m_plain_text);

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES mode: EAX | ");

    switch (m_key_size)
    {
        case NRF_CRYPTO_KEY_SIZE_128:
            p_eax_info = &g_nrf_crypto_aes_eax_128_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 128 bit\r\n");
            break;

        case NRF_CRYPTO_KEY_SIZE_192:
            p_eax_info = &g_nrf_crypto_aes_eax_192_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 192 bit\r\n");
            break;

        case NRF_CRYPTO_KEY_SIZE_256:
            p_eax_info = &g_nrf_crypto_aes_eax_256_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 256 bit\r\n");
            break;

        default:
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_ERROR,
                            "AES key: %d bit key size.\r\n"
                            "Key size not supported for this backend.\r\n",
                            m_key_size);
            return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Nonce: ");
    for (uint8_t i = 0; i < ARRAY_SIZE(nonce); i++)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "0x%02x ", nonce[i]);
    }
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\r\n");

    /* Init encrypt and decrypt context */
    ret_val = nrf_crypto_aead_init(&eax_encr_ctx,
                                   p_eax_info,
                                   m_key);
    AES_ERROR_CHECK(p_cli, ret_val);
    ret_val = nrf_crypto_aead_init(&eax_decr_ctx,
                                   p_eax_info,
                                   m_key);
    AES_ERROR_CHECK(p_cli, ret_val);
    /* encrypt and tag text */
    ret_val = nrf_crypto_aead_crypt(&eax_encr_ctx,
                                    NRF_CRYPTO_ENCRYPT,
                                    nonce,
                                    sizeof(nonce),
                                    adata,
                                    sizeof(adata),
                                    (uint8_t *)m_plain_text,
                                    len,
                                    (uint8_t *)encrypted_text,
                                    mac,
                                    sizeof(mac));
    AES_ERROR_CHECK(p_cli, ret_val);
    /* decrypt text */
    ret_val = nrf_crypto_aead_crypt(&eax_decr_ctx,
                                    NRF_CRYPTO_DECRYPT,
                                    nonce,
                                    sizeof(nonce),
                                    adata,
                                    sizeof(adata),
                                    (uint8_t *)encrypted_text,
                                    len,
                                    (uint8_t *)decrypted_text,
                                    mac,
                                    sizeof(mac));
    AES_ERROR_CHECK(p_cli, ret_val);

    ret_val = nrf_crypto_aead_uninit(&eax_encr_ctx);
    AES_ERROR_CHECK(p_cli, ret_val);
    ret_val = nrf_crypto_aead_uninit(&eax_decr_ctx);
    AES_ERROR_CHECK(p_cli, ret_val);

    plain_text_print(p_cli);
    encrypted_text_print(p_cli, encrypted_text);
    mac_print(p_cli, mac, sizeof(mac));
    decrypted_text_print(p_cli, decrypted_text);
}
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_EAX)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_GCM)
static void nrf_cli_cmd_gcm(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    uint32_t    len;
    ret_code_t  ret_val;
    uint8_t     mac[16];
    uint8_t     nonce[13];
    uint8_t     adata[] = {0xAA, 0xBB, 0xCC, 0xDD};
    char        encrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];
    char        decrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];

    nrf_crypto_aead_info_t const * p_gcm_info;
    nrf_crypto_aead_context_t      gcm_encr_ctx;
    nrf_crypto_aead_context_t      gcm_decr_ctx;

    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    memset(mac,             0,  sizeof(mac));
    memset(nonce,           0,  sizeof(nonce));
    memset(encrypted_text,  0,  sizeof(encrypted_text));
    memset(decrypted_text,  0,  sizeof(encrypted_text));
    memset(&gcm_encr_ctx,   0,  sizeof(gcm_encr_ctx));
    memset(&gcm_decr_ctx,   0,  sizeof(gcm_decr_ctx));

    len = strlen((char const *)m_plain_text);

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES mode: GCM | ");

    switch (m_key_size)
    {
        case NRF_CRYPTO_KEY_SIZE_128:
            p_gcm_info = &g_nrf_crypto_aes_gcm_128_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 128 bit\r\n");
            break;

        case NRF_CRYPTO_KEY_SIZE_192:
            p_gcm_info = &g_nrf_crypto_aes_gcm_192_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 192 bit\r\n");
            break;

        case NRF_CRYPTO_KEY_SIZE_256:
            p_gcm_info = &g_nrf_crypto_aes_gcm_256_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 256 bit\r\n");
            break;

        default:
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_ERROR,
                            "AES key: %d bit key size.\r\n"
                            "Key size not supported for this backend.\r\n",
                            m_key_size);
            return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Nonce: ");
    for (uint8_t i = 0; i < ARRAY_SIZE(nonce); i++)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "0x%02x ", nonce[i]);
    }
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\r\n");

    /* Init encrypt and decrypt context */
    ret_val = nrf_crypto_aead_init(&gcm_encr_ctx,
                                   p_gcm_info,
                                   m_key);
    AES_ERROR_CHECK(p_cli, ret_val);
    ret_val = nrf_crypto_aead_init(&gcm_decr_ctx,
                                   p_gcm_info,
                                   m_key);
    AES_ERROR_CHECK(p_cli, ret_val);
    /* encrypt and tag text */
    ret_val = nrf_crypto_aead_crypt(&gcm_encr_ctx,
                                    NRF_CRYPTO_ENCRYPT,
                                    nonce,
                                    sizeof(nonce),
                                    adata,
                                    sizeof(adata),
                                    (uint8_t *)m_plain_text,
                                    len,
                                    (uint8_t *)encrypted_text,
                                    mac,
                                    sizeof(mac));
    AES_ERROR_CHECK(p_cli, ret_val);
    /* decrypt text */
    ret_val = nrf_crypto_aead_crypt(&gcm_decr_ctx,
                                    NRF_CRYPTO_DECRYPT,
                                    nonce,
                                    sizeof(nonce),
                                    adata,
                                    sizeof(adata),
                                    (uint8_t *)encrypted_text,
                                    len,
                                    (uint8_t *)decrypted_text,
                                    mac,
                                    sizeof(mac));
    AES_ERROR_CHECK(p_cli, ret_val);

    ret_val = nrf_crypto_aead_uninit(&gcm_encr_ctx);
    AES_ERROR_CHECK(p_cli, ret_val);
    ret_val = nrf_crypto_aead_uninit(&gcm_decr_ctx);
    AES_ERROR_CHECK(p_cli, ret_val);

    plain_text_print(p_cli);
    encrypted_text_print(p_cli, encrypted_text);
    mac_print(p_cli, mac, sizeof(mac));
    decrypted_text_print(p_cli, decrypted_text);
}
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_GCM)


#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC)
static void nrf_cli_cmd_crypt_cbc(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    size_t      len_in;
    size_t      len_out;
    ret_code_t  ret_val;
    uint8_t     iv[16];
    char        encrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];
    char        decrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];

    nrf_crypto_aes_info_t const * p_cbc_info;
    nrf_crypto_aes_context_t      cbc_encr_ctx;
    nrf_crypto_aes_context_t      cbc_decr_ctx;

    memset(iv, 0, sizeof(iv));
    memset(encrypted_text, 0, sizeof(encrypted_text));
    memset(decrypted_text, 0, sizeof(decrypted_text));

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES mode: CBC | ");

    switch (m_key_size)
    {
        case NRF_CRYPTO_KEY_SIZE_128:
            p_cbc_info = &g_nrf_crypto_aes_cbc_128_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 128 bit\r\n");
            break;

#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS_AES_CBC)
        case NRF_CRYPTO_KEY_SIZE_192:
            p_cbc_info = &g_nrf_crypto_aes_cbc_192_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 192 bit\r\n");
            break;

        case NRF_CRYPTO_KEY_SIZE_256:
            p_cbc_info = &g_nrf_crypto_aes_cbc_256_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 256 bit\r\n");
            break;
#endif

        default:
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_ERROR,
                            "AES key: %d bit key size.\r\n"
                            "Key size not supported for this backend.\r\n",
                            m_key_size);
            return;
    }

    /* Init encrypt and decrypt context */
    ret_val = nrf_crypto_aes_init(&cbc_encr_ctx,
                                  p_cbc_info,
                                  NRF_CRYPTO_ENCRYPT);
    AES_ERROR_CHECK(p_cli, ret_val);

    /* Set encryption and decryption key */
    ret_val = nrf_crypto_aes_key_set(&cbc_encr_ctx, m_key);
    AES_ERROR_CHECK(p_cli, ret_val);

    /* Set IV */
    ret_val = nrf_crypto_aes_iv_set(&cbc_encr_ctx, iv);
    AES_ERROR_CHECK(p_cli, ret_val);

    len_in = strlen(m_plain_text);
    len_out = sizeof(encrypted_text);

    /* Encrypt */
    ret_val = nrf_crypto_aes_finalize(&cbc_encr_ctx,
                                      (uint8_t *)m_plain_text,
                                      len_in,
                                      (uint8_t *)encrypted_text,
                                      &len_out);
    AES_ERROR_CHECK(p_cli, ret_val);

    /* Decrypt text with integrated function */
    len_out = sizeof(decrypted_text);
    ret_val = nrf_crypto_aes_crypt(&cbc_decr_ctx,
                                   p_cbc_info,
                                   NRF_CRYPTO_DECRYPT,
                                   m_key,
                                   iv,
                                   (uint8_t *)encrypted_text,
                                   len_in,
                                   (uint8_t *)decrypted_text,
                                   &len_out);
    AES_ERROR_CHECK(p_cli, ret_val);


    plain_text_print(p_cli);
    encrypted_text_print(p_cli, encrypted_text);
    decrypted_text_print(p_cli, decrypted_text);
}
#endif

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_MAC)
static void nrf_cli_cmd_crypt_cbc_mac(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    uint8_t     iv[16];
    uint8_t     mac[16];
    ret_code_t  ret_val;
    size_t      len_in;
    size_t      len_out;

    nrf_crypto_aes_info_t const * p_cbc_mac_info;
    nrf_crypto_aes_context_t      cbc_mac_ctx;

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES mode: CBC MAC | ");

    switch (m_key_size)
    {
        case NRF_CRYPTO_KEY_SIZE_128:
            p_cbc_mac_info = &g_nrf_crypto_aes_cbc_mac_128_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 128 bit\r\n");
            break;

#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS_AES_CBC_MAC)
        case NRF_CRYPTO_KEY_SIZE_192:
            p_cbc_mac_info = &g_nrf_crypto_aes_cbc_mac_192_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 128 bit\r\n");
            break;

        case NRF_CRYPTO_KEY_SIZE_256:
            p_cbc_mac_info = &g_nrf_crypto_aes_cbc_mac_256_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 128 bit\r\n");
            break;
#endif

        default:
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_ERROR,
                            "AES key: %d bit key size.\r\n"
                            "Key size not supported for this backend.\r\n",
                            m_key_size);
            return;
    }

    /* Init MAC context */
    ret_val = nrf_crypto_aes_init(&cbc_mac_ctx,
                                  p_cbc_mac_info,
                                  NRF_CRYPTO_MAC_CALCULATE);
    AES_ERROR_CHECK(p_cli, ret_val);

    /* Set encryption key */
    ret_val = nrf_crypto_aes_key_set(&cbc_mac_ctx, m_key);
    AES_ERROR_CHECK(p_cli, ret_val);

    /* Set IV */
    memset(iv, 0, sizeof(iv));

    ret_val = nrf_crypto_aes_iv_set(&cbc_mac_ctx, iv);
    AES_ERROR_CHECK(p_cli, ret_val);

    len_in  = strlen(m_plain_text);    // must be multiple of 16 bytes
    len_out = sizeof(mac);
    memset(mac, 0, sizeof(mac));

    /* Caclculate MAC */
    ret_val = nrf_crypto_aes_finalize(&cbc_mac_ctx,
                                      (uint8_t *)m_plain_text,
                                      len_in,
                                      (uint8_t *)mac,
                                      &len_out);
    AES_ERROR_CHECK(p_cli, ret_val);

    plain_text_print(p_cli);
    mac_print(p_cli, mac, len_out);
}
#endif

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CFB)
static void nrf_cli_cmd_crypt_cfb(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    size_t      len_in;
    size_t      len_out;
    uint8_t     iv[16];
    ret_code_t  ret_val;
    char        encrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];
    char        decrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];

    nrf_crypto_aes_info_t const * p_cfb_info;
    nrf_crypto_aes_context_t      cfb_encr_ctx;

    memset(iv, 0, sizeof(iv));
    memset(encrypted_text, 0, sizeof(encrypted_text));
    memset(decrypted_text, 0, sizeof(decrypted_text));

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES mode: CFB | ");

    switch (m_key_size)
    {
        case NRF_CRYPTO_KEY_SIZE_128:
            p_cfb_info = &g_nrf_crypto_aes_cfb_128_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key:  128 bit\r\n");
            break;

        case NRF_CRYPTO_KEY_SIZE_192:
            p_cfb_info = &g_nrf_crypto_aes_cfb_192_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key:  192 bit\r\n");
            break;

        case NRF_CRYPTO_KEY_SIZE_256:
            p_cfb_info = &g_nrf_crypto_aes_cfb_256_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key:  256 bit\r\n");
            break;

        default:
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_ERROR,
                            "AES key: %d bit key size.\r\n"
                            "Key size not supported for this backend.\r\n",
                            m_key_size);
            return;
    }

    /* Init encrypt context */
    ret_val = nrf_crypto_aes_init(&cfb_encr_ctx,
                                  p_cfb_info,
                                  NRF_CRYPTO_ENCRYPT);
    AES_ERROR_CHECK(p_cli, ret_val);

    /* Set encryption and decryption key */
    ret_val = nrf_crypto_aes_key_set(&cfb_encr_ctx, m_key);
    AES_ERROR_CHECK(p_cli, ret_val);

    /* Set IV */
    ret_val = nrf_crypto_aes_iv_set(&cfb_encr_ctx, iv);
    AES_ERROR_CHECK(p_cli, ret_val);

    len_in  = strlen(m_plain_text);
    len_out = sizeof(encrypted_text);

    /* Encrypt blocks */
    ret_val = nrf_crypto_aes_finalize(&cfb_encr_ctx,
                                      (uint8_t *)m_plain_text,
                                      len_in,
                                      (uint8_t *)encrypted_text,
                                      &len_out);
    AES_ERROR_CHECK(p_cli, ret_val);

    /* Decrypt text with integrated function */
    len_out = sizeof(decrypted_text);
    ret_val = nrf_crypto_aes_crypt(NULL,
                                   p_cfb_info,
                                   NRF_CRYPTO_DECRYPT,
                                   m_key,
                                   iv,
                                   (uint8_t *)encrypted_text,
                                   len_in,
                                   (uint8_t *)decrypted_text,
                                   &len_out);
    AES_ERROR_CHECK(p_cli, ret_val);

    plain_text_print(p_cli);
    encrypted_text_print(p_cli, encrypted_text);
    decrypted_text_print(p_cli, decrypted_text);
}
#endif

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CMAC)
static void nrf_cli_cmd_crypt_cmac(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    uint8_t     mac[16];
    ret_code_t  ret_val;
    size_t      len_in;
    size_t      len_out;

    nrf_crypto_aes_context_t      mac_ctx;
    nrf_crypto_aes_info_t const * p_cmac_info;

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES mode: CMAC | ");

    switch (m_key_size)
    {
        case NRF_CRYPTO_KEY_SIZE_128:
            p_cmac_info = &g_nrf_crypto_aes_cmac_128_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 128 bit\r\n");
            break;

#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS_AES_CMAC)
        case NRF_CRYPTO_KEY_SIZE_192:
            p_cmac_info = &g_nrf_crypto_aes_cmac_192_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 192 bit\r\n");
            break;

        case NRF_CRYPTO_KEY_SIZE_256:
            p_cmac_info = &g_nrf_crypto_aes_cmac_256_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key: 256 bit\r\n");
            break;
#endif

        default:
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_ERROR,
                            "AES key: %d bit key size.\r\n"
                            "Key size not supported for this backend.\r\n",
                            m_key_size);
            return;
    }

    /* Init MAC context */
    ret_val = nrf_crypto_aes_init(&mac_ctx,
                                  p_cmac_info,
                                  NRF_CRYPTO_MAC_CALCULATE);
    AES_ERROR_CHECK(p_cli, ret_val);

    /* Set encryption key */
    ret_val = nrf_crypto_aes_key_set(&mac_ctx, m_key);
    AES_ERROR_CHECK(p_cli, ret_val);

    memset(mac, 0, sizeof(mac));
    len_in  = strlen(m_plain_text);
    len_out = sizeof(mac);

    /* Caclculate MAC */
    ret_val = nrf_crypto_aes_finalize(&mac_ctx,
                                      (uint8_t *)m_plain_text,
                                      len_in,
                                      (uint8_t *)mac,
                                      &len_out);
    AES_ERROR_CHECK(p_cli, ret_val);

    plain_text_print(p_cli);
    mac_print(p_cli, mac, len_out);
}
#endif

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CTR)
static void nrf_cli_cmd_crypt_ctr(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    size_t      len_in;
    size_t      len_out;
    uint8_t     iv[16];
    ret_code_t  ret_val;
    char        encrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];
    char        decrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];

    nrf_crypto_aes_info_t const * p_ctr_info;
    nrf_crypto_aes_context_t      ctr_encr_ctx;
    nrf_crypto_aes_context_t      ctr_decr_ctx;

    memset(iv, 0, sizeof(iv));
    memset(encrypted_text, 0, sizeof(encrypted_text));
    memset(decrypted_text, 0, sizeof(decrypted_text));

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES mode: CTR | ");

    switch (m_key_size)
    {
        case NRF_CRYPTO_KEY_SIZE_128:
            p_ctr_info = &g_nrf_crypto_aes_ctr_128_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key:  128 bit\r\n");
            break;

#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS_AES_CTR)
        case NRF_CRYPTO_KEY_SIZE_192:
            p_ctr_info = &g_nrf_crypto_aes_ctr_192_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key:  192 bit\r\n");
            break;

        case NRF_CRYPTO_KEY_SIZE_256:
            p_ctr_info = &g_nrf_crypto_aes_ctr_256_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key:  256 bit\r\n");
            break;
#endif

        default:
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_ERROR,
                            "AES key: %d bit key size.\r\n"
                            "Key size not supported for this backend.\r\n",
                            m_key_size);
            return;
    }

    /* Init encrypt and decrypt context */
    ret_val = nrf_crypto_aes_init(&ctr_encr_ctx,
                                  p_ctr_info,
                                  NRF_CRYPTO_ENCRYPT);
    AES_ERROR_CHECK(p_cli, ret_val);
    ret_val = nrf_crypto_aes_init(&ctr_decr_ctx,
                                  p_ctr_info,
                                  NRF_CRYPTO_DECRYPT);
    AES_ERROR_CHECK(p_cli, ret_val);

    /* Set encryption and decryption key */
    ret_val = nrf_crypto_aes_key_set(&ctr_encr_ctx, m_key);
    AES_ERROR_CHECK(p_cli, ret_val);
    ret_val = nrf_crypto_aes_key_set(&ctr_decr_ctx, m_key);
    AES_ERROR_CHECK(p_cli, ret_val);

    /* Set IV */
    ret_val = nrf_crypto_aes_iv_set(&ctr_encr_ctx, iv);
    AES_ERROR_CHECK(p_cli, ret_val);
    ret_val = nrf_crypto_aes_iv_set(&ctr_decr_ctx, iv);
    AES_ERROR_CHECK(p_cli, ret_val);

    len_in = strlen(m_plain_text);
    len_out = sizeof(encrypted_text);
    /* Encrypt blocks */
    ret_val = nrf_crypto_aes_finalize(&ctr_encr_ctx,
                                      (uint8_t *)m_plain_text,
                                      len_in,
                                      (uint8_t *)encrypted_text,
                                      &len_out);
    AES_ERROR_CHECK(p_cli, ret_val);

    /* Decrypt blocks */
    len_out = sizeof(decrypted_text);
    ret_val = nrf_crypto_aes_finalize(&ctr_decr_ctx,
                                      (uint8_t *)encrypted_text,
                                      len_in,
                                      (uint8_t *)decrypted_text,
                                      &len_out);
    AES_ERROR_CHECK(p_cli, ret_val);

    plain_text_print(p_cli);
    encrypted_text_print(p_cli, encrypted_text);
    decrypted_text_print(p_cli, decrypted_text);
}
#endif


#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_ECB)
static void nrf_cli_cmd_crypt_ecb(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    size_t      len_in;
    size_t      len_out;
    ret_code_t  ret_val;
    char        encrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];
    char        decrypted_text[NRF_CRYPTO_EXAMPLE_AES_MAX_TEXT_SIZE];

    nrf_crypto_aes_info_t const * p_ecb_info;
    nrf_crypto_aes_context_t      ecb_encr_ctx;
    nrf_crypto_aes_context_t      ecb_decr_ctx;

    memset(encrypted_text, 0, sizeof(encrypted_text));
    memset(decrypted_text, 0, sizeof(decrypted_text));

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES mode: ECB | ");

    switch (m_key_size)
    {
        case NRF_CRYPTO_KEY_SIZE_128:
            p_ecb_info = &g_nrf_crypto_aes_ecb_128_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key:  128 bit\r\n");
            break;

#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS_AES_ECB)
        case NRF_CRYPTO_KEY_SIZE_192:
            p_ecb_info = &g_nrf_crypto_aes_ecb_192_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key:  192 bit\r\n");
            break;

        case NRF_CRYPTO_KEY_SIZE_256:
            p_ecb_info = &g_nrf_crypto_aes_ecb_256_info;
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "AES key:  256 bit\r\n");
            break;
#endif

        default:
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_ERROR,
                            "AES key: %d bit key size.\r\n"
                            "Key size not supported for this backend.\r\n",
                            m_key_size);
            return;
    }

    len_in  = strlen(m_plain_text);
    len_out = sizeof(decrypted_text);
    /* Encrypt text with integrated function */
    ret_val = nrf_crypto_aes_crypt(&ecb_encr_ctx,
                                   p_ecb_info,
                                   NRF_CRYPTO_ENCRYPT,
                                   m_key,
                                   NULL,
                                   (uint8_t *)m_plain_text,
                                   len_in,
                                   (uint8_t *)encrypted_text,
                                   &len_out);
    AES_ERROR_CHECK(p_cli, ret_val);

    ret_val = nrf_crypto_aes_init(&ecb_decr_ctx,
                                  p_ecb_info,
                                  NRF_CRYPTO_DECRYPT);
    AES_ERROR_CHECK(p_cli, ret_val);

    /* Set encryption and decryption key */

    ret_val = nrf_crypto_aes_key_set(&ecb_decr_ctx, m_key);
    AES_ERROR_CHECK(p_cli, ret_val);

    /* Decrypt blocks */
    len_out = sizeof(decrypted_text);
    ret_val = nrf_crypto_aes_finalize(&ecb_decr_ctx,
                                      (uint8_t *)encrypted_text,
                                      len_in,
                                      (uint8_t *)decrypted_text,
                                      &len_out);
    AES_ERROR_CHECK(p_cli, ret_val);

    plain_text_print(p_cli);
    encrypted_text_print(p_cli, encrypted_text);
    decrypted_text_print(p_cli, decrypted_text);
}
#endif


static void nrf_cli_cmd_key_size_128bit(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if (argc == 1)
    {
        m_key_size = NRF_CRYPTO_KEY_SIZE_128;
        return;
    }
    if ((argc == 2) && nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }
    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n", argv[0]);
}

static void nrf_cli_cmd_key_size_192bit(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if (argc == 1)
    {
        m_key_size = NRF_CRYPTO_KEY_SIZE_192;
        return;
    }
    if ((argc == 2) && nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }
    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n", argv[0]);
}

static void nrf_cli_cmd_key_size_256bit(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if (argc == 1)
    {
        m_key_size = NRF_CRYPTO_KEY_SIZE_256;
        return;
    }
    if ((argc == 2) && nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }
    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n", argv[0]);
}

static void nrf_cli_cmd_key_size_set(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if (argc > 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n", argv[0]);
        return;
    }
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    nrf_cli_fprintf(p_cli,
                    NRF_CLI_ERROR,
                    "%s: key size not correct: %s\r\n",
                    argv[0],
                    argv[1]);
}

static void nrf_cli_cmd_aead(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if (argc > 3)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n", argv[0]);
        return;
    }
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }
    nrf_cli_fprintf(p_cli,
                    NRF_CLI_ERROR,
                    "%s: aead mode not supported: %s\r\n",
                    argv[0],
                    argv[1]);
}


static void nrf_cli_cmd_crypt(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if (argc > 3)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n", argv[0]);
        return;
    }
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }
    nrf_cli_fprintf(p_cli,
                    NRF_CLI_ERROR,
                    "%s: crypt mode not supported: %s\r\n",
                    argv[0],
                    argv[1]);
}

static void nrf_cli_cmd_mac_calc(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if (argc > 3)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n", argv[0]);
        return;
    }
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }
    nrf_cli_fprintf(p_cli,
                    NRF_CLI_ERROR,
                    "%s: MAC mode not supported: %s\r\n",
                    argv[0],
                    argv[1]);
}

static void nrf_cli_cmd_example(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }
    nrf_cli_fprintf(p_cli,
                    NRF_CLI_ERROR,
                    "Please specify correct subcommand.\r\n");
}

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_aead)
{
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM)
    NRF_CLI_CMD(ccm,            NULL,   HELP_CMD_CCM,       nrf_cli_cmd_ccm),
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM_STAR)
    NRF_CLI_CMD(ccm_star,       NULL,   HELP_CMD_CCM_STAR,  nrf_cli_cmd_ccm_star),
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_EAX)
    NRF_CLI_CMD(eax,            NULL,   HELP_CMD_EAX,       nrf_cli_cmd_eax),
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_GCM)
    NRF_CLI_CMD(gcm,            NULL,   HELP_CMD_GCM,       nrf_cli_cmd_gcm),
#endif
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_aes)
{
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC)
    NRF_CLI_CMD(cbc,    NULL,   HELP_CMD_CBC,   nrf_cli_cmd_crypt_cbc),
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CFB)
    NRF_CLI_CMD(cfb,    NULL,   HELP_CMD_CFB,   nrf_cli_cmd_crypt_cfb),
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CTR)
    NRF_CLI_CMD(ctr,    NULL,   HELP_CMD_CTR,   nrf_cli_cmd_crypt_ctr),
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_ECB)
    NRF_CLI_CMD(ecb,    NULL,   HELP_CMD_ECB,   nrf_cli_cmd_crypt_ecb),
#endif
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_mac)
{
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_MAC)
    NRF_CLI_CMD(cbc_mac,        NULL,   HELP_CMD_CBC_MAC,   nrf_cli_cmd_crypt_cbc_mac),
#endif
#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CMAC)
    NRF_CLI_CMD(cmac,           NULL,   HELP_CMD_CMAC,      nrf_cli_cmd_crypt_cmac),
#endif
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_key_size)
{
    NRF_CLI_CMD(128_bit, NULL,   HELP_CMD_KEY128,   nrf_cli_cmd_key_size_128bit),
    NRF_CLI_CMD(192_bit, NULL,   HELP_CMD_KEY192,   nrf_cli_cmd_key_size_192bit),
    NRF_CLI_CMD(256_bit, NULL,   HELP_CMD_KEY256,   nrf_cli_cmd_key_size_256bit),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_example)
{
    NRF_CLI_CMD(aead,       &m_sub_aead,        HELP_CMD_AEAD,      nrf_cli_cmd_aead),
    NRF_CLI_CMD(aes,        &m_sub_aes,         HELP_CMD_CRYPT,     nrf_cli_cmd_crypt),
    NRF_CLI_CMD(key_size,   &m_sub_key_size,    HELP_CMD_KEY_SET,   nrf_cli_cmd_key_size_set),
    NRF_CLI_CMD(mac,        &m_sub_mac,         HELP_CMD_MAC_CALC,  nrf_cli_cmd_mac_calc),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CMD_REGISTER(example,   &m_sub_example, HELP_CMD_HELP,      nrf_cli_cmd_example);

int main(void)
{
    ret_code_t ret;

    APP_ERROR_CHECK(NRF_LOG_INIT(app_timer_cnt_get));

    ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret);
    nrf_drv_clock_lfclk_request(NULL);

    APP_ERROR_CHECK(ret);
    ret = app_timer_init();

    nrf_drv_uart_config_t uart_config = NRF_DRV_UART_DEFAULT_CONFIG;
    uart_config.pseltxd = TX_PIN_NUMBER;
    uart_config.pselrxd = RX_PIN_NUMBER;
    uart_config.hwfc    = NRF_UART_HWFC_DISABLED;
    ret = nrf_cli_init(&m_cli_uart, &uart_config, true, true, NRF_LOG_SEVERITY_INFO);
    APP_ERROR_CHECK(ret);

    ret = nrf_cli_start(&m_cli_uart);
    APP_ERROR_CHECK(ret);

    ret = nrf_crypto_init();
    APP_ERROR_CHECK(ret);

    strcpy(m_plain_text, mp_default_text);

    NRF_LOG_RAW_INFO("nrf_crypto AES command line interface example started.\r\n");
    NRF_LOG_RAW_INFO("Please press the Tab key to see all available commands.\r\n");
    NRF_LOG_FLUSH();

    ret = nrf_mem_init();
    APP_ERROR_CHECK(ret);

    while (true)
    {
        UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
        nrf_cli_process(&m_cli_uart);
    }
}

/** @} */
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO)

