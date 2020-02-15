/**
  ******************************************************************************
  * @file    stm8l15x_tim5.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the TIM5 firmware
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
#ifndef __STM8L15x_TIM5_H
#define __STM8L15x_TIM5_H


/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
  
/** @addtogroup TIM5
  * @{
  */ 
/* Exported types ------------------------------------------------------------*/

/** @defgroup TIM5_Exported_Types
  * @{
  */

/** @defgroup TIM5_Forced_Action
  * @{
  */
typedef enum
{
  TIM5_ForcedAction_Active   = ((uint8_t)0x50),   /*!< Output Reference is forced low */
  TIM5_ForcedAction_Inactive = ((uint8_t)0x40)    /*!< Output Reference is forced high */
}
TIM5_ForcedAction_TypeDef;

#define IS_TIM5_FORCED_ACTION(ACTION) (((ACTION) == TIM5_ForcedAction_Active) || \
                                       ((ACTION) == TIM5_ForcedAction_Inactive))
/**
  * @}
  */
  
/** @defgroup TIM5_Prescaler
  * @{
  */
typedef enum
{
  TIM5_Prescaler_1     = ((uint8_t)0x00),   /*!< Time base Prescaler = 1 (No effect)*/
  TIM5_Prescaler_2     = ((uint8_t)0x01),   /*!< Time base Prescaler = 2 */
  TIM5_Prescaler_4     = ((uint8_t)0x02),   /*!< Time base Prescaler = 4 */
  TIM5_Prescaler_8     = ((uint8_t)0x03),   /*!< Time base Prescaler = 8 */
  TIM5_Prescaler_16    = ((uint8_t)0x04),   /*!< Time base Prescaler = 16 */
  TIM5_Prescaler_32    = ((uint8_t)0x05),   /*!< Time base Prescaler = 32 */
  TIM5_Prescaler_64    = ((uint8_t)0x06),   /*!< Time base Prescaler = 64 */
  TIM5_Prescaler_128   = ((uint8_t)0x07)    /*!< Time base Prescaler = 128 */
}TIM5_Prescaler_TypeDef;

#define IS_TIM5_PRESCALER(PRESCALER) (((PRESCALER) == TIM5_Prescaler_1)  || \
                                      ((PRESCALER) == TIM5_Prescaler_2)  || \
                                      ((PRESCALER) == TIM5_Prescaler_4)  || \
                                      ((PRESCALER) == TIM5_Prescaler_8)  || \
                                      ((PRESCALER) == TIM5_Prescaler_16) || \
                                      ((PRESCALER) == TIM5_Prescaler_32) || \
                                      ((PRESCALER) == TIM5_Prescaler_64) || \
                                      ((PRESCALER) == TIM5_Prescaler_128))
/**
  * @}
  */
  
/** @defgroup TIM5_OCMode
  * @{
  */
typedef enum
{
  TIM5_OCMode_Timing    = ((uint8_t)0x00),   /*!< Timing (Frozen) Mode*/
  TIM5_OCMode_Active    = ((uint8_t)0x10),   /*!< Active Mode*/
  TIM5_OCMode_Inactive  = ((uint8_t)0x20),   /*!< Inactive Mode*/
  TIM5_OCMode_Toggle    = ((uint8_t)0x30),   /*!< Toggle Mode*/
  TIM5_OCMode_PWM1      = ((uint8_t)0x60),   /*!< PWM Mode 1*/
  TIM5_OCMode_PWM2      = ((uint8_t)0x70)    /*!< PWM Mode 2*/
}TIM5_OCMode_TypeDef;

#define IS_TIM5_OC_MODE(MODE) (((MODE) ==  TIM5_OCMode_Timing)  || \
                               ((MODE) == TIM5_OCMode_Active)   || \
                               ((MODE) == TIM5_OCMode_Inactive) || \
                               ((MODE) == TIM5_OCMode_Toggle)   || \
                               ((MODE) == TIM5_OCMode_PWM1)     || \
                               ((MODE) == TIM5_OCMode_PWM2))

#define IS_TIM5_OCM(MODE) (((MODE) ==  TIM5_OCMode_Timing)  || \
                           ((MODE) == TIM5_OCMode_Active)   || \
                           ((MODE) == TIM5_OCMode_Inactive) || \
                           ((MODE) == TIM5_OCMode_Toggle)   || \
                           ((MODE) == TIM5_OCMode_PWM1)     || \
                           ((MODE) == TIM5_OCMode_PWM2)     || \
                           ((MODE) == (uint8_t)TIM5_ForcedAction_Active) || \
                           ((MODE) == (uint8_t)TIM5_ForcedAction_Inactive))
/**
  * @}
  */
  
/** @defgroup TIM5_OnePulseMode
  * @{
  */
