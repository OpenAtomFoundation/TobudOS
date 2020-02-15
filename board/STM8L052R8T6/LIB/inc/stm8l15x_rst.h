/**
  ******************************************************************************
  * @file    stm8l15x_rst.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the RST firmware
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
#ifndef __STM8L15x_RST_H
#define __STM8L15x_RST_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
  
/** @addtogroup RST
  * @{
  */ 
  
/* Exported variables ------------------------------------------------------- */
/* Exported types ------------------------------------------------------------*/
/** @defgroup RST_Exported_Types
  * @{
  */

/** @defgroup RST_Flags 
  * @{
  */
typedef enum {
  RST_FLAG_PORF   = (uint8_t)0x01, /*!< POR reset flag */
  RST_FLAG_SWIMF  = (uint8_t)0x08, /*!< SWIM reset flag */
  RST_FLAG_ILLOPF = (uint8_t)0x04, /*!< Illegal opcode reset flag */
  RST_FLAG_IWDGF  = (uint8_t)0x02, /*!< Independent watchdog reset flag */
  RST_FLAG_WWDGF  = (uint8_t)0x10, /*!< Window watchdog reset flag */
  RST_FLAG_BORF   = (uint8_t)0x20  /*!< BOR reset flag */
} RST_FLAG_TypeDef;

#define IS_RST_FLAG(FLAG)  (((FLAG) == RST_FLAG_PORF) || ((FLAG) == RST_FLAG_BORF) || \
                            ((FLAG) == RST_FLAG_IWDGF) || ((FLAG) == RST_FLAG_ILLOPF) || \
                            ((FLAG) == RST_FLAG_WWDGF) || ((FLAG) == RST_FLAG_SWIMF))
/**
  * @}
  */
  
/**
  * @}
  */
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* Flag management functions **************************************************/
FlagStatus RST_GetFlagStatus(RST_FLAG_TypeDef RST_Flag);
void RST_ClearFlag(RST_FLAG_TypeDef RST_Flag);

/* NRST Pin configuration function ********************************************/
void RST_GPOutputEnable(void);

#endif /* __STM8L15x_RST_H */
/**
  * @}
  */
  
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
