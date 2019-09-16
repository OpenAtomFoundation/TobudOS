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
 * @file n32g020xx_usbd_core.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @date    2019-01-19
 * @brief   文件为USBD  (USB device)内核功能函数的头文件。
 *
 * @addtogroup USB
 * @{
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __N32G020XX_USBD_CORE_H__
#define __N32G020XX_USBD_CORE_H__

/* Includes ------------------------------------------------------------------*/
#include "n32g020xx_usb_dcd.h"
#include "n32g020xx_usb_dcd_int.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef enum
{
    USBD_OK = 0,
    USBD_BUSY,
    USBD_FAIL,
} USBD_Status;
/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void USBD_Init(USB_CORE_HANDLE* pdev,
               USBD_DEVICE* pDevice,
               USBD_Class_cb_TypeDef* class_cb,
               USBD_Usr_cb_TypeDef* usr_cb);

USBD_Status USBD_DeInit(USB_CORE_HANDLE* pdev);

USBD_Status USBD_ClrCfg(USB_CORE_HANDLE* pdev, uint8_t cfgidx);

USBD_Status USBD_SetCfg(USB_CORE_HANDLE* pdev, uint8_t cfgidx);

#endif /* __N32G020XX_USBD_CORE_H__ */



/** @} */
