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

/**@brief HMAC test vectors can be found in RFC 4231 document and
 *  on NIST web pages.
 *
 * https://tools.ietf.org/html/rfc4231
 * https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Message-Authentication
 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_HMAC)

/*lint -save -e91 */

#if NRF_MODULE_ENABLED(NRF_CRYPTO_HMAC_SHA256)

// HMAC - NIST CAVS 11.0
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac256_inv_message) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_FAIL,
    .p_test_vector_name = "SHA256 invalid - message changed",
    .p_input            = "c1689c2591eaf3c9e66070f8a77954ffb81749f1b00346f9dfe0b2ee905dcc288baf4a92de3f4001dd9f44c468c3d07d6c6ee82faceafc97c2fc0fc0601719d2dcd0aa2aec92d1b0ae933c65eb06a03c9c935c2bad0459810241347ab87e9f11adb30415424c6c7f5f22a003b8ab8de54f6ded0e3ab9245fa79568451dfa258e",
    .p_key              = "9779d9120642797f1747025d5b22b7ac607cab08e1758f2f3a46c8be1e25c53b8c6a8f58ffefa176",
    .p_expected_output  = "769f00d3e6a6cc1fb426a14a4f76c6462e6149726e0dee0ec0cf97a16605ac8b"
};

// HMAC - NIST CAVS 11.0
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac256_inv_key) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_FAIL,
    .p_test_vector_name = "SHA256 invalid - key changed",
    .p_input            = "b1689c2591eaf3c9e66070f8a77954ffb81749f1b00346f9dfe0b2ee905dcc288baf4a92de3f4001dd9f44c468c3d07d6c6ee82faceafc97c2fc0fc0601719d2dcd0aa2aec92d1b0ae933c65eb06a03c9c935c2bad0459810241347ab87e9f11adb30415424c6c7f5f22a003b8ab8de54f6ded0e3ab9245fa79568451dfa258e",
    .p_key              = "a779d9120642797f1747025d5b22b7ac607cab08e1758f2f3a46c8be1e25c53b8c6a8f58ffefa176",
    .p_expected_output  = "769f00d3e6a6cc1fb426a14a4f76c6462e6149726e0dee0ec0cf97a16605ac8b"
};

// HMAC - NIST CAVS 11.0
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac256_inv_sign) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_FAIL,
    .p_test_vector_name = "SHA256 invalid - signature changed",
    .p_input            = "b1689c2591eaf3c9e66070f8a77954ffb81749f1b00346f9dfe0b2ee905dcc288baf4a92de3f4001dd9f44c468c3d07d6c6ee82faceafc97c2fc0fc0601719d2dcd0aa2aec92d1b0ae933c65eb06a03c9c935c2bad0459810241347ab87e9f11adb30415424c6c7f5f22a003b8ab8de54f6ded0e3ab9245fa79568451dfa258e",
    .p_key              = "9779d9120642797f1747025d5b22b7ac607cab08e1758f2f3a46c8be1e25c53b8c6a8f58ffefa176",
    .p_expected_output  = "869f00d3e6a6cc1fb426a14a4f76c6462e6149726e0dee0ec0cf97a16605ac8b"
};

// HMAC - Custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac256_min_key_min_message_0) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA256 key_len=1 message_len=1 zeros",
    .p_input            = "00",
    .p_key              = "00",
    .p_expected_output  = "6620b31f2924b8c01547745f41825d322336f83ebb13d723678789d554d8a3ef"
};

// HMAC - Custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac256_min_key_min_message_1) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA256 key_len=1 message_len=1 non-zeros",
    .p_input            = "34",
    .p_key              = "56",
    .p_expected_output  = "9e5770be94a15456f3009ea289aa38368dcf22db52710990327548310eece50e"
};

// HMAC - NIST CAVS 11.0
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac256_0) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA256 key_len=40 message_len=128",
    .p_input            = "b1689c2591eaf3c9e66070f8a77954ffb81749f1b00346f9dfe0b2ee905dcc288baf4a92de3f4001dd9f44c468c3d07d6c6ee82faceafc97c2fc0fc0601719d2dcd0aa2aec92d1b0ae933c65eb06a03c9c935c2bad0459810241347ab87e9f11adb30415424c6c7f5f22a003b8ab8de54f6ded0e3ab9245fa79568451dfa258e",
    .p_key              = "9779d9120642797f1747025d5b22b7ac607cab08e1758f2f3a46c8be1e25c53b8c6a8f58ffefa176",
    .p_expected_output  = "769f00d3e6a6cc1fb426a14a4f76c6462e6149726e0dee0ec0cf97a16605ac8b"
};