typedef enum
{
  TIM5_OPMode_Single      = ((uint8_t)0x01), /*!< Single one Pulse mode (OPM Active) */
  TIM5_OPMode_Repetitive  = ((uint8_t)0x00)  /*!< Repetitive Pulse mode (OPM inactive) */
}TIM5_OPMode_TypeDef;

#define IS_TIM5_OPM_MODE(MODE) (((MODE) == TIM5_OPMode_Single) || \
                                ((MODE) == TIM5_OPMode_Repetitive))
/**
  * @}
  */
  
/** @defgroup TIM5_Channel
  * @{
  */
typedef enum
{
  TIM5_Channel_1  = ((uint8_t)0x00),  /*!< Channel 1*/
  TIM5_Channel_2  = ((uint8_t)0x01)   /*!< Channel 2*/
}TIM5_Channel_TypeDef;

#define IS_TIM5_CHANNEL(CHANNEL) (((CHANNEL) == TIM5_Channel_1) || \
                                  ((CHANNEL) == TIM5_Channel_2) )
/**
  * @}
  */
  
/** @defgroup TIM5_CounterMode
  * @{
  */
typedef enum
{
  TIM5_CounterMode_Up               = ((uint8_t)0x00),   /*!< Counter Up Mode */
  TIM5_CounterMode_Down             = ((uint8_t)0x10),   /*!< Counter Down Mode */
  TIM5_CounterMode_CenterAligned1   = ((uint8_t)0x20),   /*!< Counter Central aligned Mode 1 */
  TIM5_CounterMode_CenterAligned2   = ((uint8_t)0x40),   /*!< Counter Central aligned Mode 2 */
  TIM5_CounterMode_CenterAligned3   = ((uint8_t)0x60)    /*!< Counter Central aligned Mode 3 */
}TIM5_CounterMode_TypeDef;

#define IS_TIM5_COUNTER_MODE(MODE) (((MODE) == TIM5_CounterMode_Up)   || \
                                    ((MODE) == TIM5_CounterMode_Down) || \
                                    ((MODE) == TIM5_CounterMode_CenterAligned1) || \
                                    ((MODE) == TIM5_CounterMode_CenterAligned2) || \
                                    ((MODE) == TIM5_CounterMode_CenterAligned3))
/**
  * @}
  */
  
/** @defgroup TIM5_Output_Compare_Polarity
  * @{
  */
typedef enum
{
  TIM5_OCPolarity_High   = ((uint8_t)0x00),   /*!< Output compare polarity  = High */
  TIM5_OCPolarity_Low    = ((uint8_t)0x01)    /*!< Output compare polarity  = Low */
}TIM5_OCPolarity_TypeDef;

#define IS_TIM5_OC_POLARITY(POLARITY) (((POLARITY) == TIM5_OCPolarity_High) || \
                                       ((POLARITY) == TIM5_OCPolarity_Low))
/**
  * @}
  */
  
/** @defgroup TIM5_Output_State
  * @{
  */
typedef enum
{
  TIM5_OutputState_Disable   = ((uint8_t)0x00),   /*!< Output compare State disabled (channel output disabled) */
  TIM5_OutputState_Enable    = ((uint8_t)0x01)    /*!< Output compare State enabled (channel output enabled) */
}TIM5_OutputState_TypeDef;

#define IS_TIM5_OUTPUT_STATE(STATE) (((STATE) == TIM5_OutputState_Disable) || \
                                     ((STATE) == TIM5_OutputState_Enable))

/**
  * @}
  */
  
/** @defgroup TIM5_Break_State
  * @{
  */
typedef enum
{
  TIM5_BreakState_Disable  = ((uint8_t)0x00),   /*!< Break State disabled (break option disabled) */
  TIM5_BreakState_Enable   = ((uint8_t)0x10)    /*!< Break State enabled (break option enabled) */
}TIM5_BreakState_TypeDef;

#define IS_TIM5_BREAK_STATE(STATE) (((STATE) == TIM5_BreakState_Enable) || \
                                    ((STATE) == TIM5_BreakState_Disable))
/**
  * @}
  */
  
/** @defgroup TIM5_Break_Polarity
  * @{
  */
typedef enum
{
  TIM5_BreakPolarity_High  = ((uint8_t)0x20),  /*!< if Break, channel polarity = High */
  TIM5_BreakPolarity_Low   = ((uint8_t)0x00)   /*!< if Break, channel polarity = Low */
}TIM5_BreakPolarity_TypeDef;

#define IS_TIM5_BREAK_POLARITY(POLARITY) \
  (((POLARITY) == TIM5_BreakPolarity_Low) || \
   ((POLARITY) == TIM5_BreakPolarity_High))
/**
  * @}
  */

