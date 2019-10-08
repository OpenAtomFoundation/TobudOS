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

#if NRF_MODULE_ENABLED(NRF_CRYPTO_HMAC)

NRF_SECTION_DEF(test_vector_hkdf_data, test_vector_hkdf_t);

#define CONTEXT_SIZE             (240)                                   /**< Temporary context size define. */
#define KEY_BUF_SIZE             (258)                                   /**< Key buffer size for HKDF. */

/**< Get the number of HKDF test vectors. */
#define TEST_VECTOR_HKDF_GET(i)  \
    NRF_SECTION_ITEM_GET(test_vector_hkdf_data, test_vector_hkdf_t, (i))

/**< Get the test vector reference from the array of test vectors. */
#define TEST_VECTOR_HKDF_COUNT   \
    NRF_SECTION_ITEM_COUNT(test_vector_hkdf_data, test_vector_hkdf_t)

static uint8_t                   m_hkdf_ikm_buf[KEY_BUF_SIZE];           /**< Buffer for storing the unhexified HKDF input key material data. */
static uint8_t                   m_hkdf_okm_buf[KEY_BUF_SIZE];           /**< Buffer for storing the generated  HKDF output key material data. */
static uint8_t                   m_hkdf_prk_buf[KEY_BUF_SIZE];           /**< Buffer for holding the unhexified HKDF pseudo-random key data. */
static uint8_t                   m_hkdf_salt_buf[KEY_BUF_SIZE];          /**< Buffer for storing the unhexified HKDF salt data. */
static uint8_t                   m_hkdf_info_buf[KEY_BUF_SIZE];          /**< Buffer for storing the unhexified HKDF info data. */
static uint8_t                   m_hkdf_expected_okm_buf[KEY_BUF_SIZE];  /**< Buffer for storing the unhexified HKDF expected output key material data. */

static uint8_t                 * p_hkdp_salt;
static uint8_t                 * p_hkdp_info;


/**@brief Function for running the test setup.
 */
ret_code_t setup_test_case_hkdf(void)
{
    return NRF_SUCCESS;
}


/**@brief Function for the test execution.
 */
ret_code_t exec_test_case_hkdf(test_info_t * p_test_info)
{
    uint32_t i;
    ret_code_t err_code;
    uint32_t hkdf_test_vector_count = TEST_VECTOR_HKDF_COUNT;
    size_t   ikm_len;
    size_t   okm_len;
    size_t   prk_len;
    size_t   salt_len;
    size_t   info_len;
    size_t   expected_okm_len;
    
    for (i = 0; i < hkdf_test_vector_count; i++)
    {
        test_vector_hkdf_t * p_test_vector = TEST_VECTOR_HKDF_GET(i);
        p_test_info->current_id++;

        // Reset buffers.
        memset(m_hkdf_ikm_buf,  0x00, sizeof(m_hkdf_ikm_buf));
        memset(m_hkdf_okm_buf,  0xFF, sizeof(m_hkdf_okm_buf));
        memset(m_hkdf_prk_buf,  0x00, sizeof(m_hkdf_prk_buf));
        memset(m_hkdf_salt_buf, 0x00, sizeof(m_hkdf_salt_buf));
        memset(m_hkdf_info_buf, 0x00, sizeof(m_hkdf_info_buf));
        memset(m_hkdf_expected_okm_buf,  0x00, sizeof(m_hkdf_expected_okm_buf));

        // Fetch and unhexify test vectors.
        ikm_len           = unhexify(m_hkdf_ikm_buf,  p_test_vector->p_ikm);
        prk_len           = unhexify(m_hkdf_prk_buf,  p_test_vector->p_prk);
        salt_len          = unhexify(m_hkdf_salt_buf, p_test_vector->p_salt);
        info_len          = unhexify(m_hkdf_info_buf, p_test_vector->p_info);
        expected_okm_len  = unhexify(m_hkdf_expected_okm_buf, p_test_vector->p_okm);
        okm_len           = expected_okm_len;

        p_hkdp_salt = (salt_len == 0) ? NULL : m_hkdf_salt_buf;
        p_hkdp_info = (info_len == 0) ? NULL : m_hkdf_info_buf;

        // Calculation of the HKDF extract and expand.
        start_time_measurement();
        err_code = nrf_crypto_hkdf_calculate(NULL,
                                             p_test_vector->p_hmac_info,
                                             m_hkdf_okm_buf, &okm_len,
                                             m_hkdf_ikm_buf, ikm_len,
                                             p_hkdp_salt, salt_len,
                                             p_hkdp_info, info_len,
                                             NRF_CRYPTO_HKDF_EXTRACT_AND_EXPAND);
        stop_time_measurement();

        // Verify the nrf_crypto_hkdf_calculate err_code.
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_hkdf_calculate extract and expand");

        // Verify the generated HKDF output key material.
        TEST_VECTOR_ASSERT((expected_okm_len == okm_len), "Incorrect hkdf okm length");
        TEST_VECTOR_MEMCMP_ASSERT(m_hkdf_okm_buf,
                                  m_hkdf_expected_okm_buf,
                                  expected_okm_len,
                                  p_test_vector->expected_result,
                                  "Incorrect hkdf on extract and expand");

        // Verify that the next two bytes in buffer are not overwritten.
        TEST_VECTOR_OVERFLOW_ASSERT(m_hkdf_okm_buf, okm_len, "OKM buffer overflow");

        memset(m_hkdf_okm_buf,  0xFF, sizeof(m_hkdf_okm_buf));

        // Calculation of the HKDF expand only.
        err_code = nrf_crypto_hkdf_calculate(NULL,
                                             p_test_vector->p_hmac_info,
                                             m_hkdf_okm_buf, &okm_len,
                                             m_hkdf_prk_buf, prk_len,
                                             p_hkdp_salt, salt_len,
                                             p_hkdp_info, info_len,
                                             NRF_CRYPTO_HKDF_EXPAND_ONLY);

        // Verify the nrf_crypto_hkdf_calculate err_code.
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_hkdf_calculate expand");

        // Verify the generated HKDF output key material.
        TEST_VECTOR_ASSERT((expected_okm_len == okm_len), "Incorrect hkdf okm length");
        TEST_VECTOR_MEMCMP_ASSERT(m_hkdf_okm_buf,
                                  m_hkdf_expected_okm_buf,
                                  expected_okm_len,
                                  p_test_vector->expected_result,
                                  "Incorrect hkdf on expand");

        // Verify that the next two bytes in buffer are not overwritten.
        TEST_VECTOR_OVERFLOW_ASSERT(m_hkdf_okm_buf, okm_len, "OKM buffer overflow");

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
ret_code_t teardown_test_case_hkdf(void)
{
    return NRF_SUCCESS;
}


/** @brief  Macro for registering the ECDH test case by using section variables.
 *
 * @details     This macro places a variable in a section named "test_case_data",
 *              which is initialized by main.
 */
NRF_SECTION_ITEM_REGISTER(test_case_data, test_case_t test_hkdf) =
{
    .p_test_case_name = "HKDF",
    .setup = setup_test_case_hkdf,
    .exec = exec_test_case_hkdf,
    .teardown = teardown_test_case_hkdf
};

#endif // NRF_CRYPTO_HMAC_ENABLED
