/**
  ******************************************************************************
  * @file    stm8l15x_comp.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the COMP firmware
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
#ifndef __STM8L15x_COMP_H
#define __STM8L15x_COMP_H


/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @addtogroup COMP
  * @{
  */ 
  
/* Exported types ------------------------------------------------------------*/

/** @defgroup COMP_Exported_Types
  * @{
  */

/** @defgroup COMP_Selection
  * @{
  */
typedef enum
{
  COMP_Selection_COMP1 = ((uint8_t)0x01), /*!< Selection of Comparator 1. */
  COMP_Selection_COMP2 = ((uint8_t)0x02)  /*!< Selection of Comparator 2. */
}COMP_Selection_TypeDef;

#define IS_COMP_ALL_PERIPH(PERIPH) (((PERIPH) == COMP_Selection_COMP1) || \
                                    ((PERIPH) == COMP_Selection_COMP2))
/**
  * @}
  */
  
/** @defgroup COMP_Edge
  * @{
  */
typedef enum
{
  COMP_Edge_Falling         = ((uint8_t)0x01), /*!< Falling edge selection. */
  COMP_Edge_Rising          = ((uint8_t)0x02), /*!< Rising edge selection. */
  COMP_Edge_Rising_Falling  = ((uint8_t)0x03)  /*!< Rising and Falling edge selection. */
}COMP_Edge_TypeDef;

#define IS_COMP_EDGE(EDGE) (((EDGE) == COMP_Edge_Falling) || \
                            ((EDGE) == COMP_Edge_Rising)  || \
                            ((EDGE) == COMP_Edge_Rising_Falling))
/**
  * @}
  */
  
/** @defgroup COMP_Inverting_Input_Selection
  * @{
  */
typedef enum
{
  COMP_InvertingInput_IO         = ((uint8_t)0x08), /*!< Input/Output on comparator inverting input enable.*/
  COMP_InvertingInput_VREFINT    = ((uint8_t)0x10), /*!< VREFINT on comparator inverting input enable.     */
  COMP_InvertingInput_3_4VREFINT = ((uint8_t)0x18), /*!< 3/4 VREFINT on comparator inverting input enable. */
  COMP_InvertingInput_1_2VREFINT = ((uint8_t)0x20), /*!< 1/2 VREFINT on comparator inverting input enable. */
  COMP_InvertingInput_1_4VREFINT = ((uint8_t)0x28), /*!< 1/4 VREFINT on comparator inverting input enable. */
  COMP_InvertingInput_DAC1       = ((uint8_t)0x30),  /*!< DAC1 output on comparator inverting input enable.  */
  COMP_InvertingInput_DAC2       = ((uint8_t)0x38)  /*!< DAC2 output on comparator inverting input enable.  */
}COMP_InvertingInput_Typedef;

#define IS_COMP_INVERTING_INPUT(INPUT) (((INPUT) == COMP_InvertingInput_IO) || \
                                        ((INPUT) == COMP_InvertingInput_VREFINT) || \
                                        ((INPUT) == COMP_InvertingInput_3_4VREFINT) || \
                                        ((INPUT) == COMP_InvertingInput_1_2VREFINT) || \
                                        ((INPUT) == COMP_InvertingInput_1_4VREFINT) || \
                                        ((INPUT) == COMP_InvertingInput_DAC1) || \
                                        ((INPUT) == COMP_InvertingInput_DAC2))
/**
  * @}
  */
  
/** @defgroup COMP2_Output_Selection
  * @{
  */
typedef enum
{
  COMP_OutputSelect_TIM2IC2       = ((uint8_t)0x00), /*!< COMP2 output connected to TIM2 Input Capture 2 */
  COMP_OutputSelect_TIM3IC2       = ((uint8_t)0x40), /*!< COMP2 output connected to TIM3 Input Capture 2 */
  COMP_OutputSelect_TIM1BRK       = ((uint8_t)0x80), /*!< COMP2 output connected to TIM1 Break Input     */
  COMP_OutputSelect_TIM1OCREFCLR  = ((uint8_t)0xC0)  /*!< COMP2 output connected to TIM1 OCREF Clear     */
}COMP_OutputSelect_Typedef;

#define IS_COMP_OUTPUT(OUTPUT) (((OUTPUT) == COMP_OutputSelect_TIM2IC2) || \
                                ((OUTPUT) == COMP_OutputSelect_TIM3IC2) || \
                                ((OUTPUT) == COMP_OutputSelect_TIM1BRK) || \
                                ((OUTPUT) == COMP_OutputSelect_TIM1OCREFCLR))
/**
  * @}
  */
  
/** @defgroup COMP_Speed
  * @{
  */
typedef enum
{
  COMP_Speed_Slow  = ((uint8_t)0x00), /*!< Comparator speed: slow */
  COMP_Speed_Fast  = ((uint8_t)0x04)  /*!< Comparator speed: fast */
}COMP_Speed_TypeDef;

