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
 * @file     am_reg_iomstr.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Register macros for the IOMSTR module
 ******************************************************************************/
#ifndef AM_REG_IOMSTR_H
#define AM_REG_IOMSTR_H

//*****************************************************************************
//
// Instance finder. (2 instance(s) available)
//
//*****************************************************************************
#define AM_REG_IOMSTR_NUM_MODULES                    2
#define AM_REG_IOMSTRn(n) \
    (REG_IOMSTR_BASEADDR + 0x00001000 * n)

//*****************************************************************************
//
// Register offsets.
//
//*****************************************************************************
#define AM_REG_IOMSTR_FIFO_O                         0x00000000
#define AM_REG_IOMSTR_FIFOPTR_O                      0x00000100
#define AM_REG_IOMSTR_TLNGTH_O                       0x00000104
#define AM_REG_IOMSTR_FIFOTHR_O                      0x00000108
#define AM_REG_IOMSTR_CLKCFG_O                       0x0000010C
#define AM_REG_IOMSTR_CMD_O                          0x00000110
#define AM_REG_IOMSTR_CMDRPT_O                       0x00000114
#define AM_REG_IOMSTR_STATUS_O                       0x00000118
#define AM_REG_IOMSTR_CFG_O                          0x0000011C
#define AM_REG_IOMSTR_INTEN_O                        0x00000200
#define AM_REG_IOMSTR_INTSTAT_O                      0x00000204
#define AM_REG_IOMSTR_INTCLR_O                       0x00000208
#define AM_REG_IOMSTR_INTSET_O                       0x0000020C

//*****************************************************************************
//
// IOMSTR_INTEN - IO Master Interrupts: Enable
//
//*****************************************************************************
// This is the arbitration loss interrupt.
#define AM_REG_IOMSTR_INTEN_ARB_S                    10
#define AM_REG_IOMSTR_INTEN_ARB_M                    0x00000400
#define AM_REG_IOMSTR_INTEN_ARB(n)                   (((uint32_t)(n) << 10) & 0x00000400)

// This is the STOP command interrupt.
#define AM_REG_IOMSTR_INTEN_STOP_S                   9
#define AM_REG_IOMSTR_INTEN_STOP_M                   0x00000200
#define AM_REG_IOMSTR_INTEN_STOP(n)                  (((uint32_t)(n) << 9) & 0x00000200)

// This is the START command interrupt.
#define AM_REG_IOMSTR_INTEN_START_S                  8
#define AM_REG_IOMSTR_INTEN_START_M                  0x00000100
#define AM_REG_IOMSTR_INTEN_START(n)                 (((uint32_t)(n) << 8) & 0x00000100)

// This is the illegal command interrupt.
#define AM_REG_IOMSTR_INTEN_ICMD_S                   7
#define AM_REG_IOMSTR_INTEN_ICMD_M                   0x00000080
#define AM_REG_IOMSTR_INTEN_ICMD(n)                  (((uint32_t)(n) << 7) & 0x00000080)

// This is the illegal FIFO access interrupt.
#define AM_REG_IOMSTR_INTEN_IACC_S                   6
#define AM_REG_IOMSTR_INTEN_IACC_M                   0x00000040
#define AM_REG_IOMSTR_INTEN_IACC(n)                  (((uint32_t)(n) << 6) & 0x00000040)

// This is the write length mismatch interrupt.
#define AM_REG_IOMSTR_INTEN_WTLEN_S                  5
#define AM_REG_IOMSTR_INTEN_WTLEN_M                  0x00000020
#define AM_REG_IOMSTR_INTEN_WTLEN(n)                 (((uint32_t)(n) << 5) & 0x00000020)

// This is the I2C NAK interrupt.
#define AM_REG_IOMSTR_INTEN_NAK_S                    4
#define AM_REG_IOMSTR_INTEN_NAK_M                    0x00000010
#define AM_REG_IOMSTR_INTEN_NAK(n)                   (((uint32_t)(n) << 4) & 0x00000010)

