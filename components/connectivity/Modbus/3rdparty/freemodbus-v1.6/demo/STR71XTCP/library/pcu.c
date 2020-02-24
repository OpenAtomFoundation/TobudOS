/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : pcu.c
* Author             : MCD Application Team
* Date First Issued  : 09/09/03
* Description        : This file provides all the Power Control Unit functions
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

#include "pcu.h"
#include "rccu.h"

/*******************************************************************************
* Function Name  : PCU_VRConfig
* Description    : This routine is used to configure PCU voltage regultors
* Input 1        : MVR : Main voltage Regulator
                   LPR : Low Power Regulator
* Input 2        : ENABLE : Enable the Voltage Regulator
                   DISABLE: Disable ( ByPass ) the VR
* Return         : None
*******************************************************************************/
void PCU_VRConfig ( PCU_VR Xvr, FunctionalState NewState )
{
  u16 Tmp = PCU->PWRCR;
  switch ( Xvr )
  {
    case PCU_MVR :
        // Configure the Main Voltage Regulator
        if (NewState == DISABLE) Tmp |= PCU_MVR_Mask; else Tmp &= ~PCU_MVR_Mask;
        break;
    case PCU_LPR :
        // Configure the Low power Voltage Regulator
        if (NewState == DISABLE) Tmp |= PCU_LPR_Mask; else Tmp &= ~PCU_LPR_Mask;
        break;
  }
  // Unlock Power Control Register
  PCU->PWRCR |= PCU_WREN_Mask;
  PCU->PWRCR = Tmp | PCU_WREN_Mask;
}

/*******************************************************************************
* Function Name  : PCU_WFIEnter
* Description    : This routine is used to force the Device to enter in WFI mode
* Input 1        : CLOCK2_16 : Clock2_16 as system clock for WFI mode
*                  EXTERNAL  : external clock as system clock for WFI mode
* Input 2        : ENABLE : Enable Low Power Regulator during Wait For Interrupt Mode
*                  DISABLE: Disable Low Power Regulator during Wait For Interrupt Mode
* Input 3        : ENABLE : Enable Low Power Mode during Wait For Interrupt Mode
*                  DISABLE: Disable Low Power Mode during Wait For Interrupt Mode
* Return         : None
*******************************************************************************/
void PCU_WFIEnter ( WFI_CLOCKS Xclock, FunctionalState Xlpr, FunctionalState Xlpm )
{
  u32 Tmp;
  // Enable Low Power Regulator in WFI mode
  Tmp = PCU->PWRCR;
  // Unlock Power Control Register
  PCU->PWRCR |= PCU_WREN_Mask;
  PCU->PWRCR = Xlpr == ENABLE ? Tmp & ~PCU_LPRWFI_Mask : Tmp | PCU_LPRWFI_Mask;
  // WFI Clock Selection
  Tmp = RCCU->CCR;
  RCCU->CCR = Xclock == WFI_CLOCK2_16  ? Tmp & ~PCU_WFI_CKSEL_Mask : Tmp | PCU_WFI_CKSEL_Mask;
  // Low Power Mode during WFI mode
  Tmp = RCCU->CCR;
  RCCU->CCR = Xlpm == DISABLE ? Tmp & ~PCU_LPOWFI_Mask : Tmp | PCU_LPOWFI_Mask;
  // Enter WFI Mode
  RCCU->SMR &= ~0x0001;
}

/*******************************************************************************
* Function Name  : PCU_LPMEnter
* Description    : This routine is used to force the device to enter in low
*                  power modes
* Input          : PCU_SLOW : Slow Mode
                   PCU_STOP : Stop Mode
                   PCU_STANDBY : StandBy Mode
* Return         : None
*******************************************************************************/
void PCU_LPMEnter ( LPM_MODES Xmode  )
{
 u32 temp;
  switch ( Xmode )
  {
    // Slow Mode
    case PCU_SLOW:
    {
      RCCU->PLL1CR |= 0x87;
      RCCU_RCLKSourceConfig ( RCCU_PLL1_Output );
      break;
    }

    // Stop Mode
    case PCU_STOP:
    {
      // Enable Stop EN bit
      RCCU->CCR |= PCU_STOP_EN_Mask;

      // Write '1' to Stop Bit
      XTI->CTRL |= 0x04;
      // Write '0' to Stop Bit
      XTI->CTRL &= 0x03;
      // Write '1' to Stop Bit
      XTI->CTRL |= 0x04;
      // add Delay
      
      temp = 0;
      temp = 1;
      temp = 2;
      temp = 3;
      temp = 4;
      temp = 5;
      temp = 6;
      temp = 7;
      temp = 8;

      break;

    }
    // PCU_STANDBY Mode
    case PCU_STANDBY:
    {
	  PCU->PWRCR |= PCU_WREN_Mask;	// Unlock Power Control Register
	  PCU->PWRCR |= PCU_PWRDWN_Mask;	// Set the Power Down flag
    }
  }
}
/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/

