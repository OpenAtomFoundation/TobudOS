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
 * @file     am_reg_ctimer.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Register macros for the CTIMER module
 ******************************************************************************/
#ifndef AM_REG_CTIMER_H
#define AM_REG_CTIMER_H

//*****************************************************************************
//
// Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_CTIMER_NUM_MODULES                    1
#define AM_REG_CTIMERn(n) \
    (REG_CTIMER_BASEADDR + 0x00000000 * n)

//*****************************************************************************
//
// Register offsets.
//
//*****************************************************************************
#define AM_REG_CTIMER_TMR0_O                         0x00000000
#define AM_REG_CTIMER_CMPRA0_O                       0x00000004
#define AM_REG_CTIMER_CMPRB0_O                       0x00000008
#define AM_REG_CTIMER_CTRL0_O                        0x0000000C
#define AM_REG_CTIMER_TMR1_O                         0x00000010
#define AM_REG_CTIMER_CMPRA1_O                       0x00000014
#define AM_REG_CTIMER_CMPRB1_O                       0x00000018
#define AM_REG_CTIMER_CTRL1_O                        0x0000001C
#define AM_REG_CTIMER_TMR2_O                         0x00000020
#define AM_REG_CTIMER_CMPRA2_O                       0x00000024
#define AM_REG_CTIMER_CMPRB2_O                       0x00000028
#define AM_REG_CTIMER_CTRL2_O                        0x0000002C
#define AM_REG_CTIMER_TMR3_O                         0x00000030
#define AM_REG_CTIMER_CMPRA3_O                       0x00000034
#define AM_REG_CTIMER_CMPRB3_O                       0x00000038
#define AM_REG_CTIMER_CTRL3_O                        0x0000003C
#define AM_REG_CTIMER_INTEN_O                        0x00000200
#define AM_REG_CTIMER_INTSTAT_O                      0x00000204
#define AM_REG_CTIMER_INTCLR_O                       0x00000208
#define AM_REG_CTIMER_INTSET_O                       0x0000020C

//*****************************************************************************
//
// CTIMER_INTEN - Counter/Timer Interrupts: Enable
//
//*****************************************************************************
// Counter/Timer B3 interrupt.
#define AM_REG_CTIMER_INTEN_CTMRB3INT_S              7
#define AM_REG_CTIMER_INTEN_CTMRB3INT_M              0x00000080
#define AM_REG_CTIMER_INTEN_CTMRB3INT(n)             (((uint32_t)(n) << 7) & 0x00000080)

// Counter/Timer A3 interrupt.
#define AM_REG_CTIMER_INTEN_CTMRA3INT_S              6
#define AM_REG_CTIMER_INTEN_CTMRA3INT_M              0x00000040
#define AM_REG_CTIMER_INTEN_CTMRA3INT(n)             (((uint32_t)(n) << 6) & 0x00000040)

// Counter/Timer B2 interrupt.
#define AM_REG_CTIMER_INTEN_CTMRB2INT_S              5
#define AM_REG_CTIMER_INTEN_CTMRB2INT_M              0x00000020
#define AM_REG_CTIMER_INTEN_CTMRB2INT(n)             (((uint32_t)(n) << 5) & 0x00000020)

// Counter/Timer A2 interrupt.
#define AM_REG_CTIMER_INTEN_CTMRA2INT_S              4
#define AM_REG_CTIMER_INTEN_CTMRA2INT_M              0x00000010
#define AM_REG_CTIMER_INTEN_CTMRA2INT(n)             (((uint32_t)(n) << 4) & 0x00000010)

// Counter/Timer B1 interrupt.
#define AM_REG_CTIMER_INTEN_CTMRB1INT_S              3
#define AM_REG_CTIMER_INTEN_CTMRB1INT_M              0x00000008
#define AM_REG_CTIMER_INTEN_CTMRB1INT(n)             (((uint32_t)(n) << 3) & 0x00000008)

// Counter/Timer A1 interrupt.
#define AM_REG_CTIMER_INTEN_CTMRA1INT_S              2
#define AM_REG_CTIMER_INTEN_CTMRA1INT_M              0x00000004
#define AM_REG_CTIMER_INTEN_CTMRA1INT(n)             (((uint32_t)(n) << 2) & 0x00000004)

// Counter/Timer B0 interrupt.
#define AM_REG_CTIMER_INTEN_CTMRB0INT_S              1
#define AM_REG_CTIMER_INTEN_CTMRB0INT_M              0x00000002
#define AM_REG_CTIMER_INTEN_CTMRB0INT(n)             (((uint32_t)(n) << 1) & 0x00000002)

// Counter/Timer A0 interrupt.
#define AM_REG_CTIMER_INTEN_CTMRA0INT_S              0
#define AM_REG_CTIMER_INTEN_CTMRA0INT_M              0x00000001
#define AM_REG_CTIMER_INTEN_CTMRA0INT(n)             (((uint32_t)(n) << 0) & 0x00000001)

//*****************************************************************************
//
// CTIMER_INTSTAT - Counter/Timer Interrupts: Status
//
//*****************************************************************************
// Counter/Timer B3 interrupt.
#define AM_REG_CTIMER_INTSTAT_CTMRB3INT_S            7
#define AM_REG_CTIMER_INTSTAT_CTMRB3INT_M            0x00000080
#define AM_REG_CTIMER_INTSTAT_CTMRB3INT(n)           (((uint32_t)(n) << 7) & 0x00000080)

// Counter/Timer A3 interrupt.
#define AM_REG_CTIMER_INTSTAT_CTMRA3INT_S            6
#define AM_REG_CTIMER_INTSTAT_CTMRA3INT_M            0x00000040
#define AM_REG_CTIMER_INTSTAT_CTMRA3INT(n)           (((uint32_t)(n) << 6) & 0x00000040)

// Counter/Timer B2 interrupt.
#define AM_REG_CTIMER_INTSTAT_CTMRB2INT_S            5
#define AM_REG_CTIMER_INTSTAT_CTMRB2INT_M            0x00000020
#define AM_REG_CTIMER_INTSTAT_CTMRB2INT(n)           (((uint32_t)(n) << 5) & 0x00000020)

// Counter/Timer A2 interrupt.
#define AM_REG_CTIMER_INTSTAT_CTMRA2INT_S            4
#define AM_REG_CTIMER_INTSTAT_CTMRA2INT_M            0x00000010
#define AM_REG_CTIMER_INTSTAT_CTMRA2INT(n)           (((uint32_t)(n) << 4) & 0x00000010)

// Counter/Timer B1 interrupt.
#define AM_REG_CTIMER_INTSTAT_CTMRB1INT_S            3
#define AM_REG_CTIMER_INTSTAT_CTMRB1INT_M            0x00000008
#define AM_REG_CTIMER_INTSTAT_CTMRB1INT(n)           (((uint32_t)(n) << 3) & 0x00000008)

// Counter/Timer A1 interrupt.
#define AM_REG_CTIMER_INTSTAT_CTMRA1INT_S            2
#define AM_REG_CTIMER_INTSTAT_CTMRA1INT_M            0x00000004
#define AM_REG_CTIMER_INTSTAT_CTMRA1INT(n)           (((uint32_t)(n) << 2) & 0x00000004)

// Counter/Timer B0 interrupt.
#define AM_REG_CTIMER_INTSTAT_CTMRB0INT_S            1
#define AM_REG_CTIMER_INTSTAT_CTMRB0INT_M            0x00000002
#define AM_REG_CTIMER_INTSTAT_CTMRB0INT(n)           (((uint32_t)(n) << 1) & 0x00000002)

// Counter/Timer A0 interrupt.
#define AM_REG_CTIMER_INTSTAT_CTMRA0INT_S            0
#define AM_REG_CTIMER_INTSTAT_CTMRA0INT_M            0x00000001
#define AM_REG_CTIMER_INTSTAT_CTMRA0INT(n)           (((uint32_t)(n) << 0) & 0x00000001)

