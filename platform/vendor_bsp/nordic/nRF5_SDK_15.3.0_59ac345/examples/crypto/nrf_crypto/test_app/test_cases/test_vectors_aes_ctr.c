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

/**@brief CTR test vectors can be found NIST Special Publication 800-38A.
 *
 * https://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38a.pdf
 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CTR)

/*lint -save -e91 */

const char ctr_long_plain[]       = { "d8571a7c14be149a0e94fc6c0d8ec2fa0d55510787762e41726d33f96d45f909194fe52571b7dd556a6016f8063cf1bd1601b4cac12814adf097d20c01ebc74e6ff786895ac85aca48cf982eb089eed94d0c3f1f33156a01fa7675154971756fa63493cc0d587ff3d2895c782618a67f8f7003b7c7fee18e609cc159ad99bc70bc16fda7e01f8352d9a628c861cd97b82b7ebd837506a5a14a94e8e7db0589cb5ef10c3808977accc1f261d2e87a5e4556a626a388b83349f375b79a35297c294a0deb0dff4c414235a4c3d799a602eb3633d655725e084421c20e5415a1f11765514d1d8d8800617e3c26cbbe71cc423305f62c4c770bffec44" };
const char ctr_128_long_cipher[]  = { "34dbc50f8cde682afc46ea19e710631e3b7e2d3be0057f226acd442e91158aa77363265d093eea1ad4d4dee311869df9fe9d8d5531d98c6b249de3d714876cb0dffac1714e42cbc4b8a7a8b920c24bdb15957b457ef46bf4e9bca48d34f89c749ded3fb54486540ab7e0f04065e0d43df2eacd37803db28775c679f15d1c2bc10b8a4481a0f1cbc54c091edd4b7e6183513cd2f5f8bf4049562120a9ac4844e6f35141df20baf912999cb557e2e9d1501b8497425a091bdd09d4483ce1a51c1a4fd775f236ae3fa543535e012fa89a1aa81218cf1d3b23572309afa230d8e6e1814719fb76c82ed66c4dec3c5568d2911e9b8862c8a679b884bc" };
const char ctr_128_key[]          = { "2b7e151628aed2a6abf7158809cf4f3c" };
const char ctr_counter_1[]        = { "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff" };
const char ctr_counter_2[]        = { "f0f1f2f3f4f5f6f7f8f9fafbfcfdff00" };
const char ctr_counter_3[]        = { "f0f1f2f3f4f5f6f7f8f9fafbfcfdff01" };
const char ctr_counter_4[]        = { "f0f1f2f3f4f5f6f7f8f9fafbfcfdff02" };
const char ctr_counter_5[]        = { "f0f1f2f3f4f5f6f7f8f9fafbfcfdff03" };
const char ctr_plain_1[]          = { "6bc1bee22e409f96e93d7e117393172a" };
const char ctr_plain_2[]          = { "ae2d8a571e03ac9c9eb76fac45af8e51" };
const char ctr_plain_3[]          = { "30c81c46a35ce411e5fbc1191a0a52ef" };
const char ctr_plain_4[]          = { "f69f2445df4f9b17ad2b417be66c3710" };
const char ctr_128_cipher_1[]     = { "874d6191b620e3261bef6864990db6ce" };
const char ctr_128_cipher_2[]     = { "9806f66b7970fdff8617187bb9fffdff" };
const char ctr_128_cipher_3[]     = { "5ae4df3edbd5d35e5b4f09020db03eab" };
const char ctr_128_cipher_4[]     = { "1e031dda2fbe03d1792170a0f3009cee" };

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CTR_128)

// AES CTR - Functional test using test vector NIST SP 800-38A CTR-AES128.Encrypt - Block 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_func_data, test_vector_aes_t test_vector_aes_ctr_128_functional) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "CTR 128 Functional",
    .p_plaintext        = ctr_plain_1,
    .p_ciphertext       = ctr_128_cipher_1,
    .p_key              = ctr_128_key,
    .p_iv               = ctr_counter_1
};

