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

/*lint -save -e91 */

#if NRF_CRYPTO_ECC_ENABLED && !NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_CC310_BL) && !NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_OPTIGA)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP160R1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP160R1)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP160R2)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP160R2)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP192R1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP192R1)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP224R1)

// ECDSA sign - NIST CAVS 11.2 P-224, SHA-256 - first test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp224r1_sha256_1) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_SUCCESS,
    .p_test_vector_name         = "secp224r1 valid sha256 1",
    .p_input                    = "00c6fc53c1986d19a8a8b580ee553dc1240745d760647d1c0adf442c133c7f56",
    .p_qx                       = "4c741e4d20103670b7161ae72271082155838418084335338ac38fa4",
    .p_qy                       = "db7919151ac28587b72bad7ab180ec8e95ab9e2c8d81d9b9d7e2e383",
    .p_x                        = "888fc992893bdd8aa02c80768832605d020b81ae0b25474154ec89aa"
};

// ECDSA sign - NIST CAVS 11.2 P-224, SHA-256 - second test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp224r1_sha256_2) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_SUCCESS,
    .p_test_vector_name         = "secp224r1 valid sha256 2",
    .p_input                    = "fb5dd3b8d280fe7c4838f01b2a5c28493ed3084f46b40642600ba39e43fbff7b",
    .p_qx                       = "897089f4ef05b943eeac06589f0e09ccc571a6add3eb1610a2fc830f",
    .p_qy                       = "62ba3f6b3e6f0f062058b93e6f25b6041246c5be13584a41cae7e244",
    .p_x                        = "5b5a3e186e7d5b9b0fbdfc74a05e0a3d85dc4be4c87269190c839972"
};

// ECDSA sign - NIST CAVS 11.2 P-224, SHA-256 - third test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp224r1_sha256_3) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_SUCCESS,
    .p_test_vector_name         = "secp224r1 valid sha256 3",
    .p_input                    = "f4083aebe08c9bdb8c08ff844ffc207f80fa4406fb73bdbc1c6020f71281bdae",
    .p_qx                       = "40a4ab1e6a9f84b4dedb81795e6a7124d1cfdfd7ec64c5d4b9e32666",
    .p_qy                       = "83aa32a3c2fc068e62626f2dafce5d7f050e826e5c145cd2d13d1b27",
    .p_x                        = "f60b3a4d4e31c7005a3d2d0f91cb096d016a8ddb5ab10ecb2a549170"
};

// ECDSA sign - NIST CAVS 11.2 P-224, SHA-512 - first test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp224r1_sha512_1) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_SUCCESS,
    .p_test_vector_name         = "secp224r1 valid sha512 1",
    .p_input                    = "3edbb59a32b2464291d0a96023a798c1fc6cb5ff4fcecfadcfac2be00c26fa27181aef76c96d8269aeaf2275eeacbb777abbd9571de9279edc5695a3345cad9b",
    .p_qx                       = "ac635fe00e8b7a3c8ef5655bdfb7f83e8532e59c0cc0b6534d810ffa",
    .p_qy                       = "1d067aebeba66e79b28ecfe59ac6fdf5e1970dc3a84499c9d90cd8e2",
    .p_x                        = "ba5374541c13597bded6880849184a593d69d3d4f0b1cb4d0919cbd6"
};

// ECDSA sign - NIST CAVS 11.2 P-224, SHA-512 - second test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp224r1_sha512_2) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_SUCCESS,
    .p_test_vector_name         = "secp224r1 valid sha512 2",
    .p_input                    = "825ab979af5c263d9f074a2d771d1d1cdfa435e7938245a3c9ee30cb77ee8c1475051d2f09d7d11d920a6c754bfd253903131c491994679cafdb8cfbf32b763d",
    .p_qx                       = "ecaea8ceea55c3bd418fd34a4ff2499e25e66a104eed846bc00c31d2",
    .p_qy                       = "3933a356ab1f2dabc303ff0a5d076131e77032e6f502336883bf78a7",
    .p_x                        = "1e27187134d0a63542adf4665fba22f00cfc7b0a1e02effe913ceedc"
};

// ECDSA sign - NIST CAVS 11.2 P-224, SHA-512 - third test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp224r1_sha512_3) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_SUCCESS,
    .p_test_vector_name         = "secp224r1 valid sha512 3",
    .p_input                    = "d0db7c20c201cd8c63ca777293543750d7f6a9e375b056e74cfe9fb2c95b2cc9807d8a9607a5b0fad6eeda86e4f73ace139e77a5356181b8cbef3f88173253b6",
    .p_qx                       = "d656b73b131aa4c6336a57849ce0d3682b6ab2113d013711e8c29762",
    .p_qy                       = "6328335ffc2029afbfe2a15cc5636978778c3f9dab84840b05f2e705",
    .p_x                        = "0905b40e6c29bfcbf55e04266f68f10ca8d3905001d68bb61a27749b"
};

