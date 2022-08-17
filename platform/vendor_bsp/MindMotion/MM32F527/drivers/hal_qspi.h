/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HAL_QSPI_H__
#define __HAL_QSPI_H__

#include "hal_common.h"

/*!
 * @addtogroup QSPI
 * @{
 */

/*!
 * @brief QSPI driver version number.
 */
#define QSPI_DRIVER_VERSION 0u /*!< QSPI_0. */

/*!
 * @addtogroup QSPI_STATUS
 * @{
 */
#define QSPI_STATUS_XIP_ENABLE          QSPI_SR_XIPST_MASK          /*!< Status flag when QSPI XIP enabled. */
#define QSPI_STATUS_FIFO_FULL           QSPI_SR_FULL_MASK           /*!< Status flag when QSPI FIFO full. */
#define QSPI_STATUS_FIFO_EMPTY          QSPI_SR_EMPTY_MASK          /*!< Status flag when QSPI FIFO empty. */
#define QSPI_STATUS_BUSY                QSPI_SR_BUSY_MASK           /*!< Status flag when QSPI xfer data. */
#define QSPI_STATUS_XFER_DONE           QSPI_SR_TCF_MASK            /*!< Status flag when QSPI xfer done. */
#define QSPI_STATUS_OPMODE_CHANGE_DONE  QSPI_SR_OPCRCF_MASK         /*!< Status flag when QSPI change the operation mode done. */
/*!
 * @}
 */

/*!
 * @addtogroup QSPI_INT
 * @{
 */
#define QSPI_INT_FIFO_FULL              QSPI_IDER_FULLINTEN_MASK    /*!< Interrupt enable when QSPI FIFO is full. */
#define QSPI_INT_FIFO_EMPTY             QSPI_IDER_EMPTYINTEN_MASK   /*!< Interrupt enable when QSPI FIFO is empty. */
#define QSPI_INT_XFER_DONE              QSPI_IDER_TCFINTEN_MASK     /*!< Interrupt enable when QSPI xfer done. */
/*!
 * @}
 */

/*!
 * @addtogroup QSPI_DMA
 * @{
 */
#define QSPI_DMA_FIFO_FULL              QSPI_IDER_FULLDMAEN_MASK    /*!< DMA request enable when QSPI FIFO is full. */
#define QSPI_DMA_FIFO_EMPTY             QSPI_IDER_EMPTYDMAEN_MASK   /*!< DMA request enable when QSPI FIFO is empty. */
#define QSPI_DMA_XFER_DONE              QSPI_IDER_TCFDMAEN_MASK     /*!< DMA request enable when QSPI xfer done. */
/*!
 * @}
 */

/*!
 * @brief SPI mode, set the clock polarity(CPOL) & clock phase(CPHA).
 */
typedef enum
{
    QSPI_SpiMode_0 = 0x00, /*!< SPI mode 0, CPOL = 0, CPHA = 0. */
    QSPI_SpiMode_3 = 0x01, /*!< SPI mode 0, CPOL = 1, CPHA = 1. */
} QSPI_SpiMode_Type;

/*!
 * @brief Operation mode, defines the QSPI operation status.
 */
typedef enum
{
    QSPI_OpMode_Direct   = 0x00, /*!< Direct mode, read data by mapped addr. */
    QSPI_OpMode_Indirect = 0x02, /*!< Indirect mode, read & write data by QSPI peripheral. */
    QSPI_OpMode_Idle     = 0x03, /*!< Idle mode, cannot read data by direct mode. */
} QSPI_OpMode_Type;

/*!
 * @brief Word width, defines the word width when xfer content.
 */
typedef enum
{
    QSPI_WordWidth_8b   = 0x00, /*!< The word wide is 8 bit. */
    QSPI_WordWidth_16b  = 0x01, /*!< The word wide is 16 bit. */
    QSPI_WordWidth_24b  = 0x02, /*!< The word wide is 24 bit. */
    QSPI_WordWidth_32b  = 0x03, /*!< The word wide is 32 bit. */
} QSPI_WordWidth_Type;

/*!
 * @brief Bus width or line mode, defines the bus width when xfer content.
 */
typedef enum
{
    QSPI_BusWidth_None  = 0x00, /*!< No more content in this phase. */
    QSPI_BusWidth_1b    = 0x01, /*!< Xfer content by signal line. (using MOSI or MISO xfer content). */
    QSPI_BusWidth_2b    = 0x02, /*!< Xfer content by dual lines.  (using DA0 & DA1 xfer content). */
    QSPI_BusWidth_4b    = 0x03, /*!< Xfer content by quad lines.  (using DA0, DA1, DA2 & DA3 xfer content). */
} QSPI_BusWidth_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref QSPI_Init() to initialize the QSPI module.
 */