/** @defgroup TIM5_Automatic_Output
  * @{
  */
typedef enum
{
  TIM5_AutomaticOutput_Enable    = ((uint8_t)0x40),   /*!< Automatic Output option enabled */
  TIM5_AutomaticOutput_Disable   = ((uint8_t)0x00)    /*!< Automatic Output option disabled */
}TIM5_AutomaticOutput_TypeDef;

#define IS_TIM5_AUTOMATIC_OUTPUT_STATE(STATE) \
  (((STATE) == TIM5_AutomaticOutput_Enable) || \
   ((STATE) == TIM5_AutomaticOutput_Disable))
/**
  * @}
  */
  
/** @defgroup TIM5_Lock_Level
  * @{
  */
typedef enum
{
  TIM5_LockLevel_Off  = ((uint8_t)0x00),   /*!< Lock option disabled */
  TIM5_LockLevel_1    = ((uint8_t)0x01),   /*!< Select Lock Level 1  */
  TIM5_LockLevel_2    = ((uint8_t)0x02),   /*!< Select Lock Level 2  */
  TIM5_LockLevel_3    = ((uint8_t)0x03)    /*!< Select Lock Level 3  */
}TIM5_LockLevel_TypeDef;

#define IS_TIM5_LOCK_LEVEL(LEVEL) (((LEVEL) == TIM5_LockLevel_Off) || \
                                   ((LEVEL) == TIM5_LockLevel_1)   || \
                                   ((LEVEL) == TIM5_LockLevel_2)   || \
                                   ((LEVEL) == TIM5_LockLevel_3))
/**
  * @}
  */
  
/** @defgroup TIM5_OSSI_State
  * @{
  */
typedef enum
{
  TIM5_OSSIState_Enable    = ((uint8_t)0x04),   /*!< Off-State Selection for Idle mode enabled  */
  TIM5_OSSIState_Disable   = ((uint8_t)0x00)    /*!< Off-State Selection for Idle mode disabled  */
}TIM5_OSSIState_TypeDef;

#define IS_TIM5_OSSI_STATE(STATE) \
  (((STATE) == TIM5_OSSIState_Enable) || \
   ((STATE) == TIM5_OSSIState_Disable))
/**
  * @}
  */
  
/** @defgroup TIM5_Output_Compare_Idle_state
  * @{
  */
typedef enum
{
  TIM5_OCIdleState_Reset  = ((uint8_t)0x00),   /*!< Output Compare Idle state  = Reset */
  TIM5_OCIdleState_Set    = ((uint8_t)0x01)    /*!< Output Compare Idle state  = Set */
}TIM5_OCIdleState_TypeDef;

#define IS_TIM5_OCIDLE_STATE(STATE) \
  (((STATE) == TIM5_OCIdleState_Set) || \
   ((STATE) == TIM5_OCIdleState_Reset))
/**
  * @}
  */
  
/** @defgroup TIM5_Input_Capture_Polarity
  * @{
  */
typedef enum
{
  TIM5_ICPolarity_Rising   = ((uint8_t)0x00),   /*!< Input Capture on Rising Edge*/
  TIM5_ICPolarity_Falling  = ((uint8_t)0x01)    /*!< Input Capture on Falling Edge*/
}TIM5_ICPolarity_TypeDef;

#define IS_TIM5_IC_POLARITY(POLARITY) \
  (((POLARITY) == TIM5_ICPolarity_Rising) || \
   ((POLARITY) == TIM5_ICPolarity_Falling))
/**
  * @}
  */
  
/** @defgroup TIM5_Input_Capture_Selection
  * @{
  */
typedef enum
{
  TIM5_ICSelection_DirectTI    = ((uint8_t)0x01),   /*!< Input Capture mapped on the direct input*/
  TIM5_ICSelection_IndirectTI  = ((uint8_t)0x02),   /*!< Input Capture mapped on the indirect input*/
  TIM5_ICSelection_TRGI        = ((uint8_t)0x03)    /*!< Input Capture mapped on the Trigger Input*/
}TIM5_ICSelection_TypeDef;

#define IS_TIM5_IC_SELECTION(SELECTION) \
  (((SELECTION) == TIM5_ICSelection_DirectTI)   || \
   ((SELECTION) == TIM5_ICSelection_IndirectTI) || \
   ((SELECTION) == TIM5_ICSelection_TRGI))
/**
  * @}
  */
  
/** @defgroup TIM5_Input_Capture_Prescaler
  * @{
  */
