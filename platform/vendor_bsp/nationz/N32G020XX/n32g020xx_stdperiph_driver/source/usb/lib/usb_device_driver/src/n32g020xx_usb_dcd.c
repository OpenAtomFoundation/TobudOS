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
 * @file n32g020xx_usb_dcd.c
 * @author Nations Solution Team
 * @version v1.0.1
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @date    2019-01-19
 * @brief   文件提供设备接口层，用于访问USB 内核。
 * 
 * @addtogroup USB
 * @{
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "n32g020xx_usb_dcd.h"
#include "n32g020xx_common.h"
#include "n32g020xx_usb_log.h"
#include "n32g020xx_log.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t wInterrupt_Mask = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief Device Initialization
 * @param  pdev: device instance
 * @retval : None
 */
void DCD_Init(USB_CORE_HANDLE* pdev)
{
    /*Device is in Default State*/
    pdev->dev.device_status   = USB_DEFAULT;
    pdev->dev.device_address  = 0;
    pdev->dev.DevRemoteWakeup = 0;

    pdev->dev.speed = USB_SPEED_FULL; /*kept for API compatibility reason*/
}

/**
 * @brief Stop device
 * @param  pdev: device instance
 * @retval : None
 */
void DCD_StopDevice(USB_CORE_HANDLE* pdev)
{
    /*Device is in default state*/
    pdev->dev.device_status = USB_DEFAULT;
}

/**
 * @brief Configure an EP
 * @param  pdev : Device instance
 * @param  ep_addr: endpoint address
 * @param  ep_mps: endpoint max packet size
 * @param  ep_type: endpoint Type
 */
uint32_t DCD_EP_Open(USB_CORE_HANDLE* pdev, uint16_t ep_addr, uint16_t ep_mps, uint8_t ep_type)
{
    USB_EP* ep;
    log_usb_debuginfo("DCD_EP_Open, %x, %x, %x\r\n", ep_addr, ep_mps, ep_type);

    /* initialize ep structure*/
    if ((ep_addr & 0x80) == 0x80)
    {
        ep        = &pdev->dev.in_ep[ep_addr & 0x7F];
        ep->is_in = 1;

		SetEPTxStatus(ep_addr & 0x7F, EP_ENABLE_EP_TOKEN_IRQ);
    }
    else
    {
        ep        = &pdev->dev.out_ep[ep_addr & 0x7F];
        ep->is_in = 0;

		SetEPRxStatus(ep_addr & 0x7F, EP_ENABLE_EP_TOKEN_IRQ);
    }

    ep->maxpacket = ep_mps;
    ep->type      = ep_type;
    ep->num       = ep_addr & 0x7F;

    if (ep->num == 0)
    {
        /* Initialize the control transfer variables*/
        ep->ctl_data_len   = 0;
        ep->rem_data_len   = 0;
        ep->total_data_len = 0;
    }

    /* Initialize the transaction level variables */
    ep->xfer_buff  = 0;
    ep->xfer_len   = 0;
    ep->xfer_count = 0;
    ep->is_stall   = 0;

    /* initialize HW */
    switch (ep->type)
    {
    case USB_EP_CONTROL:
        /* nothing to do */
        break;
    case USB_EP_BULK:
        /* nothing to do */
        break;
    case USB_EP_INT:
        /* nothing to do */
        break;
    case USB_EP_ISOC:
        /* nothing to do */
        break;
    }

    if (ep->doublebuffer == 0)
    {
        if (ep->is_in)
        {
            log_usb_debuginfo("ep in[%x] ,in endpoint.\r\n", ep->num);
            /* nothing to do */
        }
        else
        {
            log_usb_debuginfo("ep out[%x] ,out endpoint.\r\n", ep->num);
            /* nothing to do */
        }
    }
    /*Double Buffer*/
    else
    {
        if (ep->is_in == 0)
        {
            /* nothing to do */
        }
        else
        {
            /* nothing to do */
        }
    }
    return USB_OK;
}
/**
 * @brief called when an EP is disabled
 * @param  pdev: device instance
 * @param  ep_addr: endpoint address
 * @retval : status
 */
uint32_t DCD_EP_Close(USB_CORE_HANDLE* pdev, uint8_t ep_addr)
{
    USB_EP* ep;

    if ((ep_addr & 0x80) == 0x80)
    {
        ep = &pdev->dev.in_ep[ep_addr & 0x7F];
    }
    else
    {
        ep = &pdev->dev.out_ep[ep_addr & 0x7F];
    }

    if (ep->doublebuffer == 0)
    {
        if (ep->is_in)
        {
            /* Configure DISABLE status for the Endpoint*/
            SetEPTxStatus(ep->num, EP_CSP_HALT);
        }
        else
        {
            /* Configure DISABLE status for the Endpoint*/
            SetEPRxStatus(ep->num, EP_CSP_HALT);
        }
    }
    /*Double Buffer*/
    else
    {
        if (ep->is_in == 0)
        {
            /* Clear the data toggle bits for the endpoint IN/OUT*/
            SetEPRxStatus(ep->num, EP_CSP_HALT);
            SetEPTxStatus(ep->num, EP_CSP_HALT);
        }
        else
        {
            /* Clear the data toggle bits for the endpoint IN/OUT*/
            /* Configure DISABLE status for the Endpoint*/
            SetEPTxStatus(ep->num, EP_CSP_HALT);
            SetEPRxStatus(ep->num, EP_CSP_HALT);
        }
    }
    return USB_OK;
}

/**
 * @brief DCD_EP_PrepareRx
 * @param  pdev: device instance
 * @param  ep_addr: endpoint address
 * @param  pbuf: pointer to Rx buffer
 * @param  buf_len: data length
 * @retval : status
 */
uint32_t DCD_EP_PrepareRx(USB_CORE_HANDLE* pdev, uint8_t ep_addr, uint8_t* pbuf, uint16_t buf_len)
{
    __IO uint32_t len = 0;
    USB_EP* ep;

    log_usb_debuginfo("DCD_EP_PrepareRx.\r\n");
    ep = &pdev->dev.out_ep[ep_addr & 0x7F];

    /*setup and start the Xfer */
    ep->xfer_buff  = pbuf;
    ep->xfer_len   = buf_len;
    ep->xfer_count = 0;

    /*Multi packet transfer*/
    if (ep->xfer_len > ep->maxpacket)
    {
        len = ep->maxpacket;
        ep->xfer_len -= len;
    }
    else
    {
        len          = ep->xfer_len;
        ep->xfer_len = 0;
    }

    /* configure and validate Rx endpoint */
    if (ep->doublebuffer == 0)
    {
        /*Set RX buffer count*/
        // SetEPRxCount(ep->num, len);
    }
    else
    {
        /*Set the Double buffer counter*/
        // SetEPDblBuffCount(ep->num, ep->is_in, len);
    }

    // SetEPRxStatus(ep->num, EP_RX_VALID);

#if 0
    SetEPRxStatus(ep->num, EP_CSP_INOUT_FIFO_AUTO0);
    SetEPRxStatus(ep->num, EP_RX_FIFO_READY);
#endif

    return USB_OK;
}

/**
 * @brief Transmit data Buffer
 * @param  pdev: device instance
 * @param  ep_addr: endpoint address
 * @param  pbuf: pointer to Tx buffer
 * @param  buf_len: data length
 * @param tx_type tx type
 * @retval : status
 */
uint32_t DCD_EP_Tx(USB_CORE_HANDLE* pdev, uint8_t ep_addr, uint8_t* pbuf, uint32_t buf_len, uint8_t tx_type)
{
    __IO uint32_t len = 0, i, finished_flag = 0, temp;
    USB_EP* ep;

    // log_usb_debuginfo("DCD_EP_Tx,entry.\r\n") ;

    ep = &pdev->dev.in_ep[ep_addr & 0x7F];

     log_usb_debuginfo("EP_TX-%d:[0x%x,0x%x,0x%x,0x%x]\r\n", \
 						ep->num, ep->xfer_buff, ep->xfer_len, ep->maxpacket, tx_type);

    if (tx_type == USB_IN_BUFFER_WAIT)
    {
        /*setup and start the Xfer */
        ep->num        = ep_addr & 0x7F;
        ep->xfer_buff  = pbuf;
        ep->xfer_len   = buf_len;
        ep->xfer_count = 0;
        log_usb_debuginfo("buffer & return and later tx, addr = %x, len = %x.\r\n", pbuf, buf_len) ;
    }
    else
    {
        /*Multi packet transfer*/
        if (ep->xfer_len > ep->maxpacket)
        {
             log_usb_debuginfo(">.\r\n");
            len = ep->maxpacket;
            ep->xfer_len -= len;
        }
        else
        {
             log_usb_debuginfo("<.\r\n");
            len          = ep->xfer_len;
            ep->xfer_len = 0;
        }

        /*Write the data to the USB endpoint*/
        if (ep->num < 5)
        {
            temp = (*(volatile unsigned long*)(ep->epcrs_phy_addr));
            // log_usb_debuginfo("1-ep[%d], temp=%x, rdyvalue = 0x%x, crs addr = %x.\r\n", ep->num, temp, ep->epx_in_rdy_value,
            // ep->epcrs_phy_addr);
            if (temp & ep->epx_in_rdy_value)
            {
                // nothing to do
                // log_usb_debuginfo("Ep[%d]in ready\r\n", ep->num);
            }
            else
            {
                log_usb_debuginfo("Ep[%d] in no ready, return\r\n", ep->num);
                return USB_FAIL;
            }

            // log_usb_debuginfo("epxcrs addr[%x]\r\n", ep->epxcrs_phy_addr);
            if (ep->epxcrs_phy_addr)
            {
                (*(volatile unsigned long*)(ep->epxcrs_phy_addr)) |= EPCLR; /* ep0 fifo pointer to zero */
            }
            else
            {
                log_usb_debuginfo("error:ep[%d] epxcrs_phy_addr is 0, return\r\n", ep->num);
                return USB_FAIL;
            }

            log_usb_debuginfo("----[len =%x], addr = %x:\r\n", len, ep->epxfifo_in_phy_addr);
            for (i = 0; i < len; i++)
            {
                if (ep->epxfifo_in_phy_addr)
                {
                    (*(volatile unsigned long*)(ep->epxfifo_in_phy_addr)) = *ep->xfer_buff;
                }
                else
                {
                    log_usb_error("error:ep[%d] epxfifo_in_phy_addr is 0\r\n", ep->num);
                }
                ep->xfer_buff++;
            }
             log_usb_debuginfo("ep[%d] wr[%d] bytes\r\n", ep->num, len);

            if ((len == 0) && ((ep->num == 1) || (ep->num == 3)))
            {
                // log_usb_debuginfo("don't create irq\r\n", ep->num, len);
                 log_usb_debuginfo("ep[1-3]:addr = %x, value = %x,\r\n", ep->epcrs_phy_addr, ep->epx_in_rdy_value);
                (*(volatile unsigned long*)(ep->epcrs_phy_addr)) = ep->epx_in_rdy_value;
            }
            else
            {
                 log_usb_debuginfo("ep[other]:addr = %x, value = %x,\r\n", ep->epcrs_phy_addr, ep->epx_in_rdy_value);
                (*(volatile unsigned long*)(ep->epcrs_phy_addr)) = ep->epx_in_rdy_value;
            }
        }
        else
        {
            temp = (*(volatile unsigned long*)(ep->epcrs2_phy_addr));
            log_usb_debuginfo("#ep[%d]-crs = 0x%d, rdy = 0x%x, crs addr = %x.\r\n",
                ep->num,
                temp,
                ep->epx_in_rdy_value,
                ep->epcrs2_phy_addr);
            if (temp & ep->epx_in_rdy_value)
            {
                // nothing to do
            }
            else
            {
                log_usb_debuginfo("#Ep[%d] in no ready, return\r\n", ep->num);
                return USB_FAIL;
            }

            log_usb_debuginfo("#epxcrs addr[%x]\r\n", ep->epxcrs_phy_addr);
            if (ep->epxcrs_phy_addr)
            {
                (*(volatile unsigned long*)(ep->epxcrs_phy_addr)) |= EPCLR; /* ep0 fifo pointer to zero */
            }
            else
            {
                log_usb_debuginfo("#error:ep[%d] epxcrs_phy_addr is 0, return\r\n", ep->num);
                return USB_FAIL;
            }

            log_usb_debuginfo("----[len =%x], addr = %x:\r\n", len, ep->epxfifo_in_phy_addr);
            for (i = 0; i < len; i++)
            {
                // USB_EP0IFIFO = *ep->xfer_buff;
                if (ep->epxfifo_in_phy_addr)
                {
                    (*(volatile unsigned long*)(ep->epxfifo_in_phy_addr)) = *ep->xfer_buff;
                }
                else
                {
                    log_usb_debuginfo("error:ep[%d] epxfifo_in_phy_addr is 0\r\n", ep->num);
                }
                ep->xfer_buff++;
                // log_usb_debuginfo("%02x ", *Ep0DataPtr);
            }
            // log_usb_debuginfo("#ep[%d] write finshied[%d] num bytes\r\n", ep->num, len);

            // USB_EPCSR = EP0INRDY;
            log_usb_debuginfo("#addr = %x, value = %x,\r\n", ep->epcrs2_phy_addr, ep->epx_in_rdy_value);
            (*(volatile unsigned long*)(ep->epcrs2_phy_addr)) = ep->epx_in_rdy_value;
        }
    }

	log_usb_debuginfo("\r\n");
    return USB_OK;
}

/**
 * @brief Stall an endpoint.
 * @param  pdev: device instance
 * @param  epnum: endpoint address
 * @retval : status
 */
uint32_t DCD_EP_Stall(USB_CORE_HANDLE* pdev, uint8_t epnum)
{
    USB_EP* ep;
    if ((0x80 & epnum) == 0x80)
    {
        ep = &pdev->dev.in_ep[epnum & 0x7F];
    }
    else
    {
        ep = &pdev->dev.out_ep[epnum];
    }

    if (ep->num == 0)
    {
        /* This macro sets STALL status for RX & TX*/
        log_usb_debuginfo("ep[%d]tx stall", ep->num);
        SetEPTxStatus(ep->num, EP_CSP_SENDSTALL);

        /*Endpoint is stalled */
        ep->is_stall = 1;
        return USB_OK;
    }
    if (ep->is_in)
    {
        /* IN endpoint */
        ep->is_stall = 1;
        /* IN Endpoint stalled */
    }
    else
    {
        ep->is_stall = 1;
        /* OUT Endpoint stalled */
    }

    return USB_OK;
}

/**
 * @brief Clear stall condition on endpoints.
 * @param  pdev: device instance
 * @param  epnum: endpoint address
 * @retval : status
 */
uint32_t DCD_EP_ClrStall(USB_CORE_HANDLE* pdev, uint8_t epnum)
{
    USB_EP* ep;
    if ((0x80 & epnum) == 0x80)
    {
        ep = &pdev->dev.in_ep[epnum & 0x7F];
    }
    else
    {
        ep = &pdev->dev.out_ep[epnum];
    }

    if (ep->is_in)
    {
        ep->is_stall = 0;
    }
    else
    {
        ep->is_stall = 0;
    }

    return USB_OK;
}

/**
 * @brief This Function set USB device address
 * @param  pdev: device instance
 * @param  address: new device address
 */
void DCD_EP_SetAddress(USB_CORE_HANDLE* pdev, uint8_t address)
{
    pdev->dev.device_address = address;

    /* set address in every used endpoint */
    /*********tx endpoint***********/
    /* endpoint0 */
    pdev->dev.in_ep[0].epcrs_phy_addr       = (USB_BASE + 0x28C);
    pdev->dev.in_ep[0].epcrs2_phy_addr      = 0;
    pdev->dev.in_ep[0].epxcrs_phy_addr      = (USB_BASE + 0x290);
    pdev->dev.in_ep[0].epxfifo_in_phy_addr  = (USB_BASE + 0x2C8);
    pdev->dev.in_ep[0].epxfifo_out_phy_addr = (USB_BASE + 0x2CC);
    pdev->dev.in_ep[0].epx_in_rdy_value     = EP0INRDY;
    pdev->dev.in_ep[0].epx_out_rdy_value    = EP0OUTRDY;
    pdev->dev.in_ep[0].ep_out_bcr           = 0;
    pdev->dev.in_ep[0].num                  = 0;

    /* endpoint1 */
    pdev->dev.in_ep[1].epcrs_phy_addr       = (USB_BASE + 0x28C);
    pdev->dev.in_ep[1].epcrs2_phy_addr      = 0;
    pdev->dev.in_ep[1].epxcrs_phy_addr      = (USB_BASE + 0x298);
    pdev->dev.in_ep[1].epxfifo_in_phy_addr  = (USB_BASE + 0x2D0);
    pdev->dev.in_ep[1].epxfifo_out_phy_addr = 0;
    pdev->dev.in_ep[1].epx_in_rdy_value     = EP1RDY;
    pdev->dev.in_ep[1].epx_out_rdy_value    = 0;
    pdev->dev.in_ep[1].ep_out_bcr           = 0;
    pdev->dev.in_ep[1].num                  = 1;

    /* endpoint3 */
    pdev->dev.in_ep[3].epcrs_phy_addr       = (USB_BASE + 0x28C);
    pdev->dev.in_ep[3].epcrs2_phy_addr      = 0;
    pdev->dev.in_ep[3].epxcrs_phy_addr      = (USB_BASE + 0x2B4);
    pdev->dev.in_ep[3].epxfifo_in_phy_addr  = (USB_BASE + 0x2D8);
    pdev->dev.in_ep[3].epxfifo_out_phy_addr = 0;
    pdev->dev.in_ep[3].epx_in_rdy_value     = EP3RDY;
    pdev->dev.in_ep[3].epx_out_rdy_value    = 0;
    pdev->dev.in_ep[3].ep_out_bcr           = 0;
    pdev->dev.in_ep[3].num                  = 3;

    /* endpoint5 */
    pdev->dev.in_ep[5].epcrs_phy_addr       = (USB_BASE + 0x28C);
    pdev->dev.in_ep[5].epcrs2_phy_addr      = 0;
    pdev->dev.in_ep[5].epxcrs_phy_addr      = (USB_BASE + 0x2AC);
    pdev->dev.in_ep[5].epxfifo_in_phy_addr  = (USB_BASE + 0x33C);
    pdev->dev.in_ep[5].epxfifo_out_phy_addr = 0;
    pdev->dev.in_ep[5].epx_in_rdy_value     = EP5RDY;
    pdev->dev.in_ep[5].epx_out_rdy_value    = 0;
    pdev->dev.in_ep[5].ep_out_bcr           = 0;
    pdev->dev.in_ep[5].num                  = 5;

    /* endpoint6 */
    pdev->dev.in_ep[6].epcrs_phy_addr       = 0;
    pdev->dev.in_ep[6].epcrs2_phy_addr      = (USB_BASE + 0x344);
    pdev->dev.in_ep[6].epxcrs_phy_addr      = (USB_BASE + 0x348);
    pdev->dev.in_ep[6].epxfifo_in_phy_addr  = (USB_BASE + 0x35C);
    pdev->dev.in_ep[6].epxfifo_out_phy_addr = 0;
    pdev->dev.in_ep[6].epx_in_rdy_value     = EP6RDY;
    pdev->dev.in_ep[6].epx_out_rdy_value    = 0;
    pdev->dev.in_ep[6].ep_out_bcr           = 0;
    pdev->dev.in_ep[6].num                  = 6;

    /*********rx endpoint***********/
    /* endpoint0 */
    pdev->dev.out_ep[0].epcrs_phy_addr         = (USB_BASE + 0x28C);
    pdev->dev.out_ep[0].epcrs2_phy_addr        = 0;
    pdev->dev.out_ep[0].epxcrs_phy_addr        = (USB_BASE + 0x290);
    pdev->dev.out_ep[0].epxfifo_setup_phy_addr = (USB_BASE + 0x2C4);
    pdev->dev.out_ep[0].epxfifo_in_phy_addr    = (USB_BASE + 0x2C8);
    pdev->dev.out_ep[0].epxfifo_out_phy_addr   = (USB_BASE + 0x2CC);
    pdev->dev.out_ep[0].epx_in_rdy_value       = EP0INRDY;
    pdev->dev.out_ep[0].epx_out_rdy_value      = EP0OUTRDY;
    pdev->dev.out_ep[0].ep_out_bcr             = (USB_BASE + 0x294);
    pdev->dev.out_ep[0].num                    = 0;

    /* endpoint2 */
    pdev->dev.out_ep[2].epcrs_phy_addr       = (USB_BASE + 0x28C);
    pdev->dev.out_ep[2].epcrs2_phy_addr      = 0;
    pdev->dev.out_ep[2].epxcrs_phy_addr      = (USB_BASE + 0x298);
    pdev->dev.out_ep[2].epxfifo_in_phy_addr  = 0;
    pdev->dev.out_ep[2].epxfifo_out_phy_addr = (USB_BASE + 0x2D4);
    pdev->dev.out_ep[2].epx_in_rdy_value     = 0;
    pdev->dev.out_ep[2].epx_out_rdy_value    = EP2RDY;
    pdev->dev.out_ep[2].ep_out_bcr           = (USB_BASE + 0x2B0);
    pdev->dev.out_ep[2].num                  = 2;

    /* endpoint4 */
    pdev->dev.out_ep[4].epcrs_phy_addr       = (USB_BASE + 0x28C);
    pdev->dev.out_ep[4].epcrs2_phy_addr      = 0;
    pdev->dev.out_ep[4].epxcrs_phy_addr      = (USB_BASE + 0x29C);
    pdev->dev.out_ep[4].epxfifo_in_phy_addr  = 0;
    pdev->dev.out_ep[4].epxfifo_out_phy_addr = (USB_BASE + 0x2DC);
    pdev->dev.out_ep[4].epx_in_rdy_value     = 0;
    pdev->dev.out_ep[4].epx_out_rdy_value    = EP4RDY;
    pdev->dev.out_ep[4].ep_out_bcr           = (USB_BASE + 0x2BC);
    pdev->dev.out_ep[4].num                  = 4;

    /* endpoint7 */
    pdev->dev.out_ep[7].epcrs_phy_addr       = 0;
    pdev->dev.out_ep[7].epcrs2_phy_addr      = (USB_BASE + 0x344);
    pdev->dev.out_ep[7].epxcrs_phy_addr      = (USB_BASE + 0x2AC);
    pdev->dev.out_ep[7].epxfifo_in_phy_addr  = 0;
    pdev->dev.out_ep[7].epxfifo_out_phy_addr = (USB_BASE + 0x360);
    pdev->dev.out_ep[7].epx_in_rdy_value     = 0;
    pdev->dev.out_ep[7].epx_out_rdy_value    = EP7RDY;
    pdev->dev.out_ep[7].ep_out_bcr           = (USB_BASE + 0x350);
    pdev->dev.out_ep[7].num                  = 7;
}

/**
 * @brief Connect device (enable internal pull-up)
 * @param  pdev: device instance
 * @retval : None
 */
void DCD_DevConnect(USB_CORE_HANDLE* pdev)
{
    /*Device is in default state*/
    pdev->dev.device_status = USB_DEFAULT;
}

/**
 * @brief Disconnect device (disable internal pull-up)
 * @param  pdev: device instance
 * @retval : None
 */
void DCD_DevDisconnect(USB_CORE_HANDLE* pdev)
{
    /* Disable DP Pull-Down bit*/
    SET_REG_VAL(AFEC->USBCR3, (~DM_PULLDOWN_EN) & (~RX_FLOAT_EN) & (~DP_PULLUP_EN));

    /*Device is in unconnected state*/
    pdev->dev.device_status = USB_UNCONNECTED;
}

/**
 * @brief returns the EP Status
 * @param   pdev Selected device
 * @param  epnum endpoint address
 * @retval : EP status
 */

uint32_t DCD_GetEPStatus(USB_CORE_HANDLE* pdev, uint8_t epnum)
{
    uint16_t Status = 0;

    USB_EP* ep;
    if ((0x80 & epnum) == 0x80)
    {
        ep = &pdev->dev.in_ep[epnum & 0x7F];
    }
    else
    {
        ep = &pdev->dev.out_ep[epnum];
    }
    if (ep->is_in)
    {
    }
    else
    {
    }
    return Status;
}

/**
 * @brief Set the EP Status
 * @param   pdev : Selected device
 * @param  Status : new Status
 * @param  epnum : EP address
 * @retval : None
 */
void DCD_SetEPStatus(USB_CORE_HANDLE* pdev, uint8_t epnum, uint32_t Status)
{
    USB_EP* ep;
    if ((0x80 & epnum) == 0x80)
    {
        ep = &pdev->dev.in_ep[epnum & 0x7F];
    }
    else
    {
        ep = &pdev->dev.out_ep[epnum];
    }

    if (ep->is_in)
    {
    }
    else
    {
    }
}



/** @} */
