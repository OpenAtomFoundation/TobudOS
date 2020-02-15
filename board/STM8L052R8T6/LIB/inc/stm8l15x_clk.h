/**
  ******************************************************************************
  * @file    stm8l15x_clk.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the CLK firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8L15x_CLK_H
#define __STM8L15x_CLK_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @addtogroup CLK
  * @{
  */
/* Exported types ------------------------------------------------------------*/

/** @defgroup Exported_Types
  * @{
  */

/** @defgroup CLK_HSE_Configuration
  * @{
  */
typedef enum {
  CLK_HSE_OFF    = (uint8_t)0x00, /*!< HSE Disable */
  CLK_HSE_ON     = (uint8_t)0x01, /*!< HSE Enable */
  CLK_HSE_Bypass = (uint8_t)0x11  /*!< HSE Bypass and enable */
} CLK_HSE_TypeDef;

#define IS_CLK_HSE(CONFIG) (((CONFIG) == CLK_HSE_ON) ||\
                            ((CONFIG) == CLK_HSE_OFF)||\
                            ((CONFIG) == CLK_HSE_Bypass))
/**
  * @}
  */

/** @defgroup CLK_LSE_Configuration
  * @{
  */
typedef enum {
  CLK_LSE_OFF    = (uint8_t)0x00, /*!< LSE Disable */
  CLK_LSE_ON     = (uint8_t)0x04, /*!< LSE Enable */
  CLK_LSE_Bypass = (uint8_t)0x24  /*!< LSE Bypass and enable */
} CLK_LSE_TypeDef;

#define IS_CLK_LSE(CONFIG) (((CONFIG) == CLK_LSE_OFF) ||\
                            ((CONFIG) == CLK_LSE_ON)  ||\
                            ((CONFIG) == CLK_LSE_Bypass))
/**
  * @}
  */

/** @defgroup CLK_System_Clock_Sources
  * @{
  */
typedef enum {
  CLK_SYSCLKSource_HSI = (uint8_t)0x01, /*!< System Clock Source HSI */
  CLK_SYSCLKSource_LSI = (uint8_t)0x02, /*!< System Clock Source LSI */
  CLK_SYSCLKSource_HSE = (uint8_t)0x04, /*!< System Clock Source HSE */
  CLK_SYSCLKSource_LSE = (uint8_t)0x08  /*!< System Clock Source LSE */
} CLK_SYSCLKSource_TypeDef;

#define IS_CLK_SOURCE(SOURCE) (((SOURCE) == CLK_SYSCLKSource_HSI) ||\
                               ((SOURCE) == CLK_SYSCLKSource_LSI) ||\
                               ((SOURCE) == CLK_SYSCLKSource_HSE) ||\
                               ((SOURCE) == CLK_SYSCLKSource_LSE))
/**
  * @}
  */

/** @defgroup CLK_Clock_Output_Selection
  * @{
  */
typedef enum {
  CLK_CCOSource_Off = (uint8_t)0x00, /*!< Clock Output Off */
  CLK_CCOSource_HSI = (uint8_t)0x02, /*!< HSI Clock Output  */
  CLK_CCOSource_LSI = (uint8_t)0x04, /*!< LSI Clock Output */
  CLK_CCOSource_HSE = (uint8_t)0x08, /*!< HSE Clock Output */
  CLK_CCOSource_LSE = (uint8_t)0x10  /*!< LSE Clock Output */
} CLK_CCOSource_TypeDef;

#define IS_CLK_OUTPUT(OUTPUT)  (((OUTPUT) == CLK_CCOSource_Off)  ||\
                                ((OUTPUT) == CLK_CCOSource_HSI)  ||\
                                ((OUTPUT) == CLK_CCOSource_LSI)  ||\
                                ((OUTPUT) == CLK_CCOSource_HSE)  ||\
                                ((OUTPUT) == CLK_CCOSource_LSE))
/**
  * @}
  */

/** @defgroup CLK_Clock_Output_Prescaler
  * @{
  */
