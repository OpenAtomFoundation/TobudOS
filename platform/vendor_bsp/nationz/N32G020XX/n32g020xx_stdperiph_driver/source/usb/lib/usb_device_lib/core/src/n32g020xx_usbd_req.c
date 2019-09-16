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
 ******************************************************************************
 * @file n32g020xx_usbd_req.c
 * @author Nations Solution Team
 * @version v1.0.1
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @date    2019-01-19
 * @brief   文件提供标准USB请求.包含了USB规范的第9章节列出的请求。
 *
 * @addtogroup USB
 * @{
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "n32g020xx_usbd_req.h"
#include "n32g020xx_usb_log.h"
#include "n32g020xx_log.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t ADDRESS = 0;
uint8_t USBD_StrDesc[USB_MAX_STR_DESC_SIZ];

/* Private function prototypes -----------------------------------------------*/
static void USBD_GetDescriptor(USB_CORE_HANDLE* pdev, USB_SETUP_REQ* req);

static void USBD_SetDescriptor(USB_CORE_HANDLE* pdev, USB_SETUP_REQ* req);

static void USBD_SetAddress(USB_CORE_HANDLE* pdev, USB_SETUP_REQ* req);

static void USBD_SetConfig(USB_CORE_HANDLE* pdev, USB_SETUP_REQ* req);

static void USBD_GetConfig(USB_CORE_HANDLE* pdev, USB_SETUP_REQ* req);

static uint8_t USBD_GetLen(uint8_t* buf);

/* Private functions ---------------------------------------------------------*/
/**
 * @brief  USBD_StdDevReq
 *         Handle standard usb device requests
 * @param  pdev: device instance
 * @param  req: usb request
 * @retval status
 */
USBD_Status USBD_StdDevReq(USB_CORE_HANDLE* pdev, USB_SETUP_REQ* req)
{
    USBD_Status ret = USBD_OK;

    switch (req->bRequest)
    {
    case USB_REQ_GET_DESCRIPTOR:
        log_usb_debuginfo("@req descriptor.\r\n");

        if ((req->bmRequest & 0xFC) != 0x80)
        {
            log_usb_debuginfo("***req descriptor err.***\r\n");
        }

        USBD_GetDescriptor(pdev, req);
        break;

    case USB_REQ_SET_DESCRIPTOR:
        log_usb_debuginfo("@set descriptor. \r\n");
        USBD_SetDescriptor(pdev, req);
        break;

    case USB_REQ_SET_ADDRESS:
        log_usb_debuginfo("@set address. \r\n");
        USBD_SetAddress(pdev, req);
        break;

    case USB_REQ_SET_CONFIGURATION:
        log_usb_debuginfo("@set configuration. \r\n");
        USBD_SetConfig(pdev, req);
        break;

    case USB_REQ_GET_CONFIGURATION:
        log_usb_debuginfo("@set configuration. \r\n");
        if ((req->bmRequest & 0xFC) != 0x80)
        {
            log_usb_debuginfo("***req cfg descriptor err.***\r\n");
        }
        USBD_GetConfig(pdev, req);
        break;

    case USB_REQ_GET_STATUS:
        log_usb_debuginfo("@get status. \r\n");
        // USBD_CtlError(pdev , req);
        break;

    case USB_REQ_SET_FEATURE:
        log_usb_debuginfo("@set feature. \r\n");
        // USBD_CtlError(pdev , req);
        break;

    case USB_REQ_CLEAR_FEATURE:
        log_usb_debuginfo("@clear feature. \r\n");
        // USBD_CtlError(pdev , req);
        break;

    default:
        USBD_CtlError(pdev, req);
        break;
    }

    return ret;
}

/**
 * @brief  USBD_StdItfReq
 *         Handle standard usb interface requests
 * @param  pdev: USB device instance
 * @param  req: usb request
 * @retval status
 */
USBD_Status USBD_StdItfReq(USB_CORE_HANDLE* pdev, USB_SETUP_REQ* req)
{
    USBD_Status ret = USBD_OK;

	log_usb_debuginfo("USBD_StdItfReq.device_status =%x,[index=0x%0x] \r\n", pdev->dev.device_status, req->wIndex);
    switch (pdev->dev.device_status)
    {
    case USB_CONFIGURED:

        if (LOBYTE(req->wIndex) <= USBD_ITF_MAX_NUM)
        {
        	log_usb_debuginfo("wIndex < MAX.\r\n");
            ret = (USBD_Status)(pdev->dev.class_cb->Setup(pdev, req));

            if ((req->wLength == 0) && (ret == USBD_OK))
            {
            	log_usb_debuginfo("itfreq:snd zero.\r\n");
                USBD_CtlSendStatus(pdev);
            }
        }
        else
        {
        	log_usb_debuginfo("err:wIndex[0x%0x] > MAX.\r\n", LOBYTE(req->wIndex));
            USBD_CtlError(pdev, req);
        }
        break;

    default:
        USBD_CtlError(pdev, req);
        break;
    }
    return ret;
}

/**
 * @brief  USBD_StdEPReq
 *         Handle standard usb endpoint requests
 * @param  pdev: USB device instance
 * @param  req: usb request
 * @retval status
 */
USBD_Status USBD_StdEPReq(USB_CORE_HANDLE* pdev, USB_SETUP_REQ* req)
{
    uint8_t ep_addr;
    uint32_t USBD_ep_status = 0;
    USBD_Status ret         = USBD_OK;

    ep_addr = LOBYTE(req->wIndex);

    switch (req->bRequest)
    {
    case USB_REQ_SET_FEATURE:

        switch (pdev->dev.device_status)
        {
        case USB_ADDRESSED:
            if ((ep_addr != 0x00) && (ep_addr != 0x80))
            {
                DCD_EP_Stall(pdev, ep_addr);
            }
            break;

        case USB_CONFIGURED:
            if (req->wValue == USB_FEATURE_EP_HALT)
            {
                if ((ep_addr != 0x00) && (ep_addr != 0x80))
                {
                    DCD_EP_Stall(pdev, ep_addr);
                }
            }
            pdev->dev.class_cb->Setup(pdev, req);
            USBD_CtlSendStatus(pdev);

            break;

        default:
            USBD_CtlError(pdev, req);
            break;
        }
        break;

    case USB_REQ_CLEAR_FEATURE:

        switch (pdev->dev.device_status)
        {
        case USB_ADDRESSED:
            if ((ep_addr != 0x00) && (ep_addr != 0x80))
            {
                DCD_EP_Stall(pdev, ep_addr);
            }
            break;

        case USB_CONFIGURED:
            if (req->wValue == USB_FEATURE_EP_HALT)
            {
                if ((ep_addr != 0x00) && (ep_addr != 0x80))
                {
                    DCD_EP_ClrStall(pdev, ep_addr);
                }
            }
            pdev->dev.class_cb->Setup(pdev, req);
            USBD_CtlSendStatus(pdev);
            break;

        default:
            USBD_CtlError(pdev, req);
            break;
        }
        break;

    case USB_REQ_GET_STATUS:
        switch (pdev->dev.device_status)
        {
        case USB_ADDRESSED:
            if ((ep_addr != 0x00) && (ep_addr != 0x80))
            {
                DCD_EP_Stall(pdev, ep_addr);
            }
            break;

        case USB_CONFIGURED:

            if ((ep_addr & 0x80) == 0x80)
            {
                if (pdev->dev.in_ep[ep_addr & 0x7F].is_stall)
                {
                    USBD_ep_status = 0x0001;
                }
                else
                {
                    USBD_ep_status = 0x0000;
                }
            }
            else if ((ep_addr & 0x80) == 0x00)
            {
                if (pdev->dev.out_ep[ep_addr].is_stall)
                {
                    USBD_ep_status = 0x0001;
                }

                else
                {
                    USBD_ep_status = 0x0000;
                }
            }
            USBD_CtlSendData(pdev, (uint8_t*)&USBD_ep_status, 2, USB_IN_BUFFER_WAIT);
            break;

        default:
            USBD_CtlError(pdev, req);
            break;
        }
        break;

    default:
        break;
    }
    return ret;
}
/**
 * @brief  USBD_GetDescriptor
 *         Handle Get Descriptor requests
 * @param  pdev: device instance
 * @param  req: usb request
 * @retval status
 */
static void USBD_GetDescriptor(USB_CORE_HANDLE* pdev, USB_SETUP_REQ* req)
{
    uint16_t len  = 0;
    uint8_t* pbuf = NULL;

    /* class_cb->USBD_CDC_cb */
    /* usr_device->USR_desc */
    /* usr_cb->USR_cb */
    /* op->:USBD_DCD_INT_cb */

    log_usb_debuginfo("GetDescriptor , req->wValue = %x.\r\n", req->wValue);

    switch (req->wValue >> 8)
    {
#ifdef LPM_ENABLED
    case USB_DESC_TYPE_BOS:
        pbuf = pdev->dev.usr_device->GetBOSDescriptor(pdev->dev.speed, &len);
        break;
#endif
    case USB_DESC_TYPE_DEVICE:
		log_usb_debuginfo("(get device desc).\r\n");
        pbuf = pdev->dev.usr_device->GetDeviceDescriptor(pdev->dev.speed, &len);
        log_usb_debuginfo("req->wLength = %x,\r\n", req->wLength);
        if (len > req->wLength)
        {
            log_usb_debuginfo("cfg desc len[%x] > req_len[%x],\r\n", len, req->wLength);
            len = req->wLength;
        }
        break;

    case USB_DESC_TYPE_CONFIGURATION:
		log_usb_debuginfo("(get configuration desc).\r\n");
        pbuf = (uint8_t*)pdev->dev.class_cb->GetConfigDescriptor(pdev->dev.speed, &len); /* USBD_cdc_GetCfgDesc */
        pdev->dev.pConfig_descriptor = pbuf;
        break;

    case USB_DESC_TYPE_STRING:
		log_usb_debuginfo("(get string):");
        switch ((uint8_t)(req->wValue))
        {
        case USBD_IDX_LANGID_STR:
			log_usb_debuginfo("langid.\r\n");
            pbuf = pdev->dev.usr_device->GetLangIDStrDescriptor(pdev->dev.speed, &len);
            break;

        case USBD_IDX_MFC_STR:
			log_usb_debuginfo("mfc.\r\n");
            pbuf = pdev->dev.usr_device->GetManufacturerStrDescriptor(pdev->dev.speed, &len);
            break;

        case USBD_IDX_PRODUCT_STR:
			log_usb_debuginfo("product.\r\n");
            pbuf = pdev->dev.usr_device->GetProductStrDescriptor(pdev->dev.speed, &len);
            break;

        case USBD_IDX_SERIAL_STR:
			log_usb_debuginfo("seral.\r\n");
            pbuf = pdev->dev.usr_device->GetSerialStrDescriptor(pdev->dev.speed, &len);
            break;

        case USBD_IDX_CONFIG_STR:
			log_usb_debuginfo("config.\r\n");
            pbuf = pdev->dev.usr_device->GetConfigurationStrDescriptor(pdev->dev.speed, &len);
            break;

        case USBD_IDX_INTERFACE_STR:
			log_usb_debuginfo("interface.\r\n");
            pbuf = pdev->dev.usr_device->GetInterfaceStrDescriptor(pdev->dev.speed, &len);
            break;

        default:
#ifdef USB_SUPPORT_USER_STRING_DESC
            pbuf = pdev->dev.class_cb->GetUsrStrDescriptor(pdev->dev.speed, (req->wValue), &len);
            break;
#else
            pbuf = pdev->dev.usr_device->GetLangIDStrDescriptor(pdev->dev.speed, &len);
            return;
#endif
        }
        break;
    case USB_DESC_TYPE_DEVICE_QUALIFIER:
       log_usb_debuginfo("(get qualifier).\r\n");
        USBD_CtlError(pdev, req);
        return;

    case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		log_usb_debuginfo("(get speed cfg).\r\n");
        USBD_CtlError(pdev, req);
        return;

    case USB_DESC_TYPE_BOS:
		log_usb_debuginfo("(get bos).\r\n");
        USBD_CtlError(pdev, req);
        return;
    default:
		log_usb_debuginfo("(get desc error).\r\n");
        USBD_CtlError(pdev, req);
        return;
    }

    if ((len != 0) && (req->wLength != 0))
    {
        log_usb_debuginfo("len =0x%x , req->wLength = 0x%x.\r\n", len, req->wLength);

        len = MIN(len, req->wLength);

        USBD_CtlSendData(pdev, pbuf, len, USB_IN_BUFFER_WAIT);
    }
}

/**
 * @brief  USBD_SetDescriptor
 *         Handle Set Descriptor requests
 * @param  pdev: device instance
 * @param  req: usb request
 * @retval status
 */
static void USBD_SetDescriptor(USB_CORE_HANDLE* pdev, USB_SETUP_REQ* req)
{
    USB->EP0CSR |= EPCLR;
    if (!(USB->TKIE & EP0INTK))
    {
        USB->EPCSR = EP0INRDY;
    }
}

/**
 * @brief  USBD_SetAddress
 *         Set device address
 * @param  pdev: device instance
 * @param  req: usb request
 * @retval status
 */
static void USBD_SetAddress(USB_CORE_HANDLE* pdev, USB_SETUP_REQ* req)
{
    uint8_t dev_addr;

    if ((req->wIndex == 0) && (req->wLength == 0))
    {
        dev_addr = (uint8_t)(req->wValue) & 0x7F;

        if (pdev->dev.device_status == USB_CONFIGURED)
        {
            USBD_CtlError(pdev, req);
        }
        else
        {
            pdev->dev.device_address = dev_addr;
            ADDRESS                  = dev_addr;
            USBD_CtlSendStatus(pdev);

            if (dev_addr != 0)
            {
                pdev->dev.device_status = USB_ADDRESSED;
            }
            else
            {
                pdev->dev.device_status = USB_DEFAULT;
            }
        }
    }
    else
    {
        USBD_CtlError(pdev, req);
    }
}

/**
 * @brief  USBD_SetConfig
 *         Handle Set device configuration request
 * @param  pdev: device instance
 * @param  req: usb request
 * @retval status
 */
static void USBD_SetConfig(USB_CORE_HANDLE* pdev, USB_SETUP_REQ* req)
{
    static uint8_t cfgidx;

    log_usb_debuginfo("USBD_SetConfig \r\n");
    USB->EP0CSR |= EPCLR;

    if (req->bmRequest != 0x00)
    {
        USBD_CtlError(pdev, req);
        return;
    }
    if (req->wIndex != 0)
    {
        USBD_CtlError(pdev, req);
        return;
    }
    if (req->wLength != 0x00)
    {
        USBD_CtlError(pdev, req);
        return;
    }

    cfgidx = (uint8_t)(req->wValue);
    if (cfgidx > USBD_CFG_MAX_NUM)
    {
        USBD_CtlError(pdev, req);
    }
    else
    {
        USB->DEVCFG |= 0x01;
        log_usb_debuginfo("setcfg:device_status = %x.\r\n", pdev->dev.device_status);

        switch (pdev->dev.device_status)
        {
		case USB_ADDRESSED:
		  if (cfgidx) 
		  { 																												
			pdev->dev.device_config = cfgidx;
			pdev->dev.device_status = USB_CONFIGURED;
			USBD_SetCfg(pdev , cfgidx);
			log_usb_debuginfo("setcfg:USB_CONFIGURED.\r\n");

			//USBD_CtlSendStatus(pdev);
		  }
		  else 
		  {
			 //USBD_CtlSendStatus(pdev);
			 log_usb_debuginfo("setcfg:USB_CONFIGURED err.\r\n");
		  }
		  break;
		  
		case USB_CONFIGURED:
		  if (cfgidx == 0) 
		  { 						  
			pdev->dev.device_status = USB_ADDRESSED;
			pdev->dev.device_config = cfgidx;		   
			USBD_ClrCfg(pdev , cfgidx);
			log_usb_debuginfo("setcfg:USB_ADDRESSED.\r\n");
			//USBD_CtlSendStatus(pdev);
		  } 
		  else	if (cfgidx != pdev->dev.device_config) 
		  {
			/* Clear old configuration */
			USBD_ClrCfg(pdev , pdev->dev.device_config);
			log_usb_debuginfo("setcfg: USB_ADDRESSED err1.\r\n");
			/* set new configuration */
			pdev->dev.device_config = cfgidx;
			USBD_SetCfg(pdev , cfgidx);
			//USBD_CtlSendStatus(pdev);
		  }
		  else
		  {
			//USBD_CtlSendStatus(pdev);
			log_usb_debuginfo("setcfg: USB_ADDRESSED err2.\r\n");
		  }
		  break;
        default:
             USBD_CtlError(pdev , req);
            break;
        }
    }

    if (!(USB->TKIE & EP0INTK))
    {
        log_usb_debuginfo("USB_TKIE DISABLE IRQ. \r\n");
        USB->EPCSR = EP0INRDY;
    }
}

/**
 * @brief  USBD_GetConfig
 *         Handle Get device configuration request
 * @param  pdev: device instance
 * @param  req: usb request
 * @retval status
 */
static void USBD_GetConfig(USB_CORE_HANDLE* pdev, USB_SETUP_REQ* req)
{
    uint32_t USBD_default_cfg = 0;

    log_usb_debuginfo("USBD_GetConfig \r\n");
    USB->EP0CSR |= EPCLR;

    if (req->bmRequest != 0x80)
    {
        USBD_CtlError(pdev, req);
        return;
    }
    if (req->wValue != 0)
    {
        USBD_CtlError(pdev, req);
        return;
    }
    if (req->wIndex != 0)
    {
        USBD_CtlError(pdev, req);
        return;
    }
    if (req->wLength != 0x01)
    {
        USBD_CtlError(pdev, req);
        return;
    }

    switch (pdev->dev.device_status)
    {
    case USB_ADDRESSED:
        USBD_CtlSendData(pdev, (uint8_t*)&USBD_default_cfg, 1, USB_IN_BUFFER_WAIT);
        break;

    case USB_CONFIGURED:
        USBD_CtlSendData(pdev, &pdev->dev.device_config, 1, USB_IN_BUFFER_WAIT);
        break;

    default:
        USBD_CtlError(pdev, req);
        break;
    }
}

/**
 * @brief  USBD_ParseSetupRequest
 *         Copy buffer into setup structure
 * @param  pdev: device instance
 * @param  req: usb request
 * @retval None
 */

void USBD_ParseSetupRequest(USB_CORE_HANDLE* pdev, USB_SETUP_REQ* req)
{
    log_usb_debuginfo("parse start.\r\n");

    /* Set EP0 FIFO pointer goback to zero */
    USB->EP0CSR |= EPCLR;

    req->bmRequest = *(uint8_t*)(pdev->dev.setup_packet);
    req->bRequest  = *(uint8_t*)(pdev->dev.setup_packet + 1);
    req->wValue    = SWAPBYTE(pdev->dev.setup_packet + 2);
    req->wIndex    = SWAPBYTE(pdev->dev.setup_packet + 4);
    req->wLength   = SWAPBYTE(pdev->dev.setup_packet + 6);

    log_usb_debuginfo("bmRequest:%x.\r\n", req->bmRequest);
    log_usb_debuginfo("bRequest:%x.\r\n", req->bRequest);
    log_usb_debuginfo("wValue:%x.\r\n", req->wValue);
    log_usb_debuginfo("wIndex:%x.\r\n", req->wIndex);
    log_usb_debuginfo("wLength:%x.\r\n", req->wLength);

    pdev->dev.in_ep[0].ctl_data_len = req->wLength;
    pdev->dev.device_state          = USB_EP0_SETUP;
}

/**
 * @brief  USBD_CtlError
 *         Handle USB low level Error
 * @param  pdev: device instance
 * @param  req: usb request
 * @retval None
 */

void USBD_CtlError(USB_CORE_HANDLE* pdev, USB_SETUP_REQ* req)
{
    DCD_EP_Stall(pdev, 0);
}

/**
 * @brief  USBD_GetString
 *         Convert Ascii string into unicode one
 * @param  desc : descriptor buffer
 * @param  unicode : Formatted string buffer (unicode)
 * @param  len : descriptor length
 * @retval None
 */
void USBD_GetString(uint8_t* desc, uint8_t* unicode, uint8_t bufsize, uint16_t* len)
{
    uint8_t idx = 0;

    if (desc != NULL)
    {
        *len           = USBD_GetLen(desc) * 2 + 2;
		if (idx < bufsize) {
			unicode[idx++] = *len;
		} else {
			log_usb_error("USBD_GetString:buf overflow1.idx = %x, bufsize = %x\r\n", idx, bufsize);
		}

		if (idx < bufsize) {
			unicode[idx++] = USB_DESC_TYPE_STRING;
		} else {
			log_usb_error("USBD_GetString:buf overflow2.idx = %x, bufsize = %x\r\n", idx, bufsize);
		}

        while (*desc != NULL)
        {
			if (idx < bufsize) {
				unicode[idx++] = *desc++;
			} else {
				log_usb_error("USBD_GetString:buf overflow3.idx = %x, bufsize = %x\r\n", idx, bufsize);
			}

			if (idx < bufsize) {
				unicode[idx++] = 0x00;
			} else {
				log_usb_error("USBD_GetString:buf overflow4.idx = %x, bufsize = %x\r\n", idx, bufsize);
			}
        }
    }
}

/**
 * @brief  USBD_GetLen
 *         return the string length
 * @param  buf : pointer to the ascii string buffer
 * @retval string length
 */
static uint8_t USBD_GetLen(uint8_t* buf)
{
    uint8_t len = 0;

    while (*buf != NULL)
    {
        len++;
        buf++;
    }

    return len;
}



/** @} */
