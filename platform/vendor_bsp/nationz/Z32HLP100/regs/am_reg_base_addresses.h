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
 * @file     am_reg_base_addresses.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Register defines for all module base addresses
 ******************************************************************************/
#ifndef AM_REG_BASE_ADDRESSES_H
#define AM_REG_BASE_ADDRESSES_H

// ARM standard register space (needed for macros)
#define REG_ITM_BASEADDR                         0x00000000
#define REG_JEDEC_BASEADDR                       0x00000000
#define REG_NVIC_BASEADDR                        0x00000000
#define REG_SYSCTRL_BASEADDR                     0x00000000
#define REG_SYSTICK_BASEADDR                     0x00000000
#define REG_TPIU_BASEADDR                        0x00000000

// Peripheral register space
#define REG_ADC_BASEADDR                         0x50008000
#define REG_CLKGEN_BASEADDR                      0x40004000
#define REG_MCUCTRL_BASEADDR                     0x40020000
#define REG_CTIMER_BASEADDR                      0x40008000
#define REG_GPIO_BASEADDR                        0x40010000
#define REG_IOMSTR0_BASEADDR                     0x50004000
#define REG_IOMSTR1_BASEADDR                     0x50005000
#define REG_IOMSTR_BASEADDR                      0x50004000
#define REG_IOSLAVE_BASEADDR                     0x50000000
#define REG_RSTGEN_BASEADDR                      0x40000000
#define REG_RTC_BASEADDR                         0x40004000
#define REG_UART_BASEADDR                        0x4001C000
#define REG_VCOMP_BASEADDR                       0x4000C000
#define REG_WDT_BASEADDR                         0x40024000


// SRAM address space
#define SRAM_BASEADDR                            (0x10000000UL)
#define SRAM_TOTALSIZE                                                     (0x00010000UL)
#define SRAM_BANKSIZE  0x2000

#endif // AM_REG_BASE_ADDRESSES_H