// ECDSA sign - NIST CAVS 11.2 P-224, SHA-256 - invalid Q test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp224r1_sha256_inv_q) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name         = "secp224r1 Q changed sha256",
    .p_input                    = "00c6fc53c1986d19a8a8b580ee553dc1240745d760647d1c0adf442c133c7f56",
    .p_qx                       = "897089f4ef05b943eeac06589f0e09ccc571a6add3eb1610a2fc830f",
    .p_qy                       = "62ba3f6b3e6f0f062058b93e6f25b6041246c5be13584a41cae7e244",
    .p_x                        = "888fc992893bdd8aa02c80768832605d020b81ae0b25474154ec89aa"
};

// ECDSA sign - NIST CAVS 11.2 P-224, SHA-256 - invalid X test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp224r1_sha256_inv_x) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name         = "secp224r1 X changed sha256",
    .p_input                    = "00c6fc53c1986d19a8a8b580ee553dc1240745d760647d1c0adf442c133c7f56",
    .p_qx                       = "4c741e4d20103670b7161ae72271082155838418084335338ac38fa4",
    .p_qy                       = "db7919151ac28587b72bad7ab180ec8e95ab9e2c8d81d9b9d7e2e383",
    .p_x                        = "5b5a3e186e7d5b9b0fbdfc74a05e0a3d85dc4be4c87269190c839972"
};

// ECDSA sign - NIST CAVS 11.2 P-224, SHA-512 - invalid Q test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp224r1_sha512_inv_q) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name         = "secp224r1 Q changed sha512",
    .p_input                    = "3edbb59a32b2464291d0a96023a798c1fc6cb5ff4fcecfadcfac2be00c26fa27181aef76c96d8269aeaf2275eeacbb777abbd9571de9279edc5695a3345cad9b",
    .p_qx                       = "ecaea8ceea55c3bd418fd34a4ff2499e25e66a104eed846bc00c31d2",
    .p_qy                       = "3933a356ab1f2dabc303ff0a5d076131e77032e6f502336883bf78a7",
    .p_x                        = "ba5374541c13597bded6880849184a593d69d3d4f0b1cb4d0919cbd6"
};

// ECDSA sign - NIST CAVS 11.2 P-224, SHA-512 - invalid X test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp224r1_sha512_inv_x) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp224r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name         = "secp224r1 X changed sha512",
    .p_input                    = "3edbb59a32b2464291d0a96023a798c1fc6cb5ff4fcecfadcfac2be00c26fa27181aef76c96d8269aeaf2275eeacbb777abbd9571de9279edc5695a3345cad9b",
    .p_qx                       = "ac635fe00e8b7a3c8ef5655bdfb7f83e8532e59c0cc0b6534d810ffa",
    .p_qy                       = "1d067aebeba66e79b28ecfe59ac6fdf5e1970dc3a84499c9d90cd8e2",
    .p_x                        = "1e27187134d0a63542adf4665fba22f00cfc7b0a1e02effe913ceedc"
};
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP224R1)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP256R1)

// ECDSA sign - NIST CAVS 11.2 P-256, SHA-256 - first test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp256r1_sha256_1) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_SUCCESS,
    .p_test_vector_name         = "secp256r1 valid sha256 1",
    .p_input                    = "44acf6b7e36c1342c2c5897204fe09504e1e2efb1a900377dbc4e7a6a133ec56",
    .p_qx                       = "1ccbe91c075fc7f4f033bfa248db8fccd3565de94bbfb12f3c59ff46c271bf83",
    .p_qy                       = "ce4014c68811f9a21a1fdb2c0e6113e06db7ca93b7404e78dc7ccd5ca89a4ca9",
    .p_x                        = "519b423d715f8b581f4fa8ee59f4771a5b44c8130b4e3eacca54a56dda72b464"
};

// ECDSA sign - NIST CAVS 11.2 P-256, SHA-256 - second test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp256r1_sha256_2) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_SUCCESS,
    .p_test_vector_name         = "secp256r1 valid sha256 2",
    .p_input                    = "9b2db89cb0e8fa3cc7608b4d6cc1dec0114e0b9ff4080bea12b134f489ab2bbc",
    .p_qx                       = "e266ddfdc12668db30d4ca3e8f7749432c416044f2d2b8c10bf3d4012aeffa8a",
    .p_qy                       = "bfa86404a2e9ffe67d47c587ef7a97a7f456b863b4d02cfc6928973ab5b1cb39",
    .p_x                        = "0f56db78ca460b055c500064824bed999a25aaf48ebb519ac201537b85479813"
};

