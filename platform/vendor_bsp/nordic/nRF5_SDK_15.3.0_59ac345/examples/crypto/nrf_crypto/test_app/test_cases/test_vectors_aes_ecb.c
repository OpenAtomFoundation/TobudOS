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
#include "common_test.h"

/**@brief ECB test vectors can be found on NIST web pages.
 *
 * https://csrc.nist.gov/projects/cryptographic-algorithm-validation-program/block-ciphers#AES
 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_ECB)

/*lint -save -e91 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_ECB_128)

// AES ECB - Functional test using test vector NIST CAVS 11.1 ECB KeySbox 128 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_func_data, test_vector_aes_t test_vector_aes_ecb_128_functional) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Functional",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "6d251e6944b051e04eaa6fb4dbf78465",
    .p_key              = "10a58869d74be5a374cf867cfb473859"
};

// AES ECB - Custom test vector - Encrypt - Message length 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_c0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = "89df4c819f49dbcbcb124304023cf38c"
};

// AES ECB - Custom test vector - Decrypt - Message length 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_c0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = "89df4c819f49dbcbcb124304023cf38c"
};

// AES ECB - Custom test vector - Encrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_c3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt message_len=1 pkcs7-padding",
    .p_plaintext        = "de",
    .p_ciphertext       = "77914f065bdaccd3ca428385e4f20643",
    .p_key              = "89df4c819f49dbcbcb124304023cf38c"
};

// AES ECB - Custom test vector - Decrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_c3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt message_len=1 pkcs7-padding",
    .p_plaintext        = "de0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f",                                       /**< The 15 padded bytes (0x0f) will be removed in the actual generated plaintext. */
    .p_ciphertext       = "77914f065bdaccd3ca428385e4f20643",
    .p_key              = "89df4c819f49dbcbcb124304023cf38c"
};

// AES ECB - Custom test vector - Encrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_c4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt message_len=16 pkcs7-padding",
    .p_plaintext        = "dededededededededededededededede",
    .p_ciphertext       = "012e6cf02e5746a4ae7ad6287e244973a87e6dbf88ffd1413b95b96491c8872a",
    .p_key              = "89df4c819f49dbcbcb124304023cf38c"
};

// AES ECB - Custom test vector - Decrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_c4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt message_len=16 pkcs7-padding",
    .p_plaintext        = "dededededededededededededededede10101010101010101010101010101010",       /**< The 16 padded bytes (0x10) will be removed in the actual generated plaintext. */
    .p_ciphertext       = "012e6cf02e5746a4ae7ad6287e244973a87e6dbf88ffd1413b95b96491c8872a",
    .p_key              = "89df4c819f49dbcbcb124304023cf38c"
};

// AES ECB - Custom test vector - Encrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_c5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt message_len=31 pkcs7-padding",
    .p_plaintext        = "dedededededededededededededededededededededededededededededede",
    .p_ciphertext       = "012e6cf02e5746a4ae7ad6287e244973e0dcca6023766f779fd984c5c321c7ee",
    .p_key              = "89df4c819f49dbcbcb124304023cf38c"
};

// AES ECB - Custom test vector - Decrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_c5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt message_len=31 pkcs7-padding",
    .p_plaintext        = "dedededededededededededededededededededededededededededededede01",       /**< The padded byte (0x01) will be removed in the actual generated plaintext. */
    .p_ciphertext       = "012e6cf02e5746a4ae7ad6287e244973e0dcca6023766f779fd984c5c321c7ee",
    .p_key              = "89df4c819f49dbcbcb124304023cf38c"
};

// AES ECB - NIST CAVS 11.1 GFSBox Tests 128 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_gfsb0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt GFSBox 0",
    .p_plaintext        = "f34481ec3cc627bacd5dc3fb08f273e6",
    .p_ciphertext       = "0336763e966d92595a567cc9ce537f5e",
    .p_key              = "00000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 GFSBox Tests 128 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_gfsb0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt GFSBox 0",
    .p_plaintext        = "f34481ec3cc627bacd5dc3fb08f273e6",
    .p_ciphertext       = "0336763e966d92595a567cc9ce537f5e",
    .p_key              = "00000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 VarKey Tests 128 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_vk0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt VarKey 0",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "0edd33d3c621e546455bd8ba1418bec8",
    .p_key              = "80000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 VarKey Tests 128 Encrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_vk127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt VarKey 127",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "a1f6258c877d5fcd8964484538bfc92c",
    .p_key              = "ffffffffffffffffffffffffffffffff"
};

// AES ECB - NIST CAVS 11.1 VarKey Tests 128 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_vk0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt VarKey 0",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "0edd33d3c621e546455bd8ba1418bec8",
    .p_key              = "80000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 VarKey Tests 128 Decrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_vk127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt VarKey 127",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "a1f6258c877d5fcd8964484538bfc92c",
    .p_key              = "ffffffffffffffffffffffffffffffff"
};

// AES ECB - NIST CAVS 11.1 VarTxt Tests 128 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_vt0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt VarTxt 0",
    .p_plaintext        = "80000000000000000000000000000000",
    .p_ciphertext       = "3ad78e726c1ec02b7ebfe92b23d9ec34",
    .p_key              = "00000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 VarTxt Tests 128 Encrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_vt127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt VarTxt 127",
    .p_plaintext        = "ffffffffffffffffffffffffffffffff",
    .p_ciphertext       = "3f5b8cc9ea855a0afa7347d23e8d664e",
    .p_key              = "00000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 VarTxt Tests 128 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_vt0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt VarTxt 0",
    .p_plaintext        = "80000000000000000000000000000000",
    .p_ciphertext       = "3ad78e726c1ec02b7ebfe92b23d9ec34",
    .p_key              = "00000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 VarTxt Tests 128 Decrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_vt127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt VarTxt 127",
    .p_plaintext        = "ffffffffffffffffffffffffffffffff",
    .p_ciphertext       = "3f5b8cc9ea855a0afa7347d23e8d664e",
    .p_key              = "00000000000000000000000000000000"
};


// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_16) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt message_len=16",
    .p_plaintext        = "1695fe475421cace3557daca01f445ff",
    .p_ciphertext       = "7888beae6e7a426332a7eaa2f808e637",
    .p_key              = "edfdb257cb37cdf182c5455b0c0efebb"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_32) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt message_len=32",
    .p_plaintext        = "1b0a69b7bc534c16cecffae02cc5323190ceb413f1db3e9f0f79ba654c54b60e",
    .p_ciphertext       = "ad5b089515e7821087c61652dc477ab1f2cc6331a70dfc59c9ffb0c723c682f6",
    .p_key              = "7723d87d773a8bbfe1ae5b081235b566"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_48) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt message_len=48",
    .p_plaintext        = "6f172bb6ec364833411841a8f9ea2051735d600538a9ea5e8cd2431a432903c1d6178988b616ed76e00036c5b28ccd8b",
    .p_ciphertext       = "4cc2a8f13c8c7c36ed6a814db7f26900c7e04df49cbad916ce6a44d0ae4fe7edc0b402794675b3694933ebbc356525d8",
    .p_key              = "280afe063216a10b9cad9b2095552b16"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_64) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt message_len=64",
    .p_plaintext        = "593559318cc66bf695e49feb42794bdfb66bce895ec222ca2609b133ecf66ac7344d13021e01e11a969c4684cbe20abae2b19d3ceb2cacd41419f21f1c865149",
    .p_ciphertext       = "3ea6f4305217bd47eebe773da4b578549cac744c00cbd8f9d596d38010304bd850cc2f4b19a91c2e022eabf100266185ca2705127815dfd46efbe4ecd46a3058",
    .p_key              = "ef60fb1400c83936414a25651eb51a1b"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_80) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt message_len=80",
    .p_plaintext        = "84f809fc5c84652376cc0df10095bc00b9f0547fa91a2d3310a0adbc9cc6191ade2aaa6fffa5e406af7223955f9277bfb06eb1dd2bbfbefe32ab342c36302bf22bc64e1b394032bbb5f4e6744f1bcbf2",
    .p_ciphertext       = "a6dc096bc21b0658e416a0f679fefc6e958e9c56e3ce04fdf6e392c2db770a60d9523c255925e14a3e02a1002bf3875c2e501bac618bee1f55f9850454854eef9d693d90937cc8387b6f4c4414e2080b",
    .p_key              = "c5805cd1c4a7b98a715badb709720bf4"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_96) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt message_len=96",
    .p_plaintext        = "7adcf4a494f6b09790c82c8bb97db62c5d3fa4032f06dfeceaad9ecb374b747bd1c08d07e78e351dc2eb99bfa714d23cffe31f5fb5a472e6e0252f35a20c304c4f6d0cf7d29c99444d40af3a00a92fc86c6444fcb80ce9765362ac1bdba0b10e",
    .p_ciphertext       = "22217953f71932ab4360d97ef495081559f1fcb09caca41fa0c65f7b1792b560eabe18f3b3b06ef80c41886f24c5d6d32d20427e83d8b5564d9ac7435a2842c1cf7c6fcc229eb7f518d3e0167d510efbaee39a0438fc800eb6acfc203c93280c",
    .p_key              = "4c35be028e1475278346eae531cbee5c"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_112) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt message_len=112",
    .p_plaintext        = "37a1205ea929355d2e4ee52d5e1d9cda279ae01e640287ccb153276e7e0ecf2d633cf4f2b3afaecb548a2590ce0445c6a168bac3dc601813eb74591bb1ce8dfcd740cdbb6388719e8cd283d9cc7e736938240b410dd5a6a48ba49dd2066503e63ab592ffdf3be49e7d2de74f82158b8c",
    .p_ciphertext       = "c88e03383ba9da6f982c057fe92c0bb3ed5b9cd18295a100e13a4e12d440b919bbb8b221abead362902ce44d30d0b80e56bee1f66a7d8de0b1e1b4dbf76c90c1807a3bc5f277e9814c82ab120f7e10217dfdf6092ce4958f8906c5e32279c6537dd1fbae20cb7a1d9f89d0490b6aefc1",
    .p_key              = "00cc73c990d376b82246e45ea3ae2e37"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_128) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt message_len=128",
    .p_plaintext        = "eaf1760c0f25310dada6debeb966304db7a9f1b2d1c3af922623b263649031d299b3c56146d61d55b6ebf4cf8dd04039a4d1ace3146f49ee915f806afad64cbb2d04a64120de40382e2175dcae9480d1ca8dedc38fb64e4a40112f10f03a4c354fed01f2c5c7017dbd514b2d443a5adfd2e49c986723266cda41a69e6e459908",
    .p_ciphertext       = "5ece70a44da41bc7cfb9b582ea9ce0980030ec4af331e76499961f88860aa0554aba3ecbf77ca4293a3fee854a2caf3ae800343fb4521388b16b6dc599b3d60bf82777f98e1a8d04ab9cd54dd9a248095795d4dfe4858bfd9a05f54c795bb086e15f7c22228184ec66a9ca10b1cf71a6bb9303c5cd1dcc056460a86df651f053",
    .p_key              = "0a53aa7a3e4a4f364e8c6c7224af5501"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_144) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt message_len=144",
    .p_plaintext        = "8177d79c8f239178186b4dc5f1df2ea7fee7d0db535489ef983aefb3b2029aeba0bb2b46a2b18c94a1417a33cbeb41ca7ea9c73a677fccd2eb5470c3c500f6d3f1a6c755c944ba586f88921f6ae6c9d194e78c7233c406126633e144c3810ad23ee1b5af4c04a22d49e99e7017f74c2309492569ff49be17d2804920f2ac5f514d13fd3e7318cc7cf80ca5101a465428",
    .p_ciphertext       = "5befb3062a7a7246af1f77b0ec0ac614e28be06ac2c81b19e5a0481bf160f9f2bc43f28f6548787639e4ce3e0f1e95475f0e81ceb793004c8e46670ebd48b866d5b43d104874ead4be8a236bf90b48f862f7e252dec4475fdbb841a662efcd25ed64b2910e9baaea9466e413a4241438b31df0bd3df9a16f4641636754e259861728aa7ddf435cc51f54f79a1db25f52",
    .p_key              = "b80bcc929052cb5450479442e2b809ce"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_160) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 128 Encrypt message_len=160",
    .p_plaintext        = "451f45663b44fd005f3c288ae57b383883f02d9ad3dc1715f9e3d6948564257b9b06d7dd51935fee580a96bbdfefb918b4e6b1daac809847465578cb8b5356ed38556f801ff7c11ecba9cdd263039c15d05900fc228e1caf302d261d7fb56cee663595b96f192a78ff4455393a5fe8162170a066fdaeac35019469f22b3470686bced2f007a1a2e43e01b4562caaa502ed541b8205874ec1ffb1c8b255766942",
    .p_ciphertext       = "01043053f832ef9b911ed387ba577451e30d51d4b6b11f319d4cd539d067b7f4f9b4f41f7f3d4e920c57cbe2b5e1885aa66203ae493e93a1df63793a9563c176bc6775dd09cc9161e278a01beb8fd8a19200326bd95abc5f716768e34f90b50523d30fdabb103a3bc020afbbb0cb3bd2ad512a6fea79f8d64cef347458dec48be89451cb0b807d73593f273d9fc521b789a77524404f43e00f20b3b77b938b1a",
    .p_key              = "ebea9c6a82213a00ac1d22faea22116f"
};


// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_16) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt message_len=16",
    .p_plaintext        = "46f2c98932349c338e9d67f744a1c988",
    .p_ciphertext       = "065bd5a9540d22d5d7b0f75d66cb8b30",
    .p_key              = "54b760dd2968f079ac1d5dd20626445d"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_32) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt message_len=32",
    .p_plaintext        = "8c8e6ed8a6bdb3baabec78189b7d61010d6d168de2c08ec56f9b9c004a191e21",
    .p_ciphertext       = "76fa67e121894dbc2ad85ae7f9c5c44d14ccee5b87d5e7708950338f5c84cf58",
    .p_key              = "bce4c486cc4a12622b9147df9bca452c"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_48) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt message_len=48",
    .p_plaintext        = "8838058bd98072b72fd75641edb216301fee8323ff8c4ba18ee6a4623c2fdeb3df5f8a240a8e7e6d5ae15444614f41fb",
    .p_ciphertext       = "1ce69ab7624fed511fb42b8fb639f566d72783f9ab49801651961b048e0f5b7cc0ebb2f514f2d625f927ccc4d5a94593",
    .p_key              = "279161759d29e92dc8f1fafd8d45533d"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_64) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt message_len=64",
    .p_plaintext        = "6061df5bcd421fabdb5235fc032502650448fd8233a0237c5f6f249a63d7db3e4283ac9a8684a363ef64e760c5886564659da619466851568b32091df5516f57",
    .p_ciphertext       = "6c53489f6a4bd0f7cd15d20f6ebc7e649fd95b76b107e6daba967c8a9484797f29a8a282ee31c03fae4f8e9b8930d5473c2ed695a347e88b7ccd62376d5ebb41",
    .p_key              = "9ba852520c9fd1eb367b6ad2aed07abd"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_80) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt message_len=80",
    .p_plaintext        = "3f098ae7e7524ddb556403c5d3d0b10e5db6c324843ca54a47e34ad9bdbccc93a645e96f2e98651751f0f1a697398ea801a69079c1808b6a8e3bb02a4cf3292f1a7a05b954571339f7899f9aaa2ae1e9",
    .p_ciphertext       = "33cb1a98ce4be97f5b222aa861c945643f372687cbf7f8193a7d304e40baceb877eedfbc05be47437a86bdb1f7f0171b95c6c8b50404a7897332f26fb0f0923991d3a02e926f29babfb52e63d580efff",
    .p_key              = "cd6a33abb95d7ab92980f9c2b810c90f"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_96) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt message_len=96",
    .p_plaintext        = "3dfe918048f4c0b0b22fbc55e5af751cf3e6ce348eb9a888795033f6dd216afbea65fc8df41c0ce6c7316a8183d36060bf19709fe2bec6d13bbdf1a434cc5b8f10aadb368c269d2dc4474a1f8112640fe52803b1883cb7756743100a758dd862",
    .p_ciphertext       = "19d76b386a7715dec7aad8bb01d126c7385a4387f087faec67718f78274cf4a2d7ec3f7c56885c930bbfff1676d8dd3b0b4e1cdf418c086de6f50700b527cfcc2a81d6ba95e30a526eb4c3dee82af6ccc8c5ab55781db15c57de90995411bff6",
    .p_key              = "ff71b9f6311a44072ddb1a77488c4870"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_112) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt message_len=112",
    .p_plaintext        = "6e19cec775595c2f768de3bda3682fb4026ed1889fc3de11bc45a2a3e2b22694ddaab3274c81b29204fa034eedac564a7dfe27aabb6ff420f9aa5abadf15c74676efeabd96a7ff2491813ba53cb5ab76eacd37255a28daecbbe580f5eb77f9bc0339e0da59b2b307f52974b6182cb106",
    .p_ciphertext       = "4d6f972946a2e4ca05bfbeecd00535ece6c81ed963c4167063b1c34b6a185d5504646d81a83cd5bce2101120b2f2bc6a2fa856ae6fff4449ab6218189faa13e73038e82c5141c3f6276a8a206b9563ca11fa76092779f4deff2e58659047ed4f1a129092095ab75f1cf4b255c69531f4",
    .p_key              = "8ec6a5a054fea2fc8dafb5939a4bd788"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_128) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt message_len=128",
    .p_plaintext        = "398db04b45a724393e4baec86a220f22358df1bbf0cab278dcfecf7a3f8f6275ef45ed9f039bc1f5030c3a0c245fbb6ea0bff3e2da0859ac498db6624a572023de7c221bf3567f51e98947970dfc581b3ca670ab92953d3bdd97dbf5d6bef8aa2f31500d221198ed9f23b049eaeed67683083034b4a5a82a0e49319e368c6f55",
    .p_ciphertext       = "447557bd1561b09a082cabdbf49b0612b46c9793db3c987aa4a843ef28b39fa8d46799b019c0566765c5c8cb53f87ff51ff0c2e65c45df84e9acbfe02163bdba52d0b847706567320bc6ede07f2eaef86b0f602364f86510eabc58bc9ad1e6f0a6f6df0b83188c01e17744a4e0053a22810e99cf5a1ed3258f203509fdc3c050",
    .p_key              = "b53e54b1b09f78abcd6afac1f7e7cda9"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_144) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt message_len=144",
    .p_plaintext        = "a2e19c08fa204640b1bc73beac58f81a7923f3ef2a0affcf1568c9179d816cf1bd16b79418f0266f0ff904e06045f276df187ed72b8f6641b065b854a7d3f152b8e153cd77ec605e5b75454245745dc0e0dcfc616f5246f868b6ee5a1df56c4906c5c34bc42309d370733e50d9a1addc3f37cbe4b331ac7d3a920c794484a16b05599f46f42046a6dc9063b1d5a8ae1f",
    .p_ciphertext       = "fa2e97bed2f0b76ed2f8c1275f6d50ecfc1131240622ba9da17e5dd0f20a7d678f537ffc33762524b73e17ac3d8dc322affd679768c66ca430e3f6ab6e70b95f99570e87f02b46ff6b543efff16d339fcbead89b59fea65403bc7418737c47a301806be66fd7c307d7825da8e1d6496c3921acb7419010b60350fea069f4cf3bb017bfa11a424344e3109d30778fad90",
    .p_key              = "9e79b681c57d7feb7db7e8a6ad41382c"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_160) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 128 Decrypt message_len=160",
    .p_plaintext        = "9c29eecb2de04254fafb896a994102d1da30ddb49d82728eb23dbd029901e9b75b3d0aee03f7a05f6c852d8fada0b5c28e8c9aed334fad11829df3dfadc5c2e471eb41af9e48a8a465e03d5ebdb0216915081f3b5a0ebb2308dfc2d28e5a8ba3f32adae4c3575921bc657b63d46ba5a618880ee9ad8af3fba5643a5026facd7d667ce599327f936cdda7e1bb742a33a019990b76be648a6ec725daed540ed9e7",
    .p_ciphertext       = "931b2f5f3a5820d53a6beaaa6431083a3488f4eb03b0f5b57ef838e1579623103bd6e6800377538b2e51ef708f3c4956432e8a8ee6a34e190642b26ad8bdae6c2af9a6c7996f3b6004d2671e41f1c9f40ee03d1c4a52b0a0654a331f15f34dce4acb96bd6507815ca4347a3de11a311b7de5351c9787c4538158e28974ffa83d8296dfe9cd09cd87f7bf4f54d97d28d4788799163408323943b3e72f5eab66c1",
    .p_key              = "44f0ee626d0446e0a3924cfb078944bb"
};



// AES ECB Multi - NIST CAVS 11.1 Monte Carlo Encrypt 128
NRF_SECTION_ITEM_REGISTER(test_vector_aes_monte_carlo_data, test_vector_aes_t test_vector_aes_ecb_128_encrypt_monte_carlo) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB Encrypt Monte Carlo 128",
    .p_plaintext        = "b9145a768b7dc489a096b546f43b231f",
    .p_ciphertext       = "fb2649694783b551eacd9d5db6126d47",
    .p_key              = "139a35422f1d61de3c91787fe0507afd"
};

// AES ECB Multi - NIST CAVS 11.1 Monte Carlo Decrypt 128
NRF_SECTION_ITEM_REGISTER(test_vector_aes_monte_carlo_data, test_vector_aes_t test_vector_aes_ecb_128_decrypt_monte_carlo) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB Decrypt Monte Carlo 128",
    .p_plaintext        = "d1d2bfdc58ffcad2341b095bce55221e",
    .p_ciphertext       = "b08a29b11a500ea3aca42c36675b9785",
    .p_key              = "0c60e7bf20ada9baa9e1ddf0d1540726"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_ECB_128)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_ECB_192)

// AES ECB - Functional test using test vector NIST CAVS 11.1 ECB KeySbox 192 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_func_data, test_vector_aes_t test_vector_aes_ecb_192_functional) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Functional",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "0956259c9cd5cfd0181cca53380cde06",
    .p_key              = "e9f065d7c13573587f7875357dfbb16c53489f6a4bd0f7cd"
};

// AES ECB - Custom test vector - Encrypt - Message length 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_c0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = "73befbe26caa000bb57574a7d33ad60019b40c7a3ac6562d"
};

// AES ECB - Custom test vector - Decrypt - Message length 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_c0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = "73befbe26caa000bb57574a7d33ad60019b40c7a3ac6562d"
};

// AES ECB - Custom test vector - Encrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_c3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt message_len=1 pkcs7-padding",
    .p_plaintext        = "de",
    .p_ciphertext       = "e8bcefc38b136209ffe5599f40483121",
    .p_key              = "73befbe26caa000bb57574a7d33ad60019b40c7a3ac6562d"
};

// AES ECB - Custom test vector - Decrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_c3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt message_len=1 pkcs7-padding",
    .p_plaintext        = "de0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f",                                       /**< The 15 padded bytes (0x0f) will be removed in the actual generated plaintext. */
    .p_ciphertext       = "e8bcefc38b136209ffe5599f40483121",
    .p_key              = "73befbe26caa000bb57574a7d33ad60019b40c7a3ac6562d"
};

// AES ECB - Custom test vector - Encrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_c4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt message_len=16 pkcs7-padding",
    .p_plaintext        = "dededededededededededededededede",
    .p_ciphertext       = "6db3bddfafa0ff9de90d9f93e36cca46c1db3328aaf77dd0ba1897508a4bc2ac",
    .p_key              = "73befbe26caa000bb57574a7d33ad60019b40c7a3ac6562d"
};

// AES ECB - Custom test vector - Decrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_c4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt message_len=16 pkcs7-padding",
    .p_plaintext        = "dededededededededededededededede10101010101010101010101010101010",       /**< The 16 padded bytes (0x10) will be removed in the actual generated plaintext. */
    .p_ciphertext       = "6db3bddfafa0ff9de90d9f93e36cca46c1db3328aaf77dd0ba1897508a4bc2ac",
    .p_key              = "73befbe26caa000bb57574a7d33ad60019b40c7a3ac6562d"
};

// AES ECB - Custom test vector - Encrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_c5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt message_len=31 pkcs7-padding",
    .p_plaintext        = "dedededededededededededededededededededededededededededededede",
    .p_ciphertext       = "6db3bddfafa0ff9de90d9f93e36cca467f585ccd3aebaab4fd3f8a1b4a1183f8",
    .p_key              = "73befbe26caa000bb57574a7d33ad60019b40c7a3ac6562d"
};

// AES ECB - Custom test vector - Decrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_c5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt message_len=31 pkcs7-padding",
    .p_plaintext        = "dedededededededededededededededededededededededededededededede01",       /**< The padded byte (0x01) will be removed in the actual generated plaintext. */
    .p_ciphertext       = "6db3bddfafa0ff9de90d9f93e36cca467f585ccd3aebaab4fd3f8a1b4a1183f8",
    .p_key              = "73befbe26caa000bb57574a7d33ad60019b40c7a3ac6562d"
};

