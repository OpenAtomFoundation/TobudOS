/*
 * The Clear BSD License
 * Copyright (c) 2017, NXP Semiconductors, Inc.
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_usart.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.lpc_miniusart"
#endif

enum _usart_transfer_states
{
    kUSART_TxIdle, /* TX idle. */
    kUSART_TxBusy, /* TX busy. */
    kUSART_RxIdle, /* RX idle. */
    kUSART_RxBusy  /* RX busy. */
};

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Array of USART handle. */
static usart_handle_t *s_usartHandle[FSL_FEATURE_SOC_USART_COUNT];

/*! @brief IRQ name array */
static const IRQn_Type s_usartIRQ[] = USART_IRQS;

/*! @brief Array to map USART instance number to base address. */
static const uint32_t s_usartBaseAddrs[FSL_FEATURE_SOC_USART_COUNT] = USART_BASE_ADDRS;

/* @brief Array to map USART instance number to CLOCK names */
static const clock_ip_name_t s_usartClock[] = USART_CLOCKS;

/* @brief Array to map USART instance number to RESET names */
static const SYSCON_RSTn_t s_usartRest[] = UART_RSTS_N;

/* Typedef for interrupt handler. */
typedef void (*usart_isr_t)(USART_Type *base, usart_handle_t *handle);

/* USART ISR for transactional APIs. */
static usart_isr_t s_usartIsr;

/*******************************************************************************
 * Code
 ******************************************************************************/

/* Get the index corresponding to the USART */
uint32_t USART_GetInstance(USART_Type *base)
{
    int i;

    for (i = 0; i < FSL_FEATURE_SOC_USART_COUNT; i++)
    {
        if ((uint32_t)base == s_usartBaseAddrs[i])
        {
            return i;
        }
    }

    assert(false);
    return 0U;
}

size_t USART_TransferGetRxRingBufferLength(usart_handle_t *handle)
{
    size_t size = 0U;

    /* Check arguments */
    assert(NULL != handle);

    if (handle->rxRingBufferTail > handle->rxRingBufferHead)
    {
        size = (size_t)(handle->rxRingBufferHead + handle->rxRingBufferSize - handle->rxRingBufferTail);
    }
    else
    {
        size = (size_t)(handle->rxRingBufferHead - handle->rxRingBufferTail);
    }
    return size;
}

static bool USART_TransferIsRxRingBufferFull(usart_handle_t *handle)
{
    bool full = 0U;

    /* Check arguments */
    assert(NULL != handle);

    if (USART_TransferGetRxRingBufferLength(handle) == (handle->rxRingBufferSize - 1U))
    {
        full = true;
    }
    else
    {
        full = false;
    }
    return full;
}

void USART_TransferStartRingBuffer(USART_Type *base, usart_handle_t *handle, uint8_t *ringBuffer, size_t ringBufferSize)
{
    /* Check arguments */
    assert(NULL != base);
    assert(NULL != handle);
    assert(NULL != ringBuffer);

    /* Setup the ringbuffer address */
    handle->rxRingBuffer = ringBuffer;
    handle->rxRingBufferSize = ringBufferSize;
    handle->rxRingBufferHead = 0U;
    handle->rxRingBufferTail = 0U;

    /* Start receive data read interrupt and reveive overrun interrupt. */
    USART_EnableInterrupts(base, kUSART_RxReadyInterruptEnable | kUSART_HardwareOverRunInterruptEnable);
}

void USART_TransferStopRingBuffer(USART_Type *base, usart_handle_t *handle)
{
    /* Check arguments */
    assert(NULL != base);
    assert(NULL != handle);

    /* If USART is in idle state, dsiable the interrupts for ring buffer. */
    if (handle->rxState == kUSART_RxIdle)
    {
        USART_DisableInterrupts(base, kUSART_RxReadyInterruptEnable | kUSART_HardwareOverRunInterruptEnable);
    }
    handle->rxRingBuffer = NULL;
    handle->rxRingBufferSize = 0U;
    handle->rxRingBufferHead = 0U;
    handle->rxRingBufferTail = 0U;
}

