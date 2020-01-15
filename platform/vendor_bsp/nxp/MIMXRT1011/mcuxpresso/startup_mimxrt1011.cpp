//*****************************************************************************
// MIMXRT1011 startup code for use with MCUXpresso IDE
//
// Version : 120419
//*****************************************************************************
//
// Copyright 2016-2019 NXP
// All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause
//*****************************************************************************

#if defined (DEBUG)
#pragma GCC push_options
#pragma GCC optimize ("Og")
#endif // (DEBUG)

#if defined (__cplusplus)
#ifdef __REDLIB__
#error Redlib does not support C++
#else
//*****************************************************************************
//
// The entry point for the C++ library startup
//
//*****************************************************************************
extern "C" {
    extern void __libc_init_array(void);
}
#endif
#endif

#define WEAK __attribute__ ((weak))
#define WEAK_AV __attribute__ ((weak, section(".after_vectors")))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))

//*****************************************************************************
#if defined (__cplusplus)
extern "C" {
#endif

//*****************************************************************************
// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
//*****************************************************************************
//*****************************************************************************
// Declaration of external SystemInit function
//*****************************************************************************
#if defined (__USE_CMSIS)
extern void SystemInit(void);
#endif // (__USE_CMSIS)

//*****************************************************************************
// Forward declaration of the core exception handlers.
// When the application defines a handler (with the same name), this will
// automatically take precedence over these weak definitions.
// If your application is a C++ one, then any interrupt handlers defined
// in C++ files within in your main application will need to have C linkage
// rather than C++ linkage. To do this, make sure that you are using extern "C"
// { .... } around the interrupt handler within your main application code.
//*****************************************************************************
     void ResetISR(void);
WEAK void NMI_Handler(void);
WEAK void HardFault_Handler(void);
WEAK void SVC_Handler(void);
WEAK void PendSV_Handler(void);
WEAK void SysTick_Handler(void);
WEAK void IntDefaultHandler(void);

//*****************************************************************************
// Forward declaration of the application IRQ handlers. When the application
// defines a handler (with the same name), this will automatically take
// precedence over weak definitions below
//*****************************************************************************
WEAK void DMA0_IRQHandler(void);
WEAK void DMA1_IRQHandler(void);
WEAK void DMA2_IRQHandler(void);
WEAK void DMA3_IRQHandler(void);
WEAK void DMA4_IRQHandler(void);
WEAK void DMA5_IRQHandler(void);
WEAK void DMA6_IRQHandler(void);
WEAK void DMA7_IRQHandler(void);
WEAK void DMA8_IRQHandler(void);
WEAK void DMA9_IRQHandler(void);
WEAK void DMA10_IRQHandler(void);
WEAK void DMA11_IRQHandler(void);
WEAK void DMA12_IRQHandler(void);
WEAK void DMA13_IRQHandler(void);
WEAK void DMA14_IRQHandler(void);
WEAK void DMA15_IRQHandler(void);
WEAK void DMA_ERROR_IRQHandler(void);
WEAK void CTI0_ERROR_IRQHandler(void);
WEAK void CTI1_ERROR_IRQHandler(void);
WEAK void CORE_IRQHandler(void);
WEAK void LPUART1_IRQHandler(void);
WEAK void LPUART2_IRQHandler(void);
WEAK void LPUART3_IRQHandler(void);
WEAK void LPUART4_IRQHandler(void);
WEAK void PIT_IRQHandler(void);
WEAK void USB_OTG1_IRQHandler(void);
WEAK void FLEXSPI_IRQHandler(void);
WEAK void FLEXRAM_IRQHandler(void);
WEAK void LPI2C1_IRQHandler(void);
WEAK void LPI2C2_IRQHandler(void);
WEAK void GPT1_IRQHandler(void);
WEAK void GPT2_IRQHandler(void);
WEAK void LPSPI1_IRQHandler(void);
WEAK void LPSPI2_IRQHandler(void);
WEAK void PWM1_0_IRQHandler(void);
WEAK void PWM1_1_IRQHandler(void);
WEAK void PWM1_2_IRQHandler(void);
WEAK void PWM1_3_IRQHandler(void);
WEAK void PWM1_FAULT_IRQHandler(void);
WEAK void KPP_IRQHandler(void);
WEAK void SRC_IRQHandler(void);
WEAK void GPR_IRQ_IRQHandler(void);
WEAK void CCM_1_IRQHandler(void);
WEAK void CCM_2_IRQHandler(void);
WEAK void EWM_IRQHandler(void);
WEAK void WDOG2_IRQHandler(void);
WEAK void SNVS_HP_WRAPPER_IRQHandler(void);
WEAK void SNVS_HP_WRAPPER_TZ_IRQHandler(void);
WEAK void SNVS_LP_WRAPPER_IRQHandler(void);
WEAK void CSU_IRQHandler(void);
WEAK void DCP_IRQHandler(void);
WEAK void DCP_VMI_IRQHandler(void);
WEAK void Reserved68_IRQHandler(void);
WEAK void TRNG_IRQHandler(void);
WEAK void Reserved70_IRQHandler(void);
WEAK void Reserved71_IRQHandler(void);
WEAK void SAI1_IRQHandler(void);
WEAK void RTWDOG_IRQHandler(void);
WEAK void SAI3_RX_IRQHandler(void);
WEAK void SAI3_TX_IRQHandler(void);
WEAK void SPDIF_IRQHandler(void);
WEAK void PMU_IRQHandler(void);
WEAK void XBAR1_IRQ_0_1_2_3_IRQHandler(void);
WEAK void TEMP_LOW_HIGH_IRQHandler(void);
WEAK void TEMP_PANIC_IRQHandler(void);
WEAK void USB_PHY_IRQHandler(void);
WEAK void GPC_IRQHandler(void);
WEAK void ADC1_IRQHandler(void);
WEAK void FLEXIO1_IRQHandler(void);
WEAK void DCDC_IRQHandler(void);
WEAK void GPIO1_Combined_0_15_IRQHandler(void);
WEAK void GPIO1_Combined_16_31_IRQHandler(void);
WEAK void GPIO2_Combined_0_15_IRQHandler(void);
WEAK void GPIO5_Combined_0_15_IRQHandler(void);
WEAK void WDOG1_IRQHandler(void);
WEAK void ADC_ETC_IRQ0_IRQHandler(void);
WEAK void ADC_ETC_IRQ1_IRQHandler(void);
WEAK void ADC_ETC_IRQ2_IRQHandler(void);
WEAK void ADC_ETC_IRQ3_IRQHandler(void);
WEAK void ADC_ETC_ERROR_IRQ_IRQHandler(void);

//*****************************************************************************
// Forward declaration of the driver IRQ handlers. These are aliased
// to the IntDefaultHandler, which is a 'forever' loop. When the driver
// defines a handler (with the same name), this will automatically take
// precedence over these weak definitions
//*****************************************************************************
void DMA0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMA1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMA2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMA3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMA4_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMA5_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMA6_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMA7_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMA8_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMA9_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMA10_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMA11_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMA12_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMA13_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMA14_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMA15_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DMA_ERROR_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CTI0_ERROR_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CTI1_ERROR_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CORE_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void LPUART1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void LPUART2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void LPUART3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void LPUART4_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PIT_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void USB_OTG1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXSPI_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXRAM_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void LPI2C1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void LPI2C2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void GPT1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void GPT2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void LPSPI1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void LPSPI2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PWM1_0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PWM1_1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PWM1_2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PWM1_3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PWM1_FAULT_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void KPP_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void SRC_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void GPR_IRQ_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CCM_1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CCM_2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void EWM_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void WDOG2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void SNVS_HP_WRAPPER_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void SNVS_HP_WRAPPER_TZ_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void SNVS_LP_WRAPPER_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void CSU_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DCP_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DCP_VMI_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void Reserved68_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void TRNG_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void Reserved70_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void Reserved71_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void SAI1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void RTWDOG_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void SAI3_RX_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void SAI3_TX_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void SPDIF_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void PMU_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void XBAR1_IRQ_0_1_2_3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void TEMP_LOW_HIGH_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void TEMP_PANIC_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void USB_PHY_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void GPC_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void ADC1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void FLEXIO1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void DCDC_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void GPIO1_Combined_0_15_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void GPIO1_Combined_16_31_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void GPIO2_Combined_0_15_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void GPIO5_Combined_0_15_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void WDOG1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void ADC_ETC_IRQ0_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void ADC_ETC_IRQ1_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void ADC_ETC_IRQ2_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void ADC_ETC_IRQ3_DriverIRQHandler(void) ALIAS(IntDefaultHandler);
void ADC_ETC_ERROR_IRQ_DriverIRQHandler(void) ALIAS(IntDefaultHandler);

//*****************************************************************************
// The entry point for the application.
// __main() is the entry point for Redlib based applications
// main() is the entry point for Newlib based applications
//*****************************************************************************
#if defined (__REDLIB__)
extern void __main(void);
#endif
extern int main(void);

//*****************************************************************************
// External declaration for the pointer to the stack top from the Linker Script
//*****************************************************************************
extern void _vStackTop(void);
//*****************************************************************************
#if defined (__cplusplus)
} // extern "C"
#endif
//*****************************************************************************
// The vector table.
// This relies on the linker script to place at correct location in memory.
//*****************************************************************************
extern void (* const g_pfnVectors[])(void);
extern void * __Vectors __attribute__ ((alias ("g_pfnVectors")));

