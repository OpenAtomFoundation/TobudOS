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
/**@file
 * @defgroup leds
 * @{
 * @ingroup ant_auto_shared_channel
 *
 * @brief Header file containing platform specific LED functions
 */
#ifndef ANT_SHARED_MASTER_TO_MASTER_LEDS_H__
#define ANT_SHARED_MASTER_TO_MASTER_LEDS_H__

#if defined(BOARD_N5DK1)
    #include "n5sk_led.h"
#else
    #include "nrf_gpio.h"
    #include "boards.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


#define LED_INVALID     0xFF

static __INLINE void led_on(uint8_t led_num)
{
    if (led_num == LED_INVALID)
        return;

#if defined(BOARD_N5DK1)
    n5_io_turn_on_led(led_num);
#else
    bsp_board_led_on(led_num);
#endif
}

static __INLINE void led_off(uint8_t led_num)
{
    if (led_num == LED_INVALID)
        return;
#if defined(BOARD_N5DK1)
    n5_io_turn_off_led(led_num);
#else
    bsp_board_led_off(led_num);
#endif
}

static __INLINE void led_toggle(uint8_t led_num)
{
    if (led_num == LED_INVALID)
        return;
#if defined(BOARD_N5DK1)
    n5_io_toggle_led(led_num);
#else
    bsp_board_led_invert(led_num);
#endif
}

static __INLINE void led_clear(void)
{

#if defined(BOARD_N5DK1)
    n5_io_clear_leds();
#else
    bsp_board_leds_off();
#endif
}

static __INLINE void led_init(void)
{

#if defined(BOARD_N5DK1)
    n5_io_init_leds();
#else
    bsp_board_init(BSP_INIT_LEDS);

#endif
}

#ifdef __cplusplus
}
#endif

#endif // ANT_SHARED_MASTER_TO_MASTER_LEDS_H__
