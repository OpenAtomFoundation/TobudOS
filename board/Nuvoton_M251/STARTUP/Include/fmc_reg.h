/**************************************************************************//**
 * @file     fmc_reg.h
 * @version  V1.00
 * @brief    FMC register definition header file
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __FMC_REG_H__
#define __FMC_REG_H__

#if defined ( __CC_ARM   )
    #pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup FMC Flash Memory Controller (FMC)
    Memory Mapped Structure for FMC Controller
@{ */

typedef struct
{


    /**
     * @var FMC_T::ISPCTL
     * Offset: 0x00  ISP Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ISPEN     |ISP Enable Bit (Write Protect)
     * |        |          |ISP function enable bit. Set this bit to enable ISP function.
     * |        |          |0 = ISP function Disabled.
     * |        |          |1 = ISP function Enabled.
     * |        |          |Note: This bit is write-protected. Refer to the SYS_REGLCTL register.
     * |[1]     |BS        |Boot Select (Write Protect)
     * |        |          |Set/clear this bit to select next booting from LDROM/APROM, respectively
     * |        |          |This bit also functions as chip booting status flag, which can be used to check where chip booted from
     * |        |          |This bit is initiated with the inversed value of CBS[1] (CONFIG0[7]) after any reset is happened except CPU reset (CPU is 1) or system reset (SYS) is happened
     * |        |          |0 = Booting from APROM.
     * |        |          |1 = Booting from LDROM.
     * |        |          |Note: This bit is write-protected. Refer to the SYS_REGLCTL register.
     * |[3]     |APUEN     |APROM Update Enable Bit (Write Protect)
     * |        |          |0 = APROM cannot be updated when the chip runs in APROM.
     * |        |          |1 = APROM can be updated when the chip runs in APROM.
     * |        |          |Note: This bit is write-protected. Refer to the SYS_REGLCTL register.
     * |[4]     |CFGUEN    |CONFIG Update Enable Bit (Write Protect)
     * |        |          |0 = CONFIG cannot be updated.
     * |        |          |1 = CONFIG can be updated.
     * |        |          |Note: This bit is write-protected. Refer to the SYS_REGLCTL register.
     * |[5]     |LDUEN     |LDROM Update Enable Bit (Write Protect)
     * |        |          |LDROM update enable bit.
     * |        |          |0 = LDROM cannot be updated.
     * |        |          |1 = LDROM can be updated.
     * |        |          |Note: This bit is write-protected. Refer to the SYS_REGLCTL register.
     * |[6]     |ISPFF     |ISP Fail Flag (Write Protect)
     * |        |          |This bit is set by hardware when a triggered ISP meets any of the following conditions:
     * |        |          |This bit needs to be cleared by writing 1 to it.
     * |        |          |(1) APROM writes to itself if APUEN is set to 0.
     * |        |          |(2) LDROM writes to itself if LDUEN is set to 0.
     * |        |          |(3) CONFIG is erased/programmed if CFGUEN is set to 0.
     * |        |          |(4) Page Erase command at LOCK mode with ICE connection
     * |        |          |(5) Erase or Program command at brown-out detected
     * |        |          |(6) Destination address is illegal, such as over an available range.
     * |        |          |(7) Invalid ISP commands
     * |        |          |(8) ISP CMD in XOM region, except mass erase, page erase and chksum command
     * |        |          |(9) The wrong setting of page erase ISP CMD in XOM
     * |        |          |(10) Violate XOM setting one time protection
     * |        |          |Note: This bit is write-protected. Refer to the SYS_REGLCTL register.
     * @var FMC_T::ISPADDR
     * Offset: 0x04  ISP Address Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |ISPADDR   |ISP Address
     * |        |          |For Checksum Calculation command, this field is the Flash starting address for checksum calculation, 512 bytes alignment is necessary for checksum calculation.
     * @var FMC_T::ISPDAT
     * Offset: 0x08  ISP Data Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |ISPDAT    |ISP Data
     * |        |          |Write data to this register before ISP program operation.
     * |        |          |Read data from this register after ISP read operation.
     * |        |          |For Run Checksum Calculation command, ISPDAT is the memory size (byte) and 512 bytes alignment
     * |        |          |For ISP Read Checksum command, ISPDAT is the checksum result
     * |        |          |If ISPDAT = 0x0000_0000, it means that (1) the checksum calculation is in progress, (2) the memory range for checksum calculation is incorrect.
     * @var FMC_T::ISPCMD
     * Offset: 0x0C  ISP CMD Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[6:0]   |CMD       |ISP CMD
     * |        |          |ISP command table is shown below:
     * |        |          |0x00= FLASH Read.
     * |        |          |0x04= Read Unique ID.
     * |        |          |0x0B= Read Company ID.
     * |        |          |0x0C= Read Device ID.
     * |        |          |0x0D= Read Checksum.
     * |        |          |0x21= FLASH 32-bit Program.
     * |        |          |0x22= FLASH Page Erase.
     * |        |          |0x27= FLASH Multi-Word Program.
     * |        |          |0x28= Run Flash All-One Verification.
     * |        |          |0x2D= Run Checksum Calculation.
     * |        |          |0x2E= Vector Remap.
     * |        |          |The other commands are invalid.
     * @var FMC_T::ISPTRG
     * Offset: 0x10  ISP Trigger Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ISPGO     |ISP Start Trigger (Write Protect)
     * |        |          |Write 1 to start ISP operation and this bit will be cleared to 0 by hardware automatically when ISP operation is finished.
     * |        |          |0 = ISP operation is finished.
     * |        |          |1 = ISP is progressed.
     * |        |          |Note: This bit is write-protected. Refer to the SYS_REGLCTL register.
     * @var FMC_T::FTCTL
     * Offset: 0x18  Flash Access Time Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[9]     |CACHEINV  |Flash Cache Invalidation (Write Protect)
     * |        |          |Write 1 to start cache invalidation. The value will be change to 0 once the process finishes.
     * |        |          |0 = Flash Cache Invalidation.(default)
     * |        |          |1 = Flash Cache Invalidation.
     * |        |          |Note: This bit is write-protected. Refer to the SYS_REGLCTL register.
     * @var FMC_T::ISPSTS
     * Offset: 0x40  ISP Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ISPBUSY   |ISP Busy Flag (Read Only)
     * |        |          |Write 1 to start ISP operation and this bit will be cleared to 0 by hardware automatically when ISP operation is finished.
     * |        |          |This bit is the mirror of ISPGO(FMC_ISPTRG[0]).
     * |        |          |0 = ISP operation is finished.
     * |        |          |1 = ISP is progressed.
     * |[2:1]   |CBS       |Boot Selection of CONFIG (Read Only)
     * |        |          |This bit is initiated with the CBS (CONFIG0[7:6]) after any reset is happened except CPU reset (CPU is 1) or system reset (SYS) is happened.
     * |        |          |00 = LDROM with IAP mode.
     * |        |          |01 = LDROM without IAP mode.
     * |        |          |10 = APROM with IAP mode.
     * |        |          |11 = APROM without IAP mode.
     * |[5]     |PGFF      |Flash Program with Fast Verification Flag (Read Only)
     * |        |          |This bit is set if data is mismatched at ISP programming verification
     * |        |          |This bit is clear by performing ISP Flash erase or ISP read CID operation
     * |        |          |0 = Flash Program is success.
     * |        |          |1 = Flash Program is fail. Program data is different with data in the Flash memory
     * |[6]     |ISPFF     |ISP Fail Flag (Write Protect)
     * |        |          |This bit is the mirror of ISPFF (FMC_ISPCTL[6]), it needs to be cleared by writing 1 to FMC_ISPCTL[6] or FMC_ISPSTS[6]
     * |        |          |This bit is set by hardware when a triggered ISP meets any of the following conditions:
     * |        |          |(1) APROM writes to itself if APUEN is set to 0.
     * |        |          |(2) LDROM writes to itself if LDUEN is set to 0.
     * |        |          |(3) CONFIG is erased/programmed if CFGUEN is set to 0.
     * |        |          |(4) Page Erase command at LOCK mode with ICE connection
     * |        |          |(5) Erase or Program command at brown-out detected
     * |        |          |(6) Destination address is illegal, such as over an available range.
     * |        |          |(7) Invalid ISP commands
     * |        |          |(8) ISP CMD in XOM region, except mass erase, page erase and chksum command
     * |        |          |(9) The wrong setting of page erase ISP CMD in XOM
     * |        |          |(10) Violate XOM setting one time protection
     * |        |          |Note: This bit is write-protected. Refer to the SYS_REGLCTL register.
     * |[7]     |ALLONE    |Flash All-one Verification Flag
     * |        |          |This bit is set by hardware if all of Flash bits are 1, and clear if Flash bits are not all 1 after "Run Flash All-One Verification" complete; this bit also can be clear by writing 1
     * |        |          |0 = Flash bits are not all 1 after "Run Flash All-One Verification" complete.
     * |        |          |1 = All of Flash bits are 1 after "Run Flash All-One Verification" complete.
     * |[29:9]  |VECMAP    |Vector Page Mapping Address (Read Only)
     * |        |          |All access to 0x0000_0000~0x0000_01FF is remapped to the Flash memory or SRAM address {VECMAP[20:0], 9'h000} ~ {VECMAP[20:0], 9'h1FF}.
     * |        |          |VECMAP [20:19] = 00 system vector address is mapped to Flash memory.
     * |        |          |VECMAP [20:19] = 10 system vector address is mapped to SRAM memory.
     * |        |          |VECMAP [18:12] should be 0.
     * @var FMC_T::CYCCTL
     * Offset: 0x4C  Flash Access Cycle Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |CYCLE     |Flash Access Cycle Control (Write Protect)
     * |        |          |This register is updated automaticly by hardware while FCYCDIS (FMC_ISPSTS[4]) is 0, and updated by software while auto-tuning function disabled ( FADIS (FMC_CYCTL[8]) is 1)
     * |        |          |0001 = CPU access with zero wait cycle ; Flash access cycle is 1;.
     * |        |          |The HCLK working frequency range is <19MHz; Cache is disabled by hardware.
     * |        |          |0010 = CPU access with one wait cycles if cache miss; Flash access cycle is 2;.
     * |        |          | The optimized HCLK working frequency range is 18~33 MHz
     * |        |          |0011 = CPU access with two wait cycles if cahce miss; Flash access cycle is 3;.
     * |        |          | The optimized HCLK working frequency range is 33~50 MHz
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * @var FMC_T::MPDAT0
     * Offset: 0x80  ISP Data0 Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |ISPDAT0   |ISP Data 0
     * |        |          |This register is the first 32-bit data for 32-bit/64-bit/multi-word programming, and it is also the mirror of FMC_ISPDAT, both registers keep the same data
     * @var FMC_T::MPDAT1
     * Offset: 0x84  ISP Data1 Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |ISPDAT1   |ISP Data 1
     * |        |          |This register is the second 32-bit data for 64-bit/multi-word programming.
     * @var FMC_T::MPDAT2
     * Offset: 0x88  ISP Data2 Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |ISPDAT2   |ISP Data 2
     * |        |          |This register is the third 32-bit data for multi-word programming.
     * @var FMC_T::MPDAT3
     * Offset: 0x8C  ISP Data3 Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |ISPDAT3   |ISP Data 3
     * |        |          |This register is the fourth 32-bit data for multi-word programming.
     * @var FMC_T::MPSTS
     * Offset: 0xC0  ISP Multi-program Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MPBUSY    |ISP Multi-word Program Busy Flag (Read Only)
     * |        |          |Write 1 to start ISP Multi-Word program operation and this bit will be cleared to 0 by hardware automatically when ISP Multi-Word program operation is finished.
     * |        |          |This bit is the mirror of ISPGO(FMC_ISPTRG[0]).
     * |        |          |0 = ISP Multi-Word program operation is finished.
     * |        |          |1 = ISP Multi-Word program operation is progressed.
     * |[1]     |PPGO      |ISP Multi-program Status (Read Only)
     * |        |          |0 = ISP multi-word program operation is not active.
     * |        |          |1 = ISP multi-word program operation is in progress.
     * |[2]     |ISPFF     |ISP Fail Flag (Read Only)
     * |        |          |This bit is the mirror of ISPFF (FMC_ISPCTL[6]), it needs to be cleared by writing 1 to FMC_ISPCTL[6] or FMC_ISPSTS[6]
     * |        |          |This bit is set by hardware when a triggered ISP meets any of the following conditions:
     * |        |          |(1) APROM writes to itself if APUEN is set to 0.
     * |        |          |(2) LDROM writes to itself if LDUEN is set to 0.
     * |        |          |(3) CONFIG is erased/programmed if CFGUEN is set to 0.
     * |        |          |(4) Page Erase command at LOCK mode with ICE connection
     * |        |          |(5) Erase or Program command at brown-out detected
     * |        |          |(6) Destination address is illegal, such as over an available range.
     * |        |          |(7) Invalid ISP commands
     * |[4]     |D0        |ISP DATA 0 Flag (Read Only)
     * |        |          |This bit is set when FMC_MPDAT0 is written and auto-clear to 0 when the FMC_MPDAT0 data is programmed to Flash complete.
     * |        |          |0 = FMC_MPDAT0 register is empty, or program to Flash complete.
     * |        |          |1 = FMC_MPDAT0 register has been written, and not program to Flash complete.
     * |[5]     |D1        |ISP DATA 1 Flag (Read Only)
     * |        |          |This bit is set when FMC_MPDAT1 is written and auto-clear to 0 when the FMC_MPDAT1 data is programmed to Flash complete.
     * |        |          |0 = FMC_MPDAT1 register is empty, or program to Flash complete.
     * |        |          |1 = FMC_MPDAT1 register has been written, and not program to Flash complete.
     * |[6]     |D2        |ISP DATA 2 Flag (Read Only)
     * |        |          |This bit is set when FMC_MPDAT2 is written and auto-clear to 0 when the FMC_MPDAT2 data is programmed to Flash complete.
     * |        |          |0 = FMC_MPDAT2 register is empty, or program to Flash complete.
     * |        |          |1 = FMC_MPDAT2 register has been written, and not program to Flash complete.
     * |[7]     |D3        |ISP DATA 3 Flag (Read Only)
     * |        |          |This bit is set when FMC_MPDAT3 is written and auto-clear to 0 when the FMC_MPDAT3 data is programmed to Flash complete.
     * |        |          |0 = FMC_MPDAT3 register is empty, or program to Flash complete.
     * |        |          |1 = FMC_MPDAT3 register has been written, and not program to Flash complete.
     * @var FMC_T::MPADDR
     * Offset: 0xC4  ISP Multi-program Address Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |MPADDR    |ISP Multi-word Program Address
     * |        |          |MPADDR is the address of ISP multi-word program operation when ISPGO flag is 1.
     * |        |          |MPADDR will keep the final ISP address when ISP multi-word program is complete.
     * @var FMC_T::XOMR0STS0
     * Offset: 0xD0  XOM Region 0 Status Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[23:0]  |BASE      |XOM Region 0 Base Address (Page-aligned)
     * |        |          |BASE is the base address of XOM Region 0.
     * @var FMC_T::XOMR0STS1
     * Offset: 0xD4  XOM Region 0 Status Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[8:0]   |SIZE      |XOM Region 0 Size (Page-aligned)
     * |        |          |SIZE is the page number of XOM Region 0.
     * @var FMC_T::XOMSTS
     * Offset: 0xE0  XOM Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |XOMR0ON   |XOM Region 0 On
     * |        |          |XOM Region 0 active status.
     * |        |          |0 = No active.
     * |        |          |1 = XOM region 0 is active.
     * |[4]     |XOMPEF    |XOM Page Erase Function Fail
     * |        |          |XOM page erase function status. If XOMPEF is set to 1, user needs to erase XOM region again.
     * |        |          |0 = Sucess.
     * |        |          |1 = Fail.
     */
    __IO uint32_t ISPCTL;                /*!< [0x0000] ISP Control Register                                             */
    __IO uint32_t ISPADDR;               /*!< [0x0004] ISP Address Register                                             */
    __IO uint32_t ISPDAT;                /*!< [0x0008] ISP Data Register                                                */
    __IO uint32_t ISPCMD;                /*!< [0x000c] ISP CMD Register                                                 */
    __IO uint32_t ISPTRG;                /*!< [0x0010] ISP Trigger Control Register                                     */
    __I  uint32_t RESERVE0;
    __IO uint32_t FTCTL;                 /*!< [0x0018] Flash Access Time Control Register                               */
    __I  uint32_t RESERVE1[9];
    __IO uint32_t ISPSTS;                /*!< [0x0040] ISP Status Register                                              */
    __I  uint32_t RESERVE2[2];
    __IO uint32_t CYCCTL;                /*!< [0x004c] Flash Access Cycle Control Register                              */
    __I  uint32_t RESERVE3[12];
    __IO uint32_t MPDAT0;                /*!< [0x0080] ISP Data0 Register                                               */
    __IO uint32_t MPDAT1;                /*!< [0x0084] ISP Data1 Register                                               */
    __IO uint32_t MPDAT2;                /*!< [0x0088] ISP Data2 Register                                               */
    __IO uint32_t MPDAT3;                /*!< [0x008c] ISP Data3 Register                                               */
    __I  uint32_t RESERVE4[12];
    __I  uint32_t MPSTS;                 /*!< [0x00c0] ISP Multi-program Status Register                                */
    __I  uint32_t MPADDR;                /*!< [0x00c4] ISP Multi-program Address Register                               */
    __I  uint32_t RESERVE5[2];
    __I  uint32_t XOMR0STS0;             /*!< [0x00d0] XOM Region 0 Status Register 0                                   */
    __I  uint32_t XOMR0STS1;             /*!< [0x00d4] XOM Region 0 Status Register 1                                   */
    __I  uint32_t RESERVE6[2];
    __I  uint32_t XOMSTS;                /*!< [0x00e0] XOM Status Register                                              */

} FMC_T;

