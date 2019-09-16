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
 * @file n32g020xx_usbd_hid_core.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @date    2019-01-19
 * @brief   文件为USBD HID 内核源文件。
 * @addtogroup USB
 * @{
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __N32G020XX_USBD_HID_CORE_H__
#define __N32G020XX_USBD_HID_CORE_H__

/* Includes ------------------------------------------------------------------*/
#include "n32g020xx_usbd_req.h"

/* Exported defines ----------------------------------------------------------*/
#define USB_HID_CONFIG_DESC_SIZ       34
#define USB_HID_DESC_SIZ              9
#define HID_MOUSE_REPORT_DESC_SIZE    74

#define HID_DESCRIPTOR_TYPE           0x21
#define HID_REPORT_DESC               0x22


#define HID_REQ_SET_PROTOCOL          0x0B
#define HID_REQ_GET_PROTOCOL          0x03

#define HID_REQ_SET_IDLE              0x0A
#define HID_REQ_GET_IDLE              0x02

#define HID_REQ_SET_REPORT            0x09
#define HID_REQ_GET_REPORT            0x01

/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern USBD_Class_cb_TypeDef  USBD_HID_cb;

/* Exported functions ------------------------------------------------------- */ 
 
uint8_t USBD_HID_SendReport (USB_CORE_HANDLE  *pdev, 
                                 uint8_t *report,
                                 uint16_t len);
#endif  /* __N32G020XX_USBD_HID_CORE_H__ */

/** @} */
