; -------------------------------------------------------------------------
;  @file:    startup_MIMXRT1011.s
;  @purpose: CMSIS Cortex-M7 Core Device Startup File
;            MIMXRT1011
;  @version: 1.0
;  @date:    2019-8-1
;  @build:   b190801
; -------------------------------------------------------------------------
;
; Copyright 1997-2016 Freescale Semiconductor, Inc.
; Copyright 2016-2019 NXP
; All rights reserved.
;
; SPDX-License-Identifier: BSD-3-Clause
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
        DCD     MemManage_Handler                             ;MPU Fault Handler
        DCD     BusFault_Handler                              ;Bus Fault Handler
        DCD     UsageFault_Handler                            ;Usage Fault Handler
__vector_table_0x1c
        DCD     0                                             ;Reserved
        DCD     0                                             ;Reserved
        DCD     0                                             ;Reserved
        DCD     0                                             ;Reserved
        DCD     SVC_Handler                                   ;SVCall Handler
        DCD     DebugMon_Handler                              ;Debug Monitor Handler
        DCD     0                                             ;Reserved
        DCD     PendSV_Handler                                ;PendSV Handler
        DCD     SysTick_Handler                               ;SysTick Handler

                                                              ;External Interrupts
        DCD     DMA0_IRQHandler                               ;DMA channel 0 transfer complete
        DCD     DMA1_IRQHandler                               ;DMA channel 1 transfer complete
        DCD     DMA2_IRQHandler                               ;DMA channel 2 transfer complete
        DCD     DMA3_IRQHandler                               ;DMA channel 3 transfer complete
        DCD     DMA4_IRQHandler                               ;DMA channel 4 transfer complete
        DCD     DMA5_IRQHandler                               ;DMA channel 5 transfer complete
        DCD     DMA6_IRQHandler                               ;DMA channel 6 transfer complete
        DCD     DMA7_IRQHandler                               ;DMA channel 7 transfer complete
        DCD     DMA8_IRQHandler                               ;DMA channel 8 transfer complete
        DCD     DMA9_IRQHandler                               ;DMA channel 9 transfer complete
        DCD     DMA10_IRQHandler                              ;DMA channel 10 transfer complete
        DCD     DMA11_IRQHandler                              ;DMA channel 11 transfer complete
        DCD     DMA12_IRQHandler                              ;DMA channel 12 transfer complete
        DCD     DMA13_IRQHandler                              ;DMA channel 13 transfer complete
        DCD     DMA14_IRQHandler                              ;DMA channel 14 transfer complete
        DCD     DMA15_IRQHandler                              ;DMA channel 15 transfer complete
        DCD     DMA_ERROR_IRQHandler                          ;DMA error interrupt channels 0-15
        DCD     CTI0_ERROR_IRQHandler                         ;CTI trigger outputs
        DCD     CTI1_ERROR_IRQHandler                         ;CTI trigger outputs
        DCD     CORE_IRQHandler                               ;CorePlatform exception IRQ
        DCD     LPUART1_IRQHandler                            ;LPUART1 TX interrupt and RX interrupt
        DCD     LPUART2_IRQHandler                            ;LPUART2 TX interrupt and RX interrupt
        DCD     LPUART3_IRQHandler                            ;LPUART3 TX interrupt and RX interrupt
        DCD     LPUART4_IRQHandler                            ;LPUART4 TX interrupt and RX interrupt
        DCD     PIT_IRQHandler                                ;PIT interrupt
        DCD     USB_OTG1_IRQHandler                           ;USBO2 USB OTG1
        DCD     FLEXSPI_IRQHandler                            ;FlexSPI0 interrupt
        DCD     FLEXRAM_IRQHandler                            ;FlexRAM address out of range Or access hit IRQ
        DCD     LPI2C1_IRQHandler                             ;LPI2C1 interrupt
        DCD     LPI2C2_IRQHandler                             ;LPI2C2 interrupt
        DCD     GPT1_IRQHandler                               ;GPT1 interrupt
        DCD     GPT2_IRQHandler                               ;GPT2 interrupt
        DCD     LPSPI1_IRQHandler                             ;LPSPI1 single interrupt vector for all sources
        DCD     LPSPI2_IRQHandler                             ;LPSPI2 single interrupt vector for all sources
        DCD     PWM1_0_IRQHandler                             ;PWM1 capture 0, compare 0, or reload 0 interrupt
        DCD     PWM1_1_IRQHandler                             ;PWM1 capture 1, compare 1, or reload 0 interrupt
        DCD     PWM1_2_IRQHandler                             ;PWM1 capture 2, compare 2, or reload 0 interrupt
        DCD     PWM1_3_IRQHandler                             ;PWM1 capture 3, compare 3, or reload 0 interrupt
        DCD     PWM1_FAULT_IRQHandler                         ;PWM1 fault or reload error interrupt
        DCD     KPP_IRQHandler                                ;Keypad nterrupt
        DCD     SRC_IRQHandler                                ;SRC interrupt
        DCD     GPR_IRQ_IRQHandler                            ;Used to notify cores on exception condition while boot
        DCD     CCM_1_IRQHandler                              ;CCM IRQ1 interrupt
        DCD     CCM_2_IRQHandler                              ;CCM IRQ2 interrupt
        DCD     EWM_IRQHandler                                ;EWM interrupt
        DCD     WDOG2_IRQHandler                              ;WDOG2 interrupt
        DCD     SNVS_HP_WRAPPER_IRQHandler                    ;SNVS Functional Interrupt
        DCD     SNVS_HP_WRAPPER_TZ_IRQHandler                 ;SNVS Security Interrupt
        DCD     SNVS_LP_WRAPPER_IRQHandler                    ;ON-OFF button press shorter than 5 secs (pulse event)
        DCD     CSU_IRQHandler                                ;CSU interrupt
        DCD     DCP_IRQHandler                                ;Combined DCP channel interrupts(except channel 0) and CRC interrupt
        DCD     DCP_VMI_IRQHandler                            ;IRQ of DCP channel 0
        DCD     Reserved68_IRQHandler                         ;Reserved interrupt
        DCD     TRNG_IRQHandler                               ;TRNG interrupt
        DCD     Reserved70_IRQHandler                         ;Reserved interrupt
        DCD     Reserved71_IRQHandler                         ;Reserved interrupt
        DCD     SAI1_IRQHandler                               ;SAI1 interrupt
        DCD     RTWDOG_IRQHandler                             ;RTWDOG interrupt
        DCD     SAI3_RX_IRQHandler                            ;SAI3 interrupt
        DCD     SAI3_TX_IRQHandler                            ;SAI3 interrupt
        DCD     SPDIF_IRQHandler                              ;SPDIF interrupt
        DCD     PMU_IRQHandler                                ;PMU interrupt
        DCD     XBAR1_IRQ_0_1_2_3_IRQHandler                  ;XBAR1 interrupt
        DCD     TEMP_LOW_HIGH_IRQHandler                      ;TEMPMON interrupt
        DCD     TEMP_PANIC_IRQHandler                         ;TEMPMON interrupt
        DCD     USB_PHY_IRQHandler                            ;USBPHY (OTG1 UTMI), Interrupt
        DCD     GPC_IRQHandler                                ;GPC interrupt
        DCD     ADC1_IRQHandler                               ;ADC1 interrupt
        DCD     FLEXIO1_IRQHandler                            ;FLEXIO1 interrupt
        DCD     DCDC_IRQHandler                               ;DCDC interrupt
        DCD     GPIO1_Combined_0_15_IRQHandler                ;Combined interrupt indication for GPIO1 signal 0 throughout 15
        DCD     GPIO1_Combined_16_31_IRQHandler               ;Combined interrupt indication for GPIO1 signal 16 throughout 31
        DCD     GPIO2_Combined_0_15_IRQHandler                ;Combined interrupt indication for GPIO2 signal 0 throughout 15
        DCD     GPIO5_Combined_0_15_IRQHandler                ;Combined interrupt indication for GPIO5 signal 0 throughout 15
        DCD     WDOG1_IRQHandler                              ;WDOG1 interrupt
        DCD     ADC_ETC_IRQ0_IRQHandler                       ;ADCETC IRQ0 interrupt
        DCD     ADC_ETC_IRQ1_IRQHandler                       ;ADCETC IRQ1 interrupt
        DCD     ADC_ETC_IRQ2_IRQHandler                       ;ADCETC IRQ2 interrupt
        DCD     ADC_ETC_IRQ3_IRQHandler                       ;ADCETC IRQ3 interrupt
        DCD     ADC_ETC_ERROR_IRQ_IRQHandler                  ;ADCETC Error IRQ interrupt
        DCD     DefaultISR                                    ;96
        DCD     DefaultISR                                    ;97
        DCD     DefaultISR                                    ;98
        DCD     DefaultISR                                    ;99
        DCD     DefaultISR                                    ;100
        DCD     DefaultISR                                    ;101
        DCD     DefaultISR                                    ;102
        DCD     DefaultISR                                    ;103
        DCD     DefaultISR                                    ;104
        DCD     DefaultISR                                    ;105
        DCD     DefaultISR                                    ;106
        DCD     DefaultISR                                    ;107
        DCD     DefaultISR                                    ;108
        DCD     DefaultISR                                    ;109
        DCD     DefaultISR                                    ;110
        DCD     DefaultISR                                    ;111
        DCD     DefaultISR                                    ;112
        DCD     DefaultISR                                    ;113
        DCD     DefaultISR                                    ;114
        DCD     DefaultISR                                    ;115
        DCD     DefaultISR                                    ;116
        DCD     DefaultISR                                    ;117
        DCD     DefaultISR                                    ;118
        DCD     DefaultISR                                    ;119
        DCD     DefaultISR                                    ;120
        DCD     DefaultISR                                    ;121
        DCD     DefaultISR                                    ;122
        DCD     DefaultISR                                    ;123
        DCD     DefaultISR                                    ;124
        DCD     DefaultISR                                    ;125
        DCD     DefaultISR                                    ;126
        DCD     DefaultISR                                    ;127
        DCD     DefaultISR                                    ;128
        DCD     DefaultISR                                    ;129
        DCD     DefaultISR                                    ;130
        DCD     DefaultISR                                    ;131
        DCD     DefaultISR                                    ;132
        DCD     DefaultISR                                    ;133
        DCD     DefaultISR                                    ;134
        DCD     DefaultISR                                    ;135
        DCD     DefaultISR                                    ;136
        DCD     DefaultISR                                    ;137
        DCD     DefaultISR                                    ;138
        DCD     DefaultISR                                    ;139
        DCD     DefaultISR                                    ;140
        DCD     DefaultISR                                    ;141
        DCD     DefaultISR                                    ;142
        DCD     DefaultISR                                    ;143
        DCD     DefaultISR                                    ;144
        DCD     DefaultISR                                    ;145
        DCD     DefaultISR                                    ;146
        DCD     DefaultISR                                    ;147
        DCD     DefaultISR                                    ;148
        DCD     DefaultISR                                    ;149
        DCD     DefaultISR                                    ;150
        DCD     DefaultISR                                    ;151
        DCD     DefaultISR                                    ;152
        DCD     DefaultISR                                    ;153
        DCD     DefaultISR                                    ;154
        DCD     DefaultISR                                    ;155
        DCD     DefaultISR                                    ;156
        DCD     DefaultISR                                    ;157
        DCD     DefaultISR                                    ;158
        DCD     DefaultISR                                    ;159
        DCD     DefaultISR                                    ;160
        DCD     DefaultISR                                    ;161
        DCD     DefaultISR                                    ;162
        DCD     DefaultISR                                    ;163
        DCD     DefaultISR                                    ;164
        DCD     DefaultISR                                    ;165
        DCD     DefaultISR                                    ;166
        DCD     DefaultISR                                    ;167
        DCD     DefaultISR                                    ;168
        DCD     DefaultISR                                    ;169
        DCD     DefaultISR                                    ;170
        DCD     DefaultISR                                    ;171
        DCD     DefaultISR                                    ;172
        DCD     DefaultISR                                    ;173
        DCD     DefaultISR                                    ;174
        DCD     DefaultISR                                    ;175
        DCD     DefaultISR                                    ;176
        DCD     DefaultISR                                    ;177
        DCD     DefaultISR                                    ;178
        DCD     DefaultISR                                    ;179
        DCD     DefaultISR                                    ;180
        DCD     DefaultISR                                    ;181
        DCD     DefaultISR                                    ;182
        DCD     DefaultISR                                    ;183
        DCD     DefaultISR                                    ;184
        DCD     DefaultISR                                    ;185
        DCD     DefaultISR                                    ;186
        DCD     DefaultISR                                    ;187
        DCD     DefaultISR                                    ;188
        DCD     DefaultISR                                    ;189
        DCD     DefaultISR                                    ;190
        DCD     DefaultISR                                    ;191
        DCD     DefaultISR                                    ;192
        DCD     DefaultISR                                    ;193
        DCD     DefaultISR                                    ;194
        DCD     DefaultISR                                    ;195
        DCD     DefaultISR                                    ;196
        DCD     DefaultISR                                    ;197
        DCD     DefaultISR                                    ;198
        DCD     DefaultISR                                    ;199
        DCD     DefaultISR                                    ;200
        DCD     DefaultISR                                    ;201
        DCD     DefaultISR                                    ;202
        DCD     DefaultISR                                    ;203
        DCD     DefaultISR                                    ;204
        DCD     DefaultISR                                    ;205
        DCD     DefaultISR                                    ;206
        DCD     DefaultISR                                    ;207
        DCD     DefaultISR                                    ;208
        DCD     DefaultISR                                    ;209
        DCD     DefaultISR                                    ;210
        DCD     DefaultISR                                    ;211
        DCD     DefaultISR                                    ;212
        DCD     DefaultISR                                    ;213
        DCD     DefaultISR                                    ;214
        DCD     DefaultISR                                    ;215
        DCD     DefaultISR                                    ;216
        DCD     DefaultISR                                    ;217
        DCD     DefaultISR                                    ;218
        DCD     DefaultISR                                    ;219
        DCD     DefaultISR                                    ;220
        DCD     DefaultISR                                    ;221
        DCD     DefaultISR                                    ;222
        DCD     DefaultISR                                    ;223
        DCD     DefaultISR                                    ;224
        DCD     DefaultISR                                    ;225
        DCD     DefaultISR                                    ;226
        DCD     DefaultISR                                    ;227
        DCD     DefaultISR                                    ;228
        DCD     DefaultISR                                    ;229
        DCD     DefaultISR                                    ;230
        DCD     DefaultISR                                    ;231
        DCD     DefaultISR                                    ;232
        DCD     DefaultISR                                    ;233
        DCD     DefaultISR                                    ;234
        DCD     DefaultISR                                    ;235
        DCD     DefaultISR                                    ;236
        DCD     DefaultISR                                    ;237
        DCD     DefaultISR                                    ;238
        DCD     DefaultISR                                    ;239
        DCD     DefaultISR                                    ;240
        DCD     DefaultISR                                    ;241
        DCD     DefaultISR                                    ;242
        DCD     DefaultISR                                    ;243
        DCD     DefaultISR                                    ;244
        DCD     DefaultISR                                    ;245
        DCD     DefaultISR                                    ;246
        DCD     DefaultISR                                    ;247
        DCD     DefaultISR                                    ;248
        DCD     DefaultISR                                    ;249
        DCD     DefaultISR                                    ;250
        DCD     DefaultISR                                    ;251
        DCD     DefaultISR                                    ;252
        DCD     DefaultISR                                    ;253
        DCD     DefaultISR                                    ;254
        DCD     0xFFFFFFFF                                    ; Reserved for user TRIM value
