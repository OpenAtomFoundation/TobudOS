/* ------------------------------------------------------------------------- */
/*  @file:    startup_MIMXRT1011.s                                           */
/*  @purpose: CMSIS Cortex-M7 Core Device Startup File                       */
/*            MIMXRT1011                                                     */
/*  @version: 1.0                                                            */
/*  @date:    2019-8-1                                                       */
/*  @build:   b190801                                                        */
/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Copyright 1997-2016 Freescale Semiconductor, Inc.                         */
/* Copyright 2016-2019 NXP                                                   */
/* All rights reserved.                                                      */
/*                                                                           */
/* SPDX-License-Identifier: BSD-3-Clause                                     */
/*****************************************************************************/
/* Version: GCC for ARM Embedded Processors                                  */
/*****************************************************************************/
    .syntax unified
    .arch armv7-m

    .section .isr_vector, "a"
    .align 2
    .globl __isr_vector
__isr_vector:
    .long   __StackTop                                      /* Top of Stack */
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

                                                            /* External Interrupts*/
    .long   DMA0_IRQHandler                                 /* DMA channel 0 transfer complete*/
    .long   DMA1_IRQHandler                                 /* DMA channel 1 transfer complete*/
    .long   DMA2_IRQHandler                                 /* DMA channel 2 transfer complete*/
    .long   DMA3_IRQHandler                                 /* DMA channel 3 transfer complete*/
    .long   DMA4_IRQHandler                                 /* DMA channel 4 transfer complete*/
    .long   DMA5_IRQHandler                                 /* DMA channel 5 transfer complete*/
    .long   DMA6_IRQHandler                                 /* DMA channel 6 transfer complete*/
    .long   DMA7_IRQHandler                                 /* DMA channel 7 transfer complete*/
    .long   DMA8_IRQHandler                                 /* DMA channel 8 transfer complete*/
    .long   DMA9_IRQHandler                                 /* DMA channel 9 transfer complete*/
    .long   DMA10_IRQHandler                                /* DMA channel 10 transfer complete*/
    .long   DMA11_IRQHandler                                /* DMA channel 11 transfer complete*/
    .long   DMA12_IRQHandler                                /* DMA channel 12 transfer complete*/
    .long   DMA13_IRQHandler                                /* DMA channel 13 transfer complete*/
    .long   DMA14_IRQHandler                                /* DMA channel 14 transfer complete*/
    .long   DMA15_IRQHandler                                /* DMA channel 15 transfer complete*/
    .long   DMA_ERROR_IRQHandler                            /* DMA error interrupt channels 0-15*/
    .long   CTI0_ERROR_IRQHandler                           /* CTI trigger outputs*/
    .long   CTI1_ERROR_IRQHandler                           /* CTI trigger outputs*/
    .long   CORE_IRQHandler                                 /* CorePlatform exception IRQ*/
    .long   LPUART1_IRQHandler                              /* LPUART1 TX interrupt and RX interrupt*/
    .long   LPUART2_IRQHandler                              /* LPUART2 TX interrupt and RX interrupt*/
    .long   LPUART3_IRQHandler                              /* LPUART3 TX interrupt and RX interrupt*/
    .long   LPUART4_IRQHandler                              /* LPUART4 TX interrupt and RX interrupt*/
    .long   PIT_IRQHandler                                  /* PIT interrupt*/
    .long   USB_OTG1_IRQHandler                             /* USBO2 USB OTG1*/
    .long   FLEXSPI_IRQHandler                              /* FlexSPI0 interrupt*/
    .long   FLEXRAM_IRQHandler                              /* FlexRAM address out of range Or access hit IRQ*/
    .long   LPI2C1_IRQHandler                               /* LPI2C1 interrupt*/
    .long   LPI2C2_IRQHandler                               /* LPI2C2 interrupt*/
    .long   GPT1_IRQHandler                                 /* GPT1 interrupt*/
    .long   GPT2_IRQHandler                                 /* GPT2 interrupt*/
    .long   LPSPI1_IRQHandler                               /* LPSPI1 single interrupt vector for all sources*/
    .long   LPSPI2_IRQHandler                               /* LPSPI2 single interrupt vector for all sources*/
    .long   PWM1_0_IRQHandler                               /* PWM1 capture 0, compare 0, or reload 0 interrupt*/
    .long   PWM1_1_IRQHandler                               /* PWM1 capture 1, compare 1, or reload 0 interrupt*/
    .long   PWM1_2_IRQHandler                               /* PWM1 capture 2, compare 2, or reload 0 interrupt*/
    .long   PWM1_3_IRQHandler                               /* PWM1 capture 3, compare 3, or reload 0 interrupt*/
    .long   PWM1_FAULT_IRQHandler                           /* PWM1 fault or reload error interrupt*/
    .long   KPP_IRQHandler                                  /* Keypad nterrupt*/
    .long   SRC_IRQHandler                                  /* SRC interrupt*/
    .long   GPR_IRQ_IRQHandler                              /* Used to notify cores on exception condition while boot*/
    .long   CCM_1_IRQHandler                                /* CCM IRQ1 interrupt*/
    .long   CCM_2_IRQHandler                                /* CCM IRQ2 interrupt*/
    .long   EWM_IRQHandler                                  /* EWM interrupt*/
    .long   WDOG2_IRQHandler                                /* WDOG2 interrupt*/
    .long   SNVS_HP_WRAPPER_IRQHandler                      /* SNVS Functional Interrupt*/
    .long   SNVS_HP_WRAPPER_TZ_IRQHandler                   /* SNVS Security Interrupt*/
    .long   SNVS_LP_WRAPPER_IRQHandler                      /* ON-OFF button press shorter than 5 secs (pulse event)*/
    .long   CSU_IRQHandler                                  /* CSU interrupt*/
    .long   DCP_IRQHandler                                  /* Combined DCP channel interrupts(except channel 0) and CRC interrupt*/
    .long   DCP_VMI_IRQHandler                              /* IRQ of DCP channel 0*/
    .long   Reserved68_IRQHandler                           /* Reserved interrupt*/
    .long   TRNG_IRQHandler                                 /* TRNG interrupt*/
    .long   Reserved70_IRQHandler                           /* Reserved interrupt*/
    .long   Reserved71_IRQHandler                           /* Reserved interrupt*/
    .long   SAI1_IRQHandler                                 /* SAI1 interrupt*/
    .long   RTWDOG_IRQHandler                               /* RTWDOG interrupt*/
    .long   SAI3_RX_IRQHandler                              /* SAI3 interrupt*/
    .long   SAI3_TX_IRQHandler                              /* SAI3 interrupt*/
    .long   SPDIF_IRQHandler                                /* SPDIF interrupt*/
    .long   PMU_IRQHandler                                  /* PMU interrupt*/
    .long   XBAR1_IRQ_0_1_2_3_IRQHandler                    /* XBAR1 interrupt*/
    .long   TEMP_LOW_HIGH_IRQHandler                        /* TEMPMON interrupt*/
    .long   TEMP_PANIC_IRQHandler                           /* TEMPMON interrupt*/
    .long   USB_PHY_IRQHandler                              /* USBPHY (OTG1 UTMI), Interrupt*/
    .long   GPC_IRQHandler                                  /* GPC interrupt*/
    .long   ADC1_IRQHandler                                 /* ADC1 interrupt*/
    .long   FLEXIO1_IRQHandler                              /* FLEXIO1 interrupt*/
    .long   DCDC_IRQHandler                                 /* DCDC interrupt*/
    .long   GPIO1_Combined_0_15_IRQHandler                  /* Combined interrupt indication for GPIO1 signal 0 throughout 15*/
    .long   GPIO1_Combined_16_31_IRQHandler                 /* Combined interrupt indication for GPIO1 signal 16 throughout 31*/
    .long   GPIO2_Combined_0_15_IRQHandler                  /* Combined interrupt indication for GPIO2 signal 0 throughout 15*/
    .long   GPIO5_Combined_0_15_IRQHandler                  /* Combined interrupt indication for GPIO5 signal 0 throughout 15*/
    .long   WDOG1_IRQHandler                                /* WDOG1 interrupt*/
    .long   ADC_ETC_IRQ0_IRQHandler                         /* ADCETC IRQ0 interrupt*/
    .long   ADC_ETC_IRQ1_IRQHandler                         /* ADCETC IRQ1 interrupt*/
    .long   ADC_ETC_IRQ2_IRQHandler                         /* ADCETC IRQ2 interrupt*/
    .long   ADC_ETC_IRQ3_IRQHandler                         /* ADCETC IRQ3 interrupt*/
    .long   ADC_ETC_ERROR_IRQ_IRQHandler                    /* ADCETC Error IRQ interrupt*/
    .long   DefaultISR                                      /* 96*/
    .long   DefaultISR                                      /* 97*/
    .long   DefaultISR                                      /* 98*/
    .long   DefaultISR                                      /* 99*/
    .long   DefaultISR                                      /* 100*/
    .long   DefaultISR                                      /* 101*/
    .long   DefaultISR                                      /* 102*/
    .long   DefaultISR                                      /* 103*/
    .long   DefaultISR                                      /* 104*/
    .long   DefaultISR                                      /* 105*/
    .long   DefaultISR                                      /* 106*/
    .long   DefaultISR                                      /* 107*/
    .long   DefaultISR                                      /* 108*/
    .long   DefaultISR                                      /* 109*/
    .long   DefaultISR                                      /* 110*/
    .long   DefaultISR                                      /* 111*/
    .long   DefaultISR                                      /* 112*/
    .long   DefaultISR                                      /* 113*/
    .long   DefaultISR                                      /* 114*/
    .long   DefaultISR                                      /* 115*/
    .long   DefaultISR                                      /* 116*/
    .long   DefaultISR                                      /* 117*/
    .long   DefaultISR                                      /* 118*/
    .long   DefaultISR                                      /* 119*/
    .long   DefaultISR                                      /* 120*/
    .long   DefaultISR                                      /* 121*/
    .long   DefaultISR                                      /* 122*/
    .long   DefaultISR                                      /* 123*/
    .long   DefaultISR                                      /* 124*/
    .long   DefaultISR                                      /* 125*/
    .long   DefaultISR                                      /* 126*/
    .long   DefaultISR                                      /* 127*/
    .long   DefaultISR                                      /* 128*/
    .long   DefaultISR                                      /* 129*/
    .long   DefaultISR                                      /* 130*/
    .long   DefaultISR                                      /* 131*/
    .long   DefaultISR                                      /* 132*/
    .long   DefaultISR                                      /* 133*/
    .long   DefaultISR                                      /* 134*/
    .long   DefaultISR                                      /* 135*/
    .long   DefaultISR                                      /* 136*/
    .long   DefaultISR                                      /* 137*/
    .long   DefaultISR                                      /* 138*/
    .long   DefaultISR                                      /* 139*/
    .long   DefaultISR                                      /* 140*/
    .long   DefaultISR                                      /* 141*/
    .long   DefaultISR                                      /* 142*/
    .long   DefaultISR                                      /* 143*/
    .long   DefaultISR                                      /* 144*/
    .long   DefaultISR                                      /* 145*/
    .long   DefaultISR                                      /* 146*/
    .long   DefaultISR                                      /* 147*/
    .long   DefaultISR                                      /* 148*/
    .long   DefaultISR                                      /* 149*/
    .long   DefaultISR                                      /* 150*/
    .long   DefaultISR                                      /* 151*/
    .long   DefaultISR                                      /* 152*/
    .long   DefaultISR                                      /* 153*/
    .long   DefaultISR                                      /* 154*/
    .long   DefaultISR                                      /* 155*/
    .long   DefaultISR                                      /* 156*/
    .long   DefaultISR                                      /* 157*/
    .long   DefaultISR                                      /* 158*/
    .long   DefaultISR                                      /* 159*/
    .long   DefaultISR                                      /* 160*/
    .long   DefaultISR                                      /* 161*/
    .long   DefaultISR                                      /* 162*/
    .long   DefaultISR                                      /* 163*/
    .long   DefaultISR                                      /* 164*/
    .long   DefaultISR                                      /* 165*/
    .long   DefaultISR                                      /* 166*/
    .long   DefaultISR                                      /* 167*/
    .long   DefaultISR                                      /* 168*/
    .long   DefaultISR                                      /* 169*/
    .long   DefaultISR                                      /* 170*/
    .long   DefaultISR                                      /* 171*/
    .long   DefaultISR                                      /* 172*/
    .long   DefaultISR                                      /* 173*/
    .long   DefaultISR                                      /* 174*/
    .long   DefaultISR                                      /* 175*/
    .long   DefaultISR                                      /* 176*/
    .long   DefaultISR                                      /* 177*/
    .long   DefaultISR                                      /* 178*/
    .long   DefaultISR                                      /* 179*/
    .long   DefaultISR                                      /* 180*/
    .long   DefaultISR                                      /* 181*/
    .long   DefaultISR                                      /* 182*/
    .long   DefaultISR                                      /* 183*/
    .long   DefaultISR                                      /* 184*/
    .long   DefaultISR                                      /* 185*/
    .long   DefaultISR                                      /* 186*/
    .long   DefaultISR                                      /* 187*/
    .long   DefaultISR                                      /* 188*/
    .long   DefaultISR                                      /* 189*/
    .long   DefaultISR                                      /* 190*/
    .long   DefaultISR                                      /* 191*/
    .long   DefaultISR                                      /* 192*/
    .long   DefaultISR                                      /* 193*/
    .long   DefaultISR                                      /* 194*/
    .long   DefaultISR                                      /* 195*/
    .long   DefaultISR                                      /* 196*/
    .long   DefaultISR                                      /* 197*/
    .long   DefaultISR                                      /* 198*/
    .long   DefaultISR                                      /* 199*/
    .long   DefaultISR                                      /* 200*/
    .long   DefaultISR                                      /* 201*/
    .long   DefaultISR                                      /* 202*/
    .long   DefaultISR                                      /* 203*/
    .long   DefaultISR                                      /* 204*/
    .long   DefaultISR                                      /* 205*/
    .long   DefaultISR                                      /* 206*/
    .long   DefaultISR                                      /* 207*/
    .long   DefaultISR                                      /* 208*/
    .long   DefaultISR                                      /* 209*/
    .long   DefaultISR                                      /* 210*/
    .long   DefaultISR                                      /* 211*/
    .long   DefaultISR                                      /* 212*/
    .long   DefaultISR                                      /* 213*/
    .long   DefaultISR                                      /* 214*/
    .long   DefaultISR                                      /* 215*/
    .long   DefaultISR                                      /* 216*/
    .long   DefaultISR                                      /* 217*/
    .long   DefaultISR                                      /* 218*/
    .long   DefaultISR                                      /* 219*/
    .long   DefaultISR                                      /* 220*/
    .long   DefaultISR                                      /* 221*/
    .long   DefaultISR                                      /* 222*/
    .long   DefaultISR                                      /* 223*/
    .long   DefaultISR                                      /* 224*/
    .long   DefaultISR                                      /* 225*/
    .long   DefaultISR                                      /* 226*/
    .long   DefaultISR                                      /* 227*/
    .long   DefaultISR                                      /* 228*/
    .long   DefaultISR                                      /* 229*/
    .long   DefaultISR                                      /* 230*/
    .long   DefaultISR                                      /* 231*/
    .long   DefaultISR                                      /* 232*/
    .long   DefaultISR                                      /* 233*/
    .long   DefaultISR                                      /* 234*/
    .long   DefaultISR                                      /* 235*/
    .long   DefaultISR                                      /* 236*/
    .long   DefaultISR                                      /* 237*/
    .long   DefaultISR                                      /* 238*/
    .long   DefaultISR                                      /* 239*/
    .long   DefaultISR                                      /* 240*/
    .long   DefaultISR                                      /* 241*/
    .long   DefaultISR                                      /* 242*/
    .long   DefaultISR                                      /* 243*/
    .long   DefaultISR                                      /* 244*/
    .long   DefaultISR                                      /* 245*/
    .long   DefaultISR                                      /* 246*/
    .long   DefaultISR                                      /* 247*/
    .long   DefaultISR                                      /* 248*/
    .long   DefaultISR                                      /* 249*/
    .long   DefaultISR                                      /* 250*/
    .long   DefaultISR                                      /* 251*/
    .long   DefaultISR                                      /* 252*/
    .long   DefaultISR                                      /* 253*/
    .long   DefaultISR                                      /* 254*/
    .long   0xFFFFFFFF                                      /*  Reserved for user TRIM value*/

    .size    __isr_vector, . - __isr_vector

    .text
    .thumb

