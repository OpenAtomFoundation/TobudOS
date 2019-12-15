/**
 * Copyright (c) 2015 - 2019, Nordic Semiconductor ASA
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
 * @defgroup nrf_drv_csense_example_main main.c
 * @{
 * @ingroup nrf_drv_csense_example
 * @brief Capacitive Sensor Example main file.
 *
 * This file contains the source code for a sample application that uses a capacitive sensor.
 *
 */

#include <stdio.h>
#include "boards.h"
#include "nrf_drv_csense.h"
#include "app_error.h"
#include "nrf_drv_clock.h"
#include "app_timer.h"
#include "nrf_delay.h"
#include "nrf_cli.h"
#include "nrf_cli_uart.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/* Pin used to measure capacitor charging time. */
#if USE_COMP == 0
#ifdef ADC_PRESENT
#define OUTPUT_PIN 30
#elif defined(SAADC_PRESENT)
#define OUTPUT_PIN 26
#endif
#endif

/* Time between RTC interrupts. */
#define APP_TIMER_TICKS_TIMEOUT APP_TIMER_TICKS(50)

/* Analog inputs. */
#define AIN_1                   1
#define AIN_2                   2
#define AIN_7                   7

/* Masks of analog channels to be used by library. */
#ifdef NRF51
#define PAD_1_MASK              (1UL << AIN_2)
#else
#define PAD_1_MASK              (1UL << AIN_1)
#endif
#define PAD_2_MASK              (1UL << AIN_7)

#define PAD_ID_0                0
#define PAD_ID_1                1

#define UART_TX_BUF_SIZE 512                                                /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 1                                                  /**< UART RX buffer size. */


NRF_CLI_UART_DEF(m_cli_uart_transport, 0, 64, 16);
NRF_CLI_DEF(m_cli_uart, "uart_cli:~$ ", &m_cli_uart_transport.transport, '\r', 4);



/* Threshold number of ticks (if COMP module present) or voltage in millivolts on sensor. */
volatile uint32_t threshold_value_pad1 = 850;
volatile uint32_t threshold_value_pad2 = 850;

/* Variables needed to properly configure threshold. */
volatile uint32_t max_value[2];
volatile uint32_t min_value[2];
volatile uint32_t m_wait_timer = 0;

typedef enum
{
    CSD_STATE_IDLE,
    CSD_STATE_START_CONFIG,
    CSD_STATE_WAIT_TOUCH,
    CSD_STATE_TOUCH,
    CSD_STATE_WAIT_RELEASE,
    CSD_STATE_RELEASE
} csd_state_t;

static csd_state_t m_state;

static void timer_handle(void * p_context)
{
    if (m_wait_timer)
    {
        --m_wait_timer;
    }
}

/**
 * @brief Function for starting the internal LFCLK XTAL oscillator.
 *
 * Note that when using a SoftDevice, LFCLK is always on.
 *
 * @return Values returned by @ref nrf_drv_clock_init.
 */
static ret_code_t clock_config(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_clock_init();
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    nrf_drv_clock_lfclk_request(NULL);

    return NRF_SUCCESS;
}

static inline void find_min_and_max(uint32_t val, uint8_t pad)
{
    if (val > max_value[pad])
    {
        max_value[pad] = val;
    }
    if (val < min_value[pad])
    {
        min_value[pad] = val;
    }
}

/**
 * @brief Capacitive sensor event handler.
 */
void csense_handler(nrf_drv_csense_evt_t * p_event_struct)
{
    switch (p_event_struct->analog_channel)
    {
        case AIN_1:
        case AIN_2:
            if ((m_state == CSD_STATE_TOUCH) || (m_state == CSD_STATE_RELEASE))
            {
                find_min_and_max(p_event_struct->read_value, PAD_ID_0);
            }
            else if (p_event_struct->read_value >= threshold_value_pad1)
            {
                bsp_board_led_on(BSP_BOARD_LED_0);
            }
            else
            {
                bsp_board_led_off(BSP_BOARD_LED_0);
            }
            break;
        case AIN_7:
            if ((m_state == CSD_STATE_TOUCH) || (m_state == CSD_STATE_RELEASE))
            {
                find_min_and_max(p_event_struct->read_value, PAD_ID_1);
            }
            else if (p_event_struct->read_value >= threshold_value_pad2)
            {
                bsp_board_led_on(BSP_BOARD_LED_2);
            }
            else
            {
                bsp_board_led_off(BSP_BOARD_LED_2);
            }
            break;
        default:
            break;
    }
}
/**
 * @brief Function for initializing the capacitive sensor.
 */
void csense_initialize(void)
{
    ret_code_t err_code;

    nrf_drv_csense_config_t csense_config = { 0 };

#if USE_COMP == 0
    csense_config.output_pin = OUTPUT_PIN;
#endif

    err_code = nrf_drv_csense_init(&csense_config, csense_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_csense_channels_enable(PAD_1_MASK | PAD_2_MASK);
}

/**
 * @brief Timer event handler for the capacitive sensor.
 *
 * @param[in] p_context             General purpose pointer. Will be passed to the time-out handler
 *                                  when the timer expires.
 *
 */
static void csense_timeout_handler(void * p_context)
{
    ret_code_t err_code;

    err_code = nrf_drv_csense_sample();
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_RAW_INFO("Busy.");
        return;
    }
}

/**
 * @brief Function for initalizing app timer.
 */