// ECDSA sign - NIST CAVS 11.2 P-256, SHA-256 - third test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp256r1_sha256_3) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_SUCCESS,
    .p_test_vector_name         = "secp256r1 valid sha256 3",
    .p_input                    = "b804cf88af0c2eff8bbbfb3660ebb3294138e9d3ebd458884e19818061dacff0",
    .p_qx                       = "74ccd8a62fba0e667c50929a53f78c21b8ff0c3c737b0b40b1750b2302b0bde8",
    .p_qy                       = "29074e21f3a0ef88b9efdf10d06aa4c295cc1671f758ca0e4cd108803d0f2614",
    .p_x                        = "e283871239837e13b95f789e6e1af63bf61c918c992e62bca040d64cad1fc2ef"
};

// ECDSA sign - NIST CAVS 11.2 P-256, SHA-512 - first test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp256r1_sha512_1) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_SUCCESS,
    .p_test_vector_name         = "secp256r1 valid sha512 1",
    .p_input                    = "a59ca4dd2b0347f4f2702a8962878a206775fd91047040be60463119f02aa829b7360b940b2785395406c280375c5d90ee655e51d4120df256b9a6287161c7fc",
    .p_qx                       = "6b738de3398b6ac57b9591f9d7985dd4f32137ad3460dcf8970c1390cb9eaf8d",
    .p_qy                       = "83bc61e26d2bbbd3cf2d2ab445a2bc4ab5dde41f4a13078fd1d3cc36ab596d57",
    .p_x                        = "9dd0d3a3d514c2a8adb162b81e3adfba3299309f7d2018f607bdb15b1a25f499"
};

// ECDSA sign - NIST CAVS 11.2 P-256, SHA-512 - second test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp256r1_sha512_2) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_SUCCESS,
    .p_test_vector_name         = "secp256r1 valid sha512 2",
    .p_input                    = "9e359350e87e7573ad9894cd4aad6c6202a58e9938d098dbf65650fc6f04fce3664b9adb234bfa0821788223a306daaa3e62bd46b19d7eb7a725bc5bce8998f3",
    .p_qx                       = "f2a6674d4e86152a527199bed293fa63acde1b4d8a92b62e552210ba45c38792",
    .p_qy                       = "c72565c24f0eee6a094af341ddd8579747b865f91c8ed5b44cda8a19cc93776f",
    .p_x                        = "f9bf909b7973bf0e3dad0e43dcb2d7fa8bda49dbe6e5357f8f0e2bd119be30e6"
};

// ECDSA sign - NIST CAVS 11.2 P-256, SHA-512 - third test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp256r1_sha512_3) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_SUCCESS,
    .p_test_vector_name         = "secp256r1 valid sha512 3",
    .p_input                    = "ff5e80ccbb51b75742a1f0e632b4c6cd119692f2aca337378f7eb2f3b17fc3d912828b7e1655d2263d8757715eea31493aa89dfe1db143a8fa13f89a00379938",
    .p_qx                       = "70b877b5e365fcf08140b1eca119baba662879f38e059d074a2cb60b03ea5d39",
    .p_qy                       = "5f56f94d591df40b9f3b8763ac4b3dbe622c956d5bd0c55658b6f46fa3deb201",
    .p_x                        = "724567d21ef682dfc6dc4d46853880cfa86fe6fea0efd51fac456f03c3d36ead"
};

// ECDSA sign - NIST CAVS 11.2 P-256, SHA-256 - invalid Q test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp256r1_sha256_inv_q) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name         = "secp256r1 Q changed sha256",
    .p_input                    = "44acf6b7e36c1342c2c5897204fe09504e1e2efb1a900377dbc4e7a6a133ec56",
    .p_qx                       = "e266ddfdc12668db30d4ca3e8f7749432c416044f2d2b8c10bf3d4012aeffa8a",
    .p_qy                       = "bfa86404a2e9ffe67d47c587ef7a97a7f456b863b4d02cfc6928973ab5b1cb39",
    .p_x                        = "519b423d715f8b581f4fa8ee59f4771a5b44c8130b4e3eacca54a56dda72b464"
};

