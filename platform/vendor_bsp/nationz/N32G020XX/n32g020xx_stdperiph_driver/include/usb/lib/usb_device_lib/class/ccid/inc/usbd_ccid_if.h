/*****************************************************************************
* Copyright (c) 2019, Nations Technologies Inc.
*
* All rights reserved.
* ****************************************************************************
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* - Redistributions of source code must retain the above copyright notice,
* this list of conditions and the disclaimer below.
*
* Nations' name may not be used to endorse or promote products derived from
* this software without specific prior written permission.
*
* DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* ****************************************************************************/

/**
* @file    usbd_ccid_if.h
* @author Nations Solution Team
* @version v1.0.0
* @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
* @brief   This file provides all the functions prototypes for USB CCID
* @addtogroup USB
* @{
*/ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CCID_IF_H__
#define __USBD_CCID_IF_H__

/* Includes ------------------------------------------------------------------*/
#include "n32g020xx.h"
#include "core_cm0.h"
#include "n32g020xx_usb_regs.h"
#include "usbd_ccid_core.h"

/* Exported defines ----------------------------------------------------------*/
/* Bulk-only Command Block Wrapper */
#define ABDATA_SIZE 261
#define CCID_CMD_HEADER_SIZE 10
#define CCID_RESPONSE_HEADER_SIZE 10


#define CCID_INT_BUFF_SIZ 2

#define CARD_SLOT_FITTED  1
#define CARD_SLOT_REMOVED  0

#define BULK_MAX_PACKET_SIZE 0x40
#define CCID_IN_EP_SIZE   0x40
#define INTR_MAX_PACKET_SIZE 8
#define CCID_MESSAGE_HEADER_SIZE 10
#define CCID_NUMBER_OF_SLOTS     1       
                         /* Number of SLOTS. For single card, this value is 1 */

/* Following Parameters used in PC_to_RDR_IccPowerOn */
#define VOLTAGE_SELECTION_AUTOMATIC  0xFF
#define VOLTAGE_SELECTION_3V  0x02
#define VOLTAGE_SELECTION_5V  0x01
#define VOLTAGE_SELECTION_1V8 0x03

#define		PC_TO_RDR_ICCPOWERON		0x62
#define		PC_TO_RDR_ICCPOWEROFF		0x63
#define		PC_TO_RDR_GETSLOTSTATUS		0x65
#define		PC_TO_RDR_XFRBLOCK	        0x6F
#define		PC_TO_RDR_GETPARAMETERS		0x6C
#define		PC_TO_RDR_RESETPARAMETERS	0x6D
#define		PC_TO_RDR_SETPARAMETERS		0x61
#define		PC_TO_RDR_ESCAPE		0x6B
#define		PC_TO_RDR_ICCCLOCK		0x6E
#define		PC_TO_RDR_T0APDU		0x6A
#define		PC_TO_RDR_SECURE		0x69
#define		PC_TO_RDR_MECHANICAL		0x71
#define		PC_TO_RDR_ABORT			0x72
#define		PC_TO_RDR_SETDATARATEANDCLOCKFREQUENCY		0x73

#define		RDR_TO_PC_DATABLOCK		0x80
#define		RDR_TO_PC_SLOTSTATUS		0x81
#define		RDR_TO_PC_PARAMETERS		0x82
#define		RDR_TO_PC_ESCAPE		0x83
#define		RDR_TO_PC_DATARATEANDCLOCKFREQUENCY		0x84

#define		RDR_TO_PC_NOTIFYSLOTCHANGE	0x50
#define		RDR_TO_PC_HARDWAREERROR		0x51

#define         OFFSET_INT_BMESSAGETYPE  0
#define         OFFSET_INT_BMSLOTICCSTATE 1
#define         SLOT_ICC_PRESENT 0x01  
                /* LSb : (0b = no ICC present, 1b = ICC present) */

