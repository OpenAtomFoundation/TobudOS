/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : xti.h
* Author             : MCD Application Team
* Date First Issued  : 08/06/2003
* Description        : This file contains all the functions prototypes for the
*                      XTI software library.
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
#ifndef __XTI_H
#define __XTI_H

#include "71x_map.h"

typedef enum
{
  XTI_WakeUp           = 1,
  XTI_Interrupt        = 2,
  XTI_WakeUpInterrupt  = 3
} XTIMode_TypeDef;

typedef enum
{
  XTI_FallingEdge,
  XTI_RisingEdge
} XTITriggerEdge_TypeDef;

#define XTI_Line0  0x0001
#define XTI_Line1  (XTI_Line0<<1)
#define XTI_Line2  (XTI_Line1<<1)
#define XTI_Line3  (XTI_Line2<<1)
#define XTI_Line4  (XTI_Line3<<1)
#define XTI_Line5  (XTI_Line4<<1)
#define XTI_Line6  (XTI_Line5<<1)
#define XTI_Line7  (XTI_Line6<<1)
#define XTI_Line8  (XTI_Line7<<1)
#define XTI_Line9  (XTI_Line8<<1)
#define XTI_Line10 (XTI_Line9<<1)
#define XTI_Line11 (XTI_Line10<<1)
#define XTI_Line12 (XTI_Line11<<1)
#define XTI_Line13 (XTI_Line12<<1)
#define XTI_Line14 (XTI_Line13<<1)
#define XTI_Line15 (XTI_Line14<<1)

/*******************************************************************************
* Function Name  : XTI_Init
* Description    : Initialize the XIT peripheral to reset state.
* Input 1        : None
* Output         : None
* Return         : None
*******************************************************************************/
void XTI_Init(void);

/*******************************************************************************
* Function Name  : XTI_ModeConfig
* Description    : Configure the XTI Mede
* Input 1        : Mode can be WakeUp, Interrupt or WakeUpInterrupt.
* Input 2        : NewState can be ENABLE ro DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
INLINE void XTI_ModeConfig(XTIMode_TypeDef Mode,FunctionalState NewState)
{
  if (NewState==ENABLE) XTI->CTRL|=Mode; else XTI->CTRL&=~Mode;
}

/*******************************************************************************
* Function Name  : XTI_LineModeConfig
* Description    : Configure the trigger edge.
* Input 1        : Lines to be configured.
* Input 2        : The trigger edge can be FallingEdge or RisingEdge.
* Output         : None
* Return         : None
*******************************************************************************/
void XTI_LineModeConfig(u16 Lines, XTITriggerEdge_TypeDef TriggerEdge);

/*******************************************************************************
* Function Name  : XTI_LineConfig
* Description    : Enable and disable lines interrupts
* Input 1        : Lines to be configured.
* Input 2        : NewState can be ENABLE ro DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void XTI_LineConfig(u16 Lines, FunctionalState NewState);

/*******************************************************************************
* Function Name  : XTI_InterruptLineValue
* Description    : Get the interrupt lines
* Input 1        : None.
* Output         : The interrupts lines numbers
* Return         : None
*******************************************************************************/
INLINE u16 XTI_InterruptLineValue(void)
{
  return XTI->PRL | ((u16)XTI->PRH<<8);
}

/*******************************************************************************
* Function Name  : XTI_PendingBitClear
* Description    : Clear the pending bits
* Input 1        : Bits to be cleared.
* Output         : None
* Return         : None
*******************************************************************************/
INLINE void XTI_PendingBitClear(u16 Lines)
{
  XTI->PRL &= ~Lines;
  XTI->PRH &= ~((Lines>>8)&0x00FF);
}

/*******************************************************************************
* Function Name  : XTI_SoftwareIRQInterruprGenerate
* Description    : Generate a software IRQ interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
INLINE void XTI_SWIRQGenerate(void)
{
  XTI->SR = 0x01;
  XTI->SR = 0x00;
}

#endif /* __XTI_H */

/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
