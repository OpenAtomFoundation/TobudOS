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
#ifndef N5STARTERKIT_H
#define N5STARTERKIT_H

#if 0 // @todo FIX ME!
#include "nrf_gpio.h"
#include "boards.h"
#endif // 0

#include "bsp.h"

#ifdef __cplusplus
extern "C" {
#endif

//IO board active low leds
#define LED_A        BSP_LED_0 //LED A on N5 Starter Kit IO Board
#define LED_B        BSP_LED_1 //LED B on N5 Starter Kit IO Board
#define LED_C        BSP_LED_2 //LED C on N5 Starter Kit IO Board
#define LED_D        BSP_LED_3 //LED D on N5 Starter Kit IO Board

//IO board pull-up buttons
#define BUTTON_A     BSP_BUTTON_0 //BUTTON A on N5 Starter Kit IO Board
#define BUTTON_B     BSP_BUTTON_1 //BUTTON B on N5 Starter Kit IO Board
#define BUTTON_C     BSP_BUTTON_2 //BUTTON C on N5 Starter Kit IO Board
#define BUTTON_D     BSP_BUTTON_3 //BUTTON D on N5 Starter Kit IO Board


#if 0 // @todo REMOVE ME!
#define BUTTON_PULL  NRF_GPIO_PIN_PULLUP

//Battery board pull-up switches
#define SWITCH_1     5  // Switch 1 on N5 Starter Kit Battery Board
#define SWITCH_2     0  // Switch 2 on N5 Starter Kit Battery Board
#define SWITCH_3     6  // Switch 3 on N5 Starter Kit Battery Board
#define SWITCH_4     24 // Switch 4 on N5 Starter Kit Battery Board
#define SWITCH_5     9  // Switch 5 on N5 Starter Kit Battery Board
#define SWITCH_PULL  NRF_GPIO_PIN_PULLUP
#endif // 0


#ifdef __cplusplus
}
#endif

#endif
