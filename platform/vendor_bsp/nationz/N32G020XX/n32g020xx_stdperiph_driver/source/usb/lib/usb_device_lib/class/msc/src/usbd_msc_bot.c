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
  * @file    usbd_msc_bot.c
  * @author Nations Solution Team
  * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
  * @brief   This file provides all the BOT protocol core functions.
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_msc_bot.h"
#include "n32g020xx_usb_log.h"
#include "n32g020xx_log.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t             MSC_BOT_DataLen;
uint8_t              MSC_BOT_State;
uint8_t              MSC_BOT_Status;

uint8_t              MSC_BOT_Data[MSC_MEDIA_PACKET] ;

MSC_BOT_CBW_TypeDef  MSC_BOT_cbw ;

MSC_BOT_CSW_TypeDef  MSC_BOT_csw ;

/* Private function prototypes -----------------------------------------------*/
static void MSC_BOT_CBW_Decode (USB_CORE_HANDLE  *pdev);

static void MSC_BOT_SendData (USB_CORE_HANDLE  *pdev, 
                              uint8_t* pbuf, 
                              uint16_t len);

static void MSC_BOT_Abort(USB_CORE_HANDLE  *pdev);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  MSC_BOT_Init
  *         Initialize the BOT Process
  * @param  pdev: device instance
  * @retval None
  */
void MSC_BOT_Init (USB_CORE_HANDLE  *pdev)
{
	log_usb_data("msc bot init, MSC_BOT_Data = %x.\r\n", MSC_BOT_Data);
  MSC_BOT_State = BOT_IDLE;
  MSC_BOT_Status = BOT_STATE_NORMAL;
  USBD_STORAGE_fops->Init(0);
  
  /* Prepare EP to Receive First BOT Cmd */
  DCD_EP_PrepareRx (pdev,
                    MSC_OUT_EP,
                    (uint8_t *)&MSC_BOT_cbw,
                    BOT_CBW_LENGTH);    
}

/**
  * @brief  MSC_BOT_Reset
  *         Reset the BOT Machine
  * @param  pdev: device instance
  * @retval  None
  */
void MSC_BOT_Reset (USB_CORE_HANDLE  *pdev)
{
	log_usb_debuginfo("%s\r\n",__func__);

  MSC_BOT_State = BOT_IDLE;
  MSC_BOT_Status = BOT_STATE_RECOVERY;
  /* Prepare EP to Receive First BOT Cmd */
  DCD_EP_PrepareRx (pdev,
                    MSC_OUT_EP,
                    (uint8_t *)&MSC_BOT_cbw,
                    BOT_CBW_LENGTH);    
}

/**
  * @brief  MSC_BOT_DeInit
  *         Uninitialize the BOT Machine
  * @param  pdev: device instance
  * @retval None
  */
void MSC_BOT_DeInit (USB_CORE_HANDLE  *pdev)
{
	log_usb_debuginfo("%s\r\n",__func__);

  MSC_BOT_State = BOT_IDLE;
}

/**
  * @brief  MSC_BOT_DataIn
  *         Handle BOT IN data stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval None
  */
void MSC_BOT_DataIn (USB_CORE_HANDLE  *pdev, 
                     uint8_t epnum)
{
	log_usb_debuginfo("%s, MSC_BOT_State = %x\r\n",__func__, MSC_BOT_State);

  switch (MSC_BOT_State)
  {
  case BOT_DATA_IN:
    if(SCSI_ProcessCmd(pdev,
                        MSC_BOT_cbw.bLUN,
                        &MSC_BOT_cbw.CB[0]) < 0)
    {
      MSC_BOT_SendCSW (pdev, CSW_CMD_FAILED);
    }
    break;
    
  case BOT_SEND_DATA:
  case BOT_LAST_DATA_IN:
    MSC_BOT_SendCSW (pdev, CSW_CMD_PASSED);
    
    break;
    
  default:
    break;
  }
}
/**
  * @brief  MSC_BOT_DataOut
  *         Process MSC OUT data
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval None
  */
void MSC_BOT_DataOut (USB_CORE_HANDLE  *pdev, 
                      uint8_t epnum)
{
  log_usb_data("BOT[%x].\r\n", MSC_BOT_State);
  switch (MSC_BOT_State)
  {
  case BOT_IDLE:
    MSC_BOT_CBW_Decode(pdev);
    break;
    
  case BOT_DATA_OUT:
    
    if(SCSI_ProcessCmd(pdev,
                        MSC_BOT_cbw.bLUN,
                        &MSC_BOT_cbw.CB[0]) < 0)
    {
      MSC_BOT_SendCSW (pdev, CSW_CMD_FAILED);
    }

    break;
    
  default:
    break;
  }
  
}

/**
  * @brief  MSC_BOT_CBW_Decode
  *         Decode the CBW command and set the BOT state machine accordingly  
  * @param  pdev: device instance
  * @retval None
  */
