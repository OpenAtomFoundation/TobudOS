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

/**@brief ECDSA test vectors can be found on NIST web pages.
 *
 * https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Component-Testing
 */

#if NRF_CRYPTO_ECC_ENABLED

/*lint -save -e91 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP160R1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP160R1)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP160R2)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP160R2)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP192R1)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA256)
// ECDSA verify - NIST CAVS 11.0 P-192, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp192r1_sha256_1) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp192r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp192r1 valid sha256 1",
    .p_input                   = "f1d42d1c663fa4d88325458d31fb08b35e8fac7cebc04b224db57439680c9be4",
    .p_qx                      = "b870597b4b8dc8fc07ed59b6f079e87936d56d0326c17249",
    .p_qy                      = "e54c404920cd530f0680d8aa2a4fb70b5f8605e6ebbf2751",
    .p_r                       = "b53dc1abd4f65d5e0506fa146bee65ecb6cd5353830b67ea",
    .p_s                       = "aa44232f2fa6613f85fda824ded69e4137cdf5688c6b3ba9"
};

// ECDSA verify - NIST CAVS 11.0 P-192, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp192r1_sha256_2) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp192r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp192r1 valid sha256 2",
    .p_input                   = "cab19f4afca519c6c8a2a09ba7e631ff56cc898694b64123b62e3c94b9fb4696",
    .p_qx                      = "795bbf28b86af380c2b080e622f92f81de6d2af41a39bc39",
    .p_qy                      = "3d3bcfcbe704426e95d0edbf40eae25a259af239b00158c9",
    .p_r                       = "5a3fd911aac408cce41e0eaf42761cce155c5a6efe03df11",
    .p_s                       = "605ffbb146bf787888d9c3e45f79d0bc6959dcfacfaea437"
};

// ECDSA verify - NIST CAVS 11.0 P-192, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp192r1_sha256_3) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp192r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp192r1 valid sha256 3",
    .p_input                   = "786f3a4c00a899bfcd2a79e59ad387562c49e01370ee2fc9feab605a3552e37d",
    .p_qx                      = "8109731205bd9e363c0521cddf94af58129af3f38d276f2a",
    .p_qy                      = "9fcf7695165bafb39c2d53b61c4ccfed3891abc6db1fc22c",
    .p_r                       = "cac3fe60f567724f7afb825aeda68c3b345b44ef3879dc70",
    .p_s                       = "4544b7d4457b61b66cabfd6174f2c5a594b2c0f300b0e8ea"
};

// ECDSA verify - NIST CAVS 11.0 P-192, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp192r1_sha256_inv_r) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp192r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp192r1 R changed sha256",
    .p_input                   = "e9097723aedbac5d15745568fdca435aea353bd0c002b12f9253ee4424377c61",
    .p_qx                      = "390ca1526943a85332c96aab75a10ec30fda9fb197ded9aa",
    .p_qy                      = "1260266139849659339e3cbb83a9ee3a63f45ff7b96266bb",
    .p_r                       = "fdbe0c0864213e258abbedc29dd359a4c58e060c351b15fd",
    .p_s                       = "3dc705de56f9c1d6e7774dbb70dd48d34c891bcffbcc30df"
};

// ECDSA verify - NIST CAVS 11.0 P-192, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp192r1_sha256_inv_s) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp192r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp192r1 S changed sha256",
    .p_input                   = "4e2ddd608a1de8f2f929947c5bb722345724f909e4e18effd571299c480217d1",
    .p_qx                      = "8698b9f47ce024b0a139d1aee610c1d549aa3512a990b211",
    .p_qy                      = "998be4c51c70d4fea9b2b2e274dad47175f9d68c8a1c28a4",
    .p_r                       = "13d7c9f471863cd4ee3ce5458f1fa436f12f50f965ee40bc",
    .p_s                       = "8cce6c1787a66f30679b92de51cd3b04a0c1900bc1008133"
};

// ECDSA verify - NIST CAVS 11.0 P-192, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp192r1_sha256_inv_q) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp192r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp192r1 Q changed sha256",
    .p_input                   = "f7d05383c1dd96d0db53de2c33c68eae1fa8328639d7dbe73822ff8a4a480a4c",
    .p_qx                      = "001e75261da06eba07f3ee911276b4b6e25ee37abf54fa41",
    .p_qy                      = "5da176ff885ca3345508bc4c917128b2c87f6ab182113505",
    .p_r                       = "e5e6bb6133dc7dc8926d3705cbced15d5820a29e7ac2de8d",
    .p_s                       = "46a6e3818a7286818250cea5a023be5b401b1026ea316843"
};

// ECDSA verify - NIST CAVS 11.0 P-192, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp192r1_sha256_inv_hash) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp192r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp192r1 hash changed sha256",
    .p_input                   = "66f284b189756241e94f432b945f176b561c837e75c9a71aafedddcfcd0874c6",
    .p_qx                      = "711bdec50f8409b10fd2cfeb30e9e69c9aa27e3a535fb39a",
    .p_qy                      = "3e90f90551e7eb7eda11fb016b0a386ebf48b4b2f3e8f272",
    .p_r                       = "5e61c8445f402c0c16ddfcd050d2a9fbfe495537c3d76ffb",
    .p_s                       = "09d0219f34b46c528d81d3d9c838253f942528e8f53329a2"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA256)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA512)

// ECDSA verify - NIST CAVS 11.0 P-192, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp192r1_sha512_1) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp192r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp192r1 valid sha512 1",
    .p_input                   = "86df477d86343fa3a5432d45122c6f025e9b35a7b14ff3db6ffdbbcc17b8cb125f59adb506c2861efca72c3dea5d39550b8bb6d65ade45f1c4f963c6ad357099",
    .p_qx                      = "dea419cbbb2c7be3b59d6eb4db9ca48efb4835eccf8d0a48",
    .p_qy                      = "88dde250494bb6b910e979c5fb3a2fc44d41ae3e761fe85d",
    .p_r                       = "302648f3a89aec847742ec72209ac02d6232fe2363f72fac",
    .p_s                       = "a1895dd201f022c17cd69dab9c5438d2b25f9368aa8b9cc3"
};

// ECDSA verify - NIST CAVS 11.0 P-192, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp192r1_sha512_2) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp192r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp192r1 valid sha512 2",
    .p_input                   = "6e2286e3f4d125a0cc9d101b879c2207db690207eae578cc94bf2e3593a835fb5e07dcce35dc964dbaf641829bead62be303ba11ce819200920bf54c91f57cd7",
    .p_qx                      = "2d3cffc6aac703d224029d243036cae359af89fb24801481",
    .p_qy                      = "00346a43ccc3cdcc37cb9b2757d5f88fede01a5ac160f253",
    .p_r                       = "3a844183d6a2a59255ef9105a6b8dbbd0662c227ee04be0e",
    .p_s                       = "4f322c112b5cbdc7c23138ac51fb975cff8277676105e5f1"
};

// ECDSA verify - NIST CAVS 11.0 P-192, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp192r1_sha512_3) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp192r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp192r1 valid sha512 3",
    .p_input                   = "2b268f3f54998d777824b9b148d4c0c03bdde23053798adef5378d9c7c0a9e6e23fb0d5bd8848dbd884de44b15b90e035d3e758ca8a87ca6a9722668d2478bb6",
    .p_qx                      = "fce011d181c4d3c75043ab6936e2e9a7d6964ad47851e290",
    .p_qy                      = "71b52d04ca114434be936d9518f05ef9791997e240da0740",
    .p_r                       = "ee9a4782853080a2cb2415914dee9bb4245def81ffa83491",
    .p_s                       = "a701084db2870488d00a5886f872b81c53552ef31f09addc"
};

// ECDSA verify - NIST CAVS 11.0 P-192, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp192r1_sha512_inv_r) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp192r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp192r1 R changed sha512",
    .p_input                   = "50337a209852b14ae223f311667c2e05f5cc076d7afca97cce03bf799a836b9991956573f435af2d2ff4122d53df2831b48c1d738edf5bb288d7f43a535b7f92",
    .p_qx                      = "8a3939d2b4b4a5b251d383fdb6450d818f6319bd91ee1b70",
    .p_qy                      = "88c934f7625438d118ec95085851d56e4504e553c6701be3",
    .p_r                       = "4677fb00515485c96be8cd8556984868697edcc1170b2605",
    .p_s                       = "7a5c09c4c65085b9a25d4a2578aac167fe31404cf8dc5858"
};

// ECDSA verify - NIST CAVS 11.0 P-192, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp192r1_sha512_inv_s) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp192r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp192r1 S changed sha512",
    .p_input                   = "d70fae6073e92dc72945ddb8dc45853473dac4855f70516dbb875c970109a454e43bb9b99c2bd27f6bc9560862d70830549b07365f302d5a46ca696352006c9b",
    .p_qx                      = "28f4063b8f808a9642f2b04f794d61739b883897495cf057",
    .p_qy                      = "9be6fde04d687ea2778f845f881b059488f916c3218f42c9",
    .p_r                       = "15f3e034340d6763ecd73515ddf0b92e383b539c97f78b37",
    .p_s                       = "fa77e58f64141876845f438ac6da7bd4bf8dd3097a1f27e8"
};

// ECDSA verify - NIST CAVS 11.0 P-192, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp192r1_sha512_inv_q) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp192r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp192r1 Q changed sha512",
    .p_input                   = "53156f080461d45f7b6e3babcc2195c2db62b54ff95b0791637f195faaf5dd70d394b6ed0e2a2d9ea728846327d9ca1c49add43857e565344ef132a004c8e57a",
    .p_qx                      = "3e64f302334d85913234703619381bb0ee8d68e40334dd0e",
    .p_qy                      = "38e6ef6fe5c364f17ff3947e294613bd07fd25e46be9ee29",
    .p_r                       = "eea898e0fd6b8dea2a316cdd03a2f1a9adf0dd5beb004372",
    .p_s                       = "0bd969fcc0f08281367b0b94dac13aa4737a0861e046d93f"
};

// ECDSA verify - NIST CAVS 11.0 P-192, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp192r1_sha512_inv_hash) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp192r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp192r1 hash changed sha512",
    .p_input                   = "5fafc1d67fdb7c64373c35befc61bd886a3c433493dfdbe67c0fcf4ef2f76e1c808b11f68621743b4f8c3f71787850fd588c33fdf5bea7b8b9700d957715a8e1",
    .p_qx                      = "d008c455eadcdb98234cb6ad2d9876b82c5b0c8867ba86d9",
    .p_qy                      = "32c009ec81f8eeb99859af65e246d089d3a0c0cdaecef798",
    .p_r                       = "79f619e30cd68028263f107beab00ce2375c570a4be16487",
    .p_s                       = "82257c9eaf1e7ace39e2804e535d7df86158cf1c40425f50"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA512)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP192R1)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP224R1)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA256)

// ECDSA verify - NIST CAVS 11.0 P-224, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp224r1_sha256_1) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp224r1 valid sha256 1",
    .p_input                   = "831979405db4eb9dadf01249fa15f68d4846e0ece70a320d3022f75f3dc281cc",
    .p_qx                      = "34c5ff3de565b85bfdd9f0a8b3fb0d46f924c57b276bcc830a1ed580",
    .p_qy                      = "609d22200ef38b410da77f7a8ff2f58448188042978fd9ae1b2b4477",
    .p_r                       = "f0138024fe0516738f3bd0e0fec10defaca8c3b89c161a77489cf2b7",
    .p_s                       = "4ae0934266d9e3d64c2a12f546b132ba0f33ef50abc90e7ef5974805"
};

// ECDSA verify - NIST CAVS 11.0 P-224, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp224r1_sha256_2) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp224r1 valid sha256 2",
    .p_input                   = "25ef291dd6a2047ea193f8aedd3f4692f2c135cb519922a17cba3e969423b030",
    .p_qx                      = "a580f9a0cd15abff8e1e712f16b0fd4142d0d773af3c657abc06c2a6",
    .p_qy                      = "22c6286340dc072e64274209eda60503047700571caee64b4a2306c2",
    .p_r                       = "c6fae06274dc052e482102520b49d4ccc4cb7eb8a3ea41bd3680ddad",
    .p_s                       = "50d66b75a2bbd0468be1f9e61bfda85b6329505b0134d60846cbe4b7"
};

// ECDSA verify - NIST CAVS 11.0 P-224, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp224r1_sha256_3) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp224r1 valid sha256 3",
    .p_input                   = "e8f714f901cd2390c66f9fa9cb81ecc5f17a82bd934cf19c5ce2bcbd3a985de9",
    .p_qx                      = "5d67c1fca848ba7f3d9de5b1894d3993ac4ebe68cdb0b49553b3b0e9",
    .p_qy                      = "07c219a8323273c81f5694306d0dd1d133a49efce5003bc90f05578f",
    .p_r                       = "767cb6b2efa7a40739830659b0cc24fe3de771d00104b3dcc0f640bc",
    .p_s                       = "f2e7268bc011d79d33f3551d2edd3c95f324955479b8e29e8aba629b"
};

// ECDSA verify - NIST CAVS 11.0 P-224, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp224r1_sha256_inv_r) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp224r1 R changed sha256",
    .p_input                   = "21a9f65d4cdcc287e0bf330e4f76f1168d16d0cea796dd6c0e13cd2837decaf1",
    .p_qx                      = "13e84ec2eb993818d7d78330855ee2fbe8ddb548a5e4198e2087b3b2",
    .p_qy                      = "c95dff249e10c506fb547a92ade53c61ddbb667c760e4127a1a7f806",
    .p_r                       = "ed26f00ed696e114305c546ed04db5fc35efa43059c0d8bbcd418d0c",
    .p_s                       = "6e16efbe9501e3055d74966a49232cd76b5d1241468788b4cc7378b4"
};

// ECDSA verify - NIST CAVS 11.0 P-224, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp224r1_sha256_inv_s) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp224r1 S changed sha256",
    .p_input                   = "489cd3c10d94f7f284bc4ea472634a5d7f0d280d5abbf139c110ef80c8bf463f",
    .p_qx                      = "465afb14f4bf85022ac1f635f46c0b2f6548bace9352d32f74eab012",
    .p_qy                      = "036371a3246dbf1069d2d268ca431553d1f2bf0181225145881b7be0",
    .p_r                       = "9bcd57a2fec2518903e4b13dc0a7b84bafed5c4908546e94ffae87ed",
    .p_s                       = "a337e06582f6b3973df38b93a0fb2a63f7774b62db50dba557e5cfcc"
};

// ECDSA verify - NIST CAVS 11.0 P-224, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp224r1_sha256_inv_q) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp224r1 Q changed sha256",
    .p_input                   = "d8454640ad1f4632cc667823418ae56c62028825d727adfc84afdb0842b0c9a4",
    .p_qx                      = "8856fb8b81a4eacd971a954560018f33cbb71cc1fc243d03f63cabcb",
    .p_qy                      = "28afa26baf31b4d89de1dadd2289006f836f23a11383817ec7e4e799",
    .p_r                       = "efccef331805e71bbf876cbbc2342a6bc4508aea7c691029c8396aef",
    .p_s                       = "bed544d09e28dbf01a30b2cfb61b98ad6201a9818f22b4f543f3e7f5"
};

// ECDSA verify - NIST CAVS 11.0 P-224, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp224r1_sha256_inv_hash) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp224r1 hash changed sha256",
    .p_input                   = "d62496d0ee0453e7f13f8d9c57adcd33442a5365f626381ed665f95f528aa198",
    .p_qx                      = "16c23c93699cf665a5da8b2d4baa72c36158d3433b1b945e47204b0d",
    .p_qy                      = "12023703e1b59ec9054ff22d15567b9f74058b47cc13f2ca08ab77c1",
    .p_r                       = "ada849b673a1bd2949a8b4d8fdfc239ec53524a356d37da3c9d17ae2",
    .p_s                       = "698de3a3d8697c2e8e5b2c85fceb8796750c5b44154f01ce86d99e24"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA256)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA512)

// ECDSA verify - NIST CAVS 11.0 P-224, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp224r1_sha512_1) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp224r1 valid sha512 1",
    .p_input                   = "eaa81b1236121db12f2036611eb6d3e5386f5733125fb26844cb4188109aae402525fc63bcd73494c36c170d1934b108739d79b0c4f184bd85ffce93b1beecde",
    .p_qx                      = "c6a65011926eb64e02bf472d5ba37841d49cfb7f17a20fb9f59355de",
    .p_qy                      = "386ccb33d944fd7be6b8531863d2b6200cd602d300d7e7681537e53f",
    .p_r                       = "9e7c637a699dd52512faea847079f0ad41b20cd7a5461c36d01e857e",
    .p_s                       = "dec6e9ef361de3f6ec7d87de3129eaac5fd0b43b5f7f58ce46c29173"
};

// ECDSA verify - NIST CAVS 11.0 P-224, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp224r1_sha512_2) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp224r1 valid sha512 2",
    .p_input                   = "680aaa97b939a5031737a40b9e34e5fb8047b8a6e495c7c90653458072cb423fb098e6ecef15c9d62d4443337f61cde237c66c1683753e65f8bced5298375bf5",
    .p_qx                      = "f10652c3c2c30a765564f5e393c6c202d436c81fc7d71b88857bd458",
    .p_qy                      = "42979ba5e6c8cd044e262c73e6aa918d8c3e0e08e4bf98ec2d5c6f57",
    .p_r                       = "072e0b130267d8e124dda2d0604f4c575ef4007628fa61f66bcd8f07",
    .p_s                       = "6276475fccda3bee2af7816c7b3ec222e408cec36d0409e672af23b5"
};

// ECDSA verify - NIST CAVS 11.0 P-224, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp224r1_sha512_3) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp224r1 valid sha512 3",
    .p_input                   = "d4e0bcf58e2083383ef94b28cfb2710a4e898b2c2a0cf30bebb99c3f2dc93403dd165e1a0c3773ed05adedf967fc92f0a158e5086965a29dab3a4aaa5e9f9911",
    .p_qx                      = "dc85f6701bc95a60ea52f1c476dcb211e5e3eeba5b35dc3a3786113f",
    .p_qy                      = "a52b364fb7c4b4c9155e4ee2d8841386b8a96cbeed1e5c9957214a50",
    .p_r                       = "bdc7b4d3266e54a19f030f3055f83460ccf30fe8f5368be013076081",
    .p_s                       = "82a0e3a21a8d5e9cfcc5a673385cc2d4159498b6c1ef5791b2b02f9f"
};

// ECDSA verify - NIST CAVS 11.0 P-224, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp224r1_sha512_inv_r) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp224r1 R changed sha512",
    .p_input                   = "f59946dd73887a1fb945ae3e5b44b574512e36dd7d35925b0bdc3a3f69a88e50dc6ebaaac86092679961a99d7ba5724363c5886e7c2cd566433c5a9d59e2515d",
    .p_qx                      = "6c6a7deef8b3dee2eaa98d8ec877dddb460365968e63ffe5c249a421",
    .p_qy                      = "1ad1715797b5e47c4be24d7ecb8141f1772344a2f643cc66fbcf3f9d",
    .p_r                       = "f778faa2dc2cee7de2af0bbc4339239dc240907e59ac034464ce986b",
    .p_s                       = "2ac929588a8dc8785808c9d4366bd7b1a81e0fc5be0c63e62344c251"
};

// ECDSA verify - NIST CAVS 11.0 P-224, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp224r1_sha512_inv_s) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp224r1 S changed sha512",
    .p_input                   = "330f78db9f217f95d99cbcb5414a32523025b85c87ed5aaa3a5e6b006c53cecbbd446166bb9b98e944626332c3266e82356e72110bdbf7df2431b8e7faf59e45",
    .p_qx                      = "ea7efb9a7e7bf693dc0249504ead70dd422ec86806cd649ed57a7ffe",
    .p_qy                      = "33dfb17b1cb244279990019286ab477a2aff2b55c08959deed0b1a28",
    .p_r                       = "deab574de1f971419b7ba9e169866c3a6233fc627c13dc6a8cb88bdc",
    .p_s                       = "8d4c542bb0046da1b3912652a2c84542668cd89920808785d04e4352"
};

// ECDSA verify - NIST CAVS 11.0 P-224, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp224r1_sha512_inv_q) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp224r1 Q changed sha512",
    .p_input                   = "3cba2980faf5579c7ddbeb31c3318a162a882129a1efe1fc9acc3219a617c460701f22fd7a5c46b017bd9ec9c40587127319ea14b148d2be4df9875d5bef6e4e",
    .p_qx                      = "72013dd6f5b1eb709b3b7da234987f9a36c6f0b095620b8c31f02381",
    .p_qy                      = "319d54c719b59d91900b3c20d963ddf1a10d80d6601ac155094b075a",
    .p_r                       = "eb38db9b757b3cf04020f09188c789bf0258cd0467cf7d67368e8703",
    .p_s                       = "7664e85f01e67881712b24083f89e838c8b818de4d665494e7016833"
};

// ECDSA verify - NIST CAVS 11.0 P-224, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp224r1_sha512_inv_hash) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp224r1 hash changed sha512",
    .p_input                   = "bb7394f9b30617419daa71573f5ed9d72732930b6d24141ea2bfb6bcdd67f4fd7a7e6caed8fcac76f50bad0616c291ff875d866b4085f2a0201289a0895a319a",
    .p_qx                      = "89cef29515f1d30fdd283625eb59e682023bdf2eb0497288ff700b27",
    .p_qy                      = "ca0a3f7ee4853ae8454fff6e926c7a85dc7852fb4d01cc3de861c53d",
    .p_r                       = "0ff5dba589454d0ee5bb391a4dc8446b1ffd1cf8d66c9fc0f05a8a89",
    .p_s                       = "987c32258cd12b6b9cdcc5915eea5eb79a30ead442179a126b97fa9e"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA512)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP224R1)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP256R1)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA256)

// ECDSA verify - NIST CAVS 11.0 P-256, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp256r1_sha256_1) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp256r1 valid sha256 1",
    .p_input                   = "d1b8ef21eb4182ee270638061063a3f3c16c114e33937f69fb232cc833965a94",
    .p_qx                      = "e424dc61d4bb3cb7ef4344a7f8957a0c5134e16f7a67c074f82e6e12f49abf3c",
    .p_qy                      = "970eed7aa2bc48651545949de1dddaf0127e5965ac85d1243d6f60e7dfaee927",
    .p_r                       = "bf96b99aa49c705c910be33142017c642ff540c76349b9dab72f981fd9347f4f",
    .p_s                       = "17c55095819089c2e03b9cd415abdf12444e323075d98f31920b9e0f57ec871c"
};

// ECDSA verify - NIST CAVS 11.0 P-256, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp256r1_sha256_2) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp256r1 valid sha256 2",
    .p_input                   = "b9336a8d1f3e8ede001d19f41320bc7672d772a3d2cb0e435fff3c27d6804a2c",
    .p_qx                      = "e0fc6a6f50e1c57475673ee54e3a57f9a49f3328e743bf52f335e3eeaa3d2864",
    .p_qy                      = "7f59d689c91e463607d9194d99faf316e25432870816dde63f5d4b373f12f22a",
    .p_r                       = "1d75830cd36f4c9aa181b2c4221e87f176b7f05b7c87824e82e396c88315c407",
    .p_s                       = "cb2acb01dac96efc53a32d4a0d85d0c2e48955214783ecf50a4f0414a319c05a"
};

// ECDSA verify - NIST CAVS 11.0 P-256, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp256r1_sha256_3) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp256r1 valid sha256 3",
    .p_input                   = "41007876926a20f821d72d9c6f2c9dae6c03954123ea6e6939d7e6e669438891",
    .p_qx                      = "2d98ea01f754d34bbc3003df5050200abf445ec728556d7ed7d5c54c55552b6d",
    .p_qy                      = "9b52672742d637a32add056dfd6d8792f2a33c2e69dafabea09b960bc61e230a",
    .p_r                       = "06108e525f845d0155bf60193222b3219c98e3d49424c2fb2a0987f825c17959",
    .p_s                       = "62b5cdd591e5b507e560167ba8f6f7cda74673eb315680cb89ccbc4eec477dce"
};

// ECDSA verify - NIST CAVS 11.0 P-256, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp256r1_sha256_inv_r) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp256r1 R changed sha256",
    .p_input                   = "5984eab8854d0a9aa5f0c70f96deeb510e5f9ff8c51befcdc3c41bac53577f22",
    .p_qx                      = "5cf02a00d205bdfee2016f7421807fc38ae69e6b7ccd064ee689fc1a94a9f7d2",
    .p_qy                      = "ec530ce3cc5c9d1af463f264d685afe2b4db4b5828d7e61b748930f3ce622a85",
    .p_r                       = "dc23d130c6117fb5751201455e99f36f59aba1a6a21cf2d0e7481a97451d6693",
    .p_s                       = "d6ce7708c18dbf35d4f8aa7240922dc6823f2e7058cbc1484fcad1599db5018c"
};

// ECDSA verify - NIST CAVS 11.0 P-256, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp256r1_sha256_inv_s) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp256r1 S changed sha256",
    .p_input                   = "a82c31412f537135d1c418bd7136fb5fde9426e70c70e7c2fb11f02f30fdeae2",
    .p_qx                      = "87f8f2b218f49845f6f10eec3877136269f5c1a54736dbdf69f89940cad41555",
    .p_qy                      = "e15f369036f49842fac7a86c8a2b0557609776814448b8f5e84aa9f4395205e9",
    .p_r                       = "d19ff48b324915576416097d2544f7cbdf8768b1454ad20e0baac50e211f23b0",
    .p_s                       = "a3e81e59311cdfff2d4784949f7a2cb50ba6c3a91fa54710568e61aca3e847c6"
};

// ECDSA verify - NIST CAVS 11.0 P-256, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp256r1_sha256_inv_q) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp256r1 Q changed sha256",
    .p_input                   = "44b02ad3088076f997220a68ff0b27a58ecfa528b604427097cce5ca956274c5",
    .p_qx                      = "2ddfd145767883ffbb0ac003ab4a44346d08fa2570b3120dcce94562422244cb",
    .p_qy                      = "5f70c7d11ac2b7a435ccfbbae02c3df1ea6b532cc0e9db74f93fffca7c6f9a64",
    .p_r                       = "9913111cff6f20c5bf453a99cd2c2019a4e749a49724a08774d14e4c113edda8",
    .p_s                       = "9467cd4cd21ecb56b0cab0a9a453b43386845459127a952421f5c6382866c5cc"
};

// ECDSA verify - NIST CAVS 11.0 P-256, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp256r1_sha256_inv_hash) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp256r1 hash changed sha256",
    .p_input                   = "d80e9933e86769731ec16ff31e6821531bcf07fcbad9e2ac16ec9e6cb343a870",
    .p_qx                      = "69b7667056e1e11d6caf6e45643f8b21e7a4bebda463c7fdbc13bc98efbd0214",
    .p_qy                      = "d3f9b12eb46c7c6fda0da3fc85bc1fd831557f9abc902a3be3cb3e8be7d1aa2f",
    .p_r                       = "288f7a1cd391842cce21f00e6f15471c04dc182fe4b14d92dc18910879799790",
    .p_s                       = "247b3c4e89a3bcadfea73c7bfd361def43715fa382b8c3edf4ae15d6e55e9979"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA256)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA512)

// ECDSA verify - NIST CAVS 11.0 P-256, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp256r1_sha512_1) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp256r1 valid sha512 1",
    .p_input                   = "0250f93e6932887df519921f9a8dcff110be0768dc351ef73a940a579fae2d20061759e892e289c3e4ba5f7fe17d6ebb15c5931d48db55ebc81549f6637292fe",
    .p_qx                      = "9cb0cf69303dafc761d4e4687b4ecf039e6d34ab964af80810d8d558a4a8d6f7",
    .p_qy                      = "2d51233a1788920a86ee08a1962c79efa317fb7879e297dad2146db995fa1c78",
    .p_r                       = "4b9f91e4285287261a1d1c923cf619cd52c175cfe7f1be60a5258c610348ba3d",
    .p_s                       = "28c45f901d71c41b298638ec0d6a85d7fcb0c33bbfec5a9c810846b639289a84"
};

// ECDSA verify - NIST CAVS 11.0 P-256, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp256r1_sha512_2) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp256r1 valid sha512 2",
    .p_input                   = "9eb2f9fa96a1f3ffcef9600522730e86d26d328ec0c1bf2fbfe55a38754610341fda1b894fdcf10c9bc4f48819010fdcf0d24f27ff539e40c6855cafbd306386",
    .p_qx                      = "f63afe99e1b5fc652782f86b59926af22e6072be93390fe41f541204f9c935d1",
    .p_qy                      = "f6e19ce5935e336183c21becf66596b8f559d2d02ee282aa87a7d6f936f7260c",
    .p_r                       = "cef4831e4515c77ca062282614b54a11b7dc4057e6997685c2fbfa95b392bf72",
    .p_s                       = "f20dc01bf38e1344ba675a22239d9893b3a3e33d9a403329a3d21650e9125b75"
};

// ECDSA verify - NIST CAVS 11.0 P-256, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp256r1_sha512_3) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp256r1 valid sha512 3",
    .p_input                   = "45b082e804443b53a82229cdf13e4c5f8f31fe93170cc8a23f63eef506cb7748388e1a971a2f81e3daa324cf2bb69118f7418f40df66a24f50c34a55e1416c3a",
    .p_qx                      = "2dcbd8790cee552e9f18f2b3149a2252dcd58b99ca7dc9680b92c8c43aa33874",
    .p_qy                      = "5dbc8bb8813c8e019d80e19acdb0792f537980fecde93db621aaf1f6d0e6ee34",
    .p_r                       = "2bdbd8b0d759595662cc10b10236136ef6ce429641f68cf6480f472fcc77bc9f",
    .p_s                       = "7e7df0c8b86f7db06caf1610166f7b9c4c75447f991d5aaf4dea720c25985c8c"
};



// ECDSA verify - NIST CAVS 11.0 P-256, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp256r1_sha512_inv_r) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp256r1 R changed sha512",
    .p_input                   = "4d74631eb67fd1a6fa93ecb6e6112b6699e78c1d4c24ae81d0d5842efe5d93c2fd7a7863f8d45d1b2fafecbe41b7dc19c4b2bc208e014ffdc216e7eda0392a70",
    .p_qx                      = "bc7c8e09bd093468f706740a4130c544374fdc924a535ef02e9d3be6c6d3bbfa",
    .p_qy                      = "af3f813ae6646f5b6dbfb0f261fd42537705c800bb1647386343428a9f2e10fc",
    .p_r                       = "6bd7ce95af25abfbf14aef4b17392f1da877ab562eca38d785fe39682e9c9324",
    .p_s                       = "6688bea20c87bab34d420642da9bdd4c69456bdec50835887367bb4fb7cd8650"
};

// ECDSA verify - NIST CAVS 11.0 P-256, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp256r1_sha512_inv_s) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp256r1 S changed sha512",
    .p_input                   = "575c64df58c8dc517ce65b388fa3ed69470163afecbabc3fa94b497ff7f3fe36ff12fabe2b84cebbf667744195091e4e2335a71d36414e0af0d0260fc8e8ea44",
    .p_qx                      = "633c2ee5630b62c9ce839efd4d485a6d35e8b9430d264ffe501d28dbace79123",
    .p_qy                      = "4b668a1a6d1a25b089f75c2bd8d8c6a9a14fe7b729f45a82565da2e866e2c490",
    .p_r                       = "bf2111c93ec055a7eda90c106fce494fd866045634fd2aa28d6e018f9106994e",
    .p_s                       = "86b0341208a0aa55edecfd272f49cb34408ce54b7febc1d0a1c2ce77ab6988f8"
};

// ECDSA verify - NIST CAVS 11.0 P-256, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp256r1_sha512_inv_q) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp256r1 Q changed sha512",
    .p_input                   = "f91b09107d10904d3968ec29f85e456ac4e828f32e8da3db6a13f5566bfa625e2ad03f8dad5425a073c0d61d25de63dcafa9f4fcd206f29e9cb6b0fecd74aa57",
    .p_qx                      = "e31096c2d512fbf84f81e9bdb16f33121702897605b43a3db546f8fb695b5f6f",
    .p_qy                      = "6fbec6a04a8c59d61c900a851d8bf8522187d3ec2637b10fa8f377689e086bba",
    .p_r                       = "1b244c21c08c0c0a10477fb7a21382d405b95c755088292859ca0e71bab68361",
    .p_s                       = "852f4cbfd346e90f404e1dd5c4b2c1debca3ea1abefe8400685d703aea6c5c7f"
};

// ECDSA verify - NIST CAVS 11.0 P-256, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp256r1_sha512_inv_hash) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp256r1 hash changed sha512",
    .p_input                   = "f1e9cda2e096ece9a1fc57e55eeeb56b1c635380c0f9a1800a4a1a5f105d1fc0c60e776234daaa8a6f7c0f5286bb420b3f607e7cc0a7d840ad5dcbab26c797b0",
    .p_qx                      = "484e31e69ef70bb8527853c22c6b6b4cd2a51311dde66c7b63f097dbb6ab27bf",
    .p_qy                      = "e1ff8177f4061d4fbbacbbc70519f0fc8c8b6053d72af0fe4f048d615004f74e",
    .p_r                       = "91a303d8fe3ab4176070f6406267f6b79bfe5eb5f62ae6aeb374d90667858518",
    .p_s                       = "e152119cefa26826ea07ec40a428869132d70812c5578c5a260e48d6800e046a"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA512)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP256R1)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP384R1)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA256)

// ECDSA verify - NIST CAVS 11.0 P-384, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp384r1_sha256_1) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp384r1 valid sha256 1",
    .p_input                   = "e3bcded61cbb0bf6ec20d59f91e8e73e532f15b082b89c984c1b51fb0d1db8a9",
    .p_qx                      = "86ac12dd0a7fe5b81fdae86b12435d316ef9392a3f50b307ab65d9c6079dd0d2d819dc09e22861459c2ed99fbab66fae",
    .p_qy                      = "ac8444077aaed6d6ccacbe67a4caacee0b5a094a3575ca12ea4b4774c030fe1c870c9249023f5dc4d9ad6e333668cc38",
    .p_r                       = "798065f1d1cbd3a1897794f4a025ed47565df773843f4fa74c85fe4d30e3a394783ec5723b530fc5f57906f946ce15e8",
    .p_s                       = "b57166044c57c7d9582066805b5885abc06e0bfc02433850c2b74973205ca357a2da94a65172086f5a1580baa697400b"
};

// ECDSA verify - NIST CAVS 11.0 P-384, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp384r1_sha256_2) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp384r1 valid sha256 2",
    .p_input                   = "45c3a1b29a18780234f12f5e4b64e7af9de2acf0029ce55b706cc79a7e4df994",
    .p_qx                      = "9a74ea00203c571bd91ae873ce0ed517f8f0a929c1854d68abd3b83a5051c0b686bb37d12958a54940cfa2de23902da7",
    .p_qy                      = "6f20ccf8fa360a9ec03d7bb79ff17ad885f714757ef62995f824908561dc0c3dffc49d873627936a2fff018b82879ced",
    .p_r                       = "acc1fcac98c593fb0a0765fce35a601c2e9570d63ea1e612fff8bc99ac2d4d877750bb44cfb1014e52e00b9235e350af",
    .p_s                       = "7f53de3afa4146b1447e829ebac8f5645e948cc99e871c07280cc631613cfdaf52ccaeccbe93588a3fd12170a7ec79fa"
};

// ECDSA verify - NIST CAVS 11.0 P-384, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp384r1_sha256_3) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp384r1 valid sha256 3",
    .p_input                   = "0c7152ec620fe9b783625196b41192dd5d49df184ad26965c970ac5e28bb1c4b",
    .p_qx                      = "fa8ebc3682d90ac7356f0b75b9e3376e76518676e0bedd176cfa7fa57fea4b3a399dbb2bf735ec90b9c1705cf9fa6f57",
    .p_qy                      = "18c3fbca0150ec10696b3851f31fb3ba62c0b6be509d249e0d4b374c7a08e49338e0922e2a8a9319999e6569ab8d292e",
    .p_r                       = "fb58ab09b8a7ef7a6ec05b854eae11af9b713f7c7540e25115f609846e636ad4f88dcf4dd61e311273df23ccda474f03",
    .p_s                       = "485be4c21b7c3a9c6b39ffc9f0c39f4050f76d2a6b3fae203d016318c541c1b4ad6cfc0d0950636ff6883895dd49e4e9"
};

// ECDSA verify - NIST CAVS 11.0 P-384, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp384r1_sha256_inv_r) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp384r1 R changed sha256",
    .p_input                   = "331aefe2369b9c5ee6dd9f850259b3b8512f5934434e61573f97fe2c1cd2b147",
    .p_qx                      = "0cf4dc51e71185a29c0c6fa3c075d9da5bd7ede085053344dce5dbbe8329e8ac9045f7246c9d0efed393b8e113c71429",
    .p_qy                      = "fdb7917b73974b355cf9f3bef6a0a460c2d39fdf1fe32a7744be0a54ddd1cfa8d03914cff4b5ca536b40707ff2629aa4",
    .p_r                       = "3812c2dc2881d7ef7f621993b161672329b261ff100bbd19fb5826c9face09aec2017b6843d69336b813b673c5402527",
    .p_s                       = "5dc102fab9d6325131c556ec00309c2959d1031a63fbc1e2d5d04996d3234ed33875c0ab98e5878e9bc72742519ed398"
};

// ECDSA verify - NIST CAVS 11.0 P-384, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp384r1_sha256_inv_s) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp384r1 S changed sha256",
    .p_input                   = "f5b47101b4ff9baf64aca830b6afbc4f9620035d88a1d84a12cefa6f7f99faf2",
    .p_qx                      = "10a784abb3c549444a62c28df1c926b8aabb20c8d9aa4b1f7ca830258857cbe9718dbc9845fa9cbb78587a373baee80d",
    .p_qy                      = "a1ad0c10b5ab6780cad49c8cd3eebd27de8f1b382ddd7a604458cef8e76ca632a7e44e1c63141a742426cec598029e2e",
    .p_r                       = "d9e52be2a3f7f566899cf6daaa38116d092473066f3a1bf91f3df44d81bca1deb438d9d25ce1632599c1d3576a30f128",
    .p_s                       = "0cad30bce4b3d7f40b3eef762a21bb1a3bad77439838b13024b7b2c70316875a99e80723a74a9e7a404715ca06a5d673"
};

// ECDSA verify - NIST CAVS 11.0 P-384, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp384r1_sha256_inv_q) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp384r1 Q changed sha256",
    .p_input                   = "647eb206a8477440b4bd048d00f37dca8635b15c2a8e79e2a9d74fb9a5553211",
    .p_qx                      = "97c3f446803a61a7014f61cb7f8b3f36486c7ea96d90ee1767f5c7e1d896dd5114255abb36c74be218c1f0a4e7ebba3d",
    .p_qy                      = "553ed1fed72c62851e042f0171454f120029adba4ee26855ab881d9470355f1947aa1d2e806a7ff2583660fedbd037a0",
    .p_r                       = "7b06d6c2b63f1cc3bfdaa897d07dc15a83bdf35d979f70c34578332b3f4920422bb24867c51bde10831324df424e04ec",
    .p_s                       = "4bef715161f400dc98d4b63bd13ff4ad4a6c981ead44bfc662fe9bca4b56cd790698e4deddf9a4bd69327f26bfe801e6"
};

// ECDSA verify - NIST CAVS 11.0 P-384, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp384r1_sha256_inv_hash) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp384r1 hash changed sha256",
    .p_input                   = "9a4985f744dd6f2774cb6f20ad6b6969e212abf4ac035b72ad3f8b1955ae1862",
    .p_qx                      = "08bd5c6cdc1f8c611df96485090e20e9188df6abb766bff3c1ba341ed209ad5dfd78b628ec60998ddfdd0dd029352fbd",
    .p_qy                      = "d9831d75dec760e9f405d1aa5e23aac506dc019fb64d44bd57f6c570d017e6609f8fdbb2dc7b28ca9e00e37cd32a3b73",
    .p_r                       = "8b372c86ed1eec2163d6f7152e53696b4a10958948d863eb622873b471702ac5b2e75ff852149a499e61510905f98e4c",
    .p_s                       = "b2ed728e8b30787a28f2a6d3740872e47348686c7cb426411379411310241d25f08a026b853789b1157f1fc1a7f6ff49"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA256)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA512)

// ECDSA verify - NIST CAVS 11.0 P-384, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp384r1_sha512_1) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp384r1 valid sha512 1",
    .p_input                   = "ea056beb112fa9aad69c8dfe51ea947b772bf1c11287edcede43a98089d21492ed581edcb6d1823e2873aabba213b84291db3bffa6eac3ae43a92fc2da276a24",
    .p_qx                      = "c665feccf51e6bca31593087df60f65b9fe14a12022814615deb892eedb99d86069a82aa91319310b66588185282dad6",
    .p_qy                      = "1e6e25bb8ae7714415b94f89def0f75dcb81d4af6b78d61f277b74b990c11aff51bd12fc88d691c99f2afde7fbd13e51",
    .p_r                       = "0e18c4063137468fe864fdc405ad4e120176eb91b4538b28ce43a22ae1a310cc22a2f7a2b3a0f3d15e0f82038b4a4301",
    .p_s                       = "5a1620e42041ce4357daf824befbb2ed65596bcd8214e88726149b26b1f416b9472a8877413f1c3705fc2edf4731943b"
};

// ECDSA verify - NIST CAVS 11.0 P-384, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp384r1_sha512_2) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp384r1 valid sha512 2",
    .p_input                   = "cd7c623c3c3b52f46be0ebb2b353ff97db3cd7dfc1a059a57668fc50101aeeb37b8aee9ddda8ab611546999a120cc9acb0e2c3df48dee66d5c31a46a7be94bc7",
    .p_qx                      = "83a4fecc0bf0a353b0acf6f54094b822f2b12564e172b296f3461cafa7315d7d31d0089b1b4c18ad3c86bd18f539774a",
    .p_qy                      = "e4fd57c5b2937e6fba1e7d72fc3f02352bd79c13611931935f4dfd073b9379f862f2277585137e996e212b5b6533dcba",
    .p_r                       = "fb02804010a570d702ebfbcf3d6cc9d55ddac2bd4b4de56d325e9790571b1737f91d3fa1d4caeec6eea806195aed3187",
    .p_s                       = "1fd20fe383e907e77639c05594642798619b2742090919bedeefb672c5700881baf0df19b9529d64bc7bb02683226103"
};

// ECDSA verify - NIST CAVS 11.0 P-384, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp384r1_sha512_3) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp384r1 valid sha512 3",
    .p_input                   = "dd7947a5b9a1c988dd7dff537e15335aacafd3e602adc8373765013f338334dd58aed4fb7144de0007c3410d79f5e78bcd4cf0dd63cc33ed3dd564882e299c7b",
    .p_qx                      = "45cb6dcca8d2e80ac04536a22f9d68ea2313245550108ddcd32799d154c0a55492e49463e826275bd9bf0d5e380205c1",
    .p_qy                      = "6fd124f5a6c745751ccfb3ba4dd9144ea8fd41a4d9a4b34820434da66aa7385e73ffe71e6c11ed1beb6c7af22ce00edf",
    .p_r                       = "2c782c4263eeee63657fbf20fa287a1a81fcd14b1d3bae333928ba4fc31abb20edebc130714380608e38ea74309eca9d",
    .p_s                       = "716113d95bc9dba532bfb470112b0d43d9cd6560ad15e0de2e514994801ff339bcf19ad4ee2b8af573f57c038fbd70f0"
};

// ECDSA verify - NIST CAVS 11.0 P-384, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp384r1_sha512_inv_r) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp384r1 R changed sha512",
    .p_input                   = "c0f9ae90fe8aaf54962e7d47a832e4ca6e60355e4066cd2b08bff78650d4e4a5d1eb1de296f9f0ef92887e09f82e0db4411aa9c3c6b109159bd39feed40419a3",
    .p_qx                      = "9c1eb5cdb1a873e4c275b7ded8712b9058ee0d9ded06c96a2a8d7c652b82e894e2f918dd8e18138e5c34821744b97952",
    .p_qy                      = "dd474c93619f02b5d4fe30ea7805c1a13fb80008a81bb5f3eeb95cd11f38841b8e34d64f2c6cc2d6cc2587365eed6b6e",
    .p_r                       = "f17b2f2fa3b5c8e9c62a633e5d417139ddf3dafba75b464fa156c99b3948a0aca532c7fd3e14a266eb17e7fa80881da2",
    .p_s                       = "01c246866983fa74d6dff38b1ea091f8afd218b5a42467761b147c19a3bb20cd24be8ed1f95f1e61863a709d2d0148e2"
};

// ECDSA verify - NIST CAVS 11.0 P-384, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp384r1_sha512_inv_s) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp384r1 S changed sha512",
    .p_input                   = "242ff2713c03e3d5277652f8e7fb1e5a1f0422b6652e1bdd696e46c03cdd3aaac329b1d88e7aa345ff7224ce6dc6df05c7e9d7dc2665282c817d15a15b8288fd",
    .p_qx                      = "36c1459d9e9f7b6c1598778c784cbf94661a2b11370c02ee092f6ea0ca20acf81f1ed5048a28a1466a91689df26bc291",
    .p_qy                      = "d1367418c7b216bd32c6dafc8b2be99d02cab68df990758b2ddd543b7eb6ff6e285b649ffe588b1811b549cfb5f0289b",
    .p_r                       = "40c338adeb504193444bdb95336177362031aaadc5b7e151e42030df9dd8687f3cb8fe2292fd4f9206989c089d966dae",
    .p_s                       = "be4b2ba251094c24de006c89af2b5c77e6937f36d7bb703b4f8edcfe65d45f4b2fd2486222163ae0ed9e215c0a96f488"
};

// ECDSA verify - NIST CAVS 11.0 P-384, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp384r1_sha512_inv_q) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp384r1 Q changed sha512",
    .p_input                   = "d5c82ff11f555ce21c3f20a9ecfa6047cb6895e32fa0fb379f49085a59f61b7c8fa05058ef144cf47db5738fa40f4890cb59695998a2358162bbbf6d7f53517b",
    .p_qx                      = "d4e93c4bafb54c06814011309e9f3d8e68b76a5452e364ef05ccc3b44b271e576c9028106b1584f09271c886d467f41d",
    .p_qy                      = "db730ccfdeb6644362f4fb510d5254bfe6f23e891e936132f90f1913e93baa8b1f8c0613a0f0c61a760ce659f22babc6",
    .p_r                       = "8d0fd14a59c24b0c2a34b438e162f1f536fe09a698cacfe0760d026d1593265d02f2668d2a5e49ac0b21e93807aa9c18",
    .p_s                       = "3162ffd2adc9dd5ec1bb1d97d2b0c27b8ae234235ffb374878d0b76382002ea505e885c178d56a2d7809bd1d83117ef1"
};

// ECDSA verify - NIST CAVS 11.0 P-384, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp384r1_sha512_inv_hash) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp384r1 hash changed sha512",
    .p_input                   = "81b1303e10f25d37877b09f9d82dbd894e40264992d86cc74656ebeef505b46fdf9dec312a7f0a26e3f56a7195d5b01d198c378fff9d049e00cbad9586da20c9",
    .p_qx                      = "a6bbf85e8068151482ce855ccf0ed22988fcf4b162c4b811cb7243b849299e3390a083147fbd68683203ba33588b13ae",
    .p_qy                      = "5c837ec9f2eda225c83ab2d5f10b1aa5bfb56387deebf27ecda779f6254a17968260247c75dd813ea0e1926887d46f86",
    .p_r                       = "9c11879e59659848274fc1ef5a6a181af813d23708b09a24dc06c089b93b918828dd938a75a34d5a681b0af362dc19a0",
    .p_s                       = "9c362231962ba7579c4a874e87bdc60dc15cb2e0677149c8ea31162963e05a6614616f67a5269616071cf095be7ff44b"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA512)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP384R1)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP521R1)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA256)

// ECDSA verify - NIST CAVS 11.0 P-521, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp521r1_sha256_1) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp521r1 valid sha256 1",
    .p_input                   = "61bcad3941eeb4005c391745635dbca42b451c3222b6123af2cd1279f89f3b2d",
    .p_qx                      = "00397714abcc503eaa0c18abd1fd26586d28ec1b1035d37ac710f2823911ec9afa429b41ea89cec13d5bcae9d6d7147794407e409f3b267cf4dd27e8c77e7ccf4d36",
    .p_qy                      = "00a3a4b749d19b84708e42b59e9faa5a99ac0f0a01121655fab87785fca38c8cd4277c8c2c9a0024ff608c3cce954596315dfe0e3b133aeab08bb5389eb2a4f1fb42",
    .p_r                       = "019da96a866db12948e0aec7231f797061f345739d439bdaaba63e4d03e0bb52c3fea2fb593347d983f24a3afa6a77f476e6bb49a5de843b4c4755cddce97b8b909e",
    .p_s                       = "001bb442f428b2ca445a75ad88ed49d965d6659d748d02cebf78faa1ecc187b606f284d11d47791d585dc371c2d91848a55ca7b092f06d561efcf64e0de0814e1db4"
};

// ECDSA verify - NIST CAVS 11.0 P-521, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp521r1_sha256_2) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp521r1 valid sha256 2",
    .p_input                   = "c11b8b03fcd4f2ae25f60af2ec2ccc9afcbf760f61782fad21a02d69b1024cdb",
    .p_qx                      = "0176f1276918fed24a098d6d03077f3c33ae543316df1b6b06ce877e74b69b2cd4131fdf797e77e5f6391b0b32411120d03c0c59ba1721a7187d18708121d6f3a86f",
    .p_qy                      = "010f9d38b30a2da1a745840de7c9994578e32bb10f9334b46f533b6eab550aa55048e4ac601889564ac8314e01b61613fc7b8e2bd3f1a188c5c5e869af16a8d61d9b",
    .p_r                       = "019cb5639a321e95214c90a612d29c9ffd5ae5aaa2a814ee2d66ac1ce1d2ab3229009129ec9d472061444cbfbf50c7e4cba09aab65299a42740bce7af3fddf2a1f46",
    .p_s                       = "00082ce6bf1d809d3bb4f9f09a95590bb64b0c41bcee5fcdd332947a9b59618da5da897fff44968d92635e7833dec1e91d8d99bd8b527609393b446c83d109a32243"
};

// ECDSA verify - NIST CAVS 11.0 P-521, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp521r1_sha256_3) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp521r1 valid sha256 3",
    .p_input                   = "b7e0d79517efe3d961ca5bbdc0916bab7a32743f5150d53cd60300b3edceff49",
    .p_qx                      = "00aa42473f80d9d81f6d41ed05c8ba35c005f90e2690f71dfdb12555b7590c7a8e95b618368c39f4e84d6cba25f522c9bdd256c60d3f8c8425ad313701225a9cc9c4",
    .p_qy                      = "01992b7966b925f42c91f810eb05d602b804301849ea278466a68e5b616e3a0bce110fc9250db14f9c8f5929347e1bb8727bcf8072c6aebc26958954fe96df04e139",
    .p_r                       = "00cbb35513420f206bd26b568712503b66e159a54e154c8d4e9c661aa954e0bf425871275fff5e8f368c8ccc77ffe6adf84ba88a84483d8ba5cc862bd408f6a192c1",
    .p_s                       = "002ffb4e461e3161c801ad217a0483045181013deed29eec29cca94776139ddf5fe9d7771e5ac7b637a4bf7e5276940489bd8ae36f41ef6be93cff4b96bd0e1f3e59"
};

// ECDSA verify - NIST CAVS 11.0 P-521, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp521r1_sha256_inv_r) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp521r1 R changed sha256",
    .p_input                   = "6b7b1a121d5e8f4d3d0294abbb62ae16561f37b6fbdd6d519209c6663669323b",
    .p_qx                      = "009f21a6e7295b183656709089b3c647140c81f71b0b3812e6de22c52245335599ade6a3116cb70277dc2485f91c7b1f46d62afb60fc17a110358c9a02e02e010960",
    .p_qy                      = "01e914284cea47dd6836e7ce899d0c9a88d67fc9d039ffa9fa5bee58d247e0d0dc9251be8b82afd3add327f98c5570bdcd8ad8827820032774d19db09232aeba190a",
    .p_r                       = "00ce4b2ac68afd071531027b90d4b92d9b0e1044b824ccebb2c9ab241d5b909ead1ffa2dc3d330f57187efbea7374bc77c4f7ce7ee689aa5a1e27aa78abc3cc1e751",
    .p_s                       = "00aa85d84f9c7fecd25064dbae69c16d6fcff38040027bf476c7f913746272b5d4b9bd34d2482e27730522df724895b99253aed86011139928fa9a272892f8c99d8f"
};

// ECDSA verify - NIST CAVS 11.0 P-521, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp521r1_sha256_inv_s) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp521r1 S changed sha256",
    .p_input                   = "2e139493b5db177dd536c562e0c90445b59a686eeb27e0a7b7cd93d72fa4ab7d",
    .p_qx                      = "006d8c16536b17cab6ff41f5df4038fe416c05ccb601710909708dc561b02ceed9cf020441d9daa075e8fd604531ff58084035b1c19a498b82582f5b20f9cedf61f9",
    .p_qy                      = "00e89d71c66e55c4f5bf245413388bfe9de83944b11d1abdb4692db7da8a086442965ee512f7089f89464dda5d7786e52cc26a8a30bc8824cc56a289fefcd42bdfd2",
    .p_r                       = "0087f86cf4bd36e8253097ac1bc8500dedafdbccbe5767ec25e53c73c4f053f3b37acd1d5ea4c16e4058919b61d2a67393220ffefe07535d53923ace6815463c4c31",
    .p_s                       = "01def2582fd0df89fa28c9ce882f5c3846135f51bdf7f4b2497b190136ef04618eaa22a8c5a117b0adfc6425eac3111b6558df145a8b14ad39524b98659e01d51c21"
};

// ECDSA verify - NIST CAVS 11.0 P-521, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp521r1_sha256_inv_q) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp521r1 Q changed sha256",
    .p_input                   = "63c7b65f78581fc15992ac54b1221f16545a6e50a0c740e3f3a1685f09f358a8",
    .p_qx                      = "00defff5ef7cc5de0e1ac32261e7a74e8c434c0b51f76df7566b612cc5b8201e7b38c51aa6118b6307f436394bf452a72224c977e37e410eae9525df2ee00a8123bf",
    .p_qy                      = "00263b7db73558ddc783824f0b19776802aaf5e46ccb1b1d1dda07d2d6c5843f5036ae8d381b235ccd2ed04eb90c5d51e32cbd7acdc7031cae63c06797556fb66fe3",
    .p_r                       = "0089bd129a537840a52ef434d5a8ba4add952f72f22a84ac4523ea0bc02cbfa8b681ab0ed3fa2bca24ae575f23fce7efbb9bfd28e465174158a5ad2b08fd9e0b7132",
    .p_s                       = "0004ed533337791e05f8d097eabdf4be96b3fcc9f876d47fb8c5c7a05cbddba398cded2edf5ec9b7dbb4e32c1374b46953d66a193c211ef12de4b9d73adc369d5e95"
};

// ECDSA verify - NIST CAVS 11.0 P-521, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp521r1_sha256_inv_hash) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp521r1 hash changed sha256",
    .p_input                   = "3dfb0c771418d4f1ffd092de8aae7a563af2c9e559ad0b53a5b61090ea77eacd",
    .p_qx                      = "015bd9bf7a35cc60147b32b64e0e4e54bf9ac2173cc6784b3d4ebd076aa5d45c1e3d0846b20b61d6342341a8801a2f63028c991831318245c2fe31f8acde6bf2003e",
    .p_qy                      = "01afb67c9c700ed332b47a2d148e6ddd3571e138f02a81c3cfe6d4dee0f512d92e76574fe5797c5566c05b3239fabb212c735615e719e718fb40fa6783c964357f72",
    .p_r                       = "01a341d0e8906239faace79554b90d1445bd28f703d7c7cc8eb163337ad3d4bfb3725cb06e618991491534d399866df5c5bdef897c889947b21148d89c657e64124d",
    .p_s                       = "005c5b728837d44b7b6935efb2b721b4f45c1675d803d87f70158e451434176d9682034c9b356b5f9181e07599bdcb55e5bc808fdd36fef9c19ddb6342c975262024"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA256)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA512)

// ECDSA verify - NIST CAVS 11.0 P-521, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp521r1_sha512_1) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp521r1 valid sha512 1",
    .p_input                   = "97ff5a81fc88f7ddd3bc58154ffd2695912fe50ce7c63b62bd798fb673c6aa49f54bc7301fb7bddc6edc51b7e0d0b4dec9f80851fff02a33671ad9a406bbabe5",
    .p_qx                      = "0153eb2be05438e5c1effb41b413efc2843b927cbf19f0bc9cc14b693eee26394a0d8880dc946a06656bcd09871544a5f15c7a1fa68e00cdc728c7cfb9c448034867",
    .p_qy                      = "0143ae8eecbce8fcf6b16e6159b2970a9ceb32c17c1d878c09317311b7519ed5ece3374e7929f338ddd0ec0522d81f2fa4fa47033ef0c0872dc049bb89233eef9bc1",
    .p_r                       = "00dd633947446d0d51a96a0173c01125858abb2bece670af922a92dedcec067136c1fa92e5fa73d7116ac9c1a42b9cb642e4ac19310b049e48c53011ffc6e7461c36",
    .p_s                       = "00efbdc6a414bb8d663bb5cdb7c586bccfe7589049076f98cee82cdb5d203fddb2e0ffb77954959dfa5ed0de850e42a86f5a63c5a6592e9b9b8bd1b40557b9cd0cc0"
};

// ECDSA verify - NIST CAVS 11.0 P-521, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp521r1_sha512_2) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp521r1 valid sha512 2",
    .p_input                   = "f8e150be2f657c8266fadc9bdb04648fc5a51f3c3f7521022aaf58d24165f8af4ad66319d8aa2dab48fe8a2f773c8d0e6c8c4f732e0fdfbae4b91918530c1f91",
    .p_qx                      = "00365388d9589c18ae608124b4cf746ff488183a912e07d26b6e867c5defb552a5a0df5a16b6342014dd1b0b6760072bcd60045d6a9a514fc74d16047c2e8765636d",
    .p_qy                      = "01a5319b26fd555f2a12e557418f6aa65a3461aeaea5c0c6d8698ceaa5495eed7a7d2fed0b76e77b5be11834f36e413d5288e47231c0eb0e9007d4b042bb7a1b6014",
    .p_r                       = "01d9ef377063a592cf81e27815a2c20789ff9b60f7f125e618b52d90b35abdd41cd7f437cfad337953ab0314fe8e79a2f2d27fa08597d4b28313358f714a737321fb",
    .p_s                       = "00f01d4f150e0a174674a6a61a58a4ba781406024f6dd1b5252e04807b8a807a4ff8d52883eaa258286e506ef4b04ca890e6f81a79ed9a0cd5ed585094fea0bc5c43"
};

// ECDSA verify - NIST CAVS 11.0 P-521, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp521r1_sha512_3) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_err_code         = NRF_SUCCESS,
    .p_test_vector_name        = "secp521r1 valid sha512 3",
    .p_input                   = "8814a9dbef9e6d9b8322bdf8d471b207388bb7bf831d9fba8ad29da52d528d5d5108c01e4459f5ca13e26bf5da3c848195558828d7a00f53abb9fce47ef35091",
    .p_qx                      = "010d587aa82a4d8e690672c00e3fd71826d892862d14dc4fbad4935aaab86924dc7ee6f7fd3e2bbe86a8652589448494dab83d363d1d623cbae59f6c2670706a0576",
    .p_qy                      = "01a9734c99b6ff21267050738937c30971d0f6fe07e29794748a5017ea1036c975c9a52e6d3739ca0e8d70e784529cc1a7437aac5d75c69121b69020a95356137f1d",
    .p_r                       = "0188dcb840dfc573a97117009226d58dbb930ba8ec848931786abc770611f3519c8ba73cceb5b489170805bcf04974672fe66c908ba379aca99fa67fec81a994c2d1",
    .p_s                       = "000b1a185512dc6a65e454ea2bdb8049ef8f012a53ae87b759fb5d9edba51ea32e254e80545a99eb4b7c58af96b7c433535fa3f009cc644b1c97666d88355af9fc19"
};

// ECDSA verify - NIST CAVS 11.0 P-521, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp521r1_sha512_inv_r) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp521r1 R changed sha512",
    .p_input                   = "7679eaaf0495725fa99c51a2dd0c35c8882b840e1c2340ba793013b1e2567471cba35c0dd6247cc2c2ca14f6556912a5687023fb2f0ee02114393bed4c598742",
    .p_qx                      = "012a593f568ca2571e543e00066ecd3a3272a57e1c94fe311e5df96afc1b792e5862720fc730e62052bbf3e118d3a078f0144fc00c9d8baaaa8298ff63981d09d911",
    .p_qy                      = "017cea5ae75a74100ee03cdf2468393eef55ddabfe8fd5718e88903eb9fd241e8cbf9c68ae16f4a1db26c6352afcb1894a9812da6d32cb862021c86cd8aa483afc26",
    .p_r                       = "01aac7692baf3aa94a97907307010895efc1337cdd686f9ef2fd8404796a74701e55b03ceef41f3e6f50a0eeea11869c4789a3e8ab5b77324961d081e1a3377ccc91",
    .p_s                       = "0009c1e7d93d056b5a97759458d58c49134a45071854b8a6b8272f9fe7e78e1f3d8097e8a6e731f7ab4851eb26d5aa4fdadba6296dc7af835fe3d1b6dba4b031d5f3"
};

// ECDSA verify - NIST CAVS 11.0 P-521, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp521r1_sha512_inv_s) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp521r1 S changed sha512",
    .p_input                   = "b99c410653ce928e365d3613331b5df067020e92f634696279d5cee80f1f4a82f7d976a059e318b36eb25314b56f8765a81070d0944f4c86e8407d9c3e2aa7da",
    .p_qx                      = "01d6aef44370325a8a5882f4667c21172cdc8fa41d712562883ececff53883ac8ee276124e825088c79d6c9d96323cb7b8c0b7ea44d3f0026e2538f4b62d785bb1af",
    .p_qy                      = "0027203959a6e944b91fe6306debe74dc5dde9831fd0ec27e8be2d0b56807d63151b15f6495b8632e919e1e6b015f5ae5f2b6fb8cf75b5f848f00cf4ee457cebed3a",
    .p_r                       = "004417ff74889dde6bb1820b5d13da5c81dcf9b0723ee89bb1ff0d3faa90d497685709f315b2cbe55481dee43ebb6d25b1501ae69494dd69e7bffb72f987d1573b93",
    .p_s                       = "00fd7aa027c665458c7ac11d54d4f32cb4a1e727b499ce27b08d3d647c636cc3222a4f0a6057732249ddc22574d7cb80c3769c3ea9de3d33db3edd8ea90cb3f8dc8a"
};

// ECDSA verify - NIST CAVS 11.0 P-521, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp521r1_sha512_inv_q) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp521r1 Q changed sha512",
    .p_input                   = "996010910456dee59309f1631f30e3dbf7ac2da7d5d7f69223c8a18f491cb18f7e11d0ca09352b715354a071e6d392a8c1dc0751569bdfcf36c158c8b07a5ba6",
    .p_qx                      = "0007067d2cf7b7619b9fcff2c898246ae0950439b8bab92d809624970eda18456cb99953ce1ae45ee5d36ef02fcd5caa4d951de8581f0c21e572caad56d6dce60da3",
    .p_qy                      = "01913c59007a309005f226b6a30122828d60b4d0390359e1977f88b5347dacf2056dd362648e8b1d6fc038a3bd3fde6f1140c740efa9075ab8b4a64b334c5cd43f09",
    .p_r                       = "012aa4a532c108aa3cfb1753f95ca626bb72bd96a423d727656d4ebdc3f406d6cc6c44d3718f9abae8a0b46be9b57f8fd3a540326b63d0d4a8a93165715920437787",
    .p_s                       = "001badaf38e16efd75915f4806f054d40abd2d11e402039bd48c832f66cbfd145e4dac93357d476b7e608d7b75a017374ae76eee86c505f2cc16eaa19075827ccd60"
};

// ECDSA verify - NIST CAVS 11.0 P-521, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_verify_data, test_vector_ecdsa_verify_t test_vector_ecdsa_verify_secp521r1_sha512_inv_hash) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_err_code         = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp521r1 hash changed sha512",
    .p_input                   = "ee21776d7174103b7fb65f03fd5d78744d2706c6726ece81e3943cf90f60fad6d8978af6cae9bc059aee2412ef86d0600694447a10b9d21079b9ca77500634a9",
    .p_qx                      = "01184b27a48e223891cbd1f4a0255747d078f82768157e5adcc8e78355a2ff17d8363dfa39bcdb48e2fae759ea3bd6a8909ce1b2e7c20653915b7cd7b94d8f110349",
    .p_qy                      = "003bd6e273ee4278743f1bb71ff7aefe1f2c52954d674c96f268f3985e69727f22adbe31e0dbe01da91e3e6d19baf8efa4dcb4d1cacd06a8efe1b617bd681839e6b9",
    .p_r                       = "004c1d88d03878f967133eb56714945d3c89c3200fad08bd2d3b930190246bf8d43e453643c94fdab9c646c5a11271c800d5df25c11927c000263e785251d62acd59",
    .p_s                       = "012e31766af5c605a1a67834702052e7e56bbd9e2381163a9bf16b579912a98bebabb70587da58bec621c1e779a8a21c193dda0785018fd58034f9a6ac3e297e3790"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_HASH_SHA512)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP521R1_ENABLED)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP160K1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP160K1)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP192K1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP192K1)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP224K1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP224K1)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP256K1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP256K1)

/*lint -restore */

#endif // NRF_CRYPTO_ECC_ENABLED
