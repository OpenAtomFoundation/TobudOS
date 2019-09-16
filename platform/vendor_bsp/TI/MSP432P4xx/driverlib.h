/*
 * -------------------------------------------
 *    MSP432 DriverLib - v3_40_00_10 
 * -------------------------------------------
 *
 * --COPYRIGHT--,BSD,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
#ifndef __DRIVERLIB__H_
#define __DRIVERLIB__H_

/* Common Modules */
#include "adc14.h"
#include "aes256.h"
#include "comp_e.h"
#include "cpu.h"
#include "crc32.h"
#include "cs.h"
#include "dma.h"
#include "eusci.h"
#include "fpu.h"
#include "gpio.h"
#include "i2c.h"
#include "interrupt.h"
#include "mpu.h"
#include "pcm.h"
#include "pmap.h"
#include "pss.h"
#include "ref_a.h"
#include "reset.h"
#include "rom.h"
#include "rom_map.h"
#include "rtc_c.h"
#include "spi.h"
#include "systick.h"
#include "timer32.h"
#include "timer_a.h"
#include "uart.h"
#include "wdt_a.h"

/* Specific modules for MSP432P4111 */
#if defined(__MCU_HAS_SYSCTL_A__)
#include "sysctl_a.h"
#else
#include "sysctl.h"
#endif

#if defined(__MCU_HAS_FLCTL_A__)
#include "flash_a.h"
#else
#include "flash.h"
#endif

#if defined(__MCU_HAS_LCD_F__)
#include "lcd_f.h"
#endif

/* Offset Definitions */
#define HWREG8(x)         (*((volatile uint8_t *)(x)))
#define HWREG16(x)        (*((volatile uint16_t *)(x)))
#define HWREG32(x)        (*((volatile uint32_t *)(x)))
#define HWREG(x)          (HWREG16(x))
#define HWREG8_L(x)       (*((volatile uint8_t *)((uint8_t *)&x)))
#define HWREG8_H(x)       (*((volatile uint8_t *)(((uint8_t *)&x)+1)))
#define HWREG16_L(x)      (*((volatile uint16_t *)((uint16_t *)&x)))
#define HWREG16_H(x)      (*((volatile uint16_t *)(((uint16_t *)&x)+1)))


#endif
