/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef __MM32F5277E_H__
#define __MM32F5277E_H__

#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------------------
  -- Interrupt vector numbers
  ----------------------------------------------------------------------------- */

/*!
* @addtogroup Interrupt_vector_numbers Interrupt vector numbers

* @{
*/

/** Interrupt Number Definitions */

#define NUMBER_OF_INT_VECTORS 76                 /**< Number of interrupts in the Vector table */

typedef enum IRQ {
  Reset_IRQn                            = -15,                                   /*!< reset */
  NMI_IRQn                              = -14,                                   /*!< Non-Maskable Interrupt/RCC Clock Security System (CSS) connected to NMI vector */
  HardFault_IRQn                        = -13,                                   /*!< All types of failures */
  MemManage_IRQn                        = -12,                                   /*!< memory management */
  BusFault_IRQn                         = -11,                                   /*!< Prefetch failed, memory access failed */
  UsageFault_IRQn                       = -10,                                   /*!< undefined instruction or illegal status */
  SVCall_IRQn                           = -5,                                    /*!< System service calls via SWI instructions */
  DebugMonitor_IRQn                     = -4,                                    /*!< debug monitor */
  PendSV_IRQn                           = -2,                                    /*!< Suspendable system services */
  SysTick_IRQn                          = -1,                                    /*!< System tick timer */
  WWDG_IWDG_IRQn                        = 0,                                     /*!< Watchdog interrupt (IWDG is EXTI21) */
  PVD_IRQn                              = 1,                                     /*!< Supply Voltage Detect (PVD) Interrupt (EXTI16) */
  BKP_TAMPER_IRQn                       = 2,                                     /*!< BKP intrusion detection interrupted */
  RTC_IRQn                              = 3,                                     /*!< RTC global interrupt */
  FLASH_IRQn                            = 4,                                     /*!< Flash Global Interrupt */
  RCC_CRS_IRQn                          = 5,                                     /*!< RCC and CRS global interrupt */
  EXTI0_IRQn                            = 6,                                     /*!< EXTI line 0 interrupt */
  EXTI1_IRQn                            = 7,                                     /*!< EXTI line 1 interrupt */
  EXTI2_IRQn                            = 8,                                     /*!< EXTI line 2 interrupt */
  EXTI3_IRQn                            = 9,                                     /*!< EXTI line 3 interrupt */
  EXTI4_IRQn                            = 10,                                    /*!< EXTI line 4 interrupt */
  DMA1_CH1_IRQn                         = 11,                                    /*!< DMA1 channel 1 global interrupt */
  DMA1_CH2_IRQn                         = 12,                                    /*!< DMA1 channel 2 global interrupt */
  DMA1_CH3_IRQn                         = 13,                                    /*!< DMA1 channel 3 global interrupt */
  DMA1_CH4_IRQn                         = 14,                                    /*!< DMA1 channel 4 global interrupt */
  DMA1_CH5_IRQn                         = 15,                                    /*!< DMA1 channel 5 global interrupt */
  DMA1_CH6_IRQn                         = 16,                                    /*!< DMA1 channel 6 global interrupt */
  DMA1_CH7_IRQn                         = 17,                                    /*!< DMA1 channel 7 global interrupt */
  ADC1_2_IRQn                           = 18,                                    /*!< ADC1/2 global interrupt */
  DMA1_CH8_IRQn                         = 20,                                    /*!< DMA1 channel 8 global interrupt */
  FlexCAN1_IRQn                         = 21,                                    /*!< FlexCAN1 global interrupt */
  EXTI9_5_IRQn                          = 23,                                    /*!< EXTI line[9:5] interrupt */
  TIM1_BRK_IRQn                         = 24,                                    /*!< TIM1 brake interrupt */
  TIM1_UP_IRQn                          = 25,                                    /*!< TIM1 update interrupted */
  TIM1_TRG_COM_IRQn                     = 26,                                    /*!< TIM1 trigger/COM interrupt */
  TIM1_CC_IRQn                          = 27,                                    /*!< TIM1 capture compare interrupt */
  TIM2_IRQn                             = 28,                                    /*!< TIM2 global interrupt */
  TIM3_IRQn                             = 29,                                    /*!< TIM3 global interrupt */
  TIM4_IRQn                             = 30,                                    /*!< TIM4 global interrupt */
  I2C1_IRQn                             = 31,                                    /*!< I2C1 global interrupt */
  I2C2_IRQn                             = 33,                                    /*!< I2C2 global interrupt */
  SPI1_IRQn                             = 35,                                    /*!< SPI1 global interrupt */
  SPI2_IRQn                             = 36,                                    /*!< SPI2 global interrupt */
  UART1_IRQn                            = 37,                                    /*!< UART1 global interrupt */
  UART2_IRQn                            = 38,                                    /*!< UART2 global interrupt */
  UART3_IRQn                            = 39,                                    /*!< UART3 global interrupt */
  EXTI15_10_IRQn                        = 40,                                    /*!< Interrupt on EXTI line[15:10] */
  RTC_ALR_IRQn                          = 41,                                    /*!< RTC Alarm Interrupt (EXTI17) */
  USB_WKUP_IRQn                         = 42,                                    /*!< USB Wakeup Interrupt (EXTI18) */
  TIM8_BRK_IRQn                         = 43,                                    /*!< TIM8 brake interrupt */
  TIM8_UP_IRQn                          = 44,                                    /*!< TIM8 update interrupted */
  TIM8_TRG_COM_IRQn                     = 45,                                    /*!< TIM8 trigger/COM interrupt */
  TIM8_CC_IRQn                          = 46,                                    /*!< TIM8 capture compare interrupt */
  TIM5_IRQn                             = 50,                                    /*!< TIM5 global interrupt */
  SPI3_IRQn                             = 51,                                    /*!< SPI3 global interrupt */
  UART4_IRQn                            = 52,                                    /*!< UART4 global interrupt */
  UART5_IRQn                            = 53,                                    /*!< UART5 global interrupt */
  TIM6_IRQn                             = 54,                                    /*!< TIM6 global interrupt */
  TIM7_IRQn                             = 55,                                    /*!< TIM7 global interrupt */
  DMA2_CH1_IRQn                         = 56,                                    /*!< DMA2 channel 1 global interrupt */
  DMA2_CH2_IRQn                         = 57,                                    /*!< DMA2 channel 2 global interrupt */
  DMA2_CH3_IRQn                         = 58,                                    /*!< DMA2 channel 3 global interrupt */
  DMA2_CH4_IRQn                         = 59,                                    /*!< DMA2 channel 4 global interrupt */
  DMA2_CH5_IRQn                         = 60,                                    /*!< DMA2 channel 5 global interrupt */
  ENET_IRQn                             = 61,                                    /*!< ENET global interrupt */
  ENET_WKUP_IRQn                        = 62,                                    /*!< ENET wake-up interrupt (EXTI25) */
  COMP_IRQn                             = 64,                                    /*!< Comparator 1/2/3 Global Interrupts (EXTI19/20/24) */
  FlexCAN2_IRQn                         = 65,                                    /*!< FLexCAN2 global interrupt */
  USB_FS_IRQn                           = 67,                                    /*!< USB OTG global interrupt */
  DMA2_CH6_IRQn                         = 68,                                    /*!< DMA2 channel 6 global interrupt */
  DMA2_CH7_IRQn                         = 69,                                    /*!< DMA2 channel 7 global interrupt */
  DMA2_CH8_IRQn                         = 70,                                    /*!< DMA2 channel 8 global interrupt */
  UART6_IRQn                            = 71,                                    /*!< UART6 global interrupt */
  UART7_IRQn                            = 82,                                    /*!< UART7 global interrupt */
  QSPI_IRQn                             = 95,                                    /*!< QSPI global interrupt */
  LPTIM_IRQn                            = 102,                                   /*!< LPTIM global interrupt (EXTI22) */
  LPUART_IRQn                           = 104,                                   /*!< LPUART global interrupt (EXTI23) */
} IRQn_Type;

/*!

 * @}
 */ /* end of group Interrupt_vector_numbers */



/* -----------------------------------------------------------------------------
 -- STAR MC1 Core Configuration
  ----------------------------------------------------------------------------- */

/*!
 * @addtogroup Core_Configuration STAR MC1 Core Configuration
 * @{
 */
/* -------  Start of section using anonymous unions and disabling warnings  ------- */
#if   defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif

#define __STAR_REV                0x0100U   /* Core revision r1p0 */
#define __SAUREGION_PRESENT       0U        /* SAU regions present */
#define __MPU_PRESENT             1U        /* MPU present */
#define __VTOR_PRESENT            1U        /* VTOR present */
#define __NVIC_PRIO_BITS          3U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT             1U        /* FPU present */
#define __DSP_PRESENT             1U        /* DSP extension present */
#define __ICACHE_PRESENT          1U        /* Define if an ICACHE is present or not */
#define __DCACHE_PRESENT          1U        /* Define if an DCACHE is present or not */

#include "core_star.h"                /* Core Peripheral Access Layer */

/*!
 * @}
 */ /* end of group Cortex_Core_Configuration */



/* -----------------------------------------------------------------------------
   -- Device Peripheral Access Layer
   ----------------------------------------------------------------------------- */

/*!
 * @addtogroup Peripheral_access_layer Device Peripheral Access Layer
 * @{

 */

/*
** Start of section using anonymous unions
*/

#if defined(__ARMCC_VERSION)
  #if (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic push
  #else
    #pragma push
    #pragma anon_unions
  #endif
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__IAR_SYSTEMS_ICC__)
  #pragma language=extended
#else
  #error Not supported compiler type
#endif
/* Define the base address of the large memory block mapping */

#define FLASH_ADDR_BASE 0x08000000u
#define SRAM_BASE       0x30000000u

#define APB1_BASE       0x40000000u
#define APB2_BASE       0x40010000u
#define AHB1_BASE       0x40020000u
#define AHB1_1_BASE     0x40040000u
#define AHB2_BASE       0x50000000u
#define AHB3_BASE       0x60000000u
#define AHB3_1_BASE     0x64000000u
#define AHB3_2_BASE     0x68000000u
#define AHB3_3_BASE     0x6C000000u
#define AHB4_BASE       0x90000000u
#define AHB4_1_BASE     0xA0000000u


/* Define the mapping base address of the peripheral module */
#define TIM2_BASE        (APB1_BASE + 0x0000 )
#define TIM3_BASE        (APB1_BASE + 0x0400 )
#define TIM4_BASE        (APB1_BASE + 0x0800 )
#define TIM5_BASE        (APB1_BASE + 0x0C00 )
#define TIM6_BASE        (APB1_BASE + 0x1000 )
#define TIM7_BASE        (APB1_BASE + 0x1400 )
#define RTC_BKP_BASE     (APB1_BASE + 0x2800 )
#define WWDG_BASE        (APB1_BASE + 0x2C00 )
#define IWDG_BASE        (APB1_BASE + 0x3000 )
#define SPI2_BASE        (APB1_BASE + 0x3800 )
#define SPI3_BASE        (APB1_BASE + 0x3C00 )
#define UART2_BASE       (APB1_BASE + 0x4400 )
#define UART3_BASE       (APB1_BASE + 0x4800 )
#define UART4_BASE       (APB1_BASE + 0x4C00 )
#define UART5_BASE       (APB1_BASE + 0x5000 )
#define I2C1_BASE        (APB1_BASE + 0x5400 )
#define I2C2_BASE        (APB1_BASE + 0x5800 )
#define CRS_BASE         (APB1_BASE + 0x6C00 )
#define PWR_BASE         (APB1_BASE + 0x7000 )
#define DBGMCU_BASE      (APB1_BASE + 0x7080 )
#define DAC_BASE         (APB1_BASE + 0x7400 )
#define UART7_BASE       (APB1_BASE + 0x7800 )
#define FLEXCAN1_BASE    (APB1_BASE + 0xC000 )

#define SYSCFG_BASE      (APB2_BASE + 0x0000 )
#define EXTI_BASE        (APB2_BASE + 0x0400 )
#define LPUART_BASE      (APB2_BASE + 0x0800 )
#define ADC1_BASE        (APB2_BASE + 0x2400 )
#define ADC2_BASE        (APB2_BASE + 0x2800 )
#define TIM1_BASE        (APB2_BASE + 0x2C00 )
#define SPI1_BASE        (APB2_BASE + 0x3000 )
#define TIM8_BASE        (APB2_BASE + 0x3400 )
#define UART1_BASE       (APB2_BASE + 0x3800 )
#define UART6_BASE       (APB2_BASE + 0x3C00 )
#define COMP_BASE        (APB2_BASE + 0x4000 )
#define FLEXCAN2_BASE    (APB2_BASE + 0x8000 )
#define LPTIM_BASE         (APB2_BASE + 0xD000 )
#define MDS_BASE         (APB2_BASE + 0xFC00 )

#define DMA1_BASE        (AHB1_BASE + 0x0000 )
#define DMA2_BASE        (AHB1_BASE + 0x0400 )
#define RCC_BASE         (AHB1_BASE + 0x1000 )
#define FLASH_BASE       (AHB1_BASE + 0x2000 )
#define CRC_BASE         (AHB1_BASE + 0x3000 )
#define ENET_BASE        (AHB1_BASE + 0x8000 )
#define CORDIC_BASE      (AHB1_BASE + 0xA000 )

#define GPIOA_BASE       (AHB1_1_BASE + 0x0000 )
#define GPIOB_BASE       (AHB1_1_BASE + 0x0400 )
#define GPIOC_BASE       (AHB1_1_BASE + 0x0800 )
#define GPIOD_BASE       (AHB1_1_BASE + 0x0C00 )
#define GPIOE_BASE       (AHB1_1_BASE + 0x1000 )
#define GPIOF_BASE       (AHB1_1_BASE + 0x1400 )
#define GPIOG_BASE       (AHB1_1_BASE + 0x1800 )
#define GPIOH_BASE       (AHB1_1_BASE + 0x1C00 )
#define GPIOI_BASE       (AHB1_1_BASE + 0x2000 )

#define USB_BASE         (AHB2_BASE + 0x0000 )

#define FSMC_BANK0_BASE  (AHB3_BASE + 0x0000 )

#define FSMC_BANK1_BASE  (AHB3_1_BASE + 0x0000 )

#define FSMC_BANK2_BASE  (AHB3_2_BASE + 0x0000 )

#define FSMC_BANK3_BASE  (AHB3_3_BASE + 0x0000 )

#define QSPI_M_BASE      (AHB4_BASE + 0x0000 )

#define FSMC_BASE        (AHB4_1_BASE + 0x0000 )
#define QSPI_BASE        (AHB4_1_BASE + 0x1000 )
/*!
 * @addtogroup ADC_Peripheral_Access_Layer ADC Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * ADC Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t ADDATA;                                                         ///< A/D data register                            offset: 0x00
    __IO uint32_t ADCFG;                                                          ///< A/D Configuration Register                   offset: 0x04
    __IO uint32_t ADCR;                                                           ///< A/D Control Register                         offset: 0x08
    __IO uint32_t ADCHS;                                                          ///< A/D Channel Select Register                  offset: 0x0C
    __IO uint32_t ADCMPR;                                                         ///< A/D Analog Watchdog Compare Register         offset: 0x10
    __IO uint32_t ADSTA;                                                          ///< A/D Status Register                          offset: 0x14
    __IO uint32_t ADDR[16];                                                       ///< A/D Channel Data Register                    offset: 0x18
    __IO uint32_t ADSTAEXT;                                                       ///< A/D Extended Status Register                 offset: 0x58
    __IO uint32_t CHANY0;                                                         ///< A/D any channel channel select register 0    offset: 0x5C
    __IO uint32_t CHANY1;                                                         ///< A/D arbitrary channel channel selection register 1offset: 0x60
    __IO uint32_t ANYCFG;                                                         ///< A/D Arbitrary Channel Configuration Register offset: 0x64
    __IO uint32_t ANYCR;                                                          ///< A/D arbitrary channel control register       offset: 0x68
    __IO uint32_t ADCFG2;                                                         ///< ADC Configuration Register                   offset: 0x6C
    __IO uint32_t SMPR1;                                                          ///< A/D Sampling Configuration Register 1        offset: 0x70
    __IO uint32_t SMPR2;                                                          ///< A/D Sampling Configuration Register 2        offset: 0x74
    __IO uint32_t SMPR3;                                                          ///< A/D Sampling Configuration Register 2        offset: 0x78
    __IO uint32_t JOFR[4];                                                        ///< A/D Injection Channel Data Compensation Registeroffset: 0x7C
    __IO uint32_t JSQR;                                                           ///< A/D injection channel sequence register      offset: 0x8C
    __IO uint32_t JADDATA;                                                        ///< A/D injection data register                  offset: 0x90
    __IO uint32_t Reserved1[7];                                                   ///< Reserved
    __IO uint32_t JDR[4];                                                         ///< A/D injection channel data register          offset: 0xB0
} ADC_Type;

/*******************************************************************************
 * ADC Type
 ******************************************************************************/

/*!
 * @addtogroup ADC_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief ADC_ADDATA Register Bit Definition
 */

#define ADC_ADDATA_CHANNELSELH_SHIFT         		(22)
#define ADC_ADDATA_CHANNELSELH_MASK          		(0x01U << ADC_ADDATA_CHANNELSELH_SHIFT)
#define ADC_ADDATA_CHANNELSELH(x)               (((uint32_t)(((uint32_t)(x)) << ADC_ADDATA_CHANNELSELH_SHIFT)) & ADC_ADDATA_CHANNELSELH_MASK)

#define ADC_ADDATA_VALID_SHIFT                  (21)
#define ADC_ADDATA_VALID_MASK                   (0x01U << ADC_ADDATA_VALID_SHIFT)
#define ADC_ADDATA_VALID(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ADDATA_VALID_SHIFT)) & ADC_ADDATA_VALID_MASK)

#define ADC_ADDATA_OVERRUN_SHIFT                (20)
#define ADC_ADDATA_OVERRUN_MASK                 (0x01U << ADC_ADDATA_OVERRUN_SHIFT)
#define ADC_ADDATA_OVERRUN(x)                   (((uint32_t)(((uint32_t)(x)) << ADC_ADDATA_OVERRUN_SHIFT)) & ADC_ADDATA_OVERRUN_MASK)

#define ADC_ADDATA_CHANNELSEL_SHIFT             (16)
#define ADC_ADDATA_CHANNELSEL_MASK              (0xFU << ADC_ADDATA_CHANNELSEL_SHIFT)
#define ADC_ADDATA_CHANNELSEL(x)                (((uint32_t)(((uint32_t)(x)) << ADC_ADDATA_CHANNELSEL_SHIFT)) & ADC_ADDATA_CHANNELSEL_MASK)

#define ADC_ADDATA_DATA_SHIFT                   (0)
#define ADC_ADDATA_DATA_MASK                    (0xFFFFU << ADC_ADDATA_DATA_SHIFT)
#define ADC_ADDATA_DATA(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADDATA_DATA_SHIFT)) & ADC_ADDATA_DATA_MASK)

/*!
 * @brief ADC_ADCFG Register Bit Definition
 */

#define ADC_ADCFG_JAWDEN_SHIFT                  (16)
#define ADC_ADCFG_JAWDEN_MASK                   (0x01U << ADC_ADCFG_JAWDEN_SHIFT)
#define ADC_ADCFG_JAWDEN(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ADCFG_JAWDEN_SHIFT)) & ADC_ADCFG_JAWDEN_MASK)

#define ADC_ADCFG_RSLTCTL_SHIFT                 (7)
#define ADC_ADCFG_RSLTCTL_MASK                  (0x7U << ADC_ADCFG_RSLTCTL_SHIFT)
#define ADC_ADCFG_RSLTCTL(x)                    (((uint32_t)(((uint32_t)(x)) << ADC_ADCFG_RSLTCTL_SHIFT)) & ADC_ADCFG_RSLTCTL_MASK)

#define ADC_ADCFG_VSEN_SHIFT                    (3)
#define ADC_ADCFG_VSEN_MASK                     (0x01U << ADC_ADCFG_VSEN_SHIFT)
#define ADC_ADCFG_VSEN(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_ADCFG_VSEN_SHIFT)) & ADC_ADCFG_VSEN_MASK)

#define ADC_ADCFG_TSEN_SHIFT                    (2)
#define ADC_ADCFG_TSEN_MASK                     (0x01U << ADC_ADCFG_TSEN_SHIFT)
#define ADC_ADCFG_TSEN(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_ADCFG_TSEN_SHIFT)) & ADC_ADCFG_TSEN_MASK)

#define ADC_ADCFG_AWDEN_SHIFT                   (1)
#define ADC_ADCFG_AWDEN_MASK                    (0x01U << ADC_ADCFG_AWDEN_SHIFT)
#define ADC_ADCFG_AWDEN(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADCFG_AWDEN_SHIFT)) & ADC_ADCFG_AWDEN_MASK)

#define ADC_ADCFG_ADEN_SHIFT                    (0)
#define ADC_ADCFG_ADEN_MASK                     (0x01U << ADC_ADCFG_ADEN_SHIFT)
#define ADC_ADCFG_ADEN(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_ADCFG_ADEN_SHIFT)) & ADC_ADCFG_ADEN_MASK)

/*!
 * @brief ADC_ADCR Register Bit Definition
 */

#define ADC_ADCR_CMPCHH_SHIFT                   (28)
#define ADC_ADCR_CMPCHH_MASK                    (0x01U << ADC_ADCR_CMPCHH_SHIFT)
#define ADC_ADCR_CMPCHH(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADCR_CMPCHH_SHIFT)) & ADC_ADCR_CMPCHH_MASK)

#define ADC_ADCR_EOCIE_SHIFT                    (27)
#define ADC_ADCR_EOCIE_MASK                     (0x01U << ADC_ADCR_EOCIE_SHIFT)
#define ADC_ADCR_EOCIE(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_ADCR_EOCIE_SHIFT)) & ADC_ADCR_EOCIE_MASK)

#define ADC_ADCR_EOSMPIE_SHIFT                  (26)
#define ADC_ADCR_EOSMPIE_MASK                   (0x01U << ADC_ADCR_EOSMPIE_SHIFT)
#define ADC_ADCR_EOSMPIE(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ADCR_EOSMPIE_SHIFT)) & ADC_ADCR_EOSMPIE_MASK)

#define ADC_ADCR_TRGEDGE_SHIFT                  (24)
#define ADC_ADCR_TRGEDGE_MASK                   (0x3U << ADC_ADCR_TRGEDGE_SHIFT)
#define ADC_ADCR_TRGEDGE(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ADCR_TRGEDGE_SHIFT)) & ADC_ADCR_TRGEDGE_MASK)

#define ADC_ADCR_TRGSHIFT_SHIFT                 (19)
#define ADC_ADCR_TRGSHIFT_MASK                  (0x7U << ADC_ADCR_TRGSHIFT_SHIFT)
#define ADC_ADCR_TRGSHIFT(x)                    (((uint32_t)(((uint32_t)(x)) << ADC_ADCR_TRGSHIFT_SHIFT)) & ADC_ADCR_TRGSHIFT_MASK)

#define ADC_ADCR_SCANDIR_SHIFT                  (16)
#define ADC_ADCR_SCANDIR_MASK                   (0x01U << ADC_ADCR_SCANDIR_SHIFT)
#define ADC_ADCR_SCANDIR(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ADCR_SCANDIR_SHIFT)) & ADC_ADCR_SCANDIR_MASK)

#define ADC_ADCR_CMPCHL_SHIFT                   (12)
#define ADC_ADCR_CMPCHL_MASK                    (0xFU << ADC_ADCR_CMPCHL_SHIFT)
#define ADC_ADCR_CMPCHL(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADCR_CMPCHL_SHIFT)) & ADC_ADCR_CMPCHL_MASK)

#define ADC_ADCR_ALIGN_SHIFT                    (11)
#define ADC_ADCR_ALIGN_MASK                     (0x01U << ADC_ADCR_ALIGN_SHIFT)
#define ADC_ADCR_ALIGN(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_ADCR_ALIGN_SHIFT)) & ADC_ADCR_ALIGN_MASK)

#define ADC_ADCR_ADMD_SHIFT                     (9)
#define ADC_ADCR_ADMD_MASK                      (0x3U << ADC_ADCR_ADMD_SHIFT)
#define ADC_ADCR_ADMD(x)                        (((uint32_t)(((uint32_t)(x)) << ADC_ADCR_ADMD_SHIFT)) & ADC_ADCR_ADMD_MASK)

#define ADC_ADCR_ADST_SHIFT                     (8)
#define ADC_ADCR_ADST_MASK                      (0x01U << ADC_ADCR_ADST_SHIFT)
#define ADC_ADCR_ADST(x)                        (((uint32_t)(((uint32_t)(x)) << ADC_ADCR_ADST_SHIFT)) & ADC_ADCR_ADST_MASK)

#define ADC_ADCR_DMAEN_SHIFT                    (3)
#define ADC_ADCR_DMAEN_MASK                     (0x01U << ADC_ADCR_DMAEN_SHIFT)
#define ADC_ADCR_DMAEN(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_ADCR_DMAEN_SHIFT)) & ADC_ADCR_DMAEN_MASK)

#define ADC_ADCR_TRGEN_SHIFT                    (2)
#define ADC_ADCR_TRGEN_MASK                     (0x01U << ADC_ADCR_TRGEN_SHIFT)
#define ADC_ADCR_TRGEN(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_ADCR_TRGEN_SHIFT)) & ADC_ADCR_TRGEN_MASK)

#define ADC_ADCR_AWDIE_SHIFT                    (1)
#define ADC_ADCR_AWDIE_MASK                     (0x01U << ADC_ADCR_AWDIE_SHIFT)
#define ADC_ADCR_AWDIE(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_ADCR_AWDIE_SHIFT)) & ADC_ADCR_AWDIE_MASK)

#define ADC_ADCR_EOSIE_SHIFT                    (0)
#define ADC_ADCR_EOSIE_MASK                     (0x01U << ADC_ADCR_EOSIE_SHIFT)
#define ADC_ADCR_EOSIE(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_ADCR_EOSIE_SHIFT)) & ADC_ADCR_EOSIE_MASK)

/*!
 * @brief ADC_ADCHS Register Bit Definition
 */

#define ADC_ADCHS_CHEN15_SHIFT                  (15)
#define ADC_ADCHS_CHEN15_MASK                   (0x01U << ADC_ADCHS_CHEN15_SHIFT)
#define ADC_ADCHS_CHEN15(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ADCHS_CHEN15_SHIFT)) & ADC_ADCHS_CHEN15_MASK)

#define ADC_ADCHS_CHEN14_SHIFT                  (14)
#define ADC_ADCHS_CHEN14_MASK                   (0x01U << ADC_ADCHS_CHEN14_SHIFT)
#define ADC_ADCHS_CHEN14(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ADCHS_CHEN14_SHIFT)) & ADC_ADCHS_CHEN14_MASK)

#define ADC_ADCHS_CHEN13_SHIFT                  (13)
#define ADC_ADCHS_CHEN13_MASK                   (0x01U << ADC_ADCHS_CHEN13_SHIFT)
#define ADC_ADCHS_CHEN13(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ADCHS_CHEN13_SHIFT)) & ADC_ADCHS_CHEN13_MASK)

#define ADC_ADCHS_CHEN12_SHIFT                  (12)
#define ADC_ADCHS_CHEN12_MASK                   (0x01U << ADC_ADCHS_CHEN12_SHIFT)
#define ADC_ADCHS_CHEN12(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ADCHS_CHEN12_SHIFT)) & ADC_ADCHS_CHEN12_MASK)

#define ADC_ADCHS_CHEN11_SHIFT                  (11)
#define ADC_ADCHS_CHEN11_MASK                   (0x01U << ADC_ADCHS_CHEN11_SHIFT)
#define ADC_ADCHS_CHEN11(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ADCHS_CHEN11_SHIFT)) & ADC_ADCHS_CHEN11_MASK)

#define ADC_ADCHS_CHEN10_SHIFT                  (10)
#define ADC_ADCHS_CHEN10_MASK                   (0x01U << ADC_ADCHS_CHEN10_SHIFT)
#define ADC_ADCHS_CHEN10(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ADCHS_CHEN10_SHIFT)) & ADC_ADCHS_CHEN10_MASK)

#define ADC_ADCHS_CHEN9_SHIFT                   (9)
#define ADC_ADCHS_CHEN9_MASK                    (0x01U << ADC_ADCHS_CHEN9_SHIFT)
#define ADC_ADCHS_CHEN9(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADCHS_CHEN9_SHIFT)) & ADC_ADCHS_CHEN9_MASK)

#define ADC_ADCHS_CHEN8_SHIFT                   (8)
#define ADC_ADCHS_CHEN8_MASK                    (0x01U << ADC_ADCHS_CHEN8_SHIFT)
#define ADC_ADCHS_CHEN8(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADCHS_CHEN8_SHIFT)) & ADC_ADCHS_CHEN8_MASK)

#define ADC_ADCHS_CHEN7_SHIFT                   (7)
#define ADC_ADCHS_CHEN7_MASK                    (0x01U << ADC_ADCHS_CHEN7_SHIFT)
#define ADC_ADCHS_CHEN7(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADCHS_CHEN7_SHIFT)) & ADC_ADCHS_CHEN7_MASK)

#define ADC_ADCHS_CHEN6_SHIFT                   (6)
#define ADC_ADCHS_CHEN6_MASK                    (0x01U << ADC_ADCHS_CHEN6_SHIFT)
#define ADC_ADCHS_CHEN6(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADCHS_CHEN6_SHIFT)) & ADC_ADCHS_CHEN6_MASK)

#define ADC_ADCHS_CHEN5_SHIFT                   (5)
#define ADC_ADCHS_CHEN5_MASK                    (0x01U << ADC_ADCHS_CHEN5_SHIFT)
#define ADC_ADCHS_CHEN5(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADCHS_CHEN5_SHIFT)) & ADC_ADCHS_CHEN5_MASK)

#define ADC_ADCHS_CHEN4_SHIFT                   (4)
#define ADC_ADCHS_CHEN4_MASK                    (0x01U << ADC_ADCHS_CHEN4_SHIFT)
#define ADC_ADCHS_CHEN4(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADCHS_CHEN4_SHIFT)) & ADC_ADCHS_CHEN4_MASK)

#define ADC_ADCHS_CHEN3_SHIFT                   (3)
#define ADC_ADCHS_CHEN3_MASK                    (0x01U << ADC_ADCHS_CHEN3_SHIFT)
#define ADC_ADCHS_CHEN3(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADCHS_CHEN3_SHIFT)) & ADC_ADCHS_CHEN3_MASK)

#define ADC_ADCHS_CHEN2_SHIFT                   (2)
#define ADC_ADCHS_CHEN2_MASK                    (0x01U << ADC_ADCHS_CHEN2_SHIFT)
#define ADC_ADCHS_CHEN2(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADCHS_CHEN2_SHIFT)) & ADC_ADCHS_CHEN2_MASK)

#define ADC_ADCHS_CHEN1_SHIFT                   (1)
#define ADC_ADCHS_CHEN1_MASK                    (0x01U << ADC_ADCHS_CHEN1_SHIFT)
#define ADC_ADCHS_CHEN1(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADCHS_CHEN1_SHIFT)) & ADC_ADCHS_CHEN1_MASK)

#define ADC_ADCHS_CHEN0_SHIFT                   (0)
#define ADC_ADCHS_CHEN0_MASK                    (0x01U << ADC_ADCHS_CHEN0_SHIFT)
#define ADC_ADCHS_CHEN0(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADCHS_CHEN0_SHIFT)) & ADC_ADCHS_CHEN0_MASK)

/*!
 * @brief ADC_ADCMPR Register Bit Definition
 */

#define ADC_ADCMPR_CMPHDATA_SHIFT               (16)
#define ADC_ADCMPR_CMPHDATA_MASK                (0xFFFU << ADC_ADCMPR_CMPHDATA_SHIFT)
#define ADC_ADCMPR_CMPHDATA(x)                  (((uint32_t)(((uint32_t)(x)) << ADC_ADCMPR_CMPHDATA_SHIFT)) & ADC_ADCMPR_CMPHDATA_MASK)

#define ADC_ADCMPR_CMPLDATA_SHIFT               (0)
#define ADC_ADCMPR_CMPLDATA_MASK                (0xFFFU << ADC_ADCMPR_CMPLDATA_SHIFT)
#define ADC_ADCMPR_CMPLDATA(x)                  (((uint32_t)(((uint32_t)(x)) << ADC_ADCMPR_CMPLDATA_SHIFT)) & ADC_ADCMPR_CMPLDATA_MASK)

/*!
 * @brief ADC_ADSTA Register Bit Definition
 */

#define ADC_ADSTA_OVERRUN_SHIFT                 (20)
#define ADC_ADSTA_OVERRUN_MASK                  (0xFFFU << ADC_ADSTA_OVERRUN_SHIFT)
#define ADC_ADSTA_OVERRUN(x)                    (((uint32_t)(((uint32_t)(x)) << ADC_ADSTA_OVERRUN_SHIFT)) & ADC_ADSTA_OVERRUN_MASK)

#define ADC_ADSTA_VALID_SHIFT                   (8)
#define ADC_ADSTA_VALID_MASK                    (0xFFFU << ADC_ADSTA_VALID_SHIFT)
#define ADC_ADSTA_VALID(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADSTA_VALID_SHIFT)) & ADC_ADSTA_VALID_MASK)

#define ADC_ADSTA_CHANNELL_SHIFT                (4)
#define ADC_ADSTA_CHANNELL_MASK                 (0xFU << ADC_ADSTA_CHANNELL_SHIFT)
#define ADC_ADSTA_CHANNELL(x)                   (((uint32_t)(((uint32_t)(x)) << ADC_ADSTA_CHANNELL_SHIFT)) & ADC_ADSTA_CHANNELL_MASK)

#define ADC_ADSTA_CHANNELH_SHIFT                (3)
#define ADC_ADSTA_CHANNELH_MASK                 (0x01U << ADC_ADSTA_CHANNELH_SHIFT)
#define ADC_ADSTA_CHANNELH(x)                   (((uint32_t)(((uint32_t)(x)) << ADC_ADSTA_CHANNELH_SHIFT)) & ADC_ADSTA_CHANNELH_MASK)

#define ADC_ADSTA_BUSY_SHIFT                    (2)
#define ADC_ADSTA_BUSY_MASK                     (0x01U << ADC_ADSTA_BUSY_SHIFT)
#define ADC_ADSTA_BUSY(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_ADSTA_BUSY_SHIFT)) & ADC_ADSTA_BUSY_MASK)

#define ADC_ADSTA_AWDIF_SHIFT                   (1)
#define ADC_ADSTA_AWDIF_MASK                    (0x01U << ADC_ADSTA_AWDIF_SHIFT)
#define ADC_ADSTA_AWDIF(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADSTA_AWDIF_SHIFT)) & ADC_ADSTA_AWDIF_MASK)

#define ADC_ADSTA_EOSIF_SHIFT                   (0)
#define ADC_ADSTA_EOSIF_MASK                    (0x01U << ADC_ADSTA_EOSIF_SHIFT)
#define ADC_ADSTA_EOSIF(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADSTA_EOSIF_SHIFT)) & ADC_ADSTA_EOSIF_MASK)

/*!
 * @brief ADC_ADDR  Register Bit Definition
 */

#define ADC_ADDR_VALID_SHIFT                    (21)
#define ADC_ADDR_VALID_MASK                     (0x01U << ADC_ADDR_VALID_SHIFT)
#define ADC_ADDR_VALID(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_ADDR_VALID_SHIFT)) & ADC_ADDR_VALID_MASK)

#define ADC_ADDR_OVERRUN_SHIFT                  (20)
#define ADC_ADDR_OVERRUN_MASK                   (0x01U << ADC_ADDR_OVERRUN_SHIFT)
#define ADC_ADDR_OVERRUN(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ADDR_OVERRUN_SHIFT)) & ADC_ADDR_OVERRUN_MASK)

#define ADC_ADDR_DATA_SHIFT                     (0)
#define ADC_ADDR_DATA_MASK                      (0xFFFFU << ADC_ADDR_DATA_SHIFT)
#define ADC_ADDR_DATA(x)                        (((uint32_t)(((uint32_t)(x)) << ADC_ADDR_DATA_SHIFT)) & ADC_ADDR_DATA_MASK)

/*!
 * @brief ADC_ADSTA_EXT Register Bit Definition
 */

#define ADC_ADSTAEXT_FREOCIF_SHIFT              (26)
#define ADC_ADSTAEXT_FREOCIF_MASK               (0x01U << ADC_ADSTAEXT_FREOCIF_SHIFT)
#define ADC_ADSTAEXT_FREOCIF(x)                 (((uint32_t)(((uint32_t)(x)) << ADC_ADSTAEXT_FREOCIF_SHIFT)) & ADC_ADSTAEXT_FREOCIF_MASK)

#define ADC_ADSTAEXT_CALBUSY_SHIFT              (25)
#define ADC_ADSTAEXT_CALBUSY_MASK               (0x01U << ADC_ADSTAEXT_CALBUSY_SHIFT)
#define ADC_ADSTAEXT_CALBUSY(x)                 (((uint32_t)(((uint32_t)(x)) << ADC_ADSTAEXT_CALBUSY_SHIFT)) & ADC_ADSTAEXT_CALBUSY_MASK)

#define ADC_ADSTAEXT_EOCALIF_SHIFT              (24)
#define ADC_ADSTAEXT_EOCALIF_MASK               (0x01U << ADC_ADSTAEXT_EOCALIF_SHIFT)
#define ADC_ADSTAEXT_EOCALIF(x)                 (((uint32_t)(((uint32_t)(x)) << ADC_ADSTAEXT_EOCALIF_SHIFT)) & ADC_ADSTAEXT_EOCALIF_MASK)

#define ADC_ADSTAEXT_JBUSY_SHIFT                (21)
#define ADC_ADSTAEXT_JBUSY_MASK                 (0x01U << ADC_ADSTAEXT_JBUSY_SHIFT)
#define ADC_ADSTAEXT_JBUSY(x)                   (((uint32_t)(((uint32_t)(x)) << ADC_ADSTAEXT_JBUSY_SHIFT)) & ADC_ADSTAEXT_JBUSY_MASK)

#define ADC_ADSTAEXT_JEOSIF_SHIFT               (20)
#define ADC_ADSTAEXT_JEOSIF_MASK                (0x01U << ADC_ADSTAEXT_JEOSIF_SHIFT)
#define ADC_ADSTAEXT_JEOSIF(x)                  (((uint32_t)(((uint32_t)(x)) << ADC_ADSTAEXT_JEOSIF_SHIFT)) & ADC_ADSTAEXT_JEOSIF_MASK)

#define ADC_ADSTAEXT_JEOCIF_SHIFT               (19)
#define ADC_ADSTAEXT_JEOCIF_MASK                (0x01U << ADC_ADSTAEXT_JEOCIF_SHIFT)
#define ADC_ADSTAEXT_JEOCIF(x)                  (((uint32_t)(((uint32_t)(x)) << ADC_ADSTAEXT_JEOCIF_SHIFT)) & ADC_ADSTAEXT_JEOCIF_MASK)

#define ADC_ADSTAEXT_JEOSMPIF_SHIFT             (18)
#define ADC_ADSTAEXT_JEOSMPIF_MASK              (0x01U << ADC_ADSTAEXT_JEOSMPIF_SHIFT)
#define ADC_ADSTAEXT_JEOSMPIF(x)                (((uint32_t)(((uint32_t)(x)) << ADC_ADSTAEXT_JEOSMPIF_SHIFT)) & ADC_ADSTAEXT_JEOSMPIF_MASK)

#define ADC_ADSTAEXT_EOCIF_SHIFT                (17)
#define ADC_ADSTAEXT_EOCIF_MASK                 (0x01U << ADC_ADSTAEXT_EOCIF_SHIFT)
#define ADC_ADSTAEXT_EOCIF(x)                   (((uint32_t)(((uint32_t)(x)) << ADC_ADSTAEXT_EOCIF_SHIFT)) & ADC_ADSTAEXT_EOCIF_MASK)

#define ADC_ADSTAEXT_EOSMPIF_SHIFT              (16)
#define ADC_ADSTAEXT_EOSMPIF_MASK               (0x01U << ADC_ADSTAEXT_EOSMPIF_SHIFT)
#define ADC_ADSTAEXT_EOSMPIF(x)                 (((uint32_t)(((uint32_t)(x)) << ADC_ADSTAEXT_EOSMPIF_SHIFT)) & ADC_ADSTAEXT_EOSMPIF_MASK)

#define ADC_ADSTAEXT_OVERRUN_SHIFT              (4)
#define ADC_ADSTAEXT_OVERRUN_MASK               (0xFU << ADC_ADSTAEXT_OVERRUN_SHIFT)
#define ADC_ADSTAEXT_OVERRUN(x)                 (((uint32_t)(((uint32_t)(x)) << ADC_ADSTAEXT_OVERRUN_SHIFT)) & ADC_ADSTAEXT_OVERRUN_MASK)

#define ADC_ADSTAEXT_VALID_SHIFT                (0)
#define ADC_ADSTAEXT_VALID_MASK                 (0xFU << ADC_ADSTAEXT_VALID_SHIFT)
#define ADC_ADSTAEXT_VALID(x)                   (((uint32_t)(((uint32_t)(x)) << ADC_ADSTAEXT_VALID_SHIFT)) & ADC_ADSTAEXT_VALID_MASK)

/*!
 * @brief ADC_CHANY0 Register Bit Definition
 */

#define ADC_CHANY0_CHANYSELL7_SHIFT             (28)
#define ADC_CHANY0_CHANYSELL7_MASK              (0xFU << ADC_CHANY0_CHANYSELL7_SHIFT)
#define ADC_CHANY0_CHANYSELL7(x)                (((uint32_t)(((uint32_t)(x)) << ADC_CHANY0_CHANYSELL7_SHIFT)) & ADC_CHANY0_CHANYSELL7_MASK)

#define ADC_CHANY0_CHANYSELL6_SHIFT             (24)
#define ADC_CHANY0_CHANYSELL6_MASK              (0xFU << ADC_CHANY0_CHANYSELL6_SHIFT)
#define ADC_CHANY0_CHANYSELL6(x)                (((uint32_t)(((uint32_t)(x)) << ADC_CHANY0_CHANYSELL6_SHIFT)) & ADC_CHANY0_CHANYSELL6_MASK)

#define ADC_CHANY0_CHANYSELL5_SHIFT             (20)
#define ADC_CHANY0_CHANYSELL5_MASK              (0xFU << ADC_CHANY0_CHANYSELL5_SHIFT)
#define ADC_CHANY0_CHANYSELL5(x)                (((uint32_t)(((uint32_t)(x)) << ADC_CHANY0_CHANYSELL5_SHIFT)) & ADC_CHANY0_CHANYSELL5_MASK)

#define ADC_CHANY0_CHANYSELL4_SHIFT             (16)
#define ADC_CHANY0_CHANYSELL4_MASK              (0xFU << ADC_CHANY0_CHANYSELL4_SHIFT)
#define ADC_CHANY0_CHANYSELL4(x)                (((uint32_t)(((uint32_t)(x)) << ADC_CHANY0_CHANYSELL4_SHIFT)) & ADC_CHANY0_CHANYSELL4_MASK)

#define ADC_CHANY0_CHANYSELL3_SHIFT             (12)
#define ADC_CHANY0_CHANYSELL3_MASK              (0xFU << ADC_CHANY0_CHANYSELL3_SHIFT)
#define ADC_CHANY0_CHANYSELL3(x)                (((uint32_t)(((uint32_t)(x)) << ADC_CHANY0_CHANYSELL3_SHIFT)) & ADC_CHANY0_CHANYSELL3_MASK)

#define ADC_CHANY0_CHANYSELL2_SHIFT             (8)
#define ADC_CHANY0_CHANYSELL2_MASK              (0xFU << ADC_CHANY0_CHANYSELL2_SHIFT)
#define ADC_CHANY0_CHANYSELL2(x)                (((uint32_t)(((uint32_t)(x)) << ADC_CHANY0_CHANYSELL2_SHIFT)) & ADC_CHANY0_CHANYSELL2_MASK)

#define ADC_CHANY0_CHANYSELL1_SHIFT             (4)
#define ADC_CHANY0_CHANYSELL1_MASK              (0xFU << ADC_CHANY0_CHANYSELL1_SHIFT)
#define ADC_CHANY0_CHANYSELL1(x)                (((uint32_t)(((uint32_t)(x)) << ADC_CHANY0_CHANYSELL1_SHIFT)) & ADC_CHANY0_CHANYSELL1_MASK)

#define ADC_CHANY0_CHANYSELL0_SHIFT             (0)
#define ADC_CHANY0_CHANYSELL0_MASK              (0xFU << ADC_CHANY0_CHANYSELL0_SHIFT)
#define ADC_CHANY0_CHANYSELL0(x)                (((uint32_t)(((uint32_t)(x)) << ADC_CHANY0_CHANYSELL0_SHIFT)) & ADC_CHANY0_CHANYSELL0_MASK)

/*!
 * @brief ADC_CHANY1 Register Bit Definition
 */

#define ADC_CHANY1_CHANYSELL15_SHIFT            (28)
#define ADC_CHANY1_CHANYSELL15_MASK             (0xFU << ADC_CHANY1_CHANYSEL15_SHIFT)
#define ADC_CHANY1_CHANYSELL15(x)               (((uint32_t)(((uint32_t)(x)) << ADC_CHANY1_CHANYSEL15_SHIFT)) & ADC_CHANY1_CHANYSEL15_MASK)

#define ADC_CHANY1_CHANYSELL14_SHIFT            (24)
#define ADC_CHANY1_CHANYSELL14_MASK             (0xFU << ADC_CHANY1_CHANYSELL14_SHIFT)
#define ADC_CHANY1_CHANYSELL14(x)               (((uint32_t)(((uint32_t)(x)) << ADC_CHANY1_CHANYSELL14_SHIFT)) & ADC_CHANY1_CHANYSELL14_MASK)

#define ADC_CHANY1_CHANYSELL13_SHIFT            (20)
#define ADC_CHANY1_CHANYSELL13_MASK             (0xFU << ADC_CHANY1_CHANYSELL13_SHIFT)
#define ADC_CHANY1_CHANYSELL13(x)               (((uint32_t)(((uint32_t)(x)) << ADC_CHANY1_CHANYSELL13_SHIFT)) & ADC_CHANY1_CHANYSELL13_MASK)

#define ADC_CHANY1_CHANYSELL12_SHIFT            (16)
#define ADC_CHANY1_CHANYSELL12_MASK             (0xFU << ADC_CHANY1_CHANYSELL12_SHIFT)
#define ADC_CHANY1_CHANYSELL12(x)               (((uint32_t)(((uint32_t)(x)) << ADC_CHANY1_CHANYSELL12_SHIFT)) & ADC_CHANY1_CHANYSELL12_MASK)

#define ADC_CHANY1_CHANYSELL11_SHIFT            (12)
#define ADC_CHANY1_CHANYSELL11_MASK             (0xFU << ADC_CHANY1_CHANYSELL11_SHIFT)
#define ADC_CHANY1_CHANYSELL11(x)               (((uint32_t)(((uint32_t)(x)) << ADC_CHANY1_CHANYSELL11_SHIFT)) & ADC_CHANY1_CHANYSELL11_MASK)

#define ADC_CHANY1_CHANYSELL10_SHIFT            (8)
#define ADC_CHANY1_CHANYSELL10_MASK             (0xFU << ADC_CHANY1_CHANYSELL10_SHIFT)
#define ADC_CHANY1_CHANYSELL10(x)               (((uint32_t)(((uint32_t)(x)) << ADC_CHANY1_CHANYSELL10_SHIFT)) & ADC_CHANY1_CHANYSELL10_MASK)

#define ADC_CHANY1_CHANYSELL9_SHIFT             (4)
#define ADC_CHANY1_CHANYSELL9_MASK              (0xFU << ADC_CHANY1_CHANYSELL9_SHIFT)
#define ADC_CHANY1_CHANYSELL9(x)                (((uint32_t)(((uint32_t)(x)) << ADC_CHANY1_CHANYSELL9_SHIFT)) & ADC_CHANY1_CHANYSELL9_MASK)

#define ADC_CHANY1_CHANYSELL8_SHIFT             (0)
#define ADC_CHANY1_CHANYSELL8_MASK              (0xFU << ADC_CHANY1_CHANYSELL8_SHIFT)
#define ADC_CHANY1_CHANYSELL8(x)                (((uint32_t)(((uint32_t)(x)) << ADC_CHANY1_CHANYSELL8_SHIFT)) & ADC_CHANY1_CHANYSELL8_MASK)

/*!
 * @brief ADC_ANY_CFG Register Bit Definition
 */

#define ADC_ANYCFG_CHANYSELH15_SHIFT            (23)
#define ADC_ANYCFG_CHANYSELH15_MASK             (0x01U << ADC_ANYCFG_CHANYSELH15_SHIFT)
#define ADC_ANYCFG_CHANYSELH15(x)               (((uint32_t)(((uint32_t)(x)) << ADC_ANYCFG_CHANYSELH15_SHIFT)) & ADC_ANYCFG_CHANYSELH15_MASK)

#define ADC_ANYCFG_CHANYSELH14_SHIFT            (22)
#define ADC_ANYCFG_CHANYSELH14_MASK             (0x01U << ADC_ANYCFG_CHANYSELH14_SHIFT)
#define ADC_ANYCFG_CHANYSELH14(x)               (((uint32_t)(((uint32_t)(x)) << ADC_ANYCFG_CHANYSELH14_SHIFT)) & ADC_ANYCFG_CHANYSELH14_MASK)

#define ADC_ANYCFG_CHANYSELH13_SHIFT            (21)
#define ADC_ANYCFG_CHANYSELH13_MASK             (0x01U << ADC_ANYCFG_CHANYSELH13_SHIFT)
#define ADC_ANYCFG_CHANYSELH13(x)               (((uint32_t)(((uint32_t)(x)) << ADC_ANYCFG_CHANYSELH13_SHIFT)) & ADC_ANYCFG_CHANYSELH13_MASK)

#define ADC_ANYCFG_CHANYSELH12_SHIFT            (20)
#define ADC_ANYCFG_CHANYSELH12_MASK             (0x01U << ADC_ANYCFG_CHANYSELH12_SHIFT)
#define ADC_ANYCFG_CHANYSELH12(x)               (((uint32_t)(((uint32_t)(x)) << ADC_ANYCFG_CHANYSELH12_SHIFT)) & ADC_ANYCFG_CHANYSELH12_MASK)

#define ADC_ANYCFG_CHANYSELH11_SHIFT            (19)
#define ADC_ANYCFG_CHANYSELH11_MASK             (0x01U << ADC_ANYCFG_CHANYSELH11_SHIFT)
#define ADC_ANYCFG_CHANYSELH11(x)               (((uint32_t)(((uint32_t)(x)) << ADC_ANYCFG_CHANYSELH11_SHIFT)) & ADC_ANYCFG_CHANYSELH11_MASK)

#define ADC_ANYCFG_CHANYSELH10_SHIFT            (18)
#define ADC_ANYCFG_CHANYSELH10_MASK             (0x01U << ADC_ANYCFG_CHANYSELH10_SHIFT)
#define ADC_ANYCFG_CHANYSELH10(x)               (((uint32_t)(((uint32_t)(x)) << ADC_ANYCFG_CHANYSELH10_SHIFT)) & ADC_ANYCFG_CHANYSELH10_MASK)

#define ADC_ANYCFG_CHANYSELH9_SHIFT             (17)
#define ADC_ANYCFG_CHANYSELH9_MASK              (0x01U << ADC_ANYCFG_CHANYSELH9_SHIFT)
#define ADC_ANYCFG_CHANYSELH9(x)                (((uint32_t)(((uint32_t)(x)) << ADC_ANYCFG_CHANYSELH9_SHIFT)) & ADC_ANYCFG_CHANYSELH9_MASK)

#define ADC_ANYCFG_CHANYSELH8_SHIFT             (16)
#define ADC_ANYCFG_CHANYSELH8_MASK              (0x01U << ADC_ANYCFG_CHANYSELH8_SHIFT)
#define ADC_ANYCFG_CHANYSELH8(x)                (((uint32_t)(((uint32_t)(x)) << ADC_ANYCFG_CHANYSELH8_SHIFT)) & ADC_ANYCFG_CHANYSELH8_MASK)

#define ADC_ANYCFG_CHANYSELH7_SHIFT             (15)
#define ADC_ANYCFG_CHANYSELH7_MASK              (0x01U << ADC_ANYCFG_CHANYSELH7_SHIFT)
#define ADC_ANYCFG_CHANYSELH7(x)                (((uint32_t)(((uint32_t)(x)) << ADC_ANYCFG_CHANYSELH7_SHIFT)) & ADC_ANYCFG_CHANYSELH7_MASK)

#define ADC_ANYCFG_CHANYSELH6_SHIFT             (14)
#define ADC_ANYCFG_CHANYSELH6_MASK              (0x01U << ADC_ANYCFG_CHANYSELH6_SHIFT)
#define ADC_ANYCFG_CHANYSELH6(x)                (((uint32_t)(((uint32_t)(x)) << ADC_ANYCFG_CHANYSELH6_SHIFT)) & ADC_ANYCFG_CHANYSELH6_MASK)

#define ADC_ANYCFG_CHANYSELH5_SHIFT             (13)
#define ADC_ANYCFG_CHANYSELH5_MASK              (0x01U << ADC_ANYCFG_CHANYSELH5_SHIFT)
#define ADC_ANYCFG_CHANYSELH5(x)                (((uint32_t)(((uint32_t)(x)) << ADC_ANYCFG_CHANYSELH5_SHIFT)) & ADC_ANYCFG_CHANYSELH5_MASK)

#define ADC_ANYCFG_CHANYSELH4_SHIFT             (12)
#define ADC_ANYCFG_CHANYSELH4_MASK              (0x01U << ADC_ANYCFG_CHANYSELH4_SHIFT)
#define ADC_ANYCFG_CHANYSELH4(x)                (((uint32_t)(((uint32_t)(x)) << ADC_ANYCFG_CHANYSELH4_SHIFT)) & ADC_ANYCFG_CHANYSELH4_MASK)

#define ADC_ANYCFG_CHANYSELH3_SHIFT             (11)
#define ADC_ANYCFG_CHANYSELH3_MASK              (0x01U << ADC_ANYCFG_CHANYSELH3_SHIFT)
#define ADC_ANYCFG_CHANYSELH3(x)                (((uint32_t)(((uint32_t)(x)) << ADC_ANYCFG_CHANYSELH3_SHIFT)) & ADC_ANYCFG_CHANYSELH3_MASK)

#define ADC_ANYCFG_CHANYSELH2_SHIFT             (10)
#define ADC_ANYCFG_CHANYSELH2_MASK              (0x01U << ADC_ANYCFG_CHANYSELH2_SHIFT)
#define ADC_ANYCFG_CHANYSELH2(x)                (((uint32_t)(((uint32_t)(x)) << ADC_ANYCFG_CHANYSELH2_SHIFT)) & ADC_ANYCFG_CHANYSELH2_MASK)

#define ADC_ANYCFG_CHANYSELH1_SHIFT             (9)
#define ADC_ANYCFG_CHANYSELH1_MASK              (0x01U << ADC_ANYCFG_CHANYSELH1_SHIFT)
#define ADC_ANYCFG_CHANYSELH1(x)                (((uint32_t)(((uint32_t)(x)) << ADC_ANYCFG_CHANYSELH1_SHIFT)) & ADC_ANYCFG_CHANYSELH1_MASK)

#define ADC_ANYCFG_CHANYSELH0_SHIFT             (8)
#define ADC_ANYCFG_CHANYSELH0_MASK              (0x01U << ADC_ANYCFG_CHANYSELH0_SHIFT)
#define ADC_ANYCFG_CHANYSELH0(x)                (((uint32_t)(((uint32_t)(x)) << ADC_ANYCFG_CHANYSELH0_SHIFT)) & ADC_ANYCFG_CHANYSELH0_MASK)

#define ADC_ANYCFG_CHANYNUM_SHIFT               (0)
#define ADC_ANYCFG_CHANYNUM_MASK                (0xFU << ADC_ANYCFG_CHANYNUM_SHIFT)
#define ADC_ANYCFG_CHANYNUM(x)                  (((uint32_t)(((uint32_t)(x)) << ADC_ANYCFG_CHANYNUM_SHIFT)) & ADC_ANYCFG_CHANYNUM_MASK)

/*!
 * @brief ADC_ANY_CR Register Bit Definition
 */

#define ADC_ANYCR_EOCALIE_SHIFT                 (21)
#define ADC_ANYCR_EOCALIE_MASK                  (0x01U << ADC_ANYCR_EOCALIE_SHIFT)
#define ADC_ANYCR_EOCALIE(x)                    (((uint32_t)(((uint32_t)(x)) << ADC_ANYCR_EOCALIE_SHIFT)) & ADC_ANYCR_EOCALIE_MASK)

#define ADC_ANYCR_ADCAL_SHIFT                   (20)
#define ADC_ANYCR_ADCAL_MASK                    (0x01U << ADC_ANYCR_ADCAL_SHIFT)
#define ADC_ANYCR_ADCAL(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ANYCR_ADCAL_SHIFT)) & ADC_ANYCR_ADCAL_MASK)

#define ADC_ANYCR_JTRGEDGE_SHIFT                (16)
#define ADC_ANYCR_JTRGEDGE_MASK                 (0x3U << ADC_ANYCR_JTRGEDGE_SHIFT)
#define ADC_ANYCR_JTRGEDGE(x)                   (((uint32_t)(((uint32_t)(x)) << ADC_ANYCR_JTRGEDGE_SHIFT)) & ADC_ANYCR_JTRGEDGE_MASK)

#define ADC_ANYCR_JTRGSHIFT_SHIFT               (13)
#define ADC_ANYCR_JTRGSHIFT_MASK                (0x7U << ADC_ANYCR_JTRGSHIFT_SHIFT)
#define ADC_ANYCR_JTRGSHIFT(x)                  (((uint32_t)(((uint32_t)(x)) << ADC_ANYCR_JTRGSHIFT_SHIFT)) & ADC_ANYCR_JTRGSHIFT_MASK)

#define ADC_ANYCR_JTRGEN_SHIFT                  (7)
#define ADC_ANYCR_JTRGEN_MASK                   (0x01U << ADC_ANYCR_JTRGEN_SHIFT)
#define ADC_ANYCR_JTRGEN(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ANYCR_JTRGEN_SHIFT)) & ADC_ANYCR_JTRGEN_MASK)

#define ADC_ANYCR_JADST_SHIFT                   (6)
#define ADC_ANYCR_JADST_MASK                    (0x01U << ADC_ANYCR_JADST_SHIFT)
#define ADC_ANYCR_JADST(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ANYCR_JADST_SHIFT)) & ADC_ANYCR_JADST_MASK)

#define ADC_ANYCR_JAUTO_SHIFT                   (5)
#define ADC_ANYCR_JAUTO_MASK                    (0x01U << ADC_ANYCR_JAUTO_SHIFT)
#define ADC_ANYCR_JAUTO(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ANYCR_JAUTO_SHIFT)) & ADC_ANYCR_JAUTO_MASK)

#define ADC_ANYCR_JEOSIE_SHIFT                  (4)
#define ADC_ANYCR_JEOSIE_MASK                   (0x01U << ADC_ANYCR_JEOSIE_SHIFT)
#define ADC_ANYCR_JEOSIE(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ANYCR_JEOSIE_SHIFT)) & ADC_ANYCR_JEOSIE_MASK)

#define ADC_ANYCR_JEOCIE_SHIFT                  (3)
#define ADC_ANYCR_JEOCIE_MASK                   (0x01U << ADC_ANYCR_JEOCIE_SHIFT)
#define ADC_ANYCR_JEOCIE(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ANYCR_JEOCIE_SHIFT)) & ADC_ANYCR_JEOCIE_MASK)

#define ADC_ANYCR_JEOSMPIE_SHIFT                (2)
#define ADC_ANYCR_JEOSMPIE_MASK                 (0x01U << ADC_ANYCR_JEOSMPIE_SHIFT)
#define ADC_ANYCR_JEOSMPIE(x)                   (((uint32_t)(((uint32_t)(x)) << ADC_ANYCR_JEOSMPIE_SHIFT)) & ADC_ANYCR_JEOSMPIE_MASK)

#define ADC_ANYCR_JCEN_SHIFT                    (1)
#define ADC_ANYCR_JCEN_MASK                     (0x01U << ADC_ANYCR_JCEN_SHIFT)
#define ADC_ANYCR_JCEN(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_ANYCR_JCEN_SHIFT)) & ADC_ANYCR_JCEN_MASK)

#define ADC_ANYCR_CHANYMDEN_SHIFT               (0)
#define ADC_ANYCR_CHANYMDEN_MASK                (0x01U << ADC_ANYCR_CHANYMDEN_SHIFT)
#define ADC_ANYCR_CHANYMDEN(x)                  (((uint32_t)(((uint32_t)(x)) << ADC_ANYCR_CHANYMDEN_SHIFT)) & ADC_ANYCR_CHANYMDEN_MASK)

/*!
 * @brief ADC_ADCFG2 Register Bit Definition
 */

#define ADC_ADCFG2_TROVSM_SHIFT                  (26)
#define ADC_ADCFG2_TROVSM_MASK                   (0x01U << ADC_ADCFG2_TROVSM_SHIFT)
#define ADC_ADCFG2_TROVSM(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ADCFG2_TROVSM_SHIFT)) & ADC_ADCFG2_TROVSM_MASK)

#define ADC_ADCFG2_TROVS_SHIFT                  (25)
#define ADC_ADCFG2_TROVS_MASK                   (0x01U << ADC_ADCFG2_TROVS_SHIFT)
#define ADC_ADCFG2_TROVS(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ADCFG2_TROVS_SHIFT)) & ADC_ADCFG2_TROVS_MASK)

#define ADC_ADCFG2_OVSS_SHIFT                   (21)
#define ADC_ADCFG2_OVSS_MASK                    (0xFU << ADC_ADCFG2_OVSS_SHIFT)
#define ADC_ADCFG2_OVSS(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADCFG2_OVSS_SHIFT)) & ADC_ADCFG2_OVSS_MASK)

#define ADC_ADCFG2_OVSR_SHIFT                   (18)
#define ADC_ADCFG2_OVSR_MASK                    (0x7U << ADC_ADCFG2_OVSR_SHIFT)
#define ADC_ADCFG2_OVSR(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADCFG2_OVSR_SHIFT)) & ADC_ADCFG2_OVSR_MASK)

#define ADC_ADCFG2_JOVSE_SHIFT                  (17)
#define ADC_ADCFG2_JOVSE_MASK                   (0x01U << ADC_ADCFG2_JOVSE_SHIFT)
#define ADC_ADCFG2_JOVSE(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ADCFG2_JOVSE_SHIFT)) & ADC_ADCFG2_JOVSE_MASK)

#define ADC_ADCFG2_ROVSE_SHIFT                  (16)
#define ADC_ADCFG2_ROVSE_MASK                   (0x01U << ADC_ADCFG2_ROVSE_SHIFT)
#define ADC_ADCFG2_ROVSE(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ADCFG2_ROVSE_SHIFT)) & ADC_ADCFG2_ROVSE_MASK)

#define ADC_ADCFG2_PSDC_SHIFT                   (10)
#define ADC_ADCFG2_PSDC_MASK                    (0x1FU << ADC_ADCFG2_PSDC_SHIFT)
#define ADC_ADCFG2_PSDC(x)                      (((uint32_t)(((uint32_t)(x)) << ADC_ADCFG2_PSDC_SHIFT)) & ADC_ADCFG2_PSDC_MASK)

#define ADC_ADCFG2_DC_SHIFT                     (4)
#define ADC_ADCFG2_DC_MASK                      (0x1FU << ADC_ADCFG2_DC_SHIFT)
#define ADC_ADCFG2_DC(x)                        (((uint32_t)(((uint32_t)(x)) << ADC_ADCFG2_DC_SHIFT)) & ADC_ADCFG2_DC_MASK)

#define ADC_ADCFG2_ADCSREF_SHIFT                (2)
#define ADC_ADCFG2_ADCSREF_MASK                 (0x01U << ADC_ADCFG2_ADCSREF_SHIFT)
#define ADC_ADCFG2_ADCSREF(x)                   (((uint32_t)(((uint32_t)(x)) << ADC_ADCFG2_ADCSREF_SHIFT)) & ADC_ADCFG2_ADCSREF_MASK)

#define ADC_ADCFG2_ADCCR_SHIFT                  (1)
#define ADC_ADCFG2_ADCCR_MASK                   (0x01U << ADC_ADCFG2_ADCCR_SHIFT)
#define ADC_ADCFG2_ADCCR(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_ADCFG2_ADCCR_SHIFT)) & ADC_ADCFG2_ADCCR_MASK)

/*!
 * @brief ADC_SMPR1 Register Bit Definition
 */

#define ADC_SMPR1_SAMP_SHIFT                    (0)
#define ADC_SMPR1_SAMP_MASK                     (0xFFFFFFFFU << ADC_SMPR1_SAMP_SHIFT)
#define ADC_SMPR1_SAMP(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_SMPR1_SAMP_SHIFT)) & ADC_SMPR1_SAMP_MASK)

/*!
 * @brief ADC_SMPR2 Register Bit Definition
 */

#define ADC_SMPR2_SAMP_SHIFT                    (0)
#define ADC_SMPR2_SAMP_MASK                     (0xFFFFFFFFU << ADC_SMPR2_SAMP_SHIFT)
#define ADC_SMPR2_SAMP(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_SMPR2_SAMP_SHIFT)) & ADC_SMPR2_SAMP_MASK)

/*!
 * @brief ADC_SMPR3 Register Bit Definition
 */

#define ADC_SMPR3_SAMP_SHIFT                    (0)
#define ADC_SMPR3_SAMP_MASK                     (0xFFFU << ADC_SMPR3_SAMP_SHIFT)
#define ADC_SMPR3_SAMP(x)                       (((uint32_t)(((uint32_t)(x)) << ADC_SMPR3_SAMP_SHIFT)) & ADC_SMPR3_SAMP_MASK)

/*!
 * @brief ADC_JOFR Register Bit Definition
 */

#define ADC_JOFR_JOFFSET_SHIFT                  (0)
#define ADC_JOFR_JOFFSET_MASK                   (0xFFFU << ADC_JOFR_JOFFSET_SHIFT)
#define ADC_JOFR_JOFFSET(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_JOFR_JOFFSET_SHIFT)) & ADC_JOFR_JOFFSET_MASK)

/*!
 * @brief ADC_JSQR Register Bit Definition
 */

#define ADC_JSQR_JNUM_SHIFT                     (20)
#define ADC_JSQR_JNUM_MASK                      (0x3U << ADC_JSQR_JNUM_SHIFT)
#define ADC_JSQR_JNUM(x)                        (((uint32_t)(((uint32_t)(x)) << ADC_JSQR_JNUM_SHIFT)) & ADC_JSQR_JNUM_MASK)

#define ADC_JSQR_JSQ3_SHIFT                     (15)
#define ADC_JSQR_JSQ3_MASK                      (0x1FU << ADC_JSQR_JSQ3_SHIFT)
#define ADC_JSQR_JSQ3(x)                        (((uint32_t)(((uint32_t)(x)) << ADC_JSQR_JSQ3_SHIFT)) & ADC_JSQR_JSQ3_MASK)

#define ADC_JSQR_JSQ2_SHIFT                     (10)
#define ADC_JSQR_JSQ2_MASK                      (0x1FU << ADC_JSQR_JSQ2_SHIFT)
#define ADC_JSQR_JSQ2(x)                        (((uint32_t)(((uint32_t)(x)) << ADC_JSQR_JSQ2_SHIFT)) & ADC_JSQR_JSQ2_MASK)

#define ADC_JSQR_JSQ1_SHIFT                     (5)
#define ADC_JSQR_JSQ1_MASK                      (0x1FU << ADC_JSQR_JSQ1_SHIFT)
#define ADC_JSQR_JSQ1(x)                        (((uint32_t)(((uint32_t)(x)) << ADC_JSQR_JSQ1_SHIFT)) & ADC_JSQR_JSQ1_MASK)

#define ADC_JSQR_JSQ0_SHIFT                     (0)
#define ADC_JSQR_JSQ0_MASK                      (0x1FU << ADC_JSQR_JSQ0_SHIFT)
#define ADC_JSQR_JSQ0(x)                        (((uint32_t)(((uint32_t)(x)) << ADC_JSQR_JSQ0_SHIFT)) & ADC_JSQR_JSQ0_MASK)

/*!
 * @brief ADC_JADDATA Register Bit Definition
 */

#define ADC_JADDATA_JVALID_SHIFT                (22)
#define ADC_JADDATA_JVALID_MASK                 (0x01U << ADC_JADDATA_JVALID_SHIFT)
#define ADC_JADDATA_JVALID(x)                   (((uint32_t)(((uint32_t)(x)) << ADC_JADDATA_JVALID_SHIFT)) & ADC_JADDATA_JVALID_MASK)

#define ADC_JADDATA_JOVERRUN_SHIFT              (21)
#define ADC_JADDATA_JOVERRUN_MASK               (0x01U << ADC_JADDATA_JOVERRUN_SHIFT)
#define ADC_JADDATA_JOVERRUN(x)                 (((uint32_t)(((uint32_t)(x)) << ADC_JADDATA_JOVERRUN_SHIFT)) & ADC_JADDATA_JOVERRUN_MASK)

#define ADC_JADDATA_JCHANNELSEL_SHIFT           (16)
#define ADC_JADDATA_JCHANNELSEL_MASK            (0x1FU << ADC_JADDATA_JCHANNELSEL_SHIFT)
#define ADC_JADDATA_JCHANNELSEL(x)              (((uint32_t)(((uint32_t)(x)) << ADC_JADDATA_JCHANNELSEL_SHIFT)) & ADC_JADDATA_JCHANNELSEL_MASK)

#define ADC_JADDATA_JDATA_SHIFT                 (0)
#define ADC_JADDATA_JDATA_MASK                  (0xFFFFU << ADC_JADDATA_JDATA_SHIFT)
#define ADC_JADDATA_JDATA(x)                    (((uint32_t)(((uint32_t)(x)) << ADC_JADDATA_JDATA_SHIFT)) & ADC_JADDATA_JDATA_MASK)

/*!
 * @brief ADC_JDR Register Bit Definition
 */

#define ADC_JDR_JVALID_SHIFT                 (22)
#define ADC_JDR_JVALID_MASK                  (0x01U << ADC_JDR_JVALID_SHIFT)
#define ADC_JDR_JVALID(x)                    (((uint32_t)(((uint32_t)(x)) << ADC_JDR_JVALID_SHIFT)) & ADC_JDR_JVALID_MASK)

#define ADC_JDR_JOVERRUN_SHIFT               (21)
#define ADC_JDR_JOVERRUN_MASK                (0x01U << ADC_JDR_JOVERRUN_SHIFT)
#define ADC_JDR_JOVERRUN(x)                  (((uint32_t)(((uint32_t)(x)) << ADC_JDR_JOVERRUN_SHIFT)) & ADC_JDR_JOVERRUN_MASK)

#define ADC_JDR_JDATA_SHIFT                  (0)
#define ADC_JDR_JDATA_MASK                   (0xFFFFU << ADC_JDR_JDATA_SHIFT)
#define ADC_JDR_JDATA(x)                     (((uint32_t)(((uint32_t)(x)) << ADC_JDR_JDATA_SHIFT)) & ADC_JDR_JDATA_MASK)

/*!
 * @}
 */ /* end of group ADC_Register_Masks */
/******************************************************************************
 *ADC Instance
*******************************************************************************/

#define ADC1                  ((ADC_Type*)ADC1_BASE)
#define ADC2                  ((ADC_Type*)ADC2_BASE)

/*!
 * @}
 */ /* end of group ADC_Peripheral_Access_Layer */

/*!
 * @addtogroup BKP_Peripheral_Access_Layer BKP Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * BKP Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t Reserved2[16];                                                  ///< Reserved
    __IO uint32_t RTCCR;                                                          ///< RTC Clock Calibration Register               offset: 0x40
    __IO uint32_t CR;                                                             ///< Backup Control Register                      offset: 0x44
    __IO uint32_t CSR;                                                            ///< Backup Control Status Register               offset: 0x48
    __IO uint32_t Reserved3[1];                                                   ///< Reserved
    __IO uint32_t DR[20];                                                         ///< Backup data register n                       offset: 0x50
} BKP_Type;

/*******************************************************************************
 * BKP Type
 ******************************************************************************/

/*!
 * @addtogroup BKP_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief BKP_RTCCR Register Bit Definition
 */

#define BKP_RTCCR_ASOS_SHIFT                    (9)
#define BKP_RTCCR_ASOS_MASK                     (0x01U << BKP_RTCCR_ASOS_SHIFT)
#define BKP_RTCCR_ASOS(x)                       (((uint32_t)(((uint32_t)(x)) << BKP_RTCCR_ASOS_SHIFT)) & BKP_RTCCR_ASOS_MASK)

#define BKP_RTCCR_ASOE_SHIFT                    (8)
#define BKP_RTCCR_ASOE_MASK                     (0x01U << BKP_RTCCR_ASOE_SHIFT)
#define BKP_RTCCR_ASOE(x)                       (((uint32_t)(((uint32_t)(x)) << BKP_RTCCR_ASOE_SHIFT)) & BKP_RTCCR_ASOE_MASK)

#define BKP_RTCCR_CCO_SHIFT                     (7)
#define BKP_RTCCR_CCO_MASK                      (0x01U << BKP_RTCCR_CCO_SHIFT)
#define BKP_RTCCR_CCO(x)                        (((uint32_t)(((uint32_t)(x)) << BKP_RTCCR_CCO_SHIFT)) & BKP_RTCCR_CCO_MASK)

#define BKP_RTCCR_CAL_SHIFT                     (0)
#define BKP_RTCCR_CAL_MASK                      (0x7FU << BKP_RTCCR_CAL_SHIFT)
#define BKP_RTCCR_CAL(x)                        (((uint32_t)(((uint32_t)(x)) << BKP_RTCCR_CAL_SHIFT)) & BKP_RTCCR_CAL_MASK)

/*!
 * @brief BKP_CR Register Bit Definition
 */

#define BKP_CR_TPAL_SHIFT                       (1)
#define BKP_CR_TPAL_MASK                        (0x01U << BKP_CR_TPAL_SHIFT)
#define BKP_CR_TPAL(x)                          (((uint32_t)(((uint32_t)(x)) << BKP_CR_TPAL_SHIFT)) & BKP_CR_TPAL_MASK)

#define BKP_CR_TPE_SHIFT                        (0)
#define BKP_CR_TPE_MASK                         (0x01U << BKP_CR_TPE_SHIFT)
#define BKP_CR_TPE(x)                           (((uint32_t)(((uint32_t)(x)) << BKP_CR_TPE_SHIFT)) & BKP_CR_TPE_MASK)

/*!
 * @brief BKP_CSR Register Bit Definition
 */

#define BKP_CSR_TIF_SHIFT                       (9)
#define BKP_CSR_TIF_MASK                        (0x01U << BKP_CSR_TIF_SHIFT)
#define BKP_CSR_TIF(x)                          (((uint32_t)(((uint32_t)(x)) << BKP_CSR_TIF_SHIFT)) & BKP_CSR_TIF_MASK)

#define BKP_CSR_TEF_SHIFT                       (8)
#define BKP_CSR_TEF_MASK                        (0x01U << BKP_CSR_TEF_SHIFT)
#define BKP_CSR_TEF(x)                          (((uint32_t)(((uint32_t)(x)) << BKP_CSR_TEF_SHIFT)) & BKP_CSR_TEF_MASK)

#define BKP_CSR_TPIE_SHIFT                      (2)
#define BKP_CSR_TPIE_MASK                       (0x01U << BKP_CSR_TPIE_SHIFT)
#define BKP_CSR_TPIE(x)                         (((uint32_t)(((uint32_t)(x)) << BKP_CSR_TPIE_SHIFT)) & BKP_CSR_TPIE_MASK)

#define BKP_CSR_CTI_SHIFT                       (1)
#define BKP_CSR_CTI_MASK                        (0x01U << BKP_CSR_CTI_SHIFT)
#define BKP_CSR_CTI(x)                          (((uint32_t)(((uint32_t)(x)) << BKP_CSR_CTI_SHIFT)) & BKP_CSR_CTI_MASK)

#define BKP_CSR_CTE_SHIFT                       (0)
#define BKP_CSR_CTE_MASK                        (0x01U << BKP_CSR_CTE_SHIFT)
#define BKP_CSR_CTE(x)                          (((uint32_t)(((uint32_t)(x)) << BKP_CSR_CTE_SHIFT)) & BKP_CSR_CTE_MASK)

/*!
 * @brief BKP_DR Register Bit Definition
 */

#define BKP_DR_BKP_SHIFT                        (0)
#define BKP_DR_BKP_MASK                         (0xFFFFU << BKP_DR_BKP_SHIFT)
#define BKP_DR_BKP(x)                           (((uint32_t)(((uint32_t)(x)) << BKP_DR_BKP_SHIFT)) & BKP_DR_BKP_MASK)

/*!
 * @}
 */ /* end of group BKP_Register_Masks */
/******************************************************************************
 *BKP Instance
*******************************************************************************/

#define BKP                  ((BKP_Type*)RTC_BKP_BASE)

/*!
 * @}
 */ /* end of group BKP_Peripheral_Access_Layer */

/*!
 * @addtogroup COMP_Peripheral_Access_Layer COMP Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * COMP Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t Reserved4[3];                                                   ///< Reserved
    __IO uint32_t CSR[2];                                                         ///< Comparator x (x=1,2) Control and Status Registeroffset: 0x0C
    __IO uint32_t Reserved5[1];                                                   ///< Reserved
    __IO uint32_t CRV;                                                            ///< Comparator External Reference Voltage Registeroffset: 0x18
    __IO uint32_t POLL[3];                                                        ///< Comparator x (x=1,2,3) polling register    offset: 0x1C
    __IO uint32_t Reserved6[1];                                                   ///< Reserved
    __IO uint32_t CSR3;                                                           ///< Comparator x (x=3) Control and Status Registeroffset: 0x0C
} COMP_Type;

/*******************************************************************************
 * COMP Type
 ******************************************************************************/

/*!
 * @addtogroup COMP_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief COMP_CSR Register Bit Definition
 */

#define COMP_CSR_LOCK_SHIFT                     (31)
#define COMP_CSR_LOCK_MASK                      (0x01U << COMP_CSR_LOCK_SHIFT)
#define COMP_CSR_LOCK(x)                        (((uint32_t)(((uint32_t)(x)) << COMP_CSR_LOCK_SHIFT)) & COMP_CSR_LOCK_MASK)

#define COMP_CSR_OUT_SHIFT                      (30)
#define COMP_CSR_OUT_MASK                       (0x01U << COMP_CSR_OUT_SHIFT)
#define COMP_CSR_OUT(x)                         (((uint32_t)(((uint32_t)(x)) << COMP_CSR_OUT_SHIFT)) & COMP_CSR_OUT_MASK)

#define COMP_CSR_OUTANASEL_SHIFT                (29)
#define COMP_CSR_OUTANASEL_MASK                 (0x01U << COMP_CSR_OUTANASEL_SHIFT)
#define COMP_CSR_OUTANASEL(x)                   (((uint32_t)(((uint32_t)(x)) << COMP_CSR_OUTANASEL_SHIFT)) & COMP_CSR_OUTANASEL_MASK)

#define COMP_CSR_WE_SHIFT                       (28)
#define COMP_CSR_WE_MASK                        (0x01U << COMP_CSR_WE_SHIFT)
#define COMP_CSR_WE(x)                          (((uint32_t)(((uint32_t)(x)) << COMP_CSR_WE_SHIFT)) & COMP_CSR_WE_MASK)

#define COMP_CSR_OFLT_SHIFT                     (18)
#define COMP_CSR_OFLT_MASK                      (0x7U << COMP_CSR_OFLT_SHIFT)
#define COMP_CSR_OFLT(x)                        (((uint32_t)(((uint32_t)(x)) << COMP_CSR_OFLT_SHIFT)) & COMP_CSR_OFLT_MASK)

#define COMP_CSR_HYST_SHIFT                     (16)
#define COMP_CSR_HYST_MASK                      (0x3U << COMP_CSR_HYST_SHIFT)
#define COMP_CSR_HYST(x)                        (((uint32_t)(((uint32_t)(x)) << COMP_CSR_HYST_SHIFT)) & COMP_CSR_HYST_MASK)

#define COMP_CSR_POL_SHIFT                      (15)
#define COMP_CSR_POL_MASK                       (0x01U << COMP_CSR_POL_SHIFT)
#define COMP_CSR_POL(x)                         (((uint32_t)(((uint32_t)(x)) << COMP_CSR_POL_SHIFT)) & COMP_CSR_POL_MASK)

#define COMP_CSR_OUTSEL_SHIFT                   (10)
#define COMP_CSR_OUTSEL_MASK                    (0xFU << COMP_CSR_OUTSEL_SHIFT)
#define COMP_CSR_OUTSEL(x)                      (((uint32_t)(((uint32_t)(x)) << COMP_CSR_OUTSEL_SHIFT)) & COMP_CSR_OUTSEL_MASK)

#define COMP_CSR_INPSEL_SHIFT                   (7)
#define COMP_CSR_INPSEL_MASK                    (0x3U << COMP_CSR_INPSEL_SHIFT)
#define COMP_CSR_INPSEL(x)                      (((uint32_t)(((uint32_t)(x)) << COMP_CSR_INPSEL_SHIFT)) & COMP_CSR_INPSEL_MASK)

#define COMP_CSR_INMSEL_SHIFT                   (4)
#define COMP_CSR_INMSEL_MASK                    (0x3U << COMP_CSR_INMSEL_SHIFT)
#define COMP_CSR_INMSEL(x)                      (((uint32_t)(((uint32_t)(x)) << COMP_CSR_INMSEL_SHIFT)) & COMP_CSR_INMSEL_MASK)

#define COMP_CSR_MODE_SHIFT                     (2)
#define COMP_CSR_MODE_MASK                      (0x3U << COMP_CSR_MODE_SHIFT)
#define COMP_CSR_MODE(x)                        (((uint32_t)(((uint32_t)(x)) << COMP_CSR_MODE_SHIFT)) & COMP_CSR_MODE_MASK)

#define COMP_CSR_EN_SHIFT                       (0)
#define COMP_CSR_EN_MASK                        (0x01U << COMP_CSR_EN_SHIFT)
#define COMP_CSR_EN(x)                          (((uint32_t)(((uint32_t)(x)) << COMP_CSR_EN_SHIFT)) & COMP_CSR_EN_MASK)

/*!
 * @brief COMP_CRV Register Bit Definition
 */

#define COMP_CRV_CRVSRC_SHIFT                   (5)
#define COMP_CRV_CRVSRC_MASK                    (0x01U << COMP_CRV_CRVSRC_SHIFT)
#define COMP_CRV_CRVSRC(x)                      (((uint32_t)(((uint32_t)(x)) << COMP_CRV_CRVSRC_SHIFT)) & COMP_CRV_CRVSRC_MASK)

#define COMP_CRV_CRVEN_SHIFT                    (4)
#define COMP_CRV_CRVEN_MASK                     (0x01U << COMP_CRV_CRVEN_SHIFT)
#define COMP_CRV_CRVEN(x)                       (((uint32_t)(((uint32_t)(x)) << COMP_CRV_CRVEN_SHIFT)) & COMP_CRV_CRVEN_MASK)

#define COMP_CRV_CRVSEL_SHIFT                   (0)
#define COMP_CRV_CRVSEL_MASK                    (0xFU << COMP_CRV_CRVSEL_SHIFT)
#define COMP_CRV_CRVSEL(x)                      (((uint32_t)(((uint32_t)(x)) << COMP_CRV_CRVSEL_SHIFT)) & COMP_CRV_CRVSEL_MASK)

/*!
 * @brief COMP_POLL Register Bit Definition
 */

#define COMP_POLL_POUT_SHIFT                    (8)
#define COMP_POLL_POUT_MASK                     (0x7U << COMP_POLL_POUT_SHIFT)
#define COMP_POLL_POUT(x)                       (((uint32_t)(((uint32_t)(x)) << COMP_POLL_POUT_SHIFT)) & COMP_POLL_POUT_MASK)

#define COMP_POLL_PERIOD_SHIFT                  (4)
#define COMP_POLL_PERIOD_MASK                   (0x7U << COMP_POLL_PERIOD_SHIFT)
#define COMP_POLL_PERIOD(x)                     (((uint32_t)(((uint32_t)(x)) << COMP_POLL_PERIOD_SHIFT)) & COMP_POLL_PERIOD_MASK)

#define COMP_POLL_FIXN_SHIFT                    (2)
#define COMP_POLL_FIXN_MASK                     (0x01U << COMP_POLL_FIXN_SHIFT)
#define COMP_POLL_FIXN(x)                       (((uint32_t)(((uint32_t)(x)) << COMP_POLL_FIXN_SHIFT)) & COMP_POLL_FIXN_MASK)

#define COMP_POLL_POLLCH_SHIFT                  (1)
#define COMP_POLL_POLLCH_MASK                   (0x01U << COMP_POLL_POLLCH_SHIFT)
#define COMP_POLL_POLLCH(x)                     (((uint32_t)(((uint32_t)(x)) << COMP_POLL_POLLCH_SHIFT)) & COMP_POLL_POLLCH_MASK)

#define COMP_POLL_POLLEN_SHIFT                  (0)
#define COMP_POLL_POLLEN_MASK                   (0x01U << COMP_POLL_POLLEN_SHIFT)
#define COMP_POLL_POLLEN(x)                     (((uint32_t)(((uint32_t)(x)) << COMP_POLL_POLLEN_SHIFT)) & COMP_POLL_POLLEN_MASK)

/*!
 * @}
 */ /* end of group COMP_Register_Masks */
/******************************************************************************
 *COMP Instance
*******************************************************************************/

#define COMP                 ((COMP_Type*)COMP_BASE)

/*!
 * @}
 */ /* end of group COMP_Peripheral_Access_Layer */

/*!
 * @addtogroup CORDIC_Peripheral_Access_Layer CORDIC Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * CORDIC Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t DXR;                                                            ///< CORDIC X Data Register                       offset: 0x00
    __IO uint32_t DYR;                                                            ///< CORDIC Y data register                       offset: 0x04
    __IO uint32_t DZR;                                                            ///< CORDIC Z data register                       offset: 0x08
    __IO uint32_t RXR;                                                            ///< CORDIC X Result Register                     offset: 0x0C
    __IO uint32_t RYR;                                                            ///< CORDIC Y result register                     offset: 0x10
    __IO uint32_t RZR;                                                            ///< CORDIC Z result register                     offset: 0x14
    __IO uint32_t CR;                                                             ///< CORDIC Control Register                      offset: 0x18
    __IO uint32_t SR;                                                             ///< CORDIC Status Register                       offset: 0x1C
} CORDIC_Type;

/*******************************************************************************
 * CORDIC Type
 ******************************************************************************/

/*!
 * @addtogroup CORDIC_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief CORDIC_DXR Register Bit Definition
 */

#define CORDIC_DXR_DX_SHIFT                     (0)
#define CORDIC_DXR_DX_MASK                      (0xFFFFFFU << CORDIC_DXR_DX_SHIFT)
#define CORDIC_DXR_DX(x)                        (((uint32_t)(((uint32_t)(x)) << CORDIC_DXR_DX_SHIFT)) & CORDIC_DXR_DX_MASK)

/*!
 * @brief CORDIC_DYR Register Bit Definition
 */

#define CORDIC_DYR_DY_SHIFT                     (0)
#define CORDIC_DYR_DY_MASK                      (0xFFFFFFU << CORDIC_DYR_DY_SHIFT)
#define CORDIC_DYR_DY(x)                        (((uint32_t)(((uint32_t)(x)) << CORDIC_DYR_DY_SHIFT)) & CORDIC_DYR_DY_MASK)

/*!
 * @brief CORDIC_DZR Register Bit Definition
 */

#define CORDIC_DZR_DZ_SHIFT                     (0)
#define CORDIC_DZR_DZ_MASK                      (0xFFFFFFU << CORDIC_DZR_DZ_SHIFT)
#define CORDIC_DZR_DZ(x)                        (((uint32_t)(((uint32_t)(x)) << CORDIC_DZR_DZ_SHIFT)) & CORDIC_DZR_DZ_MASK)

/*!
 * @brief CORDIC_RXR Register Bit Definition
 */

#define CORDIC_RXR_RX_SHIFT                     (0)
#define CORDIC_RXR_RX_MASK                      (0x1FFFFFFU << CORDIC_RXR_RX_SHIFT)
#define CORDIC_RXR_RX(x)                        (((uint32_t)(((uint32_t)(x)) << CORDIC_RXR_RX_SHIFT)) & CORDIC_RXR_RX_MASK)

/*!
 * @brief CORDIC_RYR Register Bit Definition
 */

#define CORDIC_RYR_RY_SHIFT                     (0)
#define CORDIC_RYR_RY_MASK                      (0x1FFFFFFU << CORDIC_RYR_RY_SHIFT)
#define CORDIC_RYR_RY(x)                        (((uint32_t)(((uint32_t)(x)) << CORDIC_RYR_RY_SHIFT)) & CORDIC_RYR_RY_MASK)

/*!
 * @brief CORDIC_RZR Register Bit Definition
 */

#define CORDIC_RZR_RZ_SHIFT                     (0)
#define CORDIC_RZR_RZ_MASK                      (0xFFFFFFU << CORDIC_RZR_RZ_SHIFT)
#define CORDIC_RZR_RZ(x)                        (((uint32_t)(((uint32_t)(x)) << CORDIC_RZR_RZ_SHIFT)) & CORDIC_RZR_RZ_MASK)

/*!
 * @brief CORDIC_CR Register Bit Definition
 */

#define CORDIC_CR_ENABLE_SHIFT                  (3)
#define CORDIC_CR_ENABLE_MASK                   (0x01U << CORDIC_CR_ENABLE_SHIFT)
#define CORDIC_CR_ENABLE(x)                     (((uint32_t)(((uint32_t)(x)) << CORDIC_CR_ENABLE_SHIFT)) & CORDIC_CR_ENABLE_MASK)

#define CORDIC_CR_STARTMODE_SHIFT               (2)
#define CORDIC_CR_STARTMODE_MASK                (0x01U << CORDIC_CR_STARTMODE_SHIFT)
#define CORDIC_CR_STARTMODE(x)                  (((uint32_t)(((uint32_t)(x)) << CORDIC_CR_STARTMODE_SHIFT)) & CORDIC_CR_STARTMODE_MASK)

#define CORDIC_CR_WORKMODE_SHIFT                (1)
#define CORDIC_CR_WORKMODE_MASK                 (0x01U << CORDIC_CR_WORKMODE_SHIFT)
#define CORDIC_CR_WORKMODE(x)                   (((uint32_t)(((uint32_t)(x)) << CORDIC_CR_WORKMODE_SHIFT)) & CORDIC_CR_WORKMODE_MASK)

#define CORDIC_CR_START_SHIFT                   (0)
#define CORDIC_CR_START_MASK                    (0x01U << CORDIC_CR_START_SHIFT)
#define CORDIC_CR_START(x)                      (((uint32_t)(((uint32_t)(x)) << CORDIC_CR_START_SHIFT)) & CORDIC_CR_START_MASK)

/*!
 * @brief CORDIC_SR Register Bit Definition
 */

#define CORDIC_SR_BSY_SHIFT                     (0)
#define CORDIC_SR_BSY_MASK                      (0x01U << CORDIC_SR_BSY_SHIFT)
#define CORDIC_SR_BSY(x)                        (((uint32_t)(((uint32_t)(x)) << CORDIC_SR_BSY_SHIFT)) & CORDIC_SR_BSY_MASK)

/*!
 * @}
 */ /* end of group CORDIC_Register_Masks */
/******************************************************************************
 *CORDIC Instance
*******************************************************************************/

#define CORDIC               ((CORDIC_Type*)CORDIC_BASE)

/*!
 * @}
 */ /* end of group CORDIC_Peripheral_Access_Layer */




/*!
 * @addtogroup CRC_Peripheral_Access_Layer CRC Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * CRC Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t DR;                                                             ///< CRC data register                            offset: 0x00
    __IO uint32_t IDR;                                                            ///< CRC independent data register                offset: 0x04
    __IO uint32_t CR;                                                             ///< CRC Control Register                         offset: 0x08
    __IO uint32_t IVR;                                                            ///< CRC initial value register                   offset: 0x0C
    __IO uint32_t PR;                                                             ///< CRC polynomial register                      offset: 0x10
} CRC_Type;

/*******************************************************************************
 * CRC Type
 ******************************************************************************/

/*!
 * @addtogroup CRC_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief CRC_DR Register Bit Definition
 */

#define CRC_DR_DR_SHIFT                         (0)
#define CRC_DR_DR_MASK                          (0xFFFFFFFFU << CRC_DR_DR_SHIFT)
#define CRC_DR_DR(x)                            (((uint32_t)(((uint32_t)(x)) << CRC_DR_DR_SHIFT)) & CRC_DR_DR_MASK)

/*!
 * @brief CRC_IDR Register Bit Definition
 */

#define CRC_IDR_IDR_SHIFT                       (0)
#define CRC_IDR_IDR_MASK                        (0xFFFFFFFFU << CRC_IDR_IDR_SHIFT)
#define CRC_IDR_IDR(x)                          (((uint32_t)(((uint32_t)(x)) << CRC_IDR_IDR_SHIFT)) & CRC_IDR_IDR_MASK)

/*!
 * @brief CRC_CR Register Bit Definition
 */

#define CRC_CR_PWIDTH_SHIFT                     (14)
#define CRC_CR_PWIDTH_MASK                      (0x3U << CRC_CR_PWIDTH_SHIFT)
#define CRC_CR_PWIDTH(x)                        (((uint32_t)(((uint32_t)(x)) << CRC_CR_PWIDTH_SHIFT)) & CRC_CR_PWIDTH_MASK)

#define CRC_CR_RO_SHIFT                         (6)
#define CRC_CR_RO_MASK                          (0x01U << CRC_CR_RO_SHIFT)
#define CRC_CR_RO(x)                            (((uint32_t)(((uint32_t)(x)) << CRC_CR_RO_SHIFT)) & CRC_CR_RO_MASK)

#define CRC_CR_BEO_SHIFT                        (5)
#define CRC_CR_BEO_MASK                         (0x01U << CRC_CR_BEO_SHIFT)
#define CRC_CR_BEO(x)                           (((uint32_t)(((uint32_t)(x)) << CRC_CR_BEO_SHIFT)) & CRC_CR_BEO_MASK)

#define CRC_CR_BEI_SHIFT                        (4)
#define CRC_CR_BEI_MASK                         (0x01U << CRC_CR_BEI_SHIFT)
#define CRC_CR_BEI(x)                           (((uint32_t)(((uint32_t)(x)) << CRC_CR_BEI_SHIFT)) & CRC_CR_BEI_MASK)

#define CRC_CR_RI_SHIFT                         (1)
#define CRC_CR_RI_MASK                          (0x01U << CRC_CR_RI_SHIFT)
#define CRC_CR_RI(x)                            (((uint32_t)(((uint32_t)(x)) << CRC_CR_RI_SHIFT)) & CRC_CR_RI_MASK)

#define CRC_CR_RST_SHIFT                        (0)
#define CRC_CR_RST_MASK                         (0x01U << CRC_CR_RST_SHIFT)
#define CRC_CR_RST(x)                           (((uint32_t)(((uint32_t)(x)) << CRC_CR_RST_SHIFT)) & CRC_CR_RST_MASK)

/*!
 * @brief CRC_IVR Register Bit Definition
 */

#define CRC_IVR_IVR_SHIFT                       (0)
#define CRC_IVR_IVR_MASK                        (0xFFFFFFFFU << CRC_IVR_IVR_SHIFT)
#define CRC_IVR_IVR(x)                          (((uint32_t)(((uint32_t)(x)) << CRC_IVR_IVR_SHIFT)) & CRC_IVR_IVR_MASK)

/*!
 * @brief CRC_PR Register Bit Definition
 */

#define CRC_PR_PR_SHIFT                        (0)
#define CRC_PR_PR_MASK                         (0xFFFFFFFFU << CRC_PR_PR_SHIFT)
#define CRC_PR_PR(x)                           (((uint32_t)(((uint32_t)(x)) << CRC_PR_PR_SHIFT)) & CRC_PR_PR_MASK)

/*!
 * @}
 */ /* end of group CRC_Register_Masks */
/******************************************************************************
 *CRC Instance
*******************************************************************************/

#define CRC                  ((CRC_Type*)CRC_BASE)

/*!
 * @}
 */ /* end of group CRC_Peripheral_Access_Layer */




/*!
 * @addtogroup CRS_Peripheral_Access_Layer CRS Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * CRS Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t CR;                                                             ///< CRS Control Register                         offset: 0x00
    __IO uint32_t CFGR;                                                           ///< CRS Configuration Register                   offset: 0x04
    __IO uint32_t LSR;                                                            ///< CRS Interrupt Status Register                offset: 0x08
    __IO uint32_t LCR;                                                            ///< CRS Interrupt Flag Clear Register            offset: 0x0C
} CRS_Type;

/*******************************************************************************
 * CRS Type
 ******************************************************************************/

/*!
 * @addtogroup CRS_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief CRS_CR Register Bit Definition
 */

#define CRS_CR_TRIM_SHIFT                       (8)
#define CRS_CR_TRIM_MASK                        (0x3FFU << CRS_CR_TRIM_SHIFT)
#define CRS_CR_TRIM(x)                          (((uint32_t)(((uint32_t)(x)) << CRS_CR_TRIM_SHIFT)) & CRS_CR_TRIM_MASK)

#define CRS_CR_SWSYNC_SHIFT                     (7)
#define CRS_CR_SWSYNC_MASK                      (0x01U << CRS_CR_SWSYNC_SHIFT)
#define CRS_CR_SWSYNC(x)                        (((uint32_t)(((uint32_t)(x)) << CRS_CR_SWSYNC_SHIFT)) & CRS_CR_SWSYNC_MASK)

#define CRS_CR_AUTOTRIMEN_SHIFT                 (6)
#define CRS_CR_AUTOTRIMEN_MASK                  (0x01U << CRS_CR_AUTOTRIMEN_SHIFT)
#define CRS_CR_AUTOTRIMEN(x)                    (((uint32_t)(((uint32_t)(x)) << CRS_CR_AUTOTRIMEN_SHIFT)) & CRS_CR_AUTOTRIMEN_MASK)

#define CRS_CR_CEN_SHIFT                        (5)
#define CRS_CR_CEN_MASK                         (0x01U << CRS_CR_CEN_SHIFT)
#define CRS_CR_CEN(x)                           (((uint32_t)(((uint32_t)(x)) << CRS_CR_CEN_SHIFT)) & CRS_CR_CEN_MASK)

#define CRS_CR_ESYNCIE_SHIFT                    (3)
#define CRS_CR_ESYNCIE_MASK                     (0x01U << CRS_CR_ESYNCIE_SHIFT)
#define CRS_CR_ESYNCIE(x)                       (((uint32_t)(((uint32_t)(x)) << CRS_CR_ESYNCIE_SHIFT)) & CRS_CR_ESYNCIE_MASK)

#define CRS_CR_ERRIE_SHIFT                      (2)
#define CRS_CR_ERRIE_MASK                       (0x01U << CRS_CR_ERRIE_SHIFT)
#define CRS_CR_ERRIE(x)                         (((uint32_t)(((uint32_t)(x)) << CRS_CR_ERRIE_SHIFT)) & CRS_CR_ERRIE_MASK)

#define CRS_CR_SYNCWARNIE_SHIFT                 (1)
#define CRS_CR_SYNCWARNIE_MASK                  (0x01U << CRS_CR_SYNCWARNIE_SHIFT)
#define CRS_CR_SYNCWARNIE(x)                    (((uint32_t)(((uint32_t)(x)) << CRS_CR_SYNCWARNIE_SHIFT)) & CRS_CR_SYNCWARNIE_MASK)

#define CRS_CR_SYNCOKIE_SHIFT                   (0)
#define CRS_CR_SYNCOKIE_MASK                    (0x01U << CRS_CR_SYNCOKIE_SHIFT)
#define CRS_CR_SYNCOKIE(x)                      (((uint32_t)(((uint32_t)(x)) << CRS_CR_SYNCOKIE_SHIFT)) & CRS_CR_SYNCOKIE_MASK)

/*!
 * @brief CRS_CFGR Register Bit Definition
 */

#define CRS_CFGR_SYNCPOL_SHIFT                  (31)
#define CRS_CFGR_SYNCPOL_MASK                   (0x01U << CRS_CFGR_SYNCPOL_SHIFT)
#define CRS_CFGR_SYNCPOL(x)                     (((uint32_t)(((uint32_t)(x)) << CRS_CFGR_SYNCPOL_SHIFT)) & CRS_CFGR_SYNCPOL_MASK)

#define CRS_CFGR_SYNCSRC_SHIFT                  (28)
#define CRS_CFGR_SYNCSRC_MASK                   (0x3U << CRS_CFGR_SYNCSRC_SHIFT)
#define CRS_CFGR_SYNCSRC(x)                     (((uint32_t)(((uint32_t)(x)) << CRS_CFGR_SYNCSRC_SHIFT)) & CRS_CFGR_SYNCSRC_MASK)

#define CRS_CFGR_SYNCDIV_SHIFT                  (24)
#define CRS_CFGR_SYNCDIV_MASK                   (0x7U << CRS_CFGR_SYNCDIV_SHIFT)
#define CRS_CFGR_SYNCDIV(x)                     (((uint32_t)(((uint32_t)(x)) << CRS_CFGR_SYNCDIV_SHIFT)) & CRS_CFGR_SYNCDIV_MASK)

#define CRS_CFGR_FELIM_SHIFT                    (16)
#define CRS_CFGR_FELIM_MASK                     (0xFFU << CRS_CFGR_FELIM_SHIFT)
#define CRS_CFGR_FELIM(x)                       (((uint32_t)(((uint32_t)(x)) << CRS_CFGR_FELIM_SHIFT)) & CRS_CFGR_FELIM_MASK)

#define CRS_CFGR_RELOAD_SHIFT                   (0)
#define CRS_CFGR_RELOAD_MASK                    (0xFFFFU << CRS_CFGR_RELOAD_SHIFT)
#define CRS_CFGR_RELOAD(x)                      (((uint32_t)(((uint32_t)(x)) << CRS_CFGR_RELOAD_SHIFT)) & CRS_CFGR_RELOAD_MASK)

/*!
 * @brief CRS_LSR Register Bit Definition
 */

#define CRS_LSR_FECAP_SHIFT                     (16)
#define CRS_LSR_FECAP_MASK                      (0xFFFFU << CRS_LSR_FECAP_SHIFT)
#define CRS_LSR_FECAP(x)                        (((uint32_t)(((uint32_t)(x)) << CRS_LSR_FECAP_SHIFT)) & CRS_LSR_FECAP_MASK)

#define CRS_LSR_FEDIR_SHIFT                     (15)
#define CRS_LSR_FEDIR_MASK                      (0x01U << CRS_LSR_FEDIR_SHIFT)
#define CRS_LSR_FEDIR(x)                        (((uint32_t)(((uint32_t)(x)) << CRS_LSR_FEDIR_SHIFT)) & CRS_LSR_FEDIR_MASK)

#define CRS_LSR_TRIMOVF_SHIFT                   (10)
#define CRS_LSR_TRIMOVF_MASK                    (0x01U << CRS_LSR_TRIMOVF_SHIFT)
#define CRS_LSR_TRIMOVF(x)                      (((uint32_t)(((uint32_t)(x)) << CRS_LSR_TRIMOVF_SHIFT)) & CRS_LSR_TRIMOVF_MASK)

#define CRS_LSR_SYNCMISS_SHIFT                  (9)
#define CRS_LSR_SYNCMISS_MASK                   (0x01U << CRS_LSR_SYNCMISS_SHIFT)
#define CRS_LSR_SYNCMISS(x)                     (((uint32_t)(((uint32_t)(x)) << CRS_LSR_SYNCMISS_SHIFT)) & CRS_LSR_SYNCMISS_MASK)

#define CRS_LSR_SYNCERR_SHIFT                   (8)
#define CRS_LSR_SYNCERR_MASK                    (0x01U << CRS_LSR_SYNCERR_SHIFT)
#define CRS_LSR_SYNCERR(x)                      (((uint32_t)(((uint32_t)(x)) << CRS_LSR_SYNCERR_SHIFT)) & CRS_LSR_SYNCERR_MASK)

#define CRS_LSR_ESYNCF_SHIFT                    (3)
#define CRS_LSR_ESYNCF_MASK                     (0x01U << CRS_LSR_ESYNCF_SHIFT)
#define CRS_LSR_ESYNCF(x)                       (((uint32_t)(((uint32_t)(x)) << CRS_LSR_ESYNCF_SHIFT)) & CRS_LSR_ESYNCF_MASK)

#define CRS_LSR_ERRF_SHIFT                      (2)
#define CRS_LSR_ERRF_MASK                       (0x01U << CRS_LSR_ERRF_SHIFT)
#define CRS_LSR_ERRF(x)                         (((uint32_t)(((uint32_t)(x)) << CRS_LSR_ERRF_SHIFT)) & CRS_LSR_ERRF_MASK)

#define CRS_LSR_SYNCWARNF_SHIFT                 (1)
#define CRS_LSR_SYNCWARNF_MASK                  (0x01U << CRS_LSR_SYNCWARNF_SHIFT)
#define CRS_LSR_SYNCWARNF(x)                    (((uint32_t)(((uint32_t)(x)) << CRS_LSR_SYNCWARNF_SHIFT)) & CRS_LSR_SYNCWARNF_MASK)

#define CRS_LSR_SYNCOKF_SHIFT                   (0)
#define CRS_LSR_SYNCOKF_MASK                    (0x01U << CRS_LSR_SYNCOKF_SHIFT)
#define CRS_LSR_SYNCOKF(x)                      (((uint32_t)(((uint32_t)(x)) << CRS_LSR_SYNCOKF_SHIFT)) & CRS_LSR_SYNCOKF_MASK)

/*!
 * @brief CRS_LCR Register Bit Definition
 */

#define CRS_LCR_ESYNCC_SHIFT                    (3)
#define CRS_LCR_ESYNCC_MASK                     (0x01U << CRS_LCR_ESYNCC_SHIFT)
#define CRS_LCR_ESYNCC(x)                       (((uint32_t)(((uint32_t)(x)) << CRS_LCR_ESYNCC_SHIFT)) & CRS_LCR_ESYNCC_MASK)

#define CRS_LCR_ERRC_SHIFT                      (2)
#define CRS_LCR_ERRC_MASK                       (0x01U << CRS_LCR_ERRC_SHIFT)
#define CRS_LCR_ERRC(x)                         (((uint32_t)(((uint32_t)(x)) << CRS_LCR_ERRC_SHIFT)) & CRS_LCR_ERRC_MASK)

#define CRS_LCR_SYNCWARNC_SHIFT                 (1)
#define CRS_LCR_SYNCWARNC_MASK                  (0x01U << CRS_LCR_SYNCWARNC_SHIFT)
#define CRS_LCR_SYNCWARNC(x)                    (((uint32_t)(((uint32_t)(x)) << CRS_LCR_SYNCWARNC_SHIFT)) & CRS_LCR_SYNCWARNC_MASK)

#define CRS_LCR_SYNCOKC_SHIFT                   (0)
#define CRS_LCR_SYNCOKC_MASK                    (0x01U << CRS_LCR_SYNCOKC_SHIFT)
#define CRS_LCR_SYNCOKC(x)                      (((uint32_t)(((uint32_t)(x)) << CRS_LCR_SYNCOKC_SHIFT)) & CRS_LCR_SYNCOKC_MASK)

/*!
 * @}
 */ /* end of group CRS_Register_Masks */
/******************************************************************************
 *CRS Instance
*******************************************************************************/

#define CRS                  ((CRS_Type*)CRS_BASE)

/*!
 * @}
 */ /* end of group CRS_Peripheral_Access_Layer */




/*!
 * @addtogroup DAC_Peripheral_Access_Layer DAC Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * DAC Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t CR;                                                             ///< DAC Control Register                         offset: 0x00
    __IO uint32_t SWTRIGR;                                                        ///< DAC Software Trigger Register                offset: 0x04
    __IO uint32_t DHR12R1;                                                        ///< 12-Bit Right-Justified Data Holding Register for DAC Channel 1offset: 0x08
    __IO uint32_t DHR12L1;                                                        ///< 12-Bit Left-Justified Data Holding Register for DAC Channel 1offset: 0x0C
    __IO uint32_t DHR8R1;                                                         ///< 8-Bit Right-Justified Data Holding Register for DAC Channel 1offset: 0x10
    __IO uint32_t DHR12R2;                                                        ///< 12-Bit Right-Justified Data Holding Register for DAC Channel 2offset: 0x14
    __IO uint32_t DHR12L2;                                                        ///< 12-Bit Left-Justified Data Holding Register for DAC Channel 2offset: 0x18
    __IO uint32_t DHR8R2;                                                         ///< 8-Bit Right-Justified Data Holding Register for DAC Channel 2offset: 0x1C
    __IO uint32_t DHR12RD;                                                        ///< 12-Bit Right-Justified Data Holding Registers for Dual DACsoffset: 0x20
    __IO uint32_t DHR12LD;                                                        ///< 12-Bit Left-Justified Data Holding Registers for Dual DACsoffset: 0x24
    __IO uint32_t DHR8RD;                                                         ///< 8-Bit Right-Justified Data Holding Registers for Dual DACsoffset: 0x28
    __IO uint32_t DOR1;                                                           ///< DAC Channel 1 Data Output Register           offset: 0x2C
    __IO uint32_t DOR2;                                                           ///< DAC Channel 2 Data Output Register           offset: 0x30
} DAC_Type;

/*******************************************************************************
 * DAC Type
 ******************************************************************************/

/*!
 * @addtogroup DAC_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief DAC_CR Register Bit Definition
 */

#define DAC_CR_OE2EN_SHIFT                      (29)
#define DAC_CR_OE2EN_MASK                       (0x01U << DAC_CR_OE2EN_SHIFT)
#define DAC_CR_OE2EN(x)                         (((uint32_t)(((uint32_t)(x)) << DAC_CR_OE2EN_SHIFT)) & DAC_CR_OE2EN_MASK)

#define DAC_CR_DMAEN2_SHIFT                     (28)
#define DAC_CR_DMAEN2_MASK                      (0x01U << DAC_CR_DMAEN2_SHIFT)
#define DAC_CR_DMAEN2(x)                        (((uint32_t)(((uint32_t)(x)) << DAC_CR_DMAEN2_SHIFT)) & DAC_CR_DMAEN2_MASK)

#define DAC_CR_MAMP2_SHIFT                      (24)
#define DAC_CR_MAMP2_MASK                       (0xFU << DAC_CR_MAMP2_SHIFT)
#define DAC_CR_MAMP2(x)                         (((uint32_t)(((uint32_t)(x)) << DAC_CR_MAMP2_SHIFT)) & DAC_CR_MAMP2_MASK)

#define DAC_CR_WAVE2_SHIFT                      (22)
#define DAC_CR_WAVE2_MASK                       (0x3U << DAC_CR_WAVE2_SHIFT)
#define DAC_CR_WAVE2(x)                         (((uint32_t)(((uint32_t)(x)) << DAC_CR_WAVE2_SHIFT)) & DAC_CR_WAVE2_MASK)

#define DAC_CR_TEN2_SHIFT                       (18)
#define DAC_CR_TEN2_MASK                        (0x01U << DAC_CR_TEN2_SHIFT)
#define DAC_CR_TEN2(x)                          (((uint32_t)(((uint32_t)(x)) << DAC_CR_TEN2_SHIFT)) & DAC_CR_TEN2_MASK)

#define DAC_CR_BOFF2_SHIFT                      (17)
#define DAC_CR_BOFF2_MASK                       (0x01U << DAC_CR_BOFF2_SHIFT)
#define DAC_CR_BOFF2(x)                         (((uint32_t)(((uint32_t)(x)) << DAC_CR_BOFF2_SHIFT)) & DAC_CR_BOFF2_MASK)

#define DAC_CR_EN2_SHIFT                        (16)
#define DAC_CR_EN2_MASK                         (0x01U << DAC_CR_EN2_SHIFT)
#define DAC_CR_EN2(x)                           (((uint32_t)(((uint32_t)(x)) << DAC_CR_EN2_SHIFT)) & DAC_CR_EN2_MASK)

#define DAC_CR_OEEN1_SHIFT                      (13)
#define DAC_CR_OEEN1_MASK                       (0x01U << DAC_CR_OEEN1_SHIFT)
#define DAC_CR_OEEN1(x)                         (((uint32_t)(((uint32_t)(x)) << DAC_CR_OEEN1_SHIFT)) & DAC_CR_OEEN1_MASK)

#define DAC_CR_DMAEN1_SHIFT                     (12)
#define DAC_CR_DMAEN1_MASK                      (0x01U << DAC_CR_DMAEN1_SHIFT)
#define DAC_CR_DMAEN1(x)                        (((uint32_t)(((uint32_t)(x)) << DAC_CR_DMAEN1_SHIFT)) & DAC_CR_DMAEN1_MASK)

#define DAC_CR_MAMP1_SHIFT                      (8)
#define DAC_CR_MAMP1_MASK                       (0xFU << DAC_CR_MAMP1_SHIFT)
#define DAC_CR_MAMP1(x)                         (((uint32_t)(((uint32_t)(x)) << DAC_CR_MAMP1_SHIFT)) & DAC_CR_MAMP1_MASK)

#define DAC_CR_WAVE1_SHIFT                      (6)
#define DAC_CR_WAVE1_MASK                       (0x3U << DAC_CR_WAVE1_SHIFT)
#define DAC_CR_WAVE1(x)                         (((uint32_t)(((uint32_t)(x)) << DAC_CR_WAVE1_SHIFT)) & DAC_CR_WAVE1_MASK)

#define DAC_CR_TEN1_SHIFT                       (2)
#define DAC_CR_TEN1_MASK                        (0x01U << DAC_CR_TEN1_SHIFT)
#define DAC_CR_TEN1(x)                          (((uint32_t)(((uint32_t)(x)) << DAC_CR_TEN1_SHIFT)) & DAC_CR_TEN1_MASK)

#define DAC_CR_BOFF1_SHIFT                      (1)
#define DAC_CR_BOFF1_MASK                       (0x01U << DAC_CR_BOFF1_SHIFT)
#define DAC_CR_BOFF1(x)                         (((uint32_t)(((uint32_t)(x)) << DAC_CR_BOFF1_SHIFT)) & DAC_CR_BOFF1_MASK)

#define DAC_CR_EN1_SHIFT                        (0)
#define DAC_CR_EN1_MASK                         (0x01U << DAC_CR_EN1_SHIFT)
#define DAC_CR_EN1(x)                           (((uint32_t)(((uint32_t)(x)) << DAC_CR_EN1_SHIFT)) & DAC_CR_EN1_MASK)

/*!
 * @brief DAC_SWTRIGR Register Bit Definition
 */

#define DAC_SWTRIGR_SWTRIG2_SHIFT               (1)
#define DAC_SWTRIGR_SWTRIG2_MASK                (0x01U << DAC_SWTRIGR_SWTRIG2_SHIFT)
#define DAC_SWTRIGR_SWTRIG2(x)                  (((uint32_t)(((uint32_t)(x)) << DAC_SWTRIGR_SWTRIG2_SHIFT)) & DAC_SWTRIGR_SWTRIG2_MASK)

#define DAC_SWTRIGR_SWTRIG1_SHIFT               (0)
#define DAC_SWTRIGR_SWTRIG1_MASK                (0x01U << DAC_SWTRIGR_SWTRIG1_SHIFT)
#define DAC_SWTRIGR_SWTRIG1(x)                  (((uint32_t)(((uint32_t)(x)) << DAC_SWTRIGR_SWTRIG1_SHIFT)) & DAC_SWTRIGR_SWTRIG1_MASK)

/*!
 * @brief DAC_DHR12R1 Register Bit Definition
 */

#define DAC_DHR12R1_DACC1DHR_SHIFT              (0)
#define DAC_DHR12R1_DACC1DHR_MASK               (0xFFFU << DAC_DHR12R1_DACC1DHR_SHIFT)
#define DAC_DHR12R1_DACC1DHR(x)                 (((uint32_t)(((uint32_t)(x)) << DAC_DHR12R1_DACC1DHR_SHIFT)) & DAC_DHR12R1_DACC1DHR_MASK)

/*!
 * @brief DAC_DHR12L1 Register Bit Definition
 */

#define DAC_DHR12L1_DACC1DHR_SHIFT              (4)
#define DAC_DHR12L1_DACC1DHR_MASK               (0xFFFU << DAC_DHR12L1_DACC1DHR_SHIFT)
#define DAC_DHR12L1_DACC1DHR(x)                 (((uint32_t)(((uint32_t)(x)) << DAC_DHR12L1_DACC1DHR_SHIFT)) & DAC_DHR12L1_DACC1DHR_MASK)

/*!
 * @brief DAC_DHR8R1 Register Bit Definition
 */

#define DAC_DHR8R1_DACC1DHR_SHIFT               (0)
#define DAC_DHR8R1_DACC1DHR_MASK                (0xFFU << DAC_DHR8R1_DACC1DHR_SHIFT)
#define DAC_DHR8R1_DACC1DHR(x)                  (((uint32_t)(((uint32_t)(x)) << DAC_DHR8R1_DACC1DHR_SHIFT)) & DAC_DHR8R1_DACC1DHR_MASK)

/*!
 * @brief DAC_DHR12R2 Register Bit Definition
 */

#define DAC_DHR12R2_DACC2DHR_SHIFT              (0)
#define DAC_DHR12R2_DACC2DHR_MASK               (0xFFFU << DAC_DHR12R2_DACC2DHR_SHIFT)
#define DAC_DHR12R2_DACC2DHR(x)                 (((uint32_t)(((uint32_t)(x)) << DAC_DHR12R2_DACC2DHR_SHIFT)) & DAC_DHR12R2_DACC2DHR_MASK)

/*!
 * @brief DAC_DHR12L2 Register Bit Definition
 */

#define DAC_DHR12L2_DACC2DHR_SHIFT              (4)
#define DAC_DHR12L2_DACC2DHR_MASK               (0xFFFU << DAC_DHR12L2_DACC2DHR_SHIFT)
#define DAC_DHR12L2_DACC2DHR(x)                 (((uint32_t)(((uint32_t)(x)) << DAC_DHR12L2_DACC2DHR_SHIFT)) & DAC_DHR12L2_DACC2DHR_MASK)

/*!
 * @brief DAC_DHR8R2 Register Bit Definition
 */

#define DAC_DHR8R2_DACC2DHR_SHIFT               (0)
#define DAC_DHR8R2_DACC2DHR_MASK                (0xFFU << DAC_DHR8R2_DACC2DHR_SHIFT)
#define DAC_DHR8R2_DACC2DHR(x)                  (((uint32_t)(((uint32_t)(x)) << DAC_DHR8R2_DACC2DHR_SHIFT)) & DAC_DHR8R2_DACC2DHR_MASK)

/*!
 * @brief DAC_DHR12RD Register Bit Definition
 */

#define DAC_DHR12RD_DACC2DHR_SHIFT              (16)
#define DAC_DHR12RD_DACC2DHR_MASK               (0xFFFU << DAC_DHR12RD_DACC2DHR_SHIFT)
#define DAC_DHR12RD_DACC2DHR(x)                 (((uint32_t)(((uint32_t)(x)) << DAC_DHR12RD_DACC2DHR_SHIFT)) & DAC_DHR12RD_DACC2DHR_MASK)

#define DAC_DHR12RD_DACC1DHR_SHIFT              (0)
#define DAC_DHR12RD_DACC1DHR_MASK               (0xFFFU << DAC_DHR12RD_DACC1DHR_SHIFT)
#define DAC_DHR12RD_DACC1DHR(x)                 (((uint32_t)(((uint32_t)(x)) << DAC_DHR12RD_DACC1DHR_SHIFT)) & DAC_DHR12RD_DACC1DHR_MASK)

/*!
 * @brief DAC_DHR12LD Register Bit Definition
 */

#define DAC_DHR12LD_DACC2DHR_SHIFT              (20)
#define DAC_DHR12LD_DACC2DHR_MASK               (0xFFFU << DAC_DHR12LD_DACC2DHR_SHIFT)
#define DAC_DHR12LD_DACC2DHR(x)                 (((uint32_t)(((uint32_t)(x)) << DAC_DHR12LD_DACC2DHR_SHIFT)) & DAC_DHR12LD_DACC2DHR_MASK)

#define DAC_DHR12LD_DACC1DHR_SHIFT              (4)
#define DAC_DHR12LD_DACC1DHR_MASK               (0xFFFU << DAC_DHR12LD_DACC1DHR_SHIFT)
#define DAC_DHR12LD_DACC1DHR(x)                 (((uint32_t)(((uint32_t)(x)) << DAC_DHR12LD_DACC1DHR_SHIFT)) & DAC_DHR12LD_DACC1DHR_MASK)

/*!
 * @brief DAC_DHR8RD Register Bit Definition
 */

#define DAC_DHR8RD_DACC2DHR_SHIFT               (8)
#define DAC_DHR8RD_DACC2DHR_MASK                (0xFFU << DAC_DHR8RD_DACC2DHR_SHIFT)
#define DAC_DHR8RD_DACC2DHR(x)                  (((uint32_t)(((uint32_t)(x)) << DAC_DHR8RD_DACC2DHR_SHIFT)) & DAC_DHR8RD_DACC2DHR_MASK)

#define DAC_DHR8RD_DACC1DHR_SHIFT               (0)
#define DAC_DHR8RD_DACC1DHR_MASK                (0xFFU << DAC_DHR8RD_DACC1DHR_SHIFT)
#define DAC_DHR8RD_DACC1DHR(x)                  (((uint32_t)(((uint32_t)(x)) << DAC_DHR8RD_DACC1DHR_SHIFT)) & DAC_DHR8RD_DACC1DHR_MASK)

/*!
 * @brief DAC_DOR1 Register Bit Definition
 */

#define DAC_DOR1_DACC1DOR_SHIFT                 (0)
#define DAC_DOR1_DACC1DOR_MASK                  (0xFFFU << DAC_DOR1_DACC1DOR_SHIFT)
#define DAC_DOR1_DACC1DOR(x)                    (((uint32_t)(((uint32_t)(x)) << DAC_DOR1_DACC1DOR_SHIFT)) & DAC_DOR1_DACC1DOR_MASK)

/*!
 * @brief DAC_DOR2 Register Bit Definition
 */

#define DAC_DOR2_DACC2DOR_SHIFT                 (0)
#define DAC_DOR2_DACC2DOR_MASK                  (0xFFFU << DAC_DOR2_DACC2DOR_SHIFT)
#define DAC_DOR2_DACC2DOR(x)                    (((uint32_t)(((uint32_t)(x)) << DAC_DOR2_DACC2DOR_SHIFT)) & DAC_DOR2_DACC2DOR_MASK)

/*!
 * @}
 */ /* end of group DAC_Register_Masks */
/******************************************************************************
 *DAC Instance
*******************************************************************************/

#define DAC                  ((DAC_Type*)DAC_BASE)

/*!
 * @}
 */ /* end of group DAC_Peripheral_Access_Layer */




/*!
 * @addtogroup DBGMCU_Peripheral_Access_Layer DBGMCU Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * DBGMCU Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t IDCODE;                                                         ///< MCU_IDCODE register                          offset: 0x00
    __IO uint32_t CR;                                                             ///< DEBUG control register                       offset: 0x04
} DBGMCU_Type;

/*******************************************************************************
 * DBGMCU Type
 ******************************************************************************/

/*!
 * @addtogroup DBGMCU_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief DBGMCU_IDCODE Register Bit Definition
 */

#define DBGMCU_IDCODE_DEVID_SHIFT               (0)
#define DBGMCU_IDCODE_DEVID_MASK                (0xFFFFFFFFU << DBGMCU_IDCODE_DEVID_SHIFT)
#define DBGMCU_IDCODE_DEVID(x)                  (((uint32_t)(((uint32_t)(x)) << DBGMCU_IDCODE_DEVID_SHIFT)) & DBGMCU_IDCODE_DEVID_MASK)

/*!
 * @brief DBGMCU_CR Register Bit Definition
 */

#define DBGMCU_CR_DBGTIM7STOP_SHIFT           	(20)
#define DBGMCU_CR_DBGTIM7STOP_MASK            	(0x01U << DBGMCU_CR_DBGTIM7STOP_SHIFT)
#define DBGMCU_CR_DBGTIM7STOP(x)              	(((uint32_t)(((uint32_t)(x)) << DBGMCU_CR_DBGTIM7STOP_SHIFT)) & DBGMCU_CR_DBGTIM7STOP_MASK)

#define DBGMCU_CR_DBGTIM6STOP_SHIFT           	(19)
#define DBGMCU_CR_DBGTIM6STOP_MASK            	(0x01U << DBGMCU_CR_DBGTIM6STOP_SHIFT)
#define DBGMCU_CR_DBGTIM6STOP(x)              	(((uint32_t)(((uint32_t)(x)) << DBGMCU_CR_DBGTIM6STOP_SHIFT)) & DBGMCU_CR_DBGTIM6STOP_MASK)

#define DBGMCU_CR_DBGTIM5STOP_SHIFT           	(18)
#define DBGMCU_CR_DBGTIM5STOP_MASK            	(0x01U << DBGMCU_CR_DBGTIM5STOP_SHIFT)
#define DBGMCU_CR_DBGTIM5STOP(x)              	(((uint32_t)(((uint32_t)(x)) << DBGMCU_CR_DBGTIM5STOP_SHIFT)) & DBGMCU_CR_DBGTIM5STOP_MASK)

#define DBGMCU_CR_DBGTIM8STOP_SHIFT           	(17)
#define DBGMCU_CR_DBGTIM8STOP_MASK            	(0x01U << DBGMCU_CR_DBGTIM8STOP_SHIFT)
#define DBGMCU_CR_DBGTIM8STOP(x)              	(((uint32_t)(((uint32_t)(x)) << DBGMCU_CR_DBGTIM8STOP_SHIFT)) & DBGMCU_CR_DBGTIM8STOP_MASK)

#define DBGMCU_CR_DBGTIM4STOP_SHIFT           	(13)
#define DBGMCU_CR_DBGTIM4STOP_MASK            	(0x01U << DBGMCU_CR_DBGTIM4STOP_SHIFT)
#define DBGMCU_CR_DBGTIM4STOP(x)              	(((uint32_t)(((uint32_t)(x)) << DBGMCU_CR_DBGTIM4STOP_SHIFT)) & DBGMCU_CR_DBGTIM4STOP_MASK)

#define DBGMCU_CR_DBGTIM3STOP_SHIFT           	(12)
#define DBGMCU_CR_DBGTIM3STOP_MASK            	(0x01U << DBGMCU_CR_DBGTIM3STOP_SHIFT)
#define DBGMCU_CR_DBGTIM3STOP(x)              	(((uint32_t)(((uint32_t)(x)) << DBGMCU_CR_DBGTIM3STOP_SHIFT)) & DBGMCU_CR_DBGTIM3STOP_MASK)

#define DBGMCU_CR_DBGTIM2STOP_SHIFT           	(11)
#define DBGMCU_CR_DBGTIM2STOP_MASK            	(0x01U << DBGMCU_CR_DBGTIM2STOP_SHIFT)
#define DBGMCU_CR_DBGTIM2STOP(x)              	(((uint32_t)(((uint32_t)(x)) << DBGMCU_CR_DBGTIM2STOP_SHIFT)) & DBGMCU_CR_DBGTIM2STOP_MASK)

#define DBGMCU_CR_DBGTIM1STOP_SHIFT           	(10)
#define DBGMCU_CR_DBGTIM1STOP_MASK            	(0x01U << DBGMCU_CR_DBGTIM1STOP_SHIFT)
#define DBGMCU_CR_DBGTIM1STOP(x)              	(((uint32_t)(((uint32_t)(x)) << DBGMCU_CR_DBGTIM1STOP_SHIFT)) & DBGMCU_CR_DBGTIM1STOP_MASK)

#define DBGMCU_CR_DBGWWDGSTOP_SHIFT           	(9)
#define DBGMCU_CR_DBGWWDGSTOP_MASK            	(0x01U << DBGMCU_CR_DBGWWDGSTOP_SHIFT)
#define DBGMCU_CR_DBGWWDGSTOP(x)              	(((uint32_t)(((uint32_t)(x)) << DBGMCU_CR_DBGWWDGSTOP_SHIFT)) & DBGMCU_CR_DBGWWDGSTOP_MASK)

#define DBGMCU_CR_DBGIWDGSTOP_SHIFT           	(8)
#define DBGMCU_CR_DBGIWDGSTOP_MASK            	(0x01U << DBGMCU_CR_DBGIWDGSTOP_SHIFT)
#define DBGMCU_CR_DBGIWDGSTOP(x)              	(((uint32_t)(((uint32_t)(x)) << DBGMCU_CR_DBGIWDGSTOP_SHIFT)) & DBGMCU_CR_DBGIWDGSTOP_MASK)

#define DBGMCU_CR_TRACEMODE_SHIFT               (6)
#define DBGMCU_CR_TRACEMODE_MASK                (0x3U << DBGMCU_CR_TRACEMODE_SHIFT)
#define DBGMCU_CR_TRACEMODE(x)                  (((uint32_t)(((uint32_t)(x)) << DBGMCU_CR_TRACEMODE_SHIFT)) & DBGMCU_CR_TRACEMODE_MASK)

#define DBGMCU_CR_TRACEIOEN_SHIFT               (5)
#define DBGMCU_CR_TRACEIOEN_MASK                (0x01U << DBGMCU_CR_TRACEIOEN_SHIFT)
#define DBGMCU_CR_TRACEIOEN(x)                  (((uint32_t)(((uint32_t)(x)) << DBGMCU_CR_TRACEIOEN_SHIFT)) & DBGMCU_CR_TRACEIOEN_MASK)

#define DBGMCU_CR_DBGSTOPFORLDO_SHIFT         	(3)
#define DBGMCU_CR_DBGSTOPFORLDO_MASK          	(0x01U << DBGMCU_CR_DBGSTOPFORLDO_SHIFT)
#define DBGMCU_CR_DBGSTOPFORLDO(x)            	(((uint32_t)(((uint32_t)(x)) << DBGMCU_CR_DBGSTOPFORLDO_SHIFT)) & DBGMCU_CR_DBGSTOPFORLDO_MASK)

/*!
 * @}
 */ /* end of group DBGMCU_Register_Masks */
/******************************************************************************
 *DBGMCU Instance
*******************************************************************************/

#define DBGMCU               ((DBGMCU_Type*)DBGMCU_BASE)

/*!
 * @}
 */ /* end of group DBGMCU_Peripheral_Access_Layer */




/*!
 * @addtogroup DMA_Peripheral_Access_Layer DMA Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * DMA Type
 ******************************************************************************/
#define DMA_CHANNEL_COUNT  8u

typedef struct {
    __IO uint32_t ISR;                                                            ///< DMA Interrupt Status Register                offset: 0x00
    __IO uint32_t IFCR;                                                           ///< DMA Interrupt Flag Clear Register            offset: 0x04
    struct {
            __IO uint32_t CCR;                                                    ///< DMAchannelxconfigurationregister             offset: 0x08
            __IO uint32_t CNDTR;                                                  ///< DMAchannelxtransferquantityregister          offset: 0x0C
            __IO uint32_t CPAR;                                                   ///< DMAchannelxperipheraladdressregister         offset: 0x10
            __IO uint32_t CMAR;                                                   ///< DMAchannelxmemoryaddressregister             offset: 0x14
            __IO uint32_t Reserved0;
    } CH[DMA_CHANNEL_COUNT];
} DMA_Type;

/*******************************************************************************
 * DMA Type
 ******************************************************************************/

/*!
 * @addtogroup DMA_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief DMA_ISR Register Bit Definition
 */

#define DMA_ISR_TEIF8_SHIFT                     (31)
#define DMA_ISR_TEIF8_MASK                      (0x01U << DMA_ISR_TEIF8_SHIFT)
#define DMA_ISR_TEIF8(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_TEIF8_SHIFT)) & DMA_ISR_TEIF8_MASK)

#define DMA_ISR_HTIF8_SHIFT                     (30)
#define DMA_ISR_HTIF8_MASK                      (0x01U << DMA_ISR_HTIF8_SHIFT)
#define DMA_ISR_HTIF8(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_HTIF8_SHIFT)) & DMA_ISR_HTIF8_MASK)

#define DMA_ISR_TCIF8_SHIFT                     (29)
#define DMA_ISR_TCIF8_MASK                      (0x01U << DMA_ISR_TCIF8_SHIFT)
#define DMA_ISR_TCIF8(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_TCIF8_SHIFT)) & DMA_ISR_TCIF8_MASK)

#define DMA_ISR_GIF8_SHIFT                      (28)
#define DMA_ISR_GIF8_MASK                       (0x01U << DMA_ISR_GIF8_SHIFT)
#define DMA_ISR_GIF8(x)                         (((uint32_t)(((uint32_t)(x)) << DMA_ISR_GIF8_SHIFT)) & DMA_ISR_GIF8_MASK)

#define DMA_ISR_TEIF7_SHIFT                     (27)
#define DMA_ISR_TEIF7_MASK                      (0x01U << DMA_ISR_TEIF7_SHIFT)
#define DMA_ISR_TEIF7(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_TEIF7_SHIFT)) & DMA_ISR_TEIF7_MASK)

#define DMA_ISR_HTIF7_SHIFT                     (26)
#define DMA_ISR_HTIF7_MASK                      (0x01U << DMA_ISR_HTIF7_SHIFT)
#define DMA_ISR_HTIF7(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_HTIF7_SHIFT)) & DMA_ISR_HTIF7_MASK)

#define DMA_ISR_TCIF7_SHIFT                     (25)
#define DMA_ISR_TCIF7_MASK                      (0x01U << DMA_ISR_TCIF7_SHIFT)
#define DMA_ISR_TCIF7(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_TCIF7_SHIFT)) & DMA_ISR_TCIF7_MASK)

#define DMA_ISR_GIF7_SHIFT                      (24)
#define DMA_ISR_GIF7_MASK                       (0x01U << DMA_ISR_GIF7_SHIFT)
#define DMA_ISR_GIF7(x)                         (((uint32_t)(((uint32_t)(x)) << DMA_ISR_GIF7_SHIFT)) & DMA_ISR_GIF7_MASK)

#define DMA_ISR_TEIF6_SHIFT                     (23)
#define DMA_ISR_TEIF6_MASK                      (0x01U << DMA_ISR_TEIF6_SHIFT)
#define DMA_ISR_TEIF6(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_TEIF6_SHIFT)) & DMA_ISR_TEIF6_MASK)

#define DMA_ISR_HTIF6_SHIFT                     (22)
#define DMA_ISR_HTIF6_MASK                      (0x01U << DMA_ISR_HTIF6_SHIFT)
#define DMA_ISR_HTIF6(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_HTIF6_SHIFT)) & DMA_ISR_HTIF6_MASK)

#define DMA_ISR_TCIF6_SHIFT                     (21)
#define DMA_ISR_TCIF6_MASK                      (0x01U << DMA_ISR_TCIF6_SHIFT)
#define DMA_ISR_TCIF6(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_TCIF6_SHIFT)) & DMA_ISR_TCIF6_MASK)

#define DMA_ISR_GIF6_SHIFT                      (20)
#define DMA_ISR_GIF6_MASK                       (0x01U << DMA_ISR_GIF6_SHIFT)
#define DMA_ISR_GIF6(x)                         (((uint32_t)(((uint32_t)(x)) << DMA_ISR_GIF6_SHIFT)) & DMA_ISR_GIF6_MASK)

#define DMA_ISR_TEIF5_SHIFT                     (19)
#define DMA_ISR_TEIF5_MASK                      (0x01U << DMA_ISR_TEIF5_SHIFT)
#define DMA_ISR_TEIF5(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_TEIF5_SHIFT)) & DMA_ISR_TEIF5_MASK)

#define DMA_ISR_HTIF5_SHIFT                     (18)
#define DMA_ISR_HTIF5_MASK                      (0x01U << DMA_ISR_HTIF5_SHIFT)
#define DMA_ISR_HTIF5(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_HTIF5_SHIFT)) & DMA_ISR_HTIF5_MASK)

#define DMA_ISR_TCIF5_SHIFT                     (17)
#define DMA_ISR_TCIF5_MASK                      (0x01U << DMA_ISR_TCIF5_SHIFT)
#define DMA_ISR_TCIF5(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_TCIF5_SHIFT)) & DMA_ISR_TCIF5_MASK)

#define DMA_ISR_GIF5_SHIFT                      (16)
#define DMA_ISR_GIF5_MASK                       (0x01U << DMA_ISR_GIF5_SHIFT)
#define DMA_ISR_GIF5(x)                         (((uint32_t)(((uint32_t)(x)) << DMA_ISR_GIF5_SHIFT)) & DMA_ISR_GIF5_MASK)

#define DMA_ISR_TEIF4_SHIFT                     (15)
#define DMA_ISR_TEIF4_MASK                      (0x01U << DMA_ISR_TEIF4_SHIFT)
#define DMA_ISR_TEIF4(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_TEIF4_SHIFT)) & DMA_ISR_TEIF4_MASK)

#define DMA_ISR_HTIF4_SHIFT                     (14)
#define DMA_ISR_HTIF4_MASK                      (0x01U << DMA_ISR_HTIF4_SHIFT)
#define DMA_ISR_HTIF4(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_HTIF4_SHIFT)) & DMA_ISR_HTIF4_MASK)

#define DMA_ISR_TCIF4_SHIFT                     (13)
#define DMA_ISR_TCIF4_MASK                      (0x01U << DMA_ISR_TCIF4_SHIFT)
#define DMA_ISR_TCIF4(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_TCIF4_SHIFT)) & DMA_ISR_TCIF4_MASK)

#define DMA_ISR_GIF4_SHIFT                      (12)
#define DMA_ISR_GIF4_MASK                       (0x01U << DMA_ISR_GIF4_SHIFT)
#define DMA_ISR_GIF4(x)                         (((uint32_t)(((uint32_t)(x)) << DMA_ISR_GIF4_SHIFT)) & DMA_ISR_GIF4_MASK)

#define DMA_ISR_TEIF3_SHIFT                     (11)
#define DMA_ISR_TEIF3_MASK                      (0x01U << DMA_ISR_TEIF3_SHIFT)
#define DMA_ISR_TEIF3(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_TEIF3_SHIFT)) & DMA_ISR_TEIF3_MASK)

#define DMA_ISR_HTIF3_SHIFT                     (10)
#define DMA_ISR_HTIF3_MASK                      (0x01U << DMA_ISR_HTIF3_SHIFT)
#define DMA_ISR_HTIF3(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_HTIF3_SHIFT)) & DMA_ISR_HTIF3_MASK)

#define DMA_ISR_TCIF3_SHIFT                     (9)
#define DMA_ISR_TCIF3_MASK                      (0x01U << DMA_ISR_TCIF3_SHIFT)
#define DMA_ISR_TCIF3(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_TCIF3_SHIFT)) & DMA_ISR_TCIF3_MASK)

#define DMA_ISR_GIF3_SHIFT                      (8)
#define DMA_ISR_GIF3_MASK                       (0x01U << DMA_ISR_GIF3_SHIFT)
#define DMA_ISR_GIF3(x)                         (((uint32_t)(((uint32_t)(x)) << DMA_ISR_GIF3_SHIFT)) & DMA_ISR_GIF3_MASK)

#define DMA_ISR_TEIF2_SHIFT                     (7)
#define DMA_ISR_TEIF2_MASK                      (0x01U << DMA_ISR_TEIF2_SHIFT)
#define DMA_ISR_TEIF2(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_TEIF2_SHIFT)) & DMA_ISR_TEIF2_MASK)

#define DMA_ISR_HTIF2_SHIFT                     (6)
#define DMA_ISR_HTIF2_MASK                      (0x01U << DMA_ISR_HTIF2_SHIFT)
#define DMA_ISR_HTIF2(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_HTIF2_SHIFT)) & DMA_ISR_HTIF2_MASK)

#define DMA_ISR_TCIF2_SHIFT                     (5)
#define DMA_ISR_TCIF2_MASK                      (0x01U << DMA_ISR_TCIF2_SHIFT)
#define DMA_ISR_TCIF2(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_TCIF2_SHIFT)) & DMA_ISR_TCIF2_MASK)

#define DMA_ISR_GIF2_SHIFT                      (4)
#define DMA_ISR_GIF2_MASK                       (0x01U << DMA_ISR_GIF2_SHIFT)
#define DMA_ISR_GIF2(x)                         (((uint32_t)(((uint32_t)(x)) << DMA_ISR_GIF2_SHIFT)) & DMA_ISR_GIF2_MASK)

#define DMA_ISR_TEIF1_SHIFT                     (3)
#define DMA_ISR_TEIF1_MASK                      (0x01U << DMA_ISR_TEIF1_SHIFT)
#define DMA_ISR_TEIF1(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_TEIF1_SHIFT)) & DMA_ISR_TEIF1_MASK)

#define DMA_ISR_HTIF1_SHIFT                     (2)
#define DMA_ISR_HTIF1_MASK                      (0x01U << DMA_ISR_HTIF1_SHIFT)
#define DMA_ISR_HTIF1(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_HTIF1_SHIFT)) & DMA_ISR_HTIF1_MASK)

#define DMA_ISR_TCIF1_SHIFT                     (1)
#define DMA_ISR_TCIF1_MASK                      (0x01U << DMA_ISR_TCIF1_SHIFT)
#define DMA_ISR_TCIF1(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_ISR_TCIF1_SHIFT)) & DMA_ISR_TCIF1_MASK)

#define DMA_ISR_GIF1_SHIFT                      (0)
#define DMA_ISR_GIF1_MASK                       (0x01U << DMA_ISR_GIF1_SHIFT)
#define DMA_ISR_GIF1(x)                         (((uint32_t)(((uint32_t)(x)) << DMA_ISR_GIF1_SHIFT)) & DMA_ISR_GIF1_MASK)

/*!
 * @brief DMA_IFCR Register Bit Definition
 */

#define DMA_IFCR_CTEIF8_SHIFT                   (31)
#define DMA_IFCR_CTEIF8_MASK                    (0x01U << DMA_IFCR_CTEIF8_SHIFT)
#define DMA_IFCR_CTEIF8(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CTEIF8_SHIFT)) & DMA_IFCR_CTEIF8_MASK)

#define DMA_IFCR_CHTIF8_SHIFT                   (30)
#define DMA_IFCR_CHTIF8_MASK                    (0x01U << DMA_IFCR_CHTIF8_SHIFT)
#define DMA_IFCR_CHTIF8(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CHTIF8_SHIFT)) & DMA_IFCR_CHTIF8_MASK)

#define DMA_IFCR_CTCIF8_SHIFT                   (29)
#define DMA_IFCR_CTCIF8_MASK                    (0x01U << DMA_IFCR_CTCIF8_SHIFT)
#define DMA_IFCR_CTCIF8(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CTCIF8_SHIFT)) & DMA_IFCR_CTCIF8_MASK)

#define DMA_IFCR_CGIF8_SHIFT                    (28)
#define DMA_IFCR_CGIF8_MASK                     (0x01U << DMA_IFCR_CGIF8_SHIFT)
#define DMA_IFCR_CGIF8(x)                       (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CGIF8_SHIFT)) & DMA_IFCR_CGIF8_MASK)

#define DMA_IFCR_CTEIF7_SHIFT                   (27)
#define DMA_IFCR_CTEIF7_MASK                    (0x01U << DMA_IFCR_CTEIF7_SHIFT)
#define DMA_IFCR_CTEIF7(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CTEIF7_SHIFT)) & DMA_IFCR_CTEIF7_MASK)

#define DMA_IFCR_CHTIF7_SHIFT                   (26)
#define DMA_IFCR_CHTIF7_MASK                    (0x01U << DMA_IFCR_CHTIF7_SHIFT)
#define DMA_IFCR_CHTIF7(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CHTIF7_SHIFT)) & DMA_IFCR_CHTIF7_MASK)

#define DMA_IFCR_CTCIF7_SHIFT                   (25)
#define DMA_IFCR_CTCIF7_MASK                    (0x01U << DMA_IFCR_CTCIF7_SHIFT)
#define DMA_IFCR_CTCIF7(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CTCIF7_SHIFT)) & DMA_IFCR_CTCIF7_MASK)

#define DMA_IFCR_CGIF7_SHIFT                    (24)
#define DMA_IFCR_CGIF7_MASK                     (0x01U << DMA_IFCR_CGIF7_SHIFT)
#define DMA_IFCR_CGIF7(x)                       (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CGIF7_SHIFT)) & DMA_IFCR_CGIF7_MASK)

#define DMA_IFCR_CTEIF6_SHIFT                   (23)
#define DMA_IFCR_CTEIF6_MASK                    (0x01U << DMA_IFCR_CTEIF6_SHIFT)
#define DMA_IFCR_CTEIF6(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CTEIF6_SHIFT)) & DMA_IFCR_CTEIF6_MASK)

#define DMA_IFCR_CHTIF6_SHIFT                   (22)
#define DMA_IFCR_CHTIF6_MASK                    (0x01U << DMA_IFCR_CHTIF6_SHIFT)
#define DMA_IFCR_CHTIF6(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CHTIF6_SHIFT)) & DMA_IFCR_CHTIF6_MASK)

#define DMA_IFCR_CTCIF6_SHIFT                   (21)
#define DMA_IFCR_CTCIF6_MASK                    (0x01U << DMA_IFCR_CTCIF6_SHIFT)
#define DMA_IFCR_CTCIF6(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CTCIF6_SHIFT)) & DMA_IFCR_CTCIF6_MASK)

#define DMA_IFCR_CGIF6_SHIFT                    (20)
#define DMA_IFCR_CGIF6_MASK                     (0x01U << DMA_IFCR_CGIF6_SHIFT)
#define DMA_IFCR_CGIF6(x)                       (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CGIF6_SHIFT)) & DMA_IFCR_CGIF6_MASK)

#define DMA_IFCR_CTEIF5_SHIFT                   (19)
#define DMA_IFCR_CTEIF5_MASK                    (0x01U << DMA_IFCR_CTEIF5_SHIFT)
#define DMA_IFCR_CTEIF5(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CTEIF5_SHIFT)) & DMA_IFCR_CTEIF5_MASK)

#define DMA_IFCR_CHTIF5_SHIFT                   (18)
#define DMA_IFCR_CHTIF5_MASK                    (0x01U << DMA_IFCR_CHTIF5_SHIFT)
#define DMA_IFCR_CHTIF5(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CHTIF5_SHIFT)) & DMA_IFCR_CHTIF5_MASK)

#define DMA_IFCR_CTCIF5_SHIFT                   (17)
#define DMA_IFCR_CTCIF5_MASK                    (0x01U << DMA_IFCR_CTCIF5_SHIFT)
#define DMA_IFCR_CTCIF5(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CTCIF5_SHIFT)) & DMA_IFCR_CTCIF5_MASK)

#define DMA_IFCR_CGIF5_SHIFT                    (16)
#define DMA_IFCR_CGIF5_MASK                     (0x01U << DMA_IFCR_CGIF5_SHIFT)
#define DMA_IFCR_CGIF5(x)                       (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CGIF5_SHIFT)) & DMA_IFCR_CGIF5_MASK)

#define DMA_IFCR_CTEIF4_SHIFT                   (15)
#define DMA_IFCR_CTEIF4_MASK                    (0x01U << DMA_IFCR_CTEIF4_SHIFT)
#define DMA_IFCR_CTEIF4(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CTEIF4_SHIFT)) & DMA_IFCR_CTEIF4_MASK)

#define DMA_IFCR_CHTIF4_SHIFT                   (14)
#define DMA_IFCR_CHTIF4_MASK                    (0x01U << DMA_IFCR_CHTIF4_SHIFT)
#define DMA_IFCR_CHTIF4(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CHTIF4_SHIFT)) & DMA_IFCR_CHTIF4_MASK)

#define DMA_IFCR_CTCIF4_SHIFT                   (13)
#define DMA_IFCR_CTCIF4_MASK                    (0x01U << DMA_IFCR_CTCIF4_SHIFT)
#define DMA_IFCR_CTCIF4(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CTCIF4_SHIFT)) & DMA_IFCR_CTCIF4_MASK)

#define DMA_IFCR_CGIF4_SHIFT                    (12)
#define DMA_IFCR_CGIF4_MASK                     (0x01U << DMA_IFCR_CGIF4_SHIFT)
#define DMA_IFCR_CGIF4(x)                       (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CGIF4_SHIFT)) & DMA_IFCR_CGIF4_MASK)

#define DMA_IFCR_CTEIF3_SHIFT                   (11)
#define DMA_IFCR_CTEIF3_MASK                    (0x01U << DMA_IFCR_CTEIF3_SHIFT)
#define DMA_IFCR_CTEIF3(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CTEIF3_SHIFT)) & DMA_IFCR_CTEIF3_MASK)

#define DMA_IFCR_CHTIF3_SHIFT                   (10)
#define DMA_IFCR_CHTIF3_MASK                    (0x01U << DMA_IFCR_CHTIF3_SHIFT)
#define DMA_IFCR_CHTIF3(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CHTIF3_SHIFT)) & DMA_IFCR_CHTIF3_MASK)

#define DMA_IFCR_CTCIF3_SHIFT                   (9)
#define DMA_IFCR_CTCIF3_MASK                    (0x01U << DMA_IFCR_CTCIF3_SHIFT)
#define DMA_IFCR_CTCIF3(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CTCIF3_SHIFT)) & DMA_IFCR_CTCIF3_MASK)

#define DMA_IFCR_CGIF3_SHIFT                    (8)
#define DMA_IFCR_CGIF3_MASK                     (0x01U << DMA_IFCR_CGIF3_SHIFT)
#define DMA_IFCR_CGIF3(x)                       (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CGIF3_SHIFT)) & DMA_IFCR_CGIF3_MASK)

#define DMA_IFCR_CTEIF2_SHIFT                   (7)
#define DMA_IFCR_CTEIF2_MASK                    (0x01U << DMA_IFCR_CTEIF2_SHIFT)
#define DMA_IFCR_CTEIF2(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CTEIF2_SHIFT)) & DMA_IFCR_CTEIF2_MASK)

#define DMA_IFCR_CHTIF2_SHIFT                   (6)
#define DMA_IFCR_CHTIF2_MASK                    (0x01U << DMA_IFCR_CHTIF2_SHIFT)
#define DMA_IFCR_CHTIF2(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CHTIF2_SHIFT)) & DMA_IFCR_CHTIF2_MASK)

#define DMA_IFCR_CTCIF2_SHIFT                   (5)
#define DMA_IFCR_CTCIF2_MASK                    (0x01U << DMA_IFCR_CTCIF2_SHIFT)
#define DMA_IFCR_CTCIF2(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CTCIF2_SHIFT)) & DMA_IFCR_CTCIF2_MASK)

#define DMA_IFCR_CGIF2_SHIFT                    (4)
#define DMA_IFCR_CGIF2_MASK                     (0x01U << DMA_IFCR_CGIF2_SHIFT)
#define DMA_IFCR_CGIF2(x)                       (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CGIF2_SHIFT)) & DMA_IFCR_CGIF2_MASK)

#define DMA_IFCR_CTEIF1_SHIFT                   (3)
#define DMA_IFCR_CTEIF1_MASK                    (0x01U << DMA_IFCR_CTEIF1_SHIFT)
#define DMA_IFCR_CTEIF1(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CTEIF1_SHIFT)) & DMA_IFCR_CTEIF1_MASK)

#define DMA_IFCR_CHTIF1_SHIFT                   (2)
#define DMA_IFCR_CHTIF1_MASK                    (0x01U << DMA_IFCR_CHTIF1_SHIFT)
#define DMA_IFCR_CHTIF1(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CHTIF1_SHIFT)) & DMA_IFCR_CHTIF1_MASK)

#define DMA_IFCR_CTCIF1_SHIFT                   (1)
#define DMA_IFCR_CTCIF1_MASK                    (0x01U << DMA_IFCR_CTCIF1_SHIFT)
#define DMA_IFCR_CTCIF1(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CTCIF1_SHIFT)) & DMA_IFCR_CTCIF1_MASK)

#define DMA_IFCR_CGIF1_SHIFT                    (0)
#define DMA_IFCR_CGIF1_MASK                     (0x01U << DMA_IFCR_CGIF1_SHIFT)
#define DMA_IFCR_CGIF1(x)                       (((uint32_t)(((uint32_t)(x)) << DMA_IFCR_CGIF1_SHIFT)) & DMA_IFCR_CGIF1_MASK)

/*!
 * @brief DMA_CCR Register Bit Definition
 */

#define DMA_CCR_BURSTEN_SHIFT                   (16)
#define DMA_CCR_BURSTEN_MASK                    (0x01U << DMA_CCR_BURSTEN_SHIFT)
#define DMA_CCR_BURSTEN(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_CCR_BURSTEN_SHIFT)) & DMA_CCR_BURSTEN_MASK)

#define DMA_CCR_ARE_SHIFT                       (15)
#define DMA_CCR_ARE_MASK                        (0x01U << DMA_CCR_ARE_SHIFT)
#define DMA_CCR_ARE(x)                          (((uint32_t)(((uint32_t)(x)) << DMA_CCR_ARE_SHIFT)) & DMA_CCR_ARE_MASK)

#define DMA_CCR_MEM2MEM_SHIFT                   (14)
#define DMA_CCR_MEM2MEM_MASK                    (0x01U << DMA_CCR_MEM2MEM_SHIFT)
#define DMA_CCR_MEM2MEM(x)                      (((uint32_t)(((uint32_t)(x)) << DMA_CCR_MEM2MEM_SHIFT)) & DMA_CCR_MEM2MEM_MASK)

#define DMA_CCR_PL_SHIFT                        (12)
#define DMA_CCR_PL_MASK                         (0x3U << DMA_CCR_PL_SHIFT)
#define DMA_CCR_PL(x)                           (((uint32_t)(((uint32_t)(x)) << DMA_CCR_PL_SHIFT)) & DMA_CCR_PL_MASK)

#define DMA_CCR_MSIZE_SHIFT                     (10)
#define DMA_CCR_MSIZE_MASK                      (0x3U << DMA_CCR_MSIZE_SHIFT)
#define DMA_CCR_MSIZE(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_CCR_MSIZE_SHIFT)) & DMA_CCR_MSIZE_MASK)

#define DMA_CCR_PSIZE_SHIFT                     (8)
#define DMA_CCR_PSIZE_MASK                      (0x3U << DMA_CCR_PSIZE_SHIFT)
#define DMA_CCR_PSIZE(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_CCR_PSIZE_SHIFT)) & DMA_CCR_PSIZE_MASK)

#define DMA_CCR_MINC_SHIFT                      (7)
#define DMA_CCR_MINC_MASK                       (0x01U << DMA_CCR_MINC_SHIFT)
#define DMA_CCR_MINC(x)                         (((uint32_t)(((uint32_t)(x)) << DMA_CCR_MINC_SHIFT)) & DMA_CCR_MINC_MASK)

#define DMA_CCR_PINC_SHIFT                      (6)
#define DMA_CCR_PINC_MASK                       (0x01U << DMA_CCR_PINC_SHIFT)
#define DMA_CCR_PINC(x)                         (((uint32_t)(((uint32_t)(x)) << DMA_CCR_PINC_SHIFT)) & DMA_CCR_PINC_MASK)

#define DMA_CCR_CIRC_SHIFT                      (5)
#define DMA_CCR_CIRC_MASK                       (0x01U << DMA_CCR_CIRC_SHIFT)
#define DMA_CCR_CIRC(x)                         (((uint32_t)(((uint32_t)(x)) << DMA_CCR_CIRC_SHIFT)) & DMA_CCR_CIRC_MASK)

#define DMA_CCR_DIR_SHIFT                       (4)
#define DMA_CCR_DIR_MASK                        (0x01U << DMA_CCR_DIR_SHIFT)
#define DMA_CCR_DIR(x)                          (((uint32_t)(((uint32_t)(x)) << DMA_CCR_DIR_SHIFT)) & DMA_CCR_DIR_MASK)

#define DMA_CCR_TEIE_SHIFT                      (3)
#define DMA_CCR_TEIE_MASK                       (0x01U << DMA_CCR_TEIE_SHIFT)
#define DMA_CCR_TEIE(x)                         (((uint32_t)(((uint32_t)(x)) << DMA_CCR_TEIE_SHIFT)) & DMA_CCR_TEIE_MASK)

#define DMA_CCR_HTIE_SHIFT                      (2)
#define DMA_CCR_HTIE_MASK                       (0x01U << DMA_CCR_HTIE_SHIFT)
#define DMA_CCR_HTIE(x)                         (((uint32_t)(((uint32_t)(x)) << DMA_CCR_HTIE_SHIFT)) & DMA_CCR_HTIE_MASK)

#define DMA_CCR_TCIE_SHIFT                      (1)
#define DMA_CCR_TCIE_MASK                       (0x01U << DMA_CCR_TCIE_SHIFT)
#define DMA_CCR_TCIE(x)                         (((uint32_t)(((uint32_t)(x)) << DMA_CCR_TCIE_SHIFT)) & DMA_CCR_TCIE_MASK)

#define DMA_CCR_EN_SHIFT                        (0)
#define DMA_CCR_EN_MASK                         (0x01U << DMA_CCR_EN_SHIFT)
#define DMA_CCR_EN(x)                           (((uint32_t)(((uint32_t)(x)) << DMA_CCR_EN_SHIFT)) & DMA_CCR_EN_MASK)

/*!
 * @brief DMA_CNDTR Register Bit Definition
 */

#define DMA_CNDTR_NDT_SHIFT                     (0)
#define DMA_CNDTR_NDT_MASK                      (0xFFFFU << DMA_CNDTR_NDT_SHIFT)
#define DMA_CNDTR_NDT(x)                        (((uint32_t)(((uint32_t)(x)) << DMA_CNDTR_NDT_SHIFT)) & DMA_CNDTR_NDT_MASK)

/*!
 * @brief DMA_CPAR Register Bit Definition
 */

#define DMA_CPAR_PA_SHIFT                       (0)
#define DMA_CPAR_PA_MASK                        (0xFFFFFFFFU << DMA_CPAR_PA_SHIFT)
#define DMA_CPAR_PA(x)                          (((uint32_t)(((uint32_t)(x)) << DMA_CPAR_PA_SHIFT)) & DMA_CPAR_PA_MASK)

/*!
 * @brief DMA_CMAR Register Bit Definition
 */

#define DMA_CMAR_MA_SHIFT                       (0)
#define DMA_CMAR_MA_MASK                        (0xFFFFFFFFU << DMA_CMAR_MA_SHIFT)
#define DMA_CMAR_MA(x)                          (((uint32_t)(((uint32_t)(x)) << DMA_CMAR_MA_SHIFT)) & DMA_CMAR_MA_MASK)


/*!
 * @}
 */ /* end of group DMA_Register_Masks */
/******************************************************************************
 *DMA Instance
*******************************************************************************/

#define DMA1                  ((DMA_Type*)DMA1_BASE)
#define DMA2                  ((DMA_Type*)DMA2_BASE)

/*!
 * @}
 */ /* end of group DMA_Peripheral_Access_Layer */

/*!
 * @addtogroup ENET_Peripheral_Access_Layer ENET Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * ENET Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t MACCR;                                                          ///< MAC configuration register                   offset: 0x0000
    __IO uint32_t MACAFR;                                                         ///< MAC address filter control register          offset: 0x0004
    __IO uint32_t MACHTHR;                                                        ///< MAC HASH table high register                 offset: 0x0008
    __IO uint32_t MACHTLR;                                                        ///< MAC HASH table low register                  offset: 0x000C
    __IO uint32_t MACSMIAR;                                                       ///< SMI address register                         offset: 0x0010
    __IO uint32_t MACSMIDR;                                                       ///< SMI data register                            offset: 0x0014
    __IO uint32_t MACFCR;                                                         ///< MAC flow control register                    offset: 0x0018
    __IO uint32_t MACVLTRR;                                                        ///< MAC VLAN Tag Register                       offset: 0x001C
    __IO uint32_t Reserved7[1];                                                  ///< Reserved
    __IO uint32_t MACSR;                                                          ///< MAC status register                          offset: 0x0024
    __IO uint32_t PMTRWFR;                                                        ///< MAC Remote Wakeup Frame Filter Register      offset: 0x0028
    __IO uint32_t PMTCR;                                                          ///< MAC PMT Control Register                     offset: 0x002C
    __IO uint32_t Reserved8[2];                                                  ///< Reserved
    __IO uint32_t MACISR;                                                         ///< MAC Interrupt Status Register               offset: 0x0038
    __IO uint32_t MACIMR;                                                         ///< MAC Interrupt Mask Register                 offset: 0x003C
    struct{
          __IO uint32_t MACAHR;                                                   ///< MAC filter address high register            offset: 0x0040
          __IO uint32_t MACALR;                                                   ///< MAC filter address low register             offset: 0x0044
        }MACAH_L[6];                                                              ///< 0~5 group
    __IO uint32_t MMCCR;                                                          ///< MMC Control Register                         offset: 0x0100
    __IO uint32_t MMCRSR;                                                         ///< MMC Receive Status Register                  offset: 0x0104
    __IO uint32_t MMCTSR;                                                         ///< MMC Transmit Status Register                 offset: 0x0108
    __IO uint32_t MMCRIR;                                                         ///< MMC receive interrupt register               offset: 0x010C
    __IO uint32_t MMCTIR;                                                         ///< MMC Transmit Interrupt Register              offset: 0x0110
	__IO uint32_t Reserved9[14];                                                   ///< Reserved
    __IO uint32_t MMCTGSCR;                                                       ///< MMC sends a good frame statistics register after a single collision error offset: 0x014C
    __IO uint32_t MMCTGMCR;                                                       ///< MMC sends good frame statistics register after multiple collision errors  offset: 0x0150
	__IO uint32_t Reserved10[5];                                                   ///< Reserved
    __IO uint32_t MMCTGR;                                                         ///< MMC sends good frame pass statistics register            offset: 0x0168
	__IO uint32_t Reserved11[10];                                                   ///< Reserved
    __IO uint32_t MMCRCRCER;                                                      ///< MMC CRC Error Received Frame Statistics Register         offset: 0x0194
    __IO uint32_t MMCRALIER;                                                      ///< MMC Alignment Error Received Frame Statistics Register   offset: 0x0198
	__IO uint32_t Reserved12[10];                                                   ///< Reserved
    __IO uint32_t MMCRUGR;                                                        ///< MMC Receive Unicast Good Frame Statistics Register       offset: 0x01C4
	__IO uint32_t Reserved13[239];                                                   ///< Reserved
    __IO uint32_t MACVLTTR;                                                       ///< MAC VLAN TAG send register                   offset: 0x0584
    __IO uint32_t MACVLHTR;                                                       ///< MAC VLAN HASH Table Register                 offset: 0x0588
	__IO uint32_t Reserved14[93];                                                   ///< Reserved
    __IO uint32_t PTPCR;                                                          ///< PTP Control Register                         offset: 0x0700
    __IO uint32_t PTPNSAR;                                                        ///< PTP subsecond adder register                 offset: 0x0704
    __IO uint32_t PTPSBSR;                                                        ///< PTP System Second Register                   offset: 0x0708
    __IO uint32_t PTPSNSR;                                                        ///< PTP System Subsecond Register                offset: 0x070C
    __IO uint32_t PTPUBSR;                                                        ///< PTP Update Seconds Register                  offset: 0x0710
    __IO uint32_t PTPUNSR;                                                        ///< PTP update subsecond register                offset: 0x0714
    __IO uint32_t PTPBSAR;                                                        ///< PTP Basic Addend Register                    offset: 0x0718
    __IO uint32_t PTPTBSR0;                                                       ///< PTP Target Seconds Register 0                offset: 0x071C
    __IO uint32_t PTPTNSR0;                                                       ///< PTP Target Subsecond Register 0              offset: 0x0720
	__IO uint32_t Reserved15[1];                                                   ///< Reserved
    __IO uint32_t PTPSR;                                                          ///< PTP Status Register                          offset: 0x0728
    __IO uint32_t PTPPPSR;                                                        ///< PTP PPS Control Register                     offset: 0x072C
	__IO uint32_t Reserved16[20];                                                 ///< Reserved
    __IO uint32_t PTPTBSR1;                                                       ///< PTP Target Seconds Register 1                offset: 0x0780
    __IO uint32_t PTPTNSR1;                                                       ///< PTP Target Subsecond Register 1              offset: 0x0784
	__IO uint32_t Reserved17[542];                                                   ///< Reserved
    __IO uint32_t DMABSR;                                                         ///< DMA bus control register                     offset: 0x1000
    __IO uint32_t DMATXPDR;                                                       ///< DMA transmit polling register                offset: 0x1004
    __IO uint32_t DMARXPDR;                                                       ///< DMA Receive Polling Register                 offset: 0x1008
    __IO uint32_t DMARXDSAR;                                                      ///< DMA Receive Descriptor Address Register      offset: 0x100C
    __IO uint32_t DMATXDSAR;                                                      ///< DMA transmit descriptor address register     offset: 0x1010
    __IO uint32_t DMASR;                                                          ///< DMA status register                          offset: 0x1014
    __IO uint32_t DMAMDR;                                                         ///< DMA work mode register                       offset: 0x1018
    __IO uint32_t DMAIR;                                                          ///< DMA Interrupt Register                       offset: 0x101C
    __IO uint32_t DMAFLCR;                                                        ///< DMA Frame Loss Statistics Register           offset: 0x1020
    __IO uint32_t DMAWDTR;                                                        ///< DMA Watchdog Timing Register                 offset: 0x1024
	__IO uint32_t Reserved18[8];                                                   ///< Reserved
    __IO uint32_t DMACURTXDSAR;                                                   ///< DMA current transmit descriptor address registeroffset: 0x1048
    __IO uint32_t DMACURRXDSAR;                                                   ///< DMA current receive descriptor address registeroffset: 0x104C
    __IO uint32_t DMACURTXBFAR;                                                   ///< DMA current send buffer address register     offset: 0x1050
    __IO uint32_t DMACURRXBFAR;                                                   ///< DMA current receive buffer address register  offset: 0x1054

} ENET_Type;

/*******************************************************************************
 * ENET Type
 ******************************************************************************/

/*!
 * @addtogroup ENET_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief ENET_MACSMIAR Register Bit Definition
 */

#define ENET_MACSMIAR_SMIADD_SHIFT              (11)
#define ENET_MACSMIAR_SMIADD_MASK               (0x1FU << ENET_MACSMIAR_SMIADD_SHIFT)
#define ENET_MACSMIAR_SMIADD(x)                 (((uint32_t)(((uint32_t)(x)) << ENET_MACSMIAR_SMIADD_SHIFT)) & ENET_MACSMIAR_SMIADD_MASK)

#define ENET_MACSMIAR_SMIREG_SHIFT              (6)
#define ENET_MACSMIAR_SMIREG_MASK               (0x1FU << ENET_MACSMIAR_SMIREG_SHIFT)
#define ENET_MACSMIAR_SMIREG(x)                 (((uint32_t)(((uint32_t)(x)) << ENET_MACSMIAR_SMIREG_SHIFT)) & ENET_MACSMIAR_SMIREG_MASK)

#define ENET_MACSMIAR_SMICLK_SHIFT              (2)
#define ENET_MACSMIAR_SMICLK_MASK               (0xFU << ENET_MACSMIAR_SMICLK_SHIFT)
#define ENET_MACSMIAR_SMICLK(x)                 (((uint32_t)(((uint32_t)(x)) << ENET_MACSMIAR_SMICLK_SHIFT)) & ENET_MACSMIAR_SMICLK_MASK)

#define ENET_MACSMIAR_SMIWR_SHIFT               (1)
#define ENET_MACSMIAR_SMIWR_MASK                (0x01U << ENET_MACSMIAR_SMIWR_SHIFT)
#define ENET_MACSMIAR_SMIWR(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_MACSMIAR_SMIWR_SHIFT)) & ENET_MACSMIAR_SMIWR_MASK)

#define ENET_MACSMIAR_SMIBY_SHIFT               (0)
#define ENET_MACSMIAR_SMIBY_MASK                (0x01U << ENET_MACSMIAR_SMIBY_SHIFT)
#define ENET_MACSMIAR_SMIBY(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_MACSMIAR_SMIBY_SHIFT)) & ENET_MACSMIAR_SMIBY_MASK)

/*!
 * @brief ENET_MACSMIDR Register Bit Definition
 */

#define ENET_MACSMIDR_SMIDATA_SHIFT             (0)
#define ENET_MACSMIDR_SMIDATA_MASK              (0xFFFFU << ENET_MACSMIDR_SMIDATA_SHIFT)
#define ENET_MACSMIDR_SMIDATA(x)                (((uint32_t)(((uint32_t)(x)) << ENET_MACSMIDR_SMIDATA_SHIFT)) & ENET_MACSMIDR_SMIDATA_MASK)

/*!
 * @brief ENET_MACCR Register Bit Definition
 */

#define ENET_MACCR_SAIRC_SHIFT                  (28)
#define ENET_MACCR_SAIRC_MASK                   (0xFU << ENET_MACCR_SAIRC_SHIFT)
#define ENET_MACCR_SAIRC(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_MACCR_SAIRC_SHIFT)) & ENET_MACCR_SAIRC_MASK)

#define ENET_MACCR_CST_SHIFT                    (25)
#define ENET_MACCR_CST_MASK                     (0x01U << ENET_MACCR_CST_SHIFT)
#define ENET_MACCR_CST(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_MACCR_CST_SHIFT)) & ENET_MACCR_CST_MASK)

#define ENET_MACCR_WTD_SHIFT                    (23)
#define ENET_MACCR_WTD_MASK                     (0x01U << ENET_MACCR_WTD_SHIFT)
#define ENET_MACCR_WTD(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_MACCR_WTD_SHIFT)) & ENET_MACCR_WTD_MASK)

#define ENET_MACCR_JAB_SHIFT                    (22)
#define ENET_MACCR_JAB_MASK                     (0x01U << ENET_MACCR_JAB_SHIFT)
#define ENET_MACCR_JAB(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_MACCR_JAB_SHIFT)) & ENET_MACCR_JAB_MASK)

#define ENET_MACCR_IFG_SHIFT                    (17)
#define ENET_MACCR_IFG_MASK                     (0x7U << ENET_MACCR_IFG_SHIFT)
#define ENET_MACCR_IFG(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_MACCR_IFG_SHIFT)) & ENET_MACCR_IFG_MASK)

#define ENET_MACCR_MCRS_SHIFT                   (16)
#define ENET_MACCR_MCRS_MASK                    (0x01U << ENET_MACCR_MCRS_SHIFT)
#define ENET_MACCR_MCRS(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MACCR_MCRS_SHIFT)) & ENET_MACCR_MCRS_MASK)

#define ENET_MACCR_RDIS_SHIFT                   (13)
#define ENET_MACCR_RDIS_MASK                    (0x01U << ENET_MACCR_RDIS_SHIFT)
#define ENET_MACCR_RDIS(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MACCR_RDIS_SHIFT)) & ENET_MACCR_RDIS_MASK)

#define ENET_MACCR_LM_SHIFT                     (12)
#define ENET_MACCR_LM_MASK                      (0x01U << ENET_MACCR_LM_SHIFT)
#define ENET_MACCR_LM(x)                        (((uint32_t)(((uint32_t)(x)) << ENET_MACCR_LM_SHIFT)) & ENET_MACCR_LM_MASK)

#define ENET_MACCR_DM_SHIFT                     (11)
#define ENET_MACCR_DM_MASK                      (0x01U << ENET_MACCR_DM_SHIFT)
#define ENET_MACCR_DM(x)                        (((uint32_t)(((uint32_t)(x)) << ENET_MACCR_DM_SHIFT)) & ENET_MACCR_DM_MASK)

#define ENET_MACCR_IPCO_SHIFT                   (10)
#define ENET_MACCR_IPCO_MASK                    (0x01U << ENET_MACCR_IPCO_SHIFT)
#define ENET_MACCR_IPCO(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MACCR_IPCO_SHIFT)) & ENET_MACCR_IPCO_MASK)

#define ENET_MACCR_RETY_SHIFT                   (9)
#define ENET_MACCR_RETY_MASK                    (0x01U << ENET_MACCR_RETY_SHIFT)
#define ENET_MACCR_RETY(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MACCR_RETY_SHIFT)) & ENET_MACCR_RETY_MASK)

#define ENET_MACCR_APCS_SHIFT                   (7)
#define ENET_MACCR_APCS_MASK                    (0x01U << ENET_MACCR_APCS_SHIFT)
#define ENET_MACCR_APCS(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MACCR_APCS_SHIFT)) & ENET_MACCR_APCS_MASK)

#define ENET_MACCR_BL_SHIFT                     (5)
#define ENET_MACCR_BL_MASK                      (0x3U << ENET_MACCR_BL_SHIFT)
#define ENET_MACCR_BL(x)                        (((uint32_t)(((uint32_t)(x)) << ENET_MACCR_BL_SHIFT)) & ENET_MACCR_BL_MASK)

#define ENET_MACCR_DLYC_SHIFT                   (4)
#define ENET_MACCR_DLYC_MASK                    (0x01U << ENET_MACCR_DLYC_SHIFT)
#define ENET_MACCR_DLYC(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MACCR_DLYC_SHIFT)) & ENET_MACCR_DLYC_MASK)

#define ENET_MACCR_TE_SHIFT                     (3)
#define ENET_MACCR_TE_MASK                      (0x01U << ENET_MACCR_TE_SHIFT)
#define ENET_MACCR_TE(x)                        (((uint32_t)(((uint32_t)(x)) << ENET_MACCR_TE_SHIFT)) & ENET_MACCR_TE_MASK)

#define ENET_MACCR_RE_SHIFT                     (2)
#define ENET_MACCR_RE_MASK                      (0x01U << ENET_MACCR_RE_SHIFT)
#define ENET_MACCR_RE(x)                        (((uint32_t)(((uint32_t)(x)) << ENET_MACCR_RE_SHIFT)) & ENET_MACCR_RE_MASK)

/*!
 * @brief ENET_MACSR Register Bit Definition
 */

#define ENET_MACSR_TXFF_SHIFT                   (25)
#define ENET_MACSR_TXFF_MASK                    (0x01U << ENET_MACSR_TXFF_SHIFT)
#define ENET_MACSR_TXFF(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MACSR_TXFF_SHIFT)) & ENET_MACSR_TXFF_MASK)

#define ENET_MACSR_TXFNE_SHIFT                  (24)
#define ENET_MACSR_TXFNE_MASK                   (0x01U << ENET_MACSR_TXFNE_SHIFT)
#define ENET_MACSR_TXFNE(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_MACSR_TXFNE_SHIFT)) & ENET_MACSR_TXFNE_MASK)

#define ENET_MACSR_TXFWA_SHIFT                  (22)
#define ENET_MACSR_TXFWA_MASK                   (0x01U << ENET_MACSR_TXFWA_SHIFT)
#define ENET_MACSR_TXFWA(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_MACSR_TXFWA_SHIFT)) & ENET_MACSR_TXFWA_MASK)

#define ENET_MACSR_TXFRS_SHIFT                  (20)
#define ENET_MACSR_TXFRS_MASK                   (0x3U << ENET_MACSR_TXFRS_SHIFT)
#define ENET_MACSR_TXFRS(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_MACSR_TXFRS_SHIFT)) & ENET_MACSR_TXFRS_MASK)

#define ENET_MACSR_MTP_SHIFT                    (19)
#define ENET_MACSR_MTP_MASK                     (0x01U << ENET_MACSR_MTP_SHIFT)
#define ENET_MACSR_MTP(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_MACSR_MTP_SHIFT)) & ENET_MACSR_MTP_MASK)

#define ENET_MACSR_MTS_SHIFT                    (17)
#define ENET_MACSR_MTS_MASK                     (0x3U << ENET_MACSR_MTS_SHIFT)
#define ENET_MACSR_MTS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_MACSR_MTS_SHIFT)) & ENET_MACSR_MTS_MASK)

#define ENET_MACSR_MTE_SHIFT                    (16)
#define ENET_MACSR_MTE_MASK                     (0x01U << ENET_MACSR_MTE_SHIFT)
#define ENET_MACSR_MTE(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_MACSR_MTE_SHIFT)) & ENET_MACSR_MTE_MASK)

#define ENET_MACSR_RXFL_SHIFT                   (8)
#define ENET_MACSR_RXFL_MASK                    (0x3U << ENET_MACSR_RXFL_SHIFT)
#define ENET_MACSR_RXFL(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MACSR_RXFL_SHIFT)) & ENET_MACSR_RXFL_MASK)

#define ENET_MACSR_RXFRS_SHIFT                  (5)
#define ENET_MACSR_RXFRS_MASK                   (0x3U << ENET_MACSR_RXFRS_SHIFT)
#define ENET_MACSR_RXFRS(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_MACSR_RXFRS_SHIFT)) & ENET_MACSR_RXFRS_MASK)

#define ENET_MACSR_RXFWA_SHIFT                  (4)
#define ENET_MACSR_RXFWA_MASK                   (0x01U << ENET_MACSR_RXFWA_SHIFT)
#define ENET_MACSR_RXFWA(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_MACSR_RXFWA_SHIFT)) & ENET_MACSR_RXFWA_MASK)

#define ENET_MACSR_MRS_SHIFT                    (1)
#define ENET_MACSR_MRS_MASK                     (0x3U << ENET_MACSR_MRS_SHIFT)
#define ENET_MACSR_MRS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_MACSR_MRS_SHIFT)) & ENET_MACSR_MRS_MASK)

#define ENET_MACSR_MRE_SHIFT                    (0)
#define ENET_MACSR_MRE_MASK                     (0x01U << ENET_MACSR_MRE_SHIFT)
#define ENET_MACSR_MRE(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_MACSR_MRE_SHIFT)) & ENET_MACSR_MRE_MASK)

/*!
 * @brief ENET_MACFCR Register Bit Definition
 */

#define ENET_MACFCR_PSET_SHIFT                  (16)
#define ENET_MACFCR_PSET_MASK                   (0xFFFFU << ENET_MACFCR_PSET_SHIFT)
#define ENET_MACFCR_PSET(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_MACFCR_PSET_SHIFT)) & ENET_MACFCR_PSET_MASK)

#define ENET_MACFCR_DZQP_SHIFT                  (7)
#define ENET_MACFCR_DZQP_MASK                   (0x01U << ENET_MACFCR_DZQP_SHIFT)
#define ENET_MACFCR_DZQP(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_MACFCR_DZQP_SHIFT)) & ENET_MACFCR_DZQP_MASK)

#define ENET_MACFCR_PLT_SHIFT                   (4)
#define ENET_MACFCR_PLT_MASK                    (0x3U << ENET_MACFCR_PLT_SHIFT)
#define ENET_MACFCR_PLT(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MACFCR_PLT_SHIFT)) & ENET_MACFCR_PLT_MASK)

#define ENET_MACFCR_UPF_SHIFT                   (3)
#define ENET_MACFCR_UPF_MASK                    (0x01U << ENET_MACFCR_UPF_SHIFT)
#define ENET_MACFCR_UPF(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MACFCR_UPF_SHIFT)) & ENET_MACFCR_UPF_MASK)

#define ENET_MACFCR_FRE_SHIFT                   (2)
#define ENET_MACFCR_FRE_MASK                    (0x01U << ENET_MACFCR_FRE_SHIFT)
#define ENET_MACFCR_FRE(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MACFCR_FRE_SHIFT)) & ENET_MACFCR_FRE_MASK)

#define ENET_MACFCR_FTE_SHIFT                   (1)
#define ENET_MACFCR_FTE_MASK                    (0x01U << ENET_MACFCR_FTE_SHIFT)
#define ENET_MACFCR_FTE(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MACFCR_FTE_SHIFT)) & ENET_MACFCR_FTE_MASK)

#define ENET_MACFCR_FCBBPA_SHIFT                (0)
#define ENET_MACFCR_FCBBPA_MASK                 (0x01U << ENET_MACFCR_FCBBPA_SHIFT)
#define ENET_MACFCR_FCBBPA(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_MACFCR_FCBBPA_SHIFT)) & ENET_MACFCR_FCBBPA_MASK)

/*!
 * @brief ENET_MACISR Register Bit Definition
 */

#define ENET_MACISR_PTPIS_SHIFT                 (9)
#define ENET_MACISR_PTPIS_MASK                  (0x01U << ENET_MACISR_PTPIS_SHIFT)
#define ENET_MACISR_PTPIS(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_MACISR_PTPIS_SHIFT)) & ENET_MACISR_PTPIS_MASK)

#define ENET_MACISR_MMCTXIS_SHIFT               (6)
#define ENET_MACISR_MMCTXIS_MASK                (0x01U << ENET_MACISR_MMCTXIS_SHIFT)
#define ENET_MACISR_MMCTXIS(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_MACISR_MMCTXIS_SHIFT)) & ENET_MACISR_MMCTXIS_MASK)

#define ENET_MACISR_MMCRXIS_SHIFT               (5)
#define ENET_MACISR_MMCRXIS_MASK                (0x01U << ENET_MACISR_MMCRXIS_SHIFT)
#define ENET_MACISR_MMCRXIS(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_MACISR_MMCRXIS_SHIFT)) & ENET_MACISR_MMCRXIS_MASK)

#define ENET_MACISR_MMCIS_SHIFT                 (4)
#define ENET_MACISR_MMCIS_MASK                  (0x01U << ENET_MACISR_MMCIS_SHIFT)
#define ENET_MACISR_MMCIS(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_MACISR_MMCIS_SHIFT)) & ENET_MACISR_MMCIS_MASK)

#define ENET_MACISR_PMTIS_SHIFT                 (3)
#define ENET_MACISR_PMTIS_MASK                  (0x01U << ENET_MACISR_PMTIS_SHIFT)
#define ENET_MACISR_PMTIS(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_MACISR_PMTIS_SHIFT)) & ENET_MACISR_PMTIS_MASK)

/*!
 * @brief ENET_MACIMR Register Bit Definition
 */

#define ENET_MACIMR_PTPIM_SHIFT                 (9)
#define ENET_MACIMR_PTPIM_MASK                  (0x01U << ENET_MACIMR_PTPIM_SHIFT)
#define ENET_MACIMR_PTPIM(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_MACIMR_PTPIM_SHIFT)) & ENET_MACIMR_PTPIM_MASK)

#define ENET_MACIMR_PMTIM_SHIFT                 (3)
#define ENET_MACIMR_PMTIM_MASK                  (0x01U << ENET_MACIMR_PMTIM_SHIFT)
#define ENET_MACIMR_PMTIM(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_MACIMR_PMTIM_SHIFT)) & ENET_MACIMR_PMTIM_MASK)

/*!
 * @brief ENET_MACAFR Register Bit Definition
 */

#define ENET_MACAFR_RALL_SHIFT                  (31)
#define ENET_MACAFR_RALL_MASK                   (0x01U << ENET_MACAFR_RALL_SHIFT)
#define ENET_MACAFR_RALL(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_MACAFR_RALL_SHIFT)) & ENET_MACAFR_RALL_MASK)

#define ENET_MACAFR_DNTU_SHIFT                  (21)
#define ENET_MACAFR_DNTU_MASK                   (0x01U << ENET_MACAFR_DNTU_SHIFT)
#define ENET_MACAFR_DNTU(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_MACAFR_DNTU_SHIFT)) & ENET_MACAFR_DNTU_MASK)

#define ENET_MACAFR_VLTF_SHIFT                  (16)
#define ENET_MACAFR_VLTF_MASK                   (0x01U << ENET_MACAFR_VLTF_SHIFT)
#define ENET_MACAFR_VLTF(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_MACAFR_VLTF_SHIFT)) & ENET_MACAFR_VLTF_MASK)

#define ENET_MACAFR_HPF_SHIFT                   (10)
#define ENET_MACAFR_HPF_MASK                    (0x01U << ENET_MACAFR_HPF_SHIFT)
#define ENET_MACAFR_HPF(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MACAFR_HPF_SHIFT)) & ENET_MACAFR_HPF_MASK)

#define ENET_MACAFR_SAF_SHIFT                   (9)
#define ENET_MACAFR_SAF_MASK                    (0x01U << ENET_MACAFR_SAF_SHIFT)
#define ENET_MACAFR_SAF(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MACAFR_SAF_SHIFT)) & ENET_MACAFR_SAF_MASK)

#define ENET_MACAFR_SAIF_SHIFT                  (8)
#define ENET_MACAFR_SAIF_MASK                   (0x01U << ENET_MACAFR_SAIF_SHIFT)
#define ENET_MACAFR_SAIF(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_MACAFR_SAIF_SHIFT)) & ENET_MACAFR_SAIF_MASK)

#define ENET_MACAFR_PCF_SHIFT                   (6)
#define ENET_MACAFR_PCF_MASK                    (0x3U << ENET_MACAFR_PCF_SHIFT)
#define ENET_MACAFR_PCF(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MACAFR_PCF_SHIFT)) & ENET_MACAFR_PCF_MASK)

#define ENET_MACAFR_BCF_SHIFT                   (5)
#define ENET_MACAFR_BCF_MASK                    (0x01U << ENET_MACAFR_BCF_SHIFT)
#define ENET_MACAFR_BCF(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MACAFR_BCF_SHIFT)) & ENET_MACAFR_BCF_MASK)

#define ENET_MACAFR_MCF_SHIFT                   (4)
#define ENET_MACAFR_MCF_MASK                    (0x01U << ENET_MACAFR_MCF_SHIFT)
#define ENET_MACAFR_MCF(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MACAFR_MCF_SHIFT)) & ENET_MACAFR_MCF_MASK)

#define ENET_MACAFR_DAIF_SHIFT                  (3)
#define ENET_MACAFR_DAIF_MASK                   (0x01U << ENET_MACAFR_DAIF_SHIFT)
#define ENET_MACAFR_DAIF(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_MACAFR_DAIF_SHIFT)) & ENET_MACAFR_DAIF_MASK)

#define ENET_MACAFR_HM_SHIFT                    (2)
#define ENET_MACAFR_HM_MASK                     (0x01U << ENET_MACAFR_HM_SHIFT)
#define ENET_MACAFR_HM(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_MACAFR_HM_SHIFT)) & ENET_MACAFR_HM_MASK)

#define ENET_MACAFR_HU_SHIFT                    (1)
#define ENET_MACAFR_HU_MASK                     (0x01U << ENET_MACAFR_HU_SHIFT)
#define ENET_MACAFR_HU(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_MACAFR_HU_SHIFT)) & ENET_MACAFR_HU_MASK)

#define ENET_MACAFR_PM_SHIFT                    (0)
#define ENET_MACAFR_PM_MASK                     (0x01U << ENET_MACAFR_PM_SHIFT)
#define ENET_MACAFR_PM(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_MACAFR_PM_SHIFT)) & ENET_MACAFR_PM_MASK)

/*!
 * @brief ENET_MACAHR Register Bit Definition
 */

#define ENET_MACAHR_ADDE_SHIFT                  (31)
#define ENET_MACAHR_ADDE_MASK                   (0x01U << ENET_MACAHR_ADDE_SHIFT)
#define ENET_MACAHR_ADDE(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_MACAHR_ADDE_SHIFT)) & ENET_MACAHR_ADDE_MASK)

#define ENET_MACAHR_SELE_SHIFT                  (30)
#define ENET_MACAHR_SELE_MASK                   (0x01U << ENET_MACAHR_SELE_SHIFT)
#define ENET_MACAHR_SELE(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_MACAHR_SELE_SHIFT)) & ENET_MACAHR_SELE_MASK)

#define ENET_MACAHR_MBYTEC_SHIFT                (24)
#define ENET_MACAHR_MBYTEC_MASK                 (0x3FU << ENET_MACAHR_MBYTEC_SHIFT)
#define ENET_MACAHR_MBYTEC(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_MACAHR_MBYTEC_SHIFT)) & ENET_MACAHR_MBYTEC_MASK)

#define ENET_MACAHR_MACADDH_SHIFT               (0)
#define ENET_MACAHR_MACADDH_MASK                (0xFFFFU << ENET_MACAHR_MACADDH_SHIFT)
#define ENET_MACAHR_MACADDH(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_MACAHR_MACADDH_SHIFT)) & ENET_MACAHR_MACADDH_MASK)

/*!
 * @brief ENET_MACALR0 Register Bit Definition
 */

#define ENET_MACALR0_MACADDL_SHIFT              (0)
#define ENET_MACALR0_MACADDL_MASK               (0xFFFFFFFFU << ENET_MACALR0_MACADDL_SHIFT)
#define ENET_MACALR0_MACADDL(x)                 (((uint32_t)(((uint32_t)(x)) << ENET_MACALR0_MACADDL_SHIFT)) & ENET_MACALR0_MACADDL_MASK)

/*!
 * @brief ENET_MACHTHR Register Bit Definition
 */

#define ENET_MACHTHR_MACHTABH_SHIFT             (0)
#define ENET_MACHTHR_MACHTABH_MASK              (0xFFFFFFFFU << ENET_MACHTHR_MACHTABH_SHIFT)
#define ENET_MACHTHR_MACHTABH(x)                (((uint32_t)(((uint32_t)(x)) << ENET_MACHTHR_MACHTABH_SHIFT)) & ENET_MACHTHR_MACHTABH_MASK)

/*!
 * @brief ENET_MACHTLR Register Bit Definition
 */

#define ENET_MACHTLR_MACHTABL_SHIFT             (0)
#define ENET_MACHTLR_MACHTABL_MASK              (0xFFFFFFFFU << ENET_MACHTLR_MACHTABL_SHIFT)
#define ENET_MACHTLR_MACHTABL(x)                (((uint32_t)(((uint32_t)(x)) << ENET_MACHTLR_MACHTABL_SHIFT)) & ENET_MACHTLR_MACHTABL_MASK)

/*!
 * @brief ENET_MACVLTTR Register Bit Definition
 */

#define ENET_MACVLTTR_VLANS_SHIFT               (18)
#define ENET_MACVLTTR_VLANS_MASK                (0x01U << ENET_MACVLTTR_VLANS_SHIFT)
#define ENET_MACVLTTR_VLANS(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_MACVLTTR_VLANS_SHIFT)) & ENET_MACVLTTR_VLANS_MASK)

#define ENET_MACVLTTR_VLANC_SHIFT               (16)
#define ENET_MACVLTTR_VLANC_MASK                (0x3U << ENET_MACVLTTR_VLANC_SHIFT)
#define ENET_MACVLTTR_VLANC(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_MACVLTTR_VLANC_SHIFT)) & ENET_MACVLTTR_VLANC_MASK)

#define ENET_MACVLTTR_VLANV_SHIFT               (0)
#define ENET_MACVLTTR_VLANV_MASK                (0xFFFFU << ENET_MACVLTTR_VLANV_SHIFT)
#define ENET_MACVLTTR_VLANV(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_MACVLTTR_VLANV_SHIFT)) & ENET_MACVLTTR_VLANV_MASK)

/*!
 * @brief ENET_MACVLTRR Register Bit Definition
 */

#define ENET_MACVLTRR_VTHM_SHIFT                (19)
#define ENET_MACVLTRR_VTHM_MASK                 (0x01U << ENET_MACVLTRR_VTHM_SHIFT)
#define ENET_MACVLTRR_VTHM(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_MACVLTRR_VTHM_SHIFT)) & ENET_MACVLTRR_VTHM_MASK)

#define ENET_MACVLTRR_VTIM_SHIFT                (17)
#define ENET_MACVLTRR_VTIM_MASK                 (0x01U << ENET_MACVLTRR_VTIM_SHIFT)
#define ENET_MACVLTRR_VTIM(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_MACVLTRR_VTIM_SHIFT)) & ENET_MACVLTRR_VTIM_MASK)

#define ENET_MACVLTRR_VTSEL_SHIFT               (16)
#define ENET_MACVLTRR_VTSEL_MASK                (0x01U << ENET_MACVLTRR_VTSEL_SHIFT)
#define ENET_MACVLTRR_VTSEL(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_MACVLTRR_VTSEL_SHIFT)) & ENET_MACVLTRR_VTSEL_MASK)

#define ENET_MACVLTRR_VLFLT_SHIFT               (0)
#define ENET_MACVLTRR_VLFLT_MASK                (0xFFFFU << ENET_MACVLTRR_VLFLT_SHIFT)
#define ENET_MACVLTRR_VLFLT(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_MACVLTRR_VLFLT_SHIFT)) & ENET_MACVLTRR_VLFLT_MASK)

/*!
 * @brief ENET_MACVLHTR Register Bit Definition
 */

#define ENET_MACVLHTR_VLHTAB_SHIFT              (0)
#define ENET_MACVLHTR_VLHTAB_MASK               (0xFFFFU << ENET_MACVLHTR_VLHTAB_SHIFT)
#define ENET_MACVLHTR_VLHTAB(x)                 (((uint32_t)(((uint32_t)(x)) << ENET_MACVLHTR_VLHTAB_SHIFT)) & ENET_MACVLHTR_VLHTAB_MASK)

/*!
 * @brief ENET_DMAMDR Register Bit Definition
 */

#define ENET_DMAMDR_DTCOE_SHIFT                 (26)
#define ENET_DMAMDR_DTCOE_MASK                  (0x01U << ENET_DMAMDR_DTCOE_SHIFT)
#define ENET_DMAMDR_DTCOE(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_DMAMDR_DTCOE_SHIFT)) & ENET_DMAMDR_DTCOE_MASK)

#define ENET_DMAMDR_RSF_SHIFT                   (25)
#define ENET_DMAMDR_RSF_MASK                    (0x01U << ENET_DMAMDR_RSF_SHIFT)
#define ENET_DMAMDR_RSF(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_DMAMDR_RSF_SHIFT)) & ENET_DMAMDR_RSF_MASK)

#define ENET_DMAMDR_DFRF_SHIFT                  (24)
#define ENET_DMAMDR_DFRF_MASK                   (0x01U << ENET_DMAMDR_DFRF_SHIFT)
#define ENET_DMAMDR_DFRF(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_DMAMDR_DFRF_SHIFT)) & ENET_DMAMDR_DFRF_MASK)

#define ENET_DMAMDR_TSF_SHIFT                   (21)
#define ENET_DMAMDR_TSF_MASK                    (0x01U << ENET_DMAMDR_TSF_SHIFT)
#define ENET_DMAMDR_TSF(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_DMAMDR_TSF_SHIFT)) & ENET_DMAMDR_TSF_MASK)

#define ENET_DMAMDR_FTF_SHIFT                   (20)
#define ENET_DMAMDR_FTF_MASK                    (0x01U << ENET_DMAMDR_FTF_SHIFT)
#define ENET_DMAMDR_FTF(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_DMAMDR_FTF_SHIFT)) & ENET_DMAMDR_FTF_MASK)

#define ENET_DMAMDR_TTC_SHIFT                   (14)
#define ENET_DMAMDR_TTC_MASK                    (0x7U << ENET_DMAMDR_TTC_SHIFT)
#define ENET_DMAMDR_TTC(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_DMAMDR_TTC_SHIFT)) & ENET_DMAMDR_TTC_MASK)

#define ENET_DMAMDR_STT_SHIFT                   (13)
#define ENET_DMAMDR_STT_MASK                    (0x01U << ENET_DMAMDR_STT_SHIFT)
#define ENET_DMAMDR_STT(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_DMAMDR_STT_SHIFT)) & ENET_DMAMDR_STT_MASK)

#define ENET_DMAMDR_FEF_SHIFT                   (7)
#define ENET_DMAMDR_FEF_MASK                    (0x01U << ENET_DMAMDR_FEF_SHIFT)
#define ENET_DMAMDR_FEF(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_DMAMDR_FEF_SHIFT)) & ENET_DMAMDR_FEF_MASK)

#define ENET_DMAMDR_FUF_SHIFT                   (6)
#define ENET_DMAMDR_FUF_MASK                    (0x01U << ENET_DMAMDR_FUF_SHIFT)
#define ENET_DMAMDR_FUF(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_DMAMDR_FUF_SHIFT)) & ENET_DMAMDR_FUF_MASK)

#define ENET_DMAMDR_DGF_SHIFT                   (5)
#define ENET_DMAMDR_DGF_MASK                    (0x01U << ENET_DMAMDR_DGF_SHIFT)
#define ENET_DMAMDR_DGF(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_DMAMDR_DGF_SHIFT)) & ENET_DMAMDR_DGF_MASK)

#define ENET_DMAMDR_RTC_SHIFT                   (3)
#define ENET_DMAMDR_RTC_MASK                    (0x3U << ENET_DMAMDR_RTC_SHIFT)
#define ENET_DMAMDR_RTC(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_DMAMDR_RTC_SHIFT)) & ENET_DMAMDR_RTC_MASK)

#define ENET_DMAMDR_OSF_SHIFT                   (2)
#define ENET_DMAMDR_OSF_MASK                    (0x01U << ENET_DMAMDR_OSF_SHIFT)
#define ENET_DMAMDR_OSF(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_DMAMDR_OSF_SHIFT)) & ENET_DMAMDR_OSF_MASK)

#define ENET_DMAMDR_STR_SHIFT                   (1)
#define ENET_DMAMDR_STR_MASK                    (0x01U << ENET_DMAMDR_STR_SHIFT)
#define ENET_DMAMDR_STR(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_DMAMDR_STR_SHIFT)) & ENET_DMAMDR_STR_MASK)

/*!
 * @brief ENET_DMABSR Register Bit Definition
 */

#define ENET_DMABSR_TXPR_SHIFT                  (27)
#define ENET_DMABSR_TXPR_MASK                   (0x01U << ENET_DMABSR_TXPR_SHIFT)
#define ENET_DMABSR_TXPR(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_DMABSR_TXPR_SHIFT)) & ENET_DMABSR_TXPR_MASK)

#define ENET_DMABSR_MBST_SHIFT                  (26)
#define ENET_DMABSR_MBST_MASK                   (0x01U << ENET_DMABSR_MBST_SHIFT)
#define ENET_DMABSR_MBST(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_DMABSR_MBST_SHIFT)) & ENET_DMABSR_MBST_MASK)

#define ENET_DMABSR_AAL_SHIFT                   (25)
#define ENET_DMABSR_AAL_MASK                    (0x01U << ENET_DMABSR_AAL_SHIFT)
#define ENET_DMABSR_AAL(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_DMABSR_AAL_SHIFT)) & ENET_DMABSR_AAL_MASK)

#define ENET_DMABSR_MPBL_SHIFT                  (24)
#define ENET_DMABSR_MPBL_MASK                   (0x01U << ENET_DMABSR_MPBL_SHIFT)
#define ENET_DMABSR_MPBL(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_DMABSR_MPBL_SHIFT)) & ENET_DMABSR_MPBL_MASK)

#define ENET_DMABSR_SPBL_SHIFT                  (23)
#define ENET_DMABSR_SPBL_MASK                   (0x01U << ENET_DMABSR_SPBL_SHIFT)
#define ENET_DMABSR_SPBL(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_DMABSR_SPBL_SHIFT)) & ENET_DMABSR_SPBL_MASK)

#define ENET_DMABSR_RPBL_SHIFT                  (17)
#define ENET_DMABSR_RPBL_MASK                   (0x3FU << ENET_DMABSR_RPBL_SHIFT)
#define ENET_DMABSR_RPBL(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_DMABSR_RPBL_SHIFT)) & ENET_DMABSR_RPBL_MASK)

#define ENET_DMABSR_FBST_SHIFT                  (16)
#define ENET_DMABSR_FBST_MASK                   (0x01U << ENET_DMABSR_FBST_SHIFT)
#define ENET_DMABSR_FBST(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_DMABSR_FBST_SHIFT)) & ENET_DMABSR_FBST_MASK)

#define ENET_DMABSR_FTPR_SHIFT                  (14)
#define ENET_DMABSR_FTPR_MASK                   (0x3U << ENET_DMABSR_FTPR_SHIFT)
#define ENET_DMABSR_FTPR(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_DMABSR_FTPR_SHIFT)) & ENET_DMABSR_FTPR_MASK)

#define ENET_DMABSR_TPBL_SHIFT                  (8)
#define ENET_DMABSR_TPBL_MASK                   (0x3FU << ENET_DMABSR_TPBL_SHIFT)
#define ENET_DMABSR_TPBL(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_DMABSR_TPBL_SHIFT)) & ENET_DMABSR_TPBL_MASK)

#define ENET_DMABSR_DSEN_SHIFT                  (7)
#define ENET_DMABSR_DSEN_MASK                   (0x01U << ENET_DMABSR_DSEN_SHIFT)
#define ENET_DMABSR_DSEN(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_DMABSR_DSEN_SHIFT)) & ENET_DMABSR_DSEN_MASK)

#define ENET_DMABSR_DSL_SHIFT                   (2)
#define ENET_DMABSR_DSL_MASK                    (0x1FU << ENET_DMABSR_DSL_SHIFT)
#define ENET_DMABSR_DSL(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_DMABSR_DSL_SHIFT)) & ENET_DMABSR_DSL_MASK)

#define ENET_DMABSR_DMAA_SHIFT                  (1)
#define ENET_DMABSR_DMAA_MASK                   (0x01U << ENET_DMABSR_DMAA_SHIFT)
#define ENET_DMABSR_DMAA(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_DMABSR_DMAA_SHIFT)) & ENET_DMABSR_DMAA_MASK)

#define ENET_DMABSR_SR_SHIFT                    (0)
#define ENET_DMABSR_SR_MASK                     (0x01U << ENET_DMABSR_SR_SHIFT)
#define ENET_DMABSR_SR(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMABSR_SR_SHIFT)) & ENET_DMABSR_SR_MASK)

/*!
 * @brief ENET_DMASR Register Bit Definition
 */

#define ENET_DMASR_PTPS_SHIFT                   (29)
#define ENET_DMASR_PTPS_MASK                    (0x01U << ENET_DMASR_PTPS_SHIFT)
#define ENET_DMASR_PTPS(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_PTPS_SHIFT)) & ENET_DMASR_PTPS_MASK)

#define ENET_DMASR_PMTS_SHIFT                   (28)
#define ENET_DMASR_PMTS_MASK                    (0x01U << ENET_DMASR_PMTS_SHIFT)
#define ENET_DMASR_PMTS(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_PMTS_SHIFT)) & ENET_DMASR_PMTS_MASK)

#define ENET_DMASR_MMCS_SHIFT                   (27)
#define ENET_DMASR_MMCS_MASK                    (0x01U << ENET_DMASR_MMCS_SHIFT)
#define ENET_DMASR_MMCS(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_MMCS_SHIFT)) & ENET_DMASR_MMCS_MASK)

#define ENET_DMASR_EBUS_SHIFT                   (23)
#define ENET_DMASR_EBUS_MASK                    (0x7U << ENET_DMASR_EBUS_SHIFT)
#define ENET_DMASR_EBUS(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_EBUS_SHIFT)) & ENET_DMASR_EBUS_MASK)

#define ENET_DMASR_TPS_SHIFT                    (20)
#define ENET_DMASR_TPS_MASK                     (0x7U << ENET_DMASR_TPS_SHIFT)
#define ENET_DMASR_TPS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_TPS_SHIFT)) & ENET_DMASR_TPS_MASK)

#define ENET_DMASR_RPS_SHIFT                    (17)
#define ENET_DMASR_RPS_MASK                     (0x7U << ENET_DMASR_RPS_SHIFT)
#define ENET_DMASR_RPS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_RPS_SHIFT)) & ENET_DMASR_RPS_MASK)

#define ENET_DMASR_NIS_SHIFT                    (16)
#define ENET_DMASR_NIS_MASK                     (0x01U << ENET_DMASR_NIS_SHIFT)
#define ENET_DMASR_NIS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_NIS_SHIFT)) & ENET_DMASR_NIS_MASK)

#define ENET_DMASR_AIS_SHIFT                    (15)
#define ENET_DMASR_AIS_MASK                     (0x01U << ENET_DMASR_AIS_SHIFT)
#define ENET_DMASR_AIS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_AIS_SHIFT)) & ENET_DMASR_AIS_MASK)

#define ENET_DMASR_ERS_SHIFT                    (14)
#define ENET_DMASR_ERS_MASK                     (0x01U << ENET_DMASR_ERS_SHIFT)
#define ENET_DMASR_ERS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_ERS_SHIFT)) & ENET_DMASR_ERS_MASK)

#define ENET_DMASR_FBS_SHIFT                    (13)
#define ENET_DMASR_FBS_MASK                     (0x01U << ENET_DMASR_FBS_SHIFT)
#define ENET_DMASR_FBS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_FBS_SHIFT)) & ENET_DMASR_FBS_MASK)

#define ENET_DMASR_ETS_SHIFT                    (10)
#define ENET_DMASR_ETS_MASK                     (0x01U << ENET_DMASR_ETS_SHIFT)
#define ENET_DMASR_ETS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_ETS_SHIFT)) & ENET_DMASR_ETS_MASK)

#define ENET_DMASR_RWS_SHIFT                    (9)
#define ENET_DMASR_RWS_MASK                     (0x01U << ENET_DMASR_RWS_SHIFT)
#define ENET_DMASR_RWS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_RWS_SHIFT)) & ENET_DMASR_RWS_MASK)

#define ENET_DMASR_RSS_SHIFT                    (8)
#define ENET_DMASR_RSS_MASK                     (0x01U << ENET_DMASR_RSS_SHIFT)
#define ENET_DMASR_RSS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_RSS_SHIFT)) & ENET_DMASR_RSS_MASK)

#define ENET_DMASR_RUS_SHIFT                    (7)
#define ENET_DMASR_RUS_MASK                     (0x01U << ENET_DMASR_RUS_SHIFT)
#define ENET_DMASR_RUS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_RUS_SHIFT)) & ENET_DMASR_RUS_MASK)

#define ENET_DMASR_RCS_SHIFT                    (6)
#define ENET_DMASR_RCS_MASK                     (0x01U << ENET_DMASR_RCS_SHIFT)
#define ENET_DMASR_RCS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_RCS_SHIFT)) & ENET_DMASR_RCS_MASK)

#define ENET_DMASR_UNS_SHIFT                    (5)
#define ENET_DMASR_UNS_MASK                     (0x01U << ENET_DMASR_UNS_SHIFT)
#define ENET_DMASR_UNS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_UNS_SHIFT)) & ENET_DMASR_UNS_MASK)

#define ENET_DMASR_OVS_SHIFT                    (4)
#define ENET_DMASR_OVS_MASK                     (0x01U << ENET_DMASR_OVS_SHIFT)
#define ENET_DMASR_OVS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_OVS_SHIFT)) & ENET_DMASR_OVS_MASK)

#define ENET_DMASR_TJS_SHIFT                    (3)
#define ENET_DMASR_TJS_MASK                     (0x01U << ENET_DMASR_TJS_SHIFT)
#define ENET_DMASR_TJS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_TJS_SHIFT)) & ENET_DMASR_TJS_MASK)

#define ENET_DMASR_TUS_SHIFT                    (2)
#define ENET_DMASR_TUS_MASK                     (0x01U << ENET_DMASR_TUS_SHIFT)
#define ENET_DMASR_TUS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_TUS_SHIFT)) & ENET_DMASR_TUS_MASK)

#define ENET_DMASR_TSS_SHIFT                    (1)
#define ENET_DMASR_TSS_MASK                     (0x01U << ENET_DMASR_TSS_SHIFT)
#define ENET_DMASR_TSS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_TSS_SHIFT)) & ENET_DMASR_TSS_MASK)

#define ENET_DMASR_TCS_SHIFT                    (0)
#define ENET_DMASR_TCS_MASK                     (0x01U << ENET_DMASR_TCS_SHIFT)
#define ENET_DMASR_TCS(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMASR_TCS_SHIFT)) & ENET_DMASR_TCS_MASK)

/*!
 * @brief ENET_DMAIR Register Bit Definition
 */

#define ENET_DMAIR_NIE_SHIFT                    (16)
#define ENET_DMAIR_NIE_MASK                     (0x01U << ENET_DMAIR_NIE_SHIFT)
#define ENET_DMAIR_NIE(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMAIR_NIE_SHIFT)) & ENET_DMAIR_NIE_MASK)

#define ENET_DMAIR_AIE_SHIFT                    (15)
#define ENET_DMAIR_AIE_MASK                     (0x01U << ENET_DMAIR_AIE_SHIFT)
#define ENET_DMAIR_AIE(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMAIR_AIE_SHIFT)) & ENET_DMAIR_AIE_MASK)

#define ENET_DMAIR_ERE_SHIFT                    (14)
#define ENET_DMAIR_ERE_MASK                     (0x01U << ENET_DMAIR_ERE_SHIFT)
#define ENET_DMAIR_ERE(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMAIR_ERE_SHIFT)) & ENET_DMAIR_ERE_MASK)

#define ENET_DMAIR_FBE_SHIFT                    (13)
#define ENET_DMAIR_FBE_MASK                     (0x01U << ENET_DMAIR_FBE_SHIFT)
#define ENET_DMAIR_FBE(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMAIR_FBE_SHIFT)) & ENET_DMAIR_FBE_MASK)

#define ENET_DMAIR_ETE_SHIFT                    (10)
#define ENET_DMAIR_ETE_MASK                     (0x01U << ENET_DMAIR_ETE_SHIFT)
#define ENET_DMAIR_ETE(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMAIR_ETE_SHIFT)) & ENET_DMAIR_ETE_MASK)

#define ENET_DMAIR_RWE_SHIFT                    (9)
#define ENET_DMAIR_RWE_MASK                     (0x01U << ENET_DMAIR_RWE_SHIFT)
#define ENET_DMAIR_RWE(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMAIR_RWE_SHIFT)) & ENET_DMAIR_RWE_MASK)

#define ENET_DMAIR_RSE_SHIFT                    (8)
#define ENET_DMAIR_RSE_MASK                     (0x01U << ENET_DMAIR_RSE_SHIFT)
#define ENET_DMAIR_RSE(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMAIR_RSE_SHIFT)) & ENET_DMAIR_RSE_MASK)

#define ENET_DMAIR_RUE_SHIFT                    (7)
#define ENET_DMAIR_RUE_MASK                     (0x01U << ENET_DMAIR_RUE_SHIFT)
#define ENET_DMAIR_RUE(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMAIR_RUE_SHIFT)) & ENET_DMAIR_RUE_MASK)

#define ENET_DMAIR_RCE_SHIFT                    (6)
#define ENET_DMAIR_RCE_MASK                     (0x01U << ENET_DMAIR_RCE_SHIFT)
#define ENET_DMAIR_RCE(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMAIR_RCE_SHIFT)) & ENET_DMAIR_RCE_MASK)

#define ENET_DMAIR_UNE_SHIFT                    (5)
#define ENET_DMAIR_UNE_MASK                     (0x01U << ENET_DMAIR_UNE_SHIFT)
#define ENET_DMAIR_UNE(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMAIR_UNE_SHIFT)) & ENET_DMAIR_UNE_MASK)

#define ENET_DMAIR_OVE_SHIFT                    (4)
#define ENET_DMAIR_OVE_MASK                     (0x01U << ENET_DMAIR_OVE_SHIFT)
#define ENET_DMAIR_OVE(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMAIR_OVE_SHIFT)) & ENET_DMAIR_OVE_MASK)

#define ENET_DMAIR_TJE_SHIFT                    (3)
#define ENET_DMAIR_TJE_MASK                     (0x01U << ENET_DMAIR_TJE_SHIFT)
#define ENET_DMAIR_TJE(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMAIR_TJE_SHIFT)) & ENET_DMAIR_TJE_MASK)

#define ENET_DMAIR_TUE_SHIFT                    (2)
#define ENET_DMAIR_TUE_MASK                     (0x01U << ENET_DMAIR_TUE_SHIFT)
#define ENET_DMAIR_TUE(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMAIR_TUE_SHIFT)) & ENET_DMAIR_TUE_MASK)

#define ENET_DMAIR_TSE_SHIFT                    (1)
#define ENET_DMAIR_TSE_MASK                     (0x01U << ENET_DMAIR_TSE_SHIFT)
#define ENET_DMAIR_TSE(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMAIR_TSE_SHIFT)) & ENET_DMAIR_TSE_MASK)

#define ENET_DMAIR_TCE_SHIFT                    (0)
#define ENET_DMAIR_TCE_MASK                     (0x01U << ENET_DMAIR_TCE_SHIFT)
#define ENET_DMAIR_TCE(x)                       (((uint32_t)(((uint32_t)(x)) << ENET_DMAIR_TCE_SHIFT)) & ENET_DMAIR_TCE_MASK)

/*!
 * @brief ENET_DMAFLCR Register Bit Definition
 */

#define ENET_DMAFLCR_OVFF_SHIFT                 (28)
#define ENET_DMAFLCR_OVFF_MASK                  (0x01U << ENET_DMAFLCR_OVFF_SHIFT)
#define ENET_DMAFLCR_OVFF(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_DMAFLCR_OVFF_SHIFT)) & ENET_DMAFLCR_OVFF_MASK)

#define ENET_DMAFLCR_OVFC_SHIFT                 (17)
#define ENET_DMAFLCR_OVFC_MASK                  (0x7FFU << ENET_DMAFLCR_OVFC_SHIFT)
#define ENET_DMAFLCR_OVFC(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_DMAFLCR_OVFC_SHIFT)) & ENET_DMAFLCR_OVFC_MASK)

#define ENET_DMAFLCR_BUAF_SHIFT                 (16)
#define ENET_DMAFLCR_BUAF_MASK                  (0x01U << ENET_DMAFLCR_BUAF_SHIFT)
#define ENET_DMAFLCR_BUAF(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_DMAFLCR_BUAF_SHIFT)) & ENET_DMAFLCR_BUAF_MASK)

#define ENET_DMAFLCR_BUAC_SHIFT                 (0)
#define ENET_DMAFLCR_BUAC_MASK                  (0xFFFFU << ENET_DMAFLCR_BUAC_SHIFT)
#define ENET_DMAFLCR_BUAC(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_DMAFLCR_BUAC_SHIFT)) & ENET_DMAFLCR_BUAC_MASK)

/*!
 * @brief ENET_DMAWDTR Register Bit Definition
 */

#define ENET_DMAWDTR_RIWT_SHIFT                 (0)
#define ENET_DMAWDTR_RIWT_MASK                  (0xFFFFFFFFU << ENET_DMAWDTR_RIWT_SHIFT)
#define ENET_DMAWDTR_RIWT(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_DMAWDTR_RIWT_SHIFT)) & ENET_DMAWDTR_RIWT_MASK)

/*!
 * @brief ENET_DMARXPDR Register Bit Definition
 */

#define ENET_DMARXPDR_RXPD_SHIFT                (0)
#define ENET_DMARXPDR_RXPD_MASK                 (0xFFFFFFFFU << ENET_DMARXPDR_RXPD_SHIFT)
#define ENET_DMARXPDR_RXPD(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_DMARXPDR_RXPD_SHIFT)) & ENET_DMARXPDR_RXPD_MASK)

/*!
 * @brief ENET_DMATXPDR Register Bit Definition
 */

#define ENET_DMATXPDR_TXPD_SHIFT                (0)
#define ENET_DMATXPDR_TXPD_MASK                 (0xFFFFFFFFU << ENET_DMATXPDR_TXPD_SHIFT)
#define ENET_DMATXPDR_TXPD(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_DMATXPDR_TXPD_SHIFT)) & ENET_DMATXPDR_TXPD_MASK)

/*!
 * @brief ENET_DMARXDSAR Register Bit Definition
 */

#define ENET_DMARXDSAR_RXDSA_SHIFT              (0)
#define ENET_DMARXDSAR_RXDSA_MASK               (0xFFFFFFFFU << ENET_DMARXDSAR_RXDSA_SHIFT)
#define ENET_DMARXDSAR_RXDSA(x)                 (((uint32_t)(((uint32_t)(x)) << ENET_DMARXDSAR_RXDSA_SHIFT)) & ENET_DMARXDSAR_RXDSA_MASK)

/*!
 * @brief ENET_DMATXDSAR Register Bit Definition
 */

#define ENET_DMATXDSAR_TXDSA_SHIFT              (0)
#define ENET_DMATXDSAR_TXDSA_MASK               (0xFFFFFFFFU << ENET_DMATXDSAR_TXDSA_SHIFT)
#define ENET_DMATXDSAR_TXDSA(x)                 (((uint32_t)(((uint32_t)(x)) << ENET_DMATXDSAR_TXDSA_SHIFT)) & ENET_DMATXDSAR_TXDSA_MASK)

/*!
 * @brief ENET_DMACURRXDSAR Register Bit Definition
 */

#define ENET_DMACURRXDSAR_CURRXDSA_SHIFT        (0)
#define ENET_DMACURRXDSAR_CURRXDSA_MASK         (0xFFFFFFFFU << ENET_DMACURRXDSAR_CURRXDSA_SHIFT)
#define ENET_DMACURRXDSAR_CURRXDSA(x)           (((uint32_t)(((uint32_t)(x)) << ENET_DMACURRXDSAR_CURRXDSA_SHIFT)) & ENET_DMACURRXDSAR_CURRXDSA_MASK)

/*!
 * @brief ENET_DMACURTXDSAR Register Bit Definition
 */

#define ENET_DMACURTXDSAR_CURTXDSA_SHIFT        (0)
#define ENET_DMACURTXDSAR_CURTXDSA_MASK         (0xFFFFFFFFU << ENET_DMACURTXDSAR_CURTXDSA_SHIFT)
#define ENET_DMACURTXDSAR_CURTXDSA(x)           (((uint32_t)(((uint32_t)(x)) << ENET_DMACURTXDSAR_CURTXDSA_SHIFT)) & ENET_DMACURTXDSAR_CURTXDSA_MASK)

/*!
 * @brief ENET_DMACURRXBFAR Register Bit Definition
 */

#define ENET_DMACURRXBFAR_CURRXBFA_SHIFT        (0)
#define ENET_DMACURRXBFAR_CURRXBFA_MASK         (0xFFFFFFFFU << ENET_DMACURRXBFAR_CURRXBFA_SHIFT)
#define ENET_DMACURRXBFAR_CURRXBFA(x)           (((uint32_t)(((uint32_t)(x)) << ENET_DMACURRXBFAR_CURRXBFA_SHIFT)) & ENET_DMACURRXBFAR_CURRXBFA_MASK)

/*!
 * @brief ENET_DMACURTXBFAR Register Bit Definition
 */

#define ENET_DMACURTXBFAR_CURTXBFA_SHIFT        (0)
#define ENET_DMACURTXBFAR_CURTXBFA_MASK         (0xFFFFFFFFU << ENET_DMACURTXBFAR_CURTXBFA_SHIFT)
#define ENET_DMACURTXBFAR_CURTXBFA(x)           (((uint32_t)(((uint32_t)(x)) << ENET_DMACURTXBFAR_CURTXBFA_SHIFT)) & ENET_DMACURTXBFAR_CURTXBFA_MASK)

/*!
 * @brief ENET_PTPCR Register Bit Definition
 */

#define ENET_PTPCR_PTPDAF_SHIFT                 (18)
#define ENET_PTPCR_PTPDAF_MASK                  (0x01U << ENET_PTPCR_PTPDAF_SHIFT)
#define ENET_PTPCR_PTPDAF(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_PTPCR_PTPDAF_SHIFT)) & ENET_PTPCR_PTPDAF_MASK)

#define ENET_PTPCR_PTPTYPS_SHIFT                (14)
#define ENET_PTPCR_PTPTYPS_MASK                 (0xFU << ENET_PTPCR_PTPTYPS_SHIFT)
#define ENET_PTPCR_PTPTYPS(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_PTPCR_PTPTYPS_SHIFT)) & ENET_PTPCR_PTPTYPS_MASK)

#define ENET_PTPCR_PTPIPV4_SHIFT                (13)
#define ENET_PTPCR_PTPIPV4_MASK                 (0x01U << ENET_PTPCR_PTPIPV4_SHIFT)
#define ENET_PTPCR_PTPIPV4(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_PTPCR_PTPIPV4_SHIFT)) & ENET_PTPCR_PTPIPV4_MASK)

#define ENET_PTPCR_PTPIPV6_SHIFT                (12)
#define ENET_PTPCR_PTPIPV6_MASK                 (0x01U << ENET_PTPCR_PTPIPV6_SHIFT)
#define ENET_PTPCR_PTPIPV6(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_PTPCR_PTPIPV6_SHIFT)) & ENET_PTPCR_PTPIPV6_MASK)

#define ENET_PTPCR_PTPETH_SHIFT                 (11)
#define ENET_PTPCR_PTPETH_MASK                  (0x01U << ENET_PTPCR_PTPETH_SHIFT)
#define ENET_PTPCR_PTPETH(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_PTPCR_PTPETH_SHIFT)) & ENET_PTPCR_PTPETH_MASK)

#define ENET_PTPCR_PTPVER_SHIFT                 (10)
#define ENET_PTPCR_PTPVER_MASK                  (0x01U << ENET_PTPCR_PTPVER_SHIFT)
#define ENET_PTPCR_PTPVER(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_PTPCR_PTPVER_SHIFT)) & ENET_PTPCR_PTPVER_MASK)

#define ENET_PTPCR_PTPNSS_SHIFT                 (9)
#define ENET_PTPCR_PTPNSS_MASK                  (0x01U << ENET_PTPCR_PTPNSS_SHIFT)
#define ENET_PTPCR_PTPNSS(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_PTPCR_PTPNSS_SHIFT)) & ENET_PTPCR_PTPNSS_MASK)

#define ENET_PTPCR_PTPEALL_SHIFT                (8)
#define ENET_PTPCR_PTPEALL_MASK                 (0x01U << ENET_PTPCR_PTPEALL_SHIFT)
#define ENET_PTPCR_PTPEALL(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_PTPCR_PTPEALL_SHIFT)) & ENET_PTPCR_PTPEALL_MASK)

#define ENET_PTPCR_PTPADUP_SHIFT                (5)
#define ENET_PTPCR_PTPADUP_MASK                 (0x01U << ENET_PTPCR_PTPADUP_SHIFT)
#define ENET_PTPCR_PTPADUP(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_PTPCR_PTPADUP_SHIFT)) & ENET_PTPCR_PTPADUP_MASK)

#define ENET_PTPCR_PTPINT_SHIFT                 (4)
#define ENET_PTPCR_PTPINT_MASK                  (0x01U << ENET_PTPCR_PTPINT_SHIFT)
#define ENET_PTPCR_PTPINT(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_PTPCR_PTPINT_SHIFT)) & ENET_PTPCR_PTPINT_MASK)

#define ENET_PTPCR_PTPUP_SHIFT                  (3)
#define ENET_PTPCR_PTPUP_MASK                   (0x01U << ENET_PTPCR_PTPUP_SHIFT)
#define ENET_PTPCR_PTPUP(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_PTPCR_PTPUP_SHIFT)) & ENET_PTPCR_PTPUP_MASK)

#define ENET_PTPCR_PTPINI_SHIFT                 (2)
#define ENET_PTPCR_PTPINI_MASK                  (0x01U << ENET_PTPCR_PTPINI_SHIFT)
#define ENET_PTPCR_PTPINI(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_PTPCR_PTPINI_SHIFT)) & ENET_PTPCR_PTPINI_MASK)

#define ENET_PTPCR_PTPCS_SHIFT                  (1)
#define ENET_PTPCR_PTPCS_MASK                   (0x01U << ENET_PTPCR_PTPCS_SHIFT)
#define ENET_PTPCR_PTPCS(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_PTPCR_PTPCS_SHIFT)) & ENET_PTPCR_PTPCS_MASK)

#define ENET_PTPCR_PTPEN_SHIFT                  (0)
#define ENET_PTPCR_PTPEN_MASK                   (0x01U << ENET_PTPCR_PTPEN_SHIFT)
#define ENET_PTPCR_PTPEN(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_PTPCR_PTPEN_SHIFT)) & ENET_PTPCR_PTPEN_MASK)

/*!
 * @brief ENET_PTPSR Register Bit Definition
 */

#define ENET_PTPSR_TSER1_SHIFT                  (5)
#define ENET_PTPSR_TSER1_MASK                   (0x01U << ENET_PTPSR_TSER1_SHIFT)
#define ENET_PTPSR_TSER1(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_PTPSR_TSER1_SHIFT)) & ENET_PTPSR_TSER1_MASK)

#define ENET_PTPSR_TSTA1_SHIFT                  (4)
#define ENET_PTPSR_TSTA1_MASK                   (0x01U << ENET_PTPSR_TSTA1_SHIFT)
#define ENET_PTPSR_TSTA1(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_PTPSR_TSTA1_SHIFT)) & ENET_PTPSR_TSTA1_MASK)

#define ENET_PTPSR_TSER0_SHIFT                  (3)
#define ENET_PTPSR_TSER0_MASK                   (0x01U << ENET_PTPSR_TSER0_SHIFT)
#define ENET_PTPSR_TSER0(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_PTPSR_TSER0_SHIFT)) & ENET_PTPSR_TSER0_MASK)

#define ENET_PTPSR_TSTA0_SHIFT                  (1)
#define ENET_PTPSR_TSTA0_MASK                   (0x01U << ENET_PTPSR_TSTA0_SHIFT)
#define ENET_PTPSR_TSTA0(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_PTPSR_TSTA0_SHIFT)) & ENET_PTPSR_TSTA0_MASK)

#define ENET_PTPSR_TSOVF_SHIFT                  (0)
#define ENET_PTPSR_TSOVF_MASK                   (0x01U << ENET_PTPSR_TSOVF_SHIFT)
#define ENET_PTPSR_TSOVF(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_PTPSR_TSOVF_SHIFT)) & ENET_PTPSR_TSOVF_MASK)

/*!
 * @brief ENET_PTPBSAR Register Bit Definition
 */

#define ENET_PTPBSAR_BSADD_SHIFT                (0)
#define ENET_PTPBSAR_BSADD_MASK                 (0xFFFFFFFFU << ENET_PTPBSAR_BSADD_SHIFT)
#define ENET_PTPBSAR_BSADD(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_PTPBSAR_BSADD_SHIFT)) & ENET_PTPBSAR_BSADD_MASK)

/*!
 * @brief ENET_PTPNSAR Register Bit Definition
 */

#define ENET_PTPNSAR_NSADD_SHIFT                (0)
#define ENET_PTPNSAR_NSADD_MASK                 (0xFFU << ENET_PTPNSAR_NSADD_SHIFT)
#define ENET_PTPNSAR_NSADD(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_PTPNSAR_NSADD_SHIFT)) & ENET_PTPNSAR_NSADD_MASK)

/*!
 * @brief ENET_PTPSBSR Register Bit Definition
 */

#define ENET_PTPSBSR_SBSEC_SHIFT                (0)
#define ENET_PTPSBSR_SBSEC_MASK                 (0xFFFFFFFFU << ENET_PTPSBSR_SBSEC_SHIFT)
#define ENET_PTPSBSR_SBSEC(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_PTPSBSR_SBSEC_SHIFT)) & ENET_PTPSBSR_SBSEC_MASK)

/*!
 * @brief ENET_PTPSNSR Register Bit Definition
 */

#define ENET_PTPSNSR_SNSEC_SHIFT                (0)
#define ENET_PTPSNSR_SNSEC_MASK                 (0x7FFFFFFFU << ENET_PTPSNSR_SNSEC_SHIFT)
#define ENET_PTPSNSR_SNSEC(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_PTPSNSR_SNSEC_SHIFT)) & ENET_PTPSNSR_SNSEC_MASK)

/*!
 * @brief ENET_PTPUBSR Register Bit Definition
 */

#define ENET_PTPUBSR_UBSEC_SHIFT                (0)
#define ENET_PTPUBSR_UBSEC_MASK                 (0xFFFFFFFFU << ENET_PTPUBSR_UBSEC_SHIFT)
#define ENET_PTPUBSR_UBSEC(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_PTPUBSR_UBSEC_SHIFT)) & ENET_PTPUBSR_UBSEC_MASK)

/*!
 * @brief ENET_PTPUNSR Register Bit Definition
 */

#define ENET_PTPUNSR_UPNSEC_SHIFT               (31)
#define ENET_PTPUNSR_UPNSEC_MASK                (0x01U << ENET_PTPUNSR_UPNSEC_SHIFT)
#define ENET_PTPUNSR_UPNSEC(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_PTPUNSR_UPNSEC_SHIFT)) & ENET_PTPUNSR_UPNSEC_MASK)

#define ENET_PTPUNSR_UNSEC_SHIFT                (0)
#define ENET_PTPUNSR_UNSEC_MASK                 (0x7FFFFFFFU << ENET_PTPUNSR_UNSEC_SHIFT)
#define ENET_PTPUNSR_UNSEC(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_PTPUNSR_UNSEC_SHIFT)) & ENET_PTPUNSR_UNSEC_MASK)

/*!
 * @brief ENET_PTPTBSR Register Bit Definition
 */

#define ENET_PTPTBSR_TBSEC_SHIFT                (0)
#define ENET_PTPTBSR_TBSEC_MASK                 (0xFFFFFFFFU << ENET_PTPTBSR_TBSEC_SHIFT)
#define ENET_PTPTBSR_TBSEC(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_PTPTBSR_TBSEC_SHIFT)) & ENET_PTPTBSR_TBSEC_MASK)

/*!
 * @brief ENET_PTPTNSR Register Bit Definition
 */

#define ENET_PTPTNSR_TNSEC_SHIFT                (0)
#define ENET_PTPTNSR_TNSEC_MASK                 (0x7FFFFFFFU << ENET_PTPTNSR_TNSEC_SHIFT)
#define ENET_PTPTNSR_TNSEC(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_PTPTNSR_TNSEC_SHIFT)) & ENET_PTPTNSR_TNSEC_MASK)

/*!
 * @brief ENET_PTPPPSR Register Bit Definition
 */

#define ENET_PTPPPSR_TTSEL1_SHIFT               (13)
#define ENET_PTPPPSR_TTSEL1_MASK                (0x3U << ENET_PTPPPSR_TTSEL1_SHIFT)
#define ENET_PTPPPSR_TTSEL1(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_PTPPPSR_TTSEL1_SHIFT)) & ENET_PTPPPSR_TTSEL1_MASK)

#define ENET_PTPPPSR_PPSOUT1_SHIFT              (8)
#define ENET_PTPPPSR_PPSOUT1_MASK               (0x7U << ENET_PTPPPSR_PPSOUT1_SHIFT)
#define ENET_PTPPPSR_PPSOUT1(x)                 (((uint32_t)(((uint32_t)(x)) << ENET_PTPPPSR_PPSOUT1_SHIFT)) & ENET_PTPPPSR_PPSOUT1_MASK)

#define ENET_PTPPPSR_TTSEL0_SHIFT               (5)
#define ENET_PTPPPSR_TTSEL0_MASK                (0x3U << ENET_PTPPPSR_TTSEL0_SHIFT)
#define ENET_PTPPPSR_TTSEL0(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_PTPPPSR_TTSEL0_SHIFT)) & ENET_PTPPPSR_TTSEL0_MASK)

#define ENET_PTPPPSR_PPSOMD_SHIFT               (4)
#define ENET_PTPPPSR_PPSOMD_MASK                (0x01U << ENET_PTPPPSR_PPSOMD_SHIFT)
#define ENET_PTPPPSR_PPSOMD(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_PTPPPSR_PPSOMD_SHIFT)) & ENET_PTPPPSR_PPSOMD_MASK)

#define ENET_PTPPPSR_PPSOUT0_SHIFT              (0)
#define ENET_PTPPPSR_PPSOUT0_MASK               (0xFU << ENET_PTPPPSR_PPSOUT0_SHIFT)
#define ENET_PTPPPSR_PPSOUT0(x)                 (((uint32_t)(((uint32_t)(x)) << ENET_PTPPPSR_PPSOUT0_SHIFT)) & ENET_PTPPPSR_PPSOUT0_MASK)

/*!
 * @brief ENET_PMTCR Register Bit Definition
 */

#define ENET_PMTCR_RTWKFR_SHIFT                 (31)
#define ENET_PMTCR_RTWKFR_MASK                  (0x01U << ENET_PMTCR_RTWKFR_SHIFT)
#define ENET_PMTCR_RTWKFR(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_PMTCR_RTWKFR_SHIFT)) & ENET_PMTCR_RTWKFR_MASK)

#define ENET_PMTCR_RTWKPT_SHIFT                 (24)
#define ENET_PMTCR_RTWKPT_MASK                  (0x7U << ENET_PMTCR_RTWKPT_SHIFT)
#define ENET_PMTCR_RTWKPT(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_PMTCR_RTWKPT_SHIFT)) & ENET_PMTCR_RTWKPT_MASK)

#define ENET_PMTCR_RTWKTR_SHIFT                 (10)
#define ENET_PMTCR_RTWKTR_MASK                  (0x01U << ENET_PMTCR_RTWKTR_SHIFT)
#define ENET_PMTCR_RTWKTR(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_PMTCR_RTWKTR_SHIFT)) & ENET_PMTCR_RTWKTR_MASK)

#define ENET_PMTCR_GLUB_SHIFT                   (9)
#define ENET_PMTCR_GLUB_MASK                    (0x01U << ENET_PMTCR_GLUB_SHIFT)
#define ENET_PMTCR_GLUB(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_PMTCR_GLUB_SHIFT)) & ENET_PMTCR_GLUB_MASK)

#define ENET_PMTCR_WKFR_SHIFT                   (6)
#define ENET_PMTCR_WKFR_MASK                    (0x01U << ENET_PMTCR_WKFR_SHIFT)
#define ENET_PMTCR_WKFR(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_PMTCR_WKFR_SHIFT)) & ENET_PMTCR_WKFR_MASK)

#define ENET_PMTCR_MPFR_SHIFT                   (5)
#define ENET_PMTCR_MPFR_MASK                    (0x01U << ENET_PMTCR_MPFR_SHIFT)
#define ENET_PMTCR_MPFR(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_PMTCR_MPFR_SHIFT)) & ENET_PMTCR_MPFR_MASK)

#define ENET_PMTCR_WKEN_SHIFT                   (2)
#define ENET_PMTCR_WKEN_MASK                    (0x01U << ENET_PMTCR_WKEN_SHIFT)
#define ENET_PMTCR_WKEN(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_PMTCR_WKEN_SHIFT)) & ENET_PMTCR_WKEN_MASK)

#define ENET_PMTCR_MPEN_SHIFT                   (1)
#define ENET_PMTCR_MPEN_MASK                    (0x01U << ENET_PMTCR_MPEN_SHIFT)
#define ENET_PMTCR_MPEN(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_PMTCR_MPEN_SHIFT)) & ENET_PMTCR_MPEN_MASK)

#define ENET_PMTCR_PWDN_SHIFT                   (0)
#define ENET_PMTCR_PWDN_MASK                    (0x01U << ENET_PMTCR_PWDN_SHIFT)
#define ENET_PMTCR_PWDN(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_PMTCR_PWDN_SHIFT)) & ENET_PMTCR_PWDN_MASK)

/*!
 * @brief ENET_PMTRWFR Register Bit Definition
 */

#define ENET_PMTRWFR_RTWKFLT_SHIFT              (0)
#define ENET_PMTRWFR_RTWKFLT_MASK               (0xFFFFFFFFU << ENET_PMTRWFR_RTWKFLT_SHIFT)
#define ENET_PMTRWFR_RTWKFLT(x)                 (((uint32_t)(((uint32_t)(x)) << ENET_PMTRWFR_RTWKFLT_SHIFT)) & ENET_PMTRWFR_RTWKFLT_MASK)

/*!
 * @brief ENET_MMCCR Register Bit Definition
 */

#define ENET_MMCCR_CPSEL_SHIFT                  (5)
#define ENET_MMCCR_CPSEL_MASK                   (0x01U << ENET_MMCCR_CPSEL_SHIFT)
#define ENET_MMCCR_CPSEL(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_MMCCR_CPSEL_SHIFT)) & ENET_MMCCR_CPSEL_MASK)

#define ENET_MMCCR_CPSET_SHIFT                  (4)
#define ENET_MMCCR_CPSET_MASK                   (0x01U << ENET_MMCCR_CPSET_SHIFT)
#define ENET_MMCCR_CPSET(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_MMCCR_CPSET_SHIFT)) & ENET_MMCCR_CPSET_MASK)

#define ENET_MMCCR_CFIX_SHIFT                   (3)
#define ENET_MMCCR_CFIX_MASK                    (0x01U << ENET_MMCCR_CFIX_SHIFT)
#define ENET_MMCCR_CFIX(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MMCCR_CFIX_SHIFT)) & ENET_MMCCR_CFIX_MASK)

#define ENET_MMCCR_CRRST_SHIFT                  (2)
#define ENET_MMCCR_CRRST_MASK                   (0x01U << ENET_MMCCR_CRRST_SHIFT)
#define ENET_MMCCR_CRRST(x)                     (((uint32_t)(((uint32_t)(x)) << ENET_MMCCR_CRRST_SHIFT)) & ENET_MMCCR_CRRST_MASK)

#define ENET_MMCCR_COSR_SHIFT                   (1)
#define ENET_MMCCR_COSR_MASK                    (0x01U << ENET_MMCCR_COSR_SHIFT)
#define ENET_MMCCR_COSR(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MMCCR_COSR_SHIFT)) & ENET_MMCCR_COSR_MASK)

#define ENET_MMCCR_CRST_SHIFT                   (0)
#define ENET_MMCCR_CRST_MASK                    (0x01U << ENET_MMCCR_CRST_SHIFT)
#define ENET_MMCCR_CRST(x)                      (((uint32_t)(((uint32_t)(x)) << ENET_MMCCR_CRST_SHIFT)) & ENET_MMCCR_CRST_MASK)

/*!
 * @brief ENET_MMCRSR Register Bit Definition
 */

#define ENET_MMCRSR_RUGIS_SHIFT                 (17)
#define ENET_MMCRSR_RUGIS_MASK                  (0x01U << ENET_MMCRSR_RUGIS_SHIFT)
#define ENET_MMCRSR_RUGIS(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_MMCRSR_RUGIS_SHIFT)) & ENET_MMCRSR_RUGIS_MASK)

#define ENET_MMCRSR_RALIEIS_SHIFT               (6)
#define ENET_MMCRSR_RALIEIS_MASK                (0x01U << ENET_MMCRSR_RALIEIS_SHIFT)
#define ENET_MMCRSR_RALIEIS(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_MMCRSR_RALIEIS_SHIFT)) & ENET_MMCRSR_RALIEIS_MASK)

#define ENET_MMCRSR_RCRCEIS_SHIFT               (5)
#define ENET_MMCRSR_RCRCEIS_MASK                (0x01U << ENET_MMCRSR_RCRCEIS_SHIFT)
#define ENET_MMCRSR_RCRCEIS(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_MMCRSR_RCRCEIS_SHIFT)) & ENET_MMCRSR_RCRCEIS_MASK)

/*!
 * @brief ENET_MMCTSR Register Bit Definition
 */

#define ENET_MMCTSR_TAGIS_SHIFT                 (21)
#define ENET_MMCTSR_TAGIS_MASK                  (0x01U << ENET_MMCTSR_TAGIS_SHIFT)
#define ENET_MMCTSR_TAGIS(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_MMCTSR_TAGIS_SHIFT)) & ENET_MMCTSR_TAGIS_MASK)

#define ENET_MMCTSR_TGMCIS_SHIFT                (15)
#define ENET_MMCTSR_TGMCIS_MASK                 (0x01U << ENET_MMCTSR_TGMCIS_SHIFT)
#define ENET_MMCTSR_TGMCIS(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_MMCTSR_TGMCIS_SHIFT)) & ENET_MMCTSR_TGMCIS_MASK)

#define ENET_MMCTSR_TGSCIS_SHIFT                (14)
#define ENET_MMCTSR_TGSCIS_MASK                 (0x01U << ENET_MMCTSR_TGSCIS_SHIFT)
#define ENET_MMCTSR_TGSCIS(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_MMCTSR_TGSCIS_SHIFT)) & ENET_MMCTSR_TGSCIS_MASK)

/*!
 * @brief ENET_MMCRIR Register Bit Definition
 */

#define ENET_MMCRIR_RUGIM_SHIFT                 (17)
#define ENET_MMCRIR_RUGIM_MASK                  (0x01U << ENET_MMCRIR_RUGIM_SHIFT)
#define ENET_MMCRIR_RUGIM(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_MMCRIR_RUGIM_SHIFT)) & ENET_MMCRIR_RUGIM_MASK)

#define ENET_MMCRIR_RALIEIM_SHIFT               (6)
#define ENET_MMCRIR_RALIEIM_MASK                (0x01U << ENET_MMCRIR_RALIEIM_SHIFT)
#define ENET_MMCRIR_RALIEIM(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_MMCRIR_RALIEIM_SHIFT)) & ENET_MMCRIR_RALIEIM_MASK)

#define ENET_MMCRIR_RCRCEIM_SHIFT               (5)
#define ENET_MMCRIR_RCRCEIM_MASK                (0x01U << ENET_MMCRIR_RCRCEIM_SHIFT)
#define ENET_MMCRIR_RCRCEIM(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_MMCRIR_RCRCEIM_SHIFT)) & ENET_MMCRIR_RCRCEIM_MASK)

/*!
 * @brief ENET_MMCTIR Register Bit Definition
 */

#define ENET_MMCTIR_TAGIM_SHIFT                 (21)
#define ENET_MMCTIR_TAGIM_MASK                  (0x01U << ENET_MMCTIR_TAGIM_SHIFT)
#define ENET_MMCTIR_TAGIM(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_MMCTIR_TAGIM_SHIFT)) & ENET_MMCTIR_TAGIM_MASK)

#define ENET_MMCTIR_TGMCIM_SHIFT                (15)
#define ENET_MMCTIR_TGMCIM_MASK                 (0x01U << ENET_MMCTIR_TGMCIM_SHIFT)
#define ENET_MMCTIR_TGMCIM(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_MMCTIR_TGMCIM_SHIFT)) & ENET_MMCTIR_TGMCIM_MASK)

#define ENET_MMCTIR_TGSCIM_SHIFT                (14)
#define ENET_MMCTIR_TGSCIM_MASK                 (0x01U << ENET_MMCTIR_TGSCIM_SHIFT)
#define ENET_MMCTIR_TGSCIM(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_MMCTIR_TGSCIM_SHIFT)) & ENET_MMCTIR_TGSCIM_MASK)

/*!
 * @brief ENET_MMCRUGR Register Bit Definition
 */

#define ENET_MMCRUGR_MMCRUG_SHIFT               (0)
#define ENET_MMCRUGR_MMCRUG_MASK                (0xFFFFFFFFU << ENET_MMCRUGR_MMCRUG_SHIFT)
#define ENET_MMCRUGR_MMCRUG(x)                  (((uint32_t)(((uint32_t)(x)) << ENET_MMCRUGR_MMCRUG_SHIFT)) & ENET_MMCRUGR_MMCRUG_MASK)

/*!
 * @brief ENET_MMCRALIER Register Bit Definition
 */

#define ENET_MMCRALIER_RALIE_SHIFT              (0)
#define ENET_MMCRALIER_RALIE_MASK               (0xFFFFFFFFU << ENET_MMCRALIER_RALIE_SHIFT)
#define ENET_MMCRALIER_RALIE(x)                 (((uint32_t)(((uint32_t)(x)) << ENET_MMCRALIER_RALIE_SHIFT)) & ENET_MMCRALIER_RALIE_MASK)

/*!
 * @brief ENET_MMCRCRCER Register Bit Definition
 */

#define ENET_MMCRCRCER_RCRCE_SHIFT              (0)
#define ENET_MMCRCRCER_RCRCE_MASK               (0xFFFFFFFFU << ENET_MMCRCRCER_RCRCE_SHIFT)
#define ENET_MMCRCRCER_RCRCE(x)                 (((uint32_t)(((uint32_t)(x)) << ENET_MMCRCRCER_RCRCE_SHIFT)) & ENET_MMCRCRCER_RCRCE_MASK)

/*!
 * @brief ENET_MMCTGR Register Bit Definition
 */

#define ENET_MMCTGR_MMCTG_SHIFT                 (0)
#define ENET_MMCTGR_MMCTG_MASK                  (0xFFFFFFFFU << ENET_MMCTGR_MMCTG_SHIFT)
#define ENET_MMCTGR_MMCTG(x)                    (((uint32_t)(((uint32_t)(x)) << ENET_MMCTGR_MMCTG_SHIFT)) & ENET_MMCTGR_MMCTG_MASK)

/*!
 * @brief ENET_MMCTGSCR Register Bit Definition
 */

#define ENET_MMCTGSCR_TGSC_SHIFT                (0)
#define ENET_MMCTGSCR_TGSC_MASK                 (0xFFFFFFFFU << ENET_MMCTGSCR_TGSC_SHIFT)
#define ENET_MMCTGSCR_TGSC(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_MMCTGSCR_TGSC_SHIFT)) & ENET_MMCTGSCR_TGSC_MASK)

/*!
 * @brief ENET_MMCTGMCR Register Bit Definition
 */

#define ENET_MMCTGMCR_TGMC_SHIFT                (0)
#define ENET_MMCTGMCR_TGMC_MASK                 (0xFFFFFFFFU << ENET_MMCTGMCR_TGMC_SHIFT)
#define ENET_MMCTGMCR_TGMC(x)                   (((uint32_t)(((uint32_t)(x)) << ENET_MMCTGMCR_TGMC_SHIFT)) & ENET_MMCTGMCR_TGMC_MASK)

/*!
 * @}
 */ /* end of group ENET_Register_Masks */
/******************************************************************************
 *ENET Instance
*******************************************************************************/

#define ENET                 ((ENET_Type*)ENET_BASE)

/*!
 * @}
 */ /* end of group ENET_Peripheral_Access_Layer */

/*!
 * @addtogroup EXTI_Peripheral_Access_Layer EXTI Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * EXTI Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t IMR;                                                            ///< interrupt mask register                      offset: 0x00
    __IO uint32_t EMR;                                                            ///< event mask register                          offset: 0x04
    __IO uint32_t RTSR;                                                           ///< Rising edge trigger select register          offset: 0x08
    __IO uint32_t FTSR;                                                           ///< Falling edge trigger select register         offset: 0x0C
    __IO uint32_t SWIER;                                                          ///< Software Interrupt Event Register            offset: 0x10
    __IO uint32_t PR;                                                             ///< pending register                             offset: 0x14
} EXTI_Type;

/*******************************************************************************
 * EXTI Type
 ******************************************************************************/

/*!
 * @addtogroup EXTI_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief EXTI_IMR Register Bit Definition
 */

#define EXTI_IMR_IMR_SHIFT                      (0)
#define EXTI_IMR_IMR_MASK                       (0xFFFFFFFFU << EXTI_IMR_IMR_SHIFT)
#define EXTI_IMR_IMR(x)                         (((uint32_t)(((uint32_t)(x)) << EXTI_IMR_IMR_SHIFT)) & EXTI_IMR_IMR_MASK)

/*!
 * @brief EXTI_EMR Register Bit Definition
 */

#define EXTI_EMR_IMR_SHIFT                      (0)
#define EXTI_EMR_IMR_MASK                       (0xFFFFFFFFU << EXTI_EMR_IMR_SHIFT)
#define EXTI_EMR_IMR(x)                         (((uint32_t)(((uint32_t)(x)) << EXTI_EMR_IMR_SHIFT)) & EXTI_EMR_IMR_MASK)

/*!
 * @brief EXTI_RTSR Register Bit Definition
 */

#define EXTI_RTSR_TR_SHIFT                      (0)
#define EXTI_RTSR_TR_MASK                       (0xFFFFFFFFU << EXTI_RTSR_TR_SHIFT)
#define EXTI_RTSR_TR(x)                         (((uint32_t)(((uint32_t)(x)) << EXTI_RTSR_TR_SHIFT)) & EXTI_RTSR_TR_MASK)

/*!
 * @brief EXTI_FTSR Register Bit Definition
 */

#define EXTI_FTSR_TR_SHIFT                      (0)
#define EXTI_FTSR_TR_MASK                       (0xFFFFFFFFU << EXTI_FTSR_TR_SHIFT)
#define EXTI_FTSR_TR(x)                         (((uint32_t)(((uint32_t)(x)) << EXTI_FTSR_TR_SHIFT)) & EXTI_FTSR_TR_MASK)

/*!
 * @brief EXTI_SWIER Register Bit Definition
 */

#define EXTI_SWIER_SWIER_SHIFT                  (0)
#define EXTI_SWIER_SWIER_MASK                   (0xFFFFFFFFU << EXTI_SWIER_SWIER_SHIFT)
#define EXTI_SWIER_SWIER(x)                     (((uint32_t)(((uint32_t)(x)) << EXTI_SWIER_SWIER_SHIFT)) & EXTI_SWIER_SWIER_MASK)

#define EXTI_SWIER_SWIER_SHIFT                  (0)
#define EXTI_SWIER_SWIER_MASK                   (0xFFFFFFFFU << EXTI_SWIER_SWIER_SHIFT)
#define EXTI_SWIER_SWIER(x)                     (((uint32_t)(((uint32_t)(x)) << EXTI_SWIER_SWIER_SHIFT)) & EXTI_SWIER_SWIER_MASK)

/*!
 * @brief EXTI_PR Register Bit Definition
 */

#define EXTI_PR_PR_SHIFT                        (0)
#define EXTI_PR_PR_MASK                         (0xFFFFFFFFU << EXTI_PR_PR_SHIFT)
#define EXTI_PR_PR(x)                           (((uint32_t)(((uint32_t)(x)) << EXTI_PR_PR_SHIFT)) & EXTI_PR_PR_MASK)

/*!
 * @}
 */ /* end of group EXTI_Register_Masks */
/******************************************************************************
 *EXTI Instance
*******************************************************************************/

#define EXTI                 ((EXTI_Type*)EXTI_BASE)

/*!
 * @}
 */ /* end of group EXTI_Peripheral_Access_Layer */




/*!
 * @addtogroup FLASH_Peripheral_Access_Layer FLASH Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * FLASH Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t ACR;                                                            ///< Flash Access Control Register                offset: 0x00
    __IO uint32_t KEYR;                                                           ///< FPEC key register                            offset: 0x04
    __IO uint32_t OPTKEYR;                                                        ///< Flash OPTKEY register                        offset: 0x08
    __IO uint32_t SR;                                                             ///< Flash Status Register                        offset: 0x0C
    __IO uint32_t CR;                                                             ///< Flash Control Register                       offset: 0x10
    __IO uint32_t AR;                                                             ///< Flash Address Register                       offset: 0x14
    __IO uint32_t Reserved24[1];                                                   ///< Reserved
    __IO uint32_t OBR;                                                            ///< option byte register                         offset: 0x1C
    __IO uint32_t WRPR[2];                                                        ///< write protect register 0                     offset: 0x20
} FLASH_Type;

/*******************************************************************************
 * FLASH Type
 ******************************************************************************/

/*!
 * @addtogroup FLASH_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief FLASH_ACR Register Bit Definition
 */

#define FLASH_ACR_PRFTBS_SHIFT                  (5)
#define FLASH_ACR_PRFTBS_MASK                   (0x01U << FLASH_ACR_PRFTBS_SHIFT)
#define FLASH_ACR_PRFTBS(x)                     (((uint32_t)(((uint32_t)(x)) << FLASH_ACR_PRFTBS_SHIFT)) & FLASH_ACR_PRFTBS_MASK)

#define FLASH_ACR_PRFTBE_SHIFT                  (4)
#define FLASH_ACR_PRFTBE_MASK                   (0x01U << FLASH_ACR_PRFTBE_SHIFT)
#define FLASH_ACR_PRFTBE(x)                     (((uint32_t)(((uint32_t)(x)) << FLASH_ACR_PRFTBE_SHIFT)) & FLASH_ACR_PRFTBE_MASK)

#define FLASH_ACR_LATENCY_SHIFT                 (0)
#define FLASH_ACR_LATENCY_MASK                  (0x7U << FLASH_ACR_LATENCY_SHIFT)
#define FLASH_ACR_LATENCY(x)                    (((uint32_t)(((uint32_t)(x)) << FLASH_ACR_LATENCY_SHIFT)) & FLASH_ACR_LATENCY_MASK)

/*!
 * @brief FLASH_KEYR Register Bit Definition
 */

#define FLASH_KEYR_FKEYR_SHIFT                  (0)
#define FLASH_KEYR_FKEYR_MASK                   (0xFFFFFFFFU << FLASH_KEYR_FKEYR_SHIFT)
#define FLASH_KEYR_FKEYR(x)                     (((uint32_t)(((uint32_t)(x)) << FLASH_KEYR_FKEYR_SHIFT)) & FLASH_KEYR_FKEYR_MASK)

/*!
 * @brief FLASH_OPTKEYR Register Bit Definition
 */

#define FLASH_OPTKEYR_OPTKEYR_SHIFT             (0)
#define FLASH_OPTKEYR_OPTKEYR_MASK              (0xFFFFFFFFU << FLASH_OPTKEYR_OPTKEYR_SHIFT)
#define FLASH_OPTKEYR_OPTKEYR(x)                (((uint32_t)(((uint32_t)(x)) << FLASH_OPTKEYR_OPTKEYR_SHIFT)) & FLASH_OPTKEYR_OPTKEYR_MASK)

/*!
 * @brief FLASH_SR Register Bit Definition
 */

#define FLASH_SR_EOP_SHIFT                      (5)
#define FLASH_SR_EOP_MASK                       (0x01U << FLASH_SR_EOP_SHIFT)
#define FLASH_SR_EOP(x)                         (((uint32_t)(((uint32_t)(x)) << FLASH_SR_EOP_SHIFT)) & FLASH_SR_EOP_MASK)

#define FLASH_SR_WRPRTERR_SHIFT                 (4)
#define FLASH_SR_WRPRTERR_MASK                  (0x01U << FLASH_SR_WRPRTERR_SHIFT)
#define FLASH_SR_WRPRTERR(x)                    (((uint32_t)(((uint32_t)(x)) << FLASH_SR_WRPRTERR_SHIFT)) & FLASH_SR_WRPRTERR_MASK)

#define FLASH_SR_PGERR_SHIFT                    (2)
#define FLASH_SR_PGERR_MASK                     (0x01U << FLASH_SR_PGERR_SHIFT)
#define FLASH_SR_PGERR(x)                       (((uint32_t)(((uint32_t)(x)) << FLASH_SR_PGERR_SHIFT)) & FLASH_SR_PGERR_MASK)

#define FLASH_SR_BSY_SHIFT                      (0)
#define FLASH_SR_BSY_MASK                       (0x01U << FLASH_SR_BSY_SHIFT)
#define FLASH_SR_BSY(x)                         (((uint32_t)(((uint32_t)(x)) << FLASH_SR_BSY_SHIFT)) & FLASH_SR_BSY_MASK)

/*!
 * @brief FLASH_CR Register Bit Definition
 */

#define FLASH_CR_OPTWRE_SHIFT                   (9)
#define FLASH_CR_OPTWRE_MASK                    (0x01U << FLASH_CR_OPTWRE_SHIFT)
#define FLASH_CR_OPTWRE(x)                      (((uint32_t)(((uint32_t)(x)) << FLASH_CR_OPTWRE_SHIFT)) & FLASH_CR_OPTWRE_MASK)

#define FLASH_CR_LOCK_SHIFT                     (7)
#define FLASH_CR_LOCK_MASK                      (0x01U << FLASH_CR_LOCK_SHIFT)
#define FLASH_CR_LOCK(x)                        (((uint32_t)(((uint32_t)(x)) << FLASH_CR_LOCK_SHIFT)) & FLASH_CR_LOCK_MASK)

#define FLASH_CR_STRT_SHIFT                     (6)
#define FLASH_CR_STRT_MASK                      (0x01U << FLASH_CR_STRT_SHIFT)
#define FLASH_CR_STRT(x)                        (((uint32_t)(((uint32_t)(x)) << FLASH_CR_STRT_SHIFT)) & FLASH_CR_STRT_MASK)

#define FLASH_CR_OPTER_SHIFT                    (5)
#define FLASH_CR_OPTER_MASK                     (0x01U << FLASH_CR_OPTER_SHIFT)
#define FLASH_CR_OPTER(x)                       (((uint32_t)(((uint32_t)(x)) << FLASH_CR_OPTER_SHIFT)) & FLASH_CR_OPTER_MASK)

#define FLASH_CR_OPTPG_SHIFT                    (4)
#define FLASH_CR_OPTPG_MASK                     (0x01U << FLASH_CR_OPTPG_SHIFT)
#define FLASH_CR_OPTPG(x)                       (((uint32_t)(((uint32_t)(x)) << FLASH_CR_OPTPG_SHIFT)) & FLASH_CR_OPTPG_MASK)

#define FLASH_CR_MER_SHIFT                      (2)
#define FLASH_CR_MER_MASK                       (0x01U << FLASH_CR_MER_SHIFT)
#define FLASH_CR_MER(x)                         (((uint32_t)(((uint32_t)(x)) << FLASH_CR_MER_SHIFT)) & FLASH_CR_MER_MASK)

#define FLASH_CR_PER_SHIFT                      (1)
#define FLASH_CR_PER_MASK                       (0x01U << FLASH_CR_PER_SHIFT)
#define FLASH_CR_PER(x)                         (((uint32_t)(((uint32_t)(x)) << FLASH_CR_PER_SHIFT)) & FLASH_CR_PER_MASK)

#define FLASH_CR_PG_SHIFT                       (0)
#define FLASH_CR_PG_MASK                        (0x01U << FLASH_CR_PG_SHIFT)
#define FLASH_CR_PG(x)                          (((uint32_t)(((uint32_t)(x)) << FLASH_CR_PG_SHIFT)) & FLASH_CR_PG_MASK)

/*!
 * @brief FLASH_AR Register Bit Definition
 */

#define FLASH_AR_FAR_SHIFT                      (0)
#define FLASH_AR_FAR_MASK                       (0xFFFFFFFFU << FLASH_AR_FAR_SHIFT)
#define FLASH_AR_FAR(x)                         (((uint32_t)(((uint32_t)(x)) << FLASH_AR_FAR_SHIFT)) & FLASH_AR_FAR_MASK)

/*!
 * @brief FLASH_OBR Register Bit Definition
 */

#define FLASH_OBR_DATA1_SHIFT                   (18)
#define FLASH_OBR_DATA1_MASK                    (0xFFU << FLASH_OBR_DATA1_SHIFT)
#define FLASH_OBR_DATA1(x)                      (((uint32_t)(((uint32_t)(x)) << FLASH_OBR_DATA1_SHIFT)) & FLASH_OBR_DATA1_MASK)

#define FLASH_OBR_DATA0_SHIFT                   (10)
#define FLASH_OBR_DATA0_MASK                    (0xFFU << FLASH_OBR_DATA0_SHIFT)
#define FLASH_OBR_DATA0(x)                      (((uint32_t)(((uint32_t)(x)) << FLASH_OBR_DATA0_SHIFT)) & FLASH_OBR_DATA0_MASK)

#define FLASH_OBR_NRSTSTDBY_SHIFT               (4)
#define FLASH_OBR_NRSTSTDBY_MASK                (0x01U << FLASH_OBR_NRSTSTDBY_SHIFT)
#define FLASH_OBR_NRSTSTDBY(x)                  (((uint32_t)(((uint32_t)(x)) << FLASH_OBR_NRSTSTDBY_SHIFT)) & FLASH_OBR_NRSTSTDBY_MASK)

#define FLASH_OBR_NRSTSTOP_SHIFT                (3)
#define FLASH_OBR_NRSTSTOP_MASK                 (0x01U << FLASH_OBR_NRSTSTOP_SHIFT)
#define FLASH_OBR_NRSTSTOP(x)                   (((uint32_t)(((uint32_t)(x)) << FLASH_OBR_NRSTSTOP_SHIFT)) & FLASH_OBR_NRSTSTOP_MASK)

#define FLASH_OBR_WDGSW_SHIFT                   (2)
#define FLASH_OBR_WDGSW_MASK                    (0x01U << FLASH_OBR_WDGSW_SHIFT)
#define FLASH_OBR_WDGSW(x)                      (((uint32_t)(((uint32_t)(x)) << FLASH_OBR_WDGSW_SHIFT)) & FLASH_OBR_WDGSW_MASK)

#define FLASH_OBR_RDPRT_SHIFT                   (1)
#define FLASH_OBR_RDPRT_MASK                    (0x01U << FLASH_OBR_RDPRT_SHIFT)
#define FLASH_OBR_RDPRT(x)                      (((uint32_t)(((uint32_t)(x)) << FLASH_OBR_RDPRT_SHIFT)) & FLASH_OBR_RDPRT_MASK)

#define FLASH_OBR_OPTERR_SHIFT                  (0)
#define FLASH_OBR_OPTERR_MASK                   (0x01U << FLASH_OBR_OPTERR_SHIFT)
#define FLASH_OBR_OPTERR(x)                     (((uint32_t)(((uint32_t)(x)) << FLASH_OBR_OPTERR_SHIFT)) & FLASH_OBR_OPTERR_MASK)

/*!
 * @brief FLASH_WRPR Register Bit Definition
 */

#define FLASH_WRPR_WRP_SHIFT                    (0)
#define FLASH_WRPR_WRP_MASK                     (0xFFFFFFFFU << FLASH_WRPR_WRP_SHIFT)
#define FLASH_WRPR_WRP(x)                       (((uint32_t)(((uint32_t)(x)) << FLASH_WRPR_WRP_SHIFT)) & FLASH_WRPR_WRP_MASK)


/*!
 * @}
 */ /* end of group FLASH_Register_Masks */
/******************************************************************************
 *FLASH Instance
*******************************************************************************/

#define FLASH                ((FLASH_Type*)FLASH_BASE)

/*!
 * @}
 */ /* end of group FLASH_Peripheral_Access_Layer */




/*!
 * @addtogroup FLEXCAN_Peripheral_Access_Layer FLEXCAN Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * FLEXCAN Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t MCR   ;                                                         ///< Module configuration register                offset: 0x0
    __IO uint32_t CTRL1  ;                                                        ///< Control 1 register                           offset: 0x4
    __IO uint32_t TIMER ;                                                         ///< Free running timer register                  offset: 0x8
    __IO uint32_t Reserved0[1];                                                   ///< Reserved
    __IO uint32_t RXMGMASK;                                                       ///< Rx mailbox global mask register              offset: 0x10
    __IO uint32_t RX14MASK;                                                       ///< Rx 14 mask register                          offset: 0x14
    __IO uint32_t RX15MASK;                                                       ///< Rx 15 mask register                          offset: 0x18
    __IO uint32_t ECR   ;                                                         ///< Error count register                         offset: 0x1C
    __IO uint32_t ESR1 ;                                                          ///< Error and status 1 register                  offset: 0x20
    __IO uint32_t Reserved1[1];                                                   ///< Reserved
    __IO uint32_t IMASK1   ;                                                      ///< Interrupt mask 1 register                    offset: 0x28
    __IO uint32_t Reserved2[1];                                                   ///< Reserved
    __IO uint32_t IFLAG1   ;                                                      ///< Interrupt flag 1 register                    offset: 0x30
    __IO uint32_t CTRL2;                                                          ///< Control 2 register                           offset: 0x34
    __IO uint32_t ESR2 ;                                                          ///< Error and status 2 register                  offset: 0x38
    __IO uint32_t Reserved3[2];                                                   ///< Reserved
    __IO uint32_t CRCR ;                                                          ///< CRC register                                 offset: 0x44
    __IO uint32_t RXFGMASK  ;                                                     ///< Rx FIFO global mask register                 offset: 0x48
    __IO uint32_t RXFIR   ;                                                       ///< Rx FIFO information register                 offset: 0x4C
    __IO uint32_t CBT  ;                                                          ///< CAN bit timing register                      offset: 0x50
    __IO uint32_t Reserved4[11];                                                  ///< Reserved
    struct {
        __IO uint32_t CS ;                                                        ///<  Message Buffer 0 CS Register                offset: 0x80
        __IO uint32_t ID;                                                         ///<  Message Buffer 0 ID Register                offset: 0x84
    	__IO uint32_t WORD0;                                                      ///<  Message Buffer 0 WORD0 Register             offset: 0x88
        __IO uint32_t WORD1;                                                      ///<  Message Buffer 0 WORD1 Register             offset: 0x8C
    } MB[16];
    __IO uint32_t Reserved5[448];                                                 ///< Reserved
    __IO uint32_t RXIMRN[16] ;                                                    ///<   Rx single mask register                    offset: 0x880
} FLEXCAN_Type;

/*******************************************************************************
 * FLEXCAN Type
 ******************************************************************************/

/*!
 * @addtogroup FLEXCAN_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief FLEXCAN_MCR    Register Bit Definition
 */

#define FLEXCAN_MCR_MDIS_SHIFT                  (31)
#define FLEXCAN_MCR_MDIS_MASK                   (0x01U << FLEXCAN_MCR_MDIS_SHIFT)
#define FLEXCAN_MCR_MDIS(x)                     (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_MDIS_SHIFT)) & FLEXCAN_MCR_MDIS_MASK)

#define FLEXCAN_MCR_FRZ_SHIFT                   (30)
#define FLEXCAN_MCR_FRZ_MASK                    (0x01U << FLEXCAN_MCR_FRZ_SHIFT)
#define FLEXCAN_MCR_FRZ(x)                      (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_FRZ_SHIFT)) & FLEXCAN_MCR_FRZ_MASK)

#define FLEXCAN_MCR_RFEN_SHIFT                  (29)
#define FLEXCAN_MCR_RFEN_MASK                   (0x01U << FLEXCAN_MCR_RFEN_SHIFT)
#define FLEXCAN_MCR_RFEN(x)                     (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_RFEN_SHIFT)) & FLEXCAN_MCR_RFEN_MASK)

#define FLEXCAN_MCR_HALT_SHIFT                  (28)
#define FLEXCAN_MCR_HALT_MASK                   (0x01U << FLEXCAN_MCR_HALT_SHIFT)
#define FLEXCAN_MCR_HALT(x)                     (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_HALT_SHIFT)) & FLEXCAN_MCR_HALT_MASK)

#define FLEXCAN_MCR_NOTRDY_SHIFT                (27)
#define FLEXCAN_MCR_NOTRDY_MASK                 (0x01U << FLEXCAN_MCR_NOTRDY_SHIFT)
#define FLEXCAN_MCR_NOTRDY(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_NOTRDY_SHIFT)) & FLEXCAN_MCR_NOTRDY_MASK)

#define FLEXCAN_MCR_SOFTRST_SHIFT               (25)
#define FLEXCAN_MCR_SOFTRST_MASK                (0x01U << FLEXCAN_MCR_SOFTRST_SHIFT)
#define FLEXCAN_MCR_SOFTRST(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_SOFTRST_SHIFT)) & FLEXCAN_MCR_SOFTRST_MASK)

#define FLEXCAN_MCR_FRZACK_SHIFT                (24)
#define FLEXCAN_MCR_FRZACK_MASK                 (0x01U << FLEXCAN_MCR_FRZACK_SHIFT)
#define FLEXCAN_MCR_FRZACK(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_FRZACK_SHIFT)) & FLEXCAN_MCR_FRZACK_MASK)

#define FLEXCAN_MCR_SUPV_SHIFT                  (23)
#define FLEXCAN_MCR_SUPV_MASK                   (0x01U << FLEXCAN_MCR_SUPV_SHIFT)
#define FLEXCAN_MCR_SUPV(x)                     (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_SUPV_SHIFT)) & FLEXCAN_MCR_SUPV_MASK)

#define FLEXCAN_MCR_SLFWAK_SHIFT                (22)
#define FLEXCAN_MCR_SLFWAK_MASK                 (0x01U << FLEXCAN_MCR_SLFWAK_SHIFT)
#define FLEXCAN_MCR_SLFWAK(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_SLFWAK_SHIFT)) & FLEXCAN_MCR_SLFWAK_MASK)

#define FLEXCAN_MCR_WRNEN_SHIFT                 (21)
#define FLEXCAN_MCR_WRNEN_MASK                  (0x01U << FLEXCAN_MCR_WRNEN_SHIFT)
#define FLEXCAN_MCR_WRNEN(x)                    (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_WRNEN_SHIFT)) & FLEXCAN_MCR_WRNEN_MASK)

#define FLEXCAN_MCR_LPMACK_SHIFT                (20)
#define FLEXCAN_MCR_LPMACK_MASK                 (0x01U << FLEXCAN_MCR_LPMACK_SHIFT)
#define FLEXCAN_MCR_LPMACK(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_LPMACK_SHIFT)) & FLEXCAN_MCR_LPMACK_MASK)

#define FLEXCAN_MCR_WAKSRC_SHIFT                (19)
#define FLEXCAN_MCR_WAKSRC_MASK                 (0x01U << FLEXCAN_MCR_WAKSRC_SHIFT)
#define FLEXCAN_MCR_WAKSRC(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_WAKSRC_SHIFT)) & FLEXCAN_MCR_WAKSRC_MASK)

#define FLEXCAN_MCR_SRXDIS_SHIFT                (17)
#define FLEXCAN_MCR_SRXDIS_MASK                 (0x01U << FLEXCAN_MCR_SRXDIS_SHIFT)
#define FLEXCAN_MCR_SRXDIS(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_SRXDIS_SHIFT)) & FLEXCAN_MCR_SRXDIS_MASK)

#define FLEXCAN_MCR_IRMQ_SHIFT                  (16)
#define FLEXCAN_MCR_IRMQ_MASK                   (0x01U << FLEXCAN_MCR_IRMQ_SHIFT)
#define FLEXCAN_MCR_IRMQ(x)                     (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_IRMQ_SHIFT)) & FLEXCAN_MCR_IRMQ_MASK)

#define FLEXCAN_MCR_DMA_SHIFT                   (15)
#define FLEXCAN_MCR_DMA_MASK                    (0x01U << FLEXCAN_MCR_DMA_SHIFT)
#define FLEXCAN_MCR_DMA(x)                      (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_DMA_SHIFT)) & FLEXCAN_MCR_DMA_MASK)

#define FLEXCAN_MCR_LPRIOEN_SHIFT               (13)
#define FLEXCAN_MCR_LPRIOEN_MASK                (0x01U << FLEXCAN_MCR_LPRIOEN_SHIFT)
#define FLEXCAN_MCR_LPRIOEN(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_LPRIOEN_SHIFT)) & FLEXCAN_MCR_LPRIOEN_MASK)

#define FLEXCAN_MCR_AEN_SHIFT                   (12)
#define FLEXCAN_MCR_AEN_MASK                    (0x01U << FLEXCAN_MCR_AEN_SHIFT)
#define FLEXCAN_MCR_AEN(x)                      (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_AEN_SHIFT)) & FLEXCAN_MCR_AEN_MASK)

#define FLEXCAN_MCR_IDAM_SHIFT                  (8)
#define FLEXCAN_MCR_IDAM_MASK                   (0x3U << FLEXCAN_MCR_IDAM_SHIFT)
#define FLEXCAN_MCR_IDAM(x)                     (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_IDAM_SHIFT)) & FLEXCAN_MCR_IDAM_MASK)

#define FLEXCAN_MCR_MAXMB_SHIFT                 (0)
#define FLEXCAN_MCR_MAXMB_MASK                  (0x7FU << FLEXCAN_MCR_MAXMB_SHIFT)
#define FLEXCAN_MCR_MAXMB(x)                    (((uint32_t)(((uint32_t)(x)) << FLEXCAN_MCR_MAXMB_SHIFT)) & FLEXCAN_MCR_MAXMB_MASK)

/*!
 * @brief FLEXCAN_CTRL1   Register Bit Definition
 */

#define FLEXCAN_CTRL1_PRESDIV_SHIFT             (24)
#define FLEXCAN_CTRL1_PRESDIV_MASK              (0xFFU << FLEXCAN_CTRL1_PRESDIV_SHIFT)
#define FLEXCAN_CTRL1_PRESDIV(x)                (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL1_PRESDIV_SHIFT)) & FLEXCAN_CTRL1_PRESDIV_MASK)

#define FLEXCAN_CTRL1_RJW_SHIFT                 (22)
#define FLEXCAN_CTRL1_RJW_MASK                  (0x3U << FLEXCAN_CTRL1_RJW_SHIFT)
#define FLEXCAN_CTRL1_RJW(x)                    (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL1_RJW_SHIFT)) & FLEXCAN_CTRL1_RJW_MASK)

#define FLEXCAN_CTRL1_PSEG1_SHIFT               (19)
#define FLEXCAN_CTRL1_PSEG1_MASK                (0x7U << FLEXCAN_CTRL1_PSEG1_SHIFT)
#define FLEXCAN_CTRL1_PSEG1(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL1_PSEG1_SHIFT)) & FLEXCAN_CTRL1_PSEG1_MASK)

#define FLEXCAN_CTRL1_PSEG2_SHIFT               (16)
#define FLEXCAN_CTRL1_PSEG2_MASK                (0x7U << FLEXCAN_CTRL1_PSEG2_SHIFT)
#define FLEXCAN_CTRL1_PSEG2(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL1_PSEG2_SHIFT)) & FLEXCAN_CTRL1_PSEG2_MASK)

#define FLEXCAN_CTRL1_BOFFMSK_SHIFT             (15)
#define FLEXCAN_CTRL1_BOFFMSK_MASK              (0x01U << FLEXCAN_CTRL1_BOFFMSK_SHIFT)
#define FLEXCAN_CTRL1_BOFFMSK(x)                (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL1_BOFFMSK_SHIFT)) & FLEXCAN_CTRL1_BOFFMSK_MASK)

#define FLEXCAN_CTRL1_ERRMSK_SHIFT              (14)
#define FLEXCAN_CTRL1_ERRMSK_MASK               (0x01U << FLEXCAN_CTRL1_ERRMSK_SHIFT)
#define FLEXCAN_CTRL1_ERRMSK(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL1_ERRMSK_SHIFT)) & FLEXCAN_CTRL1_ERRMSK_MASK)

#define FLEXCAN_CTRL1_CLKSRC_SHIFT              (13)
#define FLEXCAN_CTRL1_CLKSRC_MASK               (0x01U << FLEXCAN_CTRL1_CLKSRC_SHIFT)
#define FLEXCAN_CTRL1_CLKSRC(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL1_CLKSRC_SHIFT)) & FLEXCAN_CTRL1_CLKSRC_MASK)

#define FLEXCAN_CTRL1_LPB_SHIFT                 (12)
#define FLEXCAN_CTRL1_LPB_MASK                  (0x01U << FLEXCAN_CTRL1_LPB_SHIFT)
#define FLEXCAN_CTRL1_LPB(x)                    (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL1_LPB_SHIFT)) & FLEXCAN_CTRL1_LPB_MASK)

#define FLEXCAN_CTRL1_TWRNMSK_SHIFT             (11)
#define FLEXCAN_CTRL1_TWRNMSK_MASK              (0x01U << FLEXCAN_CTRL1_TWRNMSK_SHIFT)
#define FLEXCAN_CTRL1_TWRNMSK(x)                (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL1_TWRNMSK_SHIFT)) & FLEXCAN_CTRL1_TWRNMSK_MASK)

#define FLEXCAN_CTRL1_RWRNMSK_SHIFT             (10)
#define FLEXCAN_CTRL1_RWRNMSK_MASK              (0x01U << FLEXCAN_CTRL1_RWRNMSK_SHIFT)
#define FLEXCAN_CTRL1_RWRNMSK(x)                (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL1_RWRNMSK_SHIFT)) & FLEXCAN_CTRL1_RWRNMSK_MASK)

#define FLEXCAN_CTRL1_SMP_SHIFT                 (7)
#define FLEXCAN_CTRL1_SMP_MASK                  (0x01U << FLEXCAN_CTRL1_SMP_SHIFT)
#define FLEXCAN_CTRL1_SMP(x)                    (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL1_SMP_SHIFT)) & FLEXCAN_CTRL1_SMP_MASK)

#define FLEXCAN_CTRL1_BOFFREC_SHIFT             (6)
#define FLEXCAN_CTRL1_BOFFREC_MASK              (0x01U << FLEXCAN_CTRL1_BOFFREC_SHIFT)
#define FLEXCAN_CTRL1_BOFFREC(x)                (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL1_BOFFREC_SHIFT)) & FLEXCAN_CTRL1_BOFFREC_MASK)

#define FLEXCAN_CTRL1_TSYN_SHIFT                (5)
#define FLEXCAN_CTRL1_TSYN_MASK                 (0x01U << FLEXCAN_CTRL1_TSYN_SHIFT)
#define FLEXCAN_CTRL1_TSYN(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL1_TSYN_SHIFT)) & FLEXCAN_CTRL1_TSYN_MASK)

#define FLEXCAN_CTRL1_LBUF_SHIFT                (4)
#define FLEXCAN_CTRL1_LBUF_MASK                 (0x01U << FLEXCAN_CTRL1_LBUF_SHIFT)
#define FLEXCAN_CTRL1_LBUF(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL1_LBUF_SHIFT)) & FLEXCAN_CTRL1_LBUF_MASK)

#define FLEXCAN_CTRL1_LOM_SHIFT                 (3)
#define FLEXCAN_CTRL1_LOM_MASK                  (0x01U << FLEXCAN_CTRL1_LOM_SHIFT)
#define FLEXCAN_CTRL1_LOM(x)                    (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL1_LOM_SHIFT)) & FLEXCAN_CTRL1_LOM_MASK)

#define FLEXCAN_CTRL1_PROPSEG_SHIFT             (0)
#define FLEXCAN_CTRL1_PROPSEG_MASK              (0x7U << FLEXCAN_CTRL1_PROPSEG_SHIFT)
#define FLEXCAN_CTRL1_PROPSEG(x)                (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL1_PROPSEG_SHIFT)) & FLEXCAN_CTRL1_PROPSEG_MASK)

/*!
 * @brief FLEXCAN_TIMER  Register Bit Definition
 */

#define FLEXCAN_TIMER_TIMER_SHIFT               (0)
#define FLEXCAN_TIMER_TIMER_MASK                (0xFFFFU << FLEXCAN_TIMER_TIMER_SHIFT)
#define FLEXCAN_TIMER_TIMER(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_TIMER_TIMER_SHIFT)) & FLEXCAN_TIMER_TIMER_MASK)

/*!
 * @brief FLEXCAN_RXMGMASK Register Bit Definition
 */

#define FLEXCAN_RXMGMASK_MG_SHIFT               (0)
#define FLEXCAN_RXMGMASK_MG_MASK                (0xFFFFFFFFU << FLEXCAN_RXMGMASK_MG_SHIFT)
#define FLEXCAN_RXMGMASK_MG(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXMGMASK_MG_SHIFT)) & FLEXCAN_RXMGMASK_MG_MASK)

/*!
 * @brief FLEXCAN_RX14MASK Register Bit Definition
 */

#define FLEXCAN_RX14MASK_RX14M_SHIFT            (0)
#define FLEXCAN_RX14MASK_RX14M_MASK             (0xFFFFFFFFU << FLEXCAN_RX14MASK_RX14M_SHIFT)
#define FLEXCAN_RX14MASK_RX14M(x)               (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RX14MASK_RX14M_SHIFT)) & FLEXCAN_RX14MASK_RX14M_MASK)

/*!
 * @brief FLEXCAN_RX15MASK Register Bit Definition
 */

#define FLEXCAN_RX15MASK_RX15M_SHIFT            (0)
#define FLEXCAN_RX15MASK_RX15M_MASK             (0xFFFFFFFFU << FLEXCAN_RX15MASK_RX15M_SHIFT)
#define FLEXCAN_RX15MASK_RX15M(x)               (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RX15MASK_RX15M_SHIFT)) & FLEXCAN_RX15MASK_RX15M_MASK)

/*!
 * @brief FLEXCAN_ECR    Register Bit Definition
 */

#define FLEXCAN_ECR_RXERRCNT_SHIFT              (8)
#define FLEXCAN_ECR_RXERRCNT_MASK               (0xFFU << FLEXCAN_ECR_RXERRCNT_SHIFT)
#define FLEXCAN_ECR_RXERRCNT(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ECR_RXERRCNT_SHIFT)) & FLEXCAN_ECR_RXERRCNT_MASK)

#define FLEXCAN_ECR_TXERRCNT_SHIFT              (0)
#define FLEXCAN_ECR_TXERRCNT_MASK               (0xFFU << FLEXCAN_ECR_TXERRCNT_SHIFT)
#define FLEXCAN_ECR_TXERRCNT(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ECR_TXERRCNT_SHIFT)) & FLEXCAN_ECR_TXERRCNT_MASK)

/*!
 * @brief FLEXCAN_ESR1  Register Bit Definition
 */

#define FLEXCAN_ESR1_ERROVR_SHIFT               (21)
#define FLEXCAN_ESR1_ERROVR_MASK                (0x01U << FLEXCAN_ESR1_ERROVR_SHIFT)
#define FLEXCAN_ESR1_ERROVR(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_ERROVR_SHIFT)) & FLEXCAN_ESR1_ERROVR_MASK)

#define FLEXCAN_ESR1_BOFFDONEINT_SHIFT          (19)
#define FLEXCAN_ESR1_BOFFDONEINT_MASK           (0x01U << FLEXCAN_ESR1_BOFFDONEINT_SHIFT)
#define FLEXCAN_ESR1_BOFFDONEINT(x)             (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_BOFFDONEINT_SHIFT)) & FLEXCAN_ESR1_BOFFDONEINT_MASK)

#define FLEXCAN_ESR1_SYNCH_SHIFT                (18)
#define FLEXCAN_ESR1_SYNCH_MASK                 (0x01U << FLEXCAN_ESR1_SYNCH_SHIFT)
#define FLEXCAN_ESR1_SYNCH(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_SYNCH_SHIFT)) & FLEXCAN_ESR1_SYNCH_MASK)

#define FLEXCAN_ESR1_TWRNINT_SHIFT              (17)
#define FLEXCAN_ESR1_TWRNINT_MASK               (0x01U << FLEXCAN_ESR1_TWRNINT_SHIFT)
#define FLEXCAN_ESR1_TWRNINT(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_TWRNINT_SHIFT)) & FLEXCAN_ESR1_TWRNINT_MASK)

#define FLEXCAN_ESR1_RWRNINT_SHIFT              (16)
#define FLEXCAN_ESR1_RWRNINT_MASK               (0x01U << FLEXCAN_ESR1_RWRNINT_SHIFT)
#define FLEXCAN_ESR1_RWRNINT(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_RWRNINT_SHIFT)) & FLEXCAN_ESR1_RWRNINT_MASK)

#define FLEXCAN_ESR1_BIT1ERR_SHIFT              (15)
#define FLEXCAN_ESR1_BIT1ERR_MASK               (0x01U << FLEXCAN_ESR1_BIT1ERR_SHIFT)
#define FLEXCAN_ESR1_BIT1ERR(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_BIT1ERR_SHIFT)) & FLEXCAN_ESR1_BIT1ERR_MASK)

#define FLEXCAN_ESR1_BIT0ERR_SHIFT              (14)
#define FLEXCAN_ESR1_BIT0ERR_MASK               (0x01U << FLEXCAN_ESR1_BIT0ERR_SHIFT)
#define FLEXCAN_ESR1_BIT0ERR(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_BIT0ERR_SHIFT)) & FLEXCAN_ESR1_BIT0ERR_MASK)

#define FLEXCAN_ESR1_ACKERR_SHIFT               (13)
#define FLEXCAN_ESR1_ACKERR_MASK                (0x01U << FLEXCAN_ESR1_ACKERR_SHIFT)
#define FLEXCAN_ESR1_ACKERR(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_ACKERR_SHIFT)) & FLEXCAN_ESR1_ACKERR_MASK)

#define FLEXCAN_ESR1_CRCERR_SHIFT               (12)
#define FLEXCAN_ESR1_CRCERR_MASK                (0x01U << FLEXCAN_ESR1_CRCERR_SHIFT)
#define FLEXCAN_ESR1_CRCERR(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_CRCERR_SHIFT)) & FLEXCAN_ESR1_CRCERR_MASK)

#define FLEXCAN_ESR1_FRMERR_SHIFT               (11)
#define FLEXCAN_ESR1_FRMERR_MASK                (0x01U << FLEXCAN_ESR1_FRMERR_SHIFT)
#define FLEXCAN_ESR1_FRMERR(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_FRMERR_SHIFT)) & FLEXCAN_ESR1_FRMERR_MASK)

#define FLEXCAN_ESR1_STFERR_SHIFT               (10)
#define FLEXCAN_ESR1_STFERR_MASK                (0x01U << FLEXCAN_ESR1_STFERR_SHIFT)
#define FLEXCAN_ESR1_STFERR(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_STFERR_SHIFT)) & FLEXCAN_ESR1_STFERR_MASK)

#define FLEXCAN_ESR1_TXWRN_SHIFT                (9)
#define FLEXCAN_ESR1_TXWRN_MASK                 (0x01U << FLEXCAN_ESR1_TXWRN_SHIFT)
#define FLEXCAN_ESR1_TXWRN(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_TXWRN_SHIFT)) & FLEXCAN_ESR1_TXWRN_MASK)

#define FLEXCAN_ESR1_RXWRN_SHIFT                (8)
#define FLEXCAN_ESR1_RXWRN_MASK                 (0x01U << FLEXCAN_ESR1_RXWRN_SHIFT)
#define FLEXCAN_ESR1_RXWRN(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_RXWRN_SHIFT)) & FLEXCAN_ESR1_RXWRN_MASK)

#define FLEXCAN_ESR1_IDLE_SHIFT                 (7)
#define FLEXCAN_ESR1_IDLE_MASK                  (0x01U << FLEXCAN_ESR1_IDLE_SHIFT)
#define FLEXCAN_ESR1_IDLE(x)                    (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_IDLE_SHIFT)) & FLEXCAN_ESR1_IDLE_MASK)

#define FLEXCAN_ESR1_TX_SHIFT                   (6)
#define FLEXCAN_ESR1_TX_MASK                    (0x01U << FLEXCAN_ESR1_TX_SHIFT)
#define FLEXCAN_ESR1_TX(x)                      (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_TX_SHIFT)) & FLEXCAN_ESR1_TX_MASK)

#define FLEXCAN_ESR1_FLTCONF_SHIFT              (4)
#define FLEXCAN_ESR1_FLTCONF_MASK               (0x3U << FLEXCAN_ESR1_FLTCONF_SHIFT)
#define FLEXCAN_ESR1_FLTCONF(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_FLTCONF_SHIFT)) & FLEXCAN_ESR1_FLTCONF_MASK)

#define FLEXCAN_ESR1_RX_SHIFT                   (3)
#define FLEXCAN_ESR1_RX_MASK                    (0x01U << FLEXCAN_ESR1_RX_SHIFT)
#define FLEXCAN_ESR1_RX(x)                      (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_RX_SHIFT)) & FLEXCAN_ESR1_RX_MASK)

#define FLEXCAN_ESR1_BOFFINT_SHIFT              (2)
#define FLEXCAN_ESR1_BOFFINT_MASK               (0x01U << FLEXCAN_ESR1_BOFFINT_SHIFT)
#define FLEXCAN_ESR1_BOFFINT(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_BOFFINT_SHIFT)) & FLEXCAN_ESR1_BOFFINT_MASK)

#define FLEXCAN_ESR1_ERRINT_SHIFT               (1)
#define FLEXCAN_ESR1_ERRINT_MASK                (0x01U << FLEXCAN_ESR1_ERRINT_SHIFT)
#define FLEXCAN_ESR1_ERRINT(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_ERRINT_SHIFT)) & FLEXCAN_ESR1_ERRINT_MASK)

#define FLEXCAN_ESR1_WAKINT_SHIFT               (0)
#define FLEXCAN_ESR1_WAKINT_MASK                (0x01U << FLEXCAN_ESR1_WAKINT_SHIFT)
#define FLEXCAN_ESR1_WAKINT(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR1_WAKINT_SHIFT)) & FLEXCAN_ESR1_WAKINT_MASK)

/*!
 * @brief FLEXCAN_IMASK1    Register Bit Definition
 */

#define FLEXCAN_IMASK1_BUF15TO0M_SHIFT          (0)
#define FLEXCAN_IMASK1_BUF15TO0M_MASK           (0xFFFFU << FLEXCAN_IMASK1_BUF15TO0M_SHIFT)
#define FLEXCAN_IMASK1_BUF15TO0M(x)             (((uint32_t)(((uint32_t)(x)) << FLEXCAN_IMASK1_BUF15TO0M_SHIFT)) & FLEXCAN_IMASK1_BUF15TO0M_MASK)

/*!
 * @brief FLEXCAN_IFLAG1    Register Bit Definition
 */

#define FLEXCAN_IFLAG1_BUF15TO8I_SHIFT          (8)
#define FLEXCAN_IFLAG1_BUF15TO8I_MASK           (0xFFU << FLEXCAN_IFLAG1_BUF15TO8I_SHIFT)
#define FLEXCAN_IFLAG1_BUF15TO8I(x)             (((uint32_t)(((uint32_t)(x)) << FLEXCAN_IFLAG1_BUF15TO8I_SHIFT)) & FLEXCAN_IFLAG1_BUF15TO8I_MASK)

#define FLEXCAN_IFLAG1_BUF7I_SHIFT              (7)
#define FLEXCAN_IFLAG1_BUF7I_MASK               (0x01U << FLEXCAN_IFLAG1_BUF7I_SHIFT)
#define FLEXCAN_IFLAG1_BUF7I(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXCAN_IFLAG1_BUF7I_SHIFT)) & FLEXCAN_IFLAG1_BUF7I_MASK)

#define FLEXCAN_IFLAG1_BUF6I_SHIFT              (6)
#define FLEXCAN_IFLAG1_BUF6I_MASK               (0x01U << FLEXCAN_IFLAG1_BUF6I_SHIFT)
#define FLEXCAN_IFLAG1_BUF6I(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXCAN_IFLAG1_BUF6I_SHIFT)) & FLEXCAN_IFLAG1_BUF6I_MASK)

#define FLEXCAN_IFLAG1_BUF5I_SHIFT              (5)
#define FLEXCAN_IFLAG1_BUF5I_MASK               (0x01U << FLEXCAN_IFLAG1_BUF5I_SHIFT)
#define FLEXCAN_IFLAG1_BUF5I(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXCAN_IFLAG1_BUF5I_SHIFT)) & FLEXCAN_IFLAG1_BUF5I_MASK)

#define FLEXCAN_IFLAG1_BUF4TO1I_SHIFT           (1)
#define FLEXCAN_IFLAG1_BUF4TO1I_MASK            (0xFU << FLEXCAN_IFLAG1_BUF4TO1I_SHIFT)
#define FLEXCAN_IFLAG1_BUF4TO1I(x)              (((uint32_t)(((uint32_t)(x)) << FLEXCAN_IFLAG1_BUF4TO1I_SHIFT)) & FLEXCAN_IFLAG1_BUF4TO1I_MASK)

#define FLEXCAN_IFLAG1_BUF0I_SHIFT              (0)
#define FLEXCAN_IFLAG1_BUF0I_MASK               (0x01U << FLEXCAN_IFLAG1_BUF0I_SHIFT)
#define FLEXCAN_IFLAG1_BUF0I(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXCAN_IFLAG1_BUF0I_SHIFT)) & FLEXCAN_IFLAG1_BUF0I_MASK)

/*!
 * @brief FLEXCAN_CTRL2 Register Bit Definition
 */

#define FLEXCAN_CTRL2_BOFFDONEMSK_SHIFT         (30)
#define FLEXCAN_CTRL2_BOFFDONEMSK_MASK          (0x01U << FLEXCAN_CTRL2_BOFFDONEMSK_SHIFT)
#define FLEXCAN_CTRL2_BOFFDONEMSK(x)            (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL2_BOFFDONEMSK_SHIFT)) & FLEXCAN_CTRL2_BOFFDONEMSK_MASK)

#define FLEXCAN_CTRL2_RFFN_SHIFT                (24)
#define FLEXCAN_CTRL2_RFFN_MASK                 (0xFU << FLEXCAN_CTRL2_RFFN_SHIFT)
#define FLEXCAN_CTRL2_RFFN(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL2_RFFN_SHIFT)) & FLEXCAN_CTRL2_RFFN_MASK)

#define FLEXCAN_CTRL2_TASD_SHIFT                (19)
#define FLEXCAN_CTRL2_TASD_MASK                 (0x1FU << FLEXCAN_CTRL2_TASD_SHIFT)
#define FLEXCAN_CTRL2_TASD(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL2_TASD_SHIFT)) & FLEXCAN_CTRL2_TASD_MASK)

#define FLEXCAN_CTRL2_MRP_SHIFT                 (18)
#define FLEXCAN_CTRL2_MRP_MASK                  (0x01U << FLEXCAN_CTRL2_MRP_SHIFT)
#define FLEXCAN_CTRL2_MRP(x)                    (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL2_MRP_SHIFT)) & FLEXCAN_CTRL2_MRP_MASK)

#define FLEXCAN_CTRL2_RRS_SHIFT                 (17)
#define FLEXCAN_CTRL2_RRS_MASK                  (0x01U << FLEXCAN_CTRL2_RRS_SHIFT)
#define FLEXCAN_CTRL2_RRS(x)                    (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL2_RRS_SHIFT)) & FLEXCAN_CTRL2_RRS_MASK)

#define FLEXCAN_CTRL2_EACEN_SHIFT               (16)
#define FLEXCAN_CTRL2_EACEN_MASK                (0x01U << FLEXCAN_CTRL2_EACEN_SHIFT)
#define FLEXCAN_CTRL2_EACEN(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CTRL2_EACEN_SHIFT)) & FLEXCAN_CTRL2_EACEN_MASK)

/*!
 * @brief FLEXCAN_ESR2  Register Bit Definition
 */

#define FLEXCAN_ESR2_LPTM_SHIFT                 (16)
#define FLEXCAN_ESR2_LPTM_MASK                  (0x7FU << FLEXCAN_ESR2_LPTM_SHIFT)
#define FLEXCAN_ESR2_LPTM(x)                    (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR2_LPTM_SHIFT)) & FLEXCAN_ESR2_LPTM_MASK)

#define FLEXCAN_ESR2_VPS_SHIFT                  (14)
#define FLEXCAN_ESR2_VPS_MASK                   (0x01U << FLEXCAN_ESR2_VPS_SHIFT)
#define FLEXCAN_ESR2_VPS(x)                     (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR2_VPS_SHIFT)) & FLEXCAN_ESR2_VPS_MASK)

#define FLEXCAN_ESR2_IMB_SHIFT                  (13)
#define FLEXCAN_ESR2_IMB_MASK                   (0x01U << FLEXCAN_ESR2_IMB_SHIFT)
#define FLEXCAN_ESR2_IMB(x)                     (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ESR2_IMB_SHIFT)) & FLEXCAN_ESR2_IMB_MASK)

/*!
 * @brief FLEXCAN_CRCR  Register Bit Definition
 */

#define FLEXCAN_CRCR_MBCRC_SHIFT                (16)
#define FLEXCAN_CRCR_MBCRC_MASK                 (0x7FU << FLEXCAN_CRCR_MBCRC_SHIFT)
#define FLEXCAN_CRCR_MBCRC(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CRCR_MBCRC_SHIFT)) & FLEXCAN_CRCR_MBCRC_MASK)

#define FLEXCAN_CRCR_TXCRC_SHIFT                (0)
#define FLEXCAN_CRCR_TXCRC_MASK                 (0x7FFFU << FLEXCAN_CRCR_TXCRC_SHIFT)
#define FLEXCAN_CRCR_TXCRC(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CRCR_TXCRC_SHIFT)) & FLEXCAN_CRCR_TXCRC_MASK)

/*!
 * @brief FLEXCAN_RXFGMASK   Register Bit Definition
 */

#define FLEXCAN_RXFGMASK_FGM_SHIFT              (0)
#define FLEXCAN_RXFGMASK_FGM_MASK               (0xFFFFFFFFU << FLEXCAN_RXFGMASK_FGM_SHIFT)
#define FLEXCAN_RXFGMASK_FGM(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXFGMASK_FGM_SHIFT)) & FLEXCAN_RXFGMASK_FGM_MASK)

/*!
 * @brief FLEXCAN_RXFIR    Register Bit Definition
 */

#define FLEXCAN_RXFIR_IDHIT_SHIFT               (0)
#define FLEXCAN_RXFIR_IDHIT_MASK                (0x1FFU << FLEXCAN_RXFIR_IDHIT_SHIFT)
#define FLEXCAN_RXFIR_IDHIT(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXFIR_IDHIT_SHIFT)) & FLEXCAN_RXFIR_IDHIT_MASK)

/*!
 * @brief FLEXCAN_CBT   Register Bit Definition
 */

#define FLEXCAN_CBT_BTF_SHIFT                   (31)
#define FLEXCAN_CBT_BTF_MASK                    (0x01U << FLEXCAN_CBT_BTF_SHIFT)
#define FLEXCAN_CBT_BTF(x)                      (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CBT_BTF_SHIFT)) & FLEXCAN_CBT_BTF_MASK)

#define FLEXCAN_CBT_EPRESDIV_SHIFT              (21)
#define FLEXCAN_CBT_EPRESDIV_MASK               (0x3FFU << FLEXCAN_CBT_EPRESDIV_SHIFT)
#define FLEXCAN_CBT_EPRESDIV(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CBT_EPRESDIV_SHIFT)) & FLEXCAN_CBT_EPRESDIV_MASK)

#define FLEXCAN_CBT_ERJW_SHIFT                  (16)
#define FLEXCAN_CBT_ERJW_MASK                   (0x1FU << FLEXCAN_CBT_ERJW_SHIFT)
#define FLEXCAN_CBT_ERJW(x)                     (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CBT_ERJW_SHIFT)) & FLEXCAN_CBT_ERJW_MASK)

#define FLEXCAN_CBT_EPROPSEG_SHIFT              (10)
#define FLEXCAN_CBT_EPROPSEG_MASK               (0x3FU << FLEXCAN_CBT_EPROPSEG_SHIFT)
#define FLEXCAN_CBT_EPROPSEG(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CBT_EPROPSEG_SHIFT)) & FLEXCAN_CBT_EPROPSEG_MASK)

#define FLEXCAN_CBT_EPSEG1_SHIFT                (5)
#define FLEXCAN_CBT_EPSEG1_MASK                 (0x1FU << FLEXCAN_CBT_EPSEG1_SHIFT)
#define FLEXCAN_CBT_EPSEG1(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CBT_EPSEG1_SHIFT)) & FLEXCAN_CBT_EPSEG1_MASK)

#define FLEXCAN_CBT_EPSEG2_SHIFT                (0)
#define FLEXCAN_CBT_EPSEG2_MASK                 (0x1FU << FLEXCAN_CBT_EPSEG2_SHIFT)
#define FLEXCAN_CBT_EPSEG2(x)                   (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CBT_EPSEG2_SHIFT)) & FLEXCAN_CBT_EPSEG2_MASK)

/*!
 * @brief FLEXCAN_CS  Register Bit Definition
 */

#define FLEXCAN_CS_CODE_SHIFT                   (24)
#define FLEXCAN_CS_CODE_MASK                    (0xFU << FLEXCAN_CS_CODE_SHIFT)
#define FLEXCAN_CS_CODE(x)                      (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CS_CODE_SHIFT)) & FLEXCAN_CS_CODE_MASK)

#define FLEXCAN_CS_SRR_SHIFT                    (22)
#define FLEXCAN_CS_SRR_MASK                     (0x01U << FLEXCAN_CS_SRR_SHIFT)
#define FLEXCAN_CS_SRR(x)                       (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CS_SRR_SHIFT)) & FLEXCAN_CS_SRR_MASK)

#define FLEXCAN_CS_IDE_SHIFT                    (21)
#define FLEXCAN_CS_IDE_MASK                     (0x01U << FLEXCAN_CS_IDE_SHIFT)
#define FLEXCAN_CS_IDE(x)                       (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CS_IDE_SHIFT)) & FLEXCAN_CS_IDE_MASK)

#define FLEXCAN_CS_RTR_SHIFT                    (20)
#define FLEXCAN_CS_RTR_MASK                     (0x01U << FLEXCAN_CS_RTR_SHIFT)
#define FLEXCAN_CS_RTR(x)                       (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CS_RTR_SHIFT)) & FLEXCAN_CS_RTR_MASK)

#define FLEXCAN_CS_DLC_SHIFT                    (16)
#define FLEXCAN_CS_DLC_MASK                     (0xFU << FLEXCAN_CS_DLC_SHIFT)
#define FLEXCAN_CS_DLC(x)                       (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CS_DLC_SHIFT)) & FLEXCAN_CS_DLC_MASK)

#define FLEXCAN_CS_TIMESTAMP_SHIFT              (0)
#define FLEXCAN_CS_TIMESTAMP_MASK               (0xFFFFU << FLEXCAN_CS_TIMESTAMP_SHIFT)
#define FLEXCAN_CS_TIMESTAMP(x)                 (((uint32_t)(((uint32_t)(x)) << FLEXCAN_CS_TIMESTAMP_SHIFT)) & FLEXCAN_CS_TIMESTAMP_MASK)

/*!
 * @brief FLEXCAN_ID Register Bit Definition
 */

#define FLEXCAN_ID_PRIO_SHIFT                   (29)
#define FLEXCAN_ID_PRIO_MASK                    (0x7U << FLEXCAN_ID_PRIO_SHIFT)
#define FLEXCAN_ID_PRIO(x)                      (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ID_PRIO_SHIFT)) & FLEXCAN_ID_PRIO_MASK)

#define FLEXCAN_ID_STD_SHIFT                    (18)
#define FLEXCAN_ID_STD_MASK                     (0x7FFU << FLEXCAN_ID_STD_SHIFT)
#define FLEXCAN_ID_STD(x)                       (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ID_STD_SHIFT)) & FLEXCAN_ID_STD_MASK)

#define FLEXCAN_ID_EXT_SHIFT                    (0)
#define FLEXCAN_ID_EXT_MASK                     (0x3FFFFU << FLEXCAN_ID_EXT_SHIFT)
#define FLEXCAN_ID_EXT(x)                       (((uint32_t)(((uint32_t)(x)) << FLEXCAN_ID_EXT_SHIFT)) & FLEXCAN_ID_EXT_MASK)

/*!
 * @brief FLEXCAN_WORD0 Register Bit Definition
 */

#define FLEXCAN_WORD0_BYTE0_SHIFT               (24)
#define FLEXCAN_WORD0_BYTE0_MASK                (0xFFU << FLEXCAN_WORD0_BYTE0_SHIFT)
#define FLEXCAN_WORD0_BYTE0(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_WORD0_BYTE0_SHIFT)) & FLEXCAN_WORD0_BYTE0_MASK)

#define FLEXCAN_WORD0_BYTE1_SHIFT               (16)
#define FLEXCAN_WORD0_BYTE1_MASK                (0x3FU << FLEXCAN_WORD0_BYTE1_SHIFT)
#define FLEXCAN_WORD0_BYTE1(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_WORD0_BYTE1_SHIFT)) & FLEXCAN_WORD0_BYTE1_MASK)

#define FLEXCAN_WORD0_BYTE2_SHIFT               (8)
#define FLEXCAN_WORD0_BYTE2_MASK                (0xFFU << FLEXCAN_WORD0_BYTE2_SHIFT)
#define FLEXCAN_WORD0_BYTE2(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_WORD0_BYTE2_SHIFT)) & FLEXCAN_WORD0_BYTE2_MASK)

#define FLEXCAN_WORD0_BYTE3_SHIFT               (0)
#define FLEXCAN_WORD0_BYTE3_MASK                (0xFFU << FLEXCAN_WORD0_BYTE3_SHIFT)
#define FLEXCAN_WORD0_BYTE3(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_WORD0_BYTE3_SHIFT)) & FLEXCAN_WORD0_BYTE3_MASK)

/*!
 * @brief FLEXCAN_WORD1 Register Bit Definition
 */

#define FLEXCAN_WORD1_BYTE4_SHIFT               (24)
#define FLEXCAN_WORD1_BYTE4_MASK                (0xFFU << FLEXCAN_WORD1_BYTE4_SHIFT)
#define FLEXCAN_WORD1_BYTE4(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_WORD1_BYTE4_SHIFT)) & FLEXCAN_WORD1_BYTE4_MASK)

#define FLEXCAN_WORD1_BYTE5_SHIFT               (16)
#define FLEXCAN_WORD1_BYTE5_MASK                (0x3FU << FLEXCAN_WORD1_BYTE5_SHIFT)
#define FLEXCAN_WORD1_BYTE5(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_WORD1_BYTE5_SHIFT)) & FLEXCAN_WORD1_BYTE5_MASK)

#define FLEXCAN_WORD1_BYTE6_SHIFT               (8)
#define FLEXCAN_WORD1_BYTE6_MASK                (0xFFU << FLEXCAN_WORD1_BYTE6_SHIFT)
#define FLEXCAN_WORD1_BYTE6(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_WORD1_BYTE6_SHIFT)) & FLEXCAN_WORD1_BYTE6_MASK)

#define FLEXCAN_WORD1_BYTE7_SHIFT               (0)
#define FLEXCAN_WORD1_BYTE7_MASK                (0xFFU << FLEXCAN_WORD1_BYTE7_SHIFT)
#define FLEXCAN_WORD1_BYTE7(x)                  (((uint32_t)(((uint32_t)(x)) << FLEXCAN_WORD1_BYTE7_SHIFT)) & FLEXCAN_WORD1_BYTE7_MASK)

/*!
 * @brief FLEXCAN_RXIMRN  Register Bit Definition
 */

#define FLEXCAN_RXIMRN_MI_SHIFT                 (0)
#define FLEXCAN_RXIMRN_MI_MASK                  (0xFFFFFFFFU << FLEXCAN_RXIMRN_MI_SHIFT)
#define FLEXCAN_RXIMRN_MI(x)                    (((uint32_t)(((uint32_t)(x)) << FLEXCAN_RXIMRN_MI_SHIFT)) & FLEXCAN_RXIMRN_MI_MASK)

/*!
 * @}
 */ /* end of group FLEXCAN_Register_Masks */
/******************************************************************************
 *FLEXCAN Instance
*******************************************************************************/

#define FLEXCAN1              ((FLEXCAN_Type*)FLEXCAN1_BASE)
#define FLEXCAN2              ((FLEXCAN_Type*)FLEXCAN2_BASE)

/*!
 * @}
 */ /* end of group FLEXCAN_Peripheral_Access_Layer */




/*!
 * @addtogroup FSMC_Peripheral_Access_Layer FSMC Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * FSMC Type
 ******************************************************************************/
#define FSMC_SMTMGR_REG_NUM 3u /*!< the number of FSMC SMTMGR. */
typedef struct {
    __IO uint32_t Reserved37[21];                                                 ///< Reserved
    __IO uint32_t SMSKR[4];                                                       ///< Memory mask register                         offset: 0x54
    __IO uint32_t Reserved38[12];                                                 ///< Reserved
    __IO uint32_t SMTMGRSET[FSMC_SMTMGR_REG_NUM];                                 ///< Memory timing register 0                     offset: 0x94
    __IO uint32_t Reserved39[1];                                                  ///< Reserved
    __IO uint32_t SMCTLR;                                                         ///< Memory control register                      offset: 0xA4
} FSMC_Type;

/*******************************************************************************
 * FSMC Type
 ******************************************************************************/

/*!
 * @addtogroup FSMC_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief FSMC_SMSKR Register Bit Definition
 */

#define FSMC_SMSKR_REGSELECT_SHIFT             (8)
#define FSMC_SMSKR_REGSELECT_MASK              (0x7U << FSMC_SMSKR_REGSELECT_SHIFT)
#define FSMC_SMSKR_REGSELECT(x)                (((uint32_t)(((uint32_t)(x)) << FSMC_SMSKR_REGSELECT_SHIFT)) & FSMC_SMSKR_REGSELECT_MASK)

#define FSMC_SMSKR_MEMTYPE_SHIFT               (5)
#define FSMC_SMSKR_MEMTYPE_MASK                (0x7U << FSMC_SMSKR_MEMTYPE_SHIFT)
#define FSMC_SMSKR_MEMTYPE(x)                  (((uint32_t)(((uint32_t)(x)) << FSMC_SMSKR_MEMTYPE_SHIFT)) & FSMC_SMSKR_MEMTYPE_MASK)

#define FSMC_SMSKR_MEMSIZE_SHIFT               (0)
#define FSMC_SMSKR_MEMSIZE_MASK                (0x1FU << FSMC_SMSKR_MEMSIZE_SHIFT)
#define FSMC_SMSKR_MEMSIZE(x)                  (((uint32_t)(((uint32_t)(x)) << FSMC_SMSKR_MEMSIZE_SHIFT)) & FSMC_SMSKR_MEMSIZE_MASK)

/*!
 * @brief FSMC_SMTMGRSET Register Bit Definition
 */

#define FSMC_SMTMGRSET_SMREADPIPE_SHIFT         (28)
#define FSMC_SMTMGRSET_SMREADPIPE_MASK          (0x3U << FSMC_SMTMGRSET_SMREADPIPE_SHIFT)
#define FSMC_SMTMGRSET_SMREADPIPE(x)            (((uint32_t)(((uint32_t)(x)) << FSMC_SMTMGRSET_SMREADPIPE_SHIFT)) & FSMC_SMTMGRSET_SMREADPIPE_MASK)

#define FSMC_SMTMGRSET_READYMODE_SHIFT          (26)
#define FSMC_SMTMGRSET_READYMODE_MASK           (0x01U << FSMC_SMTMGRSET_READYMODE_SHIFT)
#define FSMC_SMTMGRSET_READYMODE(x)             (((uint32_t)(((uint32_t)(x)) << FSMC_SMTMGRSET_READYMODE_SHIFT)) & FSMC_SMTMGRSET_READYMODE_MASK)

#define FSMC_SMTMGRSET_TWP_SHIFT                (10)
#define FSMC_SMTMGRSET_TWP_MASK                 (0x3FU << FSMC_SMTMGRSET_TWP_SHIFT)
#define FSMC_SMTMGRSET_TWP(x)                   (((uint32_t)(((uint32_t)(x)) << FSMC_SMTMGRSET_TWP_SHIFT)) & FSMC_SMTMGRSET_TWP_MASK)

#define FSMC_SMTMGRSET_TWR_SHIFT                (8)
#define FSMC_SMTMGRSET_TWR_MASK                 (0x3U << FSMC_SMTMGRSET_TWR_SHIFT)
#define FSMC_SMTMGRSET_TWR(x)                   (((uint32_t)(((uint32_t)(x)) << FSMC_SMTMGRSET_TWR_SHIFT)) & FSMC_SMTMGRSET_TWR_MASK)

#define FSMC_SMTMGRSET_TAS_SHIFT                (6)
#define FSMC_SMTMGRSET_TAS_MASK                 (0x3U << FSMC_SMTMGRSET_TAS_SHIFT)
#define FSMC_SMTMGRSET_TAS(x)                   (((uint32_t)(((uint32_t)(x)) << FSMC_SMTMGRSET_TAS_SHIFT)) & FSMC_SMTMGRSET_TAS_MASK)

#define FSMC_SMTMGRSET_TRC_SHIFT                (0)
#define FSMC_SMTMGRSET_TRC_MASK                 (0x3FU << FSMC_SMTMGRSET_TRC_SHIFT)
#define FSMC_SMTMGRSET_TRC(x)                   (((uint32_t)(((uint32_t)(x)) << FSMC_SMTMGRSET_TRC_SHIFT)) & FSMC_SMTMGRSET_TRC_MASK)


/*!
 * @brief FSMC_SMCTLR Register Bit Definition
 */

#define FSMC_SMCTLR_SMDW2_SHIFT                 (13)
#define FSMC_SMCTLR_SMDW2_MASK                  (0x7U << FSMC_SMCTLR_SMDW2_SHIFT)
#define FSMC_SMCTLR_SMDW2(x)                    (((uint32_t)(((uint32_t)(x)) << FSMC_SMCTLR_SMDW2_SHIFT)) & FSMC_SMCTLR_SMDW2_MASK)

#define FSMC_SMCTLR_SMDW1_SHIFT                 (10)
#define FSMC_SMCTLR_SMDW1_MASK                  (0x7U << FSMC_SMCTLR_SMDW1_SHIFT)
#define FSMC_SMCTLR_SMDW1(x)                    (((uint32_t)(((uint32_t)(x)) << FSMC_SMCTLR_SMDW1_SHIFT)) & FSMC_SMCTLR_SMDW1_MASK)

#define FSMC_SMCTLR_SMDW_SHIFT                  (7)
#define FSMC_SMCTLR_SMDW_MASK                   (0x7U << FSMC_SMCTLR_SMDW_SHIFT)
#define FSMC_SMCTLR_SMDW(x)                     (((uint32_t)(((uint32_t)(x)) << FSMC_SMCTLR_SMDW_SHIFT)) & FSMC_SMCTLR_SMDW_MASK)

#define FSMC_SMCTLR_FLASHRP_SHIFT               (0)
#define FSMC_SMCTLR_FLASHRP_MASK                (0x01U << FSMC_SMCTLR_FLASHRP_SHIFT)
#define FSMC_SMCTLR_FLASHRP(x)                  (((uint32_t)(((uint32_t)(x)) << FSMC_SMCTLR_FLASHRP_SHIFT)) & FSMC_SMCTLR_FLASHRP_MASK)

/*!
 * @}
 */ /* end of group FSMC_Register_Masks */
/******************************************************************************
 * FSMC Instance
******************************************************************************/
#define FSMC                                ((FSMC_Type*)FSMC_BASE)

/*!
 * @}
 */ /* end of group FSMC_Peripheral_Access_Layer */




/*!
 * @addtogroup GPIO_Peripheral_Access_Layer GPIO Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * GPIO Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t CRL;                                                            ///< Port Configuration Low Register              offset: 0x00
    __IO uint32_t CRH;                                                            ///< Port Configuration High Register             offset: 0x04
    __IO uint32_t IDR;                                                            ///< Port Input Data Register                     offset: 0x08
    __IO uint32_t ODR;                                                            ///< PORT OUTPUT DATA REGISTER                    offset: 0x0C
    __IO uint32_t BSRR;                                                           ///< Port Set/Clear Register                      offset: 0x10
    __IO uint32_t BRR;                                                            ///< Port Bit Clear Register                      offset: 0x14
    __IO uint32_t LCKR;                                                           ///< Port Configuration Lock Register             offset: 0x18
    __IO uint32_t DCR;                                                            ///< PORT OUTPUT OPEN DRAIN CONTROL REGISTER      offset: 0x1C
    __IO uint32_t AFRL;                                                           ///< Port alternate function low-order register   offset: 0x20
    __IO uint32_t AFRH;                                                           ///< Port alternate function high-order register  offset: 0x24
} GPIO_Type;

/*******************************************************************************
 * GPIO Type
 ******************************************************************************/

/*!
 * @addtogroup GPIO_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief GPIO_CRL Register Bit Definition
 */

#define GPIO_CRL_CNF7_SHIFT                     (30)
#define GPIO_CRL_CNF7_MASK                      (0x3U << GPIO_CRL_CNF7_SHIFT)
#define GPIO_CRL_CNF7(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_CRL_CNF7_SHIFT)) & GPIO_CRL_CNF7_MASK)

#define GPIO_CRL_CNF6_SHIFT                     (26)
#define GPIO_CRL_CNF6_MASK                      (0x3U << GPIO_CRL_CNF6_SHIFT)
#define GPIO_CRL_CNF6(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_CRL_CNF6_SHIFT)) & GPIO_CRL_CNF6_MASK)

#define GPIO_CRL_CNF5_SHIFT                     (22)
#define GPIO_CRL_CNF5_MASK                      (0x3U << GPIO_CRL_CNF5_SHIFT)
#define GPIO_CRL_CNF5(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_CRL_CNF5_SHIFT)) & GPIO_CRL_CNF5_MASK)

#define GPIO_CRL_CNF4_SHIFT                     (18)
#define GPIO_CRL_CNF4_MASK                      (0x3U << GPIO_CRL_CNF4_SHIFT)
#define GPIO_CRL_CNF4(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_CRL_CNF4_SHIFT)) & GPIO_CRL_CNF4_MASK)

#define GPIO_CRL_CNF3_SHIFT                     (14)
#define GPIO_CRL_CNF3_MASK                      (0x3U << GPIO_CRL_CNF3_SHIFT)
#define GPIO_CRL_CNF3(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_CRL_CNF3_SHIFT)) & GPIO_CRL_CNF3_MASK)

#define GPIO_CRL_CNF2_SHIFT                     (10)
#define GPIO_CRL_CNF2_MASK                      (0x3U << GPIO_CRL_CNF2_SHIFT)
#define GPIO_CRL_CNF2(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_CRL_CNF2_SHIFT)) & GPIO_CRL_CNF2_MASK)

#define GPIO_CRL_CNF1_SHIFT                     (6)
#define GPIO_CRL_CNF1_MASK                      (0x3U << GPIO_CRL_CNF1_SHIFT)
#define GPIO_CRL_CNF1(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_CRL_CNF1_SHIFT)) & GPIO_CRL_CNF1_MASK)

#define GPIO_CRL_CNF0_SHIFT                     (2)
#define GPIO_CRL_CNF0_MASK                      (0x3U << GPIO_CRL_CNF0_SHIFT)
#define GPIO_CRL_CNF0(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_CRL_CNF0_SHIFT)) & GPIO_CRL_CNF0_MASK)

#define GPIO_CRL_MODE7_SHIFT                    (28)
#define GPIO_CRL_MODE7_MASK                     (0x3U << GPIO_CRL_MODE7_SHIFT)
#define GPIO_CRL_MODE7(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_CRL_MODE7_SHIFT)) & GPIO_CRL_MODE7_MASK)

#define GPIO_CRL_MODE6_SHIFT                    (24)
#define GPIO_CRL_MODE6_MASK                     (0x3U << GPIO_CRL_MODE6_SHIFT)
#define GPIO_CRL_MODE6(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_CRL_MODE6_SHIFT)) & GPIO_CRL_MODE6_MASK)

#define GPIO_CRL_MODE5_SHIFT                    (20)
#define GPIO_CRL_MODE5_MASK                     (0x3U << GPIO_CRL_MODE5_SHIFT)
#define GPIO_CRL_MODE5(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_CRL_MODE5_SHIFT)) & GPIO_CRL_MODE5_MASK)

#define GPIO_CRL_MODE4_SHIFT                    (16)
#define GPIO_CRL_MODE4_MASK                     (0x3U << GPIO_CRL_MODE4_SHIFT)
#define GPIO_CRL_MODE4(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_CRL_MODE4_SHIFT)) & GPIO_CRL_MODE4_MASK)

#define GPIO_CRL_MODE3_SHIFT                    (12)
#define GPIO_CRL_MODE3_MASK                     (0x3U << GPIO_CRL_MODE3_SHIFT)
#define GPIO_CRL_MODE3(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_CRL_MODE3_SHIFT)) & GPIO_CRL_MODE3_MASK)

#define GPIO_CRL_MODE2_SHIFT                    (8)
#define GPIO_CRL_MODE2_MASK                     (0x3U << GPIO_CRL_MODE2_SHIFT)
#define GPIO_CRL_MODE2(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_CRL_MODE2_SHIFT)) & GPIO_CRL_MODE2_MASK)

#define GPIO_CRL_MODE1_SHIFT                    (4)
#define GPIO_CRL_MODE1_MASK                     (0x3U << GPIO_CRL_MODE1_SHIFT)
#define GPIO_CRL_MODE1(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_CRL_MODE1_SHIFT)) & GPIO_CRL_MODE1_MASK)

#define GPIO_CRL_MODE0_SHIFT                    (0)
#define GPIO_CRL_MODE0_MASK                     (0x3U << GPIO_CRL_MODE0_SHIFT)
#define GPIO_CRL_MODE0(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_CRL_MODE0_SHIFT)) & GPIO_CRL_MODE0_MASK)

/*!
 * @brief GPIO_CRH Register Bit Definition
 */

#define GPIO_CRH_CNF15_SHIFT                    (30)
#define GPIO_CRH_CNF15_MASK                     (0x3U << GPIO_CRH_CNF15_SHIFT)
#define GPIO_CRH_CNF15(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_CRH_CNF15_SHIFT)) & GPIO_CRH_CNF15_MASK)

#define GPIO_CRH_CNF14_SHIFT                    (26)
#define GPIO_CRH_CNF14_MASK                     (0x3U << GPIO_CRH_CNF14_SHIFT)
#define GPIO_CRH_CNF14(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_CRH_CNF14_SHIFT)) & GPIO_CRH_CNF14_MASK)

#define GPIO_CRH_CNF13_SHIFT                    (22)
#define GPIO_CRH_CNF13_MASK                     (0x3U << GPIO_CRH_CNF13_SHIFT)
#define GPIO_CRH_CNF13(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_CRH_CNF13_SHIFT)) & GPIO_CRH_CNF13_MASK)

#define GPIO_CRH_CNF12_SHIFT                    (18)
#define GPIO_CRH_CNF12_MASK                     (0x3U << GPIO_CRH_CNF12_SHIFT)
#define GPIO_CRH_CNF12(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_CRH_CNF12_SHIFT)) & GPIO_CRH_CNF12_MASK)

#define GPIO_CRH_CNF11_SHIFT                    (14)
#define GPIO_CRH_CNF11_MASK                     (0x3U << GPIO_CRH_CNF11_SHIFT)
#define GPIO_CRH_CNF11(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_CRH_CNF11_SHIFT)) & GPIO_CRH_CNF11_MASK)

#define GPIO_CRH_CNF10_SHIFT                    (10)
#define GPIO_CRH_CNF10_MASK                     (0x3U << GPIO_CRH_CNF10_SHIFT)
#define GPIO_CRH_CNF10(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_CRH_CNF10_SHIFT)) & GPIO_CRH_CNF10_MASK)

#define GPIO_CRH_CNF9_SHIFT                     (6)
#define GPIO_CRH_CNF9_MASK                      (0x3U << GPIO_CRH_CNF9_SHIFT)
#define GPIO_CRH_CNF9(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_CRH_CNF9_SHIFT)) & GPIO_CRH_CNF9_MASK)

#define GPIO_CRH_CNF8_SHIFT                     (2)
#define GPIO_CRH_CNF8_MASK                      (0x3U << GPIO_CRH_CNF8_SHIFT)
#define GPIO_CRH_CNF8(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_CRH_CNF8_SHIFT)) & GPIO_CRH_CNF8_MASK)

#define GPIO_CRH_MODE15_SHIFT                   (28)
#define GPIO_CRH_MODE15_MASK                    (0x3U << GPIO_CRH_MODE15_SHIFT)
#define GPIO_CRH_MODE15(x)                      (((uint32_t)(((uint32_t)(x)) << GPIO_CRH_MODE15_SHIFT)) & GPIO_CRH_MODE15_MASK)

#define GPIO_CRH_MODE14_SHIFT                   (24)
#define GPIO_CRH_MODE14_MASK                    (0x3U << GPIO_CRH_MODE14_SHIFT)
#define GPIO_CRH_MODE14(x)                      (((uint32_t)(((uint32_t)(x)) << GPIO_CRH_MODE14_SHIFT)) & GPIO_CRH_MODE14_MASK)

#define GPIO_CRH_MODE13_SHIFT                   (20)
#define GPIO_CRH_MODE13_MASK                    (0x3U << GPIO_CRH_MODE13_SHIFT)
#define GPIO_CRH_MODE13(x)                      (((uint32_t)(((uint32_t)(x)) << GPIO_CRH_MODE13_SHIFT)) & GPIO_CRH_MODE13_MASK)

#define GPIO_CRH_MODE12_SHIFT                   (16)
#define GPIO_CRH_MODE12_MASK                    (0x3U << GPIO_CRH_MODE12_SHIFT)
#define GPIO_CRH_MODE12(x)                      (((uint32_t)(((uint32_t)(x)) << GPIO_CRH_MODE12_SHIFT)) & GPIO_CRH_MODE12_MASK)

#define GPIO_CRH_MODE11_SHIFT                   (12)
#define GPIO_CRH_MODE11_MASK                    (0x3U << GPIO_CRH_MODE11_SHIFT)
#define GPIO_CRH_MODE11(x)                      (((uint32_t)(((uint32_t)(x)) << GPIO_CRH_MODE11_SHIFT)) & GPIO_CRH_MODE11_MASK)

#define GPIO_CRH_MODE10_SHIFT                   (8)
#define GPIO_CRH_MODE10_MASK                    (0x3U << GPIO_CRH_MODE10_SHIFT)
#define GPIO_CRH_MODE10(x)                      (((uint32_t)(((uint32_t)(x)) << GPIO_CRH_MODE10_SHIFT)) & GPIO_CRH_MODE10_MASK)

#define GPIO_CRH_MODE9_SHIFT                    (4)
#define GPIO_CRH_MODE9_MASK                     (0x3U << GPIO_CRH_MODE9_SHIFT)
#define GPIO_CRH_MODE9(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_CRH_MODE9_SHIFT)) & GPIO_CRH_MODE9_MASK)

#define GPIO_CRH_MODE8_SHIFT                    (0)
#define GPIO_CRH_MODE8_MASK                     (0x3U << GPIO_CRH_MODE8_SHIFT)
#define GPIO_CRH_MODE8(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_CRH_MODE8_SHIFT)) & GPIO_CRH_MODE8_MASK)

/*!
 * @brief GPIO_IDR Register Bit Definition
 */

#define GPIO_IDR_IDRY_SHIFT                     (0)
#define GPIO_IDR_IDRY_MASK                      (0xFFFFU << GPIO_IDR_IDRY_SHIFT)
#define GPIO_IDR_IDRY(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_IDR_IDRY_SHIFT)) & GPIO_IDR_IDRY_MASK)

/*!
 * @brief GPIO_ODR Register Bit Definition
 */

#define GPIO_ODR_ODRY_SHIFT                     (0)
#define GPIO_ODR_ODRY_MASK                      (0xFFFFU << GPIO_ODR_ODRY_SHIFT)
#define GPIO_ODR_ODRY(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_ODR_ODRY_SHIFT)) & GPIO_ODR_ODRY_MASK)

/*!
 * @brief GPIO_BSRR Register Bit Definition
 */

#define GPIO_BSRR_BRY_SHIFT                     (16)
#define GPIO_BSRR_BRY_MASK                      (0xFFFFU << GPIO_BSRR_BRY_SHIFT)
#define GPIO_BSRR_BRY(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_BSRR_BRY_SHIFT)) & GPIO_BSRR_BRY_MASK)

#define GPIO_BSRR_BSY_SHIFT                     (0)
#define GPIO_BSRR_BSY_MASK                      (0xFFFFU << GPIO_BSRR_BSY_SHIFT)
#define GPIO_BSRR_BSY(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_BSRR_BSY_SHIFT)) & GPIO_BSRR_BSY_MASK)

/*!
 * @brief GPIO_BRR Register Bit Definition
 */

#define GPIO_BRR_BRY_SHIFT                      (0)
#define GPIO_BRR_BRY_MASK                       (0xFFFFU << GPIO_BRR_BRY_SHIFT)
#define GPIO_BRR_BRY(x)                         (((uint32_t)(((uint32_t)(x)) << GPIO_BRR_BRY_SHIFT)) & GPIO_BRR_BRY_MASK)

/*!
 * @brief GPIO_LCKR Register Bit Definition
 */

#define GPIO_LCKR_PX15PX1_SHIFT                 (2)
#define GPIO_LCKR_PX15PX1_MASK                  (0x3FFFFFFFU << GPIO_LCKR_PX15PX1_SHIFT)
#define GPIO_LCKR_PX15PX1(x)                    (((uint32_t)(((uint32_t)(x)) << GPIO_LCKR_PX15PX1_SHIFT)) & GPIO_LCKR_PX15PX1_MASK)

#define GPIO_LCKR_LCKK_SHIFT                    (16)
#define GPIO_LCKR_LCKK_MASK                     (0x01U << GPIO_LCKR_LCKK_SHIFT)
#define GPIO_LCKR_LCKK(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_LCKR_LCKK_SHIFT)) & GPIO_LCKR_LCKK_MASK)

#define GPIO_LCKR_LCK_SHIFT                     (0)
#define GPIO_LCKR_LCK_MASK                      (0xFFFFU << GPIO_LCKR_LCK_SHIFT)
#define GPIO_LCKR_LCK(x)                        (((uint32_t)(((uint32_t)(x)) << GPIO_LCKR_LCK_SHIFT)) & GPIO_LCKR_LCK_MASK)

/*!
 * @brief GPIO_DCR Register Bit Definition
 */

#define GPIO_DCR_PX15PX1_SHIFT                  (2)
#define GPIO_DCR_PX15PX1_MASK                   (0x3FFFFFFFU << GPIO_DCR_PX15PX1_SHIFT)
#define GPIO_DCR_PX15PX1(x)                     (((uint32_t)(((uint32_t)(x)) << GPIO_DCR_PX15PX1_SHIFT)) & GPIO_DCR_PX15PX1_MASK)

#define GPIO_DCR_PX0_SHIFT                      (0)
#define GPIO_DCR_PX0_MASK                       (0x3U << GPIO_DCR_PX0_SHIFT)
#define GPIO_DCR_PX0(x)                         (((uint32_t)(((uint32_t)(x)) << GPIO_DCR_PX0_SHIFT)) & GPIO_DCR_PX0_MASK)

/*!
 * @brief GPIO_AFRL Register Bit Definition
 */

#define GPIO_AFRL_AFRY_SHIFT                    (0)
#define GPIO_AFRL_AFRY_MASK                     (0xFFFFFFFFU << GPIO_AFRL_AFRY_SHIFT)
#define GPIO_AFRL_AFRY(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_AFRL_AFRY_SHIFT)) & GPIO_AFRL_AFRY_MASK)

/*!
 * @brief GPIO_AFRH Register Bit Definition
 */

#define GPIO_AFRH_AFRY_SHIFT                    (0)
#define GPIO_AFRH_AFRY_MASK                     (0xFFFFFFFFU << GPIO_AFRH_AFRY_SHIFT)
#define GPIO_AFRH_AFRY(x)                       (((uint32_t)(((uint32_t)(x)) << GPIO_AFRH_AFRY_SHIFT)) & GPIO_AFRH_AFRY_MASK)

/*!
 * @}
 */ /* end of group GPIO_Register_Masks */
/******************************************************************************
 *GPIO Instance
*******************************************************************************/

#define GPIOA                 ((GPIO_Type*)GPIOA_BASE)
#define GPIOB                 ((GPIO_Type*)GPIOB_BASE)
#define GPIOC                 ((GPIO_Type*)GPIOC_BASE)
#define GPIOD                 ((GPIO_Type*)GPIOD_BASE)
#define GPIOE                 ((GPIO_Type*)GPIOE_BASE)
#define GPIOF                 ((GPIO_Type*)GPIOF_BASE)
#define GPIOG                 ((GPIO_Type*)GPIOG_BASE)
#define GPIOH                 ((GPIO_Type*)GPIOH_BASE)
#define GPIOI                 ((GPIO_Type*)GPIOI_BASE)

/*!
 * @}
 */ /* end of group GPIO_Peripheral_Access_Layer */




/*!
 * @addtogroup I2C_Peripheral_Access_Layer I2C Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * I2C Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t CR;                                                             ///< control register                             offset: 0x00
    __IO uint32_t TAR;                                                            ///< target address register                      offset: 0x04
    __IO uint32_t SAR;                                                            ///< slave address register                       offset: 0x08
    __IO uint32_t Reserved0[1];                                                   ///< Reserved
    __IO uint32_t DR;                                                             ///< Data Command Register                        offset: 0x10
    __IO uint32_t SSHR;                                                           ///< Standard Mode Clock High Count Register      offset: 0x14
    __IO uint32_t SSLR;                                                           ///< Standard Mode Clock Low Count Register       offset: 0x18
    __IO uint32_t FSHR;                                                           ///< Fast/Super Fast Mode Clock High Count Registeroffset: 0x1C
    __IO uint32_t FSLR;                                                           ///< Fast/Super Fast Mode Clock Low Count Registeroffset: 0x20
    __IO uint32_t Reserved1[2];                                                   ///< Reserved
    __IO uint32_t ISR;                                                            ///< Interrupt Status Register                    offset: 0x2C
    __IO uint32_t IMR;                                                            ///< interrupt mask register                      offset: 0x30
    __IO uint32_t RAWISR;                                                         ///< RAW interrupt register                       offset: 0x34
    __IO uint32_t RXTLR;                                                          ///< receive threshold register                   offset: 0x38
    __IO uint32_t TXTLR;                                                          ///< Transmit Threshold Register                  offset: 0x3C
    __IO uint32_t ICR;                                                            ///< Combined and Independent Interrupt Clear Registersoffset: 0x40
    __IO uint32_t RXUNDER;                                                        ///< Clear the RX_UNDER interrupt register        offset: 0x44
    __IO uint32_t RXOVER;                                                         ///< Clear the RX_OVER interrupt register         offset: 0x48
    __IO uint32_t TXOVER;                                                         ///< Clear the TX_OVER interrupt register         offset: 0x4C
    __IO uint32_t RDREQ;                                                          ///< Clear the RD_REQ interrupt register          offset: 0x50
    __IO uint32_t TXABRT;                                                         ///< Clear the TX_ABRT interrupt register         offset: 0x54
    __IO uint32_t RXDONE;                                                         ///< Clear the RX_DONE interrupt register         offset: 0x58
    __IO uint32_t ACTIV;                                                          ///< Clear the ACTIVITY interrupt register        offset: 0x5C
    __IO uint32_t STOP;                                                           ///< Clear the STOP_DET interrupt register        offset: 0x60
    __IO uint32_t START;                                                          ///< Clear the START_DET interrupt register       offset: 0x64
    __IO uint32_t GC;                                                             ///< Clear the GEN_CALL interrupt register        offset: 0x68
    __IO uint32_t ENR;                                                            ///< enable register                              offset: 0x6C
    __IO uint32_t SR;                                                             ///< status register                              offset: 0x70
    __IO uint32_t TXFLR;                                                          ///< Transmit buffer level register               offset: 0x74
    __IO uint32_t RXFLR;                                                          ///< Receive buffer level register                offset: 0x78
    __IO uint32_t HOLD;                                                           ///< SDA hold time register                       offset: 0x7C
    __IO uint32_t TXABRTSRC;                                                      ///< Transfer Abort Source Register               offset: 0x80
    __IO uint32_t SLVNACK;                                                        ///< Slave Receive NACK Register                  offset: 0x84
    __IO uint32_t DMA;                                                            ///< DMA Control Register                         offset: 0x88
    __IO uint32_t Reserved2[2];                                                   ///< Reserved
    __IO uint32_t SETUP;                                                          ///< SDA setup time register                      offset: 0x94
    __IO uint32_t GCR;                                                            ///< General Call ACK Register                    offset: 0x98
    __IO uint32_t ENSR;                                                           ///< ENABLE Status Register                       offset: 0x9C
    __IO uint32_t SPKLEN;                                                         ///< filter register                              offset: 0xA0
    __IO uint32_t Reserved3[2];                                                   ///< Reserved
    __IO uint32_t SCLTMO;                                                         ///< SCL Low Timeout Register                     offset: 0xAC
    __IO uint32_t SDATMO;                                                         ///< SDA Low Timeout Register                     offset: 0xB0
    __IO uint32_t SCLSTUCK;                                                       ///< Clear the SCL_STUCK interrupt register       offset: 0xB4
    __IO uint32_t Reserved4[1];                                                   ///< Reserved
    __IO uint32_t SMBSEXT;                                                        ///< SMBus Slave Clock Stretching Timeout Registeroffset: 0xBC
    __IO uint32_t SMBMEXT;                                                        ///< SMBus Master Clock Stretching Timeout Registeroffset: 0xC0
    __IO uint32_t SMBIDLE;                                                        ///< SMBus Bus Idle Count Register                offset: 0xC4
    __IO uint32_t SMBISR;                                                         ///< SMBus Interrupt Status Register              offset: 0xC8
    __IO uint32_t SMBIMR;                                                         ///< SMBus Interrupt Mask Register                offset: 0xCC
    __IO uint32_t SMBRAWISR;                                                      ///< SMBus RAW Interrupt Register                 offset: 0xD0
    __IO uint32_t SMBICR;                                                         ///< SMBus Combined and Independent Interrupt Clear Registersoffset: 0xD4
    __IO uint32_t OPTSAR;                                                         ///< Optional Slave Address Register              offset: 0xD8
    __IO uint32_t SMBUDIDLSB;                                                     ///< SMBus UDID LSB Register                      offset: 0xDC
    __IO uint32_t SMBUDIDMSB0;                                                    ///< SMBus UDID MSB Register 0                    offset: 0XE0
    __IO uint32_t SMBUDIDMSB1;                                                    ///< SMBus UDID MSB Register 1                    offset: 0XE4
    __IO uint32_t SMBUDIDMSB2;                                                    ///< SMBus UDID MSB Register 2                    offset: 0xE8
    __IO uint32_t SLVMASK;                                                        ///< Slave Address Mask Register                  offset: 0xEC
    __IO uint32_t SLVRCVADDR;                                                     ///< Slave Receive Address Register               offset: 0xF0
} I2C_Type;

/*******************************************************************************
 * I2C Type
 ******************************************************************************/

/*!
 * @addtogroup I2C_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief I2C_CR Register Bit Definition
 */

#define I2C_CR_RXFULLHLD_SHIFT                  (22)
#define I2C_CR_RXFULLHLD_MASK                   (0x01U << I2C_CR_RXFULLHLD_SHIFT)
#define I2C_CR_RXFULLHLD(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_CR_RXFULLHLD_SHIFT)) & I2C_CR_RXFULLHLD_MASK)

#define I2C_CR_BUSCLR_SHIFT                     (21)
#define I2C_CR_BUSCLR_MASK                      (0x01U << I2C_CR_BUSCLR_SHIFT)
#define I2C_CR_BUSCLR(x)                        (((uint32_t)(((uint32_t)(x)) << I2C_CR_BUSCLR_SHIFT)) & I2C_CR_BUSCLR_MASK)

#define I2C_CR_STOPDETMSTACT_SHIFT              (20)
#define I2C_CR_STOPDETMSTACT_MASK               (0x01U << I2C_CR_STOPDETMSTACT_SHIFT)
#define I2C_CR_STOPDETMSTACT(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_CR_STOPDETMSTACT_SHIFT)) & I2C_CR_STOPDETMSTACT_MASK)

#define I2C_CR_SMBPSAEN_SHIFT                   (19)
#define I2C_CR_SMBPSAEN_MASK                    (0x01U << I2C_CR_SMBPSAEN_SHIFT)
#define I2C_CR_SMBPSAEN(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_CR_SMBPSAEN_SHIFT)) & I2C_CR_SMBPSAEN_MASK)

#define I2C_CR_SMBARPEN_SHIFT                   (18)
#define I2C_CR_SMBARPEN_MASK                    (0x01U << I2C_CR_SMBARPEN_SHIFT)
#define I2C_CR_SMBARPEN(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_CR_SMBARPEN_SHIFT)) & I2C_CR_SMBARPEN_MASK)

#define I2C_CR_SMBSLVQCEN_SHIFT                 (17)
#define I2C_CR_SMBSLVQCEN_MASK                  (0x01U << I2C_CR_SMBSLVQCEN_SHIFT)
#define I2C_CR_SMBSLVQCEN(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_CR_SMBSLVQCEN_SHIFT)) & I2C_CR_SMBSLVQCEN_MASK)

#define I2C_CR_OPTSAREN_SHIFT                   (16)
#define I2C_CR_OPTSAREN_MASK                    (0x01U << I2C_CR_OPTSAREN_SHIFT)
#define I2C_CR_OPTSAREN(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_CR_OPTSAREN_SHIFT)) & I2C_CR_OPTSAREN_MASK)

#define I2C_CR_PADSEL_SHIFT                     (12)
#define I2C_CR_PADSEL_MASK                      (0x01U << I2C_CR_PADSEL_SHIFT)
#define I2C_CR_PADSEL(x)                        (((uint32_t)(((uint32_t)(x)) << I2C_CR_PADSEL_SHIFT)) & I2C_CR_PADSEL_MASK)

#define I2C_CR_SLVTXABRTDIS_SHIFT               (11)
#define I2C_CR_SLVTXABRTDIS_MASK                (0x01U << I2C_CR_SLVTXABRTDIS_SHIFT)
#define I2C_CR_SLVTXABRTDIS(x)                  (((uint32_t)(((uint32_t)(x)) << I2C_CR_SLVTXABRTDIS_SHIFT)) & I2C_CR_SLVTXABRTDIS_MASK)

#define I2C_CR_RESTART_SHIFT                    (10)
#define I2C_CR_RESTART_MASK                     (0x01U << I2C_CR_RESTART_SHIFT)
#define I2C_CR_RESTART(x)                       (((uint32_t)(((uint32_t)(x)) << I2C_CR_RESTART_SHIFT)) & I2C_CR_RESTART_MASK)

#define I2C_CR_STOP_SHIFT                       (9)
#define I2C_CR_STOP_MASK                        (0x01U << I2C_CR_STOP_SHIFT)
#define I2C_CR_STOP(x)                          (((uint32_t)(((uint32_t)(x)) << I2C_CR_STOP_SHIFT)) & I2C_CR_STOP_MASK)

#define I2C_CR_EMPINT_SHIFT                     (8)
#define I2C_CR_EMPINT_MASK                      (0x01U << I2C_CR_EMPINT_SHIFT)
#define I2C_CR_EMPINT(x)                        (((uint32_t)(((uint32_t)(x)) << I2C_CR_EMPINT_SHIFT)) & I2C_CR_EMPINT_MASK)

#define I2C_CR_STOPINT_SHIFT                    (7)
#define I2C_CR_STOPINT_MASK                     (0x01U << I2C_CR_STOPINT_SHIFT)
#define I2C_CR_STOPINT(x)                       (((uint32_t)(((uint32_t)(x)) << I2C_CR_STOPINT_SHIFT)) & I2C_CR_STOPINT_MASK)

#define I2C_CR_DISSLAVE_SHIFT                   (6)
#define I2C_CR_DISSLAVE_MASK                    (0x01U << I2C_CR_DISSLAVE_SHIFT)
#define I2C_CR_DISSLAVE(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_CR_DISSLAVE_SHIFT)) & I2C_CR_DISSLAVE_MASK)

#define I2C_CR_REPEN_SHIFT                      (5)
#define I2C_CR_REPEN_MASK                       (0x01U << I2C_CR_REPEN_SHIFT)
#define I2C_CR_REPEN(x)                         (((uint32_t)(((uint32_t)(x)) << I2C_CR_REPEN_SHIFT)) & I2C_CR_REPEN_MASK)

#define I2C_CR_MASTER10_SHIFT                   (4)
#define I2C_CR_MASTER10_MASK                    (0x01U << I2C_CR_MASTER10_SHIFT)
#define I2C_CR_MASTER10(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_CR_MASTER10_SHIFT)) & I2C_CR_MASTER10_MASK)

#define I2C_CR_SLAVE10_SHIFT                    (3)
#define I2C_CR_SLAVE10_MASK                     (0x01U << I2C_CR_SLAVE10_SHIFT)
#define I2C_CR_SLAVE10(x)                       (((uint32_t)(((uint32_t)(x)) << I2C_CR_SLAVE10_SHIFT)) & I2C_CR_SLAVE10_MASK)

#define I2C_CR_SPEED_SHIFT                      (1)
#define I2C_CR_SPEED_MASK                       (0x3U << I2C_CR_SPEED_SHIFT)
#define I2C_CR_SPEED(x)                         (((uint32_t)(((uint32_t)(x)) << I2C_CR_SPEED_SHIFT)) & I2C_CR_SPEED_MASK)

#define I2C_CR_MASTER_SHIFT                     (0)
#define I2C_CR_MASTER_MASK                      (0x01U << I2C_CR_MASTER_SHIFT)
#define I2C_CR_MASTER(x)                        (((uint32_t)(((uint32_t)(x)) << I2C_CR_MASTER_SHIFT)) & I2C_CR_MASTER_MASK)

/*!
 * @brief I2C_TAR Register Bit Definition
 */

#define I2C_TAR_SMBQC_SHIFT                     (16)
#define I2C_TAR_SMBQC_MASK                      (0x01U << I2C_TAR_SMBQC_SHIFT)
#define I2C_TAR_SMBQC(x)                        (((uint32_t)(((uint32_t)(x)) << I2C_TAR_SMBQC_SHIFT)) & I2C_TAR_SMBQC_MASK)

#define I2C_TAR_SPECIAL_SHIFT                   (11)
#define I2C_TAR_SPECIAL_MASK                    (0x01U << I2C_TAR_SPECIAL_SHIFT)
#define I2C_TAR_SPECIAL(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_TAR_SPECIAL_SHIFT)) & I2C_TAR_SPECIAL_MASK)

#define I2C_TAR_GC_SHIFT                        (10)
#define I2C_TAR_GC_MASK                         (0x01U << I2C_TAR_GC_SHIFT)
#define I2C_TAR_GC(x)                           (((uint32_t)(((uint32_t)(x)) << I2C_TAR_GC_SHIFT)) & I2C_TAR_GC_MASK)

#define I2C_TAR_ADDR_SHIFT                      (0)
#define I2C_TAR_ADDR_MASK                       (0x3FFU << I2C_TAR_ADDR_SHIFT)
#define I2C_TAR_ADDR(x)                         (((uint32_t)(((uint32_t)(x)) << I2C_TAR_ADDR_SHIFT)) & I2C_TAR_ADDR_MASK)

/*!
 * @brief I2C_SAR Register Bit Definition
 */

#define I2C_SAR_ADDR_SHIFT                      (0)
#define I2C_SAR_ADDR_MASK                       (0x3FFU << I2C_SAR_ADDR_SHIFT)
#define I2C_SAR_ADDR(x)                         (((uint32_t)(((uint32_t)(x)) << I2C_SAR_ADDR_SHIFT)) & I2C_SAR_ADDR_MASK)

/*!
 * @brief I2C_DR Register Bit Definition
 */

#define I2C_DR_FIRSTDATA_SHIFT                  (11)
#define I2C_DR_FIRSTDATA_MASK                   (0x01U << I2C_DR_FIRSTDATA_SHIFT)
#define I2C_DR_FIRSTDATA(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_DR_FIRSTDATA_SHIFT)) & I2C_DR_FIRSTDATA_MASK)

#define I2C_DR_CMD_SHIFT                        (8)
#define I2C_DR_CMD_MASK                         (0x01U << I2C_DR_CMD_SHIFT)
#define I2C_DR_CMD(x)                           (((uint32_t)(((uint32_t)(x)) << I2C_DR_CMD_SHIFT)) & I2C_DR_CMD_MASK)

#define I2C_DR_DAT_SHIFT                        (0)
#define I2C_DR_DAT_MASK                         (0xFFU << I2C_DR_DAT_SHIFT)
#define I2C_DR_DAT(x)                           (((uint32_t)(((uint32_t)(x)) << I2C_DR_DAT_SHIFT)) & I2C_DR_DAT_MASK)

/*!
 * @brief I2C_SSHR Register Bit Definition
 */

#define I2C_SSHR_CNT_SHIFT                      (0)
#define I2C_SSHR_CNT_MASK                       (0xFFFFU << I2C_SSHR_CNT_SHIFT)
#define I2C_SSHR_CNT(x)                         (((uint32_t)(((uint32_t)(x)) << I2C_SSHR_CNT_SHIFT)) & I2C_SSHR_CNT_MASK)

/*!
 * @brief I2C_SSLR Register Bit Definition
 */

#define I2C_SSLR_CNT_SHIFT                      (0)
#define I2C_SSLR_CNT_MASK                       (0xFFFFU << I2C_SSLR_CNT_SHIFT)
#define I2C_SSLR_CNT(x)                         (((uint32_t)(((uint32_t)(x)) << I2C_SSLR_CNT_SHIFT)) & I2C_SSLR_CNT_MASK)

/*!
 * @brief I2C_FSHR Register Bit Definition
 */

#define I2C_FSHR_CNT_SHIFT                      (0)
#define I2C_FSHR_CNT_MASK                       (0xFFFFU << I2C_FSHR_CNT_SHIFT)
#define I2C_FSHR_CNT(x)                         (((uint32_t)(((uint32_t)(x)) << I2C_FSHR_CNT_SHIFT)) & I2C_FSHR_CNT_MASK)

/*!
 * @brief I2C_FSLR Register Bit Definition
 */

#define I2C_FSLR_CNT_SHIFT                      (0)
#define I2C_FSLR_CNT_MASK                       (0xFFFFU << I2C_FSLR_CNT_SHIFT)
#define I2C_FSLR_CNT(x)                         (((uint32_t)(((uint32_t)(x)) << I2C_FSLR_CNT_SHIFT)) & I2C_FSLR_CNT_MASK)

/*!
 * @brief I2C_ISR Register Bit Definition
 */

#define I2C_ISR_RSCLSTUCKATLOW_SHIFT            (14)
#define I2C_ISR_RSCLSTUCKATLOW_MASK             (0x01U << I2C_ISR_RSCLSTUCKATLOW_SHIFT)
#define I2C_ISR_RSCLSTUCKATLOW(x)               (((uint32_t)(((uint32_t)(x)) << I2C_ISR_RSCLSTUCKATLOW_SHIFT)) & I2C_ISR_RSCLSTUCKATLOW_MASK)

#define I2C_ISR_RMSTONHOLD_SHIFT                (13)
#define I2C_ISR_RMSTONHOLD_MASK                 (0x01U << I2C_ISR_RMSTONHOLD_SHIFT)
#define I2C_ISR_RMSTONHOLD(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_ISR_RMSTONHOLD_SHIFT)) & I2C_ISR_RMSTONHOLD_MASK)

#define I2C_ISR_RGC_SHIFT                       (11)
#define I2C_ISR_RGC_MASK                        (0x01U << I2C_ISR_RGC_SHIFT)
#define I2C_ISR_RGC(x)                          (((uint32_t)(((uint32_t)(x)) << I2C_ISR_RGC_SHIFT)) & I2C_ISR_RGC_MASK)

#define I2C_ISR_RSTART_SHIFT                    (10)
#define I2C_ISR_RSTART_MASK                     (0x01U << I2C_ISR_RSTART_SHIFT)
#define I2C_ISR_RSTART(x)                       (((uint32_t)(((uint32_t)(x)) << I2C_ISR_RSTART_SHIFT)) & I2C_ISR_RSTART_MASK)

#define I2C_ISR_RSTOP_SHIFT                     (9)
#define I2C_ISR_RSTOP_MASK                      (0x01U << I2C_ISR_RSTOP_SHIFT)
#define I2C_ISR_RSTOP(x)                        (((uint32_t)(((uint32_t)(x)) << I2C_ISR_RSTOP_SHIFT)) & I2C_ISR_RSTOP_MASK)

#define I2C_ISR_RACTIV_SHIFT                    (8)
#define I2C_ISR_RACTIV_MASK                     (0x01U << I2C_ISR_RACTIV_SHIFT)
#define I2C_ISR_RACTIV(x)                       (((uint32_t)(((uint32_t)(x)) << I2C_ISR_RACTIV_SHIFT)) & I2C_ISR_RACTIV_MASK)

#define I2C_ISR_RRXDONE_SHIFT                   (7)
#define I2C_ISR_RRXDONE_MASK                    (0x01U << I2C_ISR_RRXDONE_SHIFT)
#define I2C_ISR_RRXDONE(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_ISR_RRXDONE_SHIFT)) & I2C_ISR_RRXDONE_MASK)

#define I2C_ISR_RTXABRT_SHIFT                   (6)
#define I2C_ISR_RTXABRT_MASK                    (0x01U << I2C_ISR_RTXABRT_SHIFT)
#define I2C_ISR_RTXABRT(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_ISR_RTXABRT_SHIFT)) & I2C_ISR_RTXABRT_MASK)

#define I2C_ISR_RRDREQ_SHIFT                    (5)
#define I2C_ISR_RRDREQ_MASK                     (0x01U << I2C_ISR_RRDREQ_SHIFT)
#define I2C_ISR_RRDREQ(x)                       (((uint32_t)(((uint32_t)(x)) << I2C_ISR_RRDREQ_SHIFT)) & I2C_ISR_RRDREQ_MASK)

#define I2C_ISR_RTXEMPTY_SHIFT                  (4)
#define I2C_ISR_RTXEMPTY_MASK                   (0x01U << I2C_ISR_RTXEMPTY_SHIFT)
#define I2C_ISR_RTXEMPTY(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_ISR_RTXEMPTY_SHIFT)) & I2C_ISR_RTXEMPTY_MASK)

#define I2C_ISR_RTXOVER_SHIFT                   (3)
#define I2C_ISR_RTXOVER_MASK                    (0x01U << I2C_ISR_RTXOVER_SHIFT)
#define I2C_ISR_RTXOVER(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_ISR_RTXOVER_SHIFT)) & I2C_ISR_RTXOVER_MASK)

#define I2C_ISR_RRXFULL_SHIFT                   (2)
#define I2C_ISR_RRXFULL_MASK                    (0x01U << I2C_ISR_RRXFULL_SHIFT)
#define I2C_ISR_RRXFULL(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_ISR_RRXFULL_SHIFT)) & I2C_ISR_RRXFULL_MASK)

#define I2C_ISR_RRXOVER_SHIFT                   (1)
#define I2C_ISR_RRXOVER_MASK                    (0x01U << I2C_ISR_RRXOVER_SHIFT)
#define I2C_ISR_RRXOVER(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_ISR_RRXOVER_SHIFT)) & I2C_ISR_RRXOVER_MASK)

#define I2C_ISR_RRXUNDER_SHIFT                  (0)
#define I2C_ISR_RRXUNDER_MASK                   (0x01U << I2C_ISR_RRXUNDER_SHIFT)
#define I2C_ISR_RRXUNDER(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_ISR_RRXUNDER_SHIFT)) & I2C_ISR_RRXUNDER_MASK)

/*!
 * @brief I2C_IMR Register Bit Definition
 */

#define I2C_IMR_MSCLSTUCK_SHIFT                 (14)
#define I2C_IMR_MSCLSTUCK_MASK                  (0x01U << I2C_IMR_MSCLSTUCK_SHIFT)
#define I2C_IMR_MSCLSTUCK(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_IMR_MSCLSTUCK_SHIFT)) & I2C_IMR_MSCLSTUCK_MASK)

#define I2C_IMR_MMSTONHOLD_SHIFT                (13)
#define I2C_IMR_MMSTONHOLD_MASK                 (0x01U << I2C_IMR_MMSTONHOLD_SHIFT)
#define I2C_IMR_MMSTONHOLD(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_IMR_MMSTONHOLD_SHIFT)) & I2C_IMR_MMSTONHOLD_MASK)

#define I2C_IMR_MGC_SHIFT                       (11)
#define I2C_IMR_MGC_MASK                        (0x01U << I2C_IMR_MGC_SHIFT)
#define I2C_IMR_MGC(x)                          (((uint32_t)(((uint32_t)(x)) << I2C_IMR_MGC_SHIFT)) & I2C_IMR_MGC_MASK)

#define I2C_IMR_MSTART_SHIFT                    (10)
#define I2C_IMR_MSTART_MASK                     (0x01U << I2C_IMR_MSTART_SHIFT)
#define I2C_IMR_MSTART(x)                       (((uint32_t)(((uint32_t)(x)) << I2C_IMR_MSTART_SHIFT)) & I2C_IMR_MSTART_MASK)

#define I2C_IMR_MSTOP_SHIFT                     (9)
#define I2C_IMR_MSTOP_MASK                      (0x01U << I2C_IMR_MSTOP_SHIFT)
#define I2C_IMR_MSTOP(x)                        (((uint32_t)(((uint32_t)(x)) << I2C_IMR_MSTOP_SHIFT)) & I2C_IMR_MSTOP_MASK)

#define I2C_IMR_MACTIV_SHIFT                    (8)
#define I2C_IMR_MACTIV_MASK                     (0x01U << I2C_IMR_MACTIV_SHIFT)
#define I2C_IMR_MACTIV(x)                       (((uint32_t)(((uint32_t)(x)) << I2C_IMR_MACTIV_SHIFT)) & I2C_IMR_MACTIV_MASK)

#define I2C_IMR_MRXDONE_SHIFT                   (7)
#define I2C_IMR_MRXDONE_MASK                    (0x01U << I2C_IMR_MRXDONE_SHIFT)
#define I2C_IMR_MRXDONE(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_IMR_MRXDONE_SHIFT)) & I2C_IMR_MRXDONE_MASK)

#define I2C_IMR_MTXABRT_SHIFT                   (6)
#define I2C_IMR_MTXABRT_MASK                    (0x01U << I2C_IMR_MTXABRT_SHIFT)
#define I2C_IMR_MTXABRT(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_IMR_MTXABRT_SHIFT)) & I2C_IMR_MTXABRT_MASK)

#define I2C_IMR_MRDREQ_SHIFT                    (5)
#define I2C_IMR_MRDREQ_MASK                     (0x01U << I2C_IMR_MRDREQ_SHIFT)
#define I2C_IMR_MRDREQ(x)                       (((uint32_t)(((uint32_t)(x)) << I2C_IMR_MRDREQ_SHIFT)) & I2C_IMR_MRDREQ_MASK)

#define I2C_IMR_MTXEMPTY_SHIFT                  (4)
#define I2C_IMR_MTXEMPTY_MASK                   (0x01U << I2C_IMR_MTXEMPTY_SHIFT)
#define I2C_IMR_MTXEMPTY(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_IMR_MTXEMPTY_SHIFT)) & I2C_IMR_MTXEMPTY_MASK)

#define I2C_IMR_MTXOVER_SHIFT                   (3)
#define I2C_IMR_MTXOVER_MASK                    (0x01U << I2C_IMR_MTXOVER_SHIFT)
#define I2C_IMR_MTXOVER(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_IMR_MTXOVER_SHIFT)) & I2C_IMR_MTXOVER_MASK)

#define I2C_IMR_MRXFULL_SHIFT                   (2)
#define I2C_IMR_MRXFULL_MASK                    (0x01U << I2C_IMR_MRXFULL_SHIFT)
#define I2C_IMR_MRXFULL(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_IMR_MRXFULL_SHIFT)) & I2C_IMR_MRXFULL_MASK)

#define I2C_IMR_MRXOVER_SHIFT                   (1)
#define I2C_IMR_MRXOVER_MASK                    (0x01U << I2C_IMR_MRXOVER_SHIFT)
#define I2C_IMR_MRXOVER(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_IMR_MRXOVER_SHIFT)) & I2C_IMR_MRXOVER_MASK)

#define I2C_IMR_MRXUNDER_SHIFT                  (0)
#define I2C_IMR_MRXUNDER_MASK                   (0x01U << I2C_IMR_MRXUNDER_SHIFT)
#define I2C_IMR_MRXUNDER(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_IMR_MRXUNDER_SHIFT)) & I2C_IMR_MRXUNDER_MASK)

/*!
 * @brief I2C_RAWISR Register Bit Definition
 */

#define I2C_RAWISR_SCLSTUCK_SHIFT               (14)
#define I2C_RAWISR_SCLSTUCK_MASK                (0x01U << I2C_RAWISR_SCLSTUCK_SHIFT)
#define I2C_RAWISR_SCLSTUCK(x)                  (((uint32_t)(((uint32_t)(x)) << I2C_RAWISR_SCLSTUCK_SHIFT)) & I2C_RAWISR_SCLSTUCK_MASK)

#define I2C_RAWISR_MSTONHOLD_SHIFT              (13)
#define I2C_RAWISR_MSTONHOLD_MASK               (0x01U << I2C_RAWISR_MSTONHOLD_SHIFT)
#define I2C_RAWISR_MSTONHOLD(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_RAWISR_MSTONHOLD_SHIFT)) & I2C_RAWISR_MSTONHOLD_MASK)

#define I2C_RAWISR_GC_SHIFT                     (11)
#define I2C_RAWISR_GC_MASK                      (0x01U << I2C_RAWISR_GC_SHIFT)
#define I2C_RAWISR_GC(x)                        (((uint32_t)(((uint32_t)(x)) << I2C_RAWISR_GC_SHIFT)) & I2C_RAWISR_GC_MASK)

#define I2C_RAWISR_START_SHIFT                  (10)
#define I2C_RAWISR_START_MASK                   (0x01U << I2C_RAWISR_START_SHIFT)
#define I2C_RAWISR_START(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_RAWISR_START_SHIFT)) & I2C_RAWISR_START_MASK)

#define I2C_RAWISR_STOP_SHIFT                   (9)
#define I2C_RAWISR_STOP_MASK                    (0x01U << I2C_RAWISR_STOP_SHIFT)
#define I2C_RAWISR_STOP(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_RAWISR_STOP_SHIFT)) & I2C_RAWISR_STOP_MASK)

#define I2C_RAWISR_ACTIV_SHIFT                  (8)
#define I2C_RAWISR_ACTIV_MASK                   (0x01U << I2C_RAWISR_ACTIV_SHIFT)
#define I2C_RAWISR_ACTIV(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_RAWISR_ACTIV_SHIFT)) & I2C_RAWISR_ACTIV_MASK)

#define I2C_RAWISR_RXDONE_SHIFT                 (7)
#define I2C_RAWISR_RXDONE_MASK                  (0x01U << I2C_RAWISR_RXDONE_SHIFT)
#define I2C_RAWISR_RXDONE(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_RAWISR_RXDONE_SHIFT)) & I2C_RAWISR_RXDONE_MASK)

#define I2C_RAWISR_TXABRT_SHIFT                 (6)
#define I2C_RAWISR_TXABRT_MASK                  (0x01U << I2C_RAWISR_TXABRT_SHIFT)
#define I2C_RAWISR_TXABRT(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_RAWISR_TXABRT_SHIFT)) & I2C_RAWISR_TXABRT_MASK)

#define I2C_RAWISR_RDREQ_SHIFT                  (5)
#define I2C_RAWISR_RDREQ_MASK                   (0x01U << I2C_RAWISR_RDREQ_SHIFT)
#define I2C_RAWISR_RDREQ(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_RAWISR_RDREQ_SHIFT)) & I2C_RAWISR_RDREQ_MASK)

#define I2C_RAWISR_TXEMPTY_SHIFT                (4)
#define I2C_RAWISR_TXEMPTY_MASK                 (0x01U << I2C_RAWISR_TXEMPTY_SHIFT)
#define I2C_RAWISR_TXEMPTY(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_RAWISR_TXEMPTY_SHIFT)) & I2C_RAWISR_TXEMPTY_MASK)

#define I2C_RAWISR_TXOVER_SHIFT                 (3)
#define I2C_RAWISR_TXOVER_MASK                  (0x01U << I2C_RAWISR_TXOVER_SHIFT)
#define I2C_RAWISR_TXOVER(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_RAWISR_TXOVER_SHIFT)) & I2C_RAWISR_TXOVER_MASK)

#define I2C_RAWISR_RXFULL_SHIFT                 (2)
#define I2C_RAWISR_RXFULL_MASK                  (0x01U << I2C_RAWISR_RXFULL_SHIFT)
#define I2C_RAWISR_RXFULL(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_RAWISR_RXFULL_SHIFT)) & I2C_RAWISR_RXFULL_MASK)

#define I2C_RAWISR_RXOVER_SHIFT                 (1)
#define I2C_RAWISR_RXOVER_MASK                  (0x01U << I2C_RAWISR_RXOVER_SHIFT)
#define I2C_RAWISR_RXOVER(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_RAWISR_RXOVER_SHIFT)) & I2C_RAWISR_RXOVER_MASK)

#define I2C_RAWISR_RXUNDER_SHIFT                (0)
#define I2C_RAWISR_RXUNDER_MASK                 (0x01U << I2C_RAWISR_RXUNDER_SHIFT)
#define I2C_RAWISR_RXUNDER(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_RAWISR_RXUNDER_SHIFT)) & I2C_RAWISR_RXUNDER_MASK)

/*!
 * @brief I2C_RXTLR Register Bit Definition
 */

#define I2C_RXTLR_TL_SHIFT                      (0)
#define I2C_RXTLR_TL_MASK                       (0x3U << I2C_RXTLR_TL_SHIFT)
#define I2C_RXTLR_TL(x)                         (((uint32_t)(((uint32_t)(x)) << I2C_RXTLR_TL_SHIFT)) & I2C_RXTLR_TL_MASK)

/*!
 * @brief I2C_TXTLR Register Bit Definition
 */

#define I2C_TXTLR_TL_SHIFT                      (0)
#define I2C_TXTLR_TL_MASK                       (0x3U << I2C_TXTLR_TL_SHIFT)
#define I2C_TXTLR_TL(x)                         (((uint32_t)(((uint32_t)(x)) << I2C_TXTLR_TL_SHIFT)) & I2C_TXTLR_TL_MASK)

/*!
 * @brief I2C_ICR Register Bit Definition
 */

#define I2C_ICR_ICR_SHIFT                       (0)
#define I2C_ICR_ICR_MASK                        (0x01U << I2C_ICR_ICR_SHIFT)
#define I2C_ICR_ICR(x)                          (((uint32_t)(((uint32_t)(x)) << I2C_ICR_ICR_SHIFT)) & I2C_ICR_ICR_MASK)

/*!
 * @brief I2C_RX_UNDER Register Bit Definition
 */

#define I2C_RXUNDER_RXUNDER_SHIFT               (0)
#define I2C_RXUNDER_RXUNDER_MASK                (0x01U << I2C_RXUNDER_RXUNDER_SHIFT)
#define I2C_RXUNDER_RXUNDER(x)                  (((uint32_t)(((uint32_t)(x)) << I2C_RXUNDER_RXUNDER_SHIFT)) & I2C_RXUNDER_RXUNDER_MASK)

/*!
 * @brief I2C_RX_OVER Register Bit Definition
 */

#define I2C_RXOVER_RXOVER_SHIFT                 (0)
#define I2C_RXOVER_RXOVER_MASK                  (0x01U << I2C_RXOVER_RXOVER_SHIFT)
#define I2C_RXOVER_RXOVER(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_RXOVER_RXOVER_SHIFT)) & I2C_RXOVER_RXOVER_MASK)

/*!
 * @brief I2C_TX_OVER Register Bit Definition
 */

#define I2C_TXOVER_TXOVER_SHIFT                 (0)
#define I2C_TXOVER_TXOVER_MASK                  (0x01U << I2C_TXOVER_TXOVER_SHIFT)
#define I2C_TXOVER_TXOVER(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_TXOVER_TXOVER_SHIFT)) & I2C_TXOVER_TXOVER_MASK)

/*!
 * @brief I2C_RD_REQ Register Bit Definition
 */

#define I2C_RDREQ_RDREQ_SHIFT                   (0)
#define I2C_RDREQ_RDREQ_MASK                    (0x01U << I2C_RDREQ_RDREQ_SHIFT)
#define I2C_RDREQ_RDREQ(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_RDREQ_RDREQ_SHIFT)) & I2C_RDREQ_RDREQ_MASK)

/*!
 * @brief I2C_TX_ABRT Register Bit Definition
 */

#define I2C_TXABRT_TXABRT_SHIFT                 (0)
#define I2C_TXABRT_TXABRT_MASK                  (0x01U << I2C_TXABRT_TXABRT_SHIFT)
#define I2C_TXABRT_TXABRT(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_TXABRT_TXABRT_SHIFT)) & I2C_TXABRT_TXABRT_MASK)

/*!
 * @brief I2C_RX_DONE Register Bit Definition
 */

#define I2C_RXDONE_RXDONE_SHIFT                 (0)
#define I2C_RXDONE_RXDONE_MASK                  (0x01U << I2C_RXDONE_RXDONE_SHIFT)
#define I2C_RXDONE_RXDONE(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_RXDONE_RXDONE_SHIFT)) & I2C_RXDONE_RXDONE_MASK)

/*!
 * @brief I2C_ACTIV Register Bit Definition
 */

#define I2C_ACTIV_ACTIV_SHIFT                   (0)
#define I2C_ACTIV_ACTIV_MASK                    (0x01U << I2C_ACTIV_ACTIV_SHIFT)
#define I2C_ACTIV_ACTIV(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_ACTIV_ACTIV_SHIFT)) & I2C_ACTIV_ACTIV_MASK)

/*!
 * @brief I2C_STOP Register Bit Definition
 */

#define I2C_STOP_STOP_SHIFT                     (0)
#define I2C_STOP_STOP_MASK                      (0x01U << I2C_STOP_STOP_SHIFT)
#define I2C_STOP_STOP(x)                        (((uint32_t)(((uint32_t)(x)) << I2C_STOP_STOP_SHIFT)) & I2C_STOP_STOP_MASK)

/*!
 * @brief I2C_START Register Bit Definition
 */

#define I2C_START_START_SHIFT                   (0)
#define I2C_START_START_MASK                    (0x01U << I2C_START_START_SHIFT)
#define I2C_START_START(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_START_START_SHIFT)) & I2C_START_START_MASK)

/*!
 * @brief I2C_GC Register Bit Definition
 */

#define I2C_GC_GC_SHIFT                         (0)
#define I2C_GC_GC_MASK                          (0x01U << I2C_GC_GC_SHIFT)
#define I2C_GC_GC(x)                            (((uint32_t)(((uint32_t)(x)) << I2C_GC_GC_SHIFT)) & I2C_GC_GC_MASK)

/*!
 * @brief I2C_ENR Register Bit Definition
 */

#define I2C_ENR_SMBALTEN_SHIFT                  (18)
#define I2C_ENR_SMBALTEN_MASK                   (0x01U << I2C_ENR_SMBALTEN_SHIFT)
#define I2C_ENR_SMBALTEN(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_ENR_SMBALTEN_SHIFT)) & I2C_ENR_SMBALTEN_MASK)

#define I2C_ENR_SMBCLKRST_SHIFT                 (16)
#define I2C_ENR_SMBCLKRST_MASK                  (0x01U << I2C_ENR_SMBCLKRST_SHIFT)
#define I2C_ENR_SMBCLKRST(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_ENR_SMBCLKRST_SHIFT)) & I2C_ENR_SMBCLKRST_MASK)

#define I2C_ENR_SDARCVEN_SHIFT                  (3)
#define I2C_ENR_SDARCVEN_MASK                   (0x01U << I2C_ENR_SDARCVEN_SHIFT)
#define I2C_ENR_SDARCVEN(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_ENR_SDARCVEN_SHIFT)) & I2C_ENR_SDARCVEN_MASK)

#define I2C_ENR_TXCMDBLOCK_SHIFT                (2)
#define I2C_ENR_TXCMDBLOCK_MASK                 (0x01U << I2C_ENR_TXCMDBLOCK_SHIFT)
#define I2C_ENR_TXCMDBLOCK(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_ENR_TXCMDBLOCK_SHIFT)) & I2C_ENR_TXCMDBLOCK_MASK)

#define I2C_ENR_ABORT_SHIFT                     (1)
#define I2C_ENR_ABORT_MASK                      (0x01U << I2C_ENR_ABORT_SHIFT)
#define I2C_ENR_ABORT(x)                        (((uint32_t)(((uint32_t)(x)) << I2C_ENR_ABORT_SHIFT)) & I2C_ENR_ABORT_MASK)

#define I2C_ENR_ENABLE_SHIFT                    (0)
#define I2C_ENR_ENABLE_MASK                     (0x01U << I2C_ENR_ENABLE_SHIFT)
#define I2C_ENR_ENABLE(x)                       (((uint32_t)(((uint32_t)(x)) << I2C_ENR_ENABLE_SHIFT)) & I2C_ENR_ENABLE_MASK)

/*!
 * @brief I2C_SR Register Bit Definition
 */

#define I2C_SR_SMBALT_SHIFT                     (20)
#define I2C_SR_SMBALT_MASK                      (0x01U << I2C_SR_SMBALT_SHIFT)
#define I2C_SR_SMBALT(x)                        (((uint32_t)(((uint32_t)(x)) << I2C_SR_SMBALT_SHIFT)) & I2C_SR_SMBALT_MASK)

#define I2C_SR_SMBSLVAR_SHIFT                   (18)
#define I2C_SR_SMBSLVAR_MASK                    (0x01U << I2C_SR_SMBSLVAR_SHIFT)
#define I2C_SR_SMBSLVAR(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_SR_SMBSLVAR_SHIFT)) & I2C_SR_SMBSLVAR_MASK)

#define I2C_SR_SMBSLVAV_SHIFT                   (17)
#define I2C_SR_SMBSLVAV_MASK                    (0x01U << I2C_SR_SMBSLVAV_SHIFT)
#define I2C_SR_SMBSLVAV(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_SR_SMBSLVAV_SHIFT)) & I2C_SR_SMBSLVAV_MASK)

#define I2C_SR_SMBQC_SHIFT                      (16)
#define I2C_SR_SMBQC_MASK                       (0x01U << I2C_SR_SMBQC_SHIFT)
#define I2C_SR_SMBQC(x)                         (((uint32_t)(((uint32_t)(x)) << I2C_SR_SMBQC_SHIFT)) & I2C_SR_SMBQC_MASK)

#define I2C_SR_SDANOTRECOVERED_SHIFT            (11)
#define I2C_SR_SDANOTRECOVERED_MASK             (0x01U << I2C_SR_SDANOTRECOVERED_SHIFT)
#define I2C_SR_SDANOTRECOVERED(x)               (((uint32_t)(((uint32_t)(x)) << I2C_SR_SDANOTRECOVERED_SHIFT)) & I2C_SR_SDANOTRECOVERED_MASK)

#define I2C_SR_SLVHOLDRXFULL_SHIFT              (10)
#define I2C_SR_SLVHOLDRXFULL_MASK               (0x01U << I2C_SR_SLVHOLDRXFULL_SHIFT)
#define I2C_SR_SLVHOLDRXFULL(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_SR_SLVHOLDRXFULL_SHIFT)) & I2C_SR_SLVHOLDRXFULL_MASK)

#define I2C_SR_SLVHOLDTXEMPTY_SHIFT             (9)
#define I2C_SR_SLVHOLDTXEMPTY_MASK              (0x01U << I2C_SR_SLVHOLDTXEMPTY_SHIFT)
#define I2C_SR_SLVHOLDTXEMPTY(x)                (((uint32_t)(((uint32_t)(x)) << I2C_SR_SLVHOLDTXEMPTY_SHIFT)) & I2C_SR_SLVHOLDTXEMPTY_MASK)

#define I2C_SR_MSTHOLDRXFULL_SHIFT              (8)
#define I2C_SR_MSTHOLDRXFULL_MASK               (0x01U << I2C_SR_MSTHOLDRXFULL_SHIFT)
#define I2C_SR_MSTHOLDRXFULL(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_SR_MSTHOLDRXFULL_SHIFT)) & I2C_SR_MSTHOLDRXFULL_MASK)

#define I2C_SR_MSTHOLDTXEMPTY_SHIFT             (7)
#define I2C_SR_MSTHOLDTXEMPTY_MASK              (0x01U << I2C_SR_MSTHOLDTXEMPTY_SHIFT)
#define I2C_SR_MSTHOLDTXEMPTY(x)                (((uint32_t)(((uint32_t)(x)) << I2C_SR_MSTHOLDTXEMPTY_SHIFT)) & I2C_SR_MSTHOLDTXEMPTY_MASK)

#define I2C_SR_SLVACTIV_SHIFT                   (6)
#define I2C_SR_SLVACTIV_MASK                    (0x01U << I2C_SR_SLVACTIV_SHIFT)
#define I2C_SR_SLVACTIV(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_SR_SLVACTIV_SHIFT)) & I2C_SR_SLVACTIV_MASK)

#define I2C_SR_MSTACTIV_SHIFT                   (5)
#define I2C_SR_MSTACTIV_MASK                    (0x01U << I2C_SR_MSTACTIV_SHIFT)
#define I2C_SR_MSTACTIV(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_SR_MSTACTIV_SHIFT)) & I2C_SR_MSTACTIV_MASK)

#define I2C_SR_RFF_SHIFT                        (4)
#define I2C_SR_RFF_MASK                         (0x01U << I2C_SR_RFF_SHIFT)
#define I2C_SR_RFF(x)                           (((uint32_t)(((uint32_t)(x)) << I2C_SR_RFF_SHIFT)) & I2C_SR_RFF_MASK)

#define I2C_SR_RFNE_SHIFT                       (3)
#define I2C_SR_RFNE_MASK                        (0x01U << I2C_SR_RFNE_SHIFT)
#define I2C_SR_RFNE(x)                          (((uint32_t)(((uint32_t)(x)) << I2C_SR_RFNE_SHIFT)) & I2C_SR_RFNE_MASK)

#define I2C_SR_TFE_SHIFT                        (2)
#define I2C_SR_TFE_MASK                         (0x01U << I2C_SR_TFE_SHIFT)
#define I2C_SR_TFE(x)                           (((uint32_t)(((uint32_t)(x)) << I2C_SR_TFE_SHIFT)) & I2C_SR_TFE_MASK)

#define I2C_SR_TFNF_SHIFT                       (1)
#define I2C_SR_TFNF_MASK                        (0x01U << I2C_SR_TFNF_SHIFT)
#define I2C_SR_TFNF(x)                          (((uint32_t)(((uint32_t)(x)) << I2C_SR_TFNF_SHIFT)) & I2C_SR_TFNF_MASK)

#define I2C_SR_ACTIV_SHIFT                      (0)
#define I2C_SR_ACTIV_MASK                       (0x01U << I2C_SR_ACTIV_SHIFT)
#define I2C_SR_ACTIV(x)                         (((uint32_t)(((uint32_t)(x)) << I2C_SR_ACTIV_SHIFT)) & I2C_SR_ACTIV_MASK)

/*!
 * @brief I2C_TXFLR Register Bit Definition
 */

#define I2C_TXFLR_CNT_SHIFT                     (0)
#define I2C_TXFLR_CNT_MASK                      (0x7U << I2C_TXFLR_CNT_SHIFT)
#define I2C_TXFLR_CNT(x)                        (((uint32_t)(((uint32_t)(x)) << I2C_TXFLR_CNT_SHIFT)) & I2C_TXFLR_CNT_MASK)

/*!
 * @brief I2C_RXFLR Register Bit Definition
 */

#define I2C_RXFLR_CNT_SHIFT                     (0)
#define I2C_RXFLR_CNT_MASK                      (0x7U << I2C_RXFLR_CNT_SHIFT)
#define I2C_RXFLR_CNT(x)                        (((uint32_t)(((uint32_t)(x)) << I2C_RXFLR_CNT_SHIFT)) & I2C_RXFLR_CNT_MASK)

/*!
 * @brief I2C_HOLD Register Bit Definition
 */

#define I2C_HOLD_RXHOLD_SHIFT                   (16)
#define I2C_HOLD_RXHOLD_MASK                    (0xFFU << I2C_HOLD_RXHOLD_SHIFT)
#define I2C_HOLD_RXHOLD(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_HOLD_RXHOLD_SHIFT)) & I2C_HOLD_RXHOLD_MASK)

#define I2C_HOLD_TXHOLD_SHIFT                   (0)
#define I2C_HOLD_TXHOLD_MASK                    (0xFFFFU << I2C_HOLD_TXHOLD_SHIFT)
#define I2C_HOLD_TXHOLD(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_HOLD_TXHOLD_SHIFT)) & I2C_HOLD_TXHOLD_MASK)

/*!
 * @brief I2C_TX_ABRT_SRC Register Bit Definition
 */

#define I2C_TXABRTSRC_TXFLUSHCNT_SHIFT          (23)
#define I2C_TXABRTSRC_TXFLUSHCNT_MASK           (0x1FFU << I2C_TXABRTSRC_TXFLUSHCNT_SHIFT)
#define I2C_TXABRTSRC_TXFLUSHCNT(x)             (((uint32_t)(((uint32_t)(x)) << I2C_TXABRTSRC_TXFLUSHCNT_SHIFT)) & I2C_TXABRTSRC_TXFLUSHCNT_MASK)

#define I2C_TXABRTSRC_SDALOW_SHIFT              (17)
#define I2C_TXABRTSRC_SDALOW_MASK               (0x01U << I2C_TXABRTSRC_SDALOW_SHIFT)
#define I2C_TXABRTSRC_SDALOW(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_TXABRTSRC_SDALOW_SHIFT)) & I2C_TXABRTSRC_SDALOW_MASK)

#define I2C_TXABRTSRC_USERABRT_SHIFT            (16)
#define I2C_TXABRTSRC_USERABRT_MASK             (0x01U << I2C_TXABRTSRC_USERABRT_SHIFT)
#define I2C_TXABRTSRC_USERABRT(x)               (((uint32_t)(((uint32_t)(x)) << I2C_TXABRTSRC_USERABRT_SHIFT)) & I2C_TXABRTSRC_USERABRT_MASK)

#define I2C_TXABRTSRC_SLVRDINTX_SHIFT           (15)
#define I2C_TXABRTSRC_SLVRDINTX_MASK            (0x01U << I2C_TXABRTSRC_SLVRDINTX_SHIFT)
#define I2C_TXABRTSRC_SLVRDINTX(x)              (((uint32_t)(((uint32_t)(x)) << I2C_TXABRTSRC_SLVRDINTX_SHIFT)) & I2C_TXABRTSRC_SLVRDINTX_MASK)

#define I2C_TXABRTSRC_SLVARBLOST_SHIFT          (14)
#define I2C_TXABRTSRC_SLVARBLOST_MASK           (0x01U << I2C_TXABRTSRC_SLVARBLOST_SHIFT)
#define I2C_TXABRTSRC_SLVARBLOST(x)             (((uint32_t)(((uint32_t)(x)) << I2C_TXABRTSRC_SLVARBLOST_SHIFT)) & I2C_TXABRTSRC_SLVARBLOST_MASK)

#define I2C_TXABRTSRC_SLVFLUSHTXFIFO_SHIFT      (13)
#define I2C_TXABRTSRC_SLVFLUSHTXFIFO_MASK       (0x01U << I2C_TXABRTSRC_SLVFLUSHTXFIFO_SHIFT)
#define I2C_TXABRTSRC_SLVFLUSHTXFIFO(x)         (((uint32_t)(((uint32_t)(x)) << I2C_TXABRTSRC_SLVFLUSHTXFIFO_SHIFT)) & I2C_TXABRTSRC_SLVFLUSHTXFIFO_MASK)

#define I2C_TXABRTSRC_LOST_SHIFT                (12)
#define I2C_TXABRTSRC_LOST_MASK                 (0x01U << I2C_TXABRTSRC_LOST_SHIFT)
#define I2C_TXABRTSRC_LOST(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_TXABRTSRC_LOST_SHIFT)) & I2C_TXABRTSRC_LOST_MASK)

#define I2C_TXABRTSRC_MSTDIS_SHIFT              (11)
#define I2C_TXABRTSRC_MSTDIS_MASK               (0x01U << I2C_TXABRTSRC_MSTDIS_SHIFT)
#define I2C_TXABRTSRC_MSTDIS(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_TXABRTSRC_MSTDIS_SHIFT)) & I2C_TXABRTSRC_MSTDIS_MASK)

#define I2C_TXABRTSRC_10BRDNORSTRT_SHIFT        (10)
#define I2C_TXABRTSRC_10BRDNORSTRT_MASK         (0x01U << I2C_TXABRTSRC_10BRDNORSTRT_SHIFT)
#define I2C_TXABRTSRC_10BRDNORSTRT(x)           (((uint32_t)(((uint32_t)(x)) << I2C_TXABRTSRC_10BRDNORSTRT_SHIFT)) & I2C_TXABRTSRC_10BRDNORSTRT_MASK)

#define I2C_TXABRTSRC_SBYTENORSTRT_SHIFT        (9)
#define I2C_TXABRTSRC_SBYTENORSTRT_MASK         (0x01U << I2C_TXABRTSRC_SBYTENORSTRT_SHIFT)
#define I2C_TXABRTSRC_SBYTENORSTRT(x)           (((uint32_t)(((uint32_t)(x)) << I2C_TXABRTSRC_SBYTENORSTRT_SHIFT)) & I2C_TXABRTSRC_SBYTENORSTRT_MASK)

#define I2C_TXABRTSRC_SBYTEACKDET_SHIFT         (7)
#define I2C_TXABRTSRC_SBYTEACKDET_MASK          (0x01U << I2C_TXABRTSRC_SBYTEACKDET_SHIFT)
#define I2C_TXABRTSRC_SBYTEACKDET(x)            (((uint32_t)(((uint32_t)(x)) << I2C_TXABRTSRC_SBYTEACKDET_SHIFT)) & I2C_TXABRTSRC_SBYTEACKDET_MASK)

#define I2C_TXABRTSRC_GCREAD_SHIFT              (5)
#define I2C_TXABRTSRC_GCREAD_MASK               (0x01U << I2C_TXABRTSRC_GCREAD_SHIFT)
#define I2C_TXABRTSRC_GCREAD(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_TXABRTSRC_GCREAD_SHIFT)) & I2C_TXABRTSRC_GCREAD_MASK)

#define I2C_TXABRTSRC_GCNOACK_SHIFT             (4)
#define I2C_TXABRTSRC_GCNOACK_MASK              (0x01U << I2C_TXABRTSRC_GCNOACK_SHIFT)
#define I2C_TXABRTSRC_GCNOACK(x)                (((uint32_t)(((uint32_t)(x)) << I2C_TXABRTSRC_GCNOACK_SHIFT)) & I2C_TXABRTSRC_GCNOACK_MASK)

#define I2C_TXABRTSRC_TXDATANOACK_SHIFT         (3)
#define I2C_TXABRTSRC_TXDATANOACK_MASK          (0x01U << I2C_TXABRTSRC_TXDATANOACK_SHIFT)
#define I2C_TXABRTSRC_TXDATANOACK(x)            (((uint32_t)(((uint32_t)(x)) << I2C_TXABRTSRC_TXDATANOACK_SHIFT)) & I2C_TXABRTSRC_TXDATANOACK_MASK)

#define I2C_TXABRTSRC_10ADDR2NOACK_SHIFT        (2)
#define I2C_TXABRTSRC_10ADDR2NOACK_MASK         (0x01U << I2C_TXABRTSRC_10ADDR2NOACK_SHIFT)
#define I2C_TXABRTSRC_10ADDR2NOACK(x)           (((uint32_t)(((uint32_t)(x)) << I2C_TXABRTSRC_10ADDR2NOACK_SHIFT)) & I2C_TXABRTSRC_10ADDR2NOACK_MASK)

#define I2C_TXABRTSRC_10ADDR1NOACK_SHIFT        (1)
#define I2C_TXABRTSRC_10ADDR1NOACK_MASK         (0x01U << I2C_TXABRTSRC_10ADDR1NOACK_SHIFT)
#define I2C_TXABRTSRC_10ADDR1NOACK(x)           (((uint32_t)(((uint32_t)(x)) << I2C_TXABRTSRC_10ADDR1NOACK_SHIFT)) & I2C_TXABRTSRC_10ADDR1NOACK_MASK)

#define I2C_TXABRTSRC_7ADDRNOACK_SHIFT          (0)
#define I2C_TXABRTSRC_7ADDRNOACK_MASK           (0x01U << I2C_TXABRTSRC_7ADDRNOACK_SHIFT)
#define I2C_TXABRTSRC_7ADDRNOACK(x)             (((uint32_t)(((uint32_t)(x)) << I2C_TXABRTSRC_7ADDRNOACK_SHIFT)) & I2C_TXABRTSRC_7ADDRNOACK_MASK)

/*!
 * @brief I2C_SLV_NACK Register Bit Definition
 */

#define I2C_SLVNACK_NACK_SHIFT                  (0)
#define I2C_SLVNACK_NACK_MASK                   (0x01U << I2C_SLVNACK_NACK_SHIFT)
#define I2C_SLVNACK_NACK(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_SLVNACK_NACK_SHIFT)) & I2C_SLVNACK_NACK_MASK)

/*!
 * @brief I2C_DMA Register Bit Definition
 */

#define I2C_DMA_TXEN_SHIFT                      (1)
#define I2C_DMA_TXEN_MASK                       (0x01U << I2C_DMA_TXEN_SHIFT)
#define I2C_DMA_TXEN(x)                         (((uint32_t)(((uint32_t)(x)) << I2C_DMA_TXEN_SHIFT)) & I2C_DMA_TXEN_MASK)

#define I2C_DMA_RXEN_SHIFT                      (0)
#define I2C_DMA_RXEN_MASK                       (0x01U << I2C_DMA_RXEN_SHIFT)
#define I2C_DMA_RXEN(x)                         (((uint32_t)(((uint32_t)(x)) << I2C_DMA_RXEN_SHIFT)) & I2C_DMA_RXEN_MASK)

/*!
 * @brief I2C_SETUP Register Bit Definition
 */

#define I2C_SETUP_CNT_SHIFT                     (0)
#define I2C_SETUP_CNT_MASK                      (0xFFU << I2C_SETUP_CNT_SHIFT)
#define I2C_SETUP_CNT(x)                        (((uint32_t)(((uint32_t)(x)) << I2C_SETUP_CNT_SHIFT)) & I2C_SETUP_CNT_MASK)

/*!
 * @brief I2C_GCR Register Bit Definition
 */

#define I2C_GCR_GC_SHIFT                        (0)
#define I2C_GCR_GC_MASK                         (0x01U << I2C_GCR_GC_SHIFT)
#define I2C_GCR_GC(x)                           (((uint32_t)(((uint32_t)(x)) << I2C_GCR_GC_SHIFT)) & I2C_GCR_GC_MASK)

/*!
 * @brief I2C_EN_SR Register Bit Definition
 */

#define I2C_ENSR_SLVRXDATALOST_SHIFT            (2)
#define I2C_ENSR_SLVRXDATALOST_MASK             (0x01U << I2C_ENSR_SLVRXDATALOST_SHIFT)
#define I2C_ENSR_SLVRXDATALOST(x)               (((uint32_t)(((uint32_t)(x)) << I2C_ENSR_SLVRXDATALOST_SHIFT)) & I2C_ENSR_SLVRXDATALOST_MASK)

#define I2C_ENSR_SLVDISWHILEBUSY_SHIFT          (1)
#define I2C_ENSR_SLVDISWHILEBUSY_MASK           (0x01U << I2C_ENSR_SLVDISWHILEBUSY_SHIFT)
#define I2C_ENSR_SLVDISWHILEBUSY(x)             (((uint32_t)(((uint32_t)(x)) << I2C_ENSR_SLVDISWHILEBUSY_SHIFT)) & I2C_ENSR_SLVDISWHILEBUSY_MASK)

#define I2C_ENSR_ICEN_SHIFT                     (0)
#define I2C_ENSR_ICEN_MASK                      (0x01U << I2C_ENSR_ICEN_SHIFT)
#define I2C_ENSR_ICEN(x)                        (((uint32_t)(((uint32_t)(x)) << I2C_ENSR_ICEN_SHIFT)) & I2C_ENSR_ICEN_MASK)

/*!
 * @brief I2C_SPKLEN Register Bit Definition
 */

#define I2C_SPKLEN_SPKLEN_SHIFT                 (0)
#define I2C_SPKLEN_SPKLEN_MASK                  (0xFFU << I2C_SPKLEN_SPKLEN_SHIFT)
#define I2C_SPKLEN_SPKLEN(x)                    (((uint32_t)(((uint32_t)(x)) << I2C_SPKLEN_SPKLEN_SHIFT)) & I2C_SPKLEN_SPKLEN_MASK)

/*!
 * @brief I2C_SCL_TMO Register Bit Definition
 */

#define I2C_SCLTMO_SCLTIMEOUT_SHIFT             (0)
#define I2C_SCLTMO_SCLTIMEOUT_MASK              (0xFFFFFFFFU << I2C_SCLTMO_SCLTIMEOUT_SHIFT)
#define I2C_SCLTMO_SCLTIMEOUT(x)                (((uint32_t)(((uint32_t)(x)) << I2C_SCLTMO_SCLTIMEOUT_SHIFT)) & I2C_SCLTMO_SCLTIMEOUT_MASK)

/*!
 * @brief I2C_SDA_TMO Register Bit Definition
 */

#define I2C_SDATMO_SDATIMEOUT_SHIFT             (0)
#define I2C_SDATMO_SDATIMEOUT_MASK              (0xFFFFFFFFU << I2C_SDATMO_SDATIMEOUT_SHIFT)
#define I2C_SDATMO_SDATIMEOUT(x)                (((uint32_t)(((uint32_t)(x)) << I2C_SDATMO_SDATIMEOUT_SHIFT)) & I2C_SDATMO_SDATIMEOUT_MASK)

/*!
 * @brief I2C_SCL_STUCK Register Bit Definition
 */

#define I2C_SCLSTUCK_SCLSTUCK_SHIFT             (0)
#define I2C_SCLSTUCK_SCLSTUCK_MASK              (0x01U << I2C_SCLSTUCK_SCLSTUCK_SHIFT)
#define I2C_SCLSTUCK_SCLSTUCK(x)                (((uint32_t)(((uint32_t)(x)) << I2C_SCLSTUCK_SCLSTUCK_SHIFT)) & I2C_SCLSTUCK_SCLSTUCK_MASK)

/*!
 * @brief I2C_SMB_SEXT Register Bit Definition
 */

#define I2C_SMBSEXT_SMBSEXT_SHIFT               (0)
#define I2C_SMBSEXT_SMBSEXT_MASK                (0xFFFFFFFFU << I2C_SMBSEXT_SMBSEXT_SHIFT)
#define I2C_SMBSEXT_SMBSEXT(x)                  (((uint32_t)(((uint32_t)(x)) << I2C_SMBSEXT_SMBSEXT_SHIFT)) & I2C_SMBSEXT_SMBSEXT_MASK)

/*!
 * @brief I2C_SMB_MEXT Register Bit Definition
 */

#define I2C_SMBMEXT_SMBMEXT_SHIFT               (0)
#define I2C_SMBMEXT_SMBMEXT_MASK                (0xFFFFFFFFU << I2C_SMBMEXT_SMBMEXT_SHIFT)
#define I2C_SMBMEXT_SMBMEXT(x)                  (((uint32_t)(((uint32_t)(x)) << I2C_SMBMEXT_SMBMEXT_SHIFT)) & I2C_SMBMEXT_SMBMEXT_MASK)

/*!
 * @brief I2C_SMB_IDLE Register Bit Definition
 */

#define I2C_SMBIDLE_SMBIDLECNT_SHIFT            (0)
#define I2C_SMBIDLE_SMBIDLECNT_MASK             (0xFFFFU << I2C_SMBIDLE_SMBIDLECNT_SHIFT)
#define I2C_SMBIDLE_SMBIDLECNT(x)               (((uint32_t)(((uint32_t)(x)) << I2C_SMBIDLE_SMBIDLECNT_SHIFT)) & I2C_SMBIDLE_SMBIDLECNT_MASK)

/*!
 * @brief I2C_SMB_ISR Register Bit Definition
 */

#define I2C_SMBISR_RSMBALT_SHIFT                (10)
#define I2C_SMBISR_RSMBALT_MASK                 (0x01U << I2C_SMBISR_RSMBALT_SHIFT)
#define I2C_SMBISR_RSMBALT(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_SMBISR_RSMBALT_SHIFT)) & I2C_SMBISR_RSMBALT_MASK)

#define I2C_SMBISR_RPECNACK_SHIFT               (8)
#define I2C_SMBISR_RPECNACK_MASK                (0x01U << I2C_SMBISR_RPECNACK_SHIFT)
#define I2C_SMBISR_RPECNACK(x)                  (((uint32_t)(((uint32_t)(x)) << I2C_SMBISR_RPECNACK_SHIFT)) & I2C_SMBISR_RPECNACK_MASK)

#define I2C_SMBISR_RARPASSGN_SHIFT              (7)
#define I2C_SMBISR_RARPASSGN_MASK               (0x01U << I2C_SMBISR_RARPASSGN_SHIFT)
#define I2C_SMBISR_RARPASSGN(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_SMBISR_RARPASSGN_SHIFT)) & I2C_SMBISR_RARPASSGN_MASK)

#define I2C_SMBISR_RARPUDID_SHIFT               (6)
#define I2C_SMBISR_RARPUDID_MASK                (0x01U << I2C_SMBISR_RARPUDID_SHIFT)
#define I2C_SMBISR_RARPUDID(x)                  (((uint32_t)(((uint32_t)(x)) << I2C_SMBISR_RARPUDID_SHIFT)) & I2C_SMBISR_RARPUDID_MASK)

#define I2C_SMBISR_RARPRST_SHIFT                (5)
#define I2C_SMBISR_RARPRST_MASK                 (0x01U << I2C_SMBISR_RARPRST_SHIFT)
#define I2C_SMBISR_RARPRST(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_SMBISR_RARPRST_SHIFT)) & I2C_SMBISR_RARPRST_MASK)

#define I2C_SMBISR_RARPPRE_SHIFT                (4)
#define I2C_SMBISR_RARPPRE_MASK                 (0x01U << I2C_SMBISR_RARPPRE_SHIFT)
#define I2C_SMBISR_RARPPRE(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_SMBISR_RARPPRE_SHIFT)) & I2C_SMBISR_RARPPRE_MASK)

#define I2C_SMBISR_RARPNOTIFY_SHIFT             (3)
#define I2C_SMBISR_RARPNOTIFY_MASK              (0x01U << I2C_SMBISR_RARPNOTIFY_SHIFT)
#define I2C_SMBISR_RARPNOTIFY(x)                (((uint32_t)(((uint32_t)(x)) << I2C_SMBISR_RARPNOTIFY_SHIFT)) & I2C_SMBISR_RARPNOTIFY_MASK)

#define I2C_SMBISR_RARPQUICK_SHIFT              (2)
#define I2C_SMBISR_RARPQUICK_MASK               (0x01U << I2C_SMBISR_RARPQUICK_SHIFT)
#define I2C_SMBISR_RARPQUICK(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_SMBISR_RARPQUICK_SHIFT)) & I2C_SMBISR_RARPQUICK_MASK)

#define I2C_SMBISR_RMSTTMO_SHIFT                (1)
#define I2C_SMBISR_RMSTTMO_MASK                 (0x01U << I2C_SMBISR_RMSTTMO_SHIFT)
#define I2C_SMBISR_RMSTTMO(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_SMBISR_RMSTTMO_SHIFT)) & I2C_SMBISR_RMSTTMO_MASK)

#define I2C_SMBISR_RSLVTMO_SHIFT                (0)
#define I2C_SMBISR_RSLVTMO_MASK                 (0x01U << I2C_SMBISR_RSLVTMO_SHIFT)
#define I2C_SMBISR_RSLVTMO(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_SMBISR_RSLVTMO_SHIFT)) & I2C_SMBISR_RSLVTMO_MASK)

/*!
 * @brief I2C_SMB_IMR Register Bit Definition
 */

#define I2C_SMBIMR_MSMBALT_SHIFT                (10)
#define I2C_SMBIMR_MSMBALT_MASK                 (0x01U << I2C_SMBIMR_MSMBALT_SHIFT)
#define I2C_SMBIMR_MSMBALT(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_SMBIMR_MSMBALT_SHIFT)) & I2C_SMBIMR_MSMBALT_MASK)

#define I2C_SMBIMR_MPECNACK_SHIFT               (8)
#define I2C_SMBIMR_MPECNACK_MASK                (0x01U << I2C_SMBIMR_MPECNACK_SHIFT)
#define I2C_SMBIMR_MPECNACK(x)                  (((uint32_t)(((uint32_t)(x)) << I2C_SMBIMR_MPECNACK_SHIFT)) & I2C_SMBIMR_MPECNACK_MASK)

#define I2C_SMBIMR_MARPASSGN_SHIFT              (7)
#define I2C_SMBIMR_MARPASSGN_MASK               (0x01U << I2C_SMBIMR_MARPASSGN_SHIFT)
#define I2C_SMBIMR_MARPASSGN(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_SMBIMR_MARPASSGN_SHIFT)) & I2C_SMBIMR_MARPASSGN_MASK)

#define I2C_SMBIMR_MARPUDID_SHIFT               (6)
#define I2C_SMBIMR_MARPUDID_MASK                (0x01U << I2C_SMBIMR_MARPUDID_SHIFT)
#define I2C_SMBIMR_MARPUDID(x)                  (((uint32_t)(((uint32_t)(x)) << I2C_SMBIMR_MARPUDID_SHIFT)) & I2C_SMBIMR_MARPUDID_MASK)

#define I2C_SMBIMR_MARPRST_SHIFT                (5)
#define I2C_SMBIMR_MARPRST_MASK                 (0x01U << I2C_SMBIMR_MARPRST_SHIFT)
#define I2C_SMBIMR_MARPRST(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_SMBIMR_MARPRST_SHIFT)) & I2C_SMBIMR_MARPRST_MASK)

#define I2C_SMBIMR_MARPPRE_SHIFT                (4)
#define I2C_SMBIMR_MARPPRE_MASK                 (0x01U << I2C_SMBIMR_MARPPRE_SHIFT)
#define I2C_SMBIMR_MARPPRE(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_SMBIMR_MARPPRE_SHIFT)) & I2C_SMBIMR_MARPPRE_MASK)

#define I2C_SMBIMR_MARPNOTIFY_SHIFT             (3)
#define I2C_SMBIMR_MARPNOTIFY_MASK              (0x01U << I2C_SMBIMR_MARPNOTIFY_SHIFT)
#define I2C_SMBIMR_MARPNOTIFY(x)                (((uint32_t)(((uint32_t)(x)) << I2C_SMBIMR_MARPNOTIFY_SHIFT)) & I2C_SMBIMR_MARPNOTIFY_MASK)

#define I2C_SMBIMR_MARPQUICK_SHIFT              (2)
#define I2C_SMBIMR_MARPQUICK_MASK               (0x01U << I2C_SMBIMR_MARPQUICK_SHIFT)
#define I2C_SMBIMR_MARPQUICK(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_SMBIMR_MARPQUICK_SHIFT)) & I2C_SMBIMR_MARPQUICK_MASK)

#define I2C_SMBIMR_MMSTTMO_SHIFT                (1)
#define I2C_SMBIMR_MMSTTMO_MASK                 (0x01U << I2C_SMBIMR_MMSTTMO_SHIFT)
#define I2C_SMBIMR_MMSTTMO(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_SMBIMR_MMSTTMO_SHIFT)) & I2C_SMBIMR_MMSTTMO_MASK)

#define I2C_SMBIMR_MSLVTMO_SHIFT                (0)
#define I2C_SMBIMR_MSLVTMO_MASK                 (0x01U << I2C_SMBIMR_MSLVTMO_SHIFT)
#define I2C_SMBIMR_MSLVTMO(x)                   (((uint32_t)(((uint32_t)(x)) << I2C_SMBIMR_MSLVTMO_SHIFT)) & I2C_SMBIMR_MSLVTMO_MASK)

/*!
 * @brief I2C_SMB_RAWISR Register Bit Definition
 */

#define I2C_SMBRAWISR_SMBALT_SHIFT              (10)
#define I2C_SMBRAWISR_SMBALT_MASK               (0x01U << I2C_SMBRAWISR_SMBALT_SHIFT)
#define I2C_SMBRAWISR_SMBALT(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_SMBRAWISR_SMBALT_SHIFT)) & I2C_SMBRAWISR_SMBALT_MASK)

#define I2C_SMBRAWISR_PECNACK_SHIFT             (8)
#define I2C_SMBRAWISR_PECNACK_MASK              (0x01U << I2C_SMBRAWISR_PECNACK_SHIFT)
#define I2C_SMBRAWISR_PECNACK(x)                (((uint32_t)(((uint32_t)(x)) << I2C_SMBRAWISR_PECNACK_SHIFT)) & I2C_SMBRAWISR_PECNACK_MASK)

#define I2C_SMBRAWISR_ARPASSGN_SHIFT            (7)
#define I2C_SMBRAWISR_ARPASSGN_MASK             (0x01U << I2C_SMBRAWISR_ARPASSGN_SHIFT)
#define I2C_SMBRAWISR_ARPASSGN(x)               (((uint32_t)(((uint32_t)(x)) << I2C_SMBRAWISR_ARPASSGN_SHIFT)) & I2C_SMBRAWISR_ARPASSGN_MASK)

#define I2C_SMBRAWISR_ARPUDID_SHIFT             (6)
#define I2C_SMBRAWISR_ARPUDID_MASK              (0x01U << I2C_SMBRAWISR_ARPUDID_SHIFT)
#define I2C_SMBRAWISR_ARPUDID(x)                (((uint32_t)(((uint32_t)(x)) << I2C_SMBRAWISR_ARPUDID_SHIFT)) & I2C_SMBRAWISR_ARPUDID_MASK)

#define I2C_SMBRAWISR_ARPRST_SHIFT              (5)
#define I2C_SMBRAWISR_ARPRST_MASK               (0x01U << I2C_SMBRAWISR_ARPRST_SHIFT)
#define I2C_SMBRAWISR_ARPRST(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_SMBRAWISR_ARPRST_SHIFT)) & I2C_SMBRAWISR_ARPRST_MASK)

#define I2C_SMBRAWISR_ARPPRE_SHIFT              (4)
#define I2C_SMBRAWISR_ARPPRE_MASK               (0x01U << I2C_SMBRAWISR_ARPPRE_SHIFT)
#define I2C_SMBRAWISR_ARPPRE(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_SMBRAWISR_ARPPRE_SHIFT)) & I2C_SMBRAWISR_ARPPRE_MASK)

#define I2C_SMBRAWISR_ARPNOTIFY_SHIFT           (3)
#define I2C_SMBRAWISR_ARPNOTIFY_MASK            (0x01U << I2C_SMBRAWISR_ARPNOTIFY_SHIFT)
#define I2C_SMBRAWISR_ARPNOTIFY(x)              (((uint32_t)(((uint32_t)(x)) << I2C_SMBRAWISR_ARPNOTIFY_SHIFT)) & I2C_SMBRAWISR_ARPNOTIFY_MASK)

#define I2C_SMBRAWISR_ARPQUICK_SHIFT            (2)
#define I2C_SMBRAWISR_ARPQUICK_MASK             (0x01U << I2C_SMBRAWISR_ARPQUICK_SHIFT)
#define I2C_SMBRAWISR_ARPQUICK(x)               (((uint32_t)(((uint32_t)(x)) << I2C_SMBRAWISR_ARPQUICK_SHIFT)) & I2C_SMBRAWISR_ARPQUICK_MASK)

#define I2C_SMBRAWISR_MSTTMO_SHIFT              (1)
#define I2C_SMBRAWISR_MSTTMO_MASK               (0x01U << I2C_SMBRAWISR_MSTTMO_SHIFT)
#define I2C_SMBRAWISR_MSTTMO(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_SMBRAWISR_MSTTMO_SHIFT)) & I2C_SMBRAWISR_MSTTMO_MASK)

#define I2C_SMBRAWISR_SLVTMO_SHIFT              (0)
#define I2C_SMBRAWISR_SLVTMO_MASK               (0x01U << I2C_SMBRAWISR_SLVTMO_SHIFT)
#define I2C_SMBRAWISR_SLVTMO(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_SMBRAWISR_SLVTMO_SHIFT)) & I2C_SMBRAWISR_SLVTMO_MASK)

/*!
 * @brief I2C_SMB_ICR Register Bit Definition
 */

#define I2C_SMBICR_CLRSMBALT_SHIFT              (10)
#define I2C_SMBICR_CLRSMBALT_MASK               (0x01U << I2C_SMBICR_CLRSMBALT_SHIFT)
#define I2C_SMBICR_CLRSMBALT(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_SMBICR_CLRSMBALT_SHIFT)) & I2C_SMBICR_CLRSMBALT_MASK)

#define I2C_SMBICR_CLRPECNACK_SHIFT             (8)
#define I2C_SMBICR_CLRPECNACK_MASK              (0x01U << I2C_SMBICR_CLRPECNACK_SHIFT)
#define I2C_SMBICR_CLRPECNACK(x)                (((uint32_t)(((uint32_t)(x)) << I2C_SMBICR_CLRPECNACK_SHIFT)) & I2C_SMBICR_CLRPECNACK_MASK)

#define I2C_SMBICR_CLRARPASSGN_SHIFT            (7)
#define I2C_SMBICR_CLRARPASSGN_MASK             (0x01U << I2C_SMBICR_CLRARPASSGN_SHIFT)
#define I2C_SMBICR_CLRARPASSGN(x)               (((uint32_t)(((uint32_t)(x)) << I2C_SMBICR_CLRARPASSGN_SHIFT)) & I2C_SMBICR_CLRARPASSGN_MASK)

#define I2C_SMBICR_CLRARPUDID_SHIFT             (6)
#define I2C_SMBICR_CLRARPUDID_MASK              (0x01U << I2C_SMBICR_CLRARPUDID_SHIFT)
#define I2C_SMBICR_CLRARPUDID(x)                (((uint32_t)(((uint32_t)(x)) << I2C_SMBICR_CLRARPUDID_SHIFT)) & I2C_SMBICR_CLRARPUDID_MASK)

#define I2C_SMBICR_CLRARPRST_SHIFT              (5)
#define I2C_SMBICR_CLRARPRST_MASK               (0x01U << I2C_SMBICR_CLRARPRST_SHIFT)
#define I2C_SMBICR_CLRARPRST(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_SMBICR_CLRARPRST_SHIFT)) & I2C_SMBICR_CLRARPRST_MASK)

#define I2C_SMBICR_CLRARPPRE_SHIFT              (4)
#define I2C_SMBICR_CLRARPPRE_MASK               (0x01U << I2C_SMBICR_CLRARPPRE_SHIFT)
#define I2C_SMBICR_CLRARPPRE(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_SMBICR_CLRARPPRE_SHIFT)) & I2C_SMBICR_CLRARPPRE_MASK)

#define I2C_SMBICR_CLRARPNOTIFY_SHIFT           (3)
#define I2C_SMBICR_CLRARPNOTIFY_MASK            (0x01U << I2C_SMBICR_CLRARPNOTIFY_SHIFT)
#define I2C_SMBICR_CLRARPNOTIFY(x)              (((uint32_t)(((uint32_t)(x)) << I2C_SMBICR_CLRARPNOTIFY_SHIFT)) & I2C_SMBICR_CLRARPNOTIFY_MASK)

#define I2C_SMBICR_CLRARPQUICK_SHIFT            (2)
#define I2C_SMBICR_CLRARPQUICK_MASK             (0x01U << I2C_SMBICR_CLRARPQUICK_SHIFT)
#define I2C_SMBICR_CLRARPQUICK(x)               (((uint32_t)(((uint32_t)(x)) << I2C_SMBICR_CLRARPQUICK_SHIFT)) & I2C_SMBICR_CLRARPQUICK_MASK)

#define I2C_SMBICR_CLRMSTTMO_SHIFT              (1)
#define I2C_SMBICR_CLRMSTTMO_MASK               (0x01U << I2C_SMBICR_CLRMSTTMO_SHIFT)
#define I2C_SMBICR_CLRMSTTMO(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_SMBICR_CLRMSTTMO_SHIFT)) & I2C_SMBICR_CLRMSTTMO_MASK)

#define I2C_SMBICR_CLRSLVTMO_SHIFT              (0)
#define I2C_SMBICR_CLRSLVTMO_MASK               (0x01U << I2C_SMBICR_CLRSLVTMO_SHIFT)
#define I2C_SMBICR_CLRSLVTMO(x)                 (((uint32_t)(((uint32_t)(x)) << I2C_SMBICR_CLRSLVTMO_SHIFT)) & I2C_SMBICR_CLRSLVTMO_MASK)

/*!
 * @brief I2C_OPT_SAR Register Bit Definition
 */

#define I2C_OPTSAR_ADDR_SHIFT                   (0)
#define I2C_OPTSAR_ADDR_MASK                    (0x7FU << I2C_OPTSAR_ADDR_SHIFT)
#define I2C_OPTSAR_ADDR(x)                      (((uint32_t)(((uint32_t)(x)) << I2C_OPTSAR_ADDR_SHIFT)) & I2C_OPTSAR_ADDR_MASK)

/*!
 * @brief I2C_SMB_UDID_LSB Register Bit Definition
 */

#define I2C_SMBUDIDLSB_SMBUDIDLSB_SHIFT         (0)
#define I2C_SMBUDIDLSB_SMBUDIDLSB_MASK          (0xFFFFFFFFU << I2C_SMBUDIDLSB_SMBUDIDLSB_SHIFT)
#define I2C_SMBUDIDLSB_SMBUDIDLSB(x)            (((uint32_t)(((uint32_t)(x)) << I2C_SMBUDIDLSB_SMBUDIDLSB_SHIFT)) & I2C_SMBUDIDLSB_SMBUDIDLSB_MASK)

/*!
 * @brief I2C_SMB_UDID_MSB0 Register Bit Definition
 */

#define I2C_SMBUDIDMSB0_SMBUDIDMSB0_SHIFT       (0)
#define I2C_SMBUDIDMSB0_SMBUDIDMSB0_MASK        (0xFFFFFFFFU << I2C_SMBUDIDMSB0_SMBUDIDMSB0_SHIFT)
#define I2C_SMBUDIDMSB0_SMBUDIDMSB0(x)          (((uint32_t)(((uint32_t)(x)) << I2C_SMBUDIDMSB0_SMBUDIDMSB0_SHIFT)) & I2C_SMBUDIDMSB0_SMBUDIDMSB0_MASK)

/*!
 * @brief I2C_SMB_UDID_MSB1 Register Bit Definition
 */

#define I2C_SMBUDIDMSB1_SMBUDIDMSB1_SHIFT       (0)
#define I2C_SMBUDIDMSB1_SMBUDIDMSB1_MASK        (0xFFFFFFFFU << I2C_SMBUDIDMSB1_SMBUDIDMSB1_SHIFT)
#define I2C_SMBUDIDMSB1_SMBUDIDMSB1(x)          (((uint32_t)(((uint32_t)(x)) << I2C_SMBUDIDMSB1_SMBUDIDMSB1_SHIFT)) & I2C_SMBUDIDMSB1_SMBUDIDMSB1_MASK)

/*!
 * @brief I2C_SMB_UDID_MSB2 Register Bit Definition
 */

#define I2C_SMBUDIDMSB2_SMBUDIDMSB2_SHIFT       (0)
#define I2C_SMBUDIDMSB2_SMBUDIDMSB2_MASK        (0xFFFFFFFFU << I2C_SMBUDIDMSB2_SMBUDIDMSB2_SHIFT)
#define I2C_SMBUDIDMSB2_SMBUDIDMSB2(x)          (((uint32_t)(((uint32_t)(x)) << I2C_SMBUDIDMSB2_SMBUDIDMSB2_SHIFT)) & I2C_SMBUDIDMSB2_SMBUDIDMSB2_MASK)

/*!
 * @brief I2C_SLVMASK Register Bit Definition
 */

#define I2C_SLVMASK_MASK_SHIFT                  (0)
#define I2C_SLVMASK_MASK_MASK                   (0x3FFU << I2C_SLVMASK_MASK_SHIFT)
#define I2C_SLVMASK_MASK(x)                     (((uint32_t)(((uint32_t)(x)) << I2C_SLVMASK_MASK_SHIFT)) & I2C_SLVMASK_MASK_MASK)

/*!
 * @brief I2C_SLVRCVADDR Register Bit Definition
 */

#define I2C_SLVRCVADDR_ADDR_SHIFT               (0)
#define I2C_SLVRCVADDR_ADDR_MASK                (0x3FFU << I2C_SLVRCVADDR_ADDR_SHIFT)
#define I2C_SLVRCVADDR_ADDR(x)                  (((uint32_t)(((uint32_t)(x)) << I2C_SLVRCVADDR_ADDR_SHIFT)) & I2C_SLVRCVADDR_ADDR_MASK)

/*!
 * @}
 */ /* end of group I2C_Register_Masks */
/******************************************************************************
 *I2C Instance
*******************************************************************************/

#define I2C1                  ((I2C_Type*)I2C1_BASE)
#define I2C2                  ((I2C_Type*)I2C2_BASE)

/*!
 * @}
 */ /* end of group I2C_Peripheral_Access_Layer */

/*!
 * @addtogroup IWDG_Peripheral_Access_Layer IWDG Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * IWDG Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t KR;                                                             ///< key register                                 offset: 0x00
    __IO uint32_t PR;                                                             ///< Prescaler register                           offset: 0x04
    __IO uint32_t RLR;                                                            ///< reload register                              offset: 0x08
    __IO uint32_t SR;                                                             ///< status register                              offset: 0x0C
    __IO uint32_t CR;                                                             ///< control register                             offset: 0x10
    __IO uint32_t IGEN;                                                           ///< Interrupt Generation Register                offset: 0x14
    __IO uint32_t CNT;                                                            ///< count register                               offset: 0x18
    __IO uint32_t PS;                                                             ///< Frequency division count register            offset: 0x1c
} IWDG_Type;

/*******************************************************************************
 * IWDG Type
 ******************************************************************************/

/*!
 * @addtogroup IWDG_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief IWDG_KR Register Bit Definition
 */

#define IWDG_KR_KEY_SHIFT                       (0)
#define IWDG_KR_KEY_MASK                        (0xFFFFU << IWDG_KR_KEY_SHIFT)
#define IWDG_KR_KEY(x)                          (((uint32_t)(((uint32_t)(x)) << IWDG_KR_KEY_SHIFT)) & IWDG_KR_KEY_MASK)

/*!
 * @brief IWDG_PR Register Bit Definition
 */

#define IWDG_PR_PR_SHIFT                        (0)
#define IWDG_PR_PR_MASK                         (0x7U << IWDG_PR_PR_SHIFT)
#define IWDG_PR_PR(x)                           (((uint32_t)(((uint32_t)(x)) << IWDG_PR_PR_SHIFT)) & IWDG_PR_PR_MASK)

/*!
 * @brief IWDG_RLR Register Bit Definition
 */

#define IWDG_RLR_RL_SHIFT                       (0)
#define IWDG_RLR_RL_MASK                        (0xFFFU << IWDG_RLR_RL_SHIFT)
#define IWDG_RLR_RL(x)                          (((uint32_t)(((uint32_t)(x)) << IWDG_RLR_RL_SHIFT)) & IWDG_RLR_RL_MASK)

/*!
 * @brief IWDG_SR Register Bit Definition
 */

#define IWDG_SR_UPDATE_SHIFT                    (3)
#define IWDG_SR_UPDATE_MASK                     (0x01U << IWDG_SR_UPDATE_SHIFT)
#define IWDG_SR_UPDATE(x)                       (((uint32_t)(((uint32_t)(x)) << IWDG_SR_UPDATE_SHIFT)) & IWDG_SR_UPDATE_MASK)

#define IWDG_SR_IVU_SHIFT                       (2)
#define IWDG_SR_IVU_MASK                        (0x01U << IWDG_SR_IVU_SHIFT)
#define IWDG_SR_IVU(x)                          (((uint32_t)(((uint32_t)(x)) << IWDG_SR_IVU_SHIFT)) & IWDG_SR_IVU_MASK)

#define IWDG_SR_RVU_SHIFT                       (1)
#define IWDG_SR_RVU_MASK                        (0x01U << IWDG_SR_RVU_SHIFT)
#define IWDG_SR_RVU(x)                          (((uint32_t)(((uint32_t)(x)) << IWDG_SR_RVU_SHIFT)) & IWDG_SR_RVU_MASK)

#define IWDG_SR_PVU_SHIFT                       (0)
#define IWDG_SR_PVU_MASK                        (0x01U << IWDG_SR_PVU_SHIFT)
#define IWDG_SR_PVU(x)                          (((uint32_t)(((uint32_t)(x)) << IWDG_SR_PVU_SHIFT)) & IWDG_SR_PVU_MASK)

/*!
 * @brief IWDG_CR Register Bit Definition
 */

#define IWDG_CR_IRQCLR_SHIFT                    (1)
#define IWDG_CR_IRQCLR_MASK                     (0x01U << IWDG_CR_IRQCLR_SHIFT)
#define IWDG_CR_IRQCLR(x)                       (((uint32_t)(((uint32_t)(x)) << IWDG_CR_IRQCLR_SHIFT)) & IWDG_CR_IRQCLR_MASK)

#define IWDG_CR_IRQSEL_SHIFT                    (0)
#define IWDG_CR_IRQSEL_MASK                     (0x01U << IWDG_CR_IRQSEL_SHIFT)
#define IWDG_CR_IRQSEL(x)                       (((uint32_t)(((uint32_t)(x)) << IWDG_CR_IRQSEL_SHIFT)) & IWDG_CR_IRQSEL_MASK)

/*!
 * @brief IWDG_IGEN Register Bit Definition
 */

#define IWDG_IGEN_IGEN_SHIFT                    (0)
#define IWDG_IGEN_IGEN_MASK                     (0xFFFU << IWDG_IGEN_IGEN_SHIFT)
#define IWDG_IGEN_IGEN(x)                       (((uint32_t)(((uint32_t)(x)) << IWDG_IGEN_IGEN_SHIFT)) & IWDG_IGEN_IGEN_MASK)

/*!
 * @brief IWDG_CNT Register Bit Definition
 */

#define IWDG_CNT_IWDGCNT_SHIFT                  (0)
#define IWDG_CNT_IWDGCNT_MASK                   (0xFFFU << IWDG_CNT_IWDGCNT_SHIFT)
#define IWDG_CNT_IWDGCNT(x)                     (((uint32_t)(((uint32_t)(x)) << IWDG_CNT_IWDGCNT_SHIFT)) & IWDG_CNT_IWDGCNT_MASK)

/*!
 * @brief IWDG_PS Register Bit Definition
 */

#define IWDG_PS_IWDGPS_SHIFT                    (0)
#define IWDG_PS_IWDGPS_MASK                     (0xFFU << IWDG_PS_IWDGPS_SHIFT)
#define IWDG_PS_IWDGPS(x)                       (((uint32_t)(((uint32_t)(x)) << IWDG_PS_IWDGPS_SHIFT)) & IWDG_PS_IWDGPS_MASK)

/*!
 * @}
 */ /* end of group IWDG_Register_Masks */
/******************************************************************************
 *IWDG Instance
*******************************************************************************/

#define IWDG                 ((IWDG_Type*)IWDG_BASE)

/*!
 * @}
 */ /* end of group IWDG_Peripheral_Access_Layer */




/*!
 * @addtogroup LPTIM_Peripheral_Access_Layer LPT Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * LPT Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t CFG;                                                            ///< LPTIM Configuration Register                 offset: 0x00
    __IO uint32_t IE;                                                             ///< LPTIM Interrupt Enable Register              offset: 0x04
    __IO uint32_t IF;                                                             ///< LPTIM Interrupt Flag Register                offset: 0x08
    __IO uint32_t CTRL;                                                           ///< LPTIM Control Register                       offset: 0x0C
    __IO uint32_t CNT;                                                            ///< LPTIM count register                         offset: 0x10
    __IO uint32_t CMP;                                                            ///< LPTIM Compare Value Register                 offset: 0x14
    __IO uint32_t TARGET;                                                         ///< LPTIM target value register                  offset: 0x18
} LPTIM_Type;

/*******************************************************************************
 * LPT Type
 ******************************************************************************/

/*!
 * @addtogroup LPTIM_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief LPTIM_CFG Register Bit Definition
 */

#define LPTIM_CFG_FLTEN_SHIFT                     (15)
#define LPTIM_CFG_FLTEN_MASK                      (0x01U << LPTIM_CFG_FLTEN_SHIFT)
#define LPTIM_CFG_FLTEN(x)                        (((uint32_t)(((uint32_t)(x)) << LPTIM_CFG_FLTEN_SHIFT)) & LPTIM_CFG_FLTEN_MASK)

#define LPTIM_CFG_DIVSEL_SHIFT                    (8)
#define LPTIM_CFG_DIVSEL_MASK                     (0x7U << LPTIM_CFG_DIVSEL_SHIFT)
#define LPTIM_CFG_DIVSEL(x)                       (((uint32_t)(((uint32_t)(x)) << LPTIM_CFG_DIVSEL_SHIFT)) & LPTIM_CFG_DIVSEL_MASK)

#define LPTIM_CFG_TRIGCFG_SHIFT                   (6)
#define LPTIM_CFG_TRIGCFG_MASK                    (0x3U << LPTIM_CFG_TRIGCFG_SHIFT)
#define LPTIM_CFG_TRIGCFG(x)                      (((uint32_t)(((uint32_t)(x)) << LPTIM_CFG_TRIGCFG_SHIFT)) & LPTIM_CFG_TRIGCFG_MASK)

#define LPTIM_CFG_TRIGSEL_SHIFT                   (5)
#define LPTIM_CFG_TRIGSEL_MASK                    (0x01U << LPTIM_CFG_TRIGSEL_SHIFT)
#define LPTIM_CFG_TRIGSEL(x)                      (((uint32_t)(((uint32_t)(x)) << LPTIM_CFG_TRIGSEL_SHIFT)) & LPTIM_CFG_TRIGSEL_MASK)

#define LPTIM_CFG_POLARITY_SHIFT                  (4)
#define LPTIM_CFG_POLARITY_MASK                   (0x01U << LPTIM_CFG_POLARITY_SHIFT)
#define LPTIM_CFG_POLARITY(x)                     (((uint32_t)(((uint32_t)(x)) << LPTIM_CFG_POLARITY_SHIFT)) & LPTIM_CFG_POLARITY_MASK)

#define LPTIM_CFG_PWM_SHIFT                       (3)
#define LPTIM_CFG_PWM_MASK                        (0x01U << LPTIM_CFG_PWM_SHIFT)
#define LPTIM_CFG_PWM(x)                          (((uint32_t)(((uint32_t)(x)) << LPTIM_CFG_PWM_SHIFT)) & LPTIM_CFG_PWM_MASK)

#define LPTIM_CFG_TMODE_SHIFT                     (1)
#define LPTIM_CFG_TMODE_MASK                      (0x3U << LPTIM_CFG_TMODE_SHIFT)
#define LPTIM_CFG_TMODE(x)                        (((uint32_t)(((uint32_t)(x)) << LPTIM_CFG_TMODE_SHIFT)) & LPTIM_CFG_TMODE_MASK)

#define LPTIM_CFG_MODE_SHIFT                      (0)
#define LPTIM_CFG_MODE_MASK                       (0x01U << LPTIM_CFG_MODE_SHIFT)
#define LPTIM_CFG_MODE(x)                         (((uint32_t)(((uint32_t)(x)) << LPTIM_CFG_MODE_SHIFT)) & LPTIM_CFG_MODE_MASK)

/*!
 * @brief LPTIM_IE Register Bit Definition
 */

#define LPTIM_IE_COMPIE_SHIFT                     (2)
#define LPTIM_IE_COMPIE_MASK                      (0x01U << LPTIM_IE_COMPIE_SHIFT)
#define LPTIM_IE_COMPIE(x)                        (((uint32_t)(((uint32_t)(x)) << LPTIM_IE_COMPIE_SHIFT)) & LPTIM_IE_COMPIE_MASK)

#define LPTIM_IE_TRIGIE_SHIFT                     (1)
#define LPTIM_IE_TRIGIE_MASK                      (0x01U << LPTIM_IE_TRIGIE_SHIFT)
#define LPTIM_IE_TRIGIE(x)                        (((uint32_t)(((uint32_t)(x)) << LPTIM_IE_TRIGIE_SHIFT)) & LPTIM_IE_TRIGIE_MASK)

#define LPTIM_IE_OVIE_SHIFT                       (0)
#define LPTIM_IE_OVIE_MASK                        (0x01U << LPTIM_IE_OVIE_SHIFT)
#define LPTIM_IE_OVIE(x)                          (((uint32_t)(((uint32_t)(x)) << LPTIM_IE_OVIE_SHIFT)) & LPTIM_IE_OVIE_MASK)

/*!
 * @brief LPTIM_IF Register Bit Definition
 */

#define LPTIM_IF_COMPIF_SHIFT                     (2)
#define LPTIM_IF_COMPIF_MASK                      (0x01U << LPTIM_IF_COMPIF_SHIFT)
#define LPTIM_IF_COMPIF(x)                        (((uint32_t)(((uint32_t)(x)) << LPTIM_IF_COMPIF_SHIFT)) & LPTIM_IF_COMPIF_MASK)

#define LPTIM_IF_TRIGIF_SHIFT                     (1)
#define LPTIM_IF_TRIGIF_MASK                      (0x01U << LPTIM_IF_TRIGIF_SHIFT)
#define LPTIM_IF_TRIGIF(x)                        (((uint32_t)(((uint32_t)(x)) << LPTIM_IF_TRIGIF_SHIFT)) & LPTIM_IF_TRIGIF_MASK)

#define LPTIM_IF_OVIF_SHIFT                       (0)
#define LPTIM_IF_OVIF_MASK                        (0x01U << LPTIM_IF_OVIF_SHIFT)
#define LPTIM_IF_OVIF(x)                          (((uint32_t)(((uint32_t)(x)) << LPTIM_IF_OVIF_SHIFT)) & LPTIM_IF_OVIF_MASK)

/*!
 * @brief LPTIM_CTRL Register Bit Definition
 */

#define LPTIM_CTRL_LPTEN_SHIFT                    (0)
#define LPTIM_CTRL_LPTEN_MASK                     (0x01U << LPTIM_CTRL_LPTEN_SHIFT)
#define LPTIM_CTRL_LPTEN(x)                       (((uint32_t)(((uint32_t)(x)) << LPTIM_CTRL_LPTEN_SHIFT)) & LPTIM_CTRL_LPTEN_MASK)

/*!
 * @brief LPTIM_CNT Register Bit Definition
 */

#define LPTIM_CNT_CNT_SHIFT                       (0)
#define LPTIM_CNT_CNT_MASK                        (0xFFFFU << LPTIM_CNT_CNT_SHIFT)
#define LPTIM_CNT_CNT(x)                          (((uint32_t)(((uint32_t)(x)) << LPTIM_CNT_CNT_SHIFT)) & LPTIM_CNT_CNT_MASK)

/*!
 * @brief LPTIM_CMP Register Bit Definition
 */

#define LPTIM_CMP_COMPAREREG_SHIFT                (0)
#define LPTIM_CMP_COMPAREREG_MASK                 (0xFFFFU << LPTIM_CMP_COMPAREREG_SHIFT)
#define LPTIM_CMP_COMPAREREG(x)                   (((uint32_t)(((uint32_t)(x)) << LPTIM_CMP_COMPAREREG_SHIFT)) & LPTIM_CMP_COMPAREREG_MASK)

/*!
 * @brief LPTIM_TARGET Register Bit Definition
 */

#define LPTIM_TARGET_TARGETREG_SHIFT              (0)
#define LPTIM_TARGET_TARGETREG_MASK               (0xFFFFU << LPTIM_TARGET_TARGETREG_SHIFT)
#define LPTIM_TARGET_TARGETREG(x)                 (((uint32_t)(((uint32_t)(x)) << LPTIM_TARGET_TARGETREG_SHIFT)) & LPTIM_TARGET_TARGETREG_MASK)

/*!
 * @}
 */ /* end of group LPTIM_Register_Masks */
/******************************************************************************
 *LPT Instance
*******************************************************************************/

#define LPTIM                  ((LPTIM_Type*)LPTIM_BASE)

/*!
 * @}
 */ /* end of group LPTIM_Peripheral_Access_Layer */




/*!
 * @addtogroup LPUART_Peripheral_Access_Layer LPU Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * LPU Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t LPUBAUD;                                                        ///< LPUART Baud Rate Register                    offset: 0x00
    __IO uint32_t MODU;                                                           ///< LPUART Baud Rate Modulation Control Register offset: 0x04
    __IO uint32_t LPUIF;                                                          ///< LPUART Interrupt Flag Register               offset: 0x08
    __IO uint32_t LPUSTA;                                                         ///< LPUART Status Register                       offset: 0x0C
    __IO uint32_t LPUCON;                                                         ///< LPUART Control Register                      offset: 0x10
    __IO uint32_t LPUEN;                                                          ///< LPUART transmit and receive enable register  offset: 0x14
    __IO uint32_t LPURXD;                                                         ///< LPUART receive data register                 offset: 0x18
    __IO uint32_t LPUTXD;                                                         ///< LPUART transmit data register                offset: 0x1C
    __IO uint32_t COMPARE;                                                        ///< LPUART data match register                   offset: 0x20
    __IO uint32_t WKCKE;                                                          ///< LPUART Wakeup Register                       offset: 0x24
} LPUART_Type;

/*******************************************************************************
 * LPU Type
 ******************************************************************************/

/*!
 * @addtogroup LPUART_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief LPUART_LPUBAUD Register Bit Definition
 */

#define LPUART_LPUBAUD_BR_SHIFT                    (16)
#define LPUART_LPUBAUD_BR_MASK                     (0xFFFFU << LPUART_LPUBAUD_BR_SHIFT)
#define LPUART_LPUBAUD_BR(x)                       (((uint32_t)(((uint32_t)(x)) << LPUART_LPUBAUD_BR_SHIFT)) & LPUART_LPUBAUD_BR_MASK)

#define LPUART_LPUBAUD_BREN_SHIFT                  (8)
#define LPUART_LPUBAUD_BREN_MASK                   (0x01U << LPUART_LPUBAUD_BREN_SHIFT)
#define LPUART_LPUBAUD_BREN(x)                     (((uint32_t)(((uint32_t)(x)) << LPUART_LPUBAUD_BREN_SHIFT)) & LPUART_LPUBAUD_BREN_MASK)

#define LPUART_LPUBAUD_BAUD_SHIFT                  (0)
#define LPUART_LPUBAUD_BAUD_MASK                   (0x7U << LPUART_LPUBAUD_BAUD_SHIFT)
#define LPUART_LPUBAUD_BAUD(x)                     (((uint32_t)(((uint32_t)(x)) << LPUART_LPUBAUD_BAUD_SHIFT)) & LPUART_LPUBAUD_BAUD_MASK)

/*!
 * @brief LPUART_MODU Register Bit Definition
 */

#define LPUART_MODU_MCTL_SHIFT                     (0)
#define LPUART_MODU_MCTL_MASK                      (0xFFFU << LPUART_MODU_MCTL_SHIFT)
#define LPUART_MODU_MCTL(x)                        (((uint32_t)(((uint32_t)(x)) << LPUART_MODU_MCTL_SHIFT)) & LPUART_MODU_MCTL_MASK)

/*!
 * @brief LPUART_LPUIF Register Bit Definition
 */

#define LPUART_LPUIF_TCIF_SHIFT                    (3)
#define LPUART_LPUIF_TCIF_MASK                     (0x01U << LPUART_LPUIF_TCIF_SHIFT)
#define LPUART_LPUIF_TCIF(x)                       (((uint32_t)(((uint32_t)(x)) << LPUART_LPUIF_TCIF_SHIFT)) & LPUART_LPUIF_TCIF_MASK)

#define LPUART_LPUIF_RXNEGIF_SHIFT                 (2)
#define LPUART_LPUIF_RXNEGIF_MASK                  (0x01U << LPUART_LPUIF_RXNEGIF_SHIFT)
#define LPUART_LPUIF_RXNEGIF(x)                    (((uint32_t)(((uint32_t)(x)) << LPUART_LPUIF_RXNEGIF_SHIFT)) & LPUART_LPUIF_RXNEGIF_MASK)

#define LPUART_LPUIF_TXIF_SHIFT                    (1)
#define LPUART_LPUIF_TXIF_MASK                     (0x01U << LPUART_LPUIF_TXIF_SHIFT)
#define LPUART_LPUIF_TXIF(x)                       (((uint32_t)(((uint32_t)(x)) << LPUART_LPUIF_TXIF_SHIFT)) & LPUART_LPUIF_TXIF_MASK)

#define LPUART_LPUIF_RXIF_SHIFT                    (0)
#define LPUART_LPUIF_RXIF_MASK                     (0x01U << LPUART_LPUIF_RXIF_SHIFT)
#define LPUART_LPUIF_RXIF(x)                       (((uint32_t)(((uint32_t)(x)) << LPUART_LPUIF_RXIF_SHIFT)) & LPUART_LPUIF_RXIF_MASK)

/*!
 * @brief LPUART_LPUSTA Register Bit Definition
 */

#define LPUART_LPUSTA_START_SHIFT                  (7)
#define LPUART_LPUSTA_START_MASK                   (0x01U << LPUART_LPUSTA_START_SHIFT)
#define LPUART_LPUSTA_START(x)                     (((uint32_t)(((uint32_t)(x)) << LPUART_LPUSTA_START_SHIFT)) & LPUART_LPUSTA_START_MASK)

#define LPUART_LPUSTA_PERR_SHIFT                   (6)
#define LPUART_LPUSTA_PERR_MASK                    (0x01U << LPUART_LPUSTA_PERR_SHIFT)
#define LPUART_LPUSTA_PERR(x)                      (((uint32_t)(((uint32_t)(x)) << LPUART_LPUSTA_PERR_SHIFT)) & LPUART_LPUSTA_PERR_MASK)

#define LPUART_LPUSTA_TC_SHIFT                     (5)
#define LPUART_LPUSTA_TC_MASK                      (0x01U << LPUART_LPUSTA_TC_SHIFT)
#define LPUART_LPUSTA_TC(x)                        (((uint32_t)(((uint32_t)(x)) << LPUART_LPUSTA_TC_SHIFT)) & LPUART_LPUSTA_TC_MASK)

#define LPUART_LPUSTA_TXE_SHIFT                    (4)
#define LPUART_LPUSTA_TXE_MASK                     (0x01U << LPUART_LPUSTA_TXE_SHIFT)
#define LPUART_LPUSTA_TXE(x)                       (((uint32_t)(((uint32_t)(x)) << LPUART_LPUSTA_TXE_SHIFT)) & LPUART_LPUSTA_TXE_MASK)

#define LPUART_LPUSTA_RXF_SHIFT                    (3)
#define LPUART_LPUSTA_RXF_MASK                     (0x01U << LPUART_LPUSTA_RXF_SHIFT)
#define LPUART_LPUSTA_RXF(x)                       (((uint32_t)(((uint32_t)(x)) << LPUART_LPUSTA_RXF_SHIFT)) & LPUART_LPUSTA_RXF_MASK)

#define LPUART_LPUSTA_MATCH_SHIFT                  (2)
#define LPUART_LPUSTA_MATCH_MASK                   (0x01U << LPUART_LPUSTA_MATCH_SHIFT)
#define LPUART_LPUSTA_MATCH(x)                     (((uint32_t)(((uint32_t)(x)) << LPUART_LPUSTA_MATCH_SHIFT)) & LPUART_LPUSTA_MATCH_MASK)

#define LPUART_LPUSTA_FERR_SHIFT                   (1)
#define LPUART_LPUSTA_FERR_MASK                    (0x01U << LPUART_LPUSTA_FERR_SHIFT)
#define LPUART_LPUSTA_FERR(x)                      (((uint32_t)(((uint32_t)(x)) << LPUART_LPUSTA_FERR_SHIFT)) & LPUART_LPUSTA_FERR_MASK)

#define LPUART_LPUSTA_RXOV_SHIFT                   (0)
#define LPUART_LPUSTA_RXOV_MASK                    (0x01U << LPUART_LPUSTA_RXOV_SHIFT)
#define LPUART_LPUSTA_RXOV(x)                      (((uint32_t)(((uint32_t)(x)) << LPUART_LPUSTA_RXOV_SHIFT)) & LPUART_LPUSTA_RXOV_MASK)

/*!
 * @brief LPUART_LPUCON Register Bit Definition
 */

#define LPUART_LPUCON_TXPOL_SHIFT                  (12)
#define LPUART_LPUCON_TXPOL_MASK                   (0x01U << LPUART_LPUCON_TXPOL_SHIFT)
#define LPUART_LPUCON_TXPOL(x)                     (((uint32_t)(((uint32_t)(x)) << LPUART_LPUCON_TXPOL_SHIFT)) & LPUART_LPUCON_TXPOL_MASK)

#define LPUART_LPUCON_RXPOL_SHIFT                  (11)
#define LPUART_LPUCON_RXPOL_MASK                   (0x01U << LPUART_LPUCON_RXPOL_SHIFT)
#define LPUART_LPUCON_RXPOL(x)                     (((uint32_t)(((uint32_t)(x)) << LPUART_LPUCON_RXPOL_SHIFT)) & LPUART_LPUCON_RXPOL_MASK)

#define LPUART_LPUCON_PAREN_SHIFT                  (10)
#define LPUART_LPUCON_PAREN_MASK                   (0x01U << LPUART_LPUCON_PAREN_SHIFT)
#define LPUART_LPUCON_PAREN(x)                     (((uint32_t)(((uint32_t)(x)) << LPUART_LPUCON_PAREN_SHIFT)) & LPUART_LPUCON_PAREN_MASK)

#define LPUART_LPUCON_PTYP_SHIFT                   (9)
#define LPUART_LPUCON_PTYP_MASK                    (0x01U << LPUART_LPUCON_PTYP_SHIFT)
#define LPUART_LPUCON_PTYP(x)                      (((uint32_t)(((uint32_t)(x)) << LPUART_LPUCON_PTYP_SHIFT)) & LPUART_LPUCON_PTYP_MASK)

#define LPUART_LPUCON_SL_SHIFT                     (8)
#define LPUART_LPUCON_SL_MASK                      (0x01U << LPUART_LPUCON_SL_SHIFT)
#define LPUART_LPUCON_SL(x)                        (((uint32_t)(((uint32_t)(x)) << LPUART_LPUCON_SL_SHIFT)) & LPUART_LPUCON_SL_MASK)

#define LPUART_LPUCON_DL_SHIFT                     (7)
#define LPUART_LPUCON_DL_MASK                      (0x01U << LPUART_LPUCON_DL_SHIFT)
#define LPUART_LPUCON_DL(x)                        (((uint32_t)(((uint32_t)(x)) << LPUART_LPUCON_DL_SHIFT)) & LPUART_LPUCON_DL_MASK)

#define LPUART_LPUCON_RXEV_SHIFT                   (5)
#define LPUART_LPUCON_RXEV_MASK                    (0x3U << LPUART_LPUCON_RXEV_SHIFT)
#define LPUART_LPUCON_RXEV(x)                      (((uint32_t)(((uint32_t)(x)) << LPUART_LPUCON_RXEV_SHIFT)) & LPUART_LPUCON_RXEV_MASK)

#define LPUART_LPUCON_ERRIE_SHIFT                  (4)
#define LPUART_LPUCON_ERRIE_MASK                   (0x01U << LPUART_LPUCON_ERRIE_SHIFT)
#define LPUART_LPUCON_ERRIE(x)                     (((uint32_t)(((uint32_t)(x)) << LPUART_LPUCON_ERRIE_SHIFT)) & LPUART_LPUCON_ERRIE_MASK)

#define LPUART_LPUCON_TCIE_SHIFT                   (3)
#define LPUART_LPUCON_TCIE_MASK                    (0x01U << LPUART_LPUCON_TCIE_SHIFT)
#define LPUART_LPUCON_TCIE(x)                      (((uint32_t)(((uint32_t)(x)) << LPUART_LPUCON_TCIE_SHIFT)) & LPUART_LPUCON_TCIE_MASK)

#define LPUART_LPUCON_TXIE_SHIFT                   (2)
#define LPUART_LPUCON_TXIE_MASK                    (0x01U << LPUART_LPUCON_TXIE_SHIFT)
#define LPUART_LPUCON_TXIE(x)                      (((uint32_t)(((uint32_t)(x)) << LPUART_LPUCON_TXIE_SHIFT)) & LPUART_LPUCON_TXIE_MASK)

#define LPUART_LPUCON_NEDET_SHIFT                  (1)
#define LPUART_LPUCON_NEDET_MASK                   (0x01U << LPUART_LPUCON_NEDET_SHIFT)
#define LPUART_LPUCON_NEDET(x)                     (((uint32_t)(((uint32_t)(x)) << LPUART_LPUCON_NEDET_SHIFT)) & LPUART_LPUCON_NEDET_MASK)

#define LPUART_LPUCON_RXIE_SHIFT                   (0)
#define LPUART_LPUCON_RXIE_MASK                    (0x01U << LPUART_LPUCON_RXIE_SHIFT)
#define LPUART_LPUCON_RXIE(x)                      (((uint32_t)(((uint32_t)(x)) << LPUART_LPUCON_RXIE_SHIFT)) & LPUART_LPUCON_RXIE_MASK)

/*!
 * @brief LPUART_LPUEN Register Bit Definition
 */

#define LPUART_LPUEN_DMAR_SHIFT                    (3)
#define LPUART_LPUEN_DMAR_MASK                     (0x01U << LPUART_LPUEN_DMAR_SHIFT)
#define LPUART_LPUEN_DMAR(x)                       (((uint32_t)(((uint32_t)(x)) << LPUART_LPUEN_DMAR_SHIFT)) & LPUART_LPUEN_DMAR_MASK)

#define LPUART_LPUEN_DMAT_SHIFT                    (2)
#define LPUART_LPUEN_DMAT_MASK                     (0x01U << LPUART_LPUEN_DMAT_SHIFT)
#define LPUART_LPUEN_DMAT(x)                       (((uint32_t)(((uint32_t)(x)) << LPUART_LPUEN_DMAT_SHIFT)) & LPUART_LPUEN_DMAT_MASK)

#define LPUART_LPUEN_RXEN_SHIFT                    (1)
#define LPUART_LPUEN_RXEN_MASK                     (0x01U << LPUART_LPUEN_RXEN_SHIFT)
#define LPUART_LPUEN_RXEN(x)                       (((uint32_t)(((uint32_t)(x)) << LPUART_LPUEN_RXEN_SHIFT)) & LPUART_LPUEN_RXEN_MASK)

#define LPUART_LPUEN_TXEN_SHIFT                    (0)
#define LPUART_LPUEN_TXEN_MASK                     (0x01U << LPUART_LPUEN_TXEN_SHIFT)
#define LPUART_LPUEN_TXEN(x)                       (((uint32_t)(((uint32_t)(x)) << LPUART_LPUEN_TXEN_SHIFT)) & LPUART_LPUEN_TXEN_MASK)

/*!
 * @brief LPUART_LPURXD Register Bit Definition
 */

#define LPUART_LPURXD_DATA_SHIFT                   (0)
#define LPUART_LPURXD_DATA_MASK                    (0xFFU << LPUART_LPURXD_DATA_SHIFT)
#define LPUART_LPURXD_DATA(x)                      (((uint32_t)(((uint32_t)(x)) << LPUART_LPURXD_DATA_SHIFT)) & LPUART_LPURXD_DATA_MASK)

/*!
 * @brief LPUART_LPUTXD Register Bit Definition
 */

#define LPUART_LPUTXD_DATA_SHIFT                   (0)
#define LPUART_LPUTXD_DATA_MASK                    (0xFFU << LPUART_LPUTXD_DATA_SHIFT)
#define LPUART_LPUTXD_DATA(x)                      (((uint32_t)(((uint32_t)(x)) << LPUART_LPUTXD_DATA_SHIFT)) & LPUART_LPUTXD_DATA_MASK)

/*!
 * @brief LPUART_COMPARE Register Bit Definition
 */

#define LPUART_COMPARE_COMPARE_SHIFT               (0)
#define LPUART_COMPARE_COMPARE_MASK                (0xFFU << LPUART_COMPARE_COMPARE_SHIFT)
#define LPUART_COMPARE_COMPARE(x)                  (((uint32_t)(((uint32_t)(x)) << LPUART_COMPARE_COMPARE_SHIFT)) & LPUART_COMPARE_COMPARE_MASK)

/*!
 * @brief LPUART_WKCKE Register Bit Definition
 */

#define LPUART_WKCKE_WKCKE_SHIFT                   (0)
#define LPUART_WKCKE_WKCKE_MASK                    (0x01U << LPUART_WKCKE_WKCKE_SHIFT)
#define LPUART_WKCKE_WKCKE(x)                      (((uint32_t)(((uint32_t)(x)) << LPUART_WKCKE_WKCKE_SHIFT)) & LPUART_WKCKE_WKCKE_MASK)

/*!
 * @}
 */ /* end of group LPUART_Register_Masks */
/******************************************************************************
 *LPU Instance
*******************************************************************************/

#define LPUART                  ((LPUART_Type*)LPUART_BASE)

/*!
 * @}
 */ /* end of group LPUART_Peripheral_Access_Layer */




/*!
 * @addtogroup MDS_Peripheral_Access_Layer MDS Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * MDS Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t TRIGXCR[8];                                                        ///< Trigger Channel x Control Register           offset: 0x000
    __IO uint32_t Reserved0[24];                                                     ///< Reserved
    __IO uint32_t TRIGCLUXSEL[4];                                                    ///< Trigger channel CLUx logic operation input select register offset: 0x080
    __IO uint32_t Reserved1[12];                                                     ///< Reserved
    __IO uint32_t TRIGCLUXCFG[4];                                                    ///< Trigger channel CLUx logic operation input control register offset: 0x080
    __IO uint32_t Reserved2[11];                                                     ///< Reserved
    __IO uint32_t SWTRIG;                                                            ///< Software Trigger Control Register            offset:  0x0FC
    __IO uint32_t CONNXCR[7];                                                        ///< Connect Channel x Control Register           offset: 0x100
    __IO uint32_t Reserved3[25];                                                     ///< Reserved
    __IO uint32_t CONNCLUXSEL[4];                                                    ///< Connection channel CLUx logic operation input control register offset: 0x180
    __IO uint32_t Reserved4[12];                                                    ///< Reserved
    __IO uint32_t CONNCLUXCFG[4];                                                    ///< Connection Channel CLUx Logic Operation Configuration Register offset: 0x1C0
} MDS_Type;

/*******************************************************************************
 * MDS Type
 ******************************************************************************/

/*!
 * @addtogroup MDS_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief MDS_TRIGXCR Register Bit Definition
 */
#define MDS_TRIGXCR_EDGESEL_SHIFT                (24)
#define MDS_TRIGXCR_EDGESEL_MASK                 (0x3U << MDS_TRIGXCR_EDGESEL_SHIFT)
#define MDS_TRIGXCR_EDGESEL(x)                   (((uint32_t)(((uint32_t)(x)) << MDS_TRIGXCR_EDGESEL_SHIFT)) & MDS_TRIGXCR_EDGESEL_MASK)

#define MDS_TRIGXCR_CLUSEL_SHIFT                (17)
#define MDS_TRIGXCR_CLUSEL_MASK                 (0x3U << MDS_TRIGXCR_CLUSEL_SHIFT)
#define MDS_TRIGXCR_CLUSEL(x)                   (((uint32_t)(((uint32_t)(x)) << MDS_TRIGXCR_CLUSEL_SHIFT)) & MDS_TRIGXCR_CLUSEL_MASK)

#define MDS_TRIGXCR_CLUEN_SHIFT                 (16)
#define MDS_TRIGXCR_CLUEN_MASK                  (0x01U << MDS_TRIGXCR_CLUEN_SHIFT)
#define MDS_TRIGXCR_CLUEN(x)                    (((uint32_t)(((uint32_t)(x)) << MDS_TRIGXCR_CLUEN_SHIFT)) & MDS_TRIGXCR_CLUEN_MASK)

#define MDS_TRIGXCR_TRGSEL_SHIFT                (0)
#define MDS_TRIGXCR_TRGSEL_MASK                 (0x3FU << MDS_TRIGXCR_TRGSEL_SHIFT)
#define MDS_TRIGXCR_TRGSEL(x)                   (((uint32_t)(((uint32_t)(x)) << MDS_TRIGXCR_TRGSEL_SHIFT)) & MDS_TRIGXCR_TRGSEL_MASK)

/*!
 * @brief MDS_TRIG_CLUXSEL Register Bit Definition
 */

#define MDS_TRIGCLUXSEL_CLUIN3SEL_SHIFT         (24)
#define MDS_TRIGCLUXSEL_CLUIN3SEL_MASK          (0xFFU << MDS_TRIGCLUXSEL_CLUIN3SEL_SHIFT)
#define MDS_TRIGCLUXSEL_CLUIN3SEL(x)            (((uint32_t)(((uint32_t)(x)) << MDS_TRIGCLUXSEL_CLUIN3SEL_SHIFT)) & MDS_TRIGCLUXSEL_CLUIN3SEL_MASK)

#define MDS_TRIGCLUXSEL_CLUIN2SEL_SHIFT         (16)
#define MDS_TRIGCLUXSEL_CLUIN2SEL_MASK          (0xFFU << MDS_TRIGCLUXSEL_CLUIN2SEL_SHIFT)
#define MDS_TRIGCLUXSEL_CLUIN2SEL(x)            (((uint32_t)(((uint32_t)(x)) << MDS_TRIGCLUXSEL_CLUIN2SEL_SHIFT)) & MDS_TRIGCLUXSEL_CLUIN2SEL_MASK)

#define MDS_TRIGCLUXSEL_CLUIN1SEL_SHIFT         (8)
#define MDS_TRIGCLUXSEL_CLUIN1SEL_MASK          (0xFFU << MDS_TRIGCLUXSEL_CLUIN1SEL_SHIFT)
#define MDS_TRIGCLUXSEL_CLUIN1SEL(x)            (((uint32_t)(((uint32_t)(x)) << MDS_TRIGCLUXSEL_CLUIN1SEL_SHIFT)) & MDS_TRIGCLUXSEL_CLUIN1SEL_MASK)

#define MDS_TRIGCLUXSEL_CLUIN0SEL_SHIFT         (0)
#define MDS_TRIGCLUXSEL_CLUIN0SEL_MASK          (0xFFU << MDS_TRIGCLUXSEL_CLUIN0SEL_SHIFT)
#define MDS_TRIGCLUXSEL_CLUIN0SEL(x)            (((uint32_t)(((uint32_t)(x)) << MDS_TRIGCLUXSEL_CLUIN0SEL_SHIFT)) & MDS_TRIGCLUXSEL_CLUIN0SEL_MASK)

/*!
 * @brief MDS_TRIG_CLUXCFG Register Bit Definition
 */

#define MDS_TRIGCLUXCFG_CLUIN3SED_SHIFT         (24)
#define MDS_TRIGCLUXCFG_CLUIN3SED_MASK          (0x03U << MDS_TRIGCLUXCFG_CLUIN3SED_SHIFT)
#define MDS_TRIGCLUXCFG_CLUIN3SED(x)            (((uint32_t)(((uint32_t)(x)) << MDS_TRIGCLUXCFG_CLUIN3SED_SHIFT)) & MDS_TRIGCLUXCFG_CLUIN3SED_MASK)

#define MDS_TRIGCLUXCFG_CLUIN2SED_SHIFT         (16)
#define MDS_TRIGCLUXCFG_CLUIN2SED_MASK          (0x03U << MDS_TRIGCLUXCFG_CLUIN2SED_SHIFT)
#define MDS_TRIGCLUXCFG_CLUIN2SED(x)            (((uint32_t)(((uint32_t)(x)) << MDS_TRIGCLUXCFG_CLUIN2SED_SHIFT)) & MDS_TRIGCLUXCFG_CLUIN2SED_MASK)

#define MDS_TRIGCLUXCFG_CLUIN1SED_SHIFT         (8)
#define MDS_TRIGCLUXCFG_CLUIN1SED_MASK          (0x03U << MDS_TRIGCLUXCFG_CLUIN1SED_SHIFT)
#define MDS_TRIGCLUXCFG_CLUIN1SED(x)            (((uint32_t)(((uint32_t)(x)) << MDS_TRIGCLUXCFG_CLUIN1SED_SHIFT)) & MDS_TRIGCLUXCFG_CLUIN1SED_MASK)

#define MDS_TRIGCLUXCFG_CLUIN0SED_SHIFT         (0)
#define MDS_TRIGCLUXCFG_CLUIN0SED_MASK          (0x03U << MDS_TRIGCLUXCFG_CLUIN0SED_SHIFT)
#define MDS_TRIGCLUXCFG_CLUIN0SED(x)            (((uint32_t)(((uint32_t)(x)) << MDS_TRIGCLUXCFG_CLUIN0SED_SHIFT)) & MDS_TRIGCLUXCFG_CLUIN0SED_MASK)

/*!
 * @brief MDS_SWTRIG Register Bit Definition
 */

#define MDS_SWTRIG_SWTRIG_SHIFT                 (0)
#define MDS_SWTRIG_SWTRIG_MASK                  (0x01U << MDS_SWTRIG_SWTRIG_SHIFT)
#define MDS_SWTRIG_SWTRIG(x)                    (((uint32_t)(((uint32_t)(x)) << MDS_SWTRIG_SWTRIG_SHIFT)) & MDS_SWTRIG_SWTRIG_MASK)

/*!
 * @brief MDS_CONNXCR Register Bit Definition
 */

#define MDS_CONNXCR_CLUSEL_SHIFT                (17)
#define MDS_CONNXCR_CLUSEL_MASK                 (0x3U << MDS_CONNXCR_CLUSEL_SHIFT)
#define MDS_CONNXCR_CLUSEL(x)                   (((uint32_t)(((uint32_t)(x)) << MDS_CONNXCR_CLUSEL_SHIFT)) & MDS_CONNXCR_CLUSEL_MASK)

#define MDS_CONNXCR_CLUEN_SHIFT                 (16)
#define MDS_CONNXCR_CLUEN_MASK                  (0x01U << MDS_CONNXCR_CLUEN_SHIFT)
#define MDS_CONNXCR_CLUEN(x)                    (((uint32_t)(((uint32_t)(x)) << MDS_CONNXCR_CLUEN_SHIFT)) & MDS_CONNXCR_CLUEN_MASK)

#define MDS_CONNXCR_TRGSEL_SHIFT                (0)
#define MDS_CONNXCR_TRGSEL_MASK                 (0x3FU << MDS_CONNXCR_TRGSEL_SHIFT)
#define MDS_CONNXCR_TRGSEL(x)                   (((uint32_t)(((uint32_t)(x)) << MDS_CONNXCR_TRGSEL_SHIFT)) & MDS_CONNXCR_TRGSEL_MASK)

/*!
 * @brief MDS_CONN_CLUXSEL Register Bit Definition
 */

#define MDS_CONNCLUXSEL_CLUIN3SEL_SHIFT         (24)
#define MDS_CONNCLUXSEL_CLUIN3SEL_MASK          (0xFFU << MDS_CONNCLUXSEL_CLUIN3SEL_SHIFT)
#define MDS_CONNCLUXSEL_CLUIN3SEL(x)            (((uint32_t)(((uint32_t)(x)) << MDS_CONNCLUXSEL_CLUIN3SEL_SHIFT)) & MDS_CONNCLUXSEL_CLUIN3SEL_MASK)

#define MDS_CONNCLUXSEL_CLUIN2SEL_SHIFT         (16)
#define MDS_CONNCLUXSEL_CLUIN2SEL_MASK          (0xFFU << MDS_CONNCLUXSEL_CLUIN2SEL_SHIFT)
#define MDS_CONNCLUXSEL_CLUIN2SEL(x)            (((uint32_t)(((uint32_t)(x)) << MDS_CONNCLUXSEL_CLUIN2SEL_SHIFT)) & MDS_CONNCLUXSEL_CLUIN2SEL_MASK)

#define MDS_CONNCLUXSEL_CLUIN1SEL_SHIFT         (8)
#define MDS_CONNCLUXSEL_CLUIN1SEL_MASK          (0xFFU << MDS_CONNCLUXSEL_CLUIN1SEL_SHIFT)
#define MDS_CONNCLUXSEL_CLUIN1SEL(x)            (((uint32_t)(((uint32_t)(x)) << MDS_CONNCLUXSEL_CLUIN1SEL_SHIFT)) & MDS_CONNCLUXSEL_CLUIN1SEL_MASK)

#define MDS_CONNCLUXSEL_CLUIN0SEL_SHIFT         (0)
#define MDS_CONNCLUXSEL_CLUIN0SEL_MASK          (0xFFU << MDS_CONNCLUXSEL_CLUIN0SEL_SHIFT)
#define MDS_CONNCLUXSEL_CLUIN0SEL(x)            (((uint32_t)(((uint32_t)(x)) << MDS_CONNCLUXSEL_CLUIN0SEL_SHIFT)) & MDS_CONNCLUXSEL_CLUIN0SEL_MASK)

/*!
 * @brief MDS_CONN_CLUXCFG Register Bit Definition
 */

#define MDS_CONNCLUXCFG_INV3_SHIFT              (11)
#define MDS_CONNCLUXCFG_INV3_MASK               (0x01U << MDS_CONNCLUXCFG_INV3_SHIFT)
#define MDS_CONNCLUXCFG_INV3(x)                 (((uint32_t)(((uint32_t)(x)) << MDS_CONNCLUXCFG_INV3_SHIFT)) & MDS_CONNCLUXCFG_INV3_MASK)

#define MDS_CONNCLUXCFG_INV2_SHIFT              (10)
#define MDS_CONNCLUXCFG_INV2_MASK               (0x01U << MDS_CONNCLUXCFG_INV2_SHIFT)
#define MDS_CONNCLUXCFG_INV2(x)                 (((uint32_t)(((uint32_t)(x)) << MDS_CONNCLUXCFG_INV2_SHIFT)) & MDS_CONNCLUXCFG_INV2_MASK)

#define MDS_CONNCLUXCFG_INV1_SHIFT              (9)
#define MDS_CONNCLUXCFG_INV1_MASK               (0x01U << MDS_CONNCLUXCFG_INV1_SHIFT)
#define MDS_CONNCLUXCFG_INV1(x)                 (((uint32_t)(((uint32_t)(x)) << MDS_CONNCLUXCFG_INV1_SHIFT)) & MDS_CONNCLUXCFG_INV1_MASK)

#define MDS_CONNCLUXCFG_INV0_SHIFT              (8)
#define MDS_CONNCLUXCFG_INV0_MASK               (0x01U << MDS_CONNCLUXCFG_INV0_SHIFT)
#define MDS_CONNCLUXCFG_INV0(x)                 (((uint32_t)(((uint32_t)(x)) << MDS_CONNCLUXCFG_INV0_SHIFT)) & MDS_CONNCLUXCFG_INV0_MASK)

#define MDS_CONNCLUXCFG_CLMODE_SHIFT            (0)
#define MDS_CONNCLUXCFG_CLMODE_MASK             (0x3U << MDS_CONNCLUXCFG_CLMODE_SHIFT)
#define MDS_CONNCLUXCFG_CLMODE(x)               (((uint32_t)(((uint32_t)(x)) << MDS_CONNCLUXCFG_CLMODE_SHIFT)) & MDS_CONNCLUXCFG_CLMODE_MASK)

/*!
 * @}
 */ /* end of group MDS_Register_Masks */
/******************************************************************************
 *MDS Instance
*******************************************************************************/

#define MDS                  ((MDS_Type*)MDS_BASE)

/*!
 * @}
 */ /* end of group MDS_Peripheral_Access_Layer */

/*!
 * @addtogroup PWR_Peripheral_Access_Layer PWR Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * PWR Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t CR1;                                                            ///< Power Control Register 1                     offset: 0x00
    __IO uint32_t CSR;                                                            ///< Power Control Status Register                offset: 0x04
    __IO uint32_t CR2;                                                            ///< Power Control Register 2                     offset: 0x08
    __IO uint32_t CR3;                                                            ///< Power Control Register 3                     offset: 0x0C
    __IO uint32_t CR4;                                                            ///< Power Control Register 4                     offset: 0x10
    __IO uint32_t CR5;                                                            ///< Power Control Register 5                     offset: 0x14
    __IO uint32_t CR6;                                                            ///< Power Control Register 6                     offset: 0x18
    __IO uint32_t SR;                                                             ///< Power Status Register                        offset: 0x1C
    __IO uint32_t SCR;                                                            ///< Power Status Clear Register                  offset: 0x20
    __IO uint32_t CFGR;                                                           ///< Power Configuration Register                 offset: 0x24
} PWR_Type;

/*******************************************************************************
 * PWR Type
 ******************************************************************************/

/*!
 * @addtogroup PWR_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief PWR_CR1 Register Bit Definition
 */

#define PWR_CR1_VOS_SHIFT                       (14)
#define PWR_CR1_VOS_MASK                        (0x3U << PWR_CR1_VOS_SHIFT)
#define PWR_CR1_VOS(x)                          (((uint32_t)(((uint32_t)(x)) << PWR_CR1_VOS_SHIFT)) & PWR_CR1_VOS_MASK)

#define PWR_CR1_LPR_SHIFT                       (13)
#define PWR_CR1_LPR_MASK                        (0x01U << PWR_CR1_LPR_SHIFT)
#define PWR_CR1_LPR(x)                          (((uint32_t)(((uint32_t)(x)) << PWR_CR1_LPR_SHIFT)) & PWR_CR1_LPR_MASK)

#define PWR_CR1_CSBF_SHIFT                      (3)
#define PWR_CR1_CSBF_MASK                       (0x01U << PWR_CR1_CSBF_SHIFT)
#define PWR_CR1_CSBF(x)                         (((uint32_t)(((uint32_t)(x)) << PWR_CR1_CSBF_SHIFT)) & PWR_CR1_CSBF_MASK)

#define PWR_CR1_PDDS_SHIFT                      (1)
#define PWR_CR1_PDDS_MASK                       (0x01U << PWR_CR1_PDDS_SHIFT)
#define PWR_CR1_PDDS(x)                         (((uint32_t)(((uint32_t)(x)) << PWR_CR1_PDDS_SHIFT)) & PWR_CR1_PDDS_MASK)

#define PWR_CR1_LPDS_SHIFT                      (0)
#define PWR_CR1_LPDS_MASK                       (0x01U << PWR_CR1_LPDS_SHIFT)
#define PWR_CR1_LPDS(x)                         (((uint32_t)(((uint32_t)(x)) << PWR_CR1_LPDS_SHIFT)) & PWR_CR1_LPDS_MASK)

/*!
 * @brief PWR_CSR Register Bit Definition
 */

#define PWR_CSR_VOSRDY_SHIFT                    (14)
#define PWR_CSR_VOSRDY_MASK                     (0x01U << PWR_CSR_VOSRDY_SHIFT)
#define PWR_CSR_VOSRDY(x)                       (((uint32_t)(((uint32_t)(x)) << PWR_CSR_VOSRDY_SHIFT)) & PWR_CSR_VOSRDY_MASK)

#define PWR_CSR_SBF_SHIFT                       (1)
#define PWR_CSR_SBF_MASK                        (0x01U << PWR_CSR_SBF_SHIFT)
#define PWR_CSR_SBF(x)                          (((uint32_t)(((uint32_t)(x)) << PWR_CSR_SBF_SHIFT)) & PWR_CSR_SBF_MASK)

/*!
 * @brief PWR_CR2 Register Bit Definition
 */

#define PWR_CR2_EWUP_SHIFT                      (0)
#define PWR_CR2_EWUP_MASK                       (0x3FU << PWR_CR2_EWUP_SHIFT)
#define PWR_CR2_EWUP(x)                         (((uint32_t)(((uint32_t)(x)) << PWR_CR2_EWUP_SHIFT)) & PWR_CR2_EWUP_MASK)

/*!
 * @brief PWR_CR3 Register Bit Definition
 */

#define PWR_CR3_WP_SHIFT                        (0)
#define PWR_CR3_WP_MASK                         (0x3FU << PWR_CR3_WP_SHIFT)
#define PWR_CR3_WP(x)                           (((uint32_t)(((uint32_t)(x)) << PWR_CR3_WP_SHIFT)) & PWR_CR3_WP_MASK)

/*!
 * @brief PWR_CR4 Register Bit Definition
 */

#define PWR_CR4_FILTCNT0_SHIFT                  (8)
#define PWR_CR4_FILTCNT0_MASK                   (0xFFU << PWR_CR4_FILTCNT0_SHIFT)
#define PWR_CR4_FILTCNT0(x)                     (((uint32_t)(((uint32_t)(x)) << PWR_CR4_FILTCNT0_SHIFT)) & PWR_CR4_FILTCNT0_MASK)

#define PWR_CR4_FILTF0_SHIFT                    (4)
#define PWR_CR4_FILTF0_MASK                     (0x01U << PWR_CR4_FILTF0_SHIFT)
#define PWR_CR4_FILTF0(x)                       (((uint32_t)(((uint32_t)(x)) << PWR_CR4_FILTF0_SHIFT)) & PWR_CR4_FILTF0_MASK)

#define PWR_CR4_FILTE0_SHIFT                    (2)
#define PWR_CR4_FILTE0_MASK                     (0x3U << PWR_CR4_FILTE0_SHIFT)
#define PWR_CR4_FILTE0(x)                       (((uint32_t)(((uint32_t)(x)) << PWR_CR4_FILTE0_SHIFT)) & PWR_CR4_FILTE0_MASK)

#define PWR_CR4_FILTSEL0_SHIFT                  (0)
#define PWR_CR4_FILTSEL0_MASK                   (0x3U << PWR_CR4_FILTSEL0_SHIFT)
#define PWR_CR4_FILTSEL0(x)                     (((uint32_t)(((uint32_t)(x)) << PWR_CR4_FILTSEL0_SHIFT)) & PWR_CR4_FILTSEL0_MASK)

/*!
 * @brief PWR_CR5 Register Bit Definition
 */

#define PWR_CR5_FILTCNT1_SHIFT                  (8)
#define PWR_CR5_FILTCNT1_MASK                   (0xFFU << PWR_CR5_FILTCNT1_SHIFT)
#define PWR_CR5_FILTCNT1(x)                     (((uint32_t)(((uint32_t)(x)) << PWR_CR5_FILTCNT1_SHIFT)) & PWR_CR5_FILTCNT1_MASK)

#define PWR_CR5_FILTF1_SHIFT                    (4)
#define PWR_CR5_FILTF1_MASK                     (0x01U << PWR_CR5_FILTF1_SHIFT)
#define PWR_CR5_FILTF1(x)                       (((uint32_t)(((uint32_t)(x)) << PWR_CR5_FILTF1_SHIFT)) & PWR_CR5_FILTF1_MASK)

#define PWR_CR5_FILTE1_SHIFT                    (2)
#define PWR_CR5_FILTE1_MASK                     (0x3U << PWR_CR5_FILTE1_SHIFT)
#define PWR_CR5_FILTE1(x)                       (((uint32_t)(((uint32_t)(x)) << PWR_CR5_FILTE1_SHIFT)) & PWR_CR5_FILTE1_MASK)

#define PWR_CR5_FILTSEL1_SHIFT                  (0)
#define PWR_CR5_FILTSEL1_MASK                   (0x3U << PWR_CR5_FILTSEL1_SHIFT)
#define PWR_CR5_FILTSEL1(x)                     (((uint32_t)(((uint32_t)(x)) << PWR_CR5_FILTSEL1_SHIFT)) & PWR_CR5_FILTSEL1_MASK)

/*!
 * @brief PWR_CR6 Register Bit Definition
 */

#define PWR_CR6_STDBYFSWK_SHIFT                 (0)
#define PWR_CR6_STDBYFSWK_MASK                  (0x7U << PWR_CR6_STDBYFSWK_SHIFT)
#define PWR_CR6_STDBYFSWK(x)                    (((uint32_t)(((uint32_t)(x)) << PWR_CR6_STDBYFSWK_SHIFT)) & PWR_CR6_STDBYFSWK_MASK)

/*!
 * @brief PWR_SR Register Bit Definition
 */

#define PWR_SR_WUF_SHIFT                        (0)
#define PWR_SR_WUF_MASK                         (0x3FU << PWR_SR_WUF_SHIFT)
#define PWR_SR_WUF(x)                           (((uint32_t)(((uint32_t)(x)) << PWR_SR_WUF_SHIFT)) & PWR_SR_WUF_MASK)

/*!
 * @brief PWR_SCR Register Bit Definition
 */

#define PWR_SCR_CWUF_SHIFT                      (0)
#define PWR_SCR_CWUF_MASK                       (0x3FU << PWR_SCR_CWUF_SHIFT)
#define PWR_SCR_CWUF(x)                         (((uint32_t)(((uint32_t)(x)) << PWR_SCR_CWUF_SHIFT)) & PWR_SCR_CWUF_MASK)

/*!
 * @brief PWR_CFGR Register Bit Definition
 */

#define PWR_CFGR_LSICAL_SHIFT                   (5)
#define PWR_CFGR_LSICAL_MASK                    (0x1FU << PWR_CFGR_LSICAL_SHIFT)
#define PWR_CFGR_LSICAL(x)                      (((uint32_t)(((uint32_t)(x)) << PWR_CFGR_LSICAL_SHIFT)) & PWR_CFGR_LSICAL_MASK)

#define PWR_CFGR_LSICALSEL_SHIFT                (0)
#define PWR_CFGR_LSICALSEL_MASK                 (0x1FU << PWR_CFGR_LSICALSEL_SHIFT)
#define PWR_CFGR_LSICALSEL(x)                   (((uint32_t)(((uint32_t)(x)) << PWR_CFGR_LSICALSEL_SHIFT)) & PWR_CFGR_LSICALSEL_MASK)

/*!
 * @}
 */ /* end of group PWR_Register_Masks */
/******************************************************************************
 *PWR Instance
*******************************************************************************/

#define PWR                  ((PWR_Type*)PWR_BASE)

/*!
 * @}
 */ /* end of group PWR_Peripheral_Access_Layer */

/*!
 * @addtogroup QSPI_Peripheral_Access_Layer QSPI Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * QSPI Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t CR;                                                             ///< control register                             offset: 0x00
    __IO uint32_t SR;                                                             ///< status register                              offset: 0x04
    __IO uint32_t DMCR;                                                           ///< Direct Mode Control Register                 offset: 0x08
    __IO uint32_t IMCR;                                                           ///< Indirect Mode Control Register               offset: 0x0C
    __IO uint32_t DABR;                                                           ///< Direct Mode Interactive Byte Register        offset: 0x10
    __IO uint32_t IABR;                                                           ///< Indirect Mode Interactive Byte Register      offset: 0x14
    __IO uint32_t IADR;                                                           ///< Indirect Mode Address Register               offset: 0x18
    __IO uint32_t IDFR;                                                           ///< Indirect Mode Data FIFO Register             offset: 0x1C
    __IO uint32_t IDLR;                                                           ///< Indirect Mode Data Length Register           offset: 0x20
    __IO uint32_t IWCR;                                                           ///< Indirect Mode Wait Count Register            offset: 0x24
    __IO uint32_t IDER;                                                           ///< Interrupt DMA Enable Register                offset: 0x28
} QSPI_Type;

/*******************************************************************************
 * QSPI Type
 ******************************************************************************/

/*!
 * @addtogroup QSPI_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief QSPI_CR Register Bit Definition
 */

#define QSPI_CR_SCKDIV_SHIFT                    (24)
#define QSPI_CR_SCKDIV_MASK                     (0x3FU << QSPI_CR_SCKDIV_SHIFT)
#define QSPI_CR_SCKDIV(x)                       (((uint32_t)(((uint32_t)(x)) << QSPI_CR_SCKDIV_SHIFT)) & QSPI_CR_SCKDIV_MASK)

#define QSPI_CR_CSRHT_SHIFT                     (16)
#define QSPI_CR_CSRHT_MASK                      (0x7U << QSPI_CR_CSRHT_SHIFT)
#define QSPI_CR_CSRHT(x)                        (((uint32_t)(((uint32_t)(x)) << QSPI_CR_CSRHT_SHIFT)) & QSPI_CR_CSRHT_MASK)

#define QSPI_CR_XIPMODE_SHIFT                   (5)
#define QSPI_CR_XIPMODE_MASK                    (0x01U << QSPI_CR_XIPMODE_SHIFT)
#define QSPI_CR_XIPMODE(x)                      (((uint32_t)(((uint32_t)(x)) << QSPI_CR_XIPMODE_SHIFT)) & QSPI_CR_XIPMODE_MASK)

#define QSPI_CR_SCKMODE_SHIFT                   (4)
#define QSPI_CR_SCKMODE_MASK                    (0x01U << QSPI_CR_SCKMODE_SHIFT)
#define QSPI_CR_SCKMODE(x)                      (((uint32_t)(((uint32_t)(x)) << QSPI_CR_SCKMODE_SHIFT)) & QSPI_CR_SCKMODE_MASK)

#define QSPI_CR_OPMODE_SHIFT                    (0)
#define QSPI_CR_OPMODE_MASK                     (0x3U << QSPI_CR_OPMODE_SHIFT)
#define QSPI_CR_OPMODE(x)                       (((uint32_t)(((uint32_t)(x)) << QSPI_CR_OPMODE_SHIFT)) & QSPI_CR_OPMODE_MASK)

/*!
 * @brief QSPI_SR Register Bit Definition
 */

#define QSPI_SR_DEPTH_SHIFT                     (24)
#define QSPI_SR_DEPTH_MASK                      (0xFU << QSPI_SR_DEPTH_SHIFT)
#define QSPI_SR_DEPTH(x)                        (((uint32_t)(((uint32_t)(x)) << QSPI_SR_DEPTH_SHIFT)) & QSPI_SR_DEPTH_MASK)

#define QSPI_SR_LEVEL_SHIFT                     (12)
#define QSPI_SR_LEVEL_MASK                      (0xFU << QSPI_SR_LEVEL_SHIFT)
#define QSPI_SR_LEVEL(x)                        (((uint32_t)(((uint32_t)(x)) << QSPI_SR_LEVEL_SHIFT)) & QSPI_SR_LEVEL_MASK)

#define QSPI_SR_FULL_SHIFT                      (9)
#define QSPI_SR_FULL_MASK                       (0x01U << QSPI_SR_FULL_SHIFT)
#define QSPI_SR_FULL(x)                         (((uint32_t)(((uint32_t)(x)) << QSPI_SR_FULL_SHIFT)) & QSPI_SR_FULL_MASK)

#define QSPI_SR_EMPTY_SHIFT                     (8)
#define QSPI_SR_EMPTY_MASK                      (0x01U << QSPI_SR_EMPTY_SHIFT)
#define QSPI_SR_EMPTY(x)                        (((uint32_t)(((uint32_t)(x)) << QSPI_SR_EMPTY_SHIFT)) & QSPI_SR_EMPTY_MASK)

#define QSPI_SR_XIPST_SHIFT                     (6)
#define QSPI_SR_XIPST_MASK                      (0x01U << QSPI_SR_XIPST_SHIFT)
#define QSPI_SR_XIPST(x)                        (((uint32_t)(((uint32_t)(x)) << QSPI_SR_XIPST_SHIFT)) & QSPI_SR_XIPST_MASK)

#define QSPI_SR_BUSY_SHIFT                      (5)
#define QSPI_SR_BUSY_MASK                       (0x01U << QSPI_SR_BUSY_SHIFT)
#define QSPI_SR_BUSY(x)                         (((uint32_t)(((uint32_t)(x)) << QSPI_SR_BUSY_SHIFT)) & QSPI_SR_BUSY_MASK)

#define QSPI_SR_TCF_SHIFT                       (4)
#define QSPI_SR_TCF_MASK                        (0x01U << QSPI_SR_TCF_SHIFT)
#define QSPI_SR_TCF(x)                          (((uint32_t)(((uint32_t)(x)) << QSPI_SR_TCF_SHIFT)) & QSPI_SR_TCF_MASK)

#define QSPI_SR_OPCRCF_SHIFT                    (2)
#define QSPI_SR_OPCRCF_MASK                     (0x01U << QSPI_SR_OPCRCF_SHIFT)
#define QSPI_SR_OPCRCF(x)                       (((uint32_t)(((uint32_t)(x)) << QSPI_SR_OPCRCF_SHIFT)) & QSPI_SR_OPCRCF_MASK)

#define QSPI_SR_CUROP_SHIFT                     (0)
#define QSPI_SR_CUROP_MASK                      (0x3U << QSPI_SR_CUROP_SHIFT)
#define QSPI_SR_CUROP(x)                        (((uint32_t)(((uint32_t)(x)) << QSPI_SR_CUROP_SHIFT)) & QSPI_SR_CUROP_MASK)

/*!
 * @brief QSPI_DMCR Register Bit Definition
 */

#define QSPI_DMCR_NUMDC_SHIFT                   (24)
#define QSPI_DMCR_NUMDC_MASK                    (0x1FU << QSPI_DMCR_NUMDC_SHIFT)
#define QSPI_DMCR_NUMDC(x)                      (((uint32_t)(((uint32_t)(x)) << QSPI_DMCR_NUMDC_SHIFT)) & QSPI_DMCR_NUMDC_MASK)

#define QSPI_DMCR_RXDLY_SHIFT                   (22)
#define QSPI_DMCR_RXDLY_MASK                    (0x3U << QSPI_DMCR_RXDLY_SHIFT)
#define QSPI_DMCR_RXDLY(x)                      (((uint32_t)(((uint32_t)(x)) << QSPI_DMCR_RXDLY_SHIFT)) & QSPI_DMCR_RXDLY_MASK)

#define QSPI_DMCR_DSIZE_SHIFT                   (20)
#define QSPI_DMCR_DSIZE_MASK                    (0x3U << QSPI_DMCR_DSIZE_SHIFT)
#define QSPI_DMCR_DSIZE(x)                      (((uint32_t)(((uint32_t)(x)) << QSPI_DMCR_DSIZE_SHIFT)) & QSPI_DMCR_DSIZE_MASK)

#define QSPI_DMCR_DMODE_SHIFT                   (18)
#define QSPI_DMCR_DMODE_MASK                    (0x3U << QSPI_DMCR_DMODE_SHIFT)
#define QSPI_DMCR_DMODE(x)                      (((uint32_t)(((uint32_t)(x)) << QSPI_DMCR_DMODE_SHIFT)) & QSPI_DMCR_DMODE_MASK)

#define QSPI_DMCR_ABSIZE_SHIFT                  (16)
#define QSPI_DMCR_ABSIZE_MASK                   (0x3U << QSPI_DMCR_ABSIZE_SHIFT)
#define QSPI_DMCR_ABSIZE(x)                     (((uint32_t)(((uint32_t)(x)) << QSPI_DMCR_ABSIZE_SHIFT)) & QSPI_DMCR_ABSIZE_MASK)

#define QSPI_DMCR_ABMODE_SHIFT                  (14)
#define QSPI_DMCR_ABMODE_MASK                   (0x3U << QSPI_DMCR_ABMODE_SHIFT)
#define QSPI_DMCR_ABMODE(x)                     (((uint32_t)(((uint32_t)(x)) << QSPI_DMCR_ABMODE_SHIFT)) & QSPI_DMCR_ABMODE_MASK)

#define QSPI_DMCR_ADSIZE_SHIFT                  (12)
#define QSPI_DMCR_ADSIZE_MASK                   (0x3U << QSPI_DMCR_ADSIZE_SHIFT)
#define QSPI_DMCR_ADSIZE(x)                     (((uint32_t)(((uint32_t)(x)) << QSPI_DMCR_ADSIZE_SHIFT)) & QSPI_DMCR_ADSIZE_MASK)

#define QSPI_DMCR_ADMODE_SHIFT                  (10)
#define QSPI_DMCR_ADMODE_MASK                   (0x3U << QSPI_DMCR_ADMODE_SHIFT)
#define QSPI_DMCR_ADMODE(x)                     (((uint32_t)(((uint32_t)(x)) << QSPI_DMCR_ADMODE_SHIFT)) & QSPI_DMCR_ADMODE_MASK)

#define QSPI_DMCR_IMODE_SHIFT                   (8)
#define QSPI_DMCR_IMODE_MASK                    (0x3U << QSPI_DMCR_IMODE_SHIFT)
#define QSPI_DMCR_IMODE(x)                      (((uint32_t)(((uint32_t)(x)) << QSPI_DMCR_IMODE_SHIFT)) & QSPI_DMCR_IMODE_MASK)

#define QSPI_DMCR_INST_SHIFT                    (0)
#define QSPI_DMCR_INST_MASK                     (0xFFU << QSPI_DMCR_INST_SHIFT)
#define QSPI_DMCR_INST(x)                       (((uint32_t)(((uint32_t)(x)) << QSPI_DMCR_INST_SHIFT)) & QSPI_DMCR_INST_MASK)

/*!
 * @brief QSPI_IMCR Register Bit Definition
 */

#define QSPI_IMCR_IDMODE_SHIFT                  (30)
#define QSPI_IMCR_IDMODE_MASK                   (0x01U << QSPI_IMCR_IDMODE_SHIFT)
#define QSPI_IMCR_IDMODE(x)                     (((uint32_t)(((uint32_t)(x)) << QSPI_IMCR_IDMODE_SHIFT)) & QSPI_IMCR_IDMODE_MASK)

#define QSPI_IMCR_XIPIM_SHIFT                   (29)
#define QSPI_IMCR_XIPIM_MASK                    (0x01U << QSPI_IMCR_XIPIM_SHIFT)
#define QSPI_IMCR_XIPIM(x)                      (((uint32_t)(((uint32_t)(x)) << QSPI_IMCR_XIPIM_SHIFT)) & QSPI_IMCR_XIPIM_MASK)

#define QSPI_IMCR_NUMDC_SHIFT                   (24)
#define QSPI_IMCR_NUMDC_MASK                    (0x1FU << QSPI_IMCR_NUMDC_SHIFT)
#define QSPI_IMCR_NUMDC(x)                      (((uint32_t)(((uint32_t)(x)) << QSPI_IMCR_NUMDC_SHIFT)) & QSPI_IMCR_NUMDC_MASK)

#define QSPI_IMCR_RXDLY_SHIFT                   (22)
#define QSPI_IMCR_RXDLY_MASK                    (0x3U << QSPI_IMCR_RXDLY_SHIFT)
#define QSPI_IMCR_RXDLY(x)                      (((uint32_t)(((uint32_t)(x)) << QSPI_IMCR_RXDLY_SHIFT)) & QSPI_IMCR_RXDLY_MASK)

#define QSPI_IMCR_DSIZE_SHIFT                   (20)
#define QSPI_IMCR_DSIZE_MASK                    (0x3U << QSPI_IMCR_DSIZE_SHIFT)
#define QSPI_IMCR_DSIZE(x)                      (((uint32_t)(((uint32_t)(x)) << QSPI_IMCR_DSIZE_SHIFT)) & QSPI_IMCR_DSIZE_MASK)

#define QSPI_IMCR_DMODE_SHIFT                   (18)
#define QSPI_IMCR_DMODE_MASK                    (0x3U << QSPI_IMCR_DMODE_SHIFT)
#define QSPI_IMCR_DMODE(x)                      (((uint32_t)(((uint32_t)(x)) << QSPI_IMCR_DMODE_SHIFT)) & QSPI_IMCR_DMODE_MASK)

#define QSPI_IMCR_ABSIZE_SHIFT                  (16)
#define QSPI_IMCR_ABSIZE_MASK                   (0x3U << QSPI_IMCR_ABSIZE_SHIFT)
#define QSPI_IMCR_ABSIZE(x)                     (((uint32_t)(((uint32_t)(x)) << QSPI_IMCR_ABSIZE_SHIFT)) & QSPI_IMCR_ABSIZE_MASK)

#define QSPI_IMCR_ABMODE_SHIFT                  (14)
#define QSPI_IMCR_ABMODE_MASK                   (0x3U << QSPI_IMCR_ABMODE_SHIFT)
#define QSPI_IMCR_ABMODE(x)                     (((uint32_t)(((uint32_t)(x)) << QSPI_IMCR_ABMODE_SHIFT)) & QSPI_IMCR_ABMODE_MASK)

#define QSPI_IMCR_ADSIZE_SHIFT                  (12)
#define QSPI_IMCR_ADSIZE_MASK                   (0x3U << QSPI_IMCR_ADSIZE_SHIFT)
#define QSPI_IMCR_ADSIZE(x)                     (((uint32_t)(((uint32_t)(x)) << QSPI_IMCR_ADSIZE_SHIFT)) & QSPI_IMCR_ADSIZE_MASK)

#define QSPI_IMCR_ADMODE_SHIFT                  (10)
#define QSPI_IMCR_ADMODE_MASK                   (0x3U << QSPI_IMCR_ADMODE_SHIFT)
#define QSPI_IMCR_ADMODE(x)                     (((uint32_t)(((uint32_t)(x)) << QSPI_IMCR_ADMODE_SHIFT)) & QSPI_IMCR_ADMODE_MASK)

#define QSPI_IMCR_IMODE_SHIFT                   (8)
#define QSPI_IMCR_IMODE_MASK                    (0x3U << QSPI_IMCR_IMODE_SHIFT)
#define QSPI_IMCR_IMODE(x)                      (((uint32_t)(((uint32_t)(x)) << QSPI_IMCR_IMODE_SHIFT)) & QSPI_IMCR_IMODE_MASK)

#define QSPI_IMCR_INST_SHIFT                    (0)
#define QSPI_IMCR_INST_MASK                     (0xFFU << QSPI_IMCR_INST_SHIFT)
#define QSPI_IMCR_INST(x)                       (((uint32_t)(((uint32_t)(x)) << QSPI_IMCR_INST_SHIFT)) & QSPI_IMCR_INST_MASK)

/*!
 * @brief QSPI_DABR Register Bit Definition
 */

#define QSPI_DABR_DALT_SHIFT                    (0)
#define QSPI_DABR_DALT_MASK                     (0xFFFFFFFFU << QSPI_DABR_DALT_SHIFT)
#define QSPI_DABR_DALT(x)                       (((uint32_t)(((uint32_t)(x)) << QSPI_DABR_DALT_SHIFT)) & QSPI_DABR_DALT_MASK)

/*!
 * @brief QSPI_IABR Register Bit Definition
 */

#define QSPI_IABR_IALT_SHIFT                    (0)
#define QSPI_IABR_IALT_MASK                     (0xFFFFFFFFU << QSPI_IABR_IALT_SHIFT)
#define QSPI_IABR_IALT(x)                       (((uint32_t)(((uint32_t)(x)) << QSPI_IABR_IALT_SHIFT)) & QSPI_IABR_IALT_MASK)

/*!
 * @brief QSPI_IADR Register Bit Definition
 */

#define QSPI_IADR_IADDR_SHIFT                   (0)
#define QSPI_IADR_IADDR_MASK                    (0xFFFFFFFFU << QSPI_IADR_IADDR_SHIFT)
#define QSPI_IADR_IADDR(x)                      (((uint32_t)(((uint32_t)(x)) << QSPI_IADR_IADDR_SHIFT)) & QSPI_IADR_IADDR_MASK)

/*!
 * @brief QSPI_IDFR Register Bit Definition
 */

#define QSPI_IDFR_IDATA_SHIFT                   (0)
#define QSPI_IDFR_IDATA_MASK                    (0xFFFFFFFFU << QSPI_IDFR_IDATA_SHIFT)
#define QSPI_IDFR_IDATA(x)                      (((uint32_t)(((uint32_t)(x)) << QSPI_IDFR_IDATA_SHIFT)) & QSPI_IDFR_IDATA_MASK)

/*!
 * @brief QSPI_IDLR Register Bit Definition
 */

#define QSPI_IDLR_IDLEN_SHIFT                   (0)
#define QSPI_IDLR_IDLEN_MASK                    (0xFFFFFU << QSPI_IDLR_IDLEN_SHIFT)
#define QSPI_IDLR_IDLEN(x)                      (((uint32_t)(((uint32_t)(x)) << QSPI_IDLR_IDLEN_SHIFT)) & QSPI_IDLR_IDLEN_MASK)

/*!
 * @brief QSPI_IWCR Register Bit Definition
 */

#define QSPI_IWCR_IWCNT_SHIFT                   (0)
#define QSPI_IWCR_IWCNT_MASK                    (0x3FFFFFFFU << QSPI_IWCR_IWCNT_SHIFT)
#define QSPI_IWCR_IWCNT(x)                      (((uint32_t)(((uint32_t)(x)) << QSPI_IWCR_IWCNT_SHIFT)) & QSPI_IWCR_IWCNT_MASK)

/*!
 * @brief QSPI_IDER Register Bit Definition
 */

#define QSPI_IDER_FULLDMAEN_SHIFT               (15)
#define QSPI_IDER_FULLDMAEN_MASK                (0x01U << QSPI_IDER_FULLDMAEN_SHIFT)
#define QSPI_IDER_FULLDMAEN(x)                  (((uint32_t)(((uint32_t)(x)) << QSPI_IDER_FULLDMAEN_SHIFT)) & QSPI_IDER_FULLDMAEN_MASK)

#define QSPI_IDER_EMPTYDMAEN_SHIFT              (14)
#define QSPI_IDER_EMPTYDMAEN_MASK               (0x01U << QSPI_IDER_EMPTYDMAEN_SHIFT)
#define QSPI_IDER_EMPTYDMAEN(x)                 (((uint32_t)(((uint32_t)(x)) << QSPI_IDER_EMPTYDMAEN_SHIFT)) & QSPI_IDER_EMPTYDMAEN_MASK)

#define QSPI_IDER_TCFDMAEN_SHIFT                (8)
#define QSPI_IDER_TCFDMAEN_MASK                 (0x01U << QSPI_IDER_TCFDMAEN_SHIFT)
#define QSPI_IDER_TCFDMAEN(x)                   (((uint32_t)(((uint32_t)(x)) << QSPI_IDER_TCFDMAEN_SHIFT)) & QSPI_IDER_TCFDMAEN_MASK)

#define QSPI_IDER_FULLINTEN_SHIFT               (7)
#define QSPI_IDER_FULLINTEN_MASK                (0x01U << QSPI_IDER_FULLINTEN_SHIFT)
#define QSPI_IDER_FULLINTEN(x)                  (((uint32_t)(((uint32_t)(x)) << QSPI_IDER_FULLINTEN_SHIFT)) & QSPI_IDER_FULLINTEN_MASK)

#define QSPI_IDER_EMPTYINTEN_SHIFT              (6)
#define QSPI_IDER_EMPTYINTEN_MASK               (0x01U << QSPI_IDER_EMPTYINTEN_SHIFT)
#define QSPI_IDER_EMPTYINTEN(x)                 (((uint32_t)(((uint32_t)(x)) << QSPI_IDER_EMPTYINTEN_SHIFT)) & QSPI_IDER_EMPTYINTEN_MASK)

#define QSPI_IDER_TCFINTEN_SHIFT                (0)
#define QSPI_IDER_TCFINTEN_MASK                 (0x01U << QSPI_IDER_TCFINTEN_SHIFT)
#define QSPI_IDER_TCFINTEN(x)                   (((uint32_t)(((uint32_t)(x)) << QSPI_IDER_TCFINTEN_SHIFT)) & QSPI_IDER_TCFINTEN_MASK)

/*!
 * @}
 */ /* end of group QSPI_Register_Masks */
/******************************************************************************
 *QSPI Instance
*******************************************************************************/

#define QSPI                 ((QSPI_Type*)QSPI_BASE)

/*!
 * @}
 */ /* end of group QSPI_Peripheral_Access_Layer */


/*!
 * @addtogroup RCC_Peripheral_Access_Layer RCC Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * RCC Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t CR;                                                             ///< clock control register                       offset: 0x00
    __IO uint32_t CFGR;                                                           ///< Clock Configuration Register                 offset: 0x04
    __IO uint32_t CIR;                                                            ///< clock interrupt register                     offset: 0x08
    __IO uint32_t Reserved42[2];                                                   ///< Reserved
    __IO uint32_t AHB1RSTR;                                                       ///< AHB1 Peripheral Reset Register               offset: 0x14
    __IO uint32_t APB2RSTR;                                                       ///< APB2 Peripheral Reset Register               offset: 0x18
    __IO uint32_t APB1RSTR;                                                       ///< APB1 Peripheral Reset Register               offset: 0x1C
    __IO uint32_t Reserved43[2];                                                   ///< Reserved
    __IO uint32_t AHB1ENR;                                                        ///< AHB1 peripheral clock enable register        offset: 0x28
    __IO uint32_t APB2ENR;                                                        ///< APB2 Peripheral Clock Enable Register        offset: 0x2C
    __IO uint32_t APB1ENR;                                                        ///< APB1 Peripheral Clock Enable Register        offset: 0x30
    __IO uint32_t BDCR;                                                           ///< Backup Domain Control Register               offset: 0x34
    __IO uint32_t CSR;                                                            ///< Control Status Register                      offset: 0x38
    __IO uint32_t SYSCFG;                                                         ///< System Configuration Register                offset: 0x3C
    __IO uint32_t CFGR2;                                                          ///< Clock Configuration Register 2               offset: 0x40
    __IO uint32_t Reserved44;                                                          ///< Reserved                                     offset: 0x44
    __IO uint32_t PLL1CFGR;                                                       ///< PLL1 Configuration Register                  offset: 0x48
    __IO uint32_t PLL2CFGR;                                                       ///< PLL2 Configuration Register                  offset: 0x4C
    __IO uint32_t Reserved45[4];                                                   ///< Reserved
    __IO uint32_t ADC1CFGR;                                                       ///< ADC1 Configuration Register                  offset: 0x60
    __IO uint32_t ADC2CFGR;                                                       ///< ADC2 Configuration Register                  offset: 0x64
    __IO uint32_t Reserved46[2];                                                   ///< Reserved
    __IO uint32_t DACCFGR;                                                        ///< DAC Configuration Register                   offset: 0x70
    __IO uint32_t Reserved47[1];                                                   ///< Reserved
    __IO uint32_t FSMCCFGR;                                                       ///< FSMC Configuration Register                  offset: 0x78
    __IO uint32_t TPIUCFGR;                                                       ///< TPIU configuration register                  offset: 0x7C
} RCC_Type;

/*******************************************************************************
 * RCC Type
 ******************************************************************************/

/*!
 * @addtogroup RCC_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief RCC_CR Register Bit Definition
 */

#define RCC_CR_PLL2RDY_SHIFT                    (29)
#define RCC_CR_PLL2RDY_MASK                     (0x01U << RCC_CR_PLL2RDY_SHIFT)
#define RCC_CR_PLL2RDY(x)                       (((uint32_t)(((uint32_t)(x)) << RCC_CR_PLL2RDY_SHIFT)) & RCC_CR_PLL2RDY_MASK)

#define RCC_CR_PLL2ON_SHIFT                     (28)
#define RCC_CR_PLL2ON_MASK                      (0x01U << RCC_CR_PLL2ON_SHIFT)
#define RCC_CR_PLL2ON(x)                        (((uint32_t)(((uint32_t)(x)) << RCC_CR_PLL2ON_SHIFT)) & RCC_CR_PLL2ON_MASK)

#define RCC_CR_PLL1RDY_SHIFT                    (25)
#define RCC_CR_PLL1RDY_MASK                     (0x01U << RCC_CR_PLL1RDY_SHIFT)
#define RCC_CR_PLL1RDY(x)                       (((uint32_t)(((uint32_t)(x)) << RCC_CR_PLL1RDY_SHIFT)) & RCC_CR_PLL1RDY_MASK)

#define RCC_CR_PLL1ON_SHIFT                     (24)
#define RCC_CR_PLL1ON_MASK                      (0x01U << RCC_CR_PLL1ON_SHIFT)
#define RCC_CR_PLL1ON(x)                        (((uint32_t)(((uint32_t)(x)) << RCC_CR_PLL1ON_SHIFT)) & RCC_CR_PLL1ON_MASK)

#define RCC_CR_HSEIB_SHIFT                      (22)
#define RCC_CR_HSEIB_MASK                       (0x3U << RCC_CR_HSEIB_SHIFT)
#define RCC_CR_HSEIB(x)                         (((uint32_t)(((uint32_t)(x)) << RCC_CR_HSEIB_SHIFT)) & RCC_CR_HSEIB_MASK)

#define RCC_CR_HSEAACSEL_SHIFT                  (21)
#define RCC_CR_HSEAACSEL_MASK                   (0x01U << RCC_CR_HSEAACSEL_SHIFT)
#define RCC_CR_HSEAACSEL(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_CR_HSEAACSEL_SHIFT)) & RCC_CR_HSEAACSEL_MASK)

#define RCC_CR_CSSON_SHIFT                      (19)
#define RCC_CR_CSSON_MASK                       (0x01U << RCC_CR_CSSON_SHIFT)
#define RCC_CR_CSSON(x)                         (((uint32_t)(((uint32_t)(x)) << RCC_CR_CSSON_SHIFT)) & RCC_CR_CSSON_MASK)

#define RCC_CR_HSEBYP_SHIFT                     (18)
#define RCC_CR_HSEBYP_MASK                      (0x01U << RCC_CR_HSEBYP_SHIFT)
#define RCC_CR_HSEBYP(x)                        (((uint32_t)(((uint32_t)(x)) << RCC_CR_HSEBYP_SHIFT)) & RCC_CR_HSEBYP_MASK)

#define RCC_CR_HSERDY_SHIFT                     (17)
#define RCC_CR_HSERDY_MASK                      (0x01U << RCC_CR_HSERDY_SHIFT)
#define RCC_CR_HSERDY(x)                        (((uint32_t)(((uint32_t)(x)) << RCC_CR_HSERDY_SHIFT)) & RCC_CR_HSERDY_MASK)

#define RCC_CR_HSEON_SHIFT                      (16)
#define RCC_CR_HSEON_MASK                       (0x01U << RCC_CR_HSEON_SHIFT)
#define RCC_CR_HSEON(x)                         (((uint32_t)(((uint32_t)(x)) << RCC_CR_HSEON_SHIFT)) & RCC_CR_HSEON_MASK)

#define RCC_CR_HSIDIV_SHIFT                     (11)
#define RCC_CR_HSIDIV_MASK                      (0x7U << RCC_CR_HSIDIV_SHIFT)
#define RCC_CR_HSIDIV(x)                        (((uint32_t)(((uint32_t)(x)) << RCC_CR_HSIDIV_SHIFT)) & RCC_CR_HSIDIV_MASK)

#define RCC_CR_HSEDR_SHIFT                      (9)
#define RCC_CR_HSEDR_MASK                       (0x3U << RCC_CR_HSEDR_SHIFT)
#define RCC_CR_HSEDR(x)                         (((uint32_t)(((uint32_t)(x)) << RCC_CR_HSEDR_SHIFT)) & RCC_CR_HSEDR_MASK)

#define RCC_CR_HSEOUTPUTSEL_SHIFT               (8)
#define RCC_CR_HSEOUTPUTSEL_MASK                (0x01U << RCC_CR_HSEOUTPUTSEL_SHIFT)
#define RCC_CR_HSEOUTPUTSEL(x)                  (((uint32_t)(((uint32_t)(x)) << RCC_CR_HSEOUTPUTSEL_SHIFT)) & RCC_CR_HSEOUTPUTSEL_MASK)

#define RCC_CR_HSEDEGLITCHSEL_SHIFT             (7)
#define RCC_CR_HSEDEGLITCHSEL_MASK              (0x01U << RCC_CR_HSEDEGLITCHSEL_SHIFT)
#define RCC_CR_HSEDEGLITCHSEL(x)                (((uint32_t)(((uint32_t)(x)) << RCC_CR_HSEDEGLITCHSEL_SHIFT)) & RCC_CR_HSEDEGLITCHSEL_MASK)

#define RCC_CR_HSEDEGLITCHBYP_SHIFT             (6)
#define RCC_CR_HSEDEGLITCHBYP_MASK              (0x01U << RCC_CR_HSEDEGLITCHBYP_SHIFT)
#define RCC_CR_HSEDEGLITCHBYP(x)                (((uint32_t)(((uint32_t)(x)) << RCC_CR_HSEDEGLITCHBYP_SHIFT)) & RCC_CR_HSEDEGLITCHBYP_MASK)

#define RCC_CR_HSELPFSEL_SHIFT                  (5)
#define RCC_CR_HSELPFSEL_MASK                   (0x01U << RCC_CR_HSELPFSEL_SHIFT)
#define RCC_CR_HSELPFSEL(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_CR_HSELPFSEL_SHIFT)) & RCC_CR_HSELPFSEL_MASK)

#define RCC_CR_HSELPFBYP_SHIFT                  (4)
#define RCC_CR_HSELPFBYP_MASK                   (0x01U << RCC_CR_HSELPFBYP_SHIFT)
#define RCC_CR_HSELPFBYP(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_CR_HSELPFBYP_SHIFT)) & RCC_CR_HSELPFBYP_MASK)

#define RCC_CR_HSIRDY_SHIFT                     (1)
#define RCC_CR_HSIRDY_MASK                      (0x01U << RCC_CR_HSIRDY_SHIFT)
#define RCC_CR_HSIRDY(x)                        (((uint32_t)(((uint32_t)(x)) << RCC_CR_HSIRDY_SHIFT)) & RCC_CR_HSIRDY_MASK)

#define RCC_CR_HSION_SHIFT                      (0)
#define RCC_CR_HSION_MASK                       (0x01U << RCC_CR_HSION_SHIFT)
#define RCC_CR_HSION(x)                         (((uint32_t)(((uint32_t)(x)) << RCC_CR_HSION_SHIFT)) & RCC_CR_HSION_MASK)

/*!
 * @brief RCC_CFGR Register Bit Definition
 */

#define RCC_CFGR_MCO_SHIFT                      (24)
#define RCC_CFGR_MCO_MASK                       (0xFU << RCC_CFGR_MCO_SHIFT)
#define RCC_CFGR_MCO(x)                         (((uint32_t)(((uint32_t)(x)) << RCC_CFGR_MCO_SHIFT)) & RCC_CFGR_MCO_MASK)

#define RCC_CFGR_USBPRE_SHIFT                   (22)
#define RCC_CFGR_USBPRE_MASK                    (0x3U << RCC_CFGR_USBPRE_SHIFT)
#define RCC_CFGR_USBPRE(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_CFGR_USBPRE_SHIFT)) & RCC_CFGR_USBPRE_MASK)

#define RCC_CFGR_USBCLKSEL_SHIFT                (19)
#define RCC_CFGR_USBCLKSEL_MASK                 (0x01U << RCC_CFGR_USBCLKSEL_SHIFT)
#define RCC_CFGR_USBCLKSEL(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_CFGR_USBCLKSEL_SHIFT)) & RCC_CFGR_USBCLKSEL_MASK)

#define RCC_CFGR_ADC2CLKSEL_SHIFT               (17)
#define RCC_CFGR_ADC2CLKSEL_MASK                (0x01U << RCC_CFGR_ADC2CLKSEL_SHIFT)
#define RCC_CFGR_ADC2CLKSEL(x)                  (((uint32_t)(((uint32_t)(x)) << RCC_CFGR_ADC2CLKSEL_SHIFT)) & RCC_CFGR_ADC2CLKSEL_MASK)

#define RCC_CFGR_ADC1CLKSEL_SHIFT               (16)
#define RCC_CFGR_ADC1CLKSEL_MASK                (0x01U << RCC_CFGR_ADC1CLKSEL_SHIFT)
#define RCC_CFGR_ADC1CLKSEL(x)                  (((uint32_t)(((uint32_t)(x)) << RCC_CFGR_ADC1CLKSEL_SHIFT)) & RCC_CFGR_ADC1CLKSEL_MASK)

#define RCC_CFGR_PPRE2_SHIFT                    (11)
#define RCC_CFGR_PPRE2_MASK                     (0x7U << RCC_CFGR_PPRE2_SHIFT)
#define RCC_CFGR_PPRE2(x)                       (((uint32_t)(((uint32_t)(x)) << RCC_CFGR_PPRE2_SHIFT)) & RCC_CFGR_PPRE2_MASK)

#define RCC_CFGR_PPRE1_SHIFT                    (8)
#define RCC_CFGR_PPRE1_MASK                     (0x7U << RCC_CFGR_PPRE1_SHIFT)
#define RCC_CFGR_PPRE1(x)                       (((uint32_t)(((uint32_t)(x)) << RCC_CFGR_PPRE1_SHIFT)) & RCC_CFGR_PPRE1_MASK)

#define RCC_CFGR_HPRE_SHIFT                     (4)
#define RCC_CFGR_HPRE_MASK                      (0xFU << RCC_CFGR_HPRE_SHIFT)
#define RCC_CFGR_HPRE(x)                        (((uint32_t)(((uint32_t)(x)) << RCC_CFGR_HPRE_SHIFT)) & RCC_CFGR_HPRE_MASK)

#define RCC_CFGR_SWS_SHIFT                      (2)
#define RCC_CFGR_SWS_MASK                       (0x3U << RCC_CFGR_SWS_SHIFT)
#define RCC_CFGR_SWS(x)                         (((uint32_t)(((uint32_t)(x)) << RCC_CFGR_SWS_SHIFT)) & RCC_CFGR_SWS_MASK)

#define RCC_CFGR_SW_SHIFT                       (0)
#define RCC_CFGR_SW_MASK                        (0x3U << RCC_CFGR_SW_SHIFT)
#define RCC_CFGR_SW(x)                          (((uint32_t)(((uint32_t)(x)) << RCC_CFGR_SW_SHIFT)) & RCC_CFGR_SW_MASK)

/*!
 * @brief RCC_CIR Register Bit Definition
 */

#define RCC_CIR_CSSC_SHIFT                      (23)
#define RCC_CIR_CSSC_MASK                       (0x01U << RCC_CIR_CSSC_SHIFT)
#define RCC_CIR_CSSC(x)                         (((uint32_t)(((uint32_t)(x)) << RCC_CIR_CSSC_SHIFT)) & RCC_CIR_CSSC_MASK)

#define RCC_CIR_PLL2RDYC_SHIFT                  (21)
#define RCC_CIR_PLL2RDYC_MASK                   (0x01U << RCC_CIR_PLL2RDYC_SHIFT)
#define RCC_CIR_PLL2RDYC(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_CIR_PLL2RDYC_SHIFT)) & RCC_CIR_PLL2RDYC_MASK)

#define RCC_CIR_PLL1RDYC_SHIFT                  (20)
#define RCC_CIR_PLL1RDYC_MASK                   (0x01U << RCC_CIR_PLL1RDYC_SHIFT)
#define RCC_CIR_PLL1RDYC(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_CIR_PLL1RDYC_SHIFT)) & RCC_CIR_PLL1RDYC_MASK)

#define RCC_CIR_HSERDYC_SHIFT                   (19)
#define RCC_CIR_HSERDYC_MASK                    (0x01U << RCC_CIR_HSERDYC_SHIFT)
#define RCC_CIR_HSERDYC(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_CIR_HSERDYC_SHIFT)) & RCC_CIR_HSERDYC_MASK)

#define RCC_CIR_HSIRDYC_SHIFT                   (18)
#define RCC_CIR_HSIRDYC_MASK                    (0x01U << RCC_CIR_HSIRDYC_SHIFT)
#define RCC_CIR_HSIRDYC(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_CIR_HSIRDYC_SHIFT)) & RCC_CIR_HSIRDYC_MASK)

#define RCC_CIR_LSERDYC_SHIFT                   (17)
#define RCC_CIR_LSERDYC_MASK                    (0x01U << RCC_CIR_LSERDYC_SHIFT)
#define RCC_CIR_LSERDYC(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_CIR_LSERDYC_SHIFT)) & RCC_CIR_LSERDYC_MASK)

#define RCC_CIR_LSIRDYC_SHIFT                   (16)
#define RCC_CIR_LSIRDYC_MASK                    (0x01U << RCC_CIR_LSIRDYC_SHIFT)
#define RCC_CIR_LSIRDYC(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_CIR_LSIRDYC_SHIFT)) & RCC_CIR_LSIRDYC_MASK)

#define RCC_CIR_PLL2RDYIE_SHIFT                 (13)
#define RCC_CIR_PLL2RDYIE_MASK                  (0x01U << RCC_CIR_PLL2RDYIE_SHIFT)
#define RCC_CIR_PLL2RDYIE(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_CIR_PLL2RDYIE_SHIFT)) & RCC_CIR_PLL2RDYIE_MASK)

#define RCC_CIR_PLL1RDYIE_SHIFT                 (12)
#define RCC_CIR_PLL1RDYIE_MASK                  (0x01U << RCC_CIR_PLL1RDYIE_SHIFT)
#define RCC_CIR_PLL1RDYIE(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_CIR_PLL1RDYIE_SHIFT)) & RCC_CIR_PLL1RDYIE_MASK)

#define RCC_CIR_HSERDYIE_SHIFT                  (11)
#define RCC_CIR_HSERDYIE_MASK                   (0x01U << RCC_CIR_HSERDYIE_SHIFT)
#define RCC_CIR_HSERDYIE(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_CIR_HSERDYIE_SHIFT)) & RCC_CIR_HSERDYIE_MASK)

#define RCC_CIR_HSIRDYIE_SHIFT                  (10)
#define RCC_CIR_HSIRDYIE_MASK                   (0x01U << RCC_CIR_HSIRDYIE_SHIFT)
#define RCC_CIR_HSIRDYIE(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_CIR_HSIRDYIE_SHIFT)) & RCC_CIR_HSIRDYIE_MASK)

#define RCC_CIR_LSERDYIE_SHIFT                  (9)
#define RCC_CIR_LSERDYIE_MASK                   (0x01U << RCC_CIR_LSERDYIE_SHIFT)
#define RCC_CIR_LSERDYIE(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_CIR_LSERDYIE_SHIFT)) & RCC_CIR_LSERDYIE_MASK)

#define RCC_CIR_LSIRDYIE_SHIFT                  (8)
#define RCC_CIR_LSIRDYIE_MASK                   (0x01U << RCC_CIR_LSIRDYIE_SHIFT)
#define RCC_CIR_LSIRDYIE(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_CIR_LSIRDYIE_SHIFT)) & RCC_CIR_LSIRDYIE_MASK)

#define RCC_CIR_CSSF_SHIFT                      (7)
#define RCC_CIR_CSSF_MASK                       (0x01U << RCC_CIR_CSSF_SHIFT)
#define RCC_CIR_CSSF(x)                         (((uint32_t)(((uint32_t)(x)) << RCC_CIR_CSSF_SHIFT)) & RCC_CIR_CSSF_MASK)

#define RCC_CIR_PLL2RDYF_SHIFT                  (5)
#define RCC_CIR_PLL2RDYF_MASK                   (0x01U << RCC_CIR_PLL2RDYF_SHIFT)
#define RCC_CIR_PLL2RDYF(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_CIR_PLL2RDYF_SHIFT)) & RCC_CIR_PLL2RDYF_MASK)

#define RCC_CIR_PLL1RDYF_SHIFT                  (4)
#define RCC_CIR_PLL1RDYF_MASK                   (0x01U << RCC_CIR_PLL1RDYF_SHIFT)
#define RCC_CIR_PLL1RDYF(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_CIR_PLL1RDYF_SHIFT)) & RCC_CIR_PLL1RDYF_MASK)

#define RCC_CIR_HSERDYF_SHIFT                   (3)
#define RCC_CIR_HSERDYF_MASK                    (0x01U << RCC_CIR_HSERDYF_SHIFT)
#define RCC_CIR_HSERDYF(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_CIR_HSERDYF_SHIFT)) & RCC_CIR_HSERDYF_MASK)

#define RCC_CIR_HSIRDYF_SHIFT                   (2)
#define RCC_CIR_HSIRDYF_MASK                    (0x01U << RCC_CIR_HSIRDYF_SHIFT)
#define RCC_CIR_HSIRDYF(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_CIR_HSIRDYF_SHIFT)) & RCC_CIR_HSIRDYF_MASK)

#define RCC_CIR_LSERDYF_SHIFT                   (1)
#define RCC_CIR_LSERDYF_MASK                    (0x01U << RCC_CIR_LSERDYF_SHIFT)
#define RCC_CIR_LSERDYF(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_CIR_LSERDYF_SHIFT)) & RCC_CIR_LSERDYF_MASK)

#define RCC_CIR_LSIRDYF_SHIFT                   (0)
#define RCC_CIR_LSIRDYF_MASK                    (0x01U << RCC_CIR_LSIRDYF_SHIFT)
#define RCC_CIR_LSIRDYF(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_CIR_LSIRDYF_SHIFT)) & RCC_CIR_LSIRDYF_MASK)

/*!
 * @brief RCC_AHB1RSTR Register Bit Definition
 */

#define RCC_AHB1RSTR_QSPI_SHIFT                 (30)
#define RCC_AHB1RSTR_QSPI_MASK                  (0x01U << RCC_AHB1RSTR_QSPI_SHIFT)
#define RCC_AHB1RSTR_QSPI(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_AHB1RSTR_QSPI_SHIFT)) & RCC_AHB1RSTR_QSPI_MASK)

#define RCC_AHB1RSTR_FSMC_SHIFT                 (28)
#define RCC_AHB1RSTR_FSMC_MASK                  (0x01U << RCC_AHB1RSTR_FSMC_SHIFT)
#define RCC_AHB1RSTR_FSMC(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_AHB1RSTR_FSMC_SHIFT)) & RCC_AHB1RSTR_FSMC_MASK)

#define RCC_AHB1RSTR_USBOTGFS_SHIFT             (24)
#define RCC_AHB1RSTR_USBOTGFS_MASK              (0x01U << RCC_AHB1RSTR_USBOTGFS_SHIFT)
#define RCC_AHB1RSTR_USBOTGFS(x)                (((uint32_t)(((uint32_t)(x)) << RCC_AHB1RSTR_USBOTGFS_SHIFT)) & RCC_AHB1RSTR_USBOTGFS_MASK)

#define RCC_AHB1RSTR_DMA2_SHIFT                 (22)
#define RCC_AHB1RSTR_DMA2_MASK                  (0x01U << RCC_AHB1RSTR_DMA2_SHIFT)
#define RCC_AHB1RSTR_DMA2(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_AHB1RSTR_DMA2_SHIFT)) & RCC_AHB1RSTR_DMA2_MASK)

#define RCC_AHB1RSTR_DMA1_SHIFT                 (21)
#define RCC_AHB1RSTR_DMA1_MASK                  (0x01U << RCC_AHB1RSTR_DMA1_SHIFT)
#define RCC_AHB1RSTR_DMA1(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_AHB1RSTR_DMA1_SHIFT)) & RCC_AHB1RSTR_DMA1_MASK)

#define RCC_AHB1RSTR_ENET_SHIFT                 (19)
#define RCC_AHB1RSTR_ENET_MASK                  (0x01U << RCC_AHB1RSTR_ENET_SHIFT)
#define RCC_AHB1RSTR_ENET(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_AHB1RSTR_ENET_SHIFT)) & RCC_AHB1RSTR_ENET_MASK)

#define RCC_AHB1RSTR_CORDIC_SHIFT               (15)
#define RCC_AHB1RSTR_CORDIC_MASK                (0x01U << RCC_AHB1RSTR_CORDIC_SHIFT)
#define RCC_AHB1RSTR_CORDIC(x)                  (((uint32_t)(((uint32_t)(x)) << RCC_AHB1RSTR_CORDIC_SHIFT)) & RCC_AHB1RSTR_CORDIC_MASK)

#define RCC_AHB1RSTR_CRC_SHIFT                  (12)
#define RCC_AHB1RSTR_CRC_MASK                   (0x01U << RCC_AHB1RSTR_CRC_SHIFT)
#define RCC_AHB1RSTR_CRC(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_AHB1RSTR_CRC_SHIFT)) & RCC_AHB1RSTR_CRC_MASK)

#define RCC_AHB1RSTR_GPIOI_SHIFT                (8)
#define RCC_AHB1RSTR_GPIOI_MASK                 (0x01U << RCC_AHB1RSTR_GPIOI_SHIFT)
#define RCC_AHB1RSTR_GPIOI(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_AHB1RSTR_GPIOI_SHIFT)) & RCC_AHB1RSTR_GPIOI_MASK)

#define RCC_AHB1RSTR_GPIOH_SHIFT                (7)
#define RCC_AHB1RSTR_GPIOH_MASK                 (0x01U << RCC_AHB1RSTR_GPIOH_SHIFT)
#define RCC_AHB1RSTR_GPIOH(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_AHB1RSTR_GPIOH_SHIFT)) & RCC_AHB1RSTR_GPIOH_MASK)

#define RCC_AHB1RSTR_GPIOG_SHIFT                (6)
#define RCC_AHB1RSTR_GPIOG_MASK                 (0x01U << RCC_AHB1RSTR_GPIOG_SHIFT)
#define RCC_AHB1RSTR_GPIOG(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_AHB1RSTR_GPIOG_SHIFT)) & RCC_AHB1RSTR_GPIOG_MASK)

#define RCC_AHB1RSTR_GPIOF_SHIFT                (5)
#define RCC_AHB1RSTR_GPIOF_MASK                 (0x01U << RCC_AHB1RSTR_GPIOF_SHIFT)
#define RCC_AHB1RSTR_GPIOF(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_AHB1RSTR_GPIOF_SHIFT)) & RCC_AHB1RSTR_GPIOF_MASK)

#define RCC_AHB1RSTR_GPIOE_SHIFT                (4)
#define RCC_AHB1RSTR_GPIOE_MASK                 (0x01U << RCC_AHB1RSTR_GPIOE_SHIFT)
#define RCC_AHB1RSTR_GPIOE(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_AHB1RSTR_GPIOE_SHIFT)) & RCC_AHB1RSTR_GPIOE_MASK)

#define RCC_AHB1RSTR_GPIOD_SHIFT                (3)
#define RCC_AHB1RSTR_GPIOD_MASK                 (0x01U << RCC_AHB1RSTR_GPIOD_SHIFT)
#define RCC_AHB1RSTR_GPIOD(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_AHB1RSTR_GPIOD_SHIFT)) & RCC_AHB1RSTR_GPIOD_MASK)

#define RCC_AHB1RSTR_GPIOC_SHIFT                (2)
#define RCC_AHB1RSTR_GPIOC_MASK                 (0x01U << RCC_AHB1RSTR_GPIOC_SHIFT)
#define RCC_AHB1RSTR_GPIOC(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_AHB1RSTR_GPIOC_SHIFT)) & RCC_AHB1RSTR_GPIOC_MASK)

#define RCC_AHB1RSTR_GPIOB_SHIFT                (1)
#define RCC_AHB1RSTR_GPIOB_MASK                 (0x01U << RCC_AHB1RSTR_GPIOB_SHIFT)
#define RCC_AHB1RSTR_GPIOB(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_AHB1RSTR_GPIOB_SHIFT)) & RCC_AHB1RSTR_GPIOB_MASK)

#define RCC_AHB1RSTR_GPIOA_SHIFT                (0)
#define RCC_AHB1RSTR_GPIOA_MASK                 (0x01U << RCC_AHB1RSTR_GPIOA_SHIFT)
#define RCC_AHB1RSTR_GPIOA(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_AHB1RSTR_GPIOA_SHIFT)) & RCC_AHB1RSTR_GPIOA_MASK)

/*!
 * @brief RCC_APB2RSTR Register Bit Definition
 */

#define RCC_APB2RSTR_LPUART_SHIFT               (31)
#define RCC_APB2RSTR_LPUART_MASK                (0x01U << RCC_APB2RSTR_LPUART_SHIFT)
#define RCC_APB2RSTR_LPUART(x)                  (((uint32_t)(((uint32_t)(x)) << RCC_APB2RSTR_LPUART_SHIFT)) & RCC_APB2RSTR_LPUART_MASK)

#define RCC_APB2RSTR_LPTIM_SHIFT                (30)
#define RCC_APB2RSTR_LPTIM_MASK                 (0x01U << RCC_APB2RSTR_LPTIM_SHIFT)
#define RCC_APB2RSTR_LPTIM(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_APB2RSTR_LPTIM_SHIFT)) & RCC_APB2RSTR_LPTIM_MASK)

#define RCC_APB2RSTR_MINDSWITCH_SHIFT           (28)
#define RCC_APB2RSTR_MINDSWITCH_MASK            (0x01U << RCC_APB2RSTR_MINDSWITCH_SHIFT)
#define RCC_APB2RSTR_MINDSWITCH(x)              (((uint32_t)(((uint32_t)(x)) << RCC_APB2RSTR_MINDSWITCH_SHIFT)) & RCC_APB2RSTR_MINDSWITCH_MASK)

#define RCC_APB2RSTR_FLEXCAN2_SHIFT             (24)
#define RCC_APB2RSTR_FLEXCAN2_MASK              (0x01U << RCC_APB2RSTR_FLEXCAN2_SHIFT)
#define RCC_APB2RSTR_FLEXCAN2(x)                (((uint32_t)(((uint32_t)(x)) << RCC_APB2RSTR_FLEXCAN2_SHIFT)) & RCC_APB2RSTR_FLEXCAN2_MASK)

#define RCC_APB2RSTR_COMP_SHIFT                 (15)
#define RCC_APB2RSTR_COMP_MASK                  (0x01U << RCC_APB2RSTR_COMP_SHIFT)
#define RCC_APB2RSTR_COMP(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB2RSTR_COMP_SHIFT)) & RCC_APB2RSTR_COMP_MASK)

#define RCC_APB2RSTR_SYSCFG_SHIFT               (14)
#define RCC_APB2RSTR_SYSCFG_MASK                (0x01U << RCC_APB2RSTR_SYSCFG_SHIFT)
#define RCC_APB2RSTR_SYSCFG(x)                  (((uint32_t)(((uint32_t)(x)) << RCC_APB2RSTR_SYSCFG_SHIFT)) & RCC_APB2RSTR_SYSCFG_MASK)

#define RCC_APB2RSTR_SPI1_SHIFT                 (12)
#define RCC_APB2RSTR_SPI1_MASK                  (0x01U << RCC_APB2RSTR_SPI1_SHIFT)
#define RCC_APB2RSTR_SPI1(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB2RSTR_SPI1_SHIFT)) & RCC_APB2RSTR_SPI1_MASK)

#define RCC_APB2RSTR_ADC2_SHIFT                 (9)
#define RCC_APB2RSTR_ADC2_MASK                  (0x01U << RCC_APB2RSTR_ADC2_SHIFT)
#define RCC_APB2RSTR_ADC2(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB2RSTR_ADC2_SHIFT)) & RCC_APB2RSTR_ADC2_MASK)

#define RCC_APB2RSTR_ADC1_SHIFT                 (8)
#define RCC_APB2RSTR_ADC1_MASK                  (0x01U << RCC_APB2RSTR_ADC1_SHIFT)
#define RCC_APB2RSTR_ADC1(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB2RSTR_ADC1_SHIFT)) & RCC_APB2RSTR_ADC1_MASK)

#define RCC_APB2RSTR_UART6_SHIFT                (5)
#define RCC_APB2RSTR_UART6_MASK                 (0x01U << RCC_APB2RSTR_UART6_SHIFT)
#define RCC_APB2RSTR_UART6(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_APB2RSTR_UART6_SHIFT)) & RCC_APB2RSTR_UART6_MASK)

#define RCC_APB2RSTR_UART1_SHIFT                (4)
#define RCC_APB2RSTR_UART1_MASK                 (0x01U << RCC_APB2RSTR_UART1_SHIFT)
#define RCC_APB2RSTR_UART1(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_APB2RSTR_UART1_SHIFT)) & RCC_APB2RSTR_UART1_MASK)

#define RCC_APB2RSTR_TIM8_SHIFT                 (1)
#define RCC_APB2RSTR_TIM8_MASK                  (0x01U << RCC_APB2RSTR_TIM8_SHIFT)
#define RCC_APB2RSTR_TIM8(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB2RSTR_TIM8_SHIFT)) & RCC_APB2RSTR_TIM8_MASK)

#define RCC_APB2RSTR_TIM1_SHIFT                 (0)
#define RCC_APB2RSTR_TIM1_MASK                  (0x01U << RCC_APB2RSTR_TIM1_SHIFT)
#define RCC_APB2RSTR_TIM1(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB2RSTR_TIM1_SHIFT)) & RCC_APB2RSTR_TIM1_MASK)

/*!
 * @brief RCC_APB1RSTR Register Bit Definition
 */

#define RCC_APB1RSTR_UART7_SHIFT                (30)
#define RCC_APB1RSTR_UART7_MASK                 (0x01U << RCC_APB1RSTR_UART7_SHIFT)
#define RCC_APB1RSTR_UART7(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_UART7_SHIFT)) & RCC_APB1RSTR_UART7_MASK)

#define RCC_APB1RSTR_DAC_SHIFT                  (29)
#define RCC_APB1RSTR_DAC_MASK                   (0x01U << RCC_APB1RSTR_DAC_SHIFT)
#define RCC_APB1RSTR_DAC(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_DAC_SHIFT)) & RCC_APB1RSTR_DAC_MASK)

#define RCC_APB1RSTR_PWR_SHIFT                  (28)
#define RCC_APB1RSTR_PWR_MASK                   (0x01U << RCC_APB1RSTR_PWR_SHIFT)
#define RCC_APB1RSTR_PWR(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_PWR_SHIFT)) & RCC_APB1RSTR_PWR_MASK)

#define RCC_APB1RSTR_BKP_SHIFT                  (27)
#define RCC_APB1RSTR_BKP_MASK                   (0x01U << RCC_APB1RSTR_BKP_SHIFT)
#define RCC_APB1RSTR_BKP(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_BKP_SHIFT)) & RCC_APB1RSTR_BKP_MASK)

#define RCC_APB1RSTR_DBG_SHIFT                  (26)
#define RCC_APB1RSTR_DBG_MASK                   (0x01U << RCC_APB1RSTR_DBG_SHIFT)
#define RCC_APB1RSTR_DBG(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_DBG_SHIFT)) & RCC_APB1RSTR_DBG_MASK)

#define RCC_APB1RSTR_FLEXCAN1_SHIFT             (25)
#define RCC_APB1RSTR_FLEXCAN1_MASK              (0x01U << RCC_APB1RSTR_FLEXCAN1_SHIFT)
#define RCC_APB1RSTR_FLEXCAN1(x)                (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_FLEXCAN1_SHIFT)) & RCC_APB1RSTR_FLEXCAN1_MASK)

#define RCC_APB1RSTR_CRS_SHIFT                  (24)
#define RCC_APB1RSTR_CRS_MASK                   (0x01U << RCC_APB1RSTR_CRS_SHIFT)
#define RCC_APB1RSTR_CRS(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_CRS_SHIFT)) & RCC_APB1RSTR_CRS_MASK)

#define RCC_APB1RSTR_I2C2_SHIFT                 (22)
#define RCC_APB1RSTR_I2C2_MASK                  (0x01U << RCC_APB1RSTR_I2C2_SHIFT)
#define RCC_APB1RSTR_I2C2(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_I2C2_SHIFT)) & RCC_APB1RSTR_I2C2_MASK)

#define RCC_APB1RSTR_I2C1_SHIFT                 (21)
#define RCC_APB1RSTR_I2C1_MASK                  (0x01U << RCC_APB1RSTR_I2C1_SHIFT)
#define RCC_APB1RSTR_I2C1(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_I2C1_SHIFT)) & RCC_APB1RSTR_I2C1_MASK)

#define RCC_APB1RSTR_UART5_SHIFT                (20)
#define RCC_APB1RSTR_UART5_MASK                 (0x01U << RCC_APB1RSTR_UART5_SHIFT)
#define RCC_APB1RSTR_UART5(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_UART5_SHIFT)) & RCC_APB1RSTR_UART5_MASK)

#define RCC_APB1RSTR_UART4_SHIFT                (19)
#define RCC_APB1RSTR_UART4_MASK                 (0x01U << RCC_APB1RSTR_UART4_SHIFT)
#define RCC_APB1RSTR_UART4(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_UART4_SHIFT)) & RCC_APB1RSTR_UART4_MASK)

#define RCC_APB1RSTR_UART3_SHIFT                (18)
#define RCC_APB1RSTR_UART3_MASK                 (0x01U << RCC_APB1RSTR_UART3_SHIFT)
#define RCC_APB1RSTR_UART3(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_UART3_SHIFT)) & RCC_APB1RSTR_UART3_MASK)

#define RCC_APB1RSTR_UART2_SHIFT                (17)
#define RCC_APB1RSTR_UART2_MASK                 (0x01U << RCC_APB1RSTR_UART2_SHIFT)
#define RCC_APB1RSTR_UART2(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_UART2_SHIFT)) & RCC_APB1RSTR_UART2_MASK)

#define RCC_APB1RSTR_SPI3_SHIFT                 (15)
#define RCC_APB1RSTR_SPI3_MASK                  (0x01U << RCC_APB1RSTR_SPI3_SHIFT)
#define RCC_APB1RSTR_SPI3(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_SPI3_SHIFT)) & RCC_APB1RSTR_SPI3_MASK)

#define RCC_APB1RSTR_SPI2_SHIFT                 (14)
#define RCC_APB1RSTR_SPI2_MASK                  (0x01U << RCC_APB1RSTR_SPI2_SHIFT)
#define RCC_APB1RSTR_SPI2(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_SPI2_SHIFT)) & RCC_APB1RSTR_SPI2_MASK)

#define RCC_APB1RSTR_WWDG_SHIFT                 (11)
#define RCC_APB1RSTR_WWDG_MASK                  (0x01U << RCC_APB1RSTR_WWDG_SHIFT)
#define RCC_APB1RSTR_WWDG(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_WWDG_SHIFT)) & RCC_APB1RSTR_WWDG_MASK)

#define RCC_APB1RSTR_TIM7_SHIFT                 (5)
#define RCC_APB1RSTR_TIM7_MASK                  (0x01U << RCC_APB1RSTR_TIM7_SHIFT)
#define RCC_APB1RSTR_TIM7(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_TIM7_SHIFT)) & RCC_APB1RSTR_TIM7_MASK)

#define RCC_APB1RSTR_TIM6_SHIFT                 (4)
#define RCC_APB1RSTR_TIM6_MASK                  (0x01U << RCC_APB1RSTR_TIM6_SHIFT)
#define RCC_APB1RSTR_TIM6(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_TIM6_SHIFT)) & RCC_APB1RSTR_TIM6_MASK)

#define RCC_APB1RSTR_TIM5_SHIFT                 (3)
#define RCC_APB1RSTR_TIM5_MASK                  (0x01U << RCC_APB1RSTR_TIM5_SHIFT)
#define RCC_APB1RSTR_TIM5(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_TIM5_SHIFT)) & RCC_APB1RSTR_TIM5_MASK)

#define RCC_APB1RSTR_TIM4_SHIFT                 (2)
#define RCC_APB1RSTR_TIM4_MASK                  (0x01U << RCC_APB1RSTR_TIM4_SHIFT)
#define RCC_APB1RSTR_TIM4(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_TIM4_SHIFT)) & RCC_APB1RSTR_TIM4_MASK)

#define RCC_APB1RSTR_TIM3_SHIFT                 (1)
#define RCC_APB1RSTR_TIM3_MASK                  (0x01U << RCC_APB1RSTR_TIM3_SHIFT)
#define RCC_APB1RSTR_TIM3(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_TIM3_SHIFT)) & RCC_APB1RSTR_TIM3_MASK)

#define RCC_APB1RSTR_TIM2_SHIFT                 (0)
#define RCC_APB1RSTR_TIM2_MASK                  (0x01U << RCC_APB1RSTR_TIM2_SHIFT)
#define RCC_APB1RSTR_TIM2(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB1RSTR_TIM2_SHIFT)) & RCC_APB1RSTR_TIM2_MASK)

/*!
 * @brief RCC_AHB1ENR Register Bit Definition
 */

#define RCC_AHB1ENR_QSPI_SHIFT                  (30)
#define RCC_AHB1ENR_QSPI_MASK                   (0x01U << RCC_AHB1ENR_QSPI_SHIFT)
#define RCC_AHB1ENR_QSPI(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_QSPI_SHIFT)) & RCC_AHB1ENR_QSPI_MASK)

#define RCC_AHB1ENR_FSMC_SHIFT                  (28)
#define RCC_AHB1ENR_FSMC_MASK                   (0x01U << RCC_AHB1ENR_FSMC_SHIFT)
#define RCC_AHB1ENR_FSMC(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_FSMC_SHIFT)) & RCC_AHB1ENR_FSMC_MASK)

#define RCC_AHB1ENR_USBOTGFS_SHIFT              (24)
#define RCC_AHB1ENR_USBOTGFS_MASK               (0x01U << RCC_AHB1ENR_USBOTGFS_SHIFT)
#define RCC_AHB1ENR_USBOTGFS(x)                 (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_USBOTGFS_SHIFT)) & RCC_AHB1ENR_USBOTGFS_MASK)

#define RCC_AHB1ENR_DMA2_SHIFT                  (22)
#define RCC_AHB1ENR_DMA2_MASK                   (0x01U << RCC_AHB1ENR_DMA2_SHIFT)
#define RCC_AHB1ENR_DMA2(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_DMA2_SHIFT)) & RCC_AHB1ENR_DMA2_MASK)

#define RCC_AHB1ENR_DMA1_SHIFT                  (21)
#define RCC_AHB1ENR_DMA1_MASK                   (0x01U << RCC_AHB1ENR_DMA1_SHIFT)
#define RCC_AHB1ENR_DMA1(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_DMA1_SHIFT)) & RCC_AHB1ENR_DMA1_MASK)

#define RCC_AHB1ENR_ENET_SHIFT                  (19)
#define RCC_AHB1ENR_ENET_MASK                   (0x01U << RCC_AHB1ENR_ENET_SHIFT)
#define RCC_AHB1ENR_ENET(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_ENET_SHIFT)) & RCC_AHB1ENR_ENET_MASK)

#define RCC_AHB1ENR_DTCM_SHIFT                  (17)
#define RCC_AHB1ENR_DTCM_MASK                   (0x01U << RCC_AHB1ENR_DTCM_SHIFT)
#define RCC_AHB1ENR_DTCM(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_DTCM_SHIFT)) & RCC_AHB1ENR_DTCM_MASK)

#define RCC_AHB1ENR_ITCM_SHIFT                  (16)
#define RCC_AHB1ENR_ITCM_MASK                   (0x01U << RCC_AHB1ENR_ITCM_SHIFT)
#define RCC_AHB1ENR_ITCM(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_ITCM_SHIFT)) & RCC_AHB1ENR_ITCM_MASK)

#define RCC_AHB1ENR_CORDIC_SHIFT                (15)
#define RCC_AHB1ENR_CORDIC_MASK                 (0x01U << RCC_AHB1ENR_CORDIC_SHIFT)
#define RCC_AHB1ENR_CORDIC(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_CORDIC_SHIFT)) & RCC_AHB1ENR_CORDIC_MASK)

#define RCC_AHB1ENR_SRAM_SHIFT                  (14)
#define RCC_AHB1ENR_SRAM_MASK                   (0x01U << RCC_AHB1ENR_SRAM_SHIFT)
#define RCC_AHB1ENR_SRAM(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_SRAM_SHIFT)) & RCC_AHB1ENR_SRAM_MASK)

#define RCC_AHB1ENR_FLASH_SHIFT                 (13)
#define RCC_AHB1ENR_FLASH_MASK                  (0x01U << RCC_AHB1ENR_FLASH_SHIFT)
#define RCC_AHB1ENR_FLASH(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_FLASH_SHIFT)) & RCC_AHB1ENR_FLASH_MASK)

#define RCC_AHB1ENR_CRC_SHIFT                   (12)
#define RCC_AHB1ENR_CRC_MASK                    (0x01U << RCC_AHB1ENR_CRC_SHIFT)
#define RCC_AHB1ENR_CRC(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_CRC_SHIFT)) & RCC_AHB1ENR_CRC_MASK)

#define RCC_AHB1ENR_GPIOI_SHIFT                 (8)
#define RCC_AHB1ENR_GPIOI_MASK                  (0x01U << RCC_AHB1ENR_GPIOI_SHIFT)
#define RCC_AHB1ENR_GPIOI(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_GPIOI_SHIFT)) & RCC_AHB1ENR_GPIOI_MASK)

#define RCC_AHB1ENR_GPIOH_SHIFT                 (7)
#define RCC_AHB1ENR_GPIOH_MASK                  (0x01U << RCC_AHB1ENR_GPIOH_SHIFT)
#define RCC_AHB1ENR_GPIOH(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_GPIOH_SHIFT)) & RCC_AHB1ENR_GPIOH_MASK)

#define RCC_AHB1ENR_GPIOG_SHIFT                 (6)
#define RCC_AHB1ENR_GPIOG_MASK                  (0x01U << RCC_AHB1ENR_GPIOG_SHIFT)
#define RCC_AHB1ENR_GPIOG(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_GPIOG_SHIFT)) & RCC_AHB1ENR_GPIOG_MASK)

#define RCC_AHB1ENR_GPIOF_SHIFT                 (5)
#define RCC_AHB1ENR_GPIOF_MASK                  (0x01U << RCC_AHB1ENR_GPIOF_SHIFT)
#define RCC_AHB1ENR_GPIOF(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_GPIOF_SHIFT)) & RCC_AHB1ENR_GPIOF_MASK)

#define RCC_AHB1ENR_GPIOE_SHIFT                 (4)
#define RCC_AHB1ENR_GPIOE_MASK                  (0x01U << RCC_AHB1ENR_GPIOE_SHIFT)
#define RCC_AHB1ENR_GPIOE(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_GPIOE_SHIFT)) & RCC_AHB1ENR_GPIOE_MASK)

#define RCC_AHB1ENR_GPIOD_SHIFT                 (3)
#define RCC_AHB1ENR_GPIOD_MASK                  (0x01U << RCC_AHB1ENR_GPIOD_SHIFT)
#define RCC_AHB1ENR_GPIOD(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_GPIOD_SHIFT)) & RCC_AHB1ENR_GPIOD_MASK)

#define RCC_AHB1ENR_GPIOC_SHIFT                 (2)
#define RCC_AHB1ENR_GPIOC_MASK                  (0x01U << RCC_AHB1ENR_GPIOC_SHIFT)
#define RCC_AHB1ENR_GPIOC(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_GPIOC_SHIFT)) & RCC_AHB1ENR_GPIOC_MASK)

#define RCC_AHB1ENR_GPIOB_SHIFT                 (1)
#define RCC_AHB1ENR_GPIOB_MASK                  (0x01U << RCC_AHB1ENR_GPIOB_SHIFT)
#define RCC_AHB1ENR_GPIOB(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_GPIOB_SHIFT)) & RCC_AHB1ENR_GPIOB_MASK)

#define RCC_AHB1ENR_GPIOA_SHIFT                 (0)
#define RCC_AHB1ENR_GPIOA_MASK                  (0x01U << RCC_AHB1ENR_GPIOA_SHIFT)
#define RCC_AHB1ENR_GPIOA(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_AHB1ENR_GPIOA_SHIFT)) & RCC_AHB1ENR_GPIOA_MASK)

/*!
 * @brief RCC_APB2ENR Register Bit Definition
 */

#define RCC_APB2ENR_LPUART_SHIFT                (31)
#define RCC_APB2ENR_LPUART_MASK                 (0x01U << RCC_APB2ENR_LPUART_SHIFT)
#define RCC_APB2ENR_LPUART(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_APB2ENR_LPUART_SHIFT)) & RCC_APB2ENR_LPUART_MASK)

#define RCC_APB2ENR_LPTIM_SHIFT                 (30)
#define RCC_APB2ENR_LPTIM_MASK                  (0x01U << RCC_APB2ENR_LPTIM_SHIFT)
#define RCC_APB2ENR_LPTIM(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB2ENR_LPTIM_SHIFT)) & RCC_APB2ENR_LPTIM_MASK)

#define RCC_APB2ENR_MINDSWITCH_SHIFT            (28)
#define RCC_APB2ENR_MINDSWITCH_MASK             (0x01U << RCC_APB2ENR_MINDSWITCH_SHIFT)
#define RCC_APB2ENR_MINDSWITCH(x)               (((uint32_t)(((uint32_t)(x)) << RCC_APB2ENR_MINDSWITCH_SHIFT)) & RCC_APB2ENR_MINDSWITCH_MASK)

#define RCC_APB2ENR_FLEXCAN2_SHIFT              (24)
#define RCC_APB2ENR_FLEXCAN2_MASK               (0x01U << RCC_APB2ENR_FLEXCAN2_SHIFT)
#define RCC_APB2ENR_FLEXCAN2(x)                 (((uint32_t)(((uint32_t)(x)) << RCC_APB2ENR_FLEXCAN2_SHIFT)) & RCC_APB2ENR_FLEXCAN2_MASK)

#define RCC_APB2ENR_COMP_SHIFT                  (15)
#define RCC_APB2ENR_COMP_MASK                   (0x01U << RCC_APB2ENR_COMP_SHIFT)
#define RCC_APB2ENR_COMP(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB2ENR_COMP_SHIFT)) & RCC_APB2ENR_COMP_MASK)

#define RCC_APB2ENR_SYSCFG_SHIFT                (14)
#define RCC_APB2ENR_SYSCFG_MASK                 (0x01U << RCC_APB2ENR_SYSCFG_SHIFT)
#define RCC_APB2ENR_SYSCFG(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_APB2ENR_SYSCFG_SHIFT)) & RCC_APB2ENR_SYSCFG_MASK)

#define RCC_APB2ENR_SPI1_SHIFT                  (12)
#define RCC_APB2ENR_SPI1_MASK                   (0x01U << RCC_APB2ENR_SPI1_SHIFT)
#define RCC_APB2ENR_SPI1(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB2ENR_SPI1_SHIFT)) & RCC_APB2ENR_SPI1_MASK)

#define RCC_APB2ENR_ADC2_SHIFT                  (9)
#define RCC_APB2ENR_ADC2_MASK                   (0x01U << RCC_APB2ENR_ADC2_SHIFT)
#define RCC_APB2ENR_ADC2(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB2ENR_ADC2_SHIFT)) & RCC_APB2ENR_ADC2_MASK)

#define RCC_APB2ENR_ADC1_SHIFT                  (8)
#define RCC_APB2ENR_ADC1_MASK                   (0x01U << RCC_APB2ENR_ADC1_SHIFT)
#define RCC_APB2ENR_ADC1(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB2ENR_ADC1_SHIFT)) & RCC_APB2ENR_ADC1_MASK)

#define RCC_APB2ENR_UART6_SHIFT                 (5)
#define RCC_APB2ENR_UART6_MASK                  (0x01U << RCC_APB2ENR_UART6_SHIFT)
#define RCC_APB2ENR_UART6(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB2ENR_UART6_SHIFT)) & RCC_APB2ENR_UART6_MASK)

#define RCC_APB2ENR_UART1_SHIFT                 (4)
#define RCC_APB2ENR_UART1_MASK                  (0x01U << RCC_APB2ENR_UART1_SHIFT)
#define RCC_APB2ENR_UART1(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB2ENR_UART1_SHIFT)) & RCC_APB2ENR_UART1_MASK)

#define RCC_APB2ENR_TIM8_SHIFT                  (1)
#define RCC_APB2ENR_TIM8_MASK                   (0x01U << RCC_APB2ENR_TIM8_SHIFT)
#define RCC_APB2ENR_TIM8(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB2ENR_TIM8_SHIFT)) & RCC_APB2ENR_TIM8_MASK)

#define RCC_APB2ENR_TIM1_SHIFT                  (0)
#define RCC_APB2ENR_TIM1_MASK                   (0x01U << RCC_APB2ENR_TIM1_SHIFT)
#define RCC_APB2ENR_TIM1(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB2ENR_TIM1_SHIFT)) & RCC_APB2ENR_TIM1_MASK)

/*!
 * @brief RCC_APB1ENR Register Bit Definition
 */

#define RCC_APB1ENR_UART7_SHIFT                 (30)
#define RCC_APB1ENR_UART7_MASK                  (0x01U << RCC_APB1ENR_UART7_SHIFT)
#define RCC_APB1ENR_UART7(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_UART7_SHIFT)) & RCC_APB1ENR_UART7_MASK)

#define RCC_APB1ENR_DAC_SHIFT                   (29)
#define RCC_APB1ENR_DAC_MASK                    (0x01U << RCC_APB1ENR_DAC_SHIFT)
#define RCC_APB1ENR_DAC(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_DAC_SHIFT)) & RCC_APB1ENR_DAC_MASK)

#define RCC_APB1ENR_PWRDBG_SHIFT                (28)
#define RCC_APB1ENR_PWRDBG_MASK                 (0x01U << RCC_APB1ENR_PWRDBG_SHIFT)
#define RCC_APB1ENR_PWRDBG(x)                   (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_PWRDBG_SHIFT)) & RCC_APB1ENR_PWRDBG_MASK)

#define RCC_APB1ENR_BKP_SHIFT                   (27)
#define RCC_APB1ENR_BKP_MASK                    (0x01U << RCC_APB1ENR_BKP_SHIFT)
#define RCC_APB1ENR_BKP(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_BKP_SHIFT)) & RCC_APB1ENR_BKP_MASK)

#define RCC_APB1ENR_FLEXCAN1_SHIFT              (25)
#define RCC_APB1ENR_FLEXCAN1_MASK               (0x01U << RCC_APB1ENR_FLEXCAN1_SHIFT)
#define RCC_APB1ENR_FLEXCAN1(x)                 (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_FLEXCAN1_SHIFT)) & RCC_APB1ENR_FLEXCAN1_MASK)

#define RCC_APB1ENR_CRS_SHIFT                   (24)
#define RCC_APB1ENR_CRS_MASK                    (0x01U << RCC_APB1ENR_CRS_SHIFT)
#define RCC_APB1ENR_CRS(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_CRS_SHIFT)) & RCC_APB1ENR_CRS_MASK)

#define RCC_APB1ENR_I2C2_SHIFT                  (22)
#define RCC_APB1ENR_I2C2_MASK                   (0x01U << RCC_APB1ENR_I2C2_SHIFT)
#define RCC_APB1ENR_I2C2(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_I2C2_SHIFT)) & RCC_APB1ENR_I2C2_MASK)

#define RCC_APB1ENR_I2C1_SHIFT                  (21)
#define RCC_APB1ENR_I2C1_MASK                   (0x01U << RCC_APB1ENR_I2C1_SHIFT)
#define RCC_APB1ENR_I2C1(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_I2C1_SHIFT)) & RCC_APB1ENR_I2C1_MASK)

#define RCC_APB1ENR_UART5_SHIFT                 (20)
#define RCC_APB1ENR_UART5_MASK                  (0x01U << RCC_APB1ENR_UART5_SHIFT)
#define RCC_APB1ENR_UART5(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_UART5_SHIFT)) & RCC_APB1ENR_UART5_MASK)

#define RCC_APB1ENR_UART4_SHIFT                 (19)
#define RCC_APB1ENR_UART4_MASK                  (0x01U << RCC_APB1ENR_UART4_SHIFT)
#define RCC_APB1ENR_UART4(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_UART4_SHIFT)) & RCC_APB1ENR_UART4_MASK)

#define RCC_APB1ENR_UART3_SHIFT                 (18)
#define RCC_APB1ENR_UART3_MASK                  (0x01U << RCC_APB1ENR_UART3_SHIFT)
#define RCC_APB1ENR_UART3(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_UART3_SHIFT)) & RCC_APB1ENR_UART3_MASK)

#define RCC_APB1ENR_UART2_SHIFT                 (17)
#define RCC_APB1ENR_UART2_MASK                  (0x01U << RCC_APB1ENR_UART2_SHIFT)
#define RCC_APB1ENR_UART2(x)                    (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_UART2_SHIFT)) & RCC_APB1ENR_UART2_MASK)

#define RCC_APB1ENR_SPI3_SHIFT                  (15)
#define RCC_APB1ENR_SPI3_MASK                   (0x01U << RCC_APB1ENR_SPI3_SHIFT)
#define RCC_APB1ENR_SPI3(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_SPI3_SHIFT)) & RCC_APB1ENR_SPI3_MASK)

#define RCC_APB1ENR_SPI2_SHIFT                  (14)
#define RCC_APB1ENR_SPI2_MASK                   (0x01U << RCC_APB1ENR_SPI2_SHIFT)
#define RCC_APB1ENR_SPI2(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_SPI2_SHIFT)) & RCC_APB1ENR_SPI2_MASK)

#define RCC_APB1ENR_WWDG_SHIFT                  (11)
#define RCC_APB1ENR_WWDG_MASK                   (0x01U << RCC_APB1ENR_WWDG_SHIFT)
#define RCC_APB1ENR_WWDG(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_WWDG_SHIFT)) & RCC_APB1ENR_WWDG_MASK)

#define RCC_APB1ENR_TIM7_SHIFT                  (5)
#define RCC_APB1ENR_TIM7_MASK                   (0x01U << RCC_APB1ENR_TIM7_SHIFT)
#define RCC_APB1ENR_TIM7(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_TIM7_SHIFT)) & RCC_APB1ENR_TIM7_MASK)

#define RCC_APB1ENR_TIM6_SHIFT                  (4)
#define RCC_APB1ENR_TIM6_MASK                   (0x01U << RCC_APB1ENR_TIM6_SHIFT)
#define RCC_APB1ENR_TIM6(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_TIM6_SHIFT)) & RCC_APB1ENR_TIM6_MASK)

#define RCC_APB1ENR_TIM5_SHIFT                  (3)
#define RCC_APB1ENR_TIM5_MASK                   (0x01U << RCC_APB1ENR_TIM5_SHIFT)
#define RCC_APB1ENR_TIM5(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_TIM5_SHIFT)) & RCC_APB1ENR_TIM5_MASK)

#define RCC_APB1ENR_TIM4_SHIFT                  (2)
#define RCC_APB1ENR_TIM4_MASK                   (0x01U << RCC_APB1ENR_TIM4_SHIFT)
#define RCC_APB1ENR_TIM4(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_TIM4_SHIFT)) & RCC_APB1ENR_TIM4_MASK)

#define RCC_APB1ENR_TIM3_SHIFT                  (1)
#define RCC_APB1ENR_TIM3_MASK                   (0x01U << RCC_APB1ENR_TIM3_SHIFT)
#define RCC_APB1ENR_TIM3(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_TIM3_SHIFT)) & RCC_APB1ENR_TIM3_MASK)

#define RCC_APB1ENR_TIM2_SHIFT                  (0)
#define RCC_APB1ENR_TIM2_MASK                   (0x01U << RCC_APB1ENR_TIM2_SHIFT)
#define RCC_APB1ENR_TIM2(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_APB1ENR_TIM2_SHIFT)) & RCC_APB1ENR_TIM2_MASK)

/*!
 * @brief RCC_BDCR Register Bit Definition
 */

#define RCC_BDCR_DBP_SHIFT                      (24)
#define RCC_BDCR_DBP_MASK                       (0x01U << RCC_BDCR_DBP_SHIFT)
#define RCC_BDCR_DBP(x)                         (((uint32_t)(((uint32_t)(x)) << RCC_BDCR_DBP_SHIFT)) & RCC_BDCR_DBP_MASK)

#define RCC_BDCR_BDRST_SHIFT                    (16)
#define RCC_BDCR_BDRST_MASK                     (0x01U << RCC_BDCR_BDRST_SHIFT)
#define RCC_BDCR_BDRST(x)                       (((uint32_t)(((uint32_t)(x)) << RCC_BDCR_BDRST_SHIFT)) & RCC_BDCR_BDRST_MASK)

#define RCC_BDCR_RTCEN_SHIFT                    (15)
#define RCC_BDCR_RTCEN_MASK                     (0x01U << RCC_BDCR_RTCEN_SHIFT)
#define RCC_BDCR_RTCEN(x)                       (((uint32_t)(((uint32_t)(x)) << RCC_BDCR_RTCEN_SHIFT)) & RCC_BDCR_RTCEN_MASK)

#define RCC_BDCR_RTCSEL_SHIFT                   (8)
#define RCC_BDCR_RTCSEL_MASK                    (0x3U << RCC_BDCR_RTCSEL_SHIFT)
#define RCC_BDCR_RTCSEL(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_BDCR_RTCSEL_SHIFT)) & RCC_BDCR_RTCSEL_MASK)

#define RCC_BDCR_LSEBYP_SHIFT                   (2)
#define RCC_BDCR_LSEBYP_MASK                    (0x01U << RCC_BDCR_LSEBYP_SHIFT)
#define RCC_BDCR_LSEBYP(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_BDCR_LSEBYP_SHIFT)) & RCC_BDCR_LSEBYP_MASK)

#define RCC_BDCR_LSERDY_SHIFT                   (1)
#define RCC_BDCR_LSERDY_MASK                    (0x01U << RCC_BDCR_LSERDY_SHIFT)
#define RCC_BDCR_LSERDY(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_BDCR_LSERDY_SHIFT)) & RCC_BDCR_LSERDY_MASK)

#define RCC_BDCR_LSEON_SHIFT                    (0)
#define RCC_BDCR_LSEON_MASK                     (0x01U << RCC_BDCR_LSEON_SHIFT)
#define RCC_BDCR_LSEON(x)                       (((uint32_t)(((uint32_t)(x)) << RCC_BDCR_LSEON_SHIFT)) & RCC_BDCR_LSEON_MASK)

/*!
 * @brief RCC_CSR Register Bit Definition
 */

#define RCC_CSR_LPWRRSTF_SHIFT                  (31)
#define RCC_CSR_LPWRRSTF_MASK                   (0x01U << RCC_CSR_LPWRRSTF_SHIFT)
#define RCC_CSR_LPWRRSTF(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_CSR_LPWRRSTF_SHIFT)) & RCC_CSR_LPWRRSTF_MASK)

#define RCC_CSR_WWDGRSTF_SHIFT                  (30)
#define RCC_CSR_WWDGRSTF_MASK                   (0x01U << RCC_CSR_WWDGRSTF_SHIFT)
#define RCC_CSR_WWDGRSTF(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_CSR_WWDGRSTF_SHIFT)) & RCC_CSR_WWDGRSTF_MASK)

#define RCC_CSR_IWDGRSTF_SHIFT                  (29)
#define RCC_CSR_IWDGRSTF_MASK                   (0x01U << RCC_CSR_IWDGRSTF_SHIFT)
#define RCC_CSR_IWDGRSTF(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_CSR_IWDGRSTF_SHIFT)) & RCC_CSR_IWDGRSTF_MASK)

#define RCC_CSR_SFTRSTF_SHIFT                   (28)
#define RCC_CSR_SFTRSTF_MASK                    (0x01U << RCC_CSR_SFTRSTF_SHIFT)
#define RCC_CSR_SFTRSTF(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_CSR_SFTRSTF_SHIFT)) & RCC_CSR_SFTRSTF_MASK)

#define RCC_CSR_PORRSTF_SHIFT                   (27)
#define RCC_CSR_PORRSTF_MASK                    (0x01U << RCC_CSR_PORRSTF_SHIFT)
#define RCC_CSR_PORRSTF(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_CSR_PORRSTF_SHIFT)) & RCC_CSR_PORRSTF_MASK)

#define RCC_CSR_PINRSTF_SHIFT                   (26)
#define RCC_CSR_PINRSTF_MASK                    (0x01U << RCC_CSR_PINRSTF_SHIFT)
#define RCC_CSR_PINRSTF(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_CSR_PINRSTF_SHIFT)) & RCC_CSR_PINRSTF_MASK)

#define RCC_CSR_RMVF_SHIFT                      (24)
#define RCC_CSR_RMVF_MASK                       (0x01U << RCC_CSR_RMVF_SHIFT)
#define RCC_CSR_RMVF(x)                         (((uint32_t)(((uint32_t)(x)) << RCC_CSR_RMVF_SHIFT)) & RCC_CSR_RMVF_MASK)

#define RCC_CSR_LOCKUPF_SHIFT                   (23)
#define RCC_CSR_LOCKUPF_MASK                    (0x01U << RCC_CSR_LOCKUPF_SHIFT)
#define RCC_CSR_LOCKUPF(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_CSR_LOCKUPF_SHIFT)) & RCC_CSR_LOCKUPF_MASK)

#define RCC_CSR_PVDRSTF_SHIFT                   (22)
#define RCC_CSR_PVDRSTF_MASK                    (0x01U << RCC_CSR_PVDRSTF_SHIFT)
#define RCC_CSR_PVDRSTF(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_CSR_PVDRSTF_SHIFT)) & RCC_CSR_PVDRSTF_MASK)

#define RCC_CSR_LOCKUPEN_SHIFT                  (7)
#define RCC_CSR_LOCKUPEN_MASK                   (0x01U << RCC_CSR_LOCKUPEN_SHIFT)
#define RCC_CSR_LOCKUPEN(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_CSR_LOCKUPEN_SHIFT)) & RCC_CSR_LOCKUPEN_MASK)

#define RCC_CSR_PVDRSTEN_SHIFT                  (6)
#define RCC_CSR_PVDRSTEN_MASK                   (0x01U << RCC_CSR_PVDRSTEN_SHIFT)
#define RCC_CSR_PVDRSTEN(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_CSR_PVDRSTEN_SHIFT)) & RCC_CSR_PVDRSTEN_MASK)

#define RCC_CSR_LSIOE_SHIFT                     (5)
#define RCC_CSR_LSIOE_MASK                      (0x01U << RCC_CSR_LSIOE_SHIFT)
#define RCC_CSR_LSIOE(x)                        (((uint32_t)(((uint32_t)(x)) << RCC_CSR_LSIOE_SHIFT)) & RCC_CSR_LSIOE_MASK)

#define RCC_CSR_LSIRDY_SHIFT                    (1)
#define RCC_CSR_LSIRDY_MASK                     (0x01U << RCC_CSR_LSIRDY_SHIFT)
#define RCC_CSR_LSIRDY(x)                       (((uint32_t)(((uint32_t)(x)) << RCC_CSR_LSIRDY_SHIFT)) & RCC_CSR_LSIRDY_MASK)

#define RCC_CSR_LSION_SHIFT                     (0)
#define RCC_CSR_LSION_MASK                      (0x01U << RCC_CSR_LSION_SHIFT)
#define RCC_CSR_LSION(x)                        (((uint32_t)(((uint32_t)(x)) << RCC_CSR_LSION_SHIFT)) & RCC_CSR_LSION_MASK)

/*!
 * @brief RCC_SYSCFG Register Bit Definition
 */

#define RCC_SYSCFG_HSELPFEN_SHIFT               (14)
#define RCC_SYSCFG_HSELPFEN_MASK                (0x01U << RCC_SYSCFG_HSELPFEN_SHIFT)
#define RCC_SYSCFG_HSELPFEN(x)                  (((uint32_t)(((uint32_t)(x)) << RCC_SYSCFG_HSELPFEN_SHIFT)) & RCC_SYSCFG_HSELPFEN_MASK)

#define RCC_SYSCFG_HSERFBSEL_SHIFT              (8)
#define RCC_SYSCFG_HSERFBSEL_MASK               (0x3U << RCC_SYSCFG_HSERFBSEL_SHIFT)
#define RCC_SYSCFG_HSERFBSEL(x)                 (((uint32_t)(((uint32_t)(x)) << RCC_SYSCFG_HSERFBSEL_SHIFT)) & RCC_SYSCFG_HSERFBSEL_MASK)

#define RCC_SYSCFG_SECTOR1KCFG_SHIFT            (1)
#define RCC_SYSCFG_SECTOR1KCFG_MASK             (0x01U << RCC_SYSCFG_SECTOR1KCFG_SHIFT)
#define RCC_SYSCFG_SECTOR1KCFG(x)               (((uint32_t)(((uint32_t)(x)) << RCC_SYSCFG_SECTOR1KCFG_SHIFT)) & RCC_SYSCFG_SECTOR1KCFG_MASK)

#define RCC_SYSCFG_PROGCHECKEN_SHIFT            (0)
#define RCC_SYSCFG_PROGCHECKEN_MASK             (0x01U << RCC_SYSCFG_PROGCHECKEN_SHIFT)
#define RCC_SYSCFG_PROGCHECKEN(x)               (((uint32_t)(((uint32_t)(x)) << RCC_SYSCFG_PROGCHECKEN_SHIFT)) & RCC_SYSCFG_PROGCHECKEN_MASK)

/*!
 * @brief RCC_CFGR2 Register Bit Definition
 */

#define RCC_CFGR2_LPTIMCLKSEL_SHIFT             (29)
#define RCC_CFGR2_LPTIMCLKSEL_MASK              (0x3U << RCC_CFGR2_LPTIMCLKSEL_SHIFT)
#define RCC_CFGR2_LPTIMCLKSEL(x)                (((uint32_t)(((uint32_t)(x)) << RCC_CFGR2_LPTIMCLKSEL_SHIFT)) & RCC_CFGR2_LPTIMCLKSEL_MASK)

#define RCC_CFGR2_LPUARTCLKSEL_SHIFT            (26)
#define RCC_CFGR2_LPUARTCLKSEL_MASK             (0x3U << RCC_CFGR2_LPUARTCLKSEL_SHIFT)
#define RCC_CFGR2_LPUARTCLKSEL(x)               (((uint32_t)(((uint32_t)(x)) << RCC_CFGR2_LPUARTCLKSEL_SHIFT)) & RCC_CFGR2_LPUARTCLKSEL_MASK)

#define RCC_CFGR2_MCOPRE_SHIFT                  (20)
#define RCC_CFGR2_MCOPRE_MASK                   (0xFU << RCC_CFGR2_MCOPRE_SHIFT)
#define RCC_CFGR2_MCOPRE(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_CFGR2_MCOPRE_SHIFT)) & RCC_CFGR2_MCOPRE_MASK)

#define RCC_CFGR2_APB1CLKHVPRE_SHIFT            (16)
#define RCC_CFGR2_APB1CLKHVPRE_MASK             (0xFU << RCC_CFGR2_APB1CLKHVPRE_SHIFT)
#define RCC_CFGR2_APB1CLKHVPRE(x)               (((uint32_t)(((uint32_t)(x)) << RCC_CFGR2_APB1CLKHVPRE_SHIFT)) & RCC_CFGR2_APB1CLKHVPRE_MASK)

#define RCC_CFGR2_TIMADVPRE_SHIFT               (1)
#define RCC_CFGR2_TIMADVPRE_MASK                (0x7U << RCC_CFGR2_TIMADVPRE_SHIFT)
#define RCC_CFGR2_TIMADVPRE(x)                  (((uint32_t)(((uint32_t)(x)) << RCC_CFGR2_TIMADVPRE_SHIFT)) & RCC_CFGR2_TIMADVPRE_MASK)

#define RCC_CFGR2_TIMADVCKSEL_SHIFT             (0)
#define RCC_CFGR2_TIMADVCKSEL_MASK              (0x01U << RCC_CFGR2_TIMADVCKSEL_SHIFT)
#define RCC_CFGR2_TIMADVCKSEL(x)                (((uint32_t)(((uint32_t)(x)) << RCC_CFGR2_TIMADVCKSEL_SHIFT)) & RCC_CFGR2_TIMADVCKSEL_MASK)

/*!
 * @brief RCC_PLL1CFGR Register Bit Definition
 */

#define RCC_PLL1CFGR_PLL1MUL_SHIFT              (16)
#define RCC_PLL1CFGR_PLL1MUL_MASK               (0x7FU << RCC_PLL1CFGR_PLL1MUL_SHIFT)
#define RCC_PLL1CFGR_PLL1MUL(x)                 (((uint32_t)(((uint32_t)(x)) << RCC_PLL1CFGR_PLL1MUL_SHIFT)) & RCC_PLL1CFGR_PLL1MUL_MASK)

#define RCC_PLL1CFGR_PLL1DIV_SHIFT              (8)
#define RCC_PLL1CFGR_PLL1DIV_MASK               (0x7U << RCC_PLL1CFGR_PLL1DIV_SHIFT)
#define RCC_PLL1CFGR_PLL1DIV(x)                 (((uint32_t)(((uint32_t)(x)) << RCC_PLL1CFGR_PLL1DIV_SHIFT)) & RCC_PLL1CFGR_PLL1DIV_MASK)

#define RCC_PLL1CFGR_PLL1LDS_SHIFT              (4)
#define RCC_PLL1CFGR_PLL1LDS_MASK               (0x7U << RCC_PLL1CFGR_PLL1LDS_SHIFT)
#define RCC_PLL1CFGR_PLL1LDS(x)                 (((uint32_t)(((uint32_t)(x)) << RCC_PLL1CFGR_PLL1LDS_SHIFT)) & RCC_PLL1CFGR_PLL1LDS_MASK)

#define RCC_PLL1CFGR_PLL1ICTRL_SHIFT            (2)
#define RCC_PLL1CFGR_PLL1ICTRL_MASK             (0x3U << RCC_PLL1CFGR_PLL1ICTRL_SHIFT)
#define RCC_PLL1CFGR_PLL1ICTRL(x)               (((uint32_t)(((uint32_t)(x)) << RCC_PLL1CFGR_PLL1ICTRL_SHIFT)) & RCC_PLL1CFGR_PLL1ICTRL_MASK)

#define RCC_PLL1CFGR_PLL1XTPRE_SHIFT            (1)
#define RCC_PLL1CFGR_PLL1XTPRE_MASK             (0x01U << RCC_PLL1CFGR_PLL1XTPRE_SHIFT)
#define RCC_PLL1CFGR_PLL1XTPRE(x)               (((uint32_t)(((uint32_t)(x)) << RCC_PLL1CFGR_PLL1XTPRE_SHIFT)) & RCC_PLL1CFGR_PLL1XTPRE_MASK)

#define RCC_PLL1CFGR_PLL1SRC_SHIFT              (0)
#define RCC_PLL1CFGR_PLL1SRC_MASK               (0x01U << RCC_PLL1CFGR_PLL1SRC_SHIFT)
#define RCC_PLL1CFGR_PLL1SRC(x)                 (((uint32_t)(((uint32_t)(x)) << RCC_PLL1CFGR_PLL1SRC_SHIFT)) & RCC_PLL1CFGR_PLL1SRC_MASK)

/*!
 * @brief RCC_PLL2CFGR Register Bit Definition
 */

#define RCC_PLL2CFGR_PLL2PDIV_SHIFT             (24)
#define RCC_PLL2CFGR_PLL2PDIV_MASK              (0x7U << RCC_PLL2CFGR_PLL2PDIV_SHIFT)
#define RCC_PLL2CFGR_PLL2PDIV(x)                (((uint32_t)(((uint32_t)(x)) << RCC_PLL2CFGR_PLL2PDIV_SHIFT)) & RCC_PLL2CFGR_PLL2PDIV_MASK)

#define RCC_PLL2CFGR_PLL2MUL_SHIFT              (16)
#define RCC_PLL2CFGR_PLL2MUL_MASK               (0xFFU << RCC_PLL2CFGR_PLL2MUL_SHIFT)
#define RCC_PLL2CFGR_PLL2MUL(x)                 (((uint32_t)(((uint32_t)(x)) << RCC_PLL2CFGR_PLL2MUL_SHIFT)) & RCC_PLL2CFGR_PLL2MUL_MASK)

#define RCC_PLL2CFGR_PLL2DIV_SHIFT              (8)
#define RCC_PLL2CFGR_PLL2DIV_MASK               (0x7U << RCC_PLL2CFGR_PLL2DIV_SHIFT)
#define RCC_PLL2CFGR_PLL2DIV(x)                 (((uint32_t)(((uint32_t)(x)) << RCC_PLL2CFGR_PLL2DIV_SHIFT)) & RCC_PLL2CFGR_PLL2DIV_MASK)

#define RCC_PLL2CFGR_PLL2LDS_SHIFT              (4)
#define RCC_PLL2CFGR_PLL2LDS_MASK               (0x7U << RCC_PLL2CFGR_PLL2LDS_SHIFT)
#define RCC_PLL2CFGR_PLL2LDS(x)                 (((uint32_t)(((uint32_t)(x)) << RCC_PLL2CFGR_PLL2LDS_SHIFT)) & RCC_PLL2CFGR_PLL2LDS_MASK)

#define RCC_PLL2CFGR_PLL2ICTRL_SHIFT            (2)
#define RCC_PLL2CFGR_PLL2ICTRL_MASK             (0x3U << RCC_PLL2CFGR_PLL2ICTRL_SHIFT)
#define RCC_PLL2CFGR_PLL2ICTRL(x)               (((uint32_t)(((uint32_t)(x)) << RCC_PLL2CFGR_PLL2ICTRL_SHIFT)) & RCC_PLL2CFGR_PLL2ICTRL_MASK)

#define RCC_PLL2CFGR_PLL2XTPRE_SHIFT            (1)
#define RCC_PLL2CFGR_PLL2XTPRE_MASK             (0x01U << RCC_PLL2CFGR_PLL2XTPRE_SHIFT)
#define RCC_PLL2CFGR_PLL2XTPRE(x)               (((uint32_t)(((uint32_t)(x)) << RCC_PLL2CFGR_PLL2XTPRE_SHIFT)) & RCC_PLL2CFGR_PLL2XTPRE_MASK)

#define RCC_PLL2CFGR_PLL2SRC_SHIFT              (0)
#define RCC_PLL2CFGR_PLL2SRC_MASK               (0x01U << RCC_PLL2CFGR_PLL2SRC_SHIFT)
#define RCC_PLL2CFGR_PLL2SRC(x)                 (((uint32_t)(((uint32_t)(x)) << RCC_PLL2CFGR_PLL2SRC_SHIFT)) & RCC_PLL2CFGR_PLL2SRC_MASK)

/*!
 * @brief RCC_ADC1CFGR Register Bit Definition
 */

#define RCC_ADC1CFGR_PRECAL_SHIFT               (8)
#define RCC_ADC1CFGR_PRECAL_MASK                (0x1FFU << RCC_ADC1CFGR_PRECAL_SHIFT)
#define RCC_ADC1CFGR_PRECAL(x)                  (((uint32_t)(((uint32_t)(x)) << RCC_ADC1CFGR_PRECAL_SHIFT)) & RCC_ADC1CFGR_PRECAL_MASK)

#define RCC_ADC1CFGR_PRE_SHIFT                  (0)
#define RCC_ADC1CFGR_PRE_MASK                   (0xFU << RCC_ADC1CFGR_PRE_SHIFT)
#define RCC_ADC1CFGR_PRE(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_ADC1CFGR_PRE_SHIFT)) & RCC_ADC1CFGR_PRE_MASK)

/*!
 * @brief RCC_ADC2CFGR Register Bit Definition
 */

#define RCC_ADC2CFGR_PRECAL_SHIFT               (8)
#define RCC_ADC2CFGR_PRECAL_MASK                (0x1FFU << RCC_ADC2CFGR_PRECAL_SHIFT)
#define RCC_ADC2CFGR_PRECAL(x)                  (((uint32_t)(((uint32_t)(x)) << RCC_ADC2CFGR_PRECAL_SHIFT)) & RCC_ADC2CFGR_PRECAL_MASK)

#define RCC_ADC2CFGR_PRE_SHIFT                  (0)
#define RCC_ADC2CFGR_PRE_MASK                   (0xFU << RCC_ADC2CFGR_PRE_SHIFT)
#define RCC_ADC2CFGR_PRE(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_ADC2CFGR_PRE_SHIFT)) & RCC_ADC2CFGR_PRE_MASK)

/*!
 * @brief RCC_DACCFGR Register Bit Definition
 */

#define RCC_DACCFGR_PRE_SHIFT                   (0)
#define RCC_DACCFGR_PRE_MASK                    (0x7FU << RCC_DACCFGR_PRE_SHIFT)
#define RCC_DACCFGR_PRE(x)                      (((uint32_t)(((uint32_t)(x)) << RCC_DACCFGR_PRE_SHIFT)) & RCC_DACCFGR_PRE_MASK)

/*!
 * @brief RCC_FSMCCFGR Register Bit Definition
 */

#define RCC_FSMCCFGR_FSMCSYNCEN_SHIFT           (8)
#define RCC_FSMCCFGR_FSMCSYNCEN_MASK            (0x01U << RCC_FSMCCFGR_FSMCSYNCEN_SHIFT)
#define RCC_FSMCCFGR_FSMCSYNCEN(x)              (((uint32_t)(((uint32_t)(x)) << RCC_FSMCCFGR_FSMCSYNCEN_SHIFT)) & RCC_FSMCCFGR_FSMCSYNCEN_MASK)

#define RCC_FSMCCFGR_FSMCPRE_SHIFT              (0)
#define RCC_FSMCCFGR_FSMCPRE_MASK               (0x1FU << RCC_FSMCCFGR_FSMCPRE_SHIFT)
#define RCC_FSMCCFGR_FSMCPRE(x)                 (((uint32_t)(((uint32_t)(x)) << RCC_FSMCCFGR_FSMCPRE_SHIFT)) & RCC_FSMCCFGR_FSMCPRE_MASK)

/*!
 * @brief RCC_TPIUCFGR Register Bit Definition
 */

#define RCC_TPIUCFGR_PRE_SHIFT                  (0)
#define RCC_TPIUCFGR_PRE_MASK                   (0x3U << RCC_TPIUCFGR_PRE_SHIFT)
#define RCC_TPIUCFGR_PRE(x)                     (((uint32_t)(((uint32_t)(x)) << RCC_TPIUCFGR_PRE_SHIFT)) & RCC_TPIUCFGR_PRE_MASK)

/*!
 * @}
 */ /* end of group RCC_Register_Masks */
/******************************************************************************
 *RCC Instance
*******************************************************************************/

#define RCC                  ((RCC_Type*)RCC_BASE)

/*!
 * @}
 */ /* end of group RCC_Peripheral_Access_Layer */

/*!
 * @addtogroup RTC_Peripheral_Access_Layer RTC Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * RTC Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t CRH;                                                            ///< RTC control register high bits               offset: 0x00
    __IO uint32_t CRL;                                                            ///< RTC control register low bits                offset: 0x04
    __IO uint32_t PRLH;                                                           ///< RTC prescaler load register high bits        offset: 0x08
    __IO uint32_t PRLL;                                                           ///< RTC prescaler load register low bits         offset: 0x0C
    __IO uint32_t DIVH;                                                           ///< RTC prescaler division factor register high bitsoffset: 0x10
    __IO uint32_t DIVL;                                                           ///< RTC prescaler division factor register low bitsoffset: 0x14
    __IO uint32_t CNTH;                                                           ///< RTC counter register high bits               offset: 0x18
    __IO uint32_t CNTL;                                                           ///< RTC counter register low bits                offset: 0x1C
    __IO uint32_t ALRH;                                                           ///< RTC alarm register high bit                  offset: 0x20
    __IO uint32_t ALRL;                                                           ///< RTC alarm register low bits                  offset: 0x24
    __IO uint32_t MSRH;                                                           ///< RTC millisecond register high bit            offset: 0x28
    __IO uint32_t MSRL;                                                           ///< RTC millisecond register low bit             offset: 0x2C
    __IO uint32_t Reserved48[3];                                                   ///< Reserved
    __IO uint32_t LSECFG;                                                         ///< RTC LSE Configuration Register               offset: 0x3C
} RTC_Type;

/*******************************************************************************
 * RTC Type
 ******************************************************************************/

/*!
 * @addtogroup RTC_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief RTC_CRH Register Bit Definition
 */

#define RTC_CRH_OWIE_SHIFT                      (2)
#define RTC_CRH_OWIE_MASK                       (0x01U << RTC_CRH_OWIE_SHIFT)
#define RTC_CRH_OWIE(x)                         (((uint32_t)(((uint32_t)(x)) << RTC_CRH_OWIE_SHIFT)) & RTC_CRH_OWIE_MASK)

#define RTC_CRH_ALRIE_SHIFT                     (1)
#define RTC_CRH_ALRIE_MASK                      (0x01U << RTC_CRH_ALRIE_SHIFT)
#define RTC_CRH_ALRIE(x)                        (((uint32_t)(((uint32_t)(x)) << RTC_CRH_ALRIE_SHIFT)) & RTC_CRH_ALRIE_MASK)

#define RTC_CRH_SECIE_SHIFT                     (0)
#define RTC_CRH_SECIE_MASK                      (0x01U << RTC_CRH_SECIE_SHIFT)
#define RTC_CRH_SECIE(x)                        (((uint32_t)(((uint32_t)(x)) << RTC_CRH_SECIE_SHIFT)) & RTC_CRH_SECIE_MASK)

/*!
 * @brief RTC_CRL Register Bit Definition
 */

#define RTC_CRL_ALPEN_SHIFT                     (6)
#define RTC_CRL_ALPEN_MASK                      (0x01U << RTC_CRL_ALPEN_SHIFT)
#define RTC_CRL_ALPEN(x)                        (((uint32_t)(((uint32_t)(x)) << RTC_CRL_ALPEN_SHIFT)) & RTC_CRL_ALPEN_MASK)

#define RTC_CRL_RTOFF_SHIFT                     (5)
#define RTC_CRL_RTOFF_MASK                      (0x01U << RTC_CRL_RTOFF_SHIFT)
#define RTC_CRL_RTOFF(x)                        (((uint32_t)(((uint32_t)(x)) << RTC_CRL_RTOFF_SHIFT)) & RTC_CRL_RTOFF_MASK)

#define RTC_CRL_CNF_SHIFT                       (4)
#define RTC_CRL_CNF_MASK                        (0x01U << RTC_CRL_CNF_SHIFT)
#define RTC_CRL_CNF(x)                          (((uint32_t)(((uint32_t)(x)) << RTC_CRL_CNF_SHIFT)) & RTC_CRL_CNF_MASK)

#define RTC_CRL_RSF_SHIFT                       (3)
#define RTC_CRL_RSF_MASK                        (0x01U << RTC_CRL_RSF_SHIFT)
#define RTC_CRL_RSF(x)                          (((uint32_t)(((uint32_t)(x)) << RTC_CRL_RSF_SHIFT)) & RTC_CRL_RSF_MASK)

#define RTC_CRL_OWF_SHIFT                       (2)
#define RTC_CRL_OWF_MASK                        (0x01U << RTC_CRL_OWF_SHIFT)
#define RTC_CRL_OWF(x)                          (((uint32_t)(((uint32_t)(x)) << RTC_CRL_OWF_SHIFT)) & RTC_CRL_OWF_MASK)

#define RTC_CRL_ALRF_SHIFT                      (1)
#define RTC_CRL_ALRF_MASK                       (0x01U << RTC_CRL_ALRF_SHIFT)
#define RTC_CRL_ALRF(x)                         (((uint32_t)(((uint32_t)(x)) << RTC_CRL_ALRF_SHIFT)) & RTC_CRL_ALRF_MASK)

#define RTC_CRL_SECF_SHIFT                      (0)
#define RTC_CRL_SECF_MASK                       (0x01U << RTC_CRL_SECF_SHIFT)
#define RTC_CRL_SECF(x)                         (((uint32_t)(((uint32_t)(x)) << RTC_CRL_SECF_SHIFT)) & RTC_CRL_SECF_MASK)

/*!
 * @brief RTC_PRLH Register Bit Definition
 */

#define RTC_PRLH_PRL_SHIFT                      (0)
#define RTC_PRLH_PRL_MASK                       (0xFU << RTC_PRLH_PRL_SHIFT)
#define RTC_PRLH_PRL(x)                         (((uint32_t)(((uint32_t)(x)) << RTC_PRLH_PRL_SHIFT)) & RTC_PRLH_PRL_MASK)

/*!
 * @brief RTC_PRLL Register Bit Definition
 */

#define RTC_PRLL_PRL_SHIFT                      (0)
#define RTC_PRLL_PRL_MASK                       (0xFFFFU << RTC_PRLL_PRL_SHIFT)
#define RTC_PRLL_PRL(x)                         (((uint32_t)(((uint32_t)(x)) << RTC_PRLL_PRL_SHIFT)) & RTC_PRLL_PRL_MASK)

/*!
 * @brief RTC_DIVH Register Bit Definition
 */

#define RTC_DIVH_DIV_SHIFT                      (0)
#define RTC_DIVH_DIV_MASK                       (0xFU << RTC_DIVH_DIV_SHIFT)
#define RTC_DIVH_DIV(x)                         (((uint32_t)(((uint32_t)(x)) << RTC_DIVH_DIV_SHIFT)) & RTC_DIVH_DIV_MASK)

/*!
 * @brief RTC_DIVL Register Bit Definition
 */

#define RTC_DIVL_DIV_SHIFT                      (0)
#define RTC_DIVL_DIV_MASK                       (0xFFFFU << RTC_DIVL_DIV_SHIFT)
#define RTC_DIVL_DIV(x)                         (((uint32_t)(((uint32_t)(x)) << RTC_DIVL_DIV_SHIFT)) & RTC_DIVL_DIV_MASK)

/*!
 * @brief RTC_CNTH Register Bit Definition
 */

#define RTC_CNTH_CNT_SHIFT                      (0)
#define RTC_CNTH_CNT_MASK                       (0xFFFFU << RTC_CNTH_CNT_SHIFT)
#define RTC_CNTH_CNT(x)                         (((uint32_t)(((uint32_t)(x)) << RTC_CNTH_CNT_SHIFT)) & RTC_CNTH_CNT_MASK)

/*!
 * @brief RTC_CNTL Register Bit Definition
 */

#define RTC_CNTL_CNT_SHIFT                      (0)
#define RTC_CNTL_CNT_MASK                       (0xFFFFU << RTC_CNTL_CNT_SHIFT)
#define RTC_CNTL_CNT(x)                         (((uint32_t)(((uint32_t)(x)) << RTC_CNTL_CNT_SHIFT)) & RTC_CNTL_CNT_MASK)

/*!
 * @brief RTC_ALRH Register Bit Definition
 */

#define RTC_ALRH_ALR_SHIFT                      (0)
#define RTC_ALRH_ALR_MASK                       (0xFFFFU << RTC_ALRH_ALR_SHIFT)
#define RTC_ALRH_ALR(x)                         (((uint32_t)(((uint32_t)(x)) << RTC_ALRH_ALR_SHIFT)) & RTC_ALRH_ALR_MASK)

/*!
 * @brief RTC_ALRL Register Bit Definition
 */

#define RTC_ALRL_ALR_SHIFT                      (0)
#define RTC_ALRL_ALR_MASK                       (0xFFFFU << RTC_ALRL_ALR_SHIFT)
#define RTC_ALRL_ALR(x)                         (((uint32_t)(((uint32_t)(x)) << RTC_ALRL_ALR_SHIFT)) & RTC_ALRL_ALR_MASK)

/*!
 * @brief RTC_MSRH Register Bit Definition
 */

#define RTC_MSRH_MSR_SHIFT                      (0)
#define RTC_MSRH_MSR_MASK                       (0xFU << RTC_MSRH_MSR_SHIFT)
#define RTC_MSRH_MSR(x)                         (((uint32_t)(((uint32_t)(x)) << RTC_MSRH_MSR_SHIFT)) & RTC_MSRH_MSR_MASK)

/*!
 * @brief RTC_MSRL Register Bit Definition
 */

#define RTC_MSRL_MSR_SHIFT                      (0)
#define RTC_MSRL_MSR_MASK                       (0xFFFFU << RTC_MSRL_MSR_SHIFT)
#define RTC_MSRL_MSR(x)                         (((uint32_t)(((uint32_t)(x)) << RTC_MSRL_MSR_SHIFT)) & RTC_MSRL_MSR_MASK)

/*!
 * @brief RTC_LSE_CFG Register Bit Definition
 */

#define RTC_LSECFG_LSEAAC_SHIFT                 (10)
#define RTC_LSECFG_LSEAAC_MASK                  (0x01U << RTC_LSECFG_LSEAAC_SHIFT)
#define RTC_LSECFG_LSEAAC(x)                    (((uint32_t)(((uint32_t)(x)) << RTC_LSECFG_LSEAAC_SHIFT)) & RTC_LSECFG_LSEAAC_MASK)

#define RTC_LSECFG_LSEIB_SHIFT                  (8)
#define RTC_LSECFG_LSEIB_MASK                   (0x3U << RTC_LSECFG_LSEIB_SHIFT)
#define RTC_LSECFG_LSEIB(x)                     (((uint32_t)(((uint32_t)(x)) << RTC_LSECFG_LSEIB_SHIFT)) & RTC_LSECFG_LSEIB_MASK)

#define RTC_LSECFG_LSERFBSEL_SHIFT              (6)
#define RTC_LSECFG_LSERFBSEL_MASK               (0x3U << RTC_LSECFG_LSERFBSEL_SHIFT)
#define RTC_LSECFG_LSERFBSEL(x)                 (((uint32_t)(((uint32_t)(x)) << RTC_LSECFG_LSERFBSEL_SHIFT)) & RTC_LSECFG_LSERFBSEL_MASK)

#define RTC_LSECFG_LSEDR_SHIFT                  (4)
#define RTC_LSECFG_LSEDR_MASK                   (0x3U << RTC_LSECFG_LSEDR_SHIFT)
#define RTC_LSECFG_LSEDR(x)                     (((uint32_t)(((uint32_t)(x)) << RTC_LSECFG_LSEDR_SHIFT)) & RTC_LSECFG_LSEDR_MASK)

#define RTC_LSECFG_LSEOUTENH_SHIFT              (2)
#define RTC_LSECFG_LSEOUTENH_MASK               (0x01U << RTC_LSECFG_LSEOUTENH_SHIFT)
#define RTC_LSECFG_LSEOUTENH(x)                 (((uint32_t)(((uint32_t)(x)) << RTC_LSECFG_LSEOUTENH_SHIFT)) & RTC_LSECFG_LSEOUTENH_MASK)

#define RTC_LSECFG_LSETC_SHIFT                  (0)
#define RTC_LSECFG_LSETC_MASK                   (0x02U << RTC_LSECFG_LSETC_SHIFT)
#define RTC_LSECFG_LSETC(x)                     (((uint32_t)(((uint32_t)(x)) << RTC_LSECFG_LSETC_SHIFT)) & RTC_LSECFG_LSETC_MASK)

/*!
 * @}
 */ /* end of group RTC_Register_Masks */
/******************************************************************************
 *RTC Instance
*******************************************************************************/

#define RTC                  ((RTC_Type*)RTC_BKP_BASE)

/*!
 * @}
 */ /* end of group RTC_Peripheral_Access_Layer */




/*!
 * @addtogroup SPI_Peripheral_Access_Layer SPI Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * SPI Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t TXREG;                                                       ///< transmit data register                       offset: 0x00
    __IO uint32_t RXREG;                                                       ///< receive data register                        offset: 0x04
    __IO uint32_t CSTAT;                                                       ///< current status register                      offset: 0x08
    __IO uint32_t INTSTAT;                                                     ///< Interrupt Status Register                    offset: 0x0C
    __IO uint32_t INTEN;                                                       ///< Interrupt Enable Register                    offset: 0x10
    __IO uint32_t INTCLR;                                                      ///< interrupt clear register                     offset: 0x14
    __IO uint32_t GCTL;                                                        ///< global control register                      offset: 0x18
    __IO uint32_t CCTL;                                                        ///< General Control Register                     offset: 0x1C
    __IO uint32_t SPBRG;                                                       ///< baud rate generator                          offset: 0x20
    __IO uint32_t RXDNR;                                                       ///< Receive data count register                  offset: 0x24
    __IO uint32_t NSSR;                                                        ///< Slave Chip Select Register                   offset: 0x28
    __IO uint32_t EXTCTL;                                                      ///< data control register                        offset: 0x2C
    __IO uint32_t I2SCFGR;                                                     ///< I2S_ configuration register                   offset: 0x30
} SPI_Type, I2S_Type;

/*******************************************************************************
 * SPI Type
 ******************************************************************************/

/*!
 * @addtogroup SPI_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief SPI_I2S_TXREG Register Bit Definition
 */

#define SPI_I2S_TXREG_TXREG_SHIFT                (0)
#define SPI_I2S_TXREG_TXREG_MASK                 (0xFFFFFFFFU << SPI_I2S_TXREG_TXREG_SHIFT)
#define SPI_I2S_TXREG_TXREG(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_I2S_TXREG_TXREG_SHIFT)) & SPI_I2S_TXREG_TXREG_MASK)

/*!
 * @brief SPI_I2S_RXREG Register Bit Definition
 */

#define SPI_I2S_RXREG_RXREG_SHIFT                (0)
#define SPI_I2S_RXREG_RXREG_MASK                 (0xFFFFFFFFU << SPI_I2S_RXREG_RXREG_SHIFT)
#define SPI_I2S_RXREG_RXREG(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_I2S_RXREG_RXREG_SHIFT)) & SPI_I2S_RXREG_RXREG_MASK)

/*!
 * @brief SPI_I2S_CSTAT Register Bit Definition
 */

#define SPI_I2S_CSTAT_CHSIDE_SHIFT               (13)
#define SPI_I2S_CSTAT_CHSIDE_MASK                (0x01U << SPI_I2S_CSTAT_CHSIDE_SHIFT)
#define SPI_I2S_CSTAT_CHSIDE(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_I2S_CSTAT_CHSIDE_SHIFT)) & SPI_I2S_CSTAT_CHSIDE_MASK)

#define SPI_I2S_CSTAT_BUSY_SHIFT                 (12)
#define SPI_I2S_CSTAT_BUSY_MASK                  (0x01U << SPI_I2S_CSTAT_BUSY_SHIFT)
#define SPI_I2S_CSTAT_BUSY(x)                    (((uint32_t)(((uint32_t)(x)) << SPI_I2S_CSTAT_BUSY_SHIFT)) & SPI_I2S_CSTAT_BUSY_MASK)

#define SPI_I2S_CSTAT_RXFADDR_SHIFT              (8)
#define SPI_I2S_CSTAT_RXFADDR_MASK               (0xFU << SPI_I2S_CSTAT_RXFADDR_SHIFT)
#define SPI_I2S_CSTAT_RXFADDR(x)                 (((uint32_t)(((uint32_t)(x)) << SPI_I2S_CSTAT_RXFADDR_SHIFT)) & SPI_I2S_CSTAT_RXFADDR_MASK)

#define SPI_I2S_CSTAT_TXFADDR_SHIFT              (4)
#define SPI_I2S_CSTAT_TXFADDR_MASK               (0xFU << SPI_I2S_CSTAT_TXFADDR_SHIFT)
#define SPI_I2S_CSTAT_TXFADDR(x)                 (((uint32_t)(((uint32_t)(x)) << SPI_I2S_CSTAT_TXFADDR_SHIFT)) & SPI_I2S_CSTAT_TXFADDR_MASK)

#define SPI_I2S_CSTAT_RXAVL4BYTE_SHIFT           (3)
#define SPI_I2S_CSTAT_RXAVL4BYTE_MASK            (0x01U << SPI_I2S_CSTAT_RXAVL4BYTE_SHIFT)
#define SPI_I2S_CSTAT_RXAVL4BYTE(x)              (((uint32_t)(((uint32_t)(x)) << SPI_I2S_CSTAT_RXAVL4BYTE_SHIFT)) & SPI_I2S_CSTAT_RXAVL4BYTE_MASK)

#define SPI_I2S_CSTAT_TXFULL_SHIFT               (2)
#define SPI_I2S_CSTAT_TXFULL_MASK                (0x01U << SPI_I2S_CSTAT_TXFULL_SHIFT)
#define SPI_I2S_CSTAT_TXFULL(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_I2S_CSTAT_TXFULL_SHIFT)) & SPI_I2S_CSTAT_TXFULL_MASK)

#define SPI_I2S_CSTAT_RXAVL_SHIFT                (1)
#define SPI_I2S_CSTAT_RXAVL_MASK                 (0x01U << SPI_I2S_CSTAT_RXAVL_SHIFT)
#define SPI_I2S_CSTAT_RXAVL(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_I2S_CSTAT_RXAVL_SHIFT)) & SPI_I2S_CSTAT_RXAVL_MASK)

#define SPI_I2S_CSTAT_TXEPT_SHIFT                (0)
#define SPI_I2S_CSTAT_TXEPT_MASK                 (0x01U << SPI_I2S_CSTAT_TXEPT_SHIFT)
#define SPI_I2S_CSTAT_TXEPT(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_I2S_CSTAT_TXEPT_SHIFT)) & SPI_I2S_CSTAT_TXEPT_MASK)

/*!
 * @brief SPI_I2S_INTSTAT Register Bit Definition
 */

#define SPI_I2S_INTSTAT_FREINTF_SHIFT            (7)
#define SPI_I2S_INTSTAT_FREINTF_MASK             (0x01U << SPI_I2S_INTSTAT_FREINTF_SHIFT)
#define SPI_I2S_INTSTAT_FREINTF(x)               (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTSTAT_FREINTF_SHIFT)) & SPI_I2S_INTSTAT_FREINTF_MASK)

#define SPI_I2S_INTSTAT_TXEPTINTF_SHIFT          (6)
#define SPI_I2S_INTSTAT_TXEPTINTF_MASK           (0x01U << SPI_I2S_INTSTAT_TXEPTINTF_SHIFT)
#define SPI_I2S_INTSTAT_TXEPTINTF(x)             (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTSTAT_TXEPTINTF_SHIFT)) & SPI_I2S_INTSTAT_TXEPTINTF_MASK)

#define SPI_I2S_INTSTAT_RXFULLINTF_SHIFT         (5)
#define SPI_I2S_INTSTAT_RXFULLINTF_MASK          (0x01U << SPI_I2S_INTSTAT_RXFULLINTF_SHIFT)
#define SPI_I2S_INTSTAT_RXFULLINTF(x)            (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTSTAT_RXFULLINTF_SHIFT)) & SPI_I2S_INTSTAT_RXFULLINTF_MASK)

#define SPI_I2S_INTSTAT_RXMATCHINTF_SHIFT        (4)
#define SPI_I2S_INTSTAT_RXMATCHINTF_MASK         (0x01U << SPI_I2S_INTSTAT_RXMATCHINTF_SHIFT)
#define SPI_I2S_INTSTAT_RXMATCHINTF(x)           (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTSTAT_RXMATCHINTF_SHIFT)) & SPI_I2S_INTSTAT_RXMATCHINTF_MASK)

#define SPI_I2S_INTSTAT_RXOERRINTF_SHIFT         (3)
#define SPI_I2S_INTSTAT_RXOERRINTF_MASK          (0x01U << SPI_I2S_INTSTAT_RXOERRINTF_SHIFT)
#define SPI_I2S_INTSTAT_RXOERRINTF(x)            (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTSTAT_RXOERRINTF_SHIFT)) & SPI_I2S_INTSTAT_RXOERRINTF_MASK)

#define SPI_I2S_INTSTAT_UNDERRUNINTF_SHIFT       (2)
#define SPI_I2S_INTSTAT_UNDERRUNINTF_MASK        (0x01U << SPI_I2S_INTSTAT_UNDERRUNINTF_SHIFT)
#define SPI_I2S_INTSTAT_UNDERRUNINTF(x)          (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTSTAT_UNDERRUNINTF_SHIFT)) & SPI_I2S_INTSTAT_UNDERRUNINTF_MASK)

#define SPI_I2S_INTSTAT_RXINTF_SHIFT             (1)
#define SPI_I2S_INTSTAT_RXINTF_MASK              (0x01U << SPI_I2S_INTSTAT_RXINTF_SHIFT)
#define SPI_I2S_INTSTAT_RXINTF(x)                (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTSTAT_RXINTF_SHIFT)) & SPI_I2S_INTSTAT_RXINTF_MASK)

#define SPI_I2S_INTSTAT_TXINTF_SHIFT             (0)
#define SPI_I2S_INTSTAT_TXINTF_MASK              (0x01U << SPI_I2S_INTSTAT_TXINTF_SHIFT)
#define SPI_I2S_INTSTAT_TXINTF(x)                (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTSTAT_TXINTF_SHIFT)) & SPI_I2S_INTSTAT_TXINTF_MASK)

/*!
 * @brief SPI_I2S_INTEN Register Bit Definition
 */

#define SPI_I2S_INTEN_FREIEN_SHIFT               (7)
#define SPI_I2S_INTEN_FREIEN_MASK                (0x01U << SPI_I2S_INTEN_FREIEN_SHIFT)
#define SPI_I2S_INTEN_FREIEN(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTEN_FREIEN_SHIFT)) & SPI_I2S_INTEN_FREIEN_MASK)

#define SPI_I2S_INTEN_TXEPTIEN_SHIFT             (6)
#define SPI_I2S_INTEN_TXEPTIEN_MASK              (0x01U << SPI_I2S_INTEN_TXEPTIEN_SHIFT)
#define SPI_I2S_INTEN_TXEPTIEN(x)                (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTEN_TXEPTIEN_SHIFT)) & SPI_I2S_INTEN_TXEPTIEN_MASK)

#define SPI_I2S_INTEN_RXFULLIEN_SHIFT            (5)
#define SPI_I2S_INTEN_RXFULLIEN_MASK             (0x01U << SPI_I2S_INTEN_RXFULLIEN_SHIFT)
#define SPI_I2S_INTEN_RXFULLIEN(x)               (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTEN_RXFULLIEN_SHIFT)) & SPI_I2S_INTEN_RXFULLIEN_MASK)

#define SPI_I2S_INTEN_RXMATCHIEN_SHIFT           (4)
#define SPI_I2S_INTEN_RXMATCHIEN_MASK            (0x01U << SPI_I2S_INTEN_RXMATCHIEN_SHIFT)
#define SPI_I2S_INTEN_RXMATCHIEN(x)              (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTEN_RXMATCHIEN_SHIFT)) & SPI_I2S_INTEN_RXMATCHIEN_MASK)

#define SPI_I2S_INTEN_RXOERRIEN_SHIFT            (3)
#define SPI_I2S_INTEN_RXOERRIEN_MASK             (0x01U << SPI_I2S_INTEN_RXOERRIEN_SHIFT)
#define SPI_I2S_INTEN_RXOERRIEN(x)               (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTEN_RXOERRIEN_SHIFT)) & SPI_I2S_INTEN_RXOERRIEN_MASK)

#define SPI_I2S_INTEN_UNDERRUNIEN_SHIFT          (2)
#define SPI_I2S_INTEN_UNDERRUNIEN_MASK           (0x01U << SPI_I2S_INTEN_UNDERRUNIEN_SHIFT)
#define SPI_I2S_INTEN_UNDERRUNIEN(x)             (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTEN_UNDERRUNIEN_SHIFT)) & SPI_I2S_INTEN_UNDERRUNIEN_MASK)

#define SPI_I2S_INTEN_RXIEN_SHIFT                (1)
#define SPI_I2S_INTEN_RXIEN_MASK                 (0x01U << SPI_I2S_INTEN_RXIEN_SHIFT)
#define SPI_I2S_INTEN_RXIEN(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTEN_RXIEN_SHIFT)) & SPI_I2S_INTEN_RXIEN_MASK)

#define SPI_I2S_INTEN_TXIEN_SHIFT                (0)
#define SPI_I2S_INTEN_TXIEN_MASK                 (0x01U << SPI_I2S_INTEN_TXIEN_SHIFT)
#define SPI_I2S_INTEN_TXIEN(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTEN_TXIEN_SHIFT)) & SPI_I2S_INTEN_TXIEN_MASK)

/*!
 * @brief SPI_I2S_INTCLR Register Bit Definition
 */

#define SPI_I2S_INTCLR_FREICLR_SHIFT             (7)
#define SPI_I2S_INTCLR_FREICLR_MASK              (0x01U << SPI_I2S_INTCLR_FREICLR_SHIFT)
#define SPI_I2S_INTCLR_FREICLR(x)                (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTCLR_FREICLR_SHIFT)) & SPI_I2S_INTCLR_FREICLR_MASK)

#define SPI_I2S_INTCLR_TXEPTICLR_SHIFT           (6)
#define SPI_I2S_INTCLR_TXEPTICLR_MASK            (0x01U << SPI_I2S_INTCLR_TXEPTICLR_SHIFT)
#define SPI_I2S_INTCLR_TXEPTICLR(x)              (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTCLR_TXEPTICLR_SHIFT)) & SPI_I2S_INTCLR_TXEPTICLR_MASK)

#define SPI_I2S_INTCLR_RXFULLICLR_SHIFT          (5)
#define SPI_I2S_INTCLR_RXFULLICLR_MASK           (0x01U << SPI_I2S_INTCLR_RXFULLICLR_SHIFT)
#define SPI_I2S_INTCLR_RXFULLICLR(x)             (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTCLR_RXFULLICLR_SHIFT)) & SPI_I2S_INTCLR_RXFULLICLR_MASK)

#define SPI_I2S_INTCLR_RXMATCHICLR_SHIFT         (4)
#define SPI_I2S_INTCLR_RXMATCHICLR_MASK          (0x01U << SPI_I2S_INTCLR_RXMATCHICLR_SHIFT)
#define SPI_I2S_INTCLR_RXMATCHICLR(x)            (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTCLR_RXMATCHICLR_SHIFT)) & SPI_I2S_INTCLR_RXMATCHICLR_MASK)

#define SPI_I2S_INTCLR_RXOERRICLR_SHIFT          (3)
#define SPI_I2S_INTCLR_RXOERRICLR_MASK           (0x01U << SPI_I2S_INTCLR_RXOERRICLR_SHIFT)
#define SPI_I2S_INTCLR_RXOERRICLR(x)             (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTCLR_RXOERRICLR_SHIFT)) & SPI_I2S_INTCLR_RXOERRICLR_MASK)

#define SPI_I2S_INTCLR_UNDERRUNICLR_SHIFT        (2)
#define SPI_I2S_INTCLR_UNDERRUNICLR_MASK         (0x01U << SPI_I2S_INTCLR_UNDERRUNICLR_SHIFT)
#define SPI_I2S_INTCLR_UNDERRUNICLR(x)           (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTCLR_UNDERRUNICLR_SHIFT)) & SPI_I2S_INTCLR_UNDERRUNICLR_MASK)

#define SPI_I2S_INTCLR_RXICLR_SHIFT              (1)
#define SPI_I2S_INTCLR_RXICLR_MASK               (0x01U << SPI_I2S_INTCLR_RXICLR_SHIFT)
#define SPI_I2S_INTCLR_RXICLR(x)                 (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTCLR_RXICLR_SHIFT)) & SPI_I2S_INTCLR_RXICLR_MASK)

#define SPI_I2S_INTCLR_TXICLR_SHIFT              (0)
#define SPI_I2S_INTCLR_TXICLR_MASK               (0x01U << SPI_I2S_INTCLR_TXICLR_SHIFT)
#define SPI_I2S_INTCLR_TXICLR(x)                 (((uint32_t)(((uint32_t)(x)) << SPI_I2S_INTCLR_TXICLR_SHIFT)) & SPI_I2S_INTCLR_TXICLR_MASK)

/*!
 * @brief SPI_I2S_GCTL Register Bit Definition
 */

#define SPI_I2S_GCTL_PADSEL_SHIFT                (13)
#define SPI_I2S_GCTL_PADSEL_MASK                 (0x1FU << SPI_I2S_GCTL_PADSEL_SHIFT)
#define SPI_I2S_GCTL_PADSEL(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_I2S_GCTL_PADSEL_SHIFT)) & SPI_I2S_GCTL_PADSEL_MASK)

#define SPI_I2S_GCTL_DW832_SHIFT                 (11)
#define SPI_I2S_GCTL_DW832_MASK                  (0x01U << SPI_I2S_GCTL_DW832_SHIFT)
#define SPI_I2S_GCTL_DW832(x)                    (((uint32_t)(((uint32_t)(x)) << SPI_I2S_GCTL_DW832_SHIFT)) & SPI_I2S_GCTL_DW832_MASK)

#define SPI_I2S_GCTL_NSS_SHIFT                   (10)
#define SPI_I2S_GCTL_NSS_MASK                    (0x01U << SPI_I2S_GCTL_NSS_SHIFT)
#define SPI_I2S_GCTL_NSS(x)                      (((uint32_t)(((uint32_t)(x)) << SPI_I2S_GCTL_NSS_SHIFT)) & SPI_I2S_GCTL_NSS_MASK)

#define SPI_I2S_GCTL_DMAMODE_SHIFT               (9)
#define SPI_I2S_GCTL_DMAMODE_MASK                (0x01U << SPI_I2S_GCTL_DMAMODE_SHIFT)
#define SPI_I2S_GCTL_DMAMODE(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_I2S_GCTL_DMAMODE_SHIFT)) & SPI_I2S_GCTL_DMAMODE_MASK)

#define SPI_I2S_GCTL_RXEN_SHIFT                  (4)
#define SPI_I2S_GCTL_RXEN_MASK                   (0x01U << SPI_I2S_GCTL_RXEN_SHIFT)
#define SPI_I2S_GCTL_RXEN(x)                     (((uint32_t)(((uint32_t)(x)) << SPI_I2S_GCTL_RXEN_SHIFT)) & SPI_I2S_GCTL_RXEN_MASK)

#define SPI_I2S_GCTL_TXEN_SHIFT                  (3)
#define SPI_I2S_GCTL_TXEN_MASK                   (0x01U << SPI_I2S_GCTL_TXEN_SHIFT)
#define SPI_I2S_GCTL_TXEN(x)                     (((uint32_t)(((uint32_t)(x)) << SPI_I2S_GCTL_TXEN_SHIFT)) & SPI_I2S_GCTL_TXEN_MASK)

#define SPI_I2S_GCTL_MODE_SHIFT                  (2)
#define SPI_I2S_GCTL_MODE_MASK                   (0x01U << SPI_I2S_GCTL_MODE_SHIFT)
#define SPI_I2S_GCTL_MODE(x)                     (((uint32_t)(((uint32_t)(x)) << SPI_I2S_GCTL_MODE_SHIFT)) & SPI_I2S_GCTL_MODE_MASK)

#define SPI_I2S_GCTL_INTEN_SHIFT                 (1)
#define SPI_I2S_GCTL_INTEN_MASK                  (0x01U << SPI_I2S_GCTL_INTEN_SHIFT)
#define SPI_I2S_GCTL_INTEN(x)                    (((uint32_t)(((uint32_t)(x)) << SPI_I2S_GCTL_INTEN_SHIFT)) & SPI_I2S_GCTL_INTEN_MASK)

#define SPI_I2S_GCTL_SPIEN_SHIFT                 (0)
#define SPI_I2S_GCTL_SPIEN_MASK                  (0x01U << SPI_I2S_GCTL_SPIEN_SHIFT)
#define SPI_I2S_GCTL_SPIEN(x)                    (((uint32_t)(((uint32_t)(x)) << SPI_I2S_GCTL_SPIEN_SHIFT)) & SPI_I2S_GCTL_SPIEN_MASK)

/*!
 * @brief SPI_I2S_CCTL Register Bit Definition
 */

#define SPI_I2S_CCTL_MRDECHG_SHIFT               (8)
#define SPI_I2S_CCTL_MRDECHG_MASK                (0x7U << SPI_I2S_CCTL_MRDECHG_SHIFT)
#define SPI_I2S_CCTL_MRDECHG(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_I2S_CCTL_MRDECHG_SHIFT)) & SPI_I2S_CCTL_MRDECHG_MASK)

#define SPI_I2S_CCTL_TISEL_SHIFT                 (7)
#define SPI_I2S_CCTL_TISEL_MASK                  (0x01U << SPI_I2S_CCTL_TISEL_SHIFT)
#define SPI_I2S_CCTL_TISEL(x)                    (((uint32_t)(((uint32_t)(x)) << SPI_I2S_CCTL_TISEL_SHIFT)) & SPI_I2S_CCTL_TISEL_MASK)

#define SPI_I2S_CCTL_CPHASEL_SHIFT               (6)
#define SPI_I2S_CCTL_CPHASEL_MASK                (0x01U << SPI_I2S_CCTL_CPHASEL_SHIFT)
#define SPI_I2S_CCTL_CPHASEL(x)                  (((uint32_t)(((uint32_t)(x)) << SPI_I2S_CCTL_CPHASEL_SHIFT)) & SPI_I2S_CCTL_CPHASEL_MASK)

#define SPI_I2S_CCTL_TXEDGE_SHIFT                (5)
#define SPI_I2S_CCTL_TXEDGE_MASK                 (0x01U << SPI_I2S_CCTL_TXEDGE_SHIFT)
#define SPI_I2S_CCTL_TXEDGE(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_I2S_CCTL_TXEDGE_SHIFT)) & SPI_I2S_CCTL_TXEDGE_MASK)

#define SPI_I2S_CCTL_RXEDGE_SHIFT                (4)
#define SPI_I2S_CCTL_RXEDGE_MASK                 (0x01U << SPI_I2S_CCTL_RXEDGE_SHIFT)
#define SPI_I2S_CCTL_RXEDGE(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_I2S_CCTL_RXEDGE_SHIFT)) & SPI_I2S_CCTL_RXEDGE_MASK)

#define SPI_I2S_CCTL_SPILEN_SHIFT                (3)
#define SPI_I2S_CCTL_SPILEN_MASK                 (0x01U << SPI_I2S_CCTL_SPILEN_SHIFT)
#define SPI_I2S_CCTL_SPILEN(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_I2S_CCTL_SPILEN_SHIFT)) & SPI_I2S_CCTL_SPILEN_MASK)

#define SPI_I2S_CCTL_LSBFE_SHIFT                 (2)
#define SPI_I2S_CCTL_LSBFE_MASK                  (0x01U << SPI_I2S_CCTL_LSBFE_SHIFT)
#define SPI_I2S_CCTL_LSBFE(x)                    (((uint32_t)(((uint32_t)(x)) << SPI_I2S_CCTL_LSBFE_SHIFT)) & SPI_I2S_CCTL_LSBFE_MASK)

#define SPI_I2S_CCTL_CPOL_SHIFT                  (1)
#define SPI_I2S_CCTL_CPOL_MASK                   (0x01U << SPI_I2S_CCTL_CPOL_SHIFT)
#define SPI_I2S_CCTL_CPOL(x)                     (((uint32_t)(((uint32_t)(x)) << SPI_I2S_CCTL_CPOL_SHIFT)) & SPI_I2S_CCTL_CPOL_MASK)

#define SPI_I2S_CCTL_CPHA_SHIFT                  (0)
#define SPI_I2S_CCTL_CPHA_MASK                   (0x01U << SPI_I2S_CCTL_CPHA_SHIFT)
#define SPI_I2S_CCTL_CPHA(x)                     (((uint32_t)(((uint32_t)(x)) << SPI_I2S_CCTL_CPHA_SHIFT)) & SPI_I2S_CCTL_CPHA_MASK)

/*!
 * @brief SPI_I2S_SPBRG Register Bit Definition
 */

#define SPI_I2S_SPBRG_SPBRG_SHIFT                (0)
#define SPI_I2S_SPBRG_SPBRG_MASK                 (0xFFFFU << SPI_I2S_SPBRG_SPBRG_SHIFT)
#define SPI_I2S_SPBRG_SPBRG(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_I2S_SPBRG_SPBRG_SHIFT)) & SPI_I2S_SPBRG_SPBRG_MASK)

/*!
 * @brief SPI_I2S_RXDNR Register Bit Definition
 */

#define SPI_I2S_RXDNR_RXDNR_SHIFT                (0)
#define SPI_I2S_RXDNR_RXDNR_MASK                 (0xFFFFU << SPI_I2S_RXDNR_RXDNR_SHIFT)
#define SPI_I2S_RXDNR_RXDNR(x)                   (((uint32_t)(((uint32_t)(x)) << SPI_I2S_RXDNR_RXDNR_SHIFT)) & SPI_I2S_RXDNR_RXDNR_MASK)

/*!
 * @brief SPI_I2S_NSSR Register Bit Definition
 */

#define SPI_I2S_NSSR_NSS_SHIFT                   (0)
#define SPI_I2S_NSSR_NSS_MASK                    (0x01U << SPI_I2S_NSSR_NSS_SHIFT)
#define SPI_I2S_NSSR_NSS(x)                      (((uint32_t)(((uint32_t)(x)) << SPI_I2S_NSSR_NSS_SHIFT)) & SPI_I2S_NSSR_NSS_MASK)

/*!
 * @brief SPI_I2S_EXTCTL Register Bit Definition
 */

#define SPI_I2S_EXTCTL_EXTLEN_SHIFT              (0)
#define SPI_I2S_EXTCTL_EXTLEN_MASK               (0x1FU << SPI_I2S_EXTCTL_EXTLEN_SHIFT)
#define SPI_I2S_EXTCTL_EXTLEN(x)                 (((uint32_t)(((uint32_t)(x)) << SPI_I2S_EXTCTL_EXTLEN_SHIFT)) & SPI_I2S_EXTCTL_EXTLEN_MASK)

/*!
 * @brief SPI_I2S_I2SCFGR Register Bit Definition
 */

#define SPI_I2S_I2SCFGR_I2SDIV_SHIFT             (16)
#define SPI_I2S_I2SCFGR_I2SDIV_MASK              (0x1FFU << SPI_I2S_I2SCFGR_I2SDIV_SHIFT)
#define SPI_I2S_I2SCFGR_I2SDIV(x)                (((uint32_t)(((uint32_t)(x)) << SPI_I2S_I2SCFGR_I2SDIV_SHIFT)) & SPI_I2S_I2SCFGR_I2SDIV_MASK)

#define SPI_I2S_I2SCFGR_HFDSEL_SHIFT              (13)
#define SPI_I2S_I2SCFGR_HFDSEL_MASK               (0x01U << SPI_I2S_I2SCFGR_HFDSEL_SHIFT)
#define SPI_I2S_I2SCFGR_HFDSEL(x)                 (((uint32_t)(((uint32_t)(x)) << SPI_I2S_I2SCFGR_HFDSEL_SHIFT)) & SPI_I2S_I2SCFGR_HFDSEL_MASK)

#define SPI_I2S_I2SCFGR_MCKSEL_SHIFT             (12)
#define SPI_I2S_I2SCFGR_MCKSEL_MASK              (0x01U << SPI_I2S_I2SCFGR_MCKSEL_SHIFT)
#define SPI_I2S_I2SCFGR_MCKSEL(x)                (((uint32_t)(((uint32_t)(x)) << SPI_I2S_I2SCFGR_MCKSEL_SHIFT)) & SPI_I2S_I2SCFGR_MCKSEL_MASK)

#define SPI_I2S_I2SCFGR_MCKOE_SHIFT              (11)
#define SPI_I2S_I2SCFGR_MCKOE_MASK               (0x01U << SPI_I2S_I2SCFGR_MCKOE_SHIFT)
#define SPI_I2S_I2SCFGR_MCKOE(x)                 (((uint32_t)(((uint32_t)(x)) << SPI_I2S_I2SCFGR_MCKOE_SHIFT)) & SPI_I2S_I2SCFGR_MCKOE_MASK)

#define SPI_I2S_I2SCFGR_SPII2S_SHIFT             (10)
#define SPI_I2S_I2SCFGR_SPII2S_MASK              (0x01U << SPI_I2S_I2SCFGR_SPII2S_SHIFT)
#define SPI_I2S_I2SCFGR_SPII2S(x)                (((uint32_t)(((uint32_t)(x)) << SPI_I2S_I2SCFGR_SPII2S_SHIFT)) & SPI_I2S_I2SCFGR_SPII2S_MASK)

#define SPI_I2S_I2SCFGR_PCMSYNC_SHIFT            (6)
#define SPI_I2S_I2SCFGR_PCMSYNC_MASK             (0x01U << SPI_I2S_I2SCFGR_PCMSYNC_SHIFT)
#define SPI_I2S_I2SCFGR_PCMSYNC(x)               (((uint32_t)(((uint32_t)(x)) << SPI_I2S_I2SCFGR_PCMSYNC_SHIFT)) & SPI_I2S_I2SCFGR_PCMSYNC_MASK)

#define SPI_I2S_I2SCFGR_I2SSTD_SHIFT             (4)
#define SPI_I2S_I2SCFGR_I2SSTD_MASK              (0x3U << SPI_I2S_I2SCFGR_I2SSTD_SHIFT)
#define SPI_I2S_I2SCFGR_I2SSTD(x)                (((uint32_t)(((uint32_t)(x)) << SPI_I2S_I2SCFGR_I2SSTD_SHIFT)) & SPI_I2S_I2SCFGR_I2SSTD_MASK)

#define SPI_I2S_I2SCFGR_DATLEN_SHIFT             (1)
#define SPI_I2S_I2SCFGR_DATLEN_MASK              (0x3U << SPI_I2S_I2SCFGR_DATLEN_SHIFT)
#define SPI_I2S_I2SCFGR_DATLEN(x)                (((uint32_t)(((uint32_t)(x)) << SPI_I2S_I2SCFGR_DATLEN_SHIFT)) & SPI_I2S_I2SCFGR_DATLEN_MASK)

#define SPI_I2S_I2SCFGR_CHLEN_SHIFT              (0)
#define SPI_I2S_I2SCFGR_CHLEN_MASK               (0x01U << SPI_I2S_I2SCFGR_CHLEN_SHIFT)
#define SPI_I2S_I2SCFGR_CHLEN(x)                 (((uint32_t)(((uint32_t)(x)) << SPI_I2S_I2SCFGR_CHLEN_SHIFT)) & SPI_I2S_I2SCFGR_CHLEN_MASK)

/*!
 * @}
 */ /* end of group SPI_Register_Masks */
/******************************************************************************
 *SPI Instance
*******************************************************************************/

#define SPI1                  ((SPI_Type*)SPI1_BASE)
#define SPI2                  ((SPI_Type*)SPI2_BASE)
#define SPI3                  ((SPI_Type*)SPI3_BASE)

/*!
 * @}
 */ /* end of group SPI_Peripheral_Access_Layer */

/*!
 * @addtogroup SYSCFG_Peripheral_Access_Layer SYSCFG Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * SYSCFG Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t CFGR;                                                           ///< SYSCFG Configuration Register                offset: 0x00
    __IO uint32_t Reserved49[1];                                                   ///< Reserved
    __IO uint32_t EXTICR1;                                                        ///< External Interrupt Configuration Register 1  offset: 0x08
    __IO uint32_t EXTICR2;                                                        ///< External Interrupt Configuration Register 2  offset: 0x0C
    __IO uint32_t EXTICR3;                                                        ///< External Interrupt Configuration Register 3  offset: 0x10
    __IO uint32_t EXTICR4;                                                        ///< External Interrupt Configuration Register 4  offset: 0x14
    __IO uint32_t CFGR2;                                                          ///< SYSCFG Configuration Register 2              offset: 0x18
    __IO uint32_t PDETCSR;                                                        ///< Power Detect Configuration Status Register   offset: 0x1C
    __IO uint32_t VOSDLY;                                                         ///< VOSDLY configuration register                offset: 0x20
    __IO uint32_t DMARMP;                                                         ///< DAM remap register                           offset: 0x24
    __IO uint32_t BUSPRIORITY;                                                    ///< Bus Priority Configuration Register          offset: 0x28
} SYSCFG_Type;

/*******************************************************************************
 * SYSCFG Type
 ******************************************************************************/

/*!
 * @addtogroup SYSCFG_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief SYSCFG_CFGR Register Bit Definition
 */

#define SYSCFG_CFGR_FCMODESEL_SHIFT               (29)
#define SYSCFG_CFGR_FCMODESEL_MASK                (0x3U << SYSCFG_CFGR_FCMODESEL_SHIFT)
#define SYSCFG_CFGR_FCMODESEL(x)                  (((uint32_t)(((uint32_t)(x)) << SYSCFG_CFGR_FCMODESEL_SHIFT)) & SYSCFG_CFGR_FCMODESEL_MASK)

#define SYSCFG_CFGR_FCODATAEN_SHIFT             (28)
#define SYSCFG_CFGR_FCODATAEN_MASK              (0x01U << SYSCFG_CFGR_FCODATAEN_SHIFT)
#define SYSCFG_CFGR_FCODATAEN(x)                (((uint32_t)(((uint32_t)(x)) << SYSCFG_CFGR_FCODATAEN_SHIFT)) & SYSCFG_CFGR_FCODATAEN_MASK)

#define SYSCFG_CFGR_CAN2STOP_SHIFT              (21)
#define SYSCFG_CFGR_CAN2STOP_MASK               (0x01U << SYSCFG_CFGR_CAN2STOP_SHIFT)
#define SYSCFG_CFGR_CAN2STOP(x)                 (((uint32_t)(((uint32_t)(x)) << SYSCFG_CFGR_CAN2STOP_SHIFT)) & SYSCFG_CFGR_CAN2STOP_MASK)

#define SYSCFG_CFGR_CAN2TEST_SHIFT              (20)
#define SYSCFG_CFGR_CAN2TEST_MASK               (0x01U << SYSCFG_CFGR_CAN2TEST_SHIFT)
#define SYSCFG_CFGR_CAN2TEST(x)                 (((uint32_t)(((uint32_t)(x)) << SYSCFG_CFGR_CAN2TEST_SHIFT)) & SYSCFG_CFGR_CAN2TEST_MASK)

#define SYSCFG_CFGR_CAN2SPV_SHIFT               (19)
#define SYSCFG_CFGR_CAN2SPV_MASK                (0x01U << SYSCFG_CFGR_CAN2SPV_SHIFT)
#define SYSCFG_CFGR_CAN2SPV(x)                  (((uint32_t)(((uint32_t)(x)) << SYSCFG_CFGR_CAN2SPV_SHIFT)) & SYSCFG_CFGR_CAN2SPV_MASK)

#define SYSCFG_CFGR_CAN1STOP_SHIFT              (18)
#define SYSCFG_CFGR_CAN1STOP_MASK               (0x01U << SYSCFG_CFGR_CAN1STOP_SHIFT)
#define SYSCFG_CFGR_CAN1STOP(x)                 (((uint32_t)(((uint32_t)(x)) << SYSCFG_CFGR_CAN1STOP_SHIFT)) & SYSCFG_CFGR_CAN1STOP_MASK)

#define SYSCFG_CFGR_CAN1TEST_SHIFT              (17)
#define SYSCFG_CFGR_CAN1TEST_MASK               (0x01U << SYSCFG_CFGR_CAN1TEST_SHIFT)
#define SYSCFG_CFGR_CAN1TEST(x)                 (((uint32_t)(((uint32_t)(x)) << SYSCFG_CFGR_CAN1TEST_SHIFT)) & SYSCFG_CFGR_CAN1TEST_MASK)

#define SYSCFG_CFGR_CAN1SPV_SHIFT               (16)
#define SYSCFG_CFGR_CAN1SPV_MASK                (0x01U << SYSCFG_CFGR_CAN1SPV_SHIFT)
#define SYSCFG_CFGR_CAN1SPV(x)                  (((uint32_t)(((uint32_t)(x)) << SYSCFG_CFGR_CAN1SPV_SHIFT)) & SYSCFG_CFGR_CAN1SPV_MASK)

#define SYSCFG_CFGR_USBFSIDORV_SHIFT            (3)
#define SYSCFG_CFGR_USBFSIDORV_MASK             (0x01U << SYSCFG_CFGR_USBFSIDORV_SHIFT)
#define SYSCFG_CFGR_USBFSIDORV(x)               (((uint32_t)(((uint32_t)(x)) << SYSCFG_CFGR_USBFSIDORV_SHIFT)) & SYSCFG_CFGR_USBFSIDORV_MASK)

#define SYSCFG_CFGR_USBFSVBUSORV_SHIFT          (2)
#define SYSCFG_CFGR_USBFSVBUSORV_MASK           (0x01U << SYSCFG_CFGR_USBFSVBUSORV_SHIFT)
#define SYSCFG_CFGR_USBFSVBUSORV(x)             (((uint32_t)(((uint32_t)(x)) << SYSCFG_CFGR_USBFSVBUSORV_SHIFT)) & SYSCFG_CFGR_USBFSVBUSORV_MASK)

#define SYSCFG_CFGR_USBFSIDOWDIS_SHIFT          (1)
#define SYSCFG_CFGR_USBFSIDOWDIS_MASK           (0x01U << SYSCFG_CFGR_USBFSIDOWDIS_SHIFT)
#define SYSCFG_CFGR_USBFSIDOWDIS(x)             (((uint32_t)(((uint32_t)(x)) << SYSCFG_CFGR_USBFSIDOWDIS_SHIFT)) & SYSCFG_CFGR_USBFSIDOWDIS_MASK)

#define SYSCFG_CFGR_USBFSVBUSOWDIS_SHIFT        (0)
#define SYSCFG_CFGR_USBFSVBUSOWDIS_MASK         (0x01U << SYSCFG_CFGR_USBFSVBUSOWDIS_SHIFT)
#define SYSCFG_CFGR_USBFSVBUSOWDIS(x)           (((uint32_t)(((uint32_t)(x)) << SYSCFG_CFGR_USBFSVBUSOWDIS_SHIFT)) & SYSCFG_CFGR_USBFSVBUSOWDIS_MASK)

/*!
 * @brief SYSCFG_EXTICR1 Register Bit Definition
 */

#define SYSCFG_EXTICR1_EXTIX_SHIFT              (0)
#define SYSCFG_EXTICR1_EXTIX_MASK               (0xFFFFU << SYSCFG_EXTICR1_EXTIX_SHIFT)
#define SYSCFG_EXTICR1_EXTIX(x)                 (((uint32_t)(((uint32_t)(x)) << SYSCFG_EXTICR1_EXTIX_SHIFT)) & SYSCFG_EXTICR1_EXTIX_MASK)

/*!
 * @brief SYSCFG_EXTICR2 Register Bit Definition
 */

#define SYSCFG_EXTICR2_EXTIX_SHIFT              (0)
#define SYSCFG_EXTICR2_EXTIX_MASK               (0xFFFFU << SYSCFG_EXTICR2_EXTIX_SHIFT)
#define SYSCFG_EXTICR2_EXTIX(x)                 (((uint32_t)(((uint32_t)(x)) << SYSCFG_EXTICR2_EXTIX_SHIFT)) & SYSCFG_EXTICR2_EXTIX_MASK)

/*!
 * @brief SYSCFG_EXTICR3 Register Bit Definition
 */

#define SYSCFG_EXTICR3_EXTIX_SHIFT              (0)
#define SYSCFG_EXTICR3_EXTIX_MASK               (0xFFFFU << SYSCFG_EXTICR3_EXTIX_SHIFT)
#define SYSCFG_EXTICR3_EXTIX(x)                 (((uint32_t)(((uint32_t)(x)) << SYSCFG_EXTICR3_EXTIX_SHIFT)) & SYSCFG_EXTICR3_EXTIX_MASK)

/*!
 * @brief SYSCFG_EXTICR4 Register Bit Definition
 */

#define SYSCFG_EXTICR4_EXTIX_SHIFT              (0)
#define SYSCFG_EXTICR4_EXTIX_MASK               (0xFFFFU << SYSCFG_EXTICR4_EXTIX_SHIFT)
#define SYSCFG_EXTICR4_EXTIX(x)                 (((uint32_t)(((uint32_t)(x)) << SYSCFG_EXTICR4_EXTIX_SHIFT)) & SYSCFG_EXTICR4_EXTIX_MASK)

/*!
 * @brief SYSCFG_CFGR2 Register Bit Definition
 */

#define SYSCFG_CFGR2_QSPIAUTOIDLEEN_SHIFT       (25)
#define SYSCFG_CFGR2_QSPIAUTOIDLEEN_MASK        (0x01U << SYSCFG_CFGR2_QSPIAUTOIDLEEN_SHIFT)
#define SYSCFG_CFGR2_QSPIAUTOIDLEEN(x)          (((uint32_t)(((uint32_t)(x)) << SYSCFG_CFGR2_QSPIAUTOIDLEEN_SHIFT)) & SYSCFG_CFGR2_QSPIAUTOIDLEEN_MASK)

#define SYSCFG_CFGR2_ENETSPDSEL_SHIFT           (21)
#define SYSCFG_CFGR2_ENETSPDSEL_MASK            (0x01U << SYSCFG_CFGR2_ENETSPDSEL_SHIFT)
#define SYSCFG_CFGR2_ENETSPDSEL(x)              (((uint32_t)(((uint32_t)(x)) << SYSCFG_CFGR2_ENETSPDSEL_SHIFT)) & SYSCFG_CFGR2_ENETSPDSEL_MASK)

#define SYSCFG_CFGR2_ENETMIIRMIISEL_SHIFT       (20)
#define SYSCFG_CFGR2_ENETMIIRMIISEL_MASK        (0x01U << SYSCFG_CFGR2_ENETMIIRMIISEL_SHIFT)
#define SYSCFG_CFGR2_ENETMIIRMIISEL(x)          (((uint32_t)(((uint32_t)(x)) << SYSCFG_CFGR2_ENETMIIRMIISEL_SHIFT)) & SYSCFG_CFGR2_ENETMIIRMIISEL_MASK)

#define SYSCFG_CFGR2_I2C2MODESEL_SHIFT          (17)
#define SYSCFG_CFGR2_I2C2MODESEL_MASK           (0x01U << SYSCFG_CFGR2_I2C2MODESEL_SHIFT)
#define SYSCFG_CFGR2_I2C2MODESEL(x)             (((uint32_t)(((uint32_t)(x)) << SYSCFG_CFGR2_I2C2MODESEL_SHIFT)) & SYSCFG_CFGR2_I2C2MODESEL_MASK)

#define SYSCFG_CFGR2_I2C1MODESEL_SHIFT          (16)
#define SYSCFG_CFGR2_I2C1MODESEL_MASK           (0x01U << SYSCFG_CFGR2_I2C1MODESEL_SHIFT)
#define SYSCFG_CFGR2_I2C1MODESEL(x)             (((uint32_t)(((uint32_t)(x)) << SYSCFG_CFGR2_I2C1MODESEL_SHIFT)) & SYSCFG_CFGR2_I2C1MODESEL_MASK)

/*!
 * @brief SYSCFG_PDETCSR Register Bit Definition
 */

#define SYSCFG_PDETCSR_VBATDIV3EN_SHIFT         (11)
#define SYSCFG_PDETCSR_VBATDIV3EN_MASK          (0x01U << SYSCFG_PDETCSR_VBATDIV3EN_SHIFT)
#define SYSCFG_PDETCSR_VBATDIV3EN(x)            (((uint32_t)(((uint32_t)(x)) << SYSCFG_PDETCSR_VBATDIV3EN_SHIFT)) & SYSCFG_PDETCSR_VBATDIV3EN_MASK)

#define SYSCFG_PDETCSR_VDTLS_SHIFT              (9)
#define SYSCFG_PDETCSR_VDTLS_MASK               (0x3U << SYSCFG_PDETCSR_VDTLS_SHIFT)
#define SYSCFG_PDETCSR_VDTLS(x)                 (((uint32_t)(((uint32_t)(x)) << SYSCFG_PDETCSR_VDTLS_SHIFT)) & SYSCFG_PDETCSR_VDTLS_MASK)

#define SYSCFG_PDETCSR_VDTE_SHIFT               (8)
#define SYSCFG_PDETCSR_VDTE_MASK                (0x01U << SYSCFG_PDETCSR_VDTE_SHIFT)
#define SYSCFG_PDETCSR_VDTE(x)                  (((uint32_t)(((uint32_t)(x)) << SYSCFG_PDETCSR_VDTE_SHIFT)) & SYSCFG_PDETCSR_VDTE_MASK)

#define SYSCFG_PDETCSR_VDTO_SHIFT               (6)
#define SYSCFG_PDETCSR_VDTO_MASK                (0x01U << SYSCFG_PDETCSR_VDTO_SHIFT)
#define SYSCFG_PDETCSR_VDTO(x)                  (((uint32_t)(((uint32_t)(x)) << SYSCFG_PDETCSR_VDTO_SHIFT)) & SYSCFG_PDETCSR_VDTO_MASK)

#define SYSCFG_PDETCSR_PVDO_SHIFT               (5)
#define SYSCFG_PDETCSR_PVDO_MASK                (0x01U << SYSCFG_PDETCSR_PVDO_SHIFT)
#define SYSCFG_PDETCSR_PVDO(x)                  (((uint32_t)(((uint32_t)(x)) << SYSCFG_PDETCSR_PVDO_SHIFT)) & SYSCFG_PDETCSR_PVDO_MASK)

#define SYSCFG_PDETCSR_PLS_SHIFT                (1)
#define SYSCFG_PDETCSR_PLS_MASK                 (0xFU << SYSCFG_PDETCSR_PLS_SHIFT)
#define SYSCFG_PDETCSR_PLS(x)                   (((uint32_t)(((uint32_t)(x)) << SYSCFG_PDETCSR_PLS_SHIFT)) & SYSCFG_PDETCSR_PLS_MASK)

#define SYSCFG_PDETCSR_PVDE_SHIFT               (0)
#define SYSCFG_PDETCSR_PVDE_MASK                (0x01U << SYSCFG_PDETCSR_PVDE_SHIFT)
#define SYSCFG_PDETCSR_PVDE(x)                  (((uint32_t)(((uint32_t)(x)) << SYSCFG_PDETCSR_PVDE_SHIFT)) & SYSCFG_PDETCSR_PVDE_MASK)

/*!
 * @brief SYSCFG_VOSDLY Register Bit Definition
 */

#define SYSCFG_VOSDLY_VOSDLYCNT_SHIFT           (0)
#define SYSCFG_VOSDLY_VOSDLYCNT_MASK            (0x3FFU << SYSCFG_VOSDLY_VOSDLYCNT_SHIFT)
#define SYSCFG_VOSDLY_VOSDLYCNT(x)              (((uint32_t)(((uint32_t)(x)) << SYSCFG_VOSDLY_VOSDLYCNT_SHIFT)) & SYSCFG_VOSDLY_VOSDLYCNT_MASK)

/*!
 * @brief SYSCFG_DMARMP Register Bit Definition
 */

#define SYSCFG_DMARMP_DACCH2RMP_SHIFT           (29)
#define SYSCFG_DMARMP_DACCH2RMP_MASK            (0x01U << SYSCFG_DMARMP_DACCH2RMP_SHIFT)
#define SYSCFG_DMARMP_DACCH2RMP(x)              (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_DACCH2RMP_SHIFT)) & SYSCFG_DMARMP_DACCH2RMP_MASK)

#define SYSCFG_DMARMP_DACCH1RMP_SHIFT           (28)
#define SYSCFG_DMARMP_DACCH1RMP_MASK            (0x01U << SYSCFG_DMARMP_DACCH1RMP_SHIFT)
#define SYSCFG_DMARMP_DACCH1RMP(x)              (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_DACCH1RMP_SHIFT)) & SYSCFG_DMARMP_DACCH1RMP_MASK)

#define SYSCFG_DMARMP_UART7TXRMP_SHIFT          (23)
#define SYSCFG_DMARMP_UART7TXRMP_MASK           (0x01U << SYSCFG_DMARMP_UART7TXRMP_SHIFT)
#define SYSCFG_DMARMP_UART7TXRMP(x)             (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_UART7TXRMP_SHIFT)) & SYSCFG_DMARMP_UART7TXRMP_MASK)

#define SYSCFG_DMARMP_UART7RXTMP_SHIFT          (22)
#define SYSCFG_DMARMP_UART7RXTMP_MASK           (0x01U << SYSCFG_DMARMP_UART7RXTMP_SHIFT)
#define SYSCFG_DMARMP_UART7RXTMP(x)             (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_UART7RXTMP_SHIFT)) & SYSCFG_DMARMP_UART7RXTMP_MASK)

#define SYSCFG_DMARMP_UART6TXRMP_SHIFT          (21)
#define SYSCFG_DMARMP_UART6TXRMP_MASK           (0x01U << SYSCFG_DMARMP_UART6TXRMP_SHIFT)
#define SYSCFG_DMARMP_UART6TXRMP(x)             (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_UART6TXRMP_SHIFT)) & SYSCFG_DMARMP_UART6TXRMP_MASK)

#define SYSCFG_DMARMP_UART6RXRMP_SHIFT          (20)
#define SYSCFG_DMARMP_UART6RXRMP_MASK           (0x01U << SYSCFG_DMARMP_UART6RXRMP_SHIFT)
#define SYSCFG_DMARMP_UART6RXRMP(x)             (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_UART6RXRMP_SHIFT)) & SYSCFG_DMARMP_UART6RXRMP_MASK)

#define SYSCFG_DMARMP_UART4TXRMP_SHIFT          (17)
#define SYSCFG_DMARMP_UART4TXRMP_MASK           (0x01U << SYSCFG_DMARMP_UART4TXRMP_SHIFT)
#define SYSCFG_DMARMP_UART4TXRMP(x)             (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_UART4TXRMP_SHIFT)) & SYSCFG_DMARMP_UART4TXRMP_MASK)

#define SYSCFG_DMARMP_UART4RXRMP_SHIFT          (16)
#define SYSCFG_DMARMP_UART4RXRMP_MASK           (0x01U << SYSCFG_DMARMP_UART4RXRMP_SHIFT)
#define SYSCFG_DMARMP_UART4RXRMP(x)             (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_UART4RXRMP_SHIFT)) & SYSCFG_DMARMP_UART4RXRMP_MASK)

#define SYSCFG_DMARMP_TIM8COMRMP_SHIFT          (15)
#define SYSCFG_DMARMP_TIM8COMRMP_MASK           (0x01U << SYSCFG_DMARMP_TIM8COMRMP_SHIFT)
#define SYSCFG_DMARMP_TIM8COMRMP(x)             (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_TIM8COMRMP_SHIFT)) & SYSCFG_DMARMP_TIM8COMRMP_MASK)

#define SYSCFG_DMARMP_TIM8UPRMP_SHIFT           (14)
#define SYSCFG_DMARMP_TIM8UPRMP_MASK            (0x01U << SYSCFG_DMARMP_TIM8UPRMP_SHIFT)
#define SYSCFG_DMARMP_TIM8UPRMP(x)              (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_TIM8UPRMP_SHIFT)) & SYSCFG_DMARMP_TIM8UPRMP_MASK)

#define SYSCFG_DMARMP_TIM8TRIGRMP_SHIFT         (13)
#define SYSCFG_DMARMP_TIM8TRIGRMP_MASK          (0x01U << SYSCFG_DMARMP_TIM8TRIGRMP_SHIFT)
#define SYSCFG_DMARMP_TIM8TRIGRMP(x)            (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_TIM8TRIGRMP_SHIFT)) & SYSCFG_DMARMP_TIM8TRIGRMP_MASK)

#define SYSCFG_DMARMP_TIM7UPRMP_SHIFT           (12)
#define SYSCFG_DMARMP_TIM7UPRMP_MASK            (0x01U << SYSCFG_DMARMP_TIM7UPRMP_SHIFT)
#define SYSCFG_DMARMP_TIM7UPRMP(x)              (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_TIM7UPRMP_SHIFT)) & SYSCFG_DMARMP_TIM7UPRMP_MASK)

#define SYSCFG_DMARMP_TIM6UPRMP_SHIFT           (10)
#define SYSCFG_DMARMP_TIM6UPRMP_MASK            (0x01U << SYSCFG_DMARMP_TIM6UPRMP_SHIFT)
#define SYSCFG_DMARMP_TIM6UPRMP(x)              (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_TIM6UPRMP_SHIFT)) & SYSCFG_DMARMP_TIM6UPRMP_MASK)

#define SYSCFG_DMARMP_TIM5UPRMP_SHIFT           (8)
#define SYSCFG_DMARMP_TIM5UPRMP_MASK            (0x01U << SYSCFG_DMARMP_TIM5UPRMP_SHIFT)
#define SYSCFG_DMARMP_TIM5UPRMP(x)              (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_TIM5UPRMP_SHIFT)) & SYSCFG_DMARMP_TIM5UPRMP_MASK)

#define SYSCFG_DMARMP_TIM2CC4RMP_SHIFT          (5)
#define SYSCFG_DMARMP_TIM2CC4RMP_MASK           (0x01U << SYSCFG_DMARMP_TIM2CC4RMP_SHIFT)
#define SYSCFG_DMARMP_TIM2CC4RMP(x)             (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_TIM2CC4RMP_SHIFT)) & SYSCFG_DMARMP_TIM2CC4RMP_MASK)

#define SYSCFG_DMARMP_TIM2CC3RMP_SHIFT          (4)
#define SYSCFG_DMARMP_TIM2CC3RMP_MASK           (0x01U << SYSCFG_DMARMP_TIM2CC3RMP_SHIFT)
#define SYSCFG_DMARMP_TIM2CC3RMP(x)             (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_TIM2CC3RMP_SHIFT)) & SYSCFG_DMARMP_TIM2CC3RMP_MASK)

#define SYSCFG_DMARMP_TIM2UPRMP_SHIFT           (2)
#define SYSCFG_DMARMP_TIM2UPRMP_MASK            (0x3U << SYSCFG_DMARMP_TIM2UPRMP_SHIFT)
#define SYSCFG_DMARMP_TIM2UPRMP(x)              (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_TIM2UPRMP_SHIFT)) & SYSCFG_DMARMP_TIM2UPRMP_MASK)

#define SYSCFG_DMARMP_TIM1COMRMP_SHIFT          (1)
#define SYSCFG_DMARMP_TIM1COMRMP_MASK           (0x01U << SYSCFG_DMARMP_TIM1COMRMP_SHIFT)
#define SYSCFG_DMARMP_TIM1COMRMP(x)             (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_TIM1COMRMP_SHIFT)) & SYSCFG_DMARMP_TIM1COMRMP_MASK)

#define SYSCFG_DMARMP_TIM1TRIGRMP_SHIFT         (0)
#define SYSCFG_DMARMP_TIM1TRIGRMP_MASK          (0x01U << SYSCFG_DMARMP_TIM1TRIGRMP_SHIFT)
#define SYSCFG_DMARMP_TIM1TRIGRMP(x)            (((uint32_t)(((uint32_t)(x)) << SYSCFG_DMARMP_TIM1TRIGRMP_SHIFT)) & SYSCFG_DMARMP_TIM1TRIGRMP_MASK)

/*!
 * @brief SYSCFG_BUS_PRIORITY Register Bit Definition
 */

#define SYSCFG_BUSPRI_PRIM6_SHIFT               (15)
#define SYSCFG_BUSPRI_PRIM6_MASK                (0x7U << SYSCFG_BUSPRI_PRIM6_SHIFT)
#define SYSCFG_BUSPRI_PRIM6(x)                  (((uint32_t)(((uint32_t)(x)) << SYSCFG_BUSPRI_PRIM6_SHIFT)) & SYSCFG_BUSPRI_PRIM6_MASK)

#define SYSCFG_BUSPRI_PRIM5_SHIFT               (12)
#define SYSCFG_BUSPRI_PRIM5_MASK                (0x7U << SYSCFG_BUSPRI_PRIM5_SHIFT)
#define SYSCFG_BUSPRI_PRIM5(x)                  (((uint32_t)(((uint32_t)(x)) << SYSCFG_BUSPRI_PRIM5_SHIFT)) & SYSCFG_BUSPRI_PRIM5_MASK)

#define SYSCFG_BUSPRI_PRIM4_SHIFT               (9)
#define SYSCFG_BUSPRI_PRIM4_MASK                (0x7U << SYSCFG_BUSPRI_PRIM4_SHIFT)
#define SYSCFG_BUSPRI_PRIM4(x)                  (((uint32_t)(((uint32_t)(x)) << SYSCFG_BUSPRI_PRIM4_SHIFT)) & SYSCFG_BUSPRI_PRIM4_MASK)

#define SYSCFG_BUSPRI_PRIM3_SHIFT               (6)
#define SYSCFG_BUSPRI_PRIM3_MASK                (0x7U << SYSCFG_BUSPRI_PRIM3_SHIFT)
#define SYSCFG_BUSPRI_PRIM3(x)                  (((uint32_t)(((uint32_t)(x)) << SYSCFG_BUSPRI_PRIM3_SHIFT)) & SYSCFG_BUSPRI_PRIM3_MASK)

#define SYSCFG_BUSPRI_PRIM2_SHIFT               (3)
#define SYSCFG_BUSPRI_PRIM2_MASK                (0x7U << SYSCFG_BUSPRI_PRIM2_SHIFT)
#define SYSCFG_BUSPRI_PRIM2(x)                  (((uint32_t)(((uint32_t)(x)) << SYSCFG_BUSPRI_PRIM2_SHIFT)) & SYSCFG_BUSPRI_PRIM2_MASK)

#define SYSCFG_BUSPRI_PRIM1_SHIFT               (0)
#define SYSCFG_BUSPRI_PRIM1_MASK                (0x7U << SYSCFG_BUSPRI_PRIM1_SHIFT)
#define SYSCFG_BUSPRI_PRIM1(x)                  (((uint32_t)(((uint32_t)(x)) << SYSCFG_BUSPRI_PRIM1_SHIFT)) & SYSCFG_BUSPRI_PRIM1_MASK)

/*!
 * @}
 */ /* end of group SYSCFG_Register_Masks */
/******************************************************************************
 *SYSCFG Instance
*******************************************************************************/

#define SYSCFG               ((SYSCFG_Type*)SYSCFG_BASE)

/*!
 * @}
 */ /* end of group SYSCFG_Peripheral_Access_Layer */




/*!
 * @addtogroup TIM1_Peripheral_Access_Layer TIM1 Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * TIM1 Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t CR1;                                                            ///< Control Register 1                           offset: 0x00
    __IO uint32_t CR2;                                                            ///< Control Register 2                           offset: 0x04
    __IO uint32_t SMCR;                                                           ///< Slave Mode Control Register                  offset: 0x08
    __IO uint32_t DIER;                                                           ///< DMA/Interrupt Enable Register                offset: 0x0C
    __IO uint32_t SR;                                                             ///< status register                              offset: 0x10
    __IO uint32_t EGR;                                                            ///< event generation register                    offset: 0x14
    __IO uint32_t CCMR1;                                                          ///< Capture/Compare Mode Register 1              offset: 0x18
    __IO uint32_t CCMR2;                                                          ///< Capture/Compare Mode Register 2              offset: 0x1C
    __IO uint32_t CCER;                                                           ///< Capture/Compare Enable Register              offset: 0x20
    __IO uint32_t CNT;                                                            ///< counter                                      offset: 0x24
    __IO uint32_t PSC;                                                            ///< Prescaler                                    offset: 0x28
    __IO uint32_t ARR;                                                            ///< autoload register                            offset: 0x2C
    __IO uint32_t RCR;                                                            ///< Repeat count register                        offset: 0x30
    __IO uint32_t CCR[4];                                                            ///< Capture/Compare Register 1                   offset: 0x34
    __IO uint32_t BDTR;                                                           ///< Brake and Deadband Registers                 offset: 0x44
    __IO uint32_t DCR;                                                            ///< DMA Control Register                         offset: 0x48
    __IO uint32_t DMAR;                                                           ///< DMA address for continuous mode              offset: 0x4C
    __IO uint32_t Reserved0[1];                                                   ///< Reserved
    __IO uint32_t CCMR3;                                                          ///< Capture/Compare Mode Register 3              offset: 0x54
    __IO uint32_t CCR5;                                                           ///< Capture/Compare Register 5                   offset: 0x58
    __IO uint32_t PDER;                                                           ///< PWM phase shift/DMA repeat update request enable registeroffset: 0x5C
    __IO uint32_t CCRFALL[5];                                                       ///< PWM Phase Shift Down Count Capture/Compare Registeroffset: 0x60
    __IO uint32_t BKINF;                                                          ///< brake input filter register                  offset: 0x74
} TIM1_Type;

/*******************************************************************************
 * TIM1 Type
 ******************************************************************************/

/*!
 * @addtogroup TIM1_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief TIM1_CR1 Register Bit Definition
 */

#define TIM1_CR1_CKD_SHIFT                      (8)
#define TIM1_CR1_CKD_MASK                       (0x3U << TIM1_CR1_CKD_SHIFT)
#define TIM1_CR1_CKD(x)                         (((uint32_t)(((uint32_t)(x)) << TIM1_CR1_CKD_SHIFT)) & TIM1_CR1_CKD_MASK)

#define TIM1_CR1_ARPE_SHIFT                     (7)
#define TIM1_CR1_ARPE_MASK                      (0x01U << TIM1_CR1_ARPE_SHIFT)
#define TIM1_CR1_ARPE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_CR1_ARPE_SHIFT)) & TIM1_CR1_ARPE_MASK)

#define TIM1_CR1_CMS_SHIFT                      (5)
#define TIM1_CR1_CMS_MASK                       (0x3U << TIM1_CR1_CMS_SHIFT)
#define TIM1_CR1_CMS(x)                         (((uint32_t)(((uint32_t)(x)) << TIM1_CR1_CMS_SHIFT)) & TIM1_CR1_CMS_MASK)

#define TIM1_CR1_DIR_SHIFT                      (4)
#define TIM1_CR1_DIR_MASK                       (0x01U << TIM1_CR1_DIR_SHIFT)
#define TIM1_CR1_DIR(x)                         (((uint32_t)(((uint32_t)(x)) << TIM1_CR1_DIR_SHIFT)) & TIM1_CR1_DIR_MASK)

#define TIM1_CR1_OPM_SHIFT                      (3)
#define TIM1_CR1_OPM_MASK                       (0x01U << TIM1_CR1_OPM_SHIFT)
#define TIM1_CR1_OPM(x)                         (((uint32_t)(((uint32_t)(x)) << TIM1_CR1_OPM_SHIFT)) & TIM1_CR1_OPM_MASK)

#define TIM1_CR1_URS_SHIFT                      (2)
#define TIM1_CR1_URS_MASK                       (0x01U << TIM1_CR1_URS_SHIFT)
#define TIM1_CR1_URS(x)                         (((uint32_t)(((uint32_t)(x)) << TIM1_CR1_URS_SHIFT)) & TIM1_CR1_URS_MASK)

#define TIM1_CR1_UDIS_SHIFT                     (1)
#define TIM1_CR1_UDIS_MASK                      (0x01U << TIM1_CR1_UDIS_SHIFT)
#define TIM1_CR1_UDIS(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_CR1_UDIS_SHIFT)) & TIM1_CR1_UDIS_MASK)

#define TIM1_CR1_CEN_SHIFT                      (0)
#define TIM1_CR1_CEN_MASK                       (0x01U << TIM1_CR1_CEN_SHIFT)
#define TIM1_CR1_CEN(x)                         (((uint32_t)(((uint32_t)(x)) << TIM1_CR1_CEN_SHIFT)) & TIM1_CR1_CEN_MASK)

/*!
 * @brief TIM1_CR2 Register Bit Definition
 */

#define TIM1_CR2_OIS4N_SHIFT                    (15)
#define TIM1_CR2_OIS4N_MASK                     (0x01U << TIM1_CR2_OIS4N_SHIFT)
#define TIM1_CR2_OIS4N(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_CR2_OIS4N_SHIFT)) & TIM1_CR2_OIS4N_MASK)

#define TIM1_CR2_OIS4_SHIFT                     (14)
#define TIM1_CR2_OIS4_MASK                      (0x01U << TIM1_CR2_OIS4_SHIFT)
#define TIM1_CR2_OIS4(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_CR2_OIS4_SHIFT)) & TIM1_CR2_OIS4_MASK)

#define TIM1_CR2_OIS3N_SHIFT                    (13)
#define TIM1_CR2_OIS3N_MASK                     (0x01U << TIM1_CR2_OIS3N_SHIFT)
#define TIM1_CR2_OIS3N(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_CR2_OIS3N_SHIFT)) & TIM1_CR2_OIS3N_MASK)

#define TIM1_CR2_OIS3_SHIFT                     (12)
#define TIM1_CR2_OIS3_MASK                      (0x01U << TIM1_CR2_OIS3_SHIFT)
#define TIM1_CR2_OIS3(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_CR2_OIS3_SHIFT)) & TIM1_CR2_OIS3_MASK)

#define TIM1_CR2_OIS2N_SHIFT                    (11)
#define TIM1_CR2_OIS2N_MASK                     (0x01U << TIM1_CR2_OIS2N_SHIFT)
#define TIM1_CR2_OIS2N(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_CR2_OIS2N_SHIFT)) & TIM1_CR2_OIS2N_MASK)

#define TIM1_CR2_OIS2_SHIFT                     (10)
#define TIM1_CR2_OIS2_MASK                      (0x01U << TIM1_CR2_OIS2_SHIFT)
#define TIM1_CR2_OIS2(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_CR2_OIS2_SHIFT)) & TIM1_CR2_OIS2_MASK)

#define TIM1_CR2_OIS1N_SHIFT                    (9)
#define TIM1_CR2_OIS1N_MASK                     (0x01U << TIM1_CR2_OIS1N_SHIFT)
#define TIM1_CR2_OIS1N(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_CR2_OIS1N_SHIFT)) & TIM1_CR2_OIS1N_MASK)

#define TIM1_CR2_OIS1_SHIFT                     (8)
#define TIM1_CR2_OIS1_MASK                      (0x01U << TIM1_CR2_OIS1_SHIFT)
#define TIM1_CR2_OIS1(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_CR2_OIS1_SHIFT)) & TIM1_CR2_OIS1_MASK)

#define TIM1_CR2_TI1S_SHIFT                     (7)
#define TIM1_CR2_TI1S_MASK                      (0x01U << TIM1_CR2_TI1S_SHIFT)
#define TIM1_CR2_TI1S(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_CR2_TI1S_SHIFT)) & TIM1_CR2_TI1S_MASK)

#define TIM1_CR2_MMS_SHIFT                      (4)
#define TIM1_CR2_MMS_MASK                       (0x7U << TIM1_CR2_MMS_SHIFT)
#define TIM1_CR2_MMS(x)                         (((uint32_t)(((uint32_t)(x)) << TIM1_CR2_MMS_SHIFT)) & TIM1_CR2_MMS_MASK)

#define TIM1_CR2_CCDS_SHIFT                     (3)
#define TIM1_CR2_CCDS_MASK                      (0x01U << TIM1_CR2_CCDS_SHIFT)
#define TIM1_CR2_CCDS(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_CR2_CCDS_SHIFT)) & TIM1_CR2_CCDS_MASK)

#define TIM1_CR2_CCUS_SHIFT                     (2)
#define TIM1_CR2_CCUS_MASK                      (0x01U << TIM1_CR2_CCUS_SHIFT)
#define TIM1_CR2_CCUS(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_CR2_CCUS_SHIFT)) & TIM1_CR2_CCUS_MASK)

#define TIM1_CR2_CCPC_SHIFT                     (0)
#define TIM1_CR2_CCPC_MASK                      (0x01U << TIM1_CR2_CCPC_SHIFT)
#define TIM1_CR2_CCPC(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_CR2_CCPC_SHIFT)) & TIM1_CR2_CCPC_MASK)

/*!
 * @brief TIM1_SMCR Register Bit Definition
 */

#define TIM1_SMCR_ETP_SHIFT                     (15)
#define TIM1_SMCR_ETP_MASK                      (0x01U << TIM1_SMCR_ETP_SHIFT)
#define TIM1_SMCR_ETP(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_SMCR_ETP_SHIFT)) & TIM1_SMCR_ETP_MASK)

#define TIM1_SMCR_ECE_SHIFT                     (14)
#define TIM1_SMCR_ECE_MASK                      (0x01U << TIM1_SMCR_ECE_SHIFT)
#define TIM1_SMCR_ECE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_SMCR_ECE_SHIFT)) & TIM1_SMCR_ECE_MASK)

#define TIM1_SMCR_ETPS_SHIFT                    (12)
#define TIM1_SMCR_ETPS_MASK                     (0x3U << TIM1_SMCR_ETPS_SHIFT)
#define TIM1_SMCR_ETPS(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_SMCR_ETPS_SHIFT)) & TIM1_SMCR_ETPS_MASK)

#define TIM1_SMCR_ETF_SHIFT                     (8)
#define TIM1_SMCR_ETF_MASK                      (0xFU << TIM1_SMCR_ETF_SHIFT)
#define TIM1_SMCR_ETF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_SMCR_ETF_SHIFT)) & TIM1_SMCR_ETF_MASK)

#define TIM1_SMCR_MSM_SHIFT                     (7)
#define TIM1_SMCR_MSM_MASK                      (0x01U << TIM1_SMCR_MSM_SHIFT)
#define TIM1_SMCR_MSM(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_SMCR_MSM_SHIFT)) & TIM1_SMCR_MSM_MASK)

#define TIM1_SMCR_TS_SHIFT                      (4)
#define TIM1_SMCR_TS_MASK                       (0x7U << TIM1_SMCR_TS_SHIFT)
#define TIM1_SMCR_TS(x)                         (((uint32_t)(((uint32_t)(x)) << TIM1_SMCR_TS_SHIFT)) & TIM1_SMCR_TS_MASK)

#define TIM1_SMCR_OCCS_SHIFT                    (3)
#define TIM1_SMCR_OCCS_MASK                     (0x01U << TIM1_SMCR_OCCS_SHIFT)
#define TIM1_SMCR_OCCS(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_SMCR_OCCS_SHIFT)) & TIM1_SMCR_OCCS_MASK)

#define TIM1_SMCR_SMS_SHIFT                     (0)
#define TIM1_SMCR_SMS_MASK                      (0x7U << TIM1_SMCR_SMS_SHIFT)
#define TIM1_SMCR_SMS(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_SMCR_SMS_SHIFT)) & TIM1_SMCR_SMS_MASK)

/*!
 * @brief TIM1_DIER Register Bit Definition
 */

#define TIM1_DIER_CC5DE_SHIFT                   (17)
#define TIM1_DIER_CC5DE_MASK                    (0x01U << TIM1_DIER_CC5DE_SHIFT)
#define TIM1_DIER_CC5DE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_DIER_CC5DE_SHIFT)) & TIM1_DIER_CC5DE_MASK)

#define TIM1_DIER_CC5IE_SHIFT                   (16)
#define TIM1_DIER_CC5IE_MASK                    (0x01U << TIM1_DIER_CC5IE_SHIFT)
#define TIM1_DIER_CC5IE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_DIER_CC5IE_SHIFT)) & TIM1_DIER_CC5IE_MASK)

#define TIM1_DIER_TDE_SHIFT                     (14)
#define TIM1_DIER_TDE_MASK                      (0x01U << TIM1_DIER_TDE_SHIFT)
#define TIM1_DIER_TDE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_DIER_TDE_SHIFT)) & TIM1_DIER_TDE_MASK)

#define TIM1_DIER_COMDE_SHIFT                   (13)
#define TIM1_DIER_COMDE_MASK                    (0x01U << TIM1_DIER_COMDE_SHIFT)
#define TIM1_DIER_COMDE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_DIER_COMDE_SHIFT)) & TIM1_DIER_COMDE_MASK)

#define TIM1_DIER_CC4DE_SHIFT                   (12)
#define TIM1_DIER_CC4DE_MASK                    (0x01U << TIM1_DIER_CC4DE_SHIFT)
#define TIM1_DIER_CC4DE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_DIER_CC4DE_SHIFT)) & TIM1_DIER_CC4DE_MASK)

#define TIM1_DIER_CC3DE_SHIFT                   (11)
#define TIM1_DIER_CC3DE_MASK                    (0x01U << TIM1_DIER_CC3DE_SHIFT)
#define TIM1_DIER_CC3DE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_DIER_CC3DE_SHIFT)) & TIM1_DIER_CC3DE_MASK)

#define TIM1_DIER_CC2DE_SHIFT                   (10)
#define TIM1_DIER_CC2DE_MASK                    (0x01U << TIM1_DIER_CC2DE_SHIFT)
#define TIM1_DIER_CC2DE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_DIER_CC2DE_SHIFT)) & TIM1_DIER_CC2DE_MASK)

#define TIM1_DIER_CC1DE_SHIFT                   (9)
#define TIM1_DIER_CC1DE_MASK                    (0x01U << TIM1_DIER_CC1DE_SHIFT)
#define TIM1_DIER_CC1DE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_DIER_CC1DE_SHIFT)) & TIM1_DIER_CC1DE_MASK)

#define TIM1_DIER_UDE_SHIFT                     (8)
#define TIM1_DIER_UDE_MASK                      (0x01U << TIM1_DIER_UDE_SHIFT)
#define TIM1_DIER_UDE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_DIER_UDE_SHIFT)) & TIM1_DIER_UDE_MASK)

#define TIM1_DIER_BIE_SHIFT                     (7)
#define TIM1_DIER_BIE_MASK                      (0x01U << TIM1_DIER_BIE_SHIFT)
#define TIM1_DIER_BIE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_DIER_BIE_SHIFT)) & TIM1_DIER_BIE_MASK)

#define TIM1_DIER_TIE_SHIFT                     (6)
#define TIM1_DIER_TIE_MASK                      (0x01U << TIM1_DIER_TIE_SHIFT)
#define TIM1_DIER_TIE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_DIER_TIE_SHIFT)) & TIM1_DIER_TIE_MASK)

#define TIM1_DIER_COMIE_SHIFT                   (5)
#define TIM1_DIER_COMIE_MASK                    (0x01U << TIM1_DIER_COMIE_SHIFT)
#define TIM1_DIER_COMIE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_DIER_COMIE_SHIFT)) & TIM1_DIER_COMIE_MASK)

#define TIM1_DIER_CC4IE_SHIFT                   (4)
#define TIM1_DIER_CC4IE_MASK                    (0x01U << TIM1_DIER_CC4IE_SHIFT)
#define TIM1_DIER_CC4IE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_DIER_CC4IE_SHIFT)) & TIM1_DIER_CC4IE_MASK)

#define TIM1_DIER_CC3IE_SHIFT                   (3)
#define TIM1_DIER_CC3IE_MASK                    (0x01U << TIM1_DIER_CC3IE_SHIFT)
#define TIM1_DIER_CC3IE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_DIER_CC3IE_SHIFT)) & TIM1_DIER_CC3IE_MASK)

#define TIM1_DIER_CC2IE_SHIFT                   (2)
#define TIM1_DIER_CC2IE_MASK                    (0x01U << TIM1_DIER_CC2IE_SHIFT)
#define TIM1_DIER_CC2IE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_DIER_CC2IE_SHIFT)) & TIM1_DIER_CC2IE_MASK)

#define TIM1_DIER_CC1IE_SHIFT                   (1)
#define TIM1_DIER_CC1IE_MASK                    (0x01U << TIM1_DIER_CC1IE_SHIFT)
#define TIM1_DIER_CC1IE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_DIER_CC1IE_SHIFT)) & TIM1_DIER_CC1IE_MASK)

#define TIM1_DIER_UIE_SHIFT                     (0)
#define TIM1_DIER_UIE_MASK                      (0x01U << TIM1_DIER_UIE_SHIFT)
#define TIM1_DIER_UIE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_DIER_UIE_SHIFT)) & TIM1_DIER_UIE_MASK)

/*!
 * @brief TIM1_SR Register Bit Definition
 */

#define TIM1_SR_CC5IF_SHIFT                     (16)
#define TIM1_SR_CC5IF_MASK                      (0x01U << TIM1_SR_CC5IF_SHIFT)
#define TIM1_SR_CC5IF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_SR_CC5IF_SHIFT)) & TIM1_SR_CC5IF_MASK)

#define TIM1_SR_CC4OF_SHIFT                     (12)
#define TIM1_SR_CC4OF_MASK                      (0x01U << TIM1_SR_CC4OF_SHIFT)
#define TIM1_SR_CC4OF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_SR_CC4OF_SHIFT)) & TIM1_SR_CC4OF_MASK)

#define TIM1_SR_CC3OF_SHIFT                     (11)
#define TIM1_SR_CC3OF_MASK                      (0x01U << TIM1_SR_CC3OF_SHIFT)
#define TIM1_SR_CC3OF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_SR_CC3OF_SHIFT)) & TIM1_SR_CC3OF_MASK)

#define TIM1_SR_CC2OF_SHIFT                     (10)
#define TIM1_SR_CC2OF_MASK                      (0x01U << TIM1_SR_CC2OF_SHIFT)
#define TIM1_SR_CC2OF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_SR_CC2OF_SHIFT)) & TIM1_SR_CC2OF_MASK)

#define TIM1_SR_CC1OF_SHIFT                     (9)
#define TIM1_SR_CC1OF_MASK                      (0x01U << TIM1_SR_CC1OF_SHIFT)
#define TIM1_SR_CC1OF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_SR_CC1OF_SHIFT)) & TIM1_SR_CC1OF_MASK)

#define TIM1_SR_BIF_SHIFT                       (7)
#define TIM1_SR_BIF_MASK                        (0x01U << TIM1_SR_BIF_SHIFT)
#define TIM1_SR_BIF(x)                          (((uint32_t)(((uint32_t)(x)) << TIM1_SR_BIF_SHIFT)) & TIM1_SR_BIF_MASK)

#define TIM1_SR_TIF_SHIFT                       (6)
#define TIM1_SR_TIF_MASK                        (0x01U << TIM1_SR_TIF_SHIFT)
#define TIM1_SR_TIF(x)                          (((uint32_t)(((uint32_t)(x)) << TIM1_SR_TIF_SHIFT)) & TIM1_SR_TIF_MASK)

#define TIM1_SR_COMIF_SHIFT                     (5)
#define TIM1_SR_COMIF_MASK                      (0x01U << TIM1_SR_COMIF_SHIFT)
#define TIM1_SR_COMIF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_SR_COMIF_SHIFT)) & TIM1_SR_COMIF_MASK)

#define TIM1_SR_CC4IF_SHIFT                     (4)
#define TIM1_SR_CC4IF_MASK                      (0x01U << TIM1_SR_CC4IF_SHIFT)
#define TIM1_SR_CC4IF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_SR_CC4IF_SHIFT)) & TIM1_SR_CC4IF_MASK)

#define TIM1_SR_CC3IF_SHIFT                     (3)
#define TIM1_SR_CC3IF_MASK                      (0x01U << TIM1_SR_CC3IF_SHIFT)
#define TIM1_SR_CC3IF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_SR_CC3IF_SHIFT)) & TIM1_SR_CC3IF_MASK)

#define TIM1_SR_CC2IF_SHIFT                     (2)
#define TIM1_SR_CC2IF_MASK                      (0x01U << TIM1_SR_CC2IF_SHIFT)
#define TIM1_SR_CC2IF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_SR_CC2IF_SHIFT)) & TIM1_SR_CC2IF_MASK)

#define TIM1_SR_CC1IF_SHIFT                     (1)
#define TIM1_SR_CC1IF_MASK                      (0x01U << TIM1_SR_CC1IF_SHIFT)
#define TIM1_SR_CC1IF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_SR_CC1IF_SHIFT)) & TIM1_SR_CC1IF_MASK)

#define TIM1_SR_UIF_SHIFT                       (0)
#define TIM1_SR_UIF_MASK                        (0x01U << TIM1_SR_UIF_SHIFT)
#define TIM1_SR_UIF(x)                          (((uint32_t)(((uint32_t)(x)) << TIM1_SR_UIF_SHIFT)) & TIM1_SR_UIF_MASK)

/*!
 * @brief TIM1_EGR Register Bit Definition
 */

#define TIM1_EGR_CC5G_SHIFT                     (16)
#define TIM1_EGR_CC5G_MASK                      (0x01U << TIM1_EGR_CC5G_SHIFT)
#define TIM1_EGR_CC5G(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_EGR_CC5G_SHIFT)) & TIM1_EGR_CC5G_MASK)

#define TIM1_EGR_BG_SHIFT                       (7)
#define TIM1_EGR_BG_MASK                        (0x01U << TIM1_EGR_BG_SHIFT)
#define TIM1_EGR_BG(x)                          (((uint32_t)(((uint32_t)(x)) << TIM1_EGR_BG_SHIFT)) & TIM1_EGR_BG_MASK)

#define TIM1_EGR_TG_SHIFT                       (6)
#define TIM1_EGR_TG_MASK                        (0x01U << TIM1_EGR_TG_SHIFT)
#define TIM1_EGR_TG(x)                          (((uint32_t)(((uint32_t)(x)) << TIM1_EGR_TG_SHIFT)) & TIM1_EGR_TG_MASK)

#define TIM1_EGR_COMG_SHIFT                     (5)
#define TIM1_EGR_COMG_MASK                      (0x01U << TIM1_EGR_COMG_SHIFT)
#define TIM1_EGR_COMG(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_EGR_COMG_SHIFT)) & TIM1_EGR_COMG_MASK)

#define TIM1_EGR_CC4G_SHIFT                     (4)
#define TIM1_EGR_CC4G_MASK                      (0x01U << TIM1_EGR_CC4G_SHIFT)
#define TIM1_EGR_CC4G(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_EGR_CC4G_SHIFT)) & TIM1_EGR_CC4G_MASK)

#define TIM1_EGR_CC3G_SHIFT                     (3)
#define TIM1_EGR_CC3G_MASK                      (0x01U << TIM1_EGR_CC3G_SHIFT)
#define TIM1_EGR_CC3G(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_EGR_CC3G_SHIFT)) & TIM1_EGR_CC3G_MASK)

#define TIM1_EGR_CC2G_SHIFT                     (2)
#define TIM1_EGR_CC2G_MASK                      (0x01U << TIM1_EGR_CC2G_SHIFT)
#define TIM1_EGR_CC2G(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_EGR_CC2G_SHIFT)) & TIM1_EGR_CC2G_MASK)

#define TIM1_EGR_CC1G_SHIFT                     (1)
#define TIM1_EGR_CC1G_MASK                      (0x01U << TIM1_EGR_CC1G_SHIFT)
#define TIM1_EGR_CC1G(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_EGR_CC1G_SHIFT)) & TIM1_EGR_CC1G_MASK)

#define TIM1_EGR_UG_SHIFT                       (0)
#define TIM1_EGR_UG_MASK                        (0x01U << TIM1_EGR_UG_SHIFT)
#define TIM1_EGR_UG(x)                          (((uint32_t)(((uint32_t)(x)) << TIM1_EGR_UG_SHIFT)) & TIM1_EGR_UG_MASK)

/*!
 * @brief TIM1_CCMR1 Register Bit Definition
 */

#define TIM1_CCMR1_OC2CE_SHIFT                  (15)
#define TIM1_CCMR1_OC2CE_MASK                   (0x01U << TIM1_CCMR1_OC2CE_SHIFT)
#define TIM1_CCMR1_OC2CE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR1_OC2CE_SHIFT)) & TIM1_CCMR1_OC2CE_MASK)

#define TIM1_CCMR1_OC2M_SHIFT                   (12)
#define TIM1_CCMR1_OC2M_MASK                    (0x7U << TIM1_CCMR1_OC2M_SHIFT)
#define TIM1_CCMR1_OC2M(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR1_OC2M_SHIFT)) & TIM1_CCMR1_OC2M_MASK)

#define TIM1_CCMR1_OC2PE_SHIFT                  (11)
#define TIM1_CCMR1_OC2PE_MASK                   (0x01U << TIM1_CCMR1_OC2PE_SHIFT)
#define TIM1_CCMR1_OC2PE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR1_OC2PE_SHIFT)) & TIM1_CCMR1_OC2PE_MASK)

#define TIM1_CCMR1_OC2FE_SHIFT                  (10)
#define TIM1_CCMR1_OC2FE_MASK                   (0x01U << TIM1_CCMR1_OC2FE_SHIFT)
#define TIM1_CCMR1_OC2FE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR1_OC2FE_SHIFT)) & TIM1_CCMR1_OC2FE_MASK)

#define TIM1_CCMR1_CC2S_SHIFT                   (8)
#define TIM1_CCMR1_CC2S_MASK                    (0x3U << TIM1_CCMR1_CC2S_SHIFT)
#define TIM1_CCMR1_CC2S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR1_CC2S_SHIFT)) & TIM1_CCMR1_CC2S_MASK)

#define TIM1_CCMR1_OC1CE_SHIFT                  (7)
#define TIM1_CCMR1_OC1CE_MASK                   (0x01U << TIM1_CCMR1_OC1CE_SHIFT)
#define TIM1_CCMR1_OC1CE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR1_OC1CE_SHIFT)) & TIM1_CCMR1_OC1CE_MASK)

#define TIM1_CCMR1_OC1M_SHIFT                   (4)
#define TIM1_CCMR1_OC1M_MASK                    (0x7U << TIM1_CCMR1_OC1M_SHIFT)
#define TIM1_CCMR1_OC1M(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR1_OC1M_SHIFT)) & TIM1_CCMR1_OC1M_MASK)

#define TIM1_CCMR1_OC1PE_SHIFT                  (3)
#define TIM1_CCMR1_OC1PE_MASK                   (0x01U << TIM1_CCMR1_OC1PE_SHIFT)
#define TIM1_CCMR1_OC1PE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR1_OC1PE_SHIFT)) & TIM1_CCMR1_OC1PE_MASK)

#define TIM1_CCMR1_OC1FE_SHIFT                  (2)
#define TIM1_CCMR1_OC1FE_MASK                   (0x01U << TIM1_CCMR1_OC1FE_SHIFT)
#define TIM1_CCMR1_OC1FE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR1_OC1FE_SHIFT)) & TIM1_CCMR1_OC1FE_MASK)

#define TIM1_CCMR1_CC1S_SHIFT                   (0)
#define TIM1_CCMR1_CC1S_MASK                    (0x3U << TIM1_CCMR1_CC1S_SHIFT)
#define TIM1_CCMR1_CC1S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR1_CC1S_SHIFT)) & TIM1_CCMR1_CC1S_MASK)

/*!
 * @brief TIM1_CCMR1 Register Bit Definition
 */

#define TIM1_CCMR1_IC2F_SHIFT                   (12)
#define TIM1_CCMR1_IC2F_MASK                    (0xFU << TIM1_CCMR1_IC2F_SHIFT)
#define TIM1_CCMR1_IC2F(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR1_IC2F_SHIFT)) & TIM1_CCMR1_IC2F_MASK)

#define TIM1_CCMR1_IC2PSC_SHIFT                 (10)
#define TIM1_CCMR1_IC2PSC_MASK                  (0x3U << TIM1_CCMR1_IC2PSC_SHIFT)
#define TIM1_CCMR1_IC2PSC(x)                    (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR1_IC2PSC_SHIFT)) & TIM1_CCMR1_IC2PSC_MASK)

#define TIM1_CCMR1_CC2S_SHIFT                   (8)
#define TIM1_CCMR1_CC2S_MASK                    (0x3U << TIM1_CCMR1_CC2S_SHIFT)
#define TIM1_CCMR1_CC2S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR1_CC2S_SHIFT)) & TIM1_CCMR1_CC2S_MASK)

#define TIM1_CCMR1_IC1F_SHIFT                   (4)
#define TIM1_CCMR1_IC1F_MASK                    (0xFU << TIM1_CCMR1_IC1F_SHIFT)
#define TIM1_CCMR1_IC1F(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR1_IC1F_SHIFT)) & TIM1_CCMR1_IC1F_MASK)

#define TIM1_CCMR1_IC1PSC_SHIFT                 (2)
#define TIM1_CCMR1_IC1PSC_MASK                  (0x3U << TIM1_CCMR1_IC1PSC_SHIFT)
#define TIM1_CCMR1_IC1PSC(x)                    (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR1_IC1PSC_SHIFT)) & TIM1_CCMR1_IC1PSC_MASK)

#define TIM1_CCMR1_CC1S_SHIFT                   (0)
#define TIM1_CCMR1_CC1S_MASK                    (0x3U << TIM1_CCMR1_CC1S_SHIFT)
#define TIM1_CCMR1_CC1S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR1_CC1S_SHIFT)) & TIM1_CCMR1_CC1S_MASK)

/*!
 * @brief TIM1_CCMR2 Register Bit Definition
 */

#define TIM1_CCMR2_OC4CE_SHIFT                  (15)
#define TIM1_CCMR2_OC4CE_MASK                   (0x01U << TIM1_CCMR2_OC4CE_SHIFT)
#define TIM1_CCMR2_OC4CE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR2_OC4CE_SHIFT)) & TIM1_CCMR2_OC4CE_MASK)

#define TIM1_CCMR2_OC4M_SHIFT                   (12)
#define TIM1_CCMR2_OC4M_MASK                    (0x7U << TIM1_CCMR2_OC4M_SHIFT)
#define TIM1_CCMR2_OC4M(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR2_OC4M_SHIFT)) & TIM1_CCMR2_OC4M_MASK)

#define TIM1_CCMR2_OC4PE_SHIFT                  (11)
#define TIM1_CCMR2_OC4PE_MASK                   (0x01U << TIM1_CCMR2_OC4PE_SHIFT)
#define TIM1_CCMR2_OC4PE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR2_OC4PE_SHIFT)) & TIM1_CCMR2_OC4PE_MASK)

#define TIM1_CCMR2_OC4FE_SHIFT                  (10)
#define TIM1_CCMR2_OC4FE_MASK                   (0x01U << TIM1_CCMR2_OC4FE_SHIFT)
#define TIM1_CCMR2_OC4FE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR2_OC4FE_SHIFT)) & TIM1_CCMR2_OC4FE_MASK)

#define TIM1_CCMR2_CC4S_SHIFT                   (8)
#define TIM1_CCMR2_CC4S_MASK                    (0x3U << TIM1_CCMR2_CC4S_SHIFT)
#define TIM1_CCMR2_CC4S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR2_CC4S_SHIFT)) & TIM1_CCMR2_CC4S_MASK)

#define TIM1_CCMR2_OC3CE_SHIFT                  (7)
#define TIM1_CCMR2_OC3CE_MASK                   (0x01U << TIM1_CCMR2_OC3CE_SHIFT)
#define TIM1_CCMR2_OC3CE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR2_OC3CE_SHIFT)) & TIM1_CCMR2_OC3CE_MASK)

#define TIM1_CCMR2_OC3M_SHIFT                   (4)
#define TIM1_CCMR2_OC3M_MASK                    (0x7U << TIM1_CCMR2_OC3M_SHIFT)
#define TIM1_CCMR2_OC3M(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR2_OC3M_SHIFT)) & TIM1_CCMR2_OC3M_MASK)

#define TIM1_CCMR2_OC3PE_SHIFT                  (3)
#define TIM1_CCMR2_OC3PE_MASK                   (0x01U << TIM1_CCMR2_OC3PE_SHIFT)
#define TIM1_CCMR2_OC3PE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR2_OC3PE_SHIFT)) & TIM1_CCMR2_OC3PE_MASK)

#define TIM1_CCMR2_OC3FE_SHIFT                  (2)
#define TIM1_CCMR2_OC3FE_MASK                   (0x01U << TIM1_CCMR2_OC3FE_SHIFT)
#define TIM1_CCMR2_OC3FE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR2_OC3FE_SHIFT)) & TIM1_CCMR2_OC3FE_MASK)

#define TIM1_CCMR2_CC3S_SHIFT                   (0)
#define TIM1_CCMR2_CC3S_MASK                    (0x3U << TIM1_CCMR2_CC3S_SHIFT)
#define TIM1_CCMR2_CC3S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR2_CC3S_SHIFT)) & TIM1_CCMR2_CC3S_MASK)

/*!
 * @brief TIM1_CCMR2 Register Bit Definition
 */

#define TIM1_CCMR2_IC4F_SHIFT                   (12)
#define TIM1_CCMR2_IC4F_MASK                    (0xFU << TIM1_CCMR2_IC4F_SHIFT)
#define TIM1_CCMR2_IC4F(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR2_IC4F_SHIFT)) & TIM1_CCMR2_IC4F_MASK)

#define TIM1_CCMR2_IC4PSC_SHIFT                 (10)
#define TIM1_CCMR2_IC4PSC_MASK                  (0x3U << TIM1_CCMR2_IC4PSC_SHIFT)
#define TIM1_CCMR2_IC4PSC(x)                    (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR2_IC4PSC_SHIFT)) & TIM1_CCMR2_IC4PSC_MASK)

#define TIM1_CCMR2_CC4S_SHIFT                   (8)
#define TIM1_CCMR2_CC4S_MASK                    (0x3U << TIM1_CCMR2_CC4S_SHIFT)
#define TIM1_CCMR2_CC4S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR2_CC4S_SHIFT)) & TIM1_CCMR2_CC4S_MASK)

#define TIM1_CCMR2_IC3F_SHIFT                   (4)
#define TIM1_CCMR2_IC3F_MASK                    (0xFU << TIM1_CCMR2_IC3F_SHIFT)
#define TIM1_CCMR2_IC3F(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR2_IC3F_SHIFT)) & TIM1_CCMR2_IC3F_MASK)

#define TIM1_CCMR2_IC3PSC_SHIFT                 (2)
#define TIM1_CCMR2_IC3PSC_MASK                  (0x3U << TIM1_CCMR2_IC3PSC_SHIFT)
#define TIM1_CCMR2_IC3PSC(x)                    (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR2_IC3PSC_SHIFT)) & TIM1_CCMR2_IC3PSC_MASK)

#define TIM1_CCMR2_CC3S_SHIFT                   (0)
#define TIM1_CCMR2_CC3S_MASK                    (0x3U << TIM1_CCMR2_CC3S_SHIFT)
#define TIM1_CCMR2_CC3S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR2_CC3S_SHIFT)) & TIM1_CCMR2_CC3S_MASK)

/*!
 * @brief TIM1_CCER Register Bit Definition
 */

#define TIM1_CCER_CC4NP_SHIFT                   (15)
#define TIM1_CCER_CC4NP_MASK                    (0x01U << TIM1_CCER_CC4NP_SHIFT)
#define TIM1_CCER_CC4NP(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCER_CC4NP_SHIFT)) & TIM1_CCER_CC4NP_MASK)

#define TIM1_CCER_CC4NE_SHIFT                   (14)
#define TIM1_CCER_CC4NE_MASK                    (0x01U << TIM1_CCER_CC4NE_SHIFT)
#define TIM1_CCER_CC4NE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCER_CC4NE_SHIFT)) & TIM1_CCER_CC4NE_MASK)

#define TIM1_CCER_CC4P_SHIFT                    (13)
#define TIM1_CCER_CC4P_MASK                     (0x01U << TIM1_CCER_CC4P_SHIFT)
#define TIM1_CCER_CC4P(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_CCER_CC4P_SHIFT)) & TIM1_CCER_CC4P_MASK)

#define TIM1_CCER_CC4E_SHIFT                    (12)
#define TIM1_CCER_CC4E_MASK                     (0x01U << TIM1_CCER_CC4E_SHIFT)
#define TIM1_CCER_CC4E(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_CCER_CC4E_SHIFT)) & TIM1_CCER_CC4E_MASK)

#define TIM1_CCER_CC3NP_SHIFT                   (11)
#define TIM1_CCER_CC3NP_MASK                    (0x01U << TIM1_CCER_CC3NP_SHIFT)
#define TIM1_CCER_CC3NP(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCER_CC3NP_SHIFT)) & TIM1_CCER_CC3NP_MASK)

#define TIM1_CCER_CC3NE_SHIFT                   (10)
#define TIM1_CCER_CC3NE_MASK                    (0x01U << TIM1_CCER_CC3NE_SHIFT)
#define TIM1_CCER_CC3NE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCER_CC3NE_SHIFT)) & TIM1_CCER_CC3NE_MASK)

#define TIM1_CCER_CC3P_SHIFT                    (9)
#define TIM1_CCER_CC3P_MASK                     (0x01U << TIM1_CCER_CC3P_SHIFT)
#define TIM1_CCER_CC3P(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_CCER_CC3P_SHIFT)) & TIM1_CCER_CC3P_MASK)

#define TIM1_CCER_CC3E_SHIFT                    (8)
#define TIM1_CCER_CC3E_MASK                     (0x01U << TIM1_CCER_CC3E_SHIFT)
#define TIM1_CCER_CC3E(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_CCER_CC3E_SHIFT)) & TIM1_CCER_CC3E_MASK)

#define TIM1_CCER_CC2NP_SHIFT                   (7)
#define TIM1_CCER_CC2NP_MASK                    (0x01U << TIM1_CCER_CC2NP_SHIFT)
#define TIM1_CCER_CC2NP(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCER_CC2NP_SHIFT)) & TIM1_CCER_CC2NP_MASK)

#define TIM1_CCER_CC2NE_SHIFT                   (6)
#define TIM1_CCER_CC2NE_MASK                    (0x01U << TIM1_CCER_CC2NE_SHIFT)
#define TIM1_CCER_CC2NE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCER_CC2NE_SHIFT)) & TIM1_CCER_CC2NE_MASK)

#define TIM1_CCER_CC2P_SHIFT                    (5)
#define TIM1_CCER_CC2P_MASK                     (0x01U << TIM1_CCER_CC2P_SHIFT)
#define TIM1_CCER_CC2P(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_CCER_CC2P_SHIFT)) & TIM1_CCER_CC2P_MASK)

#define TIM1_CCER_CC2E_SHIFT                    (4)
#define TIM1_CCER_CC2E_MASK                     (0x01U << TIM1_CCER_CC2E_SHIFT)
#define TIM1_CCER_CC2E(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_CCER_CC2E_SHIFT)) & TIM1_CCER_CC2E_MASK)

#define TIM1_CCER_CC1NP_SHIFT                   (3)
#define TIM1_CCER_CC1NP_MASK                    (0x01U << TIM1_CCER_CC1NP_SHIFT)
#define TIM1_CCER_CC1NP(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCER_CC1NP_SHIFT)) & TIM1_CCER_CC1NP_MASK)

#define TIM1_CCER_CC1NE_SHIFT                   (2)
#define TIM1_CCER_CC1NE_MASK                    (0x01U << TIM1_CCER_CC1NE_SHIFT)
#define TIM1_CCER_CC1NE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCER_CC1NE_SHIFT)) & TIM1_CCER_CC1NE_MASK)

#define TIM1_CCER_CC1P_SHIFT                    (1)
#define TIM1_CCER_CC1P_MASK                     (0x01U << TIM1_CCER_CC1P_SHIFT)
#define TIM1_CCER_CC1P(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_CCER_CC1P_SHIFT)) & TIM1_CCER_CC1P_MASK)

#define TIM1_CCER_CC1E_SHIFT                    (0)
#define TIM1_CCER_CC1E_MASK                     (0x01U << TIM1_CCER_CC1E_SHIFT)
#define TIM1_CCER_CC1E(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_CCER_CC1E_SHIFT)) & TIM1_CCER_CC1E_MASK)

/*!
 * @brief TIM1_CNT Register Bit Definition
 */

#define TIM1_CNT_CNT_SHIFT                      (0)
#define TIM1_CNT_CNT_MASK                       (0xFFFFU << TIM1_CNT_CNT_SHIFT)
#define TIM1_CNT_CNT(x)                         (((uint32_t)(((uint32_t)(x)) << TIM1_CNT_CNT_SHIFT)) & TIM1_CNT_CNT_MASK)

/*!
 * @brief TIM1_PSC Register Bit Definition
 */

#define TIM1_PSC_PSC_SHIFT                      (0)
#define TIM1_PSC_PSC_MASK                       (0xFFFFU << TIM1_PSC_PSC_SHIFT)
#define TIM1_PSC_PSC(x)                         (((uint32_t)(((uint32_t)(x)) << TIM1_PSC_PSC_SHIFT)) & TIM1_PSC_PSC_MASK)

/*!
 * @brief TIM1_ARR Register Bit Definition
 */

#define TIM1_ARR_ARR_SHIFT                      (0)
#define TIM1_ARR_ARR_MASK                       (0xFFFFU << TIM1_ARR_ARR_SHIFT)
#define TIM1_ARR_ARR(x)                         (((uint32_t)(((uint32_t)(x)) << TIM1_ARR_ARR_SHIFT)) & TIM1_ARR_ARR_MASK)

/*!
 * @brief TIM1_RCR Register Bit Definition
 */

#define TIM1_RCR_REPCNT_SHIFT                   (8)
#define TIM1_RCR_REPCNT_MASK                    (0xFFU << TIM1_RCR_REPCNT_SHIFT)
#define TIM1_RCR_REPCNT(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_RCR_REPCNT_SHIFT)) & TIM1_RCR_REPCNT_MASK)

#define TIM1_RCR_REP_SHIFT                      (0)
#define TIM1_RCR_REP_MASK                       (0xFFU << TIM1_RCR_REP_SHIFT)
#define TIM1_RCR_REP(x)                         (((uint32_t)(((uint32_t)(x)) << TIM1_RCR_REP_SHIFT)) & TIM1_RCR_REP_MASK)

/*!
 * @brief TIM1_CCR Register Bit Definition
 */

#define TIM1_CCR_CCR_SHIFT                      (0)
#define TIM1_CCR_CCR_MASK                       (0xFFFFU << TIM1_CCR_CCR_SHIFT)
#define TIM1_CCR_CCR(x)                         (((uint32_t)(((uint32_t)(x)) << TIM1_CCR_CCR_SHIFT)) & TIM1_CCR_CCR_MASK)

/*!
 * @brief TIM1_BDTR Register Bit Definition
 */

#define TIM1_BDTR_DOE_SHIFT                     (16)
#define TIM1_BDTR_DOE_MASK                      (0x01U << TIM1_BDTR_DOE_SHIFT)
#define TIM1_BDTR_DOE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_BDTR_DOE_SHIFT)) & TIM1_BDTR_DOE_MASK)

#define TIM1_BDTR_MOE_SHIFT                     (15)
#define TIM1_BDTR_MOE_MASK                      (0x01U << TIM1_BDTR_MOE_SHIFT)
#define TIM1_BDTR_MOE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_BDTR_MOE_SHIFT)) & TIM1_BDTR_MOE_MASK)

#define TIM1_BDTR_AOE_SHIFT                     (14)
#define TIM1_BDTR_AOE_MASK                      (0x01U << TIM1_BDTR_AOE_SHIFT)
#define TIM1_BDTR_AOE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_BDTR_AOE_SHIFT)) & TIM1_BDTR_AOE_MASK)

#define TIM1_BDTR_BKP_SHIFT                     (13)
#define TIM1_BDTR_BKP_MASK                      (0x01U << TIM1_BDTR_BKP_SHIFT)
#define TIM1_BDTR_BKP(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_BDTR_BKP_SHIFT)) & TIM1_BDTR_BKP_MASK)

#define TIM1_BDTR_BKE_SHIFT                     (12)
#define TIM1_BDTR_BKE_MASK                      (0x01U << TIM1_BDTR_BKE_SHIFT)
#define TIM1_BDTR_BKE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_BDTR_BKE_SHIFT)) & TIM1_BDTR_BKE_MASK)

#define TIM1_BDTR_OSSR_SHIFT                    (11)
#define TIM1_BDTR_OSSR_MASK                     (0x01U << TIM1_BDTR_OSSR_SHIFT)
#define TIM1_BDTR_OSSR(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_BDTR_OSSR_SHIFT)) & TIM1_BDTR_OSSR_MASK)

#define TIM1_BDTR_OSSI_SHIFT                    (10)
#define TIM1_BDTR_OSSI_MASK                     (0x01U << TIM1_BDTR_OSSI_SHIFT)
#define TIM1_BDTR_OSSI(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_BDTR_OSSI_SHIFT)) & TIM1_BDTR_OSSI_MASK)

#define TIM1_BDTR_LOCK_SHIFT                    (8)
#define TIM1_BDTR_LOCK_MASK                     (0x3U << TIM1_BDTR_LOCK_SHIFT)
#define TIM1_BDTR_LOCK(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_BDTR_LOCK_SHIFT)) & TIM1_BDTR_LOCK_MASK)

#define TIM1_BDTR_DTG_SHIFT                     (0)
#define TIM1_BDTR_DTG_MASK                      (0xFFU << TIM1_BDTR_DTG_SHIFT)
#define TIM1_BDTR_DTG(x)                        (((uint32_t)(((uint32_t)(x)) << TIM1_BDTR_DTG_SHIFT)) & TIM1_BDTR_DTG_MASK)

/*!
 * @brief TIM1_DCR Register Bit Definition
 */

#define TIM1_DCR_DBL_SHIFT                      (8)
#define TIM1_DCR_DBL_MASK                       (0x1FU << TIM1_DCR_DBL_SHIFT)
#define TIM1_DCR_DBL(x)                         (((uint32_t)(((uint32_t)(x)) << TIM1_DCR_DBL_SHIFT)) & TIM1_DCR_DBL_MASK)

#define TIM1_DCR_DBA_SHIFT                      (0)
#define TIM1_DCR_DBA_MASK                       (0x1FU << TIM1_DCR_DBA_SHIFT)
#define TIM1_DCR_DBA(x)                         (((uint32_t)(((uint32_t)(x)) << TIM1_DCR_DBA_SHIFT)) & TIM1_DCR_DBA_MASK)

/*!
 * @brief TIM1_DMAR Register Bit Definition
 */

#define TIM1_DMAR_DMAB_SHIFT                    (0)
#define TIM1_DMAR_DMAB_MASK                     (0xFFFFU << TIM1_DMAR_DMAB_SHIFT)
#define TIM1_DMAR_DMAB(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_DMAR_DMAB_SHIFT)) & TIM1_DMAR_DMAB_MASK)

/*!
 * @brief TIM1_CCMR3 Register Bit Definition
 */

#define TIM1_CCMR3_OC5M_SHIFT                   (4)
#define TIM1_CCMR3_OC5M_MASK                    (0x7U << TIM1_CCMR3_OC5M_SHIFT)
#define TIM1_CCMR3_OC5M(x)                      (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR3_OC5M_SHIFT)) & TIM1_CCMR3_OC5M_MASK)

#define TIM1_CCMR3_OC5PE_SHIFT                  (3)
#define TIM1_CCMR3_OC5PE_MASK                   (0x01U << TIM1_CCMR3_OC5PE_SHIFT)
#define TIM1_CCMR3_OC5PE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR3_OC5PE_SHIFT)) & TIM1_CCMR3_OC5PE_MASK)

#define TIM1_CCMR3_OC5FE_SHIFT                  (2)
#define TIM1_CCMR3_OC5FE_MASK                   (0x01U << TIM1_CCMR3_OC5FE_SHIFT)
#define TIM1_CCMR3_OC5FE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM1_CCMR3_OC5FE_SHIFT)) & TIM1_CCMR3_OC5FE_MASK)

/*!
 * @brief TIM1_CCR5 Register Bit Definition
 */

#define TIM1_CCR5_CCR5_SHIFT                    (0)
#define TIM1_CCR5_CCR5_MASK                     (0xFFFFU << TIM1_CCR5_CCR5_SHIFT)
#define TIM1_CCR5_CCR5(x)                       (((uint32_t)(((uint32_t)(x)) << TIM1_CCR5_CCR5_SHIFT)) & TIM1_CCR5_CCR5_MASK)

/*!
 * @brief TIM1_PDER Register Bit Definition
 */

#define TIM1_PDER_CCR5SHIFTEN_SHIFT             (5)
#define TIM1_PDER_CCR5SHIFTEN_MASK              (0x01U << TIM1_PDER_CCR5SHIFTEN_SHIFT)
#define TIM1_PDER_CCR5SHIFTEN(x)                (((uint32_t)(((uint32_t)(x)) << TIM1_PDER_CCR5SHIFTEN_SHIFT)) & TIM1_PDER_CCR5SHIFTEN_MASK)

#define TIM1_PDER_CCR4SHIFTEN_SHIFT             (4)
#define TIM1_PDER_CCR4SHIFTEN_MASK              (0x01U << TIM1_PDER_CCR4SHIFTEN_SHIFT)
#define TIM1_PDER_CCR4SHIFTEN(x)                (((uint32_t)(((uint32_t)(x)) << TIM1_PDER_CCR4SHIFTEN_SHIFT)) & TIM1_PDER_CCR4SHIFTEN_MASK)

#define TIM1_PDER_CCR3SHIFTEN_SHIFT             (3)
#define TIM1_PDER_CCR3SHIFTEN_MASK              (0x01U << TIM1_PDER_CCR3SHIFTEN_SHIFT)
#define TIM1_PDER_CCR3SHIFTEN(x)                (((uint32_t)(((uint32_t)(x)) << TIM1_PDER_CCR3SHIFTEN_SHIFT)) & TIM1_PDER_CCR3SHIFTEN_MASK)

#define TIM1_PDER_CCR2SHIFTEN_SHIFT             (2)
#define TIM1_PDER_CCR2SHIFTEN_MASK              (0x01U << TIM1_PDER_CCR2SHIFTEN_SHIFT)
#define TIM1_PDER_CCR2SHIFTEN(x)                (((uint32_t)(((uint32_t)(x)) << TIM1_PDER_CCR2SHIFTEN_SHIFT)) & TIM1_PDER_CCR2SHIFTEN_MASK)

#define TIM1_PDER_CCR1SHIFTEN_SHIFT             (1)
#define TIM1_PDER_CCR1SHIFTEN_MASK              (0x01U << TIM1_PDER_CCR1SHIFTEN_SHIFT)
#define TIM1_PDER_CCR1SHIFTEN(x)                (((uint32_t)(((uint32_t)(x)) << TIM1_PDER_CCR1SHIFTEN_SHIFT)) & TIM1_PDER_CCR1SHIFTEN_MASK)

#define TIM1_PDER_CCDREPE_SHIFT                 (0)
#define TIM1_PDER_CCDREPE_MASK                  (0x01U << TIM1_PDER_CCDREPE_SHIFT)
#define TIM1_PDER_CCDREPE(x)                    (((uint32_t)(((uint32_t)(x)) << TIM1_PDER_CCDREPE_SHIFT)) & TIM1_PDER_CCDREPE_MASK)

/*!
 * @brief TIM1_CCRFALL Register Bit Definition
 */

#define TIM1_CCRFALL_CCRFALL_SHIFT              (0)
#define TIM1_CCRFALL_CCRFALL_MASK               (0xFFFFU << TIM1_CCRFALL_CCRFALL_SHIFT)
#define TIM1_CCRFALL_CCRFALL(x)                 (((uint32_t)(((uint32_t)(x)) << TIM1_CCRFALL_CCRFALL_SHIFT)) & TIM1_CCRFALL_CCRFALL_MASK)

/*!
 * @brief TIM1_BKINF Register Bit Definition
 */

#define TIM1_BKINF_COMPBKINSEL_SHIFT             (13)
#define TIM1_BKINF_COMPBKINSEL_MASK              (0x7U << TIM1_BKINF_COMPBKINSEL_SHIFT)
#define TIM1_BKINF_COMPBKINSEL(x)                (((uint32_t)(((uint32_t)(x)) << TIM1_BKINF_COMPBKINSEL_SHIFT)) & TIM1_BKINF_COMPBKINSEL_MASK)

#define TIM1_BKINF_IOBKINSEL_SHIFT              (6)
#define TIM1_BKINF_IOBKINSEL_MASK               (0xFU << TIM1_BKINF_IOBKINSEL_SHIFT)
#define TIM1_BKINF_IOBKINSEL(x)                 (((uint32_t)(((uint32_t)(x)) << TIM1_BKINF_IOBKINSEL_SHIFT)) & TIM1_BKINF_IOBKINSEL_MASK)

#define TIM1_BKINF_CSSBKINSEL_SHIFT             (5)
#define TIM1_BKINF_CSSBKINSEL_MASK              (0x01U << TIM1_BKINF_CSSBKINSEL_SHIFT)
#define TIM1_BKINF_CSSBKINSEL(x)                (((uint32_t)(((uint32_t)(x)) << TIM1_BKINF_CSSBKINSEL_SHIFT)) & TIM1_BKINF_CSSBKINSEL_MASK)

#define TIM1_BKINF_BKINF_SHIFT                  (1)
#define TIM1_BKINF_BKINF_MASK                   (0xFU << TIM1_BKINF_BKINF_SHIFT)
#define TIM1_BKINF_BKINF(x)                     (((uint32_t)(((uint32_t)(x)) << TIM1_BKINF_BKINF_SHIFT)) & TIM1_BKINF_BKINF_MASK)

#define TIM1_BKINF_BKINFE_SHIFT                 (0)
#define TIM1_BKINF_BKINFE_MASK                  (0x01U << TIM1_BKINF_BKINFE_SHIFT)
#define TIM1_BKINF_BKINFE(x)                    (((uint32_t)(((uint32_t)(x)) << TIM1_BKINF_BKINFE_SHIFT)) & TIM1_BKINF_BKINFE_MASK)

/*!
 * @}
 */ /* end of group TIM1_Register_Masks */
/******************************************************************************
 *TIM1 Instance
*******************************************************************************/

#define TIM1                 ((TIM1_Type*)TIM1_BASE)
#define TIM8                 ((TIM1_Type*)TIM8_BASE)

/*!
 * @}
 */ /* end of group TIM1_Peripheral_Access_Layer */

/*!
 * @addtogroup TIM2_Peripheral_Access_Layer TIM2 Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * TIM2 Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t CR1;                                                            ///< Control Register 1                           offset: 0x00
    __IO uint32_t CR2;                                                            ///< Control Register 2                           offset: 0x04
    __IO uint32_t SMCR;                                                           ///< Slave Mode Control Register                  offset: 0x08
    __IO uint32_t DIER;                                                           ///< DMA/Interrupt Enable Register                offset: 0x0C
    __IO uint32_t SR;                                                             ///< status register                              offset: 0x10
    __IO uint32_t EGR;                                                            ///< event generation register                    offset: 0x14
    __IO uint32_t CCMR1;                                                          ///< Capture/Compare Mode Register 1              offset: 0x18
    __IO uint32_t CCMR2;                                                          ///< Capture/Compare Mode Register 2              offset: 0x1C
    __IO uint32_t CCER;                                                           ///< Capture/Compare Enable Register              offset: 0x20
    __IO uint32_t CNT;                                                            ///< counter                                      offset: 0x24
    __IO uint32_t PSC;                                                            ///< Prescaler                                    offset: 0x28
    __IO uint32_t ARR;                                                            ///< autoload register                            offset: 0x2C
    __IO uint32_t Reserved51[1];                                                   ///< Reserved
    __IO uint32_t CCR[4];                                                           ///< Capture/Compare Register 1                   offset: 0x34
    __IO uint32_t Reserved52[1];                                                   ///< Reserved
    __IO uint32_t DCR;                                                            ///< DMA Control Register                         offset: 0x48
    __IO uint32_t DMAR;                                                           ///< DMA address for continuous mode              offset: 0x4C
    __IO uint32_t OR;                                                             ///< TIMERx option register                       offset: 0x50
} TIM2_Type;

/*******************************************************************************
 * TIM2 Type
 ******************************************************************************/

/*!
 * @addtogroup TIM2_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief TIM2_CR1 Register Bit Definition
 */

#define TIM2_CR1_CKD_SHIFT                      (8)
#define TIM2_CR1_CKD_MASK                       (0x3U << TIM2_CR1_CKD_SHIFT)
#define TIM2_CR1_CKD(x)                         (((uint32_t)(((uint32_t)(x)) << TIM2_CR1_CKD_SHIFT)) & TIM2_CR1_CKD_MASK)

#define TIM2_CR1_ARPE_SHIFT                     (7)
#define TIM2_CR1_ARPE_MASK                      (0x01U << TIM2_CR1_ARPE_SHIFT)
#define TIM2_CR1_ARPE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_CR1_ARPE_SHIFT)) & TIM2_CR1_ARPE_MASK)

#define TIM2_CR1_CMS_SHIFT                      (5)
#define TIM2_CR1_CMS_MASK                       (0x3U << TIM2_CR1_CMS_SHIFT)
#define TIM2_CR1_CMS(x)                         (((uint32_t)(((uint32_t)(x)) << TIM2_CR1_CMS_SHIFT)) & TIM2_CR1_CMS_MASK)

#define TIM2_CR1_DIR_SHIFT                      (4)
#define TIM2_CR1_DIR_MASK                       (0x01U << TIM2_CR1_DIR_SHIFT)
#define TIM2_CR1_DIR(x)                         (((uint32_t)(((uint32_t)(x)) << TIM2_CR1_DIR_SHIFT)) & TIM2_CR1_DIR_MASK)

#define TIM2_CR1_OPM_SHIFT                      (3)
#define TIM2_CR1_OPM_MASK                       (0x01U << TIM2_CR1_OPM_SHIFT)
#define TIM2_CR1_OPM(x)                         (((uint32_t)(((uint32_t)(x)) << TIM2_CR1_OPM_SHIFT)) & TIM2_CR1_OPM_MASK)

#define TIM2_CR1_URS_SHIFT                      (2)
#define TIM2_CR1_URS_MASK                       (0x01U << TIM2_CR1_URS_SHIFT)
#define TIM2_CR1_URS(x)                         (((uint32_t)(((uint32_t)(x)) << TIM2_CR1_URS_SHIFT)) & TIM2_CR1_URS_MASK)

#define TIM2_CR1_UDIS_SHIFT                     (1)
#define TIM2_CR1_UDIS_MASK                      (0x01U << TIM2_CR1_UDIS_SHIFT)
#define TIM2_CR1_UDIS(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_CR1_UDIS_SHIFT)) & TIM2_CR1_UDIS_MASK)

#define TIM2_CR1_CEN_SHIFT                      (0)
#define TIM2_CR1_CEN_MASK                       (0x01U << TIM2_CR1_CEN_SHIFT)
#define TIM2_CR1_CEN(x)                         (((uint32_t)(((uint32_t)(x)) << TIM2_CR1_CEN_SHIFT)) & TIM2_CR1_CEN_MASK)

/*!
 * @brief TIM2_CR2 Register Bit Definition
 */

#define TIM2_CR2_TI1S_SHIFT                     (7)
#define TIM2_CR2_TI1S_MASK                      (0x01U << TIM2_CR2_TI1S_SHIFT)
#define TIM2_CR2_TI1S(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_CR2_TI1S_SHIFT)) & TIM2_CR2_TI1S_MASK)

#define TIM2_CR2_MMS_SHIFT                      (4)
#define TIM2_CR2_MMS_MASK                       (0x7U << TIM2_CR2_MMS_SHIFT)
#define TIM2_CR2_MMS(x)                         (((uint32_t)(((uint32_t)(x)) << TIM2_CR2_MMS_SHIFT)) & TIM2_CR2_MMS_MASK)

#define TIM2_CR2_CCDS_SHIFT                     (3)
#define TIM2_CR2_CCDS_MASK                      (0x01U << TIM2_CR2_CCDS_SHIFT)
#define TIM2_CR2_CCDS(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_CR2_CCDS_SHIFT)) & TIM2_CR2_CCDS_MASK)

/*!
 * @brief TIM2_SMCR Register Bit Definition
 */

#define TIM2_SMCR_ETP_SHIFT                     (15)
#define TIM2_SMCR_ETP_MASK                      (0x01U << TIM2_SMCR_ETP_SHIFT)
#define TIM2_SMCR_ETP(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_SMCR_ETP_SHIFT)) & TIM2_SMCR_ETP_MASK)

#define TIM2_SMCR_ECE_SHIFT                     (14)
#define TIM2_SMCR_ECE_MASK                      (0x01U << TIM2_SMCR_ECE_SHIFT)
#define TIM2_SMCR_ECE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_SMCR_ECE_SHIFT)) & TIM2_SMCR_ECE_MASK)

#define TIM2_SMCR_ETPS_SHIFT                    (12)
#define TIM2_SMCR_ETPS_MASK                     (0x3U << TIM2_SMCR_ETPS_SHIFT)
#define TIM2_SMCR_ETPS(x)                       (((uint32_t)(((uint32_t)(x)) << TIM2_SMCR_ETPS_SHIFT)) & TIM2_SMCR_ETPS_MASK)

#define TIM2_SMCR_ETF_SHIFT                     (8)
#define TIM2_SMCR_ETF_MASK                      (0xFU << TIM2_SMCR_ETF_SHIFT)
#define TIM2_SMCR_ETF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_SMCR_ETF_SHIFT)) & TIM2_SMCR_ETF_MASK)

#define TIM2_SMCR_MSM_SHIFT                     (7)
#define TIM2_SMCR_MSM_MASK                      (0x01U << TIM2_SMCR_MSM_SHIFT)
#define TIM2_SMCR_MSM(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_SMCR_MSM_SHIFT)) & TIM2_SMCR_MSM_MASK)

#define TIM2_SMCR_TS_SHIFT                      (4)
#define TIM2_SMCR_TS_MASK                       (0x7U << TIM2_SMCR_TS_SHIFT)
#define TIM2_SMCR_TS(x)                         (((uint32_t)(((uint32_t)(x)) << TIM2_SMCR_TS_SHIFT)) & TIM2_SMCR_TS_MASK)

#define TIM2_SMCR_OCCS_SHIFT                    (3)
#define TIM2_SMCR_OCCS_MASK                     (0x01U << TIM2_SMCR_OCCS_SHIFT)
#define TIM2_SMCR_OCCS(x)                       (((uint32_t)(((uint32_t)(x)) << TIM2_SMCR_OCCS_SHIFT)) & TIM2_SMCR_OCCS_MASK)

#define TIM2_SMCR_SMS_SHIFT                     (0)
#define TIM2_SMCR_SMS_MASK                      (0x7U << TIM2_SMCR_SMS_SHIFT)
#define TIM2_SMCR_SMS(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_SMCR_SMS_SHIFT)) & TIM2_SMCR_SMS_MASK)

/*!
 * @brief TIM2_DIER Register Bit Definition
 */

#define TIM2_DIER_TDE_SHIFT                     (14)
#define TIM2_DIER_TDE_MASK                      (0x01U << TIM2_DIER_TDE_SHIFT)
#define TIM2_DIER_TDE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_DIER_TDE_SHIFT)) & TIM2_DIER_TDE_MASK)

#define TIM2_DIER_CC4DE_SHIFT                   (12)
#define TIM2_DIER_CC4DE_MASK                    (0x01U << TIM2_DIER_CC4DE_SHIFT)
#define TIM2_DIER_CC4DE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_DIER_CC4DE_SHIFT)) & TIM2_DIER_CC4DE_MASK)

#define TIM2_DIER_CC3DE_SHIFT                   (11)
#define TIM2_DIER_CC3DE_MASK                    (0x01U << TIM2_DIER_CC3DE_SHIFT)
#define TIM2_DIER_CC3DE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_DIER_CC3DE_SHIFT)) & TIM2_DIER_CC3DE_MASK)

#define TIM2_DIER_CC2DE_SHIFT                   (10)
#define TIM2_DIER_CC2DE_MASK                    (0x01U << TIM2_DIER_CC2DE_SHIFT)
#define TIM2_DIER_CC2DE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_DIER_CC2DE_SHIFT)) & TIM2_DIER_CC2DE_MASK)

#define TIM2_DIER_CC1DE_SHIFT                   (9)
#define TIM2_DIER_CC1DE_MASK                    (0x01U << TIM2_DIER_CC1DE_SHIFT)
#define TIM2_DIER_CC1DE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_DIER_CC1DE_SHIFT)) & TIM2_DIER_CC1DE_MASK)

#define TIM2_DIER_UDE_SHIFT                     (8)
#define TIM2_DIER_UDE_MASK                      (0x01U << TIM2_DIER_UDE_SHIFT)
#define TIM2_DIER_UDE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_DIER_UDE_SHIFT)) & TIM2_DIER_UDE_MASK)

#define TIM2_DIER_TIE_SHIFT                     (6)
#define TIM2_DIER_TIE_MASK                      (0x01U << TIM2_DIER_TIE_SHIFT)
#define TIM2_DIER_TIE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_DIER_TIE_SHIFT)) & TIM2_DIER_TIE_MASK)

#define TIM2_DIER_CC4IE_SHIFT                   (4)
#define TIM2_DIER_CC4IE_MASK                    (0x01U << TIM2_DIER_CC4IE_SHIFT)
#define TIM2_DIER_CC4IE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_DIER_CC4IE_SHIFT)) & TIM2_DIER_CC4IE_MASK)

#define TIM2_DIER_CC3IE_SHIFT                   (3)
#define TIM2_DIER_CC3IE_MASK                    (0x01U << TIM2_DIER_CC3IE_SHIFT)
#define TIM2_DIER_CC3IE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_DIER_CC3IE_SHIFT)) & TIM2_DIER_CC3IE_MASK)

#define TIM2_DIER_CC2IE_SHIFT                   (2)
#define TIM2_DIER_CC2IE_MASK                    (0x01U << TIM2_DIER_CC2IE_SHIFT)
#define TIM2_DIER_CC2IE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_DIER_CC2IE_SHIFT)) & TIM2_DIER_CC2IE_MASK)

#define TIM2_DIER_CC1IE_SHIFT                   (1)
#define TIM2_DIER_CC1IE_MASK                    (0x01U << TIM2_DIER_CC1IE_SHIFT)
#define TIM2_DIER_CC1IE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_DIER_CC1IE_SHIFT)) & TIM2_DIER_CC1IE_MASK)

#define TIM2_DIER_UIE_SHIFT                     (0)
#define TIM2_DIER_UIE_MASK                      (0x01U << TIM2_DIER_UIE_SHIFT)
#define TIM2_DIER_UIE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_DIER_UIE_SHIFT)) & TIM2_DIER_UIE_MASK)

/*!
 * @brief TIM2_SR Register Bit Definition
 */

#define TIM2_SR_CC4OF_SHIFT                     (12)
#define TIM2_SR_CC4OF_MASK                      (0x01U << TIM2_SR_CC4OF_SHIFT)
#define TIM2_SR_CC4OF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_SR_CC4OF_SHIFT)) & TIM2_SR_CC4OF_MASK)

#define TIM2_SR_CC3OF_SHIFT                     (11)
#define TIM2_SR_CC3OF_MASK                      (0x01U << TIM2_SR_CC3OF_SHIFT)
#define TIM2_SR_CC3OF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_SR_CC3OF_SHIFT)) & TIM2_SR_CC3OF_MASK)

#define TIM2_SR_CC2OF_SHIFT                     (10)
#define TIM2_SR_CC2OF_MASK                      (0x01U << TIM2_SR_CC2OF_SHIFT)
#define TIM2_SR_CC2OF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_SR_CC2OF_SHIFT)) & TIM2_SR_CC2OF_MASK)

#define TIM2_SR_CC1OF_SHIFT                     (9)
#define TIM2_SR_CC1OF_MASK                      (0x01U << TIM2_SR_CC1OF_SHIFT)
#define TIM2_SR_CC1OF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_SR_CC1OF_SHIFT)) & TIM2_SR_CC1OF_MASK)

#define TIM2_SR_TIF_SHIFT                       (6)
#define TIM2_SR_TIF_MASK                        (0x01U << TIM2_SR_TIF_SHIFT)
#define TIM2_SR_TIF(x)                          (((uint32_t)(((uint32_t)(x)) << TIM2_SR_TIF_SHIFT)) & TIM2_SR_TIF_MASK)

#define TIM2_SR_CC4IF_SHIFT                     (4)
#define TIM2_SR_CC4IF_MASK                      (0x01U << TIM2_SR_CC4IF_SHIFT)
#define TIM2_SR_CC4IF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_SR_CC4IF_SHIFT)) & TIM2_SR_CC4IF_MASK)

#define TIM2_SR_CC3IF_SHIFT                     (3)
#define TIM2_SR_CC3IF_MASK                      (0x01U << TIM2_SR_CC3IF_SHIFT)
#define TIM2_SR_CC3IF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_SR_CC3IF_SHIFT)) & TIM2_SR_CC3IF_MASK)

#define TIM2_SR_CC2IF_SHIFT                     (2)
#define TIM2_SR_CC2IF_MASK                      (0x01U << TIM2_SR_CC2IF_SHIFT)
#define TIM2_SR_CC2IF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_SR_CC2IF_SHIFT)) & TIM2_SR_CC2IF_MASK)

#define TIM2_SR_CC1IF_SHIFT                     (1)
#define TIM2_SR_CC1IF_MASK                      (0x01U << TIM2_SR_CC1IF_SHIFT)
#define TIM2_SR_CC1IF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_SR_CC1IF_SHIFT)) & TIM2_SR_CC1IF_MASK)

#define TIM2_SR_UIF_SHIFT                       (0)
#define TIM2_SR_UIF_MASK                        (0x01U << TIM2_SR_UIF_SHIFT)
#define TIM2_SR_UIF(x)                          (((uint32_t)(((uint32_t)(x)) << TIM2_SR_UIF_SHIFT)) & TIM2_SR_UIF_MASK)

/*!
 * @brief TIM2_EGR Register Bit Definition
 */

#define TIM2_EGR_TG_SHIFT                       (6)
#define TIM2_EGR_TG_MASK                        (0x01U << TIM2_EGR_TG_SHIFT)
#define TIM2_EGR_TG(x)                          (((uint32_t)(((uint32_t)(x)) << TIM2_EGR_TG_SHIFT)) & TIM2_EGR_TG_MASK)

#define TIM2_EGR_COMG_SHIFT                     (5)
#define TIM2_EGR_COMG_MASK                      (0x01U << TIM2_EGR_COMG_SHIFT)
#define TIM2_EGR_COMG(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_EGR_COMG_SHIFT)) & TIM2_EGR_COMG_MASK)

#define TIM2_EGR_CC4G_SHIFT                     (4)
#define TIM2_EGR_CC4G_MASK                      (0x01U << TIM2_EGR_CC4G_SHIFT)
#define TIM2_EGR_CC4G(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_EGR_CC4G_SHIFT)) & TIM2_EGR_CC4G_MASK)

#define TIM2_EGR_CC3G_SHIFT                     (3)
#define TIM2_EGR_CC3G_MASK                      (0x01U << TIM2_EGR_CC3G_SHIFT)
#define TIM2_EGR_CC3G(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_EGR_CC3G_SHIFT)) & TIM2_EGR_CC3G_MASK)

#define TIM2_EGR_CC2G_SHIFT                     (2)
#define TIM2_EGR_CC2G_MASK                      (0x01U << TIM2_EGR_CC2G_SHIFT)
#define TIM2_EGR_CC2G(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_EGR_CC2G_SHIFT)) & TIM2_EGR_CC2G_MASK)

#define TIM2_EGR_CC1G_SHIFT                     (1)
#define TIM2_EGR_CC1G_MASK                      (0x01U << TIM2_EGR_CC1G_SHIFT)
#define TIM2_EGR_CC1G(x)                        (((uint32_t)(((uint32_t)(x)) << TIM2_EGR_CC1G_SHIFT)) & TIM2_EGR_CC1G_MASK)

#define TIM2_EGR_UG_SHIFT                       (0)
#define TIM2_EGR_UG_MASK                        (0x01U << TIM2_EGR_UG_SHIFT)
#define TIM2_EGR_UG(x)                          (((uint32_t)(((uint32_t)(x)) << TIM2_EGR_UG_SHIFT)) & TIM2_EGR_UG_MASK)

/*!
 * @brief TIM2_CCMR1 Register Bit Definition
 */

#define TIM2_CCMR1_OC2CE_SHIFT                  (15)
#define TIM2_CCMR1_OC2CE_MASK                   (0x01U << TIM2_CCMR1_OC2CE_SHIFT)
#define TIM2_CCMR1_OC2CE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR1_OC2CE_SHIFT)) & TIM2_CCMR1_OC2CE_MASK)

#define TIM2_CCMR1_OC2M_SHIFT                   (12)
#define TIM2_CCMR1_OC2M_MASK                    (0x7U << TIM2_CCMR1_OC2M_SHIFT)
#define TIM2_CCMR1_OC2M(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR1_OC2M_SHIFT)) & TIM2_CCMR1_OC2M_MASK)

#define TIM2_CCMR1_OC2PE_SHIFT                  (11)
#define TIM2_CCMR1_OC2PE_MASK                   (0x01U << TIM2_CCMR1_OC2PE_SHIFT)
#define TIM2_CCMR1_OC2PE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR1_OC2PE_SHIFT)) & TIM2_CCMR1_OC2PE_MASK)

#define TIM2_CCMR1_OC2FE_SHIFT                  (10)
#define TIM2_CCMR1_OC2FE_MASK                   (0x01U << TIM2_CCMR1_OC2FE_SHIFT)
#define TIM2_CCMR1_OC2FE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR1_OC2FE_SHIFT)) & TIM2_CCMR1_OC2FE_MASK)

#define TIM2_CCMR1_CC2S_SHIFT                   (8)
#define TIM2_CCMR1_CC2S_MASK                    (0x3U << TIM2_CCMR1_CC2S_SHIFT)
#define TIM2_CCMR1_CC2S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR1_CC2S_SHIFT)) & TIM2_CCMR1_CC2S_MASK)

#define TIM2_CCMR1_OC1CE_SHIFT                  (7)
#define TIM2_CCMR1_OC1CE_MASK                   (0x01U << TIM2_CCMR1_OC1CE_SHIFT)
#define TIM2_CCMR1_OC1CE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR1_OC1CE_SHIFT)) & TIM2_CCMR1_OC1CE_MASK)

#define TIM2_CCMR1_OC1M_SHIFT                   (4)
#define TIM2_CCMR1_OC1M_MASK                    (0x7U << TIM2_CCMR1_OC1M_SHIFT)
#define TIM2_CCMR1_OC1M(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR1_OC1M_SHIFT)) & TIM2_CCMR1_OC1M_MASK)

#define TIM2_CCMR1_OC1PE_SHIFT                  (3)
#define TIM2_CCMR1_OC1PE_MASK                   (0x01U << TIM2_CCMR1_OC1PE_SHIFT)
#define TIM2_CCMR1_OC1PE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR1_OC1PE_SHIFT)) & TIM2_CCMR1_OC1PE_MASK)

#define TIM2_CCMR1_OC1FE_SHIFT                  (2)
#define TIM2_CCMR1_OC1FE_MASK                   (0x01U << TIM2_CCMR1_OC1FE_SHIFT)
#define TIM2_CCMR1_OC1FE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR1_OC1FE_SHIFT)) & TIM2_CCMR1_OC1FE_MASK)

#define TIM2_CCMR1_CC1S_SHIFT                   (0)
#define TIM2_CCMR1_CC1S_MASK                    (0x3U << TIM2_CCMR1_CC1S_SHIFT)
#define TIM2_CCMR1_CC1S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR1_CC1S_SHIFT)) & TIM2_CCMR1_CC1S_MASK)

/*!
 * @brief TIM2_CCMR1 Register Bit Definition
 */

#define TIM2_CCMR1_IC2F_SHIFT                   (12)
#define TIM2_CCMR1_IC2F_MASK                    (0xFU << TIM2_CCMR1_IC2F_SHIFT)
#define TIM2_CCMR1_IC2F(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR1_IC2F_SHIFT)) & TIM2_CCMR1_IC2F_MASK)

#define TIM2_CCMR1_IC2PSC_SHIFT                 (10)
#define TIM2_CCMR1_IC2PSC_MASK                  (0x3U << TIM2_CCMR1_IC2PSC_SHIFT)
#define TIM2_CCMR1_IC2PSC(x)                    (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR1_IC2PSC_SHIFT)) & TIM2_CCMR1_IC2PSC_MASK)

#define TIM2_CCMR1_CC2S_SHIFT                   (8)
#define TIM2_CCMR1_CC2S_MASK                    (0x3U << TIM2_CCMR1_CC2S_SHIFT)
#define TIM2_CCMR1_CC2S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR1_CC2S_SHIFT)) & TIM2_CCMR1_CC2S_MASK)

#define TIM2_CCMR1_IC1F_SHIFT                   (4)
#define TIM2_CCMR1_IC1F_MASK                    (0xFU << TIM2_CCMR1_IC1F_SHIFT)
#define TIM2_CCMR1_IC1F(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR1_IC1F_SHIFT)) & TIM2_CCMR1_IC1F_MASK)

#define TIM2_CCMR1_IC1PSC_SHIFT                 (2)
#define TIM2_CCMR1_IC1PSC_MASK                  (0x3U << TIM2_CCMR1_IC1PSC_SHIFT)
#define TIM2_CCMR1_IC1PSC(x)                    (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR1_IC1PSC_SHIFT)) & TIM2_CCMR1_IC1PSC_MASK)

#define TIM2_CCMR1_CC1S_SHIFT                   (0)
#define TIM2_CCMR1_CC1S_MASK                    (0x3U << TIM2_CCMR1_CC1S_SHIFT)
#define TIM2_CCMR1_CC1S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR1_CC1S_SHIFT)) & TIM2_CCMR1_CC1S_MASK)

/*!
 * @brief TIM2_CCMR2 Register Bit Definition
 */

#define TIM2_CCMR2_OC4CE_SHIFT                  (15)
#define TIM2_CCMR2_OC4CE_MASK                   (0x01U << TIM2_CCMR2_OC4CE_SHIFT)
#define TIM2_CCMR2_OC4CE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR2_OC4CE_SHIFT)) & TIM2_CCMR2_OC4CE_MASK)

#define TIM2_CCMR2_OC4M_SHIFT                   (12)
#define TIM2_CCMR2_OC4M_MASK                    (0x7U << TIM2_CCMR2_OC4M_SHIFT)
#define TIM2_CCMR2_OC4M(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR2_OC4M_SHIFT)) & TIM2_CCMR2_OC4M_MASK)

#define TIM2_CCMR2_OC4PE_SHIFT                  (11)
#define TIM2_CCMR2_OC4PE_MASK                   (0x01U << TIM2_CCMR2_OC4PE_SHIFT)
#define TIM2_CCMR2_OC4PE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR2_OC4PE_SHIFT)) & TIM2_CCMR2_OC4PE_MASK)

#define TIM2_CCMR2_OC4FE_SHIFT                  (10)
#define TIM2_CCMR2_OC4FE_MASK                   (0x01U << TIM2_CCMR2_OC4FE_SHIFT)
#define TIM2_CCMR2_OC4FE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR2_OC4FE_SHIFT)) & TIM2_CCMR2_OC4FE_MASK)

#define TIM2_CCMR2_CC4S_SHIFT                   (8)
#define TIM2_CCMR2_CC4S_MASK                    (0x3U << TIM2_CCMR2_CC4S_SHIFT)
#define TIM2_CCMR2_CC4S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR2_CC4S_SHIFT)) & TIM2_CCMR2_CC4S_MASK)

#define TIM2_CCMR2_OC3CE_SHIFT                  (7)
#define TIM2_CCMR2_OC3CE_MASK                   (0x01U << TIM2_CCMR2_OC3CE_SHIFT)
#define TIM2_CCMR2_OC3CE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR2_OC3CE_SHIFT)) & TIM2_CCMR2_OC3CE_MASK)

#define TIM2_CCMR2_OC3M_SHIFT                   (4)
#define TIM2_CCMR2_OC3M_MASK                    (0x7U << TIM2_CCMR2_OC3M_SHIFT)
#define TIM2_CCMR2_OC3M(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR2_OC3M_SHIFT)) & TIM2_CCMR2_OC3M_MASK)

#define TIM2_CCMR2_OC3PE_SHIFT                  (3)
#define TIM2_CCMR2_OC3PE_MASK                   (0x01U << TIM2_CCMR2_OC3PE_SHIFT)
#define TIM2_CCMR2_OC3PE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR2_OC3PE_SHIFT)) & TIM2_CCMR2_OC3PE_MASK)

#define TIM2_CCMR2_OC3FE_SHIFT                  (2)
#define TIM2_CCMR2_OC3FE_MASK                   (0x01U << TIM2_CCMR2_OC3FE_SHIFT)
#define TIM2_CCMR2_OC3FE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR2_OC3FE_SHIFT)) & TIM2_CCMR2_OC3FE_MASK)

#define TIM2_CCMR2_CC3S_SHIFT                   (0)
#define TIM2_CCMR2_CC3S_MASK                    (0x3U << TIM2_CCMR2_CC3S_SHIFT)
#define TIM2_CCMR2_CC3S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR2_CC3S_SHIFT)) & TIM2_CCMR2_CC3S_MASK)

/*!
 * @brief TIM2_CCMR2 Register Bit Definition
 */

#define TIM2_CCMR2_IC4F_SHIFT                   (12)
#define TIM2_CCMR2_IC4F_MASK                    (0xFU << TIM2_CCMR2_IC4F_SHIFT)
#define TIM2_CCMR2_IC4F(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR2_IC4F_SHIFT)) & TIM2_CCMR2_IC4F_MASK)

#define TIM2_CCMR2_IC4PSC_SHIFT                 (10)
#define TIM2_CCMR2_IC4PSC_MASK                  (0x3U << TIM2_CCMR2_IC4PSC_SHIFT)
#define TIM2_CCMR2_IC4PSC(x)                    (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR2_IC4PSC_SHIFT)) & TIM2_CCMR2_IC4PSC_MASK)

#define TIM2_CCMR2_CC4S_SHIFT                   (8)
#define TIM2_CCMR2_CC4S_MASK                    (0x3U << TIM2_CCMR2_CC4S_SHIFT)
#define TIM2_CCMR2_CC4S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR2_CC4S_SHIFT)) & TIM2_CCMR2_CC4S_MASK)

#define TIM2_CCMR2_IC3F_SHIFT                   (4)
#define TIM2_CCMR2_IC3F_MASK                    (0xFU << TIM2_CCMR2_IC3F_SHIFT)
#define TIM2_CCMR2_IC3F(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR2_IC3F_SHIFT)) & TIM2_CCMR2_IC3F_MASK)

#define TIM2_CCMR2_IC3PSC_SHIFT                 (2)
#define TIM2_CCMR2_IC3PSC_MASK                  (0x3U << TIM2_CCMR2_IC3PSC_SHIFT)
#define TIM2_CCMR2_IC3PSC(x)                    (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR2_IC3PSC_SHIFT)) & TIM2_CCMR2_IC3PSC_MASK)

#define TIM2_CCMR2_CC3S_SHIFT                   (0)
#define TIM2_CCMR2_CC3S_MASK                    (0x3U << TIM2_CCMR2_CC3S_SHIFT)
#define TIM2_CCMR2_CC3S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCMR2_CC3S_SHIFT)) & TIM2_CCMR2_CC3S_MASK)

/*!
 * @brief TIM2_CCER Register Bit Definition
 */

#define TIM2_CCER_CC4NP_SHIFT                   (15)
#define TIM2_CCER_CC4NP_MASK                    (0x01U << TIM2_CCER_CC4NP_SHIFT)
#define TIM2_CCER_CC4NP(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCER_CC4NP_SHIFT)) & TIM2_CCER_CC4NP_MASK)

#define TIM2_CCER_CC4P_SHIFT                    (13)
#define TIM2_CCER_CC4P_MASK                     (0x01U << TIM2_CCER_CC4P_SHIFT)
#define TIM2_CCER_CC4P(x)                       (((uint32_t)(((uint32_t)(x)) << TIM2_CCER_CC4P_SHIFT)) & TIM2_CCER_CC4P_MASK)

#define TIM2_CCER_CC4E_SHIFT                    (12)
#define TIM2_CCER_CC4E_MASK                     (0x01U << TIM2_CCER_CC4E_SHIFT)
#define TIM2_CCER_CC4E(x)                       (((uint32_t)(((uint32_t)(x)) << TIM2_CCER_CC4E_SHIFT)) & TIM2_CCER_CC4E_MASK)

#define TIM2_CCER_CC3NP_SHIFT                   (11)
#define TIM2_CCER_CC3NP_MASK                    (0x01U << TIM2_CCER_CC3NP_SHIFT)
#define TIM2_CCER_CC3NP(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCER_CC3NP_SHIFT)) & TIM2_CCER_CC3NP_MASK)

#define TIM2_CCER_CC3P_SHIFT                    (9)
#define TIM2_CCER_CC3P_MASK                     (0x01U << TIM2_CCER_CC3P_SHIFT)
#define TIM2_CCER_CC3P(x)                       (((uint32_t)(((uint32_t)(x)) << TIM2_CCER_CC3P_SHIFT)) & TIM2_CCER_CC3P_MASK)

#define TIM2_CCER_CC3E_SHIFT                    (8)
#define TIM2_CCER_CC3E_MASK                     (0x01U << TIM2_CCER_CC3E_SHIFT)
#define TIM2_CCER_CC3E(x)                       (((uint32_t)(((uint32_t)(x)) << TIM2_CCER_CC3E_SHIFT)) & TIM2_CCER_CC3E_MASK)

#define TIM2_CCER_CC2NP_SHIFT                   (7)
#define TIM2_CCER_CC2NP_MASK                    (0x01U << TIM2_CCER_CC2NP_SHIFT)
#define TIM2_CCER_CC2NP(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCER_CC2NP_SHIFT)) & TIM2_CCER_CC2NP_MASK)

#define TIM2_CCER_CC2P_SHIFT                    (5)
#define TIM2_CCER_CC2P_MASK                     (0x01U << TIM2_CCER_CC2P_SHIFT)
#define TIM2_CCER_CC2P(x)                       (((uint32_t)(((uint32_t)(x)) << TIM2_CCER_CC2P_SHIFT)) & TIM2_CCER_CC2P_MASK)

#define TIM2_CCER_CC2E_SHIFT                    (4)
#define TIM2_CCER_CC2E_MASK                     (0x01U << TIM2_CCER_CC2E_SHIFT)
#define TIM2_CCER_CC2E(x)                       (((uint32_t)(((uint32_t)(x)) << TIM2_CCER_CC2E_SHIFT)) & TIM2_CCER_CC2E_MASK)

#define TIM2_CCER_CC1NP_SHIFT                   (3)
#define TIM2_CCER_CC1NP_MASK                    (0x01U << TIM2_CCER_CC1NP_SHIFT)
#define TIM2_CCER_CC1NP(x)                      (((uint32_t)(((uint32_t)(x)) << TIM2_CCER_CC1NP_SHIFT)) & TIM2_CCER_CC1NP_MASK)

#define TIM2_CCER_CC1P_SHIFT                    (1)
#define TIM2_CCER_CC1P_MASK                     (0x01U << TIM2_CCER_CC1P_SHIFT)
#define TIM2_CCER_CC1P(x)                       (((uint32_t)(((uint32_t)(x)) << TIM2_CCER_CC1P_SHIFT)) & TIM2_CCER_CC1P_MASK)

#define TIM2_CCER_CC1E_SHIFT                    (0)
#define TIM2_CCER_CC1E_MASK                     (0x01U << TIM2_CCER_CC1E_SHIFT)
#define TIM2_CCER_CC1E(x)                       (((uint32_t)(((uint32_t)(x)) << TIM2_CCER_CC1E_SHIFT)) & TIM2_CCER_CC1E_MASK)

/*!
 * @brief TIM2_CNT Register Bit Definition
 */

#define TIM2_CNT_CNT_SHIFT                      (0)
#define TIM2_CNT_CNT_MASK                       (0xFFFFU << TIM2_CNT_CNT_SHIFT)
#define TIM2_CNT_CNT(x)                         (((uint32_t)(((uint32_t)(x)) << TIM2_CNT_CNT_SHIFT)) & TIM2_CNT_CNT_MASK)

/*!
 * @brief TIM2_PSC Register Bit Definition
 */

#define TIM2_PSC_PSC_SHIFT                      (0)
#define TIM2_PSC_PSC_MASK                       (0xFFFFU << TIM2_PSC_PSC_SHIFT)
#define TIM2_PSC_PSC(x)                         (((uint32_t)(((uint32_t)(x)) << TIM2_PSC_PSC_SHIFT)) & TIM2_PSC_PSC_MASK)

/*!
 * @brief TIM2_ARR Register Bit Definition
 */

#define TIM2_ARR_ARR_SHIFT                      (0)
#define TIM2_ARR_ARR_MASK                       (0xFFFFU << TIM2_ARR_ARR_SHIFT)
#define TIM2_ARR_ARR(x)                         (((uint32_t)(((uint32_t)(x)) << TIM2_ARR_ARR_SHIFT)) & TIM2_ARR_ARR_MASK)

/*!
 * @brief TIM2_CCR Register Bit Definition
 */

#define TIM2_CCR_CCR_SHIFT                      (0)
#define TIM2_CCR_CCR_MASK                       (0xFFFFU << TIM2_CCR_CCR_SHIFT)
#define TIM2_CCR_CCR(x)                         (((uint32_t)(((uint32_t)(x)) << TIM2_CCR_CCR_SHIFT)) & TIM2_CCR_CCR_MASK)

/*!
 * @brief TIM2_DCR Register Bit Definition
 */

#define TIM2_DCR_DBL_SHIFT                      (8)
#define TIM2_DCR_DBL_MASK                       (0x1FU << TIM2_DCR_DBL_SHIFT)
#define TIM2_DCR_DBL(x)                         (((uint32_t)(((uint32_t)(x)) << TIM2_DCR_DBL_SHIFT)) & TIM2_DCR_DBL_MASK)

#define TIM2_DCR_DBA_SHIFT                      (0)
#define TIM2_DCR_DBA_MASK                       (0x1FU << TIM2_DCR_DBA_SHIFT)
#define TIM2_DCR_DBA(x)                         (((uint32_t)(((uint32_t)(x)) << TIM2_DCR_DBA_SHIFT)) & TIM2_DCR_DBA_MASK)

/*!
 * @brief TIM2_DMAR Register Bit Definition
 */

#define TIM2_DMAR_DMAB_SHIFT                    (0)
#define TIM2_DMAR_DMAB_MASK                     (0xFFFFU << TIM2_DMAR_DMAB_SHIFT)
#define TIM2_DMAR_DMAB(x)                       (((uint32_t)(((uint32_t)(x)) << TIM2_DMAR_DMAB_SHIFT)) & TIM2_DMAR_DMAB_MASK)

/*!
 * @brief TIM2_OR Register Bit Definition
 */

#define TIM2_OR_TI4RMP_SHIFT                    (6)
#define TIM2_OR_TI4RMP_MASK                     (0x3U << TIM2_OR_TI4RMP_SHIFT)
#define TIM2_OR_TI4RMP(x)                       (((uint32_t)(((uint32_t)(x)) << TIM2_OR_TI4RMP_SHIFT)) & TIM2_OR_TI4RMP_MASK)

#define TIM2_OR_ETRRMP_SHIFT                    (0)
#define TIM2_OR_ETRRMP_MASK                     (0x7U << TIM2_OR_ETRRMP_SHIFT)
#define TIM2_OR_ETRRMP(x)                       (((uint32_t)(((uint32_t)(x)) << TIM2_OR_ETRRMP_SHIFT)) & TIM2_OR_ETRRMP_MASK)

/*!
 * @}
 */ /* end of group TIM2_Register_Masks */
/******************************************************************************
 *TIM2 Instance
*******************************************************************************/

#define TIM2                 ((TIM2_Type*)TIM2_BASE)
#define TIM5                 ((TIM2_Type*)TIM5_BASE)

/*!
 * @}
 */ /* end of group TIM2_Peripheral_Access_Layer */

/*!
 * @addtogroup TIM3_Peripheral_Access_Layer TIM3 Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * TIM3 Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t CR1;                                                            ///< Control Register 1                           offset: 0x00
    __IO uint32_t CR2;                                                            ///< Control Register 2                           offset: 0x04
    __IO uint32_t SMCR;                                                           ///< Slave Mode Control Register                  offset: 0x08
    __IO uint32_t DIER;                                                           ///< DMA/Interrupt Enable Register                offset: 0x0C
    __IO uint32_t SR;                                                             ///< status register                              offset: 0x10
    __IO uint32_t EGR;                                                            ///< event generation register                    offset: 0x14
    __IO uint32_t CCMR1;                                                          ///< Capture/Compare Mode Register 1              offset: 0x18
    __IO uint32_t CCMR2;                                                          ///< Capture/Compare Mode Register 2              offset: 0x1C
    __IO uint32_t CCER;                                                           ///< Capture/Compare Enable Register              offset: 0x20
    __IO uint32_t CNT;                                                            ///< counter                                      offset: 0x24
    __IO uint32_t PSC;                                                            ///< Prescaler                                    offset: 0x28
    __IO uint32_t ARR;                                                            ///< autoload register                            offset: 0x2C
    __IO uint32_t Reserved51[1];                                                   ///< Reserved
    __IO uint32_t CCR[4];                                                           ///< Capture/Compare Register 1                   offset: 0x34
    __IO uint32_t Reserved52[1];                                                   ///< Reserved
    __IO uint32_t DCR;                                                            ///< DMA Control Register                         offset: 0x48
    __IO uint32_t DMAR;                                                           ///< DMA address for continuous mode              offset: 0x4C
} TIM3_Type;

/*******************************************************************************
 * TIM3 Type
 ******************************************************************************/

/*!
 * @addtogroup TIM3_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief TIM3_CR1 Register Bit Definition
 */

#define TIM3_CR1_CKD_SHIFT                      (8)
#define TIM3_CR1_CKD_MASK                       (0x3U << TIM3_CR1_CKD_SHIFT)
#define TIM3_CR1_CKD(x)                         (((uint32_t)(((uint32_t)(x)) << TIM3_CR1_CKD_SHIFT)) & TIM3_CR1_CKD_MASK)

#define TIM3_CR1_ARPE_SHIFT                     (7)
#define TIM3_CR1_ARPE_MASK                      (0x01U << TIM3_CR1_ARPE_SHIFT)
#define TIM3_CR1_ARPE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_CR1_ARPE_SHIFT)) & TIM3_CR1_ARPE_MASK)

#define TIM3_CR1_CMS_SHIFT                      (5)
#define TIM3_CR1_CMS_MASK                       (0x3U << TIM3_CR1_CMS_SHIFT)
#define TIM3_CR1_CMS(x)                         (((uint32_t)(((uint32_t)(x)) << TIM3_CR1_CMS_SHIFT)) & TIM3_CR1_CMS_MASK)

#define TIM3_CR1_DIR_SHIFT                      (4)
#define TIM3_CR1_DIR_MASK                       (0x01U << TIM3_CR1_DIR_SHIFT)
#define TIM3_CR1_DIR(x)                         (((uint32_t)(((uint32_t)(x)) << TIM3_CR1_DIR_SHIFT)) & TIM3_CR1_DIR_MASK)

#define TIM3_CR1_OPM_SHIFT                      (3)
#define TIM3_CR1_OPM_MASK                       (0x01U << TIM3_CR1_OPM_SHIFT)
#define TIM3_CR1_OPM(x)                         (((uint32_t)(((uint32_t)(x)) << TIM3_CR1_OPM_SHIFT)) & TIM3_CR1_OPM_MASK)

#define TIM3_CR1_URS_SHIFT                      (2)
#define TIM3_CR1_URS_MASK                       (0x01U << TIM3_CR1_URS_SHIFT)
#define TIM3_CR1_URS(x)                         (((uint32_t)(((uint32_t)(x)) << TIM3_CR1_URS_SHIFT)) & TIM3_CR1_URS_MASK)

#define TIM3_CR1_UDIS_SHIFT                     (1)
#define TIM3_CR1_UDIS_MASK                      (0x01U << TIM3_CR1_UDIS_SHIFT)
#define TIM3_CR1_UDIS(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_CR1_UDIS_SHIFT)) & TIM3_CR1_UDIS_MASK)

#define TIM3_CR1_CEN_SHIFT                      (0)
#define TIM3_CR1_CEN_MASK                       (0x01U << TIM3_CR1_CEN_SHIFT)
#define TIM3_CR1_CEN(x)                         (((uint32_t)(((uint32_t)(x)) << TIM3_CR1_CEN_SHIFT)) & TIM3_CR1_CEN_MASK)

/*!
 * @brief TIM3_CR2 Register Bit Definition
 */

#define TIM3_CR2_TI1S_SHIFT                     (7)
#define TIM3_CR2_TI1S_MASK                      (0x01U << TIM3_CR2_TI1S_SHIFT)
#define TIM3_CR2_TI1S(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_CR2_TI1S_SHIFT)) & TIM3_CR2_TI1S_MASK)

#define TIM3_CR2_MMS_SHIFT                      (4)
#define TIM3_CR2_MMS_MASK                       (0x7U << TIM3_CR2_MMS_SHIFT)
#define TIM3_CR2_MMS(x)                         (((uint32_t)(((uint32_t)(x)) << TIM3_CR2_MMS_SHIFT)) & TIM3_CR2_MMS_MASK)

#define TIM3_CR2_CCDS_SHIFT                     (3)
#define TIM3_CR2_CCDS_MASK                      (0x01U << TIM3_CR2_CCDS_SHIFT)
#define TIM3_CR2_CCDS(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_CR2_CCDS_SHIFT)) & TIM3_CR2_CCDS_MASK)

/*!
 * @brief TIM3_SMCR Register Bit Definition
 */

#define TIM3_SMCR_ETP_SHIFT                     (15)
#define TIM3_SMCR_ETP_MASK                      (0x01U << TIM3_SMCR_ETP_SHIFT)
#define TIM3_SMCR_ETP(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_SMCR_ETP_SHIFT)) & TIM3_SMCR_ETP_MASK)

#define TIM3_SMCR_ECE_SHIFT                     (14)
#define TIM3_SMCR_ECE_MASK                      (0x01U << TIM3_SMCR_ECE_SHIFT)
#define TIM3_SMCR_ECE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_SMCR_ECE_SHIFT)) & TIM3_SMCR_ECE_MASK)

#define TIM3_SMCR_ETPS_SHIFT                    (12)
#define TIM3_SMCR_ETPS_MASK                     (0x3U << TIM3_SMCR_ETPS_SHIFT)
#define TIM3_SMCR_ETPS(x)                       (((uint32_t)(((uint32_t)(x)) << TIM3_SMCR_ETPS_SHIFT)) & TIM3_SMCR_ETPS_MASK)

#define TIM3_SMCR_ETF_SHIFT                     (8)
#define TIM3_SMCR_ETF_MASK                      (0xFU << TIM3_SMCR_ETF_SHIFT)
#define TIM3_SMCR_ETF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_SMCR_ETF_SHIFT)) & TIM3_SMCR_ETF_MASK)

#define TIM3_SMCR_MSM_SHIFT                     (7)
#define TIM3_SMCR_MSM_MASK                      (0x01U << TIM3_SMCR_MSM_SHIFT)
#define TIM3_SMCR_MSM(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_SMCR_MSM_SHIFT)) & TIM3_SMCR_MSM_MASK)

#define TIM3_SMCR_TS_SHIFT                      (4)
#define TIM3_SMCR_TS_MASK                       (0x7U << TIM3_SMCR_TS_SHIFT)
#define TIM3_SMCR_TS(x)                         (((uint32_t)(((uint32_t)(x)) << TIM3_SMCR_TS_SHIFT)) & TIM3_SMCR_TS_MASK)

#define TIM3_SMCR_OCCS_SHIFT                    (3)
#define TIM3_SMCR_OCCS_MASK                     (0x01U << TIM3_SMCR_OCCS_SHIFT)
#define TIM3_SMCR_OCCS(x)                       (((uint32_t)(((uint32_t)(x)) << TIM3_SMCR_OCCS_SHIFT)) & TIM3_SMCR_OCCS_MASK)

#define TIM3_SMCR_SMS_SHIFT                     (0)
#define TIM3_SMCR_SMS_MASK                      (0x7U << TIM3_SMCR_SMS_SHIFT)
#define TIM3_SMCR_SMS(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_SMCR_SMS_SHIFT)) & TIM3_SMCR_SMS_MASK)

/*!
 * @brief TIM3_DIER Register Bit Definition
 */

#define TIM3_DIER_TDE_SHIFT                     (14)
#define TIM3_DIER_TDE_MASK                      (0x01U << TIM3_DIER_TDE_SHIFT)
#define TIM3_DIER_TDE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_DIER_TDE_SHIFT)) & TIM3_DIER_TDE_MASK)

#define TIM3_DIER_CC4DE_SHIFT                   (12)
#define TIM3_DIER_CC4DE_MASK                    (0x01U << TIM3_DIER_CC4DE_SHIFT)
#define TIM3_DIER_CC4DE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_DIER_CC4DE_SHIFT)) & TIM3_DIER_CC4DE_MASK)

#define TIM3_DIER_CC3DE_SHIFT                   (11)
#define TIM3_DIER_CC3DE_MASK                    (0x01U << TIM3_DIER_CC3DE_SHIFT)
#define TIM3_DIER_CC3DE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_DIER_CC3DE_SHIFT)) & TIM3_DIER_CC3DE_MASK)

#define TIM3_DIER_CC2DE_SHIFT                   (10)
#define TIM3_DIER_CC2DE_MASK                    (0x01U << TIM3_DIER_CC2DE_SHIFT)
#define TIM3_DIER_CC2DE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_DIER_CC2DE_SHIFT)) & TIM3_DIER_CC2DE_MASK)

#define TIM3_DIER_CC1DE_SHIFT                   (9)
#define TIM3_DIER_CC1DE_MASK                    (0x01U << TIM3_DIER_CC1DE_SHIFT)
#define TIM3_DIER_CC1DE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_DIER_CC1DE_SHIFT)) & TIM3_DIER_CC1DE_MASK)

#define TIM3_DIER_UDE_SHIFT                     (8)
#define TIM3_DIER_UDE_MASK                      (0x01U << TIM3_DIER_UDE_SHIFT)
#define TIM3_DIER_UDE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_DIER_UDE_SHIFT)) & TIM3_DIER_UDE_MASK)

#define TIM3_DIER_TIE_SHIFT                     (6)
#define TIM3_DIER_TIE_MASK                      (0x01U << TIM3_DIER_TIE_SHIFT)
#define TIM3_DIER_TIE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_DIER_TIE_SHIFT)) & TIM3_DIER_TIE_MASK)

#define TIM3_DIER_CC4IE_SHIFT                   (4)
#define TIM3_DIER_CC4IE_MASK                    (0x01U << TIM3_DIER_CC4IE_SHIFT)
#define TIM3_DIER_CC4IE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_DIER_CC4IE_SHIFT)) & TIM3_DIER_CC4IE_MASK)

#define TIM3_DIER_CC3IE_SHIFT                   (3)
#define TIM3_DIER_CC3IE_MASK                    (0x01U << TIM3_DIER_CC3IE_SHIFT)
#define TIM3_DIER_CC3IE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_DIER_CC3IE_SHIFT)) & TIM3_DIER_CC3IE_MASK)

#define TIM3_DIER_CC2IE_SHIFT                   (2)
#define TIM3_DIER_CC2IE_MASK                    (0x01U << TIM3_DIER_CC2IE_SHIFT)
#define TIM3_DIER_CC2IE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_DIER_CC2IE_SHIFT)) & TIM3_DIER_CC2IE_MASK)

#define TIM3_DIER_CC1IE_SHIFT                   (1)
#define TIM3_DIER_CC1IE_MASK                    (0x01U << TIM3_DIER_CC1IE_SHIFT)
#define TIM3_DIER_CC1IE(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_DIER_CC1IE_SHIFT)) & TIM3_DIER_CC1IE_MASK)

#define TIM3_DIER_UIE_SHIFT                     (0)
#define TIM3_DIER_UIE_MASK                      (0x01U << TIM3_DIER_UIE_SHIFT)
#define TIM3_DIER_UIE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_DIER_UIE_SHIFT)) & TIM3_DIER_UIE_MASK)

/*!
 * @brief TIM3_SR Register Bit Definition
 */

#define TIM3_SR_CC4OF_SHIFT                     (12)
#define TIM3_SR_CC4OF_MASK                      (0x01U << TIM3_SR_CC4OF_SHIFT)
#define TIM3_SR_CC4OF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_SR_CC4OF_SHIFT)) & TIM3_SR_CC4OF_MASK)

#define TIM3_SR_CC3OF_SHIFT                     (11)
#define TIM3_SR_CC3OF_MASK                      (0x01U << TIM3_SR_CC3OF_SHIFT)
#define TIM3_SR_CC3OF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_SR_CC3OF_SHIFT)) & TIM3_SR_CC3OF_MASK)

#define TIM3_SR_CC2OF_SHIFT                     (10)
#define TIM3_SR_CC2OF_MASK                      (0x01U << TIM3_SR_CC2OF_SHIFT)
#define TIM3_SR_CC2OF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_SR_CC2OF_SHIFT)) & TIM3_SR_CC2OF_MASK)

#define TIM3_SR_CC1OF_SHIFT                     (9)
#define TIM3_SR_CC1OF_MASK                      (0x01U << TIM3_SR_CC1OF_SHIFT)
#define TIM3_SR_CC1OF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_SR_CC1OF_SHIFT)) & TIM3_SR_CC1OF_MASK)

#define TIM3_SR_TIF_SHIFT                       (6)
#define TIM3_SR_TIF_MASK                        (0x01U << TIM3_SR_TIF_SHIFT)
#define TIM3_SR_TIF(x)                          (((uint32_t)(((uint32_t)(x)) << TIM3_SR_TIF_SHIFT)) & TIM3_SR_TIF_MASK)

#define TIM3_SR_CC4IF_SHIFT                     (4)
#define TIM3_SR_CC4IF_MASK                      (0x01U << TIM3_SR_CC4IF_SHIFT)
#define TIM3_SR_CC4IF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_SR_CC4IF_SHIFT)) & TIM3_SR_CC4IF_MASK)

#define TIM3_SR_CC3IF_SHIFT                     (3)
#define TIM3_SR_CC3IF_MASK                      (0x01U << TIM3_SR_CC3IF_SHIFT)
#define TIM3_SR_CC3IF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_SR_CC3IF_SHIFT)) & TIM3_SR_CC3IF_MASK)

#define TIM3_SR_CC2IF_SHIFT                     (2)
#define TIM3_SR_CC2IF_MASK                      (0x01U << TIM3_SR_CC2IF_SHIFT)
#define TIM3_SR_CC2IF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_SR_CC2IF_SHIFT)) & TIM3_SR_CC2IF_MASK)

#define TIM3_SR_CC1IF_SHIFT                     (1)
#define TIM3_SR_CC1IF_MASK                      (0x01U << TIM3_SR_CC1IF_SHIFT)
#define TIM3_SR_CC1IF(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_SR_CC1IF_SHIFT)) & TIM3_SR_CC1IF_MASK)

#define TIM3_SR_UIF_SHIFT                       (0)
#define TIM3_SR_UIF_MASK                        (0x01U << TIM3_SR_UIF_SHIFT)
#define TIM3_SR_UIF(x)                          (((uint32_t)(((uint32_t)(x)) << TIM3_SR_UIF_SHIFT)) & TIM3_SR_UIF_MASK)

/*!
 * @brief TIM3_EGR Register Bit Definition
 */

#define TIM3_EGR_TG_SHIFT                       (6)
#define TIM3_EGR_TG_MASK                        (0x01U << TIM3_EGR_TG_SHIFT)
#define TIM3_EGR_TG(x)                          (((uint32_t)(((uint32_t)(x)) << TIM3_EGR_TG_SHIFT)) & TIM3_EGR_TG_MASK)

#define TIM3_EGR_COMG_SHIFT                     (5)
#define TIM3_EGR_COMG_MASK                      (0x01U << TIM3_EGR_COMG_SHIFT)
#define TIM3_EGR_COMG(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_EGR_COMG_SHIFT)) & TIM3_EGR_COMG_MASK)

#define TIM3_EGR_CC4G_SHIFT                     (4)
#define TIM3_EGR_CC4G_MASK                      (0x01U << TIM3_EGR_CC4G_SHIFT)
#define TIM3_EGR_CC4G(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_EGR_CC4G_SHIFT)) & TIM3_EGR_CC4G_MASK)

#define TIM3_EGR_CC3G_SHIFT                     (3)
#define TIM3_EGR_CC3G_MASK                      (0x01U << TIM3_EGR_CC3G_SHIFT)
#define TIM3_EGR_CC3G(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_EGR_CC3G_SHIFT)) & TIM3_EGR_CC3G_MASK)

#define TIM3_EGR_CC2G_SHIFT                     (2)
#define TIM3_EGR_CC2G_MASK                      (0x01U << TIM3_EGR_CC2G_SHIFT)
#define TIM3_EGR_CC2G(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_EGR_CC2G_SHIFT)) & TIM3_EGR_CC2G_MASK)

#define TIM3_EGR_CC1G_SHIFT                     (1)
#define TIM3_EGR_CC1G_MASK                      (0x01U << TIM3_EGR_CC1G_SHIFT)
#define TIM3_EGR_CC1G(x)                        (((uint32_t)(((uint32_t)(x)) << TIM3_EGR_CC1G_SHIFT)) & TIM3_EGR_CC1G_MASK)

#define TIM3_EGR_UG_SHIFT                       (0)
#define TIM3_EGR_UG_MASK                        (0x01U << TIM3_EGR_UG_SHIFT)
#define TIM3_EGR_UG(x)                          (((uint32_t)(((uint32_t)(x)) << TIM3_EGR_UG_SHIFT)) & TIM3_EGR_UG_MASK)

/*!
 * @brief TIM3_CCMR1 Register Bit Definition
 */

#define TIM3_CCMR1_OC2CE_SHIFT                  (15)
#define TIM3_CCMR1_OC2CE_MASK                   (0x01U << TIM3_CCMR1_OC2CE_SHIFT)
#define TIM3_CCMR1_OC2CE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR1_OC2CE_SHIFT)) & TIM3_CCMR1_OC2CE_MASK)

#define TIM3_CCMR1_OC2M_SHIFT                   (12)
#define TIM3_CCMR1_OC2M_MASK                    (0x7U << TIM3_CCMR1_OC2M_SHIFT)
#define TIM3_CCMR1_OC2M(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR1_OC2M_SHIFT)) & TIM3_CCMR1_OC2M_MASK)

#define TIM3_CCMR1_OC2PE_SHIFT                  (11)
#define TIM3_CCMR1_OC2PE_MASK                   (0x01U << TIM3_CCMR1_OC2PE_SHIFT)
#define TIM3_CCMR1_OC2PE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR1_OC2PE_SHIFT)) & TIM3_CCMR1_OC2PE_MASK)

#define TIM3_CCMR1_OC2FE_SHIFT                  (10)
#define TIM3_CCMR1_OC2FE_MASK                   (0x01U << TIM3_CCMR1_OC2FE_SHIFT)
#define TIM3_CCMR1_OC2FE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR1_OC2FE_SHIFT)) & TIM3_CCMR1_OC2FE_MASK)

#define TIM3_CCMR1_CC2S_SHIFT                   (8)
#define TIM3_CCMR1_CC2S_MASK                    (0x3U << TIM3_CCMR1_CC2S_SHIFT)
#define TIM3_CCMR1_CC2S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR1_CC2S_SHIFT)) & TIM3_CCMR1_CC2S_MASK)

#define TIM3_CCMR1_OC1CE_SHIFT                  (7)
#define TIM3_CCMR1_OC1CE_MASK                   (0x01U << TIM3_CCMR1_OC1CE_SHIFT)
#define TIM3_CCMR1_OC1CE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR1_OC1CE_SHIFT)) & TIM3_CCMR1_OC1CE_MASK)

#define TIM3_CCMR1_OC1M_SHIFT                   (4)
#define TIM3_CCMR1_OC1M_MASK                    (0x7U << TIM3_CCMR1_OC1M_SHIFT)
#define TIM3_CCMR1_OC1M(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR1_OC1M_SHIFT)) & TIM3_CCMR1_OC1M_MASK)

#define TIM3_CCMR1_OC1PE_SHIFT                  (3)
#define TIM3_CCMR1_OC1PE_MASK                   (0x01U << TIM3_CCMR1_OC1PE_SHIFT)
#define TIM3_CCMR1_OC1PE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR1_OC1PE_SHIFT)) & TIM3_CCMR1_OC1PE_MASK)

#define TIM3_CCMR1_OC1FE_SHIFT                  (2)
#define TIM3_CCMR1_OC1FE_MASK                   (0x01U << TIM3_CCMR1_OC1FE_SHIFT)
#define TIM3_CCMR1_OC1FE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR1_OC1FE_SHIFT)) & TIM3_CCMR1_OC1FE_MASK)

#define TIM3_CCMR1_CC1S_SHIFT                   (0)
#define TIM3_CCMR1_CC1S_MASK                    (0x3U << TIM3_CCMR1_CC1S_SHIFT)
#define TIM3_CCMR1_CC1S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR1_CC1S_SHIFT)) & TIM3_CCMR1_CC1S_MASK)

/*!
 * @brief TIM3_CCMR1 Register Bit Definition
 */

#define TIM3_CCMR1_IC2F_SHIFT                   (12)
#define TIM3_CCMR1_IC2F_MASK                    (0xFU << TIM3_CCMR1_IC2F_SHIFT)
#define TIM3_CCMR1_IC2F(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR1_IC2F_SHIFT)) & TIM3_CCMR1_IC2F_MASK)

#define TIM3_CCMR1_IC2PSC_SHIFT                 (10)
#define TIM3_CCMR1_IC2PSC_MASK                  (0x3U << TIM3_CCMR1_IC2PSC_SHIFT)
#define TIM3_CCMR1_IC2PSC(x)                    (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR1_IC2PSC_SHIFT)) & TIM3_CCMR1_IC2PSC_MASK)

#define TIM3_CCMR1_CC2S_SHIFT                   (8)
#define TIM3_CCMR1_CC2S_MASK                    (0x3U << TIM3_CCMR1_CC2S_SHIFT)
#define TIM3_CCMR1_CC2S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR1_CC2S_SHIFT)) & TIM3_CCMR1_CC2S_MASK)

#define TIM3_CCMR1_IC1F_SHIFT                   (4)
#define TIM3_CCMR1_IC1F_MASK                    (0xFU << TIM3_CCMR1_IC1F_SHIFT)
#define TIM3_CCMR1_IC1F(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR1_IC1F_SHIFT)) & TIM3_CCMR1_IC1F_MASK)

#define TIM3_CCMR1_IC1PSC_SHIFT                 (2)
#define TIM3_CCMR1_IC1PSC_MASK                  (0x3U << TIM3_CCMR1_IC1PSC_SHIFT)
#define TIM3_CCMR1_IC1PSC(x)                    (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR1_IC1PSC_SHIFT)) & TIM3_CCMR1_IC1PSC_MASK)

#define TIM3_CCMR1_CC1S_SHIFT                   (0)
#define TIM3_CCMR1_CC1S_MASK                    (0x3U << TIM3_CCMR1_CC1S_SHIFT)
#define TIM3_CCMR1_CC1S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR1_CC1S_SHIFT)) & TIM3_CCMR1_CC1S_MASK)

/*!
 * @brief TIM3_CCMR2 Register Bit Definition
 */

#define TIM3_CCMR2_OC4CE_SHIFT                  (15)
#define TIM3_CCMR2_OC4CE_MASK                   (0x01U << TIM3_CCMR2_OC4CE_SHIFT)
#define TIM3_CCMR2_OC4CE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR2_OC4CE_SHIFT)) & TIM3_CCMR2_OC4CE_MASK)

#define TIM3_CCMR2_OC4M_SHIFT                   (12)
#define TIM3_CCMR2_OC4M_MASK                    (0x7U << TIM3_CCMR2_OC4M_SHIFT)
#define TIM3_CCMR2_OC4M(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR2_OC4M_SHIFT)) & TIM3_CCMR2_OC4M_MASK)

#define TIM3_CCMR2_OC4PE_SHIFT                  (11)
#define TIM3_CCMR2_OC4PE_MASK                   (0x01U << TIM3_CCMR2_OC4PE_SHIFT)
#define TIM3_CCMR2_OC4PE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR2_OC4PE_SHIFT)) & TIM3_CCMR2_OC4PE_MASK)

#define TIM3_CCMR2_OC4FE_SHIFT                  (10)
#define TIM3_CCMR2_OC4FE_MASK                   (0x01U << TIM3_CCMR2_OC4FE_SHIFT)
#define TIM3_CCMR2_OC4FE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR2_OC4FE_SHIFT)) & TIM3_CCMR2_OC4FE_MASK)

#define TIM3_CCMR2_CC4S_SHIFT                   (8)
#define TIM3_CCMR2_CC4S_MASK                    (0x3U << TIM3_CCMR2_CC4S_SHIFT)
#define TIM3_CCMR2_CC4S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR2_CC4S_SHIFT)) & TIM3_CCMR2_CC4S_MASK)

#define TIM3_CCMR2_OC3CE_SHIFT                  (7)
#define TIM3_CCMR2_OC3CE_MASK                   (0x01U << TIM3_CCMR2_OC3CE_SHIFT)
#define TIM3_CCMR2_OC3CE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR2_OC3CE_SHIFT)) & TIM3_CCMR2_OC3CE_MASK)

#define TIM3_CCMR2_OC3M_SHIFT                   (4)
#define TIM3_CCMR2_OC3M_MASK                    (0x7U << TIM3_CCMR2_OC3M_SHIFT)
#define TIM3_CCMR2_OC3M(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR2_OC3M_SHIFT)) & TIM3_CCMR2_OC3M_MASK)

#define TIM3_CCMR2_OC3PE_SHIFT                  (3)
#define TIM3_CCMR2_OC3PE_MASK                   (0x01U << TIM3_CCMR2_OC3PE_SHIFT)
#define TIM3_CCMR2_OC3PE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR2_OC3PE_SHIFT)) & TIM3_CCMR2_OC3PE_MASK)

#define TIM3_CCMR2_OC3FE_SHIFT                  (2)
#define TIM3_CCMR2_OC3FE_MASK                   (0x01U << TIM3_CCMR2_OC3FE_SHIFT)
#define TIM3_CCMR2_OC3FE(x)                     (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR2_OC3FE_SHIFT)) & TIM3_CCMR2_OC3FE_MASK)

#define TIM3_CCMR2_CC3S_SHIFT                   (0)
#define TIM3_CCMR2_CC3S_MASK                    (0x3U << TIM3_CCMR2_CC3S_SHIFT)
#define TIM3_CCMR2_CC3S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR2_CC3S_SHIFT)) & TIM3_CCMR2_CC3S_MASK)

/*!
 * @brief TIM3_CCMR2 Register Bit Definition
 */

#define TIM3_CCMR2_IC4F_SHIFT                   (12)
#define TIM3_CCMR2_IC4F_MASK                    (0xFU << TIM3_CCMR2_IC4F_SHIFT)
#define TIM3_CCMR2_IC4F(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR2_IC4F_SHIFT)) & TIM3_CCMR2_IC4F_MASK)

#define TIM3_CCMR2_IC4PSC_SHIFT                 (10)
#define TIM3_CCMR2_IC4PSC_MASK                  (0x3U << TIM3_CCMR2_IC4PSC_SHIFT)
#define TIM3_CCMR2_IC4PSC(x)                    (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR2_IC4PSC_SHIFT)) & TIM3_CCMR2_IC4PSC_MASK)

#define TIM3_CCMR2_CC4S_SHIFT                   (8)
#define TIM3_CCMR2_CC4S_MASK                    (0x3U << TIM3_CCMR2_CC4S_SHIFT)
#define TIM3_CCMR2_CC4S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR2_CC4S_SHIFT)) & TIM3_CCMR2_CC4S_MASK)

#define TIM3_CCMR2_IC3F_SHIFT                   (4)
#define TIM3_CCMR2_IC3F_MASK                    (0xFU << TIM3_CCMR2_IC3F_SHIFT)
#define TIM3_CCMR2_IC3F(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR2_IC3F_SHIFT)) & TIM3_CCMR2_IC3F_MASK)

#define TIM3_CCMR2_IC3PSC_SHIFT                 (2)
#define TIM3_CCMR2_IC3PSC_MASK                  (0x3U << TIM3_CCMR2_IC3PSC_SHIFT)
#define TIM3_CCMR2_IC3PSC(x)                    (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR2_IC3PSC_SHIFT)) & TIM3_CCMR2_IC3PSC_MASK)

#define TIM3_CCMR2_CC3S_SHIFT                   (0)
#define TIM3_CCMR2_CC3S_MASK                    (0x3U << TIM3_CCMR2_CC3S_SHIFT)
#define TIM3_CCMR2_CC3S(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCMR2_CC3S_SHIFT)) & TIM3_CCMR2_CC3S_MASK)

/*!
 * @brief TIM3_CCER Register Bit Definition
 */

#define TIM3_CCER_CC4NP_SHIFT                   (15)
#define TIM3_CCER_CC4NP_MASK                    (0x01U << TIM3_CCER_CC4NP_SHIFT)
#define TIM3_CCER_CC4NP(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCER_CC4NP_SHIFT)) & TIM3_CCER_CC4NP_MASK)

#define TIM3_CCER_CC4P_SHIFT                    (13)
#define TIM3_CCER_CC4P_MASK                     (0x01U << TIM3_CCER_CC4P_SHIFT)
#define TIM3_CCER_CC4P(x)                       (((uint32_t)(((uint32_t)(x)) << TIM3_CCER_CC4P_SHIFT)) & TIM3_CCER_CC4P_MASK)

#define TIM3_CCER_CC4E_SHIFT                    (12)
#define TIM3_CCER_CC4E_MASK                     (0x01U << TIM3_CCER_CC4E_SHIFT)
#define TIM3_CCER_CC4E(x)                       (((uint32_t)(((uint32_t)(x)) << TIM3_CCER_CC4E_SHIFT)) & TIM3_CCER_CC4E_MASK)

#define TIM3_CCER_CC3NP_SHIFT                   (11)
#define TIM3_CCER_CC3NP_MASK                    (0x01U << TIM3_CCER_CC3NP_SHIFT)
#define TIM3_CCER_CC3NP(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCER_CC3NP_SHIFT)) & TIM3_CCER_CC3NP_MASK)

#define TIM3_CCER_CC3P_SHIFT                    (9)
#define TIM3_CCER_CC3P_MASK                     (0x01U << TIM3_CCER_CC3P_SHIFT)
#define TIM3_CCER_CC3P(x)                       (((uint32_t)(((uint32_t)(x)) << TIM3_CCER_CC3P_SHIFT)) & TIM3_CCER_CC3P_MASK)

#define TIM3_CCER_CC3E_SHIFT                    (8)
#define TIM3_CCER_CC3E_MASK                     (0x01U << TIM3_CCER_CC3E_SHIFT)
#define TIM3_CCER_CC3E(x)                       (((uint32_t)(((uint32_t)(x)) << TIM3_CCER_CC3E_SHIFT)) & TIM3_CCER_CC3E_MASK)

#define TIM3_CCER_CC2NP_SHIFT                   (7)
#define TIM3_CCER_CC2NP_MASK                    (0x01U << TIM3_CCER_CC2NP_SHIFT)
#define TIM3_CCER_CC2NP(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCER_CC2NP_SHIFT)) & TIM3_CCER_CC2NP_MASK)

#define TIM3_CCER_CC2P_SHIFT                    (5)
#define TIM3_CCER_CC2P_MASK                     (0x01U << TIM3_CCER_CC2P_SHIFT)
#define TIM3_CCER_CC2P(x)                       (((uint32_t)(((uint32_t)(x)) << TIM3_CCER_CC2P_SHIFT)) & TIM3_CCER_CC2P_MASK)

#define TIM3_CCER_CC2E_SHIFT                    (4)
#define TIM3_CCER_CC2E_MASK                     (0x01U << TIM3_CCER_CC2E_SHIFT)
#define TIM3_CCER_CC2E(x)                       (((uint32_t)(((uint32_t)(x)) << TIM3_CCER_CC2E_SHIFT)) & TIM3_CCER_CC2E_MASK)

#define TIM3_CCER_CC1NP_SHIFT                   (3)
#define TIM3_CCER_CC1NP_MASK                    (0x01U << TIM3_CCER_CC1NP_SHIFT)
#define TIM3_CCER_CC1NP(x)                      (((uint32_t)(((uint32_t)(x)) << TIM3_CCER_CC1NP_SHIFT)) & TIM3_CCER_CC1NP_MASK)

#define TIM3_CCER_CC1P_SHIFT                    (1)
#define TIM3_CCER_CC1P_MASK                     (0x01U << TIM3_CCER_CC1P_SHIFT)
#define TIM3_CCER_CC1P(x)                       (((uint32_t)(((uint32_t)(x)) << TIM3_CCER_CC1P_SHIFT)) & TIM3_CCER_CC1P_MASK)

#define TIM3_CCER_CC1E_SHIFT                    (0)
#define TIM3_CCER_CC1E_MASK                     (0x01U << TIM3_CCER_CC1E_SHIFT)
#define TIM3_CCER_CC1E(x)                       (((uint32_t)(((uint32_t)(x)) << TIM3_CCER_CC1E_SHIFT)) & TIM3_CCER_CC1E_MASK)

/*!
 * @brief TIM3_CNT Register Bit Definition
 */

#define TIM3_CNT_CNT_SHIFT                      (0)
#define TIM3_CNT_CNT_MASK                       (0xFFFFU << TIM3_CNT_CNT_SHIFT)
#define TIM3_CNT_CNT(x)                         (((uint32_t)(((uint32_t)(x)) << TIM3_CNT_CNT_SHIFT)) & TIM3_CNT_CNT_MASK)

/*!
 * @brief TIM3_PSC Register Bit Definition
 */

#define TIM3_PSC_PSC_SHIFT                      (0)
#define TIM3_PSC_PSC_MASK                       (0xFFFFU << TIM3_PSC_PSC_SHIFT)
#define TIM3_PSC_PSC(x)                         (((uint32_t)(((uint32_t)(x)) << TIM3_PSC_PSC_SHIFT)) & TIM3_PSC_PSC_MASK)

/*!
 * @brief TIM3_ARR Register Bit Definition
 */

#define TIM3_ARR_ARR_SHIFT                      (0)
#define TIM3_ARR_ARR_MASK                       (0xFFFFU << TIM3_ARR_ARR_SHIFT)
#define TIM3_ARR_ARR(x)                         (((uint32_t)(((uint32_t)(x)) << TIM3_ARR_ARR_SHIFT)) & TIM3_ARR_ARR_MASK)

/*!
 * @brief TIM3_CCR Register Bit Definition
 */

#define TIM3_CCR_CCR_SHIFT                      (0)
#define TIM3_CCR_CCR_MASK                       (0xFFFFU << TIM3_CCR_CCR_SHIFT)
#define TIM3_CCR_CCR(x)                         (((uint32_t)(((uint32_t)(x)) << TIM3_CCR_CCR_SHIFT)) & TIM3_CCR_CCR_MASK)

/*!
 * @brief TIM3_DCR Register Bit Definition
 */

#define TIM3_DCR_DBL_SHIFT                      (8)
#define TIM3_DCR_DBL_MASK                       (0x1FU << TIM3_DCR_DBL_SHIFT)
#define TIM3_DCR_DBL(x)                         (((uint32_t)(((uint32_t)(x)) << TIM3_DCR_DBL_SHIFT)) & TIM3_DCR_DBL_MASK)

#define TIM3_DCR_DBA_SHIFT                      (0)
#define TIM3_DCR_DBA_MASK                       (0x1FU << TIM3_DCR_DBA_SHIFT)
#define TIM3_DCR_DBA(x)                         (((uint32_t)(((uint32_t)(x)) << TIM3_DCR_DBA_SHIFT)) & TIM3_DCR_DBA_MASK)

/*!
 * @brief TIM3_DMAR Register Bit Definition
 */

#define TIM3_DMAR_DMAB_SHIFT                    (0)
#define TIM3_DMAR_DMAB_MASK                     (0xFFFFU << TIM3_DMAR_DMAB_SHIFT)
#define TIM3_DMAR_DMAB(x)                       (((uint32_t)(((uint32_t)(x)) << TIM3_DMAR_DMAB_SHIFT)) & TIM3_DMAR_DMAB_MASK)

/*!
 * @}
 */ /* end of group TIM3_Register_Masks */
/******************************************************************************
 *TIM3 Instance
*******************************************************************************/

#define TIM3                 ((TIM3_Type*)TIM3_BASE)
#define TIM4                 ((TIM3_Type*)TIM4_BASE)

/*!
 * @}
 */ /* end of group TIM3_Peripheral_Access_Layer */

/*!
 * @addtogroup TIM6_Peripheral_Access_Layer TIM6 Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * TIM6 Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t CR1;                                                            ///< Control Register 1                           offset: 0x00
    __IO uint32_t Reserved53[2];                                                   ///< Reserved
    __IO uint32_t DIER;                                                           ///< DMA/Interrupt Enable Register                offset: 0x0C
    __IO uint32_t SR;                                                             ///< status register                              offset: 0x10
    __IO uint32_t EGR;                                                            ///< event generation register                    offset: 0x14
    __IO uint32_t Reserved54[3];                                                   ///< Reserved
    __IO uint32_t CNT;                                                            ///< counter                                      offset: 0x24
    __IO uint32_t PSC;                                                            ///< Prescaler                                    offset: 0x28
    __IO uint32_t ARR;                                                            ///< autoload register                            offset: 0x2C
} TIM6_Type;

/*******************************************************************************
 * TIM6 Type
 ******************************************************************************/

/*!
 * @addtogroup TIM6_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief TIM6_CR1 Register Bit Definition
 */

#define TIM6_CR1_ARPE_SHIFT                     (7)
#define TIM6_CR1_ARPE_MASK                      (0x01U << TIM6_CR1_ARPE_SHIFT)
#define TIM6_CR1_ARPE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM6_CR1_ARPE_SHIFT)) & TIM6_CR1_ARPE_MASK)

#define TIM6_CR1_OPM_SHIFT                      (3)
#define TIM6_CR1_OPM_MASK                       (0x01U << TIM6_CR1_OPM_SHIFT)
#define TIM6_CR1_OPM(x)                         (((uint32_t)(((uint32_t)(x)) << TIM6_CR1_OPM_SHIFT)) & TIM6_CR1_OPM_MASK)

#define TIM6_CR1_URS_SHIFT                      (2)
#define TIM6_CR1_URS_MASK                       (0x01U << TIM6_CR1_URS_SHIFT)
#define TIM6_CR1_URS(x)                         (((uint32_t)(((uint32_t)(x)) << TIM6_CR1_URS_SHIFT)) & TIM6_CR1_URS_MASK)

#define TIM6_CR1_UDIS_SHIFT                     (1)
#define TIM6_CR1_UDIS_MASK                      (0x01U << TIM6_CR1_UDIS_SHIFT)
#define TIM6_CR1_UDIS(x)                        (((uint32_t)(((uint32_t)(x)) << TIM6_CR1_UDIS_SHIFT)) & TIM6_CR1_UDIS_MASK)

#define TIM6_CR1_CEN_SHIFT                      (0)
#define TIM6_CR1_CEN_MASK                       (0x01U << TIM6_CR1_CEN_SHIFT)
#define TIM6_CR1_CEN(x)                         (((uint32_t)(((uint32_t)(x)) << TIM6_CR1_CEN_SHIFT)) & TIM6_CR1_CEN_MASK)

/*!
 * @brief TIM6_DIER Register Bit Definition
 */

#define TIM6_DIER_UDE_SHIFT                     (8)
#define TIM6_DIER_UDE_MASK                      (0x01U << TIM6_DIER_UDE_SHIFT)
#define TIM6_DIER_UDE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM6_DIER_UDE_SHIFT)) & TIM6_DIER_UDE_MASK)

#define TIM6_DIER_UIE_SHIFT                     (0)
#define TIM6_DIER_UIE_MASK                      (0x01U << TIM6_DIER_UIE_SHIFT)
#define TIM6_DIER_UIE(x)                        (((uint32_t)(((uint32_t)(x)) << TIM6_DIER_UIE_SHIFT)) & TIM6_DIER_UIE_MASK)

/*!
 * @brief TIM6_SR Register Bit Definition
 */

#define TIM6_SR_UIF_SHIFT                       (0)
#define TIM6_SR_UIF_MASK                        (0x01U << TIM6_SR_UIF_SHIFT)
#define TIM6_SR_UIF(x)                          (((uint32_t)(((uint32_t)(x)) << TIM6_SR_UIF_SHIFT)) & TIM6_SR_UIF_MASK)

/*!
 * @brief TIM6_EGR Register Bit Definition
 */

#define TIM6_EGR_UG_SHIFT                       (0)
#define TIM6_EGR_UG_MASK                        (0x01U << TIM6_EGR_UG_SHIFT)
#define TIM6_EGR_UG(x)                          (((uint32_t)(((uint32_t)(x)) << TIM6_EGR_UG_SHIFT)) & TIM6_EGR_UG_MASK)

/*!
 * @brief TIM6_CNT Register Bit Definition
 */

#define TIM6_CNT_CNT_SHIFT                      (0)
#define TIM6_CNT_CNT_MASK                       (0xFFFFU << TIM6_CNT_CNT_SHIFT)
#define TIM6_CNT_CNT(x)                         (((uint32_t)(((uint32_t)(x)) << TIM6_CNT_CNT_SHIFT)) & TIM6_CNT_CNT_MASK)

/*!
 * @brief TIM6_PSC Register Bit Definition
 */

#define TIM6_PSC_PSC_SHIFT                      (0)
#define TIM6_PSC_PSC_MASK                       (0xFFFFU << TIM6_PSC_PSC_SHIFT)
#define TIM6_PSC_PSC(x)                         (((uint32_t)(((uint32_t)(x)) << TIM6_PSC_PSC_SHIFT)) & TIM6_PSC_PSC_MASK)

/*!
 * @brief TIM6_ARR Register Bit Definition
 */

#define TIM6_ARR_ARR_SHIFT                      (0)
#define TIM6_ARR_ARR_MASK                       (0xFFFFU << TIM6_ARR_ARR_SHIFT)
#define TIM6_ARR_ARR(x)                         (((uint32_t)(((uint32_t)(x)) << TIM6_ARR_ARR_SHIFT)) & TIM6_ARR_ARR_MASK)

/*!
 * @}
 */ /* end of group TIM6_Register_Masks */
/******************************************************************************
 *TIM6 Instance
*******************************************************************************/

#define TIM6                 ((TIM6_Type*)TIM6_BASE)

/*!
 * @}
 */ /* end of group TIM6_Peripheral_Access_Layer */




/*!
 * @addtogroup UART_Peripheral_Access_Layer UART Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * UART Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t TDR;                                                            ///< UART transmit data register                  offset: 0x00
    __IO uint32_t RDR;                                                            ///< UART receive data register                   offset: 0x04
    __IO uint32_t CSR;                                                            ///< UART current status register                 offset: 0x08
    __IO uint32_t ISR;                                                            ///< UART Interrupt Status Register               offset: 0x0C
    __IO uint32_t IER;                                                            ///< UART Interrupt Enable Register               offset: 0x10
    __IO uint32_t ICR;                                                            ///< UART Interrupt Clear Register                offset: 0x14
    __IO uint32_t GCR;                                                            ///< UART Global Control Register                 offset: 0x18
    __IO uint32_t CCR;                                                            ///< UART General Control Register                offset: 0x1C
    __IO uint32_t BRR;                                                            ///< UART Baud Rate Register                      offset: 0x20
    __IO uint32_t FRA;                                                            ///< UART Fractional Baud Rate Register           offset: 0x24
    __IO uint32_t RXADDR;                                                         ///< UART receive address register                offset: 0x28
    __IO uint32_t RXMASK;                                                         ///< UART Receive Mask Register                   offset: 0x2C
    __IO uint32_t SCR;                                                            ///< UART SCR register                            offset: 0x30
    __IO uint32_t IDLR;                                                           ///< UART IDLE Data Length Register               offset: 0x34
    __IO uint32_t ABRCR;                                                          ///< UART ABRCR Auto-Baud Rate Control Register   offset: 0x38
    __IO uint32_t IRDA;                                                           ///< UART IRDA infrared function control register offset: 0x3C
} UART_Type;

/*******************************************************************************
 * UART Type
 ******************************************************************************/

/*!
 * @addtogroup UART_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief UART_TDR Register Bit Definition
 */

#define UART_TDR_TXREG_SHIFT                    (0)
#define UART_TDR_TXREG_MASK                     (0x1FFU << UART_TDR_TXREG_SHIFT)
#define UART_TDR_TXREG(x)                       (((uint32_t)(((uint32_t)(x)) << UART_TDR_TXREG_SHIFT)) & UART_TDR_TXREG_MASK)

/*!
 * @brief UART_RDR Register Bit Definition
 */

#define UART_RDR_RXREG_SHIFT                    (0)
#define UART_RDR_RXREG_MASK                     (0x1FFU << UART_RDR_RXREG_SHIFT)
#define UART_RDR_RXREG(x)                       (((uint32_t)(((uint32_t)(x)) << UART_RDR_RXREG_SHIFT)) & UART_RDR_RXREG_MASK)

/*!
 * @brief UART_CSR Register Bit Definition
 */

#define UART_CSR_TXEPT_SHIFT                    (3)
#define UART_CSR_TXEPT_MASK                     (0x01U << UART_CSR_TXEPT_SHIFT)
#define UART_CSR_TXEPT(x)                       (((uint32_t)(((uint32_t)(x)) << UART_CSR_TXEPT_SHIFT)) & UART_CSR_TXEPT_MASK)

#define UART_CSR_TXFULL_SHIFT                   (2)
#define UART_CSR_TXFULL_MASK                    (0x01U << UART_CSR_TXFULL_SHIFT)
#define UART_CSR_TXFULL(x)                      (((uint32_t)(((uint32_t)(x)) << UART_CSR_TXFULL_SHIFT)) & UART_CSR_TXFULL_MASK)

#define UART_CSR_RXAVL_SHIFT                    (1)
#define UART_CSR_RXAVL_MASK                     (0x01U << UART_CSR_RXAVL_SHIFT)
#define UART_CSR_RXAVL(x)                       (((uint32_t)(((uint32_t)(x)) << UART_CSR_RXAVL_SHIFT)) & UART_CSR_RXAVL_MASK)

#define UART_CSR_TXC_SHIFT                      (0)
#define UART_CSR_TXC_MASK                       (0x01U << UART_CSR_TXC_SHIFT)
#define UART_CSR_TXC(x)                         (((uint32_t)(((uint32_t)(x)) << UART_CSR_TXC_SHIFT)) & UART_CSR_TXC_MASK)

/*!
 * @brief UART_ISR Register Bit Definition
 */

#define UART_ISR_ABRERRINTF_SHIFT               (11)
#define UART_ISR_ABRERRINTF_MASK                (0x01U << UART_ISR_ABRERRINTF_SHIFT)
#define UART_ISR_ABRERRINTF(x)                  (((uint32_t)(((uint32_t)(x)) << UART_ISR_ABRERRINTF_SHIFT)) & UART_ISR_ABRERRINTF_MASK)

#define UART_ISR_ABRENDINTF_SHIFT               (10)
#define UART_ISR_ABRENDINTF_MASK                (0x01U << UART_ISR_ABRENDINTF_SHIFT)
#define UART_ISR_ABRENDINTF(x)                  (((uint32_t)(((uint32_t)(x)) << UART_ISR_ABRENDINTF_SHIFT)) & UART_ISR_ABRENDINTF_MASK)

#define UART_ISR_RXIDLEINTF_SHIFT               (9)
#define UART_ISR_RXIDLEINTF_MASK                (0x01U << UART_ISR_RXIDLEINTF_SHIFT)
#define UART_ISR_RXIDLEINTF(x)                  (((uint32_t)(((uint32_t)(x)) << UART_ISR_RXIDLEINTF_SHIFT)) & UART_ISR_RXIDLEINTF_MASK)

#define UART_ISR_RXB8INTF_SHIFT                 (8)
#define UART_ISR_RXB8INTF_MASK                  (0x01U << UART_ISR_RXB8INTF_SHIFT)
#define UART_ISR_RXB8INTF(x)                    (((uint32_t)(((uint32_t)(x)) << UART_ISR_RXB8INTF_SHIFT)) & UART_ISR_RXB8INTF_MASK)

#define UART_ISR_TXBRKINTF_SHIFT                (7)
#define UART_ISR_TXBRKINTF_MASK                 (0x01U << UART_ISR_TXBRKINTF_SHIFT)
#define UART_ISR_TXBRKINTF(x)                   (((uint32_t)(((uint32_t)(x)) << UART_ISR_TXBRKINTF_SHIFT)) & UART_ISR_TXBRKINTF_MASK)

#define UART_ISR_RXBRKINTF_SHIFT                (6)
#define UART_ISR_RXBRKINTF_MASK                 (0x01U << UART_ISR_RXBRKINTF_SHIFT)
#define UART_ISR_RXBRKINTF(x)                   (((uint32_t)(((uint32_t)(x)) << UART_ISR_RXBRKINTF_SHIFT)) & UART_ISR_RXBRKINTF_MASK)

#define UART_ISR_RXFERRINTF_SHIFT               (5)
#define UART_ISR_RXFERRINTF_MASK                (0x01U << UART_ISR_RXFERRINTF_SHIFT)
#define UART_ISR_RXFERRINTF(x)                  (((uint32_t)(((uint32_t)(x)) << UART_ISR_RXFERRINTF_SHIFT)) & UART_ISR_RXFERRINTF_MASK)

#define UART_ISR_RXPERRINTF_SHIFT               (4)
#define UART_ISR_RXPERRINTF_MASK                (0x01U << UART_ISR_RXPERRINTF_SHIFT)
#define UART_ISR_RXPERRINTF(x)                  (((uint32_t)(((uint32_t)(x)) << UART_ISR_RXPERRINTF_SHIFT)) & UART_ISR_RXPERRINTF_MASK)

#define UART_ISR_RXOERRINTF_SHIFT               (3)
#define UART_ISR_RXOERRINTF_MASK                (0x01U << UART_ISR_RXOERRINTF_SHIFT)
#define UART_ISR_RXOERRINTF(x)                  (((uint32_t)(((uint32_t)(x)) << UART_ISR_RXOERRINTF_SHIFT)) & UART_ISR_RXOERRINTF_MASK)

#define UART_ISR_TXCINTF_SHIFT                  (2)
#define UART_ISR_TXCINTF_MASK                   (0x01U << UART_ISR_TXCINTF_SHIFT)
#define UART_ISR_TXCINTF(x)                     (((uint32_t)(((uint32_t)(x)) << UART_ISR_TXCINTF_SHIFT)) & UART_ISR_TXCINTF_MASK)

#define UART_ISR_RXINTF_SHIFT                   (1)
#define UART_ISR_RXINTF_MASK                    (0x01U << UART_ISR_RXINTF_SHIFT)
#define UART_ISR_RXINTF(x)                      (((uint32_t)(((uint32_t)(x)) << UART_ISR_RXINTF_SHIFT)) & UART_ISR_RXINTF_MASK)

#define UART_ISR_TXINTF_SHIFT                   (0)
#define UART_ISR_TXINTF_MASK                    (0x01U << UART_ISR_TXINTF_SHIFT)
#define UART_ISR_TXINTF(x)                      (((uint32_t)(((uint32_t)(x)) << UART_ISR_TXINTF_SHIFT)) & UART_ISR_TXINTF_MASK)

/*!
 * @brief UART_IER Register Bit Definition
 */

#define UART_IER_ABRERRIEN_SHIFT                (11)
#define UART_IER_ABRERRIEN_MASK                 (0x01U << UART_IER_ABRERRIEN_SHIFT)
#define UART_IER_ABRERRIEN(x)                   (((uint32_t)(((uint32_t)(x)) << UART_IER_ABRERRIEN_SHIFT)) & UART_IER_ABRERRIEN_MASK)

#define UART_IER_ABRENDIEN_SHIFT                (10)
#define UART_IER_ABRENDIEN_MASK                 (0x01U << UART_IER_ABRENDIEN_SHIFT)
#define UART_IER_ABRENDIEN(x)                   (((uint32_t)(((uint32_t)(x)) << UART_IER_ABRENDIEN_SHIFT)) & UART_IER_ABRENDIEN_MASK)

#define UART_IER_RXIDLEIEN_SHIFT                (9)
#define UART_IER_RXIDLEIEN_MASK                 (0x01U << UART_IER_RXIDLEIEN_SHIFT)
#define UART_IER_RXIDLEIEN(x)                   (((uint32_t)(((uint32_t)(x)) << UART_IER_RXIDLEIEN_SHIFT)) & UART_IER_RXIDLEIEN_MASK)

#define UART_IER_RXB8IEN_SHIFT                  (8)
#define UART_IER_RXB8IEN_MASK                   (0x01U << UART_IER_RXB8IEN_SHIFT)
#define UART_IER_RXB8IEN(x)                     (((uint32_t)(((uint32_t)(x)) << UART_IER_RXB8IEN_SHIFT)) & UART_IER_RXB8IEN_MASK)

#define UART_IER_TXBRKIEN_SHIFT                 (7)
#define UART_IER_TXBRKIEN_MASK                  (0x01U << UART_IER_TXBRKIEN_SHIFT)
#define UART_IER_TXBRKIEN(x)                    (((uint32_t)(((uint32_t)(x)) << UART_IER_TXBRKIEN_SHIFT)) & UART_IER_TXBRKIEN_MASK)

#define UART_IER_RXBRKIEN_SHIFT                 (6)
#define UART_IER_RXBRKIEN_MASK                  (0x01U << UART_IER_RXBRKIEN_SHIFT)
#define UART_IER_RXBRKIEN(x)                    (((uint32_t)(((uint32_t)(x)) << UART_IER_RXBRKIEN_SHIFT)) & UART_IER_RXBRKIEN_MASK)

#define UART_IER_RXFERRIEN_SHIFT                (5)
#define UART_IER_RXFERRIEN_MASK                 (0x01U << UART_IER_RXFERRIEN_SHIFT)
#define UART_IER_RXFERRIEN(x)                   (((uint32_t)(((uint32_t)(x)) << UART_IER_RXFERRIEN_SHIFT)) & UART_IER_RXFERRIEN_MASK)

#define UART_IER_RXPERRIEN_SHIFT                (4)
#define UART_IER_RXPERRIEN_MASK                 (0x01U << UART_IER_RXPERRIEN_SHIFT)
#define UART_IER_RXPERRIEN(x)                   (((uint32_t)(((uint32_t)(x)) << UART_IER_RXPERRIEN_SHIFT)) & UART_IER_RXPERRIEN_MASK)

#define UART_IER_RXOERRIEN_SHIFT                (3)
#define UART_IER_RXOERRIEN_MASK                 (0x01U << UART_IER_RXOERRIEN_SHIFT)
#define UART_IER_RXOERRIEN(x)                   (((uint32_t)(((uint32_t)(x)) << UART_IER_RXOERRIEN_SHIFT)) & UART_IER_RXOERRIEN_MASK)

#define UART_IER_TXCIEN_SHIFT                   (2)
#define UART_IER_TXCIEN_MASK                    (0x01U << UART_IER_TXCIEN_SHIFT)
#define UART_IER_TXCIEN(x)                      (((uint32_t)(((uint32_t)(x)) << UART_IER_TXCIEN_SHIFT)) & UART_IER_TXCIEN_MASK)

#define UART_IER_RXIEN_SHIFT                    (1)
#define UART_IER_RXIEN_MASK                     (0x01U << UART_IER_RXIEN_SHIFT)
#define UART_IER_RXIEN(x)                       (((uint32_t)(((uint32_t)(x)) << UART_IER_RXIEN_SHIFT)) & UART_IER_RXIEN_MASK)

#define UART_IER_TXIEN_SHIFT                    (0)
#define UART_IER_TXIEN_MASK                     (0x01U << UART_IER_TXIEN_SHIFT)
#define UART_IER_TXIEN(x)                       (((uint32_t)(((uint32_t)(x)) << UART_IER_TXIEN_SHIFT)) & UART_IER_TXIEN_MASK)

/*!
 * @brief UART_ICR Register Bit Definition
 */

#define UART_ICR_ABRERRICLR_SHIFT               (11)
#define UART_ICR_ABRERRICLR_MASK                (0x01U << UART_ICR_ABRERRICLR_SHIFT)
#define UART_ICR_ABRERRICLR(x)                  (((uint32_t)(((uint32_t)(x)) << UART_ICR_ABRERRICLR_SHIFT)) & UART_ICR_ABRERRICLR_MASK)

#define UART_ICR_ABRENDICLR_SHIFT               (10)
#define UART_ICR_ABRENDICLR_MASK                (0x01U << UART_ICR_ABRENDICLR_SHIFT)
#define UART_ICR_ABRENDICLR(x)                  (((uint32_t)(((uint32_t)(x)) << UART_ICR_ABRENDICLR_SHIFT)) & UART_ICR_ABRENDICLR_MASK)

#define UART_ICR_RXIDLEICLR_SHIFT               (9)
#define UART_ICR_RXIDLEICLR_MASK                (0x01U << UART_ICR_RXIDLEICLR_SHIFT)
#define UART_ICR_RXIDLEICLR(x)                  (((uint32_t)(((uint32_t)(x)) << UART_ICR_RXIDLEICLR_SHIFT)) & UART_ICR_RXIDLEICLR_MASK)

#define UART_ICR_RXB8ICLR_SHIFT                 (8)
#define UART_ICR_RXB8ICLR_MASK                  (0x01U << UART_ICR_RXB8ICLR_SHIFT)
#define UART_ICR_RXB8ICLR(x)                    (((uint32_t)(((uint32_t)(x)) << UART_ICR_RXB8ICLR_SHIFT)) & UART_ICR_RXB8ICLR_MASK)

#define UART_ICR_TXBRKICLR_SHIFT                (7)
#define UART_ICR_TXBRKICLR_MASK                 (0x01U << UART_ICR_TXBRKICLR_SHIFT)
#define UART_ICR_TXBRKICLR(x)                   (((uint32_t)(((uint32_t)(x)) << UART_ICR_TXBRKICLR_SHIFT)) & UART_ICR_TXBRKICLR_MASK)

#define UART_ICR_RXBRKICLR_SHIFT                (6)
#define UART_ICR_RXBRKICLR_MASK                 (0x01U << UART_ICR_RXBRKICLR_SHIFT)
#define UART_ICR_RXBRKICLR(x)                   (((uint32_t)(((uint32_t)(x)) << UART_ICR_RXBRKICLR_SHIFT)) & UART_ICR_RXBRKICLR_MASK)

#define UART_ICR_RXFERRICLR_SHIFT               (5)
#define UART_ICR_RXFERRICLR_MASK                (0x01U << UART_ICR_RXFERRICLR_SHIFT)
#define UART_ICR_RXFERRICLR(x)                  (((uint32_t)(((uint32_t)(x)) << UART_ICR_RXFERRICLR_SHIFT)) & UART_ICR_RXFERRICLR_MASK)

#define UART_ICR_RXPERRICLR_SHIFT               (4)
#define UART_ICR_RXPERRICLR_MASK                (0x01U << UART_ICR_RXPERRICLR_SHIFT)
#define UART_ICR_RXPERRICLR(x)                  (((uint32_t)(((uint32_t)(x)) << UART_ICR_RXPERRICLR_SHIFT)) & UART_ICR_RXPERRICLR_MASK)

#define UART_ICR_RXOERRICLR_SHIFT               (3)
#define UART_ICR_RXOERRICLR_MASK                (0x01U << UART_ICR_RXOERRICLR_SHIFT)
#define UART_ICR_RXOERRICLR(x)                  (((uint32_t)(((uint32_t)(x)) << UART_ICR_RXOERRICLR_SHIFT)) & UART_ICR_RXOERRICLR_MASK)

#define UART_ICR_TXCICLR_SHIFT                  (2)
#define UART_ICR_TXCICLR_MASK                   (0x01U << UART_ICR_TXCICLR_SHIFT)
#define UART_ICR_TXCICLR(x)                     (((uint32_t)(((uint32_t)(x)) << UART_ICR_TXCICLR_SHIFT)) & UART_ICR_TXCICLR_MASK)

#define UART_ICR_RXICLR_SHIFT                   (1)
#define UART_ICR_RXICLR_MASK                    (0x01U << UART_ICR_RXICLR_SHIFT)
#define UART_ICR_RXICLR(x)                      (((uint32_t)(((uint32_t)(x)) << UART_ICR_RXICLR_SHIFT)) & UART_ICR_RXICLR_MASK)

#define UART_ICR_TXICLR_SHIFT                   (0)
#define UART_ICR_TXICLR_MASK                    (0x01U << UART_ICR_TXICLR_SHIFT)
#define UART_ICR_TXICLR(x)                      (((uint32_t)(((uint32_t)(x)) << UART_ICR_TXICLR_SHIFT)) & UART_ICR_TXICLR_MASK)

/*!
 * @brief UART_GCR Register Bit Definition
 */

#define UART_GCR_TXTOG_SHIFT                    (10)
#define UART_GCR_TXTOG_MASK                     (0x01U << UART_GCR_TXTOG_SHIFT)
#define UART_GCR_TXTOG(x)                       (((uint32_t)(((uint32_t)(x)) << UART_GCR_TXTOG_SHIFT)) & UART_GCR_TXTOG_MASK)

#define UART_GCR_RXTOG_SHIFT                    (9)
#define UART_GCR_RXTOG_MASK                     (0x01U << UART_GCR_RXTOG_SHIFT)
#define UART_GCR_RXTOG(x)                       (((uint32_t)(((uint32_t)(x)) << UART_GCR_RXTOG_SHIFT)) & UART_GCR_RXTOG_MASK)

#define UART_GCR_SWAP_SHIFT                     (8)
#define UART_GCR_SWAP_MASK                      (0x01U << UART_GCR_SWAP_SHIFT)
#define UART_GCR_SWAP(x)                        (((uint32_t)(((uint32_t)(x)) << UART_GCR_SWAP_SHIFT)) & UART_GCR_SWAP_MASK)

#define UART_GCR_SELB8_SHIFT                    (7)
#define UART_GCR_SELB8_MASK                     (0x01U << UART_GCR_SELB8_SHIFT)
#define UART_GCR_SELB8(x)                       (((uint32_t)(((uint32_t)(x)) << UART_GCR_SELB8_SHIFT)) & UART_GCR_SELB8_MASK)

#define UART_GCR_TXEN_SHIFT                     (4)
#define UART_GCR_TXEN_MASK                      (0x01U << UART_GCR_TXEN_SHIFT)
#define UART_GCR_TXEN(x)                        (((uint32_t)(((uint32_t)(x)) << UART_GCR_TXEN_SHIFT)) & UART_GCR_TXEN_MASK)

#define UART_GCR_RXEN_SHIFT                     (3)
#define UART_GCR_RXEN_MASK                      (0x01U << UART_GCR_RXEN_SHIFT)
#define UART_GCR_RXEN(x)                        (((uint32_t)(((uint32_t)(x)) << UART_GCR_RXEN_SHIFT)) & UART_GCR_RXEN_MASK)

#define UART_GCR_AUTOFLOWEN_SHIFT               (2)
#define UART_GCR_AUTOFLOWEN_MASK                (0x01U << UART_GCR_AUTOFLOWEN_SHIFT)
#define UART_GCR_AUTOFLOWEN(x)                  (((uint32_t)(((uint32_t)(x)) << UART_GCR_AUTOFLOWEN_SHIFT)) & UART_GCR_AUTOFLOWEN_MASK)

#define UART_GCR_DMAMODE_SHIFT                  (1)
#define UART_GCR_DMAMODE_MASK                   (0x01U << UART_GCR_DMAMODE_SHIFT)
#define UART_GCR_DMAMODE(x)                     (((uint32_t)(((uint32_t)(x)) << UART_GCR_DMAMODE_SHIFT)) & UART_GCR_DMAMODE_MASK)

#define UART_GCR_UARTEN_SHIFT                   (0)
#define UART_GCR_UARTEN_MASK                    (0x01U << UART_GCR_UARTEN_SHIFT)
#define UART_GCR_UARTEN(x)                      (((uint32_t)(((uint32_t)(x)) << UART_GCR_UARTEN_SHIFT)) & UART_GCR_UARTEN_MASK)

/*!
 * @brief UART_CCR Register Bit Definition
 */

#define UART_CCR_LIN_SHIFT                      (14)
#define UART_CCR_LIN_MASK                       (0x01U << UART_CCR_LIN_SHIFT)
#define UART_CCR_LIN(x)                         (((uint32_t)(((uint32_t)(x)) << UART_CCR_LIN_SHIFT)) & UART_CCR_LIN_MASK)

#define UART_CCR_WAKE_SHIFT                     (13)
#define UART_CCR_WAKE_MASK                      (0x01U << UART_CCR_WAKE_SHIFT)
#define UART_CCR_WAKE(x)                        (((uint32_t)(((uint32_t)(x)) << UART_CCR_WAKE_SHIFT)) & UART_CCR_WAKE_MASK)

#define UART_CCR_RWU_SHIFT                      (12)
#define UART_CCR_RWU_MASK                       (0x01U << UART_CCR_RWU_SHIFT)
#define UART_CCR_RWU(x)                         (((uint32_t)(((uint32_t)(x)) << UART_CCR_RWU_SHIFT)) & UART_CCR_RWU_MASK)

#define UART_CCR_B8EN_SHIFT                     (11)
#define UART_CCR_B8EN_MASK                      (0x01U << UART_CCR_B8EN_SHIFT)
#define UART_CCR_B8EN(x)                        (((uint32_t)(((uint32_t)(x)) << UART_CCR_B8EN_SHIFT)) & UART_CCR_B8EN_MASK)

#define UART_CCR_B8TOG_SHIFT                    (10)
#define UART_CCR_B8TOG_MASK                     (0x01U << UART_CCR_B8TOG_SHIFT)
#define UART_CCR_B8TOG(x)                       (((uint32_t)(((uint32_t)(x)) << UART_CCR_B8TOG_SHIFT)) & UART_CCR_B8TOG_MASK)

#define UART_CCR_B8POL_SHIFT                    (9)
#define UART_CCR_B8POL_MASK                     (0x01U << UART_CCR_B8POL_SHIFT)
#define UART_CCR_B8POL(x)                       (((uint32_t)(((uint32_t)(x)) << UART_CCR_B8POL_SHIFT)) & UART_CCR_B8POL_MASK)

#define UART_CCR_B8TXD_SHIFT                    (8)
#define UART_CCR_B8TXD_MASK                     (0x01U << UART_CCR_B8TXD_SHIFT)
#define UART_CCR_B8TXD(x)                       (((uint32_t)(((uint32_t)(x)) << UART_CCR_B8TXD_SHIFT)) & UART_CCR_B8TXD_MASK)

#define UART_CCR_B8RXD_SHIFT                    (7)
#define UART_CCR_B8RXD_MASK                     (0x01U << UART_CCR_B8RXD_SHIFT)
#define UART_CCR_B8RXD(x)                       (((uint32_t)(((uint32_t)(x)) << UART_CCR_B8RXD_SHIFT)) & UART_CCR_B8RXD_MASK)

#define UART_CCR_SPB1_SHIFT                     (6)
#define UART_CCR_SPB1_MASK                      (0x01U << UART_CCR_SPB1_SHIFT)
#define UART_CCR_SPB1(x)                        (((uint32_t)(((uint32_t)(x)) << UART_CCR_SPB1_SHIFT)) & UART_CCR_SPB1_MASK)

#define UART_CCR_CHAR_SHIFT                     (4)
#define UART_CCR_CHAR_MASK                      (0x3U << UART_CCR_CHAR_SHIFT)
#define UART_CCR_CHAR(x)                        (((uint32_t)(((uint32_t)(x)) << UART_CCR_CHAR_SHIFT)) & UART_CCR_CHAR_MASK)

#define UART_CCR_BRK_SHIFT                      (3)
#define UART_CCR_BRK_MASK                       (0x01U << UART_CCR_BRK_SHIFT)
#define UART_CCR_BRK(x)                         (((uint32_t)(((uint32_t)(x)) << UART_CCR_BRK_SHIFT)) & UART_CCR_BRK_MASK)

#define UART_CCR_SPB0_SHIFT                     (2)
#define UART_CCR_SPB0_MASK                      (0x01U << UART_CCR_SPB0_SHIFT)
#define UART_CCR_SPB0(x)                        (((uint32_t)(((uint32_t)(x)) << UART_CCR_SPB0_SHIFT)) & UART_CCR_SPB0_MASK)

#define UART_CCR_PSEL_SHIFT                     (1)
#define UART_CCR_PSEL_MASK                      (0x01U << UART_CCR_PSEL_SHIFT)
#define UART_CCR_PSEL(x)                        (((uint32_t)(((uint32_t)(x)) << UART_CCR_PSEL_SHIFT)) & UART_CCR_PSEL_MASK)

#define UART_CCR_PEN_SHIFT                      (0)
#define UART_CCR_PEN_MASK                       (0x01U << UART_CCR_PEN_SHIFT)
#define UART_CCR_PEN(x)                         (((uint32_t)(((uint32_t)(x)) << UART_CCR_PEN_SHIFT)) & UART_CCR_PEN_MASK)

/*!
 * @brief UART_BRR Register Bit Definition
 */

#define UART_BRR_DIVMANTISSA_SHIFT              (0)
#define UART_BRR_DIVMANTISSA_MASK               (0xFFFFU << UART_BRR_DIVMANTISSA_SHIFT)
#define UART_BRR_DIVMANTISSA(x)                 (((uint32_t)(((uint32_t)(x)) << UART_BRR_DIVMANTISSA_SHIFT)) & UART_BRR_DIVMANTISSA_MASK)

/*!
 * @brief UART_FRA Register Bit Definition
 */

#define UART_FRA_DIVFRACTION_SHIFT              (0)
#define UART_FRA_DIVFRACTION_MASK               (0xFU << UART_FRA_DIVFRACTION_SHIFT)
#define UART_FRA_DIVFRACTION(x)                 (((uint32_t)(((uint32_t)(x)) << UART_FRA_DIVFRACTION_SHIFT)) & UART_FRA_DIVFRACTION_MASK)

/*!
 * @brief UART_RXADDR Register Bit Definition
 */

#define UART_RXADDR_RXADDR_SHIFT                (0)
#define UART_RXADDR_RXADDR_MASK                 (0xFFU << UART_RXADDR_RXADDR_SHIFT)
#define UART_RXADDR_RXADDR(x)                   (((uint32_t)(((uint32_t)(x)) << UART_RXADDR_RXADDR_SHIFT)) & UART_RXADDR_RXADDR_MASK)

/*!
 * @brief UART_RXMASK Register Bit Definition
 */

#define UART_RXMASK_RXMASK_SHIFT                (0)
#define UART_RXMASK_RXMASK_MASK                 (0xFFU << UART_RXMASK_RXMASK_SHIFT)
#define UART_RXMASK_RXMASK(x)                   (((uint32_t)(((uint32_t)(x)) << UART_RXMASK_RXMASK_SHIFT)) & UART_RXMASK_RXMASK_MASK)

/*!
 * @brief UART_SCR Register Bit Definition
 */

#define UART_SCR_HDSEL_SHIFT                    (12)
#define UART_SCR_HDSEL_MASK                     (0x01U << UART_SCR_HDSEL_SHIFT)
#define UART_SCR_HDSEL(x)                       (((uint32_t)(((uint32_t)(x)) << UART_SCR_HDSEL_SHIFT)) & UART_SCR_HDSEL_MASK)

#define UART_SCR_SCFCNT_SHIFT                   (4)
#define UART_SCR_SCFCNT_MASK                    (0xFFU << UART_SCR_SCFCNT_SHIFT)
#define UART_SCR_SCFCNT(x)                      (((uint32_t)(((uint32_t)(x)) << UART_SCR_SCFCNT_SHIFT)) & UART_SCR_SCFCNT_MASK)

#define UART_SCR_NACK_SHIFT                     (2)
#define UART_SCR_NACK_MASK                      (0x01U << UART_SCR_NACK_SHIFT)
#define UART_SCR_NACK(x)                        (((uint32_t)(((uint32_t)(x)) << UART_SCR_NACK_SHIFT)) & UART_SCR_NACK_MASK)

#define UART_SCR_SCAEN_SHIFT                    (1)
#define UART_SCR_SCAEN_MASK                     (0x01U << UART_SCR_SCAEN_SHIFT)
#define UART_SCR_SCAEN(x)                       (((uint32_t)(((uint32_t)(x)) << UART_SCR_SCAEN_SHIFT)) & UART_SCR_SCAEN_MASK)

#define UART_SCR_SCEN_SHIFT                     (0)
#define UART_SCR_SCEN_MASK                      (0x01U << UART_SCR_SCEN_SHIFT)
#define UART_SCR_SCEN(x)                        (((uint32_t)(((uint32_t)(x)) << UART_SCR_SCEN_SHIFT)) & UART_SCR_SCEN_MASK)

/*!
 * @brief UART_IDLR Register Bit Definition
 */

#define UART_IDLR_IDLR_SHIFT                    (0)
#define UART_IDLR_IDLR_MASK                     (0xFFFFU << UART_IDLR_IDLR_SHIFT)
#define UART_IDLR_IDLR(x)                       (((uint32_t)(((uint32_t)(x)) << UART_IDLR_IDLR_SHIFT)) & UART_IDLR_IDLR_MASK)

/*!
 * @brief UART_ABRCR Register Bit Definition
 */

#define UART_ABRCR_LATTEREDGE_SHIFT             (4)
#define UART_ABRCR_LATTEREDGE_MASK              (0x01U << UART_ABRCR_LATTEREDGE_SHIFT)
#define UART_ABRCR_LATTEREDGE(x)                (((uint32_t)(((uint32_t)(x)) << UART_ABRCR_LATTEREDGE_SHIFT)) & UART_ABRCR_LATTEREDGE_MASK)

#define UART_ABRCR_FORMEREDGE_SHIFT             (3)
#define UART_ABRCR_FORMEREDGE_MASK              (0x01U << UART_ABRCR_FORMEREDGE_SHIFT)
#define UART_ABRCR_FORMEREDGE(x)                (((uint32_t)(((uint32_t)(x)) << UART_ABRCR_FORMEREDGE_SHIFT)) & UART_ABRCR_FORMEREDGE_MASK)

#define UART_ABRCR_ABRBITCNT_SHIFT              (1)
#define UART_ABRCR_ABRBITCNT_MASK               (0x3U << UART_ABRCR_ABRBITCNT_SHIFT)
#define UART_ABRCR_ABRBITCNT(x)                 (((uint32_t)(((uint32_t)(x)) << UART_ABRCR_ABRBITCNT_SHIFT)) & UART_ABRCR_ABRBITCNT_MASK)

#define UART_ABRCR_ABREN_SHIFT                  (0)
#define UART_ABRCR_ABREN_MASK                   (0x01U << UART_ABRCR_ABREN_SHIFT)
#define UART_ABRCR_ABREN(x)                     (((uint32_t)(((uint32_t)(x)) << UART_ABRCR_ABREN_SHIFT)) & UART_ABRCR_ABREN_MASK)

/*!
 * @brief UART_IRDA Register Bit Definition
 */

#define UART_IRDA_PSCREG_SHIFT                  (8)
#define UART_IRDA_PSCREG_MASK                   (0xFFU << UART_IRDA_PSCREG_SHIFT)
#define UART_IRDA_PSCREG(x)                     (((uint32_t)(((uint32_t)(x)) << UART_IRDA_PSCREG_SHIFT)) & UART_IRDA_PSCREG_MASK)

#define UART_IRDA_SIRLP_SHIFT                   (1)
#define UART_IRDA_SIRLP_MASK                    (0x01U << UART_IRDA_SIRLP_SHIFT)
#define UART_IRDA_SIRLP(x)                      (((uint32_t)(((uint32_t)(x)) << UART_IRDA_SIRLP_SHIFT)) & UART_IRDA_SIRLP_MASK)

#define UART_IRDA_SIREN_SHIFT                   (0)
#define UART_IRDA_SIREN_MASK                    (0x01U << UART_IRDA_SIREN_SHIFT)
#define UART_IRDA_SIREN(x)                      (((uint32_t)(((uint32_t)(x)) << UART_IRDA_SIREN_SHIFT)) & UART_IRDA_SIREN_MASK)

/*!
 * @}
 */ /* end of group UART_Register_Masks */
/******************************************************************************
 *UART Instance
*******************************************************************************/

#define UART1                 ((UART_Type*)UART1_BASE)
#define UART2                 ((UART_Type*)UART2_BASE)
#define UART3                 ((UART_Type*)UART3_BASE)
#define UART4                 ((UART_Type*)UART4_BASE)
#define UART5                 ((UART_Type*)UART5_BASE)
#define UART6                 ((UART_Type*)UART6_BASE)
#define UART7                 ((UART_Type*)UART7_BASE)

/*!
 * @}
 */ /* end of group UART_Peripheral_Access_Layer */

/*!
 * @addtogroup USB_Peripheral_Access_Layer USB Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * USB Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t Reserved55[4];                                                   ///< Reserved
    __IO uint32_t FSOTGISTAT;                                                     ///< OTG Interrupt Status Register                offset: 0x10
    __IO uint32_t FSOTGICTRL;                                                     ///< OTG Interrupt Control Register               offset: 0x14
    __IO uint32_t FSOTGSTAT;                                                      ///< OTG Status Register                          offset: 0x18
    __IO uint32_t FSOTGCTRL;                                                      ///< OTG Control Register                         offset: 0x1C
    __IO uint32_t Reserved56[24];                                                  ///< Reserved
    __IO uint32_t FSINTSTAT;                                                      ///< Interrupt Status Register                    offset: 0x80
    __IO uint32_t FSINTENB;                                                       ///< Interrupt Enable Register                    offset: 0x84
    __IO uint32_t FSERRSTAT;                                                      ///< Error Interrupt Status Register              offset: 0x88
    __IO uint32_t FSERRENB;                                                       ///< Error Interrupt Enable Register              offset: 0x8C
    __IO uint32_t FSSTAT;                                                         ///< Status Register                              offset: 0x90
    __IO uint32_t FSCTL;                                                          ///< Control Register                             offset: 0x94
    __IO uint32_t FSADDR;                                                         ///< Address Register                             offset: 0x98
    __IO uint32_t FSBDTPAGE1;                                                     ///< BDT Page Register 1                          offset: 0x9C
    __IO uint32_t FSFRMNUML;                                                      ///< Frame Number Register                        offset: 0xA0
    __IO uint32_t FSFRMNUMH;                                                      ///< Frame Number Register                        offset: 0xA4
    __IO uint32_t FSTOKEN;                                                        ///< Token Register                               offset: 0xA8
    __IO uint32_t FSSOFTHLD;                                                      ///< SOF Threshold Register                       offset: 0xAC
    __IO uint32_t FSBDTPAGE2;                                                     ///< BDT Page Register 2                          offset: 0xB0
    __IO uint32_t FSBDTPAGE3;                                                     ///< BDT Page Register 3                          offset: 0xB4
    __IO uint32_t Reserved57[2];                                                   ///< Reserved
    __IO uint32_t FSEPCTL[16];                                                    ///< Endpoint control register 0 ~15            offset: 0xC0
    __IO uint32_t FSUSBCTRL;                                                      ///< USB control register                         offset 0x100
} USB_Type;

/*******************************************************************************
 * USB Type
 ******************************************************************************/

/*!
 * @addtogroup USB_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief USB_FS_OTGISTAT Register Bit Definition
 */

#define USB_FSOTGISTAT_IDCHG_SHIFT              (7)
#define USB_FSOTGISTAT_IDCHG_MASK               (0x01U << USB_FSOTGISTAT_IDCHG_SHIFT)
#define USB_FSOTGISTAT_IDCHG(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSOTGISTAT_IDCHG_SHIFT)) & USB_FSOTGISTAT_IDCHG_MASK)

#define USB_FSOTGISTAT_1MSEC_SHIFT              (6)
#define USB_FSOTGISTAT_1MSEC_MASK               (0x01U << USB_FSOTGISTAT_1MSEC_SHIFT)
#define USB_FSOTGISTAT_1MSEC(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSOTGISTAT_1MSEC_SHIFT)) & USB_FSOTGISTAT_1MSEC_MASK)

#define USB_FSOTGISTAT_LINESTATECHG_SHIFT       (5)
#define USB_FSOTGISTAT_LINESTATECHG_MASK        (0x01U << USB_FSOTGISTAT_LINESTATECHG_SHIFT)
#define USB_FSOTGISTAT_LINESTATECHG(x)          (((uint32_t)(((uint32_t)(x)) << USB_FSOTGISTAT_LINESTATECHG_SHIFT)) & USB_FSOTGISTAT_LINESTATECHG_MASK)

#define USB_FSOTGISTAT_SESSVLDCHG_SHIFT         (2)
#define USB_FSOTGISTAT_SESSVLDCHG_MASK          (0x01U << USB_FSOTGISTAT_SESSVLDCHG_SHIFT)
#define USB_FSOTGISTAT_SESSVLDCHG(x)            (((uint32_t)(((uint32_t)(x)) << USB_FSOTGISTAT_SESSVLDCHG_SHIFT)) & USB_FSOTGISTAT_SESSVLDCHG_MASK)

#define USB_FSOTGISTAT_BSESSENDCHG_SHIFT        (2)
#define USB_FSOTGISTAT_BSESSENDCHG_MASK         (0x01U << USB_FSOTGISTAT_BSESSENDCHG_SHIFT)
#define USB_FSOTGISTAT_BSESSENDCHG(x)           (((uint32_t)(((uint32_t)(x)) << USB_FSOTGISTAT_BSESSENDCHG_SHIFT)) & USB_FSOTGISTAT_BSESSENDCHG_MASK)

#define USB_FSOTGISTAT_AVBUSVLDCHG_SHIFT        (0)
#define USB_FSOTGISTAT_AVBUSVLDCHG_MASK         (0x01U << USB_FSOTGISTAT_AVBUSVLDCHG_SHIFT)
#define USB_FSOTGISTAT_AVBUSVLDCHG(x)           (((uint32_t)(((uint32_t)(x)) << USB_FSOTGISTAT_AVBUSVLDCHG_SHIFT)) & USB_FSOTGISTAT_AVBUSVLDCHG_MASK)

/*!
 * @brief USB_FS_OTGICTRL Register Bit Definition
 */

#define USB_FSOTGICTRL_IDEN_SHIFT               (7)
#define USB_FSOTGICTRL_IDEN_MASK                (0x01U << USB_FSOTGICTRL_IDEN_SHIFT)
#define USB_FSOTGICTRL_IDEN(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSOTGICTRL_IDEN_SHIFT)) & USB_FSOTGICTRL_IDEN_MASK)

#define USB_FSOTGICTRL_1MSECEN_SHIFT            (6)
#define USB_FSOTGICTRL_1MSECEN_MASK             (0x01U << USB_FSOTGICTRL_1MSECEN_SHIFT)
#define USB_FSOTGICTRL_1MSECEN(x)               (((uint32_t)(((uint32_t)(x)) << USB_FSOTGICTRL_1MSECEN_SHIFT)) & USB_FSOTGICTRL_1MSECEN_MASK)

#define USB_FSOTGICTRL_LINESTATEEN_SHIFT        (5)
#define USB_FSOTGICTRL_LINESTATEEN_MASK         (0x01U << USB_FSOTGICTRL_LINESTATEEN_SHIFT)
#define USB_FSOTGICTRL_LINESTATEEN(x)           (((uint32_t)(((uint32_t)(x)) << USB_FSOTGICTRL_LINESTATEEN_SHIFT)) & USB_FSOTGICTRL_LINESTATEEN_MASK)

#define USB_FSOTGICTRL_SESSVLDEN_SHIFT          (3)
#define USB_FSOTGICTRL_SESSVLDEN_MASK           (0x01U << USB_FSOTGICTRL_SESSVLDEN_SHIFT)
#define USB_FSOTGICTRL_SESSVLDEN(x)             (((uint32_t)(((uint32_t)(x)) << USB_FSOTGICTRL_SESSVLDEN_SHIFT)) & USB_FSOTGICTRL_SESSVLDEN_MASK)

#define USB_FSOTGICTRL_BSESSENDEN_SHIFT         (2)
#define USB_FSOTGICTRL_BSESSENDEN_MASK          (0x01U << USB_FSOTGICTRL_BSESSENDEN_SHIFT)
#define USB_FSOTGICTRL_BSESSENDEN(x)            (((uint32_t)(((uint32_t)(x)) << USB_FSOTGICTRL_BSESSENDEN_SHIFT)) & USB_FSOTGICTRL_BSESSENDEN_MASK)

#define USB_FSOTGICTRL_AVBUSVLDEN_SHIFT         (0)
#define USB_FSOTGICTRL_AVBUSVLDEN_MASK          (0x01U << USB_FSOTGICTRL_AVBUSVLDEN_SHIFT)
#define USB_FSOTGICTRL_AVBUSVLDEN(x)            (((uint32_t)(((uint32_t)(x)) << USB_FSOTGICTRL_AVBUSVLDEN_SHIFT)) & USB_FSOTGICTRL_AVBUSVLDEN_MASK)

/*!
 * @brief USB_FS_OTGSTAT Register Bit Definition
 */

#define USB_FSOTGSTAT_ID_SHIFT                  (7)
#define USB_FSOTGSTAT_ID_MASK                   (0x01U << USB_FSOTGSTAT_ID_SHIFT)
#define USB_FSOTGSTAT_ID(x)                     (((uint32_t)(((uint32_t)(x)) << USB_FSOTGSTAT_ID_SHIFT)) & USB_FSOTGSTAT_ID_MASK)

#define USB_FSOTGSTAT_LINESTATESTABLE_SHIFT     (5)
#define USB_FSOTGSTAT_LINESTATESTABLE_MASK      (0x01U << USB_FSOTGSTAT_LINESTATESTABLE_SHIFT)
#define USB_FSOTGSTAT_LINESTATESTABLE(x)        (((uint32_t)(((uint32_t)(x)) << USB_FSOTGSTAT_LINESTATESTABLE_SHIFT)) & USB_FSOTGSTAT_LINESTATESTABLE_MASK)

#define USB_FSOTGSTAT_SESSVLD_SHIFT             (3)
#define USB_FSOTGSTAT_SESSVLD_MASK              (0x01U << USB_FSOTGSTAT_SESSVLD_SHIFT)
#define USB_FSOTGSTAT_SESSVLD(x)                (((uint32_t)(((uint32_t)(x)) << USB_FSOTGSTAT_SESSVLD_SHIFT)) & USB_FSOTGSTAT_SESSVLD_MASK)

#define USB_FSOTGSTAT_BSESSEND_SHIFT            (2)
#define USB_FSOTGSTAT_BSESSEND_MASK             (0x01U << USB_FSOTGSTAT_BSESSEND_SHIFT)
#define USB_FSOTGSTAT_BSESSEND(x)               (((uint32_t)(((uint32_t)(x)) << USB_FSOTGSTAT_BSESSEND_SHIFT)) & USB_FSOTGSTAT_BSESSEND_MASK)

#define USB_FSOTGSTAT_AVBUSVLD_SHIFT            (0)
#define USB_FSOTGSTAT_AVBUSVLD_MASK             (0x01U << USB_FSOTGSTAT_AVBUSVLD_SHIFT)
#define USB_FSOTGSTAT_AVBUSVLD(x)               (((uint32_t)(((uint32_t)(x)) << USB_FSOTGSTAT_AVBUSVLD_SHIFT)) & USB_FSOTGSTAT_AVBUSVLD_MASK)

/*!
 * @brief USB_FS_OTGCTRL Register Bit Definition
 */

#define USB_FSOTGCTRL_DPHIGH_SHIFT              (7)
#define USB_FSOTGCTRL_DPHIGH_MASK               (0x01U << USB_FSOTGCTRL_DPHIGH_SHIFT)
#define USB_FSOTGCTRL_DPHIGH(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSOTGCTRL_DPHIGH_SHIFT)) & USB_FSOTGCTRL_DPHIGH_MASK)

#define USB_FSOTGCTRL_DMHIGH_SHIFT              (6)
#define USB_FSOTGCTRL_DMHIGH_MASK               (0x01U << USB_FSOTGCTRL_DMHIGH_SHIFT)
#define USB_FSOTGCTRL_DMHIGH(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSOTGCTRL_DMHIGH_SHIFT)) & USB_FSOTGCTRL_DMHIGH_MASK)

#define USB_FSOTGCTRL_DPLOW_SHIFT               (5)
#define USB_FSOTGCTRL_DPLOW_MASK                (0x01U << USB_FSOTGCTRL_DPLOW_SHIFT)
#define USB_FSOTGCTRL_DPLOW(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSOTGCTRL_DPLOW_SHIFT)) & USB_FSOTGCTRL_DPLOW_MASK)

#define USB_FSOTGCTRL_DMLOW_SHIFT               (4)
#define USB_FSOTGCTRL_DMLOW_MASK                (0x01U << USB_FSOTGCTRL_DMLOW_SHIFT)
#define USB_FSOTGCTRL_DMLOW(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSOTGCTRL_DMLOW_SHIFT)) & USB_FSOTGCTRL_DMLOW_MASK)

#define USB_FSOTGCTRL_VBUSON_SHIFT              (3)
#define USB_FSOTGCTRL_VBUSON_MASK               (0x01U << USB_FSOTGCTRL_VBUSON_SHIFT)
#define USB_FSOTGCTRL_VBUSON(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSOTGCTRL_VBUSON_SHIFT)) & USB_FSOTGCTRL_VBUSON_MASK)

#define USB_FSOTGCTRL_OTGEN_SHIFT               (2)
#define USB_FSOTGCTRL_OTGEN_MASK                (0x01U << USB_FSOTGCTRL_OTGEN_SHIFT)
#define USB_FSOTGCTRL_OTGEN(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSOTGCTRL_OTGEN_SHIFT)) & USB_FSOTGCTRL_OTGEN_MASK)

#define USB_FSOTGCTRL_VBUSCHG_SHIFT             (1)
#define USB_FSOTGCTRL_VBUSCHG_MASK              (0x01U << USB_FSOTGCTRL_VBUSCHG_SHIFT)
#define USB_FSOTGCTRL_VBUSCHG(x)                (((uint32_t)(((uint32_t)(x)) << USB_FSOTGCTRL_VBUSCHG_SHIFT)) & USB_FSOTGCTRL_VBUSCHG_MASK)

#define USB_FSOTGCTRL_VBUSDSCHG_SHIFT           (0)
#define USB_FSOTGCTRL_VBUSDSCHG_MASK            (0x01U << USB_FSOTGCTRL_VBUSDSCHG_SHIFT)
#define USB_FSOTGCTRL_VBUSDSCHG(x)              (((uint32_t)(((uint32_t)(x)) << USB_FSOTGCTRL_VBUSDSCHG_SHIFT)) & USB_FSOTGCTRL_VBUSDSCHG_MASK)

/*!
 * @brief USB_FS_INTSTAT Register Bit Definition
 */

#define USB_FSINTSTAT_STALL_SHIFT               (7)
#define USB_FSINTSTAT_STALL_MASK                (0x01U << USB_FSINTSTAT_STALL_SHIFT)
#define USB_FSINTSTAT_STALL(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSINTSTAT_STALL_SHIFT)) & USB_FSINTSTAT_STALL_MASK)

#define USB_FSINTSTAT_ATTACH_SHIFT              (6)
#define USB_FSINTSTAT_ATTACH_MASK               (0x01U << USB_FSINTSTAT_ATTACH_SHIFT)
#define USB_FSINTSTAT_ATTACH(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSINTSTAT_ATTACH_SHIFT)) & USB_FSINTSTAT_ATTACH_MASK)

#define USB_FSINTSTAT_RESUME_SHIFT              (5)
#define USB_FSINTSTAT_RESUME_MASK               (0x01U << USB_FSINTSTAT_RESUME_SHIFT)
#define USB_FSINTSTAT_RESUME(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSINTSTAT_RESUME_SHIFT)) & USB_FSINTSTAT_RESUME_MASK)

#define USB_FSINTSTAT_SLEEP_SHIFT               (4)
#define USB_FSINTSTAT_SLEEP_MASK                (0x01U << USB_FSINTSTAT_SLEEP_SHIFT)
#define USB_FSINTSTAT_SLEEP(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSINTSTAT_SLEEP_SHIFT)) & USB_FSINTSTAT_SLEEP_MASK)

#define USB_FSINTSTAT_TOKDNE_SHIFT              (3)
#define USB_FSINTSTAT_TOKDNE_MASK               (0x01U << USB_FSINTSTAT_TOKDNE_SHIFT)
#define USB_FSINTSTAT_TOKDNE(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSINTSTAT_TOKDNE_SHIFT)) & USB_FSINTSTAT_TOKDNE_MASK)

#define USB_FSINTSTAT_SOFTOK_SHIFT              (2)
#define USB_FSINTSTAT_SOFTOK_MASK               (0x01U << USB_FSINTSTAT_SOFTOK_SHIFT)
#define USB_FSINTSTAT_SOFTOK(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSINTSTAT_SOFTOK_SHIFT)) & USB_FSINTSTAT_SOFTOK_MASK)

#define USB_FSINTSTAT_ERROR_SHIFT               (1)
#define USB_FSINTSTAT_ERROR_MASK                (0x01U << USB_FSINTSTAT_ERROR_SHIFT)
#define USB_FSINTSTAT_ERROR(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSINTSTAT_ERROR_SHIFT)) & USB_FSINTSTAT_ERROR_MASK)

#define USB_FSINTSTAT_USBRST_SHIFT              (0)
#define USB_FSINTSTAT_USBRST_MASK               (0x01U << USB_FSINTSTAT_USBRST_SHIFT)
#define USB_FSINTSTAT_USBRST(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSINTSTAT_USBRST_SHIFT)) & USB_FSINTSTAT_USBRST_MASK)

/*!
 * @brief USB_FS_INTENB Register Bit Definition
 */

#define USB_FSINTENB_STALL_SHIFT                (7)
#define USB_FSINTENB_STALL_MASK                 (0x01U << USB_FSINTENB_STALL_SHIFT)
#define USB_FSINTENB_STALL(x)                   (((uint32_t)(((uint32_t)(x)) << USB_FSINTENB_STALL_SHIFT)) & USB_FSINTENB_STALL_MASK)

#define USB_FSINTENB_ATTACH_SHIFT               (6)
#define USB_FSINTENB_ATTACH_MASK                (0x01U << USB_FSINTENB_ATTACH_SHIFT)
#define USB_FSINTENB_ATTACH(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSINTENB_ATTACH_SHIFT)) & USB_FSINTENB_ATTACH_MASK)

#define USB_FSINTENB_RESUME_SHIFT               (5)
#define USB_FSINTENB_RESUME_MASK                (0x01U << USB_FSINTENB_RESUME_SHIFT)
#define USB_FSINTENB_RESUME(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSINTENB_RESUME_SHIFT)) & USB_FSINTENB_RESUME_MASK)

#define USB_FSINTENB_SLEEP_SHIFT                (4)
#define USB_FSINTENB_SLEEP_MASK                 (0x01U << USB_FSINTENB_SLEEP_SHIFT)
#define USB_FSINTENB_SLEEP(x)                   (((uint32_t)(((uint32_t)(x)) << USB_FSINTENB_SLEEP_SHIFT)) & USB_FSINTENB_SLEEP_MASK)

#define USB_FSINTENB_TOKDNE_SHIFT               (3)
#define USB_FSINTENB_TOKDNE_MASK                (0x01U << USB_FSINTENB_TOKDNE_SHIFT)
#define USB_FSINTENB_TOKDNE(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSINTENB_TOKDNE_SHIFT)) & USB_FSINTENB_TOKDNE_MASK)

#define USB_FSINTENB_SOFTOK_SHIFT               (2)
#define USB_FSINTENB_SOFTOK_MASK                (0x01U << USB_FSINTENB_SOFTOK_SHIFT)
#define USB_FSINTENB_SOFTOK(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSINTENB_SOFTOK_SHIFT)) & USB_FSINTENB_SOFTOK_MASK)

#define USB_FSINTENB_ERROR_SHIFT                (1)
#define USB_FSINTENB_ERROR_MASK                 (0x01U << USB_FSINTENB_ERROR_SHIFT)
#define USB_FSINTENB_ERROR(x)                   (((uint32_t)(((uint32_t)(x)) << USB_FSINTENB_ERROR_SHIFT)) & USB_FSINTENB_ERROR_MASK)

#define USB_FSINTENB_USBRST_SHIFT               (0)
#define USB_FSINTENB_USBRST_MASK                (0x01U << USB_FSINTENB_USBRST_SHIFT)
#define USB_FSINTENB_USBRST(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSINTENB_USBRST_SHIFT)) & USB_FSINTENB_USBRST_MASK)

/*!
 * @brief USB_FS_ERRSTAT Register Bit Definition
 */

#define USB_FSERRSTAT_BTSERR_SHIFT              (7)
#define USB_FSERRSTAT_BTSERR_MASK               (0x01U << USB_FSERRSTAT_BTSERR_SHIFT)
#define USB_FSERRSTAT_BTSERR(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSERRSTAT_BTSERR_SHIFT)) & USB_FSERRSTAT_BTSERR_MASK)

#define USB_FSERRSTAT_DMAERR_SHIFT              (5)
#define USB_FSERRSTAT_DMAERR_MASK               (0x01U << USB_FSERRSTAT_DMAERR_SHIFT)
#define USB_FSERRSTAT_DMAERR(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSERRSTAT_DMAERR_SHIFT)) & USB_FSERRSTAT_DMAERR_MASK)

#define USB_FSERRSTAT_BTOERR_SHIFT              (4)
#define USB_FSERRSTAT_BTOERR_MASK               (0x01U << USB_FSERRSTAT_BTOERR_SHIFT)
#define USB_FSERRSTAT_BTOERR(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSERRSTAT_BTOERR_SHIFT)) & USB_FSERRSTAT_BTOERR_MASK)

#define USB_FSERRSTAT_DFN8_SHIFT                (3)
#define USB_FSERRSTAT_DFN8_MASK                 (0x01U << USB_FSERRSTAT_DFN8_SHIFT)
#define USB_FSERRSTAT_DFN8(x)                   (((uint32_t)(((uint32_t)(x)) << USB_FSERRSTAT_DFN8_SHIFT)) & USB_FSERRSTAT_DFN8_MASK)

#define USB_FSERRSTAT_CRC16_SHIFT               (2)
#define USB_FSERRSTAT_CRC16_MASK                (0x01U << USB_FSERRSTAT_CRC16_SHIFT)
#define USB_FSERRSTAT_CRC16(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSERRSTAT_CRC16_SHIFT)) & USB_FSERRSTAT_CRC16_MASK)

#define USB_FSERRSTAT_CRC5EOF_SHIFT            (1)
#define USB_FSERRSTAT_CRC5EOF_MASK             (0x01U << USB_FSERRSTAT_CRC5EOF_SHIFT)
#define USB_FSERRSTAT_CRC5EOF(x)               (((uint32_t)(((uint32_t)(x)) << USB_FSERRSTAT_CRC5EOF_SHIFT)) & USB_FSERRSTAT_CRC5EOF_MASK)

#define USB_FSERRSTAT_PIDERR_SHIFT              (0)
#define USB_FSERRSTAT_PIDERR_MASK               (0x01U << USB_FSERRSTAT_PIDERR_SHIFT)
#define USB_FSERRSTAT_PIDERR(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSERRSTAT_PIDERR_SHIFT)) & USB_FSERRSTAT_PIDERR_MASK)

/*!
 * @brief USB_FS_ERRENB Register Bit Definition
 */

#define USB_FSERRENB_BTSERR_SHIFT               (7)
#define USB_FSERRENB_BTSERR_MASK                (0x01U << USB_FSERRENB_BTSERR_SHIFT)
#define USB_FSERRENB_BTSERR(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSERRENB_BTSERR_SHIFT)) & USB_FSERRENB_BTSERR_MASK)

#define USB_FSERRENB_DMAERR_SHIFT               (5)
#define USB_FSERRENB_DMAERR_MASK                (0x01U << USB_FSERRENB_DMAERR_SHIFT)
#define USB_FSERRENB_DMAERR(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSERRENB_DMAERR_SHIFT)) & USB_FSERRENB_DMAERR_MASK)

#define USB_FSERRENB_BTOERR_SHIFT               (4)
#define USB_FSERRENB_BTOERR_MASK                (0x01U << USB_FSERRENB_BTOERR_SHIFT)
#define USB_FSERRENB_BTOERR(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSERRENB_BTOERR_SHIFT)) & USB_FSERRENB_BTOERR_MASK)

#define USB_FSERRENB_DFN8_SHIFT                 (3)
#define USB_FSERRENB_DFN8_MASK                  (0x01U << USB_FSERRENB_DFN8_SHIFT)
#define USB_FSERRENB_DFN8(x)                    (((uint32_t)(((uint32_t)(x)) << USB_FSERRENB_DFN8_SHIFT)) & USB_FSERRENB_DFN8_MASK)

#define USB_FSERRENB_CRC16_SHIFT                (2)
#define USB_FSERRENB_CRC16_MASK                 (0x01U << USB_FSERRENB_CRC16_SHIFT)
#define USB_FSERRENB_CRC16(x)                   (((uint32_t)(((uint32_t)(x)) << USB_FSERRENB_CRC16_SHIFT)) & USB_FSERRENB_CRC16_MASK)

#define USB_FSERRENB_CRC5EOF_SHIFT             (1)
#define USB_FSERRENB_CRC5EOF_MASK              (0x01U << USB_FSERRENB_CRC5EOF_SHIFT)
#define USB_FSERRENB_CRC5EOF(x)                (((uint32_t)(((uint32_t)(x)) << USB_FSERRENB_CRC5EOF_SHIFT)) & USB_FSERRENB_CRC5EOF_MASK)

#define USB_FSERRENB_PIDERR_SHIFT               (0)
#define USB_FSERRENB_PIDERR_MASK                (0x01U << USB_FSERRENB_PIDERR_SHIFT)
#define USB_FSERRENB_PIDERR(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSERRENB_PIDERR_SHIFT)) & USB_FSERRENB_PIDERR_MASK)

/*!
 * @brief USB_FS_STAT Register Bit Definition
 */

#define USB_FSSTAT_ENDP_SHIFT                   (4)
#define USB_FSSTAT_ENDP_MASK                    (0xFU << USB_FSSTAT_ENDP_SHIFT)
#define USB_FSSTAT_ENDP(x)                      (((uint32_t)(((uint32_t)(x)) << USB_FSSTAT_ENDP_SHIFT)) & USB_FSSTAT_ENDP_MASK)

#define USB_FSSTAT_TX_SHIFT                     (3)
#define USB_FSSTAT_TX_MASK                      (0x01U << USB_FSSTAT_TX_SHIFT)
#define USB_FSSTAT_TX(x)                        (((uint32_t)(((uint32_t)(x)) << USB_FSSTAT_TX_SHIFT)) & USB_FSSTAT_TX_MASK)

#define USB_FSSTAT_ODD_SHIFT                    (2)
#define USB_FSSTAT_ODD_MASK                     (0x01U << USB_FSSTAT_ODD_SHIFT)
#define USB_FSSTAT_ODD(x)                       (((uint32_t)(((uint32_t)(x)) << USB_FSSTAT_ODD_SHIFT)) & USB_FSSTAT_ODD_MASK)

/*!
 * @brief USB_FS_CTL Register Bit Definition
 */

#define USB_FSCTL_JSTATE_SHIFT                  (7)
#define USB_FSCTL_JSTATE_MASK                   (0x01U << USB_FSCTL_JSTATE_SHIFT)
#define USB_FSCTL_JSTATE(x)                     (((uint32_t)(((uint32_t)(x)) << USB_FSCTL_JSTATE_SHIFT)) & USB_FSCTL_JSTATE_MASK)

#define USB_FSCTL_SE0_SHIFT                     (6)
#define USB_FSCTL_SE0_MASK                      (0x01U << USB_FSCTL_SE0_SHIFT)
#define USB_FSCTL_SE0(x)                        (((uint32_t)(((uint32_t)(x)) << USB_FSCTL_SE0_SHIFT)) & USB_FSCTL_SE0_MASK)

#define USB_FSCTL_TXDSUSPENDTOKENBUSY_SHIFT     (5)
#define USB_FSCTL_TXDSUSPENDTOKENBUSY_MASK      (0x01U << USB_FSCTL_TXDSUSPENDTOKENBUSY_SHIFT)
#define USB_FSCTL_TXDSUSPENDTOKENBUSY(x)        (((uint32_t)(((uint32_t)(x)) << USB_FSCTL_TXDSUSPENDTOKENBUSY_SHIFT)) & USB_FSCTL_TXDSUSPENDTOKENBUSY_MASK)

#define USB_FSCTL_RESET_SHIFT                   (4)
#define USB_FSCTL_RESET_MASK                    (0x01U << USB_FSCTL_RESET_SHIFT)
#define USB_FSCTL_RESET(x)                      (((uint32_t)(((uint32_t)(x)) << USB_FSCTL_RESET_SHIFT)) & USB_FSCTL_RESET_MASK)

#define USB_FSCTL_HOSTMODEEN_SHIFT              (3)
#define USB_FSCTL_HOSTMODEEN_MASK               (0x01U << USB_FSCTL_HOSTMODEEN_SHIFT)
#define USB_FSCTL_HOSTMODEEN(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSCTL_HOSTMODEEN_SHIFT)) & USB_FSCTL_HOSTMODEEN_MASK)

#define USB_FSCTL_RESUME_SHIFT                  (2)
#define USB_FSCTL_RESUME_MASK                   (0x01U << USB_FSCTL_RESUME_SHIFT)
#define USB_FSCTL_RESUME(x)                     (((uint32_t)(((uint32_t)(x)) << USB_FSCTL_RESUME_SHIFT)) & USB_FSCTL_RESUME_MASK)

#define USB_FSCTL_ODDRST_SHIFT                  (1)
#define USB_FSCTL_ODDRST_MASK                   (0x01U << USB_FSCTL_ODDRST_SHIFT)
#define USB_FSCTL_ODDRST(x)                     (((uint32_t)(((uint32_t)(x)) << USB_FSCTL_ODDRST_SHIFT)) & USB_FSCTL_ODDRST_MASK)

#define USB_FSCTL_USBEN_SHIFT                   (0)
#define USB_FSCTL_USBEN_MASK                    (0x01U << USB_FSCTL_USBEN_SHIFT)
#define USB_FSCTL_USBEN(x)                      (((uint32_t)(((uint32_t)(x)) << USB_FSCTL_USBEN_SHIFT)) & USB_FSCTL_USBEN_MASK)

/*!
 * @brief USB_FS_ADDR Register Bit Definition
 */

#define USB_FSADDR_LSEN_SHIFT                   (7)
#define USB_FSADDR_LSEN_MASK                    (0x01U << USB_FSADDR_LSEN_SHIFT)
#define USB_FSADDR_LSEN(x)                      (((uint32_t)(((uint32_t)(x)) << USB_FSADDR_LSEN_SHIFT)) & USB_FSADDR_LSEN_MASK)

#define USB_FSADDR_ADDR_SHIFT                   (0)
#define USB_FSADDR_ADDR_MASK                    (0x7FU << USB_FSADDR_ADDR_SHIFT)
#define USB_FSADDR_ADDR(x)                      (((uint32_t)(((uint32_t)(x)) << USB_FSADDR_ADDR_SHIFT)) & USB_FSADDR_ADDR_MASK)

/*!
 * @brief USB_FS_BDTPAGE1 Register Bit Definition
 */

#define USB_FSBDTPAGE1_BDTBA_SHIFT              (1)
#define USB_FSBDTPAGE1_BDTBA_MASK               (0x7FU << USB_FSBDTPAGE1_BDTBA_SHIFT)
#define USB_FSBDTPAGE1_BDTBA(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSBDTPAGE1_BDTBA_SHIFT)) & USB_FSBDTPAGE1_BDTBA_MASK)

/*!
 * @brief USB_FS_FRMNUML Register Bit Definition
 */

#define USB_FSFRMNUML_FRM_SHIFT                 (0)
#define USB_FSFRMNUML_FRM_MASK                  (0xFFU << USB_FSFRMNUML_FRM_SHIFT)
#define USB_FSFRMNUML_FRM(x)                    (((uint32_t)(((uint32_t)(x)) << USB_FSFRMNUML_FRM_SHIFT)) & USB_FSFRMNUML_FRM_MASK)

/*!
 * @brief USB_FS_FRMNUMH Register Bit Definition
 */

#define USB_FSFRMNUMH_FRM_SHIFT                 (0)
#define USB_FSFRMNUMH_FRM_MASK                  (0x7U << USB_FSFRMNUMH_FRM_SHIFT)
#define USB_FSFRMNUMH_FRM(x)                    (((uint32_t)(((uint32_t)(x)) << USB_FSFRMNUMH_FRM_SHIFT)) & USB_FSFRMNUMH_FRM_MASK)

/*!
 * @brief USB_FS_TOKEN Register Bit Definition
 */

#define USB_FSTOKEN_TOKENPID_SHIFT              (4)
#define USB_FSTOKEN_TOKENPID_MASK               (0xFU << USB_FSTOKEN_TOKENPID_SHIFT)
#define USB_FSTOKEN_TOKENPID(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSTOKEN_TOKENPID_SHIFT)) & USB_FSTOKEN_TOKENPID_MASK)

#define USB_FSTOKEN_TOKENENDPT_SHIFT            (0)
#define USB_FSTOKEN_TOKENENDPT_MASK             (0xFU << USB_FSTOKEN_TOKENENDPT_SHIFT)
#define USB_FSTOKEN_TOKENENDPT(x)               (((uint32_t)(((uint32_t)(x)) << USB_FSTOKEN_TOKENENDPT_SHIFT)) & USB_FSTOKEN_TOKENENDPT_MASK)

/*!
 * @brief USB_FS_SOFTHLD Register Bit Definition
 */

#define USB_FSSOFTHLD_CNT_SHIFT                 (0)
#define USB_FSSOFTHLD_CNT_MASK                  (0xFFU << USB_FSSOFTHLD_CNT_SHIFT)
#define USB_FSSOFTHLD_CNT(x)                    (((uint32_t)(((uint32_t)(x)) << USB_FSSOFTHLD_CNT_SHIFT)) & USB_FSSOFTHLD_CNT_MASK)

/*!
 * @brief USB_FS_BDTPAGE2 Register Bit Definition
 */

#define USB_FSBDTPAGE2_BDTBA_SHIFT              (0)
#define USB_FSBDTPAGE2_BDTBA_MASK               (0xFFU << USB_FSBDTPAGE2_BDTBA_SHIFT)
#define USB_FSBDTPAGE2_BDTBA(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSBDTPAGE2_BDTBA_SHIFT)) & USB_FSBDTPAGE2_BDTBA_MASK)

/*!
 * @brief USB_FS_BDTPAGE3 Register Bit Definition
 */

#define USB_FSBDTPAGE3_BDTBA_SHIFT              (0)
#define USB_FSBDTPAGE3_BDTBA_MASK               (0xFFU << USB_FSBDTPAGE3_BDTBA_SHIFT)
#define USB_FSBDTPAGE3_BDTBA(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSBDTPAGE3_BDTBA_SHIFT)) & USB_FSBDTPAGE3_BDTBA_MASK)

/*!
 * @brief USB_FS_EPCTL0 Register Bit Definition
 */

#define USB_FSEPCTL_HOSTWOHUB_SHIFT            (7)
#define USB_FSEPCTL_HOSTWOHUB_MASK             (0x01U << USB_FSEPCTL_HOSTWOHUB_SHIFT)
#define USB_FSEPCTL_HOSTWOHUB(x)               (((uint32_t)(((uint32_t)(x)) << USB_FSEPCTL_HOSTWOHUB_SHIFT)) & USB_FSEPCTL_HOSTWOHUB_MASK)

#define USB_FSEPCTL_RETRYDIS_SHIFT             (6)
#define USB_FSEPCTL_RETRYDIS_MASK              (0x01U << USB_FSEPCTL_RETRYDIS_SHIFT)
#define USB_FSEPCTL_RETRYDIS(x)                (((uint32_t)(((uint32_t)(x)) << USB_FSEPCTL_RETRYDIS_SHIFT)) & USB_FSEPCTL_RETRYDIS_MASK)

#define USB_FSEPCTL_EPCTLDISEPRXENEPTXEN_SHIFT (2)
#define USB_FSEPCTL_EPCTLDISEPRXENEPTXEN_MASK  (0x7U << USB_FSEPCTL_EPCTLDISEPRXENEPTXEN_SHIFT)
#define USB_FSEPCTL_EPCTLDISEPRXENEPTXEN(x)    (((uint32_t)(((uint32_t)(x)) << USB_FSEPCTL_EPCTLDISEPRXENEPTXEN_SHIFT)) & USB_FSEPCTL_EPCTLDISEPRXENEPTXEN_MASK)

#define USB_FSEPCTL_EPSTALL_SHIFT              (1)
#define USB_FSEPCTL_EPSTALL_MASK               (0x01U << USB_FSEPCTL_EPSTALL_SHIFT)
#define USB_FSEPCTL_EPSTALL(x)                 (((uint32_t)(((uint32_t)(x)) << USB_FSEPCTL_EPSTALL_SHIFT)) & USB_FSEPCTL_EPSTALL_MASK)

#define USB_FSEPCTL_EPHSHK_SHIFT               (0)
#define USB_FSEPCTL_EPHSHK_MASK                (0x01U << USB_FSEPCTL_EPHSHK_SHIFT)
#define USB_FSEPCTL_EPHSHK(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSEPCTL_EPHSHK_SHIFT)) & USB_FSEPCTL_EPHSHK_MASK)
/*!
 * @brief USB_FS_USBCTRL Register Bit Definition
 */

#define USB_FSUSBCTRL_SUSPE_SHIFT               (7)
#define USB_FSUSBCTRL_SUSPE_MASK                (0x01U << USB_FSUSBCTRL_SUSPE_SHIFT)
#define USB_FSUSBCTRL_SUSPE(x)                  (((uint32_t)(((uint32_t)(x)) << USB_FSUSBCTRL_SUSPE_SHIFT)) & USB_FSUSBCTRL_SUSPE_MASK)

/*!
 * @}
 */ /* end of group USB_Register_Masks */
/******************************************************************************
 *USB Instance
*******************************************************************************/

#define USB                  ((USB_Type*)USB_BASE)

/*!
 * @}
 */ /* end of group USB_Peripheral_Access_Layer */

/*!
 * @addtogroup WWDG_Peripheral_Access_Layer WWDG Peripheral Access Layer
 *  @{
 */

/*******************************************************************************
 * WWDG Type
 ******************************************************************************/
typedef struct {
    __IO uint32_t CR;                                                             ///< control register                             offset: 0x00
    __IO uint32_t CFGR;                                                           ///< configuration register                       offset: 0x04
    __IO uint32_t SR;                                                             ///< status register                              offset: 0x08
} WWDG_Type;

/*******************************************************************************
 * WWDG Type
 ******************************************************************************/

/*!
 * @addtogroup WWDG_Register_Masks Register Masks
 * @{
 */

/*!
 * @brief WWDG_CR Register Bit Definition
 */

#define WWDG_CR_WDGA_SHIFT                      (7)
#define WWDG_CR_WDGA_MASK                       (0x01U << WWDG_CR_WDGA_SHIFT)
#define WWDG_CR_WDGA(x)                         (((uint32_t)(((uint32_t)(x)) << WWDG_CR_WDGA_SHIFT)) & WWDG_CR_WDGA_MASK)

#define WWDG_CR_T_SHIFT                         (0)
#define WWDG_CR_T_MASK                          (0x7FU << WWDG_CR_T_SHIFT)
#define WWDG_CR_T(x)                            (((uint32_t)(((uint32_t)(x)) << WWDG_CR_T_SHIFT)) & WWDG_CR_T_MASK)

/*!
 * @brief WWDG_CFGR Register Bit Definition
 */

#define WWDG_CFGR_EWI_SHIFT                     (9)
#define WWDG_CFGR_EWI_MASK                      (0x01U << WWDG_CFGR_EWI_SHIFT)
#define WWDG_CFGR_EWI(x)                        (((uint32_t)(((uint32_t)(x)) << WWDG_CFGR_EWI_SHIFT)) & WWDG_CFGR_EWI_MASK)

#define WWDG_CFGR_WDGTB_SHIFT                   (7)
#define WWDG_CFGR_WDGTB_MASK                    (0x3U << WWDG_CFGR_WDGTB_SHIFT)
#define WWDG_CFGR_WDGTB(x)                      (((uint32_t)(((uint32_t)(x)) << WWDG_CFGR_WDGTB_SHIFT)) & WWDG_CFGR_WDGTB_MASK)

#define WWDG_CFGR_W_SHIFT                       (0)
#define WWDG_CFGR_W_MASK                        (0x7FU << WWDG_CFGR_W_SHIFT)
#define WWDG_CFGR_W(x)                          (((uint32_t)(((uint32_t)(x)) << WWDG_CFGR_W_SHIFT)) & WWDG_CFGR_W_MASK)

/*!
 * @brief WWDG_SR Register Bit Definition
 */

#define WWDG_SR_EWIF_SHIFT                      (0)
#define WWDG_SR_EWIF_MASK                       (0x01U << WWDG_SR_EWIF_SHIFT)
#define WWDG_SR_EWIF(x)                         (((uint32_t)(((uint32_t)(x)) << WWDG_SR_EWIF_SHIFT)) & WWDG_SR_EWIF_MASK)

/*!
 * @}
 */ /* end of group WWDG_Register_Masks */
/******************************************************************************
 *WWDG Instance
*******************************************************************************/

#define WWDG                 ((WWDG_Type*)WWDG_BASE)

/*!
 * @}
 */ /* end of group WWDG_Peripheral_Access_Layer */

/*
** End of section using anonymous unions
*/

/* --------  End of section using anonymous unions and disabling warnings  -------- */
#if   defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
  #pragma clang diagnostic pop
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif


#ifdef __cplusplus
}
#endif

/*!
 * @}
 */ /* end of group Peripheral_access_layer */

#endif /* __MM32F5277E_H__ */
