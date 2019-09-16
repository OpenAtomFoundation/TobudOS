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

#ifndef _FSL_SPI_H_
#define _FSL_SPI_H_

#include "fsl_common.h"

/*!
 * @addtogroup spi_driver
 * @{
 */

/*! @file */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief SPI driver version 2.0.0. */
#define FSL_SPI_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

#ifndef SPI_DUMMYDATA
/*! @brief SPI dummy transfer data, the data is sent while txBuff is NULL. */
#define SPI_DUMMYDATA (0xFFFFU)
#endif

/* @brief Dummy data for each instance. This data is used when user's tx buffer is NULL*/
extern volatile uint16_t s_dummyData[];

/*! @brief SPI transfer option.*/
enum _spi_xfer_option
{
    kSPI_EndOfFrame = (SPI_TXDATCTL_EOF_MASK),         /*!< Data is treated as the end of a frame. */
    kSPI_EndOfTransfer = (SPI_TXDATCTL_EOT_MASK),      /*!< Data is treated as the end of a transfer. */
    kSPI_ReceiveIgnore = (SPI_TXDATCTL_RXIGNORE_MASK), /*!< Ignore the receive data. */
};

/*! @brief SPI data shifter direction options.*/
typedef enum _spi_shift_direction
{
    kSPI_MsbFirst = 0U, /*!< Data transfers start with most significant bit. */
    kSPI_LsbFirst = 1U, /*!< Data transfers start with least significant bit. */
} spi_shift_direction_t;

/*! @brief SPI clock polarity configuration.*/
typedef enum _spi_clock_polarity
{
    kSPI_ClockPolarityActiveHigh = 0x0U, /*!< Active-high SPI clock (idles low). */
    kSPI_ClockPolarityActiveLow = 0x1U,  /*!< Active-low SPI clock (idles high). */
} spi_clock_polarity_t;

/*! @brief SPI clock phase configuration.*/
typedef enum _spi_clock_phase
{
    kSPI_ClockPhaseFirstEdge = 0x0U,  /*!< First edge on SCK occurs at the middle of the first
                                          *   cycle of a data transfer. */
    kSPI_ClockPhaseSecondEdge = 0x1U, /*!< First edge on SCK occurs at the start of the
                                   *   first cycle of a data transfer. */
} spi_clock_phase_t;

/*! @brief Slave select */
typedef enum _spi_ssel
{
    kSPI_Ssel0Assert = ~SPI_TXDATCTL_TXSSEL0_N_MASK, /*!< Slave select 0 */
#if defined(SPI_TXDATCTL_TXSSEL1_N_MASK)
    kSPI_Ssel1Assert = ~SPI_TXDATCTL_TXSSEL1_N_MASK, /*!< Slave select 1 */
#endif
#if defined(SPI_TXDATCTL_TXSSEL2_N_MASK)
    kSPI_Ssel2Assert = ~SPI_TXDATCTL_TXSSEL2_N_MASK, /*!< Slave select 2 */
#endif
#if defined(SPI_TXDATCTL_TXSSEL3_N_MASK)
    kSPI_Ssel3Assert = ~SPI_TXDATCTL_TXSSEL3_N_MASK, /*!< Slave select 3 */
#endif
    kSPI_SselDeAssertAll = (SPI_TXDATCTL_TXSSEL0_N_MASK
#if defined(FSL_FEATURE_SPI_HAS_SSEL1) & FSL_FEATURE_SPI_HAS_SSEL1
                            |
                            SPI_TXDATCTL_TXSSEL1_N_MASK
#endif
#if defined(FSL_FEATURE_SPI_HAS_SSEL2) & FSL_FEATURE_SPI_HAS_SSEL2
                            |
                            SPI_TXDATCTL_TXSSEL2_N_MASK
#endif
#if defined(FSL_FEATURE_SPI_HAS_SSEL3) & FSL_FEATURE_SPI_HAS_SSEL3
                            |
                            SPI_TXDATCTL_TXSSEL3_N_MASK
#endif
                            ),
} spi_ssel_t;

