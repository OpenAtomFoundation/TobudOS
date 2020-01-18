/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __SERIAL_PORT_USB_VIRTUAL_H__
#define __SERIAL_PORT_USB_VIRTUAL_H__

/*!
 * @addtogroup serial_port_usb_virtual
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief serial port USB handle size*/
#define SERIAL_PORT_USB_VIRTUAL_HANDLE_SIZE (40U)

/*! @brief USB controller ID */
typedef enum _serial_port_usb_cdc_virtual_controller_index
{
    kSerialManager_UsbVirtualControllerKhci0 = 0U, /*!< KHCI 0U */
    kSerialManager_UsbVirtualControllerKhci1 = 1U, /*!< KHCI 1U, Currently, there are no platforms which have two KHCI
                                  IPs, this is reserved to be used in the future. */
    kSerialManager_UsbVirtualControllerEhci0 = 2U, /*!< EHCI 0U */
    kSerialManager_UsbVirtualControllerEhci1 = 3U, /*!< EHCI 1U, Currently, there are no platforms which have two EHCI
                                  IPs, this is reserved to be used in the future. */

    kSerialManager_UsbVirtualControllerLpcIp3511Fs0 = 4U, /*!< LPC USB IP3511 FS controller 0 */
    kSerialManager_UsbVirtualControllerLpcIp3511Fs1 =
        5U, /*!< LPC USB IP3511 FS controller 1, there are no platforms which
       have two IP3511 IPs, this is reserved to be used in the future. */

    kSerialManager_UsbVirtualControllerLpcIp3511Hs0 = 6U, /*!< LPC USB IP3511 HS controller 0 */
    kSerialManager_UsbVirtualControllerLpcIp3511Hs1 =
        7U, /*!< LPC USB IP3511 HS controller 1, there are no platforms which
       have two IP3511 IPs, this is reserved to be used in the future. */

    kSerialManager_UsbVirtualControllerOhci0 = 8U, /*!< OHCI 0U */
    kSerialManager_UsbVirtualControllerOhci1 = 9U, /*!< OHCI 1U, Currently, there are no platforms which have two OHCI
                                  IPs, this is reserved to be used in the future. */

    kSerialManager_UsbVirtualControllerIp3516Hs0 = 10U, /*!< IP3516HS 0U */
    kSerialManager_UsbVirtualControllerIp3516Hs1 = 11U, /*!< IP3516HS 1U, Currently, there are no platforms which have
                                               two IP3516HS IPs, this is reserved to be used in the future. */
} serial_port_usb_cdc_virtual_controller_index_t;

/*! @brief serial port usb config struct*/
typedef struct _serial_port_usb_cdc_virtual_config
{
    serial_port_usb_cdc_virtual_controller_index_t controllerIndex; /*!< controller index */
} serial_port_usb_cdc_virtual_config_t;

#endif /* __SERIAL_PORT_USB_VIRTUAL_H__ */
