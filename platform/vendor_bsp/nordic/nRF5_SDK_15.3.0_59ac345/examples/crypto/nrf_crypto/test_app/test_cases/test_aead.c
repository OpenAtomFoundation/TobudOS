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
#include "nrf_crypto_aead.h"


#if NRF_MODULE_ENABLED(NRF_CRYPTO_AEAD)

NRF_SECTION_DEF(test_vector_aead_data, test_vector_aead_t);
NRF_SECTION_DEF(test_vector_aead_simple_data, test_vector_aead_t);

#define NUM_BUFFER_OVERFLOW_TEST_BYTES (2)                                                      /**< Number of bytes to be used in the overflow test for AEAD. */
#define AEAD_MAC_SIZE                  (16)                                                     /**< Size of the AEAD MAC. */
#define AEAD_MAX_TESTED_NONCE_SIZE     (128)                                                    /**< Input buffer size for the AEAD noce. */
#define AEAD_PLAINTEXT_BUF_SIZE        (265)                                                    /**< Input buffer size for the AEAD plaintext. */
#define AEAD_PLAINTEXT_BUF_SIZE_PLUS   AEAD_PLAINTEXT_BUF_SIZE + NUM_BUFFER_OVERFLOW_TEST_BYTES /**< Input buffer size for the AEAD plaintext, including 2 buffer overflow bytes. */
#define AEAD_MAX_MAC_SIZE_PLUS         AEAD_MAC_SIZE + NUM_BUFFER_OVERFLOW_TEST_BYTES           /**< Input buffer size for the AEAD MAC, including 2 buffer overflow bytes. */
#define AEAD_KEY_SIZE                  NRF_CRYPTO_KEY_SIZE_256 / (8)                            /**< Input buffer size for the AEAD key. */

/**< Get number of the AEAD test vectors. */
#define TEST_VECTOR_AEAD_GET(i)         \
    NRF_SECTION_ITEM_GET(test_vector_aead_data, test_vector_aead_t, (i))

/**< Get the AEAD test vector reference from the array of test vectors. */
#define TEST_VECTOR_AEAD_COUNT          \
    NRF_SECTION_ITEM_COUNT(test_vector_aead_data, test_vector_aead_t)

/**< Get number of the simple AEAD test vectors. */
#define TEST_VECTOR_AEAD_INVALID_GET(i) \
    NRF_SECTION_ITEM_GET(test_vector_aead_simple_data, test_vector_aead_t, (i))

/**< Get the AEAD simple test vector reference from the array of test vectors. */
#define TEST_VECTOR_AEAD_INVALID_COUNT  \
    NRF_SECTION_ITEM_COUNT(test_vector_aead_simple_data, test_vector_aead_t)

static nrf_crypto_aead_context_t m_aead_context;                                                /**< AEAD context. */

static uint8_t m_aead_input_buf[AEAD_PLAINTEXT_BUF_SIZE];                                       /**< Buffer for holding the AEAD input plaintext or ciphertext. */
static uint8_t m_aead_output_buf[AEAD_PLAINTEXT_BUF_SIZE_PLUS];                                 /**< Buffer for holding the AEAD output plaintext or ciphertext. */
static uint8_t m_aead_expected_output_buf[AEAD_PLAINTEXT_BUF_SIZE];                             /**< Buffer for holding the AEAD expected plaintext or ciphertext. */
static uint8_t m_aead_output_mac_buf[AEAD_MAX_MAC_SIZE_PLUS];                                   /**< Buffer for holding the AEAD generated MAC. */
static uint8_t m_aead_expected_mac_buf[AEAD_MAX_MAC_SIZE_PLUS];                                 /**< Buffer for holding the AEAD expected MAC. */
static uint8_t m_aead_key_buf[AEAD_KEY_SIZE];                                                   /**< Buffer for holding the AEAD key. */
static uint8_t m_aead_ad_buf[AEAD_PLAINTEXT_BUF_SIZE];                                          /**< Buffer for holding the AEAD associated data. */
static uint8_t m_aead_nonce_buf[AEAD_MAX_TESTED_NONCE_SIZE];                                    /**< Buffer for holding the AEAD nonce data. */


