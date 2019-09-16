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
 * @file n32g020xx_usb_regs.h
 * @author Nations Solution Team
 * @version v1.0.1
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @brief USB寄存器配置。
 * 
 * @addtogroup USB
 * @{
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __N32G020XX_USB_REGS_H__
#define __N32G020XX_USB_REGS_H__

/* Exported types ------------------------------------------------------------*/
typedef enum _EP_DBUF_DIR
{
    /* double buffered endpoint direction */
    EP_DBUF_OUT,
    EP_DBUF_IN,
    EP_DBUF_ERR,
} EP_DBUF_DIR;

/* endpoint buffer number */
enum EP_BUF_NUM
{
    EP_NOBUF,
    EP_BUF0,
    EP_BUF1
};

/******************************************************************************/
/*                         Endpoint macro                                 */
/******************************************************************************/
/* Interrupt type flag */
#define DEVINT 0x01
#define TKINT 0x02
#define EPINT 0x04
#define ERR1INT 0x08
#define ERR2INT 0x10

#define EP_CSP_HALT (0x00000001)             /* EndPoint HALT */
#define EP_CSP_INOUT_FIFO_AUTO0 (0x00000002) /* EndPoint CLR */
#define EP_CSP_SENDSTALL (0x00000004)        /* EndPoint SEND STALL */
#define EP_CSP_ONEBUF0 (0x00000008)          /* EndPoint ONE BUF */
#define EP_CSP_ONEBUF1 (0x00000010)          /* EndPoint ONE BUF */
#define EP_TX_FIFO_READY (0x00000020)        /* EndPoint TX FIFO */
#define EP_RX_FIFO_READY (0x00000040)        /* EndPoint RX_FIFO_READY */
#define EP_SETUP_READY (0x00000080)          /* EndPoint setup */
#define EP_ENABLE_EP_TOKEN_IRQ (0x00000100)          /* EndPoint & TOKEN,ENABLE IRQ */


/* Endpoint Addresses (w/direction) */
#define EP0_OUT ((uint8_t)0x00)
#define EP0_IN ((uint8_t)0x80)
#define EP1_OUT ((uint8_t)0x01)
#define EP1_IN ((uint8_t)0x81)
#define EP2_OUT ((uint8_t)0x02)
#define EP2_IN ((uint8_t)0x82)
#define EP3_OUT ((uint8_t)0x03)
#define EP3_IN ((uint8_t)0x83)
#define EP4_OUT ((uint8_t)0x04)
#define EP4_IN ((uint8_t)0x84)
#define EP5_OUT ((uint8_t)0x05)
#define EP5_IN ((uint8_t)0x85)
#define EP6_OUT ((uint8_t)0x06)
#define EP6_IN ((uint8_t)0x86)
#define EP7_OUT ((uint8_t)0x07)
#define EP7_IN ((uint8_t)0x87)

/* endpoints enumeration */
#define ENDP0 ((uint8_t)0)
#define ENDP1 ((uint8_t)1)
#define ENDP2 ((uint8_t)2)
#define ENDP3 ((uint8_t)3)
#define ENDP4 ((uint8_t)4)
#define ENDP5 ((uint8_t)5)
#define ENDP6 ((uint8_t)6)
#define ENDP7 ((uint8_t)7)

/* AFEC USB3DR */
#define DM_PULLDOWN_EN (0x08)
#define RX_FLOAT_EN (0x04)
#define DP_PULLUP_EN (0x02)
#define USB_STANDBY (0x01)

// EPCSR
#define EP0SETUPRDY 0x01
#define EP0INRDY 0x02
#define EP0OUTRDY 0x04
#define EP1RDY 0x08
#define EP2RDY 0x10
#define EP3RDY 0x20
#define EP4RDY 0x40
#define EP5RDY 0x80
#define EP6RDY 0x01
#define EP7RDY 0x02

// USB end-point interrupt flag
#define SETUP 0x01
#define EP0IN 0x02
#define EP0OUT 0x04
#define EP1IN 0x08
#define EP2OUT 0x10
#define EP3IN 0x20
#define EP4OUT 0x40
#define EP5IN 0x80
#define EP6IN 0x01
#define EP7OUT 0x02

// USB tokken interrupt flag
#define SETUPTK 0x01
#define EP0INTK 0x02
#define EP1INTK 0x08
#define EP3INTK 0x20
#define EP6INTK 0x40
#define EP5INTK 0x80

// DEVCFG
#define SETCONFIG 0x01
#define REMOTEWAKEUP 0x02
#define EP1VLD 0x04
#define EP2VLD 0x08
#define EP3VLD 0x10
#define EP4VLD 0x20
#define EP5VLD 0x40

// DEVCFG2
#define EP6VLD 0x01
#define EP7VLD 0x02

// EP0 CTRL
#define SETHALT 0x01
#define EPCLR 0x02
#define SENDSTALL 0x04 // Ep0 only
#define ONEBUFFEN 0x80 // Ep0 and Bulk-transfer-EP

// USB Ctrl
#define SRDY 0x01
#define CLRF 0x02
#define CLRFEN 0x20
#define CLRSUDREG 0x40

// USB device interrupt flag
#define URES 0x01
#define SOF 0x02
#define SUSPEND 0x04
#define RESUME 0x08

#endif /* __N32G020XX_USB_REGS_H__ */



/** @} */