typedef enum {
  CLK_CCODiv_1  = (uint8_t)0x00, /*!< Clock Output Div 1 */
  CLK_CCODiv_2  = (uint8_t)0x20, /*!< Clock Output Div 2 */
  CLK_CCODiv_4  = (uint8_t)0x40, /*!< Clock Output Div 4 */
  CLK_CCODiv_8  = (uint8_t)0x60, /*!< Clock Output Div 8 */
  CLK_CCODiv_16 = (uint8_t)0x80, /*!< Clock Output Div 16 */
  CLK_CCODiv_32 = (uint8_t)0xA0, /*!< Clock Output Div 32 */
  CLK_CCODiv_64 = (uint8_t)0xC0  /*!< Clock Output Div 64 */
} CLK_CCODiv_TypeDef;

#define IS_CLK_OUTPUT_DIVIDER(PRESCALER) (((PRESCALER) == CLK_CCODiv_1)  ||\
                                          ((PRESCALER) == CLK_CCODiv_2)  ||\
                                          ((PRESCALER) == CLK_CCODiv_4)  ||\
                                          ((PRESCALER) == CLK_CCODiv_8)  ||\
                                          ((PRESCALER) == CLK_CCODiv_16) ||\
                                          ((PRESCALER) == CLK_CCODiv_32) ||\
                                          ((PRESCALER) == CLK_CCODiv_64))
/**
  * @}
  */

/** @defgroup CLK_Beep_Selection
  * @{
  */
typedef enum {
  CLK_BEEPCLKSource_Off = (uint8_t)0x00, /*!< Clock BEEP Off */
  CLK_BEEPCLKSource_LSI = (uint8_t)0x02, /*!< Clock BEEP : LSI */
  CLK_BEEPCLKSource_LSE = (uint8_t)0x04  /*!< Clock BEEP : LSE */
} CLK_BEEPCLKSource_TypeDef;

#define IS_CLK_CLOCK_BEEP(OUTPUT) (((OUTPUT) == CLK_BEEPCLKSource_Off)  ||\
                                   ((OUTPUT) == CLK_BEEPCLKSource_LSI)  ||\
                                   ((OUTPUT) == CLK_BEEPCLKSource_LSE))
/**
  * @}
  */

/** @defgroup CLK_RTC_Selection
  * @{
  */
typedef enum {
  CLK_RTCCLKSource_Off = (uint8_t)0x00, /*!< Clock RTC Off */
  CLK_RTCCLKSource_HSI = (uint8_t)0x02, /*!< Clock RTC : HSI */
  CLK_RTCCLKSource_LSI = (uint8_t)0x04, /*!< Clock RTC : LSI */
  CLK_RTCCLKSource_HSE = (uint8_t)0x08, /*!< Clock RTC : HSE */
  CLK_RTCCLKSource_LSE = (uint8_t)0x10  /*!< Clock RTC : LSE */
} CLK_RTCCLKSource_TypeDef;

#define IS_CLK_CLOCK_RTC(OUTPUT) (((OUTPUT) == CLK_RTCCLKSource_Off)  ||\
                                  ((OUTPUT) == CLK_RTCCLKSource_HSI)  ||\
                                  ((OUTPUT) == CLK_RTCCLKSource_LSI)  ||\
                                  ((OUTPUT) == CLK_RTCCLKSource_HSE)  ||\
                                  ((OUTPUT) == CLK_RTCCLKSource_LSE))
/**
  * @}
  */

/** @defgroup CLK_RTC_Prescaler
  * @{
  */
typedef enum {
  CLK_RTCCLKDiv_1  = (uint8_t)0x00, /*!< Clock RTC Div 1 */
  CLK_RTCCLKDiv_2  = (uint8_t)0x20, /*!< Clock RTC Div 2  */
  CLK_RTCCLKDiv_4  = (uint8_t)0x40, /*!< Clock RTC Div 4 */
  CLK_RTCCLKDiv_8  = (uint8_t)0x60, /*!< Clock RTC Div 8 */
  CLK_RTCCLKDiv_16 = (uint8_t)0x80, /*!< Clock RTC Div 16 */
  CLK_RTCCLKDiv_32 = (uint8_t)0xA0, /*!< Clock RTC  Div 32 */
  CLK_RTCCLKDiv_64 = (uint8_t)0xC0  /*!< Clock RTC  Div 64 */
} CLK_RTCCLKDiv_TypeDef;

