/**
  ******************************************************************************
  * @file    stm8l15x_tim1.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the TIM1 firmware
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
#ifndef __STM8L15x_TIM1_H
#define __STM8L15x_TIM1_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @addtogroup TIM1
  * @{
  */ 
  
/* Exported types ------------------------------------------------------------*/  
/** @defgroup TIM1_Exported_Types
  * @{
  */

/** @defgroup TIM1_Output_Compare_Mode
  * @{
  */
typedef enum
{
  TIM1_OCMode_Timing     = ((uint8_t)0x00),
  TIM1_OCMode_Active     = ((uint8_t)0x10),
  TIM1_OCMode_Inactive   = ((uint8_t)0x20),
  TIM1_OCMode_Toggle     = ((uint8_t)0x30),
  TIM1_OCMode_PWM1       = ((uint8_t)0x60),
  TIM1_OCMode_PWM2       = ((uint8_t)0x70)
}TIM1_OCMode_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_One_Pulse_Mode
  * @{
  */
typedef enum
{
  TIM1_OPMode_Single                 = ((uint8_t)0x01),
  TIM1_OPMode_Repetitive             = ((uint8_t)0x00)
}TIM1_OPMode_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Channels
  * @{
  */
typedef enum
{
  TIM1_Channel_1                     = ((uint8_t)0x00),
  TIM1_Channel_2                     = ((uint8_t)0x01),
  TIM1_Channel_3                     = ((uint8_t)0x02),
  TIM1_Channel_4                     = ((uint8_t)0x03)
}TIM1_Channel_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Counter_Mode
  * @{
  */
typedef enum
{
  TIM1_CounterMode_Up                = ((uint8_t)0x00),
  TIM1_CounterMode_Down              = ((uint8_t)0x10),
  TIM1_CounterMode_CenterAligned1    = ((uint8_t)0x20),
  TIM1_CounterMode_CenterAligned2    = ((uint8_t)0x40),
  TIM1_CounterMode_CenterAligned3    = ((uint8_t)0x60)
}TIM1_CounterMode_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Output_Compare_Polarity
  * @{
  */
typedef enum
{
  TIM1_OCPolarity_High               = ((uint8_t)0x00),
  TIM1_OCPolarity_Low                = ((uint8_t)0x22)
}TIM1_OCPolarity_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Output_Compare_N_Polarity
  * @{
  */
typedef enum
{
  TIM1_OCNPolarity_High              = ((uint8_t)0x00),
  TIM1_OCNPolarity_Low               = ((uint8_t)0x88)
}TIM1_OCNPolarity_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Output_State
  * @{
  */
typedef enum
{
  TIM1_OutputState_Disable           = ((uint8_t)0x00),
  TIM1_OutputState_Enable            = ((uint8_t)0x11)
}TIM1_OutputState_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Output_N_State
  * @{
  */
typedef enum
{
  TIM1_OutputNState_Disable = ((uint8_t)0x00),
  TIM1_OutputNState_Enable  = ((uint8_t)0x44)
} TIM1_OutputNState_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Break_State
  * @{
  */
typedef enum
{
  TIM1_BreakState_Enable             = ((uint8_t)0x10),
  TIM1_BreakState_Disable            = ((uint8_t)0x00)
}TIM1_BreakState_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Break_Polarity
  * @{
  */
typedef enum
{
  TIM1_BreakPolarity_Low             = ((uint8_t)0x00),
  TIM1_BreakPolarity_High            = ((uint8_t)0x20)
}TIM1_BreakPolarity_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Automatic_Output
  * @{
  */
typedef enum
{
  TIM1_AutomaticOutput_Enable        = ((uint8_t)0x40),
  TIM1_AutomaticOutput_Disable       = ((uint8_t)0x00)
}TIM1_AutomaticOutput_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Lock_Level
  * @{
  */
typedef enum
{
  TIM1_LockLevel_Off                 = ((uint8_t)0x00),
  TIM1_LockLevel_1                   = ((uint8_t)0x01),
  TIM1_LockLevel_2                   = ((uint8_t)0x02),
  TIM1_LockLevel_3                   = ((uint8_t)0x03)
}TIM1_LockLevel_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_OSSI_State
  * @{
  */
typedef enum
{
  TIM1_OSSIState_Enable              = ((uint8_t)0x04),
  TIM1_OSSIState_Disable             = ((uint8_t)0x00)
}TIM1_OSSIState_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Output_Compare_Idle_state
  * @{
  */
typedef enum
{
  TIM1_OCIdleState_Set               = ((uint8_t)0x55),
  TIM1_OCIdleState_Reset             = ((uint8_t)0x00)
}TIM1_OCIdleState_TypeDef;

/**
  * @}
  */

/** @defgroup TIM1_Output_Compare_N_Idle_state
  * @{
  */
typedef enum
{
  TIM1_OCNIdleState_Set             = ((uint8_t)0x2A),
  TIM1_OCNIdleState_Reset           = ((uint8_t)0x00)
}TIM1_OCNIdleState_TypeDef;

/**
  * @}
  */

/** @defgroup TIM1_Input_Capture_Polarity
  * @{
  */
typedef enum
{
  TIM1_ICPolarity_Rising            = ((uint8_t)0x00),
  TIM1_ICPolarity_Falling           = ((uint8_t)0x01)
}TIM1_ICPolarity_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Input_Capture_Selection
  * @{
  */
typedef enum
{
  TIM1_ICSelection_DirectTI          = ((uint8_t)0x01),
  TIM1_ICSelection_IndirectTI        = ((uint8_t)0x02),
  TIM1_ICSelection_TRGI              = ((uint8_t)0x03)
}TIM1_ICSelection_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Input_Capture_Prescaler
  * @{
  */
typedef enum
{
  TIM1_ICPSC_DIV1                    = ((uint8_t)0x00),
  TIM1_ICPSC_DIV2                    = ((uint8_t)0x04),
  TIM1_ICPSC_DIV4                    = ((uint8_t)0x08),
  TIM1_ICPSC_DIV8                    = ((uint8_t)0x0C)
}TIM1_ICPSC_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Output_Compare_Reference_Clear
  * @{
  */
typedef enum
{
  TIM1_OCReferenceClear_ETRF       = ((uint8_t)0x08),
  TIM1_OCReferenceClear_OCREFCLR   = ((uint8_t)0x00)
}
TIM1_OCReferenceClear_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Interrupts
  * @{
  */
typedef enum
{
  TIM1_IT_Update                     = ((uint8_t)0x01),
  TIM1_IT_CC1                        = ((uint8_t)0x02),
  TIM1_IT_CC2                        = ((uint8_t)0x04),
  TIM1_IT_CC3                        = ((uint8_t)0x08),
  TIM1_IT_CC4                        = ((uint8_t)0x10),
  TIM1_IT_COM                        = ((uint8_t)0x20),
  TIM1_IT_Trigger                    = ((uint8_t)0x40),
  TIM1_IT_Break                      = ((uint8_t)0x80)
}TIM1_IT_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_External_Trigger_Prescaler
  * @{
  */
typedef enum
{
  TIM1_ExtTRGPSC_OFF                 = ((uint8_t)0x00),
  TIM1_ExtTRGPSC_DIV2                = ((uint8_t)0x10),
  TIM1_ExtTRGPSC_DIV4                = ((uint8_t)0x20),
  TIM1_ExtTRGPSC_DIV8                = ((uint8_t)0x30)
}TIM1_ExtTRGPSC_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Internal_Trigger_Selection
  * @{
  */
typedef enum
{
  TIM1_TRGSelection_TIM4             = ((uint8_t)0x00),  /*!< TRIG Input source =  TIM TRIG Output  */
  TIM1_TRGSelection_TIM5             = ((uint8_t)0x10),  /*!< TRIG Input source =  TIM TRIG Output  */
  TIM1_TRGSelection_TIM3             = ((uint8_t)0x20),  /*!< TRIG Input source =  TIM TRIG Output  */
  TIM1_TRGSelection_TIM2             = ((uint8_t)0x30),  /*!< TRIG Input source =  TIM TRIG Output  */
  TIM1_TRGSelection_TI1F_ED          = ((uint8_t)0x40),
  TIM1_TRGSelection_TI1FP1           = ((uint8_t)0x50),
  TIM1_TRGSelection_TI2FP2           = ((uint8_t)0x60),
  TIM1_TRGSelection_ETRF             = ((uint8_t)0x70)
}TIM1_TRGSelection_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_TI_External_Clock_Source
  * @{
  */
typedef enum
{
  TIM1_TIxExternalCLK1Source_TI1ED   = ((uint8_t)0x40),
  TIM1_TIxExternalCLK1Source_TI1     = ((uint8_t)0x50),
  TIM1_TIxExternalCLK1Source_TI2     = ((uint8_t)0x60)
}TIM1_TIxExternalCLK1Source_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_External_Trigger_Polarity
  * @{
  */
typedef enum
{
  TIM1_ExtTRGPolarity_Inverted       = ((uint8_t)0x80),
  TIM1_ExtTRGPolarity_NonInverted    = ((uint8_t)0x00)
}TIM1_ExtTRGPolarity_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Prescaler_Reload_Mode
  * @{
  */
typedef enum
{
  TIM1_PSCReloadMode_Update          = ((uint8_t)0x00),
  TIM1_PSCReloadMode_Immediate       = ((uint8_t)0x01)
}TIM1_PSCReloadMode_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Encoder_Mode
  * @{
  */
typedef enum
{
  TIM1_EncoderMode_TI1               = ((uint8_t)0x01),
  TIM1_EncoderMode_TI2               = ((uint8_t)0x02),
  TIM1_EncoderMode_TI12              = ((uint8_t)0x03)
}TIM1_EncoderMode_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Event_Source
  * @{
  */
typedef enum
{
  TIM1_EventSource_Update            = ((uint8_t)0x01),
  TIM1_EventSource_CC1               = ((uint8_t)0x02),
  TIM1_EventSource_CC2               = ((uint8_t)0x04),
  TIM1_EventSource_CC3               = ((uint8_t)0x08),
  TIM1_EventSource_CC4               = ((uint8_t)0x10),
  TIM1_EventSource_COM               = ((uint8_t)0x20),
  TIM1_EventSource_Trigger           = ((uint8_t)0x40),
  TIM1_EventSource_Break             = ((uint8_t)0x80)
}TIM1_EventSource_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Update_Source
  * @{
  */
typedef enum
{
  TIM1_UpdateSource_Global           = ((uint8_t)0x00),
  TIM1_UpdateSource_Regular          = ((uint8_t)0x01)
}TIM1_UpdateSource_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Trigger_Output_Source
  * @{
  */
typedef enum
{
  TIM1_TRGOSource_Reset              = ((uint8_t)0x00),
  TIM1_TRGOSource_Enable             = ((uint8_t)0x10),
  TIM1_TRGOSource_Update             = ((uint8_t)0x20),
  TIM1_TRGOSource_OC1                = ((uint8_t)0x30),
  TIM1_TRGOSource_OC1REF             = ((uint8_t)0x40),
  TIM1_TRGOSource_OC2REF             = ((uint8_t)0x50),
  TIM1_TRGOSource_OC3REF             = ((uint8_t)0x60),
  TIM1_TRGOSource_OC4REF             = ((uint8_t)0x70)
}TIM1_TRGOSource_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Slave_Mode
  * @{
  */
typedef enum
{
  TIM1_SlaveMode_Reset               = ((uint8_t)0x04),
  TIM1_SlaveMode_Gated               = ((uint8_t)0x05),
  TIM1_SlaveMode_Trigger             = ((uint8_t)0x06),
  TIM1_SlaveMode_External1           = ((uint8_t)0x07)
}TIM1_SlaveMode_TypeDef;

/**
  * @}
  */

/** @defgroup TIM1_Flags
  * @{
  */
typedef enum
{
  TIM1_FLAG_Update                   = ((uint16_t)0x0001),
  TIM1_FLAG_CC1                      = ((uint16_t)0x0002),
  TIM1_FLAG_CC2                      = ((uint16_t)0x0004),
  TIM1_FLAG_CC3                      = ((uint16_t)0x0008),
  TIM1_FLAG_CC4                      = ((uint16_t)0x0010),
  TIM1_FLAG_COM                      = ((uint16_t)0x0020),
  TIM1_FLAG_Trigger                  = ((uint16_t)0x0040),
  TIM1_FLAG_Break                    = ((uint16_t)0x0080),
  TIM1_FLAG_CC1OF                    = ((uint16_t)0x0200),
  TIM1_FLAG_CC2OF                    = ((uint16_t)0x0400),
  TIM1_FLAG_CC3OF                    = ((uint16_t)0x0800),
  TIM1_FLAG_CC4OF                    = ((uint16_t)0x1000)
}TIM1_FLAG_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_Forced_Action
  * @{
  */
typedef enum
{
  TIM1_ForcedAction_Active           = ((uint8_t)0x50),
  TIM1_ForcedAction_Inactive         = ((uint8_t)0x40)
}TIM1_ForcedAction_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_DMA_Source_Requests
  * @{
  */
typedef enum
{
  TIM1_DMASource_Update     = ((uint8_t)0x01),  /*!< TIM1 DMA Update Request*/
  TIM1_DMASource_CC1        = ((uint8_t)0x02),  /*!< TIM1 DMA CC1 Request*/
  TIM1_DMASource_CC2        = ((uint8_t)0x04),  /*!< TIM1 DMA CC2 Request*/
  TIM1_DMASource_CC3        = ((uint8_t)0x08),  /*!< TIM1 DMA CC3 Request*/
  TIM1_DMASource_CC4        = ((uint8_t)0x10),  /*!< TIM1 DMA CC4 Request*/
  TIM1_DMASource_COM        = ((uint8_t)0x20)  /*!< TIM1 DMA COM Req */
} TIM1_DMASource_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_DMA_Base_Address
  * @{
  */
typedef enum
{
  TIM1_DMABase_CR1      = ((uint8_t)0x00),
  TIM1_DMABase_CR2      = ((uint8_t)0x01),
  TIM1_DMABase_SMCR     = ((uint8_t)0x02),
  TIM1_DMABase_ETR      = ((uint8_t)0x03),
  TIM1_DMABase_DER      = ((uint8_t)0x04),
  TIM1_DMABase_IER      = ((uint8_t)0x05),
  TIM1_DMABase_SR1      = ((uint8_t)0x06),
  TIM1_DMABase_SR2      = ((uint8_t)0x07),
  TIM1_DMABase_EGR      = ((uint8_t)0x08),
  TIM1_DMABase_CCMR1    = ((uint8_t)0x09),
  TIM1_DMABase_CCMR2    = ((uint8_t)0x0A),
  TIM1_DMABase_CCMR3    = ((uint8_t)0x0B),
  TIM1_DMABase_CCMR4    = ((uint8_t)0x0C),
  TIM1_DMABase_CCER1    = ((uint8_t)0x0D),
  TIM1_DMABase_CCER2    = ((uint8_t)0x0E),
  TIM1_DMABase_CNTH     = ((uint8_t)0x0F),
  TIM1_DMABase_CNTL     = ((uint8_t)0x10),
  TIM1_DMABase_PSCH     = ((uint8_t)0x11),
  TIM1_DMABase_PSCL     = ((uint8_t)0x12),
  TIM1_DMABase_ARRH     = ((uint8_t)0x13),
  TIM1_DMABase_ARRL     = ((uint8_t)0x14),
  TIM1_DMABase_RCR      = ((uint8_t)0x15),
  TIM1_DMABase_CCR1H    = ((uint8_t)0x16),
  TIM1_DMABase_CCR1L    = ((uint8_t)0x17),
  TIM1_DMABase_CCR2H    = ((uint8_t)0x18),
  TIM1_DMABase_CCR2L    = ((uint8_t)0x19),
  TIM1_DMABase_CCR3H    = ((uint8_t)0x1A),
  TIM1_DMABase_CCR3L    = ((uint8_t)0x1B),
  TIM1_DMABase_CCR4H    = ((uint8_t)0x1C),
  TIM1_DMABase_CCR4L    = ((uint8_t)0x1D),
  TIM1_DMABase_BKR      = ((uint8_t)0x1E),
  TIM1_DMABase_DTR      = ((uint8_t)0x1F)

} TIM1_DMABase_TypeDef;

/**
  * @}
  */
  
/** @defgroup TIM1_DMA_Burst_Length
  * @{
  */
typedef enum
{
  TIM1_DMABurstLength_1Byte      = ((uint8_t)0x00),
  TIM1_DMABurstLength_2Byte      = ((uint8_t)0x01),
  TIM1_DMABurstLength_3Byte      = ((uint8_t)0x02),
  TIM1_DMABurstLength_4Byte      = ((uint8_t)0x03),
  TIM1_DMABurstLength_5Byte      = ((uint8_t)0x04),
  TIM1_DMABurstLength_6Byte      = ((uint8_t)0x05),
  TIM1_DMABurstLength_7Byte      = ((uint8_t)0x06),
  TIM1_DMABurstLength_8Byte      = ((uint8_t)0x07),
  TIM1_DMABurstLength_9Byte      = ((uint8_t)0x08),
  TIM1_DMABurstLength_10Byte     = ((uint8_t)0x09),
  TIM1_DMABurstLength_11Byte     = ((uint8_t)0x0A),
  TIM1_DMABurstLength_12Byte     = ((uint8_t)0x0B),
  TIM1_DMABurstLength_13Byte     = ((uint8_t)0x0C),
  TIM1_DMABurstLength_14Byte     = ((uint8_t)0x0D),
  TIM1_DMABurstLength_15Byte     = ((uint8_t)0x0E),
  TIM1_DMABurstLength_16Byte     = ((uint8_t)0x0F),
  TIM1_DMABurstLength_17Byte     = ((uint8_t)0x10),
  TIM1_DMABurstLength_18Byte     = ((uint8_t)0x11),
  TIM1_DMABurstLength_19Byte     = ((uint8_t)0x12),
  TIM1_DMABurstLength_20Byte     = ((uint8_t)0x13),
  TIM1_DMABurstLength_21Byte     = ((uint8_t)0x14),
  TIM1_DMABurstLength_22Byte     = ((uint8_t)0x15),
  TIM1_DMABurstLength_23Byte     = ((uint8_t)0x16),
  TIM1_DMABurstLength_24Byte     = ((uint8_t)0x17),
  TIM1_DMABurstLength_25Byte     = ((uint8_t)0x18),
  TIM1_DMABurstLength_26Byte     = ((uint8_t)0x19),
  TIM1_DMABurstLength_27Byte     = ((uint8_t)0x1A),
  TIM1_DMABurstLength_28Byte     = ((uint8_t)0x1B),
  TIM1_DMABurstLength_29Byte     = ((uint8_t)0x1C),
  TIM1_DMABurstLength_30Byte     = ((uint8_t)0x1D),
  TIM1_DMABurstLength_31Byte     = ((uint8_t)0x1E),
  TIM1_DMABurstLength_32Byte     = ((uint8_t)0x1F)

} TIM1_DMABurstLength_TypeDef;

/**
  * @}
  */
  
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/** @defgroup TIM1_Exported_Macros
  * @{
  */
#define IS_TIM1_OC_MODE(MODE) (((MODE) ==  TIM1_OCMode_Timing) || \
                               ((MODE) == TIM1_OCMode_Active) || \
                               ((MODE) == TIM1_OCMode_Inactive) || \
                               ((MODE) == TIM1_OCMode_Toggle)|| \
                               ((MODE) == TIM1_OCMode_PWM1) || \
                               ((MODE) == TIM1_OCMode_PWM2))

#define IS_TIM1_OCM(MODE)(((MODE) ==  TIM1_OCMode_Timing) || \
                          ((MODE) == TIM1_OCMode_Active) || \
                          ((MODE) == TIM1_OCMode_Inactive) || \
                          ((MODE) == TIM1_OCMode_Toggle)|| \
                          ((MODE) == TIM1_OCMode_PWM1) || \
                          ((MODE) == TIM1_OCMode_PWM2) || \
                          ((MODE) == (uint8_t)TIM1_ForcedAction_Active) || \
                          ((MODE) == (uint8_t)TIM1_ForcedAction_Inactive))

#define IS_TIM1_OPM_MODE(MODE) (((MODE) == TIM1_OPMode_Single) || \
                                ((MODE) == TIM1_OPMode_Repetitive))

#define IS_TIM1_CHANNEL(CHANNEL) (((CHANNEL) == TIM1_Channel_1) || \
                                  ((CHANNEL) == TIM1_Channel_2) || \
                                  ((CHANNEL) == TIM1_Channel_3) || \
                                  ((CHANNEL) == TIM1_Channel_4))

#define IS_TIM1_PWMI_CHANNEL(CHANNEL) (((CHANNEL) == TIM1_Channel_1) || \
                                       ((CHANNEL) == TIM1_Channel_2))

#define IS_TIM1_COMPLEMENTARY_CHANNEL(CHANNEL) (((CHANNEL) == TIM1_Channel_1) || \
    ((CHANNEL) == TIM1_Channel_2) || \
    ((CHANNEL) == TIM1_Channel_3))

#define IS_TIM1_COUNTER_MODE(MODE) (((MODE) == TIM1_CounterMode_Up) || \
                                    ((MODE) == TIM1_CounterMode_Down) || \
                                    ((MODE) == TIM1_CounterMode_CenterAligned1) || \
                                    ((MODE) == TIM1_CounterMode_CenterAligned2) || \
                                    ((MODE) == TIM1_CounterMode_CenterAligned3))

#define IS_TIM1_OC_POLARITY(POLARITY) (((POLARITY) == TIM1_OCPolarity_High) || \
                                       ((POLARITY) == TIM1_OCPolarity_Low))

#define IS_TIM1_OCN_POLARITY(POLARITY) (((POLARITY) == TIM1_OCNPolarity_High) || \
                                        ((POLARITY) == TIM1_OCNPolarity_Low))

#define IS_TIM1_OUTPUT_STATE(STATE) (((STATE) == TIM1_OutputState_Disable) || \
                                     ((STATE) == TIM1_OutputState_Enable))

#define IS_TIM1_OUTPUTN_STATE(STATE) (((STATE) == TIM1_OutputNState_Disable) ||\
                                      ((STATE) == TIM1_OutputNState_Enable))

#define IS_TIM1_BREAK_STATE(STATE) (((STATE) == TIM1_BreakState_Enable) || \
                                    ((STATE) == TIM1_BreakState_Disable))

#define IS_TIM1_BREAK_POLARITY(POLARITY) (((POLARITY) == TIM1_BreakPolarity_Low) || \
    ((POLARITY) == TIM1_BreakPolarity_High))

#define IS_TIM1_AUTOMATIC_OUTPUT_STATE(STATE) (((STATE) == TIM1_AutomaticOutput_Enable) || \
    ((STATE) == TIM1_AutomaticOutput_Disable))

#define IS_TIM1_LOCK_LEVEL(LEVEL) (((LEVEL) == TIM1_LockLevel_Off) || \
                                   ((LEVEL) == TIM1_LockLevel_1) || \
                                   ((LEVEL) == TIM1_LockLevel_2) || \
                                   ((LEVEL) == TIM1_LockLevel_3))

#define IS_TIM1_OSSI_STATE(STATE) (((STATE) == TIM1_OSSIState_Enable) || \
                                   ((STATE) == TIM1_OSSIState_Disable))

#define IS_TIM1_OCIDLE_STATE(STATE) (((STATE) == TIM1_OCIdleState_Set) || \
                                     ((STATE) == TIM1_OCIdleState_Reset))

#define IS_TIM1_OCNIDLE_STATE(STATE) (((STATE) == TIM1_OCNIdleState_Set) || \
                                      ((STATE) == TIM1_OCNIdleState_Reset))

#define IS_TIM1_IC_POLARITY(POLARITY) (((POLARITY) == TIM1_ICPolarity_Rising) || \
                                       ((POLARITY) == TIM1_ICPolarity_Falling))

#define IS_TIM1_IC_SELECTION(SELECTION) (((SELECTION) == TIM1_ICSelection_DirectTI) || \
    ((SELECTION) == TIM1_ICSelection_IndirectTI) || \
    ((SELECTION) == TIM1_ICSelection_TRGI))

#define IS_TIM1_IC_PRESCALER(PRESCALER) (((PRESCALER) == TIM1_ICPSC_DIV1) || \
    ((PRESCALER) == TIM1_ICPSC_DIV2) || \
    ((PRESCALER) == TIM1_ICPSC_DIV4) || \
    ((PRESCALER) == TIM1_ICPSC_DIV8))

#define IS_TIM1_OCREFERENCECECLEAR_SOURCE(SOURCE) (((SOURCE) == TIM1_OCReferenceClear_ETRF) || \
    ((SOURCE) == TIM1_OCReferenceClear_OCREFCLR))

#define IS_TIM1_IT(IT) ((IT) != 0x00)

#define IS_TIM1_GET_IT(IT) (((IT) == TIM1_IT_Update) || \
                            ((IT) == TIM1_IT_CC1) || \
                            ((IT) == TIM1_IT_CC2) || \
                            ((IT) == TIM1_IT_CC3) || \
                            ((IT) == TIM1_IT_CC4) || \
                            ((IT) == TIM1_IT_COM) || \
                            ((IT) == TIM1_IT_Trigger) || \
                            ((IT) == TIM1_IT_Break))

#define IS_TIM1_EXT_PRESCALER(PRESCALER) (((PRESCALER) == TIM1_ExtTRGPSC_OFF) || \
    ((PRESCALER) == TIM1_ExtTRGPSC_DIV2) || \
    ((PRESCALER) == TIM1_ExtTRGPSC_DIV4) || \
    ((PRESCALER) == TIM1_ExtTRGPSC_DIV8))

#define IS_TIM1_TRIGGER_SELECTION(SELECTION) \
  (((SELECTION) == TIM1_TRGSelection_TIM2) || \
   ((SELECTION) == TIM1_TRGSelection_TIM3) || \
   ((SELECTION) == TIM1_TRGSelection_TIM4) || \
   ((SELECTION) == TIM1_TRGSelection_TIM5) || \
   ((SELECTION) == TIM1_TRGSelection_TI1F_ED) || \
   ((SELECTION) == TIM1_TRGSelection_TI1FP1) || \
   ((SELECTION) == TIM1_TRGSelection_TI2FP2) || \
   ((SELECTION) == TIM1_TRGSelection_ETRF))


#define IS_TIM1_TIX_TRIGGER_SELECTION(SELECTION) \
  (((SELECTION) == TIM1_TRGSelection_TI1F_ED) || \
   ((SELECTION) == TIM1_TRGSelection_TI1FP1) || \
   ((SELECTION) == TIM1_TRGSelection_TI2FP2))

#define IS_TIM1_TIXCLK_SOURCE(SOURCE)  (((SOURCE) == TIM1_TIxExternalCLK1Source_TI1ED) || \
                                        ((SOURCE) == TIM1_TIxExternalCLK1Source_TI2) || \
                                        ((SOURCE) == TIM1_TIxExternalCLK1Source_TI1))

#define IS_TIM1_EXT_POLARITY(POLARITY) (((POLARITY) == TIM1_ExtTRGPolarity_Inverted) || \
                                        ((POLARITY) == TIM1_ExtTRGPolarity_NonInverted))

#define IS_TIM1_PRESCALER_RELOAD(RELOAD) (((RELOAD) == TIM1_PSCReloadMode_Update) || \
    ((RELOAD) == TIM1_PSCReloadMode_Immediate))

#define IS_TIM1_ENCODER_MODE(MODE) (((MODE) == TIM1_EncoderMode_TI1) || \
                                    ((MODE) == TIM1_EncoderMode_TI2) || \
                                    ((MODE) == TIM1_EncoderMode_TI12))

#define IS_TIM1_EVENT_SOURCE(SOURCE) ((SOURCE) != 0x00)

#define IS_TIM1_TRGO_SOURCE(SOURCE) (((SOURCE) == TIM1_TRGOSource_Reset) || \
                                     ((SOURCE) == TIM1_TRGOSource_Enable) || \
                                     ((SOURCE) == TIM1_TRGOSource_Update) || \
                                     ((SOURCE) == TIM1_TRGOSource_OC1)  || \
                                     ((SOURCE) == TIM1_TRGOSource_OC1REF) || \
                                     ((SOURCE) == TIM1_TRGOSource_OC2REF) || \
                                     ((SOURCE) == TIM1_TRGOSource_OC3REF) || \
                                     ((SOURCE) == TIM1_TRGOSource_OC4REF))


#define IS_TIM1_UPDATE_SOURCE(SOURCE) (((SOURCE) == TIM1_UpdateSource_Global) || \
                                       ((SOURCE) == TIM1_UpdateSource_Regular))

