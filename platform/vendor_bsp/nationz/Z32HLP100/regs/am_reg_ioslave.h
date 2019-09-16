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
 * @file     am_reg_ioslave.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Register macros for the IOSLAVE module
 ******************************************************************************/
#ifndef AM_REG_IOSLAVE_H
#define AM_REG_IOSLAVE_H

//*****************************************************************************
//
// Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_IOSLAVE_NUM_MODULES                   1
#define AM_REG_IOSLAVEn(n) \
    (REG_IOSLAVE_BASEADDR + 0x00000000 * n)

//*****************************************************************************
//
// Register offsets.
//
//*****************************************************************************
#define AM_REG_IOSLAVE_FIFOPTR_O                     0x00000100
#define AM_REG_IOSLAVE_FIFOCFG_O                     0x00000104
#define AM_REG_IOSLAVE_FIFOTHR_O                     0x00000108
#define AM_REG_IOSLAVE_FUPD_O                        0x0000010C
#define AM_REG_IOSLAVE_FIFOCTR_O                     0x00000110
#define AM_REG_IOSLAVE_FIFOINC_O                     0x00000114
#define AM_REG_IOSLAVE_CFG_O                         0x00000118
#define AM_REG_IOSLAVE_PRENC_O                       0x0000011C
#define AM_REG_IOSLAVE_IOINTCTL_O                    0x00000120
#define AM_REG_IOSLAVE_GENADD_O                      0x00000124
#define AM_REG_IOSLAVE_INTEN_O                       0x00000200
#define AM_REG_IOSLAVE_INTSTAT_O                     0x00000204
#define AM_REG_IOSLAVE_INTCLR_O                      0x00000208
#define AM_REG_IOSLAVE_INTSET_O                      0x0000020C
#define AM_REG_IOSLAVE_REGACCINTEN_O                 0x00000210
#define AM_REG_IOSLAVE_REGACCINTSTAT_O               0x00000214
#define AM_REG_IOSLAVE_REGACCINTCLR_O                0x00000218
#define AM_REG_IOSLAVE_REGACCINTSET_O                0x0000021C

//*****************************************************************************
//
// IOSLAVE_INTEN - IO Slave Interrupts: Enable
//
//*****************************************************************************
// I2C Interrupt Write interrupt.
#define AM_REG_IOSLAVE_INTEN_IOINTW_S                5
#define AM_REG_IOSLAVE_INTEN_IOINTW_M                0x00000020
#define AM_REG_IOSLAVE_INTEN_IOINTW(n)               (((uint32_t)(n) << 5) & 0x00000020)

// I2C General Address interrupt.
#define AM_REG_IOSLAVE_INTEN_GENAD_S                 4
#define AM_REG_IOSLAVE_INTEN_GENAD_M                 0x00000010
#define AM_REG_IOSLAVE_INTEN_GENAD(n)                (((uint32_t)(n) << 4) & 0x00000010)

// FIFO Read Error interrupt.
#define AM_REG_IOSLAVE_INTEN_FRDERR_S                3
#define AM_REG_IOSLAVE_INTEN_FRDERR_M                0x00000008
#define AM_REG_IOSLAVE_INTEN_FRDERR(n)               (((uint32_t)(n) << 3) & 0x00000008)

// FIFO Underflow interrupt.
#define AM_REG_IOSLAVE_INTEN_FUNDFL_S                2
#define AM_REG_IOSLAVE_INTEN_FUNDFL_M                0x00000004
#define AM_REG_IOSLAVE_INTEN_FUNDFL(n)               (((uint32_t)(n) << 2) & 0x00000004)

// FIFO Overflow interrupt.
#define AM_REG_IOSLAVE_INTEN_FOVFL_S                 1
#define AM_REG_IOSLAVE_INTEN_FOVFL_M                 0x00000002
#define AM_REG_IOSLAVE_INTEN_FOVFL(n)                (((uint32_t)(n) << 1) & 0x00000002)

