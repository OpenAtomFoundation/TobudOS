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

/**@brief AES CBC test vectors can be found on NIST web pages.
 *
 * https://csrc.nist.gov/projects/cryptographic-algorithm-validation-program/block-ciphers#AES
 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC)

/*lint -save -e91 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_128)

// AES CBC - Functional test using test vector NIST CAVS 11.1 CBC KeySbox 128 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_func_data, test_vector_aes_t test_vector_aes_cbc_128_functional) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 128 Functional",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "6d251e6944b051e04eaa6fb4dbf78465",
    .p_key              = "10a58869d74be5a374cf867cfb473859",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - Custom test vector - Encrypt - Message length 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_encrypt_c0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 128 Encrypt message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = "89df4c819f49dbcbcb124304023cf38c",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Decrypt - Message length 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_decrypt_c0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 128 Decrypt message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = "89df4c819f49dbcbcb124304023cf38c",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Encrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_encrypt_c3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 128 Encrypt message_len=1 pkcs7-padding",
    .p_plaintext        = "de",
    .p_ciphertext       = "65a55df93aa327a88cb5710d7cc1afa2",
    .p_key              = "89df4c819f49dbcbcb124304023cf38c",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Decrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_decrypt_c3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 128 Decrypt message_len=1 pkcs7-padding",
    .p_plaintext        = "de0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f",                                       /**< The 15 padded bytes (0x0f) will be removed in the actual generated plaintext. */
    .p_ciphertext       = "65a55df93aa327a88cb5710d7cc1afa2",
    .p_key              = "89df4c819f49dbcbcb124304023cf38c",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Encrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_encrypt_c4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 128 Encrypt message_len=16 pkcs7-padding",
    .p_plaintext        = "dededededededededededededededede",
    .p_ciphertext       = "620dd9eb230c761d55593923a7bbdb820eb10da808529643edb8cfd3585bd59a",
    .p_key              = "89df4c819f49dbcbcb124304023cf38c",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Decrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_decrypt_c4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 128 Decrypt message_len=16 pkcs7-padding",
    .p_plaintext        = "dededededededededededededededede10101010101010101010101010101010",       /**< The 16 padded bytes (0x10) will be removed in the actual generated plaintext. */
    .p_ciphertext       = "620dd9eb230c761d55593923a7bbdb820eb10da808529643edb8cfd3585bd59a",
    .p_key              = "89df4c819f49dbcbcb124304023cf38c",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Encrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_encrypt_c5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 128 Encrypt message_len=31 pkcs7-padding",
    .p_plaintext        = "dedededededededededededededededededededededededededededededede",
    .p_ciphertext       = "620dd9eb230c761d55593923a7bbdb82dd286d2d7acf39756cfeb0f836cfb0db",
    .p_key              = "89df4c819f49dbcbcb124304023cf38c",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Decrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_decrypt_c5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 128 Decrypt message_len=31 pkcs7-padding",
    .p_plaintext        = "dedededededededededededededededededededededededededededededede01",       /**< The padded byte (0x01) will be removed in the actual generated plaintext. */
    .p_ciphertext       = "620dd9eb230c761d55593923a7bbdb82dd286d2d7acf39756cfeb0f836cfb0db",
    .p_key              = "89df4c819f49dbcbcb124304023cf38c",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Test using test vector NIST CAVS 11.1 GFSBox Tests 128 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_encrypt_gfsb0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 128 Encrypt Key Zero",
    .p_plaintext        = "f34481ec3cc627bacd5dc3fb08f273e6",
    .p_ciphertext       = "0336763e966d92595a567cc9ce537f5e",
    .p_key              = "00000000000000000000000000000000",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - Test using test vector NIST CAVS 11.1 GFSBox Tests 128 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_decrypt_gfsb0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 128 Decrypt Key Zero",
    .p_plaintext        = "f34481ec3cc627bacd5dc3fb08f273e6",
    .p_ciphertext       = "0336763e966d92595a567cc9ce537f5e",
    .p_key              = "00000000000000000000000000000000",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - NIST CAVS 11.1 VarKey Tests 128 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_encrypt_vk0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 128 Encrypt VarKey 0",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "0edd33d3c621e546455bd8ba1418bec8",
    .p_key              = "80000000000000000000000000000000",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - NIST CAVS 11.1 VarKey Tests 128 Encrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_encrypt_vk127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 128 Encrypt VarKey 127",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "a1f6258c877d5fcd8964484538bfc92c",
    .p_key              = "ffffffffffffffffffffffffffffffff",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - NIST CAVS 11.1 VarKey Tests 128 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_decrypt_vk0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 128 Decrypt VarKey 0",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "0edd33d3c621e546455bd8ba1418bec8",
    .p_key              = "80000000000000000000000000000000",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - NIST CAVS 11.1 VarKey Tests 128 Decrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_decrypt_vk127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 128 Decrypt VarKey 127",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "a1f6258c877d5fcd8964484538bfc92c",
    .p_key              = "ffffffffffffffffffffffffffffffff",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_encrypt_16) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 128 Encrypt message_len=16",
    .p_plaintext        = "45cf12964fc824ab76616ae2f4bf0822",
    .p_ciphertext       = "0f61c4d44c5147c03c195ad7e2cc12b2",
    .p_key              = "1f8e4973953f3fb0bd6b16662e9a3c17",
    .p_iv               = "2fe2b333ceda8f98f4a99b40d2cd34a8"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_encrypt_32) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 128 Encrypt message_len=32",
    .p_plaintext        = "068b25c7bfb1f8bdd4cfc908f69dffc5ddc726a197f0e5f720f730393279be91",
    .p_ciphertext       = "c4dc61d9725967a3020104a9738f23868527ce839aab1752fd8bdb95a82c4d00",
    .p_key              = "0700d603a1c514e46b6191ba430a3a0c",
    .p_iv               = "aad1583cd91365e3bb2f0c3430d065bb"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_encrypt_48) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 128 Encrypt message_len=48",
    .p_plaintext        = "9b7cee827a26575afdbb7c7a329f887238052e3601a7917456ba61251c214763d5e1847a6ad5d54127a399ab07ee3599",
    .p_ciphertext       = "d5aed6c9622ec451a15db12819952b6752501cf05cdbf8cda34a457726ded97818e1f127a28d72db5652749f0c6afee5",
    .p_key              = "3348aa51e9a45c2dbe33ccc47f96e8de",
    .p_iv               = "19153c673160df2b1d38c28060e59b96"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_encrypt_64) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 128 Encrypt message_len=64",
    .p_plaintext        = "9ac19954ce1319b354d3220460f71c1e373f1cd336240881160cfde46ebfed2e791e8d5a1a136ebd1dc469dec00c4187722b841cdabcb22c1be8a14657da200e",
    .p_ciphertext       = "19b9609772c63f338608bf6eb52ca10be65097f89c1e0905c42401fd47791ae2c5440b2d473116ca78bd9ff2fb6015cfd316524eae7dcb95ae738ebeae84a467",
    .p_key              = "b7f3c9576e12dd0db63e8f8fac2b9a39",
    .p_iv               = "c80f095d8bb1a060699f7c19974a1aa0"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_encrypt_80) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 128 Encrypt message_len=80",
    .p_plaintext        = "db397ec22718dbffb9c9d13de0efcd4611bf792be4fce0dc5f25d4f577ed8cdbd4eb9208d593dda3d4653954ab64f05676caa3ce9bfa795b08b67ceebc923fdc89a8c431188e9e482d8553982cf304d1",
    .p_ciphertext       = "10ea27b19e16b93af169c4a88e06e35c99d8b420980b058e34b4b8f132b13766f72728202b089f428fecdb41c79f8aa0d0ef68f5786481cca29e2126f69bc14160f1ae2187878ba5c49cf3961e1b7ee9",
    .p_key              = "b6f9afbfe5a1562bba1368fc72ac9d9c",
    .p_iv               = "3f9d5ebe250ee7ce384b0d00ee849322"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_encrypt_96) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 128 Encrypt message_len=96",
    .p_plaintext        = "2aad0c2c4306568bad7447460fd3dac054346d26feddbc9abd9110914011b4794be2a9a00a519a51a5b5124014f4ed2735480db21b434e99a911bb0b60fe0253763725b628d5739a5117b7ee3aefafc5b4c1bf446467e7bf5f78f31ff7caf187",
    .p_ciphertext       = "3b8611bfc4973c5cd8e982b073b33184cd26110159172e44988eb5ff5661a1e16fad67258fcbfee55469267a12dc374893b4e3533d36f5634c3095583596f135aa8cd1138dc898bc5651ee35a92ebf89ab6aeb5366653bc60a70e0074fc11efe",
    .p_key              = "bbe7b7ba07124ff1ae7c3416fe8b465e",
    .p_iv               = "7f65b5ee3630bed6b84202d97fb97a1e"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_encrypt_112) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 128 Encrypt message_len=112",
    .p_plaintext        = "807bc4ea684eedcfdcca30180680b0f1ae2814f35f36d053c5aea6595a386c1442770f4d7297d8b91825ee7237241da8925dd594ccf676aecd46ca2068e8d37a3a0ec8a7d5185a201e663b5ff36ae197110188a23503763b8218826d23ced74b31e9f6e2d7fbfa6cb43420c7807a8625",
    .p_ciphertext       = "406af1429a478c3d07e555c5287a60500d37fc39b68e5bbb9bafd6ddb223828561d6171a308d5b1a4551e8a5e7d572918d25c968d3871848d2f16635caa9847f38590b1df58ab5efb985f2c66cfaf86f61b3f9c0afad6c963c49cee9b8bc81a2ddb06c967f325515a4849eec37ce721a",
    .p_key              = "89a553730433f7e6d67d16d373bd5360",
    .p_iv               = "f724558db3433a523f4e51a5bea70497"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_encrypt_128) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 128 Encrypt message_len=128",
    .p_plaintext        = "cb6a787e0dec56f9a165957f81af336ca6b40785d9e94093c6190e5152649f882e874d79ac5e167bd2a74ce5ae088d2ee854f6539e0a94796b1e1bd4c9fcdbc79acbef4d01eeb89776d18af71ae2a4fc47dd66df6c4dbe1d1850e466549a47b636bcc7c2b3a62495b56bb67b6d455f1eebd9bfefecbca6c7f335cfce9b45cb9d",
    .p_ciphertext       = "7b2931f5855f717145e00f152a9f4794359b1ffcb3e55f594e33098b51c23a6c74a06c1d94fded7fd2ae42c7db7acaef5844cb33aeddc6852585ed0020a6699d2cb53809cefd169148ce42292afab063443978306c582c18b9ce0da3d084ce4d3c482cfd8fcf1a85084e89fb88b40a084d5e972466d07666126fb761f84078f2",
    .p_key              = "c491ca31f91708458e29a925ec558d78",
    .p_iv               = "9ef934946e5cd0ae97bd58532cb49381"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_encrypt_144) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 128 Encrypt message_len=144",
    .p_plaintext        = "f82bef3c73a6f7f80db285726d691db6bf55eec25a859d3ba0e0445f26b9bb3b16a3161ed1866e4dd8f2e5f8ecb4e46d74a7a78c20cdfc7bcc9e479ba7a0caba9438238ad0c01651d5d98de37f03ddce6e6b4bd4ab03cf9e8ed818aedfa1cf963b932067b97d776dce1087196e7e913f7448e38244509f0caf36bd8217e15336d35c149fd4e41707893fdb84014f8729",
    .p_ciphertext       = "b09512f3eff9ed0d85890983a73dadbb7c3678d52581be64a8a8fc586f490f2521297a478a0598040ebd0f5509fafb0969f9d9e600eaef33b1b93eed99687b167f89a5065aac439ce46f3b8d22d30865e64e45ef8cd30b6984353a844a11c8cd60dba0e8866b3ee30d24b3fa8a643b328353e06010fa8273c8fd54ef0a2b6930e5520aae5cd5902f9b86a33592ca4365",
    .p_key              = "f6e87d71b0104d6eb06a68dc6a71f498",
    .p_iv               = "1c245f26195b76ebebc2edcac412a2f8"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_encrypt_160) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 128 Encrypt message_len=160",
    .p_plaintext        = "40d930f9a05334d9816fe204999c3f82a03f6a0457a8c475c94553d1d116693adc618049f0a769a2eed6a6cb14c0143ec5cccdbc8dec4ce560cfd206225709326d4de7948e54d603d01b12d7fed752fb23f1aa4494fbb00130e9ded4e77e37c079042d828040c325b1a5efd15fc842e44014ca4374bf38f3c3fc3ee327733b0c8aee1abcd055772f18dc04603f7b2c1ea69ff662361f2be0a171bbdcea1e5d3f",
    .p_ciphertext       = "6be8a12800455a320538853e0cba31bd2d80ea0c85164a4c5c261ae485417d93effe2ebc0d0a0b51d6ea18633d210cf63c0c4ddbc27607f2e81ed9113191ef86d56f3b99be6c415a4150299fb846ce7160b40b63baf1179d19275a2e83698376d28b92548c68e06e6d994e2c1501ed297014e702cdefee2f656447706009614d801de1caaf73f8b7fa56cf1ba94b631933bbe577624380850f117435a0355b2b",
    .p_key              = "2c14413751c31e2730570ba3361c786b",
    .p_iv               = "1dbbeb2f19abb448af849796244a19d7"
};


// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_decrypt_16) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 128 Decrypt message_len=16",
    .p_plaintext        = "940bc76d61e2c49dddd5df7f37fcf105",
    .p_ciphertext       = "f8eb31b31e374e960030cd1cadb0ef0c",
    .p_key              = "6a7082cf8cda13eff48c8158dda206ae",
    .p_iv               = "bd4172934078c2011cb1f31cffaf486e"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_decrypt_32) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 128 Decrypt message_len=32",
    .p_plaintext        = "360dc1896ce601dfb2a949250067aad96737847a4580ede2654a329b842fe81e",
    .p_ciphertext       = "5d6fed86f0c4fe59a078d6361a142812514b295dc62ff5d608a42ea37614e6a1",
    .p_key              = "625eefa18a4756454e218d8bfed56e36",
    .p_iv               = "73d9d0e27c2ec568fbc11f6a0998d7c8"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_decrypt_48) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 128 Decrypt message_len=48",
    .p_plaintext        = "a206385945b21f812a9475f47fddbb7fbdda958a8d14c0dbcdaec36e8b28f1f6ececa1ceae4ce17721d162c1d42a66c1",
    .p_ciphertext       = "e29e3114c8000eb484395b256b1b3267894f290d3999819ff35da03e6463c186c4d7ebb964941f1986a2d69572fcaba8",
    .p_key              = "fd6e0b954ae2e3b723d6c9fcae6ab09b",
    .p_iv               = "f08b65c9f4dd950039941da2e8058c4e"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_decrypt_64) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 128 Decrypt message_len=64",
    .p_plaintext        = "b968aeb199ad6b3c8e01f26c2edad444538c78bfa36ed68ca76123b8cdce615a01f6112bb80bfc3f17490578fb1f909a52e162637b062db04efee291a1f1af60",
    .p_ciphertext       = "d3f89b71e033070f9d7516a6cb4ea5ef51d6fb63d4f0fea089d0a60e47bbb3c2e10e9ba3b282c7cb79aefe3068ce228377c21a58fe5a0f8883d0dbd3d096beca",
    .p_key              = "7b1ab9144b0239315cd5eec6c75663bd",
    .p_iv               = "0b1e74f45c17ff304d99c059ce5cde09"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_decrypt_80) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 128 Decrypt message_len=80",
    .p_plaintext        = "999983467c47bb1d66d7327ab5c58f61ddb09b93bd2460cb78cbc12b5fa1ea0c5f759ccc5e478697687012ff4673f6e61eecaeda0ccad2d674d3098c7d17f887b62b56f56b03b4d055bf3a4460e83efa",
    .p_ciphertext       = "9ed957bd9bc52bba76f68cfbcde52157a8ca4f71ac050a3d92bdebbfd7c78316b4c9f0ba509fad0235fdafe90056ad115dfdbf08338b2acb1c807a88182dd2a882d1810d4302d598454e34ef2b23687d",
    .p_key              = "36466b6bd25ea3857ea42f0cac1919b1",
    .p_iv               = "7186fb6bdfa98a16189544b228f3bcd3"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_decrypt_96) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 128 Decrypt message_len=96",
    .p_plaintext        = "45efd00daa4cdc8273ef785cae9e944a7664a2391e1e2c449f475acec0124bbc22944331678617408a1702917971f4654310ffb9229bec6173715ae512d37f93aaa6abf009f7e30d65669d1db0366b5bce4c7b00f871014f5753744a1878dc57",
    .p_ciphertext       = "1bcba44ddff503db7c8c2ec4c4eea0e827957740cce125c1e11769842fa97e25f1b89269e6d77923a512a358312f4ba1cd33f2d111280cd83e1ef9e7cf7036d55048d5c273652afa611cc81b4e9dac7b5078b7c4716062e1032ead1e3329588a",
    .p_key              = "89373ee6e28397640d5082eed4123239",
    .p_iv               = "1a74d7c859672c804b82472f7e6d3c6b"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_decrypt_112) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 128 Decrypt message_len=112",
    .p_plaintext        = "c5585ff215bbb73ba5393440852fb199436de0d15e55c631f877670aa3eda9f672eb1f876f09544e63558436b8928000db2f02a5ad90f95b05ac4cf49e198e617e7678480fdf0efacc6aae691271e6cdd3541ebf719a1ccaedb24e2f80f92455dd5910cb5086b0960a3942ec182dcbd7",
    .p_ciphertext       = "c65b94b1f291fa9f0600f22c3c0432c895ad5d177bcccc9ea44e8ec339c9adf43855b326179d6d81aa36ef59462fd86127e9d81b0f286f93306bf74d4c79e47c1b3d4b74edd3a16290e3c63b742e41f20d66ceee794316bb63d3bd002712a1b136ba6185bd5c1dab81b07db90d2af5e5",
    .p_key              = "bab0cceddc0abd63e3f82e9fbff7b8aa",
    .p_iv               = "68b9140f300490c5c942f66e777eb806"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_decrypt_128) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 128 Decrypt message_len=128",
    .p_plaintext        = "1d1f8d81bdc3e2c7cb057f408e6450000c5aaed3260ff1e87fbb6f324df6887ffd8f78d7e2a04c9ed9deda9d64482d2b002f4a2b78d8b4f691875c8295d4a64b22257ceaf713ed2f4b92530d7ad7151d629acda882b4829577a43990b0948c1149c22fe4273656d1b08833930e8b06709a94579a78fc220f7057bbc1fa9f6563",
    .p_ciphertext       = "ba9f646755dacc22911f51d7de2f7e7cb0bc0b75257ea44fe883edb055c7c28ede04c3a0adcb10128ad4517d0093fa16bb0bcd2635e7a0ba92c7609bc8d8568002a7a983473724d256513aa7d51b477aabec1975ab5faf2872a6407e922180eff02f1ef86a4591c8bd3d143da6f0ef0e4806f94ace0d5b0151c99640fccbc843",
    .p_key              = "9c702898efa44557b29ed283f5bc0293",
    .p_iv               = "cec6e1b82e8b2a591a9fa5ff1cf5cc51"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_decrypt_144) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 128 Decrypt message_len=144",
    .p_plaintext        = "6d40fd2f908f48ce19241b6b278b1b1676dffd4a97ce9f8a1574c33bc59237deb536bee376fd6c381e6987700e39283aa111cf1a59f26fae6fb6700bf012646a2ab80239bf5e1632329043aa87d7911978b36523a2bc0bed9a9737ccf7a00baa2f3822b4e9e742e168e7069290705fed2eb63aa044b78f97dd33a8d6b24741ec1fd8c8db79d93b884e762dba0f406961",
    .p_ciphertext       = "198351f453103face6655666fe90bdbd9630e3733b2d66c013a634e91f2bf015bd2d975d71b26322e44defa32d4e9dce50363557046ece08ba38f258dae5fd3e5049c647476c81e73482e40c171d89f9fea29452caf995733589b0061464fbd5dabe27dc5ea463a3deeb7dcb43664ae6a65c498c143883ab8e83b51e5410b181647602443dc3cfffe86f0205398fa83c",
    .p_key              = "5674636dbdb38f705f0b08c372ef4785",
    .p_iv               = "3f20ce0509b57420d53b6be4d0b7f0a9"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_128_decrypt_160) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 128 Decrypt message_len=160",
    .p_plaintext        = "e8b89150d8438bf5b17449d6ed26bd72127e10e4aa57cad85283e8359e089208e84921649f5b60ea21f7867cbc9620560c4c6238db021216db453c9943f1f1a60546173daef2557c3cdd855031b353d4bf176f28439e48785c37d38f270aa4a6faad2baabcb0c0b2d1dd5322937498ce803ba1148440a52e227ddba4872fe4d81d2d76a939d24755adb8a7b8452ceed2d179e1a5848f316f5c016300a390bfa7",
    .p_ciphertext       = "22cdc3306fcd4d31ccd32720cbb61bad28d855670657c48c7b88c31f4fa1f93c01b57da90be63ead67d6a325525e6ed45083e6fb70a53529d1fa0f55653b942af59d78a2660361d63a7290155ac5c43312a25b235dacbbc863faf00940c99624076dfa44068e7c554c9038176953e571751dfc0954d41d113771b06466b1c8d13e0d4cb675ed58d1a619e1540970983781dc11d2dd8525ab5745958d615defda",
    .p_key              = "97a1025529b9925e25bbe78770ca2f99",
    .p_iv               = "d4b4eab92aa9637e87d366384ed6915c"
};


