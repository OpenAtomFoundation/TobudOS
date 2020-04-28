/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : adc12.c
* Author             : MCD Application Team
* Date First Issued  : 07/31/2003
* Description        : This file provides all the ADC12 software functions.
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
#include "adc12.h"


/*******************************************************************************
* Function Name      : ADC12_Init
* Description        : This routine is used to initialize the ADC12 registers to 
*                      their reset values
* Input              : None.
* Return             : None.
*******************************************************************************/
void ADC12_Init(void)
{
  /* Initiate ADC12 registers to their reset values */
  ADC12->CSR  = 0x0000;
  ADC12->CPR  = 0x0005;
}

/*******************************************************************************
* Function Name      : ADC12_PrescalerConfig
* Description        : This routine is used to configure the prescaler register
* Input              : Adc12_clk: Sampling frequency in Hz.
* Return             : None.
*******************************************************************************/
void ADC12_PrescalerConfig(u32 Adc12_clk)
{
  /* Update the Prescaler Register */
  ADC12->CPR = (vu16) (RCCU_FrequencyValue(RCCU_PCLK)/(Adc12_clk*512*8));
}

/*******************************************************************************
* Function Name      : ADC12_ITConfig
* Description        : This routine is used to enable or disable the interrupt
                       routine.
* Input              : NewState = ENABLE  => Enable the interrupt routine.
*                      NewState = DISABLE => Disable the interrupt routine.
* Return             : None
*******************************************************************************/
void ADC12_ITConfig(FunctionalState NewState)
{
  if (NewState == ENABLE)
  {
    /* If the single mode is selected */
    if (ADC12->CSR & 0x0040)
    {
      /* Set the corresponding interrupt bit according to the selected channel  */
      switch (ADC12->CSR & 0x30)
      {
        case 0x00 : ADC12->CSR |= ADC12_IT0_Mask;  break;
        case 0x10 : ADC12->CSR |= ADC12_IT1_Mask;  break;
        case 0x20 : ADC12->CSR |= ADC12_IT2_Mask;  break;
        case 0x30 : ADC12->CSR |= ADC12_IT3_Mask;  break;
      }
    }    

    else
      /* Set all interrupt bits in case of round robin mode is selected */
      ADC12->CSR |= ADC12_IT_Mask;
  }
  else
    /* Clear all interrupt bits */
    ADC12->CSR &= ~ADC12_IT_Mask;
}
/*********************(c) 2003  STMicroelectronics********************* END OF FILE **/

