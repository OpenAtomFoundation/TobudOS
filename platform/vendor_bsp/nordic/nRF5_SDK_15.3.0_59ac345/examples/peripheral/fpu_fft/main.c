/**
 * Copyright (c) 2016 - 2019, Nordic Semiconductor ASA
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
 * @defgroup fpu_fft_example_main main.c
 * @{
 * @ingroup fpu_fft_example
 * @brief Floating Point Number FFT Example Application main file.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "app_util_platform.h"
#include "bsp.h"
#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
/*lint -save -e689 */ /* Apparent end of comment ignored */
#include "arm_const_structs.h"
/*lint -restore */

#define GRAPH_WINDOW_HEIGHT              20                              //!< Graph window height used in draw function.

#define FPU_EXCEPTION_MASK               0x0000009F                      //!< FPU exception mask used to clear exceptions in FPSCR register.
#define FPU_FPSCR_REG_STACK_OFF          0x40                            //!< Offset of FPSCR register stacked during interrupt handling in FPU part stack.

// We want to use 44100 Hz sampling rate to reach 22050Hz band. 128 (64 pairs) samples are used
// in FFT calculation with result contains 64 bins (22050Hz/64bins -> ~344,5Hz per bin).
#define FFT_TEST_SAMPLE_FREQ_HZ          44100.0f                        //!< Frequency of complex input samples.
#define FFT_TEST_COMP_SAMPLES_LEN        128                             //!< Complex numbers input data array size. Correspond to FFT calculation this number must be power of two starting from 2^5 (2^4 pairs) with maximum value 2^13 (2^12 pairs).
#define FFT_TEST_OUT_SAMPLES_LEN         (FFT_TEST_COMP_SAMPLES_LEN / 2) //!< Output array size.

#define SIGNALS_RESOLUTION               100.0f                          //!< Sine wave frequency and noise amplitude resolution. To count resolution as decimal places in number use this formula: resolution = 1/SIGNALS_RESOLUTION .
#define SINE_WAVE_FREQ_MAX               20000                           //!< Maximum frequency of generated sine wave.
#define NOISE_AMPLITUDE                  1                               //!< Amplitude of generated noise added to signal.

static uint32_t  m_ifft_flag             = 0;                            //!< Flag that selects forward (0) or inverse (1) transform.
static uint32_t  m_do_bit_reverse        = 1;                            //!< Flag that enables (1) or disables (0) bit reversal of output.
static float32_t m_fft_input_f32[FFT_TEST_COMP_SAMPLES_LEN];             //!< FFT input array. Time domain.
static float32_t m_fft_output_f32[FFT_TEST_OUT_SAMPLES_LEN];             //!< FFT output data. Frequency domain.

/**
 * @brief Function for generating sine wave samples for FFT calculations.
 *
 * This function fill up output array with generated sine wave data with proper sampling frequency.
 * Must be executed before fft_process function.
 *
 * @param[in] p_input     Input array to fill with sine wave generated data.
 * @param[in] size        Input array size.
 * @param[in] sample_freq Sine wave sampling frequency.
 * @param[in] sine_freq   Sine wave frequency.
 * @param[in] add_noise   Flag for enable or disble adding noise for generated data.
 */
static void fft_generate_samples(float32_t * p_input,
                                 uint16_t    size,
                                 float32_t   sampling_freq,
                                 float32_t   sine_freq,
                                 bool        add_noise)
{
    uint32_t i;

    /* Remember that sample is represented as two next values in array. */
    uint32_t sample_idx = 0;

    if (2 > size)
    {
        return;
    }

    for (i = 0; i < (size - 1UL); i += 2) {
        sample_idx = i / 2;
        // Real part.
        p_input[(uint16_t)i] = sin(sine_freq * (2.f * PI) * sample_idx / sampling_freq);
        if (add_noise)
        {
            // Noise can be positive or negative number. Numbers can be cast to signed values.
            p_input[(uint16_t)i] *= ((rand()) % ((int)(NOISE_AMPLITUDE * SIGNALS_RESOLUTION)))
                                    / SIGNALS_RESOLUTION;
        }
        // Img part.
        p_input[(uint16_t)i + 1] = 0;
    }
}

/**
 * @brief Function for processing generated sine wave samples.
 * @param[in] p_input        Pointer to input data array with complex number samples in time domain.
 * @param[in] p_input_struct Pointer to cfft instance structure describing input data.
 * @param[out] p_output      Pointer to processed data (bins) array in frequency domain.
 * @param[in] output_size    Processed data array size.
 */
static void fft_process(float32_t *                   p_input,
                        const arm_cfft_instance_f32 * p_input_struct,
                        float32_t *                   p_output,
                        uint16_t                      output_size)
{
    // Use CFFT module to process the data.
    arm_cfft_f32(p_input_struct, p_input, m_ifft_flag, m_do_bit_reverse);
    // Calculate the magnitude at each bin using Complex Magnitude Module function.
    arm_cmplx_mag_f32(p_input, p_output, output_size);
}

#ifdef FPU_INTERRUPT_MODE
/**
 * @brief FPU Interrupt handler. Clearing exception flag at the stack.
 *
 * Function clears exception flag in FPSCR register and at the stack. During interrupt handler
 * execution FPU registers might be copied to the stack (see lazy stacking option) and
 * it is necessary to clear data at the stack which will be recovered in the return from
 * interrupt handling.
 */
