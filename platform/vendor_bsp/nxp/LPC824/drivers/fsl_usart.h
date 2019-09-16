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

#ifndef _FSL_USART_H_
#define _FSL_USART_H_

#include "fsl_common.h"

/*!
 * @addtogroup usart_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief USART driver version 2.0.0. */
#define FSL_USART_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/*! @brief Error codes for the USART driver. */
enum _usart_status
{
    kStatus_USART_TxBusy = MAKE_STATUS(kStatusGroup_LPC_USART, 0),              /*!< Transmitter is busy. */
    kStatus_USART_RxBusy = MAKE_STATUS(kStatusGroup_LPC_USART, 1),              /*!< Receiver is busy. */
    kStatus_USART_TxIdle = MAKE_STATUS(kStatusGroup_LPC_USART, 2),              /*!< USART transmitter is idle. */
    kStatus_USART_RxIdle = MAKE_STATUS(kStatusGroup_LPC_USART, 3),              /*!< USART receiver is idle. */
    kStatus_USART_TxError = MAKE_STATUS(kStatusGroup_LPC_USART, 4),             /*!< Error happens on tx. */
    kStatus_USART_RxError = MAKE_STATUS(kStatusGroup_LPC_USART, 5),             /*!< Error happens on rx. */
    kStatus_USART_RxRingBufferOverrun = MAKE_STATUS(kStatusGroup_LPC_USART, 6), /*!< Error happens on rx ring buffer */
    kStatus_USART_NoiseError = MAKE_STATUS(kStatusGroup_LPC_USART, 7),          /*!< USART noise error. */
    kStatus_USART_FramingError = MAKE_STATUS(kStatusGroup_LPC_USART, 8),        /*!< USART framing error. */
    kStatus_USART_ParityError = MAKE_STATUS(kStatusGroup_LPC_USART, 9),         /*!< USART parity error. */
    kStatus_USART_HardwareOverrun = MAKE_STATUS(kStatusGroup_LPC_USART, 10),    /*!< USART hardware over flow. */
    kStatus_USART_BaudrateNotSupport =
        MAKE_STATUS(kStatusGroup_LPC_USART, 11), /*!< Baudrate is not support in current clock source */
};

/*! @brief USART parity mode. */
typedef enum _usart_parity_mode
{
    kUSART_ParityDisabled = 0x0U, /*!< Parity disabled */
    kUSART_ParityEven = 0x2U,     /*!< Parity enabled, type even, bit setting: PARITYSEL = 10 */
    kUSART_ParityOdd = 0x3U,      /*!< Parity enabled, type odd,  bit setting: PARITYSEL = 11 */
} usart_parity_mode_t;

/*! @brief USART synchronous mode. */
typedef enum _usart_sync_mode
{
    kUSART_SyncModeDisabled = 0x0U, /*!< Asynchronous mode.       */
    kUSART_SyncModeSlave = 0x2U,    /*!< Synchronous slave mode.  */
    kUSART_SyncModeMaster = 0x3U,   /*!< Synchronous master mode. */
} usart_sync_mode_t;

/*! @brief USART stop bit count. */
typedef enum _usart_stop_bit_count
{
    kUSART_OneStopBit = 0U, /*!< One stop bit */
    kUSART_TwoStopBit = 1U, /*!< Two stop bits */
} usart_stop_bit_count_t;

/*! @brief USART data size. */
typedef enum _usart_data_len
{
    kUSART_7BitsPerChar = 0U, /*!< Seven bit mode */
    kUSART_8BitsPerChar = 1U, /*!< Eight bit mode */
} usart_data_len_t;

/*!
 * @brief USART interrupt configuration structure, default settings all disabled.
 */
