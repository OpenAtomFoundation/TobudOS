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
* @file    usbd_ccid_if.c
* @author Nations Solution Team
* @version v1.0.0
* @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
* @brief   This file provides all the functions for USB Interface for CCID .
*/ 

/* Includes ------------------------------------------------------------------*/
#include "n32g020xx.h"
#include "core_cm0.h"
#include "n32g020xx_usb_regs.h"
#include "usbd_ccid_if.h"
#include "n32g020xx_usb_dcd.h"
#include "n32g020xx_usbd_conf.h"
#include "n32g020xx_usb_log.h"
#include "n32g020xx_log.h"
#include "usbd_ccid_cmd.h"
#include "n32g020xx_usbd_ioreq.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Ccid_BulkState;
uint8_t UsbIntMessageBuffer[INTR_MAX_PACKET_SIZE];  /* data buffer*/
__IO uint8_t PrevXferComplete_IntrIn;
usb_ccid_param_t usb_ccid_param;

uint8_t* pUsbMessageBuffer;
static uint32_t UsbMessageLength;
Ccid_bulkin_data_t pCcid_resp_buff;
Ccid_SlotStatus_t Ccid_SlotStatus;


uint8_t BulkOut_Data_Buff[BULK_MAX_PACKET_SIZE] ;

Ccid_bulkin_data_t Ccid_bulkin_data ;

Ccid_bulkout_data_t Ccid_bulkout_data ;

uint8_t UsbIntMessageBuffer[INTR_MAX_PACKET_SIZE] ;

/* Private function prototypes -----------------------------------------------*/
static void CCID_Response_SendData (USB_CORE_HANDLE  *pdev, 
                              uint8_t* pbuf, 
                              uint16_t len);
/* Private function ----------------------------------------------------------*/
/**
  * @brief  CCID_Init
  *         Initialize the CCID USB Layer
  * @param  pdev: device instance
  * @retval None
  */
void CCID_Init (USB_CORE_HANDLE  *pdev)
{
  /* CCID Related Initialization */
  CCID_SetIntrTransferStatus(1);  /* Transfer Complete Status */
  CCID_UpdSlotChange(1);
  //SC_InitParams();  
  
  /* Prepare EP to Receive First Cmd */
  DCD_EP_PrepareRx (pdev,
                    CCID_BULK_OUT_EP,
                    (uint8_t *)&BulkOut_Data_Buff[0],
                    CCID_BULK_EPOUT_SIZE);   
}

/**
  * @brief  CCID_DeInit
  *         Uninitialize the CCID Machine
  * @param  pdev: device instance
  * @retval None
  */
void CCID_DeInit (USB_CORE_HANDLE  *pdev)
{
   Ccid_BulkState = CCID_STATE_IDLE;
}

/**
  * @brief  CCID_Message_In
  *         Handle Bulk IN & Intr IN data stage 
  * @param  pdev: device instance
  * @param  uint8_t epnum: endpoint index
  * @retval None
  */
void CCID_BulkMessage_In (USB_CORE_HANDLE  *pdev, 
                     uint8_t epnum)
{  
  if (epnum == (CCID_BULK_IN_EP & 0x7F))
  {/* Filter the epnum by masking with 0x7f (mask of IN Direction)  */
    
    /*************** Handle Bulk Transfer IN data completion  *****************/
    log_usb_debuginfo("ep%d-CCID_In,state=%d.\r\n ", epnum, Ccid_BulkState);
    switch (Ccid_BulkState)
    {
    case CCID_STATE_SEND_RESP:
      
      Ccid_BulkState = CCID_STATE_IDLE;
      
      /* Prepare EP to Receive First Cmd */
      DCD_EP_PrepareRx (pdev,
                        CCID_BULK_OUT_EP,
                        (uint8_t *)&BulkOut_Data_Buff[0],
                        CCID_BULK_EPOUT_SIZE);   
      
      break;
      
    default:
      break;
    }
  }
#if 0
  else if (epnum == (CCID_INTR_IN_EP & 0x7F))
  {
    /* Filter the epnum by masking with 0x7f (mask of IN Direction)  */
    CCID_SetIntrTransferStatus(1);  /* Transfer Complete Status */
  }
#endif
}

/**
  * @brief  CCID_BulkMessage_Out
  *         Proccess CCID OUT data
  * @param  pdev: device instance
  * @param  uint8_t epnum: endpoint index
  * @retval None
  */