//*****************************************************************************
//
// CTIMER_INTCLR - Counter/Timer Interrupts: Clear
//
//*****************************************************************************
// Counter/Timer B3 interrupt.
#define AM_REG_CTIMER_INTCLR_CTMRB3INT_S             7
#define AM_REG_CTIMER_INTCLR_CTMRB3INT_M             0x00000080
#define AM_REG_CTIMER_INTCLR_CTMRB3INT(n)            (((uint32_t)(n) << 7) & 0x00000080)

// Counter/Timer A3 interrupt.
#define AM_REG_CTIMER_INTCLR_CTMRA3INT_S             6
#define AM_REG_CTIMER_INTCLR_CTMRA3INT_M             0x00000040
#define AM_REG_CTIMER_INTCLR_CTMRA3INT(n)            (((uint32_t)(n) << 6) & 0x00000040)

// Counter/Timer B2 interrupt.
#define AM_REG_CTIMER_INTCLR_CTMRB2INT_S             5
#define AM_REG_CTIMER_INTCLR_CTMRB2INT_M             0x00000020
#define AM_REG_CTIMER_INTCLR_CTMRB2INT(n)            (((uint32_t)(n) << 5) & 0x00000020)

// Counter/Timer A2 interrupt.
#define AM_REG_CTIMER_INTCLR_CTMRA2INT_S             4
#define AM_REG_CTIMER_INTCLR_CTMRA2INT_M             0x00000010
#define AM_REG_CTIMER_INTCLR_CTMRA2INT(n)            (((uint32_t)(n) << 4) & 0x00000010)

// Counter/Timer B1 interrupt.
#define AM_REG_CTIMER_INTCLR_CTMRB1INT_S             3
#define AM_REG_CTIMER_INTCLR_CTMRB1INT_M             0x00000008
#define AM_REG_CTIMER_INTCLR_CTMRB1INT(n)            (((uint32_t)(n) << 3) & 0x00000008)

// Counter/Timer A1 interrupt.
#define AM_REG_CTIMER_INTCLR_CTMRA1INT_S             2
#define AM_REG_CTIMER_INTCLR_CTMRA1INT_M             0x00000004
#define AM_REG_CTIMER_INTCLR_CTMRA1INT(n)            (((uint32_t)(n) << 2) & 0x00000004)

// Counter/Timer B0 interrupt.
#define AM_REG_CTIMER_INTCLR_CTMRB0INT_S             1
#define AM_REG_CTIMER_INTCLR_CTMRB0INT_M             0x00000002
#define AM_REG_CTIMER_INTCLR_CTMRB0INT(n)            (((uint32_t)(n) << 1) & 0x00000002)

// Counter/Timer A0 interrupt.
#define AM_REG_CTIMER_INTCLR_CTMRA0INT_S             0
#define AM_REG_CTIMER_INTCLR_CTMRA0INT_M             0x00000001
#define AM_REG_CTIMER_INTCLR_CTMRA0INT(n)            (((uint32_t)(n) << 0) & 0x00000001)

//*****************************************************************************
//
// CTIMER_INTSET - Counter/Timer Interrupts: Set
//
//*****************************************************************************
// Counter/Timer B3 interrupt.
#define AM_REG_CTIMER_INTSET_CTMRB3INT_S             7
#define AM_REG_CTIMER_INTSET_CTMRB3INT_M             0x00000080
#define AM_REG_CTIMER_INTSET_CTMRB3INT(n)            (((uint32_t)(n) << 7) & 0x00000080)

// Counter/Timer A3 interrupt.
#define AM_REG_CTIMER_INTSET_CTMRA3INT_S             6
#define AM_REG_CTIMER_INTSET_CTMRA3INT_M             0x00000040
#define AM_REG_CTIMER_INTSET_CTMRA3INT(n)            (((uint32_t)(n) << 6) & 0x00000040)

// Counter/Timer B2 interrupt.
#define AM_REG_CTIMER_INTSET_CTMRB2INT_S             5
#define AM_REG_CTIMER_INTSET_CTMRB2INT_M             0x00000020
#define AM_REG_CTIMER_INTSET_CTMRB2INT(n)            (((uint32_t)(n) << 5) & 0x00000020)

// Counter/Timer A2 interrupt.
#define AM_REG_CTIMER_INTSET_CTMRA2INT_S             4
#define AM_REG_CTIMER_INTSET_CTMRA2INT_M             0x00000010
#define AM_REG_CTIMER_INTSET_CTMRA2INT(n)            (((uint32_t)(n) << 4) & 0x00000010)

// Counter/Timer B1 interrupt.
#define AM_REG_CTIMER_INTSET_CTMRB1INT_S             3
#define AM_REG_CTIMER_INTSET_CTMRB1INT_M             0x00000008
#define AM_REG_CTIMER_INTSET_CTMRB1INT(n)            (((uint32_t)(n) << 3) & 0x00000008)

// Counter/Timer A1 interrupt.
#define AM_REG_CTIMER_INTSET_CTMRA1INT_S             2
#define AM_REG_CTIMER_INTSET_CTMRA1INT_M             0x00000004
#define AM_REG_CTIMER_INTSET_CTMRA1INT(n)            (((uint32_t)(n) << 2) & 0x00000004)

// Counter/Timer B0 interrupt.
#define AM_REG_CTIMER_INTSET_CTMRB0INT_S             1
#define AM_REG_CTIMER_INTSET_CTMRB0INT_M             0x00000002
#define AM_REG_CTIMER_INTSET_CTMRB0INT(n)            (((uint32_t)(n) << 1) & 0x00000002)

// Counter/Timer A0 interrupt.
#define AM_REG_CTIMER_INTSET_CTMRA0INT_S             0
#define AM_REG_CTIMER_INTSET_CTMRA0INT_M             0x00000001
#define AM_REG_CTIMER_INTSET_CTMRA0INT(n)            (((uint32_t)(n) << 0) & 0x00000001)

//*****************************************************************************
//
// CTIMER_TMR0 - Counter/Timer Register
//
//*****************************************************************************
// Counter/Timer B0.
#define AM_REG_CTIMER_TMR0_CTTMRB0_S                 16
#define AM_REG_CTIMER_TMR0_CTTMRB0_M                 0xFFFF0000
#define AM_REG_CTIMER_TMR0_CTTMRB0(n)                (((uint32_t)(n) << 16) & 0xFFFF0000)

// Counter/Timer A0.
#define AM_REG_CTIMER_TMR0_CTTMRA0_S                 0
#define AM_REG_CTIMER_TMR0_CTTMRA0_M                 0x0000FFFF
#define AM_REG_CTIMER_TMR0_CTTMRA0(n)                (((uint32_t)(n) << 0) & 0x0000FFFF)

//*****************************************************************************
//
// CTIMER_CMPRA0 - Counter/Timer A0 Compare Registers
//
//*****************************************************************************
// Counter/Timer A0 Compare Register 1. Holds the upper limit for timer half A.
#define AM_REG_CTIMER_CMPRA0_CMPR1A0_S               16
#define AM_REG_CTIMER_CMPRA0_CMPR1A0_M               0xFFFF0000
#define AM_REG_CTIMER_CMPRA0_CMPR1A0(n)              (((uint32_t)(n) << 16) & 0xFFFF0000)

// Counter/Timer A0 Compare Register 0. Holds the lower limit for timer half A.
#define AM_REG_CTIMER_CMPRA0_CMPR0A0_S               0
#define AM_REG_CTIMER_CMPRA0_CMPR0A0_M               0x0000FFFF
#define AM_REG_CTIMER_CMPRA0_CMPR0A0(n)              (((uint32_t)(n) << 0) & 0x0000FFFF)

//*****************************************************************************
//
// CTIMER_CMPRB0 - Counter/Timer B0 Compare Registers
//
//*****************************************************************************
// Counter/Timer B0 Compare Register 1. Holds the upper limit for timer half B.
#define AM_REG_CTIMER_CMPRB0_CMPR1B0_S               16
#define AM_REG_CTIMER_CMPRB0_CMPR1B0_M               0xFFFF0000
#define AM_REG_CTIMER_CMPRB0_CMPR1B0(n)              (((uint32_t)(n) << 16) & 0xFFFF0000)