enum _usart_interrupt_enable
{
    kUSART_RxReadyInterruptEnable = (USART_INTENSET_RXRDYEN_MASK), /*!< Receive ready interrupt. */
    kUSART_TxReadyInterruptEnable = (USART_INTENSET_TXRDYEN_MASK), /*!< Transmit ready interrupt. */
#if defined(FSL_FEATURE_USART_HAS_INTENSET_TXIDLEEN) && FSL_FEATURE_USART_HAS_INTENSET_TXIDLEEN
    kUSART_TxIdleInterruptEnable = (USART_INTENSET_TXIDLEEN_MASK), /*!< Transmit idle interrupt. */
#endif
    kUSART_DeltaCtsInterruptEnable = (USART_INTENSET_DELTACTSEN_MASK),       /*!< Cts pin change interrupt. */
    kUSART_TxDisableInterruptEnable = (USART_INTENSET_TXDISEN_MASK),         /*!< Transmit disable interrupt. */
    kUSART_HardwareOverRunInterruptEnable = (USART_INTENSET_OVERRUNEN_MASK), /*!< hardware ove run interrupt. */
    kUSART_RxBreakInterruptEnable = (USART_INTENSET_DELTARXBRKEN_MASK),      /*!< Receive break interrupt. */
    kUSART_RxStartInterruptEnable = (USART_INTENSET_STARTEN_MASK),           /*!< Receive ready interrupt. */
    kUSART_FramErrorInterruptEnable = (USART_INTENSET_FRAMERREN_MASK),       /*!< Receive start interrupt. */
    kUSART_ParityErrorInterruptEnable = (USART_INTENSET_PARITYERREN_MASK),   /*!< Receive frame error interrupt. */
    kUSART_RxNoiseInterruptEnable = (USART_INTENSET_RXNOISEEN_MASK),         /*!< Receive noise error interrupt. */
#if defined(FSL_FEATURE_USART_HAS_ABERR_CHECK) && FSL_FEATURE_USART_HAS_ABERR_CHECK
    kUSART_AutoBaudErrorInterruptEnable = (USART_INTENSET_ABERREN_MASK), /*!< Receive auto baud error interrupt. */
#endif
    kUSART_AllInterruptEnable =
        (USART_INTENSET_RXRDYEN_MASK | USART_INTENSET_TXRDYEN_MASK |
#if defined(FSL_FEATURE_USART_HAS_INTENSET_TXIDLEEN) && FSL_FEATURE_USART_HAS_INTENSET_TXIDLEEN
         USART_INTENSET_TXIDLEEN_MASK |
#endif
         USART_INTENSET_DELTACTSEN_MASK | USART_INTENSET_TXDISEN_MASK | USART_INTENSET_OVERRUNEN_MASK |
         USART_INTENSET_DELTARXBRKEN_MASK | USART_INTENSET_STARTEN_MASK | USART_INTENSET_FRAMERREN_MASK |
         USART_INTENSET_PARITYERREN_MASK | USART_INTENSET_RXNOISEEN_MASK
#if defined(FSL_FEATURE_USART_HAS_ABERR_CHECK) && FSL_FEATURE_USART_HAS_ABERR_CHECK
         |
         USART_INTENSET_ABERREN_MASK
#endif
         ), /*!< All interrupt. */
};

/*!
 * @brief USART status flags.
 *
 * This provides constants for the USART status flags for use in the USART functions.
 */
