/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : i2c.h
* Author             : MCD Application Team
* Date First Issued  : 05/09/2003
* Description        : This file provides prototypes of I2C functions
*                      and variable used in code sources
********************************************************************************
* History:
*  24/05/2005 : V3.0
*  30/11/2004 : V2.0
*  14/07/2004 : V1.3
*  01/01/2004 : V1.2
*******************************************************************************
 THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
 CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
 OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#ifndef __i2c_H
#define __i2c_H

#include "71x_map.h"

typedef enum
{
  I2C_CR   = 0x00,
  I2C_SR1  = 0x04,
  I2C_SR2  = 0x08,
  I2C_CCR  = 0x0C,
  I2C_OAR1 = 0x10,
  I2C_OAR2 = 0x14,
  I2C_DR   = 0x18,
  I2C_ECCR = 0x1C
} I2C_Registers;

typedef enum
{
  I2C_Mode10,
  I2C_Mode7
} I2C_Addressing;

typedef enum
{
  I2C_RX,
  I2C_TX
} I2C_Direction;

typedef enum
{
  I2C_SB    = 0x00001,
  I2C_M_SL  = 0x00002,
  I2C_ADSL  = 0x00004,
  I2C_BTF   = 0x00008,
  I2C_BUSY  = 0x00010,
  I2C_TRA   = 0x00020,
  I2C_ADD10 = 0x00040,
  I2C_EVF   = 0x00080,
  I2C_GCAL  = 0x00100,
  I2C_BERR  = 0x00200,
  I2C_ARLO  = 0x00400,
  I2C_STOPF = 0x00800,
  I2C_AF    = 0x01000,
  I2C_ENDAD = 0x02000,
  I2C_STOP  = 0x08000,
  I2C_ACK   = 0x10000,
  I2C_START = 0x20000
} I2C_Flags;

typedef enum
{
  I2C_TX_NO,
  I2C_TX_SB,
  I2C_TX_AF,
  I2C_TX_ARLO,
  I2C_TX_BERR,
  I2C_TX_ADD_OK,
  I2C_TX_DATA_OK,
  I2C_TX_ONGOING
} I2C_Tx_Status;

typedef enum
{
  I2C_RX_NO,
  I2C_RX_SB,
  I2C_RX_AF,
  I2C_RX_ARLO,
  I2C_RX_BERR,
  I2C_RX_ADD_OK,
  I2C_RX_DATA_OK,
  I2C_RX_ONGOING
} I2C_Rx_Status;

#define I2C_PESET_Mask 0x20
#define I2C_PERESET    0xDF
#define I2C_ENGC_Mask  0x10
#define I2C_START_Mask 0x08
#define I2C_STOP_Mask  0x02
#define I2C_ACK_Mask   0x04
#define I2C_ITE_Mask   0x01

/*******************************************************************************
* Function Name  : I2C_Init
* Description    : Initializes I2C peripheral control and registers to their
*                  default reset values.
* Input          : I2Cx ( I2C0 or I2C1 )
* Return         : None.
*******************************************************************************/
void I2C_Init (I2C_TypeDef *I2Cx);

/*******************************************************************************
* Function Name  : I2C_OnOffConfig
* Description    : Enables or disables I2C peripheral.
* Input          : I2Cx ( I2C0 or I2C1 )
*                  condition(ENABLE or DISABLE).
* Return         : None.
*******************************************************************************/
void I2C_OnOffConfig (I2C_TypeDef *I2Cx, FunctionalState NewState);

/*******************************************************************************
* Function Name  : I2C_GeneralCallConfig
* Description    : Enables or disables I2C general call option.
* Input          : I2Cx ( I2C0 or I2C1 )
*                  condition(ENABLE or DISABLE).
* Return         : None.
*******************************************************************************/
INLINE void I2C_GeneralCallConfig (I2C_TypeDef *I2Cx, FunctionalState NewState)
{
  if (NewState == ENABLE) I2Cx->CR |= I2C_ENGC_Mask;
     else I2Cx->CR &= ~I2C_ENGC_Mask;
}

/*******************************************************************************
* Function Name  : I2C_STARTGenerate
* Description    : Generates I2C communication START condition.
* Input          : I2Cx ( I2C0 or I2C1 )
*                  condition(ENABLE or DISABLE).
* Return         : None.
*******************************************************************************/
INLINE void I2C_STARTGenerate (I2C_TypeDef *I2Cx, FunctionalState NewState)
{
  if (NewState == ENABLE) I2Cx->CR |= I2C_START_Mask;
    else I2Cx->CR &= ~I2C_START_Mask;
}

