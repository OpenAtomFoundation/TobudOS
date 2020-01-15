/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __FSL_NOR_FLASH_H__
#define __FSL_NOR_FLASH_H__

#include "fsl_common.h"

/*!
 * @addtogroup nor_flash_component
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief NOR Flash Config block structure */
typedef struct _nor_config
{
    void *memControlConfig; /*!< memory controller configuration, should be assigned to specific controller
                               configuration structure pointer.*/
    void *driverBaseAddr;   /*! Driver Base address. */
} nor_config_t;

/*!@brief NOR Flash handle info*/
typedef struct _nor_handle
{
    /*------------Common parameters used for normal NOR flash controller operation ----------*/
    void *driverBaseAddr;       /*! Driver Base address. */
    uint32_t bytesInPageSize;   /*!< Page size in byte of Serial NOR */
    uint32_t bytesInSectorSize; /*!< Minimun Sector size in byte supported by Serial NOR */
    uint32_t bytesInMemorySize; /*!< Memory size in byte of Serial NOR */
                                /*------------Specific parameters used for specific NOR flash controller ----------*/
    void *deviceSpecific;       /*!< Device specific control parameter */
} nor_handle_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @name NOR FLASH Driver
 * @{
 */

/*!
 * @brief Initialize NOR FLASH devices.
 *
 *  This function initialize NOR Flash controller and NOR Flash.
 *
 * @param config    NOR flash configuration.
 *        The "memControlConfig" and "driverBaseAddr" are controller specific structure.
 *        please set those two parameter with your Nand controller configuration structure type pointer.
 *        such as for SEMC:
 *
 *        spifi_mem_nor_config_t spifiNorconfig =
 *        {
 *            .....
 *        }
 *        nor_config_t config =
 *        {
 *            .memControlConfig = (void *)&spifiNorconfig;
 *            .driverBaseAddr   = (void *)SPIFI0;
 *        }
 * @param handle    The NOR Flash handler.
 * @retval execution status
 */
status_t Nor_Flash_Init(nor_config_t *config, nor_handle_t *handle);

/*!
 * @brief Read page data from NOR Flash.
 *
 * @param handle    The NOR Flash handler.
 * @param address  NOR flash start address to read data from.
 * @param buffer  NOR flash buffer to read data to.
 * @param length  NOR flash read length.
 * @retval execution status
 */
status_t Nor_Flash_Read(nor_handle_t *handle, uint32_t address, uint8_t *buffer, uint32_t length);

/*!
 * @brief Program page data to NOR Flash.
 *
 * @param handle    The NOR Flash handler.
 * @param address  The address to be programed.
 * @param buffer  The buffer to be programed to the page.
 * @retval execution status
 */
status_t Nor_Flash_Page_Program(nor_handle_t *handle, uint32_t address, uint8_t *buffer);

/*!
 * @brief Erase sector.
 *
 * @param handle    The NOR Flash handler.
 * @param address   The start address to be erased.
 * @param size_Byte Erase sector size.
 * @retval execution status
 */
status_t Nor_Flash_Erase_Sector(nor_handle_t *handle, uint32_t address, uint32_t size_Byte);

/*!
 * @brief Erase block.
 *
 * @param handle    The NOR Flash handler.
 * @param address   The start address to be erased.
 * @param size_Byte Erase block size.
 * @retval execution status
 */
status_t Nor_Flash_Erase_Block(nor_handle_t *handle, uint32_t address, uint32_t size_Byte);

/*!
 * @brief Erase Chip NOR Flash .
 *
 * @param handle    The NOR Flash handler.
 * @retval execution status
 */
status_t Nor_Flash_Erase_Chip(nor_handle_t *handle);

/*! @} */

#ifdef __cplusplus
}
#endif
/*! @} */
#endif /* __FSL_NOR_FLASH_H__ */
