/**
  ******************************************************************************
  * @file    stm8l15x_tim2.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the TIM2 firmware
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
#ifndef __STM8L15x_TIM2_H
#define __STM8L15x_TIM2_H


/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
  
/** @addtogroup TIM2
  * @{
  */
   
/* Exported types ------------------------------------------------------------*/

/** @defgroup TIM2_Exported_Types
  * @{
  */

/** @defgroup TIM2_Forced_Action
  * @{
  */
typedef enum
{
  TIM2_ForcedAction_Active   = ((uint8_t)0x50),   /*!< Output Reference is forced low */
  TIM2_ForcedAction_Inactive = ((uint8_t)0x40)    /*!< Output Reference is forced high */
}
TIM2_ForcedAction_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Prescaler
  * @{
  */
typedef enum
{
  TIM2_Prescaler_1     = ((uint8_t)0x00),   /*!< Time base Prescaler = 1 (No effect)*/
  TIM2_Prescaler_2     = ((uint8_t)0x01),   /*!< Time base Prescaler = 2 */
  TIM2_Prescaler_4     = ((uint8_t)0x02),   /*!< Time base Prescaler = 4 */
  TIM2_Prescaler_8     = ((uint8_t)0x03),   /*!< Time base Prescaler = 8 */
  TIM2_Prescaler_16    = ((uint8_t)0x04),   /*!< Time base Prescaler = 16 */
  TIM2_Prescaler_32    = ((uint8_t)0x05),   /*!< Time base Prescaler = 32 */
  TIM2_Prescaler_64    = ((uint8_t)0x06),   /*!< Time base Prescaler = 64 */
  TIM2_Prescaler_128   = ((uint8_t)0x07)    /*!< Time base Prescaler = 128 */
}TIM2_Prescaler_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_OCMode
  * @{
  */
typedef enum
{
  TIM2_OCMode_Timing    = ((uint8_t)0x00),   /*!< Timing (Frozen) Mode*/
  TIM2_OCMode_Active    = ((uint8_t)0x10),   /*!< Active Mode*/
  TIM2_OCMode_Inactive  = ((uint8_t)0x20),   /*!< Inactive Mode*/
  TIM2_OCMode_Toggle    = ((uint8_t)0x30),   /*!< Toggle Mode*/
  TIM2_OCMode_PWM1      = ((uint8_t)0x60),   /*!< PWM Mode 1*/
  TIM2_OCMode_PWM2      = ((uint8_t)0x70)    /*!< PWM Mode 2*/
}TIM2_OCMode_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_OnePulseMode
  * @{
  */
typedef enum
{
  TIM2_OPMode_Single      = ((uint8_t)0x01), /*!< Single one Pulse mode (OPM Active) */
  TIM2_OPMode_Repetitive  = ((uint8_t)0x00)  /*!< Repetitive Pulse mode (OPM inactive) */
}TIM2_OPMode_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Channel
  * @{
  */
typedef enum
{
  TIM2_Channel_1  = ((uint8_t)0x00),  /*!< Channel 1*/
  TIM2_Channel_2  = ((uint8_t)0x01)   /*!< Channel 2*/
}TIM2_Channel_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_CounterMode
  * @{
  */
typedef enum
{
  TIM2_CounterMode_Up               = ((uint8_t)0x00),   /*!< Counter Up Mode */
  TIM2_CounterMode_Down             = ((uint8_t)0x10),   /*!< Counter Down Mode */
  TIM2_CounterMode_CenterAligned1   = ((uint8_t)0x20),   /*!< Counter Central aligned Mode 1 */
  TIM2_CounterMode_CenterAligned2   = ((uint8_t)0x40),   /*!< Counter Central aligned Mode 2 */
  TIM2_CounterMode_CenterAligned3   = ((uint8_t)0x60)    /*!< Counter Central aligned Mode 3 */
}TIM2_CounterMode_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Output_Compare_Polarity
  * @{
  */
typedef enum
{
  TIM2_OCPolarity_High   = ((uint8_t)0x00),   /*!< Output compare polarity  = High */
  TIM2_OCPolarity_Low    = ((uint8_t)0x01)    /*!< Output compare polarity  = Low */
}TIM2_OCPolarity_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Output_State
  * @{
  */
typedef enum
{
  TIM2_OutputState_Disable   = ((uint8_t)0x00),   /*!< Output compare State disabled (channel output disabled) */
  TIM2_OutputState_Enable    = ((uint8_t)0x01)    /*!< Output compare State enabled (channel output enabled) */
}TIM2_OutputState_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Break_State
  * @{
  */
typedef enum
{
  TIM2_BreakState_Disable  = ((uint8_t)0x00),   /*!< Break State disabled (break option disabled) */
  TIM2_BreakState_Enable   = ((uint8_t)0x10)    /*!< Break State enabled (break option enabled) */

}TIM2_BreakState_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Break_Polarity
  * @{
  */
typedef enum
{
  TIM2_BreakPolarity_High  = ((uint8_t)0x20),  /*!< if Break, channel polarity = High */
  TIM2_BreakPolarity_Low   = ((uint8_t)0x00)   /*!< if Break, channel polarity = Low */

}TIM2_BreakPolarity_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Automatic_Output
  * @{
  */  
typedef enum
{
  TIM2_AutomaticOutput_Enable    = ((uint8_t)0x40),   /*!< Automatic Output option enabled */
  TIM2_AutomaticOutput_Disable   = ((uint8_t)0x00)    /*!< Automatic Output option disabled */
}TIM2_AutomaticOutput_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Lock_Level
  * @{
  */
typedef enum
{
  TIM2_LockLevel_Off  = ((uint8_t)0x00),   /*!< Lock option disabled */
  TIM2_LockLevel_1    = ((uint8_t)0x01),   /*!< Select Lock Level 1  */
  TIM2_LockLevel_2    = ((uint8_t)0x02),   /*!< Select Lock Level 2  */
  TIM2_LockLevel_3    = ((uint8_t)0x03)    /*!< Select Lock Level 3  */
}TIM2_LockLevel_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_OSSI_State
  * @{
  */
typedef enum
{
  TIM2_OSSIState_Enable    = ((uint8_t)0x04),   /*!< Off-State Selection for Idle mode enabled  */
  TIM2_OSSIState_Disable   = ((uint8_t)0x00)    /*!< Off-State Selection for Idle mode disabled  */
}TIM2_OSSIState_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Output_Compare_Idle_state
  * @{
  */
typedef enum
{
  TIM2_OCIdleState_Reset  = ((uint8_t)0x00),   /*!< Output Compare Idle state  = Reset */
  TIM2_OCIdleState_Set    = ((uint8_t)0x01)    /*!< Output Compare Idle state  = Set */
}TIM2_OCIdleState_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Input_Capture_Polarity
  * @{
  */
typedef enum
{
  TIM2_ICPolarity_Rising   = ((uint8_t)0x00),   /*!< Input Capture on Rising Edge*/
  TIM2_ICPolarity_Falling  = ((uint8_t)0x01)    /*!< Input Capture on Falling Edge*/
}TIM2_ICPolarity_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Input_Capture_Selection
  * @{
  */
typedef enum
{
  TIM2_ICSelection_DirectTI    = ((uint8_t)0x01),   /*!< Input Capture mapped on the direct input*/
  TIM2_ICSelection_IndirectTI  = ((uint8_t)0x02),   /*!< Input Capture mapped on the indirect input*/
  TIM2_ICSelection_TRGI        = ((uint8_t)0x03)    /*!< Input Capture mapped on the Trigger Input*/
}TIM2_ICSelection_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Input_Capture_Prescaler
  * @{
  */
typedef enum
{
  TIM2_ICPSC_DIV1  = ((uint8_t)0x00),  /*!< Input Capture Prescaler = 1 (one capture every 1 event) */
  TIM2_ICPSC_DIV2  = ((uint8_t)0x04),  /*!< Input Capture Prescaler = 2 (one capture every 2 events) */
  TIM2_ICPSC_DIV4  = ((uint8_t)0x08),  /*!< Input Capture Prescaler = 4 (one capture every 4 events) */
  TIM2_ICPSC_DIV8  = ((uint8_t)0x0C)   /*!< Input Capture Prescaler = 8 (one capture every 8 events) */
}TIM2_ICPSC_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Interrupts
  * @{
  */
typedef enum
{
  TIM2_IT_Update   = ((uint8_t)0x01),   /*!< Update Interrupt*/
  TIM2_IT_CC1      = ((uint8_t)0x02),   /*!< Capture Compare Channel1 Interrupt*/
  TIM2_IT_CC2      = ((uint8_t)0x04),   /*!< Capture Compare Channel2 Interrupt*/
  TIM2_IT_Trigger  = ((uint8_t)0x40),   /*!< Trigger  Interrupt*/
  TIM2_IT_Break    = ((uint8_t)0x80)    /*!< Break Interrupt*/
}TIM2_IT_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_External_Trigger_Prescaler
  * @{
  */
typedef enum
{
  TIM2_ExtTRGPSC_OFF   = ((uint8_t)0x00),   /*!< No External Trigger prescaler  */
  TIM2_ExtTRGPSC_DIV2  = ((uint8_t)0x10),   /*!< External Trigger prescaler = 2 (ETRP frequency divided by 2) */
  TIM2_ExtTRGPSC_DIV4  = ((uint8_t)0x20),   /*!< External Trigger prescaler = 4 (ETRP frequency divided by 4) */
  TIM2_ExtTRGPSC_DIV8  = ((uint8_t)0x30)    /*!< External Trigger prescaler = 8 (ETRP frequency divided by 8) */
}TIM2_ExtTRGPSC_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Internal_Trigger_Selection
  * @{
  */
typedef enum
{
  TIM2_TRGSelection_TIM4     = ((uint8_t)0x00),  /*!< TRIG Input source =  TIM TRIG Output  */
  TIM2_TRGSelection_TIM1     = ((uint8_t)0x10),  /*!< TRIG Input source =  TIM TRIG Output  */
  TIM2_TRGSelection_TIM3     = ((uint8_t)0x20),  /*!< TRIG Input source =  TIM TRIG Output  */
  TIM2_TRGSelection_TIM5     = ((uint8_t)0x30),  /*!< TRIG Input source =  TIM TRIG Output  */
  TIM2_TRGSelection_TI1F_ED  = ((uint8_t)0x40),  /*!< TRIG Input source = TI1F_ED (TI1 Edge Detector)  */
  TIM2_TRGSelection_TI1FP1   = ((uint8_t)0x50),  /*!< TRIG Input source = TI1FP1 (Filtered Timer Input 1) */
  TIM2_TRGSelection_TI2FP2   = ((uint8_t)0x60),  /*!< TRIG Input source = TI2FP2 (Filtered Timer Input 2)  */
  TIM2_TRGSelection_ETRF     = ((uint8_t)0x70)   /*!< TRIG Input source =  ETRF (External Trigger Input ) */
}TIM2_TRGSelection_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_TI_External_Clock_Source
  * @{
  */
typedef enum
{
  TIM2_TIxExternalCLK1Source_TI1ED  = ((uint8_t)0x40),   /*!< External Clock mode 1 source = TI1ED */
  TIM2_TIxExternalCLK1Source_TI1    = ((uint8_t)0x50),   /*!< External Clock mode 1 source = TI1 */
  TIM2_TIxExternalCLK1Source_TI2    = ((uint8_t)0x60)    /*!< External Clock mode 1 source = TI2 */
}TIM2_TIxExternalCLK1Source_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_External_Trigger_Polarity
  * @{
  */
typedef enum
{
  TIM2_ExtTRGPolarity_Inverted     = ((uint8_t)0x80),   /*!< External Trigger Polarity = inverted */
  TIM2_ExtTRGPolarity_NonInverted  = ((uint8_t)0x00)    /*!< External Trigger Polarity = non inverted */
}TIM2_ExtTRGPolarity_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Prescaler_Reload_Mode
  * @{
  */
typedef enum
{
  TIM2_PSCReloadMode_Update      = ((uint8_t)0x00),   /*!< Prescaler value is reloaded at every update*/
  TIM2_PSCReloadMode_Immediate   = ((uint8_t)0x01)    /*!< Prescaler value is reloaded immediatly*/
}TIM2_PSCReloadMode_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Encoder_Mode
  * @{
  */
typedef enum
{
  TIM2_EncoderMode_TI1     = ((uint8_t)0x01),   /*!< Encoder mode 1*/
  TIM2_EncoderMode_TI2     = ((uint8_t)0x02),   /*!< Encoder mode 2*/
  TIM2_EncoderMode_TI12    = ((uint8_t)0x03)    /*!< Encoder mode 3*/
}TIM2_EncoderMode_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Event_Source
  * @{
  */
typedef enum
{
  TIM2_EventSource_Update   = ((uint8_t)0x01),   /*!< Update Event*/
  TIM2_EventSource_CC1      = ((uint8_t)0x02),   /*!< Capture Compare Channel1  Event*/
  TIM2_EventSource_CC2      = ((uint8_t)0x04),   /*!< Capture Compare Channel2 Event*/
  TIM2_EventSource_Trigger  = ((uint8_t)0x40),   /*!< Trigger Event*/
  TIM2_EventSource_Break    = ((uint8_t)0x80)    /*!< Break Event*/
}TIM2_EventSource_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Update_Source
  * @{
  */
typedef enum
{
  TIM2_UpdateSource_Global   = ((uint8_t)0x00),   /*!< Global Update request source */
  TIM2_UpdateSource_Regular  = ((uint8_t)0x01)    /*!< Regular Update request source */
}TIM2_UpdateSource_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Trigger_Output_Source
  * @{
  */
typedef enum
{
  TIM2_TRGOSource_Reset    = ((uint8_t)0x00),   /*!< Trigger Output source = Reset*/
  TIM2_TRGOSource_Enable   = ((uint8_t)0x10),   /*!< Trigger Output source = TIM2 is enabled*/
  TIM2_TRGOSource_Update   = ((uint8_t)0x20),   /*!< Trigger Output source = Update event*/
  TIM2_TRGOSource_OC1      = ((uint8_t)0x30),   /*!< Trigger Output source = output compare channel1  */
  TIM2_TRGOSource_OC1REF   = ((uint8_t)0x40),   /*!< Trigger Output source = output compare channel 1 reference */
  TIM2_TRGOSource_OC2REF   = ((uint8_t)0x50)    /*!< Trigger Output source = output compare channel 2 reference */
}TIM2_TRGOSource_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Slave_Mode
  * @{
  */
typedef enum
{
  TIM2_SlaveMode_Reset      = ((uint8_t)0x04),  /*!< Slave Mode Selection  = Reset*/
  TIM2_SlaveMode_Gated      = ((uint8_t)0x05),  /*!< Slave Mode Selection  = Gated*/
  TIM2_SlaveMode_Trigger    = ((uint8_t)0x06),  /*!< Slave Mode Selection  = Trigger*/
  TIM2_SlaveMode_External1  = ((uint8_t)0x07)   /*!< Slave Mode Selection  = External 1*/
}TIM2_SlaveMode_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_Flags
  * @{
  */
typedef enum
{
  TIM2_FLAG_Update   = ((uint16_t)0x0001),  /*!< Update Flag */
  TIM2_FLAG_CC1      = ((uint16_t)0x0002),  /*!< Capture compare 1 Flag */
  TIM2_FLAG_CC2      = ((uint16_t)0x0004),  /*!< Capture compare 2 Flag */
  TIM2_FLAG_Trigger  = ((uint16_t)0x0040),  /*!< Trigger Flag */
  TIM2_FLAG_Break    = ((uint16_t)0x0080),  /*!< Break Flag */
  TIM2_FLAG_CC1OF    = ((uint16_t)0x0200),  /*!< Capture compare 1 over capture Flag */
  TIM2_FLAG_CC2OF    = ((uint16_t)0x0400)   /*!< Capture compare 2 over capture Flag */
}TIM2_FLAG_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM2_DMA_Source_Requests
  * @{
  */
typedef enum
{
  TIM2_DMASource_Update   = ((uint8_t)0x01),  /*!< TIM2 DMA Update Request*/
  TIM2_DMASource_CC1      = ((uint8_t)0x02),  /*!< TIM2 DMA CC1 Request*/
  TIM2_DMASource_CC2      = ((uint8_t)0x04)   /*!< TIM2 DMA CC2 Request*/
}TIM2_DMASource_TypeDef;

/**
  * @}
  */
  
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/** @defgroup TIM2_Exported_Macros
  * @{
  */

/**
  * @brief Macro used by the assert function to check the different functions parameters.
  */

/**
  * @brief Macro TIM2 Forced Action
  */
#define IS_TIM2_FORCED_ACTION(ACTION) \
  (((ACTION) == TIM2_ForcedAction_Active) || \
   ((ACTION) == TIM2_ForcedAction_Inactive))

/**
  * @brief Macro TIM2 Prescaler
  */
#define IS_TIM2_PRESCALER(PRESCALER) \
  (((PRESCALER) == TIM2_Prescaler_1)  || \
   ((PRESCALER) == TIM2_Prescaler_2)  || \
   ((PRESCALER) == TIM2_Prescaler_4)  || \
   ((PRESCALER) == TIM2_Prescaler_8)  || \
   ((PRESCALER) == TIM2_Prescaler_16) || \
   ((PRESCALER) == TIM2_Prescaler_32) || \
   ((PRESCALER) == TIM2_Prescaler_64) || \
   ((PRESCALER) == TIM2_Prescaler_128))

/**
  * @brief Macro TIM2 Output Compare and PWM modes
  */
#define IS_TIM2_OC_MODE(MODE) \
  (((MODE) ==  TIM2_OCMode_Timing)  || \
   ((MODE) == TIM2_OCMode_Active)   || \
   ((MODE) == TIM2_OCMode_Inactive) || \
   ((MODE) == TIM2_OCMode_Toggle)   || \
   ((MODE) == TIM2_OCMode_PWM1)     || \
   ((MODE) == TIM2_OCMode_PWM2))

#define IS_TIM2_OCM(MODE) \
  (((MODE) ==  TIM2_OCMode_Timing)  || \
   ((MODE) == TIM2_OCMode_Active)   || \
   ((MODE) == TIM2_OCMode_Inactive) || \
   ((MODE) == TIM2_OCMode_Toggle)   || \
   ((MODE) == TIM2_OCMode_PWM1)     || \
   ((MODE) == TIM2_OCMode_PWM2)     || \
   ((MODE) == (uint8_t)TIM2_ForcedAction_Active) || \
   ((MODE) == (uint8_t)TIM2_ForcedAction_Inactive))
/**
  * @brief Macro TIM2 One Pulse Mode
  */
#define IS_TIM2_OPM_MODE(MODE) \
  (((MODE) == TIM2_OPMode_Single) || \
   ((MODE) == TIM2_OPMode_Repetitive))

/**
  * @brief Macro TIM2 Channel
  */
#define IS_TIM2_CHANNEL(CHANNEL) \
  (((CHANNEL) == TIM2_Channel_1) || \
   ((CHANNEL) == TIM2_Channel_2) )

/**
  * @brief Macro TIM2 Counter Mode
  */
#define IS_TIM2_COUNTER_MODE(MODE) \
  (((MODE) == TIM2_CounterMode_Up)   || \
   ((MODE) == TIM2_CounterMode_Down) || \
   ((MODE) == TIM2_CounterMode_CenterAligned1) || \
   ((MODE) == TIM2_CounterMode_CenterAligned2) || \
   ((MODE) == TIM2_CounterMode_CenterAligned3))

/**
  * @brief Macro TIM2 Output Compare Polarity
  */
#define IS_TIM2_OC_POLARITY(POLARITY) \
  (((POLARITY) == TIM2_OCPolarity_High) || \
   ((POLARITY) == TIM2_OCPolarity_Low))

/**
  * @brief Macro TIM2 Output Compare states
  */
#define IS_TIM2_OUTPUT_STATE(STATE) \
  (((STATE) == TIM2_OutputState_Disable) || \
   ((STATE) == TIM2_OutputState_Enable))

/**
 * @brief Macro Break Input enable/disable
 */
#define IS_TIM2_BREAK_STATE(STATE) \
  (((STATE) == TIM2_BreakState_Enable) || \
   ((STATE) == TIM2_BreakState_Disable))

/**
  * @brief Macro Break Polarity
  */
#define IS_TIM2_BREAK_POLARITY(POLARITY) \
  (((POLARITY) == TIM2_BreakPolarity_Low) || \
   ((POLARITY) == TIM2_BreakPolarity_High))

/**
  * @brief Macro TIM2 AOE Bit Set/Reset
  */
#define IS_TIM2_AUTOMATIC_OUTPUT_STATE(STATE) \
  (((STATE) == TIM2_AutomaticOutput_Enable) || \
   ((STATE) == TIM2_AutomaticOutput_Disable))

/**
  * @brief Macro Lock levels
  */
#define IS_TIM2_LOCK_LEVEL(LEVEL) \
  (((LEVEL) == TIM2_LockLevel_Off) || \
   ((LEVEL) == TIM2_LockLevel_1)   || \
   ((LEVEL) == TIM2_LockLevel_2)   || \
   ((LEVEL) == TIM2_LockLevel_3))

/**
  * @brief Macro OSSI: Off-State Selection for Idle mode states
  */
#define IS_TIM2_OSSI_STATE(STATE) \
  (((STATE) == TIM2_OSSIState_Enable) || \
   ((STATE) == TIM2_OSSIState_Disable))

/**
  * @brief Macro TIM2 OC IDLE STATE
  */
#define IS_TIM2_OCIDLE_STATE(STATE) \
  (((STATE) == TIM2_OCIdleState_Set) || \
   ((STATE) == TIM2_OCIdleState_Reset))

/**
  * @brief Macro TIM2 IC POLARITY
  */

#define IS_TIM2_IC_POLARITY(POLARITY) \
  (((POLARITY) == TIM2_ICPolarity_Rising) || \
   ((POLARITY) == TIM2_ICPolarity_Falling))

/**
  * @brief Macro TIM2 IC SELECTION
  */
#define IS_TIM2_IC_SELECTION(SELECTION) \
  (((SELECTION) == TIM2_ICSelection_DirectTI)   || \
   ((SELECTION) == TIM2_ICSelection_IndirectTI) || \
   ((SELECTION) == TIM2_ICSelection_TRGI))

/**
  * @brief Macro TIM2 IC PRESCALER
  */
#define IS_TIM2_IC_PRESCALER(PRESCALER) \
  (((PRESCALER) == TIM2_ICPSC_DIV1) || \
   ((PRESCALER) == TIM2_ICPSC_DIV2) || \
   ((PRESCALER) == TIM2_ICPSC_DIV4) || \
   ((PRESCALER) == TIM2_ICPSC_DIV8))

/**
  * @brief Macro TIM2 Input Capture Filter Value
  */
#define IS_TIM2_IC_FILTER(ICFILTER) \
  ((ICFILTER) <= 0x0F)

/**
  * @brief Macro TIM2 Interrupts
  */
#define IS_TIM2_IT(IT) \
  ((IT) != 0x00)

#define IS_TIM2_GET_IT(IT) \
  (((IT) == TIM2_IT_Update)  || \
   ((IT) == TIM2_IT_CC1)     || \
   ((IT) == TIM2_IT_CC2)     || \
   ((IT) == TIM2_IT_Trigger) || \
   ((IT) == TIM2_IT_Break))

/**
  * @brief Macro TIM2 external trigger prescaler
  */
#define IS_TIM2_EXT_PRESCALER(PRESCALER) \
  (((PRESCALER) == TIM2_ExtTRGPSC_OFF)  || \
   ((PRESCALER) == TIM2_ExtTRGPSC_DIV2) || \
   ((PRESCALER) == TIM2_ExtTRGPSC_DIV4) || \
   ((PRESCALER) == TIM2_ExtTRGPSC_DIV8))
/**
  * @brief Macro TIM2  Trigger Selection
  */
#define IS_TIM2_TRIGGER_SELECTION(SELECTION) \
  (((SELECTION) == TIM2_TRGSelection_TIM4) || \
   ((SELECTION) == TIM2_TRGSelection_TIM1) || \
   ((SELECTION) == TIM2_TRGSelection_TIM3) || \
   ((SELECTION) == TIM2_TRGSelection_TIM5) || \
   ((SELECTION) == TIM2_TRGSelection_TI1F_ED) || \
   ((SELECTION) == TIM2_TRGSelection_TI1FP1)  || \
   ((SELECTION) == TIM2_TRGSelection_TI2FP2)  || \
   ((SELECTION) == TIM2_TRGSelection_ETRF))


#define IS_TIM2_TIX_TRIGGER_SELECTION(SELECTION) \
  (((SELECTION) == TIM2_TRGSelection_TI1F_ED) || \
   ((SELECTION) == TIM2_TRGSelection_TI1FP1)  || \
   ((SELECTION) == TIM2_TRGSelection_TI2FP2))

/**
  * @brief Macro TIM2  TIx external Clock Selection
  */
#define IS_TIM2_TIXCLK_SOURCE(SOURCE)  \
  (((SOURCE) == TIM2_TIxExternalCLK1Source_TI1ED) || \
   ((SOURCE) == TIM2_TIxExternalCLK1Source_TI2)   || \
   ((SOURCE) == TIM2_TIxExternalCLK1Source_TI1))

/**
  * @brief Macro TIM2  Trigger Polarity
  */
#define IS_TIM2_EXT_POLARITY(POLARITY) \
  (((POLARITY) == TIM2_ExtTRGPolarity_Inverted) || \
   ((POLARITY) == TIM2_ExtTRGPolarity_NonInverted))

/**
  * @brief Macro TIM2 External Trigger Filter
  */
#define IS_TIM2_EXT_FILTER(EXTFILTER) \
  ((EXTFILTER) <= 0x0F)

/**
  * @brief Macro TIM2 Prescaler Reload
  */
#define IS_TIM2_PRESCALER_RELOAD(RELOAD) \
  (((RELOAD) == TIM2_PSCReloadMode_Update) || \
   ((RELOAD) == TIM2_PSCReloadMode_Immediate))

/**
  * @brief Macro TIM2 encoder mode
  */
#define IS_TIM2_ENCODER_MODE(MODE) \
  (((MODE) == TIM2_EncoderMode_TI1) || \
   ((MODE) == TIM2_EncoderMode_TI2) || \
   ((MODE) == TIM2_EncoderMode_TI12))

/**
  * @brief Macro TIM2 event source
  */
#define IS_TIM2_EVENT_SOURCE(SOURCE) \
  ((((SOURCE) & (uint8_t)0x18) == 0x00) && \
   ((SOURCE) != 0x00))

/**
  * @brief Macro TIM2 update source
  */
#define IS_TIM2_UPDATE_SOURCE(SOURCE) \
  (((SOURCE) == TIM2_UpdateSource_Global) || \
   ((SOURCE) == TIM2_UpdateSource_Regular))

/**
  * @brief Macro TIM2 TRGO source
  */
#define IS_TIM2_TRGO_SOURCE(SOURCE) \
  (((SOURCE) == TIM2_TRGOSource_Reset)  || \
   ((SOURCE) == TIM2_TRGOSource_Enable) || \
   ((SOURCE) == TIM2_TRGOSource_Update) || \
   ((SOURCE) == TIM2_TRGOSource_OC1)    || \
   ((SOURCE) == TIM2_TRGOSource_OC1REF) || \
   ((SOURCE) == TIM2_TRGOSource_OC2REF))
/**
  * @brief Macro TIM2 Slave mode
  */
#define IS_TIM2_SLAVE_MODE(MODE) \
  (((MODE) == TIM2_SlaveMode_Reset)   || \
   ((MODE) == TIM2_SlaveMode_Gated)   || \
   ((MODE) == TIM2_SlaveMode_Trigger) || \
   ((MODE) == TIM2_SlaveMode_External1))
/**
  * @brief Macro TIM2 Flags
  */
#define IS_TIM2_GET_FLAG(FLAG) \
  (((FLAG) == TIM2_FLAG_Update)  || \
   ((FLAG) == TIM2_FLAG_CC1)     || \
   ((FLAG) == TIM2_FLAG_CC2)     || \
   ((FLAG) == TIM2_FLAG_Trigger) || \
   ((FLAG) == TIM2_FLAG_Break)   || \
   ((FLAG) == TIM2_FLAG_CC1OF)   || \
   ((FLAG) == TIM2_FLAG_CC2OF))

#define IS_TIM2_CLEAR_FLAG(FLAG) \
  ((((FLAG) & (uint16_t)0xE100) == 0x0000) && ((FLAG) != 0x0000))

/**
  * @brief Macro TIM2 DMA sources
  */

#define IS_TIM2_DMA_SOURCE(SOURCE) \
  (((SOURCE) == TIM2_DMASource_Update) || \
   ((SOURCE) == TIM2_DMASource_CC1) || \
   ((SOURCE) == TIM2_DMASource_CC2))

/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */
/* TimeBase management ********************************************************/
void TIM2_DeInit(void);
void TIM2_TimeBaseInit(TIM2_Prescaler_TypeDef TIM2_Prescaler,
                       TIM2_CounterMode_TypeDef TIM2_CounterMode, uint16_t TIM2_Period);
void TIM2_PrescalerConfig(TIM2_Prescaler_TypeDef Prescaler,
                          TIM2_PSCReloadMode_TypeDef TIM2_PSCReloadMode);
void TIM2_CounterModeConfig(TIM2_CounterMode_TypeDef TIM2_CounterMode);
void TIM2_SetCounter(uint16_t Counter);
void TIM2_SetAutoreload(uint16_t Autoreload);
uint16_t TIM2_GetCounter(void);
TIM2_Prescaler_TypeDef TIM2_GetPrescaler(void);
void TIM2_UpdateDisableConfig(FunctionalState NewState);
void TIM2_UpdateRequestConfig(TIM2_UpdateSource_TypeDef TIM2_UpdateSource);
void TIM2_ARRPreloadConfig(FunctionalState NewState);
void TIM2_SelectOnePulseMode(TIM2_OPMode_TypeDef TIM2_OPMode);
void TIM2_Cmd(FunctionalState NewState);

/* Output Compare management **************************************************/
void TIM2_OC1Init(TIM2_OCMode_TypeDef TIM2_OCMode,
                  TIM2_OutputState_TypeDef TIM2_OutputState,
                  uint16_t TIM2_Pulse,
                  TIM2_OCPolarity_TypeDef TIM2_OCPolarity,
                  TIM2_OCIdleState_TypeDef TIM2_OCIdleState);
void TIM2_OC2Init(TIM2_OCMode_TypeDef TIM2_OCMode,
                  TIM2_OutputState_TypeDef TIM2_OutputState,
                  uint16_t TIM2_Pulse,
                  TIM2_OCPolarity_TypeDef TIM2_OCPolarity,
                  TIM2_OCIdleState_TypeDef TIM2_OCIdleState);
void TIM2_BKRConfig(TIM2_OSSIState_TypeDef TIM2_OSSIState,
                    TIM2_LockLevel_TypeDef TIM2_LockLevel,
                    TIM2_BreakState_TypeDef TIM2_BreakState,
                    TIM2_BreakPolarity_TypeDef TIM2_BreakPolarity,
                    TIM2_AutomaticOutput_TypeDef TIM2_AutomaticOutput);
void TIM2_CtrlPWMOutputs(FunctionalState NewState);
void TIM2_SelectOCxM(TIM2_Channel_TypeDef TIM2_Channel, TIM2_OCMode_TypeDef TIM2_OCMode);
void TIM2_SetCompare1(uint16_t Compare);
void TIM2_SetCompare2(uint16_t Compare);
void TIM2_ForcedOC1Config(TIM2_ForcedAction_TypeDef TIM2_ForcedAction);
void TIM2_ForcedOC2Config(TIM2_ForcedAction_TypeDef TIM2_ForcedAction);
void TIM2_OC1PreloadConfig(FunctionalState NewState);
void TIM2_OC2PreloadConfig(FunctionalState NewState);
void TIM2_OC1FastConfig(FunctionalState NewState);
void TIM2_OC2FastConfig(FunctionalState NewState);
void TIM2_OC1PolarityConfig(TIM2_OCPolarity_TypeDef TIM2_OCPolarity);
void TIM2_OC2PolarityConfig(TIM2_OCPolarity_TypeDef TIM2_OCPolarity);
void TIM2_CCxCmd(TIM2_Channel_TypeDef TIM2_Channel, FunctionalState NewState);

/* Input Capture management ***************************************************/
void TIM2_ICInit(TIM2_Channel_TypeDef TIM2_Channel,
                 TIM2_ICPolarity_TypeDef TIM2_ICPolarity,
                 TIM2_ICSelection_TypeDef TIM2_ICSelection,
                 TIM2_ICPSC_TypeDef TIM2_ICPrescaler,
                 uint8_t TIM2_ICFilter);
void TIM2_PWMIConfig(TIM2_Channel_TypeDef TIM2_Channel,
                     TIM2_ICPolarity_TypeDef TIM2_ICPolarity,
                     TIM2_ICSelection_TypeDef TIM2_ICSelection,
                     TIM2_ICPSC_TypeDef TIM2_ICPrescaler,
                     uint8_t TIM2_ICFilter);
uint16_t TIM2_GetCapture1(void);
uint16_t TIM2_GetCapture2(void);
void TIM2_SetIC1Prescaler(TIM2_ICPSC_TypeDef TIM2_IC1Prescaler);
void TIM2_SetIC2Prescaler(TIM2_ICPSC_TypeDef TIM2_IC2Prescaler);

/* Interrupts, DMA and flags management ***************************************/
void TIM2_ITConfig(TIM2_IT_TypeDef TIM2_IT, FunctionalState NewState);
void TIM2_GenerateEvent(TIM2_EventSource_TypeDef TIM2_EventSource);
FlagStatus TIM2_GetFlagStatus(TIM2_FLAG_TypeDef TIM2_FLAG);
void TIM2_ClearFlag(TIM2_FLAG_TypeDef TIM2_FLAG);
ITStatus TIM2_GetITStatus(TIM2_IT_TypeDef TIM2_IT);
void TIM2_ClearITPendingBit(TIM2_IT_TypeDef TIM2_IT);
void TIM2_DMACmd(TIM2_DMASource_TypeDef TIM2_DMASource, FunctionalState NewState);
void TIM2_SelectCCDMA(FunctionalState NewState);

/* Clocks management **********************************************************/
void TIM2_InternalClockConfig(void);
void TIM2_TIxExternalClockConfig(TIM2_TIxExternalCLK1Source_TypeDef TIM2_TIxExternalCLKSource,
                                 TIM2_ICPolarity_TypeDef TIM2_ICPolarity,
                                 uint8_t ICFilter);
void TIM2_ETRClockMode1Config(TIM2_ExtTRGPSC_TypeDef TIM2_ExtTRGPrescaler,
                              TIM2_ExtTRGPolarity_TypeDef TIM2_ExtTRGPolarity,
                              uint8_t ExtTRGFilter);
void TIM2_ETRClockMode2Config(TIM2_ExtTRGPSC_TypeDef TIM2_ExtTRGPrescaler,
                              TIM2_ExtTRGPolarity_TypeDef TIM2_ExtTRGPolarity,
                              uint8_t ExtTRGFilter);

/* Synchronization management *************************************************/
void TIM2_SelectInputTrigger(TIM2_TRGSelection_TypeDef TIM2_InputTriggerSource);
void TIM2_SelectOutputTrigger(TIM2_TRGOSource_TypeDef TIM2_TRGOSource);
void TIM2_SelectSlaveMode(TIM2_SlaveMode_TypeDef TIM2_SlaveMode);
void TIM2_SelectMasterSlaveMode(FunctionalState NewState);
void TIM2_ETRConfig(TIM2_ExtTRGPSC_TypeDef TIM2_ExtTRGPrescaler,
                    TIM2_ExtTRGPolarity_TypeDef TIM2_ExtTRGPolarity,
                    uint8_t ExtTRGFilter);

/* Specific interface management **********************************************/
void TIM2_EncoderInterfaceConfig(TIM2_EncoderMode_TypeDef TIM2_EncoderMode,
                                 TIM2_ICPolarity_TypeDef TIM2_IC1Polarity,
                                 TIM2_ICPolarity_TypeDef TIM2_IC2Polarity);
void TIM2_SelectHallSensor(FunctionalState NewState);

#endif /* __STM8L15x_TIM2_H */

/**
  * @}
  */
  
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