// HMAC - NIST CAVS 11.0
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac_sha256_1) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA256 key_len=45 message_len=128",
    .p_input            = "53cb09d0a788e4466d01588df6945d8728d9363f76cd012a10308dad562b6be093364892e8397a8d86f1d81a2096cfc8a1bbb26a1a75525ffebfcf16911dadd09e802aa8686acfd1e4524620254a6bca18dfa56e71417756e5a452fa9ae5aec5dc71591c11630e9defec49a4ecf85a14f60eb854657899972ea5bf6159cb9547",
    .p_key              = "b763263dc4fc62b227cd3f6b4e9e358c21ca036ce396ab9259c1bedd2f5cd90297dc703c336eca3e358a4d6dc5",
    .p_expected_output  = "737301dea93db6bcbadd7bf796693961317ca680b380416f12f466f06526b36b"
};

// HMAC - NIST CAVS 11.0
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac_sha256_2) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA256 key_len=64 message_len=128",
    .p_input            = "ed4f269a8851eb3154771516b27228155200778049b2dc1963f3ac32ba46ea1387cfbb9c39151a2cc406cdc13c3c9860a27eb0b7fe8a7201ad11552afd041e33f70e53d97c62f17194b66117028fa9071cc0e04bd92de4972cd54f719010a694e414d4977abed7ca6b90ba612df6c3d467cded85032598a48546804f9cf2ecfe",
    .p_key              = "992868504d2564c4fb47bcbd4ae482d8fb0e8e56d7b81864e61986a0e25682daeb5b50177c095edc9e971da95c3210c376e723365ac33d1b4f391817f4c35124",
    .p_expected_output  = "2f8321f416b9bb249f113b13fc12d70e1668dc332839c10daa5717896cb70ddf"
};

// HMAC - NIST CAVS 11.0
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac_sha256_3) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA256 key_len=74 message_len=128",
    .p_input            = "b9e944e0b42d0ff454f7f8aa24f00e9ee039058ce4094111e39731b6dc3ade2a4acec4cf9c5be078e4f10a72d3d685c1e5e4d5abd92cd07b64dff87f266f0853ddf1cd61d9c637a9b07ab0be32ecac119faf827218b17ad4541a27519477f76ed918089f54b63d0e1e5a92982979ac187764b5e989e066a61b1065340e9cd203",
    .p_key              = "81574323c973540719d192833ddb51f13a52dcbae294aebea51be5f6aa47f3571f5d97facdcf0c7befbe809f44bdc73963d8514e4fd559774bb96087ef8eda6e7c64275d6d96c42b4e4e",
    .p_expected_output  = "514bd18495f6de0e237054b8e3ba1a74c3fada4279ad6b8550f3a14712c528df"
};

// RFC 4231 HMAC-SHA Test Case 1
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac_sha256_rfc1) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA256 key_len=20 message_len=8",
    .p_input            = "4869205468657265",
    .p_key              = "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b",
    .p_expected_output  = "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7"
};

// RFC 4231 HMAC-SHA Test Case 2
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac_sha256_rfc2) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA256 key_len=4 message_len=28",
    .p_input            = "7768617420646f2079612077616e7420666f72206e6f7468696e673f",
    .p_key              = "4a656665",
    .p_expected_output  = "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843"
};

// RFC 4231 HMAC-SHA Test Case 3
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac_sha256_rfc3) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA256 key_len=20 message_len=50",
    .p_input            = "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd",
    .p_key              = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
    .p_expected_output  = "773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe"
};

// RFC 4231 HMAC-SHA Test Case 4
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac_sha256_rfc4) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA256 key_len=25 message_len=50",
    .p_input            = "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd",
    .p_key              = "0102030405060708090a0b0c0d0e0f10111213141516171819",
    .p_expected_output  = "82558a389a443c0ea4cc819899f2083a85f0faa3e578f8077a2e3ff46729665b"
};