/* Reset Handler */

    .thumb_func
    .align 2
    .globl   Reset_Handler
    .weak    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
    cpsid   i               /* Mask interrupts */
    .equ    VTOR, 0xE000ED08
    ldr     r0, =VTOR
    ldr     r1, =__isr_vector
    str     r1, [r0]
    ldr     r2, [r1]
    msr     msp, r2
#ifndef __NO_SYSTEM_INIT
    ldr   r0,=SystemInit
    blx   r0
#endif
/*     Loop to copy data from read only memory to RAM. The ranges
 *      of copy from/to are specified by following symbols evaluated in
 *      linker script.
 *      __etext: End of code section, i.e., begin of data sections to copy from.
 *      __data_start__/__data_end__: RAM address range that data should be
 *      __noncachedata_start__/__noncachedata_end__ : none cachable region
 *      __ram_function_start__/__ram_function_end__ : ramfunction region
 *      copied to. Both must be aligned to 4 bytes boundary.  */

    ldr    r1, =__etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__

#ifdef __PERFORMANCE_IMPLEMENTATION
/* Here are two copies of loop implementations. First one favors performance
 * and the second one favors code size. Default uses the second one.
 * Define macro "__PERFORMANCE_IMPLEMENTATION" in project to use the first one */
    subs    r3, r2
    ble    .LC1
