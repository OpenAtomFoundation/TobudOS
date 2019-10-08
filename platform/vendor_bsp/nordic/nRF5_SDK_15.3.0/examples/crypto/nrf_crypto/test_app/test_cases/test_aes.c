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
#include "app_util.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_error.h"
#include "nrf_crypto.h"
#include "nrf_section.h"
#include "common_test.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_ctrl.h"
#include "nrf_log.h"


#if NRF_MODULE_ENABLED(NRF_CRYPTO_AES)

NRF_SECTION_DEF(test_vector_aes_data, test_vector_aes_t);
NRF_SECTION_DEF(test_vector_aes_mac_data, test_vector_aes_t);
NRF_SECTION_DEF(test_vector_aes_func_data, test_vector_aes_t);
NRF_SECTION_DEF(test_vector_aes_monte_carlo_data, test_vector_aes_t);

#define NUM_BUFFER_OVERFLOW_TEST_BYTES      (2)                                                     /**< Number of bytes to be used in overflow test for AES. */
#define CFB_MONTE_CARLO_CIPHER_ARRAY_LEN    (32)                                                    /**< Number of bytes in the AES CFB ciphertext array in the AES Monte Carlo test. */
#define AES_IV_MAX_SIZE                     (16)                                                    /**< Maximum AES Initialization Vector size. */
#define AES_MAC_INPUT_BLOCK_SIZE            (16)                                                    /**< Maximum AES MAC input block size. */
#define AES_PLAINTEXT_BUF_SIZE              (256)                                                   /**< Input buffer size for the AES plaintext. */
#define AES_PLAINTEXT_BUF_SIZE_PLUS         AES_PLAINTEXT_BUF_SIZE + NUM_BUFFER_OVERFLOW_TEST_BYTES /**< Input buffer size for AES plaintext, including 2 buffer overflow bytes. */
#define AES_MAX_KEY_SIZE                    NRF_CRYPTO_KEY_SIZE_256 / (8)                           /**< AES maximum key size. */
#define AES_MIN_KEY_SIZE                    NRF_CRYPTO_KEY_SIZE_128 / (8)                           /**< AES minimum key size. */

/**< Get number of the AES test vectors. */
#define TEST_VECTOR_AES_GET(i)              \
    NRF_SECTION_ITEM_GET(test_vector_aes_data, test_vector_aes_t, (i))

/**< Get the test vector reference from array of test vectors. */
#define TEST_VECTOR_AES_COUNT               \
    NRF_SECTION_ITEM_COUNT(test_vector_aes_data, test_vector_aes_t)

/**< Get the number of the AES MAC test vectors. */
#define TEST_VECTOR_AES_MAC_GET(i)          \
    NRF_SECTION_ITEM_GET(test_vector_aes_mac_data, test_vector_aes_t, (i))

/**< Get the test vector reference from array of test vectors. */
#define TEST_VECTOR_AES_MAC_COUNT           \
    NRF_SECTION_ITEM_COUNT(test_vector_aes_mac_data, test_vector_aes_t)

/**< Get the number of AES multi-test vectors. */
#define TEST_VECTOR_AES_MULTI_GET(i)        \
    NRF_SECTION_ITEM_GET(test_vector_aes_monte_carlo_data, test_vector_aes_t, (i))

/**< Get the test vector reference from the array of test vectors. */
#define TEST_VECTOR_AES_MULTI_COUNT         \
    NRF_SECTION_ITEM_COUNT(test_vector_aes_monte_carlo_data, test_vector_aes_t)

/**< Get the number of AES funtional test vectors. */
#define TEST_VECTOR_AES_FUNCTIONAL_GET(i)   \
    NRF_SECTION_ITEM_GET(test_vector_aes_func_data, test_vector_aes_t, (i))

/**< Get the test vector reference from the array of test vectors. */
#define TEST_VECTOR_AES_FUNCTIONAL_COUNT    \
    NRF_SECTION_ITEM_COUNT(test_vector_aes_func_data, test_vector_aes_t)

static nrf_crypto_aes_context_t m_aes_context;                                                 /**< AES context. */

static uint8_t m_aes_input_buf[AES_PLAINTEXT_BUF_SIZE_PLUS];                                   /**< Buffer for holding the plaintext. */
static uint8_t m_aes_output_buf[AES_PLAINTEXT_BUF_SIZE_PLUS];                                  /**< Buffer for holding the generated plaintext or ciphertext. */
static uint8_t m_aes_expected_output_buf[AES_PLAINTEXT_BUF_SIZE_PLUS];                         /**< Buffer for holding the ciphertext. */
static uint8_t m_prev_aes_output_buf[AES_PLAINTEXT_BUF_SIZE_PLUS];                             /**< Buffer for holding the previous generated ciphertext or plaintext. */
static uint8_t m_aes_key_buf[AES_MAX_KEY_SIZE];                                                /**< Buffer for holding the key data. */
static uint8_t m_aes_iv_buf[AES_IV_MAX_SIZE + NUM_BUFFER_OVERFLOW_TEST_BYTES];                 /**< Buffer for holding the IV data. */
static uint8_t m_aes_temp_buf[AES_MAX_KEY_SIZE];                                               /**< Buffer for holding the temporary data. */