/**@brief Function for running the test setup.
 */
ret_code_t setup_test_case_aead(void)
{
    return NRF_SUCCESS;
}


/**@brief Function for the AEAD test execution.
 */
ret_code_t exec_test_case_aead(test_info_t * p_test_info)
{
    uint32_t i;
    ret_code_t err_code;
    uint32_t aead_test_vector_count;
    size_t   ad_len;
    size_t   mac_len;
    size_t   nonce_len;
    uint32_t input_len;
    size_t   output_len;

    aead_test_vector_count = TEST_VECTOR_AEAD_COUNT;

    for (i = 0; i < aead_test_vector_count; i++)
    {
        test_vector_aead_t * p_test_vector = TEST_VECTOR_AEAD_GET(i);
        p_test_info->current_id++;
    
        // Reset buffers and fetch test vectors.
        memset(m_aead_input_buf, 0xFF, sizeof(m_aead_input_buf));
        memset(m_aead_expected_output_buf, 0xFF, sizeof(m_aead_expected_output_buf));
        memset(m_aead_output_buf, 0xFF, sizeof(m_aead_output_buf));
        memset(m_aead_output_mac_buf, 0xFF, sizeof(m_aead_output_mac_buf));
        memset(m_aead_expected_mac_buf, 0xFF, sizeof(m_aead_expected_mac_buf));
        memset(m_aead_key_buf, 0x00, sizeof(m_aead_key_buf));
        memset(m_aead_ad_buf, 0x00, sizeof(m_aead_ad_buf));
        memset(m_aead_nonce_buf, 0x00, sizeof(m_aead_nonce_buf));

        (void)unhexify(m_aead_key_buf, p_test_vector->p_key);
        mac_len   = unhexify(m_aead_expected_mac_buf, p_test_vector->p_mac);
        ad_len    = unhexify(m_aead_ad_buf, p_test_vector->p_ad);
        nonce_len = unhexify(m_aead_nonce_buf, p_test_vector->p_nonce);

        // Fetch and unhexify plaintext and ciphertext for encryption.
        input_len  = unhexify(m_aead_input_buf, p_test_vector->p_plaintext);
        output_len = unhexify(m_aead_expected_output_buf, p_test_vector->p_ciphertext);

        // Initialize AEAD.
        err_code = nrf_crypto_aead_init(&m_aead_context,
                                        p_test_vector->p_aead_info,
                                        m_aead_key_buf);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == NRF_SUCCESS), "nrf_crypto_aead_init");

        // Encrypt plaintext.
        start_time_measurement();
        err_code = nrf_crypto_aead_crypt(&m_aead_context,
                                         NRF_CRYPTO_ENCRYPT,
                                         m_aead_nonce_buf,
                                         nonce_len,
                                         m_aead_ad_buf,
                                         ad_len,
                                         m_aead_input_buf,
                                         input_len,
                                         m_aead_output_buf,
                                         m_aead_output_mac_buf,
                                         mac_len);
        stop_time_measurement();

        // Verify the nrf_crypto_aead_crypt err_code.
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_aead_crypt on encryption");

        // Verify the generated AEAD ciphertext.
        TEST_VECTOR_MEMCMP_ASSERT(m_aead_expected_output_buf,
                                  m_aead_output_buf,
                                  output_len,
                                  p_test_vector->crypt_expected_result,
                                  "Incorrect generated AEAD ciphertext");

        // Verify the generated AEAD MAC.
        TEST_VECTOR_MEMCMP_ASSERT(m_aead_expected_mac_buf,
                                  m_aead_output_mac_buf,
                                  mac_len,
                                  p_test_vector->mac_expected_result,
                                  "Incorrect generated AEAD MAC on encryption");

        // Verify that the next two bytes in buffers are not overwritten.
        TEST_VECTOR_OVERFLOW_ASSERT(m_aead_output_buf, output_len, "output buffer overflow");
        TEST_VECTOR_OVERFLOW_ASSERT(m_aead_output_mac_buf, mac_len, "MAC buffer overflow");

        // Fetch and unhexify plaintext and ciphertext for decryption.
        output_len = unhexify(m_aead_expected_output_buf, p_test_vector->p_plaintext);
        input_len  = unhexify(m_aead_input_buf, p_test_vector->p_ciphertext);

        // Decrypt ciphertext. MAC will be verified in the lib.
        err_code = nrf_crypto_aead_crypt(&m_aead_context,
                                         NRF_CRYPTO_DECRYPT,
                                         m_aead_nonce_buf,
                                         nonce_len,
                                         m_aead_ad_buf,
                                         ad_len,
                                         m_aead_input_buf,
                                         input_len,
                                         m_aead_output_buf,
                                         m_aead_output_mac_buf,
                                         mac_len);

        // Verify the nrf_crypto_aead_crypt err_code.
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_aead_crypt on decryption");

        // Verify the generated AEAD plaintext.
        TEST_VECTOR_MEMCMP_ASSERT(m_aead_expected_output_buf,
                                  m_aead_output_buf,
                                  output_len,
                                  p_test_vector->crypt_expected_result,
                                  "Incorrect generated AEAD plaintext");

        // Verify that the next two bytes in buffers are not overwritten.
        TEST_VECTOR_OVERFLOW_ASSERT(m_aead_output_buf, output_len, "output buffer overflow");
        TEST_VECTOR_OVERFLOW_ASSERT(m_aead_output_mac_buf, mac_len, "MAC buffer overflow");

        NRF_LOG_INFO("#%04d Test vector passed: %s %s",
                     p_test_info->current_id,
                     p_test_info->p_test_case_name,
                     p_test_vector->p_test_vector_name);

        p_test_info->tests_passed++;