#define IS_TIM1_GET_FLAG(FLAG) (((FLAG) == TIM1_FLAG_Update) || \
                                ((FLAG) == TIM1_FLAG_CC1)    || \
                                ((FLAG) == TIM1_FLAG_CC2)    || \
                                ((FLAG) == TIM1_FLAG_CC3)    || \
                                ((FLAG) == TIM1_FLAG_CC4)    || \
                                ((FLAG) == TIM1_FLAG_COM)    || \
                                ((FLAG) == TIM1_FLAG_Trigger)|| \
                                ((FLAG) == TIM1_FLAG_Break)  || \
                                ((FLAG) == TIM1_FLAG_CC1OF)  || \
                                ((FLAG) == TIM1_FLAG_CC2OF)  || \
                                ((FLAG) == TIM1_FLAG_CC3OF)  || \
                                ((FLAG) == TIM1_FLAG_CC4OF))


#define IS_TIM1_SLAVE_MODE(MODE) (((MODE) == TIM1_SlaveMode_Reset) || \
                                  ((MODE) == TIM1_SlaveMode_Gated) || \
                                  ((MODE) == TIM1_SlaveMode_Trigger) || \
                                  ((MODE) == TIM1_SlaveMode_External1))

#define IS_TIM1_CLEAR_FLAG(FLAG) ((((uint16_t)(FLAG) & (uint16_t)0xE100) == 0x0000) && ((FLAG) != 0x0000))

