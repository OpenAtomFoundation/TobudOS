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
 * @file n32g020xx_usbd_ioreq.c
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @date    2019-01-19
 * @brief   文件为控制端点提供IO请求接口API
 *
 * @addtogroup USB
 * @{
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "n32g020xx_usbd_ioreq.h"
#include "n32g020xx_usb_log.h"
#include "n32g020xx_log.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  USBD_CtlSendData
 *         send data on the ctl pipe
 * @param  pdev device instance
 * @param  pbuf pointer to data buffer
 * @param  len length of data to be sent
 * @param tx_type
 * @retval status
 */
USBD_Status USBD_CtlSendData(USB_CORE_HANDLE* pdev, uint8_t* pbuf, uint16_t len, uint8_t tx_type)
{
    USBD_Status ret = USBD_OK;
    log_usb_debuginfo("USBD_CtlSendData, buf = %x, len = %x, tx_type = %d.\r\n", pbuf, len, tx_type);

    pdev->dev.in_ep[0].total_data_len = len;
    pdev->dev.in_ep[0].rem_data_len   = len;
    // pdev->dev.in_ep[0].xfer_len = len;
    pdev->dev.device_state = USB_EP0_DATA_IN;

    DCD_EP_Tx(pdev, 0, pbuf, len, tx_type);

    return ret;
}

/**
 * @brief  USBD_CtlContinueSendData
 *         continue sending data on the ctl pipe
 * @param  pdev device instance
 * @param  pbuf pointer to data buffer
 * @param  len length of data to be sent
 * @param  ep_tx_type
 * @retval status
 */
USBD_Status USBD_CtlContinueSendData(USB_CORE_HANDLE* pdev, uint8_t* pbuf, uint16_t len, uint8_t ep_tx_type)
{
    USBD_Status ret = USBD_OK;

    DCD_EP_Tx(pdev, 0, pbuf, len, USB_IN_BUFFER_WAIT);

    return ret;
}

/**
 * @brief  USBD_CtlPrepareRx
 *         receive data on the ctl pipe
 * @param  pdev USB device instance
 * @param  pbuf pointer to data buffer
 * @param  len length of data to be received
 * @retval status
 */
USBD_Status USBD_CtlPrepareRx(USB_CORE_HANDLE* pdev, uint8_t* pbuf, uint16_t len)
{
    USBD_Status ret = USBD_OK;

    pdev->dev.out_ep[0].total_data_len = len;
    pdev->dev.out_ep[0].rem_data_len   = len;
    pdev->dev.device_state             = USB_EP0_DATA_OUT;

    DCD_EP_PrepareRx(pdev, 0, pbuf, len);

    return ret;
}

/**
 * @brief  USBD_CtlContinueRx
 *         continue receive data on the ctl pipe
 * @param  pdev USB device instance
 * @param  pbuf pointer to data buffer
 * @param  len length of data to be received
 * @retval status
 */
USBD_Status USBD_CtlContinueRx(USB_CORE_HANDLE* pdev, uint8_t* pbuf, uint16_t len)
{
    USBD_Status ret = USBD_OK;

    DCD_EP_PrepareRx(pdev, 0, pbuf, len);
    return ret;
}
/**
 * @brief  USBD_CtlSendStatus
 *         send zero length packet on the ctl pipe
 * @param  pdev USB device instance
 * @retval status
 */
USBD_Status USBD_CtlSendStatus(USB_CORE_HANDLE* pdev)
{
    USBD_Status ret        = USBD_OK;
    pdev->dev.device_state = USB_EP0_STATUS_IN;
    DCD_EP_Tx(pdev, 0, NULL, 0, USB_IN_BUFFER_WAIT);
    return ret;
}

/**
 * @brief  USBD_CtlReceiveStatus
 *         receive zero length packet on the ctl pipe
 * @param  pdev USB device instance
 * @retval status
 */
USBD_Status USBD_CtlReceiveStatus(USB_CORE_HANDLE* pdev)
{
    USBD_Status ret        = USBD_OK;
    pdev->dev.device_state = USB_EP0_STATUS_OUT;
    DCD_EP_PrepareRx(pdev, 0, NULL, 0);

    return ret;
}

/**
 * @brief  USBD_GetRxCount
 *         returns the received data length
 * @param  pdev USB device instance
 * @param  epnum endpoint index
 * @retval Rx Data blength
 */
uint16_t USBD_GetRxCount(USB_CORE_HANDLE* pdev, uint8_t epnum)
{
    return pdev->dev.out_ep[epnum].xfer_count;
}



/** @} */
