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
  * @file    usbd_msc_data.h
  * @author Nations Solution Team
  * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
  * @brief   header for the usbd_msc_data.c file
 * @addtogroup USB
 * @{
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __USBD_MSC_DATA_H__
#define __USBD_MSC_DATA_H__

/* Includes ------------------------------------------------------------------*/
#include "n32g020xx_usbd_conf.h"
#include "n32g020xx_usbd_ioreq.h"

/* Exported defines ----------------------------------------------------------*/
#define MODE_SENSE6_LEN			 8
#define MODE_SENSE10_LEN		 8
#define LENGTH_INQUIRY_PAGE00		 7
#define LENGTH_FORMAT_CAPACITIES    	20

/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern const uint8_t MSC_Page00_Inquiry_Data[];  
extern const uint8_t MSC_Mode_Sense6_data[];
extern const uint8_t MSC_Mode_Sense10_data[]; 

/* Exported functions ------------------------------------------------------- */ 

#endif /* __USBD_MSC_DATA_H__ */

/** @} */
