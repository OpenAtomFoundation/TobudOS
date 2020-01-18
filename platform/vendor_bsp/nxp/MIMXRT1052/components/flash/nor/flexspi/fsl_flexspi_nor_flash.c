/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "fsl_nor_flash.h"
#include "fsl_flexspi_nor_flash.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_24BIT_ADDRESSING_SIZE (16UL * 1024UL * 1024UL)
#define NOR_CMD_LUT_FOR_IP_CMD 1
#define SFDP_JESD216_SIGNATURE 0x50444653U
#define SFDP_JESD216_MAJOR 1U
#define SFDP_JESD216_MINOR 0U
#define SFDP_JESD216A_MINOR 5U
#define SFDP_JESD216B_MINOR 6U
#define SFDP_JESD216C_MINOR 7U
#define SFDP_JESD216D_MINOR 8U

#define SPI_NOR_MAX_ID_LEN 6
#define SPI_NOR_MAX_ADDR_WIDTH 4

static flexspi_mem_nor_handle_t flexspiMemHandle;

/*! @brief Misc feature bit definitions. */
enum
{
    kFlexSpiMiscOffset_DiffClkEnable            = 0, /*!< Bit for Differential clock enable*/
    kFlexSpiMiscOffset_Ck2Enable                = 1, /*!< Bit for CK2 enable*/
    kFlexSpiMiscOffset_ParallelEnable           = 2, /*!< Bit for Parallel mode enable*/
    kFlexSpiMiscOffset_WordAddressableEnable    = 3, /*!< Bit for Word Addressable enable*/
    kFlexSpiMiscOffset_SafeConfigFreqEnable     = 4, /*!< Bit for Safe Configuration Frequency enable*/
    kFlexSpiMiscOffset_PadSettingOverrideEnable = 5, /*!< Bit for Pad setting override enable*/
    kFlexSpiMiscOffset_DdrModeEnable            = 6, /*!< Bit for DDR clock confiuration indication.*/
};

/*! @brief Flash read sample clock source. */
enum
{
    kFlexSPIReadSampleClk_LoopbackInternally      = 0,
    kFlexSPIReadSampleClk_LoopbackFromDqsPad      = 1,
    kFlexSPIReadSampleClk_LoopbackFromSckPad      = 2,
    kFlexSPIReadSampleClk_ExternalInputFromDqsPad = 3,
};

/*! @brief NOR flash command sequence instruction. */
enum
{
    kSerialNorCmd_BasicRead_3B = 0x03,
    kSerialNorCmd_BasicRead_4B =
        0x13, /* See JESD216D 6.6.3(JEDEC 4-byte Address Instruction Table: 1th DWORD) for more details. */
    kSerialNorCmd_PageProgram_1_1_1_3B = 0x02,
    kSerialNorCmd_PageProgram_1_1_1_4B =
        0x12, /* See JESD216D 6.6.3(JEDEC 4-byte Address Instruction Table: 1th DWORD) for more details. */
    kSerialNorCmd_PageProgram_1_4_4_4B =
        0x3E, /* See JESD216D 6.6.3(JEDEC 4-byte Address Instruction Table: 1th DWORD) for more details. */
    kSerialNorCmd_PageProgram_1_1_4_4B =
        0x34, /* See JESD216D 6.6.3(JEDEC 4-byte Address Instruction Table: 1th DWORD) for more details. */
    kSerialNorCmd_kFLEXSPI_Command_READ_SDR_1_4_4_3B = 0xEB,
    kSerialNorCmd_kFLEXSPI_Command_READ_DDR_1_4_4_3B = 0xED,
    kSerialNorCmd_kFLEXSPI_Command_READ_SDR_1_1_1_4B =
        0x0C, /* See JESD216D 6.6.3(JEDEC 4-byte Address Instruction Table: 1th DWORD) for more details. */
    kSerialNorCmd_kFLEXSPI_Command_READ_SDR_1_4_4_4B =
        0xEC, /* See JESD216D 6.6.3(JEDEC 4-byte Address Instruction Table: 1th DWORD) for more details. */
    kSerialNorCmd_kFLEXSPI_Command_READ_SDR_1_1_4_4B =
        0x6C, /* See JESD216D 6.6.3(JEDEC 4-byte Address Instruction Table: 1th DWORD) for more details. */
    kSerialNorCmd_kFLEXSPI_Command_READ_DDR_1_1_1_4B =
        0x0E, /* See JESD216D 6.6.3(JEDEC 4-byte Address Instruction Table: 1th DWORD) for more details. */
    kSerialNorCmd_kFLEXSPI_Command_READ_DDR_1_4_4_4B =
        0xEE, /* See JESD216D 6.6.3(JEDEC 4-byte Address Instruction Table: 1th DWORD) for more details. */
    kSerialNorCmd_ChipErase    = 0x60, /* 0x60 or 0xC7 */
    kSerialNorCmd_WriteEnable  = 0x06, /*See instruction set table in any serial flash RM. */
    kSerialNorCmd_WriteDisable = 0x04, /*See instruction set table in any serial flash RM. */
    kSerialNorCmd_WriteStatusReg1 =
        0x01, /* See JESD216D 6.4.18(JEDEC Basic Flash Parameter Table: 15th DWORD) for more details. */
    kSerialNorCmd_ReadStatusReg1 =
        0x05, /* See JESD216D 6.4.18(JEDEC Basic Flash Parameter Table: 15th DWORD) for more details. */
    kSerialNorCmd_WriteStatusReg2Bit1 =
        0x01, /* See JESD216D 6.4.18(JEDEC Basic Flash Parameter Table: 15th DWORD) for more details. */
    kSerialNorCmd_WriteStatusReg2Bit7 =
        0x3E, /* See JESD216D 6.4.18(JEDEC Basic Flash Parameter Table: 15th DWORD) for more details. */
    kSerialNorCmd_ReadStatusReg2Bit1 =
        0x35, /* See JESD216D 6.4.18(JEDEC Basic Flash Parameter Table: 15th DWORD) for more details. */
    kSerialNorCmd_ReadStatusReg2Bit7 =
        0x3F, /* See JESD216D 6.4.18(JEDEC Basic Flash Parameter Table: 15th DWORD) for more details. */
    kSerialNorCmd_ReadFlagReg = 0x70,
    kSerialNorCmd_ReadId      = 0x9F,
};

/*! @brief Octal serial NOR flash command sequence instruction. */
enum
{
    kOctalSerialNorCmd_Read_8s_8s_8s_4B_1st = 0xEC,
    kOctalSerialNorCmd_Read_8s_8s_8s_4B_2nd = 0x13,
    kOctalSerialNorCmd_Read_8d_8d_8d_4B_1st = 0xEE,
    kOctalSerialNorCmd_Read_8d_8d_8d_4B_2nd = 0x11,
    kOctalSerialNorCmd_PageProgram_4B_1st   = 0x12,
    kOctalSerialNorCmd_PageProgram_4B_2nd   = 0xED,
    kOctalSerialNorCmd_SectorErase_4B_1st   = 0x21,
    kOctalSerialNorCmd_SectorErase_4B_2nd   = 0xDE,
    kOctalSerialNorCmd_BlockErase_4B_1st    = 0xDC, /* 0xDC or 0xD8 for single mode, 0xDC for octal mode */
    kOctalSerialNorCmd_BlockErase_4B_2nd    = 0x23,
    kOctalSerialNorCmd_ChipErase_1st        = 0x60, /* 0x60 or 0xC7 */
    kOctalSerialNorCmd_ChipErase_2nd        = 0x9F, /* 0x9F or 0x38 */
    kOctalSerialNorCmd_WriteEnable_1st      = 0x06,
    kOctalSerialNorCmd_WriteEnable_2nd      = 0xF9,
    kOctalSerialNorCmd_WriteDisable_1st     = 0x04,
    kOctalSerialNorCmd_WriteDisable_2nd     = 0xFB,
    kOctalSerialNorCmd_WriteProtectSel_1st  = 0x68,
    kOctalSerialNorCmd_WriteProtectSel_2nd  = 0x97,
    kOctalSerialNorCmd_ReadStatusReg_1st    = 0x05,
    kOctalSerialNorCmd_ReadStatusReg_2nd    = 0xFA,
    kOctalSerialNorCmd_ReadConfigReg1_1st   = 0x15,
    kOctalSerialNorCmd_ReadConfigReg1_2nd   = 0xEA,
    kOctalSerialNorCmd_ReadConfigReg2_1st   = 0x71,
    kOctalSerialNorCmd_ReadConfigReg2_2nd   = 0x8E,
    kOctalSerialNorCmd_WriteStatusReg_1st   = 0x01,
    kOctalSerialNorCmd_WriteStatusReg_2nd   = 0xFE,
    kOctalSerialNorCmd_WriteConfigReg1_1st  = 0x01,
    kOctalSerialNorCmd_WriteConfigReg1_2nd  = 0xFE,
    kOctalSerialNorCmd_WriteConfigReg2_1st  = 0x72,
    kOctalSerialNorCmd_WriteConfigReg2_2nd  = 0x8D,
    kOctalSerialNorCmd_ReadSecurityReg_1st  = 0x2B,
    kOctalSerialNorCmd_ReadSecurityReg_2nd  = 0xD4,
    kOctalSerialNorCmd_WriteSecurityReg_1st = 0x2F,
    kOctalSerialNorCmd_WriteSecurityReg_2nd = 0xD0,
};

/*! @brief octal serial NOR flash OPI enable. */
enum
{
    kOctalSerialNor_SPIModeMacronix = 0x00,
    kOctalSerialNor_STRModeMacronix = 0x01,
    kOctalSerialNor_DTRModeMacronix = 0x02,

    kOctalSerialNor_SPIModeAdesto = 0x00,
    kOctalSerialNor_STRModeAdesto = 0x08,
    kOctalSerialNor_DTRModeAdesto = 0x88,
};

enum
{
    kSerialNorQpiMode_NotConfig   = 0,
    kSerialNorQpiMode_Cmd_0x38    = 1,
    kSerialNorQpiMode_Cmd_0x38_QE = 2,
    kSerialNorQpiMode_Cmd_0x35    = 3,
    kSerialNorQpiMode_Cmd_0x71    = 4,
    kSerialNorQpiMode_Cmd_0x61    = 5,
};

enum
{
    kSerialNorType_StandardSPI, /*!< Device that support Standard SPI and Extended SPI mode*/
    kSerialNorType_HyperBus,    /*!< Device that supports HyperBus only*/
    kSerialNorType_XPI,         /*!< Device that works under DPI, QPI or OPI mode*/
    kSerialNorType_NoCmd, /*!< Device that works under No command mode (XIP mode/Performance Enhance mode/continous
                             read*/
};

typedef struct _lut_seq
{
    uint32_t lut[4];
} lut_seq_t;

enum
{
    kSerialNOR_IndividualMode = false,
    kSerialNOR_ParallelMode   = true,
};

enum
{
    kFlexSpiSerialClk_Update,
    kFlexSpiSerialClk_Restore,
};

/*! @brief NOR flash command sequence instruction for SFDP. */
enum
{
    kSerialFlash_ReadSFDP    = 0x5A,
    kSerialFlash_ReadJEDECId = 0x9F,
};

/*! @brief NOR flash manufacturer ID. */
enum
{
    kSerialNorFlash_MicronManufacturerID    = 0x20,
    kSerialNorFlash_MacronixManufacturerID  = 0xC2,
    kSerialNorFlash_Adesto128ManufacturerID = 0x1F,
    kSerialNorFlash_Adesto32ManufacturerID  = 0x43,
    kSerialNorFlash_IssiManufacturerID      = 0x9D,
    kSerialNorFlash_WinbondManufacturerID   = 0xEF,
};

/*! @brief Octal serial NOR flash command sequence instruction for SFDP. */
enum
{
    kOctalSerialFlash_ReadSFDP_1st    = 0x5A,
    kOctalSerialFlash_ReadSFDP_2nd    = 0x60,
    kOctalSerialFlash_ReadJEDECId_1st = 0x9F,
    kOctalSerialFlash_ReadJEDECId_2nd = 0xA5,
};

/*! @brief NOR flash SFDP header. */
typedef struct _sfdp_header
{
    uint32_t signature;
    uint8_t minor_rev;
    uint8_t major_rev;
    uint8_t param_hdr_num;
    uint8_t reserved;
} sfdp_header_t;

/*! @brief Function specific parameter table ID assignments. */
enum
{
    kParameterID_BasicSpiProtocal             = 0xFF00, /* Basic Flash Parameter Table */
    kParameterID_SectorMap                    = 0xFF81, /* Sector Map Table */
    kParameterID_RPMC                         = 0xFF03, /* Replay Protected Monotonic Counters */
    kParameterID_4ByteAddressInstructionTable = 0xFF84, /* 4-byte Address Instruction Table */
    kParameterID_XSPIProfileVersion1          = 0xFF05, /* eXtended Serial Peripheral Interface (xSPI) Profile 1.0 */
    kParameterID_XSPIProfileVersion2          = 0xFF06, /* eXtended Serial Peripheral Interface (xSPI) Profile 2.0 */
    kParameterID_SCCRM                        = 0xFF87, /* Status, Control and Configuration Register Map */
    kParameterID_SCCRMForMultiChipSPIMemoryDecive =
        0xFF88, /* Status, Control and Configuration Register Map Offsets for Multi-Chip SPI Memory Devices */
    kParameterID_SCCRMForxSPIProfileVersion2 =
        0xFF09, /* Status, Control and Configuration Register Map for xSPI Profile 2.0 */
    kParameterID_CommandSeqToOctalDDRMode = 0xFF0A, /* Command Sequences to change to Octal DDR (8D-8D-8D) mode */
    kParameterID_MSPT                     = 0xFF8B, /* Long Latency NVM Media Specific Parameter Table (MSPT) */
    kParameterID_ReservedTableAssignment  = 0xFF0C, /* Reserved for next Function Specific Table assignment */
};

/*! @brief SFDP Parameter Header. */
typedef struct _sfdp_parameter_header
{
    uint8_t parameter_id_lsb;
    uint8_t minor_rev;
    uint8_t major_rev;
    uint8_t table_length_in_32bit;
    uint8_t parameter_table_pointer[3];
    uint8_t parameter_id_msb;
} sfdp_parameter_header_t;

#define SFDP_PARAM_HEADER_ID(p) (((uint32_t)(p).parameter_id_msb << 8) | (p).parameter_id_lsb)
#define SFDP_PARAM_HEADER_PTP(p)                                                                          \
    (((uint32_t)(p).parameter_table_pointer[2] << 16) | ((uint32_t)(p).parameter_table_pointer[1] << 8) | \
     ((uint32_t)(p).parameter_table_pointer[0] << 0))

