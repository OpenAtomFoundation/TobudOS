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
#include <stdarg.h>
#include <string.h>
#include <stddef.h>
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_error.h"
#include "app_util.h"
#include "nrf_section.h"
#include "nrf_crypto.h"
#include "nrf_crypto_hash.h"
#include "common_test.h"

/**@brief ChaCHa Poly test vectors can be found in RFC 7539 document.
 *
 * https://tools.ietf.org/html/rfc7539
 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_CHACHA_POLY)

/*lint -save -e91 */

// Multiple used ChaCha Poly test vectors.
const char chacha_poly_plain_114[]   = { "4c616469657320616e642047656e746c656d656e206f662074686520636c617373206f66202739393a204966204920636f756c64206f6666657220796f75206f6e6c79206f6e652074697020666f7220746865206675747572652c2073756e73637265656e20776f756c642062652069742e" };
const char chacha_poly_cipher_114[]  = { "d31a8d34648e60db7b86afbc53ef7ec2a4aded51296e08fea9e2b5a736ee62d63dbea45e8ca9671282fafb69da92728b1a71de0a9e060b2905d6a5b67ecd3b3692ddbd7f2d778b8c9803aee328091b58fab324e4fad675945585808b4831d7bc3ff4def08e4b7a9de576d26586cec64b6116" };
const char chacha_poly_key[]         = { "808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f" };
const char chacha_poly_ad[]          = { "50515253c0c1c2c3c4c5c6c7" };
const char chacha_poly_nonce[]       = { "070000004041424344454647" };
const char chacha_poly_mac[]         = { "1ae10b594f09e26a7e902ecbd0600691" };
const char chacha_poly_invalid_key[] = { "908182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f" };
const char chacha_poly_invalid_mac[] = { "2ae10b594f09e26a7e902ecbd0600691" };

// ChaCha Poly - RFC 7539 - section A.5
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_aead_chacha_poly_full_1) =
{
    .p_aead_info            = &g_nrf_crypto_chacha_poly_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "ChaCha Poly message_len=265 ad_len=12",
    .p_plaintext            = "496e7465726e65742d4472616674732061726520647261667420646f63756d656e74732076616c696420666f722061206d6178696d756d206f6620736978206d6f6e74687320616e64206d617920626520757064617465642c207265706c616365642c206f72206f62736f6c65746564206279206f7468657220646f63756d656e747320617420616e792074696d652e20497420697320696e617070726f70726961746520746f2075736520496e7465726e65742d447261667473206173207265666572656e6365206d6174657269616c206f7220746f2063697465207468656d206f74686572207468616e206173202fe2809c776f726b20696e2070726f67726573732e2fe2809d",
    .p_ciphertext           = "64a0861575861af460f062c79be643bd5e805cfd345cf389f108670ac76c8cb24c6cfc18755d43eea09ee94e382d26b0bdb7b73c321b0100d4f03b7f355894cf332f830e710b97ce98c8a84abd0b948114ad176e008d33bd60f982b1ff37c8559797a06ef4f0ef61c186324e2b3506383606907b6a7c02b0f9f6157b53c867e4b9166c767b804d46a59b5216cde7a4e99040c5a40433225ee282a1b0a06c523eaf4534d7f83fa1155b0047718cbc546a0d072b04b3564eea1b422273f548271a0bb2316053fa76991955ebd63159434ecebb4e466dae5a1073a6727627097a1049e617d91d361094fa68f0ff77987130305beaba2eda04df997b714d6c6f2c29a6ad5cb4022b02709b",
    .p_key                  = "1c9240a5eb55d38af333888604f6b5f0473917c1402b80099dca5cbc207075c0",
    .p_ad                   = "f33388860000000000004e91",
    .p_nonce                = "000000000102030405060708",
    .p_mac                  = "eead9d67890cbb22392336fea1851f38"
};

// ChaCha Poly - RFC 7539 - section 2.8.2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_aead_chacha_poly_full_2) =
{
    .p_aead_info            = &g_nrf_crypto_chacha_poly_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "ChaCha Poly message_len=114 ad_len=12",
    .p_plaintext            = chacha_poly_plain_114,
    .p_ciphertext           = chacha_poly_cipher_114,
    .p_key                  = chacha_poly_key,
    .p_ad                   = chacha_poly_ad,
    .p_nonce                = chacha_poly_nonce,
    .p_mac                  = chacha_poly_mac
};