// RFC 4231 HMAC-SHA Test Case 6
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac_sha256_rfc6) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA256 key_len=131 message_len=54",
    .p_input            = "54657374205573696e67204c6172676572205468616e20426c6f636b2d53697a65204b6579202d2048617368204b6579204669727374",
    .p_key              = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
    .p_expected_output  = "60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54"
};

// RFC 4231 HMAC-SHA Test Case 7
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac_sha256_rfc7) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha256_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA256 key_len=131 message_len=152",
    .p_input            = "5468697320697320612074657374207573696e672061206c6172676572207468616e20626c6f636b2d73697a65206b657920616e642061206c6172676572207468616e20626c6f636b2d73697a6520646174612e20546865206b6579206e6565647320746f20626520686173686564206265666f7265206265696e6720757365642062792074686520484d414320616c676f726974686d2e",
    .p_key              = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
    .p_expected_output  = "9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_HMAC_SHA256)
#if NRF_MODULE_ENABLED(NRF_CRYPTO_HMAC_SHA512)

// HMAC - NIST CAVS 11.0
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac512_inv_message) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha512_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_FAIL,
    .p_test_vector_name = "SHA512 invalid - message changed",
    .p_input            = "3423dff48b312be864cb3490641f793d2b9fb68a7763b8e298c86f42245e4540eb01ae4d2d4500370b1886f23ca2cf9701704cad5bd21ba87b811daf7a854ea24a56565ced425b35e40e1acbebe03603e35dcf4a100e57218408a1d8dbcc3b99296cfea931efe3ebd8f719a6d9a15487b9ad67eafedf15559ca42445b0f9b42e",
    .p_key              = "57c2eb677b5093b9e829ea4babb50bde55d0ad59fec34a618973802b2ad9b78e26b2045dda784df3ff90ae0f2cc51ce39cf54867320ac6f3ba2c6f0d72360480c96614ae66581f266c35fb79fd28774afd113fa5187eff9206d7cbe90dd8bf67c844e202",
    .p_expected_output  = "33c511e9bc2307c62758df61125a980ee64cefebd90931cb91c13742d4714c06de4003faf3c41c06aefc638ad47b21906e6b104816b72de6269e045a1f4429d4"
};

// HMAC - NIST CAVS 11.0
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac512_inv_key) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha512_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_FAIL,
    .p_test_vector_name = "SHA512 invalid - key changed",
    .p_input            = "2423dff48b312be864cb3490641f793d2b9fb68a7763b8e298c86f42245e4540eb01ae4d2d4500370b1886f23ca2cf9701704cad5bd21ba87b811daf7a854ea24a56565ced425b35e40e1acbebe03603e35dcf4a100e57218408a1d8dbcc3b99296cfea931efe3ebd8f719a6d9a15487b9ad67eafedf15559ca42445b0f9b42e",
    .p_key              = "67c2eb677b5093b9e829ea4babb50bde55d0ad59fec34a618973802b2ad9b78e26b2045dda784df3ff90ae0f2cc51ce39cf54867320ac6f3ba2c6f0d72360480c96614ae66581f266c35fb79fd28774afd113fa5187eff9206d7cbe90dd8bf67c844e202",
    .p_expected_output  = "33c511e9bc2307c62758df61125a980ee64cefebd90931cb91c13742d4714c06de4003faf3c41c06aefc638ad47b21906e6b104816b72de6269e045a1f4429d4"
};

// HMAC - NIST CAVS 11.0
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac512_inv_sign) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha512_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_FAIL,
    .p_test_vector_name = "SHA512 invalid - signature changed",
    .p_input            = "2423dff48b312be864cb3490641f793d2b9fb68a7763b8e298c86f42245e4540eb01ae4d2d4500370b1886f23ca2cf9701704cad5bd21ba87b811daf7a854ea24a56565ced425b35e40e1acbebe03603e35dcf4a100e57218408a1d8dbcc3b99296cfea931efe3ebd8f719a6d9a15487b9ad67eafedf15559ca42445b0f9b42e",
    .p_key              = "57c2eb677b5093b9e829ea4babb50bde55d0ad59fec34a618973802b2ad9b78e26b2045dda784df3ff90ae0f2cc51ce39cf54867320ac6f3ba2c6f0d72360480c96614ae66581f266c35fb79fd28774afd113fa5187eff9206d7cbe90dd8bf67c844e202",
    .p_expected_output  = "43c511e9bc2307c62758df61125a980ee64cefebd90931cb91c13742d4714c06de4003faf3c41c06aefc638ad47b21906e6b104816b72de6269e045a1f4429d4"
};

