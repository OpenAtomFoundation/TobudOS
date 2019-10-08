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

/**@brief AES EAX test vectors can be found in the paper: "The EAX Mode of Operation
 * (A Two-Pass Authenticated-Encryption Scheme Optimized for Simplicity and Efficiency)".
 *
 * http://web.cs.ucdavis.edu/~rogaway/papers/eax.pdf
 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_EAX)

/*lint -save -e91 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_EAX_128)

// AES EAX - 128 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_c_encrypt0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 128 Encrypt message_len=1 ad_len=0 mac_len=4 nonce_len=1",
    .p_plaintext            = "2a",
    .p_ciphertext           = "17",
    .p_key                  = "f3ac7f5781e17f46d0d7e48bd8eac9d3",
    .p_ad                   = "",
    .p_nonce                = "8b",
    .p_mac                  = "4a8d79c5"
};

// AES EAX - 128 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_c_encrypt1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 128 Encrypt message_len=0 ad_len=20 mac_len=4 nonce_len=1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "f3ac7f5781e17f46d0d7e48bd8eac9d3",
    .p_ad                   = "38b759a2375e8036502c27b58893eb9b85f47c35",
    .p_nonce                = "8b",
    .p_mac                  = "18786d39"
};

// AES EAX - 128 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_c_encrypt2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 128 Encrypt message_len=1 ad_len=20 mac_len=4 nonce_len=1",
    .p_plaintext            = "2a",
    .p_ciphertext           = "17",
    .p_key                  = "f3ac7f5781e17f46d0d7e48bd8eac9d3",
    .p_ad                   = "38b759a2375e8036502c27b58893eb9b85f47c35",
    .p_nonce                = "8b",
    .p_mac                  = "5b320e1f"
};

// AES EAX - 128 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_c_encrypt3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 128 Encrypt message_len=255 ad_len=255 mac_len=16 nonce_len=16",
    .p_plaintext            = "7e61def9f2192a90034d6a5cf1045cc374a83e5edca6a3d23b51c7cb13ce4d0a67e08937d368e0ccfc520cbd3977d1f019a37c4fe25a94ed1381f63fcb81ff17e5c2102cbcab4ee088f7da0264d8f500ed1b32103c1cbe7a1ce369f1e265e73d5feadb351179cdfb3d917cddf42bde16bc91c77ff3628d3fc034438e73d8b45a802ff872be9dd555499ba0331b4838d27629a769e36efd873ce0b704c25b927f34f43391c4f58a281be789c1afa78d89d3f5e13d39dad8b6b75f27a4a991043cc86fa2380dad9665e441b62a25822970e1a944e87a948c40579bacf4bc920dd9acee204ef3c9dbe5e09ee9845b2e312880fb949c8289438e0c48bcf28ce87d",
    .p_ciphertext           = "6d7e37a06ea3dbac3910a976fcc8ed09d7f6870b3dcd5ab992c002ff4b5135c2d351878461043abd1cf94fbd2bf342776e3bf2f0808832ac214d23cded3b36dddb9e7622c6756aafbe14d60fcbe9badc8cd3f435c3ea665727b98b8d2a94e94963fc93325a34b96e104fcf4637a44cb324cb7d2f16c62928ab3cb2cda63514cec5dc03f21c774243c2b21d7b047bde39a27c1d8e6712ae2bd08668e6357c072f9fa099647399fad106b1b6899c0c77c36a35be9b9b1318e558a5fa86ae2add1d3f5f34b1bc8d2e33768550c0628f9180a562c8c45846e9bd012ce67ab30a47a8c52749df8a132d0111de1a99b3a8d7f394aa8371c00bb5fc39c1103a3d8959",
    .p_key                  = "f3ac7f5781e17f46d0d7e48bd8eac9d3",
    .p_ad                   = "c3a3a82d25b7d6cd060985c09f02b16b10693bb4cc536052a280f0d2a4904d3e95da5a3389f92fd3b8fe20f000b926819b05edfb901f33ed75b8aae146da9f18c90bbaf1845fbb1263d7d89d0f871c899152fb5ef5b51282ba5c4e183e22ad0d416f75cef49e641ff3e5b6de8d18ca0fd9d34a9580f4abdd8b07354c232c4305c7d2bbdedf4f318579c8a76a877961bb8f52b4943351158d062e92d0a9c592f8e3776ffcbf963ec1b0e4ecc0d30a7f8a4b50b42a14d144d8a89706cdb10f7005dfc6915b489f9638e7eed492819447a5ddb321fa4080216a58fdbf8dda968b16fff7353d669aec1eb23105ecec9d511cab89b5ff16fb744b92a44ce19b5bcf",
    .p_nonce                = "82cb43c4e79279a5addc07a5667e751f",
    .p_mac                  = "294df83ae2b1690c93347632fb52034b"
};

// AES EAX - 128 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_c_decrypt0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 128 Decrypt message_len=1 ad_len=0 mac_len=4 nonce_len=1",
    .p_plaintext            = "2a",
    .p_ciphertext           = "17",
    .p_key                  = "f3ac7f5781e17f46d0d7e48bd8eac9d3",
    .p_ad                   = "",
    .p_nonce                = "8b",
    .p_mac                  = "4a8d79c5"
};

// AES EAX - 128 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_c_decrypt1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 128 Decrypt message_len=0 ad_len=20 mac_len=4 nonce_len=1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "f3ac7f5781e17f46d0d7e48bd8eac9d3",
    .p_ad                   = "38b759a2375e8036502c27b58893eb9b85f47c35",
    .p_nonce                = "8b",
    .p_mac                  = "18786d39"
};

// AES EAX - 128 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_c_decrypt2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 128 Decrypt message_len=1 ad_len=20 mac_len=4 nonce_len=1",
    .p_plaintext            = "2a",
    .p_ciphertext           = "17",
    .p_key                  = "f3ac7f5781e17f46d0d7e48bd8eac9d3",
    .p_ad                   = "38b759a2375e8036502c27b58893eb9b85f47c35",
    .p_nonce                = "8b",
    .p_mac                  = "5b320e1f"
};

// AES EAX - 128 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_c_decrypt3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 128 Decrypt message_len=255 ad_len=255 mac_len=16 nonce_len=16",
    .p_plaintext            = "7e61def9f2192a90034d6a5cf1045cc374a83e5edca6a3d23b51c7cb13ce4d0a67e08937d368e0ccfc520cbd3977d1f019a37c4fe25a94ed1381f63fcb81ff17e5c2102cbcab4ee088f7da0264d8f500ed1b32103c1cbe7a1ce369f1e265e73d5feadb351179cdfb3d917cddf42bde16bc91c77ff3628d3fc034438e73d8b45a802ff872be9dd555499ba0331b4838d27629a769e36efd873ce0b704c25b927f34f43391c4f58a281be789c1afa78d89d3f5e13d39dad8b6b75f27a4a991043cc86fa2380dad9665e441b62a25822970e1a944e87a948c40579bacf4bc920dd9acee204ef3c9dbe5e09ee9845b2e312880fb949c8289438e0c48bcf28ce87d",
    .p_ciphertext           = "6d7e37a06ea3dbac3910a976fcc8ed09d7f6870b3dcd5ab992c002ff4b5135c2d351878461043abd1cf94fbd2bf342776e3bf2f0808832ac214d23cded3b36dddb9e7622c6756aafbe14d60fcbe9badc8cd3f435c3ea665727b98b8d2a94e94963fc93325a34b96e104fcf4637a44cb324cb7d2f16c62928ab3cb2cda63514cec5dc03f21c774243c2b21d7b047bde39a27c1d8e6712ae2bd08668e6357c072f9fa099647399fad106b1b6899c0c77c36a35be9b9b1318e558a5fa86ae2add1d3f5f34b1bc8d2e33768550c0628f9180a562c8c45846e9bd012ce67ab30a47a8c52749df8a132d0111de1a99b3a8d7f394aa8371c00bb5fc39c1103a3d8959",
    .p_key                  = "f3ac7f5781e17f46d0d7e48bd8eac9d3",
    .p_ad                   = "c3a3a82d25b7d6cd060985c09f02b16b10693bb4cc536052a280f0d2a4904d3e95da5a3389f92fd3b8fe20f000b926819b05edfb901f33ed75b8aae146da9f18c90bbaf1845fbb1263d7d89d0f871c899152fb5ef5b51282ba5c4e183e22ad0d416f75cef49e641ff3e5b6de8d18ca0fd9d34a9580f4abdd8b07354c232c4305c7d2bbdedf4f318579c8a76a877961bb8f52b4943351158d062e92d0a9c592f8e3776ffcbf963ec1b0e4ecc0d30a7f8a4b50b42a14d144d8a89706cdb10f7005dfc6915b489f9638e7eed492819447a5ddb321fa4080216a58fdbf8dda968b16fff7353d669aec1eb23105ecec9d511cab89b5ff16fb744b92a44ce19b5bcf",
    .p_nonce                = "82cb43c4e79279a5addc07a5667e751f",
    .p_mac                  = "294df83ae2b1690c93347632fb52034b"
};

// AES EAX - The EAX Mode of Operation 128 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_encrypt0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 128 Encrypt message_len=0 ad_len=8 mac_len=16 nonce_len=16",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "233952dee4d5ed5f9b9c6d6ff80ff478",
    .p_ad                   = "6bfb914fd07eae6b",
    .p_nonce                = "62ec67f9c3a4a407fcb2a8c49031a8b3",
    .p_mac                  = "e037830e8389f27b025a2d6527e79d01"
};

// AES EAX - The EAX Mode of Operation 128 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_encrypt1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 128 Encrypt message_len=2 ad_len=8 mac_len=16 nonce_len=16",
    .p_plaintext            = "f7fb",
    .p_ciphertext           = "19dd",
    .p_key                  = "91945d3f4dcbee0bf45ef52255f095a4",
    .p_ad                   = "fa3bfd4806eb53fa",
    .p_nonce                = "becaf043b0a23d843194ba972c66debd",
    .p_mac                  = "5c4c9331049d0bdab0277408f67967e5"
};

// AES EAX - The EAX Mode of Operation 128 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_encrypt2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 128 Encrypt message_len=5 ad_len=8 mac_len=16 nonce_len=16 variant 1",
    .p_plaintext            = "1a47cb4933",
    .p_ciphertext           = "d851d5bae0",
    .p_key                  = "01f74ad64077f2e704c0f60ada3dd523",
    .p_ad                   = "234a3463c1264ac6",
    .p_nonce                = "70c3db4f0d26368400a10ed05d2bff5e",
    .p_mac                  = "3a59f238a23e39199dc9266626c40f80"
};

// AES EAX - The EAX Mode of Operation 128 - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_encrypt3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 128 Encrypt message_len=5 ad_len=8 mac_len=16 nonce_len=16 variant 2",
    .p_plaintext            = "481c9e39b1",
    .p_ciphertext           = "632a9d131a",
    .p_key                  = "d07cf6cbb7f313bdde66b727afd3c5e8",
    .p_ad                   = "33cce2eabff5a79d",
    .p_nonce                = "8408dfff3c1a2b1292dc199e46b7d617",
    .p_mac                  = "d4c168a4225d8e1ff755939974a7bede"
};

// AES EAX - The EAX Mode of Operation 128 - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_encrypt4) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 128 Encrypt message_len=6 ad_len=8 mac_len=16 nonce_len=16",
    .p_plaintext            = "40d0c07da5e4",
    .p_ciphertext           = "071dfe16c675",
    .p_key                  = "35b6d0580005bbc12b0587124557d2c2",
    .p_ad                   = "aeb96eaebe2970e9",
    .p_nonce                = "fdb6b06676eedc5c61d74276e1f8e816",
    .p_mac                  = "cb0677e536f73afe6a14b74ee49844dd"
};

// AES EAX - The EAX Mode of Operation 128 - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_encrypt5) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 128 Encrypt message_len=12 ad_len=8 mac_len=16 nonce_len=16",
    .p_plaintext            = "4de3b35c3fc039245bd1fb7d",
    .p_ciphertext           = "835bb4f15d743e350e728414",
    .p_key                  = "bd8e6e11475e60b268784c38c62feb22",
    .p_ad                   = "d4482d1ca78dce0f",
    .p_nonce                = "6eac5c93072d8e8513f750935e46da1b",
    .p_mac                  = "abb8644fd6ccb86947c5e10590210a4f"
};

// AES EAX - The EAX Mode of Operation 128 - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_encrypt6) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 128 Encrypt message_len=17 ad_len=8 mac_len=16 nonce_len=16",
    .p_plaintext            = "8b0a79306c9ce7ed99dae4f87f8dd61636",
    .p_ciphertext           = "02083e3979da014812f59f11d52630da30",
    .p_key                  = "7c77d6e813bed5ac98baa417477a2e7d",
    .p_ad                   = "65d2017990d62528",
    .p_nonce                = "1a8c98dcd73d38393b2bf1569deefc19",
    .p_mac                  = "137327d10649b0aa6e1c181db617d7f2"
};

// AES EAX - The EAX Mode of Operation 128 - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_encrypt7) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 128 Encrypt message_len=18 ad_len=8 mac_len=16 nonce_len=16 variant 1",
    .p_plaintext            = "1bda122bce8a8dbaf1877d962b8592dd2d56",
    .p_ciphertext           = "2ec47b2c4954a489afc7ba4897edcdae8cc3",
    .p_key                  = "5fff20cafab119ca2fc73549e20f5b0d",
    .p_ad                   = "54b9f04e6a09189a",
    .p_nonce                = "dde59b97d722156d4d9aff2bc7559826",
    .p_mac                  = "3b60450599bd02c96382902aef7f832a"
};

// AES EAX - The EAX Mode of Operation 128 - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_encrypt8) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 128 Encrypt message_len=18 ad_len=8 mac_len=16 nonce_len=16 variant 2",
    .p_plaintext            = "6cf36720872b8513f6eab1a8a44438d5ef11",
    .p_ciphertext           = "0de18fd0fdd91e7af19f1d8ee8733938b1e8",
    .p_key                  = "a4a4782bcffd3ec5e7ef6d8c34a56123",
    .p_ad                   = "899a175897561d7e",
    .p_nonce                = "b781fcf2f75fa5a8de97a9ca48e522ec",
    .p_mac                  = "e7f6d2231618102fdb7fe55ff1991700"
};

// AES EAX - The EAX Mode of Operation 128 - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_encrypt9) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 128 Encrypt message_len=21 ad_len=8 mac_len=16 nonce_len=16",
    .p_plaintext            = "ca40d7446e545ffaed3bd12a740a659ffbbb3ceab7",
    .p_ciphertext           = "cb8920f87a6c75cff39627b56e3ed197c552d295a7",
    .p_key                  = "8395fcf1e95bebd697bd010bc766aac3",
    .p_ad                   = "126735fcc320d25a",
    .p_nonce                = "22e7add93cfc6393c57ec0b3c17d6b44",
    .p_mac                  = "cfc46afc253b4652b1af3795b124ab6e"
};

// AES EAX - The EAX Mode of Operation 128 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_decrypt0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 128 Decrypt message_len=0 ad_len=8 mac_len=16 nonce_len=16",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "233952dee4d5ed5f9b9c6d6ff80ff478",
    .p_ad                   = "6bfb914fd07eae6b",
    .p_nonce                = "62ec67f9c3a4a407fcb2a8c49031a8b3",
    .p_mac                  = "e037830e8389f27b025a2d6527e79d01"
};

// AES EAX - The EAX Mode of Operation 128 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_decrypt1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 128 Decrypt message_len=2 ad_len=8 mac_len=16 nonce_len=16",
    .p_plaintext            = "f7fb",
    .p_ciphertext           = "19dd",
    .p_key                  = "91945d3f4dcbee0bf45ef52255f095a4",
    .p_ad                   = "fa3bfd4806eb53fa",
    .p_nonce                = "becaf043b0a23d843194ba972c66debd",
    .p_mac                  = "5c4c9331049d0bdab0277408f67967e5"
};

// AES EAX - The EAX Mode of Operation 128 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_decrypt2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 128 Decrypt message_len=5 ad_len=8 mac_len=16 nonce_len=16 variant 1",
    .p_plaintext            = "1a47cb4933",
    .p_ciphertext           = "d851d5bae0",
    .p_key                  = "01f74ad64077f2e704c0f60ada3dd523",
    .p_ad                   = "234a3463c1264ac6",
    .p_nonce                = "70c3db4f0d26368400a10ed05d2bff5e",
    .p_mac                  = "3a59f238a23e39199dc9266626c40f80"
};

// AES EAX - The EAX Mode of Operation 128 - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_decrypt3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 128 Decrypt message_len=5 ad_len=8 mac_len=16 nonce_len=16 variant 2",
    .p_plaintext            = "481c9e39b1",
    .p_ciphertext           = "632a9d131a",
    .p_key                  = "d07cf6cbb7f313bdde66b727afd3c5e8",
    .p_ad                   = "33cce2eabff5a79d",
    .p_nonce                = "8408dfff3c1a2b1292dc199e46b7d617",
    .p_mac                  = "d4c168a4225d8e1ff755939974a7bede"
};

// AES EAX - The EAX Mode of Operation 128 - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_decrypt4) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 128 Decrypt message_len=6 ad_len=8 mac_len=16 nonce_len=16",
    .p_plaintext            = "40d0c07da5e4",
    .p_ciphertext           = "071dfe16c675",
    .p_key                  = "35b6d0580005bbc12b0587124557d2c2",
    .p_ad                   = "aeb96eaebe2970e9",
    .p_nonce                = "fdb6b06676eedc5c61d74276e1f8e816",
    .p_mac                  = "cb0677e536f73afe6a14b74ee49844dd"
};

// AES EAX - The EAX Mode of Operation 128 - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_decrypt5) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 128 Decrypt message_len=12 ad_len=8 mac_len=16 nonce_len=16",
    .p_plaintext            = "4de3b35c3fc039245bd1fb7d",
    .p_ciphertext           = "835bb4f15d743e350e728414",
    .p_key                  = "bd8e6e11475e60b268784c38c62feb22",
    .p_ad                   = "d4482d1ca78dce0f",
    .p_nonce                = "6eac5c93072d8e8513f750935e46da1b",
    .p_mac                  = "abb8644fd6ccb86947c5e10590210a4f"
};

// AES EAX - The EAX Mode of Operation 128 - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_decrypt6) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 128 Decrypt message_len=17 ad_len=8 mac_len=16 nonce_len=16",
    .p_plaintext            = "8b0a79306c9ce7ed99dae4f87f8dd61636",
    .p_ciphertext           = "02083e3979da014812f59f11d52630da30",
    .p_key                  = "7c77d6e813bed5ac98baa417477a2e7d",
    .p_ad                   = "65d2017990d62528",
    .p_nonce                = "1a8c98dcd73d38393b2bf1569deefc19",
    .p_mac                  = "137327d10649b0aa6e1c181db617d7f2"
};

// AES EAX - The EAX Mode of Operation 128 - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_decrypt7) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 128 Decrypt message_len=18 ad_len=8 mac_len=16 nonce_len=16 variant 1",
    .p_plaintext            = "1bda122bce8a8dbaf1877d962b8592dd2d56",
    .p_ciphertext           = "2ec47b2c4954a489afc7ba4897edcdae8cc3",
    .p_key                  = "5fff20cafab119ca2fc73549e20f5b0d",
    .p_ad                   = "54b9f04e6a09189a",
    .p_nonce                = "dde59b97d722156d4d9aff2bc7559826",
    .p_mac                  = "3b60450599bd02c96382902aef7f832a"
};

// AES EAX - The EAX Mode of Operation 128 - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_decrypt8) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 128 Decrypt message_len=18 ad_len=8 mac_len=16 nonce_len=16 variant 2",
    .p_plaintext            = "6cf36720872b8513f6eab1a8a44438d5ef11",
    .p_ciphertext           = "0de18fd0fdd91e7af19f1d8ee8733938b1e8",
    .p_key                  = "a4a4782bcffd3ec5e7ef6d8c34a56123",
    .p_ad                   = "899a175897561d7e",
    .p_nonce                = "b781fcf2f75fa5a8de97a9ca48e522ec",
    .p_mac                  = "e7f6d2231618102fdb7fe55ff1991700"
};

// AES EAX - The EAX Mode of Operation 128 - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_128_decrypt9) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 128 Decrypt message_len=21 ad_len=8 mac_len=16 nonce_len=16",
    .p_plaintext            = "ca40d7446e545ffaed3bd12a740a659ffbbb3ceab7",
    .p_ciphertext           = "cb8920f87a6c75cff39627b56e3ed197c552d295a7",
    .p_key                  = "8395fcf1e95bebd697bd010bc766aac3",
    .p_ad                   = "126735fcc320d25a",
    .p_nonce                = "22e7add93cfc6393c57ec0b3c17d6b44",
    .p_mac                  = "cfc46afc253b4652B1AF3795B124AB6E"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_EAX_128)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_EAX_192)

// AES EAX - 192 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_192_encrypt0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 192 Encrypt message_len=1 ad_len=0 mac_len=4 nonce_len=1",
    .p_plaintext            = "2a",
    .p_ciphertext           = "54",
    .p_key                  = "acf5aff85f3ecfce0e7c2b3df61ce487c38bf31ce0d7bc13",
    .p_ad                   = "",
    .p_nonce                = "8b",
    .p_mac                  = "0fa31df6"
};

// AES EAX - 192 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_192_encrypt1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 192 Encrypt message_len=0 ad_len=20 mac_len=4 nonce_len=1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "acf5aff85f3ecfce0e7c2b3df61ce487c38bf31ce0d7bc13",
    .p_ad                   = "38b759a2375e8036502c27b58893eb9b85f47c35",
    .p_nonce                = "8b",
    .p_mac                  = "4b4c93b9"
};

// AES EAX - 192 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_192_encrypt2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 192 Encrypt message_len=1 ad_len=20 mac_len=4 nonce_len=1",
    .p_plaintext            = "2a",
    .p_ciphertext           = "54",
    .p_key                  = "acf5aff85f3ecfce0e7c2b3df61ce487c38bf31ce0d7bc13",
    .p_ad                   = "38b759a2375e8036502c27b58893eb9b85f47c35",
    .p_nonce                = "8b",
    .p_mac                  = "1f9d8745"
};

// AES EAX - 192 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_192_encrypt3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 192 Encrypt message_len=255 ad_len=255 mac_len=16 nonce_len=16",
    .p_plaintext            = "7e61def9f2192a90034d6a5cf1045cc374a83e5edca6a3d23b51c7cb13ce4d0a67e08937d368e0ccfc520cbd3977d1f019a37c4fe25a94ed1381f63fcb81ff17e5c2102cbcab4ee088f7da0264d8f500ed1b32103c1cbe7a1ce369f1e265e73d5feadb351179cdfb3d917cddf42bde16bc91c77ff3628d3fc034438e73d8b45a802ff872be9dd555499ba0331b4838d27629a769e36efd873ce0b704c25b927f34f43391c4f58a281be789c1afa78d89d3f5e13d39dad8b6b75f27a4a991043cc86fa2380dad9665e441b62a25822970e1a944e87a948c40579bacf4bc920dd9acee204ef3c9dbe5e09ee9845b2e325680fb949c8289438e0c48bcf28ce87d",
    .p_ciphertext           = "edb219e5be2931712c649e4f19bce36e5dbeb20b0ab481a51ce5322682d8db84254691020879475940bc63e8b90165649bd62f27ae9e94e2cbe021fd7326e700fef37d8c231f64d80855b75bb0b0b8b0d448062429a456d6971ae26e8e18ffceacb1eff103aa14575731e198d1322fdf80a51ce89affc0d851da0da314989cac738ca91764022ef2aa83f15954d4178b6e2e2c33c52411b79e009fc74464b89f17c2b2a4d950c4a1040748d08b2a4e9728ca5c936eab5087350aa9dedf9d2a377fa7c4663007f81d1b362b9b0707e0873eaf7ed989345ea7d16bb8f08ba8209923aef13b35ca7b4809b9b54fb23d2583aacd75781e9b1f1ef53cc41d0afcb8",
    .p_key                  = "acf5aff85f3ecfce0e7c2b3df61ce487c38bf31ce0d7bc13",
    .p_ad                   = "c3a3a82d25b7d6cd060985c09f02b16b10693bb4cc536052a280f0d2a4904d3e95da5a3389f92fd3b8fe20f000b926819b05edfb901f33ed75b8aae146da9f18c90bbaf1845fbb1263d7d89d0f871c899152fb5ef5b52562ba5c4e183e22ad0d416f75cef49e641ff3e5b6de8d18ca0fd9d34a9580f4abdd8b07354c232c4305c7d2bbdedf4f318579c8a76a877961bb8f52b4943351158d062e92d0a9c592f8e3776ffcbf963ec1b0e4ecc0d30a7f8a4b50b42a14d144d8a89706cdb10f7005dfc6915b489f9638e7eed492819447a5ddb321fa4080216a58fdbf8dda968b16fff7353d669aec1eb23105ecec9d511cab89b5ff16fb744b92a44ce19b5bcf",
    .p_nonce                = "82cb43c4e79279a5addc07a5667e751f",
    .p_mac                  = "92abb31d43405889a8411729d43d105a"
};

// AES EAX - 192 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_192_c_decrypt0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 192 Decrypt message_len=1 ad_len=0 mac_len=4 nonce_len=1",
    .p_plaintext            = "2a",
    .p_ciphertext           = "54",
    .p_key                  = "acf5aff85f3ecfce0e7c2b3df61ce487c38bf31ce0d7bc13",
    .p_ad                   = "",
    .p_nonce                = "8b",
    .p_mac                  = "0fa31df6"
};

// AES EAX - 192 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_192_c_decrypt1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 192 Decrypt message_len=0 ad_len=20 mac_len=4 nonce_len=1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "acf5aff85f3ecfce0e7c2b3df61ce487c38bf31ce0d7bc13",
    .p_ad                   = "38b759a2375e8036502c27b58893eb9b85f47c35",
    .p_nonce                = "8b",
    .p_mac                  = "4b4c93b9"
};

// AES EAX - 192 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_192_c_decrypt2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 192 Decrypt message_len=1 ad_len=20 mac_len=4 nonce_len=1",
    .p_plaintext            = "2a",
    .p_ciphertext           = "54",
    .p_key                  = "acf5aff85f3ecfce0e7c2b3df61ce487c38bf31ce0d7bc13",
    .p_ad                   = "38b759a2375e8036502c27b58893eb9b85f47c35",
    .p_nonce                = "8b",
    .p_mac                  = "1f9d8745"
};

// AES EAX - 192 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_192_c_decrypt3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 192 Decrypt message_len=255 ad_len=255 mac_len=16 nonce_len=16",
    .p_plaintext            = "7e61def9f2192a90034d6a5cf1045cc374a83e5edca6a3d23b51c7cb13ce4d0a67e08937d368e0ccfc520cbd3977d1f019a37c4fe25a94ed1381f63fcb81ff17e5c2102cbcab4ee088f7da0264d8f500ed1b32103c1cbe7a1ce369f1e265e73d5feadb351179cdfb3d917cddf42bde16bc91c77ff3628d3fc034438e73d8b45a802ff872be9dd555499ba0331b4838d27629a769e36efd873ce0b704c25b927f34f43391c4f58a281be789c1afa78d89d3f5e13d39dad8b6b75f27a4a991043cc86fa2380dad9665e441b62a25822970e1a944e87a948c40579bacf4bc920dd9acee204ef3c9dbe5e09ee9845b2e325680fb949c8289438e0c48bcf28ce87d",
    .p_ciphertext           = "edb219e5be2931712c649e4f19bce36e5dbeb20b0ab481a51ce5322682d8db84254691020879475940bc63e8b90165649bd62f27ae9e94e2cbe021fd7326e700fef37d8c231f64d80855b75bb0b0b8b0d448062429a456d6971ae26e8e18ffceacb1eff103aa14575731e198d1322fdf80a51ce89affc0d851da0da314989cac738ca91764022ef2aa83f15954d4178b6e2e2c33c52411b79e009fc74464b89f17c2b2a4d950c4a1040748d08b2a4e9728ca5c936eab5087350aa9dedf9d2a377fa7c4663007f81d1b362b9b0707e0873eaf7ed989345ea7d16bb8f08ba8209923aef13b35ca7b4809b9b54fb23d2583aacd75781e9b1f1ef53cc41d0afcb8",
    .p_key                  = "acf5aff85f3ecfce0e7c2b3df61ce487c38bf31ce0d7bc13",
    .p_ad                   = "c3a3a82d25b7d6cd060985c09f02b16b10693bb4cc536052a280f0d2a4904d3e95da5a3389f92fd3b8fe20f000b926819b05edfb901f33ed75b8aae146da9f18c90bbaf1845fbb1263d7d89d0f871c899152fb5ef5b52562ba5c4e183e22ad0d416f75cef49e641ff3e5b6de8d18ca0fd9d34a9580f4abdd8b07354c232c4305c7d2bbdedf4f318579c8a76a877961bb8f52b4943351158d062e92d0a9c592f8e3776ffcbf963ec1b0e4ecc0d30a7f8a4b50b42a14d144d8a89706cdb10f7005dfc6915b489f9638e7eed492819447a5ddb321fa4080216a58fdbf8dda968b16fff7353d669aec1eb23105ecec9d511cab89b5ff16fb744b92a44ce19b5bcf",
    .p_nonce                = "82cb43c4e79279a5addc07a5667e751f",
    .p_mac                  = "92abb31d43405889a8411729d43d105a"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_EAX_192)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_EAX_256)

// AES EAX - 256 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_256_encrypt0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 256 Encrypt message_len=1 ad_len=0 mac_len=4 nonce_len=1",
    .p_plaintext            = "2a",
    .p_ciphertext           = "67",
    .p_key                  = "388d601c4bcd7487ba0ea5973dc834a1e85522ef6685ecedb6a7a5fd7a48ecf2",
    .p_ad                   = "",
    .p_nonce                = "8b",
    .p_mac                  = "d57ea094"
};

// AES EAX - 256 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_256_encrypt1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 256 Encrypt message_len=0 ad_len=20 mac_len=4 nonce_len=1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "388d601c4bcd7487ba0ea5973dc834a1e85522ef6685ecedb6a7a5fd7a48ecf2",
    .p_ad                   = "38b759a2375e8036502c27b58893eb9b85f47c35",
    .p_nonce                = "8b",
    .p_mac                  = "4b99315e"
};

// AES EAX - 256 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_256_encrypt2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 256 Encrypt message_len=1 ad_len=20 mac_len=4 nonce_len=1",
    .p_plaintext            = "2a",
    .p_ciphertext           = "67",
    .p_key                  = "388d601c4bcd7487ba0ea5973dc834a1e85522ef6685ecedb6a7a5fd7a48ecf2",
    .p_ad                   = "38b759a2375e8036502c27b58893eb9b85f47c35",
    .p_nonce                = "8b",
    .p_mac                  = "511d1924"
};

// AES EAX - 256 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_256_encrypt3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "EAX 256 Encrypt message_len=255 ad_len=255 mac_len=16 nonce_len=16",
    .p_plaintext            = "7e61def9f2192a90034d6a5cf1045cc374a83e5edca6a3d23b51c7cb13ce4d0a67e08937d368e0ccfc520cbd3977d1f019a37c4fe25a94ed1381f63fcb81ff17e5c2102cbcab4ee088f7da0264d8f500ed1b32103c1cbe7a1ce369f1e265e73d5feadb351179cdfb3d917cddf42bde16bc91c77ff3628d3fc034438e73d8b45a802ff872be9dd555499ba0331b4838d27629a769e36efd873ce0b704c25b927f34f43391c4f58a281be789c1afa78d89d3f5e13d39dad8b6b75f27a4a991043cc86fa2380dad9665e441b62a25822970e1a944e87a948c40579bacf4bc920dd9acee204ef3c9dbe5e09ee9845b2e325680fb949c8289438e0c48bcf28ce87d",
    .p_ciphertext           = "0aa27ff480ddfd2f976c5ca9de1557bc62623e42c4a185020f1dafa129b16d91a02340aed8bb5cdbeacfbda4cfafa12392468cef1abcca3d3a15488b9b64478a87a6867b506772d66c96f82e1f684aa9eb4e103d3edff8ddafd954882b4d5532d3a85343fc138f390cfd85f4619f92238678452249b720ac3fb124d686c342e462500adf19b9e9a5dbe1f866b8a607a65615366d77b19fab249f201d21a8d10886aea224b1abc812312f8a97f1d407bd90d755960b81922ee62be22579c07150ebc172d1ae2e0e9101303435008f4e0fd1fb387a292c5189178c6b9e8bd47103f5400e5daa0ab3231ac6597530f391484d38dd859e433fdc27b8494d3ab7a4",
    .p_key                  = "388d601c4bcd7487ba0ea5973dc834a1e85522ef6685ecedb6a7a5fd7a48ecf2",
    .p_ad                   = "c3a3a82d25b7d6cd060985c09f02b16b10693bb4cc536052a280f0d2a4904d3e95da5a3389f92fd3b8fe20f000b926819b05edfb901f33ed75b8aae146da9f18c90bbaf1845fbb1263d7d89d0f871c899152fb5ef5b52562ba5c4e183e22ad0d416f75cef49e641ff3e5b6de8d18ca0fd9d34a9580f4abdd8b07354c232c4305c7d2bbdedf4f318579c8a76a877961bb8f52b4943351158d062e92d0a9c592f8e3776ffcbf963ec1b0e4ecc0d30a7f8a4b50b42a14d144d8a89706cdb10f7005dfc6915b489f9638e7eed492819447a5ddb321fa4080216a58fdbf8dda968b16fff7353d669aec1eb23105ecec9d511cab89b5ff16fb744b92a44ce19b5bcf",
    .p_nonce                = "82cb43c4e79279a5addc07a5667e751f",
    .p_mac                  = "fa4cffc7a78497bf41c99bf90db59161"
};

// AES EAX - 256 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_256_c_decrypt0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 256 Decrypt message_len=1 ad_len=0 mac_len=4 nonce_len=1",
    .p_plaintext            = "2a",
    .p_ciphertext           = "67",
    .p_key                  = "388d601c4bcd7487ba0ea5973dc834a1e85522ef6685ecedb6a7a5fd7a48ecf2",
    .p_ad                   = "",
    .p_nonce                = "8b",
    .p_mac                  = "d57ea094"
};

// AES EAX - 256 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_256_c_decrypt1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 256 Decrypt message_len=0 ad_len=20 mac_len=4 nonce_len=1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "388d601c4bcd7487ba0ea5973dc834a1e85522ef6685ecedb6a7a5fd7a48ecf2",
    .p_ad                   = "38b759a2375e8036502c27b58893eb9b85f47c35",
    .p_nonce                = "8b",
    .p_mac                  = "4b99315e"
};

// AES EAX - 256 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_256_c_decrypt2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 256 Decrypt message_len=1 ad_len=20 mac_len=4 nonce_len=1",
    .p_plaintext            = "2a",
    .p_ciphertext           = "67",
    .p_key                  = "388d601c4bcd7487ba0ea5973dc834a1e85522ef6685ecedb6a7a5fd7a48ecf2",
    .p_ad                   = "38b759a2375e8036502c27b58893eb9b85f47c35",
    .p_nonce                = "8b",
    .p_mac                  = "511d1924"
};

// AES EAX - 256 bits custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_eax_256_c_decrypt3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_eax_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "EAX 256 Decrypt message_len=255 ad_len=255 mac_len=16 nonce_len=16",
    .p_plaintext            = "7e61def9f2192a90034d6a5cf1045cc374a83e5edca6a3d23b51c7cb13ce4d0a67e08937d368e0ccfc520cbd3977d1f019a37c4fe25a94ed1381f63fcb81ff17e5c2102cbcab4ee088f7da0264d8f500ed1b32103c1cbe7a1ce369f1e265e73d5feadb351179cdfb3d917cddf42bde16bc91c77ff3628d3fc034438e73d8b45a802ff872be9dd555499ba0331b4838d27629a769e36efd873ce0b704c25b927f34f43391c4f58a281be789c1afa78d89d3f5e13d39dad8b6b75f27a4a991043cc86fa2380dad9665e441b62a25822970e1a944e87a948c40579bacf4bc920dd9acee204ef3c9dbe5e09ee9845b2e325680fb949c8289438e0c48bcf28ce87d",
    .p_ciphertext           = "0aa27ff480ddfd2f976c5ca9de1557bc62623e42c4a185020f1dafa129b16d91a02340aed8bb5cdbeacfbda4cfafa12392468cef1abcca3d3a15488b9b64478a87a6867b506772d66c96f82e1f684aa9eb4e103d3edff8ddafd954882b4d5532d3a85343fc138f390cfd85f4619f92238678452249b720ac3fb124d686c342e462500adf19b9e9a5dbe1f866b8a607a65615366d77b19fab249f201d21a8d10886aea224b1abc812312f8a97f1d407bd90d755960b81922ee62be22579c07150ebc172d1ae2e0e9101303435008f4e0fd1fb387a292c5189178c6b9e8bd47103f5400e5daa0ab3231ac6597530f391484d38dd859e433fdc27b8494d3ab7a4",
    .p_key                  = "388d601c4bcd7487ba0ea5973dc834a1e85522ef6685ecedb6a7a5fd7a48ecf2",
    .p_ad                   = "c3a3a82d25b7d6cd060985c09f02b16b10693bb4cc536052a280f0d2a4904d3e95da5a3389f92fd3b8fe20f000b926819b05edfb901f33ed75b8aae146da9f18c90bbaf1845fbb1263d7d89d0f871c899152fb5ef5b52562ba5c4e183e22ad0d416f75cef49e641ff3e5b6de8d18ca0fd9d34a9580f4abdd8b07354c232c4305c7d2bbdedf4f318579c8a76a877961bb8f52b4943351158d062e92d0a9c592f8e3776ffcbf963ec1b0e4ecc0d30a7f8a4b50b42a14d144d8a89706cdb10f7005dfc6915b489f9638e7eed492819447a5ddb321fa4080216a58fdbf8dda968b16fff7353d669aec1eb23105ecec9d511cab89b5ff16fb744b92a44ce19b5bcf",
    .p_nonce                = "82cb43c4e79279a5addc07a5667e751f",
    .p_mac                  = "fa4cffc7a78497bf41c99bf90db59161"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_EAX_256)

/*lint -restore */

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_EAX)
