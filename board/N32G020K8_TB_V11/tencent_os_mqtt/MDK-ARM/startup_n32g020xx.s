; ****************************************************************************
; Copyright (c) 2019, Nations Technologies Inc.
;
; All rights reserved.
; ****************************************************************************
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;
; - Redistributions of source code must retain the above copyright notice,
; this list of conditions and the disclaimer below.
;
; Nations' name may not be used to endorse or promote products derived from
; this software without specific prior written permission.
;
; DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
; IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
; MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
; DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
; INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
; LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
; OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
; LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
; NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
; EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; *****************************************************************************


;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size		EQU     0x800

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size      EQU     0x200

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

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts   
                DCD    WDT_IRQHandler                         ; 0: Watchdog Timer
                DCD    SEC_IRQHandler                          ; 1: 
		  DCD    SECPAR_IRQHandler                    ; 2: 
                DCD    FLASH_IRQHandler                          ; 3: 
		  DCD    MESH_IRQHandler                        ; 4: 
		  DCD    TIM0_IRQHandler                        ; 5: 
                DCD    TIM1_IRQHandler                        ; 6: 
                DCD    TIM2_IRQHandler                        ; 7: 
		  DCD    TIM3_IRQHandler                        ; 8: 
                DCD    TIM4_IRQHandler                         ; 9: 
		  DCD    PMU_IRQHandler                          ; 10: 
		  DCD    IOM_IRQHandler                          ; 11: 
		  DCD    SAC_IRQHandler                          ; 12: 
		  DCD    SPDMA_IRQHandler                     ; 13: 
		  DCD    DMAC_IRQHandler                       ; 14: 
		  DCD    USB_IRQHandler                          ; 15: 
		  DCD    UCDR_IRQHandler                        ; 16: 
                DCD    SCD_IRQHandler                          ; 17: 
                DCD    SCC_IRQHandler                          ; 18: 
                DCD    SPIM0_IRQHandler                      ; 19: 
		  DCD    SPIM1_IRQHandler                      ; 20: 
                DCD    SPIS0_IRQHandler                       ; 21: 
                DCD    SPIS1_IRQHandler                       ; 22: 
		  DCD    UART0_IRQHandler                      ; 23: 
		  DCD    UART1_IRQHandler                      ; 24: 
		  DCD    UART2_IRQHandler                      ; 25: 
		  DCD    RTC_IRQHandler                           ; 26: 
		  DCD    ADAC_IRQHandler                        ; 27: 
                DCD    RCVCNT_IRQHandler                     ; 28: 
                DCD    I2C0_IRQHandler                         ; 29: 
                DCD    I2C1_IRQHandler                         ; 30: 
                DCD    PWM_IRQHandler                         ; 31: 

__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
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

Default_Handler PROC

                EXPORT  WDT_IRQHandler        [WEAK]
                EXPORT  SEC_IRQHandler          [WEAK]
                EXPORT  SECPAR_IRQHandler    [WEAK]
                EXPORT  FLASH_IRQHandler          [WEAK]
                EXPORT  MESH_IRQHandler       [WEAK]
                EXPORT  TIM0_IRQHandler       [WEAK]
                EXPORT  TIM1_IRQHandler       [WEAK]
                EXPORT  TIM2_IRQHandler       [WEAK]
                EXPORT  TIM3_IRQHandler       [WEAK]
                EXPORT  TIM4_IRQHandler       [WEAK]
                EXPORT  PMU_IRQHandler         [WEAK]
                EXPORT  IOM_IRQHandler         [WEAK]
                EXPORT  SAC_IRQHandler          [WEAK]
                EXPORT  SPDMA_IRQHandler    [WEAK]
                EXPORT  DMAC_IRQHandler       [WEAK]
                EXPORT  USB_IRQHandler         [WEAK]
                EXPORT  UCDR_IRQHandler       [WEAK]
                EXPORT  SCD_IRQHandler          [WEAK]
                EXPORT  SCC_IRQHandler          [WEAK]
                EXPORT  SPIM0_IRQHandler      [WEAK]
                EXPORT  SPIM1_IRQHandler      [WEAK]
                EXPORT  SPIS0_IRQHandler       [WEAK]
                EXPORT  SPIS1_IRQHandler      [WEAK]
                EXPORT  UART0_IRQHandler     [WEAK]
                EXPORT  UART1_IRQHandler     [WEAK]
                EXPORT  UART2_IRQHandler     [WEAK]
                EXPORT  RTC_IRQHandler          [WEAK]
                EXPORT  ADAC_IRQHandler       [WEAK]
                EXPORT  RCVCNT_IRQHandler   [WEAK]
                EXPORT  I2C0_IRQHandler       [WEAK]
                EXPORT  I2C1_IRQHandler       [WEAK]
                EXPORT  PWM_IRQHandler       [WEAK]

WDT_IRQHandler
SEC_IRQHandler
SECPAR_IRQHandler
FLASH_IRQHandler
MESH_IRQHandler
TIM0_IRQHandler
TIM1_IRQHandler
TIM2_IRQHandler
TIM3_IRQHandler
TIM4_IRQHandler
PMU_IRQHandler
IOM_IRQHandler
SAC_IRQHandler
SPDMA_IRQHandler
DMAC_IRQHandler
USB_IRQHandler
UCDR_IRQHandler
SCD_IRQHandler
SCC_IRQHandler
SPIM0_IRQHandler
SPIM1_IRQHandler
SPIS0_IRQHandler
SPIS1_IRQHandler
UART0_IRQHandler
UART1_IRQHandler
UART2_IRQHandler
RTC_IRQHandler
ADAC_IRQHandler
RCVCNT_IRQHandler
I2C0_IRQHandler
I2C1_IRQHandler
PWM_IRQHandler
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
