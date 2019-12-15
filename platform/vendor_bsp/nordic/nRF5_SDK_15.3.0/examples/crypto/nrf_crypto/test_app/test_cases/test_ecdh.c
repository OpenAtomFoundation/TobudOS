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
#include "boards.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_error.h"
#include "app_util.h"
#include "nrf_section.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "common_test.h"
#include "nrf_crypto.h"
#include "mem_manager.h"

#if NRF_CRYPTO_ECC_ENABLED

NRF_SECTION_DEF(test_vector_ecdh_data_random, test_vector_ecdh_t);
NRF_SECTION_DEF(test_vector_ecdh_data_deterministic_simple, test_vector_ecdh_t);
NRF_SECTION_DEF(test_vector_ecdh_data_deterministic_full, test_vector_ecdh_t);

/**< Get number of ECDH test vectors. */
#define TEST_VECTOR_ECDH_RANDOM_GET(i)     \
    NRF_SECTION_ITEM_GET(test_vector_ecdh_data_random, test_vector_ecdh_t, (i))

/**< Get test vector reference from array of test vectors. */
#define TEST_VECTOR_ECDH_RANDOM_COUNT      \
    NRF_SECTION_ITEM_COUNT(test_vector_ecdh_data_random, test_vector_ecdh_t)

/**< Get number of ECDH test vectors. */
#define TEST_VECTOR_ECDH_DET_FULL_GET(i)   \
    NRF_SECTION_ITEM_GET(test_vector_ecdh_data_deterministic_full, test_vector_ecdh_t, (i))

/**< Get test vector reference from array of test vectors. */
#define TEST_VECTOR_ECDH_DET_FULL_COUNT    \
    NRF_SECTION_ITEM_COUNT(test_vector_ecdh_data_deterministic_full, test_vector_ecdh_t)

/**< Get number of ECDH test vectors. */
#define TEST_VECTOR_ECDH_DET_SIMPLE_GET(i) \
    NRF_SECTION_ITEM_GET(test_vector_ecdh_data_deterministic_simple, test_vector_ecdh_t, (i))

/**< Get test vector reference from array of test vectors. */
#define TEST_VECTOR_ECDH_DET_SIMPLE_COUNT  \
    NRF_SECTION_ITEM_COUNT(test_vector_ecdh_data_deterministic_simple, test_vector_ecdh_t)

static nrf_crypto_ecc_public_key_t  m_initiater_public_key;                             /**< Public key structure */
static nrf_crypto_ecc_private_key_t m_initiater_private_key;                            /**< Private key structure */
static nrf_crypto_ecc_public_key_t  m_responder_public_key;                             /**< Public key structure */
static nrf_crypto_ecc_private_key_t m_responder_private_key;                            /**< Private key structure */
static nrf_crypto_ecdh_context_t    m_initiater_ecdh_context;                           /**< ECDH context for initiater */
static nrf_crypto_ecdh_context_t    m_responder_ecdh_context;                           /**< ECDH context for responder */

static uint8_t m_ecdh_initiater_priv_key_buf[NRF_CRYPTO_ECC_RAW_PRIVATE_KEY_MAX_SIZE];  /**< Buffer for storing unhexified initiater private key. */
static uint8_t m_ecdh_responder_priv_key_buf[NRF_CRYPTO_ECC_RAW_PRIVATE_KEY_MAX_SIZE];  /**< Buffer for storing unhexified responder private key. */
static uint8_t m_ecdh_initiater_publ_key_buf[NRF_CRYPTO_ECC_RAW_PUBLIC_KEY_MAX_SIZE];   /**< Buffer for storing unhexified initiater public key. */
static uint8_t m_ecdh_responder_publ_key_buf[NRF_CRYPTO_ECC_RAW_PUBLIC_KEY_MAX_SIZE];   /**< Buffer for storing unhexified responder public key. */
static uint8_t m_ecdh_initiater_ss_buf[NRF_CRYPTO_ECDH_SHARED_SECRET_MAX_SIZE];         /**< Buffer for holding the initiater calculated shared secret. */
static uint8_t m_ecdh_responder_ss_buf[NRF_CRYPTO_ECDH_SHARED_SECRET_MAX_SIZE];         /**< Buffer for holding the responder calculated shared secret. */
static uint8_t m_ecdh_expected_ss_buf[NRF_CRYPTO_ECDH_SHARED_SECRET_MAX_SIZE];          /**< Buffer for storing unhexified expected ouput data. */