/*******************************************************************************
* Function Name  : I2C_STOPGenerate
* Description    : Generates I2C communication STOP condition.
* Input          : I2Cx ( I2C0 or I2C1 )
*                  condition(ENABLE or DISABLE).
* Return         : None.
*******************************************************************************/
INLINE void I2C_STOPGenerate (I2C_TypeDef *I2Cx, FunctionalState NewState)
{
  if (NewState == ENABLE) I2Cx->CR |= I2C_STOP_Mask;
    else I2Cx->CR &= ~I2C_STOP_Mask;
}

/*******************************************************************************
* Function Name  : I2C_AcknowledgeConfig
* Description    : Enables or disables I2C acknowledge feature.
* Input          : I2Cx ( I2C0 or I2C1 )
*                  condition(ENABLE or DISABLE).
* Return         : None.
*******************************************************************************/
INLINE void I2C_AcknowledgeConfig (I2C_TypeDef *I2Cx, FunctionalState NewState)
{
  if (NewState == ENABLE) I2Cx->CR |= I2C_ACK_Mask;
    else I2Cx->CR &= ~I2C_ACK_Mask;
}

/*******************************************************************************
* Function Name  : I2C_ITConfig
* Description    : Enables or disables I2C interrupt feature.
* Input          : I2Cx ( I2C0 or I2C1 )
*                  condition(ENABLE or DISABLE).
* Return         : None.
*******************************************************************************/
INLINE void I2C_ITConfig (I2C_TypeDef *I2Cx, FunctionalState NewState)
{
  if (NewState == ENABLE) I2Cx->CR |= I2C_ITE_Mask;
    else I2Cx->CR &= ~I2C_ITE_Mask;
}

/*******************************************************************************
* Function Name  : I2C_RegisterRead
* Description    : Reads any I2C register and returns its value.
* Input          : I2Cx ( I2C0 or I2C1 )
*                  Reg :the register to be read (I2CCR,I2CSR1,I2CSR2,I2CCCR,
*                                                I2COAR1,I2COAR2,I2CDR,I2CECCR)
* Return         : the value of the register passed as parameter(u8)
*******************************************************************************/
INLINE u8 I2C_RegisterRead (I2C_TypeDef *I2Cx, I2C_Registers reg)
{
  return (*(u8 *)(*((u32 *)&I2Cx) + reg));
}

/*******************************************************************************
* Function Name  : I2C_FlagStatus
* Description    : Checks whether any I2C Flag is set or not.
* Input          : I2Cx ( I2C0 or I2C1 )
*                  Access(DIRECT or INDIRECT)
*                  Flag : the flag to be read
*                  input 4: an (u8) variable needed in the case
*                                     of the INDIRECT access
* Return         : the NewState of the Flag (SET or RESET).
*******************************************************************************/
FlagStatus I2C_FlagStatus (I2C_TypeDef *I2Cx, RegisterAccess Access, I2C_Flags Flag,...);

/*******************************************************************************
* Function Name  :  I2C_FlagClear
* Description    : Clears the I2C Flag passed as a parameter
* Input          : I2Cx ( I2C0 or I2C1 )
*                  Flag : the flag to be read
*                  input3: an (u8) parameter needed in the case that the flag
*                         to be cleared need a write in one register
* Return         : None.
*******************************************************************************/
void I2C_FlagClear (I2C_TypeDef *I2Cx, I2C_Flags Flag,...);

/*******************************************************************************
* Function Name  : I2C_SpeedConfig
* Description    : Selects I2C clock speed and configures its corresponding mode.
* Input          : I2Cx ( I2C0 or I2C1 )
*                  Clock: I2C expected clock in Hertz.
* Return         : None.
*******************************************************************************/
void I2C_SpeedConfig (I2C_TypeDef *I2Cx, u32 Clock);

/*******************************************************************************
* Function Name  : I2C_AddressConfig
* Description    : Defines the I2C bus address of the interface.
* Input          : I2Cx ( I2C0 or I2C1 ).
*                  Address: an u16 parameter indicating the address
*                           of the interface.
*                  Mode (I2C_Mode10,I2C_Mode7).
* Return         : None.
*******************************************************************************/
void I2C_AddressConfig (I2C_TypeDef *I2Cx, u16 Address, I2C_Addressing Mode);

/*******************************************************************************
* Function Name  : I2C_FCLKConfig
* Description    : Configures frequency bits according to RCLK frequency.
*                  the I2C selected must be disabled
* Input          : I2Cx ( I2C0 or I2C1 )
* Return         : None.
*******************************************************************************/
void I2C_FCLKConfig (I2C_TypeDef *I2Cx);

/*******************************************************************************
* Function Name  : I2C_AddressSend
* Description    : Transmits the address byte to select the slave device.
* Input          : I2Cx ( I2C0 or I2C1 )
*                  Address: an u16 parameter indicating the slave address
*                  Mode (I2C_Mode10,I2C_Mode7).
*                  Direction (I2C_RX,I2C_TX).
* Return         : None.
********************************************************************************/
void I2C_AddressSend (I2C_TypeDef *I2Cx, u16 Address, I2C_Addressing Mode, I2C_Direction Direction);