typedef struct
{
    uint32_t            SckDiv;            /*!< Specify the QSPI SCK Prescaler.
                                                The SCK output clock = AHB_CLK / SckDiv.
                                                The available value must be even & the range would be 2 ~ 128. */
    uint32_t            CsHighLevelCycles; /*!< Specify the QSPI CS High Level Cycles.
                                                The minimum duration for CS output high level (invalid level) is AHB_CLK * CsHighLevelCycles.
                                                The available range would be 2 ~ 9. */
    QSPI_SpiMode_Type   SpiMode;           /*!< Specify the SPI mode. */
} QSPI_Init_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref QSPI_EnableDirectRead() to enable direct mode.
 */
typedef struct
{
    /* instruction phase. */
    QSPI_BusWidth_Type  CmdBusWidth;    /*!< Specify the QSPI instruction bus width. */
    uint32_t            CmdValue;       /*!< Specify the QSPI instruction value. */

    /* addr phase. */
    /* the addr value will be determined when accessing QSPI device by direct operation mode. */
    QSPI_BusWidth_Type  AddrBusWidth;   /*!< Specify the QSPI addr bus width. */
    QSPI_WordWidth_Type AddrWordWidth;  /*!< Specify the QSPI addr bit width. */

    /* alt phase. */
    QSPI_BusWidth_Type  AltBusWidth;    /*!< Specify the QSPI alt bus width. */
    QSPI_WordWidth_Type AltWordWidth;   /*!< Specify the QSPI alt bit width. */
    uint32_t            AltValue;       /*!< Specify the QSPI alt value. */

    /* dummy phase. */
    uint32_t            DummyCycles;    /*!< Specify the QSPI dummy cycles, available range would be 0 ~ 31. */

    /* data phase. */
    QSPI_BusWidth_Type  DataBusWidth;   /*!< Specify the QSPI data bus width. */
} QSPI_DirectXferConf_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref QSPI_SetIndirectReadConf() or @ref QSPI_SetIndirectWriteConf() to set xfer configure.
 */
typedef struct
{
    /* instruction phase. */
    QSPI_BusWidth_Type  CmdBusWidth;   /*!< Specify the QSPI instruction bus width. */
    uint32_t            CmdValue;       /*!< Specify the QSPI instruction value. */

    /* addr phase. */
    QSPI_BusWidth_Type  AddrBusWidth;   /*!< Specify the QSPI addr bus width. */
    QSPI_WordWidth_Type AddrWordWidth;  /*!< Specify the QSPI addr bit width. */
    uint32_t            AddrValue;      /*!< Specify the QSPI addr value. */

    /* alt phase. */
    QSPI_BusWidth_Type  AltBusWidth;    /*!< Specify the QSPI alt bus width. */
    QSPI_WordWidth_Type AltWordWidth;   /*!< Specify the QSPI alt bit width. */
    uint32_t            AltValue;       /*!< Specify the QSPI alt value. */

    /* dummy phase. */
    uint32_t            DummyCycles;    /*!< Specify the QSPI dummy cycles, available range would be 0 ~ 31. */

    /* data phase. */
    QSPI_BusWidth_Type  DataBusWidth;   /*!< Specify the QSPI data bus width. */
    QSPI_WordWidth_Type DataWordWidth;  /*!< Specify the QSPI data bit width. */
    uint32_t            DataLen;        /*!< Specify the QSPI data length. */
} QSPI_IndirectXferConf_Type;

/*!
 * @brief Initialize the QSPI module.
 *
 * @param QSPIx QSPI instance.
 * @param init  Pointer to the initialization structure. See to @ref QSPI_Init_Type.
 * @return None.
 */
void QSPI_Init(QSPI_Type * QSPIx, QSPI_Init_Type * init);

/*!
 * @brief Enable the QSPI direct read configure.
 *
 * @param QSPIx QSPI instance.
 * @param conf Pointer to the initialization structure. See to @ref QSPI_DirectXferConf_Type.
 * @return None.
 */
void QSPI_EnableDirectRead(QSPI_Type * QSPIx, QSPI_DirectXferConf_Type * conf);

/*!
 * @brief Get the data from mapped addr as 8 bit.
 *
 * @param QSPIx QSPI instance.
 * @param offset offset of data in QSPI Device.
 * @return The data value received from the mapped addr.
 */
