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

/**@brief CMAC test vectors can be found on NIST web pages.
 *
 * https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/CAVP-TESTING-BLOCK-CIPHER-MODES#CMAC
 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CMAC)

/*lint -save -e91 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CMAC_128)

// AES CMAC - NIST SP 800-38B, CMAC-AES128, Example #1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cmac_128_1) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cmac_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_FAIL,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CMAC 128 message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = "2b7e151628aed2a6abf7158809cf4f3c"
};

// AES CMAC - NIST SP 800-38B, CMAC-AES128, Example #2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cmac_128_2) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cmac_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CMAC 128 message_len=16",
    .p_plaintext        = "6bc1bee22e409f96e93d7e117393172a",
    .p_ciphertext       = "070a16b46b4d4144f79bdd9dd04a287c",
    .p_key              = "2b7e151628aed2a6abf7158809cf4f3c"
};

// AES CMAC - NIST SP 800-38B, CMAC-AES128, Example #3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cmac_128_3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cmac_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CMAC 128 message_len=20",
    .p_plaintext        = "6bc1bee22e409f96e93d7e117393172aae2d8a57",
    .p_ciphertext       = "7d85449ea6ea19c823a7bf78837dfade",
    .p_key              = "2b7e151628aed2a6abf7158809cf4f3c"
};

// AES CMAC - NIST SP 800-38B, CMAC-AES128, Example #4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cmac_128_4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cmac_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CMAC 128 message_len=64",
    .p_plaintext        = "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710",
    .p_ciphertext       = "51f0bebf7e3b9d92fc49741779363cfe",
    .p_key              = "2b7e151628aed2a6abf7158809cf4f3c"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CMAC_128)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CMAC_192)

// AES CMAC - NIST SP 800-38B, CMAC-AES192, Example #1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cmac_192_1) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cmac_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_FAIL,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CMAC 192 message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b"
};

// AES CMAC - NIST SP 800-38B, CMAC-AES192, Example #2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cmac_192_2) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cmac_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CMAC 192 message_len=16",
    .p_plaintext        = "6bc1bee22e409f96e93d7e117393172a",
    .p_ciphertext       = "9e99a7bf31e710900662f65e617c5184",
    .p_key              = "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b"
};

// AES CMAC - NIST SP 800-38B, CMAC-AES192, Example #3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cmac_192_3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cmac_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CMAC 192 message_len=20",
    .p_plaintext        = "6bc1bee22e409f96e93d7e117393172aae2d8a57",
    .p_ciphertext       = "3d75c194ed96070444a9fa7ec740ecf8",
    .p_key              = "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b"
};

// AES CMAC - NIST SP 800-38B, CMAC-AES192, Example #4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cmac_192_4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cmac_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CMAC 192 message_len=64",
    .p_plaintext        = "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710",
    .p_ciphertext       = "a1d5df0eed790f794d77589659f39a11",
    .p_key              = "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CMAC_192)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CMAC_256)

// AES CMAC - NIST SP 800-38B, CMAC-AES256, Example #1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cmac_256_1) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cmac_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_FAIL,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CMAC 256 message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4"
};

// AES CMAC - NIST SP 800-38B, CMAC-AES256, Example #2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cmac_256_2) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cmac_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CMAC 256 message_len=16",
    .p_plaintext        = "6bc1bee22e409f96e93d7e117393172a",
    .p_ciphertext       = "28a7023f452e8f82bd4bf28d8c37c35c",
    .p_key              = "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4"
};

// AES CMAC - NIST SP 800-38B, CMAC-AES256, Example #3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cmac_256_3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cmac_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CMAC 256 message_len=20",
    .p_plaintext        = "6bc1bee22e409f96e93d7e117393172aae2d8a57",
    .p_ciphertext       = "156727dc0878944a023c1fe03bad6d93",
    .p_key              = "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4"
};

// AES CMAC - NIST SP 800-38B, CMAC-AES256, Example #4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_mac_data, test_vector_aes_t test_vector_aes_cmac_256_4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cmac_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CMAC 256 message_len=64",
    .p_plaintext        = "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710",
    .p_ciphertext       = "e1992190549f6ed5696a2c056c315410",
    .p_key              = "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CMAC_256)

/*lint -restore */

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CMAC)