void CCID_BulkMessage_Out (USB_CORE_HANDLE  *pdev, 
                           uint8_t epnum)
{
  uint16_t dataLen;
  dataLen = USBD_GetRxCount (pdev, CCID_BULK_OUT_EP);

  log_usb_debuginfo("ccid bulk out,len=0x%x, state=0x%x.\r\n", dataLen, Ccid_BulkState);
  switch (Ccid_BulkState)
  {
  case CCID_STATE_IDLE:
    if (dataLen == 0x00)
    { /* Zero Length Packet Received */
      Ccid_BulkState = CCID_STATE_IDLE;
    }
    else  if (dataLen >= CCID_MESSAGE_HEADER_SIZE)
    {
        log_usb_debuginfo("dataLen >= CCID_MESSAGE_HEADER_SIZE.\r\n");
      UsbMessageLength = dataLen;   /* Store for future use */
      
      /* Expected Data Length Packet Received */
      pUsbMessageBuffer = (uint8_t*) &Ccid_bulkout_data;
      
      /* Fill CCID_BulkOut Data Buffer from USB Buffer */
      CCID_ReceiveCmdHeader(pUsbMessageBuffer, dataLen); 
      
      /*
      Refer : 6 CCID Messages
      The response messages always contain the exact same slot number, 
      and sequence number fields from the header that was contained in 
      the Bulk-OUT command message.
      */
      Ccid_bulkin_data.bSlot = Ccid_bulkout_data.bSlot;
      Ccid_bulkin_data.bSeq = Ccid_bulkout_data.bSeq; 
      
      if (dataLen < CCID_BULK_EPOUT_SIZE)
      {/* Short message, less than the EP Out Size, execute the command,
        if parameter like dwLength is too big, the appropriate command will 
        give an error */
		log_usb_debuginfo("len<CCID_EPOUT_SIZE.\r\n");

        CCID_CmdDecode(pdev);  
      }
      else
      { /* Long message, receive additional data with command */
        /* (u8dataLen == CCID_BULK_EPOUT_SIZE) */
        log_usb_debuginfo("len>CCID_EPOUT_SIZE.\r\n");
        if (Ccid_bulkout_data.dwLength > ABDATA_SIZE)
        { /* Check if length of data to be sent by host is > buffer size */
          
          /* Too long data received.... Error ! */
          log_usb_debuginfo("err,too long.\r\n");
          Ccid_BulkState = CCID_STATE_UNCORRECT_LENGTH;
        }
        
        else 
        { /* Expect more data on OUT EP */
          Ccid_BulkState = CCID_STATE_RECEIVE_DATA;
          pUsbMessageBuffer += dataLen;  /* Point to new offset */      

          log_usb_debuginfo("prepareRx.\r\n");
          /* Prepare EP to Receive next Cmd */
          DCD_EP_PrepareRx (pdev,
                            CCID_BULK_OUT_EP,
                            (uint8_t *)&BulkOut_Data_Buff[0], 
                            CCID_BULK_EPOUT_SIZE);  
          
        } /* if (dataLen == CCID_BULK_EPOUT_SIZE) ends */
      } /*  if (dataLen >= CCID_BULK_EPOUT_SIZE) ends */
    } /* if (dataLen >= CCID_MESSAGE_HEADER_SIZE) ends */
    break;
    
  case CCID_STATE_RECEIVE_DATA:
    
    UsbMessageLength += dataLen;
    
    if (dataLen < CCID_BULK_EPOUT_SIZE)
    {/* Short message, less than the EP Out Size, execute the command,
        if parameter like dwLength is too big, the appropriate command will 
        give an error */
      
      /* Full command is received, process the Command */
      CCID_ReceiveCmdHeader(pUsbMessageBuffer, dataLen);
      CCID_CmdDecode(pdev); 
    }
    else if (dataLen == CCID_BULK_EPOUT_SIZE)
    {  
      if (UsbMessageLength < (Ccid_bulkout_data.dwLength + CCID_CMD_HEADER_SIZE))
      {
        CCID_ReceiveCmdHeader(pUsbMessageBuffer, dataLen); /* Copy data */
        pUsbMessageBuffer += dataLen; 
        /* Increment the pointer to receive more data */
        
        /* Prepare EP to Receive next Cmd */
        DCD_EP_PrepareRx (pdev,
                          CCID_BULK_OUT_EP,
                          (uint8_t *)&BulkOut_Data_Buff[0], 
                          CCID_BULK_EPOUT_SIZE);  
      }
      else if (UsbMessageLength == (Ccid_bulkout_data.dwLength + CCID_CMD_HEADER_SIZE))
      { 
        /* Full command is received, process the Command */
        CCID_ReceiveCmdHeader(pUsbMessageBuffer, dataLen);
        CCID_CmdDecode(pdev);
      }
      else
      {
        /* Too long data received.... Error ! */
        Ccid_BulkState = CCID_STATE_UNCORRECT_LENGTH;
      }
    }
    
    break;
  
  case CCID_STATE_UNCORRECT_LENGTH:
    Ccid_BulkState = CCID_STATE_IDLE;
    break;
  
  default:
    break;
  }
}

