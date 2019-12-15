/**
 * Copyright (c) 2014 - 2019, Nordic Semiconductor ASA
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
 * @defgroup ram_retention_example_main main.c
 * @{
 * @ingroup ram_retention_example
 * @brief RAM Retention Example Application main file.
 *
 * This file contains the source code for a sample application using RAM retention.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "bsp.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define RAM_MEMORY_TEST_ADDRESS (0x20002000UL)  /**< Address in RAM where test word (RAM_MEMORY_TEST_WORD) is written before System OFF and checked after System RESET.*/
#define RAM_MEMORY_TEST_WORD    (0xFEEDBEEFUL)  /**< Test word that is written to RAM address RAM_MEMORY_TEST_ADDRESS. */
#define RESET_MEMORY_TEST_BYTE  (0x0DUL)        /**< Known sequence written to a special register to check if this wake up is from System OFF. */
#define MAX_TEST_ITERATIONS     (1)             /**< Maximum number of iterations this example will run. */
#define SUCCESS_OUTPUT_VALUE    (0xAB)          /**< If RAM retention is tested for MAX_TEST_ITERATIONS, this value will be given as output.*/

#ifdef BSP_BUTTON_0
    #define PIN_GPIO_WAKEUP BSP_BUTTON_0        /**< Wake up pin number. */
#endif
#ifndef PIN_GPIO_WAKEUP
    #error "Please indicate wake up pin"
#endif

#ifdef BSP_LED_0
    #define READY_PIN_NUMBER BSP_BOARD_LED_0  /**< Pin number for output. */
#endif
#ifndef READY_PIN_NUMBER
    #error "Please indicate output pin"
#endif

#ifdef BSP_LED_1
    #define SUCCESS_PIN_NUMBER BSP_BOARD_LED_1  /**< Pin number for output. */
#endif
#ifndef SUCCESS_PIN_NUMBER
    #error "Please indicate output pin"
#endif

/** @brief Function for handling HardFaults. In case something went wrong
 * or System OFF did not work and reached the end of the program.
 */
void HardFault_Handler()
{
    bsp_board_leds_on();

    // Loop forever.
    while (true)
    {
        // Do nothing.
    }
}


/** @brief Function for indicating failure.
 */
static void display_failure(void)
{
    bsp_board_leds_on();

    // Loop forever.
    while (true)
    {
        // Do nothing.
    }
}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t * volatile p_ram_test = (uint32_t *)RAM_MEMORY_TEST_ADDRESS;
    uint32_t            loop_count = 0;

    bsp_board_init(BSP_INIT_LEDS);

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    // This pin is used for waking up from System OFF and is active low, enabling sense capabilities.
    nrf_gpio_cfg_sense_input(PIN_GPIO_WAKEUP, BUTTON_PULL, NRF_GPIO_PIN_SENSE_LOW);

    // Workaround for PAN_028 rev1.1 anomaly 22 - System: Issues with disable System OFF mechanism
    nrf_delay_ms(1);

    // Check if the system woke up from System OFF mode by reading the NRF_POWER->GPREGRET register which has
    // retained the value written before going to System OFF. Below is the layout for usage for
    // NRF_POWER->GPREGRET register.
    //  BITS |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0
    //  --------------------------------------------------------------------
    //       |        SPECIAL_SEQUENCE       |          LOOP_COUNT
    //  --------------------------------------------------------------------
    ///

    if ((NRF_POWER->GPREGRET >> 4) == RESET_MEMORY_TEST_BYTE)
    {
        // Take the loop_count value.
        loop_count          = (uint8_t)(NRF_POWER->GPREGRET & 0xFUL);
        NRF_POWER->GPREGRET = 0;

        if (loop_count >= (uint8_t)MAX_TEST_ITERATIONS)
        {
            // clear GPREGRET register before exit.
            NRF_POWER->GPREGRET = 0;
            bsp_board_led_on(SUCCESS_PIN_NUMBER);

            while (true)
            {
                // Do nothing.
            }
        }

        if (*p_ram_test != RAM_MEMORY_TEST_WORD)
        {
            display_failure();
        }

        *p_ram_test = 0;
    }

    // Write the known sequence + loop_count to the GPREGRET register.
    loop_count++;
    NRF_POWER->GPREGRET = ( (RESET_MEMORY_TEST_BYTE << 4) | loop_count);

    // Write the known value to the known address in RAM, enable RAM retention, set System OFF, and wait
    // for GPIO wakeup from external source.

    bsp_board_led_on(READY_PIN_NUMBER);
    nrf_delay_ms(1000);

    // Switch on both RAM banks when in System OFF mode.

#if defined(NRF52832_XXAA) || defined(NRF51422) || defined(NRF51822)
    NRF_POWER->RAMON |= (POWER_RAMON_OFFRAM0_RAM0On << POWER_RAMON_OFFRAM0_Pos) |
                        (POWER_RAMON_OFFRAM1_RAM1On << POWER_RAMON_OFFRAM1_Pos);