/**@brief Function for updating the AES key buffer based on the algorithms
 *  in the NIST Monte Carlo test cases.
 *
 * @param[in]  key_len          Length of the AES key in bytes.
 * @param[in]  ciphertext_len   Length of the ciphertext in bytes.
 *
 */
void ecb_cbc_monte_carlo_update_key(size_t key_len, size_t ciphertext_len)
{
    uint8_t  divider;
    divider = key_len - ciphertext_len;

    // Xor previous cipher with key if key_len > cipher_len.
    for (uint8_t xor_start = 0; xor_start < divider; xor_start++)
    {
        m_aes_key_buf[xor_start] ^= m_prev_aes_output_buf[ciphertext_len - divider + xor_start];
    }

    // Xor cipher with last 16 bytes of key.
    for (uint8_t xor_start = 0; xor_start < ciphertext_len; xor_start++)
    {
        m_aes_key_buf[divider + xor_start] ^= m_aes_output_buf[xor_start];
    }
}

/**@brief Function for handling the AES ECB encryption or decryption in the NIST Monte Carlo test cases.
 *
 * @details Check out NIST The Advanced Encryption Standard Algorithm Validation Suite (AESAVS)
 *  for more information about the Monte Carlo algorithm.
 *
 * @param[in]  p_test_info      Pointer to the test case information variable.
 * @param[in]  p_test_vector    Pointer to the test vector information variable.
 * @param[in]  p_aes_context    Pointer to the AES context variable.
 * @param[in]  key_len          Length of the AES key in bytes.
 * @param[in]  input_len        Length of the AES input in bytes.
 * @param[in]  output_len       Length of the AES output in bytes.
 *
 * @return NRF_SUCCESS on success.
 */
ret_code_t ecb_monte_carlo(test_info_t *                  p_test_info,
                           test_vector_aes_t *            p_test_vector,
                           nrf_crypto_aes_context_t *     p_aes_context,
                           size_t                         key_len,
                           size_t                         input_len,
                           size_t                         output_len)
{
    uint16_t j;
    ret_code_t err_code;

    // Execution of encryption or decryption 1000 times with same AES key.
    for (j = 0; j < 1000; j++)
    {
        memcpy(m_prev_aes_output_buf, m_aes_output_buf, sizeof(m_prev_aes_output_buf));
        err_code = nrf_crypto_aes_update(p_aes_context,
                                         m_aes_input_buf,
                                         input_len,
                                         m_aes_output_buf);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_aes_blocks_crypt");
        memcpy(m_aes_input_buf, m_aes_output_buf, input_len);
    }

    // Update the AES key.
    ecb_cbc_monte_carlo_update_key(key_len, output_len);
    err_code = nrf_crypto_aes_key_set(p_aes_context, m_aes_key_buf);
    TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                "nrf_crypto_aes_key_set");

exit_test_vector:

        return err_code;
}


/**@brief Function for handling the AES CBC encryption or decryption in the NIST Monte Carlo test cases.
 *
 * @details Check out NIST The Advanced Encryption Standard Algorithm Validation Suite (AESAVS)
 *  for more information about the Monte Carlo algorithm.
 *
 * @param[in]  p_test_info      Pointer to the test case information variable.
 * @param[in]  p_test_vector    Pointer to the test vector information variable.
 * @param[in]  p_aes_context    Pointer to the AES context variable.
 * @param[in]  key_len          Length of the AES key in bytes.
 * @param[in]  input_len        Length of the AES input in bytes.
 * @param[in]  output_len       Length of the AES output in bytes.
 * @param[in]  iv_len           Length of the AES IV in bytes.
 *
 * @return NRF_SUCCESS on success.
 */