// ChaCha Poly - Based on RFC 7539 - section 2.8.2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_aead_chacha_poly_inv_1) =
{
    .p_aead_info            = &g_nrf_crypto_chacha_poly_256_info,
    .expected_err_code      = NRF_ERROR_CRYPTO_INPUT_LENGTH,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "ChaCha Poly Invalid ad_len=0",
    .p_plaintext            = chacha_poly_plain_114,
    .p_ciphertext           = chacha_poly_cipher_114,
    .p_key                  = chacha_poly_key,
    .p_ad                   = "",
    .p_nonce                = chacha_poly_nonce,
    .p_mac                  = chacha_poly_mac
};

// ChaCha Poly - Based on RFC 7539 - section 2.8.2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_aead_chacha_poly_inv_nonce) =
{
    .p_aead_info            = &g_nrf_crypto_chacha_poly_256_info,
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_NONCE_SIZE,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "ChaCha Poly Invalid nonce_len=12",
    .p_plaintext            = chacha_poly_plain_114,
    .p_ciphertext           = chacha_poly_cipher_114,
    .p_key                  = chacha_poly_key,
    .p_ad                   = chacha_poly_ad,
    .p_nonce                = "0000000001020304050607",
    .p_mac                  = chacha_poly_mac
};

// ChaCha Poly - Based on RFC 7539 - section 2.8.2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_aead_chacha_poly_inv_mac_len) =
{
    .p_aead_info            = &g_nrf_crypto_chacha_poly_256_info,
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_MAC_SIZE,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "ChaCha Poly Invalid mac_len=15",
    .p_plaintext            = chacha_poly_plain_114,
    .p_ciphertext           = chacha_poly_cipher_114,
    .p_key                  = chacha_poly_key,
    .p_ad                   = chacha_poly_ad,
    .p_nonce                = chacha_poly_nonce,
    .p_mac                  = "1ae10b594f09e26a7e902ecbd06006"
};

// ChaCha Poly - Based on RFC 7539 - section 2.8.2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_chacha_poly_inv_key_encrypt) =
{
    .p_aead_info            = &g_nrf_crypto_chacha_poly_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "ChaCha Poly Encrypt Invalid key",
    .p_plaintext            = chacha_poly_plain_114,
    .p_ciphertext           = chacha_poly_cipher_114,
    .p_key                  = chacha_poly_invalid_key,
    .p_ad                   = chacha_poly_ad,
    .p_nonce                = chacha_poly_nonce,
    .p_mac                  = chacha_poly_mac
};

// ChaCha Poly - Based on RFC 7539 - section 2.8.2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_chacha_poly_inv_key_decrypt) =
{
    .p_aead_info            = &g_nrf_crypto_chacha_poly_256_info,
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "ChaCha Poly Decrypt Invalid key",
    .p_plaintext            = chacha_poly_plain_114,
    .p_ciphertext           = chacha_poly_cipher_114,
    .p_key                  = chacha_poly_invalid_key,
    .p_ad                   = chacha_poly_ad,
    .p_nonce                = chacha_poly_nonce,
    .p_mac                  = chacha_poly_mac
};

// ChaCha Poly - Based on RFC 7539 - section 2.8.2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_chacha_poly_inv_mac_encrypt) =
{
    .p_aead_info            = &g_nrf_crypto_chacha_poly_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "ChaCha Poly Encrypt Invalid mac",
    .p_plaintext            = chacha_poly_plain_114,
    .p_ciphertext           = chacha_poly_cipher_114,
    .p_key                  = chacha_poly_key,
    .p_ad                   = chacha_poly_ad,
    .p_nonce                = chacha_poly_nonce,
    .p_mac                  = chacha_poly_invalid_mac
};

// ChaCha Poly - Based on RFC 7539 - section 2.8.2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_chacha_poly_inv_mac_decrypt) =
{
    .p_aead_info            = &g_nrf_crypto_chacha_poly_256_info,
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#if NRF_CRYPTO_BACKEND_OBERON_ENABLED
    .crypt_expected_result  = EXPECTED_TO_PASS,
#else
    .crypt_expected_result  = EXPECTED_TO_FAIL,
#endif
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "ChaCha Poly Decrypt Invalid mac",
    .p_plaintext            = chacha_poly_plain_114,
    .p_ciphertext           = chacha_poly_cipher_114,
    .p_key                  = chacha_poly_key,
    .p_ad                   = chacha_poly_ad,
    .p_nonce                = chacha_poly_nonce,
    .p_mac                  = chacha_poly_invalid_mac
};

/*lint -restore */

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_CHACHA_POLY)
