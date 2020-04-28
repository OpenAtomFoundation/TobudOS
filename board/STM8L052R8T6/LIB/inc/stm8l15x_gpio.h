/**
  ******************************************************************************
  * @file    stm8l15x_gpio.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the GPIO firmware
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
#ifndef __STM8L15x_GPIO_H
#define __STM8L15x_GPIO_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
  
/** @addtogroup I2C
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup GPIO_Exported_Types
  * @{
  */

/**
  * @defgroup GPIO_Modes
  *
  * @brief
  *     
  * Bits definitions:
  * - Bit 7: 0 = INPUT mode
  *          1 = OUTPUT mode
  *          1 = PULL-UP (input) or PUSH-PULL (output)
  * - Bit 5: 0 = No external interrupt (input) or No slope control (output)
  *          1 = External interrupt (input) or Slow control enabled (output)
  * - Bit 4: 0 = Low level (output)
  *          1 = High level (output push-pull) or HI-Z (output open-drain)
  * @{
  */
typedef enum
{
  GPIO_Mode_In_FL_No_IT      = (uint8_t)0x00,   /*!< Input floating, no external interrupt */
  GPIO_Mode_In_PU_No_IT      = (uint8_t)0x40,   /*!< Input pull-up, no external interrupt */
  GPIO_Mode_In_FL_IT         = (uint8_t)0x20,   /*!< Input floating, external interrupt */
  GPIO_Mode_In_PU_IT         = (uint8_t)0x60,   /*!< Input pull-up, external interrupt */
  GPIO_Mode_Out_OD_Low_Fast  = (uint8_t)0xA0,   /*!< Output open-drain, low level, 10MHz */
  GPIO_Mode_Out_PP_Low_Fast  = (uint8_t)0xE0,   /*!< Output push-pull, low level, 10MHz */
  GPIO_Mode_Out_OD_Low_Slow  = (uint8_t)0x80,   /*!< Output open-drain, low level, 2MHz */
  GPIO_Mode_Out_PP_Low_Slow  = (uint8_t)0xC0,   /*!< Output push-pull, low level, 2MHz */
  GPIO_Mode_Out_OD_HiZ_Fast  = (uint8_t)0xB0,   /*!< Output open-drain, high-impedance level, 10MHz */
  GPIO_Mode_Out_PP_High_Fast = (uint8_t)0xF0,   /*!< Output push-pull, high level, 10MHz */
  GPIO_Mode_Out_OD_HiZ_Slow  = (uint8_t)0x90,   /*!< Output open-drain, high-impedance level, 2MHz */
  GPIO_Mode_Out_PP_High_Slow = (uint8_t)0xD0    /*!< Output push-pull, high level, 2MHz */
}GPIO_Mode_TypeDef;

/**
  * @}
  */
  
/** @defgroup GPIO_Pin
  * @{
  */

typedef enum
{
  GPIO_Pin_0    = ((uint8_t)0x01),   /*!< Pin 0 selected */
  GPIO_Pin_1    = ((uint8_t)0x02),   /*!< Pin 1 selected */
  GPIO_Pin_2    = ((uint8_t)0x04),   /*!< Pin 2 selected */
  GPIO_Pin_3    = ((uint8_t)0x08),   /*!< Pin 3 selected */
  GPIO_Pin_4    = ((uint8_t)0x10),   /*!< Pin 4 selected */
  GPIO_Pin_5    = ((uint8_t)0x20),   /*!< Pin 5 selected */
  GPIO_Pin_6    = ((uint8_t)0x40),   /*!< Pin 6 selected */
  GPIO_Pin_7    = ((uint8_t)0x80),   /*!< Pin 7 selected */
  GPIO_Pin_LNib = ((uint8_t)0x0F),   /*!< Low nibble pins selected */
  GPIO_Pin_HNib = ((uint8_t)0xF0),   /*!< High nibble pins selected */
  GPIO_Pin_All  = ((uint8_t)0xFF)    /*!< All pins selected */
}GPIO_Pin_TypeDef;

/**
  * @}
  */
  
/**
  * @}
  */
  
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/** @addtogroup GPIO_Exported_Macros
  * @{
  */

/**
  * @brief Macro used by the assert function to check the different functions parameters.
  */

/**
  * @brief Macro used by the assert function in order to check the different
  * values of GPIOMode_TypeDef.
  */
#define IS_GPIO_MODE(MODE) \
  (((MODE) == GPIO_Mode_In_FL_No_IT)       || \
   ((MODE) == GPIO_Mode_In_PU_No_IT)       || \
   ((MODE) == GPIO_Mode_In_FL_IT)          || \
   ((MODE) == GPIO_Mode_In_PU_IT)          || \
   ((MODE) == GPIO_Mode_Out_OD_Low_Fast)   || \
   ((MODE) == GPIO_Mode_Out_PP_Low_Fast)   || \
   ((MODE) == GPIO_Mode_Out_OD_Low_Slow)   || \
   ((MODE) == GPIO_Mode_Out_PP_Low_Slow)   || \
   ((MODE) == GPIO_Mode_Out_OD_HiZ_Fast)   || \
   ((MODE) == GPIO_Mode_Out_PP_High_Fast)  || \
   ((MODE) == GPIO_Mode_Out_OD_HiZ_Slow)   || \
   ((MODE) == GPIO_Mode_Out_PP_High_Slow))

/**
  * @brief Macro used by the assert function in order to check the different
  * values of GPIO_Pins.
  */
#define IS_GPIO_PIN(PIN) ((PIN) != (uint8_t)0x00)

/**
  * @}
  */


/* Exported functions ------------------------------------------------------- */
/* Initialization and Configuration *******************************************/
void GPIO_DeInit(GPIO_TypeDef* GPIOx);
void GPIO_Init(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin, GPIO_Mode_TypeDef GPIO_Mode);
void GPIO_ExternalPullUpConfig(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin, FunctionalState NewState);

/* GPIO Read and Write ********************************************************/
void GPIO_Write(GPIO_TypeDef* GPIOx, uint8_t GPIO_PortVal);
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin, BitAction GPIO_BitVal);
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin);
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin);
void GPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin);
uint8_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);
uint8_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);
BitStatus GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin);
BitStatus GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin);

#endif /* __STM8L15x_GPIO_H */

/**
  * @}
  */
  
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