/*! @brief ssel polarity */
typedef enum _spi_spol
{
#if defined(FSL_FEATURE_SPI_HAS_SSEL0) & FSL_FEATURE_SPI_HAS_SSEL0
    kSPI_Spol0ActiveHigh = SPI_CFG_SPOL0_MASK,
#endif
#if defined(FSL_FEATURE_SPI_HAS_SSEL1) & FSL_FEATURE_SPI_HAS_SSEL1
    kSPI_Spol1ActiveHigh = SPI_CFG_SPOL1_MASK,
#endif
#if defined(FSL_FEATURE_SPI_HAS_SSEL2) & FSL_FEATURE_SPI_HAS_SSEL2
    kSPI_Spol2ActiveHigh = SPI_CFG_SPOL2_MASK,
#endif
#if defined(FSL_FEATURE_SPI_HAS_SSEL3) & FSL_FEATURE_SPI_HAS_SSEL3
    kSPI_Spol3ActiveHigh = SPI_CFG_SPOL3_MASK,
#endif
    kSPI_SpolActiveAllHigh = (SPI_CFG_SPOL0_MASK
#if defined(FSL_FEATURE_SPI_HAS_SSEL1) & FSL_FEATURE_SPI_HAS_SSEL1
                              |
                              SPI_CFG_SPOL1_MASK
#endif
#if defined(FSL_FEATURE_SPI_HAS_SSEL2) & FSL_FEATURE_SPI_HAS_SSEL2
                              |
                              SPI_CFG_SPOL2_MASK
#endif
#if defined(FSL_FEATURE_SPI_HAS_SSEL3) & FSL_FEATURE_SPI_HAS_SSEL3
                              |
                              SPI_CFG_SPOL3_MASK
#endif
                              ),
    kSPI_SpolActiveAllLow = ~kSPI_SpolActiveAllHigh,
} spi_spol_t;

/*! @brief SPI delay time configure structure.*/
typedef struct _spi_delay_config
{
    uint8_t preDelay;      /*!< Delay between SSEL assertion and the beginning of transfer. */
    uint8_t postDelay;     /*!< Delay between the end of transfer and SSEL deassertion. */
    uint8_t frameDelay;    /*!< Delay between frame to frame. */
    uint8_t transferDelay; /*!< Delay between transfer to transfer. */
} spi_delay_config_t;

/*! @brief SPI master user configure structure.*/
typedef struct _spi_master_config
{
    bool enableLoopback;                /*!< Enable loopback for test purpose */
    bool enableMaster;                  /*!< Enable SPI at initialization time */
    uint32_t baudRate_Bps;              /*!< Baud Rate for SPI in Hz */
    spi_clock_polarity_t clockPolarity; /*!< Clock polarity */
    spi_clock_phase_t clockPhase;       /*!< Clock phase */
    spi_shift_direction_t direction;    /*!< MSB or LSB */
    uint8_t dataWidth;                  /*!< Width of the data */
    spi_ssel_t sselNumber;              /*!< Slave select number */
    spi_spol_t sselPolarity;            /*!< Configure active CS polarity */
    spi_delay_config_t delayConfig;     /*!< Configure for delay time. */
} spi_master_config_t;

/*! @brief SPI slave user configure structure.*/
typedef struct _spi_slave_config
{
    bool enableSlave;                   /*!< Enable SPI at initialization time */
    spi_clock_polarity_t clockPolarity; /*!< Clock polarity */
    spi_clock_phase_t clockPhase;       /*!< Clock phase */
    spi_shift_direction_t direction;    /*!< MSB or LSB */
    uint8_t dataWidth;                  /*!< Width of the data */
    spi_spol_t sselPolarity;            /*!< Configure active CS polarity */
} spi_slave_config_t;

/*! @brief SPI transfer status.*/
enum _spi_status
{
    kStatus_SPI_Busy = MAKE_STATUS(kStatusGroup_LPC_MINISPI, 0),  /*!< SPI bus is busy */
    kStatus_SPI_Idle = MAKE_STATUS(kStatusGroup_LPC_MINISPI, 1),  /*!< SPI is idle */
    kStatus_SPI_Error = MAKE_STATUS(kStatusGroup_LPC_MINISPI, 2), /*!< SPI  error */
    kStatus_SPI_BaudrateNotSupport =
        MAKE_STATUS(kStatusGroup_LPC_MINISPI, 3) /*!< Baudrate is not support in current clock source */
};

