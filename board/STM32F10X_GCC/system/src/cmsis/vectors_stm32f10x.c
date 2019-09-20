/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include "cortexm/ExceptionHandlers.h"

// ----------------------------------------------------------------------------

void __attribute__((weak))
Default_Handler(void);

// Forward declaration of the specific IRQ handlers. These are aliased
// to the Default_Handler, which is a 'forever' loop. When the application
// defines a handler (with the same name), this will automatically take
// precedence over these weak definitions

void __attribute__ ((weak, alias ("Default_Handler")))
WWDG_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
PVD_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TAMPER_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
RTC_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
FLASH_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
RCC_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
EXTI0_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
EXTI1_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
EXTI2_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
EXTI3_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
EXTI4_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
DMA1_Channel1_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
DMA1_Channel2_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
DMA1_Channel3_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
DMA1_Channel4_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
DMA1_Channel5_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
DMA1_Channel6_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
DMA1_Channel7_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
ADC1_2_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
USB_HP_CAN1_TX_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
USB_LP_CAN1_RX0_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
CAN1_RX1_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
CAN1_SCE_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
EXTI9_5_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM1_BRK_TIM9_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM1_UP_TIM10_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM1_TRG_COM_TIM11_IRQHandler(void);

void __attribute__ ((weak, alias ("Default_Handler")))
TIM1_BRK_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM1_UP_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM1_TRG_COM_IRQHandler(void);

void __attribute__ ((weak, alias ("Default_Handler")))
TIM1_CC_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM2_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM3_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM4_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
I2C1_EV_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
I2C1_ER_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
I2C2_EV_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
I2C2_ER_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
SPI1_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
SPI2_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
USART1_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
USART2_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
USART3_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
EXTI15_10_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
RTCAlarm_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
USBWakeUp_IRQHandler(void);

void __attribute__ ((weak, alias ("Default_Handler")))
TIM8_BRK_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM8_UP_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM8_TRG_COM_IRQHandler(void);

void __attribute__ ((weak, alias ("Default_Handler")))
TIM8_BRK_TIM12_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM8_UP_TIM13_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM8_TRG_COM_TIM14_IRQHandler(void);

void __attribute__ ((weak, alias ("Default_Handler")))
TIM8_CC_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
ADC3_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
FSMC_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
SDIO_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM5_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
SPI3_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
UART4_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
UART5_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM6_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM7_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
DMA2_Channel1_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
DMA2_Channel2_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
DMA2_Channel3_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
DMA2_Channel4_5_IRQHandler(void);

void __attribute__ ((weak, alias ("Default_Handler")))
CAN1_TX_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
CAN1_RX0_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
OTG_FS_WKUP_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
DMA2_Channel4_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
DMA2_Channel5_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
ETH_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
ETH_WKUP_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
CAN2_TX_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
CAN2_RX0_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
CAN2_RX1_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
CAN2_SCE_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
OTG_FS_IRQHandler(void);

void __attribute__ ((weak, alias ("Default_Handler")))
TIM1_BRK_TIM15_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM1_UP_TIM16_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM1_TRG_COM_TIM17_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
CEC_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM6_DAC_IRQHandler(void);

void __attribute__ ((weak, alias ("Default_Handler")))
TIM12_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM13_IRQHandler(void);
void __attribute__ ((weak, alias ("Default_Handler")))
TIM14_IRQHandler(void);

// ----------------------------------------------------------------------------

extern unsigned int _estack;

typedef void
(* const pHandler)(void);

// ----------------------------------------------------------------------------

// The vector table.
// This relies on the linker script to place at correct location in memory.

