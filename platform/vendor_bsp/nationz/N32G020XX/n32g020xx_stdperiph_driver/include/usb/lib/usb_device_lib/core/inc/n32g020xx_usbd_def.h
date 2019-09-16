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
 * @file n32g020xx_usbd_def.h
 * @author Nations Solution Team
 * @version v1.0.1
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @date    2019-01-19
 * @brief   文件为USBD  (USB DEVICE)描述符请求相关宏定义的头文件。
 *
 * @addtogroup USB
 * @{
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __N32G020XX_USBD_DEF_H__
#define __N32G020XX_USBD_DEF_H__

/* Includes ------------------------------------------------------------------*/
#include "n32g020xx_usbd_conf.h"
#include <stdbool.h>

/* Exported defines ----------------------------------------------------------*/
#ifndef NULL
#define NULL 0
#endif

#define USB_LEN_DEV_QUALIFIER_DESC 0x0A
#define USB_LEN_CFG_DESC 0x09

#define USBD_IDX_LANGID_STR 0x00
#define USBD_IDX_MFC_STR 0x01
#define USBD_IDX_PRODUCT_STR 0x02
#define USBD_IDX_SERIAL_STR 0x03
#define USBD_IDX_CONFIG_STR 0x04
#define USBD_IDX_INTERFACE_STR 0x05

#define USB_REQ_TYPE_STANDARD 0x00
#define USB_REQ_TYPE_CLASS 0x20
#define USB_REQ_TYPE_MASK 0x60

#define USB_REQ_RECIPIENT_DEVICE 0x00
#define USB_REQ_RECIPIENT_INTERFACE 0x01
#define USB_REQ_RECIPIENT_ENDPOINT 0x02
#define USB_REQ_RECIPIENT_MASK 0x03

#define USB_REQ_GET_STATUS 0x00
#define USB_REQ_CLEAR_FEATURE 0x01
#define USB_REQ_SET_FEATURE 0x03
#define USB_REQ_SET_ADDRESS 0x05
#define USB_REQ_GET_DESCRIPTOR 0x06
#define USB_REQ_SET_DESCRIPTOR 0x07
#define USB_REQ_GET_CONFIGURATION 0x08
#define USB_REQ_SET_CONFIGURATION 0x09
#define USB_REQ_GET_INTERFACE 0x0A
#define USB_REQ_SET_INTERFACE 0x0B

#define USB_DESC_TYPE_DEVICE 1
#define USB_DESC_TYPE_CONFIGURATION 2
#define USB_DESC_TYPE_STRING 3
#define USB_DESC_TYPE_DEVICE_QUALIFIER 6
#define USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION 7
#define USB_DESC_TYPE_BOS 0xF

#define USB_CONFIG_REMOTE_WAKEUP 2
#define USB_CONFIG_SELF_POWERED 1

#define USB_FEATURE_EP_HALT 0
#define USB_FEATURE_REMOTE_WAKEUP 1
#define USB_FEATURE_TEST_MODE 2

/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
#define SWAPBYTE(addr) (((uint16_t)(*((uint8_t*)(addr)))) + (((uint16_t)(*(((uint8_t*)(addr)) + 1))) << 8))
#define LOBYTE(x) ((uint8_t)(x & 0x00FF))
#define HIBYTE(x) ((uint8_t)((x & 0xFF00) >> 8))
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

/** @addtogroup Exported_types
  * @{
  */  

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))
typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;

/** @defgroup Bit_SET_and_Bit_RESET_enumeration
  * @{
  */
typedef enum
{ 
  Bit_RESET = 0,
  Bit_SET
}BitAction;


/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __N32G020XX_USBD_DEF_H__ */



/** @} */
