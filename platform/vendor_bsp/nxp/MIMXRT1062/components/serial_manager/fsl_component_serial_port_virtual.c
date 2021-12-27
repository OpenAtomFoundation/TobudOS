/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_component_serial_manager.h"
#include "fsl_component_serial_port_internal.h"

#if (defined(SERIAL_PORT_TYPE_VIRTUAL) && (SERIAL_PORT_TYPE_VIRTUAL > 0U))

#include "fsl_component_serial_port_virtual.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#ifndef NDEBUG
#if (defined(DEBUG_CONSOLE_ASSERT_DISABLE) && (DEBUG_CONSOLE_ASSERT_DISABLE > 0U))
#undef assert
#define assert(n)
#else
/* MISRA C-2012 Rule 17.2 */
#undef assert
#define assert(n) \
    while (!(n))  \
    {             \
        ;         \
    }
#endif
#endif

/* Weak function. */
#if defined(__GNUC__)
#define __WEAK_FUNC __attribute__((weak))
#elif defined(__ICCARM__)
#define __WEAK_FUNC __weak
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
#define __WEAK_FUNC __attribute__((weak))
#endif

typedef struct _serial_port_virtual_send_state
{
    serial_manager_callback_t callback;
    void *callbackParam;
} serial_port_virtual_send_state_t;

typedef struct _serial_port_virtual_recv_state
{
    serial_manager_callback_t callback;
    void *callbackParam;
} serial_port_virtual_recv_state_t;

/* Define the types for application */
typedef struct _serial_port_virtual_state
{
    struct _serial_port_virtual_state *next; /* Next pointer of the interface */
    serial_port_virtual_send_state_t tx;
    serial_port_virtual_recv_state_t rx;
    uint8_t instance; /* The instance of the interface */
} serial_port_virtual_state_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static serial_port_virtual_state_t *s_serialPortVirtualHead;

/*******************************************************************************
 * Code
 ******************************************************************************/