/*! @brief SPI interrupt sources.*/
enum _spi_interrupt_enable
{
    kSPI_RxReadyInterruptEnable = SPI_INTENSET_RXRDYEN_MASK,           /*!< Rx ready interrupt */
    kSPI_TxReadyInterruptEnable = SPI_INTENSET_TXRDYEN_MASK,           /*!< Tx ready interrupt */
    kSPI_RxOverrunInterruptEnable = SPI_INTENSET_RXOVEN_MASK,          /*!< Rx overrun interrupt */
    kSPI_TxUnderrunInterruptEnable = SPI_INTENSET_TXUREN_MASK,         /*!< Tx underrun interrupt */
    kSPI_SlaveSelectAssertInterruptEnable = SPI_INTENSET_SSAEN_MASK,   /*!< Slave select assert interrupt */
    kSPI_SlaveSelectDeassertInterruptEnable = SPI_INTENSET_SSDEN_MASK, /*!< Slave select deassert interrupt */
    kSPI_AllInterruptEnable = (SPI_INTENSET_RXRDYEN_MASK | SPI_INTENSET_TXRDYEN_MASK | SPI_INTENSET_RXOVEN_MASK |
                               SPI_INTENSET_TXUREN_MASK | SPI_INTENSET_SSAEN_MASK | SPI_INTENSET_SSDEN_MASK)
};

/*! @brief SPI status flags.*/
enum _spi_status_flags
{
    kSPI_RxReadyFlag = SPI_STAT_RXRDY_MASK,           /*!< Receive ready flag. */
    kSPI_TxReadyFlag = SPI_STAT_TXRDY_MASK,           /*!< Transmit ready flag. */
    kSPI_RxOverrunFlag = SPI_STAT_RXOV_MASK,          /*!< Receive overrun flag. */
    kSPI_TxUnderrunFlag = SPI_STAT_TXUR_MASK,         /*!< Transmit underrun flag. */
    kSPI_SlaveSelectAssertFlag = SPI_STAT_SSA_MASK,   /*!< Slave select assert flag. */
    kSPI_SlaveSelectDeassertFlag = SPI_STAT_SSD_MASK, /*!< slave select deassert flag. */
    kSPI_StallFlag = SPI_STAT_STALLED_MASK,           /*!< Stall flag. */
    kSPI_EndTransferFlag = SPI_STAT_ENDTRANSFER_MASK, /*!< End transfer bit. */
    kSPI_MasterIdleFlag = SPI_STAT_MSTIDLE_MASK,      /*!< Master in idle status flag. */
};

/*! @brief SPI transfer structure */
typedef struct _spi_transfer
{
    uint8_t *txData;      /*!< Send buffer */
    uint8_t *rxData;      /*!< Receive buffer */
    size_t dataSize;      /*!< Transfer bytes */
    uint32_t configFlags; /*!< Additional option to control transfer @ref _spi_xfer_option. */
} spi_transfer_t;

/*! @brief  Master handle type */
typedef struct _spi_master_handle spi_master_handle_t;

/*! @brief  Slave handle type */
typedef spi_master_handle_t spi_slave_handle_t;

/*! @brief SPI master callback for finished transmit */
typedef void (*spi_master_callback_t)(SPI_Type *base, spi_master_handle_t *handle, status_t status, void *userData);

/*! @brief SPI slave callback for finished transmit */
typedef void (*spi_slave_callback_t)(SPI_Type *base, spi_slave_handle_t *handle, status_t status, void *userData);

/*! @brief SPI transfer handle structure */
struct _spi_master_handle
{
    uint8_t *volatile txData;         /*!< Transfer buffer */
    uint8_t *volatile rxData;         /*!< Receive buffer */
    volatile size_t txRemainingBytes; /*!< Number of data to be transmitted [in bytes] */
    volatile size_t rxRemainingBytes; /*!< Number of data to be received [in bytes] */
    size_t totalByteCount;            /*!< A number of transfer bytes */
    volatile uint32_t state;          /*!< SPI internal state */
    spi_master_callback_t callback;   /*!< SPI callback */
    void *userData;                   /*!< Callback parameter */
    uint8_t dataWidth;                /*!< Width of the data [Valid values: 1 to 16] */
    uint32_t lastCommand;             /*!< Last command for transfer. */
};

