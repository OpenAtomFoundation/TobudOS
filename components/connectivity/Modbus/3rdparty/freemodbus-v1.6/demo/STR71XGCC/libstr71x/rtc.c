/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : rtc.c
* Author             : MCD Application Team
* Date First Issued  : 09/30/2003
* Description        : This file provides all the RTC software functions
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

#include "rtc.h"

/* Second interrupt */
#define RTC_SI_Mask   	0x0001

/* Alarm interrupt */
#define RTC_AI_Mask   	0x0002

/* OverFlow interrupt */
#define RTC_OWI_Mask  	0x0004

/* Global interrupt */
#define RTC_GI_Mask   	0x0008

/* Configuration Flag Mask */
#define RTC_CNF_Mask  	0x0010

/* Operation OFF flag */
#define RTC_RTOFF_Mask	0x0020

/*******************************************************************************
* Function Name  : RTC_CounterConfig
* Description    : This routine is used to update the RTC counter value
* Input          : The new counter value.
* Return         : None
*******************************************************************************/
void RTC_CounterConfig (u32 CounterValue)
{
  /*  Wait For Last Task Completition */
  RTC_WaitForLastTask();

  /*  Enter to configuration mode */
  RTC_EnterCfgMode();

  /* Update the RTC counter register with the new time and date */
  /* Update the CNTL register */
  RTC->CNTL = (u16)CounterValue;

  /*  Wait For Last Task Completition */
  RTC_WaitForLastTask();

  /* Update the CNTH register */
  RTC->CNTH = (u16)(CounterValue  >> 16);

  /*  Wait For Last Task Completition */
  RTC_WaitForLastTask();

  /*  Exit From Configuration Mode */
  RTC_ExitCfgMode ();
}

/*******************************************************************************
* Function Name  : RTC_PrescalerConfig
* Description    : This routine is used to set the RTC 20-bit Prescaler Value
* Input          : The New prescaler Value
* Return         : None
*******************************************************************************/
void RTC_PrescalerConfig (u32 Xprescaler)
{
  if ( RTC_PrescalerValue () != Xprescaler )
  {
    /* Wait For Last Task Completion */
    RTC_WaitForLastTask ();

    /* Enter In Configuration Mode */
    RTC_EnterCfgMode ();

    /* Set the prescaler MSB  part */
    RTC->PRLH = (Xprescaler & 0x000F0000) >> 16;

    /* Wait For Last Task Completion */
    RTC_WaitForLastTask();

    /* Set the prescaler LSB  part */
    RTC->PRLL = (Xprescaler & 0x0000FFFF);

    /* Wait For Last Task Completion */
    RTC_WaitForLastTask();

    /* Exit From Configuration Mode */
    RTC_ExitCfgMode ();
  }
}

/*******************************************************************************
* Function Name  : RTC_AlarmConfig
* Description    : This routine is used to set the RTC alarm Value
* Input          : an u32 value that holds the Real Time clock alarm time.
* Return         : None
*******************************************************************************/
void RTC_AlarmConfig (u32 Xalarm)
{
  /* Wait For Last Task Completion */
  RTC_WaitForLastTask ();

  /* Enter In Configuration Mode */
  RTC_EnterCfgMode ();

  /* Set The MSB part of the Alarm Time */
  RTC->ALRH = (Xalarm & 0xFFFF0000) >> 16;

  /* Wait For Last Task Completion */
  RTC_WaitForLastTask();

  /* Set The LSB part of the Alarm Time */
  RTC->ALRL = (Xalarm & 0x0000FFFF);

  /* Wait For Last Task Completion */
  RTC_WaitForLastTask();

  /* Exit From Configuration Mode */
  RTC_ExitCfgMode ();
}

/*******************************************************************************
* Function Name  : RTC_FlagClear
* Description    : This routine is used to clear the RTC flags
* Input          : an RTC flag RTC_GIR, RTC_OWIR, RTC_AIR or RTC_SIR
* Return         : None
*******************************************************************************/
void RTC_FlagClear (RTC_FLAGS Xflag)
{
  /* Wait For Last Task Completion */
  RTC_WaitForLastTask();

  /* Enter In Configuration Mode */
  RTC_EnterCfgMode();

  /* Clear an RTC flag */
  RTC->CRL &= ~Xflag;

  /* Wait For Last Task Completion */
  RTC_WaitForLastTask();

  /* Exit From Configuration Mode */
  RTC_ExitCfgMode ();
 }

/*******************************************************************************
* Function Name  : RTC_ITClear
* Description    : This routine is used to clear a RTC interrupt pending bit
* Input          : a RTC interrupt: RTC_GIT, RTC_OWIT, RTC_AIT or RTC_SIT
* Return         : None
*******************************************************************************/
void RTC_ITClear (RTC_IT Xrtcit)
{
  /* Wait For Last Task Completion */
  RTC_WaitForLastTask();

  /* Enter In Configuration Mode */
  RTC_EnterCfgMode();

  /* Clears an RTC interrupt */
  RTC->CRL &= ~Xrtcit;

  /* Wait For Last Task Completion */
  RTC_WaitForLastTask ();

  /* Exit From Configuration Mode */
  RTC_ExitCfgMode ();
}

/*******************************************************************************
* Function Name  : RTC_EnterCfgMode
* Description    : This routine is used to enter in the Configuration Mode
* Input          : None
* Return         : None
*******************************************************************************/
void RTC_EnterCfgMode(void)
{
  /* Set the CNF flag to enter in the Configuration Mode */
  RTC->CRL |= RTC_CNF_Mask;

  /* Wait For Last Task Completion */
  RTC_WaitForLastTask ();
}

/*******************************************************************************
* Function Name  : RTC_ExitCfgMode
* Description    : This routine is used to exit from the Configuration Mode
* Input          : None
* Return         : None
*******************************************************************************/
void RTC_ExitCfgMode(void)
{
  /* Reset the CNF flag to exit from the Configuration Mode */
  RTC->CRL &= ~RTC_CNF_Mask;

  /* Wait For Last Task Completion */
  RTC_WaitForLastTask ();
}

/*******************************************************************************
* Function Name  : RTC_WaitForLastTask
* Description    : This routine waits for the last task completion
* Input          : None
* Return         : None
*******************************************************************************/
void RTC_WaitForLastTask(void)
{
  /* Loop until the Last operation Completion */
  while (!(RTC->CRL & RTC_RTOFF_Mask));
}
/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
