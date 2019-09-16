/** ----------------------------------------------------------------------------
 *         Nationz Technology Software Support  -  NATIONZ  -
 * -----------------------------------------------------------------------------
 * Copyright (c) 2013гн2018, Nationz Corporation  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaiimer below.
 * 
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the disclaimer below in the documentation and/or
 * other materials provided with the distribution. 
 * 
 * Nationz's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission. 
 * 
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONZ "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
 * -----------------------------------------------------------------------------
 */

/** ****************************************************************************
 * @copyright      Nationz Co.,Ltd 
 *        Copyright (c) 2013гн2018 All Rights Reserved 
 *******************************************************************************
 * @file     am_reg_mcuctrl.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Register macros for the MCUCTRL module
 ******************************************************************************/
#ifndef AM_REG_MCUCTRL_H
#define AM_REG_MCUCTRL_H

//*****************************************************************************
//
// Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_MCUCTRL_NUM_MODULES                   1
#define AM_REG_MCUCTRLn(n) \
    (REG_MCUCTRL_BASEADDR + 0x00000000 * n)

//*****************************************************************************
//
// Register offsets.
//
//*****************************************************************************
#define AM_REG_MCUCTRL_CHIP_INFO_O                   0x00000000
#define AM_REG_MCUCTRL_CHIPID0_O                     0x00000004
#define AM_REG_MCUCTRL_CHIPID1_O                     0x00000008
#define AM_REG_MCUCTRL_CHIPREV_O                     0x0000000C
#define AM_REG_MCUCTRL_SUPPLYSRC_O                   0x00000010
#define AM_REG_MCUCTRL_SUPPLYSTATUS_O                0x00000014
#define AM_REG_MCUCTRL_BANDGAPEN_O                   0x000000FC
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_O              0x00000140
#define AM_REG_MCUCTRL_SRAMPWRDIS_O                  0x00000144
#define AM_REG_MCUCTRL_FLASHPWRDIS_O                 0x00000148
#define AM_REG_MCUCTRL_ICODEFAULTADDR_O              0x000001C0
#define AM_REG_MCUCTRL_DCODEFAULTADDR_O              0x000001C4
#define AM_REG_MCUCTRL_SYSFAULTADDR_O                0x000001C8
#define AM_REG_MCUCTRL_FAULTSTATUS_O                 0x000001CC
#define AM_REG_MCUCTRL_FAULTCAPTUREEN_O              0x000001D0
#define AM_REG_MCUCTRL_TPIUCTRL_O                    0x00000250

//*****************************************************************************
//
// MCUCTRL_CHIP_INFO - Chip Information Register
//
//*****************************************************************************
// Device class.
#define AM_REG_MCUCTRL_CHIP_INFO_CLASS_S             24
#define AM_REG_MCUCTRL_CHIP_INFO_CLASS_M             0xFF000000
#define AM_REG_MCUCTRL_CHIP_INFO_CLASS(n)            (((uint32_t)(n) << 24) & 0xFF000000)
#define AM_REG_MCUCTRL_CHIP_INFO_CLASS_Z32HLP100         0x01000000

// Device flash size.
#define AM_REG_MCUCTRL_CHIP_INFO_FLASH_S             20
#define AM_REG_MCUCTRL_CHIP_INFO_FLASH_M             0x00F00000
#define AM_REG_MCUCTRL_CHIP_INFO_FLASH(n)            (((uint32_t)(n) << 20) & 0x00F00000)
#define AM_REG_MCUCTRL_CHIP_INFO_FLASH_256K          0x00300000
#define AM_REG_MCUCTRL_CHIP_INFO_FLASH_512K          0x00400000

// Device RAM size.
#define AM_REG_MCUCTRL_CHIP_INFO_RAM_S               16
#define AM_REG_MCUCTRL_CHIP_INFO_RAM_M               0x000F0000
#define AM_REG_MCUCTRL_CHIP_INFO_RAM(n)              (((uint32_t)(n) << 16) & 0x000F0000)
#define AM_REG_MCUCTRL_CHIP_INFO_RAM_32K             0x00000000
#define AM_REG_MCUCTRL_CHIP_INFO_RAM_64K             0x00010000

// Major device revision number.
#define AM_REG_MCUCTRL_CHIP_INFO_MAJORREV_S          12
#define AM_REG_MCUCTRL_CHIP_INFO_MAJORREV_M          0x0000F000
#define AM_REG_MCUCTRL_CHIP_INFO_MAJORREV(n)         (((uint32_t)(n) << 12) & 0x0000F000)

