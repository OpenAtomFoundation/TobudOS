/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : eic.c
* Author             : MCD Application Team
* Date First Issued  : 06/25/2003
* Description        : This file provides all the EIC software functions
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

#include "eic.h"

/*******************************************************************************
* Function Name  : EIC_Init
* Description    : Initialise the EIC using the load PC instruction
*                  (PC = PC +offset)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_Init(void)
{
  extern u32 T0TIMI_Addr;
  u8 bCounter;
  u32 dOffset=((u32)&T0TIMI_Addr);
  EIC->ICR = 0x00000000; // Disable FIQ and IRQ interrupts
  EIC->IER = 0x00000000; // Disable all channels interrupts
  EIC->IPR = 0xFFFFFFFF; // Clear all pending bits
  EIC->FIR = 0x0000000C; // Disable all FIQ channels interrupts and clear FIQ
                         // channels pending bits
  EIC->CIPR = 0x00000000; // Ste the current priority level to zero
  EIC->IVR  = 0xE59F0000; // IVR = high half of load PC instruction
  dOffset = (dOffset+0x00007E0)<<16;
  for(bCounter=0; bCounter<32; bCounter++) // Initialize SIRn registers
  {
    EIC->SIR[bCounter] = dOffset|0xF0000000;
    dOffset += 0x00000004 << 16;
  }
}

/*******************************************************************************
* Function Name  : EIC_CurrentPriorityLevelConfig
* Description    : Change the current priority level of the srved IRQ routine
* Input 1        : New priority
* Output         : None
* Return         : None
*******************************************************************************/
void EIC_CurrentPriorityLevelConfig(u8 NewPriorityLevel)
{
  u32 dVal;
  dVal = EIC->ICR;              // Save ICR content
  EIC->ICR &= ~0x0001;          // Disable IRQ interrupts.
  EIC->CIPR = NewPriorityLevel; // Change the current priority
  EIC->ICR = dVal;              // Restore ICR content
}

/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