// Counter/Timer B0 Compare Register 0. Holds the lower limit for timer half B.
#define AM_REG_CTIMER_CMPRB0_CMPR0B0_S               0
#define AM_REG_CTIMER_CMPRB0_CMPR0B0_M               0x0000FFFF
#define AM_REG_CTIMER_CMPRB0_CMPR0B0(n)              (((uint32_t)(n) << 0) & 0x0000FFFF)

//*****************************************************************************
//
// CTIMER_CTRL0 - Counter/Timer Control
//
//*****************************************************************************
// Counter/Timer A0/B0 Link bit.
#define AM_REG_CTIMER_CTRL0_CTLINK0_S                31
#define AM_REG_CTIMER_CTRL0_CTLINK0_M                0x80000000
#define AM_REG_CTIMER_CTRL0_CTLINK0(n)               (((uint32_t)(n) << 31) & 0x80000000)
#define AM_REG_CTIMER_CTRL0_CTLINK0_TWO_16BIT_TIMERS 0x00000000
#define AM_REG_CTIMER_CTRL0_CTLINK0_32BIT_TIMER      0x80000000

// Counter/Timer B0 output polarity.
#define AM_REG_CTIMER_CTRL0_TMRB0POL_S               28
#define AM_REG_CTIMER_CTRL0_TMRB0POL_M               0x10000000
#define AM_REG_CTIMER_CTRL0_TMRB0POL(n)              (((uint32_t)(n) << 28) & 0x10000000)
#define AM_REG_CTIMER_CTRL0_TMRB0POL_NORMAL          0x00000000
#define AM_REG_CTIMER_CTRL0_TMRB0POL_INVERTED        0x10000000

// Counter/Timer B0 Clear bit.
#define AM_REG_CTIMER_CTRL0_TMRB0CLR_S               27
#define AM_REG_CTIMER_CTRL0_TMRB0CLR_M               0x08000000
#define AM_REG_CTIMER_CTRL0_TMRB0CLR(n)              (((uint32_t)(n) << 27) & 0x08000000)
#define AM_REG_CTIMER_CTRL0_TMRB0CLR_RUN             0x00000000
#define AM_REG_CTIMER_CTRL0_TMRB0CLR_CLEAR           0x08000000

// Counter/Timer B0 Output Enable bit.
#define AM_REG_CTIMER_CTRL0_TMRB0PE_S                26
#define AM_REG_CTIMER_CTRL0_TMRB0PE_M                0x04000000
#define AM_REG_CTIMER_CTRL0_TMRB0PE(n)               (((uint32_t)(n) << 26) & 0x04000000)
#define AM_REG_CTIMER_CTRL0_TMRB0PE_DIS              0x00000000
#define AM_REG_CTIMER_CTRL0_TMRB0PE_EN               0x04000000

// Counter/Timer B0 Interrupt Enable bit.
#define AM_REG_CTIMER_CTRL0_TMRB0IE_S                25
#define AM_REG_CTIMER_CTRL0_TMRB0IE_M                0x02000000
#define AM_REG_CTIMER_CTRL0_TMRB0IE(n)               (((uint32_t)(n) << 25) & 0x02000000)
#define AM_REG_CTIMER_CTRL0_TMRB0IE_DIS              0x00000000
#define AM_REG_CTIMER_CTRL0_TMRB0IE_EN               0x02000000

// Counter/Timer B0 Function Select.
#define AM_REG_CTIMER_CTRL0_TMRB0FN_S                22
#define AM_REG_CTIMER_CTRL0_TMRB0FN_M                0x01C00000
#define AM_REG_CTIMER_CTRL0_TMRB0FN(n)               (((uint32_t)(n) << 22) & 0x01C00000)
#define AM_REG_CTIMER_CTRL0_TMRB0FN_SINGLECOUNT      0x00000000
#define AM_REG_CTIMER_CTRL0_TMRB0FN_REPEATEDCOUNT    0x00400000
#define AM_REG_CTIMER_CTRL0_TMRB0FN_PULSE_ONCE       0x00800000
#define AM_REG_CTIMER_CTRL0_TMRB0FN_PULSE_CONT       0x00C00000
#define AM_REG_CTIMER_CTRL0_TMRB0FN_CONTINUOUS       0x01000000

// Counter/Timer B0 Clock Select.
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_S               17
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_M               0x003E0000
#define AM_REG_CTIMER_CTRL0_TMRB0CLK(n)              (((uint32_t)(n) << 17) & 0x003E0000)
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_TMRPIN          0x00000000
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_HFRC            0x00020000
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_HFRC_DIV8       0x00040000
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_HFRC_DIV128     0x00060000
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_HFRC_DIV512     0x00080000
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_HFRC_DIV2K      0x000A0000
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_XT              0x000C0000
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_XT_DIV2         0x000E0000
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_XT_DIV16        0x00100000
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_XT_DIV256       0x00120000
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_LFRC_DIV2       0x00140000
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_LFRC_DIV32      0x00160000
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_LFRC_DIV1K      0x00180000
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_LFRC            0x001A0000
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_RTC_100HZ       0x001C0000
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_HCLK            0x001E0000
#define AM_REG_CTIMER_CTRL0_TMRB0CLK_BUCKB           0x00200000

// Counter/Timer B0 Enable bit.
#define AM_REG_CTIMER_CTRL0_TMRB0EN_S                16
#define AM_REG_CTIMER_CTRL0_TMRB0EN_M                0x00010000
#define AM_REG_CTIMER_CTRL0_TMRB0EN(n)               (((uint32_t)(n) << 16) & 0x00010000)
#define AM_REG_CTIMER_CTRL0_TMRB0EN_DIS              0x00000000
#define AM_REG_CTIMER_CTRL0_TMRB0EN_EN               0x00010000

// Counter/Timer A0 output polarity.
#define AM_REG_CTIMER_CTRL0_TMRA0POL_S               12
#define AM_REG_CTIMER_CTRL0_TMRA0POL_M               0x00001000
#define AM_REG_CTIMER_CTRL0_TMRA0POL(n)              (((uint32_t)(n) << 12) & 0x00001000)
#define AM_REG_CTIMER_CTRL0_TMRA0POL_NORMAL          0x00000000
#define AM_REG_CTIMER_CTRL0_TMRA0POL_INVERTED        0x00001000

// Counter/Timer A0 Clear bit.
#define AM_REG_CTIMER_CTRL0_TMRA0CLR_S               11
#define AM_REG_CTIMER_CTRL0_TMRA0CLR_M               0x00000800
#define AM_REG_CTIMER_CTRL0_TMRA0CLR(n)              (((uint32_t)(n) << 11) & 0x00000800)
#define AM_REG_CTIMER_CTRL0_TMRA0CLR_RUN             0x00000000
#define AM_REG_CTIMER_CTRL0_TMRA0CLR_CLEAR           0x00000800

// Counter/Timer A0 Output Enable bit.
#define AM_REG_CTIMER_CTRL0_TMRA0PE_S                10
#define AM_REG_CTIMER_CTRL0_TMRA0PE_M                0x00000400
#define AM_REG_CTIMER_CTRL0_TMRA0PE(n)               (((uint32_t)(n) << 10) & 0x00000400)
#define AM_REG_CTIMER_CTRL0_TMRA0PE_DIS              0x00000000
#define AM_REG_CTIMER_CTRL0_TMRA0PE_EN               0x00000400

// Counter/Timer A0 Interrupt Enable bit.
#define AM_REG_CTIMER_CTRL0_TMRA0IE_S                9
#define AM_REG_CTIMER_CTRL0_TMRA0IE_M                0x00000200
#define AM_REG_CTIMER_CTRL0_TMRA0IE(n)               (((uint32_t)(n) << 9) & 0x00000200)
#define AM_REG_CTIMER_CTRL0_TMRA0IE_DIS              0x00000000
#define AM_REG_CTIMER_CTRL0_TMRA0IE_EN               0x00000200