.LC0:
    subs    r3, #4
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    bgt    .LC0
.LC1:
#else  /* code size implemenation */
.LC0:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .LC0
#endif
#ifdef __STARTUP_INITIALIZE_RAMFUNCTION
    ldr    r2, =__ram_function_start__
    ldr    r3, =__ram_function_end__
#ifdef __PERFORMANCE_IMPLEMENTATION
/* Here are two copies of loop implementations. First one favors performance
 * and the second one favors code size. Default uses the second one.
 * Define macro "__PERFORMANCE_IMPLEMENTATION" in project to use the first one */
    subs    r3, r2
    ble    .LC_ramfunc_copy_end
.LC_ramfunc_copy_start:
    subs    r3, #4
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    bgt    .LC_ramfunc_copy_start
.LC_ramfunc_copy_end:
#else  /* code size implemenation */
.LC_ramfunc_copy_start:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .LC_ramfunc_copy_start
#endif
#endif /* __STARTUP_INITIALIZE_RAMFUNCTION */
#ifdef __STARTUP_INITIALIZE_NONCACHEDATA
    ldr    r2, =__noncachedata_start__
    ldr    r3, =__noncachedata_init_end__
#ifdef __PERFORMANCE_IMPLEMENTATION
/* Here are two copies of loop implementations. First one favors performance
 * and the second one favors code size. Default uses the second one.
 * Define macro "__PERFORMANCE_IMPLEMENTATION" in project to use the first one */
    subs    r3, r2
    ble    .LC3