// This is the Read FIFO Overflow interrupt.
#define AM_REG_IOMSTR_INTEN_FOVFL_S                  3
#define AM_REG_IOMSTR_INTEN_FOVFL_M                  0x00000008
#define AM_REG_IOMSTR_INTEN_FOVFL(n)                 (((uint32_t)(n) << 3) & 0x00000008)

// This is the Write FIFO Underflow interrupt.
#define AM_REG_IOMSTR_INTEN_FUNDFL_S                 2
#define AM_REG_IOMSTR_INTEN_FUNDFL_M                 0x00000004
#define AM_REG_IOMSTR_INTEN_FUNDFL(n)                (((uint32_t)(n) << 2) & 0x00000004)

// This is the FIFO Threshold interrupt.
#define AM_REG_IOMSTR_INTEN_THR_S                    1
#define AM_REG_IOMSTR_INTEN_THR_M                    0x00000002
#define AM_REG_IOMSTR_INTEN_THR(n)                   (((uint32_t)(n) << 1) & 0x00000002)

// This is the Command Complete interrupt.
#define AM_REG_IOMSTR_INTEN_CMDCMP_S                 0
#define AM_REG_IOMSTR_INTEN_CMDCMP_M                 0x00000001
#define AM_REG_IOMSTR_INTEN_CMDCMP(n)                (((uint32_t)(n) << 0) & 0x00000001)

//*****************************************************************************
//
// IOMSTR_INTSTAT - IO Master Interrupts: Status
//
//*****************************************************************************
// This is the arbitration loss interrupt.
#define AM_REG_IOMSTR_INTSTAT_ARB_S                  10
#define AM_REG_IOMSTR_INTSTAT_ARB_M                  0x00000400
#define AM_REG_IOMSTR_INTSTAT_ARB(n)                 (((uint32_t)(n) << 10) & 0x00000400)

// This is the STOP command interrupt.
#define AM_REG_IOMSTR_INTSTAT_STOP_S                 9
#define AM_REG_IOMSTR_INTSTAT_STOP_M                 0x00000200
#define AM_REG_IOMSTR_INTSTAT_STOP(n)                (((uint32_t)(n) << 9) & 0x00000200)

// This is the START command interrupt.
#define AM_REG_IOMSTR_INTSTAT_START_S                8
#define AM_REG_IOMSTR_INTSTAT_START_M                0x00000100
#define AM_REG_IOMSTR_INTSTAT_START(n)               (((uint32_t)(n) << 8) & 0x00000100)

// This is the illegal command interrupt.
#define AM_REG_IOMSTR_INTSTAT_ICMD_S                 7
#define AM_REG_IOMSTR_INTSTAT_ICMD_M                 0x00000080
#define AM_REG_IOMSTR_INTSTAT_ICMD(n)                (((uint32_t)(n) << 7) & 0x00000080)

// This is the illegal FIFO access interrupt.
#define AM_REG_IOMSTR_INTSTAT_IACC_S                 6
#define AM_REG_IOMSTR_INTSTAT_IACC_M                 0x00000040
#define AM_REG_IOMSTR_INTSTAT_IACC(n)                (((uint32_t)(n) << 6) & 0x00000040)

// This is the write length mismatch interrupt.
#define AM_REG_IOMSTR_INTSTAT_WTLEN_S                5
#define AM_REG_IOMSTR_INTSTAT_WTLEN_M                0x00000020
#define AM_REG_IOMSTR_INTSTAT_WTLEN(n)               (((uint32_t)(n) << 5) & 0x00000020)

// This is the I2C NAK interrupt.
#define AM_REG_IOMSTR_INTSTAT_NAK_S                  4
#define AM_REG_IOMSTR_INTSTAT_NAK_M                  0x00000010
#define AM_REG_IOMSTR_INTSTAT_NAK(n)                 (((uint32_t)(n) << 4) & 0x00000010)

// This is the Read FIFO Overflow interrupt.
#define AM_REG_IOMSTR_INTSTAT_FOVFL_S                3
#define AM_REG_IOMSTR_INTSTAT_FOVFL_M                0x00000008
#define AM_REG_IOMSTR_INTSTAT_FOVFL(n)               (((uint32_t)(n) << 3) & 0x00000008)