// ECDSA sign - NIST CAVS 11.2 P-256, SHA-512 - invalid Q test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp256r1_sha512_inv_q) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name         = "secp256r1 Q changed sha512",
    .p_input                    = "a59ca4dd2b0347f4f2702a8962878a206775fd91047040be60463119f02aa829b7360b940b2785395406c280375c5d90ee655e51d4120df256b9a6287161c7fc",
    .p_qx                       = "f2a6674d4e86152a527199bed293fa63acde1b4d8a92b62e552210ba45c38792",
    .p_qy                       = "c72565c24f0eee6a094af341ddd8579747b865f91c8ed5b44cda8a19cc93776f",
    .p_x                        = "9dd0d3a3d514c2a8adb162b81e3adfba3299309f7d2018f607bdb15b1a25f499"
};

// ECDSA sign - NIST CAVS 11.2 P-256, SHA-256 - invalid X test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp256r1_sha256_inv_x) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name         = "secp256r1 X changed sha256",
    .p_input                    = "44acf6b7e36c1342c2c5897204fe09504e1e2efb1a900377dbc4e7a6a133ec56",
    .p_qx                       = "1ccbe91c075fc7f4f033bfa248db8fccd3565de94bbfb12f3c59ff46c271bf83",
    .p_qy                       = "ce4014c68811f9a21a1fdb2c0e6113e06db7ca93b7404e78dc7ccd5ca89a4ca9",
    .p_x                        = "0f56db78ca460b055c500064824bed999a25aaf48ebb519ac201537b85479813"
};

// ECDSA sign - NIST CAVS 11.2 P-256, SHA-512 - invalid X test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp256r1_sha512_inv_x) =
{
    .p_curve_info               = &g_nrf_crypto_ecc_secp256r1_curve_info,
    .expected_sign_err_code     = NRF_SUCCESS,
    .expected_verify_err_code   = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name         = "secp256r1 X changed sha512",
    .p_input                    = "a59ca4dd2b0347f4f2702a8962878a206775fd91047040be60463119f02aa829b7360b940b2785395406c280375c5d90ee655e51d4120df256b9a6287161c7fc",
    .p_qx                       = "6b738de3398b6ac57b9591f9d7985dd4f32137ad3460dcf8970c1390cb9eaf8d",
    .p_qy                       = "83bc61e26d2bbbd3cf2d2ab445a2bc4ab5dde41f4a13078fd1d3cc36ab596d57",
    .p_x                        = "f9bf909b7973bf0e3dad0e43dcb2d7fa8bda49dbe6e5357f8f0e2bd119be30e6"
};
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP256R1)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP384R1)

// ECDSA sign - NIST CAVS 11.2 P-384, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp384r1_sha256_1) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_SUCCESS,
    .p_test_vector_name        = "secp384r1 valid sha256 1",
    .p_input                   = "bbbd0a5f645d3fda10e288d172b299455f9dff00e0fbc2833e18cd017d7f3ed1",
    .p_qx                      = "0400193b21f07cd059826e9453d3e96dd145041c97d49ff6b7047f86bb0b0439e909274cb9c282bfab88674c0765bc75",
    .p_qy                      = "f70d89c52acbc70468d2c5ae75c76d7f69b76af62dcf95e99eba5dd11adf8f42ec9a425b0c5ec98e2f234a926b82a147",
    .p_x                       = "c602bc74a34592c311a6569661e0832c84f7207274676cc42a89f058162630184b52f0d99b855a7783c987476d7f9e6b"
};

// ECDSA sign - NIST CAVS 11.2 P-384, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp384r1_sha256_2) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_SUCCESS,
    .p_test_vector_name        = "secp384r1 valid sha256 2",
    .p_input                   = "e4cb4f2f94793f4a471a1e1de49b2c6ceb5825c5d02c69a7fe122eebda7b28b9",
    .p_qx                      = "b36418a3014074ec9bbcc6a4b2367a4fb464cca7ec0a324cb68670d5c5e03e7a7eb07da117c5ea50b665ab62bd02a491",
    .p_qy                      = "4ea299c30e7d76e2c5905babada2d3bb4ee5eb35a5a23605cdb0d5133471a53eb9e6758e49105a4eaf29d2267ba84ef2",
    .p_x                       = "0287f62a5aa8432ff5e95618ec8f9ccaa870dde99c30b51b7673378efe4ccac598f4bbebbfd8993f9abb747b6ad638b9"
};

// ECDSA sign - NIST CAVS 11.2 P-384, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp384r1_sha256_3) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_SUCCESS,
    .p_test_vector_name        = "secp384r1 valid sha256 3",
    .p_input                   = "a22a3d3da784a68d37ae5aaca8ef28dc1ceefc8e530431e455dd2bb2ed969684",
    .p_qx                      = "c703835d723c85c643260379d8445b0c816fe9534351921e14a8e147fe140ec7b0c4d704f8dc66a232b2333b28f03dee",
    .p_qy                      = "c5d0bb054053fd86c26f147c4966757aa04b00513a02d427b8d06c16055c607955efdc518d338abfe7927c195dc28588",
    .p_x                       = "d44d3108873977036c9b97e03f914cba2f5775b68c425d550995574081191da764acc50196f6d2508082a150af5cd41f"
};

