/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : can.h
* Author             : MCD Application Team
* Date First Issued  : 27/10/2003
* Description        : This file contains all the functions prototypes for the
*                      CAN bus software library.
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
#ifndef __can_H
#define __can_H

#include "71x_map.h"

// Standard bitrates available
enum
{
  CAN_BITRATE_100K,
  CAN_BITRATE_125K,
  CAN_BITRATE_250K,
  CAN_BITRATE_500K,
  CAN_BITRATE_1M
};

// Control register
#define CAN_CR_TEST		0x0080
#define CAN_CR_CCE		0x0040
#define CAN_CR_DAR		0x0020
#define CAN_CR_EIE		0x0008
#define CAN_CR_SIE		0x0004
#define CAN_CR_IE		0x0002
#define CAN_CR_INIT		0x0001

// Status register
#define CAN_SR_LEC		0x0007
#define CAN_SR_TXOK		0x0008
#define CAN_SR_RXOK		0x0010
#define CAN_SR_EPASS		0x0020
#define CAN_SR_EWARN		0x0040
#define CAN_SR_BOFF		0x0080

// Test register
#define CAN_TESTR_RX		0x0080
#define CAN_TESTR_TX1		0x0040
#define CAN_TESTR_TX0		0x0020
#define CAN_TESTR_LBACK		0x0010
#define CAN_TESTR_SILENT	0x0008
#define CAN_TESTR_BASIC		0x0004

// IFn / Command Request register
#define CAN_CRR_BUSY		0x8000

// IFn / Command Mask register
#define CAN_CMR_WRRD		0x0080
#define CAN_CMR_MASK		0x0040
#define CAN_CMR_ARB		0x0020
#define CAN_CMR_CONTROL		0x0010
#define CAN_CMR_CLRINTPND	0x0008
#define CAN_CMR_TXRQST		0x0004
#define CAN_CMR_DATAA		0x0002
#define CAN_CMR_DATAB		0x0001

// IFn / Mask 2 register
#define CAN_M2R_MXTD		0x8000
#define CAN_M2R_MDIR		0x4000

// IFn / Arbitration 2 register
#define CAN_A2R_MSGVAL		0x8000
#define CAN_A2R_XTD		0x4000
#define CAN_A2R_DIR		0x2000

// IFn / Message Control register
#define CAN_MCR_NEWDAT		0x8000
#define CAN_MCR_MSGLST		0x4000
#define CAN_MCR_INTPND		0x2000
#define CAN_MCR_UMASK		0x1000
#define CAN_MCR_TXIE		0x0800
#define CAN_MCR_RXIE		0x0400
#define CAN_MCR_RMTEN		0x0200
#define CAN_MCR_TXRQST		0x0100
#define CAN_MCR_EOB		0x0080


// Wake-up modes
enum
{
  CAN_WAKEUP_ON_EXT,
  CAN_WAKEUP_ON_CAN
};


// CAN message structure
typedef struct
{
  u32 IdType;
  vu32 Id;
  vu8 Dlc;
  vu8 Data[8];
} canmsg;

// message ID types
enum
{
  CAN_STD_ID,
  CAN_EXT_ID
};

// message ID limits
#define CAN_LAST_STD_ID	((1<<11) - 1)
#define CAN_LAST_EXT_ID	((1L<<29) - 1)

/*******************************************************************************
* Function Name  : CAN_EnterInitMode
* Description    : Switch the CAN into initialization mode
* Input 1        : any binary value formed from the CAN_CR_xxx defines
* Output         : None
* Return         : None
* Note           : CAN_LeaveInitMode must be called when all is done
*******************************************************************************/
INLINE void CAN_EnterInitMode(u8 mask)
{
	CAN->CR = mask | CAN_CR_INIT;
	CAN->SR = 0;					// reset the status
}

