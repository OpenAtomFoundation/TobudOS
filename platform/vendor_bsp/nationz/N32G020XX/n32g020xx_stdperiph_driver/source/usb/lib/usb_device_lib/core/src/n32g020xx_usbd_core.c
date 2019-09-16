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
 * @file n32g020xx_usbd_core.c
 * @author Nations Solution Team
 * @version v1.0.1
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @date    2019-01-19
 * @brief   文件提供USBD  (USB device)内核功能函数，包括处理所有USB
 *通信和状态机的函数。
 *
 * @addtogroup USB
 * @{
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "n32g020xx_usbd_core.h"
#include "n32g020xx_usbd_req.h"
#include "n32g020xx_usbd_ioreq.h"
#include "n32g020xx_usb_log.h"
#include "n32g020xx_log.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint32_t ADDRESS;

/* Private function prototypes -----------------------------------------------*/
static uint8_t USBD_SetupStage(USB_CORE_HANDLE* pdev);
static uint8_t USBD_DataOutStage(USB_CORE_HANDLE* pdev, uint8_t epnum);
static uint8_t USBD_DataInStage(USB_CORE_HANDLE* pdev, uint8_t epnum, uint8_t tx_type);
static uint8_t USBD_SOF(USB_CORE_HANDLE* pdev);
static uint8_t USBD_Reset(USB_CORE_HANDLE* pdev);
static uint8_t USBD_Suspend(USB_CORE_HANDLE* pdev);
static uint8_t USBD_Resume(USB_CORE_HANDLE* pdev);

USBD_DCD_INT_cb_TypeDef USBD_DCD_INT_cb = {
    USBD_DataOutStage,
    USBD_DataInStage,
    USBD_SetupStage,
    USBD_SOF,
    USBD_Reset,
    USBD_Suspend,
    USBD_Resume,
};

USBD_DCD_INT_cb_TypeDef* USBD_DCD_INT_fops = &USBD_DCD_INT_cb;

/* Private functions ---------------------------------------------------------*/
/**
 * @brief  USBD_Init
 *         Initializes the device stack and load the class driver
 * @param  pdev: device instance
 * @param pDevice device
 * @param  class_cb: Class callback structure address
 * @param  usr_cb: User callback structure address
 * @retval None
 */
void USBD_Init(USB_CORE_HANDLE* pdev,
               USBD_DEVICE* pDevice,
               USBD_Class_cb_TypeDef* class_cb,
               USBD_Usr_cb_TypeDef* usr_cb)
{
    USBD_DeInit(pdev);

    /*Register class and user callbacks */
    pdev->dev.class_cb   = class_cb;
    pdev->dev.usr_cb     = usr_cb;
    pdev->dev.usr_device = pDevice;

    /* set USB DEVICE core params */
    DCD_Init(pdev);

    /* Upon Init call usr callback */
    pdev->dev.usr_cb->Init();

    /* Enable Interrupts */
	NVIC_EnableIRQ(Usb_Exception_IrqNum);

    DCD_DevConnect(pdev);
}

/**
 * @brief  USBD_DeInit
 *         Re-Initialize th device library
 * @param  pdev: device instance
 * @retval status: status
 */
USBD_Status USBD_DeInit(USB_CORE_HANDLE* pdev)
{
    /* Software Init */

    return USBD_OK;
}

/**
 * @brief  USBD_SetupStage
 *         Handle the setup stage
 * @param  pdev: device instance
 * @retval status
 */
static uint8_t USBD_SetupStage(USB_CORE_HANDLE* pdev)
{
    USB_SETUP_REQ req;

    log_usb_debuginfo("entry USBD_SetupStage.\r\n");

    USBD_ParseSetupRequest(pdev, &req);

    switch (req.bmRequest & 0x1F)
    {
    case USB_REQ_RECIPIENT_DEVICE:
        log_usb_debuginfo("#req device.\r\n");
        USBD_StdDevReq(pdev, &req);
        break;

    case USB_REQ_RECIPIENT_INTERFACE:
        log_usb_debuginfo("#req interface.\r\n");
        USBD_StdItfReq(pdev, &req);
        break;

    case USB_REQ_RECIPIENT_ENDPOINT:
        log_usb_debuginfo("#req endpoint.\r\n");
        USBD_StdEPReq(pdev, &req);
        break;

    default:
        log_usb_debuginfo("#default:ep stall.\r\n");
        DCD_EP_Stall(pdev, req.bmRequest & 0x80);
        break;
    }
    return USBD_OK;
}

/**
 * @brief  USBD_DataOutStage
 *         Handle data out stage
 * @param  pdev: device instance
 * @param  epnum: endpoint index
 * @retval status
 */
static uint8_t USBD_DataOutStage(USB_CORE_HANDLE* pdev, uint8_t epnum)
{
    USB_EP* ep;

    log_usb_debuginfo("USBD_DataOutStage.\r\n");

    if (epnum == 0)
    {
        ep = &pdev->dev.out_ep[0];
        if (pdev->dev.device_state == USB_EP0_DATA_OUT)
        {
            if (ep->rem_data_len > ep->maxpacket)
            {
                log_usb_debuginfo("rem_data_len > max.\r\n");
                ep->rem_data_len -= ep->maxpacket;

                USBD_CtlContinueRx(pdev, ep->xfer_buff, MIN(ep->rem_data_len, ep->maxpacket));
            }
            else
            {
                log_usb_debuginfo("rem_data_len <= max.\r\n");
                if ((pdev->dev.class_cb->EP0_RxReady != NULL) && (pdev->dev.device_status == USB_CONFIGURED))
                {
                    log_usb_debuginfo("rx ready.\r\n");
                    pdev->dev.class_cb->EP0_RxReady(pdev);
                }
                USBD_CtlSendStatus(pdev);
            }
        }
    }
    else if ((pdev->dev.class_cb->DataOut != NULL) && (pdev->dev.device_status == USB_CONFIGURED))
    {
        log_usb_debuginfo("cb->DataOut.\r\n");
        pdev->dev.class_cb->DataOut(pdev, epnum);
    }
    return USBD_OK;
}

/**
 * @brief  USBD_DataInStage
 *         Handle data in stage
 * @param  pdev: device instance
 * @param  epnum: endpoint index
 * @param  ep_tx_type: endpoint tx type
 * @retval status
 */
static uint8_t USBD_DataInStage(USB_CORE_HANDLE* pdev, uint8_t epnum, uint8_t tx_type)
{
    USB_EP* ep;

    if (epnum == 0)
    {
        ep = &pdev->dev.in_ep[0];
        if (pdev->dev.device_state == USB_EP0_DATA_IN)
        {
            if (ep->rem_data_len > ep->maxpacket)
            {
                ep->rem_data_len -= ep->maxpacket;
                USBD_CtlContinueSendData(pdev, ep->xfer_buff, ep->rem_data_len, tx_type);
            }
            else
            { /* last packet is MPS multiple, so send ZLP packet */
                if ((ep->total_data_len % ep->maxpacket == 0) && (ep->total_data_len >= ep->maxpacket)
                    && (ep->total_data_len < ep->ctl_data_len))
                {
                    USBD_CtlContinueSendData(pdev, NULL, 0, tx_type);
                    ep->ctl_data_len = 0;
                }
                else
                {
                    if ((pdev->dev.class_cb->EP0_TxSent != NULL) && (pdev->dev.device_status == USB_CONFIGURED))
                    {
                        pdev->dev.class_cb->EP0_TxSent(pdev);
                    }
                    USBD_CtlReceiveStatus(pdev);
                }
            }
        }
        else if ((pdev->dev.device_state == USB_EP0_STATUS_IN) && (ADDRESS != 0))
        {
            DCD_EP_SetAddress(pdev, ADDRESS);
            ADDRESS = 0;
        }
    }
    else if ((pdev->dev.class_cb->DataIn != NULL) && (pdev->dev.device_status == USB_CONFIGURED))
    {
        log_usb_debuginfo("USBD_DataInStage, ep[%d], tx data .\r\n", epnum);
        pdev->dev.class_cb->DataIn(pdev, epnum); // usbd_cdc_DataIn
    }
    return USBD_OK;
}

/**
 * @brief  USBD_Reset
 *         Handle Reset event
 * @param  pdev: device instance
 * @retval status
 */

static uint8_t USBD_Reset(USB_CORE_HANDLE* pdev)
{
    /* Open EP0 OUT */
    DCD_EP_Open(pdev, 0x00, USB_MAX_EP0_SIZE, EP_TYPE_CTRL);

    /* Open EP0 IN */
    DCD_EP_Open(pdev, 0x80, USB_MAX_EP0_SIZE, EP_TYPE_CTRL);

    /* Upon Reset call user call back */
    pdev->dev.device_status = USB_DEFAULT;
    pdev->dev.usr_cb->DeviceReset(pdev->dev.speed);

    return USBD_OK;
}

/**
 * @brief  USBD_Resume
 *         Handle Resume event
 * @param  pdev: device instance
 * @retval status
 */

static uint8_t USBD_Resume(USB_CORE_HANDLE* pdev)
{
    /* Upon Resume call user call back */
    pdev->dev.usr_cb->DeviceResumed();
    pdev->dev.device_status = pdev->dev.device_old_status;
    return USBD_OK;
}

/**
 * @brief  USBD_Suspend
 *         Handle Suspend event
 * @param  pdev: device instance
 * @retval status
 */

static uint8_t USBD_Suspend(USB_CORE_HANDLE* pdev)
{
    pdev->dev.device_old_status = pdev->dev.device_status;
    /*Device is in Suspended State*/
    pdev->dev.device_status = USB_SUSPENDED;
    /* Upon Resume call user call back */
    pdev->dev.usr_cb->DeviceSuspended();
    return USBD_OK;
}

/**
 * @brief  USBD_SOF
 *         Handle SOF event
 * @param  pdev: device instance
 * @retval status
 */

static uint8_t USBD_SOF(USB_CORE_HANDLE* pdev)
{
    if (pdev->dev.class_cb->StartFrame)
    {
        pdev->dev.class_cb->StartFrame(pdev);
    }
    return USBD_OK;
}
/**
 * @brief  USBD_SetCfg
 *        Configure device and start the interface
 * @param  pdev: device instance
 * @param  cfgidx: configuration index
 * @retval status
 */

USBD_Status USBD_SetCfg(USB_CORE_HANDLE* pdev, uint8_t cfgidx)
{
    pdev->dev.class_cb->Init(pdev, cfgidx); // usbd_cdc_Init

    /* Upon set config call user call back */
    pdev->dev.usr_cb->DeviceConfigured();
    return USBD_OK;
}

/**
 * @brief  USBD_ClrCfg
 *         Clear current configuration
 * @param  pdev: device instance
 * @param  cfgidx: configuration index
 * @retval status: USBD_Status
 */
USBD_Status USBD_ClrCfg(USB_CORE_HANDLE* pdev, uint8_t cfgidx)
{
    pdev->dev.class_cb->DeInit(pdev, cfgidx);
    return USBD_OK;
}



/** @} */