// Counter/Timer A0 Function Select.
#define AM_REG_CTIMER_CTRL0_TMRA0FN_S                6
#define AM_REG_CTIMER_CTRL0_TMRA0FN_M                0x000001C0
#define AM_REG_CTIMER_CTRL0_TMRA0FN(n)               (((uint32_t)(n) << 6) & 0x000001C0)
#define AM_REG_CTIMER_CTRL0_TMRA0FN_SINGLECOUNT      0x00000000
#define AM_REG_CTIMER_CTRL0_TMRA0FN_REPEATEDCOUNT    0x00000040
#define AM_REG_CTIMER_CTRL0_TMRA0FN_PULSE_ONCE       0x00000080
#define AM_REG_CTIMER_CTRL0_TMRA0FN_PULSE_CONT       0x000000C0
#define AM_REG_CTIMER_CTRL0_TMRA0FN_CONTINUOUS       0x00000100

// Counter/Timer A0 Clock Select.
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_S               1
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_M               0x0000003E
#define AM_REG_CTIMER_CTRL0_TMRA0CLK(n)              (((uint32_t)(n) << 1) & 0x0000003E)
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_TMRPIN          0x00000000
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_HFRC            0x00000002
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_HFRC_DIV8       0x00000004
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_HFRC_DIV128     0x00000006
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_HFRC_DIV512     0x00000008
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_HFRC_DIV2K      0x0000000A
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_XT              0x0000000C
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_XT_DIV2         0x0000000E
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_XT_DIV16        0x00000010
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_XT_DIV256       0x00000012
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_LFRC_DIV2       0x00000014
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_LFRC_DIV32      0x00000016
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_LFRC_DIV1K      0x00000018
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_LFRC            0x0000001A
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_RTC_100HZ       0x0000001C
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_HCLK            0x0000001E
#define AM_REG_CTIMER_CTRL0_TMRA0CLK_BUCKA           0x00000020

// Counter/Timer A0 Enable bit.
#define AM_REG_CTIMER_CTRL0_TMRA0EN_S                0
#define AM_REG_CTIMER_CTRL0_TMRA0EN_M                0x00000001
#define AM_REG_CTIMER_CTRL0_TMRA0EN(n)               (((uint32_t)(n) << 0) & 0x00000001)
#define AM_REG_CTIMER_CTRL0_TMRA0EN_DIS              0x00000000
#define AM_REG_CTIMER_CTRL0_TMRA0EN_EN               0x00000001

//*****************************************************************************
//
// CTIMER_TMR1 - Counter/Timer Register
//
//*****************************************************************************
// Counter/Timer B1.
#define AM_REG_CTIMER_TMR1_CTTMRB1_S                 16
#define AM_REG_CTIMER_TMR1_CTTMRB1_M                 0xFFFF0000
#define AM_REG_CTIMER_TMR1_CTTMRB1(n)                (((uint32_t)(n) << 16) & 0xFFFF0000)

// Counter/Timer A1.
#define AM_REG_CTIMER_TMR1_CTTMRA1_S                 0
#define AM_REG_CTIMER_TMR1_CTTMRA1_M                 0x0000FFFF
#define AM_REG_CTIMER_TMR1_CTTMRA1(n)                (((uint32_t)(n) << 0) & 0x0000FFFF)

//*****************************************************************************
//
// CTIMER_CMPRA1 - Counter/Timer A1 Compare Registers
//
//*****************************************************************************
// Counter/Timer A1 Compare Register 1.
#define AM_REG_CTIMER_CMPRA1_CMPR1A1_S               16
#define AM_REG_CTIMER_CMPRA1_CMPR1A1_M               0xFFFF0000
#define AM_REG_CTIMER_CMPRA1_CMPR1A1(n)              (((uint32_t)(n) << 16) & 0xFFFF0000)

// Counter/Timer A1 Compare Register 0.
#define AM_REG_CTIMER_CMPRA1_CMPR0A1_S               0
#define AM_REG_CTIMER_CMPRA1_CMPR0A1_M               0x0000FFFF
#define AM_REG_CTIMER_CMPRA1_CMPR0A1(n)              (((uint32_t)(n) << 0) & 0x0000FFFF)

//*****************************************************************************
//
// CTIMER_CMPRB1 - Counter/Timer B1 Compare Registers
//
//*****************************************************************************
// Counter/Timer B1 Compare Register 1.
#define AM_REG_CTIMER_CMPRB1_CMPR1B1_S               16
#define AM_REG_CTIMER_CMPRB1_CMPR1B1_M               0xFFFF0000
#define AM_REG_CTIMER_CMPRB1_CMPR1B1(n)              (((uint32_t)(n) << 16) & 0xFFFF0000)

// Counter/Timer B1 Compare Register 0.
#define AM_REG_CTIMER_CMPRB1_CMPR0B1_S               0
#define AM_REG_CTIMER_CMPRB1_CMPR0B1_M               0x0000FFFF
#define AM_REG_CTIMER_CMPRB1_CMPR0B1(n)              (((uint32_t)(n) << 0) & 0x0000FFFF)

//*****************************************************************************
//
// CTIMER_CTRL1 - Counter/Timer Control
//
//*****************************************************************************
// Counter/Timer A1/B1 Link bit.
#define AM_REG_CTIMER_CTRL1_CTLINK1_S                31
#define AM_REG_CTIMER_CTRL1_CTLINK1_M                0x80000000
#define AM_REG_CTIMER_CTRL1_CTLINK1(n)               (((uint32_t)(n) << 31) & 0x80000000)
#define AM_REG_CTIMER_CTRL1_CTLINK1_TWO_16BIT_TIMERS 0x00000000
#define AM_REG_CTIMER_CTRL1_CTLINK1_32BIT_TIMER      0x80000000

// Counter/Timer B1 output polarity.
#define AM_REG_CTIMER_CTRL1_TMRB1POL_S               28
#define AM_REG_CTIMER_CTRL1_TMRB1POL_M               0x10000000
#define AM_REG_CTIMER_CTRL1_TMRB1POL(n)              (((uint32_t)(n) << 28) & 0x10000000)
#define AM_REG_CTIMER_CTRL1_TMRB1POL_NORMAL          0x00000000
#define AM_REG_CTIMER_CTRL1_TMRB1POL_INVERTED        0x10000000

// Counter/Timer B1 Clear bit.
#define AM_REG_CTIMER_CTRL1_TMRB1CLR_S               27
#define AM_REG_CTIMER_CTRL1_TMRB1CLR_M               0x08000000
#define AM_REG_CTIMER_CTRL1_TMRB1CLR(n)              (((uint32_t)(n) << 27) & 0x08000000)
#define AM_REG_CTIMER_CTRL1_TMRB1CLR_RUN             0x00000000
#define AM_REG_CTIMER_CTRL1_TMRB1CLR_CLEAR           0x08000000

// Counter/Timer B1 Output Enable bit.
#define AM_REG_CTIMER_CTRL1_TMRB1PE_S                26
#define AM_REG_CTIMER_CTRL1_TMRB1PE_M                0x04000000
#define AM_REG_CTIMER_CTRL1_TMRB1PE(n)               (((uint32_t)(n) << 26) & 0x04000000)
#define AM_REG_CTIMER_CTRL1_TMRB1PE_DIS              0x00000000
#define AM_REG_CTIMER_CTRL1_TMRB1PE_EN               0x04000000

// Counter/Timer B1 Interrupt Enable bit.
#define AM_REG_CTIMER_CTRL1_TMRB1IE_S                25
#define AM_REG_CTIMER_CTRL1_TMRB1IE_M                0x02000000
#define AM_REG_CTIMER_CTRL1_TMRB1IE(n)               (((uint32_t)(n) << 25) & 0x02000000)
#define AM_REG_CTIMER_CTRL1_TMRB1IE_DIS              0x00000000
#define AM_REG_CTIMER_CTRL1_TMRB1IE_EN               0x02000000