// FIFO Size interrupt.
#define AM_REG_IOSLAVE_INTEN_FSIZE_S                 0
#define AM_REG_IOSLAVE_INTEN_FSIZE_M                 0x00000001
#define AM_REG_IOSLAVE_INTEN_FSIZE(n)                (((uint32_t)(n) << 0) & 0x00000001)

//*****************************************************************************
//
// IOSLAVE_INTSTAT - IO Slave Interrupts: Status
//
//*****************************************************************************
// I2C Interrupt Write interrupt.
#define AM_REG_IOSLAVE_INTSTAT_IOINTW_S              5
#define AM_REG_IOSLAVE_INTSTAT_IOINTW_M              0x00000020
#define AM_REG_IOSLAVE_INTSTAT_IOINTW(n)             (((uint32_t)(n) << 5) & 0x00000020)

// I2C General Address interrupt.
#define AM_REG_IOSLAVE_INTSTAT_GENAD_S               4
#define AM_REG_IOSLAVE_INTSTAT_GENAD_M               0x00000010
#define AM_REG_IOSLAVE_INTSTAT_GENAD(n)              (((uint32_t)(n) << 4) & 0x00000010)

// FIFO Read Error interrupt.
#define AM_REG_IOSLAVE_INTSTAT_FRDERR_S              3
#define AM_REG_IOSLAVE_INTSTAT_FRDERR_M              0x00000008
#define AM_REG_IOSLAVE_INTSTAT_FRDERR(n)             (((uint32_t)(n) << 3) & 0x00000008)

// FIFO Underflow interrupt.
#define AM_REG_IOSLAVE_INTSTAT_FUNDFL_S              2
#define AM_REG_IOSLAVE_INTSTAT_FUNDFL_M              0x00000004
#define AM_REG_IOSLAVE_INTSTAT_FUNDFL(n)             (((uint32_t)(n) << 2) & 0x00000004)

// FIFO Overflow interrupt.
#define AM_REG_IOSLAVE_INTSTAT_FOVFL_S               1
#define AM_REG_IOSLAVE_INTSTAT_FOVFL_M               0x00000002
#define AM_REG_IOSLAVE_INTSTAT_FOVFL(n)              (((uint32_t)(n) << 1) & 0x00000002)

// FIFO Size interrupt.
#define AM_REG_IOSLAVE_INTSTAT_FSIZE_S               0
#define AM_REG_IOSLAVE_INTSTAT_FSIZE_M               0x00000001
#define AM_REG_IOSLAVE_INTSTAT_FSIZE(n)              (((uint32_t)(n) << 0) & 0x00000001)

//*****************************************************************************
//
// IOSLAVE_INTCLR - IO Slave Interrupts: Clear
//
//*****************************************************************************
// I2C Interrupt Write interrupt.
#define AM_REG_IOSLAVE_INTCLR_IOINTW_S               5
#define AM_REG_IOSLAVE_INTCLR_IOINTW_M               0x00000020
#define AM_REG_IOSLAVE_INTCLR_IOINTW(n)              (((uint32_t)(n) << 5) & 0x00000020)

// I2C General Address interrupt.
#define AM_REG_IOSLAVE_INTCLR_GENAD_S                4
#define AM_REG_IOSLAVE_INTCLR_GENAD_M                0x00000010
#define AM_REG_IOSLAVE_INTCLR_GENAD(n)               (((uint32_t)(n) << 4) & 0x00000010)

// FIFO Read Error interrupt.
#define AM_REG_IOSLAVE_INTCLR_FRDERR_S               3
#define AM_REG_IOSLAVE_INTCLR_FRDERR_M               0x00000008
#define AM_REG_IOSLAVE_INTCLR_FRDERR(n)              (((uint32_t)(n) << 3) & 0x00000008)

// FIFO Underflow interrupt.
#define AM_REG_IOSLAVE_INTCLR_FUNDFL_S               2
#define AM_REG_IOSLAVE_INTCLR_FUNDFL_M               0x00000004
#define AM_REG_IOSLAVE_INTCLR_FUNDFL(n)              (((uint32_t)(n) << 2) & 0x00000004)