// AES CTR - Custom test vector - long
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_128_encrypt_long) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 128 Encrypt message_len=250",
    .p_plaintext        = ctr_long_plain,
    .p_ciphertext       = ctr_128_long_cipher,
    .p_key              = ctr_128_key,
    .p_iv               = ctr_counter_1
};

// AES CTR - Custom test vector - short
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_128_encrypt_short) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 128 Encrypt message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = ctr_128_key,
    .p_iv               = ctr_counter_1
};

// AES CTR - NIST SP 800-38A CTR-AES128.Encrypt - Block 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_128_encrypt_1) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 128 Encrypt message_len=16 valid 1",
    .p_plaintext        = ctr_plain_1,
    .p_ciphertext       = ctr_128_cipher_1,
    .p_key              = ctr_128_key,
    .p_iv               = ctr_counter_1,
    .p_ad               = ctr_counter_2
};

// AES CTR - NIST SP 800-38A CTR-AES128.Encrypt - Block 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_128_encrypt_2) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 128 Encrypt message_len=16 valid 2",
    .p_plaintext        = ctr_plain_2,
    .p_ciphertext       = ctr_128_cipher_2,
    .p_key              = ctr_128_key,
    .p_iv               = ctr_counter_2,
    .p_ad               = ctr_counter_3
};

// AES CTR - NIST SP 800-38A CTR-AES128.Encrypt - Block 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_128_encrypt_3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 128 Encrypt message_len=16 valid 3",
    .p_plaintext        = ctr_plain_3,
    .p_ciphertext       = ctr_128_cipher_3,
    .p_key              = ctr_128_key,
    .p_iv               = ctr_counter_3,
    .p_ad               = ctr_counter_4
};

// AES CTR - NIST SP 800-38A CTR-AES128.Encrypt - Block 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_128_encrypt_4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 128 Encrypt message_len=16 valid 4",
    .p_plaintext        = ctr_plain_4,
    .p_ciphertext       = ctr_128_cipher_4,
    .p_key              = ctr_128_key,
    .p_iv               = ctr_counter_4,
    .p_ad               = ctr_counter_5
};

// AES CTR - NIST SP 800-38A CTR-AES128.Encrypt - Counter Oveflow
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_128_encrypt_5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 128 Encrypt Counter Overflow",
    .p_plaintext        = "563d9cd1fcf40d7335cfe7932ae6183c",
    .p_ciphertext       = "cfcb732b9d80dc141cb6a163516e0882",
    .p_key              = "7d5b752df6956655c091c69fb2684483",
    .p_iv               = "ffffffffffffffffffffffffffffffff",
    .p_ad               = "00000000000000000000000000000000"
};

// AES CTR - Custom test vector - long
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_128_decrypt_long) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 128 Decrypt message_len=250",
    .p_plaintext        = ctr_long_plain,
    .p_ciphertext       = ctr_128_long_cipher,
    .p_key              = ctr_128_key,
    .p_iv               = ctr_counter_1
};

// AES CTR - Custom test vector - short
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_128_decrypt_short) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 128 Decrypt message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = ctr_128_key,
    .p_iv               = ctr_counter_1
};

// AES CTR - NIST SP 800-38A CTR-AES128.Decrypt - Block 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_128_decrypt_1) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 128 Decrypt message_len=16 valid 1",
    .p_plaintext        = ctr_plain_1,
    .p_ciphertext       = ctr_128_cipher_1,
    .p_key              = ctr_128_key,
    .p_iv               = ctr_counter_1,
    .p_ad               = ctr_counter_2
};

// AES CTR - NIST SP 800-38A CTR-AES128.Decrypt - Block 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_128_decrypt_2) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 128 Decrypt message_len=16 valid 2",
    .p_plaintext        = ctr_plain_2,
    .p_ciphertext       = ctr_128_cipher_2,
    .p_key              = ctr_128_key,
    .p_iv               = ctr_counter_2,
    .p_ad               = ctr_counter_3
};

