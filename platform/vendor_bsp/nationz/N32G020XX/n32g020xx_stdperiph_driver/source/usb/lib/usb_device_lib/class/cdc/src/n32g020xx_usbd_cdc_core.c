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
 * @file n32g020xx_usbd_cdc_core.c
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @date    2019-01-19
 * @brief   文件提供了上层接口函数用于管理 USB CDC 类.
 *          USB CDC(Communication Device Class)类主要包含以下的功能 :
 *           - 初始化并配置底层和上层回调。
 *           - 作为CDC设备枚举。
 *           - 通过OUT/IN端点进行输出的输入和输出功能。
 *           - 枚举过程中的命令传送。
 *           - 以及相关的错误管理。
 *
 *  @verbatim
 *
 *          ===================================================================
 *                                CDC (Communication Device Class)类驱动描述
 *          ===================================================================
 *           驱动管理 "Universal Serial Bus Class Definitions for Communications Devices
 *           Revision 1.2 November 16, 2007" 和子协议规范"Universal Serial Bus
 *           Communications Class Subclass Specification for PSTN Devices Revision 1.2 February 9, 2007"
 *           驱动实现了规范中的如下几个方面 :
 *             - 设备描述管理
 *             - 配置描述管理
 *             - 作为带有2个数据端点(IN/OUT)和一个命令端点(IN)的CDC 设备进行枚举。
 *             - 请求管理 (作为规范中6.2章节所描述)
 *             - 抽象控制模型
 *             - 数据接口类
 * @endverbatim
 * 
 * @addtogroup USB
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "n32g020xx_usbd_cdc_core.h"
#include "n32g020xx_usbd_cdc_vcp.h"
#include "n32g020xx_usb_log.h"
#include "n32g020xx_log.h"
#ifndef LOG_ENABLE
#include <stdio.h>
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/*********************************************
   CDC Device library callbacks
 *********************************************/
uint8_t usbd_cdc_Init(void* pdev, uint8_t cfgidx);
uint8_t usbd_cdc_DeInit(void* pdev, uint8_t cfgidx);
uint8_t usbd_cdc_Setup(void* pdev, USB_SETUP_REQ* req);
uint8_t usbd_cdc_EP0_RxReady(void* pdev);
uint8_t usbd_cdc_DataIn(void* pdev, uint8_t epnum);
uint8_t usbd_cdc_DataOut(void* pdev, uint8_t epnum);
uint8_t usbd_cdc_SOF(void* pdev);
/*********************************************
   CDC specific management functions
 *********************************************/
static void Handle_USBAsynchXfer(void* pdev);
static uint8_t* USBD_cdc_GetCfgDesc(uint8_t speed, uint16_t* length);

extern CDC_IF_Prop_TypeDef APP_FOPS;
extern uint8_t USBD_DeviceDesc[USB_SIZ_DEVICE_DESC];

uint8_t usbd_cdc_OtherCfgDesc[USB_CDC_CONFIG_DESC_SIZ];

static __IO uint32_t usbd_cdc_AltSet = 0;

uint8_t USB_Rx_Buffer[CDC_DATA_MAX_PACKET_SIZE];

uint8_t APP_Rx_Buffer[APP_RX_DATA_SIZE];

uint8_t CmdBuff[CDC_CMD_PACKET_SZE];
__IO uint32_t last_packet = 0;
uint32_t APP_Rx_ptr_in    = 0;
uint32_t APP_Rx_ptr_out   = 0;
uint32_t APP_Rx_length    = 0;

uint8_t USB_Tx_State = 0;

static uint32_t cdcCmd     = 0xFF;
static uint32_t cdcLen     = 0;
static uint8_t const* pstr = {"VirtualCom Test By Usb Interface [%d] Times.\r\n"};
static uint8_t chBufData[CDC_DATA_MAX_PACKET_SIZE];
static uint32_t chBufCnt = 0;

/* CDC interface class callbacks structure */
USBD_Class_cb_TypeDef USBD_CDC_cb = {
    usbd_cdc_Init,
    usbd_cdc_DeInit,
    usbd_cdc_Setup,
    NULL, /* EP0_TxSent, */
    usbd_cdc_EP0_RxReady,
    usbd_cdc_DataIn,
    usbd_cdc_DataOut,
    usbd_cdc_SOF,
    USBD_cdc_GetCfgDesc,
};

