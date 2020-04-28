/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : can.c
* Author             : MCD Application Team
* Date First Issued  : 10/27/2003
* Description        : This file contains all the functions prototypes for the
*                      CAN bus software library.
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

#include "can.h"
#include "xti.h"
#include "pcu.h"

// macro to format the timing register value from the timing parameters
#define CAN_TIMING(tseg1, tseg2, sjw, brp)	(((tseg2-1) & 0x07) << 12) | (((tseg1-1) & 0x0F) << 8) | (((sjw-1) & 0x03) << 6) | ((brp-1) & 0x3F)

// array of pre-defined timing parameters for standard bitrates
u16 CanTimings[] = {         /* value   bitrate     NTQ  TSEG1  TSEG2  SJW  BRP */
	CAN_TIMING(11, 4, 4, 5), /* 0x3AC4  100 kbit/s  16   11     4      4    5   */
	CAN_TIMING(11, 4, 4, 4), /* 0x3AC3  125 kbit/s  16   11     4      4    4   */
	CAN_TIMING( 4, 3, 3, 4), /* 0x2383  250 kbit/s   8    4     3      3    4   */
	CAN_TIMING(13, 2, 1, 1), /* 0x1C00  500 kbit/s  16   13     2      1    1   */
	CAN_TIMING( 4, 3, 1, 1), /* 0x2300  1 Mbit/s     8    4     3      1    1   */
};

/*******************************************************************************
* Function Name  : CAN_SetBitrate
* Description    : Setup a standard CAN bitrate
* Input 1        : one of the CAN_BITRATE_xxx defines
* Output         : None
* Return         : None
* Note           : CAN must be in initialization mode
*******************************************************************************/
void CAN_SetBitrate(u32 bitrate)
{
  /* write the predefined timing value */
  CAN->BTR = CanTimings[bitrate];

  /* clear the Extended Baud Rate Prescaler */
  CAN->BRPR = 0; 		
}

/*******************************************************************************
* Function Name  : CAN_SetTiming
* Description    : Setup the CAN timing with specific parameters
* Input 1        : Time Segment before the sample point position, from 2 to 16
* Input 2        : Time Segment after the sample point position, from 1 to 8
* Input 3        : Synchronisation Jump Width, from 1 to 4
* Input 4        : Baud Rate Prescaler, from 1 to 1024
* Output         : None
* Return         : None
* Note           : CAN must be in initialization mode
*******************************************************************************/
void CAN_SetTiming(u32 tseg1, u32 tseg2, u32 sjw, u32 brp)
{
	CAN->BTR = CAN_TIMING(tseg1, tseg2, sjw, brp);
	CAN->BRPR = ((brp-1) >> 6) & 0x0F;
}

/*******************************************************************************
* Function Name  : CAN_SleepRequest
* Description    : Request the CAN cell to enter sleep mode
* Input 1        : CAN_WAKEUP_ON_EXT or CAN_WAKEUP_ON_CAN
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SleepRequest(u32 WakeupMode)
{
	/* Wakeup Line 6 is linked to CAN RX pin (port 1.11) */
	/* Wakeup Line 2 is linked to external pin (port 2.8) */
	u32 WakeupLine = (WakeupMode == CAN_WAKEUP_ON_CAN ? XTI_Line6 : XTI_Line2);

	CAN_WaitEndOfTx();

	XTI_Init();

	/* Configure the Wakeup Line mode, select Falling edge (transition to dominant state) */
	XTI_LineModeConfig(WakeupLine, XTI_FallingEdge);

	/* Enable Wake-Up interrupt */
	XTI_LineConfig(WakeupLine, ENABLE);

	/* Enable Wake-Up mode with interrupt */
	XTI_ModeConfig(XTI_WakeUpInterrupt, ENABLE);

	XTI_PendingBitClear(XTI_InterruptLineValue());

	/* Enter STOP mode (resume execution from here) */
	PCU_LPMEnter(PCU_STOP);
}