// AES CTR - NIST SP 800-38A CTR-AES128.Decrypt - Block 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_128_decrypt_3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 128 Decrypt message_len=16 valid 3",
    .p_plaintext        = ctr_plain_3,
    .p_ciphertext       = ctr_128_cipher_3,
    .p_key              = ctr_128_key,
    .p_iv               = ctr_counter_3,
    .p_ad               = ctr_counter_4
};

// AES CTR - NIST SP 800-38A CTR-AES128.Decrypt - Block 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_128_decrypt_4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 128 Decrypt message_len=16 valid 4",
    .p_plaintext        = ctr_plain_4,
    .p_ciphertext       = ctr_128_cipher_4,
    .p_key              = ctr_128_key,
    .p_iv               = ctr_counter_4,
    .p_ad               = ctr_counter_5
};

// AES CTR - NIST SP 800-38A CTR-AES128.Decrypt - Counter Oveflow
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_128_decrypt_5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 128 Decrypt Counter Overflow",
    .p_plaintext        = "563d9cd1fcf40d7335cfe7932ae6183c",
    .p_ciphertext       = "cfcb732b9d80dc141cb6a163516e0882",
    .p_key              = "7d5b752df6956655c091c69fb2684483",
    .p_iv               = "ffffffffffffffffffffffffffffffff",
    .p_ad               = "00000000000000000000000000000000"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CTR_128)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CTR_192)

const char ctr_192_key[]           = { "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b" };
const char ctr_192_long_cipher[]   = { "a92a37ba2dac97aea88286248063bbdbaa7be2bc93d3783239169e93dc24f9cc37b14b080300ff345e26ca874a1608a5afe63779ccff86e2072b499bbf41400e27e5cabe58fca4ffe09e784cc5cfca70930f46ad9227b517d872291b05d8456085d360f1a27ea78c5efd48aaa80d6b9a17f07eb814082fa6746b48e8b308e90345927f4c587d9c6b18c2e166efbfc538e0e02d0162a42ac696a013290757f80e8bc04135efe6c2f3166259e1878d091d7c00e30f18ab9c20b2ff62f7e0d8a7ab8120ef931e4a272f4f3760c0def8b3059834767b3476cd538bfc25709e936ced8825d02cf2283db2807f1fb903f2c0fef64d6f478e67d68af46a" };
const char ctr_192_cipher_1[]      = { "1abc932417521ca24f2b0459fe7e6e0b" };
const char ctr_192_cipher_2[]      = { "090339ec0aa6faefd5ccc2c6f4ce8e94" };
const char ctr_192_cipher_3[]      = { "1e36b26bd1ebc670d1bd1d665620abf7" };
const char ctr_192_cipher_4[]      = { "4f78a7f6d29809585a97daec58c6b050" };

// AES CTR - Functional test using test vector NIST SP 800-38A CTR-AES192.Encrypt - Block 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_func_data, test_vector_aes_t test_vector_aes_ctr_192_functional) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "CTR 192 Functional",
    .p_plaintext        = ctr_plain_1,
    .p_ciphertext       = ctr_192_cipher_1,
    .p_key              = ctr_192_key,
    .p_iv               = ctr_counter_1
};

// AES CTR - Custom test vector - long
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_192_encrypt_long) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 192 Encrypt message_len=250",
    .p_plaintext        = ctr_long_plain,
    .p_ciphertext       = ctr_192_long_cipher,
    .p_key              = ctr_192_key,
    .p_iv               = ctr_counter_1
};

// AES CTR - Custom test vector - short
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_192_encrypt_short) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 192 Encrypt message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = ctr_192_key,
    .p_iv               = ctr_counter_1
};

// AES CTR - NIST SP 800-38A CTR-AES192.Encrypt - Block 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_192_encrypt_1) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 192 Encrypt message_len=16 valid 1",
    .p_plaintext        = ctr_plain_1,
    .p_ciphertext       = ctr_192_cipher_1,
    .p_key              = ctr_192_key,
    .p_iv               = ctr_counter_1,
    .p_ad               = ctr_counter_2
};

