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

/**@brief GCM test vectors can be found on NIST web pages.
 *
 * https://csrc.nist.gov
 * https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Algorithm-Validation-Program/documents/mac/gcmtestvectors.zip
 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_GCM)

/*lint -save -e91 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_GCM_128)

// AES GCM - Custom test vector 1 - Invalid behavior test for AES plaintext and AD.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_gcm_128_inv_c1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 message_len=0 ad_len=0",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "",
    .p_nonce                = "43925318850a09c9fb23247cbd",
    .p_mac                  = "61e84f784ac1975b"
};

// AES GCM - Custom test vector 2 - Invalid behavior test for AES buffer authenticated decryption.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_gcm_128_inv_c2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt Invalid ciphertext",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "fbcd7ef1f0c1f41c7187ec6bd407c3f3322c5b3a783b14",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "43925318850a09c9fb23247cbd",
    .p_mac                  = "ca4deff6d149528c"
};

// AES GCM - Custom test vector 3 - Invalid behavior test for AES buffer authenticated decryption.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_gcm_128_inv_c3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
    .crypt_expected_result  = EXPECTED_TO_FAIL,  // Generated plaintext will be incorrect.
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
    .crypt_expected_result  = EXPECTED_TO_PASS,  // Generated plaintext will be correct.
#endif
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt Invalid MAC",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "ebcd7ef1f0c1f41c7187ec6bd407c3f3322c5b3a783b14",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "43925318850a09c9fb23247cbd",
    .p_mac                  = "da4deff6d149528c"
};

// AES GCM - Custom test vector 4 - Invalid behavior test for AES key.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_gcm_128_inv_c4) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt with invalid AES key",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "ebcd7ef1f0c1f41c7187ec6bd407c3f3322c5b3a783b14",
    .p_key                  = "e0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "43925318850a09c9fb23247cbd",
    .p_mac                  = "ca4deff6d149528c"
};

// AES GCM - Custom test vector 5 - Invalid behavior test for AES key.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_gcm_128_inv_c5) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt with invalid AES key",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "ebcd7ef1f0c1f41c7187ec6bd407c3f3322c5b3a783b14",
    .p_key                  = "e0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "43925318850a09c9fb23247cbd",
    .p_mac                  = "ca4deff6d149528c"
};

// AES GCM - Custom test vector 6 - Invalid behavior test for AES nonce.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_gcm_128_inv_c6) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt with invalid AES nonce",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "ebcd7ef1f0c1f41c7187ec6bd407c3f3322c5b3a783b14",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "53925318850a09c9fb23247cbd",
    .p_mac                  = "ca4deff6d149528c"
};

// AES GCM - Custom test vector 7 - Invalid behavior test for AES nonce.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_gcm_128_inv_c7) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt with invalid AES nonce",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "ebcd7ef1f0c1f41c7187ec6bd407c3f3322c5b3a783b14",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "53925318850a09c9fb23247cbd",
    .p_mac                  = "ca4deff6d149528c"
};

// AES GCM - Custom test vector 8
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_gcm_128_c8) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 message_len=250 ad_len=8 mac_len=8 nonce_len=13",
    .p_plaintext            = "e96325847ce207c2bf6638a52634075139998888fa0ca1501a4b57ee0767210f1fee065b7ed1e4005fe1c3ec482b2d3cad3d8461d4ab2ba1c73cbe2c65ac8b1e61be3f052ecadf671a3f58601ec0f1ac95367dae85601449f434b9d10757a613784a229aa14bebe6ed07579ec0a513ce7ce799d1e6cd468613f0d3f42e6162680b28f13aeb826572934424459137bd3aabec437bf4fdff007d2675d7fd2195280da092eb1b3549e5457e065cc47349f2c3013b8a4a1938b05c033b2a5b6b477be39b415f4f50beac41f817783bda28f24bab3c8095e1b01e48ce4271cd2a323d1d8c100bb9856f75f63bdee34510de01e32efc0b12e980e9161a",
    .p_ciphertext           = "0aa7517e802efdd1def0c6dde626d2b513acc9a99e2aab368c9a470281487a5dbdfbd3f44678144cda5f28a0c219424ff62068c3f6fa95e003c16459b8638df90f07250c4b7d2a2d45ae0337498688b7aed64489e6263df3933eb587eba90f382daf07915407d5a55d663437bd72dc7e6609329353c2dd8ccb21446500c2d04440af55eb0671effd3c51881e3b71997e340c470ca3366ae8f05617785eecf735b38520f176f789e1511b52282666e3e44372ef571d6b55c64f1fea7f528a5f1ca3d53710be9bad15968a6da93d2c77cbc9002ba3b343d3fdefd9f753f4fcb595242079ae83a6cb13d4a3d012d291f9faa6a08007d17fca800a8d",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "43925318850a09c9fb23247cbd",
    .p_mac                  = "7f53de1354ffae14"
};

// AES GCM - Custom test vector 9
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_gcm_128_c9) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 message_len=16 ad_len=250 mac_len=8 nonce_len=13",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "ebcd7ef1f0c1f41c7187ec6bd407c3f3322c5b3a783b14",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "0ee0089ec32c7cfa036bd1ab9c23b98cd5d166214d61c719688c6a0ecff705da47ac179cae7eaa3b638ab022a6669729fe47f24679e78488125d989cc7d1bbd8b472b04678b45adde4cca92cd91c4e214257243b163eca04bf643e9df6f7c5bf55aa91bc348154b5c3e9529a56fed8b04e0726b6de3ecc43f499108933e07297bdfcd4d8df2aa9af908083aeccc5c23fe9efbbf5fa9a27ea2923d36188197c87c955fcc5f7e0ebff4b01ebf008a5c93dab12ddfbf65f4225e5a3dfd44e36dbc3ba7891fce514d127e5eb8adb939670b4c5643c46425c614546b90cf0d720203bef4a12ff2d766623c92e134420e3b423b526598f32e8884843bc",
    .p_nonce                = "43925318850a09c9fb23247cbd",
    .p_mac                  = "38cec0517f5fa1d8"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_encrypt0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "cf063a34d4a9a76c2c86787d3f96db71",
    .p_ad                   = "",
    .p_nonce                = "113b9785971864c83b01c787",
    .p_mac                  = "72ac8493e3a5228b5d130a69d2510e42"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_encrypt1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "a49a5e26a2f8cb63d05546c2a62f5343",
    .p_ad                   = "",
    .p_nonce                = "907763b19b9b4ab6bd4f0281",
    .p_mac                  = "a2be08210d8c470a8df6e8fbd79ec5cf"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_encrypt2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "2ad0bf5aeb47a0c1a98da3dfdab4fded",
    .p_ad                   = "",
    .p_nonce                = "25f1b6091ee7040fea4ba854",
    .p_mac                  = "d7963d240317653e01cf5abe5d0966ae"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_encrypt3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=12 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "d1cb603b886e9deb62cbf5182caaeb35",
    .p_ad                   = "",
    .p_nonce                = "cd23f90cfd17727ea0975ff2",
    .p_mac                  = "e319af65"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_encrypt4) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=12 invalid 3",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "e626638a113c617077f5c4b2284921c5",
    .p_ad                   = "",
    .p_nonce                = "c4a8864b4c3385a78cb8759f",
    .p_mac                  = "039d51f8"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_encrypt5) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=12 valid 3",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "8f74a8b922fcf1a8eed7320df9f03fd6",
    .p_ad                   = "",
    .p_nonce                = "925ec6414e777062ac839f06",
    .p_mac                  = "bb0dc912"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_encrypt6) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt message_len=0 ad_len=16 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "d1f6af919cde85661208bdce0c27cb22",
    .p_ad                   = "7c5faa40e636bbc91107e68010c92b9f",
    .p_nonce                = "898c6929b435017bf031c3c5",
    .p_mac                  = "ae45f11777540a2caeb128be8092468a"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_encrypt7) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt message_len=0 ad_len=16 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "2370e320d4344208e0ff5683f243b213",
    .p_ad                   = "d43a8e5089eea0d026c03a85178b27da",
    .p_nonce                = "04dbb82f044d30831c441228",
    .p_mac                  = "2a049c049d25aa95969b451d93c31c6e"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_encrypt8) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt message_len=0 ad_len=16 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "bc3ab28150fd4cb731d5f48ed2784173",
    .p_ad                   = "53d2fc19963fc99a36a524e39cb68aa8",
    .p_nonce                = "40408445c203c647312b8f01",
    .p_mac                  = "3f04dbd2f49909f73044175041fd9eff"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_encrypt9) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt message_len=16 ad_len=0 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "28286a321293253c3e0aa2704a278032",
    .p_ciphertext           = "5a3c1cf1985dbb8bed818036fdd5ab42",
    .p_key                  = "e98b72a9881a84ca6b76e0f43e68647a",
    .p_ad                   = "",
    .p_nonce                = "8b23299fde174053f3d652ba",
    .p_mac                  = "23c7ab0f952b7091cd324835043b5eb5"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_encrypt10) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt message_len=16 ad_len=0 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "95695a5b12f2870b9cc5fdc8f218a97d",
    .p_ciphertext           = "47bb7e23f7bdfe05a8091ac90e4f8b2e",
    .p_key                  = "33240636cd3236165f1a553b773e728e",
    .p_ad                   = "",
    .p_nonce                = "17c4d61493ecdc8f31700b12",
    .p_mac                  = "b723c70e931d9785f40fd4ab1d612dc9"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_encrypt11) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt message_len=16 ad_len=0 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "95695a5b12f2870b9cc5fdc8f218a97d",
    .p_ciphertext           = "0216c899c88d6e32c958c7e553daa5bc",
    .p_key                  = "5164df856f1e9cac04a79b808dc5be39",
    .p_ad                   = "",
    .p_nonce                = "e76925d5355e0584ce871b2b",
    .p_mac                  = "a145319896329c96df291f64efbe0e3a"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_encrypt12) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt message_len=16 ad_len=16 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "ecafe96c67a1646744f1c891f5e69427",
    .p_ciphertext           = "552ebe012e7bcf90fcef712f8344e8f1",
    .p_key                  = "816e39070410cf2184904da03ea5075a",
    .p_ad                   = "f2a30728ed874ee02983c294435d3c16",
    .p_nonce                = "32c367a3362613b27fc3e67e",
    .p_mac                  = "ecaae9fc68276a45ab0ca3cb9dd9539f"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_encrypt13) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt message_len=16 ad_len=16 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "ecafe96c67a1646744f1c891f5e69427",
    .p_ciphertext           = "1c785025e5a2678e4b29b29276e395bb",
    .p_key                  = "867fc5d5476d5008f0703d81e3622255",
    .p_ad                   = "261a9efd4f32bc3d07c115b4edcf8adf",
    .p_nonce                = "22945529dff947c3c9264df7",
    .p_mac                  = "87fdf1261846164a950c37a3f2eea17d"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_encrypt14) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt message_len=16 ad_len=16 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "ec18a057c22d12373b5efe4d177eb068",
    .p_ciphertext           = "0128a239bb43c12885f9591386ecac0f",
    .p_key                  = "3d17f97bf1dae4268b6610dc90c70b28",
    .p_ad                   = "681a4feac147ee2d25e9191aaa4c8830",
    .p_nonce                = "ebcd88fc18d4c99d28524d41",
    .p_mac                  = "144def0210af9348f07afe27e65bdc7e"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_encrypt15) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt message_len=16 ad_len=90 mac_len=13 nonce_len=128 valid 1",
    .p_plaintext            = "1a964bfebe3908831558699d740bc46f",
    .p_ciphertext           = "55895bbe1f3bc758463a4a9355ef453f",
    .p_key                  = "dc5a53d122399fdfb1e5cc2dfca8900b",
    .p_ad                   = "2b7288f435248dc03dc0f3a2d5c82451bc34c1e319f23ce779d228d150bc8a69e82fb5062aa218d2a7810b39d551227b03e2e954179eac955be7f0a5b213f3a9f262a24f75c7631af3761cfa34adca7d100b9b17ade5c3b19af5",
    .p_nonce                = "fac4d131bc09ffb642a3e19f633f58a7bbc61949cea0ada34ab89df13e0e1b9080f968195782f0f765170c46476bec7b4d062c9714e5bb9cd63e4586d74e4a624ea875ef955b7614323a3f75af979423b4a39140c3a5edcef9ec9a158853a3fee6774e59c9863e1befaee60e2148639db953f4ab3b63e450529b0fccb3d3d134",
    .p_mac                  = "c11a27542917f024c973500da2"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_encrypt16) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt message_len=16 ad_len=90 mac_len=13 nonce_len=128 invalid 1",
    .p_plaintext            = "1a964bfebe3908831558699d740bc46f",
    .p_ciphertext           = "12559383ff63bfaf2863f612e3ee39bc",
    .p_key                  = "c40ee4fec74740982e2b07183b9b81bb",
    .p_ad                   = "3a00dfeb4b66254ce1294c0a18619ae98ab97dac1ba9f321b747979dba6594a5d6fd7d6cea0f7453e1abd65752b168e4c907e32b486396dcf4c2648d534b22e8e619cd390e370cfa7138ac967ff424f6e00fb887da79d6d96c53",
    .p_nonce                = "42c38dae820a700f91ec38ae901947074250f2d813286ed5ec6353f7cd84e1eaa4649e03cbfb2ea9604b05c3fdabf091681a1646b9cec137a2d46b31925ab6741cbd3cc02d1b623b142407472b73a85cf9feef076fea6a56965733f5b15712b97622d75bd818fd996cd1edb38ef1c5d0344169a165a87a5fb278f740ced141d9",
    .p_mac                  = "4412b1fdb1405c250fcfa8db40"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_encrypt17) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 128 Encrypt message_len=16 ad_len=90 mac_len=13 nonce_len=128 valid 2",
    .p_plaintext            = "f3e05222ea6133e1b867a53986e4b432",
    .p_ciphertext           = "5d5d86133086a689fed21cc37c856a6f",
    .p_key                  = "fef36ef7f9305ecbd990619f28b2b9b6",
    .p_ad                   = "5f4990dce32043db4143aa58aba99a3cfb745444cc1a282bcc47aa58cc7031b36fd25c7575ea585fffe09e60bdcb477381a35f3fa3ec09426f05c84136e24f9aeabe94f8117e8e2e9f7b3527877c0d9442587642c57c2be63415",
    .p_nonce                = "601ef18957f79ccf67cacfd801aaed57a66c7e666385aeb9456284c567c27e38190fe652d44e9affe538236f25939ab127418498f04feeca35b38fca44e3fd0b6f98b77425cc403e6edb9800cc5b343d6c01d9871cbc9a3b478447267aa5b6614ee8d25e141275c1f46f083ee93eaccb35040b22ceaed28f0f28cadb48d576a0",
    .p_mac                  = "bd273924ae7e3ef6aa3ee66dbf"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_decrypt0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "cf063a34d4a9a76c2c86787d3f96db71",
    .p_ad                   = "",
    .p_nonce                = "113b9785971864c83b01c787",
    .p_mac                  = "72ac8493e3a5228b5d130a69d2510e42"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_decrypt1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "a49a5e26a2f8cb63d05546c2a62f5343",
    .p_ad                   = "",
    .p_nonce                = "907763b19b9b4ab6bd4f0281",
    .p_mac                  = "a2be08210d8c470a8df6e8fbd79ec5cf"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_decrypt2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "2ad0bf5aeb47a0c1a98da3dfdab4fded",
    .p_ad                   = "",
    .p_nonce                = "25f1b6091ee7040fea4ba854",
    .p_mac                  = "d7963d240317653e01cf5abe5d0966ae"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_decrypt3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=12 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "d1cb603b886e9deb62cbf5182caaeb35",
    .p_ad                   = "",
    .p_nonce                = "cd23f90cfd17727ea0975ff2",
    .p_mac                  = "e319af65"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_decrypt4) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,

#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=12 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "e626638a113c617077f5c4b2284921c5",
    .p_ad                   = "",
    .p_nonce                = "c4a8864b4c3385a78cb8759f",
    .p_mac                  = "039d51f8"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_decrypt5) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=12 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "8f74a8b922fcf1a8eed7320df9f03fd6",
    .p_ad                   = "",
    .p_nonce                = "925ec6414e777062ac839f06",
    .p_mac                  = "bb0dc912"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_decrypt6) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,

#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt message_len=0 ad_len=16 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "d1f6af919cde85661208bdce0c27cb22",
    .p_ad                   = "7c5faa40e636bbc91107e68010c92b9f",
    .p_nonce                = "898c6929b435017bf031c3c5",
    .p_mac                  = "ae45f11777540a2caeb128be8092468a"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_decrypt7) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt message_len=0 ad_len=16 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "2370e320d4344208e0ff5683f243b213",
    .p_ad                   = "d43a8e5089eea0d026c03a85178b27da",
    .p_nonce                = "04dbb82f044d30831c441228",
    .p_mac                  = "2a049c049d25aa95969b451d93c31c6e"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_decrypt8) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt message_len=0 ad_len=16 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "bc3ab28150fd4cb731d5f48ed2784173",
    .p_ad                   = "53d2fc19963fc99a36a524e39cb68aa8",
    .p_nonce                = "40408445c203c647312b8f01",
    .p_mac                  = "3f04dbd2f49909f73044175041fd9eff"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_decrypt9) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt message_len=16 ad_len=0 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "28286a321293253c3e0aa2704a278032",
    .p_ciphertext           = "5a3c1cf1985dbb8bed818036fdd5ab42",
    .p_key                  = "e98b72a9881a84ca6b76e0f43e68647a",
    .p_ad                   = "",
    .p_nonce                = "8b23299fde174053f3d652ba",
    .p_mac                  = "23c7ab0f952b7091cd324835043b5eb5"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_decrypt10) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt message_len=16 ad_len=0 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "95695a5b12f2870b9cc5fdc8f218a97d",
    .p_ciphertext           = "47bb7e23f7bdfe05a8091ac90e4f8b2e",
    .p_key                  = "33240636cd3236165f1a553b773e728e",
    .p_ad                   = "",
    .p_nonce                = "17c4d61493ecdc8f31700b12",
    .p_mac                  = "b723c70e931d9785f40fd4ab1d612dc9"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_decrypt11) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt message_len=16 ad_len=0 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "95695a5b12f2870b9cc5fdc8f218a97d",
    .p_ciphertext           = "0216c899c88d6e32c958c7e553daa5bc",
    .p_key                  = "5164df856f1e9cac04a79b808dc5be39",
    .p_ad                   = "",
    .p_nonce                = "e76925d5355e0584ce871b2b",
    .p_mac                  = "a145319896329c96df291f64efbe0e3a"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_decrypt12) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt message_len=16 ad_len=16 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "ecafe96c67a1646744f1c891f5e69427",
    .p_ciphertext           = "552ebe012e7bcf90fcef712f8344e8f1",
    .p_key                  = "816e39070410cf2184904da03ea5075a",
    .p_ad                   = "f2a30728ed874ee02983c294435d3c16",
    .p_nonce                = "32c367a3362613b27fc3e67e",
    .p_mac                  = "ecaae9fc68276a45ab0ca3cb9dd9539f"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_decrypt13) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt message_len=16 ad_len=16 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "ecafe96c67a1646744f1c891f5e69427",
    .p_ciphertext           = "1c785025e5a2678e4b29b29276e395bb",
    .p_key                  = "867fc5d5476d5008f0703d81e3622255",
    .p_ad                   = "261a9efd4f32bc3d07c115b4edcf8adf",
    .p_nonce                = "22945529dff947c3c9264df7",
    .p_mac                  = "87fdf1261846164a950c37a3f2eea17d"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_decrypt14) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt message_len=16 ad_len=16 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "ec18a057c22d12373b5efe4d177eb068",
    .p_ciphertext           = "0128a239bb43c12885f9591386ecac0f",
    .p_key                  = "3d17f97bf1dae4268b6610dc90c70b28",
    .p_ad                   = "681a4feac147ee2d25e9191aaa4c8830",
    .p_nonce                = "ebcd88fc18d4c99d28524d41",
    .p_mac                  = "144def0210af9348f07afe27e65bdc7e"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_decrypt15) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt message_len=16 ad_len=90 mac_len=13 nonce_len=128 valid 1",
    .p_plaintext            = "1a964bfebe3908831558699d740bc46f",
    .p_ciphertext           = "55895bbe1f3bc758463a4a9355ef453f",
    .p_key                  = "dc5a53d122399fdfb1e5cc2dfca8900b",
    .p_ad                   = "2b7288f435248dc03dc0f3a2d5c82451bc34c1e319f23ce779d228d150bc8a69e82fb5062aa218d2a7810b39d551227b03e2e954179eac955be7f0a5b213f3a9f262a24f75c7631af3761cfa34adca7d100b9b17ade5c3b19af5",
    .p_nonce                = "fac4d131bc09ffb642a3e19f633f58a7bbc61949cea0ada34ab89df13e0e1b9080f968195782f0f765170c46476bec7b4d062c9714e5bb9cd63e4586d74e4a624ea875ef955b7614323a3f75af979423b4a39140c3a5edcef9ec9a158853a3fee6774e59c9863e1befaee60e2148639db953f4ab3b63e450529b0fccb3d3d134",
    .p_mac                  = "c11a27542917f024c973500da2"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_decrypt16) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt message_len=16 ad_len=90 mac_len=13 nonce_len=128 invalid 1",
    .p_plaintext            = "1a964bfebe3908831558699d740bc46f",
    .p_ciphertext           = "12559383ff63bfaf2863f612e3ee39bc",
    .p_key                  = "c40ee4fec74740982e2b07183b9b81bb",
    .p_ad                   = "3a00dfeb4b66254ce1294c0a18619ae98ab97dac1ba9f321b747979dba6594a5d6fd7d6cea0f7453e1abd65752b168e4c907e32b486396dcf4c2648d534b22e8e619cd390e370cfa7138ac967ff424f6e00fb887da79d6d96c53",
    .p_nonce                = "42c38dae820a700f91ec38ae901947074250f2d813286ed5ec6353f7cd84e1eaa4649e03cbfb2ea9604b05c3fdabf091681a1646b9cec137a2d46b31925ab6741cbd3cc02d1b623b142407472b73a85cf9feef076fea6a56965733f5b15712b97622d75bd818fd996cd1edb38ef1c5d0344169a165a87a5fb278f740ced141d9",
    .p_mac                  = "4412b1fdb1405c250fcfa8db40"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 128 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_128_decrypt17) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 128 Decrypt message_len=16 ad_len=90 mac_len=13 nonce_len=128 valid 2",
    .p_plaintext            = "f3e05222ea6133e1b867a53986e4b432",
    .p_ciphertext           = "5d5d86133086a689fed21cc37c856a6f",
    .p_key                  = "fef36ef7f9305ecbd990619f28b2b9b6",
    .p_ad                   = "5f4990dce32043db4143aa58aba99a3cfb745444cc1a282bcc47aa58cc7031b36fd25c7575ea585fffe09e60bdcb477381a35f3fa3ec09426f05c84136e24f9aeabe94f8117e8e2e9f7b3527877c0d9442587642c57c2be63415",
    .p_nonce                = "601ef18957f79ccf67cacfd801aaed57a66c7e666385aeb9456284c567c27e38190fe652d44e9affe538236f25939ab127418498f04feeca35b38fca44e3fd0b6f98b77425cc403e6edb9800cc5b343d6c01d9871cbc9a3b478447267aa5b6614ee8d25e141275c1f46f083ee93eaccb35040b22ceaed28f0f28cadb48d576a0",
    .p_mac                  = "bd273924ae7e3ef6aa3ee66dbf"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_GCM_128)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_GCM_192)

// AES GCM - Custom test vector 1 - Invalid behavior test for AES plaintext and AD.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_gcm_192_inv_c1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 message_len=0 ad_len=0",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "3f4acd43fdcff484b1df2073c3d506bcccaf4eeb9184c9db",
    .p_ad                   = "",
    .p_nonce                = "457d26179c4405a24dfcc1e8da",
    .p_mac                  = "04cb55750acf4061"
};

// AES GCM - Custom test vector 2 - Invalid behavior test for AES buffer authenticated decryption.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_gcm_192_inv_c2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt Invalid ciphertext",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "5d592caa5ecffd6815bc73543ba1a8f7db636b88abd377",
    .p_key                  = "3f4acd43fdcff484b1df2073c3d506bcccaf4eeb9184c9db",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "457d26179c4405a24dfcc1e8da",
    .p_mac                  = "146e5bd753a09177"
};

// AES GCM - Custom test vector 3 - Invalid behavior test for AES buffer authenticated decryption.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_gcm_192_inv_c3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
    .crypt_expected_result  = EXPECTED_TO_FAIL,  // Generated plaintext will be incorrect.
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
    .crypt_expected_result  = EXPECTED_TO_PASS,  // Generated plaintext will be correct.
#endif
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt Invalid MAC",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "4d592caa5ecffd6815bc73543ba1a8f7db636b88abd377",
    .p_key                  = "3f4acd43fdcff484b1df2073c3d506bcccaf4eeb9184c9db",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "457d26179c4405a24dfcc1e8da",
    .p_mac                  = "246e5bd753a09177"
};

// AES GCM - Custom test vector 4 - Invalid behavior test for AES key.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_gcm_192_inv_c4) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt with invalid AES key",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "4d592caa5ecffd6815bc73543ba1a8f7db636b88abd377",
    .p_key                  = "4f4acd43fdcff484b1df2073c3d506bcccaf4eeb9184c9db",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "457d26179c4405a24dfcc1e8da",
    .p_mac                  = "146e5bd753a09177"
};

// AES GCM - Custom test vector 5 - Invalid behavior test for AES key.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_gcm_192_inv_c5) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt with invalid AES key",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "4d592caa5ecffd6815bc73543ba1a8f7db636b88abd377",
    .p_key                  = "4f4acd43fdcff484b1df2073c3d506bcccaf4eeb9184c9db",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "457d26179c4405a24dfcc1e8da",
    .p_mac                  = "146e5bd753a09177"
};

// AES GCM - Custom test vector 6 - Invalid behavior test for AES nonce.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_gcm_192_inv_c6) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt with invalid AES nonce",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "4d592caa5ecffd6815bc73543ba1a8f7db636b88abd377",
    .p_key                  = "3f4acd43fdcff484b1df2073c3d506bcccaf4eeb9184c9db",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "557d26179c4405a24dfcc1e8da",
    .p_mac                  = "146e5bd753a09177"
};

// AES GCM - Custom test vector 7 - Invalid behavior test for AES nonce.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_gcm_192_inv_c7) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt with invalid AES nonce",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "4d592caa5ecffd6815bc73543ba1a8f7db636b88abd377",
    .p_key                  = "3f4acd43fdcff484b1df2073c3d506bcccaf4eeb9184c9db",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "557d26179c4405a24dfcc1e8da",
    .p_mac                  = "146e5bd753a09177"
};

// AES GCM - Custom test vector 8
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_gcm_192_c8) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 message_len=250 ad_len=8 mac_len=8 nonce_len=13",
    .p_plaintext            = "e96325847ce207c2bf6638a52634075139998888fa0ca1501a4b57ee0767210f1fee065b7ed1e4005fe1c3ec482b2d3cad3d8461d4ab2ba1c73cbe2c65ac8b1e61be3f052ecadf671a3f58601ec0f1ac95367dae85601449f434b9d10757a613784a229aa14bebe6ed07579ec0a513ce7ce799d1e6cd468613f0d3f42e6162680b28f13aeb826572934424459137bd3aabec437bf4fdff007d2675d7fd2195280da092eb1b3549e5457e065cc47349f2c3013b8a4a1938b05c033b2a5b6b477be39b415f4f50beac41f817783bda28f24bab3c8095e1b01e48ce4271cd2a323d1d8c100bb9856f75f63bdee34510de01e32efc0b12e980e9161a",
    .p_ciphertext           = "ac3303252e20f4a5bacb59e20980b9b1fae3f91b4dc2c8deca8ef69c0cac8141e093d07a83b21eb7100bb51594e8e86b13c3bb9cb687d827cca37d42aa03716c83d9c31251a9401afb8a522c8d0dbb4787b60eb5e2d4cf0e939678065eb3cbdb2c5d4a1d40cb226a607bca61ca749ef280903d1bd607cb223add245a8f6463864249175e0936d9f882f99ccb814e4cfb77f5758dc871ddcf736df52af2447ade0ef3b0f1b8d6e66fc2468398e1e3cc4c5ba3eca38f9f30b9b13f92f7512dcc24475f4bfcfe081c28d1f83603efeec36c1b8ee8b3d5e40214898f5964b6c62cafa19a541728bb3e678677e1e62423fb54085360434f2a7495dd9d",
    .p_key                  = "3f4acd43fdcff484b1df2073c3d506bcccaf4eeb9184c9db",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "457d26179c4405a24dfcc1e8da",
    .p_mac                  = "c0afcc477c8f5395"
};

// AES GCM - Custom test vector 9
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_gcm_192_c9) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 message_len=16 ad_len=250 mac_len=8 nonce_len=13",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "4d592caa5ecffd6815bc73543ba1a8f7db636b88abd377",
    .p_key                  = "3f4acd43fdcff484b1df2073c3d506bcccaf4eeb9184c9db",
    .p_ad                   = "0ee0089ec32c7cfa036bd1ab9c23b98cd5d166214d61c719688c6a0ecff705da47ac179cae7eaa3b638ab022a6669729fe47f24679e78488125d989cc7d1bbd8b472b04678b45adde4cca92cd91c4e214257243b163eca04bf643e9df6f7c5bf55aa91bc348154b5c3e9529a56fed8b04e0726b6de3ecc43f499108933e07297bdfcd4d8df2aa9af908083aeccc5c23fe9efbbf5fa9a27ea2923d36188197c87c955fcc5f7e0ebff4b01ebf008a5c93dab12ddfbf65f4225e5a3dfd44e36dbc3ba7891fce514d127e5eb8adb939670b4c5643c46425c614546b90cf0d720203bef4a12ff2d766623c92e134420e3b423b526598f32e8884843bc",
    .p_nonce                = "457d26179c4405a24dfcc1e8da",
    .p_mac                  = "7e099f28fb401297"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_encrypt0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "0e5d6e68f82f32bea3f0b69498c1a31ef6d955cd3d27a2a8",
    .p_ad                   = "",
    .p_nonce                = "caf72ee1e62e1001e8cfbc63",
    .p_mac                  = "db1a74ffb5f7de26f5742e0942b1b9cb"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_encrypt1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "f780e4089a6224b9db576b368149fc4f58e6c8f7061814e2",
    .p_ad                   = "",
    .p_nonce                = "ae84a7ac9f89a70c92d566ec",
    .p_mac                  = "cd65b48665146066b92b34b38ec3415e"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_encrypt2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "0ed9f074d53b9b71bb87d123c16d269728ad819d89bba91c",
    .p_ad                   = "",
    .p_nonce                = "8b4e5be3cef24b12640bd781",
    .p_mac                  = "258e9d80875b4a6f4b7216da0db35e47"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_encrypt3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=12 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bc27da31f2ad93f1bf07a386c3e437d5d1d8e641a778396",
    .p_ad                   = "",
    .p_nonce                = "1e437ff8358fc7401bd935e2",
    .p_mac                  = "08967511"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_encrypt4) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=12 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "3d673cb7f8f9b599cef3eb4956c319c939caccb627aea76e",
    .p_ad                   = "",
    .p_nonce                = "bfc4d6b4a20ce5f9193ee4f5",
    .p_mac                  = "a082171c"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_encrypt5) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=12 invalid 3",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "f3d1cd3c9c112a3a3b3768c3b65db6b9345028679513c4df",
    .p_ad                   = "",
    .p_nonce                = "9a789d2c76b3d9d5731e8acf",
    .p_mac                  = "bbd39626"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_encrypt6) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt message_len=0 ad_len=16 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "aa921cb5a243ab08911f3289266b39dab133f5c420a6c5cd",
    .p_ad                   = "487443c7c14ce474cb3d291f257070a2",
    .p_nonce                = "8f73db68daeeed2d155fb1a0",
    .p_mac                  = "b12674fbeac6889a24948f2792e30a50"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_encrypt7) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt message_len=0 ad_len=16 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "0778c653b8318c16cbe96af79e6fb57a628bb0a5eb65addc",
    .p_ad                   = "303a3c3f74104252abcf89282521cb65",
    .p_nonce                = "d901bc5babba7958af15c8b8",
    .p_mac                  = "d599c2b12f4afa1aeebbdee4c9e3ccf8"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_encrypt8) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt message_len=0 ad_len=16 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "409e7a1f2dc87b887da6cd51fa66b3c439d6bb64eed4f58b",
    .p_ad                   = "7ca7f102db61be6777cfb3142c81df38",
    .p_nonce                = "2c3eac60c57a4db5343c9fe5",
    .p_mac                  = "c4d72c983924fcd886b6e076525b7104"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_encrypt9) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt message_len=16 ad_len=0 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "99ae6f479b3004354ff18cd86c0b6efb",
    .p_ciphertext           = "132ae95bd359c44aaefa6348632cafbd",
    .p_key                  = "7a7c5b6a8a9ab5acae34a9f6e41f19a971f9c330023c0f0c",
    .p_ad                   = "",
    .p_nonce                = "aa4c38bf587f94f99fee77d5",
    .p_mac                  = "19d7c7d5809ad6648110f22f272e7d72"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_encrypt10) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt message_len=16 ad_len=0 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "7d81b948515c975308086fa960f7cc9a",
    .p_ciphertext           = "6e3720e76b6b4bf567ce9ae57f4aac9f",
    .p_key                  = "2797751ef6dd828c794429543f8d690e9fe618139ac1510f",
    .p_ad                   = "",
    .p_nonce                = "d960f74be927580a2e9f57ef",
    .p_mac                  = "f277556fc0a94b4ea786465ca5146aa4"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_encrypt11) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt message_len=16 ad_len=0 mac_len=16 nonce_len=12 valid 3",
    .p_plaintext            = "28aa46393e1b8b8784b035f35d1ea9fa",
    .p_ciphertext           = "2f3c56eeeb62b393c571d126177865e9",
    .p_key                  = "843ae0ef237636df82b0307fb9064ee12fdab5a98e838bd6",
    .p_ad                   = "",
    .p_nonce                = "1df141ca68757aef80e414ec",
    .p_mac                  = "0fa560a3474eb37895bebb990d5ebf14"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_encrypt12) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt message_len=16 ad_len=16 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "28aa46393e1b8b8784b035f35d1ea9fa",
    .p_ciphertext           = "ad475445f3eecb217387be44895254e1",
    .p_key                  = "f3167d3ea919d15bb5254717cc8b2a18da1e35950a872089",
    .p_ad                   = "cc67a35257f75e8816d0de5b83d321f0",
    .p_nonce                = "5583ee965e59308a902a0703",
    .p_mac                  = "f7e9cef0e7296bdb0b4305b0a8edf9ac"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_encrypt13) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt message_len=16 ad_len=16 mac_len=16 nonce_len=12 invalid 2",
    .p_plaintext            = "28aa46393e1b8b8784b035f35d1ea9fa",
    .p_ciphertext           = "1d8a02f16a49117c4358380db3d6518d",
    .p_key                  = "1a4c7488415dd3ba950be4a3bcd1ca00cd004afa28c2e1ce",
    .p_ad                   = "fa0f64cb4ea0ce865aa4144e03ee0fd1",
    .p_nonce                = "c9de7004ac7f9ab4ece85998",
    .p_mac                  = "c6bd80d982cc9db923b06e18bc557401"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_encrypt14) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt message_len=16 ad_len=16 mac_len=16 nonce_len=12 invalid 3",
    .p_plaintext            = "28aa46393e1b8b8784b035f35d1ea9fa",
    .p_ciphertext           = "8ce3218d510f2f73621935a9dea64d1e",
    .p_key                  = "85e0788c66431fd78ea4dbc188a380a74f2568252623e293",
    .p_ad                   = "c1cc58d09b4a1e354275478ee87673e9",
    .p_nonce                = "b067415cca095c907c4e7473",
    .p_mac                  = "d0bea06cf298f9e60a148715e587acb5"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_encrypt15) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt message_len=16 ad_len=90 mac_len=13 nonce_len=128 valid 1",
    .p_plaintext            = "aaa7c2dab7d6157f66268d60b6a718c0",
    .p_ciphertext           = "5b25b7681904d78ff19dde920c97623b",
    .p_key                  = "f10357dde935755c762a96659886688a25bf5fd2ad71ac9b",
    .p_ad                   = "bf36dc030d5bd3abbf2999df74aa81c2769338603e9d29f5c6016b041937157eadf786666bc7c30f6db9bede08273768942a552692fce7e1ea5c17b3e7b86733adcea5f6711a143b1be9525bf3c8b355e316029fa787c32e52dc",
    .p_nonce                = "8bbd8715baf1025c029d1191ef59f29564ca8bde6cfb1c97aee754a7876205f31769c9f774ce697d11b9c2581aae1efdf17b5ad759d0f67e06687abb81eda51726e11de3e45a34d29b826a2c0fdc76a7f8180be72d602da9ec6bb186bd4c4e7c111cdf8a94b3a30473e3e2ce7c8055b9edf41386b09480b0efe61e8199a576ed",
    .p_mac                  = "0a951dc55d979a5615a9467bce"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_encrypt16) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt message_len=16 ad_len=90 mac_len=13 nonce_len=128 invalid 1",
    .p_plaintext            = "aaa7c2dab7d6157f66268d60b6a718c0",
    .p_ciphertext           = "ae2e4c37a04ae37aade697cea78a26b2",
    .p_key                  = "fe06c336529fd8533a53cb79c0500e2a569fc8febd161b0f",
    .p_ad                   = "1ea558f79d06bed3bda5e03989dbaa583f27491a76cb9347f74b753e0c537df9736db3ab38bc9f80d78190b8fda0cf2d31c9d3678540e8685c136e6ebb08e4b0c3133b3b9732cb8ebbbfb0cbb780f3fc338c1ed338cd23537866",
    .p_nonce                = "13aa3a4cc62921bedc8936f7a8e80fa7ed89639f2729565e2f8076efda1a411bee8bb10802d30538574fd86bbd05183c98d10bdc0bdc80fe3c8617c69bd9c839bec88f81b6624543d8ead96c85106338e2898d60289e1f4ddf49dac2c3dc0c6032533b1d7afe81e8701c5a9d0ef8a2e5bca1bf82d332b22c7d4ef2110e803308",
    .p_mac                  = "c35d020bf4bcda7e972a81a253"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_encrypt17) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 192 Encrypt message_len=16 ad_len=90 mac_len=13 nonce_len=128 valid 2",
    .p_plaintext            = "b32ef00ee70256466b9cdc1fa5728907",
    .p_ciphertext           = "0f74951242b2693f853daf1dc6660230",
    .p_key                  = "e218693f9fe9ee77a9cf7a7ac4d44b8a4cd8ca4e33cfe6b8",
    .p_ad                   = "d3bebae709147dbee6d023d3c2efc4d50b544a624b9843f8e6a3d13e7366b36c89c50c202b43d1f3ef0d405b632cd3e3512f405dd339ce9b858bb721ae36e27cdda66c8163f37072fa84be152d1e96734c620f5228da2bc2403c",
    .p_nonce                = "6a8d1a8778445165d4ee8212f8a2f48063f5ae588f9029ebf8431b9efcb185035c97488ed8b7f0004b864028f76692f193f0c68f20d9b5e6306147db45913403ebd8f5a9cf0f342e540c77cbb27ae33e159a2acadcf06bd196852fcab6f7275061bf20f7648e0ad155ec4b5de75254275ddad8f2e05469a20195b799103c6435",
    .p_mac                  = "814e7e5595f8001801e482ed45"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_decrypt0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "0e5d6e68f82f32bea3f0b69498c1a31ef6d955cd3d27a2a8",
    .p_ad                   = "",
    .p_nonce                = "caf72ee1e62e1001e8cfbc63",
    .p_mac                  = "db1a74ffb5f7de26f5742e0942b1b9cb"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_decrypt1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "f780e4089a6224b9db576b368149fc4f58e6c8f7061814e2",
    .p_ad                   = "",
    .p_nonce                = "ae84a7ac9f89a70c92d566ec",
    .p_mac                  = "cd65b48665146066b92b34b38ec3415e"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_decrypt2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "0ed9f074d53b9b71bb87d123c16d269728ad819d89bba91c",
    .p_ad                   = "",
    .p_nonce                = "8b4e5be3cef24b12640bd781",
    .p_mac                  = "258e9d80875b4a6f4b7216da0db35e47"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_decrypt3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=12 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bc27da31f2ad93f1bf07a386c3e437d5d1d8e641a778396",
    .p_ad                   = "",
    .p_nonce                = "1e437ff8358fc7401bd935e2",
    .p_mac                  = "08967511"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_decrypt4) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=12 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "3d673cb7f8f9b599cef3eb4956c319c939caccb627aea76e",
    .p_ad                   = "",
    .p_nonce                = "bfc4d6b4a20ce5f9193ee4f5",
    .p_mac                  = "a082171c"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_decrypt5) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=12 invalid 3",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "f3d1cd3c9c112a3a3b3768c3b65db6b9345028679513c4df",
    .p_ad                   = "",
    .p_nonce                = "9a789d2c76b3d9d5731e8acf",
    .p_mac                  = "bbd39626"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_decrypt6) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt message_len=0 ad_len=16 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "aa921cb5a243ab08911f3289266b39dab133f5c420a6c5cd",
    .p_ad                   = "487443c7c14ce474cb3d291f257070a2",
    .p_nonce                = "8f73db68daeeed2d155fb1a0",
    .p_mac                  = "b12674fbeac6889a24948f2792e30a50"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_decrypt7) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt message_len=0 ad_len=16 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "0778c653b8318c16cbe96af79e6fb57a628bb0a5eb65addc",
    .p_ad                   = "303a3c3f74104252abcf89282521cb65",
    .p_nonce                = "d901bc5babba7958af15c8b8",
    .p_mac                  = "d599c2b12f4afa1aeebbdee4c9e3ccf8"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_decrypt8) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt message_len=0 ad_len=16 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "409e7a1f2dc87b887da6cd51fa66b3c439d6bb64eed4f58b",
    .p_ad                   = "7ca7f102db61be6777cfb3142c81df38",
    .p_nonce                = "2c3eac60c57a4db5343c9fe5",
    .p_mac                  = "c4d72c983924fcd886b6e076525b7104"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_decrypt9) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt message_len=16 ad_len=0 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "99ae6f479b3004354ff18cd86c0b6efb",
    .p_ciphertext           = "132ae95bd359c44aaefa6348632cafbd",
    .p_key                  = "7a7c5b6a8a9ab5acae34a9f6e41f19a971f9c330023c0f0c",
    .p_ad                   = "",
    .p_nonce                = "aa4c38bf587f94f99fee77d5",
    .p_mac                  = "19d7c7d5809ad6648110f22f272e7d72"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_decrypt10) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt message_len=16 ad_len=0 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "7d81b948515c975308086fa960f7cc9a",
    .p_ciphertext           = "6e3720e76b6b4bf567ce9ae57f4aac9f",
    .p_key                  = "2797751ef6dd828c794429543f8d690e9fe618139ac1510f",
    .p_ad                   = "",
    .p_nonce                = "d960f74be927580a2e9f57ef",
    .p_mac                  = "f277556fc0a94b4ea786465ca5146aa4"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_decrypt11) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt message_len=16 ad_len=0 mac_len=16 nonce_len=12 valid 3",
    .p_plaintext            = "28aa46393e1b8b8784b035f35d1ea9fa",
    .p_ciphertext           = "2f3c56eeeb62b393c571d126177865e9",
    .p_key                  = "843ae0ef237636df82b0307fb9064ee12fdab5a98e838bd6",
    .p_ad                   = "",
    .p_nonce                = "1df141ca68757aef80e414ec",
    .p_mac                  = "0fa560a3474eb37895bebb990d5ebf14"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_decrypt12) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt message_len=16 ad_len=16 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "28aa46393e1b8b8784b035f35d1ea9fa",
    .p_ciphertext           = "ad475445f3eecb217387be44895254e1",
    .p_key                  = "f3167d3ea919d15bb5254717cc8b2a18da1e35950a872089",
    .p_ad                   = "cc67a35257f75e8816d0de5b83d321f0",
    .p_nonce                = "5583ee965e59308a902a0703",
    .p_mac                  = "f7e9cef0e7296bdb0b4305b0a8edf9ac"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_decrypt13) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt message_len=16 ad_len=16 mac_len=16 nonce_len=12 invalid 2",
    .p_plaintext            = "28aa46393e1b8b8784b035f35d1ea9fa",
    .p_ciphertext           = "1d8a02f16a49117c4358380db3d6518d",
    .p_key                  = "1a4c7488415dd3ba950be4a3bcd1ca00cd004afa28c2e1ce",
    .p_ad                   = "fa0f64cb4ea0ce865aa4144e03ee0fd1",
    .p_nonce                = "c9de7004ac7f9ab4ece85998",
    .p_mac                  = "c6bd80d982cc9db923b06e18bc557401"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_decrypt14) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt message_len=16 ad_len=16 mac_len=16 nonce_len=12 invalid 3",
    .p_plaintext            = "28aa46393e1b8b8784b035f35d1ea9fa",
    .p_ciphertext           = "8ce3218d510f2f73621935a9dea64d1e",
    .p_key                  = "85e0788c66431fd78ea4dbc188a380a74f2568252623e293",
    .p_ad                   = "c1cc58d09b4a1e354275478ee87673e9",
    .p_nonce                = "b067415cca095c907c4e7473",
    .p_mac                  = "d0bea06cf298f9e60a148715e587acb5"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_decrypt15) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt message_len=16 ad_len=90 mac_len=13 nonce_len=128 valid 1",
    .p_plaintext            = "aaa7c2dab7d6157f66268d60b6a718c0",
    .p_ciphertext           = "5b25b7681904d78ff19dde920c97623b",
    .p_key                  = "f10357dde935755c762a96659886688a25bf5fd2ad71ac9b",
    .p_ad                   = "bf36dc030d5bd3abbf2999df74aa81c2769338603e9d29f5c6016b041937157eadf786666bc7c30f6db9bede08273768942a552692fce7e1ea5c17b3e7b86733adcea5f6711a143b1be9525bf3c8b355e316029fa787c32e52dc",
    .p_nonce                = "8bbd8715baf1025c029d1191ef59f29564ca8bde6cfb1c97aee754a7876205f31769c9f774ce697d11b9c2581aae1efdf17b5ad759d0f67e06687abb81eda51726e11de3e45a34d29b826a2c0fdc76a7f8180be72d602da9ec6bb186bd4c4e7c111cdf8a94b3a30473e3e2ce7c8055b9edf41386b09480b0efe61e8199a576ed",
    .p_mac                  = "0a951dc55d979a5615a9467bce"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_decrypt16) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt message_len=16 ad_len=90 mac_len=13 nonce_len=128 invalid 1",
    .p_plaintext            = "aaa7c2dab7d6157f66268d60b6a718c0",
    .p_ciphertext           = "ae2e4c37a04ae37aade697cea78a26b2",
    .p_key                  = "fe06c336529fd8533a53cb79c0500e2a569fc8febd161b0f",
    .p_ad                   = "1ea558f79d06bed3bda5e03989dbaa583f27491a76cb9347f74b753e0c537df9736db3ab38bc9f80d78190b8fda0cf2d31c9d3678540e8685c136e6ebb08e4b0c3133b3b9732cb8ebbbfb0cbb780f3fc338c1ed338cd23537866",
    .p_nonce                = "13aa3a4cc62921bedc8936f7a8e80fa7ed89639f2729565e2f8076efda1a411bee8bb10802d30538574fd86bbd05183c98d10bdc0bdc80fe3c8617c69bd9c839bec88f81b6624543d8ead96c85106338e2898d60289e1f4ddf49dac2c3dc0c6032533b1d7afe81e8701c5a9d0ef8a2e5bca1bf82d332b22c7d4ef2110e803308",
    .p_mac                  = "c35d020bf4bcda7e972a81a253"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 192 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_192_decrypt17) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 192 Decrypt message_len=16 ad_len=90 mac_len=13 nonce_len=128 valid 2",
    .p_plaintext            = "b32ef00ee70256466b9cdc1fa5728907",
    .p_ciphertext           = "0f74951242b2693f853daf1dc6660230",
    .p_key                  = "e218693f9fe9ee77a9cf7a7ac4d44b8a4cd8ca4e33cfe6b8",
    .p_ad                   = "d3bebae709147dbee6d023d3c2efc4d50b544a624b9843f8e6a3d13e7366b36c89c50c202b43d1f3ef0d405b632cd3e3512f405dd339ce9b858bb721ae36e27cdda66c8163f37072fa84be152d1e96734c620f5228da2bc2403c",
    .p_nonce                = "6a8d1a8778445165d4ee8212f8a2f48063f5ae588f9029ebf8431b9efcb185035c97488ed8b7f0004b864028f76692f193f0c68f20d9b5e6306147db45913403ebd8f5a9cf0f342e540c77cbb27ae33e159a2acadcf06bd196852fcab6f7275061bf20f7648e0ad155ec4b5de75254275ddad8f2e05469a20195b799103c6435",
    .p_mac                  = "814e7e5595f8001801e482ed45"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_GCM_192)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_GCM_256)

// AES GCM - Custom test vector 1 - Invalid behavior test for AES plaintext and AD.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_gcm_256_inv_c1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 message_len=0 ad_len=0",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "37e16ad43779aa3b211679f375b31d0f15163ede0c5db4b9ebfa0ba7fc9e59ea",
    .p_ad                   = "",
    .p_nonce                = "cd45bc603e251dfbceb5a8de2f",
    .p_mac                  = "ca8a20df75d44807"
};

// AES GCM - Custom test vector 2 - Invalid behavior test for AES buffer authenticated decryption.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_gcm_256_inv_c2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt Invalid ciphertext",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "9fe6262e7040eae0feb2b17983d20ffbe04f23faa0246f",
    .p_key                  = "37e16ad43779aa3b211679f375b31d0f15163ede0c5db4b9ebfa0ba7fc9e59ea",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "cd45bc603e251dfbceb5a8de2f",
    .p_mac                  = "a4d04ea61a68aa21"
};

// AES GCM - Custom test vector 3 - Invalid behavior test for AES buffer authenticated decryption.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_gcm_256_inv_c3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
    .crypt_expected_result  = EXPECTED_TO_FAIL,  // Generated plaintext will be incorrect.
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
    .crypt_expected_result  = EXPECTED_TO_PASS,  // Generated plaintext will be correct.
#endif
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt Invalid MAC",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "8fe6262e7040eae0feb2b17983d20ffbe04f23faa0246f",
    .p_key                  = "37e16ad43779aa3b211679f375b31d0f15163ede0c5db4b9ebfa0ba7fc9e59ea",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "cd45bc603e251dfbceb5a8de2f",
    .p_mac                  = "b4d04ea61a68aa21"
};

// AES GCM - Custom test vector 4 - Invalid behavior test for AES key.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_gcm_256_inv_c4) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt with invalid AES key",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "8fe6262e7040eae0feb2b17983d20ffbe04f23faa0246f",
    .p_key                  = "47e16ad43779aa3b211679f375b31d0f15163ede0c5db4b9ebfa0ba7fc9e59ea",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "cd45bc603e251dfbceb5a8de2f",
    .p_mac                  = "a4d04ea61a68aa21"
};

// AES GCM - Custom test vector 5 - Invalid behavior test for AES key.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_gcm_256_inv_c5) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt with invalid AES key",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "8fe6262e7040eae0feb2b17983d20ffbe04f23faa0246f",
    .p_key                  = "47e16ad43779aa3b211679f375b31d0f15163ede0c5db4b9ebfa0ba7fc9e59ea",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "cd45bc603e251dfbceb5a8de2f",
    .p_mac                  = "a4d04ea61a68aa21"
};

// AES GCM - Custom test vector 6 - Invalid behavior test for AES nonce.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_gcm_256_inv_c6) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt with invalid AES nonce",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "8fe6262e7040eae0feb2b17983d20ffbe04f23faa0246f",
    .p_key                  = "37e16ad43779aa3b211679f375b31d0f15163ede0c5db4b9ebfa0ba7fc9e59ea",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "dd45bc603e251dfbceb5a8de2f",
    .p_mac                  = "a4d04ea61a68aa21"
};

// AES GCM - Custom test vector 7 - Invalid behavior test for AES nonce.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_gcm_256_inv_c7) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt with invalid AES nonce",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "8fe6262e7040eae0feb2b17983d20ffbe04f23faa0246f",
    .p_key                  = "37e16ad43779aa3b211679f375b31d0f15163ede0c5db4b9ebfa0ba7fc9e59ea",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "dd45bc603e251dfbceb5a8de2f",
    .p_mac                  = "a4d04ea61a68aa21"
};

// AES GCM - Custom test vector 8
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_gcm_256_c8) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 message_len=250 ad_len=8 mac_len=8 nonce_len=13",
    .p_plaintext            = "e96325847ce207c2bf6638a52634075139998888fa0ca1501a4b57ee0767210f1fee065b7ed1e4005fe1c3ec482b2d3cad3d8461d4ab2ba1c73cbe2c65ac8b1e61be3f052ecadf671a3f58601ec0f1ac95367dae85601449f434b9d10757a613784a229aa14bebe6ed07579ec0a513ce7ce799d1e6cd468613f0d3f42e6162680b28f13aeb826572934424459137bd3aabec437bf4fdff007d2675d7fd2195280da092eb1b3549e5457e065cc47349f2c3013b8a4a1938b05c033b2a5b6b477be39b415f4f50beac41f817783bda28f24bab3c8095e1b01e48ce4271cd2a323d1d8c100bb9856f75f63bdee34510de01e32efc0b12e980e9161a",
    .p_ciphertext           = "6e8c09a100afe32d51c59bcfb1f31ebdc1cfb1694635d02e23f49514ad0a4b0b1fbc2c0b09a56d2c59e6bb4f75328c4e55ee3bf4182930825dd6119fb862a33a2d89e59b6856ec8e86402f6a06e60ecb0c70c73a64d51255f5ee43f3fcee46e694518344441c0692e1056cdd9159b390aa0c56500eeedef93cb4fbc0da2fac791f5866fb32dd96e31c7926e10700cd0ec8cae45bac04f3ef21246a4b4d1e16ff0823b02c442cad3041f79f38cbf169f3d8ffad1f07dc06a3035768328d9539a3f9dc821b04b6ba2620340dcc51968d479fe199dc8271fca1d5f074a3f69399824ed6d520d5e573dc64dd832ca4918bb3f6df42fab62ed22e7c62",
    .p_key                  = "37e16ad43779aa3b211679f375b31d0f15163ede0c5db4b9ebfa0ba7fc9e59ea",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "cd45bc603e251dfbceb5a8de2f",
    .p_mac                  = "cca55736e5392b49"
};

// AES GCM - Custom test vector 9
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_gcm_256_c9) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 message_len=16 ad_len=250 mac_len=8 nonce_len=13",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "8fe6262e7040eae0feb2b17983d20ffbe04f23faa0246f",
    .p_key                  = "37e16ad43779aa3b211679f375b31d0f15163ede0c5db4b9ebfa0ba7fc9e59ea",
    .p_ad                   = "0ee0089ec32c7cfa036bd1ab9c23b98cd5d166214d61c719688c6a0ecff705da47ac179cae7eaa3b638ab022a6669729fe47f24679e78488125d989cc7d1bbd8b472b04678b45adde4cca92cd91c4e214257243b163eca04bf643e9df6f7c5bf55aa91bc348154b5c3e9529a56fed8b04e0726b6de3ecc43f499108933e07297bdfcd4d8df2aa9af908083aeccc5c23fe9efbbf5fa9a27ea2923d36188197c87c955fcc5f7e0ebff4b01ebf008a5c93dab12ddfbf65f4225e5a3dfd44e36dbc3ba7891fce514d127e5eb8adb939670b4c5643c46425c614546b90cf0d720203bef4a12ff2d766623c92e134420e3b423b526598f32e8884843bc",
    .p_nonce                = "cd45bc603e251dfbceb5a8de2f",
    .p_mac                  = "4b9f2a6b57c00b1f"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_encrypt0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "f5a2b27c74355872eb3ef6c5feafaa740e6ae990d9d48c3bd9bb8235e589f010",
    .p_ad                   = "",
    .p_nonce                = "58d2240f580a31c1d24948e9",
    .p_mac                  = "15e051a5e4a5f5da6cea92e2ebee5bac"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_encrypt1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "e5a8123f2e2e007d4e379ba114a2fb66e6613f57c72d4e4f024964053028a831",
    .p_ad                   = "",
    .p_nonce                = "51e43385bf533e168427e1ad",
    .p_mac                  = "38fe845c66e66bdd884c2aecafd280e6"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_encrypt2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=12 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "c5ba3bf9ada560fb0301cd4e0b8b8a46a2aff2400395a72ed5804b3c755c4e59",
    .p_ad                   = "",
    .p_nonce                = "de2b956e704063c000f28bbb",
    .p_mac                  = "04eb9b20fd4548dc71594f20073a45a2"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_encrypt3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=12 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "5765ad16f4dbd4d4e6370dbfa18b8ef5ac37459e39556afaba9b553dea4c1b23",
    .p_ad                   = "",
    .p_nonce                = "59678f262588e4d526d9fc34",
    .p_mac                  = "a68f33ed"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_encrypt4) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=12 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "a29fa93614e9f714c73d3707d1142b2514fa8a14641f79ef14e9bf0e812f49fa",
    .p_ad                   = "",
    .p_nonce                = "68272acf71b1cdad4655dbdc",
    .p_mac                  = "b90e554a"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_encrypt5) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=12 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "717d2a5396ae21560a895a7847f53c47956d155ca4dd2165137d3506ff48a7bf",
    .p_ad                   = "",
    .p_nonce                = "6ccd8086830f586fa3b0e777",
    .p_mac                  = "a618ac6a"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_encrypt6) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt message_len=0 ad_len=16 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "6dfdafd6703c285c01f14fd10a6012862b2af950d4733abb403b2e745b26945d",
    .p_ad                   = "c0d249871992e70302ae008193d1e89f",
    .p_nonce                = "3749d0b3d5bacb71be06ade6",
    .p_mac                  = "4aa4cc69f84ee6ac16d9bfb4e05de500"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_encrypt7) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt message_len=0 ad_len=16 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "033360a08d0b2963ce4dcf807b772ac86ae3e8b8fabb9cd3a636f8ec54365646",
    .p_ad                   = "f9d15fc34984b8f4a7caa29a82b24c52",
    .p_nonce                = "8dcaa63d13a2425395609914",
    .p_mac                  = "42bc9cc93a1c8592bd29c9ef9f907780"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_encrypt8) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt message_len=0 ad_len=16 mac_len=16 nonce_len=12 valid 3",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4f1a5fc8e4689c493ef8e23a653e86e4d4c8972a2338653375b0f36a4feb91d7",
    .p_ad                   = "f09b0fc4e4ccb5e28dfcc0792a6800f3",
    .p_nonce                = "9b8221a631404088218fe487",
    .p_mac                  = "31073f1a0050462a03c7ad0bbefaf93b"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_encrypt9) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt message_len=16 ad_len=0 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "7789b41cb3ee548814ca0b388c10b343",
    .p_ciphertext           = "d2c78110ac7e8f107c0df0570bd7c90c",
    .p_key                  = "4c8ebfe1444ec1b2d503c6986659af2c94fafe945f72c1e8486a5acfedb8a0f8",
    .p_ad                   = "",
    .p_nonce                = "473360e0ad24889959858995",
    .p_mac                  = "c26a379b6d98ef2852ead8ce83a833a7"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_encrypt10) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt message_len=16 ad_len=0 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "697aff2d6b77e5ed6232770e400c1ead",
    .p_ciphertext           = "e0de64302ac2d04048d65a87d2ad09fe",
    .p_key                  = "3934f363fd9f771352c4c7a060682ed03c2864223a1573b3af997e2ababd60ab",
    .p_ad                   = "",
    .p_nonce                = "efe2656d878c586e41c539c4",
    .p_mac                  = "33cbd8d2fb8a3a03e30c1eb1b53c1d99"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_encrypt11) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt message_len=16 ad_len=0 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "697aff2d6b77e5ed6232770e400c1ead",
    .p_ciphertext           = "9f6607d68e22ccf21928db0986be126e",
    .p_key                  = "c997768e2d14e3d38259667a6649079de77beb4543589771e5068e6cd7cd0b14",
    .p_ad                   = "",
    .p_nonce                = "835090aed9552dbdd45277e2",
    .p_mac                  = "f32617f67c574fd9f44ef76ff880ab9f"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_encrypt12) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt message_len=16 ad_len=16 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "85fc3dfad9b5a8d3258e4fc44571bd3b",
    .p_ciphertext           = "426e0efc693b7be1f3018db7ddbb7e4d",
    .p_key                  = "54e352ea1d84bfe64a1011096111fbe7668ad2203d902a01458c3bbd85bfce14",
    .p_ad                   = "7e968d71b50c1f11fd001f3fef49d045",
    .p_nonce                = "df7c3bca00396d0c018495d9",
    .p_mac                  = "ee8257795be6a1164d7e1d2d6cac77a7"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_encrypt13) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt message_len=16 ad_len=16 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "3fe7811a8224a1881da34a27e03da86a",
    .p_ciphertext           = "c61155d41495e9fc76060fe7f8c926a2",
    .p_key                  = "82f0d1ddc58123f805541f55a7eab43f56ddfefc06c73d57709df3d5a4aabfb3",
    .p_ad                   = "34325620a392739beeee6c370967d539",
    .p_nonce                = "0c9d74af29ed4406c77a8e4b",
    .p_mac                  = "66d8c881d66370504d2bf00cdb06259e"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_encrypt14) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt message_len=16 ad_len=16 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "3fe7811a8224a1881da34a27e03da86a",
    .p_ciphertext           = "e93165935ac18e3a2845d15fe31a9286",
    .p_key                  = "9a0343f850a6427120f764789ffec6d237447b898fbf51d2182f065d3861497d",
    .p_ad                   = "dbb8226a624520863db6897017b2a4f8",
    .p_nonce                = "3deef6f453dd70d92143adcd",
    .p_mac                  = "f5fc50d18766bc3d9e16dd136d45816b"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_encrypt15) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt message_len=16 ad_len=90 mac_len=13 nonce_len=128 invalid 1",
    .p_plaintext            = "3fe7811a8224a1881da34a27e03da86a",
    .p_ciphertext           = "2975341596f99a22f85a48272d089357",
    .p_key                  = "f65818c25506e571ea4778e71b838ab24d3d6a318670885ded4761c2214ae08c",
    .p_ad                   = "f16c6a6a94a09f7936c718ca182f0e2d8b90de8edecec7257354a02539bee9d232c04b25d6fcc081e8852d834b7044cfec8b0073c62fc676b6d062693b99e791ddc6292bee1f5dc39acc18b06bf5c73a64772195b89659b87275",
    .p_nonce                = "f3d6c665c371db5c8d69ab46ac53eabfd4481a337d005bd0204f5838d770a1bb9868082542b43732d371c7786ab5e3fa217176f959ede631e373488c996c03c00496ff468cc9a2a15700e3aef82ae01f598f703e55da6d6cc9cace3c1f2adf6973af9f7f19dd903d7d0670bc082ec0e97c244426910b6c8e85358eaea8a9807b",
    .p_mac                  = "f260536b28c1220940044c3593"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_encrypt16) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt message_len=16 ad_len=90 mac_len=13 nonce_len=128 valid 1",
    .p_plaintext            = "32f7cc94968659f34f23bda8117f065e",
    .p_ciphertext           = "8700601301096fbfe50b413a8059202d",
    .p_key                  = "fb9cf2d324f5ca351b37d960f314d602d33c01b21be3fcbe0e5a3c55eb9f7d74",
    .p_ad                   = "b0b6f49d881e0af5d879219d7acdd8efd7c2561ee5516de0cc32b61d1c8abd9629bfed1bfdd3cb73e3b39d480af6ea7f9c823f55512a8013ac92b6f3b13efe707dd08c4349e6e15bb2fd6ea4cd6de69b8f1b1c290353ea6ec548",
    .p_nonce                = "53571073c7deffe06b42e3a5cd0d0574ff9ba8afb2fa504420d5fbb1fc6c6aec70b412d40e4e0e0c0abccda8830d3aa6dcb14514f1648b13920a1cf0bc0dfc7ef26d9304f8c1a2858c5ae18993120508ead1f6aa1f7f5ed3f470b203045e9d3d97b493c7d6991061d62555c90bdbd46fa5fe40a4e762361c951f05ee3ce4dd1a",
    .p_mac                  = "247b1c2705c6300785ff514d58"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_encrypt17) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "GCM 256 Encrypt message_len=16 ad_len=90 mac_len=13 nonce_len=128 invalid 2",
    .p_plaintext            = "32f7cc94968659f34f23bda8117f065e",
    .p_ciphertext           = "5bfb289d9832cc5dffce4d1d33357951",
    .p_key                  = "b21ef6860b889efdc04ee1cbae0e84a4f31ac9369b01caa901e873ee6f970839",
    .p_ad                   = "d721291424b17f9ca4f059f208dd7908cdcfd50681641c8dfca185c89e7f1ecf1761bc32b492d8e5ce9cd873cd18a778595fd9b53878634b285f5383a79e01abf6542abffbf4f67347193740f73c3dbac654398027315280e2d5",
    .p_nonce                = "7657f649159a91a7f953e323c236a62f678dab54dd5ae8381419fbcb0ce3d3ec358d46fba5e4dc67cf4157bc6a8f42bc4b1d8624c0b9501f00146e628cecc6580aff6d1bf59f5667f3005b2636f4333930b07f8e814966fce1740919d1f3befa418a81693c0be066b1d17ede09ef36b35b1d908608aeb7ea77d03eec9936736b",
    .p_mac                  = "8eac04b744d91e7b2c5a6ed792"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_decrypt0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "f5a2b27c74355872eb3ef6c5feafaa740e6ae990d9d48c3bd9bb8235e589f010",
    .p_ad                   = "",
    .p_nonce                = "58d2240f580a31c1d24948e9",
    .p_mac                  = "15e051a5e4a5f5da6cea92e2ebee5bac"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_decrypt1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "e5a8123f2e2e007d4e379ba114a2fb66e6613f57c72d4e4f024964053028a831",
    .p_ad                   = "",
    .p_nonce                = "51e43385bf533e168427e1ad",
    .p_mac                  = "38fe845c66e66bdd884c2aecafd280e6"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_decrypt2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=12 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "c5ba3bf9ada560fb0301cd4e0b8b8a46a2aff2400395a72ed5804b3c755c4e59",
    .p_ad                   = "",
    .p_nonce                = "de2b956e704063c000f28bbb",
    .p_mac                  = "04eb9b20fd4548dc71594f20073a45a2"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_decrypt3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=12 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "5765ad16f4dbd4d4e6370dbfa18b8ef5ac37459e39556afaba9b553dea4c1b23",
    .p_ad                   = "",
    .p_nonce                = "59678f262588e4d526d9fc34",
    .p_mac                  = "a68f33ed"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_decrypt4) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=12 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "a29fa93614e9f714c73d3707d1142b2514fa8a14641f79ef14e9bf0e812f49fa",
    .p_ad                   = "",
    .p_nonce                = "68272acf71b1cdad4655dbdc",
    .p_mac                  = "b90e554a"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_decrypt5) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=12 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "717d2a5396ae21560a895a7847f53c47956d155ca4dd2165137d3506ff48a7bf",
    .p_ad                   = "",
    .p_nonce                = "6ccd8086830f586fa3b0e777",
    .p_mac                  = "a618ac6a"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_decrypt6) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt message_len=0 ad_len=16 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "6dfdafd6703c285c01f14fd10a6012862b2af950d4733abb403b2e745b26945d",
    .p_ad                   = "c0d249871992e70302ae008193d1e89f",
    .p_nonce                = "3749d0b3d5bacb71be06ade6",
    .p_mac                  = "4aa4cc69f84ee6ac16d9bfb4e05de500"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_decrypt7) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt message_len=0 ad_len=16 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "033360a08d0b2963ce4dcf807b772ac86ae3e8b8fabb9cd3a636f8ec54365646",
    .p_ad                   = "f9d15fc34984b8f4a7caa29a82b24c52",
    .p_nonce                = "8dcaa63d13a2425395609914",
    .p_mac                  = "42bc9cc93a1c8592bd29c9ef9f907780"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_decrypt8) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt message_len=0 ad_len=16 mac_len=16 nonce_len=12 valid 3",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4f1a5fc8e4689c493ef8e23a653e86e4d4c8972a2338653375b0f36a4feb91d7",
    .p_ad                   = "f09b0fc4e4ccb5e28dfcc0792a6800f3",
    .p_nonce                = "9b8221a631404088218fe487",
    .p_mac                  = "31073f1a0050462a03c7ad0bbefaf93b"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_decrypt9) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt message_len=16 ad_len=0 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "7789b41cb3ee548814ca0b388c10b343",
    .p_ciphertext           = "d2c78110ac7e8f107c0df0570bd7c90c",
    .p_key                  = "4c8ebfe1444ec1b2d503c6986659af2c94fafe945f72c1e8486a5acfedb8a0f8",
    .p_ad                   = "",
    .p_nonce                = "473360e0ad24889959858995",
    .p_mac                  = "c26a379b6d98ef2852ead8ce83a833a7"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_decrypt10) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt message_len=16 ad_len=0 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "697aff2d6b77e5ed6232770e400c1ead",
    .p_ciphertext           = "e0de64302ac2d04048d65a87d2ad09fe",
    .p_key                  = "3934f363fd9f771352c4c7a060682ed03c2864223a1573b3af997e2ababd60ab",
    .p_ad                   = "",
    .p_nonce                = "efe2656d878c586e41c539c4",
    .p_mac                  = "33cbd8d2fb8a3a03e30c1eb1b53c1d99"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_decrypt11) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt message_len=16 ad_len=0 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "697aff2d6b77e5ed6232770e400c1ead",
    .p_ciphertext           = "9f6607d68e22ccf21928db0986be126e",
    .p_key                  = "c997768e2d14e3d38259667a6649079de77beb4543589771e5068e6cd7cd0b14",
    .p_ad                   = "",
    .p_nonce                = "835090aed9552dbdd45277e2",
    .p_mac                  = "f32617f67c574fd9f44ef76ff880ab9f"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_decrypt12) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt message_len=16 ad_len=16 mac_len=16 nonce_len=12 valid 1",
    .p_plaintext            = "85fc3dfad9b5a8d3258e4fc44571bd3b",
    .p_ciphertext           = "426e0efc693b7be1f3018db7ddbb7e4d",
    .p_key                  = "54e352ea1d84bfe64a1011096111fbe7668ad2203d902a01458c3bbd85bfce14",
    .p_ad                   = "7e968d71b50c1f11fd001f3fef49d045",
    .p_nonce                = "df7c3bca00396d0c018495d9",
    .p_mac                  = "ee8257795be6a1164d7e1d2d6cac77a7"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_decrypt13) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt message_len=16 ad_len=16 mac_len=16 nonce_len=12 valid 2",
    .p_plaintext            = "3fe7811a8224a1881da34a27e03da86a",
    .p_ciphertext           = "c61155d41495e9fc76060fe7f8c926a2",
    .p_key                  = "82f0d1ddc58123f805541f55a7eab43f56ddfefc06c73d57709df3d5a4aabfb3",
    .p_ad                   = "34325620a392739beeee6c370967d539",
    .p_nonce                = "0c9d74af29ed4406c77a8e4b",
    .p_mac                  = "66d8c881d66370504d2bf00cdb06259e"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_decrypt14) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt message_len=16 ad_len=16 mac_len=16 nonce_len=12 invalid 1",
    .p_plaintext            = "3fe7811a8224a1881da34a27e03da86a",
    .p_ciphertext           = "e93165935ac18e3a2845d15fe31a9286",
    .p_key                  = "9a0343f850a6427120f764789ffec6d237447b898fbf51d2182f065d3861497d",
    .p_ad                   = "dbb8226a624520863db6897017b2a4f8",
    .p_nonce                = "3deef6f453dd70d92143adcd",
    .p_mac                  = "f5fc50d18766bc3d9e16dd136d45816b"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_decrypt15) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt message_len=16 ad_len=90 mac_len=13 nonce_len=128 invalid 1",
    .p_plaintext            = "3fe7811a8224a1881da34a27e03da86a",
    .p_ciphertext           = "2975341596f99a22f85a48272d089357",
    .p_key                  = "f65818c25506e571ea4778e71b838ab24d3d6a318670885ded4761c2214ae08c",
    .p_ad                   = "f16c6a6a94a09f7936c718ca182f0e2d8b90de8edecec7257354a02539bee9d232c04b25d6fcc081e8852d834b7044cfec8b0073c62fc676b6d062693b99e791ddc6292bee1f5dc39acc18b06bf5c73a64772195b89659b87275",
    .p_nonce                = "f3d6c665c371db5c8d69ab46ac53eabfd4481a337d005bd0204f5838d770a1bb9868082542b43732d371c7786ab5e3fa217176f959ede631e373488c996c03c00496ff468cc9a2a15700e3aef82ae01f598f703e55da6d6cc9cace3c1f2adf6973af9f7f19dd903d7d0670bc082ec0e97c244426910b6c8e85358eaea8a9807b",
    .p_mac                  = "f260536b28c1220940044c3593"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_decrypt16) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt message_len=16 ad_len=90 mac_len=13 nonce_len=128 valid 1",
    .p_plaintext            = "32f7cc94968659f34f23bda8117f065e",
    .p_ciphertext           = "8700601301096fbfe50b413a8059202d",
    .p_key                  = "fb9cf2d324f5ca351b37d960f314d602d33c01b21be3fcbe0e5a3c55eb9f7d74",
    .p_ad                   = "b0b6f49d881e0af5d879219d7acdd8efd7c2561ee5516de0cc32b61d1c8abd9629bfed1bfdd3cb73e3b39d480af6ea7f9c823f55512a8013ac92b6f3b13efe707dd08c4349e6e15bb2fd6ea4cd6de69b8f1b1c290353ea6ec548",
    .p_nonce                = "53571073c7deffe06b42e3a5cd0d0574ff9ba8afb2fa504420d5fbb1fc6c6aec70b412d40e4e0e0c0abccda8830d3aa6dcb14514f1648b13920a1cf0bc0dfc7ef26d9304f8c1a2858c5ae18993120508ead1f6aa1f7f5ed3f470b203045e9d3d97b493c7d6991061d62555c90bdbd46fa5fe40a4e762361c951f05ee3ce4dd1a",
    .p_mac                  = "247b1c2705c6300785ff514d58"
};

// AES GCM - NIST CAVS 14.0 Decrypt with keysize 256 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_gcm_256_decrypt17) =
{
    .p_aead_info            = &g_nrf_crypto_aes_gcm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "GCM 256 Decrypt message_len=16 ad_len=90 mac_len=13 nonce_len=128 invalid 2",
    .p_plaintext            = "32f7cc94968659f34f23bda8117f065e",
    .p_ciphertext           = "5bfb289d9832cc5dffce4d1d33357951",
    .p_key                  = "b21ef6860b889efdc04ee1cbae0e84a4f31ac9369b01caa901e873ee6f970839",
    .p_ad                   = "d721291424b17f9ca4f059f208dd7908cdcfd50681641c8dfca185c89e7f1ecf1761bc32b492d8e5ce9cd873cd18a778595fd9b53878634b285f5383a79e01abf6542abffbf4f67347193740f73c3dbac654398027315280e2d5",
    .p_nonce                = "7657f649159a91a7f953e323c236a62f678dab54dd5ae8381419fbcb0ce3d3ec358d46fba5e4dc67cf4157bc6a8f42bc4b1d8624c0b9501f00146e628cecc6580aff6d1bf59f5667f3005b2636f4333930b07f8e814966fce1740919d1f3befa418a81693c0be066b1d17ede09ef36b35b1d908608aeb7ea77d03eec9936736b",
    .p_mac                  = "8eac04b744d91e7b2c5a6ed792"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_GCM_256)

/*lint -restore */

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_GCM)