// AES CBC - NIST CAVS 11.1 Monte Carlo 128 Encrypt
NRF_SECTION_ITEM_REGISTER(test_vector_aes_monte_carlo_data, test_vector_aes_t test_vector_aes_cbc_128_encrypt_monte_carlo) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC Encrypt Monte Carlo 128",
    .p_plaintext        = "1fd4ee65603e6130cfc2a82ab3d56c24",
    .p_ciphertext       = "7bed7671c8913aa1330f193761523e67",
    .p_key              = "8809e7dd3a959ee5d8dbb13f501f2274",
    .p_iv               = "e5c0bb535d7d54572ad06d170a0e58ae"
};

// AES CBC - NIST CAVS 11.1 Monte Carlo 128 Decrypt
NRF_SECTION_ITEM_REGISTER(test_vector_aes_monte_carlo_data, test_vector_aes_t test_vector_aes_cbc_128_decrypt_monte_carlo) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC Decrypt Monte Carlo 128",
    .p_plaintext        = "4769317b0562c45949c18b3855f8bf4a",
    .p_ciphertext       = "7c54923b0490a9d4de4ec1ce6790aa4d",
    .p_key              = "287b07c78f8e3e1be7c41b3d96c04e6e",
    .p_iv               = "41b461f9464fd515d25413b4241002b8"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_128)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_192)

