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

/**
 * @defgroup nrf_crypto_test_example
 * @{
 * @addtogroup nrf_crypto_test
 * @brief Cryptographic Test Example Application main file.
 *
 * This file contains the source code for a sample application that demonstrates how to test
 * the cryptographic functions inside the nrf_crypto library. Different backends can be used
 * by adjusting @ref sdk_config.h accordingly.
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stddef.h>
#include "boards.h"
#include "nrf_gpio.h"
#include "app_util.h"
#include "nrf_delay.h"
#include "nrf_error.h"
#include "nrf_section.h"
#include "nrf_crypto_rng.h"
#include "nrf_crypto_init.h"
#if NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_OPTIGA)
#include "nrf_drv_clock.h"
#endif // NRF_MODULE_ENABLED(NRF_CRYPTO_BACKEND_OPTIGA)
#include "nrf_log_default_backends.h"
#include "nrf_log_ctrl.h"
#include "nrf_log.h"
#include "common_test.h"
#include "mem_manager.h"

NRF_SECTION_DEF(test_case_data, test_case_t);

static test_info_t test_info;           /**< Overall test results. */


/**@brief Function for initializing the log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for application main entry.
 */
int main(void)
{
    uint32_t   i;
    uint32_t   j;
    uint32_t   test_case_count = TEST_CASE_COUNT;
    ret_code_t ret_val;


#if NRF_CRYPTO_BACKEND_OPTIGA_ENABLED
    // Start internal LFCLK XTAL oscillator
    ret_val = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret_val);
    nrf_drv_clock_lfclk_request(NULL);
#endif

    log_init();
    ret_val = nrf_crypto_init();
    APP_ERROR_CHECK(ret_val);

    ret_val = nrf_mem_init();
    APP_ERROR_CHECK(ret_val);

    bsp_board_init(BSP_INIT_LEDS);

#if defined (NRF52832_XXAA) || defined (NRF52832_XXAB) || defined (NRF52840_XXAA)
    // Enable I-code cache to speed up certain memory operatins.
    NRF_NVMC->ICACHECNF = NVMC_ICACHECNF_CACHEEN_Enabled;
#endif

    NRF_LOG_INFO("Crypto Test Application Started");
    NRF_LOG_INFO("***************************************");

    // Two seconds delay for synchronizing time measurements.
    nrf_delay_ms(2000);

    // Iterate through and execute all tests.
    for(i = 0; i < TEST_SUITE_EXECUTION_COUNT; i++)
    {
        for (j = 0; j < test_case_count; j++)
        {
            test_case_t * test_case = TEST_CASE_GET(j);
            test_info.p_test_case_name = test_case->p_test_case_name;
            NRF_LOG_INFO("Test case %s Started", test_case->p_test_case_name);

            // Run test case setup function.
            ret_val = test_case->setup();
            TEST_CASE_ASSERT(ret_val, test_case->p_test_case_name, "setup");

            // Run test case execution function.
            ret_val = test_case->exec(&test_info);
            TEST_CASE_ASSERT(ret_val, test_case->p_test_case_name, "exec");

            // Run test case teardown function.
            ret_val = test_case->teardown();
            TEST_CASE_ASSERT(ret_val, test_case->p_test_case_name, "teardown");

            NRF_LOG_INFO("Test case %s Done", test_case->p_test_case_name);
            NRF_LOG_INFO("***************************************");

            while (NRF_LOG_PROCESS());
        }
    }

    NRF_LOG_INFO("All Tests Done");
    NRF_LOG_INFO("%d test vectors passed", test_info.tests_passed);
    NRF_LOG_INFO("%d test vectors failed", test_info.tests_failed);
    NRF_LOG_INFO("***************************************");

    if (test_info.tests_failed == 0)
    {
        NRF_LOG_INFO("Crypto Test Application executed successfully.");
    }
    else
    {
        NRF_LOG_ERROR("Crypto Test Application failed!!!");
    }

    for(;;)
    {
        if (NRF_LOG_PROCESS() == false)
        {
            __WFE();
        }
    }
}


/** @}
 */