/*! @brief Basic Flash Parameter Table. */
typedef struct _jedec_basic_flash_param_table
{
    /* 1st DWORD */
    struct
    {
        uint32_t erase_size : 2;
        uint32_t write_granularity : 1;
        uint32_t vsr_block_protect : 1;
        uint32_t vsr_write_enable : 1;
        uint32_t unused0 : 3;
        uint32_t erase4k_inst : 8;
        uint32_t support_1_1_2_fast_read : 1;
        uint32_t address_bytes : 2;
        uint32_t support_dtr_clocking : 1;
        uint32_t support_1_2_2_fast_read : 1;
        uint32_t supports_1_4_4_fast_read : 1;
        uint32_t support_1_1_4_fast_read : 1;
        uint32_t unused1 : 9;
    } misc;

    /* 2nd DWORD */
    uint32_t flash_density;

    /* 3rd DWORD */
    struct
    {
        uint32_t dummy_clocks_1_4_4_read : 5;
        uint32_t mode_clocks_1_4_4_read : 3;
        uint32_t inst_1_4_4_read : 8;
        uint32_t dummy_clocks_1_1_4_read : 5;
        uint32_t mode_clocks_1_1_4_read : 3;
        uint32_t inst_1_1_4_read : 8;
    } read_1_4_info;

    /* 4th DWORD */
    struct
    {
        uint32_t dummy_clocks_1_1_2_read : 5;
        uint32_t mode_clocks_1_1_2_read : 3;
        uint32_t inst_1_1_2_read : 8;
        uint32_t dummy_clocks_1_2_2_read : 5;
        uint32_t mode_clocks_1_2_2_read : 3;
        uint32_t inst_1_2_2_read : 8;
    } read_1_2_info;

    /* 5th DWORD */
    struct
    {
        uint32_t support_2_2_2_fast_read : 1;
        uint32_t reserved0 : 3;
        uint32_t support_4_4_4_fast_read : 1;
        uint32_t reserved1 : 27;
    } read_22_44_check;

    /* 6th DWORD */
    struct
    {
        uint32_t reserved0 : 16;
        uint32_t dummy_clocks_2_2_2_read : 5;
        uint32_t mode_clocks_2_2_2_read : 3;
        uint32_t inst_2_2_2_read : 8;
    } read_2_2_info;

    /* 7th DWORD */
    struct
    {
        uint32_t reserved0 : 16;
        uint32_t dummy_clocks_4_4_4_read : 5;
        uint32_t mode_clocks_4_4_4_read : 3;
        uint32_t inst_4_4_4_read : 8;
    } read_4_4_info;

    /* 8st and 9th DWORD */
    struct
    {
        uint8_t size;
        uint8_t inst;
    } erase_info[4];

    /* 10th DWORD */
    struct
    {
        uint32_t multiplier_from_typical_to_max : 4;
        uint32_t type_1_count : 5;
        uint32_t type_1_units : 2;
        uint32_t type_2_count : 5;
        uint32_t type_2_units : 2;
        uint32_t type_3_count : 5;
        uint32_t type_3_units : 2;
        uint32_t type_4_count : 5;
        uint32_t type_4_units : 2;
    } erase_timing;

    /* 11th DWORD */
    struct
    {
        uint32_t program_max_time : 4;
        uint32_t page_size : 4;
        uint32_t page_program_time_count : 5;
        uint32_t page_program_time_units : 1;
        uint32_t byte_program_time_first_count : 4;
        uint32_t byte_program_time_first_units : 1;
        uint32_t byte_program_time_additional_count : 4;
        uint32_t byte_program_time_additional_units : 1;
        uint32_t chip_erase_time_count : 5;
        uint32_t chip_erase_time_units : 2;
        uint32_t reserved0 : 1;
    } chip_erase_progrm_info;

    /* 12th DWORD */
    struct
    {
        uint32_t prohibited_oprations_during_program_suspend : 4;
        uint32_t prohibited_oprations_during_erase_suspend : 4;
        uint32_t reserved0 : 1;
        uint32_t program_resume_2_suspend_interval : 4;
        uint32_t suspend_ipp_max_latency_count : 5;
        uint32_t suspend_ipp_max_latency_units : 2;
        uint32_t erase_resume_2_suspend_interval_count : 4;
        uint32_t suspend_ipe_max_latency_count : 5;
        uint32_t suspend_ipe_max_latency_units : 2;
        uint32_t support_suspend_resume : 1;
    } suspend_resume_info;

    /* 13th DWORD */
    struct
    {
        uint32_t program_resume_inst : 8;
        uint32_t program_suspend_inst : 8;
        uint32_t resume_inst : 8;
        uint32_t suspend_inst : 8;
    } suspend_resume_inst;

    /* 14th DWORD */
    struct
    {
        uint32_t reserved0 : 2;
        uint32_t busy_status_polling : 6;
        uint32_t delay_from_deep_powerdown_to_next_operation_count : 5;
        uint32_t delay_from_deep_powerdown_to_next_operation_units : 2;
        uint32_t exit_deep_powerdown_inst : 8;
        uint32_t enter_deep_powerdown_inst : 8;
        uint32_t support_deep_powerdown : 1;
    } deep_power_down_info;

    /* 15th DWORD */
    struct
    {
        uint32_t mode_4_4_4_disable_seq : 4;
        uint32_t mode_4_4_4_enable_seq : 5;
        uint32_t support_mode_0_4_4 : 1;
        uint32_t mode_0_4_4_exit_method : 6;
        uint32_t mode_0_4_4_entry_method : 4;
        uint32_t quad_enable_requirement : 3;
        uint32_t hold_reset_disable : 1;
        uint32_t reserved0 : 8;
    } mode_4_4_info;

    /* 16th DWORD */
    struct
    {
        uint32_t status_reg_write_enable : 7;
        uint32_t reserved0 : 1;
        uint32_t soft_reset_rescue_support : 6;
        uint32_t exit_4_byte_addressing : 10;
        uint32_t enter_4_byte_addrssing : 8;
    } mode_config_info;

    /* 17th DWORD */
    struct
    {
        uint32_t dummy_clocks_1_8_8_read : 5;
        uint32_t mode_clocks_1_8_8_read : 3;
        uint32_t inst_1_8_8_read : 8;
        uint32_t dummy_clocks_1_1_8_read : 5;
        uint32_t mode_clocks_1_1_8_read : 3;
        uint32_t inst_1_1_8_read : 8;
    } read_1_8_info;

    /* 18th DWORD */
    struct
    {
        uint32_t reserved0 : 18;
        uint32_t driver_strength : 5;
        uint32_t jedec_spi_protocal_reset : 1;
        uint32_t data_strobe_waveforms_str : 2;
        uint32_t data_strobe_support_qpi_str : 1;
        uint32_t data_strobe_support_qpi_dtr : 1;
        uint32_t reserved1 : 1;
        uint32_t octal_dtr_cmd_and_cmd_extension : 2;
        uint32_t byte_order_8_8_8 : 1;
    } octal_1_8_info;

    /* 19th DWORD */
    struct
    {
        uint32_t mode_8s_8s_8s_disable_seq : 4;
        uint32_t mode_8s_8s_8s_enable_seq : 4;
        uint32_t support_mode_0_8_8 : 1;
        uint32_t mode_0_8_8_exit_method : 6;
        uint32_t mode_0_8_8_entry_method : 4;
        uint32_t octal_enable_requirement : 3;
        uint32_t reserved0 : 9;
    } mode_8_8_info;

    /* 20th DWORD */
    struct
    {
        uint32_t max_speed_without_data_strobe_mode_4s_4s_4s : 4;
        uint32_t max_speed_with_data_strobe_mode_4s_4s_4s : 4;
        uint32_t max_speed_without_data_strobe_mode_4d_4d_4d : 4;
        uint32_t max_speed_with_data_strobe_mode_4d_4d_4d : 4;
        uint32_t max_speed_without_data_strobe_mode_8s_8s_8s : 4;
        uint32_t max_speed_with_data_strobe_mode_8s_8s_8s : 4;
        uint32_t max_speed_without_data_strobe_mode_8d_8d_8d : 4;
        uint32_t max_speed_with_data_strobe_mode_8d_8d_8d : 4;
    } max_operate_speed;
} jedec_basic_flash_param_table_t;

/*! @brief 4Byte Addressing Instruction Table. */
typedef struct _jedec_4byte_addressing_inst_table
{
    /* 1st DWORD */
    struct
    {
        uint32_t support_1_1_1_read : 1;
        uint32_t support_1_1_1_fast_read : 1;
        uint32_t support_1_1_2_fast_read : 1;
        uint32_t support_1_2_2_fast_read : 1;
        uint32_t support_1_1_4_fast_read : 1;
        uint32_t support_1_4_4_fast_read : 1;
        uint32_t support_1_1_1_page_program : 1;
        uint32_t support_1_1_4_page_program : 1;
        uint32_t support_1_4_4_page_program : 1;
        uint32_t support_erase_type1_size : 1;
        uint32_t support_erase_type2_size : 1;
        uint32_t support_erase_type3_size : 1;
        uint32_t support_erase_type4_size : 1;
        uint32_t support_1_1_1_dtr_read : 1;
        uint32_t support_1_2_2_dtr_read : 1;
        uint32_t support_1_4_4_dtr_read : 1;
        uint32_t support_volatile_sector_lock_read_cmd : 1;
        uint32_t support_volatile_sector_lock_write_cmd : 1;
        uint32_t support_nonvolatile_sector_lock_read_cmd : 1;
        uint32_t support_nonvolatile_sector_lock_write_cmd : 1;
        uint32_t support_1_1_8_fast_read : 1;
        uint32_t support_1_8_8_fast_read : 1;
        uint32_t support_1_8_8_dtr_read : 1;
        uint32_t support_1_1_8_page_program : 1;
        uint32_t support_1_8_8_page_program : 1;
        uint32_t reserved : 7;
    } cmd_4byte_support_info;

    /* 2nd DWORD */
    struct
    {
        uint8_t erase_inst[4];
    } erase_inst_info;
} jedec_4byte_addressing_inst_table_t;

/*! @brief xSPI profile 1.0. */
typedef struct _jedec_xspi_profile_1_table
{
    /* 1st DWORD */
    struct
    {
        uint32_t read_fast_wrapped_cmd : 8;
        uint32_t read_fast_cmd : 8;
        uint32_t reserved : 6;
        uint32_t write_reg_cmd_data_bytes : 1;
        uint32_t write_reg_cmd_additional_modifier_bytes : 1;
        uint32_t write_status_cfg_reg_cmd_additional_modifier_bytes : 1;
        uint32_t read_volatile_reg_cmd_latency : 1;
        uint32_t read_nv_reg_cmd_latency : 1;
        uint32_t read_reg_cmd_additional_modifier_bytes : 1;
        uint32_t read_status_reg_cmd_latency : 1;
        uint32_t read_status_reg_cmd_additional_modifier_bytes : 1;
        uint32_t dummy_cycles : 1;
        uint32_t address_bytes : 1;
    } cmd_8d_8d_8d_mode_info;

    /* 2nd DWORD */
    struct
    {
        uint32_t write_nv_reg_cmd : 8;
        uint32_t write_volatile_reg_cmd : 8;
        uint32_t read_nv_reg_cmd : 8;
        uint32_t read_volatile_reg_cmd : 8;
    } cmd_8d_8d_8d_inst;

    /* 3th DWORD */
    struct
    {
        uint32_t reserved1 : 10;
        uint32_t enter_default_protocol : 1;
        uint32_t soft_reset_enter_default_protocol : 1;
        uint32_t reset_enable : 1;
        uint32_t soft_reset : 1;
        uint32_t exit_deep_power_down : 1;
        uint32_t enter_deep_power_down : 1;
        uint32_t write_nv_reg : 1;
        uint32_t write_volatile_reg : 1;
        uint32_t write_reg : 1;
        uint32_t clear_flag_status_reg : 1;
        uint32_t write_status_cfg_reg : 1;
        uint32_t read_nv_reg : 1;
        uint32_t read_volatile_reg : 1;
        uint32_t read_reg : 1;
        uint32_t read_flag_status_reg : 1;
        uint32_t read_cfg_reg : 1;
        uint32_t erase_chip : 1;
        uint32_t erase_32kbytes : 1;
        uint32_t erase_4kbytes : 1;
        uint32_t setup_read_wrap : 1;
        uint32_t read_fast_wrapped : 1;
        uint32_t read_sfdp_8d_8d_8d : 1;
    } cmd_octal_support_info;

    /* 4th DWORD */
    struct
    {
        uint32_t reserved1 : 2;
        uint32_t bit_pattern_200mhz : 5;
        uint32_t dummy_cyclses_200mhz : 5;
        uint32_t reserved2 : 20;
    } dummy_cycles_200mhz_setting;

    /* 5th DWORD */
    struct
    {
        uint32_t reserved1 : 2;
        uint32_t bit_pattern_100mhz : 5;
        uint32_t dummy_cyclses_100mhz : 5;
        uint32_t bit_pattern_133mhz : 5;
        uint32_t dummy_cyclses_133mhz : 5;
        uint32_t bit_pattern_166mhz : 5;
        uint32_t dummy_cyclses_166mhz : 5;
    } dummy_cycles_100_133_166mhz_setting;
} jedec_xspi_profile_1_table_t;

/*! @brief Status, Control and Configuration Register Map. */
typedef struct _jedec_sccr_table
{
    /* 1st DWORD */
    uint32_t volatile_reg_address_offset;

    /* 2nd DWORD */
    uint32_t non_volatile_reg_address_offset;

    /* 3rd DWORD */
    struct
    {
        uint32_t dummy_cycles_1s_1s_1s_backup : 4;
        uint32_t reserved : 2;
        uint32_t dummy_cycles_8d_8d_8d : 4;
        uint32_t dummy_cycles_8s_8s_8s : 4;
        uint32_t dummy_cycles_4d_4d_4d : 4;
        uint32_t dummy_cycles_4s_4s_4s : 4;
        uint32_t dummy_cycles_2s_2s_2s : 4;
        uint32_t dummy_cycles_1s_1s_1s : 2;
        uint32_t address_bytes : 2;
        uint32_t support_wirte_control_cmd : 1;
        uint32_t support_read_control_cmd : 1;
    } volatile_reg_read_write_control_cmd;

    /* 4th DWORD */
    struct
    {
        uint32_t dummy_cycles_1s_1s_1s_backup : 4;
        uint32_t reserved : 2;
        uint32_t dummy_cycles_8d_8d_8d : 4;
        uint32_t dummy_cycles_8s_8s_8s : 4;
        uint32_t dummy_cycles_4d_4d_4d : 4;
        uint32_t dummy_cycles_4s_4s_4s : 4;
        uint32_t dummy_cycles_2s_2s_2s : 4;
        uint32_t dummy_cycles_1s_1s_1s : 2;
        uint32_t address_bytes : 2;
        uint32_t support_wirte_control_cmd : 1;
        uint32_t support_read_control_cmd : 1;
    } non_volatile_reg_read_write_control_cmd;

    /* 5th DWORD */
    struct
    {
        uint32_t write_access_cmd : 8;
        uint32_t read_access_cmd : 8;
        uint32_t dummy_cycle_select : 4;
        uint32_t support_4s_4s_4s : 1;
        uint32_t support_4d_4d_4d : 1;
        uint32_t support_8s_8s_8s : 1;
        uint32_t support_8d_8d_8d : 1;
        uint32_t wip_bit_position : 3;
        uint32_t wip_bit_exist : 1;
        uint32_t bit_access_use_addrss : 1;
        uint32_t reserved : 1;
        uint32_t wip_polarity : 1;
        uint32_t wip_available : 1;
    } write_in_progress_info;

    /* 6th DWORD */
    struct
    {
        uint32_t write_access_cmd : 8;
        uint32_t read_access_cmd : 8;
        uint32_t dummy_cycle_select : 4;
        uint32_t support_4s_4s_4s : 1;
        uint32_t support_4d_4d_4d : 1;
        uint32_t support_8s_8s_8s : 1;
        uint32_t support_8d_8d_8d : 1;
        uint32_t wel_bit_position : 3;
        uint32_t wel_bit_exist : 1;
        uint32_t bit_access_use_addrss : 1;
        uint32_t wel_write_access : 1;
        uint32_t wel_polarity : 1;
        uint32_t wel_available : 1;
    } write_enable_info;

    /* 7th DWORD */
    // program error
    uint32_t program_error;

    /* 8th DWORD */
    // erase error
    uint32_t erase_error;

    /* 9th DWORD */
    // dummy cycles setting for volatile reg
    uint32_t yummy_cyclse_volatile;

    /* 10th DWORD */
    // dummy cycles setting for non-volatile reg
    uint32_t yummy_cyclse_nonvolatile;

    /* 11th DWORD */
    // dummy cycles setting for bit patterns
    uint32_t yummy_cyclse_bit_pattern;

    /* 12th DWORD */
    // dummy cycles setting for bit patterns
    uint32_t yummy_cyclse_bit_pattern2;

    /* 13th DWORD */
    // dummy cycles setting for bit patterns
    uint32_t yummy_cyclse_bit_pattern3;

    /* 14th DWORD */
    struct
    {
        uint32_t write_access_cmd : 8;
        uint32_t read_access_cmd : 8;
        uint32_t dummy_cycle_select : 8;
        uint32_t qpi_bit_position : 3;
        uint32_t qpi_bit_exist : 1;
        uint32_t bit_access_use_addrss : 1;
        uint32_t reserved : 1;
        uint32_t qpi_polarity : 1;
        uint32_t qpi_available : 1;
    } volatile_qpi_enable_info;

    /* 15th DWORD */
    struct
    {
        uint32_t write_access_cmd : 8;
        uint32_t read_access_cmd : 8;
        uint32_t dummy_cycle_select : 8;
        uint32_t qpi_bit_position : 3;
        uint32_t qpi_bit_exist : 1;
        uint32_t bit_access_use_addrss : 1;
        uint32_t reserved : 1;
        uint32_t qpi_polarity : 1;
        uint32_t qpi_available : 1;
    } non_volatile_qpi_enable_info;

    /* 16th DWORD */
    struct
    {
        uint32_t write_access_cmd : 8;
        uint32_t read_access_cmd : 8;
        uint32_t dummy_cycle_select : 8;
        uint32_t octal_bit_position : 3;
        uint32_t octal_bit_exist : 1;
        uint32_t bit_access_use_addrss : 1;
        uint32_t reserved : 1;
        uint32_t octal_polarity : 1;
        uint32_t octal_available : 1;
    } volatile_octal_enable_info;

    /* 17th DWORD */
    struct
    {
        uint32_t write_access_cmd : 8;
        uint32_t read_access_cmd : 8;
        uint32_t dummy_cycle_select : 8;
        uint32_t octal_bit_position : 3;
        uint32_t octal_bit_exist : 1;
        uint32_t bit_access_use_addrss : 1;
        uint32_t otp_bit : 1;
        uint32_t octal_polarity : 1;
        uint32_t octal_available : 1;
    } non_volatile_octal_enable_info;

    /* 18th DWORD */
    struct
    {
        uint32_t write_access_cmd : 8;
        uint32_t read_access_cmd : 8;
        uint32_t dummy_cycle_select : 8;
        uint32_t str_dtr_bit_position : 3;
        uint32_t str_dtr_bit_exist : 1;
        uint32_t bit_access_use_addrss : 1;
        uint32_t reserved : 1;
        uint32_t str_dtr_polarity : 1;
        uint32_t str_dtr_available : 1;
    } volatile_str_dtr_enable_info;

    /* 19th DWORD */
    struct
    {
        uint32_t write_access_cmd : 8;
        uint32_t read_access_cmd : 8;
        uint32_t dummy_cycle_select : 8;
        uint32_t str_dtr_bit_position : 3;
        uint32_t str_dtr_bit_exist : 1;
        uint32_t bit_access_use_addrss : 1;
        uint32_t reserved : 1;
        uint32_t str_dtr_polarity : 1;
        uint32_t str_dtr_available : 1;
    } non_volatile_str_dtr_enable_info;

    /* 20th DWORD */
    struct
    {
        uint32_t write_access_cmd : 8;
        uint32_t read_access_cmd : 8;
        uint32_t dummy_cycle_select : 8;
        uint32_t str_octal_bit_position : 3;
        uint32_t str_octal_bit_exist : 1;
        uint32_t bit_access_use_addrss : 1;
        uint32_t reserved : 1;
        uint32_t str_octal_polarity : 1;
        uint32_t str_octal_available : 1;
    } volatile_str_octal_enable_info;

    /* 21th DWORD */
    struct
    {
        uint32_t write_access_cmd : 8;
        uint32_t read_access_cmd : 8;
        uint32_t dummy_cycle_select : 8;
        uint32_t str_octal_bit_position : 3;
        uint32_t str_octal_bit_exist : 1;
        uint32_t bit_access_use_addrss : 1;
        uint32_t reserved : 1;
        uint32_t str_octal_polarity : 1;
        uint32_t str_octal_available : 1;
    } non_volatile_str_octal_enable_info;

    /* 22th DWORD */
    struct
    {
        uint32_t write_access_cmd : 8;
        uint32_t read_access_cmd : 8;
        uint32_t dummy_cycle_select : 8;
        uint32_t dtr_octal_bit_position : 3;
        uint32_t dtr_octal_bit_exist : 1;
        uint32_t bit_access_use_addrss : 1;
        uint32_t reserved : 1;
        uint32_t dtr_octal_polarity : 1;
        uint32_t dtr_octal_available : 1;
    } volatile_dtr_octal_enable_info;

    /* 23th DWORD */
    struct
    {
        uint32_t write_access_cmd : 8;
        uint32_t read_access_cmd : 8;
        uint32_t dummy_cycle_select : 8;
        uint32_t dtr_octal_bit_position : 3;
        uint32_t dtr_octal_bit_exist : 1;
        uint32_t bit_access_use_addrss : 1;
        uint32_t reserved : 1;
        uint32_t dtr_octal_polarity : 1;
        uint32_t dtr_octal_available : 1;
    } non_volatile_dtr_octal_enable_info;

    /* 24th DWORD */
    // dpd status
    uint32_t dpd_status;

    /* 25th DWORD */
    // udpd status
    uint32_t udpd_status;

    /* 26th DWORD */
    // output driver strength volatile
    uint32_t output_driver_strength;

    /* 27th DWORD */
    // output driver strength non-volatile
    uint32_t output_driver_strength_nv;

    /* 28th DWORD */
    // output driver strength control bit patterns
    uint32_t output_driver_strength_bit_pattern;
} jedec_sccr_table_t;

