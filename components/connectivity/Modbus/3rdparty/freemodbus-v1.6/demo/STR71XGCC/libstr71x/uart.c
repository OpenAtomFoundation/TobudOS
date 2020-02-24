/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : uart.c
* Author             : MCD Application Team
* Date First Issued  : 06/08/2003
* Description        : This file provides all the UART software functions
********************************************************************************
* History:
*  13/01/2006 : V3.1
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

#include "uart.h"

/*******************************************************************************
* Function Name  : UART_Init
* Description    : This function initializes the selected UART registers to 
*                  their reset values
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Output         : None
* Return         : None
*******************************************************************************/
void UART_Init(UART_TypeDef *UARTx)
{ 
  UARTx->CR = 0x0000;
  UARTx->IER = 0x0000;
  (void)UARTx->RxBUFR;
  UARTx->RxRSTR = 0xFFFF;
  UARTx->TxRSTR = 0xFFFF;
  UARTx->TOR = 0x0000;
  UARTx->GTR = 0x0000;
}

/*******************************************************************************
* Function Name  : UART_BaudRateConfig
* Description    : This function configures the baud rate of the selected UART.
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Input 2        : u32 The baudrate value
* Output         : None
* Return         : None
*******************************************************************************/
void UART_BaudRateConfig(UART_TypeDef *UARTx, u32 BaudRate)
{
  UARTx->BR = (u16)(RCCU_FrequencyValue(RCCU_FCLK)/(16*BaudRate));
}

/*******************************************************************************
* Function Name  : UART_Config
* Description    : This function configures the baudrate, the mode, the data
*                  parity and the number of stop bits of the selected UART.
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Input 2        : u32 The baudrate value
* Input 3        : The parity type, it can be UART_EVEN_PARITY, UART_ODD_PARITY
*		   or UART_NO_PARITY
* Input 4        : The number of stop bits UART_0_5_StopBits, UART_1_StopBits, 
*                  UART_1_5_StopBits or UART_2_StopBits
* Input 5        : The UART mode, it can be
*                  UARTM_8D 	for 8-bit data format
*                  UARTM_7D_P 	for 7-bit data + parity format
*                  UART_9D 	for 9-bit data format
*                  UART_8D_W	for 8-bit data + wake-up bit format
*                  UART_8D_P	for 8-bit data + parity bit format   
* Output         : None
* Return         : None
*******************************************************************************/
void UART_Config(UART_TypeDef *UARTx, u32 BaudRate, UARTParity_TypeDef Parity,
                 UARTStopBits_TypeDef StopBits, UARTMode_TypeDef Mode)
{
  UART_ModeConfig(UARTx, Mode);
  UART_BaudRateConfig(UARTx, BaudRate);
  UART_ParityConfig(UARTx, Parity);
  UART_StopBitsConfig(UARTx, StopBits);
}

/*******************************************************************************
* Function Name  : UART_ItConfig
* Description    : This function enables or disables one or several interrupt
*                  sources of the selected UART.
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Input 2        : The new interrupt flag or flags    
* Input 3        : ENABLE or DISABLE
* Output         : None
* Return         : None
* Note           : The UART interrupt flags are listed in the file uart.h
*******************************************************************************/
void UART_ItConfig(UART_TypeDef *UARTx, u16 UART_Flag, FunctionalState NewState)
{
  if (NewState==ENABLE) UARTx->IER|=UART_Flag; else UARTx->IER&=~UART_Flag;
}

/*******************************************************************************
* Function Name  : UART_FifoConfig
* Description    : This function enables or disables the Rx and Tx FIFOs of
*                  the selected UART.
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Input 2        : ENABLE or DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void UART_FifoConfig(UART_TypeDef *UARTx, FunctionalState NewState)
{
  if (NewState==ENABLE) UARTx->CR|=0x0400; else UARTx->CR&=~0x0400;
}

/*******************************************************************************
* Function Name  : UART_FifoReset
* Description    : This function resets the Rx and the Tx FIFOs of the
*                  selected UART.
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Input 2        : UART_RxFIFO or UART_TxFIFO
* Output         : None
* Return         : None
*******************************************************************************/
void UART_FifoReset(UART_TypeDef *UARTx, UARTFIFO_TypeDef FIFO)
{
  if (FIFO==UART_RxFIFO) UARTx->RxRSTR=0xFFFF; else UARTx->TxRSTR=0xFFFF;
}