// AES ECB - NIST CAVS 11.1 GFSBox Tests 192 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_gfsb0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt GFSBox 0",
    .p_plaintext        = "1b077a6af4b7f98229de786d7516b639",
    .p_ciphertext       = "275cfc0413d8ccb70513c3859b1d0f72",
    .p_key              = "000000000000000000000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 GFSBox Tests 192 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_gfsb0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt GFSBox 0",
    .p_plaintext        = "1b077a6af4b7f98229de786d7516b639",
    .p_ciphertext       = "275cfc0413d8ccb70513c3859b1d0f72",
    .p_key              = "000000000000000000000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 VarKey Tests 192 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_vk0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt VarKey 0",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "de885dc87f5a92594082d02cc1e1b42c",
    .p_key              = "800000000000000000000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 VarKey Tests 192 Encrypt - Count 191
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_vk191) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt VarKey 191",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "dd8a493514231cbf56eccee4c40889fb",
    .p_key              = "ffffffffffffffffffffffffffffffffffffffffffffffff"
};

// AES ECB - NIST CAVS 11.1 VarKey Tests 192 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_vk0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt VarKey 0",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "de885dc87f5a92594082d02cc1e1b42c",
    .p_key              = "800000000000000000000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 VarKey Tests 192 Decrypt - Count 191
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_vk191) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt VarKey 191",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "dd8a493514231cbf56eccee4c40889fb",
    .p_key              = "ffffffffffffffffffffffffffffffffffffffffffffffff"
};

// AES ECB - NIST CAVS 11.1 VarTxt Tests 192 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_vt0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt VarTxt 0",
    .p_plaintext        = "80000000000000000000000000000000",
    .p_ciphertext       = "6cd02513e8d4dc986b4afe087a60bd0c",
    .p_key              = "000000000000000000000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 VarTxt Tests 192 Encrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_vt127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt VarTxt 127",
    .p_plaintext        = "ffffffffffffffffffffffffffffffff",
    .p_ciphertext       = "b13db4da1f718bc6904797c82bcf2d32",
    .p_key              = "000000000000000000000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 VarTxt Tests 192 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_vt0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt VarTxt 0",
    .p_plaintext        = "80000000000000000000000000000000",
    .p_ciphertext       = "6cd02513e8d4dc986b4afe087a60bd0c",
    .p_key              = "000000000000000000000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 VarTxt Tests 192 Decrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_vt127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt VarTxt 127",
    .p_plaintext        = "ffffffffffffffffffffffffffffffff",
    .p_ciphertext       = "b13db4da1f718bc6904797c82bcf2d32",
    .p_key              = "000000000000000000000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_16) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt message_len=16",
    .p_plaintext        = "60bcdb9416bac08d7fd0d780353740a5",
    .p_ciphertext       = "24f40c4eecd9c49825000fcb4972647a",
    .p_key              = "61396c530cc1749a5bab6fbcf906fe672d0c4ab201af4554"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_32) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt message_len=32",
    .p_plaintext        = "64145e61e61cd96f796b187464fabbde6f42e693f501f1d73b3c606f00801506",
    .p_ciphertext       = "502a73e4051cfac8fe6343211a129f5a5f56710c41b32c84da978dda2cec34ad",
    .p_key              = "c9c86a51224e5f1916d3f33a602f697afc852a2c44d30d5f"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_48) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt message_len=48",
    .p_plaintext        = "07d3781a71e87b8cf12cac4616f9c7a819be57a0770a7a66e0e6e469506826897c8530866f2715b8757f0f01389dc301",
    .p_ciphertext       = "0b0455954e477e87d1a338414f8ac4d760efb70e5770e5d496b31152cb07e9316378f7fd38c6903b31d4f1b1a53931af",
    .p_key              = "c4854919c324d381bbef9bea771172f6e4563ffbd1e1e956"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_64) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt message_len=64",
    .p_plaintext        = "096612f4111ebdb9accf5efbb973589e5a2c9103407ed2da29bc7137a6c02de822df5907201d3dcbc49cb9a3955b2b86811d938422a6f5a23f2dffe4960fecab",
    .p_ciphertext       = "d375e1af5fcd03e31d0f735f6bc5d7e773bc34ff3eb706be85842f998e4e361303825b848170608f300ad1ced848b5233477f53710503c55736a730595599964",
    .p_key              = "3deecf7a037ebb2ada805e8059bfaeaebb195cace379fcd2"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_80) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt message_len=80",
    .p_plaintext        = "6f825ad0e2fcabd811db11e2840f0884470b87eaa678e329342bf0d1125569c16dfa924049a51b497d594825ffca4f4062cd02359d0ead2b0e014264bf1bb313e741a06eb4813205470047520133cf91",
    .p_ciphertext       = "e6128a9fb497b2ec0d0e9fe1b24e691ea36c0628eb55046b2f20de9d281dd9dbbbeaa0519a4875d3516b15022330163b2798a9102ee9d141a3416c1a277442d4cf1fec87ccea571a23b91a461991ec0d",
    .p_key              = "0c0333d8fe3e99a1eecc120d6b54bb8faa888d12794de93e"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_96) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt message_len=96",
    .p_plaintext        = "27756c11f57c40169f95f25141fb1a07d9444d864e4c43b9b16f39fbc2fb9529b6e4c9fb37ba7994add9e057bce070fd0d64b8e452e58baa84005ffb6fe7f3a7710f35f87f28624268526fbb5e8ea88db272972adf2227e0ca7b09ad48915754",
    .p_ciphertext       = "7516e53877236e931e7fbae22ed2ca5116b72a700f6e397c2aa47c304b3f5c2a026d89831a5104ae7b73ef48de709c1b9e661ae973dea6bb38217a7650228ccc6b228b33f567121427e9f21cbd3ec6aa35ebba336f51ed8e4bf6e75c5866bc20",
    .p_key              = "b07da99fbe0b193e5336141a9819974b1a9a4102129db861"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_112) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt message_len=112",
    .p_plaintext        = "1e49e9541ed0f97023f8205e71fe75668680f12fb5902d7a11066d108c1adef960321cefaef8f71365b077de66c91e59e6b16c9113eea945fa6bdee3a80f25b38f9422512c97d260e7eb837d3b33247216b0bfac835744f08627884947b98d4573190460e0d410c170524d2437661118",
    .p_ciphertext       = "a47e7d0d8266595792bc980bf3c47822229e4428cc91aa1b15519823a133dd479a4664ec8408b4d9301760454ed2037279d996eb1ccc98df77660aa97422dca1c54d7bfac37223d0caee6a6b34d760e767b9595f7f3bb631d6545a7442d3497a7b85054e8b99d24e65df985e5bccf450",
    .p_key              = "80da652b1844dafe4fd4ca8ccc26b564b263711723b6cd48"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_128) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt message_len=128",
    .p_plaintext        = "823fade2722805e4948db57639cbb67d477d4b01319d927826ec594789933486a20acc0393397a8a8ddd1564432e081e9fbfa78cb4a5c30cf5db7b0fa4e556d68db48f7af3cab02fcb9e62c928428480f7437af0ecd1c864bc1fdf538d0d362baa059d4e7c22cf5ab69846aecf50532ab5d26d35c068ca05e4802c1b2c53fe0c",
    .p_ciphertext       = "52c54ec4345e776b84952f5803ad3a52ba4cfedb32c2dc146ecc8813705f02fd3d4d559027e895e2ba29710396272436c398fa12ba02c85c02947df8035647334bc6c53e1d716f582206c8538f397784c3e42f535b68e8151b3cff71a82d2785b0eb298ff63aebc5d822549f44fbbcf0b69114323c9572d0bf674332fac3b57d",
    .p_key              = "9a35954f400034b29c9f85766a255cf1393dccb8d6c93e65"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_144) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt message_len=144",
    .p_plaintext        = "f4d46d6ac9969cb6994c4f5f97a58634e5927e52bfaa82a2f7cc04a3a106ec6a0fb530e700eea569141d156483ec91730f670899375d19ccae0926b40bf1aab0c20fcc1a532f34ccc1981de246eeabc245577f5a80dd8aa98b141e8181fc4b8bf528688b2b20f0e58805906148737d2b6fab0b1e02f27c417634159269ed7803904a2dc3f18b82be52ab9567f143afb5",
    .p_ciphertext       = "a61376ae4c6f8fef51736087dd54ddc6cd917735386f73ff95884d92739d1af4a65386ad396fa01def9e3381133e2357506215519c2ebeffff12c35a70fb149d23f035d026ac1d24e5ec81f5c28684b4a203c29aaffc54794cabd3885090fc871cb8d4569190a39e3d34df87c3e05245ef7de4856d103e2a780361115f78e227622f93b7f9c2fd8390bfaf23a06cb348",
    .p_key              = "280a05e6ba0542e4e16dafe63d738fb4c544241b6626729a"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_160) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 192 Encrypt message_len=160",
    .p_plaintext        = "7c727bd3e7048e7a8995b7b1169ae4b5a55e854bb4f7a9576d7863ab2868731d307322dcca606e047343676f6af4d9cf6ebf2bf9c95d87848d233c931e7a60eff08fb959924cde1eec8699ebc57890e3887024ef47c89a550018788d1faa3250452e06f148af25f07bc613cd2f0e501a79d738d4361f28f34dbee24034e03367b6b8d34df3738ca3a86b9ebcb09e639bcb5e2f519f4a7a86fc7c41556404a95d",
    .p_ciphertext       = "922812ad5feacdf11fe7fdae96300149419e31cff54061b3c5ed27fdb8b50c9c0932b522a6c04e482499b011ef3c3e9dc56a1a61cfeb78b34032d26dbdc3cac51a3279bc934b9bce2d9c19bf858235613ba784e48e292d22c6b5a28e1d1bb860524fb7b5f9b3d9a5f4da66e340585bd2496fe6d6942db8d05d716fec03b17d19abb58b33332e24beaec7995d69525364fe139aa1fd62054668c58f23f1f94cfd",
    .p_key              = "4f41fa4d4a25100b586551828373bcca5540c68e9bf84562"
};


// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_16) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt message_len=16",
    .p_plaintext        = "49aabe67da5322b6e11d63b78b5a0e15",
    .p_ciphertext       = "21c8229a4dceaf533fe4e96eced482a6",
    .p_key              = "f2d2b82280c2592ecfbcf500ae647078c9c57624cde9bf6c"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_32) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt message_len=32",
    .p_plaintext        = "71b805feb56aaca22049073db10ad696751a4818b7005ae35eb3111cfeafcac8",
    .p_ciphertext       = "e741f38ed720877b53264ce81bdf2e5ea5b97163710ba29e9755cd3ba2caf091",
    .p_key              = "4bb83429faaaa25265f65c8ebb1caf815916810defe89281"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_48) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt message_len=48",
    .p_plaintext        = "7991ecf8ec98c6da6a27ee89055783f7302f5cb063f9c67084850702dfc9d64834536ecf524dc10f8504c1b132101ce5",
    .p_ciphertext       = "d1568d45f495112f12e37461aacddbfda7a71d0a40dacb6c1d7e6e13bb2c0f72110dfd337437ca29b83a978698f5914e",
    .p_key              = "2ad0db3278b7f6afd75a0cb62b6132d36449010ef509a5b6"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_64) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt message_len=64",
    .p_plaintext        = "9b540d3eb0bf52210aefe3b1929b3e43561c4110fb81520d41d671fc0f404dcddd879ef4a70f93311e6b034a6020e2b4fd5161748b57b61e7cbbd1d8e1d2cf42",
    .p_ciphertext       = "fd71fec985a92046a5c0b633112bb2cdde3581d98bf4323b417bdbc55a51384d21229602d8b5ef00001e5721d4359616174617b70f0a0198d2d6a3ddc013154f",
    .p_key              = "b7f4c377e6233c34717e7ac9040d369ac66c6d3f3b1e627d"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_80) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt message_len=80",
    .p_plaintext        = "f7058804932feae0851aca9be500c54ec35b3552dbb1417d81de51b0b0f3209dc999302369f25b1f133dfdbd9fe50b6834273806ef525c61ec4f5e88714cb945b0fa6481ce75bbff50e8eece1527baa7",
    .p_ciphertext       = "e24ff85f7ae75226ddd393972c3f836a0fc4aa13f997b896ae01249b030f12c5e58a7a409d29879952063dd3d04f5fb7883e906bd74bb8468ba0a9e3a3cd037456bd545957e884f258cb71b1cd95d673",
    .p_key              = "f714e621c97768d82eea6bf98ebf4a82c005262188ff894a"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_96) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt message_len=96",
    .p_plaintext        = "482466fe1bcbeb5ce8d21eb3b2c30997d36d4dd55f21e8271ad0b1b0cb12be86702eda3879db2838ee3a55fe9aef70c86118ac30b44030b7b53f8b21daba1a4c0137f58e1832aa8247809746e33da9a45ba6431d261546a9784c321be360d32e",
    .p_ciphertext       = "2a81ae3d397c7cd3b9746db43cc04b137261e5124c9158ed609e8d7590f903c1e68acf20c347e151390588f7b58e3947e1f38c04c82487ad3cbd1ff57423fc73fad07d9bc5ebfceda11c536cd89e9e520b96430261a3303ee4b6649a2724b130",
    .p_key              = "7c045f663d668e1f7ba80f06d0d25fefcb00b7808aaa3e95"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_112) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt message_len=112",
    .p_plaintext        = "723e9a19c8526d312c75beef81c398dd3371594f98e7740ed9ffd65e347fde28cd677f58284930c6f45c61365103f24bc28034fb19f16549e0acbcaf141952e44df19e9256789a5054e8196eb4c52dad29a5e683cc668a78fa43526a0fc62c88229448098ef3d9bc237ff4f97a2e9491",
    .p_ciphertext       = "fe9a6e64dc5a5e485c96b854299ba3e6b958610780c325f1647825cf70a3f1c3cc21032dede2972b0992bd5cce78acea539e59c74f324836892ca28e8f31893aabb7a805136b0e9560cf3c3441287cd771977558e686fd074c452e61b1f5bc1f82a68573b6bc6a8e49975e65ec73bcb5",
    .p_key              = "0b4bd7be14e239620c8eb992a48ab50b3815849401d04e83"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_128) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt message_len=128",
    .p_plaintext        = "34a8cbb7afab72a8e5753f04a5d18726f1f0a9a92e02f593d78d28c9dd50377af7bd4efa3eeb7b71ca7beb420161358088e2bdc295b65fcbe6097f7178a278b601a26102afd341f8c94f89b792ae421fcd458879704e9224f102b6ce204c55df73e15539d2fe04e8dc3e25e9e893994b5ae2705d65c01e0586ca4b18abf14f30",
    .p_ciphertext       = "557d3629e71ba544a20abaa2f314e77b844ac791609b5f6359a6a5a4a7cd3eebd18b8cacd107e86773e92c1ffe6c750e16b42797e163317c6967925141151c316c7c77af7af36a0ff4f556b7fa2f03e02bd69f91c53e04ce0da93a0df25d3de9edf9001a2b08b1a5eed73f5031841a598b583a67555662c10f14a6be00ca2b7d",
    .p_key              = "f5bd199c43e9917dfddf8464483ee605587d21ad907f5c7b"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_144) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt message_len=144",
    .p_plaintext        = "d09b7aa92197e1a6a42d67a02091bc10f8b59cfea963d8ea279e8cd46b517508817aab5d4a7627ea39ad27b03a3b1c610f724bad22996d8249a4eab076e3091100da1aeb20698c00879b1f44549cc5209a0ee3a2f897355eed420cfa42a141944e904023bcd099dda51eb5444adbb30d920f887dc04362783c2fed4f336d5c0631acef029d95b545dcf9d0c40df2de6b",
    .p_ciphertext       = "c11a65accf2faa952d078713b6a73c7648fe4a4b0f1cd82d06a055deba6f5f9652ace39acce48611898844130ca15704ffbf5b207632af56787d1e5abc08bff6b01519a1bff58b52d47cec961ab26ee107cbc821985a9499dec1f5edb9ec7e79e43f4fe731cc810116cac5c3694247b5680cd978fcfa4d9e3f5322bf3b8364c8f510cd97aad030c5585a18c380baac29",
    .p_key              = "2f8118dd5b2ec401b9f501cf0149cb65ac451ed943e25e15"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_160) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 192 Decrypt message_len=160",
    .p_plaintext        = "b8bb5ce53a15aa6dfdf2cb61bc8e3617d1d0fefe9ba5d175550470e32397f6f3b3e65b43bded2b21e5c181d3c4c4c526c41ceab044289508458048b63352dfc379de373fd19a2c900c43524b75949e677cceda866f7f2bcc4844ef2e5dac5b804b4045e657c8156d1dcdb43cbf2f5e00a4f9255e3be2439436c4d0449a8d2c4c1a56bece98ea0fd68abaf12398039994aebffc692b9000e580479b4f4b28b5fe",
    .p_ciphertext       = "952f4546a8bf7166964917ece01bda3c6857e427cef5da0ff90b0e4bf44cf7ccfccfdf01d713dcf9673f01c87eaed52bf4aa046ff778558ea396dc9cd240716136386148a5c76378b3ffcd40864407b8e60b40a594e0619eddae3f6d6e3b15b86af231e1bae5ed2aa512e11da0e5572b67ffff934c36e585cfdd9f877045cb19c183b994bf74645862ffa726739aadcb9e10aaffc881c88ca3aa65b37f667bcb",
    .p_key              = "9cc24ea1f1959d9a972e7182ef3b4e22a97a87d0da7ff64b"
};