/*******************************************************************************
* Function Name  : CAN_GetFreeIF
* Description    : Search the first free message interface, starting from 0
* Input          : None
* Output         : None
* Return         : A free message interface number (0 or 1) if found, else 2
*******************************************************************************/
static u32 CAN_GetFreeIF(void)
{
	if ((CAN->sMsgObj[0].CRR & CAN_CRR_BUSY) == 0)
		return 0;
	else if ((CAN->sMsgObj[1].CRR & CAN_CRR_BUSY) == 0)
		return 1;
	else return 2;
}

/*******************************************************************************
* Macro Name     : xxx_ID_MSK, xxx_ID_ARB
* Description    : Form the Mask and Arbitration registers value to filter a 
*                  range of identifiers or a fixed identifier, for standard 
*                  and extended IDs
*******************************************************************************/
#define RANGE_ID_MSK(range_start, range_end)	(~((range_end) - (range_start)))
#define RANGE_ID_ARB(range_start, range_end)	((range_start) & (range_end))

#define FIXED_ID_MSK(id)	RANGE_ID_MSK((id), (id))
#define FIXED_ID_ARB(id)	RANGE_ID_ARB((id), (id))

#define STD_RANGE_ID_MSK(range_start, range_end)	((u16)((RANGE_ID_MSK((range_start), (range_end)) & 0x7FF) << 2))
#define STD_RANGE_ID_ARB(range_start, range_end)	((u16)(RANGE_ID_ARB((range_start), (range_end)) << 2))

#define STD_FIXED_ID_MSK(id)	((u16)((FIXED_ID_MSK(id) & 0x7FF) << 2))
#define STD_FIXED_ID_ARB(id)	((u16)(FIXED_ID_ARB(id) << 2))

#define EXT_RANGE_ID_MSK_L(range_start, range_end)	((u16)(RANGE_ID_MSK((range_start), (range_end)) >> 11))
#define EXT_RANGE_ID_MSK_H(range_start, range_end)	((u16)(STD_RANGE_ID_MSK((range_start), (range_end)) | ((RANGE_ID_MSK((range_start), (range_end)) >> 27) & 0x03)))
#define EXT_RANGE_ID_ARB_L(range_start, range_end)	((u16)(RANGE_ID_ARB((range_start), (range_end)) >> 11))
#define EXT_RANGE_ID_ARB_H(range_start, range_end)	((u16)(STD_RANGE_ID_ARB((range_start), (range_end)) | ((RANGE_ID_ARB((range_start), (range_end)) >> 27) & 0x03)))

#define EXT_FIXED_ID_MSK_L(id)	((u16)(FIXED_ID_MSK(id) >> 11))
#define EXT_FIXED_ID_MSK_H(id)	((u16)(STD_FIXED_ID_MSK(id) | ((FIXED_ID_MSK(id) >> 27) & 0x03)))
#define EXT_FIXED_ID_ARB_L(id)	((u16)(FIXED_ID_ARB(id) >> 11))
#define EXT_FIXED_ID_ARB_H(id)	((u16)(STD_FIXED_ID_ARB(id) | ((FIXED_ID_ARB(id) >> 27) & 0x03)))