// ECDSA sign - NIST CAVS 11.2 P-384, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp384r1_sha512_1) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_SUCCESS,
    .p_test_vector_name        = "secp384r1 valid sha512 1",
    .p_input                   = "f863cf3749ae5256da0ceb2e6d391fcce939b1490b024527687b1a2908da35c48b44255d82956c76d70672c41c6456d78c57342e932490083f73016b560a0245",
    .p_qx                      = "fb937e4a303617b71b6c1a25f2ac786087328a3e26bdef55e52d46ab5e69e5411bf9fc55f5df9994d2bf82e8f39a153e",
    .p_qy                      = "a97d9075e92fa5bfe67e6ec18e21cc4d11fde59a68aef72c0e46a28f31a9d60385f41f39da468f4e6c3d3fbac9046765",
    .p_x                       = "217afba406d8ab32ee07b0f27eef789fc201d121ffab76c8fbe3c2d352c594909abe591c6f86233992362c9d631baf7c"
};

// ECDSA sign - NIST CAVS 11.2 P-384, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp384r1_sha512_2) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_SUCCESS,
    .p_test_vector_name        = "secp384r1 valid sha512 2",
    .p_input                   = "68944ca920620e1d42184d264c4ffe295882f00079f9daaaa0efc305fe10cc7bbc667985c148eacb51ee1c05a6d8a861e98371a045e11e18e200ad52d5b4cb13",
    .p_qx                      = "832cbb7061a719a316e73dbad348fa67cd17c33f40b9000a3d3b691a2a2cd821052566717c3ead01089b56086af1366f",
    .p_qy                      = "1e15a048d1dce642d9ebcbfac7f92b1bcee90fd0240cc79abd29e32e0e655c4ee1fd34fb88178bba92aca100e7794ed0",
    .p_x                       = "0a3f45a28a355381a919372f60320d6610cfb69c3e318eb1607db3cadfc42b728b77a6a9e9e333de9183c58933daf60f"
};

// ECDSA sign - NIST CAVS 11.2 P-384, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp384r1_sha512_3) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_SUCCESS,
    .p_test_vector_name        = "secp384r1 valid sha512 3",
    .p_input                   = "8876ecd423fd0e903364fac777897b80b31c03f0f1ef1678372a6cb240ab701e49b51b798fd8e588ab39db7d3c77879901916babdd0ef0632cc16d5ccb9d1535",
    .p_qx                      = "a2b24a5ad4a2e91f12199ed7699e3f297e27bf8b8ea8fbe7ed28366f3544cd8e680c238450f8a6422b40829d6647b25c",
    .p_qy                      = "2732be0075536e6519f6a099b975a40f8e0de337fa4d48bd0762b43f41cab8deafdef9cfbb9973e457801e3bf9c93304",
    .p_x                       = "2e408c57921939f0e0fe2e80ce74a4fa4a1b4fa7ab070206298fe894d655be50e2583af9e45544b5d69c73dce8a2c8e7"
};

// ECDSA sign - NIST CAVS 11.2 P-384, SHA-256 - invalid Q test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp384r1_sha256_inv_q) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp384r1 Q changed sha256",
    .p_input                   = "bbbd0a5f645d3fda10e288d172b299455f9dff00e0fbc2833e18cd017d7f3ed1",
    .p_qx                      = "b36418a3014074ec9bbcc6a4b2367a4fb464cca7ec0a324cb68670d5c5e03e7a7eb07da117c5ea50b665ab62bd02a491",
    .p_qy                      = "4ea299c30e7d76e2c5905babada2d3bb4ee5eb35a5a23605cdb0d5133471a53eb9e6758e49105a4eaf29d2267ba84ef2",
    .p_x                       = "c602bc74a34592c311a6569661e0832c84f7207274676cc42a89f058162630184b52f0d99b855a7783c987476d7f9e6b"
};

// ECDSA sign - NIST CAVS 11.2 P-384, SHA-256 - invalid X test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp384r1_sha256_inv_x) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp384r1 X changed sha256",
    .p_input                   = "bbbd0a5f645d3fda10e288d172b299455f9dff00e0fbc2833e18cd017d7f3ed1",
    .p_qx                      = "0400193b21f07cd059826e9453d3e96dd145041c97d49ff6b7047f86bb0b0439e909274cb9c282bfab88674c0765bc75",
    .p_qy                      = "f70d89c52acbc70468d2c5ae75c76d7f69b76af62dcf95e99eba5dd11adf8f42ec9a425b0c5ec98e2f234a926b82a147",
    .p_x                       = "0287f62a5aa8432ff5e95618ec8f9ccaa870dde99c30b51b7673378efe4ccac598f4bbebbfd8993f9abb747b6ad638b9"
};

