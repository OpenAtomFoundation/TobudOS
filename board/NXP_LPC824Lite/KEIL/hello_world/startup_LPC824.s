;/*****************************************************************************
; * @file:    startup_LPC824.s
; * @purpose: CMSIS Cortex-M0 Core Device Startup File for the
; *           LPC824
; * @version: 1.2
; * @date:    2017-6-8
; *
; * The Clear BSD License
; * Copyright 1997-2016 Freescale Semiconductor, Inc.
; * Copyright 2016-2018 NXP
; * All rights reserved.
; *
; * Redistribution and use in source and binary forms, with or without
; * modification, are permitted (subject to the limitations in the
; * disclaimer below) provided that the following conditions are met:
; *
; * * Redistributions of source code must retain the above copyright
; *   notice, this list of conditions and the following disclaimer.
; *
; * * Redistributions in binary form must reproduce the above copyright
; *   notice, this list of conditions and the following disclaimer in the
; *   documentation and/or other materials provided with the distribution.
; *
; * * Neither the name of the copyright holder nor the names of its
; *   contributors may be used to endorse or promote products derived from
; *   this software without specific prior written permission.
; *
; * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
; * GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
; * HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
; * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
; * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
; * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
; * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
; * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
; * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
; * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
; * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; *
; *------- <<< Use Configuration Wizard in Context Menu >>> ------------------
; *
; *****************************************************************************/
LOS_Heap_Min_Size   EQU     0x400

                AREA    LOS_HEAP, NOINIT, READWRITE, ALIGN=3
__los_heap_base
LOS_Heap_Mem    SPACE   LOS_Heap_Min_Size


                AREA    LOS_HEAP_INFO, DATA, READONLY, ALIGN=2
                IMPORT  |Image$$ARM_LIB_STACK$$ZI$$Base|
                EXPORT  __LOS_HEAP_ADDR_START__
                EXPORT  __LOS_HEAP_ADDR_END__
__LOS_HEAP_ADDR_START__
                DCD     __los_heap_base
__LOS_HEAP_ADDR_END__
                DCD     |Image$$ARM_LIB_STACK$$ZI$$Base| - 1
					

                PRESERVE8
                THUMB

; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                IMPORT  |Image$$ARM_LIB_STACK$$ZI$$Limit|

__Vectors       DCD     |Image$$ARM_LIB_STACK$$ZI$$Limit| ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler

                DCD     NMI_Handler
                DCD     HardFault_Handler
                DCD     0
                DCD     0
                DCD     0
__vector_table_0x1c
                DCD     0                         ; Checksum of the first 7 words
                DCD     0
                DCD     0                         ; Enhanced image marker, set to 0x0 for legacy boot
                DCD     0                         ; Pointer to enhanced boot block, set to 0x0 for legacy boot
                DCD     SVC_Handler
                DCD     0
                DCD     0
                DCD     PendSV_Handler
                DCD     SysTick_Handler

                ; External Interrupts
                DCD     SPI0_IRQHandler  ; SPI0 interrupt
                DCD     SPI1_IRQHandler  ; SPI1 interrupt
                DCD     Reserved18_IRQHandler  ; Reserved interrupt
                DCD     USART0_IRQHandler  ; USART0 interrupt
                DCD     USART1_IRQHandler  ; USART1 interrupt
                DCD     USART2_IRQHandler  ; USART2 interrupt
                DCD     Reserved22_IRQHandler  ; Reserved interrupt
                DCD     I2C1_IRQHandler  ; I2C1 interrupt
                DCD     I2C0_IRQHandler  ; I2C0 interrupt
                DCD     SCT0_IRQHandler  ; State configurable timer interrupt
                DCD     MRT0_IRQHandler  ; Multi-rate timer interrupt
                DCD     CMP_CAPT_IRQHandler  ; Analog comparator interrupt or Capacitive Touch interrupt
                DCD     WDT_IRQHandler  ; Windowed watchdog timer interrupt
                DCD     BOD_IRQHandler  ; BOD interrupts
                DCD     FLASH_IRQHandler  ; flash interrupt
                DCD     WKT_IRQHandler  ; Self-wake-up timer interrupt
                DCD     ADC0_SEQA_IRQHandler  ; ADC0 sequence A completion.
                DCD     ADC0_SEQB_IRQHandler  ; ADC0 sequence B completion.
                DCD     ADC0_THCMP_IRQHandler  ; ADC0 threshold compare and error.
                DCD     ADC0_OVR_IRQHandler  ; ADC0 overrun
                DCD     DMA0_IRQHandler  ; DMA0 interrupt
                DCD     I2C2_IRQHandler  ; I2C2 interrupt
                DCD     I2C3_IRQHandler  ; I2C3 interrupt
                DCD     Reserved39_IRQHandler  ; Reserved interrupt
                DCD     PIN_INT0_IRQHandler  ; Pin interrupt 0 or pattern match engine slice 0 interrupt
                DCD     PIN_INT1_IRQHandler  ; Pin interrupt 1 or pattern match engine slice 1 interrupt
                DCD     PIN_INT2_IRQHandler  ; Pin interrupt 2 or pattern match engine slice 2 interrupt
                DCD     PIN_INT3_IRQHandler  ; Pin interrupt 3 or pattern match engine slice 3 interrupt
                DCD     PIN_INT4_IRQHandler  ; Pin interrupt 4 or pattern match engine slice 4 interrupt
                DCD     PIN_INT5_IRQHandler  ; Pin interrupt 5 or pattern match engine slice 5 interrupt
                DCD     PIN_INT6_IRQHandler  ; Pin interrupt 6 or pattern match engine slice 6 interrupt
                DCD     PIN_INT7_IRQHandler  ; Pin interrupt 7 or pattern match engine slice 7 interrupt

                AREA    |.text|, CODE, READONLY