/**
  * @brief  CCID_CmdDecode
  *         Parse the commands and Proccess command
  * @param  pdev: device instance
  * @retval None
  */
void CCID_CmdDecode(USB_CORE_HANDLE  *pdev)
{
  uint8_t errorCode;
  log_usb_debuginfo("CCID_CmdDecode, entry,msgtype=%02x.\r\n", Ccid_bulkout_data.bMessageType);
  switch (Ccid_bulkout_data.bMessageType)
  {
  case PC_TO_RDR_ICCPOWERON:
  	log_usb_debuginfo("icc pwr on.\r\n");
    errorCode = PC_to_RDR_IccPowerOn();
    RDR_to_PC_DataBlock(errorCode);
    break;
  case PC_TO_RDR_ICCPOWEROFF:
  	log_usb_debuginfo("icc pwr off.\r\n");
    errorCode = PC_to_RDR_IccPowerOff();
    RDR_to_PC_SlotStatus(errorCode);
    break;
  case PC_TO_RDR_GETSLOTSTATUS:
  	log_usb_debuginfo("icc get slots sts.\r\n");
    errorCode = PC_to_RDR_GetSlotStatus();
    RDR_to_PC_SlotStatus(errorCode);
    break;
  case PC_TO_RDR_XFRBLOCK:
  	log_usb_debuginfo("icc xfr bock.\r\n");
    errorCode = PC_to_RDR_XfrBlock();
    RDR_to_PC_DataBlock(errorCode);
    break;
  case PC_TO_RDR_GETPARAMETERS:
  	log_usb_debuginfo("icc get param.\r\n");
    errorCode = PC_to_RDR_GetParameters();
    RDR_to_PC_Parameters(errorCode);
    break;
  case PC_TO_RDR_RESETPARAMETERS:
  	log_usb_debuginfo("icc reset praram.\r\n");
    errorCode = PC_to_RDR_ResetParameters();
    RDR_to_PC_Parameters(errorCode);
    break;
  case PC_TO_RDR_SETPARAMETERS:
  	log_usb_debuginfo("icc set param.\r\n");
    errorCode = PC_to_RDR_SetParameters();
    RDR_to_PC_Parameters(errorCode);
    break;
  case PC_TO_RDR_ESCAPE:
  	log_usb_debuginfo("icc escape.\r\n");
    errorCode = PC_to_RDR_Escape();
    RDR_to_PC_Escape(errorCode);
    break;
  case PC_TO_RDR_ICCCLOCK:
  	log_usb_debuginfo("icc lock.\r\n");
    errorCode = PC_to_RDR_IccClock();
    RDR_to_PC_SlotStatus(errorCode);
    break;
  case PC_TO_RDR_ABORT:
  	log_usb_debuginfo("icc abort.\r\n");
    errorCode = PC_to_RDR_Abort();
    RDR_to_PC_SlotStatus(errorCode);
    break;
  case PC_TO_RDR_T0APDU:
  	log_usb_debuginfo("icc t0 apdu.\r\n");
    errorCode = PC_TO_RDR_T0Apdu();
    RDR_to_PC_SlotStatus(errorCode);
    break;
  case PC_TO_RDR_MECHANICAL:
  	log_usb_debuginfo("icc mechanical.\r\n");
    errorCode = PC_TO_RDR_Mechanical();
    RDR_to_PC_SlotStatus(errorCode);
    break;   
  case PC_TO_RDR_SETDATARATEANDCLOCKFREQUENCY:
  	log_usb_debuginfo("icc set clk freq.\r\n");
    errorCode = PC_TO_RDR_SetDataRateAndClockFrequency();
    RDR_to_PC_DataRateAndClockFrequency(errorCode);
    break;
  case PC_TO_RDR_SECURE:
  	log_usb_debuginfo("icc secure.\r\n");
    errorCode = PC_TO_RDR_Secure();
    RDR_to_PC_DataBlock(errorCode);
    break;
  default:
  	log_usb_debuginfo("default:icc not support.\r\n");
    RDR_to_PC_SlotStatus(SLOTERROR_CMD_NOT_SUPPORTED);
    break;
  }
  
     /********** Decide for all commands ***************/ 
  if (Ccid_BulkState == CCID_STATE_SEND_RESP)
  {
	 log_usb_debuginfo("ccid need to snd-[%d]\r\n", Ccid_bulkin_data.u16SizeToSend);
	 for (uint32_t i = 0; i < Ccid_bulkin_data.u16SizeToSend; i++) {
		 if (i % 16 == 0) {
			 log_usb_debuginfo("\r\n");
		 }
		 log_usb_debuginfo("%x, ", *(((uint8_t*)&Ccid_bulkin_data) + i));
	 }
     log_usb_debuginfo("\r\n");

    CCID_Response_SendData(pdev, (uint8_t*)&Ccid_bulkin_data, 
                                  Ccid_bulkin_data.u16SizeToSend);
  }
}

