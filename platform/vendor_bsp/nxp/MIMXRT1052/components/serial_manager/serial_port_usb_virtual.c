/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "serial_manager.h"

#if (defined(SERIAL_PORT_TYPE_USBCDC_VIRTUAL) && (SERIAL_PORT_TYPE_USBCDC_VIRTUAL > 0U))

#include "serial_port_usb_virtual.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#if (defined(DEBUG_CONSOLE_ASSERT_DISABLE) && (DEBUG_CONSOLE_ASSERT_DISABLE > 0U))
#undef assert
#define assert(n)
#endif

/* Weak function. */
#if defined(__GNUC__)
#define __WEAK_FUNC __attribute__((weak))
#elif defined(__ICCARM__)
#define __WEAK_FUNC __weak
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
#define __WEAK_FUNC __attribute__((weak))
#endif

typedef struct _serial_usb_cdc_virtual_send_state
{
    serial_manager_callback_t callback;
    void *callbackParam;
} serial_usb_cdc_virtual_send_state_t;

typedef struct _serial_usb_cdc_virtual_recv_state
{
    serial_manager_callback_t callback;
    void *callbackParam;
} serial_usb_cdc_virtual_recv_state_t;

/* Define the types for application */
typedef struct _serial_usb_cdc_virtual_state
{
    struct _serial_usb_cdc_virtual_state *next; /* Next pointer of the interface */
    serial_usb_cdc_virtual_send_state_t tx;
    serial_usb_cdc_virtual_recv_state_t rx;
    uint8_t instance; /* The instance of the interface */
} serial_usb_cdc_virtual_state_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static serial_usb_cdc_virtual_state_t *s_UsbCdcVirtualHead;

/*******************************************************************************
 * Code
 ******************************************************************************/

static serial_manager_status_t Serial_UsbCdcVirtualAddItem(serial_usb_cdc_virtual_state_t **head,
                                                           serial_usb_cdc_virtual_state_t *node)
{
    serial_usb_cdc_virtual_state_t *p = *head;
    uint32_t regPrimask;

    regPrimask = DisableGlobalIRQ();

    if (NULL == p)
    {
        *head = node;
    }
    else
    {
        while (p->next)
        {
            if (p == node)
            {
                EnableGlobalIRQ(regPrimask);
                return kStatus_SerialManager_Error;
            }
            p = p->next;
        }

        p->next = node;
    }
    node->next = NULL;
    EnableGlobalIRQ(regPrimask);
    return kStatus_SerialManager_Success;
}

static serial_manager_status_t Serial_UsbCdcVirtualRemoveItem(serial_usb_cdc_virtual_state_t **head,
                                                              serial_usb_cdc_virtual_state_t *node)
{
    serial_usb_cdc_virtual_state_t *p = *head;
    serial_usb_cdc_virtual_state_t *q = NULL;
    uint32_t regPrimask;

    regPrimask = DisableGlobalIRQ();
    while (p)
    {
        if (p == node)
        {
            if (NULL == q)
            {
                *head = p->next;
            }
            else
            {
                q->next = p->next;
            }
            break;
        }
        else
        {
            q = p;
            p = p->next;
        }
    }
    EnableGlobalIRQ(regPrimask);
    return kStatus_SerialManager_Success;
}

static serial_manager_status_t Serial_UsbCdcVirtualGetInstance(uint8_t controller,
                                                               serial_usb_cdc_virtual_state_t **handle)
{
    serial_usb_cdc_virtual_state_t *p = s_UsbCdcVirtualHead;
    uint32_t regPrimask;

    *handle    = NULL;
    regPrimask = DisableGlobalIRQ();
    while (p)
    {
        if (p->instance == controller)
        {
            *handle = p;
            break;
        }
    }
    EnableGlobalIRQ(regPrimask);
    return (p) ? kStatus_SerialManager_Success : kStatus_SerialManager_Error;
}

