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

#include "fsl_spi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.lpc_minispi"
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief SPI internal handle pointer array */
static spi_master_handle_t *s_spiHandle[FSL_FEATURE_SOC_SPI_COUNT];

/*! @brief Array to map SPI instance number to base address. */
static const uint32_t s_spiBaseAddrs[FSL_FEATURE_SOC_SPI_COUNT] = SPI_BASE_ADDRS;

/*! @brief IRQ name array */
static const IRQn_Type s_spiIRQ[] = SPI_IRQS;

/* @brief Array to map SPI instance number to CLOCK names */
static const clock_ip_name_t s_spiClock[] = SPI_CLOCKS;

/* @brief Array to map SPI reset. */
static const SYSCON_RSTn_t s_spiReset[] = SPI_RSTS_N;

/*! @brief Typedef for spi master interrupt handler. spi master and slave handle is the same. */
typedef void (*spi_isr_t)(SPI_Type *base, spi_master_handle_t *spiHandle);

/*! @brief Pointer to master IRQ handler for each instance. */
static spi_isr_t s_spiMasterIsr;
static spi_isr_t s_spiSlaveIsr;

/* @brief Dummy data for each instance. This data is used when user's tx buffer is NULL*/
volatile uint16_t s_dummyData[FSL_FEATURE_SOC_SPI_COUNT] = {0};

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Send a piece of data for SPI.
 *
 * This function will check if TXDAT register ready, and write the data into it.
 * At the same time, this function updates the values in master handle structure.
 *
 * @param base SPI base pointer
 * @param handle Pointer to SPI master handle structure.
 */
static void SPI_SendTransfer(SPI_Type *base, spi_master_handle_t *handle)
{
    uint32_t tmp32 = 0U;
    uint32_t instance = SPI_GetInstance(base);

    /* If transmit is ready, write data to TXDAT register. */
    if (kSPI_TxReadyFlag & SPI_GetStatusFlags(base))
    {
        if (handle->txData)
        {
            tmp32 = *(handle->txData++);
            handle->txRemainingBytes--;
            if (handle->dataWidth > 7U)
            {
                tmp32 |= ((uint32_t)(*(handle->txData++)) << 8U);
                handle->txRemainingBytes--;
            }
        }
        else
        {
            tmp32 = (uint32_t)s_dummyData[instance];
            handle->txRemainingBytes--;
            if (handle->dataWidth > 7U)
            {
                handle->txRemainingBytes--;
            }
        }
        /* If this transmit is the last to send, Set the control bits. */
        if (handle->txRemainingBytes == 0U)
        {
            base->TXCTL = handle->lastCommand;
        }

        base->TXDAT = tmp32;
    }
}

/*!
 * @brief Receive a piece of data for SPI.
 *
 * This function will check if RX register is ready, and write the data to destination address.
 * At the same time, this function updates the values in master handle structure.
 *
 * @param base SPI base pointer
 * @param handle Pointer to SPI master handle structure.
 */
static void SPI_ReceiveTransfer(SPI_Type *base, spi_master_handle_t *handle)
{
    uint32_t tmp32 = 0U;

    /* If receive is ready, read data from RXDAT register. */
    if (kSPI_RxReadyFlag & SPI_GetStatusFlags(base))
    {
        tmp32 = SPI_ReadData(base);
        /* Check If receive buffer is NULL. */
        if (handle->rxData)
        {
            *(handle->rxData++) = tmp32;
            handle->rxRemainingBytes--;
            if (handle->dataWidth > 7U)
            {
                *(handle->rxData++) = (tmp32 >> 8U);
                handle->rxRemainingBytes--;
            }
        }
    }
}

/*!
 * @brief Get the instance for SPI module.
 *
 * @param base SPI base address
 */
