/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HAL_I2S_H__
#define __HAL_I2S_H__

#include "hal_common.h"

/*!
 * @addtogroup I2S
 * @{
 */

/*!
 * @brief I2S driver version number.
 */
#define I2S_DRIVER_VERSION 0u /*!< i2s_0. */

/*!
 * @addtogroup I2S_STATUS
 * @{
 */
#define I2S_STATUS_BUSY             SPI_I2S_CSTAT_BUSY_MASK   /*!< Status flag when I2S is transmitting or receiving. */
#define I2S_STATUS_TX_FULL          SPI_I2S_CSTAT_TXFULL_MASK /*!< Status flag when I2S transmiter buffer is full. */
#define I2S_STATUS_TX_DONE          SPI_I2S_CSTAT_TXEPT_MASK  /*!< Status flag when I2S transmiter buffer & transmiter are empty. */
#define I2S_STATUS_RX_DONE          SPI_I2S_CSTAT_RXAVL_MASK  /*!< Status flag when I2S receiving buffer is with available data. */
/*!
 * @}
 */

/*!
 * @addtogroup I2S_INT
 * @{
 */
#define I2S_INT_TX_EMPTY           SPI_I2S_INTEN_TXIEN_MASK         /*!< Interrupt flag when I2S transmiter buffer is empty. */
#define I2S_INT_TX_DONE            SPI_I2S_INTSTAT_TXEPTINTF_MASK   /*!< Interrupt flag when I2S transmiter buffer & transmiter are empty. */
#define I2S_INT_RX_DONE            SPI_I2S_INTEN_RXIEN_MASK         /*!< Interrupt flag when I2S receiving buffer is with available data. */
#define I2S_INT_XFER_ERROR         SPI_I2S_INTEN_FREIEN_MASK        /*!< Interrupt flag when I2S xfer data error. */
/*!
 * @}
 */

/*!
 * @brief I2S polarity type.
 */
typedef enum
{
    I2S_Polarity_0 = 0u,     /*!< CPOL = 0, Clock line is low when idle.  */
    I2S_Polarity_1 = 1u,     /*!< CPOL = 1, Clock line is high when idle. */
} I2S_Polarity_Type;

/*!
 * @brief I2S data width type.
 */
typedef enum
{
    I2S_DataWidth_16b = 0u,  /*!< Data width is 16 bit. */
    I2S_DataWidth_24b = 1u,  /*!< Data width is 24 bit. */
    I2S_DataWidth_32b = 2u,  /*!< Data width is 32 bit. */
} I2S_DataWidth_Type;

/*!
 * @brief I2S protocol type.
 */
typedef enum
{
    I2S_Protocol_PHILIPS = 0u, /*!< Philips xfer protocol. */
    I2S_Protocol_MSB     = 1u, /*!< MSB xfer protocol. */
    I2S_Protocol_LSB     = 2u, /*!< LSB xfer protocol. */
    I2S_Protocol_PCM     = 3u, /*!< PCM xfer protocol. */
} I2S_Protocol_Type;

/*!
 * @brief I2S xfer mode type.
 */
typedef enum
{
    I2S_XferMode_TxOnly = 0u,  /*!< Enable Tx only. */
    I2S_XferMode_RxOnly = 1u,  /*!< Enable Rx only. */
    I2S_XferMode_TxRx   = 2u,  /*!< Enable both Tx and Rx. */
} I2S_XferMode_Type;

/*!
 * @brief I2S xfer channel type.
 */
typedef enum
{
    I2S_Channel_Left  = 0u, /*!< Left Xfer channel. */
    I2S_Channel_Right = 1u, /*!< Right Xfer channel. */
} I2S_Channel_Type;


/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref I2S_InitMaster() to initialize the I2S module.
 */
typedef struct
{
    uint32_t                     ClockFreqHz;     /*!< Bus Clock Freq. */
    uint32_t                     SampleRate;      /*!< This member configures the I2S sample rate. */
    I2S_DataWidth_Type           DataWidth;       /*!< Specifies the number of bits of data to be transmitted. */
    I2S_Polarity_Type            Polarity;        /*!< Specifies the polarity of clock. */
    I2S_Protocol_Type            Protocol;        /*!< Specifies different communication protocols. */
    I2S_XferMode_Type            XferMode;        /*!< Specifies different communication transmit modes. */
    bool                         EnableMCLK;      /*!< Specifies whether enable MCLK. */
} I2S_Master_Init_Type;