/*******************************************************************************
* Function Name  : CAN_LeaveInitMode
* Description    : Leave the initialization mode (switch into normal mode)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
INLINE void CAN_LeaveInitMode(void)
{
	CAN->CR &= ~(CAN_CR_INIT | CAN_CR_CCE);
}

/*******************************************************************************
* Function Name  : CAN_EnterTestMode
* Description    : Switch the CAN into test mode
* Input 1        : any binary value formed from the CAN_TESTR_xxx defines
* Output         : None
* Return         : None
* Note           : CAN_LeaveTestMode must be called when all is done
*******************************************************************************/
INLINE void CAN_EnterTestMode(u8 mask)
{
	CAN->CR |= CAN_CR_TEST;
	CAN->TESTR |= mask;
}

/*******************************************************************************
* Function Name  : CAN_LeaveTestMode
* Description    : Leave the current test mode (switch into normal mode)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
INLINE void CAN_LeaveTestMode(void)
{
	CAN->CR |= CAN_CR_TEST;
	CAN->TESTR &= ~(CAN_TESTR_LBACK | CAN_TESTR_SILENT | CAN_TESTR_BASIC);
	CAN->CR &= ~CAN_CR_TEST;
}

/*******************************************************************************
* Function Name  : CAN_SetBitrate
* Description    : Setup a standard CAN bitrate
* Input 1        : one of the CAN_BITRATE_xxx defines
* Output         : None
* Return         : None
* Note           : CAN must be in initialization mode
*******************************************************************************/
void CAN_SetBitrate(u32 bitrate);

/*******************************************************************************
* Function Name  : CAN_SetTiming
* Description    : Setup the CAN timing with specific parameters
* Input 1        : Time Segment before the sample point position, from 1 to 16
* Input 2        : Time Segment after the sample point position, from 1 to 8
* Input 3        : Synchronisation Jump Width, from 1 to 4
* Input 4        : Baud Rate Prescaler, from 1 to 1024
* Output         : None
* Return         : None
* Note           : CAN must be in initialization mode
*******************************************************************************/
void CAN_SetTiming(u32 tseg1, u32 tseg2, u32 sjw, u32 brp);

/*******************************************************************************
* Function Name  : CAN_SleepRequest
* Description    : Request the CAN cell to enter sleep mode
* Input 1        : CAN_WAKEUP_ON_EXT or CAN_WAKEUP_ON_CAN
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SleepRequest(u32 WakeupMode);

/*******************************************************************************
* Function Name  : CAN_SetUnusedMsgObj
* Description    : Configure the message object as unused
* Input 1        : message object number, from 0 to 31
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SetUnusedMsgObj(u32 msgobj);

/*******************************************************************************
* Function Name  : CAN_SetTxMsgObj
* Description    : Configure the message object as TX
* Input 1        : message object number, from 0 to 31
* Input 2        : CAN_STD_ID or CAN_EXT_ID
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SetTxMsgObj(u32 msgobj, u32 idType);

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
void CAN_SetRxMsgObj(u32 msgobj, u32 idType, u32 idLow, u32 idHigh, bool singleOrFifoLast);

/*******************************************************************************
* Function Name  : CAN_InvalidateAllMsgObj
* Description    : Configure all the message objects as unused
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_InvalidateAllMsgObj(void);

/*******************************************************************************
* Function Name  : CAN_Init
* Description    : Initialize the CAN cell and set the bitrate
* Input 1        : any binary value formed from the CAN_CTL_xxx defines
* Input 2        : one of the CAN_BITRATE_xxx defines
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_Init(u8 mask, u32 bitrate);

/*******************************************************************************
* Function Name  : CAN_ReleaseMessage
* Description    : Release the message object
* Input 1        : message object number, from 0 to 31
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_ReleaseMessage(u32 msgobj);

/*******************************************************************************
* Function Name  : CAN_ReleaseTxMessage
* Description    : Release the transmit message object
* Input 1        : message object number, from 0 to 31
* Output         : None
* Return         : None
* Note           : assume that message interface 0 is free
*******************************************************************************/
INLINE void CAN_ReleaseTxMessage(u32 msgobj)
{
	CAN->sMsgObj[0].CMR = CAN_CMR_CLRINTPND | CAN_CMR_TXRQST;
	CAN->sMsgObj[0].CRR = 1 + msgobj;
}

