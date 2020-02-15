/**
  ******************************************************************************
  * @file    stm8l15x_exti.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the EXTI firmware
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
#ifndef __STM8L15x_EXTI_H
#define __STM8L15x_EXTI_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @addtogroup EXTI
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup EXTI_Exported_Types
  * @{
  */

/** @defgroup EXTI_Trigger
  * @{
  */
typedef enum
{
  EXTI_Trigger_Falling_Low    = (uint8_t)0x00, /*!< Interrupt on Falling edge and Low level */
  EXTI_Trigger_Rising         = (uint8_t)0x01, /*!< Interrupt on Rising edge only */
  EXTI_Trigger_Falling        = (uint8_t)0x02, /*!< Interrupt on Falling edge only */
  EXTI_Trigger_Rising_Falling = (uint8_t)0x03  /*!< Interrupt on Rising and Falling edges */
} EXTI_Trigger_TypeDef;

/**
  * @}
  */

/** @defgroup EXTI_Half_Port
  *
  * @brief EXTI halfPort possible values
  * Values are coded as following:
  * - Bit 7: 0 => the half port is in EXTI_CONF1 register
  *          1 => the half port is in EXTI_CONF2 register
  * - Bits[6:0] => the half port selection mask
  * @{
  */
typedef enum
{
  EXTI_HalfPort_B_LSB  = (uint8_t)0x01, /*!< Interrupt selector PB(3:0) */
  EXTI_HalfPort_B_MSB  = (uint8_t)0x02, /*!< Interrupt selector PB(7:4) */
  EXTI_HalfPort_D_LSB  = (uint8_t)0x04, /*!< Interrupt selector PD(3:0) */
  EXTI_HalfPort_D_MSB  = (uint8_t)0x08, /*!< Interrupt selector PD(7:4) */
  EXTI_HalfPort_E_LSB  = (uint8_t)0x10, /*!< Interrupt selector PE(3:0) */
  EXTI_HalfPort_E_MSB  = (uint8_t)0x20, /*!< Interrupt selector PE(7:4) */
  EXTI_HalfPort_F_LSB  = (uint8_t)0x40, /*!< Interrupt selector PF(3:0) */
  EXTI_HalfPort_F_MSB  = (uint8_t)0x81, /*!< Interrupt selector PF(7:4) */
  EXTI_HalfPort_G_LSB  = (uint8_t)0x82, /*!< Interrupt selector PG(3:0) */
  EXTI_HalfPort_G_MSB  = (uint8_t)0x84, /*!< Interrupt selector PG(7:4) */
  EXTI_HalfPort_H_LSB  = (uint8_t)0x88, /*!< Interrupt selector PH(3:0) */
  EXTI_HalfPort_H_MSB  = (uint8_t)0x90  /*!< Interrupt selector PH(7:4) */
} EXTI_HalfPort_TypeDef;

/**
  * @}
  */

/** @defgroup EXTI_Port
  *
  * @brief EXTI Port possible values
  * Values are coded in 0xXY format where
  * X: the register index
  *   X = 0: EXTI_CR3
  *   X = 1: EXTI_CR4
  * Y: the number of shift to be performed
  * @{
  */
typedef enum
{
  EXTI_Port_B = (uint8_t)0x00, /*!< GPIO Port B */
  EXTI_Port_D = (uint8_t)0x02, /*!< GPIO Port D */
  EXTI_Port_E = (uint8_t)0x04, /*!< GPIO Port E */
  EXTI_Port_F = (uint8_t)0x06, /*!< GPIO Port F */
  EXTI_Port_G = (uint8_t)0x10, /*!< GPIO Port G */
  EXTI_Port_H = (uint8_t)0x12  /*!< GPIO Port H */
} EXTI_Port_TypeDef;

/**
  * @}
  */

/** @defgroup EXTI_Pin
  *
  * @brief EXTI PinNum possible values
  * Values are coded in 0xXY format where
  * X: the register index
  *   X = 0: EXTI_CR1
  *   X = 1: EXTI_CR2
  * Y: the number of shift to be performed
  * @{
  */