static void  MSC_BOT_CBW_Decode (USB_CORE_HANDLE  *pdev)
{
	log_usb_debuginfo("%s\r\n",__func__);

  MSC_BOT_csw.dTag = MSC_BOT_cbw.dTag;
  MSC_BOT_csw.dDataResidue = MSC_BOT_cbw.dDataLength;
  
  if ((USBD_GetRxCount (pdev ,MSC_OUT_EP) != BOT_CBW_LENGTH) ||
      (MSC_BOT_cbw.dSignature != BOT_CBW_SIGNATURE)||
        (MSC_BOT_cbw.bLUN > 1) || 
          (MSC_BOT_cbw.bCBLength < 1) || 
            (MSC_BOT_cbw.bCBLength > 16))
  {
    	log_usb_debuginfo("cbw decode param err.\r\n");
    SCSI_SenseCode(MSC_BOT_cbw.bLUN, 
                   ILLEGAL_REQUEST, 
                   INVALID_CDB);
     MSC_BOT_Status = BOT_STATE_ERROR;   
    MSC_BOT_Abort(pdev);
 
  }
  else
  {
  	log_usb_debuginfo("cbw decode param correct.\r\n");
    if(SCSI_ProcessCmd(pdev,
                              MSC_BOT_cbw.bLUN,
                              &MSC_BOT_cbw.CB[0]) < 0)
    {
    	log_usb_debuginfo("bot abort.\r\n");
      MSC_BOT_Abort(pdev);
    }
    /*Burst xfer handled internally*/
    else if ((MSC_BOT_State != BOT_DATA_IN) && 
             (MSC_BOT_State != BOT_DATA_OUT) &&
             (MSC_BOT_State != BOT_LAST_DATA_IN)) 
    {
      if (MSC_BOT_DataLen > 0)
      {
      	log_usb_debuginfo("bot snd data.\r\n");
        MSC_BOT_SendData(pdev,
                         MSC_BOT_Data, 
                         MSC_BOT_DataLen);
      }
      else if (MSC_BOT_DataLen == 0) 
      {
      	log_usb_debuginfo("bot snd csw.\r\n");
        MSC_BOT_SendCSW (pdev,
                         CSW_CMD_PASSED);
      }
    }
  }
}

/**
  * @brief  MSC_BOT_SendData
  *         Send the requested data
  * @param  pdev: device instance
  * @param  buf: pointer to data buffer
  * @param  len: Data Length
  * @retval None
  */
static void  MSC_BOT_SendData(USB_CORE_HANDLE  *pdev,
                              uint8_t* buf, 
                              uint16_t len)
{
  log_usb_debuginfo("%s\r\n",__func__);
  len = MIN (MSC_BOT_cbw.dDataLength, len);
  MSC_BOT_csw.dDataResidue -= len;
  MSC_BOT_csw.bStatus = CSW_CMD_PASSED;
  MSC_BOT_State = BOT_SEND_DATA;
  
  DCD_EP_Tx (pdev, MSC_IN_EP, buf, len, USB_IN_BUFFER_WAIT);  
}

/**
  * @brief  MSC_BOT_SendCSW
  *         Send the Command Status Wrapper
  * @param  pdev: device instance
  * @param  status : CSW status
  * @retval None
  */
void  MSC_BOT_SendCSW (USB_CORE_HANDLE  *pdev,
                              uint8_t CSW_Status)
{
 	log_usb_debuginfo("%s\r\n",__func__);

  MSC_BOT_csw.dSignature = BOT_CSW_SIGNATURE;
  MSC_BOT_csw.bStatus = CSW_Status;
  MSC_BOT_State = BOT_IDLE;
  
  DCD_EP_Tx (pdev, 
             MSC_IN_EP, 
             (uint8_t *)&MSC_BOT_csw, 
             BOT_CSW_LENGTH,
             USB_IN_BUFFER_WAIT);
  
  /* Prepare EP to Receive next Cmd */
  DCD_EP_PrepareRx (pdev,
                    MSC_OUT_EP,
                    (uint8_t *)&MSC_BOT_cbw, 
                    BOT_CBW_LENGTH);  
  
}

/**
  * @brief  MSC_BOT_Abort
  *         Abort the current transfer
  * @param  pdev: device instance
  * @retval status
  */

static void  MSC_BOT_Abort (USB_CORE_HANDLE  *pdev)
{
	log_usb_debuginfo("%s\r\n",__func__);

  if ((MSC_BOT_cbw.bmFlags == 0) && 
      (MSC_BOT_cbw.dDataLength != 0) &&
      (MSC_BOT_Status == BOT_STATE_NORMAL) )
  {
    DCD_EP_Stall(pdev, MSC_OUT_EP );
  }
  DCD_EP_Stall(pdev, MSC_IN_EP);
  
  if(MSC_BOT_Status == BOT_STATE_ERROR)
  {
    DCD_EP_PrepareRx (pdev,
                      MSC_OUT_EP,
                      (uint8_t *)&MSC_BOT_cbw, 
                      BOT_CBW_LENGTH);    
  }
}

/**
  * @brief  MSC_BOT_CplClrFeature
  *         Complete the clear feature request
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval None
  */

void  MSC_BOT_CplClrFeature (USB_CORE_HANDLE  *pdev, uint8_t epnum)
{
	log_usb_debuginfo("%s\r\n",__func__);

  if(MSC_BOT_Status == BOT_STATE_ERROR )/* Bad CBW Signature */
  {
    DCD_EP_Stall(pdev, MSC_IN_EP);
    MSC_BOT_Status = BOT_STATE_NORMAL;    
  }
  else if(((epnum & 0x80) == 0x80) && ( MSC_BOT_Status != BOT_STATE_RECOVERY))
  {
    MSC_BOT_SendCSW (pdev, CSW_CMD_FAILED);
  }
  
}