__Vectors_End

__Vectors       EQU   __vector_table
__Vectors_Size  EQU   __Vectors_End - __Vectors


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler
        CPSID   I               ; Mask interrupts
        LDR     R0, =0xE000ED08
        LDR     R1, =__vector_table
        STR     R1, [R0]
        LDR     R2, [R1]
        MSR     MSP, R2
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

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
MemManage_Handler
        B .

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BusFault_Handler
        B .

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UsageFault_Handler
        B .

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B .

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
DebugMon_Handler
        B .

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B .

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B .

        PUBWEAK DMA0_IRQHandler
        PUBWEAK DMA0_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
DMA0_IRQHandler
        LDR     R0, =DMA0_DriverIRQHandler
        BX      R0

        PUBWEAK DMA1_IRQHandler
        PUBWEAK DMA1_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
DMA1_IRQHandler
        LDR     R0, =DMA1_DriverIRQHandler
        BX      R0

        PUBWEAK DMA2_IRQHandler
        PUBWEAK DMA2_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
DMA2_IRQHandler
        LDR     R0, =DMA2_DriverIRQHandler
        BX      R0

        PUBWEAK DMA3_IRQHandler
        PUBWEAK DMA3_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
DMA3_IRQHandler
        LDR     R0, =DMA3_DriverIRQHandler
        BX      R0

        PUBWEAK DMA4_IRQHandler
        PUBWEAK DMA4_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