/* USB CDC device Configuration Descriptor */
const uint8_t usbd_cdc_CfgDesc[USB_CDC_CONFIG_DESC_SIZ] = {
    /*Configuration Descriptor*/
    0x09,                              /* bLength: Configuration Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
    USB_CDC_CONFIG_DESC_SIZ,           /* wTotalLength:no of returned bytes */
    0x00,
    0x02, /* bNumInterfaces: 2 interface */
    0x01, /* bConfigurationValue: Configuration value */
    0x00, /* iConfiguration: Index of string descriptor describing the configuration */
    0xC0, /* bmAttributes: self powered */
    0x32, /* MaxPower 0 mA */

    /*---------------------------------------------------------------------------*/

    /*Interface Descriptor */
    0x09,                          /* bLength: Interface Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: Interface */
    /* Interface descriptor type */
    0x00, /* bInterfaceNumber: Number of Interface */
    0x00, /* bAlternateSetting: Alternate setting */
    0x01, /* bNumEndpoints: One endpoints used */
    0x02, /* bInterfaceClass: Communication Interface Class */
    0x02, /* bInterfaceSubClass: Abstract Control Model */
    0x01, /* bInterfaceProtocol: Common AT commands */
    0x00, /* iInterface: */

    /*Header Functional Descriptor*/
    0x05, /* bLength: Endpoint Descriptor size */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x00, /* bDescriptorSubtype: Header Func Desc */
    0x10, /* bcdCDC: spec release number */
    0x01,

    /*Call Management Functional Descriptor*/
    0x05, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x01, /* bDescriptorSubtype: Call Management Func Desc */
    0x00, /* bmCapabilities: D0+D1 */
    0x01, /* bDataInterface: 1 */

    /*ACM Functional Descriptor*/
    0x04, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x02, /* bDescriptorSubtype: Abstract Control Management desc */
    0x02, /* bmCapabilities */

    /*Union Functional Descriptor*/
    0x05, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x06, /* bDescriptorSubtype: Union func desc */
    0x00, /* bMasterInterface: Communication class interface */
    0x01, /* bSlaveInterface0: Data Class Interface */

    /*Endpoint 2 Descriptor*/
    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
    CDC_CMD_EP,                   /* bEndpointAddress */
    0x03,                         /* bmAttributes: Interrupt */
    LOBYTE(CDC_CMD_PACKET_SZE),   /* wMaxPacketSize: */
    HIBYTE(CDC_CMD_PACKET_SZE),
    0xFF, /* bInterval: */
    /*---------------------------------------------------------------------------*/

    /*Data class interface descriptor*/
    0x09,                          /* bLength: Endpoint Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: */
    0x01,                          /* bInterfaceNumber: Number of Interface */
    0x00,                          /* bAlternateSetting: Alternate setting */
    0x02,                          /* bNumEndpoints: Two endpoints used */
    0x0A,                          /* bInterfaceClass: CDC */
    0x00,                          /* bInterfaceSubClass: */
    0x00,                          /* bInterfaceProtocol: */
    0x00,                          /* iInterface: */

    /*Endpoint OUT Descriptor*/
    0x07,                             /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,     /* bDescriptorType: Endpoint */
    CDC_OUT_EP,                       /* bEndpointAddress */
    0x02,                             /* bmAttributes: Bulk */
    LOBYTE(CDC_DATA_MAX_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_DATA_MAX_PACKET_SIZE),
    0x00, /* bInterval: ignore for Bulk transfer */

    /*Endpoint IN Descriptor*/
    0x07,                             /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,     /* bDescriptorType: Endpoint */
    CDC_IN_EP,                        /* bEndpointAddress */
    0x02,                             /* bmAttributes: Bulk */
    LOBYTE(CDC_DATA_MAX_PACKET_SIZE), /* wMaxPacketSize: */
    HIBYTE(CDC_DATA_MAX_PACKET_SIZE),
    0x00 /* bInterval: ignore for Bulk transfer */
};

/* Private function ----------------------------------------------------------*/
/**
 * @brief  usbd_cdc_Init
 *         Initialize the CDC interface
 * @param  pdev: device instance
 * @param  cfgidx: Configuration index
 * @retval status
 */