/*******************************************************************************
* Function Name  : CAN_SetUnusedMsgObj
* Description    : Configure the message object as unused
* Input 1        : message object number, from 0 to 31
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SetUnusedMsgObj(u32 msgobj)
{
	u32 msg_if;
    while ((msg_if = CAN_GetFreeIF()) == 2);

  	CAN->sMsgObj[msg_if].CMR =  CAN_CMR_WRRD
  	                          | CAN_CMR_MASK
  	                          | CAN_CMR_ARB
  	                          | CAN_CMR_CONTROL
  	                          | CAN_CMR_DATAA
  	                          | CAN_CMR_DATAB;

  	CAN->sMsgObj[msg_if].M1R = 0;
  	CAN->sMsgObj[msg_if].M2R = 0;

  	CAN->sMsgObj[msg_if].A1R = 0;
  	CAN->sMsgObj[msg_if].A2R = 0;

  	CAN->sMsgObj[msg_if].MCR = 0;

  	CAN->sMsgObj[msg_if].DA1R = 0;
  	CAN->sMsgObj[msg_if].DA2R = 0;
  	CAN->sMsgObj[msg_if].DB1R = 0;
  	CAN->sMsgObj[msg_if].DB2R = 0;

  	CAN->sMsgObj[msg_if].CRR = 1 + msgobj;
}

/*******************************************************************************
* Function Name  : CAN_SetTxMsgObj
* Description    : Configure the message object as TX
* Input 1        : message object number, from 0 to 31
* Input 2        : CAN_STD_ID or CAN_EXT_ID
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SetTxMsgObj(u32 msgobj, u32 idType)
{
	u32 msg_if;
    while ((msg_if = CAN_GetFreeIF()) == 2);

  	CAN->sMsgObj[msg_if].CMR =  CAN_CMR_WRRD
  	                          | CAN_CMR_MASK
  	                          | CAN_CMR_ARB
  	                          | CAN_CMR_CONTROL
  	                          | CAN_CMR_DATAA
  	                          | CAN_CMR_DATAB;

	CAN->sMsgObj[msg_if].M1R = 0;
	CAN->sMsgObj[msg_if].A1R = 0;

	if (idType == CAN_STD_ID)
	{
  		CAN->sMsgObj[msg_if].M2R = CAN_M2R_MDIR;
  		CAN->sMsgObj[msg_if].A2R = CAN_A2R_MSGVAL | CAN_A2R_DIR;
  	}
  	else
  	{
  		CAN->sMsgObj[msg_if].M2R = CAN_M2R_MDIR | CAN_M2R_MXTD;
  		CAN->sMsgObj[msg_if].A2R = CAN_A2R_MSGVAL | CAN_A2R_DIR | CAN_A2R_XTD;
  	}

  	CAN->sMsgObj[msg_if].MCR = CAN_MCR_TXIE | CAN_MCR_EOB;

  	CAN->sMsgObj[msg_if].DA1R = 0;
  	CAN->sMsgObj[msg_if].DA2R = 0;
  	CAN->sMsgObj[msg_if].DB1R = 0;
  	CAN->sMsgObj[msg_if].DB2R = 0;

  	CAN->sMsgObj[msg_if].CRR = 1 + msgobj;
}

/*******************************************************************************
* Function Name  : CAN_SetRxMsgObj
* Description    : Configure the message object as RX
* Input 1        : message object number, from 0 to 31
* Input 2        : CAN_STD_ID or CAN_EXT_ID
* Input 3        : low part of the identifier range used for acceptance filtering
* Input 4        : high part of the identifier range used for acceptance filtering
* Input 5        : TRUE for a single receive object or a FIFO receive object that
*                  is the last one of the FIFO
*                  FALSE for a FIFO receive object that is not the last one
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SetRxMsgObj(u32 msgobj, u32 idType, u32 idLow, u32 idHigh, bool singleOrFifoLast)
{
	u32 msg_if;
    while ((msg_if = CAN_GetFreeIF()) == 2);

  	CAN->sMsgObj[msg_if].CMR =  CAN_CMR_WRRD
  	                          | CAN_CMR_MASK
  	                          | CAN_CMR_ARB
  	                          | CAN_CMR_CONTROL
  	                          | CAN_CMR_DATAA
  	                          | CAN_CMR_DATAB;

	if (idType == CAN_STD_ID)
	{
  		CAN->sMsgObj[msg_if].M1R = 0;
  		CAN->sMsgObj[msg_if].M2R = STD_RANGE_ID_MSK(idLow, idHigh);

  		CAN->sMsgObj[msg_if].A1R = 0;
  		CAN->sMsgObj[msg_if].A2R = CAN_A2R_MSGVAL | STD_RANGE_ID_ARB(idLow, idHigh);
  	}
  	else
  	{
  		CAN->sMsgObj[msg_if].M1R = EXT_RANGE_ID_MSK_L(idLow, idHigh);
  		CAN->sMsgObj[msg_if].M2R = CAN_M2R_MXTD | EXT_RANGE_ID_MSK_H(idLow, idHigh);

  		CAN->sMsgObj[msg_if].A1R = EXT_RANGE_ID_ARB_L(idLow, idHigh);
  		CAN->sMsgObj[msg_if].A2R = CAN_A2R_MSGVAL | CAN_A2R_XTD | EXT_RANGE_ID_ARB_H(idLow, idHigh);
  	}

  	CAN->sMsgObj[msg_if].MCR = CAN_MCR_RXIE | CAN_MCR_UMASK | (singleOrFifoLast ? CAN_MCR_EOB : 0);

  	CAN->sMsgObj[msg_if].DA1R = 0;
  	CAN->sMsgObj[msg_if].DA2R = 0;
  	CAN->sMsgObj[msg_if].DB1R = 0;
  	CAN->sMsgObj[msg_if].DB2R = 0;

  	CAN->sMsgObj[msg_if].CRR = 1 + msgobj;
}

/*******************************************************************************
* Function Name  : CAN_InvalidateAllMsgObj
* Description    : Configure all the message objects as unused
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_InvalidateAllMsgObj(void)
{
	u32 i;
	for (i = 0; i < 32; i++)
		CAN_SetUnusedMsgObj(i);
}

/*******************************************************************************
* Function Name  : CAN_Init
* Description    : Initialize the CAN cell and set the bitrate
* Input 1        : any binary value formed from the CAN_CTL_xxx defines
* Input 2        : one of the CAN_BITRATE_xxx defines
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_Init(u8 mask, u32 bitrate)
{
  CAN_EnterInitMode(CAN_CR_CCE | mask);
  CAN_SetBitrate(bitrate);
  CAN_LeaveInitMode();
  CAN_LeaveTestMode();
}

/*******************************************************************************
* Function Name  : CAN_ReleaseMessage
* Description    : Release the message object
* Input 1        : message object number, from 0 to 31
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_ReleaseMessage(u32 msgobj)
{
	u32 i;
    while ((i = CAN_GetFreeIF()) == 2);

	CAN->sMsgObj[i].CMR = CAN_CMR_CLRINTPND | CAN_CMR_TXRQST;
	CAN->sMsgObj[i].CRR = 1 + msgobj;
}

/*******************************************************************************
* Function Name  : CAN_SendMessage
* Description    : Start transmission of a message
* Input 1        : message object number, from 0 to 31
* Input 2        : pointer to the message structure containing data to transmit
* Output         : None
* Return         : 1 if transmission was OK, else 0
*******************************************************************************/
u32 CAN_SendMessage(u32 msgobj, canmsg* pCanMsg)
{
  while (CAN->sMsgObj[0].CRR & CAN_CRR_BUSY);

/*  if (CAN->sMsgObj[0].COMR & CAN_CRQ_BUSY) */
/*	return 0;	// message interface 0 not free */

  CAN->SR &= ~CAN_SR_TXOK;

  /* read the Arbitration and Message Control */
  CAN->sMsgObj[0].CMR = CAN_CMR_ARB | CAN_CMR_CONTROL;

  CAN->sMsgObj[0].CRR = 1 + msgobj;

  while (CAN->sMsgObj[0].CRR & CAN_CRR_BUSY);

  /* update the contents needed for transmission */
  CAN->sMsgObj[0].CMR =   CAN_CMR_WRRD
  	                | CAN_CMR_ARB
  	                | CAN_CMR_CONTROL
  	                | CAN_CMR_DATAA
  	                | CAN_CMR_DATAB;

  if ((CAN->sMsgObj[0].A2R & CAN_A2R_XTD) == 0)
  {
  	/* standard ID */
	CAN->sMsgObj[0].A1R = 0;
	CAN->sMsgObj[0].A2R = (CAN->sMsgObj[0].A2R & 0xE000) | STD_FIXED_ID_ARB(pCanMsg->Id);
  }
  else
  {
  	// extended ID
	CAN->sMsgObj[0].A1R = EXT_FIXED_ID_ARB_L(pCanMsg->Id);
	CAN->sMsgObj[0].A2R = (CAN->sMsgObj[0].A2R & 0xE000) | EXT_FIXED_ID_ARB_H(pCanMsg->Id);
  }

  CAN->sMsgObj[0].MCR = (CAN->sMsgObj[0].MCR & 0xFEF0) | CAN_MCR_NEWDAT | CAN_MCR_TXRQST | pCanMsg->Dlc;

  CAN->sMsgObj[0].DA1R = ((u16)pCanMsg->Data[1]<<8) | pCanMsg->Data[0];
  CAN->sMsgObj[0].DA2R = ((u16)pCanMsg->Data[3]<<8) | pCanMsg->Data[2];
  CAN->sMsgObj[0].DB1R = ((u16)pCanMsg->Data[5]<<8) | pCanMsg->Data[4];
  CAN->sMsgObj[0].DB2R = ((u16)pCanMsg->Data[7]<<8) | pCanMsg->Data[6];

  CAN->sMsgObj[0].CRR = 1 + msgobj;

  return 1;
}

