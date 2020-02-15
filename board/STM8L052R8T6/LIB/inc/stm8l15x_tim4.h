/**
  ******************************************************************************
  * @file    stm8l15x_tim4.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the TIM4 firmware
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
#ifndef __STM8L15x_TIM4_H
#define __STM8L15x_TIM4_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @addtogroup TIM4
  * @{
  */ 
  
/* Exported types ------------------------------------------------------------*/

/** @defgroup TIM4_Exported_Types
  * @{
  */


/** @defgroup TIM4_Prescaler
  * @{
  */
typedef enum
{
  TIM4_Prescaler_1      = ((uint8_t)0x00),   /*!< Time base Prescaler = 1 (No effect)*/
  TIM4_Prescaler_2      = ((uint8_t)0x01),  /*!< Time base Prescaler = 2 */
  TIM4_Prescaler_4      = ((uint8_t)0x02),  /*!< Time base Prescaler = 4 */
  TIM4_Prescaler_8      = ((uint8_t)0x03),  /*!< Time base Prescaler = 8 */
  TIM4_Prescaler_16     = ((uint8_t)0x04),  /*!< Time base Prescaler = 16 */
  TIM4_Prescaler_32     = ((uint8_t)0x05),  /*!< Time base Prescaler = 32 */
  TIM4_Prescaler_64     = ((uint8_t)0x06),  /*!< Time base Prescaler = 64 */
  TIM4_Prescaler_128    = ((uint8_t)0x07),  /*!< Time base Prescaler = 128 */
  TIM4_Prescaler_256    = ((uint8_t)0x08),  /*!< Time base Prescaler = 256 */
  TIM4_Prescaler_512    = ((uint8_t)0x09),  /*!< Time base Prescaler = 512 */
  TIM4_Prescaler_1024   = ((uint8_t)0x0A),  /*!< Time base Prescaler = 1024 */
  TIM4_Prescaler_2048   = ((uint8_t)0x0B),  /*!< Time base Prescaler = 2048 */
  TIM4_Prescaler_4096   = ((uint8_t)0x0C),  /*!< Time base Prescaler = 4096 */
  TIM4_Prescaler_8192   = ((uint8_t)0x0D),  /*!< Time base Prescaler = 8196 */
  TIM4_Prescaler_16384  = ((uint8_t)0x0E),  /*!< Time base Prescaler = 16384 */
  TIM4_Prescaler_32768  = ((uint8_t)0x0F)   /*!< Time base Prescaler = 32768 */
}TIM4_Prescaler_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM4_One_Pulse_Mode
  * @{
  */
typedef enum
{
  TIM4_OPMode_Single      = ((uint8_t) 0x01),  /*!< Single one Pulse mode (OPM Active) */
  TIM4_OPMode_Repetitive  = ((uint8_t) 0x00)   /*!< Repetitive Pulse mode (OPM inactive) */
}TIM4_OPMode_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM4_Reload_Mode_Prescaler
  * @{
  */
typedef enum
{
  TIM4_PSCReloadMode_Update      = ((uint8_t)0x00), /*!< Prescaler value is reloaded at every update */
  TIM4_PSCReloadMode_Immediate  = ((uint8_t)0x01)  /*!< Prescaler value is reloaded immediatly */
}TIM4_PSCReloadMode_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM4_Update_Source
  * @{
  */
typedef enum
{
  TIM4_UpdateSource_Global  = ((uint8_t)0x00),  /*!< Global Update request source */
  TIM4_UpdateSource_Regular  = ((uint8_t)0x01)   /*!< Regular Update request source */
}TIM4_UpdateSource_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM4_Event_Source
  * @{
  */
typedef enum
{
  TIM4_EventSource_Update    = ((uint8_t)0x01),  /*!< Update Event */
  TIM4_EventSource_Trigger  = ((uint8_t)0x40)   /*!< Trigger Event */
}TIM4_EventSource_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM4_Trigger_Output_Source
  * @{
  */
typedef enum
{
  TIM4_TRGOSource_Reset   = ((uint8_t)0x00),  /*!< Trigger Output source = Reset */
  TIM4_TRGOSource_Enable  = ((uint8_t)0x10),  /*!< Trigger Output source = TIM4 is enabled */
  TIM4_TRGOSource_Update  = ((uint8_t)0x20)   /*!< Trigger Output source = Update event */
}TIM4_TRGOSource_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM4_Salve_Mode
  * @{
  */
typedef enum
{
  TIM4_SlaveMode_Disable    = ((uint8_t)0x00),   /*!< Disable slave mode to clock the prescaler
                                                      directly with the internal clock */
  TIM4_SlaveMode_Reset      = ((uint8_t)0x04),   /*!< Slave Mode Selection  = Reset*/
  TIM4_SlaveMode_Gated      = ((uint8_t)0x05),   /*!< Slave Mode Selection  = Gated*/
  TIM4_SlaveMode_Trigger    = ((uint8_t)0x06),  /*!< Slave Mode Selection  = Trigger*/
  TIM4_SlaveMode_External1  = ((uint8_t)0x07)    /*!< Slave Mode Selection  = External 1*/
}TIM4_SlaveMode_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM4_Flags
  * @{
  */
typedef enum
{
  TIM4_FLAG_Update  = ((uint8_t)0x01),  /*!< Update Flag */
  TIM4_FLAG_Trigger  = ((uint8_t)0x40)    /*!< Trigger Flag */
}TIM4_FLAG_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM4_Interrupts
  * @{
  */
typedef enum
{
  TIM4_IT_Update  = ((uint8_t)0x01),  /*!< Update Interrupt*/
  TIM4_IT_Trigger  = ((uint8_t)0x40)   /*!< Trigger  Interrupt*/
}TIM4_IT_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM4_Internal_Trigger_Selection
  * @{
  */
typedef enum
{
  TIM4_TRGSelection_TIM5  = ((uint8_t)0x00), /*!< TRIG Input source =  TIM5 TRIG Output */
  TIM4_TRGSelection_TIM1  = ((uint8_t)0x10), /*!< TRIG Input source =  TIM1 TRIG Output */
  TIM4_TRGSelection_TIM3  = ((uint8_t)0x20), /*!< TRIG Input source =  TIM3 TRIG Output */
  TIM4_TRGSelection_TIM2  = ((uint8_t)0x30)  /*!< TRIG Input source =  TIM2 TRIG Output */
}TIM4_TRGSelection_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM4_DMA_source_requests
  * @{
  */
typedef enum
{
  TIM4_DMASource_Update    = ((uint8_t)0x01)  /*!< TIM4 DMA Update Request*/
}TIM4_DMASource_TypeDef;
/**
  * @}
  */

/**
  * @}
  */
  
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/** @defgroup TIM4_Exported_Macros
  * @{
  */

/**
  * @brief Macro used by the assert function to check the different functions parameters.
  */

/**
  * @brief Macro TIM4 Prescaler
  */
#define IS_TIM4_Prescaler(PRESCALER)  \
  (((PRESCALER) == TIM4_Prescaler_1)    || \
   ((PRESCALER) == TIM4_Prescaler_2)    || \
   ((PRESCALER) == TIM4_Prescaler_4)    || \
   ((PRESCALER) == TIM4_Prescaler_8)    || \
   ((PRESCALER) == TIM4_Prescaler_16)   || \
   ((PRESCALER) == TIM4_Prescaler_32)   || \
   ((PRESCALER) == TIM4_Prescaler_64)   || \
   ((PRESCALER) == TIM4_Prescaler_128)  || \
   ((PRESCALER) == TIM4_Prescaler_256)  || \
   ((PRESCALER) == TIM4_Prescaler_512)  || \
   ((PRESCALER) == TIM4_Prescaler_1024) || \
   ((PRESCALER) == TIM4_Prescaler_2048) || \
   ((PRESCALER) == TIM4_Prescaler_4096) || \
   ((PRESCALER) == TIM4_Prescaler_8192) || \
   ((PRESCALER) == TIM4_Prescaler_16384)|| \
   ((PRESCALER) == TIM4_Prescaler_32768))
/**
  * @brief Macro TIM4 One Pulse Mode
  */
#define IS_TIM4_OPM_MODE(MODE)  \
  (((MODE) == TIM4_OPMode_Single) || \
   ((MODE) == TIM4_OPMode_Repetitive))

/**
  * @brief Macro TIM4 Prescaler reload
  */
#define IS_TIM4_Prescaler_RELOAD(RELOAD) \
  (((RELOAD) == TIM4_PSCReloadMode_Update) || \
   ((RELOAD) == TIM4_PSCReloadMode_Immediate))
/**
  * @brief Macro TIM4 Update source
  */
#define IS_TIM4_UPDATE_SOURCE(SOURCE) \
  (((SOURCE) == TIM4_UpdateSource_Global) || \
   ((SOURCE) == TIM4_UpdateSource_Regular))
/**
  * @brief Macro TIM4 Event source
  */
#define IS_TIM4_EVENT_SOURCE(SOURCE) \
  ((((SOURCE) & (uint8_t)0xBE) == 0x00) && \
   ((SOURCE) != 0x00))

/**
  * @brief Macro TIM4 TRGO source
  */
#define IS_TIM4_TRGO_SOURCE(SOURCE) \
  (((SOURCE) == TIM4_TRGOSource_Reset) || \
   ((SOURCE) == TIM4_TRGOSource_Enable)|| \
   ((SOURCE) == TIM4_TRGOSource_Update))
/**
  * @brief Macro TIM4 Slave mode
  */
#define IS_TIM4_SLAVE_MODE(MODE) \
  (((MODE) == TIM4_SlaveMode_Disable) || \
   ((MODE) == TIM4_SlaveMode_Reset)   || \
   ((MODE) == TIM4_SlaveMode_Gated)   || \
   ((MODE) == TIM4_SlaveMode_Trigger) || \
   ((MODE) == TIM4_SlaveMode_External1))
/**
  * @brief Macro TIM4 Flags
  */
#define IS_TIM4_GET_FLAG(FLAG) \
  (((FLAG) == TIM4_FLAG_Update) || \
   ((FLAG) == TIM4_FLAG_Trigger))

#define IS_TIM4_CLEAR_FLAG(FLAG) \
  ((((FLAG) & (uint8_t)0xBE) == 0x00) && ((FLAG) != 0x00))
/**
  * @brief Macro TIM4 interrupts
  */
#define IS_TIM4_IT(IT) \
  (((IT) != 0x00) && \
   (((uint8_t)(IT) & (uint8_t)(~(uint8_t)(0x41)))== 0x00))

#define IS_TIM4_GET_IT(IT) \
  (((IT) == TIM4_IT_Update) || \
   ((IT) == TIM4_IT_Trigger))
/**
  * @brief Macro TIM4 Trigger selection
  */
#define IS_TIM4_TRIGGER_SELECTION(SELECTION) \
  (((SELECTION) == TIM4_TRGSelection_TIM5) || \
   ((SELECTION) == TIM4_TRGSelection_TIM1) || \
   ((SELECTION) == TIM4_TRGSelection_TIM3) || \
   ((SELECTION) == TIM4_TRGSelection_TIM2))

/**
  * @brief Macro TIM4 DMA sources
  */

#define IS_TIM4_DMA_SOURCE(SOURCE) (((SOURCE) == TIM4_DMASource_Update))
/**
 * @}
 */

/* Exported functions --------------------------------------------------------*/

/* TimeBase management ********************************************************/
void TIM4_DeInit(void);
void TIM4_TimeBaseInit(TIM4_Prescaler_TypeDef TIM4_Prescaler,
                       uint8_t TIM4_Period);
void TIM4_PrescalerConfig(TIM4_Prescaler_TypeDef Prescaler,
                          TIM4_PSCReloadMode_TypeDef TIM4_PSCReloadMode);
void TIM4_SetCounter(uint8_t Counter);
void TIM4_SetAutoreload(uint8_t Autoreload);
uint8_t TIM4_GetCounter(void);
TIM4_Prescaler_TypeDef TIM4_GetPrescaler(void);
void TIM4_UpdateDisableConfig(FunctionalState NewState);
void TIM4_UpdateRequestConfig(TIM4_UpdateSource_TypeDef TIM4_UpdateSource);
void TIM4_ARRPreloadConfig(FunctionalState NewState);
void TIM4_SelectOnePulseMode(TIM4_OPMode_TypeDef TIM4_OPMode);
void TIM4_Cmd(FunctionalState NewState);

/* Interrupts, DMA and flags management ***************************************/
void TIM4_ITConfig(TIM4_IT_TypeDef TIM4_IT, FunctionalState NewState);
void TIM4_GenerateEvent(TIM4_EventSource_TypeDef TIM4_EventSource);
FlagStatus TIM4_GetFlagStatus(TIM4_FLAG_TypeDef TIM4_FLAG);
void TIM4_ClearFlag(TIM4_FLAG_TypeDef TIM4_FLAG);
ITStatus TIM4_GetITStatus(TIM4_IT_TypeDef TIM4_IT);
void TIM4_ClearITPendingBit(TIM4_IT_TypeDef TIM4_IT);
void TIM4_DMACmd(TIM4_DMASource_TypeDef TIM4_DMASource, FunctionalState NewState);

/* Clocks management **********************************************************/
void TIM4_InternalClockConfig(void);

/* Synchronization management *************************************************/
void TIM4_SelectInputTrigger(TIM4_TRGSelection_TypeDef TIM4_InputTriggerSource);
void TIM4_SelectOutputTrigger(TIM4_TRGOSource_TypeDef TIM4_TRGOSource);
void TIM4_SelectSlaveMode(TIM4_SlaveMode_TypeDef TIM4_SlaveMode);
void TIM4_SelectMasterSlaveMode(FunctionalState NewState);

#endif /* __STM8L15x_TIM4_H */

/**
 * @}
 */


/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