// Minor device revision number.
#define AM_REG_MCUCTRL_CHIP_INFO_MINORREV_S          8
#define AM_REG_MCUCTRL_CHIP_INFO_MINORREV_M          0x00000F00
#define AM_REG_MCUCTRL_CHIP_INFO_MINORREV(n)         (((uint32_t)(n) << 8) & 0x00000F00)

// Device package type.
#define AM_REG_MCUCTRL_CHIP_INFO_PKG_S               6
#define AM_REG_MCUCTRL_CHIP_INFO_PKG_M               0x000000C0
#define AM_REG_MCUCTRL_CHIP_INFO_PKG(n)              (((uint32_t)(n) << 6) & 0x000000C0)
#define AM_REG_MCUCTRL_CHIP_INFO_PKG_BGA             0x00000080
#define AM_REG_MCUCTRL_CHIP_INFO_PKG_CSP             0x000000C0

// Number of pins.
#define AM_REG_MCUCTRL_CHIP_INFO_PINS_S              3
#define AM_REG_MCUCTRL_CHIP_INFO_PINS_M              0x00000038
#define AM_REG_MCUCTRL_CHIP_INFO_PINS(n)             (((uint32_t)(n) << 3) & 0x00000038)
#define AM_REG_MCUCTRL_CHIP_INFO_PINS_41PINS         0x00000008
#define AM_REG_MCUCTRL_CHIP_INFO_PINS_64PINS         0x00000008

// Device temperature range.
#define AM_REG_MCUCTRL_CHIP_INFO_TEMP_S              1
#define AM_REG_MCUCTRL_CHIP_INFO_TEMP_M              0x00000006
#define AM_REG_MCUCTRL_CHIP_INFO_TEMP(n)             (((uint32_t)(n) << 1) & 0x00000006)
#define AM_REG_MCUCTRL_CHIP_INFO_TEMP_COMMERCIAL     0x00000000

// Device qualified.
#define AM_REG_MCUCTRL_CHIP_INFO_QUAL_S              0
#define AM_REG_MCUCTRL_CHIP_INFO_QUAL_M              0x00000001
#define AM_REG_MCUCTRL_CHIP_INFO_QUAL(n)             (((uint32_t)(n) << 0) & 0x00000001)
#define AM_REG_MCUCTRL_CHIP_INFO_QUAL_PROTOTYPE      0x00000000
#define AM_REG_MCUCTRL_CHIP_INFO_QUAL_QUALIFIED      0x00000001

//*****************************************************************************
//
// MCUCTRL_CHIPID0 - Unique Chip ID 0
//
//*****************************************************************************
// Unique chip ID 0.
#define AM_REG_MCUCTRL_CHIPID0_VALUE_S               0
#define AM_REG_MCUCTRL_CHIPID0_VALUE_M               0xFFFFFFFF
#define AM_REG_MCUCTRL_CHIPID0_VALUE(n)              (((uint32_t)(n) << 0) & 0xFFFFFFFF)
#define AM_REG_MCUCTRL_CHIPID0_VALUE_Z32HLP100           0x00000000

//*****************************************************************************
//
// MCUCTRL_CHIPID1 - Unique Chip ID 1
//
//*****************************************************************************
// Unique chip ID 1.
#define AM_REG_MCUCTRL_CHIPID1_VALUE_S               0
#define AM_REG_MCUCTRL_CHIPID1_VALUE_M               0xFFFFFFFF
#define AM_REG_MCUCTRL_CHIPID1_VALUE(n)              (((uint32_t)(n) << 0) & 0xFFFFFFFF)
#define AM_REG_MCUCTRL_CHIPID1_VALUE_Z32HLP100           0x00000000

//*****************************************************************************
//
// MCUCTRL_CHIPREV - Chip Revision
//
//*****************************************************************************
// Chip Revision Number.
#define AM_REG_MCUCTRL_CHIPREV_REVISION_S            0
#define AM_REG_MCUCTRL_CHIPREV_REVISION_M            0x000000FF
#define AM_REG_MCUCTRL_CHIPREV_REVISION(n)           (((uint32_t)(n) << 0) & 0x000000FF)
#define AM_REG_MCUCTRL_CHIPREV_REVISION_Z32HLP100        0x00000000