static void Serial_UsbCdcVirtualTxCallback(void *callbackParam,
                                           serial_manager_callback_message_t *message,
                                           serial_manager_status_t status)
{
    serial_usb_cdc_virtual_state_t *serialUsbCdcVirtual;

    if (callbackParam)
    {
        serialUsbCdcVirtual = (serial_usb_cdc_virtual_state_t *)callbackParam;
        if (serialUsbCdcVirtual->tx.callback)
        {
            serialUsbCdcVirtual->tx.callback(serialUsbCdcVirtual->tx.callbackParam, message, status);
        }
    }
}

static void Serial_UsbCdcVirtualRxCallback(void *callbackParam,
                                           serial_manager_callback_message_t *message,
                                           serial_manager_status_t status)
{
    serial_usb_cdc_virtual_state_t *serialUsbCdcVirtual;

    if (callbackParam)
    {
        serialUsbCdcVirtual = (serial_usb_cdc_virtual_state_t *)callbackParam;
        if (serialUsbCdcVirtual->rx.callback)
        {
            serialUsbCdcVirtual->rx.callback(serialUsbCdcVirtual->rx.callbackParam, message, status);
        }
    }
}

__WEAK_FUNC status_t USB_DeviceVcomInit(uint8_t controller)
{
    return kStatus_Success;
}

__WEAK_FUNC status_t USB_DeviceVcomDeinit(uint8_t controller)
{
    return kStatus_Success;
}

__WEAK_FUNC status_t USB_DeviceVcomWrite(uint8_t controller, uint8_t *buffer, uint32_t length)
{
    serial_usb_cdc_virtual_state_t *serialUsbCdcVirtual;
    serial_manager_callback_message_t msg;

    if (kStatus_SerialManager_Success != Serial_UsbCdcVirtualGetInstance(controller, &serialUsbCdcVirtual))
    {
        return kStatus_Fail;
    }

    if ((NULL != serialUsbCdcVirtual->tx.callback))
    {
        msg.buffer = buffer;
        msg.length = length;
        serialUsbCdcVirtual->tx.callback(serialUsbCdcVirtual->tx.callbackParam, &msg, kStatus_SerialManager_Success);
    }
    return kStatus_Success;
}

__WEAK_FUNC status_t USB_DeviceVcomRead(uint8_t controller, uint8_t *buffer, uint32_t length)
{
    return kStatus_Fail;
}

__WEAK_FUNC status_t USB_DeviceVcomCancelWrite(uint8_t controller)
{
    return kStatus_Success;
}

__WEAK_FUNC status_t USB_DeviceVcomInstallTxCallback(uint8_t controller,
                                                     serial_manager_callback_t callback,
                                                     void *callbackParam)
{
    return kStatus_Success;
}

__WEAK_FUNC status_t USB_DeviceVcomInstallRxCallback(uint8_t controller,
                                                     serial_manager_callback_t callback,
                                                     void *callbackParam)
{
    return kStatus_Success;
}

__WEAK_FUNC void USB_DeviceVcomIsrFunction(uint8_t controller)
{
}