#define IS_TIM1_FORCED_ACTION(ACTION) (((ACTION) == TIM1_ForcedAction_Active) || \
                                       ((ACTION) == TIM1_ForcedAction_Inactive))

#define IS_TIM1_DMA_SOURCE(SOURCE) \
  (((SOURCE) == TIM1_DMASource_Update) || \
   ((SOURCE) == TIM1_DMASource_CC1) || \
   ((SOURCE) == TIM1_DMASource_CC2) || \
   ((SOURCE) == TIM1_DMASource_CC3) || \
   ((SOURCE) == TIM1_DMASource_CC4) || \
   ((SOURCE) == TIM1_DMASource_COM))

#define IS_TIM1_DMABase(SOURCE) \
  (((SOURCE) == TIM1_DMABase_CR1)  || \
   ((SOURCE) == TIM1_DMABase_CR2)  || \
   ((SOURCE) == TIM1_DMABase_SMCR) || \
   ((SOURCE) == TIM1_DMABase_ETR) || \
   ((SOURCE) == TIM1_DMABase_DER) || \
   ((SOURCE) == TIM1_DMABase_IER) || \
   ((SOURCE) == TIM1_DMABase_SR1) || \
   ((SOURCE) == TIM1_DMABase_SR2) || \
   ((SOURCE) == TIM1_DMABase_EGR) || \
   ((SOURCE) == TIM1_DMABase_CCMR1) || \
   ((SOURCE) == TIM1_DMABase_CCMR2  ) || \
   ((SOURCE) == TIM1_DMABase_CCMR3) || \
   ((SOURCE) == TIM1_DMABase_CCMR4) || \
   ((SOURCE) == TIM1_DMABase_CCER1) || \
   ((SOURCE) == TIM1_DMABase_CCER2) || \
   ((SOURCE) == TIM1_DMABase_CNTH) || \
   ((SOURCE) == TIM1_DMABase_CNTL) || \
   ((SOURCE) == TIM1_DMABase_PSCH) || \
   ((SOURCE) == TIM1_DMABase_PSCL) || \
   ((SOURCE) == TIM1_DMABase_ARRH) || \
   ((SOURCE) == TIM1_DMABase_ARRL) || \
   ((SOURCE) == TIM1_DMABase_RCR)  || \
   ((SOURCE) == TIM1_DMABase_CCR1H) || \
   ((SOURCE) == TIM1_DMABase_CCR1L) || \
   ((SOURCE) == TIM1_DMABase_CCR2H  ) || \
   ((SOURCE) == TIM1_DMABase_CCR2L) || \
   ((SOURCE) == TIM1_DMABase_CCR3H) || \
   ((SOURCE) == TIM1_DMABase_CCR3L) || \
   ((SOURCE) == TIM1_DMABase_CCR4H) || \
   ((SOURCE) == TIM1_DMABase_CCR4L) || \
   ((SOURCE) == TIM1_DMABase_BKR) || \
   ((SOURCE) == TIM1_DMABase_DTR))

