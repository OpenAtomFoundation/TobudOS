/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : rtc.h
* Author             : MCD Application Team
* Date First Issued  : 20/05/2003
* Description        : This file contains all the functions prototypes for the
*                      RTC software library.
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
#ifndef __RTC_H
#define __RTC_H

#include "71x_map.h"

typedef enum
{
  RTC_GIR  = 0x0008,
  RTC_OWIR = 0x0004,
  RTC_AIR  = 0x0002,
  RTC_SIR  = 0x0001
} RTC_FLAGS;

typedef enum
{
  RTC_GIT  = 0x0008,
  RTC_OWIT = 0x0004,
  RTC_AIT  = 0x0002,
  RTC_SIT  = 0x0001
} RTC_IT;


/*******************************************************************************
* Function Name  : RTC_CounterClear
* Description    : This routine is used to clear the RTC counter
* Input          : None
* Return         : None
*******************************************************************************/
void RTC_CounterClear (void);

/*******************************************************************************
* Function Name  : RTC_CounterValue
* Description    : This routine is used to get the RTC counter value
* Input          : None
* Return         : The current counter value.
*******************************************************************************/
inline u32 RTC_CounterValue (void)
{
	return ( (u32)RTC->CNTH << 16 ) | RTC->CNTL;
}

/*******************************************************************************
* Function Name  : RTC_CounterConfig
* Description    : This routine is used to update the RTC counter value
* Input          : The new counter value.
* Return         : None
*******************************************************************************/
void RTC_CounterConfig (u32 CounterValue);

/*******************************************************************************
* Function Name  : RTC_PrescalerValue
* Description    : This routine is used to get the RTC prescaler Value
* Input          : None
* Return         : an u32 value that holds the prescaler Value.
*******************************************************************************/
inline u32 RTC_PrescalerValue (void)
{
	return ( (u32)(RTC->PRLH & 0x000F) << 16 ) | RTC->PRLL;
}

/*******************************************************************************
* Function Name  : RTC_PrescalerConfig
* Description    : This routine is used to set the Prescaler Value
* Input          : The New prescaler Value
* Return         : None
*******************************************************************************/
void RTC_PrescalerConfig (u32 Xprescaler);

/*******************************************************************************
* Function Name  : RTC_AlarmValue
* Description    : This routine is used to get the RTC alarm Value
* Input          : None
* Return         : an u32 value that holds the Real Time clock alarm time .
*******************************************************************************/
inline u32 RTC_AlarmValue (void)
{
	return ( (u32)RTC->ALRH << 16 ) | RTC->ALRL;
}

/*******************************************************************************
* Function Name  : RTC_AlarmConfig
* Description    : This routine is used to set the RTC alarm Value
* Input          : an u32 value that holds the Real Time clock alarm time .
* Return         : None
*******************************************************************************/
void RTC_AlarmConfig (u32 Xalarm);

/*******************************************************************************
* Function Name  : RTC_FlagStatus
* Description    : This routine is used to get the RTC flag status
* Input          : an RTC flag RTC_GIR, RTC_OWIR, RTC_AIR or RTC_SIR
* Return         : SET or RESET
*******************************************************************************/
inline FlagStatus RTC_FlagStatus (RTC_FLAGS Xflag)
{
	return ( RTC->CRL & Xflag ) == 0 ? RESET : SET;
}

/*******************************************************************************
* Function Name  : RTC_FlagClear
* Description    : This routine is used to clear the RTC flags
* Input          : an RTC flag RTC_GIR, RTC_OWIR, RTC_AIR or RTC_SIR
* Return         : None
*******************************************************************************/
void RTC_FlagClear (RTC_FLAGS Xflag);

/*******************************************************************************
* Function Name  : RTC_ITConfig
* Description    : This routine is used to configure the RTC interrupts
* Input 1        : a RTC interrupt RTC_GIT, RTC_OWIT, RTC_AIT or RTC_SIT
* Input 2        : ENABLE or DISABLE
* Return         : None
*******************************************************************************/
inline void RTC_ITConfig (RTC_IT Xrtcit, FunctionalState NewState)
{
  if (NewState == ENABLE) RTC->CRH |= Xrtcit; else RTC->CRH &= ~Xrtcit;
}

/*******************************************************************************
* Function Name  : RTC_ITStatus
* Description    : This routine is used to get the RTC interrupts status
* Input          : an RTC interrupt RTC_GIT, RTC_OWIT, RTC_AIT or RTC_SIT
* Return         : ENABLE or DISABLE
*******************************************************************************/
inline FunctionalState RTC_ITStatus (RTC_IT Xrtcit)
{
  return ( RTC->CRH & Xrtcit ) == 0 ? DISABLE : ENABLE;
}

/*******************************************************************************
* Function Name  : RTC_ITClear
* Description    : This routine is used to clear the RTC interrupts
* Input          : an RTC interrupt RTC_GIT, RTC_OWIT, RTC_AIT or RTC_SIT
* Return         : None
*******************************************************************************/
void RTC_ITClear (RTC_IT Xrtcit);

/*******************************************************************************
* Function Name  : RTC_EnterCfgMode
* Description    : This routine is used to enter in the Configuration Mode
* Input          : None
* Return         : None
*******************************************************************************/
void RTC_EnterCfgMode(void);

/*******************************************************************************
* Function Name  : RTC_ExitCfgMode
* Description    : This routine is used to exit from the Configuration Mode
* Input          : None
* Return         : None
*******************************************************************************/
void RTC_ExitCfgMode(void);

/*******************************************************************************
* Function Name  : RTC_WaitForLastTask
* Description    : This routine waits for the last task completetion
* Input          : None
* Return         : None
*******************************************************************************/
void RTC_WaitForLastTask(void);

#endif /* __RTC_H */

/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