// FIFO Overflow interrupt.
#define AM_REG_IOSLAVE_INTCLR_FOVFL_S                1
#define AM_REG_IOSLAVE_INTCLR_FOVFL_M                0x00000002
#define AM_REG_IOSLAVE_INTCLR_FOVFL(n)               (((uint32_t)(n) << 1) & 0x00000002)

// FIFO Size interrupt.
#define AM_REG_IOSLAVE_INTCLR_FSIZE_S                0
#define AM_REG_IOSLAVE_INTCLR_FSIZE_M                0x00000001
#define AM_REG_IOSLAVE_INTCLR_FSIZE(n)               (((uint32_t)(n) << 0) & 0x00000001)

//*****************************************************************************
//
// IOSLAVE_INTSET - IO Slave Interrupts: Set
//
//*****************************************************************************
// I2C Interrupt Write interrupt.
#define AM_REG_IOSLAVE_INTSET_IOINTW_S               5
#define AM_REG_IOSLAVE_INTSET_IOINTW_M               0x00000020
#define AM_REG_IOSLAVE_INTSET_IOINTW(n)              (((uint32_t)(n) << 5) & 0x00000020)

// I2C General Address interrupt.
#define AM_REG_IOSLAVE_INTSET_GENAD_S                4
#define AM_REG_IOSLAVE_INTSET_GENAD_M                0x00000010
#define AM_REG_IOSLAVE_INTSET_GENAD(n)               (((uint32_t)(n) << 4) & 0x00000010)

// FIFO Read Error interrupt.
#define AM_REG_IOSLAVE_INTSET_FRDERR_S               3
#define AM_REG_IOSLAVE_INTSET_FRDERR_M               0x00000008
#define AM_REG_IOSLAVE_INTSET_FRDERR(n)              (((uint32_t)(n) << 3) & 0x00000008)

// FIFO Underflow interrupt.
#define AM_REG_IOSLAVE_INTSET_FUNDFL_S               2
#define AM_REG_IOSLAVE_INTSET_FUNDFL_M               0x00000004
#define AM_REG_IOSLAVE_INTSET_FUNDFL(n)              (((uint32_t)(n) << 2) & 0x00000004)

// FIFO Overflow interrupt.
#define AM_REG_IOSLAVE_INTSET_FOVFL_S                1
#define AM_REG_IOSLAVE_INTSET_FOVFL_M                0x00000002
#define AM_REG_IOSLAVE_INTSET_FOVFL(n)               (((uint32_t)(n) << 1) & 0x00000002)

// FIFO Size interrupt.
#define AM_REG_IOSLAVE_INTSET_FSIZE_S                0
#define AM_REG_IOSLAVE_INTSET_FSIZE_M                0x00000001
#define AM_REG_IOSLAVE_INTSET_FSIZE(n)               (((uint32_t)(n) << 0) & 0x00000001)

//*****************************************************************************
//
// IOSLAVE_REGACCINTEN - Register Access Interrupts: Enable
//
//*****************************************************************************
// Register access interrupts.
#define AM_REG_IOSLAVE_REGACCINTEN_REGACC_S          0
#define AM_REG_IOSLAVE_REGACCINTEN_REGACC_M          0xFFFFFFFF
#define AM_REG_IOSLAVE_REGACCINTEN_REGACC(n)         (((uint32_t)(n) << 0) & 0xFFFFFFFF)

//*****************************************************************************
//
// IOSLAVE_REGACCINTSTAT - Register Access Interrupts: Status
//
//*****************************************************************************
// Register access interrupts.
#define AM_REG_IOSLAVE_REGACCINTSTAT_REGACC_S        0
#define AM_REG_IOSLAVE_REGACCINTSTAT_REGACC_M        0xFFFFFFFF
#define AM_REG_IOSLAVE_REGACCINTSTAT_REGACC(n)       (((uint32_t)(n) << 0) & 0xFFFFFFFF)