ret_code_t cbc_monte_carlo(test_info_t *                  p_test_info,
                           test_vector_aes_t *            p_test_vector,
                           nrf_crypto_aes_context_t *     p_aes_context,
                           size_t                         key_len,
                           size_t                         input_len,
                           size_t                         output_len,
                           size_t                         iv_len)
{
    uint16_t j;
    ret_code_t err_code;

    // Execution encryption or decryption 1000 times with same AES key.
    for (j = 0; j < 1000; j++)
    {
        memcpy(m_prev_aes_output_buf, m_aes_output_buf, sizeof(m_prev_aes_output_buf));
        err_code = nrf_crypto_aes_update(p_aes_context,
                                         m_aes_input_buf,
                                         input_len,
                                         m_aes_output_buf);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_aes_blocks_crypt");

        if (j == 0)
        {
            memcpy(m_aes_input_buf, m_aes_iv_buf, input_len);
        }
        else
        {
            memcpy(m_aes_input_buf, m_prev_aes_output_buf, input_len);
        }
    }

    // Update the AES key.
    ecb_cbc_monte_carlo_update_key(key_len, output_len);
    err_code = nrf_crypto_aes_key_set(p_aes_context, m_aes_key_buf);
    TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                "nrf_crypto_aes_key_set");

    // Update the AES IV.
    memcpy(m_aes_iv_buf, m_aes_output_buf, iv_len);
    err_code = nrf_crypto_aes_iv_set(p_aes_context, m_aes_iv_buf);
    TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                "nrf_crypto_aes_iv_set");

exit_test_vector:

    return err_code;
}


/**@brief Function for handling the AES CFB8 encryption or decryption in the NIST Monte Carlo test cases.
 *
 * @details Check out NIST The Advanced Encryption Standard Algorithm Validation Suite (AESAVS)
 *  for more information about the Monte Carlo algorithm.
 *
 * @param[in]  p_test_info      Pointer to the test case information variable.
 * @param[in]  p_test_vector    Pointer to the test vector information variable.
 * @param[in]  p_aes_context    Pointer to the AES context variable.
 * @param[in]  key_len          Length of the AES key in bytes.
 * @param[in]  input_len        Length of the AES input in bytes.
 * @param[in]  output_len       Length of the AES output in bytes.
 * @param[in]  iv_len           Length of the AES IV in bytes.
 *
 * @return NRF_SUCCESS on success.
 */
ret_code_t cfb8_monte_carlo(test_info_t *                  p_test_info,
                            test_vector_aes_t *            p_test_vector,
                            nrf_crypto_aes_context_t *     p_aes_context,
                            size_t                         key_len,
                            size_t                         input_len,
                            size_t                         output_len,
                            size_t                         iv_len)
{
    uint16_t j;
    ret_code_t err_code;
    uint8_t  oldest_cipher_index;
    uint8_t  current_cipher_index;
    uint8_t  remaining_cipher_elements;
    uint8_t  cipher_array[CFB_MONTE_CARLO_CIPHER_ARRAY_LEN] = {0};

    if ((key_len < AES_MIN_KEY_SIZE) || (key_len > AES_MAX_KEY_SIZE))
    {
        return NRF_ERROR_CRYPTO_KEY_SIZE;
    }

    // Execution encryption or decryption 1000 times with same AES key.
    for (j = 0; j < 1000; j++)
    {
        err_code = nrf_crypto_aes_update(p_aes_context,
                                         m_aes_input_buf,
                                         input_len,
                                         m_aes_output_buf);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_aes_update");

        if (j < 16)
        {
            m_aes_input_buf[0] = m_aes_iv_buf[j];
        }
        else
        {
            m_aes_input_buf[0] = cipher_array[(j - 16) % CFB_MONTE_CARLO_CIPHER_ARRAY_LEN];
        }

        cipher_array[j % CFB_MONTE_CARLO_CIPHER_ARRAY_LEN] = m_aes_output_buf[0];
    }

    // Variables needed for the AES key calculation.
    current_cipher_index      = (j - 1) % CFB_MONTE_CARLO_CIPHER_ARRAY_LEN;
    oldest_cipher_index       = (current_cipher_index - key_len + 1)
                                % CFB_MONTE_CARLO_CIPHER_ARRAY_LEN;
    remaining_cipher_elements = CFB_MONTE_CARLO_CIPHER_ARRAY_LEN - oldest_cipher_index;

    // Use m_aes_temp_buf as temporary buffer.
    memcpy(m_aes_temp_buf,
           &cipher_array[oldest_cipher_index],
           remaining_cipher_elements);
    memcpy(&m_aes_temp_buf[remaining_cipher_elements],
           cipher_array,
           key_len - remaining_cipher_elements);

    // Xor key with ciphers.
    for (uint8_t xor_start = 0; xor_start < key_len; xor_start++)
    {
        m_aes_key_buf[xor_start] ^= m_aes_temp_buf[xor_start];
    }

    // Update the AES key.
    err_code = nrf_crypto_aes_key_set(p_aes_context, m_aes_key_buf);
    TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                "nrf_crypto_aes_key_set");

    // Update the AES IV.
    /*lint -save -e662 */
    memcpy(m_aes_iv_buf, &m_aes_temp_buf[key_len - 16], iv_len);
    err_code = nrf_crypto_aes_iv_set(p_aes_context, m_aes_iv_buf);
    TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                "nrf_crypto_aes_iv_set");

