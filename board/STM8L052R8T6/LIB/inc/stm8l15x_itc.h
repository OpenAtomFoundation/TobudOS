/**
  ******************************************************************************
  * @file    stm8l15x_itc.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the ITC firmware
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
#ifndef __STM8L15x_ITC_H
#define __STM8L15x_ITC_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
  
/** @addtogroup ITC
  * @{
  */ 
/* Exported types ------------------------------------------------------------*/

/** @defgroup ITC_Exported_Types
  * @{
  */

/** @defgroup ITC_Interrupt_Lines_selection
  * @{
  */
typedef enum {
  FLASH_IRQn                               = (uint8_t)1,   /*!< Flash interrupt  */
  DMA1_CHANNEL0_1_IRQn                     = (uint8_t)2,   /*!< DMA Channels 0/1 */
  DMA1_CHANNEL2_3_IRQn                     = (uint8_t)3,   /*!< DMA Channels 2/3 */
  EXTIE_F_PVD_IRQn                         = (uint8_t)5,   /*!< GPIOE/F and PVD interrupt */
  EXTI0_IRQn                               = (uint8_t)8,   /*!< PIN0 interrupt   */
  EXTI1_IRQn                               = (uint8_t)9,   /*!< PIN1 interrupt   */
  EXTI2_IRQn                               = (uint8_t)10,  /*!< PIN2 interrupt   */
  EXTI3_IRQn                               = (uint8_t)11,  /*!< PIN3 interrupt   */
  EXTI4_IRQn                               = (uint8_t)12,  /*!< PIN4 interrupt   */
  EXTI5_IRQn                               = (uint8_t)13,  /*!< PIN5 interrupt   */
  EXTI6_IRQn                               = (uint8_t)14,  /*!< PIN6 interrupt   */
  EXTI7_IRQn                               = (uint8_t)15,  /*!< PIN7 interrupt   */
  ADC1_COMP_IRQn                           = (uint8_t)18,  /*!< ADC1/Comparator interrupt */
  TIM4_UPD_OVF_TRG_IRQn                    = (uint8_t)25,  /*!< TIM4 Update/Overflow/Trigger interrupt */
  SPI1_IRQn                                = (uint8_t)26,  /*!< SPI1 interrupt */
#if defined (STM8L15X_MD) || defined (STM8L05X_MD_VL) || defined (STM8AL31_L_MD)
  RTC_IRQn                                 = (uint8_t)4,   /*!< RTC interrupt    */
  EXTIB_IRQn                               = (uint8_t)6,   /*!< GPIOB interrupt  */
  EXTID_IRQn                               = (uint8_t)7,   /*!< GPIOD interrupt  */
  LCD_IRQn                                 = (uint8_t)16,  /*!< LCD Driver interrupt    */
	SWITCH_CSS_BREAK_DAC_IRQn                = (uint8_t)17,  /*!< Clock switch/CSS interrupt/TIM1 Break /DAC  interrupt */
  TIM2_UPD_OVF_TRG_BRK_IRQn                = (uint8_t)19,  /*!< TIM2 Update/Overflow/Trigger/Break interrupt*/
  TIM2_CC_IRQn                             = (uint8_t)20,  /*!< TIM2 input captute/output compare interrupt  */
  TIM3_UPD_OVF_TRG_BRK_IRQn                = (uint8_t)21,  /*!< TIM3 Update/Overflow/Trigger/Break interrupt */
  TIM3_CC_IRQn                             = (uint8_t)22,  /*!< TIM3 capture/compare interrupt  */
	TIM1_UPD_OVF_TRG_IRQn                    = (uint8_t)23,  /*!< TIM1 TIM1 Update/Overflow/Trigger interrupt */
  TIM1_CC_IRQn                             = (uint8_t)24,  /*!< TIM1 capture/compare interrupt  */
  USART1_TX_IRQn                           = (uint8_t)27,  /*!< USART1 TX interrupt */
  USART1_RX_IRQn                           = (uint8_t)28,  /*!< USART1 RX interrupt */
  I2C1_IRQn                                = (uint8_t)29   /*!< I2C1 interrupt */
#elif defined (STM8L15X_LD) || defined (STM8L05X_LD_VL)
  RTC_CSSLSE_IRQn                          = (uint8_t)4,   /*!< RTC / CSSLSE interrupt    */
  EXTIB_IRQn                               = (uint8_t)6,   /*!< GPIOB interrupt  */
  EXTID_IRQn                               = (uint8_t)7,   /*!< GPIOD interrupt  */
	SWITCH_CSS_IRQn                          = (uint8_t)17,  /*!< Clock switch/CSS interrupt/TIM1 Break /DAC  interrupt */
  TIM2_UPD_OVF_TRG_BRK_IRQn                = (uint8_t)19,  /*!< TIM2 Update/Overflow/Trigger/Break interrupt*/
  TIM2_CC_IRQn                             = (uint8_t)20,  /*!< TIM2 input captute/output compare interrupt  */
  TIM3_UPD_OVF_TRG_BRK_IRQn                = (uint8_t)21,  /*!< TIM3 Update/Overflow/Trigger/Break interrupt */
  TIM3_CC_IRQn                             = (uint8_t)22,  /*!< TIM3 capture/compare interrupt  */
  USART1_TX_IRQn                           = (uint8_t)27,  /*!< USART1 TX interrupt */
  USART1_RX_IRQn                           = (uint8_t)28,  /*!< USART1 RX interrupt */
  I2C1_IRQn                                = (uint8_t)29   /*!< I2C1 interrupt */
#elif defined (STM8L15X_HD) || defined (STM8L15X_MDP) || defined (STM8L05X_HD_VL)
  RTC_CSSLSE_IRQn                          = (uint8_t)4,   /*!< RTC / CSSLSE interrupt    */
  EXTIB_G_IRQn                             = (uint8_t)6,   /*!< GPIOB / G interrupt  */
  EXTID_H_IRQn                             = (uint8_t)7,   /*!< GPIOD / H interrupt  */
  LCD_AES_IRQn                             = (uint8_t)16,  /*!< LCD / AES interrupt    */
	SWITCH_CSS_BREAK_DAC_IRQn                = (uint8_t)17,  /*!< Clock switch/CSS interrupt/TIM1 Break /DAC  interrupt */
  TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQn      = (uint8_t)19,  /*!< TIM2 Update/Overflow/Trigger/Break /USART2 TX interrupt*/
  TIM2_CC_USART2_RX_IRQn                   = (uint8_t)20,  /*!< TIM2 capture/compare / USART2 RX interrupt  */
  TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQn      = (uint8_t)21,  /*!< TIM3 Update/Overflow/Trigger/Break / USART3 TX interrupt */
  TIM3_CC_USART3_RX_IRQn                   = (uint8_t)22,  /*!< TIM3 capture/compare / USART3 RX interrupt  */
	TIM1_UPD_OVF_TRG_IRQn                    = (uint8_t)23,  /*!< TIM1 TIM1 Update/Overflow/Trigger interrupt */
  TIM1_CC_IRQn                             = (uint8_t)24,  /*!< TIM1 capture/compare interrupt  */
  USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQn      = (uint8_t)27,  /*!< USART1 TX / TIM5 Update/Overflow/Trigger/Break interrupt */
  USART1_RX_TIM5_CC_IRQn                   = (uint8_t)28,  /*!< USART1 RX / TIM5 capture/compare interrupt */
  I2C1_SPI2_IRQn                           = (uint8_t)29   /*!< I2C1 / SPI2 interrupt */
#endif  /* STM8L15X_MD */
}IRQn_TypeDef;