typedef enum
{
  TIM5_ICPSC_DIV1  = ((uint8_t)0x00),  /*!< Input Capture Prescaler = 1 (one capture every 1 event) */
  TIM5_ICPSC_DIV2  = ((uint8_t)0x04),  /*!< Input Capture Prescaler = 2 (one capture every 2 events) */
  TIM5_ICPSC_DIV4  = ((uint8_t)0x08),  /*!< Input Capture Prescaler = 4 (one capture every 4 events) */
  TIM5_ICPSC_DIV8  = ((uint8_t)0x0C)   /*!< Input Capture Prescaler = 8 (one capture every 8 events) */
}TIM5_ICPSC_TypeDef;

#define IS_TIM5_IC_PRESCALER(PRESCALER) \
  (((PRESCALER) == TIM5_ICPSC_DIV1) || \
   ((PRESCALER) == TIM5_ICPSC_DIV2) || \
   ((PRESCALER) == TIM5_ICPSC_DIV4) || \
   ((PRESCALER) == TIM5_ICPSC_DIV8))
/**
  * @}
  */
  
/** @defgroup TIM5_Interrupts
  * @{
  */
typedef enum
{
  TIM5_IT_Update   = ((uint8_t)0x01),   /*!< Update Interrupt*/
  TIM5_IT_CC1      = ((uint8_t)0x02),   /*!< Capture Compare Channel1 Interrupt*/
  TIM5_IT_CC2      = ((uint8_t)0x04),   /*!< Capture Compare Channel2 Interrupt*/
  TIM5_IT_Trigger  = ((uint8_t)0x40),   /*!< Trigger  Interrupt*/
  TIM5_IT_Break    = ((uint8_t)0x80)    /*!< Break Interrupt*/
}TIM5_IT_TypeDef;

#define IS_TIM5_IT(IT) \
  ((IT) != 0x00)

#define IS_TIM5_GET_IT(IT) \
  (((IT) == TIM5_IT_Update)  || \
   ((IT) == TIM5_IT_CC1)     || \
   ((IT) == TIM5_IT_CC2)     || \
   ((IT) == TIM5_IT_Trigger) || \
   ((IT) == TIM5_IT_Break))
/**
  * @}
  */
  
/** @defgroup TIM5_External_Trigger_Prescaler
  * @{
  */
typedef enum
{
  TIM5_ExtTRGPSC_OFF   = ((uint8_t)0x00),   /*!< No External Trigger prescaler  */
  TIM5_ExtTRGPSC_DIV2  = ((uint8_t)0x10),   /*!< External Trigger prescaler = 2 (ETRP frequency divided by 2) */
  TIM5_ExtTRGPSC_DIV4  = ((uint8_t)0x20),   /*!< External Trigger prescaler = 4 (ETRP frequency divided by 4) */
  TIM5_ExtTRGPSC_DIV8  = ((uint8_t)0x30)    /*!< External Trigger prescaler = 8 (ETRP frequency divided by 8) */
}TIM5_ExtTRGPSC_TypeDef;

#define IS_TIM5_EXT_PRESCALER(PRESCALER) \
  (((PRESCALER) == TIM5_ExtTRGPSC_OFF)  || \
   ((PRESCALER) == TIM5_ExtTRGPSC_DIV2) || \
   ((PRESCALER) == TIM5_ExtTRGPSC_DIV4) || \
   ((PRESCALER) == TIM5_ExtTRGPSC_DIV8))
/**
  * @}
  */
  
/** @defgroup TIM5_Internal_Trigger_Selection
  * @{
  */
typedef enum
{
  TIM5_TRGSelection_TIM4     = ((uint8_t)0x00),  /*!< TRIG Input source =  TIM TRIG Output  */
  TIM5_TRGSelection_TIM1     = ((uint8_t)0x10),  /*!< TRIG Input source =  TIM TRIG Output  */
  TIM5_TRGSelection_TIM3     = ((uint8_t)0x20),  /*!< TRIG Input source =  TIM TRIG Output  */
  TIM5_TRGSelection_TIM2     = ((uint8_t)0x30),  /*!< TRIG Input source =  TIM TRIG Output  */
  TIM5_TRGSelection_TI1F_ED  = ((uint8_t)0x40),  /*!< TRIG Input source = TI1F_ED (TI1 Edge Detector)  */
  TIM5_TRGSelection_TI1FP1   = ((uint8_t)0x50),  /*!< TRIG Input source = TI1FP1 (Filtered Timer Input 1) */
  TIM5_TRGSelection_TI2FP2   = ((uint8_t)0x60),  /*!< TRIG Input source = TI2FP2 (Filtered Timer Input 2)  */
  TIM5_TRGSelection_ETRF     = ((uint8_t)0x70)   /*!< TRIG Input source =  ETRF (External Trigger Input ) */
}TIM5_TRGSelection_TypeDef;