.LC2:
    subs    r3, #4
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    bgt    .LC2
.LC3:
#else  /* code size implemenation */
.LC2:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .LC2
#endif
/* zero inited ncache section initialization */
    ldr r3, =__noncachedata_end__
    movs    r0,0
.LC4:
    cmp    r2,r3
    itt    lt
    strlt   r0,[r2],#4
    blt    .LC4
#endif /* __STARTUP_INITIALIZE_NONCACHEDATA */

#ifdef __STARTUP_CLEAR_BSS
/*     This part of work usually is done in C library startup code. Otherwise,
 *     define this macro to enable it in this startup.
 *
 *     Loop to zero out BSS section, which uses following symbols
 *     in linker script:
 *      __bss_start__: start of BSS section. Must align to 4
 *      __bss_end__: end of BSS section. Must align to 4
 */
    ldr r1, =__bss_start__
    ldr r2, =__bss_end__

    movs    r0, 0
.LC5:
    cmp     r1, r2
    itt    lt
    strlt   r0, [r1], #4
    blt    .LC5
#endif /* __STARTUP_CLEAR_BSS */

    cpsie   i               /* Unmask interrupts */
#ifndef __START
#define __START _start
#endif
#ifndef __ATOLLIC__
    ldr   r0,=__START
    blx   r0