uint32_t SPI_GetInstance(SPI_Type *base)
{
    assert(NULL != base);

    int i = 0U;

    for (i = 0; i < FSL_FEATURE_SOC_SPI_COUNT; i++)
    {
        if ((uint32_t)base == s_spiBaseAddrs[i])
        {
            return i;
        }
    }

    assert(false);
    return 0U;
}

void SPI_SetDummyData(SPI_Type *base, uint16_t dummyData)
{
    uint32_t instance = SPI_GetInstance(base);
    s_dummyData[instance] = dummyData;
}

/* Set delay time for SPI transfer. */
void SPI_SetTransferDelay(SPI_Type *base, const spi_delay_config_t *config)
{
    assert(NULL != config);
    /* Set the delay configuration. */
    base->DLY = (SPI_DLY_PRE_DELAY(config->preDelay) | SPI_DLY_POST_DELAY(config->postDelay) |
                 SPI_DLY_FRAME_DELAY(config->frameDelay) | SPI_DLY_TRANSFER_DELAY(config->transferDelay));
}

void SPI_MasterGetDefaultConfig(spi_master_config_t *config)
{
    assert(NULL != config);
    config->enableLoopback = false;
    config->enableMaster = true;
    config->clockPolarity = kSPI_ClockPolarityActiveHigh;
    config->clockPhase = kSPI_ClockPhaseFirstEdge;
    config->direction = kSPI_MsbFirst;
    config->baudRate_Bps = 500000U;
    config->dataWidth = 7U;
    config->sselNumber = kSPI_Ssel0Assert;
    config->sselPolarity = kSPI_SpolActiveAllLow;
    config->delayConfig.frameDelay = 0U;
    config->delayConfig.postDelay = 0U;
    config->delayConfig.preDelay = 0U;
    config->delayConfig.transferDelay = 0U;
}

status_t SPI_MasterInit(SPI_Type *base, const spi_master_config_t *config, uint32_t srcClock_Hz)
{
    int32_t result = 0, instance = 0;
    uint32_t tmp = 0U;

    /* assert params */
    assert(!((NULL == base) || (NULL == config) || (0 == srcClock_Hz)));

    /* Get instance number */
    instance = SPI_GetInstance(base);

    /* Enable clock. */
    CLOCK_EnableClock(s_spiClock[instance]);

    /* Reset spi periphera. */
    RESET_PeripheralReset(s_spiReset[instance]);

    /* set divider */
    result = SPI_MasterSetBaudRate(base, config->baudRate_Bps, srcClock_Hz);
    if (kStatus_Success != result)
    {
        return result;
    }
    /* Set CFG register to configure phase/polarity/direction/master mode/loopback/ssel pin select. */
    tmp |= (SPI_CFG_CPHA(config->clockPhase) | SPI_CFG_CPOL(config->clockPolarity) | SPI_CFG_LSBF(config->direction) |
            SPI_CFG_MASTER(1) | SPI_CFG_LOOP(config->enableLoopback) |
            ((uint32_t)config->sselPolarity & (kSPI_SpolActiveAllHigh)));
    base->CFG = tmp;

    /* Set delay configuration. */
    SPI_SetTransferDelay(base, &(config->delayConfig));

    /* Set dummy data. */
    SPI_SetDummyData(base, (uint8_t)SPI_DUMMYDATA);

    /* Set TXCTL register. */
    base->TXCTL |= (SPI_TXCTL_LEN(config->dataWidth) | (config->sselNumber & kSPI_SselDeAssertAll));

    /* Enable the SPI module. */
    SPI_Enable(base, config->enableMaster);

    return kStatus_Success;
}

void SPI_SlaveGetDefaultConfig(spi_slave_config_t *config)
{
    assert(NULL != config);
    config->enableSlave = true;
    config->clockPolarity = kSPI_ClockPolarityActiveHigh;
    config->clockPhase = kSPI_ClockPhaseFirstEdge;
    config->direction = kSPI_MsbFirst;
    config->dataWidth = 7U;
    config->sselPolarity = kSPI_SpolActiveAllLow;
}