typedef enum
{
  EXTI_Pin_0 = (uint8_t)0x00, /*!< GPIO Pin 0 */
  EXTI_Pin_1 = (uint8_t)0x02, /*!< GPIO Pin 1 */
  EXTI_Pin_2 = (uint8_t)0x04, /*!< GPIO Pin 2 */
  EXTI_Pin_3 = (uint8_t)0x06, /*!< GPIO Pin 3 */
  EXTI_Pin_4 = (uint8_t)0x10, /*!< GPIO Pin 4 */
  EXTI_Pin_5 = (uint8_t)0x12, /*!< GPIO Pin 5 */
  EXTI_Pin_6 = (uint8_t)0x14, /*!< GPIO Pin 6 */
  EXTI_Pin_7 = (uint8_t)0x16  /*!< GPIO Pin 7 */
} EXTI_Pin_TypeDef;

/**
  * @}
  */

/** @defgroup EXTI_Interrupts
  *
  * @brief EXTI IT pending bit possible values
  * Values are coded in 0xXY format where
  * X: the register index
  *    X = 00:  EXTI_SR1
  *    X = 01:  EXTI_SR2
  * Y: the IT pending bit mask
  * @{
  */
typedef enum
{
  EXTI_IT_Pin0    = (uint16_t)0x0001, /*!< GPIO Pin pos 0 */
  EXTI_IT_Pin1    = (uint16_t)0x0002, /*!< GPIO Pin pos 1 */
  EXTI_IT_Pin2    = (uint16_t)0x0004, /*!< GPIO Pin pos 2 */
  EXTI_IT_Pin3    = (uint16_t)0x0008, /*!< GPIO Pin pos 3 */
  EXTI_IT_Pin4    = (uint16_t)0x0010, /*!< GPIO Pin pos 4 */
  EXTI_IT_Pin5    = (uint16_t)0x0020, /*!< GPIO Pin pos 5 */
  EXTI_IT_Pin6    = (uint16_t)0x0040, /*!< GPIO Pin pos 6 */
  EXTI_IT_Pin7    = (uint16_t)0x0080, /*!< GPIO Pin pos 7 */
  EXTI_IT_PortB   = (uint16_t)0x0101, /*!< GPIO Port B    */
  EXTI_IT_PortD   = (uint16_t)0x0102, /*!< GPIO Port D    */
  EXTI_IT_PortE   = (uint16_t)0x0104, /*!< GPIO Port E    */
  EXTI_IT_PortF   = (uint16_t)0x0108, /*!< GPIO Port F    */
  EXTI_IT_PortG   = (uint16_t)0x0110, /*!< GPIO Port G    */
  EXTI_IT_PortH   = (uint16_t)0x0120  /*!< GPIO Port H    */
} EXTI_IT_TypeDef;

/**
  * @}
  */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/** @addtogroup EXTI_Exported_Macros
  * @{
  */

/**
  * @brief Macro used by the assert function to check the different functions parameters.
  */

/**
  * @brief Macro used by the assert function in order to check the different values
  *  of EXTI Sensitivity
  */
#define IS_EXTI_TRIGGER(TRIGGER) \
  (((TRIGGER) == EXTI_Trigger_Falling_Low)  || \
   ((TRIGGER) == EXTI_Trigger_Rising) || \
   ((TRIGGER) == EXTI_Trigger_Falling) || \
   ((TRIGGER) == EXTI_Trigger_Rising_Falling))

/**
  * @brief Macro used by the assert function in order to check the different
  *   half ports values for configuration.
  */
#define IS_EXTI_HALFPORT(HALFPORT) \
  (((HALFPORT) == EXTI_HalfPort_B_LSB) ||\
   ((HALFPORT) == EXTI_HalfPort_B_MSB) ||\
   ((HALFPORT) == EXTI_HalfPort_D_LSB) ||\
   ((HALFPORT) == EXTI_HalfPort_D_MSB) ||\
   ((HALFPORT) == EXTI_HalfPort_E_LSB) ||\
   ((HALFPORT) == EXTI_HalfPort_E_MSB) ||\
   ((HALFPORT) == EXTI_HalfPort_F_LSB) ||\
   ((HALFPORT) == EXTI_HalfPort_F_MSB) ||\
   ((HALFPORT) == EXTI_HalfPort_G_LSB) ||\
   ((HALFPORT) == EXTI_HalfPort_G_MSB) ||\
   ((HALFPORT) == EXTI_HalfPort_H_LSB) ||\
   ((HALFPORT) == EXTI_HalfPort_H_MSB))

