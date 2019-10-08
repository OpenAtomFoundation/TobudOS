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


#if NRF_CRYPTO_ECC_ENABLED && NRF_CRYPTO_ECC_ED25519_ENABLED

NRF_SECTION_DEF(test_vector_eddsa_verify_data, test_vector_ecdsa_verify_t);
NRF_SECTION_DEF(test_vector_eddsa_sign_data, test_vector_ecdsa_sign_t);

#define EDDSA_MAX_INPUT_SIZE                (1024)                                            /**< EdDSA requires a large buffer. */

/**< Get number of the EdDSA verify test vectors. */
#define TEST_VECTOR_EDDSA_VERIFY_GET(i)     \
    NRF_SECTION_ITEM_GET(test_vector_eddsa_verify_data, test_vector_ecdsa_verify_t, (i))

/**< Get the test vector reference from the array of test vectors. */
#define TEST_VECTOR_EDDSA_VERIFY_COUNT      \
    NRF_SECTION_ITEM_COUNT(test_vector_eddsa_verify_data, test_vector_ecdsa_verify_t)

/**< Get the number of EdDSA sign test vectors. */
#define TEST_VECTOR_EDDSA_SIGN_GET(i)       \
    NRF_SECTION_ITEM_GET(test_vector_eddsa_sign_data, test_vector_ecdsa_sign_t, (i))

/**< Get the test vector reference from the array of test vectors. */
#define TEST_VECTOR_EDDSA_SIGN_COUNT        \
    NRF_SECTION_ITEM_COUNT(test_vector_eddsa_sign_data, test_vector_ecdsa_sign_t)

static uint8_t            m_eddsa_input_buf[EDDSA_MAX_INPUT_SIZE];                            /**< Buffer for storing the raw EdDSA input. */
static uint8_t            m_eddsa_signature_buf[NRF_CRYPTO_EDDSA_ED25519_SIGNATURE_SIZE];     /**< Buffer for storing the raw EdDSA signature. */
__ALIGN(4) static uint8_t m_eddsa_public_key_buf[NRF_CRYPTO_ECC_RAW_PUBLIC_KEY_MAX_SIZE];     /**< Buffer for storing the raw EdDSA public key. */
__ALIGN(4) static uint8_t m_eddsa_private_key_buf[NRF_CRYPTO_ECC_RAW_PRIVATE_KEY_MAX_SIZE];   /**< Buffer for storing the raw EdDSA private key. */

static nrf_crypto_ecc_public_key_t          m_eddsa_public_key;                               /**< Public key structure. */
static nrf_crypto_ecc_private_key_t         m_eddsa_private_key;                              /**< Private key structure. */
static nrf_crypto_eddsa_verify_context_t    m_eddsa_verify_context;                           /**< EdDSA verify context. */
static nrf_crypto_eddsa_sign_context_t      m_eddsa_sign_context;                             /**< EdDSA sign context. */

static uint8_t const                      * p_eddsa_input = m_eddsa_input_buf;                /**< Pointer to the EdDSA input buffer. */
static uint8_t const                      * p_eddsa_signature = m_eddsa_signature_buf;        /**< Pointer to the EdDSA signature buffer. */


/**@brief Function for running the test setup.
 */
ret_code_t setup_test_case_eddsa(void)
{
    return NRF_SUCCESS;
}


/**@brief Function for the EdDSA sign test execution.
 */
