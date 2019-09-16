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
 * @file n32g020xx_usb_dcd.h
 * @author Nations Solution Team
 * @version v1.0.1
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @date    2019-01-19
 * @brief   文件提供设备接口层的头文件。
 * 
 * @addtogroup USB
 * @{
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __N32G020XX_USB_DCD_H__
#define __N32G020XX_USB_DCD_H__

/* Includes ------------------------------------------------------------------*/
#include "n32g020xx_usb_core.h"

/* Exported defines ----------------------------------------------------------*/
#define USB_EP_CONTROL 0
#define USB_EP_ISOC 1
#define USB_EP_BULK 2
#define USB_EP_INT 3

/*  Endpoint Kind */
#define USB_SNG_BUF 0
#define USB_DBL_BUF 1

/*  Device Status */
#define USB_UNCONNECTED 0
#define USB_DEFAULT 2
#define USB_ADDRESSED 2
#define USB_CONFIGURED 3
#define USB_SUSPENDED 4

/* usb tx(ep in) type(device->host) */
#define USB_IN_BUFFER_WAIT 0x00
#define USB_IN_IMMEDIATE_SEND 0x01

/* usb data type */
#define USB_CTRL_SETUP 0x00
#define USB_CTRL_IN 0x01
#define USB_CTRL_OUT 0x02
#define USB_EP_IN 0x03
#define USB_EP_OUT 0x04

/* Exported types ------------------------------------------------------------*/
/********************************************************************************
Data structure type
********************************************************************************/
typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
} EP_DESCRIPTOR, *PEP_DESCRIPTOR;

typedef struct USB_ep
{
    uint8_t num;
    uint8_t is_in;
    uint8_t is_stall;
    uint8_t type;
    uint32_t epcrs_phy_addr;
    uint32_t epcrs2_phy_addr;
    uint32_t epxcrs_phy_addr;
    uint32_t epxfifo_in_phy_addr;
    uint32_t epxfifo_out_phy_addr;
    uint32_t epxfifo_setup_phy_addr;
    uint32_t ep_out_bcr;
    uint16_t epx_in_rdy_value;
    uint16_t epx_out_rdy_value;

    uint16_t pmaadress;
    uint16_t pmaaddr0;
    uint16_t pmaaddr1;
    uint8_t doublebuffer;
    uint8_t reserve;

    uint32_t maxpacket;
    /* transaction level variables !!! up to one max packet per transaction !!! */
    uint8_t* xfer_buff;
    uint32_t xfer_len; /* will to be send */
    uint32_t xfer_count;

    /* control transfer variables*/
    uint32_t rem_data_len;
    uint32_t total_data_len;
    uint32_t ctl_data_len;
} USB_EP;

typedef struct usb_setup_req
{
    uint8_t bmRequest;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} USB_SETUP_REQ;

typedef struct _Device_TypeDef
{
    uint8_t* (*GetDeviceDescriptor)(uint8_t speed, uint16_t* length);
#ifdef LPM_ENABLED
    uint8_t* (*GetBOSDescriptor)(uint8_t speed, uint16_t* length);
#endif
    uint8_t* (*GetLangIDStrDescriptor)(uint8_t speed, uint16_t* length);
    uint8_t* (*GetManufacturerStrDescriptor)(uint8_t speed, uint16_t* length);
    uint8_t* (*GetProductStrDescriptor)(uint8_t speed, uint16_t* length);
    uint8_t* (*GetSerialStrDescriptor)(uint8_t speed, uint16_t* length);
    uint8_t* (*GetConfigurationStrDescriptor)(uint8_t speed, uint16_t* length);
    uint8_t* (*GetInterfaceStrDescriptor)(uint8_t speed, uint16_t* length);
} USBD_DEVICE, *pUSBD_DEVICE;