DMA4_IRQHandler
        LDR     R0, =DMA4_DriverIRQHandler
        BX      R0

        PUBWEAK DMA5_IRQHandler
        PUBWEAK DMA5_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
DMA5_IRQHandler
        LDR     R0, =DMA5_DriverIRQHandler
        BX      R0

        PUBWEAK DMA6_IRQHandler
        PUBWEAK DMA6_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
DMA6_IRQHandler
        LDR     R0, =DMA6_DriverIRQHandler
        BX      R0

        PUBWEAK DMA7_IRQHandler
        PUBWEAK DMA7_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
DMA7_IRQHandler
        LDR     R0, =DMA7_DriverIRQHandler
        BX      R0

        PUBWEAK DMA8_IRQHandler
        PUBWEAK DMA8_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
DMA8_IRQHandler
        LDR     R0, =DMA8_DriverIRQHandler
        BX      R0

        PUBWEAK DMA9_IRQHandler
        PUBWEAK DMA9_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
DMA9_IRQHandler
        LDR     R0, =DMA9_DriverIRQHandler
        BX      R0

        PUBWEAK DMA10_IRQHandler
        PUBWEAK DMA10_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
DMA10_IRQHandler
        LDR     R0, =DMA10_DriverIRQHandler
        BX      R0

        PUBWEAK DMA11_IRQHandler
        PUBWEAK DMA11_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