// This is the Write FIFO Underflow interrupt.
#define AM_REG_IOMSTR_INTSTAT_FUNDFL_S               2
#define AM_REG_IOMSTR_INTSTAT_FUNDFL_M               0x00000004
#define AM_REG_IOMSTR_INTSTAT_FUNDFL(n)              (((uint32_t)(n) << 2) & 0x00000004)

// This is the FIFO Threshold interrupt.
#define AM_REG_IOMSTR_INTSTAT_THR_S                  1
#define AM_REG_IOMSTR_INTSTAT_THR_M                  0x00000002
#define AM_REG_IOMSTR_INTSTAT_THR(n)                 (((uint32_t)(n) << 1) & 0x00000002)

// This is the Command Complete interrupt.
#define AM_REG_IOMSTR_INTSTAT_CMDCMP_S               0
#define AM_REG_IOMSTR_INTSTAT_CMDCMP_M               0x00000001
#define AM_REG_IOMSTR_INTSTAT_CMDCMP(n)              (((uint32_t)(n) << 0) & 0x00000001)

//*****************************************************************************
//
// IOMSTR_INTCLR - IO Master Interrupts: Clear
//
//*****************************************************************************
// This is the arbitration loss interrupt.
#define AM_REG_IOMSTR_INTCLR_ARB_S                   10
#define AM_REG_IOMSTR_INTCLR_ARB_M                   0x00000400
#define AM_REG_IOMSTR_INTCLR_ARB(n)                  (((uint32_t)(n) << 10) & 0x00000400)

// This is the STOP command interrupt.
#define AM_REG_IOMSTR_INTCLR_STOP_S                  9
#define AM_REG_IOMSTR_INTCLR_STOP_M                  0x00000200
#define AM_REG_IOMSTR_INTCLR_STOP(n)                 (((uint32_t)(n) << 9) & 0x00000200)

// This is the START command interrupt.
#define AM_REG_IOMSTR_INTCLR_START_S                 8
#define AM_REG_IOMSTR_INTCLR_START_M                 0x00000100
#define AM_REG_IOMSTR_INTCLR_START(n)                (((uint32_t)(n) << 8) & 0x00000100)

// This is the illegal command interrupt.
#define AM_REG_IOMSTR_INTCLR_ICMD_S                  7
#define AM_REG_IOMSTR_INTCLR_ICMD_M                  0x00000080
#define AM_REG_IOMSTR_INTCLR_ICMD(n)                 (((uint32_t)(n) << 7) & 0x00000080)

// This is the illegal FIFO access interrupt.
#define AM_REG_IOMSTR_INTCLR_IACC_S                  6
#define AM_REG_IOMSTR_INTCLR_IACC_M                  0x00000040
#define AM_REG_IOMSTR_INTCLR_IACC(n)                 (((uint32_t)(n) << 6) & 0x00000040)

// This is the write length mismatch interrupt.
#define AM_REG_IOMSTR_INTCLR_WTLEN_S                 5
#define AM_REG_IOMSTR_INTCLR_WTLEN_M                 0x00000020
#define AM_REG_IOMSTR_INTCLR_WTLEN(n)                (((uint32_t)(n) << 5) & 0x00000020)

// This is the I2C NAK interrupt.
#define AM_REG_IOMSTR_INTCLR_NAK_S                   4
#define AM_REG_IOMSTR_INTCLR_NAK_M                   0x00000010
#define AM_REG_IOMSTR_INTCLR_NAK(n)                  (((uint32_t)(n) << 4) & 0x00000010)

// This is the Read FIFO Overflow interrupt.
#define AM_REG_IOMSTR_INTCLR_FOVFL_S                 3
#define AM_REG_IOMSTR_INTCLR_FOVFL_M                 0x00000008
#define AM_REG_IOMSTR_INTCLR_FOVFL(n)                (((uint32_t)(n) << 3) & 0x00000008)

// This is the Write FIFO Underflow interrupt.
#define AM_REG_IOMSTR_INTCLR_FUNDFL_S                2
#define AM_REG_IOMSTR_INTCLR_FUNDFL_M                0x00000004
#define AM_REG_IOMSTR_INTCLR_FUNDFL(n)               (((uint32_t)(n) << 2) & 0x00000004)