exit_test_vector:

        (void) nrf_crypto_aead_uninit(&m_aead_context);

        while (NRF_LOG_PROCESS());

    }
    return NRF_SUCCESS;
}


/**@brief Function for the AEAD test execution.
 */
ret_code_t exec_test_case_aead_simple(test_info_t * p_test_info)
{
    uint32_t i;
    ret_code_t err_code;
    uint32_t aead_test_vector_count;
    size_t   ad_len;
    size_t   mac_len;
    size_t   nonce_len;
    uint32_t input_len;
    uint32_t output_len;

    aead_test_vector_count = TEST_VECTOR_AEAD_INVALID_COUNT;

    for (i = 0; i < aead_test_vector_count; i++)
    {
        test_vector_aead_t * p_test_vector = TEST_VECTOR_AEAD_INVALID_GET(i);
        p_test_info->current_id++;
    
        // Reset buffers and fetch test vectors.
        memset(m_aead_input_buf, 0xFF, sizeof(m_aead_input_buf));
        memset(m_aead_expected_output_buf, 0xFF, sizeof(m_aead_expected_output_buf));
        memset(m_aead_output_buf, 0xFF, sizeof(m_aead_output_buf));
        memset(m_aead_key_buf, 0x00, sizeof(m_aead_key_buf));
        memset(m_aead_ad_buf, 0x00, sizeof(m_aead_ad_buf));
        memset(m_aead_nonce_buf, 0x00, sizeof(m_aead_nonce_buf));

        (void)unhexify(m_aead_key_buf, p_test_vector->p_key);
        ad_len    = unhexify(m_aead_ad_buf, p_test_vector->p_ad);
        nonce_len = unhexify(m_aead_nonce_buf, p_test_vector->p_nonce);

        if (p_test_vector->direction == NRF_CRYPTO_ENCRYPT)
        {
            // Fetch and unhexify plaintext and ciphertext for encryption.
            input_len  = unhexify(m_aead_input_buf, p_test_vector->p_plaintext);
            output_len = unhexify(m_aead_expected_output_buf, p_test_vector->p_ciphertext);
            
            memset(m_aead_output_mac_buf, 0xFF, sizeof(m_aead_output_mac_buf));
            memset(m_aead_expected_mac_buf, 0xFF, sizeof(m_aead_expected_mac_buf));
            mac_len = unhexify(m_aead_expected_mac_buf, p_test_vector->p_mac);
        }
        else
        {
            // Fetch and unhexify plaintext and ciphertext for decryption.
            input_len  = unhexify(m_aead_input_buf, p_test_vector->p_ciphertext);
            output_len = unhexify(m_aead_expected_output_buf, p_test_vector->p_plaintext);
            
            memset(m_aead_output_mac_buf, 0xFF, sizeof(m_aead_output_mac_buf));
            mac_len = unhexify(m_aead_output_mac_buf, p_test_vector->p_mac);
        }

        // Initialize AEAD.
        err_code = nrf_crypto_aead_init(&m_aead_context,
                                        p_test_vector->p_aead_info,
                                        m_aead_key_buf);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == NRF_SUCCESS), "nrf_crypto_aead_init");

        // Encrypt or decrypt input.
        start_time_measurement();
        err_code = nrf_crypto_aead_crypt(&m_aead_context,
                                         p_test_vector->direction,
                                         m_aead_nonce_buf,
                                         nonce_len,
                                         m_aead_ad_buf,
                                         ad_len,
                                         m_aead_input_buf,
                                         input_len,
                                         m_aead_output_buf,
                                         m_aead_output_mac_buf,
                                         mac_len);
        stop_time_measurement();

        // Verify the nrf_crypto_aead_crypt err_code.
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_aead_crypt");
        if (input_len != 0)
        {
            // Verify the generated AEAD plaintext or ciphertext.
            TEST_VECTOR_MEMCMP_ASSERT(m_aead_expected_output_buf,
                                      m_aead_output_buf,
                                      output_len,
                                      p_test_vector->crypt_expected_result,
                                      "Incorrect generated AEAD plaintext/ciphertext");
        }
        if (p_test_vector->direction == NRF_CRYPTO_ENCRYPT)
        {
            // Verify the generated AEAD MAC.
            TEST_VECTOR_MEMCMP_ASSERT(m_aead_expected_mac_buf,
                                      m_aead_output_mac_buf,
                                      mac_len,
                                      p_test_vector->mac_expected_result,
                                      "Incorrect generated AEAD MAC on encryption");
        }

        NRF_LOG_INFO("#%04d Test vector passed: %s %s",
                     p_test_info->current_id,
                     p_test_info->p_test_case_name,
                     p_test_vector->p_test_vector_name);

        p_test_info->tests_passed++;
        