DMA11_IRQHandler
        LDR     R0, =DMA11_DriverIRQHandler
        BX      R0

        PUBWEAK DMA12_IRQHandler
        PUBWEAK DMA12_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
DMA12_IRQHandler
        LDR     R0, =DMA12_DriverIRQHandler
        BX      R0

        PUBWEAK DMA13_IRQHandler
        PUBWEAK DMA13_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
DMA13_IRQHandler
        LDR     R0, =DMA13_DriverIRQHandler
        BX      R0

        PUBWEAK DMA14_IRQHandler
        PUBWEAK DMA14_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
DMA14_IRQHandler
        LDR     R0, =DMA14_DriverIRQHandler
        BX      R0

        PUBWEAK DMA15_IRQHandler
        PUBWEAK DMA15_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
DMA15_IRQHandler
        LDR     R0, =DMA15_DriverIRQHandler
        BX      R0

        PUBWEAK DMA_ERROR_IRQHandler
        PUBWEAK DMA_ERROR_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
DMA_ERROR_IRQHandler
        LDR     R0, =DMA_ERROR_DriverIRQHandler
        BX      R0

        PUBWEAK CTI0_ERROR_IRQHandler
        PUBWEAK CTI1_ERROR_IRQHandler
        PUBWEAK CORE_IRQHandler
        PUBWEAK LPUART1_IRQHandler
        PUBWEAK LPUART1_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
