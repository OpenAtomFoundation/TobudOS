/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : flash.c
* Author             : MCD Application Team
* Date First Issued  : 07/28/2003
* Description        : This file provides all the Flash software functions
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

#include "flash.h"

  u32 D0_temp;


/*******************************************************************************
* Function Name  : FLASH_Init
* Description    : Initialise the Flash
* Input          : None
* Return         : None
*******************************************************************************/
void FLASH_Init(void)
{
// Reset Flash Control Registers
  FLASHR->CR0 = 0x00000000;
  FLASHR->CR1 = 0x00000000;
// Reset Flash Data Registers
  FLASHR->DR0 = 0xFFFFFFFF;
  FLASHR->DR1 = 0xFFFFFFFF;
// Reset Flash Error Register
  FLASHR->ER  = 0x00000000;
}

/*******************************************************************************
* Function Name  : FLASH_WordWrite
* Description    : Writes a Word to the Flash
* Input 1        : Address of the Destination
* Input 2        : Word To program
* Return         : None
*******************************************************************************/
void FLASH_WordWrite(u32 XtargetAdd, u32 Xdata)
{
  FLASH_WaitForLastTask();
  // set the Word Programming bit 'WPG' in the CR0 Reg
  FLASHR->CR0 |= FLASH_WPG_Mask;
  // Load the destination address in AR
  FLASHR->AR   = XtargetAdd;
  // Load DATA to be programmed in DR0
  FLASHR->DR0  = Xdata;
  // Set the Write Mode Start bit 'WMS' in the CR0 Reg to Start Write Operation
  FLASHR->CR0 |= FLASH_WMS_Mask;
  // Wait until the write operation is completed
  FLASH_WaitForLastTask();
}

/*******************************************************************************
* Function Name  : FLASH_DWordWrite
* Description    : Writes Double Word to the Flash
* Input 1        : Address of the Destination
* Input 2        : Word 1 To program
* Input 3        : Word 2 To program
* Return         : None
*******************************************************************************/
void FLASH_DWordWrite(u32 XtargetAdd, u32 Xdata0, u32 Xdata1)
{

  FLASH_WaitForLastTask();
  // set the Double Word Programming bit 'DWPG' in the CR0 Reg
  FLASHR->CR0 |= FLASH_DWPG_Mask;
  // Load the destination address in AR
  FLASHR->AR   = XtargetAdd;
  // Load DATA0 in DR0 Reg
  FLASHR->DR0  = Xdata0;
  // Load DATA1 in DR1 Reg
  FLASHR->DR1  = Xdata1;
  // Set the Write Mode Start bit 'WMS' in the CR0 Reg to Start Write Operation
  FLASHR->CR0 |= FLASH_WMS_Mask;
  // Wait until the write operation is completed
  FLASH_WaitForLastTask();
}


/*******************************************************************************
* Function Name  : FLASH_SectorErase
* Description    : Erases a Flash sector
* Input 1        : Sectors to be Erased
* Return         : None
*******************************************************************************/
void FLASH_SectorErase(u32 Xsectors)
{
  FLASH_WaitForLastTask();
  // Set the Sector Erase flag 'SER' in the FCRO reg
  FLASHR->CR0 |= FLASH_SER_Mask;
  // Select the Sectors to be erased in the CR1 register
  FLASHR->CR1 |= Xsectors;
  // Set the Write Mode Start bit 'WMS' in the CR0 Reg to Start Erase Operation
  FLASHR->CR0 |= FLASH_WMS_Mask;
  // Wait until the erase operation is completed
  FLASH_WaitForLastTask();
}
/*******************************************************************************
* Function Name  : FLASH_Suspend
* Description    : Suspends the current program or erase operation
* Input 1        : None
* Return         : Flash CR0 register
*******************************************************************************/
u32 FLASH_Suspend(void)
{
  u32 temp;
  /* Save the CR0 Contetnt */
  temp = FLASHR->CR0;
  /* Set The suspend Bit 'SUSP' in the CR0 register */
  FLASHR->CR0 |= FLASH_SUSP_Mask;
  /* Wait until the flash controller acknowlegs the suspend of the current
     operation */
  FLASH_WaitForLastTask();
  return temp;
}
/*******************************************************************************
* Function Name  : FLASH_Resume
* Description    : Resume a Suspended program or erase operation
* Input 1        : The operation to be resumed
* Return         : None
*******************************************************************************/
void FLASH_Resume(u32 FLASH_LastOperation)
{
  // Clear The suspend Bit 'SUSP' in the FRC0 register
  FLASHR->CR0 &= ~FLASH_SUSP_Mask;
  // Resume last operation
  FLASHR->CR0 = FLASH_LastOperation & 0x7FFFFFFF;
  // Set write mode bit
  FLASHR->CR0  |= FLASH_WMS_Mask;
}

