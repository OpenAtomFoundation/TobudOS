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
  * @file    usbd_msc_core.c
  * @author Nations Solution Team
  * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
  * @brief   This file provides all the MSC core functions.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                MSC Class  Description
  *          =================================================================== 
  *           This module manages the MSC class V1.0 following the "Universal 
  *           Serial Bus Mass Storage Class (MSC) Bulk-Only Transport (BOT) Version 1.0
  *           Sep. 31, 1999".
  *           This driver implements the following aspects of the specification:
  *             - Bulk-Only Transport protocol
  *             - Subclass : SCSI transparent command set (ref. SCSI Primary Commands - 3 (SPC-3))
  *      
  *  @endverbatim
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_msc_core.h"
#include "n32g020xx_usb_log.h"
#include "n32g020xx_log.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
uint8_t  USBD_MSC_Init (void  *pdev, 
                            uint8_t cfgidx);

uint8_t  USBD_MSC_DeInit (void  *pdev, 
                              uint8_t cfgidx);

uint8_t  USBD_MSC_Setup (void  *pdev, 
                             USB_SETUP_REQ *req);

uint8_t  USBD_MSC_DataIn (void  *pdev, 
                              uint8_t epnum);


uint8_t  USBD_MSC_DataOut (void  *pdev, 
                               uint8_t epnum);

uint8_t  *USBD_MSC_GetCfgDesc (uint8_t speed, 
                                      uint16_t *length);

USBD_Class_cb_TypeDef  USBD_MSC_cb = 
{
  USBD_MSC_Init,
  USBD_MSC_DeInit,
  USBD_MSC_Setup,
  NULL, /*EP0_TxSent*/  
  NULL, /*EP0_RxReady*/
  USBD_MSC_DataIn,
  USBD_MSC_DataOut,
  NULL, /*SOF */      
  USBD_MSC_GetCfgDesc,
};

/* USB Mass storage device Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t USBD_MSC_CfgDesc[USB_MSC_CONFIG_DESC_SIZ] =
{ 
  0x09,   /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,   /* bDescriptorType: Configuration */
  USB_MSC_CONFIG_DESC_SIZ,
  
  0x00,
  0x01,   /* bNumInterfaces: 1 interface */
  0x01,   /* bConfigurationValue: */
  0x04,   /* iConfiguration: */
  0xC0,   /* bmAttributes: */
  0x32,   /* MaxPower 100 mA */
  
  /********************  Mass Storage interface ********************/
  0x09,   /* bLength: Interface Descriptor size */
  0x04,   /* bDescriptorType: */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints*/
  0x08,   /* bInterfaceClass: MSC Class */
  0x06,   /* bInterfaceSubClass : SCSI transparent*/
  0x50,   /* nInterfaceProtocol */
  0x05,          /* iInterface: */
  /********************  Mass Storage Endpoints ********************/
  0x07,   /*Endpoint descriptor length = 7*/
  0x05,   /*Endpoint descriptor type */
  MSC_IN_EP,   /*Endpoint address (IN, address 1) */
  0x02,   /*Bulk endpoint type */
  LOBYTE(MSC_MAX_PACKET),
  HIBYTE(MSC_MAX_PACKET),
  0x00,   /*Polling interval in milliseconds */ 
  0x07,   /*Endpoint descriptor length = 7 */
  0x05,   /*Endpoint descriptor type */
  MSC_OUT_EP,   /*Endpoint address (OUT, address 1) */
  0x02,   /*Bulk endpoint type */
  LOBYTE(MSC_MAX_PACKET),
  HIBYTE(MSC_MAX_PACKET),
  0x00     /*Polling interval in milliseconds*/
};
static uint8_t  USBD_MSC_MaxLun = 0;

static uint8_t  USBD_MSC_AltSet = 0;

/* Private function ----------------------------------------------------------*/
/**
  * @brief  USBD_MSC_Init
  *         Initialize  the mass storage configuration
  * @param  pdev: device instance
  * @param  cfgidx: configuration index
  * @retval status
  */