/*! @brief Status, Control and Configuration Register Map for xSPI Profile 2.0 Memory Devices. */
typedef struct _jedec_sccr_2_table
{
    /* 1st DWORD */
    uint32_t volatile_reg_address_offset;

    /* 2nd DWORD */
    uint32_t non_volatile_reg_address_offset;

    /* 3rd DWORD */
    uint32_t cmd_status_control;

    /* 4th DWARD */
    uint32_t program_error;

    /* 5th DWARD */
    uint32_t erase_error;

    /* 6th DWARD */
    uint32_t dummy_cycle_setting_volatile;

    /* 7th DWARD */
    uint32_t dummy_cycle_setting_nv;

    /* 8th DWARD */
    uint32_t dummy_cycle_setting_bit_pattern1;

    /* 9th DWARD */
    uint32_t dummy_cycle_setting_bit_pattern2;

    /* 10th DWARD */
    uint32_t dummy_cycle_setting_bit_pattern3;

    /* 11th DWARD */
    uint32_t output_driver_strength_volatile;

    /* 12th DWARD */
    uint32_t output_driver_strength_nv;

    /* 13th DWARD */
    uint32_t output_driver_strength_bit_pattern;

} jedec_sccr_2_table_t;

/*! @brief Command Sequences to change to Octal DDR (8D-8D-8D) mode Instruction Table. */
typedef struct _jedec_octal_ddr_mode_inst_table
{
    /* 1st DWORD */
    struct
    {
        uint32_t byte_3_of_1st_command_seq : 8;
        uint32_t byte_2_of_1st_command_seq : 8;
        uint32_t byte_1_of_1st_command_seq : 8;
        uint32_t length_of_1st_command_seq : 8;
    } first_cmd_info_1st_dword;

    /* 2nd DWORD */
    struct
    {
        uint32_t byte_7_of_1st_command_seq : 8;
        uint32_t byte_6_of_1st_command_seq : 8;
        uint32_t byte_5_of_1st_command_seq : 8;
        uint32_t byte_4_of_1st_command_seq : 8;
    } first_cmd_info_2nd_dword;

    /* 3rd DWORD */
    struct
    {
        uint32_t byte_3_of_2nd_command_seq : 8;
        uint32_t byte_2_of_2nd_command_seq : 8;
        uint32_t byte_1_of_2nd_command_seq : 8;
        uint32_t length_of_2nd_command_seq : 8;
    } second_cmd_info_1st_dword;

    /* 4th DWORD */
    struct
    {
        uint32_t byte_7_of_2nd_command_seq : 8;
        uint32_t byte_6_of_2nd_command_seq : 8;
        uint32_t byte_5_of_2nd_command_seq : 8;
        uint32_t byte_4_of_2nd_command_seq : 8;
    } second_cmd_info_2nd_dword;

    /* 5th DWORD */
    struct
    {
        uint32_t byte_3_of_3rd_command_seq : 8;
        uint32_t byte_2_of_3rd_command_seq : 8;
        uint32_t byte_1_of_3rd_command_seq : 8;
        uint32_t length_of_3rd_command_seq : 8;
    } third_cmd_info_1st_dword;

    /* 6th DWORD */
    struct
    {
        uint32_t byte_7_of_3rd_command_seq : 8;
        uint32_t byte_6_of_3rd_command_seq : 8;
        uint32_t byte_5_of_3rd_command_seq : 8;
        uint32_t byte_4_of_3rd_command_seq : 8;
    } third_cmd_info_2nd_dword;

    /* 7th DWORD */
    struct
    {
        uint32_t byte_3_of_4th_command_seq : 8;
        uint32_t byte_2_of_4th_command_seq : 8;
        uint32_t byte_1_of_4th_command_seq : 8;
        uint32_t length_of_4th_command_seq : 8;
    } fouth_cmd_info_1st_dword;

    /* 8th DWORD */
    struct
    {
        uint32_t byte_7_of_4th_command_seq : 8;
        uint32_t byte_6_of_4th_command_seq : 8;
        uint32_t byte_5_of_4th_command_seq : 8;
        uint32_t byte_4_of_4th_command_seq : 8;
    } fouth_cmd_info_2nd_dword;
} jedec_octal_ddr_mode_cmd_table_t;

typedef struct _jdec_query_table
{
    uint32_t standard_version;
    uint32_t basic_flash_param_tbl_size;
    jedec_basic_flash_param_table_t basic_flash_param_tbl;
    bool has_4b_addressing_inst_table;
    jedec_4byte_addressing_inst_table_t flash_4b_inst_tbl;
    jedec_xspi_profile_1_table_t xspi_profile_1_tb1;
    jedec_sccr_table_t sccr_tb1;
    jedec_sccr_2_table_t sccr_2_tb1;
    jedec_octal_ddr_mode_cmd_table_t octal_ddr_cmd_tb1;
} jedec_info_table_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static status_t FLEXSPI_NOR_PrepareQuadModeEnableSequence(nor_handle_t *handle,
                                                          flexspi_mem_config_t *config,
                                                          jedec_info_table_t *tbl);
static status_t FLEXSPI_NOR_EnableQuadMode(nor_handle_t *handle, flexspi_mem_config_t *config);

static status_t FLEXSPI_NOR_PreEnterOctalMode(nor_handle_t *handle,
                                              flexspi_mem_config_t *config,
                                              jedec_info_table_t *tbl);

static status_t FLEXSPI_NOR_MacronixEnterOctalMode(nor_handle_t *handle,
                                                   flexspi_mem_config_t *config,
                                                   jedec_info_table_t *tbl);

static status_t FLEXSPI_NOR_MacronixExitOctalMode(nor_handle_t *handle,
                                                  flexspi_mem_config_t *config,
                                                  jedec_info_table_t *tbl);

static status_t FLEXSPI_NOR_AdestoEnterOctalMode(nor_handle_t *handle,
                                                 flexspi_mem_config_t *config,
                                                 jedec_info_table_t *tbl);

static status_t FLEXSPI_NOR_ProbeCommandMode(nor_handle_t *handle, flexspi_mem_config_t *config);

static status_t FLEXSPI_NOR_CheckCommandModeAvailability(nor_handle_t *handle,
                                                         flexspi_mem_config_t *config,
                                                         jedec_info_table_t *tbl);

static status_t FLEXSPI_NOR_GenerateConfigBlockUsingSFDP(nor_handle_t *handle, flexspi_mem_config_t *config);

static status_t FLEXSPI_NOR_WaitBusBusy(FLEXSPI_Type *base, flexspi_mem_nor_handle_t *handle, uint32_t baseAddr);

static status_t FLEXSPI_NOR_WriteEnable(FLEXSPI_Type *base, flexspi_port_t port, uint32_t baseAddr);

static status_t FLEXSPI_NOR_ParseSFDP(nor_handle_t *handle, flexspi_mem_config_t *config, jedec_info_table_t *tbl);

static status_t FLEXSPI_NOR_GetPageSectorBlockSizeFromSFDP(nor_handle_t *handle,
                                                           jedec_info_table_t *tbl,
                                                           uint32_t *sector_erase_cmd,
                                                           uint32_t *block_erase_cmd);

status_t FLEXSPI_NOR_ExitNoCMDMode(nor_handle_t *handle, flexspi_mem_config_t *config, uint32_t baseAddr);
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
static status_t FLEXSPI_NOR_WaitBusBusy(FLEXSPI_Type *base, flexspi_mem_nor_handle_t *handle, uint32_t baseAddr)
{
    /* Wait status ready. */
    bool isBusy = true;
    uint32_t readValue;
    status_t status;
    flexspi_transfer_t flashXfer;

    flashXfer.deviceAddress = baseAddr;
    flashXfer.port          = handle->port;
    flashXfer.cmdType       = kFLEXSPI_Read;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_READSTATUS;
    flashXfer.data          = &readValue;
    flashXfer.dataSize      = 1;

    do
    {
        status = FLEXSPI_TransferBlocking(base, &flashXfer);

        if (status != kStatus_Success)
        {
            return status;
        }

        if (handle->busyBitPolarity == 0x01U)
        {
            isBusy = (((~readValue) & (0x01UL << handle->busyOffset)) == (0x01UL << handle->busyOffset)) ? true : false;
        }
        else
        {
            isBusy = ((readValue & (0x01UL << handle->busyOffset)) == (0x01UL << handle->busyOffset)) ? true : false;
        }
    } while (isBusy);

    return status;
}