typedef struct _Device_cb
{
    uint8_t (*Init)(void* pdev, uint8_t cfgidx);
    uint8_t (*DeInit)(void* pdev, uint8_t cfgidx);
    /* Control Endpoints*/
    uint8_t (*Setup)(void* pdev, USB_SETUP_REQ* req);
    uint8_t (*EP0_TxSent)(void* pdev);
    uint8_t (*EP0_RxReady)(void* pdev);
    /* Class Specific Endpoints*/
    uint8_t (*DataIn)(void* pdev, uint8_t epnum);
    uint8_t (*DataOut)(void* pdev, uint8_t epnum);
    uint8_t (*StartFrame)(void* pdev);
    uint8_t* (*GetConfigDescriptor)(uint8_t speed, uint16_t* length);

#ifdef USB_SUPPORT_USER_STRING_DESC
    uint8_t* (*GetUsrStrDescriptor)(uint8_t speed, uint8_t index, uint16_t* length);
#endif

} USBD_Class_cb_TypeDef;

typedef struct _USBD_USR_PROP
{
    void (*Init)(void);
    void (*DeviceReset)(uint8_t speed);
    void (*DeviceConfigured)(void);
    void (*DeviceSuspended)(void);
    void (*DeviceResumed)(void);
} USBD_Usr_cb_TypeDef;

typedef struct _DCD
{
    uint8_t device_config;
    uint8_t device_state;
    uint8_t device_status;
    uint8_t device_old_status;
    uint8_t device_address;
    uint32_t DevRemoteWakeup;
    uint32_t speed;
    uint8_t setup_packet[8];
    USB_EP in_ep[8];  /* totals is  8 ep. contain in ep:ep0, ep1, ep3, ep5, ep6 */
    USB_EP out_ep[8]; /* totals is  8 ep. contain out ep:ep0, ep2, ep4, ep7 */
    USBD_Class_cb_TypeDef* class_cb;
    USBD_Usr_cb_TypeDef* usr_cb;
    USBD_DEVICE* usr_device;
    uint8_t* pConfig_descriptor;
} DCD_DEV, *DCD_PDEV;

typedef struct USB_Device_handle
{
    DCD_DEV dev;
} USB_DEVICE_HANDLE, USB_CORE_HANDLE;

/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/********************************************************************************
EXPORTED FUNCTION FROM THE USB-DEVICE LAYER
********************************************************************************/
void DCD_Init(USB_CORE_HANDLE* pdev);

void DCD_DevConnect(USB_CORE_HANDLE* pdev);
void DCD_DevDisconnect(USB_CORE_HANDLE* pdev);
void DCD_EP_SetAddress(USB_CORE_HANDLE* pdev, uint8_t address);
uint32_t DCD_EP_Open(USB_CORE_HANDLE* pdev, uint16_t ep_addr, uint16_t ep_mps, uint8_t ep_type);

uint32_t DCD_EP_Close(USB_CORE_HANDLE* pdev, uint8_t ep_addr);

uint32_t DCD_EP_PrepareRx(USB_CORE_HANDLE* pdev, uint8_t ep_addr, uint8_t* pbuf, uint16_t buf_len);

uint32_t DCD_EP_Tx(USB_CORE_HANDLE* pdev, uint8_t ep_addr, uint8_t* pbuf, uint32_t buf_len, uint8_t tx_type);
uint32_t DCD_EP_Stall(USB_CORE_HANDLE* pdev, uint8_t epnum);
uint32_t DCD_EP_ClrStall(USB_CORE_HANDLE* pdev, uint8_t epnum);

uint32_t DCD_Handle_ISR(USB_CORE_HANDLE* pdev);

uint32_t DCD_GetEPStatus(USB_CORE_HANDLE* pdev, uint8_t epnum);

void DCD_SetEPStatus(USB_CORE_HANDLE* pdev, uint8_t epnum, uint32_t Status);

void DCD_StopDevice(USB_CORE_HANDLE* pdev);

#endif /* __N32G020XX_USB_DCD_H__ */



/** @} */
