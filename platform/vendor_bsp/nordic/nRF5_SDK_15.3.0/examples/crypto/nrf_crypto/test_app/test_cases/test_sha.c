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

#if NRF_MODULE_ENABLED(NRF_CRYPTO_HASH)

NRF_SECTION_DEF(test_vector_hash_data, test_vector_hash_t);
NRF_SECTION_DEF(test_vector_hash_long_data, test_vector_hash_t);

#define CONTEXT_SIZE                (240)                                      /**< Temporary context size define. */
#define INPUT_BUF_SIZE              (4125)                                     /**< Input buffer size for SHA. */

/**< Get number of the SHA test vectors. */
#define TEST_VECTOR_SHA_GET(i)      \
    NRF_SECTION_ITEM_GET(test_vector_hash_data, test_vector_hash_t, (i))

/**< Get the test vector reference from the array of test vectors. */
#define TEST_VECTOR_SHA_COUNT       \
    NRF_SECTION_ITEM_COUNT(test_vector_hash_data, test_vector_hash_t)

/**< Get the number of long SHA test vectors. */
#define TEST_VECTOR_LONG_SHA_GET(i) \
    NRF_SECTION_ITEM_GET(test_vector_hash_long_data, test_vector_hash_t, (i))

/**< Get the test vector reference from the array of long test vectors. */
#define TEST_VECTOR_LONG_SHA_COUNT  \
    NRF_SECTION_ITEM_COUNT(test_vector_hash_long_data, test_vector_hash_t)

static nrf_crypto_hash_context_t hash_context;                                 /**< Hash context. */

static uint8_t m_sha_input_buf[INPUT_BUF_SIZE];                                /**< Buffer for storing the unhexified m_sha_input_buf data. */
static uint8_t m_sha_output_buf[NRF_CRYPTO_HASH_SIZE_SHA512];                  /**< Buffer for holding the calculated hash. */
static uint8_t m_sha_expected_output_buf[NRF_CRYPTO_HASH_SIZE_SHA512];         /**< Buffer for storing the unhexified expected ouput data. */


/**@brief Function for running the test setup.
 */
ret_code_t setup_test_case_sha(void)
{
    return NRF_SUCCESS;
}


/**@brief Function for the test execution.
 */
ret_code_t exec_test_case_sha(test_info_t * p_test_info)
{
    uint32_t i;
    uint32_t in_len;
    ret_code_t err_code;
    uint32_t expected_out_len;
    uint32_t sha_test_vector_count = TEST_VECTOR_SHA_COUNT;
    size_t   out_len;

    for (i = 0; i < sha_test_vector_count; i++)
    {
        test_vector_hash_t * p_test_vector = TEST_VECTOR_SHA_GET(i);
        p_test_info->current_id++;

        memset(m_sha_input_buf, 0x00, sizeof(m_sha_input_buf));
        memset(m_sha_output_buf, 0x00, sizeof(m_sha_output_buf));
        memset(m_sha_expected_output_buf, 0x00, sizeof(m_sha_expected_output_buf));

        // Fetch and unhexify test vectors.
        in_len           = unhexify(m_sha_input_buf, p_test_vector->p_input);
        expected_out_len = unhexify(m_sha_expected_output_buf, p_test_vector->p_expected_output);
        out_len          = expected_out_len;

        // Initialize the hash.
        start_time_measurement();
        err_code = nrf_crypto_hash_init(&hash_context, p_test_vector->p_hash_info);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == NRF_SUCCESS), "nrf_crypto_hash_init");

        // Update the hash.
        err_code = nrf_crypto_hash_update(&hash_context, m_sha_input_buf, in_len);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_hash_update");

        // Finalize the hash.
        err_code = nrf_crypto_hash_finalize(&hash_context, m_sha_output_buf, &out_len);
        stop_time_measurement();

        // Verify the nrf_crypto_hash_finalize err_code.
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_hash_finalize");

        // Verify the generated digest.
        TEST_VECTOR_ASSERT((expected_out_len == out_len), "Incorrect length");
        TEST_VECTOR_MEMCMP_ASSERT(m_sha_output_buf,
                                  m_sha_expected_output_buf,
                                  expected_out_len,
                                  p_test_vector->expected_result,
                                  "Incorrect hash");

        NRF_LOG_INFO("#%04d Test vector passed: %s %s",
                     p_test_info->current_id,
                     p_test_info->p_test_case_name,
                     p_test_vector->p_test_vector_name);

        p_test_info->tests_passed++;

exit_test_vector:

        while (NRF_LOG_PROCESS());

    }
    return NRF_SUCCESS;
}

/**@brief Function for the test execution.
 */
