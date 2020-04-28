/**
  ******************************************************************************
  * @file    stm8l15x_wwdg.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the WWDG firmware
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
#ifndef __STM8L15x_WWDG_H
#define __STM8L15x_WWDG_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
  
/** @addtogroup WWDG
  * @{
  */ 
  
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/** @defgroup  WWDG_Exported_Macros
  * @{
  */

/** @defgroup WWDG_WindowLimitValue 
  * @{
  */ 
#define IS_WWDG_WINDOW_LIMIT_VALUE(WindowLimitValue) ((WindowLimitValue) <= 0x7F)

/**
  * @}
  */

/** @defgroup WWDG_CounterValue 
  * @{
  */
#define IS_WWDG_COUNTER_VALUE(CounterValue) ((CounterValue) <= 0x7F)
/**
  * @}
  */
  
/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */
/* Refresh window and Counter configuration functions *************************/
void WWDG_Init(uint8_t Counter, uint8_t WindowValue);
void WWDG_SetWindowValue(uint8_t WindowValue);
void WWDG_SetCounter(uint8_t Counter);

/* WWDG activation function ***************************************************/
void WWDG_Enable(uint8_t Counter);

/* WWDG counter and software reset management **********************************/
uint8_t WWDG_GetCounter(void);
void WWDG_SWReset(void);


#endif /* __STM8L15x_WWDG_H */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/