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

/**@brief AES CCM test vectors can be found on NIST web pages.
 *
 * https://csrc.nist.gov
 * https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Algorithm-Validation-Program/documents/mac/ccmtestvectors.zip
 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM)

/*lint -save -e91 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM_128)

// AES CCM - Custom test vector 1 - Invalid behavior test for AES plaintext and AD.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_128_inv_c18) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 message_len=0 ad_len=0",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "",
    .p_nonce                = "00000003020100a0a1a2a3a4a5",
    .p_mac                  = "f48122034d40c898"
};

// AES CCM - Custom test vector 2 - Invalid behavior test for AES buffer authenticated decryption.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_128_inv_c19) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt Invalid ciphertext",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "688c979a61c663d2f066d0c2c0f989806d5f6b61dac384",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "00000003020100a0a1a2a3a4a5",
    .p_mac                  = "17e8d12cfdf926e0"
};

// AES CCM - Custom test vector 3 - Invalid behavior test for AES buffer authenticated decryption.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_128_inv_c20) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
    .crypt_expected_result  = EXPECTED_TO_FAIL,  // Generated plaintext will be incorrect.
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
    .crypt_expected_result  = EXPECTED_TO_PASS,  // Generated plaintext will be correct.
#endif
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt Invalid MAC",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "588c979a61c663d2f066d0c2c0f989806d5f6b61dac384",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "00000003020100a0a1a2a3a4a5",
    .p_mac                  = "27e8d12cfdf926e0"
};

// AES CCM - Custom test vector 4 - Invalid behavior test for AES key.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_128_inv_c21) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt with invalid AES key",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "588c979a61c663d2f066d0c2c0f989806d5f6b61dac384",
    .p_key                  = "d0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "00000003020100a0a1a2a3a4a5",
    .p_mac                  = "17e8d12cfdf926e0"
};

// AES CCM - Custom test vector 5 - Invalid behavior test for AES key.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_128_inv_c22) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt with invalid AES key",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "588c979a61c663d2f066d0c2c0f989806d5f6b61dac384",
    .p_key                  = "d0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "00000003020100a0a1a2a3a4a5",
    .p_mac                  = "17e8d12cfdf926e0"
};

// AES CCM - Custom test vector 6 - Invalid behavior test for AES nonce.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_128_inv_c23) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt with invalid AES nonce",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "588c979a61c663d2f066d0c2c0f989806d5f6b61dac384",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "10000003020100a0a1a2a3a4a5",
    .p_mac                  = "17e8d12cfdf926e0"
};

// AES CCM - Custom test vector 7 - Invalid behavior test for AES nonce.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_128_inv_c24) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt with invalid AES nonce",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "588c979a61c663d2f066d0c2c0f989806d5f6b61dac384",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "10000003020100a0a1a2a3a4a5",
    .p_mac                  = "17e8d12cfdf926e0"
};

// AES CCM - Custom test vector 8
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_128_c8) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 message_len=250 ad_len=8 mac_len=8 nonce_len=13",
    .p_plaintext            = "e96325847ce207c2bf6638a52634075139998888fa0ca1501a4b57ee0767210f1fee065b7ed1e4005fe1c3ec482b2d3cad3d8461d4ab2ba1c73cbe2c65ac8b1e61be3f052ecadf671a3f58601ec0f1ac95367dae85601449f434b9d10757a613784a229aa14bebe6ed07579ec0a513ce7ce799d1e6cd468613f0d3f42e6162680b28f13aeb826572934424459137bd3aabec437bf4fdff007d2675d7fd2195280da092eb1b3549e5457e065cc47349f2c3013b8a4a1938b05c033b2a5b6b477be39b415f4f50beac41f817783bda28f24bab3c8095e1b01e48ce4271cd2a323d1d8c100bb9856f75f63bdee34510de01e32efc0b12e980e9161a",
    .p_ciphertext           = "5f58855b5dce6c704a380d36e1ae8e5fbdd1103edb5afebf74c434fcdcab346f3b2219c0afe77e355464c32626336dc1a7909942548cf23f89e5eba4dba14af9c6c0eca4099a1a962d0fe3d8aaf64f050fc5fdd04228bde4d35a8601a975c0dbdb1d30fea3b7d9648a2666e521195e1e2a2ac811855dca78cc1941709d54cf2a1d8327de81ed4dfe024db1efb8975ba7e61855e3729b11bdba502bc59039b6446957473b362015e88c7403ddd3dfd3e2eb88a4232a32bf5f23d5e91d2abbaae4d549b23e2bacb75cc332edecce7492fc5e009986f925ceef99ea6d72fc97cfb5720302be8d6deaf9a1038c90d1ba3973b7e045cf3330579c7a21",
    .p_key                  = "7e376f9b5227648a72f52df8437c0d36",
    .p_ad                   = "4bf9a35147f3b430",
    .p_nonce                = "43925318850a09c9fb23247cbd",
    .p_mac                  = "fa070211947a3af3"
};

// AES CCM - Custom test vector 9
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_128_c9) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 message_len=16 ad_len=250 mac_len=8 nonce_len=13",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "be32aad42d2165bde54f2780d38f9f199c5182ad3d4b41",
    .p_key                  = "7e376f9b5227648a72f52df8437c0d36",
    .p_ad                   = "0ee0089ec32c7cfa036bd1ab9c23b98cd5d166214d61c719688c6a0ecff705da47ac179cae7eaa3b638ab022a6669729fe47f24679e78488125d989cc7d1bbd8b472b04678b45adde4cca92cd91c4e214257243b163eca04bf643e9df6f7c5bf55aa91bc348154b5c3e9529a56fed8b04e0726b6de3ecc43f499108933e07297bdfcd4d8df2aa9af908083aeccc5c23fe9efbbf5fa9a27ea2923d36188197c87c955fcc5f7e0ebff4b01ebf008a5c93dab12ddfbf65f4225e5a3dfd44e36dbc3ba7891fce514d127e5eb8adb939670b4c5643c46425c614546b90cf0d720203bef4a12ff2d766623c92e134420e3b423b526598f32e8884843bc",
    .p_nonce                = "43925318850a09c9fb23247cbd",
    .p_mac                  = "64f4f8957d6b69e3"
};


// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt_0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4ae701103c63deca5b5a3939d7d05992",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "02209f55"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt_1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4ae701103c63deca5b5a3939d7d05992",
    .p_ad                   = "",
    .p_nonce                = "3796cf51b87266",
    .p_mac                  = "9a04c241"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt_2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4ae701103c63deca5b5a3939d7d05992",
    .p_ad                   = "",
    .p_nonce                = "89ca5a64050f9f",
    .p_mac                  = "f5f915df"
};


// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 15
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt15) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "75d582db43ce9b13ab4b6f7f14341330"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 16
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt16) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3",
    .p_ad                   = "",
    .p_nonce                = "3796cf51b87266",
    .p_mac                  = "3a65e03af37b81d05acc7ec1bc39deb0"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 17
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt17) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3",
    .p_ad                   = "",
    .p_nonce                = "89ca5a64050f9f",
    .p_mac                  = "efc5721e0b9e4c3c90deab0e1d5c11bd"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 30
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt30) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=13 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9403aff859fbb",
    .p_mac                  = "90156f3f"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 31
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt31) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=13 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3",
    .p_ad                   = "",
    .p_nonce                = "a16a2e741f1cd9717285b6d882",
    .p_mac                  = "88909016"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 32
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt32) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=13 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3",
    .p_ad                   = "",
    .p_nonce                = "368f3b8180fd4b851b7b272cb1",
    .p_mac                  = "de547d03"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 45
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt45) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=13 valid",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9403aff859fbb",
    .p_mac                  = "fb04dc5a44c6bb000f2440f5154364b4"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 46
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt46) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=13 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd",
    .p_ad                   = "",
    .p_nonce                = "a16a2e741f1cd9717285b6d882",
    .p_mac                  = "5447075bf42a59b91f08064738b015ab"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 47
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt47) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=13 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd",
    .p_ad                   = "",
    .p_nonce                = "368f3b8180fd4b851b7b272cb1",
    .p_mac                  = "fdc992847f0815fac67aa935b35208ed"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 60
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt60) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=24 ad_len=0 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "a90e8ea44085ced791b2fdb7fd44b5cf0bd7d27718029bb7",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "03e1fa6b"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 61
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt61) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=24 ad_len=0 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "50aafe0578c115c4a8e126ff7b3ccb64dce8ccaa8ceda69f",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd",
    .p_ad                   = "",
    .p_nonce                = "31f8fa25827d48",
    .p_mac                  = "23e5d81c"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 62
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt62) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=24 ad_len=0 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "512ed208bf10d57406537e94d20a5b6e2e9ab0683dfdc685",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd",
    .p_ad                   = "",
    .p_nonce                = "5340ed7752c9ff",
    .p_mac                  = "869a97f0"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 75
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt75) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=24 ad_len=0 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "24ab9eeb0e5508cae80074f1070ee188a637171860881f1f",
    .p_key                  = "197afb02ffbd8f699dacae87094d5243",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "2d9a3fbc210595b7b8b1b41523111a8e"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 76
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt76) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=24 ad_len=0 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "7ebfda6fa5da1dbffd82dc29b875798fbcef8ba0084fbd24",
    .p_key                  = "197afb02ffbd8f699dacae87094d5243",
    .p_ad                   = "",
    .p_nonce                = "31f8fa25827d48",
    .p_mac                  = "63af747cc88a001fa94e060290f209c4"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 77
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt77) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=24 ad_len=0 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "cbf133643851f91ddc7a1e19a0c21990459f2b7728da58f5",
    .p_key                  = "197afb02ffbd8f699dacae87094d5243",
    .p_ad                   = "",
    .p_nonce                = "5340ed7752c9ff",
    .p_mac                  = "cf3b8e6c8aeb5eeb0a5efb3700be45a2"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 120
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt120) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=0 ad_len=32 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "90929a4b0ac65b350ad1591611fe4829",
    .p_ad                   = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "782e4318"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 121
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt121) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=0 ad_len=32 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "90929a4b0ac65b350ad1591611fe4829",
    .p_ad                   = "a2248a882ecbf850daf91933a389e78e81623d233dfd47bf8321361a38f138fe",
    .p_nonce                = "a265480ca88d5f",
    .p_mac                  = "a04f270a"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 122
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt122) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=0 ad_len=32 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "90929a4b0ac65b350ad1591611fe4829",
    .p_ad                   = "2bed1ec06c1ca149d9ffbaf048c474ea2de000eb7950f18d6c25acf6ab3f19b5",
    .p_nonce                = "87ec7423f1ebfc",
    .p_mac                  = "97dfd257"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 135
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt135) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=0 ad_len=32 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "6a798d7c5e1a72b43e20ad5c7b08567b",
    .p_ad                   = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "41b476013f45e4a781f253a6f3b1e530"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 136
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt136) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=0 ad_len=32 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "6a798d7c5e1a72b43e20ad5c7b08567b",
    .p_ad                   = "a2248a882ecbf850daf91933a389e78e81623d233dfd47bf8321361a38f138fe",
    .p_nonce                = "a265480ca88d5f",
    .p_mac                  = "f9f018fcd125822616083fffebc4c8e6"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 137
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt137) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=0 ad_len=32 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "6a798d7c5e1a72b43e20ad5c7b08567b",
    .p_ad                   = "2bed1ec06c1ca149d9ffbaf048c474ea2de000eb7950f18d6c25acf6ab3f19b5",
    .p_nonce                = "87ec7423f1ebfc",
    .p_mac                  = "534cc67c44c877c9c908071ee1082f4c"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 180
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt180) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=24 ad_len=32 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "6be31860ca271ef448de8f8d8b39346daf4b81d7e92d65b3",
    .p_key                  = "f9fdca4ac64fe7f014de0f43039c7571",
    .p_ad                   = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "38f125fa"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 181
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt181) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=24 ad_len=32 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "4cc57a9927a6bc401441870d3193bf89ebd163f5c01501c7",
    .p_key                  = "f9fdca4ac64fe7f014de0f43039c7571",
    .p_ad                   = "5b92394f21ddc3ad49d9b0881b829a5935cb3a4d23e292a62fb66b5e7ab7020e",
    .p_nonce                = "fdd2d6f503c915",
    .p_mac                  = "28a66b69"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 182
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt182) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=24 ad_len=32 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "1f16c6d370fff40c011a243356076b67e905d4672ae2f38f",
    .p_key                  = "f9fdca4ac64fe7f014de0f43039c7571",
    .p_ad                   = "f6468542923be79b4b06dfe70920d57d1da73a9c16f9c9a12d810d7de0d12467",
    .p_nonce                = "27d73d58100054",
    .p_mac                  = "ee2de18c"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 195
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt195) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=24 ad_len=32 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "b351ab96b2e45515254558d5212673ee6c776d42dbca3b51",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728c",
    .p_ad                   = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "2cf3a20b7fd7c49e6e79bef475c2906f"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 196
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt196) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=24 ad_len=32 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "df1a5285caa41b4bb47f6e5ceceba4e82721828d68427a30",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728c",
    .p_ad                   = "5b92394f21ddc3ad49d9b0881b829a5935cb3a4d23e292a62fb66b5e7ab7020e",
    .p_nonce                = "fdd2d6f503c915",
    .p_mac                  = "81d18ca149d6766bfaccec88f194eb5b"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 197
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt197) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=24 ad_len=32 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "04a29fc109dfc626e8297e0f586d0bfaf31260017d95f62d",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728c",
    .p_ad                   = "f6468542923be79b4b06dfe70920d57d1da73a9c16f9c9a12d810d7de0d12467",
    .p_nonce                = "27d73d58100054",
    .p_mac                  = "5eb4f0875dda5ccd9b94026ba49fb34e"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 210
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt210) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=24 ad_len=32 mac_len=4 nonce_len=13 valid",
    .p_plaintext            = "8739b4bea1a099fe547499cbc6d1b13d849b8084c9b6acc5",
    .p_ciphertext           = "934f893824e880f743d196b22d1f340a52608155087bd28a",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728c",
    .p_ad                   = "a16a2e741f1cd9717285b6d882c1fc53655e9773761ad697a7ee6410184c7982",
    .p_nonce                = "5a8aa485c316e9403aff859fbb",
    .p_mac                  = "c25e5329"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 211
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt211) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=24 ad_len=32 mac_len=4 nonce_len=13 invalid 1",
    .p_plaintext            = "8739b4bea1a099fe547499cbc6d1b13d849b8084c9b6acc5",
    .p_ciphertext           = "f43ba9d834ad85dfab3f1c0c27c3441fe4e411a38a261a65",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728c",
    .p_ad                   = "ec6c44a7e94e51a3ca6dee229098391575ec7213c85267fbf7492fdbeee61b10",
    .p_nonce                = "0812757ad0cc4d17c4cfe7a642",
    .p_mac                  = "59b3b3ee"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 128 - Count 212
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_encrypt212) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 128 Encrypt message_len=24 ad_len=32 mac_len=4 nonce_len=13 invalid 2",
    .p_plaintext            = "8739b4bea1a099fe547499cbc6d1b13d849b8084c9b6acc5",
    .p_ciphertext           = "c686eac859a7bae3cce97d0b6527a0a7c8c2b24ece35f437",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728c",
    .p_ad                   = "0923b927b8295c5dfaf67da55e5014293bc8c708fda50af06c1e8aef31cccc86",
    .p_nonce                = "eff510acc1b85f35029cf7dc00",
    .p_mac                  = "0bf6688e"
};



// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt_0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4ae701103c63deca5b5a3939d7d05992",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "02209f55"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt_1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4ae701103c63deca5b5a3939d7d05992",
    .p_ad                   = "",
    .p_nonce                = "3796cf51b87266",
    .p_mac                  = "9a04c241"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt_2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4ae701103c63deca5b5a3939d7d05992",
    .p_ad                   = "",
    .p_nonce                = "89ca5a64050f9f",
    .p_mac                  = "f5f915df"
};


// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 15
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt15) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "75d582db43ce9b13ab4b6f7f14341330"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 16
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt16) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3",
    .p_ad                   = "",
    .p_nonce                = "3796cf51b87266",
    .p_mac                  = "3a65e03af37b81d05acc7ec1bc39deb0"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 17
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt17) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3",
    .p_ad                   = "",
    .p_nonce                = "89ca5a64050f9f",
    .p_mac                  = "efc5721e0b9e4c3c90deab0e1d5c11bd"
};


// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 30
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt30) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=13 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9403aff859fbb",
    .p_mac                  = "90156f3f"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 31
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt31) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=13 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3",
    .p_ad                   = "",
    .p_nonce                = "a16a2e741f1cd9717285b6d882",
    .p_mac                  = "88909016"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 32
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt32) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=13 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3",
    .p_ad                   = "",
    .p_nonce                = "368f3b8180fd4b851b7b272cb1",
    .p_mac                  = "de547d03"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 45
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt45) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=13 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9403aff859fbb",
    .p_mac                  = "fb04dc5a44c6bb000f2440f5154364b4"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 46
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt46) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=13 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd",
    .p_ad                   = "",
    .p_nonce                = "a16a2e741f1cd9717285b6d882",
    .p_mac                  = "5447075bf42a59b91f08064738b015ab"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 47
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt47) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=13 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd",
    .p_ad                   = "",
    .p_nonce                = "368f3b8180fd4b851b7b272cb1",
    .p_mac                  = "fdc992847f0815fac67aa935b35208ed"
};


// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 60
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt60) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=24 ad_len=0 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "a90e8ea44085ced791b2fdb7fd44b5cf0bd7d27718029bb7",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "03e1fa6b"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 61
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt61) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=24 ad_len=0 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "50aafe0578c115c4a8e126ff7b3ccb64dce8ccaa8ceda69f",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd",
    .p_ad                   = "",
    .p_nonce                = "31f8fa25827d48",
    .p_mac                  = "23e5d81c"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 62
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt62) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=24 ad_len=0 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "512ed208bf10d57406537e94d20a5b6e2e9ab0683dfdc685",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd",
    .p_ad                   = "",
    .p_nonce                = "5340ed7752c9ff",
    .p_mac                  = "869a97f0"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 75
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt75) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=24 ad_len=0 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "24ab9eeb0e5508cae80074f1070ee188a637171860881f1f",
    .p_key                  = "197afb02ffbd8f699dacae87094d5243",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "2d9a3fbc210595b7b8b1b41523111a8e"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 76
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt76) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=24 ad_len=0 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "7ebfda6fa5da1dbffd82dc29b875798fbcef8ba0084fbd24",
    .p_key                  = "197afb02ffbd8f699dacae87094d5243",
    .p_ad                   = "",
    .p_nonce                = "31f8fa25827d48",
    .p_mac                  = "63af747cc88a001fa94e060290f209c4"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 77
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt77) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=24 ad_len=0 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "cbf133643851f91ddc7a1e19a0c21990459f2b7728da58f5",
    .p_key                  = "197afb02ffbd8f699dacae87094d5243",
    .p_ad                   = "",
    .p_nonce                = "5340ed7752c9ff",
    .p_mac                  = "cf3b8e6c8aeb5eeb0a5efb3700be45a2"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 120
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt120) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=0 ad_len=32 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "90929a4b0ac65b350ad1591611fe4829",
    .p_ad                   = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "782e4318"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 121
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt121) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=0 ad_len=32 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "90929a4b0ac65b350ad1591611fe4829",
    .p_ad                   = "a2248a882ecbf850daf91933a389e78e81623d233dfd47bf8321361a38f138fe",
    .p_nonce                = "a265480ca88d5f",
    .p_mac                  = "a04f270a"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 122
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt122) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=0 ad_len=32 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "90929a4b0ac65b350ad1591611fe4829",
    .p_ad                   = "2bed1ec06c1ca149d9ffbaf048c474ea2de000eb7950f18d6c25acf6ab3f19b5",
    .p_nonce                = "87ec7423f1ebfc",
    .p_mac                  = "97dfd257"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 135
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt135) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=0 ad_len=32 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "6a798d7c5e1a72b43e20ad5c7b08567b",
    .p_ad                   = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "41b476013f45e4a781f253a6f3b1e530"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 136
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt136) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=0 ad_len=32 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "6a798d7c5e1a72b43e20ad5c7b08567b",
    .p_ad                   = "a2248a882ecbf850daf91933a389e78e81623d233dfd47bf8321361a38f138fe",
    .p_nonce                = "a265480ca88d5f",
    .p_mac                  = "f9f018fcd125822616083fffebc4c8e6"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 137
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt137) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=0 ad_len=32 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "6a798d7c5e1a72b43e20ad5c7b08567b",
    .p_ad                   = "2bed1ec06c1ca149d9ffbaf048c474ea2de000eb7950f18d6c25acf6ab3f19b5",
    .p_nonce                = "87ec7423f1ebfc",
    .p_mac                  = "534cc67c44c877c9c908071ee1082f4c"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 180
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt180) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=24 ad_len=32 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "6be31860ca271ef448de8f8d8b39346daf4b81d7e92d65b3",
    .p_key                  = "f9fdca4ac64fe7f014de0f43039c7571",
    .p_ad                   = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "38f125fa"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 181
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt181) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=24 ad_len=32 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "4cc57a9927a6bc401441870d3193bf89ebd163f5c01501c7",
    .p_key                  = "f9fdca4ac64fe7f014de0f43039c7571",
    .p_ad                   = "5b92394f21ddc3ad49d9b0881b829a5935cb3a4d23e292a62fb66b5e7ab7020e",
    .p_nonce                = "fdd2d6f503c915",
    .p_mac                  = "28a66b69"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 182
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt182) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=24 ad_len=32 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "1f16c6d370fff40c011a243356076b67e905d4672ae2f38f",
    .p_key                  = "f9fdca4ac64fe7f014de0f43039c7571",
    .p_ad                   = "f6468542923be79b4b06dfe70920d57d1da73a9c16f9c9a12d810d7de0d12467",
    .p_nonce                = "27d73d58100054",
    .p_mac                  = "ee2de18c"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 195
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt195) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=24 ad_len=32 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "b351ab96b2e45515254558d5212673ee6c776d42dbca3b51",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728c",
    .p_ad                   = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "2cf3a20b7fd7c49e6e79bef475c2906f"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 196
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt196) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=24 ad_len=32 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "df1a5285caa41b4bb47f6e5ceceba4e82721828d68427a30",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728c",
    .p_ad                   = "5b92394f21ddc3ad49d9b0881b829a5935cb3a4d23e292a62fb66b5e7ab7020e",
    .p_nonce                = "fdd2d6f503c915",
    .p_mac                  = "81d18ca149d6766bfaccec88f194eb5b"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 197
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt197) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=24 ad_len=32 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "04a29fc109dfc626e8297e0f586d0bfaf31260017d95f62d",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728c",
    .p_ad                   = "f6468542923be79b4b06dfe70920d57d1da73a9c16f9c9a12d810d7de0d12467",
    .p_nonce                = "27d73d58100054",
    .p_mac                  = "5eb4f0875dda5ccd9b94026ba49fb34e"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 210
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt210) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=24 ad_len=32 mac_len=4 nonce_len=13 valid 1",
    .p_plaintext            = "8739b4bea1a099fe547499cbc6d1b13d849b8084c9b6acc5",
    .p_ciphertext           = "934f893824e880f743d196b22d1f340a52608155087bd28a",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728c",
    .p_ad                   = "a16a2e741f1cd9717285b6d882c1fc53655e9773761ad697a7ee6410184c7982",
    .p_nonce                = "5a8aa485c316e9403aff859fbb",
    .p_mac                  = "c25e5329"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 211
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt211) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=24 ad_len=32 mac_len=4 nonce_len=13 invalid 1",
    .p_plaintext            = "8739b4bea1a099fe547499cbc6d1b13d849b8084c9b6acc5",
    .p_ciphertext           = "f43ba9d834ad85dfab3f1c0c27c3441fe4e411a38a261a65",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728c",
    .p_ad                   = "ec6c44a7e94e51a3ca6dee229098391575ec7213c85267fbf7492fdbeee61b10",
    .p_nonce                = "0812757ad0cc4d17c4cfe7a642",
    .p_mac                  = "59b3b3ee"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 128 - Count 212
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_128_decrypt212) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_128_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 128 Decrypt message_len=24 ad_len=32 mac_len=4 nonce_len=13 invalid 2",
    .p_plaintext            = "8739b4bea1a099fe547499cbc6d1b13d849b8084c9b6acc5",
    .p_ciphertext           = "c686eac859a7bae3cce97d0b6527a0a7c8c2b24ece35f437",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728c",
    .p_ad                   = "0923b927b8295c5dfaf67da55e5014293bc8c708fda50af06c1e8aef31cccc86",
    .p_nonce                = "eff510acc1b85f35029cf7dc00",
    .p_mac                  = "0bf6688e"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM_128)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM_STAR_128)

// AES CCM STAR - Formal Specification of the CCM* Mode of Operation, September 16, 2004 - 2.5.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_spec) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM STAR 128 message_len=23 ad_len=8 mac_len=8 nonce_len=13",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "1a55a36abb6c610d066b3375649cef10d4664ecad854a8",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "a0a1a2a3a4a5a6a70302010006",
    .p_mac                  = "0a895cc1d8ff9469"
};

// AES CCM STAR - Formal Specification of the CCM* Mode of Operation, September 9, 2005 - 2.6. Not supported in cc310 lib.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_spec_2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM STAR 128 message_len=0 ad_len=26 mac_len=8 nonce_len=13",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "08d0842143010000000048deac020500000055cf000051525354",
    .p_nonce                = "acde4800000000010000000502",
    .p_mac                  = "223bc1ec841ab553"
};

// AES CCM STAR - Formal Specification of the CCM* Mode of Operation, September 9, 2005 - 2.7.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_spec_3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM STAR 128 message_len=4 ad_len=26 mac_len=0 nonce_len=13",
    .p_plaintext            = "61626364",
    .p_ciphertext           = "d43e022b",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "69dc842143020000000048deac010000000048deac0405000000",
    .p_nonce                = "acde4800000000010000000504",
    .p_mac                  = ""
};

// AES CCM STAR - Formal Specification of the CCM* Mode of Operation, September 9, 2005 - 2.8.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_spec_4) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM STAR 128 message_len=1 ad_len=29 mac_len=8 nonce_len=13",
    .p_plaintext            = "ce",
    .p_ciphertext           = "d8",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "2bdc842143020000000048deacffff010000000048deac060500000001",
    .p_nonce                = "acde4800000000010000000506",
    .p_mac                  = "4fde529061f9c6f1"
};

// AES CCM STAR - Custom Test vector.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_level_0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM STAR 128 Security level 0",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "d1fa7145ecd7a327ca3a8b58cd1147e6",
    .p_ad                   = "",
    .p_nonce                = "01f04f8873ea675d98a43a4e00",
    .p_mac                  = ""
};

// AES CCM STAR - Custom Test vector.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_level_1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM STAR 128 Security level 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "d1fa7145ecd7a327ca3a8b58cd1147e6",
    .p_ad                   = "",
    .p_nonce                = "01f04f8873ea675d98a43a4e01",
    .p_mac                  = "fc91d4fb"
};

// AES CCM STAR - Custom Test vector.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_level_2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM STAR 128 Security level 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "d1fa7145ecd7a327ca3a8b58cd1147e6",
    .p_ad                   = "",
    .p_nonce                = "01f04f8873ea675d98a43a4e02",
    .p_mac                  = "dc52d7f5b66dde7f"
};

// AES CCM STAR - Custom Test vector.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_level_3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM STAR 128 Security level 3",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "d1fa7145ecd7a327ca3a8b58cd1147e6",
    .p_ad                   = "",
    .p_nonce                = "01f04f8873ea675d98a43a4e03",
    .p_mac                  = "38e9c4fc3b4129d893bba206c820bb67"
};

// AES CCM STAR - Custom Test vector.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_level_4) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM STAR 128 Security level 4",
    .p_plaintext            = "34cb14f841ef56495779d46b21978724",
    .p_ciphertext           = "d16c46686c549404206596899b935c4b",
    .p_key                  = "d1fa7145ecd7a327ca3a8b58cd1147e6",
    .p_ad                   = "",
    .p_nonce                = "01f04f8873ea675d98a43a4e04",
    .p_mac                  = ""
};

// AES CCM STAR - Custom Test vector.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_level_5) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM STAR 128 Security level 5",
    .p_plaintext            = "34cb14f841ef56495779d46b21978724",
    .p_ciphertext           = "44d1db2dc774a623ada69afd47b952af",
    .p_key                  = "d1fa7145ecd7a327ca3a8b58cd1147e6",
    .p_ad                   = "",
    .p_nonce                = "01f04f8873ea675d98a43a4e05",
    .p_mac                  = "031367fb"
};

// AES CCM STAR - Custom Test vector.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_level_6) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM STAR 128 Security level 6",
    .p_plaintext            = "34cb14f841ef56495779d46b21978724",
    .p_ciphertext           = "96b87289284ed8779e98a5bf55d16f00",
    .p_key                  = "d1fa7145ecd7a327ca3a8b58cd1147e6",
    .p_ad                   = "04ebed593e86388a",
    .p_nonce                = "01f04f8873ea675d98a43a4e06",
    .p_mac                  = "a3fd8b8dae862dc5"
};

// AES CCM STAR - Custom Test vector.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_level_7) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM STAR 128 Security level 7",
    .p_plaintext            = "34cb14f841ef56495779d46b21978724",
    .p_ciphertext           = "604e3fe798363abfb65c29a7dd3ba533",
    .p_key                  = "d1fa7145ecd7a327ca3a8b58cd1147e6",
    .p_ad                   = "04ebed593e86388a",
    .p_nonce                = "01f04f8873ea675d98a43a4e07",
    .p_mac                  = "4eb450488d495c1092f65217aa6b3de8"
};

// AES CCM STAR - Custom Test vector.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM STAR 128 message_len=0 ad_len=8 mac_len=0 nonce_len=13",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "d1fa7145ecd7a327ca3a8b58cd1147e6",
    .p_ad                   = "04ebed593e86388a",
    .p_nonce                = "01f04f8873ea675d98a43a4e00",
    .p_mac                  = ""
};

// AES CCM STAR - Custom Test vector.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM STAR 128 message_len=16 ad_len=8 mac_len=0 nonce_len=13",
    .p_plaintext            = "34cb14f841ef56495779d46b21978724",
    .p_ciphertext           = "d16c46686c549404206596899b935c4b",
    .p_key                  = "d1fa7145ecd7a327ca3a8b58cd1147e6",
    .p_ad                   = "04ebed593e86388a",
    .p_nonce                = "01f04f8873ea675d98a43a4e04",
    .p_mac                  = ""
};

// AES CCM STAR - Custom Test vector.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM STAR 128 message_len=16 ad_len=8 mac_len=4 nonce_len=13",
    .p_plaintext            = "34cb14f841ef56495779d46b21978724",
    .p_ciphertext           = "44d1db2dc774a623ada69afd47b952af",
    .p_key                  = "d1fa7145ecd7a327ca3a8b58cd1147e6",
    .p_ad                   = "04ebed593e86388a",
    .p_nonce                = "01f04f8873ea675d98a43a4e05",
    .p_mac                  = "d01b3d22"
};

// AES CCM STAR - Custom Test vector.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_3) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM STAR 128 message_len=250 ad_len=8 mac_len=8 nonce_len=13",
    .p_plaintext            = "27d37753eea01ca96d03840176e29a3a5c7b5ce8978f293478915ccaaee70eb1159de4bbb4af9ff9e10878bb1d0d40ea63c38693d222c34923f57e640a31e5e099cc5d043407b2c5ad059603a76b9b1498e7b972c924f1211890e887423118c6470c922c6b6179718fa60903ef9a9c6671f413525d79015b936f465704abe06a11dc2b5b2e1bc9b79d4916da565f0efd05cb151a6263c2c2ba4a87e07a2bbb6a4992bc6678fa7ffd8db19d681c61737325cde751932f94cacc977edf7d5bb91a4a486b6959c792deb3adaaedc6a9efba651991cc625760b2d08b555c59967081fd2bb0701ca50a74f0dbca3f1cbc44c682e5629b2876b7587fd0",
    .p_ciphertext           = "85a0112287019297a4e2f5d502a4721e3d3458fa5744ce738e9e49ba4a85f88129146bd808f6322fbcbae5763813081d7ea3bb708571581329d99a5326b0b98725374c7df07d18f378f86a4fb5864b911e1e7a66780741ac6133fcf921505fe94509d277ddb62839fa55dcb3dd76eb50379ba20576c815678a316146d963abd9ca2586591a5cd1b0ddc75ffd1f64cfa3d7ece393a8036668390e5a87d770081f41b2df3c288b97ec756f20170aa7d83fb37b4a0570ec1d8abb1f180017473fd4edcf802ec5ade4eb0bc617675f72a9a531bd5a48e2f4e055975e25de130bc160c93f333bc7d149e932459e015e44bd95a53531e67fac5ec94689",
    .p_key                  = "d1fa7145ecd7a327ca3a8b58cd1147e6",
    .p_ad                   = "04ebed593e86388a",
    .p_nonce                = "01f04f8873ea675d98a43a4e06",
    .p_mac                  = "14a407a361e30073"
};

// AES CCM STAR - Custom Test vector.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_4) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM STAR 128 message_len=16 ad_len=250 mac_len=8 nonce_len=13",
    .p_plaintext            = "34cb14f841ef56495779d46b21978724",
    .p_ciphertext           = "96b87289284ed8779e98a5bf55d16f00",
    .p_key                  = "d1fa7145ecd7a327ca3a8b58cd1147e6",
    .p_ad                   = "690ad6de2661681e8f02b16710372eb9992ea97ab1976a7c2b2b15b08500b307e5d6c7501fff91b71e07d70104c35fbafe9d62dca19005703ff896ea429112aedeb6c9a5c5bc9f7df1d1fae08049ec1f72f050e6758c72c7584ceb89dc86e79112b42d0052b96e5ba155e9b21240c87bdd309ab8f6d88de816c9ddfde6b95af7ee617f379f5235a86a99322a33e5fb9470eb9d19c42e73386098c50b10a7d5d423858f92334ff55143158b00a75ea59ffea0dc107c3d7bfc1f564b2140f8efe2faeb587978bf305a1c6e90077d846426b4543efd4aa916fda72326ef04eebedbdf276dcff15dc1b8eedb3051c57fe9f405366a51c81ed4529cd2",
    .p_nonce                = "01f04f8873ea675d98a43a4e06",
    .p_mac                  = "bc3df5a9240a81d4"
};

// AES CCM STAR - Custom test vector - Invalid behavior test for AES buffer authenticated decryption.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_inv_c0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_NONCE_SIZE,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .p_test_vector_name     = "CCM STAR 128 Invalid nonce_len=12",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "1a55a36abb6c610d066b3375649cef10d4664ecad854a8",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "a0a1a2a3a4a5a6a703020100",
    .p_mac                  = "0a895cc1d8ff9469"
};

// AES CCM STAR - Custom test vector - Invalid behavior test for AES buffer authenticated decryption.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_inv_c1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_NONCE_SIZE,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .p_test_vector_name     = "CCM STAR 128 Invalid nonce_len=14",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "1a55a36abb6c610d066b3375649cef10d4664ecad854a8",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "a0a1a2a3a4a5a6a7030201000606",
    .p_mac                  = "0a895cc1d8ff9469"
};

// AES CCM STAR - Custom test vector - Invalid behavior test for AES buffer authenticated decryption.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_star_128_inv_c2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_ERROR_CRYPTO_INVALID_PARAM,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .p_test_vector_name     = "CCM STAR 128 Invalid CCM STAR Security level field",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "1a55a36abb6c610d066b3375649cef10d4664ecad854a8",
    .p_key                  = "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "a0a1a2a3a4a5a6a70302010007",
    .p_mac                  = "0a895cc1d8ff9469"
};

// AES CCM STAR - Custom Test vector.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_star_128_inv_c4) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM STAR 128 Encrypt with invalid AES key",
    .p_plaintext            = "34cb14f841ef56495779d46b21978724",
    .p_ciphertext           = "96b87289284ed8779e98a5bf55d16f00",
    .p_key                  = "e1fa7145ecd7a327ca3a8b58cd1147e6",
    .p_ad                   = "04ebed593e86388a",
    .p_nonce                = "01f04f8873ea675d98a43a4e06",
    .p_mac                  = "a3fd8b8dae862dc5"
};

// AES CCM STAR - Custom Test vector.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_star_128_inv_c5) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM STAR 128 Decrypt with invalid AES key",
    .p_plaintext            = "34cb14f841ef56495779d46b21978724",
    .p_ciphertext           = "96b87289284ed8779e98a5bf55d16f00",
    .p_key                  = "e1fa7145ecd7a327ca3a8b58cd1147e6",
    .p_ad                   = "04ebed593e86388a",
    .p_nonce                = "01f04f8873ea675d98a43a4e06",
    .p_mac                  = "a3fd8b8dae862dc5"
};

// AES CCM STAR - Custom Test vector.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_star_128_inv_c6) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM STAR 128 Decrypt Invalid MAC",
    .p_plaintext            = "34cb14f841ef56495779d46b21978724",
    .p_ciphertext           = "96b87289284ed8779e98a5bf55d16f00",
    .p_key                  = "d1fa7145ecd7a327ca3a8b58cd1147e6",
    .p_ad                   = "04ebed593e86388a",
    .p_nonce                = "01f04f8873ea675d98a43a4e06",
    .p_mac                  = "b3fd8b8dae862dc5"
};

// AES CCM STAR - Custom Test vector.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_star_128_inv_c7) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_star_128_info,
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM STAR 128 Decrypt Invalid ciphertext",
    .p_plaintext            = "34cb14f841ef56495779d46b21978724",
    .p_ciphertext           = "a6b87289284ed8779e98a5bf55d16f00",
    .p_key                  = "d1fa7145ecd7a327ca3a8b58cd1147e6",
    .p_ad                   = "04ebed593e86388a",
    .p_nonce                = "01f04f8873ea675d98a43a4e06",
    .p_mac                  = "a3fd8b8dae862dc5"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM_128)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM_192)

// AES CCM - Custom test vector 1 - Invalid behavior test for AES plaintext and AD.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_192_inv_c18) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 message_len=0 ad_len=0",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "01fb305afb2d32be292f90033763ab46fe8589bec3a8995c",
    .p_ad                   = "",
    .p_nonce                = "0dde69f96145d07925e0bafcc0",
    .p_mac                  = "c3c30acbddcb17ac"
};

// AES CCM - Custom test vector 2 - Invalid behavior test for AES buffer authenticated decryption.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_192_inv_c19) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt Invalid ciphertext",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "99b683088b1c9588e1bfd295ed496144c594508be17021",
    .p_key                  = "01fb305afb2d32be292f90033763ab46fe8589bec3a8995c",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "0dde69f96145d07925e0bafcc0",
    .p_mac                  = "e2e7c52772fd89a7"
};

// AES CCM - Custom test vector 3 - Invalid behavior test for AES buffer authenticated decryption.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_192_inv_c20) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
    .crypt_expected_result  = EXPECTED_TO_FAIL,  // Generated plaintext will be incorrect.
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
    .crypt_expected_result  = EXPECTED_TO_PASS,  // Generated plaintext will be correct.
#endif
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt Invalid MAC",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "89b683088b1c9588e1bfd295ed496144c594508be17021",
    .p_key                  = "01fb305afb2d32be292f90033763ab46fe8589bec3a8995c",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "0dde69f96145d07925e0bafcc0",
    .p_mac                  = "f2e7c52772fd89a7"
};

// AES CCM - Custom test vector 4 - Invalid behavior test for AES key.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_192_inv_c21) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt with invalid AES key",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "89b683088b1c9588e1bfd295ed496144c594508be17021",
    .p_key                  = "11fb305afb2d32be292f90033763ab46fe8589bec3a8995c",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "0dde69f96145d07925e0bafcc0",
    .p_mac                  = "e2e7c52772fd89a7"
};

// AES CCM - Custom test vector 5 - Invalid behavior test for AES key.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_192_inv_c22) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt with invalid AES key",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "89b683088b1c9588e1bfd295ed496144c594508be17021",
    .p_key                  = "11fb305afb2d32be292f90033763ab46fe8589bec3a8995c",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "0dde69f96145d07925e0bafcc0",
    .p_mac                  = "e2e7c52772fd89a7"
};

// AES CCM - Custom test vector 6 - Invalid behavior test for AES nonce.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_192_inv_c23) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt with invalid AES nonce",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "89b683088b1c9588e1bfd295ed496144c594508be17021",
    .p_key                  = "01fb305afb2d32be292f90033763ab46fe8589bec3a8995c",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "1dde69f96145d07925e0bafcc0",
    .p_mac                  = "e2e7c52772fd89a7"
};

// AES CCM - Custom test vector 7 - Invalid behavior test for AES nonce.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_192_inv_c24) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt with invalid AES nonce",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "89b683088b1c9588e1bfd295ed496144c594508be17021",
    .p_key                  = "01fb305afb2d32be292f90033763ab46fe8589bec3a8995c",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "1dde69f96145d07925e0bafcc0",
    .p_mac                  = "e2e7c52772fd89a7"
};

// AES CCM - Custom test vector 8
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_192_c8) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 message_len=250 ad_len=8 mac_len=8",
    .p_plaintext            = "e96325847ce207c2bf6638a52634075139998888fa0ca1501a4b57ee0767210f1fee065b7ed1e4005fe1c3ec482b2d3cad3d8461d4ab2ba1c73cbe2c65ac8b1e61be3f052ecadf671a3f58601ec0f1ac95367dae85601449f434b9d10757a613784a229aa14bebe6ed07579ec0a513ce7ce799d1e6cd468613f0d3f42e6162680b28f13aeb826572934424459137bd3aabec437bf4fdff007d2675d7fd2195280da092eb1b3549e5457e065cc47349f2c3013b8a4a1938b05c033b2a5b6b477be39b415f4f50beac41f817783bda28f24bab3c8095e1b01e48ce4271cd2a323d1d8c100bb9856f75f63bdee34510de01e32efc0b12e980e9161a",
    .p_ciphertext           = "cb0bf54c813d8565f18b38c36ccfe0ce12e2105229a0ee76219e8b1e84beac3bbfdde2c04ee870b78d519d696f3b7e0fc7365448dababae43fa3a15578c5ef5d670a310b492075dccef10afb36abe74f58d3c884127119e7924f007b0d3f6a4582774567b9a15e647a1845ffdaaec4933c1c44f7f2c0abdafcff9be04b12f9dbcf64fb34477ac2b40cf9144bb0602a65773dda5f4a90da33904cbcb675590595fc9eeae690aa16f18f52b75e42f4f1481868934b47df8b6521fdf04fa770c92b8e836b5fba653853ebe6429e9f31f65ede4ebde133fca9fb0130951db79c0441664ea8293abbbdc6ab20ebd78458be42beb72f1dc44aaf7f7b36",
    .p_key                  = "db14cb8ad97209986d73e45d23bff3a1546f9e2108370780",
    .p_ad                   = "4bf9a35147f3b430",
    .p_nonce                = "43925318850a09c9fb23247cbd",
    .p_mac                  = "b90db57cea414f03"
};

// AES CCM - Custom test vector 9
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_192_c9) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 message_len=16 ad_len=250 mac_len=8 nonce_len=13",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "2a61dac3f1d28ca85efc12755eeef188336282c1cfb151",
    .p_key                  = "db14cb8ad97209986d73e45d23bff3a1546f9e2108370780",
    .p_ad                   = "0ee0089ec32c7cfa036bd1ab9c23b98cd5d166214d61c719688c6a0ecff705da47ac179cae7eaa3b638ab022a6669729fe47f24679e78488125d989cc7d1bbd8b472b04678b45adde4cca92cd91c4e214257243b163eca04bf643e9df6f7c5bf55aa91bc348154b5c3e9529a56fed8b04e0726b6de3ecc43f499108933e07297bdfcd4d8df2aa9af908083aeccc5c23fe9efbbf5fa9a27ea2923d36188197c87c955fcc5f7e0ebff4b01ebf008a5c93dab12ddfbf65f4225e5a3dfd44e36dbc3ba7891fce514d127e5eb8adb939670b4c5643c46425c614546b90cf0d720203bef4a12ff2d766623c92e134420e3b423b526598f32e8884843bc",
    .p_nonce                = "43925318850a09c9fb23247cbd",
    .p_mac                  = "800cace2bac521f8"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "c98ad7f38b2c7e970c9b965ec87a08208384718f78206c6c",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "9d4b7f3b"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "c98ad7f38b2c7e970c9b965ec87a08208384718f78206c6c",
    .p_ad                   = "",
    .p_nonce                = "3796cf51b87266",
    .p_mac                  = "80745de9"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "c98ad7f38b2c7e970c9b965ec87a08208384718f78206c6c",
    .p_ad                   = "",
    .p_nonce                = "89ca5a64050f9f",
    .p_mac                  = "2f6fa823"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 15
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt15) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3d3ad1bccf9282a65",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "17223038fa99d53681ca1beabe78d1b4"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 16
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt16) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3d3ad1bccf9282a65",
    .p_ad                   = "",
    .p_nonce                = "3796cf51b87266",
    .p_mac                  = "d0e1eeef4d2a264536bb1c2c1bde7c35"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 17
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt17) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3d3ad1bccf9282a65",
    .p_ad                   = "",
    .p_nonce                = "89ca5a64050f9f",
    .p_mac                  = "81d587f8673fd514c23172af7fb7523d"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 30
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt30) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=13 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3d3ad1bccf9282a65",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9403aff859fbb",
    .p_mac                  = "fe69ed84"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 31
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt31) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=13 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3d3ad1bccf9282a65",
    .p_ad                   = "",
    .p_nonce                = "a16a2e741f1cd9717285b6d882",
    .p_mac                  = "db7ffc82"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 32
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt32) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=13 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3d3ad1bccf9282a65",
    .p_ad                   = "",
    .p_nonce                = "368f3b8180fd4b851b7b272cb1",
    .p_mac                  = "7a677329"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 45
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt45) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=13 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd4094afcb205393fa",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9403aff859fbb",
    .p_mac                  = "0c66a8e547ed4f8c2c9a9a1eb5d455b9"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 46
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt46) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=13 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd4094afcb205393fa",
    .p_ad                   = "",
    .p_nonce                = "a16a2e741f1cd9717285b6d882",
    .p_mac                  = "38757b3a61a4dc97ca3ab88bf1240695"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 47
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt47) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=13 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd4094afcb205393fa",
    .p_ad                   = "",
    .p_nonce                = "368f3b8180fd4b851b7b272cb1",
    .p_mac                  = "11875da4445d92391d0fab5f3625497b"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 60
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt60) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=24 ad_len=0 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "411986d04d6463100bff03f7d0bde7ea2c3488784378138c",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd4094afcb205393fa",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "ddc93a54"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 61
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt61) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=24 ad_len=0 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "32b649ab56162e55d4148a1292d6a225a988eb1308298273",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd4094afcb205393fa",
    .p_ad                   = "",
    .p_nonce                = "31f8fa25827d48",
    .p_mac                  = "b6889036"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 62
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt62) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=24 ad_len=0 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "a963c3568ab413b174cd95cc1e3ca61ee181292bebdb2817",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd4094afcb205393fa",
    .p_ad                   = "",
    .p_nonce                = "5340ed7752c9ff",
    .p_mac                  = "9b4de35f"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 75
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt75) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=24 ad_len=0 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "cba4b4aeb85f0492fd8d905c4a6d8233139833373ef188a8",
    .p_key                  = "197afb02ffbd8f699dacae87094d524324576b99844f75e1",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "c5a5ebecf7ac8607fe412189e83d9d20"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 76
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt76) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=24 ad_len=0 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "ca62713728b5c9d652504b0ae8fd4fee5d297ee6a8d19cb6",
    .p_key                  = "197afb02ffbd8f699dacae87094d524324576b99844f75e1",
    .p_ad                   = "",
    .p_nonce                = "31f8fa25827d48",
    .p_mac                  = "e699f15f14d34dcaf9ba8ed4b877c97d"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 77
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt77) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=24 ad_len=0 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "93012c0a5f6f1025b8c4a5d897d3eea0b1c77be8000c9e59",
    .p_key                  = "197afb02ffbd8f699dacae87094d524324576b99844f75e1",
    .p_ad                   = "",
    .p_nonce                = "5340ed7752c9ff",
    .p_mac                  = "f3b8899459788c58794f177cfd838f35"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 120
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt120) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=0 ad_len=32 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "90929a4b0ac65b350ad1591611fe48297e03956f6083e451",
    .p_ad                   = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "1d089a5f"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 121
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt121) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=0 ad_len=32 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "90929a4b0ac65b350ad1591611fe48297e03956f6083e451",
    .p_ad                   = "a2248a882ecbf850daf91933a389e78e81623d233dfd47bf8321361a38f138fe",
    .p_nonce                = "a265480ca88d5f",
    .p_mac                  = "2f46022a"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 122
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt122) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=0 ad_len=32 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "90929a4b0ac65b350ad1591611fe48297e03956f6083e451",
    .p_ad                   = "2bed1ec06c1ca149d9ffbaf048c474ea2de000eb7950f18d6c25acf6ab3f19b5",
    .p_nonce                = "87ec7423f1ebfc",
    .p_mac                  = "67dc4693"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 135
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt135) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=0 ad_len=32 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "6a798d7c5e1a72b43e20ad5c7b08567b12ab744b61c070e2",
    .p_ad                   = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "5280a2137fee3deefcfe9b63a1199fb3"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 136
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt136) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=0 ad_len=32 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "6a798d7c5e1a72b43e20ad5c7b08567b12ab744b61c070e2",
    .p_ad                   = "a2248a882ecbf850daf91933a389e78e81623d233dfd47bf8321361a38f138fe",
    .p_nonce                = "a265480ca88d5f",
    .p_mac                  = "d40a7318c5f2d82f838c0beeefe0d598"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 137
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt137) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=0 ad_len=32 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "6a798d7c5e1a72b43e20ad5c7b08567b12ab744b61c070e2",
    .p_ad                   = "2bed1ec06c1ca149d9ffbaf048c474ea2de000eb7950f18d6c25acf6ab3f19b5",
    .p_nonce                = "87ec7423f1ebfc",
    .p_mac                  = "7551978bc9592bf9e294b4984c5862bb"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 180
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt180) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=24 ad_len=32 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "9f6ca4af9b159148c889a6584d1183ea26e2614874b05045",
    .p_key                  = "f9fdca4ac64fe7f014de0f43039c757194d544ce5d15eed4",
    .p_ad                   = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "75dea8d1"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 181
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt181) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=24 ad_len=32 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "84d8212e9cfc2121252baa3b065b1edcf50497b9594db1eb",
    .p_key                  = "f9fdca4ac64fe7f014de0f43039c757194d544ce5d15eed4",
    .p_ad                   = "5b92394f21ddc3ad49d9b0881b829a5935cb3a4d23e292a62fb66b5e7ab7020e",
    .p_nonce                = "fdd2d6f503c915",
    .p_mac                  = "d7965825"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 182
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt182) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=24 ad_len=32 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "5f60a8f867a33b2077ecc69863b295c3c6aeae7d7cade7f8",
    .p_key                  = "f9fdca4ac64fe7f014de0f43039c757194d544ce5d15eed4",
    .p_ad                   = "f6468542923be79b4b06dfe70920d57d1da73a9c16f9c9a12d810d7de0d12467",
    .p_nonce                = "27d73d58100054",
    .p_mac                  = "f7f796fe"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 195
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt195) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=24 ad_len=32 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "6aab64c4787599d8f213446beadb16e08dba60e97f56dbd1",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728ccd4b3e8cdd2ab33d",
    .p_ad                   = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "4d1d980d6fe0fb44b421992662b97975"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 196
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt196) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=24 ad_len=32 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "4980b2ee49b1aaf393175f5ab9bae95ec7904557dfa20660",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728ccd4b3e8cdd2ab33d",
    .p_ad                   = "5b92394f21ddc3ad49d9b0881b829a5935cb3a4d23e292a62fb66b5e7ab7020e",
    .p_nonce                = "fdd2d6f503c915",
    .p_mac                  = "3c51d36c826f01384100886198a7f6a3"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 197
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt197) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=24 ad_len=32 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "86a02bdd6ae733eee26f8eab898b336105978b5bbd6df781",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728ccd4b3e8cdd2ab33d",
    .p_ad                   = "f6468542923be79b4b06dfe70920d57d1da73a9c16f9c9a12d810d7de0d12467",
    .p_nonce                = "27d73d58100054",
    .p_mac                  = "758a111aae4f735b7dd4d9802f2a8406"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 210
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt210) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=24 ad_len=32 mac_len=4 nonce_len=13 valid 1",
    .p_plaintext            = "8739b4bea1a099fe547499cbc6d1b13d849b8084c9b6acc5",
    .p_ciphertext           = "16e543d0e20615ff0df15acd9927ddfe40668a54bb854ccc",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728ccd4b3e8cdd2ab33d",
    .p_ad                   = "a16a2e741f1cd9717285b6d882c1fc53655e9773761ad697a7ee6410184c7982",
    .p_nonce                = "5a8aa485c316e9403aff859fbb",
    .p_mac                  = "c25e9fce"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 211
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt211) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=24 ad_len=32 mac_len=4 nonce_len=13 invalid 1",
    .p_plaintext            = "8739b4bea1a099fe547499cbc6d1b13d849b8084c9b6acc5",
    .p_ciphertext           = "df35b109caf690656ae278bbd8f8bba687a2ce11b105dae9",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728ccd4b3e8cdd2ab33d",
    .p_ad                   = "ec6c44a7e94e51a3ca6dee229098391575ec7213c85267fbf7492fdbeee61b10",
    .p_nonce                = "0812757ad0cc4d17c4cfe7a642",
    .p_mac                  = "8ecedb3e"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 192 - Count 212
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_encrypt212) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 192 Encrypt message_len=24 ad_len=32 mac_len=4 nonce_len=13 invalid 2",
    .p_plaintext            = "8739b4bea1a099fe547499cbc6d1b13d849b8084c9b6acc5",
    .p_ciphertext           = "7075da2291e2cb527eb926ed08d8020c5f8f0f2d4a6a4745",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728ccd4b3e8cdd2ab33d",
    .p_ad                   = "0923b927b8295c5dfaf67da55e5014293bc8c708fda50af06c1e8aef31cccc86",
    .p_nonce                = "eff510acc1b85f35029cf7dc00",
    .p_mac                  = "728da544"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "c98ad7f38b2c7e970c9b965ec87a08208384718f78206c6c",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "9d4b7f3b"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "c98ad7f38b2c7e970c9b965ec87a08208384718f78206c6c",
    .p_ad                   = "",
    .p_nonce                = "3796cf51b87266",
    .p_mac                  = "80745de9"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "c98ad7f38b2c7e970c9b965ec87a08208384718f78206c6c",
    .p_ad                   = "",
    .p_nonce                = "89ca5a64050f9f",
    .p_mac                  = "2f6fa823"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 15
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt15) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3d3ad1bccf9282a65",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "17223038fa99d53681ca1beabe78d1b4"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 16
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt16) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3d3ad1bccf9282a65",
    .p_ad                   = "",
    .p_nonce                = "3796cf51b87266",
    .p_mac                  = "d0e1eeef4d2a264536bb1c2c1bde7c35"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 17
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt17) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3d3ad1bccf9282a65",
    .p_ad                   = "",
    .p_nonce                = "89ca5a64050f9f",
    .p_mac                  = "81d587f8673fd514c23172af7fb7523d"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 30
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt30) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=13 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3d3ad1bccf9282a65",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9403aff859fbb",
    .p_mac                  = "fe69ed84"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 31
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt31) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=13 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3d3ad1bccf9282a65",
    .p_ad                   = "",
    .p_nonce                = "a16a2e741f1cd9717285b6d882",
    .p_mac                  = "db7ffc82"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 32
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt32) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=13 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4bb3c4a4f893ad8c9bdc833c325d62b3d3ad1bccf9282a65",
    .p_ad                   = "",
    .p_nonce                = "368f3b8180fd4b851b7b272cb1",
    .p_mac                  = "7a677329"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 45
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt45) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=13 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd4094afcb205393fa",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9403aff859fbb",
    .p_mac                  = "0c66a8e547ed4f8c2c9a9a1eb5d455b9"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 46
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt46) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=13 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd4094afcb205393fa",
    .p_ad                   = "",
    .p_nonce                = "a16a2e741f1cd9717285b6d882",
    .p_mac                  = "38757b3a61a4dc97ca3ab88bf1240695"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 47
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt47) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=13 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd4094afcb205393fa",
    .p_ad                   = "",
    .p_nonce                = "368f3b8180fd4b851b7b272cb1",
    .p_mac                  = "11875da4445d92391d0fab5f3625497b"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 60
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt60) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=24 ad_len=0 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "411986d04d6463100bff03f7d0bde7ea2c3488784378138c",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd4094afcb205393fa",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "ddc93a54"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 61
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt61) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=24 ad_len=0 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "32b649ab56162e55d4148a1292d6a225a988eb1308298273",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd4094afcb205393fa",
    .p_ad                   = "",
    .p_nonce                = "31f8fa25827d48",
    .p_mac                  = "b6889036"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 62
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt62) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=24 ad_len=0 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "a963c3568ab413b174cd95cc1e3ca61ee181292bebdb2817",
    .p_key                  = "19ebfde2d5468ba0a3031bde629b11fd4094afcb205393fa",
    .p_ad                   = "",
    .p_nonce                = "5340ed7752c9ff",
    .p_mac                  = "9b4de35f"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 75
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt75) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=24 ad_len=0 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "cba4b4aeb85f0492fd8d905c4a6d8233139833373ef188a8",
    .p_key                  = "197afb02ffbd8f699dacae87094d524324576b99844f75e1",
    .p_ad                   = "",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "c5a5ebecf7ac8607fe412189e83d9d20"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 76
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt76) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=24 ad_len=0 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "ca62713728b5c9d652504b0ae8fd4fee5d297ee6a8d19cb6",
    .p_key                  = "197afb02ffbd8f699dacae87094d524324576b99844f75e1",
    .p_ad                   = "",
    .p_nonce                = "31f8fa25827d48",
    .p_mac                  = "e699f15f14d34dcaf9ba8ed4b877c97d"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 77
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt77) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=24 ad_len=0 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    .p_ciphertext           = "93012c0a5f6f1025b8c4a5d897d3eea0b1c77be8000c9e59",
    .p_key                  = "197afb02ffbd8f699dacae87094d524324576b99844f75e1",
    .p_ad                   = "",
    .p_nonce                = "5340ed7752c9ff",
    .p_mac                  = "f3b8899459788c58794f177cfd838f35"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 120
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt120) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=0 ad_len=32 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "90929a4b0ac65b350ad1591611fe48297e03956f6083e451",
    .p_ad                   = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "1d089a5f"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 121
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt121) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=0 ad_len=32 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "90929a4b0ac65b350ad1591611fe48297e03956f6083e451",
    .p_ad                   = "a2248a882ecbf850daf91933a389e78e81623d233dfd47bf8321361a38f138fe",
    .p_nonce                = "a265480ca88d5f",
    .p_mac                  = "2f46022a"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 122
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt122) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=0 ad_len=32 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "90929a4b0ac65b350ad1591611fe48297e03956f6083e451",
    .p_ad                   = "2bed1ec06c1ca149d9ffbaf048c474ea2de000eb7950f18d6c25acf6ab3f19b5",
    .p_nonce                = "87ec7423f1ebfc",
    .p_mac                  = "67dc4693"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 135
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt135) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=0 ad_len=32 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "6a798d7c5e1a72b43e20ad5c7b08567b12ab744b61c070e2",
    .p_ad                   = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "5280a2137fee3deefcfe9b63a1199fb3"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 136
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt136) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=0 ad_len=32 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "6a798d7c5e1a72b43e20ad5c7b08567b12ab744b61c070e2",
    .p_ad                   = "a2248a882ecbf850daf91933a389e78e81623d233dfd47bf8321361a38f138fe",
    .p_nonce                = "a265480ca88d5f",
    .p_mac                  = "d40a7318c5f2d82f838c0beeefe0d598"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 137
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt137) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=0 ad_len=32 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "6a798d7c5e1a72b43e20ad5c7b08567b12ab744b61c070e2",
    .p_ad                   = "2bed1ec06c1ca149d9ffbaf048c474ea2de000eb7950f18d6c25acf6ab3f19b5",
    .p_nonce                = "87ec7423f1ebfc",
    .p_mac                  = "7551978bc9592bf9e294b4984c5862bb"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 180
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt180) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=24 ad_len=32 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "9f6ca4af9b159148c889a6584d1183ea26e2614874b05045",
    .p_key                  = "f9fdca4ac64fe7f014de0f43039c757194d544ce5d15eed4",
    .p_ad                   = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "75dea8d1"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 181
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt181) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=24 ad_len=32 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "84d8212e9cfc2121252baa3b065b1edcf50497b9594db1eb",
    .p_key                  = "f9fdca4ac64fe7f014de0f43039c757194d544ce5d15eed4",
    .p_ad                   = "5b92394f21ddc3ad49d9b0881b829a5935cb3a4d23e292a62fb66b5e7ab7020e",
    .p_nonce                = "fdd2d6f503c915",
    .p_mac                  = "d7965825"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 182
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt182) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=24 ad_len=32 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "5f60a8f867a33b2077ecc69863b295c3c6aeae7d7cade7f8",
    .p_key                  = "f9fdca4ac64fe7f014de0f43039c757194d544ce5d15eed4",
    .p_ad                   = "f6468542923be79b4b06dfe70920d57d1da73a9c16f9c9a12d810d7de0d12467",
    .p_nonce                = "27d73d58100054",
    .p_mac                  = "f7f796fe"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 195
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt195) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=24 ad_len=32 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "6aab64c4787599d8f213446beadb16e08dba60e97f56dbd1",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728ccd4b3e8cdd2ab33d",
    .p_ad                   = "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c",
    .p_nonce                = "5a8aa485c316e9",
    .p_mac                  = "4d1d980d6fe0fb44b421992662b97975"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 196
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt196) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=24 ad_len=32 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "4980b2ee49b1aaf393175f5ab9bae95ec7904557dfa20660",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728ccd4b3e8cdd2ab33d",
    .p_ad                   = "5b92394f21ddc3ad49d9b0881b829a5935cb3a4d23e292a62fb66b5e7ab7020e",
    .p_nonce                = "fdd2d6f503c915",
    .p_mac                  = "3c51d36c826f01384100886198a7f6a3"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 197
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt197) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=24 ad_len=32 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    .p_ciphertext           = "86a02bdd6ae733eee26f8eab898b336105978b5bbd6df781",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728ccd4b3e8cdd2ab33d",
    .p_ad                   = "f6468542923be79b4b06dfe70920d57d1da73a9c16f9c9a12d810d7de0d12467",
    .p_nonce                = "27d73d58100054",
    .p_mac                  = "758a111aae4f735b7dd4d9802f2a8406"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 210
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt210) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=24 ad_len=32 mac_len=4 nonce_len=13 valid 1",
    .p_plaintext            = "8739b4bea1a099fe547499cbc6d1b13d849b8084c9b6acc5",
    .p_ciphertext           = "16e543d0e20615ff0df15acd9927ddfe40668a54bb854ccc",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728ccd4b3e8cdd2ab33d",
    .p_ad                   = "a16a2e741f1cd9717285b6d882c1fc53655e9773761ad697a7ee6410184c7982",
    .p_nonce                = "5a8aa485c316e9403aff859fbb",
    .p_mac                  = "c25e9fce"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 211
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt211) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=24 ad_len=32 mac_len=4 nonce_len=13 invalid 1",
    .p_plaintext            = "8739b4bea1a099fe547499cbc6d1b13d849b8084c9b6acc5",
    .p_ciphertext           = "df35b109caf690656ae278bbd8f8bba687a2ce11b105dae9",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728ccd4b3e8cdd2ab33d",
    .p_ad                   = "ec6c44a7e94e51a3ca6dee229098391575ec7213c85267fbf7492fdbeee61b10",
    .p_nonce                = "0812757ad0cc4d17c4cfe7a642",
    .p_mac                  = "8ecedb3e"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 192 - Count 212
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_192_decrypt212) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_192_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 192 Decrypt message_len=24 ad_len=32 mac_len=4 nonce_len=13 invalid 2",
    .p_plaintext            = "8739b4bea1a099fe547499cbc6d1b13d849b8084c9b6acc5",
    .p_ciphertext           = "7075da2291e2cb527eb926ed08d8020c5f8f0f2d4a6a4745",
    .p_key                  = "a7aa635ea51b0bb20a092bd5573e728ccd4b3e8cdd2ab33d",
    .p_ad                   = "0923b927b8295c5dfaf67da55e5014293bc8c708fda50af06c1e8aef31cccc86",
    .p_nonce                = "eff510acc1b85f35029cf7dc00",
    .p_mac                  = "728da544"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM_192)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM_256)

// AES CCM - Custom test vector 1 - Invalid behavior test for AES plaintext and AD.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_256_inv_c18) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 message_len=0 ad_len=0",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "4a44110f37e0f8761aa1adc2eeae1c741568036b7eb3b3b8a7ae8ea41a353f5b",
    .p_ad                   = "",
    .p_nonce                = "c34791ac7cc811e6d6642a7274",
    .p_mac                  = "16b4db7258906488"
};

// AES CCM - Custom test vector 2 - Invalid behavior test for AES buffer authenticated decryption.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_256_inv_c19) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt Invalid ciphertext",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "c7877d560c6f604032ff9a67b99a8ca3697a76ee40bc73",
    .p_key                  = "4a44110f37e0f8761aa1adc2eeae1c741568036b7eb3b3b8a7ae8ea41a353f5b",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "c34791ac7cc811e6d6642a7274",
    .p_mac                  = "b9f4e4d93b978141"
};

// AES CCM - Custom test vector 3 - Invalid behavior test for AES buffer authenticated decryption.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_256_inv_c20) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
    .crypt_expected_result  = EXPECTED_TO_FAIL,  // Generated plaintext will be incorrect.
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
    .crypt_expected_result  = EXPECTED_TO_PASS,  // Generated plaintext will be correct.
#endif
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt Invalid MAC",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "b7877d560c6f604032ff9a67b99a8ca3697a76ee40bc73",
    .p_key                  = "4a44110f37e0f8761aa1adc2eeae1c741568036b7eb3b3b8a7ae8ea41a353f5b",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "c34791ac7cc811e6d6642a7274",
    .p_mac                  = "c9f4e4d93b978141"
};

// AES CCM - Custom test vector 4 - Invalid behavior test for AES key.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_256_inv_c21) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt with invalid AES key",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "b7877d560c6f604032ff9a67b99a8ca3697a76ee40bc73",
    .p_key                  = "5a44110f37e0f8761aa1adc2eeae1c741568036b7eb3b3b8a7ae8ea41a353f5b",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "c34791ac7cc811e6d6642a7274",
    .p_mac                  = "b9f4e4d93b978141"
};

// AES CCM - Custom test vector 5 - Invalid behavior test for AES key.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_256_inv_c22) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt with invalid AES key",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "b7877d560c6f604032ff9a67b99a8ca3697a76ee40bc73",
    .p_key                  = "5a44110f37e0f8761aa1adc2eeae1c741568036b7eb3b3b8a7ae8ea41a353f5b",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "c34791ac7cc811e6d6642a7274",
    .p_mac                  = "b9f4e4d93b978141"
};

// AES CCM - Custom test vector 6 - Invalid behavior test for AES nonce.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_256_inv_c23) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt with invalid AES nonce",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "b7877d560c6f604032ff9a67b99a8ca3697a76ee40bc73",
    .p_key                  = "4a44110f37e0f8761aa1adc2eeae1c741568036b7eb3b3b8a7ae8ea41a353f5b",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "d34791ac7cc811e6d6642a7274",
    .p_mac                  = "b9f4e4d93b978141"
};

// AES CCM - Custom test vector 7 - Invalid behavior test for AES nonce.
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_ccm_256_inv_c24) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt with invalid AES nonce",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "b7877d560c6f604032ff9a67b99a8ca3697a76ee40bc73",
    .p_key                  = "4a44110f37e0f8761aa1adc2eeae1c741568036b7eb3b3b8a7ae8ea41a353f5b",
    .p_ad                   = "0001020304050607",
    .p_nonce                = "d34791ac7cc811e6d6642a7274",
    .p_mac                  = "b9f4e4d93b978141"
};

// AES CCM - Custom test vector 8
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_256_c8) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 message_len=250 ad_len=8 mac_len=8",
    .p_plaintext            = "e96325847ce207c2bf6638a52634075139998888fa0ca1501a4b57ee0767210f1fee065b7ed1e4005fe1c3ec482b2d3cad3d8461d4ab2ba1c73cbe2c65ac8b1e61be3f052ecadf671a3f58601ec0f1ac95367dae85601449f434b9d10757a613784a229aa14bebe6ed07579ec0a513ce7ce799d1e6cd468613f0d3f42e6162680b28f13aeb826572934424459137bd3aabec437bf4fdff007d2675d7fd2195280da092eb1b3549e5457e065cc47349f2c3013b8a4a1938b05c033b2a5b6b477be39b415f4f50beac41f817783bda28f24bab3c8095e1b01e48ce4271cd2a323d1d8c100bb9856f75f63bdee34510de01e32efc0b12e980e9161a",
    .p_ciphertext           = "9149b2ed37d55b4624bbffc5cefdd81e4def4846e37b36145a9ee65fd5ab6d5257cf45b9e820d398e2eca51187edf2fedc77f261ef7c9ce9ba32dfcb512dc7d5b02e8cea52a05fbe67494305e5db19d522a3654f368f916c249c0d969b04532c1b07cba06274b031c8b99127c01b8743c2c0edc199805059942bc29fa2c0fc2a038714bee5e59c3f0e7fd5fbceaedd06e3d8eb464cf13a4897a57d1898578ac743263ab3c75ba456f0f5a39d874d369060c316c931901714451715648459a4b171fe3ab6092a06da3a4d34c6cf8311dc27276d6093a886a3cb05259f7e2cb11f73d4eb8c567a9867111c8f49a8a172bc8bc4f1df4180ed1a2db0",
    .p_key                  = "b64736f19c3e10324637ccb90c4558170a0dc151a2d9769c686438e55fb78e7e",
    .p_ad                   = "4bf9a35147f3b430",
    .p_nonce                = "43925318850a09c9fb23247cbd",
    .p_mac                  = "a74563df99af87f2"
};

// AES CCM - Custom test vector 9
NRF_SECTION_ITEM_REGISTER(test_vector_aead_data, test_vector_aead_t test_vector_aes_ccm_256_c9) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 message_len=16 ad_len=250 mac_len=8 nonce_len=13",
    .p_plaintext            = "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e",
    .p_ciphertext           = "70239d62473a528b8bccd573fcdcc9586c6fdad5056a89",
    .p_key                  = "b64736f19c3e10324637ccb90c4558170a0dc151a2d9769c686438e55fb78e7e",
    .p_ad                   = "0ee0089ec32c7cfa036bd1ab9c23b98cd5d166214d61c719688c6a0ecff705da47ac179cae7eaa3b638ab022a6669729fe47f24679e78488125d989cc7d1bbd8b472b04678b45adde4cca92cd91c4e214257243b163eca04bf643e9df6f7c5bf55aa91bc348154b5c3e9529a56fed8b04e0726b6de3ecc43f499108933e07297bdfcd4d8df2aa9af908083aeccc5c23fe9efbbf5fa9a27ea2923d36188197c87c955fcc5f7e0ebff4b01ebf008a5c93dab12ddfbf65f4225e5a3dfd44e36dbc3ba7891fce514d127e5eb8adb939670b4c5643c46425c614546b90cf0d720203bef4a12ff2d766623c92e134420e3b423b526598f32e8884843bc",
    .p_nonce                = "43925318850a09c9fb23247cbd",
    .p_mac                  = "515280d5ec0db1c4"
};


// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "eda32f751456e33195f1f499cf2dc7c97ea127b6d488f211ccc5126fbb24afa6",
    .p_ad                   = "",
    .p_nonce                = "a544218dadd3c1",
    .p_mac                  = "469c90bb"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "eda32f751456e33195f1f499cf2dc7c97ea127b6d488f211ccc5126fbb24afa6",
    .p_ad                   = "",
    .p_nonce                = "d3d5424e20fbec",
    .p_mac                  = "46a908ed"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "eda32f751456e33195f1f499cf2dc7c97ea127b6d488f211ccc5126fbb24afa6",
    .p_ad                   = "",
    .p_nonce                = "e776620a3bd961",
    .p_mac                  = "fdd35c4d"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 15
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt15) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "e1b8a927a95efe94656677b692662000278b441c79e879dd5c0ddc758bdc9ee8",
    .p_ad                   = "",
    .p_nonce                = "a544218dadd3c1",
    .p_mac                  = "8207eb14d33855a52acceed17dbcbf6e"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 16
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt16) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "e1b8a927a95efe94656677b692662000278b441c79e879dd5c0ddc758bdc9ee8",
    .p_ad                   = "",
    .p_nonce                = "d3d5424e20fbec",
    .p_mac                  = "60f8e127cb4d30db6df0622158cd931d"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 17
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt17) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "e1b8a927a95efe94656677b692662000278b441c79e879dd5c0ddc758bdc9ee8",
    .p_ad                   = "",
    .p_nonce                = "e776620a3bd961",
    .p_mac                  = "4239f29871651e9a26b8b06ffc5b3748"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 30
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt30) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=13 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "e1b8a927a95efe94656677b692662000278b441c79e879dd5c0ddc758bdc9ee8",
    .p_ad                   = "",
    .p_nonce                = "a544218dadd3c10583db49cf39",
    .p_mac                  = "8a19a133"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 31
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt31) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=13 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "e1b8a927a95efe94656677b692662000278b441c79e879dd5c0ddc758bdc9ee8",
    .p_ad                   = "",
    .p_nonce                = "3c0e2815d37d844f7ac240ba9d",
    .p_mac                  = "2e317f1b"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 32
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt32) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=0 ad_len=0 mac_len=4 nonce_len=13 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "e1b8a927a95efe94656677b692662000278b441c79e879dd5c0ddc758bdc9ee8",
    .p_ad                   = "",
    .p_nonce                = "75549e7e5657e5fe19872fcee0",
    .p_mac                  = "979bdcfe"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 45
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt45) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=13 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "af063639e66c284083c5cf72b70d8bc277f5978e80d9322d99f2fdc718cda569",
    .p_ad                   = "",
    .p_nonce                = "a544218dadd3c10583db49cf39",
    .p_mac                  = "97e1a8dd4259ccd2e431e057b0397fcf"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 46
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt46) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=13 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "af063639e66c284083c5cf72b70d8bc277f5978e80d9322d99f2fdc718cda569",
    .p_ad                   = "",
    .p_nonce                = "3c0e2815d37d844f7ac240ba9d",
    .p_mac                  = "5a9596c511ea6a8671adefc4f2157d8b"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 47
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt47) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=0 ad_len=0 mac_len=16 nonce_len=13 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "af063639e66c284083c5cf72b70d8bc277f5978e80d9322d99f2fdc718cda569",
    .p_ad                   = "",
    .p_nonce                = "75549e7e5657e5fe19872fcee0",
    .p_mac                  = "66f5c53efbc74fa02dedc303fd95133a"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 60
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt60) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=24 ad_len=0 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "d3d5424e20fbec43ae495353ed830271515ab104f8860c98",
    .p_ciphertext           = "64a1341679972dc5869fcf69b19d5c5ea50aa0b5e985f5b7",
    .p_key                  = "af063639e66c284083c5cf72b70d8bc277f5978e80d9322d99f2fdc718cda569",
    .p_ad                   = "",
    .p_nonce                = "a544218dadd3c1",
    .p_mac                  = "22aa8d59"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 61
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt61) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=24 ad_len=0 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "d3d5424e20fbec43ae495353ed830271515ab104f8860c98",
    .p_ciphertext           = "c5b7f802bffc498c1626e3774f1d9f94045dfd8e1a10a202",
    .p_key                  = "af063639e66c284083c5cf72b70d8bc277f5978e80d9322d99f2fdc718cda569",
    .p_ad                   = "",
    .p_nonce                = "bfcda8b5a2d0d2",
    .p_mac                  = "77d00a75"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 62
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt62) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=24 ad_len=0 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "d3d5424e20fbec43ae495353ed830271515ab104f8860c98",
    .p_ciphertext           = "bf432e246b7fa4aff8b3ada738432b51f6872ed92284db9d",
    .p_key                  = "af063639e66c284083c5cf72b70d8bc277f5978e80d9322d99f2fdc718cda569",
    .p_ad                   = "",
    .p_nonce                = "6bae7f35c56b27",
    .p_mac                  = "28588021"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 75
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt75) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=24 ad_len=0 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "d3d5424e20fbec43ae495353ed830271515ab104f8860c98",
    .p_ciphertext           = "bc51c3925a960e7732533e4ef3a4f69ee6826de952bcb0fd",
    .p_key                  = "f7079dfa3b5c7b056347d7e437bcded683abd6e2c9e069d333284082cbb5d453",
    .p_ad                   = "",
    .p_nonce                = "a544218dadd3c1",
    .p_mac                  = "374f3bb6db8377ebfc79674858c4f305"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 76
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt76) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=24 ad_len=0 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "d3d5424e20fbec43ae495353ed830271515ab104f8860c98",
    .p_ciphertext           = "afa1fa8e8a70e26b02161150556d604101fdf423f332c336",
    .p_key                  = "f7079dfa3b5c7b056347d7e437bcded683abd6e2c9e069d333284082cbb5d453",
    .p_ad                   = "",
    .p_nonce                = "bfcda8b5a2d0d2",
    .p_mac                  = "3275f2a4907d51b734fe7238cebbd48f"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 77
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt77) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=24 ad_len=0 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "d3d5424e20fbec43ae495353ed830271515ab104f8860c98",
    .p_ciphertext           = "72bc8ef21a847047091b673ccf231d35ecf6f4049741703b",
    .p_key                  = "f7079dfa3b5c7b056347d7e437bcded683abd6e2c9e069d333284082cbb5d453",
    .p_ad                   = "",
    .p_nonce                = "6bae7f35c56b27",
    .p_mac                  = "e672f1f22cbe4a5305f19aaa6967237b"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 120
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt120) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=0 ad_len=32 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "1b0e8df63c57f05d9ac457575ea764524b8610ae5164e6215f426f5a7ae6ede4",
    .p_ad                   = "d3d5424e20fbec43ae495353ed830271515ab104f8860c988d15b6d36c038eab",
    .p_nonce                = "a544218dadd3c1",
    .p_mac                  = "92d00fbe"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 121
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt121) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=0 ad_len=32 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "1b0e8df63c57f05d9ac457575ea764524b8610ae5164e6215f426f5a7ae6ede4",
    .p_ad                   = "232e957c65ffa11988e830d4617d500f1c4a35c1221f396c41ab214f074ca2dc",
    .p_nonce                = "78c46e3249ca28",
    .p_mac                  = "9143e5c4"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 122
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt122) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=0 ad_len=32 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "1b0e8df63c57f05d9ac457575ea764524b8610ae5164e6215f426f5a7ae6ede4",
    .p_ad                   = "0d40324aa758dbbb5391b5e6edb8a2310c94a4ae51d4fba8a7458d7cc8488baa",
    .p_nonce                = "c18d9e7971e2ae",
    .p_mac                  = "54337466"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 135
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt135) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=0 ad_len=32 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "a4bc10b1a62c96d459fbaf3a5aa3face7313bb9e1253e696f96a7a8e36801088",
    .p_ad                   = "d3d5424e20fbec43ae495353ed830271515ab104f8860c988d15b6d36c038eab",
    .p_nonce                = "a544218dadd3c1",
    .p_mac                  = "93af11a08379eb37a16aa2837f09d69d"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 136
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt136) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=0 ad_len=32 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "a4bc10b1a62c96d459fbaf3a5aa3face7313bb9e1253e696f96a7a8e36801088",
    .p_ad                   = "232e957c65ffa11988e830d4617d500f1c4a35c1221f396c41ab214f074ca2dc",
    .p_nonce                = "78c46e3249ca28",
    .p_mac                  = "d19b0c14ec686a7961ca7c386d125a65"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 137
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt137) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=0 ad_len=32 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "a4bc10b1a62c96d459fbaf3a5aa3face7313bb9e1253e696f96a7a8e36801088",
    .p_ad                   = "0d40324aa758dbbb5391b5e6edb8a2310c94a4ae51d4fba8a7458d7cc8488baa",
    .p_nonce                = "c18d9e7971e2ae",
    .p_mac                  = "02ea916d60e2ceec6d9dc9b1185569b3"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 180
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt180) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=24 ad_len=32 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "78c46e3249ca28e1ef0531d80fd37c124d9aecb7be6668e3",
    .p_ciphertext           = "c2fe12658139f5d0dd22cadf2e901695b579302a72fc5608",
    .p_key                  = "8c5cf3457ff22228c39c051c4e05ed4093657eb303f859a9d4b0f8be0127d88a",
    .p_ad                   = "d3d5424e20fbec43ae495353ed830271515ab104f8860c988d15b6d36c038eab",
    .p_nonce                = "a544218dadd3c1",
    .p_mac                  = "3ebc7720"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 181
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt181) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=24 ad_len=32 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "78c46e3249ca28e1ef0531d80fd37c124d9aecb7be6668e3",
    .p_ciphertext           = "94748ba81229e53c38583a8564b23ebbafc6f6efdf4c2a81",
    .p_key                  = "8c5cf3457ff22228c39c051c4e05ed4093657eb303f859a9d4b0f8be0127d88a",
    .p_ad                   = "5a053b2a1bb87e85d56527bfcdcd3ecafb991bb10e4c862bb0751c700a29f54b",
    .p_nonce                = "6ba004fd176791",
    .p_mac                  = "c44db2c9"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 182
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt182) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=24 ad_len=32 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "78c46e3249ca28e1ef0531d80fd37c124d9aecb7be6668e3",
    .p_ciphertext           = "1d670ccf3e9ba59186c48da2e5bd0ab21973eee2ea2985bf",
    .p_key                  = "8c5cf3457ff22228c39c051c4e05ed4093657eb303f859a9d4b0f8be0127d88a",
    .p_ad                   = "8f01a61eb17366d4e70942ab69b4f4bcf8ff6a97f5972ee5780a264c9dcf7d93",
    .p_nonce                = "45c5c284836414",
    .p_mac                  = "83a09067"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 195
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt195) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=24 ad_len=32 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "78c46e3249ca28e1ef0531d80fd37c124d9aecb7be6668e3",
    .p_ciphertext           = "3341168eb8c48468c414347fb08f71d2086f7c2d1bd581ce",
    .p_key                  = "705334e30f53dd2f92d190d2c1437c8772f940c55aa35e562214ed45bd458ffe",
    .p_ad                   = "d3d5424e20fbec43ae495353ed830271515ab104f8860c988d15b6d36c038eab",
    .p_nonce                = "a544218dadd3c1",
    .p_mac                  = "1ac68bd42f5ec7fa7e068cc0ecd79c2a"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 196
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt196) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=24 ad_len=32 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "78c46e3249ca28e1ef0531d80fd37c124d9aecb7be6668e3",
    .p_ciphertext           = "d543acda712b898cbb27b8f598b2e4438ce587a836e27851",
    .p_key                  = "705334e30f53dd2f92d190d2c1437c8772f940c55aa35e562214ed45bd458ffe",
    .p_ad                   = "5a053b2a1bb87e85d56527bfcdcd3ecafb991bb10e4c862bb0751c700a29f54b",
    .p_nonce                = "6ba004fd176791",
    .p_mac                  = "47c3338a2400809e739b63ba8227d2f9"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 197
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt197) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=24 ad_len=32 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "78c46e3249ca28e1ef0531d80fd37c124d9aecb7be6668e3",
    .p_ciphertext           = "39a8af5c976b995ea8049e55b68bc65503592ab009156386",
    .p_key                  = "705334e30f53dd2f92d190d2c1437c8772f940c55aa35e562214ed45bd458ffe",
    .p_ad                   = "8f01a61eb17366d4e70942ab69b4f4bcf8ff6a97f5972ee5780a264c9dcf7d93",
    .p_nonce                = "45c5c284836414",
    .p_mac                  = "46288ce9dd1c7088c752e35947fdca98"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 210
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt210) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=24 ad_len=32 mac_len=4 nonce_len=13 valid 1",
    .p_plaintext            = "e8de970f6ee8e80ede933581b5bcf4d837e2b72baa8b00c3",
    .p_ciphertext           = "c0ea400b599561e7905b99262b4565d5c3dc49fad84d7c69",
    .p_key                  = "705334e30f53dd2f92d190d2c1437c8772f940c55aa35e562214ed45bd458ffe",
    .p_ad                   = "3c0e2815d37d844f7ac240ba9d6e3a0b2a86f706e885959e09a1005e024f6907",
    .p_nonce                = "a544218dadd3c10583db49cf39",
    .p_mac                  = "ef891339"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 211
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt211) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=24 ad_len=32 mac_len=4 nonce_len=13 invalid 1",
    .p_plaintext            = "e8de970f6ee8e80ede933581b5bcf4d837e2b72baa8b00c3",
    .p_ciphertext           = "60871e03ea0eb968536c99f926ea24ef43d41272ad9fb7f6",
    .p_key                  = "705334e30f53dd2f92d190d2c1437c8772f940c55aa35e562214ed45bd458ffe",
    .p_ad                   = "5bb40e3bb72b4509324a7edc852f72535f1f6283156e63f6959ffaf39dcde800",
    .p_nonce                = "8fa501c5dd9ac9b868144c9fa5",
    .p_mac                  = "3d488623"
};

// AES CCM - NIST CAVS 11.1 DVPT Encrypt with keysize 256 - Count 212
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_encrypt212) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name     = "CCM 256 Encrypt message_len=24 ad_len=32 mac_len=4 nonce_len=13 invalid 2",
    .p_plaintext            = "e8de970f6ee8e80ede933581b5bcf4d837e2b72baa8b00c3",
    .p_ciphertext           = "81d7859dcbe51dcc94fe2591cd3b0540003d49a8c4dccbf4",
    .p_key                  = "705334e30f53dd2f92d190d2c1437c8772f940c55aa35e562214ed45bd458ffe",
    .p_ad                   = "07203674260208d5bd4d39506836f7e76ffc58e938799f21aff7bb4dea4410d2",
    .p_nonce                = "9bc0d1502a47e46350fe8667ca",
    .p_mac                  = "527e5ed0"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt0) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "eda32f751456e33195f1f499cf2dc7c97ea127b6d488f211ccc5126fbb24afa6",
    .p_ad                   = "",
    .p_nonce                = "a544218dadd3c1",
    .p_mac                  = "469c90bb"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt1) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "eda32f751456e33195f1f499cf2dc7c97ea127b6d488f211ccc5126fbb24afa6",
    .p_ad                   = "",
    .p_nonce                = "d3d5424e20fbec",
    .p_mac                  = "46a908ed"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt2) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "eda32f751456e33195f1f499cf2dc7c97ea127b6d488f211ccc5126fbb24afa6",
    .p_ad                   = "",
    .p_nonce                = "e776620a3bd961",
    .p_mac                  = "fdd35c4d"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 15
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt15) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "e1b8a927a95efe94656677b692662000278b441c79e879dd5c0ddc758bdc9ee8",
    .p_ad                   = "",
    .p_nonce                = "a544218dadd3c1",
    .p_mac                  = "8207eb14d33855a52acceed17dbcbf6e"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 16
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt16) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "e1b8a927a95efe94656677b692662000278b441c79e879dd5c0ddc758bdc9ee8",
    .p_ad                   = "",
    .p_nonce                = "d3d5424e20fbec",
    .p_mac                  = "60f8e127cb4d30db6df0622158cd931d"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 17
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt17) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "e1b8a927a95efe94656677b692662000278b441c79e879dd5c0ddc758bdc9ee8",
    .p_ad                   = "",
    .p_nonce                = "e776620a3bd961",
    .p_mac                  = "4239f29871651e9a26b8b06ffc5b3748"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 30
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt30) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=13 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "e1b8a927a95efe94656677b692662000278b441c79e879dd5c0ddc758bdc9ee8",
    .p_ad                   = "",
    .p_nonce                = "a544218dadd3c10583db49cf39",
    .p_mac                  = "8a19a133"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 31
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt31) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=13 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "e1b8a927a95efe94656677b692662000278b441c79e879dd5c0ddc758bdc9ee8",
    .p_ad                   = "",
    .p_nonce                = "3c0e2815d37d844f7ac240ba9d",
    .p_mac                  = "2e317f1b"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 32
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt32) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=0 ad_len=0 mac_len=4 nonce_len=13 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "e1b8a927a95efe94656677b692662000278b441c79e879dd5c0ddc758bdc9ee8",
    .p_ad                   = "",
    .p_nonce                = "75549e7e5657e5fe19872fcee0",
    .p_mac                  = "979bdcfe"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 45
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt45) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=13 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "af063639e66c284083c5cf72b70d8bc277f5978e80d9322d99f2fdc718cda569",
    .p_ad                   = "",
    .p_nonce                = "a544218dadd3c10583db49cf39",
    .p_mac                  = "97e1a8dd4259ccd2e431e057b0397fcf"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 46
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt46) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=13 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "af063639e66c284083c5cf72b70d8bc277f5978e80d9322d99f2fdc718cda569",
    .p_ad                   = "",
    .p_nonce                = "3c0e2815d37d844f7ac240ba9d",
    .p_mac                  = "5a9596c511ea6a8671adefc4f2157d8b"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 47
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt47) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=0 ad_len=0 mac_len=16 nonce_len=13 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "af063639e66c284083c5cf72b70d8bc277f5978e80d9322d99f2fdc718cda569",
    .p_ad                   = "",
    .p_nonce                = "75549e7e5657e5fe19872fcee0",
    .p_mac                  = "66f5c53efbc74fa02dedc303fd95133a"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 60
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt60) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=24 ad_len=0 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "d3d5424e20fbec43ae495353ed830271515ab104f8860c98",
    .p_ciphertext           = "64a1341679972dc5869fcf69b19d5c5ea50aa0b5e985f5b7",
    .p_key                  = "af063639e66c284083c5cf72b70d8bc277f5978e80d9322d99f2fdc718cda569",
    .p_ad                   = "",
    .p_nonce                = "a544218dadd3c1",
    .p_mac                  = "22aa8d59"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 61
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt61) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=24 ad_len=0 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "d3d5424e20fbec43ae495353ed830271515ab104f8860c98",
    .p_ciphertext           = "c5b7f802bffc498c1626e3774f1d9f94045dfd8e1a10a202",
    .p_key                  = "af063639e66c284083c5cf72b70d8bc277f5978e80d9322d99f2fdc718cda569",
    .p_ad                   = "",
    .p_nonce                = "bfcda8b5a2d0d2",
    .p_mac                  = "77d00a75"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 62
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt62) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=24 ad_len=0 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "d3d5424e20fbec43ae495353ed830271515ab104f8860c98",
    .p_ciphertext           = "bf432e246b7fa4aff8b3ada738432b51f6872ed92284db9d",
    .p_key                  = "af063639e66c284083c5cf72b70d8bc277f5978e80d9322d99f2fdc718cda569",
    .p_ad                   = "",
    .p_nonce                = "6bae7f35c56b27",
    .p_mac                  = "28588021"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 75
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt75) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=24 ad_len=0 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "d3d5424e20fbec43ae495353ed830271515ab104f8860c98",
    .p_ciphertext           = "bc51c3925a960e7732533e4ef3a4f69ee6826de952bcb0fd",
    .p_key                  = "f7079dfa3b5c7b056347d7e437bcded683abd6e2c9e069d333284082cbb5d453",
    .p_ad                   = "",
    .p_nonce                = "a544218dadd3c1",
    .p_mac                  = "374f3bb6db8377ebfc79674858c4f305"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 76
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt76) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=24 ad_len=0 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "d3d5424e20fbec43ae495353ed830271515ab104f8860c98",
    .p_ciphertext           = "afa1fa8e8a70e26b02161150556d604101fdf423f332c336",
    .p_key                  = "f7079dfa3b5c7b056347d7e437bcded683abd6e2c9e069d333284082cbb5d453",
    .p_ad                   = "",
    .p_nonce                = "bfcda8b5a2d0d2",
    .p_mac                  = "3275f2a4907d51b734fe7238cebbd48f"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 77
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt77) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=24 ad_len=0 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "d3d5424e20fbec43ae495353ed830271515ab104f8860c98",
    .p_ciphertext           = "72bc8ef21a847047091b673ccf231d35ecf6f4049741703b",
    .p_key                  = "f7079dfa3b5c7b056347d7e437bcded683abd6e2c9e069d333284082cbb5d453",
    .p_ad                   = "",
    .p_nonce                = "6bae7f35c56b27",
    .p_mac                  = "e672f1f22cbe4a5305f19aaa6967237b"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 120
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt120) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=0 ad_len=32 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "1b0e8df63c57f05d9ac457575ea764524b8610ae5164e6215f426f5a7ae6ede4",
    .p_ad                   = "d3d5424e20fbec43ae495353ed830271515ab104f8860c988d15b6d36c038eab",
    .p_nonce                = "a544218dadd3c1",
    .p_mac                  = "92d00fbe"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 121
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt121) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=0 ad_len=32 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "1b0e8df63c57f05d9ac457575ea764524b8610ae5164e6215f426f5a7ae6ede4",
    .p_ad                   = "232e957c65ffa11988e830d4617d500f1c4a35c1221f396c41ab214f074ca2dc",
    .p_nonce                = "78c46e3249ca28",
    .p_mac                  = "9143e5c4"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 122
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt122) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=0 ad_len=32 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "1b0e8df63c57f05d9ac457575ea764524b8610ae5164e6215f426f5a7ae6ede4",
    .p_ad                   = "0d40324aa758dbbb5391b5e6edb8a2310c94a4ae51d4fba8a7458d7cc8488baa",
    .p_nonce                = "c18d9e7971e2ae",
    .p_mac                  = "54337466"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 135
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt135) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=0 ad_len=32 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "a4bc10b1a62c96d459fbaf3a5aa3face7313bb9e1253e696f96a7a8e36801088",
    .p_ad                   = "d3d5424e20fbec43ae495353ed830271515ab104f8860c988d15b6d36c038eab",
    .p_nonce                = "a544218dadd3c1",
    .p_mac                  = "93af11a08379eb37a16aa2837f09d69d"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 136
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt136) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=0 ad_len=32 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "a4bc10b1a62c96d459fbaf3a5aa3face7313bb9e1253e696f96a7a8e36801088",
    .p_ad                   = "232e957c65ffa11988e830d4617d500f1c4a35c1221f396c41ab214f074ca2dc",
    .p_nonce                = "78c46e3249ca28",
    .p_mac                  = "d19b0c14ec686a7961ca7c386d125a65"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 137
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt137) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=0 ad_len=32 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "",
    .p_ciphertext           = "",
    .p_key                  = "a4bc10b1a62c96d459fbaf3a5aa3face7313bb9e1253e696f96a7a8e36801088",
    .p_ad                   = "0d40324aa758dbbb5391b5e6edb8a2310c94a4ae51d4fba8a7458d7cc8488baa",
    .p_nonce                = "c18d9e7971e2ae",
    .p_mac                  = "02ea916d60e2ceec6d9dc9b1185569b3"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 180
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt180) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=24 ad_len=32 mac_len=4 nonce_len=7 valid 1",
    .p_plaintext            = "78c46e3249ca28e1ef0531d80fd37c124d9aecb7be6668e3",
    .p_ciphertext           = "c2fe12658139f5d0dd22cadf2e901695b579302a72fc5608",
    .p_key                  = "8c5cf3457ff22228c39c051c4e05ed4093657eb303f859a9d4b0f8be0127d88a",
    .p_ad                   = "d3d5424e20fbec43ae495353ed830271515ab104f8860c988d15b6d36c038eab",
    .p_nonce                = "a544218dadd3c1",
    .p_mac                  = "3ebc7720"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 181
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt181) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=24 ad_len=32 mac_len=4 nonce_len=7 invalid 1",
    .p_plaintext            = "78c46e3249ca28e1ef0531d80fd37c124d9aecb7be6668e3",
    .p_ciphertext           = "94748ba81229e53c38583a8564b23ebbafc6f6efdf4c2a81",
    .p_key                  = "8c5cf3457ff22228c39c051c4e05ed4093657eb303f859a9d4b0f8be0127d88a",
    .p_ad                   = "5a053b2a1bb87e85d56527bfcdcd3ecafb991bb10e4c862bb0751c700a29f54b",
    .p_nonce                = "6ba004fd176791",
    .p_mac                  = "c44db2c9"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 182
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt182) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=24 ad_len=32 mac_len=4 nonce_len=7 invalid 2",
    .p_plaintext            = "78c46e3249ca28e1ef0531d80fd37c124d9aecb7be6668e3",
    .p_ciphertext           = "1d670ccf3e9ba59186c48da2e5bd0ab21973eee2ea2985bf",
    .p_key                  = "8c5cf3457ff22228c39c051c4e05ed4093657eb303f859a9d4b0f8be0127d88a",
    .p_ad                   = "8f01a61eb17366d4e70942ab69b4f4bcf8ff6a97f5972ee5780a264c9dcf7d93",
    .p_nonce                = "45c5c284836414",
    .p_mac                  = "83a09067"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 195
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt195) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=24 ad_len=32 mac_len=16 nonce_len=7 valid 1",
    .p_plaintext            = "78c46e3249ca28e1ef0531d80fd37c124d9aecb7be6668e3",
    .p_ciphertext           = "3341168eb8c48468c414347fb08f71d2086f7c2d1bd581ce",
    .p_key                  = "705334e30f53dd2f92d190d2c1437c8772f940c55aa35e562214ed45bd458ffe",
    .p_ad                   = "d3d5424e20fbec43ae495353ed830271515ab104f8860c988d15b6d36c038eab",
    .p_nonce                = "a544218dadd3c1",
    .p_mac                  = "1ac68bd42f5ec7fa7e068cc0ecd79c2a"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 196
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt196) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=24 ad_len=32 mac_len=16 nonce_len=7 invalid 1",
    .p_plaintext            = "78c46e3249ca28e1ef0531d80fd37c124d9aecb7be6668e3",
    .p_ciphertext           = "d543acda712b898cbb27b8f598b2e4438ce587a836e27851",
    .p_key                  = "705334e30f53dd2f92d190d2c1437c8772f940c55aa35e562214ed45bd458ffe",
    .p_ad                   = "5a053b2a1bb87e85d56527bfcdcd3ecafb991bb10e4c862bb0751c700a29f54b",
    .p_nonce                = "6ba004fd176791",
    .p_mac                  = "47c3338a2400809e739b63ba8227d2f9"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 197
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt197) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=24 ad_len=32 mac_len=16 nonce_len=7 invalid 2",
    .p_plaintext            = "78c46e3249ca28e1ef0531d80fd37c124d9aecb7be6668e3",
    .p_ciphertext           = "39a8af5c976b995ea8049e55b68bc65503592ab009156386",
    .p_key                  = "705334e30f53dd2f92d190d2c1437c8772f940c55aa35e562214ed45bd458ffe",
    .p_ad                   = "8f01a61eb17366d4e70942ab69b4f4bcf8ff6a97f5972ee5780a264c9dcf7d93",
    .p_nonce                = "45c5c284836414",
    .p_mac                  = "46288ce9dd1c7088c752e35947fdca98"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 210
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt210) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
    .expected_err_code      = NRF_SUCCESS,
    .crypt_expected_result  = EXPECTED_TO_PASS,
    .mac_expected_result    = EXPECTED_TO_PASS,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=24 ad_len=32 mac_len=4 nonce_len=13 valid 1",
    .p_plaintext            = "e8de970f6ee8e80ede933581b5bcf4d837e2b72baa8b00c3",
    .p_ciphertext           = "c0ea400b599561e7905b99262b4565d5c3dc49fad84d7c69",
    .p_key                  = "705334e30f53dd2f92d190d2c1437c8772f940c55aa35e562214ed45bd458ffe",
    .p_ad                   = "3c0e2815d37d844f7ac240ba9d6e3a0b2a86f706e885959e09a1005e024f6907",
    .p_nonce                = "a544218dadd3c10583db49cf39",
    .p_mac                  = "ef891339"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 211
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt211) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=24 ad_len=32 mac_len=4 nonce_len=13 invalid 1",
    .p_plaintext            = "e8de970f6ee8e80ede933581b5bcf4d837e2b72baa8b00c3",
    .p_ciphertext           = "60871e03ea0eb968536c99f926ea24ef43d41272ad9fb7f6",
    .p_key                  = "705334e30f53dd2f92d190d2c1437c8772f940c55aa35e562214ed45bd458ffe",
    .p_ad                   = "5bb40e3bb72b4509324a7edc852f72535f1f6283156e63f6959ffaf39dcde800",
    .p_nonce                = "8fa501c5dd9ac9b868144c9fa5",
    .p_mac                  = "3d488623"
};

// AES CCM - NIST CAVS 11.1 DVPT Decrypt with keysize 256 - Count 212
NRF_SECTION_ITEM_REGISTER(test_vector_aead_simple_data, test_vector_aead_t test_vector_aes_aead_ccm_256_decrypt212) =
{
    .p_aead_info            = &g_nrf_crypto_aes_ccm_256_info,
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_MBEDTLS)
    .expected_err_code      = NRF_ERROR_CRYPTO_AEAD_INVALID_MAC,
#else
    .expected_err_code      = NRF_ERROR_CRYPTO_INTERNAL,
#endif
    .crypt_expected_result  = EXPECTED_TO_FAIL,
    .mac_expected_result    = EXPECTED_TO_FAIL,
    .direction              = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name     = "CCM 256 Decrypt message_len=24 ad_len=32 mac_len=4 nonce_len=13 invalid 2",
    .p_plaintext            = "e8de970f6ee8e80ede933581b5bcf4d837e2b72baa8b00c3",
    .p_ciphertext           = "81d7859dcbe51dcc94fe2591cd3b0540003d49a8c4dccbf4",
    .p_key                  = "705334e30f53dd2f92d190d2c1437c8772f940c55aa35e562214ed45bd458ffe",
    .p_ad                   = "07203674260208d5bd4d39506836f7e76ffc58e938799f21aff7bb4dea4410d2",
    .p_nonce                = "9bc0d1502a47e46350fe8667ca",
    .p_mac                  = "527e5ed0"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM_256)

/*lint -restore */

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CCM)
