/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : xti.c
* Author             : MCD Application Team
* Date First Issued  : 06/08/2003
* Description        : This file provides all the XTI software functions.
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

#include "xti.h"

/*******************************************************************************
* Function Name  : XTI_Init
* Description    : This routine is used to initialize the XTI cell
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void XTI_Init(void)
{
  XTI->SR=0x00;
  XTI->MRH=0x00;
  XTI->MRL=0x00;
  XTI->TRH=0x00;
  XTI->TRL=0x00;
  XTI->PRH=0x00;
  XTI->PRL=0x00;
  XTI->CTRL=0x00; /* Write the STOP bit twice to reset the STOP state machine */
  XTI->CTRL=0x00;
}

/*******************************************************************************
* Function Name  : XTI_LineModeConfig
* Description    : This routine is used to configure the trigger edge.
* Input 1        : Lines to be configured.
* Input 2        : The trigger edge can be FallingEdge or RisingEdge.
* Output         : None
* Return         : None
*******************************************************************************/
void XTI_LineModeConfig(u16 Lines, XTITriggerEdge_TypeDef TriggerEdge)
{
  if(TriggerEdge == XTI_FallingEdge)
  {
    XTI->TRL &= ~Lines;
    XTI->TRH &= ~(Lines>>8)&0x00FF;
  }
  else
  {
    XTI->TRL |= Lines;
    XTI->TRH |= (Lines>>8)&0x00FF;
  }
}

/*******************************************************************************
* Function Name  : XTI_LineConfig
* Description    : This routine is used to enable and disable lines interrupts
* Input 1        : Lines to be configured.
* Input 2        : NewState can be ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void XTI_LineConfig(u16 Lines, FunctionalState NewState)
{
  if(NewState)
  {
    XTI->MRL |= Lines;
    XTI->MRH |= (Lines>>8)&0x00FF;
  }
  else
  {
    XTI->MRL &= ~Lines;
    XTI->MRH &= ~(Lines>>8)&0x00FF;
  }
}

/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