// This is the FIFO Threshold interrupt.
#define AM_REG_IOMSTR_INTCLR_THR_S                   1
#define AM_REG_IOMSTR_INTCLR_THR_M                   0x00000002
#define AM_REG_IOMSTR_INTCLR_THR(n)                  (((uint32_t)(n) << 1) & 0x00000002)

// This is the Command Complete interrupt.
#define AM_REG_IOMSTR_INTCLR_CMDCMP_S                0
#define AM_REG_IOMSTR_INTCLR_CMDCMP_M                0x00000001
#define AM_REG_IOMSTR_INTCLR_CMDCMP(n)               (((uint32_t)(n) << 0) & 0x00000001)

//*****************************************************************************
//
// IOMSTR_INTSET - IO Master Interrupts: Set
//
//*****************************************************************************
// This is the arbitration loss interrupt.
#define AM_REG_IOMSTR_INTSET_ARB_S                   10
#define AM_REG_IOMSTR_INTSET_ARB_M                   0x00000400
#define AM_REG_IOMSTR_INTSET_ARB(n)                  (((uint32_t)(n) << 10) & 0x00000400)

// This is the STOP command interrupt.
#define AM_REG_IOMSTR_INTSET_STOP_S                  9
#define AM_REG_IOMSTR_INTSET_STOP_M                  0x00000200
#define AM_REG_IOMSTR_INTSET_STOP(n)                 (((uint32_t)(n) << 9) & 0x00000200)

// This is the START command interrupt.
#define AM_REG_IOMSTR_INTSET_START_S                 8
#define AM_REG_IOMSTR_INTSET_START_M                 0x00000100
#define AM_REG_IOMSTR_INTSET_START(n)                (((uint32_t)(n) << 8) & 0x00000100)

// This is the illegal command interrupt.
#define AM_REG_IOMSTR_INTSET_ICMD_S                  7
#define AM_REG_IOMSTR_INTSET_ICMD_M                  0x00000080
#define AM_REG_IOMSTR_INTSET_ICMD(n)                 (((uint32_t)(n) << 7) & 0x00000080)

// This is the illegal FIFO access interrupt.
#define AM_REG_IOMSTR_INTSET_IACC_S                  6
#define AM_REG_IOMSTR_INTSET_IACC_M                  0x00000040
#define AM_REG_IOMSTR_INTSET_IACC(n)                 (((uint32_t)(n) << 6) & 0x00000040)

// This is the write length mismatch interrupt.
#define AM_REG_IOMSTR_INTSET_WTLEN_S                 5
#define AM_REG_IOMSTR_INTSET_WTLEN_M                 0x00000020
#define AM_REG_IOMSTR_INTSET_WTLEN(n)                (((uint32_t)(n) << 5) & 0x00000020)

// This is the I2C NAK interrupt.
#define AM_REG_IOMSTR_INTSET_NAK_S                   4
#define AM_REG_IOMSTR_INTSET_NAK_M                   0x00000010
#define AM_REG_IOMSTR_INTSET_NAK(n)                  (((uint32_t)(n) << 4) & 0x00000010)

// This is the Read FIFO Overflow interrupt.
#define AM_REG_IOMSTR_INTSET_FOVFL_S                 3
#define AM_REG_IOMSTR_INTSET_FOVFL_M                 0x00000008
#define AM_REG_IOMSTR_INTSET_FOVFL(n)                (((uint32_t)(n) << 3) & 0x00000008)

// This is the Write FIFO Underflow interrupt.
#define AM_REG_IOMSTR_INTSET_FUNDFL_S                2
#define AM_REG_IOMSTR_INTSET_FUNDFL_M                0x00000004
#define AM_REG_IOMSTR_INTSET_FUNDFL(n)               (((uint32_t)(n) << 2) & 0x00000004)

// This is the FIFO Threshold interrupt.
#define AM_REG_IOMSTR_INTSET_THR_S                   1
#define AM_REG_IOMSTR_INTSET_THR_M                   0x00000002
#define AM_REG_IOMSTR_INTSET_THR(n)                  (((uint32_t)(n) << 1) & 0x00000002)