#define IS_TIM5_TRIGGER_SELECTION(SELECTION) \
  (((SELECTION) == TIM5_TRGSelection_TIM4) || \
   ((SELECTION) == TIM5_TRGSelection_TIM1) || \
   ((SELECTION) == TIM5_TRGSelection_TIM3) || \
   ((SELECTION) == TIM5_TRGSelection_TIM2) || \
   ((SELECTION) == TIM5_TRGSelection_TI1F_ED) || \
   ((SELECTION) == TIM5_TRGSelection_TI1FP1)  || \
   ((SELECTION) == TIM5_TRGSelection_TI2FP2)  || \
   ((SELECTION) == TIM5_TRGSelection_ETRF))


#define IS_TIM5_TIX_TRIGGER_SELECTION(SELECTION) \
  (((SELECTION) == TIM5_TRGSelection_TI1F_ED) || \
   ((SELECTION) == TIM5_TRGSelection_TI1FP1)  || \
   ((SELECTION) == TIM5_TRGSelection_TI2FP2))
/**
  * @}
  */
  
/** @defgroup TIM5_TI_External_Clock_Source
  * @{
  */
typedef enum
{
  TIM5_TIxExternalCLK1Source_TI1ED  = ((uint8_t)0x40),   /*!< External Clock mode 1 source = TI1ED */
  TIM5_TIxExternalCLK1Source_TI1    = ((uint8_t)0x50),   /*!< External Clock mode 1 source = TI1 */
  TIM5_TIxExternalCLK1Source_TI2    = ((uint8_t)0x60)    /*!< External Clock mode 1 source = TI2 */
}TIM5_TIxExternalCLK1Source_TypeDef;

#define IS_TIM5_TIXCLK_SOURCE(SOURCE)  \
  (((SOURCE) == TIM5_TIxExternalCLK1Source_TI1ED) || \
   ((SOURCE) == TIM5_TIxExternalCLK1Source_TI2)   || \
   ((SOURCE) == TIM5_TIxExternalCLK1Source_TI1))

/**
  * @}
  */
  
/** @defgroup TIM5_External_Trigger_Polarity
  * @{
  */
typedef enum
{
  TIM5_ExtTRGPolarity_Inverted     = ((uint8_t)0x80),   /*!< External Trigger Polarity = inverted */
  TIM5_ExtTRGPolarity_NonInverted  = ((uint8_t)0x00)    /*!< External Trigger Polarity = non inverted */
}TIM5_ExtTRGPolarity_TypeDef;

#define IS_TIM5_EXT_POLARITY(POLARITY) \
  (((POLARITY) == TIM5_ExtTRGPolarity_Inverted) || \
   ((POLARITY) == TIM5_ExtTRGPolarity_NonInverted))
/**
  * @}
  */
  
/** @defgroup TIM5_Prescaler_Reload_Mode
  * @{
  */
typedef enum
{
  TIM5_PSCReloadMode_Update      = ((uint8_t)0x00),   /*!< Prescaler value is reloaded at every update*/
  TIM5_PSCReloadMode_Immediate   = ((uint8_t)0x01)    /*!< Prescaler value is reloaded immediatly*/
}TIM5_PSCReloadMode_TypeDef;

#define IS_TIM5_PRESCALER_RELOAD(RELOAD) \
  (((RELOAD) == TIM5_PSCReloadMode_Update) || \
   ((RELOAD) == TIM5_PSCReloadMode_Immediate))
/**
  * @}
  */
  
/** @defgroup TIM5_Encoder_Mode
  * @{
  */
typedef enum
{
  TIM5_EncoderMode_TI1     = ((uint8_t)0x01),   /*!< Encoder mode 1*/
  TIM5_EncoderMode_TI2     = ((uint8_t)0x02),   /*!< Encoder mode 2*/
  TIM5_EncoderMode_TI12    = ((uint8_t)0x03)    /*!< Encoder mode 3*/
}TIM5_EncoderMode_TypeDef;

#define IS_TIM5_ENCODER_MODE(MODE) \
  (((MODE) == TIM5_EncoderMode_TI1) || \
   ((MODE) == TIM5_EncoderMode_TI2) || \
   ((MODE) == TIM5_EncoderMode_TI12))
/**
  * @}
  */
  
/** @defgroup TIM5_Event_Source
  * @{
  */
typedef enum
{
  TIM5_EventSource_Update   = ((uint8_t)0x01),   /*!< Update Event*/
  TIM5_EventSource_CC1      = ((uint8_t)0x02),   /*!< Capture Compare Channel1  Event*/
  TIM5_EventSource_CC2      = ((uint8_t)0x04),   /*!< Capture Compare Channel2 Event*/
  TIM5_EventSource_Trigger  = ((uint8_t)0x40),   /*!< Trigger Event*/
  TIM5_EventSource_Break    = ((uint8_t)0x80)    /*!< Break Event*/
}TIM5_EventSource_TypeDef;

