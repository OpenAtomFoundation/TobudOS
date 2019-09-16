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
  * @file    usbd_msc_core.h
  * @author Nations Solution Team
  * @version v1.0.0
  * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
  * @brief   header for the usbd_msc_core.c file
 * @addtogroup USB
 * @{
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_MSC_CORE_H__
#define __USBD_MSC_CORE_H__

/* Includes ------------------------------------------------------------------*/
#include "n32g020xx_usbd_req.h"
#include "usbd_msc_bot.h"

/* Exported defines ----------------------------------------------------------*/
#define BOT_GET_MAX_LUN              0xFE
#define BOT_RESET                    0xFF
#define USB_MSC_CONFIG_DESC_SIZ      32

#define MSC_EPIN_SIZE                MSC_MAX_PACKET 
#define MSC_EPOUT_SIZE               MSC_MAX_PACKET 

/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern USBD_Class_cb_TypeDef  USBD_MSC_cb;

/* Exported functions ------------------------------------------------------- */ 

#endif  /* __USBD_MSC_CORE_H__ */

/** @} */
