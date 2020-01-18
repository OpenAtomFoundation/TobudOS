/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flexspi.h"
#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define NOR_CMD_INDEX_READ CMD_INDEX_READ               /*!< 0*/
#define NOR_CMD_INDEX_READSTATUS CMD_INDEX_READSTATUS   /*!< 1*/
#define NOR_CMD_INDEX_WRITEENABLE CMD_INDEX_WRITEENABLE /*!< 2*/
#define NOR_CMD_INDEX_ERASESECTOR 3                     /*!< 3*/
#define NOR_CMD_INDEX_PAGEPROGRAM CMD_INDEX_WRITE       /*!< 4*/
#define NOR_CMD_INDEX_CHIPERASE 5                       /*!< 5*/
#define NOR_CMD_INDEX_DUMMY 6                           /*!< 6*/

#define NOR_CMD_LUT_SEQ_IDX_READSTATUS_XPI \
    2 /*!< 2  Read status DPI/QPI/OPI sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_WRITEENABLE_XPI \
    4 /*!< 4  Write Enable DPI/QPI/OPI sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_RESTORE_NOCMD \
    14 /*!< 14 Restore 0-4-4/0-8-8 mode sequence id in lookupTable stored in config block*/

#define NOR_CMD_LUT_SEQ_IDX_READ 0U        /*!< Read LUT sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_READSTATUS 1U  /*!< Read Status LUT sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_WRITESTATUS 2U /*!< Write Status LUT sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_WRITEENABLE 3U /*!< Write enable LUT sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_WRITECONFIG \
    4U /*!< Write configuration LUT sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_RESTORESPI \
    5U /*!< Restore standard SPI mode LUT sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_READCONFIG \
    6U /*!< Read configuration LUT sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_ERASESECTOR 7U /*!< Erase sector LUT sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_READID 8U /*!< Read manifacture ID LUT sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM 9U /*!< Page program LUT sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_CHIPERASE 11U  /*!< Chip erase LUT sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_ERASEBLOCK 12U /*!< Block erase LUT sequence id in lookupTable stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_READ_SFDP \
    13U /*!< Read SFDP information sequence id in lookupTable id stored in config block*/
#define NOR_CMD_LUT_SEQ_IDX_EXIT_NOCMD \
    15U /*!< Exit 0-4-4/0-8-8 mode sequence id in lookupTable stored in config blobk*/

/* Flexspi nor status */
enum
{
    kStatus_FLEXSPINOR_ProgramFail =
        MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 0), /*!< Status for Page programming failure*/
    kStatus_FLEXSPINOR_EraseSectorFail =
        MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 1), /*!< Status for Sector Erase failure*/
    kStatus_FLEXSPINOR_EraseAllFail = MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 2), /*!< Status for Chip Erase failure*/
    kStatus_FLEXSPINOR_WaitTimeout  = MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 3), /*!< Status for timeout*/
    kStatus_FlexSPINOR_NotSupported = MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 4), /*!< Not support*/
    kStatus_FlexSPINOR_WriteAlignmentError =
        MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 5), /*!< Status for Alignement error*/
    kStatus_FlexSPINOR_CommandFailure =
        MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 6), /*!< Status for Erase/Program Verify Error*/
    kStatus_FlexSPINOR_SFDP_NotFound = MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 7), /*!< Status for SFDP read failure*/
    kStatus_FLEXSPINOR_Unsupported_SFDP_Version =
        MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 8), /*!< Status for Unrecognized SFDP version*/
    kStatus_FLEXSPINOR_Flash_NotFound =
        MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 9), /*!< Status for Flash detection failure*/
    kStatus_FLEXSPINOR_DTRRead_DummyProbeFailed =
        MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 10), /*!< Status for DDR Read dummy probe failure*/
    kStatus_FLEXSPINOR_UnsupportedFunctionParameterType =
        MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 11), /*!< Status for DDR Read dummy probe failure*/
    kStatus_FLEXSPINOR_UnsupportedOctalModeEnterMethod =
        MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 12), /*!< Unsupported method into octal mode*/
    kStatus_FLEXSPINOR_EnterOctalModeFailed =
        MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 13), /*!< Enter octal mode failure*/
    kStatus_FLEXSPINOR_UnprotectMemoryFailed =
        MAKE_STATUS(kStatusGroup_SDK_FLEXSPINOR, 14), /*!< Unprotect memory failure*/
};