#if defined (STM8L15X_MD) || defined (STM8L05X_MD_VL) || defined (STM8AL31_L_MD)
 #define IS_ITC_IRQ(Irq) (((Irq) == FLASH_IRQn) || \
                         ((Irq) == DMA1_CHANNEL0_1_IRQn) || \
                         ((Irq) == DMA1_CHANNEL2_3_IRQn) || \
                         ((Irq) == RTC_IRQn) || \
                         ((Irq) == EXTIE_F_PVD_IRQn) || \
                         ((Irq) == EXTIB_IRQn) || \
                         ((Irq) == EXTID_IRQn) || \
                         ((Irq) == EXTI0_IRQn) || \
                         ((Irq) == EXTI1_IRQn) || \
                         ((Irq) == EXTI2_IRQn) || \
                         ((Irq) == EXTI3_IRQn) || \
                         ((Irq) == EXTI4_IRQn) || \
                         ((Irq) == EXTI5_IRQn) || \
                         ((Irq) == EXTI6_IRQn) || \
                         ((Irq) == EXTI7_IRQn) || \
                         ((Irq) == LCD_IRQn) || \
                         ((Irq) == SWITCH_CSS_BREAK_DAC_IRQn) || \
                         ((Irq) == ADC1_COMP_IRQn) || \
                         ((Irq) == TIM2_UPD_OVF_TRG_BRK_IRQn) || \
                         ((Irq) == TIM2_CC_IRQn) || \
                         ((Irq) == TIM3_UPD_OVF_TRG_BRK_IRQn) || \
                         ((Irq) == TIM3_CC_IRQn) || \
                         ((Irq) == TIM1_UPD_OVF_TRG_IRQn) || \
                         ((Irq) == TIM1_CC_IRQn) || \
                         ((Irq) == TIM4_UPD_OVF_TRG_IRQn) || \
                         ((Irq) == SPI1_IRQn) || \
                         ((Irq) == USART1_TX_IRQn) || \
                         ((Irq) == USART1_RX_IRQn) || \
                         ((Irq) == I2C1_IRQn))