/*******************************************************************************
* Function Name  : FLASH_WordRead
* Description    : Read a single word of the flash
* Input 1        : Source Address
* Return         : Word
*******************************************************************************/
u32 FLASH_WordRead(u32 FLASH_SourceAdd)
{
  FLASH_WaitForLastTask();
  // Reads the data from the specified Address
  return *(u32 *)(FLASH_SourceAdd + 0x40000000);
}
/*******************************************************************************
* Function Name  : FLASH_WritePrConfig
* Description    : Configures The Write Protection Bits
* Input 1        : Flash Bank
* Input 2        : Enable or disable Protection
* Return         : None
*******************************************************************************/
void FLASH_WritePrConfig(u32 Xsectors, FunctionalState NewState)
{
  FLASH_WaitForLastTask();
  
  if (NewState == ENABLE)
  {
   // Set the Set protection Bit
    FLASHR->CR0 |= FLASH_SPR_Mask;
    // Set the Register Address
    FLASHR->AR  = 0x4010DFB0;
    // Data To be Programmed to the Protection Register
    FLASHR->DR0  = ~Xsectors;
    // Set the WMS bit to Start the Sequence
    FLASHR->CR0 |= FLASH_WMS_Mask;
  }
  else
  {
    // Set the Set protection Bit
    FLASHR->CR0 |= FLASH_SPR_Mask;
    // Set the Register Address
    FLASHR->AR  = 0x4010DFB0;
    // Data To be Programmed to the Protection Register
    FLASHR->DR0  = FLASHPR->NVWPAR|Xsectors;
    // Set the WMS bit to Start the Sequence
    FLASHR->CR0 |= FLASH_WMS_Mask;
  }
}

/*******************************************************************************
* Function Name  : FLASH_PermanantDebugPrConfig
* Description    : Configures The Debug Protection Bits
* Input 1        : ENABLE or DISABLE
* Return         : Word
*******************************************************************************/
void FLASH_PermanantDebugPrConfig(FunctionalState NewState)
{
  u16 ProtectionLevel = FLASH_ProtectionLevel();


  if (NewState == ENABLE)
  {
    //  If the First Protection Reset the DBGP bit
    if(!ProtectionLevel)
    {
      // Set the Set protection Bit
      FLASHR->CR0 |= FLASH_SPR_Mask;
      // Set the Register Address
      FLASHR->AR  = 0x4010DFB8;
      // Data To be Programmed to the Protection Register
      FLASHR->DR0 = ~2;
      // Set the WMS bit to Start the Sequence
      FLASHR->CR0 |= FLASH_WMS_Mask;
    }
    else
    {
       // Set the Set protection Bit
      FLASHR->CR0 |= FLASH_SPR_Mask;
      // Set the Register Address
      FLASHR->AR  = 0x4010DFBC;
      // Data To be Programmed to the Protection Register
      D0_temp=~(1<<(15+ProtectionLevel));
      FLASHR->DR0 =D0_temp;
      // Set the WMS bit to Start the Sequence
      FLASHR->CR0 |= FLASH_WMS_Mask;
    }

  }
    else if(ProtectionLevel)
   {
      // Set the Set protection Bit
      FLASHR->CR0 |= FLASH_SPR_Mask;
      // Set the Register Address
      FLASHR->AR  = 0x4010DFBC;
      // Data To be Programmed to the Protection Register
      FLASHR->DR0 =~(1<<(ProtectionLevel-1));
      // Set the WMS bit to Start the Sequence
      FLASHR->CR0 |= FLASH_WMS_Mask;
  }

}

