;/**************************************************************************//**
; * @file     startup_ARMCM4.s
; * @brief    CMSIS Core Device Startup File for
; *           ARMCM4 Device Series
; * @version  V5.00
; * @date     02. March 2016
; ******************************************************************************/
;/*
; * Copyright (c) 2009-2016 ARM Limited. All rights reserved.
; *
; * SPDX-License-Identifier: Apache-2.0
; *
; * Licensed under the Apache License, Version 2.0 (the License); you may
; * not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; * www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an AS IS BASIS, WITHOUT
; * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; */

;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00001800

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp               ;(0x00)Top of Stack
                DCD     Reset_Handler              ;(0x04)IRQ -15  Reset Handler
                DCD     NMI_Handler                ;(0x08)IRQ -14  NMI Handler
                DCD     HardFault_Handler          ;(0x0C)IRQ -13  Hard Fault Handler
                DCD     MemManage_Handler          ;(0x10)IRQ -12  MPU Fault Handler
                DCD     BusFault_Handler           ;(0x14)IRQ -11  Bus Fault Handler
                DCD     UsageFault_Handler         ;(0x18)IRQ -10  Usage Fault Handler
                DCD     0                          ;(0x1C)IRQ -9   Reserved
                DCD     0                          ;(0x20)IRQ -8   Reserved
                DCD     0                          ;(0x24)IRQ -7   Reserved
                DCD     0                          ;(0x28)IRQ -6   Reserved
                DCD     SVC_Handler                ;(0x2C)IRQ -5   SVCall Handler
                DCD     DebugMon_Handler           ;(0x30)IRQ -4   Debug Monitor Handler
                DCD     0                          ;(0x34)IRQ -3   Reserved
                DCD     PendSV_Handler             ;(0x38)IRQ -2   PendSV Handler
                DCD     SysTick_Handler            ;(0x3C)IRQ -1   SysTick Handler

                ; External Interrupts
                DCD     WDT_IRQHandler             ;(0x40)IRQ0
                DCD     EXTERNAL_IRQHandler        ;(0x44)IRQ1
                DCD     RTC_IRQHandler             ;(0x48)IRQ2
                DCD	    SLEEP_IRQHandler           ;(0x4C)IRQ3
                DCD     MAC_IRQHandler             ;(0x50)IRQ4
                DCD     DMA_IRQHandler             ;(0x54)IRQ5
                DCD     QSPI_IRQHandler            ;(0x58)IRQ6
                DCD     SDIO_FUN1_IRQHandler       ;(0x5C)IRQ7
                DCD     SDIO_FUN2_IRQHandler       ;(0x60)IRQ8
                DCD     SDIO_FUN3_IRQHandler       ;(0x64)IRQ9
                DCD     SDIO_FUN4_IRQHandler       ;(0x68)IRQ10
                DCD     SDIO_FUN5_IRQHandler       ;(0x6C)IRQ11
                DCD     SDIO_FUN6_IRQHandler       ;(0x70)IRQ12
                DCD     SDIO_FUN7_IRQHandler       ;(0x74)IRQ13
                DCD     SDIO_ASYNC_HOST_IRQHandler ;(0x78)IRQ14
                DCD     SDIO_M2S_IRQHandler        ;(0x7C)IRQ15
                DCD     CM4_INTR0_IRQHandler       ;(0x80)IRQ16
                DCD     CM4_INTR1_IRQHandler       ;(0x84)IRQ17
                DCD     CM4_INTR2_IRQHandler       ;(0x88)IRQ18
                DCD     CM4_INTR3_IRQHandler       ;(0x8C)IRQ19
                DCD     CM4_INTR4_IRQHandler       ;(0x90)IRQ20
                DCD     CM4_INTR5_IRQHandler       ;(0x94)IRQ21
                DCD     ADC_IRQHandler             ;(0x98)IRQ22
                DCD     TIMER_IRQHandler           ;(0x9C)IRQ23
                DCD     I2C0_IRQHandler            ;(0xA0)IRQ24
                DCD     I2C1_IRQHandler            ;(0xA4)IRQ25
                DCD     SPI0_IRQHandler          ;(0xA8)IRQ26
                DCD     SPI2_IRQHandler          ;(0xAC)IRQ27
                DCD     UART0_IRQHandler           ;(0xB0)IRQ28
                DCD     UART1_IRQHandler           ;(0xB4)IRQ29
                DCD     SPI1_IRQHandler          ;(0xB8)IRQ30
                DCD     GPIO_IRQHandler            ;(0xBC)IRQ31
                DCD     I2S_IRQHandler             ;(0xC0)IRQ32
                DCD     PAOTD_IRQHandler           ;(0xC4)IRQ33
                DCD     PWM_IRQHandler             ;(0xC8)IRQ34
                DCD     TRNG_IRQHandler            ;(0xCC)IRQ35
                DCD     AES_IRQHandler             ;(0xD0)IRQ36
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R4, =SystemInit
                BLX     R4
                LDR     R4, =__main
                BX      R4
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler        [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
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

Default_Handler PROC

                EXPORT  WDT_IRQHandler             [WEAK]
                EXPORT  EXTERNAL_IRQHandler        [WEAK]
                EXPORT  RTC_IRQHandler             [WEAK]
                EXPORT  SLEEP_IRQHandler           [WEAK]
                EXPORT  MAC_IRQHandler             [WEAK]
                EXPORT  DMA_IRQHandler             [WEAK]
                EXPORT  QSPI_IRQHandler            [WEAK]
                EXPORT  SDIO_FUN1_IRQHandler       [WEAK]
                EXPORT  SDIO_FUN2_IRQHandler       [WEAK]
                EXPORT  SDIO_FUN3_IRQHandler       [WEAK]
                EXPORT  SDIO_FUN4_IRQHandler       [WEAK]
                EXPORT  SDIO_FUN5_IRQHandler       [WEAK]
                EXPORT  SDIO_FUN6_IRQHandler       [WEAK]
                EXPORT  SDIO_FUN7_IRQHandler       [WEAK]
                EXPORT  SDIO_ASYNC_HOST_IRQHandler [WEAK]
                EXPORT  SDIO_M2S_IRQHandler        [WEAK]
                EXPORT  CM4_INTR0_IRQHandler       [WEAK]
                EXPORT  CM4_INTR1_IRQHandler       [WEAK]
                EXPORT  CM4_INTR2_IRQHandler       [WEAK]
                EXPORT  CM4_INTR3_IRQHandler       [WEAK]
                EXPORT  CM4_INTR4_IRQHandler       [WEAK]
                EXPORT  CM4_INTR5_IRQHandler       [WEAK]
                EXPORT  ADC_IRQHandler             [WEAK]
                EXPORT  TIMER_IRQHandler           [WEAK]
                EXPORT  I2C0_IRQHandler            [WEAK]
                EXPORT  I2C1_IRQHandler            [WEAK]
                EXPORT  SPI0_IRQHandler            [WEAK]
                EXPORT  SPI2_IRQHandler            [WEAK]
                EXPORT  UART0_IRQHandler           [WEAK]
                EXPORT  UART1_IRQHandler           [WEAK]
                EXPORT  SPI1_IRQHandler            [WEAK]
                EXPORT  GPIO_IRQHandler            [WEAK]
                EXPORT  I2S_IRQHandler             [WEAK]
                EXPORT  PAOTD_IRQHandler           [WEAK]
                EXPORT  PWM_IRQHandler             [WEAK]
                EXPORT  TRNG_IRQHandler            [WEAK]
                EXPORT  AES_IRQHandler             [WEAK]
WDT_IRQHandler
EXTERNAL_IRQHandler
RTC_IRQHandler
SLEEP_IRQHandler
MAC_IRQHandler
DMA_IRQHandler
QSPI_IRQHandler
SDIO_FUN1_IRQHandler
SDIO_FUN2_IRQHandler
SDIO_FUN3_IRQHandler
SDIO_FUN4_IRQHandler
SDIO_FUN5_IRQHandler
SDIO_FUN6_IRQHandler
SDIO_FUN7_IRQHandler
SDIO_ASYNC_HOST_IRQHandler
SDIO_M2S_IRQHandler
CM4_INTR0_IRQHandler
CM4_INTR1_IRQHandler
CM4_INTR2_IRQHandler
CM4_INTR3_IRQHandler
CM4_INTR4_IRQHandler
CM4_INTR5_IRQHandler
ADC_IRQHandler
TIMER_IRQHandler
I2C0_IRQHandler
I2C1_IRQHandler
SPI0_IRQHandler
SPI2_IRQHandler
UART0_IRQHandler
UART1_IRQHandler
SPI1_IRQHandler
GPIO_IRQHandler
I2S_IRQHandler
PAOTD_IRQHandler
PWM_IRQHandler
TRNG_IRQHandler
AES_IRQHandler
                B       .
                ENDP
                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF


                END
