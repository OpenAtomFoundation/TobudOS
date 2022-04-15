;/*!
;    \file    startup_gd32f3x0.s
;    \brief   start up file
;
;    \version 2017-06-06, V1.0.0, firmware for GD32F3x0
;    \version 2019-06-01, V2.0.0, firmware for GD32F3x0
;    \version 2020-09-30, V2.1.0, firmware for GD32F3x0
;    \version 2022-01-06, V2.2.0, firmware for GD32F3x0
;*/

;/*
    ;Copyright (c) 2022, GigaDevice Semiconductor Inc.

    ;Redistribution and use in source and binary forms, with or without modification, 
;are permitted provided that the following conditions are met:

    ;1. Redistributions of source code must retain the above copyright notice, this 
       ;list of conditions and the following disclaimer.
    ;2. Redistributions in binary form must reproduce the above copyright notice, 
       ;this list of conditions and the following disclaimer in the documentation 
       ;and/or other materials provided with the distribution.
    ;3. Neither the name of the copyright holder nor the names of its contributors 
       ;may be used to endorse or promote products derived from this software without 
       ;specific prior written permission.

    ;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
;AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
;WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
;IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
;INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
;NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
;PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
;WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
;ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
;OF SUCH DAMAGE.
;*/

.syntax unified
.cpu cortex-m4
.fpu fpv4-sp-d16
.thumb

.section  .isr_vector,"a",%progbits
.type  g_pfnVectors, %object
g_pfnVectors:
                .word     _estack                           // Top of Stack
                .word     Reset_Handler                     // Reset Handler
                .word     NMI_Handler                       // NMI Handler
                .word     HardFault_Handler                 // Hard Fault Handler
                .word     MemManage_Handler                 // MPU Fault Handler
                .word     BusFault_Handler                  // Bus Fault Handler
                .word     UsageFault_Handler                // Usage Fault Handler
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     SVC_Handler                       // SVCall Handler
                .word     DebugMon_Handler                  // Debug Monitor Handler
                .word     0                                 // Reserved
                .word     PendSV_Handler                    // PendSV Handler
                .word     SysTick_Handler                   // SysTick Handler

                /* external interrupts handler */
                .word     WWDGT_IRQHandler                  // 16:Window Watchdog Timer
                .word     LVD_IRQHandler                    // 17:LVD through EXTI Line detect
                .word     RTC_IRQHandler                    // 18:RTC through EXTI Line
                .word     FMC_IRQHandler                    // 19:FMC
                .word     RCU_CTC_IRQHandler                // 20:RCU and CTC
                .word     EXTI0_1_IRQHandler                // 21:EXTI Line 0 and EXTI Line 1
                .word     EXTI2_3_IRQHandler                // 22:EXTI Line 2 and EXTI Line 3
                .word     EXTI4_15_IRQHandler               // 23:EXTI Line 4 to EXTI Line 15
                .word     TSI_IRQHandler                    // 24:TSI
                .word     DMA_Channel0_IRQHandler           // 25:DMA Channel 0 
                .word     DMA_Channel1_2_IRQHandler         // 26:DMA Channel 1 and DMA Channel 2
                .word     DMA_Channel3_4_IRQHandler         // 27:DMA Channel 3 and DMA Channel 4
                .word     ADC_CMP_IRQHandler                // 28:ADC and Comparator 0-1
                .word     TIMER0_BRK_UP_TRG_COM_IRQHandler  // 29:TIMER0 Break,Update,Trigger and Commutation
                .word     TIMER0_Channel_IRQHandler         // 30:TIMER0 Channel Capture Compare
                .word     TIMER1_IRQHandler                 // 31:TIMER1
                .word     TIMER2_IRQHandler                 // 32:TIMER2
                .word     TIMER5_DAC_IRQHandler             // 33:TIMER5 and DAC
                .word     0                                 // Reserved
                .word     TIMER13_IRQHandler                // 35:TIMER13
                .word     TIMER14_IRQHandler                // 36:TIMER14
                .word     TIMER15_IRQHandler                // 37:TIMER15
                .word     TIMER16_IRQHandler                // 38:TIMER16
                .word     I2C0_EV_IRQHandler                // 39:I2C0 Event
                .word     I2C1_EV_IRQHandler                // 40:I2C1 Event
                .word     SPI0_IRQHandler                   // 41:SPI0
                .word     SPI1_IRQHandler                   // 42:SPI1
                .word     USART0_IRQHandler                 // 43:USART0
                .word     USART1_IRQHandler                 // 44:USART1
                .word     0                                 // Reserved
                .word     CEC_IRQHandler                    // 46:CEC
                .word     0                                 // Reserved
                .word     I2C0_ER_IRQHandler                // 48:I2C0 Error
                .word     0                                 // Reserved
                .word     I2C1_ER_IRQHandler                // 50:I2C1 Error
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     USBFS_WKUP_IRQHandler             // 58:USBFS Wakeup
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     DMA_Channel5_6_IRQHandler         // 64:DMA Channel5 and Channel6 
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     0                                 // Reserved
                .word     USBFS_IRQHandler                  // 83:USBFS
                /* set size of g_pfnVectors */
                .size g_pfnVectors, . - g_pfnVectors

