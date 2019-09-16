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
* @file    usbd_ccid_core.h
* @author Nations Solution Team
* @version v1.0.0
* @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
* @brief   This file provides all the CCID core functions.
* @addtogroup USB
* @{
*/ 


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CCID_CORE_H__
#define __USB_CCID_CORE_H__

/* Includes ------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "n32g020xx.h"
#include "core_cm0.h"
#include "n32g020xx_usb_regs.h"
#include "n32g020xx_usb_dcd.h"

/* Exported defines ----------------------------------------------------------*/
#define TPDU_EXCHANGE                  0x01
#define SHORT_APDU_EXCHANGE            0x02
#define EXTENDED_APDU_EXCHANGE         0x04
#define CHARACTER_EXCHANGE             0x00

#define EXCHANGE_LEVEL_FEATURE         TPDU_EXCHANGE

#define REQUEST_ABORT                  0x01
#define REQUEST_GET_CLOCK_FREQUENCIES  0x02
#define REQUEST_GET_DATA_RATES         0x03

#define SMARTCARD_SIZ_CONFIG_DESC      0x56 

#define CCID_BULK_EPIN_SIZE          CCID_BULK_EP_MAX_PACKET 
#define CCID_BULK_EPOUT_SIZE         CCID_BULK_EP_MAX_PACKET  
#define CCID_INTR_EPIN_SIZE          CCID_INTR_EP_MAX_PACKET
#define CCID_EP0_BUFF_SIZ            64

/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern USBD_Class_cb_TypeDef  USBD_CCID_cb;
/* Exported functions ------------------------------------------------------- */

#endif  /* __USB_CCID_CORE_H__ */

/** @} */