#else
    ldr   r0,=__libc_init_array
    blx   r0
    ldr   r0,=main
    bx    r0
#endif
    .pool
    .size Reset_Handler, . - Reset_Handler

    .align  1
    .thumb_func
    .weak DefaultISR
    .type DefaultISR, %function
DefaultISR:
    b DefaultISR
    .size DefaultISR, . - DefaultISR

    .align 1
    .thumb_func
    .weak NMI_Handler
    .type NMI_Handler, %function
NMI_Handler:
    ldr   r0,=NMI_Handler
    bx    r0
    .size NMI_Handler, . - NMI_Handler

    .align 1
    .thumb_func
    .weak HardFault_Handler
    .type HardFault_Handler, %function
HardFault_Handler:
    ldr   r0,=HardFault_Handler
    bx    r0
    .size HardFault_Handler, . - HardFault_Handler

    .align 1
    .thumb_func
    .weak SVC_Handler
    .type SVC_Handler, %function
SVC_Handler:
    ldr   r0,=SVC_Handler
    bx    r0
    .size SVC_Handler, . - SVC_Handler

    .align 1
    .thumb_func
    .weak PendSV_Handler
    .type PendSV_Handler, %function
PendSV_Handler:
    ldr   r0,=PendSV_Handler
    bx    r0
    .size PendSV_Handler, . - PendSV_Handler

    .align 1
    .thumb_func
    .weak SysTick_Handler
    .type SysTick_Handler, %function
SysTick_Handler:
    ldr   r0,=SysTick_Handler
    bx    r0
    .size SysTick_Handler, . - SysTick_Handler

    .align 1
    .thumb_func
    .weak DMA0_IRQHandler
    .type DMA0_IRQHandler, %function
DMA0_IRQHandler:
    ldr   r0,=DMA0_DriverIRQHandler
    bx    r0
    .size DMA0_IRQHandler, . - DMA0_IRQHandler

    .align 1
    .thumb_func
    .weak DMA1_IRQHandler
    .type DMA1_IRQHandler, %function
DMA1_IRQHandler:
    ldr   r0,=DMA1_DriverIRQHandler
    bx    r0
    .size DMA1_IRQHandler, . - DMA1_IRQHandler

    .align 1
    .thumb_func
    .weak DMA2_IRQHandler
    .type DMA2_IRQHandler, %function
