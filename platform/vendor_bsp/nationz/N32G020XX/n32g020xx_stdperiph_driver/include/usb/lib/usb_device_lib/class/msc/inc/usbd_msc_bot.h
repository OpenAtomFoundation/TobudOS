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
  * @file    usbd_msc_bot.h
  * @author Nations Solution Team
  * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
  * @brief   header for the usbd_msc_bot.c file
 * @addtogroup USB
 * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_MSC_BOT_H__
#define __USBD_MSC_BOT_H__

/* Includes ------------------------------------------------------------------*/
#include "n32g020xx_usbd_core.h"
#include "usbd_msc_scsi.h"
#include "n32g020xx_usbd_ioreq.h"
#include "usbd_msc_mem.h"

/* Exported defines ----------------------------------------------------------*/
#define BOT_IDLE                      0       /* Idle state */
#define BOT_DATA_OUT                  1       /* Data Out state */
#define BOT_DATA_IN                   2       /* Data In state */
#define BOT_LAST_DATA_IN              3       /* Last Data In Last */
#define BOT_SEND_DATA                 4       /* Send Immediate data */

#define BOT_CBW_SIGNATURE             0x43425355
#define BOT_CSW_SIGNATURE             0x53425355
#define BOT_CBW_LENGTH                31
#define BOT_CSW_LENGTH                13

/* CSW Status Definitions */
#define CSW_CMD_PASSED                0x00
#define CSW_CMD_FAILED                0x01
#define CSW_PHASE_ERROR               0x02

/* BOT Status */
#define BOT_STATE_NORMAL              0
#define BOT_STATE_RECOVERY            1
#define BOT_STATE_ERROR               2


#define DIR_IN                        0
#define DIR_OUT                       1
#define BOTH_DIR                      2

/* Exported types ------------------------------------------------------------*/
typedef struct _MSC_BOT_CBW
{
  uint32_t dSignature;
  uint32_t dTag;
  uint32_t dDataLength;
  uint8_t  bmFlags;
  uint8_t  bLUN;
  uint8_t  bCBLength;
  uint8_t  CB[16];
}
MSC_BOT_CBW_TypeDef;


typedef struct _MSC_BOT_CSW
{
  uint32_t dSignature;
  uint32_t dTag;
  uint32_t dDataResidue;
  uint8_t  bStatus;
}
MSC_BOT_CSW_TypeDef;

/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern uint8_t              MSC_BOT_Data[];
extern uint16_t             MSC_BOT_DataLen;
extern uint8_t              MSC_BOT_State;
extern uint8_t              MSC_BOT_BurstMode;
extern MSC_BOT_CBW_TypeDef  MSC_BOT_cbw;
extern MSC_BOT_CSW_TypeDef  MSC_BOT_csw;

/* Exported functions ------------------------------------------------------- */ 
void MSC_BOT_Init (USB_CORE_HANDLE  *pdev);
void MSC_BOT_Reset (USB_CORE_HANDLE  *pdev);
void MSC_BOT_DeInit (USB_CORE_HANDLE  *pdev);
void MSC_BOT_DataIn (USB_CORE_HANDLE  *pdev, 
                     uint8_t epnum);

void MSC_BOT_DataOut (USB_CORE_HANDLE  *pdev, 
                      uint8_t epnum);

void MSC_BOT_SendCSW (USB_CORE_HANDLE  *pdev,
                             uint8_t CSW_Status);

void  MSC_BOT_CplClrFeature (USB_CORE_HANDLE  *pdev, 
                             uint8_t epnum);

#endif /* __USBD_MSC_BOT_H__ */

/** @} */