// Counter/Timer B1 Function Select.
#define AM_REG_CTIMER_CTRL1_TMRB1FN_S                22
#define AM_REG_CTIMER_CTRL1_TMRB1FN_M                0x01C00000
#define AM_REG_CTIMER_CTRL1_TMRB1FN(n)               (((uint32_t)(n) << 22) & 0x01C00000)
#define AM_REG_CTIMER_CTRL1_TMRB1FN_SINGLECOUNT      0x00000000
#define AM_REG_CTIMER_CTRL1_TMRB1FN_REPEATEDCOUNT    0x00400000
#define AM_REG_CTIMER_CTRL1_TMRB1FN_PULSE_ONCE       0x00800000
#define AM_REG_CTIMER_CTRL1_TMRB1FN_PULSE_CONT       0x00C00000
#define AM_REG_CTIMER_CTRL1_TMRB1FN_CONTINUOUS       0x01000000

// Counter/Timer B1 Clock Select.
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_S               17
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_M               0x003E0000
#define AM_REG_CTIMER_CTRL1_TMRB1CLK(n)              (((uint32_t)(n) << 17) & 0x003E0000)
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_TMRPIN          0x00000000
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_HFRC            0x00020000
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_HFRC_DIV8       0x00040000
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_HFRC_DIV128     0x00060000
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_HFRC_DIV512     0x00080000
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_HFRC_DIV2K      0x000A0000
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_XT              0x000C0000
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_XT_DIV2         0x000E0000
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_XT_DIV16        0x00100000
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_XT_DIV256       0x00120000
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_LFRC_DIV2       0x00140000
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_LFRC_DIV32      0x00160000
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_LFRC_DIV1K      0x00180000
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_LFRC            0x001A0000
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_RTC_100HZ       0x001C0000
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_HCLK            0x001E0000
#define AM_REG_CTIMER_CTRL1_TMRB1CLK_BUCKB           0x00200000

// Counter/Timer B1 Enable bit.
#define AM_REG_CTIMER_CTRL1_TMRB1EN_S                16
#define AM_REG_CTIMER_CTRL1_TMRB1EN_M                0x00010000
#define AM_REG_CTIMER_CTRL1_TMRB1EN(n)               (((uint32_t)(n) << 16) & 0x00010000)
#define AM_REG_CTIMER_CTRL1_TMRB1EN_DIS              0x00000000
#define AM_REG_CTIMER_CTRL1_TMRB1EN_EN               0x00010000

// Counter/Timer A1 output polarity.
#define AM_REG_CTIMER_CTRL1_TMRA1POL_S               12
#define AM_REG_CTIMER_CTRL1_TMRA1POL_M               0x00001000
#define AM_REG_CTIMER_CTRL1_TMRA1POL(n)              (((uint32_t)(n) << 12) & 0x00001000)
#define AM_REG_CTIMER_CTRL1_TMRA1POL_NORMAL          0x00000000
#define AM_REG_CTIMER_CTRL1_TMRA1POL_INVERTED        0x00001000

// Counter/Timer A1 Clear bit.
#define AM_REG_CTIMER_CTRL1_TMRA1CLR_S               11
#define AM_REG_CTIMER_CTRL1_TMRA1CLR_M               0x00000800
#define AM_REG_CTIMER_CTRL1_TMRA1CLR(n)              (((uint32_t)(n) << 11) & 0x00000800)
#define AM_REG_CTIMER_CTRL1_TMRA1CLR_RUN             0x00000000
#define AM_REG_CTIMER_CTRL1_TMRA1CLR_CLEAR           0x00000800

// Counter/Timer A1 Output Enable bit.
#define AM_REG_CTIMER_CTRL1_TMRA1PE_S                10
#define AM_REG_CTIMER_CTRL1_TMRA1PE_M                0x00000400
#define AM_REG_CTIMER_CTRL1_TMRA1PE(n)               (((uint32_t)(n) << 10) & 0x00000400)
#define AM_REG_CTIMER_CTRL1_TMRA1PE_DIS              0x00000000
#define AM_REG_CTIMER_CTRL1_TMRA1PE_EN               0x00000400

// Counter/Timer A1 Interrupt Enable bit.
#define AM_REG_CTIMER_CTRL1_TMRA1IE_S                9
#define AM_REG_CTIMER_CTRL1_TMRA1IE_M                0x00000200
#define AM_REG_CTIMER_CTRL1_TMRA1IE(n)               (((uint32_t)(n) << 9) & 0x00000200)
#define AM_REG_CTIMER_CTRL1_TMRA1IE_DIS              0x00000000
#define AM_REG_CTIMER_CTRL1_TMRA1IE_EN               0x00000200

// Counter/Timer A1 Function Select.
#define AM_REG_CTIMER_CTRL1_TMRA1FN_S                6
#define AM_REG_CTIMER_CTRL1_TMRA1FN_M                0x000001C0
#define AM_REG_CTIMER_CTRL1_TMRA1FN(n)               (((uint32_t)(n) << 6) & 0x000001C0)
#define AM_REG_CTIMER_CTRL1_TMRA1FN_SINGLECOUNT      0x00000000
#define AM_REG_CTIMER_CTRL1_TMRA1FN_REPEATEDCOUNT    0x00000040
#define AM_REG_CTIMER_CTRL1_TMRA1FN_PULSE_ONCE       0x00000080
#define AM_REG_CTIMER_CTRL1_TMRA1FN_PULSE_CONT       0x000000C0
#define AM_REG_CTIMER_CTRL1_TMRA1FN_CONTINUOUS       0x00000100

// Counter/Timer A1 Clock Select.
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_S               1
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_M               0x0000003E
#define AM_REG_CTIMER_CTRL1_TMRA1CLK(n)              (((uint32_t)(n) << 1) & 0x0000003E)
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_TMRPIN          0x00000000
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_HFRC            0x00000002
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_HFRC_DIV8       0x00000004
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_HFRC_DIV128     0x00000006
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_HFRC_DIV512     0x00000008
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_HFRC_DIV2K      0x0000000A
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_XT              0x0000000C
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_XT_DIV2         0x0000000E
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_XT_DIV16        0x00000010
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_XT_DIV256       0x00000012
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_LFRC_DIV2       0x00000014
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_LFRC_DIV32      0x00000016
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_LFRC_DIV1K      0x00000018
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_LFRC            0x0000001A
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_RTC_100HZ       0x0000001C
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_HCLK            0x0000001E
#define AM_REG_CTIMER_CTRL1_TMRA1CLK_BUCKA           0x00000020

// Counter/Timer A1 Enable bit.
#define AM_REG_CTIMER_CTRL1_TMRA1EN_S                0
#define AM_REG_CTIMER_CTRL1_TMRA1EN_M                0x00000001
#define AM_REG_CTIMER_CTRL1_TMRA1EN(n)               (((uint32_t)(n) << 0) & 0x00000001)
#define AM_REG_CTIMER_CTRL1_TMRA1EN_DIS              0x00000000
#define AM_REG_CTIMER_CTRL1_TMRA1EN_EN               0x00000001

//*****************************************************************************
//
// CTIMER_TMR2 - Counter/Timer Register
//
//*****************************************************************************
// Counter/Timer B2.
#define AM_REG_CTIMER_TMR2_CTTMRB2_S                 16
#define AM_REG_CTIMER_TMR2_CTTMRB2_M                 0xFFFF0000
#define AM_REG_CTIMER_TMR2_CTTMRB2(n)                (((uint32_t)(n) << 16) & 0xFFFF0000)

// Counter/Timer A2.
#define AM_REG_CTIMER_TMR2_CTTMRA2_S                 0
#define AM_REG_CTIMER_TMR2_CTTMRA2_M                 0x0000FFFF
#define AM_REG_CTIMER_TMR2_CTTMRA2(n)                (((uint32_t)(n) << 0) & 0x0000FFFF)

//*****************************************************************************
//
// CTIMER_CMPRA2 - Counter/Timer A2 Compare Registers
//
//*****************************************************************************
// Counter/Timer A2 Compare Register 1.
#define AM_REG_CTIMER_CMPRA2_CMPR1A2_S               16
#define AM_REG_CTIMER_CMPRA2_CMPR1A2_M               0xFFFF0000
#define AM_REG_CTIMER_CMPRA2_CMPR1A2(n)              (((uint32_t)(n) << 16) & 0xFFFF0000)