/**
  * @brief Macro used by the assert function in order to check the different Port Number values
  */
#define IS_EXTI_PORT(PORT) (((PORT) == EXTI_Port_B) ||\
                            ((PORT) == EXTI_Port_D) ||\
                            ((PORT) == EXTI_Port_E) ||\
                            ((PORT) == EXTI_Port_F) ||\
                            ((PORT) == EXTI_Port_G) ||\
                            ((PORT) == EXTI_Port_H))

/**
  * @brief Macro used by the assert function in order to check the different Pin numbers values
  */
#define IS_EXTI_PINNUM(PINNUM) \
  (((PINNUM) == EXTI_Pin_0) ||\
   ((PINNUM) == EXTI_Pin_1) ||\
   ((PINNUM) == EXTI_Pin_2) ||\
   ((PINNUM) == EXTI_Pin_3) ||\
   ((PINNUM) == EXTI_Pin_4) ||\
   ((PINNUM) == EXTI_Pin_5) ||\
   ((PINNUM) == EXTI_Pin_6) ||\
   ((PINNUM) == EXTI_Pin_7))

/**
  * @brief Macro used by the assert function in order to check the different flags values
  */
#define IS_EXTI_ITPENDINGBIT(ITPENDINGBIT) \
  (((ITPENDINGBIT) == EXTI_IT_Pin0) ||\
   ((ITPENDINGBIT) == EXTI_IT_Pin1) ||\
   ((ITPENDINGBIT) == EXTI_IT_Pin2) ||\
   ((ITPENDINGBIT) == EXTI_IT_Pin3) ||\
   ((ITPENDINGBIT) == EXTI_IT_Pin4) ||\
   ((ITPENDINGBIT) == EXTI_IT_Pin5) ||\
   ((ITPENDINGBIT) == EXTI_IT_Pin6) ||\
   ((ITPENDINGBIT) == EXTI_IT_Pin7) ||\
   ((ITPENDINGBIT) == EXTI_IT_PortB) ||\
   ((ITPENDINGBIT) == EXTI_IT_PortD) ||\
   ((ITPENDINGBIT) == EXTI_IT_PortE) ||\
   ((ITPENDINGBIT) == EXTI_IT_PortF) ||\
   ((ITPENDINGBIT) == EXTI_IT_PortG) ||\
   ((ITPENDINGBIT) == EXTI_IT_PortH))
/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */
/* EXTI configuration *********************************************************/
void EXTI_DeInit(void);
void EXTI_SetPinSensitivity(EXTI_Pin_TypeDef EXTI_Pin, EXTI_Trigger_TypeDef EXTI_Trigger);
void EXTI_SelectPort(EXTI_Port_TypeDef EXTI_Port);
void EXTI_SetHalfPortSelection(EXTI_HalfPort_TypeDef EXTI_HalfPort, FunctionalState NewState);
void EXTI_SetPortSensitivity(EXTI_Port_TypeDef EXTI_Port, EXTI_Trigger_TypeDef EXTI_Trigger);
EXTI_Trigger_TypeDef EXTI_GetPinSensitivity(EXTI_Pin_TypeDef EXTI_Pin);
EXTI_Trigger_TypeDef EXTI_GetPortSensitivity(EXTI_Port_TypeDef EXTI_Port);

/* EXTI Interrupt status management *******************************************/
ITStatus EXTI_GetITStatus(EXTI_IT_TypeDef EXTI_IT);
void EXTI_ClearITPendingBit(EXTI_IT_TypeDef EXTI_IT);

#endif /* __STM8L15x_EXTI_H */

/**
  * @}
  */
  
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