uint8_t  USBD_MSC_Init (void  *pdev, 
                            uint8_t cfgidx)
{ 
  //DCD_PMA_Config(pdev , MSC_IN_EP,USB_SNG_BUF,MSC_IN_TX_ADDRESS);
  //DCD_PMA_Config(pdev , MSC_OUT_EP,USB_SNG_BUF,MSC_OUT_RX_ADDRESS);
 
  /* Open EP IN */
  DCD_EP_Open(pdev,
              MSC_IN_EP,
              MSC_EPIN_SIZE,
              USB_EP_BULK);
  
  /* Open EP OUT */
  DCD_EP_Open(pdev,
              MSC_OUT_EP,
              MSC_EPOUT_SIZE,
              USB_EP_BULK);
 
  /* Init the BOT  layer */
  MSC_BOT_Init(pdev); 
  
  return USBD_OK;
}

/**
  * @brief  USBD_MSC_DeInit
  *         DeInitilaize  the mass storage configuration
  * @param  pdev: device instance
  * @param  cfgidx: configuration index
  * @retval status
  */
uint8_t  USBD_MSC_DeInit (void  *pdev, 
                              uint8_t cfgidx)
{
  /* Close MSC EPs */
  DCD_EP_Close (pdev , MSC_IN_EP);
  DCD_EP_Close (pdev , MSC_OUT_EP);
  
  /* Un Init the BOT layer */
  MSC_BOT_DeInit(pdev);   
  return USBD_OK;
}
/**
  * @brief  USBD_MSC_Setup
  *         Handle the MSC specific requests
  * @param  pdev: device instance
  * @param  req: USB request
  * @retval status
  */
uint8_t  USBD_MSC_Setup (void  *pdev, USB_SETUP_REQ *req)
{

  log_usb_debuginfo("USBD_MSC_Setup[0x%x,0x%x].\r\n", req->bmRequest, req->bRequest);

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {

  /* Class request */
  case USB_REQ_TYPE_CLASS :
    switch (req->bRequest)
    {
    case BOT_GET_MAX_LUN :

      if((req->wValue  == 0) && 
         (req->wLength == 1) &&
         ((req->bmRequest & 0x80) == 0x80))
      {
        USBD_MSC_MaxLun = USBD_STORAGE_fops->GetMaxLun();
        if(USBD_MSC_MaxLun > 0)
        {
        	log_usb_debuginfo("msc setup.\r\n");
           USBD_CtlSendData (pdev,
                             &USBD_MSC_MaxLun,
                              1,
                              USB_IN_BUFFER_WAIT);
        }
        else
        {
          USBD_CtlError(pdev , req);
          return USBD_FAIL; 
        }
      }
      else
      {
         USBD_CtlError(pdev , req);
         return USBD_FAIL; 
      }
      break;
      
    case BOT_RESET :
      if((req->wValue  == 0) && 
         (req->wLength == 0) &&
        ((req->bmRequest & 0x80) != 0x80))
      {      
         MSC_BOT_Reset(pdev);
      }
      else
      {
         USBD_CtlError(pdev , req);
         return USBD_FAIL; 
      }
      break;

    default:
       USBD_CtlError(pdev , req);
       return USBD_FAIL; 
    }
    break;
  /* Interface & Endpoint request */
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev,
                        &USBD_MSC_AltSet,
                        1,
                        USB_IN_BUFFER_WAIT);
      break;
      
    case USB_REQ_SET_INTERFACE :
      USBD_MSC_AltSet = (uint8_t)(req->wValue);
      break;
    
    case USB_REQ_CLEAR_FEATURE:  
       
      /* Handle BOT error */
      MSC_BOT_CplClrFeature(pdev, (uint8_t)req->wIndex);
      break;
      
    }  
    break;
   
  default:
    break;
  }
  return USBD_OK;
}

/**
  * @brief  USBD_MSC_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
uint8_t  USBD_MSC_DataIn (void  *pdev, 
                              uint8_t epnum)
{
  MSC_BOT_DataIn(pdev , epnum);
  return USBD_OK;
}

/**
  * @brief  USBD_MSC_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
uint8_t  USBD_MSC_DataOut (void  *pdev, 
                               uint8_t epnum)
{
  MSC_BOT_DataOut(pdev , epnum);
  return USBD_OK;
}

/**
  * @brief  USBD_MSC_GetCfgDesc 
  *         return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
uint8_t  *USBD_MSC_GetCfgDesc (uint8_t speed, uint16_t *length)
{
  *length = sizeof (USBD_MSC_CfgDesc);
  return (uint8_t *)USBD_MSC_CfgDesc;
}
