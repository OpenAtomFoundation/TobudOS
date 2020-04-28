/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : tim.c
* Author             : MCD Application Team
* Date First Issued  : 09/08/2003
* Description        : This file provides all the TIM software functions
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

#include "tim.h"

/*******************************************************************************
* Function Name  : TIM_Init
* Description    : This routine is used to Initialize the TIM peripheral 
*                  registers to their default values.
* Input          : TIM Timer to Initialize(TIM0, TIM1, TIM2, TIM3).
* Return         : None.
*******************************************************************************/
void TIM_Init( TIM_TypeDef *TIMx ) 
{
  TIMx->CR1 = 0x0000;
  TIMx->CR2 = 0x0000;
  TIMx->SR  = 0x0000;
}

/*******************************************************************************
* Function Name  : TIM_ICAPModeConfig
* Description    : This routine is used to configure the input capture feature
* Input          : (1) TIM Timer (TIM0, TIM1, TIM2, TIM3).
*                : (2) Input Capture Channel (TIM_CHANNEL_A, TIM_CHANNEL_B).
*                : (3) Active Edge :(TIM_RISING,TIM_FALLING).
* Return         : None.
*******************************************************************************/
void TIM_ICAPModeConfig ( TIM_TypeDef  *TIMx,
                          TIM_Channels Xchannel,
                          TIM_Clock_Edges  Xedge )
{
  switch (Xchannel)
  {
    case TIM_CHANNEL_A :
      if (Xedge == TIM_RISING) TIMx->CR1 |= TIM_IEDGA_Mask; else TIMx->CR1 &= ~TIM_IEDGA_Mask;
      break;
    case TIM_CHANNEL_B :
      if (Xedge == TIM_RISING) TIMx->CR1 |= TIM_IEDGB_Mask; else TIMx->CR1 &= ~TIM_IEDGB_Mask;
      break;
  }
}

/*******************************************************************************
* Function Name  : TIM_OCMPModeConfig
* Description    : This routine is used to configure the output compare feature.
* Input          : (1) TIM Timer(TIM0, TIM1, TIM2, TIM3).
*                : (2) OCMP Channel (TIM_CHANNEL_A, TIM_CHANNEL_B).
*                : (3) Pulse Length.
*                : (4) OC_Mode (TIM_TIMING,TIM_WAVE).
*                : (5) Level (TIM_HIGH,TIM_LOW).
* Return         : None.
*******************************************************************************/
void TIM_OCMPModeConfig (  TIM_TypeDef  *TIMx,
                           TIM_Channels Xchannel,
                           u16          XpulseLength,
                           TIM_OC_Modes     Xmode,
                           TIM_Logic_Levels Xlevel )
{
  u16 Tmp1 = 0x0000;
  u16 Tmp2 = TIMx->CR2;

  TIMx->CR2 = 0x0000;
  /* Start The TIM Counter */
  TIMx->CR1  = TIM_EN_Mask;
  /* Update the CR2 Register */
  TIMx->CR2  = Tmp2;
  switch ( Xmode )
  {
    case TIM_TIMING :
      /* Output Compare Used only for Internal Timing Operation */
      Tmp1 = Xchannel == TIM_CHANNEL_A ? Tmp1 & ~TIM_OCAE_Mask : Tmp1 & ~TIM_OCBE_Mask;
      break;
    case TIM_WAVE :
      /* Output Compare Used for external wave generation */
      Tmp1 = Xchannel == TIM_CHANNEL_A ? TIM_OCAE_Mask : TIM_OCBE_Mask;
      if ( Xlevel == TIM_HIGH )
        Tmp1 = Xchannel == TIM_CHANNEL_A ? Tmp1 | TIM_OLVLA_Mask  : Tmp1 | TIM_OLVLB_Mask;
      else
        Tmp1 = Xchannel == TIM_CHANNEL_A ? Tmp1 & ~TIM_OLVLA_Mask : Tmp1 & ~TIM_OLVLB_Mask;
      break;
  }
  if ( Xchannel == TIM_CHANNEL_A )
    TIMx->OCAR = (XpulseLength);
  else
    TIMx->OCBR = ( XpulseLength);
    TIMx->CNTR = 0x0000;
    TIMx->CR1 |= Tmp1;
}