// This is the Command Complete interrupt.
#define AM_REG_IOMSTR_INTSET_CMDCMP_S                0
#define AM_REG_IOMSTR_INTSET_CMDCMP_M                0x00000001
#define AM_REG_IOMSTR_INTSET_CMDCMP(n)               (((uint32_t)(n) << 0) & 0x00000001)

//*****************************************************************************
//
// IOMSTR_FIFO - FIFO Access Port
//
//*****************************************************************************
// FIFO access port.
#define AM_REG_IOMSTR_FIFO_FIFO_S                    0
#define AM_REG_IOMSTR_FIFO_FIFO_M                    0xFFFFFFFF
#define AM_REG_IOMSTR_FIFO_FIFO(n)                   (((uint32_t)(n) << 0) & 0xFFFFFFFF)

//*****************************************************************************
//
// IOMSTR_FIFOPTR - Current FIFO Pointers
//
//*****************************************************************************
// The number of bytes remaining in the FIFO (i.e. 64-FIFOSIZ).
#define AM_REG_IOMSTR_FIFOPTR_FIFOREM_S              16
#define AM_REG_IOMSTR_FIFOPTR_FIFOREM_M              0x007F0000
#define AM_REG_IOMSTR_FIFOPTR_FIFOREM(n)             (((uint32_t)(n) << 16) & 0x007F0000)

// The number of bytes currently in the FIFO.
#define AM_REG_IOMSTR_FIFOPTR_FIFOSIZ_S              0
#define AM_REG_IOMSTR_FIFOPTR_FIFOSIZ_M              0x0000007F
#define AM_REG_IOMSTR_FIFOPTR_FIFOSIZ(n)             (((uint32_t)(n) << 0) & 0x0000007F)

//*****************************************************************************
//
// IOMSTR_TLNGTH - Transfer Length
//
//*****************************************************************************
// Remaining transfer length.
#define AM_REG_IOMSTR_TLNGTH_TLNGTH_S                0
#define AM_REG_IOMSTR_TLNGTH_TLNGTH_M                0x00000FFF
#define AM_REG_IOMSTR_TLNGTH_TLNGTH(n)               (((uint32_t)(n) << 0) & 0x00000FFF)

//*****************************************************************************
//
// IOMSTR_FIFOTHR - FIFO Threshold Configuration
//
//*****************************************************************************
// FIFO write threshold.
#define AM_REG_IOMSTR_FIFOTHR_FIFOWTHR_S             8
#define AM_REG_IOMSTR_FIFOTHR_FIFOWTHR_M             0x00003F00
#define AM_REG_IOMSTR_FIFOTHR_FIFOWTHR(n)            (((uint32_t)(n) << 8) & 0x00003F00)

// FIFO read threshold.
#define AM_REG_IOMSTR_FIFOTHR_FIFORTHR_S             0
#define AM_REG_IOMSTR_FIFOTHR_FIFORTHR_M             0x0000003F
#define AM_REG_IOMSTR_FIFOTHR_FIFORTHR(n)            (((uint32_t)(n) << 0) & 0x0000003F)

//*****************************************************************************
//
// IOMSTR_CLKCFG - I/O Clock Configuration
//
//*****************************************************************************
// Clock total count minus 1.
#define AM_REG_IOMSTR_CLKCFG_TOTPER_S                24
#define AM_REG_IOMSTR_CLKCFG_TOTPER_M                0xFF000000
#define AM_REG_IOMSTR_CLKCFG_TOTPER(n)               (((uint32_t)(n) << 24) & 0xFF000000)

// Clock low count minus 1.
#define AM_REG_IOMSTR_CLKCFG_LOWPER_S                16
#define AM_REG_IOMSTR_CLKCFG_LOWPER_M                0x00FF0000
#define AM_REG_IOMSTR_CLKCFG_LOWPER(n)               (((uint32_t)(n) << 16) & 0x00FF0000)

// Enable clock division by TOTPER.
#define AM_REG_IOMSTR_CLKCFG_DIVEN_S                 12
#define AM_REG_IOMSTR_CLKCFG_DIVEN_M                 0x00001000
#define AM_REG_IOMSTR_CLKCFG_DIVEN(n)                (((uint32_t)(n) << 12) & 0x00001000)
#define AM_REG_IOMSTR_CLKCFG_DIVEN_DIS               0x00000000
#define AM_REG_IOMSTR_CLKCFG_DIVEN_EN                0x00001000

