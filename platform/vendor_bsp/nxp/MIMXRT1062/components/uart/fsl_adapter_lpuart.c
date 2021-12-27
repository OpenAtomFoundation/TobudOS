/*
 * Copyright 2018, 2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_lpuart.h"

#include "fsl_adapter_uart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef NDEBUG
#if (defined(DEBUG_CONSOLE_ASSERT_DISABLE) && (DEBUG_CONSOLE_ASSERT_DISABLE > 0U))
#undef assert
#define assert(n)
#endif
#endif

#ifndef HAL_UART_ADAPTER_LOWPOWER_RESTORE
#define HAL_UART_ADAPTER_LOWPOWER_RESTORE (1)
#endif

#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))
/*! @brief uart RX state structure. */
typedef struct _hal_uart_receive_state
{
    uint8_t *volatile buffer;
    volatile uint32_t bufferLength;
    volatile uint32_t bufferSofar;
} hal_uart_receive_state_t;

/*! @brief uart TX state structure. */
typedef struct _hal_uart_send_state
{
    uint8_t *volatile buffer;
    volatile uint32_t bufferLength;
    volatile uint32_t bufferSofar;
} hal_uart_send_state_t;
#endif
/*! @brief uart state structure. */
typedef struct _hal_uart_state
{
    uint8_t instance;
#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))
    hal_uart_block_mode_t mode;
    hal_uart_transfer_callback_t callback;
    void *callbackParam;
#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
    lpuart_handle_t hardwareHandle;
#endif
    hal_uart_receive_state_t rx;
    hal_uart_send_state_t tx;
#endif
#if (defined(HAL_UART_ADAPTER_LOWPOWER) && (HAL_UART_ADAPTER_LOWPOWER > 0U))
#if (defined(HAL_UART_ADAPTER_LOWPOWER_RESTORE) && (HAL_UART_ADAPTER_LOWPOWER_RESTORE > 0U))
    uint32_t reg_BAUD;
    uint32_t reg_CTRL;
    uint32_t reg_WATER;
    uint32_t reg_MODIR;
#else
    hal_uart_config_t config;
#endif
#endif
} hal_uart_state_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static LPUART_Type *const s_LpuartAdapterBase[] = LPUART_BASE_PTRS;
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
#if (defined(HAL_UART_ADAPTER_LOWPOWER) && (HAL_UART_ADAPTER_LOWPOWER > 0U))
#if (defined(HAL_UART_ADAPTER_LOWPOWER_RESTORE) && (HAL_UART_ADAPTER_LOWPOWER_RESTORE > 0U))
static const clock_ip_name_t s_LpuartAdapterClock[] = LPUART_CLOCKS;
#endif /* HAL_UART_ADAPTER_LOWPOWER_RESTORE */
#endif /* HAL_UART_ADAPTER_LOWPOWER */
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))

#if !(defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
/* Array of LPUART IRQ number. */
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
static const IRQn_Type s_LpuartRxIRQ[] = LPUART_RX_IRQS;
static const IRQn_Type s_LpuartTxIRQ[] = LPUART_TX_IRQS;
#else
static const IRQn_Type s_LpuartIRQ[] = LPUART_RX_TX_IRQS;
#endif
#endif

#if !(defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
static hal_uart_state_t *s_UartState[sizeof(s_LpuartAdapterBase) / sizeof(LPUART_Type *)];
#endif

#endif

/*******************************************************************************
 * Code
 ******************************************************************************/

#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
static hal_uart_status_t HAL_UartGetStatus(status_t status)
{
    hal_uart_status_t uartStatus = kStatus_HAL_UartError;
    switch (status)
    {
        case (int32_t)kStatus_Success:
            uartStatus = kStatus_HAL_UartSuccess;
            break;
        case (int32_t)kStatus_LPUART_TxBusy:
            uartStatus = kStatus_HAL_UartTxBusy;
            break;
        case (int32_t)kStatus_LPUART_RxBusy:
            uartStatus = kStatus_HAL_UartRxBusy;
            break;
        case (int32_t)kStatus_LPUART_TxIdle:
            uartStatus = kStatus_HAL_UartTxIdle;
            break;
        case (int32_t)kStatus_LPUART_RxIdle:
            uartStatus = kStatus_HAL_UartRxIdle;
            break;
        case (int32_t)kStatus_LPUART_BaudrateNotSupport:
            uartStatus = kStatus_HAL_UartBaudrateNotSupport;
            break;
        case (int32_t)kStatus_LPUART_NoiseError:
        case (int32_t)kStatus_LPUART_FramingError:
        case (int32_t)kStatus_LPUART_ParityError:
            uartStatus = kStatus_HAL_UartProtocolError;
            break;
        default:
            /*MISRA rule 16.4*/
            break;
    }
    return uartStatus;
}
#else
static hal_uart_status_t HAL_UartGetStatus(status_t status)
{
    hal_uart_status_t uartStatus;
    if ((int32_t)kStatus_Success == status)
    {
        uartStatus = kStatus_HAL_UartSuccess; /* Successfully */
    }
    else
    {
        uartStatus = kStatus_HAL_UartError; /* Error occurs on HAL uart */
    }
    return uartStatus;
}
#endif

#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))