exit_test_vector:

    return err_code;
}


/**@brief Function for running the test setup.
 */
ret_code_t setup_test_case_aes(void)
{
    return NRF_SUCCESS;
}


/**@brief Function for the AES functional test execution.
 */
ret_code_t exec_test_case_aes_functional(test_info_t * p_test_info)
{
    uint32_t i;
    ret_code_t err_code;
    uint32_t aes_test_vector_count;
    size_t   iv_len;
    size_t   input_len;
    size_t   output_len;

    aes_test_vector_count = TEST_VECTOR_AES_FUNCTIONAL_COUNT;

    for (i = 0; i < aes_test_vector_count; i++)
    {
        test_vector_aes_t * p_test_vector = TEST_VECTOR_AES_FUNCTIONAL_GET(i);
        p_test_info->current_id++;

        // Reset buffers and fetch test vectors.
        memset(m_aes_input_buf, 0xFF, sizeof(m_aes_input_buf));
        memset(m_aes_output_buf, 0xFF, sizeof(m_aes_output_buf));
        memset(m_aes_expected_output_buf, 0x00, sizeof(m_aes_expected_output_buf));

        input_len  = unhexify(m_aes_input_buf, p_test_vector->p_plaintext);
        output_len = unhexify(m_aes_expected_output_buf, p_test_vector->p_ciphertext);

        // Initialize AES to encrypt.
        err_code = nrf_crypto_aes_init(&m_aes_context,
                                       p_test_vector->p_aes_info,
                                       NRF_CRYPTO_ENCRYPT);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == NRF_SUCCESS),
                                    "nrf_crypto_aes_init on encrypt");

        // Fetch and set the AES key.
        memset(m_aes_key_buf, 0x00, sizeof(m_aes_key_buf));
        (void)unhexify(m_aes_key_buf, p_test_vector->p_key);
        err_code = nrf_crypto_aes_key_set(&m_aes_context, m_aes_key_buf);

        // Fetch and set the AES IV.
        memset(m_aes_iv_buf, 0x00, sizeof(m_aes_iv_buf));
        iv_len = unhexify(m_aes_iv_buf, p_test_vector->p_iv);
        if (iv_len != 0)
        {
            err_code = nrf_crypto_aes_iv_set(&m_aes_context, m_aes_iv_buf);
        }

        // Encrypt input plaintext.
        start_time_measurement();
        err_code = nrf_crypto_aes_finalize(&m_aes_context,
                                           m_aes_input_buf,
                                           input_len,
                                           m_aes_output_buf,
                                           &output_len);
        stop_time_measurement();

        // Verify the nrf_crypto_aes_finalize err_code.
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_aes_finalize on encrypt");

        // Verify the generated AES ciphertext.
        TEST_VECTOR_MEMCMP_ASSERT(m_aes_expected_output_buf,
                                  m_aes_output_buf,
                                  output_len,
                                  p_test_vector->expected_result,
                                  "Incorrect generated AES ciphertext");

        // Verify that the next two bytes in buffers are not overwritten.
        TEST_VECTOR_OVERFLOW_ASSERT(m_aes_output_buf, output_len, "output buffer overflow");
        TEST_VECTOR_OVERFLOW_ASSERT(m_aes_input_buf, input_len, "input buffer overflow");

        // Reset buffers and re-fetch the test vectors.
        memset(m_aes_input_buf, 0xFF, sizeof(m_aes_input_buf));
        memset(m_aes_output_buf, 0xFF, sizeof(m_aes_output_buf));
        memset(m_aes_expected_output_buf, 0x00, sizeof(m_aes_expected_output_buf));

        // Fetch and unhexify plaintext and ciphertext.
        input_len  = unhexify(m_aes_input_buf, p_test_vector->p_ciphertext);
        output_len = unhexify(m_aes_expected_output_buf, p_test_vector->p_plaintext);

        // Initialize AES to decrypt.
        err_code = nrf_crypto_aes_init(&m_aes_context,
                                       p_test_vector->p_aes_info,
                                       NRF_CRYPTO_DECRYPT);

        TEST_VECTOR_ASSERT_ERR_CODE((err_code == NRF_SUCCESS),
                                    "nrf_crypto_aes_init on decrypt");

        // Fetch and set the AES key.
        memset(m_aes_key_buf, 0x00, sizeof(m_aes_key_buf));
        (void)unhexify(m_aes_key_buf, p_test_vector->p_key);
        err_code = nrf_crypto_aes_key_set(&m_aes_context, m_aes_key_buf);

        // Fetch and set the AES IV.
        memset(m_aes_iv_buf, 0x00, sizeof(m_aes_iv_buf));
        iv_len = unhexify(m_aes_iv_buf, p_test_vector->p_iv);
        if (iv_len != 0)
        {
            err_code = nrf_crypto_aes_iv_set(&m_aes_context, m_aes_iv_buf);
        }

        // Decrypt input ciphertext.
        err_code = nrf_crypto_aes_finalize(&m_aes_context,
                                           m_aes_input_buf,
                                           input_len,
                                           m_aes_output_buf,
                                           &output_len);

        // Verify the nrf_crypto_aes_finalize err_code.
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_aes_finalize on decrypt");

        // Verify the generated AES plaintext.
        TEST_VECTOR_MEMCMP_ASSERT(m_aes_expected_output_buf,
                                  m_aes_output_buf,
                                  output_len,
                                  p_test_vector->expected_result,
                                  "Incorrect generated AES plaintext");

        // Verify that the next two bytes in buffers are not overwritten.
        TEST_VECTOR_OVERFLOW_ASSERT(m_aes_output_buf, output_len, "output buffer overflow");
        TEST_VECTOR_OVERFLOW_ASSERT(m_aes_input_buf, input_len, "input buffer overflow");

        NRF_LOG_INFO("#%04d Test vector passed: %s %s",
                     p_test_info->current_id,
                     p_test_info->p_test_case_name,
                     p_test_vector->p_test_vector_name);

        p_test_info->tests_passed++;

