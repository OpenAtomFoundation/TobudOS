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

/**@brief EdDSA test vectors can be found on the NIST web pages.
 *
 * https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Component-Testing
 */

#if NRF_CRYPTO_ECC_ENABLED

/*lint -save -e91 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_ED25519)

const char eddsa_ed25519_input_test3[] = { "af82" };
const char eddsa_ed25519_qx_test3[]    = { "fc51cd8e6218a1a38da47ed00230f058" };
const char eddsa_ed25519_qy_test3[]    = { "0816ed13ba3303ac5deb911548908025" };
const char eddsa_ed25519_r_test3[]     = { "6291d657deec24024827e69c3abe01a30ce548a284743a445e3680d7db5ac3ac" };
const char eddsa_ed25519_s_test3[]     = { "18ff9b538d16f290ae67f760984dc6594a7c15e9716ed28dc027beceea1ec40a" };

// EdDSA sign - RFC 8032 Edwards-Curve Digital Signature Algorithm (EdDSA) - Test 1
NRF_SECTION_ITEM_REGISTER(test_vector_eddsa_sign_data, test_vector_ecdsa_sign_t test_vector_eddsa_sign_ed25519_1) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_ed25519_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_SUCCESS,
    .p_test_vector_name        = "ed25519 message_len=0",
    .p_input                   = "",
    .p_qx                      = "d75a980182b10ab7d54bfed3c964073a",
    .p_qy                      = "0ee172f3daa62325af021a68f707511a",
    .p_x                       = "9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60"
};


// EdDSA sign - RFC 8032 Edwards-Curve Digital Signature Algorithm (EdDSA) - Test 2
NRF_SECTION_ITEM_REGISTER(test_vector_eddsa_sign_data, test_vector_ecdsa_sign_t test_vector_eddsa_sign_ed25519_2) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_ed25519_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_SUCCESS,
    .p_test_vector_name        = "ed25519 message_len=1",
    .p_input                   = "72",
    .p_qx                      = "3d4017c3e843895a92b70aa74d1b7ebc",
    .p_qy                      = "9c982ccf2ec4968cc0cd55f12af4660c",
    .p_x                       = "4ccd089b28ff96da9db6c346ec114e0f5b8a319f35aba624da8cf6ed4fb8a6fb"
};

// EdDSA sign - RFC 8032 Edwards-Curve Digital Signature Algorithm (EdDSA) - Test 3
NRF_SECTION_ITEM_REGISTER(test_vector_eddsa_sign_data, test_vector_ecdsa_sign_t test_vector_eddsa_sign_ed25519_3) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_ed25519_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_SUCCESS,
    .p_test_vector_name        = "ed25519 message_len=2",
    .p_input                   = "af82",
    .p_qx                      = "fc51cd8e6218a1a38da47ed00230f058",
    .p_qy                      = "0816ed13ba3303ac5deb911548908025",
    .p_x                       = "c5aa8df43f9f837bedb7442f31dcb7b166d38535076f094b85ce3a2e0b4458f7"
};

// EdDSA sign - RFC 8032 Edwards-Curve Digital Signature Algorithm (EdDSA) - Test 4
NRF_SECTION_ITEM_REGISTER(test_vector_eddsa_sign_data, test_vector_ecdsa_sign_t test_vector_eddsa_sign_ed25519_4) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_ed25519_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_SUCCESS,
    .p_test_vector_name        = "ed25519 message_len=1023",
    .p_input                   = "08b8b2b733424243760fe426a4b54908632110a66c2f6591eabd3345e3e4eb98fa6e264bf09efe12ee50f8f54e9f77b1e355f6c50544e23fb1433ddf73be84d879de7c0046dc4996d9e773f4bc9efe5738829adb26c81b37c93a1b270b20329d658675fc6ea534e0810a4432826bf58c941efb65d57a338bbd2e26640f89ffbc1a858efcb8550ee3a5e1998bd177e93a7363c344fe6b199ee5d02e82d522c4feba15452f80288a821a579116ec6dad2b3b310da903401aa62100ab5d1a36553e06203b33890cc9b832f79ef80560ccb9a39ce767967ed628c6ad573cb116dbefefd75499da96bd68a8a97b928a8bbc103b6621fcde2beca1231d206be6cd9ec7aff6f6c94fcd7204ed3455c68c83f4a41da4af2b74ef5c53f1d8ac70bdcb7ed185ce81bd84359d44254d95629e9855a94a7c1958d1f8ada5d0532ed8a5aa3fb2d17ba70eb6248e594e1a2297acbbb39d502f1a8c6eb6f1ce22b3de1a1f40cc24554119a831a9aad6079cad88425de6bde1a9187ebb6092cf67bf2b13fd65f27088d78b7e883c8759d2c4f5c65adb7553878ad575f9fad878e80a0c9ba63bcbcc2732e69485bbc9c90bfbd62481d9089beccf80cfe2df16a2cf65bd92dd597b0707e0917af48bbb75fed413d238f5555a7a569d80c3414a8d0859dc65a46128bab27af87a71314f318c782b23ebfe808b82b0ce26401d2e22f04d83d1255dc51addd3b75a2b1ae0784504df543af8969be3ea7082ff7fc9888c144da2af58429ec96031dbcad3dad9af0dcbaaaf268cb8fcffead94f3c7ca495e056a9b47acdb751fb73e666c6c655ade8297297d07ad1ba5e43f1bca32301651339e22904cc8c42f58c30c04aafdb038dda0847dd988dcda6f3bfd15c4b4c4525004aa06eeff8ca61783aacec57fb3d1f92b0fe2fd1a85f6724517b65e614ad6808d6f6ee34dff7310fdc82aebfd904b01e1dc54b2927094b2db68d6f903b68401adebf5a7e08d78ff4ef5d63653a65040cf9bfd4aca7984a74d37145986780fc0b16ac451649de6188a7dbdf191f64b5fc5e2ab47b57f7f7276cd419c17a3ca8e1b939ae49e488acba6b965610b5480109c8b17b80e1b7b750dfc7598d5d5011fd2dcc5600a32ef5b52a1ecc820e308aa342721aac0943bf6686b64b2579376504ccc493d97e6aed3fb0f9cd71a43dd497f01f17c0e2cb3797aa2a2f256656168e6c496afc5fb93246f6b1116398a346f1a641f3b041e989f7914f90cc2c7fff357876e506b50d334ba77c225bc307ba537152f3f1610e4eafe595f6d9d90d11faa933a15ef1369546868a7f3a45a96768d40fd9d03412c091c6315cf4fde7cb68606937380db2eaaa707b4c4185c32eddcdd306705e4dc1ffc872eeee475a64dfac86aba41c0618983f8741c5ef68d3a101e8a3b8cac60c905c15fc910840b94c00a0b9d0",
    .p_qx                      = "278117fc144c72340f67d0f2316e8386",
    .p_qy                      = "ceffbf2b2428c9c51fef7c597f1d426e",
    .p_x                       = "f5e5767cf153319517630f226876b86c8160cc583bc013744c6bf255f5cc0ee5"
};


// EdDSA verify - RFC 8032 Edwards-Curve Digital Signature Algorithm (EdDSA) - Test 1
NRF_SECTION_ITEM_REGISTER(test_vector_eddsa_verify_data, test_vector_ecdsa_verify_t test_vector_eddsa_verify_ed25519_1) =
{
    .p_curve_info         = &g_nrf_crypto_ecc_ed25519_curve_info,
    .expected_err_code    = NRF_SUCCESS,
    .p_test_vector_name   = "ed25519 valid 1",
    .p_input              = "",
    .p_qx                 = "d75a980182b10ab7d54bfed3c964073a",
    .p_qy                 = "0ee172f3daa62325af021a68f707511a",
    .p_r                  = "e5564300c360ac729086e2cc806e828a84877f1eb8e5d974d873e06522490155",
    .p_s                  = "5fb8821590a33bacc61e39701cf9b46bd25bf5f0595bbe24655141438e7a100b"
};

// EdDSA verify - RFC 8032 Edwards-Curve Digital Signature Algorithm (EdDSA) - Test 2
NRF_SECTION_ITEM_REGISTER(test_vector_eddsa_verify_data, test_vector_ecdsa_verify_t test_vector_eddsa_verify_ed25519_2) =
{
    .p_curve_info         = &g_nrf_crypto_ecc_ed25519_curve_info,
    .expected_err_code    = NRF_SUCCESS,
    .p_test_vector_name   = "ed25519 valid 2",
    .p_input              = "72",
    .p_qx                 = "3d4017c3e843895a92b70aa74d1b7ebc",
    .p_qy                 = "9c982ccf2ec4968cc0cd55f12af4660c",
    .p_r                  = "92a009a9f0d4cab8720e820b5f642540a2b27b5416503f8fb3762223ebdb69da",
    .p_s                  = "085ac1e43e15996e458f3613d0f11d8c387b2eaeb4302aeeb00d291612bb0c00"
};

// EdDSA verify - RFC 8032 Edwards-Curve Digital Signature Algorithm (EdDSA) - Test 3
NRF_SECTION_ITEM_REGISTER(test_vector_eddsa_verify_data, test_vector_ecdsa_verify_t test_vector_eddsa_verify_ed25519_3) =
{
    .p_curve_info         = &g_nrf_crypto_ecc_ed25519_curve_info,
    .expected_err_code    = NRF_SUCCESS,
    .p_test_vector_name   = "ed25519 valid 3",
    .p_input              = eddsa_ed25519_input_test3,
    .p_qx                 = eddsa_ed25519_qx_test3,
    .p_qy                 = eddsa_ed25519_qy_test3,
    .p_r                  = eddsa_ed25519_r_test3,
    .p_s                  = eddsa_ed25519_s_test3
};

// EdDSA verify - RFC 8032 Edwards-Curve Digital Signature Algorithm (EdDSA) - Test 4
NRF_SECTION_ITEM_REGISTER(test_vector_eddsa_verify_data, test_vector_ecdsa_verify_t test_vector_eddsa_verify_ed25519_4) =
{
    .p_curve_info         = &g_nrf_crypto_ecc_ed25519_curve_info,
    .expected_err_code    = NRF_SUCCESS,
    .p_test_vector_name   = "ed25519 valid 4",
    .p_input              = "08b8b2b733424243760fe426a4b54908632110a66c2f6591eabd3345e3e4eb98fa6e264bf09efe12ee50f8f54e9f77b1e355f6c50544e23fb1433ddf73be84d879de7c0046dc4996d9e773f4bc9efe5738829adb26c81b37c93a1b270b20329d658675fc6ea534e0810a4432826bf58c941efb65d57a338bbd2e26640f89ffbc1a858efcb8550ee3a5e1998bd177e93a7363c344fe6b199ee5d02e82d522c4feba15452f80288a821a579116ec6dad2b3b310da903401aa62100ab5d1a36553e06203b33890cc9b832f79ef80560ccb9a39ce767967ed628c6ad573cb116dbefefd75499da96bd68a8a97b928a8bbc103b6621fcde2beca1231d206be6cd9ec7aff6f6c94fcd7204ed3455c68c83f4a41da4af2b74ef5c53f1d8ac70bdcb7ed185ce81bd84359d44254d95629e9855a94a7c1958d1f8ada5d0532ed8a5aa3fb2d17ba70eb6248e594e1a2297acbbb39d502f1a8c6eb6f1ce22b3de1a1f40cc24554119a831a9aad6079cad88425de6bde1a9187ebb6092cf67bf2b13fd65f27088d78b7e883c8759d2c4f5c65adb7553878ad575f9fad878e80a0c9ba63bcbcc2732e69485bbc9c90bfbd62481d9089beccf80cfe2df16a2cf65bd92dd597b0707e0917af48bbb75fed413d238f5555a7a569d80c3414a8d0859dc65a46128bab27af87a71314f318c782b23ebfe808b82b0ce26401d2e22f04d83d1255dc51addd3b75a2b1ae0784504df543af8969be3ea7082ff7fc9888c144da2af58429ec96031dbcad3dad9af0dcbaaaf268cb8fcffead94f3c7ca495e056a9b47acdb751fb73e666c6c655ade8297297d07ad1ba5e43f1bca32301651339e22904cc8c42f58c30c04aafdb038dda0847dd988dcda6f3bfd15c4b4c4525004aa06eeff8ca61783aacec57fb3d1f92b0fe2fd1a85f6724517b65e614ad6808d6f6ee34dff7310fdc82aebfd904b01e1dc54b2927094b2db68d6f903b68401adebf5a7e08d78ff4ef5d63653a65040cf9bfd4aca7984a74d37145986780fc0b16ac451649de6188a7dbdf191f64b5fc5e2ab47b57f7f7276cd419c17a3ca8e1b939ae49e488acba6b965610b5480109c8b17b80e1b7b750dfc7598d5d5011fd2dcc5600a32ef5b52a1ecc820e308aa342721aac0943bf6686b64b2579376504ccc493d97e6aed3fb0f9cd71a43dd497f01f17c0e2cb3797aa2a2f256656168e6c496afc5fb93246f6b1116398a346f1a641f3b041e989f7914f90cc2c7fff357876e506b50d334ba77c225bc307ba537152f3f1610e4eafe595f6d9d90d11faa933a15ef1369546868a7f3a45a96768d40fd9d03412c091c6315cf4fde7cb68606937380db2eaaa707b4c4185c32eddcdd306705e4dc1ffc872eeee475a64dfac86aba41c0618983f8741c5ef68d3a101e8a3b8cac60c905c15fc910840b94c00a0b9d0",
    .p_qx                 = "278117fc144c72340f67d0f2316e8386",
    .p_qy                 = "ceffbf2b2428c9c51fef7c597f1d426e",
    .p_r                  = "0aab4c900501b3e24d7cdf4663326a3a87df5e4843b2cbdb67cbf6e460fec350",
    .p_s                  = "aa5371b1508f9f4528ecea23c436d94b5e8fcd4f681e30a6ac00a9704a188a03"
};

// EdDSA verify - RFC 8032 Edwards-Curve Digital Signature Algorithm (EdDSA) - Test 3
NRF_SECTION_ITEM_REGISTER(test_vector_eddsa_verify_data, test_vector_ecdsa_verify_t test_vector_eddsa_verify_ed25519_invalid_input) =
{
    .p_curve_info         = &g_nrf_crypto_ecc_ed25519_curve_info,
    .expected_err_code    = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name   = "ed25519 invalid input",
    .p_input              = "bf82",
    .p_qx                 = eddsa_ed25519_qx_test3,
    .p_qy                 = eddsa_ed25519_qy_test3,
    .p_r                  = eddsa_ed25519_r_test3,
    .p_s                  = eddsa_ed25519_s_test3
};

// EdDSA verify - RFC 8032 Edwards-Curve Digital Signature Algorithm (EdDSA) - Test 3
NRF_SECTION_ITEM_REGISTER(test_vector_eddsa_verify_data, test_vector_ecdsa_verify_t test_vector_eddsa_verify_ed25519_invalid_public_key) =
{
    .p_curve_info         = &g_nrf_crypto_ecc_ed25519_curve_info,
    .expected_err_code    = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name   = "ed25519 invalid public_key",
    .p_input              = eddsa_ed25519_input_test3,
    .p_qx                 = "0c51cd8e6218a1a38da47ed00230f058",
    .p_qy                 = eddsa_ed25519_qy_test3,
    .p_r                  = eddsa_ed25519_r_test3,
    .p_s                  = eddsa_ed25519_s_test3
};

// EdDSA verify - RFC 8032 Edwards-Curve Digital Signature Algorithm (EdDSA) - Test 3
NRF_SECTION_ITEM_REGISTER(test_vector_eddsa_verify_data, test_vector_ecdsa_verify_t test_vector_eddsa_verify_ed25519_invalid_signature) =
{
    .p_curve_info         = &g_nrf_crypto_ecc_ed25519_curve_info,
    .expected_err_code    = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name   = "ed25519 invalid signature",
    .p_input              = eddsa_ed25519_input_test3,
    .p_qx                 = eddsa_ed25519_qy_test3,
    .p_qy                 = eddsa_ed25519_qy_test3,
    .p_r                  = "7291d657deec24024827e69c3abe01a30ce548a284743a445e3680d7db5ac3ac",
    .p_s                  = eddsa_ed25519_s_test3
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_ED25519)

/*lint -restore */

#endif // NRF_CRYPTO_ECC_ENABLED