// AES CTR - NIST SP 800-38A CTR-AES192.Encrypt - Block 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_192_encrypt_2) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 192 Encrypt message_len=16 valid 2",
    .p_plaintext        = ctr_plain_2,
    .p_ciphertext       = ctr_192_cipher_2,
    .p_key              = ctr_192_key,
    .p_iv               = ctr_counter_2,
    .p_ad               = ctr_counter_3
};

// AES CTR - NIST SP 800-38A CTR-AES192.Encrypt - Block 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_192_encrypt_3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 192 Encrypt message_len=16 valid 3",
    .p_plaintext        = ctr_plain_3,
    .p_ciphertext       = ctr_192_cipher_3,
    .p_key              = ctr_192_key,
    .p_iv               = ctr_counter_3,
    .p_ad               = ctr_counter_4
};

// AES CTR - NIST SP 800-38A CTR-AES192.Encrypt - Block 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_192_encrypt_4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 192 Encrypt message_len=16 valid 4",
    .p_plaintext        = ctr_plain_4,
    .p_ciphertext       = ctr_192_cipher_4,
    .p_key              = ctr_192_key,
    .p_iv               = ctr_counter_4,
    .p_ad               = ctr_counter_5
};

// AES CTR - NIST SP 800-38A CTR-AES192.Encrypt - Counter Oveflow
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_192_encrypt_5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 192 Encrypt Counter Overflow",
    .p_plaintext        = "563d9cd1fcf40d7335cfe7932ae6183c",
    .p_ciphertext       = "e11c6b5f89b024821b042e9cf8c0005d",
    .p_key              = "24ce863c213533bb79f1592f4f350da3c7f70e5f977b315d",
    .p_iv               = "ffffffffffffffffffffffffffffffff",
    .p_ad               = "00000000000000000000000000000000"
};


// AES CTR - Custom test vector - long
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_192_decrypt_long) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 192 Decrypt message_len=250",
    .p_plaintext        = ctr_long_plain,
    .p_ciphertext       = ctr_192_long_cipher,
    .p_key              = ctr_192_key,
    .p_iv               = ctr_counter_1
};

// AES CTR - Custom test vector - short
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_192_decrypt_short) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 192 Decrypt message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = ctr_192_key,
    .p_iv               = ctr_counter_1
};


// AES CTR - NIST SP 800-38A CTR-AES192.Decrypt - Block 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_192_decrypt_1) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 192 Decrypt message_len=16 valid 1",
    .p_plaintext        = ctr_plain_1,
    .p_ciphertext       = ctr_192_cipher_1,
    .p_key              = ctr_192_key,
    .p_iv               = ctr_counter_1,
    .p_ad               = ctr_counter_2
};

// AES CTR - NIST SP 800-38A CTR-AES192.Decrypt - Block 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_192_decrypt_2) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 192 Decrypt message_len=16 valid 2",
    .p_plaintext        = ctr_plain_2,
    .p_ciphertext       = ctr_192_cipher_2,
    .p_key              = ctr_192_key,
    .p_iv               = ctr_counter_2,
    .p_ad               = ctr_counter_3
};

// AES CTR - NIST SP 800-38A CTR-AES192.Decrypt - Block 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_192_decrypt_3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 192 Decrypt message_len=16 valid 3",
    .p_plaintext        = ctr_plain_3,
    .p_ciphertext       = ctr_192_cipher_3,
    .p_key              = ctr_192_key,
    .p_iv               = ctr_counter_3,
    .p_ad               = ctr_counter_4
};

// AES CTR - NIST SP 800-38A CTR-AES192.Decrypt - Block 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_192_decrypt_4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 192 Decrypt message_len=16 valid 4",
    .p_plaintext        = ctr_plain_4,
    .p_ciphertext       = ctr_192_cipher_4,
    .p_key              = ctr_192_key,
    .p_iv               = ctr_counter_4,
    .p_ad               = ctr_counter_5
};