/*! @brief Serial NOR device type */
typedef enum _serial_nor_device_type
{
    kSerialNorCfgOption_Tag                         = 0x0E, /*!< Device type tag*/
    kSerialNorCfgOption_DeviceType_ReadSFDP_SDR     = 0,    /*!< Read SFDP in SDR mode*/
    kSerialNorCfgOption_DeviceType_ReadSFDP_DDR     = 1,    /*!< Read SFDP in DDR mode*/
    kSerialNorCfgOption_DeviceType_HyperFLASH1V8    = 2,    /*!< Hyper flash with 1.8v*/
    kSerialNorCfgOption_DeviceType_HyperFLASH3V0    = 3,    /*!< Hyper flash with 3.0v*/
    kSerialNorCfgOption_DeviceType_MacronixOctalDDR = 4,    /*!< Macronix octal flash in DDR mode*/
    kSerialNorCfgOption_DeviceType_MacronixOctalSDR = 5,    /*!< Macronix octal flash in SDR mode*/
    kSerialNorCfgOption_DeviceType_MicronOctalDDR   = 6,    /*!< Micron octal flash in DDR mode*/
    kSerialNorCfgOption_DeviceType_AdestoOctalDDR   = 8,    /*!< Adesto octal flash in DDR mode*/
} serial_nor_device_type_t;

/*! @brief Serial NOR transfer mode */
typedef enum _serial_nor_transfer_mode
{
    kSerialNorTransferMode_SDR = 0, /*!< Transfer rate is SDR mode*/
    kSerialNorTransferMode_DDR = 1, /*!< Transfer rate is DDR mode*/
} serial_nor_transfer_mode_t;

/*! @brief Serial NOR command mode */
typedef enum _serial_nor_command_mode
{
    kSerialNorCommandMode_1_1_1    = 0, /*!< Command mode is 1-1-1 mode*/
    kSerialNorCommandMode_2_2_2    = 1, /*!< Command mode is 2-3-2 mode*/
    kSerialNorCommandMode_4s_4s_4s = 2, /*!< Command mode is 4s-4s-4s mode*/
    kSerialNorCommandMode_4d_4d_4d = 3, /*!< Command mode is 4d-4d-4d mode*/
    kSerialNorCommandMode_8d_8d_8d = 4, /*!< Command mode is 8s-8s-8s mode*/
    kSerialNorCommandMode_8s_8s_8s = 5, /*!< Command mode is 8d-8d-8d mode*/
    kSerialNorCommandMode_max      = 6, /*!< Command mode maximum limit*/
} serial_nor_command_mode_t;

/*! @brief Serial NOR quad mode status */
typedef enum _serial_nor_quad_mode
{
    kSerialNorQuadMode_NotConfig            = 0, /*!< Quad mode do not configure*/
    kSerialNorQuadMode_StatusReg1_Bit6      = 1, /*!< Enable quad mode with status register 1 bit 6*/
    kSerialNorQuadMode_StatusReg2_Bit1      = 2, /*!< Enable quad mode with status register 2 bit 1*/
    kSerialNorQuadMode_StatusReg2_Bit1_0x31 = 3, /*!< Enable quad mode with status register 2 bit 1(0x31)*/
    kSerialNorQuadMode_StatusReg2_Bit7      = 4, /*!< Enable quad mode with status register 2 bit 7*/
} serial_nor_quad_mode_t;