/**
    @addtogroup FMC_CONST FMC Bit Field Definition
    Constant Definitions for FMC Controller
@{ */

#define FMC_ISPCTL_ISPEN_Pos             (0)                                               /*!< FMC_T::ISPCTL: ISPEN Position          */
#define FMC_ISPCTL_ISPEN_Msk             (0x1ul << FMC_ISPCTL_ISPEN_Pos)                   /*!< FMC_T::ISPCTL: ISPEN Mask              */

#define FMC_ISPCTL_BS_Pos                (1)                                               /*!< FMC_T::ISPCTL: BS Position             */
#define FMC_ISPCTL_BS_Msk                (0x1ul << FMC_ISPCTL_BS_Pos)                      /*!< FMC_T::ISPCTL: BS Mask                 */

#define FMC_ISPCTL_APUEN_Pos             (3)                                               /*!< FMC_T::ISPCTL: APUEN Position          */
#define FMC_ISPCTL_APUEN_Msk             (0x1ul << FMC_ISPCTL_APUEN_Pos)                   /*!< FMC_T::ISPCTL: APUEN Mask              */

#define FMC_ISPCTL_CFGUEN_Pos            (4)                                               /*!< FMC_T::ISPCTL: CFGUEN Position         */
#define FMC_ISPCTL_CFGUEN_Msk            (0x1ul << FMC_ISPCTL_CFGUEN_Pos)                  /*!< FMC_T::ISPCTL: CFGUEN Mask             */