// Enable divide by 3.
#define AM_REG_IOMSTR_CLKCFG_DIV3_S                  11
#define AM_REG_IOMSTR_CLKCFG_DIV3_M                  0x00000800
#define AM_REG_IOMSTR_CLKCFG_DIV3(n)                 (((uint32_t)(n) << 11) & 0x00000800)
#define AM_REG_IOMSTR_CLKCFG_DIV3_DIS                0x00000000
#define AM_REG_IOMSTR_CLKCFG_DIV3_EN                 0x00000800

// Select the input clock frequency.
#define AM_REG_IOMSTR_CLKCFG_FSEL_S                  8
#define AM_REG_IOMSTR_CLKCFG_FSEL_M                  0x00000700
#define AM_REG_IOMSTR_CLKCFG_FSEL(n)                 (((uint32_t)(n) << 8) & 0x00000700)
#define AM_REG_IOMSTR_CLKCFG_FSEL_HFRC_DIV64         0x00000000
#define AM_REG_IOMSTR_CLKCFG_FSEL_HFRC               0x00000100
#define AM_REG_IOMSTR_CLKCFG_FSEL_HFRC_DIV2          0x00000200
#define AM_REG_IOMSTR_CLKCFG_FSEL_HFRC_DIV4          0x00000300
#define AM_REG_IOMSTR_CLKCFG_FSEL_HFRC_DIV8          0x00000400
#define AM_REG_IOMSTR_CLKCFG_FSEL_HFRC_DIV16         0x00000500
#define AM_REG_IOMSTR_CLKCFG_FSEL_HFRC_DIV32         0x00000600
#define AM_REG_IOMSTR_CLKCFG_FSEL_RSVD               0x00000700

//*****************************************************************************
//
// IOMSTR_CMD - Command Register
//
//*****************************************************************************
// This register is the I/O Command.
#define AM_REG_IOMSTR_CMD_CMD_S                      0
#define AM_REG_IOMSTR_CMD_CMD_M                      0xFFFFFFFF
#define AM_REG_IOMSTR_CMD_CMD(n)                     (((uint32_t)(n) << 0) & 0xFFFFFFFF)
#define AM_REG_IOMSTR_CMD_CMD_POS_LENGTH             0x00000000
#define AM_REG_IOMSTR_CMD_CMD_POS_OFFSET             0x00000008
#define AM_REG_IOMSTR_CMD_CMD_POS_ADDRESS            0x00000010
#define AM_REG_IOMSTR_CMD_CMD_POS_CHNL               0x00000010
#define AM_REG_IOMSTR_CMD_CMD_POS_UPLNGTH            0x00000017
#define AM_REG_IOMSTR_CMD_CMD_POS_10BIT              0x0000001A
#define AM_REG_IOMSTR_CMD_CMD_POS_LSB                0x0000001B
#define AM_REG_IOMSTR_CMD_CMD_POS_CONT               0x0000001C
#define AM_REG_IOMSTR_CMD_CMD_POS_OPER               0x0000001D
#define AM_REG_IOMSTR_CMD_CMD_MSK_LENGTH             0x000000FF
#define AM_REG_IOMSTR_CMD_CMD_MSK_OFFSET             0x0000FF00
#define AM_REG_IOMSTR_CMD_CMD_MSK_ADDRESS            0x00FF0000
#define AM_REG_IOMSTR_CMD_CMD_MSK_CHNL               0x00070000
#define AM_REG_IOMSTR_CMD_CMD_MSK_UPLNGTH            0x07800000
#define AM_REG_IOMSTR_CMD_CMD_MSK_10BIT              0x04000000
#define AM_REG_IOMSTR_CMD_CMD_MSK_LSB                0x08000000
#define AM_REG_IOMSTR_CMD_CMD_MSK_CONT               0x10000000
#define AM_REG_IOMSTR_CMD_CMD_MSK_OPER               0xE0000000