/*!
 * @brief Initialize the I2S Master module.
 *
 * @param I2Sx I2S instance.
 * @param init  Pointer to the initialization structure. See to @ref I2S_Master_Init_Type.
 * @return None.
 */
void I2S_InitMaster(I2S_Type * I2Sx, I2S_Master_Init_Type * init);

/*!
 * @brief Enable the I2S module.
 *
 * The I2S module should be enabled before sending or receiving data.
 *
 * @param I2Sx I2S instance.
 * @param enable 'true' to enable the module, 'false' to disable the module.
 * @return None.
 */
void I2S_Enable(I2S_Type * I2Sx, bool enable);

/*!
 * @brief Get the current status flags of the I2S module.
 *
 * @param I2Sx I2S instance.
 * @return Status flags. See to @ref I2S_STATUS.
 */
uint32_t I2S_GetStatus(I2S_Type * I2Sx);

/*!
 * @brief Enable interrupts of the I2S module.
 *
 * @param I2Sx I2S instance.
 * @param interrupts Interrupt code masks. See to @ref I2S_INT.
 * @param enable 'true' to enable the indicated interrupts, 'false' to disable the indicated interrupts.
 * @return None.
 */
void I2S_EnableInterrupts(I2S_Type * I2Sx, uint32_t interrupts, bool enable);

/*!
 * @brief Get the interrupts status flags of the I2S module.
 *
 * @param I2Sx I2S instance.
 * @return Interrupt status flags. See to @ref I2S_INT.
 */
uint32_t I2S_GetInterruptStatus(I2S_Type * I2Sx);

/*!
 * @brief Clear the interrupt status flags of the I2S module.
 *
 * @param I2Sx I2S instance.
 * @param interrupts The mask codes of the indicated interrupt flags to be cleared.
 * @return None.
 */
void I2S_ClearInterruptStatus(I2S_Type * I2Sx, uint32_t interrupts);

/*!
 * @brief Put the data into transmiter buffer of the I2S module.
 *
 * @param I2Sx I2S instance.
 * @param data Data value to be send through the transmiter.
 * @return None.
 */
void I2S_PutData(I2S_Type * I2Sx, uint32_t dat);

/*!
 * @brief Get the data from receiver buffer of the I2S module.
 *
 * @param I2Sx I2S instance.
 * @return The data value received from the receiver.
 * @return data Data value to be received from the transmiter.
 */
uint32_t I2S_GetData(I2S_Type * I2Sx);

/*!
 * @brief Enable the DMA trigger from the I2S module.
 *
 * The DMA trigger events are the same as the interrupts.
 *
 * @param I2Sx I2S instance.
 * @param enable 'true' to enable the DMA trigger, 'false' to disable the DMA trigger.
 * @return None.
 */
void I2S_EnableDMA(I2S_Type * I2Sx, bool enable);

/*!
 * @brief Get the hardware Rx data buffer's address the I2S module.
 *
 * The return value of the address is most used with DMA module.
 *
 * @param I2Sx I2S instance.
 * @return The value of the address for I2S module's hardware Rx data buffer.
 */
uint32_t I2S_GetRxDataRegAddr(I2S_Type * I2Sx);

/*!
 * @brief Get the hardware Tx data buffer's address the I2S module.
 *
 * The return value of the address is most used with DMA module.
 *
 * @param I2Sx I2S instance.
 * @return The value of the address for I2S module's hardware Tx data buffer.
 */
uint32_t I2S_GetTxDataRegAddr(I2S_Type * I2Sx);

/*!
 * @brief Get the transmitting channel of the I2S module.
 *
 * @param I2Sx I2S instance.
 * @return The current transmitting channel.
 */
I2S_Channel_Type I2S_GetXferChannel(I2S_Type * I2Sx);

/*!
 *@}
 */

#endif /* __HAL_I2S_H__ */

/* EOF. */