#define FMC_ISPCTL_LDUEN_Pos             (5)                                               /*!< FMC_T::ISPCTL: LDUEN Position          */
#define FMC_ISPCTL_LDUEN_Msk             (0x1ul << FMC_ISPCTL_LDUEN_Pos)                   /*!< FMC_T::ISPCTL: LDUEN Mask              */

#define FMC_ISPCTL_ISPFF_Pos             (6)                                               /*!< FMC_T::ISPCTL: ISPFF Position          */
#define FMC_ISPCTL_ISPFF_Msk             (0x1ul << FMC_ISPCTL_ISPFF_Pos)                   /*!< FMC_T::ISPCTL: ISPFF Mask              */

#define FMC_ISPADDR_ISPADDR_Pos          (0)                                               /*!< FMC_T::ISPADDR: ISPADDR Position       */
#define FMC_ISPADDR_ISPADDR_Msk          (0xfffffffful << FMC_ISPADDR_ISPADDR_Pos)         /*!< FMC_T::ISPADDR: ISPADDR Mask           */

#define FMC_ISPDAT_ISPDAT_Pos            (0)                                               /*!< FMC_T::ISPDAT: ISPDAT Position         */
#define FMC_ISPDAT_ISPDAT_Msk            (0xfffffffful << FMC_ISPDAT_ISPDAT_Pos)           /*!< FMC_T::ISPDAT: ISPDAT Mask             */