#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
static void HAL_UartCallback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *callbackParam)
{
    hal_uart_state_t *uartHandle;
    hal_uart_status_t uartStatus = HAL_UartGetStatus(status);
    assert(callbackParam);

    uartHandle = (hal_uart_state_t *)callbackParam;

    assert(uartHandle->mode != kHAL_UartBlockMode);

    if (kStatus_HAL_UartProtocolError == uartStatus)
    {
        if (0U != uartHandle->hardwareHandle.rxDataSize)
        {
            uartStatus = kStatus_HAL_UartError;
        }
    }

    if (NULL != uartHandle->callback)
    {
        uartHandle->callback(uartHandle, uartStatus, uartHandle->callbackParam);
    }
}

#else

static void HAL_UartInterruptHandle(uint8_t instance)
{
    hal_uart_state_t *uartHandle = s_UartState[instance];
    uint32_t status;
#if (defined(HAL_UART_ADAPTER_FIFO) && (HAL_UART_ADAPTER_FIFO > 0u))
    uint8_t count;
#endif

    assert(uartHandle);

    status = LPUART_GetStatusFlags(s_LpuartAdapterBase[instance]);

#if (defined(HAL_UART_ADAPTER_FIFO) && (HAL_UART_ADAPTER_FIFO > 0u))
    /* If RX overrun. */
    if ((uint32_t)kLPUART_RxOverrunFlag == ((uint32_t)kLPUART_RxOverrunFlag & status))
    {
        /* Clear overrun flag, otherwise the RX does not work. */
        s_LpuartAdapterBase[instance]->STAT =
            ((s_LpuartAdapterBase[instance]->STAT & 0x3FE00000U) | LPUART_STAT_OR_MASK);
    }
#endif

    /* Receive data register full */
    if (((0U != ((uint32_t)kLPUART_RxDataRegFullFlag & status)) &&
         (0U != (LPUART_GetEnabledInterrupts(s_LpuartAdapterBase[instance]) &
                 (uint32_t)kLPUART_RxDataRegFullInterruptEnable)))
#if (defined(HAL_UART_ADAPTER_FIFO) && (HAL_UART_ADAPTER_FIFO > 0u))
        || ((0U != ((uint32_t)kLPUART_IdleLineFlag & status)) &&
            (0U !=
             (LPUART_GetEnabledInterrupts(s_LpuartAdapterBase[instance]) & (uint32_t)kLPUART_IdleLineInterruptEnable)))
#endif
    )
    {
        if (NULL != uartHandle->rx.buffer)
        {
#if (defined(HAL_UART_ADAPTER_FIFO) && (HAL_UART_ADAPTER_FIFO > 0u))
            /* Get the size that can be stored into buffer for this interrupt. */
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
            count = ((uint8_t)((s_LpuartAdapterBase[instance]->WATER & LPUART_WATER_RXCOUNT_MASK) >>
                               LPUART_WATER_RXCOUNT_SHIFT));
#else
            count = 1u;
#endif
            while (0u != count)
            {
                count--;
#endif
                uartHandle->rx.buffer[uartHandle->rx.bufferSofar++] = LPUART_ReadByte(s_LpuartAdapterBase[instance]);
                if (uartHandle->rx.bufferSofar >= uartHandle->rx.bufferLength)
                {
                    LPUART_DisableInterrupts(
                        s_LpuartAdapterBase[instance], (uint32_t)kLPUART_RxDataRegFullInterruptEnable |
                                                           (uint32_t)kLPUART_RxOverrunInterruptEnable
#if (defined(HAL_UART_ADAPTER_FIFO) && (HAL_UART_ADAPTER_FIFO > 0u))
                                                           | (uint32_t)kLPUART_IdleLineInterruptEnable
#endif
                    );
                    uartHandle->rx.buffer = NULL;
#if (defined(HAL_UART_ADAPTER_FIFO) && (HAL_UART_ADAPTER_FIFO > 0u))
                    count = 0u;
#endif
                    if (NULL != uartHandle->callback)
                    {
                        uartHandle->callback(uartHandle, kStatus_HAL_UartRxIdle, uartHandle->callbackParam);
                    }
                }
#if (defined(HAL_UART_ADAPTER_FIFO) && (HAL_UART_ADAPTER_FIFO > 0u))
            }
#endif
        }
#if (defined(HAL_UART_ADAPTER_FIFO) && (HAL_UART_ADAPTER_FIFO > 0u))
        if ((0U != ((uint32_t)kLPUART_IdleLineFlag & status)) &&
            (0U !=
             (LPUART_GetEnabledInterrupts(s_LpuartAdapterBase[instance]) & (uint32_t)kLPUART_IdleLineInterruptEnable)))
        {
            s_LpuartAdapterBase[instance]->STAT |= ((uint32_t)kLPUART_IdleLineFlag);
        }
#endif
    }

    /* Send data register empty and the interrupt is enabled. */
    if ((0U != (LPUART_STAT_TDRE_MASK & status)) && (0U != (LPUART_GetEnabledInterrupts(s_LpuartAdapterBase[instance]) &
                                                            (uint32_t)kLPUART_TxDataRegEmptyInterruptEnable)))
    {
        if (NULL != uartHandle->tx.buffer)
        {
#if (defined(HAL_UART_ADAPTER_FIFO) && (HAL_UART_ADAPTER_FIFO > 0u))
            /* Get the size that transmit buffer for this interrupt. */
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
            count = (uint8_t)FSL_FEATURE_LPUART_FIFO_SIZEn(s_LpuartAdapterBase[instance]) -
                    (uint8_t)((s_LpuartAdapterBase[instance]->WATER & LPUART_WATER_TXCOUNT_MASK) >>
                              LPUART_WATER_TXCOUNT_SHIFT);
#else
            count = 1u;
#endif
            while (0u != count)
            {
                count--;
#endif
                LPUART_WriteByte(s_LpuartAdapterBase[instance], uartHandle->tx.buffer[uartHandle->tx.bufferSofar++]);
                if (uartHandle->tx.bufferSofar >= uartHandle->tx.bufferLength)
                {
                    LPUART_DisableInterrupts(s_LpuartAdapterBase[instance],
                                             (uint32_t)kLPUART_TxDataRegEmptyInterruptEnable);
                    uartHandle->tx.buffer = NULL;
#if (defined(HAL_UART_ADAPTER_FIFO) && (HAL_UART_ADAPTER_FIFO > 0u))
                    count = 0u;
#endif
                    if (NULL != uartHandle->callback)
                    {
                        uartHandle->callback(uartHandle, kStatus_HAL_UartTxIdle, uartHandle->callbackParam);
                    }
                }
#if (defined(HAL_UART_ADAPTER_FIFO) && (HAL_UART_ADAPTER_FIFO > 0u))
            }
#endif
        }
    }

#if !(defined(HAL_UART_ADAPTER_FIFO) && (HAL_UART_ADAPTER_FIFO > 0u))
    (void)LPUART_ClearStatusFlags(s_LpuartAdapterBase[instance], status);
#endif
}
#endif