#define IS_COMP_SPEED(SPEED) (((SPEED) == COMP_Speed_Slow) || \
                              ((SPEED) == COMP_Speed_Fast))
/**
  * @}
  */
  
/** @defgroup COMP_Trigger_Group
  * @{
  */
typedef enum
{
  COMP_TriggerGroup_InvertingInput     = ((uint8_t)0x01), /*!< Trigger on comparator 2 inverting input */
  COMP_TriggerGroup_NonInvertingInput  = ((uint8_t)0x02), /*!< Trigger on comparator 2 non inverting input */
  COMP_TriggerGroup_VREFINTOutput      = ((uint8_t)0x03), /*!< Trigger on VREFINT output */
  COMP_TriggerGroup_DACOutput          = ((uint8_t)0x04)  /*!< Trigger on DAC output */
}COMP_TriggerGroup_TypeDef;

#define IS_COMP_TRIGGERGROUP(TRIGGERGROUP) (((TRIGGERGROUP) == COMP_TriggerGroup_NonInvertingInput) || \
                                            ((TRIGGERGROUP) == COMP_TriggerGroup_InvertingInput) || \
                                            ((TRIGGERGROUP) == COMP_TriggerGroup_VREFINTOutput) || \
                                            ((TRIGGERGROUP) == COMP_TriggerGroup_DACOutput)
/**
  * @}
  */
  
/** @defgroup COMP_Trigger_Pin
  * @{
  */
typedef enum
{
  COMP_TriggerPin_0 = ((uint8_t)0x01), /*!< PE5 for the non inverting input Trigger Group
                                                PC3 for the inverting input Trigger Group
                                                PB6 for the DAC output Trigger Group
                                                PC2 for the VREFINT output Trigger Group
                                              */
  COMP_TriggerPin_1 = ((uint8_t)0x02), /*!< PD0 for the non inverting input Trigger Group
                                                PC4 for the inverting input Trigger Group
                                                PB5 for the DAC output Trigger Group
                                                PD7 for the VREFINT output Trigger Group
                                              */
  COMP_TriggerPin_2 = ((uint8_t)0x04)  /*!< PD1 for the non inverting input Trigger Group
                                            PC7 for the inverting input Trigger Group
                                            PB4 for the DAC output Trigger Group
                                            PD6 for the VREFINT output Trigger Group */
}COMP_TriggerPin_TypeDef;

#define IS_COMP_TRIGGERPIN(TRIGGERPIN)  ((((uint8_t)(TRIGGERPIN) & (uint8_t)0xF8) == (uint8_t) 0x00) && \
                                         ((TRIGGERPIN) != (uint8_t)0x00))
/**
  * @}
  */
  
/** @defgroup COMP_Output_Level
  * @{
  */
typedef enum
{
  COMP_OutputLevel_Low   = ((uint8_t)0x00), /*!< Comparator output level is low */
  COMP_OutputLevel_High  = ((uint8_t)0x01)  /*!< Comparator output level is high */
}COMP_OutputLevel_TypeDef;

/**
  * @}
  */
  
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Function used to set the CLK configuration to the default reset state ******/
void COMP_DeInit(void);
/* Initialization and Configuration functions ****************************/
void COMP_Init(COMP_InvertingInput_Typedef COMP_InvertingInput, COMP_OutputSelect_Typedef COMP_OutputSelect,
               COMP_Speed_TypeDef COMP_Speed);
void COMP_VrefintToCOMP1Connect(FunctionalState NewState);
void COMP_EdgeConfig(COMP_Selection_TypeDef COMP_Selection, COMP_Edge_TypeDef COMP_Edge);
COMP_OutputLevel_TypeDef COMP_GetOutputLevel(COMP_Selection_TypeDef COMP_Selection);
/* Window mode control function ***********************************************/
void COMP_WindowCmd(FunctionalState NewState);
/* Internal Reference Voltage (VREFINT) output function ***********************/
void COMP_VrefintOutputCmd(FunctionalState NewState);
/* Comparator channels trigger configuration functions ************************/
void COMP_SchmittTriggerCmd(FunctionalState NewState);
void COMP_TriggerConfig(COMP_TriggerGroup_TypeDef COMP_TriggerGroup,
                        COMP_TriggerPin_TypeDef COMP_TriggerPin,
                        FunctionalState NewState);
/* Interrupts and flags management functions **********************************/
void COMP_ITConfig(COMP_Selection_TypeDef COMP_Selection, FunctionalState NewState);
FlagStatus COMP_GetFlagStatus(COMP_Selection_TypeDef COMP_Selection);
void COMP_ClearFlag(COMP_Selection_TypeDef COMP_Selection);
ITStatus COMP_GetITStatus(COMP_Selection_TypeDef COMP_Selection);
void COMP_ClearITPendingBit(COMP_Selection_TypeDef COMP_Selection);

/**
  * @}
  */

#endif /* __STM8L15x_COMP_H */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
