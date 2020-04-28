/**
  ******************************************************************************
  * @file    stm8l15x_dac.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the DAC firmware
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
#ifndef __STM8L15x_DAC_H
#define __STM8L15x_DAC_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @addtogroup DAC
  * @{
  */
  
/* Exported types ------------------------------------------------------------*/

/** @defgroup DAC_Exported_types
  * @{
  */
  
/** @defgroup DAC_trigger_selection 
  * @{
  */
typedef enum
{
  DAC_Trigger_None     = ((uint8_t)0x30), /*!< DAC trigger None  */
  DAC_Trigger_T4_TRGO  = ((uint8_t)0x00), /*!< DAC trigger TIM4 TRGO  */
  DAC_Trigger_T5_TRGO  = ((uint8_t)0x08), /*!< DAC trigger TIM5 TRGO  */
  DAC_Trigger_Ext      = ((uint8_t)0x10), /*!< DAC trigger External Trigger (PE4) */
  DAC_Trigger_Software = ((uint8_t)0x38)  /*!< DAC trigger software  */
}DAC_Trigger_TypeDef;

#define IS_DAC_TRIGGER(TRIGGER) (((TRIGGER) == DAC_Trigger_None) || \
                                 ((TRIGGER) == DAC_Trigger_T4_TRGO) || \
                                 ((TRIGGER) == DAC_Trigger_T5_TRGO) || \
                                 ((TRIGGER) == DAC_Trigger_Ext) || \
                                 ((TRIGGER) == DAC_Trigger_Software))
/**
  * @}
  */

/** @defgroup DAC_data_alignment 
  * @{
  */
typedef enum
{
  DAC_Align_12b_R = ((uint8_t)0x00), /*!< DAC alignment  Right 12bit */
  DAC_Align_12b_L = ((uint8_t)0x04), /*!< DAC alignment  Left 12bit */
  DAC_Align_8b_R  = ((uint8_t)0x08)  /*!< DAC alignment  Right 8bit */
}DAC_Align_TypeDef;

#define IS_DAC_ALIGN(ALIGN) (((ALIGN) == DAC_Align_12b_R) || \
                             ((ALIGN) == DAC_Align_12b_L) || \
                             ((ALIGN) == DAC_Align_8b_R))
/**
  * @}
  */
  
/** @defgroup DAC_Channel_selection 
  * @{
  */
typedef enum
{
  DAC_Channel_1 = ((uint8_t)0x00), /*!< DAC Channel 1 selection  */
  DAC_Channel_2 = ((uint8_t)0x01)  /*!< DAC Channel 2 selection  */
}DAC_Channel_TypeDef;

#define IS_DAC_CHANNEL(CHANNEL) (((CHANNEL) == DAC_Channel_1) || \
                                 ((CHANNEL) == DAC_Channel_2))
                                 
/**
  * @}
  */
  
/** @defgroup DAC_wave_generation 
  * @{
  */

typedef enum
{
  DAC_Wave_Noise    = ((uint8_t)0x40), /*!< Noise Wave Generation  */
  DAC_Wave_Triangle = ((uint8_t)0x80)  /*!< Triangle Wave Generation */
}DAC_Wave_TypeDef;

#define IS_DAC_WAVE(WAVE) (((WAVE) == DAC_Wave_Noise) || \
                           ((WAVE) == DAC_Wave_Triangle))
/**
  * @}
  */
  
/** @defgroup DAC_output_buffer 
  * @{
  */
typedef enum
{
  DAC_OutputBuffer_Enable  = ((uint8_t)0x00), /*!< DAC output buffer Enabled  */
  DAC_OutputBuffer_Disable = ((uint8_t)0x02)  /*!< DAC output buffer Disabled  */
}DAC_OutputBuffer_TypeDef;

#define IS_DAC_OUTPUT_BUFFER_STATE(STATE) (((STATE) == DAC_OutputBuffer_Enable) || \
                                           ((STATE) == DAC_OutputBuffer_Disable))
/**
  * @}
  */
  
/** @defgroup DAC_interrupts_definition 
  * @{
  */
typedef enum
{
  DAC_IT_DMAUDR = ((uint8_t)0x20) /*!< DMA Underrun Interrupt */
}DAC_IT_TypeDef;