exit_test_vector:

        // Un-initialize AES.
        (void) nrf_crypto_aes_uninit(&m_aes_context);

        while (NRF_LOG_PROCESS());

    }
    return NRF_SUCCESS;
}

/**@brief Function for the AES test execution.
 */
ret_code_t exec_test_case_aes(test_info_t * p_test_info)
{
    uint32_t i;
    ret_code_t err_code;
    uint32_t aes_test_vector_count;
    size_t   iv_len;
    size_t   ad_len;
    size_t   input_len;
    size_t   output_len;

    aes_test_vector_count = TEST_VECTOR_AES_COUNT;

    for (i = 0; i < aes_test_vector_count; i++)
    {
        test_vector_aes_t * p_test_vector = TEST_VECTOR_AES_GET(i);
        p_test_info->current_id++;

        // Reset buffers and fetch test vectors.
        memset(m_aes_input_buf, 0xFF, sizeof(m_aes_input_buf));
        memset(m_aes_expected_output_buf, 0xFF, sizeof(m_aes_expected_output_buf));
        memset(m_aes_output_buf, 0x00, sizeof(m_aes_output_buf));

        // Fetch and unhexify plaintext and ciphertext.
        if (p_test_vector->direction == NRF_CRYPTO_ENCRYPT)
        {
            input_len  = unhexify(m_aes_input_buf, p_test_vector->p_plaintext);
            output_len = unhexify(m_aes_expected_output_buf, p_test_vector->p_ciphertext);
        }
        else
        {
            output_len = unhexify(m_aes_expected_output_buf, p_test_vector->p_plaintext);
            input_len  = unhexify(m_aes_input_buf, p_test_vector->p_ciphertext);
        }

        // Initialize AES.
        err_code = nrf_crypto_aes_init(&m_aes_context,
                                       p_test_vector->p_aes_info,
                                       p_test_vector->direction);

        // Fetch and set the AES key.
        memset(m_aes_key_buf, 0x00, sizeof(m_aes_key_buf));
        (void)unhexify(m_aes_key_buf, p_test_vector->p_key);
        err_code = nrf_crypto_aes_key_set(&m_aes_context, m_aes_key_buf);

        // Fetch and set the AES IV.
        memset(m_aes_iv_buf, 0xFF, sizeof(m_aes_iv_buf));
        iv_len = unhexify(m_aes_iv_buf, p_test_vector->p_iv);
        if (iv_len != 0)
        {
            err_code = nrf_crypto_aes_iv_set(&m_aes_context, m_aes_iv_buf);
            TEST_VECTOR_ASSERT_ERR_CODE((err_code == NRF_SUCCESS),
                                        "nrf_crypto_aes_iv_set");
        }

        // Encrypt or decrypt input.
        start_time_measurement();
        err_code = nrf_crypto_aes_finalize(&m_aes_context,
                                           m_aes_input_buf,
                                           input_len,
                                           m_aes_output_buf,
                                           &output_len);
        stop_time_measurement();

        // Verify the nrf_crypto_aes_finalize err_code.
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_aes_finalize");

        // Verify the generated AES ciphertext.
        TEST_VECTOR_MEMCMP_ASSERT(m_aes_expected_output_buf,
                                  m_aes_output_buf,
                                  input_len,
                                  p_test_vector->expected_result,
                                  "Incorrect generated AES ciphertext");

        if (p_test_vector->p_aes_info->mode == NRF_CRYPTO_AES_MODE_CTR)
        {
            // Read the current counter value for the AES CTR mode.
            memset(m_aes_temp_buf, 0xFF, sizeof(m_aes_temp_buf));
            ad_len = unhexify(m_aes_temp_buf, p_test_vector->p_ad);
            err_code = nrf_crypto_aes_iv_get(&m_aes_context, m_aes_iv_buf);
            TEST_VECTOR_ASSERT_ERR_CODE((err_code == NRF_SUCCESS), "nrf_crypto_aes_iv_get");

            // Verify that the counter value has incremented as expected.
            TEST_VECTOR_MEMCMP_ASSERT(m_aes_iv_buf,
                                      m_aes_temp_buf,
                                      ad_len,
                                      NRF_SUCCESS,
                                      "Incorrect incremented counter value.");

            // Verify that the next two bytes in buffers are not overwritten.
            TEST_VECTOR_OVERFLOW_ASSERT(m_aes_iv_buf, iv_len, "IV buffer overflow");
        }

        NRF_LOG_INFO("#%04d Test vector passed: %s %s",
                     p_test_info->current_id,
                     p_test_info->p_test_case_name,
                     p_test_vector->p_test_vector_name);
        p_test_info->tests_passed++;

exit_test_vector:

        // Un-initialize AES.
        (void) nrf_crypto_aes_uninit(&m_aes_context);

        while (NRF_LOG_PROCESS());

    }
    return NRF_SUCCESS;
}