// ECDSA sign - NIST CAVS 11.2 P-384, SHA-512 - invalid Q test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp384r1_sha512_inv_q) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp384r1 Q changed sha512",
    .p_input                   = "f863cf3749ae5256da0ceb2e6d391fcce939b1490b024527687b1a2908da35c48b44255d82956c76d70672c41c6456d78c57342e932490083f73016b560a0245",
    .p_qx                      = "832cbb7061a719a316e73dbad348fa67cd17c33f40b9000a3d3b691a2a2cd821052566717c3ead01089b56086af1366f",
    .p_qy                      = "1e15a048d1dce642d9ebcbfac7f92b1bcee90fd0240cc79abd29e32e0e655c4ee1fd34fb88178bba92aca100e7794ed0",
    .p_x                       = "217afba406d8ab32ee07b0f27eef789fc201d121ffab76c8fbe3c2d352c594909abe591c6f86233992362c9d631baf7c"
};

// ECDSA sign - NIST CAVS 11.2 P-384, SHA-512 - invalid X test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp384r1_sha512_inv_x) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp384r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp384r1 X changed sha512",
    .p_input                   = "f863cf3749ae5256da0ceb2e6d391fcce939b1490b024527687b1a2908da35c48b44255d82956c76d70672c41c6456d78c57342e932490083f73016b560a0245",
    .p_qx                      = "fb937e4a303617b71b6c1a25f2ac786087328a3e26bdef55e52d46ab5e69e5411bf9fc55f5df9994d2bf82e8f39a153e",
    .p_qy                      = "a97d9075e92fa5bfe67e6ec18e21cc4d11fde59a68aef72c0e46a28f31a9d60385f41f39da468f4e6c3d3fbac9046765",
    .p_x                       = "0a3f45a28a355381a919372f60320d6610cfb69c3e318eb1607db3cadfc42b728b77a6a9e9e333de9183c58933daf60f"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP384R1)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP521R1)

// ECDSA sign - NIST CAVS 11.2 P-521, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp521r1_sha256_1) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_SUCCESS,
    .p_test_vector_name        = "secp521r1 valid sha256 1",
    .p_input                   = "53e6537cb6ea68ae47a81611c22756d770d7a37e336c3af0b0814b04fa39434b",
    .p_qx                      = "007d042ca19408524e68b981f1419351e3b84736c77fe58fee7d11317df2e850d960c7dd10d10ba714c8a609d163502b79d682e8bbecd4f52591d2748533e45a867a",
    .p_qy                      = "0197ac6416111ccf987d290459ebc8ad9ec56e49059c992155539a36a626631f4a2d89164b985154f2dddc0281ee5b5178271f3a76a0914c3fcd1f97be8e8376efb3",
    .p_x                       = "01e8c05996b85e6f3f875712a09c1b40672b5e7a78d5852de01585c5fb990bf3812c3245534a714389ae9014d677a449efd658254e610da8e6cad33414b9d33e0d7a"
};

// ECDSA sign - NIST CAVS 11.2 P-521, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp521r1_sha256_2) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_SUCCESS,
    .p_test_vector_name        = "secp521r1 valid sha256 2",
    .p_input                   = "40aef13bb7192a564d72ba58f7efad15635248eca49619b4182bf6f979842d6f",
    .p_qx                      = "010038bb9a7aea626de68c14c64243150e72c69e2f8a1ab922bfbdaa6f33d24fb4542c0324357b0dd640bbcd07632ecd253f64ca2bfbfbf3de9b24fffd0568ab82da",
    .p_qy                      = "00faf867d95308cc36d6f46844a0f535dc70f9768eed011a2464d2f308fa1d8e72c3616aec7e70516908183ffce7fdd36984a15f73efaa3858c2edf16a784d40e6c2",
    .p_x                       = "00b65bf33b2f27d52cbfabcadce741e691bf4762089afd37964de1a0deda98331bf8c74020a14b52d44d26e2f6fa7bcddbe83be7db17a0c8a1b376469cf92c6da27c"
};

