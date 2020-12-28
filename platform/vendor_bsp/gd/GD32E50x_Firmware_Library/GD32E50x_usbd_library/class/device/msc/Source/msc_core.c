/*!
    \file    msc_core.c
    \brief   USB MSC device class core functions

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "msc_core.h"
#include "msc_mem.h"
#include "usbd_transc.h"
#include <string.h>

#define USBD_VID                     0x28E9U
#define USBD_PID                     0x0989U

/* note: it should use the C99 standard when compiling the below codes */
/* USB standard device descriptor */
usb_desc_dev msc_dev_desc =
{
    .header = {
        .bLength           = USB_DEV_DESC_LEN,
        .bDescriptorType   = USB_DESCTYPE_DEV
    },
    .bcdUSB                = 0x0200U,
    .bDeviceClass          = 0x00U,
    .bDeviceSubClass       = 0x00U,
    .bDeviceProtocol       = 0x00U,
    .bMaxPacketSize0       = USBD_EP0_MAX_SIZE,
    .idVendor              = USBD_VID,
    .idProduct             = USBD_PID,
    .bcdDevice             = 0x0100U,
    .iManufacturer         = STR_IDX_MFC,
    .iProduct              = STR_IDX_PRODUCT ,
    .iSerialNumber         = STR_IDX_SERIAL,
    .bNumberConfigurations = USBD_CFG_MAX_NUM
};

/* USB device configuration descriptor */
usb_msc_desc_config_set msc_config_desc = 
{
    .config = 
    {
        .header = {
            .bLength         = sizeof(usb_desc_config), 
            .bDescriptorType = USB_DESCTYPE_CONFIG
        },
        .wTotalLength        = USB_MSC_CONFIG_DESC_LEN,
        .bNumInterfaces      = 0x01U,
        .bConfigurationValue = 0x01U,
        .iConfiguration      = 0x00U,
        .bmAttributes        = 0xC0U,
        .bMaxPower           = 0x32U
    },

    .msc_itf = 
    {
        .header = {
            .bLength         = sizeof(usb_desc_itf), 
            .bDescriptorType = USB_DESCTYPE_ITF
        },
        .bInterfaceNumber    = 0x00U,
        .bAlternateSetting   = 0x00U,
        .bNumEndpoints       = 0x02U,
        .bInterfaceClass     = USB_CLASS_MSC,
        .bInterfaceSubClass  = USB_MSC_SUBCLASS_SCSI,
        .bInterfaceProtocol  = USB_MSC_PROTOCOL_BBB,
        .iInterface          = 0x00U
    },

    .msc_epin = 
    {
        .header = {
            .bLength         = sizeof(usb_desc_ep), 
            .bDescriptorType = USB_DESCTYPE_EP
        },
        .bEndpointAddress    = MSC_IN_EP,
        .bmAttributes        = USB_EP_ATTR_BULK,
        .wMaxPacketSize      = MSC_EPIN_SIZE,
        .bInterval           = 0x00U
    },

    .msc_epout = 
    {
        .header = {
            .bLength         = sizeof(usb_desc_ep), 
            .bDescriptorType = USB_DESCTYPE_EP
        },
        .bEndpointAddress    = MSC_OUT_EP,
        .bmAttributes        = USB_EP_ATTR_BULK,
        .wMaxPacketSize      = MSC_EPOUT_SIZE,
        .bInterval           = 0x00U
    }
};

/* USB language ID descriptor */
usb_desc_LANGID usbd_language_id_desc = 
{
    .header = 
     {
         .bLength            = sizeof(usb_desc_LANGID), 
         .bDescriptorType    = USB_DESCTYPE_STR
     },
    .wLANGID                 = ENG_LANGID
};

/* USB manufacture string */
static usb_desc_str manufacturer_string = 
{
    .header = 
     {
         .bLength         = USB_STRING_LEN(10U), 
         .bDescriptorType = USB_DESCTYPE_STR,
     },
    .unicode_string = {'G', 'i', 'g', 'a', 'D', 'e', 'v', 'i', 'c', 'e'}
};

/* USB product string */
static usb_desc_str product_string = 
{
    .header = 
     {
         .bLength         = USB_STRING_LEN(12U), 
         .bDescriptorType = USB_DESCTYPE_STR,
     },
    .unicode_string = {'G', 'D', '3', '2', '-', 'U', 'S', 'B', '_', 'M', 'S', 'C'}
};

/* USB serial string */
static usb_desc_str serial_string = 
{
    .header = 
     {
         .bLength         = USB_STRING_LEN(12U), 
         .bDescriptorType = USB_DESCTYPE_STR,
     }
};

/* USB string descriptor set */
uint8_t* usbd_msc_strings[] = 
{
    [STR_IDX_LANGID]  = (uint8_t *)&usbd_language_id_desc,
    [STR_IDX_MFC]     = (uint8_t *)&manufacturer_string,
    [STR_IDX_PRODUCT] = (uint8_t *)&product_string,
    [STR_IDX_SERIAL]  = (uint8_t *)&serial_string
};

usb_desc msc_desc = {
    .dev_desc    = (uint8_t *)&msc_dev_desc,
    .config_desc = (uint8_t *)&msc_config_desc,
    .strings     = usbd_msc_strings
};