#define IS_CLK_CLOCK_RTC_DIV(DIV) (((DIV) == CLK_RTCCLKDiv_1)  ||\
                                   ((DIV) == CLK_RTCCLKDiv_2)  ||\
                                   ((DIV) == CLK_RTCCLKDiv_4)  ||\
                                   ((DIV) == CLK_RTCCLKDiv_8)  ||\
                                   ((DIV) == CLK_RTCCLKDiv_16) ||\
                                   ((DIV) == CLK_RTCCLKDiv_32) ||\
                                   ((DIV) == CLK_RTCCLKDiv_64))
/**
  * @}
  */

/** @defgroup CLK_Peripherals
  * @{
  */
/* Elements values convention: 0xXY
        X = choice between the peripheral registers
        X = 0 : PCKENR1
        X = 1 : PCKENR2
        X = 2 : PCKENR3
        Y = Peripheral position in the register
  */
typedef enum {
  CLK_Peripheral_TIM2    = (uint8_t)0x00, /*!< Peripheral Clock Enable 1, TIM2 */
  CLK_Peripheral_TIM3    = (uint8_t)0x01, /*!< Peripheral Clock Enable 1, TIM3 */
  CLK_Peripheral_TIM4    = (uint8_t)0x02, /*!< Peripheral Clock Enable 1, TIM4 */
  CLK_Peripheral_I2C1    = (uint8_t)0x03, /*!< Peripheral Clock Enable 1, I2C1 */
  CLK_Peripheral_SPI1    = (uint8_t)0x04, /*!< Peripheral Clock Enable 1, SPI1 */
  CLK_Peripheral_USART1  = (uint8_t)0x05, /*!< Peripheral Clock Enable 1, USART1 */
  CLK_Peripheral_BEEP    = (uint8_t)0x06, /*!< Peripheral Clock Enable 1, BEEP */
  CLK_Peripheral_DAC     = (uint8_t)0x07, /*!< Peripheral Clock Enable 1, DAC */
  CLK_Peripheral_ADC1    = (uint8_t)0x10, /*!< Peripheral Clock Enable 2, ADC1 */
  CLK_Peripheral_TIM1    = (uint8_t)0x11, /*!< Peripheral Clock Enable 2, TIM1 */
  CLK_Peripheral_RTC     = (uint8_t)0x12, /*!< Peripheral Clock Enable 2, RTC */
  CLK_Peripheral_LCD     = (uint8_t)0x13, /*!< Peripheral Clock Enable 2, LCD */
  CLK_Peripheral_DMA1    = (uint8_t)0x14, /*!< Peripheral Clock Enable 2, DMA1 */
  CLK_Peripheral_COMP    = (uint8_t)0x15, /*!< Peripheral Clock Enable 2, COMP1 and COMP2 */
  CLK_Peripheral_BOOTROM = (uint8_t)0x17,/*!< Peripheral Clock Enable 2, Boot ROM */
  CLK_Peripheral_AES     = (uint8_t)0x20, /*!< Peripheral Clock Enable 3, AES */
  CLK_Peripheral_TIM5    = (uint8_t)0x21, /*!< Peripheral Clock Enable 3, TIM5 */
  CLK_Peripheral_SPI2    = (uint8_t)0x22, /*!< Peripheral Clock Enable 3, SPI2 */
  CLK_Peripheral_USART2  = (uint8_t)0x23, /*!< Peripheral Clock Enable 3, USART2 */
  CLK_Peripheral_USART3  = (uint8_t)0x24,  /*!< Peripheral Clock Enable 3, USART3 */
  CLK_Peripheral_CSSLSE  = (uint8_t)0x25   /*!< Peripheral Clock Enable 3, CSS on LSE */
} CLK_Peripheral_TypeDef;