uint8_t usbd_cdc_Init(void* pdev, uint8_t cfgidx)
{
    log_usb_debuginfo("usbd_cdc_Init.\r\n");

    /* Open EP IN */
    DCD_EP_Open(pdev, CDC_IN_EP, CDC_DATA_IN_PACKET_SIZE, USB_EP_BULK);

    /* Open EP OUT */
    DCD_EP_Open(pdev, CDC_OUT_EP, CDC_DATA_OUT_PACKET_SIZE, USB_EP_BULK);

    /* Open Command IN EP */
    DCD_EP_Open(pdev, CDC_CMD_EP, CDC_CMD_PACKET_SZE, USB_EP_INT);

    /* Initialize the Interface physical components */
    APP_FOPS.pIf_Init();

    /* Prepare Out endpoint to receive next packet */
    DCD_EP_PrepareRx(pdev, CDC_OUT_EP, (uint8_t*)(USB_Rx_Buffer), CDC_DATA_OUT_PACKET_SIZE);

    return USBD_OK;
}

/**
 * @brief  usbd_cdc_Init
 *         DeInitialize the CDC layer
 * @param  pdev: device instance
 * @param  cfgidx: Configuration index
 * @retval status
 */
uint8_t usbd_cdc_DeInit(void* pdev, uint8_t cfgidx)
{
    log_usb_debuginfo("usbd_cdc_DeInit.\r\n");
    /* Open EP IN */
    DCD_EP_Close(pdev, CDC_IN_EP);

    /* Open EP OUT */
    DCD_EP_Close(pdev, CDC_OUT_EP);

    /* Open Command IN EP */
    DCD_EP_Close(pdev, CDC_CMD_EP);

    /* Restore default state of the Interface physical components */
    APP_FOPS.pIf_DeInit();

    return USBD_OK;
}

/**
 * @brief  usbd_cdc_Setup
 *         Handle the CDC specific requests
 * @param  pdev: instance
 * @param  req: usb requests
 * @retval status
 */
uint8_t usbd_cdc_Setup(void* pdev, USB_SETUP_REQ* req)
{
    uint16_t len  = USB_CDC_DESC_SIZ;
    uint8_t* pbuf = (uint8_t*)usbd_cdc_CfgDesc + 9;

    log_usb_debuginfo("usbd_cdc_Setup.\r\n");
    switch (req->bmRequest & USB_REQ_TYPE_MASK)
    {
        /* CDC Class Requests -------------------------------*/
    case USB_REQ_TYPE_CLASS:
        /* Check if the request is a data setup packet */
        if (req->wLength)
        {
            log_usb_debuginfo("req->wLength = %x.\r\n", req->wLength);
            /* Check if the request is Device-to-Host */
            if (req->bmRequest & 0x80)
            {
                log_usb_debuginfo("get uart cfg\r\n");

                /* Get the data to be sent to Host from interface layer */
                APP_FOPS.pIf_Ctrl(req->bRequest, CmdBuff, req->wLength);

                /* Send the data to the host */
                USBD_CtlSendData(pdev, CmdBuff, req->wLength, USB_IN_BUFFER_WAIT);
            }
            else /* Host-to-Device requeset */
            {
                log_usb_debuginfo("set uart cfg\r\n");
                /* Set the value of the current command to be processed */
                cdcCmd = req->bRequest;
                cdcLen = req->wLength;

                /* Prepare the reception of the buffer over EP0
                Next step: the received data will be managed in usbd_cdc_EP0_TxSent()
                function. */
                USBD_CtlPrepareRx(pdev, CmdBuff, req->wLength);
            }
        }
        else /* No Data request */
        {
            /* Transfer the command to the interface layer */
            APP_FOPS.pIf_Ctrl(req->bRequest, NULL, 0);
        }

        return USBD_OK;

    default:
        USBD_CtlError(pdev, req);
        return USBD_FAIL;

        /* Standard Requests -------------------------------*/
    case USB_REQ_TYPE_STANDARD:
        switch (req->bRequest)
        {
        case USB_REQ_GET_DESCRIPTOR:
            if ((req->wValue >> 8) == CDC_DESCRIPTOR_TYPE)
            {
                pbuf = (uint8_t*)usbd_cdc_CfgDesc + 9 + (9 * USBD_ITF_MAX_NUM);
                len  = MIN(USB_CDC_DESC_SIZ, req->wLength);
            }

            USBD_CtlSendData(pdev, pbuf, len, USB_IN_BUFFER_WAIT);
            break;

        case USB_REQ_GET_INTERFACE:
            USBD_CtlSendData(pdev, (uint8_t*)&usbd_cdc_AltSet, 1, USB_IN_BUFFER_WAIT);
            break;

        case USB_REQ_SET_INTERFACE:
            if ((uint8_t)(req->wValue) < USBD_ITF_MAX_NUM)
            {
                usbd_cdc_AltSet = (uint8_t)(req->wValue);
            }
            else
            {
                /* Call the error management function (command will be nacked */
                USBD_CtlError(pdev, req);
            }
            break;
        }
    }
    return USBD_OK;
}

