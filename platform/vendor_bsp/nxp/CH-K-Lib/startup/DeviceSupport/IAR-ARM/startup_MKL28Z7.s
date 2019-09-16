; ---------------------------------------------------------------------------------------
;  @file:    startup_MKL28Z7.s
;  @purpose: CMSIS Cortex-M0P Core Device Startup File
;            MKL28Z7
;  @version: 1.2
;  @date:    2015-3-20
;  @build:   b150320
; ---------------------------------------------------------------------------------------
;
; Copyright (c) 1997 - 2015 , Freescale Semiconductor, Inc.
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without modification,
; are permitted provided that the following conditions are met:
;
; o Redistributions of source code must retain the above copyright notice, this list
;   of conditions and the following disclaimer.
;
; o Redistributions in binary form must reproduce the above copyright notice, this
;   list of conditions and the following disclaimer in the documentation and/or
;   other materials provided with the distribution.
;
; o Neither the name of Freescale Semiconductor, Inc. nor the names of its
;   contributors may be used to endorse or promote products derived from this
;   software without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
; ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
; ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        EXTERN  SystemInit
        PUBLIC  __vector_table
        PUBLIC  __vector_table_0x1c
        PUBLIC  __Vectors
        PUBLIC  __Vectors_End
        PUBLIC  __Vectors_Size

        DATA

__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler

        DCD     NMI_Handler                                   ;NMI Handler
        DCD     HardFault_Handler                             ;Hard Fault Handler
        DCD     0                                             ;Reserved
        DCD     0                                             ;Reserved
        DCD     0                                             ;Reserved
__vector_table_0x1c
        DCD     0                                             ;Reserved
        DCD     0                                             ;Reserved
        DCD     0                                             ;Reserved
        DCD     0                                             ;Reserved
        DCD     SVC_Handler                                   ;SVCall Handler
        DCD     0                                             ;Reserved
        DCD     0                                             ;Reserved
        DCD     PendSV_Handler                                ;PendSV Handler
        DCD     SysTick_Handler                               ;SysTick Handler

                                                              ;External Interrupts
        DCD     DMA0_04_IRQHandler                            ;DMA0 channel 0/4 transfer complete
        DCD     DMA0_15_IRQHandler                            ;DMA0 channel 1/5 transfer complete
        DCD     DMA0_26_IRQHandler                            ;DMA0 channel 2/6 transfer complete
        DCD     DMA0_37_IRQHandler                            ;DMA0 channel 3/7 transfer complete
        DCD     CTI0_DMA0_Error_IRQHandler                    ;CTI0 or DMA0 error
        DCD     FLEXIO0_IRQHandler                            ;FLEXIO0
        DCD     TPM0_IRQHandler                               ;TPM0 single interrupt vector for all sources
        DCD     TPM1_IRQHandler                               ;TPM1 single interrupt vector for all sources
        DCD     TPM2_IRQHandler                               ;TPM2 single interrupt vector for all sources
        DCD     LPIT0_IRQHandler                              ;LPIT0 interrupt
        DCD     LPSPI0_IRQHandler                             ;LPSPI0 single interrupt vector for all sources
        DCD     LPSPI1_IRQHandler                             ;LPSPI1 single interrupt vector for all sources
        DCD     LPUART0_IRQHandler                            ;LPUART0 status and error
        DCD     LPUART1_IRQHandler                            ;LPUART1 status and error
        DCD     LPI2C0_IRQHandler                             ;LPI2C0 interrupt
        DCD     LPI2C1_IRQHandler                             ;LPI2C1 interrupt
        DCD     MU0_A_IRQHandler                              ;MU0 Side A interrupt
        DCD     PORTA_IRQHandler                              ;PORTA Pin detect
        DCD     PORTB_IRQHandler                              ;PORTB Pin detect
        DCD     PORTC_IRQHandler                              ;PORTC Pin detect
        DCD     PORTD_IRQHandler                              ;PORTD Pin detect
        DCD     PORTE_IRQHandler                              ;PORTE Pin detect
        DCD     LLWU0_IRQHandler                              ;Low leakage wakeup
        DCD     I2S0_IRQHandler                               ;I2S0 interrupt
        DCD     USB0_IRQHandler                               ;USB0 interrupt
        DCD     ADC0_IRQHandler                               ;ADC0 interrupt
        DCD     LPTMR0_IRQHandler                             ;LPTMR0 interrupt
        DCD     RTC_Seconds_IRQHandler                        ;RTC seconds
        DCD     INTMUX0_0_IRQHandler                          ;INTMUX0 channel 0 interrupt
        DCD     INTMUX0_1_IRQHandler                          ;INTMUX0 channel 1 interrupt
        DCD     INTMUX0_2_IRQHandler                          ;INTMUX0 channel 2 interrupt
        DCD     INTMUX0_3_IRQHandler                          ;INTMUX0 channel 3 interrupt
        DCD     LPTMR1_IRQHandler                             ;LPTMR1 interrupt
        DCD     LPIT1_IRQHandler                              ;LPIT1 interrupt
        DCD     Reserved50_IRQHandler                         ;Reserved interrupt
        DCD     Reserved51_IRQHandler                         ;Reserved interrupt
        DCD     LPSPI2_IRQHandler                             ;LPSPI2 single interrupt vector for all sources
        DCD     LPUART2_IRQHandler                            ;LPUART2 status and error
        DCD     EMVSIM0_IRQHandler                            ;EMVSIM0 interrupt
        DCD     LPI2C2_IRQHandler                             ;LPI2C2 interrupt
        DCD     TSI0_IRQHandler                               ;TSI0 interrupt
        DCD     PMC_IRQHandler                                ;PMC interrupt
        DCD     FTFA_IRQHandler                               ;FTFA interrupt
        DCD     SCG_IRQHandler                                ;SCG interrupt
        DCD     WDOG0_IRQHandler                              ;WDOG0 interrupt
        DCD     DAC0_IRQHandler                               ;DAC0 interrupt
        DCD     TRNG_IRQHandler                               ;TRNG interrupt
        DCD     RCM_IRQHandler                                ;RCM interrupt
        DCD     CMP0_IRQHandler                               ;CMP0 interrupt
        DCD     CMP1_IRQHandler                               ;CMP1 interrupt
        DCD     RTC_Alarm_IRQHandler                          ;RTC Alarm interrupt
        DCD     Reserved67_IRQHandler                         ;Reserved interrupt
        DCD     Reserved68_IRQHandler                         ;Reserved interrupt
        DCD     Reserved69_IRQHandler                         ;Reserved interrupt
        DCD     Reserved70_IRQHandler                         ;Reserved interrupt
        DCD     Reserved71_IRQHandler                         ;Reserved interrupt
        DCD     DMA1_04_IRQHandler                            ;DMA1 channel 0/4 transfer complete
        DCD     DMA1_15_IRQHandler                            ;DMA1 channel 1/5 transfer complete
        DCD     DMA1_26_IRQHandler                            ;DMA1 channel 2/6 transfer complete
        DCD     DMA1_37_IRQHandler                            ;DMA1 channel 3/7 transfer complete
        DCD     DMA1_Error_IRQHandler                         ;DMA1 error
        DCD     Reserved77_IRQHandler                         ;Reserved interrupt
        DCD     Reserved78_IRQHandler                         ;Reserved interrupt
        DCD     Reserved79_IRQHandler                         ;Reserved interrupt