/*******************************************************************************
* Function Name  : CAN_ReceiveMessage
* Description    : Get the message, if received
* Input 1        : message object number, from 0 to 31
* Input 2        : if TRUE, the message object is released when getting the data
*                  if FALSE, the message object is not released
* Input 3        : pointer to the message structure where received data is stored
* Output         : None
* Return         : 1 if reception was OK, else 0 (no message pending)
*******************************************************************************/
u32 CAN_ReceiveMessage(u32 msgobj, bool release, canmsg* pCanMsg)
{
    if (!CAN_IsMessageWaiting(msgobj))
    	return 0;

	CAN->SR &= ~CAN_SR_RXOK;

    /* read the message contents */
    CAN->sMsgObj[1].CMR =   CAN_CMR_MASK
	                  | CAN_CMR_ARB
	                  | CAN_CMR_CONTROL
	                  | CAN_CMR_CLRINTPND
	                  | (release ? CAN_CMR_TXRQST : 0)
	                  | CAN_CMR_DATAA
	                  | CAN_CMR_DATAB;

	CAN->sMsgObj[1].CRR = 1 + msgobj;

	while (CAN->sMsgObj[1].CRR & CAN_CRR_BUSY);

    if ((CAN->sMsgObj[1].A2R & CAN_A2R_XTD) == 0)
    {
  	  /* standard ID */
  	  pCanMsg->IdType = CAN_STD_ID;
      pCanMsg->Id = (CAN->sMsgObj[1].A2R >> 2) & 0x07FF;
    }
    else
    {
      /* extended ID */
  	  pCanMsg->IdType = CAN_EXT_ID;
      pCanMsg->Id = ((CAN->sMsgObj[1].A2R >> 2) & 0x07FF) | ((u32)CAN->sMsgObj[1].A1R << 11) | (((u32)CAN->sMsgObj[1].A2R & 0x0003) << 27);
    }

    pCanMsg->Dlc = CAN->sMsgObj[1].MCR & 0x0F;

    pCanMsg->Data[0] = (u8) CAN->sMsgObj[1].DA1R;
    pCanMsg->Data[1] = (u8)(CAN->sMsgObj[1].DA1R >> 8);
    pCanMsg->Data[2] = (u8) CAN->sMsgObj[1].DA2R;
    pCanMsg->Data[3] = (u8)(CAN->sMsgObj[1].DA2R >> 8);
    pCanMsg->Data[4] = (u8) CAN->sMsgObj[1].DB1R;
    pCanMsg->Data[5] = (u8)(CAN->sMsgObj[1].DB1R >> 8);
    pCanMsg->Data[6] = (u8) CAN->sMsgObj[1].DB2R;
    pCanMsg->Data[7] = (u8)(CAN->sMsgObj[1].DB2R >> 8);

    return 1;
}