enum _usart_flags
{
    kUSART_RxReady = (USART_STAT_RXRDY_MASK),                  /*!< Receive ready flag. */
    kUSART_RxIdleFlag = (USART_STAT_RXIDLE_MASK),              /*!< Receive IDLE flag. */
    kUSART_TxReady = (USART_STAT_TXRDY_MASK),                  /*!< Transmit ready flag. */
    kUSART_TxIdleFlag = (USART_STAT_TXIDLE_MASK),              /*!< Transmit idle flag. */
    kUSART_CtsState = (USART_STAT_CTS_MASK),                   /*!< Cts pin status. */
    kUSART_DeltaCtsFlag = (USART_STAT_DELTACTS_MASK),          /*!< Cts pin change flag. */
    kUSART_TxDisableFlag = (USART_STAT_TXDISSTAT_MASK),        /*!< Transmit disable flag. */
    kUSART_HardwareOverrunFlag = (USART_STAT_OVERRUNINT_MASK), /*!< Hardware over run flag. */
    kUSART_RxBreakFlag = (USART_STAT_DELTARXBRK_MASK),         /*!< Receive break flag. */
    kUSART_RxStartFlag = (USART_STAT_START_MASK),              /*!< receive start flag. */
    kUSART_FramErrorFlag = (USART_STAT_FRAMERRINT_MASK),       /*!< Frame error flag. */
    kUSART_ParityErrorFlag = (USART_STAT_PARITYERRINT_MASK),   /*!< Parity error flag. */
    kUSART_RxNoiseFlag = (USART_STAT_RXNOISEINT_MASK),         /*!< Receive noise flag. */
#if defined(FSL_FEATURE_USART_HAS_ABERR_CHECK) && FSL_FEATURE_USART_HAS_ABERR_CHECK
    kUSART_AutoBaudErrorFlag = (USART_STAT_ABERR_MASK), /*!< Auto baud error flag. */
#endif
};

/*! @brief USART configuration structure. */
typedef struct _usart_config
{
    uint32_t baudRate_Bps;               /*!< USART baud rate  */
    bool enableRx;                       /*!< USART receive enable.  */
    bool enableTx;                       /*!< USART transmit enable.  */
    usart_parity_mode_t parityMode;      /*!< Parity mode, disabled (default), even, odd */
    usart_stop_bit_count_t stopBitCount; /*!< Number of stop bits, 1 stop bit (default) or 2 stop bits  */
    usart_data_len_t bitCountPerChar;    /*!< Data length - 7 bit, 8 bit  */
    bool loopback;                       /*!< Enable peripheral loopback */
    usart_sync_mode_t syncMode;          /*!< Transfer mode - asynchronous, synchronous master, synchronous slave. */
} usart_config_t;

/*! @brief USART transfer structure. */
typedef struct _usart_transfer
{
    uint8_t *data;   /*!< The buffer of data to be transfer.*/
    size_t dataSize; /*!< The byte count to be transfer. */
} usart_transfer_t;

/* Forward declaration of the handle typedef. */
typedef struct _usart_handle usart_handle_t;

/*! @brief USART transfer callback function. */
typedef void (*usart_transfer_callback_t)(USART_Type *base, usart_handle_t *handle, status_t status, void *userData);

/*! @brief USART handle structure. */
struct _usart_handle
{
    uint8_t *volatile txData;   /*!< Address of remaining data to send. */
    volatile size_t txDataSize; /*!< Size of the remaining data to send. */
    size_t txDataSizeAll;       /*!< Size of the data to send out. */
    uint8_t *volatile rxData;   /*!< Address of remaining data to receive. */
    volatile size_t rxDataSize; /*!< Size of the remaining data to receive. */
    size_t rxDataSizeAll;       /*!< Size of the data to receive. */

    uint8_t *rxRingBuffer;              /*!< Start address of the receiver ring buffer. */
    size_t rxRingBufferSize;            /*!< Size of the ring buffer. */
    volatile uint16_t rxRingBufferHead; /*!< Index for the driver to store received data into ring buffer. */
    volatile uint16_t rxRingBufferTail; /*!< Index for the user to get data from the ring buffer. */

    usart_transfer_callback_t callback; /*!< Callback function. */
    void *userData;                     /*!< USART callback function parameter.*/

    volatile uint8_t txState; /*!< TX transfer state. */
    volatile uint8_t rxState; /*!< RX transfer state */
};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/*!
 * @name Get the instance of USART
 * @{
 */