#elif defined (STM8L15X_LD) || defined (STM8L05X_LD_VL)
 #define IS_ITC_IRQ(Irq) (((Irq) == FLASH_IRQn) || \
                         ((Irq) == DMA1_CHANNEL0_1_IRQn) || \
                         ((Irq) == DMA1_CHANNEL2_3_IRQn) || \
                         ((Irq) == RTC_CSSLSE_IRQn) || \
                         ((Irq) == EXTIE_F_PVD_IRQn) || \
                         ((Irq) == EXTIB_IRQn) || \
                         ((Irq) == EXTID_IRQn) || \
                         ((Irq) == EXTI0_IRQn) || \
                         ((Irq) == EXTI1_IRQn) || \
                         ((Irq) == EXTI2_IRQn) || \
                         ((Irq) == EXTI3_IRQn) || \
                         ((Irq) == EXTI4_IRQn) || \
                         ((Irq) == EXTI5_IRQn) || \
                         ((Irq) == EXTI6_IRQn) || \
                         ((Irq) == EXTI7_IRQn) || \
                         ((Irq) == SWITCH_CSS_IRQn) || \
                         ((Irq) == ADC1_COMP_IRQn) || \
                         ((Irq) == TIM2_UPD_OVF_TRG_BRK_IRQn) || \
                         ((Irq) == TIM2_CC_IRQn) || \
                         ((Irq) == TIM3_UPD_OVF_TRG_BRK_IRQn) || \
                         ((Irq) == TIM3_CC_IRQn) || \
                         ((Irq) == TIM4_UPD_OVF_TRG_IRQn) || \
                         ((Irq) == SPI1_IRQn) || \
                         ((Irq) == USART1_TX_IRQn) || \
                         ((Irq) == USART1_RX_IRQn) || \
                         ((Irq) == I2C1_IRQn))												 
