/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "serial_manager.h"

#if (defined(SERIAL_PORT_TYPE_SWO) && (SERIAL_PORT_TYPE_SWO > 0U))
#include "serial_port_swo.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef NDEBUG
#if (defined(DEBUG_CONSOLE_ASSERT_DISABLE) && (DEBUG_CONSOLE_ASSERT_DISABLE > 0U))
#undef assert
#define assert(n)
#endif
#endif

typedef struct _serial_swo_state
{
    serial_manager_callback_t txCallback;
    void *txCallbackParam;
    uint32_t port;
} serial_swo_state_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
serial_manager_status_t Serial_SwoInit(serial_handle_t serialHandle, void *serialConfig)
{
    serial_swo_state_t *serialSwoHandle;
    serial_port_swo_config_t *swoConfig;
    uint32_t prescaler;

    assert(serialConfig);
    assert(serialHandle);

    if (SERIAL_PORT_SWO_HANDLE_SIZE < sizeof(serial_swo_state_t))
    {
        return kStatus_SerialManager_Error;
    }

    swoConfig = (serial_port_swo_config_t *)serialConfig;

    assert(swoConfig->baudRate <= swoConfig->clockRate);
    assert((swoConfig->clockRate / swoConfig->baudRate) <= TPI_ACPR_PRESCALER_Msk);
    assert((TPI->DEVID & (TPI_DEVID_NRZVALID_Msk | TPI_DEVID_MANCVALID_Msk)) != 0U);

    serialSwoHandle = (serial_swo_state_t *)serialHandle;

    serialSwoHandle->port = swoConfig->port;

    prescaler = swoConfig->clockRate / swoConfig->baudRate - 1U;

    /* enable the ITM and DWT units */
    CoreDebug->DEMCR = CoreDebug_DEMCR_TRCENA_Msk;

    if ((CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk) == 0U)
    {
        return kStatus_SerialManager_Error;
    }

    /* Lock access */
    ITM->LAR = 0xC5ACCE55U;
    /* Disable ITM */
    ITM->TER &= ~(1U << swoConfig->port);
    ITM->TCR = 0U;
    /* select SWO encoding protocol */
    TPI->SPPR = swoConfig->protocol;
    /* select asynchronous clock prescaler */
    TPI->ACPR = prescaler & 0xFFFFU;
    /* allow unprivilege access */
    ITM->TPR = 0U;
    /* enable ITM */
    ITM->TCR = ITM_TCR_ITMENA_Msk | ITM_TCR_SYNCENA_Msk
#ifdef ITM_TCR_TraceBusID_Msk
               | ITM_TCR_TraceBusID_Msk
#elif defined(ITM_TCR_TRACEBUSID_Msk)
               | ITM_TCR_TRACEBUSID_Msk
#else
#endif
               | ITM_TCR_SWOENA_Msk | ITM_TCR_DWTENA_Msk;
    /* enable the port bits */
    ITM->TER = 1U << swoConfig->port;

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_SwoDeinit(serial_handle_t serialHandle)
{
    serial_swo_state_t *serialSwoHandle;

    assert(serialHandle);

    serialSwoHandle = (serial_swo_state_t *)serialHandle;
    /* disable ITM */
    ITM->TER &= ~(1U << serialSwoHandle->port);

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_SwoWrite(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length)
{
    serial_swo_state_t *serialSwoHandle;
    uint8_t *strAddr;
#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
    serial_manager_callback_message_t msg;
#endif

    assert(serialHandle);
    assert(buffer);
    assert(length);

    serialSwoHandle = (serial_swo_state_t *)serialHandle;

    assert((ITM->TCR & ITM_TCR_ITMENA_Msk) != 0U);
    assert((ITM->TER & (1U << serialSwoHandle->port)) != 0U);

    strAddr = buffer;
#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
    msg.buffer = buffer;
    msg.length = length;
#endif
    while (length)
    {
        /* wait FIFO ready */
        while (ITM->PORT[serialSwoHandle->port].u32 == 0U)
        {
        }

        length -= 1U;
        ITM->PORT[serialSwoHandle->port].u8 = *strAddr;

        strAddr++;
    }

#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
    if (NULL != serialSwoHandle->txCallback)
    {
        serialSwoHandle->txCallback(serialSwoHandle->txCallbackParam, &msg, kStatus_SerialManager_Success);
    }
#endif
    return kStatus_SerialManager_Success;
}

#if !(defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
serial_manager_status_t Serial_SwoRead(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length)
{
    return kStatus_SerialManager_Error;
}
#endif

serial_manager_status_t Serial_SwoCancelWrite(serial_handle_t serialHandle)
{
    return kStatus_SerialManager_Error;
}

serial_manager_status_t Serial_SwoInstallTxCallback(serial_handle_t serialHandle,
                                                    serial_manager_callback_t callback,
                                                    void *callbackParam)
{
    serial_swo_state_t *serialSwoHandle;

    assert(serialHandle);

    serialSwoHandle = (serial_swo_state_t *)serialHandle;

    serialSwoHandle->txCallback      = callback;
    serialSwoHandle->txCallbackParam = callbackParam;

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_SwoInstallRxCallback(serial_handle_t serialHandle,
                                                    serial_manager_callback_t callback,
                                                    void *callbackParam)
{
    return kStatus_SerialManager_Error;
}

void Serial_SwoIsrFunction(serial_handle_t serialHandle)
{
}

#endif