/*******************************************************************************
* Function Name  : UART_LoopBackConfig
* Description    : This function enables or disables the loop back mode of
*                  the selected UART.
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Input 2        : ENABLE or DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void UART_LoopBackConfig(UART_TypeDef *UARTx, FunctionalState NewState)
{
  if (NewState==ENABLE) UARTx->CR|=0x0040; else UARTx->CR&=~0x0040;
}

/*******************************************************************************
* Function Name  : UART_RxConfig
* Description    : This function enables or disables the UART data reception.
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Input 2        : ENABLE or DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void UART_RxConfig(UART_TypeDef *UARTx, FunctionalState NewState)
{
  if (NewState==ENABLE) UARTx->CR|=0x0100; else UARTx->CR&=~0x0100;
}

/*******************************************************************************
* Function Name  : UART_OnOffConfig
* Description    : This function sets On/Off the selected UART.
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Input 2        : ENABLE or DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void UART_OnOffConfig(UART_TypeDef *UARTx, FunctionalState NewState)
{
  if (NewState==ENABLE) UARTx->CR|=0x0080; else UARTx->CR&=~0x0080;
}

/*******************************************************************************
* Function Name  : UART_ByteSend
* Description    : This function sends a data byte to the selected UART.
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Input 2        : A pointer on the data byte to send
* Output         : None
* Return         : None
*******************************************************************************/
void UART_ByteSend(UART_TypeDef *UARTx, u8 *Data)
{
 /* if FIFO ENABLED */  
 if (UARTx->CR & 0x0400)

    /* Wait until the TxFIFO contains at least 1 free place */
    while((UARTx->SR & UART_TxFull));

 /* if FIFO DISABLED */  
 else 

    /* Wait until the transmit shift register is empty */
    while (!(UARTx->SR & UART_TxEmpty));

  UARTx->TxBUFR = *Data;
}

/*******************************************************************************
* Function Name  : UART_9BitByteSend
* Description    : This function sends a 9-bit data to the selected UART.
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Input 2        : A pointer on the 9-bit data to send
* Output         : None
* Return         : None
*******************************************************************************/
void UART_9BitByteSend(UART_TypeDef *UARTx, u16 *Data)
{
 /* if FIFO ENABLED */  
 if(UARTx->CR & 0x0400)

    /* Wait until the TxFIFO contains at least 1 free place */    
    while((UARTx->SR & UART_TxFull));

 /* if FIFO DISABLED */  
 else

    /* Wait until the transmit shift register is empty */
    while (!(UARTx->SR & UART_TxEmpty)); 

  UARTx->TxBUFR = ((*Data) & 0x01FF);
}

/*******************************************************************************
* Function Name  : UART_DataSend
* Description    : This function sends several data bytes to the selected UART.
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Input 2        : A pointer on the data to send
* Input 3        : The data length in bytes
* Output         : None
* Return         : None
*******************************************************************************/
void UART_DataSend(UART_TypeDef *UARTx, u8 *Data, u8 DataLength)
{
  
 while(DataLength--)
  {
    UART_ByteSend(UARTx,Data);
    Data++;
  }
}

/*******************************************************************************
* Function Name  : UART_9BitDataSend
* Description    : This function sends several 9-bit data to the selected UART.
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Input 2        : A pointer to the data to send
* Input 3        : The data length 
* Output         : None
* Return         : None
*******************************************************************************/
void UART_9BitDataSend(UART_TypeDef *UARTx, u16 *Data, u8 DataLength)
{
   while(DataLength--)
  {
    UART_9BitByteSend(UARTx,Data);
    Data++;
  }
}

/*******************************************************************************
* Function Name  : UART_StringSend
* Description    : This function sends a string to the selected UART.
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Input 2        : A pointer on the string to send
* Output         : None
* Return         : None
*******************************************************************************/
void UART_StringSend(UART_TypeDef *UARTx, u8 *String)
{
  u8 *Data=String;
  while(*Data != '\0')
    UART_ByteSend(UARTx, Data++);
   *Data='\0';
  UART_ByteSend(UARTx, Data);
}

/*******************************************************************************
* Function Name  : UART_ByteReceive
* Description    : This function gets a data byte from the selected UART.
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Input 2        : A pointer on the data where the data will be stored
* Input 3        : The time-out period
* Output         : The received 8-bit data
* Return         : The UARTx.SR register content
*******************************************************************************/
u16 UART_ByteReceive(UART_TypeDef *UARTx, u8 *Data, u8 TimeOut)
{
   u16 wStatus = 0;

/* reload the Timeout counter */
   UARTx->TOR=TimeOut;

/* while the UART_RxFIFO is empty and no Timeoutidle */
   while (!((wStatus=UARTx->SR) & (UART_TimeOutIdle|UART_RxHalfFull|UART_RxBufFull)));

/* then read the Receive Buffer Register */
   *Data = (u8)UARTx->RxBUFR;

   return wStatus;
}

