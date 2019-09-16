/** ----------------------------------------------------------------------------
 *         Nationz Technology Software Support  -  NATIONZ  -
 * -----------------------------------------------------------------------------
 * Copyright (c) 2013гн2018, Nationz Corporation  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaiimer below.
 * 
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the disclaimer below in the documentation and/or
 * other materials provided with the distribution. 
 * 
 * Nationz's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission. 
 * 
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONZ "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
 * -----------------------------------------------------------------------------
 */

/** ****************************************************************************
 * @copyright      Nationz Co.,Ltd 
 *        Copyright (c) 2013гн2018 All Rights Reserved 
 *******************************************************************************
 * @file     am_hal_pwrctrl.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Function stubs for accessing and configuring the PWR controller.
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup pwrctrl Power Control
 * @ingroup hal
 * @{
 ******************************************************************************/
#ifndef AM_HAL_PWRCTRL_H
#define AM_HAL_PWRCTRL_H

//*****************************************************************************
//
// Peripheral enable bits for am_hal_pwrctrl_periph_enable/disable()
//
//*****************************************************************************
#define AM_HAL_PWRCTRL_ADC      AM_REG_PWRCTRL_DEVICEEN_ADC_EN
#define AM_HAL_PWRCTRL_IOM0     AM_REG_PWRCTRL_DEVICEEN_IO_MASTER0_EN
#define AM_HAL_PWRCTRL_IOM1     AM_REG_PWRCTRL_DEVICEEN_IO_MASTER1_EN
#define AM_HAL_PWRCTRL_IOM2     AM_REG_PWRCTRL_DEVICEEN_IO_MASTER2_EN
#define AM_HAL_PWRCTRL_IOM3     AM_REG_PWRCTRL_DEVICEEN_IO_MASTER3_EN
#define AM_HAL_PWRCTRL_IOM4     AM_REG_PWRCTRL_DEVICEEN_IO_MASTER4_EN
#define AM_HAL_PWRCTRL_IOM5     AM_REG_PWRCTRL_DEVICEEN_IO_MASTER5_EN
#define AM_HAL_PWRCTRL_IOS      AM_REG_PWRCTRL_DEVICEEN_IO_SLAVE_EN
#define AM_HAL_PWRCTRL_PDM      AM_REG_PWRCTRL_DEVICEEN_PDM_EN
#define AM_HAL_PWRCTRL_UART0    AM_REG_PWRCTRL_DEVICEEN_UART0_EN
#define AM_HAL_PWRCTRL_UART1    AM_REG_PWRCTRL_DEVICEEN_UART1_EN

//*****************************************************************************
//
// Macro to set the appropriate IOM peripheral when using
//  am_hal_pwrctrl_periph_enable()/disable().
// For Z32HLP200 , the module argument must resolve to be a value from 0-5.
//
//*****************************************************************************
#define AM_HAL_PWRCTRL_IOM(module)

//*****************************************************************************
//
// Macro to set the appropriate UART peripheral when using
//  am_hal_pwrctrl_periph_enable()/disable().
// For Z32HLP200 , the module argument must resolve to be a value from 0-1.
//
//*****************************************************************************
#define AM_HAL_PWRCTRL_UART(module)


#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Function prototypes
//
//*****************************************************************************
#define am_hal_pwrctrl_periph_enable(x)
#define am_hal_pwrctrl_periph_disable(x)
#define am_hal_pwrctrl_memory_enable(x)
#define am_hal_pwrctrl_bucks_enable(x)
#define am_hal_pwrctrl_bucks_disable(x)
#define am_hal_pwrctrl_low_power_init(x)

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_PWRCTRL_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