; Reset Handler
Reset_Handler   PROC
                EXPORT  Reset_Handler               [WEAK]
                IMPORT  SystemInit
                IMPORT  __main

                LDR     r0, =SystemInit
                BLX     r0
                LDR     r0, =__main
                BX      r0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)
NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP

HardFault_Handler \
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP

SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP

PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP

SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

SPI0_IRQHandler\
                PROC
                EXPORT     SPI0_IRQHandler        [WEAK]
                LDR        R0, =SPI0_DriverIRQHandler
                BX         R0
                ENDP

SPI1_IRQHandler\
                PROC
                EXPORT     SPI1_IRQHandler        [WEAK]
                LDR        R0, =SPI1_DriverIRQHandler
                BX         R0
                ENDP

Reserved18_IRQHandler\
                PROC
                EXPORT     Reserved18_IRQHandler        [WEAK]
                LDR        R0, =Reserved18_DriverIRQHandler
                BX         R0
                ENDP

USART0_IRQHandler\
                PROC
                EXPORT     USART0_IRQHandler        [WEAK]
                LDR        R0, =USART0_DriverIRQHandler
                BX         R0
                ENDP

USART1_IRQHandler\
                PROC
                EXPORT     USART1_IRQHandler        [WEAK]
                LDR        R0, =USART1_DriverIRQHandler
                BX         R0
                ENDP

USART2_IRQHandler\
                PROC
                EXPORT     USART2_IRQHandler        [WEAK]
                LDR        R0, =USART2_DriverIRQHandler
                BX         R0
                ENDP

Reserved22_IRQHandler\
                PROC
                EXPORT     Reserved22_IRQHandler        [WEAK]
                LDR        R0, =Reserved22_DriverIRQHandler
                BX         R0
                ENDP

I2C1_IRQHandler\
                PROC
                EXPORT     I2C1_IRQHandler        [WEAK]
                LDR        R0, =I2C1_DriverIRQHandler
                BX         R0
                ENDP

I2C0_IRQHandler\
                PROC
                EXPORT     I2C0_IRQHandler        [WEAK]
                LDR        R0, =I2C0_DriverIRQHandler
                BX         R0
                ENDP

SCT0_IRQHandler\
                PROC
                EXPORT     SCT0_IRQHandler        [WEAK]
                LDR        R0, =SCT0_DriverIRQHandler
                BX         R0
                ENDP

MRT0_IRQHandler\
                PROC
                EXPORT     MRT0_IRQHandler        [WEAK]
                LDR        R0, =MRT0_DriverIRQHandler
                BX         R0
                ENDP

