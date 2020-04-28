/**
  ******************************************************************************
  * @file    stm8l15x_wfe.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the WFE firmware
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
#ifndef __STM8L15x_WFE_H
#define __STM8L15x_WFE_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
  
/** @addtogroup WFE
  * @{
  */ 
/* Exported types ------------------------------------------------------------*/

/** @defgroup WFE_Exported_Types
  * @{
  */
/** @defgroup WFE_Sources
  * @brief Signal Sources to generate wake_up events
@verbatim    
  Elements values convention: 0xXXYY
    X = SRx registers index
        XX = 01 : CR1
        XX = 02 : CR2
        XX = 03 : CR3
        XX = 04 : CR4
    YY = flag mask in the dedicated register(XX register)
@endverbatim
  * @{
  */
typedef enum {
  WFE_Source_TIM2_EV0     = (uint16_t)0x0101, /*!< TIM2 Update/Trigger and Break interrupt */
  WFE_Source_TIM2_EV1     = (uint16_t)0x0102, /*!< TIM2 Capture/Compare interrupt          */
  WFE_Source_TIM1_EV0     = (uint16_t)0x0104, /*!< TIM1 Update/Trigger and Break interrupt */
  WFE_Source_TIM1_EV1     = (uint16_t)0x0108, /*!< TIM1 Capture/Compare interrupt          */
  WFE_Source_EXTI_EV0     = (uint16_t)0x0110, /*!< I/O port interrupt from Pins 0          */
  WFE_Source_EXTI_EV1     = (uint16_t)0x0120, /*!< I/O port interrupt from Pins 1          */
  WFE_Source_EXTI_EV2     = (uint16_t)0x0140, /*!< I/O port interrupt from Pins 2          */
  WFE_Source_EXTI_EV3     = (uint16_t)0x0180, /*!< I/O port interrupt from Pins 3          */
  WFE_Source_EXTI_EV4     = (uint16_t)0x0201, /*!< I/O port interrupt from Pins 4          */
  WFE_Source_EXTI_EV5     = (uint16_t)0x0202, /*!< I/O port interrupt from Pins 5          */
  WFE_Source_EXTI_EV6     = (uint16_t)0x0204, /*!< I/O port interrupt from Pins 6          */
  WFE_Source_EXTI_EV7     = (uint16_t)0x0208, /*!< I/O port interrupt from Pins 7          */
  WFE_Source_EXTI_EVB_G   = (uint16_t)0x0210, /*!< I/O port interrupt from port B and G    */
  WFE_Source_EXTI_EVD_H   = (uint16_t)0x0220, /*!< I/O port interrupt from Port D and H    */
  WFE_Source_EXTI_EVE_F   = (uint16_t)0x0240, /*!< I/O port interrupt from Port E and F    */
  WFE_Source_ADC1_COMP_EV = (uint16_t)0x0280, /*!< ADC1, COMP1 and COMP2 interrupts        */
  WFE_Source_TIM3_EV0     = (uint16_t)0x0301, /*!< TIM3 Update/Trigger and Break interrupt */
  WFE_Source_TIM3_EV1     = (uint16_t)0x0302, /*!< TIM3 Capture/Compare interrupt          */
  WFE_Source_TIM4_EV      = (uint16_t)0x0304, /*!< TIM4 Update and Trigger interrupt       */
  WFE_Source_SPI1_EV      = (uint16_t)0x0308, /*!< SPI1 Rx and Tx interrupt                */
  WFE_Source_I2C1_EV      = (uint16_t)0x0310, /*!< I2C1 Rx and Tx interrupt                */
  WFE_Source_USART1_EV    = (uint16_t)0x0320, /*!< USART1 Rx and Tx interrupt              */
  WFE_Source_DMA1CH01_EV  = (uint16_t)0x0340, /*!< DMA1 channel 0 and  1 interrupt         */
  WFE_Source_DMA1CH23_EV  = (uint16_t)0x0380,  /*!< DMA1 channel 2 and  3  interrupt        */
  WFE_Source_RTC_CSS_EV   = (uint16_t)0x0401,  /*!< RTC or CSS on LSE event */
  WFE_Source_SPI2_EV      = (uint16_t)0x0402,  /*!< SPI2 Rx and Tx interrupt */
  WFE_Source_USART2_EV    = (uint16_t)0x0404,  /*!< USART2 Rx and Tx interrupt */
  WFE_Source_USART3_EV    = (uint16_t)0x0408,  /*!< USART3 Rx and Tx interrupt */
  WFE_Source_TIM5_EV0     = (uint16_t)0x0410,  /*!< TIM5 Update/Trigger and Break interrupt */
  WFE_Source_TIM5_EV1     = (uint16_t)0x0420,  /*!< TIM5 Capture/Compare interrupt */
  WFE_Source_AES_EV       = (uint16_t)0x0440   /*!< AES interrupt */
} WFE_Source_TypeDef;

#define IS_WFE_SOURCE(Source) (((Source) == WFE_Source_TIM2_EV0) || \
                               ((Source) == WFE_Source_TIM2_EV1) || \
                               ((Source) == WFE_Source_TIM1_EV0) || \
                               ((Source) == WFE_Source_TIM1_EV1) || \
                               ((Source) == WFE_Source_EXTI_EV0) || \
                               ((Source) == WFE_Source_EXTI_EV1) || \
                               ((Source) == WFE_Source_EXTI_EV2) || \
                               ((Source) == WFE_Source_EXTI_EV3) || \
                               ((Source) == WFE_Source_EXTI_EV4) || \
                               ((Source) == WFE_Source_EXTI_EV5) || \
                               ((Source) == WFE_Source_EXTI_EV6) || \
                               ((Source) == WFE_Source_EXTI_EV7) || \
                               ((Source) == WFE_Source_EXTI_EVB_G) || \
                               ((Source) == WFE_Source_EXTI_EVD_H) || \
                               ((Source) == WFE_Source_EXTI_EVE_F) || \
                               ((Source) == WFE_Source_ADC1_COMP_EV) || \
                               ((Source) == WFE_Source_TIM3_EV0) || \
                               ((Source) == WFE_Source_TIM3_EV1) || \
                               ((Source) == WFE_Source_TIM4_EV) || \
                               ((Source) == WFE_Source_SPI1_EV) || \
                               ((Source) == WFE_Source_I2C1_EV) || \
                               ((Source) == WFE_Source_USART1_EV) || \
                               ((Source) == WFE_Source_DMA1CH01_EV) || \
                               ((Source) == WFE_Source_AES_EV) || \
                               ((Source) == WFE_Source_TIM5_EV1) || \
                               ((Source) == WFE_Source_TIM5_EV0) || \
                               ((Source) == WFE_Source_USART3_EV) || \
                               ((Source) == WFE_Source_USART2_EV) || \
                               ((Source) == WFE_Source_SPI2_EV) || \
                               ((Source) == WFE_Source_RTC_CSS_EV) || \
                               ((Source) == WFE_Source_DMA1CH23_EV))

/**
  * @}
  */

/**
  * @}
  */
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* Function used to set the WFE configuration to the default reset state ******/ 
void WFE_DeInit(void);

/* WFE Source configuration and management functions **************************/ 
void WFE_WakeUpSourceEventCmd(WFE_Source_TypeDef WFE_Source, FunctionalState NewState);
FunctionalState WFE_GetWakeUpSourceEventStatus(WFE_Source_TypeDef WFE_Source);

#endif /* __STM8L15x_WFE_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
