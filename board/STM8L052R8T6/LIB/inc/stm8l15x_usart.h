/**
  ********************************************************************************
  * @file    stm8l15x_usart.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the USART firmware
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
#ifndef __STM8L15x_USART_H
#define __STM8L15x_USART_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include "stm8l15x_clk.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
/** @addtogroup USART
  * @{
  */ 
  
/* Exported types ------------------------------------------------------------*/

/** @defgroup USART_Exported_Types
  * @{
  */

/** @defgroup USART_Flags
  * @{
  */
typedef enum 
{ 
 USART_FLAG_TXE   = (uint16_t)0x0080, /*!< Transmit Data Register Empty flag */
 USART_FLAG_TC    = (uint16_t)0x0040, /*!< Transmission Complete flag */
 USART_FLAG_RXNE  = (uint16_t)0x0020, /*!< Read Data Register Not Empty flag */
 USART_FLAG_IDLE  = (uint16_t)0x0010, /*!< Idle line detected flag */
 USART_FLAG_OR    = (uint16_t)0x0008, /*!< OverRun error flag */
 USART_FLAG_NF    = (uint16_t)0x0004, /*!< Noise error flag */
 USART_FLAG_FE    = (uint16_t)0x0002, /*!< Framing Error flag */
 USART_FLAG_PE    = (uint16_t)0x0001, /*!< Parity Error flag */
 USART_FLAG_SBK   = (uint16_t)0x0101  /*!< Send Break characters Flag */
} USART_FLAG_TypeDef;

#define IS_USART_FLAG(Flag) \
  (((Flag) == USART_FLAG_TXE) || \
   ((Flag) == USART_FLAG_TC)  || \
   ((Flag) == USART_FLAG_RXNE) || \
   ((Flag) == USART_FLAG_IDLE) || \
   ((Flag) == USART_FLAG_OR) || \
   ((Flag) == USART_FLAG_NF) || \
   ((Flag) == USART_FLAG_FE) || \
   ((Flag) == USART_FLAG_PE) || \
   ((Flag) == USART_FLAG_SBK))
   
#define IS_USART_CLEAR_FLAG(Flag) (((Flag) == USART_FLAG_TC))
   
/**
  * @}
  */

/** @defgroup USART_Interrupts
  * @{
  */
    
/**
  * @brief USART Interrupt definition
  * USART_IT possible values
  * Elements values convention: 0x0ZYX
  *   X: Position of the corresponding Interrupt
  *   Y: Flag position
  *   Z: Register index
  */
typedef enum 
{ 
  USART_IT_TXE        = (uint16_t)0x0277, /*!< Transmit interrupt */
  USART_IT_TC         = (uint16_t)0x0266, /*!< Transmission Complete interrupt */
  USART_IT_RXNE       = (uint16_t)0x0255, /*!< Receive interrupt */
  USART_IT_IDLE       = (uint16_t)0x0244, /*!< IDLE line interrupt */
  USART_IT_OR         = (uint16_t)0x0235, /*!< Overrun Error interrupt */
  USART_IT_PE         = (uint16_t)0x0100, /*!< Parity Error interrupt */
  USART_IT_ERR        = (uint16_t)0x0500, /*!< Error interrupt */
  USART_IT_NF         = (uint16_t)0x0102, /*!< Noise Error interrupt */
  USART_IT_FE         = (uint16_t)0x0101  /*!< Frame Error interrupt */
} USART_IT_TypeDef;

#define IS_USART_CONFIG_IT(Interrupt) \
  (((Interrupt) == USART_IT_PE) || \
   ((Interrupt) == USART_IT_TXE) || \
   ((Interrupt) == USART_IT_TC) || \
   ((Interrupt) == USART_IT_RXNE) || \
   ((Interrupt) == USART_IT_OR) || \
   ((Interrupt) == USART_IT_ERR) || \
   ((Interrupt) == USART_IT_IDLE))

#define IS_USART_GET_IT(ITPendingBit) \
  (((ITPendingBit) == USART_IT_TXE)  || \
   ((ITPendingBit) == USART_IT_TC)   || \
   ((ITPendingBit) == USART_IT_RXNE) || \
   ((ITPendingBit) == USART_IT_IDLE) || \
   ((ITPendingBit) == USART_IT_OR)  || \
   ((ITPendingBit) == USART_IT_PE))

#define IS_USART_CLEAR_IT(IT) (((IT) == USART_IT_TC) || ((IT) == USART_IT_RXNE))
/**
  * @}
  */
  
/** @defgroup USART_Wakeup_Modes
  * @{
  */
typedef enum
{
  USART_WakeUp_IdleLine       = (uint8_t)0x00, /*!< 0x01 Idle Line wake up */
  USART_WakeUp_AddressMark    = (uint8_t)0x08  /*!< 0x02 Address Mark wake up */
} USART_WakeUp_TypeDef;

#define IS_USART_WAKEUP(WakeUpMode)(((WakeUpMode) == USART_WakeUp_IdleLine) || \
                                    ((WakeUpMode) == USART_WakeUp_AddressMark))
