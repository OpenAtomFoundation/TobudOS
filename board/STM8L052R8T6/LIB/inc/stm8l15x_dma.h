/**
  ******************************************************************************
  * @file    stm8l15x_dma.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the DMA 
  *          firmware library.
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
#ifndef __STM8L15x_DMA_H
 #define __STM8L15x_DMA_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @addtogroup DMA
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/

/** @addtogroup DMA_Exported_Types
  * @{
  */

/** @defgroup DMA_Data_Transfer_Direction
  * @{
  */
typedef enum
{
  DMA_DIR_PeripheralToMemory  = ((uint8_t)0x00), /*!< Data transfer direction is Peripheral To Memory   */
  DMA_DIR_MemoryToPeripheral  = ((uint8_t)0x08), /*!< Data transfer direction is Memory To Peripheral */
  DMA_DIR_Memory0ToMemory1    = ((uint8_t)0x40)  /*!< Data transfer direction is Memory0 To Memory 1 */
}DMA_DIR_TypeDef;

#define IS_DMA_DIR(DIR) (((DIR) == DMA_DIR_MemoryToPeripheral) || \
                         ((DIR) == DMA_DIR_PeripheralToMemory) || \
                         ((DIR) == DMA_DIR_Memory0ToMemory1 ))
/**
  * @}
  */
  
/** @defgroup DMA_Mode
  * @{
  */
typedef enum
{
  DMA_Mode_Normal = ((uint8_t)0x00),  /*!< DMA normal buffer mode*/
  DMA_Mode_Circular = ((uint8_t)0x10) /*!< DMA circular buffer mode */
}DMA_Mode_TypeDef;

#define IS_DMA_MODE(MODE) (((MODE) == DMA_Mode_Circular) || \
                           ((MODE) == DMA_Mode_Normal))
/**
  * @}
  */
  
/** @defgroup DMA_Incremented_Mode
  * @{
  */
typedef enum
{
  DMA_MemoryIncMode_Dec = ((uint8_t)0x00), /*!< DMA memory incremented mode is decremental */
  DMA_MemoryIncMode_Inc = ((uint8_t)0x20)  /*!< DMA memory incremented mode is incremental */
}DMA_MemoryIncMode_TypeDef;

#define IS_DMA_MEMORY_INC_MODE(MODE) (((MODE) == DMA_MemoryIncMode_Inc) || \
                                      ((MODE) == DMA_MemoryIncMode_Dec))
/**
  * @}
  */
  
/** @defgroup DMA_Priority
  * @{
  */
typedef enum
{
  DMA_Priority_Low      = ((uint8_t)0x00), /*!< Software Priority is Low */
  DMA_Priority_Medium   = ((uint8_t)0x10), /*!< Software Priority is Medium */
  DMA_Priority_High     = ((uint8_t)0x20), /*!< Software Priority is High */
  DMA_Priority_VeryHigh = ((uint8_t)0x30)  /*!< Software Priority is Very High*/
}DMA_Priority_TypeDef;

#define IS_DMA_PRIORITY(PRIORITY) (((PRIORITY) == DMA_Priority_VeryHigh) || \
                                   ((PRIORITY) == DMA_Priority_High) || \
                                   ((PRIORITY) == DMA_Priority_Medium) || \
                                   ((PRIORITY) == DMA_Priority_Low))
/**
  * @}
  */
  
/** @defgroup DMA_Memory_Data_Size
  * @{
  */
typedef enum
{
  DMA_MemoryDataSize_Byte     = ((uint8_t)0x00),/*!< Memory Data Size is 1 Byte */
  DMA_MemoryDataSize_HalfWord = ((uint8_t)0x08) /*!< Memory Data Size is 2 Bytes */
}DMA_MemoryDataSize_TypeDef;

#define IS_DMA_DATA_SIZE(SIZE) (((SIZE) == DMA_MemoryDataSize_Byte) || \
                                ((SIZE) == DMA_MemoryDataSize_HalfWord))