CMP_CAPT_IRQHandler\
                PROC
                EXPORT     CMP_CAPT_IRQHandler        [WEAK]
                LDR        R0, =CMP_CAPT_DriverIRQHandler
                BX         R0
                ENDP

WDT_IRQHandler\
                PROC
                EXPORT     WDT_IRQHandler        [WEAK]
                LDR        R0, =WDT_DriverIRQHandler
                BX         R0
                ENDP

BOD_IRQHandler\
                PROC
                EXPORT     BOD_IRQHandler        [WEAK]
                LDR        R0, =BOD_DriverIRQHandler
                BX         R0
                ENDP

FLASH_IRQHandler\
                PROC
                EXPORT     FLASH_IRQHandler        [WEAK]
                LDR        R0, =FLASH_DriverIRQHandler
                BX         R0
                ENDP

WKT_IRQHandler\
                PROC
                EXPORT     WKT_IRQHandler        [WEAK]
                LDR        R0, =WKT_DriverIRQHandler
                BX         R0
                ENDP

ADC0_SEQA_IRQHandler\
                PROC
                EXPORT     ADC0_SEQA_IRQHandler        [WEAK]
                LDR        R0, =ADC0_SEQA_DriverIRQHandler
                BX         R0
                ENDP

ADC0_SEQB_IRQHandler\
                PROC
                EXPORT     ADC0_SEQB_IRQHandler        [WEAK]
                LDR        R0, =ADC0_SEQB_DriverIRQHandler
                BX         R0
                ENDP

ADC0_THCMP_IRQHandler\
                PROC
                EXPORT     ADC0_THCMP_IRQHandler        [WEAK]
                LDR        R0, =ADC0_THCMP_DriverIRQHandler
                BX         R0
                ENDP

ADC0_OVR_IRQHandler\
                PROC
                EXPORT     ADC0_OVR_IRQHandler        [WEAK]
                LDR        R0, =ADC0_OVR_DriverIRQHandler
                BX         R0
                ENDP

DMA0_IRQHandler\
                PROC
                EXPORT     DMA0_IRQHandler        [WEAK]
                LDR        R0, =DMA0_DriverIRQHandler
                BX         R0
                ENDP

I2C2_IRQHandler\
                PROC
                EXPORT     I2C2_IRQHandler        [WEAK]
                LDR        R0, =I2C2_DriverIRQHandler
                BX         R0
                ENDP

I2C3_IRQHandler\
                PROC
                EXPORT     I2C3_IRQHandler        [WEAK]
                LDR        R0, =I2C3_DriverIRQHandler
                BX         R0
                ENDP

Reserved39_IRQHandler\
                PROC
                EXPORT     Reserved39_IRQHandler        [WEAK]
                LDR        R0, =Reserved39_DriverIRQHandler
                BX         R0
                ENDP

PIN_INT0_IRQHandler\
                PROC
                EXPORT     PIN_INT0_IRQHandler        [WEAK]
                LDR        R0, =PIN_INT0_DriverIRQHandler
                BX         R0
                ENDP

PIN_INT1_IRQHandler\
                PROC
                EXPORT     PIN_INT1_IRQHandler        [WEAK]
                LDR        R0, =PIN_INT1_DriverIRQHandler
                BX         R0
                ENDP

PIN_INT2_IRQHandler\
                PROC
                EXPORT     PIN_INT2_IRQHandler        [WEAK]
                LDR        R0, =PIN_INT2_DriverIRQHandler
                BX         R0
                ENDP

PIN_INT3_IRQHandler\
                PROC
                EXPORT     PIN_INT3_IRQHandler        [WEAK]
                LDR        R0, =PIN_INT3_DriverIRQHandler
                BX         R0
                ENDP

PIN_INT4_IRQHandler\
                PROC
                EXPORT     PIN_INT4_IRQHandler        [WEAK]
                LDR        R0, =PIN_INT4_DriverIRQHandler
                BX         R0
                ENDP

PIN_INT5_IRQHandler\
                PROC
                EXPORT     PIN_INT5_IRQHandler        [WEAK]
                LDR        R0, =PIN_INT5_DriverIRQHandler
                BX         R0
                ENDP