LPUART1_IRQHandler
        LDR     R0, =LPUART1_DriverIRQHandler
        BX      R0

        PUBWEAK LPUART2_IRQHandler
        PUBWEAK LPUART2_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
LPUART2_IRQHandler
        LDR     R0, =LPUART2_DriverIRQHandler
        BX      R0

        PUBWEAK LPUART3_IRQHandler
        PUBWEAK LPUART3_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
LPUART3_IRQHandler
        LDR     R0, =LPUART3_DriverIRQHandler
        BX      R0

        PUBWEAK LPUART4_IRQHandler
        PUBWEAK LPUART4_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
LPUART4_IRQHandler
        LDR     R0, =LPUART4_DriverIRQHandler
        BX      R0

        PUBWEAK PIT_IRQHandler
        PUBWEAK USB_OTG1_IRQHandler
        PUBWEAK FLEXSPI_IRQHandler
        PUBWEAK FLEXSPI_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
FLEXSPI_IRQHandler
        LDR     R0, =FLEXSPI_DriverIRQHandler
        BX      R0

        PUBWEAK FLEXRAM_IRQHandler
        PUBWEAK LPI2C1_IRQHandler
        PUBWEAK LPI2C1_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
LPI2C1_IRQHandler
        LDR     R0, =LPI2C1_DriverIRQHandler
        BX      R0

        PUBWEAK LPI2C2_IRQHandler
        PUBWEAK LPI2C2_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