/**@brief Function for running test setup.
 */
ret_code_t setup_test_case_ecdh(void)
{
    return NRF_SUCCESS;
}


/**@brief Function for executing ECDH for initiater and repsonder by 
 * using random generated keys.
 */
ret_code_t exec_test_case_ecdh_random(test_info_t * p_test_info)
{
    uint32_t i;
    ret_code_t err_code;
    uint32_t ecdh_test_vector_count = TEST_VECTOR_ECDH_RANDOM_COUNT;

    size_t initiater_ss_len;
    size_t responder_ss_len;

    for (i = 0; i < ecdh_test_vector_count; i++)
    {
        test_vector_ecdh_t * p_test_vector = TEST_VECTOR_ECDH_RANDOM_GET(i);
        p_test_info->current_id++;

        // Reset buffers.
        memset(m_ecdh_initiater_ss_buf, 0x00, sizeof(m_ecdh_initiater_ss_buf));
        memset(m_ecdh_responder_ss_buf, 0x00, sizeof(m_ecdh_responder_ss_buf));

#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_OPTIGA)
        // select different storage locations for private key in OPTIGA
        m_initiater_private_key.key_secp256r1.oid = 0xE101;
        m_responder_private_key.key_secp256r1.oid = 0xE102;
#endif

        // Fetch and unhexify test vectors.
        initiater_ss_len = p_test_vector->p_curve_info->raw_private_key_size;
        responder_ss_len = p_test_vector->p_curve_info->raw_private_key_size;

        // Generate random public and private keys for initiater.
        err_code = nrf_crypto_ecc_key_pair_generate(NULL,
                                                    p_test_vector->p_curve_info,
                                                    &m_initiater_private_key,
                                                    &m_initiater_public_key);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_ecc_key_pair_generate for initiater");

        // Generate random public and private keys for responder.
        err_code = nrf_crypto_ecc_key_pair_generate(NULL,
                                                    p_test_vector->p_curve_info,
                                                    &m_responder_private_key,
                                                    &m_responder_public_key);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_ecc_key_pair_generate for responder");

        // Execute ECDH on initiater side.
        start_time_measurement();
        err_code = nrf_crypto_ecdh_compute(&m_initiater_ecdh_context,
                                           &m_initiater_private_key,
                                           &m_responder_public_key,
                                           m_ecdh_initiater_ss_buf,
                                           &initiater_ss_len);
        stop_time_measurement();

        // Verify nrf_crypto_ecdh_compute err_code.
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_ecdh_compute for initiater");

        // Execute ECDH on responder side.
        err_code = nrf_crypto_ecdh_compute(&m_responder_ecdh_context,
                                           &m_responder_private_key,
                                           &m_initiater_public_key,
                                           m_ecdh_responder_ss_buf,
                                           &responder_ss_len);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_ecdh_compute for responder");
        
        // Verify length of generated ECDH shared secrets.
        TEST_VECTOR_ASSERT((initiater_ss_len == responder_ss_len),
                            "Different shared secret length");
        
        // Compare generated initiater shared secret to responder shared secret.
        TEST_VECTOR_MEMCMP_ASSERT(m_ecdh_initiater_ss_buf,
                                  m_ecdh_responder_ss_buf,
                                  initiater_ss_len,
                                  p_test_vector->expected_result,
                                  "Shared secret mismatch between responder and initiater");

        NRF_LOG_INFO("#%04d Test vector passed: %s %s",
                     p_test_info->current_id,
                     p_test_info->p_test_case_name,
                     p_test_vector->p_test_vector_name);

        p_test_info->tests_passed++;

exit_test_vector:

        // Free the generated keys.
        (void) nrf_crypto_ecc_private_key_free(&m_initiater_private_key);
        (void) nrf_crypto_ecc_private_key_free(&m_responder_private_key);
        (void) nrf_crypto_ecc_public_key_free(&m_initiater_public_key);
        (void) nrf_crypto_ecc_public_key_free(&m_responder_public_key);

        while (NRF_LOG_PROCESS());
    }

    return NRF_SUCCESS;
}


