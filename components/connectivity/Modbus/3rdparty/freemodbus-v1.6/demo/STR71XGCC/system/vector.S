/*
 * STR71X vectors for FreeModbus
 * Copyright C) 2005 Anglia Design, Spencer Oliver
 * Modifications (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: vector.S,v 1.1 2006/11/02 23:14:44 wolti Exp $
 */

/* ----------------------- System memory locations --------------------------*/
#include "FreeRTOSConfig.h"

/* ----------------------- System memory locations --------------------------*/
    .equ    EIC_ADDR,       0xFFFFF800    /* EIC base address */
    .equ    EIC_IVR_OFF,    0x18          /* Interrupt Vector Register offset */

/* ----------------------- Start implementation -----------------------------*/
    .global _vector_reset
    .global T0TIMI_Addr

/* ----------------------- Exception vectors --------------------------------*/
    .text
    .arm
.if ROM_RUN == 1
    .section .vectram, "ax"
.else
    .section .vectrom, "ax"
.endif
  
_vector_reset:
    ldr   pc, reset_handler_address       /* Reset Handler */
    ldr   pc, undef_handler_address
    ldr   pc, swi_handler_address
    ldr   pc, pabort_handler_address
    ldr   pc, dabort_handler_address
    nop
    ldr   pc, irq_handler_address
    ldr   pc, fiq_handler_address

/* ----------------------- Exception handlers address table -----------------*/

reset_handler_address:  .word _start
undef_handler_address:  .word _undef_handler
swi_handler_address:    .word vPortYieldProcessor
pabort_handler_address: .word _pabort_handler
dabort_handler_address: .word _dabort_handler
irq_handler_address:    .word _irq_handler
fiq_handler_address:    .word _fiq_handler

/* ----------------------- EIC IRQ handlers address table -------------------*/

T0TIMI_Addr:            .word prvvMBTimerIRQHandler
FLASH_Addr:             .word _undef_IRQhandler
RCCU_Addr:              .word _undef_IRQhandler
RTC_Addr:               .word _undef_IRQhandler
#if configUSE_PREEMPTION == 0
WDG_Addr:               .word vPortNonPreemptiveTick
#else
WDG_Addr:               .word vPortPreemptiveTick
#endif
XTI_Addr:               .word _undef_IRQhandler
USBHP_Addr:             .word _undef_IRQhandler
I2C0ITERR_Addr:         .word _undef_IRQhandler
I2C1ITERR_Addr:         .word _undef_IRQhandler
UART0_Addr:             .word prvvMBSerialIRQHandler
UART1_Addr:             .word _undef_IRQhandler
UART2_Aadd:             .word _undef_IRQhandler
UART3_Addr:             .word _undef_IRQhandler
BSPI0_Addr:             .word _undef_IRQhandler
BSPI1_Addr:             .word _undef_IRQhandler
I2C0_Addr:              .word _undef_IRQhandler
I2C1_Addr:              .word _undef_IRQhandler
CAN_Addr:               .word _undef_IRQhandler
ADC12_Addr:             .word _undef_IRQhandler
T1TIMI_Addr:            .word _undef_IRQhandler
T2TIMI_Addr:            .word _undef_IRQhandler
T3TIMI_Addr:            .word _undef_IRQhandler
                        .word 0
                        .word 0
                        .word 0
HDLC_Addr:              .word _undef_IRQhandler
USBLP_Addr:             .word _undef_IRQhandler
                        .word 0
                        .word 0
T0TOI_Addr:             .word _undef_IRQhandler
T0OC1_Addr:             .word _undef_IRQhandler
T0OC2_Addr:             .word _undef_IRQhandler

/* ----------------------- Default IRQ handlers -----------------------------*/

_undef_handler:
    b     _undef_handler

_swi_handler:
    b     _swi_handler

_pabort_handler:
    b     _pabort_handler

_dabort_handler:
    b     _dabort_handler

_irq_handler:
    ldr   pc, =(EIC_ADDR + EIC_IVR_OFF)

_fiq_handler:
    b   _fiq_handler

/* ----------------------- Default EIC IRQ handlers -------------------------*/
_undef_IRQhandler:
    b   _undef_IRQhandler