#endif

hal_uart_status_t HAL_UartInit(hal_uart_handle_t handle, const hal_uart_config_t *config)
{
    hal_uart_state_t *uartHandle;
    lpuart_config_t lpuartConfig;
    status_t status;
    hal_uart_status_t uartStatus = kStatus_HAL_UartSuccess;
    assert(handle);
    assert(config);
    assert(config->instance < (sizeof(s_LpuartAdapterBase) / sizeof(LPUART_Type *)));
    assert(s_LpuartAdapterBase[config->instance]);
    assert(HAL_UART_HANDLE_SIZE >= sizeof(hal_uart_state_t));

    LPUART_GetDefaultConfig(&lpuartConfig);
    lpuartConfig.baudRate_Bps = config->baudRate_Bps;
    lpuartConfig.parityMode   = (lpuart_parity_mode_t)config->parityMode;
    lpuartConfig.stopBitCount = (lpuart_stop_bit_count_t)config->stopBitCount;
    lpuartConfig.enableRx     = (bool)config->enableRx;
    lpuartConfig.enableTx     = (bool)config->enableTx;
#if (defined(HAL_UART_ADAPTER_FIFO) && (HAL_UART_ADAPTER_FIFO > 0u))
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
    if (config->txFifoWatermark > 0U)
    {
        lpuartConfig.txFifoWatermark =
            MIN(config->txFifoWatermark, FSL_FEATURE_LPUART_FIFO_SIZEn(s_LpuartAdapterBase[config->instance])) - 1U;
    }
    if (config->rxFifoWatermark > 0U)
    {
        lpuartConfig.rxFifoWatermark =
            MIN(config->rxFifoWatermark, FSL_FEATURE_LPUART_FIFO_SIZEn(s_LpuartAdapterBase[config->instance])) - 1U;
    }
#endif
#endif
#if defined(FSL_FEATURE_LPUART_HAS_MODEM_SUPPORT) && FSL_FEATURE_LPUART_HAS_MODEM_SUPPORT
    lpuartConfig.enableRxRTS = (bool)config->enableRxRTS;
    lpuartConfig.enableTxCTS = (bool)config->enableTxCTS;
#endif /* FSL_FEATURE_LPUART_HAS_MODEM_SUPPORT */
    status = LPUART_Init(s_LpuartAdapterBase[config->instance], (void *)&lpuartConfig, config->srcClock_Hz);

    if ((int32_t)kStatus_Success != status)
    {
        uartStatus = HAL_UartGetStatus(status); /*Get current uart status*/
    }
    else
    {
        uartHandle           = (hal_uart_state_t *)handle;
        uartHandle->instance = config->instance;

#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))
        uartHandle->mode = config->mode;
#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
        LPUART_TransferCreateHandle(s_LpuartAdapterBase[config->instance], &uartHandle->hardwareHandle,
                                    (lpuart_transfer_callback_t)HAL_UartCallback, handle);
#else
        s_UartState[uartHandle->instance] = uartHandle;
/* Enable interrupt in NVIC. */
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
        NVIC_SetPriority((IRQn_Type)s_LpuartRxIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
        EnableIRQ(s_LpuartRxIRQ[uartHandle->instance]);
        NVIC_SetPriority((IRQn_Type)s_LpuartTxIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
        EnableIRQ(s_LpuartTxIRQ[uartHandle->instance]);
#else
        NVIC_SetPriority((IRQn_Type)s_LpuartIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
        (void)EnableIRQ(s_LpuartIRQ[uartHandle->instance]);
#endif
#endif

#endif
#if (defined(HAL_UART_ADAPTER_LOWPOWER) && (HAL_UART_ADAPTER_LOWPOWER > 0U))
#if (defined(HAL_UART_ADAPTER_LOWPOWER_RESTORE) && (HAL_UART_ADAPTER_LOWPOWER_RESTORE > 0U))
        uartHandle->reg_BAUD  = s_LpuartAdapterBase[uartHandle->instance]->BAUD;
        uartHandle->reg_CTRL  = s_LpuartAdapterBase[uartHandle->instance]->CTRL;
        uartHandle->reg_WATER = s_LpuartAdapterBase[uartHandle->instance]->WATER;
        uartHandle->reg_MODIR = s_LpuartAdapterBase[uartHandle->instance]->MODIR;
#else
        memcpy(&uartHandle->config, config, sizeof(hal_uart_config_t));
#endif
#endif
    }

    return uartStatus;
}

hal_uart_status_t HAL_UartDeinit(hal_uart_handle_t handle)
{
    hal_uart_state_t *uartHandle;

    assert(handle);

    uartHandle = (hal_uart_state_t *)handle;

    LPUART_Deinit(s_LpuartAdapterBase[uartHandle->instance]); /*LPUART Deinitialization*/

#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))

#if !(defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
    s_UartState[uartHandle->instance] = NULL;
#endif

#endif

    return kStatus_HAL_UartSuccess;
}

hal_uart_status_t HAL_UartReceiveBlocking(hal_uart_handle_t handle, uint8_t *data, size_t length)
{
    hal_uart_state_t *uartHandle;
    status_t status;
    assert(handle);
    assert(data);
    assert(length);

    uartHandle = (hal_uart_state_t *)handle;

    status = LPUART_ReadBlocking(s_LpuartAdapterBase[uartHandle->instance], data, length);

    return HAL_UartGetStatus(status);
}

hal_uart_status_t HAL_UartSendBlocking(hal_uart_handle_t handle, const uint8_t *data, size_t length)
{
    hal_uart_state_t *uartHandle;

    assert(handle);
    assert(data);
    assert(length);

    uartHandle = (hal_uart_state_t *)handle;

    (void)LPUART_WriteBlocking(s_LpuartAdapterBase[uartHandle->instance], data, length);

    return kStatus_HAL_UartSuccess;
}

hal_uart_status_t HAL_UartEnterLowpower(hal_uart_handle_t handle)
{
    assert(handle);

    return kStatus_HAL_UartSuccess;
}

hal_uart_status_t HAL_UartExitLowpower(hal_uart_handle_t handle)
{
#if (defined(HAL_UART_ADAPTER_LOWPOWER) && (HAL_UART_ADAPTER_LOWPOWER > 0U))
    hal_uart_state_t *uartHandle;
    assert(handle);

    uartHandle = (hal_uart_state_t *)handle;
#if (defined(HAL_UART_ADAPTER_LOWPOWER_RESTORE) && (HAL_UART_ADAPTER_LOWPOWER_RESTORE > 0U))
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable lpuart clock */
    CLOCK_EnableClock(s_LpuartAdapterClock[uartHandle->instance]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
    s_LpuartAdapterBase[uartHandle->instance]->BAUD  = uartHandle->reg_BAUD;
    s_LpuartAdapterBase[uartHandle->instance]->WATER = uartHandle->reg_WATER;
    s_LpuartAdapterBase[uartHandle->instance]->MODIR = uartHandle->reg_MODIR;
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
    /* Enable tx/rx FIFO */
    s_LpuartAdapterBase[uartHandle->instance]->FIFO |= (LPUART_FIFO_TXFE_MASK | LPUART_FIFO_RXFE_MASK);
    /* Flush FIFO */
    s_LpuartAdapterBase[uartHandle->instance]->FIFO |= (LPUART_FIFO_TXFLUSH_MASK | LPUART_FIFO_RXFLUSH_MASK);
#endif /* FSL_FEATURE_LPUART_HAS_FIFO */
    s_LpuartAdapterBase[uartHandle->instance]->CTRL = uartHandle->reg_CTRL;
#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))
/* Enable interrupt in NVIC. */
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
    NVIC_SetPriority((IRQn_Type)s_LpuartRxIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
    EnableIRQ(s_LpuartRxIRQ[uartHandle->instance]);
    NVIC_SetPriority((IRQn_Type)s_LpuartTxIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
    EnableIRQ(s_LpuartTxIRQ[uartHandle->instance]);
#else
    NVIC_SetPriority((IRQn_Type)s_LpuartIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
    (void)EnableIRQ(s_LpuartIRQ[uartHandle->instance]);
#endif
    if (uartHandle->mode == kHAL_UartNonBlockMode)
    {
        s_LpuartAdapterBase[uartHandle->instance]->CTRL |= LPUART_CTRL_RIE_MASK;
        HAL_UartIsrFunction(uartHandle);
    }

#endif
#else
    HAL_UartInit(handle, &uartHandle->config);
#endif
#endif
    return kStatus_HAL_UartSuccess;
}

#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))

#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))

hal_uart_status_t HAL_UartTransferInstallCallback(hal_uart_handle_t handle,
                                                  hal_uart_transfer_callback_t callback,
                                                  void *callbackParam)
{
    hal_uart_state_t *uartHandle;

    assert(handle);
    assert(0U != HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

    assert(uartHandle->mode != kHAL_UartBlockMode);

    uartHandle->callbackParam = callbackParam;
    uartHandle->callback      = callback;

    return kStatus_HAL_UartSuccess;
}

hal_uart_status_t HAL_UartTransferReceiveNonBlocking(hal_uart_handle_t handle, hal_uart_transfer_t *transfer)
{
    hal_uart_state_t *uartHandle;
    status_t status;
    assert(handle);
    assert(transfer);
    assert(0U != HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;
    assert(uartHandle->mode != kHAL_UartBlockMode);
    status = LPUART_TransferReceiveNonBlocking(s_LpuartAdapterBase[uartHandle->instance], &uartHandle->hardwareHandle,
                                               (lpuart_transfer_t *)(void *)transfer, NULL);

    return HAL_UartGetStatus(status);
}

hal_uart_status_t HAL_UartTransferSendNonBlocking(hal_uart_handle_t handle, hal_uart_transfer_t *transfer)
{
    hal_uart_state_t *uartHandle;
    status_t status;
    assert(handle);
    assert(transfer);
    assert(0U != HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;
    assert(uartHandle->mode != kHAL_UartBlockMode);
    status = LPUART_TransferSendNonBlocking(s_LpuartAdapterBase[uartHandle->instance], &uartHandle->hardwareHandle,
                                            (lpuart_transfer_t *)(void *)transfer);

    return HAL_UartGetStatus(status);
}

hal_uart_status_t HAL_UartTransferGetReceiveCount(hal_uart_handle_t handle, uint32_t *count)
{
    hal_uart_state_t *uartHandle;
    status_t status;
    assert(handle);
    assert(count);
    assert(0U != HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;
    assert(uartHandle->mode != kHAL_UartBlockMode);
    status =
        LPUART_TransferGetReceiveCount(s_LpuartAdapterBase[uartHandle->instance], &uartHandle->hardwareHandle, count);

    return HAL_UartGetStatus(status);
}

hal_uart_status_t HAL_UartTransferGetSendCount(hal_uart_handle_t handle, uint32_t *count)
{
    hal_uart_state_t *uartHandle;
    status_t status;
    assert(handle);
    assert(count);
    assert(0U != HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;
    assert(uartHandle->mode != kHAL_UartBlockMode);
    status = LPUART_TransferGetSendCount(s_LpuartAdapterBase[uartHandle->instance], &uartHandle->hardwareHandle, count);

    return HAL_UartGetStatus(status);
}

hal_uart_status_t HAL_UartTransferAbortReceive(hal_uart_handle_t handle)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(0U != HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;
    assert(uartHandle->mode != kHAL_UartBlockMode);
    LPUART_TransferAbortReceive(s_LpuartAdapterBase[uartHandle->instance], &uartHandle->hardwareHandle);

    return kStatus_HAL_UartSuccess;
}

hal_uart_status_t HAL_UartTransferAbortSend(hal_uart_handle_t handle)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(0U != HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;
    assert(uartHandle->mode != kHAL_UartBlockMode);
    LPUART_TransferAbortSend(s_LpuartAdapterBase[uartHandle->instance], &uartHandle->hardwareHandle);

    return kStatus_HAL_UartSuccess;
}

#else

/* None transactional API with non-blocking mode. */
hal_uart_status_t HAL_UartInstallCallback(hal_uart_handle_t handle,
                                          hal_uart_transfer_callback_t callback,
                                          void *callbackParam)
{
    hal_uart_state_t *uartHandle;

    assert(handle);
    assert(0U == HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;
    assert(uartHandle->mode != kHAL_UartBlockMode);
    uartHandle->callbackParam = callbackParam;
    uartHandle->callback = callback;

    return kStatus_HAL_UartSuccess;
}

hal_uart_status_t HAL_UartReceiveNonBlocking(hal_uart_handle_t handle, uint8_t *data, size_t length)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(data);
    assert(length);
    assert(0U == HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;
    assert(uartHandle->mode != kHAL_UartBlockMode);
    if (NULL != uartHandle->rx.buffer)
    {
        return kStatus_HAL_UartRxBusy;
    }

    uartHandle->rx.bufferLength = length;
    uartHandle->rx.bufferSofar = 0;
    uartHandle->rx.buffer = data;
    LPUART_EnableInterrupts(s_LpuartAdapterBase[uartHandle->instance], (uint32_t)kLPUART_RxDataRegFullInterruptEnable |
                                                                           (uint32_t)kLPUART_RxOverrunInterruptEnable
#if (defined(HAL_UART_ADAPTER_FIFO) && (HAL_UART_ADAPTER_FIFO > 0u))
                                                                           | (uint32_t)kLPUART_IdleLineInterruptEnable
#endif
    );
    return kStatus_HAL_UartSuccess;
}

hal_uart_status_t HAL_UartSendNonBlocking(hal_uart_handle_t handle, uint8_t *data, size_t length)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(data);
    assert(length);
    assert(0U == HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;
    assert(uartHandle->mode != kHAL_UartBlockMode);
    if (NULL != uartHandle->tx.buffer)
    {
        return kStatus_HAL_UartTxBusy;
    }
    uartHandle->tx.bufferLength = length;
    uartHandle->tx.bufferSofar = 0;
    uartHandle->tx.buffer = data;
    LPUART_EnableInterrupts(s_LpuartAdapterBase[uartHandle->instance], (uint32_t)kLPUART_TxDataRegEmptyInterruptEnable);
    return kStatus_HAL_UartSuccess;
}

hal_uart_status_t HAL_UartGetReceiveCount(hal_uart_handle_t handle, uint32_t *reCount)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(reCount);
    assert(0U == HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;
    assert(uartHandle->mode != kHAL_UartBlockMode);
    if (NULL != uartHandle->rx.buffer)
    {
        *reCount = uartHandle->rx.bufferSofar;
        return kStatus_HAL_UartSuccess;
    }
    return kStatus_HAL_UartError;
}

hal_uart_status_t HAL_UartGetSendCount(hal_uart_handle_t handle, uint32_t *seCount)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(seCount);
    assert(0U == HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;
    assert(uartHandle->mode != kHAL_UartBlockMode);
    if (NULL != uartHandle->tx.buffer)
    {
        *seCount = uartHandle->tx.bufferSofar;
        return kStatus_HAL_UartSuccess;
    }
    return kStatus_HAL_UartError;
}

hal_uart_status_t HAL_UartAbortReceive(hal_uart_handle_t handle)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(0U == HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;
    assert(uartHandle->mode != kHAL_UartBlockMode);
    if (NULL != uartHandle->rx.buffer)
    {
        LPUART_DisableInterrupts(
            s_LpuartAdapterBase[uartHandle->instance],
            (uint32_t)kLPUART_RxDataRegFullInterruptEnable | (uint32_t)kLPUART_RxOverrunInterruptEnable);
        uartHandle->rx.buffer = NULL;
    }

    return kStatus_HAL_UartSuccess;
}

hal_uart_status_t HAL_UartAbortSend(hal_uart_handle_t handle)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(0U == HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;
    assert(uartHandle->mode != kHAL_UartBlockMode);
    if (NULL != uartHandle->tx.buffer)
    {
        LPUART_DisableInterrupts(s_LpuartAdapterBase[uartHandle->instance],
                                 (uint32_t)kLPUART_TxDataRegEmptyInterruptEnable);
        uartHandle->tx.buffer = NULL;
    }

    return kStatus_HAL_UartSuccess;
}

#endif

#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))

void HAL_UartIsrFunction(hal_uart_handle_t handle)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(0U != HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

#if 0
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
    DisableIRQ(s_LpuartRxIRQ[uartHandle->instance]);
    DisableIRQ(s_LpuartTxIRQ[uartHandle->instance]);
#else
    DisableIRQ(s_LpuartIRQ[uartHandle->instance]);
#endif
#endif
    LPUART_TransferHandleIRQ(s_LpuartAdapterBase[uartHandle->instance], &uartHandle->hardwareHandle);
#if 0
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
    NVIC_SetPriority((IRQn_Type)s_LpuartRxIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
    EnableIRQ(s_LpuartRxIRQ[uartHandle->instance]);
    NVIC_SetPriority((IRQn_Type)s_LpuartTxIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
    EnableIRQ(s_LpuartTxIRQ[uartHandle->instance]);
#else
    NVIC_SetPriority((IRQn_Type)s_LpuartIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
    EnableIRQ(s_LpuartIRQ[uartHandle->instance]);
#endif
#endif
}

#else

void HAL_UartIsrFunction(hal_uart_handle_t handle)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(0U == HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

#if 0
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
    DisableIRQ(s_LpuartRxIRQ[uartHandle->instance]);
    DisableIRQ(s_LpuartTxIRQ[uartHandle->instance]);
#else
    DisableIRQ(s_LpuartIRQ[uartHandle->instance]);
#endif
#endif
    HAL_UartInterruptHandle(uartHandle->instance);
#if 0
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
    NVIC_SetPriority((IRQn_Type)s_LpuartRxIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
    EnableIRQ(s_LpuartRxIRQ[uartHandle->instance]);
    NVIC_SetPriority((IRQn_Type)s_LpuartTxIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
    EnableIRQ(s_LpuartTxIRQ[uartHandle->instance]);
#else
    NVIC_SetPriority((IRQn_Type)s_LpuartIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
    EnableIRQ(s_LpuartIRQ[uartHandle->instance]);
#endif
#endif
}

#if defined(FSL_FEATURE_LPUART_HAS_SHARED_IRQ0_IRQ1) && FSL_FEATURE_LPUART_HAS_SHARED_IRQ0_IRQ1
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART0_LPUART1_RX_IRQHandler(void)
{
    if ((s_UartState[0]))
    {
        if ((LPUART_STAT_OR_MASK & LPUART0->STAT) ||
            ((LPUART_STAT_RDRF_MASK & LPUART0->STAT) && (LPUART_CTRL_RIE_MASK & LPUART0->CTRL)))
        {
            HAL_UartInterruptHandle(0);
        }
    }
    if ((s_UartState[1]))
    {
        if ((LPUART_STAT_OR_MASK & LPUART1->STAT) ||
            ((LPUART_STAT_RDRF_MASK & LPUART1->STAT) && (LPUART_CTRL_RIE_MASK & LPUART1->CTRL)))
        {
            HAL_UartInterruptHandle(1);
        }
    }
    SDK_ISR_EXIT_BARRIER;
}
void LPUART0_LPUART1_TX_IRQHandler(void)
{
    if ((s_UartState[0]))
    {
        if ((LPUART_STAT_OR_MASK & LPUART0->STAT) ||
            ((LPUART0->STAT & LPUART_STAT_TDRE_MASK) && (LPUART0->CTRL & LPUART_CTRL_TIE_MASK)))
        {
            HAL_UartInterruptHandle(0);
        }
    }
    if ((s_UartState[1]))
    {
        if ((LPUART_STAT_OR_MASK & LPUART1->STAT) ||
            ((LPUART1->STAT & LPUART_STAT_TDRE_MASK) && (LPUART1->CTRL & LPUART_CTRL_TIE_MASK)))
        {
            HAL_UartInterruptHandle(1);
        }
    }
    SDK_ISR_EXIT_BARRIER;
}
#else
void LPUART0_LPUART1_IRQHandler(void);
void LPUART0_LPUART1_IRQHandler(void)
{
    uint32_t orMask;
    uint32_t rdrfMask;
    uint32_t rieMask;
    uint32_t tdreMask;
    uint32_t tieMask;
    if (NULL != (s_UartState[0]))
    {
        orMask = LPUART_STAT_OR_MASK & LPUART0->STAT;
        rdrfMask = LPUART_STAT_RDRF_MASK & LPUART0->STAT;
        rieMask = LPUART_CTRL_RIE_MASK & LPUART0->CTRL;
        tdreMask = LPUART0->STAT & LPUART_STAT_TDRE_MASK;
        tieMask = LPUART0->CTRL & LPUART_CTRL_TIE_MASK;
        if ((bool)orMask || ((bool)rdrfMask && (bool)rieMask) || ((bool)tdreMask && (bool)tieMask))
        {
            HAL_UartInterruptHandle(0);
        }
    }
    if (NULL != (s_UartState[1]))
    {
        orMask = LPUART_STAT_OR_MASK & LPUART1->STAT;
        rdrfMask = LPUART_STAT_RDRF_MASK & LPUART1->STAT;
        rieMask = LPUART_CTRL_RIE_MASK & LPUART1->CTRL;
        tdreMask = LPUART1->STAT & LPUART_STAT_TDRE_MASK;
        tieMask = LPUART1->CTRL & LPUART_CTRL_TIE_MASK;
        if ((bool)orMask || ((bool)rdrfMask && (bool)rieMask) || ((bool)tdreMask && (bool)tieMask))
        {
            HAL_UartInterruptHandle(1);
        }
    }
    SDK_ISR_EXIT_BARRIER;
}
#endif
#endif

#if defined(LPUART0)
#if !(defined(FSL_FEATURE_LPUART_HAS_SHARED_IRQ0_IRQ1) && FSL_FEATURE_LPUART_HAS_SHARED_IRQ0_IRQ1)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART0_TX_IRQHandler(void)
{
    HAL_UartInterruptHandle(0);
    SDK_ISR_EXIT_BARRIER;
}
void LPUART0_RX_IRQHandler(void)
{
    HAL_UartInterruptHandle(0);
    SDK_ISR_EXIT_BARRIER;
}
#else
void LPUART0_IRQHandler(void);
void LPUART0_IRQHandler(void)
{
    HAL_UartInterruptHandle(0);
    SDK_ISR_EXIT_BARRIER;
}
#endif
#endif
#endif

#if defined(LPUART1)
#if !(defined(FSL_FEATURE_LPUART_HAS_SHARED_IRQ0_IRQ1) && FSL_FEATURE_LPUART_HAS_SHARED_IRQ0_IRQ1)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART1_TX_IRQHandler(void)
{
    HAL_UartInterruptHandle(1);
    SDK_ISR_EXIT_BARRIER;
}
void LPUART1_RX_IRQHandler(void)
{
    HAL_UartInterruptHandle(1);
    SDK_ISR_EXIT_BARRIER;
}
#else
void LPUART1_IRQHandler(void);
void LPUART1_IRQHandler(void)
{
    HAL_UartInterruptHandle(1);
    SDK_ISR_EXIT_BARRIER;
}
#endif
#endif
#endif

#if defined(LPUART2)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART2_TX_IRQHandler(void)
{
    HAL_UartInterruptHandle(2);
    SDK_ISR_EXIT_BARRIER;
}
void LPUART2_RX_IRQHandler(void)
{
    HAL_UartInterruptHandle(2);
    SDK_ISR_EXIT_BARRIER;
}
#else
void LPUART2_IRQHandler(void);
void LPUART2_IRQHandler(void)
{
    HAL_UartInterruptHandle(2);
    SDK_ISR_EXIT_BARRIER;
}
#endif
#endif

#if defined(LPUART3)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART3_TX_IRQHandler(void)
{
    HAL_UartInterruptHandle(3);
    SDK_ISR_EXIT_BARRIER;
}
void LPUART3_RX_IRQHandler(void)
{
    HAL_UartInterruptHandle(3);
    SDK_ISR_EXIT_BARRIER;
}
#else
void LPUART3_IRQHandler(void);
void LPUART3_IRQHandler(void)
{
    HAL_UartInterruptHandle(3);
    SDK_ISR_EXIT_BARRIER;
}
#endif
#endif

#if defined(LPUART4)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART4_TX_IRQHandler(void)
{
    HAL_UartInterruptHandle(4);
    SDK_ISR_EXIT_BARRIER;
}
void LPUART4_RX_IRQHandler(void)
{
    HAL_UartInterruptHandle(4);
    SDK_ISR_EXIT_BARRIER;
}
#else
void LPUART4_IRQHandler(void);
void LPUART4_IRQHandler(void)
{
    HAL_UartInterruptHandle(4);
    SDK_ISR_EXIT_BARRIER;
}
#endif
#endif

#if defined(LPUART5)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART5_TX_IRQHandler(void)
{
    HAL_UartInterruptHandle(5);
    SDK_ISR_EXIT_BARRIER;
}
void LPUART5_RX_IRQHandler(void)
{
    HAL_UartInterruptHandle(5);
    SDK_ISR_EXIT_BARRIER;
}
#else
void LPUART5_IRQHandler(void);
void LPUART5_IRQHandler(void)
{
    HAL_UartInterruptHandle(5);
    SDK_ISR_EXIT_BARRIER;
}
#endif
#endif

#if defined(LPUART6)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART6_TX_IRQHandler(void)
{
    HAL_UartInterruptHandle(6);
    SDK_ISR_EXIT_BARRIER;
}
void LPUART6_RX_IRQHandler(void)
{
    HAL_UartInterruptHandle(6);
    SDK_ISR_EXIT_BARRIER;
}
#else
void LPUART6_IRQHandler(void);
void LPUART6_IRQHandler(void)
{
    HAL_UartInterruptHandle(6);
    SDK_ISR_EXIT_BARRIER;
}
#endif
#endif

#if defined(LPUART7)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART7_TX_IRQHandler(void)
{
    HAL_UartInterruptHandle(7);
    SDK_ISR_EXIT_BARRIER;
}
void LPUART7_RX_IRQHandler(void)
{
    HAL_UartInterruptHandle(7);
    SDK_ISR_EXIT_BARRIER;
}
#else
void LPUART7_IRQHandler(void);
void LPUART7_IRQHandler(void)
{
    HAL_UartInterruptHandle(7);
    SDK_ISR_EXIT_BARRIER;
}
#endif
#endif

#if defined(LPUART8)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART8_TX_IRQHandler(void)
{
    HAL_UartInterruptHandle(8);
    SDK_ISR_EXIT_BARRIER;
}
void LPUART8_RX_IRQHandler(void)
{
    HAL_UartInterruptHandle(8);
    SDK_ISR_EXIT_BARRIER;
}
#else
void LPUART8_IRQHandler(void);
void LPUART8_IRQHandler(void)
{
    HAL_UartInterruptHandle(8);
    SDK_ISR_EXIT_BARRIER;
}
#endif
#endif

#if defined(CM4_0__LPUART)
void M4_0_LPUART_IRQHandler(void);
void M4_0_LPUART_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(CM4_0__LPUART));
    SDK_ISR_EXIT_BARRIER;
}
#endif

#if defined(CM4_1__LPUART)
void M4_1_LPUART_IRQHandler(void);
void M4_1_LPUART_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(CM4_1__LPUART));
    SDK_ISR_EXIT_BARRIER;
}
#endif

#if defined(CM4__LPUART)
void M4_LPUART_IRQHandler(void);
void M4_LPUART_IRQHandler(void)
{
    HAL_UartInterruptHandle((uint8_t)LPUART_GetInstance(CM4__LPUART));
    SDK_ISR_EXIT_BARRIER;
}
#endif

#if defined(DMA__LPUART0)
void DMA_UART0_INT_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(DMA__LPUART0));
    SDK_ISR_EXIT_BARRIER;
}
#endif

#if defined(DMA__LPUART1)
void DMA_UART1_INT_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(DMA__LPUART1));
    SDK_ISR_EXIT_BARRIER;
}
#endif

#if defined(DMA__LPUART2)
void DMA_UART2_INT_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(DMA__LPUART2));
    SDK_ISR_EXIT_BARRIER;
}
#endif

#if defined(DMA__LPUART3)
void DMA_UART3_INT_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(DMA__LPUART3));
    SDK_ISR_EXIT_BARRIER;
}
#endif

#if defined(DMA__LPUART4)
void DMA_UART4_INT_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(DMA__LPUART4));
    SDK_ISR_EXIT_BARRIER;
}
#endif

#if defined(ADMA__LPUART0)
void ADMA_UART0_INT_IRQHandler(void);
void ADMA_UART0_INT_IRQHandler(void)
{
    HAL_UartInterruptHandle((uint8_t)LPUART_GetInstance(ADMA__LPUART0));
    SDK_ISR_EXIT_BARRIER;
}
#endif

#if defined(ADMA__LPUART1)
void ADMA_UART1_INT_IRQHandler(void);
void ADMA_UART1_INT_IRQHandler(void)
{
    HAL_UartInterruptHandle((uint8_t)LPUART_GetInstance(ADMA__LPUART1));
    SDK_ISR_EXIT_BARRIER;
}
#endif

#if defined(ADMA__LPUART2)
void ADMA_UART2_INT_IRQHandler(void);
void ADMA_UART2_INT_IRQHandler(void)
{
    HAL_UartInterruptHandle((uint8_t)LPUART_GetInstance(ADMA__LPUART2));
    SDK_ISR_EXIT_BARRIER;
}
#endif

#if defined(ADMA__LPUART3)
void ADMA_UART3_INT_IRQHandler(void);
void ADMA_UART3_INT_IRQHandler(void)
{
    HAL_UartInterruptHandle((uint8_t)LPUART_GetInstance(ADMA__LPUART3));
    SDK_ISR_EXIT_BARRIER;
}
#endif

#endif

#endif