//*****************************************************************************
//
// IOSLAVE_REGACCINTCLR - Register Access Interrupts: Clear
//
//*****************************************************************************
// Register access interrupts.
#define AM_REG_IOSLAVE_REGACCINTCLR_REGACC_S         0
#define AM_REG_IOSLAVE_REGACCINTCLR_REGACC_M         0xFFFFFFFF
#define AM_REG_IOSLAVE_REGACCINTCLR_REGACC(n)        (((uint32_t)(n) << 0) & 0xFFFFFFFF)

//*****************************************************************************
//
// IOSLAVE_REGACCINTSET - Register Access Interrupts: Set
//
//*****************************************************************************
// Register access interrupts.
#define AM_REG_IOSLAVE_REGACCINTSET_REGACC_S         0
#define AM_REG_IOSLAVE_REGACCINTSET_REGACC_M         0xFFFFFFFF
#define AM_REG_IOSLAVE_REGACCINTSET_REGACC(n)        (((uint32_t)(n) << 0) & 0xFFFFFFFF)

//*****************************************************************************
//
// IOSLAVE_FIFOPTR - Current FIFO Pointer
//
//*****************************************************************************
// The number of bytes currently in the hardware FIFO.
#define AM_REG_IOSLAVE_FIFOPTR_FIFOSIZ_S             8
#define AM_REG_IOSLAVE_FIFOPTR_FIFOSIZ_M             0x0000FF00
#define AM_REG_IOSLAVE_FIFOPTR_FIFOSIZ(n)            (((uint32_t)(n) << 8) & 0x0000FF00)

// Current FIFO pointer.
#define AM_REG_IOSLAVE_FIFOPTR_FIFOPTR_S             0
#define AM_REG_IOSLAVE_FIFOPTR_FIFOPTR_M             0x000000FF
#define AM_REG_IOSLAVE_FIFOPTR_FIFOPTR(n)            (((uint32_t)(n) << 0) & 0x000000FF)

//*****************************************************************************
//
// IOSLAVE_FIFOCFG - FIFO Configuration
//
//*****************************************************************************
// Defines the read-only area.  The IO Slave read-only area is situated in LRAM
// at (ROBASE*8) to (FIFOOBASE*8-1)
#define AM_REG_IOSLAVE_FIFOCFG_ROBASE_S              24
#define AM_REG_IOSLAVE_FIFOCFG_ROBASE_M              0x3F000000
#define AM_REG_IOSLAVE_FIFOCFG_ROBASE(n)             (((uint32_t)(n) << 24) & 0x3F000000)

// These bits hold the maximum FIFO address in 8 byte segments.  It is also the
// beginning of the RAM area of the LRAM.  Note that no RAM area is configured
// if FIFOMAX is set to 0x1F.
#define AM_REG_IOSLAVE_FIFOCFG_FIFOMAX_S             8
#define AM_REG_IOSLAVE_FIFOCFG_FIFOMAX_M             0x00003F00
#define AM_REG_IOSLAVE_FIFOCFG_FIFOMAX(n)            (((uint32_t)(n) << 8) & 0x00003F00)

// These bits hold the base address of the I/O FIFO in 8 byte segments. The IO
// Slave FIFO is situated in LRAM at (FIFOBASE*8) to (FIFOMAX*8-1).
#define AM_REG_IOSLAVE_FIFOCFG_FIFOBASE_S            0
#define AM_REG_IOSLAVE_FIFOCFG_FIFOBASE_M            0x0000001F
#define AM_REG_IOSLAVE_FIFOCFG_FIFOBASE(n)           (((uint32_t)(n) << 0) & 0x0000001F)

//*****************************************************************************
//
// IOSLAVE_FIFOTHR - FIFO Threshold Configuration
//
//*****************************************************************************
// FIFO size interrupt threshold.
#define AM_REG_IOSLAVE_FIFOTHR_FIFOTHR_S             0
#define AM_REG_IOSLAVE_FIFOTHR_FIFOTHR_M             0x000000FF
#define AM_REG_IOSLAVE_FIFOTHR_FIFOTHR(n)            (((uint32_t)(n) << 0) & 0x000000FF)