status_t USART_Init(USART_Type *base, const usart_config_t *config, uint32_t srcClock_Hz)
{
    int result = 0U;

    /* Check arguments */
    assert(!((NULL == base) || (NULL == config) || (0 == srcClock_Hz)));

    uint32_t instance = USART_GetInstance(base);
    /* Enable usart clock. */
    CLOCK_EnableClock(s_usartClock[instance]);
    /* Reset USART for transfer. */
    RESET_PeripheralReset(s_usartRest[instance]);

    /* Setup configuration and enable USART to configure other register. */
    base->CFG = USART_CFG_PARITYSEL(config->parityMode) | USART_CFG_STOPLEN(config->stopBitCount) |
                USART_CFG_SYNCEN(config->syncMode >> 1) | USART_CFG_DATALEN(config->bitCountPerChar) |
                USART_CFG_LOOP(config->loopback) | USART_CFG_SYNCMST(config->syncMode) | USART_CFG_ENABLE_MASK;

    if (0U != config->baudRate_Bps)
    {
        /* Setup baudrate */
        result = USART_SetBaudRate(base, config->baudRate_Bps, srcClock_Hz);
    }

    if (kStatus_Success != result)
    {
        return result;
    }

    /* Setup the USART transmit and receive. */
    USART_EnableTx(base, config->enableTx);
    USART_EnableRx(base, config->enableRx);

    return kStatus_Success;
}

void USART_Deinit(USART_Type *base)
{
    /* Check arguments */
    assert(NULL != base);
    /* Wait for data transmit complete. */
    while (!(base->STAT & USART_STAT_TXIDLE_MASK))
    {
    }
    /* Disable the USART module. */
    base->CFG &= ~(USART_CFG_ENABLE_MASK);
    /* Disbable the clock for usart */
    CLOCK_DisableClock(s_usartClock[USART_GetInstance(base)]);
}

void USART_GetDefaultConfig(usart_config_t *config)
{
    /* Check arguments */
    assert(NULL != config);

    /* Set always all members ! */
    config->baudRate_Bps = 9600U;
    config->parityMode = kUSART_ParityDisabled;
    config->stopBitCount = kUSART_OneStopBit;
    config->bitCountPerChar = kUSART_8BitsPerChar;
    config->loopback = false;
    config->enableRx = false;
    config->enableTx = false;
    config->syncMode = kUSART_SyncModeDisabled;
}

status_t USART_SetBaudRate(USART_Type *base, uint32_t baudrate_Bps, uint32_t srcClock_Hz)
{
    /* check arguments */
    assert(!((NULL == base) || (0 == baudrate_Bps) || (0 == srcClock_Hz)));

#if defined(FSL_FEATURE_USART_HAS_OSR_REGISTER) && (FSL_FEATURE_USART_HAS_OSR_REGISTER)
    uint32_t best_diff = (uint32_t)-1, best_osrval = 0xf, best_brgval = (uint32_t)-1;
    uint32_t diff = 0U, brgval = 0U, osrval = 0U, baudrate = 0U;

    /* If synchronous is enable, only BRG register is useful. */
    if (base->CFG & USART_CFG_SYNCEN_MASK)
    {
        brgval = srcClock_Hz / baudrate_Bps;
        base->BRG = brgval - 1;
    }
    else
    {
        for (osrval = best_osrval; osrval >= 8; osrval--)
        {
            brgval = (srcClock_Hz / ((osrval + 1) * baudrate_Bps)) - 1;
            if (brgval > 0xFFFF)
            {
                continue;
            }
            baudrate = srcClock_Hz / ((osrval + 1) * (brgval + 1));
            diff = baudrate_Bps < baudrate ? baudrate - baudrate_Bps : baudrate_Bps - baudrate;
            if (diff < best_diff)
            {
                best_diff = diff;
                best_osrval = osrval;
                best_brgval = brgval;
            }
        }

        /* value over range */
        if (best_brgval > 0xFFFF)
        {
            return kStatus_USART_BaudrateNotSupport;
        }

        base->OSR = best_osrval;
        base->BRG = best_brgval;
    }
#else
    uint32_t brgval = 0U;
    /* If synchronous is enable. */
    if (base->CFG & USART_CFG_SYNCEN_MASK)
    {
        brgval = srcClock_Hz / baudrate_Bps;
        base->BRG = brgval - 1;
    }
    else
    {
        /* In asynchronous mode, The baud rate divider divides the common USART
         * peripheral clock by a factor of 16 multiplied by the baud rate value
         * to provide the baud rate.
         */
        brgval = srcClock_Hz / (baudrate_Bps * 16);
        base->BRG = brgval - 1;
    }
#endif /* FSL_FEATURE_USART_HAS_OSR_REGISTER */

    return kStatus_Success;
}