LPI2C2_IRQHandler
        LDR     R0, =LPI2C2_DriverIRQHandler
        BX      R0

        PUBWEAK GPT1_IRQHandler
        PUBWEAK GPT2_IRQHandler
        PUBWEAK LPSPI1_IRQHandler
        PUBWEAK LPSPI1_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
LPSPI1_IRQHandler
        LDR     R0, =LPSPI1_DriverIRQHandler
        BX      R0

        PUBWEAK LPSPI2_IRQHandler
        PUBWEAK LPSPI2_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
LPSPI2_IRQHandler
        LDR     R0, =LPSPI2_DriverIRQHandler
        BX      R0

        PUBWEAK PWM1_0_IRQHandler
        PUBWEAK PWM1_1_IRQHandler
        PUBWEAK PWM1_2_IRQHandler
        PUBWEAK PWM1_3_IRQHandler
        PUBWEAK PWM1_FAULT_IRQHandler
        PUBWEAK KPP_IRQHandler
        PUBWEAK SRC_IRQHandler
        PUBWEAK GPR_IRQ_IRQHandler
        PUBWEAK CCM_1_IRQHandler
        PUBWEAK CCM_2_IRQHandler
        PUBWEAK EWM_IRQHandler
        PUBWEAK WDOG2_IRQHandler
        PUBWEAK SNVS_HP_WRAPPER_IRQHandler
        PUBWEAK SNVS_HP_WRAPPER_TZ_IRQHandler
        PUBWEAK SNVS_LP_WRAPPER_IRQHandler
        PUBWEAK CSU_IRQHandler
        PUBWEAK DCP_IRQHandler
        PUBWEAK DCP_VMI_IRQHandler
        PUBWEAK Reserved68_IRQHandler
        PUBWEAK TRNG_IRQHandler
        PUBWEAK Reserved70_IRQHandler
        PUBWEAK Reserved71_IRQHandler
        PUBWEAK SAI1_IRQHandler
        PUBWEAK SAI1_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
SAI1_IRQHandler
        LDR     R0, =SAI1_DriverIRQHandler
        BX      R0

        PUBWEAK RTWDOG_IRQHandler
        PUBWEAK SAI3_RX_IRQHandler
        PUBWEAK SAI3_RX_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
SAI3_RX_IRQHandler
        LDR     R0, =SAI3_RX_DriverIRQHandler
        BX      R0

        PUBWEAK SAI3_TX_IRQHandler
        PUBWEAK SAI3_TX_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
SAI3_TX_IRQHandler
        LDR     R0, =SAI3_TX_DriverIRQHandler
        BX      R0

        PUBWEAK SPDIF_IRQHandler
        PUBWEAK SPDIF_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
SPDIF_IRQHandler
        LDR     R0, =SPDIF_DriverIRQHandler
        BX      R0

        PUBWEAK PMU_IRQHandler
        PUBWEAK XBAR1_IRQ_0_1_2_3_IRQHandler
        PUBWEAK TEMP_LOW_HIGH_IRQHandler
        PUBWEAK TEMP_PANIC_IRQHandler
        PUBWEAK USB_PHY_IRQHandler
        PUBWEAK GPC_IRQHandler
        PUBWEAK ADC1_IRQHandler
        PUBWEAK FLEXIO1_IRQHandler
        PUBWEAK FLEXIO1_DriverIRQHandler
        SECTION .text:CODE:REORDER:NOROOT(2)