#if defined(__cplusplus)
extern "C" {
#endif
/*******************************************************************************
 * API
 ******************************************************************************/

/*! @brief Returns instance number for SPI peripheral base address. */
uint32_t SPI_GetInstance(SPI_Type *base);

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief  Sets the SPI master configuration structure to default values.
 *
 * The purpose of this API is to get the configuration structure initialized for use in SPI_MasterInit().
 * User may use the initialized structure unchanged in SPI_MasterInit(), or modify
 * some fields of the structure before calling SPI_MasterInit(). After calling this API,
 * the master is ready to transfer.
 * Example:
   @code
   spi_master_config_t config;
   SPI_MasterGetDefaultConfig(&config);
   @endcode
 *
 * @param config pointer to master config structure
 */
void SPI_MasterGetDefaultConfig(spi_master_config_t *config);

/*!
 * @brief Initializes the SPI with master configuration.
 *
 * The configuration structure can be filled by user from scratch, or be set with default
 * values by SPI_MasterGetDefaultConfig(). After calling this API, the slave is ready to transfer.
 * Example
   @code
   spi_master_config_t config = {
   .baudRate_Bps = 500000,
   ...
   };
   SPI_MasterInit(SPI0, &config);
   @endcode
 *
 * @param base SPI base pointer
 * @param config pointer to master configuration structure
 * @param srcClock_Hz Source clock frequency.
 */
status_t SPI_MasterInit(SPI_Type *base, const spi_master_config_t *config, uint32_t srcClock_Hz);

/*!
 * @brief  Sets the SPI slave configuration structure to default values.
 *
 * The purpose of this API is to get the configuration structure initialized for use in SPI_SlaveInit().
 * Modify some fields of the structure before calling SPI_SlaveInit().
 * Example:
   @code
   spi_slave_config_t config;
   SPI_SlaveGetDefaultConfig(&config);
   @endcode
 *
 * @param config pointer to slave configuration structure
 */
void SPI_SlaveGetDefaultConfig(spi_slave_config_t *config);

/*!
 * @brief Initializes the SPI with slave configuration.
 *
 * The configuration structure can be filled by user from scratch or be set with
 * default values by SPI_SlaveGetDefaultConfig().
 * After calling this API, the slave is ready to transfer.
 * Example
   @code
    spi_slave_config_t config = {
    .polarity = kSPI_ClockPolarityActiveHigh;
    .phase = kSPI_ClockPhaseFirstEdge;
    .direction = kSPI_MsbFirst;
    ...
    };
    SPI_SlaveInit(SPI0, &config);
   @endcode
 *
 * @param base SPI base pointer
 * @param config pointer to slave configuration structure
 */
status_t SPI_SlaveInit(SPI_Type *base, const spi_slave_config_t *config);

/*!
 * @brief De-initializes the SPI.
 *
 * Calling this API resets the SPI module, gates the SPI clock.
 * Disable the fifo if enabled.
 * The SPI module can't work unless calling the SPI_MasterInit/SPI_SlaveInit to initialize module.
 *
 * @param base SPI base pointer
 */
void SPI_Deinit(SPI_Type *base);

/*!

 * @brief Enable or disable the SPI Master or Slave
 * @param base SPI base pointer
 * @param enable or disable ( true = enable, false = disable)
 */
static inline void SPI_Enable(SPI_Type *base, bool enable)
{
    if (enable)
    {
        base->CFG |= SPI_CFG_ENABLE_MASK;
    }
    else
    {
        base->CFG &= ~SPI_CFG_ENABLE_MASK;
    }
}

/*! @} */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Gets the status flag.
 *
 * @param base SPI base pointer
 * @return SPI Status, use status flag to AND @ref _spi_status_flags could get the related status.
 */
static inline uint32_t SPI_GetStatusFlags(SPI_Type *base)
{
    assert(NULL != base);
    return base->STAT;
}

/*!
 * @brief Clear the status flag.
 *
 * @param base SPI base pointer
 * @param mask SPI Status, use status flag to AND @ref _spi_status_flags could get the related status.
 */
static inline void SPI_ClearStatusFlags(SPI_Type *base, uint32_t mask)
{
    assert(NULL != base);
    base->STAT |= mask;
}
/*! @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Enables the interrupt for the SPI.
 *
 * @param base SPI base pointer
 * @param irqs SPI interrupt source. The parameter can be any combination of the following values:
 *        @arg kSPI_RxReadyInterruptEnable
 *        @arg kSPI_TxReadyInterruptEnable
 */
static inline void SPI_EnableInterrupts(SPI_Type *base, uint32_t irqs)
{
    assert(NULL != base);
    base->INTENSET = irqs;
}

/*!
 * @brief Disables the interrupt for the SPI.
 *
 * @param base SPI base pointer
 * @param irqs SPI interrupt source. The parameter can be any combination of the following values:
 *        @arg kSPI_RxReadyInterruptEnable
 *        @arg kSPI_TxReadyInterruptEnable
 */
static inline void SPI_DisableInterrupts(SPI_Type *base, uint32_t irqs)
{
    assert(NULL != base);
    base->INTENCLR = irqs;
}
/*! @} */

/*!
 * @name Bus Operations
 * @{
 */

/*!
 * @brief Returns whether the SPI module is in master mode.
 *
 * @param base SPI peripheral address.
 * @return Returns true if the module is in master mode or false if the module is in slave mode.
 */
static inline bool SPI_IsMaster(SPI_Type *base)
{
    return (bool)(((base->CFG) & SPI_CFG_MASTER_MASK) >> SPI_CFG_MASTER_SHIFT);
}
/*!
 * @brief Sets the baud rate for SPI transfer. This is only used in master.
 *
 * @param base SPI base pointer
 * @param baudrate_Bps baud rate needed in Hz.
 * @param srcClock_Hz SPI source clock frequency in Hz.
 */
status_t SPI_MasterSetBaudRate(SPI_Type *base, uint32_t baudrate_Bps, uint32_t srcClock_Hz);

/*!
 * @brief Writes a data into the SPI data register directly.
 *
 * @param base SPI base pointer
 * @param data needs to be write.
 */
static inline void SPI_WriteData(SPI_Type *base, uint16_t data)
{
    base->TXDAT = (data & 0x0000FFFFU);
}

/*!
 * @brief Writes a data into the SPI TXCTL register directly.
 *
 * @param base         SPI base pointer
 * @param configFlags  control command needs to be write.
 */
static inline void SPI_WriteConfigFlags(SPI_Type *base, uint32_t configFlags)
{
    base->TXCTL |= (configFlags & (SPI_TXCTL_EOT_MASK | SPI_TXCTL_EOF_MASK | SPI_TXCTL_RXIGNORE_MASK));
}

/*!
 * @brief Writes a data control info and data into the SPI TX register directly.
 *
 * @param base SPI base pointer
 * @param value needs to be write.
 */
void SPI_WriteDataWithConfigFlags(SPI_Type *base, uint16_t data, uint32_t configFlags);

/*!
 * @brief Gets a data from the SPI data register.
 *
 * @param base SPI base pointer
 * @return Data in the register.
 */
static inline uint32_t SPI_ReadData(SPI_Type *base)
{
    return (base->RXDAT & 0x0000FFFFU);
}

/*!
 * @brief Set delay time for transfer.
 *        the delay uint is SPI clock time, maximum value is 0xF.
 * @param base SPI base pointer
 * @param config configuration for delay option @ref spi_delay_config_t.
 */
void SPI_SetTransferDelay(SPI_Type *base, const spi_delay_config_t *config);

/*!
 * @brief Set up the dummy data. This API can change the default data to be transferred
 *        when users set the tx buffer to NULL.
 *
 * @param base SPI peripheral address.
 * @param dummyData Data to be transferred when tx buffer is NULL.
 */
void SPI_SetDummyData(SPI_Type *base, uint16_t dummyData);
/*! @} */

/*!
 * @name Transactional
 * @{
 */

/*!
 * @brief Initializes the SPI master handle.
 *
 * This function initializes the SPI master handle which can be used for other SPI master transactional APIs. Usually,
 * for a specified SPI instance, call this API once to get the initialized handle.
 *
 * @param base SPI peripheral base address.
 * @param handle SPI handle pointer.
 * @param callback Callback function.
 * @param userData User data.
 */
status_t SPI_MasterTransferCreateHandle(SPI_Type *base,
                                        spi_master_handle_t *handle,
                                        spi_master_callback_t callback,
                                        void *userData);

/*!
 * @brief Transfers a block of data using a polling method.
 *
 * @param base SPI base pointer
 * @param xfer pointer to spi_xfer_config_t structure
 * @retval kStatus_Success Successfully start a transfer.
 * @retval kStatus_InvalidArgument Input argument is invalid.
 */
status_t SPI_MasterTransferBlocking(SPI_Type *base, spi_transfer_t *xfer);

/*!
 * @brief Performs a non-blocking SPI interrupt transfer.
 *
 * @param base SPI peripheral base address.
 * @param handle pointer to spi_master_handle_t structure which stores the transfer state
 * @param xfer pointer to spi_xfer_config_t structure
 * @retval kStatus_Success Successfully start a transfer.
 * @retval kStatus_InvalidArgument Input argument is invalid.
 * @retval kStatus_SPI_Busy SPI is not idle, is running another transfer.
 */
status_t SPI_MasterTransferNonBlocking(SPI_Type *base, spi_master_handle_t *handle, spi_transfer_t *xfer);

/*!
 * @brief Gets the master transfer count.
 *
 * This function gets the master transfer count.
 *
 * @param base SPI peripheral base address.
 * @param handle Pointer to the spi_master_handle_t structure which stores the transfer state.
 * @param count The number of bytes transferred by using the non-blocking transaction.
 * @return status of status_t.
 */
status_t SPI_MasterTransferGetCount(SPI_Type *base, spi_master_handle_t *handle, size_t *count);

/*!
 * @brief SPI master aborts a transfer using an interrupt.
 *
 * This function aborts a transfer using an interrupt.
 *
 * @param base SPI peripheral base address.
 * @param handle Pointer to the spi_master_handle_t structure which stores the transfer state.
 */
void SPI_MasterTransferAbort(SPI_Type *base, spi_master_handle_t *handle);

/*!
 * @brief Interrupts the handler for the SPI.
 *
 * @param base SPI peripheral base address.
 * @param handle pointer to spi_master_handle_t structure which stores the transfer state.
 */
void SPI_MasterTransferHandleIRQ(SPI_Type *base, spi_master_handle_t *handle);

/*!
 * @brief Initializes the SPI slave handle.
 *
 * This function initializes the SPI slave handle which can be used for other SPI slave transactional APIs. Usually,
 * for a specified SPI instance, call this API once to get the initialized handle.
 *
 * @param base SPI peripheral base address.
 * @param handle SPI handle pointer.
 * @param callback Callback function.
 * @param userData User data.
 */
status_t SPI_SlaveTransferCreateHandle(SPI_Type *base,
                                       spi_slave_handle_t *handle,
                                       spi_slave_callback_t callback,
                                       void *userData);

/*!
 * @brief Performs a non-blocking SPI slave interrupt transfer.
 *
 * @note The API returns immediately after the transfer initialization is finished.
 *
 * @param base SPI peripheral base address.
 * @param handle pointer to spi_master_handle_t structure which stores the transfer state
 * @param xfer pointer to spi_xfer_config_t structure
 * @retval kStatus_Success Successfully start a transfer.
 * @retval kStatus_InvalidArgument Input argument is invalid.
 * @retval kStatus_SPI_Busy SPI is not idle, is running another transfer.
 */
status_t SPI_SlaveTransferNonBlocking(SPI_Type *base, spi_slave_handle_t *handle, spi_transfer_t *xfer);

/*!
 * @brief Gets the slave transfer count.
 *
 * This function gets the slave transfer count.
 *
 * @param base SPI peripheral base address.
 * @param handle Pointer to the spi_master_handle_t structure which stores the transfer state.
 * @param count The number of bytes transferred by using the non-blocking transaction.
 * @return status of status_t.
 */
static inline status_t SPI_SlaveTransferGetCount(SPI_Type *base, spi_slave_handle_t *handle, size_t *count)
{
    return SPI_MasterTransferGetCount(base, (spi_master_handle_t *)handle, count);
}

/*!
 * @brief SPI slave aborts a transfer using an interrupt.
 *
 * This function aborts a transfer using an interrupt.
 *
 * @param base SPI peripheral base address.
 * @param handle Pointer to the spi_slave_handle_t structure which stores the transfer state.
 */
static inline void SPI_SlaveTransferAbort(SPI_Type *base, spi_slave_handle_t *handle)
{
    SPI_MasterTransferAbort(base, (spi_master_handle_t *)handle);
}

/*!
 * @brief Interrupts a handler for the SPI slave.
 *
 * @param base SPI peripheral base address.
 * @param handle pointer to spi_slave_handle_t structure which stores the transfer state
 */
void SPI_SlaveTransferHandleIRQ(SPI_Type *base, spi_slave_handle_t *handle);

/*! @} */

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _FSL_SPI_H_*/