/*******************************************************************************
* Function Name  : UART_9BitByteReceive
* Description    : This function gets a 9-bit data from the selected UART.
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Input 2        : A pointer on the buffer where the data will be stored
* Input 3        : The time-out period value
* Output         : The received 9-bit data
* Return         : The UARTx.SR register contents
*******************************************************************************/
u16 UART_9BitByteReceive(UART_TypeDef *UARTx, u16 *Data, u8 TimeOut)
{
  u16 wStatus = 0;

/* reload the Timeout counter */
  UARTx->TOR=TimeOut;

/* while the UART_RxFIFO is empty and no Timeoutidle */
  while (!((wStatus=UARTx->SR) & (UART_TimeOutIdle|UART_RxHalfFull|UART_RxBufFull)));
 
/* then read the RxBUFR*/
  *Data = (u16)UARTx->RxBUFR; 
  return wStatus;
}

/*******************************************************************************
* Function Name  : UART_DataReceive
* Description    : This function gets 8 bits data bytes from the selected UART.
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Input 2        : A pointer on the buffer where the data will be stored
* Input 3        : The data length
* Input 4        : The time-out period value
* Output         : The received 8-bit data buffer
* Return         : The UARTx.SR register contents
*******************************************************************************/
u16 UART_DataReceive(UART_TypeDef *UARTx, u8 *Data, u8 DataLength, u8 TimeOut)
{
   u16 wStatus = 0;
  while(DataLength--)
    wStatus=UART_ByteReceive(UARTx,Data++,TimeOut);
  return wStatus;
}

/*******************************************************************************
* Function Name  : UART_9BitDataReceive
* Description    : This function gets several 9-bits data from the selected UART.
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Input 2        : A pointer on the buffer where the data will be stored
* Input 3        : The data length
* Input 4        : The time-out value
* Output         : The received 9-bit data buffer
* Return         : The UARTx.SR register contents
*******************************************************************************/
u16 UART_9BitDataReceive(UART_TypeDef *UARTx, u16 *Data, u8 DataLength, u8 TimeOut)
{ 
 u16 wStatus = 0;
  
  while(DataLength--)
    wStatus=UART_9BitByteReceive(UARTx,Data++,TimeOut);
  return wStatus;
}

/*******************************************************************************
* Function Name  : UART_StringReceive
* Description    : This function gets 8-bit data bytes from the selected UART.
* Input 1        : UARTx (x can be 0,1, 2 or 3) the selected UART
* Input 2        : A pointer on the buffer where the string will be stored
* Output         : The received string
* Return         : The UARTx.SR register contents
*******************************************************************************/
u16 UART_StringReceive(UART_TypeDef *UARTx, u8 *Data)
{
  u8 *pSTRING=Data;
  u16 wStatus = 0;

  do
  {
 /* while the UART_RxFIFO is empty  */
    while (!((wStatus=UARTx->SR) & (UART_RxHalfFull|UART_RxBufFull)));

 /* then read the RxBUFR */
    *(pSTRING++) = (u8)UARTx->RxBUFR;

  } while((*(pSTRING - 1)!=0x0D)&(*(pSTRING - 1)!='\0'));

  *(pSTRING - 1)='\0';

  return wStatus;
}

#ifdef USE_SERIAL_PORT
/*******************************************************************************
* Function Name  : sendchar
* Description    : This function sends a character to the selected UART.
* Input 1        : A pointer on the character to send.
* Output         : None
* Return         : None
*******************************************************************************/
void sendchar( char *ch )
{
   #ifdef USE_UART0
     #define  UARTx  UART0
   #endif /* Use_UART0 */

   #ifdef USE_UART1
     #define  UARTx  UART1
   #endif /* Use_UART1 */

   #ifdef USE_UART2
     #define  UARTx  UART2
   #endif /* Use_UART2 */

   #ifdef USE_UART3
     #define  UARTx  UART3
   #endif /* Use_UART3 */

   UART_ByteSend(UARTx,(u8 *)ch);
}
#endif /* USE_SERIAL_PORT */

/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