/*! @brief Serial NOR enhance mode */
typedef enum _serial_nor_enhance_mode
{
    kSerialNorEnhanceMode_Disabled         = 0, /*!< Disable enhance mode*/
    kSerialNorEnhanceMode_0_4_4_Mode       = 1, /*!< Enable enhance mode with 0-4-4*/
    kSerialNorEnhanceMode_InternalLoopback = 2, /*!< Enable enhance mode with internal loopback*/
} serial_nor_enhance_mode_t;

/*! @brief Serial NOR configuration block */
typedef struct _flexspi_memory_config
{
    flexspi_device_config_t deviceConfig; /*!< Device configuration structure */
    flexspi_port_t devicePort;           /*!< Device connected to which port, SS0_A means port A1, SS0_B means port B1*/
    serial_nor_device_type_t deviceType; /*!< Serial nor flash device type */
    serial_nor_command_mode_t CurrentCommandMode; /*!< Serial nor flash corrent command mode */
    serial_nor_transfer_mode_t transferMode;      /*!< Transfer rate is SDR or DDR mode */
    serial_nor_quad_mode_t quadMode;              /*!< Serial nor flash quad mode setting. */
    serial_nor_enhance_mode_t enhanceMode;        /*!< Serial nor flash performance enhance mode setting. */
    flexspi_pad_t commandPads;                    /*!< Command padS setting. */
    flexspi_pad_t queryPads;                      /*!< SFDP read pads setting. */
    uint8_t statusOverride;        /*!< Override status register value during device mode configuration. */
    uint32_t controllerMiscOption; /*!< Controller Misc Options, see Misc feature bit definitions for more*/
    uint32_t bytesInPageSize;      /*!< Page size in byte of Serial NOR */
    uint32_t bytesInSectorSize;    /*!< Minimun Sector size in byte supported by Serial NOR */
    uint32_t bytesInMemorySize;    /*!< Memory size in byte of Serial NOR */
    uint16_t busyOffset; /*!< Busy offset, valid value: 0-31, only need for check option kNandReadyCheckOption_RB */
    uint16_t busyBitPolarity;   /*!< Busy flag polarity, 0 - busy flag is 1 when flash device is busy, 1 -busy flag is 0
                                     when flash device is busy, only need for check option kNandReadyCheckOption_RB */
    uint8_t ipcmdSerialClkFreq; /*!< Clock frequency for IP command*/
    uint32_t lookupTable[64];   /*!< Lookup table holds Flash command sequences */
    uint8_t serialNorType;      /*!< Serial NOR Flash type: 0/1/2/3*/
    uint8_t needExitNoCmdMode;  /*!< Need to exit NoCmd mode before other IP command*/
    bool halfClkForNonReadCmd;  /*!< Half the Serial Clock for non-read command: true/false*/
    uint8_t needRestoreNoCmdMode; /*!< Need to Restore NoCmd mode after IP commmand execution*/
    bool deviceModeCfgEnable;     /*!< Device Mode Configure enable flag, true - Enable, false - Disable*/
    uint32_t manufacturerId;      /*!< Manufacturer ID that each vendor has a unique ID depend on JESD216 spec*/
} flexspi_mem_config_t;

/*! @brief NOR Flash handle info */
typedef struct _flexspi_mem_nor_handle
{
    flexspi_port_t port;      /*!< Device connected to which port, SS0_A means port A1, SS0_B means port B1*/
    uint16_t busyOffset;      /*!< Busy offset, valid value: 0-31, only need for check option kNORReadyCheckOption_RB */
    uint16_t busyBitPolarity; /*!< Busy flag polarity, 0 - busy flag is 1 when flash device is busy, 1 -busy flag is 0
                                   when flash device is busy, only need for check option kNORReadyCheckOption_RB */
    uint32_t configuredFlashSize; /*!< Flash size configured by user through device configuration. */
} flexspi_mem_nor_handle_t;