//*****************************************************************************
//
// IOMSTR_CMDRPT - Command Repeat Register
//
//*****************************************************************************
// These bits hold the Command repeat count.
#define AM_REG_IOMSTR_CMDRPT_CMDRPT_S                0
#define AM_REG_IOMSTR_CMDRPT_CMDRPT_M                0x0000001F
#define AM_REG_IOMSTR_CMDRPT_CMDRPT(n)               (((uint32_t)(n) << 0) & 0x0000001F)

//*****************************************************************************
//
// IOMSTR_STATUS - Status Register
//
//*****************************************************************************
// This bit indicates if the I/O state machine is IDLE.
#define AM_REG_IOMSTR_STATUS_IDLEST_S                2
#define AM_REG_IOMSTR_STATUS_IDLEST_M                0x00000004
#define AM_REG_IOMSTR_STATUS_IDLEST(n)               (((uint32_t)(n) << 2) & 0x00000004)
#define AM_REG_IOMSTR_STATUS_IDLEST_IDLE             0x00000004

// This bit indicates if the I/O Command is active.
#define AM_REG_IOMSTR_STATUS_CMDACT_S                1
#define AM_REG_IOMSTR_STATUS_CMDACT_M                0x00000002
#define AM_REG_IOMSTR_STATUS_CMDACT(n)               (((uint32_t)(n) << 1) & 0x00000002)
#define AM_REG_IOMSTR_STATUS_CMDACT_ACTIVE           0x00000002

// This bit indicates if an error interrupt has occurred.
#define AM_REG_IOMSTR_STATUS_ERR_S                   0
#define AM_REG_IOMSTR_STATUS_ERR_M                   0x00000001
#define AM_REG_IOMSTR_STATUS_ERR(n)                  (((uint32_t)(n) << 0) & 0x00000001)
#define AM_REG_IOMSTR_STATUS_ERR_ERROR               0x00000001

//*****************************************************************************
//
// IOMSTR_CFG - I/O Master Configuration
//
//*****************************************************************************
// This bit enables the IO Master.
#define AM_REG_IOMSTR_CFG_IFCEN_S                    31
#define AM_REG_IOMSTR_CFG_IFCEN_M                    0x80000000
#define AM_REG_IOMSTR_CFG_IFCEN(n)                   (((uint32_t)(n) << 31) & 0x80000000)
#define AM_REG_IOMSTR_CFG_IFCEN_DIS                  0x00000000
#define AM_REG_IOMSTR_CFG_IFCEN_EN                   0x80000000

// This bit selects SPI phase.
#define AM_REG_IOMSTR_CFG_SPHA_S                     2
#define AM_REG_IOMSTR_CFG_SPHA_M                     0x00000004
#define AM_REG_IOMSTR_CFG_SPHA(n)                    (((uint32_t)(n) << 2) & 0x00000004)
#define AM_REG_IOMSTR_CFG_SPHA_SAMPLE_LEADING_EDGE   0x00000000
#define AM_REG_IOMSTR_CFG_SPHA_SAMPLE_TRAILING_EDGE  0x00000004

// This bit selects SPI polarity.
#define AM_REG_IOMSTR_CFG_SPOL_S                     1
#define AM_REG_IOMSTR_CFG_SPOL_M                     0x00000002
#define AM_REG_IOMSTR_CFG_SPOL(n)                    (((uint32_t)(n) << 1) & 0x00000002)
#define AM_REG_IOMSTR_CFG_SPOL_CLK_BASE_0            0x00000000
#define AM_REG_IOMSTR_CFG_SPOL_CLK_BASE_1            0x00000002

// This bit selects the I/O interface.
#define AM_REG_IOMSTR_CFG_IFCSEL_S                   0
#define AM_REG_IOMSTR_CFG_IFCSEL_M                   0x00000001
#define AM_REG_IOMSTR_CFG_IFCSEL(n)                  (((uint32_t)(n) << 0) & 0x00000001)
#define AM_REG_IOMSTR_CFG_IFCSEL_I2C                 0x00000000
#define AM_REG_IOMSTR_CFG_IFCSEL_SPI                 0x00000001

#endif // AM_REG_IOMSTR_H