status_t SPI_SlaveInit(SPI_Type *base, const spi_slave_config_t *config)
{
    int32_t instance = 0U;
    uint32_t tmp = 0U;

    /* assert params */
    assert(!((NULL == base) || (NULL == config)));
    /* Get the instance of SPI. */
    instance = SPI_GetInstance(base);
    /* Enable clock. */
    CLOCK_EnableClock(s_spiClock[instance]);

    /* Reset spi periphera. */
    RESET_PeripheralReset(s_spiReset[instance]);

    /* Set confiuration for phase/polarity/direction/active level for SSEL pins. */
    tmp |= SPI_CFG_CPHA(config->clockPhase) | SPI_CFG_CPOL(config->clockPolarity) | SPI_CFG_LSBF(config->direction) |
           ((uint32_t)config->sselPolarity & (kSPI_SpolActiveAllHigh));
    base->CFG = tmp;

    /* Set dummy data. */
    SPI_SetDummyData(base, (uint8_t)SPI_DUMMYDATA);

    /* Set TXCTL register. */
    base->TXCTL |= SPI_TXCTL_LEN(config->dataWidth);

    /* Enable the SPI module. */
    SPI_Enable(base, config->enableSlave);

    return kStatus_Success;
}

void SPI_Deinit(SPI_Type *base)
{
    /* Assert arguments */
    assert(NULL != base);
    uint32_t instance = SPI_GetInstance(base);
    /* Disable SPI module before shutting down the clock. */
    SPI_Enable(base, false);
    /* Disable clock. */
    CLOCK_DisableClock(s_spiClock[instance]);
}

status_t SPI_MasterSetBaudRate(SPI_Type *base, uint32_t baudrate_Bps, uint32_t srcClock_Hz)
{
    uint32_t tmp;

    /* assert params */
    assert(!((NULL == base) || (0 == baudrate_Bps) || (0 == srcClock_Hz)));

    /* calculate baudrate */
    tmp = (srcClock_Hz / baudrate_Bps) - 1;
    if (tmp > 0xFFFF)
    {
        return kStatus_SPI_BaudrateNotSupport;
    }
    base->DIV &= ~SPI_DIV_DIVVAL_MASK;
    base->DIV |= SPI_DIV_DIVVAL(tmp);
    return kStatus_Success;
}

void SPI_WriteDataWithConfigFlags(SPI_Type *base, uint16_t data, uint32_t configFlags)
{
    uint32_t control = 0;
    /* check params */
    assert(NULL != base);
    /* Read origin command from TXCTL register. */
    control = base->TXCTL & (~(SPI_TXDATCTL_EOT_MASK | SPI_TXDATCTL_EOF_MASK | SPI_TXDATCTL_RXIGNORE_MASK));
    /* Mask configFlags */
    control |= (configFlags & (SPI_TXDATCTL_EOT_MASK | SPI_TXDATCTL_EOF_MASK | SPI_TXDATCTL_RXIGNORE_MASK));
    /* Write data and command to register. */
    base->TXDATCTL = (data | control);
}

status_t SPI_MasterTransferCreateHandle(SPI_Type *base,
                                        spi_master_handle_t *handle,
                                        spi_master_callback_t callback,
                                        void *userData)
{
    uint32_t instance = 0;
    /* check 'base' and 'handle'. */
    assert((NULL != base) || (NULL != handle));

    /* Get SPI instance by 'base' param */
    instance = SPI_GetInstance(base);

    memset(handle, 0, sizeof(*handle));
    handle->dataWidth = ((base->TXCTL & SPI_TXCTL_LEN_MASK) >> SPI_TXCTL_LEN_SHIFT);
    handle->callback = callback;
    handle->userData = userData;
    s_spiHandle[instance] = handle;
    s_spiMasterIsr = SPI_MasterTransferHandleIRQ;
    /* Enable SPI NVIC */
    EnableIRQ(s_spiIRQ[instance]);

    return kStatus_Success;
}