#define IS_TIM1_DMABurstLength(SOURCE) \
  (((SOURCE)  == TIM1_DMABurstLength_1Byte)  || \
   ((SOURCE) == TIM1_DMABurstLength_2Byte)  || \
   ((SOURCE) == TIM1_DMABurstLength_3Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_4Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_5Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_6Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_7Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_8Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_9Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_10Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_11Byte  ) || \
   ((SOURCE) == TIM1_DMABurstLength_12Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_13Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_14Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_15Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_16Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_17Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_18Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_19Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_20Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_21Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_22Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_23Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_24Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_25Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_26Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_27Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_28Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_29Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_30Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_31Byte) || \
   ((SOURCE) == TIM1_DMABurstLength_32Byte))

/** TIM1 External Trigger Filer Value */
#define IS_TIM1_EXT_TRG_FILTER(FILTER) ((FILTER) <= 0x0F)

/** TIM1 Input Capture Filer Value */
#define IS_TIM1_IC_FILTER(ICFILTER) ((ICFILTER) <= 0x0F)
/**
  * @}
  */

/* Exported Functions -----------------------------------------------------------*/

/* TimeBase management ********************************************************/
void TIM1_DeInit(void);
void TIM1_TimeBaseInit(uint16_t TIM1_Prescaler,
                       TIM1_CounterMode_TypeDef TIM1_CounterMode,
                       uint16_t TIM1_Period,
                       uint8_t TIM1_RepetitionCounter);