static serial_manager_status_t Serial_PortVirtualAddItem(serial_port_virtual_state_t **head,
                                                         serial_port_virtual_state_t *node)
{
    serial_port_virtual_state_t *p = *head;
    uint32_t regPrimask;

    regPrimask = DisableGlobalIRQ();

    if (NULL == p)
    {
        *head = node;
    }
    else
    {
        while (NULL != p->next)
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

static serial_manager_status_t Serial_PortVirtualRemoveItem(serial_port_virtual_state_t **head,
                                                            serial_port_virtual_state_t *node)
{
    serial_port_virtual_state_t *p = *head;
    serial_port_virtual_state_t *q = NULL;
    uint32_t regPrimask;

    regPrimask = DisableGlobalIRQ();
    while (NULL != p)
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

static serial_manager_status_t Serial_PortVirtualGetInstance(uint8_t controller, serial_port_virtual_state_t **handle)
{
    serial_port_virtual_state_t *p = s_serialPortVirtualHead;
    uint32_t regPrimask;

    *handle    = NULL;
    regPrimask = DisableGlobalIRQ();
    while (NULL != p)
    {
        if (p->instance == controller)
        {
            *handle = p;
            break;
        }
    }
    EnableGlobalIRQ(regPrimask);
    return (NULL != p) ? kStatus_SerialManager_Success : kStatus_SerialManager_Error;
}

static void Serial_PortVirtualTxCallback(void *callbackParam,
                                         serial_manager_callback_message_t *message,
                                         serial_manager_status_t status)
{
    serial_port_virtual_state_t *serialPortVirtual;

    if (NULL != callbackParam)
    {
        serialPortVirtual = (serial_port_virtual_state_t *)callbackParam;
        if (NULL != serialPortVirtual->tx.callback)
        {
            serialPortVirtual->tx.callback(serialPortVirtual->tx.callbackParam, message, status);
        }
    }
}

static void Serial_PortVirtualRxCallback(void *callbackParam,
                                         serial_manager_callback_message_t *message,
                                         serial_manager_status_t status)
{
    serial_port_virtual_state_t *serialPortVirtual;

    if (NULL != callbackParam)
    {
        serialPortVirtual = (serial_port_virtual_state_t *)callbackParam;
        if (NULL != serialPortVirtual->rx.callback)
        {
            serialPortVirtual->rx.callback(serialPortVirtual->rx.callbackParam, message, status);
        }
    }
}

__WEAK_FUNC status_t USB_DeviceVcomInit(uint8_t controller);
__WEAK_FUNC status_t USB_DeviceVcomInit(uint8_t controller)
{
    return kStatus_Success;
}

__WEAK_FUNC status_t USB_DeviceVcomDeinit(uint8_t controller);
__WEAK_FUNC status_t USB_DeviceVcomDeinit(uint8_t controller)
{
    return kStatus_Success;
}

__WEAK_FUNC status_t USB_DeviceVcomWrite(uint8_t controller, uint8_t *buffer, uint32_t length);
__WEAK_FUNC status_t USB_DeviceVcomWrite(uint8_t controller, uint8_t *buffer, uint32_t length)
{
    serial_port_virtual_state_t *serialPortVirtual;
    serial_manager_callback_message_t msg;

    if (kStatus_SerialManager_Success != Serial_PortVirtualGetInstance(controller, &serialPortVirtual))
    {
        return kStatus_Fail;
    }

    if ((NULL != serialPortVirtual->tx.callback))
    {
        msg.buffer = buffer;
        msg.length = length;
        serialPortVirtual->tx.callback(serialPortVirtual->tx.callbackParam, &msg, kStatus_SerialManager_Success);
    }
    return kStatus_Success;
}

__WEAK_FUNC status_t USB_DeviceVcomRead(uint8_t controller, uint8_t *buffer, uint32_t length);
__WEAK_FUNC status_t USB_DeviceVcomRead(uint8_t controller, uint8_t *buffer, uint32_t length)
{
    return kStatus_Fail;
}

__WEAK_FUNC status_t USB_DeviceVcomCancelWrite(uint8_t controller);
__WEAK_FUNC status_t USB_DeviceVcomCancelWrite(uint8_t controller)
{
    return kStatus_Success;
}

__WEAK_FUNC status_t USB_DeviceVcomInstallTxCallback(uint8_t controller,
                                                     serial_manager_callback_t callback,
                                                     void *callbackParam);
__WEAK_FUNC status_t USB_DeviceVcomInstallTxCallback(uint8_t controller,
                                                     serial_manager_callback_t callback,
                                                     void *callbackParam)
{
    return kStatus_Success;
}

__WEAK_FUNC status_t USB_DeviceVcomInstallRxCallback(uint8_t controller,
                                                     serial_manager_callback_t callback,
                                                     void *callbackParam);
__WEAK_FUNC status_t USB_DeviceVcomInstallRxCallback(uint8_t controller,
                                                     serial_manager_callback_t callback,
                                                     void *callbackParam)
{
    return kStatus_Success;
}

__WEAK_FUNC void USB_DeviceVcomIsrFunction(uint8_t controller);
__WEAK_FUNC void USB_DeviceVcomIsrFunction(uint8_t controller)
{
}

serial_manager_status_t Serial_PortVirtualInit(serial_handle_t serialHandle, void *config)
{
    serial_port_virtual_state_t *serialPortVirtual;
    serial_port_virtual_config_t *serialPortVirtualConfig;
    serial_manager_status_t ret;

    assert(NULL != config);
    assert(NULL != serialHandle);
    assert(SERIAL_PORT_VIRTUAL_HANDLE_SIZE >= sizeof(serial_port_virtual_state_t));

    serialPortVirtual       = (serial_port_virtual_state_t *)serialHandle;
    serialPortVirtualConfig = (serial_port_virtual_config_t *)config;

    serialPortVirtual->instance = (uint8_t)serialPortVirtualConfig->controllerIndex;

    if ((status_t)kStatus_Success != USB_DeviceVcomInit((uint8_t)serialPortVirtualConfig->controllerIndex))
    {
        return kStatus_SerialManager_Error;
    }

    ret = Serial_PortVirtualAddItem(&s_serialPortVirtualHead, serialPortVirtual);

    if (kStatus_SerialManager_Success != ret)
    {
        return ret;
    }

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_PortVirtualDeinit(serial_handle_t serialHandle)
{
    serial_port_virtual_state_t *serialPortVirtual;

    assert(serialHandle);

    serialPortVirtual = (serial_port_virtual_state_t *)serialHandle;

    (void)USB_DeviceVcomDeinit(serialPortVirtual->instance);

    (void)Serial_PortVirtualRemoveItem(&s_serialPortVirtualHead, serialPortVirtual);

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_PortVirtualWrite(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length)
{
    serial_port_virtual_state_t *serialPortVirtual;

    assert(serialHandle);

    serialPortVirtual = (serial_port_virtual_state_t *)serialHandle;

    if (kStatus_Success != USB_DeviceVcomWrite(serialPortVirtual->instance, buffer, length))
    {
        return kStatus_SerialManager_Error;
    }

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_PortVirtualRead(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length)
{
    serial_port_virtual_state_t *serialPortVirtual;

    assert(serialHandle);

    serialPortVirtual = (serial_port_virtual_state_t *)serialHandle;

    if (kStatus_Success != USB_DeviceVcomRead(serialPortVirtual->instance, buffer, length))
    {
        return kStatus_SerialManager_Error;
    }

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_PortVirtualCancelWrite(serial_handle_t serialHandle)
{
    serial_port_virtual_state_t *serialPortVirtual;

    assert(serialHandle);

    serialPortVirtual = (serial_port_virtual_state_t *)serialHandle;

    if (kStatus_Success != USB_DeviceVcomCancelWrite(serialPortVirtual->instance))
    {
        return kStatus_SerialManager_Error;
    }

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_PortVirtualInstallTxCallback(serial_handle_t serialHandle,
                                                            serial_manager_callback_t callback,
                                                            void *callbackParam)
{
    serial_port_virtual_state_t *serialPortVirtual;

    assert(serialHandle);

    serialPortVirtual = (serial_port_virtual_state_t *)serialHandle;

    serialPortVirtual->tx.callback      = callback;
    serialPortVirtual->tx.callbackParam = callbackParam;

    return (serial_manager_status_t)USB_DeviceVcomInstallTxCallback(serialPortVirtual->instance,
                                                                    Serial_PortVirtualTxCallback, serialHandle);
}

serial_manager_status_t Serial_PortVirtualInstallRxCallback(serial_handle_t serialHandle,
                                                            serial_manager_callback_t callback,
                                                            void *callbackParam)
{
    serial_port_virtual_state_t *serialPortVirtual;

    assert(serialHandle);

    serialPortVirtual = (serial_port_virtual_state_t *)serialHandle;

    serialPortVirtual->rx.callback      = callback;
    serialPortVirtual->rx.callbackParam = callbackParam;

    return (serial_manager_status_t)USB_DeviceVcomInstallRxCallback(serialPortVirtual->instance,
                                                                    Serial_PortVirtualRxCallback, serialHandle);
}

void Serial_PortVirtualIsrFunction(serial_handle_t serialHandle)
{
    serial_port_virtual_state_t *serialPortVirtual;

    assert(serialHandle);

    serialPortVirtual = (serial_port_virtual_state_t *)serialHandle;

    USB_DeviceVcomIsrFunction(serialPortVirtual->instance);
}

#endif /* SERIAL_PORT_TYPE_VIRTUAL */