FLEXIO1_IRQHandler
        LDR     R0, =FLEXIO1_DriverIRQHandler
        BX      R0

        PUBWEAK DCDC_IRQHandler
        PUBWEAK GPIO1_Combined_0_15_IRQHandler
        PUBWEAK GPIO1_Combined_16_31_IRQHandler
        PUBWEAK GPIO2_Combined_0_15_IRQHandler
        PUBWEAK GPIO5_Combined_0_15_IRQHandler
        PUBWEAK WDOG1_IRQHandler
        PUBWEAK ADC_ETC_IRQ0_IRQHandler
        PUBWEAK ADC_ETC_IRQ1_IRQHandler
        PUBWEAK ADC_ETC_IRQ2_IRQHandler
        PUBWEAK ADC_ETC_IRQ3_IRQHandler
        PUBWEAK ADC_ETC_ERROR_IRQ_IRQHandler
        PUBWEAK DefaultISR
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA0_DriverIRQHandler
DMA1_DriverIRQHandler
DMA2_DriverIRQHandler
DMA3_DriverIRQHandler
DMA4_DriverIRQHandler
DMA5_DriverIRQHandler
DMA6_DriverIRQHandler
DMA7_DriverIRQHandler
DMA8_DriverIRQHandler
DMA9_DriverIRQHandler
DMA10_DriverIRQHandler
DMA11_DriverIRQHandler
DMA12_DriverIRQHandler
DMA13_DriverIRQHandler
DMA14_DriverIRQHandler
DMA15_DriverIRQHandler
DMA_ERROR_DriverIRQHandler
CTI0_ERROR_IRQHandler
CTI1_ERROR_IRQHandler
CORE_IRQHandler
LPUART1_DriverIRQHandler
LPUART2_DriverIRQHandler
LPUART3_DriverIRQHandler
LPUART4_DriverIRQHandler
PIT_IRQHandler
USB_OTG1_IRQHandler
FLEXSPI_DriverIRQHandler
FLEXRAM_IRQHandler
LPI2C1_DriverIRQHandler
LPI2C2_DriverIRQHandler
GPT1_IRQHandler
GPT2_IRQHandler
LPSPI1_DriverIRQHandler
LPSPI2_DriverIRQHandler
PWM1_0_IRQHandler
PWM1_1_IRQHandler
PWM1_2_IRQHandler
PWM1_3_IRQHandler
PWM1_FAULT_IRQHandler
KPP_IRQHandler
SRC_IRQHandler
GPR_IRQ_IRQHandler
CCM_1_IRQHandler
CCM_2_IRQHandler
EWM_IRQHandler
WDOG2_IRQHandler
SNVS_HP_WRAPPER_IRQHandler
SNVS_HP_WRAPPER_TZ_IRQHandler
SNVS_LP_WRAPPER_IRQHandler
CSU_IRQHandler
DCP_IRQHandler
DCP_VMI_IRQHandler
Reserved68_IRQHandler
TRNG_IRQHandler
Reserved70_IRQHandler
Reserved71_IRQHandler
SAI1_DriverIRQHandler
RTWDOG_IRQHandler
SAI3_RX_DriverIRQHandler
SAI3_TX_DriverIRQHandler
SPDIF_DriverIRQHandler
PMU_IRQHandler
XBAR1_IRQ_0_1_2_3_IRQHandler
TEMP_LOW_HIGH_IRQHandler
TEMP_PANIC_IRQHandler
USB_PHY_IRQHandler
GPC_IRQHandler
ADC1_IRQHandler
FLEXIO1_DriverIRQHandler
DCDC_IRQHandler
GPIO1_Combined_0_15_IRQHandler
GPIO1_Combined_16_31_IRQHandler
GPIO2_Combined_0_15_IRQHandler
GPIO5_Combined_0_15_IRQHandler
WDOG1_IRQHandler
ADC_ETC_IRQ0_IRQHandler
ADC_ETC_IRQ1_IRQHandler
ADC_ETC_IRQ2_IRQHandler
ADC_ETC_IRQ3_IRQHandler
ADC_ETC_ERROR_IRQ_IRQHandler
DefaultISR
        B DefaultISR

        END
