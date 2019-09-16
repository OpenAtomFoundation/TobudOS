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
  * @file    usbd_msc_mem.h
  * @author Nations Solution Team
  * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
  * @brief   header for the STORAGE DISK file file
 * @addtogroup USB
 * @{
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_MSC_MEM_H__
#define __USBD_MSC_MEM_H__

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "n32g020xx_usbd_def.h"
#include "n32g020xx_usb_core.h"

/* Exported defines ----------------------------------------------------------*/
#define USBD_STD_INQUIRY_LENGTH		36

/* Exported types ------------------------------------------------------------*/
typedef struct _USBD_STORAGE
{
  int8_t (* Init) (uint8_t lun);
  int8_t (* GetCapacity) (uint8_t lun, uint32_t *block_num, uint32_t *block_size);
  int8_t (* IsReady) (uint8_t lun);
  int8_t (* IsWriteProtected) (uint8_t lun);
  int8_t (* Read) (uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
  int8_t (* Write)(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
  int8_t (* GetMaxLun)(void);
  int8_t *pInquiry;
  
}USBD_STORAGE_cb_TypeDef;

/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 
extern USBD_STORAGE_cb_TypeDef *USBD_STORAGE_fops;


#endif /* __USBD_MSC_MEM_H__ */

/** @} */