/**@brief Function for the AES MAC test execution.
 */
ret_code_t exec_test_case_aes_mac(test_info_t * p_test_info)
{
    uint32_t i;
    ret_code_t err_code;
    uint32_t aes_test_vector_count;
    size_t   input_len;
    size_t   output_len;
    size_t   input_remaining;

    aes_test_vector_count = TEST_VECTOR_AES_MAC_COUNT;

    for (i = 0; i < aes_test_vector_count; i++)
    {
        test_vector_aes_t * p_test_vector = TEST_VECTOR_AES_MAC_GET(i);
        p_test_info->current_id++;

        // Reset buffers and fetch test vectors.
        memset(m_aes_input_buf, 0xFF, sizeof(m_aes_input_buf));
        memset(m_aes_expected_output_buf, 0xFF, sizeof(m_aes_expected_output_buf));
        memset(m_aes_output_buf, 0x00, sizeof(m_aes_output_buf));

        output_len = 16;
        input_len = unhexify(m_aes_input_buf, p_test_vector->p_plaintext);
        (void)unhexify(m_aes_expected_output_buf, p_test_vector->p_ciphertext);

        // Initialize AES.
        err_code = nrf_crypto_aes_init(&m_aes_context,
                                       p_test_vector->p_aes_info,
                                       NRF_CRYPTO_MAC_CALCULATE);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == NRF_SUCCESS), "nrf_crypto_aes_init");

        // Fetch and set the AES key.
        memset(m_aes_key_buf, 0x00, sizeof(m_aes_key_buf));
        (void)unhexify(m_aes_key_buf, p_test_vector->p_key);
        err_code = nrf_crypto_aes_key_set(&m_aes_context, m_aes_key_buf);
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == NRF_SUCCESS), "nrf_crypto_aes_key_set");

        // Do quick AES MAC calculation.
        start_time_measurement();
        err_code = nrf_crypto_aes_finalize(&m_aes_context,
                                           m_aes_input_buf,
                                           input_len,
                                           m_aes_output_buf,
                                           &output_len);
        stop_time_measurement();

        // Verify the nrf_crypto_aes_mac_calculate err_code.
        TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                    "nrf_crypto_aes_finalize");

        // Verify the generated AES MAC.
        TEST_VECTOR_ASSERT((output_len == 16), "output_len is not 16");
        TEST_VECTOR_MEMCMP_ASSERT(m_aes_expected_output_buf,
                                  m_aes_output_buf,
                                  output_len,
                                  p_test_vector->expected_result,
                                  "Incorrect generated AES MAC");

        // Also, perform a multi update MAC calculation.
        if (input_len > 0)
        {
            size_t buffer_index;
            // Execute the same test vector again, in blocks of 16 bytes.
            input_remaining = input_len;
            memset(m_aes_output_buf, 0x00, sizeof(m_aes_output_buf));

            // Initialize AES.
            err_code = nrf_crypto_aes_init(&m_aes_context,
                                           p_test_vector->p_aes_info,
                                           NRF_CRYPTO_MAC_CALCULATE);
            TEST_VECTOR_ASSERT_ERR_CODE((err_code == NRF_SUCCESS),
                                        "nrf_crypto_aes_init multi");

            // Set the AES key.
            err_code = nrf_crypto_aes_key_set(&m_aes_context, m_aes_key_buf);
            TEST_VECTOR_ASSERT_ERR_CODE((err_code == NRF_SUCCESS),
                                        "nrf_crypto_aes_key_set multi");

            while (input_remaining)
            {
                buffer_index = input_len - input_remaining;
                if (input_remaining <= AES_MAC_INPUT_BLOCK_SIZE)
                {
                    // Update with the final input and calculate MAC.
                    output_len = AES_MAC_INPUT_BLOCK_SIZE;
                    err_code   = nrf_crypto_aes_finalize(&m_aes_context,
                                                         &m_aes_input_buf[buffer_index],
                                                         input_remaining,
                                                         m_aes_output_buf,
                                                         &output_len);
                    input_remaining = 0;
                }
                else
                {
                    // Update with the 16-byte input.
                    output_len = 0;
                    err_code   = nrf_crypto_aes_update(&m_aes_context,
                                                       &m_aes_input_buf[buffer_index],
                                                       AES_MAC_INPUT_BLOCK_SIZE,                                        
                                                       m_aes_output_buf);
                    input_remaining -= AES_MAC_INPUT_BLOCK_SIZE;
                }
                TEST_VECTOR_ASSERT_ERR_CODE((err_code == p_test_vector->expected_err_code),
                                            "nrf_crypto_aes_mac_calculate multi");
            }

            // Verify the generated AES MAC.
            TEST_VECTOR_ASSERT((output_len == 16), "output_len is not 16");
            TEST_VECTOR_MEMCMP_ASSERT(m_aes_expected_output_buf,
                                      m_aes_output_buf,
                                      output_len,
                                      p_test_vector->expected_result,
                                      "Incorrect generated AES MAC second");
        }
        NRF_LOG_INFO("#%04d Test vector passed: %s %s",
                     p_test_info->current_id,
                     p_test_info->p_test_case_name,
                     p_test_vector->p_test_vector_name);

        p_test_info->tests_passed++;