#elif defined (STM8L15X_HD) || defined (STM8L15X_MDP) || defined (STM8L05X_HD_VL)
 #define IS_ITC_IRQ(Irq) (((Irq) == FLASH_IRQn) || \
                         ((Irq) == DMA1_CHANNEL0_1_IRQn) || \
                         ((Irq) == DMA1_CHANNEL2_3_IRQn) || \
                         ((Irq) == RTC_CSSLSE_IRQn) || \
                         ((Irq) == EXTIE_F_PVD_IRQn) || \
                         ((Irq) == EXTIB_G_IRQn) || \
                         ((Irq) == EXTID_H_IRQn) || \
                         ((Irq) == EXTI0_IRQn) || \
                         ((Irq) == EXTI1_IRQn) || \
                         ((Irq) == EXTI2_IRQn) || \
                         ((Irq) == EXTI3_IRQn) || \
                         ((Irq) == EXTI4_IRQn) || \
                         ((Irq) == EXTI5_IRQn) || \
                         ((Irq) == EXTI6_IRQn) || \
                         ((Irq) == EXTI7_IRQn) || \
                         ((Irq) == LCD_AES_IRQn) || \
                         ((Irq) == SWITCH_CSS_BREAK_DAC_IRQn) || \
                         ((Irq) == ADC1_COMP_IRQn) || \
                         ((Irq) == TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQn) || \
                         ((Irq) == TIM2_CC_USART2_RX_IRQn) || \
                         ((Irq) == TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQn) || \
                         ((Irq) == TIM3_CC_USART3_RX_IRQn) || \
                         ((Irq) == TIM1_UPD_OVF_TRG_IRQn) || \
                         ((Irq) == TIM1_CC_IRQn) || \
                         ((Irq) == TIM4_UPD_OVF_TRG_IRQn) || \
                         ((Irq) == SPI1_IRQn) || \
                         ((Irq) == USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQn) || \
                         ((Irq) == USART1_RX_TIM5_CC_IRQn) || \
                         ((Irq) == I2C1_SPI2_IRQn))
#endif /* STM8L15X_MD */

/**
  * @}
  */
  
/** @defgroup ITC_Priority_Level_selection
  * @{
  */
typedef enum {
  ITC_PriorityLevel_0 = (uint8_t)0x02, /*!< Software priority level 0 (cannot be written) */
  ITC_PriorityLevel_1 = (uint8_t)0x01, /*!< Software priority level 1 */
  ITC_PriorityLevel_2 = (uint8_t)0x00, /*!< Software priority level 2 */
  ITC_PriorityLevel_3 = (uint8_t)0x03  /*!< Software priority level 3 */
} ITC_PriorityLevel_TypeDef;

#define IS_ITC_PRIORITY(PriorityValue) \
  (((PriorityValue) == ITC_PriorityLevel_0) || \
   ((PriorityValue) == ITC_PriorityLevel_1) || \
   ((PriorityValue) == ITC_PriorityLevel_2) || \
   ((PriorityValue) == ITC_PriorityLevel_3))

/**
  * @}
  */
  
/**
  * @}
  */
/* Exported constants --------------------------------------------------------*/

/** @defgroup ITC_Exported_Constants
  * @{
  */

#define CPU_SOFT_INT_DISABLED ((uint8_t)0x28) /*!< Mask for I1 and I0 bits in CPU_CC register */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/

/** @defgroup ITC_Exported_Constants
  * @{
  */
#define IS_ITC_INTERRUPTS_DISABLED (ITC_GetSoftIntStatus() == CPU_SOFT_INT_DISABLED)

/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */
/* Function used to set the ITC configuration to the default reset state ******/ 
void ITC_DeInit(void);

/* ITC configuration and management functions ******/ 
uint8_t ITC_GetCPUCC(void);
uint8_t ITC_GetSoftIntStatus(void);
void ITC_SetSoftwarePriority(IRQn_TypeDef IRQn, ITC_PriorityLevel_TypeDef ITC_PriorityLevel);
ITC_PriorityLevel_TypeDef ITC_GetSoftwarePriority(IRQn_TypeDef IRQn);

#endif /* __STM8L15x_ITC_H */

/**
  * @}
  */

/**
  * @}
  */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