#elif defined(NRF52840_XXAA)
    NRF_POWER->RAM[0].POWERSET = (POWER_RAM_POWER_S0POWER_On << POWER_RAM_POWER_S0POWER_Pos)      |
                            (POWER_RAM_POWER_S1POWER_On      << POWER_RAM_POWER_S1POWER_Pos)      |
                            (POWER_RAM_POWER_S2POWER_On      << POWER_RAM_POWER_S2POWER_Pos)      |
                            (POWER_RAM_POWER_S3POWER_On      << POWER_RAM_POWER_S3POWER_Pos)      |
                            (POWER_RAM_POWER_S4POWER_On      << POWER_RAM_POWER_S4POWER_Pos)      |
                            (POWER_RAM_POWER_S5POWER_On      << POWER_RAM_POWER_S5POWER_Pos)      |
                            (POWER_RAM_POWER_S6POWER_On      << POWER_RAM_POWER_S6POWER_Pos)      |
                            (POWER_RAM_POWER_S7POWER_On      << POWER_RAM_POWER_S7POWER_Pos)      |
                            (POWER_RAM_POWER_S8POWER_On      << POWER_RAM_POWER_S8POWER_Pos)      |
                            (POWER_RAM_POWER_S9POWER_On      << POWER_RAM_POWER_S9POWER_Pos)      |
                            (POWER_RAM_POWER_S10POWER_On     << POWER_RAM_POWER_S10POWER_Pos)     |
                            (POWER_RAM_POWER_S11POWER_On     << POWER_RAM_POWER_S11POWER_Pos)     |
                            (POWER_RAM_POWER_S12POWER_On     << POWER_RAM_POWER_S12POWER_Pos)     |
                            (POWER_RAM_POWER_S13POWER_On     << POWER_RAM_POWER_S13POWER_Pos)     |
                            (POWER_RAM_POWER_S14POWER_On     << POWER_RAM_POWER_S14POWER_Pos)     |
                            (POWER_RAM_POWER_S15POWER_On     << POWER_RAM_POWER_S15POWER_Pos)     |
                            (POWER_RAM_POWER_S0RETENTION_On  << POWER_RAM_POWER_S0RETENTION_Pos)  |
                            (POWER_RAM_POWER_S1RETENTION_On  << POWER_RAM_POWER_S1RETENTION_Pos)  |
                            (POWER_RAM_POWER_S2RETENTION_On  << POWER_RAM_POWER_S2RETENTION_Pos)  |
                            (POWER_RAM_POWER_S3RETENTION_On  << POWER_RAM_POWER_S3RETENTION_Pos)  |
                            (POWER_RAM_POWER_S4RETENTION_On  << POWER_RAM_POWER_S4RETENTION_Pos)  |
                            (POWER_RAM_POWER_S5RETENTION_On  << POWER_RAM_POWER_S5RETENTION_Pos)  |
                            (POWER_RAM_POWER_S6RETENTION_On  << POWER_RAM_POWER_S6RETENTION_Pos)  |
                            (POWER_RAM_POWER_S7RETENTION_On  << POWER_RAM_POWER_S7RETENTION_Pos)  |
                            (POWER_RAM_POWER_S8RETENTION_On  << POWER_RAM_POWER_S8RETENTION_Pos)  |
                            (POWER_RAM_POWER_S9RETENTION_On  << POWER_RAM_POWER_S9RETENTION_Pos)  |
                            (POWER_RAM_POWER_S10RETENTION_On << POWER_RAM_POWER_S10RETENTION_Pos) |
                            (POWER_RAM_POWER_S11RETENTION_On << POWER_RAM_POWER_S11RETENTION_Pos) |
                            (POWER_RAM_POWER_S12RETENTION_On << POWER_RAM_POWER_S12RETENTION_Pos) |
                            (POWER_RAM_POWER_S13RETENTION_On << POWER_RAM_POWER_S13RETENTION_Pos) |
                            (POWER_RAM_POWER_S14RETENTION_On << POWER_RAM_POWER_S14RETENTION_Pos) |
                            (POWER_RAM_POWER_S15RETENTION_On << POWER_RAM_POWER_S15RETENTION_Pos);
#else
#error 'Unsupported MCU'
#endif
    // Write test word to RAM memory.
    *p_ram_test = RAM_MEMORY_TEST_WORD;
    bsp_board_leds_off();
    // Enter System OFF and wait for wake up from GPIO detect signal.
    NRF_POWER->SYSTEMOFF = 0x1;
    // Use data synchronization barrier and a delay to ensure that no failure
    // indication occurs before System OFF is actually entered.
    __DSB();
    __NOP();

    // This code will only be reached if System OFF did not work and will trigger a hard-fault which will
    // be handled in HardFault_Handler(). If wake the up condition is already active while System OFF is triggered,
    // then the system will go to System OFF and wake up immediately with a System RESET.
    display_failure();
}


/** @} */