// AES CBC - Functional test using test vector NIST CAVS 11.1 CBC KeySbox 192 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_func_data, test_vector_aes_t test_vector_aes_cbc_192_functional) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 192 Functional",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "0956259c9cd5cfd0181cca53380cde06",
    .p_key              = "e9f065d7c13573587f7875357dfbb16c53489f6a4bd0f7cd",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - Custom test vector - Encrypt - Message length 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_encrypt_c0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 192 Encrypt message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = "73befbe26caa000bb57574a7d33ad60019b40c7a3ac6562d",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Decrypt - Message length 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_decrypt_c0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 192 Decrypt message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = "73befbe26caa000bb57574a7d33ad60019b40c7a3ac6562d",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Encrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_encrypt_c3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 192 Encrypt message_len=1 pkcs7-padding",
    .p_plaintext        = "de",
    .p_ciphertext       = "4bb7de2040dad371b882c161608b50ed",
    .p_key              = "73befbe26caa000bb57574a7d33ad60019b40c7a3ac6562d",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Decrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_decrypt_c3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 192 Decrypt message_len=1 pkcs7-padding",
    .p_plaintext        = "de0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f",                                       /**< The 15 padded bytes (0x0f) will be removed in the actual generated plaintext. */
    .p_ciphertext       = "4bb7de2040dad371b882c161608b50ed",
    .p_key              = "73befbe26caa000bb57574a7d33ad60019b40c7a3ac6562d",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Encrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_encrypt_c4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 192 Encrypt message_len=16 pkcs7-padding",
    .p_plaintext        = "dededededededededededededededede",
    .p_ciphertext       = "20410bb5a89451158f6e82fb4ab423e5ae4b22c09442248b12d9d7bc09266ae2",
    .p_key              = "73befbe26caa000bb57574a7d33ad60019b40c7a3ac6562d",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Decrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_decrypt_c4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 192 Decrypt message_len=16 pkcs7-padding",
    .p_plaintext        = "dededededededededededededededede10101010101010101010101010101010",       /**< The 16 padded bytes (0x10) will be removed in the actual generated plaintext. */
    .p_ciphertext       = "20410bb5a89451158f6e82fb4ab423e5ae4b22c09442248b12d9d7bc09266ae2",
    .p_key              = "73befbe26caa000bb57574a7d33ad60019b40c7a3ac6562d",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Encrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_encrypt_c5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 192 Encrypt message_len=31 pkcs7-padding",
    .p_plaintext        = "dedededededededededededededededededededededededededededededede",
    .p_ciphertext       = "20410bb5a89451158f6e82fb4ab423e5ca4aaff6411c38eb07d63a0c7117d93f",
    .p_key              = "73befbe26caa000bb57574a7d33ad60019b40c7a3ac6562d",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Decrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_decrypt_c5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 192 Decrypt message_len=31 pkcs7-padding",
    .p_plaintext        = "dedededededededededededededededededededededededededededededede01",       /**< The padded byte (0x01) will be removed in the actual generated plaintext. */
    .p_ciphertext       = "20410bb5a89451158f6e82fb4ab423e5ca4aaff6411c38eb07d63a0c7117d93f",
    .p_key              = "73befbe26caa000bb57574a7d33ad60019b40c7a3ac6562d",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - NIST CAVS 11.1 GFSBox Tests 192 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_encrypt_gfsb0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 192 Encrypt GFSBox 0",
    .p_plaintext        = "1b077a6af4b7f98229de786d7516b639",
    .p_ciphertext       = "275cfc0413d8ccb70513c3859b1d0f72",
    .p_key              = "000000000000000000000000000000000000000000000000",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - NIST CAVS 11.1 GFSBox Tests 192 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_decrypt_gfsb0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 192 Decrypt GFSBox 0",
    .p_plaintext        = "1b077a6af4b7f98229de786d7516b639",
    .p_ciphertext       = "275cfc0413d8ccb70513c3859b1d0f72",
    .p_key              = "000000000000000000000000000000000000000000000000",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - NIST CAVS 11.1 VarKey Tests 192 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_encrypt_vk0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 192 Encrypt VarKey 0",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "de885dc87f5a92594082d02cc1e1b42c",
    .p_key              = "800000000000000000000000000000000000000000000000",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - NIST CAVS 11.1 VarKey Tests 192 Encrypt - Count 191
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_encrypt_vk191) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 192 Encrypt VarKey 191",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "dd8a493514231cbf56eccee4c40889fb",
    .p_key              = "ffffffffffffffffffffffffffffffffffffffffffffffff",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - NIST CAVS 11.1 VarKey Tests 192 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_decrypt_vk0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 192 Decrypt VarKey 0",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "de885dc87f5a92594082d02cc1e1b42c",
    .p_key              = "800000000000000000000000000000000000000000000000",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - NIST CAVS 11.1 VarKey Tests 192 Decrypt - Count 191
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_decrypt_vk191) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 192 Decrypt VarKey 191",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "dd8a493514231cbf56eccee4c40889fb",
    .p_key              = "ffffffffffffffffffffffffffffffffffffffffffffffff",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_encrypt_16) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 192 Encrypt message_len=16",
    .p_plaintext        = "c51fc276774dad94bcdc1d2891ec8668",
    .p_ciphertext       = "70dd95a14ee975e239df36ff4aee1d5d",
    .p_key              = "ba75f4d1d9d7cf7f551445d56cc1a8ab2a078e15e049dc2c",
    .p_iv               = "531ce78176401666aa30db94ec4a30eb"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_encrypt_32) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,

    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 192 Encrypt message_len=32",
    .p_plaintext        = "9d4e4cccd1682321856df069e3f1c6fa391a083a9fb02d59db74c14081b3acc4",
    .p_ciphertext       = "51d44779f90d40a80048276c035cb49ca2a47bcb9b9cf7270b9144793787d53f",
    .p_key              = "eab3b19c581aa873e1981c83ab8d83bbf8025111fb2e6b21",
    .p_iv               = "f3d6667e8d4d791e60f7505ba383eb05"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_encrypt_48) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 192 Encrypt message_len=48",
    .p_plaintext        = "c5ce958613bf741718c17444484ebaf1050ddcacb59b9590178cbe69d7ad7919608cb03af13bbe04f3506b718a301ea0",
    .p_ciphertext       = "ed6a50e0c6921d52d6647f75d67b4fd56ace1fedb8b5a6a997b4d131640547d22c5d884a75e6752b5846b5b33a5181f4",
    .p_key              = "16c93bb398f1fc0cf6d68fc7a5673cdf431fa147852b4a2d",
    .p_iv               = "eaaeca2e07ddedf562f94df63f0a650f"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_encrypt_64) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 192 Encrypt message_len=64",
    .p_plaintext        = "db3785a889b4bd387754da222f0e4c2d2bfe0d79e05bc910fba941beea30f1239eacf0068f4619ec01c368e986fca6b7c58e490579d29611bd10087986eff54f",
    .p_ciphertext       = "d5f5589760bf9c762228fde236de1fa2dd2dad448db3fa9be0c4196efd46a35c84dd1ac77d9db58c95918cb317a6430a08d2fb6a8e8b0f1c9b72c7a344dc349f",
    .p_key              = "067bb17b4df785697eaccf961f98e212cb75e6797ce935cb",
    .p_iv               = "8b59c9209c529ca8391c9fc0ce033c38"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_encrypt_80) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 192 Encrypt message_len=80",
    .p_plaintext        = "38fbda37e28fa86d9d83a4345e419dea95d28c7818ff25925db6ac3aedaf0a86154e20a4dfcc5b1b4192895393e5eb5846c88bdbd41ecf7af3104f410eaee470f5d9017ed460475f626953035a13db1f",
    .p_ciphertext       = "edadae2f9a45ff3473e02d904c94d94a30a4d92da4deb6bcb4b0774472694571842039f21c496ef93fd658842c735f8a81fcd0aa578442ab893b18f606aed1bab11f81452dd45e9b56adf2eccf4ea095",
    .p_key              = "0fd39de83e0be77a79c8a4a612e3dd9c8aae2ce35e7a2bf8",
    .p_iv               = "7e1d629b84f93b079be51f9a5f5cb23c"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_encrypt_96) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 192 Encrypt message_len=96",
    .p_plaintext        = "931b2f5f3a5820d53a6beaaa6431083a3488f4eb03b0f5b57ef838e1579623103bd6e6800377538b2e51ef708f3c4956432e8a8ee6a34e190642b26ad8bdae6c2af9a6c7996f3b6004d2671e41f1c9f40ee03d1c4a52b0a0654a331f15f34dce",
    .p_ciphertext       = "75395974bd32b3665654a6c8e396b88ae34b123575872a7ab687d8e76b46df911a8a590cd01d2f5c330be3a6626e9dd3aa5e10ed14e8ff829811b6fed50f3f533ca4385a1cbca78f5c4744e50f2f8359165c2485d1324e76c3eae76a0ccac629",
    .p_key              = "e3fecc75f0075a09b383dfd389a3d33cc9b854b3b254c0f4",
    .p_iv               = "36eab883afef936cc38f63284619cd19"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_encrypt_112) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 192 Encrypt message_len=112",
    .p_plaintext        = "6abcc270173cf114d44847e911a050db57ba7a2e2c161c6f37ccb6aaa4677bddcaf50cad0b5f8758fcf7c0ebc650ceb5cd52cafb8f8dd3edcece55d9f1f08b9fa8f54365cf56e28b9596a7e1dd1d3418e4444a7724add4cf79d527b183ec88de4be4eeff29c80a97e54f85351cb189ee",
    .p_ciphertext       = "ca282924a61187feb40520979106e5cc861957f23828dcb7285e0eaac8a0ca2a6b60503d63d6039f4693dba32fa1f73ae2e709ca94911f28a5edd1f30eaddd54680c43acc9c74cd90d8bb648b4e544275f47e514daa20697f66c738eb30337f017fca1a26da4d1a0cc0a0e98e2463070",
    .p_key              = "f9c27565eb07947c8cb51b79248430f7b1066c3d2fdc3d13",
    .p_iv               = "2bd67cc89ab7948d644a49672843cbd9"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_encrypt_128) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 192 Encrypt message_len=128",
    .p_plaintext        = "c1a37683fb289467dd1b2c89efba16bbd2ee24cf18d19d44596ded2682c79a2f711c7a32bf6a24badd32a4ee637c73b7a41da6258635650f91fb9ffa45bdfc3cb122136241b3deced8996aa51ea8d3e81c9d70e006a44bc0571ed48623a0d622a93fa9da290baaedf5d9e876c94620945ff8ecc83f27379ed55cf490c5790f27",
    .p_ciphertext       = "8158e21420f25b59d6ae943fa1cbf21f02e979f419dab0126a721b7eef55bee9ad97f5ccff7d239057bbc19a8c378142f7672f1d5e7e17d7bebcb0070e8355cace6660171a53b61816ae824a6ef69ce470b6ffd3b5bb4b438874d91d27854d3b6f25860d3868958de3307d62b1339bdddb8a318c0ce0f33c17caf0e9f6040820",
    .p_key              = "fb09cf9e00dbf883689d079c920077c0073c31890b55bab5",
    .p_iv               = "e3c89bd097c3abddf64f4881db6dbfe2"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_encrypt_144) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 192 Encrypt message_len=144",
    .p_plaintext        = "5becbc31d8bead6d36ae014a5863d14a431e6b55d29ea6baaa417271716db3a33b2e506b452086dfe690834ac2de30bc41254ec5401ec47d064237c7792fdcd7914d8af20eb114756642d519021a8c75a92f6bc53d326ae9a5b7e1b10a9756574692934d9939fc399e0c203f7edf8e7e6482eadd31a0400770e897b48c6bca2b404593045080e93377358c42a0f4dede",
    .p_ciphertext       = "926db248cc1ba20f0c57631a7c8aef094f791937b905949e3460240e8bfa6fa483115a1b310b6e4369caebc5262888377b1ddaa5800ea496a2bdff0f9a1031e7129c9a20e35621e7f0b8baca0d87030f2ae7ca8593c8599677a06fd4b26009ead08fecac24caa9cf2cad3b470c8227415a7b1e0f2eab3fad96d70a209c8bb26c627677e2531b9435ca6e3c444d195b5f",
    .p_key              = "bca6fa3c67fd294e958f66fe8bd64f45f428f5bc8e9733a7",
    .p_iv               = "92a47f2833f1450d1da41717bdc6e83c"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_encrypt_160) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 192 Encrypt message_len=160",
    .p_plaintext        = "be8abf00901363987a82cc77d0ec91697ba3857f9e4f84bd79406c138d02698f003276d0449120bef4578d78fecabe8e070e11710b3f0a2744bd52434ec70015884c181ebdfd51c604a71c52e4c0e110bc408cd462b248a80b8a8ac06bb952ac1d7faed144807f1a731b7febcaf7835762defe92eccfc7a9944e1c702cffe6bc86733ed321423121085ac02df8962bcbc1937092eebf0e90a8b20e3dd8c244ae",
    .p_ciphertext       = "c82cf2c476dea8cb6a6e607a40d2f0391be82ea9ec84a537a6820f9afb997b76397d005424faa6a74dc4e8c7aa4a8900690f894b6d1dca80675393d2243adac762f159301e357e98b724762310cd5a7bafe1c2a030dba46fd93a9fdb89cc132ca9c17dc72031ec6822ee5a9d99dbca66c784c01b0885cbb62e29d97801927ec415a5d215158d325f9ee689437ad1b7684ad33c0d92739451ac87f39ff8c31b84",
    .p_key              = "162ad50ee64a0702aa551f571dedc16b2c1b6a1e4d4b5eee",
    .p_iv               = "24408038161a2ccae07b029bb66355c1"
};




// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_decrypt_16) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 192 Decrypt message_len=16",
    .p_plaintext        = "aa41179d880e6fe3b14818d6e4a62eb5",
    .p_ciphertext       = "39a9b42de19e512ab7f3043564c3515a",
    .p_key              = "8e2740fba157aef2422e442312d15c14d312553684fcdc15",
    .p_iv               = "324015878cdc82bfae59a2dc1ff34ea6"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_decrypt_32) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 192 Decrypt message_len=32",
    .p_plaintext        = "854e97e19b5c4fbd7a2ac7f8ddccdc8eac1a166832b58f05ae5088d7caba8fee",
    .p_ciphertext       = "2cd57dce7465d5ecde153e87ce45e62286c6b023a446dae3ec0fdc0648f29308",
    .p_key              = "0ac0d2add273d1a260c432c662b4be4d8d366edc3f402e40",
    .p_iv               = "0cc3744fa9cef13fe04a5ab6ac9b8de4"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_decrypt_48) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 192 Decrypt message_len=48",
    .p_plaintext        = "4a7a4dca5c555d3f0358be7db4af14f1322a8861a3cb977f029fdcbd8ee4a8d451f32d7865e6a2376edf67e4d1092e15",
    .p_ciphertext       = "a9968021d6df78ff2c4c236bdd9a55bc727b0dc506f44958b2041f0948860a3444588242ffbdcf2726001e2f6b5bd5fb",
    .p_key              = "3915d786c786731cfe35abe39fac714f5fa32c7ef3c6681b",
    .p_iv               = "a2d326a8226576e32e48f62b3da96c40"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_decrypt_64) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 192 Decrypt message_len=64",
    .p_plaintext        = "7b88fb0195a57ac61ccb3198a05517717523444da92d2e8c37840a7f7614c9effa6dd6f1d1a730ec350cd64b99738cfb3b962c791b2674929f936e894cbcb994",
    .p_ciphertext       = "f67581763d23326f699e05696043b4c553928c2a9f857377f12029fcae4acee992dba50697f617a51899fbd6367214d97bf5dbd9bdab7fd745cd2be431118793",
    .p_key              = "92317d4d38168a359118a0df0b7b45cbfdcc2011e7175d3c",
    .p_iv               = "75be95a6a54400b2e1b485e24ead18ed"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_decrypt_80) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 192 Decrypt message_len=80",
    .p_plaintext        = "07d471fa87fb5f267346aa4956c8bdb6c95493b1c19be8ca09deffd690d57463229352faf2878bc66a20f199d9f6b2378e6073c2cef002c628ce94d1adb5539bd15c4a51156f98f52bbe90a1905d35de",
    .p_ciphertext       = "a12b32199ae6484418ac7097fda9bb33f2ae421dfd795c9b553615e17546dcec6f3e7caf83334e6df035ac660a19a8b58d7cfe79310448337ee9716fe2b46ca7014726644c1eb9a6d5d4e28661e9b51a",
    .p_key              = "cd00048ce8ead5b5dff2346a86eac594b2a4194ca99fc89f",
    .p_iv               = "154cb1d42ad9e8d85ebb0b5189b6e1bc"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_decrypt_96) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 192 Decrypt message_len=96",
    .p_plaintext        = "8ab7b8b3c3c7d79b6d5cc605d3094a33756a8755140782967fb86297cf599eaea03e384018631b18425363e9ada971412d3eab03c63748749001e5b1a4a2e80ed7b915e6b9fa38e490301d6b45e27c0c72fd8cf6895de950d8d02774a8c33a6a",
    .p_ciphertext       = "69e771f860e0291e4477dce2a48f2c6ae2922b9337667b86f79cb38c16ea0523ecdb1e5135c54e385cfce9ebf945ed80988de466bcdb0cf92384b6544c9eec6637b656496cc65fce3e61935d51314bfc5aa38dad26e12efdde0139da897b95a5",
    .p_key              = "c4b39f1d90658aa1769a777956026573567bd0f3d6333b3b",
    .p_iv               = "f6085f2331e851db9c2654dacb5baf19"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_decrypt_112) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 192 Decrypt message_len=112",
    .p_plaintext        = "839238a996e51b542ab7aa55eae3150097291ca19e756325c326803926ea45ad6ef6b7790ce89d084bbb2ad2d95bc889f5d19ffd3092aff609d5e63b7324ea207ce22f8598f189563cc6e611f5ef25be06b6a78fc6a68270683542de69b0a44aee456d1138d0fd9ad4df68083633defc",
    .p_ciphertext       = "65cfb38f922f1716225472eb36a127327007f8f5c08479ca7beac4b0aee26f3bb130bbf1ff390ef344c2a4e0b8fa81f6acbbaa7a620d945a22ecdd128a4b3acc2658b1cb41020809fab87d1f9a74b76624f9fd5c2e59a649f0b9d0229b5855adeccefbe60092eba26abf65728318b1ca",
    .p_key              = "62cff862e7bef3569a380ea7ff40918e3afb5c7ad265cd5f",
    .p_iv               = "04d1cab2b002d9dd6c5b66add5d6148b"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_decrypt_128) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 192 Decrypt message_len=128",
    .p_plaintext        = "d50a55ea83ccd556e1d663424d3e19c1ad9b8d16ff01f5422accadf3dae07d597f1375aaa319994ef71e1753485660e418dc1d6767a37ae93d8e700fc639e5d7283a9ecc2945b4429e8203f33178f25ed67d231667af7b8f2994e3d904437fed121612a04eb3e4c230789d46e4409e24c7bd1f86ae502eee5a11af1cad5c98b5",
    .p_ciphertext       = "f5f05cedea55a4f47ce943463bd15ccc7ae4f6bda2b3f5c3ccc6495a8e2965791428f2757356c5223bd40f3b4bcfd404ea4b9f2d131f73fa0f4104a14a4427f45f1f883f75309b74ce81d5859d491b1218b67b44cfe91af93c436d219bdfac67fc5f841596ec0d315e78fbecd846183e2dadf2ca7f19d0077952eba12a01db1d",
    .p_key              = "2c25af9d60e1af42c7f0fc2fbf011637e6119bd0e8e9bab9",
    .p_iv               = "c264b21a1eca4b2c8bd8606d87e38471"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_decrypt_144) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 192 Decrypt message_len=144",
    .p_plaintext        = "c970a819cfb715f777e8b63167999ebe17c71ff505c3ff24cc6995430fad4013e1fc69ba5123072a7123e376d1f7de8cc610ada3fdd905a1476bc23724861e85dcf950db2b4982b60271752b49e438a20ef4e8e09cac0dc49ed15b84e32627e243814fee0430744ac675c7e5673d3f57a52360ec6ff8d18ed4b5bd8f1456c1f688825cb999789cad5e1b37a4b92ace3b",
    .p_ciphertext       = "8a21ea1381284bcdf818c2d4dfa976c13e5a3c253164ba1d30eccc27947c263457b43bff1c3d5e9c6fff27544d9419b0e7fc81d4a392a10e643e0eaf0bed571a3e3ee71a687e2d7900d7face0fc42a96ecc886864a60e9207536a285d9971a8ac427b70d6dd4ff8a340801e92b23f09ad62812e42fb6d87aed3b4f500664b7ac73d8708033251ef792fa054eab98b5e5",
    .p_key              = "c6680fe9a1968f899479eca1092beaac18ad945a42ce8f93",
    .p_iv               = "54130eea9e96a1199d3c090f690a479f"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_192_decrypt_160) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 192 Decrypt message_len=160",
    .p_plaintext        = "67d2dda6da26e21307973400600725727ae81415511772f4a09ad9903bcf90cc2c0dac58ba559a0109c54a9d6117b15bb574ca473e848047e9a54ee4abde76aff9849c44109d161f46442e1610d8b015cf36a010ed8efa3207fdfc8fcc548f145c027e44c5b0ec35c9886f4b9d6513a5bc10d0ea6bbbc26f54b183bcae27fb799d8872ff748fc459d55cfa255aae29d71b076d9b44c14d5ceba9332a763d9c94",
    .p_ciphertext       = "6928299c52b4f047926f8a541529da2d6bbaa399143ced8efb77ab47409d9a953a386c7abd6026f49831c717627c2a5e77bd2d433d4d130dacd927ea0d13a23d01a7cf39c6716dafb6ed552410ef5d27fb947be2c8782eee7829196c7edcf151c65f9a01f54f8d20f38b7da4a7e83a2f0127d59d3e2405d8674fc9f41b604f788f4715f9d3624eee57f387bfadd18a1f905e839c26b8617482347fab6d08845a",
    .p_key              = "509baf46fb9de34281dafcc3db79593bffa8426904302688",
    .p_iv               = "d6d86e0c82dd8788f4147a26f9a71c74"
};