ret_code_t exec_test_case_sha_combined(test_info_t * p_test_info)
{
    uint32_t i;
    uint32_t in_len;
    ret_code_t err_code;
    uint32_t expected_out_len;
    uint32_t sha_test_vector_count = TEST_VECTOR_SHA_COUNT;
    size_t   out_len;

    for (i = 0; i < sha_test_vector_count; i++)
    {
        test_vector_hash_t * p_test_vector = TEST_VECTOR_SHA_GET(i);
        p_test_info->current_id++;

        memset(m_sha_input_buf, 0x00, sizeof(m_sha_input_buf));
        memset(m_sha_output_buf, 0x00, sizeof(m_sha_output_buf));
        memset(m_sha_expected_output_buf, 0x00, sizeof(m_sha_expected_output_buf));

        // Fetch message to the hash.
        in_len           = unhexify(m_sha_input_buf, p_test_vector->p_input);
        expected_out_len = unhexify(m_sha_expected_output_buf, p_test_vector->p_expected_output);
        out_len          = expected_out_len;

        // Execute the hash method.
        start_time_measurement();
        err_code = nrf_crypto_hash_calculate(&hash_context,
                                             p_test_vector->p_hash_info,
                                             m_sha_input_buf,
                                             in_len,
                                             m_sha_output_buf,
                                             &out_len);
        stop_time_measurement();

        // Verify the nrf_crypto_hash_calculate err_code.
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_hash_calculate");

        // Verify the generated digest.
        TEST_VECTOR_ASSERT((expected_out_len == out_len), "Incorrect length");
        TEST_VECTOR_MEMCMP_ASSERT(m_sha_output_buf,
                                  m_sha_expected_output_buf,
                                  expected_out_len,
                                  p_test_vector->expected_result,
                                  "Incorrect hash");

        NRF_LOG_INFO("#%04d Test vector passed: %s %s",
                     p_test_info->current_id,
                     p_test_info->p_test_case_name,
                     p_test_vector->p_test_vector_name);

        p_test_info->tests_passed++;

exit_test_vector:

        while (NRF_LOG_PROCESS());

    }
    return NRF_SUCCESS;
}


/**@brief Function for verifying the SHA digest of long messages.
 */
ret_code_t exec_test_case_sha_long(test_info_t * p_test_info)
{
    uint32_t i;
    uint32_t j;
    uint32_t in_len;
    ret_code_t err_code;
    uint32_t expected_out_len;
    uint32_t sha_test_vector_count = TEST_VECTOR_LONG_SHA_COUNT;
    size_t   out_len;

    for (i = 0; i < sha_test_vector_count; i++)
    {
        test_vector_hash_t * p_test_vector = TEST_VECTOR_LONG_SHA_GET(i);
        p_test_info->current_id++;

        memset(m_sha_input_buf, 0x00, sizeof(m_sha_input_buf));
        memset(m_sha_output_buf, 0x00, sizeof(m_sha_output_buf));
        memset(m_sha_expected_output_buf, 0x00, sizeof(m_sha_expected_output_buf));

        // Fetch and unhexify test vectors.
        in_len           = p_test_vector->chunk_length;
        expected_out_len = unhexify(m_sha_expected_output_buf, p_test_vector->p_expected_output);
        out_len          = expected_out_len;

        memcpy(m_sha_input_buf, p_test_vector->p_input, in_len);

        // Initialize the hash.
        start_time_measurement();
        err_code = nrf_crypto_hash_init(&hash_context, p_test_vector->p_hash_info);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_hash_init");

        // Update the hash until all input data is processed.
        for (j = 0; j < p_test_vector->update_iterations; j++)
        {
            // Test mode for measuring the memcpy from the flash in SHA.
            if (p_test_vector->mode == DO_MEMCPY)
            {
                memcpy(m_sha_input_buf, p_test_vector->p_input, 4096);
            }

            err_code = nrf_crypto_hash_update(&hash_context, m_sha_input_buf, in_len);
            TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                        "nrf_crypto_hash_update");
        }

        // Finalize the hash.
        err_code = nrf_crypto_hash_finalize(&hash_context, m_sha_output_buf, &out_len);
        stop_time_measurement();

        // Verify the nrf_crypto_hash_finalize err_code.
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_hash_finalize");

        // Verify the generated digest.
        TEST_VECTOR_ASSERT((expected_out_len == out_len), "Incorrect length");
        TEST_VECTOR_MEMCMP_ASSERT(m_sha_output_buf,
                                  m_sha_expected_output_buf,
                                  expected_out_len,
                                  p_test_vector->expected_result,
                                  "Incorrect hash");

        NRF_LOG_INFO("#%04d Test vector passed: %s %s",
                     p_test_info->current_id,
                     p_test_info->p_test_case_name,
                     p_test_vector->p_test_vector_name);

        p_test_info->tests_passed++;

exit_test_vector:

        while (NRF_LOG_PROCESS());

    }
    return NRF_SUCCESS;
}


/**@brief Function for running the test teardown.
 */
ret_code_t teardown_test_case_sha(void)
{
    return NRF_SUCCESS;
}


/** @brief  Macro for registering the SHA test case by using section variables.
 *
 * @details     This macro places a variable in a section named "test_case_data",
 *              which is initialized by main.
 */
NRF_SECTION_ITEM_REGISTER(test_case_data, test_case_t test_sha) =
{
    .p_test_case_name = "SHA",
    .setup = setup_test_case_sha,
    .exec = exec_test_case_sha,
    .teardown = teardown_test_case_sha
};


/** @brief  Macro for registering the SHA test case by using section variables.
 *
 * @details     This macro places a variable in a section named "test_case_data",
 *              which is initialized by main.
 */
NRF_SECTION_ITEM_REGISTER(test_case_data, test_case_t test_sha_combined) =
{
    .p_test_case_name = "SHA combined",
    .setup = setup_test_case_sha,
    .exec = exec_test_case_sha_combined,
    .teardown = teardown_test_case_sha
};


/** @brief  Macro for registering the SHA test case by using section variables.
 *
 * @details     This macro places a variable in a section named "test_case_data",
 *              which is initialized by main.
 */
NRF_SECTION_ITEM_REGISTER(test_case_data, test_case_t test_sha_long) =
{
    .p_test_case_name = "SHA long",
    .setup = setup_test_case_sha,
    .exec = exec_test_case_sha_long,
    .teardown = teardown_test_case_sha
};

#endif // NRF_CRYPTO_HASH_ENABLED