/**
 * @brief  usbd_cdc_EP0_RxReady
 *         Data received on control endpoint
 * @param  pdev: device device instance
 * @retval status
 */
uint8_t usbd_cdc_EP0_RxReady(void* pdev)
{
    log_usb_debuginfo("usbd_cdc_EP0_RxReady.\r\n");

    if (cdcCmd != NO_CMD)
    {
        log_usb_debuginfo("have cmd[%x].\r\n", cdcCmd);

        /* Process the data */
        APP_FOPS.pIf_Ctrl(cdcCmd, CmdBuff, cdcLen);

        /* Reset the command variable to default value */
        cdcCmd = NO_CMD;
    }

    return USBD_OK;
}

/**
 * @brief  usbd_audio_DataIn
 *         Data sent on non-control IN endpoint
 * @param  pdev: device instance
 * @param  epnum: endpoint number
 * @retval status
 */
uint8_t usbd_cdc_DataIn(void* pdev, uint8_t epnum)
{
    uint16_t USB_Tx_ptr;
    uint16_t USB_Tx_length;
    // log_usb_debuginfo("usbd_cdc_DataIn.\r\n");

    if (USB_Tx_State == 1)
    {
        if (APP_Rx_length == 0)
        {
            if (last_packet == 1)
            {
                last_packet = 0;

                /*Send zero-length packet*/
                log_usb_debuginfo("send len 0.\r\n");
                DCD_EP_Tx(pdev, CDC_IN_EP, 0, 0, USB_IN_BUFFER_WAIT);
            }
            else
            {
                // log_usb_debuginfo("txs=0.\r\n");
                USB_Tx_State = 0;
            }
        }
        else
        {
            if (APP_Rx_length > CDC_DATA_IN_PACKET_SIZE)
            {
                USB_Tx_ptr    = APP_Rx_ptr_out;
                USB_Tx_length = CDC_DATA_IN_PACKET_SIZE;

                APP_Rx_ptr_out += CDC_DATA_IN_PACKET_SIZE;
                APP_Rx_length -= CDC_DATA_IN_PACKET_SIZE;
            }
            else
            {
                USB_Tx_ptr    = APP_Rx_ptr_out;
                USB_Tx_length = APP_Rx_length;

                APP_Rx_ptr_out += APP_Rx_length;
                APP_Rx_length = 0;
                if (APP_Rx_length == CDC_DATA_IN_PACKET_SIZE)
                    last_packet = 1;
            }

            DCD_EP_Tx(pdev, CDC_IN_EP, (uint8_t*)&APP_Rx_Buffer[USB_Tx_ptr], USB_Tx_length, USB_IN_BUFFER_WAIT);
        }
    }

    return USBD_OK;
}

/**
 * @brief  usbd_cdc_DataOut
 *         Data received on non-control Out endpoint
 * @param  pdev: device instance
 * @param  epnum: endpoint number
 * @retval status
 */
uint8_t usbd_cdc_DataOut(void* pdev, uint8_t epnum)
{
    uint16_t USB_Rx_Cnt;
    // log_usb_debuginfo("usbd_cdc_DataOut.\r\n");
    /* Get the received data buffer and update the counter */
    USB_Rx_Cnt = ((USB_CORE_HANDLE*)pdev)->dev.out_ep[epnum].xfer_count;

    /* USB data will be immediately processed, this allow next USB traffic being
       NAKed till the end of the application Xfer */
    APP_FOPS.pIf_DataRx(USB_Rx_Buffer, USB_Rx_Cnt);

    /* Prepare Out endpoint to receive next packet */
    DCD_EP_PrepareRx(pdev, CDC_OUT_EP, (uint8_t*)(USB_Rx_Buffer), CDC_DATA_OUT_PACKET_SIZE);

    return USBD_OK;
}

/**
 * @brief  usbd_CDC_SOF
 *         Start Of Frame event management
 * @param  pdev: instance
 * @retval status
 */
