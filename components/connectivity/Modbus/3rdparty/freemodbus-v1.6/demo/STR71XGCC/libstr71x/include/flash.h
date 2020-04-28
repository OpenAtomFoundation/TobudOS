/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : flash.h
* Author             : MCD Application Team
* Date First Issued  : 28/07/2003
* Description        : This file contains all the functions prototypes for the
*                      Embedded Flash software library.
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
#ifndef __FLASH_H
#define __FLASH_H

#include "71x_map.h"

#define FLASH_B0F0 0x00000001
#define FLASH_B0F1 0x00000002
#define FLASH_B0F2 0x00000004
#define FLASH_B0F3 0x00000008
#define FLASH_B0F4 0x00000010
#define FLASH_B0F5 0x00000020
#define FLASH_B0F6 0x00000040
#define FLASH_B0F7 0x00000080

#define FLASH_B1F0 0x00010000
#define FLASH_B1F1 0x00020000

#define FLASH_B0 (FLASH_B0F0 | FLASH_B0F1 | FLASH_B0F2 | FLASH_B0F3 | FLASH_B0F4 | FLASH_B0F5 | FLASH_B0F6 | FLASH_B0F7)
#define FLASH_B1 (FLASH_B1F0 | FLASH_B1F1)

typedef enum
{
  FLASH_BANK0 = 0x1000000,
  FLASH_BANK1 = 0x2000000
} flashbanks;

typedef enum
{
  FLASH_BSYA0   =  0x01, /* 000-00001 = 0000 0001 = 0x01 */ /* FLASH_CR0.1 */
  FLASH_BSYA1   =  0x02, /* 000-00010 = 0000 0010 = 0x02 */ /* FLASH_CR0.2 */
  FLASH_LOCK    =  0x04, /* 000-00100 = 0000 0100 = 0x04 */ /* FLASH_CR0.4*/
  FLASH_INTP    =  0x14, /* 000-10100 = 0001 0100 = 0x14 */ /* FLASH_CR0.20 */
  FLASH_B0S     =  0x38, /* 001-11000 = 0011 1000 = 0x38 */ /* FLASH_CR1.24*/
  FLASH_B1S     =  0x39, /* 001-11001 = 0011 1001 = 0x39 */ /* FLASH_CR1.25*/
  FLASH_ERR     =  0xA0, /* 101-00000 = 1010 0000 = 0xA0 */ /* FLASH_ER.0*/
  FLASH_ERER    =  0xA1, /* 101-00001 = 1010 0001 = 0xA1 */ /* FLASH_ER.1*/
  FLASH_PGER    =  0xA2, /* 101-00010 = 1010 0010 = 0xA2 */ /* FLASH_ER.2*/
  FLASH_10ER    =  0xA3, /* 101-00011 = 1010 0011 = 0xA3 */ /* FLASH_ER.3*/
  FLASH_SEQER   =  0xA6, /* 101-00110 = 1010 0110 = 0xA6 */ /* FLASH_ER.6*/
  FLASH_RESER   =  0xA7, /* 101-00111 = 1010 0111 = 0xA7 */ /* FLASH_ER.7*/
  FLASH_WPF     =  0xA8  /* 101-01000 = 1010 1000 = 0xA8 */ /* FLASH_ER.8*/
} flashflags;

#define FLASH_WMS_Mask   0x80000000 
#define FLASH_SUSP_Mask  0x40000000 
#define FLASH_WPG_Mask   0x20000000 
#define FLASH_DWPG_Mask  0x10000000  
#define FLASH_SER_Mask   0x08000000  
#define FLASH_SPR_Mask   0x01000000 
#define FLASH_DBGP_Mask  0x00000002
#define FLASH_ACCP_Mask  0x00000001

#define FLASH_Reg_Mask   0xE0 
#define FLASH_Flag_Mask  0x1F 

#define FLASH_INTM_Mask  0x00200000 


/*******************************************************************************
* Function Name  : FLASH_Init
* Description    : Initialise the Flash
* Input          : None
* Return         : None
*******************************************************************************/
void FLASH_Init(void);

/*******************************************************************************
* Function Name  : FLASH_FlagStatus
* Description    : Returns the NewState of Flash flags
* Input 1        : Flash Flag (FLASH_BSYA0,  FLASH_BSYA1, FLASH_LOCK, FLASH_INTP
*                  FLASH_B0S, FLASH_B1S, FLASH_ERR, FLASH_ERER, FLASH_PGER, 
*                  FLASH_10ER, FLASH_SEQER, FLASH_RESER, FLASH_WPF)  
* Return         : flagstatus (SET or RESET)
*******************************************************************************/
FlagStatus FLASH_FlagStatus(flashflags Xflag);

/*******************************************************************************
* Function Name  : FLASH_WriteOpStatus
* Description    : Checks the write operation status
* Input 1        : Flash bank (FLASH_BANK0 or FLASH_BANK1)
* Return         : write operation status (Write operation is going on if 
*                  FlagStatus is SET and write operation is completed if
*                  FlagStatus is RESET)
*******************************************************************************/
inline FlagStatus FLASH_WriteOpStatus(flashbanks Xbank)
{
  return Xbank == FLASH_BANK0 ? FLASH_FlagStatus(FLASH_BSYA0) : FLASH_FlagStatus(FLASH_BSYA1);
}

