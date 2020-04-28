/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : bspi.h
* Author             : MCD Application Team
* Date First Issued  : 16/05/2003
* Description        : This file contains all the functions prototypes for the
*                      BSPI software library.
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

#ifndef __BSPI_H
#define __BSPI_H

#include "71x_map.h"

typedef enum {
  BSPI_BERIT = 0x80,
  BSPI_RCIT  = 0x10,
  BSPI_ALL   = 0x90
} BSPI_ITS;

typedef enum {
  BSPI_BERR = 0x004,
  BSPI_RFNE = 0x008,
  BSPI_RFF  = 0x010,
  BSPI_ROFL = 0x020,
  BSPI_TFE  = 0x040,
  BSPI_TUFL = 0x080,
  BSPI_TFF  = 0x100,
  BSPI_TFNE = 0x200
} BSPI_Flags;

typedef enum {
  BSPI_TR_FE,
  BSPI_TR_UFL,
  BSPI_TR_FF,
  BSPI_TR_DIS
} BSPI_TR_IT_SRCS;

typedef enum {
  BSPI_RC_FNE,
  BSPI_RC_FF,
  BSPI_RC_DIS
} BSPI_RC_IR_SRCS;

#define BSPI_PESET_Mask 0x01
#define BSPI_DFIFO_Mask 0xFFFE
#define BSPI_CPHA_Mask  0x200
#define BSPI_CPOL_Mask  0x100
#define BSPI_BSPI0_Base 0xA000
#define BSPI_BSPI1_Base 0xB000


/*******************************************************************************
* Function Name  : BSPI_BSPI0Conf
* Description    : configure STR71x on BSPI0 mode.
* Input 2        : NewState: specifies the status of the BSPI
*		 : NewState value must be either ENABLE or DISABLE:
*		 : ENABLE: the specified BSPI0 peripheral will be enabled.
*		 : DISABLE: the specified BSPI0 peripheral will be disabled..
* Output         : SPI0 EN  bit  in BOOTCR.
* Return         : None.
*******************************************************************************/
inline void BSPI_BSPI0Conf(FunctionalState NewState)
{
  if (NewState == ENABLE) PCU->BOOTCR |= 0x04; else PCU->BOOTCR &= ~0x04;
}

/*******************************************************************************
* Function Name  : BSPI_Init
* Description    : Initializes BSPI peripheral control and registers to their default reset values.
* Input          : BSPIx where x can be 0 or 1 to select the BSPI peripheral.
* Output         : BSPI peripheral registers and BSPI global variables are initialized.
* Return         : None.
*******************************************************************************/
void BSPI_Init( BSPI_TypeDef *BSPIx);

/*******************************************************************************
* Function Name  : BSPI_Enable
* Description    : Enables/disables the specified BSPI peripheral.
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI peripheral to be enabled or disabled.
* Input 2        : NewState: specifies the status of the BSPI
*		 : NewState value must be either ENABLE or DISABLE:
*		 : ENABLE: the specified BSPI peripheral will be enabled.
*		 : DISABLE: the specified BSPI peripheral will be disabled.
* Output         : BSPE bit  in BSPCSR1.
* Return         : None.
*******************************************************************************/
inline void BSPI_Enable( BSPI_TypeDef *BSPIx, FunctionalState NewState)
{
  if (NewState == ENABLE) BSPIx->CSR1 |= BSPI_PESET_Mask;
    else BSPIx->CSR1 &= ~BSPI_PESET_Mask;
}