static status_t FLEXSPI_NOR_WriteEnable(FLEXSPI_Type *base, flexspi_port_t port, uint32_t baseAddr)
{
    flexspi_transfer_t flashXfer;
    status_t status;

    /* Write enable */
    flashXfer.deviceAddress = baseAddr;
    flashXfer.port          = port;
    flashXfer.cmdType       = kFLEXSPI_Command;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITEENABLE;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

static status_t FLEXSPI_NOR_ReadSFDP(FLEXSPI_Type *base, uint32_t address, uint32_t *buffer, uint32_t bytes)
{
    flexspi_transfer_t flashXfer;
    status_t status;

    (void)memset(&flashXfer, 0, sizeof(flashXfer));

    flashXfer.deviceAddress = address;
    flashXfer.port          = flexspiMemHandle.port;
    flashXfer.cmdType       = kFLEXSPI_Read;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_READ_SFDP;
    flashXfer.data          = buffer;
    flashXfer.dataSize      = bytes;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

/*
 * SFDP Overall Header Structure
 * ---------------------
 * SFDP Header, always located at SFDP offset 0x0. Note that the SFDP space is
 * always read in 3 Byte addressing mode, where the expected SFDP address space
 * layout looks like the following:
 *
 *  ------------------0x00
 *  | SFDP Header        |  (specifying X number of Parameter Headers)
 *  ------------------0x08
 *  | Parameter Header 1 |  (specifying Y Parameter Table Pointer & Length L)
 *  ------------------0x10
 *          - - -
 *  --------------X * 0x08
 *  | Parameter Header X |  (specifying Z Parameter Table Pointer & Length K)
 *  --------(X + 1) * 0x08
 *          - - -
 *  ---------------------Y
 *  | Parameter Table 1  |
 *  -------------------Y+L
 *         - - -
 *  ---------------------Z            Key:    ------start_sfdp_offset
 *  | Parameter Table X  |                    | Region Name         |
 *  -------------------Z+K                    ------limit_sfdp_offset
 */
static status_t FLEXSPI_NOR_ReadSFDPInfo(FLEXSPI_Type *base, jedec_info_table_t *tbl)
{
    status_t status = kStatus_Fail;
    do
    {
        if (tbl == NULL)
        {
            status = kStatus_InvalidArgument;
            break;
        }

        (void)memset(tbl, 0, sizeof(*tbl));

        /*
         * Read SFDP header
         * ---------------------
         * The SFDP Header is located at address 000000h of the SFDP data structure. It identifies the SFDP
         * Signature, the number of parameter headers, and the SFDP revision numbers.
         */
        sfdp_header_t sfdp_header;
        uint32_t address = 0;

        /* Zero initialization*/
        (void)memset(&sfdp_header, 0, sizeof(sfdp_header));

        status = FLEXSPI_NOR_ReadSFDP(base, 0, (uint32_t *)&sfdp_header, sizeof(sfdp_header));
        if (status != kStatus_Success)
        {
            break;
        }

        if (sfdp_header.signature != SFDP_JESD216_SIGNATURE || sfdp_header.major_rev != SFDP_JESD216_MAJOR)
        {
            break;
        }

        tbl->standard_version = sfdp_header.minor_rev;

        /* This number is 0-based. Therefore, 0 indicates 1 parameter header. */
        uint8_t parameter_header_number = sfdp_header.param_hdr_num + 1U;

        /*
         * Read parameter headers
         * ---------------------
         * The first parameter header is mandatory, is defined by JEDEC standard, and starts at byte offset 08h.
         */
        sfdp_parameter_header_t sfdp_param_hdrs[12];
        uint8_t max_hdr_count = (parameter_header_number > 10U) ? 10U : parameter_header_number;
        address               = 0x08U;
        uint8_t i;

        /* Zero initialization*/
        (void)memset(sfdp_param_hdrs, 0, sizeof(sfdp_param_hdrs));

        status = FLEXSPI_NOR_ReadSFDP(base, address, (uint32_t *)&sfdp_param_hdrs[0],
                                      (uint32_t)max_hdr_count * sizeof(sfdp_parameter_header_t));
        if (status != kStatus_Success)
        {
            break;
        }

        for (i = 0; i < max_hdr_count; i++)
        {
            uint32_t parameter_id = SFDP_PARAM_HEADER_ID(sfdp_param_hdrs[i]);

            address = SFDP_PARAM_HEADER_PTP(sfdp_param_hdrs[i]);

            uint32_t table_size = (uint32_t)sfdp_param_hdrs[i].table_length_in_32bit * sizeof(uint32_t);

            if (parameter_id == (uint32_t)kParameterID_BasicSpiProtocal)
            {
                /* Limit table size to the max supported JEDEC216C standard */
                if (table_size > sizeof(jedec_basic_flash_param_table_t))
                {
                    table_size = sizeof(jedec_basic_flash_param_table_t);
                }

                /* Read Basic SPI Protocol Table */
                status = FLEXSPI_NOR_ReadSFDP(base, address, (uint32_t *)&tbl->basic_flash_param_tbl, table_size);

                if (status != kStatus_Success)
                {
                    break;
                }

                tbl->basic_flash_param_tbl_size = table_size;
            }
            else if (parameter_id == (uint32_t)kParameterID_4ByteAddressInstructionTable)
            {
                /* Limit table size to the max supported JEDEC216C/D standard */
                if (table_size > sizeof(jedec_4byte_addressing_inst_table_t))
                {
                    table_size = sizeof(jedec_4byte_addressing_inst_table_t);
                }

                /* Read 4-byte Address Instruction Table */
                status = FLEXSPI_NOR_ReadSFDP(base, address, (uint32_t *)&tbl->flash_4b_inst_tbl, table_size);
                if (status != kStatus_Success)
                {
                    break;
                }
                tbl->has_4b_addressing_inst_table = true;
            }
            else if (parameter_id == (uint32_t)kParameterID_CommandSeqToOctalDDRMode)
            {
                /* Limit table size to the max supported JEDEC216C/D standard */
                if (table_size > sizeof(jedec_octal_ddr_mode_cmd_table_t))
                {
                    table_size = sizeof(jedec_octal_ddr_mode_cmd_table_t);
                }

                /* Read Command Sequences to change to Octal DDR (8D-8D-8D) mode Instruction Table */
                status = FLEXSPI_NOR_ReadSFDP(base, address, (uint32_t *)&tbl->octal_ddr_cmd_tb1, table_size);
                if (status != kStatus_Success)
                {
                    break;
                }
            }
            else if (parameter_id == (uint32_t)kParameterID_XSPIProfileVersion1)
            {
                /* Limit table size to the max supported JEDEC216C/D standard */
                if (table_size > sizeof(jedec_xspi_profile_1_table_t))
                {
                    table_size = sizeof(jedec_xspi_profile_1_table_t);
                }

                /* Read eXtended Serial Peripheral Interface (xSPI) Profile 1.0 Table */
                status = FLEXSPI_NOR_ReadSFDP(base, address, (uint32_t *)&tbl->xspi_profile_1_tb1, table_size);
                if (status != kStatus_Success)
                {
                    break;
                }
            }
            else if (parameter_id == (uint32_t)kParameterID_SCCRM)
            {
                /* Limit table size to the max supported JEDEC216C/D standard */
                if (table_size > sizeof(jedec_xspi_profile_1_table_t))
                {
                    table_size = sizeof(jedec_xspi_profile_1_table_t);
                }

                /* Read Status, Control and Configuration Register Map Table */
                status = FLEXSPI_NOR_ReadSFDP(base, address, (uint32_t *)&tbl->sccr_tb1, table_size);
                if (status != kStatus_Success)
                {
                    break;
                }
            }
            else if (parameter_id == (uint32_t)kParameterID_SCCRMForxSPIProfileVersion2)
            {
                /* Limit table size to the max supported JEDEC216C/D standard */
                if (table_size > sizeof(jedec_sccr_2_table_t))
                {
                    table_size = sizeof(jedec_sccr_2_table_t);
                }

                /* Read Status, Control and Configuration Register Map Table */
                status = FLEXSPI_NOR_ReadSFDP(base, address, (uint32_t *)&tbl->sccr_2_tb1, table_size);
                if (status != kStatus_Success)
                {
                    break;
                }
            }
            else
            {
                /* Unsupported function specific parameter type, ignore */
            }
        }

    } while (false);

    return status;
}

static status_t FLEXSPI_NOR_GetPageSectorBlockSizeFromSFDP(nor_handle_t *handle,
                                                           jedec_info_table_t *tbl,
                                                           uint32_t *sector_erase_cmd,
                                                           uint32_t *block_erase_cmd)
{
    jedec_basic_flash_param_table_t *param_tbl        = &tbl->basic_flash_param_tbl;
    jedec_4byte_addressing_inst_table_t *flash_4b_tbl = &tbl->flash_4b_inst_tbl;

    uint32_t flashSize;
    uint32_t flashDensity = tbl->basic_flash_param_tbl.flash_density;

    if ((flashDensity & (1UL << 0x1F)) != 0x00U)
    {
        /* Flash size >= 4G bits */
        flashSize = 1UL << (flashDensity & ~(1UL << 0x1F));
    }
    else
    {
        /* Flash size < 4G bits */
        flashSize = flashDensity + 1U;
    }

    /* Convert to bytes. */
    flashSize >>= 3;

    handle->bytesInMemorySize = flashSize;

    /* Calculate Page size */
    uint32_t page_size;
    if (tbl->basic_flash_param_tbl_size < 64U)
    {
        handle->bytesInPageSize = 256U;
    }
    else
    {
        page_size               = 0x01UL << (param_tbl->chip_erase_progrm_info.page_size);
        handle->bytesInPageSize = (page_size == (0x01UL << 15)) ? 256UL : page_size;
    }

    /* Calculate Sector Size */
    uint32_t sectorSize      = 0xFFFFU;
    uint32_t blockSize       = 0x00U;
    uint32_t sectorEraseType = 0U;
    uint32_t blockEraseType  = 0U;
    uint8_t index;

    for (index = 0U; index < 4U; index++)
    {
        if (param_tbl->erase_info[index].size != 0UL)
        {
            uint32_t current_erase_size = 1UL << param_tbl->erase_info[index].size;
            /* Calculate min non-zero sector size */
            if ((current_erase_size < sectorSize) && (current_erase_size < (1024U * 1024U)))
            {
                sectorSize      = current_erase_size;
                sectorEraseType = index;
            }

            /* Calculate max non-zero sector size */
            if ((current_erase_size > blockSize) && (current_erase_size < (1024U * 1024U)))
            {
                blockEraseType = index;
            }
        }
    }

    handle->bytesInSectorSize = sectorSize;

    /* Calculate erase instruction operand for LUT */
    if ((flashSize > MAX_24BIT_ADDRESSING_SIZE) && (tbl->has_4b_addressing_inst_table))
    {
        *sector_erase_cmd = flash_4b_tbl->erase_inst_info.erase_inst[sectorEraseType];
        *block_erase_cmd  = flash_4b_tbl->erase_inst_info.erase_inst[blockEraseType];
    }
    else
    {
        *sector_erase_cmd = param_tbl->erase_info[sectorEraseType].inst;
        *block_erase_cmd  = param_tbl->erase_info[blockEraseType].inst;
    }

    return kStatus_Success;
}

static status_t FLEXSPI_NOR_ParseSFDP(nor_handle_t *handle, flexspi_mem_config_t *config, jedec_info_table_t *tbl)
{
    status_t status = kStatus_InvalidArgument;
    bool isSDRMode  = false;

    do
    {
        jedec_basic_flash_param_table_t *param_tbl        = &tbl->basic_flash_param_tbl;
        jedec_4byte_addressing_inst_table_t *flash_4b_tbl = &tbl->flash_4b_inst_tbl;
        jedec_xspi_profile_1_table_t *xspi_profile_1_tb1  = &tbl->xspi_profile_1_tb1;

        uint8_t read_cmd        = 0U;
        uint32_t dummy_cycles   = 0U;
        uint8_t mode_cycles     = 0U;
        uint32_t sectorEraseCmd = 0U;
        uint32_t blockEraseCmd  = 0U;
        uint32_t address_bits   = 24U;
        uint32_t address_pads   = kFLEXSPI_1PAD;
        bool support_ddr_mode   = false;

        /* Get page and sector information from SFDP. */
        FLEXSPI_NOR_GetPageSectorBlockSizeFromSFDP(handle, tbl, &sectorEraseCmd, &blockEraseCmd);

        if ((flexspiMemHandle.configuredFlashSize > MAX_24BIT_ADDRESSING_SIZE) && (tbl->has_4b_addressing_inst_table))
        {
            address_bits = 32;
        }

        /* Check whether octal command mode is supported or not. */
        bool supportOctalCmdMode = (param_tbl->mode_8_8_info.mode_8s_8s_8s_enable_seq != 0x00U) ? true : false;

        /* Check whether DDR transfer rate is supported or not. */
        support_ddr_mode = (param_tbl->misc.support_dtr_clocking != 0x00U) ? true : false;

        if (!supportOctalCmdMode)
        {
            /* Check whether DDR mode is supported. */
            if (config->deviceType == kSerialNorCfgOption_DeviceType_ReadSFDP_DDR)
            {
                if (!support_ddr_mode)
                {
                    break;
                }
            }
            else
            {
                support_ddr_mode = false;
            }

            /* Prepare Quad Mode enable sequence as needed. */
            status = FLEXSPI_NOR_PrepareQuadModeEnableSequence(handle, config, tbl);

            if (status != kStatus_Success)
            {
                break;
            }

            /* Determine Read command based on SFDP. */
            if (param_tbl->misc.supports_1_4_4_fast_read != 0x00U)
            {
                address_pads = (uint32_t)kFLEXSPI_4PAD;
                mode_cycles  = param_tbl->read_1_4_info.mode_clocks_1_4_4_read;
                dummy_cycles = param_tbl->read_1_4_info.dummy_clocks_1_4_4_read;
            }
            else if (param_tbl->misc.support_1_1_4_fast_read != 0x00U)
            {
                mode_cycles  = param_tbl->read_1_4_info.mode_clocks_1_1_4_read;
                dummy_cycles = param_tbl->read_1_4_info.dummy_clocks_1_1_4_read;
            }
            else
            {
                ;
            }

            /* Set page program instruction for LUT*/
            if (address_bits == 32U)
            {
                if (flash_4b_tbl->cmd_4byte_support_info.support_1_4_4_page_program != 0x00U)
                {
                    /*
                     * Page Program command for 1-4-4 mode based on 4 bytes address
                     * --------------------------------------------------------------
                     * Inst No.| Inst opcode | Inst num_pads | Inst oprand
                     * ==============================================================
                     * 0       | CMD_SDR     | 0x0 (Single)  | 0x3E
                     * --------------------------------------------------------------
                     * 1       | ADDR_SDR    | 0x2 (Quad)    | 0x20 (4 bytes address)
                     * --------------------------------------------------------------
                     * 2       | WRITE_SDR   | 0x2 (Quad)    | Any non-zero value
                     * --------------------------------------------------------------
                     * 3 ~ 7   | STOP (0x00) | 0x0 (Single)  | 0x00
                     * -------------------------------------------------------------- */
                    config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM] =
                        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_PageProgram_1_4_4_4B,
                                        kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 32);
                    config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM + 1U] = FLEXSPI_LUT_SEQ(
                        kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);
                }
                else if (flash_4b_tbl->cmd_4byte_support_info.support_1_1_4_page_program != 0x00U)
                {
                    /*
                     * Page Program command for 1-1-4 mode based on 4 bytes address
                     * --------------------------------------------------------------
                     * Inst No.| Inst opcode | Inst num_pads | Inst oprand
                     * ==============================================================
                     * 0       | CMD_SDR     | 0x0 (Single)  | 0x01
                     * --------------------------------------------------------------
                     * 1       | ADDR_SDR    | 0x0 (Single)  | 0x20 (4 bytes address)
                     * --------------------------------------------------------------
                     * 2       | WRITE_SDR   | 0x2 (Quad)    | Any non-zero value
                     * --------------------------------------------------------------
                     * 3 ~ 7   | STOP (0x00) | 0x0 (Single)  | 0x00
                     * -------------------------------------------------------------- */
                    config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM] =
                        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_PageProgram_1_1_4_4B,
                                        kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 32);
                    config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM + 1U] = FLEXSPI_LUT_SEQ(
                        kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);
                }
                else
                {
                    /*
                     * Page Program command for 1-1-1 mode based on 4 bytes address
                     * --------------------------------------------------------------
                     * Inst No.| Inst opcode | Inst num_pads | Inst oprand
                     * ==============================================================
                     * 0       | CMD_SDR     | 0x0 (Single)  | 0x12
                     * --------------------------------------------------------------
                     * 1       | ADDR_SDR    | 0x0 (Single)  | 0x20 (4 bytes address)
                     * --------------------------------------------------------------
                     * 2       | WRITE_SDR   | 0x0 (Single)  | Any non-zero value
                     * --------------------------------------------------------------
                     * 3 ~ 7   | STOP (0x00) | 0x0 (Single)  | 0x00
                     * -------------------------------------------------------------- */
                    config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM] =
                        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_PageProgram_1_1_1_4B,
                                        kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 32);
                    config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM + 1U] = FLEXSPI_LUT_SEQ(
                        kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);
                }
            }
            else
            {
                /*
                 * Page Program command for 1-1-1 mode based on 3 bytes address
                 * --------------------------------------------------------------
                 * Inst No.| Inst opcode | Inst num_pads | Inst oprand
                 * ==============================================================
                 * 0       | CMD_SDR     | 0x0 (Single)  | 0x12
                 * --------------------------------------------------------------
                 * 1       | ADDR_SDR    | 0x0 (Single)  | 0x18 (3 bytes address)
                 * --------------------------------------------------------------
                 * 2       | WRITE_SDR   | 0x0 (Single)  | Any non-zero value
                 * --------------------------------------------------------------
                 * 3 ~ 7   | STOP (0x00) | 0x0 (Single)  | 0x00
                 * -------------------------------------------------------------- */
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM] =
                    FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_PageProgram_1_1_1_3B,
                                    kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 24);
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM + 1U] = FLEXSPI_LUT_SEQ(
                    kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);
            }

            /* Set write enable command instruction for LUT. */
            config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] =
                FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_WriteEnable, kFLEXSPI_Command_STOP,
                                kFLEXSPI_1PAD, 0);

            /* Set read status command instruction for LUT. */
            config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_READSTATUS] =
                FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_ReadStatusReg1,
                                kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04);

            /* Set sector erase command instruction for LUT. */
            config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_ERASESECTOR] =
                FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, sectorEraseCmd, kFLEXSPI_Command_RADDR_SDR,
                                kFLEXSPI_1PAD, address_bits);

            /* Set sector erase command instruction for LUT. */
            config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_ERASEBLOCK] =
                FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, blockEraseCmd, kFLEXSPI_Command_RADDR_SDR,
                                kFLEXSPI_1PAD, address_bits);

            /* Set chip erase command instruction for LUT. */
            config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_CHIPERASE] = FLEXSPI_LUT_SEQ(
                kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_ChipErase, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);

            /* Set read commmand instruction for LUT. */
            if (address_bits == 24U)
            {
                if (support_ddr_mode)
                {
                    read_cmd = kSerialNorCmd_kFLEXSPI_Command_READ_DDR_1_4_4_3B;
                }
                else if (param_tbl->misc.supports_1_4_4_fast_read != 0x00U)
                {
                    read_cmd = param_tbl->read_1_4_info.inst_1_4_4_read;
                }
                else if (param_tbl->misc.support_1_1_4_fast_read != 0x00U)
                {
                    read_cmd = param_tbl->read_1_4_info.inst_1_1_4_read;
                }
                else
                {
                    read_cmd     = kSerialNorCmd_BasicRead_3B;
                    dummy_cycles = 0;
                    mode_cycles  = 0;
                }
            }
            else
            {
                address_pads = (uint32_t)kFLEXSPI_4PAD;
                if (support_ddr_mode)
                {
                    read_cmd = kSerialNorCmd_kFLEXSPI_Command_READ_DDR_1_4_4_4B;
                }
                else if (flash_4b_tbl->cmd_4byte_support_info.support_1_4_4_fast_read != 0x00U)
                {
                    read_cmd = kSerialNorCmd_kFLEXSPI_Command_READ_SDR_1_4_4_4B;
                }
                else if (flash_4b_tbl->cmd_4byte_support_info.support_1_1_4_fast_read != 0x00U)
                {
                    read_cmd     = kSerialNorCmd_kFLEXSPI_Command_READ_SDR_1_1_4_4B;
                    address_pads = kFLEXSPI_1PAD;
                }
                else
                {
                    read_cmd     = kSerialNorCmd_BasicRead_4B;
                    dummy_cycles = 0;
                    mode_cycles  = 0;
                }
            }

            /* Set read command instruction for LUT. */
            if (support_ddr_mode)
            {
                config->controllerMiscOption |= (0x01UL << kFlexSpiMiscOffset_DdrModeEnable);
                config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READ * 0U] =
                    FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, read_cmd, kFLEXSPI_Command_RADDR_DDR,
                                    kFLEXSPI_4PAD, address_bits);
                /* Dummy cycle is depending on flash device, different flash vendor has a different dummy cycle, this
                 * part will be updated. */
                config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READ * 0U + 1U] =
                    FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_DDR, kFLEXSPI_4PAD, (mode_cycles + dummy_cycles) * 2U,
                                    kFLEXSPI_Command_READ_DDR, kFLEXSPI_4PAD, 0x04);
            }
            else
            {
                config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READ * 0U] =
                    FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, read_cmd, kFLEXSPI_Command_RADDR_SDR,
                                    address_pads, address_bits);
                config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READ * 0U + 1U] =
                    FLEXSPI_LUT_SEQ(kFLEXSPI_Command_MODE8_SDR, kFLEXSPI_4PAD, mode_cycles, kFLEXSPI_Command_DUMMY_SDR,
                                    kFLEXSPI_4PAD, dummy_cycles);
                config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READ * 0U + 2U] =
                    FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, address_pads, 0x04, 0, 0, 0);
            }

            if (support_ddr_mode)
            {
                config->halfClkForNonReadCmd = true;
                config->controllerMiscOption |= (0x01UL << kFlexSpiMiscOffset_DdrModeEnable);
            }
        }
        else
        {
            /* Force transfer rate as DDR if serial nor flash support octal DDR mode. */
            if (support_ddr_mode)
            {
                config->transferMode = kSerialNorTransferMode_DDR;
            }
            else
            {
                config->transferMode = kSerialNorTransferMode_SDR;
            }

            /* read fast(8-8-8) command from SFDP information */
            uint8_t readCmd = xspi_profile_1_tb1->cmd_8d_8d_8d_mode_info.read_fast_cmd;

            /* Set read manufacturer ID LUT table in single mode */
            config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READID * 4U] =
                FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialFlash_ReadJEDECId,
                                kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04);
            FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_READID * 4UL,
                              &config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_READID], 4);

            /* Set write enable LUT table in single mode */
            config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] =
                FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x06, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);
            FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_WRITEENABLE * 4UL,
                              &config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE], 4);

            config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_READSTATUS + 0] = FLEXSPI_LUT_SEQ(
                kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x05, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04);
            FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_READSTATUS * 4UL,
                              &config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_READSTATUS], 4);

            /* Octal serial nor flash */
            status = FLEXSPI_NOR_PreEnterOctalMode(handle, config, tbl);
            if (kStatus_Success != status)
            {
                break;
            }

            if (config->transferMode == kSerialNorTransferMode_DDR)
            {
                isSDRMode = false;
            }
            else
            {
                isSDRMode = true;
            }

            uint32_t cmdInst   = isSDRMode ? kFLEXSPI_Command_SDR : kFLEXSPI_Command_DDR;
            uint32_t addrInst  = isSDRMode ? kFLEXSPI_Command_RADDR_SDR : kFLEXSPI_Command_RADDR_DDR;
            uint32_t dummyInst = isSDRMode ? kFLEXSPI_Command_DUMMY_SDR : kFLEXSPI_Command_DUMMY_DDR;
            uint32_t readInst  = isSDRMode ? kFLEXSPI_Command_READ_SDR : kFLEXSPI_Command_READ_DDR;
            uint32_t writeInst = isSDRMode ? kFLEXSPI_Command_WRITE_SDR : kFLEXSPI_Command_WRITE_DDR;

            /* Command extension to 16 bits, and start command should be under DDR mode in this situation */
            if (param_tbl->octal_1_8_info.octal_dtr_cmd_and_cmd_extension == 0x01U)
            {
                /* Update write enable LUT table and current command mode for octal mode */
                (void)memset(&config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE], 0, 4 * 4);
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] =
                    FLEXSPI_LUT_SEQ(cmdInst, kFLEXSPI_8PAD, 0x06, cmdInst, kFLEXSPI_8PAD, 0xF9);

                /* Set read command instruction for LUT. */
                if (isSDRMode)
                {
                    config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_READ + 0U] =
                        FLEXSPI_LUT_SEQ(cmdInst, kFLEXSPI_8PAD, kOctalSerialNorCmd_Read_8s_8s_8s_4B_1st, cmdInst,
                                        kFLEXSPI_8PAD, kOctalSerialNorCmd_Read_8s_8s_8s_4B_2nd);
                }
                else
                {
                    config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_READ + 0U] =
                        FLEXSPI_LUT_SEQ(cmdInst, kFLEXSPI_8PAD, readCmd, cmdInst, kFLEXSPI_8PAD,
                                        kOctalSerialNorCmd_Read_8d_8d_8d_4B_2nd);
                }
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_READ + 1U] =
                    FLEXSPI_LUT_SEQ(addrInst, kFLEXSPI_8PAD, 0x20, dummyInst, kFLEXSPI_8PAD, 10);
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_READ + 2U] =
                    FLEXSPI_LUT_SEQ(readInst, kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0x00);

                /* Read status register */
                (void)memset(&config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_READSTATUS], 0, 4 * 4);
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_READSTATUS + 0U] =
                    FLEXSPI_LUT_SEQ(cmdInst, kFLEXSPI_8PAD, kOctalSerialNorCmd_ReadStatusReg_1st, cmdInst,
                                    kFLEXSPI_8PAD, kOctalSerialNorCmd_ReadStatusReg_2nd);
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_READSTATUS + 1U] =
                    FLEXSPI_LUT_SEQ(addrInst, kFLEXSPI_8PAD, 0x20, readInst, kFLEXSPI_8PAD, 0x04);

                /* Program */
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM + 0U] =
                    FLEXSPI_LUT_SEQ(cmdInst, kFLEXSPI_8PAD, kOctalSerialNorCmd_PageProgram_4B_1st, cmdInst,
                                    kFLEXSPI_8PAD, kOctalSerialNorCmd_PageProgram_4B_2nd);
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM + 1U] =
                    FLEXSPI_LUT_SEQ(addrInst, kFLEXSPI_8PAD, 0x20, writeInst, kFLEXSPI_8PAD, 0x04);

                /* Erase Sector */
                /* Macronix Sector(4KB) erase is 0x21 and 0xDE */
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_ERASESECTOR + 0U] =
                    FLEXSPI_LUT_SEQ(cmdInst, kFLEXSPI_8PAD, kOctalSerialNorCmd_SectorErase_4B_1st, cmdInst,
                                    kFLEXSPI_8PAD, kOctalSerialNorCmd_SectorErase_4B_2nd);
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_ERASESECTOR + 1U] =
                    FLEXSPI_LUT_SEQ(addrInst, kFLEXSPI_8PAD, 0x20, kFLEXSPI_Command_STOP, kFLEXSPI_8PAD, 0x00);

                /* Erase Block */
                /* Macronix Block(64KB) erase is 0xDC and 0x23 */
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_ERASEBLOCK + 0U] =
                    FLEXSPI_LUT_SEQ(cmdInst, kFLEXSPI_8PAD, kOctalSerialNorCmd_BlockErase_4B_1st, cmdInst,
                                    kFLEXSPI_8PAD, kOctalSerialNorCmd_BlockErase_4B_2nd);
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_ERASEBLOCK + 1U] =
                    FLEXSPI_LUT_SEQ(addrInst, kFLEXSPI_8PAD, 0x20, kFLEXSPI_Command_STOP, kFLEXSPI_8PAD, 0x00);

                /* Erase Chip */
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_CHIPERASE] =
                    FLEXSPI_LUT_SEQ(cmdInst, kFLEXSPI_8PAD, kOctalSerialNorCmd_ChipErase_1st, cmdInst, kFLEXSPI_8PAD,
                                    kOctalSerialNorCmd_ChipErase_2nd);
            }
            /* No command extension, and start command should be CMD_SDR in this situation */
            else
            {
                /* The reason why make command instruction as CMD_SDR is that it must be in SDR mode if
                   the octal flash timing requires first command instruction. */
                cmdInst = kFLEXSPI_Command_SDR;

                /* Update write enable LUT table and current command mode for octal mode */
                (void)memset(&config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE], 0, 4 * 4);
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] =
                    FLEXSPI_LUT_SEQ(cmdInst, kFLEXSPI_8PAD, 0x06, kFLEXSPI_Command_STOP, kFLEXSPI_8PAD, 0);

                /* Set read command instruction for LUT. */
                /* Adesto 4B read instruction is readCmd(0x0B) for both SDR and DDR in octal mode, 0x03 is for 3B single
                 * mode and 0x13 is for 4B single mode */
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_READ + 0U] =
                    FLEXSPI_LUT_SEQ(cmdInst, kFLEXSPI_8PAD, readCmd, addrInst, kFLEXSPI_8PAD, 0x20);
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_READ + 1U] =
                    FLEXSPI_LUT_SEQ(dummyInst, kFLEXSPI_8PAD, 8, readInst, kFLEXSPI_8PAD, 0x04);

                /* Read status register */
                (void)memset(&config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_READSTATUS], 0, 4 * 4);
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_READSTATUS + 0U] = FLEXSPI_LUT_SEQ(
                    cmdInst, kFLEXSPI_8PAD, kOctalSerialNorCmd_ReadStatusReg_1st, readInst, kFLEXSPI_8PAD, 0x04);

                /* Program */
                /* Adesto 4B program instruction is 0x02, but others is 3B program instruction is 0x02 and 4B program
                 * instruction is 0x12 */
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM + 0U] =
                    FLEXSPI_LUT_SEQ(cmdInst, kFLEXSPI_8PAD, 0x02, addrInst, kFLEXSPI_8PAD, 0x20);
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM + 1U] =
                    FLEXSPI_LUT_SEQ(writeInst, kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_8PAD, 0x00);

                /* Erase Sector */
                /* Adesto Sector(4KB) erase is 0x20, which aligns with SFDP erase information */
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_ERASESECTOR + 0U] =
                    FLEXSPI_LUT_SEQ(cmdInst, kFLEXSPI_8PAD, sectorEraseCmd, addrInst, kFLEXSPI_8PAD, 0x20);

                /* Erase Block */
                /* Adesto Block(64KB) erase instruction is 0xD8, which aligned with SFDP erase information */
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_ERASEBLOCK + 0U] =
                    FLEXSPI_LUT_SEQ(cmdInst, kFLEXSPI_8PAD, blockEraseCmd, addrInst, kFLEXSPI_8PAD, 0x20);

                /* Erase Chip */
                config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_CHIPERASE] =
                    FLEXSPI_LUT_SEQ(cmdInst, kFLEXSPI_8PAD, kOctalSerialNorCmd_ChipErase_1st, kFLEXSPI_Command_STOP,
                                    kFLEXSPI_8PAD, 0x00);
            }
        }

        status = kStatus_Success;
    } while (false);

    return status;
}