#define IS_DAC_IT(IT) (((IT) == DAC_IT_DMAUDR))
/**
  * @}
  */
  
/** @defgroup DAC_flags_definition 
  * @{
  */ 
typedef enum
{
  DAC_FLAG_DMAUDR = ((uint8_t)0x01) /*!< DMA Underrun flag  */
}DAC_FLAG_TypeDef;

#define IS_DAC_GET_FLAG(FLAG) (((FLAG) == DAC_FLAG_DMAUDR))
#define IS_DAC_FLAG(FLAG) (((FLAG) == DAC_FLAG_DMAUDR))

/**
  * @}
  */
  
/** @defgroup DAC_lfsrunmask
  * @{
  */
  
typedef enum
{
  DAC_LFSRUnmask_Bit0       =  ((uint8_t)0x00), /*!<  Noise LFSR Unmask 1 LSB */
  DAC_LFSRUnmask_Bits1_0    =  ((uint8_t)0x01), /*!<  Noise LFSR Unmask 2 LSB */
  DAC_LFSRUnmask_Bits2_0    =  ((uint8_t)0x02), /*!<  Noise LFSR Unmask 3 LSB */
  DAC_LFSRUnmask_Bits3_0    =  ((uint8_t)0x03), /*!<  Noise LFSR Unmask 4 LSB */
  DAC_LFSRUnmask_Bits4_0    =  ((uint8_t)0x04), /*!<  Noise LFSR Unmask 5 LSB */
  DAC_LFSRUnmask_Bits5_0    =  ((uint8_t)0x05), /*!<  Noise LFSR Unmask 6 LSB */
  DAC_LFSRUnmask_Bits6_0    =  ((uint8_t)0x06), /*!<  Noise LFSR Unmask 7 LSB */
  DAC_LFSRUnmask_Bits7_0    =  ((uint8_t)0x07), /*!<  Noise LFSR Unmask 8 LSB */
  DAC_LFSRUnmask_Bits8_0    =  ((uint8_t)0x08), /*!<  Noise LFSR Unmask 9 LSB */
  DAC_LFSRUnmask_Bits9_0    =  ((uint8_t)0x09), /*!<  Noise LFSR Unmask 10 LSB */
  DAC_LFSRUnmask_Bits10_0   =  ((uint8_t)0x0A), /*!<  Noise LFSR Unmask 11 LSB */
  DAC_LFSRUnmask_Bits11_0   =  ((uint8_t)0x0B)  /*!<  Noise LFSR Unmask 12 LSB */
}DAC_LFSRUnmask_TypeDef;

#define IS_DAC_LFSR_UNMASK_TRIANGLE_AMPLITUDE(VALUE) ((VALUE) <= 0x0F)
/**
  * @}
  */
  
/** @defgroup DAC_triangleamplitude
  * @{
  */
typedef enum
{
  DAC_TriangleAmplitude_1     =  ((uint8_t)0x00), /*!<  Triangle Amplitude = Vref.(1/4096)*/
  DAC_TriangleAmplitude_3     =  ((uint8_t)0x01), /*!<  Triangle Amplitude = Vref.(3/4096)*/
  DAC_TriangleAmplitude_7     =  ((uint8_t)0x02), /*!<  Triangle Amplitude = Vref.(7/4096)*/
  DAC_TriangleAmplitude_15    =  ((uint8_t)0x03), /*!<  Triangle Amplitude = Vref.(15/4096)*/
  DAC_TriangleAmplitude_31    =  ((uint8_t)0x04), /*!<  Triangle Amplitude = Vref.(31/4096)*/
  DAC_TriangleAmplitude_63    =  ((uint8_t)0x05), /*!<  Triangle Amplitude = Vref.(63/4096)*/
  DAC_TriangleAmplitude_127   =  ((uint8_t)0x06), /*!<  Triangle Amplitude = Vref.(127/4096)*/
  DAC_TriangleAmplitude_255   =  ((uint8_t)0x07), /*!<  Triangle Amplitude = Vref.(255/4096)*/
  DAC_TriangleAmplitude_511   =  ((uint8_t)0x08), /*!<  Triangle Amplitude = Vref.(511/4096)*/
  DAC_TriangleAmplitude_1023  =  ((uint8_t)0x09), /*!<  Triangle Amplitude = Vref.(1023/4096)*/
  DAC_TriangleAmplitude_2047  =  ((uint8_t)0x0A), /*!<  Triangle Amplitude = Vref.(2047/4096)*/
  DAC_TriangleAmplitude_4095  =  ((uint8_t)0x0B)  /*!<  Triangle Amplitude = Vref.(4095/4096)*/
}DAC_TriangleAmplitude_TypeDef;

