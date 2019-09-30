/**
 * This software is subject to the ANT+ Shared Source License
 * www.thisisant.com/swlicenses
 * Copyright (c) Garmin Canada Inc. 2014
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 *    1) Redistributions of source code must retain the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer.
 *
 *    2) Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 *    3) Neither the name of Garmin nor the names of its
 *       contributors may be used to endorse or promote products
 *       derived from this software without specific prior
 *       written permission.
 *
 * The following actions are prohibited:
 *
 *    1) Redistribution of source code containing the ANT+ Network
 *       Key. The ANT+ Network Key is available to ANT+ Adopters.
 *       Please refer to http://thisisant.com to become an ANT+
 *       Adopter and access the key. 
 *
 *    2) Reverse engineering, decompilation, and/or disassembly of
 *       software provided in binary form under this license.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE HEREBY
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; DAMAGE TO ANY DEVICE, LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE. SOME STATES DO NOT ALLOW 
 * THE EXCLUSION OF INCIDENTAL OR CONSEQUENTIAL DAMAGES, SO THE
 * ABOVE LIMITATIONS MAY NOT APPLY TO YOU.
 *
 */
#include "n5sk_led.h"
#include "nrf_gpio.h"
#if defined(BOARD_N5DK1)
    #include "n5_starterkit.h"
#endif


////////////////////////////////////////////////////////////////////////////////
// Description: LED control functions for the N5 Starter Kit IO Board
// Note: N5 Starter Kit IO Board LEDs are active low.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////

#if 0
/**@brief Function for turning off all leds.
 *
 */
void n5_io_clear_leds(void)
{
    nrf_gpio_pin_set(LED_A);
    nrf_gpio_pin_set(LED_B);
    nrf_gpio_pin_set(LED_C);
    nrf_gpio_pin_set(LED_D);
}

/**@brief Function for turning on all leds.
 *
 */
void n5_io_set_leds(void)
{
    nrf_gpio_pin_clear(LED_A);
    nrf_gpio_pin_clear(LED_B);
    nrf_gpio_pin_clear(LED_C);
    nrf_gpio_pin_clear(LED_D);
}

/**@brief Function for initializing all leds as outputs.
 *
 * Leds are initialized as off
 */
void n5_io_init_leds(void)
{
    nrf_gpio_cfg_output(LED_A);
    nrf_gpio_cfg_output(LED_B);
    nrf_gpio_cfg_output(LED_C);
    nrf_gpio_cfg_output(LED_D);

    n5_io_clear_leds();
}

/**@brief Function for turning on a specified led.
 *
 * @param[in] led_num   The led to turn on.
 */
void n5_io_turn_on_led(uint8_t led_num)
{
    nrf_gpio_pin_clear(led_num);
}

/**@brief Function for turning off a specified led.
 *
 * @param[in] led_num   The led to turn off.
 */
void n5_io_turn_off_led(uint8_t led_num)
{
    nrf_gpio_pin_set(led_num);
}

/**@brief Function for toggling a specified led.
 *
 * @param[in] led_num   The led to toggle.
 */
void n5_io_toggle_led(uint8_t led_num)
{
    nrf_gpio_pin_toggle(led_num);
}

/**
 *@}
 **/
#endif // 0