uint8_t usbd_cdc_SOF(void* pdev)
{
    static uint32_t FrameCount  = 0;
    static uint32_t StringCount = 0;
    static uint32_t TmpFlag     = 0;
    static uint32_t SofCount    = 0;

    // log_usb_debuginfo("usbd_cdc_SOF.\r\n");
    if (FrameCount++ == CDC_IN_FRAME_INTERVAL)
    {
        // log_usb_debuginfo("FrameCount++, = %x.\r\n", FrameCount);
        /* Reset the frame counter */
        FrameCount = 0;

        if ((SofCount++ % 300) == 0)
        {
            log_usb_data("usb sof=%d times.\r\n", SofCount);
        }

        /* Check the data to be sent through IN pipe */
        Handle_USBAsynchXfer(pdev);
    }

    if (!TmpFlag)
    {
        sprintf((char*)chBufData, (const char*)pstr, StringCount);
        StringCount++;
        TmpFlag = 1;
    }

    /* Convert & Add the String to TX Buffer*/
	if (APP_FOPS.pIf_DataTx) 
	{
		APP_FOPS.pIf_DataTx(chBufData[chBufCnt]);
	}

    if (++chBufCnt >= strlen((const char*)chBufData))
    {
        chBufCnt = 0;
        TmpFlag  = 0;
    }

    return USBD_OK;
}

/**
 * @brief  Handle_USBAsynchXfer
 *         Send data to USB
 * @param  pdev: instance
 * @retval None
 */
static void Handle_USBAsynchXfer(void* pdev)
{
    uint16_t USB_Tx_ptr;
    uint16_t USB_Tx_length;
    uint32_t i;

    // log_usb_debuginfo("tx=%d.\r\n", USB_Tx_State);
    if (USB_Tx_State != 1)
    {
        // log_usb_debuginfo("tx!=1.\r\n");
        if (APP_Rx_ptr_out == APP_RX_DATA_SIZE)
        {
            APP_Rx_ptr_out = 0;
        }

        if (APP_Rx_ptr_out == APP_Rx_ptr_in)
        {
            // log_usb_debuginfo("rt2.\r\n");
            USB_Tx_State = 0;
            return;
        }

        if (APP_Rx_ptr_out > APP_Rx_ptr_in) /* rollback */
        {
            APP_Rx_length = APP_RX_DATA_SIZE - APP_Rx_ptr_out;
        }
        else
        {
            APP_Rx_length = APP_Rx_ptr_in - APP_Rx_ptr_out;
        }

        if (APP_Rx_length > CDC_DATA_IN_PACKET_SIZE)
        {
            USB_Tx_ptr    = APP_Rx_ptr_out;
            USB_Tx_length = CDC_DATA_IN_PACKET_SIZE;

            APP_Rx_ptr_out += CDC_DATA_IN_PACKET_SIZE;
            APP_Rx_length -= CDC_DATA_IN_PACKET_SIZE;
        }
        else
        {
            USB_Tx_ptr    = APP_Rx_ptr_out;
            USB_Tx_length = APP_Rx_length;

            APP_Rx_ptr_out += APP_Rx_length;
            APP_Rx_length = 0;
            if (USB_Tx_length == CDC_DATA_IN_PACKET_SIZE)
                last_packet = 1; // IBA
            if (APP_Rx_ptr_in == 64)
                APP_Rx_ptr_in = 0;
        }
        USB_Tx_State = 1;
        // log_usb_debuginfo("txs=1, txlen = %x.\r\n", USB_Tx_length);

        for (i = 0; i < USB_Tx_length; i++)
        {
            // log_usb_debuginfo("%02x, ", APP_Rx_Buffer[USB_Tx_ptr + i]);
        }
        // log_usb_debuginfo("\r\nend1.\r\n", USB_Tx_length);

        DCD_EP_Tx(pdev, CDC_IN_EP, (uint8_t*)&APP_Rx_Buffer[USB_Tx_ptr], USB_Tx_length, USB_IN_BUFFER_WAIT);
    }
}

/**
 * @brief  USBD_cdc_GetCfgDesc
 *         Return configuration descriptor
 * @param  speed : current device speed
 * @param  length : pointer data length
 * @retval pointer to descriptor buffer
 */
static uint8_t* USBD_cdc_GetCfgDesc(uint8_t speed, uint16_t* length)
{
    log_usb_debuginfo("USBD_cdc_GetCfgDesc.\r\n");
    *length = sizeof(usbd_cdc_CfgDesc);
    log_usb_debuginfo("cfg *length = %x.\r\n", *length);

    return (uint8_t*)usbd_cdc_CfgDesc;
}

/** @} */