/**
  * @}
  */
  
/**
  * @}
  */
/* Exported constants --------------------------------------------------------*/
/** @defgroup DAC_Exported_Constants
  * @{
  */

/** @defgroup DAC_data 
  * @{
  */
#define IS_DAC_DATA_08R(DATA) ((DATA) <= 0x00FF) 

/**
  * @}
  */
/** @defgroup DAC_Registers_Offset 
  * @{
  */  
#define CR1_Offset         ((uint8_t)0x00)
#define CR2_Offset         ((uint8_t)0x01)
#define DCH1RDHRH_Offset   ((uint8_t)0x20)
#define CH1RDHRH_Offset    ((uint8_t)0x08)
#define CH2RDHRH_Offset    ((uint8_t)0x14)
/**
  * @}
  */

/** @defgroup DAC_legacy 
  * @{
  */
#define DAC_TriangleWaveAmplitude  DAC_SetTriangleWaveAmplitude
#define DAC_NoiseWaveLFSR  DAC_SetNoiseWaveLFSR 

/**
  * @}
  */
     
/**
  * @}
  */ 
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 

/*  Function used to set the DAC configuration to the default reset state *****/  
void DAC_DeInit(void);

/*  DAC channels configuration: trigger, output buffer, data format functions */
void DAC_Init(DAC_Channel_TypeDef DAC_Channel,
              DAC_Trigger_TypeDef DAC_Trigger,
              DAC_OutputBuffer_TypeDef DAC_OutputBuffer);
void DAC_Cmd(DAC_Channel_TypeDef DAC_Channel, FunctionalState NewState);
void DAC_SoftwareTriggerCmd(DAC_Channel_TypeDef DAC_Channel, FunctionalState NewState);
void DAC_DualSoftwareTriggerCmd(FunctionalState NewState);
void DAC_WaveGenerationCmd(DAC_Channel_TypeDef DAC_Channel, DAC_Wave_TypeDef DAC_Wave, FunctionalState NewState);
void DAC_SetNoiseWaveLFSR(DAC_Channel_TypeDef DAC_Channel, DAC_LFSRUnmask_TypeDef DAC_LFSRUnmask);
void DAC_SetTriangleWaveAmplitude(DAC_Channel_TypeDef DAC_Channel, DAC_TriangleAmplitude_TypeDef DAC_TriangleAmplitude);
void DAC_SetChannel1Data(DAC_Align_TypeDef DAC_Align, uint16_t DAC_Data);
void DAC_SetChannel2Data(DAC_Align_TypeDef DAC_Align, uint16_t DAC_Data);
void DAC_SetDualChannelData(DAC_Align_TypeDef DAC_Align, uint16_t DAC_Data2, uint16_t DAC_Data1);
uint16_t DAC_GetDataOutputValue(DAC_Channel_TypeDef DAC_Channel);

/* DMA management function ***************************************************/                            
void DAC_DMACmd(DAC_Channel_TypeDef DAC_Channel, FunctionalState NewState);

/* Interrupts and flags management functions **********************************/
void DAC_ITConfig(DAC_Channel_TypeDef DAC_Channel, DAC_IT_TypeDef DAC_IT, FunctionalState NewState);
FlagStatus DAC_GetFlagStatus(DAC_Channel_TypeDef DAC_Channel, DAC_FLAG_TypeDef DAC_FLAG);
void DAC_ClearFlag(DAC_Channel_TypeDef DAC_Channel, DAC_FLAG_TypeDef DAC_FLAG);
ITStatus DAC_GetITStatus(DAC_Channel_TypeDef DAC_Channel, DAC_IT_TypeDef DAC_IT);
void DAC_ClearITPendingBit(DAC_Channel_TypeDef DAC_Channel, DAC_IT_TypeDef DAC_IT);

#endif /*__STM8L15x_DAC_H */

/**
  * @}
  */
  
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