/*******************************************************************************
* Function Name  : FLASH_WordWrite
* Description    : Writes a Word to the Flash
* Input 1        : Destination Address
* Input 2        : Word to program
* Return         : None
*******************************************************************************/
void FLASH_WordWrite(u32 TargetAdd, u32 Data);

/*******************************************************************************
* Function Name  : FLASH_DWordWrite
* Description    : Writes Double Word to the Flash
* Input 1        : Destination Address
* Input 2        : Word 1 To program
* Input 3        : Word 2 To program
* Return         : None
*******************************************************************************/
void FLASH_DWordWrite(u32 TargetAdd, u32 Data0, u32 Data1);

/*******************************************************************************
* Function Name  : FLASH_SectorErase
* Description    : Erases a Flash sector
* Input 1        : Sectors to be Erased
* Return         : None
*******************************************************************************/
void FLASH_SectorErase(u32 Xsectors);

/*******************************************************************************
* Function Name  : FLASH_BankErase
* Description    : Erases a Bank of the Flash
* Input 1        : Bank to be Erased
* Return         : None
*******************************************************************************/
inline void FLASH_BankErase(flashbanks Xbank)
{
  FLASH_SectorErase(Xbank == FLASH_BANK0 ? FLASH_B0 : FLASH_B1);
}

/*******************************************************************************
* Function Name  : FLASH_Suspend
* Description    : Suspends the current program or erase operation
* Input 1        : None
* Return         : Flash CR0 register
*******************************************************************************/
u32 FLASH_Suspend(void);

/*******************************************************************************
* Function Name  : FLASH_Resume
* Description    : Resume a Suspended program or erase operation
* Input 1        : The operation to be resumed
* Return         : None
*******************************************************************************/
void FLASH_Resume(u32 FLASH_LastOperation);

/*******************************************************************************
* Function Name  : FLASH_WordRead
* Description    : Read a single word of the flash
* Input 1        : Source Address
* Return         : Word
*******************************************************************************/
u32 FLASH_WordRead(u32 FLASH_SourceAdd);

/*******************************************************************************
* Function Name  : FLASH_FlagClear
* Description    : Clears a flash flag
* Input 1        : Flash Flag
* Return         : None
*******************************************************************************/
void FLASH_FlagClear(flashflags Xflag);

/*******************************************************************************
* Function Name  : FLASH_ITConfig
* Description    : Enables or Disables the write-end interrupt
* Input 1        : FunctionalState(Enable, Disable)
* Return         : None
*******************************************************************************/
inline void FLASH_ITConfig(FunctionalState NewState)
{
  if (NewState == ENABLE) FLASHR->CR0 |= FLASH_INTM_Mask;
  else FLASHR->CR0 &= ~FLASH_INTM_Mask;
}

/*******************************************************************************
* Function Name  : FLASH_ITStatus
* Description    : Checks if the write-end interrupt is enabled or not
* Input 1        : None
* Return         : Enable, Disable
*******************************************************************************/
inline FunctionalState FLASH_ITStatus(void)
{
  return (FLASHR->CR0 & FLASH_INTM_Mask)==0 ? DISABLE : ENABLE;
}

/*******************************************************************************
* Function Name  : FLASH_ITClear
* Description    : Clears an interrupt pending flag
* Input 1        : None
* Return         : None
*******************************************************************************/
inline void FLASH_ITClear(void)
{
  FLASHR->CR0 &= ~FLASH_INTM_Mask;
}

/*******************************************************************************
* Function Name  : FLASH_WritePrConfig
* Description    : Enable Write protection or Disable temporarily Write 
*                  protection of a flash sector.
* Input 1        : Flash Sector.
* Input 2        : Enable or disable Flash sector Write Protection.
* Return         : None.
*******************************************************************************/
void FLASH_WritePrConfig(u32 Xsectors, FunctionalState NewState);

/*******************************************************************************
* Function Name  : FLASH_PermanentDebugPrConfig
* Description    : Configures the Debug Protection Bits
* Input 1        : NewState: ENABLE or DISABLE.
* Return         : None.
*******************************************************************************/
void FLASH_PermanentDebugPrConfig(FunctionalState NewState);

/*******************************************************************************
* Function Name  : FLASH_ProtectionLevel
* Description    : If the flash is Debug protected, up to 16 unprotection/
*                  protection cycles are possible using the NVAPR1 register.
*                  This routine returns the number of times of Debug unprotection. 
* Input          : None.
* Return         : Number of times of Debug Protection/Unprotection(0..15)
*                  Example: if 5 is returned, this means that the flash was debug 
*                  unprotected 5 times(using PDS bits)and debug protected 5 times 
*                  (1 time:using DBGP bit & 4 times: using PEN bits).   
*******************************************************************************/
u16 FLASH_ProtectionLevel(void);

/*******************************************************************************
* Function Name  : FLASH_WaitForLastTask
* Description    : Waits for the end of last task on a Flash Bank
* Input 1        : None
* Return         : Nones
*******************************************************************************/
void FLASH_WaitForLastTask(void);

#endif  /* __FLASH_H */

/*******************(C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