DMA2_IRQHandler:
    ldr   r0,=DMA2_DriverIRQHandler
    bx    r0
    .size DMA2_IRQHandler, . - DMA2_IRQHandler

    .align 1
    .thumb_func
    .weak DMA3_IRQHandler
    .type DMA3_IRQHandler, %function
DMA3_IRQHandler:
    ldr   r0,=DMA3_DriverIRQHandler
    bx    r0
    .size DMA3_IRQHandler, . - DMA3_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_IRQHandler
    .type DMA4_IRQHandler, %function
DMA4_IRQHandler:
    ldr   r0,=DMA4_DriverIRQHandler
    bx    r0
    .size DMA4_IRQHandler, . - DMA4_IRQHandler

    .align 1
    .thumb_func
    .weak DMA5_IRQHandler
    .type DMA5_IRQHandler, %function
DMA5_IRQHandler:
    ldr   r0,=DMA5_DriverIRQHandler
    bx    r0
    .size DMA5_IRQHandler, . - DMA5_IRQHandler

    .align 1
    .thumb_func
    .weak DMA6_IRQHandler
    .type DMA6_IRQHandler, %function
DMA6_IRQHandler:
    ldr   r0,=DMA6_DriverIRQHandler
    bx    r0
    .size DMA6_IRQHandler, . - DMA6_IRQHandler

    .align 1
    .thumb_func
    .weak DMA7_IRQHandler
    .type DMA7_IRQHandler, %function
DMA7_IRQHandler:
    ldr   r0,=DMA7_DriverIRQHandler
    bx    r0
    .size DMA7_IRQHandler, . - DMA7_IRQHandler

    .align 1
    .thumb_func
    .weak DMA8_IRQHandler
    .type DMA8_IRQHandler, %function
DMA8_IRQHandler:
    ldr   r0,=DMA8_DriverIRQHandler
    bx    r0
    .size DMA8_IRQHandler, . - DMA8_IRQHandler

    .align 1
    .thumb_func
    .weak DMA9_IRQHandler
    .type DMA9_IRQHandler, %function
DMA9_IRQHandler:
    ldr   r0,=DMA9_DriverIRQHandler
    bx    r0
    .size DMA9_IRQHandler, . - DMA9_IRQHandler

    .align 1
    .thumb_func
    .weak DMA10_IRQHandler
    .type DMA10_IRQHandler, %function
DMA10_IRQHandler:
    ldr   r0,=DMA10_DriverIRQHandler
    bx    r0
    .size DMA10_IRQHandler, . - DMA10_IRQHandler

    .align 1
    .thumb_func
    .weak DMA11_IRQHandler
    .type DMA11_IRQHandler, %function
DMA11_IRQHandler:
    ldr   r0,=DMA11_DriverIRQHandler
    bx    r0
    .size DMA11_IRQHandler, . - DMA11_IRQHandler

    .align 1
    .thumb_func
    .weak DMA12_IRQHandler
    .type DMA12_IRQHandler, %function
DMA12_IRQHandler:
    ldr   r0,=DMA12_DriverIRQHandler
    bx    r0
    .size DMA12_IRQHandler, . - DMA12_IRQHandler

    .align 1
    .thumb_func
    .weak DMA13_IRQHandler
    .type DMA13_IRQHandler, %function
DMA13_IRQHandler:
    ldr   r0,=DMA13_DriverIRQHandler
    bx    r0
    .size DMA13_IRQHandler, . - DMA13_IRQHandler

    .align 1
    .thumb_func
    .weak DMA14_IRQHandler
    .type DMA14_IRQHandler, %function
DMA14_IRQHandler:
    ldr   r0,=DMA14_DriverIRQHandler
    bx    r0
    .size DMA14_IRQHandler, . - DMA14_IRQHandler

    .align 1
    .thumb_func
    .weak DMA15_IRQHandler
    .type DMA15_IRQHandler, %function
DMA15_IRQHandler:
    ldr   r0,=DMA15_DriverIRQHandler
    bx    r0
    .size DMA15_IRQHandler, . - DMA15_IRQHandler

    .align 1
    .thumb_func
    .weak DMA_ERROR_IRQHandler
    .type DMA_ERROR_IRQHandler, %function
DMA_ERROR_IRQHandler:
    ldr   r0,=DMA_ERROR_DriverIRQHandler
    bx    r0
    .size DMA_ERROR_IRQHandler, . - DMA_ERROR_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART1_IRQHandler
    .type LPUART1_IRQHandler, %function
LPUART1_IRQHandler:
    ldr   r0,=LPUART1_DriverIRQHandler
    bx    r0
    .size LPUART1_IRQHandler, . - LPUART1_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART2_IRQHandler
    .type LPUART2_IRQHandler, %function