void USART_WriteBlocking(USART_Type *base, const uint8_t *data, size_t length)
{
    /* Check arguments */
    assert(!((NULL == base) || (NULL == data)));

    for (; length > 0; length--)
    {
        /* Wait for TX is ready to transmit new data. */
        while (!(base->STAT & USART_STAT_TXRDY_MASK))
        {
        }
        base->TXDAT = *data;
        data++;
    }
    /* Wait to finish transfer */
    while (!(base->STAT & USART_STAT_TXIDLE_MASK))
    {
    }
}

status_t USART_ReadBlocking(USART_Type *base, uint8_t *data, size_t length)
{
    uint32_t status;
    /* Check arguments */
    assert(!((NULL == base) || (NULL == data)));

    for (; length > 0; length--)
    {
        /* loop until receive is ready to read */
        while (!(base->STAT & USART_STAT_RXRDY_MASK))
        {
        }

        *data = base->RXDAT;
        data++;

        /* Check receive status */
        status = base->STAT;

        if (status & USART_STAT_FRAMERRINT_MASK)
        {
            base->STAT |= USART_STAT_FRAMERRINT_MASK;
            return kStatus_USART_FramingError;
        }
        if (status & USART_STAT_PARITYERRINT_MASK)
        {
            base->STAT |= USART_STAT_PARITYERRINT_MASK;
            return kStatus_USART_ParityError;
        }
        if (status & USART_STAT_RXNOISEINT_MASK)
        {
            base->STAT |= USART_STAT_RXNOISEINT_MASK;
            return kStatus_USART_NoiseError;
        }
        if (base->STAT & USART_STAT_OVERRUNINT_MASK)
        {
            base->STAT |= USART_STAT_OVERRUNINT_MASK;
            return kStatus_USART_HardwareOverrun;
        }
    }
    return kStatus_Success;
}

status_t USART_TransferCreateHandle(USART_Type *base,
                                    usart_handle_t *handle,
                                    usart_transfer_callback_t callback,
                                    void *userData)
{
    int32_t instance = 0;

    /* Check 'base' */
    assert(!((NULL == base) || (NULL == handle)));

    instance = USART_GetInstance(base);

    memset(handle, 0, sizeof(*handle));
    /* Set the TX/RX state. */
    handle->rxState = kUSART_RxIdle;
    handle->txState = kUSART_TxIdle;
    /* Set the callback and user data. */
    handle->callback = callback;
    handle->userData = userData;
    /* Store the handle data to global variables. */
    s_usartHandle[instance] = handle;
    /* Mapping the interrupt function. */
    s_usartIsr = USART_TransferHandleIRQ;
    /* Enable interrupt in NVIC. */
    EnableIRQ(s_usartIRQ[instance]);

    return kStatus_Success;
}

status_t USART_TransferSendNonBlocking(USART_Type *base, usart_handle_t *handle, usart_transfer_t *xfer)
{
    /* Check arguments */
    assert(!((NULL == base) || (NULL == handle) || (NULL == xfer)));
    /* Check xfer members */
    assert(!((0 == xfer->dataSize) || (NULL == xfer->data)));

    /* Return error if current TX busy. */
    if (kUSART_TxBusy == handle->txState)
    {
        return kStatus_USART_TxBusy;
    }
    else
    {
        handle->txData = xfer->data;
        handle->txDataSize = xfer->dataSize;
        handle->txDataSizeAll = xfer->dataSize;
        handle->txState = kUSART_TxBusy;

        USART_EnableInterrupts(base, kUSART_TxReadyInterruptEnable);
        /* Clear transmit disable bit. */
        base->CTL &= ~USART_CTL_TXDIS_MASK;
    }
    return kStatus_Success;
}

void USART_TransferAbortSend(USART_Type *base, usart_handle_t *handle)
{
    assert(NULL != handle);

    USART_DisableInterrupts(base, kUSART_TxReadyInterruptEnable);
    /* Disable transmit after data being transmitted. */
    USART_EnableTx(base, false);
    handle->txDataSize = 0;
    handle->txState = kUSART_TxIdle;
}

status_t USART_TransferGetSendCount(USART_Type *base, usart_handle_t *handle, uint32_t *count)
{
    assert(NULL != handle);
    assert(NULL != count);

    if (kUSART_TxIdle == handle->txState)
    {
        return kStatus_NoTransferInProgress;
    }

    *count = handle->txDataSizeAll - handle->txDataSize;

    return kStatus_Success;
}