// ECDSA sign - NIST CAVS 11.2 P-521, SHA-256
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp521r1_sha256_3) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_SUCCESS,
    .p_test_vector_name        = "secp521r1 valid sha256 3",
    .p_input                   = "6fd829bd5fc68d7a36436284c1282e3fae76ba44c5babcee423142c1cf2b52c4",
    .p_qx                      = "00fb3868238ca840dbb36ecc6cf04f5f773ea0ab8e8b0fdcf779dc4039a8d7146a417504e953c0cb5e7f4e599cc2c168deda8b7f16084b5582f89f2ece4cae5167f7",
    .p_qy                      = "01f90b5c15eeda48e747cf3ee8183166a49dbfac6161cbd09d29d40a6854f4c495e88a435892a920cdaad20d41985890b648badd4f0a858ffcbd9afdfc23134ede18",
    .p_x                       = "002c4e660609e99becd61c14d043e8b419a663010cc1d8f9469897d7d0a4f076a619a7214a2a9d07957b028f7d8539ba7430d0b9a7de08beeeae8452d7bb0eac669d"
};

// ECDSA sign - NIST CAVS 11.2 P-521, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp521r1_sha512_1) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_SUCCESS,
    .p_test_vector_name        = "secp521r1 valid sha512 1",
    .p_input                   = "65f83408092261bda599389df03382c5be01a81fe00a36f3f4bb6541263f801627c440e50809712b0cace7c217e6e5051af81de9bfec3204dcd63c4f9a741047",
    .p_qx                      = "0061387fd6b95914e885f912edfbb5fb274655027f216c4091ca83e19336740fd81aedfe047f51b42bdf68161121013e0d55b117a14e4303f926c8debb77a7fdaad1",
    .p_qy                      = "00e7d0c75c38626e895ca21526b9f9fdf84dcecb93f2b233390550d2b1463b7ee3f58df7346435ff0434199583c97c665a97f12f706f2357da4b40288def888e59e6",
    .p_x                       = "00f749d32704bc533ca82cef0acf103d8f4fba67f08d2678e515ed7db886267ffaf02fab0080dca2359b72f574ccc29a0f218c8655c0cccf9fee6c5e567aa14cb926"
};

// ECDSA sign - NIST CAVS 11.2 P-521, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp521r1_sha512_2) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_SUCCESS,
    .p_test_vector_name        = "secp521r1 valid sha512 2",
    .p_input                   = "a6200971c6a289e2fcb80f78ec08a5079ea2675efd68bcab479552aa5bcb8edf3c993c79d7cebcc23c20e5af41723052b871134cc71d5c57206182a7068cc39b",
    .p_qx                      = "004d5c8afee038984d2ea96681ec0dccb6b52dfa4ee2e2a77a23c8cf43ef19905a34d6f5d8c5cf0981ed804d89d175b17d1a63522ceb1e785c0f5a1d2f3d15e51352",
    .p_qy                      = "0014368b8e746807b2b68f3615cd78d761a464ddd7918fc8df51d225962fdf1e3dc243e265100ff0ec133359e332e44dd49afd8e5f38fe86133573432d33c02fa0a3",
    .p_x                       = "01a4d2623a7d59c55f408331ba8d1523b94d6bf8ac83375ceb57a2b395a5bcf977cfc16234d4a97d6f6ee25a99aa5bff15ff535891bcb7ae849a583e01ac49e0e9b6"
};

// ECDSA sign - NIST CAVS 11.2 P-521, SHA-512
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp521r1_sha512_3) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_SUCCESS,
    .p_test_vector_name        = "secp521r1 valid sha512 3",
    .p_input                   = "46ff533622cc90321a3aeb077ec4db4fbf372c7a9db48b59de7c5d59e6314110676ba5491bd20d0f02774eef96fc2e88ca99857d21ef255184c93fb1ff4f01d3",
    .p_qx                      = "00c2d540a7557f4530de35bbd94da8a6defbff783f54a65292f8f76341c996cea38795805a1b97174a9147a8644282e0d7040a6f83423ef2a0453248156393a1782e",
    .p_qy                      = "0119f746c5df8cec24e4849ac1870d0d8594c799d2ceb6c3bdf891dfbd2242e7ea24d6aec3166214734acc4cbf4da8f71e2429c5c187b2b3a048527c861f58a9b97f",
    .p_x                       = "014787f95fb1057a2f3867b8407e54abb91740c097dac5024be92d5d65666bb16e4879f3d3904d6eab269cf5e7b632ab3c5f342108d1d4230c30165fba3a1bf1c66f"
};