LPUART2_IRQHandler:
    ldr   r0,=LPUART2_DriverIRQHandler
    bx    r0
    .size LPUART2_IRQHandler, . - LPUART2_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART3_IRQHandler
    .type LPUART3_IRQHandler, %function
LPUART3_IRQHandler:
    ldr   r0,=LPUART3_DriverIRQHandler
    bx    r0
    .size LPUART3_IRQHandler, . - LPUART3_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART4_IRQHandler
    .type LPUART4_IRQHandler, %function
LPUART4_IRQHandler:
    ldr   r0,=LPUART4_DriverIRQHandler
    bx    r0
    .size LPUART4_IRQHandler, . - LPUART4_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXSPI_IRQHandler
    .type FLEXSPI_IRQHandler, %function
FLEXSPI_IRQHandler:
    ldr   r0,=FLEXSPI_DriverIRQHandler
    bx    r0
    .size FLEXSPI_IRQHandler, . - FLEXSPI_IRQHandler

    .align 1
    .thumb_func
    .weak LPI2C1_IRQHandler
    .type LPI2C1_IRQHandler, %function
LPI2C1_IRQHandler:
    ldr   r0,=LPI2C1_DriverIRQHandler
    bx    r0
    .size LPI2C1_IRQHandler, . - LPI2C1_IRQHandler

    .align 1
    .thumb_func
    .weak LPI2C2_IRQHandler
    .type LPI2C2_IRQHandler, %function
LPI2C2_IRQHandler:
    ldr   r0,=LPI2C2_DriverIRQHandler
    bx    r0
    .size LPI2C2_IRQHandler, . - LPI2C2_IRQHandler

    .align 1
    .thumb_func
    .weak LPSPI1_IRQHandler
    .type LPSPI1_IRQHandler, %function
LPSPI1_IRQHandler:
    ldr   r0,=LPSPI1_DriverIRQHandler
    bx    r0
    .size LPSPI1_IRQHandler, . - LPSPI1_IRQHandler

    .align 1
    .thumb_func
    .weak LPSPI2_IRQHandler
    .type LPSPI2_IRQHandler, %function
LPSPI2_IRQHandler:
    ldr   r0,=LPSPI2_DriverIRQHandler
    bx    r0
    .size LPSPI2_IRQHandler, . - LPSPI2_IRQHandler

    .align 1
    .thumb_func
    .weak SAI1_IRQHandler
    .type SAI1_IRQHandler, %function
SAI1_IRQHandler:
    ldr   r0,=SAI1_DriverIRQHandler
    bx    r0
    .size SAI1_IRQHandler, . - SAI1_IRQHandler

    .align 1
    .thumb_func
    .weak SAI3_RX_IRQHandler
    .type SAI3_RX_IRQHandler, %function
SAI3_RX_IRQHandler:
    ldr   r0,=SAI3_RX_DriverIRQHandler
    bx    r0
    .size SAI3_RX_IRQHandler, . - SAI3_RX_IRQHandler

    .align 1
    .thumb_func
    .weak SAI3_TX_IRQHandler
    .type SAI3_TX_IRQHandler, %function
SAI3_TX_IRQHandler:
    ldr   r0,=SAI3_TX_DriverIRQHandler
    bx    r0
    .size SAI3_TX_IRQHandler, . - SAI3_TX_IRQHandler

    .align 1
    .thumb_func
    .weak SPDIF_IRQHandler
    .type SPDIF_IRQHandler, %function
SPDIF_IRQHandler:
    ldr   r0,=SPDIF_DriverIRQHandler
    bx    r0
    .size SPDIF_IRQHandler, . - SPDIF_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXIO1_IRQHandler
    .type FLEXIO1_IRQHandler, %function
FLEXIO1_IRQHandler:
    ldr   r0,=FLEXIO1_DriverIRQHandler
    bx    r0
    .size FLEXIO1_IRQHandler, . - FLEXIO1_IRQHandler


/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro def_irq_handler  handler_name
    .weak \handler_name
    .set  \handler_name, DefaultISR
    .endm

