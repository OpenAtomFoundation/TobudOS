/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : bspi.c
* Author             : MCD Application Team
* Date First Issued  : 05/16/2003
* Description        : This file provides all the BSPI software functions
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

#include "bspi.h"

/*******************************************************************************
* Function Name  : BSPI_Init
* Description    : Initializes BSPI peripheral control and registers to their default reset values.
* Input          : BSPIx where x can be 0 or 1 to select the BSPI peripheral.
* Output         : BSPI peripheral registers and BSPI global variables are initialized.
* Return         : None.
*******************************************************************************/
void BSPI_Init(BSPI_TypeDef *BSPIx)
{
  BSPIx->CSR2 = 0x41;
  BSPIx->CSR1 = 0x00;
  BSPIx->CLK  = 0x06;
}

/*******************************************************************************
* Function Name  : BSPI_TrItSrc.
* Description    : Configures the transmit interrupt source.
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI peripheral.
* Input 2        : TrItSrc: specifies the transmit interrupt source.
*		 : Refer to the section ‘Transmit Interrupt Sources’ for more details on the
*		 : allowed values of this parameter.
* Output         : TIE bit in BSPCSR2 register.
* Return         : None.
*******************************************************************************/
void BSPI_TrItSrc(BSPI_TypeDef *BSPIx, BSPI_TR_IT_SRCS TrItSrc)
{
  switch (TrItSrc)
  {
    case BSPI_TR_DIS:
      BSPIx->CSR2 &= ~0xC000;
      break;
	case BSPI_TR_FE:
	  BSPIx->CSR2 &= ~0x8000;
	  BSPIx->CSR2 |= 0x4000;
      break;
	case BSPI_TR_UFL:
	  BSPIx->CSR2 &= ~0x4000;
	  BSPIx->CSR2 |= 0x8000;
	  break;
	case BSPI_TR_FF:
	  BSPIx->CSR2 |= 0xC000;
	  break;
  }
}

/*******************************************************************************
* Function Name  : BSPI_RcItSrc.
* Description    : Configures the receive interrupt source.
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI peripheral.
* Input 2        : RcItSrc: specifies the source for the receive interrupt.
*		 : Refer to the section ‘Receive Interrupt Sources’ for more details on the
*		 : allowed values of this parameter.
* Output         : RIE bit in BSPCSR1 is register.
* Return         : None.
*******************************************************************************/
void BSPI_RcItSrc(BSPI_TypeDef *BSPIx, BSPI_RC_IR_SRCS RcItSrc)
{
  switch (RcItSrc)
  {
    case BSPI_RC_DIS:
      BSPIx->CSR1 &= ~0x000C;
      break;
	case BSPI_RC_FNE:
	  BSPIx->CSR1 &= ~0x0008;
	  BSPIx->CSR1 |= 0x0004;
      break;
	case BSPI_RC_FF:
	  BSPIx->CSR1 |= 0x000C;
      break;
  }
}

/*******************************************************************************
* Function Name  : BSPI_TrFifoDepth
* Description    : Configures BSPI transmission FIFO number of words.
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI.
* Input 2        : TDepth:specifies the depth of the transmit FIFO.
* Output         : TFE bit in BSPCSR2 register.
* Return         : None.
*******************************************************************************/
void BSPI_TrFifoDepth(BSPI_TypeDef *BSPIx, u8 TDepth)
{
  if (TDepth > 0 && TDepth < 11)
  {
    TDepth--;
    BSPIx->CSR2 &= (TDepth*0x400) | ~0x3C00;
    BSPIx->CSR2 |= TDepth*0x400;
  }
  else BSPIx->CSR2 &= ~0x3C00;
}

/*******************************************************************************
* Function Name  : BSPI_RcFifoDepth
* Description    : Configures BSPI reception FIFO number of words.
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI peripheral.
* Input 2        : RDepth:specifies the depth of the receive FIFO.
* Output         : RFE bits [3:0] in BSPCSR1 register.
* Return         : None.
*******************************************************************************/
void BSPI_RcFifoDepth(BSPI_TypeDef *BSPIx, u8 RDepth)
{
  if (RDepth > 0 && RDepth < 11)
  {
    RDepth--;
    BSPIx->CSR1 &= (RDepth*0x1000) | ~0xF000;
    BSPIx->CSR1 |= RDepth*0x1000;
  }
  else BSPIx->CSR1 &= ~0xF000;
}

/*******************************************************************************
* Function Name  : BSPI_BufferSend.
* Description    : Transmits data from a buffer.
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI peripheral.
* Input 2        : PtrToBuffer is an ‘u8’ pointer to the first word of the buffer to be transmitted.
* Input 3        : NbOfWords parameter indicates the number of words saved in the buffer to be sent.
* Output         : None.
* Return         : None.
*******************************************************************************/
void  BSPI_BufferSend(BSPI_TypeDef *BSPIx, u8 *PtrToBuffer, u8 NbOfWords)
{
  vu8 SendWord = 0;
  while (SendWord < NbOfWords)
  {
    BSPI_WordSend(BSPIx, *(PtrToBuffer+SendWord));
    SendWord++;
  }
}

/*******************************************************************************
* Function Name  : BSPI_BufferReceive.
* Description    : Receives number of data words and stores them in user defined area.
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI peripheral.
* Input 2        : PtrToBuffer is an ‘u8’ pointer to the first word of the defined area to save the received buffer.
* Input 3        : NbOfWords parameter indicates the number of words to be received in the buffer.
* Output         : None.
* Return         : None.
*******************************************************************************/
void BSPI_BufferReceive(BSPI_TypeDef *BSPIx, u8 *PtrToBuffer, u8 NbOfWords)
{
  vu16 ReceiveWord = 0;
  while (ReceiveWord < NbOfWords)
  {
    *(PtrToBuffer+ReceiveWord) = BSPI_WordReceive(BSPIx);
    ReceiveWord++;
  }
}

/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