static status_t FLEXSPI_NOR_PrepareQuadModeEnableSequence(nor_handle_t *handle,
                                                          flexspi_mem_config_t *config,
                                                          jedec_info_table_t *tbl)
{
    status_t status = kStatus_InvalidArgument;

    /* See JESD216B/C/D 6.4.18(JEDEC Basic Flash Parameter Table: 15th DWORD) for more details. */
    FLEXSPI_Type *base = (FLEXSPI_Type *)handle->driverBaseAddr;

    do
    {
        uint32_t enter_quad_mode_option = (uint32_t)kSerialNorQuadMode_NotConfig;
        uint32_t lut_seq[4];
        (void)memset(lut_seq, 0, sizeof(lut_seq));

        if (tbl->basic_flash_param_tbl_size >= 64U)
        {
            config->deviceModeCfgEnable = true;

            /* Set Quad mode read instruction for LUT. */
            switch (tbl->basic_flash_param_tbl.mode_4_4_info.quad_enable_requirement)
            {
                case 1:
                case 4:
                case 5:
                    enter_quad_mode_option = (uint32_t)kSerialNorQuadMode_StatusReg2_Bit1;
                    lut_seq[0] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_ReadStatusReg2Bit1,
                                                 kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 1);
                    break;
                case 6:
                    enter_quad_mode_option = (uint32_t)kSerialNorQuadMode_StatusReg2_Bit1_0x31;
                    lut_seq[0] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_ReadStatusReg2Bit1,
                                                 kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 1);
                    break;
                case 2:
                    enter_quad_mode_option = (uint32_t)kSerialNorQuadMode_StatusReg1_Bit6;
                    lut_seq[0] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_ReadStatusReg1,
                                                 kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 1);
                    break;
                case 3:
                    enter_quad_mode_option = (uint32_t)kSerialNorQuadMode_StatusReg2_Bit7;
                    lut_seq[0] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_ReadStatusReg2Bit7,
                                                 kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 1);
                    break;
                default:
                    enter_quad_mode_option      = (uint32_t)kSerialNorQuadMode_NotConfig;
                    config->deviceModeCfgEnable = false;
                    break;
            }
        }
        else
        {
            enter_quad_mode_option = (uint32_t)config->quadMode;
        }

        if (enter_quad_mode_option != (uint32_t)kSerialNorQuadMode_NotConfig)
        {
            flexspi_transfer_t flashXfer;
            uint32_t status_val     = 0;
            flashXfer.deviceAddress = 0;
            flashXfer.port          = config->devicePort;
            flashXfer.cmdType       = kFLEXSPI_Read;
            flashXfer.data          = &status_val;
            flashXfer.dataSize      = 1;
            flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_READSTATUS;
            flashXfer.SeqNumber     = 1;

            FLEXSPI_UpdateLUT(base, NOR_CMD_LUT_SEQ_IDX_READSTATUS * 4U, lut_seq, sizeof(lut_seq) / sizeof(lut_seq[0]));

            /* Read status based on new LUT for Quad mode. */
            status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);
            if (status != kStatus_Success)
            {
                break;
            }

            /* Enable Quad mode if status register corresponding QE bit is 0. */
            switch (enter_quad_mode_option)
            {
                case (uint32_t)kSerialNorQuadMode_StatusReg1_Bit6:
                    if ((status_val & (1UL << 0x06)) != (1UL << 0x06))
                    {
                        (void)FLEXSPI_NOR_EnableQuadMode(handle, config);
                    };
                    break;
                case (uint32_t)kSerialNorQuadMode_StatusReg2_Bit1:
                    if ((status_val & (1UL << 0x08)) != (1UL << 0x08))
                    {
                        (void)FLEXSPI_NOR_EnableQuadMode(handle, config);
                    };
                    break;
                case (uint32_t)kSerialNorQuadMode_StatusReg2_Bit1_0x31:
                    if ((status_val & (1UL << 0x01)) != (1UL << 0x01))
                    {
                        (void)FLEXSPI_NOR_EnableQuadMode(handle, config);
                    };
                    break;
                case (uint32_t)kSerialNorQuadMode_StatusReg2_Bit7:
                    if ((status_val & (1UL << 0x07)) != (1UL << 0x07))
                    {
                        (void)FLEXSPI_NOR_EnableQuadMode(handle, config);
                    };
                    break;
                default:
                    config->deviceModeCfgEnable = false;
                    break;
            }

            /* Override status value if it is required by user. */
            if (config->statusOverride != 0x00U)
            {
                status_val = config->statusOverride;
            }

            /* Do modify-afer-read status and then create Quad mode Enable sequence. */
            config->deviceModeCfgEnable = true;

            switch (enter_quad_mode_option)
            {
                case (uint32_t)kSerialNorQuadMode_StatusReg1_Bit6:
                    /* QE is set via Write Status with one data byte where bit 6 is one. */
                    config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITESTATUS * 4U] =
                        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_WriteStatusReg1,
                                        kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x01);
                    status_val |= (1UL << 0x06);
                    break;
                case (uint32_t)kSerialNorQuadMode_StatusReg2_Bit1:
                    /* QE is set via Write Status instruction 01h with two data bytes where bit 1 of the second byte is
                     * one. */
                    config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITESTATUS * 4U] =
                        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_WriteStatusReg2Bit1,
                                        kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x02);
                    status_val |= (1UL << 1);
                    status_val <<= 8;
                    break;
                case (uint32_t)kSerialNorQuadMode_StatusReg2_Bit1_0x31:
                    /* QE is set via Write Status instruction 01h with two data bytes where bit 1 of the second byte is
                     * one. */
                    config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITESTATUS * 4U] = FLEXSPI_LUT_SEQ(
                        kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x31, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x01);
                    status_val |= (1UL << 1);
                    break;
                case (uint32_t)kSerialNorQuadMode_StatusReg2_Bit7:
                    /*QE is set via Write status register 2 instruction 3Eh with one data byte where bit 7 is one. */
                    config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITESTATUS * 4U] =
                        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_WriteStatusReg2Bit7,
                                        kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x01);
                    status_val |= (1UL << 0x07);
                    break;
                default:
                    config->deviceModeCfgEnable = false;
                    break;
            }

            /* Update LUT table. */
            FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_WRITESTATUS * 4UL,
                              &config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITESTATUS], 4);

            config->statusOverride = (uint8_t)status_val;
        }

        status = kStatus_Success;
    } while (false);

    return status;
}