/*! @brief Returns instance number for USART peripheral base address. */
uint32_t USART_GetInstance(USART_Type *base);
/* @} */

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Initializes a USART instance with user configuration structure and peripheral clock.
 *
 * This function configures the USART module with the user-defined settings. The user can configure the configuration
 * structure and also get the default configuration by using the USART_GetDefaultConfig() function.
 * Example below shows how to use this API to configure USART.
 * @code
 *  usart_config_t usartConfig;
 *  usartConfig.baudRate_Bps = 115200U;
 *  usartConfig.parityMode = kUSART_ParityDisabled;
 *  usartConfig.stopBitCount = kUSART_OneStopBit;
 *  USART_Init(USART1, &usartConfig, 20000000U);
 * @endcode
 *
 * @param base USART peripheral base address.
 * @param config Pointer to user-defined configuration structure.
 * @param srcClock_Hz USART clock source frequency in HZ.
 * @retval kStatus_USART_BaudrateNotSupport Baudrate is not support in current clock source.
 * @retval kStatus_InvalidArgument USART base address is not valid
 * @retval kStatus_Success Status USART initialize succeed
 */
status_t USART_Init(USART_Type *base, const usart_config_t *config, uint32_t srcClock_Hz);

/*!
 * @brief Deinitializes a USART instance.
 *
 * This function waits for TX complete, disables the USART clock.
 *
 * @param base USART peripheral base address.
 */
void USART_Deinit(USART_Type *base);

/*!
 * @brief Gets the default configuration structure.
 *
 * This function initializes the USART configuration structure to a default value. The default
 * values are:
 *   usartConfig->baudRate_Bps = 9600U;
 *   usartConfig->parityMode = kUSART_ParityDisabled;
 *   usartConfig->stopBitCount = kUSART_OneStopBit;
 *   usartConfig->bitCountPerChar = kUSART_8BitsPerChar;
 *   usartConfig->loopback = false;
 *   usartConfig->enableTx = false;
 *   usartConfig->enableRx = false;
 *   ...
 * @param config Pointer to configuration structure.
 */
void USART_GetDefaultConfig(usart_config_t *config);

/*!
 * @brief Sets the USART instance baud rate.
 *
 * This function configures the USART module baud rate. This function is used to update
 * the USART module baud rate after the USART module is initialized by the USART_Init.
 * @code
 *  USART_SetBaudRate(USART1, 115200U, 20000000U);
 * @endcode
 *
 * @param base USART peripheral base address.
 * @param baudrate_Bps USART baudrate to be set.
 * @param srcClock_Hz USART clock source freqency in HZ.
 * @retval kStatus_USART_BaudrateNotSupport Baudrate is not support in current clock source.
 * @retval kStatus_Success Set baudrate succeed.
 * @retval kStatus_InvalidArgument One or more arguments are invalid.
 */
status_t USART_SetBaudRate(USART_Type *base, uint32_t baudrate_Bps, uint32_t srcClock_Hz);
/* @} */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Get USART status flags.
 *
 * This function get all USART status flags, the flags are returned as the logical
 * OR value of the enumerators @ref _usart_flags. To check a specific status,
 * compare the return value with enumerators in @ref _usart_flags.
 * For example, to check whether the RX is ready:
 * @code
 *     if (kUSART_RxReady & USART_GetStatusFlags(USART1))
 *     {
 *         ...
 *     }
 * @endcode
 *
 * @param base USART peripheral base address.
 * @return USART status flags which are ORed by the enumerators in the _usart_flags.
 */
static inline uint32_t USART_GetStatusFlags(USART_Type *base)
{
    return base->STAT;
}

/*!
 * @brief Clear USART status flags.
 *
 * This function clear supported USART status flags
 * For example:
 * @code
 *     USART_ClearStatusFlags(USART1, kUSART_HardwareOverrunFlag)
 * @endcode
 *
 * @param base USART peripheral base address.
 * @param mask status flags to be cleared.
 */
static inline void USART_ClearStatusFlags(USART_Type *base, uint32_t mask)
{
    base->STAT = mask;
}
/* @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Enables USART interrupts according to the provided mask.
 *
 * This function enables the USART interrupts according to the provided mask. The mask
 * is a logical OR of enumeration members. See @ref _usart_interrupt_enable.
 * For example, to enable TX ready interrupt and RX ready interrupt:
 * @code
 *     USART_EnableInterrupts(USART1, kUSART_RxReadyInterruptEnable | kUSART_TxReadyInterruptEnable);
 * @endcode
 *
 * @param base USART peripheral base address.
 * @param mask The interrupts to enable. Logical OR of @ref _usart_interrupt_enable.
 */