/**
  * @}
  */
  
/** @defgroup DMA_Flags
  * @{
  */
typedef enum
{
  DMA1_FLAG_GB    = ((uint16_t)0x0002), /*!< Global Busy Flag */

  DMA1_FLAG_IFC0  = ((uint16_t)0x1001), /*!< Global Interrupt Flag Channel 0 */
  DMA1_FLAG_IFC1  = ((uint16_t)0x1002), /*!< Global Interrupt Flag Channel 1 */
  DMA1_FLAG_IFC2  = ((uint16_t)0x1004), /*!< Global Interrupt Flag Channel 2 */
  DMA1_FLAG_IFC3  = ((uint16_t)0x1008), /*!< Global Interrupt Flag Channel 3 */

  DMA1_FLAG_TC0   = ((uint16_t)0x0102), /*!< Transaction Complete Interrupt Flag Channel 0 */
  DMA1_FLAG_TC1   = ((uint16_t)0x0202), /*!< Transaction Complete Interrupt Flag Channel 1 */
  DMA1_FLAG_TC2   = ((uint16_t)0x0402), /*!< Transaction Complete Interrupt Flag Channel 2 */
  DMA1_FLAG_TC3   = ((uint16_t)0x0802), /*!< Transaction Complete Interrupt Flag Channel 3 */

  DMA1_FLAG_HT0   = ((uint16_t)0x0104), /*!< Half Transaction Interrupt Flag Channel 0 */
  DMA1_FLAG_HT1   = ((uint16_t)0x0204), /*!< Half Transaction Interrupt Flag Channel 1 */
  DMA1_FLAG_HT2   = ((uint16_t)0x0404), /*!< Half Transaction Interrupt Flag Channel 2 */
  DMA1_FLAG_HT3   = ((uint16_t)0x0804), /*!< Half Transaction Interrupt Flag Channel 3 */

  DMA1_FLAG_PEND0 = ((uint16_t)0x0140), /*!< DMA Request pending on Channel 0 */
  DMA1_FLAG_PEND1 = ((uint16_t)0x0240), /*!< DMA Request pending on Channel 1 */
  DMA1_FLAG_PEND2 = ((uint16_t)0x0440), /*!< DMA Request pending on Channel 2 */
  DMA1_FLAG_PEND3 = ((uint16_t)0x0840), /*!< DMA Request pending on Channel 3 */

  DMA1_FLAG_BUSY0 = ((uint16_t)0x0180), /*!< No DMA transfer on going in Channel 0 */
  DMA1_FLAG_BUSY1 = ((uint16_t)0x0280), /*!< No DMA transfer on going in Channel 1 */
  DMA1_FLAG_BUSY2 = ((uint16_t)0x0480), /*!< No DMA transfer on going in Channel 2 */
  DMA1_FLAG_BUSY3 = ((uint16_t)0x0880)  /*!< No DMA transfer on going in Channel 3 */
}DMA_FLAG_TypeDef;

#define IS_DMA_GET_FLAG(FLAG) (((FLAG) == DMA1_FLAG_GB) || \
                               ((FLAG) == DMA1_FLAG_IFC0) || \
                               ((FLAG) == DMA1_FLAG_IFC1) || \
                               ((FLAG) == DMA1_FLAG_IFC2) || \
                               ((FLAG) == DMA1_FLAG_IFC3) || \
                               ((FLAG) == DMA1_FLAG_TC0) || \
                               ((FLAG) == DMA1_FLAG_TC1) || \
                               ((FLAG) == DMA1_FLAG_TC2) || \
                               ((FLAG) == DMA1_FLAG_TC3) || \
                               ((FLAG) == DMA1_FLAG_HT0) || \
                               ((FLAG) == DMA1_FLAG_HT1) || \
                               ((FLAG) == DMA1_FLAG_HT2) || \
                               ((FLAG) == DMA1_FLAG_HT3) || \
                               ((FLAG) == DMA1_FLAG_PEND0) || \
                               ((FLAG) == DMA1_FLAG_PEND1) || \
                               ((FLAG) == DMA1_FLAG_PEND2) || \
                               ((FLAG) == DMA1_FLAG_PEND3) || \
                               ((FLAG) == DMA1_FLAG_BUSY0) || \
                               ((FLAG) == DMA1_FLAG_BUSY1) || \
                               ((FLAG) == DMA1_FLAG_BUSY2) || \
                               ((FLAG) == DMA1_FLAG_BUSY3))