static status_t FLEXSPI_NOR_EnableQuadMode(nor_handle_t *handle, flexspi_mem_config_t *config)
{
    flexspi_transfer_t flashXfer;
    flexspi_mem_nor_handle_t *memHandle = (flexspi_mem_nor_handle_t *)handle->deviceSpecific;

    flexspi_port_t port = memHandle->port;

    status_t status;
    uint32_t writeValue = config->statusOverride;

    /* Set write enable command instruction for LUT. */
    config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] = FLEXSPI_LUT_SEQ(
        kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialNorCmd_WriteEnable, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);

    /* Update LUT table. */
    FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_WRITEENABLE * 4U,
                      &config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE], 4);

    /* Write enable. */
    status = FLEXSPI_NOR_WriteEnable((FLEXSPI_Type *)handle->driverBaseAddr, port, 0);

    if (status != kStatus_Success)
    {
        return status;
    }

    /* Enable quad mode. */
    flashXfer.deviceAddress = 0;
    flashXfer.port          = port;
    flashXfer.cmdType       = kFLEXSPI_Write;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITESTATUS;
    flashXfer.data          = &writeValue;
    flashXfer.dataSize      = 1;

    status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);
    if (status != kStatus_Success)
    {
        return status;
    }

    status = FLEXSPI_NOR_WaitBusBusy((FLEXSPI_Type *)handle->driverBaseAddr, memHandle, 0);

    return status;
}

static status_t FLEXSPI_NOR_PreEnterOctalMode(nor_handle_t *handle,
                                              flexspi_mem_config_t *config,
                                              jedec_info_table_t *tbl)
{
    assert(handle != NULL);
    assert(config != NULL);

    status_t status                     = kStatus_Success;
    flexspi_mem_nor_handle_t *memHandle = (flexspi_mem_nor_handle_t *)handle->deviceSpecific;
    flexspi_port_t port                 = memHandle->port;
    flexspi_transfer_t flashXfer;
    uint32_t manufacturerId;

    /* ID */
    flashXfer.deviceAddress = 0;
    flashXfer.port          = port;
    flashXfer.cmdType       = kFLEXSPI_Read;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_READID;
    flashXfer.data          = &manufacturerId;
    flashXfer.dataSize      = 1;

    status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);

    config->manufacturerId = manufacturerId;

    /* Enter octal mode based on manufacture ID */
    switch (manufacturerId)
    {
        case kSerialNorFlash_MacronixManufacturerID:
            status = FLEXSPI_NOR_MacronixEnterOctalMode(handle, config, tbl);
            break;

        case kSerialNorFlash_Adesto32ManufacturerID:
            SUPPRESS_FALL_THROUGH_WARNING();
        case kSerialNorFlash_Adesto128ManufacturerID:
            status = FLEXSPI_NOR_AdestoEnterOctalMode(handle, config, tbl);
            break;

        default:
            assert(false);
            break;
    }

    return status;
}

static status_t FLEXSPI_NOR_MacronixEnterOctalMode(nor_handle_t *handle,
                                                   flexspi_mem_config_t *config,
                                                   jedec_info_table_t *tbl)
{
    assert(handle != NULL);
    assert(config != NULL);

    status_t status          = kStatus_Success;
    uint32_t enableOctalMode = 0U;
    flexspi_transfer_t flashXfer;
    flexspi_mem_nor_handle_t *memHandle              = (flexspi_mem_nor_handle_t *)handle->deviceSpecific;
    flexspi_port_t port                              = memHandle->port;
    uint8_t writeConfigRegCmd                        = tbl->xspi_profile_1_tb1.cmd_8d_8d_8d_inst.write_volatile_reg_cmd;
    jedec_basic_flash_param_table_t *basicFlashTable = &tbl->basic_flash_param_tbl;
    uint32_t lut_seq[4];

    (void)memset(lut_seq, 0, sizeof(lut_seq));

    /* Enter octal mode through write configuration register 2. Details in 19th DWORD, JEDEC basic flash parameter
     * table.*/
    if ((basicFlashTable->mode_8_8_info.mode_8s_8s_8s_enable_seq & 0x04U) == 0x04U)
    {
        if (config->transferMode == kSerialNorTransferMode_SDR)
        {
            enableOctalMode = kOctalSerialNor_STRModeMacronix;

            /* Enable DQS and read strobe, and make the timing input source from DQS pad.*/
            ((FLEXSPI_Type *)handle->driverBaseAddr)->MCR0 &= ~FLEXSPI_MCR0_RXCLKSRC_MASK;
            ((FLEXSPI_Type *)handle->driverBaseAddr)->MCR0 |=
                FLEXSPI_MCR0_RXCLKSRC(kFLEXSPI_ReadSampleClkExternalInputFromDqsPad);

            /* Set the command instruction of read status register for LUT in octal sdr mode. */
            lut_seq[0] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, kOctalSerialNorCmd_ReadStatusReg_1st,
                                         kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, kOctalSerialNorCmd_ReadStatusReg_2nd);
            lut_seq[1] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_8PAD, 0x20, kFLEXSPI_Command_DUMMY_SDR,
                                         kFLEXSPI_8PAD, 0x08);
            lut_seq[2] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP,
                                         kFLEXSPI_8PAD, 0x0);
        }
        else
        {
            enableOctalMode = kOctalSerialNor_DTRModeMacronix;

            /* Enable DQS and read strobe, and make the timing input source from DQS pad.*/
            ((FLEXSPI_Type *)handle->driverBaseAddr)->MCR0 &= ~FLEXSPI_MCR0_RXCLKSRC_MASK;
            ((FLEXSPI_Type *)handle->driverBaseAddr)->MCR0 |=
                FLEXSPI_MCR0_RXCLKSRC(kFLEXSPI_ReadSampleClkExternalInputFromDqsPad);

            /* Set the command instruction of read status register for LUT in octal ddr mode. */
            lut_seq[0] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, kOctalSerialNorCmd_ReadStatusReg_1st,
                                         kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, kOctalSerialNorCmd_ReadStatusReg_2nd);
            lut_seq[1] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x20, kFLEXSPI_Command_READ_DDR,
                                         kFLEXSPI_8PAD, 0x04);
        }
        /* Update LUT table for status register. */
        FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_READSTATUS * 4UL, lut_seq, 4);

        (void)memset(&config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_WRITECONFIG], 0, 4 * 4);
        /* Set the command instruction of write configuration register 2 for LUT in single mode. */
        config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4] = FLEXSPI_LUT_SEQ(
            kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, writeConfigRegCmd, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x20);
        config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4 + 1] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);
        /* Update LUT table for configuration register. */
        FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4UL,
                          &config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_WRITECONFIG], 4);

        /* Enable DQS on SDR mode */
        if (config->transferMode == kSerialNorTransferMode_SDR)
        {
            uint32_t enableDQS = 0x02U;

            /* Write enable. */
            status = FLEXSPI_NOR_WriteEnable((FLEXSPI_Type *)handle->driverBaseAddr, port, 0);

            if (status != kStatus_Success)
            {
                return status;
            }

            /* Enter octal mode. */
            flashXfer.deviceAddress = 0x200U;
            flashXfer.port          = port;
            flashXfer.cmdType       = kFLEXSPI_Write;
            flashXfer.SeqNumber     = 1;
            flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITECONFIG;
            flashXfer.data          = &enableDQS;
            flashXfer.dataSize      = 1;

            status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);
            if (status != kStatus_Success)
            {
                return status;
            }
        }

        /* Write enable. */
        status = FLEXSPI_NOR_WriteEnable((FLEXSPI_Type *)handle->driverBaseAddr, port, 0);

        if (status != kStatus_Success)
        {
            return status;
        }

        /* Enter octal mode. */
        flashXfer.deviceAddress = 0;
        flashXfer.port          = port;
        flashXfer.cmdType       = kFLEXSPI_Write;
        flashXfer.SeqNumber     = 1;
        flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITECONFIG;
        flashXfer.data          = &enableOctalMode;
        flashXfer.dataSize      = 1;

        status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);
        if (status != kStatus_Success)
        {
            return status;
        }

        status = FLEXSPI_NOR_WaitBusBusy((FLEXSPI_Type *)handle->driverBaseAddr, memHandle, 0);

        if (status != kStatus_Success)
        {
            return status;
        }

        if (config->transferMode == kSerialNorTransferMode_DDR)
        {
            config->CurrentCommandMode = kSerialNorCommandMode_8d_8d_8d;
        }
        else
        {
            config->CurrentCommandMode = kSerialNorCommandMode_8s_8s_8s;
        }
    }
    /* Enter octal mode through issue instruction 0x06/0xE8, details in JEDEC basic flash parameter table 19th DWORD */
    else if ((basicFlashTable->mode_8_8_info.mode_8s_8s_8s_enable_seq & 0x02U) == 0x02U)
    {
        /* Enable DQS and read strobe, and make the timing input source from DQS pad.*/
        ((FLEXSPI_Type *)handle->driverBaseAddr)->MCR0 &= ~FLEXSPI_MCR0_RXCLKSRC_MASK;
        ((FLEXSPI_Type *)handle->driverBaseAddr)->MCR0 |=
            FLEXSPI_MCR0_RXCLKSRC(kFLEXSPI_ReadSampleClkExternalInputFromDqsPad);

        /* Set the command instruction of read status register for LUT in octal sdr mode. */
        lut_seq[0] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0x05, kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_8PAD, 0x04);
        lut_seq[1] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_8PAD, 0x0);

        /* Enter octal mode */
        status = FLEXSPI_NOR_WriteEnable((FLEXSPI_Type *)handle->driverBaseAddr, port, 0);

        status = FLEXSPI_NOR_WaitBusBusy((FLEXSPI_Type *)handle->driverBaseAddr, memHandle, 0);
        FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_READSTATUS * 4UL, lut_seq, 4);

        (void)memset(&config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4U], 0, 4 * 4);
        config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4U] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0xE8, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);
        FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4U,
                          &config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITECONFIG], 4);

        flashXfer.deviceAddress = 0;
        flashXfer.port          = port;
        flashXfer.cmdType       = kFLEXSPI_Command;
        flashXfer.SeqNumber     = 1;
        flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITECONFIG;

        status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);

        if (status != kStatus_Success)
        {
            return status;
        }

        status = FLEXSPI_NOR_WaitBusBusy((FLEXSPI_Type *)handle->driverBaseAddr, memHandle, 0);

        if (status != kStatus_Success)
        {
            return status;
        }

        config->CurrentCommandMode = kSerialNorCommandMode_8s_8s_8s;

        if (config->transferMode == kSerialNorTransferMode_DDR)
        {
            (void)memset(lut_seq, 0, sizeof(lut_seq));
            /* Set the command instruction of read status register for LUT in octal ddr mode. */
            lut_seq[0] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x05, kFLEXSPI_Command_DUMMY_DDR,
                                         kFLEXSPI_8PAD, 0x03);
            lut_seq[1] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_DDR, kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP,
                                         kFLEXSPI_8PAD, 0x0);
            FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_READSTATUS * 4UL, lut_seq, 4);

            /* Update write enable LUT table and current command mode for octal sdr mode */
            (void)memset(&config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE], 0, 4 * 4);
            config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] =
                FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0x06, kFLEXSPI_Command_STOP, kFLEXSPI_8PAD, 0);
            FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_WRITEENABLE * 4UL,
                              &config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE], 4);

            /* Set the command instruction of write configuration register 2 for LUT in octal sdr mode. */
            (void)memset(&config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4], 0, 4 * 4);
            config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4] = FLEXSPI_LUT_SEQ(
                kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0x31, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_8PAD, 0x01);
            FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4UL,
                              &config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITECONFIG], 4);

            status = FLEXSPI_NOR_WriteEnable((FLEXSPI_Type *)handle->driverBaseAddr, port, 0);

            /* Enable DDR mode */
            uint32_t enableDdrMode = 0x88U;

            flashXfer.deviceAddress = 0;
            flashXfer.port          = port;
            flashXfer.cmdType       = kFLEXSPI_Write;
            flashXfer.SeqNumber     = 1;
            flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITECONFIG;
            flashXfer.data          = &enableDdrMode;
            flashXfer.dataSize      = 1;

            status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);

            if (status != kStatus_Success)
            {
                return status;
            }

            status = FLEXSPI_NOR_WaitBusBusy((FLEXSPI_Type *)handle->driverBaseAddr, memHandle, 0);

            if (status != kStatus_Success)
            {
                return status;
            }

            /* Update command mode into 8d-8d-8d */
            config->CurrentCommandMode = kSerialNorCommandMode_8d_8d_8d;
        }
        else
        {
            ; /* Intentional empty */
        }
    }
    else
    {
        ; /* Intentional empty */
    }

    /* Check if having entered octal mode successfully or not */
    status = FLEXSPI_NOR_CheckCommandModeAvailability(handle, config, tbl);

    return status;
}

static status_t FLEXSPI_NOR_MacronixExitOctalMode(nor_handle_t *handle,
                                                  flexspi_mem_config_t *config,
                                                  jedec_info_table_t *tbl)
{
    assert(handle != NULL);
    assert(config != NULL);

    status_t status = kStatus_Success;
    flexspi_transfer_t flashXfer;
    flexspi_mem_nor_handle_t *memHandle              = (flexspi_mem_nor_handle_t *)handle->deviceSpecific;
    flexspi_port_t port                              = memHandle->port;
    jedec_basic_flash_param_table_t *basicFlashTable = &tbl->basic_flash_param_tbl;

    /* Exit octal mode through issuing the soft reset command. Details in the 19th DWORD, JEDEC basic flash parameter
     * table.*/
    if ((basicFlashTable->mode_8_8_info.mode_8s_8s_8s_disable_seq & 0x08U) == 0x08U)
    {
        (void)memset(&config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4U], 0, 4 * 8);

        if (config->transferMode == kSerialNorTransferMode_SDR)
        {
            config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4U] =
                FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0x66, kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0x99);
            config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4U + 4U] =
                FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0x99, kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0x66);
        }
        else
        {
            config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4U] =
                FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x66, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x99);
            config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4U + 4U] =
                FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x99, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x66);
        }

        FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4U,
                          &config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITECONFIG], 8);

        flashXfer.deviceAddress = 0;
        flashXfer.port          = port;
        flashXfer.cmdType       = kFLEXSPI_Command;
        flashXfer.SeqNumber     = 2;
        flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITECONFIG;

        status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);

        if (status != kStatus_Success)
        {
            return status;
        }

        /* Update RX sample clock source: disable DQS and read strobe.*/
        ((FLEXSPI_Type *)handle->driverBaseAddr)->MCR0 &= ~FLEXSPI_MCR0_RXCLKSRC_MASK;
        ((FLEXSPI_Type *)handle->driverBaseAddr)->MCR0 |=
            FLEXSPI_MCR0_RXCLKSRC(kFLEXSPI_ReadSampleClkLoopbackFromDqsPad);
    }
    else
    {
        /* TODO: for xxx1b with issue command */
        status = kStatus_FLEXSPINOR_UnsupportedOctalModeEnterMethod;
    }

    if (status == kStatus_Success)
    {
        config->CurrentCommandMode = kSerialNorCommandMode_1_1_1;
    }

    return status;
}