/*******************************************************************************
* Function Name  : BSPI_MasterEnable
* Description    : Configures the BSPI as a Master or a Slave.
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI peripheral.
* Input 2        : NewState: specifies whether configuring BSPI as master is enabled or disabled.
*		 : ENABLE: the specified BSPI will be configured as a master.
*		 : DISABLE: the specified BSPI will be configured as a slave.
* Output         : MSTR bit in BSPCSR1 is modified register.
* Return         : None.
*******************************************************************************/
inline void BSPI_MasterEnable(BSPI_TypeDef *BSPIx, FunctionalState NewState)
{
  if (NewState == ENABLE) BSPIx->CSR1 |= 0x02; else BSPIx->CSR1 &= ~0x02;
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
void BSPI_TrItSrc(BSPI_TypeDef *BSPIx, BSPI_TR_IT_SRCS TrItSrc);

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
void BSPI_RcItSrc(BSPI_TypeDef *BSPIx, BSPI_RC_IR_SRCS RcItSrc);

/*******************************************************************************
* Function Name  : BSPI_TrFifoDepth
* Description    : Configures BSPI transmission FIFO number of words.
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI.
* Input 2        : TDepth:specifies the depth of the transmit FIFO.
* Output         : TFE bit in BSPCSR2 register.
* Return         : None.
*******************************************************************************/
void BSPI_TrFifoDepth(BSPI_TypeDef *BSPIx, u8 TDepth);

/*******************************************************************************
* Function Name  : BSPI_RcFifoDepth
* Description    : Configures BSPI reception FIFO number of words.
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI peripheral.
* Input 2        : RDepth:specifies the depth of the receive FIFO.
* Output         : RFE bits [3:0] in BSPCSR1 register.
* Return         : None.
*******************************************************************************/
void BSPI_RcFifoDepth(BSPI_TypeDef *BSPIx, u8 RDepth);

/*******************************************************************************
* Function Name  : BSPI_8bLEn
* Description    : Sets the word length of the receive FIFO and transmit data registers to either 8 or 16 bits.
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI.
* Input 2        : status: specifies if the word length is 8 or 16 bits.
*                : status value must be either ENABLE or DISABLE:
*                : ENABLE: to enable setting the word length to 8 bits.
*                : DISABLE: disables setting the word length to 8 bits: the wordlength will be configured to 16 bits.
* Output         : WL bits in BSPCSR1 register.
* Return         : None.
*******************************************************************************/
inline void BSPI_8bLEn(BSPI_TypeDef *BSPIx, FunctionalState NewState)
{
  if (NewState == DISABLE) BSPIx->CSR1 |= 0x0400; else BSPIx->CSR1 &= ~0x0C00;
}

/*******************************************************************************
* Function Name  : BSPI_ClkFEdge
* Description    : Enables capturing the first data sample on the first edge of SCK or on the second edge.
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI peripheral.
* Input 2        : NewState: specifies whether capturing the first data sample on the first edge of SCK is enabled or disabled.
*		 : status value must be either ENABLE or DISABLE.
* 		 :DISABLE: to enable capturing the first data sample on the first edge of SCK.
* 		 : ENABLE: to enable capturing the first data sample on the second edge of SCK.
* Output         : CPHA bits in BSPCSR1 register.
* Return         : None.
*******************************************************************************/
inline void BSPI_ClkFEdge(BSPI_TypeDef *BSPIx, FunctionalState NewState)
{
  if (NewState == ENABLE) BSPIx->CSR1 |= BSPI_CPHA_Mask;
  else BSPIx->CSR1&=~BSPI_CPHA_Mask;
}

/*******************************************************************************
* Function Name  : BSPI_ClkActiveHigh
* Description    : Configures the clock to be active high or low.
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI peripheral.
* Input 2        : NewState: specifies whether the clock is active high or low.
*		 : status value must be ENABLE or DISABLE.
*		 : ENABLE: configures the clock to be active high.
*		 : DISABLE: confgures the clock to be active low.
* Output         : CPOL bits in BSPCSR1 register.
* Return         : None.
*******************************************************************************/
inline void BSPI_ClkActiveHigh(BSPI_TypeDef *BSPIx, FunctionalState NewState)
{
  if (NewState == ENABLE) BSPIx->CSR1 |= BSPI_CPOL_Mask;
  else BSPIx->CSR1 &= ~BSPI_CPOL_Mask;
}

/*******************************************************************************
* Function Name  : BSPI_FifoDisable
* Description    : Configure the BSPI FIFO.
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI peripheral.
* Output         : DFIFO bit in BSPCSR2 register.
* Return         : None.
*******************************************************************************/
inline void BSPI_FifoDisable(BSPI_TypeDef *BSPIx)
{
  BSPIx->CSR2 |= 0x0001;
}

/*******************************************************************************
* Function Name  : BSPI_ClockDividerConfig
* Description    : Configure BSPI clock divider
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI.
* Input 2        : Div: holds the value of the clock divider.
* Output         : Div [7:0] bits in BSPCLK register.
* Return         : None.
*******************************************************************************/
inline void BSPI_ClockDividerConfig(BSPI_TypeDef *BSPIx, u8 Div)
{
  BSPIx->CLK = Div;
}

/*******************************************************************************
* Function Name  : BSPI_FlagStatus.
* Description    : Check whether the specified BSPI Flag is set or not.
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI peripheral.
* Input 2        : flag: specifies the flag to see the status.
*                : Refer to the section ‘BSPI Flags’ for more details on the allowed values of this parameter.
* Output         : None.
* Return         : FlagStatus type
*		 : SET: if the tested flag is set.
*                 : RESET: if the corresponding flag is reset.
*******************************************************************************/
inline FlagStatus BSPI_FlagStatus(BSPI_TypeDef *BSPIx, BSPI_Flags flag)
{
  return (BSPIx->CSR2 & flag) != 0 ? SET : RESET;
}

/*******************************************************************************
* Function Name  : BSPI_WordSend.
* Description    : Transmit a single Word.
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI peripheral.
* Input 2        : holds the word which will be transmitted.
* Output         : BSPTXR register.
* Return         : None.
*******************************************************************************/
inline void BSPI_WordSend(BSPI_TypeDef *BSPIx, u16 Data)
{
  if ((BSPIx->CSR1 & 0x0400) == 0) Data <<= 8;
  BSPIx->TXR = Data;
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
void BSPI_BufferSend(BSPI_TypeDef *BSPIx, u8 *PtrToBuffer, u8 NbOfWords);

/*******************************************************************************
* Function Name  : BSPI_WordReceive.
* Description    : Returns the recent received word.
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI peripheral.
* Output         : None.
* Return         : The value of the received word.
*******************************************************************************/
inline u16 BSPI_WordReceive(BSPI_TypeDef *BSPIx)
{
  return (BSPIx->CSR1 & 0x0400) == 0 ? BSPIx->RXR >> 8 : BSPIx->RXR;
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
void BSPI_BufferReceive(BSPI_TypeDef *BSPIx, u8 *PtrToBuffer, u8 NbOfWords);

/*******************************************************************************
* Function Name  : BSPI_ItEnable.
* Description    : Enable the specified interrupt.
* Input 1        : BSPIx where x can be 0 or 1 to select the BSPI peripheral.
* Input 2        : BSPI_IT: specifies the BSPI interrupt.
*                : Refer to the section ‘Interrupt Enable’ for more details on the allowed values of this parameter..
* Input 3        : NewState: specified whether the BSPI interrupt is enabled or disabled.
*		 : status value must be either ENABLE or DISABLE.
*		 : ENABLE: to enable interrupt
*		 : DISABLE: to disable interrupt.
* Output         : BSPCSR1 register.
* Return         : None.
*******************************************************************************/
inline void BSPI_ItEnable(BSPI_TypeDef *BSPIx, BSPI_ITS BSPI_IE, FunctionalState NewState)
{
  if (NewState == ENABLE) BSPIx->CSR1 |= BSPI_IE; else BSPIx->CSR1 &= ~BSPI_IE;
}

#endif /* __BSPI_H */

/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