// Counter/Timer A2 Compare Register 0.
#define AM_REG_CTIMER_CMPRA2_CMPR0A2_S               0
#define AM_REG_CTIMER_CMPRA2_CMPR0A2_M               0x0000FFFF
#define AM_REG_CTIMER_CMPRA2_CMPR0A2(n)              (((uint32_t)(n) << 0) & 0x0000FFFF)

//*****************************************************************************
//
// CTIMER_CMPRB2 - Counter/Timer B2 Compare Registers
//
//*****************************************************************************
// Counter/Timer B2 Compare Register 1.
#define AM_REG_CTIMER_CMPRB2_CMPR1B2_S               16
#define AM_REG_CTIMER_CMPRB2_CMPR1B2_M               0xFFFF0000
#define AM_REG_CTIMER_CMPRB2_CMPR1B2(n)              (((uint32_t)(n) << 16) & 0xFFFF0000)

// Counter/Timer B2 Compare Register 0.
#define AM_REG_CTIMER_CMPRB2_CMPR0B2_S               0
#define AM_REG_CTIMER_CMPRB2_CMPR0B2_M               0x0000FFFF
#define AM_REG_CTIMER_CMPRB2_CMPR0B2(n)              (((uint32_t)(n) << 0) & 0x0000FFFF)

//*****************************************************************************
//
// CTIMER_CTRL2 - Counter/Timer Control
//
//*****************************************************************************
// Counter/Timer A2/B2 Link bit.
#define AM_REG_CTIMER_CTRL2_CTLINK2_S                31
#define AM_REG_CTIMER_CTRL2_CTLINK2_M                0x80000000
#define AM_REG_CTIMER_CTRL2_CTLINK2(n)               (((uint32_t)(n) << 31) & 0x80000000)
#define AM_REG_CTIMER_CTRL2_CTLINK2_TWO_16BIT_TIMERS 0x00000000
#define AM_REG_CTIMER_CTRL2_CTLINK2_32BIT_TIMER      0x80000000

// Counter/Timer B2 output polarity.
#define AM_REG_CTIMER_CTRL2_TMRB2POL_S               28
#define AM_REG_CTIMER_CTRL2_TMRB2POL_M               0x10000000
#define AM_REG_CTIMER_CTRL2_TMRB2POL(n)              (((uint32_t)(n) << 28) & 0x10000000)
#define AM_REG_CTIMER_CTRL2_TMRB2POL_NORMAL          0x00000000
#define AM_REG_CTIMER_CTRL2_TMRB2POL_INVERTED        0x10000000

// Counter/Timer B2 Clear bit.
#define AM_REG_CTIMER_CTRL2_TMRB2CLR_S               27
#define AM_REG_CTIMER_CTRL2_TMRB2CLR_M               0x08000000
#define AM_REG_CTIMER_CTRL2_TMRB2CLR(n)              (((uint32_t)(n) << 27) & 0x08000000)
#define AM_REG_CTIMER_CTRL2_TMRB2CLR_RUN             0x00000000
#define AM_REG_CTIMER_CTRL2_TMRB2CLR_CLEAR           0x08000000

// Counter/Timer B2 Output Enable bit.
#define AM_REG_CTIMER_CTRL2_TMRB2PE_S                26
#define AM_REG_CTIMER_CTRL2_TMRB2PE_M                0x04000000
#define AM_REG_CTIMER_CTRL2_TMRB2PE(n)               (((uint32_t)(n) << 26) & 0x04000000)
#define AM_REG_CTIMER_CTRL2_TMRB2PE_DIS              0x00000000
#define AM_REG_CTIMER_CTRL2_TMRB2PE_EN               0x04000000

// Counter/Timer B2 Interrupt Enable bit.
#define AM_REG_CTIMER_CTRL2_TMRB2IE_S                25
#define AM_REG_CTIMER_CTRL2_TMRB2IE_M                0x02000000
#define AM_REG_CTIMER_CTRL2_TMRB2IE(n)               (((uint32_t)(n) << 25) & 0x02000000)
#define AM_REG_CTIMER_CTRL2_TMRB2IE_DIS              0x00000000
#define AM_REG_CTIMER_CTRL2_TMRB2IE_EN               0x02000000

// Counter/Timer B2 Function Select.
#define AM_REG_CTIMER_CTRL2_TMRB2FN_S                22
#define AM_REG_CTIMER_CTRL2_TMRB2FN_M                0x01C00000
#define AM_REG_CTIMER_CTRL2_TMRB2FN(n)               (((uint32_t)(n) << 22) & 0x01C00000)
#define AM_REG_CTIMER_CTRL2_TMRB2FN_SINGLECOUNT      0x00000000
#define AM_REG_CTIMER_CTRL2_TMRB2FN_REPEATEDCOUNT    0x00400000
#define AM_REG_CTIMER_CTRL2_TMRB2FN_PULSE_ONCE       0x00800000
#define AM_REG_CTIMER_CTRL2_TMRB2FN_PULSE_CONT       0x00C00000
#define AM_REG_CTIMER_CTRL2_TMRB2FN_CONTINUOUS       0x01000000

// Counter/Timer B2 Clock Select.
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_S               17
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_M               0x003E0000
#define AM_REG_CTIMER_CTRL2_TMRB2CLK(n)              (((uint32_t)(n) << 17) & 0x003E0000)
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_TMRPIN          0x00000000
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_HFRC            0x00020000
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_HFRC_DIV8       0x00040000
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_HFRC_DIV128     0x00060000
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_HFRC_DIV512     0x00080000
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_HFRC_DIV2K      0x000A0000
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_XT              0x000C0000
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_XT_DIV2         0x000E0000
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_XT_DIV16        0x00100000
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_XT_DIV256       0x00120000
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_LFRC_DIV2       0x00140000
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_LFRC_DIV32      0x00160000
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_LFRC_DIV1K      0x00180000
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_LFRC            0x001A0000
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_RTC_100HZ       0x001C0000
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_HCLK            0x001E0000
#define AM_REG_CTIMER_CTRL2_TMRB2CLK_BUCKA           0x00200000

// Counter/Timer B2 Enable bit.
#define AM_REG_CTIMER_CTRL2_TMRB2EN_S                16
#define AM_REG_CTIMER_CTRL2_TMRB2EN_M                0x00010000
#define AM_REG_CTIMER_CTRL2_TMRB2EN(n)               (((uint32_t)(n) << 16) & 0x00010000)
#define AM_REG_CTIMER_CTRL2_TMRB2EN_DIS              0x00000000
#define AM_REG_CTIMER_CTRL2_TMRB2EN_EN               0x00010000

// Counter/Timer A2 output polarity.
#define AM_REG_CTIMER_CTRL2_TMRA2POL_S               12
#define AM_REG_CTIMER_CTRL2_TMRA2POL_M               0x00001000
#define AM_REG_CTIMER_CTRL2_TMRA2POL(n)              (((uint32_t)(n) << 12) & 0x00001000)
#define AM_REG_CTIMER_CTRL2_TMRA2POL_NORMAL          0x00000000
#define AM_REG_CTIMER_CTRL2_TMRA2POL_INVERTED        0x00001000

// Counter/Timer A2 Clear bit.
#define AM_REG_CTIMER_CTRL2_TMRA2CLR_S               11
#define AM_REG_CTIMER_CTRL2_TMRA2CLR_M               0x00000800
#define AM_REG_CTIMER_CTRL2_TMRA2CLR(n)              (((uint32_t)(n) << 11) & 0x00000800)
#define AM_REG_CTIMER_CTRL2_TMRA2CLR_RUN             0x00000000
#define AM_REG_CTIMER_CTRL2_TMRA2CLR_CLEAR           0x00000800