/*******************************************************************************
* Function Name  : I2C_ByteSend
* Description    : Send a single byte of data.
* Input          : I2Cx ( I2C0 or I2C1 )
*                  Data : the byte to be sent to the slave
* Return         : None.
*******************************************************************************/
void I2C_ByteSend (I2C_TypeDef *I2Cx, u8 Data);

/*******************************************************************************
* Function Name  : I2C_BufferSend
* Description    : Send data from a buffer whose number of bytes is known
* Input          : I2Cx ( I2C0 or I2C1 )
*                  PtrToBuffer :pointer to the byte of buffer to be transmitted.
*                  NbOfBytes:Number of byte of the buffer
* Return         : I2C_Tx_Status :transmission status (I2C_TX_AF, I2C_TX_ARLO,
*                  I2C_TX_BERR,I2C_TX_DATA_OK)
*******************************************************************************/
I2C_Tx_Status I2C_BufferSend (I2C_TypeDef *I2Cx, u8 *PtrToBuffer, u8 NbOfBytes);

/*******************************************************************************
* Function Name  : I2C_StringSend
* Description    : Send data from a buffer
* Input          : I2Cx ( I2C0 or I2C1 )
*                  PtrToString :pointer to the string to be transmitted.
* Return         : I2C_Tx_Status :transmission status (I2C_TX_AF, I2C_TX_ARLO,
*                  I2C_TX_BERR,I2C_TX_DATA_OK)
*******************************************************************************/
I2C_Tx_Status I2C_StringSend (I2C_TypeDef *I2Cx, char *PtrToString);

/*******************************************************************************
* Function Name  : I2C_TransmissionStatus
* Description    : Report the NewState of the transmission
* Input          : I2Cx ( I2C0 or I2C1 )
* Return         : I2C_Tx_Status :transmission status (I2C_TX_NO, I2C_TX_SB,
*                   I2C_TX_AF, I2C_TX_ARLO, I2C_TX_BERR,I2C_TX_ADD_OK,
*                   I2C_TX_DATA_OK,I2C_TX_ONGOING)
*******************************************************************************/
I2C_Tx_Status I2C_TransmissionStatus (I2C_TypeDef *I2Cx);

/*******************************************************************************
* Function Name  : I2C_ByteReceive
* Description    : Returns the received byte.
* Input          : I2Cx ( I2C0 or I2C1 )
* Return         : the byte received
*******************************************************************************/
u8 I2C_ByteReceive (I2C_TypeDef *I2Cx);

/*******************************************************************************
* Function Name  : I2C_BufferReceive
* Description    : received a buffer. and return the status of error.
* Input          : I2Cx ( I2C0 or I2C1 )
*                  PtrToBuffer :pointer to the byte of buffer received.
*                  NbOfBytes:Number of byte to be received
* Return         : I2C_Rx_Status:the NewState of the reception (,I2C_RX_AF,
*                               I2C_RX_ARLO,I2C_RX_BERR, I2C_RX_DATA_OK)
*******************************************************************************/
I2C_Rx_Status I2C_BufferReceive (I2C_TypeDef *I2Cx, u8 *PtrToBuffer, u8 NbOfBytes);

/*******************************************************************************
* Function Name  :I2C_ReceptionStatus
* Description    : Report the reception NewState.
* Input          : I2Cx ( I2C0 or I2C1 )
* Return         : I2C_Rx_Status:the NewState of the reception ( I2C_RX_NO,
*                  I2C_RX_SB,I2C_RX_AF,I2C_RX_ARLO,I2C_RX_BERR,I2C_RX_ADD_OK,
*                  I2C_RX_DATA_OK, I2C_RX_ONGOING)
*******************************************************************************/
I2C_Rx_Status I2C_ReceptionStatus (I2C_TypeDef *I2Cx);

/*******************************************************************************
* Function Name  : I2C_ErrorClear
* Description    : Clears any error flags.
* Input          : I2Cx ( I2C0 or I2C1 )
* Return         : None.
*******************************************************************************/
INLINE void I2C_ErrorClear (I2C_TypeDef *I2Cx)
{
  // Clear all error flags by reading the SR2 register
  (void)I2Cx->SR2;
}

/*******************************************************************************
* Function Name  : I2C_GetStatus
* Description    : Reads the I2C status registers.
* Input          : None
* Return         : I2C status registers.
*******************************************************************************/
INLINE u32 I2C_GetStatus(I2C_TypeDef *I2Cx)
{
 return (((I2Cx->CR&0x04)<<14)|(I2Cx->SR1|(I2Cx->SR2<<8))&0x3FFF);
}

#endif /* __i2c_H */

/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