void TIM1_PrescalerConfig(uint16_t Prescaler,
                          TIM1_PSCReloadMode_TypeDef TIM1_PSCReloadMode);
void TIM1_CounterModeConfig(TIM1_CounterMode_TypeDef TIM1_CounterMode);
void TIM1_SetCounter(uint16_t Counter);
void TIM1_SetAutoreload(uint16_t Autoreload);
uint16_t TIM1_GetCounter(void);
uint16_t TIM1_GetPrescaler(void);
void TIM1_UpdateDisableConfig(FunctionalState NewState);
void TIM1_UpdateRequestConfig(TIM1_UpdateSource_TypeDef TIM1_UpdateSource);
void TIM1_ARRPreloadConfig(FunctionalState NewState);
void TIM1_SelectOnePulseMode(TIM1_OPMode_TypeDef TIM1_OPMode);
void TIM1_Cmd(FunctionalState NewState);

/* Output Compare management **************************************************/
void TIM1_OC1Init(TIM1_OCMode_TypeDef TIM1_OCMode,
                  TIM1_OutputState_TypeDef TIM1_OutputState,
                  TIM1_OutputNState_TypeDef TIM1_OutputNState,
                  uint16_t TIM1_Pulse,
                  TIM1_OCPolarity_TypeDef TIM1_OCPolarity,
                  TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity,
                  TIM1_OCIdleState_TypeDef TIM1_OCIdleState,
                  TIM1_OCNIdleState_TypeDef TIM1_OCNIdleState);