//*****************************************************************************
//
// MCUCTRL_SUPPLYSRC - Memory and Core Voltage Supply Source Select Register
//
//*****************************************************************************
// Enables and Selects the Core Buck as the supply for the low-voltage power
// domain.
#define AM_REG_MCUCTRL_SUPPLYSRC_COREBUCKEN_S        1
#define AM_REG_MCUCTRL_SUPPLYSRC_COREBUCKEN_M        0x00000002
#define AM_REG_MCUCTRL_SUPPLYSRC_COREBUCKEN(n)       (((uint32_t)(n) << 1) & 0x00000002)
#define AM_REG_MCUCTRL_SUPPLYSRC_COREBUCKEN_EN       0x00000002

// Enables and select the Memory Buck as the supply for the Flash and SRAM power
// domain.
#define AM_REG_MCUCTRL_SUPPLYSRC_MEMBUCKEN_S         0
#define AM_REG_MCUCTRL_SUPPLYSRC_MEMBUCKEN_M         0x00000001
#define AM_REG_MCUCTRL_SUPPLYSRC_MEMBUCKEN(n)        (((uint32_t)(n) << 0) & 0x00000001)
#define AM_REG_MCUCTRL_SUPPLYSRC_MEMBUCKEN_EN        0x00000001

//*****************************************************************************
//
// MCUCTRL_SUPPLYSTATUS - Memory and Core Voltage Supply Source Status Register
//
//*****************************************************************************
// Indicates whether the Core low-voltage domain is supplied from the LDO or the
// Buck.
#define AM_REG_MCUCTRL_SUPPLYSTATUS_COREBUCKON_S     1
#define AM_REG_MCUCTRL_SUPPLYSTATUS_COREBUCKON_M     0x00000002
#define AM_REG_MCUCTRL_SUPPLYSTATUS_COREBUCKON(n)    (((uint32_t)(n) << 1) & 0x00000002)
#define AM_REG_MCUCTRL_SUPPLYSTATUS_COREBUCKON_LDO   0x00000000
#define AM_REG_MCUCTRL_SUPPLYSTATUS_COREBUCKON_BUCK  0x00000002

// Indicate whether the Memory power domain is supplied from the LDO or the
// Buck.
#define AM_REG_MCUCTRL_SUPPLYSTATUS_MEMBUCKON_S      0
#define AM_REG_MCUCTRL_SUPPLYSTATUS_MEMBUCKON_M      0x00000001
#define AM_REG_MCUCTRL_SUPPLYSTATUS_MEMBUCKON(n)     (((uint32_t)(n) << 0) & 0x00000001)
#define AM_REG_MCUCTRL_SUPPLYSTATUS_MEMBUCKON_LDO    0x00000000
#define AM_REG_MCUCTRL_SUPPLYSTATUS_MEMBUCKON_BUCK   0x00000001

//*****************************************************************************
//
// MCUCTRL_BANDGAPEN - Band Gap Enable
//
//*****************************************************************************
// Bandgap Enable
#define AM_REG_MCUCTRL_BANDGAPEN_BGPEN_S             0
#define AM_REG_MCUCTRL_BANDGAPEN_BGPEN_M             0x00000001
#define AM_REG_MCUCTRL_BANDGAPEN_BGPEN(n)            (((uint32_t)(n) << 0) & 0x00000001)
#define AM_REG_MCUCTRL_BANDGAPEN_BGPEN_DIS           0x00000000
#define AM_REG_MCUCTRL_BANDGAPEN_BGPEN_EN            0x00000001

//*****************************************************************************
//
// MCUCTRL_SRAMPWDINSLEEP - Powerdown an SRAM Bank in Deep Sleep mode
//
//*****************************************************************************
// Force SRAM Bank 7 to powerdown in deep sleep mode, causing the contents of
// the bank to be lost.
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK7_S        7
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK7_M        0x00000080
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK7(n)       (((uint32_t)(n) << 7) & 0x00000080)
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK7_NORMAL   0x00000000
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK7_PWRDN_IN_DEEPSLEEP 0x00000080

// Force SRAM Bank 6 to powerdown in deep sleep mode, causing the contents of
// the bank to be lost.
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK6_S        6
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK6_M        0x00000040
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK6(n)       (((uint32_t)(n) << 6) & 0x00000040)
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK6_NORMAL   0x00000000
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK6_PWRDN_IN_DEEPSLEEP 0x00000040