// HMAC - Custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac512_min_key_min_message_0) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha512_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA512 key_len=1 message_len=1 zeros",
    .p_input            = "00",
    .p_key              = "00",
    .p_expected_output  = "869b81e40c5d97086fcaed2e14edb49ec5db2e4eb5d015b4639cc51098eaa4a4f2155695f831773ad03c716f6f859482cfbf7d0074d4ec7801921fedd07d253d"
};

// HMAC - Custom test vector
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac512_min_key_min_message_1) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha512_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA512 key_len=1 message_len=1 non-zeros",
    .p_input            = "34",
    .p_key              = "56",
    .p_expected_output  = "10574996dffef454583940f057589200331423b354cffe9c5f13a82c49695b0a700fb2607a9242ae83b85e81795e815a549dd5ff25bc9ebfb5f008b4f928c4ab"
};

// HMAC - NIST CAVS 11.0
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac_sha512_0) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha512_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA512 key_len=100 message_len=128",
    .p_input            = "2423dff48b312be864cb3490641f793d2b9fb68a7763b8e298c86f42245e4540eb01ae4d2d4500370b1886f23ca2cf9701704cad5bd21ba87b811daf7a854ea24a56565ced425b35e40e1acbebe03603e35dcf4a100e57218408a1d8dbcc3b99296cfea931efe3ebd8f719a6d9a15487b9ad67eafedf15559ca42445b0f9b42e",
    .p_key              = "57c2eb677b5093b9e829ea4babb50bde55d0ad59fec34a618973802b2ad9b78e26b2045dda784df3ff90ae0f2cc51ce39cf54867320ac6f3ba2c6f0d72360480c96614ae66581f266c35fb79fd28774afd113fa5187eff9206d7cbe90dd8bf67c844e202",
    .p_expected_output  = "33c511e9bc2307c62758df61125a980ee64cefebd90931cb91c13742d4714c06de4003faf3c41c06aefc638ad47b21906e6b104816b72de6269e045a1f4429d4"
};

// HMAC - NIST CAVS 11.0
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac_sha512_1) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha512_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA512 key_len=128 message_len=128",
    .p_input            = "b82eefb2081bd14dab0e9e345248a34ade73f3291886b91ea3e8cc742fd884f6ee0ccdaf4c9879f4db12dba58cf491af2541a1d5ef6cc8b1af750ef5d8559ef7ff9cd56d8f599974be3aecd8c0f4c08f3ae50d86f9f822a1e4ca39fd2f0b4d78d22630733a24d8d63ecdf9555411daf205a761c39ef46ff6292e74129bc13a7f",
    .p_key              = "e9e4480d1c4a621e0c4e1505992556347a7ab34fd2b289910474766cc969116f8040d96dc5f66cdc4454fa7bcfb9f838af19195038467ab8a16e1cbc12e598e6fd250e21b2145f1e2e859cf73400be12a0c69749f7100847429875351d5a76970b9ccf700c2ca3ad72e9e4c0f0840e8cf488158136989b0891f867211350134a",
    .p_expected_output  = "90093bdcc45da7338bd2efe92e30933b14f75582739c747f7572b3270b104f33af0c939e3c8ae53b2066fc8c97ccf38785cd2ec3d79e6946499d36121e44a3e7"
};

// HMAC - NIST CAVS 11.0
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac_sha512_2) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha512_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA512 key_len=142 message_len=128",
    .p_input            = "632afa8e79b14b2a3604f5855d2bf182d3c56d6853f21fe46271da5286065f38b31f751306b63c57b679beb14729c78f0040f7e2a0d615224dc5a693cd0cbec8f87117656d6b6029853ed72b85681a63183c3a6dfccd128afb0dd7e81d36f0231c69070b189560a88c9b697b81b0930701026190cf9ebe23559194d6de4d9a51",
    .p_key              = "01b95a887927ce31b1242391bbd00965eb77a903d4b8399b72e6cebda9ae721beefa779145160b626b110cc554671da0d8dcf993a9ab073888e02fa9b803ed43b3f6a3aa1d20340df6ccceac13cb0797cf612cb8fe5fd513228cbd4de249d16bb77587dde98f71bbba1a124ee046f0d239ccea7abb1accb5aab021b00dca491c623fcb3191a9ecf31fc680b4a41e",
    .p_expected_output  = "210ad45ca2fd1f105c0a18f993774f933ece57ace4da619689e1cb8b491a189cc6e4ee1954a32201072e70f934837c0fb6e239b4fdfbd26ebf11b9a919eafd09"
};