// AES CBC - NIST CAVS 11.1 Monte Carlo 192 Encrypt
NRF_SECTION_ITEM_REGISTER(test_vector_aes_monte_carlo_data, test_vector_aes_t test_vector_aes_cbc_192_encrypt_monte_carlo) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC Encrypt Monte Carlo 192",
    .p_plaintext        = "cd0b8c8a8179ecb171b64c894a4d60fd",
    .p_ciphertext       = "e6457bfc3433e80299c52b2be418f582",
    .p_key              = "dea64f83cfe6a0a183ddbe865cfca059b3c615c1623d63fc",
    .p_iv               = "426fbc087b50b395c0fc81ef9fd6d1aa"
};

// AES CBC - NIST CAVS 11.1 Monte Carlo 192 Decrypt
NRF_SECTION_ITEM_REGISTER(test_vector_aes_monte_carlo_data, test_vector_aes_t test_vector_aes_cbc_192_decrypt_monte_carlo) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC Decrypt Monte Carlo 192",
    .p_plaintext        = "836424eadf8155aaf9a9a51391a1cf7e",
    .p_ciphertext       = "eb2c4e2712591ff13b8ac7870c9c404c",
    .p_key              = "a24ebd4d7a080c28caae984b5098a9ea38cf7280e2c5f122",
    .p_iv               = "c5aeb9b51ad5108371c59d0b90816310"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_192)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_256)

// AES CBC - Functional test using test vector NIST CAVS 11.1 CBC KeySbox 256 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_func_data, test_vector_aes_t test_vector_aes_cbc_256_functional) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 256 Functional",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "46f2fb342d6f0ab477476fc501242c5f",
    .p_key              = "c47b0294dbbbee0fec4757f22ffeee3587ca4730c3d33b691df38bab076bc558",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - Custom test vector - Encrypt - Message length 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_encrypt_c0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 256 Encrypt message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = "c9defba265de96a179a1128c2b5ef0edf6365c3064df88e2d46b2bfae20b599d",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Decrypt - Message length 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_decrypt_c0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 256 Decrypt message_len=0",
    .p_plaintext        = "",
    .p_ciphertext       = "",
    .p_key              = "c9defba265de96a179a1128c2b5ef0edf6365c3064df88e2d46b2bfae20b599d",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Encrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_encrypt_c3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 256 Encrypt message_len=1 pkcs7-padding",
    .p_plaintext        = "de",
    .p_ciphertext       = "5f4ae74bf458250d7ccab86c0fd78d5f",
    .p_key              = "c9defba265de96a179a1128c2b5ef0edf6365c3064df88e2d46b2bfae20b599d",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Decrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_decrypt_c3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 256 Decrypt message_len=1 pkcs7-padding",
    .p_plaintext        = "de0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f",                                       /**< The 15 padded bytes (0x0f) will be removed in the actual generated plaintext. */
    .p_ciphertext       = "5f4ae74bf458250d7ccab86c0fd78d5f",
    .p_key              = "c9defba265de96a179a1128c2b5ef0edf6365c3064df88e2d46b2bfae20b599d",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Encrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_encrypt_c4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 256 Encrypt message_len=16 pkcs7-padding",
    .p_plaintext        = "dededededededededededededededede",
    .p_ciphertext       = "e672ab8a22ae0d97eada096311f0ce7a88059d5133fad50d906286782f77c874",
    .p_key              = "c9defba265de96a179a1128c2b5ef0edf6365c3064df88e2d46b2bfae20b599d",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Decrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_decrypt_c4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 256 Decrypt message_len=16 pkcs7-padding",
    .p_plaintext        = "dededededededededededededededede10101010101010101010101010101010",       /**< The 16 padded bytes (0x10) will be removed in the actual generated plaintext. */
    .p_ciphertext       = "e672ab8a22ae0d97eada096311f0ce7a88059d5133fad50d906286782f77c874",
    .p_key              = "c9defba265de96a179a1128c2b5ef0edf6365c3064df88e2d46b2bfae20b599d",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Encrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_encrypt_c5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 256 Encrypt message_len=31 pkcs7-padding",
    .p_plaintext        = "dedededededededededededededededededededededededededededededede",
    .p_ciphertext       = "e672ab8a22ae0d97eada096311f0ce7a2440990504b280f31c303b966bc28f03",
    .p_key              = "c9defba265de96a179a1128c2b5ef0edf6365c3064df88e2d46b2bfae20b599d",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - Custom test vector - Decrypt - pkcs7-padding
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_decrypt_c5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_pad_pkcs7_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 256 Decrypt message_len=31 pkcs7-padding",
    .p_plaintext        = "dedededededededededededededededededededededededededededededede01",       /**< The padded byte (0x01) will be removed in the actual generated plaintext. */
    .p_ciphertext       = "e672ab8a22ae0d97eada096311f0ce7a2440990504b280f31c303b966bc28f03",
    .p_key              = "c9defba265de96a179a1128c2b5ef0edf6365c3064df88e2d46b2bfae20b599d",
    .p_iv               = "97ead25a84abd4a55268d1d347faee05"
};