/*******************************************************************************
* Function Name  : FLASH_FlagStatus
* Description    : Returns the NewState of Flash flags
* Input 1        : Flash Flag
* Return         : flagstate
*******************************************************************************/
FlagStatus FLASH_FlagStatus(flashflags Xflag)
{
  FlagStatus TmpResult;
  u8 TmpReg;
  u8 TmpPos;
  // get the Register Index
  TmpReg = (Xflag & FLASH_Reg_Mask) >> 5;
  // get the Flag Index
  TmpPos = (Xflag & FLASH_Flag_Mask);

  switch(TmpReg)
  {
    case 0 : // CR0
    {
      // Returns the status of the CR0[TmpPos] flag
      TmpResult = (FLASHR->CR0 & (1<<TmpPos))==0 ? RESET : SET;
      break;
    }
    case 1 : // CR1
    {
      // Returns the status of the CR1[TmpPos] flag
      TmpResult = (FLASHR->CR1 & (1<<TmpPos))==0 ? RESET : SET;
      break;
    }
    case 5 : // ER
    {
      // Returns the status of the ER[TmpPos] flag
      TmpResult = (FLASHR->ER  & (1<<TmpPos))==0 ? RESET : SET;
      break;
    }
  }
  return(TmpResult);
}

/*******************************************************************************
* Function Name  : FLASH_FlagClear
* Description    : Clears a flash flag
* Input 1        : Flash Flag
* Return         : None
*******************************************************************************/
void FLASH_FlagClear(flashflags Xflag)
{
  u8 TmpReg, TmpPos;
  TmpReg = (Xflag & FLASH_Reg_Mask) >> 5;
  TmpPos = (Xflag & FLASH_Flag_Mask);

  switch(TmpReg)
  {
    case 0 : // CR0
    {
      // Clears the status of the CR0[TmpPos] flag
      FLASHR->CR0 &= ~(1<<TmpPos);
      break;
    }
    case 1 : // CR1
    {
      // Clears the status of the CR1[TmpPos] flag
      FLASHR->CR1 &= ~(1<<TmpPos);
      break;
    }
    case 5 : // ER
    {
      // Clears the status of the ER[TmpPos] flag
      FLASHR->ER &= ~(1<<TmpPos);
      break;
    }
  }
}
/*******************************************************************************
* Function Name  : FLASH_ProtectionLevel
* Description    : Gives the level of protection in the PDS PEN registers
* Input 1        : None
* Return         : The last bit not yet reset
*******************************************************************************/
u16 FLASH_ProtectionLevel(void)
{
  u16 TmpBitIndex =0;
  u16 ProtectionRegs;

  if(FLASHPR->NVAPR0&2)
    TmpBitIndex=0;
  else
    TmpBitIndex=1;

  ProtectionRegs=~(FLASHPR->NVAPR1>>16);
  while (((ProtectionRegs) != 0) && (TmpBitIndex < 16))
  {
    ProtectionRegs  = ProtectionRegs >>  1 ;
    TmpBitIndex++;
  }
  return TmpBitIndex;
}
/*******************************************************************************
* Function Name  : Wait For Last Task
* Description    : Waits for the end of last task on a Flash Bank
* Input 1        : None.
* Return         : None
*******************************************************************************/
void FLASH_WaitForLastTask(void)
{
  while (FLASHR->CR0&0x16);
}

/*******************(C)COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