__attribute__ ((used, section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
    // Core Level - CM7
    &_vStackTop,                       // The initial stack pointer
    ResetISR,                          // The reset handler
    NMI_Handler,                       // The NMI handler
    HardFault_Handler,                 // The hard fault handler
    0,                                 // Reserved
    0,                                 // Reserved
    0,                                 // Reserved
    0,                                 // Reserved
    0,                                 // Reserved
    0,                                 // Reserved
    0,                                 // Reserved
    SVC_Handler,                       // SVCall handler
    0,                                 // Reserved
    0,                                 // Reserved
    PendSV_Handler,                    // The PendSV handler
    SysTick_Handler,                   // The SysTick handler

    // Chip Level - MIMXRT1011
    DMA0_IRQHandler,                  // 16: DMA channel 0 transfer complete
    DMA1_IRQHandler,                  // 17: DMA channel 1 transfer complete
    DMA2_IRQHandler,                  // 18: DMA channel 2 transfer complete
    DMA3_IRQHandler,                  // 19: DMA channel 3 transfer complete
    DMA4_IRQHandler,                  // 20: DMA channel 4 transfer complete
    DMA5_IRQHandler,                  // 21: DMA channel 5 transfer complete
    DMA6_IRQHandler,                  // 22: DMA channel 6 transfer complete
    DMA7_IRQHandler,                  // 23: DMA channel 7 transfer complete
    DMA8_IRQHandler,                  // 24: DMA channel 8 transfer complete
    DMA9_IRQHandler,                  // 25: DMA channel 9 transfer complete
    DMA10_IRQHandler,                 // 26: DMA channel 10 transfer complete
    DMA11_IRQHandler,                 // 27: DMA channel 11 transfer complete
    DMA12_IRQHandler,                 // 28: DMA channel 12 transfer complete
    DMA13_IRQHandler,                 // 29: DMA channel 13 transfer complete
    DMA14_IRQHandler,                 // 30: DMA channel 14 transfer complete
    DMA15_IRQHandler,                 // 31: DMA channel 15 transfer complete
    DMA_ERROR_IRQHandler,             // 32: DMA error interrupt channels 0-15
    CTI0_ERROR_IRQHandler,            // 33: CTI trigger outputs
    CTI1_ERROR_IRQHandler,            // 34: CTI trigger outputs
    CORE_IRQHandler,                  // 35: CorePlatform exception IRQ
    LPUART1_IRQHandler,               // 36: LPUART1 TX interrupt and RX interrupt
    LPUART2_IRQHandler,               // 37: LPUART2 TX interrupt and RX interrupt
    LPUART3_IRQHandler,               // 38: LPUART3 TX interrupt and RX interrupt
    LPUART4_IRQHandler,               // 39: LPUART4 TX interrupt and RX interrupt
    PIT_IRQHandler,                   // 40: PIT interrupt
    USB_OTG1_IRQHandler,              // 41: USBO2 USB OTG1
    FLEXSPI_IRQHandler,               // 42: FlexSPI0 interrupt
    FLEXRAM_IRQHandler,               // 43: FlexRAM address out of range Or access hit IRQ
    LPI2C1_IRQHandler,                // 44: LPI2C1 interrupt
    LPI2C2_IRQHandler,                // 45: LPI2C2 interrupt
    GPT1_IRQHandler,                  // 46: GPT1 interrupt
    GPT2_IRQHandler,                  // 47: GPT2 interrupt
    LPSPI1_IRQHandler,                // 48: LPSPI1 single interrupt vector for all sources
    LPSPI2_IRQHandler,                // 49: LPSPI2 single interrupt vector for all sources
    PWM1_0_IRQHandler,                // 50: PWM1 capture 0, compare 0, or reload 0 interrupt
    PWM1_1_IRQHandler,                // 51: PWM1 capture 1, compare 1, or reload 0 interrupt
    PWM1_2_IRQHandler,                // 52: PWM1 capture 2, compare 2, or reload 0 interrupt
    PWM1_3_IRQHandler,                // 53: PWM1 capture 3, compare 3, or reload 0 interrupt
    PWM1_FAULT_IRQHandler,            // 54: PWM1 fault or reload error interrupt
    KPP_IRQHandler,                   // 55: Keypad nterrupt
    SRC_IRQHandler,                   // 56: SRC interrupt
    GPR_IRQ_IRQHandler,               // 57: Used to notify cores on exception condition while boot
    CCM_1_IRQHandler,                 // 58: CCM IRQ1 interrupt
    CCM_2_IRQHandler,                 // 59: CCM IRQ2 interrupt
    EWM_IRQHandler,                   // 60: EWM interrupt
    WDOG2_IRQHandler,                 // 61: WDOG2 interrupt
    SNVS_HP_WRAPPER_IRQHandler,       // 62: SNVS Functional Interrupt
    SNVS_HP_WRAPPER_TZ_IRQHandler,    // 63: SNVS Security Interrupt
    SNVS_LP_WRAPPER_IRQHandler,       // 64: ON-OFF button press shorter than 5 secs (pulse event)
    CSU_IRQHandler,                   // 65: CSU interrupt
    DCP_IRQHandler,                   // 66: Combined DCP channel interrupts(except channel 0) and CRC interrupt
    DCP_VMI_IRQHandler,               // 67: IRQ of DCP channel 0
    Reserved68_IRQHandler,            // 68: Reserved interrupt
    TRNG_IRQHandler,                  // 69: TRNG interrupt
    Reserved70_IRQHandler,            // 70: Reserved interrupt
    Reserved71_IRQHandler,            // 71: Reserved interrupt
    SAI1_IRQHandler,                  // 72: SAI1 interrupt
    RTWDOG_IRQHandler,                // 73: RTWDOG interrupt
    SAI3_RX_IRQHandler,               // 74: SAI3 interrupt
    SAI3_TX_IRQHandler,               // 75: SAI3 interrupt
    SPDIF_IRQHandler,                 // 76: SPDIF interrupt
    PMU_IRQHandler,                   // 77: PMU interrupt
    XBAR1_IRQ_0_1_2_3_IRQHandler,     // 78: XBAR1 interrupt
    TEMP_LOW_HIGH_IRQHandler,         // 79: TEMPMON interrupt
    TEMP_PANIC_IRQHandler,            // 80: TEMPMON interrupt
    USB_PHY_IRQHandler,               // 81: USBPHY (OTG1 UTMI), Interrupt
    GPC_IRQHandler,                   // 82: GPC interrupt
    ADC1_IRQHandler,                  // 83: ADC1 interrupt
    FLEXIO1_IRQHandler,               // 84: FLEXIO1 interrupt
    DCDC_IRQHandler,                  // 85: DCDC interrupt
    GPIO1_Combined_0_15_IRQHandler,   // 86: Combined interrupt indication for GPIO1 signal 0 throughout 15
    GPIO1_Combined_16_31_IRQHandler,  // 87: Combined interrupt indication for GPIO1 signal 16 throughout 31
    GPIO2_Combined_0_15_IRQHandler,   // 88: Combined interrupt indication for GPIO2 signal 0 throughout 15
    GPIO5_Combined_0_15_IRQHandler,   // 89: Combined interrupt indication for GPIO5 signal 0 throughout 15
    WDOG1_IRQHandler,                 // 90: WDOG1 interrupt
    ADC_ETC_IRQ0_IRQHandler,          // 91: ADCETC IRQ0 interrupt
    ADC_ETC_IRQ1_IRQHandler,          // 92: ADCETC IRQ1 interrupt
    ADC_ETC_IRQ2_IRQHandler,          // 93: ADCETC IRQ2 interrupt
    ADC_ETC_IRQ3_IRQHandler,          // 94: ADCETC IRQ3 interrupt
    ADC_ETC_ERROR_IRQ_IRQHandler,     // 95: ADCETC Error IRQ interrupt

}; /* End of g_pfnVectors */

//*****************************************************************************
// Functions to carry out the initialization of RW and BSS data sections. These
// are written as separate functions rather than being inlined within the
// ResetISR() function in order to cope with MCUs with multiple banks of
// memory.
//*****************************************************************************
__attribute__ ((section(".after_vectors.init_data")))
void data_init(unsigned int romstart, unsigned int start, unsigned int len) {
    unsigned int *pulDest = (unsigned int*) start;
    unsigned int *pulSrc = (unsigned int*) romstart;
    unsigned int loop;
    for (loop = 0; loop < len; loop = loop + 4)
        *pulDest++ = *pulSrc++;
}

__attribute__ ((section(".after_vectors.init_bss")))
void bss_init(unsigned int start, unsigned int len) {
    unsigned int *pulDest = (unsigned int*) start;
    unsigned int loop;
    for (loop = 0; loop < len; loop = loop + 4)
        *pulDest++ = 0;
}

//*****************************************************************************
// The following symbols are constructs generated by the linker, indicating
// the location of various points in the "Global Section Table". This table is
// created by the linker via the Code Red managed linker script mechanism. It
// contains the load address, execution address and length of each RW data
// section and the execution and length of each BSS (zero initialized) section.
//*****************************************************************************
extern unsigned int __data_section_table;
extern unsigned int __data_section_table_end;
extern unsigned int __bss_section_table;
extern unsigned int __bss_section_table_end;

//*****************************************************************************
// Reset entry point for your code.
// Sets up a simple runtime environment and initializes the C/C++
// library.
//*****************************************************************************
__attribute__ ((section(".after_vectors.reset")))
void ResetISR(void) {

    // Disable interrupts
    __asm volatile ("cpsid i");


#if defined (__USE_CMSIS)
// If __USE_CMSIS defined, then call CMSIS SystemInit code
    SystemInit();
#else
    // Disable Watchdog
    volatile unsigned int *WDOG1_WCR = (unsigned int *) 0x400B8000;
    *WDOG1_WCR = *WDOG1_WCR & ~(1 << 2);
    volatile unsigned int *WDOG2_WCR = (unsigned int *) 0x400D0000;
    *WDOG2_WCR = *WDOG2_WCR & ~(1 << 2);
    // Write watchdog update key to unlock
    *((volatile unsigned int *)0x400BC004) = 0xD928C520;
    // Set timeout value
    *((volatile unsigned int *)0x400BC008) = 0xFFFF;
    // Now disable watchdog via control register
    volatile unsigned int *RTWDOG_CS = (unsigned int *) 0x400BC000;
    *RTWDOG_CS = (*RTWDOG_CS & ~(1 << 7)) | (1 << 5);

#endif // (__USE_CMSIS)

    //
    // Copy the data sections from flash to SRAM.
    //
    unsigned int LoadAddr, ExeAddr, SectionLen;
    unsigned int *SectionTableAddr;

    // Load base address of Global Section Table
    SectionTableAddr = &__data_section_table;

    // Copy the data sections from flash to SRAM.
    while (SectionTableAddr < &__data_section_table_end) {
        LoadAddr = *SectionTableAddr++;
        ExeAddr = *SectionTableAddr++;
        SectionLen = *SectionTableAddr++;
        data_init(LoadAddr, ExeAddr, SectionLen);
    }

    // At this point, SectionTableAddr = &__bss_section_table;
    // Zero fill the bss segment
    while (SectionTableAddr < &__bss_section_table_end) {
        ExeAddr = *SectionTableAddr++;
        SectionLen = *SectionTableAddr++;
        bss_init(ExeAddr, SectionLen);
    }


#if !defined (__USE_CMSIS)
// Assume that if __USE_CMSIS defined, then CMSIS SystemInit code
// will setup the VTOR register

    // Check to see if we are running the code from a non-zero
    // address (eg RAM, external flash), in which case we need
    // to modify the VTOR register to tell the CPU that the
    // vector table is located at a non-0x0 address.
    unsigned int * pSCB_VTOR = (unsigned int *) 0xE000ED08;
    if ((unsigned int *)g_pfnVectors!=(unsigned int *) 0x00000000) {
        *pSCB_VTOR = (unsigned int)g_pfnVectors;
    }
#endif // (__USE_CMSIS)
#if defined (__cplusplus)
    //
    // Call C++ library initialisation
    //
    __libc_init_array();
#endif

    // Reenable interrupts
    __asm volatile ("cpsie i");

#if defined (__REDLIB__)
    // Call the Redlib library, which in turn calls main()
    __main();
#else
    main();
#endif

    //
    // main() shouldn't return, but if it does, we'll just enter an infinite loop
    //
    while (1) {
        ;
    }
}

//*****************************************************************************
// Default core exception handlers. Override the ones here by defining your own
// handler routines in your application code.
//*****************************************************************************
WEAK_AV void NMI_Handler(void)
{ while(1) {}
}

WEAK_AV void HardFault_Handler(void)
{ while(1) {}
}

WEAK_AV void SVC_Handler(void)
{ while(1) {}
}

WEAK_AV void PendSV_Handler(void)
{ while(1) {}
}

WEAK_AV void SysTick_Handler(void)
{ while(1) {}
}

//*****************************************************************************
// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
//*****************************************************************************
WEAK_AV void IntDefaultHandler(void)
{ while(1) {}
}

//*****************************************************************************
// Default application exception handlers. Override the ones here by defining
// your own handler routines in your application code. These routines call
// driver exception handlers or IntDefaultHandler() if no driver exception
// handler is included.
//*****************************************************************************
WEAK void DMA0_IRQHandler(void)
{   DMA0_DriverIRQHandler();
}

WEAK void DMA1_IRQHandler(void)
{   DMA1_DriverIRQHandler();
}

WEAK void DMA2_IRQHandler(void)
{   DMA2_DriverIRQHandler();
}

WEAK void DMA3_IRQHandler(void)
{   DMA3_DriverIRQHandler();
}

WEAK void DMA4_IRQHandler(void)
{   DMA4_DriverIRQHandler();
}

WEAK void DMA5_IRQHandler(void)
{   DMA5_DriverIRQHandler();
}

WEAK void DMA6_IRQHandler(void)
{   DMA6_DriverIRQHandler();
}

WEAK void DMA7_IRQHandler(void)
{   DMA7_DriverIRQHandler();
}

WEAK void DMA8_IRQHandler(void)
{   DMA8_DriverIRQHandler();
}

WEAK void DMA9_IRQHandler(void)
{   DMA9_DriverIRQHandler();
}

WEAK void DMA10_IRQHandler(void)
{   DMA10_DriverIRQHandler();
}

WEAK void DMA11_IRQHandler(void)
{   DMA11_DriverIRQHandler();
}

WEAK void DMA12_IRQHandler(void)
{   DMA12_DriverIRQHandler();
}

WEAK void DMA13_IRQHandler(void)
{   DMA13_DriverIRQHandler();
}

WEAK void DMA14_IRQHandler(void)
{   DMA14_DriverIRQHandler();
}

WEAK void DMA15_IRQHandler(void)
{   DMA15_DriverIRQHandler();
}

WEAK void DMA_ERROR_IRQHandler(void)
{   DMA_ERROR_DriverIRQHandler();
}

WEAK void CTI0_ERROR_IRQHandler(void)
{   CTI0_ERROR_DriverIRQHandler();
}

WEAK void CTI1_ERROR_IRQHandler(void)
{   CTI1_ERROR_DriverIRQHandler();
}

WEAK void CORE_IRQHandler(void)
{   CORE_DriverIRQHandler();
}

WEAK void LPUART1_IRQHandler(void)
{   LPUART1_DriverIRQHandler();
}

WEAK void LPUART2_IRQHandler(void)
{   LPUART2_DriverIRQHandler();
}

WEAK void LPUART3_IRQHandler(void)
{   LPUART3_DriverIRQHandler();
}

WEAK void LPUART4_IRQHandler(void)
{   LPUART4_DriverIRQHandler();
}

WEAK void PIT_IRQHandler(void)
{   PIT_DriverIRQHandler();
}

WEAK void USB_OTG1_IRQHandler(void)
{   USB_OTG1_DriverIRQHandler();
}

WEAK void FLEXSPI_IRQHandler(void)
{   FLEXSPI_DriverIRQHandler();
}

WEAK void FLEXRAM_IRQHandler(void)
{   FLEXRAM_DriverIRQHandler();
}

WEAK void LPI2C1_IRQHandler(void)
{   LPI2C1_DriverIRQHandler();
}

WEAK void LPI2C2_IRQHandler(void)
{   LPI2C2_DriverIRQHandler();
}

WEAK void GPT1_IRQHandler(void)
{   GPT1_DriverIRQHandler();
}

WEAK void GPT2_IRQHandler(void)
{   GPT2_DriverIRQHandler();
}

WEAK void LPSPI1_IRQHandler(void)
{   LPSPI1_DriverIRQHandler();
}

WEAK void LPSPI2_IRQHandler(void)
{   LPSPI2_DriverIRQHandler();
}

WEAK void PWM1_0_IRQHandler(void)
{   PWM1_0_DriverIRQHandler();
}

WEAK void PWM1_1_IRQHandler(void)
{   PWM1_1_DriverIRQHandler();
}

WEAK void PWM1_2_IRQHandler(void)
{   PWM1_2_DriverIRQHandler();
}

WEAK void PWM1_3_IRQHandler(void)
{   PWM1_3_DriverIRQHandler();
}

WEAK void PWM1_FAULT_IRQHandler(void)
{   PWM1_FAULT_DriverIRQHandler();
}

WEAK void KPP_IRQHandler(void)
{   KPP_DriverIRQHandler();
}

WEAK void SRC_IRQHandler(void)
{   SRC_DriverIRQHandler();
}

WEAK void GPR_IRQ_IRQHandler(void)
{   GPR_IRQ_DriverIRQHandler();
}

WEAK void CCM_1_IRQHandler(void)
{   CCM_1_DriverIRQHandler();
}

WEAK void CCM_2_IRQHandler(void)
{   CCM_2_DriverIRQHandler();
}

WEAK void EWM_IRQHandler(void)
{   EWM_DriverIRQHandler();
}

WEAK void WDOG2_IRQHandler(void)
{   WDOG2_DriverIRQHandler();
}

WEAK void SNVS_HP_WRAPPER_IRQHandler(void)
{   SNVS_HP_WRAPPER_DriverIRQHandler();
}

WEAK void SNVS_HP_WRAPPER_TZ_IRQHandler(void)
{   SNVS_HP_WRAPPER_TZ_DriverIRQHandler();
}

WEAK void SNVS_LP_WRAPPER_IRQHandler(void)
{   SNVS_LP_WRAPPER_DriverIRQHandler();
}

WEAK void CSU_IRQHandler(void)
{   CSU_DriverIRQHandler();
}

WEAK void DCP_IRQHandler(void)
{   DCP_DriverIRQHandler();
}

WEAK void DCP_VMI_IRQHandler(void)
{   DCP_VMI_DriverIRQHandler();
}

WEAK void Reserved68_IRQHandler(void)
{   Reserved68_DriverIRQHandler();
}

WEAK void TRNG_IRQHandler(void)
{   TRNG_DriverIRQHandler();
}

WEAK void Reserved70_IRQHandler(void)
{   Reserved70_DriverIRQHandler();
}

WEAK void Reserved71_IRQHandler(void)
{   Reserved71_DriverIRQHandler();
}

WEAK void SAI1_IRQHandler(void)
{   SAI1_DriverIRQHandler();
}

WEAK void RTWDOG_IRQHandler(void)
{   RTWDOG_DriverIRQHandler();
}

WEAK void SAI3_RX_IRQHandler(void)
{   SAI3_RX_DriverIRQHandler();
}

WEAK void SAI3_TX_IRQHandler(void)
{   SAI3_TX_DriverIRQHandler();
}

WEAK void SPDIF_IRQHandler(void)
{   SPDIF_DriverIRQHandler();
}

WEAK void PMU_IRQHandler(void)
{   PMU_DriverIRQHandler();
}

WEAK void XBAR1_IRQ_0_1_2_3_IRQHandler(void)
{   XBAR1_IRQ_0_1_2_3_DriverIRQHandler();
}

WEAK void TEMP_LOW_HIGH_IRQHandler(void)
{   TEMP_LOW_HIGH_DriverIRQHandler();
}

WEAK void TEMP_PANIC_IRQHandler(void)
{   TEMP_PANIC_DriverIRQHandler();
}

WEAK void USB_PHY_IRQHandler(void)
{   USB_PHY_DriverIRQHandler();
}

WEAK void GPC_IRQHandler(void)
{   GPC_DriverIRQHandler();
}

WEAK void ADC1_IRQHandler(void)
{   ADC1_DriverIRQHandler();
}

WEAK void FLEXIO1_IRQHandler(void)
{   FLEXIO1_DriverIRQHandler();
}

WEAK void DCDC_IRQHandler(void)
{   DCDC_DriverIRQHandler();
}

WEAK void GPIO1_Combined_0_15_IRQHandler(void)
{   GPIO1_Combined_0_15_DriverIRQHandler();
}

WEAK void GPIO1_Combined_16_31_IRQHandler(void)
{   GPIO1_Combined_16_31_DriverIRQHandler();
}

WEAK void GPIO2_Combined_0_15_IRQHandler(void)
{   GPIO2_Combined_0_15_DriverIRQHandler();
}

WEAK void GPIO5_Combined_0_15_IRQHandler(void)
{   GPIO5_Combined_0_15_DriverIRQHandler();
}

WEAK void WDOG1_IRQHandler(void)
{   WDOG1_DriverIRQHandler();
}

WEAK void ADC_ETC_IRQ0_IRQHandler(void)
{   ADC_ETC_IRQ0_DriverIRQHandler();
}

WEAK void ADC_ETC_IRQ1_IRQHandler(void)
{   ADC_ETC_IRQ1_DriverIRQHandler();
}

WEAK void ADC_ETC_IRQ2_IRQHandler(void)
{   ADC_ETC_IRQ2_DriverIRQHandler();
}

WEAK void ADC_ETC_IRQ3_IRQHandler(void)
{   ADC_ETC_IRQ3_DriverIRQHandler();
}

WEAK void ADC_ETC_ERROR_IRQ_IRQHandler(void)
{   ADC_ETC_ERROR_IRQ_DriverIRQHandler();
}

//*****************************************************************************

#if defined (DEBUG)
#pragma GCC pop_options
#endif // (DEBUG)
