/*
** ###################################################################
**     Processors:          MK60DN512VLL10
**                          MK60DX256VLL10
**                          MK60DN256VLL10
**                          MK60DN512VLQ10
**                          MK60DN256VLQ10
**                          MK60DX256VLQ10
**                          MK60DN512VMC10
**                          MK60DN256VMC10
**                          MK60DX256VMC10
**                          MK60DN512VMD10
**                          MK60DX256VMD10
**                          MK60DN256VMD10
**
**     Compilers:           ARM Compiler
**                          Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**
**     Reference manual:    K60P144M100SF2V2RM Rev. 2, Jun 2012
**     Version:             rev. 1.3, 2012-10-29
**
**     Abstract:
**         CMSIS Peripheral Access Layer for MK60D10
**
**     Copyright: 1997 - 2012 Freescale, Inc. All Rights Reserved.
**
**     http:                 www.freescale.com
**     mail:                 support@freescale.com
**
**     Revisions:
**     - rev. 1.0 (2012-01-03)
**         Initial version
**     - rev. 1.1 (2012-04-13)
**         Added new #define symbol MCU_MEM_MAP_VERSION_MINOR.
**         Added new #define symbols <peripheralType>_BASE_PTRS.
**     - rev. 1.2 (2012-07-09)
**         UART0 - Fixed register definition - CEA709.1-B (LON) registers added.
**     - rev. 1.3 (2012-10-29)
**         Registers updated according to the new reference manual revision - Rev. 2, Jun 2012
**
** ###################################################################
*/

/**
 * @file MK60D10.h
 * @version 1.3
 * @date 2012-10-29
 * @brief CMSIS Peripheral Access Layer for MK60D10
 *
 * CMSIS Peripheral Access Layer for MK60D10
 */

#if !defined(MK60D10_H_)
#define MK60D10_H_                               /**< Symbol preventing repeated inclusion */

/** Memory map major version (memory maps with equal major version number are
 * compatible) */
#define MCU_MEM_MAP_VERSION 0x0100u
/** Memory map minor version */
#define MCU_MEM_MAP_VERSION_MINOR 0x0003u

/**
 * @brief Macro to access a single bit of a peripheral register (bit band region
 *        0x40000000 to 0x400FFFFF) using the bit-band alias region access.
 * @param Reg Register to access.
 * @param Bit Bit number to access.
 * @return Value of the targeted bit in the bit band region.
 */
#define BITBAND_REG(Reg,Bit) (*((uint32_t volatile*)(0x42000000u + (32u*((uint32_t)&(Reg) - (uint32_t)0x40000000u)) + (4u*((uint32_t)(Bit))))))

/* ----------------------------------------------------------------------------
   -- Interrupt vector numbers
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup Interrupt_vector_numbers Interrupt vector numbers
 * @{
 */

/** Interrupt Number Definitions */
typedef enum IRQn {
  /* Core interrupts */
  NonMaskableInt_IRQn          = -14,              /**< Non Maskable Interrupt */
  HardFault_IRQn               = -13,              /**< Cortex-M4 SV Hard Fault Interrupt */
  MemoryManagement_IRQn        = -12,              /**< Cortex-M4 Memory Management Interrupt */
  BusFault_IRQn                = -11,              /**< Cortex-M4 Bus Fault Interrupt */
  UsageFault_IRQn              = -10,              /**< Cortex-M4 Usage Fault Interrupt */
  SVCall_IRQn                  = -5,               /**< Cortex-M4 SV Call Interrupt */
  DebugMonitor_IRQn            = -4,               /**< Cortex-M4 Debug Monitor Interrupt */
  PendSV_IRQn                  = -2,               /**< Cortex-M4 Pend SV Interrupt */
  SysTick_IRQn                 = -1,               /**< Cortex-M4 System Tick Interrupt */

  /* Device specific interrupts */
  DMA0_IRQn                    = 0,                /**< DMA Channel 0 Transfer Complete */
  DMA1_IRQn                    = 1,                /**< DMA Channel 1 Transfer Complete */
  DMA2_IRQn                    = 2,                /**< DMA Channel 2 Transfer Complete */
  DMA3_IRQn                    = 3,                /**< DMA Channel 3 Transfer Complete */
  DMA4_IRQn                    = 4,                /**< DMA Channel 4 Transfer Complete */
  DMA5_IRQn                    = 5,                /**< DMA Channel 5 Transfer Complete */
  DMA6_IRQn                    = 6,                /**< DMA Channel 6 Transfer Complete */
  DMA7_IRQn                    = 7,                /**< DMA Channel 7 Transfer Complete */
  DMA8_IRQn                    = 8,                /**< DMA Channel 8 Transfer Complete */
  DMA9_IRQn                    = 9,                /**< DMA Channel 9 Transfer Complete */
  DMA10_IRQn                   = 10,               /**< DMA Channel 10 Transfer Complete */
  DMA11_IRQn                   = 11,               /**< DMA Channel 11 Transfer Complete */
  DMA12_IRQn                   = 12,               /**< DMA Channel 12 Transfer Complete */
  DMA13_IRQn                   = 13,               /**< DMA Channel 13 Transfer Complete */
  DMA14_IRQn                   = 14,               /**< DMA Channel 14 Transfer Complete */
  DMA15_IRQn                   = 15,               /**< DMA Channel 15 Transfer Complete */
  DMA_Error_IRQn               = 16,               /**< DMA Error Interrupt */
  MCM_IRQn                     = 17,               /**< Normal Interrupt */
  FTFL_IRQn                    = 18,               /**< FTFL Interrupt */
  Read_Collision_IRQn          = 19,               /**< Read Collision Interrupt */
  LVD_LVW_IRQn                 = 20,               /**< Low Voltage Detect, Low Voltage Warning */
  LLW_IRQn                     = 21,               /**< Low Leakage Wakeup */
  Watchdog_IRQn                = 22,               /**< WDOG Interrupt */
  RNG_IRQn                     = 23,               /**< RNGB Interrupt */
  I2C0_IRQn                    = 24,               /**< I2C0 interrupt */
  I2C1_IRQn                    = 25,               /**< I2C1 interrupt */
  SPI0_IRQn                    = 26,               /**< SPI0 Interrupt */
  SPI1_IRQn                    = 27,               /**< SPI1 Interrupt */
  SPI2_IRQn                    = 28,               /**< SPI2 Interrupt */
  CAN0_ORed_Message_buffer_IRQn = 29,              /**< CAN0 OR'd message buffers interrupt */
  CAN0_Bus_Off_IRQn            = 30,               /**< CAN0 bus off interrupt */
  CAN0_Error_IRQn              = 31,               /**< CAN0 error interrupt */
  CAN0_Tx_Warning_IRQn         = 32,               /**< CAN0 Tx warning interrupt */
  CAN0_Rx_Warning_IRQn         = 33,               /**< CAN0 Rx warning interrupt */
  CAN0_Wake_Up_IRQn            = 34,               /**< CAN0 wake up interrupt */
  I2S0_Tx_IRQn                 = 35,               /**< I2S0 transmit interrupt */
  I2S0_Rx_IRQn                 = 36,               /**< I2S0 receive interrupt */
  CAN1_ORed_Message_buffer_IRQn = 37,              /**< CAN1 OR'd message buffers interrupt */
  CAN1_Bus_Off_IRQn            = 38,               /**< CAN1 bus off interrupt */
  CAN1_Error_IRQn              = 39,               /**< CAN1 error interrupt */
  CAN1_Tx_Warning_IRQn         = 40,               /**< CAN1 Tx warning interrupt */
  CAN1_Rx_Warning_IRQn         = 41,               /**< CAN1 Rx warning interrupt */
  CAN1_Wake_Up_IRQn            = 42,               /**< CAN1 wake up interrupt */
  Reserved59_IRQn              = 43,               /**< Reserved interrupt 59 */
  UART0_LON_IRQn               = 44,               /**< UART0 LON interrupt */
  UART0_RX_TX_IRQn             = 45,               /**< UART0 Receive/Transmit interrupt */
  UART0_ERR_IRQn               = 46,               /**< UART0 Error interrupt */
  UART1_RX_TX_IRQn             = 47,               /**< UART1 Receive/Transmit interrupt */
  UART1_ERR_IRQn               = 48,               /**< UART1 Error interrupt */
  UART2_RX_TX_IRQn             = 49,               /**< UART2 Receive/Transmit interrupt */
  UART2_ERR_IRQn               = 50,               /**< UART2 Error interrupt */
  UART3_RX_TX_IRQn             = 51,               /**< UART3 Receive/Transmit interrupt */
  UART3_ERR_IRQn               = 52,               /**< UART3 Error interrupt */
  UART4_RX_TX_IRQn             = 53,               /**< UART4 Receive/Transmit interrupt */
  UART4_ERR_IRQn               = 54,               /**< UART4 Error interrupt */
  UART5_RX_TX_IRQn             = 55,               /**< UART5 Receive/Transmit interrupt */
  UART5_ERR_IRQn               = 56,               /**< UART5 Error interrupt */
  ADC0_IRQn                    = 57,               /**< ADC0 interrupt */
  ADC1_IRQn                    = 58,               /**< ADC1 interrupt */
  CMP0_IRQn                    = 59,               /**< CMP0 interrupt */
  CMP1_IRQn                    = 60,               /**< CMP1 interrupt */
  CMP2_IRQn                    = 61,               /**< CMP2 interrupt */
  FTM0_IRQn                    = 62,               /**< FTM0 fault, overflow and channels interrupt */
  FTM1_IRQn                    = 63,               /**< FTM1 fault, overflow and channels interrupt */
  FTM2_IRQn                    = 64,               /**< FTM2 fault, overflow and channels interrupt */
  CMT_IRQn                     = 65,               /**< CMT interrupt */
  RTC_IRQn                     = 66,               /**< RTC interrupt */
  RTC_Seconds_IRQn             = 67,               /**< RTC seconds interrupt */
  PIT0_IRQn                    = 68,               /**< PIT timer channel 0 interrupt */
  PIT1_IRQn                    = 69,               /**< PIT timer channel 1 interrupt */
  PIT2_IRQn                    = 70,               /**< PIT timer channel 2 interrupt */
  PIT3_IRQn                    = 71,               /**< PIT timer channel 3 interrupt */
  PDB0_IRQn                    = 72,               /**< PDB0 Interrupt */
  USB0_IRQn                    = 73,               /**< USB0 interrupt */
  USBDCD_IRQn                  = 74,               /**< USBDCD Interrupt */
  ENET_1588_Timer_IRQn         = 75,               /**< Ethernet MAC IEEE 1588 Timer Interrupt */
  ENET_Transmit_IRQn           = 76,               /**< Ethernet MAC Transmit Interrupt */
  ENET_Receive_IRQn            = 77,               /**< Ethernet MAC Receive Interrupt */
  ENET_Error_IRQn              = 78,               /**< Ethernet MAC Error and miscelaneous Interrupt */
  Reserved95_IRQn              = 79,               /**< Reserved interrupt 95 */
  SDHC_IRQn                    = 80,               /**< SDHC Interrupt */
  DAC0_IRQn                    = 81,               /**< DAC0 interrupt */
  DAC1_IRQn                    = 82,               /**< DAC1 interrupt */
  TSI0_IRQn                    = 83,               /**< TSI0 Interrupt */
  MCG_IRQn                     = 84,               /**< MCG Interrupt */
  LPTimer_IRQn                 = 85,               /**< LPTimer interrupt */
  Reserved102_IRQn             = 86,               /**< Reserved interrupt 102 */
  PORTA_IRQn                   = 87,               /**< Port A interrupt */
  PORTB_IRQn                   = 88,               /**< Port B interrupt */
  PORTC_IRQn                   = 89,               /**< Port C interrupt */
  PORTD_IRQn                   = 90,               /**< Port D interrupt */
  PORTE_IRQn                   = 91,               /**< Port E interrupt */
  Reserved108_IRQn             = 92,               /**< Reserved interrupt 108 */
  Reserved109_IRQn             = 93,               /**< Reserved interrupt 109 */
  SWI_IRQn                     = 94,               /**< Software interrupt */
  Reserved111_IRQn             = 95,               /**< Reserved interrupt 111 */
  Reserved112_IRQn             = 96,               /**< Reserved interrupt 112 */
  Reserved113_IRQn             = 97,               /**< Reserved interrupt 113 */
  Reserved114_IRQn             = 98,               /**< Reserved interrupt 114 */
  Reserved115_IRQn             = 99,               /**< Reserved interrupt 115 */
  Reserved116_IRQn             = 100,              /**< Reserved interrupt 116 */
  Reserved117_IRQn             = 101,              /**< Reserved interrupt 117 */
  Reserved118_IRQn             = 102,              /**< Reserved interrupt 118 */
  Reserved119_IRQn             = 103               /**< Reserved interrupt 119 */
} IRQn_Type;

/**
 * @}
 */ /* end of group Interrupt_vector_numbers */


/* ----------------------------------------------------------------------------
   -- Cortex M4 Core Configuration
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup Cortex_Core_Configuration Cortex M4 Core Configuration
 * @{
 */

#define __MPU_PRESENT                  0         /**< Defines if an MPU is present or not */
#define __NVIC_PRIO_BITS               4         /**< Number of priority bits implemented in the NVIC */
#define __Vendor_SysTickConfig         0         /**< Vendor specific implementation of SysTickConfig is defined */

#include "core_cm4.h"                  /* Core Peripheral Access Layer */
#include "system_MK60D10.h"            /* Device specific configuration file */

/**
 * @}
 */ /* end of group Cortex_Core_Configuration */


/* ----------------------------------------------------------------------------
   -- Device Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup Peripheral_access_layer Device Peripheral Access Layer
 * @{
 */


/*
** Start of section using anonymous unions
*/

#if defined(__ARMCC_VERSION)
  #pragma push
  #pragma anon_unions
#elif defined(__CWCC__)
  #pragma push
  #pragma cpp_extensions on
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__IAR_SYSTEMS_ICC__)
  #pragma language=extended
#else
  #error Not supported compiler type
#endif

/* ----------------------------------------------------------------------------
   -- ADC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup ADC_Peripheral_Access_Layer ADC Peripheral Access Layer
 * @{
 */

/** ADC - Register Layout Typedef */
typedef struct {
  __IO uint32_t SC1[2];                            /**< ADC Status and Control Registers 1, array offset: 0x0, array step: 0x4 */
  __IO uint32_t CFG1;                              /**< ADC Configuration Register 1, offset: 0x8 */
  __IO uint32_t CFG2;                              /**< ADC Configuration Register 2, offset: 0xC */
  __I  uint32_t R[2];                              /**< ADC Data Result Register, array offset: 0x10, array step: 0x4 */
  __IO uint32_t CV1;                               /**< Compare Value Registers, offset: 0x18 */
  __IO uint32_t CV2;                               /**< Compare Value Registers, offset: 0x1C */
  __IO uint32_t SC2;                               /**< Status and Control Register 2, offset: 0x20 */
  __IO uint32_t SC3;                               /**< Status and Control Register 3, offset: 0x24 */
  __IO uint32_t OFS;                               /**< ADC Offset Correction Register, offset: 0x28 */
  __IO uint32_t PG;                                /**< ADC Plus-Side Gain Register, offset: 0x2C */
  __IO uint32_t MG;                                /**< ADC Minus-Side Gain Register, offset: 0x30 */
  __IO uint32_t CLPD;                              /**< ADC Plus-Side General Calibration Value Register, offset: 0x34 */
  __IO uint32_t CLPS;                              /**< ADC Plus-Side General Calibration Value Register, offset: 0x38 */
  __IO uint32_t CLP4;                              /**< ADC Plus-Side General Calibration Value Register, offset: 0x3C */
  __IO uint32_t CLP3;                              /**< ADC Plus-Side General Calibration Value Register, offset: 0x40 */
  __IO uint32_t CLP2;                              /**< ADC Plus-Side General Calibration Value Register, offset: 0x44 */
  __IO uint32_t CLP1;                              /**< ADC Plus-Side General Calibration Value Register, offset: 0x48 */
  __IO uint32_t CLP0;                              /**< ADC Plus-Side General Calibration Value Register, offset: 0x4C */
  __IO uint32_t PGA;                               /**< ADC PGA Register, offset: 0x50 */
  __IO uint32_t CLMD;                              /**< ADC Minus-Side General Calibration Value Register, offset: 0x54 */
  __IO uint32_t CLMS;                              /**< ADC Minus-Side General Calibration Value Register, offset: 0x58 */
  __IO uint32_t CLM4;                              /**< ADC Minus-Side General Calibration Value Register, offset: 0x5C */
  __IO uint32_t CLM3;                              /**< ADC Minus-Side General Calibration Value Register, offset: 0x60 */
  __IO uint32_t CLM2;                              /**< ADC Minus-Side General Calibration Value Register, offset: 0x64 */
  __IO uint32_t CLM1;                              /**< ADC Minus-Side General Calibration Value Register, offset: 0x68 */
  __IO uint32_t CLM0;                              /**< ADC Minus-Side General Calibration Value Register, offset: 0x6C */
} ADC_Type;

/* ----------------------------------------------------------------------------
   -- ADC Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup ADC_Register_Masks ADC Register Masks
 * @{
 */

/* SC1 Bit Fields */
#define ADC_SC1_ADCH_MASK                        0x1Fu
#define ADC_SC1_ADCH_SHIFT                       0
#define ADC_SC1_ADCH(x)                          (((uint32_t)(((uint32_t)(x))<<ADC_SC1_ADCH_SHIFT))&ADC_SC1_ADCH_MASK)
#define ADC_SC1_DIFF_MASK                        0x20u
#define ADC_SC1_DIFF_SHIFT                       5
#define ADC_SC1_AIEN_MASK                        0x40u
#define ADC_SC1_AIEN_SHIFT                       6
#define ADC_SC1_COCO_MASK                        0x80u
#define ADC_SC1_COCO_SHIFT                       7
/* CFG1 Bit Fields */
#define ADC_CFG1_ADICLK_MASK                     0x3u
#define ADC_CFG1_ADICLK_SHIFT                    0
#define ADC_CFG1_ADICLK(x)                       (((uint32_t)(((uint32_t)(x))<<ADC_CFG1_ADICLK_SHIFT))&ADC_CFG1_ADICLK_MASK)
#define ADC_CFG1_MODE_MASK                       0xCu
#define ADC_CFG1_MODE_SHIFT                      2
#define ADC_CFG1_MODE(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_CFG1_MODE_SHIFT))&ADC_CFG1_MODE_MASK)
#define ADC_CFG1_ADLSMP_MASK                     0x10u
#define ADC_CFG1_ADLSMP_SHIFT                    4
#define ADC_CFG1_ADIV_MASK                       0x60u
#define ADC_CFG1_ADIV_SHIFT                      5
#define ADC_CFG1_ADIV(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_CFG1_ADIV_SHIFT))&ADC_CFG1_ADIV_MASK)
#define ADC_CFG1_ADLPC_MASK                      0x80u
#define ADC_CFG1_ADLPC_SHIFT                     7
/* CFG2 Bit Fields */
#define ADC_CFG2_ADLSTS_MASK                     0x3u
#define ADC_CFG2_ADLSTS_SHIFT                    0
#define ADC_CFG2_ADLSTS(x)                       (((uint32_t)(((uint32_t)(x))<<ADC_CFG2_ADLSTS_SHIFT))&ADC_CFG2_ADLSTS_MASK)
#define ADC_CFG2_ADHSC_MASK                      0x4u
#define ADC_CFG2_ADHSC_SHIFT                     2
#define ADC_CFG2_ADACKEN_MASK                    0x8u
#define ADC_CFG2_ADACKEN_SHIFT                   3
#define ADC_CFG2_MUXSEL_MASK                     0x10u
#define ADC_CFG2_MUXSEL_SHIFT                    4
/* R Bit Fields */
#define ADC_R_D_MASK                             0xFFFFu
#define ADC_R_D_SHIFT                            0
#define ADC_R_D(x)                               (((uint32_t)(((uint32_t)(x))<<ADC_R_D_SHIFT))&ADC_R_D_MASK)
/* CV1 Bit Fields */
#define ADC_CV1_CV_MASK                          0xFFFFu
#define ADC_CV1_CV_SHIFT                         0
#define ADC_CV1_CV(x)                            (((uint32_t)(((uint32_t)(x))<<ADC_CV1_CV_SHIFT))&ADC_CV1_CV_MASK)
/* CV2 Bit Fields */
#define ADC_CV2_CV_MASK                          0xFFFFu
#define ADC_CV2_CV_SHIFT                         0
#define ADC_CV2_CV(x)                            (((uint32_t)(((uint32_t)(x))<<ADC_CV2_CV_SHIFT))&ADC_CV2_CV_MASK)
/* SC2 Bit Fields */
#define ADC_SC2_REFSEL_MASK                      0x3u
#define ADC_SC2_REFSEL_SHIFT                     0
#define ADC_SC2_REFSEL(x)                        (((uint32_t)(((uint32_t)(x))<<ADC_SC2_REFSEL_SHIFT))&ADC_SC2_REFSEL_MASK)
#define ADC_SC2_DMAEN_MASK                       0x4u
#define ADC_SC2_DMAEN_SHIFT                      2
#define ADC_SC2_ACREN_MASK                       0x8u
#define ADC_SC2_ACREN_SHIFT                      3
#define ADC_SC2_ACFGT_MASK                       0x10u
#define ADC_SC2_ACFGT_SHIFT                      4
#define ADC_SC2_ACFE_MASK                        0x20u
#define ADC_SC2_ACFE_SHIFT                       5
#define ADC_SC2_ADTRG_MASK                       0x40u
#define ADC_SC2_ADTRG_SHIFT                      6
#define ADC_SC2_ADACT_MASK                       0x80u
#define ADC_SC2_ADACT_SHIFT                      7
/* SC3 Bit Fields */
#define ADC_SC3_AVGS_MASK                        0x3u
#define ADC_SC3_AVGS_SHIFT                       0
#define ADC_SC3_AVGS(x)                          (((uint32_t)(((uint32_t)(x))<<ADC_SC3_AVGS_SHIFT))&ADC_SC3_AVGS_MASK)
#define ADC_SC3_AVGE_MASK                        0x4u
#define ADC_SC3_AVGE_SHIFT                       2
#define ADC_SC3_ADCO_MASK                        0x8u
#define ADC_SC3_ADCO_SHIFT                       3
#define ADC_SC3_CALF_MASK                        0x40u
#define ADC_SC3_CALF_SHIFT                       6
#define ADC_SC3_CAL_MASK                         0x80u
#define ADC_SC3_CAL_SHIFT                        7
/* OFS Bit Fields */
#define ADC_OFS_OFS_MASK                         0xFFFFu
#define ADC_OFS_OFS_SHIFT                        0
#define ADC_OFS_OFS(x)                           (((uint32_t)(((uint32_t)(x))<<ADC_OFS_OFS_SHIFT))&ADC_OFS_OFS_MASK)
/* PG Bit Fields */
#define ADC_PG_PG_MASK                           0xFFFFu
#define ADC_PG_PG_SHIFT                          0
#define ADC_PG_PG(x)                             (((uint32_t)(((uint32_t)(x))<<ADC_PG_PG_SHIFT))&ADC_PG_PG_MASK)
/* MG Bit Fields */
#define ADC_MG_MG_MASK                           0xFFFFu
#define ADC_MG_MG_SHIFT                          0
#define ADC_MG_MG(x)                             (((uint32_t)(((uint32_t)(x))<<ADC_MG_MG_SHIFT))&ADC_MG_MG_MASK)
/* CLPD Bit Fields */
#define ADC_CLPD_CLPD_MASK                       0x3Fu
#define ADC_CLPD_CLPD_SHIFT                      0
#define ADC_CLPD_CLPD(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_CLPD_CLPD_SHIFT))&ADC_CLPD_CLPD_MASK)
/* CLPS Bit Fields */
#define ADC_CLPS_CLPS_MASK                       0x3Fu
#define ADC_CLPS_CLPS_SHIFT                      0
#define ADC_CLPS_CLPS(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_CLPS_CLPS_SHIFT))&ADC_CLPS_CLPS_MASK)
/* CLP4 Bit Fields */
#define ADC_CLP4_CLP4_MASK                       0x3FFu
#define ADC_CLP4_CLP4_SHIFT                      0
#define ADC_CLP4_CLP4(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_CLP4_CLP4_SHIFT))&ADC_CLP4_CLP4_MASK)
/* CLP3 Bit Fields */
#define ADC_CLP3_CLP3_MASK                       0x1FFu
#define ADC_CLP3_CLP3_SHIFT                      0
#define ADC_CLP3_CLP3(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_CLP3_CLP3_SHIFT))&ADC_CLP3_CLP3_MASK)
/* CLP2 Bit Fields */
#define ADC_CLP2_CLP2_MASK                       0xFFu
#define ADC_CLP2_CLP2_SHIFT                      0
#define ADC_CLP2_CLP2(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_CLP2_CLP2_SHIFT))&ADC_CLP2_CLP2_MASK)
/* CLP1 Bit Fields */
#define ADC_CLP1_CLP1_MASK                       0x7Fu
#define ADC_CLP1_CLP1_SHIFT                      0
#define ADC_CLP1_CLP1(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_CLP1_CLP1_SHIFT))&ADC_CLP1_CLP1_MASK)
/* CLP0 Bit Fields */
#define ADC_CLP0_CLP0_MASK                       0x3Fu
#define ADC_CLP0_CLP0_SHIFT                      0
#define ADC_CLP0_CLP0(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_CLP0_CLP0_SHIFT))&ADC_CLP0_CLP0_MASK)
/* PGA Bit Fields */
#define ADC_PGA_PGAG_MASK                        0xF0000u
#define ADC_PGA_PGAG_SHIFT                       16
#define ADC_PGA_PGAG(x)                          (((uint32_t)(((uint32_t)(x))<<ADC_PGA_PGAG_SHIFT))&ADC_PGA_PGAG_MASK)
#define ADC_PGA_PGALPb_MASK                      0x100000u
#define ADC_PGA_PGALPb_SHIFT                     20
#define ADC_PGA_PGAEN_MASK                       0x800000u
#define ADC_PGA_PGAEN_SHIFT                      23
/* CLMD Bit Fields */
#define ADC_CLMD_CLMD_MASK                       0x3Fu
#define ADC_CLMD_CLMD_SHIFT                      0
#define ADC_CLMD_CLMD(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_CLMD_CLMD_SHIFT))&ADC_CLMD_CLMD_MASK)
/* CLMS Bit Fields */
#define ADC_CLMS_CLMS_MASK                       0x3Fu
#define ADC_CLMS_CLMS_SHIFT                      0
#define ADC_CLMS_CLMS(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_CLMS_CLMS_SHIFT))&ADC_CLMS_CLMS_MASK)
/* CLM4 Bit Fields */
#define ADC_CLM4_CLM4_MASK                       0x3FFu
#define ADC_CLM4_CLM4_SHIFT                      0
#define ADC_CLM4_CLM4(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_CLM4_CLM4_SHIFT))&ADC_CLM4_CLM4_MASK)
/* CLM3 Bit Fields */
#define ADC_CLM3_CLM3_MASK                       0x1FFu
#define ADC_CLM3_CLM3_SHIFT                      0
#define ADC_CLM3_CLM3(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_CLM3_CLM3_SHIFT))&ADC_CLM3_CLM3_MASK)
/* CLM2 Bit Fields */
#define ADC_CLM2_CLM2_MASK                       0xFFu
#define ADC_CLM2_CLM2_SHIFT                      0
#define ADC_CLM2_CLM2(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_CLM2_CLM2_SHIFT))&ADC_CLM2_CLM2_MASK)
/* CLM1 Bit Fields */
#define ADC_CLM1_CLM1_MASK                       0x7Fu
#define ADC_CLM1_CLM1_SHIFT                      0
#define ADC_CLM1_CLM1(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_CLM1_CLM1_SHIFT))&ADC_CLM1_CLM1_MASK)
/* CLM0 Bit Fields */
#define ADC_CLM0_CLM0_MASK                       0x3Fu
#define ADC_CLM0_CLM0_SHIFT                      0
#define ADC_CLM0_CLM0(x)                         (((uint32_t)(((uint32_t)(x))<<ADC_CLM0_CLM0_SHIFT))&ADC_CLM0_CLM0_MASK)

/**
 * @}
 */ /* end of group ADC_Register_Masks */


/* ADC - Peripheral instance base addresses */
/** Peripheral ADC0 base address */
#define ADC0_BASE                                (0x4003B000u)
/** Peripheral ADC0 base pointer */
#define ADC0                                     ((ADC_Type *)ADC0_BASE)
/** Peripheral ADC1 base address */
#define ADC1_BASE                                (0x400BB000u)
/** Peripheral ADC1 base pointer */
#define ADC1                                     ((ADC_Type *)ADC1_BASE)
/** Array initializer of ADC peripheral base pointers */
#define ADC_BASES                                { ADC0, ADC1 }

/**
 * @}
 */ /* end of group ADC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- AIPS Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup AIPS_Peripheral_Access_Layer AIPS Peripheral Access Layer
 * @{
 */

/** AIPS - Register Layout Typedef */
typedef struct {
  __IO uint32_t MPRA;                              /**< Master Privilege Register A, offset: 0x0 */
       uint8_t RESERVED_0[28];
  __IO uint32_t PACRA;                             /**< Peripheral Access Control Register, offset: 0x20 */
  __IO uint32_t PACRB;                             /**< Peripheral Access Control Register, offset: 0x24 */
  __IO uint32_t PACRC;                             /**< Peripheral Access Control Register, offset: 0x28 */
  __IO uint32_t PACRD;                             /**< Peripheral Access Control Register, offset: 0x2C */
       uint8_t RESERVED_1[16];
  __IO uint32_t PACRE;                             /**< Peripheral Access Control Register, offset: 0x40 */
  __IO uint32_t PACRF;                             /**< Peripheral Access Control Register, offset: 0x44 */
  __IO uint32_t PACRG;                             /**< Peripheral Access Control Register, offset: 0x48 */
  __IO uint32_t PACRH;                             /**< Peripheral Access Control Register, offset: 0x4C */
  __IO uint32_t PACRI;                             /**< Peripheral Access Control Register, offset: 0x50 */
  __IO uint32_t PACRJ;                             /**< Peripheral Access Control Register, offset: 0x54 */
  __IO uint32_t PACRK;                             /**< Peripheral Access Control Register, offset: 0x58 */
  __IO uint32_t PACRL;                             /**< Peripheral Access Control Register, offset: 0x5C */
  __IO uint32_t PACRM;                             /**< Peripheral Access Control Register, offset: 0x60 */
  __IO uint32_t PACRN;                             /**< Peripheral Access Control Register, offset: 0x64 */
  __IO uint32_t PACRO;                             /**< Peripheral Access Control Register, offset: 0x68 */
  __IO uint32_t PACRP;                             /**< Peripheral Access Control Register, offset: 0x6C */
} AIPS_Type;

/* ----------------------------------------------------------------------------
   -- AIPS Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup AIPS_Register_Masks AIPS Register Masks
 * @{
 */

/* MPRA Bit Fields */
#define AIPS_MPRA_MPL5_MASK                      0x100u
#define AIPS_MPRA_MPL5_SHIFT                     8
#define AIPS_MPRA_MTW5_MASK                      0x200u
#define AIPS_MPRA_MTW5_SHIFT                     9
#define AIPS_MPRA_MTR5_MASK                      0x400u
#define AIPS_MPRA_MTR5_SHIFT                     10
#define AIPS_MPRA_MPL4_MASK                      0x1000u
#define AIPS_MPRA_MPL4_SHIFT                     12
#define AIPS_MPRA_MTW4_MASK                      0x2000u
#define AIPS_MPRA_MTW4_SHIFT                     13
#define AIPS_MPRA_MTR4_MASK                      0x4000u
#define AIPS_MPRA_MTR4_SHIFT                     14
#define AIPS_MPRA_MPL3_MASK                      0x10000u
#define AIPS_MPRA_MPL3_SHIFT                     16
#define AIPS_MPRA_MTW3_MASK                      0x20000u
#define AIPS_MPRA_MTW3_SHIFT                     17
#define AIPS_MPRA_MTR3_MASK                      0x40000u
#define AIPS_MPRA_MTR3_SHIFT                     18
#define AIPS_MPRA_MPL2_MASK                      0x100000u
#define AIPS_MPRA_MPL2_SHIFT                     20
#define AIPS_MPRA_MTW2_MASK                      0x200000u
#define AIPS_MPRA_MTW2_SHIFT                     21
#define AIPS_MPRA_MTR2_MASK                      0x400000u
#define AIPS_MPRA_MTR2_SHIFT                     22
#define AIPS_MPRA_MPL1_MASK                      0x1000000u
#define AIPS_MPRA_MPL1_SHIFT                     24
#define AIPS_MPRA_MTW1_MASK                      0x2000000u
#define AIPS_MPRA_MTW1_SHIFT                     25
#define AIPS_MPRA_MTR1_MASK                      0x4000000u
#define AIPS_MPRA_MTR1_SHIFT                     26
#define AIPS_MPRA_MPL0_MASK                      0x10000000u
#define AIPS_MPRA_MPL0_SHIFT                     28
#define AIPS_MPRA_MTW0_MASK                      0x20000000u
#define AIPS_MPRA_MTW0_SHIFT                     29
#define AIPS_MPRA_MTR0_MASK                      0x40000000u
#define AIPS_MPRA_MTR0_SHIFT                     30
/* PACRA Bit Fields */
#define AIPS_PACRA_TP7_MASK                      0x1u
#define AIPS_PACRA_TP7_SHIFT                     0
#define AIPS_PACRA_WP7_MASK                      0x2u
#define AIPS_PACRA_WP7_SHIFT                     1
#define AIPS_PACRA_SP7_MASK                      0x4u
#define AIPS_PACRA_SP7_SHIFT                     2
#define AIPS_PACRA_TP6_MASK                      0x10u
#define AIPS_PACRA_TP6_SHIFT                     4
#define AIPS_PACRA_WP6_MASK                      0x20u
#define AIPS_PACRA_WP6_SHIFT                     5
#define AIPS_PACRA_SP6_MASK                      0x40u
#define AIPS_PACRA_SP6_SHIFT                     6
#define AIPS_PACRA_TP5_MASK                      0x100u
#define AIPS_PACRA_TP5_SHIFT                     8
#define AIPS_PACRA_WP5_MASK                      0x200u
#define AIPS_PACRA_WP5_SHIFT                     9
#define AIPS_PACRA_SP5_MASK                      0x400u
#define AIPS_PACRA_SP5_SHIFT                     10
#define AIPS_PACRA_TP4_MASK                      0x1000u
#define AIPS_PACRA_TP4_SHIFT                     12
#define AIPS_PACRA_WP4_MASK                      0x2000u
#define AIPS_PACRA_WP4_SHIFT                     13
#define AIPS_PACRA_SP4_MASK                      0x4000u
#define AIPS_PACRA_SP4_SHIFT                     14
#define AIPS_PACRA_TP3_MASK                      0x10000u
#define AIPS_PACRA_TP3_SHIFT                     16
#define AIPS_PACRA_WP3_MASK                      0x20000u
#define AIPS_PACRA_WP3_SHIFT                     17
#define AIPS_PACRA_SP3_MASK                      0x40000u
#define AIPS_PACRA_SP3_SHIFT                     18
#define AIPS_PACRA_TP2_MASK                      0x100000u
#define AIPS_PACRA_TP2_SHIFT                     20
#define AIPS_PACRA_WP2_MASK                      0x200000u
#define AIPS_PACRA_WP2_SHIFT                     21
#define AIPS_PACRA_SP2_MASK                      0x400000u
#define AIPS_PACRA_SP2_SHIFT                     22
#define AIPS_PACRA_TP1_MASK                      0x1000000u
#define AIPS_PACRA_TP1_SHIFT                     24
#define AIPS_PACRA_WP1_MASK                      0x2000000u
#define AIPS_PACRA_WP1_SHIFT                     25
#define AIPS_PACRA_SP1_MASK                      0x4000000u
#define AIPS_PACRA_SP1_SHIFT                     26
#define AIPS_PACRA_TP0_MASK                      0x10000000u
#define AIPS_PACRA_TP0_SHIFT                     28
#define AIPS_PACRA_WP0_MASK                      0x20000000u
#define AIPS_PACRA_WP0_SHIFT                     29
#define AIPS_PACRA_SP0_MASK                      0x40000000u
#define AIPS_PACRA_SP0_SHIFT                     30
/* PACRB Bit Fields */
#define AIPS_PACRB_TP7_MASK                      0x1u
#define AIPS_PACRB_TP7_SHIFT                     0
#define AIPS_PACRB_WP7_MASK                      0x2u
#define AIPS_PACRB_WP7_SHIFT                     1
#define AIPS_PACRB_SP7_MASK                      0x4u
#define AIPS_PACRB_SP7_SHIFT                     2
#define AIPS_PACRB_TP6_MASK                      0x10u
#define AIPS_PACRB_TP6_SHIFT                     4
#define AIPS_PACRB_WP6_MASK                      0x20u
#define AIPS_PACRB_WP6_SHIFT                     5
#define AIPS_PACRB_SP6_MASK                      0x40u
#define AIPS_PACRB_SP6_SHIFT                     6
#define AIPS_PACRB_TP5_MASK                      0x100u
#define AIPS_PACRB_TP5_SHIFT                     8
#define AIPS_PACRB_WP5_MASK                      0x200u
#define AIPS_PACRB_WP5_SHIFT                     9
#define AIPS_PACRB_SP5_MASK                      0x400u
#define AIPS_PACRB_SP5_SHIFT                     10
#define AIPS_PACRB_TP4_MASK                      0x1000u
#define AIPS_PACRB_TP4_SHIFT                     12
#define AIPS_PACRB_WP4_MASK                      0x2000u
#define AIPS_PACRB_WP4_SHIFT                     13
#define AIPS_PACRB_SP4_MASK                      0x4000u
#define AIPS_PACRB_SP4_SHIFT                     14
#define AIPS_PACRB_TP3_MASK                      0x10000u
#define AIPS_PACRB_TP3_SHIFT                     16
#define AIPS_PACRB_WP3_MASK                      0x20000u
#define AIPS_PACRB_WP3_SHIFT                     17
#define AIPS_PACRB_SP3_MASK                      0x40000u
#define AIPS_PACRB_SP3_SHIFT                     18
#define AIPS_PACRB_TP2_MASK                      0x100000u
#define AIPS_PACRB_TP2_SHIFT                     20
#define AIPS_PACRB_WP2_MASK                      0x200000u
#define AIPS_PACRB_WP2_SHIFT                     21
#define AIPS_PACRB_SP2_MASK                      0x400000u
#define AIPS_PACRB_SP2_SHIFT                     22
#define AIPS_PACRB_TP1_MASK                      0x1000000u
#define AIPS_PACRB_TP1_SHIFT                     24
#define AIPS_PACRB_WP1_MASK                      0x2000000u
#define AIPS_PACRB_WP1_SHIFT                     25
#define AIPS_PACRB_SP1_MASK                      0x4000000u
#define AIPS_PACRB_SP1_SHIFT                     26
#define AIPS_PACRB_TP0_MASK                      0x10000000u
#define AIPS_PACRB_TP0_SHIFT                     28
#define AIPS_PACRB_WP0_MASK                      0x20000000u
#define AIPS_PACRB_WP0_SHIFT                     29
#define AIPS_PACRB_SP0_MASK                      0x40000000u
#define AIPS_PACRB_SP0_SHIFT                     30
/* PACRC Bit Fields */
#define AIPS_PACRC_TP7_MASK                      0x1u
#define AIPS_PACRC_TP7_SHIFT                     0
#define AIPS_PACRC_WP7_MASK                      0x2u
#define AIPS_PACRC_WP7_SHIFT                     1
#define AIPS_PACRC_SP7_MASK                      0x4u
#define AIPS_PACRC_SP7_SHIFT                     2
#define AIPS_PACRC_TP6_MASK                      0x10u
#define AIPS_PACRC_TP6_SHIFT                     4
#define AIPS_PACRC_WP6_MASK                      0x20u
#define AIPS_PACRC_WP6_SHIFT                     5
#define AIPS_PACRC_SP6_MASK                      0x40u
#define AIPS_PACRC_SP6_SHIFT                     6
#define AIPS_PACRC_TP5_MASK                      0x100u
#define AIPS_PACRC_TP5_SHIFT                     8
#define AIPS_PACRC_WP5_MASK                      0x200u
#define AIPS_PACRC_WP5_SHIFT                     9
#define AIPS_PACRC_SP5_MASK                      0x400u
#define AIPS_PACRC_SP5_SHIFT                     10
#define AIPS_PACRC_TP4_MASK                      0x1000u
#define AIPS_PACRC_TP4_SHIFT                     12
#define AIPS_PACRC_WP4_MASK                      0x2000u
#define AIPS_PACRC_WP4_SHIFT                     13
#define AIPS_PACRC_SP4_MASK                      0x4000u
#define AIPS_PACRC_SP4_SHIFT                     14
#define AIPS_PACRC_TP3_MASK                      0x10000u
#define AIPS_PACRC_TP3_SHIFT                     16
#define AIPS_PACRC_WP3_MASK                      0x20000u
#define AIPS_PACRC_WP3_SHIFT                     17
#define AIPS_PACRC_SP3_MASK                      0x40000u
#define AIPS_PACRC_SP3_SHIFT                     18
#define AIPS_PACRC_TP2_MASK                      0x100000u
#define AIPS_PACRC_TP2_SHIFT                     20
#define AIPS_PACRC_WP2_MASK                      0x200000u
#define AIPS_PACRC_WP2_SHIFT                     21
#define AIPS_PACRC_SP2_MASK                      0x400000u
#define AIPS_PACRC_SP2_SHIFT                     22
#define AIPS_PACRC_TP1_MASK                      0x1000000u
#define AIPS_PACRC_TP1_SHIFT                     24
#define AIPS_PACRC_WP1_MASK                      0x2000000u
#define AIPS_PACRC_WP1_SHIFT                     25
#define AIPS_PACRC_SP1_MASK                      0x4000000u
#define AIPS_PACRC_SP1_SHIFT                     26
#define AIPS_PACRC_TP0_MASK                      0x10000000u
#define AIPS_PACRC_TP0_SHIFT                     28
#define AIPS_PACRC_WP0_MASK                      0x20000000u
#define AIPS_PACRC_WP0_SHIFT                     29
#define AIPS_PACRC_SP0_MASK                      0x40000000u
#define AIPS_PACRC_SP0_SHIFT                     30
/* PACRD Bit Fields */
#define AIPS_PACRD_TP7_MASK                      0x1u
#define AIPS_PACRD_TP7_SHIFT                     0
#define AIPS_PACRD_WP7_MASK                      0x2u
#define AIPS_PACRD_WP7_SHIFT                     1
#define AIPS_PACRD_SP7_MASK                      0x4u
#define AIPS_PACRD_SP7_SHIFT                     2
#define AIPS_PACRD_TP6_MASK                      0x10u
#define AIPS_PACRD_TP6_SHIFT                     4
#define AIPS_PACRD_WP6_MASK                      0x20u
#define AIPS_PACRD_WP6_SHIFT                     5
#define AIPS_PACRD_SP6_MASK                      0x40u
#define AIPS_PACRD_SP6_SHIFT                     6
#define AIPS_PACRD_TP5_MASK                      0x100u
#define AIPS_PACRD_TP5_SHIFT                     8
#define AIPS_PACRD_WP5_MASK                      0x200u
#define AIPS_PACRD_WP5_SHIFT                     9
#define AIPS_PACRD_SP5_MASK                      0x400u
#define AIPS_PACRD_SP5_SHIFT                     10
#define AIPS_PACRD_TP4_MASK                      0x1000u
#define AIPS_PACRD_TP4_SHIFT                     12
#define AIPS_PACRD_WP4_MASK                      0x2000u
#define AIPS_PACRD_WP4_SHIFT                     13
#define AIPS_PACRD_SP4_MASK                      0x4000u
#define AIPS_PACRD_SP4_SHIFT                     14
#define AIPS_PACRD_TP3_MASK                      0x10000u
#define AIPS_PACRD_TP3_SHIFT                     16
#define AIPS_PACRD_WP3_MASK                      0x20000u
#define AIPS_PACRD_WP3_SHIFT                     17
#define AIPS_PACRD_SP3_MASK                      0x40000u
#define AIPS_PACRD_SP3_SHIFT                     18
#define AIPS_PACRD_TP2_MASK                      0x100000u
#define AIPS_PACRD_TP2_SHIFT                     20
#define AIPS_PACRD_WP2_MASK                      0x200000u
#define AIPS_PACRD_WP2_SHIFT                     21
#define AIPS_PACRD_SP2_MASK                      0x400000u
#define AIPS_PACRD_SP2_SHIFT                     22
#define AIPS_PACRD_TP1_MASK                      0x1000000u
#define AIPS_PACRD_TP1_SHIFT                     24
#define AIPS_PACRD_WP1_MASK                      0x2000000u
#define AIPS_PACRD_WP1_SHIFT                     25
#define AIPS_PACRD_SP1_MASK                      0x4000000u
#define AIPS_PACRD_SP1_SHIFT                     26
#define AIPS_PACRD_TP0_MASK                      0x10000000u
#define AIPS_PACRD_TP0_SHIFT                     28
#define AIPS_PACRD_WP0_MASK                      0x20000000u
#define AIPS_PACRD_WP0_SHIFT                     29
#define AIPS_PACRD_SP0_MASK                      0x40000000u
#define AIPS_PACRD_SP0_SHIFT                     30
/* PACRE Bit Fields */
#define AIPS_PACRE_TP7_MASK                      0x1u
#define AIPS_PACRE_TP7_SHIFT                     0
#define AIPS_PACRE_WP7_MASK                      0x2u
#define AIPS_PACRE_WP7_SHIFT                     1
#define AIPS_PACRE_SP7_MASK                      0x4u
#define AIPS_PACRE_SP7_SHIFT                     2
#define AIPS_PACRE_TP6_MASK                      0x10u
#define AIPS_PACRE_TP6_SHIFT                     4
#define AIPS_PACRE_WP6_MASK                      0x20u
#define AIPS_PACRE_WP6_SHIFT                     5
#define AIPS_PACRE_SP6_MASK                      0x40u
#define AIPS_PACRE_SP6_SHIFT                     6
#define AIPS_PACRE_TP5_MASK                      0x100u
#define AIPS_PACRE_TP5_SHIFT                     8
#define AIPS_PACRE_WP5_MASK                      0x200u
#define AIPS_PACRE_WP5_SHIFT                     9
#define AIPS_PACRE_SP5_MASK                      0x400u
#define AIPS_PACRE_SP5_SHIFT                     10
#define AIPS_PACRE_TP4_MASK                      0x1000u
#define AIPS_PACRE_TP4_SHIFT                     12
#define AIPS_PACRE_WP4_MASK                      0x2000u
#define AIPS_PACRE_WP4_SHIFT                     13
#define AIPS_PACRE_SP4_MASK                      0x4000u
#define AIPS_PACRE_SP4_SHIFT                     14
#define AIPS_PACRE_TP3_MASK                      0x10000u
#define AIPS_PACRE_TP3_SHIFT                     16
#define AIPS_PACRE_WP3_MASK                      0x20000u
#define AIPS_PACRE_WP3_SHIFT                     17
#define AIPS_PACRE_SP3_MASK                      0x40000u
#define AIPS_PACRE_SP3_SHIFT                     18
#define AIPS_PACRE_TP2_MASK                      0x100000u
#define AIPS_PACRE_TP2_SHIFT                     20
#define AIPS_PACRE_WP2_MASK                      0x200000u
#define AIPS_PACRE_WP2_SHIFT                     21
#define AIPS_PACRE_SP2_MASK                      0x400000u
#define AIPS_PACRE_SP2_SHIFT                     22
#define AIPS_PACRE_TP1_MASK                      0x1000000u
#define AIPS_PACRE_TP1_SHIFT                     24
#define AIPS_PACRE_WP1_MASK                      0x2000000u
#define AIPS_PACRE_WP1_SHIFT                     25
#define AIPS_PACRE_SP1_MASK                      0x4000000u
#define AIPS_PACRE_SP1_SHIFT                     26
#define AIPS_PACRE_TP0_MASK                      0x10000000u
#define AIPS_PACRE_TP0_SHIFT                     28
#define AIPS_PACRE_WP0_MASK                      0x20000000u
#define AIPS_PACRE_WP0_SHIFT                     29
#define AIPS_PACRE_SP0_MASK                      0x40000000u
#define AIPS_PACRE_SP0_SHIFT                     30
/* PACRF Bit Fields */
#define AIPS_PACRF_TP7_MASK                      0x1u
#define AIPS_PACRF_TP7_SHIFT                     0
#define AIPS_PACRF_WP7_MASK                      0x2u
#define AIPS_PACRF_WP7_SHIFT                     1
#define AIPS_PACRF_SP7_MASK                      0x4u
#define AIPS_PACRF_SP7_SHIFT                     2
#define AIPS_PACRF_TP6_MASK                      0x10u
#define AIPS_PACRF_TP6_SHIFT                     4
#define AIPS_PACRF_WP6_MASK                      0x20u
#define AIPS_PACRF_WP6_SHIFT                     5
#define AIPS_PACRF_SP6_MASK                      0x40u
#define AIPS_PACRF_SP6_SHIFT                     6
#define AIPS_PACRF_TP5_MASK                      0x100u
#define AIPS_PACRF_TP5_SHIFT                     8
#define AIPS_PACRF_WP5_MASK                      0x200u
#define AIPS_PACRF_WP5_SHIFT                     9
#define AIPS_PACRF_SP5_MASK                      0x400u
#define AIPS_PACRF_SP5_SHIFT                     10
#define AIPS_PACRF_TP4_MASK                      0x1000u
#define AIPS_PACRF_TP4_SHIFT                     12
#define AIPS_PACRF_WP4_MASK                      0x2000u
#define AIPS_PACRF_WP4_SHIFT                     13
#define AIPS_PACRF_SP4_MASK                      0x4000u
#define AIPS_PACRF_SP4_SHIFT                     14
#define AIPS_PACRF_TP3_MASK                      0x10000u
#define AIPS_PACRF_TP3_SHIFT                     16
#define AIPS_PACRF_WP3_MASK                      0x20000u
#define AIPS_PACRF_WP3_SHIFT                     17
#define AIPS_PACRF_SP3_MASK                      0x40000u
#define AIPS_PACRF_SP3_SHIFT                     18
#define AIPS_PACRF_TP2_MASK                      0x100000u
#define AIPS_PACRF_TP2_SHIFT                     20
#define AIPS_PACRF_WP2_MASK                      0x200000u
#define AIPS_PACRF_WP2_SHIFT                     21
#define AIPS_PACRF_SP2_MASK                      0x400000u
#define AIPS_PACRF_SP2_SHIFT                     22
#define AIPS_PACRF_TP1_MASK                      0x1000000u
#define AIPS_PACRF_TP1_SHIFT                     24
#define AIPS_PACRF_WP1_MASK                      0x2000000u
#define AIPS_PACRF_WP1_SHIFT                     25
#define AIPS_PACRF_SP1_MASK                      0x4000000u
#define AIPS_PACRF_SP1_SHIFT                     26
#define AIPS_PACRF_TP0_MASK                      0x10000000u
#define AIPS_PACRF_TP0_SHIFT                     28
#define AIPS_PACRF_WP0_MASK                      0x20000000u
#define AIPS_PACRF_WP0_SHIFT                     29
#define AIPS_PACRF_SP0_MASK                      0x40000000u
#define AIPS_PACRF_SP0_SHIFT                     30
/* PACRG Bit Fields */
#define AIPS_PACRG_TP7_MASK                      0x1u
#define AIPS_PACRG_TP7_SHIFT                     0
#define AIPS_PACRG_WP7_MASK                      0x2u
#define AIPS_PACRG_WP7_SHIFT                     1
#define AIPS_PACRG_SP7_MASK                      0x4u
#define AIPS_PACRG_SP7_SHIFT                     2
#define AIPS_PACRG_TP6_MASK                      0x10u
#define AIPS_PACRG_TP6_SHIFT                     4
#define AIPS_PACRG_WP6_MASK                      0x20u
#define AIPS_PACRG_WP6_SHIFT                     5
#define AIPS_PACRG_SP6_MASK                      0x40u
#define AIPS_PACRG_SP6_SHIFT                     6
#define AIPS_PACRG_TP5_MASK                      0x100u
#define AIPS_PACRG_TP5_SHIFT                     8
#define AIPS_PACRG_WP5_MASK                      0x200u
#define AIPS_PACRG_WP5_SHIFT                     9
#define AIPS_PACRG_SP5_MASK                      0x400u
#define AIPS_PACRG_SP5_SHIFT                     10
#define AIPS_PACRG_TP4_MASK                      0x1000u
#define AIPS_PACRG_TP4_SHIFT                     12
#define AIPS_PACRG_WP4_MASK                      0x2000u
#define AIPS_PACRG_WP4_SHIFT                     13
#define AIPS_PACRG_SP4_MASK                      0x4000u
#define AIPS_PACRG_SP4_SHIFT                     14
#define AIPS_PACRG_TP3_MASK                      0x10000u
#define AIPS_PACRG_TP3_SHIFT                     16
#define AIPS_PACRG_WP3_MASK                      0x20000u
#define AIPS_PACRG_WP3_SHIFT                     17
#define AIPS_PACRG_SP3_MASK                      0x40000u
#define AIPS_PACRG_SP3_SHIFT                     18
#define AIPS_PACRG_TP2_MASK                      0x100000u
#define AIPS_PACRG_TP2_SHIFT                     20
#define AIPS_PACRG_WP2_MASK                      0x200000u
#define AIPS_PACRG_WP2_SHIFT                     21
#define AIPS_PACRG_SP2_MASK                      0x400000u
#define AIPS_PACRG_SP2_SHIFT                     22
#define AIPS_PACRG_TP1_MASK                      0x1000000u
#define AIPS_PACRG_TP1_SHIFT                     24
#define AIPS_PACRG_WP1_MASK                      0x2000000u
#define AIPS_PACRG_WP1_SHIFT                     25
#define AIPS_PACRG_SP1_MASK                      0x4000000u
#define AIPS_PACRG_SP1_SHIFT                     26
#define AIPS_PACRG_TP0_MASK                      0x10000000u
#define AIPS_PACRG_TP0_SHIFT                     28
#define AIPS_PACRG_WP0_MASK                      0x20000000u
#define AIPS_PACRG_WP0_SHIFT                     29
#define AIPS_PACRG_SP0_MASK                      0x40000000u
#define AIPS_PACRG_SP0_SHIFT                     30
/* PACRH Bit Fields */
#define AIPS_PACRH_TP7_MASK                      0x1u
#define AIPS_PACRH_TP7_SHIFT                     0
#define AIPS_PACRH_WP7_MASK                      0x2u
#define AIPS_PACRH_WP7_SHIFT                     1
#define AIPS_PACRH_SP7_MASK                      0x4u
#define AIPS_PACRH_SP7_SHIFT                     2
#define AIPS_PACRH_TP6_MASK                      0x10u
#define AIPS_PACRH_TP6_SHIFT                     4
#define AIPS_PACRH_WP6_MASK                      0x20u
#define AIPS_PACRH_WP6_SHIFT                     5
#define AIPS_PACRH_SP6_MASK                      0x40u
#define AIPS_PACRH_SP6_SHIFT                     6
#define AIPS_PACRH_TP5_MASK                      0x100u
#define AIPS_PACRH_TP5_SHIFT                     8
#define AIPS_PACRH_WP5_MASK                      0x200u
#define AIPS_PACRH_WP5_SHIFT                     9
#define AIPS_PACRH_SP5_MASK                      0x400u
#define AIPS_PACRH_SP5_SHIFT                     10
#define AIPS_PACRH_TP4_MASK                      0x1000u
#define AIPS_PACRH_TP4_SHIFT                     12
#define AIPS_PACRH_WP4_MASK                      0x2000u
#define AIPS_PACRH_WP4_SHIFT                     13
#define AIPS_PACRH_SP4_MASK                      0x4000u
#define AIPS_PACRH_SP4_SHIFT                     14
#define AIPS_PACRH_TP3_MASK                      0x10000u
#define AIPS_PACRH_TP3_SHIFT                     16
#define AIPS_PACRH_WP3_MASK                      0x20000u
#define AIPS_PACRH_WP3_SHIFT                     17
#define AIPS_PACRH_SP3_MASK                      0x40000u
#define AIPS_PACRH_SP3_SHIFT                     18
#define AIPS_PACRH_TP2_MASK                      0x100000u
#define AIPS_PACRH_TP2_SHIFT                     20
#define AIPS_PACRH_WP2_MASK                      0x200000u
#define AIPS_PACRH_WP2_SHIFT                     21
#define AIPS_PACRH_SP2_MASK                      0x400000u
#define AIPS_PACRH_SP2_SHIFT                     22
#define AIPS_PACRH_TP1_MASK                      0x1000000u
#define AIPS_PACRH_TP1_SHIFT                     24
#define AIPS_PACRH_WP1_MASK                      0x2000000u
#define AIPS_PACRH_WP1_SHIFT                     25
#define AIPS_PACRH_SP1_MASK                      0x4000000u
#define AIPS_PACRH_SP1_SHIFT                     26
#define AIPS_PACRH_TP0_MASK                      0x10000000u
#define AIPS_PACRH_TP0_SHIFT                     28
#define AIPS_PACRH_WP0_MASK                      0x20000000u
#define AIPS_PACRH_WP0_SHIFT                     29
#define AIPS_PACRH_SP0_MASK                      0x40000000u
#define AIPS_PACRH_SP0_SHIFT                     30
/* PACRI Bit Fields */
#define AIPS_PACRI_TP7_MASK                      0x1u
#define AIPS_PACRI_TP7_SHIFT                     0
#define AIPS_PACRI_WP7_MASK                      0x2u
#define AIPS_PACRI_WP7_SHIFT                     1
#define AIPS_PACRI_SP7_MASK                      0x4u
#define AIPS_PACRI_SP7_SHIFT                     2
#define AIPS_PACRI_TP6_MASK                      0x10u
#define AIPS_PACRI_TP6_SHIFT                     4
#define AIPS_PACRI_WP6_MASK                      0x20u
#define AIPS_PACRI_WP6_SHIFT                     5
#define AIPS_PACRI_SP6_MASK                      0x40u
#define AIPS_PACRI_SP6_SHIFT                     6
#define AIPS_PACRI_TP5_MASK                      0x100u
#define AIPS_PACRI_TP5_SHIFT                     8
#define AIPS_PACRI_WP5_MASK                      0x200u
#define AIPS_PACRI_WP5_SHIFT                     9
#define AIPS_PACRI_SP5_MASK                      0x400u
#define AIPS_PACRI_SP5_SHIFT                     10
#define AIPS_PACRI_TP4_MASK                      0x1000u
#define AIPS_PACRI_TP4_SHIFT                     12
#define AIPS_PACRI_WP4_MASK                      0x2000u
#define AIPS_PACRI_WP4_SHIFT                     13
#define AIPS_PACRI_SP4_MASK                      0x4000u
#define AIPS_PACRI_SP4_SHIFT                     14
#define AIPS_PACRI_TP3_MASK                      0x10000u
#define AIPS_PACRI_TP3_SHIFT                     16
#define AIPS_PACRI_WP3_MASK                      0x20000u
#define AIPS_PACRI_WP3_SHIFT                     17
#define AIPS_PACRI_SP3_MASK                      0x40000u
#define AIPS_PACRI_SP3_SHIFT                     18
#define AIPS_PACRI_TP2_MASK                      0x100000u
#define AIPS_PACRI_TP2_SHIFT                     20
#define AIPS_PACRI_WP2_MASK                      0x200000u
#define AIPS_PACRI_WP2_SHIFT                     21
#define AIPS_PACRI_SP2_MASK                      0x400000u
#define AIPS_PACRI_SP2_SHIFT                     22
#define AIPS_PACRI_TP1_MASK                      0x1000000u
#define AIPS_PACRI_TP1_SHIFT                     24
#define AIPS_PACRI_WP1_MASK                      0x2000000u
#define AIPS_PACRI_WP1_SHIFT                     25
#define AIPS_PACRI_SP1_MASK                      0x4000000u
#define AIPS_PACRI_SP1_SHIFT                     26
#define AIPS_PACRI_TP0_MASK                      0x10000000u
#define AIPS_PACRI_TP0_SHIFT                     28
#define AIPS_PACRI_WP0_MASK                      0x20000000u
#define AIPS_PACRI_WP0_SHIFT                     29
#define AIPS_PACRI_SP0_MASK                      0x40000000u
#define AIPS_PACRI_SP0_SHIFT                     30
/* PACRJ Bit Fields */
#define AIPS_PACRJ_TP7_MASK                      0x1u
#define AIPS_PACRJ_TP7_SHIFT                     0
#define AIPS_PACRJ_WP7_MASK                      0x2u
#define AIPS_PACRJ_WP7_SHIFT                     1
#define AIPS_PACRJ_SP7_MASK                      0x4u
#define AIPS_PACRJ_SP7_SHIFT                     2
#define AIPS_PACRJ_TP6_MASK                      0x10u
#define AIPS_PACRJ_TP6_SHIFT                     4
#define AIPS_PACRJ_WP6_MASK                      0x20u
#define AIPS_PACRJ_WP6_SHIFT                     5
#define AIPS_PACRJ_SP6_MASK                      0x40u
#define AIPS_PACRJ_SP6_SHIFT                     6
#define AIPS_PACRJ_TP5_MASK                      0x100u
#define AIPS_PACRJ_TP5_SHIFT                     8
#define AIPS_PACRJ_WP5_MASK                      0x200u
#define AIPS_PACRJ_WP5_SHIFT                     9
#define AIPS_PACRJ_SP5_MASK                      0x400u
#define AIPS_PACRJ_SP5_SHIFT                     10
#define AIPS_PACRJ_TP4_MASK                      0x1000u
#define AIPS_PACRJ_TP4_SHIFT                     12
#define AIPS_PACRJ_WP4_MASK                      0x2000u
#define AIPS_PACRJ_WP4_SHIFT                     13
#define AIPS_PACRJ_SP4_MASK                      0x4000u
#define AIPS_PACRJ_SP4_SHIFT                     14
#define AIPS_PACRJ_TP3_MASK                      0x10000u
#define AIPS_PACRJ_TP3_SHIFT                     16
#define AIPS_PACRJ_WP3_MASK                      0x20000u
#define AIPS_PACRJ_WP3_SHIFT                     17
#define AIPS_PACRJ_SP3_MASK                      0x40000u
#define AIPS_PACRJ_SP3_SHIFT                     18
#define AIPS_PACRJ_TP2_MASK                      0x100000u
#define AIPS_PACRJ_TP2_SHIFT                     20
#define AIPS_PACRJ_WP2_MASK                      0x200000u
#define AIPS_PACRJ_WP2_SHIFT                     21
#define AIPS_PACRJ_SP2_MASK                      0x400000u
#define AIPS_PACRJ_SP2_SHIFT                     22
#define AIPS_PACRJ_TP1_MASK                      0x1000000u
#define AIPS_PACRJ_TP1_SHIFT                     24
#define AIPS_PACRJ_WP1_MASK                      0x2000000u
#define AIPS_PACRJ_WP1_SHIFT                     25
#define AIPS_PACRJ_SP1_MASK                      0x4000000u
#define AIPS_PACRJ_SP1_SHIFT                     26
#define AIPS_PACRJ_TP0_MASK                      0x10000000u
#define AIPS_PACRJ_TP0_SHIFT                     28
#define AIPS_PACRJ_WP0_MASK                      0x20000000u
#define AIPS_PACRJ_WP0_SHIFT                     29
#define AIPS_PACRJ_SP0_MASK                      0x40000000u
#define AIPS_PACRJ_SP0_SHIFT                     30
/* PACRK Bit Fields */
#define AIPS_PACRK_TP7_MASK                      0x1u
#define AIPS_PACRK_TP7_SHIFT                     0
#define AIPS_PACRK_WP7_MASK                      0x2u
#define AIPS_PACRK_WP7_SHIFT                     1
#define AIPS_PACRK_SP7_MASK                      0x4u
#define AIPS_PACRK_SP7_SHIFT                     2
#define AIPS_PACRK_TP6_MASK                      0x10u
#define AIPS_PACRK_TP6_SHIFT                     4
#define AIPS_PACRK_WP6_MASK                      0x20u
#define AIPS_PACRK_WP6_SHIFT                     5
#define AIPS_PACRK_SP6_MASK                      0x40u
#define AIPS_PACRK_SP6_SHIFT                     6
#define AIPS_PACRK_TP5_MASK                      0x100u
#define AIPS_PACRK_TP5_SHIFT                     8
#define AIPS_PACRK_WP5_MASK                      0x200u
#define AIPS_PACRK_WP5_SHIFT                     9
#define AIPS_PACRK_SP5_MASK                      0x400u
#define AIPS_PACRK_SP5_SHIFT                     10
#define AIPS_PACRK_TP4_MASK                      0x1000u
#define AIPS_PACRK_TP4_SHIFT                     12
#define AIPS_PACRK_WP4_MASK                      0x2000u
#define AIPS_PACRK_WP4_SHIFT                     13
#define AIPS_PACRK_SP4_MASK                      0x4000u
#define AIPS_PACRK_SP4_SHIFT                     14
#define AIPS_PACRK_TP3_MASK                      0x10000u
#define AIPS_PACRK_TP3_SHIFT                     16
#define AIPS_PACRK_WP3_MASK                      0x20000u
#define AIPS_PACRK_WP3_SHIFT                     17
#define AIPS_PACRK_SP3_MASK                      0x40000u
#define AIPS_PACRK_SP3_SHIFT                     18
#define AIPS_PACRK_TP2_MASK                      0x100000u
#define AIPS_PACRK_TP2_SHIFT                     20
#define AIPS_PACRK_WP2_MASK                      0x200000u
#define AIPS_PACRK_WP2_SHIFT                     21
#define AIPS_PACRK_SP2_MASK                      0x400000u
#define AIPS_PACRK_SP2_SHIFT                     22
#define AIPS_PACRK_TP1_MASK                      0x1000000u
#define AIPS_PACRK_TP1_SHIFT                     24
#define AIPS_PACRK_WP1_MASK                      0x2000000u
#define AIPS_PACRK_WP1_SHIFT                     25
#define AIPS_PACRK_SP1_MASK                      0x4000000u
#define AIPS_PACRK_SP1_SHIFT                     26
#define AIPS_PACRK_TP0_MASK                      0x10000000u
#define AIPS_PACRK_TP0_SHIFT                     28
#define AIPS_PACRK_WP0_MASK                      0x20000000u
#define AIPS_PACRK_WP0_SHIFT                     29
#define AIPS_PACRK_SP0_MASK                      0x40000000u
#define AIPS_PACRK_SP0_SHIFT                     30
/* PACRL Bit Fields */
#define AIPS_PACRL_TP7_MASK                      0x1u
#define AIPS_PACRL_TP7_SHIFT                     0
#define AIPS_PACRL_WP7_MASK                      0x2u
#define AIPS_PACRL_WP7_SHIFT                     1
#define AIPS_PACRL_SP7_MASK                      0x4u
#define AIPS_PACRL_SP7_SHIFT                     2
#define AIPS_PACRL_TP6_MASK                      0x10u
#define AIPS_PACRL_TP6_SHIFT                     4
#define AIPS_PACRL_WP6_MASK                      0x20u
#define AIPS_PACRL_WP6_SHIFT                     5
#define AIPS_PACRL_SP6_MASK                      0x40u
#define AIPS_PACRL_SP6_SHIFT                     6
#define AIPS_PACRL_TP5_MASK                      0x100u
#define AIPS_PACRL_TP5_SHIFT                     8
#define AIPS_PACRL_WP5_MASK                      0x200u
#define AIPS_PACRL_WP5_SHIFT                     9
#define AIPS_PACRL_SP5_MASK                      0x400u
#define AIPS_PACRL_SP5_SHIFT                     10
#define AIPS_PACRL_TP4_MASK                      0x1000u
#define AIPS_PACRL_TP4_SHIFT                     12
#define AIPS_PACRL_WP4_MASK                      0x2000u
#define AIPS_PACRL_WP4_SHIFT                     13
#define AIPS_PACRL_SP4_MASK                      0x4000u
#define AIPS_PACRL_SP4_SHIFT                     14
#define AIPS_PACRL_TP3_MASK                      0x10000u
#define AIPS_PACRL_TP3_SHIFT                     16
#define AIPS_PACRL_WP3_MASK                      0x20000u
#define AIPS_PACRL_WP3_SHIFT                     17
#define AIPS_PACRL_SP3_MASK                      0x40000u
#define AIPS_PACRL_SP3_SHIFT                     18
#define AIPS_PACRL_TP2_MASK                      0x100000u
#define AIPS_PACRL_TP2_SHIFT                     20
#define AIPS_PACRL_WP2_MASK                      0x200000u
#define AIPS_PACRL_WP2_SHIFT                     21
#define AIPS_PACRL_SP2_MASK                      0x400000u
#define AIPS_PACRL_SP2_SHIFT                     22
#define AIPS_PACRL_TP1_MASK                      0x1000000u
#define AIPS_PACRL_TP1_SHIFT                     24
#define AIPS_PACRL_WP1_MASK                      0x2000000u
#define AIPS_PACRL_WP1_SHIFT                     25
#define AIPS_PACRL_SP1_MASK                      0x4000000u
#define AIPS_PACRL_SP1_SHIFT                     26
#define AIPS_PACRL_TP0_MASK                      0x10000000u
#define AIPS_PACRL_TP0_SHIFT                     28
#define AIPS_PACRL_WP0_MASK                      0x20000000u
#define AIPS_PACRL_WP0_SHIFT                     29
#define AIPS_PACRL_SP0_MASK                      0x40000000u
#define AIPS_PACRL_SP0_SHIFT                     30
/* PACRM Bit Fields */
#define AIPS_PACRM_TP7_MASK                      0x1u
#define AIPS_PACRM_TP7_SHIFT                     0
#define AIPS_PACRM_WP7_MASK                      0x2u
#define AIPS_PACRM_WP7_SHIFT                     1
#define AIPS_PACRM_SP7_MASK                      0x4u
#define AIPS_PACRM_SP7_SHIFT                     2
#define AIPS_PACRM_TP6_MASK                      0x10u
#define AIPS_PACRM_TP6_SHIFT                     4
#define AIPS_PACRM_WP6_MASK                      0x20u
#define AIPS_PACRM_WP6_SHIFT                     5
#define AIPS_PACRM_SP6_MASK                      0x40u
#define AIPS_PACRM_SP6_SHIFT                     6
#define AIPS_PACRM_TP5_MASK                      0x100u
#define AIPS_PACRM_TP5_SHIFT                     8
#define AIPS_PACRM_WP5_MASK                      0x200u
#define AIPS_PACRM_WP5_SHIFT                     9
#define AIPS_PACRM_SP5_MASK                      0x400u
#define AIPS_PACRM_SP5_SHIFT                     10
#define AIPS_PACRM_TP4_MASK                      0x1000u
#define AIPS_PACRM_TP4_SHIFT                     12
#define AIPS_PACRM_WP4_MASK                      0x2000u
#define AIPS_PACRM_WP4_SHIFT                     13
#define AIPS_PACRM_SP4_MASK                      0x4000u
#define AIPS_PACRM_SP4_SHIFT                     14
#define AIPS_PACRM_TP3_MASK                      0x10000u
#define AIPS_PACRM_TP3_SHIFT                     16
#define AIPS_PACRM_WP3_MASK                      0x20000u
#define AIPS_PACRM_WP3_SHIFT                     17
#define AIPS_PACRM_SP3_MASK                      0x40000u
#define AIPS_PACRM_SP3_SHIFT                     18
#define AIPS_PACRM_TP2_MASK                      0x100000u
#define AIPS_PACRM_TP2_SHIFT                     20
#define AIPS_PACRM_WP2_MASK                      0x200000u
#define AIPS_PACRM_WP2_SHIFT                     21
#define AIPS_PACRM_SP2_MASK                      0x400000u
#define AIPS_PACRM_SP2_SHIFT                     22
#define AIPS_PACRM_TP1_MASK                      0x1000000u
#define AIPS_PACRM_TP1_SHIFT                     24
#define AIPS_PACRM_WP1_MASK                      0x2000000u
#define AIPS_PACRM_WP1_SHIFT                     25
#define AIPS_PACRM_SP1_MASK                      0x4000000u
#define AIPS_PACRM_SP1_SHIFT                     26
#define AIPS_PACRM_TP0_MASK                      0x10000000u
#define AIPS_PACRM_TP0_SHIFT                     28
#define AIPS_PACRM_WP0_MASK                      0x20000000u
#define AIPS_PACRM_WP0_SHIFT                     29
#define AIPS_PACRM_SP0_MASK                      0x40000000u
#define AIPS_PACRM_SP0_SHIFT                     30
/* PACRN Bit Fields */
#define AIPS_PACRN_TP7_MASK                      0x1u
#define AIPS_PACRN_TP7_SHIFT                     0
#define AIPS_PACRN_WP7_MASK                      0x2u
#define AIPS_PACRN_WP7_SHIFT                     1
#define AIPS_PACRN_SP7_MASK                      0x4u
#define AIPS_PACRN_SP7_SHIFT                     2
#define AIPS_PACRN_TP6_MASK                      0x10u
#define AIPS_PACRN_TP6_SHIFT                     4
#define AIPS_PACRN_WP6_MASK                      0x20u
#define AIPS_PACRN_WP6_SHIFT                     5
#define AIPS_PACRN_SP6_MASK                      0x40u
#define AIPS_PACRN_SP6_SHIFT                     6
#define AIPS_PACRN_TP5_MASK                      0x100u
#define AIPS_PACRN_TP5_SHIFT                     8
#define AIPS_PACRN_WP5_MASK                      0x200u
#define AIPS_PACRN_WP5_SHIFT                     9
#define AIPS_PACRN_SP5_MASK                      0x400u
#define AIPS_PACRN_SP5_SHIFT                     10
#define AIPS_PACRN_TP4_MASK                      0x1000u
#define AIPS_PACRN_TP4_SHIFT                     12
#define AIPS_PACRN_WP4_MASK                      0x2000u
#define AIPS_PACRN_WP4_SHIFT                     13
#define AIPS_PACRN_SP4_MASK                      0x4000u
#define AIPS_PACRN_SP4_SHIFT                     14
#define AIPS_PACRN_TP3_MASK                      0x10000u
#define AIPS_PACRN_TP3_SHIFT                     16
#define AIPS_PACRN_WP3_MASK                      0x20000u
#define AIPS_PACRN_WP3_SHIFT                     17
#define AIPS_PACRN_SP3_MASK                      0x40000u
#define AIPS_PACRN_SP3_SHIFT                     18
#define AIPS_PACRN_TP2_MASK                      0x100000u
#define AIPS_PACRN_TP2_SHIFT                     20
#define AIPS_PACRN_WP2_MASK                      0x200000u
#define AIPS_PACRN_WP2_SHIFT                     21
#define AIPS_PACRN_SP2_MASK                      0x400000u
#define AIPS_PACRN_SP2_SHIFT                     22
#define AIPS_PACRN_TP1_MASK                      0x1000000u
#define AIPS_PACRN_TP1_SHIFT                     24
#define AIPS_PACRN_WP1_MASK                      0x2000000u
#define AIPS_PACRN_WP1_SHIFT                     25
#define AIPS_PACRN_SP1_MASK                      0x4000000u
#define AIPS_PACRN_SP1_SHIFT                     26
#define AIPS_PACRN_TP0_MASK                      0x10000000u
#define AIPS_PACRN_TP0_SHIFT                     28
#define AIPS_PACRN_WP0_MASK                      0x20000000u
#define AIPS_PACRN_WP0_SHIFT                     29
#define AIPS_PACRN_SP0_MASK                      0x40000000u
#define AIPS_PACRN_SP0_SHIFT                     30
/* PACRO Bit Fields */
#define AIPS_PACRO_TP7_MASK                      0x1u
#define AIPS_PACRO_TP7_SHIFT                     0
#define AIPS_PACRO_WP7_MASK                      0x2u
#define AIPS_PACRO_WP7_SHIFT                     1
#define AIPS_PACRO_SP7_MASK                      0x4u
#define AIPS_PACRO_SP7_SHIFT                     2
#define AIPS_PACRO_TP6_MASK                      0x10u
#define AIPS_PACRO_TP6_SHIFT                     4
#define AIPS_PACRO_WP6_MASK                      0x20u
#define AIPS_PACRO_WP6_SHIFT                     5
#define AIPS_PACRO_SP6_MASK                      0x40u
#define AIPS_PACRO_SP6_SHIFT                     6
#define AIPS_PACRO_TP5_MASK                      0x100u
#define AIPS_PACRO_TP5_SHIFT                     8
#define AIPS_PACRO_WP5_MASK                      0x200u
#define AIPS_PACRO_WP5_SHIFT                     9
#define AIPS_PACRO_SP5_MASK                      0x400u
#define AIPS_PACRO_SP5_SHIFT                     10
#define AIPS_PACRO_TP4_MASK                      0x1000u
#define AIPS_PACRO_TP4_SHIFT                     12
#define AIPS_PACRO_WP4_MASK                      0x2000u
#define AIPS_PACRO_WP4_SHIFT                     13
#define AIPS_PACRO_SP4_MASK                      0x4000u
#define AIPS_PACRO_SP4_SHIFT                     14
#define AIPS_PACRO_TP3_MASK                      0x10000u
#define AIPS_PACRO_TP3_SHIFT                     16
#define AIPS_PACRO_WP3_MASK                      0x20000u
#define AIPS_PACRO_WP3_SHIFT                     17
#define AIPS_PACRO_SP3_MASK                      0x40000u
#define AIPS_PACRO_SP3_SHIFT                     18
#define AIPS_PACRO_TP2_MASK                      0x100000u
#define AIPS_PACRO_TP2_SHIFT                     20
#define AIPS_PACRO_WP2_MASK                      0x200000u
#define AIPS_PACRO_WP2_SHIFT                     21
#define AIPS_PACRO_SP2_MASK                      0x400000u
#define AIPS_PACRO_SP2_SHIFT                     22
#define AIPS_PACRO_TP1_MASK                      0x1000000u
#define AIPS_PACRO_TP1_SHIFT                     24
#define AIPS_PACRO_WP1_MASK                      0x2000000u
#define AIPS_PACRO_WP1_SHIFT                     25
#define AIPS_PACRO_SP1_MASK                      0x4000000u
#define AIPS_PACRO_SP1_SHIFT                     26
#define AIPS_PACRO_TP0_MASK                      0x10000000u
#define AIPS_PACRO_TP0_SHIFT                     28
#define AIPS_PACRO_WP0_MASK                      0x20000000u
#define AIPS_PACRO_WP0_SHIFT                     29
#define AIPS_PACRO_SP0_MASK                      0x40000000u
#define AIPS_PACRO_SP0_SHIFT                     30
/* PACRP Bit Fields */
#define AIPS_PACRP_TP7_MASK                      0x1u
#define AIPS_PACRP_TP7_SHIFT                     0
#define AIPS_PACRP_WP7_MASK                      0x2u
#define AIPS_PACRP_WP7_SHIFT                     1
#define AIPS_PACRP_SP7_MASK                      0x4u
#define AIPS_PACRP_SP7_SHIFT                     2
#define AIPS_PACRP_TP6_MASK                      0x10u
#define AIPS_PACRP_TP6_SHIFT                     4
#define AIPS_PACRP_WP6_MASK                      0x20u
#define AIPS_PACRP_WP6_SHIFT                     5
#define AIPS_PACRP_SP6_MASK                      0x40u
#define AIPS_PACRP_SP6_SHIFT                     6
#define AIPS_PACRP_TP5_MASK                      0x100u
#define AIPS_PACRP_TP5_SHIFT                     8
#define AIPS_PACRP_WP5_MASK                      0x200u
#define AIPS_PACRP_WP5_SHIFT                     9
#define AIPS_PACRP_SP5_MASK                      0x400u
#define AIPS_PACRP_SP5_SHIFT                     10
#define AIPS_PACRP_TP4_MASK                      0x1000u
#define AIPS_PACRP_TP4_SHIFT                     12
#define AIPS_PACRP_WP4_MASK                      0x2000u
#define AIPS_PACRP_WP4_SHIFT                     13
#define AIPS_PACRP_SP4_MASK                      0x4000u
#define AIPS_PACRP_SP4_SHIFT                     14
#define AIPS_PACRP_TP3_MASK                      0x10000u
#define AIPS_PACRP_TP3_SHIFT                     16
#define AIPS_PACRP_WP3_MASK                      0x20000u
#define AIPS_PACRP_WP3_SHIFT                     17
#define AIPS_PACRP_SP3_MASK                      0x40000u
#define AIPS_PACRP_SP3_SHIFT                     18
#define AIPS_PACRP_TP2_MASK                      0x100000u
#define AIPS_PACRP_TP2_SHIFT                     20
#define AIPS_PACRP_WP2_MASK                      0x200000u
#define AIPS_PACRP_WP2_SHIFT                     21
#define AIPS_PACRP_SP2_MASK                      0x400000u
#define AIPS_PACRP_SP2_SHIFT                     22
#define AIPS_PACRP_TP1_MASK                      0x1000000u
#define AIPS_PACRP_TP1_SHIFT                     24
#define AIPS_PACRP_WP1_MASK                      0x2000000u
#define AIPS_PACRP_WP1_SHIFT                     25
#define AIPS_PACRP_SP1_MASK                      0x4000000u
#define AIPS_PACRP_SP1_SHIFT                     26
#define AIPS_PACRP_TP0_MASK                      0x10000000u
#define AIPS_PACRP_TP0_SHIFT                     28
#define AIPS_PACRP_WP0_MASK                      0x20000000u
#define AIPS_PACRP_WP0_SHIFT                     29
#define AIPS_PACRP_SP0_MASK                      0x40000000u
#define AIPS_PACRP_SP0_SHIFT                     30

/**
 * @}
 */ /* end of group AIPS_Register_Masks */


/* AIPS - Peripheral instance base addresses */
/** Peripheral AIPS0 base address */
#define AIPS0_BASE                               (0x40000000u)
/** Peripheral AIPS0 base pointer */
#define AIPS0                                    ((AIPS_Type *)AIPS0_BASE)
/** Peripheral AIPS1 base address */
#define AIPS1_BASE                               (0x40080000u)
/** Peripheral AIPS1 base pointer */
#define AIPS1                                    ((AIPS_Type *)AIPS1_BASE)
/** Array initializer of AIPS peripheral base pointers */
#define AIPS_BASES                               { AIPS0, AIPS1 }

/**
 * @}
 */ /* end of group AIPS_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- AXBS Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup AXBS_Peripheral_Access_Layer AXBS Peripheral Access Layer
 * @{
 */

/** AXBS - Register Layout Typedef */
typedef struct {
  struct {                                         /* offset: 0x0, array step: 0x100 */
    __IO uint32_t PRS;                               /**< Priority Registers Slave, array offset: 0x0, array step: 0x100 */
         uint8_t RESERVED_0[12];
    __IO uint32_t CRS;                               /**< Control Register, array offset: 0x10, array step: 0x100 */
         uint8_t RESERVED_1[236];
  } SLAVE[5];
       uint8_t RESERVED_0[768];
  __IO uint32_t MGPCR0;                            /**< Master General Purpose Control Register, offset: 0x800 */
       uint8_t RESERVED_1[252];
  __IO uint32_t MGPCR1;                            /**< Master General Purpose Control Register, offset: 0x900 */
       uint8_t RESERVED_2[252];
  __IO uint32_t MGPCR2;                            /**< Master General Purpose Control Register, offset: 0xA00 */
       uint8_t RESERVED_3[252];
  __IO uint32_t MGPCR3;                            /**< Master General Purpose Control Register, offset: 0xB00 */
       uint8_t RESERVED_4[252];
  __IO uint32_t MGPCR4;                            /**< Master General Purpose Control Register, offset: 0xC00 */
       uint8_t RESERVED_5[252];
  __IO uint32_t MGPCR5;                            /**< Master General Purpose Control Register, offset: 0xD00 */
} AXBS_Type;

/* ----------------------------------------------------------------------------
   -- AXBS Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup AXBS_Register_Masks AXBS Register Masks
 * @{
 */

/* PRS Bit Fields */
#define AXBS_PRS_M0_MASK                         0x7u
#define AXBS_PRS_M0_SHIFT                        0
#define AXBS_PRS_M0(x)                           (((uint32_t)(((uint32_t)(x))<<AXBS_PRS_M0_SHIFT))&AXBS_PRS_M0_MASK)
#define AXBS_PRS_M1_MASK                         0x70u
#define AXBS_PRS_M1_SHIFT                        4
#define AXBS_PRS_M1(x)                           (((uint32_t)(((uint32_t)(x))<<AXBS_PRS_M1_SHIFT))&AXBS_PRS_M1_MASK)
#define AXBS_PRS_M2_MASK                         0x700u
#define AXBS_PRS_M2_SHIFT                        8
#define AXBS_PRS_M2(x)                           (((uint32_t)(((uint32_t)(x))<<AXBS_PRS_M2_SHIFT))&AXBS_PRS_M2_MASK)
#define AXBS_PRS_M3_MASK                         0x7000u
#define AXBS_PRS_M3_SHIFT                        12
#define AXBS_PRS_M3(x)                           (((uint32_t)(((uint32_t)(x))<<AXBS_PRS_M3_SHIFT))&AXBS_PRS_M3_MASK)
#define AXBS_PRS_M4_MASK                         0x70000u
#define AXBS_PRS_M4_SHIFT                        16
#define AXBS_PRS_M4(x)                           (((uint32_t)(((uint32_t)(x))<<AXBS_PRS_M4_SHIFT))&AXBS_PRS_M4_MASK)
#define AXBS_PRS_M5_MASK                         0x700000u
#define AXBS_PRS_M5_SHIFT                        20
#define AXBS_PRS_M5(x)                           (((uint32_t)(((uint32_t)(x))<<AXBS_PRS_M5_SHIFT))&AXBS_PRS_M5_MASK)
/* CRS Bit Fields */
#define AXBS_CRS_PARK_MASK                       0x7u
#define AXBS_CRS_PARK_SHIFT                      0
#define AXBS_CRS_PARK(x)                         (((uint32_t)(((uint32_t)(x))<<AXBS_CRS_PARK_SHIFT))&AXBS_CRS_PARK_MASK)
#define AXBS_CRS_PCTL_MASK                       0x30u
#define AXBS_CRS_PCTL_SHIFT                      4
#define AXBS_CRS_PCTL(x)                         (((uint32_t)(((uint32_t)(x))<<AXBS_CRS_PCTL_SHIFT))&AXBS_CRS_PCTL_MASK)
#define AXBS_CRS_ARB_MASK                        0x300u
#define AXBS_CRS_ARB_SHIFT                       8
#define AXBS_CRS_ARB(x)                          (((uint32_t)(((uint32_t)(x))<<AXBS_CRS_ARB_SHIFT))&AXBS_CRS_ARB_MASK)
#define AXBS_CRS_HLP_MASK                        0x40000000u
#define AXBS_CRS_HLP_SHIFT                       30
#define AXBS_CRS_RO_MASK                         0x80000000u
#define AXBS_CRS_RO_SHIFT                        31
/* MGPCR0 Bit Fields */
#define AXBS_MGPCR0_AULB_MASK                    0x7u
#define AXBS_MGPCR0_AULB_SHIFT                   0
#define AXBS_MGPCR0_AULB(x)                      (((uint32_t)(((uint32_t)(x))<<AXBS_MGPCR0_AULB_SHIFT))&AXBS_MGPCR0_AULB_MASK)
/* MGPCR1 Bit Fields */
#define AXBS_MGPCR1_AULB_MASK                    0x7u
#define AXBS_MGPCR1_AULB_SHIFT                   0
#define AXBS_MGPCR1_AULB(x)                      (((uint32_t)(((uint32_t)(x))<<AXBS_MGPCR1_AULB_SHIFT))&AXBS_MGPCR1_AULB_MASK)
/* MGPCR2 Bit Fields */
#define AXBS_MGPCR2_AULB_MASK                    0x7u
#define AXBS_MGPCR2_AULB_SHIFT                   0
#define AXBS_MGPCR2_AULB(x)                      (((uint32_t)(((uint32_t)(x))<<AXBS_MGPCR2_AULB_SHIFT))&AXBS_MGPCR2_AULB_MASK)
/* MGPCR3 Bit Fields */
#define AXBS_MGPCR3_AULB_MASK                    0x7u
#define AXBS_MGPCR3_AULB_SHIFT                   0
#define AXBS_MGPCR3_AULB(x)                      (((uint32_t)(((uint32_t)(x))<<AXBS_MGPCR3_AULB_SHIFT))&AXBS_MGPCR3_AULB_MASK)
/* MGPCR4 Bit Fields */
#define AXBS_MGPCR4_AULB_MASK                    0x7u
#define AXBS_MGPCR4_AULB_SHIFT                   0
#define AXBS_MGPCR4_AULB(x)                      (((uint32_t)(((uint32_t)(x))<<AXBS_MGPCR4_AULB_SHIFT))&AXBS_MGPCR4_AULB_MASK)
/* MGPCR5 Bit Fields */
#define AXBS_MGPCR5_AULB_MASK                    0x7u
#define AXBS_MGPCR5_AULB_SHIFT                   0
#define AXBS_MGPCR5_AULB(x)                      (((uint32_t)(((uint32_t)(x))<<AXBS_MGPCR5_AULB_SHIFT))&AXBS_MGPCR5_AULB_MASK)

/**
 * @}
 */ /* end of group AXBS_Register_Masks */


/* AXBS - Peripheral instance base addresses */
/** Peripheral AXBS base address */
#define AXBS_BASE                                (0x40004000u)
/** Peripheral AXBS base pointer */
#define AXBS                                     ((AXBS_Type *)AXBS_BASE)
/** Array initializer of AXBS peripheral base pointers */
#define AXBS_BASES                               { AXBS }

/**
 * @}
 */ /* end of group AXBS_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- CAN Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup CAN_Peripheral_Access_Layer CAN Peripheral Access Layer
 * @{
 */

/** CAN - Register Layout Typedef */
typedef struct {
  __IO uint32_t MCR;                               /**< Module Configuration Register, offset: 0x0 */
  __IO uint32_t CTRL1;                             /**< Control 1 register, offset: 0x4 */
  __IO uint32_t TIMER;                             /**< Free Running Timer, offset: 0x8 */
       uint8_t RESERVED_0[4];
  __IO uint32_t RXMGMASK;                          /**< Rx Mailboxes Global Mask Register, offset: 0x10 */
  __IO uint32_t RX14MASK;                          /**< Rx 14 Mask register, offset: 0x14 */
  __IO uint32_t RX15MASK;                          /**< Rx 15 Mask register, offset: 0x18 */
  __IO uint32_t ECR;                               /**< Error Counter, offset: 0x1C */
  __IO uint32_t ESR1;                              /**< Error and Status 1 register, offset: 0x20 */
       uint8_t RESERVED_1[4];
  __IO uint32_t IMASK1;                            /**< Interrupt Masks 1 register, offset: 0x28 */
       uint8_t RESERVED_2[4];
  __IO uint32_t IFLAG1;                            /**< Interrupt Flags 1 register, offset: 0x30 */
  __IO uint32_t CTRL2;                             /**< Control 2 register, offset: 0x34 */
  __I  uint32_t ESR2;                              /**< Error and Status 2 register, offset: 0x38 */
       uint8_t RESERVED_3[8];
  __I  uint32_t CRCR;                              /**< CRC Register, offset: 0x44 */
  __IO uint32_t RXFGMASK;                          /**< Rx FIFO Global Mask register, offset: 0x48 */
  __I  uint32_t RXFIR;                             /**< Rx FIFO Information Register, offset: 0x4C */
       uint8_t RESERVED_4[48];
  struct {                                         /* offset: 0x80, array step: 0x10 */
    __IO uint32_t CS;                                /**< Message Buffer 0 CS Register..Message Buffer 15 CS Register, array offset: 0x80, array step: 0x10 */
    __IO uint32_t ID;                                /**< Message Buffer 0 ID Register..Message Buffer 15 ID Register, array offset: 0x84, array step: 0x10 */
    __IO uint32_t WORD0;                             /**< Message Buffer 0 WORD0 Register..Message Buffer 15 WORD0 Register, array offset: 0x88, array step: 0x10 */
    __IO uint32_t WORD1;                             /**< Message Buffer 0 WORD1 Register..Message Buffer 15 WORD1 Register, array offset: 0x8C, array step: 0x10 */
  } MB[16];
       uint8_t RESERVED_5[1792];
  __IO uint32_t RXIMR[16];                         /**< Rx Individual Mask Registers, array offset: 0x880, array step: 0x4 */
} CAN_Type;

/* ----------------------------------------------------------------------------
   -- CAN Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup CAN_Register_Masks CAN Register Masks
 * @{
 */

/* MCR Bit Fields */
#define CAN_MCR_MAXMB_MASK                       0x7Fu
#define CAN_MCR_MAXMB_SHIFT                      0
#define CAN_MCR_MAXMB(x)                         (((uint32_t)(((uint32_t)(x))<<CAN_MCR_MAXMB_SHIFT))&CAN_MCR_MAXMB_MASK)
#define CAN_MCR_IDAM_MASK                        0x300u
#define CAN_MCR_IDAM_SHIFT                       8
#define CAN_MCR_IDAM(x)                          (((uint32_t)(((uint32_t)(x))<<CAN_MCR_IDAM_SHIFT))&CAN_MCR_IDAM_MASK)
#define CAN_MCR_AEN_MASK                         0x1000u
#define CAN_MCR_AEN_SHIFT                        12
#define CAN_MCR_LPRIOEN_MASK                     0x2000u
#define CAN_MCR_LPRIOEN_SHIFT                    13
#define CAN_MCR_IRMQ_MASK                        0x10000u
#define CAN_MCR_IRMQ_SHIFT                       16
#define CAN_MCR_SRXDIS_MASK                      0x20000u
#define CAN_MCR_SRXDIS_SHIFT                     17
#define CAN_MCR_WAKSRC_MASK                      0x80000u
#define CAN_MCR_WAKSRC_SHIFT                     19
#define CAN_MCR_LPMACK_MASK                      0x100000u
#define CAN_MCR_LPMACK_SHIFT                     20
#define CAN_MCR_WRNEN_MASK                       0x200000u
#define CAN_MCR_WRNEN_SHIFT                      21
#define CAN_MCR_SLFWAK_MASK                      0x400000u
#define CAN_MCR_SLFWAK_SHIFT                     22
#define CAN_MCR_SUPV_MASK                        0x800000u
#define CAN_MCR_SUPV_SHIFT                       23
#define CAN_MCR_FRZACK_MASK                      0x1000000u
#define CAN_MCR_FRZACK_SHIFT                     24
#define CAN_MCR_SOFTRST_MASK                     0x2000000u
#define CAN_MCR_SOFTRST_SHIFT                    25
#define CAN_MCR_WAKMSK_MASK                      0x4000000u
#define CAN_MCR_WAKMSK_SHIFT                     26
#define CAN_MCR_NOTRDY_MASK                      0x8000000u
#define CAN_MCR_NOTRDY_SHIFT                     27
#define CAN_MCR_HALT_MASK                        0x10000000u
#define CAN_MCR_HALT_SHIFT                       28
#define CAN_MCR_RFEN_MASK                        0x20000000u
#define CAN_MCR_RFEN_SHIFT                       29
#define CAN_MCR_FRZ_MASK                         0x40000000u
#define CAN_MCR_FRZ_SHIFT                        30
#define CAN_MCR_MDIS_MASK                        0x80000000u
#define CAN_MCR_MDIS_SHIFT                       31
/* CTRL1 Bit Fields */
#define CAN_CTRL1_PROPSEG_MASK                   0x7u
#define CAN_CTRL1_PROPSEG_SHIFT                  0
#define CAN_CTRL1_PROPSEG(x)                     (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_PROPSEG_SHIFT))&CAN_CTRL1_PROPSEG_MASK)
#define CAN_CTRL1_LOM_MASK                       0x8u
#define CAN_CTRL1_LOM_SHIFT                      3
#define CAN_CTRL1_LBUF_MASK                      0x10u
#define CAN_CTRL1_LBUF_SHIFT                     4
#define CAN_CTRL1_TSYN_MASK                      0x20u
#define CAN_CTRL1_TSYN_SHIFT                     5
#define CAN_CTRL1_BOFFREC_MASK                   0x40u
#define CAN_CTRL1_BOFFREC_SHIFT                  6
#define CAN_CTRL1_SMP_MASK                       0x80u
#define CAN_CTRL1_SMP_SHIFT                      7
#define CAN_CTRL1_RWRNMSK_MASK                   0x400u
#define CAN_CTRL1_RWRNMSK_SHIFT                  10
#define CAN_CTRL1_TWRNMSK_MASK                   0x800u
#define CAN_CTRL1_TWRNMSK_SHIFT                  11
#define CAN_CTRL1_LPB_MASK                       0x1000u
#define CAN_CTRL1_LPB_SHIFT                      12
#define CAN_CTRL1_CLKSRC_MASK                    0x2000u
#define CAN_CTRL1_CLKSRC_SHIFT                   13
#define CAN_CTRL1_ERRMSK_MASK                    0x4000u
#define CAN_CTRL1_ERRMSK_SHIFT                   14
#define CAN_CTRL1_BOFFMSK_MASK                   0x8000u
#define CAN_CTRL1_BOFFMSK_SHIFT                  15
#define CAN_CTRL1_PSEG2_MASK                     0x70000u
#define CAN_CTRL1_PSEG2_SHIFT                    16
#define CAN_CTRL1_PSEG2(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_PSEG2_SHIFT))&CAN_CTRL1_PSEG2_MASK)
#define CAN_CTRL1_PSEG1_MASK                     0x380000u
#define CAN_CTRL1_PSEG1_SHIFT                    19
#define CAN_CTRL1_PSEG1(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_PSEG1_SHIFT))&CAN_CTRL1_PSEG1_MASK)
#define CAN_CTRL1_RJW_MASK                       0xC00000u
#define CAN_CTRL1_RJW_SHIFT                      22
#define CAN_CTRL1_RJW(x)                         (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_RJW_SHIFT))&CAN_CTRL1_RJW_MASK)
#define CAN_CTRL1_PRESDIV_MASK                   0xFF000000u
#define CAN_CTRL1_PRESDIV_SHIFT                  24
#define CAN_CTRL1_PRESDIV(x)                     (((uint32_t)(((uint32_t)(x))<<CAN_CTRL1_PRESDIV_SHIFT))&CAN_CTRL1_PRESDIV_MASK)
/* TIMER Bit Fields */
#define CAN_TIMER_TIMER_MASK                     0xFFFFu
#define CAN_TIMER_TIMER_SHIFT                    0
#define CAN_TIMER_TIMER(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_TIMER_TIMER_SHIFT))&CAN_TIMER_TIMER_MASK)
/* RXMGMASK Bit Fields */
#define CAN_RXMGMASK_MG_MASK                     0xFFFFFFFFu
#define CAN_RXMGMASK_MG_SHIFT                    0
#define CAN_RXMGMASK_MG(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_RXMGMASK_MG_SHIFT))&CAN_RXMGMASK_MG_MASK)
/* RX14MASK Bit Fields */
#define CAN_RX14MASK_RX14M_MASK                  0xFFFFFFFFu
#define CAN_RX14MASK_RX14M_SHIFT                 0
#define CAN_RX14MASK_RX14M(x)                    (((uint32_t)(((uint32_t)(x))<<CAN_RX14MASK_RX14M_SHIFT))&CAN_RX14MASK_RX14M_MASK)
/* RX15MASK Bit Fields */
#define CAN_RX15MASK_RX15M_MASK                  0xFFFFFFFFu
#define CAN_RX15MASK_RX15M_SHIFT                 0
#define CAN_RX15MASK_RX15M(x)                    (((uint32_t)(((uint32_t)(x))<<CAN_RX15MASK_RX15M_SHIFT))&CAN_RX15MASK_RX15M_MASK)
/* ECR Bit Fields */
#define CAN_ECR_TXERRCNT_MASK                    0xFFu
#define CAN_ECR_TXERRCNT_SHIFT                   0
#define CAN_ECR_TXERRCNT(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_ECR_TXERRCNT_SHIFT))&CAN_ECR_TXERRCNT_MASK)
#define CAN_ECR_RXERRCNT_MASK                    0xFF00u
#define CAN_ECR_RXERRCNT_SHIFT                   8
#define CAN_ECR_RXERRCNT(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_ECR_RXERRCNT_SHIFT))&CAN_ECR_RXERRCNT_MASK)
/* ESR1 Bit Fields */
#define CAN_ESR1_WAKINT_MASK                     0x1u
#define CAN_ESR1_WAKINT_SHIFT                    0
#define CAN_ESR1_ERRINT_MASK                     0x2u
#define CAN_ESR1_ERRINT_SHIFT                    1
#define CAN_ESR1_BOFFINT_MASK                    0x4u
#define CAN_ESR1_BOFFINT_SHIFT                   2
#define CAN_ESR1_RX_MASK                         0x8u
#define CAN_ESR1_RX_SHIFT                        3
#define CAN_ESR1_FLTCONF_MASK                    0x30u
#define CAN_ESR1_FLTCONF_SHIFT                   4
#define CAN_ESR1_FLTCONF(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_ESR1_FLTCONF_SHIFT))&CAN_ESR1_FLTCONF_MASK)
#define CAN_ESR1_TX_MASK                         0x40u
#define CAN_ESR1_TX_SHIFT                        6
#define CAN_ESR1_IDLE_MASK                       0x80u
#define CAN_ESR1_IDLE_SHIFT                      7
#define CAN_ESR1_RXWRN_MASK                      0x100u
#define CAN_ESR1_RXWRN_SHIFT                     8
#define CAN_ESR1_TXWRN_MASK                      0x200u
#define CAN_ESR1_TXWRN_SHIFT                     9
#define CAN_ESR1_STFERR_MASK                     0x400u
#define CAN_ESR1_STFERR_SHIFT                    10
#define CAN_ESR1_FRMERR_MASK                     0x800u
#define CAN_ESR1_FRMERR_SHIFT                    11
#define CAN_ESR1_CRCERR_MASK                     0x1000u
#define CAN_ESR1_CRCERR_SHIFT                    12
#define CAN_ESR1_ACKERR_MASK                     0x2000u
#define CAN_ESR1_ACKERR_SHIFT                    13
#define CAN_ESR1_BIT0ERR_MASK                    0x4000u
#define CAN_ESR1_BIT0ERR_SHIFT                   14
#define CAN_ESR1_BIT1ERR_MASK                    0x8000u
#define CAN_ESR1_BIT1ERR_SHIFT                   15
#define CAN_ESR1_RWRNINT_MASK                    0x10000u
#define CAN_ESR1_RWRNINT_SHIFT                   16
#define CAN_ESR1_TWRNINT_MASK                    0x20000u
#define CAN_ESR1_TWRNINT_SHIFT                   17
#define CAN_ESR1_SYNCH_MASK                      0x40000u
#define CAN_ESR1_SYNCH_SHIFT                     18
/* IMASK1 Bit Fields */
#define CAN_IMASK1_BUFLM_MASK                    0xFFFFFFFFu
#define CAN_IMASK1_BUFLM_SHIFT                   0
#define CAN_IMASK1_BUFLM(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_IMASK1_BUFLM_SHIFT))&CAN_IMASK1_BUFLM_MASK)
/* IFLAG1 Bit Fields */
#define CAN_IFLAG1_BUF4TO0I_MASK                 0x1Fu
#define CAN_IFLAG1_BUF4TO0I_SHIFT                0
#define CAN_IFLAG1_BUF4TO0I(x)                   (((uint32_t)(((uint32_t)(x))<<CAN_IFLAG1_BUF4TO0I_SHIFT))&CAN_IFLAG1_BUF4TO0I_MASK)
#define CAN_IFLAG1_BUF5I_MASK                    0x20u
#define CAN_IFLAG1_BUF5I_SHIFT                   5
#define CAN_IFLAG1_BUF6I_MASK                    0x40u
#define CAN_IFLAG1_BUF6I_SHIFT                   6
#define CAN_IFLAG1_BUF7I_MASK                    0x80u
#define CAN_IFLAG1_BUF7I_SHIFT                   7
#define CAN_IFLAG1_BUF31TO8I_MASK                0xFFFFFF00u
#define CAN_IFLAG1_BUF31TO8I_SHIFT               8
#define CAN_IFLAG1_BUF31TO8I(x)                  (((uint32_t)(((uint32_t)(x))<<CAN_IFLAG1_BUF31TO8I_SHIFT))&CAN_IFLAG1_BUF31TO8I_MASK)
/* CTRL2 Bit Fields */
#define CAN_CTRL2_EACEN_MASK                     0x10000u
#define CAN_CTRL2_EACEN_SHIFT                    16
#define CAN_CTRL2_RRS_MASK                       0x20000u
#define CAN_CTRL2_RRS_SHIFT                      17
#define CAN_CTRL2_MRP_MASK                       0x40000u
#define CAN_CTRL2_MRP_SHIFT                      18
#define CAN_CTRL2_TASD_MASK                      0xF80000u
#define CAN_CTRL2_TASD_SHIFT                     19
#define CAN_CTRL2_TASD(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_CTRL2_TASD_SHIFT))&CAN_CTRL2_TASD_MASK)
#define CAN_CTRL2_RFFN_MASK                      0xF000000u
#define CAN_CTRL2_RFFN_SHIFT                     24
#define CAN_CTRL2_RFFN(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_CTRL2_RFFN_SHIFT))&CAN_CTRL2_RFFN_MASK)
#define CAN_CTRL2_WRMFRZ_MASK                    0x10000000u
#define CAN_CTRL2_WRMFRZ_SHIFT                   28
/* ESR2 Bit Fields */
#define CAN_ESR2_IMB_MASK                        0x2000u
#define CAN_ESR2_IMB_SHIFT                       13
#define CAN_ESR2_VPS_MASK                        0x4000u
#define CAN_ESR2_VPS_SHIFT                       14
#define CAN_ESR2_LPTM_MASK                       0x7F0000u
#define CAN_ESR2_LPTM_SHIFT                      16
#define CAN_ESR2_LPTM(x)                         (((uint32_t)(((uint32_t)(x))<<CAN_ESR2_LPTM_SHIFT))&CAN_ESR2_LPTM_MASK)
/* CRCR Bit Fields */
#define CAN_CRCR_TXCRC_MASK                      0x7FFFu
#define CAN_CRCR_TXCRC_SHIFT                     0
#define CAN_CRCR_TXCRC(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_CRCR_TXCRC_SHIFT))&CAN_CRCR_TXCRC_MASK)
#define CAN_CRCR_MBCRC_MASK                      0x7F0000u
#define CAN_CRCR_MBCRC_SHIFT                     16
#define CAN_CRCR_MBCRC(x)                        (((uint32_t)(((uint32_t)(x))<<CAN_CRCR_MBCRC_SHIFT))&CAN_CRCR_MBCRC_MASK)
/* RXFGMASK Bit Fields */
#define CAN_RXFGMASK_FGM_MASK                    0xFFFFFFFFu
#define CAN_RXFGMASK_FGM_SHIFT                   0
#define CAN_RXFGMASK_FGM(x)                      (((uint32_t)(((uint32_t)(x))<<CAN_RXFGMASK_FGM_SHIFT))&CAN_RXFGMASK_FGM_MASK)
/* RXFIR Bit Fields */
#define CAN_RXFIR_IDHIT_MASK                     0x1FFu
#define CAN_RXFIR_IDHIT_SHIFT                    0
#define CAN_RXFIR_IDHIT(x)                       (((uint32_t)(((uint32_t)(x))<<CAN_RXFIR_IDHIT_SHIFT))&CAN_RXFIR_IDHIT_MASK)
/* CS Bit Fields */
#define CAN_CS_TIME_STAMP_MASK                   0xFFFFu
#define CAN_CS_TIME_STAMP_SHIFT                  0
#define CAN_CS_TIME_STAMP(x)                     (((uint32_t)(((uint32_t)(x))<<CAN_CS_TIME_STAMP_SHIFT))&CAN_CS_TIME_STAMP_MASK)
#define CAN_CS_DLC_MASK                          0xF0000u
#define CAN_CS_DLC_SHIFT                         16
#define CAN_CS_DLC(x)                            (((uint32_t)(((uint32_t)(x))<<CAN_CS_DLC_SHIFT))&CAN_CS_DLC_MASK)
#define CAN_CS_RTR_MASK                          0x100000u
#define CAN_CS_RTR_SHIFT                         20
#define CAN_CS_IDE_MASK                          0x200000u
#define CAN_CS_IDE_SHIFT                         21
#define CAN_CS_SRR_MASK                          0x400000u
#define CAN_CS_SRR_SHIFT                         22
#define CAN_CS_CODE_MASK                         0xF000000u
#define CAN_CS_CODE_SHIFT                        24
#define CAN_CS_CODE(x)                           (((uint32_t)(((uint32_t)(x))<<CAN_CS_CODE_SHIFT))&CAN_CS_CODE_MASK)
/* ID Bit Fields */
#define CAN_ID_EXT_MASK                          0x3FFFFu
#define CAN_ID_EXT_SHIFT                         0
#define CAN_ID_EXT(x)                            (((uint32_t)(((uint32_t)(x))<<CAN_ID_EXT_SHIFT))&CAN_ID_EXT_MASK)
#define CAN_ID_STD_MASK                          0x1FFC0000u
#define CAN_ID_STD_SHIFT                         18
#define CAN_ID_STD(x)                            (((uint32_t)(((uint32_t)(x))<<CAN_ID_STD_SHIFT))&CAN_ID_STD_MASK)
#define CAN_ID_PRIO_MASK                         0xE0000000u
#define CAN_ID_PRIO_SHIFT                        29
#define CAN_ID_PRIO(x)                           (((uint32_t)(((uint32_t)(x))<<CAN_ID_PRIO_SHIFT))&CAN_ID_PRIO_MASK)
/* WORD0 Bit Fields */
#define CAN_WORD0_DATA_BYTE_3_MASK               0xFFu
#define CAN_WORD0_DATA_BYTE_3_SHIFT              0
#define CAN_WORD0_DATA_BYTE_3(x)                 (((uint32_t)(((uint32_t)(x))<<CAN_WORD0_DATA_BYTE_3_SHIFT))&CAN_WORD0_DATA_BYTE_3_MASK)
#define CAN_WORD0_DATA_BYTE_2_MASK               0xFF00u
#define CAN_WORD0_DATA_BYTE_2_SHIFT              8
#define CAN_WORD0_DATA_BYTE_2(x)                 (((uint32_t)(((uint32_t)(x))<<CAN_WORD0_DATA_BYTE_2_SHIFT))&CAN_WORD0_DATA_BYTE_2_MASK)
#define CAN_WORD0_DATA_BYTE_1_MASK               0xFF0000u
#define CAN_WORD0_DATA_BYTE_1_SHIFT              16
#define CAN_WORD0_DATA_BYTE_1(x)                 (((uint32_t)(((uint32_t)(x))<<CAN_WORD0_DATA_BYTE_1_SHIFT))&CAN_WORD0_DATA_BYTE_1_MASK)
#define CAN_WORD0_DATA_BYTE_0_MASK               0xFF000000u
#define CAN_WORD0_DATA_BYTE_0_SHIFT              24
#define CAN_WORD0_DATA_BYTE_0(x)                 (((uint32_t)(((uint32_t)(x))<<CAN_WORD0_DATA_BYTE_0_SHIFT))&CAN_WORD0_DATA_BYTE_0_MASK)
/* WORD1 Bit Fields */
#define CAN_WORD1_DATA_BYTE_7_MASK               0xFFu
#define CAN_WORD1_DATA_BYTE_7_SHIFT              0
#define CAN_WORD1_DATA_BYTE_7(x)                 (((uint32_t)(((uint32_t)(x))<<CAN_WORD1_DATA_BYTE_7_SHIFT))&CAN_WORD1_DATA_BYTE_7_MASK)
#define CAN_WORD1_DATA_BYTE_6_MASK               0xFF00u
#define CAN_WORD1_DATA_BYTE_6_SHIFT              8
#define CAN_WORD1_DATA_BYTE_6(x)                 (((uint32_t)(((uint32_t)(x))<<CAN_WORD1_DATA_BYTE_6_SHIFT))&CAN_WORD1_DATA_BYTE_6_MASK)
#define CAN_WORD1_DATA_BYTE_5_MASK               0xFF0000u
#define CAN_WORD1_DATA_BYTE_5_SHIFT              16
#define CAN_WORD1_DATA_BYTE_5(x)                 (((uint32_t)(((uint32_t)(x))<<CAN_WORD1_DATA_BYTE_5_SHIFT))&CAN_WORD1_DATA_BYTE_5_MASK)
#define CAN_WORD1_DATA_BYTE_4_MASK               0xFF000000u
#define CAN_WORD1_DATA_BYTE_4_SHIFT              24
#define CAN_WORD1_DATA_BYTE_4(x)                 (((uint32_t)(((uint32_t)(x))<<CAN_WORD1_DATA_BYTE_4_SHIFT))&CAN_WORD1_DATA_BYTE_4_MASK)
/* RXIMR Bit Fields */
#define CAN_RXIMR_MI_MASK                        0xFFFFFFFFu
#define CAN_RXIMR_MI_SHIFT                       0
#define CAN_RXIMR_MI(x)                          (((uint32_t)(((uint32_t)(x))<<CAN_RXIMR_MI_SHIFT))&CAN_RXIMR_MI_MASK)

/**
 * @}
 */ /* end of group CAN_Register_Masks */


/* CAN - Peripheral instance base addresses */
/** Peripheral CAN0 base address */
#define CAN0_BASE                                (0x40024000u)
/** Peripheral CAN0 base pointer */
#define CAN0                                     ((CAN_Type *)CAN0_BASE)
/** Peripheral CAN1 base address */
#define CAN1_BASE                                (0x400A4000u)
/** Peripheral CAN1 base pointer */
#define CAN1                                     ((CAN_Type *)CAN1_BASE)
/** Array initializer of CAN peripheral base pointers */
#define CAN_BASES                                { CAN0, CAN1 }

/**
 * @}
 */ /* end of group CAN_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- CAU Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup CAU_Peripheral_Access_Layer CAU Peripheral Access Layer
 * @{
 */

/** CAU - Register Layout Typedef */
typedef struct {
  __O  uint32_t DIRECT[16];                        /**< Direct access register 0..Direct access register 15, array offset: 0x0, array step: 0x4 */
       uint8_t RESERVED_0[2048];
  __O  uint32_t LDR_CASR;                          /**< Status register  - Load Register command, offset: 0x840 */
  __O  uint32_t LDR_CAA;                           /**< Accumulator register - Load Register command, offset: 0x844 */
  __O  uint32_t LDR_CA[9];                         /**< General Purpose Register 0 - Load Register command..General Purpose Register 8 - Load Register command, array offset: 0x848, array step: 0x4 */
       uint8_t RESERVED_1[20];
  __I  uint32_t STR_CASR;                          /**< Status register  - Store Register command, offset: 0x880 */
  __I  uint32_t STR_CAA;                           /**< Accumulator register - Store Register command, offset: 0x884 */
  __I  uint32_t STR_CA[9];                         /**< General Purpose Register 0 - Store Register command..General Purpose Register 8 - Store Register command, array offset: 0x888, array step: 0x4 */
       uint8_t RESERVED_2[20];
  __O  uint32_t ADR_CASR;                          /**< Status register  - Add Register command, offset: 0x8C0 */
  __O  uint32_t ADR_CAA;                           /**< Accumulator register - Add to register command, offset: 0x8C4 */
  __O  uint32_t ADR_CA[9];                         /**< General Purpose Register 0 - Add to register command..General Purpose Register 8 - Add to register command, array offset: 0x8C8, array step: 0x4 */
       uint8_t RESERVED_3[20];
  __O  uint32_t RADR_CASR;                         /**< Status register  - Reverse and Add to Register command, offset: 0x900 */
  __O  uint32_t RADR_CAA;                          /**< Accumulator register - Reverse and Add to Register command, offset: 0x904 */
  __O  uint32_t RADR_CA[9];                        /**< General Purpose Register 0 - Reverse and Add to Register command..General Purpose Register 8 - Reverse and Add to Register command, array offset: 0x908, array step: 0x4 */
       uint8_t RESERVED_4[84];
  __O  uint32_t XOR_CASR;                          /**< Status register  - Exclusive Or command, offset: 0x980 */
  __O  uint32_t XOR_CAA;                           /**< Accumulator register - Exclusive Or command, offset: 0x984 */
  __O  uint32_t XOR_CA[9];                         /**< General Purpose Register 0 - Exclusive Or command..General Purpose Register 8 - Exclusive Or command, array offset: 0x988, array step: 0x4 */
       uint8_t RESERVED_5[20];
  __O  uint32_t ROTL_CASR;                         /**< Status register  - Rotate Left command, offset: 0x9C0 */
  __O  uint32_t ROTL_CAA;                          /**< Accumulator register - Rotate Left command, offset: 0x9C4 */
  __O  uint32_t ROTL_CA[9];                        /**< General Purpose Register 0 - Rotate Left command..General Purpose Register 8 - Rotate Left command, array offset: 0x9C8, array step: 0x4 */
       uint8_t RESERVED_6[276];
  __O  uint32_t AESC_CASR;                         /**< Status register  - AES Column Operation command, offset: 0xB00 */
  __O  uint32_t AESC_CAA;                          /**< Accumulator register - AES Column Operation command, offset: 0xB04 */
  __O  uint32_t AESC_CA[9];                        /**< General Purpose Register 0 - AES Column Operation command..General Purpose Register 8 - AES Column Operation command, array offset: 0xB08, array step: 0x4 */
       uint8_t RESERVED_7[20];
  __O  uint32_t AESIC_CASR;                        /**< Status register  - AES Inverse Column Operation command, offset: 0xB40 */
  __O  uint32_t AESIC_CAA;                         /**< Accumulator register - AES Inverse Column Operation command, offset: 0xB44 */
  __O  uint32_t AESIC_CA[9];                       /**< General Purpose Register 0 - AES Inverse Column Operation command..General Purpose Register 8 - AES Inverse Column Operation command, array offset: 0xB48, array step: 0x4 */
} CAU_Type;

/* ----------------------------------------------------------------------------
   -- CAU Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup CAU_Register_Masks CAU Register Masks
 * @{
 */

/* LDR_CASR Bit Fields */
#define CAU_LDR_CASR_IC_MASK                     0x1u
#define CAU_LDR_CASR_IC_SHIFT                    0
#define CAU_LDR_CASR_DPE_MASK                    0x2u
#define CAU_LDR_CASR_DPE_SHIFT                   1
#define CAU_LDR_CASR_VER_MASK                    0xF0000000u
#define CAU_LDR_CASR_VER_SHIFT                   28
#define CAU_LDR_CASR_VER(x)                      (((uint32_t)(((uint32_t)(x))<<CAU_LDR_CASR_VER_SHIFT))&CAU_LDR_CASR_VER_MASK)
/* STR_CASR Bit Fields */
#define CAU_STR_CASR_IC_MASK                     0x1u
#define CAU_STR_CASR_IC_SHIFT                    0
#define CAU_STR_CASR_DPE_MASK                    0x2u
#define CAU_STR_CASR_DPE_SHIFT                   1
#define CAU_STR_CASR_VER_MASK                    0xF0000000u
#define CAU_STR_CASR_VER_SHIFT                   28
#define CAU_STR_CASR_VER(x)                      (((uint32_t)(((uint32_t)(x))<<CAU_STR_CASR_VER_SHIFT))&CAU_STR_CASR_VER_MASK)
/* ADR_CASR Bit Fields */
#define CAU_ADR_CASR_IC_MASK                     0x1u
#define CAU_ADR_CASR_IC_SHIFT                    0
#define CAU_ADR_CASR_DPE_MASK                    0x2u
#define CAU_ADR_CASR_DPE_SHIFT                   1
#define CAU_ADR_CASR_VER_MASK                    0xF0000000u
#define CAU_ADR_CASR_VER_SHIFT                   28
#define CAU_ADR_CASR_VER(x)                      (((uint32_t)(((uint32_t)(x))<<CAU_ADR_CASR_VER_SHIFT))&CAU_ADR_CASR_VER_MASK)
/* RADR_CASR Bit Fields */
#define CAU_RADR_CASR_IC_MASK                    0x1u
#define CAU_RADR_CASR_IC_SHIFT                   0
#define CAU_RADR_CASR_DPE_MASK                   0x2u
#define CAU_RADR_CASR_DPE_SHIFT                  1
#define CAU_RADR_CASR_VER_MASK                   0xF0000000u
#define CAU_RADR_CASR_VER_SHIFT                  28
#define CAU_RADR_CASR_VER(x)                     (((uint32_t)(((uint32_t)(x))<<CAU_RADR_CASR_VER_SHIFT))&CAU_RADR_CASR_VER_MASK)
/* XOR_CASR Bit Fields */
#define CAU_XOR_CASR_IC_MASK                     0x1u
#define CAU_XOR_CASR_IC_SHIFT                    0
#define CAU_XOR_CASR_DPE_MASK                    0x2u
#define CAU_XOR_CASR_DPE_SHIFT                   1
#define CAU_XOR_CASR_VER_MASK                    0xF0000000u
#define CAU_XOR_CASR_VER_SHIFT                   28
#define CAU_XOR_CASR_VER(x)                      (((uint32_t)(((uint32_t)(x))<<CAU_XOR_CASR_VER_SHIFT))&CAU_XOR_CASR_VER_MASK)
/* ROTL_CASR Bit Fields */
#define CAU_ROTL_CASR_IC_MASK                    0x1u
#define CAU_ROTL_CASR_IC_SHIFT                   0
#define CAU_ROTL_CASR_DPE_MASK                   0x2u
#define CAU_ROTL_CASR_DPE_SHIFT                  1
#define CAU_ROTL_CASR_VER_MASK                   0xF0000000u
#define CAU_ROTL_CASR_VER_SHIFT                  28
#define CAU_ROTL_CASR_VER(x)                     (((uint32_t)(((uint32_t)(x))<<CAU_ROTL_CASR_VER_SHIFT))&CAU_ROTL_CASR_VER_MASK)
/* AESC_CASR Bit Fields */
#define CAU_AESC_CASR_IC_MASK                    0x1u
#define CAU_AESC_CASR_IC_SHIFT                   0
#define CAU_AESC_CASR_DPE_MASK                   0x2u
#define CAU_AESC_CASR_DPE_SHIFT                  1
#define CAU_AESC_CASR_VER_MASK                   0xF0000000u
#define CAU_AESC_CASR_VER_SHIFT                  28
#define CAU_AESC_CASR_VER(x)                     (((uint32_t)(((uint32_t)(x))<<CAU_AESC_CASR_VER_SHIFT))&CAU_AESC_CASR_VER_MASK)
/* AESIC_CASR Bit Fields */
#define CAU_AESIC_CASR_IC_MASK                   0x1u
#define CAU_AESIC_CASR_IC_SHIFT                  0
#define CAU_AESIC_CASR_DPE_MASK                  0x2u
#define CAU_AESIC_CASR_DPE_SHIFT                 1
#define CAU_AESIC_CASR_VER_MASK                  0xF0000000u
#define CAU_AESIC_CASR_VER_SHIFT                 28
#define CAU_AESIC_CASR_VER(x)                    (((uint32_t)(((uint32_t)(x))<<CAU_AESIC_CASR_VER_SHIFT))&CAU_AESIC_CASR_VER_MASK)

/**
 * @}
 */ /* end of group CAU_Register_Masks */


/* CAU - Peripheral instance base addresses */
/** Peripheral CAU base address */
#define CAU_BASE                                 (0xE0081000u)
/** Peripheral CAU base pointer */
#define CAU                                      ((CAU_Type *)CAU_BASE)
/** Array initializer of CAU peripheral base pointers */
#define CAU_BASES                                { CAU }

/**
 * @}
 */ /* end of group CAU_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- CMP Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup CMP_Peripheral_Access_Layer CMP Peripheral Access Layer
 * @{
 */

/** CMP - Register Layout Typedef */
typedef struct {
  __IO uint8_t CR0;                                /**< CMP Control Register 0, offset: 0x0 */
  __IO uint8_t CR1;                                /**< CMP Control Register 1, offset: 0x1 */
  __IO uint8_t FPR;                                /**< CMP Filter Period Register, offset: 0x2 */
  __IO uint8_t SCR;                                /**< CMP Status and Control Register, offset: 0x3 */
  __IO uint8_t DACCR;                              /**< DAC Control Register, offset: 0x4 */
  __IO uint8_t MUXCR;                              /**< MUX Control Register, offset: 0x5 */
} CMP_Type;

/* ----------------------------------------------------------------------------
   -- CMP Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup CMP_Register_Masks CMP Register Masks
 * @{
 */

/* CR0 Bit Fields */
#define CMP_CR0_HYSTCTR_MASK                     0x3u
#define CMP_CR0_HYSTCTR_SHIFT                    0
#define CMP_CR0_HYSTCTR(x)                       (((uint8_t)(((uint8_t)(x))<<CMP_CR0_HYSTCTR_SHIFT))&CMP_CR0_HYSTCTR_MASK)
#define CMP_CR0_FILTER_CNT_MASK                  0x70u
#define CMP_CR0_FILTER_CNT_SHIFT                 4
#define CMP_CR0_FILTER_CNT(x)                    (((uint8_t)(((uint8_t)(x))<<CMP_CR0_FILTER_CNT_SHIFT))&CMP_CR0_FILTER_CNT_MASK)
/* CR1 Bit Fields */
#define CMP_CR1_EN_MASK                          0x1u
#define CMP_CR1_EN_SHIFT                         0
#define CMP_CR1_OPE_MASK                         0x2u
#define CMP_CR1_OPE_SHIFT                        1
#define CMP_CR1_COS_MASK                         0x4u
#define CMP_CR1_COS_SHIFT                        2
#define CMP_CR1_INV_MASK                         0x8u
#define CMP_CR1_INV_SHIFT                        3
#define CMP_CR1_PMODE_MASK                       0x10u
#define CMP_CR1_PMODE_SHIFT                      4
#define CMP_CR1_WE_MASK                          0x40u
#define CMP_CR1_WE_SHIFT                         6
#define CMP_CR1_SE_MASK                          0x80u
#define CMP_CR1_SE_SHIFT                         7
/* FPR Bit Fields */
#define CMP_FPR_FILT_PER_MASK                    0xFFu
#define CMP_FPR_FILT_PER_SHIFT                   0
#define CMP_FPR_FILT_PER(x)                      (((uint8_t)(((uint8_t)(x))<<CMP_FPR_FILT_PER_SHIFT))&CMP_FPR_FILT_PER_MASK)
/* SCR Bit Fields */
#define CMP_SCR_COUT_MASK                        0x1u
#define CMP_SCR_COUT_SHIFT                       0
#define CMP_SCR_CFF_MASK                         0x2u
#define CMP_SCR_CFF_SHIFT                        1
#define CMP_SCR_CFR_MASK                         0x4u
#define CMP_SCR_CFR_SHIFT                        2
#define CMP_SCR_IEF_MASK                         0x8u
#define CMP_SCR_IEF_SHIFT                        3
#define CMP_SCR_IER_MASK                         0x10u
#define CMP_SCR_IER_SHIFT                        4
#define CMP_SCR_DMAEN_MASK                       0x40u
#define CMP_SCR_DMAEN_SHIFT                      6
/* DACCR Bit Fields */
#define CMP_DACCR_VOSEL_MASK                     0x3Fu
#define CMP_DACCR_VOSEL_SHIFT                    0
#define CMP_DACCR_VOSEL(x)                       (((uint8_t)(((uint8_t)(x))<<CMP_DACCR_VOSEL_SHIFT))&CMP_DACCR_VOSEL_MASK)
#define CMP_DACCR_VRSEL_MASK                     0x40u
#define CMP_DACCR_VRSEL_SHIFT                    6
#define CMP_DACCR_DACEN_MASK                     0x80u
#define CMP_DACCR_DACEN_SHIFT                    7
/* MUXCR Bit Fields */
#define CMP_MUXCR_MSEL_MASK                      0x7u
#define CMP_MUXCR_MSEL_SHIFT                     0
#define CMP_MUXCR_MSEL(x)                        (((uint8_t)(((uint8_t)(x))<<CMP_MUXCR_MSEL_SHIFT))&CMP_MUXCR_MSEL_MASK)
#define CMP_MUXCR_PSEL_MASK                      0x38u
#define CMP_MUXCR_PSEL_SHIFT                     3
#define CMP_MUXCR_PSEL(x)                        (((uint8_t)(((uint8_t)(x))<<CMP_MUXCR_PSEL_SHIFT))&CMP_MUXCR_PSEL_MASK)
#define CMP_MUXCR_PSTM_MASK                      0x40u
#define CMP_MUXCR_PSTM_SHIFT                     6

/**
 * @}
 */ /* end of group CMP_Register_Masks */


/* CMP - Peripheral instance base addresses */
/** Peripheral CMP0 base address */
#define CMP0_BASE                                (0x40073000u)
/** Peripheral CMP0 base pointer */
#define CMP0                                     ((CMP_Type *)CMP0_BASE)
/** Peripheral CMP1 base address */
#define CMP1_BASE                                (0x40073008u)
/** Peripheral CMP1 base pointer */
#define CMP1                                     ((CMP_Type *)CMP1_BASE)
/** Peripheral CMP2 base address */
#define CMP2_BASE                                (0x40073010u)
/** Peripheral CMP2 base pointer */
#define CMP2                                     ((CMP_Type *)CMP2_BASE)
/** Array initializer of CMP peripheral base pointers */
#define CMP_BASES                                { CMP0, CMP1, CMP2 }

/**
 * @}
 */ /* end of group CMP_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- CMT Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup CMT_Peripheral_Access_Layer CMT Peripheral Access Layer
 * @{
 */

/** CMT - Register Layout Typedef */
typedef struct {
  __IO uint8_t CGH1;                               /**< CMT Carrier Generator High Data Register 1, offset: 0x0 */
  __IO uint8_t CGL1;                               /**< CMT Carrier Generator Low Data Register 1, offset: 0x1 */
  __IO uint8_t CGH2;                               /**< CMT Carrier Generator High Data Register 2, offset: 0x2 */
  __IO uint8_t CGL2;                               /**< CMT Carrier Generator Low Data Register 2, offset: 0x3 */
  __IO uint8_t OC;                                 /**< CMT Output Control Register, offset: 0x4 */
  __IO uint8_t MSC;                                /**< CMT Modulator Status and Control Register, offset: 0x5 */
  __IO uint8_t CMD1;                               /**< CMT Modulator Data Register Mark High, offset: 0x6 */
  __IO uint8_t CMD2;                               /**< CMT Modulator Data Register Mark Low, offset: 0x7 */
  __IO uint8_t CMD3;                               /**< CMT Modulator Data Register Space High, offset: 0x8 */
  __IO uint8_t CMD4;                               /**< CMT Modulator Data Register Space Low, offset: 0x9 */
  __IO uint8_t PPS;                                /**< CMT Primary Prescaler Register, offset: 0xA */
  __IO uint8_t DMA;                                /**< CMT Direct Memory Access Register, offset: 0xB */
} CMT_Type;

/* ----------------------------------------------------------------------------
   -- CMT Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup CMT_Register_Masks CMT Register Masks
 * @{
 */

/* CGH1 Bit Fields */
#define CMT_CGH1_PH_MASK                         0xFFu
#define CMT_CGH1_PH_SHIFT                        0
#define CMT_CGH1_PH(x)                           (((uint8_t)(((uint8_t)(x))<<CMT_CGH1_PH_SHIFT))&CMT_CGH1_PH_MASK)
/* CGL1 Bit Fields */
#define CMT_CGL1_PL_MASK                         0xFFu
#define CMT_CGL1_PL_SHIFT                        0
#define CMT_CGL1_PL(x)                           (((uint8_t)(((uint8_t)(x))<<CMT_CGL1_PL_SHIFT))&CMT_CGL1_PL_MASK)
/* CGH2 Bit Fields */
#define CMT_CGH2_SH_MASK                         0xFFu
#define CMT_CGH2_SH_SHIFT                        0
#define CMT_CGH2_SH(x)                           (((uint8_t)(((uint8_t)(x))<<CMT_CGH2_SH_SHIFT))&CMT_CGH2_SH_MASK)
/* CGL2 Bit Fields */
#define CMT_CGL2_SL_MASK                         0xFFu
#define CMT_CGL2_SL_SHIFT                        0
#define CMT_CGL2_SL(x)                           (((uint8_t)(((uint8_t)(x))<<CMT_CGL2_SL_SHIFT))&CMT_CGL2_SL_MASK)
/* OC Bit Fields */
#define CMT_OC_IROPEN_MASK                       0x20u
#define CMT_OC_IROPEN_SHIFT                      5
#define CMT_OC_CMTPOL_MASK                       0x40u
#define CMT_OC_CMTPOL_SHIFT                      6
#define CMT_OC_IROL_MASK                         0x80u
#define CMT_OC_IROL_SHIFT                        7
/* MSC Bit Fields */
#define CMT_MSC_MCGEN_MASK                       0x1u
#define CMT_MSC_MCGEN_SHIFT                      0
#define CMT_MSC_EOCIE_MASK                       0x2u
#define CMT_MSC_EOCIE_SHIFT                      1
#define CMT_MSC_FSK_MASK                         0x4u
#define CMT_MSC_FSK_SHIFT                        2
#define CMT_MSC_BASE_MASK                        0x8u
#define CMT_MSC_BASE_SHIFT                       3
#define CMT_MSC_EXSPC_MASK                       0x10u
#define CMT_MSC_EXSPC_SHIFT                      4
#define CMT_MSC_CMTDIV_MASK                      0x60u
#define CMT_MSC_CMTDIV_SHIFT                     5
#define CMT_MSC_CMTDIV(x)                        (((uint8_t)(((uint8_t)(x))<<CMT_MSC_CMTDIV_SHIFT))&CMT_MSC_CMTDIV_MASK)
#define CMT_MSC_EOCF_MASK                        0x80u
#define CMT_MSC_EOCF_SHIFT                       7
/* CMD1 Bit Fields */
#define CMT_CMD1_MB_MASK                         0xFFu
#define CMT_CMD1_MB_SHIFT                        0
#define CMT_CMD1_MB(x)                           (((uint8_t)(((uint8_t)(x))<<CMT_CMD1_MB_SHIFT))&CMT_CMD1_MB_MASK)
/* CMD2 Bit Fields */
#define CMT_CMD2_MB_MASK                         0xFFu
#define CMT_CMD2_MB_SHIFT                        0
#define CMT_CMD2_MB(x)                           (((uint8_t)(((uint8_t)(x))<<CMT_CMD2_MB_SHIFT))&CMT_CMD2_MB_MASK)
/* CMD3 Bit Fields */
#define CMT_CMD3_SB_MASK                         0xFFu
#define CMT_CMD3_SB_SHIFT                        0
#define CMT_CMD3_SB(x)                           (((uint8_t)(((uint8_t)(x))<<CMT_CMD3_SB_SHIFT))&CMT_CMD3_SB_MASK)
/* CMD4 Bit Fields */
#define CMT_CMD4_SB_MASK                         0xFFu
#define CMT_CMD4_SB_SHIFT                        0
#define CMT_CMD4_SB(x)                           (((uint8_t)(((uint8_t)(x))<<CMT_CMD4_SB_SHIFT))&CMT_CMD4_SB_MASK)
/* PPS Bit Fields */
#define CMT_PPS_PPSDIV_MASK                      0xFu
#define CMT_PPS_PPSDIV_SHIFT                     0
#define CMT_PPS_PPSDIV(x)                        (((uint8_t)(((uint8_t)(x))<<CMT_PPS_PPSDIV_SHIFT))&CMT_PPS_PPSDIV_MASK)
/* DMA Bit Fields */
#define CMT_DMA_DMA_MASK                         0x1u
#define CMT_DMA_DMA_SHIFT                        0

/**
 * @}
 */ /* end of group CMT_Register_Masks */


/* CMT - Peripheral instance base addresses */
/** Peripheral CMT base address */
#define CMT_BASE                                 (0x40062000u)
/** Peripheral CMT base pointer */
#define CMT                                      ((CMT_Type *)CMT_BASE)
/** Array initializer of CMT peripheral base pointers */
#define CMT_BASES                                { CMT }

/**
 * @}
 */ /* end of group CMT_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- CRC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup CRC_Peripheral_Access_Layer CRC Peripheral Access Layer
 * @{
 */

/** CRC - Register Layout Typedef */
typedef struct {
  union {                                          /* offset: 0x0 */
    struct {                                         /* offset: 0x0 */
      __IO uint16_t CRCL;                              /**< CRC_CRCL register., offset: 0x0 */
      __IO uint16_t CRCH;                              /**< CRC_CRCH register., offset: 0x2 */
    } ACCESS16BIT;
    __IO uint32_t CRC;                               /**< CRC Data register, offset: 0x0 */
    struct {                                         /* offset: 0x0 */
      __IO uint8_t CRCLL;                              /**< CRC_CRCLL register., offset: 0x0 */
      __IO uint8_t CRCLU;                              /**< CRC_CRCLU register., offset: 0x1 */
      __IO uint8_t CRCHL;                              /**< CRC_CRCHL register., offset: 0x2 */
      __IO uint8_t CRCHU;                              /**< CRC_CRCHU register., offset: 0x3 */
    } ACCESS8BIT;
  };
  union {                                          /* offset: 0x4 */
    struct {                                         /* offset: 0x4 */
      __IO uint16_t GPOLYL;                            /**< CRC_GPOLYL register., offset: 0x4 */
      __IO uint16_t GPOLYH;                            /**< CRC_GPOLYH register., offset: 0x6 */
    } GPOLY_ACCESS16BIT;
    __IO uint32_t GPOLY;                             /**< CRC Polynomial register, offset: 0x4 */
    struct {                                         /* offset: 0x4 */
      __IO uint8_t GPOLYLL;                            /**< CRC_GPOLYLL register., offset: 0x4 */
      __IO uint8_t GPOLYLU;                            /**< CRC_GPOLYLU register., offset: 0x5 */
      __IO uint8_t GPOLYHL;                            /**< CRC_GPOLYHL register., offset: 0x6 */
      __IO uint8_t GPOLYHU;                            /**< CRC_GPOLYHU register., offset: 0x7 */
    } GPOLY_ACCESS8BIT;
  };
  union {                                          /* offset: 0x8 */
    __IO uint32_t CTRL;                              /**< CRC Control register, offset: 0x8 */
    struct {                                         /* offset: 0x8 */
           uint8_t RESERVED_0[3];
      __IO uint8_t CTRLHU;                             /**< CRC_CTRLHU register., offset: 0xB */
    } CTRL_ACCESS8BIT;
  };
} CRC_Type;

/* ----------------------------------------------------------------------------
   -- CRC Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup CRC_Register_Masks CRC Register Masks
 * @{
 */

/* CRCL Bit Fields */
#define CRC_CRCL_CRCL_MASK                       0xFFFFu
#define CRC_CRCL_CRCL_SHIFT                      0
#define CRC_CRCL_CRCL(x)                         (((uint16_t)(((uint16_t)(x))<<CRC_CRCL_CRCL_SHIFT))&CRC_CRCL_CRCL_MASK)
/* CRCH Bit Fields */
#define CRC_CRCH_CRCH_MASK                       0xFFFFu
#define CRC_CRCH_CRCH_SHIFT                      0
#define CRC_CRCH_CRCH(x)                         (((uint16_t)(((uint16_t)(x))<<CRC_CRCH_CRCH_SHIFT))&CRC_CRCH_CRCH_MASK)
/* CRC Bit Fields */
#define CRC_CRC_LL_MASK                          0xFFu
#define CRC_CRC_LL_SHIFT                         0
#define CRC_CRC_LL(x)                            (((uint32_t)(((uint32_t)(x))<<CRC_CRC_LL_SHIFT))&CRC_CRC_LL_MASK)
#define CRC_CRC_LU_MASK                          0xFF00u
#define CRC_CRC_LU_SHIFT                         8
#define CRC_CRC_LU(x)                            (((uint32_t)(((uint32_t)(x))<<CRC_CRC_LU_SHIFT))&CRC_CRC_LU_MASK)
#define CRC_CRC_HL_MASK                          0xFF0000u
#define CRC_CRC_HL_SHIFT                         16
#define CRC_CRC_HL(x)                            (((uint32_t)(((uint32_t)(x))<<CRC_CRC_HL_SHIFT))&CRC_CRC_HL_MASK)
#define CRC_CRC_HU_MASK                          0xFF000000u
#define CRC_CRC_HU_SHIFT                         24
#define CRC_CRC_HU(x)                            (((uint32_t)(((uint32_t)(x))<<CRC_CRC_HU_SHIFT))&CRC_CRC_HU_MASK)
/* CRCLL Bit Fields */
#define CRC_CRCLL_CRCLL_MASK                     0xFFu
#define CRC_CRCLL_CRCLL_SHIFT                    0
#define CRC_CRCLL_CRCLL(x)                       (((uint8_t)(((uint8_t)(x))<<CRC_CRCLL_CRCLL_SHIFT))&CRC_CRCLL_CRCLL_MASK)
/* CRCLU Bit Fields */
#define CRC_CRCLU_CRCLU_MASK                     0xFFu
#define CRC_CRCLU_CRCLU_SHIFT                    0
#define CRC_CRCLU_CRCLU(x)                       (((uint8_t)(((uint8_t)(x))<<CRC_CRCLU_CRCLU_SHIFT))&CRC_CRCLU_CRCLU_MASK)
/* CRCHL Bit Fields */
#define CRC_CRCHL_CRCHL_MASK                     0xFFu
#define CRC_CRCHL_CRCHL_SHIFT                    0
#define CRC_CRCHL_CRCHL(x)                       (((uint8_t)(((uint8_t)(x))<<CRC_CRCHL_CRCHL_SHIFT))&CRC_CRCHL_CRCHL_MASK)
/* CRCHU Bit Fields */
#define CRC_CRCHU_CRCHU_MASK                     0xFFu
#define CRC_CRCHU_CRCHU_SHIFT                    0
#define CRC_CRCHU_CRCHU(x)                       (((uint8_t)(((uint8_t)(x))<<CRC_CRCHU_CRCHU_SHIFT))&CRC_CRCHU_CRCHU_MASK)
/* GPOLYL Bit Fields */
#define CRC_GPOLYL_GPOLYL_MASK                   0xFFFFu
#define CRC_GPOLYL_GPOLYL_SHIFT                  0
#define CRC_GPOLYL_GPOLYL(x)                     (((uint16_t)(((uint16_t)(x))<<CRC_GPOLYL_GPOLYL_SHIFT))&CRC_GPOLYL_GPOLYL_MASK)
/* GPOLYH Bit Fields */
#define CRC_GPOLYH_GPOLYH_MASK                   0xFFFFu
#define CRC_GPOLYH_GPOLYH_SHIFT                  0
#define CRC_GPOLYH_GPOLYH(x)                     (((uint16_t)(((uint16_t)(x))<<CRC_GPOLYH_GPOLYH_SHIFT))&CRC_GPOLYH_GPOLYH_MASK)
/* GPOLY Bit Fields */
#define CRC_GPOLY_LOW_MASK                       0xFFFFu
#define CRC_GPOLY_LOW_SHIFT                      0
#define CRC_GPOLY_LOW(x)                         (((uint32_t)(((uint32_t)(x))<<CRC_GPOLY_LOW_SHIFT))&CRC_GPOLY_LOW_MASK)
#define CRC_GPOLY_HIGH_MASK                      0xFFFF0000u
#define CRC_GPOLY_HIGH_SHIFT                     16
#define CRC_GPOLY_HIGH(x)                        (((uint32_t)(((uint32_t)(x))<<CRC_GPOLY_HIGH_SHIFT))&CRC_GPOLY_HIGH_MASK)
/* GPOLYLL Bit Fields */
#define CRC_GPOLYLL_GPOLYLL_MASK                 0xFFu
#define CRC_GPOLYLL_GPOLYLL_SHIFT                0
#define CRC_GPOLYLL_GPOLYLL(x)                   (((uint8_t)(((uint8_t)(x))<<CRC_GPOLYLL_GPOLYLL_SHIFT))&CRC_GPOLYLL_GPOLYLL_MASK)
/* GPOLYLU Bit Fields */
#define CRC_GPOLYLU_GPOLYLU_MASK                 0xFFu
#define CRC_GPOLYLU_GPOLYLU_SHIFT                0
#define CRC_GPOLYLU_GPOLYLU(x)                   (((uint8_t)(((uint8_t)(x))<<CRC_GPOLYLU_GPOLYLU_SHIFT))&CRC_GPOLYLU_GPOLYLU_MASK)
/* GPOLYHL Bit Fields */
#define CRC_GPOLYHL_GPOLYHL_MASK                 0xFFu
#define CRC_GPOLYHL_GPOLYHL_SHIFT                0
#define CRC_GPOLYHL_GPOLYHL(x)                   (((uint8_t)(((uint8_t)(x))<<CRC_GPOLYHL_GPOLYHL_SHIFT))&CRC_GPOLYHL_GPOLYHL_MASK)
/* GPOLYHU Bit Fields */
#define CRC_GPOLYHU_GPOLYHU_MASK                 0xFFu
#define CRC_GPOLYHU_GPOLYHU_SHIFT                0
#define CRC_GPOLYHU_GPOLYHU(x)                   (((uint8_t)(((uint8_t)(x))<<CRC_GPOLYHU_GPOLYHU_SHIFT))&CRC_GPOLYHU_GPOLYHU_MASK)
/* CTRL Bit Fields */
#define CRC_CTRL_TCRC_MASK                       0x1000000u
#define CRC_CTRL_TCRC_SHIFT                      24
#define CRC_CTRL_WAS_MASK                        0x2000000u
#define CRC_CTRL_WAS_SHIFT                       25
#define CRC_CTRL_FXOR_MASK                       0x4000000u
#define CRC_CTRL_FXOR_SHIFT                      26
#define CRC_CTRL_TOTR_MASK                       0x30000000u
#define CRC_CTRL_TOTR_SHIFT                      28
#define CRC_CTRL_TOTR(x)                         (((uint32_t)(((uint32_t)(x))<<CRC_CTRL_TOTR_SHIFT))&CRC_CTRL_TOTR_MASK)
#define CRC_CTRL_TOT_MASK                        0xC0000000u
#define CRC_CTRL_TOT_SHIFT                       30
#define CRC_CTRL_TOT(x)                          (((uint32_t)(((uint32_t)(x))<<CRC_CTRL_TOT_SHIFT))&CRC_CTRL_TOT_MASK)
/* CTRLHU Bit Fields */
#define CRC_CTRLHU_TCRC_MASK                     0x1u
#define CRC_CTRLHU_TCRC_SHIFT                    0
#define CRC_CTRLHU_WAS_MASK                      0x2u
#define CRC_CTRLHU_WAS_SHIFT                     1
#define CRC_CTRLHU_FXOR_MASK                     0x4u
#define CRC_CTRLHU_FXOR_SHIFT                    2
#define CRC_CTRLHU_TOTR_MASK                     0x30u
#define CRC_CTRLHU_TOTR_SHIFT                    4
#define CRC_CTRLHU_TOTR(x)                       (((uint8_t)(((uint8_t)(x))<<CRC_CTRLHU_TOTR_SHIFT))&CRC_CTRLHU_TOTR_MASK)
#define CRC_CTRLHU_TOT_MASK                      0xC0u
#define CRC_CTRLHU_TOT_SHIFT                     6
#define CRC_CTRLHU_TOT(x)                        (((uint8_t)(((uint8_t)(x))<<CRC_CTRLHU_TOT_SHIFT))&CRC_CTRLHU_TOT_MASK)

/**
 * @}
 */ /* end of group CRC_Register_Masks */


/* CRC - Peripheral instance base addresses */
/** Peripheral CRC base address */
#define CRC_BASE                                 (0x40032000u)
/** Peripheral CRC base pointer */
#define CRC0                                     ((CRC_Type *)CRC_BASE)
/** Array initializer of CRC peripheral base pointers */
#define CRC_BASES                                { CRC0 }

/**
 * @}
 */ /* end of group CRC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- DAC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup DAC_Peripheral_Access_Layer DAC Peripheral Access Layer
 * @{
 */

/** DAC - Register Layout Typedef */
typedef struct {
  struct {                                         /* offset: 0x0, array step: 0x2 */
    __IO uint8_t DATL;                               /**< DAC Data Low Register, array offset: 0x0, array step: 0x2 */
    __IO uint8_t DATH;                               /**< DAC Data High Register, array offset: 0x1, array step: 0x2 */
  } DAT[16];
  __IO uint8_t SR;                                 /**< DAC Status Register, offset: 0x20 */
  __IO uint8_t C0;                                 /**< DAC Control Register, offset: 0x21 */
  __IO uint8_t C1;                                 /**< DAC Control Register 1, offset: 0x22 */
  __IO uint8_t C2;                                 /**< DAC Control Register 2, offset: 0x23 */
} DAC_Type;

/* ----------------------------------------------------------------------------
   -- DAC Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup DAC_Register_Masks DAC Register Masks
 * @{
 */

/* DATL Bit Fields */
#define DAC_DATL_DATA0_MASK                      0xFFu
#define DAC_DATL_DATA0_SHIFT                     0
#define DAC_DATL_DATA0(x)                        (((uint8_t)(((uint8_t)(x))<<DAC_DATL_DATA0_SHIFT))&DAC_DATL_DATA0_MASK)
/* DATH Bit Fields */
#define DAC_DATH_DATA1_MASK                      0xFu
#define DAC_DATH_DATA1_SHIFT                     0
#define DAC_DATH_DATA1(x)                        (((uint8_t)(((uint8_t)(x))<<DAC_DATH_DATA1_SHIFT))&DAC_DATH_DATA1_MASK)
/* SR Bit Fields */
#define DAC_SR_DACBFRPBF_MASK                    0x1u
#define DAC_SR_DACBFRPBF_SHIFT                   0
#define DAC_SR_DACBFRPTF_MASK                    0x2u
#define DAC_SR_DACBFRPTF_SHIFT                   1
#define DAC_SR_DACBFWMF_MASK                     0x4u
#define DAC_SR_DACBFWMF_SHIFT                    2
/* C0 Bit Fields */
#define DAC_C0_DACBBIEN_MASK                     0x1u
#define DAC_C0_DACBBIEN_SHIFT                    0
#define DAC_C0_DACBTIEN_MASK                     0x2u
#define DAC_C0_DACBTIEN_SHIFT                    1
#define DAC_C0_DACBWIEN_MASK                     0x4u
#define DAC_C0_DACBWIEN_SHIFT                    2
#define DAC_C0_LPEN_MASK                         0x8u
#define DAC_C0_LPEN_SHIFT                        3
#define DAC_C0_DACSWTRG_MASK                     0x10u
#define DAC_C0_DACSWTRG_SHIFT                    4
#define DAC_C0_DACTRGSEL_MASK                    0x20u
#define DAC_C0_DACTRGSEL_SHIFT                   5
#define DAC_C0_DACRFS_MASK                       0x40u
#define DAC_C0_DACRFS_SHIFT                      6
#define DAC_C0_DACEN_MASK                        0x80u
#define DAC_C0_DACEN_SHIFT                       7
/* C1 Bit Fields */
#define DAC_C1_DACBFEN_MASK                      0x1u
#define DAC_C1_DACBFEN_SHIFT                     0
#define DAC_C1_DACBFMD_MASK                      0x6u
#define DAC_C1_DACBFMD_SHIFT                     1
#define DAC_C1_DACBFMD(x)                        (((uint8_t)(((uint8_t)(x))<<DAC_C1_DACBFMD_SHIFT))&DAC_C1_DACBFMD_MASK)
#define DAC_C1_DACBFWM_MASK                      0x18u
#define DAC_C1_DACBFWM_SHIFT                     3
#define DAC_C1_DACBFWM(x)                        (((uint8_t)(((uint8_t)(x))<<DAC_C1_DACBFWM_SHIFT))&DAC_C1_DACBFWM_MASK)
#define DAC_C1_DMAEN_MASK                        0x80u
#define DAC_C1_DMAEN_SHIFT                       7
/* C2 Bit Fields */
#define DAC_C2_DACBFUP_MASK                      0xFu
#define DAC_C2_DACBFUP_SHIFT                     0
#define DAC_C2_DACBFUP(x)                        (((uint8_t)(((uint8_t)(x))<<DAC_C2_DACBFUP_SHIFT))&DAC_C2_DACBFUP_MASK)
#define DAC_C2_DACBFRP_MASK                      0xF0u
#define DAC_C2_DACBFRP_SHIFT                     4
#define DAC_C2_DACBFRP(x)                        (((uint8_t)(((uint8_t)(x))<<DAC_C2_DACBFRP_SHIFT))&DAC_C2_DACBFRP_MASK)

/**
 * @}
 */ /* end of group DAC_Register_Masks */


/* DAC - Peripheral instance base addresses */
/** Peripheral DAC0 base address */
#define DAC0_BASE                                (0x400CC000u)
/** Peripheral DAC0 base pointer */
#define DAC0                                     ((DAC_Type *)DAC0_BASE)
/** Peripheral DAC1 base address */
#define DAC1_BASE                                (0x400CD000u)
/** Peripheral DAC1 base pointer */
#define DAC1                                     ((DAC_Type *)DAC1_BASE)
/** Array initializer of DAC peripheral base pointers */
#define DAC_BASES                                { DAC0, DAC1 }

/**
 * @}
 */ /* end of group DAC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- DMA Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup DMA_Peripheral_Access_Layer DMA Peripheral Access Layer
 * @{
 */

/** DMA - Register Layout Typedef */
typedef struct {
  __IO uint32_t CR;                                /**< Control Register, offset: 0x0 */
  __I  uint32_t ES;                                /**< Error Status Register, offset: 0x4 */
       uint8_t RESERVED_0[4];
  __IO uint32_t ERQ;                               /**< Enable Request Register, offset: 0xC */
       uint8_t RESERVED_1[4];
  __IO uint32_t EEI;                               /**< Enable Error Interrupt Register, offset: 0x14 */
  __O  uint8_t CEEI;                               /**< Clear Enable Error Interrupt Register, offset: 0x18 */
  __O  uint8_t SEEI;                               /**< Set Enable Error Interrupt Register, offset: 0x19 */
  __O  uint8_t CERQ;                               /**< Clear Enable Request Register, offset: 0x1A */
  __O  uint8_t SERQ;                               /**< Set Enable Request Register, offset: 0x1B */
  __O  uint8_t CDNE;                               /**< Clear DONE Status Bit Register, offset: 0x1C */
  __O  uint8_t SSRT;                               /**< Set START Bit Register, offset: 0x1D */
  __O  uint8_t CERR;                               /**< Clear Error Register, offset: 0x1E */
  __O  uint8_t CINT;                               /**< Clear Interrupt Request Register, offset: 0x1F */
       uint8_t RESERVED_2[4];
  __IO uint32_t INT;                               /**< Interrupt Request Register, offset: 0x24 */
       uint8_t RESERVED_3[4];
  __IO uint32_t ERR;                               /**< Error Register, offset: 0x2C */
       uint8_t RESERVED_4[4];
  __IO uint32_t HRS;                               /**< Hardware Request Status Register, offset: 0x34 */
       uint8_t RESERVED_5[200];
  __IO uint8_t DCHPRI3;                            /**< Channel n Priority Register, offset: 0x100 */
  __IO uint8_t DCHPRI2;                            /**< Channel n Priority Register, offset: 0x101 */
  __IO uint8_t DCHPRI1;                            /**< Channel n Priority Register, offset: 0x102 */
  __IO uint8_t DCHPRI0;                            /**< Channel n Priority Register, offset: 0x103 */
  __IO uint8_t DCHPRI7;                            /**< Channel n Priority Register, offset: 0x104 */
  __IO uint8_t DCHPRI6;                            /**< Channel n Priority Register, offset: 0x105 */
  __IO uint8_t DCHPRI5;                            /**< Channel n Priority Register, offset: 0x106 */
  __IO uint8_t DCHPRI4;                            /**< Channel n Priority Register, offset: 0x107 */
  __IO uint8_t DCHPRI11;                           /**< Channel n Priority Register, offset: 0x108 */
  __IO uint8_t DCHPRI10;                           /**< Channel n Priority Register, offset: 0x109 */
  __IO uint8_t DCHPRI9;                            /**< Channel n Priority Register, offset: 0x10A */
  __IO uint8_t DCHPRI8;                            /**< Channel n Priority Register, offset: 0x10B */
  __IO uint8_t DCHPRI15;                           /**< Channel n Priority Register, offset: 0x10C */
  __IO uint8_t DCHPRI14;                           /**< Channel n Priority Register, offset: 0x10D */
  __IO uint8_t DCHPRI13;                           /**< Channel n Priority Register, offset: 0x10E */
  __IO uint8_t DCHPRI12;                           /**< Channel n Priority Register, offset: 0x10F */
       uint8_t RESERVED_6[3824];
  struct {                                         /* offset: 0x1000, array step: 0x20 */
    __IO uint32_t SADDR;                             /**< TCD Source Address, array offset: 0x1000, array step: 0x20 */
    __IO uint16_t SOFF;                              /**< TCD Signed Source Address Offset, array offset: 0x1004, array step: 0x20 */
    __IO uint16_t ATTR;                              /**< TCD Transfer Attributes, array offset: 0x1006, array step: 0x20 */
    union {                                          /* offset: 0x1008, array step: 0x20 */
      __IO uint32_t NBYTES_MLNO;                       /**< TCD Minor Byte Count (Minor Loop Disabled), array offset: 0x1008, array step: 0x20 */
      __IO uint32_t NBYTES_MLOFFNO;                    /**< TCD Signed Minor Loop Offset (Minor Loop Enabled and Offset Disabled), array offset: 0x1008, array step: 0x20 */
      __IO uint32_t NBYTES_MLOFFYES;                   /**< TCD Signed Minor Loop Offset (Minor Loop and Offset Enabled), array offset: 0x1008, array step: 0x20 */
    };
    __IO uint32_t SLAST;                             /**< TCD Last Source Address Adjustment, array offset: 0x100C, array step: 0x20 */
    __IO uint32_t DADDR;                             /**< TCD Destination Address, array offset: 0x1010, array step: 0x20 */
    __IO uint16_t DOFF;                              /**< TCD Signed Destination Address Offset, array offset: 0x1014, array step: 0x20 */
    union {                                          /* offset: 0x1016, array step: 0x20 */
      __IO uint16_t CITER_ELINKNO;                     /**< TCD Current Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x1016, array step: 0x20 */
      __IO uint16_t CITER_ELINKYES;                    /**< TCD Current Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x1016, array step: 0x20 */
    };
    __IO uint32_t DLAST_SGA;                         /**< TCD Last Destination Address Adjustment/Scatter Gather Address, array offset: 0x1018, array step: 0x20 */
    __IO uint16_t CSR;                               /**< TCD Control and Status, array offset: 0x101C, array step: 0x20 */
    union {                                          /* offset: 0x101E, array step: 0x20 */
      __IO uint16_t BITER_ELINKNO;                     /**< TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x101E, array step: 0x20 */
      __IO uint16_t BITER_ELINKYES;                    /**< TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x101E, array step: 0x20 */
    };
  } TCD[16];
} DMA_Type;

/* ----------------------------------------------------------------------------
   -- DMA Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup DMA_Register_Masks DMA Register Masks
 * @{
 */

/* CR Bit Fields */
#define DMA_CR_EDBG_MASK                         0x2u
#define DMA_CR_EDBG_SHIFT                        1
#define DMA_CR_ERCA_MASK                         0x4u
#define DMA_CR_ERCA_SHIFT                        2
#define DMA_CR_HOE_MASK                          0x10u
#define DMA_CR_HOE_SHIFT                         4
#define DMA_CR_HALT_MASK                         0x20u
#define DMA_CR_HALT_SHIFT                        5
#define DMA_CR_CLM_MASK                          0x40u
#define DMA_CR_CLM_SHIFT                         6
#define DMA_CR_EMLM_MASK                         0x80u
#define DMA_CR_EMLM_SHIFT                        7
#define DMA_CR_ECX_MASK                          0x10000u
#define DMA_CR_ECX_SHIFT                         16
#define DMA_CR_CX_MASK                           0x20000u
#define DMA_CR_CX_SHIFT                          17
/* ES Bit Fields */
#define DMA_ES_DBE_MASK                          0x1u
#define DMA_ES_DBE_SHIFT                         0
#define DMA_ES_SBE_MASK                          0x2u
#define DMA_ES_SBE_SHIFT                         1
#define DMA_ES_SGE_MASK                          0x4u
#define DMA_ES_SGE_SHIFT                         2
#define DMA_ES_NCE_MASK                          0x8u
#define DMA_ES_NCE_SHIFT                         3
#define DMA_ES_DOE_MASK                          0x10u
#define DMA_ES_DOE_SHIFT                         4
#define DMA_ES_DAE_MASK                          0x20u
#define DMA_ES_DAE_SHIFT                         5
#define DMA_ES_SOE_MASK                          0x40u
#define DMA_ES_SOE_SHIFT                         6
#define DMA_ES_SAE_MASK                          0x80u
#define DMA_ES_SAE_SHIFT                         7
#define DMA_ES_ERRCHN_MASK                       0xF00u
#define DMA_ES_ERRCHN_SHIFT                      8
#define DMA_ES_ERRCHN(x)                         (((uint32_t)(((uint32_t)(x))<<DMA_ES_ERRCHN_SHIFT))&DMA_ES_ERRCHN_MASK)
#define DMA_ES_CPE_MASK                          0x4000u
#define DMA_ES_CPE_SHIFT                         14
#define DMA_ES_ECX_MASK                          0x10000u
#define DMA_ES_ECX_SHIFT                         16
#define DMA_ES_VLD_MASK                          0x80000000u
#define DMA_ES_VLD_SHIFT                         31
/* ERQ Bit Fields */
#define DMA_ERQ_ERQ0_MASK                        0x1u
#define DMA_ERQ_ERQ0_SHIFT                       0
#define DMA_ERQ_ERQ1_MASK                        0x2u
#define DMA_ERQ_ERQ1_SHIFT                       1
#define DMA_ERQ_ERQ2_MASK                        0x4u
#define DMA_ERQ_ERQ2_SHIFT                       2
#define DMA_ERQ_ERQ3_MASK                        0x8u
#define DMA_ERQ_ERQ3_SHIFT                       3
#define DMA_ERQ_ERQ4_MASK                        0x10u
#define DMA_ERQ_ERQ4_SHIFT                       4
#define DMA_ERQ_ERQ5_MASK                        0x20u
#define DMA_ERQ_ERQ5_SHIFT                       5
#define DMA_ERQ_ERQ6_MASK                        0x40u
#define DMA_ERQ_ERQ6_SHIFT                       6
#define DMA_ERQ_ERQ7_MASK                        0x80u
#define DMA_ERQ_ERQ7_SHIFT                       7
#define DMA_ERQ_ERQ8_MASK                        0x100u
#define DMA_ERQ_ERQ8_SHIFT                       8
#define DMA_ERQ_ERQ9_MASK                        0x200u
#define DMA_ERQ_ERQ9_SHIFT                       9
#define DMA_ERQ_ERQ10_MASK                       0x400u
#define DMA_ERQ_ERQ10_SHIFT                      10
#define DMA_ERQ_ERQ11_MASK                       0x800u
#define DMA_ERQ_ERQ11_SHIFT                      11
#define DMA_ERQ_ERQ12_MASK                       0x1000u
#define DMA_ERQ_ERQ12_SHIFT                      12
#define DMA_ERQ_ERQ13_MASK                       0x2000u
#define DMA_ERQ_ERQ13_SHIFT                      13
#define DMA_ERQ_ERQ14_MASK                       0x4000u
#define DMA_ERQ_ERQ14_SHIFT                      14
#define DMA_ERQ_ERQ15_MASK                       0x8000u
#define DMA_ERQ_ERQ15_SHIFT                      15
/* EEI Bit Fields */
#define DMA_EEI_EEI0_MASK                        0x1u
#define DMA_EEI_EEI0_SHIFT                       0
#define DMA_EEI_EEI1_MASK                        0x2u
#define DMA_EEI_EEI1_SHIFT                       1
#define DMA_EEI_EEI2_MASK                        0x4u
#define DMA_EEI_EEI2_SHIFT                       2
#define DMA_EEI_EEI3_MASK                        0x8u
#define DMA_EEI_EEI3_SHIFT                       3
#define DMA_EEI_EEI4_MASK                        0x10u
#define DMA_EEI_EEI4_SHIFT                       4
#define DMA_EEI_EEI5_MASK                        0x20u
#define DMA_EEI_EEI5_SHIFT                       5
#define DMA_EEI_EEI6_MASK                        0x40u
#define DMA_EEI_EEI6_SHIFT                       6
#define DMA_EEI_EEI7_MASK                        0x80u
#define DMA_EEI_EEI7_SHIFT                       7
#define DMA_EEI_EEI8_MASK                        0x100u
#define DMA_EEI_EEI8_SHIFT                       8
#define DMA_EEI_EEI9_MASK                        0x200u
#define DMA_EEI_EEI9_SHIFT                       9
#define DMA_EEI_EEI10_MASK                       0x400u
#define DMA_EEI_EEI10_SHIFT                      10
#define DMA_EEI_EEI11_MASK                       0x800u
#define DMA_EEI_EEI11_SHIFT                      11
#define DMA_EEI_EEI12_MASK                       0x1000u
#define DMA_EEI_EEI12_SHIFT                      12
#define DMA_EEI_EEI13_MASK                       0x2000u
#define DMA_EEI_EEI13_SHIFT                      13
#define DMA_EEI_EEI14_MASK                       0x4000u
#define DMA_EEI_EEI14_SHIFT                      14
#define DMA_EEI_EEI15_MASK                       0x8000u
#define DMA_EEI_EEI15_SHIFT                      15
/* CEEI Bit Fields */
#define DMA_CEEI_CEEI_MASK                       0xFu
#define DMA_CEEI_CEEI_SHIFT                      0
#define DMA_CEEI_CEEI(x)                         (((uint8_t)(((uint8_t)(x))<<DMA_CEEI_CEEI_SHIFT))&DMA_CEEI_CEEI_MASK)
#define DMA_CEEI_CAEE_MASK                       0x40u
#define DMA_CEEI_CAEE_SHIFT                      6
#define DMA_CEEI_NOP_MASK                        0x80u
#define DMA_CEEI_NOP_SHIFT                       7
/* SEEI Bit Fields */
#define DMA_SEEI_SEEI_MASK                       0xFu
#define DMA_SEEI_SEEI_SHIFT                      0
#define DMA_SEEI_SEEI(x)                         (((uint8_t)(((uint8_t)(x))<<DMA_SEEI_SEEI_SHIFT))&DMA_SEEI_SEEI_MASK)
#define DMA_SEEI_SAEE_MASK                       0x40u
#define DMA_SEEI_SAEE_SHIFT                      6
#define DMA_SEEI_NOP_MASK                        0x80u
#define DMA_SEEI_NOP_SHIFT                       7
/* CERQ Bit Fields */
#define DMA_CERQ_CERQ_MASK                       0xFu
#define DMA_CERQ_CERQ_SHIFT                      0
#define DMA_CERQ_CERQ(x)                         (((uint8_t)(((uint8_t)(x))<<DMA_CERQ_CERQ_SHIFT))&DMA_CERQ_CERQ_MASK)
#define DMA_CERQ_CAER_MASK                       0x40u
#define DMA_CERQ_CAER_SHIFT                      6
#define DMA_CERQ_NOP_MASK                        0x80u
#define DMA_CERQ_NOP_SHIFT                       7
/* SERQ Bit Fields */
#define DMA_SERQ_SERQ_MASK                       0xFu
#define DMA_SERQ_SERQ_SHIFT                      0
#define DMA_SERQ_SERQ(x)                         (((uint8_t)(((uint8_t)(x))<<DMA_SERQ_SERQ_SHIFT))&DMA_SERQ_SERQ_MASK)
#define DMA_SERQ_SAER_MASK                       0x40u
#define DMA_SERQ_SAER_SHIFT                      6
#define DMA_SERQ_NOP_MASK                        0x80u
#define DMA_SERQ_NOP_SHIFT                       7
/* CDNE Bit Fields */
#define DMA_CDNE_CDNE_MASK                       0xFu
#define DMA_CDNE_CDNE_SHIFT                      0
#define DMA_CDNE_CDNE(x)                         (((uint8_t)(((uint8_t)(x))<<DMA_CDNE_CDNE_SHIFT))&DMA_CDNE_CDNE_MASK)
#define DMA_CDNE_CADN_MASK                       0x40u
#define DMA_CDNE_CADN_SHIFT                      6
#define DMA_CDNE_NOP_MASK                        0x80u
#define DMA_CDNE_NOP_SHIFT                       7
/* SSRT Bit Fields */
#define DMA_SSRT_SSRT_MASK                       0xFu
#define DMA_SSRT_SSRT_SHIFT                      0
#define DMA_SSRT_SSRT(x)                         (((uint8_t)(((uint8_t)(x))<<DMA_SSRT_SSRT_SHIFT))&DMA_SSRT_SSRT_MASK)
#define DMA_SSRT_SAST_MASK                       0x40u
#define DMA_SSRT_SAST_SHIFT                      6
#define DMA_SSRT_NOP_MASK                        0x80u
#define DMA_SSRT_NOP_SHIFT                       7
/* CERR Bit Fields */
#define DMA_CERR_CERR_MASK                       0xFu
#define DMA_CERR_CERR_SHIFT                      0
#define DMA_CERR_CERR(x)                         (((uint8_t)(((uint8_t)(x))<<DMA_CERR_CERR_SHIFT))&DMA_CERR_CERR_MASK)
#define DMA_CERR_CAEI_MASK                       0x40u
#define DMA_CERR_CAEI_SHIFT                      6
#define DMA_CERR_NOP_MASK                        0x80u
#define DMA_CERR_NOP_SHIFT                       7
/* CINT Bit Fields */
#define DMA_CINT_CINT_MASK                       0xFu
#define DMA_CINT_CINT_SHIFT                      0
#define DMA_CINT_CINT(x)                         (((uint8_t)(((uint8_t)(x))<<DMA_CINT_CINT_SHIFT))&DMA_CINT_CINT_MASK)
#define DMA_CINT_CAIR_MASK                       0x40u
#define DMA_CINT_CAIR_SHIFT                      6
#define DMA_CINT_NOP_MASK                        0x80u
#define DMA_CINT_NOP_SHIFT                       7
/* INT Bit Fields */
#define DMA_INT_INT0_MASK                        0x1u
#define DMA_INT_INT0_SHIFT                       0
#define DMA_INT_INT1_MASK                        0x2u
#define DMA_INT_INT1_SHIFT                       1
#define DMA_INT_INT2_MASK                        0x4u
#define DMA_INT_INT2_SHIFT                       2
#define DMA_INT_INT3_MASK                        0x8u
#define DMA_INT_INT3_SHIFT                       3
#define DMA_INT_INT4_MASK                        0x10u
#define DMA_INT_INT4_SHIFT                       4
#define DMA_INT_INT5_MASK                        0x20u
#define DMA_INT_INT5_SHIFT                       5
#define DMA_INT_INT6_MASK                        0x40u
#define DMA_INT_INT6_SHIFT                       6
#define DMA_INT_INT7_MASK                        0x80u
#define DMA_INT_INT7_SHIFT                       7
#define DMA_INT_INT8_MASK                        0x100u
#define DMA_INT_INT8_SHIFT                       8
#define DMA_INT_INT9_MASK                        0x200u
#define DMA_INT_INT9_SHIFT                       9
#define DMA_INT_INT10_MASK                       0x400u
#define DMA_INT_INT10_SHIFT                      10
#define DMA_INT_INT11_MASK                       0x800u
#define DMA_INT_INT11_SHIFT                      11
#define DMA_INT_INT12_MASK                       0x1000u
#define DMA_INT_INT12_SHIFT                      12
#define DMA_INT_INT13_MASK                       0x2000u
#define DMA_INT_INT13_SHIFT                      13
#define DMA_INT_INT14_MASK                       0x4000u
#define DMA_INT_INT14_SHIFT                      14
#define DMA_INT_INT15_MASK                       0x8000u
#define DMA_INT_INT15_SHIFT                      15
/* ERR Bit Fields */
#define DMA_ERR_ERR0_MASK                        0x1u
#define DMA_ERR_ERR0_SHIFT                       0
#define DMA_ERR_ERR1_MASK                        0x2u
#define DMA_ERR_ERR1_SHIFT                       1
#define DMA_ERR_ERR2_MASK                        0x4u
#define DMA_ERR_ERR2_SHIFT                       2
#define DMA_ERR_ERR3_MASK                        0x8u
#define DMA_ERR_ERR3_SHIFT                       3
#define DMA_ERR_ERR4_MASK                        0x10u
#define DMA_ERR_ERR4_SHIFT                       4
#define DMA_ERR_ERR5_MASK                        0x20u
#define DMA_ERR_ERR5_SHIFT                       5
#define DMA_ERR_ERR6_MASK                        0x40u
#define DMA_ERR_ERR6_SHIFT                       6
#define DMA_ERR_ERR7_MASK                        0x80u
#define DMA_ERR_ERR7_SHIFT                       7
#define DMA_ERR_ERR8_MASK                        0x100u
#define DMA_ERR_ERR8_SHIFT                       8
#define DMA_ERR_ERR9_MASK                        0x200u
#define DMA_ERR_ERR9_SHIFT                       9
#define DMA_ERR_ERR10_MASK                       0x400u
#define DMA_ERR_ERR10_SHIFT                      10
#define DMA_ERR_ERR11_MASK                       0x800u
#define DMA_ERR_ERR11_SHIFT                      11
#define DMA_ERR_ERR12_MASK                       0x1000u
#define DMA_ERR_ERR12_SHIFT                      12
#define DMA_ERR_ERR13_MASK                       0x2000u
#define DMA_ERR_ERR13_SHIFT                      13
#define DMA_ERR_ERR14_MASK                       0x4000u
#define DMA_ERR_ERR14_SHIFT                      14
#define DMA_ERR_ERR15_MASK                       0x8000u
#define DMA_ERR_ERR15_SHIFT                      15
/* HRS Bit Fields */
#define DMA_HRS_HRS0_MASK                        0x1u
#define DMA_HRS_HRS0_SHIFT                       0
#define DMA_HRS_HRS1_MASK                        0x2u
#define DMA_HRS_HRS1_SHIFT                       1
#define DMA_HRS_HRS2_MASK                        0x4u
#define DMA_HRS_HRS2_SHIFT                       2
#define DMA_HRS_HRS3_MASK                        0x8u
#define DMA_HRS_HRS3_SHIFT                       3
#define DMA_HRS_HRS4_MASK                        0x10u
#define DMA_HRS_HRS4_SHIFT                       4
#define DMA_HRS_HRS5_MASK                        0x20u
#define DMA_HRS_HRS5_SHIFT                       5
#define DMA_HRS_HRS6_MASK                        0x40u
#define DMA_HRS_HRS6_SHIFT                       6
#define DMA_HRS_HRS7_MASK                        0x80u
#define DMA_HRS_HRS7_SHIFT                       7
#define DMA_HRS_HRS8_MASK                        0x100u
#define DMA_HRS_HRS8_SHIFT                       8
#define DMA_HRS_HRS9_MASK                        0x200u
#define DMA_HRS_HRS9_SHIFT                       9
#define DMA_HRS_HRS10_MASK                       0x400u
#define DMA_HRS_HRS10_SHIFT                      10
#define DMA_HRS_HRS11_MASK                       0x800u
#define DMA_HRS_HRS11_SHIFT                      11
#define DMA_HRS_HRS12_MASK                       0x1000u
#define DMA_HRS_HRS12_SHIFT                      12
#define DMA_HRS_HRS13_MASK                       0x2000u
#define DMA_HRS_HRS13_SHIFT                      13
#define DMA_HRS_HRS14_MASK                       0x4000u
#define DMA_HRS_HRS14_SHIFT                      14
#define DMA_HRS_HRS15_MASK                       0x8000u
#define DMA_HRS_HRS15_SHIFT                      15
/* DCHPRI3 Bit Fields */
#define DMA_DCHPRI3_CHPRI_MASK                   0xFu
#define DMA_DCHPRI3_CHPRI_SHIFT                  0
#define DMA_DCHPRI3_CHPRI(x)                     (((uint8_t)(((uint8_t)(x))<<DMA_DCHPRI3_CHPRI_SHIFT))&DMA_DCHPRI3_CHPRI_MASK)
#define DMA_DCHPRI3_DPA_MASK                     0x40u
#define DMA_DCHPRI3_DPA_SHIFT                    6
#define DMA_DCHPRI3_ECP_MASK                     0x80u
#define DMA_DCHPRI3_ECP_SHIFT                    7
/* DCHPRI2 Bit Fields */
#define DMA_DCHPRI2_CHPRI_MASK                   0xFu
#define DMA_DCHPRI2_CHPRI_SHIFT                  0
#define DMA_DCHPRI2_CHPRI(x)                     (((uint8_t)(((uint8_t)(x))<<DMA_DCHPRI2_CHPRI_SHIFT))&DMA_DCHPRI2_CHPRI_MASK)
#define DMA_DCHPRI2_DPA_MASK                     0x40u
#define DMA_DCHPRI2_DPA_SHIFT                    6
#define DMA_DCHPRI2_ECP_MASK                     0x80u
#define DMA_DCHPRI2_ECP_SHIFT                    7
/* DCHPRI1 Bit Fields */
#define DMA_DCHPRI1_CHPRI_MASK                   0xFu
#define DMA_DCHPRI1_CHPRI_SHIFT                  0
#define DMA_DCHPRI1_CHPRI(x)                     (((uint8_t)(((uint8_t)(x))<<DMA_DCHPRI1_CHPRI_SHIFT))&DMA_DCHPRI1_CHPRI_MASK)
#define DMA_DCHPRI1_DPA_MASK                     0x40u
#define DMA_DCHPRI1_DPA_SHIFT                    6
#define DMA_DCHPRI1_ECP_MASK                     0x80u
#define DMA_DCHPRI1_ECP_SHIFT                    7
/* DCHPRI0 Bit Fields */
#define DMA_DCHPRI0_CHPRI_MASK                   0xFu
#define DMA_DCHPRI0_CHPRI_SHIFT                  0
#define DMA_DCHPRI0_CHPRI(x)                     (((uint8_t)(((uint8_t)(x))<<DMA_DCHPRI0_CHPRI_SHIFT))&DMA_DCHPRI0_CHPRI_MASK)
#define DMA_DCHPRI0_DPA_MASK                     0x40u
#define DMA_DCHPRI0_DPA_SHIFT                    6
#define DMA_DCHPRI0_ECP_MASK                     0x80u
#define DMA_DCHPRI0_ECP_SHIFT                    7
/* DCHPRI7 Bit Fields */
#define DMA_DCHPRI7_CHPRI_MASK                   0xFu
#define DMA_DCHPRI7_CHPRI_SHIFT                  0
#define DMA_DCHPRI7_CHPRI(x)                     (((uint8_t)(((uint8_t)(x))<<DMA_DCHPRI7_CHPRI_SHIFT))&DMA_DCHPRI7_CHPRI_MASK)
#define DMA_DCHPRI7_DPA_MASK                     0x40u
#define DMA_DCHPRI7_DPA_SHIFT                    6
#define DMA_DCHPRI7_ECP_MASK                     0x80u
#define DMA_DCHPRI7_ECP_SHIFT                    7
/* DCHPRI6 Bit Fields */
#define DMA_DCHPRI6_CHPRI_MASK                   0xFu
#define DMA_DCHPRI6_CHPRI_SHIFT                  0
#define DMA_DCHPRI6_CHPRI(x)                     (((uint8_t)(((uint8_t)(x))<<DMA_DCHPRI6_CHPRI_SHIFT))&DMA_DCHPRI6_CHPRI_MASK)
#define DMA_DCHPRI6_DPA_MASK                     0x40u
#define DMA_DCHPRI6_DPA_SHIFT                    6
#define DMA_DCHPRI6_ECP_MASK                     0x80u
#define DMA_DCHPRI6_ECP_SHIFT                    7
/* DCHPRI5 Bit Fields */
#define DMA_DCHPRI5_CHPRI_MASK                   0xFu
#define DMA_DCHPRI5_CHPRI_SHIFT                  0
#define DMA_DCHPRI5_CHPRI(x)                     (((uint8_t)(((uint8_t)(x))<<DMA_DCHPRI5_CHPRI_SHIFT))&DMA_DCHPRI5_CHPRI_MASK)
#define DMA_DCHPRI5_DPA_MASK                     0x40u
#define DMA_DCHPRI5_DPA_SHIFT                    6
#define DMA_DCHPRI5_ECP_MASK                     0x80u
#define DMA_DCHPRI5_ECP_SHIFT                    7
/* DCHPRI4 Bit Fields */
#define DMA_DCHPRI4_CHPRI_MASK                   0xFu
#define DMA_DCHPRI4_CHPRI_SHIFT                  0
#define DMA_DCHPRI4_CHPRI(x)                     (((uint8_t)(((uint8_t)(x))<<DMA_DCHPRI4_CHPRI_SHIFT))&DMA_DCHPRI4_CHPRI_MASK)
#define DMA_DCHPRI4_DPA_MASK                     0x40u
#define DMA_DCHPRI4_DPA_SHIFT                    6
#define DMA_DCHPRI4_ECP_MASK                     0x80u
#define DMA_DCHPRI4_ECP_SHIFT                    7
/* DCHPRI11 Bit Fields */
#define DMA_DCHPRI11_CHPRI_MASK                  0xFu
#define DMA_DCHPRI11_CHPRI_SHIFT                 0
#define DMA_DCHPRI11_CHPRI(x)                    (((uint8_t)(((uint8_t)(x))<<DMA_DCHPRI11_CHPRI_SHIFT))&DMA_DCHPRI11_CHPRI_MASK)
#define DMA_DCHPRI11_DPA_MASK                    0x40u
#define DMA_DCHPRI11_DPA_SHIFT                   6
#define DMA_DCHPRI11_ECP_MASK                    0x80u
#define DMA_DCHPRI11_ECP_SHIFT                   7
/* DCHPRI10 Bit Fields */
#define DMA_DCHPRI10_CHPRI_MASK                  0xFu
#define DMA_DCHPRI10_CHPRI_SHIFT                 0
#define DMA_DCHPRI10_CHPRI(x)                    (((uint8_t)(((uint8_t)(x))<<DMA_DCHPRI10_CHPRI_SHIFT))&DMA_DCHPRI10_CHPRI_MASK)
#define DMA_DCHPRI10_DPA_MASK                    0x40u
#define DMA_DCHPRI10_DPA_SHIFT                   6
#define DMA_DCHPRI10_ECP_MASK                    0x80u
#define DMA_DCHPRI10_ECP_SHIFT                   7
/* DCHPRI9 Bit Fields */
#define DMA_DCHPRI9_CHPRI_MASK                   0xFu
#define DMA_DCHPRI9_CHPRI_SHIFT                  0
#define DMA_DCHPRI9_CHPRI(x)                     (((uint8_t)(((uint8_t)(x))<<DMA_DCHPRI9_CHPRI_SHIFT))&DMA_DCHPRI9_CHPRI_MASK)
#define DMA_DCHPRI9_DPA_MASK                     0x40u
#define DMA_DCHPRI9_DPA_SHIFT                    6
#define DMA_DCHPRI9_ECP_MASK                     0x80u
#define DMA_DCHPRI9_ECP_SHIFT                    7
/* DCHPRI8 Bit Fields */
#define DMA_DCHPRI8_CHPRI_MASK                   0xFu
#define DMA_DCHPRI8_CHPRI_SHIFT                  0
#define DMA_DCHPRI8_CHPRI(x)                     (((uint8_t)(((uint8_t)(x))<<DMA_DCHPRI8_CHPRI_SHIFT))&DMA_DCHPRI8_CHPRI_MASK)
#define DMA_DCHPRI8_DPA_MASK                     0x40u
#define DMA_DCHPRI8_DPA_SHIFT                    6
#define DMA_DCHPRI8_ECP_MASK                     0x80u
#define DMA_DCHPRI8_ECP_SHIFT                    7
/* DCHPRI15 Bit Fields */
#define DMA_DCHPRI15_CHPRI_MASK                  0xFu
#define DMA_DCHPRI15_CHPRI_SHIFT                 0
#define DMA_DCHPRI15_CHPRI(x)                    (((uint8_t)(((uint8_t)(x))<<DMA_DCHPRI15_CHPRI_SHIFT))&DMA_DCHPRI15_CHPRI_MASK)
#define DMA_DCHPRI15_DPA_MASK                    0x40u
#define DMA_DCHPRI15_DPA_SHIFT                   6
#define DMA_DCHPRI15_ECP_MASK                    0x80u
#define DMA_DCHPRI15_ECP_SHIFT                   7
/* DCHPRI14 Bit Fields */
#define DMA_DCHPRI14_CHPRI_MASK                  0xFu
#define DMA_DCHPRI14_CHPRI_SHIFT                 0
#define DMA_DCHPRI14_CHPRI(x)                    (((uint8_t)(((uint8_t)(x))<<DMA_DCHPRI14_CHPRI_SHIFT))&DMA_DCHPRI14_CHPRI_MASK)
#define DMA_DCHPRI14_DPA_MASK                    0x40u
#define DMA_DCHPRI14_DPA_SHIFT                   6
#define DMA_DCHPRI14_ECP_MASK                    0x80u
#define DMA_DCHPRI14_ECP_SHIFT                   7
/* DCHPRI13 Bit Fields */
#define DMA_DCHPRI13_CHPRI_MASK                  0xFu
#define DMA_DCHPRI13_CHPRI_SHIFT                 0
#define DMA_DCHPRI13_CHPRI(x)                    (((uint8_t)(((uint8_t)(x))<<DMA_DCHPRI13_CHPRI_SHIFT))&DMA_DCHPRI13_CHPRI_MASK)
#define DMA_DCHPRI13_DPA_MASK                    0x40u
#define DMA_DCHPRI13_DPA_SHIFT                   6
#define DMA_DCHPRI13_ECP_MASK                    0x80u
#define DMA_DCHPRI13_ECP_SHIFT                   7
/* DCHPRI12 Bit Fields */
#define DMA_DCHPRI12_CHPRI_MASK                  0xFu
#define DMA_DCHPRI12_CHPRI_SHIFT                 0
#define DMA_DCHPRI12_CHPRI(x)                    (((uint8_t)(((uint8_t)(x))<<DMA_DCHPRI12_CHPRI_SHIFT))&DMA_DCHPRI12_CHPRI_MASK)
#define DMA_DCHPRI12_DPA_MASK                    0x40u
#define DMA_DCHPRI12_DPA_SHIFT                   6
#define DMA_DCHPRI12_ECP_MASK                    0x80u
#define DMA_DCHPRI12_ECP_SHIFT                   7
/* SADDR Bit Fields */
#define DMA_SADDR_SADDR_MASK                     0xFFFFFFFFu
#define DMA_SADDR_SADDR_SHIFT                    0
#define DMA_SADDR_SADDR(x)                       (((uint32_t)(((uint32_t)(x))<<DMA_SADDR_SADDR_SHIFT))&DMA_SADDR_SADDR_MASK)
/* SOFF Bit Fields */
#define DMA_SOFF_SOFF_MASK                       0xFFFFu
#define DMA_SOFF_SOFF_SHIFT                      0
#define DMA_SOFF_SOFF(x)                         (((uint16_t)(((uint16_t)(x))<<DMA_SOFF_SOFF_SHIFT))&DMA_SOFF_SOFF_MASK)
/* ATTR Bit Fields */
#define DMA_ATTR_DSIZE_MASK                      0x7u
#define DMA_ATTR_DSIZE_SHIFT                     0
#define DMA_ATTR_DSIZE(x)                        (((uint16_t)(((uint16_t)(x))<<DMA_ATTR_DSIZE_SHIFT))&DMA_ATTR_DSIZE_MASK)
#define DMA_ATTR_DMOD_MASK                       0xF8u
#define DMA_ATTR_DMOD_SHIFT                      3
#define DMA_ATTR_DMOD(x)                         (((uint16_t)(((uint16_t)(x))<<DMA_ATTR_DMOD_SHIFT))&DMA_ATTR_DMOD_MASK)
#define DMA_ATTR_SSIZE_MASK                      0x700u
#define DMA_ATTR_SSIZE_SHIFT                     8
#define DMA_ATTR_SSIZE(x)                        (((uint16_t)(((uint16_t)(x))<<DMA_ATTR_SSIZE_SHIFT))&DMA_ATTR_SSIZE_MASK)
#define DMA_ATTR_SMOD_MASK                       0xF800u
#define DMA_ATTR_SMOD_SHIFT                      11
#define DMA_ATTR_SMOD(x)                         (((uint16_t)(((uint16_t)(x))<<DMA_ATTR_SMOD_SHIFT))&DMA_ATTR_SMOD_MASK)
/* NBYTES_MLNO Bit Fields */
#define DMA_NBYTES_MLNO_NBYTES_MASK              0xFFFFFFFFu
#define DMA_NBYTES_MLNO_NBYTES_SHIFT             0
#define DMA_NBYTES_MLNO_NBYTES(x)                (((uint32_t)(((uint32_t)(x))<<DMA_NBYTES_MLNO_NBYTES_SHIFT))&DMA_NBYTES_MLNO_NBYTES_MASK)
/* NBYTES_MLOFFNO Bit Fields */
#define DMA_NBYTES_MLOFFNO_NBYTES_MASK           0x3FFFFFFFu
#define DMA_NBYTES_MLOFFNO_NBYTES_SHIFT          0
#define DMA_NBYTES_MLOFFNO_NBYTES(x)             (((uint32_t)(((uint32_t)(x))<<DMA_NBYTES_MLOFFNO_NBYTES_SHIFT))&DMA_NBYTES_MLOFFNO_NBYTES_MASK)
#define DMA_NBYTES_MLOFFNO_DMLOE_MASK            0x40000000u
#define DMA_NBYTES_MLOFFNO_DMLOE_SHIFT           30
#define DMA_NBYTES_MLOFFNO_SMLOE_MASK            0x80000000u
#define DMA_NBYTES_MLOFFNO_SMLOE_SHIFT           31
/* NBYTES_MLOFFYES Bit Fields */
#define DMA_NBYTES_MLOFFYES_NBYTES_MASK          0x3FFu
#define DMA_NBYTES_MLOFFYES_NBYTES_SHIFT         0
#define DMA_NBYTES_MLOFFYES_NBYTES(x)            (((uint32_t)(((uint32_t)(x))<<DMA_NBYTES_MLOFFYES_NBYTES_SHIFT))&DMA_NBYTES_MLOFFYES_NBYTES_MASK)
#define DMA_NBYTES_MLOFFYES_MLOFF_MASK           0x3FFFFC00u
#define DMA_NBYTES_MLOFFYES_MLOFF_SHIFT          10
#define DMA_NBYTES_MLOFFYES_MLOFF(x)             (((uint32_t)(((uint32_t)(x))<<DMA_NBYTES_MLOFFYES_MLOFF_SHIFT))&DMA_NBYTES_MLOFFYES_MLOFF_MASK)
#define DMA_NBYTES_MLOFFYES_DMLOE_MASK           0x40000000u
#define DMA_NBYTES_MLOFFYES_DMLOE_SHIFT          30
#define DMA_NBYTES_MLOFFYES_SMLOE_MASK           0x80000000u
#define DMA_NBYTES_MLOFFYES_SMLOE_SHIFT          31
/* SLAST Bit Fields */
#define DMA_SLAST_SLAST_MASK                     0xFFFFFFFFu
#define DMA_SLAST_SLAST_SHIFT                    0
#define DMA_SLAST_SLAST(x)                       (((uint32_t)(((uint32_t)(x))<<DMA_SLAST_SLAST_SHIFT))&DMA_SLAST_SLAST_MASK)
/* DADDR Bit Fields */
#define DMA_DADDR_DADDR_MASK                     0xFFFFFFFFu
#define DMA_DADDR_DADDR_SHIFT                    0
#define DMA_DADDR_DADDR(x)                       (((uint32_t)(((uint32_t)(x))<<DMA_DADDR_DADDR_SHIFT))&DMA_DADDR_DADDR_MASK)
/* DOFF Bit Fields */
#define DMA_DOFF_DOFF_MASK                       0xFFFFu
#define DMA_DOFF_DOFF_SHIFT                      0
#define DMA_DOFF_DOFF(x)                         (((uint16_t)(((uint16_t)(x))<<DMA_DOFF_DOFF_SHIFT))&DMA_DOFF_DOFF_MASK)
/* CITER_ELINKNO Bit Fields */
#define DMA_CITER_ELINKNO_CITER_MASK             0x7FFFu
#define DMA_CITER_ELINKNO_CITER_SHIFT            0
#define DMA_CITER_ELINKNO_CITER(x)               (((uint16_t)(((uint16_t)(x))<<DMA_CITER_ELINKNO_CITER_SHIFT))&DMA_CITER_ELINKNO_CITER_MASK)
#define DMA_CITER_ELINKNO_ELINK_MASK             0x8000u
#define DMA_CITER_ELINKNO_ELINK_SHIFT            15
/* CITER_ELINKYES Bit Fields */
#define DMA_CITER_ELINKYES_CITER_MASK            0x1FFu
#define DMA_CITER_ELINKYES_CITER_SHIFT           0
#define DMA_CITER_ELINKYES_CITER(x)              (((uint16_t)(((uint16_t)(x))<<DMA_CITER_ELINKYES_CITER_SHIFT))&DMA_CITER_ELINKYES_CITER_MASK)
#define DMA_CITER_ELINKYES_LINKCH_MASK           0x1E00u
#define DMA_CITER_ELINKYES_LINKCH_SHIFT          9
#define DMA_CITER_ELINKYES_LINKCH(x)             (((uint16_t)(((uint16_t)(x))<<DMA_CITER_ELINKYES_LINKCH_SHIFT))&DMA_CITER_ELINKYES_LINKCH_MASK)
#define DMA_CITER_ELINKYES_ELINK_MASK            0x8000u
#define DMA_CITER_ELINKYES_ELINK_SHIFT           15
/* DLAST_SGA Bit Fields */
#define DMA_DLAST_SGA_DLASTSGA_MASK              0xFFFFFFFFu
#define DMA_DLAST_SGA_DLASTSGA_SHIFT             0
#define DMA_DLAST_SGA_DLASTSGA(x)                (((uint32_t)(((uint32_t)(x))<<DMA_DLAST_SGA_DLASTSGA_SHIFT))&DMA_DLAST_SGA_DLASTSGA_MASK)
/* CSR Bit Fields */
#define DMA_CSR_START_MASK                       0x1u
#define DMA_CSR_START_SHIFT                      0
#define DMA_CSR_INTMAJOR_MASK                    0x2u
#define DMA_CSR_INTMAJOR_SHIFT                   1
#define DMA_CSR_INTHALF_MASK                     0x4u
#define DMA_CSR_INTHALF_SHIFT                    2
#define DMA_CSR_DREQ_MASK                        0x8u
#define DMA_CSR_DREQ_SHIFT                       3
#define DMA_CSR_ESG_MASK                         0x10u
#define DMA_CSR_ESG_SHIFT                        4
#define DMA_CSR_MAJORELINK_MASK                  0x20u
#define DMA_CSR_MAJORELINK_SHIFT                 5
#define DMA_CSR_ACTIVE_MASK                      0x40u
#define DMA_CSR_ACTIVE_SHIFT                     6
#define DMA_CSR_DONE_MASK                        0x80u
#define DMA_CSR_DONE_SHIFT                       7
#define DMA_CSR_MAJORLINKCH_MASK                 0xF00u
#define DMA_CSR_MAJORLINKCH_SHIFT                8
#define DMA_CSR_MAJORLINKCH(x)                   (((uint16_t)(((uint16_t)(x))<<DMA_CSR_MAJORLINKCH_SHIFT))&DMA_CSR_MAJORLINKCH_MASK)
#define DMA_CSR_BWC_MASK                         0xC000u
#define DMA_CSR_BWC_SHIFT                        14
#define DMA_CSR_BWC(x)                           (((uint16_t)(((uint16_t)(x))<<DMA_CSR_BWC_SHIFT))&DMA_CSR_BWC_MASK)
/* BITER_ELINKNO Bit Fields */
#define DMA_BITER_ELINKNO_BITER_MASK             0x7FFFu
#define DMA_BITER_ELINKNO_BITER_SHIFT            0
#define DMA_BITER_ELINKNO_BITER(x)               (((uint16_t)(((uint16_t)(x))<<DMA_BITER_ELINKNO_BITER_SHIFT))&DMA_BITER_ELINKNO_BITER_MASK)
#define DMA_BITER_ELINKNO_ELINK_MASK             0x8000u
#define DMA_BITER_ELINKNO_ELINK_SHIFT            15
/* BITER_ELINKYES Bit Fields */
#define DMA_BITER_ELINKYES_BITER_MASK            0x1FFu
#define DMA_BITER_ELINKYES_BITER_SHIFT           0
#define DMA_BITER_ELINKYES_BITER(x)              (((uint16_t)(((uint16_t)(x))<<DMA_BITER_ELINKYES_BITER_SHIFT))&DMA_BITER_ELINKYES_BITER_MASK)
#define DMA_BITER_ELINKYES_LINKCH_MASK           0x1E00u
#define DMA_BITER_ELINKYES_LINKCH_SHIFT          9
#define DMA_BITER_ELINKYES_LINKCH(x)             (((uint16_t)(((uint16_t)(x))<<DMA_BITER_ELINKYES_LINKCH_SHIFT))&DMA_BITER_ELINKYES_LINKCH_MASK)
#define DMA_BITER_ELINKYES_ELINK_MASK            0x8000u
#define DMA_BITER_ELINKYES_ELINK_SHIFT           15

/**
 * @}
 */ /* end of group DMA_Register_Masks */


/* DMA - Peripheral instance base addresses */
/** Peripheral DMA base address */
#define DMA_BASE                                 (0x40008000u)
/** Peripheral DMA base pointer */
#define DMA0                                     ((DMA_Type *)DMA_BASE)
/** Array initializer of DMA peripheral base pointers */
#define DMA_BASES                                { DMA0 }

/**
 * @}
 */ /* end of group DMA_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- DMAMUX Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup DMAMUX_Peripheral_Access_Layer DMAMUX Peripheral Access Layer
 * @{
 */

/** DMAMUX - Register Layout Typedef */
typedef struct {
  __IO uint8_t CHCFG[16];                          /**< Channel Configuration register, array offset: 0x0, array step: 0x1 */
} DMAMUX_Type;

/* ----------------------------------------------------------------------------
   -- DMAMUX Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup DMAMUX_Register_Masks DMAMUX Register Masks
 * @{
 */

/* CHCFG Bit Fields */
#define DMAMUX_CHCFG_SOURCE_MASK                 0x3Fu
#define DMAMUX_CHCFG_SOURCE_SHIFT                0
#define DMAMUX_CHCFG_SOURCE(x)                   (((uint8_t)(((uint8_t)(x))<<DMAMUX_CHCFG_SOURCE_SHIFT))&DMAMUX_CHCFG_SOURCE_MASK)
#define DMAMUX_CHCFG_TRIG_MASK                   0x40u
#define DMAMUX_CHCFG_TRIG_SHIFT                  6
#define DMAMUX_CHCFG_ENBL_MASK                   0x80u
#define DMAMUX_CHCFG_ENBL_SHIFT                  7

/**
 * @}
 */ /* end of group DMAMUX_Register_Masks */


/* DMAMUX - Peripheral instance base addresses */
/** Peripheral DMAMUX base address */
#define DMAMUX_BASE                              (0x40021000u)
/** Peripheral DMAMUX base pointer */
#define DMAMUX                                   ((DMAMUX_Type *)DMAMUX_BASE)
/** Array initializer of DMAMUX peripheral base pointers */
#define DMAMUX_BASES                             { DMAMUX }

/**
 * @}
 */ /* end of group DMAMUX_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- ENET Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup ENET_Peripheral_Access_Layer ENET Peripheral Access Layer
 * @{
 */

/** ENET - Register Layout Typedef */
typedef struct {
       uint8_t RESERVED_0[4];
  __IO uint32_t EIR;                               /**< Interrupt Event Register, offset: 0x4 */
  __IO uint32_t EIMR;                              /**< Interrupt Mask Register, offset: 0x8 */
       uint8_t RESERVED_1[4];
  __IO uint32_t RDAR;                              /**< Receive Descriptor Active Register, offset: 0x10 */
  __IO uint32_t TDAR;                              /**< Transmit Descriptor Active Register, offset: 0x14 */
       uint8_t RESERVED_2[12];
  __IO uint32_t ECR;                               /**< Ethernet Control Register, offset: 0x24 */
       uint8_t RESERVED_3[24];
  __IO uint32_t MMFR;                              /**< MII Management Frame Register, offset: 0x40 */
  __IO uint32_t MSCR;                              /**< MII Speed Control Register, offset: 0x44 */
       uint8_t RESERVED_4[28];
  __IO uint32_t MIBC;                              /**< MIB Control Register, offset: 0x64 */
       uint8_t RESERVED_5[28];
  __IO uint32_t RCR;                               /**< Receive Control Register, offset: 0x84 */
       uint8_t RESERVED_6[60];
  __IO uint32_t TCR;                               /**< Transmit Control Register, offset: 0xC4 */
       uint8_t RESERVED_7[28];
  __IO uint32_t PALR;                              /**< Physical Address Lower Register, offset: 0xE4 */
  __IO uint32_t PAUR;                              /**< Physical Address Upper Register, offset: 0xE8 */
  __IO uint32_t OPD;                               /**< Opcode/Pause Duration Register, offset: 0xEC */
       uint8_t RESERVED_8[40];
  __IO uint32_t IAUR;                              /**< Descriptor Individual Upper Address Register, offset: 0x118 */
  __IO uint32_t IALR;                              /**< Descriptor Individual Lower Address Register, offset: 0x11C */
  __IO uint32_t GAUR;                              /**< Descriptor Group Upper Address Register, offset: 0x120 */
  __IO uint32_t GALR;                              /**< Descriptor Group Lower Address Register, offset: 0x124 */
       uint8_t RESERVED_9[28];
  __IO uint32_t TFWR;                              /**< Transmit FIFO Watermark Register, offset: 0x144 */
       uint8_t RESERVED_10[56];
  __IO uint32_t RDSR;                              /**< Receive Descriptor Ring Start Register, offset: 0x180 */
  __IO uint32_t TDSR;                              /**< Transmit Buffer Descriptor Ring Start Register, offset: 0x184 */
  __IO uint32_t MRBR;                              /**< Maximum Receive Buffer Size Register, offset: 0x188 */
       uint8_t RESERVED_11[4];
  __IO uint32_t RSFL;                              /**< Receive FIFO Section Full Threshold, offset: 0x190 */
  __IO uint32_t RSEM;                              /**< Receive FIFO Section Empty Threshold, offset: 0x194 */
  __IO uint32_t RAEM;                              /**< Receive FIFO Almost Empty Threshold, offset: 0x198 */
  __IO uint32_t RAFL;                              /**< Receive FIFO Almost Full Threshold, offset: 0x19C */
  __IO uint32_t TSEM;                              /**< Transmit FIFO Section Empty Threshold, offset: 0x1A0 */
  __IO uint32_t TAEM;                              /**< Transmit FIFO Almost Empty Threshold, offset: 0x1A4 */
  __IO uint32_t TAFL;                              /**< Transmit FIFO Almost Full Threshold, offset: 0x1A8 */
  __IO uint32_t TIPG;                              /**< Transmit Inter-Packet Gap, offset: 0x1AC */
  __IO uint32_t FTRL;                              /**< Frame Truncation Length, offset: 0x1B0 */
       uint8_t RESERVED_12[12];
  __IO uint32_t TACC;                              /**< Transmit Accelerator Function Configuration, offset: 0x1C0 */
  __IO uint32_t RACC;                              /**< Receive Accelerator Function Configuration, offset: 0x1C4 */
       uint8_t RESERVED_13[56];
  __IO uint32_t RMON_T_DROP;                       /**< Count of frames not counted correctly (RMON_T_DROP). NOTE: Counter not implemented (read 0 always) as not applicable., offset: 0x200 */
  __IO uint32_t RMON_T_PACKETS;                    /**< RMON Tx packet count (RMON_T_PACKETS), offset: 0x204 */
  __IO uint32_t RMON_T_BC_PKT;                     /**< RMON Tx Broadcast Packets (RMON_T_BC_PKT), offset: 0x208 */
  __IO uint32_t RMON_T_MC_PKT;                     /**< RMON Tx Multicast Packets (RMON_T_MC_PKT), offset: 0x20C */
  __IO uint32_t RMON_T_CRC_ALIGN;                  /**< RMON Tx Packets w CRC/Align error (RMON_T_CRC_ALIGN), offset: 0x210 */
  __IO uint32_t RMON_T_UNDERSIZE;                  /**< RMON Tx Packets < 64 bytes, good CRC (RMON_T_UNDERSIZE), offset: 0x214 */
  __IO uint32_t RMON_T_OVERSIZE;                   /**< RMON Tx Packets > MAX_FL bytes, good CRC (RMON_T_OVERSIZE), offset: 0x218 */
  __IO uint32_t RMON_T_FRAG;                       /**< RMON Tx Packets < 64 bytes, bad CRC (RMON_T_FRAG), offset: 0x21C */
  __IO uint32_t RMON_T_JAB;                        /**< RMON Tx Packets > MAX_FL bytes, bad CRC (RMON_T_JAB), offset: 0x220 */
  __IO uint32_t RMON_T_COL;                        /**< RMON Tx collision count (RMON_T_COL), offset: 0x224 */
  __IO uint32_t RMON_T_P64;                        /**< RMON Tx 64 byte packets (RMON_T_P64), offset: 0x228 */
  __IO uint32_t RMON_T_P65TO127;                   /**< RMON Tx 65 to 127 byte packets (RMON_T_P65TO127), offset: 0x22C */
  __IO uint32_t RMON_T_P128TO255;                  /**< RMON Tx 128 to 255 byte packets (RMON_T_P128TO255), offset: 0x230 */
  __IO uint32_t RMON_T_P256TO511;                  /**< RMON Tx 256 to 511 byte packets (RMON_T_P256TO511), offset: 0x234 */
  __IO uint32_t RMON_T_P512TO1023;                 /**< RMON Tx 512 to 1023 byte packets (RMON_T_P512TO1023), offset: 0x238 */
  __IO uint32_t RMON_T_P1024TO2047;                /**< RMON Tx 1024 to 2047 byte packets (RMON_T_P1024TO2047), offset: 0x23C */
  __IO uint32_t RMON_T_P_GTE2048;                  /**< RMON Tx packets w > 2048 bytes (RMON_T_P_GTE2048), offset: 0x240 */
  __IO uint32_t RMON_T_OCTETS;                     /**< RMON Tx Octets (RMON_T_OCTETS), offset: 0x244 */
  __IO uint32_t IEEE_T_DROP;                       /**< Count of frames not counted correctly (IEEE_T_DROP). NOTE: Counter not implemented (read 0 always) as not applicable., offset: 0x248 */
  __IO uint32_t IEEE_T_FRAME_OK;                   /**< Frames Transmitted OK (IEEE_T_FRAME_OK), offset: 0x24C */
  __IO uint32_t IEEE_T_1COL;                       /**< Frames Transmitted with Single Collision (IEEE_T_1COL), offset: 0x250 */
  __IO uint32_t IEEE_T_MCOL;                       /**< Frames Transmitted with Multiple Collisions (IEEE_T_MCOL), offset: 0x254 */
  __IO uint32_t IEEE_T_DEF;                        /**< Frames Transmitted after Deferral Delay (IEEE_T_DEF), offset: 0x258 */
  __IO uint32_t IEEE_T_LCOL;                       /**< Frames Transmitted with Late Collision (IEEE_T_LCOL), offset: 0x25C */
  __IO uint32_t IEEE_T_EXCOL;                      /**< Frames Transmitted with Excessive Collisions (IEEE_T_EXCOL), offset: 0x260 */
  __IO uint32_t IEEE_T_MACERR;                     /**< Frames Transmitted with Tx FIFO Underrun (IEEE_T_MACERR), offset: 0x264 */
  __IO uint32_t IEEE_T_CSERR;                      /**< Frames Transmitted with Carrier Sense Error (IEEE_T_CSERR), offset: 0x268 */
  __IO uint32_t IEEE_T_SQE;                        /**< Frames Transmitted with SQE Error (IEEE_T_SQE). NOTE: Counter not implemented (read 0 always) as no SQE information is available., offset: 0x26C */
  __IO uint32_t IEEE_T_FDXFC;                      /**< Flow Control Pause frames transmitted (IEEE_T_FDXFC), offset: 0x270 */
  __IO uint32_t IEEE_T_OCTETS_OK;                  /**< Octet count for Frames Transmitted w/o Error (IEEE_T_OCTETS_OK). NOTE: Counts total octets (includes header and FCS fields)., offset: 0x274 */
       uint8_t RESERVED_14[12];
  __IO uint32_t RMON_R_PACKETS;                    /**< RMON Rx packet count (RMON_R_PACKETS), offset: 0x284 */
  __IO uint32_t RMON_R_BC_PKT;                     /**< RMON Rx Broadcast Packets (RMON_R_BC_PKT), offset: 0x288 */
  __IO uint32_t RMON_R_MC_PKT;                     /**< RMON Rx Multicast Packets (RMON_R_MC_PKT), offset: 0x28C */
  __IO uint32_t RMON_R_CRC_ALIGN;                  /**< RMON Rx Packets w CRC/Align error (RMON_R_CRC_ALIGN), offset: 0x290 */
  __IO uint32_t RMON_R_UNDERSIZE;                  /**< RMON Rx Packets < 64 bytes, good CRC (RMON_R_UNDERSIZE), offset: 0x294 */
  __IO uint32_t RMON_R_OVERSIZE;                   /**< RMON Rx Packets > MAX_FL bytes, good CRC (RMON_R_OVERSIZE), offset: 0x298 */
  __IO uint32_t RMON_R_FRAG;                       /**< RMON Rx Packets < 64 bytes, bad CRC (RMON_R_FRAG), offset: 0x29C */
  __IO uint32_t RMON_R_JAB;                        /**< RMON Rx Packets > MAX_FL bytes, bad CRC (RMON_R_JAB), offset: 0x2A0 */
  __IO uint32_t RMON_R_RESVD_0;                    /**< Reserved (RMON_R_RESVD_0), offset: 0x2A4 */
  __IO uint32_t RMON_R_P64;                        /**< RMON Rx 64 byte packets (RMON_R_P64), offset: 0x2A8 */
  __IO uint32_t RMON_R_P65TO127;                   /**< RMON Rx 65 to 127 byte packets (RMON_R_P65TO127), offset: 0x2AC */
  __IO uint32_t RMON_R_P128TO255;                  /**< RMON Rx 128 to 255 byte packets (RMON_R_P128TO255), offset: 0x2B0 */
  __IO uint32_t RMON_R_P256TO511;                  /**< RMON Rx 256 to 511 byte packets (RMON_R_P256TO511), offset: 0x2B4 */
  __IO uint32_t RMON_R_P512TO1023;                 /**< RMON Rx 512 to 1023 byte packets (RMON_R_P512TO1023), offset: 0x2B8 */
  __IO uint32_t RMON_R_P1024TO2047;                /**< RMON Rx 1024 to 2047 byte packets (RMON_R_P1024TO2047), offset: 0x2BC */
  __IO uint32_t RMON_R_P_GTE2048;                  /**< RMON Rx packets w > 2048 bytes (RMON_R_P_GTE2048), offset: 0x2C0 */
  __IO uint32_t RMON_R_OCTETS;                     /**< RMON Rx Octets (RMON_R_OCTETS), offset: 0x2C4 */
  __IO uint32_t RMON_R_DROP;                       /**< Count of frames not counted correctly (IEEE_R_DROP). NOTE: Counter increments if a frame with valid/missing SFD character is detected and has been dropped. None of the other counters increments if this counter increments., offset: 0x2C8 */
  __IO uint32_t RMON_R_FRAME_OK;                   /**< Frames Received OK (IEEE_R_FRAME_OK), offset: 0x2CC */
  __IO uint32_t IEEE_R_CRC;                        /**< Frames Received with CRC Error (IEEE_R_CRC), offset: 0x2D0 */
  __IO uint32_t IEEE_R_ALIGN;                      /**< Frames Received with Alignment Error (IEEE_R_ALIGN), offset: 0x2D4 */
  __IO uint32_t IEEE_R_MACERR;                     /**< Receive Fifo Overflow count (IEEE_R_MACERR), offset: 0x2D8 */
  __IO uint32_t IEEE_R_FDXFC;                      /**< Flow Control Pause frames received (IEEE_R_FDXFC), offset: 0x2DC */
  __IO uint32_t IEEE_R_OCTETS_OK;                  /**< Octet count for Frames Rcvd w/o Error (IEEE_R_OCTETS_OK). Counts total octets (includes header and FCS fields)., offset: 0x2E0 */
       uint8_t RESERVED_15[284];
  __IO uint32_t ATCR;                              /**< Timer Control Register, offset: 0x400 */
  __IO uint32_t ATVR;                              /**< Timer Value Register, offset: 0x404 */
  __IO uint32_t ATOFF;                             /**< Timer Offset Register, offset: 0x408 */
  __IO uint32_t ATPER;                             /**< Timer Period Register, offset: 0x40C */
  __IO uint32_t ATCOR;                             /**< Timer Correction Register, offset: 0x410 */
  __IO uint32_t ATINC;                             /**< Time-Stamping Clock Period Register, offset: 0x414 */
  __IO uint32_t ATSTMP;                            /**< Timestamp of Last Transmitted Frame, offset: 0x418 */
       uint8_t RESERVED_16[488];
  __IO uint32_t TGSR;                              /**< Timer Global Status Register, offset: 0x604 */
  struct {                                         /* offset: 0x608, array step: 0x8 */
    __IO uint32_t TCSR;                              /**< Timer Control Status Register, array offset: 0x608, array step: 0x8 */
    __IO uint32_t TCCR;                              /**< Timer Compare Capture Register, array offset: 0x60C, array step: 0x8 */
  } CHANNEL[4];
} ENET_Type;

/* ----------------------------------------------------------------------------
   -- ENET Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup ENET_Register_Masks ENET Register Masks
 * @{
 */

/* EIR Bit Fields */
#define ENET_EIR_TS_TIMER_MASK                   0x8000u
#define ENET_EIR_TS_TIMER_SHIFT                  15
#define ENET_EIR_TS_AVAIL_MASK                   0x10000u
#define ENET_EIR_TS_AVAIL_SHIFT                  16
#define ENET_EIR_WAKEUP_MASK                     0x20000u
#define ENET_EIR_WAKEUP_SHIFT                    17
#define ENET_EIR_PLR_MASK                        0x40000u
#define ENET_EIR_PLR_SHIFT                       18
#define ENET_EIR_UN_MASK                         0x80000u
#define ENET_EIR_UN_SHIFT                        19
#define ENET_EIR_RL_MASK                         0x100000u
#define ENET_EIR_RL_SHIFT                        20
#define ENET_EIR_LC_MASK                         0x200000u
#define ENET_EIR_LC_SHIFT                        21
#define ENET_EIR_EBERR_MASK                      0x400000u
#define ENET_EIR_EBERR_SHIFT                     22
#define ENET_EIR_MII_MASK                        0x800000u
#define ENET_EIR_MII_SHIFT                       23
#define ENET_EIR_RXB_MASK                        0x1000000u
#define ENET_EIR_RXB_SHIFT                       24
#define ENET_EIR_RXF_MASK                        0x2000000u
#define ENET_EIR_RXF_SHIFT                       25
#define ENET_EIR_TXB_MASK                        0x4000000u
#define ENET_EIR_TXB_SHIFT                       26
#define ENET_EIR_TXF_MASK                        0x8000000u
#define ENET_EIR_TXF_SHIFT                       27
#define ENET_EIR_GRA_MASK                        0x10000000u
#define ENET_EIR_GRA_SHIFT                       28
#define ENET_EIR_BABT_MASK                       0x20000000u
#define ENET_EIR_BABT_SHIFT                      29
#define ENET_EIR_BABR_MASK                       0x40000000u
#define ENET_EIR_BABR_SHIFT                      30
/* EIMR Bit Fields */
#define ENET_EIMR_TS_TIMER_MASK                  0x8000u
#define ENET_EIMR_TS_TIMER_SHIFT                 15
#define ENET_EIMR_TS_AVAIL_MASK                  0x10000u
#define ENET_EIMR_TS_AVAIL_SHIFT                 16
#define ENET_EIMR_WAKEUP_MASK                    0x20000u
#define ENET_EIMR_WAKEUP_SHIFT                   17
#define ENET_EIMR_PLR_MASK                       0x40000u
#define ENET_EIMR_PLR_SHIFT                      18
#define ENET_EIMR_UN_MASK                        0x80000u
#define ENET_EIMR_UN_SHIFT                       19
#define ENET_EIMR_RL_MASK                        0x100000u
#define ENET_EIMR_RL_SHIFT                       20
#define ENET_EIMR_LC_MASK                        0x200000u
#define ENET_EIMR_LC_SHIFT                       21
#define ENET_EIMR_EBERR_MASK                     0x400000u
#define ENET_EIMR_EBERR_SHIFT                    22
#define ENET_EIMR_MII_MASK                       0x800000u
#define ENET_EIMR_MII_SHIFT                      23
#define ENET_EIMR_RXB_MASK                       0x1000000u
#define ENET_EIMR_RXB_SHIFT                      24
#define ENET_EIMR_RXF_MASK                       0x2000000u
#define ENET_EIMR_RXF_SHIFT                      25
#define ENET_EIMR_TXB_MASK                       0x4000000u
#define ENET_EIMR_TXB_SHIFT                      26
#define ENET_EIMR_TXF_MASK                       0x8000000u
#define ENET_EIMR_TXF_SHIFT                      27
#define ENET_EIMR_GRA_MASK                       0x10000000u
#define ENET_EIMR_GRA_SHIFT                      28
#define ENET_EIMR_BABT_MASK                      0x20000000u
#define ENET_EIMR_BABT_SHIFT                     29
#define ENET_EIMR_BABR_MASK                      0x40000000u
#define ENET_EIMR_BABR_SHIFT                     30
/* RDAR Bit Fields */
#define ENET_RDAR_RDAR_MASK                      0x1000000u
#define ENET_RDAR_RDAR_SHIFT                     24
/* TDAR Bit Fields */
#define ENET_TDAR_TDAR_MASK                      0x1000000u
#define ENET_TDAR_TDAR_SHIFT                     24
/* ECR Bit Fields */
#define ENET_ECR_RESET_MASK                      0x1u
#define ENET_ECR_RESET_SHIFT                     0
#define ENET_ECR_ETHEREN_MASK                    0x2u
#define ENET_ECR_ETHEREN_SHIFT                   1
#define ENET_ECR_MAGICEN_MASK                    0x4u
#define ENET_ECR_MAGICEN_SHIFT                   2
#define ENET_ECR_SLEEP_MASK                      0x8u
#define ENET_ECR_SLEEP_SHIFT                     3
#define ENET_ECR_EN1588_MASK                     0x10u
#define ENET_ECR_EN1588_SHIFT                    4
#define ENET_ECR_DBGEN_MASK                      0x40u
#define ENET_ECR_DBGEN_SHIFT                     6
#define ENET_ECR_STOPEN_MASK                     0x80u
#define ENET_ECR_STOPEN_SHIFT                    7
#define ENET_ECR_DBSWP_MASK                      0x100u
#define ENET_ECR_DBSWP_SHIFT                     8
/* MMFR Bit Fields */
#define ENET_MMFR_DATA_MASK                      0xFFFFu
#define ENET_MMFR_DATA_SHIFT                     0
#define ENET_MMFR_DATA(x)                        (((uint32_t)(((uint32_t)(x))<<ENET_MMFR_DATA_SHIFT))&ENET_MMFR_DATA_MASK)
#define ENET_MMFR_TA_MASK                        0x30000u
#define ENET_MMFR_TA_SHIFT                       16
#define ENET_MMFR_TA(x)                          (((uint32_t)(((uint32_t)(x))<<ENET_MMFR_TA_SHIFT))&ENET_MMFR_TA_MASK)
#define ENET_MMFR_RA_MASK                        0x7C0000u
#define ENET_MMFR_RA_SHIFT                       18
#define ENET_MMFR_RA(x)                          (((uint32_t)(((uint32_t)(x))<<ENET_MMFR_RA_SHIFT))&ENET_MMFR_RA_MASK)
#define ENET_MMFR_PA_MASK                        0xF800000u
#define ENET_MMFR_PA_SHIFT                       23
#define ENET_MMFR_PA(x)                          (((uint32_t)(((uint32_t)(x))<<ENET_MMFR_PA_SHIFT))&ENET_MMFR_PA_MASK)
#define ENET_MMFR_OP_MASK                        0x30000000u
#define ENET_MMFR_OP_SHIFT                       28
#define ENET_MMFR_OP(x)                          (((uint32_t)(((uint32_t)(x))<<ENET_MMFR_OP_SHIFT))&ENET_MMFR_OP_MASK)
#define ENET_MMFR_ST_MASK                        0xC0000000u
#define ENET_MMFR_ST_SHIFT                       30
#define ENET_MMFR_ST(x)                          (((uint32_t)(((uint32_t)(x))<<ENET_MMFR_ST_SHIFT))&ENET_MMFR_ST_MASK)
/* MSCR Bit Fields */
#define ENET_MSCR_MII_SPEED_MASK                 0x7Eu
#define ENET_MSCR_MII_SPEED_SHIFT                1
#define ENET_MSCR_MII_SPEED(x)                   (((uint32_t)(((uint32_t)(x))<<ENET_MSCR_MII_SPEED_SHIFT))&ENET_MSCR_MII_SPEED_MASK)
#define ENET_MSCR_DIS_PRE_MASK                   0x80u
#define ENET_MSCR_DIS_PRE_SHIFT                  7
#define ENET_MSCR_HOLDTIME_MASK                  0x700u
#define ENET_MSCR_HOLDTIME_SHIFT                 8
#define ENET_MSCR_HOLDTIME(x)                    (((uint32_t)(((uint32_t)(x))<<ENET_MSCR_HOLDTIME_SHIFT))&ENET_MSCR_HOLDTIME_MASK)
/* MIBC Bit Fields */
#define ENET_MIBC_MIB_CLEAR_MASK                 0x20000000u
#define ENET_MIBC_MIB_CLEAR_SHIFT                29
#define ENET_MIBC_MIB_IDLE_MASK                  0x40000000u
#define ENET_MIBC_MIB_IDLE_SHIFT                 30
#define ENET_MIBC_MIB_DIS_MASK                   0x80000000u
#define ENET_MIBC_MIB_DIS_SHIFT                  31
/* RCR Bit Fields */
#define ENET_RCR_LOOP_MASK                       0x1u
#define ENET_RCR_LOOP_SHIFT                      0
#define ENET_RCR_DRT_MASK                        0x2u
#define ENET_RCR_DRT_SHIFT                       1
#define ENET_RCR_MII_MODE_MASK                   0x4u
#define ENET_RCR_MII_MODE_SHIFT                  2
#define ENET_RCR_PROM_MASK                       0x8u
#define ENET_RCR_PROM_SHIFT                      3
#define ENET_RCR_BC_REJ_MASK                     0x10u
#define ENET_RCR_BC_REJ_SHIFT                    4
#define ENET_RCR_FCE_MASK                        0x20u
#define ENET_RCR_FCE_SHIFT                       5
#define ENET_RCR_RMII_MODE_MASK                  0x100u
#define ENET_RCR_RMII_MODE_SHIFT                 8
#define ENET_RCR_RMII_10T_MASK                   0x200u
#define ENET_RCR_RMII_10T_SHIFT                  9
#define ENET_RCR_PADEN_MASK                      0x1000u
#define ENET_RCR_PADEN_SHIFT                     12
#define ENET_RCR_PAUFWD_MASK                     0x2000u
#define ENET_RCR_PAUFWD_SHIFT                    13
#define ENET_RCR_CRCFWD_MASK                     0x4000u
#define ENET_RCR_CRCFWD_SHIFT                    14
#define ENET_RCR_CFEN_MASK                       0x8000u
#define ENET_RCR_CFEN_SHIFT                      15
#define ENET_RCR_MAX_FL_MASK                     0x3FFF0000u
#define ENET_RCR_MAX_FL_SHIFT                    16
#define ENET_RCR_MAX_FL(x)                       (((uint32_t)(((uint32_t)(x))<<ENET_RCR_MAX_FL_SHIFT))&ENET_RCR_MAX_FL_MASK)
#define ENET_RCR_NLC_MASK                        0x40000000u
#define ENET_RCR_NLC_SHIFT                       30
#define ENET_RCR_GRS_MASK                        0x80000000u
#define ENET_RCR_GRS_SHIFT                       31
/* TCR Bit Fields */
#define ENET_TCR_GTS_MASK                        0x1u
#define ENET_TCR_GTS_SHIFT                       0
#define ENET_TCR_FDEN_MASK                       0x4u
#define ENET_TCR_FDEN_SHIFT                      2
#define ENET_TCR_TFC_PAUSE_MASK                  0x8u
#define ENET_TCR_TFC_PAUSE_SHIFT                 3
#define ENET_TCR_RFC_PAUSE_MASK                  0x10u
#define ENET_TCR_RFC_PAUSE_SHIFT                 4
#define ENET_TCR_ADDSEL_MASK                     0xE0u
#define ENET_TCR_ADDSEL_SHIFT                    5
#define ENET_TCR_ADDSEL(x)                       (((uint32_t)(((uint32_t)(x))<<ENET_TCR_ADDSEL_SHIFT))&ENET_TCR_ADDSEL_MASK)
#define ENET_TCR_ADDINS_MASK                     0x100u
#define ENET_TCR_ADDINS_SHIFT                    8
#define ENET_TCR_CRCFWD_MASK                     0x200u
#define ENET_TCR_CRCFWD_SHIFT                    9
/* PALR Bit Fields */
#define ENET_PALR_PADDR1_MASK                    0xFFFFFFFFu
#define ENET_PALR_PADDR1_SHIFT                   0
#define ENET_PALR_PADDR1(x)                      (((uint32_t)(((uint32_t)(x))<<ENET_PALR_PADDR1_SHIFT))&ENET_PALR_PADDR1_MASK)
/* PAUR Bit Fields */
#define ENET_PAUR_TYPE_MASK                      0xFFFFu
#define ENET_PAUR_TYPE_SHIFT                     0
#define ENET_PAUR_TYPE(x)                        (((uint32_t)(((uint32_t)(x))<<ENET_PAUR_TYPE_SHIFT))&ENET_PAUR_TYPE_MASK)
#define ENET_PAUR_PADDR2_MASK                    0xFFFF0000u
#define ENET_PAUR_PADDR2_SHIFT                   16
#define ENET_PAUR_PADDR2(x)                      (((uint32_t)(((uint32_t)(x))<<ENET_PAUR_PADDR2_SHIFT))&ENET_PAUR_PADDR2_MASK)
/* OPD Bit Fields */
#define ENET_OPD_PAUSE_DUR_MASK                  0xFFFFu
#define ENET_OPD_PAUSE_DUR_SHIFT                 0
#define ENET_OPD_PAUSE_DUR(x)                    (((uint32_t)(((uint32_t)(x))<<ENET_OPD_PAUSE_DUR_SHIFT))&ENET_OPD_PAUSE_DUR_MASK)
#define ENET_OPD_OPCODE_MASK                     0xFFFF0000u
#define ENET_OPD_OPCODE_SHIFT                    16
#define ENET_OPD_OPCODE(x)                       (((uint32_t)(((uint32_t)(x))<<ENET_OPD_OPCODE_SHIFT))&ENET_OPD_OPCODE_MASK)
/* IAUR Bit Fields */
#define ENET_IAUR_IADDR1_MASK                    0xFFFFFFFFu
#define ENET_IAUR_IADDR1_SHIFT                   0
#define ENET_IAUR_IADDR1(x)                      (((uint32_t)(((uint32_t)(x))<<ENET_IAUR_IADDR1_SHIFT))&ENET_IAUR_IADDR1_MASK)
/* IALR Bit Fields */
#define ENET_IALR_IADDR2_MASK                    0xFFFFFFFFu
#define ENET_IALR_IADDR2_SHIFT                   0
#define ENET_IALR_IADDR2(x)                      (((uint32_t)(((uint32_t)(x))<<ENET_IALR_IADDR2_SHIFT))&ENET_IALR_IADDR2_MASK)
/* GAUR Bit Fields */
#define ENET_GAUR_GADDR1_MASK                    0xFFFFFFFFu
#define ENET_GAUR_GADDR1_SHIFT                   0
#define ENET_GAUR_GADDR1(x)                      (((uint32_t)(((uint32_t)(x))<<ENET_GAUR_GADDR1_SHIFT))&ENET_GAUR_GADDR1_MASK)
/* GALR Bit Fields */
#define ENET_GALR_GADDR2_MASK                    0xFFFFFFFFu
#define ENET_GALR_GADDR2_SHIFT                   0
#define ENET_GALR_GADDR2(x)                      (((uint32_t)(((uint32_t)(x))<<ENET_GALR_GADDR2_SHIFT))&ENET_GALR_GADDR2_MASK)
/* TFWR Bit Fields */
#define ENET_TFWR_TFWR_MASK                      0x3Fu
#define ENET_TFWR_TFWR_SHIFT                     0
#define ENET_TFWR_TFWR(x)                        (((uint32_t)(((uint32_t)(x))<<ENET_TFWR_TFWR_SHIFT))&ENET_TFWR_TFWR_MASK)
#define ENET_TFWR_STRFWD_MASK                    0x100u
#define ENET_TFWR_STRFWD_SHIFT                   8
/* RDSR Bit Fields */
#define ENET_RDSR_R_DES_START_MASK               0xFFFFFFF8u
#define ENET_RDSR_R_DES_START_SHIFT              3
#define ENET_RDSR_R_DES_START(x)                 (((uint32_t)(((uint32_t)(x))<<ENET_RDSR_R_DES_START_SHIFT))&ENET_RDSR_R_DES_START_MASK)
/* TDSR Bit Fields */
#define ENET_TDSR_X_DES_START_MASK               0xFFFFFFF8u
#define ENET_TDSR_X_DES_START_SHIFT              3
#define ENET_TDSR_X_DES_START(x)                 (((uint32_t)(((uint32_t)(x))<<ENET_TDSR_X_DES_START_SHIFT))&ENET_TDSR_X_DES_START_MASK)
/* MRBR Bit Fields */
#define ENET_MRBR_R_BUF_SIZE_MASK                0x3FF0u
#define ENET_MRBR_R_BUF_SIZE_SHIFT               4
#define ENET_MRBR_R_BUF_SIZE(x)                  (((uint32_t)(((uint32_t)(x))<<ENET_MRBR_R_BUF_SIZE_SHIFT))&ENET_MRBR_R_BUF_SIZE_MASK)
/* RSFL Bit Fields */
#define ENET_RSFL_RX_SECTION_FULL_MASK           0xFFu
#define ENET_RSFL_RX_SECTION_FULL_SHIFT          0
#define ENET_RSFL_RX_SECTION_FULL(x)             (((uint32_t)(((uint32_t)(x))<<ENET_RSFL_RX_SECTION_FULL_SHIFT))&ENET_RSFL_RX_SECTION_FULL_MASK)
/* RSEM Bit Fields */
#define ENET_RSEM_RX_SECTION_EMPTY_MASK          0xFFu
#define ENET_RSEM_RX_SECTION_EMPTY_SHIFT         0
#define ENET_RSEM_RX_SECTION_EMPTY(x)            (((uint32_t)(((uint32_t)(x))<<ENET_RSEM_RX_SECTION_EMPTY_SHIFT))&ENET_RSEM_RX_SECTION_EMPTY_MASK)
/* RAEM Bit Fields */
#define ENET_RAEM_RX_ALMOST_EMPTY_MASK           0xFFu
#define ENET_RAEM_RX_ALMOST_EMPTY_SHIFT          0
#define ENET_RAEM_RX_ALMOST_EMPTY(x)             (((uint32_t)(((uint32_t)(x))<<ENET_RAEM_RX_ALMOST_EMPTY_SHIFT))&ENET_RAEM_RX_ALMOST_EMPTY_MASK)
/* RAFL Bit Fields */
#define ENET_RAFL_RX_ALMOST_FULL_MASK            0xFFu
#define ENET_RAFL_RX_ALMOST_FULL_SHIFT           0
#define ENET_RAFL_RX_ALMOST_FULL(x)              (((uint32_t)(((uint32_t)(x))<<ENET_RAFL_RX_ALMOST_FULL_SHIFT))&ENET_RAFL_RX_ALMOST_FULL_MASK)
/* TSEM Bit Fields */
#define ENET_TSEM_TX_SECTION_EMPTY_MASK          0xFFu
#define ENET_TSEM_TX_SECTION_EMPTY_SHIFT         0
#define ENET_TSEM_TX_SECTION_EMPTY(x)            (((uint32_t)(((uint32_t)(x))<<ENET_TSEM_TX_SECTION_EMPTY_SHIFT))&ENET_TSEM_TX_SECTION_EMPTY_MASK)
/* TAEM Bit Fields */
#define ENET_TAEM_TX_ALMOST_EMPTY_MASK           0xFFu
#define ENET_TAEM_TX_ALMOST_EMPTY_SHIFT          0
#define ENET_TAEM_TX_ALMOST_EMPTY(x)             (((uint32_t)(((uint32_t)(x))<<ENET_TAEM_TX_ALMOST_EMPTY_SHIFT))&ENET_TAEM_TX_ALMOST_EMPTY_MASK)
/* TAFL Bit Fields */
#define ENET_TAFL_TX_ALMOST_FULL_MASK            0xFFu
#define ENET_TAFL_TX_ALMOST_FULL_SHIFT           0
#define ENET_TAFL_TX_ALMOST_FULL(x)              (((uint32_t)(((uint32_t)(x))<<ENET_TAFL_TX_ALMOST_FULL_SHIFT))&ENET_TAFL_TX_ALMOST_FULL_MASK)
/* TIPG Bit Fields */
#define ENET_TIPG_IPG_MASK                       0x1Fu
#define ENET_TIPG_IPG_SHIFT                      0
#define ENET_TIPG_IPG(x)                         (((uint32_t)(((uint32_t)(x))<<ENET_TIPG_IPG_SHIFT))&ENET_TIPG_IPG_MASK)
/* FTRL Bit Fields */
#define ENET_FTRL_TRUNC_FL_MASK                  0x3FFFu
#define ENET_FTRL_TRUNC_FL_SHIFT                 0
#define ENET_FTRL_TRUNC_FL(x)                    (((uint32_t)(((uint32_t)(x))<<ENET_FTRL_TRUNC_FL_SHIFT))&ENET_FTRL_TRUNC_FL_MASK)
/* TACC Bit Fields */
#define ENET_TACC_SHIFT16_MASK                   0x1u
#define ENET_TACC_SHIFT16_SHIFT                  0
#define ENET_TACC_IPCHK_MASK                     0x8u
#define ENET_TACC_IPCHK_SHIFT                    3
#define ENET_TACC_PROCHK_MASK                    0x10u
#define ENET_TACC_PROCHK_SHIFT                   4
/* RACC Bit Fields */
#define ENET_RACC_PADREM_MASK                    0x1u
#define ENET_RACC_PADREM_SHIFT                   0
#define ENET_RACC_IPDIS_MASK                     0x2u
#define ENET_RACC_IPDIS_SHIFT                    1
#define ENET_RACC_PRODIS_MASK                    0x4u
#define ENET_RACC_PRODIS_SHIFT                   2
#define ENET_RACC_LINEDIS_MASK                   0x40u
#define ENET_RACC_LINEDIS_SHIFT                  6
#define ENET_RACC_SHIFT16_MASK                   0x80u
#define ENET_RACC_SHIFT16_SHIFT                  7
/* ATCR Bit Fields */
#define ENET_ATCR_EN_MASK                        0x1u
#define ENET_ATCR_EN_SHIFT                       0
#define ENET_ATCR_OFFEN_MASK                     0x4u
#define ENET_ATCR_OFFEN_SHIFT                    2
#define ENET_ATCR_OFFRST_MASK                    0x8u
#define ENET_ATCR_OFFRST_SHIFT                   3
#define ENET_ATCR_PEREN_MASK                     0x10u
#define ENET_ATCR_PEREN_SHIFT                    4
#define ENET_ATCR_PINPER_MASK                    0x80u
#define ENET_ATCR_PINPER_SHIFT                   7
#define ENET_ATCR_RESTART_MASK                   0x200u
#define ENET_ATCR_RESTART_SHIFT                  9
#define ENET_ATCR_CAPTURE_MASK                   0x800u
#define ENET_ATCR_CAPTURE_SHIFT                  11
#define ENET_ATCR_SLAVE_MASK                     0x2000u
#define ENET_ATCR_SLAVE_SHIFT                    13
/* ATVR Bit Fields */
#define ENET_ATVR_ATIME_MASK                     0xFFFFFFFFu
#define ENET_ATVR_ATIME_SHIFT                    0
#define ENET_ATVR_ATIME(x)                       (((uint32_t)(((uint32_t)(x))<<ENET_ATVR_ATIME_SHIFT))&ENET_ATVR_ATIME_MASK)
/* ATOFF Bit Fields */
#define ENET_ATOFF_OFFSET_MASK                   0xFFFFFFFFu
#define ENET_ATOFF_OFFSET_SHIFT                  0
#define ENET_ATOFF_OFFSET(x)                     (((uint32_t)(((uint32_t)(x))<<ENET_ATOFF_OFFSET_SHIFT))&ENET_ATOFF_OFFSET_MASK)
/* ATPER Bit Fields */
#define ENET_ATPER_PERIOD_MASK                   0xFFFFFFFFu
#define ENET_ATPER_PERIOD_SHIFT                  0
#define ENET_ATPER_PERIOD(x)                     (((uint32_t)(((uint32_t)(x))<<ENET_ATPER_PERIOD_SHIFT))&ENET_ATPER_PERIOD_MASK)
/* ATCOR Bit Fields */
#define ENET_ATCOR_COR_MASK                      0x7FFFFFFFu
#define ENET_ATCOR_COR_SHIFT                     0
#define ENET_ATCOR_COR(x)                        (((uint32_t)(((uint32_t)(x))<<ENET_ATCOR_COR_SHIFT))&ENET_ATCOR_COR_MASK)
/* ATINC Bit Fields */
#define ENET_ATINC_INC_MASK                      0x7Fu
#define ENET_ATINC_INC_SHIFT                     0
#define ENET_ATINC_INC(x)                        (((uint32_t)(((uint32_t)(x))<<ENET_ATINC_INC_SHIFT))&ENET_ATINC_INC_MASK)
#define ENET_ATINC_INC_CORR_MASK                 0x7F00u
#define ENET_ATINC_INC_CORR_SHIFT                8
#define ENET_ATINC_INC_CORR(x)                   (((uint32_t)(((uint32_t)(x))<<ENET_ATINC_INC_CORR_SHIFT))&ENET_ATINC_INC_CORR_MASK)
/* ATSTMP Bit Fields */
#define ENET_ATSTMP_TIMESTAMP_MASK               0xFFFFFFFFu
#define ENET_ATSTMP_TIMESTAMP_SHIFT              0
#define ENET_ATSTMP_TIMESTAMP(x)                 (((uint32_t)(((uint32_t)(x))<<ENET_ATSTMP_TIMESTAMP_SHIFT))&ENET_ATSTMP_TIMESTAMP_MASK)
/* TGSR Bit Fields */
#define ENET_TGSR_TF0_MASK                       0x1u
#define ENET_TGSR_TF0_SHIFT                      0
#define ENET_TGSR_TF1_MASK                       0x2u
#define ENET_TGSR_TF1_SHIFT                      1
#define ENET_TGSR_TF2_MASK                       0x4u
#define ENET_TGSR_TF2_SHIFT                      2
#define ENET_TGSR_TF3_MASK                       0x8u
#define ENET_TGSR_TF3_SHIFT                      3
/* TCSR Bit Fields */
#define ENET_TCSR_TDRE_MASK                      0x1u
#define ENET_TCSR_TDRE_SHIFT                     0
#define ENET_TCSR_TMODE_MASK                     0x3Cu
#define ENET_TCSR_TMODE_SHIFT                    2
#define ENET_TCSR_TMODE(x)                       (((uint32_t)(((uint32_t)(x))<<ENET_TCSR_TMODE_SHIFT))&ENET_TCSR_TMODE_MASK)
#define ENET_TCSR_TIE_MASK                       0x40u
#define ENET_TCSR_TIE_SHIFT                      6
#define ENET_TCSR_TF_MASK                        0x80u
#define ENET_TCSR_TF_SHIFT                       7
/* TCCR Bit Fields */
#define ENET_TCCR_TCC_MASK                       0xFFFFFFFFu
#define ENET_TCCR_TCC_SHIFT                      0
#define ENET_TCCR_TCC(x)                         (((uint32_t)(((uint32_t)(x))<<ENET_TCCR_TCC_SHIFT))&ENET_TCCR_TCC_MASK)

/**
 * @}
 */ /* end of group ENET_Register_Masks */


/* ENET - Peripheral instance base addresses */
/** Peripheral ENET base address */
#define ENET_BASE                                (0x400C0000u)
/** Peripheral ENET base pointer */
#define ENET                                     ((ENET_Type *)ENET_BASE)
/** Array initializer of ENET peripheral base pointers */
#define ENET_BASES                               { ENET }

/**
 * @}
 */ /* end of group ENET_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- EWM Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup EWM_Peripheral_Access_Layer EWM Peripheral Access Layer
 * @{
 */

/** EWM - Register Layout Typedef */
typedef struct {
  __IO uint8_t CTRL;                               /**< Control Register, offset: 0x0 */
  __O  uint8_t SERV;                               /**< Service Register, offset: 0x1 */
  __IO uint8_t CMPL;                               /**< Compare Low Register, offset: 0x2 */
  __IO uint8_t CMPH;                               /**< Compare High Register, offset: 0x3 */
       uint8_t RESERVED_0[1];
  __IO uint8_t CLKPRESCALER;                       /**< Clock Prescaler Register, offset: 0x5 */
} EWM_Type;

/* ----------------------------------------------------------------------------
   -- EWM Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup EWM_Register_Masks EWM Register Masks
 * @{
 */

/* CTRL Bit Fields */
#define EWM_CTRL_EWMEN_MASK                      0x1u
#define EWM_CTRL_EWMEN_SHIFT                     0
#define EWM_CTRL_ASSIN_MASK                      0x2u
#define EWM_CTRL_ASSIN_SHIFT                     1
#define EWM_CTRL_INEN_MASK                       0x4u
#define EWM_CTRL_INEN_SHIFT                      2
#define EWM_CTRL_INTEN_MASK                      0x8u
#define EWM_CTRL_INTEN_SHIFT                     3
/* SERV Bit Fields */
#define EWM_SERV_SERVICE_MASK                    0xFFu
#define EWM_SERV_SERVICE_SHIFT                   0
#define EWM_SERV_SERVICE(x)                      (((uint8_t)(((uint8_t)(x))<<EWM_SERV_SERVICE_SHIFT))&EWM_SERV_SERVICE_MASK)
/* CMPL Bit Fields */
#define EWM_CMPL_COMPAREL_MASK                   0xFFu
#define EWM_CMPL_COMPAREL_SHIFT                  0
#define EWM_CMPL_COMPAREL(x)                     (((uint8_t)(((uint8_t)(x))<<EWM_CMPL_COMPAREL_SHIFT))&EWM_CMPL_COMPAREL_MASK)
/* CMPH Bit Fields */
#define EWM_CMPH_COMPAREH_MASK                   0xFFu
#define EWM_CMPH_COMPAREH_SHIFT                  0
#define EWM_CMPH_COMPAREH(x)                     (((uint8_t)(((uint8_t)(x))<<EWM_CMPH_COMPAREH_SHIFT))&EWM_CMPH_COMPAREH_MASK)
/* CLKPRESCALER Bit Fields */
#define EWM_CLKPRESCALER_CLK_DIV_MASK            0xFFu
#define EWM_CLKPRESCALER_CLK_DIV_SHIFT           0
#define EWM_CLKPRESCALER_CLK_DIV(x)              (((uint8_t)(((uint8_t)(x))<<EWM_CLKPRESCALER_CLK_DIV_SHIFT))&EWM_CLKPRESCALER_CLK_DIV_MASK)

/**
 * @}
 */ /* end of group EWM_Register_Masks */


/* EWM - Peripheral instance base addresses */
/** Peripheral EWM base address */
#define EWM_BASE                                 (0x40061000u)
/** Peripheral EWM base pointer */
#define EWM                                      ((EWM_Type *)EWM_BASE)
/** Array initializer of EWM peripheral base pointers */
#define EWM_BASES                                { EWM }

/**
 * @}
 */ /* end of group EWM_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- FB Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup FB_Peripheral_Access_Layer FB Peripheral Access Layer
 * @{
 */

/** FB - Register Layout Typedef */
typedef struct {
  struct {                                         /* offset: 0x0, array step: 0xC */
    __IO uint32_t CSAR;                              /**< Chip Select Address Register, array offset: 0x0, array step: 0xC */
    __IO uint32_t CSMR;                              /**< Chip Select Mask Register, array offset: 0x4, array step: 0xC */
    __IO uint32_t CSCR;                              /**< Chip Select Control Register, array offset: 0x8, array step: 0xC */
  } CS[6];
       uint8_t RESERVED_0[24];
  __IO uint32_t CSPMCR;                            /**< Chip Select port Multiplexing Control Register, offset: 0x60 */
} FB_Type;

/* ----------------------------------------------------------------------------
   -- FB Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup FB_Register_Masks FB Register Masks
 * @{
 */

/* CSAR Bit Fields */
#define FB_CSAR_BA_MASK                          0xFFFF0000u
#define FB_CSAR_BA_SHIFT                         16
#define FB_CSAR_BA(x)                            (((uint32_t)(((uint32_t)(x))<<FB_CSAR_BA_SHIFT))&FB_CSAR_BA_MASK)
/* CSMR Bit Fields */
#define FB_CSMR_V_MASK                           0x1u
#define FB_CSMR_V_SHIFT                          0
#define FB_CSMR_WP_MASK                          0x100u
#define FB_CSMR_WP_SHIFT                         8
#define FB_CSMR_BAM_MASK                         0xFFFF0000u
#define FB_CSMR_BAM_SHIFT                        16
#define FB_CSMR_BAM(x)                           (((uint32_t)(((uint32_t)(x))<<FB_CSMR_BAM_SHIFT))&FB_CSMR_BAM_MASK)
/* CSCR Bit Fields */
#define FB_CSCR_BSTW_MASK                        0x8u
#define FB_CSCR_BSTW_SHIFT                       3
#define FB_CSCR_BSTR_MASK                        0x10u
#define FB_CSCR_BSTR_SHIFT                       4
#define FB_CSCR_BEM_MASK                         0x20u
#define FB_CSCR_BEM_SHIFT                        5
#define FB_CSCR_PS_MASK                          0xC0u
#define FB_CSCR_PS_SHIFT                         6
#define FB_CSCR_PS(x)                            (((uint32_t)(((uint32_t)(x))<<FB_CSCR_PS_SHIFT))&FB_CSCR_PS_MASK)
#define FB_CSCR_AA_MASK                          0x100u
#define FB_CSCR_AA_SHIFT                         8
#define FB_CSCR_BLS_MASK                         0x200u
#define FB_CSCR_BLS_SHIFT                        9
#define FB_CSCR_WS_MASK                          0xFC00u
#define FB_CSCR_WS_SHIFT                         10
#define FB_CSCR_WS(x)                            (((uint32_t)(((uint32_t)(x))<<FB_CSCR_WS_SHIFT))&FB_CSCR_WS_MASK)
#define FB_CSCR_WRAH_MASK                        0x30000u
#define FB_CSCR_WRAH_SHIFT                       16
#define FB_CSCR_WRAH(x)                          (((uint32_t)(((uint32_t)(x))<<FB_CSCR_WRAH_SHIFT))&FB_CSCR_WRAH_MASK)
#define FB_CSCR_RDAH_MASK                        0xC0000u
#define FB_CSCR_RDAH_SHIFT                       18
#define FB_CSCR_RDAH(x)                          (((uint32_t)(((uint32_t)(x))<<FB_CSCR_RDAH_SHIFT))&FB_CSCR_RDAH_MASK)
#define FB_CSCR_ASET_MASK                        0x300000u
#define FB_CSCR_ASET_SHIFT                       20
#define FB_CSCR_ASET(x)                          (((uint32_t)(((uint32_t)(x))<<FB_CSCR_ASET_SHIFT))&FB_CSCR_ASET_MASK)
#define FB_CSCR_EXTS_MASK                        0x400000u
#define FB_CSCR_EXTS_SHIFT                       22
#define FB_CSCR_SWSEN_MASK                       0x800000u
#define FB_CSCR_SWSEN_SHIFT                      23
#define FB_CSCR_SWS_MASK                         0xFC000000u
#define FB_CSCR_SWS_SHIFT                        26
#define FB_CSCR_SWS(x)                           (((uint32_t)(((uint32_t)(x))<<FB_CSCR_SWS_SHIFT))&FB_CSCR_SWS_MASK)
/* CSPMCR Bit Fields */
#define FB_CSPMCR_GROUP5_MASK                    0xF000u
#define FB_CSPMCR_GROUP5_SHIFT                   12
#define FB_CSPMCR_GROUP5(x)                      (((uint32_t)(((uint32_t)(x))<<FB_CSPMCR_GROUP5_SHIFT))&FB_CSPMCR_GROUP5_MASK)
#define FB_CSPMCR_GROUP4_MASK                    0xF0000u
#define FB_CSPMCR_GROUP4_SHIFT                   16
#define FB_CSPMCR_GROUP4(x)                      (((uint32_t)(((uint32_t)(x))<<FB_CSPMCR_GROUP4_SHIFT))&FB_CSPMCR_GROUP4_MASK)
#define FB_CSPMCR_GROUP3_MASK                    0xF00000u
#define FB_CSPMCR_GROUP3_SHIFT                   20
#define FB_CSPMCR_GROUP3(x)                      (((uint32_t)(((uint32_t)(x))<<FB_CSPMCR_GROUP3_SHIFT))&FB_CSPMCR_GROUP3_MASK)
#define FB_CSPMCR_GROUP2_MASK                    0xF000000u
#define FB_CSPMCR_GROUP2_SHIFT                   24
#define FB_CSPMCR_GROUP2(x)                      (((uint32_t)(((uint32_t)(x))<<FB_CSPMCR_GROUP2_SHIFT))&FB_CSPMCR_GROUP2_MASK)
#define FB_CSPMCR_GROUP1_MASK                    0xF0000000u
#define FB_CSPMCR_GROUP1_SHIFT                   28
#define FB_CSPMCR_GROUP1(x)                      (((uint32_t)(((uint32_t)(x))<<FB_CSPMCR_GROUP1_SHIFT))&FB_CSPMCR_GROUP1_MASK)

/**
 * @}
 */ /* end of group FB_Register_Masks */


/* FB - Peripheral instance base addresses */
/** Peripheral FB base address */
#define FB_BASE                                  (0x4000C000u)
/** Peripheral FB base pointer */
#define FB                                       ((FB_Type *)FB_BASE)
/** Array initializer of FB peripheral base pointers */
#define FB_BASES                                 { FB }

/**
 * @}
 */ /* end of group FB_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- FMC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup FMC_Peripheral_Access_Layer FMC Peripheral Access Layer
 * @{
 */

/** FMC - Register Layout Typedef */
typedef struct {
  __IO uint32_t PFAPR;                             /**< Flash Access Protection Register, offset: 0x0 */
  __IO uint32_t PFB0CR;                            /**< Flash Bank 0 Control Register, offset: 0x4 */
  __IO uint32_t PFB1CR;                            /**< Flash Bank 1 Control Register, offset: 0x8 */
       uint8_t RESERVED_0[244];
  __IO uint32_t TAGVD[4][8];                       /**< Cache Tag Storage, array offset: 0x100, array step: index*0x20, index2*0x4 */
       uint8_t RESERVED_1[128];
  struct {                                         /* offset: 0x200, array step: index*0x40, index2*0x8 */
    __IO uint32_t DATA_U;                            /**< Cache Data Storage (upper word), array offset: 0x200, array step: index*0x40, index2*0x8 */
    __IO uint32_t DATA_L;                            /**< Cache Data Storage (lower word), array offset: 0x204, array step: index*0x40, index2*0x8 */
  } SET[4][8];
} FMC_Type;

/* ----------------------------------------------------------------------------
   -- FMC Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup FMC_Register_Masks FMC Register Masks
 * @{
 */

/* PFAPR Bit Fields */
#define FMC_PFAPR_M0AP_MASK                      0x3u
#define FMC_PFAPR_M0AP_SHIFT                     0
#define FMC_PFAPR_M0AP(x)                        (((uint32_t)(((uint32_t)(x))<<FMC_PFAPR_M0AP_SHIFT))&FMC_PFAPR_M0AP_MASK)
#define FMC_PFAPR_M1AP_MASK                      0xCu
#define FMC_PFAPR_M1AP_SHIFT                     2
#define FMC_PFAPR_M1AP(x)                        (((uint32_t)(((uint32_t)(x))<<FMC_PFAPR_M1AP_SHIFT))&FMC_PFAPR_M1AP_MASK)
#define FMC_PFAPR_M2AP_MASK                      0x30u
#define FMC_PFAPR_M2AP_SHIFT                     4
#define FMC_PFAPR_M2AP(x)                        (((uint32_t)(((uint32_t)(x))<<FMC_PFAPR_M2AP_SHIFT))&FMC_PFAPR_M2AP_MASK)
#define FMC_PFAPR_M3AP_MASK                      0xC0u
#define FMC_PFAPR_M3AP_SHIFT                     6
#define FMC_PFAPR_M3AP(x)                        (((uint32_t)(((uint32_t)(x))<<FMC_PFAPR_M3AP_SHIFT))&FMC_PFAPR_M3AP_MASK)
#define FMC_PFAPR_M4AP_MASK                      0x300u
#define FMC_PFAPR_M4AP_SHIFT                     8
#define FMC_PFAPR_M4AP(x)                        (((uint32_t)(((uint32_t)(x))<<FMC_PFAPR_M4AP_SHIFT))&FMC_PFAPR_M4AP_MASK)
#define FMC_PFAPR_M5AP_MASK                      0xC00u
#define FMC_PFAPR_M5AP_SHIFT                     10
#define FMC_PFAPR_M5AP(x)                        (((uint32_t)(((uint32_t)(x))<<FMC_PFAPR_M5AP_SHIFT))&FMC_PFAPR_M5AP_MASK)
#define FMC_PFAPR_M6AP_MASK                      0x3000u
#define FMC_PFAPR_M6AP_SHIFT                     12
#define FMC_PFAPR_M6AP(x)                        (((uint32_t)(((uint32_t)(x))<<FMC_PFAPR_M6AP_SHIFT))&FMC_PFAPR_M6AP_MASK)
#define FMC_PFAPR_M7AP_MASK                      0xC000u
#define FMC_PFAPR_M7AP_SHIFT                     14
#define FMC_PFAPR_M7AP(x)                        (((uint32_t)(((uint32_t)(x))<<FMC_PFAPR_M7AP_SHIFT))&FMC_PFAPR_M7AP_MASK)
#define FMC_PFAPR_M0PFD_MASK                     0x10000u
#define FMC_PFAPR_M0PFD_SHIFT                    16
#define FMC_PFAPR_M1PFD_MASK                     0x20000u
#define FMC_PFAPR_M1PFD_SHIFT                    17
#define FMC_PFAPR_M2PFD_MASK                     0x40000u
#define FMC_PFAPR_M2PFD_SHIFT                    18
#define FMC_PFAPR_M3PFD_MASK                     0x80000u
#define FMC_PFAPR_M3PFD_SHIFT                    19
#define FMC_PFAPR_M4PFD_MASK                     0x100000u
#define FMC_PFAPR_M4PFD_SHIFT                    20
#define FMC_PFAPR_M5PFD_MASK                     0x200000u
#define FMC_PFAPR_M5PFD_SHIFT                    21
#define FMC_PFAPR_M6PFD_MASK                     0x400000u
#define FMC_PFAPR_M6PFD_SHIFT                    22
#define FMC_PFAPR_M7PFD_MASK                     0x800000u
#define FMC_PFAPR_M7PFD_SHIFT                    23
/* PFB0CR Bit Fields */
#define FMC_PFB0CR_B0SEBE_MASK                   0x1u
#define FMC_PFB0CR_B0SEBE_SHIFT                  0
#define FMC_PFB0CR_B0IPE_MASK                    0x2u
#define FMC_PFB0CR_B0IPE_SHIFT                   1
#define FMC_PFB0CR_B0DPE_MASK                    0x4u
#define FMC_PFB0CR_B0DPE_SHIFT                   2
#define FMC_PFB0CR_B0ICE_MASK                    0x8u
#define FMC_PFB0CR_B0ICE_SHIFT                   3
#define FMC_PFB0CR_B0DCE_MASK                    0x10u
#define FMC_PFB0CR_B0DCE_SHIFT                   4
#define FMC_PFB0CR_CRC_MASK                      0xE0u
#define FMC_PFB0CR_CRC_SHIFT                     5
#define FMC_PFB0CR_CRC(x)                        (((uint32_t)(((uint32_t)(x))<<FMC_PFB0CR_CRC_SHIFT))&FMC_PFB0CR_CRC_MASK)
#define FMC_PFB0CR_B0MW_MASK                     0x60000u
#define FMC_PFB0CR_B0MW_SHIFT                    17
#define FMC_PFB0CR_B0MW(x)                       (((uint32_t)(((uint32_t)(x))<<FMC_PFB0CR_B0MW_SHIFT))&FMC_PFB0CR_B0MW_MASK)
#define FMC_PFB0CR_S_B_INV_MASK                  0x80000u
#define FMC_PFB0CR_S_B_INV_SHIFT                 19
#define FMC_PFB0CR_CINV_WAY_MASK                 0xF00000u
#define FMC_PFB0CR_CINV_WAY_SHIFT                20
#define FMC_PFB0CR_CINV_WAY(x)                   (((uint32_t)(((uint32_t)(x))<<FMC_PFB0CR_CINV_WAY_SHIFT))&FMC_PFB0CR_CINV_WAY_MASK)
#define FMC_PFB0CR_CLCK_WAY_MASK                 0xF000000u
#define FMC_PFB0CR_CLCK_WAY_SHIFT                24
#define FMC_PFB0CR_CLCK_WAY(x)                   (((uint32_t)(((uint32_t)(x))<<FMC_PFB0CR_CLCK_WAY_SHIFT))&FMC_PFB0CR_CLCK_WAY_MASK)
#define FMC_PFB0CR_B0RWSC_MASK                   0xF0000000u
#define FMC_PFB0CR_B0RWSC_SHIFT                  28
#define FMC_PFB0CR_B0RWSC(x)                     (((uint32_t)(((uint32_t)(x))<<FMC_PFB0CR_B0RWSC_SHIFT))&FMC_PFB0CR_B0RWSC_MASK)
/* PFB1CR Bit Fields */
#define FMC_PFB1CR_B1SEBE_MASK                   0x1u
#define FMC_PFB1CR_B1SEBE_SHIFT                  0
#define FMC_PFB1CR_B1IPE_MASK                    0x2u
#define FMC_PFB1CR_B1IPE_SHIFT                   1
#define FMC_PFB1CR_B1DPE_MASK                    0x4u
#define FMC_PFB1CR_B1DPE_SHIFT                   2
#define FMC_PFB1CR_B1ICE_MASK                    0x8u
#define FMC_PFB1CR_B1ICE_SHIFT                   3
#define FMC_PFB1CR_B1DCE_MASK                    0x10u
#define FMC_PFB1CR_B1DCE_SHIFT                   4
#define FMC_PFB1CR_B1MW_MASK                     0x60000u
#define FMC_PFB1CR_B1MW_SHIFT                    17
#define FMC_PFB1CR_B1MW(x)                       (((uint32_t)(((uint32_t)(x))<<FMC_PFB1CR_B1MW_SHIFT))&FMC_PFB1CR_B1MW_MASK)
#define FMC_PFB1CR_B1RWSC_MASK                   0xF0000000u
#define FMC_PFB1CR_B1RWSC_SHIFT                  28
#define FMC_PFB1CR_B1RWSC(x)                     (((uint32_t)(((uint32_t)(x))<<FMC_PFB1CR_B1RWSC_SHIFT))&FMC_PFB1CR_B1RWSC_MASK)
/* TAGVD Bit Fields */
#define FMC_TAGVD_valid_MASK                     0x1u
#define FMC_TAGVD_valid_SHIFT                    0
#define FMC_TAGVD_tag_MASK                       0x7FFC0u
#define FMC_TAGVD_tag_SHIFT                      6
#define FMC_TAGVD_tag(x)                         (((uint32_t)(((uint32_t)(x))<<FMC_TAGVD_tag_SHIFT))&FMC_TAGVD_tag_MASK)
/* DATA_U Bit Fields */
#define FMC_DATA_U_data_MASK                     0xFFFFFFFFu
#define FMC_DATA_U_data_SHIFT                    0
#define FMC_DATA_U_data(x)                       (((uint32_t)(((uint32_t)(x))<<FMC_DATA_U_data_SHIFT))&FMC_DATA_U_data_MASK)
/* DATA_L Bit Fields */
#define FMC_DATA_L_data_MASK                     0xFFFFFFFFu
#define FMC_DATA_L_data_SHIFT                    0
#define FMC_DATA_L_data(x)                       (((uint32_t)(((uint32_t)(x))<<FMC_DATA_L_data_SHIFT))&FMC_DATA_L_data_MASK)

/**
 * @}
 */ /* end of group FMC_Register_Masks */


/* FMC - Peripheral instance base addresses */
/** Peripheral FMC base address */
#define FMC_BASE                                 (0x4001F000u)
/** Peripheral FMC base pointer */
#define FMC                                      ((FMC_Type *)FMC_BASE)
/** Array initializer of FMC peripheral base pointers */
#define FMC_BASES                                { FMC }

/**
 * @}
 */ /* end of group FMC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- FTFL Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup FTFL_Peripheral_Access_Layer FTFL Peripheral Access Layer
 * @{
 */

/** FTFL - Register Layout Typedef */
typedef struct {
  __IO uint8_t FSTAT;                              /**< Flash Status Register, offset: 0x0 */
  __IO uint8_t FCNFG;                              /**< Flash Configuration Register, offset: 0x1 */
  __I  uint8_t FSEC;                               /**< Flash Security Register, offset: 0x2 */
  __I  uint8_t FOPT;                               /**< Flash Option Register, offset: 0x3 */
  __IO uint8_t FCCOB3;                             /**< Flash Common Command Object Registers, offset: 0x4 */
  __IO uint8_t FCCOB2;                             /**< Flash Common Command Object Registers, offset: 0x5 */
  __IO uint8_t FCCOB1;                             /**< Flash Common Command Object Registers, offset: 0x6 */
  __IO uint8_t FCCOB0;                             /**< Flash Common Command Object Registers, offset: 0x7 */
  __IO uint8_t FCCOB7;                             /**< Flash Common Command Object Registers, offset: 0x8 */
  __IO uint8_t FCCOB6;                             /**< Flash Common Command Object Registers, offset: 0x9 */
  __IO uint8_t FCCOB5;                             /**< Flash Common Command Object Registers, offset: 0xA */
  __IO uint8_t FCCOB4;                             /**< Flash Common Command Object Registers, offset: 0xB */
  __IO uint8_t FCCOBB;                             /**< Flash Common Command Object Registers, offset: 0xC */
  __IO uint8_t FCCOBA;                             /**< Flash Common Command Object Registers, offset: 0xD */
  __IO uint8_t FCCOB9;                             /**< Flash Common Command Object Registers, offset: 0xE */
  __IO uint8_t FCCOB8;                             /**< Flash Common Command Object Registers, offset: 0xF */
  __IO uint8_t FPROT3;                             /**< Program Flash Protection Registers, offset: 0x10 */
  __IO uint8_t FPROT2;                             /**< Program Flash Protection Registers, offset: 0x11 */
  __IO uint8_t FPROT1;                             /**< Program Flash Protection Registers, offset: 0x12 */
  __IO uint8_t FPROT0;                             /**< Program Flash Protection Registers, offset: 0x13 */
       uint8_t RESERVED_0[2];
  __IO uint8_t FEPROT;                             /**< EEPROM Protection Register, offset: 0x16 */
  __IO uint8_t FDPROT;                             /**< Data Flash Protection Register, offset: 0x17 */
} FTFL_Type;

/* ----------------------------------------------------------------------------
   -- FTFL Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup FTFL_Register_Masks FTFL Register Masks
 * @{
 */

/* FSTAT Bit Fields */
#define FTFL_FSTAT_MGSTAT0_MASK                  0x1u
#define FTFL_FSTAT_MGSTAT0_SHIFT                 0
#define FTFL_FSTAT_FPVIOL_MASK                   0x10u
#define FTFL_FSTAT_FPVIOL_SHIFT                  4
#define FTFL_FSTAT_ACCERR_MASK                   0x20u
#define FTFL_FSTAT_ACCERR_SHIFT                  5
#define FTFL_FSTAT_RDCOLERR_MASK                 0x40u
#define FTFL_FSTAT_RDCOLERR_SHIFT                6
#define FTFL_FSTAT_CCIF_MASK                     0x80u
#define FTFL_FSTAT_CCIF_SHIFT                    7
/* FCNFG Bit Fields */
#define FTFL_FCNFG_EEERDY_MASK                   0x1u
#define FTFL_FCNFG_EEERDY_SHIFT                  0
#define FTFL_FCNFG_RAMRDY_MASK                   0x2u
#define FTFL_FCNFG_RAMRDY_SHIFT                  1
#define FTFL_FCNFG_PFLSH_MASK                    0x4u
#define FTFL_FCNFG_PFLSH_SHIFT                   2
#define FTFL_FCNFG_SWAP_MASK                     0x8u
#define FTFL_FCNFG_SWAP_SHIFT                    3
#define FTFL_FCNFG_ERSSUSP_MASK                  0x10u
#define FTFL_FCNFG_ERSSUSP_SHIFT                 4
#define FTFL_FCNFG_ERSAREQ_MASK                  0x20u
#define FTFL_FCNFG_ERSAREQ_SHIFT                 5
#define FTFL_FCNFG_RDCOLLIE_MASK                 0x40u
#define FTFL_FCNFG_RDCOLLIE_SHIFT                6
#define FTFL_FCNFG_CCIE_MASK                     0x80u
#define FTFL_FCNFG_CCIE_SHIFT                    7
/* FSEC Bit Fields */
#define FTFL_FSEC_SEC_MASK                       0x3u
#define FTFL_FSEC_SEC_SHIFT                      0
#define FTFL_FSEC_SEC(x)                         (((uint8_t)(((uint8_t)(x))<<FTFL_FSEC_SEC_SHIFT))&FTFL_FSEC_SEC_MASK)
#define FTFL_FSEC_FSLACC_MASK                    0xCu
#define FTFL_FSEC_FSLACC_SHIFT                   2
#define FTFL_FSEC_FSLACC(x)                      (((uint8_t)(((uint8_t)(x))<<FTFL_FSEC_FSLACC_SHIFT))&FTFL_FSEC_FSLACC_MASK)
#define FTFL_FSEC_MEEN_MASK                      0x30u
#define FTFL_FSEC_MEEN_SHIFT                     4
#define FTFL_FSEC_MEEN(x)                        (((uint8_t)(((uint8_t)(x))<<FTFL_FSEC_MEEN_SHIFT))&FTFL_FSEC_MEEN_MASK)
#define FTFL_FSEC_KEYEN_MASK                     0xC0u
#define FTFL_FSEC_KEYEN_SHIFT                    6
#define FTFL_FSEC_KEYEN(x)                       (((uint8_t)(((uint8_t)(x))<<FTFL_FSEC_KEYEN_SHIFT))&FTFL_FSEC_KEYEN_MASK)
/* FOPT Bit Fields */
#define FTFL_FOPT_OPT_MASK                       0xFFu
#define FTFL_FOPT_OPT_SHIFT                      0
#define FTFL_FOPT_OPT(x)                         (((uint8_t)(((uint8_t)(x))<<FTFL_FOPT_OPT_SHIFT))&FTFL_FOPT_OPT_MASK)
/* FCCOB3 Bit Fields */
#define FTFL_FCCOB3_CCOBn_MASK                   0xFFu
#define FTFL_FCCOB3_CCOBn_SHIFT                  0
#define FTFL_FCCOB3_CCOBn(x)                     (((uint8_t)(((uint8_t)(x))<<FTFL_FCCOB3_CCOBn_SHIFT))&FTFL_FCCOB3_CCOBn_MASK)
/* FCCOB2 Bit Fields */
#define FTFL_FCCOB2_CCOBn_MASK                   0xFFu
#define FTFL_FCCOB2_CCOBn_SHIFT                  0
#define FTFL_FCCOB2_CCOBn(x)                     (((uint8_t)(((uint8_t)(x))<<FTFL_FCCOB2_CCOBn_SHIFT))&FTFL_FCCOB2_CCOBn_MASK)
/* FCCOB1 Bit Fields */
#define FTFL_FCCOB1_CCOBn_MASK                   0xFFu
#define FTFL_FCCOB1_CCOBn_SHIFT                  0
#define FTFL_FCCOB1_CCOBn(x)                     (((uint8_t)(((uint8_t)(x))<<FTFL_FCCOB1_CCOBn_SHIFT))&FTFL_FCCOB1_CCOBn_MASK)
/* FCCOB0 Bit Fields */
#define FTFL_FCCOB0_CCOBn_MASK                   0xFFu
#define FTFL_FCCOB0_CCOBn_SHIFT                  0
#define FTFL_FCCOB0_CCOBn(x)                     (((uint8_t)(((uint8_t)(x))<<FTFL_FCCOB0_CCOBn_SHIFT))&FTFL_FCCOB0_CCOBn_MASK)
/* FCCOB7 Bit Fields */
#define FTFL_FCCOB7_CCOBn_MASK                   0xFFu
#define FTFL_FCCOB7_CCOBn_SHIFT                  0
#define FTFL_FCCOB7_CCOBn(x)                     (((uint8_t)(((uint8_t)(x))<<FTFL_FCCOB7_CCOBn_SHIFT))&FTFL_FCCOB7_CCOBn_MASK)
/* FCCOB6 Bit Fields */
#define FTFL_FCCOB6_CCOBn_MASK                   0xFFu
#define FTFL_FCCOB6_CCOBn_SHIFT                  0
#define FTFL_FCCOB6_CCOBn(x)                     (((uint8_t)(((uint8_t)(x))<<FTFL_FCCOB6_CCOBn_SHIFT))&FTFL_FCCOB6_CCOBn_MASK)
/* FCCOB5 Bit Fields */
#define FTFL_FCCOB5_CCOBn_MASK                   0xFFu
#define FTFL_FCCOB5_CCOBn_SHIFT                  0
#define FTFL_FCCOB5_CCOBn(x)                     (((uint8_t)(((uint8_t)(x))<<FTFL_FCCOB5_CCOBn_SHIFT))&FTFL_FCCOB5_CCOBn_MASK)
/* FCCOB4 Bit Fields */
#define FTFL_FCCOB4_CCOBn_MASK                   0xFFu
#define FTFL_FCCOB4_CCOBn_SHIFT                  0
#define FTFL_FCCOB4_CCOBn(x)                     (((uint8_t)(((uint8_t)(x))<<FTFL_FCCOB4_CCOBn_SHIFT))&FTFL_FCCOB4_CCOBn_MASK)
/* FCCOBB Bit Fields */
#define FTFL_FCCOBB_CCOBn_MASK                   0xFFu
#define FTFL_FCCOBB_CCOBn_SHIFT                  0
#define FTFL_FCCOBB_CCOBn(x)                     (((uint8_t)(((uint8_t)(x))<<FTFL_FCCOBB_CCOBn_SHIFT))&FTFL_FCCOBB_CCOBn_MASK)
/* FCCOBA Bit Fields */
#define FTFL_FCCOBA_CCOBn_MASK                   0xFFu
#define FTFL_FCCOBA_CCOBn_SHIFT                  0
#define FTFL_FCCOBA_CCOBn(x)                     (((uint8_t)(((uint8_t)(x))<<FTFL_FCCOBA_CCOBn_SHIFT))&FTFL_FCCOBA_CCOBn_MASK)
/* FCCOB9 Bit Fields */
#define FTFL_FCCOB9_CCOBn_MASK                   0xFFu
#define FTFL_FCCOB9_CCOBn_SHIFT                  0
#define FTFL_FCCOB9_CCOBn(x)                     (((uint8_t)(((uint8_t)(x))<<FTFL_FCCOB9_CCOBn_SHIFT))&FTFL_FCCOB9_CCOBn_MASK)
/* FCCOB8 Bit Fields */
#define FTFL_FCCOB8_CCOBn_MASK                   0xFFu
#define FTFL_FCCOB8_CCOBn_SHIFT                  0
#define FTFL_FCCOB8_CCOBn(x)                     (((uint8_t)(((uint8_t)(x))<<FTFL_FCCOB8_CCOBn_SHIFT))&FTFL_FCCOB8_CCOBn_MASK)
/* FPROT3 Bit Fields */
#define FTFL_FPROT3_PROT_MASK                    0xFFu
#define FTFL_FPROT3_PROT_SHIFT                   0
#define FTFL_FPROT3_PROT(x)                      (((uint8_t)(((uint8_t)(x))<<FTFL_FPROT3_PROT_SHIFT))&FTFL_FPROT3_PROT_MASK)
/* FPROT2 Bit Fields */
#define FTFL_FPROT2_PROT_MASK                    0xFFu
#define FTFL_FPROT2_PROT_SHIFT                   0
#define FTFL_FPROT2_PROT(x)                      (((uint8_t)(((uint8_t)(x))<<FTFL_FPROT2_PROT_SHIFT))&FTFL_FPROT2_PROT_MASK)
/* FPROT1 Bit Fields */
#define FTFL_FPROT1_PROT_MASK                    0xFFu
#define FTFL_FPROT1_PROT_SHIFT                   0
#define FTFL_FPROT1_PROT(x)                      (((uint8_t)(((uint8_t)(x))<<FTFL_FPROT1_PROT_SHIFT))&FTFL_FPROT1_PROT_MASK)
/* FPROT0 Bit Fields */
#define FTFL_FPROT0_PROT_MASK                    0xFFu
#define FTFL_FPROT0_PROT_SHIFT                   0
#define FTFL_FPROT0_PROT(x)                      (((uint8_t)(((uint8_t)(x))<<FTFL_FPROT0_PROT_SHIFT))&FTFL_FPROT0_PROT_MASK)
/* FEPROT Bit Fields */
#define FTFL_FEPROT_EPROT_MASK                   0xFFu
#define FTFL_FEPROT_EPROT_SHIFT                  0
#define FTFL_FEPROT_EPROT(x)                     (((uint8_t)(((uint8_t)(x))<<FTFL_FEPROT_EPROT_SHIFT))&FTFL_FEPROT_EPROT_MASK)
/* FDPROT Bit Fields */
#define FTFL_FDPROT_DPROT_MASK                   0xFFu
#define FTFL_FDPROT_DPROT_SHIFT                  0
#define FTFL_FDPROT_DPROT(x)                     (((uint8_t)(((uint8_t)(x))<<FTFL_FDPROT_DPROT_SHIFT))&FTFL_FDPROT_DPROT_MASK)

/**
 * @}
 */ /* end of group FTFL_Register_Masks */


/* FTFL - Peripheral instance base addresses */
/** Peripheral FTFL base address */
#define FTFL_BASE                                (0x40020000u)
/** Peripheral FTFL base pointer */
#define FTFL                                     ((FTFL_Type *)FTFL_BASE)
/** Array initializer of FTFL peripheral base pointers */
#define FTFL_BASES                               { FTFL }

/**
 * @}
 */ /* end of group FTFL_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- FTM Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup FTM_Peripheral_Access_Layer FTM Peripheral Access Layer
 * @{
 */

/** FTM - Register Layout Typedef */
typedef struct {
  __IO uint32_t SC;                                /**< Status And Control, offset: 0x0 */
  __IO uint32_t CNT;                               /**< Counter, offset: 0x4 */
  __IO uint32_t MOD;                               /**< Modulo, offset: 0x8 */
  struct {                                         /* offset: 0xC, array step: 0x8 */
    __IO uint32_t CnSC;                              /**< Channel (n) Status And Control, array offset: 0xC, array step: 0x8 */
    __IO uint32_t CnV;                               /**< Channel (n) Value, array offset: 0x10, array step: 0x8 */
  } CONTROLS[8];
  __IO uint32_t CNTIN;                             /**< Counter Initial Value, offset: 0x4C */
  __I  uint32_t STATUS;                            /**< Capture And Compare Status, offset: 0x50 */
  __IO uint32_t MODE;                              /**< Features Mode Selection, offset: 0x54 */
  __IO uint32_t SYNC;                              /**< Synchronization, offset: 0x58 */
  __IO uint32_t OUTINIT;                           /**< Initial State For Channels Output, offset: 0x5C */
  __IO uint32_t OUTMASK;                           /**< Output Mask, offset: 0x60 */
  __IO uint32_t COMBINE;                           /**< Function For Linked Channels, offset: 0x64 */
  __IO uint32_t DEADTIME;                          /**< Deadtime Insertion Control, offset: 0x68 */
  __IO uint32_t EXTTRIG;                           /**< FTM External Trigger, offset: 0x6C */
  __IO uint32_t POL;                               /**< Channels Polarity, offset: 0x70 */
  __IO uint32_t FMS;                               /**< Fault Mode Status, offset: 0x74 */
  __IO uint32_t FILTER;                            /**< Input Capture Filter Control, offset: 0x78 */
  __IO uint32_t FLTCTRL;                           /**< Fault Control, offset: 0x7C */
  __IO uint32_t QDCTRL;                            /**< Quadrature Decoder Control And Status, offset: 0x80 */
  __IO uint32_t CONF;                              /**< Configuration, offset: 0x84 */
  __IO uint32_t FLTPOL;                            /**< FTM Fault Input Polarity, offset: 0x88 */
  __IO uint32_t SYNCONF;                           /**< Synchronization Configuration, offset: 0x8C */
  __IO uint32_t INVCTRL;                           /**< FTM Inverting Control, offset: 0x90 */
  __IO uint32_t SWOCTRL;                           /**< FTM Software Output Control, offset: 0x94 */
  __IO uint32_t PWMLOAD;                           /**< FTM PWM Load, offset: 0x98 */
} FTM_Type;

/* ----------------------------------------------------------------------------
   -- FTM Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup FTM_Register_Masks FTM Register Masks
 * @{
 */

/* SC Bit Fields */
#define FTM_SC_PS_MASK                           0x7u
#define FTM_SC_PS_SHIFT                          0
#define FTM_SC_PS(x)                             (((uint32_t)(((uint32_t)(x))<<FTM_SC_PS_SHIFT))&FTM_SC_PS_MASK)
#define FTM_SC_CLKS_MASK                         0x18u
#define FTM_SC_CLKS_SHIFT                        3
#define FTM_SC_CLKS(x)                           (((uint32_t)(((uint32_t)(x))<<FTM_SC_CLKS_SHIFT))&FTM_SC_CLKS_MASK)
#define FTM_SC_CPWMS_MASK                        0x20u
#define FTM_SC_CPWMS_SHIFT                       5
#define FTM_SC_TOIE_MASK                         0x40u
#define FTM_SC_TOIE_SHIFT                        6
#define FTM_SC_TOF_MASK                          0x80u
#define FTM_SC_TOF_SHIFT                         7
/* CNT Bit Fields */
#define FTM_CNT_COUNT_MASK                       0xFFFFu
#define FTM_CNT_COUNT_SHIFT                      0
#define FTM_CNT_COUNT(x)                         (((uint32_t)(((uint32_t)(x))<<FTM_CNT_COUNT_SHIFT))&FTM_CNT_COUNT_MASK)
/* MOD Bit Fields */
#define FTM_MOD_MOD_MASK                         0xFFFFu
#define FTM_MOD_MOD_SHIFT                        0
#define FTM_MOD_MOD(x)                           (((uint32_t)(((uint32_t)(x))<<FTM_MOD_MOD_SHIFT))&FTM_MOD_MOD_MASK)
/* CnSC Bit Fields */
#define FTM_CnSC_DMA_MASK                        0x1u
#define FTM_CnSC_DMA_SHIFT                       0
#define FTM_CnSC_ELSA_MASK                       0x4u
#define FTM_CnSC_ELSA_SHIFT                      2
#define FTM_CnSC_ELSB_MASK                       0x8u
#define FTM_CnSC_ELSB_SHIFT                      3
#define FTM_CnSC_MSA_MASK                        0x10u
#define FTM_CnSC_MSA_SHIFT                       4
#define FTM_CnSC_MSB_MASK                        0x20u
#define FTM_CnSC_MSB_SHIFT                       5
#define FTM_CnSC_CHIE_MASK                       0x40u
#define FTM_CnSC_CHIE_SHIFT                      6
#define FTM_CnSC_CHF_MASK                        0x80u
#define FTM_CnSC_CHF_SHIFT                       7
/* CnV Bit Fields */
#define FTM_CnV_VAL_MASK                         0xFFFFu
#define FTM_CnV_VAL_SHIFT                        0
#define FTM_CnV_VAL(x)                           (((uint32_t)(((uint32_t)(x))<<FTM_CnV_VAL_SHIFT))&FTM_CnV_VAL_MASK)
/* CNTIN Bit Fields */
#define FTM_CNTIN_INIT_MASK                      0xFFFFu
#define FTM_CNTIN_INIT_SHIFT                     0
#define FTM_CNTIN_INIT(x)                        (((uint32_t)(((uint32_t)(x))<<FTM_CNTIN_INIT_SHIFT))&FTM_CNTIN_INIT_MASK)
/* STATUS Bit Fields */
#define FTM_STATUS_CH0F_MASK                     0x1u
#define FTM_STATUS_CH0F_SHIFT                    0
#define FTM_STATUS_CH1F_MASK                     0x2u
#define FTM_STATUS_CH1F_SHIFT                    1
#define FTM_STATUS_CH2F_MASK                     0x4u
#define FTM_STATUS_CH2F_SHIFT                    2
#define FTM_STATUS_CH3F_MASK                     0x8u
#define FTM_STATUS_CH3F_SHIFT                    3
#define FTM_STATUS_CH4F_MASK                     0x10u
#define FTM_STATUS_CH4F_SHIFT                    4
#define FTM_STATUS_CH5F_MASK                     0x20u
#define FTM_STATUS_CH5F_SHIFT                    5
#define FTM_STATUS_CH6F_MASK                     0x40u
#define FTM_STATUS_CH6F_SHIFT                    6
#define FTM_STATUS_CH7F_MASK                     0x80u
#define FTM_STATUS_CH7F_SHIFT                    7
/* MODE Bit Fields */
#define FTM_MODE_FTMEN_MASK                      0x1u
#define FTM_MODE_FTMEN_SHIFT                     0
#define FTM_MODE_INIT_MASK                       0x2u
#define FTM_MODE_INIT_SHIFT                      1
#define FTM_MODE_WPDIS_MASK                      0x4u
#define FTM_MODE_WPDIS_SHIFT                     2
#define FTM_MODE_PWMSYNC_MASK                    0x8u
#define FTM_MODE_PWMSYNC_SHIFT                   3
#define FTM_MODE_CAPTEST_MASK                    0x10u
#define FTM_MODE_CAPTEST_SHIFT                   4
#define FTM_MODE_FAULTM_MASK                     0x60u
#define FTM_MODE_FAULTM_SHIFT                    5
#define FTM_MODE_FAULTM(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_MODE_FAULTM_SHIFT))&FTM_MODE_FAULTM_MASK)
#define FTM_MODE_FAULTIE_MASK                    0x80u
#define FTM_MODE_FAULTIE_SHIFT                   7
/* SYNC Bit Fields */
#define FTM_SYNC_CNTMIN_MASK                     0x1u
#define FTM_SYNC_CNTMIN_SHIFT                    0
#define FTM_SYNC_CNTMAX_MASK                     0x2u
#define FTM_SYNC_CNTMAX_SHIFT                    1
#define FTM_SYNC_REINIT_MASK                     0x4u
#define FTM_SYNC_REINIT_SHIFT                    2
#define FTM_SYNC_SYNCHOM_MASK                    0x8u
#define FTM_SYNC_SYNCHOM_SHIFT                   3
#define FTM_SYNC_TRIG0_MASK                      0x10u
#define FTM_SYNC_TRIG0_SHIFT                     4
#define FTM_SYNC_TRIG1_MASK                      0x20u
#define FTM_SYNC_TRIG1_SHIFT                     5
#define FTM_SYNC_TRIG2_MASK                      0x40u
#define FTM_SYNC_TRIG2_SHIFT                     6
#define FTM_SYNC_SWSYNC_MASK                     0x80u
#define FTM_SYNC_SWSYNC_SHIFT                    7
/* OUTINIT Bit Fields */
#define FTM_OUTINIT_CH0OI_MASK                   0x1u
#define FTM_OUTINIT_CH0OI_SHIFT                  0
#define FTM_OUTINIT_CH1OI_MASK                   0x2u
#define FTM_OUTINIT_CH1OI_SHIFT                  1
#define FTM_OUTINIT_CH2OI_MASK                   0x4u
#define FTM_OUTINIT_CH2OI_SHIFT                  2
#define FTM_OUTINIT_CH3OI_MASK                   0x8u
#define FTM_OUTINIT_CH3OI_SHIFT                  3
#define FTM_OUTINIT_CH4OI_MASK                   0x10u
#define FTM_OUTINIT_CH4OI_SHIFT                  4
#define FTM_OUTINIT_CH5OI_MASK                   0x20u
#define FTM_OUTINIT_CH5OI_SHIFT                  5
#define FTM_OUTINIT_CH6OI_MASK                   0x40u
#define FTM_OUTINIT_CH6OI_SHIFT                  6
#define FTM_OUTINIT_CH7OI_MASK                   0x80u
#define FTM_OUTINIT_CH7OI_SHIFT                  7
/* OUTMASK Bit Fields */
#define FTM_OUTMASK_CH0OM_MASK                   0x1u
#define FTM_OUTMASK_CH0OM_SHIFT                  0
#define FTM_OUTMASK_CH1OM_MASK                   0x2u
#define FTM_OUTMASK_CH1OM_SHIFT                  1
#define FTM_OUTMASK_CH2OM_MASK                   0x4u
#define FTM_OUTMASK_CH2OM_SHIFT                  2
#define FTM_OUTMASK_CH3OM_MASK                   0x8u
#define FTM_OUTMASK_CH3OM_SHIFT                  3
#define FTM_OUTMASK_CH4OM_MASK                   0x10u
#define FTM_OUTMASK_CH4OM_SHIFT                  4
#define FTM_OUTMASK_CH5OM_MASK                   0x20u
#define FTM_OUTMASK_CH5OM_SHIFT                  5
#define FTM_OUTMASK_CH6OM_MASK                   0x40u
#define FTM_OUTMASK_CH6OM_SHIFT                  6
#define FTM_OUTMASK_CH7OM_MASK                   0x80u
#define FTM_OUTMASK_CH7OM_SHIFT                  7
/* COMBINE Bit Fields */
#define FTM_COMBINE_COMBINE0_MASK                0x1u
#define FTM_COMBINE_COMBINE0_SHIFT               0
#define FTM_COMBINE_COMP0_MASK                   0x2u
#define FTM_COMBINE_COMP0_SHIFT                  1
#define FTM_COMBINE_DECAPEN0_MASK                0x4u
#define FTM_COMBINE_DECAPEN0_SHIFT               2
#define FTM_COMBINE_DECAP0_MASK                  0x8u
#define FTM_COMBINE_DECAP0_SHIFT                 3
#define FTM_COMBINE_DTEN0_MASK                   0x10u
#define FTM_COMBINE_DTEN0_SHIFT                  4
#define FTM_COMBINE_SYNCEN0_MASK                 0x20u
#define FTM_COMBINE_SYNCEN0_SHIFT                5
#define FTM_COMBINE_FAULTEN0_MASK                0x40u
#define FTM_COMBINE_FAULTEN0_SHIFT               6
#define FTM_COMBINE_COMBINE1_MASK                0x100u
#define FTM_COMBINE_COMBINE1_SHIFT               8
#define FTM_COMBINE_COMP1_MASK                   0x200u
#define FTM_COMBINE_COMP1_SHIFT                  9
#define FTM_COMBINE_DECAPEN1_MASK                0x400u
#define FTM_COMBINE_DECAPEN1_SHIFT               10
#define FTM_COMBINE_DECAP1_MASK                  0x800u
#define FTM_COMBINE_DECAP1_SHIFT                 11
#define FTM_COMBINE_DTEN1_MASK                   0x1000u
#define FTM_COMBINE_DTEN1_SHIFT                  12
#define FTM_COMBINE_SYNCEN1_MASK                 0x2000u
#define FTM_COMBINE_SYNCEN1_SHIFT                13
#define FTM_COMBINE_FAULTEN1_MASK                0x4000u
#define FTM_COMBINE_FAULTEN1_SHIFT               14
#define FTM_COMBINE_COMBINE2_MASK                0x10000u
#define FTM_COMBINE_COMBINE2_SHIFT               16
#define FTM_COMBINE_COMP2_MASK                   0x20000u
#define FTM_COMBINE_COMP2_SHIFT                  17
#define FTM_COMBINE_DECAPEN2_MASK                0x40000u
#define FTM_COMBINE_DECAPEN2_SHIFT               18
#define FTM_COMBINE_DECAP2_MASK                  0x80000u
#define FTM_COMBINE_DECAP2_SHIFT                 19
#define FTM_COMBINE_DTEN2_MASK                   0x100000u
#define FTM_COMBINE_DTEN2_SHIFT                  20
#define FTM_COMBINE_SYNCEN2_MASK                 0x200000u
#define FTM_COMBINE_SYNCEN2_SHIFT                21
#define FTM_COMBINE_FAULTEN2_MASK                0x400000u
#define FTM_COMBINE_FAULTEN2_SHIFT               22
#define FTM_COMBINE_COMBINE3_MASK                0x1000000u
#define FTM_COMBINE_COMBINE3_SHIFT               24
#define FTM_COMBINE_COMP3_MASK                   0x2000000u
#define FTM_COMBINE_COMP3_SHIFT                  25
#define FTM_COMBINE_DECAPEN3_MASK                0x4000000u
#define FTM_COMBINE_DECAPEN3_SHIFT               26
#define FTM_COMBINE_DECAP3_MASK                  0x8000000u
#define FTM_COMBINE_DECAP3_SHIFT                 27
#define FTM_COMBINE_DTEN3_MASK                   0x10000000u
#define FTM_COMBINE_DTEN3_SHIFT                  28
#define FTM_COMBINE_SYNCEN3_MASK                 0x20000000u
#define FTM_COMBINE_SYNCEN3_SHIFT                29
#define FTM_COMBINE_FAULTEN3_MASK                0x40000000u
#define FTM_COMBINE_FAULTEN3_SHIFT               30
/* DEADTIME Bit Fields */
#define FTM_DEADTIME_DTVAL_MASK                  0x3Fu
#define FTM_DEADTIME_DTVAL_SHIFT                 0
#define FTM_DEADTIME_DTVAL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_DEADTIME_DTVAL_SHIFT))&FTM_DEADTIME_DTVAL_MASK)
#define FTM_DEADTIME_DTPS_MASK                   0xC0u
#define FTM_DEADTIME_DTPS_SHIFT                  6
#define FTM_DEADTIME_DTPS(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_DEADTIME_DTPS_SHIFT))&FTM_DEADTIME_DTPS_MASK)
/* EXTTRIG Bit Fields */
#define FTM_EXTTRIG_CH2TRIG_MASK                 0x1u
#define FTM_EXTTRIG_CH2TRIG_SHIFT                0
#define FTM_EXTTRIG_CH3TRIG_MASK                 0x2u
#define FTM_EXTTRIG_CH3TRIG_SHIFT                1
#define FTM_EXTTRIG_CH4TRIG_MASK                 0x4u
#define FTM_EXTTRIG_CH4TRIG_SHIFT                2
#define FTM_EXTTRIG_CH5TRIG_MASK                 0x8u
#define FTM_EXTTRIG_CH5TRIG_SHIFT                3
#define FTM_EXTTRIG_CH0TRIG_MASK                 0x10u
#define FTM_EXTTRIG_CH0TRIG_SHIFT                4
#define FTM_EXTTRIG_CH1TRIG_MASK                 0x20u
#define FTM_EXTTRIG_CH1TRIG_SHIFT                5
#define FTM_EXTTRIG_INITTRIGEN_MASK              0x40u
#define FTM_EXTTRIG_INITTRIGEN_SHIFT             6
#define FTM_EXTTRIG_TRIGF_MASK                   0x80u
#define FTM_EXTTRIG_TRIGF_SHIFT                  7
/* POL Bit Fields */
#define FTM_POL_POL0_MASK                        0x1u
#define FTM_POL_POL0_SHIFT                       0
#define FTM_POL_POL1_MASK                        0x2u
#define FTM_POL_POL1_SHIFT                       1
#define FTM_POL_POL2_MASK                        0x4u
#define FTM_POL_POL2_SHIFT                       2
#define FTM_POL_POL3_MASK                        0x8u
#define FTM_POL_POL3_SHIFT                       3
#define FTM_POL_POL4_MASK                        0x10u
#define FTM_POL_POL4_SHIFT                       4
#define FTM_POL_POL5_MASK                        0x20u
#define FTM_POL_POL5_SHIFT                       5
#define FTM_POL_POL6_MASK                        0x40u
#define FTM_POL_POL6_SHIFT                       6
#define FTM_POL_POL7_MASK                        0x80u
#define FTM_POL_POL7_SHIFT                       7
/* FMS Bit Fields */
#define FTM_FMS_FAULTF0_MASK                     0x1u
#define FTM_FMS_FAULTF0_SHIFT                    0
#define FTM_FMS_FAULTF1_MASK                     0x2u
#define FTM_FMS_FAULTF1_SHIFT                    1
#define FTM_FMS_FAULTF2_MASK                     0x4u
#define FTM_FMS_FAULTF2_SHIFT                    2
#define FTM_FMS_FAULTF3_MASK                     0x8u
#define FTM_FMS_FAULTF3_SHIFT                    3
#define FTM_FMS_FAULTIN_MASK                     0x20u
#define FTM_FMS_FAULTIN_SHIFT                    5
#define FTM_FMS_WPEN_MASK                        0x40u
#define FTM_FMS_WPEN_SHIFT                       6
#define FTM_FMS_FAULTF_MASK                      0x80u
#define FTM_FMS_FAULTF_SHIFT                     7
/* FILTER Bit Fields */
#define FTM_FILTER_CH0FVAL_MASK                  0xFu
#define FTM_FILTER_CH0FVAL_SHIFT                 0
#define FTM_FILTER_CH0FVAL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_FILTER_CH0FVAL_SHIFT))&FTM_FILTER_CH0FVAL_MASK)
#define FTM_FILTER_CH1FVAL_MASK                  0xF0u
#define FTM_FILTER_CH1FVAL_SHIFT                 4
#define FTM_FILTER_CH1FVAL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_FILTER_CH1FVAL_SHIFT))&FTM_FILTER_CH1FVAL_MASK)
#define FTM_FILTER_CH2FVAL_MASK                  0xF00u
#define FTM_FILTER_CH2FVAL_SHIFT                 8
#define FTM_FILTER_CH2FVAL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_FILTER_CH2FVAL_SHIFT))&FTM_FILTER_CH2FVAL_MASK)
#define FTM_FILTER_CH3FVAL_MASK                  0xF000u
#define FTM_FILTER_CH3FVAL_SHIFT                 12
#define FTM_FILTER_CH3FVAL(x)                    (((uint32_t)(((uint32_t)(x))<<FTM_FILTER_CH3FVAL_SHIFT))&FTM_FILTER_CH3FVAL_MASK)
/* FLTCTRL Bit Fields */
#define FTM_FLTCTRL_FAULT0EN_MASK                0x1u
#define FTM_FLTCTRL_FAULT0EN_SHIFT               0
#define FTM_FLTCTRL_FAULT1EN_MASK                0x2u
#define FTM_FLTCTRL_FAULT1EN_SHIFT               1
#define FTM_FLTCTRL_FAULT2EN_MASK                0x4u
#define FTM_FLTCTRL_FAULT2EN_SHIFT               2
#define FTM_FLTCTRL_FAULT3EN_MASK                0x8u
#define FTM_FLTCTRL_FAULT3EN_SHIFT               3
#define FTM_FLTCTRL_FFLTR0EN_MASK                0x10u
#define FTM_FLTCTRL_FFLTR0EN_SHIFT               4
#define FTM_FLTCTRL_FFLTR1EN_MASK                0x20u
#define FTM_FLTCTRL_FFLTR1EN_SHIFT               5
#define FTM_FLTCTRL_FFLTR2EN_MASK                0x40u
#define FTM_FLTCTRL_FFLTR2EN_SHIFT               6
#define FTM_FLTCTRL_FFLTR3EN_MASK                0x80u
#define FTM_FLTCTRL_FFLTR3EN_SHIFT               7
#define FTM_FLTCTRL_FFVAL_MASK                   0xF00u
#define FTM_FLTCTRL_FFVAL_SHIFT                  8
#define FTM_FLTCTRL_FFVAL(x)                     (((uint32_t)(((uint32_t)(x))<<FTM_FLTCTRL_FFVAL_SHIFT))&FTM_FLTCTRL_FFVAL_MASK)
/* QDCTRL Bit Fields */
#define FTM_QDCTRL_QUADEN_MASK                   0x1u
#define FTM_QDCTRL_QUADEN_SHIFT                  0
#define FTM_QDCTRL_TOFDIR_MASK                   0x2u
#define FTM_QDCTRL_TOFDIR_SHIFT                  1
#define FTM_QDCTRL_QUADIR_MASK                   0x4u
#define FTM_QDCTRL_QUADIR_SHIFT                  2
#define FTM_QDCTRL_QUADMODE_MASK                 0x8u
#define FTM_QDCTRL_QUADMODE_SHIFT                3
#define FTM_QDCTRL_PHBPOL_MASK                   0x10u
#define FTM_QDCTRL_PHBPOL_SHIFT                  4
#define FTM_QDCTRL_PHAPOL_MASK                   0x20u
#define FTM_QDCTRL_PHAPOL_SHIFT                  5
#define FTM_QDCTRL_PHBFLTREN_MASK                0x40u
#define FTM_QDCTRL_PHBFLTREN_SHIFT               6
#define FTM_QDCTRL_PHAFLTREN_MASK                0x80u
#define FTM_QDCTRL_PHAFLTREN_SHIFT               7
/* CONF Bit Fields */
#define FTM_CONF_NUMTOF_MASK                     0x1Fu
#define FTM_CONF_NUMTOF_SHIFT                    0
#define FTM_CONF_NUMTOF(x)                       (((uint32_t)(((uint32_t)(x))<<FTM_CONF_NUMTOF_SHIFT))&FTM_CONF_NUMTOF_MASK)
#define FTM_CONF_BDMMODE_MASK                    0xC0u
#define FTM_CONF_BDMMODE_SHIFT                   6
#define FTM_CONF_BDMMODE(x)                      (((uint32_t)(((uint32_t)(x))<<FTM_CONF_BDMMODE_SHIFT))&FTM_CONF_BDMMODE_MASK)
#define FTM_CONF_GTBEEN_MASK                     0x200u
#define FTM_CONF_GTBEEN_SHIFT                    9
#define FTM_CONF_GTBEOUT_MASK                    0x400u
#define FTM_CONF_GTBEOUT_SHIFT                   10
/* FLTPOL Bit Fields */
#define FTM_FLTPOL_FLT0POL_MASK                  0x1u
#define FTM_FLTPOL_FLT0POL_SHIFT                 0
#define FTM_FLTPOL_FLT1POL_MASK                  0x2u
#define FTM_FLTPOL_FLT1POL_SHIFT                 1
#define FTM_FLTPOL_FLT2POL_MASK                  0x4u
#define FTM_FLTPOL_FLT2POL_SHIFT                 2
#define FTM_FLTPOL_FLT3POL_MASK                  0x8u
#define FTM_FLTPOL_FLT3POL_SHIFT                 3
/* SYNCONF Bit Fields */
#define FTM_SYNCONF_HWTRIGMODE_MASK              0x1u
#define FTM_SYNCONF_HWTRIGMODE_SHIFT             0
#define FTM_SYNCONF_CNTINC_MASK                  0x4u
#define FTM_SYNCONF_CNTINC_SHIFT                 2
#define FTM_SYNCONF_INVC_MASK                    0x10u
#define FTM_SYNCONF_INVC_SHIFT                   4
#define FTM_SYNCONF_SWOC_MASK                    0x20u
#define FTM_SYNCONF_SWOC_SHIFT                   5
#define FTM_SYNCONF_SYNCMODE_MASK                0x80u
#define FTM_SYNCONF_SYNCMODE_SHIFT               7
#define FTM_SYNCONF_SWRSTCNT_MASK                0x100u
#define FTM_SYNCONF_SWRSTCNT_SHIFT               8
#define FTM_SYNCONF_SWWRBUF_MASK                 0x200u
#define FTM_SYNCONF_SWWRBUF_SHIFT                9
#define FTM_SYNCONF_SWOM_MASK                    0x400u
#define FTM_SYNCONF_SWOM_SHIFT                   10
#define FTM_SYNCONF_SWINVC_MASK                  0x800u
#define FTM_SYNCONF_SWINVC_SHIFT                 11
#define FTM_SYNCONF_SWSOC_MASK                   0x1000u
#define FTM_SYNCONF_SWSOC_SHIFT                  12
#define FTM_SYNCONF_HWRSTCNT_MASK                0x10000u
#define FTM_SYNCONF_HWRSTCNT_SHIFT               16
#define FTM_SYNCONF_HWWRBUF_MASK                 0x20000u
#define FTM_SYNCONF_HWWRBUF_SHIFT                17
#define FTM_SYNCONF_HWOM_MASK                    0x40000u
#define FTM_SYNCONF_HWOM_SHIFT                   18
#define FTM_SYNCONF_HWINVC_MASK                  0x80000u
#define FTM_SYNCONF_HWINVC_SHIFT                 19
#define FTM_SYNCONF_HWSOC_MASK                   0x100000u
#define FTM_SYNCONF_HWSOC_SHIFT                  20
/* INVCTRL Bit Fields */
#define FTM_INVCTRL_INV0EN_MASK                  0x1u
#define FTM_INVCTRL_INV0EN_SHIFT                 0
#define FTM_INVCTRL_INV1EN_MASK                  0x2u
#define FTM_INVCTRL_INV1EN_SHIFT                 1
#define FTM_INVCTRL_INV2EN_MASK                  0x4u
#define FTM_INVCTRL_INV2EN_SHIFT                 2
#define FTM_INVCTRL_INV3EN_MASK                  0x8u
#define FTM_INVCTRL_INV3EN_SHIFT                 3
/* SWOCTRL Bit Fields */
#define FTM_SWOCTRL_CH0OC_MASK                   0x1u
#define FTM_SWOCTRL_CH0OC_SHIFT                  0
#define FTM_SWOCTRL_CH1OC_MASK                   0x2u
#define FTM_SWOCTRL_CH1OC_SHIFT                  1
#define FTM_SWOCTRL_CH2OC_MASK                   0x4u
#define FTM_SWOCTRL_CH2OC_SHIFT                  2
#define FTM_SWOCTRL_CH3OC_MASK                   0x8u
#define FTM_SWOCTRL_CH3OC_SHIFT                  3
#define FTM_SWOCTRL_CH4OC_MASK                   0x10u
#define FTM_SWOCTRL_CH4OC_SHIFT                  4
#define FTM_SWOCTRL_CH5OC_MASK                   0x20u
#define FTM_SWOCTRL_CH5OC_SHIFT                  5
#define FTM_SWOCTRL_CH6OC_MASK                   0x40u
#define FTM_SWOCTRL_CH6OC_SHIFT                  6
#define FTM_SWOCTRL_CH7OC_MASK                   0x80u
#define FTM_SWOCTRL_CH7OC_SHIFT                  7
#define FTM_SWOCTRL_CH0OCV_MASK                  0x100u
#define FTM_SWOCTRL_CH0OCV_SHIFT                 8
#define FTM_SWOCTRL_CH1OCV_MASK                  0x200u
#define FTM_SWOCTRL_CH1OCV_SHIFT                 9
#define FTM_SWOCTRL_CH2OCV_MASK                  0x400u
#define FTM_SWOCTRL_CH2OCV_SHIFT                 10
#define FTM_SWOCTRL_CH3OCV_MASK                  0x800u
#define FTM_SWOCTRL_CH3OCV_SHIFT                 11
#define FTM_SWOCTRL_CH4OCV_MASK                  0x1000u
#define FTM_SWOCTRL_CH4OCV_SHIFT                 12
#define FTM_SWOCTRL_CH5OCV_MASK                  0x2000u
#define FTM_SWOCTRL_CH5OCV_SHIFT                 13
#define FTM_SWOCTRL_CH6OCV_MASK                  0x4000u
#define FTM_SWOCTRL_CH6OCV_SHIFT                 14
#define FTM_SWOCTRL_CH7OCV_MASK                  0x8000u
#define FTM_SWOCTRL_CH7OCV_SHIFT                 15
/* PWMLOAD Bit Fields */
#define FTM_PWMLOAD_CH0SEL_MASK                  0x1u
#define FTM_PWMLOAD_CH0SEL_SHIFT                 0
#define FTM_PWMLOAD_CH1SEL_MASK                  0x2u
#define FTM_PWMLOAD_CH1SEL_SHIFT                 1
#define FTM_PWMLOAD_CH2SEL_MASK                  0x4u
#define FTM_PWMLOAD_CH2SEL_SHIFT                 2
#define FTM_PWMLOAD_CH3SEL_MASK                  0x8u
#define FTM_PWMLOAD_CH3SEL_SHIFT                 3
#define FTM_PWMLOAD_CH4SEL_MASK                  0x10u
#define FTM_PWMLOAD_CH4SEL_SHIFT                 4
#define FTM_PWMLOAD_CH5SEL_MASK                  0x20u
#define FTM_PWMLOAD_CH5SEL_SHIFT                 5
#define FTM_PWMLOAD_CH6SEL_MASK                  0x40u
#define FTM_PWMLOAD_CH6SEL_SHIFT                 6
#define FTM_PWMLOAD_CH7SEL_MASK                  0x80u
#define FTM_PWMLOAD_CH7SEL_SHIFT                 7
#define FTM_PWMLOAD_LDOK_MASK                    0x200u
#define FTM_PWMLOAD_LDOK_SHIFT                   9

/**
 * @}
 */ /* end of group FTM_Register_Masks */


/* FTM - Peripheral instance base addresses */
/** Peripheral FTM0 base address */
#define FTM0_BASE                                (0x40038000u)
/** Peripheral FTM0 base pointer */
#define FTM0                                     ((FTM_Type *)FTM0_BASE)
/** Peripheral FTM1 base address */
#define FTM1_BASE                                (0x40039000u)
/** Peripheral FTM1 base pointer */
#define FTM1                                     ((FTM_Type *)FTM1_BASE)
/** Peripheral FTM2 base address */
#define FTM2_BASE                                (0x400B8000u)
/** Peripheral FTM2 base pointer */
#define FTM2                                     ((FTM_Type *)FTM2_BASE)
/** Array initializer of FTM peripheral base pointers */
#define FTM_BASES                                { FTM0, FTM1, FTM2 }

/**
 * @}
 */ /* end of group FTM_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- GPIO Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup GPIO_Peripheral_Access_Layer GPIO Peripheral Access Layer
 * @{
 */

/** GPIO - Register Layout Typedef */
typedef struct {
  __IO uint32_t PDOR;                              /**< Port Data Output Register, offset: 0x0 */
  __O  uint32_t PSOR;                              /**< Port Set Output Register, offset: 0x4 */
  __O  uint32_t PCOR;                              /**< Port Clear Output Register, offset: 0x8 */
  __O  uint32_t PTOR;                              /**< Port Toggle Output Register, offset: 0xC */
  __I  uint32_t PDIR;                              /**< Port Data Input Register, offset: 0x10 */
  __IO uint32_t PDDR;                              /**< Port Data Direction Register, offset: 0x14 */
} GPIO_Type;

/* ----------------------------------------------------------------------------
   -- GPIO Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup GPIO_Register_Masks GPIO Register Masks
 * @{
 */

/* PDOR Bit Fields */
#define GPIO_PDOR_PDO_MASK                       0xFFFFFFFFu
#define GPIO_PDOR_PDO_SHIFT                      0
#define GPIO_PDOR_PDO(x)                         (((uint32_t)(((uint32_t)(x))<<GPIO_PDOR_PDO_SHIFT))&GPIO_PDOR_PDO_MASK)
/* PSOR Bit Fields */
#define GPIO_PSOR_PTSO_MASK                      0xFFFFFFFFu
#define GPIO_PSOR_PTSO_SHIFT                     0
#define GPIO_PSOR_PTSO(x)                        (((uint32_t)(((uint32_t)(x))<<GPIO_PSOR_PTSO_SHIFT))&GPIO_PSOR_PTSO_MASK)
/* PCOR Bit Fields */
#define GPIO_PCOR_PTCO_MASK                      0xFFFFFFFFu
#define GPIO_PCOR_PTCO_SHIFT                     0
#define GPIO_PCOR_PTCO(x)                        (((uint32_t)(((uint32_t)(x))<<GPIO_PCOR_PTCO_SHIFT))&GPIO_PCOR_PTCO_MASK)
/* PTOR Bit Fields */
#define GPIO_PTOR_PTTO_MASK                      0xFFFFFFFFu
#define GPIO_PTOR_PTTO_SHIFT                     0
#define GPIO_PTOR_PTTO(x)                        (((uint32_t)(((uint32_t)(x))<<GPIO_PTOR_PTTO_SHIFT))&GPIO_PTOR_PTTO_MASK)
/* PDIR Bit Fields */
#define GPIO_PDIR_PDI_MASK                       0xFFFFFFFFu
#define GPIO_PDIR_PDI_SHIFT                      0
#define GPIO_PDIR_PDI(x)                         (((uint32_t)(((uint32_t)(x))<<GPIO_PDIR_PDI_SHIFT))&GPIO_PDIR_PDI_MASK)
/* PDDR Bit Fields */
#define GPIO_PDDR_PDD_MASK                       0xFFFFFFFFu
#define GPIO_PDDR_PDD_SHIFT                      0
#define GPIO_PDDR_PDD(x)                         (((uint32_t)(((uint32_t)(x))<<GPIO_PDDR_PDD_SHIFT))&GPIO_PDDR_PDD_MASK)

/**
 * @}
 */ /* end of group GPIO_Register_Masks */


/* GPIO - Peripheral instance base addresses */
/** Peripheral PTA base address */
#define PTA_BASE                                 (0x400FF000u)
/** Peripheral PTA base pointer */
#define PTA                                      ((GPIO_Type *)PTA_BASE)
/** Peripheral PTB base address */
#define PTB_BASE                                 (0x400FF040u)
/** Peripheral PTB base pointer */
#define PTB                                      ((GPIO_Type *)PTB_BASE)
/** Peripheral PTC base address */
#define PTC_BASE                                 (0x400FF080u)
/** Peripheral PTC base pointer */
#define PTC                                      ((GPIO_Type *)PTC_BASE)
/** Peripheral PTD base address */
#define PTD_BASE                                 (0x400FF0C0u)
/** Peripheral PTD base pointer */
#define PTD                                      ((GPIO_Type *)PTD_BASE)
/** Peripheral PTE base address */
#define PTE_BASE                                 (0x400FF100u)
/** Peripheral PTE base pointer */
#define PTE                                      ((GPIO_Type *)PTE_BASE)
/** Array initializer of GPIO peripheral base pointers */
#define GPIO_BASES                               { PTA, PTB, PTC, PTD, PTE }

/**
 * @}
 */ /* end of group GPIO_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- I2C Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup I2C_Peripheral_Access_Layer I2C Peripheral Access Layer
 * @{
 */

/** I2C - Register Layout Typedef */
typedef struct {
  __IO uint8_t A1;                                 /**< I2C Address Register 1, offset: 0x0 */
  __IO uint8_t F;                                  /**< I2C Frequency Divider register, offset: 0x1 */
  __IO uint8_t C1;                                 /**< I2C Control Register 1, offset: 0x2 */
  __IO uint8_t S;                                  /**< I2C Status register, offset: 0x3 */
  __IO uint8_t D;                                  /**< I2C Data I/O register, offset: 0x4 */
  __IO uint8_t C2;                                 /**< I2C Control Register 2, offset: 0x5 */
  __IO uint8_t FLT;                                /**< I2C Programmable Input Glitch Filter register, offset: 0x6 */
  __IO uint8_t RA;                                 /**< I2C Range Address register, offset: 0x7 */
  __IO uint8_t SMB;                                /**< I2C SMBus Control and Status register, offset: 0x8 */
  __IO uint8_t A2;                                 /**< I2C Address Register 2, offset: 0x9 */
  __IO uint8_t SLTH;                               /**< I2C SCL Low Timeout Register High, offset: 0xA */
  __IO uint8_t SLTL;                               /**< I2C SCL Low Timeout Register Low, offset: 0xB */
} I2C_Type;

/* ----------------------------------------------------------------------------
   -- I2C Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup I2C_Register_Masks I2C Register Masks
 * @{
 */

/* A1 Bit Fields */
#define I2C_A1_AD_MASK                           0xFEu
#define I2C_A1_AD_SHIFT                          1
#define I2C_A1_AD(x)                             (((uint8_t)(((uint8_t)(x))<<I2C_A1_AD_SHIFT))&I2C_A1_AD_MASK)
/* F Bit Fields */
#define I2C_F_ICR_MASK                           0x3Fu
#define I2C_F_ICR_SHIFT                          0
#define I2C_F_ICR(x)                             (((uint8_t)(((uint8_t)(x))<<I2C_F_ICR_SHIFT))&I2C_F_ICR_MASK)
#define I2C_F_MULT_MASK                          0xC0u
#define I2C_F_MULT_SHIFT                         6
#define I2C_F_MULT(x)                            (((uint8_t)(((uint8_t)(x))<<I2C_F_MULT_SHIFT))&I2C_F_MULT_MASK)
/* C1 Bit Fields */
#define I2C_C1_DMAEN_MASK                        0x1u
#define I2C_C1_DMAEN_SHIFT                       0
#define I2C_C1_WUEN_MASK                         0x2u
#define I2C_C1_WUEN_SHIFT                        1
#define I2C_C1_RSTA_MASK                         0x4u
#define I2C_C1_RSTA_SHIFT                        2
#define I2C_C1_TXAK_MASK                         0x8u
#define I2C_C1_TXAK_SHIFT                        3
#define I2C_C1_TX_MASK                           0x10u
#define I2C_C1_TX_SHIFT                          4
#define I2C_C1_MST_MASK                          0x20u
#define I2C_C1_MST_SHIFT                         5
#define I2C_C1_IICIE_MASK                        0x40u
#define I2C_C1_IICIE_SHIFT                       6
#define I2C_C1_IICEN_MASK                        0x80u
#define I2C_C1_IICEN_SHIFT                       7
/* S Bit Fields */
#define I2C_S_RXAK_MASK                          0x1u
#define I2C_S_RXAK_SHIFT                         0
#define I2C_S_IICIF_MASK                         0x2u
#define I2C_S_IICIF_SHIFT                        1
#define I2C_S_SRW_MASK                           0x4u
#define I2C_S_SRW_SHIFT                          2
#define I2C_S_RAM_MASK                           0x8u
#define I2C_S_RAM_SHIFT                          3
#define I2C_S_ARBL_MASK                          0x10u
#define I2C_S_ARBL_SHIFT                         4
#define I2C_S_BUSY_MASK                          0x20u
#define I2C_S_BUSY_SHIFT                         5
#define I2C_S_IAAS_MASK                          0x40u
#define I2C_S_IAAS_SHIFT                         6
#define I2C_S_TCF_MASK                           0x80u
#define I2C_S_TCF_SHIFT                          7
/* D Bit Fields */
#define I2C_D_DATA_MASK                          0xFFu
#define I2C_D_DATA_SHIFT                         0
#define I2C_D_DATA(x)                            (((uint8_t)(((uint8_t)(x))<<I2C_D_DATA_SHIFT))&I2C_D_DATA_MASK)
/* C2 Bit Fields */
#define I2C_C2_AD_MASK                           0x7u
#define I2C_C2_AD_SHIFT                          0
#define I2C_C2_AD(x)                             (((uint8_t)(((uint8_t)(x))<<I2C_C2_AD_SHIFT))&I2C_C2_AD_MASK)
#define I2C_C2_RMEN_MASK                         0x8u
#define I2C_C2_RMEN_SHIFT                        3
#define I2C_C2_SBRC_MASK                         0x10u
#define I2C_C2_SBRC_SHIFT                        4
#define I2C_C2_HDRS_MASK                         0x20u
#define I2C_C2_HDRS_SHIFT                        5
#define I2C_C2_ADEXT_MASK                        0x40u
#define I2C_C2_ADEXT_SHIFT                       6
#define I2C_C2_GCAEN_MASK                        0x80u
#define I2C_C2_GCAEN_SHIFT                       7
/* FLT Bit Fields */
#define I2C_FLT_FLT_MASK                         0x1Fu
#define I2C_FLT_FLT_SHIFT                        0
#define I2C_FLT_FLT(x)                           (((uint8_t)(((uint8_t)(x))<<I2C_FLT_FLT_SHIFT))&I2C_FLT_FLT_MASK)
/* RA Bit Fields */
#define I2C_RA_RAD_MASK                          0xFEu
#define I2C_RA_RAD_SHIFT                         1
#define I2C_RA_RAD(x)                            (((uint8_t)(((uint8_t)(x))<<I2C_RA_RAD_SHIFT))&I2C_RA_RAD_MASK)
/* SMB Bit Fields */
#define I2C_SMB_SHTF2IE_MASK                     0x1u
#define I2C_SMB_SHTF2IE_SHIFT                    0
#define I2C_SMB_SHTF2_MASK                       0x2u
#define I2C_SMB_SHTF2_SHIFT                      1
#define I2C_SMB_SHTF1_MASK                       0x4u
#define I2C_SMB_SHTF1_SHIFT                      2
#define I2C_SMB_SLTF_MASK                        0x8u
#define I2C_SMB_SLTF_SHIFT                       3
#define I2C_SMB_TCKSEL_MASK                      0x10u
#define I2C_SMB_TCKSEL_SHIFT                     4
#define I2C_SMB_SIICAEN_MASK                     0x20u
#define I2C_SMB_SIICAEN_SHIFT                    5
#define I2C_SMB_ALERTEN_MASK                     0x40u
#define I2C_SMB_ALERTEN_SHIFT                    6
#define I2C_SMB_FACK_MASK                        0x80u
#define I2C_SMB_FACK_SHIFT                       7
/* A2 Bit Fields */
#define I2C_A2_SAD_MASK                          0xFEu
#define I2C_A2_SAD_SHIFT                         1
#define I2C_A2_SAD(x)                            (((uint8_t)(((uint8_t)(x))<<I2C_A2_SAD_SHIFT))&I2C_A2_SAD_MASK)
/* SLTH Bit Fields */
#define I2C_SLTH_SSLT_MASK                       0xFFu
#define I2C_SLTH_SSLT_SHIFT                      0
#define I2C_SLTH_SSLT(x)                         (((uint8_t)(((uint8_t)(x))<<I2C_SLTH_SSLT_SHIFT))&I2C_SLTH_SSLT_MASK)
/* SLTL Bit Fields */
#define I2C_SLTL_SSLT_MASK                       0xFFu
#define I2C_SLTL_SSLT_SHIFT                      0
#define I2C_SLTL_SSLT(x)                         (((uint8_t)(((uint8_t)(x))<<I2C_SLTL_SSLT_SHIFT))&I2C_SLTL_SSLT_MASK)

/**
 * @}
 */ /* end of group I2C_Register_Masks */


/* I2C - Peripheral instance base addresses */
/** Peripheral I2C0 base address */
#define I2C0_BASE                                (0x40066000u)
/** Peripheral I2C0 base pointer */
#define I2C0                                     ((I2C_Type *)I2C0_BASE)
/** Peripheral I2C1 base address */
#define I2C1_BASE                                (0x40067000u)
/** Peripheral I2C1 base pointer */
#define I2C1                                     ((I2C_Type *)I2C1_BASE)
/** Array initializer of I2C peripheral base pointers */
#define I2C_BASES                                { I2C0, I2C1 }

/**
 * @}
 */ /* end of group I2C_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- I2S Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup I2S_Peripheral_Access_Layer I2S Peripheral Access Layer
 * @{
 */

/** I2S - Register Layout Typedef */
typedef struct {
  __IO uint32_t TCSR;                              /**< SAI Transmit Control Register, offset: 0x0 */
  __IO uint32_t TCR1;                              /**< SAI Transmit Configuration 1 Register, offset: 0x4 */
  __IO uint32_t TCR2;                              /**< SAI Transmit Configuration 2 Register, offset: 0x8 */
  __IO uint32_t TCR3;                              /**< SAI Transmit Configuration 3 Register, offset: 0xC */
  __IO uint32_t TCR4;                              /**< SAI Transmit Configuration 4 Register, offset: 0x10 */
  __IO uint32_t TCR5;                              /**< SAI Transmit Configuration 5 Register, offset: 0x14 */
       uint8_t RESERVED_0[8];
  __O  uint32_t TDR[2];                            /**< SAI Transmit Data Register, array offset: 0x20, array step: 0x4 */
       uint8_t RESERVED_1[24];
  __I  uint32_t TFR[2];                            /**< SAI Transmit FIFO Register, array offset: 0x40, array step: 0x4 */
       uint8_t RESERVED_2[24];
  __IO uint32_t TMR;                               /**< SAI Transmit Mask Register, offset: 0x60 */
       uint8_t RESERVED_3[28];
  __IO uint32_t RCSR;                              /**< SAI Receive Control Register, offset: 0x80 */
  __IO uint32_t RCR1;                              /**< SAI Receive Configuration 1 Register, offset: 0x84 */
  __IO uint32_t RCR2;                              /**< SAI Receive Configuration 2 Register, offset: 0x88 */
  __IO uint32_t RCR3;                              /**< SAI Receive Configuration 3 Register, offset: 0x8C */
  __IO uint32_t RCR4;                              /**< SAI Receive Configuration 4 Register, offset: 0x90 */
  __IO uint32_t RCR5;                              /**< SAI Receive Configuration 5 Register, offset: 0x94 */
       uint8_t RESERVED_4[8];
  __I  uint32_t RDR[2];                            /**< SAI Receive Data Register, array offset: 0xA0, array step: 0x4 */
       uint8_t RESERVED_5[24];
  __I  uint32_t RFR[2];                            /**< SAI Receive FIFO Register, array offset: 0xC0, array step: 0x4 */
       uint8_t RESERVED_6[24];
  __IO uint32_t RMR;                               /**< SAI Receive Mask Register, offset: 0xE0 */
       uint8_t RESERVED_7[28];
  __IO uint32_t MCR;                               /**< SAI MCLK Control Register, offset: 0x100 */
  __IO uint32_t MDR;                               /**< SAI MCLK Divide Register, offset: 0x104 */
} I2S_Type;

/* ----------------------------------------------------------------------------
   -- I2S Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup I2S_Register_Masks I2S Register Masks
 * @{
 */

/* TCSR Bit Fields */
#define I2S_TCSR_FRDE_MASK                       0x1u
#define I2S_TCSR_FRDE_SHIFT                      0
#define I2S_TCSR_FWDE_MASK                       0x2u
#define I2S_TCSR_FWDE_SHIFT                      1
#define I2S_TCSR_FRIE_MASK                       0x100u
#define I2S_TCSR_FRIE_SHIFT                      8
#define I2S_TCSR_FWIE_MASK                       0x200u
#define I2S_TCSR_FWIE_SHIFT                      9
#define I2S_TCSR_FEIE_MASK                       0x400u
#define I2S_TCSR_FEIE_SHIFT                      10
#define I2S_TCSR_SEIE_MASK                       0x800u
#define I2S_TCSR_SEIE_SHIFT                      11
#define I2S_TCSR_WSIE_MASK                       0x1000u
#define I2S_TCSR_WSIE_SHIFT                      12
#define I2S_TCSR_FRF_MASK                        0x10000u
#define I2S_TCSR_FRF_SHIFT                       16
#define I2S_TCSR_FWF_MASK                        0x20000u
#define I2S_TCSR_FWF_SHIFT                       17
#define I2S_TCSR_FEF_MASK                        0x40000u
#define I2S_TCSR_FEF_SHIFT                       18
#define I2S_TCSR_SEF_MASK                        0x80000u
#define I2S_TCSR_SEF_SHIFT                       19
#define I2S_TCSR_WSF_MASK                        0x100000u
#define I2S_TCSR_WSF_SHIFT                       20
#define I2S_TCSR_SR_MASK                         0x1000000u
#define I2S_TCSR_SR_SHIFT                        24
#define I2S_TCSR_FR_MASK                         0x2000000u
#define I2S_TCSR_FR_SHIFT                        25
#define I2S_TCSR_BCE_MASK                        0x10000000u
#define I2S_TCSR_BCE_SHIFT                       28
#define I2S_TCSR_DBGE_MASK                       0x20000000u
#define I2S_TCSR_DBGE_SHIFT                      29
#define I2S_TCSR_STOPE_MASK                      0x40000000u
#define I2S_TCSR_STOPE_SHIFT                     30
#define I2S_TCSR_TE_MASK                         0x80000000u
#define I2S_TCSR_TE_SHIFT                        31
/* TCR1 Bit Fields */
#define I2S_TCR1_TFW_MASK                        0x7u
#define I2S_TCR1_TFW_SHIFT                       0
#define I2S_TCR1_TFW(x)                          (((uint32_t)(((uint32_t)(x))<<I2S_TCR1_TFW_SHIFT))&I2S_TCR1_TFW_MASK)
/* TCR2 Bit Fields */
#define I2S_TCR2_DIV_MASK                        0xFFu
#define I2S_TCR2_DIV_SHIFT                       0
#define I2S_TCR2_DIV(x)                          (((uint32_t)(((uint32_t)(x))<<I2S_TCR2_DIV_SHIFT))&I2S_TCR2_DIV_MASK)
#define I2S_TCR2_BCD_MASK                        0x1000000u
#define I2S_TCR2_BCD_SHIFT                       24
#define I2S_TCR2_BCP_MASK                        0x2000000u
#define I2S_TCR2_BCP_SHIFT                       25
#define I2S_TCR2_MSEL_MASK                       0xC000000u
#define I2S_TCR2_MSEL_SHIFT                      26
#define I2S_TCR2_MSEL(x)                         (((uint32_t)(((uint32_t)(x))<<I2S_TCR2_MSEL_SHIFT))&I2S_TCR2_MSEL_MASK)
#define I2S_TCR2_BCI_MASK                        0x10000000u
#define I2S_TCR2_BCI_SHIFT                       28
#define I2S_TCR2_BCS_MASK                        0x20000000u
#define I2S_TCR2_BCS_SHIFT                       29
#define I2S_TCR2_SYNC_MASK                       0xC0000000u
#define I2S_TCR2_SYNC_SHIFT                      30
#define I2S_TCR2_SYNC(x)                         (((uint32_t)(((uint32_t)(x))<<I2S_TCR2_SYNC_SHIFT))&I2S_TCR2_SYNC_MASK)
/* TCR3 Bit Fields */
#define I2S_TCR3_WDFL_MASK                       0x1Fu
#define I2S_TCR3_WDFL_SHIFT                      0
#define I2S_TCR3_WDFL(x)                         (((uint32_t)(((uint32_t)(x))<<I2S_TCR3_WDFL_SHIFT))&I2S_TCR3_WDFL_MASK)
#define I2S_TCR3_TCE_MASK                        0x30000u
#define I2S_TCR3_TCE_SHIFT                       16
#define I2S_TCR3_TCE(x)                          (((uint32_t)(((uint32_t)(x))<<I2S_TCR3_TCE_SHIFT))&I2S_TCR3_TCE_MASK)
/* TCR4 Bit Fields */
#define I2S_TCR4_FSD_MASK                        0x1u
#define I2S_TCR4_FSD_SHIFT                       0
#define I2S_TCR4_FSP_MASK                        0x2u
#define I2S_TCR4_FSP_SHIFT                       1
#define I2S_TCR4_FSE_MASK                        0x8u
#define I2S_TCR4_FSE_SHIFT                       3
#define I2S_TCR4_MF_MASK                         0x10u
#define I2S_TCR4_MF_SHIFT                        4
#define I2S_TCR4_SYWD_MASK                       0x1F00u
#define I2S_TCR4_SYWD_SHIFT                      8
#define I2S_TCR4_SYWD(x)                         (((uint32_t)(((uint32_t)(x))<<I2S_TCR4_SYWD_SHIFT))&I2S_TCR4_SYWD_MASK)
#define I2S_TCR4_FRSZ_MASK                       0x1F0000u
#define I2S_TCR4_FRSZ_SHIFT                      16
#define I2S_TCR4_FRSZ(x)                         (((uint32_t)(((uint32_t)(x))<<I2S_TCR4_FRSZ_SHIFT))&I2S_TCR4_FRSZ_MASK)
/* TCR5 Bit Fields */
#define I2S_TCR5_FBT_MASK                        0x1F00u
#define I2S_TCR5_FBT_SHIFT                       8
#define I2S_TCR5_FBT(x)                          (((uint32_t)(((uint32_t)(x))<<I2S_TCR5_FBT_SHIFT))&I2S_TCR5_FBT_MASK)
#define I2S_TCR5_W0W_MASK                        0x1F0000u
#define I2S_TCR5_W0W_SHIFT                       16
#define I2S_TCR5_W0W(x)                          (((uint32_t)(((uint32_t)(x))<<I2S_TCR5_W0W_SHIFT))&I2S_TCR5_W0W_MASK)
#define I2S_TCR5_WNW_MASK                        0x1F000000u
#define I2S_TCR5_WNW_SHIFT                       24
#define I2S_TCR5_WNW(x)                          (((uint32_t)(((uint32_t)(x))<<I2S_TCR5_WNW_SHIFT))&I2S_TCR5_WNW_MASK)
/* TDR Bit Fields */
#define I2S_TDR_TDR_MASK                         0xFFFFFFFFu
#define I2S_TDR_TDR_SHIFT                        0
#define I2S_TDR_TDR(x)                           (((uint32_t)(((uint32_t)(x))<<I2S_TDR_TDR_SHIFT))&I2S_TDR_TDR_MASK)
/* TFR Bit Fields */
#define I2S_TFR_RFP_MASK                         0xFu
#define I2S_TFR_RFP_SHIFT                        0
#define I2S_TFR_RFP(x)                           (((uint32_t)(((uint32_t)(x))<<I2S_TFR_RFP_SHIFT))&I2S_TFR_RFP_MASK)
#define I2S_TFR_WFP_MASK                         0xF0000u
#define I2S_TFR_WFP_SHIFT                        16
#define I2S_TFR_WFP(x)                           (((uint32_t)(((uint32_t)(x))<<I2S_TFR_WFP_SHIFT))&I2S_TFR_WFP_MASK)
/* TMR Bit Fields */
#define I2S_TMR_TWM_MASK                         0xFFFFFFFFu
#define I2S_TMR_TWM_SHIFT                        0
#define I2S_TMR_TWM(x)                           (((uint32_t)(((uint32_t)(x))<<I2S_TMR_TWM_SHIFT))&I2S_TMR_TWM_MASK)
/* RCSR Bit Fields */
#define I2S_RCSR_FRDE_MASK                       0x1u
#define I2S_RCSR_FRDE_SHIFT                      0
#define I2S_RCSR_FWDE_MASK                       0x2u
#define I2S_RCSR_FWDE_SHIFT                      1
#define I2S_RCSR_FRIE_MASK                       0x100u
#define I2S_RCSR_FRIE_SHIFT                      8
#define I2S_RCSR_FWIE_MASK                       0x200u
#define I2S_RCSR_FWIE_SHIFT                      9
#define I2S_RCSR_FEIE_MASK                       0x400u
#define I2S_RCSR_FEIE_SHIFT                      10
#define I2S_RCSR_SEIE_MASK                       0x800u
#define I2S_RCSR_SEIE_SHIFT                      11
#define I2S_RCSR_WSIE_MASK                       0x1000u
#define I2S_RCSR_WSIE_SHIFT                      12
#define I2S_RCSR_FRF_MASK                        0x10000u
#define I2S_RCSR_FRF_SHIFT                       16
#define I2S_RCSR_FWF_MASK                        0x20000u
#define I2S_RCSR_FWF_SHIFT                       17
#define I2S_RCSR_FEF_MASK                        0x40000u
#define I2S_RCSR_FEF_SHIFT                       18
#define I2S_RCSR_SEF_MASK                        0x80000u
#define I2S_RCSR_SEF_SHIFT                       19
#define I2S_RCSR_WSF_MASK                        0x100000u
#define I2S_RCSR_WSF_SHIFT                       20
#define I2S_RCSR_SR_MASK                         0x1000000u
#define I2S_RCSR_SR_SHIFT                        24
#define I2S_RCSR_FR_MASK                         0x2000000u
#define I2S_RCSR_FR_SHIFT                        25
#define I2S_RCSR_BCE_MASK                        0x10000000u
#define I2S_RCSR_BCE_SHIFT                       28
#define I2S_RCSR_DBGE_MASK                       0x20000000u
#define I2S_RCSR_DBGE_SHIFT                      29
#define I2S_RCSR_STOPE_MASK                      0x40000000u
#define I2S_RCSR_STOPE_SHIFT                     30
#define I2S_RCSR_RE_MASK                         0x80000000u
#define I2S_RCSR_RE_SHIFT                        31
/* RCR1 Bit Fields */
#define I2S_RCR1_RFW_MASK                        0x7u
#define I2S_RCR1_RFW_SHIFT                       0
#define I2S_RCR1_RFW(x)                          (((uint32_t)(((uint32_t)(x))<<I2S_RCR1_RFW_SHIFT))&I2S_RCR1_RFW_MASK)
/* RCR2 Bit Fields */
#define I2S_RCR2_DIV_MASK                        0xFFu
#define I2S_RCR2_DIV_SHIFT                       0
#define I2S_RCR2_DIV(x)                          (((uint32_t)(((uint32_t)(x))<<I2S_RCR2_DIV_SHIFT))&I2S_RCR2_DIV_MASK)
#define I2S_RCR2_BCD_MASK                        0x1000000u
#define I2S_RCR2_BCD_SHIFT                       24
#define I2S_RCR2_BCP_MASK                        0x2000000u
#define I2S_RCR2_BCP_SHIFT                       25
#define I2S_RCR2_MSEL_MASK                       0xC000000u
#define I2S_RCR2_MSEL_SHIFT                      26
#define I2S_RCR2_MSEL(x)                         (((uint32_t)(((uint32_t)(x))<<I2S_RCR2_MSEL_SHIFT))&I2S_RCR2_MSEL_MASK)
#define I2S_RCR2_BCI_MASK                        0x10000000u
#define I2S_RCR2_BCI_SHIFT                       28
#define I2S_RCR2_BCS_MASK                        0x20000000u
#define I2S_RCR2_BCS_SHIFT                       29
#define I2S_RCR2_SYNC_MASK                       0xC0000000u
#define I2S_RCR2_SYNC_SHIFT                      30
#define I2S_RCR2_SYNC(x)                         (((uint32_t)(((uint32_t)(x))<<I2S_RCR2_SYNC_SHIFT))&I2S_RCR2_SYNC_MASK)
/* RCR3 Bit Fields */
#define I2S_RCR3_WDFL_MASK                       0x1Fu
#define I2S_RCR3_WDFL_SHIFT                      0
#define I2S_RCR3_WDFL(x)                         (((uint32_t)(((uint32_t)(x))<<I2S_RCR3_WDFL_SHIFT))&I2S_RCR3_WDFL_MASK)
#define I2S_RCR3_RCE_MASK                        0x30000u
#define I2S_RCR3_RCE_SHIFT                       16
#define I2S_RCR3_RCE(x)                          (((uint32_t)(((uint32_t)(x))<<I2S_RCR3_RCE_SHIFT))&I2S_RCR3_RCE_MASK)
/* RCR4 Bit Fields */
#define I2S_RCR4_FSD_MASK                        0x1u
#define I2S_RCR4_FSD_SHIFT                       0
#define I2S_RCR4_FSP_MASK                        0x2u
#define I2S_RCR4_FSP_SHIFT                       1
#define I2S_RCR4_FSE_MASK                        0x8u
#define I2S_RCR4_FSE_SHIFT                       3
#define I2S_RCR4_MF_MASK                         0x10u
#define I2S_RCR4_MF_SHIFT                        4
#define I2S_RCR4_SYWD_MASK                       0x1F00u
#define I2S_RCR4_SYWD_SHIFT                      8
#define I2S_RCR4_SYWD(x)                         (((uint32_t)(((uint32_t)(x))<<I2S_RCR4_SYWD_SHIFT))&I2S_RCR4_SYWD_MASK)
#define I2S_RCR4_FRSZ_MASK                       0x1F0000u
#define I2S_RCR4_FRSZ_SHIFT                      16
#define I2S_RCR4_FRSZ(x)                         (((uint32_t)(((uint32_t)(x))<<I2S_RCR4_FRSZ_SHIFT))&I2S_RCR4_FRSZ_MASK)
/* RCR5 Bit Fields */
#define I2S_RCR5_FBT_MASK                        0x1F00u
#define I2S_RCR5_FBT_SHIFT                       8
#define I2S_RCR5_FBT(x)                          (((uint32_t)(((uint32_t)(x))<<I2S_RCR5_FBT_SHIFT))&I2S_RCR5_FBT_MASK)
#define I2S_RCR5_W0W_MASK                        0x1F0000u
#define I2S_RCR5_W0W_SHIFT                       16
#define I2S_RCR5_W0W(x)                          (((uint32_t)(((uint32_t)(x))<<I2S_RCR5_W0W_SHIFT))&I2S_RCR5_W0W_MASK)
#define I2S_RCR5_WNW_MASK                        0x1F000000u
#define I2S_RCR5_WNW_SHIFT                       24
#define I2S_RCR5_WNW(x)                          (((uint32_t)(((uint32_t)(x))<<I2S_RCR5_WNW_SHIFT))&I2S_RCR5_WNW_MASK)
/* RDR Bit Fields */
#define I2S_RDR_RDR_MASK                         0xFFFFFFFFu
#define I2S_RDR_RDR_SHIFT                        0
#define I2S_RDR_RDR(x)                           (((uint32_t)(((uint32_t)(x))<<I2S_RDR_RDR_SHIFT))&I2S_RDR_RDR_MASK)
/* RFR Bit Fields */
#define I2S_RFR_RFP_MASK                         0xFu
#define I2S_RFR_RFP_SHIFT                        0
#define I2S_RFR_RFP(x)                           (((uint32_t)(((uint32_t)(x))<<I2S_RFR_RFP_SHIFT))&I2S_RFR_RFP_MASK)
#define I2S_RFR_WFP_MASK                         0xF0000u
#define I2S_RFR_WFP_SHIFT                        16
#define I2S_RFR_WFP(x)                           (((uint32_t)(((uint32_t)(x))<<I2S_RFR_WFP_SHIFT))&I2S_RFR_WFP_MASK)
/* RMR Bit Fields */
#define I2S_RMR_RWM_MASK                         0xFFFFFFFFu
#define I2S_RMR_RWM_SHIFT                        0
#define I2S_RMR_RWM(x)                           (((uint32_t)(((uint32_t)(x))<<I2S_RMR_RWM_SHIFT))&I2S_RMR_RWM_MASK)
/* MCR Bit Fields */
#define I2S_MCR_MICS_MASK                        0x3000000u
#define I2S_MCR_MICS_SHIFT                       24
#define I2S_MCR_MICS(x)                          (((uint32_t)(((uint32_t)(x))<<I2S_MCR_MICS_SHIFT))&I2S_MCR_MICS_MASK)
#define I2S_MCR_MOE_MASK                         0x40000000u
#define I2S_MCR_MOE_SHIFT                        30
#define I2S_MCR_DUF_MASK                         0x80000000u
#define I2S_MCR_DUF_SHIFT                        31
/* MDR Bit Fields */
#define I2S_MDR_DIVIDE_MASK                      0xFFFu
#define I2S_MDR_DIVIDE_SHIFT                     0
#define I2S_MDR_DIVIDE(x)                        (((uint32_t)(((uint32_t)(x))<<I2S_MDR_DIVIDE_SHIFT))&I2S_MDR_DIVIDE_MASK)
#define I2S_MDR_FRACT_MASK                       0xFF000u
#define I2S_MDR_FRACT_SHIFT                      12
#define I2S_MDR_FRACT(x)                         (((uint32_t)(((uint32_t)(x))<<I2S_MDR_FRACT_SHIFT))&I2S_MDR_FRACT_MASK)

/**
 * @}
 */ /* end of group I2S_Register_Masks */


/* I2S - Peripheral instance base addresses */
/** Peripheral I2S0 base address */
#define I2S0_BASE                                (0x4002F000u)
/** Peripheral I2S0 base pointer */
#define I2S0                                     ((I2S_Type *)I2S0_BASE)
/** Array initializer of I2S peripheral base pointers */
#define I2S_BASES                                { I2S0 }

/**
 * @}
 */ /* end of group I2S_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- LLWU Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup LLWU_Peripheral_Access_Layer LLWU Peripheral Access Layer
 * @{
 */

/** LLWU - Register Layout Typedef */
typedef struct {
  __IO uint8_t PE1;                                /**< LLWU Pin Enable 1 register, offset: 0x0 */
  __IO uint8_t PE2;                                /**< LLWU Pin Enable 2 register, offset: 0x1 */
  __IO uint8_t PE3;                                /**< LLWU Pin Enable 3 register, offset: 0x2 */
  __IO uint8_t PE4;                                /**< LLWU Pin Enable 4 register, offset: 0x3 */
  __IO uint8_t ME;                                 /**< LLWU Module Enable register, offset: 0x4 */
  __IO uint8_t F1;                                 /**< LLWU Flag 1 register, offset: 0x5 */
  __IO uint8_t F2;                                 /**< LLWU Flag 2 register, offset: 0x6 */
  __I  uint8_t F3;                                 /**< LLWU Flag 3 register, offset: 0x7 */
  __IO uint8_t FILT1;                              /**< LLWU Pin Filter 1 register, offset: 0x8 */
  __IO uint8_t FILT2;                              /**< LLWU Pin Filter 2 register, offset: 0x9 */
  __IO uint8_t RST;                                /**< LLWU Reset Enable register, offset: 0xA */
} LLWU_Type;

/* ----------------------------------------------------------------------------
   -- LLWU Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup LLWU_Register_Masks LLWU Register Masks
 * @{
 */

/* PE1 Bit Fields */
#define LLWU_PE1_WUPE0_MASK                      0x3u
#define LLWU_PE1_WUPE0_SHIFT                     0
#define LLWU_PE1_WUPE0(x)                        (((uint8_t)(((uint8_t)(x))<<LLWU_PE1_WUPE0_SHIFT))&LLWU_PE1_WUPE0_MASK)
#define LLWU_PE1_WUPE1_MASK                      0xCu
#define LLWU_PE1_WUPE1_SHIFT                     2
#define LLWU_PE1_WUPE1(x)                        (((uint8_t)(((uint8_t)(x))<<LLWU_PE1_WUPE1_SHIFT))&LLWU_PE1_WUPE1_MASK)
#define LLWU_PE1_WUPE2_MASK                      0x30u
#define LLWU_PE1_WUPE2_SHIFT                     4
#define LLWU_PE1_WUPE2(x)                        (((uint8_t)(((uint8_t)(x))<<LLWU_PE1_WUPE2_SHIFT))&LLWU_PE1_WUPE2_MASK)
#define LLWU_PE1_WUPE3_MASK                      0xC0u
#define LLWU_PE1_WUPE3_SHIFT                     6
#define LLWU_PE1_WUPE3(x)                        (((uint8_t)(((uint8_t)(x))<<LLWU_PE1_WUPE3_SHIFT))&LLWU_PE1_WUPE3_MASK)
/* PE2 Bit Fields */
#define LLWU_PE2_WUPE4_MASK                      0x3u
#define LLWU_PE2_WUPE4_SHIFT                     0
#define LLWU_PE2_WUPE4(x)                        (((uint8_t)(((uint8_t)(x))<<LLWU_PE2_WUPE4_SHIFT))&LLWU_PE2_WUPE4_MASK)
#define LLWU_PE2_WUPE5_MASK                      0xCu
#define LLWU_PE2_WUPE5_SHIFT                     2
#define LLWU_PE2_WUPE5(x)                        (((uint8_t)(((uint8_t)(x))<<LLWU_PE2_WUPE5_SHIFT))&LLWU_PE2_WUPE5_MASK)
#define LLWU_PE2_WUPE6_MASK                      0x30u
#define LLWU_PE2_WUPE6_SHIFT                     4
#define LLWU_PE2_WUPE6(x)                        (((uint8_t)(((uint8_t)(x))<<LLWU_PE2_WUPE6_SHIFT))&LLWU_PE2_WUPE6_MASK)
#define LLWU_PE2_WUPE7_MASK                      0xC0u
#define LLWU_PE2_WUPE7_SHIFT                     6
#define LLWU_PE2_WUPE7(x)                        (((uint8_t)(((uint8_t)(x))<<LLWU_PE2_WUPE7_SHIFT))&LLWU_PE2_WUPE7_MASK)
/* PE3 Bit Fields */
#define LLWU_PE3_WUPE8_MASK                      0x3u
#define LLWU_PE3_WUPE8_SHIFT                     0
#define LLWU_PE3_WUPE8(x)                        (((uint8_t)(((uint8_t)(x))<<LLWU_PE3_WUPE8_SHIFT))&LLWU_PE3_WUPE8_MASK)
#define LLWU_PE3_WUPE9_MASK                      0xCu
#define LLWU_PE3_WUPE9_SHIFT                     2
#define LLWU_PE3_WUPE9(x)                        (((uint8_t)(((uint8_t)(x))<<LLWU_PE3_WUPE9_SHIFT))&LLWU_PE3_WUPE9_MASK)
#define LLWU_PE3_WUPE10_MASK                     0x30u
#define LLWU_PE3_WUPE10_SHIFT                    4
#define LLWU_PE3_WUPE10(x)                       (((uint8_t)(((uint8_t)(x))<<LLWU_PE3_WUPE10_SHIFT))&LLWU_PE3_WUPE10_MASK)
#define LLWU_PE3_WUPE11_MASK                     0xC0u
#define LLWU_PE3_WUPE11_SHIFT                    6
#define LLWU_PE3_WUPE11(x)                       (((uint8_t)(((uint8_t)(x))<<LLWU_PE3_WUPE11_SHIFT))&LLWU_PE3_WUPE11_MASK)
/* PE4 Bit Fields */
#define LLWU_PE4_WUPE12_MASK                     0x3u
#define LLWU_PE4_WUPE12_SHIFT                    0
#define LLWU_PE4_WUPE12(x)                       (((uint8_t)(((uint8_t)(x))<<LLWU_PE4_WUPE12_SHIFT))&LLWU_PE4_WUPE12_MASK)
#define LLWU_PE4_WUPE13_MASK                     0xCu
#define LLWU_PE4_WUPE13_SHIFT                    2
#define LLWU_PE4_WUPE13(x)                       (((uint8_t)(((uint8_t)(x))<<LLWU_PE4_WUPE13_SHIFT))&LLWU_PE4_WUPE13_MASK)
#define LLWU_PE4_WUPE14_MASK                     0x30u
#define LLWU_PE4_WUPE14_SHIFT                    4
#define LLWU_PE4_WUPE14(x)                       (((uint8_t)(((uint8_t)(x))<<LLWU_PE4_WUPE14_SHIFT))&LLWU_PE4_WUPE14_MASK)
#define LLWU_PE4_WUPE15_MASK                     0xC0u
#define LLWU_PE4_WUPE15_SHIFT                    6
#define LLWU_PE4_WUPE15(x)                       (((uint8_t)(((uint8_t)(x))<<LLWU_PE4_WUPE15_SHIFT))&LLWU_PE4_WUPE15_MASK)
/* ME Bit Fields */
#define LLWU_ME_WUME0_MASK                       0x1u
#define LLWU_ME_WUME0_SHIFT                      0
#define LLWU_ME_WUME1_MASK                       0x2u
#define LLWU_ME_WUME1_SHIFT                      1
#define LLWU_ME_WUME2_MASK                       0x4u
#define LLWU_ME_WUME2_SHIFT                      2
#define LLWU_ME_WUME3_MASK                       0x8u
#define LLWU_ME_WUME3_SHIFT                      3
#define LLWU_ME_WUME4_MASK                       0x10u
#define LLWU_ME_WUME4_SHIFT                      4
#define LLWU_ME_WUME5_MASK                       0x20u
#define LLWU_ME_WUME5_SHIFT                      5
#define LLWU_ME_WUME6_MASK                       0x40u
#define LLWU_ME_WUME6_SHIFT                      6
#define LLWU_ME_WUME7_MASK                       0x80u
#define LLWU_ME_WUME7_SHIFT                      7
/* F1 Bit Fields */
#define LLWU_F1_WUF0_MASK                        0x1u
#define LLWU_F1_WUF0_SHIFT                       0
#define LLWU_F1_WUF1_MASK                        0x2u
#define LLWU_F1_WUF1_SHIFT                       1
#define LLWU_F1_WUF2_MASK                        0x4u
#define LLWU_F1_WUF2_SHIFT                       2
#define LLWU_F1_WUF3_MASK                        0x8u
#define LLWU_F1_WUF3_SHIFT                       3
#define LLWU_F1_WUF4_MASK                        0x10u
#define LLWU_F1_WUF4_SHIFT                       4
#define LLWU_F1_WUF5_MASK                        0x20u
#define LLWU_F1_WUF5_SHIFT                       5
#define LLWU_F1_WUF6_MASK                        0x40u
#define LLWU_F1_WUF6_SHIFT                       6
#define LLWU_F1_WUF7_MASK                        0x80u
#define LLWU_F1_WUF7_SHIFT                       7
/* F2 Bit Fields */
#define LLWU_F2_WUF8_MASK                        0x1u
#define LLWU_F2_WUF8_SHIFT                       0
#define LLWU_F2_WUF9_MASK                        0x2u
#define LLWU_F2_WUF9_SHIFT                       1
#define LLWU_F2_WUF10_MASK                       0x4u
#define LLWU_F2_WUF10_SHIFT                      2
#define LLWU_F2_WUF11_MASK                       0x8u
#define LLWU_F2_WUF11_SHIFT                      3
#define LLWU_F2_WUF12_MASK                       0x10u
#define LLWU_F2_WUF12_SHIFT                      4
#define LLWU_F2_WUF13_MASK                       0x20u
#define LLWU_F2_WUF13_SHIFT                      5
#define LLWU_F2_WUF14_MASK                       0x40u
#define LLWU_F2_WUF14_SHIFT                      6
#define LLWU_F2_WUF15_MASK                       0x80u
#define LLWU_F2_WUF15_SHIFT                      7
/* F3 Bit Fields */
#define LLWU_F3_MWUF0_MASK                       0x1u
#define LLWU_F3_MWUF0_SHIFT                      0
#define LLWU_F3_MWUF1_MASK                       0x2u
#define LLWU_F3_MWUF1_SHIFT                      1
#define LLWU_F3_MWUF2_MASK                       0x4u
#define LLWU_F3_MWUF2_SHIFT                      2
#define LLWU_F3_MWUF3_MASK                       0x8u
#define LLWU_F3_MWUF3_SHIFT                      3
#define LLWU_F3_MWUF4_MASK                       0x10u
#define LLWU_F3_MWUF4_SHIFT                      4
#define LLWU_F3_MWUF5_MASK                       0x20u
#define LLWU_F3_MWUF5_SHIFT                      5
#define LLWU_F3_MWUF6_MASK                       0x40u
#define LLWU_F3_MWUF6_SHIFT                      6
#define LLWU_F3_MWUF7_MASK                       0x80u
#define LLWU_F3_MWUF7_SHIFT                      7
/* FILT1 Bit Fields */
#define LLWU_FILT1_FILTSEL_MASK                  0xFu
#define LLWU_FILT1_FILTSEL_SHIFT                 0
#define LLWU_FILT1_FILTSEL(x)                    (((uint8_t)(((uint8_t)(x))<<LLWU_FILT1_FILTSEL_SHIFT))&LLWU_FILT1_FILTSEL_MASK)
#define LLWU_FILT1_FILTE_MASK                    0x60u
#define LLWU_FILT1_FILTE_SHIFT                   5
#define LLWU_FILT1_FILTE(x)                      (((uint8_t)(((uint8_t)(x))<<LLWU_FILT1_FILTE_SHIFT))&LLWU_FILT1_FILTE_MASK)
#define LLWU_FILT1_FILTF_MASK                    0x80u
#define LLWU_FILT1_FILTF_SHIFT                   7
/* FILT2 Bit Fields */
#define LLWU_FILT2_FILTSEL_MASK                  0xFu
#define LLWU_FILT2_FILTSEL_SHIFT                 0
#define LLWU_FILT2_FILTSEL(x)                    (((uint8_t)(((uint8_t)(x))<<LLWU_FILT2_FILTSEL_SHIFT))&LLWU_FILT2_FILTSEL_MASK)
#define LLWU_FILT2_FILTE_MASK                    0x60u
#define LLWU_FILT2_FILTE_SHIFT                   5
#define LLWU_FILT2_FILTE(x)                      (((uint8_t)(((uint8_t)(x))<<LLWU_FILT2_FILTE_SHIFT))&LLWU_FILT2_FILTE_MASK)
#define LLWU_FILT2_FILTF_MASK                    0x80u
#define LLWU_FILT2_FILTF_SHIFT                   7
/* RST Bit Fields */
#define LLWU_RST_RSTFILT_MASK                    0x1u
#define LLWU_RST_RSTFILT_SHIFT                   0
#define LLWU_RST_LLRSTE_MASK                     0x2u
#define LLWU_RST_LLRSTE_SHIFT                    1

/**
 * @}
 */ /* end of group LLWU_Register_Masks */


/* LLWU - Peripheral instance base addresses */
/** Peripheral LLWU base address */
#define LLWU_BASE                                (0x4007C000u)
/** Peripheral LLWU base pointer */
#define LLWU                                     ((LLWU_Type *)LLWU_BASE)
/** Array initializer of LLWU peripheral base pointers */
#define LLWU_BASES                               { LLWU }

/**
 * @}
 */ /* end of group LLWU_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- LPTMR Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup LPTMR_Peripheral_Access_Layer LPTMR Peripheral Access Layer
 * @{
 */

/** LPTMR - Register Layout Typedef */
typedef struct {
  __IO uint32_t CSR;                               /**< Low Power Timer Control Status Register, offset: 0x0 */
  __IO uint32_t PSR;                               /**< Low Power Timer Prescale Register, offset: 0x4 */
  __IO uint32_t CMR;                               /**< Low Power Timer Compare Register, offset: 0x8 */
  __I  uint32_t CNR;                               /**< Low Power Timer Counter Register, offset: 0xC */
} LPTMR_Type;

/* ----------------------------------------------------------------------------
   -- LPTMR Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup LPTMR_Register_Masks LPTMR Register Masks
 * @{
 */

/* CSR Bit Fields */
#define LPTMR_CSR_TEN_MASK                       0x1u
#define LPTMR_CSR_TEN_SHIFT                      0
#define LPTMR_CSR_TMS_MASK                       0x2u
#define LPTMR_CSR_TMS_SHIFT                      1
#define LPTMR_CSR_TFC_MASK                       0x4u
#define LPTMR_CSR_TFC_SHIFT                      2
#define LPTMR_CSR_TPP_MASK                       0x8u
#define LPTMR_CSR_TPP_SHIFT                      3
#define LPTMR_CSR_TPS_MASK                       0x30u
#define LPTMR_CSR_TPS_SHIFT                      4
#define LPTMR_CSR_TPS(x)                         (((uint32_t)(((uint32_t)(x))<<LPTMR_CSR_TPS_SHIFT))&LPTMR_CSR_TPS_MASK)
#define LPTMR_CSR_TIE_MASK                       0x40u
#define LPTMR_CSR_TIE_SHIFT                      6
#define LPTMR_CSR_TCF_MASK                       0x80u
#define LPTMR_CSR_TCF_SHIFT                      7
/* PSR Bit Fields */
#define LPTMR_PSR_PCS_MASK                       0x3u
#define LPTMR_PSR_PCS_SHIFT                      0
#define LPTMR_PSR_PCS(x)                         (((uint32_t)(((uint32_t)(x))<<LPTMR_PSR_PCS_SHIFT))&LPTMR_PSR_PCS_MASK)
#define LPTMR_PSR_PBYP_MASK                      0x4u
#define LPTMR_PSR_PBYP_SHIFT                     2
#define LPTMR_PSR_PRESCALE_MASK                  0x78u
#define LPTMR_PSR_PRESCALE_SHIFT                 3
#define LPTMR_PSR_PRESCALE(x)                    (((uint32_t)(((uint32_t)(x))<<LPTMR_PSR_PRESCALE_SHIFT))&LPTMR_PSR_PRESCALE_MASK)
/* CMR Bit Fields */
#define LPTMR_CMR_COMPARE_MASK                   0xFFFFu
#define LPTMR_CMR_COMPARE_SHIFT                  0
#define LPTMR_CMR_COMPARE(x)                     (((uint32_t)(((uint32_t)(x))<<LPTMR_CMR_COMPARE_SHIFT))&LPTMR_CMR_COMPARE_MASK)
/* CNR Bit Fields */
#define LPTMR_CNR_COUNTER_MASK                   0xFFFFu
#define LPTMR_CNR_COUNTER_SHIFT                  0
#define LPTMR_CNR_COUNTER(x)                     (((uint32_t)(((uint32_t)(x))<<LPTMR_CNR_COUNTER_SHIFT))&LPTMR_CNR_COUNTER_MASK)

/**
 * @}
 */ /* end of group LPTMR_Register_Masks */


/* LPTMR - Peripheral instance base addresses */
/** Peripheral LPTMR0 base address */
#define LPTMR0_BASE                              (0x40040000u)
/** Peripheral LPTMR0 base pointer */
#define LPTMR0                                   ((LPTMR_Type *)LPTMR0_BASE)
/** Array initializer of LPTMR peripheral base pointers */
#define LPTMR_BASES                              { LPTMR0 }

/**
 * @}
 */ /* end of group LPTMR_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- MCG Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup MCG_Peripheral_Access_Layer MCG Peripheral Access Layer
 * @{
 */

/** MCG - Register Layout Typedef */
typedef struct {
  __IO uint8_t C1;                                 /**< MCG Control 1 Register, offset: 0x0 */
  __IO uint8_t C2;                                 /**< MCG Control 2 Register, offset: 0x1 */
  __IO uint8_t C3;                                 /**< MCG Control 3 Register, offset: 0x2 */
  __IO uint8_t C4;                                 /**< MCG Control 4 Register, offset: 0x3 */
  __IO uint8_t C5;                                 /**< MCG Control 5 Register, offset: 0x4 */
  __IO uint8_t C6;                                 /**< MCG Control 6 Register, offset: 0x5 */
  __I  uint8_t S;                                  /**< MCG Status Register, offset: 0x6 */
       uint8_t RESERVED_0[1];
  __IO uint8_t SC;                                 /**< MCG Status and Control Register, offset: 0x8 */
       uint8_t RESERVED_1[1];
  __IO uint8_t ATCVH;                              /**< MCG Auto Trim Compare Value High Register, offset: 0xA */
  __IO uint8_t ATCVL;                              /**< MCG Auto Trim Compare Value Low Register, offset: 0xB */
  __IO uint8_t C7;                                 /**< MCG Control 7 Register, offset: 0xC */
  __IO uint8_t C8;                                 /**< MCG Control 8 Register, offset: 0xD */
  __I  uint8_t C9;                                 /**< MCG Control 9 Register, offset: 0xE */
  __I  uint8_t C10;                                /**< MCG Control 10 Register, offset: 0xF */
} MCG_Type;

/* ----------------------------------------------------------------------------
   -- MCG Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup MCG_Register_Masks MCG Register Masks
 * @{
 */

/* C1 Bit Fields */
#define MCG_C1_IREFSTEN_MASK                     0x1u
#define MCG_C1_IREFSTEN_SHIFT                    0
#define MCG_C1_IRCLKEN_MASK                      0x2u
#define MCG_C1_IRCLKEN_SHIFT                     1
#define MCG_C1_IREFS_MASK                        0x4u
#define MCG_C1_IREFS_SHIFT                       2
#define MCG_C1_FRDIV_MASK                        0x38u
#define MCG_C1_FRDIV_SHIFT                       3
#define MCG_C1_FRDIV(x)                          (((uint8_t)(((uint8_t)(x))<<MCG_C1_FRDIV_SHIFT))&MCG_C1_FRDIV_MASK)
#define MCG_C1_CLKS_MASK                         0xC0u
#define MCG_C1_CLKS_SHIFT                        6
#define MCG_C1_CLKS(x)                           (((uint8_t)(((uint8_t)(x))<<MCG_C1_CLKS_SHIFT))&MCG_C1_CLKS_MASK)
/* C2 Bit Fields */
#define MCG_C2_IRCS_MASK                         0x1u
#define MCG_C2_IRCS_SHIFT                        0
#define MCG_C2_LP_MASK                           0x2u
#define MCG_C2_LP_SHIFT                          1
#define MCG_C2_EREFS0_MASK                       0x4u
#define MCG_C2_EREFS0_SHIFT                      2
#define MCG_C2_HGO0_MASK                         0x8u
#define MCG_C2_HGO0_SHIFT                        3
#define MCG_C2_RANGE0_MASK                       0x30u
#define MCG_C2_RANGE0_SHIFT                      4
#define MCG_C2_RANGE0(x)                         (((uint8_t)(((uint8_t)(x))<<MCG_C2_RANGE0_SHIFT))&MCG_C2_RANGE0_MASK)
#define MCG_C2_LOCRE0_MASK                       0x80u
#define MCG_C2_LOCRE0_SHIFT                      7
/* C3 Bit Fields */
#define MCG_C3_SCTRIM_MASK                       0xFFu
#define MCG_C3_SCTRIM_SHIFT                      0
#define MCG_C3_SCTRIM(x)                         (((uint8_t)(((uint8_t)(x))<<MCG_C3_SCTRIM_SHIFT))&MCG_C3_SCTRIM_MASK)
/* C4 Bit Fields */
#define MCG_C4_SCFTRIM_MASK                      0x1u
#define MCG_C4_SCFTRIM_SHIFT                     0
#define MCG_C4_FCTRIM_MASK                       0x1Eu
#define MCG_C4_FCTRIM_SHIFT                      1
#define MCG_C4_FCTRIM(x)                         (((uint8_t)(((uint8_t)(x))<<MCG_C4_FCTRIM_SHIFT))&MCG_C4_FCTRIM_MASK)
#define MCG_C4_DRST_DRS_MASK                     0x60u
#define MCG_C4_DRST_DRS_SHIFT                    5
#define MCG_C4_DRST_DRS(x)                       (((uint8_t)(((uint8_t)(x))<<MCG_C4_DRST_DRS_SHIFT))&MCG_C4_DRST_DRS_MASK)
#define MCG_C4_DMX32_MASK                        0x80u
#define MCG_C4_DMX32_SHIFT                       7
/* C5 Bit Fields */
#define MCG_C5_PRDIV0_MASK                       0x1Fu
#define MCG_C5_PRDIV0_SHIFT                      0
#define MCG_C5_PRDIV0(x)                         (((uint8_t)(((uint8_t)(x))<<MCG_C5_PRDIV0_SHIFT))&MCG_C5_PRDIV0_MASK)
#define MCG_C5_PLLSTEN0_MASK                     0x20u
#define MCG_C5_PLLSTEN0_SHIFT                    5
#define MCG_C5_PLLCLKEN0_MASK                    0x40u
#define MCG_C5_PLLCLKEN0_SHIFT                   6
/* C6 Bit Fields */
#define MCG_C6_VDIV0_MASK                        0x1Fu
#define MCG_C6_VDIV0_SHIFT                       0
#define MCG_C6_VDIV0(x)                          (((uint8_t)(((uint8_t)(x))<<MCG_C6_VDIV0_SHIFT))&MCG_C6_VDIV0_MASK)
#define MCG_C6_CME0_MASK                         0x20u
#define MCG_C6_CME0_SHIFT                        5
#define MCG_C6_PLLS_MASK                         0x40u
#define MCG_C6_PLLS_SHIFT                        6
#define MCG_C6_LOLIE0_MASK                       0x80u
#define MCG_C6_LOLIE0_SHIFT                      7
/* S Bit Fields */
#define MCG_S_IRCST_MASK                         0x1u
#define MCG_S_IRCST_SHIFT                        0
#define MCG_S_OSCINIT0_MASK                      0x2u
#define MCG_S_OSCINIT0_SHIFT                     1
#define MCG_S_CLKST_MASK                         0xCu
#define MCG_S_CLKST_SHIFT                        2
#define MCG_S_CLKST(x)                           (((uint8_t)(((uint8_t)(x))<<MCG_S_CLKST_SHIFT))&MCG_S_CLKST_MASK)
#define MCG_S_IREFST_MASK                        0x10u
#define MCG_S_IREFST_SHIFT                       4
#define MCG_S_PLLST_MASK                         0x20u
#define MCG_S_PLLST_SHIFT                        5
#define MCG_S_LOCK0_MASK                         0x40u
#define MCG_S_LOCK0_SHIFT                        6
#define MCG_S_LOLS_MASK                          0x80u
#define MCG_S_LOLS_SHIFT                         7
/* SC Bit Fields */
#define MCG_SC_LOCS0_MASK                        0x1u
#define MCG_SC_LOCS0_SHIFT                       0
#define MCG_SC_FCRDIV_MASK                       0xEu
#define MCG_SC_FCRDIV_SHIFT                      1
#define MCG_SC_FCRDIV(x)                         (((uint8_t)(((uint8_t)(x))<<MCG_SC_FCRDIV_SHIFT))&MCG_SC_FCRDIV_MASK)
#define MCG_SC_FLTPRSRV_MASK                     0x10u
#define MCG_SC_FLTPRSRV_SHIFT                    4
#define MCG_SC_ATMF_MASK                         0x20u
#define MCG_SC_ATMF_SHIFT                        5
#define MCG_SC_ATMS_MASK                         0x40u
#define MCG_SC_ATMS_SHIFT                        6
#define MCG_SC_ATME_MASK                         0x80u
#define MCG_SC_ATME_SHIFT                        7
/* ATCVH Bit Fields */
#define MCG_ATCVH_ATCVH_MASK                     0xFFu
#define MCG_ATCVH_ATCVH_SHIFT                    0
#define MCG_ATCVH_ATCVH(x)                       (((uint8_t)(((uint8_t)(x))<<MCG_ATCVH_ATCVH_SHIFT))&MCG_ATCVH_ATCVH_MASK)
/* ATCVL Bit Fields */
#define MCG_ATCVL_ATCVL_MASK                     0xFFu
#define MCG_ATCVL_ATCVL_SHIFT                    0
#define MCG_ATCVL_ATCVL(x)                       (((uint8_t)(((uint8_t)(x))<<MCG_ATCVL_ATCVL_SHIFT))&MCG_ATCVL_ATCVL_MASK)
/* C7 Bit Fields */
#define MCG_C7_OSCSEL_MASK                       0x1u
#define MCG_C7_OSCSEL_SHIFT                      0
/* C8 Bit Fields */
#define MCG_C8_LOCS1_MASK                        0x1u
#define MCG_C8_LOCS1_SHIFT                       0
#define MCG_C8_CME1_MASK                         0x20u
#define MCG_C8_CME1_SHIFT                        5
#define MCG_C8_LOLRE_MASK                        0x40u
#define MCG_C8_LOLRE_SHIFT                       6
#define MCG_C8_LOCRE1_MASK                       0x80u
#define MCG_C8_LOCRE1_SHIFT                      7

/**
 * @}
 */ /* end of group MCG_Register_Masks */


/* MCG - Peripheral instance base addresses */
/** Peripheral MCG base address */
#define MCG_BASE                                 (0x40064000u)
/** Peripheral MCG base pointer */
#define MCG                                      ((MCG_Type *)MCG_BASE)
/** Array initializer of MCG peripheral base pointers */
#define MCG_BASES                                { MCG }

/**
 * @}
 */ /* end of group MCG_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- MCM Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup MCM_Peripheral_Access_Layer MCM Peripheral Access Layer
 * @{
 */

/** MCM - Register Layout Typedef */
typedef struct {
       uint8_t RESERVED_0[8];
  __I  uint16_t PLASC;                             /**< Crossbar Switch (AXBS) Slave Configuration, offset: 0x8 */
  __I  uint16_t PLAMC;                             /**< Crossbar Switch (AXBS) Master Configuration, offset: 0xA */
  __IO uint32_t CR;                                /**< Control Register, offset: 0xC */
  __IO uint32_t ISR;                               /**< Interrupt Status Register, offset: 0x10 */
  __IO uint32_t ETBCC;                             /**< ETB Counter Control register, offset: 0x14 */
  __IO uint32_t ETBRL;                             /**< ETB Reload register, offset: 0x18 */
  __I  uint32_t ETBCNT;                            /**< ETB Counter Value register, offset: 0x1C */
       uint8_t RESERVED_1[16];
  __IO uint32_t PID;                               /**< Process ID register, offset: 0x30 */
} MCM_Type;

/* ----------------------------------------------------------------------------
   -- MCM Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup MCM_Register_Masks MCM Register Masks
 * @{
 */

/* PLASC Bit Fields */
#define MCM_PLASC_ASC_MASK                       0xFFu
#define MCM_PLASC_ASC_SHIFT                      0
#define MCM_PLASC_ASC(x)                         (((uint16_t)(((uint16_t)(x))<<MCM_PLASC_ASC_SHIFT))&MCM_PLASC_ASC_MASK)
/* PLAMC Bit Fields */
#define MCM_PLAMC_AMC_MASK                       0xFFu
#define MCM_PLAMC_AMC_SHIFT                      0
#define MCM_PLAMC_AMC(x)                         (((uint16_t)(((uint16_t)(x))<<MCM_PLAMC_AMC_SHIFT))&MCM_PLAMC_AMC_MASK)
/* CR Bit Fields */
#define MCM_CR_SRAMUAP_MASK                      0x3000000u
#define MCM_CR_SRAMUAP_SHIFT                     24
#define MCM_CR_SRAMUAP(x)                        (((uint32_t)(((uint32_t)(x))<<MCM_CR_SRAMUAP_SHIFT))&MCM_CR_SRAMUAP_MASK)
#define MCM_CR_SRAMUWP_MASK                      0x4000000u
#define MCM_CR_SRAMUWP_SHIFT                     26
#define MCM_CR_SRAMLAP_MASK                      0x30000000u
#define MCM_CR_SRAMLAP_SHIFT                     28
#define MCM_CR_SRAMLAP(x)                        (((uint32_t)(((uint32_t)(x))<<MCM_CR_SRAMLAP_SHIFT))&MCM_CR_SRAMLAP_MASK)
#define MCM_CR_SRAMLWP_MASK                      0x40000000u
#define MCM_CR_SRAMLWP_SHIFT                     30
/* ISR Bit Fields */
#define MCM_ISR_IRQ_MASK                         0x2u
#define MCM_ISR_IRQ_SHIFT                        1
#define MCM_ISR_NMI_MASK                         0x4u
#define MCM_ISR_NMI_SHIFT                        2
#define MCM_ISR_DHREQ_MASK                       0x8u
#define MCM_ISR_DHREQ_SHIFT                      3
/* ETBCC Bit Fields */
#define MCM_ETBCC_CNTEN_MASK                     0x1u
#define MCM_ETBCC_CNTEN_SHIFT                    0
#define MCM_ETBCC_RSPT_MASK                      0x6u
#define MCM_ETBCC_RSPT_SHIFT                     1
#define MCM_ETBCC_RSPT(x)                        (((uint32_t)(((uint32_t)(x))<<MCM_ETBCC_RSPT_SHIFT))&MCM_ETBCC_RSPT_MASK)
#define MCM_ETBCC_RLRQ_MASK                      0x8u
#define MCM_ETBCC_RLRQ_SHIFT                     3
#define MCM_ETBCC_ETDIS_MASK                     0x10u
#define MCM_ETBCC_ETDIS_SHIFT                    4
#define MCM_ETBCC_ITDIS_MASK                     0x20u
#define MCM_ETBCC_ITDIS_SHIFT                    5
/* ETBRL Bit Fields */
#define MCM_ETBRL_RELOAD_MASK                    0x7FFu
#define MCM_ETBRL_RELOAD_SHIFT                   0
#define MCM_ETBRL_RELOAD(x)                      (((uint32_t)(((uint32_t)(x))<<MCM_ETBRL_RELOAD_SHIFT))&MCM_ETBRL_RELOAD_MASK)
/* ETBCNT Bit Fields */
#define MCM_ETBCNT_COUNTER_MASK                  0x7FFu
#define MCM_ETBCNT_COUNTER_SHIFT                 0
#define MCM_ETBCNT_COUNTER(x)                    (((uint32_t)(((uint32_t)(x))<<MCM_ETBCNT_COUNTER_SHIFT))&MCM_ETBCNT_COUNTER_MASK)
/* PID Bit Fields */
#define MCM_PID_PID_MASK                         0xFFu
#define MCM_PID_PID_SHIFT                        0
#define MCM_PID_PID(x)                           (((uint32_t)(((uint32_t)(x))<<MCM_PID_PID_SHIFT))&MCM_PID_PID_MASK)

/**
 * @}
 */ /* end of group MCM_Register_Masks */


/* MCM - Peripheral instance base addresses */
/** Peripheral MCM base address */
#define MCM_BASE                                 (0xE0080000u)
/** Peripheral MCM base pointer */
#define MCM                                      ((MCM_Type *)MCM_BASE)
/** Array initializer of MCM peripheral base pointers */
#define MCM_BASES                                { MCM }

/**
 * @}
 */ /* end of group MCM_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- MPU Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup MPU_Peripheral_Access_Layer MPU Peripheral Access Layer
 * @{
 */

/** MPU - Register Layout Typedef */
typedef struct {
  __IO uint32_t CESR;                              /**< Control/Error Status Register, offset: 0x0 */
       uint8_t RESERVED_0[12];
  struct {                                         /* offset: 0x10, array step: 0x8 */
    __I  uint32_t EAR;                               /**< Error Address Register, slave port n, array offset: 0x10, array step: 0x8 */
    __I  uint32_t EDR;                               /**< Error Detail Register, slave port n, array offset: 0x14, array step: 0x8 */
  } SP[5];
       uint8_t RESERVED_1[968];
  __IO uint32_t WORD[12][4];                       /**< Region Descriptor n, Word 0..Region Descriptor n, Word 3, array offset: 0x400, array step: index*0x10, index2*0x4 */
       uint8_t RESERVED_2[832];
  __IO uint32_t RGDAAC[12];                        /**< Region Descriptor Alternate Access Control n, array offset: 0x800, array step: 0x4 */
} MPU_Type;

/* ----------------------------------------------------------------------------
   -- MPU Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup MPU_Register_Masks MPU Register Masks
 * @{
 */

/* CESR Bit Fields */
#define MPU_CESR_VLD_MASK                        0x1u
#define MPU_CESR_VLD_SHIFT                       0
#define MPU_CESR_NRGD_MASK                       0xF00u
#define MPU_CESR_NRGD_SHIFT                      8
#define MPU_CESR_NRGD(x)                         (((uint32_t)(((uint32_t)(x))<<MPU_CESR_NRGD_SHIFT))&MPU_CESR_NRGD_MASK)
#define MPU_CESR_NSP_MASK                        0xF000u
#define MPU_CESR_NSP_SHIFT                       12
#define MPU_CESR_NSP(x)                          (((uint32_t)(((uint32_t)(x))<<MPU_CESR_NSP_SHIFT))&MPU_CESR_NSP_MASK)
#define MPU_CESR_HRL_MASK                        0xF0000u
#define MPU_CESR_HRL_SHIFT                       16
#define MPU_CESR_HRL(x)                          (((uint32_t)(((uint32_t)(x))<<MPU_CESR_HRL_SHIFT))&MPU_CESR_HRL_MASK)
#define MPU_CESR_SPERR_MASK                      0xF8000000u
#define MPU_CESR_SPERR_SHIFT                     27
#define MPU_CESR_SPERR(x)                        (((uint32_t)(((uint32_t)(x))<<MPU_CESR_SPERR_SHIFT))&MPU_CESR_SPERR_MASK)
/* EAR Bit Fields */
#define MPU_EAR_EADDR_MASK                       0xFFFFFFFFu
#define MPU_EAR_EADDR_SHIFT                      0
#define MPU_EAR_EADDR(x)                         (((uint32_t)(((uint32_t)(x))<<MPU_EAR_EADDR_SHIFT))&MPU_EAR_EADDR_MASK)
/* EDR Bit Fields */
#define MPU_EDR_ERW_MASK                         0x1u
#define MPU_EDR_ERW_SHIFT                        0
#define MPU_EDR_EATTR_MASK                       0xEu
#define MPU_EDR_EATTR_SHIFT                      1
#define MPU_EDR_EATTR(x)                         (((uint32_t)(((uint32_t)(x))<<MPU_EDR_EATTR_SHIFT))&MPU_EDR_EATTR_MASK)
#define MPU_EDR_EMN_MASK                         0xF0u
#define MPU_EDR_EMN_SHIFT                        4
#define MPU_EDR_EMN(x)                           (((uint32_t)(((uint32_t)(x))<<MPU_EDR_EMN_SHIFT))&MPU_EDR_EMN_MASK)
#define MPU_EDR_EACD_MASK                        0xFFFF0000u
#define MPU_EDR_EACD_SHIFT                       16
#define MPU_EDR_EACD(x)                          (((uint32_t)(((uint32_t)(x))<<MPU_EDR_EACD_SHIFT))&MPU_EDR_EACD_MASK)
/* WORD Bit Fields */
#define MPU_WORD_M0UM_MASK                       0x7u
#define MPU_WORD_M0UM_SHIFT                      0
#define MPU_WORD_M0UM(x)                         (((uint32_t)(((uint32_t)(x))<<MPU_WORD_M0UM_SHIFT))&MPU_WORD_M0UM_MASK)
#define MPU_WORD_VLD_MASK                        0x1u
#define MPU_WORD_VLD_SHIFT                       0
#define MPU_WORD_M0SM_MASK                       0x18u
#define MPU_WORD_M0SM_SHIFT                      3
#define MPU_WORD_M0SM(x)                         (((uint32_t)(((uint32_t)(x))<<MPU_WORD_M0SM_SHIFT))&MPU_WORD_M0SM_MASK)
#define MPU_WORD_ENDADDR_MASK                    0xFFFFFFE0u
#define MPU_WORD_ENDADDR_SHIFT                   5
#define MPU_WORD_ENDADDR(x)                      (((uint32_t)(((uint32_t)(x))<<MPU_WORD_ENDADDR_SHIFT))&MPU_WORD_ENDADDR_MASK)
#define MPU_WORD_SRTADDR_MASK                    0xFFFFFFE0u
#define MPU_WORD_SRTADDR_SHIFT                   5
#define MPU_WORD_SRTADDR(x)                      (((uint32_t)(((uint32_t)(x))<<MPU_WORD_SRTADDR_SHIFT))&MPU_WORD_SRTADDR_MASK)
#define MPU_WORD_M1UM_MASK                       0x1C0u
#define MPU_WORD_M1UM_SHIFT                      6
#define MPU_WORD_M1UM(x)                         (((uint32_t)(((uint32_t)(x))<<MPU_WORD_M1UM_SHIFT))&MPU_WORD_M1UM_MASK)
#define MPU_WORD_M1SM_MASK                       0x600u
#define MPU_WORD_M1SM_SHIFT                      9
#define MPU_WORD_M1SM(x)                         (((uint32_t)(((uint32_t)(x))<<MPU_WORD_M1SM_SHIFT))&MPU_WORD_M1SM_MASK)
#define MPU_WORD_M2UM_MASK                       0x7000u
#define MPU_WORD_M2UM_SHIFT                      12
#define MPU_WORD_M2UM(x)                         (((uint32_t)(((uint32_t)(x))<<MPU_WORD_M2UM_SHIFT))&MPU_WORD_M2UM_MASK)
#define MPU_WORD_M2SM_MASK                       0x18000u
#define MPU_WORD_M2SM_SHIFT                      15
#define MPU_WORD_M2SM(x)                         (((uint32_t)(((uint32_t)(x))<<MPU_WORD_M2SM_SHIFT))&MPU_WORD_M2SM_MASK)
#define MPU_WORD_M3UM_MASK                       0x1C0000u
#define MPU_WORD_M3UM_SHIFT                      18
#define MPU_WORD_M3UM(x)                         (((uint32_t)(((uint32_t)(x))<<MPU_WORD_M3UM_SHIFT))&MPU_WORD_M3UM_MASK)
#define MPU_WORD_M3SM_MASK                       0x600000u
#define MPU_WORD_M3SM_SHIFT                      21
#define MPU_WORD_M3SM(x)                         (((uint32_t)(((uint32_t)(x))<<MPU_WORD_M3SM_SHIFT))&MPU_WORD_M3SM_MASK)
#define MPU_WORD_M4WE_MASK                       0x1000000u
#define MPU_WORD_M4WE_SHIFT                      24
#define MPU_WORD_M4RE_MASK                       0x2000000u
#define MPU_WORD_M4RE_SHIFT                      25
#define MPU_WORD_M5WE_MASK                       0x4000000u
#define MPU_WORD_M5WE_SHIFT                      26
#define MPU_WORD_M5RE_MASK                       0x8000000u
#define MPU_WORD_M5RE_SHIFT                      27
#define MPU_WORD_M6WE_MASK                       0x10000000u
#define MPU_WORD_M6WE_SHIFT                      28
#define MPU_WORD_M6RE_MASK                       0x20000000u
#define MPU_WORD_M6RE_SHIFT                      29
#define MPU_WORD_M7WE_MASK                       0x40000000u
#define MPU_WORD_M7WE_SHIFT                      30
#define MPU_WORD_M7RE_MASK                       0x80000000u
#define MPU_WORD_M7RE_SHIFT                      31
/* RGDAAC Bit Fields */
#define MPU_RGDAAC_M0UM_MASK                     0x7u
#define MPU_RGDAAC_M0UM_SHIFT                    0
#define MPU_RGDAAC_M0UM(x)                       (((uint32_t)(((uint32_t)(x))<<MPU_RGDAAC_M0UM_SHIFT))&MPU_RGDAAC_M0UM_MASK)
#define MPU_RGDAAC_M0SM_MASK                     0x18u
#define MPU_RGDAAC_M0SM_SHIFT                    3
#define MPU_RGDAAC_M0SM(x)                       (((uint32_t)(((uint32_t)(x))<<MPU_RGDAAC_M0SM_SHIFT))&MPU_RGDAAC_M0SM_MASK)
#define MPU_RGDAAC_M1UM_MASK                     0x1C0u
#define MPU_RGDAAC_M1UM_SHIFT                    6
#define MPU_RGDAAC_M1UM(x)                       (((uint32_t)(((uint32_t)(x))<<MPU_RGDAAC_M1UM_SHIFT))&MPU_RGDAAC_M1UM_MASK)
#define MPU_RGDAAC_M1SM_MASK                     0x600u
#define MPU_RGDAAC_M1SM_SHIFT                    9
#define MPU_RGDAAC_M1SM(x)                       (((uint32_t)(((uint32_t)(x))<<MPU_RGDAAC_M1SM_SHIFT))&MPU_RGDAAC_M1SM_MASK)
#define MPU_RGDAAC_M2UM_MASK                     0x7000u
#define MPU_RGDAAC_M2UM_SHIFT                    12
#define MPU_RGDAAC_M2UM(x)                       (((uint32_t)(((uint32_t)(x))<<MPU_RGDAAC_M2UM_SHIFT))&MPU_RGDAAC_M2UM_MASK)
#define MPU_RGDAAC_M2SM_MASK                     0x18000u
#define MPU_RGDAAC_M2SM_SHIFT                    15
#define MPU_RGDAAC_M2SM(x)                       (((uint32_t)(((uint32_t)(x))<<MPU_RGDAAC_M2SM_SHIFT))&MPU_RGDAAC_M2SM_MASK)
#define MPU_RGDAAC_M3UM_MASK                     0x1C0000u
#define MPU_RGDAAC_M3UM_SHIFT                    18
#define MPU_RGDAAC_M3UM(x)                       (((uint32_t)(((uint32_t)(x))<<MPU_RGDAAC_M3UM_SHIFT))&MPU_RGDAAC_M3UM_MASK)
#define MPU_RGDAAC_M3SM_MASK                     0x600000u
#define MPU_RGDAAC_M3SM_SHIFT                    21
#define MPU_RGDAAC_M3SM(x)                       (((uint32_t)(((uint32_t)(x))<<MPU_RGDAAC_M3SM_SHIFT))&MPU_RGDAAC_M3SM_MASK)
#define MPU_RGDAAC_M4WE_MASK                     0x1000000u
#define MPU_RGDAAC_M4WE_SHIFT                    24
#define MPU_RGDAAC_M4RE_MASK                     0x2000000u
#define MPU_RGDAAC_M4RE_SHIFT                    25
#define MPU_RGDAAC_M5WE_MASK                     0x4000000u
#define MPU_RGDAAC_M5WE_SHIFT                    26
#define MPU_RGDAAC_M5RE_MASK                     0x8000000u
#define MPU_RGDAAC_M5RE_SHIFT                    27
#define MPU_RGDAAC_M6WE_MASK                     0x10000000u
#define MPU_RGDAAC_M6WE_SHIFT                    28
#define MPU_RGDAAC_M6RE_MASK                     0x20000000u
#define MPU_RGDAAC_M6RE_SHIFT                    29
#define MPU_RGDAAC_M7WE_MASK                     0x40000000u
#define MPU_RGDAAC_M7WE_SHIFT                    30
#define MPU_RGDAAC_M7RE_MASK                     0x80000000u
#define MPU_RGDAAC_M7RE_SHIFT                    31

/**
 * @}
 */ /* end of group MPU_Register_Masks */


/* MPU - Peripheral instance base addresses */
/** Peripheral MPU base address */
#define MPU_BASE                                 (0x4000D000u)
/** Peripheral MPU base pointer */
#define MPU                                      ((MPU_Type *)MPU_BASE)
/** Array initializer of MPU peripheral base pointers */
#define MPU_BASES                                { MPU }

/**
 * @}
 */ /* end of group MPU_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- NV Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup NV_Peripheral_Access_Layer NV Peripheral Access Layer
 * @{
 */

/** NV - Register Layout Typedef */
typedef struct {
  __I  uint8_t BACKKEY3;                           /**< Backdoor Comparison Key 3., offset: 0x0 */
  __I  uint8_t BACKKEY2;                           /**< Backdoor Comparison Key 2., offset: 0x1 */
  __I  uint8_t BACKKEY1;                           /**< Backdoor Comparison Key 1., offset: 0x2 */
  __I  uint8_t BACKKEY0;                           /**< Backdoor Comparison Key 0., offset: 0x3 */
  __I  uint8_t BACKKEY7;                           /**< Backdoor Comparison Key 7., offset: 0x4 */
  __I  uint8_t BACKKEY6;                           /**< Backdoor Comparison Key 6., offset: 0x5 */
  __I  uint8_t BACKKEY5;                           /**< Backdoor Comparison Key 5., offset: 0x6 */
  __I  uint8_t BACKKEY4;                           /**< Backdoor Comparison Key 4., offset: 0x7 */
  __I  uint8_t FPROT3;                             /**< Non-volatile P-Flash Protection 1 - Low Register, offset: 0x8 */
  __I  uint8_t FPROT2;                             /**< Non-volatile P-Flash Protection 1 - High Register, offset: 0x9 */
  __I  uint8_t FPROT1;                             /**< Non-volatile P-Flash Protection 0 - Low Register, offset: 0xA */
  __I  uint8_t FPROT0;                             /**< Non-volatile P-Flash Protection 0 - High Register, offset: 0xB */
  __I  uint8_t FSEC;                               /**< Non-volatile Flash Security Register, offset: 0xC */
  __I  uint8_t FOPT;                               /**< Non-volatile Flash Option Register, offset: 0xD */
  __I  uint8_t FEPROT;                             /**< Non-volatile EERAM Protection Register, offset: 0xE */
  __I  uint8_t FDPROT;                             /**< Non-volatile D-Flash Protection Register, offset: 0xF */
} NV_Type;

/* ----------------------------------------------------------------------------
   -- NV Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup NV_Register_Masks NV Register Masks
 * @{
 */

/* BACKKEY3 Bit Fields */
#define NV_BACKKEY3_KEY_MASK                     0xFFu
#define NV_BACKKEY3_KEY_SHIFT                    0
#define NV_BACKKEY3_KEY(x)                       (((uint8_t)(((uint8_t)(x))<<NV_BACKKEY3_KEY_SHIFT))&NV_BACKKEY3_KEY_MASK)
/* BACKKEY2 Bit Fields */
#define NV_BACKKEY2_KEY_MASK                     0xFFu
#define NV_BACKKEY2_KEY_SHIFT                    0
#define NV_BACKKEY2_KEY(x)                       (((uint8_t)(((uint8_t)(x))<<NV_BACKKEY2_KEY_SHIFT))&NV_BACKKEY2_KEY_MASK)
/* BACKKEY1 Bit Fields */
#define NV_BACKKEY1_KEY_MASK                     0xFFu
#define NV_BACKKEY1_KEY_SHIFT                    0
#define NV_BACKKEY1_KEY(x)                       (((uint8_t)(((uint8_t)(x))<<NV_BACKKEY1_KEY_SHIFT))&NV_BACKKEY1_KEY_MASK)
/* BACKKEY0 Bit Fields */
#define NV_BACKKEY0_KEY_MASK                     0xFFu
#define NV_BACKKEY0_KEY_SHIFT                    0
#define NV_BACKKEY0_KEY(x)                       (((uint8_t)(((uint8_t)(x))<<NV_BACKKEY0_KEY_SHIFT))&NV_BACKKEY0_KEY_MASK)
/* BACKKEY7 Bit Fields */
#define NV_BACKKEY7_KEY_MASK                     0xFFu
#define NV_BACKKEY7_KEY_SHIFT                    0
#define NV_BACKKEY7_KEY(x)                       (((uint8_t)(((uint8_t)(x))<<NV_BACKKEY7_KEY_SHIFT))&NV_BACKKEY7_KEY_MASK)
/* BACKKEY6 Bit Fields */
#define NV_BACKKEY6_KEY_MASK                     0xFFu
#define NV_BACKKEY6_KEY_SHIFT                    0
#define NV_BACKKEY6_KEY(x)                       (((uint8_t)(((uint8_t)(x))<<NV_BACKKEY6_KEY_SHIFT))&NV_BACKKEY6_KEY_MASK)
/* BACKKEY5 Bit Fields */
#define NV_BACKKEY5_KEY_MASK                     0xFFu
#define NV_BACKKEY5_KEY_SHIFT                    0
#define NV_BACKKEY5_KEY(x)                       (((uint8_t)(((uint8_t)(x))<<NV_BACKKEY5_KEY_SHIFT))&NV_BACKKEY5_KEY_MASK)
/* BACKKEY4 Bit Fields */
#define NV_BACKKEY4_KEY_MASK                     0xFFu
#define NV_BACKKEY4_KEY_SHIFT                    0
#define NV_BACKKEY4_KEY(x)                       (((uint8_t)(((uint8_t)(x))<<NV_BACKKEY4_KEY_SHIFT))&NV_BACKKEY4_KEY_MASK)
/* FPROT3 Bit Fields */
#define NV_FPROT3_PROT_MASK                      0xFFu
#define NV_FPROT3_PROT_SHIFT                     0
#define NV_FPROT3_PROT(x)                        (((uint8_t)(((uint8_t)(x))<<NV_FPROT3_PROT_SHIFT))&NV_FPROT3_PROT_MASK)
/* FPROT2 Bit Fields */
#define NV_FPROT2_PROT_MASK                      0xFFu
#define NV_FPROT2_PROT_SHIFT                     0
#define NV_FPROT2_PROT(x)                        (((uint8_t)(((uint8_t)(x))<<NV_FPROT2_PROT_SHIFT))&NV_FPROT2_PROT_MASK)
/* FPROT1 Bit Fields */
#define NV_FPROT1_PROT_MASK                      0xFFu
#define NV_FPROT1_PROT_SHIFT                     0
#define NV_FPROT1_PROT(x)                        (((uint8_t)(((uint8_t)(x))<<NV_FPROT1_PROT_SHIFT))&NV_FPROT1_PROT_MASK)
/* FPROT0 Bit Fields */
#define NV_FPROT0_PROT_MASK                      0xFFu
#define NV_FPROT0_PROT_SHIFT                     0
#define NV_FPROT0_PROT(x)                        (((uint8_t)(((uint8_t)(x))<<NV_FPROT0_PROT_SHIFT))&NV_FPROT0_PROT_MASK)
/* FSEC Bit Fields */
#define NV_FSEC_SEC_MASK                         0x3u
#define NV_FSEC_SEC_SHIFT                        0
#define NV_FSEC_SEC(x)                           (((uint8_t)(((uint8_t)(x))<<NV_FSEC_SEC_SHIFT))&NV_FSEC_SEC_MASK)
#define NV_FSEC_FSLACC_MASK                      0xCu
#define NV_FSEC_FSLACC_SHIFT                     2
#define NV_FSEC_FSLACC(x)                        (((uint8_t)(((uint8_t)(x))<<NV_FSEC_FSLACC_SHIFT))&NV_FSEC_FSLACC_MASK)
#define NV_FSEC_MEEN_MASK                        0x30u
#define NV_FSEC_MEEN_SHIFT                       4
#define NV_FSEC_MEEN(x)                          (((uint8_t)(((uint8_t)(x))<<NV_FSEC_MEEN_SHIFT))&NV_FSEC_MEEN_MASK)
#define NV_FSEC_KEYEN_MASK                       0xC0u
#define NV_FSEC_KEYEN_SHIFT                      6
#define NV_FSEC_KEYEN(x)                         (((uint8_t)(((uint8_t)(x))<<NV_FSEC_KEYEN_SHIFT))&NV_FSEC_KEYEN_MASK)
/* FOPT Bit Fields */
#define NV_FOPT_LPBOOT_MASK                      0x1u
#define NV_FOPT_LPBOOT_SHIFT                     0
#define NV_FOPT_EZPORT_DIS_MASK                  0x2u
#define NV_FOPT_EZPORT_DIS_SHIFT                 1
/* FEPROT Bit Fields */
#define NV_FEPROT_EPROT_MASK                     0xFFu
#define NV_FEPROT_EPROT_SHIFT                    0
#define NV_FEPROT_EPROT(x)                       (((uint8_t)(((uint8_t)(x))<<NV_FEPROT_EPROT_SHIFT))&NV_FEPROT_EPROT_MASK)
/* FDPROT Bit Fields */
#define NV_FDPROT_DPROT_MASK                     0xFFu
#define NV_FDPROT_DPROT_SHIFT                    0
#define NV_FDPROT_DPROT(x)                       (((uint8_t)(((uint8_t)(x))<<NV_FDPROT_DPROT_SHIFT))&NV_FDPROT_DPROT_MASK)

/**
 * @}
 */ /* end of group NV_Register_Masks */


/* NV - Peripheral instance base addresses */
/** Peripheral FTFL_FlashConfig base address */
#define FTFL_FlashConfig_BASE                    (0x400u)
/** Peripheral FTFL_FlashConfig base pointer */
#define FTFL_FlashConfig                         ((NV_Type *)FTFL_FlashConfig_BASE)
/** Array initializer of NV peripheral base pointers */
#define NV_BASES                                 { FTFL_FlashConfig }

/**
 * @}
 */ /* end of group NV_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- OSC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup OSC_Peripheral_Access_Layer OSC Peripheral Access Layer
 * @{
 */

/** OSC - Register Layout Typedef */
typedef struct {
  __IO uint8_t CR;                                 /**< OSC Control Register, offset: 0x0 */
} OSC_Type;

/* ----------------------------------------------------------------------------
   -- OSC Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup OSC_Register_Masks OSC Register Masks
 * @{
 */

/* CR Bit Fields */
#define OSC_CR_SC16P_MASK                        0x1u
#define OSC_CR_SC16P_SHIFT                       0
#define OSC_CR_SC8P_MASK                         0x2u
#define OSC_CR_SC8P_SHIFT                        1
#define OSC_CR_SC4P_MASK                         0x4u
#define OSC_CR_SC4P_SHIFT                        2
#define OSC_CR_SC2P_MASK                         0x8u
#define OSC_CR_SC2P_SHIFT                        3
#define OSC_CR_EREFSTEN_MASK                     0x20u
#define OSC_CR_EREFSTEN_SHIFT                    5
#define OSC_CR_ERCLKEN_MASK                      0x80u
#define OSC_CR_ERCLKEN_SHIFT                     7

/**
 * @}
 */ /* end of group OSC_Register_Masks */


/* OSC - Peripheral instance base addresses */
/** Peripheral OSC base address */
#define OSC_BASE                                 (0x40065000u)
/** Peripheral OSC base pointer */
#define OSC                                      ((OSC_Type *)OSC_BASE)
/** Array initializer of OSC peripheral base pointers */
#define OSC_BASES                                { OSC }

/**
 * @}
 */ /* end of group OSC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- PDB Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup PDB_Peripheral_Access_Layer PDB Peripheral Access Layer
 * @{
 */

/** PDB - Register Layout Typedef */
typedef struct {
  __IO uint32_t SC;                                /**< Status and Control Register, offset: 0x0 */
  __IO uint32_t MOD;                               /**< Modulus Register, offset: 0x4 */
  __I  uint32_t CNT;                               /**< Counter Register, offset: 0x8 */
  __IO uint32_t IDLY;                              /**< Interrupt Delay Register, offset: 0xC */
  struct {                                         /* offset: 0x10, array step: 0x28 */
    __IO uint32_t C1;                                /**< Channel n Control Register 1, array offset: 0x10, array step: 0x28 */
    __IO uint32_t S;                                 /**< Channel n Status Register, array offset: 0x14, array step: 0x28 */
    __IO uint32_t DLY[2];                            /**< Channel n Delay 0 Register..Channel n Delay 1 Register, array offset: 0x18, array step: index*0x28, index2*0x4 */
         uint8_t RESERVED_0[24];
  } CH[2];
       uint8_t RESERVED_0[240];
  struct {                                         /* offset: 0x150, array step: 0x8 */
    __IO uint32_t INTC;                              /**< DAC Interval Trigger n Control Register, array offset: 0x150, array step: 0x8 */
    __IO uint32_t INT;                               /**< DAC Interval n Register, array offset: 0x154, array step: 0x8 */
  } DAC[2];
       uint8_t RESERVED_1[48];
  __IO uint32_t POEN;                              /**< Pulse-Out n Enable Register, offset: 0x190 */
  __IO uint32_t PODLY[3];                          /**< Pulse-Out n Delay Register, array offset: 0x194, array step: 0x4 */
} PDB_Type;

/* ----------------------------------------------------------------------------
   -- PDB Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup PDB_Register_Masks PDB Register Masks
 * @{
 */

/* SC Bit Fields */
#define PDB_SC_LDOK_MASK                         0x1u
#define PDB_SC_LDOK_SHIFT                        0
#define PDB_SC_CONT_MASK                         0x2u
#define PDB_SC_CONT_SHIFT                        1
#define PDB_SC_MULT_MASK                         0xCu
#define PDB_SC_MULT_SHIFT                        2
#define PDB_SC_MULT(x)                           (((uint32_t)(((uint32_t)(x))<<PDB_SC_MULT_SHIFT))&PDB_SC_MULT_MASK)
#define PDB_SC_PDBIE_MASK                        0x20u
#define PDB_SC_PDBIE_SHIFT                       5
#define PDB_SC_PDBIF_MASK                        0x40u
#define PDB_SC_PDBIF_SHIFT                       6
#define PDB_SC_PDBEN_MASK                        0x80u
#define PDB_SC_PDBEN_SHIFT                       7
#define PDB_SC_TRGSEL_MASK                       0xF00u
#define PDB_SC_TRGSEL_SHIFT                      8
#define PDB_SC_TRGSEL(x)                         (((uint32_t)(((uint32_t)(x))<<PDB_SC_TRGSEL_SHIFT))&PDB_SC_TRGSEL_MASK)
#define PDB_SC_PRESCALER_MASK                    0x7000u
#define PDB_SC_PRESCALER_SHIFT                   12
#define PDB_SC_PRESCALER(x)                      (((uint32_t)(((uint32_t)(x))<<PDB_SC_PRESCALER_SHIFT))&PDB_SC_PRESCALER_MASK)
#define PDB_SC_DMAEN_MASK                        0x8000u
#define PDB_SC_DMAEN_SHIFT                       15
#define PDB_SC_SWTRIG_MASK                       0x10000u
#define PDB_SC_SWTRIG_SHIFT                      16
#define PDB_SC_PDBEIE_MASK                       0x20000u
#define PDB_SC_PDBEIE_SHIFT                      17
#define PDB_SC_LDMOD_MASK                        0xC0000u
#define PDB_SC_LDMOD_SHIFT                       18
#define PDB_SC_LDMOD(x)                          (((uint32_t)(((uint32_t)(x))<<PDB_SC_LDMOD_SHIFT))&PDB_SC_LDMOD_MASK)
/* MOD Bit Fields */
#define PDB_MOD_MOD_MASK                         0xFFFFu
#define PDB_MOD_MOD_SHIFT                        0
#define PDB_MOD_MOD(x)                           (((uint32_t)(((uint32_t)(x))<<PDB_MOD_MOD_SHIFT))&PDB_MOD_MOD_MASK)
/* CNT Bit Fields */
#define PDB_CNT_CNT_MASK                         0xFFFFu
#define PDB_CNT_CNT_SHIFT                        0
#define PDB_CNT_CNT(x)                           (((uint32_t)(((uint32_t)(x))<<PDB_CNT_CNT_SHIFT))&PDB_CNT_CNT_MASK)
/* IDLY Bit Fields */
#define PDB_IDLY_IDLY_MASK                       0xFFFFu
#define PDB_IDLY_IDLY_SHIFT                      0
#define PDB_IDLY_IDLY(x)                         (((uint32_t)(((uint32_t)(x))<<PDB_IDLY_IDLY_SHIFT))&PDB_IDLY_IDLY_MASK)
/* C1 Bit Fields */
#define PDB_C1_EN_MASK                           0xFFu
#define PDB_C1_EN_SHIFT                          0
#define PDB_C1_EN(x)                             (((uint32_t)(((uint32_t)(x))<<PDB_C1_EN_SHIFT))&PDB_C1_EN_MASK)
#define PDB_C1_TOS_MASK                          0xFF00u
#define PDB_C1_TOS_SHIFT                         8
#define PDB_C1_TOS(x)                            (((uint32_t)(((uint32_t)(x))<<PDB_C1_TOS_SHIFT))&PDB_C1_TOS_MASK)
#define PDB_C1_BB_MASK                           0xFF0000u
#define PDB_C1_BB_SHIFT                          16
#define PDB_C1_BB(x)                             (((uint32_t)(((uint32_t)(x))<<PDB_C1_BB_SHIFT))&PDB_C1_BB_MASK)
/* S Bit Fields */
#define PDB_S_ERR_MASK                           0xFFu
#define PDB_S_ERR_SHIFT                          0
#define PDB_S_ERR(x)                             (((uint32_t)(((uint32_t)(x))<<PDB_S_ERR_SHIFT))&PDB_S_ERR_MASK)
#define PDB_S_CF_MASK                            0xFF0000u
#define PDB_S_CF_SHIFT                           16
#define PDB_S_CF(x)                              (((uint32_t)(((uint32_t)(x))<<PDB_S_CF_SHIFT))&PDB_S_CF_MASK)
/* DLY Bit Fields */
#define PDB_DLY_DLY_MASK                         0xFFFFu
#define PDB_DLY_DLY_SHIFT                        0
#define PDB_DLY_DLY(x)                           (((uint32_t)(((uint32_t)(x))<<PDB_DLY_DLY_SHIFT))&PDB_DLY_DLY_MASK)
/* INTC Bit Fields */
#define PDB_INTC_TOE_MASK                        0x1u
#define PDB_INTC_TOE_SHIFT                       0
#define PDB_INTC_EXT_MASK                        0x2u
#define PDB_INTC_EXT_SHIFT                       1
/* INT Bit Fields */
#define PDB_INT_INT_MASK                         0xFFFFu
#define PDB_INT_INT_SHIFT                        0
#define PDB_INT_INT(x)                           (((uint32_t)(((uint32_t)(x))<<PDB_INT_INT_SHIFT))&PDB_INT_INT_MASK)
/* POEN Bit Fields */
#define PDB_POEN_POEN_MASK                       0xFFu
#define PDB_POEN_POEN_SHIFT                      0
#define PDB_POEN_POEN(x)                         (((uint32_t)(((uint32_t)(x))<<PDB_POEN_POEN_SHIFT))&PDB_POEN_POEN_MASK)
/* PODLY Bit Fields */
#define PDB_PODLY_DLY2_MASK                      0xFFFFu
#define PDB_PODLY_DLY2_SHIFT                     0
#define PDB_PODLY_DLY2(x)                        (((uint32_t)(((uint32_t)(x))<<PDB_PODLY_DLY2_SHIFT))&PDB_PODLY_DLY2_MASK)
#define PDB_PODLY_DLY1_MASK                      0xFFFF0000u
#define PDB_PODLY_DLY1_SHIFT                     16
#define PDB_PODLY_DLY1(x)                        (((uint32_t)(((uint32_t)(x))<<PDB_PODLY_DLY1_SHIFT))&PDB_PODLY_DLY1_MASK)

/**
 * @}
 */ /* end of group PDB_Register_Masks */


/* PDB - Peripheral instance base addresses */
/** Peripheral PDB0 base address */
#define PDB0_BASE                                (0x40036000u)
/** Peripheral PDB0 base pointer */
#define PDB0                                     ((PDB_Type *)PDB0_BASE)
/** Array initializer of PDB peripheral base pointers */
#define PDB_BASES                                { PDB0 }

/**
 * @}
 */ /* end of group PDB_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- PIT Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup PIT_Peripheral_Access_Layer PIT Peripheral Access Layer
 * @{
 */

/** PIT - Register Layout Typedef */
typedef struct {
  __IO uint32_t MCR;                               /**< PIT Module Control Register, offset: 0x0 */
       uint8_t RESERVED_0[252];
  struct {                                         /* offset: 0x100, array step: 0x10 */
    __IO uint32_t LDVAL;                             /**< Timer Load Value Register, array offset: 0x100, array step: 0x10 */
    __I  uint32_t CVAL;                              /**< Current Timer Value Register, array offset: 0x104, array step: 0x10 */
    __IO uint32_t TCTRL;                             /**< Timer Control Register, array offset: 0x108, array step: 0x10 */
    __IO uint32_t TFLG;                              /**< Timer Flag Register, array offset: 0x10C, array step: 0x10 */
  } CHANNEL[4];
} PIT_Type;

/* ----------------------------------------------------------------------------
   -- PIT Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup PIT_Register_Masks PIT Register Masks
 * @{
 */

/* MCR Bit Fields */
#define PIT_MCR_FRZ_MASK                         0x1u
#define PIT_MCR_FRZ_SHIFT                        0
#define PIT_MCR_MDIS_MASK                        0x2u
#define PIT_MCR_MDIS_SHIFT                       1
/* LDVAL Bit Fields */
#define PIT_LDVAL_TSV_MASK                       0xFFFFFFFFu
#define PIT_LDVAL_TSV_SHIFT                      0
#define PIT_LDVAL_TSV(x)                         (((uint32_t)(((uint32_t)(x))<<PIT_LDVAL_TSV_SHIFT))&PIT_LDVAL_TSV_MASK)
/* CVAL Bit Fields */
#define PIT_CVAL_TVL_MASK                        0xFFFFFFFFu
#define PIT_CVAL_TVL_SHIFT                       0
#define PIT_CVAL_TVL(x)                          (((uint32_t)(((uint32_t)(x))<<PIT_CVAL_TVL_SHIFT))&PIT_CVAL_TVL_MASK)
/* TCTRL Bit Fields */
#define PIT_TCTRL_TEN_MASK                       0x1u
#define PIT_TCTRL_TEN_SHIFT                      0
#define PIT_TCTRL_TIE_MASK                       0x2u
#define PIT_TCTRL_TIE_SHIFT                      1
#define PIT_TCTRL_CHN_MASK                       0x4u
#define PIT_TCTRL_CHN_SHIFT                      2
/* TFLG Bit Fields */
#define PIT_TFLG_TIF_MASK                        0x1u
#define PIT_TFLG_TIF_SHIFT                       0

/**
 * @}
 */ /* end of group PIT_Register_Masks */


/* PIT - Peripheral instance base addresses */
/** Peripheral PIT base address */
#define PIT_BASE                                 (0x40037000u)
/** Peripheral PIT base pointer */
#define PIT                                      ((PIT_Type *)PIT_BASE)
/** Array initializer of PIT peripheral base pointers */
#define PIT_BASES                                { PIT }

/**
 * @}
 */ /* end of group PIT_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- PMC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup PMC_Peripheral_Access_Layer PMC Peripheral Access Layer
 * @{
 */

/** PMC - Register Layout Typedef */
typedef struct {
  __IO uint8_t LVDSC1;                             /**< Low Voltage Detect Status And Control 1 register, offset: 0x0 */
  __IO uint8_t LVDSC2;                             /**< Low Voltage Detect Status And Control 2 register, offset: 0x1 */
  __IO uint8_t REGSC;                              /**< Regulator Status And Control register, offset: 0x2 */
} PMC_Type;

/* ----------------------------------------------------------------------------
   -- PMC Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup PMC_Register_Masks PMC Register Masks
 * @{
 */

/* LVDSC1 Bit Fields */
#define PMC_LVDSC1_LVDV_MASK                     0x3u
#define PMC_LVDSC1_LVDV_SHIFT                    0
#define PMC_LVDSC1_LVDV(x)                       (((uint8_t)(((uint8_t)(x))<<PMC_LVDSC1_LVDV_SHIFT))&PMC_LVDSC1_LVDV_MASK)
#define PMC_LVDSC1_LVDRE_MASK                    0x10u
#define PMC_LVDSC1_LVDRE_SHIFT                   4
#define PMC_LVDSC1_LVDIE_MASK                    0x20u
#define PMC_LVDSC1_LVDIE_SHIFT                   5
#define PMC_LVDSC1_LVDACK_MASK                   0x40u
#define PMC_LVDSC1_LVDACK_SHIFT                  6
#define PMC_LVDSC1_LVDF_MASK                     0x80u
#define PMC_LVDSC1_LVDF_SHIFT                    7
/* LVDSC2 Bit Fields */
#define PMC_LVDSC2_LVWV_MASK                     0x3u
#define PMC_LVDSC2_LVWV_SHIFT                    0
#define PMC_LVDSC2_LVWV(x)                       (((uint8_t)(((uint8_t)(x))<<PMC_LVDSC2_LVWV_SHIFT))&PMC_LVDSC2_LVWV_MASK)
#define PMC_LVDSC2_LVWIE_MASK                    0x20u
#define PMC_LVDSC2_LVWIE_SHIFT                   5
#define PMC_LVDSC2_LVWACK_MASK                   0x40u
#define PMC_LVDSC2_LVWACK_SHIFT                  6
#define PMC_LVDSC2_LVWF_MASK                     0x80u
#define PMC_LVDSC2_LVWF_SHIFT                    7
/* REGSC Bit Fields */
#define PMC_REGSC_BGBE_MASK                      0x1u
#define PMC_REGSC_BGBE_SHIFT                     0
#define PMC_REGSC_REGONS_MASK                    0x4u
#define PMC_REGSC_REGONS_SHIFT                   2
#define PMC_REGSC_ACKISO_MASK                    0x8u
#define PMC_REGSC_ACKISO_SHIFT                   3
#define PMC_REGSC_BGEN_MASK                      0x10u
#define PMC_REGSC_BGEN_SHIFT                     4

/**
 * @}
 */ /* end of group PMC_Register_Masks */


/* PMC - Peripheral instance base addresses */
/** Peripheral PMC base address */
#define PMC_BASE                                 (0x4007D000u)
/** Peripheral PMC base pointer */
#define PMC                                      ((PMC_Type *)PMC_BASE)
/** Array initializer of PMC peripheral base pointers */
#define PMC_BASES                                { PMC }

/**
 * @}
 */ /* end of group PMC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- PORT Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup PORT_Peripheral_Access_Layer PORT Peripheral Access Layer
 * @{
 */

/** PORT - Register Layout Typedef */
typedef struct {
  __IO uint32_t PCR[32];                           /**< Pin Control Register n, array offset: 0x0, array step: 0x4 */
  __O  uint32_t GPCLR;                             /**< Global Pin Control Low Register, offset: 0x80 */
  __O  uint32_t GPCHR;                             /**< Global Pin Control High Register, offset: 0x84 */
       uint8_t RESERVED_0[24];
  __IO uint32_t ISFR;                              /**< Interrupt Status Flag Register, offset: 0xA0 */
       uint8_t RESERVED_1[28];
  __IO uint32_t DFER;                              /**< Digital Filter Enable Register, offset: 0xC0 */
  __IO uint32_t DFCR;                              /**< Digital Filter Clock Register, offset: 0xC4 */
  __IO uint32_t DFWR;                              /**< Digital Filter Width Register, offset: 0xC8 */
} PORT_Type;

/* ----------------------------------------------------------------------------
   -- PORT Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup PORT_Register_Masks PORT Register Masks
 * @{
 */

/* PCR Bit Fields */
#define PORT_PCR_PS_MASK                         0x1u
#define PORT_PCR_PS_SHIFT                        0
#define PORT_PCR_PE_MASK                         0x2u
#define PORT_PCR_PE_SHIFT                        1
#define PORT_PCR_SRE_MASK                        0x4u
#define PORT_PCR_SRE_SHIFT                       2
#define PORT_PCR_PFE_MASK                        0x10u
#define PORT_PCR_PFE_SHIFT                       4
#define PORT_PCR_ODE_MASK                        0x20u
#define PORT_PCR_ODE_SHIFT                       5
#define PORT_PCR_DSE_MASK                        0x40u
#define PORT_PCR_DSE_SHIFT                       6
#define PORT_PCR_MUX_MASK                        0x700u
#define PORT_PCR_MUX_SHIFT                       8
#define PORT_PCR_MUX(x)                          (((uint32_t)(((uint32_t)(x))<<PORT_PCR_MUX_SHIFT))&PORT_PCR_MUX_MASK)
#define PORT_PCR_LK_MASK                         0x8000u
#define PORT_PCR_LK_SHIFT                        15
#define PORT_PCR_IRQC_MASK                       0xF0000u
#define PORT_PCR_IRQC_SHIFT                      16
#define PORT_PCR_IRQC(x)                         (((uint32_t)(((uint32_t)(x))<<PORT_PCR_IRQC_SHIFT))&PORT_PCR_IRQC_MASK)
#define PORT_PCR_ISF_MASK                        0x1000000u
#define PORT_PCR_ISF_SHIFT                       24
/* GPCLR Bit Fields */
#define PORT_GPCLR_GPWD_MASK                     0xFFFFu
#define PORT_GPCLR_GPWD_SHIFT                    0
#define PORT_GPCLR_GPWD(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_GPCLR_GPWD_SHIFT))&PORT_GPCLR_GPWD_MASK)
#define PORT_GPCLR_GPWE_MASK                     0xFFFF0000u
#define PORT_GPCLR_GPWE_SHIFT                    16
#define PORT_GPCLR_GPWE(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_GPCLR_GPWE_SHIFT))&PORT_GPCLR_GPWE_MASK)
/* GPCHR Bit Fields */
#define PORT_GPCHR_GPWD_MASK                     0xFFFFu
#define PORT_GPCHR_GPWD_SHIFT                    0
#define PORT_GPCHR_GPWD(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_GPCHR_GPWD_SHIFT))&PORT_GPCHR_GPWD_MASK)
#define PORT_GPCHR_GPWE_MASK                     0xFFFF0000u
#define PORT_GPCHR_GPWE_SHIFT                    16
#define PORT_GPCHR_GPWE(x)                       (((uint32_t)(((uint32_t)(x))<<PORT_GPCHR_GPWE_SHIFT))&PORT_GPCHR_GPWE_MASK)
/* ISFR Bit Fields */
#define PORT_ISFR_ISF_MASK                       0xFFFFFFFFu
#define PORT_ISFR_ISF_SHIFT                      0
#define PORT_ISFR_ISF(x)                         (((uint32_t)(((uint32_t)(x))<<PORT_ISFR_ISF_SHIFT))&PORT_ISFR_ISF_MASK)
/* DFER Bit Fields */
#define PORT_DFER_DFE_MASK                       0xFFFFFFFFu
#define PORT_DFER_DFE_SHIFT                      0
#define PORT_DFER_DFE(x)                         (((uint32_t)(((uint32_t)(x))<<PORT_DFER_DFE_SHIFT))&PORT_DFER_DFE_MASK)
/* DFCR Bit Fields */
#define PORT_DFCR_CS_MASK                        0x1u
#define PORT_DFCR_CS_SHIFT                       0
/* DFWR Bit Fields */
#define PORT_DFWR_FILT_MASK                      0x1Fu
#define PORT_DFWR_FILT_SHIFT                     0
#define PORT_DFWR_FILT(x)                        (((uint32_t)(((uint32_t)(x))<<PORT_DFWR_FILT_SHIFT))&PORT_DFWR_FILT_MASK)

/**
 * @}
 */ /* end of group PORT_Register_Masks */


/* PORT - Peripheral instance base addresses */
/** Peripheral PORTA base address */
#define PORTA_BASE                               (0x40049000u)
/** Peripheral PORTA base pointer */
#define PORTA                                    ((PORT_Type *)PORTA_BASE)
/** Peripheral PORTB base address */
#define PORTB_BASE                               (0x4004A000u)
/** Peripheral PORTB base pointer */
#define PORTB                                    ((PORT_Type *)PORTB_BASE)
/** Peripheral PORTC base address */
#define PORTC_BASE                               (0x4004B000u)
/** Peripheral PORTC base pointer */
#define PORTC                                    ((PORT_Type *)PORTC_BASE)
/** Peripheral PORTD base address */
#define PORTD_BASE                               (0x4004C000u)
/** Peripheral PORTD base pointer */
#define PORTD                                    ((PORT_Type *)PORTD_BASE)
/** Peripheral PORTE base address */
#define PORTE_BASE                               (0x4004D000u)
/** Peripheral PORTE base pointer */
#define PORTE                                    ((PORT_Type *)PORTE_BASE)
/** Array initializer of PORT peripheral base pointers */
#define PORT_BASES                               { PORTA, PORTB, PORTC, PORTD, PORTE }

/**
 * @}
 */ /* end of group PORT_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- RCM Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup RCM_Peripheral_Access_Layer RCM Peripheral Access Layer
 * @{
 */

/** RCM - Register Layout Typedef */
typedef struct {
  __I  uint8_t SRS0;                               /**< System Reset Status Register 0, offset: 0x0 */
  __I  uint8_t SRS1;                               /**< System Reset Status Register 1, offset: 0x1 */
       uint8_t RESERVED_0[2];
  __IO uint8_t RPFC;                               /**< Reset Pin Filter Control register, offset: 0x4 */
  __IO uint8_t RPFW;                               /**< Reset Pin Filter Width register, offset: 0x5 */
       uint8_t RESERVED_1[1];
  __I  uint8_t MR;                                 /**< Mode Register, offset: 0x7 */
} RCM_Type;

/* ----------------------------------------------------------------------------
   -- RCM Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup RCM_Register_Masks RCM Register Masks
 * @{
 */

/* SRS0 Bit Fields */
#define RCM_SRS0_WAKEUP_MASK                     0x1u
#define RCM_SRS0_WAKEUP_SHIFT                    0
#define RCM_SRS0_LVD_MASK                        0x2u
#define RCM_SRS0_LVD_SHIFT                       1
#define RCM_SRS0_LOC_MASK                        0x4u
#define RCM_SRS0_LOC_SHIFT                       2
#define RCM_SRS0_LOL_MASK                        0x8u
#define RCM_SRS0_LOL_SHIFT                       3
#define RCM_SRS0_WDOG_MASK                       0x20u
#define RCM_SRS0_WDOG_SHIFT                      5
#define RCM_SRS0_PIN_MASK                        0x40u
#define RCM_SRS0_PIN_SHIFT                       6
#define RCM_SRS0_POR_MASK                        0x80u
#define RCM_SRS0_POR_SHIFT                       7
/* SRS1 Bit Fields */
#define RCM_SRS1_JTAG_MASK                       0x1u
#define RCM_SRS1_JTAG_SHIFT                      0
#define RCM_SRS1_LOCKUP_MASK                     0x2u
#define RCM_SRS1_LOCKUP_SHIFT                    1
#define RCM_SRS1_SW_MASK                         0x4u
#define RCM_SRS1_SW_SHIFT                        2
#define RCM_SRS1_MDM_AP_MASK                     0x8u
#define RCM_SRS1_MDM_AP_SHIFT                    3
#define RCM_SRS1_EZPT_MASK                       0x10u
#define RCM_SRS1_EZPT_SHIFT                      4
#define RCM_SRS1_SACKERR_MASK                    0x20u
#define RCM_SRS1_SACKERR_SHIFT                   5
/* RPFC Bit Fields */
#define RCM_RPFC_RSTFLTSRW_MASK                  0x3u
#define RCM_RPFC_RSTFLTSRW_SHIFT                 0
#define RCM_RPFC_RSTFLTSRW(x)                    (((uint8_t)(((uint8_t)(x))<<RCM_RPFC_RSTFLTSRW_SHIFT))&RCM_RPFC_RSTFLTSRW_MASK)
#define RCM_RPFC_RSTFLTSS_MASK                   0x4u
#define RCM_RPFC_RSTFLTSS_SHIFT                  2
/* RPFW Bit Fields */
#define RCM_RPFW_RSTFLTSEL_MASK                  0x1Fu
#define RCM_RPFW_RSTFLTSEL_SHIFT                 0
#define RCM_RPFW_RSTFLTSEL(x)                    (((uint8_t)(((uint8_t)(x))<<RCM_RPFW_RSTFLTSEL_SHIFT))&RCM_RPFW_RSTFLTSEL_MASK)
/* MR Bit Fields */
#define RCM_MR_EZP_MS_MASK                       0x2u
#define RCM_MR_EZP_MS_SHIFT                      1

/**
 * @}
 */ /* end of group RCM_Register_Masks */


/* RCM - Peripheral instance base addresses */
/** Peripheral RCM base address */
#define RCM_BASE                                 (0x4007F000u)
/** Peripheral RCM base pointer */
#define RCM                                      ((RCM_Type *)RCM_BASE)
/** Array initializer of RCM peripheral base pointers */
#define RCM_BASES                                { RCM }

/**
 * @}
 */ /* end of group RCM_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- RFSYS Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup RFSYS_Peripheral_Access_Layer RFSYS Peripheral Access Layer
 * @{
 */

/** RFSYS - Register Layout Typedef */
typedef struct {
  __IO uint32_t REG[8];                            /**< Register file register, array offset: 0x0, array step: 0x4 */
} RFSYS_Type;

/* ----------------------------------------------------------------------------
   -- RFSYS Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup RFSYS_Register_Masks RFSYS Register Masks
 * @{
 */

/* REG Bit Fields */
#define RFSYS_REG_LL_MASK                        0xFFu
#define RFSYS_REG_LL_SHIFT                       0
#define RFSYS_REG_LL(x)                          (((uint32_t)(((uint32_t)(x))<<RFSYS_REG_LL_SHIFT))&RFSYS_REG_LL_MASK)
#define RFSYS_REG_LH_MASK                        0xFF00u
#define RFSYS_REG_LH_SHIFT                       8
#define RFSYS_REG_LH(x)                          (((uint32_t)(((uint32_t)(x))<<RFSYS_REG_LH_SHIFT))&RFSYS_REG_LH_MASK)
#define RFSYS_REG_HL_MASK                        0xFF0000u
#define RFSYS_REG_HL_SHIFT                       16
#define RFSYS_REG_HL(x)                          (((uint32_t)(((uint32_t)(x))<<RFSYS_REG_HL_SHIFT))&RFSYS_REG_HL_MASK)
#define RFSYS_REG_HH_MASK                        0xFF000000u
#define RFSYS_REG_HH_SHIFT                       24
#define RFSYS_REG_HH(x)                          (((uint32_t)(((uint32_t)(x))<<RFSYS_REG_HH_SHIFT))&RFSYS_REG_HH_MASK)

/**
 * @}
 */ /* end of group RFSYS_Register_Masks */


/* RFSYS - Peripheral instance base addresses */
/** Peripheral RFSYS base address */
#define RFSYS_BASE                               (0x40041000u)
/** Peripheral RFSYS base pointer */
#define RFSYS                                    ((RFSYS_Type *)RFSYS_BASE)
/** Array initializer of RFSYS peripheral base pointers */
#define RFSYS_BASES                              { RFSYS }

/**
 * @}
 */ /* end of group RFSYS_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- RFVBAT Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup RFVBAT_Peripheral_Access_Layer RFVBAT Peripheral Access Layer
 * @{
 */

/** RFVBAT - Register Layout Typedef */
typedef struct {
  __IO uint32_t REG[8];                            /**< VBAT register file register, array offset: 0x0, array step: 0x4 */
} RFVBAT_Type;

/* ----------------------------------------------------------------------------
   -- RFVBAT Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup RFVBAT_Register_Masks RFVBAT Register Masks
 * @{
 */

/* REG Bit Fields */
#define RFVBAT_REG_LL_MASK                       0xFFu
#define RFVBAT_REG_LL_SHIFT                      0
#define RFVBAT_REG_LL(x)                         (((uint32_t)(((uint32_t)(x))<<RFVBAT_REG_LL_SHIFT))&RFVBAT_REG_LL_MASK)
#define RFVBAT_REG_LH_MASK                       0xFF00u
#define RFVBAT_REG_LH_SHIFT                      8
#define RFVBAT_REG_LH(x)                         (((uint32_t)(((uint32_t)(x))<<RFVBAT_REG_LH_SHIFT))&RFVBAT_REG_LH_MASK)
#define RFVBAT_REG_HL_MASK                       0xFF0000u
#define RFVBAT_REG_HL_SHIFT                      16
#define RFVBAT_REG_HL(x)                         (((uint32_t)(((uint32_t)(x))<<RFVBAT_REG_HL_SHIFT))&RFVBAT_REG_HL_MASK)
#define RFVBAT_REG_HH_MASK                       0xFF000000u
#define RFVBAT_REG_HH_SHIFT                      24
#define RFVBAT_REG_HH(x)                         (((uint32_t)(((uint32_t)(x))<<RFVBAT_REG_HH_SHIFT))&RFVBAT_REG_HH_MASK)

/**
 * @}
 */ /* end of group RFVBAT_Register_Masks */


/* RFVBAT - Peripheral instance base addresses */
/** Peripheral RFVBAT base address */
#define RFVBAT_BASE                              (0x4003E000u)
/** Peripheral RFVBAT base pointer */
#define RFVBAT                                   ((RFVBAT_Type *)RFVBAT_BASE)
/** Array initializer of RFVBAT peripheral base pointers */
#define RFVBAT_BASES                             { RFVBAT }

/**
 * @}
 */ /* end of group RFVBAT_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- RNG Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup RNG_Peripheral_Access_Layer RNG Peripheral Access Layer
 * @{
 */

/** RNG - Register Layout Typedef */
typedef struct {
  __IO uint32_t CR;                                /**< RNGA Control Register, offset: 0x0 */
  __I  uint32_t SR;                                /**< RNGA Status Register, offset: 0x4 */
  __O  uint32_t ER;                                /**< RNGA Entropy Register, offset: 0x8 */
  __I  uint32_t OR;                                /**< RNGA Output Register, offset: 0xC */
} RNG_Type;

/* ----------------------------------------------------------------------------
   -- RNG Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup RNG_Register_Masks RNG Register Masks
 * @{
 */

/* CR Bit Fields */
#define RNG_CR_GO_MASK                           0x1u
#define RNG_CR_GO_SHIFT                          0
#define RNG_CR_HA_MASK                           0x2u
#define RNG_CR_HA_SHIFT                          1
#define RNG_CR_INTM_MASK                         0x4u
#define RNG_CR_INTM_SHIFT                        2
#define RNG_CR_CLRI_MASK                         0x8u
#define RNG_CR_CLRI_SHIFT                        3
#define RNG_CR_SLP_MASK                          0x10u
#define RNG_CR_SLP_SHIFT                         4
/* SR Bit Fields */
#define RNG_SR_SECV_MASK                         0x1u
#define RNG_SR_SECV_SHIFT                        0
#define RNG_SR_LRS_MASK                          0x2u
#define RNG_SR_LRS_SHIFT                         1
#define RNG_SR_ORU_MASK                          0x4u
#define RNG_SR_ORU_SHIFT                         2
#define RNG_SR_ERRI_MASK                         0x8u
#define RNG_SR_ERRI_SHIFT                        3
#define RNG_SR_SLP_MASK                          0x10u
#define RNG_SR_SLP_SHIFT                         4
#define RNG_SR_OREG_LVL_MASK                     0xFF00u
#define RNG_SR_OREG_LVL_SHIFT                    8
#define RNG_SR_OREG_LVL(x)                       (((uint32_t)(((uint32_t)(x))<<RNG_SR_OREG_LVL_SHIFT))&RNG_SR_OREG_LVL_MASK)
#define RNG_SR_OREG_SIZE_MASK                    0xFF0000u
#define RNG_SR_OREG_SIZE_SHIFT                   16
#define RNG_SR_OREG_SIZE(x)                      (((uint32_t)(((uint32_t)(x))<<RNG_SR_OREG_SIZE_SHIFT))&RNG_SR_OREG_SIZE_MASK)
/* ER Bit Fields */
#define RNG_ER_EXT_ENT_MASK                      0xFFFFFFFFu
#define RNG_ER_EXT_ENT_SHIFT                     0
#define RNG_ER_EXT_ENT(x)                        (((uint32_t)(((uint32_t)(x))<<RNG_ER_EXT_ENT_SHIFT))&RNG_ER_EXT_ENT_MASK)
/* OR Bit Fields */
#define RNG_OR_RANDOUT_MASK                      0xFFFFFFFFu
#define RNG_OR_RANDOUT_SHIFT                     0
#define RNG_OR_RANDOUT(x)                        (((uint32_t)(((uint32_t)(x))<<RNG_OR_RANDOUT_SHIFT))&RNG_OR_RANDOUT_MASK)

/**
 * @}
 */ /* end of group RNG_Register_Masks */


/* RNG - Peripheral instance base addresses */
/** Peripheral RNG base address */
#define RNG_BASE                                 (0x400A0000u)
/** Peripheral RNG base pointer */
#define RNG                                      ((RNG_Type *)RNG_BASE)
/** Array initializer of RNG peripheral base pointers */
#define RNG_BASES                                { RNG }

/**
 * @}
 */ /* end of group RNG_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- RTC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup RTC_Peripheral_Access_Layer RTC Peripheral Access Layer
 * @{
 */

/** RTC - Register Layout Typedef */
typedef struct {
  __IO uint32_t TSR;                               /**< RTC Time Seconds Register, offset: 0x0 */
  __IO uint32_t TPR;                               /**< RTC Time Prescaler Register, offset: 0x4 */
  __IO uint32_t TAR;                               /**< RTC Time Alarm Register, offset: 0x8 */
  __IO uint32_t TCR;                               /**< RTC Time Compensation Register, offset: 0xC */
  __IO uint32_t CR;                                /**< RTC Control Register, offset: 0x10 */
  __IO uint32_t SR;                                /**< RTC Status Register, offset: 0x14 */
  __IO uint32_t LR;                                /**< RTC Lock Register, offset: 0x18 */
  __IO uint32_t IER;                               /**< RTC Interrupt Enable Register, offset: 0x1C */
       uint8_t RESERVED_0[2016];
  __IO uint32_t WAR;                               /**< RTC Write Access Register, offset: 0x800 */
  __IO uint32_t RAR;                               /**< RTC Read Access Register, offset: 0x804 */
} RTC_Type;

/* ----------------------------------------------------------------------------
   -- RTC Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup RTC_Register_Masks RTC Register Masks
 * @{
 */

/* TSR Bit Fields */
#define RTC_TSR_TSR_MASK                         0xFFFFFFFFu
#define RTC_TSR_TSR_SHIFT                        0
#define RTC_TSR_TSR(x)                           (((uint32_t)(((uint32_t)(x))<<RTC_TSR_TSR_SHIFT))&RTC_TSR_TSR_MASK)
/* TPR Bit Fields */
#define RTC_TPR_TPR_MASK                         0xFFFFu
#define RTC_TPR_TPR_SHIFT                        0
#define RTC_TPR_TPR(x)                           (((uint32_t)(((uint32_t)(x))<<RTC_TPR_TPR_SHIFT))&RTC_TPR_TPR_MASK)
/* TAR Bit Fields */
#define RTC_TAR_TAR_MASK                         0xFFFFFFFFu
#define RTC_TAR_TAR_SHIFT                        0
#define RTC_TAR_TAR(x)                           (((uint32_t)(((uint32_t)(x))<<RTC_TAR_TAR_SHIFT))&RTC_TAR_TAR_MASK)
/* TCR Bit Fields */
#define RTC_TCR_TCR_MASK                         0xFFu
#define RTC_TCR_TCR_SHIFT                        0
#define RTC_TCR_TCR(x)                           (((uint32_t)(((uint32_t)(x))<<RTC_TCR_TCR_SHIFT))&RTC_TCR_TCR_MASK)
#define RTC_TCR_CIR_MASK                         0xFF00u
#define RTC_TCR_CIR_SHIFT                        8
#define RTC_TCR_CIR(x)                           (((uint32_t)(((uint32_t)(x))<<RTC_TCR_CIR_SHIFT))&RTC_TCR_CIR_MASK)
#define RTC_TCR_TCV_MASK                         0xFF0000u
#define RTC_TCR_TCV_SHIFT                        16
#define RTC_TCR_TCV(x)                           (((uint32_t)(((uint32_t)(x))<<RTC_TCR_TCV_SHIFT))&RTC_TCR_TCV_MASK)
#define RTC_TCR_CIC_MASK                         0xFF000000u
#define RTC_TCR_CIC_SHIFT                        24
#define RTC_TCR_CIC(x)                           (((uint32_t)(((uint32_t)(x))<<RTC_TCR_CIC_SHIFT))&RTC_TCR_CIC_MASK)
/* CR Bit Fields */
#define RTC_CR_SWR_MASK                          0x1u
#define RTC_CR_SWR_SHIFT                         0
#define RTC_CR_WPE_MASK                          0x2u
#define RTC_CR_WPE_SHIFT                         1
#define RTC_CR_SUP_MASK                          0x4u
#define RTC_CR_SUP_SHIFT                         2
#define RTC_CR_UM_MASK                           0x8u
#define RTC_CR_UM_SHIFT                          3
#define RTC_CR_OSCE_MASK                         0x100u
#define RTC_CR_OSCE_SHIFT                        8
#define RTC_CR_CLKO_MASK                         0x200u
#define RTC_CR_CLKO_SHIFT                        9
#define RTC_CR_SC16P_MASK                        0x400u
#define RTC_CR_SC16P_SHIFT                       10
#define RTC_CR_SC8P_MASK                         0x800u
#define RTC_CR_SC8P_SHIFT                        11
#define RTC_CR_SC4P_MASK                         0x1000u
#define RTC_CR_SC4P_SHIFT                        12
#define RTC_CR_SC2P_MASK                         0x2000u
#define RTC_CR_SC2P_SHIFT                        13
/* SR Bit Fields */
#define RTC_SR_TIF_MASK                          0x1u
#define RTC_SR_TIF_SHIFT                         0
#define RTC_SR_TOF_MASK                          0x2u
#define RTC_SR_TOF_SHIFT                         1
#define RTC_SR_TAF_MASK                          0x4u
#define RTC_SR_TAF_SHIFT                         2
#define RTC_SR_TCE_MASK                          0x10u
#define RTC_SR_TCE_SHIFT                         4
/* LR Bit Fields */
#define RTC_LR_TCL_MASK                          0x8u
#define RTC_LR_TCL_SHIFT                         3
#define RTC_LR_CRL_MASK                          0x10u
#define RTC_LR_CRL_SHIFT                         4
#define RTC_LR_SRL_MASK                          0x20u
#define RTC_LR_SRL_SHIFT                         5
#define RTC_LR_LRL_MASK                          0x40u
#define RTC_LR_LRL_SHIFT                         6
/* IER Bit Fields */
#define RTC_IER_TIIE_MASK                        0x1u
#define RTC_IER_TIIE_SHIFT                       0
#define RTC_IER_TOIE_MASK                        0x2u
#define RTC_IER_TOIE_SHIFT                       1
#define RTC_IER_TAIE_MASK                        0x4u
#define RTC_IER_TAIE_SHIFT                       2
#define RTC_IER_TSIE_MASK                        0x10u
#define RTC_IER_TSIE_SHIFT                       4
#define RTC_IER_WPON_MASK                        0x80u
#define RTC_IER_WPON_SHIFT                       7
/* WAR Bit Fields */
#define RTC_WAR_TSRW_MASK                        0x1u
#define RTC_WAR_TSRW_SHIFT                       0
#define RTC_WAR_TPRW_MASK                        0x2u
#define RTC_WAR_TPRW_SHIFT                       1
#define RTC_WAR_TARW_MASK                        0x4u
#define RTC_WAR_TARW_SHIFT                       2
#define RTC_WAR_TCRW_MASK                        0x8u
#define RTC_WAR_TCRW_SHIFT                       3
#define RTC_WAR_CRW_MASK                         0x10u
#define RTC_WAR_CRW_SHIFT                        4
#define RTC_WAR_SRW_MASK                         0x20u
#define RTC_WAR_SRW_SHIFT                        5
#define RTC_WAR_LRW_MASK                         0x40u
#define RTC_WAR_LRW_SHIFT                        6
#define RTC_WAR_IERW_MASK                        0x80u
#define RTC_WAR_IERW_SHIFT                       7
/* RAR Bit Fields */
#define RTC_RAR_TSRR_MASK                        0x1u
#define RTC_RAR_TSRR_SHIFT                       0
#define RTC_RAR_TPRR_MASK                        0x2u
#define RTC_RAR_TPRR_SHIFT                       1
#define RTC_RAR_TARR_MASK                        0x4u
#define RTC_RAR_TARR_SHIFT                       2
#define RTC_RAR_TCRR_MASK                        0x8u
#define RTC_RAR_TCRR_SHIFT                       3
#define RTC_RAR_CRR_MASK                         0x10u
#define RTC_RAR_CRR_SHIFT                        4
#define RTC_RAR_SRR_MASK                         0x20u
#define RTC_RAR_SRR_SHIFT                        5
#define RTC_RAR_LRR_MASK                         0x40u
#define RTC_RAR_LRR_SHIFT                        6
#define RTC_RAR_IERR_MASK                        0x80u
#define RTC_RAR_IERR_SHIFT                       7

/**
 * @}
 */ /* end of group RTC_Register_Masks */


/* RTC - Peripheral instance base addresses */
/** Peripheral RTC base address */
#define RTC_BASE                                 (0x4003D000u)
/** Peripheral RTC base pointer */
#define RTC                                      ((RTC_Type *)RTC_BASE)
/** Array initializer of RTC peripheral base pointers */
#define RTC_BASES                                { RTC }

/**
 * @}
 */ /* end of group RTC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- SDHC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup SDHC_Peripheral_Access_Layer SDHC Peripheral Access Layer
 * @{
 */

/** SDHC - Register Layout Typedef */
typedef struct {
  __IO uint32_t DSADDR;                            /**< DMA System Address register, offset: 0x0 */
  __IO uint32_t BLKATTR;                           /**< Block Attributes register, offset: 0x4 */
  __IO uint32_t CMDARG;                            /**< Command Argument register, offset: 0x8 */
  __IO uint32_t XFERTYP;                           /**< Transfer Type register, offset: 0xC */
  __I  uint32_t CMDRSP[4];                         /**< Command Response 0..Command Response 3, array offset: 0x10, array step: 0x4 */
  __IO uint32_t DATPORT;                           /**< Buffer Data Port register, offset: 0x20 */
  __I  uint32_t PRSSTAT;                           /**< Present State register, offset: 0x24 */
  __IO uint32_t PROCTL;                            /**< Protocol Control register, offset: 0x28 */
  __IO uint32_t SYSCTL;                            /**< System Control register, offset: 0x2C */
  __IO uint32_t IRQSTAT;                           /**< Interrupt Status register, offset: 0x30 */
  __IO uint32_t IRQSTATEN;                         /**< Interrupt Status Enable register, offset: 0x34 */
  __IO uint32_t IRQSIGEN;                          /**< Interrupt Signal Enable register, offset: 0x38 */
  __I  uint32_t AC12ERR;                           /**< Auto CMD12 Error Status Register, offset: 0x3C */
  __I  uint32_t HTCAPBLT;                          /**< Host Controller Capabilities, offset: 0x40 */
  __IO uint32_t WML;                               /**< Watermark Level Register, offset: 0x44 */
       uint8_t RESERVED_0[8];
  __O  uint32_t FEVT;                              /**< Force Event register, offset: 0x50 */
  __I  uint32_t ADMAES;                            /**< ADMA Error Status register, offset: 0x54 */
  __IO uint32_t ADSADDR;                           /**< ADMA System Addressregister, offset: 0x58 */
       uint8_t RESERVED_1[100];
  __IO uint32_t VENDOR;                            /**< Vendor Specific register, offset: 0xC0 */
  __IO uint32_t MMCBOOT;                           /**< MMC Boot register, offset: 0xC4 */
       uint8_t RESERVED_2[52];
  __I  uint32_t HOSTVER;                           /**< Host Controller Version, offset: 0xFC */
} SDHC_Type;

/* ----------------------------------------------------------------------------
   -- SDHC Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup SDHC_Register_Masks SDHC Register Masks
 * @{
 */

/* DSADDR Bit Fields */
#define SDHC_DSADDR_DSADDR_MASK                  0xFFFFFFFCu
#define SDHC_DSADDR_DSADDR_SHIFT                 2
#define SDHC_DSADDR_DSADDR(x)                    (((uint32_t)(((uint32_t)(x))<<SDHC_DSADDR_DSADDR_SHIFT))&SDHC_DSADDR_DSADDR_MASK)
/* BLKATTR Bit Fields */
#define SDHC_BLKATTR_BLKSIZE_MASK                0x1FFFu
#define SDHC_BLKATTR_BLKSIZE_SHIFT               0
#define SDHC_BLKATTR_BLKSIZE(x)                  (((uint32_t)(((uint32_t)(x))<<SDHC_BLKATTR_BLKSIZE_SHIFT))&SDHC_BLKATTR_BLKSIZE_MASK)
#define SDHC_BLKATTR_BLKCNT_MASK                 0xFFFF0000u
#define SDHC_BLKATTR_BLKCNT_SHIFT                16
#define SDHC_BLKATTR_BLKCNT(x)                   (((uint32_t)(((uint32_t)(x))<<SDHC_BLKATTR_BLKCNT_SHIFT))&SDHC_BLKATTR_BLKCNT_MASK)
/* CMDARG Bit Fields */
#define SDHC_CMDARG_CMDARG_MASK                  0xFFFFFFFFu
#define SDHC_CMDARG_CMDARG_SHIFT                 0
#define SDHC_CMDARG_CMDARG(x)                    (((uint32_t)(((uint32_t)(x))<<SDHC_CMDARG_CMDARG_SHIFT))&SDHC_CMDARG_CMDARG_MASK)
/* XFERTYP Bit Fields */
#define SDHC_XFERTYP_DMAEN_MASK                  0x1u
#define SDHC_XFERTYP_DMAEN_SHIFT                 0
#define SDHC_XFERTYP_BCEN_MASK                   0x2u
#define SDHC_XFERTYP_BCEN_SHIFT                  1
#define SDHC_XFERTYP_AC12EN_MASK                 0x4u
#define SDHC_XFERTYP_AC12EN_SHIFT                2
#define SDHC_XFERTYP_DTDSEL_MASK                 0x10u
#define SDHC_XFERTYP_DTDSEL_SHIFT                4
#define SDHC_XFERTYP_MSBSEL_MASK                 0x20u
#define SDHC_XFERTYP_MSBSEL_SHIFT                5
#define SDHC_XFERTYP_RSPTYP_MASK                 0x30000u
#define SDHC_XFERTYP_RSPTYP_SHIFT                16
#define SDHC_XFERTYP_RSPTYP(x)                   (((uint32_t)(((uint32_t)(x))<<SDHC_XFERTYP_RSPTYP_SHIFT))&SDHC_XFERTYP_RSPTYP_MASK)
#define SDHC_XFERTYP_CCCEN_MASK                  0x80000u
#define SDHC_XFERTYP_CCCEN_SHIFT                 19
#define SDHC_XFERTYP_CICEN_MASK                  0x100000u
#define SDHC_XFERTYP_CICEN_SHIFT                 20
#define SDHC_XFERTYP_DPSEL_MASK                  0x200000u
#define SDHC_XFERTYP_DPSEL_SHIFT                 21
#define SDHC_XFERTYP_CMDTYP_MASK                 0xC00000u
#define SDHC_XFERTYP_CMDTYP_SHIFT                22
#define SDHC_XFERTYP_CMDTYP(x)                   (((uint32_t)(((uint32_t)(x))<<SDHC_XFERTYP_CMDTYP_SHIFT))&SDHC_XFERTYP_CMDTYP_MASK)
#define SDHC_XFERTYP_CMDINX_MASK                 0x3F000000u
#define SDHC_XFERTYP_CMDINX_SHIFT                24
#define SDHC_XFERTYP_CMDINX(x)                   (((uint32_t)(((uint32_t)(x))<<SDHC_XFERTYP_CMDINX_SHIFT))&SDHC_XFERTYP_CMDINX_MASK)
/* CMDRSP Bit Fields */
#define SDHC_CMDRSP_CMDRSP0_MASK                 0xFFFFFFFFu
#define SDHC_CMDRSP_CMDRSP0_SHIFT                0
#define SDHC_CMDRSP_CMDRSP0(x)                   (((uint32_t)(((uint32_t)(x))<<SDHC_CMDRSP_CMDRSP0_SHIFT))&SDHC_CMDRSP_CMDRSP0_MASK)
#define SDHC_CMDRSP_CMDRSP1_MASK                 0xFFFFFFFFu
#define SDHC_CMDRSP_CMDRSP1_SHIFT                0
#define SDHC_CMDRSP_CMDRSP1(x)                   (((uint32_t)(((uint32_t)(x))<<SDHC_CMDRSP_CMDRSP1_SHIFT))&SDHC_CMDRSP_CMDRSP1_MASK)
#define SDHC_CMDRSP_CMDRSP2_MASK                 0xFFFFFFFFu
#define SDHC_CMDRSP_CMDRSP2_SHIFT                0
#define SDHC_CMDRSP_CMDRSP2(x)                   (((uint32_t)(((uint32_t)(x))<<SDHC_CMDRSP_CMDRSP2_SHIFT))&SDHC_CMDRSP_CMDRSP2_MASK)
#define SDHC_CMDRSP_CMDRSP3_MASK                 0xFFFFFFFFu
#define SDHC_CMDRSP_CMDRSP3_SHIFT                0
#define SDHC_CMDRSP_CMDRSP3(x)                   (((uint32_t)(((uint32_t)(x))<<SDHC_CMDRSP_CMDRSP3_SHIFT))&SDHC_CMDRSP_CMDRSP3_MASK)
/* DATPORT Bit Fields */
#define SDHC_DATPORT_DATCONT_MASK                0xFFFFFFFFu
#define SDHC_DATPORT_DATCONT_SHIFT               0
#define SDHC_DATPORT_DATCONT(x)                  (((uint32_t)(((uint32_t)(x))<<SDHC_DATPORT_DATCONT_SHIFT))&SDHC_DATPORT_DATCONT_MASK)
/* PRSSTAT Bit Fields */
#define SDHC_PRSSTAT_CIHB_MASK                   0x1u
#define SDHC_PRSSTAT_CIHB_SHIFT                  0
#define SDHC_PRSSTAT_CDIHB_MASK                  0x2u
#define SDHC_PRSSTAT_CDIHB_SHIFT                 1
#define SDHC_PRSSTAT_DLA_MASK                    0x4u
#define SDHC_PRSSTAT_DLA_SHIFT                   2
#define SDHC_PRSSTAT_SDSTB_MASK                  0x8u
#define SDHC_PRSSTAT_SDSTB_SHIFT                 3
#define SDHC_PRSSTAT_IPGOFF_MASK                 0x10u
#define SDHC_PRSSTAT_IPGOFF_SHIFT                4
#define SDHC_PRSSTAT_HCKOFF_MASK                 0x20u
#define SDHC_PRSSTAT_HCKOFF_SHIFT                5
#define SDHC_PRSSTAT_PEROFF_MASK                 0x40u
#define SDHC_PRSSTAT_PEROFF_SHIFT                6
#define SDHC_PRSSTAT_SDOFF_MASK                  0x80u
#define SDHC_PRSSTAT_SDOFF_SHIFT                 7
#define SDHC_PRSSTAT_WTA_MASK                    0x100u
#define SDHC_PRSSTAT_WTA_SHIFT                   8
#define SDHC_PRSSTAT_RTA_MASK                    0x200u
#define SDHC_PRSSTAT_RTA_SHIFT                   9
#define SDHC_PRSSTAT_BWEN_MASK                   0x400u
#define SDHC_PRSSTAT_BWEN_SHIFT                  10
#define SDHC_PRSSTAT_BREN_MASK                   0x800u
#define SDHC_PRSSTAT_BREN_SHIFT                  11
#define SDHC_PRSSTAT_CINS_MASK                   0x10000u
#define SDHC_PRSSTAT_CINS_SHIFT                  16
#define SDHC_PRSSTAT_CLSL_MASK                   0x800000u
#define SDHC_PRSSTAT_CLSL_SHIFT                  23
#define SDHC_PRSSTAT_DLSL_MASK                   0xFF000000u
#define SDHC_PRSSTAT_DLSL_SHIFT                  24
#define SDHC_PRSSTAT_DLSL(x)                     (((uint32_t)(((uint32_t)(x))<<SDHC_PRSSTAT_DLSL_SHIFT))&SDHC_PRSSTAT_DLSL_MASK)
/* PROCTL Bit Fields */
#define SDHC_PROCTL_LCTL_MASK                    0x1u
#define SDHC_PROCTL_LCTL_SHIFT                   0
#define SDHC_PROCTL_DTW_MASK                     0x6u
#define SDHC_PROCTL_DTW_SHIFT                    1
#define SDHC_PROCTL_DTW(x)                       (((uint32_t)(((uint32_t)(x))<<SDHC_PROCTL_DTW_SHIFT))&SDHC_PROCTL_DTW_MASK)
#define SDHC_PROCTL_D3CD_MASK                    0x8u
#define SDHC_PROCTL_D3CD_SHIFT                   3
#define SDHC_PROCTL_EMODE_MASK                   0x30u
#define SDHC_PROCTL_EMODE_SHIFT                  4
#define SDHC_PROCTL_EMODE(x)                     (((uint32_t)(((uint32_t)(x))<<SDHC_PROCTL_EMODE_SHIFT))&SDHC_PROCTL_EMODE_MASK)
#define SDHC_PROCTL_CDTL_MASK                    0x40u
#define SDHC_PROCTL_CDTL_SHIFT                   6
#define SDHC_PROCTL_CDSS_MASK                    0x80u
#define SDHC_PROCTL_CDSS_SHIFT                   7
#define SDHC_PROCTL_DMAS_MASK                    0x300u
#define SDHC_PROCTL_DMAS_SHIFT                   8
#define SDHC_PROCTL_DMAS(x)                      (((uint32_t)(((uint32_t)(x))<<SDHC_PROCTL_DMAS_SHIFT))&SDHC_PROCTL_DMAS_MASK)
#define SDHC_PROCTL_SABGREQ_MASK                 0x10000u
#define SDHC_PROCTL_SABGREQ_SHIFT                16
#define SDHC_PROCTL_CREQ_MASK                    0x20000u
#define SDHC_PROCTL_CREQ_SHIFT                   17
#define SDHC_PROCTL_RWCTL_MASK                   0x40000u
#define SDHC_PROCTL_RWCTL_SHIFT                  18
#define SDHC_PROCTL_IABG_MASK                    0x80000u
#define SDHC_PROCTL_IABG_SHIFT                   19
#define SDHC_PROCTL_WECINT_MASK                  0x1000000u
#define SDHC_PROCTL_WECINT_SHIFT                 24
#define SDHC_PROCTL_WECINS_MASK                  0x2000000u
#define SDHC_PROCTL_WECINS_SHIFT                 25
#define SDHC_PROCTL_WECRM_MASK                   0x4000000u
#define SDHC_PROCTL_WECRM_SHIFT                  26
/* SYSCTL Bit Fields */
#define SDHC_SYSCTL_IPGEN_MASK                   0x1u
#define SDHC_SYSCTL_IPGEN_SHIFT                  0
#define SDHC_SYSCTL_HCKEN_MASK                   0x2u
#define SDHC_SYSCTL_HCKEN_SHIFT                  1
#define SDHC_SYSCTL_PEREN_MASK                   0x4u
#define SDHC_SYSCTL_PEREN_SHIFT                  2
#define SDHC_SYSCTL_SDCLKEN_MASK                 0x8u
#define SDHC_SYSCTL_SDCLKEN_SHIFT                3
#define SDHC_SYSCTL_DVS_MASK                     0xF0u
#define SDHC_SYSCTL_DVS_SHIFT                    4
#define SDHC_SYSCTL_DVS(x)                       (((uint32_t)(((uint32_t)(x))<<SDHC_SYSCTL_DVS_SHIFT))&SDHC_SYSCTL_DVS_MASK)
#define SDHC_SYSCTL_SDCLKFS_MASK                 0xFF00u
#define SDHC_SYSCTL_SDCLKFS_SHIFT                8
#define SDHC_SYSCTL_SDCLKFS(x)                   (((uint32_t)(((uint32_t)(x))<<SDHC_SYSCTL_SDCLKFS_SHIFT))&SDHC_SYSCTL_SDCLKFS_MASK)
#define SDHC_SYSCTL_DTOCV_MASK                   0xF0000u
#define SDHC_SYSCTL_DTOCV_SHIFT                  16
#define SDHC_SYSCTL_DTOCV(x)                     (((uint32_t)(((uint32_t)(x))<<SDHC_SYSCTL_DTOCV_SHIFT))&SDHC_SYSCTL_DTOCV_MASK)
#define SDHC_SYSCTL_RSTA_MASK                    0x1000000u
#define SDHC_SYSCTL_RSTA_SHIFT                   24
#define SDHC_SYSCTL_RSTC_MASK                    0x2000000u
#define SDHC_SYSCTL_RSTC_SHIFT                   25
#define SDHC_SYSCTL_RSTD_MASK                    0x4000000u
#define SDHC_SYSCTL_RSTD_SHIFT                   26
#define SDHC_SYSCTL_INITA_MASK                   0x8000000u
#define SDHC_SYSCTL_INITA_SHIFT                  27
/* IRQSTAT Bit Fields */
#define SDHC_IRQSTAT_CC_MASK                     0x1u
#define SDHC_IRQSTAT_CC_SHIFT                    0
#define SDHC_IRQSTAT_TC_MASK                     0x2u
#define SDHC_IRQSTAT_TC_SHIFT                    1
#define SDHC_IRQSTAT_BGE_MASK                    0x4u
#define SDHC_IRQSTAT_BGE_SHIFT                   2
#define SDHC_IRQSTAT_DINT_MASK                   0x8u
#define SDHC_IRQSTAT_DINT_SHIFT                  3
#define SDHC_IRQSTAT_BWR_MASK                    0x10u
#define SDHC_IRQSTAT_BWR_SHIFT                   4
#define SDHC_IRQSTAT_BRR_MASK                    0x20u
#define SDHC_IRQSTAT_BRR_SHIFT                   5
#define SDHC_IRQSTAT_CINS_MASK                   0x40u
#define SDHC_IRQSTAT_CINS_SHIFT                  6
#define SDHC_IRQSTAT_CRM_MASK                    0x80u
#define SDHC_IRQSTAT_CRM_SHIFT                   7
#define SDHC_IRQSTAT_CINT_MASK                   0x100u
#define SDHC_IRQSTAT_CINT_SHIFT                  8
#define SDHC_IRQSTAT_CTOE_MASK                   0x10000u
#define SDHC_IRQSTAT_CTOE_SHIFT                  16
#define SDHC_IRQSTAT_CCE_MASK                    0x20000u
#define SDHC_IRQSTAT_CCE_SHIFT                   17
#define SDHC_IRQSTAT_CEBE_MASK                   0x40000u
#define SDHC_IRQSTAT_CEBE_SHIFT                  18
#define SDHC_IRQSTAT_CIE_MASK                    0x80000u
#define SDHC_IRQSTAT_CIE_SHIFT                   19
#define SDHC_IRQSTAT_DTOE_MASK                   0x100000u
#define SDHC_IRQSTAT_DTOE_SHIFT                  20
#define SDHC_IRQSTAT_DCE_MASK                    0x200000u
#define SDHC_IRQSTAT_DCE_SHIFT                   21
#define SDHC_IRQSTAT_DEBE_MASK                   0x400000u
#define SDHC_IRQSTAT_DEBE_SHIFT                  22
#define SDHC_IRQSTAT_AC12E_MASK                  0x1000000u
#define SDHC_IRQSTAT_AC12E_SHIFT                 24
#define SDHC_IRQSTAT_DMAE_MASK                   0x10000000u
#define SDHC_IRQSTAT_DMAE_SHIFT                  28
/* IRQSTATEN Bit Fields */
#define SDHC_IRQSTATEN_CCSEN_MASK                0x1u
#define SDHC_IRQSTATEN_CCSEN_SHIFT               0
#define SDHC_IRQSTATEN_TCSEN_MASK                0x2u
#define SDHC_IRQSTATEN_TCSEN_SHIFT               1
#define SDHC_IRQSTATEN_BGESEN_MASK               0x4u
#define SDHC_IRQSTATEN_BGESEN_SHIFT              2
#define SDHC_IRQSTATEN_DINTSEN_MASK              0x8u
#define SDHC_IRQSTATEN_DINTSEN_SHIFT             3
#define SDHC_IRQSTATEN_BWRSEN_MASK               0x10u
#define SDHC_IRQSTATEN_BWRSEN_SHIFT              4
#define SDHC_IRQSTATEN_BRRSEN_MASK               0x20u
#define SDHC_IRQSTATEN_BRRSEN_SHIFT              5
#define SDHC_IRQSTATEN_CINSEN_MASK               0x40u
#define SDHC_IRQSTATEN_CINSEN_SHIFT              6
#define SDHC_IRQSTATEN_CRMSEN_MASK               0x80u
#define SDHC_IRQSTATEN_CRMSEN_SHIFT              7
#define SDHC_IRQSTATEN_CINTSEN_MASK              0x100u
#define SDHC_IRQSTATEN_CINTSEN_SHIFT             8
#define SDHC_IRQSTATEN_CTOESEN_MASK              0x10000u
#define SDHC_IRQSTATEN_CTOESEN_SHIFT             16
#define SDHC_IRQSTATEN_CCESEN_MASK               0x20000u
#define SDHC_IRQSTATEN_CCESEN_SHIFT              17
#define SDHC_IRQSTATEN_CEBESEN_MASK              0x40000u
#define SDHC_IRQSTATEN_CEBESEN_SHIFT             18
#define SDHC_IRQSTATEN_CIESEN_MASK               0x80000u
#define SDHC_IRQSTATEN_CIESEN_SHIFT              19
#define SDHC_IRQSTATEN_DTOESEN_MASK              0x100000u
#define SDHC_IRQSTATEN_DTOESEN_SHIFT             20
#define SDHC_IRQSTATEN_DCESEN_MASK               0x200000u
#define SDHC_IRQSTATEN_DCESEN_SHIFT              21
#define SDHC_IRQSTATEN_DEBESEN_MASK              0x400000u
#define SDHC_IRQSTATEN_DEBESEN_SHIFT             22
#define SDHC_IRQSTATEN_AC12ESEN_MASK             0x1000000u
#define SDHC_IRQSTATEN_AC12ESEN_SHIFT            24
#define SDHC_IRQSTATEN_DMAESEN_MASK              0x10000000u
#define SDHC_IRQSTATEN_DMAESEN_SHIFT             28
/* IRQSIGEN Bit Fields */
#define SDHC_IRQSIGEN_CCIEN_MASK                 0x1u
#define SDHC_IRQSIGEN_CCIEN_SHIFT                0
#define SDHC_IRQSIGEN_TCIEN_MASK                 0x2u
#define SDHC_IRQSIGEN_TCIEN_SHIFT                1
#define SDHC_IRQSIGEN_BGEIEN_MASK                0x4u
#define SDHC_IRQSIGEN_BGEIEN_SHIFT               2
#define SDHC_IRQSIGEN_DINTIEN_MASK               0x8u
#define SDHC_IRQSIGEN_DINTIEN_SHIFT              3
#define SDHC_IRQSIGEN_BWRIEN_MASK                0x10u
#define SDHC_IRQSIGEN_BWRIEN_SHIFT               4
#define SDHC_IRQSIGEN_BRRIEN_MASK                0x20u
#define SDHC_IRQSIGEN_BRRIEN_SHIFT               5
#define SDHC_IRQSIGEN_CINSIEN_MASK               0x40u
#define SDHC_IRQSIGEN_CINSIEN_SHIFT              6
#define SDHC_IRQSIGEN_CRMIEN_MASK                0x80u
#define SDHC_IRQSIGEN_CRMIEN_SHIFT               7
#define SDHC_IRQSIGEN_CINTIEN_MASK               0x100u
#define SDHC_IRQSIGEN_CINTIEN_SHIFT              8
#define SDHC_IRQSIGEN_CTOEIEN_MASK               0x10000u
#define SDHC_IRQSIGEN_CTOEIEN_SHIFT              16
#define SDHC_IRQSIGEN_CCEIEN_MASK                0x20000u
#define SDHC_IRQSIGEN_CCEIEN_SHIFT               17
#define SDHC_IRQSIGEN_CEBEIEN_MASK               0x40000u
#define SDHC_IRQSIGEN_CEBEIEN_SHIFT              18
#define SDHC_IRQSIGEN_CIEIEN_MASK                0x80000u
#define SDHC_IRQSIGEN_CIEIEN_SHIFT               19
#define SDHC_IRQSIGEN_DTOEIEN_MASK               0x100000u
#define SDHC_IRQSIGEN_DTOEIEN_SHIFT              20
#define SDHC_IRQSIGEN_DCEIEN_MASK                0x200000u
#define SDHC_IRQSIGEN_DCEIEN_SHIFT               21
#define SDHC_IRQSIGEN_DEBEIEN_MASK               0x400000u
#define SDHC_IRQSIGEN_DEBEIEN_SHIFT              22
#define SDHC_IRQSIGEN_AC12EIEN_MASK              0x1000000u
#define SDHC_IRQSIGEN_AC12EIEN_SHIFT             24
#define SDHC_IRQSIGEN_DMAEIEN_MASK               0x10000000u
#define SDHC_IRQSIGEN_DMAEIEN_SHIFT              28
/* AC12ERR Bit Fields */
#define SDHC_AC12ERR_AC12NE_MASK                 0x1u
#define SDHC_AC12ERR_AC12NE_SHIFT                0
#define SDHC_AC12ERR_AC12TOE_MASK                0x2u
#define SDHC_AC12ERR_AC12TOE_SHIFT               1
#define SDHC_AC12ERR_AC12EBE_MASK                0x4u
#define SDHC_AC12ERR_AC12EBE_SHIFT               2
#define SDHC_AC12ERR_AC12CE_MASK                 0x8u
#define SDHC_AC12ERR_AC12CE_SHIFT                3
#define SDHC_AC12ERR_AC12IE_MASK                 0x10u
#define SDHC_AC12ERR_AC12IE_SHIFT                4
#define SDHC_AC12ERR_CNIBAC12E_MASK              0x80u
#define SDHC_AC12ERR_CNIBAC12E_SHIFT             7
/* HTCAPBLT Bit Fields */
#define SDHC_HTCAPBLT_MBL_MASK                   0x70000u
#define SDHC_HTCAPBLT_MBL_SHIFT                  16
#define SDHC_HTCAPBLT_MBL(x)                     (((uint32_t)(((uint32_t)(x))<<SDHC_HTCAPBLT_MBL_SHIFT))&SDHC_HTCAPBLT_MBL_MASK)
#define SDHC_HTCAPBLT_ADMAS_MASK                 0x100000u
#define SDHC_HTCAPBLT_ADMAS_SHIFT                20
#define SDHC_HTCAPBLT_HSS_MASK                   0x200000u
#define SDHC_HTCAPBLT_HSS_SHIFT                  21
#define SDHC_HTCAPBLT_DMAS_MASK                  0x400000u
#define SDHC_HTCAPBLT_DMAS_SHIFT                 22
#define SDHC_HTCAPBLT_SRS_MASK                   0x800000u
#define SDHC_HTCAPBLT_SRS_SHIFT                  23
#define SDHC_HTCAPBLT_VS33_MASK                  0x1000000u
#define SDHC_HTCAPBLT_VS33_SHIFT                 24
#define SDHC_HTCAPBLT_VS30_MASK                  0x2000000u
#define SDHC_HTCAPBLT_VS30_SHIFT                 25
#define SDHC_HTCAPBLT_VS18_MASK                  0x4000000u
#define SDHC_HTCAPBLT_VS18_SHIFT                 26
/* WML Bit Fields */
#define SDHC_WML_RDWML_MASK                      0xFFu
#define SDHC_WML_RDWML_SHIFT                     0
#define SDHC_WML_RDWML(x)                        (((uint32_t)(((uint32_t)(x))<<SDHC_WML_RDWML_SHIFT))&SDHC_WML_RDWML_MASK)
#define SDHC_WML_WRWML_MASK                      0xFF0000u
#define SDHC_WML_WRWML_SHIFT                     16
#define SDHC_WML_WRWML(x)                        (((uint32_t)(((uint32_t)(x))<<SDHC_WML_WRWML_SHIFT))&SDHC_WML_WRWML_MASK)
/* FEVT Bit Fields */
#define SDHC_FEVT_AC12NE_MASK                    0x1u
#define SDHC_FEVT_AC12NE_SHIFT                   0
#define SDHC_FEVT_AC12TOE_MASK                   0x2u
#define SDHC_FEVT_AC12TOE_SHIFT                  1
#define SDHC_FEVT_AC12CE_MASK                    0x4u
#define SDHC_FEVT_AC12CE_SHIFT                   2
#define SDHC_FEVT_AC12EBE_MASK                   0x8u
#define SDHC_FEVT_AC12EBE_SHIFT                  3
#define SDHC_FEVT_AC12IE_MASK                    0x10u
#define SDHC_FEVT_AC12IE_SHIFT                   4
#define SDHC_FEVT_CNIBAC12E_MASK                 0x80u
#define SDHC_FEVT_CNIBAC12E_SHIFT                7
#define SDHC_FEVT_CTOE_MASK                      0x10000u
#define SDHC_FEVT_CTOE_SHIFT                     16
#define SDHC_FEVT_CCE_MASK                       0x20000u
#define SDHC_FEVT_CCE_SHIFT                      17
#define SDHC_FEVT_CEBE_MASK                      0x40000u
#define SDHC_FEVT_CEBE_SHIFT                     18
#define SDHC_FEVT_CIE_MASK                       0x80000u
#define SDHC_FEVT_CIE_SHIFT                      19
#define SDHC_FEVT_DTOE_MASK                      0x100000u
#define SDHC_FEVT_DTOE_SHIFT                     20
#define SDHC_FEVT_DCE_MASK                       0x200000u
#define SDHC_FEVT_DCE_SHIFT                      21
#define SDHC_FEVT_DEBE_MASK                      0x400000u
#define SDHC_FEVT_DEBE_SHIFT                     22
#define SDHC_FEVT_AC12E_MASK                     0x1000000u
#define SDHC_FEVT_AC12E_SHIFT                    24
#define SDHC_FEVT_DMAE_MASK                      0x10000000u
#define SDHC_FEVT_DMAE_SHIFT                     28
#define SDHC_FEVT_CINT_MASK                      0x80000000u
#define SDHC_FEVT_CINT_SHIFT                     31
/* ADMAES Bit Fields */
#define SDHC_ADMAES_ADMAES_MASK                  0x3u
#define SDHC_ADMAES_ADMAES_SHIFT                 0
#define SDHC_ADMAES_ADMAES(x)                    (((uint32_t)(((uint32_t)(x))<<SDHC_ADMAES_ADMAES_SHIFT))&SDHC_ADMAES_ADMAES_MASK)
#define SDHC_ADMAES_ADMALME_MASK                 0x4u
#define SDHC_ADMAES_ADMALME_SHIFT                2
#define SDHC_ADMAES_ADMADCE_MASK                 0x8u
#define SDHC_ADMAES_ADMADCE_SHIFT                3
/* ADSADDR Bit Fields */
#define SDHC_ADSADDR_ADSADDR_MASK                0xFFFFFFFCu
#define SDHC_ADSADDR_ADSADDR_SHIFT               2
#define SDHC_ADSADDR_ADSADDR(x)                  (((uint32_t)(((uint32_t)(x))<<SDHC_ADSADDR_ADSADDR_SHIFT))&SDHC_ADSADDR_ADSADDR_MASK)
/* VENDOR Bit Fields */
#define SDHC_VENDOR_EXTDMAEN_MASK                0x1u
#define SDHC_VENDOR_EXTDMAEN_SHIFT               0
#define SDHC_VENDOR_EXBLKNU_MASK                 0x2u
#define SDHC_VENDOR_EXBLKNU_SHIFT                1
#define SDHC_VENDOR_INTSTVAL_MASK                0xFF0000u
#define SDHC_VENDOR_INTSTVAL_SHIFT               16
#define SDHC_VENDOR_INTSTVAL(x)                  (((uint32_t)(((uint32_t)(x))<<SDHC_VENDOR_INTSTVAL_SHIFT))&SDHC_VENDOR_INTSTVAL_MASK)
/* MMCBOOT Bit Fields */
#define SDHC_MMCBOOT_DTOCVACK_MASK               0xFu
#define SDHC_MMCBOOT_DTOCVACK_SHIFT              0
#define SDHC_MMCBOOT_DTOCVACK(x)                 (((uint32_t)(((uint32_t)(x))<<SDHC_MMCBOOT_DTOCVACK_SHIFT))&SDHC_MMCBOOT_DTOCVACK_MASK)
#define SDHC_MMCBOOT_BOOTACK_MASK                0x10u
#define SDHC_MMCBOOT_BOOTACK_SHIFT               4
#define SDHC_MMCBOOT_BOOTMODE_MASK               0x20u
#define SDHC_MMCBOOT_BOOTMODE_SHIFT              5
#define SDHC_MMCBOOT_BOOTEN_MASK                 0x40u
#define SDHC_MMCBOOT_BOOTEN_SHIFT                6
#define SDHC_MMCBOOT_AUTOSABGEN_MASK             0x80u
#define SDHC_MMCBOOT_AUTOSABGEN_SHIFT            7
#define SDHC_MMCBOOT_BOOTBLKCNT_MASK             0xFFFF0000u
#define SDHC_MMCBOOT_BOOTBLKCNT_SHIFT            16
#define SDHC_MMCBOOT_BOOTBLKCNT(x)               (((uint32_t)(((uint32_t)(x))<<SDHC_MMCBOOT_BOOTBLKCNT_SHIFT))&SDHC_MMCBOOT_BOOTBLKCNT_MASK)
/* HOSTVER Bit Fields */
#define SDHC_HOSTVER_SVN_MASK                    0xFFu
#define SDHC_HOSTVER_SVN_SHIFT                   0
#define SDHC_HOSTVER_SVN(x)                      (((uint32_t)(((uint32_t)(x))<<SDHC_HOSTVER_SVN_SHIFT))&SDHC_HOSTVER_SVN_MASK)
#define SDHC_HOSTVER_VVN_MASK                    0xFF00u
#define SDHC_HOSTVER_VVN_SHIFT                   8
#define SDHC_HOSTVER_VVN(x)                      (((uint32_t)(((uint32_t)(x))<<SDHC_HOSTVER_VVN_SHIFT))&SDHC_HOSTVER_VVN_MASK)

/**
 * @}
 */ /* end of group SDHC_Register_Masks */


/* SDHC - Peripheral instance base addresses */
/** Peripheral SDHC base address */
#define SDHC_BASE                                (0x400B1000u)
/** Peripheral SDHC base pointer */
#define SDHC                                     ((SDHC_Type *)SDHC_BASE)
/** Array initializer of SDHC peripheral base pointers */
#define SDHC_BASES                               { SDHC }

/**
 * @}
 */ /* end of group SDHC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- SIM Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup SIM_Peripheral_Access_Layer SIM Peripheral Access Layer
 * @{
 */

/** SIM - Register Layout Typedef */
typedef struct {
  __IO uint32_t SOPT1;                             /**< System Options Register 1, offset: 0x0 */
  __IO uint32_t SOPT1CFG;                          /**< SOPT1 Configuration Register, offset: 0x4 */
       uint8_t RESERVED_0[4092];
  __IO uint32_t SOPT2;                             /**< System Options Register 2, offset: 0x1004 */
       uint8_t RESERVED_1[4];
  __IO uint32_t SOPT4;                             /**< System Options Register 4, offset: 0x100C */
  __IO uint32_t SOPT5;                             /**< System Options Register 5, offset: 0x1010 */
       uint8_t RESERVED_2[4];
  __IO uint32_t SOPT7;                             /**< System Options Register 7, offset: 0x1018 */
       uint8_t RESERVED_3[8];
  __I  uint32_t SDID;                              /**< System Device Identification Register, offset: 0x1024 */
  __IO uint32_t SCGC1;                             /**< System Clock Gating Control Register 1, offset: 0x1028 */
  __IO uint32_t SCGC2;                             /**< System Clock Gating Control Register 2, offset: 0x102C */
  __IO uint32_t SCGC3;                             /**< System Clock Gating Control Register 3, offset: 0x1030 */
  __IO uint32_t SCGC4;                             /**< System Clock Gating Control Register 4, offset: 0x1034 */
  __IO uint32_t SCGC5;                             /**< System Clock Gating Control Register 5, offset: 0x1038 */
  __IO uint32_t SCGC6;                             /**< System Clock Gating Control Register 6, offset: 0x103C */
  __IO uint32_t SCGC7;                             /**< System Clock Gating Control Register 7, offset: 0x1040 */
  __IO uint32_t CLKDIV1;                           /**< System Clock Divider Register 1, offset: 0x1044 */
  __IO uint32_t CLKDIV2;                           /**< System Clock Divider Register 2, offset: 0x1048 */
  __IO uint32_t FCFG1;                             /**< Flash Configuration Register 1, offset: 0x104C */
  __I  uint32_t FCFG2;                             /**< Flash Configuration Register 2, offset: 0x1050 */
  __I  uint32_t UIDH;                              /**< Unique Identification Register High, offset: 0x1054 */
  __I  uint32_t UIDMH;                             /**< Unique Identification Register Mid-High, offset: 0x1058 */
  __I  uint32_t UIDML;                             /**< Unique Identification Register Mid Low, offset: 0x105C */
  __I  uint32_t UIDL;                              /**< Unique Identification Register Low, offset: 0x1060 */
} SIM_Type;

/* ----------------------------------------------------------------------------
   -- SIM Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup SIM_Register_Masks SIM Register Masks
 * @{
 */

/* SOPT1 Bit Fields */
#define SIM_SOPT1_RAMSIZE_MASK                   0xF000u
#define SIM_SOPT1_RAMSIZE_SHIFT                  12
#define SIM_SOPT1_RAMSIZE(x)                     (((uint32_t)(((uint32_t)(x))<<SIM_SOPT1_RAMSIZE_SHIFT))&SIM_SOPT1_RAMSIZE_MASK)
#define SIM_SOPT1_OSC32KSEL_MASK                 0xC0000u
#define SIM_SOPT1_OSC32KSEL_SHIFT                18
#define SIM_SOPT1_OSC32KSEL(x)                   (((uint32_t)(((uint32_t)(x))<<SIM_SOPT1_OSC32KSEL_SHIFT))&SIM_SOPT1_OSC32KSEL_MASK)
#define SIM_SOPT1_USBVSTBY_MASK                  0x20000000u
#define SIM_SOPT1_USBVSTBY_SHIFT                 29
#define SIM_SOPT1_USBSSTBY_MASK                  0x40000000u
#define SIM_SOPT1_USBSSTBY_SHIFT                 30
#define SIM_SOPT1_USBREGEN_MASK                  0x80000000u
#define SIM_SOPT1_USBREGEN_SHIFT                 31
/* SOPT1CFG Bit Fields */
#define SIM_SOPT1CFG_URWE_MASK                   0x1000000u
#define SIM_SOPT1CFG_URWE_SHIFT                  24
#define SIM_SOPT1CFG_UVSWE_MASK                  0x2000000u
#define SIM_SOPT1CFG_UVSWE_SHIFT                 25
#define SIM_SOPT1CFG_USSWE_MASK                  0x4000000u
#define SIM_SOPT1CFG_USSWE_SHIFT                 26
/* SOPT2 Bit Fields */
#define SIM_SOPT2_RTCCLKOUTSEL_MASK              0x10u
#define SIM_SOPT2_RTCCLKOUTSEL_SHIFT             4
#define SIM_SOPT2_CLKOUTSEL_MASK                 0xE0u
#define SIM_SOPT2_CLKOUTSEL_SHIFT                5
#define SIM_SOPT2_CLKOUTSEL(x)                   (((uint32_t)(((uint32_t)(x))<<SIM_SOPT2_CLKOUTSEL_SHIFT))&SIM_SOPT2_CLKOUTSEL_MASK)
#define SIM_SOPT2_FBSL_MASK                      0x300u
#define SIM_SOPT2_FBSL_SHIFT                     8
#define SIM_SOPT2_FBSL(x)                        (((uint32_t)(((uint32_t)(x))<<SIM_SOPT2_FBSL_SHIFT))&SIM_SOPT2_FBSL_MASK)
#define SIM_SOPT2_PTD7PAD_MASK                   0x800u
#define SIM_SOPT2_PTD7PAD_SHIFT                  11
#define SIM_SOPT2_TRACECLKSEL_MASK               0x1000u
#define SIM_SOPT2_TRACECLKSEL_SHIFT              12
#define SIM_SOPT2_PLLFLLSEL_MASK                 0x10000u
#define SIM_SOPT2_PLLFLLSEL_SHIFT                16
#define SIM_SOPT2_USBSRC_MASK                    0x40000u
#define SIM_SOPT2_USBSRC_SHIFT                   18
#define SIM_SOPT2_RMIISRC_MASK                   0x80000u
#define SIM_SOPT2_RMIISRC_SHIFT                  19
#define SIM_SOPT2_TIMESRC_MASK                   0x300000u
#define SIM_SOPT2_TIMESRC_SHIFT                  20
#define SIM_SOPT2_TIMESRC(x)                     (((uint32_t)(((uint32_t)(x))<<SIM_SOPT2_TIMESRC_SHIFT))&SIM_SOPT2_TIMESRC_MASK)
#define SIM_SOPT2_SDHCSRC_MASK                   0x30000000u
#define SIM_SOPT2_SDHCSRC_SHIFT                  28
#define SIM_SOPT2_SDHCSRC(x)                     (((uint32_t)(((uint32_t)(x))<<SIM_SOPT2_SDHCSRC_SHIFT))&SIM_SOPT2_SDHCSRC_MASK)
/* SOPT4 Bit Fields */
#define SIM_SOPT4_FTM0FLT0_MASK                  0x1u
#define SIM_SOPT4_FTM0FLT0_SHIFT                 0
#define SIM_SOPT4_FTM0FLT1_MASK                  0x2u
#define SIM_SOPT4_FTM0FLT1_SHIFT                 1
#define SIM_SOPT4_FTM0FLT2_MASK                  0x4u
#define SIM_SOPT4_FTM0FLT2_SHIFT                 2
#define SIM_SOPT4_FTM1FLT0_MASK                  0x10u
#define SIM_SOPT4_FTM1FLT0_SHIFT                 4
#define SIM_SOPT4_FTM2FLT0_MASK                  0x100u
#define SIM_SOPT4_FTM2FLT0_SHIFT                 8
#define SIM_SOPT4_FTM1CH0SRC_MASK                0xC0000u
#define SIM_SOPT4_FTM1CH0SRC_SHIFT               18
#define SIM_SOPT4_FTM1CH0SRC(x)                  (((uint32_t)(((uint32_t)(x))<<SIM_SOPT4_FTM1CH0SRC_SHIFT))&SIM_SOPT4_FTM1CH0SRC_MASK)
#define SIM_SOPT4_FTM2CH0SRC_MASK                0x300000u
#define SIM_SOPT4_FTM2CH0SRC_SHIFT               20
#define SIM_SOPT4_FTM2CH0SRC(x)                  (((uint32_t)(((uint32_t)(x))<<SIM_SOPT4_FTM2CH0SRC_SHIFT))&SIM_SOPT4_FTM2CH0SRC_MASK)
#define SIM_SOPT4_FTM0CLKSEL_MASK                0x1000000u
#define SIM_SOPT4_FTM0CLKSEL_SHIFT               24
#define SIM_SOPT4_FTM1CLKSEL_MASK                0x2000000u
#define SIM_SOPT4_FTM1CLKSEL_SHIFT               25
#define SIM_SOPT4_FTM2CLKSEL_MASK                0x4000000u
#define SIM_SOPT4_FTM2CLKSEL_SHIFT               26
#define SIM_SOPT4_FTM0TRG0SRC_MASK               0x10000000u
#define SIM_SOPT4_FTM0TRG0SRC_SHIFT              28
#define SIM_SOPT4_FTM0TRG1SRC_MASK               0x20000000u
#define SIM_SOPT4_FTM0TRG1SRC_SHIFT              29
/* SOPT5 Bit Fields */
#define SIM_SOPT5_UART0TXSRC_MASK                0x3u
#define SIM_SOPT5_UART0TXSRC_SHIFT               0
#define SIM_SOPT5_UART0TXSRC(x)                  (((uint32_t)(((uint32_t)(x))<<SIM_SOPT5_UART0TXSRC_SHIFT))&SIM_SOPT5_UART0TXSRC_MASK)
#define SIM_SOPT5_UART0RXSRC_MASK                0xCu
#define SIM_SOPT5_UART0RXSRC_SHIFT               2
#define SIM_SOPT5_UART0RXSRC(x)                  (((uint32_t)(((uint32_t)(x))<<SIM_SOPT5_UART0RXSRC_SHIFT))&SIM_SOPT5_UART0RXSRC_MASK)
#define SIM_SOPT5_UART1TXSRC_MASK                0x30u
#define SIM_SOPT5_UART1TXSRC_SHIFT               4
#define SIM_SOPT5_UART1TXSRC(x)                  (((uint32_t)(((uint32_t)(x))<<SIM_SOPT5_UART1TXSRC_SHIFT))&SIM_SOPT5_UART1TXSRC_MASK)
#define SIM_SOPT5_UART1RXSRC_MASK                0xC0u
#define SIM_SOPT5_UART1RXSRC_SHIFT               6
#define SIM_SOPT5_UART1RXSRC(x)                  (((uint32_t)(((uint32_t)(x))<<SIM_SOPT5_UART1RXSRC_SHIFT))&SIM_SOPT5_UART1RXSRC_MASK)
/* SOPT7 Bit Fields */
#define SIM_SOPT7_ADC0TRGSEL_MASK                0xFu
#define SIM_SOPT7_ADC0TRGSEL_SHIFT               0
#define SIM_SOPT7_ADC0TRGSEL(x)                  (((uint32_t)(((uint32_t)(x))<<SIM_SOPT7_ADC0TRGSEL_SHIFT))&SIM_SOPT7_ADC0TRGSEL_MASK)
#define SIM_SOPT7_ADC0PRETRGSEL_MASK             0x10u
#define SIM_SOPT7_ADC0PRETRGSEL_SHIFT            4
#define SIM_SOPT7_ADC0ALTTRGEN_MASK              0x80u
#define SIM_SOPT7_ADC0ALTTRGEN_SHIFT             7
#define SIM_SOPT7_ADC1TRGSEL_MASK                0xF00u
#define SIM_SOPT7_ADC1TRGSEL_SHIFT               8
#define SIM_SOPT7_ADC1TRGSEL(x)                  (((uint32_t)(((uint32_t)(x))<<SIM_SOPT7_ADC1TRGSEL_SHIFT))&SIM_SOPT7_ADC1TRGSEL_MASK)
#define SIM_SOPT7_ADC1PRETRGSEL_MASK             0x1000u
#define SIM_SOPT7_ADC1PRETRGSEL_SHIFT            12
#define SIM_SOPT7_ADC1ALTTRGEN_MASK              0x8000u
#define SIM_SOPT7_ADC1ALTTRGEN_SHIFT             15
/* SDID Bit Fields */
#define SIM_SDID_PINID_MASK                      0xFu
#define SIM_SDID_PINID_SHIFT                     0
#define SIM_SDID_PINID(x)                        (((uint32_t)(((uint32_t)(x))<<SIM_SDID_PINID_SHIFT))&SIM_SDID_PINID_MASK)
#define SIM_SDID_FAMID_MASK                      0x70u
#define SIM_SDID_FAMID_SHIFT                     4
#define SIM_SDID_FAMID(x)                        (((uint32_t)(((uint32_t)(x))<<SIM_SDID_FAMID_SHIFT))&SIM_SDID_FAMID_MASK)
#define SIM_SDID_REVID_MASK                      0xF000u
#define SIM_SDID_REVID_SHIFT                     12
#define SIM_SDID_REVID(x)                        (((uint32_t)(((uint32_t)(x))<<SIM_SDID_REVID_SHIFT))&SIM_SDID_REVID_MASK)
/* SCGC1 Bit Fields */
#define SIM_SCGC1_UART4_MASK                     0x400u
#define SIM_SCGC1_UART4_SHIFT                    10
#define SIM_SCGC1_UART5_MASK                     0x800u
#define SIM_SCGC1_UART5_SHIFT                    11
/* SCGC2 Bit Fields */
#define SIM_SCGC2_ENET_MASK                      0x1u
#define SIM_SCGC2_ENET_SHIFT                     0
#define SIM_SCGC2_DAC0_MASK                      0x1000u
#define SIM_SCGC2_DAC0_SHIFT                     12
#define SIM_SCGC2_DAC1_MASK                      0x2000u
#define SIM_SCGC2_DAC1_SHIFT                     13
/* SCGC3 Bit Fields */
#define SIM_SCGC3_RNGA_MASK                      0x1u
#define SIM_SCGC3_RNGA_SHIFT                     0
#define SIM_SCGC3_FLEXCAN1_MASK                  0x10u
#define SIM_SCGC3_FLEXCAN1_SHIFT                 4
#define SIM_SCGC3_SPI2_MASK                      0x1000u
#define SIM_SCGC3_SPI2_SHIFT                     12
#define SIM_SCGC3_SDHC_MASK                      0x20000u
#define SIM_SCGC3_SDHC_SHIFT                     17
#define SIM_SCGC3_FTM2_MASK                      0x1000000u
#define SIM_SCGC3_FTM2_SHIFT                     24
#define SIM_SCGC3_ADC1_MASK                      0x8000000u
#define SIM_SCGC3_ADC1_SHIFT                     27
/* SCGC4 Bit Fields */
#define SIM_SCGC4_EWM_MASK                       0x2u
#define SIM_SCGC4_EWM_SHIFT                      1
#define SIM_SCGC4_CMT_MASK                       0x4u
#define SIM_SCGC4_CMT_SHIFT                      2
#define SIM_SCGC4_I2C0_MASK                      0x40u
#define SIM_SCGC4_I2C0_SHIFT                     6
#define SIM_SCGC4_I2C1_MASK                      0x80u
#define SIM_SCGC4_I2C1_SHIFT                     7
#define SIM_SCGC4_UART0_MASK                     0x400u
#define SIM_SCGC4_UART0_SHIFT                    10
#define SIM_SCGC4_UART1_MASK                     0x800u
#define SIM_SCGC4_UART1_SHIFT                    11
#define SIM_SCGC4_UART2_MASK                     0x1000u
#define SIM_SCGC4_UART2_SHIFT                    12
#define SIM_SCGC4_UART3_MASK                     0x2000u
#define SIM_SCGC4_UART3_SHIFT                    13
#define SIM_SCGC4_USBOTG_MASK                    0x40000u
#define SIM_SCGC4_USBOTG_SHIFT                   18
#define SIM_SCGC4_CMP_MASK                       0x80000u
#define SIM_SCGC4_CMP_SHIFT                      19
#define SIM_SCGC4_VREF_MASK                      0x100000u
#define SIM_SCGC4_VREF_SHIFT                     20
#define SIM_SCGC4_LLWU_MASK                      0x10000000u
#define SIM_SCGC4_LLWU_SHIFT                     28
/* SCGC5 Bit Fields */
#define SIM_SCGC5_LPTIMER_MASK                   0x1u
#define SIM_SCGC5_LPTIMER_SHIFT                  0
#define SIM_SCGC5_TSI_MASK                       0x20u
#define SIM_SCGC5_TSI_SHIFT                      5
#define SIM_SCGC5_PORTA_MASK                     0x200u
#define SIM_SCGC5_PORTA_SHIFT                    9
#define SIM_SCGC5_PORTB_MASK                     0x400u
#define SIM_SCGC5_PORTB_SHIFT                    10
#define SIM_SCGC5_PORTC_MASK                     0x800u
#define SIM_SCGC5_PORTC_SHIFT                    11
#define SIM_SCGC5_PORTD_MASK                     0x1000u
#define SIM_SCGC5_PORTD_SHIFT                    12
#define SIM_SCGC5_PORTE_MASK                     0x2000u
#define SIM_SCGC5_PORTE_SHIFT                    13
/* SCGC6 Bit Fields */
#define SIM_SCGC6_FTFL_MASK                      0x1u
#define SIM_SCGC6_FTFL_SHIFT                     0
#define SIM_SCGC6_DMAMUX_MASK                    0x2u
#define SIM_SCGC6_DMAMUX_SHIFT                   1
#define SIM_SCGC6_FLEXCAN0_MASK                  0x10u
#define SIM_SCGC6_FLEXCAN0_SHIFT                 4
#define SIM_SCGC6_SPI0_MASK                      0x1000u
#define SIM_SCGC6_SPI0_SHIFT                     12
#define SIM_SCGC6_SPI1_MASK                      0x2000u
#define SIM_SCGC6_SPI1_SHIFT                     13
#define SIM_SCGC6_I2S_MASK                       0x8000u
#define SIM_SCGC6_I2S_SHIFT                      15
#define SIM_SCGC6_CRC_MASK                       0x40000u
#define SIM_SCGC6_CRC_SHIFT                      18
#define SIM_SCGC6_USBDCD_MASK                    0x200000u
#define SIM_SCGC6_USBDCD_SHIFT                   21
#define SIM_SCGC6_PDB_MASK                       0x400000u
#define SIM_SCGC6_PDB_SHIFT                      22
#define SIM_SCGC6_PIT_MASK                       0x800000u
#define SIM_SCGC6_PIT_SHIFT                      23
#define SIM_SCGC6_FTM0_MASK                      0x1000000u
#define SIM_SCGC6_FTM0_SHIFT                     24
#define SIM_SCGC6_FTM1_MASK                      0x2000000u
#define SIM_SCGC6_FTM1_SHIFT                     25
#define SIM_SCGC6_ADC0_MASK                      0x8000000u
#define SIM_SCGC6_ADC0_SHIFT                     27
#define SIM_SCGC6_RTC_MASK                       0x20000000u
#define SIM_SCGC6_RTC_SHIFT                      29
/* SCGC7 Bit Fields */
#define SIM_SCGC7_FLEXBUS_MASK                   0x1u
#define SIM_SCGC7_FLEXBUS_SHIFT                  0
#define SIM_SCGC7_DMA_MASK                       0x2u
#define SIM_SCGC7_DMA_SHIFT                      1
#define SIM_SCGC7_MPU_MASK                       0x4u
#define SIM_SCGC7_MPU_SHIFT                      2
/* CLKDIV1 Bit Fields */
#define SIM_CLKDIV1_OUTDIV4_MASK                 0xF0000u
#define SIM_CLKDIV1_OUTDIV4_SHIFT                16
#define SIM_CLKDIV1_OUTDIV4(x)                   (((uint32_t)(((uint32_t)(x))<<SIM_CLKDIV1_OUTDIV4_SHIFT))&SIM_CLKDIV1_OUTDIV4_MASK)
#define SIM_CLKDIV1_OUTDIV3_MASK                 0xF00000u
#define SIM_CLKDIV1_OUTDIV3_SHIFT                20
#define SIM_CLKDIV1_OUTDIV3(x)                   (((uint32_t)(((uint32_t)(x))<<SIM_CLKDIV1_OUTDIV3_SHIFT))&SIM_CLKDIV1_OUTDIV3_MASK)
#define SIM_CLKDIV1_OUTDIV2_MASK                 0xF000000u
#define SIM_CLKDIV1_OUTDIV2_SHIFT                24
#define SIM_CLKDIV1_OUTDIV2(x)                   (((uint32_t)(((uint32_t)(x))<<SIM_CLKDIV1_OUTDIV2_SHIFT))&SIM_CLKDIV1_OUTDIV2_MASK)
#define SIM_CLKDIV1_OUTDIV1_MASK                 0xF0000000u
#define SIM_CLKDIV1_OUTDIV1_SHIFT                28
#define SIM_CLKDIV1_OUTDIV1(x)                   (((uint32_t)(((uint32_t)(x))<<SIM_CLKDIV1_OUTDIV1_SHIFT))&SIM_CLKDIV1_OUTDIV1_MASK)
/* CLKDIV2 Bit Fields */
#define SIM_CLKDIV2_USBFRAC_MASK                 0x1u
#define SIM_CLKDIV2_USBFRAC_SHIFT                0
#define SIM_CLKDIV2_USBDIV_MASK                  0xEu
#define SIM_CLKDIV2_USBDIV_SHIFT                 1
#define SIM_CLKDIV2_USBDIV(x)                    (((uint32_t)(((uint32_t)(x))<<SIM_CLKDIV2_USBDIV_SHIFT))&SIM_CLKDIV2_USBDIV_MASK)
/* FCFG1 Bit Fields */
#define SIM_FCFG1_FLASHDIS_MASK                  0x1u
#define SIM_FCFG1_FLASHDIS_SHIFT                 0
#define SIM_FCFG1_FLASHDOZE_MASK                 0x2u
#define SIM_FCFG1_FLASHDOZE_SHIFT                1
#define SIM_FCFG1_DEPART_MASK                    0xF00u
#define SIM_FCFG1_DEPART_SHIFT                   8
#define SIM_FCFG1_DEPART(x)                      (((uint32_t)(((uint32_t)(x))<<SIM_FCFG1_DEPART_SHIFT))&SIM_FCFG1_DEPART_MASK)
#define SIM_FCFG1_EESIZE_MASK                    0xF0000u
#define SIM_FCFG1_EESIZE_SHIFT                   16
#define SIM_FCFG1_EESIZE(x)                      (((uint32_t)(((uint32_t)(x))<<SIM_FCFG1_EESIZE_SHIFT))&SIM_FCFG1_EESIZE_MASK)
#define SIM_FCFG1_PFSIZE_MASK                    0xF000000u
#define SIM_FCFG1_PFSIZE_SHIFT                   24
#define SIM_FCFG1_PFSIZE(x)                      (((uint32_t)(((uint32_t)(x))<<SIM_FCFG1_PFSIZE_SHIFT))&SIM_FCFG1_PFSIZE_MASK)
#define SIM_FCFG1_NVMSIZE_MASK                   0xF0000000u
#define SIM_FCFG1_NVMSIZE_SHIFT                  28
#define SIM_FCFG1_NVMSIZE(x)                     (((uint32_t)(((uint32_t)(x))<<SIM_FCFG1_NVMSIZE_SHIFT))&SIM_FCFG1_NVMSIZE_MASK)
/* FCFG2 Bit Fields */
#define SIM_FCFG2_MAXADDR1_MASK                  0x7F0000u
#define SIM_FCFG2_MAXADDR1_SHIFT                 16
#define SIM_FCFG2_MAXADDR1(x)                    (((uint32_t)(((uint32_t)(x))<<SIM_FCFG2_MAXADDR1_SHIFT))&SIM_FCFG2_MAXADDR1_MASK)
#define SIM_FCFG2_PFLSH_MASK                     0x800000u
#define SIM_FCFG2_PFLSH_SHIFT                    23
#define SIM_FCFG2_MAXADDR0_MASK                  0x7F000000u
#define SIM_FCFG2_MAXADDR0_SHIFT                 24
#define SIM_FCFG2_MAXADDR0(x)                    (((uint32_t)(((uint32_t)(x))<<SIM_FCFG2_MAXADDR0_SHIFT))&SIM_FCFG2_MAXADDR0_MASK)
#define SIM_FCFG2_SWAPPFLSH_MASK                 0x80000000u
#define SIM_FCFG2_SWAPPFLSH_SHIFT                31
/* UIDH Bit Fields */
#define SIM_UIDH_UID_MASK                        0xFFFFFFFFu
#define SIM_UIDH_UID_SHIFT                       0
#define SIM_UIDH_UID(x)                          (((uint32_t)(((uint32_t)(x))<<SIM_UIDH_UID_SHIFT))&SIM_UIDH_UID_MASK)
/* UIDMH Bit Fields */
#define SIM_UIDMH_UID_MASK                       0xFFFFFFFFu
#define SIM_UIDMH_UID_SHIFT                      0
#define SIM_UIDMH_UID(x)                         (((uint32_t)(((uint32_t)(x))<<SIM_UIDMH_UID_SHIFT))&SIM_UIDMH_UID_MASK)
/* UIDML Bit Fields */
#define SIM_UIDML_UID_MASK                       0xFFFFFFFFu
#define SIM_UIDML_UID_SHIFT                      0
#define SIM_UIDML_UID(x)                         (((uint32_t)(((uint32_t)(x))<<SIM_UIDML_UID_SHIFT))&SIM_UIDML_UID_MASK)
/* UIDL Bit Fields */
#define SIM_UIDL_UID_MASK                        0xFFFFFFFFu
#define SIM_UIDL_UID_SHIFT                       0
#define SIM_UIDL_UID(x)                          (((uint32_t)(((uint32_t)(x))<<SIM_UIDL_UID_SHIFT))&SIM_UIDL_UID_MASK)

/**
 * @}
 */ /* end of group SIM_Register_Masks */


/* SIM - Peripheral instance base addresses */
/** Peripheral SIM base address */
#define SIM_BASE                                 (0x40047000u)
/** Peripheral SIM base pointer */
#define SIM                                      ((SIM_Type *)SIM_BASE)
/** Array initializer of SIM peripheral base pointers */
#define SIM_BASES                                { SIM }

/**
 * @}
 */ /* end of group SIM_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- SMC Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup SMC_Peripheral_Access_Layer SMC Peripheral Access Layer
 * @{
 */

/** SMC - Register Layout Typedef */
typedef struct {
  __IO uint8_t PMPROT;                             /**< Power Mode Protection register, offset: 0x0 */
  __IO uint8_t PMCTRL;                             /**< Power Mode Control register, offset: 0x1 */
  __IO uint8_t VLLSCTRL;                           /**< VLLS Control register, offset: 0x2 */
  __I  uint8_t PMSTAT;                             /**< Power Mode Status register, offset: 0x3 */
} SMC_Type;

/* ----------------------------------------------------------------------------
   -- SMC Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup SMC_Register_Masks SMC Register Masks
 * @{
 */

/* PMPROT Bit Fields */
#define SMC_PMPROT_AVLLS_MASK                    0x2u
#define SMC_PMPROT_AVLLS_SHIFT                   1
#define SMC_PMPROT_ALLS_MASK                     0x8u
#define SMC_PMPROT_ALLS_SHIFT                    3
#define SMC_PMPROT_AVLP_MASK                     0x20u
#define SMC_PMPROT_AVLP_SHIFT                    5
/* PMCTRL Bit Fields */
#define SMC_PMCTRL_STOPM_MASK                    0x7u
#define SMC_PMCTRL_STOPM_SHIFT                   0
#define SMC_PMCTRL_STOPM(x)                      (((uint8_t)(((uint8_t)(x))<<SMC_PMCTRL_STOPM_SHIFT))&SMC_PMCTRL_STOPM_MASK)
#define SMC_PMCTRL_STOPA_MASK                    0x8u
#define SMC_PMCTRL_STOPA_SHIFT                   3
#define SMC_PMCTRL_RUNM_MASK                     0x60u
#define SMC_PMCTRL_RUNM_SHIFT                    5
#define SMC_PMCTRL_RUNM(x)                       (((uint8_t)(((uint8_t)(x))<<SMC_PMCTRL_RUNM_SHIFT))&SMC_PMCTRL_RUNM_MASK)
#define SMC_PMCTRL_LPWUI_MASK                    0x80u
#define SMC_PMCTRL_LPWUI_SHIFT                   7
/* VLLSCTRL Bit Fields */
#define SMC_VLLSCTRL_VLLSM_MASK                  0x7u
#define SMC_VLLSCTRL_VLLSM_SHIFT                 0
#define SMC_VLLSCTRL_VLLSM(x)                    (((uint8_t)(((uint8_t)(x))<<SMC_VLLSCTRL_VLLSM_SHIFT))&SMC_VLLSCTRL_VLLSM_MASK)
#define SMC_VLLSCTRL_RAM2PO_MASK                 0x10u
#define SMC_VLLSCTRL_RAM2PO_SHIFT                4
/* PMSTAT Bit Fields */
#define SMC_PMSTAT_PMSTAT_MASK                   0x7Fu
#define SMC_PMSTAT_PMSTAT_SHIFT                  0
#define SMC_PMSTAT_PMSTAT(x)                     (((uint8_t)(((uint8_t)(x))<<SMC_PMSTAT_PMSTAT_SHIFT))&SMC_PMSTAT_PMSTAT_MASK)

/**
 * @}
 */ /* end of group SMC_Register_Masks */


/* SMC - Peripheral instance base addresses */
/** Peripheral SMC base address */
#define SMC_BASE                                 (0x4007E000u)
/** Peripheral SMC base pointer */
#define SMC                                      ((SMC_Type *)SMC_BASE)
/** Array initializer of SMC peripheral base pointers */
#define SMC_BASES                                { SMC }

/**
 * @}
 */ /* end of group SMC_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- SPI Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup SPI_Peripheral_Access_Layer SPI Peripheral Access Layer
 * @{
 */

/** SPI - Register Layout Typedef */
typedef struct {
  __IO uint32_t MCR;                               /**< Module Configuration Register, offset: 0x0 */
       uint8_t RESERVED_0[4];
  __IO uint32_t TCR;                               /**< Transfer Count Register, offset: 0x8 */
  union {                                          /* offset: 0xC */
    __IO uint32_t CTAR[2];                           /**< DSPI Clock and Transfer Attributes Register (In Master Mode), array offset: 0xC, array step: 0x4 */
    __IO uint32_t CTAR_SLAVE[1];                     /**< Clock and Transfer Attributes Register (In Slave Mode), array offset: 0xC, array step: 0x4 */
  };
       uint8_t RESERVED_1[24];
  __IO uint32_t SR;                                /**< DSPI Status Register, offset: 0x2C */
  __IO uint32_t RSER;                              /**< DMA/Interrupt Request Select and Enable Register, offset: 0x30 */
  union {                                          /* offset: 0x34 */
    __IO uint32_t PUSHR;                             /**< PUSH TX FIFO Register In Master Mode, offset: 0x34 */
    __IO uint32_t PUSHR_SLAVE;                       /**< PUSH TX FIFO Register In Slave Mode, offset: 0x34 */
  };
  __I  uint32_t POPR;                              /**< POP RX FIFO Register, offset: 0x38 */
  __I  uint32_t TXFR0;                             /**< DSPI Transmit FIFO Registers, offset: 0x3C */
  __I  uint32_t TXFR1;                             /**< DSPI Transmit FIFO Registers, offset: 0x40 */
  __I  uint32_t TXFR2;                             /**< DSPI Transmit FIFO Registers, offset: 0x44 */
  __I  uint32_t TXFR3;                             /**< DSPI Transmit FIFO Registers, offset: 0x48 */
       uint8_t RESERVED_2[48];
  __I  uint32_t RXFR0;                             /**< DSPI Receive FIFO Registers, offset: 0x7C */
  __I  uint32_t RXFR1;                             /**< DSPI Receive FIFO Registers, offset: 0x80 */
  __I  uint32_t RXFR2;                             /**< DSPI Receive FIFO Registers, offset: 0x84 */
  __I  uint32_t RXFR3;                             /**< DSPI Receive FIFO Registers, offset: 0x88 */
} SPI_Type;

/* ----------------------------------------------------------------------------
   -- SPI Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup SPI_Register_Masks SPI Register Masks
 * @{
 */

/* MCR Bit Fields */
#define SPI_MCR_HALT_MASK                        0x1u
#define SPI_MCR_HALT_SHIFT                       0
#define SPI_MCR_SMPL_PT_MASK                     0x300u
#define SPI_MCR_SMPL_PT_SHIFT                    8
#define SPI_MCR_SMPL_PT(x)                       (((uint32_t)(((uint32_t)(x))<<SPI_MCR_SMPL_PT_SHIFT))&SPI_MCR_SMPL_PT_MASK)
#define SPI_MCR_CLR_RXF_MASK                     0x400u
#define SPI_MCR_CLR_RXF_SHIFT                    10
#define SPI_MCR_CLR_TXF_MASK                     0x800u
#define SPI_MCR_CLR_TXF_SHIFT                    11
#define SPI_MCR_DIS_RXF_MASK                     0x1000u
#define SPI_MCR_DIS_RXF_SHIFT                    12
#define SPI_MCR_DIS_TXF_MASK                     0x2000u
#define SPI_MCR_DIS_TXF_SHIFT                    13
#define SPI_MCR_MDIS_MASK                        0x4000u
#define SPI_MCR_MDIS_SHIFT                       14
#define SPI_MCR_DOZE_MASK                        0x8000u
#define SPI_MCR_DOZE_SHIFT                       15
#define SPI_MCR_PCSIS_MASK                       0x1F0000u
#define SPI_MCR_PCSIS_SHIFT                      16
#define SPI_MCR_PCSIS(x)                         (((uint32_t)(((uint32_t)(x))<<SPI_MCR_PCSIS_SHIFT))&SPI_MCR_PCSIS_MASK)
#define SPI_MCR_ROOE_MASK                        0x1000000u
#define SPI_MCR_ROOE_SHIFT                       24
#define SPI_MCR_PCSSE_MASK                       0x2000000u
#define SPI_MCR_PCSSE_SHIFT                      25
#define SPI_MCR_MTFE_MASK                        0x4000000u
#define SPI_MCR_MTFE_SHIFT                       26
#define SPI_MCR_FRZ_MASK                         0x8000000u
#define SPI_MCR_FRZ_SHIFT                        27
#define SPI_MCR_DCONF_MASK                       0x30000000u
#define SPI_MCR_DCONF_SHIFT                      28
#define SPI_MCR_DCONF(x)                         (((uint32_t)(((uint32_t)(x))<<SPI_MCR_DCONF_SHIFT))&SPI_MCR_DCONF_MASK)
#define SPI_MCR_CONT_SCKE_MASK                   0x40000000u
#define SPI_MCR_CONT_SCKE_SHIFT                  30
#define SPI_MCR_MSTR_MASK                        0x80000000u
#define SPI_MCR_MSTR_SHIFT                       31
/* TCR Bit Fields */
#define SPI_TCR_SPI_TCNT_MASK                    0xFFFF0000u
#define SPI_TCR_SPI_TCNT_SHIFT                   16
#define SPI_TCR_SPI_TCNT(x)                      (((uint32_t)(((uint32_t)(x))<<SPI_TCR_SPI_TCNT_SHIFT))&SPI_TCR_SPI_TCNT_MASK)
/* CTAR Bit Fields */
#define SPI_CTAR_BR_MASK                         0xFu
#define SPI_CTAR_BR_SHIFT                        0
#define SPI_CTAR_BR(x)                           (((uint32_t)(((uint32_t)(x))<<SPI_CTAR_BR_SHIFT))&SPI_CTAR_BR_MASK)
#define SPI_CTAR_DT_MASK                         0xF0u
#define SPI_CTAR_DT_SHIFT                        4
#define SPI_CTAR_DT(x)                           (((uint32_t)(((uint32_t)(x))<<SPI_CTAR_DT_SHIFT))&SPI_CTAR_DT_MASK)
#define SPI_CTAR_ASC_MASK                        0xF00u
#define SPI_CTAR_ASC_SHIFT                       8
#define SPI_CTAR_ASC(x)                          (((uint32_t)(((uint32_t)(x))<<SPI_CTAR_ASC_SHIFT))&SPI_CTAR_ASC_MASK)
#define SPI_CTAR_CSSCK_MASK                      0xF000u
#define SPI_CTAR_CSSCK_SHIFT                     12
#define SPI_CTAR_CSSCK(x)                        (((uint32_t)(((uint32_t)(x))<<SPI_CTAR_CSSCK_SHIFT))&SPI_CTAR_CSSCK_MASK)
#define SPI_CTAR_PBR_MASK                        0x30000u
#define SPI_CTAR_PBR_SHIFT                       16
#define SPI_CTAR_PBR(x)                          (((uint32_t)(((uint32_t)(x))<<SPI_CTAR_PBR_SHIFT))&SPI_CTAR_PBR_MASK)
#define SPI_CTAR_PDT_MASK                        0xC0000u
#define SPI_CTAR_PDT_SHIFT                       18
#define SPI_CTAR_PDT(x)                          (((uint32_t)(((uint32_t)(x))<<SPI_CTAR_PDT_SHIFT))&SPI_CTAR_PDT_MASK)
#define SPI_CTAR_PASC_MASK                       0x300000u
#define SPI_CTAR_PASC_SHIFT                      20
#define SPI_CTAR_PASC(x)                         (((uint32_t)(((uint32_t)(x))<<SPI_CTAR_PASC_SHIFT))&SPI_CTAR_PASC_MASK)
#define SPI_CTAR_PCSSCK_MASK                     0xC00000u
#define SPI_CTAR_PCSSCK_SHIFT                    22
#define SPI_CTAR_PCSSCK(x)                       (((uint32_t)(((uint32_t)(x))<<SPI_CTAR_PCSSCK_SHIFT))&SPI_CTAR_PCSSCK_MASK)
#define SPI_CTAR_LSBFE_MASK                      0x1000000u
#define SPI_CTAR_LSBFE_SHIFT                     24
#define SPI_CTAR_CPHA_MASK                       0x2000000u
#define SPI_CTAR_CPHA_SHIFT                      25
#define SPI_CTAR_CPOL_MASK                       0x4000000u
#define SPI_CTAR_CPOL_SHIFT                      26
#define SPI_CTAR_FMSZ_MASK                       0x78000000u
#define SPI_CTAR_FMSZ_SHIFT                      27
#define SPI_CTAR_FMSZ(x)                         (((uint32_t)(((uint32_t)(x))<<SPI_CTAR_FMSZ_SHIFT))&SPI_CTAR_FMSZ_MASK)
#define SPI_CTAR_DBR_MASK                        0x80000000u
#define SPI_CTAR_DBR_SHIFT                       31
/* CTAR_SLAVE Bit Fields */
#define SPI_CTAR_SLAVE_CPHA_MASK                 0x2000000u
#define SPI_CTAR_SLAVE_CPHA_SHIFT                25
#define SPI_CTAR_SLAVE_CPOL_MASK                 0x4000000u
#define SPI_CTAR_SLAVE_CPOL_SHIFT                26
#define SPI_CTAR_SLAVE_FMSZ_MASK                 0xF8000000u
#define SPI_CTAR_SLAVE_FMSZ_SHIFT                27
#define SPI_CTAR_SLAVE_FMSZ(x)                   (((uint32_t)(((uint32_t)(x))<<SPI_CTAR_SLAVE_FMSZ_SHIFT))&SPI_CTAR_SLAVE_FMSZ_MASK)
/* SR Bit Fields */
#define SPI_SR_POPNXTPTR_MASK                    0xFu
#define SPI_SR_POPNXTPTR_SHIFT                   0
#define SPI_SR_POPNXTPTR(x)                      (((uint32_t)(((uint32_t)(x))<<SPI_SR_POPNXTPTR_SHIFT))&SPI_SR_POPNXTPTR_MASK)
#define SPI_SR_RXCTR_MASK                        0xF0u
#define SPI_SR_RXCTR_SHIFT                       4
#define SPI_SR_RXCTR(x)                          (((uint32_t)(((uint32_t)(x))<<SPI_SR_RXCTR_SHIFT))&SPI_SR_RXCTR_MASK)
#define SPI_SR_TXNXTPTR_MASK                     0xF00u
#define SPI_SR_TXNXTPTR_SHIFT                    8
#define SPI_SR_TXNXTPTR(x)                       (((uint32_t)(((uint32_t)(x))<<SPI_SR_TXNXTPTR_SHIFT))&SPI_SR_TXNXTPTR_MASK)
#define SPI_SR_TXCTR_MASK                        0xF000u
#define SPI_SR_TXCTR_SHIFT                       12
#define SPI_SR_TXCTR(x)                          (((uint32_t)(((uint32_t)(x))<<SPI_SR_TXCTR_SHIFT))&SPI_SR_TXCTR_MASK)
#define SPI_SR_RFDF_MASK                         0x20000u
#define SPI_SR_RFDF_SHIFT                        17
#define SPI_SR_RFOF_MASK                         0x80000u
#define SPI_SR_RFOF_SHIFT                        19
#define SPI_SR_TFFF_MASK                         0x2000000u
#define SPI_SR_TFFF_SHIFT                        25
#define SPI_SR_TFUF_MASK                         0x8000000u
#define SPI_SR_TFUF_SHIFT                        27
#define SPI_SR_EOQF_MASK                         0x10000000u
#define SPI_SR_EOQF_SHIFT                        28
#define SPI_SR_TXRXS_MASK                        0x40000000u
#define SPI_SR_TXRXS_SHIFT                       30
#define SPI_SR_TCF_MASK                          0x80000000u
#define SPI_SR_TCF_SHIFT                         31
/* RSER Bit Fields */
#define SPI_RSER_RFDF_DIRS_MASK                  0x10000u
#define SPI_RSER_RFDF_DIRS_SHIFT                 16
#define SPI_RSER_RFDF_RE_MASK                    0x20000u
#define SPI_RSER_RFDF_RE_SHIFT                   17
#define SPI_RSER_RFOF_RE_MASK                    0x80000u
#define SPI_RSER_RFOF_RE_SHIFT                   19
#define SPI_RSER_TFFF_DIRS_MASK                  0x1000000u
#define SPI_RSER_TFFF_DIRS_SHIFT                 24
#define SPI_RSER_TFFF_RE_MASK                    0x2000000u
#define SPI_RSER_TFFF_RE_SHIFT                   25
#define SPI_RSER_TFUF_RE_MASK                    0x8000000u
#define SPI_RSER_TFUF_RE_SHIFT                   27
#define SPI_RSER_EOQF_RE_MASK                    0x10000000u
#define SPI_RSER_EOQF_RE_SHIFT                   28
#define SPI_RSER_TCF_RE_MASK                     0x80000000u
#define SPI_RSER_TCF_RE_SHIFT                    31
/* PUSHR Bit Fields */
#define SPI_PUSHR_TXDATA_MASK                    0xFFFFu
#define SPI_PUSHR_TXDATA_SHIFT                   0
#define SPI_PUSHR_TXDATA(x)                      (((uint32_t)(((uint32_t)(x))<<SPI_PUSHR_TXDATA_SHIFT))&SPI_PUSHR_TXDATA_MASK)
#define SPI_PUSHR_PCS_MASK                       0x3F0000u
#define SPI_PUSHR_PCS_SHIFT                      16
#define SPI_PUSHR_PCS(x)                         (((uint32_t)(((uint32_t)(x))<<SPI_PUSHR_PCS_SHIFT))&SPI_PUSHR_PCS_MASK)
#define SPI_PUSHR_CTCNT_MASK                     0x4000000u
#define SPI_PUSHR_CTCNT_SHIFT                    26
#define SPI_PUSHR_EOQ_MASK                       0x8000000u
#define SPI_PUSHR_EOQ_SHIFT                      27
#define SPI_PUSHR_CTAS_MASK                      0x70000000u
#define SPI_PUSHR_CTAS_SHIFT                     28
#define SPI_PUSHR_CTAS(x)                        (((uint32_t)(((uint32_t)(x))<<SPI_PUSHR_CTAS_SHIFT))&SPI_PUSHR_CTAS_MASK)
#define SPI_PUSHR_CONT_MASK                      0x80000000u
#define SPI_PUSHR_CONT_SHIFT                     31
/* PUSHR_SLAVE Bit Fields */
#define SPI_PUSHR_SLAVE_TXDATA_MASK              0xFFFFu
#define SPI_PUSHR_SLAVE_TXDATA_SHIFT             0
#define SPI_PUSHR_SLAVE_TXDATA(x)                (((uint32_t)(((uint32_t)(x))<<SPI_PUSHR_SLAVE_TXDATA_SHIFT))&SPI_PUSHR_SLAVE_TXDATA_MASK)
/* POPR Bit Fields */
#define SPI_POPR_RXDATA_MASK                     0xFFFFFFFFu
#define SPI_POPR_RXDATA_SHIFT                    0
#define SPI_POPR_RXDATA(x)                       (((uint32_t)(((uint32_t)(x))<<SPI_POPR_RXDATA_SHIFT))&SPI_POPR_RXDATA_MASK)
/* TXFR0 Bit Fields */
#define SPI_TXFR0_TXDATA_MASK                    0xFFFFu
#define SPI_TXFR0_TXDATA_SHIFT                   0
#define SPI_TXFR0_TXDATA(x)                      (((uint32_t)(((uint32_t)(x))<<SPI_TXFR0_TXDATA_SHIFT))&SPI_TXFR0_TXDATA_MASK)
#define SPI_TXFR0_TXCMD_TXDATA_MASK              0xFFFF0000u
#define SPI_TXFR0_TXCMD_TXDATA_SHIFT             16
#define SPI_TXFR0_TXCMD_TXDATA(x)                (((uint32_t)(((uint32_t)(x))<<SPI_TXFR0_TXCMD_TXDATA_SHIFT))&SPI_TXFR0_TXCMD_TXDATA_MASK)
/* TXFR1 Bit Fields */
#define SPI_TXFR1_TXDATA_MASK                    0xFFFFu
#define SPI_TXFR1_TXDATA_SHIFT                   0
#define SPI_TXFR1_TXDATA(x)                      (((uint32_t)(((uint32_t)(x))<<SPI_TXFR1_TXDATA_SHIFT))&SPI_TXFR1_TXDATA_MASK)
#define SPI_TXFR1_TXCMD_TXDATA_MASK              0xFFFF0000u
#define SPI_TXFR1_TXCMD_TXDATA_SHIFT             16
#define SPI_TXFR1_TXCMD_TXDATA(x)                (((uint32_t)(((uint32_t)(x))<<SPI_TXFR1_TXCMD_TXDATA_SHIFT))&SPI_TXFR1_TXCMD_TXDATA_MASK)
/* TXFR2 Bit Fields */
#define SPI_TXFR2_TXDATA_MASK                    0xFFFFu
#define SPI_TXFR2_TXDATA_SHIFT                   0
#define SPI_TXFR2_TXDATA(x)                      (((uint32_t)(((uint32_t)(x))<<SPI_TXFR2_TXDATA_SHIFT))&SPI_TXFR2_TXDATA_MASK)
#define SPI_TXFR2_TXCMD_TXDATA_MASK              0xFFFF0000u
#define SPI_TXFR2_TXCMD_TXDATA_SHIFT             16
#define SPI_TXFR2_TXCMD_TXDATA(x)                (((uint32_t)(((uint32_t)(x))<<SPI_TXFR2_TXCMD_TXDATA_SHIFT))&SPI_TXFR2_TXCMD_TXDATA_MASK)
/* TXFR3 Bit Fields */
#define SPI_TXFR3_TXDATA_MASK                    0xFFFFu
#define SPI_TXFR3_TXDATA_SHIFT                   0
#define SPI_TXFR3_TXDATA(x)                      (((uint32_t)(((uint32_t)(x))<<SPI_TXFR3_TXDATA_SHIFT))&SPI_TXFR3_TXDATA_MASK)
#define SPI_TXFR3_TXCMD_TXDATA_MASK              0xFFFF0000u
#define SPI_TXFR3_TXCMD_TXDATA_SHIFT             16
#define SPI_TXFR3_TXCMD_TXDATA(x)                (((uint32_t)(((uint32_t)(x))<<SPI_TXFR3_TXCMD_TXDATA_SHIFT))&SPI_TXFR3_TXCMD_TXDATA_MASK)
/* RXFR0 Bit Fields */
#define SPI_RXFR0_RXDATA_MASK                    0xFFFFFFFFu
#define SPI_RXFR0_RXDATA_SHIFT                   0
#define SPI_RXFR0_RXDATA(x)                      (((uint32_t)(((uint32_t)(x))<<SPI_RXFR0_RXDATA_SHIFT))&SPI_RXFR0_RXDATA_MASK)
/* RXFR1 Bit Fields */
#define SPI_RXFR1_RXDATA_MASK                    0xFFFFFFFFu
#define SPI_RXFR1_RXDATA_SHIFT                   0
#define SPI_RXFR1_RXDATA(x)                      (((uint32_t)(((uint32_t)(x))<<SPI_RXFR1_RXDATA_SHIFT))&SPI_RXFR1_RXDATA_MASK)
/* RXFR2 Bit Fields */
#define SPI_RXFR2_RXDATA_MASK                    0xFFFFFFFFu
#define SPI_RXFR2_RXDATA_SHIFT                   0
#define SPI_RXFR2_RXDATA(x)                      (((uint32_t)(((uint32_t)(x))<<SPI_RXFR2_RXDATA_SHIFT))&SPI_RXFR2_RXDATA_MASK)
/* RXFR3 Bit Fields */
#define SPI_RXFR3_RXDATA_MASK                    0xFFFFFFFFu
#define SPI_RXFR3_RXDATA_SHIFT                   0
#define SPI_RXFR3_RXDATA(x)                      (((uint32_t)(((uint32_t)(x))<<SPI_RXFR3_RXDATA_SHIFT))&SPI_RXFR3_RXDATA_MASK)

/**
 * @}
 */ /* end of group SPI_Register_Masks */


/* SPI - Peripheral instance base addresses */
/** Peripheral SPI0 base address */
#define SPI0_BASE                                (0x4002C000u)
/** Peripheral SPI0 base pointer */
#define SPI0                                     ((SPI_Type *)SPI0_BASE)
/** Peripheral SPI1 base address */
#define SPI1_BASE                                (0x4002D000u)
/** Peripheral SPI1 base pointer */
#define SPI1                                     ((SPI_Type *)SPI1_BASE)
/** Peripheral SPI2 base address */
#define SPI2_BASE                                (0x400AC000u)
/** Peripheral SPI2 base pointer */
#define SPI2                                     ((SPI_Type *)SPI2_BASE)
/** Array initializer of SPI peripheral base pointers */
#define SPI_BASES                                { SPI0, SPI1, SPI2 }

/**
 * @}
 */ /* end of group SPI_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- TSI Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup TSI_Peripheral_Access_Layer TSI Peripheral Access Layer
 * @{
 */

/** TSI - Register Layout Typedef */
typedef struct {
  __IO uint32_t GENCS;                             /**< General Control and Status register, offset: 0x0 */
  __IO uint32_t SCANC;                             /**< SCAN Control register, offset: 0x4 */
  __IO uint32_t PEN;                               /**< Pin Enable register, offset: 0x8 */
  __I  uint32_t WUCNTR;                            /**< Wake-Up Channel Counter Register, offset: 0xC */
       uint8_t RESERVED_0[240];
  __I  uint32_t CNTR1;                             /**< Counter Register, offset: 0x100 */
  __I  uint32_t CNTR3;                             /**< Counter Register, offset: 0x104 */
  __I  uint32_t CNTR5;                             /**< Counter Register, offset: 0x108 */
  __I  uint32_t CNTR7;                             /**< Counter Register, offset: 0x10C */
  __I  uint32_t CNTR9;                             /**< Counter Register, offset: 0x110 */
  __I  uint32_t CNTR11;                            /**< Counter Register, offset: 0x114 */
  __I  uint32_t CNTR13;                            /**< Counter Register, offset: 0x118 */
  __I  uint32_t CNTR15;                            /**< Counter Register, offset: 0x11C */
  __IO uint32_t THRESHOLD;                         /**< Low-Power Channel Threshold register, offset: 0x120 */
} TSI_Type;

/* ----------------------------------------------------------------------------
   -- TSI Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup TSI_Register_Masks TSI Register Masks
 * @{
 */

/* GENCS Bit Fields */
#define TSI_GENCS_STPE_MASK                      0x1u
#define TSI_GENCS_STPE_SHIFT                     0
#define TSI_GENCS_STM_MASK                       0x2u
#define TSI_GENCS_STM_SHIFT                      1
#define TSI_GENCS_ESOR_MASK                      0x10u
#define TSI_GENCS_ESOR_SHIFT                     4
#define TSI_GENCS_ERIE_MASK                      0x20u
#define TSI_GENCS_ERIE_SHIFT                     5
#define TSI_GENCS_TSIIE_MASK                     0x40u
#define TSI_GENCS_TSIIE_SHIFT                    6
#define TSI_GENCS_TSIEN_MASK                     0x80u
#define TSI_GENCS_TSIEN_SHIFT                    7
#define TSI_GENCS_SWTS_MASK                      0x100u
#define TSI_GENCS_SWTS_SHIFT                     8
#define TSI_GENCS_SCNIP_MASK                     0x200u
#define TSI_GENCS_SCNIP_SHIFT                    9
#define TSI_GENCS_OVRF_MASK                      0x1000u
#define TSI_GENCS_OVRF_SHIFT                     12
#define TSI_GENCS_EXTERF_MASK                    0x2000u
#define TSI_GENCS_EXTERF_SHIFT                   13
#define TSI_GENCS_OUTRGF_MASK                    0x4000u
#define TSI_GENCS_OUTRGF_SHIFT                   14
#define TSI_GENCS_EOSF_MASK                      0x8000u
#define TSI_GENCS_EOSF_SHIFT                     15
#define TSI_GENCS_PS_MASK                        0x70000u
#define TSI_GENCS_PS_SHIFT                       16
#define TSI_GENCS_PS(x)                          (((uint32_t)(((uint32_t)(x))<<TSI_GENCS_PS_SHIFT))&TSI_GENCS_PS_MASK)
#define TSI_GENCS_NSCN_MASK                      0xF80000u
#define TSI_GENCS_NSCN_SHIFT                     19
#define TSI_GENCS_NSCN(x)                        (((uint32_t)(((uint32_t)(x))<<TSI_GENCS_NSCN_SHIFT))&TSI_GENCS_NSCN_MASK)
#define TSI_GENCS_LPSCNITV_MASK                  0xF000000u
#define TSI_GENCS_LPSCNITV_SHIFT                 24
#define TSI_GENCS_LPSCNITV(x)                    (((uint32_t)(((uint32_t)(x))<<TSI_GENCS_LPSCNITV_SHIFT))&TSI_GENCS_LPSCNITV_MASK)
#define TSI_GENCS_LPCLKS_MASK                    0x10000000u
#define TSI_GENCS_LPCLKS_SHIFT                   28
/* SCANC Bit Fields */
#define TSI_SCANC_AMPSC_MASK                     0x7u
#define TSI_SCANC_AMPSC_SHIFT                    0
#define TSI_SCANC_AMPSC(x)                       (((uint32_t)(((uint32_t)(x))<<TSI_SCANC_AMPSC_SHIFT))&TSI_SCANC_AMPSC_MASK)
#define TSI_SCANC_AMCLKS_MASK                    0x18u
#define TSI_SCANC_AMCLKS_SHIFT                   3
#define TSI_SCANC_AMCLKS(x)                      (((uint32_t)(((uint32_t)(x))<<TSI_SCANC_AMCLKS_SHIFT))&TSI_SCANC_AMCLKS_MASK)
#define TSI_SCANC_SMOD_MASK                      0xFF00u
#define TSI_SCANC_SMOD_SHIFT                     8
#define TSI_SCANC_SMOD(x)                        (((uint32_t)(((uint32_t)(x))<<TSI_SCANC_SMOD_SHIFT))&TSI_SCANC_SMOD_MASK)
#define TSI_SCANC_EXTCHRG_MASK                   0xF0000u
#define TSI_SCANC_EXTCHRG_SHIFT                  16
#define TSI_SCANC_EXTCHRG(x)                     (((uint32_t)(((uint32_t)(x))<<TSI_SCANC_EXTCHRG_SHIFT))&TSI_SCANC_EXTCHRG_MASK)
#define TSI_SCANC_REFCHRG_MASK                   0xF000000u
#define TSI_SCANC_REFCHRG_SHIFT                  24
#define TSI_SCANC_REFCHRG(x)                     (((uint32_t)(((uint32_t)(x))<<TSI_SCANC_REFCHRG_SHIFT))&TSI_SCANC_REFCHRG_MASK)
/* PEN Bit Fields */
#define TSI_PEN_PEN0_MASK                        0x1u
#define TSI_PEN_PEN0_SHIFT                       0
#define TSI_PEN_PEN1_MASK                        0x2u
#define TSI_PEN_PEN1_SHIFT                       1
#define TSI_PEN_PEN2_MASK                        0x4u
#define TSI_PEN_PEN2_SHIFT                       2
#define TSI_PEN_PEN3_MASK                        0x8u
#define TSI_PEN_PEN3_SHIFT                       3
#define TSI_PEN_PEN4_MASK                        0x10u
#define TSI_PEN_PEN4_SHIFT                       4
#define TSI_PEN_PEN5_MASK                        0x20u
#define TSI_PEN_PEN5_SHIFT                       5
#define TSI_PEN_PEN6_MASK                        0x40u
#define TSI_PEN_PEN6_SHIFT                       6
#define TSI_PEN_PEN7_MASK                        0x80u
#define TSI_PEN_PEN7_SHIFT                       7
#define TSI_PEN_PEN8_MASK                        0x100u
#define TSI_PEN_PEN8_SHIFT                       8
#define TSI_PEN_PEN9_MASK                        0x200u
#define TSI_PEN_PEN9_SHIFT                       9
#define TSI_PEN_PEN10_MASK                       0x400u
#define TSI_PEN_PEN10_SHIFT                      10
#define TSI_PEN_PEN11_MASK                       0x800u
#define TSI_PEN_PEN11_SHIFT                      11
#define TSI_PEN_PEN12_MASK                       0x1000u
#define TSI_PEN_PEN12_SHIFT                      12
#define TSI_PEN_PEN13_MASK                       0x2000u
#define TSI_PEN_PEN13_SHIFT                      13
#define TSI_PEN_PEN14_MASK                       0x4000u
#define TSI_PEN_PEN14_SHIFT                      14
#define TSI_PEN_PEN15_MASK                       0x8000u
#define TSI_PEN_PEN15_SHIFT                      15
#define TSI_PEN_LPSP_MASK                        0xF0000u
#define TSI_PEN_LPSP_SHIFT                       16
#define TSI_PEN_LPSP(x)                          (((uint32_t)(((uint32_t)(x))<<TSI_PEN_LPSP_SHIFT))&TSI_PEN_LPSP_MASK)
/* WUCNTR Bit Fields */
#define TSI_WUCNTR_WUCNT_MASK                    0xFFFFu
#define TSI_WUCNTR_WUCNT_SHIFT                   0
#define TSI_WUCNTR_WUCNT(x)                      (((uint32_t)(((uint32_t)(x))<<TSI_WUCNTR_WUCNT_SHIFT))&TSI_WUCNTR_WUCNT_MASK)
/* CNTR1 Bit Fields */
#define TSI_CNTR1_CTN1_MASK                      0xFFFFu
#define TSI_CNTR1_CTN1_SHIFT                     0
#define TSI_CNTR1_CTN1(x)                        (((uint32_t)(((uint32_t)(x))<<TSI_CNTR1_CTN1_SHIFT))&TSI_CNTR1_CTN1_MASK)
#define TSI_CNTR1_CTN_MASK                       0xFFFF0000u
#define TSI_CNTR1_CTN_SHIFT                      16
#define TSI_CNTR1_CTN(x)                         (((uint32_t)(((uint32_t)(x))<<TSI_CNTR1_CTN_SHIFT))&TSI_CNTR1_CTN_MASK)
/* CNTR3 Bit Fields */
#define TSI_CNTR3_CTN1_MASK                      0xFFFFu
#define TSI_CNTR3_CTN1_SHIFT                     0
#define TSI_CNTR3_CTN1(x)                        (((uint32_t)(((uint32_t)(x))<<TSI_CNTR3_CTN1_SHIFT))&TSI_CNTR3_CTN1_MASK)
#define TSI_CNTR3_CTN_MASK                       0xFFFF0000u
#define TSI_CNTR3_CTN_SHIFT                      16
#define TSI_CNTR3_CTN(x)                         (((uint32_t)(((uint32_t)(x))<<TSI_CNTR3_CTN_SHIFT))&TSI_CNTR3_CTN_MASK)
/* CNTR5 Bit Fields */
#define TSI_CNTR5_CTN1_MASK                      0xFFFFu
#define TSI_CNTR5_CTN1_SHIFT                     0
#define TSI_CNTR5_CTN1(x)                        (((uint32_t)(((uint32_t)(x))<<TSI_CNTR5_CTN1_SHIFT))&TSI_CNTR5_CTN1_MASK)
#define TSI_CNTR5_CTN_MASK                       0xFFFF0000u
#define TSI_CNTR5_CTN_SHIFT                      16
#define TSI_CNTR5_CTN(x)                         (((uint32_t)(((uint32_t)(x))<<TSI_CNTR5_CTN_SHIFT))&TSI_CNTR5_CTN_MASK)
/* CNTR7 Bit Fields */
#define TSI_CNTR7_CTN1_MASK                      0xFFFFu
#define TSI_CNTR7_CTN1_SHIFT                     0
#define TSI_CNTR7_CTN1(x)                        (((uint32_t)(((uint32_t)(x))<<TSI_CNTR7_CTN1_SHIFT))&TSI_CNTR7_CTN1_MASK)
#define TSI_CNTR7_CTN_MASK                       0xFFFF0000u
#define TSI_CNTR7_CTN_SHIFT                      16
#define TSI_CNTR7_CTN(x)                         (((uint32_t)(((uint32_t)(x))<<TSI_CNTR7_CTN_SHIFT))&TSI_CNTR7_CTN_MASK)
/* CNTR9 Bit Fields */
#define TSI_CNTR9_CTN1_MASK                      0xFFFFu
#define TSI_CNTR9_CTN1_SHIFT                     0
#define TSI_CNTR9_CTN1(x)                        (((uint32_t)(((uint32_t)(x))<<TSI_CNTR9_CTN1_SHIFT))&TSI_CNTR9_CTN1_MASK)
#define TSI_CNTR9_CTN_MASK                       0xFFFF0000u
#define TSI_CNTR9_CTN_SHIFT                      16
#define TSI_CNTR9_CTN(x)                         (((uint32_t)(((uint32_t)(x))<<TSI_CNTR9_CTN_SHIFT))&TSI_CNTR9_CTN_MASK)
/* CNTR11 Bit Fields */
#define TSI_CNTR11_CTN1_MASK                     0xFFFFu
#define TSI_CNTR11_CTN1_SHIFT                    0
#define TSI_CNTR11_CTN1(x)                       (((uint32_t)(((uint32_t)(x))<<TSI_CNTR11_CTN1_SHIFT))&TSI_CNTR11_CTN1_MASK)
#define TSI_CNTR11_CTN_MASK                      0xFFFF0000u
#define TSI_CNTR11_CTN_SHIFT                     16
#define TSI_CNTR11_CTN(x)                        (((uint32_t)(((uint32_t)(x))<<TSI_CNTR11_CTN_SHIFT))&TSI_CNTR11_CTN_MASK)
/* CNTR13 Bit Fields */
#define TSI_CNTR13_CTN1_MASK                     0xFFFFu
#define TSI_CNTR13_CTN1_SHIFT                    0
#define TSI_CNTR13_CTN1(x)                       (((uint32_t)(((uint32_t)(x))<<TSI_CNTR13_CTN1_SHIFT))&TSI_CNTR13_CTN1_MASK)
#define TSI_CNTR13_CTN_MASK                      0xFFFF0000u
#define TSI_CNTR13_CTN_SHIFT                     16
#define TSI_CNTR13_CTN(x)                        (((uint32_t)(((uint32_t)(x))<<TSI_CNTR13_CTN_SHIFT))&TSI_CNTR13_CTN_MASK)
/* CNTR15 Bit Fields */
#define TSI_CNTR15_CTN1_MASK                     0xFFFFu
#define TSI_CNTR15_CTN1_SHIFT                    0
#define TSI_CNTR15_CTN1(x)                       (((uint32_t)(((uint32_t)(x))<<TSI_CNTR15_CTN1_SHIFT))&TSI_CNTR15_CTN1_MASK)
#define TSI_CNTR15_CTN_MASK                      0xFFFF0000u
#define TSI_CNTR15_CTN_SHIFT                     16
#define TSI_CNTR15_CTN(x)                        (((uint32_t)(((uint32_t)(x))<<TSI_CNTR15_CTN_SHIFT))&TSI_CNTR15_CTN_MASK)
/* THRESHOLD Bit Fields */
#define TSI_THRESHOLD_HTHH_MASK                  0xFFFFu
#define TSI_THRESHOLD_HTHH_SHIFT                 0
#define TSI_THRESHOLD_HTHH(x)                    (((uint32_t)(((uint32_t)(x))<<TSI_THRESHOLD_HTHH_SHIFT))&TSI_THRESHOLD_HTHH_MASK)
#define TSI_THRESHOLD_LTHH_MASK                  0xFFFF0000u
#define TSI_THRESHOLD_LTHH_SHIFT                 16
#define TSI_THRESHOLD_LTHH(x)                    (((uint32_t)(((uint32_t)(x))<<TSI_THRESHOLD_LTHH_SHIFT))&TSI_THRESHOLD_LTHH_MASK)

/**
 * @}
 */ /* end of group TSI_Register_Masks */


/* TSI - Peripheral instance base addresses */
/** Peripheral TSI0 base address */
#define TSI0_BASE                                (0x40045000u)
/** Peripheral TSI0 base pointer */
#define TSI0                                     ((TSI_Type *)TSI0_BASE)
/** Array initializer of TSI peripheral base pointers */
#define TSI_BASES                                { TSI0 }

/**
 * @}
 */ /* end of group TSI_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- UART Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup UART_Peripheral_Access_Layer UART Peripheral Access Layer
 * @{
 */

/** UART - Register Layout Typedef */
typedef struct {
  __IO uint8_t BDH;                                /**< UART Baud Rate Registers: High, offset: 0x0 */
  __IO uint8_t BDL;                                /**< UART Baud Rate Registers: Low, offset: 0x1 */
  __IO uint8_t C1;                                 /**< UART Control Register 1, offset: 0x2 */
  __IO uint8_t C2;                                 /**< UART Control Register 2, offset: 0x3 */
  __I  uint8_t S1;                                 /**< UART Status Register 1, offset: 0x4 */
  __IO uint8_t S2;                                 /**< UART Status Register 2, offset: 0x5 */
  __IO uint8_t C3;                                 /**< UART Control Register 3, offset: 0x6 */
  __IO uint8_t D;                                  /**< UART Data Register, offset: 0x7 */
  __IO uint8_t MA1;                                /**< UART Match Address Registers 1, offset: 0x8 */
  __IO uint8_t MA2;                                /**< UART Match Address Registers 2, offset: 0x9 */
  __IO uint8_t C4;                                 /**< UART Control Register 4, offset: 0xA */
  __IO uint8_t C5;                                 /**< UART Control Register 5, offset: 0xB */
  __I  uint8_t ED;                                 /**< UART Extended Data Register, offset: 0xC */
  __IO uint8_t MODEM;                              /**< UART Modem Register, offset: 0xD */
  __IO uint8_t IR;                                 /**< UART Infrared Register, offset: 0xE */
       uint8_t RESERVED_0[1];
  __IO uint8_t PFIFO;                              /**< UART FIFO Parameters, offset: 0x10 */
  __IO uint8_t CFIFO;                              /**< UART FIFO Control Register, offset: 0x11 */
  __IO uint8_t SFIFO;                              /**< UART FIFO Status Register, offset: 0x12 */
  __IO uint8_t TWFIFO;                             /**< UART FIFO Transmit Watermark, offset: 0x13 */
  __I  uint8_t TCFIFO;                             /**< UART FIFO Transmit Count, offset: 0x14 */
  __IO uint8_t RWFIFO;                             /**< UART FIFO Receive Watermark, offset: 0x15 */
  __I  uint8_t RCFIFO;                             /**< UART FIFO Receive Count, offset: 0x16 */
       uint8_t RESERVED_1[1];
  __IO uint8_t C7816;                              /**< UART 7816 Control Register, offset: 0x18 */
  __IO uint8_t IE7816;                             /**< UART 7816 Interrupt Enable Register, offset: 0x19 */
  __IO uint8_t IS7816;                             /**< UART 7816 Interrupt Status Register, offset: 0x1A */
  union {                                          /* offset: 0x1B */
    __IO uint8_t WP7816_T_TYPE0;                     /**< UART 7816 Wait Parameter Register, offset: 0x1B */
    __IO uint8_t WP7816_T_TYPE1;                     /**< UART 7816 Wait Parameter Register, offset: 0x1B */
  };
  __IO uint8_t WN7816;                             /**< UART 7816 Wait N Register, offset: 0x1C */
  __IO uint8_t WF7816;                             /**< UART 7816 Wait FD Register, offset: 0x1D */
  __IO uint8_t ET7816;                             /**< UART 7816 Error Threshold Register, offset: 0x1E */
  __IO uint8_t TL7816;                             /**< UART 7816 Transmit Length Register, offset: 0x1F */
       uint8_t RESERVED_2[1];
  __IO uint8_t C6;                                 /**< UART CEA709.1-B Control Register 6, offset: 0x21 */
  __IO uint8_t PCTH;                               /**< UART CEA709.1-B Packet Cycle Time Counter High, offset: 0x22 */
  __IO uint8_t PCTL;                               /**< UART CEA709.1-B Packet Cycle Time Counter Low, offset: 0x23 */
  __IO uint8_t B1T;                                /**< UART CEA709.1-B Beta1 Timer, offset: 0x24 */
  __IO uint8_t SDTH;                               /**< UART CEA709.1-B Secondary Delay Timer High, offset: 0x25 */
  __IO uint8_t SDTL;                               /**< UART CEA709.1-B Secondary Delay Timer Low, offset: 0x26 */
  __IO uint8_t PRE;                                /**< UART CEA709.1-B Preamble, offset: 0x27 */
  __IO uint8_t TPL;                                /**< UART CEA709.1-B Transmit Packet Length, offset: 0x28 */
  __IO uint8_t IE;                                 /**< UART CEA709.1-B Interrupt Enable Register, offset: 0x29 */
  __IO uint8_t WB;                                 /**< UART CEA709.1-B WBASE, offset: 0x2A */
  __IO uint8_t S3;                                 /**< UART CEA709.1-B Status Register, offset: 0x2B */
  __IO uint8_t S4;                                 /**< UART CEA709.1-B Status Register, offset: 0x2C */
  __I  uint8_t RPL;                                /**< UART CEA709.1-B Received Packet Length, offset: 0x2D */
  __I  uint8_t RPREL;                              /**< UART CEA709.1-B Received Preamble Length, offset: 0x2E */
  __IO uint8_t CPW;                                /**< UART CEA709.1-B Collision Pulse Width, offset: 0x2F */
  __IO uint8_t RIDT;                               /**< UART CEA709.1-B Receive Indeterminate Time, offset: 0x30 */
  __IO uint8_t TIDT;                               /**< UART CEA709.1-B Transmit Indeterminate Time, offset: 0x31 */
} UART_Type;

/* ----------------------------------------------------------------------------
   -- UART Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup UART_Register_Masks UART Register Masks
 * @{
 */

/* BDH Bit Fields */
#define UART_BDH_SBR_MASK                        0x1Fu
#define UART_BDH_SBR_SHIFT                       0
#define UART_BDH_SBR(x)                          (((uint8_t)(((uint8_t)(x))<<UART_BDH_SBR_SHIFT))&UART_BDH_SBR_MASK)
#define UART_BDH_RXEDGIE_MASK                    0x40u
#define UART_BDH_RXEDGIE_SHIFT                   6
#define UART_BDH_LBKDIE_MASK                     0x80u
#define UART_BDH_LBKDIE_SHIFT                    7
/* BDL Bit Fields */
#define UART_BDL_SBR_MASK                        0xFFu
#define UART_BDL_SBR_SHIFT                       0
#define UART_BDL_SBR(x)                          (((uint8_t)(((uint8_t)(x))<<UART_BDL_SBR_SHIFT))&UART_BDL_SBR_MASK)
/* C1 Bit Fields */
#define UART_C1_PT_MASK                          0x1u
#define UART_C1_PT_SHIFT                         0
#define UART_C1_PE_MASK                          0x2u
#define UART_C1_PE_SHIFT                         1
#define UART_C1_ILT_MASK                         0x4u
#define UART_C1_ILT_SHIFT                        2
#define UART_C1_WAKE_MASK                        0x8u
#define UART_C1_WAKE_SHIFT                       3
#define UART_C1_M_MASK                           0x10u
#define UART_C1_M_SHIFT                          4
#define UART_C1_RSRC_MASK                        0x20u
#define UART_C1_RSRC_SHIFT                       5
#define UART_C1_UARTSWAI_MASK                    0x40u
#define UART_C1_UARTSWAI_SHIFT                   6
#define UART_C1_LOOPS_MASK                       0x80u
#define UART_C1_LOOPS_SHIFT                      7
/* C2 Bit Fields */
#define UART_C2_SBK_MASK                         0x1u
#define UART_C2_SBK_SHIFT                        0
#define UART_C2_RWU_MASK                         0x2u
#define UART_C2_RWU_SHIFT                        1
#define UART_C2_RE_MASK                          0x4u
#define UART_C2_RE_SHIFT                         2
#define UART_C2_TE_MASK                          0x8u
#define UART_C2_TE_SHIFT                         3
#define UART_C2_ILIE_MASK                        0x10u
#define UART_C2_ILIE_SHIFT                       4
#define UART_C2_RIE_MASK                         0x20u
#define UART_C2_RIE_SHIFT                        5
#define UART_C2_TCIE_MASK                        0x40u
#define UART_C2_TCIE_SHIFT                       6
#define UART_C2_TIE_MASK                         0x80u
#define UART_C2_TIE_SHIFT                        7
/* S1 Bit Fields */
#define UART_S1_PF_MASK                          0x1u
#define UART_S1_PF_SHIFT                         0
#define UART_S1_FE_MASK                          0x2u
#define UART_S1_FE_SHIFT                         1
#define UART_S1_NF_MASK                          0x4u
#define UART_S1_NF_SHIFT                         2
#define UART_S1_OR_MASK                          0x8u
#define UART_S1_OR_SHIFT                         3
#define UART_S1_IDLE_MASK                        0x10u
#define UART_S1_IDLE_SHIFT                       4
#define UART_S1_RDRF_MASK                        0x20u
#define UART_S1_RDRF_SHIFT                       5
#define UART_S1_TC_MASK                          0x40u
#define UART_S1_TC_SHIFT                         6
#define UART_S1_TDRE_MASK                        0x80u
#define UART_S1_TDRE_SHIFT                       7
/* S2 Bit Fields */
#define UART_S2_RAF_MASK                         0x1u
#define UART_S2_RAF_SHIFT                        0
#define UART_S2_LBKDE_MASK                       0x2u
#define UART_S2_LBKDE_SHIFT                      1
#define UART_S2_BRK13_MASK                       0x4u
#define UART_S2_BRK13_SHIFT                      2
#define UART_S2_RWUID_MASK                       0x8u
#define UART_S2_RWUID_SHIFT                      3
#define UART_S2_RXINV_MASK                       0x10u
#define UART_S2_RXINV_SHIFT                      4
#define UART_S2_MSBF_MASK                        0x20u
#define UART_S2_MSBF_SHIFT                       5
#define UART_S2_RXEDGIF_MASK                     0x40u
#define UART_S2_RXEDGIF_SHIFT                    6
#define UART_S2_LBKDIF_MASK                      0x80u
#define UART_S2_LBKDIF_SHIFT                     7
/* C3 Bit Fields */
#define UART_C3_PEIE_MASK                        0x1u
#define UART_C3_PEIE_SHIFT                       0
#define UART_C3_FEIE_MASK                        0x2u
#define UART_C3_FEIE_SHIFT                       1
#define UART_C3_NEIE_MASK                        0x4u
#define UART_C3_NEIE_SHIFT                       2
#define UART_C3_ORIE_MASK                        0x8u
#define UART_C3_ORIE_SHIFT                       3
#define UART_C3_TXINV_MASK                       0x10u
#define UART_C3_TXINV_SHIFT                      4
#define UART_C3_TXDIR_MASK                       0x20u
#define UART_C3_TXDIR_SHIFT                      5
#define UART_C3_T8_MASK                          0x40u
#define UART_C3_T8_SHIFT                         6
#define UART_C3_R8_MASK                          0x80u
#define UART_C3_R8_SHIFT                         7
/* D Bit Fields */
#define UART_D_RT_MASK                           0xFFu
#define UART_D_RT_SHIFT                          0
#define UART_D_RT(x)                             (((uint8_t)(((uint8_t)(x))<<UART_D_RT_SHIFT))&UART_D_RT_MASK)
/* MA1 Bit Fields */
#define UART_MA1_MA_MASK                         0xFFu
#define UART_MA1_MA_SHIFT                        0
#define UART_MA1_MA(x)                           (((uint8_t)(((uint8_t)(x))<<UART_MA1_MA_SHIFT))&UART_MA1_MA_MASK)
/* MA2 Bit Fields */
#define UART_MA2_MA_MASK                         0xFFu
#define UART_MA2_MA_SHIFT                        0
#define UART_MA2_MA(x)                           (((uint8_t)(((uint8_t)(x))<<UART_MA2_MA_SHIFT))&UART_MA2_MA_MASK)
/* C4 Bit Fields */
#define UART_C4_BRFA_MASK                        0x1Fu
#define UART_C4_BRFA_SHIFT                       0
#define UART_C4_BRFA(x)                          (((uint8_t)(((uint8_t)(x))<<UART_C4_BRFA_SHIFT))&UART_C4_BRFA_MASK)
#define UART_C4_M10_MASK                         0x20u
#define UART_C4_M10_SHIFT                        5
#define UART_C4_MAEN2_MASK                       0x40u
#define UART_C4_MAEN2_SHIFT                      6
#define UART_C4_MAEN1_MASK                       0x80u
#define UART_C4_MAEN1_SHIFT                      7
/* C5 Bit Fields */
#define UART_C5_RDMAS_MASK                       0x20u
#define UART_C5_RDMAS_SHIFT                      5
#define UART_C5_TDMAS_MASK                       0x80u
#define UART_C5_TDMAS_SHIFT                      7
/* ED Bit Fields */
#define UART_ED_PARITYE_MASK                     0x40u
#define UART_ED_PARITYE_SHIFT                    6
#define UART_ED_NOISY_MASK                       0x80u
#define UART_ED_NOISY_SHIFT                      7
/* MODEM Bit Fields */
#define UART_MODEM_TXCTSE_MASK                   0x1u
#define UART_MODEM_TXCTSE_SHIFT                  0
#define UART_MODEM_TXRTSE_MASK                   0x2u
#define UART_MODEM_TXRTSE_SHIFT                  1
#define UART_MODEM_TXRTSPOL_MASK                 0x4u
#define UART_MODEM_TXRTSPOL_SHIFT                2
#define UART_MODEM_RXRTSE_MASK                   0x8u
#define UART_MODEM_RXRTSE_SHIFT                  3
/* IR Bit Fields */
#define UART_IR_TNP_MASK                         0x3u
#define UART_IR_TNP_SHIFT                        0
#define UART_IR_TNP(x)                           (((uint8_t)(((uint8_t)(x))<<UART_IR_TNP_SHIFT))&UART_IR_TNP_MASK)
#define UART_IR_IREN_MASK                        0x4u
#define UART_IR_IREN_SHIFT                       2
/* PFIFO Bit Fields */
#define UART_PFIFO_RXFIFOSIZE_MASK               0x7u
#define UART_PFIFO_RXFIFOSIZE_SHIFT              0
#define UART_PFIFO_RXFIFOSIZE(x)                 (((uint8_t)(((uint8_t)(x))<<UART_PFIFO_RXFIFOSIZE_SHIFT))&UART_PFIFO_RXFIFOSIZE_MASK)
#define UART_PFIFO_RXFE_MASK                     0x8u
#define UART_PFIFO_RXFE_SHIFT                    3
#define UART_PFIFO_TXFIFOSIZE_MASK               0x70u
#define UART_PFIFO_TXFIFOSIZE_SHIFT              4
#define UART_PFIFO_TXFIFOSIZE(x)                 (((uint8_t)(((uint8_t)(x))<<UART_PFIFO_TXFIFOSIZE_SHIFT))&UART_PFIFO_TXFIFOSIZE_MASK)
#define UART_PFIFO_TXFE_MASK                     0x80u
#define UART_PFIFO_TXFE_SHIFT                    7
/* CFIFO Bit Fields */
#define UART_CFIFO_RXUFE_MASK                    0x1u
#define UART_CFIFO_RXUFE_SHIFT                   0
#define UART_CFIFO_TXOFE_MASK                    0x2u
#define UART_CFIFO_TXOFE_SHIFT                   1
#define UART_CFIFO_RXOFE_MASK                    0x4u
#define UART_CFIFO_RXOFE_SHIFT                   2
#define UART_CFIFO_RXFLUSH_MASK                  0x40u
#define UART_CFIFO_RXFLUSH_SHIFT                 6
#define UART_CFIFO_TXFLUSH_MASK                  0x80u
#define UART_CFIFO_TXFLUSH_SHIFT                 7
/* SFIFO Bit Fields */
#define UART_SFIFO_RXUF_MASK                     0x1u
#define UART_SFIFO_RXUF_SHIFT                    0
#define UART_SFIFO_TXOF_MASK                     0x2u
#define UART_SFIFO_TXOF_SHIFT                    1
#define UART_SFIFO_RXOF_MASK                     0x4u
#define UART_SFIFO_RXOF_SHIFT                    2
#define UART_SFIFO_RXEMPT_MASK                   0x40u
#define UART_SFIFO_RXEMPT_SHIFT                  6
#define UART_SFIFO_TXEMPT_MASK                   0x80u
#define UART_SFIFO_TXEMPT_SHIFT                  7
/* TWFIFO Bit Fields */
#define UART_TWFIFO_TXWATER_MASK                 0xFFu
#define UART_TWFIFO_TXWATER_SHIFT                0
#define UART_TWFIFO_TXWATER(x)                   (((uint8_t)(((uint8_t)(x))<<UART_TWFIFO_TXWATER_SHIFT))&UART_TWFIFO_TXWATER_MASK)
/* TCFIFO Bit Fields */
#define UART_TCFIFO_TXCOUNT_MASK                 0xFFu
#define UART_TCFIFO_TXCOUNT_SHIFT                0
#define UART_TCFIFO_TXCOUNT(x)                   (((uint8_t)(((uint8_t)(x))<<UART_TCFIFO_TXCOUNT_SHIFT))&UART_TCFIFO_TXCOUNT_MASK)
/* RWFIFO Bit Fields */
#define UART_RWFIFO_RXWATER_MASK                 0xFFu
#define UART_RWFIFO_RXWATER_SHIFT                0
#define UART_RWFIFO_RXWATER(x)                   (((uint8_t)(((uint8_t)(x))<<UART_RWFIFO_RXWATER_SHIFT))&UART_RWFIFO_RXWATER_MASK)
/* RCFIFO Bit Fields */
#define UART_RCFIFO_RXCOUNT_MASK                 0xFFu
#define UART_RCFIFO_RXCOUNT_SHIFT                0
#define UART_RCFIFO_RXCOUNT(x)                   (((uint8_t)(((uint8_t)(x))<<UART_RCFIFO_RXCOUNT_SHIFT))&UART_RCFIFO_RXCOUNT_MASK)
/* C7816 Bit Fields */
#define UART_C7816_ISO_7816E_MASK                0x1u
#define UART_C7816_ISO_7816E_SHIFT               0
#define UART_C7816_TTYPE_MASK                    0x2u
#define UART_C7816_TTYPE_SHIFT                   1
#define UART_C7816_INIT_MASK                     0x4u
#define UART_C7816_INIT_SHIFT                    2
#define UART_C7816_ANACK_MASK                    0x8u
#define UART_C7816_ANACK_SHIFT                   3
#define UART_C7816_ONACK_MASK                    0x10u
#define UART_C7816_ONACK_SHIFT                   4
/* IE7816 Bit Fields */
#define UART_IE7816_RXTE_MASK                    0x1u
#define UART_IE7816_RXTE_SHIFT                   0
#define UART_IE7816_TXTE_MASK                    0x2u
#define UART_IE7816_TXTE_SHIFT                   1
#define UART_IE7816_GTVE_MASK                    0x4u
#define UART_IE7816_GTVE_SHIFT                   2
#define UART_IE7816_INITDE_MASK                  0x10u
#define UART_IE7816_INITDE_SHIFT                 4
#define UART_IE7816_BWTE_MASK                    0x20u
#define UART_IE7816_BWTE_SHIFT                   5
#define UART_IE7816_CWTE_MASK                    0x40u
#define UART_IE7816_CWTE_SHIFT                   6
#define UART_IE7816_WTE_MASK                     0x80u
#define UART_IE7816_WTE_SHIFT                    7
/* IS7816 Bit Fields */
#define UART_IS7816_RXT_MASK                     0x1u
#define UART_IS7816_RXT_SHIFT                    0
#define UART_IS7816_TXT_MASK                     0x2u
#define UART_IS7816_TXT_SHIFT                    1
#define UART_IS7816_GTV_MASK                     0x4u
#define UART_IS7816_GTV_SHIFT                    2
#define UART_IS7816_INITD_MASK                   0x10u
#define UART_IS7816_INITD_SHIFT                  4
#define UART_IS7816_BWT_MASK                     0x20u
#define UART_IS7816_BWT_SHIFT                    5
#define UART_IS7816_CWT_MASK                     0x40u
#define UART_IS7816_CWT_SHIFT                    6
#define UART_IS7816_WT_MASK                      0x80u
#define UART_IS7816_WT_SHIFT                     7
/* WP7816_T_TYPE0 Bit Fields */
#define UART_WP7816_T_TYPE0_WI_MASK              0xFFu
#define UART_WP7816_T_TYPE0_WI_SHIFT             0
#define UART_WP7816_T_TYPE0_WI(x)                (((uint8_t)(((uint8_t)(x))<<UART_WP7816_T_TYPE0_WI_SHIFT))&UART_WP7816_T_TYPE0_WI_MASK)
/* WP7816_T_TYPE1 Bit Fields */
#define UART_WP7816_T_TYPE1_BWI_MASK             0xFu
#define UART_WP7816_T_TYPE1_BWI_SHIFT            0
#define UART_WP7816_T_TYPE1_BWI(x)               (((uint8_t)(((uint8_t)(x))<<UART_WP7816_T_TYPE1_BWI_SHIFT))&UART_WP7816_T_TYPE1_BWI_MASK)
#define UART_WP7816_T_TYPE1_CWI_MASK             0xF0u
#define UART_WP7816_T_TYPE1_CWI_SHIFT            4
#define UART_WP7816_T_TYPE1_CWI(x)               (((uint8_t)(((uint8_t)(x))<<UART_WP7816_T_TYPE1_CWI_SHIFT))&UART_WP7816_T_TYPE1_CWI_MASK)
/* WN7816 Bit Fields */
#define UART_WN7816_GTN_MASK                     0xFFu
#define UART_WN7816_GTN_SHIFT                    0
#define UART_WN7816_GTN(x)                       (((uint8_t)(((uint8_t)(x))<<UART_WN7816_GTN_SHIFT))&UART_WN7816_GTN_MASK)
/* WF7816 Bit Fields */
#define UART_WF7816_GTFD_MASK                    0xFFu
#define UART_WF7816_GTFD_SHIFT                   0
#define UART_WF7816_GTFD(x)                      (((uint8_t)(((uint8_t)(x))<<UART_WF7816_GTFD_SHIFT))&UART_WF7816_GTFD_MASK)
/* ET7816 Bit Fields */
#define UART_ET7816_RXTHRESHOLD_MASK             0xFu
#define UART_ET7816_RXTHRESHOLD_SHIFT            0
#define UART_ET7816_RXTHRESHOLD(x)               (((uint8_t)(((uint8_t)(x))<<UART_ET7816_RXTHRESHOLD_SHIFT))&UART_ET7816_RXTHRESHOLD_MASK)
#define UART_ET7816_TXTHRESHOLD_MASK             0xF0u
#define UART_ET7816_TXTHRESHOLD_SHIFT            4
#define UART_ET7816_TXTHRESHOLD(x)               (((uint8_t)(((uint8_t)(x))<<UART_ET7816_TXTHRESHOLD_SHIFT))&UART_ET7816_TXTHRESHOLD_MASK)
/* TL7816 Bit Fields */
#define UART_TL7816_TLEN_MASK                    0xFFu
#define UART_TL7816_TLEN_SHIFT                   0
#define UART_TL7816_TLEN(x)                      (((uint8_t)(((uint8_t)(x))<<UART_TL7816_TLEN_SHIFT))&UART_TL7816_TLEN_MASK)
/* C6 Bit Fields */
#define UART_C6_CP_MASK                          0x10u
#define UART_C6_CP_SHIFT                         4
#define UART_C6_CE_MASK                          0x20u
#define UART_C6_CE_SHIFT                         5
#define UART_C6_TX709_MASK                       0x40u
#define UART_C6_TX709_SHIFT                      6
#define UART_C6_EN709_MASK                       0x80u
#define UART_C6_EN709_SHIFT                      7
/* PCTH Bit Fields */
#define UART_PCTH_PCTH_MASK                      0xFFu
#define UART_PCTH_PCTH_SHIFT                     0
#define UART_PCTH_PCTH(x)                        (((uint8_t)(((uint8_t)(x))<<UART_PCTH_PCTH_SHIFT))&UART_PCTH_PCTH_MASK)
/* PCTL Bit Fields */
#define UART_PCTL_PCTL_MASK                      0xFFu
#define UART_PCTL_PCTL_SHIFT                     0
#define UART_PCTL_PCTL(x)                        (((uint8_t)(((uint8_t)(x))<<UART_PCTL_PCTL_SHIFT))&UART_PCTL_PCTL_MASK)
/* B1T Bit Fields */
#define UART_B1T_B1T_MASK                        0xFFu
#define UART_B1T_B1T_SHIFT                       0
#define UART_B1T_B1T(x)                          (((uint8_t)(((uint8_t)(x))<<UART_B1T_B1T_SHIFT))&UART_B1T_B1T_MASK)
/* SDTH Bit Fields */
#define UART_SDTH_SDTH_MASK                      0xFFu
#define UART_SDTH_SDTH_SHIFT                     0
#define UART_SDTH_SDTH(x)                        (((uint8_t)(((uint8_t)(x))<<UART_SDTH_SDTH_SHIFT))&UART_SDTH_SDTH_MASK)
/* SDTL Bit Fields */
#define UART_SDTL_SDTL_MASK                      0xFFu
#define UART_SDTL_SDTL_SHIFT                     0
#define UART_SDTL_SDTL(x)                        (((uint8_t)(((uint8_t)(x))<<UART_SDTL_SDTL_SHIFT))&UART_SDTL_SDTL_MASK)
/* PRE Bit Fields */
#define UART_PRE_PREAMBLE_MASK                   0xFFu
#define UART_PRE_PREAMBLE_SHIFT                  0
#define UART_PRE_PREAMBLE(x)                     (((uint8_t)(((uint8_t)(x))<<UART_PRE_PREAMBLE_SHIFT))&UART_PRE_PREAMBLE_MASK)
/* TPL Bit Fields */
#define UART_TPL_TPL_MASK                        0xFFu
#define UART_TPL_TPL_SHIFT                       0
#define UART_TPL_TPL(x)                          (((uint8_t)(((uint8_t)(x))<<UART_TPL_TPL_SHIFT))&UART_TPL_TPL_MASK)
/* IE Bit Fields */
#define UART_IE_TXFIE_MASK                       0x1u
#define UART_IE_TXFIE_SHIFT                      0
#define UART_IE_PSIE_MASK                        0x2u
#define UART_IE_PSIE_SHIFT                       1
#define UART_IE_PCTEIE_MASK                      0x4u
#define UART_IE_PCTEIE_SHIFT                     2
#define UART_IE_PTXIE_MASK                       0x8u
#define UART_IE_PTXIE_SHIFT                      3
#define UART_IE_PRXIE_MASK                       0x10u
#define UART_IE_PRXIE_SHIFT                      4
#define UART_IE_ISDIE_MASK                       0x20u
#define UART_IE_ISDIE_SHIFT                      5
#define UART_IE_WBEIE_MASK                       0x40u
#define UART_IE_WBEIE_SHIFT                      6
/* WB Bit Fields */
#define UART_WB_WBASE_MASK                       0xFFu
#define UART_WB_WBASE_SHIFT                      0
#define UART_WB_WBASE(x)                         (((uint8_t)(((uint8_t)(x))<<UART_WB_WBASE_SHIFT))&UART_WB_WBASE_MASK)
/* S3 Bit Fields */
#define UART_S3_TXFF_MASK                        0x1u
#define UART_S3_TXFF_SHIFT                       0
#define UART_S3_PSF_MASK                         0x2u
#define UART_S3_PSF_SHIFT                        1
#define UART_S3_PCTEF_MASK                       0x4u
#define UART_S3_PCTEF_SHIFT                      2
#define UART_S3_PTXF_MASK                        0x8u
#define UART_S3_PTXF_SHIFT                       3
#define UART_S3_PRXF_MASK                        0x10u
#define UART_S3_PRXF_SHIFT                       4
#define UART_S3_ISD_MASK                         0x20u
#define UART_S3_ISD_SHIFT                        5
#define UART_S3_WBEF_MASK                        0x40u
#define UART_S3_WBEF_SHIFT                       6
#define UART_S3_PEF_MASK                         0x80u
#define UART_S3_PEF_SHIFT                        7
/* S4 Bit Fields */
#define UART_S4_FE_MASK                          0x1u
#define UART_S4_FE_SHIFT                         0
#define UART_S4_ILCV_MASK                        0x2u
#define UART_S4_ILCV_SHIFT                       1
#define UART_S4_CDET_MASK                        0xCu
#define UART_S4_CDET_SHIFT                       2
#define UART_S4_CDET(x)                          (((uint8_t)(((uint8_t)(x))<<UART_S4_CDET_SHIFT))&UART_S4_CDET_MASK)
#define UART_S4_INITF_MASK                       0x10u
#define UART_S4_INITF_SHIFT                      4
/* RPL Bit Fields */
#define UART_RPL_RPL_MASK                        0xFFu
#define UART_RPL_RPL_SHIFT                       0
#define UART_RPL_RPL(x)                          (((uint8_t)(((uint8_t)(x))<<UART_RPL_RPL_SHIFT))&UART_RPL_RPL_MASK)
/* RPREL Bit Fields */
#define UART_RPREL_RPREL_MASK                    0xFFu
#define UART_RPREL_RPREL_SHIFT                   0
#define UART_RPREL_RPREL(x)                      (((uint8_t)(((uint8_t)(x))<<UART_RPREL_RPREL_SHIFT))&UART_RPREL_RPREL_MASK)
/* CPW Bit Fields */
#define UART_CPW_CPW_MASK                        0xFFu
#define UART_CPW_CPW_SHIFT                       0
#define UART_CPW_CPW(x)                          (((uint8_t)(((uint8_t)(x))<<UART_CPW_CPW_SHIFT))&UART_CPW_CPW_MASK)
/* RIDT Bit Fields */
#define UART_RIDT_RIDT_MASK                      0xFFu
#define UART_RIDT_RIDT_SHIFT                     0
#define UART_RIDT_RIDT(x)                        (((uint8_t)(((uint8_t)(x))<<UART_RIDT_RIDT_SHIFT))&UART_RIDT_RIDT_MASK)
/* TIDT Bit Fields */
#define UART_TIDT_TIDT_MASK                      0xFFu
#define UART_TIDT_TIDT_SHIFT                     0
#define UART_TIDT_TIDT(x)                        (((uint8_t)(((uint8_t)(x))<<UART_TIDT_TIDT_SHIFT))&UART_TIDT_TIDT_MASK)

/**
 * @}
 */ /* end of group UART_Register_Masks */


/* UART - Peripheral instance base addresses */
/** Peripheral UART0 base address */
#define UART0_BASE                               (0x4006A000u)
/** Peripheral UART0 base pointer */
#define UART0                                    ((UART_Type *)UART0_BASE)
/** Peripheral UART1 base address */
#define UART1_BASE                               (0x4006B000u)
/** Peripheral UART1 base pointer */
#define UART1                                    ((UART_Type *)UART1_BASE)
/** Peripheral UART2 base address */
#define UART2_BASE                               (0x4006C000u)
/** Peripheral UART2 base pointer */
#define UART2                                    ((UART_Type *)UART2_BASE)
/** Peripheral UART3 base address */
#define UART3_BASE                               (0x4006D000u)
/** Peripheral UART3 base pointer */
#define UART3                                    ((UART_Type *)UART3_BASE)
/** Peripheral UART4 base address */
#define UART4_BASE                               (0x400EA000u)
/** Peripheral UART4 base pointer */
#define UART4                                    ((UART_Type *)UART4_BASE)
/** Peripheral UART5 base address */
#define UART5_BASE                               (0x400EB000u)
/** Peripheral UART5 base pointer */
#define UART5                                    ((UART_Type *)UART5_BASE)
/** Array initializer of UART peripheral base pointers */
#define UART_BASES                               { UART0, UART1, UART2, UART3, UART4, UART5 }

/**
 * @}
 */ /* end of group UART_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- USB Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup USB_Peripheral_Access_Layer USB Peripheral Access Layer
 * @{
 */

/** USB - Register Layout Typedef */
typedef struct {
  __I  uint8_t PERID;                              /**< Peripheral ID register, offset: 0x0 */
       uint8_t RESERVED_0[3];
  __I  uint8_t IDCOMP;                             /**< Peripheral ID Complement register, offset: 0x4 */
       uint8_t RESERVED_1[3];
  __I  uint8_t REV;                                /**< Peripheral Revision register, offset: 0x8 */
       uint8_t RESERVED_2[3];
  __I  uint8_t ADDINFO;                            /**< Peripheral Additional Info register, offset: 0xC */
       uint8_t RESERVED_3[3];
  __IO uint8_t OTGISTAT;                           /**< OTG Interrupt Status register, offset: 0x10 */
       uint8_t RESERVED_4[3];
  __IO uint8_t OTGICR;                             /**< OTG Interrupt Control Register, offset: 0x14 */
       uint8_t RESERVED_5[3];
  __IO uint8_t OTGSTAT;                            /**< OTG Status register, offset: 0x18 */
       uint8_t RESERVED_6[3];
  __IO uint8_t OTGCTL;                             /**< OTG Control register, offset: 0x1C */
       uint8_t RESERVED_7[99];
  __IO uint8_t ISTAT;                              /**< Interrupt Status register, offset: 0x80 */
       uint8_t RESERVED_8[3];
  __IO uint8_t INTEN;                              /**< Interrupt Enable register, offset: 0x84 */
       uint8_t RESERVED_9[3];
  __IO uint8_t ERRSTAT;                            /**< Error Interrupt Status register, offset: 0x88 */
       uint8_t RESERVED_10[3];
  __IO uint8_t ERREN;                              /**< Error Interrupt Enable register, offset: 0x8C */
       uint8_t RESERVED_11[3];
  __I  uint8_t STAT;                               /**< Status register, offset: 0x90 */
       uint8_t RESERVED_12[3];
  __IO uint8_t CTL;                                /**< Control register, offset: 0x94 */
       uint8_t RESERVED_13[3];
  __IO uint8_t ADDR;                               /**< Address register, offset: 0x98 */
       uint8_t RESERVED_14[3];
  __IO uint8_t BDTPAGE1;                           /**< BDT Page Register 1, offset: 0x9C */
       uint8_t RESERVED_15[3];
  __IO uint8_t FRMNUML;                            /**< Frame Number Register Low, offset: 0xA0 */
       uint8_t RESERVED_16[3];
  __IO uint8_t FRMNUMH;                            /**< Frame Number Register High, offset: 0xA4 */
       uint8_t RESERVED_17[3];
  __IO uint8_t TOKEN;                              /**< Token register, offset: 0xA8 */
       uint8_t RESERVED_18[3];
  __IO uint8_t SOFTHLD;                            /**< SOF Threshold Register, offset: 0xAC */
       uint8_t RESERVED_19[3];
  __IO uint8_t BDTPAGE2;                           /**< BDT Page Register 2, offset: 0xB0 */
       uint8_t RESERVED_20[3];
  __IO uint8_t BDTPAGE3;                           /**< BDT Page Register 3, offset: 0xB4 */
       uint8_t RESERVED_21[11];
  struct {                                         /* offset: 0xC0, array step: 0x4 */
    __IO uint8_t ENDPT;                              /**< Endpoint Control register, array offset: 0xC0, array step: 0x4 */
         uint8_t RESERVED_0[3];
  } ENDPOINT[16];
  __IO uint8_t USBCTRL;                            /**< USB Control register, offset: 0x100 */
       uint8_t RESERVED_22[3];
  __I  uint8_t OBSERVE;                            /**< USB OTG Observe register, offset: 0x104 */
       uint8_t RESERVED_23[3];
  __IO uint8_t CONTROL;                            /**< USB OTG Control register, offset: 0x108 */
       uint8_t RESERVED_24[3];
  __IO uint8_t USBTRC0;                            /**< USB Transceiver Control Register 0, offset: 0x10C */
       uint8_t RESERVED_25[7];
  __IO uint8_t USBFRMADJUST;                       /**< Frame Adjust Register, offset: 0x114 */
} USB_Type;

/* ----------------------------------------------------------------------------
   -- USB Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup USB_Register_Masks USB Register Masks
 * @{
 */

/* PERID Bit Fields */
#define USB_PERID_ID_MASK                        0x3Fu
#define USB_PERID_ID_SHIFT                       0
#define USB_PERID_ID(x)                          (((uint8_t)(((uint8_t)(x))<<USB_PERID_ID_SHIFT))&USB_PERID_ID_MASK)
/* IDCOMP Bit Fields */
#define USB_IDCOMP_NID_MASK                      0x3Fu
#define USB_IDCOMP_NID_SHIFT                     0
#define USB_IDCOMP_NID(x)                        (((uint8_t)(((uint8_t)(x))<<USB_IDCOMP_NID_SHIFT))&USB_IDCOMP_NID_MASK)
/* REV Bit Fields */
#define USB_REV_REV_MASK                         0xFFu
#define USB_REV_REV_SHIFT                        0
#define USB_REV_REV(x)                           (((uint8_t)(((uint8_t)(x))<<USB_REV_REV_SHIFT))&USB_REV_REV_MASK)
/* ADDINFO Bit Fields */
#define USB_ADDINFO_IEHOST_MASK                  0x1u
#define USB_ADDINFO_IEHOST_SHIFT                 0
#define USB_ADDINFO_IRQNUM_MASK                  0xF8u
#define USB_ADDINFO_IRQNUM_SHIFT                 3
#define USB_ADDINFO_IRQNUM(x)                    (((uint8_t)(((uint8_t)(x))<<USB_ADDINFO_IRQNUM_SHIFT))&USB_ADDINFO_IRQNUM_MASK)
/* OTGISTAT Bit Fields */
#define USB_OTGISTAT_AVBUSCHG_MASK               0x1u
#define USB_OTGISTAT_AVBUSCHG_SHIFT              0
#define USB_OTGISTAT_B_SESS_CHG_MASK             0x4u
#define USB_OTGISTAT_B_SESS_CHG_SHIFT            2
#define USB_OTGISTAT_SESSVLDCHG_MASK             0x8u
#define USB_OTGISTAT_SESSVLDCHG_SHIFT            3
#define USB_OTGISTAT_LINE_STATE_CHG_MASK         0x20u
#define USB_OTGISTAT_LINE_STATE_CHG_SHIFT        5
#define USB_OTGISTAT_ONEMSEC_MASK                0x40u
#define USB_OTGISTAT_ONEMSEC_SHIFT               6
#define USB_OTGISTAT_IDCHG_MASK                  0x80u
#define USB_OTGISTAT_IDCHG_SHIFT                 7
/* OTGICR Bit Fields */
#define USB_OTGICR_AVBUSEN_MASK                  0x1u
#define USB_OTGICR_AVBUSEN_SHIFT                 0
#define USB_OTGICR_BSESSEN_MASK                  0x4u
#define USB_OTGICR_BSESSEN_SHIFT                 2
#define USB_OTGICR_SESSVLDEN_MASK                0x8u
#define USB_OTGICR_SESSVLDEN_SHIFT               3
#define USB_OTGICR_LINESTATEEN_MASK              0x20u
#define USB_OTGICR_LINESTATEEN_SHIFT             5
#define USB_OTGICR_ONEMSECEN_MASK                0x40u
#define USB_OTGICR_ONEMSECEN_SHIFT               6
#define USB_OTGICR_IDEN_MASK                     0x80u
#define USB_OTGICR_IDEN_SHIFT                    7
/* OTGSTAT Bit Fields */
#define USB_OTGSTAT_AVBUSVLD_MASK                0x1u
#define USB_OTGSTAT_AVBUSVLD_SHIFT               0
#define USB_OTGSTAT_BSESSEND_MASK                0x4u
#define USB_OTGSTAT_BSESSEND_SHIFT               2
#define USB_OTGSTAT_SESS_VLD_MASK                0x8u
#define USB_OTGSTAT_SESS_VLD_SHIFT               3
#define USB_OTGSTAT_LINESTATESTABLE_MASK         0x20u
#define USB_OTGSTAT_LINESTATESTABLE_SHIFT        5
#define USB_OTGSTAT_ONEMSECEN_MASK               0x40u
#define USB_OTGSTAT_ONEMSECEN_SHIFT              6
#define USB_OTGSTAT_ID_MASK                      0x80u
#define USB_OTGSTAT_ID_SHIFT                     7
/* OTGCTL Bit Fields */
#define USB_OTGCTL_OTGEN_MASK                    0x4u
#define USB_OTGCTL_OTGEN_SHIFT                   2
#define USB_OTGCTL_DMLOW_MASK                    0x10u
#define USB_OTGCTL_DMLOW_SHIFT                   4
#define USB_OTGCTL_DPLOW_MASK                    0x20u
#define USB_OTGCTL_DPLOW_SHIFT                   5
#define USB_OTGCTL_DPHIGH_MASK                   0x80u
#define USB_OTGCTL_DPHIGH_SHIFT                  7
/* ISTAT Bit Fields */
#define USB_ISTAT_USBRST_MASK                    0x1u
#define USB_ISTAT_USBRST_SHIFT                   0
#define USB_ISTAT_ERROR_MASK                     0x2u
#define USB_ISTAT_ERROR_SHIFT                    1
#define USB_ISTAT_SOFTOK_MASK                    0x4u
#define USB_ISTAT_SOFTOK_SHIFT                   2
#define USB_ISTAT_TOKDNE_MASK                    0x8u
#define USB_ISTAT_TOKDNE_SHIFT                   3
#define USB_ISTAT_SLEEP_MASK                     0x10u
#define USB_ISTAT_SLEEP_SHIFT                    4
#define USB_ISTAT_RESUME_MASK                    0x20u
#define USB_ISTAT_RESUME_SHIFT                   5
#define USB_ISTAT_ATTACH_MASK                    0x40u
#define USB_ISTAT_ATTACH_SHIFT                   6
#define USB_ISTAT_STALL_MASK                     0x80u
#define USB_ISTAT_STALL_SHIFT                    7
/* INTEN Bit Fields */
#define USB_INTEN_USBRSTEN_MASK                  0x1u
#define USB_INTEN_USBRSTEN_SHIFT                 0
#define USB_INTEN_ERROREN_MASK                   0x2u
#define USB_INTEN_ERROREN_SHIFT                  1
#define USB_INTEN_SOFTOKEN_MASK                  0x4u
#define USB_INTEN_SOFTOKEN_SHIFT                 2
#define USB_INTEN_TOKDNEEN_MASK                  0x8u
#define USB_INTEN_TOKDNEEN_SHIFT                 3
#define USB_INTEN_SLEEPEN_MASK                   0x10u
#define USB_INTEN_SLEEPEN_SHIFT                  4
#define USB_INTEN_RESUMEEN_MASK                  0x20u
#define USB_INTEN_RESUMEEN_SHIFT                 5
#define USB_INTEN_ATTACHEN_MASK                  0x40u
#define USB_INTEN_ATTACHEN_SHIFT                 6
#define USB_INTEN_STALLEN_MASK                   0x80u
#define USB_INTEN_STALLEN_SHIFT                  7
/* ERRSTAT Bit Fields */
#define USB_ERRSTAT_PIDERR_MASK                  0x1u
#define USB_ERRSTAT_PIDERR_SHIFT                 0
#define USB_ERRSTAT_CRC5EOF_MASK                 0x2u
#define USB_ERRSTAT_CRC5EOF_SHIFT                1
#define USB_ERRSTAT_CRC16_MASK                   0x4u
#define USB_ERRSTAT_CRC16_SHIFT                  2
#define USB_ERRSTAT_DFN8_MASK                    0x8u
#define USB_ERRSTAT_DFN8_SHIFT                   3
#define USB_ERRSTAT_BTOERR_MASK                  0x10u
#define USB_ERRSTAT_BTOERR_SHIFT                 4
#define USB_ERRSTAT_DMAERR_MASK                  0x20u
#define USB_ERRSTAT_DMAERR_SHIFT                 5
#define USB_ERRSTAT_BTSERR_MASK                  0x80u
#define USB_ERRSTAT_BTSERR_SHIFT                 7
/* ERREN Bit Fields */
#define USB_ERREN_PIDERREN_MASK                  0x1u
#define USB_ERREN_PIDERREN_SHIFT                 0
#define USB_ERREN_CRC5EOFEN_MASK                 0x2u
#define USB_ERREN_CRC5EOFEN_SHIFT                1
#define USB_ERREN_CRC16EN_MASK                   0x4u
#define USB_ERREN_CRC16EN_SHIFT                  2
#define USB_ERREN_DFN8EN_MASK                    0x8u
#define USB_ERREN_DFN8EN_SHIFT                   3
#define USB_ERREN_BTOERREN_MASK                  0x10u
#define USB_ERREN_BTOERREN_SHIFT                 4
#define USB_ERREN_DMAERREN_MASK                  0x20u
#define USB_ERREN_DMAERREN_SHIFT                 5
#define USB_ERREN_BTSERREN_MASK                  0x80u
#define USB_ERREN_BTSERREN_SHIFT                 7
/* STAT Bit Fields */
#define USB_STAT_ODD_MASK                        0x4u
#define USB_STAT_ODD_SHIFT                       2
#define USB_STAT_TX_MASK                         0x8u
#define USB_STAT_TX_SHIFT                        3
#define USB_STAT_ENDP_MASK                       0xF0u
#define USB_STAT_ENDP_SHIFT                      4
#define USB_STAT_ENDP(x)                         (((uint8_t)(((uint8_t)(x))<<USB_STAT_ENDP_SHIFT))&USB_STAT_ENDP_MASK)
/* CTL Bit Fields */
#define USB_CTL_USBENSOFEN_MASK                  0x1u
#define USB_CTL_USBENSOFEN_SHIFT                 0
#define USB_CTL_ODDRST_MASK                      0x2u
#define USB_CTL_ODDRST_SHIFT                     1
#define USB_CTL_RESUME_MASK                      0x4u
#define USB_CTL_RESUME_SHIFT                     2
#define USB_CTL_HOSTMODEEN_MASK                  0x8u
#define USB_CTL_HOSTMODEEN_SHIFT                 3
#define USB_CTL_RESET_MASK                       0x10u
#define USB_CTL_RESET_SHIFT                      4
#define USB_CTL_TXSUSPENDTOKENBUSY_MASK          0x20u
#define USB_CTL_TXSUSPENDTOKENBUSY_SHIFT         5
#define USB_CTL_SE0_MASK                         0x40u
#define USB_CTL_SE0_SHIFT                        6
#define USB_CTL_JSTATE_MASK                      0x80u
#define USB_CTL_JSTATE_SHIFT                     7
/* ADDR Bit Fields */
#define USB_ADDR_ADDR_MASK                       0x7Fu
#define USB_ADDR_ADDR_SHIFT                      0
#define USB_ADDR_ADDR(x)                         (((uint8_t)(((uint8_t)(x))<<USB_ADDR_ADDR_SHIFT))&USB_ADDR_ADDR_MASK)
#define USB_ADDR_LSEN_MASK                       0x80u
#define USB_ADDR_LSEN_SHIFT                      7
/* BDTPAGE1 Bit Fields */
#define USB_BDTPAGE1_BDTBA_MASK                  0xFEu
#define USB_BDTPAGE1_BDTBA_SHIFT                 1
#define USB_BDTPAGE1_BDTBA(x)                    (((uint8_t)(((uint8_t)(x))<<USB_BDTPAGE1_BDTBA_SHIFT))&USB_BDTPAGE1_BDTBA_MASK)
/* FRMNUML Bit Fields */
#define USB_FRMNUML_FRM_MASK                     0xFFu
#define USB_FRMNUML_FRM_SHIFT                    0
#define USB_FRMNUML_FRM(x)                       (((uint8_t)(((uint8_t)(x))<<USB_FRMNUML_FRM_SHIFT))&USB_FRMNUML_FRM_MASK)
/* FRMNUMH Bit Fields */
#define USB_FRMNUMH_FRM_MASK                     0x7u
#define USB_FRMNUMH_FRM_SHIFT                    0
#define USB_FRMNUMH_FRM(x)                       (((uint8_t)(((uint8_t)(x))<<USB_FRMNUMH_FRM_SHIFT))&USB_FRMNUMH_FRM_MASK)
/* TOKEN Bit Fields */
#define USB_TOKEN_TOKENENDPT_MASK                0xFu
#define USB_TOKEN_TOKENENDPT_SHIFT               0
#define USB_TOKEN_TOKENENDPT(x)                  (((uint8_t)(((uint8_t)(x))<<USB_TOKEN_TOKENENDPT_SHIFT))&USB_TOKEN_TOKENENDPT_MASK)
#define USB_TOKEN_TOKENPID_MASK                  0xF0u
#define USB_TOKEN_TOKENPID_SHIFT                 4
#define USB_TOKEN_TOKENPID(x)                    (((uint8_t)(((uint8_t)(x))<<USB_TOKEN_TOKENPID_SHIFT))&USB_TOKEN_TOKENPID_MASK)
/* SOFTHLD Bit Fields */
#define USB_SOFTHLD_CNT_MASK                     0xFFu
#define USB_SOFTHLD_CNT_SHIFT                    0
#define USB_SOFTHLD_CNT(x)                       (((uint8_t)(((uint8_t)(x))<<USB_SOFTHLD_CNT_SHIFT))&USB_SOFTHLD_CNT_MASK)
/* BDTPAGE2 Bit Fields */
#define USB_BDTPAGE2_BDTBA_MASK                  0xFFu
#define USB_BDTPAGE2_BDTBA_SHIFT                 0
#define USB_BDTPAGE2_BDTBA(x)                    (((uint8_t)(((uint8_t)(x))<<USB_BDTPAGE2_BDTBA_SHIFT))&USB_BDTPAGE2_BDTBA_MASK)
/* BDTPAGE3 Bit Fields */
#define USB_BDTPAGE3_BDTBA_MASK                  0xFFu
#define USB_BDTPAGE3_BDTBA_SHIFT                 0
#define USB_BDTPAGE3_BDTBA(x)                    (((uint8_t)(((uint8_t)(x))<<USB_BDTPAGE3_BDTBA_SHIFT))&USB_BDTPAGE3_BDTBA_MASK)
/* ENDPT Bit Fields */
#define USB_ENDPT_EPHSHK_MASK                    0x1u
#define USB_ENDPT_EPHSHK_SHIFT                   0
#define USB_ENDPT_EPSTALL_MASK                   0x2u
#define USB_ENDPT_EPSTALL_SHIFT                  1
#define USB_ENDPT_EPTXEN_MASK                    0x4u
#define USB_ENDPT_EPTXEN_SHIFT                   2
#define USB_ENDPT_EPRXEN_MASK                    0x8u
#define USB_ENDPT_EPRXEN_SHIFT                   3
#define USB_ENDPT_EPCTLDIS_MASK                  0x10u
#define USB_ENDPT_EPCTLDIS_SHIFT                 4
#define USB_ENDPT_RETRYDIS_MASK                  0x40u
#define USB_ENDPT_RETRYDIS_SHIFT                 6
#define USB_ENDPT_HOSTWOHUB_MASK                 0x80u
#define USB_ENDPT_HOSTWOHUB_SHIFT                7
/* USBCTRL Bit Fields */
#define USB_USBCTRL_PDE_MASK                     0x40u
#define USB_USBCTRL_PDE_SHIFT                    6
#define USB_USBCTRL_SUSP_MASK                    0x80u
#define USB_USBCTRL_SUSP_SHIFT                   7
/* OBSERVE Bit Fields */
#define USB_OBSERVE_DMPD_MASK                    0x10u
#define USB_OBSERVE_DMPD_SHIFT                   4
#define USB_OBSERVE_DPPD_MASK                    0x40u
#define USB_OBSERVE_DPPD_SHIFT                   6
#define USB_OBSERVE_DPPU_MASK                    0x80u
#define USB_OBSERVE_DPPU_SHIFT                   7
/* CONTROL Bit Fields */
#define USB_CONTROL_DPPULLUPNONOTG_MASK          0x10u
#define USB_CONTROL_DPPULLUPNONOTG_SHIFT         4
/* USBTRC0 Bit Fields */
#define USB_USBTRC0_USB_RESUME_INT_MASK          0x1u
#define USB_USBTRC0_USB_RESUME_INT_SHIFT         0
#define USB_USBTRC0_SYNC_DET_MASK                0x2u
#define USB_USBTRC0_SYNC_DET_SHIFT               1
#define USB_USBTRC0_USBRESMEN_MASK               0x20u
#define USB_USBTRC0_USBRESMEN_SHIFT              5
#define USB_USBTRC0_USBRESET_MASK                0x80u
#define USB_USBTRC0_USBRESET_SHIFT               7
/* USBFRMADJUST Bit Fields */
#define USB_USBFRMADJUST_ADJ_MASK                0xFFu
#define USB_USBFRMADJUST_ADJ_SHIFT               0
#define USB_USBFRMADJUST_ADJ(x)                  (((uint8_t)(((uint8_t)(x))<<USB_USBFRMADJUST_ADJ_SHIFT))&USB_USBFRMADJUST_ADJ_MASK)

/**
 * @}
 */ /* end of group USB_Register_Masks */


/* USB - Peripheral instance base addresses */
/** Peripheral USB0 base address */
#define USB0_BASE                                (0x40072000u)
/** Peripheral USB0 base pointer */
#define USB0                                     ((USB_Type *)USB0_BASE)
/** Array initializer of USB peripheral base pointers */
#define USB_BASES                                { USB0 }

/**
 * @}
 */ /* end of group USB_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- USBDCD Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup USBDCD_Peripheral_Access_Layer USBDCD Peripheral Access Layer
 * @{
 */

/** USBDCD - Register Layout Typedef */
typedef struct {
  __IO uint32_t CONTROL;                           /**< Control register, offset: 0x0 */
  __IO uint32_t CLOCK;                             /**< Clock register, offset: 0x4 */
  __I  uint32_t STATUS;                            /**< Status register, offset: 0x8 */
       uint8_t RESERVED_0[4];
  __IO uint32_t TIMER0;                            /**< TIMER0 register, offset: 0x10 */
  __IO uint32_t TIMER1;                            /**< TIMER1 register, offset: 0x14 */
  __IO uint32_t TIMER2;                            /**< TIMER2 register, offset: 0x18 */
} USBDCD_Type;

/* ----------------------------------------------------------------------------
   -- USBDCD Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup USBDCD_Register_Masks USBDCD Register Masks
 * @{
 */

/* CONTROL Bit Fields */
#define USBDCD_CONTROL_IACK_MASK                 0x1u
#define USBDCD_CONTROL_IACK_SHIFT                0
#define USBDCD_CONTROL_IF_MASK                   0x100u
#define USBDCD_CONTROL_IF_SHIFT                  8
#define USBDCD_CONTROL_IE_MASK                   0x10000u
#define USBDCD_CONTROL_IE_SHIFT                  16
#define USBDCD_CONTROL_START_MASK                0x1000000u
#define USBDCD_CONTROL_START_SHIFT               24
#define USBDCD_CONTROL_SR_MASK                   0x2000000u
#define USBDCD_CONTROL_SR_SHIFT                  25
/* CLOCK Bit Fields */
#define USBDCD_CLOCK_CLOCK_UNIT_MASK             0x1u
#define USBDCD_CLOCK_CLOCK_UNIT_SHIFT            0
#define USBDCD_CLOCK_CLOCK_SPEED_MASK            0xFFCu
#define USBDCD_CLOCK_CLOCK_SPEED_SHIFT           2
#define USBDCD_CLOCK_CLOCK_SPEED(x)              (((uint32_t)(((uint32_t)(x))<<USBDCD_CLOCK_CLOCK_SPEED_SHIFT))&USBDCD_CLOCK_CLOCK_SPEED_MASK)
/* STATUS Bit Fields */
#define USBDCD_STATUS_SEQ_RES_MASK               0x30000u
#define USBDCD_STATUS_SEQ_RES_SHIFT              16
#define USBDCD_STATUS_SEQ_RES(x)                 (((uint32_t)(((uint32_t)(x))<<USBDCD_STATUS_SEQ_RES_SHIFT))&USBDCD_STATUS_SEQ_RES_MASK)
#define USBDCD_STATUS_SEQ_STAT_MASK              0xC0000u
#define USBDCD_STATUS_SEQ_STAT_SHIFT             18
#define USBDCD_STATUS_SEQ_STAT(x)                (((uint32_t)(((uint32_t)(x))<<USBDCD_STATUS_SEQ_STAT_SHIFT))&USBDCD_STATUS_SEQ_STAT_MASK)
#define USBDCD_STATUS_ERR_MASK                   0x100000u
#define USBDCD_STATUS_ERR_SHIFT                  20
#define USBDCD_STATUS_TO_MASK                    0x200000u
#define USBDCD_STATUS_TO_SHIFT                   21
#define USBDCD_STATUS_ACTIVE_MASK                0x400000u
#define USBDCD_STATUS_ACTIVE_SHIFT               22
/* TIMER0 Bit Fields */
#define USBDCD_TIMER0_TUNITCON_MASK              0xFFFu
#define USBDCD_TIMER0_TUNITCON_SHIFT             0
#define USBDCD_TIMER0_TUNITCON(x)                (((uint32_t)(((uint32_t)(x))<<USBDCD_TIMER0_TUNITCON_SHIFT))&USBDCD_TIMER0_TUNITCON_MASK)
#define USBDCD_TIMER0_TSEQ_INIT_MASK             0x3FF0000u
#define USBDCD_TIMER0_TSEQ_INIT_SHIFT            16
#define USBDCD_TIMER0_TSEQ_INIT(x)               (((uint32_t)(((uint32_t)(x))<<USBDCD_TIMER0_TSEQ_INIT_SHIFT))&USBDCD_TIMER0_TSEQ_INIT_MASK)
/* TIMER1 Bit Fields */
#define USBDCD_TIMER1_TVDPSRC_ON_MASK            0x3FFu
#define USBDCD_TIMER1_TVDPSRC_ON_SHIFT           0
#define USBDCD_TIMER1_TVDPSRC_ON(x)              (((uint32_t)(((uint32_t)(x))<<USBDCD_TIMER1_TVDPSRC_ON_SHIFT))&USBDCD_TIMER1_TVDPSRC_ON_MASK)
#define USBDCD_TIMER1_TDCD_DBNC_MASK             0x3FF0000u
#define USBDCD_TIMER1_TDCD_DBNC_SHIFT            16
#define USBDCD_TIMER1_TDCD_DBNC(x)               (((uint32_t)(((uint32_t)(x))<<USBDCD_TIMER1_TDCD_DBNC_SHIFT))&USBDCD_TIMER1_TDCD_DBNC_MASK)
/* TIMER2 Bit Fields */
#define USBDCD_TIMER2_CHECK_DM_MASK              0xFu
#define USBDCD_TIMER2_CHECK_DM_SHIFT             0
#define USBDCD_TIMER2_CHECK_DM(x)                (((uint32_t)(((uint32_t)(x))<<USBDCD_TIMER2_CHECK_DM_SHIFT))&USBDCD_TIMER2_CHECK_DM_MASK)
#define USBDCD_TIMER2_TVDPSRC_CON_MASK           0x3FF0000u
#define USBDCD_TIMER2_TVDPSRC_CON_SHIFT          16
#define USBDCD_TIMER2_TVDPSRC_CON(x)             (((uint32_t)(((uint32_t)(x))<<USBDCD_TIMER2_TVDPSRC_CON_SHIFT))&USBDCD_TIMER2_TVDPSRC_CON_MASK)

/**
 * @}
 */ /* end of group USBDCD_Register_Masks */


/* USBDCD - Peripheral instance base addresses */
/** Peripheral USBDCD base address */
#define USBDCD_BASE                              (0x40035000u)
/** Peripheral USBDCD base pointer */
#define USBDCD                                   ((USBDCD_Type *)USBDCD_BASE)
/** Array initializer of USBDCD peripheral base pointers */
#define USBDCD_BASES                             { USBDCD }

/**
 * @}
 */ /* end of group USBDCD_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- VREF Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup VREF_Peripheral_Access_Layer VREF Peripheral Access Layer
 * @{
 */

/** VREF - Register Layout Typedef */
typedef struct {
  __IO uint8_t TRM;                                /**< VREF Trim Register, offset: 0x0 */
  __IO uint8_t SC;                                 /**< VREF Status and Control Register, offset: 0x1 */
} VREF_Type;

/* ----------------------------------------------------------------------------
   -- VREF Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup VREF_Register_Masks VREF Register Masks
 * @{
 */

/* TRM Bit Fields */
#define VREF_TRM_TRIM_MASK                       0x3Fu
#define VREF_TRM_TRIM_SHIFT                      0
#define VREF_TRM_TRIM(x)                         (((uint8_t)(((uint8_t)(x))<<VREF_TRM_TRIM_SHIFT))&VREF_TRM_TRIM_MASK)
#define VREF_TRM_CHOPEN_MASK                     0x40u
#define VREF_TRM_CHOPEN_SHIFT                    6
/* SC Bit Fields */
#define VREF_SC_MODE_LV_MASK                     0x3u
#define VREF_SC_MODE_LV_SHIFT                    0
#define VREF_SC_MODE_LV(x)                       (((uint8_t)(((uint8_t)(x))<<VREF_SC_MODE_LV_SHIFT))&VREF_SC_MODE_LV_MASK)
#define VREF_SC_VREFST_MASK                      0x4u
#define VREF_SC_VREFST_SHIFT                     2
#define VREF_SC_ICOMPEN_MASK                     0x20u
#define VREF_SC_ICOMPEN_SHIFT                    5
#define VREF_SC_REGEN_MASK                       0x40u
#define VREF_SC_REGEN_SHIFT                      6
#define VREF_SC_VREFEN_MASK                      0x80u
#define VREF_SC_VREFEN_SHIFT                     7

/**
 * @}
 */ /* end of group VREF_Register_Masks */


/* VREF - Peripheral instance base addresses */
/** Peripheral VREF base address */
#define VREF_BASE                                (0x40074000u)
/** Peripheral VREF base pointer */
#define VREF                                     ((VREF_Type *)VREF_BASE)
/** Array initializer of VREF peripheral base pointers */
#define VREF_BASES                               { VREF }

/**
 * @}
 */ /* end of group VREF_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- WDOG Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup WDOG_Peripheral_Access_Layer WDOG Peripheral Access Layer
 * @{
 */

/** WDOG - Register Layout Typedef */
typedef struct {
  __IO uint16_t STCTRLH;                           /**< Watchdog Status and Control Register High, offset: 0x0 */
  __IO uint16_t STCTRLL;                           /**< Watchdog Status and Control Register Low, offset: 0x2 */
  __IO uint16_t TOVALH;                            /**< Watchdog Time-out Value Register High, offset: 0x4 */
  __IO uint16_t TOVALL;                            /**< Watchdog Time-out Value Register Low, offset: 0x6 */
  __IO uint16_t WINH;                              /**< Watchdog Window Register High, offset: 0x8 */
  __IO uint16_t WINL;                              /**< Watchdog Window Register Low, offset: 0xA */
  __IO uint16_t REFRESH;                           /**< Watchdog Refresh register, offset: 0xC */
  __IO uint16_t UNLOCK;                            /**< Watchdog Unlock register, offset: 0xE */
  __IO uint16_t TMROUTH;                           /**< Watchdog Timer Output Register High, offset: 0x10 */
  __IO uint16_t TMROUTL;                           /**< Watchdog Timer Output Register Low, offset: 0x12 */
  __IO uint16_t RSTCNT;                            /**< Watchdog Reset Count register, offset: 0x14 */
  __IO uint16_t PRESC;                             /**< Watchdog Prescaler register, offset: 0x16 */
} WDOG_Type;

/* ----------------------------------------------------------------------------
   -- WDOG Register Masks
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup WDOG_Register_Masks WDOG Register Masks
 * @{
 */

/* STCTRLH Bit Fields */
#define WDOG_STCTRLH_WDOGEN_MASK                 0x1u
#define WDOG_STCTRLH_WDOGEN_SHIFT                0
#define WDOG_STCTRLH_CLKSRC_MASK                 0x2u
#define WDOG_STCTRLH_CLKSRC_SHIFT                1
#define WDOG_STCTRLH_IRQRSTEN_MASK               0x4u
#define WDOG_STCTRLH_IRQRSTEN_SHIFT              2
#define WDOG_STCTRLH_WINEN_MASK                  0x8u
#define WDOG_STCTRLH_WINEN_SHIFT                 3
#define WDOG_STCTRLH_ALLOWUPDATE_MASK            0x10u
#define WDOG_STCTRLH_ALLOWUPDATE_SHIFT           4
#define WDOG_STCTRLH_DBGEN_MASK                  0x20u
#define WDOG_STCTRLH_DBGEN_SHIFT                 5
#define WDOG_STCTRLH_STOPEN_MASK                 0x40u
#define WDOG_STCTRLH_STOPEN_SHIFT                6
#define WDOG_STCTRLH_WAITEN_MASK                 0x80u
#define WDOG_STCTRLH_WAITEN_SHIFT                7
#define WDOG_STCTRLH_TESTWDOG_MASK               0x400u
#define WDOG_STCTRLH_TESTWDOG_SHIFT              10
#define WDOG_STCTRLH_TESTSEL_MASK                0x800u
#define WDOG_STCTRLH_TESTSEL_SHIFT               11
#define WDOG_STCTRLH_BYTESEL_MASK                0x3000u
#define WDOG_STCTRLH_BYTESEL_SHIFT               12
#define WDOG_STCTRLH_BYTESEL(x)                  (((uint16_t)(((uint16_t)(x))<<WDOG_STCTRLH_BYTESEL_SHIFT))&WDOG_STCTRLH_BYTESEL_MASK)
#define WDOG_STCTRLH_DISTESTWDOG_MASK            0x4000u
#define WDOG_STCTRLH_DISTESTWDOG_SHIFT           14
/* STCTRLL Bit Fields */
#define WDOG_STCTRLL_INTFLG_MASK                 0x8000u
#define WDOG_STCTRLL_INTFLG_SHIFT                15
/* TOVALH Bit Fields */
#define WDOG_TOVALH_TOVALHIGH_MASK               0xFFFFu
#define WDOG_TOVALH_TOVALHIGH_SHIFT              0
#define WDOG_TOVALH_TOVALHIGH(x)                 (((uint16_t)(((uint16_t)(x))<<WDOG_TOVALH_TOVALHIGH_SHIFT))&WDOG_TOVALH_TOVALHIGH_MASK)
/* TOVALL Bit Fields */
#define WDOG_TOVALL_TOVALLOW_MASK                0xFFFFu
#define WDOG_TOVALL_TOVALLOW_SHIFT               0
#define WDOG_TOVALL_TOVALLOW(x)                  (((uint16_t)(((uint16_t)(x))<<WDOG_TOVALL_TOVALLOW_SHIFT))&WDOG_TOVALL_TOVALLOW_MASK)
/* WINH Bit Fields */
#define WDOG_WINH_WINHIGH_MASK                   0xFFFFu
#define WDOG_WINH_WINHIGH_SHIFT                  0
#define WDOG_WINH_WINHIGH(x)                     (((uint16_t)(((uint16_t)(x))<<WDOG_WINH_WINHIGH_SHIFT))&WDOG_WINH_WINHIGH_MASK)
/* WINL Bit Fields */
#define WDOG_WINL_WINLOW_MASK                    0xFFFFu
#define WDOG_WINL_WINLOW_SHIFT                   0
#define WDOG_WINL_WINLOW(x)                      (((uint16_t)(((uint16_t)(x))<<WDOG_WINL_WINLOW_SHIFT))&WDOG_WINL_WINLOW_MASK)
/* REFRESH Bit Fields */
#define WDOG_REFRESH_WDOGREFRESH_MASK            0xFFFFu
#define WDOG_REFRESH_WDOGREFRESH_SHIFT           0
#define WDOG_REFRESH_WDOGREFRESH(x)              (((uint16_t)(((uint16_t)(x))<<WDOG_REFRESH_WDOGREFRESH_SHIFT))&WDOG_REFRESH_WDOGREFRESH_MASK)
/* UNLOCK Bit Fields */
#define WDOG_UNLOCK_WDOGUNLOCK_MASK              0xFFFFu
#define WDOG_UNLOCK_WDOGUNLOCK_SHIFT             0
#define WDOG_UNLOCK_WDOGUNLOCK(x)                (((uint16_t)(((uint16_t)(x))<<WDOG_UNLOCK_WDOGUNLOCK_SHIFT))&WDOG_UNLOCK_WDOGUNLOCK_MASK)
/* TMROUTH Bit Fields */
#define WDOG_TMROUTH_TIMEROUTHIGH_MASK           0xFFFFu
#define WDOG_TMROUTH_TIMEROUTHIGH_SHIFT          0
#define WDOG_TMROUTH_TIMEROUTHIGH(x)             (((uint16_t)(((uint16_t)(x))<<WDOG_TMROUTH_TIMEROUTHIGH_SHIFT))&WDOG_TMROUTH_TIMEROUTHIGH_MASK)
/* TMROUTL Bit Fields */
#define WDOG_TMROUTL_TIMEROUTLOW_MASK            0xFFFFu
#define WDOG_TMROUTL_TIMEROUTLOW_SHIFT           0
#define WDOG_TMROUTL_TIMEROUTLOW(x)              (((uint16_t)(((uint16_t)(x))<<WDOG_TMROUTL_TIMEROUTLOW_SHIFT))&WDOG_TMROUTL_TIMEROUTLOW_MASK)
/* RSTCNT Bit Fields */
#define WDOG_RSTCNT_RSTCNT_MASK                  0xFFFFu
#define WDOG_RSTCNT_RSTCNT_SHIFT                 0
#define WDOG_RSTCNT_RSTCNT(x)                    (((uint16_t)(((uint16_t)(x))<<WDOG_RSTCNT_RSTCNT_SHIFT))&WDOG_RSTCNT_RSTCNT_MASK)
/* PRESC Bit Fields */
#define WDOG_PRESC_PRESCVAL_MASK                 0x700u
#define WDOG_PRESC_PRESCVAL_SHIFT                8
#define WDOG_PRESC_PRESCVAL(x)                   (((uint16_t)(((uint16_t)(x))<<WDOG_PRESC_PRESCVAL_SHIFT))&WDOG_PRESC_PRESCVAL_MASK)

/**
 * @}
 */ /* end of group WDOG_Register_Masks */


/* WDOG - Peripheral instance base addresses */
/** Peripheral WDOG base address */
#define WDOG_BASE                                (0x40052000u)
/** Peripheral WDOG base pointer */
#define WDOG                                     ((WDOG_Type *)WDOG_BASE)
/** Array initializer of WDOG peripheral base pointers */
#define WDOG_BASES                               { WDOG }

/**
 * @}
 */ /* end of group WDOG_Peripheral_Access_Layer */


/*
** End of section using anonymous unions
*/

#if defined(__ARMCC_VERSION)
  #pragma pop
#elif defined(__CWCC__)
  #pragma pop
#elif defined(__GNUC__)
  /* leave anonymous unions enabled */
#elif defined(__IAR_SYSTEMS_ICC__)
  #pragma language=default
#else
  #error Not supported compiler type
#endif

/**
 * @}
 */ /* end of group Peripheral_access_layer */


/* ----------------------------------------------------------------------------
   -- Backward Compatibility
   ---------------------------------------------------------------------------- */

/**
 * @addtogroup Backward_Compatibility_Symbols Backward Compatibility
 * @{
 */

#define SDHC_WML_WRBRSTLEN_MASK                  This_symbol_has_been_deprecated
#define SDHC_WML_WRBRSTLEN_SHIFT                 This_symbol_has_been_deprecated
#define SDHC_WML_WRBRSTLEN(x)                    This_symbol_has_been_deprecated
#define CAN_IMASK2_BUFHM_MASK                    This_symbol_has_been_deprecated
#define CAN_IMASK2_BUFHM_SHIFT                   This_symbol_has_been_deprecated
#define CAN_IMASK2_BUFHM(x)                      This_symbol_has_been_deprecated
#define CAN_IFLAG2_BUFHI_MASK                    This_symbol_has_been_deprecated
#define CAN_IFLAG2_BUFHI_SHIFT                   This_symbol_has_been_deprecated
#define CAN_IFLAG2_BUFHI(x)                      This_symbol_has_been_deprecated
#define DAC_DATL_DATA_MASK                       DAC_DATL_DATA0_MASK
#define DAC_DATL_DATA_SHIFT                      DAC_DATL_DATA0_SHIFT
#define DAC_DATL_DATA(x)                         DAC_DATL_DATA0(x)
#define DAC_DATH_DATA_MASK                       DAC_DATH_DATA1_MASK
#define DAC_DATH_DATA_SHIFT                      DAC_DATH_DATA1_SHIFT
#define DAC_DATH_DATA(x)                         DAC_DATH_DATA1(x)
#define MCG_S_LOLS0_MASK                         MCG_S_LOLS_MASK
#define MCG_S_LOLS0_SHIFT                        MCG_S_LOLS_SHIFT
#define SIM_SCGC6_RNGA_MASK                      This_symbol_has_been_deprecated
#define SIM_SCGC6_RNGA_SHIFT                     This_symbol_has_been_deprecated

/**
 * @}
 */ /* end of group Backward_Compatibility_Symbols */


#endif  /* #if !defined(MK60D10_H_) */

/* MK60D10.h, eof. */