status_t SPI_SlaveTransferCreateHandle(SPI_Type *base,
                                       spi_slave_handle_t *handle,
                                       spi_slave_callback_t callback,
                                       void *userData)
{
    SPI_MasterTransferCreateHandle(base, handle, callback, userData);
    s_spiSlaveIsr = SPI_SlaveTransferHandleIRQ;
    return kStatus_Success;
}

status_t SPI_MasterTransferBlocking(SPI_Type *base, spi_transfer_t *xfer)
{
    uint32_t tx_ctrl = 0, last_ctrl = 0;
    uint32_t tmp32, remainingBytes, dataWidth;
    uint32_t instance = SPI_GetInstance(base);
    /* Check params */
    assert(!((NULL == base) || (NULL == xfer) || ((NULL == xfer->txData) && (NULL == xfer->rxData))));

    remainingBytes = xfer->dataSize;
    /* Read datawidth and ssel info from TXCTL. */
    tx_ctrl = base->TXCTL & (SPI_TXCTL_LEN_MASK | SPI_TXCTL_RXIGNORE_MASK | SPI_TXCTL_EOF_MASK | SPI_TXCTL_EOT_MASK |
                             kSPI_SselDeAssertAll);
    dataWidth = ((tx_ctrl & SPI_TXCTL_LEN_MASK) >> SPI_TXCTL_LEN_SHIFT);

    /* Set end of frame configuration. */
    tx_ctrl |= (xfer->configFlags & (uint32_t)kSPI_EndOfFrame) ? (uint32_t)kSPI_EndOfFrame : 0;
    /* Set ignore configuration. */
    tx_ctrl |= (xfer->configFlags & kSPI_ReceiveIgnore);

    /* If rxData is NULL, ignore the receive. */
    if (NULL == xfer->rxData)
    {
        tx_ctrl |= kSPI_ReceiveIgnore;
    }

    /* Setup last command for transfer. */
    last_ctrl = tx_ctrl;
    /* Set end of transfer configuration for last command. */
    last_ctrl |= (xfer->configFlags & (uint32_t)kSPI_EndOfTransfer) ? (uint32_t)kSPI_EndOfTransfer : 0;

    /* Write configuration to register. */
    SPI_WriteConfigFlags(base, tx_ctrl);

    /* Index of loop */
    while (remainingBytes)
    {
        tmp32 = 0U;
        while (!(base->STAT & SPI_STAT_TXRDY_MASK))
        {
        }
        /* If txdata is not NULL. */
        if (xfer->txData)
        {
            tmp32 = *(xfer->txData++);
            if (dataWidth > 7U)
            {
                tmp32 |= ((uint32_t)(*(xfer->txData++))) << 8U;
            }
        }
        else
        {
            tmp32 = (uint32_t)s_dummyData[instance];
        }
        if ((dataWidth > 7) ? (remainingBytes == 2U) : (remainingBytes == 1U))
        {
            base->TXDATCTL = (tmp32 | last_ctrl);
        }
        else
        {
            /* Write data to the Transmit register. */
            base->TXDAT = tmp32;
        }
        /* If the RX ignore bits is not set. */
        if (!(xfer->configFlags & kSPI_ReceiveIgnore))
        {
            /* Read data from the receive register. */
            while (!(base->STAT & SPI_STAT_RXRDY_MASK))
            {
            }
            tmp32 = base->RXDAT;

            /* If receive buffer is not NULL. */
            if (xfer->rxData)
            {
                *(xfer->rxData++) = tmp32;
                if (dataWidth > 7U)
                {
                    *(xfer->rxData++) = (tmp32 >> 8U);
                }
            }
        }
        remainingBytes--;
        if (dataWidth > 7U)
        {
            remainingBytes--;
        }
    }
    return kStatus_Success;
}