#define IS_TIM5_EVENT_SOURCE(SOURCE) ((((SOURCE) & (uint8_t)0x18) == 0x00) && \
                                      ((SOURCE) != 0x00))
/**
  * @}
  */
  
/** @defgroup TIM5_Update_Source
  * @{
  */
typedef enum
{
  TIM5_UpdateSource_Global   = ((uint8_t)0x00),   /*!< Global Update request source */
  TIM5_UpdateSource_Regular  = ((uint8_t)0x01)    /*!< Regular Update request source */
}TIM5_UpdateSource_TypeDef;

#define IS_TIM5_UPDATE_SOURCE(SOURCE) \
  (((SOURCE) == TIM5_UpdateSource_Global) || \
   ((SOURCE) == TIM5_UpdateSource_Regular))

/**
  * @}
  */
  
/** @defgroup TIM5_Trigger_Output_Source
  * @{
  */
typedef enum
{
  TIM5_TRGOSource_Reset    = ((uint8_t)0x00),   /*!< Trigger Output source = Reset*/
  TIM5_TRGOSource_Enable   = ((uint8_t)0x10),   /*!< Trigger Output source = TIM5 is enabled*/
  TIM5_TRGOSource_Update   = ((uint8_t)0x20),   /*!< Trigger Output source = Update event*/
  TIM5_TRGOSource_OC1      = ((uint8_t)0x30),   /*!< Trigger Output source = output compare channel1  */
  TIM5_TRGOSource_OC1REF   = ((uint8_t)0x40),   /*!< Trigger Output source = output compare channel 1 reference */
  TIM5_TRGOSource_OC2REF   = ((uint8_t)0x50)    /*!< Trigger Output source = output compare channel 2 reference */
}TIM5_TRGOSource_TypeDef;

#define IS_TIM5_TRGO_SOURCE(SOURCE) \
  (((SOURCE) == TIM5_TRGOSource_Reset)  || \
   ((SOURCE) == TIM5_TRGOSource_Enable) || \
   ((SOURCE) == TIM5_TRGOSource_Update) || \
   ((SOURCE) == TIM5_TRGOSource_OC1)    || \
   ((SOURCE) == TIM5_TRGOSource_OC1REF) || \
   ((SOURCE) == TIM5_TRGOSource_OC2REF))
/**
  * @}
  */
  
/** @defgroup TIM5_Slave_Mode
  * @{
  */
typedef enum
{
  TIM5_SlaveMode_Reset      = ((uint8_t)0x04),  /*!< Slave Mode Selection  = Reset*/
  TIM5_SlaveMode_Gated      = ((uint8_t)0x05),  /*!< Slave Mode Selection  = Gated*/
  TIM5_SlaveMode_Trigger    = ((uint8_t)0x06),  /*!< Slave Mode Selection  = Trigger*/
  TIM5_SlaveMode_External1  = ((uint8_t)0x07)   /*!< Slave Mode Selection  = External 1*/
}TIM5_SlaveMode_TypeDef;

#define IS_TIM5_SLAVE_MODE(MODE) \
  (((MODE) == TIM5_SlaveMode_Reset)   || \
   ((MODE) == TIM5_SlaveMode_Gated)   || \
   ((MODE) == TIM5_SlaveMode_Trigger) || \
   ((MODE) == TIM5_SlaveMode_External1))
/**
  * @}
  */
  
/** @defgroup TIM5_Flags
  * @{
  */
typedef enum
{
  TIM5_FLAG_Update   = ((uint16_t)0x0001),  /*!< Update Flag */
  TIM5_FLAG_CC1      = ((uint16_t)0x0002),  /*!< Capture compare 1 Flag */
  TIM5_FLAG_CC2      = ((uint16_t)0x0004),  /*!< Capture compare 2 Flag */
  TIM5_FLAG_Trigger  = ((uint16_t)0x0040),  /*!< Trigger Flag */
  TIM5_FLAG_Break    = ((uint16_t)0x0080),  /*!< Break Flag */
  TIM5_FLAG_CC1OF    = ((uint16_t)0x0200),  /*!< Capture compare 1 over capture Flag */
  TIM5_FLAG_CC2OF    = ((uint16_t)0x0400)   /*!< Capture compare 2 over capture Flag */
}TIM5_FLAG_TypeDef;

#define IS_TIM5_GET_FLAG(FLAG) \
  (((FLAG) == TIM5_FLAG_Update)  || \
   ((FLAG) == TIM5_FLAG_CC1)     || \
   ((FLAG) == TIM5_FLAG_CC2)     || \
   ((FLAG) == TIM5_FLAG_Trigger) || \
   ((FLAG) == TIM5_FLAG_Break)   || \
   ((FLAG) == TIM5_FLAG_CC1OF)   || \
   ((FLAG) == TIM5_FLAG_CC2OF))