/**
  * @}
  */
  
/** @defgroup USART_Stop_Bits
  * @{
  */
typedef enum
{
  USART_StopBits_1   = (uint8_t)0x00, /*!< One stop bit is transmitted at the end of frame*/
  USART_StopBits_2   = (uint8_t)0x20, /*!< Two stop bits are transmitted at the end of frame*/
  USART_StopBits_1_5 = (uint8_t)0x30  /*!< One and half stop bits*/
} USART_StopBits_TypeDef;

#define IS_USART_STOPBITS(StopBit)(((StopBit) == USART_StopBits_1) || \
                                   ((StopBit) == USART_StopBits_1_5) || \
                                   ((StopBit) == USART_StopBits_2))
/**
  * @}
  */
  
/** @defgroup USART_Parity
  * @{
  */
typedef enum
{
  USART_Parity_No    = (uint8_t)0x00,      /*!< No Parity*/
  USART_Parity_Even  = (uint8_t)0x04,      /*!< Even Parity*/
  USART_Parity_Odd   = (uint8_t)0x06       /*!< Odd Parity*/
} USART_Parity_TypeDef;

#define IS_USART_PARITY(Parity)(((Parity) == USART_Parity_No)   || \
                                ((Parity) == USART_Parity_Even) || \
                                ((Parity) == USART_Parity_Odd ))
/**
  * @}
  */
  
/** @defgroup USART_Lin_Break_Detection_Length 
  * @{
  */
typedef enum
{
  USART_LINBreakDetectionLength_10BITS = (uint8_t)0x00, /*!< 10 bits Lin Break detection */
  USART_LINBreakDetectionLength_11BITS = (uint8_t)0x01  /*!< 11 bits Lin Break detection */
} USART_LINBreakDetectionLength_TypeDef;

/**
  * @}
  */
  
/** @defgroup USART_Word_Length 
  * @{
  */
typedef enum
{
  USART_WordLength_8b = (uint8_t)0x00,  /*!< 8 bits Data */
  USART_WordLength_9b = (uint8_t)0x10   /*!< 9 bits Data */
} USART_WordLength_TypeDef;

#define IS_USART_WORDLENGTH(WordLength) (((WordLength) == USART_WordLength_8b) || \
                                         ((WordLength) == USART_WordLength_9b))
   
/**
  * @}
  */
  
/** @defgroup USART_Mode 
  * @{
  */
typedef enum
{
  USART_Mode_Rx    = (uint8_t)0x04,  /*!< Receive Enable */
  USART_Mode_Tx    = (uint8_t)0x08   /*!< Transmit Enable */
} USART_Mode_TypeDef;

#define IS_USART_MODE(MODE) ((((MODE) & (uint8_t)0xF3) == 0x00) && ((MODE) != (uint16_t)0x00))
/**
  * @}
  */
  
/** @defgroup USART_DMA_Requests 
  * @{
  */
typedef enum
{
  USART_DMAReq_TX    = (uint8_t)0x80,  /*!< Receive DMA request Enable */
  USART_DMAReq_RX    = (uint8_t)0x40   /*!< Transmit DMA request Enable */
} USART_DMAReq_TypeDef;

#define IS_USART_DMAREQ(DMAReq) ((((DMAReq) & (uint8_t)0x3F) == 0x00) && ((DMAReq) != (uint8_t)0x00))
/**
  * @}
  */
  
/** @defgroup USART_IrDA_Mode 
  * @{
  */
typedef enum
{
  USART_IrDAMode_Normal    = (uint8_t)0x00, /*!< IrDA Normal Mode   */
  USART_IrDAMode_LowPower  = (uint8_t)0x01  /*!< IrDA Low Power Mode */
} USART_IrDAMode_TypeDef;

#define IS_USART_IRDAMODE(IrDAMode) (((IrDAMode) == USART_IrDAMode_LowPower) || \
                                     ((IrDAMode) == USART_IrDAMode_Normal))
/**
  * @}
  */
  
/** @defgroup USART_Clock 
  * @{
  */
typedef enum
{
  USART_Clock_Disable    = (uint8_t)0x00,  /*!< CK pin disabled */
  USART_Clock_Enable     = (uint8_t)0x08   /*!< CK pin enabled */
} USART_Clock_TypeDef;

#define IS_USART_CLOCK(CLOCK) (((CLOCK) == USART_Clock_Disable) ||((CLOCK) == USART_Clock_Enable))
/**
  * @}
  */
  
/** @defgroup USART_Clock_Polarity 
  * @{
  */
typedef enum
{
  USART_CPOL_Low      = (uint8_t)0x00,  /*!< CK to 0 when idle */
  USART_CPOL_High     = (uint8_t)0x04   /*!< CK to 1 when idle.*/
} USART_CPOL_TypeDef;

#define IS_USART_CPOL(CPOL) (((CPOL) == USART_CPOL_Low) || ((CPOL) == USART_CPOL_High))
/**
  * @}
  */
  
/** @defgroup USART_Clock_Phase 
  * @{
  */
