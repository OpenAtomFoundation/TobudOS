/* ------------------------------------------------------------------------- */
/*  @file:    startup_MM32F5277.S                                            */
/*  @purpose: CMSIS Star-MC1 Core Device Startup File                        */
/*                                                                           */
/*  @version: 1.0                                                            */
/*  @date:    2022-07-01                                                     */
/*  @build:   b220701                                                        */
/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Copyright 2022 MindMotion                                                 */
/* All rights reserved.                                                      */
/*                                                                           */
/* SPDX-License-Identifier: BSD-3-Clause                                     */
/*****************************************************************************/
/* Version: ICC for ARM Embedded Processors                                  */
/*****************************************************************************/
    .syntax unified
    .arch armv8-m.main

                #define __INITIAL_SP     __StackTop
                #define __STACK_LIMIT    __StackLimit
                #if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
                #define __STACK_SEAL     __StackSeal
                #endif
                .section .vectors
                .align   2
                .globl   __Vectors
                .globl   __Vectors_End
                .globl   __Vectors_Size
__Vectors:
                .long    __INITIAL_SP                                   /*     Initial Stack Pointer */
                .long   Reset_Handler                                   /* Reset Handler */
                .long   NMI_Handler                                     /* NMI Handler*/
                .long   HardFault_Handler                               /* Hard Fault Handler*/
                .long   MemManage_Handler                               /* MPU Fault Handler*/
                .long   BusFault_Handler                                /* Bus Fault Handler*/
                .long   UsageFault_Handler                              /* Usage Fault Handler*/
                .long   0                                               /* Reserved*/
                .long   0                                               /* Reserved*/
                .long   0                                               /* Reserved*/
                .long   0                                               /* Reserved*/
                .long   SVC_Handler                                     /* SVCall Handler*/
                .long   DebugMon_Handler                                /* Debug Monitor Handler*/
                .long   0                                               /* Reserved*/
                .long   PendSV_Handler                                  /* PendSV Handler*/
                .long   SysTick_Handler                                 /* SysTick Handler*/

                .long   WWDG_IWDG_IRQHandler                            /* 0  Watchdog interrupt (IWDG is EXTI21) */
                .long   PVD_IRQHandler                                  /* 1  Supply Voltage Detect (PVD) Interrupt (EXTI16) */
                .long   BKP_TAMPER_IRQHandler                           /* 2  BKP intrusion detection interrupted */
                .long   RTC_IRQHandler                                  /* 3  RTC global interrupt */
                .long   FLASH_IRQHandler                                /* 4  Flash Global Interrupt */
                .long   RCC_CRS_IRQHandler                              /* 5  RCC and CRS global interrupt */
                .long   EXTI0_IRQHandler                                /* 6  EXTI line 0 interrupt */
                .long   EXTI1_IRQHandler                                /* 7  EXTI line 1 interrupt */
                .long   EXTI2_IRQHandler                                /* 8  EXTI line 2 interrupt */
                .long   EXTI3_IRQHandler                                /* 9  EXTI line 3 interrupt */
                .long   EXTI4_IRQHandler                                /* 10  EXTI line 4 interrupt */
                .long   DMA1_CH1_IRQHandler                             /* 11  DMA1 channel 1 global interrupt */
                .long   DMA1_CH2_IRQHandler                             /* 12  DMA1 channel 2 global interrupt */
                .long   DMA1_CH3_IRQHandler                             /* 13  DMA1 channel 3 global interrupt */
                .long   DMA1_CH4_IRQHandler                             /* 14  DMA1 channel 4 global interrupt */
                .long   DMA1_CH5_IRQHandler                             /* 15  DMA1 channel 5 global interrupt */
                .long   DMA1_CH6_IRQHandler                             /* 16  DMA1 channel 6 global interrupt */
                .long   DMA1_CH7_IRQHandler                             /* 17  DMA1 channel 7 global interrupt */
                .long   ADC1_2_IRQHandler                               /* 18  ADC1/2 global interrupt */
                .long   0                                               /* 19  Reserved */
                .long   DMA1_CH8_IRQHandler                             /* 20  DMA1 channel 8 global interrupt */
                .long   FlexCAN1_IRQHandler                             /* 21  FlexCAN1 global interrupt */
                .long   0                                               /* 22  Reserved */
                .long   EXTI9_5_IRQHandler                              /* 23  EXTI line[9:5] interrupt */
                .long   TIM1_BRK_IRQHandler                             /* 24  TIM1 brake interrupt */
                .long   TIM1_UP_IRQHandler                              /* 25  TIM1 update interrupted */
                .long   TIM1_TRG_COM_IRQHandler                         /* 26  TIM1 trigger/COM interrupt */
                .long   TIM1_CC_IRQHandler                              /* 27  TIM1 capture compare interrupt */
                .long   TIM2_IRQHandler                                 /* 28  TIM2 global interrupt */
                .long   TIM3_IRQHandler                                 /* 29  TIM3 global interrupt */
                .long   TIM4_IRQHandler                                 /* 30  TIM4 global interrupt */
                .long   I2C1_IRQHandler                                 /* 31  I2C1 global interrupt */
                .long   0                                               /* 32  Reserved */
                .long   I2C2_IRQHandler                                 /* 33  I2C2 global interrupt */
                .long   0                                               /* 34  Reserved */
                .long   SPI1_IRQHandler                                 /* 35  SPI1 global interrupt */
                .long   SPI2_IRQHandler                                 /* 36  SPI2 global interrupt */
                .long   UART1_IRQHandler                                /* 37  UART1 global interrupt */
                .long   UART2_IRQHandler                                /* 38  UART2 global interrupt */
                .long   UART3_IRQHandler                                /* 39  UART3 global interrupt */
                .long   EXTI15_10_IRQHandler                            /* 40  Interrupt on EXTI line[15:10] */
                .long   RTC_ALR_IRQHandler                              /* 41  RTC Alarm Interrupt (EXTI17) */
                .long   USB_WKUP_IRQHandler                             /* 42  USB Wakeup Interrupt (EXTI18) */
                .long   TIM8_BRK_IRQHandler                             /* 43  TIM8 brake interrupt */
                .long   TIM8_UP_IRQHandler                              /* 44  TIM8 update interrupted */
                .long   TIM8_TRG_COM_IRQHandler                         /* 45  TIM8 trigger/COM interrupt */
                .long   TIM8_CC_IRQHandler                              /* 46  TIM8 capture compare interrupt */
                .long   0                                               /* 47  Reserved */
                .long   0                                               /* 48  Reserved */
                .long   0                                               /* 49  Reserved */
                .long   TIM5_IRQHandler                                 /* 50  TIM5 global interrupt */
                .long   SPI3_IRQHandler                                 /* 51  SPI3 global interrupt */
                .long   UART4_IRQHandler                                /* 52  UART4 global interrupt */
                .long   UART5_IRQHandler                                /* 53  UART5 global interrupt */
                .long   TIM6_IRQHandler                                 /* 54  TIM6 global interrupt */
                .long   TIM7_IRQHandler                                 /* 55  TIM7 global interrupt */
                .long   DMA2_CH1_IRQHandler                             /* 56  DMA2 channel 1 global interrupt */
                .long   DMA2_CH2_IRQHandler                             /* 57  DMA2 channel 2 global interrupt */
                .long   DMA2_CH3_IRQHandler                             /* 58  DMA2 channel 3 global interrupt */
                .long   DMA2_CH4_IRQHandler                             /* 59  DMA2 channel 4 global interrupt */
                .long   DMA2_CH5_IRQHandler                             /* 60  DMA2 channel 5 global interrupt */
                .long   ENET_IRQHandler                                 /* 61  ENET global interrupt */
                .long   ENET_WKUP_IRQHandler                            /* 62  ENET wake-up interrupt (EXTI25) */
                .long   0                                               /* 63  Reserved */
                .long   COMP_IRQHandler                                 /* 64  Comparator 1/2/3 Global Interrupts (EXTI19/20/24) */
                .long   FlexCAN2_IRQHandler                             /* 65  FLexCAN2 global interrupt */
                .long   0                                               /* 66  Reserved */
                .long   USB_FS_IRQHandler                               /* 67  USB OTG global interrupt */
                .long   DMA2_CH6_IRQHandler                             /* 68  DMA2 channel 6 global interrupt */
                .long   DMA2_CH7_IRQHandler                             /* 69  DMA2 channel 7 global interrupt */
                .long   DMA2_CH8_IRQHandler                             /* 70  DMA2 channel 8 global interrupt */
                .long   UART6_IRQHandler                                /* 71  UART6 global interrupt */
                .long   0                                               /* 72  Reserved */
                .long   0                                               /* 73  Reserved */
                .long   0                                               /* 74  Reserved */
                .long   0                                               /* 75  Reserved */
                .long   0                                               /* 76  Reserved */
                .long   0                                               /* 77  Reserved */
                .long   0                                               /* 78  Reserved */
                .long   0                                               /* 79  Reserved */
                .long   0                                               /* 80  Reserved */
                .long   0                                               /* 81  Reserved */
                .long   UART7_IRQHandler                                /* 82  UART7 global interrupt */
                .long   0                                               /* 83  Reserved */
                .long   0                                               /* 84  Reserved */
                .long   0                                               /* 85  Reserved */
                .long   0                                               /* 86  Reserved */
                .long   0                                               /* 87  Reserved */
                .long   0                                               /* 88  Reserved */
                .long   0                                               /* 89  Reserved */
                .long   0                                               /* 90  Reserved */
                .long   0                                               /* 91  Reserved */
                .long   0                                               /* 92  Reserved */
                .long   0                                               /* 93  Reserved */
                .long   0                                               /* 94  Reserved */
                .long   QSPI_IRQHandler                                 /* 95  QSPI global interrupt */
                .long   0                                               /* 96  Reserved */
                .long   0                                               /* 97  Reserved */
                .long   0                                               /* 98  Reserved */
                .long   0                                               /* 99  Reserved */
                .long   0                                               /* 100  Reserved */
                .long   0                                               /* 101  Reserved */
                .long   LPTIM_IRQHandler                                /* 102  LPTIM global interrupt (EXTI22) */
                .long   0                                               /* 103  Reserved */
                .long   LPUART_IRQHandler                               /* 104  LPUART global interrupt (EXTI23) */

                .space   (470 * 4)                          /* Interrupts 10 .. 480 are left out */