void TIM1_OC2Init(TIM1_OCMode_TypeDef TIM1_OCMode,
                  TIM1_OutputState_TypeDef TIM1_OutputState,
                  TIM1_OutputNState_TypeDef TIM1_OutputNState,
                  uint16_t TIM1_Pulse,
                  TIM1_OCPolarity_TypeDef TIM1_OCPolarity,
                  TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity,
                  TIM1_OCIdleState_TypeDef TIM1_OCIdleState,
                  TIM1_OCNIdleState_TypeDef TIM1_OCNIdleState);
void TIM1_OC3Init(TIM1_OCMode_TypeDef TIM1_OCMode,
                  TIM1_OutputState_TypeDef TIM1_OutputState,
                  TIM1_OutputNState_TypeDef TIM1_OutputNState,
                  uint16_t TIM1_Pulse,
                  TIM1_OCPolarity_TypeDef TIM1_OCPolarity,
                  TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity,
                  TIM1_OCIdleState_TypeDef TIM1_OCIdleState,
                  TIM1_OCNIdleState_TypeDef TIM1_OCNIdleState);
void TIM1_BDTRConfig(TIM1_OSSIState_TypeDef TIM1_OSSIState,
                     TIM1_LockLevel_TypeDef TIM1_LockLevel,
                     uint8_t TIM1_DeadTime,
                     TIM1_BreakState_TypeDef TIM1_Break,
                     TIM1_BreakPolarity_TypeDef TIM1_BreakPolarity,
                     TIM1_AutomaticOutput_TypeDef TIM1_AutomaticOutput);