__attribute__ ((section(".isr_vector"),used))
pHandler __isr_vectors[] =
  {
  // Core Level - CM3
      (pHandler) &_estack, // The initial stack pointer
      Reset_Handler, // The reset handler

      NMI_Handler, // The NMI handler
      HardFault_Handler, // The hard fault handler
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
      MemManage_Handler,                        // The MPU fault handler
      BusFault_Handler,                        // The bus fault handler
      UsageFault_Handler,                        // The usage fault handler
#else
      0, 0, 0,                                  // Reserved
#endif
      0,                                        // Reserved
      0,                                        // Reserved
      0,                                        // Reserved
      0,                                        // Reserved
      SVC_Handler,                              // SVCall handler
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
      DebugMon_Handler,                         // Debug monitor handler
#else
      0,                                        // Reserved
#endif
      0, // Reserved
      PendSV_Handler, // The PendSV handler
      SysTick_Handler, // The SysTick handler

      // ----------------------------------------------------------------------
      // Chip Level - STM32F10x
      WWDG_IRQHandler, // Window WatchDog
      PVD_IRQHandler, // PVD through EXTI Line detection
      TAMPER_IRQHandler, // Tamper through the EXTI line
      RTC_IRQHandler, // RTC Wakeup through the EXTI line
      FLASH_IRQHandler, // FLASH
      RCC_IRQHandler, // RCC
      EXTI0_IRQHandler, // EXTI Line0
      EXTI1_IRQHandler, // EXTI Line1
      EXTI2_IRQHandler, // EXTI Line2
      EXTI3_IRQHandler, // EXTI Line3
      EXTI4_IRQHandler, // EXTI Line4
      DMA1_Channel1_IRQHandler, // DMA1 Channel 1
      DMA1_Channel2_IRQHandler, // DMA1 Channel 2
      DMA1_Channel3_IRQHandler, // DMA1 Channel 3
      DMA1_Channel4_IRQHandler, // DMA1 Channel 4
      DMA1_Channel5_IRQHandler, // DMA1 Channel 5
      DMA1_Channel6_IRQHandler, // DMA1 Channel 6
      DMA1_Channel7_IRQHandler, // DMA1 Channel 7
      ADC1_2_IRQHandler, // ADC1, ADC2

#if defined(STM32F10X_LD)

      USB_HP_CAN1_TX_IRQHandler, //
      USB_LP_CAN1_RX0_IRQHandler, //
      CAN1_RX1_IRQHandler, //
      CAN1_SCE_IRQHandler, //
      EXTI9_5_IRQHandler, //
      TIM1_BRK_IRQHandler, //
      TIM1_UP_IRQHandler, //
      TIM1_TRG_COM_IRQHandler, //
      TIM1_CC_IRQHandler, //
      TIM2_IRQHandler, //
      TIM3_IRQHandler, //
      0, //
      I2C1_EV_IRQHandler, //
      I2C1_ER_IRQHandler, //
      0, //
      0, //
      SPI1_IRQHandler, //
      0, //
      USART1_IRQHandler, //
      USART2_IRQHandler, //
      0, //
      EXTI15_10_IRQHandler, //
      RTCAlarm_IRQHandler, //
      USBWakeUp_IRQHandler, //
      0, //
      0, //
      0, //
      0, //
      0, //
      0, //
      0, //
    // @0x108. This is for boot in RAM mode for STM32F10x Low Density devices.
    (pHandler)0xF108F85F

#elif defined(STM32F10X_MD)

    USB_HP_CAN1_TX_IRQHandler, //
    USB_LP_CAN1_RX0_IRQHandler, //
    CAN1_RX1_IRQHandler, //
    CAN1_SCE_IRQHandler, //
    EXTI9_5_IRQHandler, //
    TIM1_BRK_IRQHandler, //
    TIM1_UP_IRQHandler, //
    TIM1_TRG_COM_IRQHandler, //
    TIM1_CC_IRQHandler, //
    TIM2_IRQHandler, //
    TIM3_IRQHandler, //
    TIM4_IRQHandler, //
    I2C1_EV_IRQHandler, //
    I2C1_ER_IRQHandler, //
    I2C2_EV_IRQHandler, //
    I2C2_ER_IRQHandler, //
    SPI1_IRQHandler, //
    SPI2_IRQHandler, //
    USART1_IRQHandler, //
    USART2_IRQHandler, //
    USART3_IRQHandler, //
    EXTI15_10_IRQHandler, //
    RTCAlarm_IRQHandler, //
    USBWakeUp_IRQHandler, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    // @0x108. This is for boot in RAM mode for STM32F10x Low Density devices.
    (pHandler)0xF108F85F

#elif defined(STM32F10X_HD)

    USB_HP_CAN1_TX_IRQHandler, //
    USB_LP_CAN1_RX0_IRQHandler, //
    CAN1_RX1_IRQHandler, //
    CAN1_SCE_IRQHandler, //
    EXTI9_5_IRQHandler, //
    TIM1_BRK_IRQHandler, //
    TIM1_UP_IRQHandler, //
    TIM1_TRG_COM_IRQHandler, //
    TIM1_CC_IRQHandler, //
    TIM2_IRQHandler, //
    TIM3_IRQHandler, //
    TIM4_IRQHandler, //
    I2C1_EV_IRQHandler, //
    I2C1_ER_IRQHandler, //
    I2C2_EV_IRQHandler, //
    I2C2_ER_IRQHandler, //
    SPI1_IRQHandler, //
    SPI2_IRQHandler, //
    USART1_IRQHandler, //
    USART2_IRQHandler, //
    USART3_IRQHandler, //
    EXTI15_10_IRQHandler, //
    RTCAlarm_IRQHandler, //
    USBWakeUp_IRQHandler, //
    TIM8_BRK_IRQHandler, //
    TIM8_UP_IRQHandler, //
    TIM8_TRG_COM_IRQHandler, //
    TIM8_CC_IRQHandler, //
    ADC3_IRQHandler, //
    FSMC_IRQHandler, //
    SDIO_IRQHandler, //
    TIM5_IRQHandler, //
    SPI3_IRQHandler, //
    UART4_IRQHandler, //
    UART5_IRQHandler, //
    TIM6_IRQHandler, //
    TIM7_IRQHandler, //
    DMA2_Channel1_IRQHandler, //
    DMA2_Channel2_IRQHandler, //
    DMA2_Channel3_IRQHandler, //
    DMA2_Channel4_5_IRQHandler, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    // @0x1E0. This is for boot in RAM mode for STM32F10x High Density devices.
    (pHandler)0xF1E0F85F

#elif defined(STM32F10X_XL)

    USB_HP_CAN1_TX_IRQHandler, //
    USB_LP_CAN1_RX0_IRQHandler, //
    CAN1_RX1_IRQHandler, //
    CAN1_SCE_IRQHandler, //
    EXTI9_5_IRQHandler, //
    TIM1_BRK_TIM9_IRQHandler, //
    TIM1_UP_TIM10_IRQHandler, //
    TIM1_TRG_COM_TIM11_IRQHandler, //
    TIM1_CC_IRQHandler, //
    TIM2_IRQHandler, //
    TIM3_IRQHandler, //
    TIM4_IRQHandler, //
    I2C1_EV_IRQHandler, //
    I2C1_ER_IRQHandler, //
    I2C2_EV_IRQHandler, //
    I2C2_ER_IRQHandler, //
    SPI1_IRQHandler, //
    SPI2_IRQHandler, //
    USART1_IRQHandler, //
    USART2_IRQHandler, //
    USART3_IRQHandler, //
    EXTI15_10_IRQHandler, //
    RTCAlarm_IRQHandler, //
    USBWakeUp_IRQHandler, //
    TIM8_BRK_TIM12_IRQHandler, //
    TIM8_UP_TIM13_IRQHandler, //
    TIM8_TRG_COM_TIM14_IRQHandler, //
    TIM8_CC_IRQHandler, //
    ADC3_IRQHandler, //
    FSMC_IRQHandler, //
    SDIO_IRQHandler, //
    TIM5_IRQHandler, //
    SPI3_IRQHandler, //
    UART4_IRQHandler, //
    UART5_IRQHandler, //
    TIM6_IRQHandler, //
    TIM7_IRQHandler, //
    DMA2_Channel1_IRQHandler, //
    DMA2_Channel2_IRQHandler, //
    DMA2_Channel3_IRQHandler, //
    DMA2_Channel4_5_IRQHandler, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    // @0x1E0. This is for boot in RAM mode for STM32F10x Connectivity line Devices.
    (pHandler)0xF1E0F85F

#elif defined(STM32F10X_CL)

    CAN1_TX_IRQHandler, //
    CAN1_RX0_IRQHandler, //
    CAN1_RX1_IRQHandler, //
    CAN1_SCE_IRQHandler, //
    EXTI9_5_IRQHandler, //
    TIM1_BRK_IRQHandler, //
    TIM1_UP_IRQHandler, //
    TIM1_TRG_COM_IRQHandler, //
    TIM1_CC_IRQHandler, //
    TIM2_IRQHandler, //
    TIM3_IRQHandler, //
    TIM4_IRQHandler, //
    I2C1_EV_IRQHandler, //
    I2C1_ER_IRQHandler, //
    I2C2_EV_IRQHandler, //
    I2C2_ER_IRQHandler, //
    SPI1_IRQHandler, //
    SPI2_IRQHandler, //
    USART1_IRQHandler, //
    USART2_IRQHandler, //
    USART3_IRQHandler, //
    EXTI15_10_IRQHandler, //
    RTCAlarm_IRQHandler, //
    OTG_FS_WKUP_IRQHandler, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    TIM5_IRQHandler, //
    SPI3_IRQHandler, //
    UART4_IRQHandler, //
    UART5_IRQHandler, //
    TIM6_IRQHandler, //
    TIM7_IRQHandler, //
    DMA2_Channel1_IRQHandler, //
    DMA2_Channel2_IRQHandler, //
    DMA2_Channel3_IRQHandler, //
    DMA2_Channel4_IRQHandler, //
    DMA2_Channel5_IRQHandler, //
    ETH_IRQHandler, //
    ETH_WKUP_IRQHandler, //
    CAN2_TX_IRQHandler, //
    CAN2_RX0_IRQHandler, //
    CAN2_RX1_IRQHandler, //
    CAN2_SCE_IRQHandler, //
    OTG_FS_IRQHandler, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //

    // @0x1E0. This is for boot in RAM mode for STM32F10x Connectivity line Devices.
    (pHandler)0xF1E0F85F

#elif defined(STM32F10X_LD_VL)

    0, //
    0, //
    0, //
    0, //
    EXTI9_5_IRQHandler, //
    TIM1_BRK_TIM15_IRQHandler, //
    TIM1_UP_TIM16_IRQHandler, //
    TIM1_TRG_COM_TIM17_IRQHandler, //
    TIM1_CC_IRQHandler, //
    TIM2_IRQHandler, //
    TIM3_IRQHandler, //
    0, //
    I2C1_EV_IRQHandler, //
    I2C1_ER_IRQHandler, //
    0, //
    0, //
    SPI1_IRQHandler, //
    0, //
    USART1_IRQHandler, //
    USART2_IRQHandler, //
    0, //
    EXTI15_10_IRQHandler, //
    RTCAlarm_IRQHandler, //
    CEC_IRQHandler, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    TIM6_DAC_IRQHandler, //
    TIM7_IRQHandler, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //

    // @0x01CC. This is for boot in RAM mode for STM32F10x Low Density Value Line devices.
    (pHandler)0xF108F85F

#elif defined(STM32F10X_MD_VL)

    0, //
    0, //
    0, //
    0, //
    EXTI9_5_IRQHandler, //
    TIM1_BRK_TIM15_IRQHandler, //
    TIM1_UP_TIM16_IRQHandler, //
    TIM1_TRG_COM_TIM17_IRQHandler, //
    TIM1_CC_IRQHandler, //
    TIM2_IRQHandler, //
    TIM3_IRQHandler, //
    TIM4_IRQHandler, //
    I2C1_EV_IRQHandler, //
    I2C1_ER_IRQHandler, //
    I2C2_EV_IRQHandler, //
    I2C2_ER_IRQHandler, //
    SPI1_IRQHandler, //
    SPI2_IRQHandler, //
    USART1_IRQHandler, //
    USART2_IRQHandler, //
    USART3_IRQHandler, //
    EXTI15_10_IRQHandler, //
    RTCAlarm_IRQHandler, //
    CEC_IRQHandler, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    TIM6_DAC_IRQHandler, //
    TIM7_IRQHandler, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //

    // @0x01CC. This is for boot in RAM mode for STM32F10x Low Density Value Line devices.
    (pHandler)0xF108F85F

#elif defined(STM32F10X_HD_VL)

    0, //
    0, //
    0, //
    0, //
    EXTI9_5_IRQHandler, //
    TIM1_BRK_TIM15_IRQHandler, //
    TIM1_UP_TIM16_IRQHandler, //
    TIM1_TRG_COM_TIM17_IRQHandler, //
    TIM1_CC_IRQHandler, //
    TIM2_IRQHandler, //
    TIM3_IRQHandler, //
    TIM4_IRQHandler, //
    I2C1_EV_IRQHandler, //
    I2C1_ER_IRQHandler, //
    I2C2_EV_IRQHandler, //
    I2C2_ER_IRQHandler, //
    SPI1_IRQHandler, //
    SPI2_IRQHandler, //
    USART1_IRQHandler, //
    USART2_IRQHandler, //
    USART3_IRQHandler, //
    EXTI15_10_IRQHandler, //
    RTCAlarm_IRQHandler, //
    CEC_IRQHandler, //
    TIM12_IRQHandler, //
    TIM13_IRQHandler, //
    TIM14_IRQHandler, //
    0, //
    0, //
    0, //
    0, //
    TIM5_IRQHandler, //
    SPI3_IRQHandler, //
    UART4_IRQHandler, //
    UART5_IRQHandler, //
    TIM6_DAC_IRQHandler, //
    TIM7_IRQHandler, //
    DMA2_Channel1_IRQHandler, //
    DMA2_Channel2_IRQHandler, //
    DMA2_Channel3_IRQHandler, //
    DMA2_Channel4_5_IRQHandler, //
    DMA2_Channel5_IRQHandler, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //
    0, //

    // @0x1E0. This is for boot in RAM mode for STM32F10x High Density Value line devices.
    (pHandler)0xF108F85F

#else
#error "missing vectors"
#endif
  };

// ----------------------------------------------------------------------------

// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.

void __attribute__ ((section(".after_vectors")))
Default_Handler(void)
{
#if defined(DEBUG)
  __DEBUG_BKPT();
#endif
  while (1)
    {
    }
}

// ----------------------------------------------------------------------------
