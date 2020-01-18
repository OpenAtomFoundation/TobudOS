/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HAL_SPI_ADAPTER_H__
#define __HAL_SPI_ADAPTER_H__

/*!
 * @addtogroup SPI_Adapter
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define HAL_SPI_MASTER_HANDLE_SIZE (68U)
#define HAL_SPI_SLAVE_HANDLE_SIZE (68U)

/*! @brief Return status for the spi driver.*/
typedef enum _hal_spi_status
{
    kStatus_HAL_SpiSuccess = kStatus_Success,                      /*!< Successfully */
    kStatus_HAL_SpiError   = MAKE_STATUS(kStatusGroup_HAL_SPI, 1), /*!< spi error */
    kStatus_HAL_SpiBusy    = MAKE_STATUS(kStatusGroup_HAL_SPI, 2), /*!< spi bus is busy */
    kStatus_HAL_SpiIdle    = MAKE_STATUS(kStatusGroup_HAL_SPI, 3), /*!< spi is idle */
} hal_spi_status_t;

/*! @brief spi clock polarity configuration.*/
typedef enum _hal_spi_clock_polarity
{
    kHAL_SpiClockPolarityActiveHigh = 0x0U, /*!< Active-high spi clock (idles low). */
    kHAL_SpiClockPolarityActiveLow          /*!< Active-low spi clock (idles high). */
} hal_spi_clock_polarity_t;

/*! @brief spi clock phase configuration.*/
typedef enum _hal_spi_clock_phase
{
    kHAL_SpiClockPhaseFirstEdge = 0x0U, /*!< First edge on SPSCK occurs at the middle of the first
                                         *   cycle of a data transfer. */
    kHAL_SpiClockPhaseSecondEdge        /*!< First edge on SPSCK occurs at the start of the
                                         *   first cycle of a data transfer. */
} hal_spi_clock_phase_t;

/*! @brief spi data shifter direction options.*/
typedef enum _hal_spi_shift_direction
{
    kHAL_SpiMsbFirst = 0x0U, /*!< Data transfers start with most significant bit. */
    kHAL_SpiLsbFirst         /*!< Data transfers start with least significant bit. */
} hal_spi_shift_direction_t;

/*! @brief spi master user configure structure.*/
typedef struct _hal_spi_master_config
{
    uint32_t srcClock_Hz;                /*!< Clock source for spi in Hz */
    uint32_t baudRate_Bps;               /*!< Baud Rate for spi in Hz */
    hal_spi_clock_polarity_t polarity;   /*!< Clock polarity */
    hal_spi_clock_phase_t phase;         /*!< Clock phase */
    hal_spi_shift_direction_t direction; /*!< MSB or LSB */
    uint8_t instance;                    /*!< Instance of the spi */
    bool enableMaster;                   /*!< Enable spi at initialization time */
} hal_spi_master_config_t;

/*! @brief spi slave user configure structure.*/
typedef struct _hal_spi_slave_config
{
    hal_spi_clock_polarity_t polarity;   /*!< Clock polarity */
    hal_spi_clock_phase_t phase;         /*!< Clock phase */
    hal_spi_shift_direction_t direction; /*!< MSB or LSB */
    uint8_t instance;                    /*!< Instance of the spi */
    bool enableSlave;                    /*!< Enable spi at initialization time */
} hal_spi_slave_config_t;

/*! @brief spi transfer structure */
typedef struct _hal_spi_transfer
{
    uint8_t *txData; /*!< Send buffer */
    uint8_t *rxData; /*!< Receive buffer */
    size_t dataSize; /*!< Transfer bytes */
    uint32_t flags;  /*!< spi control flag.*/
} hal_spi_transfer_t;

typedef void *hal_spi_master_handle_t;
typedef void *hal_spi_slave_handle_t;

/*! @brief spi master callback for finished transmit */
typedef void (*hal_spi_master_transfer_callback_t)(hal_spi_master_handle_t handle,
                                                   hal_spi_status_t status,
                                                   void *callbackParam);

/*! @brief spi slave callback for finished transmit */
typedef void (*hal_spi_slave_transfer_callback_t)(hal_spi_slave_handle_t handle,
                                                  hal_spi_status_t status,
                                                  void *callbackParam);

