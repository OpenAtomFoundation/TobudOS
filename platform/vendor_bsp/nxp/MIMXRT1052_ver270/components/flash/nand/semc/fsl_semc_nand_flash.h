/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __FSL_SEMC_NAND_FLASH_H__
#define __FSL_SEMC_NAND_FLASH_H__

#include "fsl_common.h"
#include "fsl_semc.h"

/*!
 * @addtogroup semc_nand_flash
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief NAND Flash vendor type */
enum _nand_vendor_type_index
{
    kNandVendorType_Micron   = 0U,
    kNandVendorType_Spansion = 1U,
    kNandVendorType_Samsung  = 2U,
    kNandVendorType_Winbond  = 3U,
    kNandVendorType_Hynix    = 4U,
    kNandVendorType_Toshiba  = 5U,
    kNandVendorType_Macronix = 6U,
    kNandVendorType_Unknown  = 7U,
};

/*! @brief Parallel NAND Flash AC timing mode */
enum _nand_ac_timing_table_index
{
    kNandAcTimingTableIndex_ONFI_1p0_Mode0_10MHz = 0U,
    kNandAcTimingTableIndex_ONFI_1p0_Mode1_20MHz = 1U,
    kNandAcTimingTableIndex_ONFI_1p0_Mode2_28MHz = 2U,
    kNandAcTimingTableIndex_ONFI_1p0_Mode3_33MHz = 3U,
    kNandAcTimingTableIndex_ONFI_1p0_Mode4_40MHz = 4U,
    kNandAcTimingTableIndex_ONFI_1p0_Mode5_50MHz = 5U,
    kNandAcTimingTableIndex_ONFI_1p0_FastestMode = 6U,
};

/*! @brief Parallel NAND Flash commands */
enum _nand_onfi_command_set
{
    /* Must-have command */
    kNandDeviceCmd_ONFI_Reset                         = 0xFFU,
    kNandDeviceCmd_ONFI_ReadMode                      = 0x00U,
    kNandDeviceCmd_ONFI_ReadParameterPage             = 0xECU,
    kNandDeviceCmd_ONFI_ReadStatus                    = 0x70U,
    kNandDeviceCmd_ONFI_ReadPageSetup                 = 0x00U,
    kNandDeviceCmd_ONFI_ReadPageConfirm               = 0x30U,
    kNandDeviceCmd_ONFI_ChangeReadColumnSetup         = 0x05U,
    kNandDeviceCmd_ONFI_ChangeReadColumnEnhancedSetup = 0x06U,
    kNandDeviceCmd_ONFI_ChangeReadColumnConfirm       = 0xE0U,
    kNandDeviceCmd_ONFI_EraseBlockSetup               = 0x60U,
    kNandDeviceCmd_ONFI_EraseBlockConfirm             = 0xD0U,
    kNandDeviceCmd_ONFI_ProgramPageSetup              = 0x80U,
    kNandDeviceCmd_ONFI_ProgramPageConfirm            = 0x10U,
    /* Optional command */
    kNandDeviceCmd_ONFI_ReadStatusEnhanced = 0x78U,
    kNandDeviceCmd_ONFI_SetFeatures        = 0xEFU,
    kNandDeviceCmd_ONFI_GetFeatures        = 0xEEU,
};

/*! @brief Parallel NAND Flash ONFI Version */
typedef enum _nand_onfi_version
{
    kNandOnfiVersion_None = 0U,
    kNandOnfiVersion_1p0  = 1U,
    kNandOnfiVersion_2p0  = 2U,
    kNandOnfiVersion_3p0  = 3U,
    kNandOnfiVersion_4p0  = 4U,
} nand_onfi_version_t;

/*! @brief Parallel NAND Flash Status Command Type */
typedef enum _nand_status_command_type
{
    kNandStatusCommandType_Common   = 0U,
    kNandStatusCommandType_Enhanced = 1U,
} nand_status_command_type_t;

/*! @brief Parallel NAND Flash change read column Command Type */
typedef enum _nand_change_readcolumn_command_type
{
    kNandChangeReadColumnCommandType_Common   = 0U,
    kNandChangeReadColumnCommandType_Enhanced = 1U,
} nand_change_readcolumn_command_type_t;

/*! @brief NAND Flash ecc check type */
typedef enum _nand_ecc_check_type
{
    kNandEccCheckType_SoftwareECC = 0U,
    kNandEccCheckType_DeviceECC   = 1U,
} nand_ecc_check_type_t;

/*! @brief Parallel NAND Flash Ready check option */
typedef enum _nand_ready_check_option
{
    kNandReadyCheckOption_SR = 0U, /*!< Via Status Register */
    kNandReadyCheckOption_RB = 1U, /*!< Via R/B# signal */
} nand_ready_check_option_t;