/* local function prototypes ('static') */
static uint8_t msc_init             (usb_dev *udev, uint8_t config_index);
static uint8_t msc_deinit           (usb_dev *udev, uint8_t config_index);
static uint8_t msc_req_handler      (usb_dev *udev, usb_req *req);
static void msc_data_in_handler     (usb_dev *udev, uint8_t ep_num);
static void msc_data_out_handler    (usb_dev *udev, uint8_t ep_num);

usb_class msc_class = {
    .init             = msc_init,
    .deinit           = msc_deinit,
    .req_process      = msc_req_handler,
    .data_in          = msc_data_in_handler,
    .data_out         = msc_data_out_handler
};

/*!
    \brief      initialize the MSC device
    \param[in]  udev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t msc_init (usb_dev *udev, uint8_t config_index)
{
    uint8_t i = 0U;

    static usbd_msc_handler msc_handler;

    memset((void *)&msc_handler, 0, sizeof(usbd_msc_handler));

    /* initialize TX endpoint */
    usbd_ep_init(udev, EP_BUF_SNG, BULK_TX_ADDR, &(msc_config_desc.msc_epin));

    /* initialize RX endpoint */
    usbd_ep_init(udev, EP_BUF_SNG, BULK_RX_ADDR, &(msc_config_desc.msc_epout));

    udev->ep_transc[EP_ID(MSC_IN_EP)][TRANSC_IN] = msc_class.data_in;
    udev->ep_transc[MSC_OUT_EP][TRANSC_OUT] = msc_class.data_out;

    for (i = 0U; i < MEM_LUN_NUM; i ++) {
        usbd_mem_fops->mem_init(i);
    }

    msc_handler.bbb_transport_stage = MSC_STAGE_CMD;

    /* prepare endpoint to receive first bbb_cbw */
    usbd_ep_recev (udev, MSC_OUT_EP, (uint8_t *)&msc_handler.bbb_cbw, BBB_CBW_LENGTH);

    udev->class_data[USBD_MSC_INTERFACE] = (void *)&msc_handler;

    return USBD_OK;
}

/*!
    \brief      de-initialize the MSC device
    \param[in]  udev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t msc_deinit (usb_dev *udev, uint8_t config_index)
{
    usbd_msc_handler *msc = (usbd_msc_handler *)udev->class_data[USBD_MSC_INTERFACE];

    /* deinitialize MSC endpoints */
    usbd_ep_deinit (udev, MSC_IN_EP);
    usbd_ep_deinit (udev, MSC_OUT_EP);

    msc->bbb_transport_stage = MSC_STAGE_CMD;

    /* prepare endpoint to receive first bbb_cbw */
    usbd_ep_recev (udev, MSC_OUT_EP, (uint8_t *)&msc->bbb_cbw, BBB_CBW_LENGTH);

    return USBD_OK;
}

/*!
    \brief      handle the MSC class-specific and standard requests
    \param[in]  udev: pointer to USB device instance
    \param[in]  req: device class-specific request
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t msc_req_handler (usb_dev *udev, usb_req *req)
{
    uint8_t status = REQ_NOTSUPP;

    usbd_msc_handler *msc = (usbd_msc_handler *)udev->class_data[USBD_MSC_INTERFACE];

    switch (req->bRequest) {
    case BBB_GET_MAX_LUN:
        msc->usbd_msc_maxlun = (uint8_t)usbd_mem_fops->mem_maxlun();

        msc->bbb_transport_stage = MSC_STAGE_CMD;

        usb_transc_config(&udev->transc_in[0], &msc->usbd_msc_maxlun, 1U, 0U);

        status = REQ_SUPP;
        break;

    case BBB_RESET:
        msc->bbb_transport_stage = MSC_STAGE_CMD;

        /* prepare endpoint to receive first bbb_cbw */
        usbd_ep_recev (udev, MSC_OUT_EP, (uint8_t *)&msc->bbb_cbw, BBB_CBW_LENGTH);

        status = REQ_SUPP;
        break;

    default:
        break;
    }

    return status;
}

/*!
    \brief      handle data stage
    \param[in]  udev: pointer to USB device instance
    \param[in]  ep_num: the endpoint ID
    \param[out] none
    \retval     none
*/
static void msc_data_in_handler (usb_dev *udev, uint8_t ep_num)
{
    usb_transc *transc = &udev->transc_in[ep_num];

    usbd_msc_handler *msc = (usbd_msc_handler *)udev->class_data[USBD_MSC_INTERFACE];

    if (transc->xfer_len) {
        usbd_ep_send (udev, ep_num, transc->xfer_buf, transc->xfer_len);
    } else {
        if (MSC_STAGE_CMD != msc->bbb_transport_stage) {
            process_scsi_command(msc->bbb_cbw.bCBWLUN, udev); 
        }
    }
}

/*!
    \brief      handle data out stage
    \param[in]  udev: pointer to USB device instance
    \param[in]  ep_num: the endpoint ID
    \param[out] none
    \retval     none
*/
static void msc_data_out_handler (usb_dev *udev, uint8_t ep_num)
{
    usbd_msc_handler *msc = (usbd_msc_handler *)udev->class_data[USBD_MSC_INTERFACE];

    if (MSC_STAGE_CMD == msc->bbb_transport_stage){
        scsi_command_parse(udev);

        process_scsi_command(msc->bbb_cbw.bCBWLUN, udev);
    } else if (MSC_STAGE_DATA_OUT == msc->bbb_transport_stage) {
        process_scsi_command(msc->bbb_cbw.bCBWLUN, udev); 
    } else {
        /* no operation */
    }
}