void start_app_timer(void)
{
    ret_code_t err_code;

    /* APP_TIMER definition for csense example. */
    APP_TIMER_DEF(timer_0);
    APP_TIMER_DEF(timer_1);

    err_code = app_timer_create(&timer_1, APP_TIMER_MODE_REPEATED, timer_handle);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(timer_1, APP_TIMER_TICKS(100), NULL);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&timer_0, APP_TIMER_MODE_REPEATED, csense_timeout_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(timer_0, APP_TIMER_TICKS_TIMEOUT, NULL);
    APP_ERROR_CHECK(err_code);
}

#define CSD_NEXT_STATE(_state)  (m_state = (_state))
static void configuration(void)
{
    ret_code_t err_code;
    uint32_t new_th_pad_1;
    uint32_t new_th_pad_2;

    switch (m_state)
    {
        case CSD_STATE_IDLE:
            /* do nothing */
            break;
        case CSD_STATE_START_CONFIG:
            NRF_LOG_RAW_INFO("[STEP: %d] Touch both pads.\n", CSD_STATE_START_CONFIG);
            for (int i = 0; i < 2; i++)
            {
                max_value[i] = 0;
                min_value[i] = UINT32_MAX;
            }
            CSD_NEXT_STATE(CSD_STATE_WAIT_TOUCH);
            m_wait_timer = 15;  // wait 1,5 s
            break;
        case CSD_STATE_WAIT_TOUCH:
            if (m_wait_timer == 0)
            {
                NRF_LOG_RAW_INFO("[STEP: %d] Measurement started: wait 3s\n",
                                 CSD_STATE_WAIT_TOUCH);
                m_wait_timer = 30;   // measure pads for 3 seconds
                CSD_NEXT_STATE(CSD_STATE_TOUCH);
                break;
            }
            break;
        case CSD_STATE_TOUCH:
            if (m_wait_timer == 0)
            {
                CSD_NEXT_STATE(CSD_STATE_WAIT_RELEASE);
                err_code = nrf_drv_csense_sample();
                if (err_code != NRF_SUCCESS)
                {
                    NRF_LOG_RAW_INFO("Sensor Busy - configuration terminated.\n");
                    CSD_NEXT_STATE(CSD_STATE_IDLE);
                    return;
                }
                while (nrf_drv_csense_is_busy());
                NRF_LOG_RAW_INFO("-> measurement finished OK.\n");
                NRF_LOG_RAW_INFO("[STEP: %d] Release both pads.\n", CSD_STATE_TOUCH);
                m_wait_timer = 15;  // wait 1,5 s
                break;
            }
            break;
        case CSD_STATE_WAIT_RELEASE:
            if (m_wait_timer == 0)
            {
                NRF_LOG_RAW_INFO("[STEP: %d] Measurement started: wait 3s\n",
                                 CSD_STATE_WAIT_RELEASE);
                CSD_NEXT_STATE(CSD_STATE_RELEASE);
                m_wait_timer = 30;  // wait 1 s
            }
            break;
        case CSD_STATE_RELEASE:
            if (m_wait_timer == 0)
            {
                CSD_NEXT_STATE(CSD_STATE_IDLE);
                err_code = nrf_drv_csense_sample();
                if (err_code != NRF_SUCCESS)
                {
                    NRF_LOG_RAW_INFO("Sensor Busy - configuration terminated.\n");
                    return;
                }
                while (nrf_drv_csense_is_busy());
                NRF_LOG_RAW_INFO("-> measurement finished OK.\n");
                new_th_pad_1 = max_value[PAD_ID_0];
                new_th_pad_1 += min_value[PAD_ID_0];
                new_th_pad_1 /= 2;
                new_th_pad_2 = max_value[PAD_ID_1];
                new_th_pad_2 += min_value[PAD_ID_1];
                new_th_pad_2 /= 2;
                threshold_value_pad1 = new_th_pad_1;
                threshold_value_pad2 = new_th_pad_2;

                NRF_LOG_RAW_INFO("\nNew thresholds:\n\tAIN1 = %d\n\tAIN7 = %d"
                                 "\nModule ready.\n",
                                 (unsigned int)new_th_pad_1,
                                 (unsigned int)new_th_pad_2);
                break;
            }
            break;
        default:
            break;
    }
}


/**
 * @brief Function for main application entry.
 */
int main(void)
{
    ret_code_t err_code;

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));

    bsp_board_init(BSP_INIT_LEDS);

    err_code = clock_config();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_uart_config_t uart_config = NRF_DRV_UART_DEFAULT_CONFIG;
    uart_config.pseltxd = TX_PIN_NUMBER;
    uart_config.pselrxd = RX_PIN_NUMBER;
    uart_config.hwfc    = NRF_UART_HWFC_DISABLED;
    err_code = nrf_cli_init(&m_cli_uart, &uart_config, true, true, NRF_LOG_SEVERITY_INFO);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_cli_start(&m_cli_uart);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_RAW_INFO("Capacitive sensing driver example started. \n");
    csense_initialize();
    NRF_LOG_RAW_INFO("Please execute: \"configure\" command to set thresholds.\n"
                     "In order to see all available commands please press the Tab button.\n");

    start_app_timer();

    while (1)
    {
        UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
        nrf_cli_process(&m_cli_uart);
        configuration();
        __WFI();
    }
}

static void configure_thresholds(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    if (argc > 1)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: too many parameters\n", argv[0]);
        return;
    }

    if (m_state != CSD_STATE_IDLE)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_WARNING, "configuration already ongoing\n");
    }
    else
    {
        CSD_NEXT_STATE(CSD_STATE_START_CONFIG);
    }
}

NRF_CLI_CMD_REGISTER(configure, NULL, "Thresholds configuration command.", configure_thresholds);

/** @} */