static status_t FLEXSPI_NOR_CheckCommandModeAvailability(nor_handle_t *handle,
                                                         flexspi_mem_config_t *config,
                                                         jedec_info_table_t *tbl)
{
    assert(handle != NULL);
    assert(config != NULL);
    assert(tbl != NULL);

    status_t status                     = kStatus_Success;
    flexspi_mem_nor_handle_t *memHandle = (flexspi_mem_nor_handle_t *)handle->deviceSpecific;
    flexspi_port_t port                 = memHandle->port;
    uint8_t readValue[4]                = {0x55U, 0x55U, 0x55U, 0x55U};
    uint8_t currentCommandMode          = (uint8_t)config->CurrentCommandMode;
    uint8_t readConfigRegCmd            = tbl->xspi_profile_1_tb1.cmd_8d_8d_8d_inst.read_volatile_reg_cmd;
    uint32_t manufacturerId             = config->manufacturerId;
    flexspi_transfer_t flashXfer;

    (void)memset(&config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READCONFIG * 4], 0, 4 * 4);

    if (manufacturerId == (uint32_t)kSerialNorFlash_MacronixManufacturerID)
    {
        /* Command extension to 16 bits */
        if (tbl->basic_flash_param_tbl.octal_1_8_info.octal_dtr_cmd_and_cmd_extension == 0x01U)
        {
            /* Read configuration register 2 */
            if (currentCommandMode == (uint8_t)kSerialNorCommandMode_8d_8d_8d)
            {
                config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READCONFIG * 4U] = FLEXSPI_LUT_SEQ(
                    kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, readConfigRegCmd, kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x8E);
                config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READCONFIG * 4U + 1U] = FLEXSPI_LUT_SEQ(
                    kFLEXSPI_Command_RADDR_DDR, kFLEXSPI_8PAD, 0x20, kFLEXSPI_Command_READ_DDR, kFLEXSPI_8PAD, 0x04);
            }
            else if (currentCommandMode == (uint8_t)kSerialNorCommandMode_8s_8s_8s)
            {
                config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READCONFIG * 4U] = FLEXSPI_LUT_SEQ(
                    kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, readConfigRegCmd, kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0x8E);
                config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READCONFIG * 4U + 1U] = FLEXSPI_LUT_SEQ(
                    kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_8PAD, 0x20, kFLEXSPI_Command_READ_SDR, kFLEXSPI_8PAD, 0x04);
            }
            else if (currentCommandMode == (uint8_t)kSerialNorCommandMode_1_1_1)
            {
                config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READCONFIG * 4U] =
                    FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, readConfigRegCmd, kFLEXSPI_Command_RADDR_SDR,
                                    kFLEXSPI_1PAD, 0x20);
                config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READCONFIG * 4U + 1U] = FLEXSPI_LUT_SEQ(
                    kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);
            }
            else
            {
                ; /* TBD */
            }

            FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_READCONFIG * 4UL,
                              &config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_READCONFIG], 4);

            flashXfer.deviceAddress = 0;
            flashXfer.port          = port;
            flashXfer.cmdType       = kFLEXSPI_Read;
            flashXfer.SeqNumber     = 1;
            flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_READCONFIG;
            flashXfer.data          = (uint32_t *)&readValue[0];
            flashXfer.dataSize      = 1;

            status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);

            if (currentCommandMode == (uint8_t)kSerialNorCommandMode_8d_8d_8d)
            {
                if (readValue[0] != (uint8_t)kOctalSerialNor_DTRModeMacronix)
                {
                    status = kStatus_FLEXSPINOR_EnterOctalModeFailed;
                }
            }
            else if (currentCommandMode == (uint8_t)kSerialNorCommandMode_8s_8s_8s)
            {
                if (readValue[0] != (uint8_t)kOctalSerialNor_STRModeMacronix)
                {
                    status = kStatus_FLEXSPINOR_EnterOctalModeFailed;
                }
            }
            else if (currentCommandMode == (uint8_t)kSerialNorCommandMode_1_1_1)
            {
                if (readValue[0] != (uint8_t)kOctalSerialNor_SPIModeMacronix)
                {
                    status = kStatus_FLEXSPINOR_EnterOctalModeFailed;
                }
            }
        }
        /* No command extension */
        else
        {
            ; /* TBD */
        }
    }
    else if (manufacturerId == (uint32_t)kSerialNorFlash_Adesto128ManufacturerID ||
             manufacturerId == (uint32_t)kSerialNorFlash_Adesto32ManufacturerID)
    {
        /* Command extension to 16 bits */
        if (tbl->basic_flash_param_tbl.octal_1_8_info.octal_dtr_cmd_and_cmd_extension == 0x01U)
        {
            ; /* TBD */
        }
        /* No command extension */
        else
        {
            /* Read all status and control register */
            /* For Adesto octal flash, 1 byte address is required following the command instruction.
               However, 4 bytes also applies. So, 4 bytes address may probably be set in default for other vendors.*/
            if (currentCommandMode == (uint8_t)kSerialNorCommandMode_8d_8d_8d)
            {
                /* The reason why make command instruction as CMD_SDR is that it must be in SDR mode if
                   the octal flash timing requires first command instruction. */
                config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READCONFIG * 4U] =
                    FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, readConfigRegCmd, kFLEXSPI_Command_RADDR_DDR,
                                    kFLEXSPI_8PAD, 0x20);
                config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READCONFIG * 4U + 1U] = FLEXSPI_LUT_SEQ(
                    kFLEXSPI_Command_DUMMY_DDR, kFLEXSPI_8PAD, 4, kFLEXSPI_Command_READ_DDR, kFLEXSPI_8PAD, 0x04);
            }
            else if (currentCommandMode == (uint8_t)kSerialNorCommandMode_8s_8s_8s)
            {
                config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READCONFIG * 4U] =
                    FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, readConfigRegCmd, kFLEXSPI_Command_RADDR_SDR,
                                    kFLEXSPI_8PAD, 0x20);
                config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READCONFIG * 4U + 1U] = FLEXSPI_LUT_SEQ(
                    kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_8PAD, 4, kFLEXSPI_Command_READ_SDR, kFLEXSPI_8PAD, 0x04);
            }
            else if (currentCommandMode == (uint8_t)kSerialNorCommandMode_1_1_1)
            {
                config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READCONFIG * 4U] =
                    FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, readConfigRegCmd, kFLEXSPI_Command_RADDR_SDR,
                                    kFLEXSPI_1PAD, 0x20);
                config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READCONFIG * 4U + 1U] = FLEXSPI_LUT_SEQ(
                    kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 8, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04);
            }
            else
            {
                ; /* TBD */
            }

            FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_READCONFIG * 4UL,
                              &config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_READCONFIG], 4);

            flashXfer.deviceAddress = 0;
            flashXfer.port          = port;
            flashXfer.cmdType       = kFLEXSPI_Read;
            flashXfer.SeqNumber     = 1;
            flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_READCONFIG;
            flashXfer.data          = (uint32_t *)&readValue[0];
            flashXfer.dataSize      = 4;

            status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);

            /*
             * Volatile register address map
             * ------------------------------------------
             * Address | Register name
             * ==========================================
             * 0       | Reserved - read as 0
             * -------------------------------------------
             * 1       | Status Register Byte 1
             * -------------------------------------------
             * 2       | Status Register Byte 2
             * -------------------------------------------
             * 3       | Status/Control Register Byte 3
             * -------------------------------------------
             * 4       | Status/Control Register Byte 4
             * ----------------------------------------- */
            /* Status register 2 (adesto DS)
             * bit-7: DTR(1), STR(1)
             * bit-2 and bit-3: SPI(00), QPI(01), OPI(10)
             */
            if (currentCommandMode == (uint8_t)kSerialNorCommandMode_8d_8d_8d)
            {
                if (readValue[2] != (uint8_t)kOctalSerialNor_DTRModeAdesto)
                {
                    status = kStatus_FLEXSPINOR_EnterOctalModeFailed;
                }
            }
            else if (currentCommandMode == (uint8_t)kSerialNorCommandMode_8s_8s_8s)
            {
                if (readValue[2] != (uint8_t)kOctalSerialNor_STRModeAdesto)
                {
                    status = kStatus_FLEXSPINOR_EnterOctalModeFailed;
                }
            }
            else if (currentCommandMode == (uint8_t)kSerialNorCommandMode_1_1_1)
            {
                if (readValue[2] != (uint8_t)kOctalSerialNor_SPIModeAdesto)
                {
                    status = kStatus_FLEXSPINOR_EnterOctalModeFailed;
                }
            }

            /* Check software protection status, adesto octal flash should be protected in default */
            if ((readValue[1] & 0x0CU) == 0x0CU)
            {
                uint32_t globalUnprotect = 0x00U;

                (void)memset(&config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE], 0, 4 * 4);
                (void)memset(&config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITESTATUS], 0, 4 * 4);
                if (config->transferMode == kSerialNorTransferMode_SDR)
                {
                    /* Set write enable instruction for LUT in octal sdr mode. */
                    config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] = FLEXSPI_LUT_SEQ(
                        kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0x06, kFLEXSPI_Command_STOP, kFLEXSPI_8PAD, 0);

                    /* Set write stauts register 1 instruction for LUT in octal sdr mode. */
                    config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITESTATUS * 4] = FLEXSPI_LUT_SEQ(
                        kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0x01, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_8PAD, 0x04);
                }
                else
                {
                    /* Set write enable instruction for LUT in octal ddr mode. */
                    config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] = FLEXSPI_LUT_SEQ(
                        kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x06, kFLEXSPI_Command_STOP, kFLEXSPI_8PAD, 0);

                    /* Set write stauts register 1 instruction for LUT in octal ddr mode. */
                    /* The reason why make command instruction as CMD_SDR is that it must be in SDR mode if
                   the octal flash timing requires first command instruction. */
                    config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITESTATUS * 4U] = FLEXSPI_LUT_SEQ(
                        kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0x01, kFLEXSPI_Command_WRITE_DDR, kFLEXSPI_8PAD, 0x04);
                }
                /* Update write enable LUT table */
                FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_WRITEENABLE * 4U,
                                  &config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE], 4);
                /* Update status register 1 LUT table. */
                FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_WRITESTATUS * 4U,
                                  &config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITESTATUS], 4);

                /* Global unprotect for entire memory array */
                if ((readValue[1] & 0x80U) == 0x00U)
                {
                    /* Write enable. */
                    (void)FLEXSPI_NOR_WriteEnable((FLEXSPI_Type *)handle->driverBaseAddr, port, 0);

                    flashXfer.deviceAddress = 0;
                    flashXfer.port          = port;
                    flashXfer.cmdType       = kFLEXSPI_Write;
                    flashXfer.SeqNumber     = 1;
                    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITESTATUS;
                    flashXfer.data          = &globalUnprotect;
                    flashXfer.dataSize      = 1;

                    status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);
                }
                else
                {
                    /* SPRL be in 0x01 and set it into 0x00 */
                    readValue[0] = 0x00U;

                    /* Write enable. */
                    (void)FLEXSPI_NOR_WriteEnable((FLEXSPI_Type *)handle->driverBaseAddr, port, 0);

                    flashXfer.deviceAddress = 0;
                    flashXfer.port          = port;
                    flashXfer.cmdType       = kFLEXSPI_Write;
                    flashXfer.SeqNumber     = 1;
                    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITESTATUS;
                    flashXfer.data          = (uint32_t *)&readValue[0];
                    flashXfer.dataSize      = 1;

                    (void)FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);

                    /* LUT table update */
                    flashXfer.deviceAddress = 0;
                    flashXfer.port          = port;
                    flashXfer.cmdType       = kFLEXSPI_Read;
                    flashXfer.SeqNumber     = 1;
                    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_READCONFIG;
                    flashXfer.data          = (uint32_t *)&readValue[0];
                    flashXfer.dataSize      = 1;

                    (void)FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);

                    /* Global unprotect for entire memory array */
                    if ((readValue[0] & 0x80U) == 0x00U)
                    {
                        /* Write enable. */
                        (void)FLEXSPI_NOR_WriteEnable((FLEXSPI_Type *)handle->driverBaseAddr, port, 0);

                        flashXfer.deviceAddress = 0;
                        flashXfer.port          = port;
                        flashXfer.cmdType       = kFLEXSPI_Write;
                        flashXfer.SeqNumber     = 1;
                        flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITESTATUS;
                        flashXfer.data          = (uint32_t *)&globalUnprotect;
                        flashXfer.dataSize      = 1;

                        status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);
                    }
                    else
                    {
                        status = kStatus_FLEXSPINOR_UnprotectMemoryFailed;
                    }
                }

                if (kStatus_Success == status)
                {
                    flashXfer.deviceAddress = 0;
                    flashXfer.port          = port;
                    flashXfer.cmdType       = kFLEXSPI_Read;
                    flashXfer.SeqNumber     = 1;
                    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_READCONFIG;
                    flashXfer.data          = (uint32_t *)&readValue[0];
                    flashXfer.dataSize      = 4;

                    status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);

                    if ((readValue[1] & 0x0CU) != 0x00U)
                    {
                        status = kStatus_FLEXSPINOR_UnprotectMemoryFailed;
                    }
                }
            }
            else
            {
                ;
            }
        }
    }
    else
    {
        ;
    }

    return status;
}

static status_t FLEXSPI_NOR_AdestoEnterOctalMode(nor_handle_t *handle,
                                                 flexspi_mem_config_t *config,
                                                 jedec_info_table_t *tbl)
{
    assert(handle != NULL);
    assert(config != NULL);
    assert(tbl != NULL);

    status_t status = kStatus_Success;
    flexspi_transfer_t flashXfer;
    flexspi_mem_nor_handle_t *memHandle              = (flexspi_mem_nor_handle_t *)handle->deviceSpecific;
    flexspi_port_t port                              = memHandle->port;
    uint32_t enableDdrMode                           = 0x88U;
    jedec_basic_flash_param_table_t *basicFlashTable = &tbl->basic_flash_param_tbl;
    uint32_t lut_seq[4];

    (void)memset(lut_seq, 0, sizeof(lut_seq));

    /* Enter octal mode through issue command, details in JEDEC basic flash parameter table 19th DWORD */
    if ((basicFlashTable->mode_8_8_info.mode_8s_8s_8s_enable_seq & 0x02U) == 0x02U)
    {
        /* Enable DQS and read strobe, and make the timing input source from DQS pad.*/
        ((FLEXSPI_Type *)handle->driverBaseAddr)->MCR0 &= ~FLEXSPI_MCR0_RXCLKSRC_MASK;
        ((FLEXSPI_Type *)handle->driverBaseAddr)->MCR0 |=
            FLEXSPI_MCR0_RXCLKSRC(kFLEXSPI_ReadSampleClkExternalInputFromDqsPad);

        /* Set the command instruction of read status register for LUT in octal sdr mode. */
        lut_seq[0] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0x05, kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_8PAD, 0x04);
        lut_seq[1] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_8PAD, 0x0);

        /* Enter octal mode */
        (void)FLEXSPI_NOR_WriteEnable((FLEXSPI_Type *)handle->driverBaseAddr, port, 0);

        FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_READSTATUS * 4UL, lut_seq, 4);

        (void)memset(&config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4U], 0, 4 * 4);
        config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4U] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0xE8, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);
        FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4UL,
                          &config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITECONFIG], 4);

        flashXfer.deviceAddress = 0;
        flashXfer.port          = port;
        flashXfer.cmdType       = kFLEXSPI_Command;
        flashXfer.SeqNumber     = 1;
        flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITECONFIG;

        status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);

        if (status != kStatus_Success)
        {
            return status;
        }

        status = FLEXSPI_NOR_WaitBusBusy((FLEXSPI_Type *)handle->driverBaseAddr, memHandle, 0);

        /* Update command mode into 8s-8s-8s mode */
        config->CurrentCommandMode = kSerialNorCommandMode_8s_8s_8s;

        if (config->transferMode == kSerialNorTransferMode_DDR)
        {
            (void)memset(lut_seq, 0, sizeof(lut_seq));
            /* Set the command instruction of read status register for LUT in octal ddr mode. */
            lut_seq[0] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DDR, kFLEXSPI_8PAD, 0x05, kFLEXSPI_Command_DUMMY_DDR,
                                         kFLEXSPI_8PAD, 0x03);
            lut_seq[1] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_DDR, kFLEXSPI_8PAD, 0x04, kFLEXSPI_Command_STOP,
                                         kFLEXSPI_8PAD, 0x0);
            FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_READSTATUS * 4UL, lut_seq, 4);

            /* Update write enable LUT table and current command mode for octal sdr mode */
            (void)memset(&config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE], 0, 4 * 4);
            config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] =
                FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0x06, kFLEXSPI_Command_STOP, kFLEXSPI_8PAD, 0);
            FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_WRITEENABLE * 4U,
                              &config->lookupTable[4 * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE], 4);

            /* Set the command instruction of write configuration register 2 for LUT in octal sdr mode. */
            (void)memset(&config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4U], 0, 4 * 4);
            config->lookupTable[NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4U] = FLEXSPI_LUT_SEQ(
                kFLEXSPI_Command_SDR, kFLEXSPI_8PAD, 0x31, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_8PAD, 0x01);
            FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_WRITECONFIG * 4U,
                              &config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_WRITECONFIG], 4);

            (void)FLEXSPI_NOR_WriteEnable((FLEXSPI_Type *)handle->driverBaseAddr, port, 0);

            flashXfer.deviceAddress = 0;
            flashXfer.port          = port;
            flashXfer.cmdType       = kFLEXSPI_Write;
            flashXfer.SeqNumber     = 1;
            flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_WRITECONFIG;
            flashXfer.data          = &enableDdrMode;
            flashXfer.dataSize      = 1;

            status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);

            if (status != kStatus_Success)
            {
                return status;
            }

            status = FLEXSPI_NOR_WaitBusBusy((FLEXSPI_Type *)handle->driverBaseAddr, memHandle, 0);

            /* Update command mode into 8d-8d-8d */
            config->CurrentCommandMode = kSerialNorCommandMode_8d_8d_8d;
        }
    }
    else
    {
        /* TODO: for x_x1xxb with write config register 2 */
        status = kStatus_FLEXSPINOR_UnsupportedOctalModeEnterMethod;
    }

    if (kStatus_Success == status)
    {
        status = FLEXSPI_NOR_CheckCommandModeAvailability(handle, config, tbl);
    }

    return status;
}