__Vectors_End

        SECTION FlashConfig:CODE
__FlashConfig
      	DCD	0xFFFFFFFF
      	DCD	0xFFFFFFFF
      	DCD	0xFFFFFFFF
      	DCD	0xFFFFFFFE
__FlashConfig_End

__Vectors       EQU   __vector_table
__Vectors_Size 	EQU   __Vectors_End - __Vectors


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler
        CPSID   I               ; Mask interrupts
        LDR     R0, =SystemInit
        BLX     R0
        CPSIE   I               ; Unmask interrupts
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B .

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B .

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B .

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B .

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B .

        PUBWEAK DMA0_04_IRQHandler
        PUBWEAK DMA0_15_IRQHandler
        PUBWEAK DMA0_26_IRQHandler
        PUBWEAK DMA0_37_IRQHandler
        PUBWEAK CTI0_DMA0_Error_IRQHandler
        PUBWEAK FLEXIO0_IRQHandler
        PUBWEAK TPM0_IRQHandler
        PUBWEAK TPM1_IRQHandler
        PUBWEAK TPM2_IRQHandler
        PUBWEAK LPIT0_IRQHandler
        PUBWEAK LPSPI0_IRQHandler
        PUBWEAK LPSPI1_IRQHandler
        PUBWEAK LPUART0_IRQHandler
        PUBWEAK LPUART1_IRQHandler
        PUBWEAK LPI2C0_IRQHandler
        PUBWEAK LPI2C1_IRQHandler
        PUBWEAK MU0_A_IRQHandler
        PUBWEAK PORTA_IRQHandler
        PUBWEAK PORTB_IRQHandler
        PUBWEAK PORTC_IRQHandler
        PUBWEAK PORTD_IRQHandler
        PUBWEAK PORTE_IRQHandler
        PUBWEAK LLWU0_IRQHandler
        PUBWEAK I2S0_IRQHandler
        PUBWEAK USB0_IRQHandler
        PUBWEAK ADC0_IRQHandler
        PUBWEAK LPTMR0_IRQHandler
        PUBWEAK RTC_Seconds_IRQHandler
        PUBWEAK INTMUX0_0_IRQHandler
        PUBWEAK INTMUX0_1_IRQHandler
        PUBWEAK INTMUX0_2_IRQHandler
        PUBWEAK INTMUX0_3_IRQHandler
        PUBWEAK LPTMR1_IRQHandler
        PUBWEAK LPIT1_IRQHandler
        PUBWEAK Reserved50_IRQHandler
        PUBWEAK Reserved51_IRQHandler
        PUBWEAK LPSPI2_IRQHandler
        PUBWEAK LPUART2_IRQHandler
        PUBWEAK EMVSIM0_IRQHandler
        PUBWEAK LPI2C2_IRQHandler
        PUBWEAK TSI0_IRQHandler
        PUBWEAK PMC_IRQHandler
        PUBWEAK FTFA_IRQHandler
        PUBWEAK SCG_IRQHandler
        PUBWEAK WDOG0_IRQHandler
        PUBWEAK DAC0_IRQHandler
        PUBWEAK TRNG_IRQHandler
        PUBWEAK RCM_IRQHandler
        PUBWEAK CMP0_IRQHandler
        PUBWEAK CMP1_IRQHandler
        PUBWEAK RTC_Alarm_IRQHandler
        PUBWEAK Reserved67_IRQHandler
        PUBWEAK Reserved68_IRQHandler
        PUBWEAK Reserved69_IRQHandler
        PUBWEAK Reserved70_IRQHandler
        PUBWEAK Reserved71_IRQHandler
        PUBWEAK DMA1_04_IRQHandler
        PUBWEAK DMA1_15_IRQHandler
        PUBWEAK DMA1_26_IRQHandler
        PUBWEAK DMA1_37_IRQHandler
        PUBWEAK DMA1_Error_IRQHandler
        PUBWEAK Reserved77_IRQHandler
        PUBWEAK Reserved78_IRQHandler
        PUBWEAK Reserved79_IRQHandler
        PUBWEAK DefaultISR
        SECTION .text:CODE:REORDER:NOROOT(2)