// AES CBC - NIST CAVS 11.1 GFSBox Tests 256 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_encrypt_gfsb0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 256 Encrypt GFSBox 0",
    .p_plaintext        = "014730f80ac625fe84f026c60bfd547d",
    .p_ciphertext       = "5c9d844ed46f9885085e5d6a4f94c7d7",
    .p_key              = "0000000000000000000000000000000000000000000000000000000000000000",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - NIST CAVS 11.1 GFSBox Tests 256 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_decrypt_gfsb0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 256 Decrypt GFSBox 0",
    .p_plaintext        = "014730f80ac625fe84f026c60bfd547d",
    .p_ciphertext       = "5c9d844ed46f9885085e5d6a4f94c7d7",
    .p_key              = "0000000000000000000000000000000000000000000000000000000000000000",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - NIST CAVS 11.1 VarKey Tests 256 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_encrypt_vk0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 256 Encrypt VarKey 0",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "e35a6dcb19b201a01ebcfa8aa22b5759",
    .p_key              = "8000000000000000000000000000000000000000000000000000000000000000",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - NIST CAVS 11.1 VarKey Tests 256 Encrypt - Count 255
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_encrypt_vk255) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 256 Encrypt VarKey 255",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "4bf85f1b5d54adbc307b0a048389adcb",
    .p_key              = "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - NIST CAVS 11.1 VarKey Tests 256 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_decrypt_vk0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 256 Decrypt VarKey 0",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "e35a6dcb19b201a01ebcfa8aa22b5759",
    .p_key              = "8000000000000000000000000000000000000000000000000000000000000000",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - NIST CAVS 11.1 VarKey Tests 256 Decrypt - Count 255
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_decrypt_vk255) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 256 Decrypt VarKey 255",
    .p_plaintext        = "00000000000000000000000000000000",
    .p_ciphertext       = "4bf85f1b5d54adbc307b0a048389adcb",
    .p_key              = "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_encrypt_16) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 256 Encrypt message_len=16",
    .p_plaintext        = "6282b8c05c5c1530b97d4816ca434762",
    .p_ciphertext       = "6acc04142e100a65f51b97adf5172c41",
    .p_key              = "6ed76d2d97c69fd1339589523931f2a6cff554b15f738f21ec72dd97a7330907",
    .p_iv               = "851e8764776e6796aab722dbb644ace8"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_encrypt_32) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 256 Encrypt message_len=32",
    .p_plaintext        = "50e9eee1ac528009e8cbcd356975881f957254b13f91d7c6662d10312052eb00",
    .p_ciphertext       = "2fa0df722a9fd3b64cb18fb2b3db55ff2267422757289413f8f657507412a64c",
    .p_key              = "dce26c6b4cfb286510da4eecd2cffe6cdf430f33db9b5f77b460679bd49d13ae",
    .p_iv               = "fdeaa134c8d7379d457175fd1a57d3fc"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_encrypt_48) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 256 Encrypt message_len=48",
    .p_plaintext        = "8d3aa196ec3d7c9b5bb122e7fe77fb1295a6da75abe5d3a510194d3a8a4157d5c89d40619716619859da3ec9b247ced9",
    .p_ciphertext       = "608e82c7ab04007adb22e389a44797fed7de090c8c03ca8a2c5acd9e84df37fbc58ce8edb293e98f02b640d6d1d72464",
    .p_key              = "fe8901fecd3ccd2ec5fdc7c7a0b50519c245b42d611a5ef9e90268d59f3edf33",
    .p_iv               = "bd416cb3b9892228d8f1df575692e4d0"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_encrypt_64) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 256 Encrypt message_len=64",
    .p_plaintext        = "8b37f9148df4bb25956be6310c73c8dc58ea9714ff49b643107b34c9bff096a94fedd6823526abc27a8e0b16616eee254ab4567dd68e8ccd4c38ac563b13639c",
    .p_ciphertext       = "05d5c77729421b08b737e41119fa4438d1f570cc772a4d6c3df7ffeda0384ef84288ce37fc4c4c7d1125a499b051364c389fd639bdda647daa3bdadab2eb5594",
    .p_key              = "0493ff637108af6a5b8e90ac1fdf035a3d4bafd1afb573be7ade9e8682e663e5",
    .p_iv               = "c0cd2bebccbb6c49920bd5482ac756e8"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_encrypt_80) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 256 Encrypt message_len=80",
    .p_plaintext        = "c7917f84f747cd8c4b4fedc2219bdbc5f4d07588389d8248854cf2c2f89667a2d7bcf53e73d32684535f42318e24cd45793950b3825e5d5c5c8fcd3e5dda4ce9246d18337ef3052d8b21c5561c8b660e",
    .p_ciphertext       = "9c99e68236bb2e929db1089c7750f1b356d39ab9d0c40c3e2f05108ae9d0c30b04832ccdbdc08ebfa426b7f5efde986ed05784ce368193bb3699bc691065ac62e258b9aa4cc557e2b45b49ce05511e65",
    .p_key              = "9adc8fbd506e032af7fa20cf5343719de6d1288c158c63d6878aaf64ce26ca85",
    .p_iv               = "11958dc6ab81e1c7f01631e9944e620f"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_encrypt_96) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 256 Encrypt message_len=96",
    .p_plaintext        = "3adea6e06e42c4f041021491f2775ef6378cb08824165edc4f6448e232175b60d0345b9f9c78df6596ec9d22b7b9e76e8f3c76b32d5d67273f1d83fe7a6fc3dd3c49139170fa5701b3beac61b490f0a9e13f844640c4500f9ad3087adfb0ae10",
    .p_ciphertext       = "ac3d6dbafe2e0f740632fd9e820bf6044cd5b1551cbb9cc03c0b25c39ccb7f33b83aacfca40a3265f2bbff879153448acacb88fcfb3bb7b10fe463a68c0109f028382e3e557b1adf02ed648ab6bb895df0205d26ebbfa9a5fd8cebd8e4bee3dc",
    .p_key              = "73b8faf00b3302ac99855cf6f9e9e48518690a5906a4869d4dcf48d282faae2a",
    .p_iv               = "b3cb97a80a539912b8c21f450d3b9395"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_encrypt_112) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 256 Encrypt message_len=112",
    .p_plaintext        = "cf52e5c3954c51b94c9e38acb8c9a7c76aebdaa9943eae0a1ce155a2efdb4d46985d935511471452d9ee64d2461cb2991d59fc0060697f9a671672163230f367fed1422316e52d29eceacb8768f56d9b80f6d278093c9a8acd3cfd7edd8ebd5c293859f64d2f8486ae1bd593c65bc014",
    .p_ciphertext       = "34df561bd2cfebbcb7af3b4b8d21ca5258312e7e2e4e538e35ad2490b6112f0d7f148f6aa8d522a7f3c61d785bd667db0e1dc4606c318ea4f26af4fe7d11d4dcff0456511b4aed1a0d91ba4a1fd6cd9029187bc5881a5a07fe02049d39368e83139b12825bae2c7be81e6f12c61bb5c5",
    .p_key              = "9ddf3745896504ff360a51a3eb49c01b79fccebc71c3abcb94a949408b05b2c9",
    .p_iv               = "e79026639d4aa230b5ccffb0b29d79bc"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_encrypt_128) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 256 Encrypt message_len=128",
    .p_plaintext        = "256fd73ce35ae3ea9c25dd2a9454493e96d8633fe633b56176dce8785ce5dbbb84dbf2c8a2eeb1e96b51899605e4f13bbc11b93bf6f39b3469be14858b5b720d4a522d36feed7a329c9b1e852c9280c47db8039c17c4921571a07d1864128330e09c308ddea1694e95c84500f1a61e614197e86a30ecc28df64ccb3ccf5437aa",
    .p_ciphertext       = "90b7b9630a2378f53f501ab7beff039155008071bc8438e789932cfd3eb1299195465e6633849463fdb44375278e2fdb1310821e6492cf80ff15cb772509fb426f3aeee27bd4938882fd2ae6b5bd9d91fa4a43b17bb439ebbe59c042310163a82a5fe5388796eee35a181a1271f00be29b852d8fa759bad01ff4678f010594cd",
    .p_key              = "458b67bf212d20f3a57fce392065582dcefbf381aa22949f8338ab9052260e1d",
    .p_iv               = "4c12effc5963d40459602675153e9649"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_encrypt_144) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 256 Encrypt message_len=144",
    .p_plaintext        = "026006c4a71a180c9929824d9d095b8faaa86fc4fa25ecac61d85ff6de92dfa8702688c02a282c1b8af4449707f22d75e91991015db22374c95f8f195d5bb0afeb03040ff8965e0e1339dba5653e174f8aa5a1b39fe3ac839ce307a4e44b4f8f1b0063f738ec18acdbff2ebfe07383e734558723e741f0a1836dafdf9de82210a9248bc113b3c1bc8b4e252ca01bd803",
    .p_ciphertext       = "0254b23463bcabec5a395eb74c8fb0eb137a07bc6f5e9f61ec0b057de305714f8fa294221c91a159c315939b81e300ee902192ec5f15254428d8772f79324ec43298ca21c00b370273ee5e5ed90e43efa1e05a5d171209fe34f9f29237dba2a6726650fd3b1321747d1208863c6c3c6b3e2d879ab5f25782f08ba8f2abbe63e0bedb4a227e81afb36bb6645508356d34",
    .p_key              = "d2412db0845d84e5732b8bbd642957473b81fb99ca8bff70e7920d16c1dbec89",
    .p_iv               = "51c619fcf0b23f0c7925f400a6cacb6d"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_encrypt_160) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC 256 Encrypt message_len=160",
    .p_plaintext        = "0c63d413d3864570e70bb6618bf8a4b9585586688c32bba0a5ecc1362fada74ada32c52acfd1aa7444ba567b4e7daaecf7cc1cb29182af164ae5232b002868695635599807a9a7f07a1f137e97b1e1c9dabc89b6a5e4afa9db5855edaa575056a8f4f8242216242bb0c256310d9d329826ac353d715fa39f80cec144d6424558f9f70b98c920096e0f2c855d594885a00625880e9dfb734163cecef72cf030b8",
    .p_ciphertext       = "fc5873e50de8faf4c6b84ba707b0854e9db9ab2e9f7d707fbba338c6843a18fc6facebaf663d26296fb329b4d26f18494c79e09e779647f9bafa87489630d79f4301610c2300c19dbf3148b7cac8c4f4944102754f332e92b6f7c5e75bc6179eb877a078d4719009021744c14f13fd2a55a2b9c44d18000685a845a4f632c7c56a77306efa66a24d05d088dcd7c13fe24fc447275965db9e4d37fbc9304448cd",
    .p_key              = "48be597e632c16772324c8d3fa1d9c5a9ecd010f14ec5d110d3bfec376c5532b",
    .p_iv               = "d6d581b8cf04ebd3b6eaa1b53f047ee1"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_decrypt_16) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 256 Decrypt message_len=16",
    .p_plaintext        = "07270d0e63aa36daed8c6ade13ac1af1",
    .p_ciphertext       = "d51d19ded5ca4ae14b2b20b027ffb020",
    .p_key              = "43e953b2aea08a3ad52d182f58c72b9c60fbe4a9ca46a3cb89e3863845e22c9e",
    .p_iv               = "ddbbb0173f1e2deb2394a62aa2a0240e"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_decrypt_32) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 256 Decrypt message_len=32",
    .p_plaintext        = "98a8a9d84356bf403a9ccc384a06fe043dfeecb89e59ce0cb8bd0a495ef76cf0",
    .p_ciphertext       = "6cb4e2f4ddf79a8e08c96c7f4040e8a83266c07fc88dd0074ee25b00d445985a",
    .p_key              = "addf88c1ab997eb58c0455288c3a4fa320ada8c18a69cc90aa99c73b174dfde6",
    .p_iv               = "60cc50e0887532e0d4f3d2f20c3c5d58"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_decrypt_48) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 256 Decrypt message_len=48",
    .p_plaintext        = "0faa5d01b9afad3bb519575daaf4c60a5ed4ca2ba20c625bc4f08799addcf89d19796d1eff0bd790c622dc22c1094ec7",
    .p_ciphertext       = "e4046d05385ab789c6a72866e08350f93f583e2a005ca0faecc32b5cfc323d461c76c107307654db5566a5bd693e227c",
    .p_key              = "54682728db5035eb04b79645c64a95606abb6ba392b6633d79173c027c5acf77",
    .p_iv               = "2eb94297772851963dd39a1eb95d438f"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_decrypt_64) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 256 Decrypt message_len=64",
    .p_plaintext        = "82fec664466d585023821c2e39a0c43345669a41244d05018a23d7159515f8ff4d88b01cd0eb83070d0077e065d74d7373816b61505718f8d4f270286a59d45e",
    .p_ciphertext       = "6c24f19b9c0b18d7126bf68090cb8ae72db3ca7eabb594f506aae7a2493e5326a5afae4ec4d109375b56e2b6ff4c9cf639e72c63dc8114c796df95b3c6b62021",
    .p_key              = "7482c47004aef406115ca5fd499788d582efc0b29dc9e951b1f959406693a54f",
    .p_iv               = "485ebf2215d20b816ea53944829717ce"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_decrypt_80) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 256 Decrypt message_len=80",
    .p_plaintext        = "8d22db30c4253c3e3add9685c14d55b05f7cf7626c52cccfcbe9b99fd8913663b8b1f22e277a4cc3d0e7e978a34782eb876867556ad4728486d5e890ea738243e3700a696d6eb58cd81c0e60eb121c50",
    .p_ciphertext       = "d50ea48c8962962f7c3d301fa9f877245026c204a7771292cddca1e7ffebbef00e86d72910b7d8a756dfb45c9f1040978bb748ca537edd90b670ecee375e15d98582b9f93b6355adc9f80f4fb2108fb9",
    .p_key              = "3ae38d4ebf7e7f6dc0a1e31e5efa7ca123fdc321e533e79fedd5132c5999ef5b",
    .p_iv               = "36d55dc9edf8669beecd9a2a029092b9"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_decrypt_96) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 256 Decrypt message_len=96",
    .p_plaintext        = "0b6e2a8213169b3b78db6de324e286f0366044e035c6970afbf0a1a5c32a05b24ba706cd9c6609737651a81b2bcf4c681dc0861983a5aec76e6c8b244112d64d489e84328974737394b83a39459011727162652b7aa793bfb1b71488b7dec96b",
    .p_ciphertext       = "31e4677a17aed120bd3af69fbb0e4b645b9e8c104e280b799ddd49f1e241c3ccb7d40e1c6ff226bf04f8049c51a86e2981cf1331c824d7d451746ccf77fc22fd3717001ee51913d81f7a06fb0037f309957579f695670f2c4c7397d2d990374e",
    .p_key              = "d30bfc0b2a19d5b8b6f8f46ab7f444ee136a7fa3fbdaf530cc3e8976339afcc4",
    .p_iv               = "80be76a7f885d2c06b37d6a528fae0cd"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_decrypt_112) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 256 Decrypt message_len=112",
    .p_plaintext        = "f7e0f79cfddd15ed3600ab2d29c56ba3c8e96d1a896aff6dec773e6ea4710a77f2f4ec646b76efda6428c175d007c84aa9f4b18c5e1bac5f27f7307b737655eee813f7e1f5880a37ac63ad1666e7883083b648454d45786f53ea3db1b5129291138abe40c79fcb7ab7c6f6b9ea133b5f",
    .p_ciphertext       = "d99771963b7ae5202e382ff8c06e035367909cd24fe5ada7f3d39bfaeb5de98b04eaf4989648e00112f0d2aadb8c5f2157b64581450359965140c141e5fb631e43469d65d1b7370eb3b396399fec32cced294a5eee46d6547f7bbd49dee148b4bc31d6c493cfd28f3908e36cb698629d",
    .p_key              = "64a256a663527ebea71f8d770990b4cee4a2d3afbfd33fb12c7ac300ef59e49a",
    .p_iv               = "18cce9147f295c5c00dbe0424089d3b4"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_decrypt_128) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 256 Decrypt message_len=128",
    .p_plaintext        = "cfc155a3967de347f58fa2e8bbeb4183d6d32f7427155e6ab39cddf2e627c572acae02f1f243f3b784e73e21e7e520eacd3befafbee814867334c6ee8c2f0ee7376d3c72728cde7813173dbdfe3357deac41d3ae2a04229c0262f2d109d01f5d03e7f848fb50c28849146c02a2f4ebf7d7ffe3c9d40e31970bf151873672ef2b",
    .p_ciphertext       = "b32e2b171b63827034ebb0d1909f7ef1d51c5f82c1bb9bc26bc4ac4dccdee8357dca6154c2510ae1c87b1b422b02b621bb06cac280023894fcff3406af08ee9be1dd72419beccddff77c722d992cdcc87e9c7486f56ab406ea608d8c6aeb060c64cf2785ad1a159147567e39e303370da445247526d95942bf4d7e88057178b0",
    .p_key              = "31358e8af34d6ac31c958bbd5c8fb33c334714bffb41700d28b07f11cfe891e7",
    .p_iv               = "144516246a752c329056d884daf3c89d"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_decrypt_144) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 256 Decrypt message_len=144",
    .p_plaintext        = "a0bb1d2fdeb7e6bf34c690fe7b72a5e9d65796aa57982fe340c286d6923dbddb426566ff58e9c0b3af52e4db446f6cc5daa5bfcf4e3c85db5a5638e670c370cce128db22c97542a64a63846f18a228d3462a11376dcb71f66ec52ebda474f7b6752915b0801797974bc51eb1218127fed60f1009430eb5089fb3ba5f28fad24c518ccddc2501393ceb6dffc46a159421",
    .p_ciphertext       = "ba21db8ec170fa4d73cfc381687f3fa188dd2d012bef48007f3dc88329e22ba32fe235a315be362546468b9db6af6705c6e5d4d36822f42883c08d4a994cc454a7db292c4ca1f4b62ebf8e479a5d545d6af9978d2cfee7bc80999192c2c8662ce9b4be11af40bd68f3e2d5685bb28c0f3dc08017c0aba8263e6fdc45ed7f9893bf14fd3a86c418a35c5667e642d59985",
    .p_key              = "5b4b69339891db4e3337c3486f439dfbd0fb2a782ca71ef0059819d51669d93c",
    .p_iv               = "2b28a2d19ba9ecd149dae96622c21769"
};