// AES CTR - NIST SP 800-38A CTR-AES192.Decrypt - Counter Oveflow
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_192_decrypt_5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 192 Decrypt Counter Overflow",
    .p_plaintext        = "563d9cd1fcf40d7335cfe7932ae6183c",
    .p_ciphertext       = "e11c6b5f89b024821b042e9cf8c0005d",
    .p_key              = "24ce863c213533bb79f1592f4f350da3c7f70e5f977b315d",
    .p_iv               = "ffffffffffffffffffffffffffffffff",
    .p_ad               = "00000000000000000000000000000000"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CTR_192)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CTR_256)

const char ctr_256_key[]           = { "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4" };
const char ctr_256_long_cipher[]   = { "d388678d4da902a9500e7779c5ee07f8573b389ad4173747265eb5c5e220829d028ec9b970d6d00867ebc05b31b23bdf3f575502a81d221c4e7e4e7fa2feb1f8e480796983b453589f38963259462d6e01e7589d5313628296d4be506cebf0c2bffac0303d4dfe8107de4038c859ff17bb1f2abe7933808d04bb1dc2864ed19662ab6edaaa716e5b0fe9dbe14a0c19c8b1b2942b24e5d4ac817b1592c9eb97fc93fd96e8ce43aae1540589d7fcaa9139fb8043ca6df80cd8a68978a9ecfdc47eb7fdab2e9f0bfe75b9113f56854edf14fcfc56daa86fb60046058ecc089a5042724f176b2806ce3abc7952ae2de0e3742eff568a90ee491d8203" };
const char ctr_256_cipher_1[]      = { "601ec313775789a5b7a7f504bbf3d228" };
const char ctr_256_cipher_2[]      = { "f443e3ca4d62b59aca84e990cacaf5c5" };
const char ctr_256_cipher_3[]      = { "2b0930daa23de94ce87017ba2d84988d" };
const char ctr_256_cipher_4[]      = { "dfc9c58db67aada613c2dd08457941a6" };

// AES CTR - Functional test using test vector NIST SP 800-38A CTR-AES256.Encrypt - Block 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_func_data, test_vector_aes_t test_vector_aes_ctr_256_functional) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "CTR 256 Functional",
    .p_plaintext        = ctr_plain_1,
    .p_ciphertext       = ctr_256_cipher_1,
    .p_key              = ctr_256_key,
    .p_iv               = ctr_counter_1
};


// AES CTR - Custom test vector - long
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_256_encrypt_long) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 256 Encrypt message_len=250",
    .p_plaintext        = ctr_long_plain,
    .p_ciphertext       = ctr_256_long_cipher,
    .p_key              = ctr_256_key,
    .p_iv               = ctr_counter_1
};

// AES CTR - Custom test vector - short
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_256_encrypt_short) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 256 Encrypt message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = ctr_256_key,
    .p_iv               = ctr_counter_1
};


// AES CTR - NIST SP 800-38A CTR-AES256.Encrypt - Block 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_256_encrypt_1) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 256 Encrypt message_len=16 valid 1",
    .p_plaintext        = ctr_plain_1,
    .p_ciphertext       = ctr_256_cipher_1,
    .p_key              = ctr_256_key,
    .p_iv               = ctr_counter_1,
    .p_ad               = ctr_counter_2
};

// AES CTR - NIST SP 800-38A CTR-AES256.Encrypt - Block 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_256_encrypt_2) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 256 Encrypt message_len=16 valid 2",
    .p_plaintext        = ctr_plain_2,
    .p_ciphertext       = ctr_256_cipher_2,
    .p_key              = ctr_256_key,
    .p_iv               = ctr_counter_2,
    .p_ad               = ctr_counter_3
};

// AES CTR - NIST SP 800-38A CTR-AES256.Encrypt - Block 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_256_encrypt_3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 256 Encrypt message_len=16 valid 3",
    .p_plaintext        = ctr_plain_3,
    .p_ciphertext       = ctr_256_cipher_3,
    .p_key              = ctr_256_key,
    .p_iv               = ctr_counter_3,
    .p_ad               = ctr_counter_4
};