// Counter/Timer A2 Output Enable bit.
#define AM_REG_CTIMER_CTRL2_TMRA2PE_S                10
#define AM_REG_CTIMER_CTRL2_TMRA2PE_M                0x00000400
#define AM_REG_CTIMER_CTRL2_TMRA2PE(n)               (((uint32_t)(n) << 10) & 0x00000400)
#define AM_REG_CTIMER_CTRL2_TMRA2PE_DIS              0x00000000
#define AM_REG_CTIMER_CTRL2_TMRA2PE_EN               0x00000400

// Counter/Timer A2 Interrupt Enable bit.
#define AM_REG_CTIMER_CTRL2_TMRA2IE_S                9
#define AM_REG_CTIMER_CTRL2_TMRA2IE_M                0x00000200
#define AM_REG_CTIMER_CTRL2_TMRA2IE(n)               (((uint32_t)(n) << 9) & 0x00000200)
#define AM_REG_CTIMER_CTRL2_TMRA2IE_DIS              0x00000000
#define AM_REG_CTIMER_CTRL2_TMRA2IE_EN               0x00000200

// Counter/Timer A2 Function Select.
#define AM_REG_CTIMER_CTRL2_TMRA2FN_S                6
#define AM_REG_CTIMER_CTRL2_TMRA2FN_M                0x000001C0
#define AM_REG_CTIMER_CTRL2_TMRA2FN(n)               (((uint32_t)(n) << 6) & 0x000001C0)
#define AM_REG_CTIMER_CTRL2_TMRA2FN_SINGLECOUNT      0x00000000
#define AM_REG_CTIMER_CTRL2_TMRA2FN_REPEATEDCOUNT    0x00000040
#define AM_REG_CTIMER_CTRL2_TMRA2FN_PULSE_ONCE       0x00000080
#define AM_REG_CTIMER_CTRL2_TMRA2FN_PULSE_CONT       0x000000C0
#define AM_REG_CTIMER_CTRL2_TMRA2FN_CONTINUOUS       0x00000100

// Counter/Timer A2 Clock Select.
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_S               1
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_M               0x0000003E
#define AM_REG_CTIMER_CTRL2_TMRA2CLK(n)              (((uint32_t)(n) << 1) & 0x0000003E)
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_TMRPIN          0x00000000
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_HFRC            0x00000002
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_HFRC_DIV8       0x00000004
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_HFRC_DIV128     0x00000006
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_HFRC_DIV512     0x00000008
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_HFRC_DIV2K      0x0000000A
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_XT              0x0000000C
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_XT_DIV2         0x0000000E
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_XT_DIV16        0x00000010
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_XT_DIV256       0x00000012
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_LFRC_DIV2       0x00000014
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_LFRC_DIV32      0x00000016
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_LFRC_DIV1K      0x00000018
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_LFRC            0x0000001A
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_RTC_100HZ       0x0000001C
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_HCLK            0x0000001E
#define AM_REG_CTIMER_CTRL2_TMRA2CLK_BUCKB           0x00000020

// Counter/Timer A2 Enable bit.
#define AM_REG_CTIMER_CTRL2_TMRA2EN_S                0
#define AM_REG_CTIMER_CTRL2_TMRA2EN_M                0x00000001
#define AM_REG_CTIMER_CTRL2_TMRA2EN(n)               (((uint32_t)(n) << 0) & 0x00000001)
#define AM_REG_CTIMER_CTRL2_TMRA2EN_DIS              0x00000000
#define AM_REG_CTIMER_CTRL2_TMRA2EN_EN               0x00000001

//*****************************************************************************
//
// CTIMER_TMR3 - Counter/Timer Register
//
//*****************************************************************************
// Counter/Timer B3.
#define AM_REG_CTIMER_TMR3_CTTMRB3_S                 16
#define AM_REG_CTIMER_TMR3_CTTMRB3_M                 0xFFFF0000
#define AM_REG_CTIMER_TMR3_CTTMRB3(n)                (((uint32_t)(n) << 16) & 0xFFFF0000)

// Counter/Timer A3.
#define AM_REG_CTIMER_TMR3_CTTMRA3_S                 0
#define AM_REG_CTIMER_TMR3_CTTMRA3_M                 0x0000FFFF
#define AM_REG_CTIMER_TMR3_CTTMRA3(n)                (((uint32_t)(n) << 0) & 0x0000FFFF)

//*****************************************************************************
//
// CTIMER_CMPRA3 - Counter/Timer A3 Compare Registers
//
//*****************************************************************************
// Counter/Timer A3 Compare Register 1.
#define AM_REG_CTIMER_CMPRA3_CMPR1A3_S               16
#define AM_REG_CTIMER_CMPRA3_CMPR1A3_M               0xFFFF0000
#define AM_REG_CTIMER_CMPRA3_CMPR1A3(n)              (((uint32_t)(n) << 16) & 0xFFFF0000)

// Counter/Timer A3 Compare Register 0.
#define AM_REG_CTIMER_CMPRA3_CMPR0A3_S               0
#define AM_REG_CTIMER_CMPRA3_CMPR0A3_M               0x0000FFFF
#define AM_REG_CTIMER_CMPRA3_CMPR0A3(n)              (((uint32_t)(n) << 0) & 0x0000FFFF)

//*****************************************************************************
//
// CTIMER_CMPRB3 - Counter/Timer B3 Compare Registers
//
//*****************************************************************************
// Counter/Timer B3 Compare Register 1.
#define AM_REG_CTIMER_CMPRB3_CMPR1B3_S               16
#define AM_REG_CTIMER_CMPRB3_CMPR1B3_M               0xFFFF0000
#define AM_REG_CTIMER_CMPRB3_CMPR1B3(n)              (((uint32_t)(n) << 16) & 0xFFFF0000)

// Counter/Timer B3 Compare Register 0.
#define AM_REG_CTIMER_CMPRB3_CMPR0B3_S               0
#define AM_REG_CTIMER_CMPRB3_CMPR0B3_M               0x0000FFFF
#define AM_REG_CTIMER_CMPRB3_CMPR0B3(n)              (((uint32_t)(n) << 0) & 0x0000FFFF)

//*****************************************************************************
//
// CTIMER_CTRL3 - Counter/Timer Control
//
//*****************************************************************************
// Counter/Timer A/B Link bit.
#define AM_REG_CTIMER_CTRL3_CTLINK3_S                31
#define AM_REG_CTIMER_CTRL3_CTLINK3_M                0x80000000
#define AM_REG_CTIMER_CTRL3_CTLINK3(n)               (((uint32_t)(n) << 31) & 0x80000000)
#define AM_REG_CTIMER_CTRL3_CTLINK3_TWO_16BIT_TIMERS 0x00000000
#define AM_REG_CTIMER_CTRL3_CTLINK3_32BIT_TIMER      0x80000000

// Counter/Timer B3 output polarity.
#define AM_REG_CTIMER_CTRL3_TMRB3POL_S               28
#define AM_REG_CTIMER_CTRL3_TMRB3POL_M               0x10000000
#define AM_REG_CTIMER_CTRL3_TMRB3POL(n)              (((uint32_t)(n) << 28) & 0x10000000)
#define AM_REG_CTIMER_CTRL3_TMRB3POL_NORMAL          0x00000000
#define AM_REG_CTIMER_CTRL3_TMRB3POL_INVERTED        0x10000000

// Counter/Timer B3 Clear bit.
#define AM_REG_CTIMER_CTRL3_TMRB3CLR_S               27
#define AM_REG_CTIMER_CTRL3_TMRB3CLR_M               0x08000000
#define AM_REG_CTIMER_CTRL3_TMRB3CLR(n)              (((uint32_t)(n) << 27) & 0x08000000)
#define AM_REG_CTIMER_CTRL3_TMRB3CLR_RUN             0x00000000
#define AM_REG_CTIMER_CTRL3_TMRB3CLR_CLEAR           0x08000000

// Counter/Timer B3 Output Enable bit.
#define AM_REG_CTIMER_CTRL3_TMRB3PE_S                26
#define AM_REG_CTIMER_CTRL3_TMRB3PE_M                0x04000000
#define AM_REG_CTIMER_CTRL3_TMRB3PE(n)               (((uint32_t)(n) << 26) & 0x04000000)
#define AM_REG_CTIMER_CTRL3_TMRB3PE_DIS              0x00000000
#define AM_REG_CTIMER_CTRL3_TMRB3PE_EN               0x04000000

