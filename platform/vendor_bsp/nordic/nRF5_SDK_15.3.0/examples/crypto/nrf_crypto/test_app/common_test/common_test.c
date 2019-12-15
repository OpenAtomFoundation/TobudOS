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
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "boards.h"

#include "common_test.h"


uint32_t unhexify(uint8_t * p_output, char const * p_input)
{
    unsigned char c, c2;

    if (p_input == NULL)
    {
        return 0;
    }

    if (p_output == NULL)
    {
        return 0;
    }

    int len = strlen( p_input ) / 2;

    while( *p_input != 0 )
    {
        c = *p_input++;
        if( c >= '0' && c <= '9' )
        {
            c -= '0';
        }
        else if( c >= 'a' && c <= 'f' )
        {
            c -= 'a' - 10;
        }
        else if( c >= 'A' && c <= 'F' )
        {
            c -= 'A' - 10;
        }
        else
        {
            NRF_LOG_ERROR("unhexify ERROR");
        }

        c2 = *p_input++;
        if( c2 >= '0' && c2 <= '9' )
        {
            c2 -= '0';
        }
        else if( c2 >= 'a' && c2 <= 'f' )
        {
            c2 -= 'a' - 10;
        }
        else if( c2 >= 'A' && c2 <= 'F' )
        {
            c2 -= 'A' - 10;
        }
        else
        {
            NRF_LOG_ERROR("unhexify ERROR");
        }

        *p_output++ = ( c << 4 ) | c2;
    }

    return len;
}

/**@brief Function for running starting time measurements.
 */
void start_time_measurement()
{
    nrf_gpio_pin_clear(LED_1);
}

/**@brief Function for running stopping time measurements.
 */
void stop_time_measurement()
{
    nrf_gpio_pin_set(LED_1);
}