#define IS_TIM5_CLEAR_FLAG(FLAG) \
  ((((FLAG) & (uint16_t)0xE100) == 0x0000) && ((FLAG) != 0x0000))
/**
  * @}
  */
  
/** @defgroup TIM5_DMA_Source_Requests
  * @{
  */
typedef enum
{
  TIM5_DMASource_Update   = ((uint8_t)0x01),  /*!< TIM5 DMA Update Request*/
  TIM5_DMASource_CC1      = ((uint8_t)0x02),  /*!< TIM5 DMA CC1 Request*/
  TIM5_DMASource_CC2      = ((uint8_t)0x04)   /*!< TIM5 DMA CC2 Request*/
}TIM5_DMASource_TypeDef;

#define IS_TIM5_DMA_SOURCE(SOURCE) \
  (((SOURCE) == TIM5_DMASource_Update) || \
   ((SOURCE) == TIM5_DMASource_CC1) || \
   ((SOURCE) == TIM5_DMASource_CC2))
/**
  * @}
  */
  
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/** @defgroup TIM5_Exported_Macros
  * @{
  */

/**
  * @brief Macro TIM5 Input Capture Filter Value
  */
#define IS_TIM5_IC_FILTER(ICFILTER) ((ICFILTER) <= 0x0F)

/**
  * @brief Macro TIM5 External Trigger Filter
  */
#define IS_TIM5_EXT_FILTER(EXTFILTER) \
  ((EXTFILTER) <= 0x0F)

/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */

/* TimeBase management ********************************************************/
void TIM5_DeInit(void);
void TIM5_TimeBaseInit(TIM5_Prescaler_TypeDef TIM5_Prescaler,
                       TIM5_CounterMode_TypeDef TIM5_CounterMode, uint16_t TIM5_Period);
void TIM5_PrescalerConfig(TIM5_Prescaler_TypeDef Prescaler,
                          TIM5_PSCReloadMode_TypeDef TIM5_PSCReloadMode);
void TIM5_CounterModeConfig(TIM5_CounterMode_TypeDef TIM5_CounterMode);
void TIM5_SetCounter(uint16_t Counter);
void TIM5_SetAutoreload(uint16_t Autoreload);
uint16_t TIM5_GetCounter(void);
TIM5_Prescaler_TypeDef TIM5_GetPrescaler(void);
void TIM5_UpdateDisableConfig(FunctionalState NewState);
void TIM5_UpdateRequestConfig(TIM5_UpdateSource_TypeDef TIM5_UpdateSource);
void TIM5_ARRPreloadConfig(FunctionalState NewState);
void TIM5_SelectOnePulseMode(TIM5_OPMode_TypeDef TIM5_OPMode);
void TIM5_Cmd(FunctionalState NewState);

/* Output Compare management **************************************************/
void TIM5_OC1Init(TIM5_OCMode_TypeDef TIM5_OCMode,
                  TIM5_OutputState_TypeDef TIM5_OutputState,
                  uint16_t TIM5_Pulse,
                  TIM5_OCPolarity_TypeDef TIM5_OCPolarity,
                  TIM5_OCIdleState_TypeDef TIM5_OCIdleState);
void TIM5_OC2Init(TIM5_OCMode_TypeDef TIM5_OCMode,
                  TIM5_OutputState_TypeDef TIM5_OutputState,
                  uint16_t TIM5_Pulse,
                  TIM5_OCPolarity_TypeDef TIM5_OCPolarity,
                  TIM5_OCIdleState_TypeDef TIM5_OCIdleState);
void TIM5_BKRConfig(TIM5_OSSIState_TypeDef TIM5_OSSIState,
                    TIM5_LockLevel_TypeDef TIM5_LockLevel,
                    TIM5_BreakState_TypeDef TIM5_BreakState,
                    TIM5_BreakPolarity_TypeDef TIM5_BreakPolarity,
                    TIM5_AutomaticOutput_TypeDef TIM5_AutomaticOutput);