PIN_INT6_IRQHandler\
                PROC
                EXPORT     PIN_INT6_IRQHandler        [WEAK]
                LDR        R0, =PIN_INT6_DriverIRQHandler
                BX         R0
                ENDP

PIN_INT7_IRQHandler\
                PROC
                EXPORT     PIN_INT7_IRQHandler        [WEAK]
                LDR        R0, =PIN_INT7_DriverIRQHandler
                BX         R0
                ENDP

Default_Handler PROC
                EXPORT     SPI0_DriverIRQHandler        [WEAK]
                EXPORT     SPI1_DriverIRQHandler        [WEAK]
                EXPORT     Reserved18_DriverIRQHandler        [WEAK]
                EXPORT     USART0_DriverIRQHandler        [WEAK]
                EXPORT     USART1_DriverIRQHandler        [WEAK]
                EXPORT     USART2_DriverIRQHandler        [WEAK]
                EXPORT     Reserved22_DriverIRQHandler        [WEAK]
                EXPORT     I2C1_DriverIRQHandler        [WEAK]
                EXPORT     I2C0_DriverIRQHandler        [WEAK]
                EXPORT     SCT0_DriverIRQHandler        [WEAK]
                EXPORT     MRT0_DriverIRQHandler        [WEAK]
                EXPORT     CMP_CAPT_DriverIRQHandler        [WEAK]
                EXPORT     WDT_DriverIRQHandler        [WEAK]
                EXPORT     BOD_DriverIRQHandler        [WEAK]
                EXPORT     FLASH_DriverIRQHandler        [WEAK]
                EXPORT     WKT_DriverIRQHandler        [WEAK]
                EXPORT     ADC0_SEQA_DriverIRQHandler        [WEAK]
                EXPORT     ADC0_SEQB_DriverIRQHandler        [WEAK]
                EXPORT     ADC0_THCMP_DriverIRQHandler        [WEAK]
                EXPORT     ADC0_OVR_DriverIRQHandler        [WEAK]
                EXPORT     DMA0_DriverIRQHandler        [WEAK]
                EXPORT     I2C2_DriverIRQHandler        [WEAK]
                EXPORT     I2C3_DriverIRQHandler        [WEAK]
                EXPORT     Reserved39_DriverIRQHandler        [WEAK]
                EXPORT     PIN_INT0_DriverIRQHandler        [WEAK]
                EXPORT     PIN_INT1_DriverIRQHandler        [WEAK]
                EXPORT     PIN_INT2_DriverIRQHandler        [WEAK]
                EXPORT     PIN_INT3_DriverIRQHandler        [WEAK]
                EXPORT     PIN_INT4_DriverIRQHandler        [WEAK]
                EXPORT     PIN_INT5_DriverIRQHandler        [WEAK]
                EXPORT     PIN_INT6_DriverIRQHandler        [WEAK]
                EXPORT     PIN_INT7_DriverIRQHandler        [WEAK]

SPI0_DriverIRQHandler
SPI1_DriverIRQHandler
Reserved18_DriverIRQHandler
USART0_DriverIRQHandler
USART1_DriverIRQHandler
USART2_DriverIRQHandler
Reserved22_DriverIRQHandler
I2C1_DriverIRQHandler
I2C0_DriverIRQHandler
SCT0_DriverIRQHandler
MRT0_DriverIRQHandler
CMP_CAPT_DriverIRQHandler
WDT_DriverIRQHandler
BOD_DriverIRQHandler
FLASH_DriverIRQHandler
WKT_DriverIRQHandler
ADC0_SEQA_DriverIRQHandler
ADC0_SEQB_DriverIRQHandler
ADC0_THCMP_DriverIRQHandler
ADC0_OVR_DriverIRQHandler
DMA0_DriverIRQHandler
I2C2_DriverIRQHandler
I2C3_DriverIRQHandler
Reserved39_DriverIRQHandler
PIN_INT0_DriverIRQHandler
PIN_INT1_DriverIRQHandler
PIN_INT2_DriverIRQHandler
PIN_INT3_DriverIRQHandler
PIN_INT4_DriverIRQHandler
PIN_INT5_DriverIRQHandler
PIN_INT6_DriverIRQHandler
PIN_INT7_DriverIRQHandler

                B       .

                ENDP


                ALIGN


                END

