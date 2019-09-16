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
 * @file n32g020xx_usbd_cdc_core.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @date    2019-01-19
 * @brief   管理USB  CDC 类的头文件.
 * 
 * @addtogroup USB
 * @{
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __N32G020XX_USBD_CDC_CORE_H__
#define __N32G020XX_USBD_CDC_CORE_H__

/* Includes ------------------------------------------------------------------*/
#include "n32g020xx_usbd_desc.h"

/* Exported defines ----------------------------------------------------------*/

#define USB_CDC_CONFIG_DESC_SIZ (67)
#define USB_CDC_DESC_SIZ (67 - 9)

#define CDC_DESCRIPTOR_TYPE 0x21

#define DEVICE_CLASS_CDC 0x02
#define DEVICE_SUBCLASS_CDC 0x00

#define USB_DEVICE_DESCRIPTOR_TYPE 0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE 0x02
#define USB_STRING_DESCRIPTOR_TYPE 0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE 0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE 0x05

#define STANDARD_ENDPOINT_DESC_SIZE 0x09

#define CDC_DATA_IN_PACKET_SIZE CDC_DATA_MAX_PACKET_SIZE

#define CDC_DATA_OUT_PACKET_SIZE CDC_DATA_MAX_PACKET_SIZE

/*---------------------------------------------------------------------*/
/*  CDC definitions                                                    */
/*---------------------------------------------------------------------*/

/**************************************************/
/* CDC Requests                                   */
/**************************************************/
#define SEND_ENCAPSULATED_COMMAND 0x00
#define GET_ENCAPSULATED_RESPONSE 0x01
#define SET_COMM_FEATURE 0x02
#define GET_COMM_FEATURE 0x03
#define CLEAR_COMM_FEATURE 0x04
#define SET_LINE_CODING 0x20
#define GET_LINE_CODING 0x21
#define SET_CONTROL_LINE_STATE 0x22
#define SEND_BREAK 0x23
#define NO_CMD 0xFF

/* Exported types ------------------------------------------------------------*/
typedef struct _CDC_IF_PROP
{
    uint16_t (*pIf_Init)(void);
    uint16_t (*pIf_DeInit)(void);
    uint16_t (*pIf_Ctrl)(uint32_t Cmd, uint8_t* Buf, uint32_t Len);
    uint16_t (*pIf_DataTx)(uint8_t ch);
    uint16_t (*pIf_DataRx)(uint8_t* Buf, uint32_t Len);
} CDC_IF_Prop_TypeDef;

/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern USBD_Class_cb_TypeDef USBD_CDC_cb;

/* Exported functions ------------------------------------------------------- */

#endif /* __N32G020XX_USBD_CDC_CORE_H__ */



/** @} */