__Vectors_End:
                .equ     __Vectors_Size, __Vectors_End - __Vectors
                .size    __Vectors, . - __Vectors


                .thumb
                .section .text
                .align   2

                .thumb_func
                .type    Reset_Handler, %function
                .globl   Reset_Handler
                .fnstart
Reset_Handler:
                ldr      r0, =__INITIAL_SP
                msr      psp, r0

                #ifdef STACK_LIMIT_CHECK
                ldr      r0, =__STACK_LIMIT
                msr      msplim, r0
                msr      psplim, r0
                #else
                ldr      r0, =0xE000ED14
                ldr      r1, [r0]
                orr      r1, #0x400
                str      r1, [r0]
                #endif

                #if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
                ldr      r0, =__STACK_SEAL
                ldr      r1, =0xFEF5EDA5U
                strd     r1,r1,[r0,#0]
                #endif

                bl       SystemInit

                ldr      r4, =__copy_table_start__
                ldr      r5, =__copy_table_end__

.L_loop0:
                cmp      r4, r5
                bge      .L_loop0_done
                ldr      r1, [r4]                /* source address */
                ldr      r2, [r4, #4]            /* destination address */
                ldr      r3, [r4, #8]            /* word count */
                lsls     r3, r3, #2              /* byte count */

.L_loop0_0:
                subs     r3, #4                  /* decrement byte count */
                ittt     ge
                ldrge    r0, [r1, r3]
                strge    r0, [r2, r3]
                bge      .L_loop0_0

                adds     r4, #12
                b        .L_loop0
.L_loop0_done:

                ldr      r3, =__zero_table_start__
                ldr      r4, =__zero_table_end__

.L_loop2:
                cmp      r3, r4
                bge      .L_loop2_done
                ldr      r1, [r3]                /* destination address */
                ldr      r2, [r3, #4]            /* word count */
                lsls     r2, r2, #2              /* byte count */
                movs     r0, 0

.L_loop2_0:
                subs     r2, #4                  /* decrement byte count */
                itt      ge
                strge    r0, [r1, r2]
                bge      .L_loop2_0

                adds     r3, #8
                b        .L_loop2
.L_loop2_done:

                bl       _start

                .fnend
                .size    Reset_Handler, . - Reset_Handler


/* The default macro is not used for HardFault_Handler
 * because this results in a poor debug illusion.
 */
                .thumb_func
                .type    HardFault_Handler, %function
                .weak    HardFault_Handler
                .fnstart
HardFault_Handler:
                b        .
                .fnend
                .size    HardFault_Handler, . - HardFault_Handler

                .thumb_func
                .type    Default_Handler, %function
                .weak    Default_Handler
                .fnstart
Default_Handler:
                b        .
                .fnend
                .size    Default_Handler, . - Default_Handler

/* Macro to define default exception/interrupt handlers.
 * Default handler are weak symbols with an endless loop.
 * They can be overwritten by real handlers.
 */
                .macro   Set_Default_Handler  Handler_Name
                .weak    \Handler_Name
                .set     \Handler_Name, Default_Handler
                .endm

/* Exception Handlers */
    Set_Default_Handler    NMI_Handler
    Set_Default_Handler    MemManage_Handler
    Set_Default_Handler    BusFault_Handler
    Set_Default_Handler    UsageFault_Handler
    Set_Default_Handler    DebugMon_Handler
    Set_Default_Handler    SVC_Handler
    Set_Default_Handler    PendSV_Handler
    Set_Default_Handler    SysTick_Handler

    Set_Default_Handler    WWDG_IWDG_IRQHandler
    Set_Default_Handler    PVD_IRQHandler
    Set_Default_Handler    BKP_TAMPER_IRQHandler
    Set_Default_Handler    RTC_IRQHandler
    Set_Default_Handler    FLASH_IRQHandler
    Set_Default_Handler    RCC_CRS_IRQHandler
    Set_Default_Handler    EXTI0_IRQHandler
    Set_Default_Handler    EXTI1_IRQHandler
    Set_Default_Handler    EXTI2_IRQHandler
    Set_Default_Handler    EXTI3_IRQHandler
    Set_Default_Handler    EXTI4_IRQHandler
    Set_Default_Handler    DMA1_CH1_IRQHandler
    Set_Default_Handler    DMA1_CH2_IRQHandler
    Set_Default_Handler    DMA1_CH3_IRQHandler
    Set_Default_Handler    DMA1_CH4_IRQHandler
    Set_Default_Handler    DMA1_CH5_IRQHandler
    Set_Default_Handler    DMA1_CH6_IRQHandler
    Set_Default_Handler    DMA1_CH7_IRQHandler
    Set_Default_Handler    ADC1_2_IRQHandler
    Set_Default_Handler    DMA1_CH8_IRQHandler
    Set_Default_Handler    FlexCAN1_IRQHandler
    Set_Default_Handler    EXTI9_5_IRQHandler
    Set_Default_Handler    TIM1_BRK_IRQHandler
    Set_Default_Handler    TIM1_UP_IRQHandler
    Set_Default_Handler    TIM1_TRG_COM_IRQHandler
    Set_Default_Handler    TIM1_CC_IRQHandler
    Set_Default_Handler    TIM2_IRQHandler
    Set_Default_Handler    TIM3_IRQHandler
    Set_Default_Handler    TIM4_IRQHandler
    Set_Default_Handler    I2C1_IRQHandler
    Set_Default_Handler    I2C2_IRQHandler
    Set_Default_Handler    SPI1_IRQHandler
    Set_Default_Handler    SPI2_IRQHandler
    Set_Default_Handler    UART1_IRQHandler
    Set_Default_Handler    UART2_IRQHandler
    Set_Default_Handler    UART3_IRQHandler
    Set_Default_Handler    EXTI15_10_IRQHandler
    Set_Default_Handler    RTC_ALR_IRQHandler
    Set_Default_Handler    USB_WKUP_IRQHandler
    Set_Default_Handler    TIM8_BRK_IRQHandler
    Set_Default_Handler    TIM8_UP_IRQHandler
    Set_Default_Handler    TIM8_TRG_COM_IRQHandler
    Set_Default_Handler    TIM8_CC_IRQHandler
    Set_Default_Handler    TIM5_IRQHandler
    Set_Default_Handler    SPI3_IRQHandler
    Set_Default_Handler    UART4_IRQHandler
    Set_Default_Handler    UART5_IRQHandler
    Set_Default_Handler    TIM6_IRQHandler
    Set_Default_Handler    TIM7_IRQHandler
    Set_Default_Handler    DMA2_CH1_IRQHandler
    Set_Default_Handler    DMA2_CH2_IRQHandler
    Set_Default_Handler    DMA2_CH3_IRQHandler
    Set_Default_Handler    DMA2_CH4_IRQHandler
    Set_Default_Handler    DMA2_CH5_IRQHandler
    Set_Default_Handler    ENET_IRQHandler
    Set_Default_Handler    ENET_WKUP_IRQHandler
    Set_Default_Handler    COMP_IRQHandler
    Set_Default_Handler    FlexCAN2_IRQHandler
    Set_Default_Handler    USB_FS_IRQHandler
    Set_Default_Handler    DMA2_CH6_IRQHandler
    Set_Default_Handler    DMA2_CH7_IRQHandler
    Set_Default_Handler    DMA2_CH8_IRQHandler
    Set_Default_Handler    UART6_IRQHandler
    Set_Default_Handler    UART7_IRQHandler
    Set_Default_Handler    QSPI_IRQHandler
    Set_Default_Handler    LPTIM_IRQHandler
    Set_Default_Handler    LPUART_IRQHandler

    .end