DMA0_04_IRQHandler
DMA0_15_IRQHandler
DMA0_26_IRQHandler
DMA0_37_IRQHandler
CTI0_DMA0_Error_IRQHandler
FLEXIO0_IRQHandler
TPM0_IRQHandler
TPM1_IRQHandler
TPM2_IRQHandler
LPIT0_IRQHandler
LPSPI0_IRQHandler
LPSPI1_IRQHandler
LPUART0_IRQHandler
LPUART1_IRQHandler
LPI2C0_IRQHandler
LPI2C1_IRQHandler
MU0_A_IRQHandler
PORTA_IRQHandler
PORTB_IRQHandler
PORTC_IRQHandler
PORTD_IRQHandler
PORTE_IRQHandler
LLWU0_IRQHandler
I2S0_IRQHandler
USB0_IRQHandler
ADC0_IRQHandler
LPTMR0_IRQHandler
RTC_Seconds_IRQHandler
INTMUX0_0_IRQHandler
INTMUX0_1_IRQHandler
INTMUX0_2_IRQHandler
INTMUX0_3_IRQHandler
LPTMR1_IRQHandler
LPIT1_IRQHandler
Reserved50_IRQHandler
Reserved51_IRQHandler
LPSPI2_IRQHandler
LPUART2_IRQHandler
EMVSIM0_IRQHandler
LPI2C2_IRQHandler
TSI0_IRQHandler
PMC_IRQHandler
FTFA_IRQHandler
SCG_IRQHandler
WDOG0_IRQHandler
DAC0_IRQHandler
TRNG_IRQHandler
RCM_IRQHandler
CMP0_IRQHandler
CMP1_IRQHandler
RTC_Alarm_IRQHandler
Reserved67_IRQHandler
Reserved68_IRQHandler
Reserved69_IRQHandler
Reserved70_IRQHandler
Reserved71_IRQHandler
DMA1_04_IRQHandler
DMA1_15_IRQHandler
DMA1_26_IRQHandler
DMA1_37_IRQHandler
DMA1_Error_IRQHandler
Reserved77_IRQHandler
Reserved78_IRQHandler
Reserved79_IRQHandler
DefaultISR
        LDR R0, =DefaultISR
        BX R0

        END
