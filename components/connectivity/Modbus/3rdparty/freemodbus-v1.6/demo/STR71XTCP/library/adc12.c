/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : adc12.c
* Author             : MCD Application Team
* Date First Issued  : 07/31/2003
* Description        : Code sources of ADC12 functions
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
#include "adc12.h"

/*******************************************************************************
* Function Name      : ADC12_Init
* Description        : Initialize the ADC
* Input              : None.
* Return             : None.
*******************************************************************************/
void ADC12_Init(void)
{
  //Initiate ADC12 registers to their reset values
  ADC12->CSR  = 0x00;
  ADC12->CPR = 0x01;
}

/*******************************************************************************
* Function Name      : ADC12_PrescalerConfig
* Description        : Configure the prescaler
* Input              : None.
* Return             : None.
*******************************************************************************/
void ADC12_PrescalerConfig(u32 Adc12_clk)
{
  //Update the Prescaler Register
  ADC12->CPR = (vu16) (RCCU_FrequencyValue(RCCU_PCLK)/(Adc12_clk*512*8));
}

/*******************************************************************************
* Function Name      : ADC12_ITConfig
* Description        : enable or disable the interrupt
* Input              : status=ENABLE=>enable interrupt
*                      status=DISABLE=>disable interrupt
* Return             : None
*******************************************************************************/
void ADC12_ITConfig(FunctionalState NewState)
{
  if (NewState == ENABLE)
  {
    // Test the conversion mode
    if (ADC12->CSR & 0x0040)
    {
      // Set interrupt bit equivalent to the channel selected
      switch (ADC12->CSR & 0x30)
      {
        case 0x00 : ADC12->CSR |= ADC12_IT0_Mask;  break;
        case 0x10 : ADC12->CSR |= ADC12_IT1_Mask;  break;
        case 0x20 : ADC12->CSR |= ADC12_IT2_Mask;  break;
        case 0x30 : ADC12->CSR |= ADC12_IT3_Mask;  break;
      }
    }
    else
      // Set all interrupt bits in case of round robin mode
      ADC12->CSR |= ADC12_IT_Mask;
  }
  else
    // Clear all interrupt bits
    ADC12->CSR &= ~ADC12_IT_Mask;
}
/*********************(c) 2003  STMicroelectronics********************* END OF FILE **/

