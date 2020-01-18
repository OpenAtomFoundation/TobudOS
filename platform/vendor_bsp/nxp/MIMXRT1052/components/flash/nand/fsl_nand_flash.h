/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __FSL_NAND_FLASH_H__
#define __FSL_NAND_FLASH_H__

#include "fsl_common.h"

/*!
 * @addtogroup nand_flash_component
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief NAND Flash Config block structure */
typedef struct _nand_config
{
    void *memControlConfig; /*!< memory controller configuration, should be assigned to specific controller
                               configuration structure pointer.*/
    void *driverBaseAddr;   /*! Driver Base address. */
} nand_config_t;

/*!@brief NAND Flash handle info*/
typedef struct _nand_handle
{
    /*------------Common parameters used for normal nand flash controller operation ----------*/
    void *driverBaseAddr;          /*! Driver Base address. */
    uint8_t vendorType;            /*!< vendor type */
    uint32_t bytesInPageDataArea;  /*!< Bytes in page data area .*/
    uint32_t bytesInPageSpareArea; /*!< Bytes in page spare area .*/
    uint32_t pagesInBlock;         /*!< Pages in each block. */
    uint32_t blocksInPlane;        /*!< blocks in each plane. */
    uint32_t planesInDevice;       /*!< planes in each device .*/
    /*------------Specific parameters used for specific nand flash controller ----------*/
    void *deviceSpecific; /*!< Device specific control parameter */
} nand_handle_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @name NAND FLASH Driver
 * @{
 */

/*!
 * @brief Initialize NAND FLASH devices.
 *
 *  This function initialize NAND Flash controller and NAND Flash.
 *
 * @param config    Nand flash configuration.
 *        The "memControlConfig" and "driverBaseAddr" are controller specific structure.
 *        please set those two parameter with your Nand controller configuration structure type pointer.
 *        such as for SEMC:
 *
 *        semc_mem_nand_config_t semcNandconfig =
 *        {
 *            .....
 *        }
 *        nand_config_t config =
 *        {
 *            .memControlConfig = (void *)&semcNandconfig;
 *            .driverBaseAddr   = (void *)SEMC;
 *        }
 * @param handle    The NAND Flash handler.
 * @retval execution status
 */
status_t Nand_Flash_Init(nand_config_t *config, nand_handle_t *handle);

/*!
 * @brief Read page data from NAND Flash.
 *
 * @param handle    The NAND Flash handler.
 * @param pageIndex  Nand flash page index, range from 0 ~ xxx.
 * @param buffer  Nand flash buffer to read data to.
 * @param length  Nand flash read length.
 * @retval execution status
 */
status_t Nand_Flash_Read_Page(nand_handle_t *handle, uint32_t pageIndex, uint8_t *buffer, uint32_t length);

/*!
 * @brief Read page partial data from NAND Flash.
 *
 * @param handle    The NAND Flash handler.
 * @param pageIndex  Nand flash page index, range from 0 ~ xxx.
 * @param offset_bytes Offset to start read the page data.
 * @param buffer  Nand flash buffer to read data to.
 * @param length  Nand flash read length.
 * @retval execution status
 */
status_t Nand_Flash_Read_Page_Partial(
    nand_handle_t *handle, uint32_t pageIndex, uint32_t offset_bytes, uint8_t *buffer, uint32_t length);

/*!
 * @brief Program page data to NAND Flash.
 *
 * @param handle    The NAND Flash handler.
 * @param pageIndex  Nand flash page index, range from 0 ~ xxx.
 * @param src  The data to be programed to the page.
 * @param length  Nand flash read length.
 * @retval execution status
 */
status_t Nand_Flash_Page_Program(nand_handle_t *handle, uint32_t pageIndex, uint8_t *src, uint32_t length);

/*!
 * @brief Erase block NAND Flash.
 *
 * @param handle    The NAND Flash handler.
 * @param blockIndex  Nand flash block index to be erased, range from 0 ~ xxx.
 * @retval execution status
 */
status_t Nand_Flash_Erase_Block(nand_handle_t *handle, uint32_t blockIndex);

/*! @} */

#ifdef __cplusplus
}
#endif
/*! @} */
#endif /* __FSL_NAND_FLASH_H__ */