#define         SLOT_ICC_CHANGE  0x02  /* MSb : (0b = no change, 1b = change) */
/*****************************************************************************/
/*********************** CCID Bulk Transfer State machine ********************/
/*****************************************************************************/
#define CCID_STATE_IDLE                      0       /* Idle state */
#define CCID_STATE_DATA_OUT                  1       /* Data Out state */
#define CCID_STATE_RECEIVE_DATA              2
#define CCID_STATE_SEND_RESP                 3
#define CCID_STATE_DATAIN                    4
#define CCID_STATE_UNCORRECT_LENGTH          5

#define DIR_IN                        0
#define DIR_OUT                       1
#define BOTH_DIR                      2

/* Exported types ------------------------------------------------------------*/
#pragma pack(1)
typedef struct 
{ 
  uint8_t bMessageType; /* Offset = 0*/
  uint32_t dwLength;    /* Offset = 1, The length field (dwLength) is the length  
                          of the message not including the 10-byte header.*/
  uint8_t bSlot;        /* Offset = 5*/
  uint8_t bSeq;         /* Offset = 6*/
  uint8_t bSpecific_0;  /* Offset = 7*/
  uint8_t bSpecific_1;  /* Offset = 8*/
  uint8_t bSpecific_2;  /* Offset = 9*/
  uint8_t abData [ABDATA_SIZE]; /* Offset = 10, For reference, the absolute 
                           maximum block size for a TPDU T=0 block is 260 bytes 
                           (5 bytes command; 255 bytes data), 
                           or for a TPDU T=1 block is 259 bytes, 
                           or for a short APDU T=1 block is 261 bytes, 
                           or for an extended APDU T=1 block is 65544 bytes.*/
} Ccid_bulkout_data_t; 
#pragma pack()

#pragma pack(1)
typedef struct 
{ 
  uint8_t bMessageType;   /* Offset = 0*/
  uint32_t dwLength;      /* Offset = 1*/
  uint8_t bSlot;          /* Offset = 5, Same as Bulk-OUT message */
  uint8_t bSeq;           /* Offset = 6, Same as Bulk-OUT message */
  uint8_t bStatus;        /* Offset = 7, Slot status as defined in § 6.2.6*/
  uint8_t bError;         /* Offset = 8, Slot error  as defined in § 6.2.6*/
  uint8_t bSpecific;      /* Offset = 9*/
  uint8_t abData[ABDATA_SIZE]; /* Offset = 10*/
  uint16_t u16SizeToSend; 
} Ccid_bulkin_data_t; 
#pragma pack()

typedef struct 
{ 
  __IO uint8_t SlotStatus;
  __IO uint8_t SlotStatusChange;
} Ccid_SlotStatus_t; 


typedef struct 
{ 
  __IO uint8_t bAbortRequestFlag; 
  __IO uint8_t bSeq; 
  __IO uint8_t bSlot;
} usb_ccid_param_t; 

extern usb_ccid_param_t usb_ccid_param;
extern Ccid_bulkout_data_t Ccid_bulkout_data; /* Buffer for the Out Data */
extern Ccid_bulkin_data_t Ccid_bulkin_data;   /* Buffer for the IN Data */
extern Ccid_SlotStatus_t Ccid_SlotStatus;
extern uint8_t UsbIntMessageBuffer[];
extern uint8_t Ccid_BulkState;

/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 
void CCID_BulkMessage_In (USB_CORE_HANDLE  *pdev, 
                     uint8_t epnum);

void CCID_BulkMessage_Out (USB_CORE_HANDLE  *pdev, 
                            uint8_t epnum);

void CCID_ReceiveCmdHeader(uint8_t* pDst, uint8_t u8length);
void CCID_CmdDecode(USB_CORE_HANDLE  *pdev);

void CCID_IntMessage(USB_CORE_HANDLE  *pdev);
void CCID_Init(USB_CORE_HANDLE  *pdev);
void CCID_DeInit(USB_CORE_HANDLE  *pdev);

uint8_t CCID_IsIntrTransferComplete(void);
void CCID_SetIntrTransferStatus (uint8_t );
void Transfer_Data_Request(uint8_t* dataPointer, uint16_t dataLen);
void Set_CSW (uint8_t CSW_Status, uint8_t Send_Permission);

#endif /* __USBD_CCID_IF_H__ */

/** @} */