/*******************************************************************************
* Function Name  : TIM_OPModeConfig
* Description    : This routine is used to configure the one pulse mode.
* Input          : (1) TIM Timer(TIM0, TIM1, TIM2, TIM3).
*                : (2) XpulseLength      : Length of the pulse.
*                : (3) Level1  :Level during the pulse    (TIM_HIGH,TIM_LOW).    
*                : (4) Level2  :Level after the pulse     (TIM_HIGH,TIM_LOW).
*                : (5) Activation Edge :High or Low on ICAP A (TIM_RISING,
*                                                              TIM_FALLING).
* Return         : None.
*******************************************************************************/
void TIM_OPModeConfig ( TIM_TypeDef      *TIMx,
                        u16              XpulseLength,
                        TIM_Logic_Levels XLevel1,
                        TIM_Logic_Levels XLevel2,
                        TIM_Clock_Edges  Xedge )
{
  u16 Tmp = 0;

  /* Set the Level During the pulse */
  if (XLevel1 == TIM_HIGH) Tmp |= TIM_OLVLB_Mask;
  /* Set the Level after After the pulse */
  if (XLevel2 == TIM_HIGH) Tmp |= TIM_OLVLA_Mask;
  /* Set the Activation Edge on the INCAP 1 */  /* to be verified*/
  if (Xedge == TIM_RISING) Tmp |= TIM_IEDGA_Mask;
  /* Set the Output Compare Function */
  Tmp |= TIM_OCAE_Mask;
  /* Set the One pulse mode */
  Tmp |= TIM_OPM_Mask;
  /* Update the CR1 register Value */
  TIMx->CR1 = Tmp;
  /* Set the Pulse length */
  TIMx->OCAR = XpulseLength;
}

/*******************************************************************************
* Function Name  : TIM_PWMOModeConfig
* Description    : This routine is used to configure the PWM in output mode.
* Input          : (1) TIM Timer(TIM0, TIM1, TIM2, TIM3).
*                : (2) DutyCycle.   
*                : (3) Level 1 : During the Duty Cycle(TIM_HIGH,TIM_LOW).
*                : (4) Full period 
*                : (5) Level 2 : After the pulse(TIM_HIGH,TIM_LOW).
* Return         : None.
*******************************************************************************/
void TIM_PWMOModeConfig ( TIM_TypeDef  *TIMx,
                          u16          XDutyCycle,
                          TIM_Logic_Levels XLevel1,
                          u16          XFullperiod,
                          TIM_Logic_Levels XLevel2)
{
  u16 Tmp = TIMx->CR1;
  /* Set the level during the pulse */
  Tmp = XLevel1 == TIM_HIGH  ? Tmp | TIM_OLVLB_Mask : Tmp & ~TIM_OLVLB_Mask;
  /* Set the level after After the pulse */
  Tmp = XLevel2 == TIM_HIGH  ? Tmp | TIM_OLVLA_Mask : Tmp & ~TIM_OLVLA_Mask;
  /* Set the OCAE */
  Tmp |= TIM_OCAE_Mask;
  /* Set the PWM Bit */
  Tmp |= TIM_PWM_Mask;
  /* Update the CR1 */
  TIMx->CR1 = Tmp;
  /* Set the Duty Cycle value */
  if ( XDutyCycle < 5 ) XDutyCycle = 5;
  TIMx->OCAR = XDutyCycle - 5;
  /* Set the Full Period */
  TIMx->OCBR = XFullperiod - 5;
}

/*******************************************************************************
* Function Name  : TIM_PWMIModeConfig
* Description    : This routine is used to configure the PWM in input mode.
* Input          : (1) TIM Timer(TIM0, TIM1, TIM2, TIM3).
*                : (2) First Activation Edge(TIM_RISING,TIM_FALLING).
* Output         : None.
*******************************************************************************/
void TIM_PWMIModeConfig ( TIM_TypeDef  *TIMx,  TIM_Clock_Edges  Xedge )
{
  u16 Tmp = TIMx->CR1;
  /* Set the first edge Level */
  Tmp = Xedge == TIM_RISING ? Tmp | TIM_IEDGA_Mask : Tmp & ~TIM_IEDGA_Mask;
  /* Set the Second edge Level ( Opposit of the first level )*/
  Tmp = Xedge == TIM_FALLING ? Tmp | TIM_IEDGB_Mask : Tmp & ~TIM_IEDGB_Mask;
  /* Set the PWM I Bit */
  Tmp |= TIM_PWMI_Mask;
  /* Update the CR1 */
  TIMx->CR1 = Tmp;
}

/*******************************************************************************
* Function Name  : TIM_PWMIValue
* Description    : This routine is used to get the PWMI values.
* Input          : TIM Timer(TIM0, TIM1, TIM2, TIM3).
* Return         : PWMI_parameters(Pulse, Period).
*******************************************************************************/
PWMI_parameters TIM_PWMIValue (  TIM_TypeDef  *TIMx )
{
  PWMI_parameters Tmp;
  Tmp.Pulse  = TIMx->ICBR;
  Tmp.Period = TIMx->ICAR;
  return Tmp;
}

/*******************************************************************************
* Function Name  : TIM_CounterConfig
* Description    : This routine is used to configure the Timer conuter. 
* Input          : (1) TIM Timer(TIM0, TIM1, TIM2, TIM3).
*                : (2) Xoperation(TIM_START, TIM_STOP, TIM_CLEAR).
* Return         : None.
*******************************************************************************/
void TIM_CounterConfig ( TIM_TypeDef  *TIMx, TIM_CounterOperations Xoperation)
{
  switch ( Xoperation )
  {
    case TIM_START :
      TIMx->CR1 |= TIM_EN_Mask;
      break;
    case TIM_STOP :
      TIMx->CR1 &= ~TIM_EN_Mask;
      break;
    case TIM_CLEAR :
      TIMx->CNTR = 0x1234;
      break;
  }
}

/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
