/*!
    \file  usbd_int.h
    \brief usb device interrupt handler header file
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=5)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=5)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=5)
*/

#ifndef USBD_INT_H
#define USBD_INT_H

#include "usbd_core.h"
#include "usbd_std.h"
#include "usbd_pwr.h"

extern usbd_core_handle_struct usb_device_dev;

typedef struct
{
    uint8_t (*SOF) (usbd_core_handle_struct *pudev); /*!< SOF ISR callback */
}usbd_int_cb_struct;

extern usbd_int_cb_struct *usbd_int_fops;

/* function declarations */
/* USB device interrupt service routine */
void  usbd_isr (void);
/* handle USB high priority successful transfer event */
uint8_t  usbd_intf_hpst (usbd_core_handle_struct *pudev);

#endif /* USBD_INT_H */
