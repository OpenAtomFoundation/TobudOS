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

/**@brief CFB test vectors can be found on NIST web pages.
 *
 * https://csrc.nist.gov/projects/cryptographic-algorithm-validation-program/block-ciphers#AES
 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CFB)

/*lint -save -e91 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CFB_128)

// AES CFB8 - Functional test using test vector NIST CAVS 11.1 CFB8 KeySbox 128 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_func_data, test_vector_aes_t test_vector_aes_cfb_128_functional) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 128 Functional",
    .p_plaintext        = "00",
    .p_ciphertext       = "6d",
    .p_key              = "10a58869d74be5a374cf867cfb473859",
    .p_iv               = "00000000000000000000000000000000"
};


// AES CFB8 - NIST CAVS 11.1 GFSBox Tests 128 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_encrypt_gfsb0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 128 Encrypt GFSBox 0",
    .p_plaintext        = "00",
    .p_ciphertext       = "03",
    .p_key              = "00000000000000000000000000000000",
    .p_iv               = "f34481ec3cc627bacd5dc3fb08f273e6"
};

// AES CFB8 - NIST CAVS 11.1 GFSBox Tests 128 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_decrypt_gfsb0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 128 Decrypt GFSBox 0",
    .p_plaintext        = "00",
    .p_ciphertext       = "08",
    .p_key              = "00000000000000000000000000000000",
    .p_iv               = "58c8e00b2631686d54eab84b91f0aca1"
};

// AES CFB8 - NIST CAVS 11.1 VarKey Tests 128 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_encrypt_vk0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 128 Encrypt VarKey 0",
    .p_plaintext        = "00",
    .p_ciphertext       = "0e",
    .p_key              = "80000000000000000000000000000000",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CFB8 - NIST CAVS 11.1 VarKey Tests 128 Encrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_encrypt_vk127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 128 Encrypt VarKey 127",
    .p_plaintext        = "00",
    .p_ciphertext       = "a1",
    .p_key              = "ffffffffffffffffffffffffffffffff",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CFB8 - NIST CAVS 11.1 VarKey Tests 128 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_decrypt_vk0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 128 Decrypt VarKey 0",
    .p_plaintext        = "00",
    .p_ciphertext       = "0e",
    .p_key              = "80000000000000000000000000000000",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CFB8 - NIST CAVS 11.1 VarKey Tests 128 Decrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_decrypt_vk127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,

    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 128 Decrypt VarKey 127",
    .p_plaintext        = "00",
    .p_ciphertext       = "a1",
    .p_key              = "ffffffffffffffffffffffffffffffff",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CFB8 - NIST CAVS 11.1 VarTxt Tests 128 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_encrypt_vt0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 128 Encrypt VarTxt 0",
    .p_plaintext        = "00",
    .p_ciphertext       = "3a",
    .p_key              = "00000000000000000000000000000000",
    .p_iv               = "80000000000000000000000000000000"
};

// AES CFB8 - NIST CAVS 11.1 VarTxt Tests 128 Encrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_encrypt_vt127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 128 Encrypt VarTxt 127",
    .p_plaintext        = "00",
    .p_ciphertext       = "3f",
    .p_key              = "00000000000000000000000000000000",
    .p_iv               = "ffffffffffffffffffffffffffffffff"
};

// AES CFB8 - NIST CAVS 11.1 VarTxt Tests 128 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_decrypt_vt0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,

    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 128 Decrypt VarTxt 0",
    .p_plaintext        = "00",
    .p_ciphertext       = "3a",
    .p_key              = "00000000000000000000000000000000",
    .p_iv               = "80000000000000000000000000000000"
};

// AES CFB8 - NIST CAVS 11.1 VarTxt Tests 128 Decrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_decrypt_vt127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 128 Decrypt VarTxt 127",
    .p_plaintext        = "00",
    .p_ciphertext       = "3f",
    .p_key              = "00000000000000000000000000000000",
    .p_iv               = "ffffffffffffffffffffffffffffffff"
};


// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_encrypt_1) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,

    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 128 Encrypt message_len=1",
    .p_plaintext        = "61",
    .p_ciphertext       = "24",
    .p_key              = "c57d699d89df7cfbef71c080a6b10ac3",
    .p_iv               = "fcb2bc4c006b87483978796a2ae2c42e"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_encrypt_2) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 128 Encrypt message_len=2",
    .p_plaintext        = "aacd",
    .p_ciphertext       = "5066",
    .p_key              = "0d8f3dc3edee60db658bb97faf46fba3",
    .p_iv               = "e481fdc42e606b96a383c0a1a5520ebb"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_encrypt_3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,

    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 128 Encrypt message_len=3",
    .p_plaintext        = "c90635",
    .p_ciphertext       = "d27691",
    .p_key              = "c8fe9bf77b930f46d2078b8c0e657cd4",
    .p_iv               = "f475c64991b20eaee183a22629e21e22"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_encrypt_4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 128 Encrypt message_len=4",
    .p_plaintext        = "b2a22ced",
    .p_ciphertext       = "73f3aebf",
    .p_key              = "280cf81af5cc7e7363579c1da03390e6",
    .p_iv               = "5d6cf4722d0e21f1d9ced53a0e36c342"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_encrypt_5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 128 Encrypt message_len=5",
    .p_plaintext        = "b84a90fc6d",
    .p_ciphertext       = "1a9a61c307",
    .p_key              = "5d5e7f20e0a66d3e09e0e5a9912f8a46",
    .p_iv               = "052d7ea0ad1f2956a23b27afe1d87b6b"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_encrypt_6) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 128 Encrypt message_len=6",
    .p_plaintext        = "14253472e99d",
    .p_ciphertext       = "cfc247e33a3b",
    .p_key              = "ec89fb348787cf902ca973c47081438d",
    .p_iv               = "528fe95c711bd13f37bc52cc9e96d45c"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_encrypt_7) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 128 Encrypt message_len=7",
    .p_plaintext        = "9b1a5a1369166e",
    .p_ciphertext       = "b7ab2a4cc71904",
    .p_key              = "6607987c354809cba818639dcd185147",
    .p_iv               = "552c101a0b7c0ca143af258453937fa3"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_encrypt_8) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 128 Encrypt message_len=8",
    .p_plaintext        = "324015878cdc82bf",
    .p_ciphertext       = "873250152fc6a5bb",
    .p_key              = "c028e6bf2b749ffa86759f2f84e93cb0",
    .p_iv               = "288c752d9faccf367e5d0cca1fa6ec3b"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_encrypt_9) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 128 Encrypt message_len=9",
    .p_plaintext        = "57c1a30e48166d9640",
    .p_ciphertext       = "e9a8c3b776edd39e3d",
    .p_key              = "d01da95d2c2a61da06ea78cfba59cc30",
    .p_iv               = "f9a393ad90814faf262e3a5b1d97592e"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Encrypt - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_encrypt_10) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 128 Encrypt message_len=10",
    .p_plaintext        = "87efeb8d559ed3367728",
    .p_ciphertext       = "8e9c50425614d540ce11",
    .p_key              = "3a6f9159263fa6cef2a075caface5817",
    .p_iv               = "0fc23662b7dbf73827f0c7de321ca36e"
};



// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_decrypt_1) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 128 Decrypt message_len=1",
    .p_plaintext        = "09",
    .p_ciphertext       = "21",
    .p_key              = "03edfe082550bd5ac8ddf64f42a0547f",
    .p_iv               = "52acd8dab62c981da08e51939cc08dab"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_decrypt_2) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 128 Decrypt message_len=2",
    .p_plaintext        = "eb24",
    .p_ciphertext       = "4878",
    .p_key              = "38cf776750162edc63c3b5dbe311ab9f",
    .p_iv               = "98fbbd288872c40f1926b16ecaec1561"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_decrypt_3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 128 Decrypt message_len=3",
    .p_plaintext        = "910949",
    .p_ciphertext       = "aeb6d2",
    .p_key              = "c9053c87c3e56bc5e52bd31f6545f991",
    .p_iv               = "b8f9640d0923da13fe6eb87b01f0cfa0"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_decrypt_4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 128 Decrypt message_len=4",
    .p_plaintext        = "3b7f1f1c",
    .p_ciphertext       = "83cab2f3",
    .p_key              = "e96771f5f20a89ee871261d2d18e1e46",
    .p_iv               = "6e86403e33396655907ae06ef192262f"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_decrypt_5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 128 Decrypt message_len=5",
    .p_plaintext        = "17b9b9e16d",
    .p_ciphertext       = "49f73e652b",
    .p_key              = "92ad13ecb60bde1bb3b34ce07867672b",
    .p_iv               = "f95a4060b8f80e3f839d4c3ca33dad94"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_decrypt_6) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 128 Decrypt message_len=6",
    .p_plaintext        = "ca989fa4e818",
    .p_ciphertext       = "b0eacbf2ca46",
    .p_key              = "eb57b8dd076e7bbb33d4bfc4d7ecb27e",
    .p_iv               = "51135997a067dcd2e016c57134c5fa52"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_decrypt_7) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 128 Decrypt message_len=7",
    .p_plaintext        = "5405da1186b7e0",
    .p_ciphertext       = "08fbef9b2a369a",
    .p_key              = "70abc48bb1be490183f0fe3df56195ff",
    .p_iv               = "e251f179174b71ee1e488ab3dd200483"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_decrypt_8) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 128 Decrypt message_len=8",
    .p_plaintext        = "d497df73afb9787c",
    .p_ciphertext       = "b3cb9d8892423aeb",
    .p_key              = "1273b8e0eee1a1ca827059b4d0a3a55d",
    .p_iv               = "622cab49092d026f554dd98a6441dc26"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_decrypt_9) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 128 Decrypt message_len=9",
    .p_plaintext        = "f203bcd402b65919da",
    .p_ciphertext       = "01992a986279c3685e",
    .p_key              = "49437e06b6faa5f20fd98bf71f8ff554",
    .p_iv               = "63c818e0d3cb5b7054ef3e1e87df0e12"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 128 Decrypt - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_128_decrypt_10) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 128 Decrypt message_len=10",
    .p_plaintext        = "feff4e2e2458addf2a54",
    .p_ciphertext       = "c9c284e9abbfe6fb11fe",
    .p_key              = "6399c1dc068ba3509845628fa9ed1a96",
    .p_iv               = "1157c2766c86b754df485be9dd5851df"
};


// AES CFB8 - NIST CAVS 11.1 Monte Carlo 128 Encrypt
NRF_SECTION_ITEM_REGISTER(test_vector_aes_monte_carlo_data, test_vector_aes_t test_vector_aes_cfb_128_encrypt_monte_carlo) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 Encrypt Monte Carlo 128",
    .p_plaintext        = "b7",
    .p_ciphertext       = "48",
    .p_key              = "4f139e69f5f6b81258fb612efc6464ae",
    .p_iv               = "150af93612b3630f898e52febf1e4e41"
};

// AES CFB8 - NIST CAVS 11.1 Monte Carlo 128 Decrypt
NRF_SECTION_ITEM_REGISTER(test_vector_aes_monte_carlo_data, test_vector_aes_t test_vector_aes_cfb_128_decrypt_monte_carlo) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_128_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 Decrypt Monte Carlo 128",
    .p_plaintext        = "af",
    .p_ciphertext       = "c9",
    .p_key              = "78180bac8a1c9782d705a82c8632b0a0",
    .p_iv               = "41c7c979f3d184f7aa61fb5c5ed6219d"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CFB_128)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CFB_192)

// AES CFB8 - Functional test using test vector NIST CAVS 11.1 CFB8 KeySbox 192 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_func_data, test_vector_aes_t test_vector_aes_cfb_192_functional) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 192 Functional",
    .p_plaintext        = "00",
    .p_ciphertext       = "09",
    .p_key              = "e9f065d7c13573587f7875357dfbb16c53489f6a4bd0f7cd",
    .p_iv               = "00000000000000000000000000000000"
};


// AES CFB8 - NIST CAVS 11.1 GFSBox Tests 192 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_encrypt_gfsb0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 192 Encrypt GFSBox 0",
    .p_plaintext        = "00",
    .p_ciphertext       = "27",
    .p_key              = "000000000000000000000000000000000000000000000000",
    .p_iv               = "1b077a6af4b7f98229de786d7516b639"
};

// AES CFB8 - NIST CAVS 11.1 GFSBox Tests 192 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_decrypt_gfsb0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 192 Decrypt GFSBox 0",
    .p_plaintext        = "00",
    .p_ciphertext       = "27",
    .p_key              = "000000000000000000000000000000000000000000000000",
    .p_iv               = "1b077a6af4b7f98229de786d7516b639"
};

// AES CFB8 - NIST CAVS 11.1 VarKey Tests 192 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_encrypt_vk0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 192 Encrypt VarKey 0",
    .p_plaintext        = "00",
    .p_ciphertext       = "de",
    .p_key              = "800000000000000000000000000000000000000000000000",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CFB8 - NIST CAVS 11.1 VarKey Tests 192 Encrypt - Count 191
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_encrypt_vk191) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 192 Encrypt VarKey 191",
    .p_plaintext        = "00",
    .p_ciphertext       = "dd",
    .p_key              = "ffffffffffffffffffffffffffffffffffffffffffffffff",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CFB8 - NIST CAVS 11.1 VarKey Tests 192 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_decrypt_vk0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 192 Decrypt VarKey 0",
    .p_plaintext        = "00",
    .p_ciphertext       = "de",
    .p_key              = "800000000000000000000000000000000000000000000000",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CFB8 - NIST CAVS 11.1 VarKey Tests 192 Decrypt - Count 191
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_decrypt_vk191) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 192 Decrypt VarKey 191",
    .p_plaintext        = "00",
    .p_ciphertext       = "dd",
    .p_key              = "ffffffffffffffffffffffffffffffffffffffffffffffff",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CFB8 - NIST CAVS 11.1 VarTxt Tests 192 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_encrypt_vt0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 192 Encrypt VarTxt 0",
    .p_plaintext        = "00",
    .p_ciphertext       = "6c",
    .p_key              = "000000000000000000000000000000000000000000000000",
    .p_iv               = "80000000000000000000000000000000"
};

// AES CFB8 - NIST CAVS 11.1 VarTxt Tests 192 Encrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_encrypt_vt127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 192 Encrypt VarTxt 127",
    .p_plaintext        = "00",
    .p_ciphertext       = "b1",
    .p_key              = "000000000000000000000000000000000000000000000000",
    .p_iv               = "ffffffffffffffffffffffffffffffff"
};

// AES CFB8 - NIST CAVS 11.1 VarTxt Tests 192 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_decrypt_vt0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 192 Decrypt VarTxt 0",
    .p_plaintext        = "00",
    .p_ciphertext       = "6c",
    .p_key              = "00000000000000000000000000000000",
    .p_iv               = "80000000000000000000000000000000"
};

// AES CFB8 - NIST CAVS 11.1 VarTxt Tests 192 Decrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_decrypt_vt127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 192 Decrypt VarTxt 127",
    .p_plaintext        = "00",
    .p_ciphertext       = "b1",
    .p_key              = "000000000000000000000000000000000000000000000000",
    .p_iv               = "ffffffffffffffffffffffffffffffff"
};


// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_encrypt_1) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 192 Encrypt message_len=1",
    .p_plaintext        = "54",
    .p_ciphertext       = "6d",
    .p_key              = "32a1b0e3da368db563d7316b9779d3327e53d9a6d287ed97",
    .p_iv               = "3dd0e7e21f09d5842f3a699da9b57346"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_encrypt_2) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 192 Encrypt message_len=2",
    .p_plaintext        = "a456",
    .p_ciphertext       = "8fb6",
    .p_key              = "a6381dcc18dd85d7729c1dce90743bbe1df580d857f5b9c4",
    .p_iv               = "c0ac501fad7f4a1465daf32e18fc1a4f"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_encrypt_3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 192 Encrypt message_len=3",
    .p_plaintext        = "fd115d",
    .p_ciphertext       = "c4c016",
    .p_key              = "d08dbee4732c7ffc544c1695b201d30e795037325ef0aa18",
    .p_iv               = "a1e39aeeb972a8d70aa0fc7d6fac6eac"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_encrypt_4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 192 Encrypt message_len=4",
    .p_plaintext        = "88e07061",
    .p_ciphertext       = "8bb630ba",
    .p_key              = "277185a4a440869920f523c4d578fc5bedd33aee8d2ebaf7",
    .p_iv               = "67be00572f82aabc13d6e5a2e51d1f08"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_encrypt_5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 192 Encrypt message_len=5",
    .p_plaintext        = "36664e222d",
    .p_ciphertext       = "20855555d1",
    .p_key              = "83f70fdce47306fcbb8c21b6a8b3209f7ec185fef4deebd4",
    .p_iv               = "ff73b310cf7e62ce6f501092fa6cc888"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_encrypt_6) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 192 Encrypt message_len=6",
    .p_plaintext        = "b57f12342a62",
    .p_ciphertext       = "73ff9bf3ec4b",
    .p_key              = "c5be271a29f4a29e085e8e98196601dcb88ccc03e559a304",
    .p_iv               = "9f51fa2eb8a084718f7240e47d135dce"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_encrypt_7) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 192 Encrypt message_len=7",
    .p_plaintext        = "6dcede43c2ee65",
    .p_ciphertext       = "7c897658282220",
    .p_key              = "9c55322e6d495be01076d4b80371ad1479ae5636ff9861f5",
    .p_iv               = "2b79cfc1ff37254dedf5924a6b61e3e0"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_encrypt_8) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 192 Encrypt message_len=8",
    .p_plaintext        = "ceda42527871f802",
    .p_ciphertext       = "d92ff89045b1917f",
    .p_key              = "6e78ccece7d1b2a3c08cf0de738bee33cbbbf78d9bf4922c",
    .p_iv               = "4bbe15b1e94a7b97250a2136d8804e46"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_encrypt_9) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 192 Encrypt message_len=9",
    .p_plaintext        = "1b0a63d73464ab3c8a",
    .p_ciphertext       = "5485847e5d3c2e2cc4",
    .p_key              = "13c98665746f7825b37b404916240adbd1e4364be1d05c63",
    .p_iv               = "0e479fbd5f3961f38b8a26be1f2d65c5"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Encrypt - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_encrypt_10) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 192 Encrypt message_len=10",
    .p_plaintext        = "d3d8b9b984adc24237ee",
    .p_ciphertext       = "3879fea72ac99929e53a",
    .p_key              = "537e7bf661fd4024a024613f15b13690f7d0c847c1e18965",
    .p_iv               = "3a81f9d9d3c155b0caad5d73349476fc"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_decrypt_1) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 192 Decrypt message_len=1",
    .p_plaintext        = "2d",
    .p_ciphertext       = "25",
    .p_key              = "7dbdc15ad4034ed828dc862799b7adc9abd68eaf9d526d5d",
    .p_iv               = "4359683af5a3a85c248fb7f5506f317b"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_decrypt_2) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 192 Decrypt message_len=2",
    .p_plaintext        = "7b04",
    .p_ciphertext       = "0678",
    .p_key              = "3a2cdf9c9608c1dd6233d03dd855293b0885915114b25279",
    .p_iv               = "e7a28ee34acc52128ddae658ec6398a2"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_decrypt_3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 192 Decrypt message_len=3",
    .p_plaintext        = "658d0a",
    .p_ciphertext       = "037cf6",
    .p_key              = "c984b99a6cc5bc88003143cbe4b755e6e30ba94114f7ad1e",
    .p_iv               = "41e3b8fd138f8c358dfeef420302f634"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_decrypt_4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 192 Decrypt message_len=4",
    .p_plaintext        = "2d191f2f",
    .p_ciphertext       = "0fb0cee2",
    .p_key              = "39747da225bdc0c53c3463fd686dbe19d14157535171f91d",
    .p_iv               = "77d3a5ad8bbdb169f8d29e5f21798651"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_decrypt_5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 192 Decrypt message_len=5",
    .p_plaintext        = "105a64c872",
    .p_ciphertext       = "09d914cf0b",
    .p_key              = "4cd13179dfa16d01c6a8633dfc8783e723e72114c9b0d50a",
    .p_iv               = "6657c46c99d642474c330d8016b71dbe"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_decrypt_6) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 192 Decrypt message_len=6",
    .p_plaintext        = "98f332d37b78",
    .p_ciphertext       = "288c752d9fac",
    .p_key              = "5dcc9b8d8a456e9917cd8d54d7f7100b34964b4ed2d398a0",
    .p_iv               = "4fa295a8987f1b010ce4e011fbf94156"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_decrypt_7) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 192 Decrypt message_len=7",
    .p_plaintext        = "56bb4c3cae53b3",
    .p_ciphertext       = "c9981a34b7aa89",
    .p_key              = "c8baf0204ef80b8e0125efe43a0bccdfd0f356b62e6c75fe",
    .p_iv               = "e9144bf2cbc5720a1b4cb6f37d11edff"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_decrypt_8) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 192 Decrypt message_len=8",
    .p_plaintext        = "b567411bc61b0a76",
    .p_ciphertext       = "d8fcf83a88510a0d",
    .p_key              = "64e40763f38a63ae378c32052b0ae3aa538bb868a04ac985",
    .p_iv               = "aacf65089e4b285438451ffdcd0f6389"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_decrypt_9) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 192 Decrypt message_len=9",
    .p_plaintext        = "89358df65c3ef14d26",
    .p_ciphertext       = "9c195b1944c4af5bfb",
    .p_key              = "7bfdca9605f17253f203efffc92da96fde023007d22cdad0",
    .p_iv               = "45c09e44036070f8a7737a5176b8cf26"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 192 Decrypt - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_192_decrypt_10) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 192 Decrypt message_len=10",
    .p_plaintext        = "9895932402393dc33a60",
    .p_ciphertext       = "729c0b6deb75fa6eb5e8",
    .p_key              = "baf08b76317a65c5f07ae6f57eb0e65488659324d29709e3",
    .p_iv               = "0a02846b62abb693ef31d754842eed29"
};


// AES CFB8 - NIST CAVS 11.1 Monte Carlo 192 Encrypt
NRF_SECTION_ITEM_REGISTER(test_vector_aes_monte_carlo_data, test_vector_aes_t test_vector_aes_cfb_192_encrypt_monte_carlo) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 Encrypt Monte Carlo 192",
    .p_plaintext        = "92",
    .p_ciphertext       = "96",
    .p_key              = "dc66d5ccce06d47fee3fa2eb65e2dc0bd8536df29ae85c54",
    .p_iv               = "c3caee0b8e23f400cd472daefc4ba204"
};

// AES CFB8 - NIST CAVS 11.1 Monte Carlo 192 Decrypt
NRF_SECTION_ITEM_REGISTER(test_vector_aes_monte_carlo_data, test_vector_aes_t test_vector_aes_cfb_192_decrypt_monte_carlo) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_192_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 Decrypt Monte Carlo 192",
    .p_plaintext        = "4b",
    .p_ciphertext       = "33",
    .p_key              = "d86b0dc7a9016a8ff57840a0443e1108c0ebe33f52d58b5a",
    .p_iv               = "d5fc3d0cdce1e64da43049776e32593e"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CFB_192)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CFB_256)

// AES CFB8 - Functional test using test vector NIST CAVS 11.1 CFB8 KeySbox 256 - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_func_data, test_vector_aes_t test_vector_aes_cfb_256_functional) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 256 Functional",
    .p_plaintext        = "00",
    .p_ciphertext       = "46",
    .p_key              = "c47b0294dbbbee0fec4757f22ffeee3587ca4730c3d33b691df38bab076bc558",
    .p_iv               = "00000000000000000000000000000000"
};


// AES CFB8 - NIST CAVS 11.1 GFSBox Tests 256 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_encrypt_gfsb0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 256 Encrypt GFSBox 0",
    .p_plaintext        = "00",
    .p_ciphertext       = "5c",
    .p_key              = "0000000000000000000000000000000000000000000000000000000000000000",
    .p_iv               = "014730f80ac625fe84f026c60bfd547d"
};

// AES CFB8 - NIST CAVS 11.1 GFSBox Tests 256 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_decrypt_gfsb0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 256 Decrypt GFSBox 0",
    .p_plaintext        = "00",
    .p_ciphertext       = "5c",
    .p_key              = "0000000000000000000000000000000000000000000000000000000000000000",
    .p_iv               = "014730f80ac625fe84f026c60bfd547d"
};

// AES CFB8 - NIST CAVS 11.1 VarKey Tests 256 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_encrypt_vk0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 256 Encrypt VarKey 0",
    .p_plaintext        = "00",
    .p_ciphertext       = "e3",
    .p_key              = "8000000000000000000000000000000000000000000000000000000000000000",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CFB8 - NIST CAVS 11.1 VarKey Tests 256 Encrypt - Count 255
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_encrypt_vk255) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 256 Encrypt VarKey 255",
    .p_plaintext        = "00",
    .p_ciphertext       = "4b",
    .p_key              = "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CFB8 - NIST CAVS 11.1 VarKey Tests 256 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_decrypt_vk0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 256 Decrypt VarKey 0",
    .p_plaintext        = "00",
    .p_ciphertext       = "e3",
    .p_key              = "8000000000000000000000000000000000000000000000000000000000000000",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CFB8 - NIST CAVS 11.1 VarKey Tests 256 Decrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_decrypt_vk127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 256 Decrypt VarKey 127",
    .p_plaintext        = "00",
    .p_ciphertext       = "4b",
    .p_key              = "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
    .p_iv               = "00000000000000000000000000000000"
};

// AES CFB8 - NIST CAVS 11.1 VarTxt Tests 256 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_encrypt_vt0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 256 Encrypt VarTxt 0",
    .p_plaintext        = "00",
    .p_ciphertext       = "dd",
    .p_key              = "0000000000000000000000000000000000000000000000000000000000000000",
    .p_iv               = "80000000000000000000000000000000"
};

// AES CFB8 - NIST CAVS 11.1 VarTxt Tests 256 Encrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_encrypt_vt127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 256 Encrypt VarTxt 127",
    .p_plaintext        = "00",
    .p_ciphertext       = "ac",
    .p_key              = "0000000000000000000000000000000000000000000000000000000000000000",
    .p_iv               = "ffffffffffffffffffffffffffffffff"
};

// AES CFB8 - NIST CAVS 11.1 VarTxt Tests 256 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_decrypt_vt0) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 256 Decrypt VarTxt 0",
    .p_plaintext        = "00",
    .p_ciphertext       = "dd",
    .p_key              = "0000000000000000000000000000000000000000000000000000000000000000",
    .p_iv               = "80000000000000000000000000000000"
};

// AES CFB8 - NIST CAVS 11.1 VarTxt Tests 256 Decrypt - Count 127
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_decrypt_vt127) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 256 Decrypt VarTxt 127",
    .p_plaintext        = "00",
    .p_ciphertext       = "ac",
    .p_key              = "0000000000000000000000000000000000000000000000000000000000000000",
    .p_iv               = "ffffffffffffffffffffffffffffffff"
};


// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_encrypt_1) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 256 Encrypt message_len=1",
    .p_plaintext        = "f9",
    .p_ciphertext       = "28",
    .p_key              = "34e8091cee09f1bd3ebf1e8f05f51bfbd4899ef2ae006a3a0f7875052cdd46c8",
    .p_iv               = "43eb4dcc4b04a80216a20e4a09a7abb5"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_encrypt_2) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 256 Encrypt message_len=2",
    .p_plaintext        = "2914",
    .p_ciphertext       = "69a6",
    .p_key              = "e04e43173113109e1343393842fe6caef3f8a2e506d7f55f83dcb10444c6ad23",
    .p_iv               = "a38b88a293b077fb5546636aad90d663"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_encrypt_3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 256 Encrypt message_len=3",
    .p_plaintext        = "b974fa",
    .p_ciphertext       = "1cff95",
    .p_key              = "064874092f7a13cc4462247ad423d0e96edf42e8b67a5a23b7a0a6477b098e66",
    .p_iv               = "338c552ff1eca14408e05d8cf9f3b31b"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_encrypt_4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 256 Encrypt message_len=4",
    .p_plaintext        = "db774912",
    .p_ciphertext       = "f04d9d4f",
    .p_key              = "56794adb0ef04aeddeabd650de736531d408837954b919002c33edfdff976cc2",
    .p_iv               = "71b5526facea4236d33f1f4107e4b04f"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_encrypt_5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 256 Encrypt message_len=5",
    .p_plaintext        = "ad1d5311ea",
    .p_ciphertext       = "19cc97a662",
    .p_key              = "dddd7f234e7d0e6ec64560b96430986a856f2ee9805443a7946e31601ef6679d",
    .p_iv               = "e20f39db0025eb24491bd06012887108"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_encrypt_6) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 256 Encrypt message_len=6",
    .p_plaintext        = "02fe724fbc5d",
    .p_ciphertext       = "b0eca63405f4",
    .p_key              = "ec73a760272c83f91771b3ab7b188715c6d6afb9c554feae83856e966a3863d0",
    .p_iv               = "ae7bfa38fd25778fcf66ce8157f6e42e"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_encrypt_7) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 256 Encrypt message_len=7",
    .p_plaintext        = "b4777e6bcd9a05",
    .p_ciphertext       = "8c017397ad5bab",
    .p_key              = "a66874ca0b70fb98b37c033ec96413f339adae02acade015b9f016b459db3309",
    .p_iv               = "6ed480d9e4ed031cf66bb1e07f8d5514"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_encrypt_8) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 256 Encrypt message_len=8",
    .p_plaintext        = "1d70a5a82badf5ea",
    .p_ciphertext       = "1e22bebebeacd81d",
    .p_key              = "a3dbbb775ada92b0b8ed1632444e21c1c86ff3eba8f628307306e766b8c15b5c",
    .p_iv               = "4ec56a8e541f5cfe7b8ab947bfa4fd08"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_encrypt_9) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 256 Encrypt message_len=9",
    .p_plaintext        = "a36ca5ea382a322eef",
    .p_ciphertext       = "3105016567d3174aed",
    .p_key              = "64135e67c0ca1acef3360d930afcd726c5b04861a69c1b6a48bde1daf20f3b1f",
    .p_iv               = "5377a154d5f948189f9aa57b466c16b2"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Encrypt - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_encrypt_10) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 256 Encrypt message_len=10",
    .p_plaintext        = "b0fe25ac8d3d28a2f471",
    .p_ciphertext       = "638c6823e7256fb5626e",
    .p_key              = "ebbb4566b5e182e0f072466b0b311df38f9175bc0213a5530bce2ec4d74f400d",
    .p_iv               = "0956a48e01002c9e16376d6e308dbad1"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 0
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_decrypt_1) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 256 Decrypt message_len=1",
    .p_plaintext        = "98",
    .p_ciphertext       = "6f",
    .p_key              = "1687831580cb764321a9d674dbd0a9640f668b0f58ef01b87a710b3095d5f855",
    .p_iv               = "6cd5bec6d6e1fd23afc543b8f80d3f89"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 1
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_decrypt_2) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 256 Decrypt message_len=2",
    .p_plaintext        = "3572",
    .p_ciphertext       = "17d4",
    .p_key              = "b6b504e8b7065373ea31cd549e52eda7cb96fd1db14eddacbc420085ab48b747",
    .p_iv               = "870ecd45b1241803ddaf8bad15a025d7"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 2
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_decrypt_3) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 256 Decrypt message_len=3",
    .p_plaintext        = "f9411a",
    .p_ciphertext       = "a9841e",
    .p_key              = "6ad3105e15fb5b742bf4fe1eb8e98c6c1ffea653107c84f6b42ed1232a0bbc21",
    .p_iv               = "17534c89c4eae5dea6ea353dde7b1623"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 3
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_decrypt_4) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 256 Decrypt message_len=4",
    .p_plaintext        = "47887872",
    .p_ciphertext       = "84b35e25",
    .p_key              = "758f3fa8b2b289f19fd59e7316be40b904eff7f565caac4570f972360e0da787",
    .p_iv               = "b21335ae980898fa92c4b3069e532973"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 4
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_decrypt_5) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 256 Decrypt message_len=5",
    .p_plaintext        = "ed414b5689",
    .p_ciphertext       = "d1e96bf1e8",
    .p_key              = "802e854eb799500975d960a67885820d195e02ab23d51f15e5cdbcee86a1580c",
    .p_iv               = "94478c4e44e2fa8d2e6bc43d384597e6"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 5
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_decrypt_6) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 256 Decrypt message_len=6",
    .p_plaintext        = "8207eee2a7ab",
    .p_ciphertext       = "74410ccd12da",
    .p_key              = "3a0c03ca9d1e5d49bb37f9041f88d159c3f1d5ce26c798f59ed54a93f0a0e600",
    .p_iv               = "9aae38ba832e4b093b50444074517d20"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 6
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_decrypt_7) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 256 Decrypt message_len=7",
    .p_plaintext        = "aff3aa4c24e353",
    .p_ciphertext       = "ff039e89877b44",
    .p_key              = "ee05462128fea75e919f6f436cb198f222847d698a283f5767df682d33d3ce77",
    .p_iv               = "d2ad55e41887075184635112a22fc093"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 7
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_decrypt_8) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 256 Decrypt message_len=8",
    .p_plaintext        = "b7370050288bf600",
    .p_ciphertext       = "e176ba99ea602fd9",
    .p_key              = "08abbdcc3eb9c1717db1faa38dcd0893afd5e16e2596747af58f8d61ebedf9cd",
    .p_iv               = "b925c8dc9a9b55a4372ea6d37d21c1eb"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 8
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_decrypt_9) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 256 Decrypt message_len=9",
    .p_plaintext        = "e44995bbb0fff40fee",
    .p_ciphertext       = "c55a9a917a809a784b",
    .p_key              = "56d404a893fb3b3f594aab18939230b096646a37a781629fbd9270f3891a5cea",
    .p_iv               = "e5906b36f2d97e6f2db19b6c7a3ce319"
};

// AES CFB8 - NIST CAVS 11.1 Multiblock Message Tests 256 Decrypt - Count 9
NRF_SECTION_ITEM_REGISTER(test_vector_aes_data, test_vector_aes_t test_vector_aes_cfb_256_decrypt_10) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 256 Decrypt message_len=10",
    .p_plaintext        = "547bfd642cf6e12ed942",
    .p_ciphertext       = "2bfc3f0209307140101a",
    .p_key              = "ec13062551e4d7291e320f565b749eea1809b663b26f2c4d53b52058b833e0ad",
    .p_iv               = "fbfa5a528e20863012790c2abafb5a0c"
};

// AES CFB8 - NIST CAVS 11.1 Monte Carlo 256 Encrypt
NRF_SECTION_ITEM_REGISTER(test_vector_aes_monte_carlo_data, test_vector_aes_t test_vector_aes_cfb_256_encrypt_monte_carlo) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_ENCRYPT,
    .p_test_vector_name = "CFB8 Encrypt Monte Carlo 256",
    .p_plaintext        = "b9",
    .p_ciphertext       = "46",
    .p_key              = "7c046546c5542ff9c06823cc78efc28e8fd1e8ffd56ffc36192c6a40402c530a",
    .p_iv               = "ea42a2fb73b36b8951c187a10205fcc4"
};

// AES CFB8 - NIST CAVS 11.1 Monte Carlo 256 Decrypt
NRF_SECTION_ITEM_REGISTER(test_vector_aes_monte_carlo_data, test_vector_aes_t test_vector_aes_cfb_256_decrypt_monte_carlo) =
{
    .p_aes_info         = &g_nrf_crypto_aes_cfb_256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .direction          = NRF_CRYPTO_DECRYPT,
    .p_test_vector_name = "CFB8 Decrypt Monte Carlo 256",
    .p_plaintext        = "a4",
    .p_ciphertext       = "aa",
    .p_key              = "39b7297f2d0504dc87e1f4c5e2ca8a567c580b8de723c55102e0a3b328cd0073",
    .p_iv               = "ffe3c1b78e92e01779f29a9349f76bc8"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CFB_256)

/*lint -restore */

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_AES_CFB)