#if defined(__cplusplus)
extern "C" {
#endif

/*******************************************************************************
 * APIs
 ******************************************************************************/
/*!
 * @name Initialization and de-initialization
 * @{
 */

/*!
 * @brief Initializes the spi with master configuration.
 *
 * This function configures the spi master with user-defined settings. The user can configure the configuration
 * structure. The parameter handle is a pointer to point to a memory space of size #HAL_SPI_MASTER_HANDLE_SIZE
 * allocated by the caller.
 *
 * Example
 * @code
 *   static uint32_t s_masterHandleBuffer[((HAL_SPI_MASTER_HANDLE_SIZE + sizeof(uint32_t) - 1) / sizeof(uitn32_t))];
 *   static hal_spi_master_handle_t s_masterHandle = (hal_spi_master_handle_t)&s_masterHandleBuffer[0];
 *   hal_spi_master_config_t userConfig;
 *   userConfig.polarity = kHAL_SpiClockPolarityActiveHigh;
 *   userConfig.phase = kHAL_SpiClockPhaseFirstEdge;
 *   userConfig.direction = kHAL_SpiMsbFirst;
 *   userConfig.baudRate_Bps = 500000U;
 *   userConfig.enableMaster = true;
 *   userConfig.srcClock_Hz = 48000000;
 *   userConfig.instance = 0;
 *   HAL_SpiMasterInit(s_masterHandle, &userConfig);
 * @endcode
 *
 * @param handle Pointer to point to a memory space of size #HAL_SPI_MASTER_HANDLE_SIZE allocated by the caller.
 * The handle should be 4 byte aligned, because unaligned access does not support on some devices.
 * @param config pointer to master configuration structure
 * @retval kStatus_HAL_SpiError An error occurred.
 * @retval kStatus_HAL_SpiSuccess spi master initialization succeed
 *
 */
hal_spi_status_t HAL_SpiMasterInit(hal_spi_master_handle_t handle, const hal_spi_master_config_t *config);

/*!
 * @brief Initializes the spi with slave configuration.
 *
 * This function configures the spi slave with user-defined settings. The user can configure the configuration
 * structure. The parameter handle is a pointer to point to a memory space of size #HAL_SPI_SLAVE_HANDLE_SIZE
 * allocated by the caller.
 * After calling this API, the slave is ready to transfer.
 *
 * Example
 * @code
 *   static uint32_t s_slaveHandleBuffer[((HAL_SPI_SLAVE_HANDLE_SIZE + sizeof(uint32_t) - 1) / sizeof(uitn32_t))];
 *   static hal_spi_slave_handle_t s_slaveHandle = (hal_spi_slave_handle_t)&s_slaveHandleBuffer[0];
 *   hal_spi_slave_config_t userConfig;
 *   userConfig.polarity = kHAL_SpiClockPolarityActiveHigh;
 *   userConfig.phase = kHAL_SpiClockPhaseFirstEdge;
 *   userConfig.direction = kHAL_SpiMsbFirst;
 *   userConfig.instance = 0;
 *   userConfig.enableSlave = true;
 *   HAL_SpiSlaveInit(s_slaveHandle, &userConfig);
 * @endcode
 *
 * @param handle Pointer to point to a memory space of size #HAL_SPI_SLAVE_HANDLE_SIZE allocated by the caller.
 * The handle should be 4 byte aligned, because unaligned access does not support on some devices.
 * @param config pointer to slave configuration structure
 * @retval kStatus_HAL_SpiError An error occurred.
 * @retval kStatus_HAL_SpiSuccess spi slave initialization succeed
 */
hal_spi_status_t HAL_SpiSlaveInit(hal_spi_slave_handle_t handle, const hal_spi_slave_config_t *config);

/*!
 * @brief De-initializes the spi master.
 *
 * De-initializes the spi master.
 * The spi master can't work unless calling the HAL_SpiMasterInit to initialize module.
 *
 * @param handle spi master handle pointer, this should be a static variable.
 * @retval kStatus_HAL_SpiSuccess spi master de-initialization succeed
 */
hal_spi_status_t HAL_SpiMasterDeinit(hal_spi_master_handle_t handle);

/*!
 * @brief De-initializes the spi slave.
 *
 * De-initializes the spi slave.
 * The spi slave can't work unless calling the HAL_SpiSlaveInit to initialize module.
 *
 * @param handle spi slave handle pointer, this should be a static variable.
 * @retval kStatus_HAL_SpiSuccess spi slave de-initialization succeed
 */
hal_spi_status_t HAL_SpiSlaveDeinit(hal_spi_slave_handle_t handle);

/*! @} */

/*!
 * @name Transactional
 * @{
 */

/*!
 * @brief Installs a callback and callback parameter.
 *
 * This function is used to install the callback and callback parameter for spi master module.
 * When any status of the spi master changed, the driver will notify the upper layer by the installed callback
 * function. And the status is also passed as status parameter when the callback is called.
 *
 * @param handle spi master handle pointer, this should be a static variable.
 * @param callback Callback function.
 * @param callbackParam The parameter of the callback function.
 * @retval kStatus_HAL_SpiSuccess spi master handle created
 */
hal_spi_status_t HAL_SpiMasterTransferInstallCallback(hal_spi_master_handle_t handle,
                                                      hal_spi_master_transfer_callback_t callback,
                                                      void *callbackParam);

/*!
 * @brief Transfers a block of data using a polling method.
 *
 * @param handle spi master handle pointer, this should be a static variable.
 * @param xfer pointer to hal_spi_transfer_t structure
 * @retval kStatus_HAL_SpiError An error occurred.
 * @retval kStatus_HAL_SpiSuccess Successfully sent all data.
 */
hal_spi_status_t HAL_SpiMasterTransferBlocking(hal_spi_master_handle_t handle, hal_spi_transfer_t *xfer);

/*!
 * @brief Performs a non-blocking spi interrupt transfer.
 *
 * @note If spi transfer data frame size is 16 bits, the transfer size cannot be an odd number.
 *
 * @param handle spi master handle pointer, this should be a static variable.
 * @param xfer pointer to hal_spi_transfer_t structure
 * @retval kStatus_HAL_SpiError An error occurred.
 * @retval kStatus_HAL_SpiBusy spi is not idle, is running another transfer.
 * @retval kStatus_HAL_SpiSuccess Successfully start the data transmission.
 */
hal_spi_status_t HAL_SpiMasterTransferNonBlocking(hal_spi_master_handle_t handle, hal_spi_transfer_t *xfer);

/*!
 * @brief Gets the bytes of the spi interrupt transferred.
 *
 * @param handle spi master handle pointer, this should be a static variable.
 * @param count Transferred bytes of spi master.
 * @retval kStatus_HAL_SpiError An error occurred.
 * @retval kStatus_HAL_SpiSuccess Succeed get the transfer count.
 */
hal_spi_status_t HAL_SpiMasterTransferGetCount(hal_spi_master_handle_t handle, size_t *spiCount);

/*!
 * @brief Aborts an spi transfer using interrupt.
 *
 * @param handle spi master handle pointer, this should be a static variable.
 * @retval kStatus_HAL_SpiSuccess Succeed abort the transfer.
 */
hal_spi_status_t HAL_SpiMasterTransferAbort(hal_spi_master_handle_t handle);

/*!
 * @brief Installs a callback and callback parameter.
 *
 * This function is used to install the callback and callback parameter for spi slave module.
 * When any status of the spi slave changed, the driver will notify the upper layer by the installed callback
 * function. And the status is also passed as status parameter when the callback is called.
 *
 * @param handle spi slave handle pointer, this should be a static variable.
 * @param callback Callback function.
 * @param callbackParam The parameter of the callback function.
 * @retval kStatus_HAL_SpiSuccess spi slave handle created
 */
hal_spi_status_t HAL_SpiSlaveTransferInstallCallback(hal_spi_slave_handle_t handle,
                                                     hal_spi_slave_transfer_callback_t callback,
                                                     void *callbackParam);

/*!
 * @brief Performs a non-blocking spi slave interrupt transfer.
 *
 * @note If spi transfer data frame size is 16 bits, the transfer size cannot be an odd number.
 *
 * @param handle spi slave handle pointer, this should be a static variable.
 * @param xfer pointer to hal_spi_xfer_config_t structure
 * @retval kStatus_HAL_SpiSuccess Successfully start a transfer.
 * @retval kStatus_HAL_SpiError An error occurred.
 * @retval kStatus_HAL_SpiBusy spi is not idle, is running another transfer.
 */
hal_spi_status_t HAL_SpiSlaveTransferNonBlocking(hal_spi_slave_handle_t handle, hal_spi_transfer_t *xfer);

/*!
 * @brief Gets the bytes of the spi interrupt transferred.
 *
 * @param handle spi slave handle pointer, this should be a static variable.
 * @param count Transferred bytes of spi slave.
 * @retval kStatus_HAL_SpiSuccess Succeed get the transfer count.
 * @retval kStatus_HAL_SpiError An error occurred.
 */
hal_spi_status_t HAL_SpiSlaveTransferGetCount(hal_spi_slave_handle_t handle, size_t *spiCount);

/*!
 * @brief Aborts an spi slave transfer using interrupt.
 *
 * @param handle spi slave handle pointer, this should be a static variable.
 * @retval kStatus_HAL_SpiSuccess Succeed abort the transfer.
 */
hal_spi_status_t HAL_SpiSlaveTransferAbort(hal_spi_slave_handle_t handle);

/*! @} */

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* __HAL_SPI_ADAPTER_H__ */