/**
  * @brief  Transfer_Data_Request
  *         Prepare the request response to be sent to the host
  * @param  uint8_t* dataPointer: Pointer to the data buffer to send
  * @param  uint16_t dataLen : number of bytes to send
  * @retval None
  */
void Transfer_Data_Request(uint8_t* dataPointer, uint16_t dataLen)
{
   /**********  Update Global Variables ***************/
   Ccid_bulkin_data.u16SizeToSend = dataLen;
   Ccid_BulkState = CCID_STATE_SEND_RESP;    
}   
  
  
/**
  * @brief  CCID_Response_SendData
  *         Send the data on bulk-in EP 
  * @param  pdev: device instance
  * @param  uint8_t* buf: pointer to data buffer
  * @param  uint16_t len: Data Length
  * @retval None
  */
static void  CCID_Response_SendData(USB_CORE_HANDLE  *pdev,
                              uint8_t* buf, 
                              uint16_t len)
{  
  DCD_EP_Tx (pdev, CCID_BULK_IN_EP, buf, len, USB_IN_BUFFER_WAIT); 


  switch (Ccid_BulkState)
  {
  case CCID_STATE_SEND_RESP:
    
    Ccid_BulkState = CCID_STATE_IDLE;
    
    /* Prepare EP to Receive First Cmd */
    DCD_EP_PrepareRx (pdev,
                      CCID_BULK_OUT_EP,
                      (uint8_t *)&BulkOut_Data_Buff[0],
                      CCID_BULK_EPOUT_SIZE);   
    
    break;
    
  default:
    break;
  }
}

/**
  * @brief  CCID_IntMessage
  *         Send the Interrupt-IN data to the host
  * @param  pdev: device instance
  * @retval None
  */
void CCID_IntMessage(USB_CORE_HANDLE  *pdev)
{
  /* Check if there us change in Smartcard Slot status */  
  if ( CCID_IsSlotStatusChange() && CCID_IsIntrTransferComplete() )
  {
    /* Check Slot Status is changed. Card is Removed/ Fitted  */
    RDR_to_PC_NotifySlotChange();
    
    CCID_SetIntrTransferStatus(0);  /* Reset the Status */
    CCID_UpdSlotChange(0);    /* Reset the Status of Slot Change */
#if 0
    DCD_EP_Tx (pdev, CCID_INTR_IN_EP, UsbIntMessageBuffer, 2, USB_IN_BUFFER_WAIT); 
#endif
  }
}  

/**
  * @brief  CCID_ReceiveCmdHeader
  *         Receive the Data from USB BulkOut Buffer to Pointer 
  * @param  uint8_t* pDst: destination address to copy the buffer
  * @param  uint8_t u8length: length of data to copy
  * @retval None
  */
void CCID_ReceiveCmdHeader(uint8_t* pDst, uint8_t u8length)
{
  uint32_t Counter;

  
  	log_usb_debuginfo("ccid rcv bufaddr=%x.\r\n", BulkOut_Data_Buff);

  for (Counter = 0; Counter < u8length; Counter++)
  {
    *pDst++ = BulkOut_Data_Buff[Counter];
	log_usb_debuginfo("[%d]=0x%02x.\r\n", Counter, BulkOut_Data_Buff[Counter]);
  }
  
}

/**
  * @brief  CCID_IsIntrTransferComplete
  *         Provides the status of previous Interrupt transfer status
  * @param  None 
  * @retval uint8_t PrevXferComplete_IntrIn: Value of the previous transfer status
  */
uint8_t CCID_IsIntrTransferComplete (void)
{
  return PrevXferComplete_IntrIn;
}

/**
  * @brief  CCID_IsIntrTransferComplete
  *         Set the value of the Interrupt transfer status 
  * @param  uint8_t xfer_Status: Value of the Interrupt transfer status to set
  * @retval None 
  */
void CCID_SetIntrTransferStatus (uint8_t xfer_Status)
{
  PrevXferComplete_IntrIn = xfer_Status;
}