#define IS_DMA_CLEAR_FLAG(FLAG) (((FLAG) == DMA1_FLAG_TC0) || \
                                 ((FLAG) == DMA1_FLAG_TC1) || \
                                 ((FLAG) == DMA1_FLAG_TC2) || \
                                 ((FLAG) == DMA1_FLAG_TC3) || \
                                 ((FLAG) == DMA1_FLAG_HT0) || \
                                 ((FLAG) == DMA1_FLAG_HT1) || \
                                 ((FLAG) == DMA1_FLAG_HT2) || \
                                 ((FLAG) == DMA1_FLAG_HT3) || \
                                 ((FLAG) == (DMA1_FLAG_TC0 |DMA1_FLAG_HT0)) || \
                                 ((FLAG) == (DMA1_FLAG_TC1 |DMA1_FLAG_HT1)) || \
                                 ((FLAG) == (DMA1_FLAG_TC2 |DMA1_FLAG_HT2)) || \
                                 ((FLAG) == (DMA1_FLAG_TC3 |DMA1_FLAG_HT3)))
/**
  * @}
  */
  
/** @defgroup DMA_One_Channel_Interrupts
  * @{
  */
typedef enum
{
  DMA_ITx_TC = ((uint8_t)0x02),/*!< Transaction Complete Interrupt  */
  DMA_ITx_HT = ((uint8_t)0x04) /*!< Half Transaction Interrupt*/
}DMA_ITx_TypeDef;

#define IS_DMA_CONFIG_ITX(IT) ((((IT) & 0xF9) == 0x00) && ((IT) != 0x00))
/**
  * @}
  */
  
/** @defgroup DMA_Interrupts 
  * @{
  */
typedef enum
{
  /* Transaction Complete Interrupts*/
  DMA1_IT_TC0 = ((uint8_t)0x12), /*!< Transaction Complete Interrupt Channel 0 */
  DMA1_IT_TC1 = ((uint8_t)0x22), /*!< Transaction Complete Interrupt Channel 1 */
  DMA1_IT_TC2 = ((uint8_t)0x42), /*!< Transaction Complete Interrupt Channel 2 */
  DMA1_IT_TC3 = ((uint8_t)0x82), /*!< Transaction Complete Interrupt Channel 3 */
  /* Half Transaction Interrupts */
  DMA1_IT_HT0 = ((uint8_t)0x14), /*!< Half Transaction Interrupt Channel 0 */
  DMA1_IT_HT1 = ((uint8_t)0x24), /*!< Half Transaction Interrupt Channel 1 */
  DMA1_IT_HT2 = ((uint8_t)0x44), /*!< Half Transaction Interrupt Channel 2 */
  DMA1_IT_HT3 = ((uint8_t)0x84)  /*!< Half Transaction Interrupt Channel 3 */
}DMA_IT_TypeDef;

#define IS_DMA_CLEAR_IT(IT) (((IT) == DMA1_IT_TC0) || \
                             ((IT) == DMA1_IT_TC1) || \
                             ((IT) == DMA1_IT_TC2) || \
                             ((IT) == DMA1_IT_TC3) || \
                             ((IT) == DMA1_IT_HT0) || \
                             ((IT) == DMA1_IT_HT1) || \
                             ((IT) == DMA1_IT_HT2) || \
                             ((IT) == DMA1_IT_HT3) || \
                             ((IT) == (DMA1_IT_TC0|DMA1_IT_HT0)) || \
                             ((IT) == (DMA1_IT_TC1|DMA1_IT_HT1)) || \
                             ((IT) == (DMA1_IT_TC2|DMA1_IT_HT2)) || \
                             ((IT) == (DMA1_IT_TC3|DMA1_IT_HT3)))