#define FMC_ISPCMD_CMD_Pos               (0)                                               /*!< FMC_T::ISPCMD: CMD Position            */
#define FMC_ISPCMD_CMD_Msk               (0x7ful << FMC_ISPCMD_CMD_Pos)                    /*!< FMC_T::ISPCMD: CMD Mask                */

#define FMC_ISPTRG_ISPGO_Pos             (0)                                               /*!< FMC_T::ISPTRG: ISPGO Position          */
#define FMC_ISPTRG_ISPGO_Msk             (0x1ul << FMC_ISPTRG_ISPGO_Pos)                   /*!< FMC_T::ISPTRG: ISPGO Mask              */

#define FMC_FTCTL_CACHEINV_Pos           (9)                                               /*!< FMC_T::FTCTL: CACHEINV Position        */
#define FMC_FTCTL_CACHEINV_Msk           (0x1ul << FMC_FTCTL_CACHEINV_Pos)                 /*!< FMC_T::FTCTL: CACHEINV Mask            */

#define FMC_ISPSTS_ISPBUSY_Pos           (0)                                               /*!< FMC_T::ISPSTS: ISPBUSY Position        */
#define FMC_ISPSTS_ISPBUSY_Msk           (0x1ul << FMC_ISPSTS_ISPBUSY_Pos)                 /*!< FMC_T::ISPSTS: ISPBUSY Mask            */