void TIM1_CtrlPWMOutputs(FunctionalState NewState);
void TIM1_SelectOCxM(TIM1_Channel_TypeDef TIM1_Channel, TIM1_OCMode_TypeDef TIM1_OCMode);
void TIM1_SetCompare1(uint16_t Compare1);
void TIM1_SetCompare2(uint16_t Compare2);
void TIM1_SetCompare3(uint16_t Compare3);
void TIM1_SetCompare4(uint16_t Compare4);
void TIM1_CCPreloadControl(FunctionalState NewState);
void TIM1_ForcedOC1Config(TIM1_ForcedAction_TypeDef TIM1_ForcedAction);
void TIM1_ForcedOC2Config(TIM1_ForcedAction_TypeDef TIM1_ForcedAction);
void TIM1_ForcedOC3Config(TIM1_ForcedAction_TypeDef TIM1_ForcedAction);
void TIM1_OC1PreloadConfig(FunctionalState NewState);
void TIM1_OC2PreloadConfig(FunctionalState NewState);
void TIM1_OC3PreloadConfig(FunctionalState NewState);
void TIM1_OC4PreloadConfig(FunctionalState NewState);
void TIM1_OC1FastConfig(FunctionalState NewState);
void TIM1_OC2FastConfig(FunctionalState NewState);
void TIM1_OC3FastConfig(FunctionalState NewState);
void TIM1_ClearOC1Ref(FunctionalState NewState);
void TIM1_ClearOC2Ref(FunctionalState NewState);
void TIM1_ClearOC3Ref(FunctionalState NewState);
void TIM1_ClearOC4Ref(FunctionalState NewState);
void TIM1_OC1PolarityConfig(TIM1_OCPolarity_TypeDef TIM1_OCPolarity);
void TIM1_OC1NPolarityConfig(TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity);
void TIM1_OC2PolarityConfig(TIM1_OCPolarity_TypeDef TIM1_OCPolarity);
void TIM1_OC2NPolarityConfig(TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity);
void TIM1_OC3PolarityConfig(TIM1_OCPolarity_TypeDef TIM1_OCPolarity);
void TIM1_OC3NPolarityConfig(TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity);
void TIM1_SelectOCREFClear(TIM1_OCReferenceClear_TypeDef TIM1_OCReferenceClear);
void TIM1_SelectCOM(FunctionalState NewState);
void TIM1_CCxCmd(TIM1_Channel_TypeDef TIM1_Channel, FunctionalState NewState);
void TIM1_CCxNCmd(TIM1_Channel_TypeDef TIM1_Channel, FunctionalState NewState);