status_t USART_TransferReceiveNonBlocking(USART_Type *base,
                                          usart_handle_t *handle,
                                          usart_transfer_t *xfer,
                                          size_t *receivedBytes)
{
    uint32_t i;
    /* How many bytes to copy from ring buffer to user memory. */
    size_t bytesToCopy = 0U;
    /* How many bytes to receive. */
    size_t bytesToReceive;
    /* How many bytes currently have received. */
    size_t bytesCurrentReceived;

    /* Check arguments */
    assert(!((NULL == base) || (NULL == handle) || (NULL == xfer)));
    /* Check xfer members */
    assert(!((0 == xfer->dataSize) || (NULL == xfer->data)));

    /* How to get data:
       1. If RX ring buffer is not enabled, then save xfer->data and xfer->dataSize
          to uart handle, enable interrupt to store received data to xfer->data. When
          all data received, trigger callback.
       2. If RX ring buffer is enabled and not empty, get data from ring buffer first.
          If there are enough data in ring buffer, copy them to xfer->data and return.
          If there are not enough data in ring buffer, copy all of them to xfer->data,
          save the xfer->data remained empty space to uart handle, receive data
          to this empty space and trigger callback when finished. */
    if (kUSART_RxBusy == handle->rxState)
    {
        return kStatus_USART_RxBusy;
    }
    else
    {
        bytesToReceive = xfer->dataSize;
        bytesCurrentReceived = 0U;
        /* If RX ring buffer is used. */
        if (handle->rxRingBuffer)
        {
            USART_DisableInterrupts(base, kUSART_RxReadyInterruptEnable);
            /* How many bytes in RX ring buffer currently. */
            bytesToCopy = USART_TransferGetRxRingBufferLength(handle);
            if (bytesToCopy)
            {
                bytesToCopy = MIN(bytesToReceive, bytesToCopy);
                bytesToReceive -= bytesToCopy;
                /* Copy data from ring buffer to user memory. */
                for (i = 0U; i < bytesToCopy; i++)
                {
                    xfer->data[bytesCurrentReceived++] = handle->rxRingBuffer[handle->rxRingBufferTail];
                    /* Wrap to 0. Not use modulo (%) because it might be large and slow. */
                    if (handle->rxRingBufferTail + 1U == handle->rxRingBufferSize)
                    {
                        handle->rxRingBufferTail = 0U;
                    }
                    else
                    {
                        handle->rxRingBufferTail++;
                    }
                }
            }
            /* If ring buffer does not have enough data, still need to read more data. */
            if (bytesToReceive)
            {
                /* No data in ring buffer, save the request to UART handle. */
                handle->rxData = xfer->data + bytesCurrentReceived;
                handle->rxDataSize = bytesToReceive;
                handle->rxDataSizeAll = bytesToReceive;
                handle->rxState = kUSART_RxBusy;
            }

            USART_EnableInterrupts(base, kUSART_RxReadyInterruptEnable);
            /* Call user callback since all data are received. */
            if (0 == bytesToReceive)
            {
                if (handle->callback)
                {
                    handle->callback(base, handle, kStatus_USART_RxIdle, handle->userData);
                }
            }
        }
        /* Ring buffer not used. */
        else
        {
            handle->rxData = xfer->data + bytesCurrentReceived;
            handle->rxDataSize = bytesToReceive;
            handle->rxDataSizeAll = bytesToReceive;
            handle->rxState = kUSART_RxBusy;

            USART_EnableInterrupts(base, kUSART_RxReadyInterruptEnable | kUSART_HardwareOverRunInterruptEnable);
        }
        /* Return the how many bytes have read. */
        if (receivedBytes)
        {
            *receivedBytes = bytesCurrentReceived;
        }
    }
    return kStatus_Success;
}

void USART_TransferAbortReceive(USART_Type *base, usart_handle_t *handle)
{
    assert(NULL != handle);

    /* Only abort the receive to handle->rxData, the RX ring buffer is still working. */
    if (!handle->rxRingBuffer)
    {
        USART_DisableInterrupts(base, kUSART_RxReadyInterruptEnable | kUSART_HardwareOverRunInterruptEnable);
    }

    handle->rxDataSize = 0U;
    handle->rxState = kUSART_RxIdle;
}