/*******************************************************************************
* Function Name  : CAN_WaitEndOfTx
* Description    : Wait until current transmission is finished
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_WaitEndOfTx(void)
{
	while ((CAN->SR & CAN_SR_TXOK) == 0);
	CAN->SR &= ~CAN_SR_TXOK;
}

/*******************************************************************************
* Function Name  : CAN_BasicSendMessage
* Description    : Start transmission of a message in BASIC mode
* Input 1        : pointer to the message structure containing data to transmit
* Output         : None
* Return         : 1 if transmission was OK, else 0
* Note           : CAN must be in BASIC mode
*******************************************************************************/
u32 CAN_BasicSendMessage(canmsg* pCanMsg)
{
	/* clear NewDat bit in IF2 to detect next reception */
	CAN->sMsgObj[1].MCR &= ~CAN_MCR_NEWDAT;

	CAN->SR &= ~CAN_SR_TXOK;

	CAN->sMsgObj[0].CMR = CAN_CMR_WRRD
  	                    | CAN_CMR_ARB
  	                    | CAN_CMR_CONTROL
  	                    | CAN_CMR_DATAA
  	                    | CAN_CMR_DATAB;

	if (pCanMsg->IdType == CAN_STD_ID)
	{
	  	/* standard ID */
		CAN->sMsgObj[0].A1R = 0;
		CAN->sMsgObj[0].A2R = (CAN->sMsgObj[0].A2R & 0xE000) | STD_FIXED_ID_ARB(pCanMsg->Id);
	}
	else
	{
  		/* extended ID */
		CAN->sMsgObj[0].A1R = EXT_FIXED_ID_ARB_L(pCanMsg->Id);
		CAN->sMsgObj[0].A2R = (CAN->sMsgObj[0].A2R & 0xE000) | EXT_FIXED_ID_ARB_H(pCanMsg->Id);
	}

	CAN->sMsgObj[0].MCR = (CAN->sMsgObj[0].MCR & 0xFCF0) | pCanMsg->Dlc;

	CAN->sMsgObj[0].DA1R = ((u16)pCanMsg->Data[1]<<8) | pCanMsg->Data[0];
	CAN->sMsgObj[0].DA2R = ((u16)pCanMsg->Data[3]<<8) | pCanMsg->Data[2];
	CAN->sMsgObj[0].DB1R = ((u16)pCanMsg->Data[5]<<8) | pCanMsg->Data[4];
	CAN->sMsgObj[0].DB2R = ((u16)pCanMsg->Data[7]<<8) | pCanMsg->Data[6];

	/* request transmission */
	CAN->sMsgObj[0].CRR = CAN_CRR_BUSY | (1 + 0);

	return 1;
}

