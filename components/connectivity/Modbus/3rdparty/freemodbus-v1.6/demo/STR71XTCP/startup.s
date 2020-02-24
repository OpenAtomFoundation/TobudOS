/*****************************************************************************
 * Copyright (c) 2001, 2002 Rowley Associates Limited.                       *
 *                                                                           *
 * This file may be distributed under the terms of the License Agreement     *
 * provided with this software.                                              *
 *                                                                           *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE   *
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. *
 *****************************************************************************
 *                                 WARNING                                   *
 *****************************************************************************
 * There have been a lot of modifications to this file for the FreeRTOS port *
 * for the following reason:                                                 *
 *                                                                           *
 *  - We need support for the interrupt vectors from the EIC.                *
 *  - The default file did only make a copy of the standard vectors when in  *
 *    FLASH mode. Because in the used EIC configuration the new programm     *
 *    counter value is loaded with a relative offset to the current pc the   *
 *    interrupt vectors must also be available in the RAM.                   *
 *  - The clock source configuration did not fit for the specific            *
 *    needs of the author.                                                   *
 *                                                                           *
 * All modifications copyright (c) 2006, Christian Walter <wolti@sil.at>     *
 *****************************************************************************/

#include "FreeRTOSConfig.h"             /* import configUSE_PREEMPTION */

    .equ    EIC_BASE,       0xFFFFF800  /* EIC base address. */
    .equ    EIC_IVR_OFF,    0x18        /* EIC interrupt vector register. */

    .section .vectors, "ax"
    .code 32
    .align 0
  
/*****************************************************************************
 * Exception vectors                                                         *
 *****************************************************************************/