#define FMC_ISPSTS_CBS_Pos               (1)                                               /*!< FMC_T::ISPSTS: CBS Position            */
#define FMC_ISPSTS_CBS_Msk               (0x3ul << FMC_ISPSTS_CBS_Pos)                     /*!< FMC_T::ISPSTS: CBS Mask                */

#define FMC_ISPSTS_PGFF_Pos              (5)                                               /*!< FMC_T::ISPSTS: PGFF Position           */
#define FMC_ISPSTS_PGFF_Msk              (0x1ul << FMC_ISPSTS_PGFF_Pos)                    /*!< FMC_T::ISPSTS: PGFF Mask               */

#define FMC_ISPSTS_ISPFF_Pos             (6)                                               /*!< FMC_T::ISPSTS: ISPFF Position          */
#define FMC_ISPSTS_ISPFF_Msk             (0x1ul << FMC_ISPSTS_ISPFF_Pos)                   /*!< FMC_T::ISPSTS: ISPFF Mask              */

#define FMC_ISPSTS_ALLONE_Pos            (7)                                               /*!< FMC_T::ISPSTS: ALLONE Position         */
#define FMC_ISPSTS_ALLONE_Msk            (0x1ul << FMC_ISPSTS_ALLONE_Pos)                  /*!< FMC_T::ISPSTS: ALLONE Mask             */

