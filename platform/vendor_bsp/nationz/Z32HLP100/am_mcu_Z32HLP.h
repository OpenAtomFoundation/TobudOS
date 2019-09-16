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
 * @file     am_mcu_Z32HLP.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Top Include for Z32HLP100  class devices.
 *
 *  This file provides all the includes necessary for an Z32HLP100  device.
 *
 ******************************************************************************/
#ifndef am_mcu_Z32HLP_H
#define am_mcu_Z32HLP_H

//*****************************************************************************
//
// C99
//
//*****************************************************************************
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#ifdef __IAR_SYSTEMS_ICC__
#include "intrinsics.h"     // __CLZ() and other intrinsics
#endif

#define AM_HAL_DEBUG_NO_ASSERT

//*****************************************************************************
//
// Registers
//
//*****************************************************************************
#include "regs/am_reg_adc.h"
#include "regs/am_reg_base_addresses.h"
#include "regs/am_reg_clkgen.h"
#include "regs/am_reg_ctimer.h"
#include "regs/am_reg_gpio.h"
#include "regs/am_reg_iomstr.h"
#include "regs/am_reg_ioslave.h"
#include "regs/am_reg_itm.h"
#include "regs/am_reg_jedec.h"
#include "regs/am_reg_macros.h"
#include "regs/am_reg_mcuctrl.h"
#include "regs/am_reg_nvic.h"
#include "regs/am_reg_rtc.h"
#include "regs/am_reg_rstgen.h"
#include "regs/am_reg_sysctrl.h"
#include "regs/am_reg_systick.h"
#include "regs/am_reg_tpiu.h"
#include "regs/am_reg_uart.h"
#include "regs/am_reg_vcomp.h"
#include "regs/am_reg_wdt.h"

//*****************************************************************************
//
// HAL
//
//*****************************************************************************
#include "hal/am_hal_adc.h"
#include "hal/am_hal_cachectrl.h"
#include "hal/am_hal_clkgen.h"
#include "hal/am_hal_ctimer.h"
#include "hal/am_hal_debug.h"
#include "hal/am_hal_flash.h"
#include "hal/am_hal_global.h"
#include "hal/am_hal_gpio.h"
#include "hal/am_hal_i2c_bit_bang.h"
#include "hal/am_hal_interrupt.h"
#include "hal/am_hal_iom.h"
#include "hal/am_hal_ios.h"
#include "hal/am_hal_itm.h"
#include "hal/am_hal_mcuctrl.h"
#include "hal/am_hal_otp.h"
#include "hal/am_hal_pin.h"
#include "hal/am_hal_pwrctrl.h"
#include "hal/am_hal_queue.h"
#include "hal/am_hal_reset.h"
#include "hal/am_hal_rtc.h"
#include "hal/am_hal_sysctrl.h"
#include "hal/am_hal_systick.h"
#include "hal/am_hal_tpiu.h"
#include "hal/am_hal_uart.h"
#include "hal/am_hal_vcomp.h"
#include "hal/am_hal_wdt.h"


#endif // am_mcu_Z32HLP_H