// AES CTR - NIST SP 800-38A CTR-AES256.Encrypt - Block 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_256_encrypt_4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 256 Encrypt message_len=16 valid 4",
    .p_plaintext        = ctr_plain_4,
    .p_ciphertext       = ctr_256_cipher_4,
    .p_key              = ctr_256_key,
    .p_iv               = ctr_counter_4,
    .p_ad               = ctr_counter_5
};

// AES CTR - NIST SP 800-38A CTR-AES256.Encrypt - Counter Oveflow
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_256_encrypt_5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CTR 256 Encrypt Counter Overflow",
    .p_plaintext        = "2bdb7c499b55bfe636860c320d2eee1b",
    .p_ciphertext       = "02a1ef073c9c0ba74f195564455398c5",
    .p_key              = "47778b2c9558790eb5def620fafcb8f5a447e3bfe34eb8d6c0911b27f0f95e85",
    .p_iv               = "ffffffffffffffffffffffffffffffff",
    .p_ad               = "00000000000000000000000000000000"
};

// AES CTR - Custom test vector - long
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_256_decrypt_long) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 256 Decrypt message_len=250",
    .p_plaintext        = ctr_long_plain,
    .p_ciphertext       = ctr_256_long_cipher,
    .p_key              = ctr_256_key,
    .p_iv               = ctr_counter_1
};

// AES CTR - Custom test vector - short
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_256_decrypt_short) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 256 Decrypt message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = ctr_256_key,
    .p_iv               = ctr_counter_1
};


// AES CTR - NIST SP 800-38A CTR-AES256.Decrypt - Block 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_256_decrypt_1) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 256 Decrypt message_len=16 valid 1",
    .p_plaintext        = ctr_plain_1,
    .p_ciphertext       = ctr_256_cipher_1,
    .p_key              = ctr_256_key,
    .p_iv               = ctr_counter_1,
    .p_ad               = ctr_counter_2
};

// AES CTR - NIST SP 800-38A CTR-AES256.Decrypt - Block 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_256_decrypt_2) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 256 Decrypt message_len=16 valid 2",
    .p_plaintext        = ctr_plain_2,
    .p_ciphertext       = ctr_256_cipher_2,
    .p_key              = ctr_256_key,
    .p_iv               = ctr_counter_2,
    .p_ad               = ctr_counter_3
};

// AES CTR - NIST SP 800-38A CTR-AES256.Decrypt - Block 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_256_decrypt_3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 256 Decrypt message_len=16 valid 3",
    .p_plaintext        = ctr_plain_3,
    .p_ciphertext       = ctr_256_cipher_3,
    .p_key              = ctr_256_key,
    .p_iv               = ctr_counter_3,
    .p_ad               = ctr_counter_4
};

// AES CTR - NIST SP 800-38A CTR-AES256.Decrypt - Block 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_256_decrypt_4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 256 Decrypt message_len=16 valid 4",
    .p_plaintext        = ctr_plain_4,
    .p_ciphertext       = ctr_256_cipher_4,
    .p_key              = ctr_256_key,
    .p_iv               = ctr_counter_4,
    .p_ad               = ctr_counter_5
};

// AES CTR - NIST SP 800-38A CTR-AES256.Decrypt - Counter Oveflow
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ctr_256_decrypt_5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ctr_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CTR 256 Decrypt Counter Overflow",
    .p_plaintext        = "2bdb7c499b55bfe636860c320d2eee1b",
    .p_ciphertext       = "02a1ef073c9c0ba74f195564455398c5",
    .p_key              = "47778b2c9558790eb5def620fafcb8f5a447e3bfe34eb8d6c0911b27f0f95e85",
    .p_iv               = "ffffffffffffffffffffffffffffffff",
    .p_ad               = "00000000000000000000000000000000"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CTR_256)

/*lint -restore */

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CTR)
