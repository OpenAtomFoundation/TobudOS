/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
*
* \file
*
* \brief This file implements the platform abstraction layer APIs for gpio.
*
* \addtogroup  grPAL
* @{
*/


/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
#include "optiga/pal/pal_gpio.h"
#include "optiga/pal/pal_ifx_i2c_config.h"
#include "nrf_gpio.h"
#include "boards.h"

/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * LOCAL DATA
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * LOCAL ROUTINES
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * API IMPLEMENTATION
 *********************************************************************************************************************/

void pal_gpio_init()
{
    // Init power pins
    nrf_gpio_cfg_output((uint32_t)optiga_vdd_0.p_gpio_hw);

    // Set power pins to enable power
    nrf_gpio_pin_set((uint32_t)optiga_vdd_0.p_gpio_hw); // Enable power for onboard OPTIGA


    // Init reset pin
    nrf_gpio_cfg_output((uint32_t)(optiga_reset_0.p_gpio_hw));
}

void pal_gpio_set_high(const pal_gpio_t* p_gpio_context)
{
    if (p_gpio_context != NULL && p_gpio_context->p_gpio_hw != NULL)
    {
         nrf_gpio_pin_set((uint32_t)(p_gpio_context->p_gpio_hw));
    }
}

void pal_gpio_set_low(const pal_gpio_t* p_gpio_context)
{
    if (p_gpio_context != NULL && p_gpio_context->p_gpio_hw != NULL)
    {
        nrf_gpio_pin_clear((uint32_t)(p_gpio_context->p_gpio_hw));
    }
}

/**
* @}
*/

