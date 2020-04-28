/**
  ******************************************************************************
  * @file    stm8l15x_pwr.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the PWR firmware
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
#ifndef __STM8L15x_PWR_H
#define __STM8L15x_PWR_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @addtogroup PWR
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/

/** @defgroup PWR_Exported_Types
  * @{
  */

/** @defgroup PVD_detection_level 
  * @{
  */

typedef enum {
  PWR_PVDLevel_1V85  = (uint8_t)0x00, /*!< PVD threshold = 1.85 V */
  PWR_PVDLevel_2V05  = (uint8_t)0x02, /*!< PVD threshold = 2.05 V */
  PWR_PVDLevel_2V26  = (uint8_t)0x04, /*!< PVD threshold = 2.26 V */
  PWR_PVDLevel_2V45  = (uint8_t)0x06, /*!< PVD threshold = 2.45 V */
  PWR_PVDLevel_2V65  = (uint8_t)0x08, /*!< PVD threshold = 2.65 V */
  PWR_PVDLevel_2V85  = (uint8_t)0x0A, /*!< PVD threshold = 2.85 V */
  PWR_PVDLevel_3V05  = (uint8_t)0x0C, /*!< PVD threshold = 3.05 V */
  PWR_PVDLevel_PVDIn = (uint8_t)0x0E  /*!< PVD threshold = PVD_IN input pin */
} PWR_PVDLevel_TypeDef;

#define IS_PWR_PVD_LEVEL(LEVEL) (((LEVEL) == PWR_PVDLevel_1V85) || \
                                 ((LEVEL) == PWR_PVDLevel_2V05) || \
                                 ((LEVEL) == PWR_PVDLevel_2V26) || \
                                 ((LEVEL) == PWR_PVDLevel_2V45) || \
                                 ((LEVEL) == PWR_PVDLevel_2V65) || \
                                 ((LEVEL) == PWR_PVDLevel_2V85) || \
                                 ((LEVEL) == PWR_PVDLevel_3V05) || \
                                 ((LEVEL) == PWR_PVDLevel_PVDIn))
/**
  * @}
  */
  
/** @defgroup PWR_Flag 
  * @{
  */
  
typedef enum {
  PWR_FLAG_PVDOF    = (uint8_t)0x40,/*!< PVD output flag */
  PWR_FLAG_PVDIF    = (uint8_t)0x20, /*!< PVD Interrupt flag */
  PWR_FLAG_VREFINTF = (uint8_t)0x01  /*!< Internal reference voltage status flag */
} PWR_FLAG_TypeDef;

#define IS_PWR_FLAG(FLAG)   (((FLAG) == PWR_FLAG_PVDOF) || \
                             ((FLAG) == PWR_FLAG_PVDIF) || \
                             ((FLAG) == PWR_FLAG_VREFINTF))

/**
  * @}
  */
  
/**
  * @}
  */
  
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/* Function used to set the PWR configuration to the default reset state ******/ 
void PWR_DeInit(void);

/* PVD configuration functions ************************************************/ 
void PWR_PVDLevelConfig(PWR_PVDLevel_TypeDef PWR_PVDLevel);
void PWR_PVDCmd(FunctionalState NewState);

/* Ultra Low Power mode configuration functions *******************************/ 
void PWR_FastWakeUpCmd(FunctionalState NewState);
void PWR_UltraLowPowerCmd(FunctionalState NewState);

/* Interrupts and flags management functions **********************************/
void PWR_PVDITConfig(FunctionalState NewState);
ITStatus PWR_PVDGetITStatus(void);
FlagStatus PWR_GetFlagStatus(PWR_FLAG_TypeDef PWR_FLAG);
void PWR_PVDClearFlag(void);
void PWR_PVDClearITPendingBit(void);

#endif /* __STM8L15x_PWR_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
