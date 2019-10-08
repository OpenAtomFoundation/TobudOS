/**
 * Copyright (c) 2012 - 2019, Nordic Semiconductor ASA
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
 * @file
 * @brief Implementation of mouse_sensor_emulate.h.
 */

#include "mouse_sensor_emulator.h"

#define MOUSE_SENSOR_TIMER              NRF_TIMER1                ///< Timer 1 is used to emulate the mouse.
#define MOUSE_SENSOR_TIMER_PERPOWER_Msk POWER_PERPOWER_TIMER1_Msk ///< Power mask for Timer 1 peripheral.
#define MOUSE_SENSOR_TIMER_IRQn         TIMER1_IRQn               ///< Mouse interrupt set to Timer 1's interrupt.
#define MOUSE_SENSOR_TIMER_IRQ_HANDLER  TIMER1_IRQHandler         ///< Mouse interrupt handler set to Timer 1's interrupt handler.

/**
 * @struct mouse_sensor_status_t
 * @brief Data structure holding internal mouse sensor variables.
 */
typedef struct
{
    int32_t acc_x;        ///< Accumulator holding accumulated mouse samples.
    int32_t acc_y;        ///< Accumulator holding accumulated mouse samples.
    bool    enabled;      ///< Mouse sensor enabled / disabled flag.
    bool    sample_ready; ///< Mouse sensor sample ready flag.
} mouse_sensor_status_t;

static mouse_sensor_status_t mouse_sensor_status; ///< Internal mouse sensor variables.

bool mouse_sensor_init(mouse_sensor_sample_period_t sample_period)
{
    mouse_sensor_status.acc_x        = 0;
    mouse_sensor_status.acc_y        = 0;
    mouse_sensor_status.sample_ready = false;
    mouse_sensor_status.enabled      = false;

    MOUSE_SENSOR_TIMER->TASKS_STOP  = 1;
    MOUSE_SENSOR_TIMER->TASKS_CLEAR = 1;
    MOUSE_SENSOR_TIMER->INTENCLR    = 0xffffffff;
    MOUSE_SENSOR_TIMER->INTENSET    = TIMER_INTENSET_COMPARE0_Msk;

    MOUSE_SENSOR_TIMER->SHORTS    = (1 << TIMER_SHORTS_COMPARE0_CLEAR_Pos);
    MOUSE_SENSOR_TIMER->PRESCALER = 4; // Ensures 1 us resolution @ 16MHz

    switch (sample_period)
    {
        case MOUSE_SENSOR_SAMPLE_PERIOD_4_MS:
            MOUSE_SENSOR_TIMER->CC[0] = 4000;
            break;

        case MOUSE_SENSOR_SAMPLE_PERIOD_8_MS:
        default:
            MOUSE_SENSOR_TIMER->CC[0] = 8000;
            break;
    }

    NVIC_ClearPendingIRQ(MOUSE_SENSOR_TIMER_IRQn);
    NVIC_EnableIRQ(MOUSE_SENSOR_TIMER_IRQn);
    return true;
}


void mouse_sensor_enable(void)
{
    MOUSE_SENSOR_TIMER->TASKS_CLEAR = 1;
    MOUSE_SENSOR_TIMER->TASKS_START = 1;
}


void mouse_sensor_disable(void)
{
    MOUSE_SENSOR_TIMER->TASKS_STOP = 1;
}


bool mouse_sensor_data_is_ready(void)
{
    return mouse_sensor_status.sample_ready;
}


bool mouse_sensor_read(uint8_t * out_mouse_packet)
{
    if (mouse_sensor_status.sample_ready)
    {
        out_mouse_packet[NRFR_REPORT_ID]               = NRFR_MOUSE_MOV_REPORT_ID;
        out_mouse_packet[NRFR_MOUSE_MOV_XLSBYTE]       = mouse_sensor_status.acc_y;
        out_mouse_packet[NRFR_MOUSE_MOV_YLSNIB_XMSNIB] = ((mouse_sensor_status.acc_x << 4) & 0xf0) |
                                                         ((mouse_sensor_status.acc_y >> 8) & 0x0f);
        out_mouse_packet[NRFR_MOUSE_MOV_YMSBYTE] = mouse_sensor_status.acc_x >> 4;
        mouse_sensor_status.acc_x                = 0;
        mouse_sensor_status.acc_y                = 0;
        mouse_sensor_status.sample_ready         = false;
        return true;
    }
    else
    {
        return false;
    }
}


/**
 * @brief Mouse sensor timer interrupt.
 *
 * Mouse sensor accumulator is updated and the callback
 * mouse_sensor_new_sample_generated_cb() is made to the application.
 */
void MOUSE_SENSOR_TIMER_IRQ_HANDLER(void)
{
    static uint8_t sample_cnt = 0;
    int32_t        new_value  = 0;

    if (sample_cnt < 20)
    {
        sample_cnt++;
    }
    else
    {
        sample_cnt = 0;
    }

    if (sample_cnt < 10)
    {
        new_value = 2;
    }
    else
    {
        new_value = -2;
    }

    mouse_sensor_status.acc_x       += new_value;
    mouse_sensor_status.acc_y       += new_value;
    mouse_sensor_status.sample_ready = true;

    MOUSE_SENSOR_TIMER->EVENTS_COMPARE[0] = 0;

    mouse_sensor_new_sample_generated_cb();
}