//*****************************************************************************
//
// IOSLAVE_FUPD - FIFO Update Status
//
//*****************************************************************************
// This bitfield indicates an IO read is active.
#define AM_REG_IOSLAVE_FUPD_IOREAD_S                 1
#define AM_REG_IOSLAVE_FUPD_IOREAD_M                 0x00000002
#define AM_REG_IOSLAVE_FUPD_IOREAD(n)                (((uint32_t)(n) << 1) & 0x00000002)

// This bit indicates that a FIFO update is underway.
#define AM_REG_IOSLAVE_FUPD_FIFOUPD_S                0
#define AM_REG_IOSLAVE_FUPD_FIFOUPD_M                0x00000001
#define AM_REG_IOSLAVE_FUPD_FIFOUPD(n)               (((uint32_t)(n) << 0) & 0x00000001)

//*****************************************************************************
//
// IOSLAVE_FIFOCTR - Overall FIFO Counter
//
//*****************************************************************************
// Virtual FIFO byte count
#define AM_REG_IOSLAVE_FIFOCTR_FIFOCTR_S             0
#define AM_REG_IOSLAVE_FIFOCTR_FIFOCTR_M             0x000003FF
#define AM_REG_IOSLAVE_FIFOCTR_FIFOCTR(n)            (((uint32_t)(n) << 0) & 0x000003FF)

//*****************************************************************************
//
// IOSLAVE_FIFOINC - Overall FIFO Counter Increment
//
//*****************************************************************************
// Increment the Overall FIFO Counter by this value on a write
#define AM_REG_IOSLAVE_FIFOINC_FIFOINC_S             0
#define AM_REG_IOSLAVE_FIFOINC_FIFOINC_M             0x000003FF
#define AM_REG_IOSLAVE_FIFOINC_FIFOINC(n)            (((uint32_t)(n) << 0) & 0x000003FF)

//*****************************************************************************
//
// IOSLAVE_CFG - I/O Slave Configuration
//
//*****************************************************************************
// IOSLAVE interface enable.
#define AM_REG_IOSLAVE_CFG_IFCEN_S                   31
#define AM_REG_IOSLAVE_CFG_IFCEN_M                   0x80000000
#define AM_REG_IOSLAVE_CFG_IFCEN(n)                  (((uint32_t)(n) << 31) & 0x80000000)
#define AM_REG_IOSLAVE_CFG_IFCEN_DIS                 0x00000000
#define AM_REG_IOSLAVE_CFG_IFCEN_EN                  0x80000000

// 7-bit or 10-bit I2C device address.
#define AM_REG_IOSLAVE_CFG_I2CADDR_S                 8
#define AM_REG_IOSLAVE_CFG_I2CADDR_M                 0x000FFF00
#define AM_REG_IOSLAVE_CFG_I2CADDR(n)                (((uint32_t)(n) << 8) & 0x000FFF00)

// This bit holds the cycle to initiate an I/O RAM read.
#define AM_REG_IOSLAVE_CFG_STARTRD_S                 4
#define AM_REG_IOSLAVE_CFG_STARTRD_M                 0x00000010
#define AM_REG_IOSLAVE_CFG_STARTRD(n)                (((uint32_t)(n) << 4) & 0x00000010)
#define AM_REG_IOSLAVE_CFG_STARTRD_LATE              0x00000000
#define AM_REG_IOSLAVE_CFG_STARTRD_EARLY             0x00000010

// This bit selects the transfer bit ordering.
#define AM_REG_IOSLAVE_CFG_LSB_S                     2
#define AM_REG_IOSLAVE_CFG_LSB_M                     0x00000004
#define AM_REG_IOSLAVE_CFG_LSB(n)                    (((uint32_t)(n) << 2) & 0x00000004)
#define AM_REG_IOSLAVE_CFG_LSB_MSB_FIRST             0x00000000
#define AM_REG_IOSLAVE_CFG_LSB_LSB_FIRST             0x00000004