.weak  Reset_Handler
.type  Reset_Handler, %function
Reset_Handler:
    ldr r1, =_sidata
    ldr r2, =_sdata
    ldr r3, =_edata

    subs r3, r2
    ble fill_bss_start

loop_copy_data:
    subs r3, #4
    ldr r0, [r1,r3]
    str r0, [r2,r3]
    bgt loop_copy_data

fill_bss_start:
    ldr r1, =__bss_start
    ldr r2, =__bss_end
    movs r0, 0
    subs r2, r1
    ble startup_enter

loop_fill_bss:
    subs r2, #4
    str r0, [r1, r2]
    bgt loop_fill_bss

startup_enter:
    bl SystemInit
    bl main

    .weak   NMI_Handler
    .type   NMI_Handler, %function
NMI_Handler:
    b       .
    .size   NMI_Handler, . - NMI_Handler

    .weak   HardFault_Handler
    .type   HardFault_Handler, %function
HardFault_Handler:
    b       .
    .size   HardFault_Handler, . - HardFault_Handler

    .weak   MemManage_Handler
    .type   MemManage_Handler, %function
MemManage_Handler:
    b       .
    .size   MemManage_Handler, . - MemManage_Handler

    .weak   BusFault_Handler
    .type   BusFault_Handler, %function
BusFault_Handler:
    b       .
    .size   BusFault_Handler, . - BusFault_Handler

    .weak   UsageFault_Handler
    .type   UsageFault_Handler, %function
UsageFault_Handler:
    b       .
    .size   UsageFault_Handler, . - UsageFault_Handler

    .weak   SVC_Handler
    .type   SVC_Handler, %function
SVC_Handler:
    b       .
    .size   SVC_Handler, . - SVC_Handler

    .weak   DebugMon_Handler
    .type   DebugMon_Handler, %function
DebugMon_Handler:
    b       .
    .size   DebugMon_Handler, . - DebugMon_Handler

    .weak   PendSV_Handler
    .type   PendSV_Handler, %function
PendSV_Handler:
    b       .
    .size   PendSV_Handler, . - PendSV_Handler

    .weak   SysTick_Handler
    .type   SysTick_Handler, %function
SysTick_Handler:
    b       .
    .size   SysTick_Handler, . - SysTick_Handler

    .section  .text.Default_Handler,"ax",%progbits
    .type  Default_Handler, %function
Default_Handler:
    b  .
    .size  Default_Handler, . - Default_Handler

    .macro  IRQ handler
    .weak   \handler
    .set    \handler, Default_Handler
    .endm
/* external interrupts handler */
    IRQ WWDGT_IRQHandler
    IRQ LVD_IRQHandler
    IRQ RTC_IRQHandler
    IRQ FMC_IRQHandler
    IRQ RCU_CTC_IRQHandler
    IRQ EXTI0_1_IRQHandler
    IRQ EXTI2_3_IRQHandler
    IRQ EXTI4_15_IRQHandler
    IRQ TSI_IRQHandler
    IRQ DMA_Channel0_IRQHandler
    IRQ DMA_Channel1_2_IRQHandler
    IRQ DMA_Channel3_4_IRQHandler
    IRQ ADC_CMP_IRQHandler
    IRQ TIMER0_BRK_UP_TRG_COM_IRQHandler
    IRQ TIMER0_Channel_IRQHandler
    IRQ TIMER1_IRQHandler
    IRQ TIMER2_IRQHandler
    IRQ TIMER5_DAC_IRQHandler
    IRQ TIMER13_IRQHandler
    IRQ TIMER14_IRQHandler
    IRQ TIMER15_IRQHandler
    IRQ TIMER16_IRQHandler
    IRQ I2C0_EV_IRQHandler
    IRQ I2C1_EV_IRQHandler
    IRQ SPI0_IRQHandler
    IRQ SPI1_IRQHandler
    IRQ USART0_IRQHandler
    IRQ USART1_IRQHandler
    IRQ CEC_IRQHandler
    IRQ I2C0_ER_IRQHandler
    IRQ I2C1_ER_IRQHandler
    IRQ USBFS_WKUP_IRQHandler
    IRQ DMA_Channel5_6_IRQHandler
    IRQ USBFS_IRQHandler