uint8_t QSPI_GetDirectData8b(QSPI_Type * QSPIx, uint32_t offset);

/*!
 * @brief Get the data from mapped addr as 16 bit.
 *
 * @param QSPIx QSPI instance.
 * @param offset offset of data in QSPI Device.
 * @return The data value received from the mapped addr.
 */
uint16_t QSPI_GetDirectData16b(QSPI_Type * QSPIx, uint32_t addr);

/*!
 * @brief Get the data from mapped addr as 32 bit.
 *
 * @param QSPIx QSPI instance.
 * @param offset offset of data in QSPI Device.
 * @return The data value received from the mapped addr.
 */
uint32_t QSPI_GetDirectData32b(QSPI_Type * QSPIx, uint32_t addr);

/*!
 * @brief Set the QSPI indirect read configure.
 *
 * After configuration, the indirect read process will start.
 *
 * @param QSPIx QSPI instance.
 * @param conf Pointer to the config structure. See to @ref QSPI_IndirectXferConf_Type.
 * @return None.
 */
void QSPI_SetIndirectReadConf(QSPI_Type * QSPIx, QSPI_IndirectXferConf_Type * conf);

/*!
 * @brief Get the data from FIFO in indirect mode of the QSPI module.
 *
 * @param QSPIx QSPI instance.
 * @return The data value received from the FIFO.
 */
uint32_t QSPI_GetIndirectData(QSPI_Type * QSPIx);

/*!
 * @brief Set the QSPI indirect write configure.
 *
 * After configuration, the indirect write process will start.
 *
 * @param QSPIx QSPI instance.
 * @param conf Pointer to the config structure. See to @ref QSPI_IndirectXferConf_Type.
 * @return None.
 */
void QSPI_SetIndirectWriteConf(QSPI_Type * QSPIx, QSPI_IndirectXferConf_Type * conf);

/*!
 * @brief Put the data into FIFO of the QSPI module.
 *
 * @param QSPIx QSPI instance.
 * @param value Data value to be send through the FIFO.
 * @return None.
 */
void QSPI_PutIndirectData(QSPI_Type * QSPIx, uint32_t value);

/*!
 * @brief Enable the QSPI XIP mode.
 *
 * @param QSPIx QSPI instance.
 * @param enable 'true' to enable XIP, 'false' to disable XIP.
 * @return None.
 */
void QSPI_EnableXIP(QSPI_Type * QSPIx, bool enable);

/*!
 * @brief Set the QSPI enbale XIP mode after quit indirect mode.
 *
 * @param QSPIx QSPI instance.
 * @param enable 'true' to enable XIP, 'false' to disable XIP.
 * @return None.
 */
void QSPI_SetIndirectAutoXIP(QSPI_Type * QSPIx, bool enable);

/*!
 * @brief Get the current status flags of the QSPI module.
 *
 * @param QSPIx QSPI instance.
 * @return Status flags. See to @ref QSPI_STATUS.
 */
uint32_t QSPI_GetStatus(QSPI_Type * QSPIx);

/*!
 * @brief Clear the interrupts status flags of the QSPI module.
 *
 * @param QSPIx QSPI instance.
 * @param status The mask codes of the indicated status flags to be cleared.
 * @return Status flags. See to @ref QSPI_INT.
 */
void QSPI_ClearStatus(QSPI_Type * QSPIx, uint32_t status);

/*!
 * @brief Gets the current operating mode.
 *
 * @param QSPIx QSPI instance.
 * @return Operation, See to @ref QSPI_OpMode_Type.
 */
uint32_t QSPI_GetOpMode(QSPI_Type * QSPIx);

/*!
 * @brief Enable interrupts of the QSPI module.
 *
 * @param QSPIx QSPI instance.
 * @param interrupts Interrupt code masks. See to @ref QSPI_INT.
 * @param enable 'true' to enable the indicated interrupts, 'false' to disable the indicated interrupts.
 * @return None.
 */
void QSPI_EnableInterrupts(QSPI_Type * QSPIx, uint32_t interrupts, bool enable);

/*!
 * @brief Enable DMA request of the QSPI module.
 *
 * @param QSPIx QSPI instance.
 * @param dmas dma request code masks. See to @ref QSPI_DMA.
 * @param enable 'true' to enable the indicated request, 'false' to disable the indicated request.
 * @return None.
 */
void QSPI_EnableDMA(QSPI_Type * QSPIx, uint32_t dmas, bool enable);

/*!
 *@}
 */

#endif /* __HAL_QSPI_H__ */