/*******************************************************************************
* Function Name  : CAN_BasicReceiveMessage
* Description    : Get the message in BASIC mode, if received
* Input 1        : pointer to the message structure where received data is stored
* Output         : None
* Return         : 1 if reception was OK, else 0 (no message pending)
* Note           : CAN must be in BASIC mode
*******************************************************************************/
u32 CAN_BasicReceiveMessage(canmsg* pCanMsg)
{
	if ((CAN->sMsgObj[1].MCR & CAN_MCR_NEWDAT) == 0)
		return 0;

	CAN->SR &= ~CAN_SR_RXOK;

	CAN->sMsgObj[1].CMR =  CAN_CMR_ARB
  	                     | CAN_CMR_CONTROL
  	                     | CAN_CMR_DATAA
  	                     | CAN_CMR_DATAB;

    if ((CAN->sMsgObj[1].A2R & CAN_A2R_XTD) == 0)
    {
  	  /* standard ID */
  	  pCanMsg->IdType = CAN_STD_ID;
      pCanMsg->Id = (CAN->sMsgObj[1].A2R >> 2) & 0x07FF;
    }
    else
    {
      /* extended ID */
  	  pCanMsg->IdType = CAN_EXT_ID;
      pCanMsg->Id = ((CAN->sMsgObj[1].A2R >> 2) & 0x07FF) | ((u32)CAN->sMsgObj[1].A1R << 11) | (((u32)CAN->sMsgObj[1].A2R & 0x0003) << 27);
    }

    pCanMsg->Dlc = CAN->sMsgObj[1].MCR & 0x0F;

    pCanMsg->Data[0] = (u8) CAN->sMsgObj[1].DA1R;
    pCanMsg->Data[1] = (u8)(CAN->sMsgObj[1].DA1R >> 8);
    pCanMsg->Data[2] = (u8) CAN->sMsgObj[1].DA2R;
    pCanMsg->Data[3] = (u8)(CAN->sMsgObj[1].DA2R >> 8);
    pCanMsg->Data[4] = (u8) CAN->sMsgObj[1].DB1R;
    pCanMsg->Data[5] = (u8)(CAN->sMsgObj[1].DB1R >> 8);
    pCanMsg->Data[6] = (u8) CAN->sMsgObj[1].DB2R;
    pCanMsg->Data[7] = (u8)(CAN->sMsgObj[1].DB2R >> 8);

    return 1;
}

/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
