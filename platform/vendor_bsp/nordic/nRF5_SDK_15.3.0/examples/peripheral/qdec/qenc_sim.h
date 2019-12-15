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
#ifndef QENC_SIM_H__
#define QENC_SIM_H__

#include <stdbool.h>
#include <stdint.h>
#include "boards.h"
#include "nrf_qdec.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#define QENC_CONFIG_PIO_LED         ARDUINO_A3_PIN
#define QENC_CONFIG_PIO_A           ARDUINO_A5_PIN
#define QENC_CONFIG_PIO_B           ARDUINO_A4_PIN

#define QENC_CONFIG_PIO_PULL_CFG    NRF_GPIO_PIN_NOPULL

/**@brief quadrature encoder simulator initialization.
 *
 * The simulator uses GPIOTE module to generate pulses compatible with QDEC on A,B outputs using LED input as a clocking signal
 * The sole purpose of this module is to test software driver
 *
 * @param[in] active level for LED in QDEC
 *
 */
void qenc_init(nrf_qdec_ledpol_t led_pol);


/**@brief quadrature simulator simulator set-up for pulse generation.
 * This function starts normal pulse generation if pulse_count is nonzero
 * When pulse_count is greater then zero positive pulses are generated and pulse_count is decremented till it reaches zero
 * When pulse_count is less then zero negative pulses are generated and pulse_count is incremented till it reaches zero
 *
 * @param[in] number of normal pulses
 *
 */
void qenc_pulse_count_set(int32_t pulse_count);


/**@brief quadrature simulator simulator set-up set-up for pulse and double pulse generation.
 * This function starts leading normal pulse generation if pulse_count is nonzero
 * and trailing double generation if dble_pulse_count is nonzero
 * When pulse_count is greater then zero positive pulses are generated and pulse_count is decremented till it reaches zero
 * When pulse_count is less then zero negative pulses are generated and pulse_count is incremented till it reaches zero
 * Next if dble_pulse_count is nonzero double pulses are generated and dble_pulse_count is decremented till it reaches zero
 *
 * @param[in] number of normal pulses
 * @param[in] number of double pulses
 */
void qenc_pulse_dblpulse_count_set(int32_t pulse_count, uint32_t dble_pulse_count);

#ifdef __cplusplus
}
#endif

#endif // QENC_SIM_H__