// Force SRAM Bank 5 to powerdown in deep sleep mode, causing the contents of
// the bank to be lost.
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK5_S        5
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK5_M        0x00000020
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK5(n)       (((uint32_t)(n) << 5) & 0x00000020)
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK5_NORMAL   0x00000000
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK5_PWRDN_IN_DEEPSLEEP 0x00000020

// Force SRAM Bank 4 to powerdown in deep sleep mode, causing the contents of
// the bank to be lost.
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK4_S        4
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK4_M        0x00000010
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK4(n)       (((uint32_t)(n) << 4) & 0x00000010)
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK4_NORMAL   0x00000000
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK4_PWRDN_IN_DEEPSLEEP 0x00000010

// Force SRAM Bank 3 to powerdown in deep sleep mode, causing the contents of
// the bank to be lost.
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK3_S        3
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK3_M        0x00000008
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK3(n)       (((uint32_t)(n) << 3) & 0x00000008)
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK3_NORMAL   0x00000000
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK3_PWRDN_IN_DEEPSLEEP 0x00000008

// Force SRAM Bank 2 to powerdown in deep sleep mode, causing the contents of
// the bank to be lost.
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK2_S        2
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK2_M        0x00000004
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK2(n)       (((uint32_t)(n) << 2) & 0x00000004)
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK2_NORMAL   0x00000000
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK2_PWRDN_IN_DEEPSLEEP 0x00000004

// Force SRAM Bank 1 to powerdown in deep sleep mode, causing the contents of
// the bank to be lost.
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK1_S        1
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK1_M        0x00000002
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK1(n)       (((uint32_t)(n) << 1) & 0x00000002)
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK1_NORMAL   0x00000000
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK1_PWRDN_IN_DEEPSLEEP 0x00000002

// Force SRAM Bank 0 to powerdown in deep sleep mode, causing the contents of
// the bank to be lost.
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK0_S        0
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK0_M        0x00000001
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK0(n)       (((uint32_t)(n) << 0) & 0x00000001)
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK0_NORMAL   0x00000000
#define AM_REG_MCUCTRL_SRAMPWDINSLEEP_BANK0_PWRDN_IN_DEEPSLEEP 0x00000001

//*****************************************************************************
//
// MCUCTRL_SRAMPWRDIS - Disables individual banks of the SRAM array
//
//*****************************************************************************
// Remove power from SRAM Bank 7 which will cause an access to its address space
// to generate a Hard Fault.
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK7_S            7
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK7_M            0x00000080
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK7(n)           (((uint32_t)(n) << 7) & 0x00000080)
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK7_DIS          0x00000080

// Remove power from SRAM Bank 6 which will cause an access to its address space
// to generate a Hard Fault.
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK6_S            6
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK6_M            0x00000040
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK6(n)           (((uint32_t)(n) << 6) & 0x00000040)
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK6_DIS          0x00000040

// Remove power from SRAM Bank 5 which will cause an access to its address space
// to generate a Hard Fault.
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK5_S            5
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK5_M            0x00000020
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK5(n)           (((uint32_t)(n) << 5) & 0x00000020)
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK5_DIS          0x00000020

// Remove power from SRAM Bank 4 which will cause an access to its address space
// to generate a Hard Fault.
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK4_S            4
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK4_M            0x00000010
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK4(n)           (((uint32_t)(n) << 4) & 0x00000010)
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK4_DIS          0x00000010

// Remove power from SRAM Bank 3 which will cause an access to its address space
// to generate a Hard Fault.
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK3_S            3
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK3_M            0x00000008
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK3(n)           (((uint32_t)(n) << 3) & 0x00000008)
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK3_DIS          0x00000008

// Remove power from SRAM Bank 2 which will cause an access to its address space
// to generate a Hard Fault.
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK2_S            2
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK2_M            0x00000004
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK2(n)           (((uint32_t)(n) << 2) & 0x00000004)
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK2_DIS          0x00000004

// Remove power from SRAM Bank 1 which will cause an access to its address space
// to generate a Hard Fault.
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK1_S            1
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK1_M            0x00000002
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK1(n)           (((uint32_t)(n) << 1) & 0x00000002)
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK1_DIS          0x00000002