/*!@brief Parallel NAND ONFI parameter config */
typedef struct __nand_onfi_parameter_config
{
    /* Revision information and features block */
    uint32_t signature;      /*!< [0x000-0x003] */
    uint16_t revisionNumber; /*!< [0x004-0x005] */
    struct
    {
        uint16_t x16bitDataBusWidth : 1;
        uint16_t multipleLUNoperations : 1;
        uint16_t reserved : 14;
    } supportedFeatures; /*!< [0x006-0x007] */
    struct
    {
        uint16_t reserved0 : 2;
        uint16_t setGetfeatures : 1;
        uint16_t readStatusEnhanced : 1;
        uint16_t reserved1 : 2;
        uint16_t changeReadColumnEnhanced : 1;
        uint16_t reserved2 : 9;
    } optionalCommands;    /*!< [0x008-0x009] */
    uint8_t reserved0[22]; /*!< [0x00a-0x01f] */
    /* Manufacturer information block */
    uint8_t deviceManufacturer[12]; /*!< [0x020-0x02b] */
    uint8_t deviceModel[20];        /*!< [0x02c-0x03f] */
    uint8_t JEDECid;                /*!< [0x040-0x040] */
    uint8_t dataCode[2];            /*!< [0x041-0x042] */
    uint8_t reserved1[13];          /*!< [0x043-0x04f] */
    /* Memory organization block */
    uint32_t dataBytesPerPage;  /*!< [0x050-0x053] */
    uint16_t spareBytesPerPage; /*!< [0x054-0x055] */
    uint8_t reserved2[6];       /*!< [0x056-0x05b] */
    uint32_t pagesPerBlock;     /*!< [0x05c-0x05f] */
    uint32_t blocksPerLUN;      /*!< [0x060-0x063] */
    uint8_t LUNsPerDevice;      /*!< [0x064-0x064] */
    union
    {
        uint8_t addressCycles;
        struct
        {
            uint8_t rowAddressCycles : 4;
            uint8_t columnAddressCycles : 4;
        } addressCyclesStr;
    };                     /*!< [0x065-0x065] */
    uint8_t reserved3[26]; /*!< [0x066-0x07f] */
    /* Electrical parameters block */
    uint8_t reserved4; /*!< [0x080-0x080] */
    struct
    {
        uint8_t mode0 : 1;
        uint8_t mode1 : 1;
        uint8_t mode2 : 1;
        uint8_t mode3 : 1;
        uint8_t mode4 : 1;
        uint8_t mode5 : 1;
        uint8_t reserved : 2;
    } timingMode;                            /*!< [0x081-0x081] */
    uint8_t reserved5[3];                    /*!< [0x082-0x084] */
    uint8_t maxPageProgramTimeInUs[2];       /*!< [0x085-0x086] */
    uint8_t maxBlockEraseTimeInUs[2];        /*!< [0x087-0x088] */
    uint8_t maxPageReadTimeInUs[2];          /*!< [0x089-0x08a] */
    uint8_t minChangeColunmSetupTimeInNs[2]; /*!< [0x08b-0x08c] */
    uint8_t reserved6[23];                   /*!< [0x08d-0x0a3] */
    /* Vendor block */
    uint16_t vendorSpecificRevisionNumber; /*!< [0x0a4-0x0a5] */
    uint8_t reserved7[88];                 /*!< [0x0a6-0x0fd] */
    uint16_t integrityCRC;                 /*!< [0x0fe-0x0ff] */
} nand_onfi_parameter_config_t;

/*! @brief Parallel NAND ONFI feature config */
typedef struct __nand_onfi_feature_config
{
    uint8_t command;
    uint8_t address;
    uint8_t parameter[4];
    uint8_t reserved[2];
} nand_onfi_feature_config_t;

/*! @brief us delay function pointer */
typedef void (*delay_us)(uint32_t us);

/*! @brief SEMC NAND Flash Config block structure */
typedef struct _semc_mem_nand_config
{
    semc_nand_config_t *semcNandConfig; /*!< memory controller configuration, shoule bd configured with controller
                               configure structure. */
    uint32_t clkSrc_Hz;                 /*!< The async clock frequency */
    delay_us delayUS;                /*!< delay function pointer, application should prepare a delay function for it */
    nand_onfi_version_t onfiVersion; /*!< only onfi nand flash will be supported currently. */
    uint8_t readyCheckOption;        /*!< Set with enum type defined in "nand_ready_check_option_t" */
    nand_ecc_check_type_t eccCheckType; /*!< Soft/device ECC check. */
} semc_mem_nand_config_t;

/*!@brief NAND Flash handle info*/
typedef struct _semc_mem_nand_handle
{
    delay_us delayUS;           /*!< delay function pointer, application should prepare a delay function for it */
    uint32_t ctlAccessMemAddr1; /*!< Nand memory address for memory controller access. */
    uint32_t ctlAccessMemAddr2; /*!< Nand memory address for memory controller access. */
    uint8_t readyCheckOption;   /*!< Set with enum type defined in "nand_ready_check_option_t" */
    nand_ecc_check_type_t eccCheckType;      /*!< Soft/device ECC check. */
    uint8_t statusCommandType;               /*!< the command enhanced mode or normal command mode */
    uint8_t changeReadColumnType;            /*!< the change read column type. */
    uint8_t columnWidth;                     /*!< the Colum width setting in the controller. */
    bool isFeatureCommandSupport;            /*!< feature command support .*/
    uint32_t rowAddressToGetSR;              /*!< Row address for read status enhanced command */
    uint32_t pageReadTimeInUs_tR;            /*!< Page read time delay */
    uint32_t PageProgramTimeInUs_tPROG;      /*!< Page program time delay */
    uint32_t blockEraseTimeInUs_tBERS;       /*!< block erase time delay */
    uint32_t changeColumnSetupTimeInNs_tCCS; /*!< Change column setup time delay */
} semc_mem_nand_handle_t;
/*! @} */
#endif /* __FSL_SEMC_NAND_FLASH_H__ */