status_t SPI_MasterTransferNonBlocking(SPI_Type *base, spi_master_handle_t *handle, spi_transfer_t *xfer)
{
    /* check params */
    assert(
        !((NULL == base) || (NULL == handle) || (NULL == xfer) || ((NULL == xfer->txData) && (NULL == xfer->rxData))));
    uint16_t temp = 0U;

    /* dataSize (in bytes) is not aligned to 16bit (2B) transfer */
    if ((handle->dataWidth > 7U) && (xfer->dataSize & 0x1))
    {
        return kStatus_InvalidArgument;
    }

    /* Check if SPI is busy */
    if (handle->state == kStatus_SPI_Busy)
    {
        return kStatus_SPI_Busy;
    }

    /* Set the handle information */
    handle->txData = xfer->txData;
    handle->rxData = xfer->rxData;
    /* Set count */
    handle->txRemainingBytes = xfer->dataSize;
    handle->rxRemainingBytes = xfer->dataSize;
    handle->totalByteCount = xfer->dataSize;
    /* If the rxData is NULL, ignore the receive. */
    if (NULL == xfer->rxData)
    {
        xfer->configFlags |= kSPI_ReceiveIgnore;
    }
    /* Set config flags for transfer. */
    SPI_WriteConfigFlags(base, (xfer->configFlags & (~SPI_TXDATCTL_EOT_MASK)));
    /* Set the last command. */
    handle->lastCommand = base->TXCTL & (SPI_TXCTL_LEN_MASK | SPI_TXCTL_RXIGNORE_MASK | SPI_TXCTL_EOF_MASK |
                                         SPI_TXCTL_EOT_MASK | kSPI_SselDeAssertAll);
    if (xfer->configFlags & kSPI_EndOfTransfer)
    {
        handle->lastCommand |= SPI_TXDATCTL_EOT_MASK;
    }
    /* Set the SPI state to busy */
    handle->state = kStatus_SPI_Busy;

    /* Write data to TXDAT register to trigger a SPI receive. */
    temp = *handle->txData++;
    handle->txRemainingBytes--;
    if (handle->dataWidth > 7U)
    {
        temp |= (uint32_t)(*handle->txData++) << 8U;
        handle->txRemainingBytes--;
    }

    SPI_WriteData(base, temp);

    /* Enable generating IRQ.
     * If RX ignore bit was set, only enable TX ready interrupt, otherwise,
     * enable RX ready interrupt.
     */
    if (kSPI_ReceiveIgnore & xfer->configFlags)
    {
        handle->rxRemainingBytes = 0U;
        SPI_EnableInterrupts(base, kSPI_TxReadyInterruptEnable);
    }
    else
    {
        SPI_EnableInterrupts(base, kSPI_RxReadyInterruptEnable);
    }

    return kStatus_Success;
}

status_t SPI_SlaveTransferNonBlocking(SPI_Type *base, spi_slave_handle_t *handle, spi_transfer_t *xfer)
{
    status_t status = 0U;

    s_spiSlaveIsr = SPI_SlaveTransferHandleIRQ;
    status = SPI_MasterTransferNonBlocking(base, handle, xfer);

    if (kStatus_Success != status)
    {
        return status;
    }
    return kStatus_Success;
}

status_t SPI_MasterTransferGetCount(SPI_Type *base, spi_master_handle_t *handle, size_t *count)
{
    assert(NULL != handle);

    if (!count)
    {
        return kStatus_InvalidArgument;
    }

    /* Catch when there is not an active transfer. */
    if (handle->state != kStatus_SPI_Busy)
    {
        *count = 0;
        return kStatus_NoTransferInProgress;
    }

    *count = handle->totalByteCount - handle->rxRemainingBytes;
    return kStatus_Success;
}