// Remove power from SRAM Bank 0 which will cause an access to its address space
// to generate a Hard Fault.
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK0_S            0
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK0_M            0x00000001
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK0(n)           (((uint32_t)(n) << 0) & 0x00000001)
#define AM_REG_MCUCTRL_SRAMPWRDIS_BANK0_DIS          0x00000001

//*****************************************************************************
//
// MCUCTRL_FLASHPWRDIS - Disables individual banks of the Flash array
//
//*****************************************************************************
// Remove power from Flash Bank 1 which will cause an access to its address
// space to generate a Hard Fault.
#define AM_REG_MCUCTRL_FLASHPWRDIS_BANK1_S           1
#define AM_REG_MCUCTRL_FLASHPWRDIS_BANK1_M           0x00000002
#define AM_REG_MCUCTRL_FLASHPWRDIS_BANK1(n)          (((uint32_t)(n) << 1) & 0x00000002)
#define AM_REG_MCUCTRL_FLASHPWRDIS_BANK1_DIS         0x00000002

// Remove power from Flash Bank 0 which will cause an access to its address
// space to generate a Hard Fault.
#define AM_REG_MCUCTRL_FLASHPWRDIS_BANK0_S           0
#define AM_REG_MCUCTRL_FLASHPWRDIS_BANK0_M           0x00000001
#define AM_REG_MCUCTRL_FLASHPWRDIS_BANK0(n)          (((uint32_t)(n) << 0) & 0x00000001)
#define AM_REG_MCUCTRL_FLASHPWRDIS_BANK0_DIS         0x00000001

//*****************************************************************************
//
// MCUCTRL_ICODEFAULTADDR - ICODE bus address which was present when a bus fault
// occurred.
//
//*****************************************************************************
// The ICODE bus address observed when a Bus Fault occurred. Once an address is
// captured in this field, it is held until the corresponding Fault Observed bit
// is cleared in the FAULTSTATUS register.
#define AM_REG_MCUCTRL_ICODEFAULTADDR_ADDR_S         0
#define AM_REG_MCUCTRL_ICODEFAULTADDR_ADDR_M         0xFFFFFFFF
#define AM_REG_MCUCTRL_ICODEFAULTADDR_ADDR(n)        (((uint32_t)(n) << 0) & 0xFFFFFFFF)

//*****************************************************************************
//
// MCUCTRL_DCODEFAULTADDR - DCODE bus address which was present when a bus fault
// occurred.
//
//*****************************************************************************
// The DCODE bus address observed when a Bus Fault occurred. Once an address is
// captured in this field, it is held until the corresponding Fault Observed bit
// is cleared in the FAULTSTATUS register.
#define AM_REG_MCUCTRL_DCODEFAULTADDR_ADDR_S         0
#define AM_REG_MCUCTRL_DCODEFAULTADDR_ADDR_M         0xFFFFFFFF
#define AM_REG_MCUCTRL_DCODEFAULTADDR_ADDR(n)        (((uint32_t)(n) << 0) & 0xFFFFFFFF)

//*****************************************************************************
//
// MCUCTRL_SYSFAULTADDR - System bus address which was present when a bus fault
// occurred.
//
//*****************************************************************************
// SYS bus address observed when a Bus Fault occurred. Once an address is
// captured in this field, it is held until the corresponding Fault Observed bit
// is cleared in the FAULTSTATUS register.
#define AM_REG_MCUCTRL_SYSFAULTADDR_ADDR_S           0
#define AM_REG_MCUCTRL_SYSFAULTADDR_ADDR_M           0xFFFFFFFF
#define AM_REG_MCUCTRL_SYSFAULTADDR_ADDR(n)          (((uint32_t)(n) << 0) & 0xFFFFFFFF)

//*****************************************************************************
//
// MCUCTRL_FAULTSTATUS - Reflects the status of the bus decoders' fault
// detection. Any write to this register will clear all of the status bits
// within the register.
//
//*****************************************************************************
// SYS Bus Decoder Fault Detected bit. When set, a fault has been detected, and
// the SYSFAULTADDR register will contain the bus address which generated the
// fault.
#define AM_REG_MCUCTRL_FAULTSTATUS_SYS_S             2
#define AM_REG_MCUCTRL_FAULTSTATUS_SYS_M             0x00000004
#define AM_REG_MCUCTRL_FAULTSTATUS_SYS(n)            (((uint32_t)(n) << 2) & 0x00000004)
#define AM_REG_MCUCTRL_FAULTSTATUS_SYS_NOFAULT       0x00000000
#define AM_REG_MCUCTRL_FAULTSTATUS_SYS_FAULT         0x00000004