#define FMC_ISPSTS_VECMAP_Pos            (9)                                               /*!< FMC_T::ISPSTS: VECMAP Position         */
#define FMC_ISPSTS_VECMAP_Msk            (0x1ffffful << FMC_ISPSTS_VECMAP_Pos)             /*!< FMC_T::ISPSTS: VECMAP Mask             */

#define FMC_CYCCTL_CYCLE_Pos             (0)                                               /*!< FMC_T::CYCCTL: CYCLE Position          */
#define FMC_CYCCTL_CYCLE_Msk             (0xful << FMC_CYCCTL_CYCLE_Pos)                   /*!< FMC_T::CYCCTL: CYCLE Mask              */

#define FMC_MPDAT0_ISPDAT0_Pos           (0)                                               /*!< FMC_T::MPDAT0: ISPDAT0 Position        */
#define FMC_MPDAT0_ISPDAT0_Msk           (0xfffffffful << FMC_MPDAT0_ISPDAT0_Pos)          /*!< FMC_T::MPDAT0: ISPDAT0 Mask            */

#define FMC_MPDAT1_ISPDAT1_Pos           (0)                                               /*!< FMC_T::MPDAT1: ISPDAT1 Position        */
#define FMC_MPDAT1_ISPDAT1_Msk           (0xfffffffful << FMC_MPDAT1_ISPDAT1_Pos)          /*!< FMC_T::MPDAT1: ISPDAT1 Mask            */

#define FMC_MPDAT2_ISPDAT2_Pos           (0)                                               /*!< FMC_T::MPDAT2: ISPDAT2 Position        */
#define FMC_MPDAT2_ISPDAT2_Msk           (0xfffffffful << FMC_MPDAT2_ISPDAT2_Pos)          /*!< FMC_T::MPDAT2: ISPDAT2 Mask            */

#define FMC_MPDAT3_ISPDAT3_Pos           (0)                                               /*!< FMC_T::MPDAT3: ISPDAT3 Position        */
#define FMC_MPDAT3_ISPDAT3_Msk           (0xfffffffful << FMC_MPDAT3_ISPDAT3_Pos)          /*!< FMC_T::MPDAT3: ISPDAT3 Mask            */