/*
 * Probe API only supports change from octal mode into standard SPI mode currently.
 * TBD: will support from quad mode.
 */
static status_t FLEXSPI_NOR_ProbeCommandMode(nor_handle_t *handle, flexspi_mem_config_t *config)
{
    status_t status                     = kStatus_InvalidArgument;
    flexspi_mem_nor_handle_t *memHandle = (flexspi_mem_nor_handle_t *)handle->deviceSpecific;
    flexspi_port_t port                 = memHandle->port;
    uint8_t i;
    sfdp_header_t sfdp_header;
    jedec_info_table_t tbl;

    /*
     * Read SFDP command protocol based on JESD216C/D
     *
     * The Read SFDP command can be used with devices supporting modes of (1-1-1), (2-2-2), (4-4-4), or (4S-4D-4D),
     * but the instruction (5Ah), address (24 bits), eight wait states, and 50 MHz requirements remain the same.
     */
    const lut_seq_t k_sdfp_lut[1] = {
        /* Read SFDP LUT sequence for 1 pad instruction. */
        {{FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialFlash_ReadSFDP, kFLEXSPI_Command_RADDR_SDR,
                          kFLEXSPI_1PAD, 24),
          FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 8, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0xFF),
          0, 0}}};
    /* Zero initialization */
    (void)memset(&sfdp_header, 0, sizeof(sfdp_header));

    for (i = 0U; i < (uint8_t)kSerialNorCommandMode_max; i++)
    {
        FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_READ_SFDP * 4U,
                          (const uint32_t *)&k_sdfp_lut[0], 4);

        status = FLEXSPI_NOR_ReadSFDP((FLEXSPI_Type *)handle->driverBaseAddr, 0, (uint32_t *)&sfdp_header,
                                      sizeof(sfdp_header));
        if (status != kStatus_Success)
        {
            status = kStatus_FLEXSPINOR_Unsupported_SFDP_Version;
        }

        if (sfdp_header.signature != SFDP_JESD216_SIGNATURE || sfdp_header.major_rev != SFDP_JESD216_MAJOR)
        {
            /* Exit octal command mode */
            if ((i == (uint8_t)kSerialNorCommandMode_8d_8d_8d) || (i == (uint8_t)kSerialNorCommandMode_8s_8s_8s))
            {
                /*
                 * There are two methods to change from octal mode(8-8-8) into standard SPI mode(1-1-1) based on
                 * JESD216C/D spec. First method: Issue the Soft Reset 66/99 sequence, which is named as software reset
                 * and any on-going internal operations will be terminated and the device will return to its default
                 * power-on state(The device will always revert back to Standard SPI Mode after a Reset) and lose all
                 * the current volatile settings. Second method: Issue instruction 06h (WREN), then issue FFh
                 * instruction.
                 */
                tbl.basic_flash_param_tbl.mode_8_8_info.mode_8s_8s_8s_disable_seq = 0x08U;

                do
                {
                    config->CurrentCommandMode = kSerialNorCommandMode_8d_8d_8d;

                    /*
                     * Revert back to standard SPI mode
                     *
                     * Data corruption may happen if there is an on-going or suspended internal Erase or Program
                     * operation in progress when a software reset instruction sequence(First Method back to standard
                     * SPI mode) is accepted by device.
                     */
                    /* TBD: It is recommended to check the RDY/BSY bit in Status Register Byte 1 and the PS and ES bits
                       in Status Register Byte 2 before issuing the Reset instruction sequence. */
                    status = FLEXSPI_NOR_MacronixExitOctalMode(handle, config, &tbl);

                    if (status != kStatus_Success)
                    {
                        tbl.basic_flash_param_tbl.mode_8_8_info.mode_8s_8s_8s_disable_seq = 0x01U;
                    }

                    /* Check standard SPI mode depending on signature */
                    status = FLEXSPI_NOR_ReadSFDP((FLEXSPI_Type *)handle->driverBaseAddr, 0, (uint32_t *)&sfdp_header,
                                                  sizeof(sfdp_header));
                    if (status != kStatus_Success)
                    {
                        status = kStatus_FLEXSPINOR_Unsupported_SFDP_Version;
                    }

                    if (sfdp_header.signature != SFDP_JESD216_SIGNATURE || sfdp_header.major_rev != SFDP_JESD216_MAJOR)
                    {
                        status = kStatus_FLEXSPINOR_Unsupported_SFDP_Version;
                    }

                    /* ID */
                    (void)memset(&config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_READID], 0, 4U * 4U);
                    config->lookupTable[NOR_CMD_LUT_SEQ_IDX_READID * 4U] =
                        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialFlash_ReadJEDECId,
                                        kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04);
                    FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_READID * 4U,
                                      &config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_READID], 4);

                    /* ID */
                    if (1)
                    {
                        uint32_t manufacturerId = 0x55U;
                        flexspi_transfer_t flashXfer;
                        flashXfer.deviceAddress = 0;
                        flashXfer.port          = port;
                        flashXfer.cmdType       = kFLEXSPI_Read;
                        flashXfer.SeqNumber     = 1;
                        flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_READID;
                        flashXfer.data          = &manufacturerId;
                        flashXfer.dataSize      = 1;

                        (void)FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);

                        flashXfer.dataSize = 2;
                    }
                } while (status != kStatus_Success);
            }
        }
        else
        {
            config->CurrentCommandMode = kSerialNorCommandMode_1_1_1;
            status                     = kStatus_Success;
            break;
        }
    }

    return status;
}

static status_t FLEXSPI_NOR_GenerateConfigBlockUsingSFDP(nor_handle_t *handle, flexspi_mem_config_t *config)
{
    assert(handle != NULL);
    assert(config != NULL);

    status_t status = kStatus_InvalidArgument;

    /*
     * Read SFDP command protocol based on JESD216C/D
     *
     * The Read SFDP command can be used with device supported modes of (1-1-1), (2-2-2), (4-4-4), or (4S-4D-4D),
     * but the instruction (5Ah), address (24 bits), eight wait states, and 50 MHz requirements remain the same.
     */
    const lut_seq_t k_sdfp_lut[4] = {
        /* Read SFDP LUT sequence in 1-1-1 mode. */
        {{FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, kSerialFlash_ReadSFDP, kFLEXSPI_Command_RADDR_SDR,
                          kFLEXSPI_1PAD, 24),
          FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 8, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0xFF),
          0, 0}},
        /* Read SFDP LUT sequence in 2-2-2 mode. */
        {{FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_2PAD, kSerialFlash_ReadSFDP, kFLEXSPI_Command_RADDR_SDR,
                          kFLEXSPI_2PAD, 24),
          FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_2PAD, 8, kFLEXSPI_Command_READ_SDR, kFLEXSPI_2PAD, 0xFF),
          0, 0}},
        /* Read SFDP LUT sequence in 4s-4s-4s mode. */
        {{FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, kSerialFlash_ReadSFDP, kFLEXSPI_Command_RADDR_SDR,
                          kFLEXSPI_4PAD, 24),
          FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, 8, kFLEXSPI_Command_READ_SDR, kFLEXSPI_4PAD, 0xFF),
          0, 0}},
        /* Reserved */
        {{0, 0, 0, 0}}};

    do
    {
        /* Probe current command mode and retore standard SPI protocol(1-1-1 mode) */
        status = FLEXSPI_NOR_ProbeCommandMode(handle, config);
        if (status != kStatus_Success)
        {
            break;
        }

        /* This will typically be used when the host controller and slave memory device are communicating in single
           SPI mode after being powered on, and the host controller will read SFDP in this mode. */
        if (config->queryPads != kFLEXSPI_1PAD)
        {
            break;
        }

        if (config->enhanceMode == kSerialNorEnhanceMode_0_4_4_Mode)
        {
            /* Try to exit 0-4-4 mode. */
            config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_EXIT_NOCMD] =
                FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, 0xFF, kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, 0xFF);
            config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_EXIT_NOCMD + 1U] =
                FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, 0xFF, kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, 0xFF);
            config->lookupTable[4U * NOR_CMD_LUT_SEQ_IDX_EXIT_NOCMD + 2U] =
                FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, 0xFF, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0);

            status = FLEXSPI_NOR_ExitNoCMDMode(handle, config, 0);
            if (status != kStatus_Success)
            {
                break;
            }
        }

        /* Read SFDP, probe whether the Flash device is present or not. */
        jedec_info_table_t jedec_info_tbl;

        (void)memset(&jedec_info_tbl, 0x00, sizeof(jedec_info_tbl));
        FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_READ_SFDP * 4U,
                          (const uint32_t *)&k_sdfp_lut[config->queryPads], 4);

        status = FLEXSPI_NOR_ReadSFDPInfo((FLEXSPI_Type *)handle->driverBaseAddr, &jedec_info_tbl);
        if (status != kStatus_Success)
        {
            break;
        }

        status = FLEXSPI_NOR_ParseSFDP(handle, config, &jedec_info_tbl);

    } while (false);

    return status;
}

status_t Nor_Flash_Erase_Chip(nor_handle_t *handle)
{
    assert(handle != NULL);

    flexspi_mem_nor_handle_t *memHandle = (flexspi_mem_nor_handle_t *)handle->deviceSpecific;
    flexspi_port_t port                 = memHandle->port;
    status_t status;
    flexspi_transfer_t flashXfer;

    /* Write enable */
    status = FLEXSPI_NOR_WriteEnable((FLEXSPI_Type *)handle->driverBaseAddr, port, 0);
    if (status != kStatus_Success)
    {
        return status;
    }

    /* Send erase command. */
    flashXfer.deviceAddress = 0;
    flashXfer.port          = port;
    flashXfer.cmdType       = kFLEXSPI_Command;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_CHIPERASE;

    status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);
    if (status != kStatus_Success)
    {
        return status;
    }

    status = FLEXSPI_NOR_WaitBusBusy((FLEXSPI_Type *)handle->driverBaseAddr, memHandle, 0);
    if (status != kStatus_Success)
    {
        return status;
    }

    return status;
}

status_t Nor_Flash_Erase_Block(nor_handle_t *handle, uint32_t address, uint32_t size_Byte)
{
    assert(handle != NULL);

    flexspi_mem_nor_handle_t *memHandle = (flexspi_mem_nor_handle_t *)handle->deviceSpecific;
    flexspi_port_t port                 = memHandle->port;
    status_t status;
    flexspi_transfer_t flashXfer;

    /* Write enable */
    status = FLEXSPI_NOR_WriteEnable((FLEXSPI_Type *)handle->driverBaseAddr, port, address);
    if (status != kStatus_Success)
    {
        return status;
    }

    /* Send erase command. */
    flashXfer.deviceAddress = address;
    flashXfer.port          = port;
    flashXfer.cmdType       = kFLEXSPI_Command;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_ERASEBLOCK;
    flashXfer.dataSize      = size_Byte;

    status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);
    if (status != kStatus_Success)
    {
        return status;
    }

    status = FLEXSPI_NOR_WaitBusBusy((FLEXSPI_Type *)handle->driverBaseAddr, memHandle, address);
    if (status != kStatus_Success)
    {
        return status;
    }

    return status;
}

status_t Nor_Flash_Erase_Sector(nor_handle_t *handle, uint32_t address, uint32_t size_Byte)
{
    assert(handle != NULL);

    flexspi_mem_nor_handle_t *memHandle = (flexspi_mem_nor_handle_t *)handle->deviceSpecific;
    flexspi_port_t port                 = memHandle->port;
    status_t status;
    flexspi_transfer_t flashXfer;

    /* Write enable */
    status = FLEXSPI_NOR_WriteEnable((FLEXSPI_Type *)handle->driverBaseAddr, port, address);
    if (status != kStatus_Success)
    {
        return status;
    }

    /* Send erase command. */
    flashXfer.deviceAddress = address;
    flashXfer.port          = port;
    flashXfer.cmdType       = kFLEXSPI_Command;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_ERASESECTOR;
    flashXfer.dataSize      = size_Byte;

    status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);
    if (status != kStatus_Success)
    {
        return status;
    }

    status = FLEXSPI_NOR_WaitBusBusy((FLEXSPI_Type *)handle->driverBaseAddr, memHandle, address);
    if (status != kStatus_Success)
    {
        return status;
    }

    return status;
}

status_t Nor_Flash_Page_Program(nor_handle_t *handle, uint32_t address, uint8_t *buffer)
{
    status_t status;
    flexspi_transfer_t flashXfer;
    flexspi_mem_nor_handle_t *memHandle = (flexspi_mem_nor_handle_t *)handle->deviceSpecific;
    flexspi_port_t port                 = memHandle->port;
    uint32_t pageSize                   = handle->bytesInPageSize;
    /* Write enable. */
    status = FLEXSPI_NOR_WriteEnable((FLEXSPI_Type *)handle->driverBaseAddr, port, address);
    if (status != kStatus_Success)
    {
        return status;
    }

    /* Send page load command. */
    flashXfer.deviceAddress = address;
    flashXfer.port          = port;
    flashXfer.cmdType       = kFLEXSPI_Write;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM;
    flashXfer.data          = (uint32_t *)buffer;
    flashXfer.dataSize      = pageSize;

    status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);
    if (status != kStatus_Success)
    {
        return status;
    }

    status = FLEXSPI_NOR_WaitBusBusy((FLEXSPI_Type *)handle->driverBaseAddr, memHandle, address);
    if (status != kStatus_Success)
    {
        return status;
    }

    return status;
}

status_t Nor_Flash_Read(nor_handle_t *handle, uint32_t address, uint8_t *buffer, uint32_t length)
{
    assert(handle != NULL);
    assert(buffer != NULL);

    status_t status = kStatus_Success;
    flexspi_transfer_t flashXfer;
    flexspi_mem_nor_handle_t *memHandle = (flexspi_mem_nor_handle_t *)handle->deviceSpecific;
    flexspi_port_t port                 = memHandle->port;

    /* Read page. */
    flashXfer.deviceAddress = address;
    flashXfer.port          = port;
    flashXfer.cmdType       = kFLEXSPI_Read;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_READ;
    flashXfer.data          = (uint32_t *)buffer;
    flashXfer.dataSize      = length;

    status = FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);

    if (status != kStatus_Success)
    {
        return status;
    }
    status = FLEXSPI_NOR_WaitBusBusy((FLEXSPI_Type *)handle->driverBaseAddr, memHandle, address);
    if (status != kStatus_Success)
    {
        return status;
    }

    return status;
}

status_t Nor_Flash_Init(nor_config_t *config, nor_handle_t *handle)
{
    assert(config != NULL);
    assert(handle != NULL);

    status_t status                      = kStatus_Success;
    flexspi_mem_config_t *memConfig      = (flexspi_mem_config_t *)config->memControlConfig;
    handle->driverBaseAddr               = config->driverBaseAddr;
    handle->deviceSpecific               = &flexspiMemHandle;
    flexspiMemHandle.configuredFlashSize = memConfig->deviceConfig.flashSize;
    flexspiMemHandle.port                = memConfig->devicePort;

    (void)memset(memConfig->lookupTable, 0, sizeof(memConfig->lookupTable));

    /* Configure flash settings according to serial flash feature. */
    FLEXSPI_SetFlashConfig((FLEXSPI_Type *)handle->driverBaseAddr, &(memConfig->deviceConfig), memConfig->devicePort);

    status = FLEXSPI_NOR_GenerateConfigBlockUsingSFDP(handle, memConfig);

    /* Update LUT table. */
    FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, 0, memConfig->lookupTable,
                      sizeof(memConfig->lookupTable) / sizeof(memConfig->lookupTable[0]));

    /* Do software reset. */
    FLEXSPI_SoftwareReset((FLEXSPI_Type *)handle->driverBaseAddr);

    if (status != kStatus_Success)
    {
        return status;
    }

    return kStatus_Success;
}

status_t FLEXSPI_NOR_ExitNoCMDMode(nor_handle_t *handle, flexspi_mem_config_t *config, uint32_t baseAddr)
{
    flexspi_transfer_t flashXfer;
    flashXfer.cmdType       = kFLEXSPI_Command;
    flashXfer.seqIndex      = NOR_CMD_LUT_SEQ_IDX_EXIT_NOCMD;
    flashXfer.SeqNumber     = 1;
    flashXfer.deviceAddress = baseAddr;
    flashXfer.port          = flexspiMemHandle.port;
    FLEXSPI_UpdateLUT((FLEXSPI_Type *)handle->driverBaseAddr, NOR_CMD_LUT_SEQ_IDX_EXIT_NOCMD * 4U,
                      &config->lookupTable[4U * flashXfer.seqIndex], 4U);
    return FLEXSPI_TransferBlocking((FLEXSPI_Type *)handle->driverBaseAddr, &flashXfer);
}