// AES ECB - NIST CAVS 11.1 Monte Carlo 192 Encrypt
NRF_SECTION_ITEM_REGISTER(test_vector_aes_monte_carlo_data, test_vector_aes_t test_vector_aes_ecb_192_encrypt_monte_carlo) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB Encrypt Monte Carlo 192",
    .p_plaintext        = "85a1f7a58167b389cddc8a9ff175ee26",
    .p_ciphertext       = "5d1196da8f184975e240949a25104554",
    .p_key              = "b9a63e09e1dfc42e93a90d9bad739e5967aef672eedd5da9"
};

// AES ECB - NIST CAVS 11.1 Monte Carlo 192 Decrypt
NRF_SECTION_ITEM_REGISTER(test_vector_aes_monte_carlo_data, test_vector_aes_t test_vector_aes_ecb_192_decrypt_monte_carlo) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB Decrypt Monte Carlo 192",
    .p_plaintext        = "b63ef1b79507a62eba3dafcec54a6328",
    .p_ciphertext       = "d0bd0e02ded155e4516be83f42d347a4",
    .p_key              = "4b97585701c03fbebdfa8555024f589f1482c58a00fdd9fd"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_ECB_192)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_ECB_256)

// AES ECB - Functional test using test vector NIST CAVS 11.1 ECB KeySbox 256 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_func_data, test_vector_aes_t test_vector_aes_ecb_256_functional) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Functional",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "46f2fb342d6f0ab477476fc501242c5f",
    .p_key              = "c47b0294dbbbee0fec4757f22ffeee3587ca4730c3d33b691df38bab076bc558"
};

// AES ECB - Custom test vector - Encrypt - Message length 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_c0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = "c9defba265de96a179a1128c2b5ef0edf6365c3064df88e2d46b2bfae20b599d"
};

// AES ECB - Custom test vector - Decrypt - Message length 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_c0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = "c9defba265de96a179a1128c2b5ef0edf6365c3064df88e2d46b2bfae20b599d"
};

// AES ECB - Custom test vector - Encrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_c3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt message_len=1 pkcs7-padding",
    .p_plaintext        = "de",
    .p_ciphertext       = "884506eb1952533a9020db23b501558b",
    .p_key              = "c9defba265de96a179a1128c2b5ef0edf6365c3064df88e2d46b2bfae20b599d"
};

// AES ECB - Custom test vector - Decrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_c3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt message_len=1 pkcs7-padding",
    .p_plaintext        = "de0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f",                                       /**< The 15 padded bytes (0x0f) will be removed in the actual generated plaintext. */
    .p_ciphertext       = "884506eb1952533a9020db23b501558b",
    .p_key              = "c9defba265de96a179a1128c2b5ef0edf6365c3064df88e2d46b2bfae20b599d"
};

// AES ECB - Custom test vector - Encrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_c4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt message_len=16 pkcs7-padding",
    .p_plaintext        = "dededededededededededededededede",
    .p_ciphertext       = "758dbc3193a5d4edbe88d59d5488af319e10a9035476d946d238f0e5813f3678",
    .p_key              = "c9defba265de96a179a1128c2b5ef0edf6365c3064df88e2d46b2bfae20b599d"
};

// AES ECB - Custom test vector - Decrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_c4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt message_len=16 pkcs7-padding",
    .p_plaintext        = "dededededededededededededededede10101010101010101010101010101010",       /**< The 16 padded bytes (0x10) will be removed in the actual generated plaintext. */
    .p_ciphertext       = "758dbc3193a5d4edbe88d59d5488af319e10a9035476d946d238f0e5813f3678",
    .p_key              = "c9defba265de96a179a1128c2b5ef0edf6365c3064df88e2d46b2bfae20b599d"
};

// AES ECB - Custom test vector - Encrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_c5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt message_len=31 pkcs7-padding",
    .p_plaintext        = "dedededededededededededededededededededededededededededededede",
    .p_ciphertext       = "758dbc3193a5d4edbe88d59d5488af31946e8b4acfb239142704b12f8cd961a0",
    .p_key              = "c9defba265de96a179a1128c2b5ef0edf6365c3064df88e2d46b2bfae20b599d"
};

// AES ECB - Custom test vector - Decrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_c5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt message_len=31 pkcs7-padding",
    .p_plaintext        = "dedededededededededededededededededededededededededededededede01",       /**< The padded byte (0x01) will be removed in the actual generated plaintext. */
    .p_ciphertext       = "758dbc3193a5d4edbe88d59d5488af31946e8b4acfb239142704b12f8cd961a0",
    .p_key              = "c9defba265de96a179a1128c2b5ef0edf6365c3064df88e2d46b2bfae20b599d"
};

// AES ECB - NIST CAVS 11.1 GFSBox Tests 256 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_gfsb0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt GFSBox 0",
    .p_plaintext        = "014730f80ac625fe84f026c60bfd547d",
    .p_ciphertext       = "5c9d844ed46f9885085e5d6a4f94c7d7",
    .p_key              = "0000000000000000000000000000000000000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 GFSBox Tests 256 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_gfsb0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt GFSBox 0",
    .p_plaintext        = "014730f80ac625fe84f026c60bfd547d",
    .p_ciphertext       = "5c9d844ed46f9885085e5d6a4f94c7d7",
    .p_key              = "0000000000000000000000000000000000000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 VarKey Tests 256 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_vk0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt VarKey 0",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "e35a6dcb19b201a01ebcfa8aa22b5759",
    .p_key              = "8000000000000000000000000000000000000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 VarKey Tests 256 Encrypt - Count 255
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_vk255) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt VarKey 255",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "4bf85f1b5d54adbc307b0a048389adcb",
    .p_key              = "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
};

// AES ECB - NIST CAVS 11.1 VarKey Tests 256 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_vk0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt VarKey 0",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "e35a6dcb19b201a01ebcfa8aa22b5759",
    .p_key              = "8000000000000000000000000000000000000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 VarKey Tests 256 Decrypt - Count 255
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_vk255) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt VarKey 255",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "4bf85f1b5d54adbc307b0a048389adcb",
    .p_key              = "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
};

// AES ECB - NIST CAVS 11.1 VarTxt Tests 256 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_vt0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt VarTxt 0",
    .p_plaintext        = "80000000000000000000000000000000",
    .p_ciphertext       = "ddc6bf790c15760d8d9aeb6f9a75fd4e",
    .p_key              = "0000000000000000000000000000000000000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 VarTxt Tests 256 Encrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_vt127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt VarTxt 127",
    .p_plaintext        = "ffffffffffffffffffffffffffffffff",
    .p_ciphertext       = "acdace8078a32b1a182bfa4987ca1347",
    .p_key              = "0000000000000000000000000000000000000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 VarTxt Tests 256 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_vt0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt VarTxt 0",
    .p_plaintext        = "80000000000000000000000000000000",
    .p_ciphertext       = "ddc6bf790c15760d8d9aeb6f9a75fd4e",
    .p_key              = "0000000000000000000000000000000000000000000000000000000000000000"
};

// AES ECB - NIST CAVS 11.1 VarTxt Tests 256 Decrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_vt127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt VarTxt 127",
    .p_plaintext        = "ffffffffffffffffffffffffffffffff",
    .p_ciphertext       = "acdace8078a32b1a182bfa4987ca1347",
    .p_key              = "0000000000000000000000000000000000000000000000000000000000000000"
};


// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_16) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt message_len=16",
    .p_plaintext        = "ccc62c6b0a09a671d64456818db29a4d",
    .p_ciphertext       = "df8634ca02b13a125b786e1dce90658b",
    .p_key              = "cc22da787f375711c76302bef0979d8eddf842829c2b99ef3dd04e23e54cc24b"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_32) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt message_len=32",
    .p_plaintext        = "7e771c6ee4b26db89050e982ba7e9803c8da34606434dd85d2910e538076d001",
    .p_ciphertext       = "a91d8b2ddf37520bc469470ad0dd6394923143ce55386beb1f9c4bd51584658e",
    .p_key              = "7a52e4d342aa07255a7e7c34266cf7302abe2d4dd7ec4468a46187ee61825ffa"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_48) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt message_len=48",
    .p_plaintext        = "68c9885ba2be03181f65f1e04e83d6ba6880467550bcf099be26dc9d9c0af15ab02abac07c116ac862a41da90cfa604f",
    .p_ciphertext       = "a7603d29bbba4c77208bf2f3df9f5ec85204adce012299f2cce7b326ce78f5cf8040343dd291e8cf9f3645726368dc20",
    .p_key              = "605c4139c961b496ca5148f1bdb1bb1901f2101943a0ec10fcdc403d3b0c285a"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_64) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt message_len=64",
    .p_plaintext        = "351fee099122e371c4830f409c6c4411186d22176f7138b054f16b3c79679c2f520685651ba8e4b61c08dccb2c31982f743631a97524d2ca4d351ac23546c178",
    .p_ciphertext       = "8b9c9e692c16e7059818e285e85d8fa5433dee2aff9fec61d6a0a781e24b24f64902fbd18cef7461ad7760cfb2442fb74ffd9be108a386545f2a216430ef16fb",
    .p_key              = "f984b0f534fc0ae2c0a8593e16ab8365f25fcc9c5947f9a2db45b588160d35c3"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_80) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt message_len=80",
    .p_plaintext        = "4b4b12d6ee6fc0bf987eaafe2634aad464781ff4c83d3f8a61a6af7c0a6d51f0e3855d0e02feb307652a6f562bfebe4604baf1b4e7cdd01603f231bcf7a0c95645a141b704008cd8d62979201a4c84e2",
    .p_ciphertext       = "fa18d25e37ea0ce94f0949efc0edecc6a40fada8f007fd8e760afed0a83ebb350c82b03baaa6ee19f791bb9bd1b44d27a76fc6eb0e1c0017d68776ed69a541851a732e46ef328def064baf6a0a755588",
    .p_key              = "ba42b760bb5a5de21acb9aba214c9783cd71ea841ada018580abc4e1be3b76dd"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_96) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt message_len=96",
    .p_plaintext        = "b36809198132518d815aa37f32f40dd7a952ec8bd63355700837b450b396b33c72125e23482a84a42b916021c3de78156f85c6a78906167feb64afd8b1d935d641c8ce1a89f3849588eeeb9910d40336ca385fc37a5e87bb84ab9ccbb05b3a28",
    .p_ciphertext       = "7ae803b14914d156439f580c02c592bf9a41b7b80c20168129a33fae2290403f01a6fabe4fea7ac770ffbc6c421f8e013b9e831674ef17eb27d446dded3baf50686b809c18b6bbd588cd3c7423b64987e9164b7e1e66987dc9319790ae27b3e8",
    .p_key              = "1d29079cc34ab5a3bc713f416a129f9d26ada15fca458cc2731404ea857d2f79"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_112) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt message_len=112",
    .p_plaintext        = "e199899e1e12cffcb28909aec51b36c2f96fab49ef32b9650cc38aa37d2f4c8b785f9176c590f6a07e04037e13f7535290d5f5fc23aa1113d9dacf34a812749ab27ecfefc83dd3622d1285fa9d5c192a8e4857a5b6454473cdb8ff4594f1e906165e08b22effae6b491a55ca6d30ce73",
    .p_ciphertext       = "2c4998428e72f6d96e982a316f73bf2a7da81730909b65403489ab92ada6de11882d08742f90f0f109d3420b00b8abe6873f4fdd1474923da2c5bdea2e4523ffca2132015ecf7c9cac9de2f956b112c4ba8e4c8e4b354a3df6e4652d6a77ae982a24d15cff71b979538d49994fd38761",
    .p_key              = "2e39c585ce4900d323ce29713bebe73a1be08a0cb22e9f1310fcc14ad4b9b23e"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_128) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt message_len=128",
    .p_plaintext        = "0f5321db6fd9d816d88e28183a739d90974e76095caa9f12f11fe49c8fd35fa3524132118f397cdf6728857d9c9a3c74a4fde44afca80aa5bf1cbab47789f2cb3394574dda5727cfbea96f7a74a07eb1e45599f49ce7f0056ac3d14929221c70dbd3f759f83ac22f06994ed96a8e49917eddfdd2e3703b78199c91234ca6c3dc",
    .p_ciphertext       = "2ac6de212da0434bea9cdd7332637307131d31e8c4b0c1fd02298e249bfa9f64f34aae45faadf79d971ae82b033d903f6b18adec171ec83ccd147b44d05dec5cedb574534e8901385534c3f1dae4aebee0eb2164975e8b4e85a7b5a766ffd8247885c2b6429eb659b92c8d953af92b54517933566654d804665112bec17ff3a4",
    .p_key              = "85405c4f0ebbe8f29228f02f1ff184e2f5e7857e8933c2a1d08f61ecb9b68111"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_144) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt message_len=144",
    .p_plaintext        = "3637f71f60a430322980349ad414fcfdc14f87e9915d210e8b7be5aa3e09814468e0399d17e72fe40ee1e1296a89f31486e12fd71bc7ca61acc9e8d4213a633ab285c87406c2a3729c87fdaa6b0122c2c543a89081ddac4559b15fe59102c2fbdacead8a755e16469b1b90041da31270481cfafe0bc951235cda51c4d78924efa862efc9ace20fc7d344a321c9984a84",
    .p_ciphertext       = "ca0a683e759c1312928fe01198f625bba044bb9003e82b9279b6812feefe54e30ccc0ca51fb858eba9ed4667e85c146a424071e7bf603f1f538ead57a3e29c583549c08e279bb078cc51e42eeb3d2443da965192cd0478e8d7ea1343cf90196f520933e3aab6db2dcd9a76534e05483eea2d373deae95d6213b91db2b96aa6adce727cdf7e43bd01978d07e0241cf1c1",
    .p_key              = "f157285db00e64c2791668a54493966e3039a19426605056b95b7eac5106667d"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_160) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB 256 Encrypt message_len=160",
    .p_plaintext        = "c91b8a7b9c511784b6a37f73b290516bb9ef1e8df68d89bf49169eac4039650c4307b6260e9c4e93650223440252f5c7d31c26c56209cbd095bf035b9705880a1628832daf9da587a6e77353dbbce189f963235df160c008a753e8ccea1e0732aa469a97659c42e6e31c16a723153e39958abe5b8ad88ff2e89af40622ca0b0d6729a26c1ae04d3b8367b548c4a6335f0e5a9ec914bb6113c05cd0112552bc21",
    .p_ciphertext       = "05d51af0e2b61e2c06cb1e843fee3172825e63b5d1ce8183b7e1db6268db5aa726521f46e948028aa443af9ebd8b7c6baf958067ab0d4a8ac530ecbb68cdfc3eb93034a428eb7e8f6a3813cea6189068dfecfa268b7ecd5987f8cb2732c6882bbec8f716bac254d72269230aec5dc7f5a6b866fd305242552d400f5b0404f19cbfe7291fab690ecfe6018c4309fc639d1b65fcb65e643edb0ad1f09cfe9cee4a",
    .p_key              = "44a2b5a7453e49f38261904f21ac797641d1bcd8ddedd293f319449fe63b2948"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_16) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt message_len=16",
    .p_plaintext        = "8b2b1b22f733ac09d1196d6be6a87a72",
    .p_ciphertext       = "4154c0be71072945d8156f5f046d198d",
    .p_key              = "a81fd6ca56683d0f5445659dde4d995dc65f4bce208963053e28d7f2df517ce4"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_32) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt message_len=32",
    .p_plaintext        = "a8fab53790af3519cf21978e3cf03ba8e52b902be23311bf17f1ad2c5ff37c16",
    .p_ciphertext       = "0a5f3278d7d96632e0508391e813f06b35d8d754ddf5867240d3168dd69f4a66",
    .p_key              = "bc1415aa119c29a9a27c0ea9d19ed50ace86c488b8e39d6a056424fb23cddb3e"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_48) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt message_len=48",
    .p_plaintext        = "f17733de8f763110ef4b305594936ca2bb75119ad65261be32ba919a2c3ef8b8f1c42f62b8474362e53ee7cc6c82a647",
    .p_ciphertext       = "c1c83fa3cd3d52524876e715bc28efe7c7c4256a139e9d2c874ea029bf56b792ba0606cdd39ddbbdf3b1874304d16d05",
    .p_key              = "0d0ec1b61ebc5177c4513ef1d7d5bb97d06abaa2d33710a8eda6d3709acf0705"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_64) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt message_len=64",
    .p_plaintext        = "959b7d696e2267f3d465f6f77ff2453296cfa54843363981484853e67cc21d340b803d6d653213d1037b81d849c5acc361771a5a072c9c29d6bc5096c38c9c86",
    .p_ciphertext       = "254c015626baa3ed2d7f05f5d0a98c8cc2f29dd7a4410ea41d74db4e2c5d1be1bd0a327f7b4a4703e66cf9e3a54a4d5b1d87ebc842148b52adf9aacdf05281e1",
    .p_key              = "ae67999eb240a9e5b6e3f0bd6b504530000be51343b8bc3b0ae8e0eed1335f98"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_80) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt message_len=80",
    .p_plaintext        = "5889ad2c09a6307611e6115a78c13566de05b5892f7850fb917f83898e07486cc9ce746e891db102a0f570d7ad3c2804f40cdfe23bcc8b2c8a3bfba48632892d3df3bb7bbd1029b915cab2faf281c4e6",
    .p_ciphertext       = "ea1fd2f064548906ad10ce1240758868ed9fb32921dabe18681232a8308b955ad0d28e45c9f34af64bec1d7bfb626dbbf393cc090a8c64f8569b9870f008e801f7001578d8d286b0cc5eebaba2c920e1",
    .p_key              = "dde079371133ebd68df061b56f0efd3a14c137ced35a30e0eb68422cb924dc3d"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_96) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt message_len=96",
    .p_plaintext        = "a56e000be19eced20479cbe8964fdb366c437ca3b5fd9d04c339c1a51bdb0aad46df1b78ef05b02e492464136155d76dd32c3b4aa4198c3926cb75d2578434ab4fff0894508b6f60ff39686a0fd151d1bffba7a786b1bc02acd2d23b56e45749",
    .p_ciphertext       = "59722586a56ed6c8207d6a0a9f7278588f5203782fb64e6ffd71f1486d732d10e1ed7d25d6b66db15beeaf71c8e16cbbac2dd8bf0728090dba7c09573b142a788d3c99b670033644d5e4a30b94a69bab618186bcfd824a59a93163dfe07036f2",
    .p_key              = "d68a345159178b9bd2e3bd7a13c9512ee9b397944eff81a8df28b44890a2df3b"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_112) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt message_len=112",
    .p_plaintext        = "34467d07c2e49c44e9003da836786169e7c0fb54f1e2f178387c2f759d50809ce84d6705fe6350907d5e945e262af378753163f5569be9c09c874d254dde2545898d4cbd2eaa1ade9c8a02f8d76d418565d02075e082999bbf6f2ba985f65f17c73cc0cc29ac04824db98b3cc2b77126",
    .p_ciphertext       = "bed9f2db31ebbfac57f3745e0dacf71087cb35c94326b9ba3b1b7dc35f30003942f798e714350a52d0425842b6b3da7dfc3790d6c1bfb76642e29a1a507fab6e02a4d277a7ae0527feb219cdaa1cd8dbe09653ec632c7a05b2ade91b7f5405d664a74895d2a0d9b24a0b6007cf2f18b1",
    .p_key              = "5a20ff7da47c7e853becca0ca5f32bc80e17de973337146f7e1f3c93725a850d"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_128) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt message_len=128",
    .p_plaintext        = "51c527c0985369234a599f476731e8b51ac7a44bfbe37ec71a641ad71ad464f9e45467d82a0d101f67043b87e2da34de183459295000cea9e0f0ccdd82270fc70b880fa80104e0b78b7a5b1620dee83da84702223b277a0966b10ddf44ef06b98e48788092c763895d95f13fd389fff5706e70876d5af8c197cdebb38a4d2ba6",
    .p_ciphertext       = "496553f24d47264ef74a584b893ccd4ea1cfc0b1044aac15997e6dfab9fca28da68557c058c2e1a0803508e4c7706cc1568521069cbd9d644b8dd40da4c5a182f50b68a56088fa16dde1b8bc8269fca20395b40575b050ab57cfdf76d208743840dfcfc9ec0139571835b0fe458f4498aee6c1d716898e91c32ed55acb2be458",
    .p_key              = "082d33edd0a1ad3de596762d711bae6f3188a12c7b6ced987fc7e8c9cd7a3cc9"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_144) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt message_len=144",
    .p_plaintext        = "5f967e10da68887567b514293149697212888efc9fd90604459f8a9c4fa1f4117f0214fa675b680571bd980cf941bfccd4826e1fe1420a0ba59554341b507e760d2d85c7219b3f5e261eef2d201bc134d2ec32d8b97157e3de91f1c312b26ff193e637bf78012850f23d0536715e51a68be730c55375774642345e0daaa4c40185985cab38df09dac9a688e6b597d327",
    .p_ciphertext       = "2b3c036be68fba7a625dc72a0e68b6677326f8751714e0e1a14df2b73d5b5df5424dc640130947d643e4b8c4fc702d59a195bfdab2e49077f027097ec23d66cfd82575900e589a21935c17524a968ad47060b7ee805b8871530652ee8c905a2633eb98c2a022bb459bec6d3ec181d7c22aac68194822d2b81212566f62fd42e49cc8b8409110b028a3bd40ef29da1c0a",
    .p_key              = "3e1934554952b87a07d6371721f4d78a0e9adadc42be7347a2fcd8f53c81990b"
};

