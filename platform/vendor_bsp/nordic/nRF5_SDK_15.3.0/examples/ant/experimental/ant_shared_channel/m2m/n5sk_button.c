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
#include "n5sk_button.h"
#include "nrf_gpio.h"
#if defined(BOARD_N5DK1)
    #include "n5_starterkit.h"
#endif


////////////////////////////////////////////////////////////////////////////////
// Description: Button and switch control functions for the N5 Starter Kit
// IO Board (4 Buttons) and Battery Board (5 Buttons)
// Note: N5Starter Kit IO Board buttons and Battery Board switches are pull up.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////

/**@brief Function for initializing the IO board buttons as inputs.

 */
#if 0 // @todo REMOVE ME!
void n5_io_button_init(void)
{
    nrf_gpio_cfg_input(BUTTON_A, BUTTON_PULL);
    nrf_gpio_cfg_input(BUTTON_B, BUTTON_PULL);
    nrf_gpio_cfg_input(BUTTON_C, BUTTON_PULL);
    nrf_gpio_cfg_input(BUTTON_D, BUTTON_PULL);
}

/**@brief Function for initializing the battery board switches as inputs.

 */

void n5_io_switch_init(void)
{
    nrf_gpio_cfg_input(SWITCH_1, SWITCH_PULL);
    nrf_gpio_cfg_input(SWITCH_2, SWITCH_PULL);
    nrf_gpio_cfg_input(SWITCH_3, SWITCH_PULL);
    nrf_gpio_cfg_input(SWITCH_4, SWITCH_PULL);
    nrf_gpio_cfg_input(SWITCH_5, SWITCH_PULL);
}
#endif // 0