ret_code_t exec_test_case_eddsa_sign(test_info_t * p_test_info)
{
    uint32_t i;
    ret_code_t err_code;
    uint32_t input_len;
    uint32_t publ_key_len;
    uint32_t priv_key_len;
    uint32_t eddsa_test_vector_count = TEST_VECTOR_EDDSA_SIGN_COUNT;
    size_t   sign_len;

    p_eddsa_input = m_eddsa_input_buf;
    p_eddsa_signature = m_eddsa_signature_buf;

    for (i = 0; i < eddsa_test_vector_count; i++)
    {
        test_vector_ecdsa_sign_t * p_test_vector = TEST_VECTOR_EDDSA_SIGN_GET(i);
        p_test_info->current_id++;

        // Reset buffers.
        memset(m_eddsa_input_buf, 0x00, sizeof(m_eddsa_input_buf));
        memset(m_eddsa_signature_buf, 0x00, sizeof(m_eddsa_signature_buf));
        memset(m_eddsa_public_key_buf, 0x00, sizeof(m_eddsa_public_key_buf));
        memset(m_eddsa_private_key_buf, 0x00, sizeof(m_eddsa_private_key_buf));

        // Fetch test vectors.
        input_len     = unhexify(m_eddsa_input_buf, p_test_vector->p_input);
        publ_key_len  = unhexify(m_eddsa_public_key_buf, p_test_vector->p_qx);
        publ_key_len += unhexify(&m_eddsa_public_key_buf[publ_key_len], p_test_vector->p_qy);
        priv_key_len  = unhexify(m_eddsa_private_key_buf, p_test_vector->p_x);
        sign_len      = sizeof(m_eddsa_signature_buf);

        // Generate the EdDSA public key from the raw buffer.
        err_code = nrf_crypto_ecc_public_key_from_raw(p_test_vector->p_curve_info,
                                                      &m_eddsa_public_key,
                                                      m_eddsa_public_key_buf,
                                                      publ_key_len);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == NRF_SUCCESS),
                                    "nrf_crypto_ecc_public_key_from_raw");

        // Generate the EdDSA private key from the raw buffer.
        err_code = nrf_crypto_ecc_private_key_from_raw(p_test_vector->p_curve_info,
                                                       &m_eddsa_private_key,
                                                       m_eddsa_private_key_buf,
                                                       priv_key_len);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == NRF_SUCCESS),
                                    "nrf_crypto_ecc_private_key_from_raw");

        // Generate the EdDSA signature.
        start_time_measurement();
        err_code = nrf_crypto_eddsa_sign(&m_eddsa_sign_context,
                                         &m_eddsa_private_key,
                                         p_eddsa_input,
                                         input_len,
                                         m_eddsa_signature_buf,
                                         &sign_len);
        stop_time_measurement();

        // Verify the nrf_crypto_ecdh_compute err_code.
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_sign_err_code),
        "nrf_crypto_eddsa_sign");

        // Verify the generated EdDSA signature by running the EdDSA verify.
        err_code = nrf_crypto_eddsa_verify(&m_eddsa_verify_context,
                                           &m_eddsa_public_key,
                                           p_eddsa_input,
                                           input_len,
                                           p_eddsa_signature,
                                           sign_len);

        // Verify the nrf_crypto_eddsa_verify err_code.
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_verify_err_code),
                                    "nrf_crypto_eddsa_verify");

        NRF_LOG_INFO("#%04d Test vector passed: %s %s",
                     p_test_info->current_id,
                     p_test_info->p_test_case_name,
                     p_test_vector->p_test_vector_name);

        p_test_info->tests_passed++;

exit_test_vector:

        // Free the generated keys.
        (void)nrf_crypto_ecc_private_key_free(&m_eddsa_private_key);
        (void)nrf_crypto_ecc_public_key_free(&m_eddsa_public_key);

        while (NRF_LOG_PROCESS());
    }

    return NRF_SUCCESS;
}


/**@brief Function for the EdDSA verify test execution.
 */