// AES ECB - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_160) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB 256 Decrypt message_len=160",
    .p_plaintext        = "31fd5a307e279b2f34581e2c432379df8eccbaf79532938916711cd377540b9045373e47f2214b8f876040af733f6c9d8f03a7c58f8714d2fbb4c14af59c75b483adc718946ee907a18286cc4efd206789064b6f1b195f0d0d234468e4f00e6f1cad5cd3b9c0a643b3c0dd09280ff2e2a5929183409384dd72dc94e39687ea2b623d5d776700bd8b36e6130ffde966f134c4b1f35f29c5cc4a03297e1ccc9539",
    .p_ciphertext       = "2c487fa96f4090c56aa1b5be81918a934c9492878fb0cd686dcf8d17d86485454c51237bbd09205dcef1552f430dd098b9d827a694730c133a0222c77f540f9d5fc2d36af359583c9e3b49df884228a64de79b67f66207c8281360b99b214042ce61367ff97960e944453cd63679bb44708897d29bc5e70f9fc8f1f715143fbb00f7f5c1b7b161ec26d8d41d36fab0fa8a85c3ee6ce4d37007eb7a89d6753590",
    .p_key              = "c4a71e055a7254dda360693fe1be49f10faa6731c36dbaa6590b05974e185c5b"
};




// AES ECB - NIST CAVS 11.1 Monte Carlo 256 Encrypt
NRF_SECTION_ITEM_REGISTER(test_vector_aes_monte_carlo_data, test_vector_aes_t test_vector_aes_ecb_256_encrypt_monte_carlo) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "ECB Encrypt Monte Carlo 256",
    .p_plaintext        = "b379777f9050e2a818f2940cbbd9aba4",
    .p_ciphertext       = "c5d2cb3d5b7ff0e23e308967ee074825",
    .p_key              = "f9e8389f5b80712e3886cc1fa2d28a3b8c9cd88a2d4a54c6aa86ce0fef944be0"
};

// AES ECB - NIST CAVS 11.1 Monte Carlo 256 Decrypt
NRF_SECTION_ITEM_REGISTER(test_vector_aes_monte_carlo_data, test_vector_aes_t test_vector_aes_ecb_256_decrypt_monte_carlo) =
{
    .p_aes_info         = &g_nrf_crypto_aes_ecb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "ECB Decrypt Monte Carlo 256",
    .p_plaintext        = "e3d3868f578caf34e36445bf14cefc68",
    .p_ciphertext       = "89649bd0115f30bd878567610223a59d",
    .p_key              = "2b09ba39b834062b9e93f48373b8dd018dedf1e5ba1b8af831ebbacbc92a2643"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_ECB_256)

/*lint -restore */

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_ECB)