typedef enum
{
  USART_CPHA_1Edge     = (uint8_t)0x00,  /*!< The first clock transition is the first data capture edge*/
  USART_CPHA_2Edge     = (uint8_t)0x02   /*!< The second clock transition is the first data capture edge*/
} USART_CPHA_TypeDef;

#define IS_USART_CPHA(CPHA) (((CPHA) == USART_CPHA_1Edge) || ((CPHA) == USART_CPHA_2Edge))
/**
  * @}
  */
  
/** @defgroup USART_LastBit 
  * @{
  */
typedef enum
{
  USART_LastBit_Disable  = (uint8_t)0x00,  /*!< The clock pulse of the last data bit is not output to the SCLK pin.*/
  USART_LastBit_Enable   = (uint8_t)0x01   /*!< The clock pulse of the last data bit is output to the SCLK pin.*/
} USART_LastBit_TypeDef;

#define IS_USART_LASTBIT(LASTBIT) (((LASTBIT) == USART_LastBit_Disable) || \
                                   ((LASTBIT) == USART_LastBit_Enable))
/**
  * @}
  */

/**
  * @}
  */
  
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/** @defgroupUSART_Exported_Macros
  * @{
  */
/* BaudRate value should be < 625000 bps */
#define IS_USART_BAUDRATE(NUM) ((NUM) <= (uint32_t)625000)

#define USART_ADDRESS_MAX ((uint8_t)16)
#define IS_USART_ADDRESS(address) ((address) < USART_ADDRESS_MAX)

#define USART_DATA_9BITS_MAX ((uint16_t)0x1FF)
#define IS_USART_DATA_9BITS(DATA) ((DATA) < USART_DATA_9BITS_MAX)

/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */
	
/*  Function used to set the USART configuration to the default reset state ***/ 
void USART_DeInit(USART_TypeDef* USARTx);

/* Initialization and Configuration functions *********************************/
void USART_Init(USART_TypeDef* USARTx, uint32_t BaudRate, USART_WordLength_TypeDef
                USART_WordLength, USART_StopBits_TypeDef USART_StopBits,
                USART_Parity_TypeDef USART_Parity, USART_Mode_TypeDef USART_Mode);
void USART_ClockInit(USART_TypeDef* USARTx, USART_Clock_TypeDef USART_Clock,
                     USART_CPOL_TypeDef USART_CPOL, USART_CPHA_TypeDef USART_CPHA,
                     USART_LastBit_TypeDef USART_LastBit);
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);
void USART_SetPrescaler(USART_TypeDef* USARTx, uint8_t USART_Prescaler);
void USART_SendBreak(USART_TypeDef* USARTx);

/* Data transfers functions ***************************************************/ 
void USART_SendData8(USART_TypeDef* USARTx, uint8_t Data);
void USART_SendData9(USART_TypeDef* USARTx, uint16_t Data);
uint8_t USART_ReceiveData8(USART_TypeDef* USARTx);
uint16_t USART_ReceiveData9(USART_TypeDef* USARTx);

/* Multi-Processor Communication functions ************************************/
void USART_WakeUpConfig(USART_TypeDef* USARTx, USART_WakeUp_TypeDef USART_WakeUp);
void USART_ReceiverWakeUpCmd(USART_TypeDef* USARTx, FunctionalState NewState);
void USART_SetAddress(USART_TypeDef* USARTx, uint8_t USART_Address);

/* Half-duplex mode function **************************************************/
void USART_HalfDuplexCmd(USART_TypeDef* USARTx, FunctionalState NewState);

/* Smartcard mode functions ***************************************************/
void USART_SmartCardCmd(USART_TypeDef* USARTx, FunctionalState NewState);
void USART_SmartCardNACKCmd(USART_TypeDef* USARTx, FunctionalState NewState);
void USART_SetGuardTime(USART_TypeDef* USARTx, uint8_t USART_GuardTime);

/* IrDA mode functions ********************************************************/
void USART_IrDAConfig(USART_TypeDef* USARTx, USART_IrDAMode_TypeDef USART_IrDAMode);
void USART_IrDACmd(USART_TypeDef* USARTx, FunctionalState NewState);

/* DMA transfers management functions *****************************************/
void USART_DMACmd(USART_TypeDef* USARTx, USART_DMAReq_TypeDef USART_DMAReq,
                  FunctionalState NewState);

/* Interrupts and flags management functions **********************************/
void USART_ITConfig(USART_TypeDef* USARTx, USART_IT_TypeDef USART_IT,
                    FunctionalState NewState);
FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, USART_FLAG_TypeDef USART_FLAG);
void USART_ClearFlag(USART_TypeDef* USARTx, USART_FLAG_TypeDef USART_FLAG);
ITStatus USART_GetITStatus(USART_TypeDef* USARTx, USART_IT_TypeDef USART_IT);
void USART_ClearITPendingBit(USART_TypeDef* USARTx, USART_IT_TypeDef USART_IT);



#endif /* __STM8L15x_USART_H */

/**
  * @}
  */
  
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