// This bit selects SPI polarity.
#define AM_REG_IOSLAVE_CFG_SPOL_S                    1
#define AM_REG_IOSLAVE_CFG_SPOL_M                    0x00000002
#define AM_REG_IOSLAVE_CFG_SPOL(n)                   (((uint32_t)(n) << 1) & 0x00000002)
#define AM_REG_IOSLAVE_CFG_SPOL_SPI_MODES_0_3        0x00000000
#define AM_REG_IOSLAVE_CFG_SPOL_SPI_MODES_1_2        0x00000002

// This bit selects the I/O interface.
#define AM_REG_IOSLAVE_CFG_IFCSEL_S                  0
#define AM_REG_IOSLAVE_CFG_IFCSEL_M                  0x00000001
#define AM_REG_IOSLAVE_CFG_IFCSEL(n)                 (((uint32_t)(n) << 0) & 0x00000001)
#define AM_REG_IOSLAVE_CFG_IFCSEL_I2C                0x00000000
#define AM_REG_IOSLAVE_CFG_IFCSEL_SPI                0x00000001

//*****************************************************************************
//
// IOSLAVE_PRENC - I/O Slave Interrupt Priority Encode
//
//*****************************************************************************
// These bits hold the priority encode of the REGACC interrupts.
#define AM_REG_IOSLAVE_PRENC_PRENC_S                 0
#define AM_REG_IOSLAVE_PRENC_PRENC_M                 0x0000001F
#define AM_REG_IOSLAVE_PRENC_PRENC(n)                (((uint32_t)(n) << 0) & 0x0000001F)

//*****************************************************************************
//
// IOSLAVE_IOINTCTL - I/O Interrupt Control
//
//*****************************************************************************
// These bits set the IOINT interrupts when written with a 1.
#define AM_REG_IOSLAVE_IOINTCTL_IOINTSET_S           24
#define AM_REG_IOSLAVE_IOINTCTL_IOINTSET_M           0xFF000000
#define AM_REG_IOSLAVE_IOINTCTL_IOINTSET(n)          (((uint32_t)(n) << 24) & 0xFF000000)

// This bit clears all of the IOINT interrupts when written with a 1.
#define AM_REG_IOSLAVE_IOINTCTL_IOINTCLR_S           16
#define AM_REG_IOSLAVE_IOINTCTL_IOINTCLR_M           0x00010000
#define AM_REG_IOSLAVE_IOINTCTL_IOINTCLR(n)          (((uint32_t)(n) << 16) & 0x00010000)

// These bits read the IOINT interrupts.
#define AM_REG_IOSLAVE_IOINTCTL_IOINT_S              8
#define AM_REG_IOSLAVE_IOINTCTL_IOINT_M              0x0000FF00
#define AM_REG_IOSLAVE_IOINTCTL_IOINT(n)             (((uint32_t)(n) << 8) & 0x0000FF00)

// These bits setread the IOINT interrupt enables.
#define AM_REG_IOSLAVE_IOINTCTL_IOINTEN_S            0
#define AM_REG_IOSLAVE_IOINTCTL_IOINTEN_M            0x000000FF
#define AM_REG_IOSLAVE_IOINTCTL_IOINTEN(n)           (((uint32_t)(n) << 0) & 0x000000FF)

//*****************************************************************************
//
// IOSLAVE_GENADD - General Address Data
//
//*****************************************************************************
// The data supplied on the last General Address reference.
#define AM_REG_IOSLAVE_GENADD_GADATA_S               0
#define AM_REG_IOSLAVE_GENADD_GADATA_M               0x000000FF
#define AM_REG_IOSLAVE_GENADD_GADATA(n)              (((uint32_t)(n) << 0) & 0x000000FF)

#endif // AM_REG_IOSLAVE_H