// Counter/Timer B3 Interrupt Enable bit.
#define AM_REG_CTIMER_CTRL3_TMRB3IE_S                25
#define AM_REG_CTIMER_CTRL3_TMRB3IE_M                0x02000000
#define AM_REG_CTIMER_CTRL3_TMRB3IE(n)               (((uint32_t)(n) << 25) & 0x02000000)
#define AM_REG_CTIMER_CTRL3_TMRB3IE_DIS              0x00000000
#define AM_REG_CTIMER_CTRL3_TMRB3IE_EN               0x02000000

// Counter/Timer B3 Function Select.
#define AM_REG_CTIMER_CTRL3_TMRB3FN_S                22
#define AM_REG_CTIMER_CTRL3_TMRB3FN_M                0x01C00000
#define AM_REG_CTIMER_CTRL3_TMRB3FN(n)               (((uint32_t)(n) << 22) & 0x01C00000)
#define AM_REG_CTIMER_CTRL3_TMRB3FN_SINGLECOUNT      0x00000000
#define AM_REG_CTIMER_CTRL3_TMRB3FN_REPEATEDCOUNT    0x00400000
#define AM_REG_CTIMER_CTRL3_TMRB3FN_PULSE_ONCE       0x00800000
#define AM_REG_CTIMER_CTRL3_TMRB3FN_PULSE_CONT       0x00C00000
#define AM_REG_CTIMER_CTRL3_TMRB3FN_CONTINUOUS       0x01000000

// Counter/Timer B3 Clock Select.
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_S               17
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_M               0x003E0000
#define AM_REG_CTIMER_CTRL3_TMRB3CLK(n)              (((uint32_t)(n) << 17) & 0x003E0000)
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_TMRPIN          0x00000000
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_HFRC            0x00020000
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_HFRC_DIV8       0x00040000
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_HFRC_DIV128     0x00060000
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_HFRC_DIV512     0x00080000
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_HFRC_DIV2K      0x000A0000
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_XT              0x000C0000
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_XT_DIV2         0x000E0000
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_XT_DIV16        0x00100000
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_XT_DIV256       0x00120000
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_LFRC_DIV2       0x00140000
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_LFRC_DIV32      0x00160000
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_LFRC_DIV1K      0x00180000
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_LFRC            0x001A0000
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_RTC_100HZ       0x001C0000
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_HCLK            0x001E0000
#define AM_REG_CTIMER_CTRL3_TMRB3CLK_BUCKA           0x00200000

// Counter/Timer B3 Enable bit.
#define AM_REG_CTIMER_CTRL3_TMRB3EN_S                16
#define AM_REG_CTIMER_CTRL3_TMRB3EN_M                0x00010000
#define AM_REG_CTIMER_CTRL3_TMRB3EN(n)               (((uint32_t)(n) << 16) & 0x00010000)
#define AM_REG_CTIMER_CTRL3_TMRB3EN_DIS              0x00000000
#define AM_REG_CTIMER_CTRL3_TMRB3EN_EN               0x00010000

// Special Timer A3 enable for ADC function.
#define AM_REG_CTIMER_CTRL3_ADCEN_S                  15
#define AM_REG_CTIMER_CTRL3_ADCEN_M                  0x00008000
#define AM_REG_CTIMER_CTRL3_ADCEN(n)                 (((uint32_t)(n) << 15) & 0x00008000)

// Counter/Timer A3 output polarity.
#define AM_REG_CTIMER_CTRL3_TMRA3POL_S               12
#define AM_REG_CTIMER_CTRL3_TMRA3POL_M               0x00001000
#define AM_REG_CTIMER_CTRL3_TMRA3POL(n)              (((uint32_t)(n) << 12) & 0x00001000)
#define AM_REG_CTIMER_CTRL3_TMRA3POL_NORMAL          0x00000000
#define AM_REG_CTIMER_CTRL3_TMRA3POL_INVERTED        0x00001000

// Counter/Timer A3 Clear bit.
#define AM_REG_CTIMER_CTRL3_TMRA3CLR_S               11
#define AM_REG_CTIMER_CTRL3_TMRA3CLR_M               0x00000800
#define AM_REG_CTIMER_CTRL3_TMRA3CLR(n)              (((uint32_t)(n) << 11) & 0x00000800)
#define AM_REG_CTIMER_CTRL3_TMRA3CLR_CLEAR           0x00000800

// Counter/Timer A3 Output Enable bit.
#define AM_REG_CTIMER_CTRL3_TMRA3PE_S                10
#define AM_REG_CTIMER_CTRL3_TMRA3PE_M                0x00000400
#define AM_REG_CTIMER_CTRL3_TMRA3PE(n)               (((uint32_t)(n) << 10) & 0x00000400)
#define AM_REG_CTIMER_CTRL3_TMRA3PE_DIS              0x00000000
#define AM_REG_CTIMER_CTRL3_TMRA3PE_EN               0x00000400

// Counter/Timer A3 Interrupt Enable bit.
#define AM_REG_CTIMER_CTRL3_TMRA3IE_S                9
#define AM_REG_CTIMER_CTRL3_TMRA3IE_M                0x00000200
#define AM_REG_CTIMER_CTRL3_TMRA3IE(n)               (((uint32_t)(n) << 9) & 0x00000200)
#define AM_REG_CTIMER_CTRL3_TMRA3IE_DIS              0x00000000
#define AM_REG_CTIMER_CTRL3_TMRA3IE_EN               0x00000200

// Counter/Timer A3 Function Select.
#define AM_REG_CTIMER_CTRL3_TMRA3FN_S                6
#define AM_REG_CTIMER_CTRL3_TMRA3FN_M                0x000001C0
#define AM_REG_CTIMER_CTRL3_TMRA3FN(n)               (((uint32_t)(n) << 6) & 0x000001C0)
#define AM_REG_CTIMER_CTRL3_TMRA3FN_SINGLECOUNT      0x00000000
#define AM_REG_CTIMER_CTRL3_TMRA3FN_REPEATEDCOUNT    0x00000040
#define AM_REG_CTIMER_CTRL3_TMRA3FN_PULSE_ONCE       0x00000080
#define AM_REG_CTIMER_CTRL3_TMRA3FN_PULSE_CONT       0x000000C0
#define AM_REG_CTIMER_CTRL3_TMRA3FN_CONTINUOUS       0x00000100

// Counter/Timer A3 Clock Select.
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_S               1
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_M               0x0000003E
#define AM_REG_CTIMER_CTRL3_TMRA3CLK(n)              (((uint32_t)(n) << 1) & 0x0000003E)
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_TMRPIN          0x00000000
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_HFRC            0x00000002
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_HFRC_DIV8       0x00000004
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_HFRC_DIV128     0x00000006
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_HFRC_DIV512     0x00000008
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_HFRC_DIV2K      0x0000000A
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_XT              0x0000000C
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_XT_DIV2         0x0000000E
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_XT_DIV16        0x00000010
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_XT_DIV256       0x00000012
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_LFRC_DIV2       0x00000014
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_LFRC_DIV32      0x00000016
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_LFRC_DIV1K      0x00000018
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_LFRC            0x0000001A
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_RTC_100HZ       0x0000001C
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_HCLK            0x0000001E
#define AM_REG_CTIMER_CTRL3_TMRA3CLK_BUCKB           0x00000020

// Counter/Timer A3 Enable bit.
#define AM_REG_CTIMER_CTRL3_TMRA3EN_S                0
#define AM_REG_CTIMER_CTRL3_TMRA3EN_M                0x00000001
#define AM_REG_CTIMER_CTRL3_TMRA3EN(n)               (((uint32_t)(n) << 0) & 0x00000001)
#define AM_REG_CTIMER_CTRL3_TMRA3EN_DIS              0x00000000
#define AM_REG_CTIMER_CTRL3_TMRA3EN_EN               0x00000001

#endif // AM_REG_CTIMER_H