exit_test_vector:

        // Un-initialize AES.
        (void) nrf_crypto_aes_uninit(&m_aes_context);

        while (NRF_LOG_PROCESS());

    }
    return NRF_SUCCESS;
}


/**@brief Function for the AES Monte Carlo test execution.
 */
ret_code_t exec_test_case_aes_monte_carlo(test_info_t * p_test_info)
{
    uint16_t i;
    uint16_t k;
    ret_code_t err_code;
    uint32_t aes_test_vector_count;
    size_t   iv_len;
    size_t   key_len;
    size_t   input_len;
    size_t   output_len;

    aes_test_vector_count = TEST_VECTOR_AES_MULTI_COUNT;

    for (i = 0; i < aes_test_vector_count; i++)
    {
        test_vector_aes_t * p_test_vector = TEST_VECTOR_AES_MULTI_GET(i);
        p_test_info->current_id++;

        // Reset buffers and fetch test vectors.
        memset(m_aes_input_buf, 0xFF, sizeof(m_aes_input_buf));
        memset(m_aes_expected_output_buf, 0xFF, sizeof(m_aes_expected_output_buf));
        memset(m_aes_output_buf, 0x00, sizeof(m_aes_output_buf));
        memset(m_prev_aes_output_buf, 0x00, sizeof(m_prev_aes_output_buf));

        // Fetch and unhexify plaintext and ciphertext.
        if (p_test_vector->direction == NRF_CRYPTO_ENCRYPT)
        {
            input_len  = unhexify(m_aes_input_buf, p_test_vector->p_plaintext);
            output_len = unhexify(m_aes_expected_output_buf, p_test_vector->p_ciphertext);
        }
        else
        {
            input_len  = unhexify(m_aes_input_buf, p_test_vector->p_ciphertext);
            output_len = unhexify(m_aes_expected_output_buf, p_test_vector->p_plaintext);
        }
        TEST_VECTOR_ASSERT((input_len == output_len),
                           "plaintext length not equal to ciphertext length");

        // Initialize AES.
        err_code = nrf_crypto_aes_init(&m_aes_context,
                                       p_test_vector->p_aes_info,
                                       p_test_vector->direction);

        // Fetch and set the AES key.
        memset(m_aes_key_buf, 0x00, sizeof(m_aes_key_buf));
        key_len  = unhexify(m_aes_key_buf, p_test_vector->p_key);
        err_code = nrf_crypto_aes_key_set(&m_aes_context, m_aes_key_buf);

        // Fetch and set the AES IV.
        memset(m_aes_iv_buf, 0x00, sizeof(m_aes_iv_buf));
        iv_len = unhexify(m_aes_iv_buf, p_test_vector->p_iv);
        if (iv_len != 0)
        {
            err_code = nrf_crypto_aes_iv_set(&m_aes_context, m_aes_iv_buf);
        }

        // Start the Monte Carlo test.
        start_time_measurement();
        for (k = 0; k < 100; k++)
        {
            switch(p_test_vector->p_aes_info->mode)
            {
                case NRF_CRYPTO_AES_MODE_ECB:
                    err_code = ecb_monte_carlo(p_test_info,
                                               p_test_vector,
                                               &m_aes_context,
                                               key_len,
                                               input_len,
                                               output_len);
                    if (err_code != p_test_vector->expected_err_code)
                    {
                        goto exit_test_vector;
                    }
                    break;

                case NRF_CRYPTO_AES_MODE_CBC:
                    err_code = cbc_monte_carlo(p_test_info,
                                               p_test_vector,
                                               &m_aes_context,
                                               key_len,
                                               input_len,
                                               output_len,
                                               iv_len);
                    if (err_code != p_test_vector->expected_err_code)
                    {
                        goto exit_test_vector;
                    }
                    break;

                case NRF_CRYPTO_AES_MODE_CFB:
                    err_code = cfb8_monte_carlo(p_test_info,
                                                p_test_vector,
                                                &m_aes_context,
                                                key_len,
                                                input_len,
                                                output_len,
                                                iv_len);
                    if (err_code != p_test_vector->expected_err_code)
                    {
                        goto exit_test_vector;
                    }
                    break;

                default:
                    break;
            }
        }

        // Verify generated AES plaintext or ciphertext.
        TEST_VECTOR_MEMCMP_ASSERT(m_aes_expected_output_buf,
                                  m_aes_output_buf,
                                  output_len,
                                  p_test_vector->expected_result,
                                  "Incorrect generated AES ciphertext");

        NRF_LOG_INFO("#%04d Test vector passed: %s %s",
                     p_test_info->current_id,
                     p_test_info->p_test_case_name,
                     p_test_vector->p_test_vector_name);

        p_test_info->tests_passed++;

exit_test_vector:
        stop_time_measurement();

        // Un-initialize AES.
        (void) nrf_crypto_aes_uninit(&m_aes_context);

        while (NRF_LOG_PROCESS());

    }
    return NRF_SUCCESS;
}