serial_manager_status_t Serial_UsbCdcVirtualInit(serial_handle_t serialHandle, void *config)
{
    serial_usb_cdc_virtual_state_t *serialUsbCdcVirtual;
    serial_port_usb_cdc_virtual_config_t *usbCdcVirtualConfig;
    serial_manager_status_t ret;

    assert(config && serialHandle && (SERIAL_PORT_USB_VIRTUAL_HANDLE_SIZE >= sizeof(serial_usb_cdc_virtual_state_t)));

    serialUsbCdcVirtual = (serial_usb_cdc_virtual_state_t *)serialHandle;
    usbCdcVirtualConfig = (serial_port_usb_cdc_virtual_config_t *)config;

    serialUsbCdcVirtual->instance = usbCdcVirtualConfig->controllerIndex;

    if (kStatus_Success != USB_DeviceVcomInit(usbCdcVirtualConfig->controllerIndex))
    {
        return kStatus_SerialManager_Error;
    }

    ret = Serial_UsbCdcVirtualAddItem(&s_UsbCdcVirtualHead, serialUsbCdcVirtual);

    if (kStatus_SerialManager_Success != ret)
    {
        return ret;
    }

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_UsbCdcVirtualDeinit(serial_handle_t serialHandle)
{
    serial_usb_cdc_virtual_state_t *serialUsbCdcVirtual;

    assert(serialHandle);

    serialUsbCdcVirtual = (serial_usb_cdc_virtual_state_t *)serialHandle;

    USB_DeviceVcomDeinit(serialUsbCdcVirtual->instance);

    Serial_UsbCdcVirtualRemoveItem(&s_UsbCdcVirtualHead, serialUsbCdcVirtual);

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_UsbCdcVirtualWrite(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length)
{
    serial_usb_cdc_virtual_state_t *serialUsbCdcVirtual;

    assert(serialHandle);

    serialUsbCdcVirtual = (serial_usb_cdc_virtual_state_t *)serialHandle;

    if (kStatus_Success != USB_DeviceVcomWrite(serialUsbCdcVirtual->instance, buffer, length))
    {
        return kStatus_SerialManager_Error;
    }

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_UsbCdcVirtualRead(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length)
{
    serial_usb_cdc_virtual_state_t *serialUsbCdcVirtual;

    assert(serialHandle);

    serialUsbCdcVirtual = (serial_usb_cdc_virtual_state_t *)serialHandle;

    if (kStatus_Success != USB_DeviceVcomRead(serialUsbCdcVirtual->instance, buffer, length))
    {
        return kStatus_SerialManager_Error;
    }

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_UsbCdcVirtualCancelWrite(serial_handle_t serialHandle)
{
    serial_usb_cdc_virtual_state_t *serialUsbCdcVirtual;

    assert(serialHandle);

    serialUsbCdcVirtual = (serial_usb_cdc_virtual_state_t *)serialHandle;

    if (kStatus_Success != USB_DeviceVcomCancelWrite(serialUsbCdcVirtual->instance))
    {
        return kStatus_SerialManager_Error;
    }

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_UsbCdcVirtualInstallTxCallback(serial_handle_t serialHandle,
                                                              serial_manager_callback_t callback,
                                                              void *callbackParam)
{
    serial_usb_cdc_virtual_state_t *serialUsbCdcVirtual;

    assert(serialHandle);

    serialUsbCdcVirtual = (serial_usb_cdc_virtual_state_t *)serialHandle;

    serialUsbCdcVirtual->tx.callback      = callback;
    serialUsbCdcVirtual->tx.callbackParam = callbackParam;

    return (serial_manager_status_t)USB_DeviceVcomInstallTxCallback(serialUsbCdcVirtual->instance,
                                                                    Serial_UsbCdcVirtualTxCallback, serialHandle);
}

serial_manager_status_t Serial_UsbCdcVirtualInstallRxCallback(serial_handle_t serialHandle,
                                                              serial_manager_callback_t callback,
                                                              void *callbackParam)
{
    serial_usb_cdc_virtual_state_t *serialUsbCdcVirtual;

    assert(serialHandle);

    serialUsbCdcVirtual = (serial_usb_cdc_virtual_state_t *)serialHandle;

    serialUsbCdcVirtual->rx.callback      = callback;
    serialUsbCdcVirtual->rx.callbackParam = callbackParam;

    return (serial_manager_status_t)USB_DeviceVcomInstallRxCallback(serialUsbCdcVirtual->instance,
                                                                    Serial_UsbCdcVirtualRxCallback, serialHandle);
}

void Serial_UsbCdcVirtualIsrFunction(serial_handle_t serialHandle)
{
    serial_usb_cdc_virtual_state_t *serialUsbCdcVirtual;

    assert(serialHandle);

    serialUsbCdcVirtual = (serial_usb_cdc_virtual_state_t *)serialHandle;

    USB_DeviceVcomIsrFunction(serialUsbCdcVirtual->instance);
}

#endif /* SERIAL_PORT_TYPE_USBCDC_VIRTUAL */