/*******************************************************************************
* Function Name  : CAN_ReleaseRxMessage
* Description    : Release the receive message object
* Input 1        : message object number, from 0 to 31
* Output         : None
* Return         : None
* Note           : assume that message interface 1 is free
*******************************************************************************/
INLINE void CAN_ReleaseRxMessage(u32 msgobj)
{
	CAN->sMsgObj[1].CMR = CAN_CMR_CLRINTPND | CAN_CMR_TXRQST;
	CAN->sMsgObj[1].CRR = 1 + msgobj;
}

/*******************************************************************************
* Function Name  : CAN_SendMessage
* Description    : Start transmission of a message
* Input 1        : message object number, from 0 to 31
* Input 2        : pointer to the message structure containing data to transmit
* Output         : None
* Return         : 1 if transmission was OK, else 0
*******************************************************************************/
u32  CAN_SendMessage(u32 msgobj, canmsg* pCanMsg);

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
u32  CAN_ReceiveMessage(u32 msgobj, bool release, canmsg* pCanMsg);

/*******************************************************************************
* Function Name  : CAN_WaitEndOfTx
* Description    : Wait until current transmission is finished
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_WaitEndOfTx(void);

/*******************************************************************************
* Function Name  : CAN_BasicSendMessage
* Description    : Start transmission of a message in BASIC mode
* Input 1        : pointer to the message structure containing data to transmit
* Output         : None
* Return         : 1 if transmission was OK, else 0
* Note           : CAN must be in BASIC mode
*******************************************************************************/
u32 CAN_BasicSendMessage(canmsg* pCanMsg);

/*******************************************************************************
* Function Name  : CAN_BasicReceiveMessage
* Description    : Get the message in BASIC mode, if received
* Input 1        : pointer to the message structure where received data is stored
* Output         : None
* Return         : 1 if reception was OK, else 0 (no message pending)
* Note           : CAN must be in BASIC mode
*******************************************************************************/
u32 CAN_BasicReceiveMessage(canmsg* pCanMsg);

/*******************************************************************************
* Function Name  : CAN_IsMessageWaiting
* Description    : Test the waiting status of a received message
* Input 1        : message object number, from 0 to 31
* Output         : None
* Return         : A non-zero value if the corresponding message object has
*                  received a message waiting to be copied, else 0
*******************************************************************************/
INLINE u32 CAN_IsMessageWaiting(u32 msgobj)
{
  return (msgobj < 16 ? CAN->ND1R & (1 << msgobj) : CAN->ND2R & (1 << (msgobj-16)));
}

/*******************************************************************************
* Function Name  : CAN_IsTransmitRequested
* Description    : Test the request status of a transmitted message
* Input 1        : message object number, from 0 to 31
* Output         : None
* Return         : A non-zero value if the corresponding message is requested
*                  to transmit, else 0
*******************************************************************************/
INLINE u32 CAN_IsTransmitRequested(u32 msgobj)
{
  return (msgobj < 16 ? CAN->TR1R & (1 << msgobj) : CAN->TR2R & (1 << (msgobj-16)));
}

/*******************************************************************************
* Function Name  : CAN_IsInterruptPending
* Description    : Test the interrupt status of a message object
* Input 1        : message object number, from 0 to 31
* Output         : None
* Return         : A non-zero value if the corresponding message has an interrupt
*                  pending, else 0
*******************************************************************************/
INLINE u32 CAN_IsInterruptPending(u32 msgobj)
{
  return (msgobj < 16 ? CAN->IP1R & (1 << msgobj) : CAN->IP2R & (1 << (msgobj-16)));
}

/*******************************************************************************
* Function Name  : CAN_IsObjectValid
* Description    : Test the validity of a message object (ready to use)
* Input 1        : message object number, from 0 to 31
* Output         : None
* Return         : A non-zero value if the corresponding message object is valid,
*                  else 0
*******************************************************************************/
INLINE u32 CAN_IsObjectValid(u32 msgobj)
{
  return (msgobj < 16 ? CAN->MV1R & (1 << msgobj) : CAN->MV2R & (1 << (msgobj-16)));
}

#endif /* __can_H */

/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