static inline void USART_EnableInterrupts(USART_Type *base, uint32_t mask)
{
    base->INTENSET = mask & 0x1FF;
}

/*!
 * @brief Disables USART interrupts according to a provided mask.
 *
 * This function disables the USART interrupts according to a provided mask. The mask
 * is a logical OR of enumeration members. See @ref _usart_interrupt_enable.
 * This example shows how to disable the TX ready interrupt and RX ready interrupt:
 * @code
 *     USART_DisableInterrupts(USART1, kUSART_TxReadyInterruptEnable | kUSART_RxReadyInterruptEnable);
 * @endcode
 *
 * @param base USART peripheral base address.
 * @param mask The interrupts to disable. Logical OR of @ref _usart_interrupt_enable.
 */
static inline void USART_DisableInterrupts(USART_Type *base, uint32_t mask)
{
    base->INTENCLR = mask & 0x1FF;
}

/*!
 * @brief Returns enabled USART interrupts.
 *
 * This function returns the enabled USART interrupts.
 *
 * @param base USART peripheral base address.
 */
static inline uint32_t USART_GetEnabledInterrupts(USART_Type *base)
{
    return base->INTENSET;
}
/* @} */

/*!
 * @name Bus Operations
 * @{
 */

/*!
 * @brief Enable the USART transmit.
 *
 * This function will enable or disable the USART transmit.
 *
 * @param base USART peripheral base address.
 * @param enable true for enable and false for disable.
 */
static inline void USART_EnableTx(USART_Type *base, bool enable)
{
    if (enable)
    {
        /* Make sure the USART module is enabled. */
        base->CFG |= USART_CFG_ENABLE_MASK;
        base->CTL &= ~USART_CTL_TXDIS_MASK;
    }
    else
    {
        base->CTL |= USART_CTL_TXDIS_MASK;
    }
}

/*!
 * @brief Enable the USART receive.
 *
 * This function will enable or disable the USART receive.
 * Note: if the transmit is enabled, the receive will not be disabled.
 * @param base USART peripheral base address.
 * @param enable true for enable and false for disable.
 */
static inline void USART_EnableRx(USART_Type *base, bool enable)
{
    if (enable)
    {
        /* Make sure the USART module is enabled. */
        base->CFG |= USART_CFG_ENABLE_MASK;
    }
    else
    {
        /* If the transmit is disabled too. */
        if (base->CTL & USART_CTL_TXDIS_MASK)
        {
            base->CFG &= ~USART_CFG_ENABLE_MASK;
        }
    }
}

/*!
 * @brief Writes to the TXDAT  register.
 *
 * This function will writes data to the TXDAT automatly.The upper layer must ensure
 * that TXDATA has space for data to write before calling this function.
 *
 * @param base USART peripheral base address.
 * @param data The byte to write.
 */
static inline void USART_WriteByte(USART_Type *base, uint8_t data)
{
    base->TXDAT = data;
}

/*!
 * @brief Reads the RXDAT directly.
 *
 * This function reads data from the RXDAT automatly. The upper layer must
 * ensure that the RXDAT is not empty before calling this function.
 *
 * @param base USART peripheral base address.
 * @return The byte read from USART data register.
 */
static inline uint8_t USART_ReadByte(USART_Type *base)
{
    return (base->RXDAT & 0x000000FFU);
}

/*!
 * @brief Writes to the TX register using a blocking method.
 *
 * This function polls the TX register, waits for the TX register to be empty.
 *
 * @param base USART peripheral base address.
 * @param data Start address of the data to write.
 * @param length Size of the data to write.
 */
void USART_WriteBlocking(USART_Type *base, const uint8_t *data, size_t length);