/**@brief Function for executing deterministic ECDH for initiater and repsonder.
 */
ret_code_t exec_test_case_ecdh_deterministic_full(test_info_t * p_test_info)
{
    uint32_t i;
    ret_code_t err_code;
    uint32_t ecdh_test_vector_count = TEST_VECTOR_ECDH_DET_FULL_COUNT;

    size_t expected_ss_len;
    size_t initiater_ss_len;
    size_t responder_ss_len;
    size_t initiater_priv_key_len;
    size_t responder_priv_key_len;
    size_t initiater_publ_key_len;
    size_t responder_publ_key_len;

    for (i = 0; i < ecdh_test_vector_count; i++)
    {
        test_vector_ecdh_t * p_test_vector = TEST_VECTOR_ECDH_DET_FULL_GET(i);
        p_test_info->current_id++;

        // Reset buffers.
        memset(m_ecdh_expected_ss_buf, 0x00, sizeof(m_ecdh_expected_ss_buf));
        memset(m_ecdh_initiater_ss_buf, 0x00, sizeof(m_ecdh_initiater_ss_buf));
        memset(m_ecdh_responder_ss_buf, 0x00, sizeof(m_ecdh_responder_ss_buf));
        memset(m_ecdh_initiater_priv_key_buf, 0x00, sizeof(m_ecdh_initiater_priv_key_buf));
        memset(m_ecdh_responder_priv_key_buf, 0x00, sizeof(m_ecdh_responder_priv_key_buf));
        memset(m_ecdh_initiater_publ_key_buf, 0x00, sizeof(m_ecdh_initiater_publ_key_buf));
        memset(m_ecdh_responder_publ_key_buf, 0x00, sizeof(m_ecdh_responder_publ_key_buf));

        // Fetch and unhexify test vectors.
        initiater_priv_key_len  = unhexify(m_ecdh_initiater_priv_key_buf,
                                          p_test_vector->p_initiater_priv);
        responder_priv_key_len  = unhexify(m_ecdh_responder_priv_key_buf,
                                          p_test_vector->p_responder_priv);
        initiater_publ_key_len  = unhexify(m_ecdh_initiater_publ_key_buf,
                                          p_test_vector->p_initiater_publ_x);
        initiater_publ_key_len += unhexify(&m_ecdh_initiater_publ_key_buf[initiater_publ_key_len],
                                           p_test_vector->p_initiater_publ_y);
        responder_publ_key_len  = unhexify(m_ecdh_responder_publ_key_buf,
                                          p_test_vector->p_responder_publ_x);
        responder_publ_key_len += unhexify(&m_ecdh_responder_publ_key_buf[responder_publ_key_len],
                                           p_test_vector->p_responder_publ_y);
        expected_ss_len         = unhexify(m_ecdh_expected_ss_buf,
                                              p_test_vector->p_expected_shared_secret);
        initiater_ss_len = expected_ss_len;
        responder_ss_len = expected_ss_len;

        // Generate public and private keys from raw data.
        err_code = nrf_crypto_ecc_public_key_from_raw(p_test_vector->p_curve_info,
                                                      &m_initiater_public_key,
                                                      m_ecdh_initiater_publ_key_buf,
                                                      initiater_publ_key_len);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_ecc_public_key_from_raw for initiater");

        err_code = nrf_crypto_ecc_public_key_from_raw(p_test_vector->p_curve_info,
                                                      &m_responder_public_key,
                                                      m_ecdh_responder_publ_key_buf,
                                                      responder_publ_key_len);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_ecc_public_key_from_raw for responder");

        err_code = nrf_crypto_ecc_private_key_from_raw(p_test_vector->p_curve_info,
                                                       &m_initiater_private_key,
                                                       m_ecdh_initiater_priv_key_buf,
                                                       initiater_priv_key_len);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_ecc_private_key_from_raw for initiater");

        err_code = nrf_crypto_ecc_private_key_from_raw(p_test_vector->p_curve_info,
                                                       &m_responder_private_key,
                                                       m_ecdh_responder_priv_key_buf,
                                                       responder_priv_key_len);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_ecc_private_key_from_raw for responder");

        // Execute ECDH on initiater side.
        start_time_measurement();
        err_code = nrf_crypto_ecdh_compute(&m_initiater_ecdh_context,
                                           &m_initiater_private_key,
                                           &m_responder_public_key,
                                           m_ecdh_initiater_ss_buf,
                                           &initiater_ss_len);
        stop_time_measurement();

        // Verify nrf_crypto_ecdh_compute err_code.
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_ecdh_compute for initiater");

        // Execute ECDH on responder side.
        err_code = nrf_crypto_ecdh_compute(&m_responder_ecdh_context,
                                           &m_responder_private_key,
                                           &m_initiater_public_key,
                                           m_ecdh_responder_ss_buf,
                                           &responder_ss_len);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_ecdh_compute for responder");

        // Verify length of generated shared secrets.
        TEST_VECTOR_ASSERT(((expected_ss_len == initiater_ss_len)   &&
                            (expected_ss_len == responder_ss_len)   ),
                           "Incorrect shared secret length");

        // Compare generated initiater shared secret to responder shared secret.
        TEST_VECTOR_MEMCMP_ASSERT(m_ecdh_initiater_ss_buf,
                                  m_ecdh_responder_ss_buf,
                                  initiater_ss_len,
                                  p_test_vector->expected_result,
                                  "Shared secret mismatch between responder and initiater");

        // Compare generated shared secret to expected shared secret.
        TEST_VECTOR_MEMCMP_ASSERT(m_ecdh_responder_ss_buf,
                                  m_ecdh_expected_ss_buf,
                                  expected_ss_len,
                                  p_test_vector->expected_result,
                                  "Shared secret mismatch between responder and expected");

        NRF_LOG_INFO("#%04d Test vector passed: %s %s",
                     p_test_info->current_id,
                     p_test_info->p_test_case_name,
                     p_test_vector->p_test_vector_name);

        p_test_info->tests_passed++;

exit_test_vector:

        // Free the generated keys.
        (void) nrf_crypto_ecc_private_key_free(&m_initiater_private_key);
        (void) nrf_crypto_ecc_private_key_free(&m_responder_private_key);
        (void) nrf_crypto_ecc_public_key_free(&m_initiater_public_key);
        (void) nrf_crypto_ecc_public_key_free(&m_responder_public_key);

        while (NRF_LOG_PROCESS());
    }

    return NRF_SUCCESS;
}