/* Exception Handlers */
    def_irq_handler    MemManage_Handler
    def_irq_handler    BusFault_Handler
    def_irq_handler    UsageFault_Handler
    def_irq_handler    DebugMon_Handler
    def_irq_handler    DMA0_DriverIRQHandler
    def_irq_handler    DMA1_DriverIRQHandler
    def_irq_handler    DMA2_DriverIRQHandler
    def_irq_handler    DMA3_DriverIRQHandler
    def_irq_handler    DMA4_DriverIRQHandler
    def_irq_handler    DMA5_DriverIRQHandler
    def_irq_handler    DMA6_DriverIRQHandler
    def_irq_handler    DMA7_DriverIRQHandler
    def_irq_handler    DMA8_DriverIRQHandler
    def_irq_handler    DMA9_DriverIRQHandler
    def_irq_handler    DMA10_DriverIRQHandler
    def_irq_handler    DMA11_DriverIRQHandler
    def_irq_handler    DMA12_DriverIRQHandler
    def_irq_handler    DMA13_DriverIRQHandler
    def_irq_handler    DMA14_DriverIRQHandler
    def_irq_handler    DMA15_DriverIRQHandler
    def_irq_handler    DMA_ERROR_DriverIRQHandler
    def_irq_handler    CTI0_ERROR_IRQHandler
    def_irq_handler    CTI1_ERROR_IRQHandler
    def_irq_handler    CORE_IRQHandler
    def_irq_handler    LPUART1_DriverIRQHandler
    def_irq_handler    LPUART2_DriverIRQHandler
    def_irq_handler    LPUART3_DriverIRQHandler
    def_irq_handler    LPUART4_DriverIRQHandler
    def_irq_handler    PIT_IRQHandler
    def_irq_handler    USB_OTG1_IRQHandler
    def_irq_handler    FLEXSPI_DriverIRQHandler
    def_irq_handler    FLEXRAM_IRQHandler
    def_irq_handler    LPI2C1_DriverIRQHandler
    def_irq_handler    LPI2C2_DriverIRQHandler
    def_irq_handler    GPT1_IRQHandler
    def_irq_handler    GPT2_IRQHandler
    def_irq_handler    LPSPI1_DriverIRQHandler
    def_irq_handler    LPSPI2_DriverIRQHandler
    def_irq_handler    PWM1_0_IRQHandler
    def_irq_handler    PWM1_1_IRQHandler
    def_irq_handler    PWM1_2_IRQHandler
    def_irq_handler    PWM1_3_IRQHandler
    def_irq_handler    PWM1_FAULT_IRQHandler
    def_irq_handler    KPP_IRQHandler
    def_irq_handler    SRC_IRQHandler
    def_irq_handler    GPR_IRQ_IRQHandler
    def_irq_handler    CCM_1_IRQHandler
    def_irq_handler    CCM_2_IRQHandler
    def_irq_handler    EWM_IRQHandler
    def_irq_handler    WDOG2_IRQHandler
    def_irq_handler    SNVS_HP_WRAPPER_IRQHandler
    def_irq_handler    SNVS_HP_WRAPPER_TZ_IRQHandler
    def_irq_handler    SNVS_LP_WRAPPER_IRQHandler
    def_irq_handler    CSU_IRQHandler
    def_irq_handler    DCP_IRQHandler
    def_irq_handler    DCP_VMI_IRQHandler
    def_irq_handler    Reserved68_IRQHandler
    def_irq_handler    TRNG_IRQHandler
    def_irq_handler    Reserved70_IRQHandler
    def_irq_handler    Reserved71_IRQHandler
    def_irq_handler    SAI1_DriverIRQHandler
    def_irq_handler    RTWDOG_IRQHandler
    def_irq_handler    SAI3_RX_DriverIRQHandler
    def_irq_handler    SAI3_TX_DriverIRQHandler
    def_irq_handler    SPDIF_DriverIRQHandler
    def_irq_handler    PMU_IRQHandler
    def_irq_handler    XBAR1_IRQ_0_1_2_3_IRQHandler
    def_irq_handler    TEMP_LOW_HIGH_IRQHandler
    def_irq_handler    TEMP_PANIC_IRQHandler
    def_irq_handler    USB_PHY_IRQHandler
    def_irq_handler    GPC_IRQHandler
    def_irq_handler    ADC1_IRQHandler
    def_irq_handler    FLEXIO1_DriverIRQHandler
    def_irq_handler    DCDC_IRQHandler
    def_irq_handler    GPIO1_Combined_0_15_IRQHandler
    def_irq_handler    GPIO1_Combined_16_31_IRQHandler
    def_irq_handler    GPIO2_Combined_0_15_IRQHandler
    def_irq_handler    GPIO5_Combined_0_15_IRQHandler
    def_irq_handler    WDOG1_IRQHandler
    def_irq_handler    ADC_ETC_IRQ0_IRQHandler
    def_irq_handler    ADC_ETC_IRQ1_IRQHandler
    def_irq_handler    ADC_ETC_IRQ2_IRQHandler
    def_irq_handler    ADC_ETC_IRQ3_IRQHandler
    def_irq_handler    ADC_ETC_ERROR_IRQ_IRQHandler

    .end