#define IS_DMA_GET_IT(IT)(((IT) == DMA1_IT_TC0) || \
                          ((IT) == DMA1_IT_TC1) || \
                          ((IT) == DMA1_IT_TC2) || \
                          ((IT) == DMA1_IT_TC3) || \
                          ((IT) == DMA1_IT_HT0) || \
                          ((IT) == DMA1_IT_HT1) || \
                          ((IT) == DMA1_IT_HT2) || \
                          ((IT) == DMA1_IT_HT3))
/**
  * @}
  */

/**
  * @}
  */
/* Exported constants --------------------------------------------------------*/    
/* Exported macro ------------------------------------------------------------*/
/** @addtogroup DMA_Exported_Macros
  * @{
  */

/** @defgroup DMA_Channels
  * @{
  */
#define IS_DMA_CHANNEL(PERIPH) (((*(uint16_t*)&(PERIPH)) == DMA1_Channel0_BASE)  || \
                                ((*(uint16_t*)&(PERIPH)) == DMA1_Channel1_BASE)  || \
                                ((*(uint16_t*)&(PERIPH)) == DMA1_Channel2_BASE)  || \
                                ((*(uint16_t*)&(PERIPH)) == DMA1_Channel3_BASE))
/**
  * @}
  */


/** @defgroup DMA_Buffer_Size
  * @{
  */
#define IS_DMA_BUFFER_SIZE(SIZE) ((SIZE) > (uint8_t)0x0)

/**
  * @}
  */

/** @defgroup DMA_Timeout
  * @{
  */
#define IS_DMA_TIMEOUT(TIME) ((TIME) < (uint8_t)0x40)

/**
  * @}
  */
  
/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */ 
  
/*  Functions used to set the DMA configuration to the default reset state ****/ 
void DMA_GlobalDeInit(void);
void DMA_DeInit(DMA_Channel_TypeDef* DMA_Channelx);

/* Initialization and Configuration functions *********************************/
void DMA_Init(DMA_Channel_TypeDef* DMA_Channelx,
              uint32_t DMA_Memory0BaseAddr,
              uint16_t DMA_PeripheralMemory1BaseAddr,
              uint8_t DMA_BufferSize,
              DMA_DIR_TypeDef DMA_DIR,
              DMA_Mode_TypeDef DMA_Mode,
              DMA_MemoryIncMode_TypeDef DMA_MemoryIncMode,
              DMA_Priority_TypeDef DMA_Priority,
              DMA_MemoryDataSize_TypeDef DMA_MemoryDataSize );
void DMA_GlobalCmd(FunctionalState NewState);
void DMA_Cmd(DMA_Channel_TypeDef* DMA_Channelx, FunctionalState NewState);
void DMA_SetTimeOut(uint8_t DMA_TimeOut);

/* Data Counter functions *****************************************************/
void DMA_SetCurrDataCounter(DMA_Channel_TypeDef* DMA_Channelx, uint8_t DataNumber);
uint8_t DMA_GetCurrDataCounter(DMA_Channel_TypeDef* DMA_Channelx);

/* Interrupts and flags management functions **********************************/
void DMA_ITConfig(DMA_Channel_TypeDef* DMA_Channelx, DMA_ITx_TypeDef DMA_ITx, FunctionalState NewState);
FlagStatus DMA_GetFlagStatus(DMA_FLAG_TypeDef DMA_FLAG);
void DMA_ClearFlag(DMA_FLAG_TypeDef DMA_FLAG);
ITStatus DMA_GetITStatus(DMA_IT_TypeDef DMA_IT);
void DMA_ClearITPendingBit(DMA_IT_TypeDef DMA_IT);

#endif /*__STM8L15x_DMA_H */

/**
  * @}
  */
  
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