#define IS_CLK_PERIPHERAL(PERIPHERAL) (((PERIPHERAL) == CLK_Peripheral_DAC)     ||\
                                       ((PERIPHERAL) == CLK_Peripheral_ADC1)    ||\
                                       ((PERIPHERAL) == CLK_Peripheral_DMA1)    ||\
                                       ((PERIPHERAL) == CLK_Peripheral_RTC)     ||\
                                       ((PERIPHERAL) == CLK_Peripheral_LCD)     ||\
                                       ((PERIPHERAL) == CLK_Peripheral_COMP)    ||\
                                       ((PERIPHERAL) == CLK_Peripheral_TIM1)    ||\
                                       ((PERIPHERAL) == CLK_Peripheral_USART1)  ||\
                                       ((PERIPHERAL) == CLK_Peripheral_SPI1)    ||\
                                       ((PERIPHERAL) == CLK_Peripheral_I2C1)    ||\
                                       ((PERIPHERAL) == CLK_Peripheral_TIM4)    ||\
                                       ((PERIPHERAL) == CLK_Peripheral_TIM3)    ||\
                                       ((PERIPHERAL) == CLK_Peripheral_BEEP)    ||\
                                       ((PERIPHERAL) == CLK_Peripheral_BOOTROM) ||\
                                       ((PERIPHERAL) == CLK_Peripheral_AES)     ||\
                                       ((PERIPHERAL) == CLK_Peripheral_TIM5)    ||\
                                       ((PERIPHERAL) == CLK_Peripheral_SPI2)    ||\
                                       ((PERIPHERAL) == CLK_Peripheral_USART2)  ||\
                                       ((PERIPHERAL) == CLK_Peripheral_USART3)  ||\
                                       ((PERIPHERAL) == CLK_Peripheral_CSSLSE)  ||\
                                       ((PERIPHERAL) == CLK_Peripheral_TIM2))
/**
  * @}
  */

/** @defgroup CLK_System_Clock_Divider
  * @{
  */
typedef enum {
  CLK_SYSCLKDiv_1   = (uint8_t)0x00, /*!< System Clock Divider: 1 */
  CLK_SYSCLKDiv_2   = (uint8_t)0x01, /*!< System Clock Divider: 2 */
  CLK_SYSCLKDiv_4   = (uint8_t)0x02, /*!< System Clock Divider: 4 */
  CLK_SYSCLKDiv_8   = (uint8_t)0x03, /*!< System Clock Divider: 8 */
  CLK_SYSCLKDiv_16  = (uint8_t)0x04, /*!< System Clock Divider: 16 */
  CLK_SYSCLKDiv_32  = (uint8_t)0x05, /*!< System Clock Divider: 32 */
  CLK_SYSCLKDiv_64  = (uint8_t)0x06, /*!< System Clock Divider: 64 */
  CLK_SYSCLKDiv_128 = (uint8_t)0x07  /*!< System Clock Divider: 128 */
} CLK_SYSCLKDiv_TypeDef;

#define IS_CLK_SYSTEM_DIVIDER(DIV) (((DIV) == CLK_SYSCLKDiv_1) ||\
                                    ((DIV) == CLK_SYSCLKDiv_2) ||\
                                    ((DIV) == CLK_SYSCLKDiv_4) ||\
                                    ((DIV) == CLK_SYSCLKDiv_8) ||\
                                    ((DIV) == CLK_SYSCLKDiv_16) ||\
                                    ((DIV) == CLK_SYSCLKDiv_32) ||\
                                    ((DIV) == CLK_SYSCLKDiv_64) ||\
                                    ((DIV) == CLK_SYSCLKDiv_128))
/**
  * @}
  */

/** @defgroup CLK_Flags
  * @{
  */
