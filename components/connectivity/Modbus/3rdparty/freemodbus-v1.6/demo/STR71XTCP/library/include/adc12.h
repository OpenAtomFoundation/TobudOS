/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : adc12.h
* Author             : MCD Application Team
* Date First Issued  : 07/31/2003
* Description        : ADC12 functions prototypes
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
#ifndef _ADC12_H
#define _ADC12_H

#include "71x_map.h"
#include "rccu.h"

//--------------------ADC12 Conversion modes-----------------------------------
typedef enum
{
  ADC12_SINGLE,
  ADC12_ROUND
} ADC12_Modes;

//--------------------ADC12 Channels-------------------------------------------
typedef enum
{
  ADC12_CHANNEL0 = 0x00,
  ADC12_CHANNEL1 = 0x10,
  ADC12_CHANNEL2 = 0x20,
  ADC12_CHANNEL3 = 0x30
} ADC12_Channels;

//--------------------ADC12 control status register flag-----------------------
typedef enum
{
  ADC12_DA0 = 0x01,
  ADC12_DA1 = 0x02,
  ADC12_DA2 = 0x04,
  ADC12_DA3 = 0x08,
  ADC12_OR  = 0x2000
} ADC12_Flags;

// Masks for the update of interrupt bit for channel n
#define ADC12_IT0_Mask   0x0100
#define ADC12_IT1_Mask   0x0200
#define ADC12_IT2_Mask   0x0400
#define ADC12_IT3_Mask   0x0800

//Mask for the update of all the interrupt bit in the CSR
#define ADC12_IT_Mask    0x0F00

//Mask for Selecting mode
#define ADC12_Mode_Mask  0x0040

//Mask for configuring the converter
#define ADC12_Start_Mask 0x0020

/*******************************************************************************
* Function Name      : ADC12_Init
* Description        : Intialize the ADC
* Input              : None.
* Return             : None.
*******************************************************************************/
void ADC12_Init(void);

/*******************************************************************************
* Function Name      : ADC12_ConversionStart
* Description        : start the Conversion.
* Input              : None
* Return             : None
*******************************************************************************/
INLINE void ADC12_ConversionStart (void)
{
  // Set the ADCen bit of the bootconf register
  PCU->BOOTCR |= ADC12_Start_Mask;
}

/*******************************************************************************
* Function Name      : ADC12_ConversionStop
* Description        : Disable the ADC
* Input              : None.
* Return             : None.
*******************************************************************************/
INLINE void ADC12_ConversionStop(void)
{
  // Clear the ADCen bit of the bootconf register
  PCU->BOOTCR &= ~ADC12_Start_Mask;
}

/*******************************************************************************
* Function Name      : ADC12_ModeConfig
* Description        : Configure the mode of conversion
* Input              : ADC12_SINGLE: single channel mode
*                      ADC12_ROUND : round robin mode
* Return             : None
*******************************************************************************/
INLINE void ADC12_ModeConfig (ADC12_Modes ConversionMode)
{
  // Select the mode of conversion and update the CSR[6]
  ADC12->CSR= ConversionMode == ADC12_SINGLE ? ADC12->CSR | ADC12_Mode_Mask :\
                                               ADC12->CSR & ~ADC12_Mode_Mask;
}

/*******************************************************************************
* Function Name      : ADC12_PrescalerConfig
* Description        : Configure the prescaler
* Input              : Adc12_clk: Sampling frequency.
* Return             : None.
*******************************************************************************/
void ADC12_PrescalerConfig(u32 Adc12_clk);

/*******************************************************************************
* Function Name      : ADC12_ChannelSelect
* Description        : select the channel passed as parameter to be converted.
* Input              : ADC12_Channel: channel selected to be converted it may be
*                      ADC12_CHANNEL0 : select channel 0
*                      ADC12_CHANNEL1 : select channel 1
*                      ADC12_CHANNEL2 : select channel 2
*                      ADC12_CHANNEL3 : select channel 3
* Return             : None
*******************************************************************************/
INLINE void ADC12_ChannelSelect(ADC12_Channels ADC12_Channel)
{
  // Update the CSR by the value of the selected channel
  ADC12->CSR |= ADC12_Channel;
}

/*******************************************************************************
* Function Name      : ADC12_FlagStatus
* Description        : test if the flag passed in parameter is set or not
* Input              : ADC12_DA0 :Data Available on Channel 0
*                      ADC12_DA1 :Data Available on Channel 1
*                      ADC12_DA2 :Data Available on Channel 2
*                      ADC12_DA3 :Data Available on Channel 3
*                      ADC12_OR  :Overrun
* Return             : SET: if the flag is set
*                      RESET: if the flag is clear
*******************************************************************************/
INLINE FlagStatus ADC12_FlagStatus (ADC12_Flags flag)
{
  // Test on the flag status and return set or RESET
  return ADC12->CSR & flag ? SET : RESET;
}

/*******************************************************************************
* Function Name      : ADC12_ConversionValue
* Description        : Read the conversion result from the data register.
* Input              : ADC12_Channel :number of the register to read
*                      ADC12_CHANNEL0 : read the DATA0 register
*                      ADC12_CHANNEL1 : read the DATA1 register
*                      ADC12_CHANNEL2 : read the DATA2 register
*                      ADC12_CHANNEL3 : read the DATA3 register
* Return             : the register value of the channel converted
*******************************************************************************/
INLINE u16 ADC12_ConversionValue( ADC12_Channels ADC12_Channel)
{
  // Clear the corespondent DA bit 
  ADC12->CSR &= ~(1<<(ADC12_Channel >> 4));
  // Only the 12 MSB of the DATAn Register are taken
  return *(u16 *)(ADC12_BASE + (ADC12_Channel >> 1)) >> 4;
}

/*******************************************************************************
* Function Name      : ADC12_ITConfig
* Description        : enable or disable the interruption
* Input              : status=ENABLE=>enable interrupt
*                      status=DISABLE=>disable interrupt
* Return             : None
*******************************************************************************/
void ADC12_ITConfig (FunctionalState NewState);

#endif
