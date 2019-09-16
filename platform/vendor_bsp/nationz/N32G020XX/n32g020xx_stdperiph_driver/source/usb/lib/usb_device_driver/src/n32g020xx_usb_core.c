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
 * @file n32g020xx_usb_core.c
 * @author Nations Solution Team
 * @version v1.0.1
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @date    2019-01-19
 * @brief   文件提供配置USB寄存器的接口源码文件.
 * 
 * @addtogroup USB
 * @{
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "n32g020xx_usb_regs.h"
#include "n32g020xx_usb_core.h"
#include "n32g020xx_usb_dcd.h"
#include "n32g020xx_common.h"
#include "n32g020xx_usb_log.h"
#include "n32g020xx_log.h"

/**
 * @brief Set the status of Tx endpoint.
 * @param   bEpNum: Endpoint Number
 * @param   wState: new state.
 * @retval None
 */
void SetEPTxStatus(uint8_t bEpNum, uint16_t wState)
{
    switch (bEpNum)
    {
    case 0:
        // ep0csp
        if (wState & EP_CSP_HALT)
        {
            SET_REG_BIT(USB->EP0CSR, SETHALT);
        }
        if (wState & EP_CSP_INOUT_FIFO_AUTO0)
        {
            SET_REG_BIT(USB->EP0CSR, EPCLR);
        }
        if (wState & EP_CSP_SENDSTALL)
        {
            SET_REG_BIT(USB->EP0CSR, SENDSTALL);
        }
        if (wState & EP_CSP_ONEBUF0)
        {
            // nothing
        }
        else if (wState & EP_CSP_ONEBUF1)
        {
            SET_REG_BIT(USB->EP0CSR, ONEBUFFEN);
        }

        // epcsp
        if (wState & EP_TX_FIFO_READY)
        {
            SET_REG_BIT(USB->EPCSR, EP0INRDY);
        }
        if (wState & EP_SETUP_READY)
        {
            SET_REG_BIT(USB->EPCSR, EP0SETUPRDY);
        }

		//ENABLE EP & TOKEN IRQ
        if (wState & EP_ENABLE_EP_TOKEN_IRQ)
        {
			SET_REG_BIT(USB->EPIE, EP0OUT);
			SET_REG_BIT(USB->TKIE, EP0INTK);
        }
        break;

    case 1:
        // ep1csp
        if (wState & EP_CSP_HALT)
        {
            SET_REG_BIT(USB->EP1CSR, SETHALT);
        }
        if (wState & EP_CSP_INOUT_FIFO_AUTO0)
        {
            SET_REG_BIT(USB->EP1CSR, EPCLR);
        }

        // epcsp
        if (wState & EP_TX_FIFO_READY)
        {
            SET_REG_BIT(USB->EPCSR, EP1RDY);
        }

		//ENABLE EP & TOKEN IRQ
        if (wState & EP_ENABLE_EP_TOKEN_IRQ)
        {
			SET_REG_BIT(USB->TKIE, EP1INTK);
			SET_REG_BIT(USB->DEVCFG, EP1VLD);
        }
        break;

    case 3:
        // ep3csp
        if (wState & EP_CSP_HALT)
        {
            SET_REG_BIT(USB->EP3CSR, SETHALT);
        }
        if (wState & EP_CSP_INOUT_FIFO_AUTO0)
        {
            SET_REG_BIT(USB->EP3CSR, EPCLR);
        }
        if (wState & EP_CSP_ONEBUF0)
        {
            // nothing
        }
        else if (wState & EP_CSP_ONEBUF1)
        {
            SET_REG_BIT(USB->EP3CSR, ONEBUFFEN);
        }
        // epcsp
        if (wState & EP_TX_FIFO_READY)
        {
            SET_REG_BIT(USB->EPCSR, EP3RDY);
        }
		//ENABLE EP & TOKEN IRQ
        if (wState & EP_ENABLE_EP_TOKEN_IRQ)
        {
			SET_REG_BIT(USB->TKIE, EP3INTK);
			SET_REG_BIT(USB->DEVCFG, EP3VLD);
        }
        break;

	case 5:
		// ep5csp
		if (wState & EP_CSP_HALT)
		{
			SET_REG_BIT(USB->EP5CSR, SETHALT);
		}
		if (wState & EP_CSP_INOUT_FIFO_AUTO0)
		{
			SET_REG_BIT(USB->EP5CSR, EPCLR);
		}
		if (wState & EP_CSP_ONEBUF0)
		{
			// nothing
		}
		else if (wState & EP_CSP_ONEBUF1)
		{
			// nothing
		}
		// epcsp
		if (wState & EP_TX_FIFO_READY)
		{
			SET_REG_BIT(USB->EPCSR, EP5RDY);
		}
		//ENABLE EP & TOKEN IRQ
		if (wState & EP_ENABLE_EP_TOKEN_IRQ)
		{
			SET_REG_BIT(USB->TKIE, EP5INTK);
			SET_REG_BIT(USB->EPIE, EP5IN);
			SET_REG_BIT(USB->DEVCFG, EP5VLD);
		}
		break;

	case 6:
		// ep6csp
		if (wState & EP_CSP_HALT)
		{
			SET_REG_BIT(USB->EP6CSR, SETHALT);
		}
		if (wState & EP_CSP_INOUT_FIFO_AUTO0)
		{
			SET_REG_BIT(USB->EP6CSR, EPCLR);
		}
		if (wState & EP_CSP_ONEBUF0)
		{
			// nothing
		}
		else if (wState & EP_CSP_ONEBUF1)
		{
			SET_REG_BIT(USB->EP6CSR, ONEBUFFEN);
		}
		// epcsp
		if (wState & EP_TX_FIFO_READY)
		{
			SET_REG_BIT(USB->EPCSR2, EP6RDY);
		}
		//ENABLE EP & TOKEN IRQ
		if (wState & EP_ENABLE_EP_TOKEN_IRQ)
		{
			SET_REG_BIT(USB->EPIE2, EP6IN);
			SET_REG_BIT(USB->DEVCFG2, EP6VLD);
		}
		break;


    default:
        log_usb_debuginfo("SET EP RX STS, error no endpint.\r\n");
        break;
    }
}

/**
 * @brief Set the status of Rx endpoint.
 * @param   bEpNum: Endpoint Number
 * @param   wState: new state.
 * @retval None
 */
void SetEPRxStatus(uint8_t bEpNum, uint16_t wState)
{
    //  _SetEPRxStatus(bEpNum, wState);

    switch (bEpNum)
    {
    case 0:
        // ep0csp
        if (wState & EP_CSP_HALT)
        {
            SET_REG_BIT(USB->EP0CSR, SETHALT);
        }
        if (wState & EP_CSP_INOUT_FIFO_AUTO0)
        {
            SET_REG_BIT(USB->EP0CSR, EPCLR);
        }
        if (wState & EP_CSP_SENDSTALL)
        {
            SET_REG_BIT(USB->EP0CSR, SENDSTALL);
        }
        if (wState & EP_CSP_ONEBUF0)
        {
            // nothing
        }
        else if (wState & EP_CSP_ONEBUF1)
        {
            SET_REG_BIT(USB->EP0CSR, ONEBUFFEN);
        }

        // epcsp
        if (wState & EP_RX_FIFO_READY)
        {
            SET_REG_VAL(USB->EPCSR, EP0OUTRDY);
        }
        if (wState & EP_SETUP_READY)
        {
            SET_REG_BIT(USB->EPCSR, EP0SETUPRDY);
        }
		//ENABLE EP & TOKEN IRQ
		if (wState & EP_ENABLE_EP_TOKEN_IRQ)
		{
			SET_REG_BIT(USB->EPIE, EP0OUT);
			SET_REG_BIT(USB->TKIE, EP0INTK);
		}
        break;

    case 2:
        // ep2csp
        if (wState & EP_CSP_HALT)
        {
            SET_REG_BIT(USB->EP2CSR, SETHALT);
        }
        if (wState & EP_CSP_INOUT_FIFO_AUTO0)
        {
            SET_REG_BIT(USB->EP2CSR, EPCLR);
        }

        // epcsp
        if (wState & EP_RX_FIFO_READY)
        {
            SET_REG_BIT(USB->EPCSR, EP2RDY);
        }
		//ENABLE EP & TOKEN IRQ
		if (wState & EP_ENABLE_EP_TOKEN_IRQ)
		{
			SET_REG_BIT(USB->EPIE, EP2OUT);
			SET_REG_BIT(USB->DEVCFG, EP2VLD);
		}
        break;
	case 4:
		// ep4csp
		if (wState & EP_CSP_HALT)
		{
			SET_REG_BIT(USB->EP4CSR, SETHALT);
		}
		if (wState & EP_CSP_INOUT_FIFO_AUTO0)
		{
			SET_REG_BIT(USB->EP4CSR, EPCLR);
		}
	
		// epcsp
		if (wState & EP_RX_FIFO_READY)
		{
			SET_REG_BIT(USB->EPCSR, EP4RDY);
		}
		//ENABLE EP & TOKEN IRQ
		if (wState & EP_ENABLE_EP_TOKEN_IRQ)
		{
			SET_REG_BIT(USB->EPIE, EP4OUT);
			SET_REG_BIT(USB->DEVCFG, EP4VLD);
		}
		break;
	case 7:
		// ep7csp
		if (wState & EP_CSP_HALT)
		{
			SET_REG_BIT(USB->EP7CSR, SETHALT);
		}
		if (wState & EP_CSP_INOUT_FIFO_AUTO0)
		{
			SET_REG_BIT(USB->EP7CSR, EPCLR);
		}

		// epcsp
		if (wState & EP_RX_FIFO_READY)
		{
			SET_REG_BIT(USB->EPCSR2, EP7RDY);
		}
		//ENABLE EP & TOKEN IRQ
		if (wState & EP_ENABLE_EP_TOKEN_IRQ)
		{
			SET_REG_BIT(USB->EPIE2, EP7OUT);
			SET_REG_BIT(USB->DEVCFG2, EP7VLD);
		}
		break;

    default:
        log_usb_debuginfo("SET EP RX STS, error no endpint.\r\n");
        break;
    }
}

/**
 * @brief Copy a buffer from user memory area to packet memory area (PMA)
 * @param ep    
 * @param type    
 * @param   pbUsrBuf    pointer to user memory area.
 * @param   wPMABufAddr  address into PMA.
 * @param   wNBytes  no. of bytes to be copied.
 * @retval None
 */
void PMAToUserBufferCopy(void* ep, uint8_t type, uint8_t* pbUsrBuf, uint32_t wPMABufAddr, uint16_t wNBytes)
{
    USB_EP* ep_point = (USB_EP*)ep;
    uint32_t i;

 	log_usb_debuginfo("PMAToUserBufferCopy.\r\n");
    if ((!ep) || (!pbUsrBuf) || (!wPMABufAddr))
    {
        log_usb_data("error, param null, ep = %x, pbUsrBuf = %x, wPMABufAddr = %x.\r\n", ep, pbUsrBuf, wPMABufAddr);
        return;
    }

    // log_usb_debuginfo("xfer_count = %x, max pkt = %x, xfer_len = %x, num = %x.\r\n", ep_point->xfer_count, ep_point->maxpacket,
    // ep_point->xfer_len, ep_point->num);

	log_usb_debuginfo("ep num = %x.\r\n", ep_point->num);
    if (ep_point->num <= 5)
    {
        if (ep_point->ep_out_bcr && ep_point->epcrs_phy_addr && ep_point->epxcrs_phy_addr
            && ep_point->epx_out_rdy_value)
        {
            /* get crs value, data is ready? */
            if (ep_point->epx_out_rdy_value && ep_point->ep_out_bcr)
            {
                /* get data len */
                log_usb_debuginfo("ep%d uiRecvLen = %d.\r\n", ep_point->num, wNBytes);
                if (!wNBytes)
                {
                    (*((volatile unsigned long*)(ep_point->epxcrs_phy_addr))) |= EPCLR; // fifo pointer to zero
                    (*((volatile unsigned long*)(ep_point->epcrs_phy_addr))) |=
                        ep_point->epx_out_rdy_value; // fifo pointer to zero
                    log_usb_data("xcrs = %x, crs = %x, rdyvalue = %x, .\r\n",
                        ep_point->epxcrs_phy_addr,
                        ep_point->epcrs_phy_addr,
                        ep_point->epx_out_rdy_value);
                    return;
                }

                if (wNBytes > ep_point->maxpacket)
                {
                    SetEPRxStatus(ep_point->num, EP_CSP_INOUT_FIFO_AUTO0);
                    SetEPRxStatus(ep_point->num, EP_RX_FIFO_READY);
                    SetEPRxStatus(ep_point->num, EP_CSP_SENDSTALL);
                    log_usb_data("bcr3 error, stall.wNBytes = %x, maxpacket = %x\r\n", wNBytes, ep_point->maxpacket);
                    return;
                }

                /* read data from fifo */
                // log_usb_debuginfo("[Ep%d] Receive start, addr = %x:\r\n", ep_point->num, wPMABufAddr);
                /* ep2,4,7 */
				log_usb_data("buf[%x], ", (uint32_t)pbUsrBuf);
                for (i = 0; i < wNBytes; i++)
                {
                    *pbUsrBuf = (uint8_t)(*((volatile unsigned long*)(wPMABufAddr)));
                    //if (ep_point->num == 2)
                    //{
                        log_usb_data("%d:0x%02x.   \r\n", i, *pbUsrBuf);
                        //log_usb_data("%d:%d. \r\n", i, *pbUsrBuf);
                        //log_usb_data("%c.\r\n", *pbUsrBuf);
                    //}
                    pbUsrBuf++;
                }
                 log_usb_data("[ep%d]rd[%x]\r\n", ep_point->num, wNBytes);

                /* read finished to tell fifo reg */
                if (type == USB_CTRL_SETUP)
                {
                    log_usb_debuginfo("setup:epcrs_phy_addr = [%x]\r\n", ep_point->epcrs_phy_addr);
                    (*((volatile unsigned long*)(ep_point->epcrs_phy_addr))) = EP0SETUPRDY;
                }
                else
                {
                    log_usb_debuginfo("ep:epcrs_phy_addr = %x, epx_out_rdy_value = [%x]\r\n",
                        ep_point->epcrs_phy_addr,
                        ep_point->epx_out_rdy_value);
                    (*((volatile unsigned long*)(ep_point->epcrs_phy_addr))) = ep_point->epx_out_rdy_value;
                }
            }
            else
            {
                log_usb_debuginfo("error3:bcr = %x, epcrs phy = %x, rdy value = %x, bcr = %x.\r\n",
                    ep_point->ep_out_bcr,
                    ep_point->epcrs_phy_addr,
                    ep_point->epx_out_rdy_value,
                    ep_point->ep_out_bcr);
                return;
            }
        }
        else
        {
            log_usb_debuginfo("error4:bcr = %x, epcrs phy = %x, rdy value = %x, epxcrs = %x.\r\n",
                ep_point->ep_out_bcr,
                ep_point->epcrs_phy_addr,
                ep_point->epx_out_rdy_value,
                ep_point->epxcrs_phy_addr);
            return;
        }
    }
    else
    {
        if (ep_point->ep_out_bcr && ep_point->epcrs2_phy_addr && ep_point->epxcrs_phy_addr
            && ep_point->epx_out_rdy_value)
        {
            /* get crs2 value, data is ready? */
            if (ep_point->epx_out_rdy_value && ep_point->ep_out_bcr)
            {
                /* get data len */
				log_usb_debuginfo("2#-ep%d uiRecvLen = %d.\r\n", ep_point->num, wNBytes);
                if (!wNBytes)
                {
                    (*((volatile unsigned long*)(ep_point->epxcrs_phy_addr))) |= EPCLR; // fifo pointer to zero
                    (*((volatile unsigned long*)(ep_point->epcrs2_phy_addr))) |=
                        ep_point->epx_out_rdy_value; // fifo pointer to zero
                    log_usb_data("2-xcrs = %x, crs = %x, rdyvalue = %x, .\r\n",
                        ep_point->epxcrs_phy_addr,
                        ep_point->epcrs2_phy_addr,
                        ep_point->epx_out_rdy_value);
                    return;
                }

                if (wNBytes > ep_point->maxpacket)
                {
                    SetEPRxStatus(ep_point->num, EP_CSP_INOUT_FIFO_AUTO0);
                    SetEPRxStatus(ep_point->num, EP_RX_FIFO_READY);
                    SetEPRxStatus(ep_point->num, EP_CSP_SENDSTALL);
                    log_usb_data("2-bcr3 error, stall.wNBytes = %x, xfer_count = %x\r\n", wNBytes, ep_point->xfer_count);
                    return;
                }

                if (ep_point->epxcrs_phy_addr)
                {
                    (*((volatile unsigned long*)(ep_point->epxcrs_phy_addr))) |= EPCLR; // fifo pointer to zero
                }

                /* read data from fifo */
                log_usb_debuginfo("[Ep%d] Out Receive start, addr = %x:\r\n", ep_point->num, wPMABufAddr);
                /* ep2,4,7 */
                for (i = 0; i < wNBytes; i++)
                {
                    *pbUsrBuf = (uint8_t)(*((volatile unsigned long*)(wPMABufAddr)));
                    log_usb_debuginfo("%d:0x%02x   ", i, *pbUsrBuf);
                    log_usb_debuginfo("%d:%d, \r\n", i, *pbUsrBuf);
                    pbUsrBuf++;
                }
                log_usb_debuginfo("read finished[%x]\r\n", wNBytes);

                /* read finished to tell fifo reg */
                if (type == USB_CTRL_SETUP)
                {
                    (*((volatile unsigned long*)(ep_point->epxfifo_setup_phy_addr))) = EP0SETUPRDY;
                }
                else
                {
                    (*((volatile unsigned long*)(ep_point->epcrs2_phy_addr))) = ep_point->epx_out_rdy_value;
                }
            }
            else
            {
                log_usb_data("2-error3:bcr = %x, epcrs phy = %x, rdy value = %x, bcr = %x.\r\n",
                    ep_point->ep_out_bcr,
                    ep_point->epcrs2_phy_addr,
                    ep_point->epx_out_rdy_value,
                    ep_point->ep_out_bcr);
                return;
            }
        }
        else
        {
            log_usb_data("2-error4:bcr = %x, epcrs phy = %x, rdy value = %x, epxcrs = %x.\r\n",
                ep_point->ep_out_bcr,
                ep_point->epcrs2_phy_addr,
                ep_point->epx_out_rdy_value,
                ep_point->epxcrs_phy_addr);
            return;
        }
    }
}



/** @} */