/**@brief Function for executing deterministic ECDH for responder.
 */
ret_code_t exec_test_case_ecdh_deterministic(test_info_t * p_test_info)
{
    uint32_t i;
    ret_code_t err_code;
    uint32_t ecdh_test_vector_count = TEST_VECTOR_ECDH_DET_SIMPLE_COUNT;

    size_t  expected_ss_len;
    size_t  responder_ss_len;
    size_t  responder_priv_key_len;
    size_t  initiater_publ_key_len;

    for (i = 0; i < ecdh_test_vector_count; i++)
    {
        test_vector_ecdh_t * p_test_vector = TEST_VECTOR_ECDH_DET_SIMPLE_GET(i);
        p_test_info->current_id++;

        // Reset buffers.
        memset(m_ecdh_expected_ss_buf, 0x00, sizeof(m_ecdh_expected_ss_buf));
        memset(m_ecdh_responder_ss_buf, 0x00, sizeof(m_ecdh_responder_ss_buf));
        memset(m_ecdh_responder_priv_key_buf, 0x00, sizeof(m_ecdh_responder_priv_key_buf));
        memset(m_ecdh_initiater_publ_key_buf, 0x00, sizeof(m_ecdh_initiater_publ_key_buf));

        // Fetch and unhexify test vectors.
        responder_priv_key_len  = unhexify(m_ecdh_responder_priv_key_buf,
                                          p_test_vector->p_responder_priv);
        initiater_publ_key_len  = unhexify(m_ecdh_initiater_publ_key_buf,
                                          p_test_vector->p_initiater_publ_x);
        initiater_publ_key_len += unhexify(&m_ecdh_initiater_publ_key_buf[initiater_publ_key_len],
                                           p_test_vector->p_initiater_publ_y);
        expected_ss_len         = unhexify(m_ecdh_expected_ss_buf,
                                              p_test_vector->p_expected_shared_secret);
        responder_ss_len        = expected_ss_len;

        // Generate public and private keys from raw data.
        (void) nrf_crypto_ecc_public_key_from_raw(p_test_vector->p_curve_info,
                                                  &m_initiater_public_key,
                                                  m_ecdh_initiater_publ_key_buf,
                                                  initiater_publ_key_len);
        (void) nrf_crypto_ecc_private_key_from_raw(p_test_vector->p_curve_info,
                                                   &m_responder_private_key,
                                                   m_ecdh_responder_priv_key_buf,
                                                   responder_priv_key_len);

        // Execute ECDH on responder side.
        start_time_measurement();
        err_code = nrf_crypto_ecdh_compute(NULL,
                                           &m_responder_private_key,
                                           &m_initiater_public_key,
                                           m_ecdh_responder_ss_buf,
                                           &responder_ss_len);
        stop_time_measurement();

        // Verify nrf_crypto_ecdh_compute err_code.
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_ecdh_compute for initiater");

        // Verify length of generated shared secrets.
        TEST_VECTOR_ASSERT((expected_ss_len == responder_ss_len),
                           "Incorrect shared secret length");

        // Compare generated shared secret to expected shared secret.
        TEST_VECTOR_MEMCMP_ASSERT(m_ecdh_responder_ss_buf,
                                  m_ecdh_expected_ss_buf,
                                  expected_ss_len,
                                  p_test_vector->expected_result,
                                  "Shared secret mismatch between responder and expected");

        NRF_LOG_INFO("#%04d Test vector passed: %s %s",
                     p_test_info->current_id,
                     p_test_info->p_test_case_name,
                     p_test_vector->p_test_vector_name);

        p_test_info->tests_passed++;

exit_test_vector:

        // Free the generated keys.
        (void) nrf_crypto_ecc_private_key_free(&m_responder_private_key);
        (void) nrf_crypto_ecc_public_key_free(&m_initiater_public_key);

        while (NRF_LOG_PROCESS());
    }

    return NRF_SUCCESS;
}