/*!
 * @brief Read RX data register using a blocking method.
 *
 * This function polls the RX register, waits for the RX register to be full.
 *
 * @param base USART peripheral base address.
 * @param data Start address of the buffer to store the received data.
 * @param length Size of the buffer.
 * @retval kStatus_USART_FramingError Receiver overrun happened while receiving data.
 * @retval kStatus_USART_ParityError Noise error happened while receiving data.
 * @retval kStatus_USART_NoiseError Framing error happened while receiving data.
 * @retval kStatus_USART_RxError Overflow or underflow happened.
 * @retval kStatus_Success Successfully received all data.
 */
status_t USART_ReadBlocking(USART_Type *base, uint8_t *data, size_t length);

/* @} */

/*!
 * @name Transactional
 * @{
 */

/*!
 * @brief Initializes the USART handle.
 *
 * This function initializes the USART handle which can be used for other USART
 * transactional APIs. Usually, for a specified USART instance,
 * call this API once to get the initialized handle.
 *
 * @param base USART peripheral base address.
 * @param handle USART handle pointer.
 * @param callback The callback function.
 * @param userData The parameter of the callback function.
 */
status_t USART_TransferCreateHandle(USART_Type *base,
                                    usart_handle_t *handle,
                                    usart_transfer_callback_t callback,
                                    void *userData);

/*!
 * @brief Transmits a buffer of data using the interrupt method.
 *
 * This function sends data using an interrupt method. This is a non-blocking function, which
 * returns directly without waiting for all data to be written to the TX register. When
 * all data is written to the TX register in the IRQ handler, the USART driver calls the callback
 * function and passes the @ref kStatus_USART_TxIdle as status parameter.
 *
 * @note The kStatus_USART_TxIdle is passed to the upper layer when all data is written
 * to the TX register. However it does not ensure that all data are sent out. Before disabling the TX,
 * check the kUSART_TransmissionCompleteFlag to ensure that the TX is finished.
 *
 * @param base USART peripheral base address.
 * @param handle USART handle pointer.
 * @param xfer USART transfer structure. See  #usart_transfer_t.
 * @retval kStatus_Success Successfully start the data transmission.
 * @retval kStatus_USART_TxBusy Previous transmission still not finished, data not all written to TX register yet.
 * @retval kStatus_InvalidArgument Invalid argument.
 */
status_t USART_TransferSendNonBlocking(USART_Type *base, usart_handle_t *handle, usart_transfer_t *xfer);

/*!
 * @brief Sets up the RX ring buffer.
 *
 * This function sets up the RX ring buffer to a specific USART handle.
 *
 * When the RX ring buffer is used, data received are stored into the ring buffer even when the
 * user doesn't call the USART_TransferReceiveNonBlocking() API. If there is already data received
 * in the ring buffer, the user can get the received data from the ring buffer directly.
 *
 * @note When using the RX ring buffer, one byte is reserved for internal use. In other
 * words, if ringBufferSize is 32, then only 31 bytes are used for saving data.
 *
 * @param base USART peripheral base address.
 * @param handle USART handle pointer.
 * @param ringBuffer Start address of the ring buffer for background receiving. Pass NULL to disable the ring buffer.
 * @param ringBufferSize size of the ring buffer.
 */
void USART_TransferStartRingBuffer(USART_Type *base,
                                   usart_handle_t *handle,
                                   uint8_t *ringBuffer,
                                   size_t ringBufferSize);

/*!
 * @brief Aborts the background transfer and uninstalls the ring buffer.
 *
 * This function aborts the background transfer and uninstalls the ring buffer.
 *
 * @param base USART peripheral base address.
 * @param handle USART handle pointer.
 */
void USART_TransferStopRingBuffer(USART_Type *base, usart_handle_t *handle);

/*!
 * @brief Get the length of received data in RX ring buffer.
 *
 * @param handle USART handle pointer.
 * @return Length of received data in RX ring buffer.
 */
size_t USART_TransferGetRxRingBufferLength(usart_handle_t *handle);