status_t USART_TransferGetReceiveCount(USART_Type *base, usart_handle_t *handle, uint32_t *count)
{
    assert(NULL != handle);
    assert(NULL != count);

    if (kUSART_RxIdle == handle->rxState)
    {
        return kStatus_NoTransferInProgress;
    }

    *count = handle->rxDataSizeAll - handle->rxDataSize;

    return kStatus_Success;
}

void USART_TransferHandleIRQ(USART_Type *base, usart_handle_t *handle)
{
    /* Check arguments */
    assert((NULL != base) && (NULL != handle));

    bool receiveEnabled = (handle->rxDataSize) || (handle->rxRingBuffer);
    bool sendEnabled = handle->txDataSize;

    /* If RX overrun. */
    if (base->STAT & USART_STAT_OVERRUNINT_MASK)
    {
        /* Clear rx error state. */
        base->STAT |= USART_STAT_OVERRUNINT_MASK;
        /* Trigger callback. */
        if (handle->callback)
        {
            handle->callback(base, handle, kStatus_USART_RxError, handle->userData);
        }
    }

    /* Receive data */
    if ((receiveEnabled) && (base->STAT & USART_STAT_RXRDY_MASK))
    {
        /* Receive to app bufffer if app buffer is present */
        if (handle->rxDataSize)
        {
            *handle->rxData = base->RXDAT;
            handle->rxDataSize--;
            handle->rxData++;
            receiveEnabled = ((handle->rxDataSize != 0) || (handle->rxRingBuffer));

            if (!handle->rxDataSize)
            {
                if (!handle->rxRingBuffer)
                {
                    USART_DisableInterrupts(base,
                                            kUSART_RxReadyInterruptEnable | kUSART_HardwareOverRunInterruptEnable);
                }
                handle->rxState = kUSART_RxIdle;
                if (handle->callback)
                {
                    handle->callback(base, handle, kStatus_USART_RxIdle, handle->userData);
                }
            }
        }
        /* Otherwise receive to ring buffer if ring buffer is present */
        else
        {
            if (handle->rxRingBuffer)
            {
                if (USART_TransferIsRxRingBufferFull(handle))
                {
                    if (handle->callback)
                    {
                        handle->callback(base, handle, kStatus_USART_RxRingBufferOverrun, handle->userData);
                    }
                }
                /* If ring buffer is still full after callback function, the oldest data is overrided. */
                if (USART_TransferIsRxRingBufferFull(handle))
                {
                    /* Increase handle->rxRingBufferTail to make room for new data. */
                    if (handle->rxRingBufferTail + 1U == handle->rxRingBufferSize)
                    {
                        handle->rxRingBufferTail = 0U;
                    }
                    else
                    {
                        handle->rxRingBufferTail++;
                    }
                }

                handle->rxRingBuffer[handle->rxRingBufferHead] = base->RXDAT;

                /* Increase handle->rxRingBufferHead. */
                if (handle->rxRingBufferHead + 1U == handle->rxRingBufferSize)
                {
                    handle->rxRingBufferHead = 0U;
                }
                else
                {
                    handle->rxRingBufferHead++;
                }
            }
        }
    }
    /* Send data */
    if (sendEnabled && (base->STAT & USART_STAT_TXRDY_MASK))
    {
        base->TXDAT = *handle->txData;
        handle->txDataSize--;
        handle->txData++;

        if (0U == handle->txDataSize)
        {
            USART_DisableInterrupts(base, kUSART_TxReadyInterruptEnable);
            handle->txState = kUSART_TxIdle;
            if (handle->callback)
            {
                handle->callback(base, handle, kStatus_USART_TxIdle, handle->userData);
            }
        }
    }
}

#if defined(USART0)
void USART0_DriverIRQHandler(void)
{
    s_usartIsr(USART0, s_usartHandle[0]);
}
#endif

#if defined(USART1)
void USART1_DriverIRQHandler(void)
{
    s_usartIsr(USART1, s_usartHandle[1]);
}
#endif

#if defined(USART2)
void USART2_DriverIRQHandler(void)
{
    s_usartIsr(USART2, s_usartHandle[2]);
}
#endif

#if defined(USART3)
void PIN_INT6_USART3_DriverIRQHandler(void)
{
    s_usartIsr(USART3, s_usartHandle[3]);
}
#endif

#if defined(USART4)
void PIN_INT7_USART4_DriverIRQHandler(void)
{
    s_usartIsr(USART4, s_usartHandle[4]);
}
#endif