void TIM5_CtrlPWMOutputs(FunctionalState NewState);
void TIM5_SelectOCxM(TIM5_Channel_TypeDef TIM5_Channel, TIM5_OCMode_TypeDef TIM5_OCMode);
void TIM5_SetCompare1(uint16_t Compare);
void TIM5_SetCompare2(uint16_t Compare);
void TIM5_ForcedOC1Config(TIM5_ForcedAction_TypeDef TIM5_ForcedAction);
void TIM5_ForcedOC2Config(TIM5_ForcedAction_TypeDef TIM5_ForcedAction);
void TIM5_OC1PreloadConfig(FunctionalState NewState);
void TIM5_OC2PreloadConfig(FunctionalState NewState);
void TIM5_OC1FastConfig(FunctionalState NewState);
void TIM5_OC2FastConfig(FunctionalState NewState);
void TIM5_OC1PolarityConfig(TIM5_OCPolarity_TypeDef TIM5_OCPolarity);
void TIM5_OC2PolarityConfig(TIM5_OCPolarity_TypeDef TIM5_OCPolarity);
void TIM5_CCxCmd(TIM5_Channel_TypeDef TIM5_Channel, FunctionalState NewState);

/* Input Capture management ***************************************************/
void TIM5_ICInit(TIM5_Channel_TypeDef TIM5_Channel,
                 TIM5_ICPolarity_TypeDef TIM5_ICPolarity,
                 TIM5_ICSelection_TypeDef TIM5_ICSelection,
                 TIM5_ICPSC_TypeDef TIM5_ICPrescaler,
                 uint8_t TIM5_ICFilter);
void TIM5_PWMIConfig(TIM5_Channel_TypeDef TIM5_Channel,
                     TIM5_ICPolarity_TypeDef TIM5_ICPolarity,
                     TIM5_ICSelection_TypeDef TIM5_ICSelection,
                     TIM5_ICPSC_TypeDef TIM5_ICPrescaler,
                     uint8_t TIM5_ICFilter);
uint16_t TIM5_GetCapture1(void);
uint16_t TIM5_GetCapture2(void);
void TIM5_SetIC1Prescaler(TIM5_ICPSC_TypeDef TIM5_IC1Prescaler);
void TIM5_SetIC2Prescaler(TIM5_ICPSC_TypeDef TIM5_IC2Prescaler);

/* Interrupts, DMA and flags management ***************************************/
void TIM5_ITConfig(TIM5_IT_TypeDef TIM5_IT, FunctionalState NewState);
void TIM5_GenerateEvent(TIM5_EventSource_TypeDef TIM5_EventSource);
FlagStatus TIM5_GetFlagStatus(TIM5_FLAG_TypeDef TIM5_FLAG);
void TIM5_ClearFlag(TIM5_FLAG_TypeDef TIM5_FLAG);
ITStatus TIM5_GetITStatus(TIM5_IT_TypeDef TIM5_IT);
void TIM5_ClearITPendingBit(TIM5_IT_TypeDef TIM5_IT);
void TIM5_DMACmd(TIM5_DMASource_TypeDef TIM5_DMASource, FunctionalState NewState);
void TIM5_SelectCCDMA(FunctionalState NewState);

/* Clocks management **********************************************************/
void TIM5_InternalClockConfig(void);
void TIM5_TIxExternalClockConfig(TIM5_TIxExternalCLK1Source_TypeDef TIM5_TIxExternalCLKSource,
                                 TIM5_ICPolarity_TypeDef TIM5_ICPolarity,
                                 uint8_t ICFilter);
void TIM5_ETRClockMode1Config(TIM5_ExtTRGPSC_TypeDef TIM5_ExtTRGPrescaler,
                              TIM5_ExtTRGPolarity_TypeDef TIM5_ExtTRGPolarity,
                              uint8_t ExtTRGFilter);
void TIM5_ETRClockMode2Config(TIM5_ExtTRGPSC_TypeDef TIM5_ExtTRGPrescaler,
                              TIM5_ExtTRGPolarity_TypeDef TIM5_ExtTRGPolarity,
                              uint8_t ExtTRGFilter);

/* Synchronization management *************************************************/
void TIM5_SelectInputTrigger(TIM5_TRGSelection_TypeDef TIM5_InputTriggerSource);
void TIM5_SelectOutputTrigger(TIM5_TRGOSource_TypeDef TIM5_TRGOSource);
void TIM5_SelectSlaveMode(TIM5_SlaveMode_TypeDef TIM5_SlaveMode);
void TIM5_SelectMasterSlaveMode(FunctionalState NewState);
void TIM5_ETRConfig(TIM5_ExtTRGPSC_TypeDef TIM5_ExtTRGPrescaler,
                    TIM5_ExtTRGPolarity_TypeDef TIM5_ExtTRGPolarity,
                    uint8_t ExtTRGFilter);

/* Specific interface management **********************************************/
void TIM5_EncoderInterfaceConfig(TIM5_EncoderMode_TypeDef TIM5_EncoderMode,
                                 TIM5_ICPolarity_TypeDef TIM5_IC1Polarity,
                                 TIM5_ICPolarity_TypeDef TIM5_IC2Polarity);
void TIM5_SelectHallSensor(FunctionalState NewState);

#endif /* __STM8L15x_TIM5_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