ret_code_t exec_test_case_eddsa_verify(test_info_t * p_test_info)
{
    uint32_t i;
    ret_code_t err_code;
    uint32_t input_len;
    uint32_t publ_key_len;
    uint32_t sign_len;
    uint32_t eddsa_test_vector_count = TEST_VECTOR_EDDSA_VERIFY_COUNT;

    p_eddsa_input = m_eddsa_input_buf;
    p_eddsa_signature = m_eddsa_signature_buf;

    for (i = 0; i < eddsa_test_vector_count; i++)
    {
        test_vector_ecdsa_verify_t * p_test_vector = TEST_VECTOR_EDDSA_VERIFY_GET(i);
        p_test_info->current_id++;

        // Reset buffers.
        memset(m_eddsa_input_buf, 0x00, sizeof(m_eddsa_input_buf));
        memset(m_eddsa_signature_buf, 0x00, sizeof(m_eddsa_signature_buf));
        memset(m_eddsa_public_key_buf, 0x00, sizeof(m_eddsa_public_key_buf));

        // Fetch test vectors.
        input_len = unhexify(m_eddsa_input_buf, p_test_vector->p_input);
        sign_len  = unhexify(m_eddsa_signature_buf, p_test_vector->p_r);
        sign_len += unhexify(&m_eddsa_signature_buf[sign_len], p_test_vector->p_s);
        publ_key_len = unhexify(m_eddsa_public_key_buf, p_test_vector->p_qx);
        publ_key_len += unhexify(&m_eddsa_public_key_buf[publ_key_len], p_test_vector->p_qy);

        // Generate the EdDSA public key from the raw buffer.
        err_code = nrf_crypto_ecc_public_key_from_raw(p_test_vector->p_curve_info,
                                                      &m_eddsa_public_key,
                                                      m_eddsa_public_key_buf,
                                                      publ_key_len);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == NRF_SUCCESS),
                                    "nrf_crypto_ecc_public_key_from_raw");

        // Verify the EdDSA signature by running the EdDSA verify.
        start_time_measurement();
        err_code = nrf_crypto_eddsa_verify(&m_eddsa_verify_context,
                                           &m_eddsa_public_key,
                                           p_eddsa_input,
                                           input_len,
                                           p_eddsa_signature,
                                           sign_len);
        stop_time_measurement();

        // Verify the nrf_crypto_eddsa_verify err_code.
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_eddsa_verify");

        NRF_LOG_INFO("#%04d Test vector passed: %s %s",
                     p_test_info->current_id,
                     p_test_info->p_test_case_name,
                     p_test_vector->p_test_vector_name);

        p_test_info->tests_passed++;

exit_test_vector:

        // Free the generated key.
        (void)nrf_crypto_ecc_public_key_free(&m_eddsa_public_key);

        while (NRF_LOG_PROCESS());
    }
    return NRF_SUCCESS;
}



/**@brief Function for running the test teardown.
 */
ret_code_t teardown_test_case_eddsa(void)
{
    (void)nrf_crypto_ecc_public_key_free(&m_eddsa_public_key);
    (void)nrf_crypto_ecc_private_key_free(&m_eddsa_private_key);
    return NRF_SUCCESS;
}


/** @brief  Macro for registering the EdDSA sign test case by using section variables.
 *
 * @details     This macro places a variable in a section named "test_case_data",
 *              which is initialized by main.
 */
NRF_SECTION_ITEM_REGISTER(test_case_data, test_case_t test_eddsa_sign) =
{
    .p_test_case_name = "EdDSA Sign",
    .setup = setup_test_case_eddsa,
    .exec = exec_test_case_eddsa_sign,
    .teardown = teardown_test_case_eddsa
};


/** @brief  Macro for registering the EdDSA verify test case by using section variables.
 *
 * @details     This macro places a variable in a section named "test_case_data",
 *              which is initialized by main.
 */
NRF_SECTION_ITEM_REGISTER(test_case_data, test_case_t test_eddsa_verify) =
{
    .p_test_case_name = "EdDSA Verify",
    .setup = setup_test_case_eddsa,
    .exec = exec_test_case_eddsa_verify,
    .teardown = teardown_test_case_eddsa
};

#endif // NRF_CRYPTO_ECC_ENABLED && NRF_CRYPTO_ECC_ED25519_ENABLED