/* Input Capture management ***************************************************/
void TIM1_ICInit(TIM1_Channel_TypeDef TIM1_Channel,
                 TIM1_ICPolarity_TypeDef TIM1_ICPolarity,
                 TIM1_ICSelection_TypeDef TIM1_ICSelection,
                 TIM1_ICPSC_TypeDef TIM1_ICPrescaler,
                 uint8_t TIM1_ICFilter);
void TIM1_PWMIConfig(TIM1_Channel_TypeDef TIM1_Channel,
                     TIM1_ICPolarity_TypeDef TIM1_ICPolarity,
                     TIM1_ICSelection_TypeDef TIM1_ICSelection,
                     TIM1_ICPSC_TypeDef TIM1_ICPrescaler,
                     uint8_t TIM1_ICFilter);
uint16_t TIM1_GetCapture1(void);
uint16_t TIM1_GetCapture2(void);
uint16_t TIM1_GetCapture3(void);
uint16_t TIM1_GetCapture4(void);
void TIM1_SetIC1Prescaler(TIM1_ICPSC_TypeDef TIM1_IC1Prescaler);
void TIM1_SetIC2Prescaler(TIM1_ICPSC_TypeDef TIM1_IC2Prescaler);
void TIM1_SetIC3Prescaler(TIM1_ICPSC_TypeDef TIM1_IC3Prescaler);
void TIM1_SetIC4Prescaler(TIM1_ICPSC_TypeDef TIM1_IC4Prescaler);

/* Interrupts, DMA and flags management ***************************************/
void TIM1_ITConfig(TIM1_IT_TypeDef TIM1_IT, FunctionalState NewState);
void TIM1_GenerateEvent(TIM1_EventSource_TypeDef TIM1_EventSource);
FlagStatus TIM1_GetFlagStatus(TIM1_FLAG_TypeDef TIM1_FLAG);
void TIM1_ClearFlag(TIM1_FLAG_TypeDef TIM1_FLAG);
ITStatus TIM1_GetITStatus(TIM1_IT_TypeDef TIM1_IT);
void TIM1_ClearITPendingBit(TIM1_IT_TypeDef TIM1_IT);
void TIM1_DMAConfig(TIM1_DMABase_TypeDef TIM1_DMABase,
                    TIM1_DMABurstLength_TypeDef TIM1_DMABurstLength);
void TIM1_DMACmd(TIM1_DMASource_TypeDef TIM1_DMASource, FunctionalState NewState);
void TIM1_SelectCCDMA(FunctionalState NewState);

/* Clocks management **********************************************************/
void TIM1_InternalClockConfig(void);
void TIM1_TIxExternalClockConfig(TIM1_TIxExternalCLK1Source_TypeDef TIM1_TIxExternalCLKSource,
                                 TIM1_ICPolarity_TypeDef TIM1_ICPolarity,
                                 uint8_t ICFilter);
void TIM1_ETRClockMode1Config(TIM1_ExtTRGPSC_TypeDef TIM1_ExtTRGPrescaler,
                              TIM1_ExtTRGPolarity_TypeDef TIM1_ExtTRGPolarity,
                              uint8_t ExtTRGFilter);
void TIM1_ETRClockMode2Config(TIM1_ExtTRGPSC_TypeDef TIM1_ExtTRGPrescaler,
                              TIM1_ExtTRGPolarity_TypeDef TIM1_ExtTRGPolarity,
                              uint8_t ExtTRGFilter);
                              
/* Synchronization management *************************************************/
void TIM1_SelectInputTrigger(TIM1_TRGSelection_TypeDef TIM1_InputTriggerSource);
void TIM1_SelectOutputTrigger(TIM1_TRGOSource_TypeDef TIM1_TRGOSource);
void TIM1_SelectSlaveMode(TIM1_SlaveMode_TypeDef TIM1_SlaveMode);
void TIM1_SelectMasterSlaveMode(FunctionalState NewState);
void TIM1_ETRConfig(TIM1_ExtTRGPSC_TypeDef TIM1_ExtTRGPrescaler,
                    TIM1_ExtTRGPolarity_TypeDef TIM1_ExtTRGPolarity,
                    uint8_t ExtTRGFilter);

/* Specific interface management **********************************************/
void TIM1_EncoderInterfaceConfig(TIM1_EncoderMode_TypeDef TIM1_EncoderMode,
                                 TIM1_ICPolarity_TypeDef TIM1_IC1Polarity,
                                 TIM1_ICPolarity_TypeDef TIM1_IC2Polarity);
void TIM1_SelectHallSensor(FunctionalState NewState);

#endif /* __STM8L15x_TIM1_H */

/**
  * @}
  */
  
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