// ECDSA sign - NIST CAVS 11.2 P-521, SHA-256 - invalid Q test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp521r1_sha256_inv_q) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp521r1 Q changed sha256",
    .p_input                   = "53e6537cb6ea68ae47a81611c22756d770d7a37e336c3af0b0814b04fa39434b",
    .p_qx                      = "004d5c8afee038984d2ea96681ec0dccb6b52dfa4ee2e2a77a23c8cf43ef19905a34d6f5d8c5cf0981ed804d89d175b17d1a63522ceb1e785c0f5a1d2f3d15e51352",
    .p_qy                      = "0014368b8e746807b2b68f3615cd78d761a464ddd7918fc8df51d225962fdf1e3dc243e265100ff0ec133359e332e44dd49afd8e5f38fe86133573432d33c02fa0a3",
    .p_x                       = "01e8c05996b85e6f3f875712a09c1b40672b5e7a78d5852de01585c5fb990bf3812c3245534a714389ae9014d677a449efd658254e610da8e6cad33414b9d33e0d7a"
};

// ECDSA sign - NIST CAVS 11.2 P-521, SHA-512 - invalid Q test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp521r1_sha512_inv_q) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp521r1 Q changed sha512",
    .p_input                   = "65f83408092261bda599389df03382c5be01a81fe00a36f3f4bb6541263f801627c440e50809712b0cace7c217e6e5051af81de9bfec3204dcd63c4f9a741047",
    .p_qx                      = "004d5c8afee038984d2ea96681ec0dccb6b52dfa4ee2e2a77a23c8cf43ef19905a34d6f5d8c5cf0981ed804d89d175b17d1a63522ceb1e785c0f5a1d2f3d15e51352",
    .p_qy                      = "0014368b8e746807b2b68f3615cd78d761a464ddd7918fc8df51d225962fdf1e3dc243e265100ff0ec133359e332e44dd49afd8e5f38fe86133573432d33c02fa0a3",
    .p_x                       = "00f749d32704bc533ca82cef0acf103d8f4fba67f08d2678e515ed7db886267ffaf02fab0080dca2359b72f574ccc29a0f218c8655c0cccf9fee6c5e567aa14cb926"
};

// ECDSA sign - NIST CAVS 11.2 P-521, SHA-256 - invalid X test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp521r1_sha256_inv_x) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp521r1 X changed sha256",
    .p_input                   = "53e6537cb6ea68ae47a81611c22756d770d7a37e336c3af0b0814b04fa39434b",
    .p_qx                      = "007d042ca19408524e68b981f1419351e3b84736c77fe58fee7d11317df2e850d960c7dd10d10ba714c8a609d163502b79d682e8bbecd4f52591d2748533e45a867a",
    .p_qy                      = "0197ac6416111ccf987d290459ebc8ad9ec56e49059c992155539a36a626631f4a2d89164b985154f2dddc0281ee5b5178271f3a76a0914c3fcd1f97be8e8376efb3",
    .p_x                       = "01a4d2623a7d59c55f408331ba8d1523b94d6bf8ac83375ceb57a2b395a5bcf977cfc16234d4a97d6f6ee25a99aa5bff15ff535891bcb7ae849a583e01ac49e0e9b6"
};

// ECDSA sign - NIST CAVS 11.2 P-521, SHA-512 - invalid X test case
NRF_SECTION_ITEM_REGISTER(test_vector_ecdsa_sign_data, test_vector_ecdsa_sign_t test_vector_ecdsa_sign_secp521r1_sha512_inv_x) =
{
    .p_curve_info              = &g_nrf_crypto_ecc_secp521r1_curve_info,
    .expected_sign_err_code    = NRF_SUCCESS,
    .expected_verify_err_code  = NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE,
    .p_test_vector_name        = "secp521r1 X changed sha512",
    .p_input                   = "65f83408092261bda599389df03382c5be01a81fe00a36f3f4bb6541263f801627c440e50809712b0cace7c217e6e5051af81de9bfec3204dcd63c4f9a741047",
    .p_qx                      = "0061387fd6b95914e885f912edfbb5fb274655027f216c4091ca83e19336740fd81aedfe047f51b42bdf68161121013e0d55b117a14e4303f926c8debb77a7fdaad1",
    .p_qy                      = "00e7d0c75c38626e895ca21526b9f9fdf84dcecb93f2b233390550d2b1463b7ee3f58df7346435ff0434199583c97c665a97f12f706f2357da4b40288def888e59e6",
    .p_x                       = "01a4d2623a7d59c55f408331ba8d1523b94d6bf8ac83375ceb57a2b395a5bcf977cfc16234d4a97d6f6ee25a99aa5bff15ff535891bcb7ae849a583e01ac49e0e9b6"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP521R1)

#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP160K1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP160K1)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP192K1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP192K1)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP224K1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP224K1)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP256K1)
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_ECC_SECP256K1)

#endif // NRF_CRYPTO_ECC_ENABLED && !NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_CC310_BL)

/*lint -restore */