/*!
 * @brief Aborts the interrupt-driven data transmit.
 *
 * This function aborts the interrupt driven data sending. The user can get the remainBtyes to find out
 * how many bytes are still not sent out.
 *
 * @param base USART peripheral base address.
 * @param handle USART handle pointer.
 */
void USART_TransferAbortSend(USART_Type *base, usart_handle_t *handle);

/*!
 * @brief Get the number of bytes that have been written to USART TX register.
 *
 * This function gets the number of bytes that have been written to USART TX
 * register by interrupt method.
 *
 * @param base USART peripheral base address.
 * @param handle USART handle pointer.
 * @param count Send bytes count.
 * @retval kStatus_NoTransferInProgress No send in progress.
 * @retval kStatus_InvalidArgument Parameter is invalid.
 * @retval kStatus_Success Get successfully through the parameter \p count;
 */
status_t USART_TransferGetSendCount(USART_Type *base, usart_handle_t *handle, uint32_t *count);

/*!
 * @brief Receives a buffer of data using an interrupt method.
 *
 * This function receives data using an interrupt method. This is a non-blocking function, which
 *  returns without waiting for all data to be received.
 * If the RX ring buffer is used and not empty, the data in the ring buffer is copied and
 * the parameter @p receivedBytes shows how many bytes are copied from the ring buffer.
 * After copying, if the data in the ring buffer is not enough to read, the receive
 * request is saved by the USART driver. When the new data arrives, the receive request
 * is serviced first. When all data is received, the USART driver notifies the upper layer
 * through a callback function and passes the status parameter @ref kStatus_USART_RxIdle.
 * For example, the upper layer needs 10 bytes but there are only 5 bytes in the ring buffer.
 * The 5 bytes are copied to the xfer->data and this function returns with the
 * parameter @p receivedBytes set to 5. For the left 5 bytes, newly arrived data is
 * saved from the xfer->data[5]. When 5 bytes are received, the USART driver notifies the upper layer.
 * If the RX ring buffer is not enabled, this function enables the RX and RX interrupt
 * to receive data to the xfer->data. When all data is received, the upper layer is notified.
 *
 * @param base USART peripheral base address.
 * @param handle USART handle pointer.
 * @param xfer USART transfer structure, see #usart_transfer_t.
 * @param receivedBytes Bytes received from the ring buffer directly.
 * @retval kStatus_Success Successfully queue the transfer into transmit queue.
 * @retval kStatus_USART_RxBusy Previous receive request is not finished.
 * @retval kStatus_InvalidArgument Invalid argument.
 */
status_t USART_TransferReceiveNonBlocking(USART_Type *base,
                                          usart_handle_t *handle,
                                          usart_transfer_t *xfer,
                                          size_t *receivedBytes);

/*!
 * @brief Aborts the interrupt-driven data receiving.
 *
 * This function aborts the interrupt-driven data receiving. The user can get the remainBytes to find out
 * how many bytes not received yet.
 *
 * @param base USART peripheral base address.
 * @param handle USART handle pointer.
 */
void USART_TransferAbortReceive(USART_Type *base, usart_handle_t *handle);

/*!
 * @brief Get the number of bytes that have been received.
 *
 * This function gets the number of bytes that have been received.
 *
 * @param base USART peripheral base address.
 * @param handle USART handle pointer.
 * @param count Receive bytes count.
 * @retval kStatus_NoTransferInProgress No receive in progress.
 * @retval kStatus_InvalidArgument Parameter is invalid.
 * @retval kStatus_Success Get successfully through the parameter \p count;
 */
status_t USART_TransferGetReceiveCount(USART_Type *base, usart_handle_t *handle, uint32_t *count);

/*!
 * @brief USART IRQ handle function.
 *
 * This function handles the USART transmit and receive IRQ request.
 *
 * @param base USART peripheral base address.
 * @param handle USART handle pointer.
 */
void USART_TransferHandleIRQ(USART_Type *base, usart_handle_t *handle);

/* @} */

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_USART_H_ */
