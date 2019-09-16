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
 * @file n32g020xx_usb_dcd_int.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @date    2019-01-19
 * @brief   文件提供USB中断管理头文件
 * 
 * @addtogroup USB
 * @{
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __N32G020XX_USB_DCD_INT_H__
#define __N32G020XX_USB_DCD_INT_H__
#include <string.h>
#include "n32g020xx_usb_dcd.h"

/* Exported defines ----------------------------------------------------------*/
/* Mask defining which events has to be handled by the device application software */

#define IMR_MSK (EPINT | CNTR_WKUPM | CNTR_SUSPM | CNTR_ERRM | CNTR_SOFM | CNTR_ESOFM | CNTR_RESETM)

#ifdef LPM_ENABLED
#undef IMR_MSK
#define IMR_MSK (CNTR_CTRM | CNTR_WKUPM | CNTR_SUSPM | CNTR_ERRM | CNTR_SOFM | CNTR_ESOFM | CNTR_RESETM | CNTR_L1REQM)
#endif

/* Exported types ------------------------------------------------------------*/
typedef struct _USBD_DCD_INT
{
    uint8_t (*DataOutStage)(USB_CORE_HANDLE* pdev, uint8_t epnum);
    uint8_t (*DataInStage)(USB_CORE_HANDLE* pdev, uint8_t epnum, uint8_t tx_type);
    uint8_t (*SetupStage)(USB_CORE_HANDLE* pdev);
    uint8_t (*StartFrame)(USB_CORE_HANDLE* pdev);
    uint8_t (*Reset)(USB_CORE_HANDLE* pdev);
    uint8_t (*Suspend)(USB_CORE_HANDLE* pdev);
    uint8_t (*Resume)(USB_CORE_HANDLE* pdev);
} USBD_DCD_INT_cb_TypeDef;

extern USBD_DCD_INT_cb_TypeDef* USBD_DCD_INT_fops;

/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void EP_Handler(void);
void USB_IRQHandler(void);

#endif /* __N32G020XX_USB_DCD_INT_H__ */



/** @} */
