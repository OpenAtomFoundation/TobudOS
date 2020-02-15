/**
  ******************************************************************************
  * @file    stm8l15x_tim3.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the TIM3 firmware
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
#ifndef __STM8L15x_TIM3_H
#define __STM8L15x_TIM3_H


/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @defgroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @defgroup TIM3
  * @{
  */ 
  
/* Exported types ------------------------------------------------------------*/

/** @defgroup TIM3_Exported_Types
  * @{
  */

/** @defgroup TIM3_Forced_Action
  * @{
  */
typedef enum
{
  TIM3_ForcedAction_Active    = ((uint8_t)0x50), /*!< Output Reference is forced low */
  TIM3_ForcedAction_Inactive  = ((uint8_t)0x40)  /*!< Output Reference is forced high */
}
TIM3_ForcedAction_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Prescaler
  * @{
  */
typedef enum
{
  TIM3_Prescaler_1     = ((uint8_t)0x00), /*!< Time base Prescaler = 1 (No effect)*/
  TIM3_Prescaler_2     = ((uint8_t)0x01), /*!< Time base Prescaler = 2 */
  TIM3_Prescaler_4     = ((uint8_t)0x02), /*!< Time base Prescaler = 4 */
  TIM3_Prescaler_8     = ((uint8_t)0x03), /*!< Time base Prescaler = 8 */
  TIM3_Prescaler_16    = ((uint8_t)0x04), /*!< Time base Prescaler = 16 */
  TIM3_Prescaler_32    = ((uint8_t)0x05), /*!< Time base Prescaler = 32 */
  TIM3_Prescaler_64    = ((uint8_t)0x06), /*!< Time base Prescaler = 64 */
  TIM3_Prescaler_128   = ((uint8_t)0x07)  /*!< Time base Prescaler = 128 */
}TIM3_Prescaler_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_OCMode
  * @{
  */
typedef enum
{
  TIM3_OCMode_Timing    = ((uint8_t)0x00), /*!< Timing (Frozen) Mode*/
  TIM3_OCMode_Active    = ((uint8_t)0x10), /*!< Active Mode*/
  TIM3_OCMode_Inactive  = ((uint8_t)0x20), /*!< Inactive Mode*/
  TIM3_OCMode_Toggle    = ((uint8_t)0x30), /*!< Toggle Mode*/
  TIM3_OCMode_PWM1      = ((uint8_t)0x60), /*!< PWM Mode 1*/
  TIM3_OCMode_PWM2      = ((uint8_t)0x70)  /*!< PWM Mode 2*/
}TIM3_OCMode_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_OnePulseMode
  * @{
  */
typedef enum
{
  TIM3_OPMode_Single      = ((uint8_t)0x01), /*!< Single one Pulse mode (OPM Active) */
  TIM3_OPMode_Repetitive  = ((uint8_t)0x00)  /*!< Repetitive Pulse mode (OPM inactive) */
}TIM3_OPMode_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Channel
  * @{
  */
typedef enum
{
  TIM3_Channel_1     = ((uint8_t)0x00),  /*!< Channel 1*/
  TIM3_Channel_2     = ((uint8_t)0x01)   /*!< Channel 2*/
}TIM3_Channel_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_CounterMode
  * @{
  */
typedef enum
{
  TIM3_CounterMode_Up              = ((uint8_t)0x00), /*!< Counter Up Mode */
  TIM3_CounterMode_Down            = ((uint8_t)0x10), /*!< Counter Down Mode */
  TIM3_CounterMode_CenterAligned1  = ((uint8_t)0x20), /*!< Counter Central aligned Mode 1 */
  TIM3_CounterMode_CenterAligned2  = ((uint8_t)0x40), /*!< Counter Central aligned Mode 2 */
  TIM3_CounterMode_CenterAligned3  = ((uint8_t)0x60)  /*!< Counter Central aligned Mode 3 */
}TIM3_CounterMode_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Output_Compare_Polarity
  * @{
  */
typedef enum
{
  TIM3_OCPolarity_High   = ((uint8_t)0x00), /*!< Output compare polarity  = High */
  TIM3_OCPolarity_Low    = ((uint8_t)0x01)  /*!< Output compare polarity  = Low */
}TIM3_OCPolarity_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Output_State
  * @{
  */
typedef enum
{
  TIM3_OutputState_Disable  = ((uint8_t)0x00), /*!< Output compare State disabled
                                                          (channel output disabled) */
  TIM3_OutputState_Enable   = ((uint8_t)0x01)  /*!< Output compare State enabled
                                                   (channel output enabled) */
}TIM3_OutputState_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Break_State
  * @{
  */
typedef enum
{
  TIM3_BreakState_Disable   = ((uint8_t)0x00), /*!< Break State disabled (break option disabled) */
  TIM3_BreakState_Enable    = ((uint8_t)0x10)  /*!< Break State enabled (break option enabled) */
}TIM3_BreakState_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Break_Polarity
  * @{
  */
typedef enum
{
  TIM3_BreakPolarity_High   = ((uint8_t)0x20),  /*!< if Break, channel polarity = High */
  TIM3_BreakPolarity_Low    = ((uint8_t)0x00)   /*!< if Break, channel polarity = Low */
}TIM3_BreakPolarity_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Automatic_Output
  * @{
  */
typedef enum
{
  TIM3_AutomaticOutput_Enable   = ((uint8_t)0x40), /*!< Automatic Output option enabled */
  TIM3_AutomaticOutput_Disable  = ((uint8_t)0x00)  /*!< Automatic Output option disabled */
}TIM3_AutomaticOutput_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Lock_Level
  * @{
  */
typedef enum
{
  TIM3_LockLevel_Off        = ((uint8_t)0x00), /*!< Lock option disabled */
  TIM3_LockLevel_1          = ((uint8_t)0x01), /*!< Select Lock Level 1  */
  TIM3_LockLevel_2          = ((uint8_t)0x02), /*!< Select Lock Level 2  */
  TIM3_LockLevel_3          = ((uint8_t)0x03)  /*!< Select Lock Level 3  */
}TIM3_LockLevel_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_OSSI_State
  * @{
  */
typedef enum
{
  TIM3_OSSIState_Enable   = ((uint8_t)0x04), /*!< Off-State Selection for Idle mode enabled  */
  TIM3_OSSIState_Disable  = ((uint8_t)0x00)  /*!< Off-State Selection for Idle mode disabled  */
}TIM3_OSSIState_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Output_Compare_Idle_state
  * @{
  */
typedef enum
{
  TIM3_OCIdleState_Reset  = ((uint8_t)0x00), /*!< Output Compare Idle state  = Reset */
  TIM3_OCIdleState_Set    = ((uint8_t)0x01)  /*!< Output Compare Idle state  = Set */
}TIM3_OCIdleState_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Input_Capture_Polarity
  * @{
  */
typedef enum
{
  TIM3_ICPolarity_Rising  = ((uint8_t)0x00), /*!< Input Capture on Rising Edge*/
  TIM3_ICPolarity_Falling  = ((uint8_t)0x01)  /*!< Input Capture on Falling Edge*/
}TIM3_ICPolarity_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Input_Capture_Selection
  * @{
  */
typedef enum
{
  TIM3_ICSelection_DirectTI    = ((uint8_t)0x01), /*!< Input Capture mapped on the direct input*/
  TIM3_ICSelection_IndirectTI  = ((uint8_t)0x02), /*!< Input Capture mapped on the indirect input*/
  TIM3_ICSelection_TRGI        = ((uint8_t)0x03)  /*!< Input Capture mapped on the Trigger Input*/
}TIM3_ICSelection_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Input_Capture_Prescaler
  * @{
  */
typedef enum
{
  TIM3_ICPSC_DIV1  = ((uint8_t)0x00),  /*!< Input Capture Prescaler = 1 (one capture every 1 event) */
  TIM3_ICPSC_DIV2  = ((uint8_t)0x04),  /*!< Input Capture Prescaler = 2 (one capture every 2 events) */
  TIM3_ICPSC_DIV4  = ((uint8_t)0x08),  /*!< Input Capture Prescaler = 4 (one capture every 4 events) */
  TIM3_ICPSC_DIV8  = ((uint8_t)0x0C)   /*!< Input Capture Prescaler = 8 (one capture every 8 events) */
}TIM3_ICPSC_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Interrupts
  * @{
  */
typedef enum
{
  TIM3_IT_Update   = ((uint8_t)0x01), /*!< Update Interrupt*/
  TIM3_IT_CC1      = ((uint8_t)0x02), /*!< Capture Compare Channel1 Interrupt*/
  TIM3_IT_CC2      = ((uint8_t)0x04), /*!< Capture Compare Channel2 Interrupt*/
  TIM3_IT_Trigger  = ((uint8_t)0x40), /*!< Trigger  Interrupt*/
  TIM3_IT_Break    = ((uint8_t)0x80)  /*!< Break Interrupt*/
}TIM3_IT_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_External_Trigger_Prescaler
  * @{
  */
typedef enum
{
  TIM3_ExtTRGPSC_OFF   = ((uint8_t)0x00), /*!< No External Trigger prescaler  */
  TIM3_ExtTRGPSC_DIV2  = ((uint8_t)0x10), /*!< External Trigger prescaler = 2 (ETRP frequency divided by 2) */
  TIM3_ExtTRGPSC_DIV4  = ((uint8_t)0x20), /*!< External Trigger prescaler = 4 (ETRP frequency divided by 4) */
  TIM3_ExtTRGPSC_DIV8  = ((uint8_t)0x30)  /*!< External Trigger prescaler = 8 (ETRP frequency divided by 8) */
}TIM3_ExtTRGPSC_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Internal_Trigger_Selection
  * @{
  */
typedef enum
{
  TIM3_TRGSelection_TIM4     = ((uint8_t)0x00), /*!< TRIG Input source =  TIM TRIG Output  */
  TIM3_TRGSelection_TIM1     = ((uint8_t)0x10), /*!< TRIG Input source =  TIM TRIG Output  */
  TIM3_TRGSelection_TIM5     = ((uint8_t)0x20), /*!< TRIG Input source =  TIM TRIG Output  */
  TIM3_TRGSelection_TIM2     = ((uint8_t)0x30), /*!< TRIG Input source =  TIM TRIG Output  */
  TIM3_TRGSelection_TI1F_ED  = ((uint8_t)0x40), /*!< TRIG Input source = TI1F_ED (TI1 Edge Detector)  */
  TIM3_TRGSelection_TI1FP1   = ((uint8_t)0x50), /*!< TRIG Input source = TI1FP1  (Filtered Timer Input 1) */
  TIM3_TRGSelection_TI2FP2   = ((uint8_t)0x60), /*!< TRIG Input source = TI2FP2 (Filtered Timer Input 2)  */
  TIM3_TRGSelection_ETRF     = ((uint8_t)0x70)  /*!< TRIG Input source =  ETRF (External Trigger Input ) */
}TIM3_TRGSelection_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_TI_External_Clock_Source
  * @{
  */
typedef enum
{
  TIM3_TIxExternalCLK1Source_TI1ED  = ((uint8_t)0x40),  /*!< External Clock mode 1 source = TI1ED */
  TIM3_TIxExternalCLK1Source_TI1    = ((uint8_t)0x50),  /*!< External Clock mode 1 source = TI1 */
  TIM3_TIxExternalCLK1Source_TI2    = ((uint8_t)0x60)   /*!< External Clock mode 1 source = TI2 */
}TIM3_TIxExternalCLK1Source_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_External_Trigger_Polarity
  * @{
  */
typedef enum
{
  TIM3_ExtTRGPolarity_Inverted     = ((uint8_t)0x80), /*!< External Trigger Polarity = inverted */
  TIM3_ExtTRGPolarity_NonInverted  = ((uint8_t)0x00)  /*!< External Trigger Polarity = non inverted */
}TIM3_ExtTRGPolarity_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Prescaler_Reload_Mode
  * @{
  */
typedef enum
{
  TIM3_PSCReloadMode_Update     = ((uint8_t)0x00), /*!< Prescaler value is reloaded at every update*/
  TIM3_PSCReloadMode_Immediate  = ((uint8_t)0x01)  /*!< Prescaler value is reloaded immediatly*/
}TIM3_PSCReloadMode_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Encoder_Mode
  * @{
  */
typedef enum
{
  TIM3_EncoderMode_TI1    = ((uint8_t)0x01), /*!< Encoder mode 1*/
  TIM3_EncoderMode_TI2    = ((uint8_t)0x02), /*!< Encoder mode 2*/
  TIM3_EncoderMode_TI12   = ((uint8_t)0x03)  /*!< Encoder mode 3*/
}TIM3_EncoderMode_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Event_Source
  * @{
  */
typedef enum
{
  TIM3_EventSource_Update   = ((uint8_t)0x01), /*!< Update Event*/
  TIM3_EventSource_CC1      = ((uint8_t)0x02), /*!< Capture Compare Channel1  Event*/
  TIM3_EventSource_CC2      = ((uint8_t)0x04), /*!< Capture Compare Channel2 Event*/
  TIM3_EventSource_Trigger  = ((uint8_t)0x40), /*!< Trigger Event*/
  TIM3_EventSource_Break    = ((uint8_t)0x80)  /*!< Break Event*/
}TIM3_EventSource_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Update_Source
  * @{
  */
typedef enum
{
  TIM3_UpdateSource_Global  = ((uint8_t)0x00), /*!< Global Update request source */
  TIM3_UpdateSource_Regular = ((uint8_t)0x01)  /*!< Regular Update request source */
}TIM3_UpdateSource_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Trigger_Output_Source
  * @{
  */
typedef enum
{
  TIM3_TRGOSource_Reset   = ((uint8_t)0x00), /*!< Trigger Output source = Reset*/
  TIM3_TRGOSource_Enable  = ((uint8_t)0x10), /*!< Trigger Output source = TIM3 is enabled*/
  TIM3_TRGOSource_Update  = ((uint8_t)0x20), /*!< Trigger Output source = Update event*/
  TIM3_TRGOSource_OC1     = ((uint8_t)0x30), /*!< Trigger Output source = output compare channel1  */
  TIM3_TRGOSource_OC1REF  = ((uint8_t)0x40), /*!< Trigger Output source = output compare channel 1 reference */
  TIM3_TRGOSource_OC2REF  = ((uint8_t)0x50)  /*!< Trigger Output source = output compare channel 2 reference */
}TIM3_TRGOSource_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Slave_Mode
  * @{
  */
typedef enum
{
  TIM3_SlaveMode_Reset      = ((uint8_t)0x04), /*!< Slave Mode Selection  = Reset*/
  TIM3_SlaveMode_Gated      = ((uint8_t)0x05), /*!< Slave Mode Selection  = Gated*/
  TIM3_SlaveMode_Trigger    = ((uint8_t)0x06), /*!< Slave Mode Selection  = Trigger*/
  TIM3_SlaveMode_External1  = ((uint8_t)0x07)  /*!< Slave Mode Selection  = External 1*/
}TIM3_SlaveMode_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_Flags
  * @{
  */
typedef enum
{
  TIM3_FLAG_Update   = ((uint16_t)0x0001), /*!< Update Flag */
  TIM3_FLAG_CC1      = ((uint16_t)0x0002), /*!< Capture compare 1 Flag */
  TIM3_FLAG_CC2      = ((uint16_t)0x0004), /*!< Capture compare 2 Flag */
  TIM3_FLAG_Trigger  = ((uint16_t)0x0040), /*!< Trigger Flag */
  TIM3_FLAG_Break    = ((uint16_t)0x0080), /*!< Break Flag */
  TIM3_FLAG_CC1OF    = ((uint16_t)0x0200), /*!< Capture compare 1 over capture Flag */
  TIM3_FLAG_CC2OF    = ((uint16_t)0x0400)  /*!< Capture compare 2 over capture Flag */
}TIM3_FLAG_TypeDef;

/**
  * @}
  */

/** @defgroup TIM3_DMA_Source_Requests
  * @{
  */
typedef enum
{
  TIM3_DMASource_Update   = ((uint8_t)0x01),  /*!< TIM3 DMA Update Request*/
  TIM3_DMASource_CC1      = ((uint8_t)0x02),
  TIM3_DMASource_CC2      = ((uint8_t)0x04)
}TIM3_DMASource_TypeDef;

/**
  * @}
  */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/** @defgroup TIM3_Exported_Macros
  * @{
  */

/**
  * @brief Macro used by the assert function to check the different functions parameters.
  */

/**
  * @brief Macro TIM3 Forced Action
  */
#define IS_TIM3_FORCED_ACTION(ACTION) \
  (((ACTION) == TIM3_ForcedAction_Active) || \
   ((ACTION) == TIM3_ForcedAction_Inactive))

/**
  * @brief Macro TIM3 Prescaler
  */
#define IS_TIM3_PRESCALER(PRESCALER) \
  (((PRESCALER) == TIM3_Prescaler_1)  || \
   ((PRESCALER) == TIM3_Prescaler_2)  || \
   ((PRESCALER) == TIM3_Prescaler_4)  || \
   ((PRESCALER) == TIM3_Prescaler_8)  || \
   ((PRESCALER) == TIM3_Prescaler_16) || \
   ((PRESCALER) == TIM3_Prescaler_32) || \
   ((PRESCALER) == TIM3_Prescaler_64) || \
   ((PRESCALER) == TIM3_Prescaler_128))

/**
  * @brief Macro TIM3 Output Compare and PWM modes
  */
#define IS_TIM3_OC_MODE(MODE) \
  (((MODE) ==  TIM3_OCMode_Timing)  || \
   ((MODE) == TIM3_OCMode_Active)   || \
   ((MODE) == TIM3_OCMode_Inactive) || \
   ((MODE) == TIM3_OCMode_Toggle)   || \
   ((MODE) == TIM3_OCMode_PWM1)     || \
   ((MODE) == TIM3_OCMode_PWM2))

#define IS_TIM3_OCM(MODE)   \
  (((MODE) ==  TIM3_OCMode_Timing)  || \
   ((MODE) == TIM3_OCMode_Active)   || \
   ((MODE) == TIM3_OCMode_Inactive) || \
   ((MODE) == TIM3_OCMode_Toggle)   || \
   ((MODE) == TIM3_OCMode_PWM1)     || \
   ((MODE) == TIM3_OCMode_PWM2)     || \
   ((MODE) == (uint8_t)TIM3_ForcedAction_Active) || \
   ((MODE) == (uint8_t)TIM3_ForcedAction_Inactive))
/**
  * @brief Macro TIM3 One Pulse Mode
  */
#define IS_TIM3_OPM_MODE(MODE) \
  (((MODE) == TIM3_OPMode_Single) || \
   ((MODE) == TIM3_OPMode_Repetitive))

/**
  * @brief Macro TIM3 Channel
  */
#define IS_TIM3_CHANNEL(CHANNEL) \
  (((CHANNEL) == TIM3_Channel_1) || \
   ((CHANNEL) == TIM3_Channel_2) )

/**
  * @brief Macro TIM3 Counter Mode
  */
#define IS_TIM3_COUNTER_MODE(MODE) \
  (((MODE) == TIM3_CounterMode_Up)   || \
   ((MODE) == TIM3_CounterMode_Down) || \
   ((MODE) == TIM3_CounterMode_CenterAligned1) || \
   ((MODE) == TIM3_CounterMode_CenterAligned2) || \
   ((MODE) == TIM3_CounterMode_CenterAligned3))

/**
  * @brief Macro TIM3 Output Compare Polarity
  */
#define IS_TIM3_OC_POLARITY(POLARITY) \
  (((POLARITY) == TIM3_OCPolarity_High) || \
   ((POLARITY) == TIM3_OCPolarity_Low))

/**
  * @brief Macro TIM3 Output Compare states
  */
#define IS_TIM3_OUTPUT_STATE(STATE) \
  (((STATE) == TIM3_OutputState_Disable) || \
   ((STATE) == TIM3_OutputState_Enable))


/**
  * @brief Macro Break Input enable/disable
  */
#define IS_TIM3_BREAK_STATE(STATE) \
  (((STATE) == TIM3_BreakState_Enable) || \
   ((STATE) == TIM3_BreakState_Disable))

/**
  * @brief Macro Break Polarity
  */
#define IS_TIM3_BREAK_POLARITY(POLARITY) \
  (((POLARITY) == TIM3_BreakPolarity_Low) || \
   ((POLARITY) == TIM3_BreakPolarity_High))

/**
  * @brief Macro TIM3 AOE Bit Set/Reset
  */
#define IS_TIM3_AUTOMATIC_OUTPUT_STATE(STATE) \
  (((STATE) == TIM3_AutomaticOutput_Enable) || \
   ((STATE) == TIM3_AutomaticOutput_Disable))

/**
  * @brief Macro Lock levels
  */
#define IS_TIM3_LOCK_LEVEL(LEVEL) \
  (((LEVEL) == TIM3_LockLevel_Off) || \
   ((LEVEL) == TIM3_LockLevel_1)   || \
   ((LEVEL) == TIM3_LockLevel_2)   || \
   ((LEVEL) == TIM3_LockLevel_3))

/**
  * @brief Macro OSSI: Off-State Selection for Idle mode states
  */
#define IS_TIM3_OSSI_STATE(STATE) \
  (((STATE) == TIM3_OSSIState_Enable) || \
   ((STATE) == TIM3_OSSIState_Disable))

/**
  * @brief Macro TIM3 OC IDLE STATE
  */
#define IS_TIM3_OCIDLE_STATE(STATE) \
  (((STATE) == TIM3_OCIdleState_Set) || \
   ((STATE) == TIM3_OCIdleState_Reset))

/**
  * @brief Macro TIM3 IC POLARITY
  */

#define IS_TIM3_IC_POLARITY(POLARITY) \
  (((POLARITY) == TIM3_ICPolarity_Rising) || \
   ((POLARITY) == TIM3_ICPolarity_Falling))

/**
  * @brief Macro TIM3 IC SELECTION
  */
#define IS_TIM3_IC_SELECTION(SELECTION) \
  (((SELECTION) == TIM3_ICSelection_DirectTI)   || \
   ((SELECTION) == TIM3_ICSelection_IndirectTI) || \
   ((SELECTION) == TIM3_ICSelection_TRGI))


/**
  * @brief Macro TIM3 IC PRESCALER
  */
#define IS_TIM3_IC_PRESCALER(PRESCALER) \
  (((PRESCALER) == TIM3_ICPSC_DIV1) || \
   ((PRESCALER) == TIM3_ICPSC_DIV2) || \
   ((PRESCALER) == TIM3_ICPSC_DIV4) || \
   ((PRESCALER) == TIM3_ICPSC_DIV8))

/**
  * @brief Macro TIM3 Input Capture Filter Value
  */
#define IS_TIM3_IC_FILTER(ICFILTER) \
  ((ICFILTER) <= 0x0F)

/**
  * @brief Macro TIM3 Interrupts
  */
#define IS_TIM3_IT(IT) \
  ((IT) != 0x00)

#define IS_TIM3_GET_IT(IT) \
  (((IT) == TIM3_IT_Update)  || \
   ((IT) == TIM3_IT_CC1)     || \
   ((IT) == TIM3_IT_CC2)     || \
   ((IT) == TIM3_IT_Trigger) || \
   ((IT) == TIM3_IT_Break))

/**
  * @brief Macro TIM3 external trigger prescaler
  */
#define IS_TIM3_EXT_PRESCALER(PRESCALER) \
  (((PRESCALER) == TIM3_ExtTRGPSC_OFF)  || \
   ((PRESCALER) == TIM3_ExtTRGPSC_DIV2) || \
   ((PRESCALER) == TIM3_ExtTRGPSC_DIV4) || \
   ((PRESCALER) == TIM3_ExtTRGPSC_DIV8))

/**
  * @brief Macro TIM3  Trigger Selection
  */
#define IS_TIM3_TRIGGER_SELECTION(SELECTION) \
  (((SELECTION) == TIM3_TRGSelection_TI1F_ED) || \
   ((SELECTION) == TIM3_TRGSelection_TI1FP1)  || \
   ((SELECTION) == TIM3_TRGSelection_TI2FP2)  || \
   ((SELECTION) == TIM3_TRGSelection_TIM4) || \
   ((SELECTION) == TIM3_TRGSelection_TIM1) || \
   ((SELECTION) == TIM3_TRGSelection_TIM5) || \
   ((SELECTION) == TIM3_TRGSelection_TIM2) || \
   ((SELECTION) == TIM3_TRGSelection_ETRF))


#define IS_TIM3_TIX_TRIGGER_SELECTION(SELECTION) \
  (((SELECTION) == TIM3_TRGSelection_TI1F_ED) || \
   ((SELECTION) == TIM3_TRGSelection_TI1FP1)  || \
   ((SELECTION) == TIM3_TRGSelection_TI2FP2))

/**
  * @brief Macro TIM3  TIx external Clock Selection
  */
#define IS_TIM3_TIXCLK_SOURCE(SOURCE)  \
  (((SOURCE) == TIM3_TIxExternalCLK1Source_TI1ED) || \
   ((SOURCE) == TIM3_TIxExternalCLK1Source_TI2)   || \
   ((SOURCE) == TIM3_TIxExternalCLK1Source_TI1))

/**
  * @brief Macro TIM3  Trigger Polarity
  */
#define IS_TIM3_EXT_POLARITY(POLARITY) \
  (((POLARITY) == TIM3_ExtTRGPolarity_Inverted) || \
   ((POLARITY) == TIM3_ExtTRGPolarity_NonInverted))

/**
  * @brief Macro TIM3 External Trigger Filter
  */
#define IS_TIM3_EXT_FILTER(EXTFILTER) \
  ((EXTFILTER) <= 0x0F)

/**
  * @brief Macro TIM3 Prescaler Reload
  */
#define IS_TIM3_PRESCALER_RELOAD(RELOAD) \
  (((RELOAD) == TIM3_PSCReloadMode_Update) || \
   ((RELOAD) == TIM3_PSCReloadMode_Immediate))

/**
  * @brief Macro TIM3 encoder mode
  */
#define IS_TIM3_ENCODER_MODE(MODE) \
  (((MODE) == TIM3_EncoderMode_TI1) || \
   ((MODE) == TIM3_EncoderMode_TI2) || \
   ((MODE) == TIM3_EncoderMode_TI12))

/**
  * @brief Macro TIM3 event source
  */
#define IS_TIM3_EVENT_SOURCE(SOURCE) \
  ((((SOURCE) & (uint8_t)0x18) == 0x00) && ((SOURCE) != 0x00))

/**
  * @brief Macro TIM3 update source
  */
#define IS_TIM3_UPDATE_SOURCE(SOURCE) \
  (((SOURCE) == TIM3_UpdateSource_Global) || \
   ((SOURCE) == TIM3_UpdateSource_Regular))

/**
  * @brief Macro TIM3 TRGO source
  */
#define IS_TIM3_TRGO_SOURCE(SOURCE) \
  (((SOURCE) == TIM3_TRGOSource_Reset)  || \
   ((SOURCE) == TIM3_TRGOSource_Enable) || \
   ((SOURCE) == TIM3_TRGOSource_Update) || \
   ((SOURCE) == TIM3_TRGOSource_OC1)    || \
   ((SOURCE) == TIM3_TRGOSource_OC1REF) || \
   ((SOURCE) == TIM3_TRGOSource_OC2REF))
/**
  * @brief Macro TIM3 Slave mode
  */
#define IS_TIM3_SLAVE_MODE(MODE) \
  (((MODE) == TIM3_SlaveMode_Reset)   || \
   ((MODE) == TIM3_SlaveMode_Gated)   || \
   ((MODE) == TIM3_SlaveMode_Trigger) || \
   ((MODE) == TIM3_SlaveMode_External1))
/**
  * @brief Macro TIM3 Flags
  */
#define IS_TIM3_GET_FLAG(FLAG) \
  (((FLAG) == TIM3_FLAG_Update)  || \
   ((FLAG) == TIM3_FLAG_CC1)     || \
   ((FLAG) == TIM3_FLAG_CC2)     || \
   ((FLAG) == TIM3_FLAG_Trigger) || \
   ((FLAG) == TIM3_FLAG_Break)   || \
   ((FLAG) == TIM3_FLAG_CC1OF)   || \
   ((FLAG) == TIM3_FLAG_CC2OF))

#define IS_TIM3_CLEAR_FLAG(FLAG) \
  ((((FLAG) & (uint16_t)0xE100) == 0x0000) && ((FLAG) != 0x0000))

/**
  * @brief Macro TIM3 DMA sources
  */

#define IS_TIM3_DMA_SOURCE(SOURCE) \
  (((SOURCE) == TIM3_DMASource_Update) || \
   ((SOURCE) == TIM3_DMASource_CC1) || \
   ((SOURCE) == TIM3_DMASource_CC2))

/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */
/* TimeBase management ********************************************************/
void TIM3_DeInit(void);
void TIM3_TimeBaseInit(TIM3_Prescaler_TypeDef TIM3_Prescaler,
                       TIM3_CounterMode_TypeDef TIM3_CounterMode,
                       uint16_t TIM3_Period);
void TIM3_PrescalerConfig(TIM3_Prescaler_TypeDef Prescaler,
                          TIM3_PSCReloadMode_TypeDef TIM3_PSCReloadMode);
void TIM3_CounterModeConfig(TIM3_CounterMode_TypeDef TIM3_CounterMode);
void TIM3_SetCounter(uint16_t Counter);
void TIM3_SetAutoreload(uint16_t Autoreload);
uint16_t TIM3_GetCounter(void);
TIM3_Prescaler_TypeDef TIM3_GetPrescaler(void);
void TIM3_UpdateDisableConfig(FunctionalState NewState);
void TIM3_UpdateRequestConfig(TIM3_UpdateSource_TypeDef TIM3_UpdateSource);
void TIM3_ARRPreloadConfig(FunctionalState NewState);
void TIM3_SelectOnePulseMode(TIM3_OPMode_TypeDef TIM3_OPMode);
void TIM3_Cmd(FunctionalState NewState);

/* Output Compare management **************************************************/
void TIM3_OC1Init(TIM3_OCMode_TypeDef TIM3_OCMode,
                  TIM3_OutputState_TypeDef TIM3_OutputState,
                  uint16_t TIM3_Pulse,
                  TIM3_OCPolarity_TypeDef TIM3_OCPolarity,
                  TIM3_OCIdleState_TypeDef TIM3_OCIdleState);
void TIM3_OC2Init(TIM3_OCMode_TypeDef TIM3_OCMode,
                  TIM3_OutputState_TypeDef TIM3_OutputState,
                  uint16_t TIM3_Pulse,
                  TIM3_OCPolarity_TypeDef TIM3_OCPolarity,
                  TIM3_OCIdleState_TypeDef TIM3_OCIdleState);
void TIM3_BKRConfig(TIM3_OSSIState_TypeDef TIM3_OSSIState,
                    TIM3_LockLevel_TypeDef TIM3_LockLevel,
                    TIM3_BreakState_TypeDef TIM3_BreakState,
                    TIM3_BreakPolarity_TypeDef TIM3_BreakPolarity,
                    TIM3_AutomaticOutput_TypeDef TIM3_AutomaticOutput);
void TIM3_CtrlPWMOutputs(FunctionalState NewState);
void TIM3_SelectOCxM(TIM3_Channel_TypeDef TIM3_Channel,
                     TIM3_OCMode_TypeDef TIM3_OCMode);
void TIM3_SetCompare1(uint16_t Compare);
void TIM3_SetCompare2(uint16_t Compare);
void TIM3_ForcedOC1Config(TIM3_ForcedAction_TypeDef TIM3_ForcedAction);
void TIM3_ForcedOC2Config(TIM3_ForcedAction_TypeDef TIM3_ForcedAction);
void TIM3_OC1PreloadConfig(FunctionalState NewState);
void TIM3_OC2PreloadConfig(FunctionalState NewState);
void TIM3_OC1FastConfig(FunctionalState NewState);
void TIM3_OC2FastConfig(FunctionalState NewState);
void TIM3_OC1PolarityConfig(TIM3_OCPolarity_TypeDef TIM3_OCPolarity);
void TIM3_OC2PolarityConfig(TIM3_OCPolarity_TypeDef TIM3_OCPolarity);
void TIM3_CCxCmd(TIM3_Channel_TypeDef TIM3_Channel, FunctionalState NewState);

/* Input Capture management ***************************************************/
void TIM3_ICInit(TIM3_Channel_TypeDef TIM3_Channel,
                 TIM3_ICPolarity_TypeDef TIM3_ICPolarity,
                 TIM3_ICSelection_TypeDef TIM3_ICSelection,
                 TIM3_ICPSC_TypeDef TIM3_ICPrescaler,
                 uint8_t TIM3_ICFilter);
void TIM3_PWMIConfig(TIM3_Channel_TypeDef TIM3_Channel,
                     TIM3_ICPolarity_TypeDef TIM3_ICPolarity,
                     TIM3_ICSelection_TypeDef TIM3_ICSelection,
                     TIM3_ICPSC_TypeDef TIM3_ICPrescaler,
                     uint8_t TIM3_ICFilter);
uint16_t TIM3_GetCapture1(void);
uint16_t TIM3_GetCapture2(void);
void TIM3_SetIC1Prescaler(TIM3_ICPSC_TypeDef TIM3_IC1Prescaler);
void TIM3_SetIC2Prescaler(TIM3_ICPSC_TypeDef TIM3_IC2Prescaler);

/* Interrupts, DMA and flags management ***************************************/
void TIM3_ITConfig(TIM3_IT_TypeDef TIM3_IT, FunctionalState NewState);
void TIM3_GenerateEvent(TIM3_EventSource_TypeDef TIM3_EventSource);
FlagStatus TIM3_GetFlagStatus(TIM3_FLAG_TypeDef TIM3_FLAG);
void TIM3_ClearFlag(TIM3_FLAG_TypeDef TIM3_FLAG);
ITStatus TIM3_GetITStatus(TIM3_IT_TypeDef TIM3_IT);
void TIM3_ClearITPendingBit(TIM3_IT_TypeDef TIM3_IT);
void TIM3_DMACmd(TIM3_DMASource_TypeDef TIM3_DMASource, FunctionalState NewState);
void TIM3_SelectCCDMA(FunctionalState NewState);

/* Clocks management **********************************************************/
void TIM3_InternalClockConfig(void);
void TIM3_TIxExternalClockConfig(TIM3_TIxExternalCLK1Source_TypeDef TIM3_TIxExternalCLKSource,
                                 TIM3_ICPolarity_TypeDef TIM3_ICPolarity,
                                 uint8_t ICFilter);
void TIM3_ETRClockMode1Config(TIM3_ExtTRGPSC_TypeDef TIM3_ExtTRGPrescaler,
                              TIM3_ExtTRGPolarity_TypeDef TIM3_ExtTRGPolarity,
                              uint8_t ExtTRGFilter);
void TIM3_ETRClockMode2Config(TIM3_ExtTRGPSC_TypeDef TIM3_ExtTRGPrescaler,
                              TIM3_ExtTRGPolarity_TypeDef TIM3_ExtTRGPolarity,
                              uint8_t ExtTRGFilter);

/* Synchronization management *************************************************/
void TIM3_SelectInputTrigger(TIM3_TRGSelection_TypeDef TIM3_InputTriggerSource);
void TIM3_SelectOutputTrigger(TIM3_TRGOSource_TypeDef TIM3_TRGOSource);
void TIM3_SelectSlaveMode(TIM3_SlaveMode_TypeDef TIM3_SlaveMode);
void TIM3_SelectMasterSlaveMode(FunctionalState NewState);
void TIM3_ETRConfig(TIM3_ExtTRGPSC_TypeDef TIM3_ExtTRGPrescaler,
                    TIM3_ExtTRGPolarity_TypeDef TIM3_ExtTRGPolarity,
                    uint8_t ExtTRGFilter);

/* Specific interface management **********************************************/
void TIM3_EncoderInterfaceConfig(TIM3_EncoderMode_TypeDef TIM3_EncoderMode,
                                 TIM3_ICPolarity_TypeDef TIM3_IC1Polarity,
                                 TIM3_ICPolarity_TypeDef TIM3_IC2Polarity);
void TIM3_SelectHallSensor(FunctionalState NewState);

#endif /* __STM8L15x_TIM3_H */

/**
  * @}
  */
  
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