/* Elements values convention: 0xXY
    X = choice between the register's flags
       X = 0 : CLK_CRTCR
       X = 1 : CLK_ICKCR
       X = 2 : CLK_CCOR
       X = 3 : CLK_ECKCR
       X = 4 : CLK_SWCR
       X = 5 : CLK_CSSR
       X = 6 : CLK_CBEEPR
       X = 7 : CLK_REGCSRR
       X = 8 : CSSLSE_CSR
  Y = flag position in the register
*/
typedef enum {
  CLK_FLAG_RTCSWBSY    = (uint8_t)0x00, /*!< RTC clock busy in switch Flag */
  CLK_FLAG_HSIRDY      = (uint8_t)0x11, /*!< High speed internal oscillator ready Flag */
  CLK_FLAG_LSIRDY      = (uint8_t)0x13, /*!< Low speed internal oscillator ready Flag */
  CLK_FLAG_CCOBSY      = (uint8_t)0x20, /*!< Configurable clock output busy */
  CLK_FLAG_HSERDY      = (uint8_t)0x31, /*!< High speed external oscillator ready Flag */
  CLK_FLAG_LSERDY      = (uint8_t)0x33, /*!< Low speed external oscillator ready Flag */
  CLK_FLAG_SWBSY       = (uint8_t)0x40, /*!< Switch busy Flag */
  CLK_FLAG_AUX         = (uint8_t)0x51, /*!< Auxiliary oscillator connected to master clock */
  CLK_FLAG_CSSD        = (uint8_t)0x53, /*!< Clock security system detection Flag */
  CLK_FLAG_BEEPSWBSY   = (uint8_t)0x60, /*!< BEEP clock busy in switch Flag*/
  CLK_FLAG_EEREADY     = (uint8_t)0x77, /*!< Flash program memory and Data EEPROM ready Flag */
  CLK_FLAG_EEBUSY      = (uint8_t)0x76, /*!< Flash program memory and Data EEPROM busy Flag */
  CLK_FLAG_LSEPD       = (uint8_t)0x75, /*!< LSE power-down Flag */
  CLK_FLAG_HSEPD       = (uint8_t)0x74, /*!< HSE power-down Flag */
  CLK_FLAG_LSIPD       = (uint8_t)0x73, /*!< LSI power-down Flag */
  CLK_FLAG_HSIPD       = (uint8_t)0x72, /*!< HSI power-down Flag */
  CLK_FLAG_REGREADY    = (uint8_t)0x70,  /*!< REGREADY Flag */
  CLK_FLAG_LSECSSF     = (uint8_t)0x83,  /*!< CSS on LSE detection Flag */
  CLK_FLAG_RTCCLKSWF   = (uint8_t)0x84   /*!< RTCCLK switch completed flag on LSE failure */
}CLK_FLAG_TypeDef;

#define IS_CLK_FLAGS(FLAG) (((FLAG) == CLK_FLAG_LSIRDY) ||\
                            ((FLAG) == CLK_FLAG_HSIRDY) ||\
                            ((FLAG) == CLK_FLAG_HSERDY) ||\
                            ((FLAG) == CLK_FLAG_SWBSY) ||\
                            ((FLAG) == CLK_FLAG_CSSD) ||\
                            ((FLAG) == CLK_FLAG_AUX) ||\
                            ((FLAG) == CLK_FLAG_LSERDY) ||\
                            ((FLAG) == CLK_FLAG_CCOBSY) ||\
                            ((FLAG) == CLK_FLAG_RTCSWBSY) ||\
                            ((FLAG) == CLK_FLAG_EEREADY) ||\
                            ((FLAG) == CLK_FLAG_EEBUSY) ||\
                            ((FLAG) == CLK_FLAG_LSEPD) ||\
                            ((FLAG) == CLK_FLAG_LSIPD) ||\
                            ((FLAG) == CLK_FLAG_HSEPD) ||\
                            ((FLAG) == CLK_FLAG_HSIPD) ||\
                            ((FLAG) == CLK_FLAG_REGREADY) ||\
                            ((FLAG) == CLK_FLAG_BEEPSWBSY)||\
                            ((FLAG) == CLK_FLAG_LSECSSF)||\
                            ((FLAG) == CLK_FLAG_RTCCLKSWF))
/**
  * @}
  */

/** @defgroup CLK_Interrupts
  * @{
  */
