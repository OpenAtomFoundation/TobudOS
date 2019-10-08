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

/**@brief AES CBC-MAC is tested using only custom generated test vectors.
 *
 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_MAC)

/*lint -save -e91 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_MAC_128)

// AES CBC MAC - Custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cbc_mac_128_1) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_mac_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_FAIL,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC MAC 128 message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = "2b7e151628aed2a6abf7158809cf4f3c",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC MAC - Custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cbc_mac_128_2) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_mac_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC MAC 128 message_len=16",
    .p_plaintext        = "6bc1bee22e409f96e93d7e117393172a",
    .p_ciphertext       = "3ad77bb40d7a3660a89ecaf32466ef97",
    .p_key              = "2b7e151628aed2a6abf7158809cf4f3c",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC MAC - Custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cbc_mac_128_3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_mac_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC MAC 128 message_len=32",
    .p_plaintext        = "d602b63eebba5b8fe1db84d8ca71abf5023e147508ce206c9732a28cc94eaabc",
    .p_ciphertext       = "5a88111a1e75ccc0ebad8b7b74e1c6d1",
    .p_key              = "2b7e151628aed2a6abf7158809cf4f3c",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC MAC - Custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cbc_mac_128_4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_mac_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC MAC 128 message_len=64",
    .p_plaintext        = "93cccff91971303929c53d0de3dd7a96851e54fe1b484d240cae8ff2b99051766f4d6f7b500d26e2e43295bd4c6313bca988875944215d8de20298e3bb795d9d",
    .p_ciphertext       = "289df89c9703958b37b1b9b1f7842984",
    .p_key              = "2b7e151628aed2a6abf7158809cf4f3c",
    .p_iv               = "00000000000000000000000000000000"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_MAC_128)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_MAC_192)

// AES CBC MAC - Custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cbc_mac_192_1) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_mac_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_FAIL,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC MAC 192 message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = "291dc8f04aabad1d63b9820389329e2b4db30bc94264f677",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC MAC - Custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cbc_mac_192_2) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_mac_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC MAC 192 message_len=16",
    .p_plaintext        = "6bc1bee22e409f96e93d7e117393172a",
    .p_ciphertext       = "9cd89b028e16378d71ac45fdc45b7b08",
    .p_key              = "291dc8f04aabad1d63b9820389329e2b4db30bc94264f677",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC MAC - Custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cbc_mac_192_3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_mac_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC MAC 192 message_len=32",
    .p_plaintext        = "d602b63eebba5b8fe1db84d8ca71abf5023e147508ce206c9732a28cc94eaabc",
    .p_ciphertext       = "098f08d19aef1c6bf63f38c5aa2b9b31",
    .p_key              = "291dc8f04aabad1d63b9820389329e2b4db30bc94264f677",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC MAC - Custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cbc_mac_192_4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_mac_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC MAC 192 message_len=64",
    .p_plaintext        = "93cccff91971303929c53d0de3dd7a96851e54fe1b484d240cae8ff2b99051766f4d6f7b500d26e2e43295bd4c6313bca988875944215d8de20298e3bb795d9d",
    .p_ciphertext       = "5e0eb2751c73e2b7de96c302caed1459",
    .p_key              = "291dc8f04aabad1d63b9820389329e2b4db30bc94264f677",
    .p_iv               = "00000000000000000000000000000000"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_MAC_192)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_MAC_256)

// AES CBC MAC - Custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cbc_mac_256_1) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_mac_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_FAIL,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC MAC 256 message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC MAC - Custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cbc_mac_256_2) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_mac_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC MAC 256 message_len=16",
    .p_plaintext        = "6bc1bee22e409f96e93d7e117393172a",
    .p_ciphertext       = "f3eed1bdb5d2a03c064b5a7e3db181f8",
    .p_key              = "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC MAC - Custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cbc_mac_256_3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_mac_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC MAC 256 message_len=32",
    .p_plaintext        = "d602b63eebba5b8fe1db84d8ca71abf5023e147508ce206c9732a28cc94eaabc",
    .p_ciphertext       = "b0d33b64ae39d12fdd26cb39657b9047",
    .p_key              = "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC MAC - Custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cbc_mac_256_4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_mac_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC MAC 256 message_len=64",
    .p_plaintext        = "93cccff91971303929c53d0de3dd7a96851e54fe1b484d240cae8ff2b99051766f4d6f7b500d26e2e43295bd4c6313bca988875944215d8de20298e3bb795d9d",
    .p_ciphertext       = "8dbe503b77ebed416cd0ad049314aaa4",
    .p_key              = "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
    .p_iv               = "00000000000000000000000000000000"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_MAC_256)

/*lint -restore */

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_MAC)