// AES CBC - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cbc_256_decrypt_160) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC 256 Decrypt message_len=160",
    .p_plaintext        = "bfe5c6354b7a3ff3e192e05775b9b75807de12e38a626b8bf0e12d5fff78e4f1775aa7d792d885162e66d88930f9c3b2cdf8654f56972504803190386270f0aa43645db187af41fcea639b1f8026ccdd0c23e0de37094a8b941ecb7602998a4b2604e69fc04219585d854600e0ad6f99a53b2504043c08b1c3e214d17cde053cbdf91daa999ed5b47c37983ba3ee254bc5c793837daaa8c85cfc12f7f54f699f",
    .p_ciphertext       = "5b97a9d423f4b97413f388d9a341e727bb339f8e18a3fac2f2fb85abdc8f135deb30054a1afdc9b6ed7da16c55eba6b0d4d10c74e1d9a7cf8edfaeaa684ac0bd9f9d24ba674955c79dc6be32aee1c260b558ff07e3a4d49d24162011ff254db8be078e8ad07e648e6bf5679376cb4321a5ef01afe6ad8816fcc7634669c8c4389295c9241e45fff39f3225f7745032daeebe99d4b19bcb215d1bfdb36eda2c24",
    .p_key              = "87725bd43a45608814180773f0e7ab95a3c859d83a2130e884190e44d14c6996",
    .p_iv               = "e49651988ebbb72eb8bb80bb9abbca34"
};

// AES CBC - NIST CAVS 11.1 Monte Carlo 256 Encrypt
NRF_SECTION_ITEM_REGISTER(test_vector_aes_monte_carlo_data, test_vector_aes_t test_vector_aes_cbc_256_encrypt_monte_carlo) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CBC Encrypt Monte Carlo 256",
    .p_plaintext        = "90ed17475f0a62bc381ba1f3ffbfff33",
    .p_ciphertext       = "bade1667b42f537f0cb3f5573a949aaa",
    .p_key              = "632bac4fe4db44cfcf18cfa90b43f86f378611b8d968595eb89e7ae98624564a",
    .p_iv               = "ff8127621be616803e3f002377730185"
};

// AES CBC - NIST CAVS 11.1 Monte Carlo 256 Decrypt
NRF_SECTION_ITEM_REGISTER(test_vector_aes_monte_carlo_data, test_vector_aes_t test_vector_aes_cbc_256_decrypt_monte_carlo) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cbc_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CBC Decrypt Monte Carlo 256",
    .p_plaintext        = "9be831799a79b0955241f308f0d5b2e1",
    .p_ciphertext       = "27a1d5c10fe45b801d15f56e654a70f0",
    .p_key              = "31397ad8cc79c519e0f46e0f70303587e38958d70723b771552336b7771f6311",
    .p_iv               = "4139cb54eeac3fcf36ed72941122c40f"
};

#endif // #if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC_256)

/*lint -restore */

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CBC)