/**@brief Function for running the test teardown.
 */
ret_code_t teardown_test_case_aes(void)
{
    return NRF_SUCCESS;
}


/** @brief  Macro for registering the the AES funtional test case by using section variables.
 *
 * @details     This macro places a variable in a section named "test_case_data",
 *              which is initialized by main.
 */
NRF_SECTION_ITEM_REGISTER(test_case_data, test_case_t test_aes_encrypt_functional) =
{
    .p_test_case_name = "AES",
    .setup = setup_test_case_aes,
    .exec = exec_test_case_aes_functional,
    .teardown = teardown_test_case_aes
};


/** @brief  Macro for registering the AES test case by using section variables.
 *
 * @details     This macro places a variable in a section named "test_case_data",
 *              which is initialized by main.
 */
NRF_SECTION_ITEM_REGISTER(test_case_data, test_case_t test_aes_encrypt) =
{
    .p_test_case_name = "AES",
    .setup = setup_test_case_aes,
    .exec = exec_test_case_aes,
    .teardown = teardown_test_case_aes
};


/** @brief  Macro for registering the AES test case by using section variables.
 *
 * @details     This macro places a variable in a section named "test_case_data",
 *              which is initialized by main.
 */
NRF_SECTION_ITEM_REGISTER(test_case_data, test_case_t test_aes_mac) =
{
    .p_test_case_name = "AES",
    .setup = setup_test_case_aes,
    .exec = exec_test_case_aes_mac,
    .teardown = teardown_test_case_aes
};


/** @brief  Macro for registering the AES Monte Carlo test case by using section variables.
 *
 * @details     This macro places a variable in a section named "test_case_data",
 *              which is initialized by main.
 */
NRF_SECTION_ITEM_REGISTER(test_case_data, test_case_t test_aes_encrypt_monte_carlo) =
{
    .p_test_case_name = "AES",
    .setup = setup_test_case_aes,
    .exec = exec_test_case_aes_monte_carlo,
    .teardown = teardown_test_case_aes
};
/*lint -restore*/


#endif // NRF_CRYPTO_AES_ENABLED
