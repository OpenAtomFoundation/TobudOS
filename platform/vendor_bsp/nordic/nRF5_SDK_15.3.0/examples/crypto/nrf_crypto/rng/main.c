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
/** @file
 *
 * @defgroup nrf_crypto_rng main.c
 * @{
 * @ingroup nrf_crypto_rng
 * @brief Cryptographic Random Number Generator (RNG) Example Application main file.
 *
 * This file contains the source code for a sample application that demonstrates using the
 * nrf_crypto library to generate random numbers. Different backends can be used by adjusting
 * @ref sdk_config accordingly.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "nrf_log_default_backends.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_crypto.h"


#define VECTOR_LENGTH   8
#define ITERATIONS      5

static uint8_t m_random_vector[VECTOR_LENGTH];
static uint8_t m_min[VECTOR_LENGTH] = {0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Big-endian byte array
static uint8_t m_max[VECTOR_LENGTH] = {0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Big-endian byte array

/**
 * @brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t ret_val = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret_val);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    ret_code_t ret_val;

    log_init();

    NRF_LOG_INFO("RNG example started.");

    ret_val = nrf_crypto_init();
    APP_ERROR_CHECK(ret_val);

    // The RNG module is not explicitly initialized in this example, as
    // NRF_CRYPTO_RNG_AUTO_INIT_ENABLED and NRF_CRYPTO_RNG_STATIC_MEMORY_BUFFERS_ENABLED
    // are enabled in sdk_config.h.

    NRF_LOG_INFO("Generate %u random vectors of length %u:", ITERATIONS, VECTOR_LENGTH);
    for (int i = 0; i < ITERATIONS; i++)
    {
        ret_val = nrf_crypto_rng_vector_generate(m_random_vector, VECTOR_LENGTH);
        APP_ERROR_CHECK(ret_val);
        NRF_LOG_HEXDUMP_INFO(m_random_vector, VECTOR_LENGTH)
    }

    NRF_LOG_INFO("Generate %u constrained random vectors of length %u:", ITERATIONS, VECTOR_LENGTH);
    for (int i = 0; i < ITERATIONS; i++)
    {
        ret_val = nrf_crypto_rng_vector_generate_in_range(m_random_vector,
                                                          m_min,
                                                          m_max,
                                                          VECTOR_LENGTH);
        APP_ERROR_CHECK(ret_val);
        NRF_LOG_HEXDUMP_INFO(m_random_vector, VECTOR_LENGTH)
    }

    NRF_LOG_INFO("RNG example executed successfully.");

    for (;;)
    {
    }
}

/**
 *@}
 **/