/**@brief Function for running test teardown.
 */
ret_code_t teardown_test_case_ecdh(void)
{
    return NRF_SUCCESS;
}


/** @brief  Macro for registering the ECDH test case by using section variables.
 *
 * @details     This macro places a variable in a section named "test_case_data",
 *              which is initialized by main.
 */
NRF_SECTION_ITEM_REGISTER(test_case_data, test_case_t test_ecdh) =
{
    .p_test_case_name = "ECDH random",
    .setup = setup_test_case_ecdh,
    .exec = exec_test_case_ecdh_random,
    .teardown = teardown_test_case_ecdh
};


/** @brief  Macro for registering the ECDH test case by using section variables.
 *
 * @details     This macro places a variable in a section named "test_case_data",
 *              which is initialized by main.
 */
NRF_SECTION_ITEM_REGISTER(test_case_data, test_case_t test_ecdh_det) =
{
    .p_test_case_name = "ECDH deterministic",
    .setup = setup_test_case_ecdh,
    .exec = exec_test_case_ecdh_deterministic,
    .teardown = teardown_test_case_ecdh
};


/** @brief  Macro for registering the ECDH test case by using section variables.
 *
 * @details     This macro places a variable in a section named "test_case_data",
 *              which is initialized by main.
 */
NRF_SECTION_ITEM_REGISTER(test_case_data, test_case_t test_ecdh_det_full) =
{
    .p_test_case_name = "ECDH deterministic full",
    .setup = setup_test_case_ecdh,
    .exec = exec_test_case_ecdh_deterministic_full,
    .teardown = teardown_test_case_ecdh
};

#endif // NRF_CRYPTO_ECC_ENABLED