exit_test_vector:

        (void) nrf_crypto_aead_uninit(&m_aead_context);

        while (NRF_LOG_PROCESS());
    }
    return NRF_SUCCESS;
}

/**@brief Function for running the test teardown.
 */
ret_code_t teardown_test_case_aead(void)
{
    return NRF_SUCCESS;
}

/** @brief  Macro for registering the AEAD test case by using section variables.
 *
 * @details     This macro places a variable in a section named "test_case_data",
 *              which is initialized by main.
 */
NRF_SECTION_ITEM_REGISTER(test_case_data, test_case_t test_aead) =
{
    .p_test_case_name = "AEAD",
    .setup = setup_test_case_aead,
    .exec = exec_test_case_aead,
    .teardown = teardown_test_case_aead
};

/** @brief  Macro for registering the AEAD test case by using section variables.
 *
 * @details     This macro places a variable in a section named "test_case_data",
 *              which is initialized by main.
 */
NRF_SECTION_ITEM_REGISTER(test_case_data, test_case_t test_aead_simple) =
{
    .p_test_case_name = "AEAD simple",
    .setup = setup_test_case_aead,
    .exec = exec_test_case_aead_simple,
    .teardown = teardown_test_case_aead
};

#endif // NRF_CRYPTO_AEAD_ENABLED