_vectors_start:
    ldr pc, [pc, #reset_handler_address - . - 8]  /* reset */
    ldr pc, [pc, #undef_handler_address - . - 8]  /* undefined instruction */
    ldr pc, [pc, #swi_handler_address - . - 8]    /* swi handler */
    ldr pc, [pc, #pabort_handler_address - . - 8] /* abort prefetch */
    ldr pc, [pc, #dabort_handler_address - . - 8] /* abort data */
    nop
    ldr pc, [pc, #irq_handler_address - . - 8]
    ldr pc, [pc, #fiq_handler_address - . - 8]    /* fiq */

reset_handler_address:  .word _reset_handler
undef_handler_address:  .word _undef_handler
swi_handler_address:    .word vPortYieldProcessor
pabort_handler_address: .word _pabort_handler
dabort_handler_address: .word _dabort_handler
irq_handler_address:    .word _irq_handler
fiq_handler_address:    .word _fiq_handler

/*******************************************************************************
 * EIC interrupt vectors
 ******************************************************************************/
.global T0TIMI_address
.global T0TIMI_Addr
T0TIMI_Addr:            /* for compatibility with STR71X library. */
T0TIMI_address:         .word  _undef_IRQhandler
FLASH_address:          .word  _undef_IRQhandler
RCCU_address:           .word  _undef_IRQhandler
RTC_Addr:               .word  _undef_IRQhandler
#if configUSE_PREEMPTION == 0
WDG_Addr:               .word   vPortNonPreemptiveTick
#else
WDG_Addr:               .word   vPortPreemptiveTick
#endif    
XTI_Addr:               .word  _undef_IRQhandler
USBHP_Addr:             .word  _undef_IRQhandler
I2C0ITERR_Addr:         .word  _undef_IRQhandler
I2C1ITERR_ADDR:         .word  _undef_IRQhandler
UART0_ADDR:             .word  sio_uart0_irq
UART1_ADDR:             .word  sio_uart1_irq
UART2_ADDR:             .word  _undef_IRQhandler
UART3_ADDR:             .word  _undef_IRQhandler
BSPI0_ADDR:             .word  _undef_IRQhandler
BSPI1_Addr:             .word  _undef_IRQhandler
I2C0_Addr:              .word  _undef_IRQhandler
I2C1_Addr:              .word  _undef_IRQhandler
CAN_Addr:               .word  _undef_IRQhandler
ADC12_Addr:             .word  _undef_IRQhandler
T1TIMI_Addr:            .word  _undef_IRQhandler
T2TIMI_Addr:            .word  _undef_IRQhandler
T3TIMI_Addr:            .word  _undef_IRQhandler
                        .word   0                  /* reserved */
                        .word   0                  /* reserved */
                        .word   0                  /* reserved */
HDLC_Addr:              .word  _undef_IRQhandler
USBLP_Addr:             .word  _undef_IRQhandler
                        .word  0                   /* reserved */
                        .word  0                   /* reserved */
T0TOI_Addr:             .word  _undef_IRQhandler
T0OC1_Addr:             .word  _undef_IRQhandler
T0OC2_Addr:             .word  _undef_IRQhandler
_vectors_end:

/*******************************************************************************
 * If flash execution is used and the RAM is mapped at address 0 we also need
 * to place a copy of the interrupt vector address table in the RAM.
 ******************************************************************************/
 #ifdef __FLASH_BUILD  
    .section .vectors_ram, "ax"
    .code 32
    .align 0
_vectors_ram:
    .fill _vectors_end - _vectors_start
#endif

/******************************************************************************
 * Default exception handlers                                                 *
 ******************************************************************************/
     .section .init, "ax"
    .code 32
    .align 0

_reset_handler:
#ifdef __FLASH_BUILD    
    ldr r0, =_vectors_start
    ldr r1, =_vectors_end
    sub r1, r1, r0          /* r2 = number of bytes to copy */
    ldr r2, =0x20000000     /* r0 = start address to place copy */
_vector_copy:
    ldr r3, [r0], #4        /* read a word from the source */
    str r3, [r2], #4        /* copy the word to destination */
    subs  r1, r1, #4        /* decrement number of words to copy */
    bne _vector_copy
#endif

    /* Configure system and jump to the _start entry point. */
    ldr r0, =0x6C000000
    ldr r1, =0x801D
    str r1, [r0, #4]        /* set up the emi to 1 wait state */

    ldr r0, =0xA0000000
    ldr r1, =0x1c2
    str r1, [r0, #0x50]     /* set up the internal RAM at 0x0 */
    
    /* After setup the folling clock configuration will be active. Note
     * that the value of CK is dependent on the target oscillator.
     *
     * CK    = 4Mhz
     * CLK2  = 2Mhz (with DIV2 = 1)
     * RCLK  = 16 * CLK2 / 1 = 32Mhz
     * MCLK  = RCLK
     * PCLK1 = RCLK/2
     * PCLK2 = RCLK/2
     */
     
    /* set RCLK to 16 * CLK2 / 1 = 32Mhz    
     * (0x0071 = FREEN=1, FREF_RANGE=1, MX1:0=11b, DX2:0=000b) */
    ldr r1, =0x00F0        
    str r1, [r0, #0x18]

    /* (0x8009 = DIV2=1, CK2_16=1, CKU_CKSEL=1) */
    ldr r1, =0x8009
    str r1, [r0, #0x8]

    /* cpu and memory clock div = 1 */
    ldr r1, =0x0
    str r1, [r0, #0x40]

    /* pclk1 = rclk/1 and pclk2 = rclk/1 = 32Mhz 
     * (0x0101 = FACT2_9:8=01b, FACT1_1:0=01b) */
    ldr r1, =0x0000
    str r1, [r0, #0x44]

    /* Jump to the default C runtime startup code. */
    B _start

_undef_handler:
    b   _undef_handler

_swi_handler:
    b   vPortYieldProcessor

_pabort_handler:
    b   _pabort_handler

_dabort_handler:
    b   _dabort_handler

_irq_handler:
    ldr pc, =(EIC_BASE + EIC_IVR_OFF)
    
_fiq_handler:
    b   _fiq_handler

/******************************************************************************
 * Default IRQ handlers                                                       *
 ******************************************************************************/
_undef_IRQhandler:
    b   _undef_IRQhandler