typedef enum {
  CLK_IT_CSSD     = (uint8_t)0x0C,  /*!< Clock security system detection Flag */
  CLK_IT_SWIF     = (uint8_t)0x1C,  /*!< Clock switch interrupt Flag */
  CLK_IT_LSECSSF  = (uint8_t)0x2C   /*!< LSE Clock security system detection Interrupt */
}CLK_IT_TypeDef;
#define IS_CLK_IT(IT) (((IT) == CLK_IT_CSSD) ||\
                       ((IT) == CLK_IT_SWIF) ||\
                       ((IT) == CLK_IT_LSECSSF))

#define IS_CLK_CLEAR_IT(IT) (((IT) == CLK_IT_SWIF)||\
                             ((IT) == CLK_IT_LSECSSF))
/**
  * @}
  */

/** @defgroup CLK_Halt_Configuration
  * @{
  */
typedef enum {
  CLK_Halt_BEEPRunning = (uint8_t)0x40, /*!< BEEP clock Halt/Active-halt mode */
  CLK_Halt_FastWakeup  = (uint8_t)0x20, /*!< Fast wakeup from Halt/Active-halt modes */
  CLK_Halt_SlowWakeup  = (uint8_t)0x10  /*!< Slow Active-halt mode */
}
CLK_Halt_TypeDef;

#define IS_CLK_HALT(HALT) (((HALT) == CLK_Halt_BEEPRunning) ||\
                           ((HALT) == CLK_Halt_FastWakeup) ||\
                           ((HALT) == CLK_Halt_SlowWakeup))
/**
  * @}
  */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/* Function used to set the CLK configuration to the default reset state ******/
void CLK_DeInit(void);

/* Internal/external clocks, CSS and CCO configuration functions **************/
void CLK_HSICmd(FunctionalState NewState);
void CLK_AdjustHSICalibrationValue(uint8_t CLK_HSICalibrationValue);
void CLK_LSICmd(FunctionalState NewState);
void CLK_HSEConfig(CLK_HSE_TypeDef CLK_HSE);
void CLK_LSEConfig(CLK_LSE_TypeDef CLK_LSE);
void CLK_ClockSecuritySystemEnable(void);
void CLK_ClockSecuritySytemDeglitchCmd(FunctionalState NewState);
void CLK_CCOConfig(CLK_CCOSource_TypeDef CLK_CCOSource, CLK_CCODiv_TypeDef CLK_CCODiv);

/* System clocks configuration functions ******************/
void CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_TypeDef CLK_SYSCLKSource);
CLK_SYSCLKSource_TypeDef CLK_GetSYSCLKSource(void);
uint32_t CLK_GetClockFreq(void);
void CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_TypeDef CLK_SYSCLKDiv);
void CLK_SYSCLKSourceSwitchCmd(FunctionalState NewState);

/* Peripheral clocks configuration functions **********************************/
void CLK_RTCClockConfig(CLK_RTCCLKSource_TypeDef CLK_RTCCLKSource, CLK_RTCCLKDiv_TypeDef CLK_RTCCLKDiv);
void CLK_BEEPClockConfig(CLK_BEEPCLKSource_TypeDef CLK_BEEPCLKSource);
void CLK_PeripheralClockConfig(CLK_Peripheral_TypeDef CLK_Peripheral, FunctionalState NewState);

/* CSS on LSE configuration functions *****************************************/
void CLK_LSEClockSecuritySystemEnable(void);
void CLK_RTCCLKSwitchOnLSEFailureEnable(void);

/* Low power clock configuration functions ************************************/
void CLK_HaltConfig(CLK_Halt_TypeDef CLK_Halt, FunctionalState NewState);
void CLK_MainRegulatorCmd(FunctionalState NewState);

/* Interrupts and flags management functions **********************************/
void CLK_ITConfig(CLK_IT_TypeDef CLK_IT, FunctionalState NewState);
FlagStatus CLK_GetFlagStatus(CLK_FLAG_TypeDef CLK_FLAG);
void CLK_ClearFlag(void);
ITStatus CLK_GetITStatus(CLK_IT_TypeDef CLK_IT);
void CLK_ClearITPendingBit(CLK_IT_TypeDef CLK_IT);


#endif /* __STM8L15x_CLK_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