void FPU_IRQHandler(void)
{
    // Prepare pointer to stack address with pushed FPSCR register.
    uint32_t * fpscr = (uint32_t * )(FPU->FPCAR + FPU_FPSCR_REG_STACK_OFF);
    // Execute FPU instruction to activate lazy stacking.
    (void)__get_FPSCR();
    // Clear flags in stacked FPSCR register.
    *fpscr = *fpscr & ~(FPU_EXCEPTION_MASK);
}
#endif

/**
 * @brief Function for drawing line with given width.
 * @param[in] line_width Line width.
 */
static void draw_line(uint16_t line_width)
{
    uint32_t i;
    char     line[line_width + 1];

    for (i = 0; i < line_width; i++)
    {
        line[i] = '-';
    }
    line[line_width] = 0;
    NRF_LOG_RAW_INFO("%s\r\n", nrf_log_push(line));
}

/**
 * @brief Function for drawing line and processed data informations.
 * @param[in] input_sine_freq Input sine wave frequency.
 * @param[in] is_noisy        Flag if data is noisy.
 * @param[in] chart_width     Drawing chart height.
 */
static void draw_fft_header(float32_t input_sine_freq, bool is_noisy)
{
    NRF_LOG_RAW_INFO("Input: sine %uHz, noise: %s.\r\n", (uint16_t)input_sine_freq,
           (uint32_t)((is_noisy == true) ? "yes" : "no"));
}

/**
 * @brief Function for drawing ASCII data processed by FFT function.
 * @param[in] p_input_data Pointer to FFT data array.
 * @param[in] data_size    FFT array size.
 * @param[in] chart_height Drawing chart height.
 */
static void draw_fft_data(float32_t * p_input_data, uint16_t data_size, uint16_t chart_height)
{
    uint32_t  graph_y, graph_x;
    float32_t curr_drawing_val;
    float32_t curr_percent;
    float32_t max_value;
    uint32_t  max_val_index;
    char      tmp_str[data_size + 1];

    // Search FFT max value in input array.
    arm_max_f32(p_input_data, data_size, &max_value, &max_val_index);

    // Draw graph. Put space if number is less than currently processed, put '|' character otherwise.
    for (graph_y = chart_height; graph_y > 0; graph_y--)
    {
        curr_percent = ((graph_y - 1) / (chart_height * 1.f));
        curr_drawing_val = max_value * curr_percent;
        for (graph_x = 0; graph_x < data_size; graph_x++)
        {
            if (m_fft_output_f32[graph_x] > curr_drawing_val)
            {
                tmp_str[graph_x] = '|';
            } else
            {
                tmp_str[graph_x] = ' ';
            }
        }
        tmp_str[data_size] = 0;
        NRF_LOG_RAW_INFO("%s\r\n", NRF_LOG_PUSH(tmp_str));
        NRF_LOG_FLUSH();
    }

    draw_line(data_size);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t  err_code;
    bool      noise = false;
    float32_t sine_freq;

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();

    // Welcome message.
    NRF_LOG_INFO("FPU FFT example started.");
    NRF_LOG_RAW_INFO("This is FPU usage example with FFT calculation and drawing.\r\n");

#ifdef FPU_INTERRUPT_MODE
    // Enable FPU interrupt
    NVIC_SetPriority(FPU_IRQn, APP_IRQ_PRIORITY_LOWEST);
    NVIC_ClearPendingIRQ(FPU_IRQn);
    NVIC_EnableIRQ(FPU_IRQn);
#endif

    // Enter main loop.
    for (;;)
    {
        // Generate new samples.
        noise = !noise;
        // Sine wave frequency must be positive number so cast rand to unsigned number.
        // stdlib using with Segger Embedded Studio uses rand() configured to return values up to
        // 32767 instead of 0x7fffffff configured in other compilers. It causes that generated sine
        // frequency is smaller, but example works in the same way.
        sine_freq = (((uint32_t)rand()) % ((uint32_t)(SINE_WAVE_FREQ_MAX * SIGNALS_RESOLUTION)))
                    / SIGNALS_RESOLUTION;
        fft_generate_samples(m_fft_input_f32,
                             FFT_TEST_COMP_SAMPLES_LEN,
                             FFT_TEST_SAMPLE_FREQ_HZ,
                             sine_freq, noise);

        // Process generated data. 64 pairs of complex data (real, img). It is important to use
        // proper arm_cfft_sR_f32 structure associated with input/output data length.
        // For example:
        //  - 128 numbers in input array (64 complex pairs of samples) -> 64 output bins power data -> &arm_cfft_sR_f32_len64.
        //  - 256 numbers in input array (128 complex pairs of samples) -> 128 output bins power data -> &arm_cfft_sR_f32_len128.
        fft_process(m_fft_input_f32,
                    &arm_cfft_sR_f32_len64,
                    m_fft_output_f32,
                    FFT_TEST_OUT_SAMPLES_LEN);

        // Draw FFT bin power chart.
        draw_fft_header(sine_freq, noise);
        draw_fft_data(m_fft_output_f32, FFT_TEST_OUT_SAMPLES_LEN, GRAPH_WINDOW_HEIGHT);

        NRF_LOG_FLUSH();

#ifndef FPU_INTERRUPT_MODE
        /* Clear FPSCR register and clear pending FPU interrupts. This code is base on
         * nRF5x_release_notes.txt in documentation folder. It is necessary part of code when
         * application using power saving mode and after handling FPU errors in polling mode.
         */
        __set_FPSCR(__get_FPSCR() & ~(FPU_EXCEPTION_MASK));
        (void) __get_FPSCR();
        NVIC_ClearPendingIRQ(FPU_IRQn);
#endif
        nrf_delay_ms(1000);
    }
}

/**
 * @}
 */