void SPI_MasterTransferAbort(SPI_Type *base, spi_master_handle_t *handle)
{
    assert(NULL != handle);

    SPI_DisableInterrupts(base, kSPI_TxReadyInterruptEnable | kSPI_RxReadyInterruptEnable);

    handle->state = kStatus_SPI_Idle;
    handle->txRemainingBytes = 0;
    handle->rxRemainingBytes = 0;
}

void SPI_MasterTransferHandleIRQ(SPI_Type *base, spi_master_handle_t *handle)
{
    assert((NULL != base) && (NULL != handle));

    /* IRQ behaviour:
     * - First interrupt is triggered by receive ready interrupt. The transfer function then
     *   tries read data and transmit data interleaved that results to strategy to process
     *   as many items as possible.
     * - In last interrupt, the last state is known by empty rxBuffer and txBuffer. If there
     *   is nothing to receive or send - both operations have been finished and interrupts can be disabled.
     *   If the callback function is not NULL, trigger it.
     */

    /* Data to send or read or expected to receive */
    if (handle->rxRemainingBytes)
    {
        SPI_ReceiveTransfer(base, handle);
    }
    if (handle->txRemainingBytes)
    {
        SPI_SendTransfer(base, handle);
    }
    if ((0U == handle->txRemainingBytes) && (0U == handle->rxRemainingBytes))
    {
        /* Disable TX and RX interrupt. */
        SPI_DisableInterrupts(base, kSPI_RxReadyInterruptEnable | kSPI_TxReadyInterruptEnable);

        /* Set transfer state to idle. */
        handle->state = kStatus_SPI_Idle;
        /* If callback is not NULL, call this function. */
        if (handle->callback)
        {
            (handle->callback)(base, handle, handle->state, handle->userData);
        }
    }
}

void SPI_SlaveTransferHandleIRQ(SPI_Type *base, spi_slave_handle_t *handle)
{
    assert((NULL != base) && (NULL != handle));

    /* IRQ behaviour:
     * - First interrupt is triggered by receive ready interrupt. The transfer function then
     *   tries read data and transmit data interleaved that results to strategy to process
     *   as many items as possible.
     * - In the last interrupt, the last state is known by empty rxBuffer. If there is nothing
     *   to receive or send - both operations have been finished and interrupt can be disabled.
     *   If the callback function is not NULL, call it.
     */

    /* Sending data to TXDAT first in case of data missing. */
    if (handle->txRemainingBytes)
    {
        SPI_SendTransfer(base, handle);
    }

    /* Read data from RXDAT. */
    if (handle->rxRemainingBytes)
    {
        SPI_ReceiveTransfer(base, handle);
    }

    if ((0U == handle->txRemainingBytes) && (0U == handle->rxRemainingBytes))
    {
        /* Disable RX interrupt. */
        SPI_DisableInterrupts(base, kSPI_RxReadyInterruptEnable | kSPI_TxReadyInterruptEnable);
        /* Set transfer state to idle. */
        handle->state = kStatus_SPI_Idle;
        /* If callback is not NULL, call this function. */
        if (handle->callback)
        {
            (handle->callback)(base, handle, handle->state, handle->userData);
        }
    }
}

static void SPI_CommonIRQHandler(SPI_Type *base, void *param)
{
    if (SPI_IsMaster(base))
    {
        s_spiMasterIsr(base, (spi_master_handle_t *)param);
    }
    else
    {
        s_spiSlaveIsr(base, (spi_slave_handle_t *)param);
    }
}

#if defined(SPI0)
void SPI0_DriverIRQHandler(void)
{
    assert(s_spiHandle[0]);
    SPI_CommonIRQHandler(SPI0, s_spiHandle[0]);
}
#endif

#if defined(SPI1)
void SPI1_DriverIRQHandler(void)
{
    assert(s_spiHandle[1]);
    SPI_CommonIRQHandler(SPI1, s_spiHandle[1]);
}
#endif
