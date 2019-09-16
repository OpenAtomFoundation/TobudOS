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
 * @file n32g020xx_usb_core.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @date    2019-01-19
 * @brief   文件为配置USB寄存器的接口 的头文件
 * @defgroup USB USB模块
 * @{
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __N32G020XX_USB_CORE_H__
#define __N32G020XX_USB_CORE_H__
/* Includes ------------------------------------------------------------------*/
#include "n32g020xx.h"
#include "core_cm0.h"
#include "n32g020xx_usb_regs.h"

/* Exported defines ----------------------------------------------------------*/
#define EP_TYPE_CTRL 0

#define USB_EP0_IDLE 0
#define USB_EP0_SETUP 1
#define USB_EP0_DATA_IN 2
#define USB_EP0_DATA_OUT 3
#define USB_EP0_STATUS_IN 4
#define USB_EP0_STATUS_OUT 5

#define USB_MAX_EP0_SIZE 64

#define USB_SPEED_FULL 1

/* Exported types ------------------------------------------------------------*/
typedef enum
{
    USB_OK = 0,
    USB_FAIL
} USB_STS;

void SetEPTxStatus(uint8_t /*bEpNum*/, uint16_t /*wState*/);
void SetEPRxStatus(uint8_t /*bEpNum*/, uint16_t /*wState*/);
void PMAToUserBufferCopy(void* ep, uint8_t type, uint8_t* pbUsrBuf, uint32_t wPMABufAddr, uint16_t wNBytes);

#endif /* __N32G020XX_USB_CORE_H__ */



/** @} */