// RFC 4231 HMAC-SHA Test Case 1
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac_sha512_rfc1) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha512_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA512 key_len=20 message_len=8",
    .p_input            = "4869205468657265",
    .p_key              = "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b",
    .p_expected_output  = "87aa7cdea5ef619d4ff0b4241a1d6cb02379f4e2ce4ec2787ad0b30545e17cdedaa833b7d6b8a702038b274eaea3f4e4be9d914eeb61f1702e696c203a126854"
};

// RFC 4231 HMAC-SHA Test Case 2
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac_sha512_rfc2) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha512_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA512 key_len=4 message_len=28",
    .p_input            = "7768617420646f2079612077616e7420666f72206e6f7468696e673f",
    .p_key              = "4a656665",
    .p_expected_output  = "164b7a7bfcf819e2e395fbe73b56e0a387bd64222e831fd610270cd7ea2505549758bf75c05a994a6d034f65f8f0e6fdcaeab1a34d4a6b4b636e070a38bce737"
};

// RFC 4231 HMAC-SHA Test Case 3
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac_sha512_rfc3) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha512_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA512 key_len=20 message_len=50",
    .p_input            = "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd",
    .p_key              = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
    .p_expected_output  = "fa73b0089d56a284efb0f0756c890be9b1b5dbdd8ee81a3655f83e33b2279d39bf3e848279a722c806b485a47e67c807b946a337bee8942674278859e13292fb"
};

// RFC 4231 HMAC-SHA Test Case 4
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac_sha512_rfc4) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha512_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA512 key_len=25 message_len=50",
    .p_input            = "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd",
    .p_key              = "0102030405060708090a0b0c0d0e0f10111213141516171819",
    .p_expected_output  = "b0ba465637458c6990e5a8c5f61d4af7e576d97ff94b872de76f8050361ee3dba91ca5c11aa25eb4d679275cc5788063a5f19741120c4f2de2adebeb10a298dd"
};

// RFC 4231 HMAC-SHA Test Case 6
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac_sha512_rfc6) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha512_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA512 key_len=131 message_len=54",
    .p_input            = "54657374205573696e67204c6172676572205468616e20426c6f636b2d53697a65204b6579202d2048617368204b6579204669727374",
    .p_key              = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
    .p_expected_output  = "80b24263c7c1a3ebb71493c1dd7be8b49b46d1f41b4aeec1121b013783f8f3526b56d037e05f2598bd0fd2215d6a1e5295e64f73f63f0aec8b915a985d786598"
};

// RFC 4231 HMAC-SHA Test Case 7
NRF_SECTION_ITEM_REGISTER(test_vector_hmac_data, test_vector_hmac_t test_vector_hmac_sha512_rfc7) =
{
    .p_hmac_info        = &g_nrf_crypto_hmac_sha512_info,
    .expected_err_code  = NRF_SUCCESS,
    .expected_result    = EXPECTED_TO_PASS,
    .p_test_vector_name = "SHA512 key_len=131 message_len=152",
    .p_input            = "5468697320697320612074657374207573696e672061206c6172676572207468616e20626c6f636b2d73697a65206b657920616e642061206c6172676572207468616e20626c6f636b2d73697a6520646174612e20546865206b6579206e6565647320746f20626520686173686564206265666f7265206265696e6720757365642062792074686520484d414320616c676f726974686d2e",
    .p_key              = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
    .p_expected_output  = "e37b6a775dc87dbaa4dfa9f96e5e3ffddebd71f8867289865df5a32d20cdc944b6022cac3c4982b10d5eeb55c3e4de15134676fb6de0446065c97440fa8c6a58"
};

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_HMAC_SHA512)

/*lint -restore */

#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_HMAC)
