/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : apb.h
* Author             : MCD Application Team
* Date First Issued  : 05/30/2003
* Description        : This file contains all the functions prototypes for the
*                      APB bridge software library.
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
#ifndef __APB_H
#define __APB_H

#include "71x_map.h"

/* APB1 Peripherals */ 
#define I2C0_Periph  0x0000 
#define I2C1_Periph  0x0002 
#define UART0_Periph 0x0008
#define UART1_Periph 0x0010 
#define UART2_Periph 0x0020 
#define UART3_Periph 0x0040 
#define USB_Periph   0x0080 
#define CAN_Periph   0x0100 
#define BSPI0_Periph 0x0200 
#define BSPI1_Periph 0x0400 
#define HDLC_Periph  0x2000 

/* APB2 Peripherals */ 
#define XTI_Periph   0x0000 
#define GPIO0_Periph 0x0004 
#define GPIO1_Periph 0x0008 
#define GPIO2_Periph 0x0010 
#define ADC12_Periph 0x0040 
#define CKOUT_Periph 0x0080 
#define TIM0_Periph  0x0100 
#define TIM1_Periph  0x0200 
#define TIM2_Periph  0x0400 
#define TIM3_Periph  0x0800 
#define RTC_Periph   0x1000 
#define EIC_Periph   0x4000

/*******************************************************************************
* Function Name  : APB_ClockConfig
* Description    : Enables/Disables the Clock gating for peripherals on the APB
*                  bridge passed in parameters.
* Input          : APBx ( APB1 or APB2 )
*                  NewState ENABLE or DISABLE
*                  NewValue (u16)
* Return         : None
*******************************************************************************/
inline void APB_ClockConfig ( APB_TypeDef *APBx,
                              FunctionalState NewState,
                              u16 NewValue )
{
  if (NewState == ENABLE) APBx->CKDIS &= ~NewValue;
    else APBx->CKDIS |= NewValue;
}

/*******************************************************************************
* Function Name  : APB_SwResetConfig
* Description    : Enables/Disables the software Reset for peripherals on the APB
*                  bridge passed in parameters.
* Input          : APBx ( APB1 or APB2 )
*                  NewState ENABLE or DISABLE
*                  NewValue (u16)
* Return         : None
*******************************************************************************/
inline void APB_SwResetConfig ( APB_TypeDef *APBx,
                                FunctionalState NewState,
                                u16 NewValue )
{
  if (NewState == ENABLE) APBx->SWRES |= NewValue;
    else APBx->SWRES &= ~NewValue;
}

#endif  /* __APB_H */

/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
