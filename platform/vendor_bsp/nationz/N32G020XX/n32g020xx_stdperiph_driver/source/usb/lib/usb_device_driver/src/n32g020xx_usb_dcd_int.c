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
 * @file n32g020xx_usb_dcd_int.c
 * @author Nations Solution Team
 * @version v1.0.1
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @date    2019-01-19
 * @brief   文件提供USB总中断函数，以及总中断的下级 处理函数
 * 
 * @addtogroup USB
 * @{
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "n32g020xx_usb_dcd_int.h"
#include "n32g020xx_usbd_req.h"
#include "n32g020xx_usb_log.h"
#include "n32g020xx_log.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern USB_CORE_HANDLE USB_Device_dev;
#ifdef LPM_ENABLED
__IO uint32_t L1_remote_wakeup = 0;
__IO uint32_t BESL             = 0;
#endif
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Correct Transfer interrupt's service
 */
void EP_Handler(void)
{
    USB_EP* ep;
    uint16_t count = 0;
    uint32_t temp;
    __IO uint8_t EPStatus0, EPStatus1;

    EPStatus0 = USB->EPIR;
    EPStatus1 = USB->EPIR2;

     log_usb_debuginfo( "EPINT, EPIR = %x, EPIR2 = %x.\r\n", EPStatus0, EPStatus1);

    if ((EPStatus0 & SETUP) && (USB->EPIE & SETUP)) /* Processing the enum request in the phase */
    {
        log_usb_debuginfo("ep0, setup\r\n");
        ep             = &((&USB_Device_dev)->dev.out_ep[0]);
        ep->xfer_count = 8;

        if (((*(volatile unsigned long*)(ep->epcrs_phy_addr)) & EP0SETUPRDY) == 0)
        {
            USBD_CtlError(&USB_Device_dev, 0);
            log_usb_debuginfo("setup not setup ready, return.\r\n");
            return;
        }

        /* set fifo pointer to 0 */
        if (ep->epxcrs_phy_addr)
        {
            (*(volatile unsigned long*)(ep->epxcrs_phy_addr)) |= EPCLR;
        }
        while ((((*(volatile unsigned long*)(ep->epxcrs_phy_addr)) & EPCLR) != 0))
            ;

        /* read data from ep0 fifo to user buffer */
        PMAToUserBufferCopy((void*)ep,
                            USB_CTRL_SETUP,
                            &((&USB_Device_dev)->dev.setup_packet[0]),
                            ep->epxfifo_setup_phy_addr,
                            ep->xfer_count);
        USBD_DCD_INT_fops->SetupStage(&USB_Device_dev);

        /* set fifo pointer to 0 again */
        if (ep->epxcrs_phy_addr)
        {
            (*(volatile unsigned long*)(ep->epxcrs_phy_addr)) |= EPCLR;
        }
        while ((((*(volatile unsigned long*)(ep->epxcrs_phy_addr)) & EPCLR) != 0))
            ;
    }
    else if ((EPStatus0 & EP0OUT) && (USB->EPIE & EP0OUT))
    {
        log_usb_debuginfo("UsbEp0OutIsr\r\n");
        ep = &((&USB_Device_dev)->dev.out_ep[0]);

        /* Get Control Data OUT Packet*/
        temp = (*(volatile unsigned long*)(ep->epcrs_phy_addr));
        log_usb_debuginfo("addr = %x, temp = %x, rdy_value = %x,bcr_addr = %x \r\n",
            ep->epcrs_phy_addr,
            temp,
            ep->epx_out_rdy_value,
            ep->ep_out_bcr);
        if (!(temp & ep->epx_out_rdy_value))
        {
            log_usb_debuginfo("EP0OUT no ready!\r\n");
            return;
        }

        ep->xfer_count = (*((volatile unsigned long*)(ep->ep_out_bcr))); /* get valid length of out packet */
        log_usb_debuginfo("EP0OUT, count = %x.\r\n", ep->xfer_count);
        if (ep->xfer_count == 0)
        {
            log_usb_debuginfo("ep->epcrs_phy_addr = %x, ep->epxcrs_phy_addr = %x.\r\n", ep->epcrs_phy_addr, ep->epxcrs_phy_addr);
            (*(volatile unsigned long*)(ep->epxcrs_phy_addr)) |= EPCLR;
            (*(volatile unsigned long*)(ep->epcrs_phy_addr)) = ep->epx_out_rdy_value;

            log_usb_debuginfo("ep len is 0, return.\r\n");
            log_usb_debuginfo("Ctrl IN transfenr's status transaction\r\n");
            return;
        }
        else
        {
            if (ep->xfer_count > ep->maxpacket)
            {
                log_usb_debuginfo("error:xfer_count > maxpacketreturn.[ep:%d]\r\n", ep->num);
                SetEPRxStatus(ep->num, EP_CSP_INOUT_FIFO_AUTO0);
                SetEPRxStatus(ep->num, EP_RX_FIFO_READY);

                SetEPRxStatus(ep->num, EP_CSP_SENDSTALL);
                return;
            }

            /* Clear fifo pointer to zero */
            if (ep->epxcrs_phy_addr)
            {
                (*(volatile unsigned long*)(ep->epxcrs_phy_addr)) |= EPCLR;
            }
            PMAToUserBufferCopy((void*)ep, USB_CTRL_OUT, ep->xfer_buff, ep->epxfifo_out_phy_addr, ep->xfer_count);
            ep->xfer_buff += ep->xfer_count;
        }

        /* Process Control Data OUT Packet*/
        USBD_DCD_INT_fops->DataOutStage(&USB_Device_dev, 0x00);
    }

    if ((EPStatus0 & EP0IN) && (USB->EPIE & EP0IN))
    {
        /* Nothing to Handler */
        log_usb_debuginfo("EP0IN.r\n");
    }

    if ((EPStatus0 & EP1IN) && (USB->EPIE & EP1IN))
    {
        /* Nothing to Handler */
        log_usb_debuginfo("EP1IN.r\n");
    }

    if ((EPStatus0 & EP3IN) && (USB->EPIE & EP3IN))
    {
        /* Nothing to Handler */
        log_usb_debuginfo("EP3IN.r\n");
    }

    if ((EPStatus0 & EP5IN) && (USB->EPIE & EP5IN))
    {
        /* Nothing to Handler */
        log_usb_debuginfo("EP5IN.r\n");
    }

    if ((EPStatus1 & EP6IN) && (USB->EPIE2 & EP6IN))
    {
        /* Nothing to Handler */
        log_usb_debuginfo("EP6IN.r\n");
    }

    if ((EPStatus0 & EP2OUT) && (USB->EPIE & EP2OUT))
    {
        /* Decode and service non control endpoints interrupt  */
        // log_usb_debuginfo( "UsbEp2OutIsr\r\n");
        ep = &((&USB_Device_dev)->dev.out_ep[2]);

        /* Get Control Data OUT Packet*/
        temp = (*(volatile unsigned long*)(ep->epcrs_phy_addr));
        // log_usb_debuginfo("addr = %x, temp = %x, out_rdy = %x,bcr_addr = %x \r\n", ep->epcrs_phy_addr, temp, ep->epx_out_rdy_value,
        // ep->ep_out_bcr);
        if (!(temp & ep->epx_out_rdy_value))
        {
            log_usb_debuginfo("EP2OUT no ready!\r\n");
            return;
        }

        ep->xfer_count = (*((volatile unsigned long*)(ep->ep_out_bcr))); /* get valid length of fifo data len */
        // log_usb_debuginfo("EP2OUT, count = %x.\r\n", ep->xfer_count);
        if (ep->xfer_count == 0)
        {
            log_usb_debuginfo("ep->epcrs_phy_addr = %x, ep->epxcrs_phy_addr = %x.\r\n", ep->epcrs_phy_addr, ep->epxcrs_phy_addr);
            (*(volatile unsigned long*)(ep->epxcrs_phy_addr)) |= EPCLR;
            (*(volatile unsigned long*)(ep->epcrs_phy_addr)) = ep->epx_out_rdy_value;

            log_usb_debuginfo("ep len is 0, return.\r\n");
            log_usb_debuginfo("Ctrl IN transfenr's status transaction\r\n");
            return;
        }
        else
        {
            if (ep->xfer_count > ep->maxpacket)
            {
                log_usb_debuginfo("error:xfer_count > maxpacketreturn.[ep:%d]\r\n", ep->num);
                SetEPRxStatus(ep->num, EP_CSP_INOUT_FIFO_AUTO0);
                SetEPRxStatus(ep->num, EP_RX_FIFO_READY);

                SetEPRxStatus(ep->num, EP_CSP_SENDSTALL);
                return;
            }

            /* Clear fifo pointer to zero */
            if (ep->epxcrs_phy_addr)
            {
                (*(volatile unsigned long*)(ep->epxcrs_phy_addr)) |= EPCLR;
            }

            PMAToUserBufferCopy((void*)ep, USB_CTRL_OUT, ep->xfer_buff, ep->epxfifo_out_phy_addr, ep->xfer_count);
            ep->xfer_buff += ep->xfer_count;

            /*multi-packet on the NON control OUT endpoint*/
            ep->xfer_count += count;

            if ((ep->xfer_len == 0) || (count < ep->maxpacket))
            {
                /* RX COMPLETE */
                USBD_DCD_INT_fops->DataOutStage(&USB_Device_dev, ep->num);
            }
            else
            {
                DCD_EP_PrepareRx(&USB_Device_dev, ep->num, ep->xfer_buff, ep->xfer_len);
            }
        }
    }

    if ((EPStatus0 & EP4OUT) && (USB->EPIE & EP4OUT))
    {
        /* Decode and service non control endpoints interrupt  */
         log_usb_debuginfo( "UsbEp4OutIsr\r\n");
        ep = &((&USB_Device_dev)->dev.out_ep[4]);

        /* Get Control Data OUT Packet*/
        temp = (*(volatile unsigned long*)(ep->epcrs_phy_addr));
         log_usb_debuginfo("addr = %x, temp = %x, out_rdy = %x,bcr_addr = %x \r\n", ep->epcrs_phy_addr, temp, ep->epx_out_rdy_value,
         ep->ep_out_bcr);
        if (!(temp & ep->epx_out_rdy_value))
        {
            log_usb_debuginfo("EP4OUT no ready!\r\n");
            return;
        }

        count = (*((volatile unsigned long*)(ep->ep_out_bcr))); /* get valid length of fifo data len */
         log_usb_debuginfo("EP4OUT, rcv len = 0x%x.\r\n", count);
        if (count == 0)
        {
            log_usb_debuginfo("ep->epcrs_phy_addr = %x, ep->epxcrs_phy_addr = %x.\r\n", ep->epcrs_phy_addr, ep->epxcrs_phy_addr);
            (*(volatile unsigned long*)(ep->epxcrs_phy_addr)) |= EPCLR;
            (*(volatile unsigned long*)(ep->epcrs_phy_addr)) = ep->epx_out_rdy_value;

            log_usb_debuginfo("ep len is 0, return.\r\n");
            log_usb_debuginfo("Ctrl IN transfenr's status transaction\r\n");
            return;
        }
        else
        {
            if (count > ep->maxpacket)
            {
                log_usb_debuginfo("error:xfer_count > maxpacketreturn.[ep:%d]\r\n", ep->num);
                SetEPRxStatus(ep->num, EP_CSP_INOUT_FIFO_AUTO0);
                SetEPRxStatus(ep->num, EP_RX_FIFO_READY);

                SetEPRxStatus(ep->num, EP_CSP_SENDSTALL);
                return;
            }

            /* Clear fifo pointer to zero */
            if (ep->epxcrs_phy_addr)
            {
                (*(volatile unsigned long*)(ep->epxcrs_phy_addr)) |= EPCLR;
            }
			log_usb_debuginfo("ep->xfer_buff = %x.\r\n", ep->xfer_buff);
            PMAToUserBufferCopy((void*)ep, USB_CTRL_OUT, ep->xfer_buff, ep->epxfifo_out_phy_addr, count);
            ep->xfer_buff += count;

            /*multi-packet on the NON control OUT endpoint*/
            ep->xfer_count += count;
			log_usb_debuginfo("ep[%d]xfer_count=%x, cur=%x\r\n", ep->num, ep->xfer_count, count);
			log_usb_debuginfo("ep[%d]xfer_len=%x\r\n", ep->num, ep->xfer_len);

            if ((ep->xfer_len == 0) || (count < ep->maxpacket))
            {
                /* RX COMPLETE */
				log_usb_debuginfo("fops.\r\n");
                USBD_DCD_INT_fops->DataOutStage(&USB_Device_dev, ep->num);
            }
            else
            {
				log_usb_debuginfo("preprx.\r\n");            	
                DCD_EP_PrepareRx(&USB_Device_dev, ep->num, ep->xfer_buff, ep->xfer_len);
            }
        }
    }

    if ((EPStatus1 & EP7OUT) && (USB->EPIE2 & EP7OUT))
    {
        log_usb_debuginfo("UsbEp7OutIsr\r\n");
    }
}

/**
 * @brief ISTR events interrupt service routine
 */
void USB_IRQHandler(void)
{
    __IO uint8_t UsbTir, UsbIr, uUsb_Tkir;
    uint8_t ClrMsk;
    USB_EP* ep;
    UsbTir = USB->TIR;

    // log_usb_debuginfo("UsbTir = %x\r\n", UsbTir);
    if (UsbTir & EPINT)
    {
        // log_usb_debuginfo("entry epint crt .\r\n");

        /* servicing of the endpoint correct transfer interrupt */
        /* clear of the EP flag into the sub */
        EP_Handler();
    }
    else if (UsbTir & TKINT) /* token irq */
    {
        uUsb_Tkir = USB->TKIR;

        // log_usb_debuginfo("uUsb_Tkir = %x\r\n", uUsb_Tkir);

        ClrMsk = 0;
        if ((uUsb_Tkir & SETUPTK)) /* Setup Tokken */
        {
            ClrMsk |= SETUPTK;
            log_usb_debuginfo("TKINT clr   \r\n");
        }

        if ((uUsb_Tkir & EP0INTK) && (USB->TKIE & EP0INTK)) // Ep0 In Tokken
        {
            ep = &((&USB_Device_dev)->dev.in_ep[0]);

            /* Zero Length Packet? */
            log_usb_debuginfo("ep0 snd 1 time[0x%0x].\r\n", ep->xfer_len);
            DCD_EP_Tx(&USB_Device_dev, ep->num, ep->xfer_buff, ep->xfer_len, USB_IN_IMMEDIATE_SEND);
            if (ep->xfer_len == 0)
            {
                log_usb_debuginfo("transfer finished.\r\n");
            }

            ClrMsk |= EP0INTK;
        }

        if ((uUsb_Tkir & EP1INTK) && (USB->TKIE & EP1INTK)) /* Ep1 In Tokken */
        {
            /*multi-packet on the NON control IN endpoint*/
            ep = &((&USB_Device_dev)->dev.in_ep[1]);
            // log_usb_debuginfo("EP1 in. xfer_len = %d, \r\n", ep->xfer_len);

            if (ep->xfer_len)
            {
                // log_usb_debuginfo("buff = %x\r\n", (uint32_t)(ep->xfer_buff));
            }
            /* Zero Length Packet? */
            if (ep->xfer_len > 0)
            {
                /* TX COMPLETE */
                DCD_EP_Tx(&USB_Device_dev, ep->num, ep->xfer_buff, ep->xfer_len, USB_IN_IMMEDIATE_SEND);
            }
            else
            {
                USBD_DCD_INT_fops->DataInStage(&USB_Device_dev, ep->num, USB_IN_BUFFER_WAIT);
            }
            ClrMsk |= EP1INTK;
        }

        if ((uUsb_Tkir & EP3INTK) && (USB->TKIE & EP3INTK)) /* Ep3 In Tokken */
        {
            /*multi-packet on the NON control IN endpoint*/
            ep = &((&USB_Device_dev)->dev.in_ep[3]);
            log_usb_debuginfo("EP3 snd data:xfer_len = %d, \r\n", ep->xfer_len);

            if (ep->xfer_len)
            {
                // log_usb_debuginfo("buff = %x\r\n", (uint32_t)(ep->xfer_buff));
            }

            /* Zero Length Packet? */
            if (ep->xfer_len > 0)
            {
                /* TX COMPLETE */
                DCD_EP_Tx(&USB_Device_dev, ep->num, ep->xfer_buff, ep->xfer_len, USB_IN_IMMEDIATE_SEND);
            }
            else
            {
                /* RxReady Next Data Packet */
                USBD_DCD_INT_fops->DataInStage(&USB_Device_dev, ep->num, USB_IN_BUFFER_WAIT);
            }
            ClrMsk |= EP3INTK;
        }

        if ((uUsb_Tkir & EP5INTK) && (USB->TKIE & EP5INTK))
        {
            /*multi-packet on the NON control IN endpoint*/
            ClrMsk |= EP5INTK;
        }
        if ((uUsb_Tkir & EP6INTK) && (USB->TKIE & EP6INTK))
        {
            /*multi-packet on the NON control IN endpoint*/
            ClrMsk |= EP6INTK;
        }

        USB->TKIR = ClrMsk;
    }
    else if (UsbTir & DEVINT) /* device irq */
    {
        UsbIr = USB->USBIR;
        // log_usb_debuginfo("UsbIr = %x\r\n", UsbIr);

        ClrMsk = 0;
        if ((UsbIr & URES) && (USB->USBIE & URES)) /* usb reset */
        {
            ClrMsk |= URES;

            log_usb_debuginfo("usb reset.\r\n");
            USBD_DCD_INT_fops->Reset(&USB_Device_dev);
            DCD_EP_SetAddress(&USB_Device_dev, 0);
        }
        if ((UsbIr & SUSPEND) && (USB->USBIE & SUSPEND)) /* usb suspend  */
        {
            ClrMsk |= SUSPEND;

            /* process library core layer suspend routine*/
            USBD_DCD_INT_fops->Suspend(&USB_Device_dev);
        }
        if ((UsbIr & RESUME) && (USB->USBIE & RESUME)) /* usb resume  */
        {
            ClrMsk |= RESUME;
            log_usb_debuginfo("RESUME\r\n");
        }

        if (UsbIr & SOF) /* usb sof  */
        {
            // log_usb_debuginfo( "SOF\r\n");
            ClrMsk |= SOF;

            USBD_DCD_INT_fops->StartFrame(&USB_Device_dev);
        }
        USB->USBIR = ClrMsk;
    }
}



/** @} */