// DCODE Bus Decoder Fault Detected bit. When set, a fault has been detected,
// and the DCODEFAULTADDR register will contain the bus address which generated
// the fault.
#define AM_REG_MCUCTRL_FAULTSTATUS_DCODE_S           1
#define AM_REG_MCUCTRL_FAULTSTATUS_DCODE_M           0x00000002
#define AM_REG_MCUCTRL_FAULTSTATUS_DCODE(n)          (((uint32_t)(n) << 1) & 0x00000002)
#define AM_REG_MCUCTRL_FAULTSTATUS_DCODE_NOFAULT     0x00000000
#define AM_REG_MCUCTRL_FAULTSTATUS_DCODE_FAULT       0x00000002

// The ICODE Bus Decoder Fault Detected bit. When set, a fault has been
// detected, and the ICODEFAULTADDR register will contain the bus address which
// generated the fault.
#define AM_REG_MCUCTRL_FAULTSTATUS_ICODE_S           0
#define AM_REG_MCUCTRL_FAULTSTATUS_ICODE_M           0x00000001
#define AM_REG_MCUCTRL_FAULTSTATUS_ICODE(n)          (((uint32_t)(n) << 0) & 0x00000001)
#define AM_REG_MCUCTRL_FAULTSTATUS_ICODE_NOFAULT     0x00000000
#define AM_REG_MCUCTRL_FAULTSTATUS_ICODE_FAULT       0x00000001

//*****************************************************************************
//
// MCUCTRL_FAULTCAPTUREEN - Enable the fault capture registers
//
//*****************************************************************************
// Fault Capture Enable field. When set, the Fault Capture monitors are enabled
// and addresses which generate a hard fault are captured into the FAULTADDR
// registers.
#define AM_REG_MCUCTRL_FAULTCAPTUREEN_ENABLE_S       0
#define AM_REG_MCUCTRL_FAULTCAPTUREEN_ENABLE_M       0x00000001
#define AM_REG_MCUCTRL_FAULTCAPTUREEN_ENABLE(n)      (((uint32_t)(n) << 0) & 0x00000001)
#define AM_REG_MCUCTRL_FAULTCAPTUREEN_ENABLE_DIS     0x00000000
#define AM_REG_MCUCTRL_FAULTCAPTUREEN_ENABLE_EN      0x00000001

//*****************************************************************************
//
// MCUCTRL_TPIUCTRL - TPIU Control Register. Determines the clock enable and
// frequency for the M4's TPIU interface.
//
//*****************************************************************************
// This field selects the frequency of the ARM M4 TPIU port.
#define AM_REG_MCUCTRL_TPIUCTRL_CLKSEL_S             8
#define AM_REG_MCUCTRL_TPIUCTRL_CLKSEL_M             0x00000300
#define AM_REG_MCUCTRL_TPIUCTRL_CLKSEL(n)            (((uint32_t)(n) << 8) & 0x00000300)
#define AM_REG_MCUCTRL_TPIUCTRL_CLKSEL_LOW_PWR       0x00000000
#define AM_REG_MCUCTRL_TPIUCTRL_CLKSEL_0MHz          0x00000000
#define AM_REG_MCUCTRL_TPIUCTRL_CLKSEL_6MHZ          0x00000100
#define AM_REG_MCUCTRL_TPIUCTRL_CLKSEL_3MHZ          0x00000200
#define AM_REG_MCUCTRL_TPIUCTRL_CLKSEL_1_5MHZ        0x00000300

// TPIU Enable field. When set, the ARM M4 TPIU is enabled and data can be
// streamed out of the MCU's SWO port using the ARM ITM and TPIU modules.
#define AM_REG_MCUCTRL_TPIUCTRL_ENABLE_S             0
#define AM_REG_MCUCTRL_TPIUCTRL_ENABLE_M             0x00000001
#define AM_REG_MCUCTRL_TPIUCTRL_ENABLE(n)            (((uint32_t)(n) << 0) & 0x00000001)
#define AM_REG_MCUCTRL_TPIUCTRL_ENABLE_DIS           0x00000000
#define AM_REG_MCUCTRL_TPIUCTRL_ENABLE_EN            0x00000001

#endif // AM_REG_MCUCTRL_H