#define FMC_MPSTS_MPBUSY_Pos             (0)                                               /*!< FMC_T::MPSTS: MPBUSY Position          */
#define FMC_MPSTS_MPBUSY_Msk             (0x1ul << FMC_MPSTS_MPBUSY_Pos)                   /*!< FMC_T::MPSTS: MPBUSY Mask              */

#define FMC_MPSTS_PPGO_Pos               (1)                                               /*!< FMC_T::MPSTS: PPGO Position            */
#define FMC_MPSTS_PPGO_Msk               (0x1ul << FMC_MPSTS_PPGO_Pos)                     /*!< FMC_T::MPSTS: PPGO Mask                */

#define FMC_MPSTS_ISPFF_Pos              (2)                                               /*!< FMC_T::MPSTS: ISPFF Position           */
#define FMC_MPSTS_ISPFF_Msk              (0x1ul << FMC_MPSTS_ISPFF_Pos)                    /*!< FMC_T::MPSTS: ISPFF Mask               */

#define FMC_MPSTS_D0_Pos                 (4)                                               /*!< FMC_T::MPSTS: D0 Position              */
#define FMC_MPSTS_D0_Msk                 (0x1ul << FMC_MPSTS_D0_Pos)                       /*!< FMC_T::MPSTS: D0 Mask                  */

#define FMC_MPSTS_D1_Pos                 (5)                                               /*!< FMC_T::MPSTS: D1 Position              */
#define FMC_MPSTS_D1_Msk                 (0x1ul << FMC_MPSTS_D1_Pos)                       /*!< FMC_T::MPSTS: D1 Mask                  */

#define FMC_MPSTS_D2_Pos                 (6)                                               /*!< FMC_T::MPSTS: D2 Position              */
#define FMC_MPSTS_D2_Msk                 (0x1ul << FMC_MPSTS_D2_Pos)                       /*!< FMC_T::MPSTS: D2 Mask                  */

#define FMC_MPSTS_D3_Pos                 (7)                                               /*!< FMC_T::MPSTS: D3 Position              */
#define FMC_MPSTS_D3_Msk                 (0x1ul << FMC_MPSTS_D3_Pos)                       /*!< FMC_T::MPSTS: D3 Mask                  */

#define FMC_MPADDR_MPADDR_Pos            (0)                                               /*!< FMC_T::MPADDR: MPADDR Position         */
#define FMC_MPADDR_MPADDR_Msk            (0xfffffffful << FMC_MPADDR_MPADDR_Pos)           /*!< FMC_T::MPADDR: MPADDR Mask             */

#define FMC_XOMR0STS0_BASE_Pos           (0)                                               /*!< FMC_T::XOMR0STS0: BASE Position        */
#define FMC_XOMR0STS0_BASE_Msk           (0xfffffful << FMC_XOMR0STS0_BASE_Pos)            /*!< FMC_T::XOMR0STS0: BASE Mask            */

#define FMC_XOMR0STS1_SIZE_Pos           (0)                                               /*!< FMC_T::XOMR0STS1: SIZE Position        */
#define FMC_XOMR0STS1_SIZE_Msk           (0x1fful << FMC_XOMR0STS1_SIZE_Pos)               /*!< FMC_T::XOMR0STS1: SIZE Mask            */

#define FMC_XOMSTS_XOMR0ON_Pos           (0)                                               /*!< FMC_T::XOMSTS: XOMR0ON Position        */
#define FMC_XOMSTS_XOMR0ON_Msk           (0x1ul << FMC_XOMSTS_XOMR0ON_Pos)                 /*!< FMC_T::XOMSTS: XOMR0ON Mask            */

#define FMC_XOMSTS_XOMPEF_Pos            (4)                                               /*!< FMC_T::XOMSTS: XOMPEF Position         */
#define FMC_XOMSTS_XOMPEF_Msk            (0x1ul << FMC_XOMSTS_XOMPEF_Pos)                  /*!< FMC_T::XOMSTS: XOMPEF Mask             */

/**@}*/ /* FMC_CONST */
/**@}*/ /* end of FMC register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
    #pragma no_anon_unions
#endif

#endif /* __FMC_REG_H__ */