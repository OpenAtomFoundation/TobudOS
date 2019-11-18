/*!
    \file  usbd_pwr.h
    \brief USB device power management functions prototype
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=5)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=5)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=5)
*/

#ifndef USBD_PWR_H
#define USBD_PWR_H

#include "usbd_core.h"

/* function declarations */
/* USB wakeup first operation is to wakeup mcu */
void  resume_mcu (void);
/* set USB device to suspend mode */
void  usbd_suspend (void);
/* start to remote wakeup */
void  usbd_remote_wakeup_active (void);

#endif /* USBD_PWR_H */
