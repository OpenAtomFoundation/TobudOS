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
 * @file     am_reg_jedec.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Register macros for the JEDEC module
 ******************************************************************************/
#ifndef AM_REG_JEDEC_H
#define AM_REG_JEDEC_H

//*****************************************************************************
//
// Instance finder. (1 instance(s) available)
//
//*****************************************************************************
#define AM_REG_JEDEC_NUM_MODULES                     1
#define AM_REG_JEDECn(n) \
    (REG_JEDEC_BASEADDR + 0x00000000 * n)

//*****************************************************************************
//
// Register offsets.
//
//*****************************************************************************
#define AM_REG_JEDEC_PID4_O                          0xF0000FD0
#define AM_REG_JEDEC_PID5_O                          0xF0000FD4
#define AM_REG_JEDEC_PID6_O                          0xF0000FD8
#define AM_REG_JEDEC_PID7_O                          0xF0000FDC
#define AM_REG_JEDEC_PID0_O                          0xF0000FE0
#define AM_REG_JEDEC_PID1_O                          0xF0000FE4
#define AM_REG_JEDEC_PID2_O                          0xF0000FE8
#define AM_REG_JEDEC_PID3_O                          0xF0000FEC
#define AM_REG_JEDEC_CID0_O                          0xF0000FF0
#define AM_REG_JEDEC_CID1_O                          0xF0000FF4
#define AM_REG_JEDEC_CID2_O                          0xF0000FF8
#define AM_REG_JEDEC_CID3_O                          0xF0000FFC

//*****************************************************************************
//
// JEDEC_PID4 - JEP Continuation Register
//
//*****************************************************************************
// Contains the JEP Continuation bits.
#define AM_REG_JEDEC_PID4_JEPCONT_S                  0
#define AM_REG_JEDEC_PID4_JEPCONT_M                  0x0000000F
#define AM_REG_JEDEC_PID4_JEPCONT(n)                 (((uint32_t)(n) << 0) & 0x0000000F)

//*****************************************************************************
//
// JEDEC_PID5 - JEP reserved Register
//
//*****************************************************************************
// Contains the value of 0x00000000.
#define AM_REG_JEDEC_PID5_VALUE_S                    0
#define AM_REG_JEDEC_PID5_VALUE_M                    0xFFFFFFFF
#define AM_REG_JEDEC_PID5_VALUE(n)                   (((uint32_t)(n) << 0) & 0xFFFFFFFF)

//*****************************************************************************
//
// JEDEC_PID6 - JEP reserved Register
//
//*****************************************************************************
// Contains the value of 0x00000000.
#define AM_REG_JEDEC_PID6_VALUE_S                    0
#define AM_REG_JEDEC_PID6_VALUE_M                    0xFFFFFFFF
#define AM_REG_JEDEC_PID6_VALUE(n)                   (((uint32_t)(n) << 0) & 0xFFFFFFFF)

//*****************************************************************************
//
// JEDEC_PID7 - JEP reserved Register
//
//*****************************************************************************
// Contains the value of 0x00000000.
#define AM_REG_JEDEC_PID7_VALUE_S                    0
#define AM_REG_JEDEC_PID7_VALUE_M                    0xFFFFFFFF
#define AM_REG_JEDEC_PID7_VALUE(n)                   (((uint32_t)(n) << 0) & 0xFFFFFFFF)

//*****************************************************************************
//
// JEDEC_PID0 - Nationz Partnum low byte
//
//*****************************************************************************
// Contains the low 8 bits of the Nationz device part number.
#define AM_REG_JEDEC_PID0_PNL8_S                     0
#define AM_REG_JEDEC_PID0_PNL8_M                     0x000000FF
#define AM_REG_JEDEC_PID0_PNL8(n)                    (((uint32_t)(n) << 0) & 0x000000FF)

//*****************************************************************************
//
// JEDEC_PID1 - Nationz part number high-nibble, JEPID low-nibble.
//
//*****************************************************************************
// Contains the low 4 bits of the Nationz JEDEC JEP-106 ID. The full JEPID
// is therefore 0x9B.
#define AM_REG_JEDEC_PID1_JEPIDL_S                   4
#define AM_REG_JEDEC_PID1_JEPIDL_M                   0x000000F0
#define AM_REG_JEDEC_PID1_JEPIDL(n)                  (((uint32_t)(n) << 4) & 0x000000F0)

// Contains the high 4 bits of the Nationz device part number.
#define AM_REG_JEDEC_PID1_PNH4_S                     0
#define AM_REG_JEDEC_PID1_PNH4_M                     0x0000000F
#define AM_REG_JEDEC_PID1_PNH4(n)                    (((uint32_t)(n) << 0) & 0x0000000F)

//*****************************************************************************
//
// JEDEC_PID2 - Nationz chip revision low-nibble, JEPID high-nibble
//
//*****************************************************************************
// Contains the high 4 bits of the Nationz CHIPREV (see also
// MCUCTRL.CHIPREV). Note that this field will change with each revision of the
// chip.
#define AM_REG_JEDEC_PID2_CHIPREVH4_S                4
#define AM_REG_JEDEC_PID2_CHIPREVH4_M                0x000000F0
#define AM_REG_JEDEC_PID2_CHIPREVH4(n)               (((uint32_t)(n) << 4) & 0x000000F0)

// Contains the high 3 bits of the Nationz JEPID. Note that bit3 of this
// field is hard-coded to 1. The full JEPID is therefore 0x9B.
#define AM_REG_JEDEC_PID2_JEPIDH_S                   0
#define AM_REG_JEDEC_PID2_JEPIDH_M                   0x0000000F
#define AM_REG_JEDEC_PID2_JEPIDH(n)                  (((uint32_t)(n) << 0) & 0x0000000F)

//*****************************************************************************
//
// JEDEC_PID3 - Nationz chip revision high-nibble.
//
//*****************************************************************************
// Contains the low 4 bits of the Nationz CHIPREV (see also
// MCUCTRL.CHIPREV). Note that this field will change with each revision of the
// chip.
#define AM_REG_JEDEC_PID3_CHIPREVL4_S                4
#define AM_REG_JEDEC_PID3_CHIPREVL4_M                0x000000F0
#define AM_REG_JEDEC_PID3_CHIPREVL4(n)               (((uint32_t)(n) << 4) & 0x000000F0)

// This field is hard-coded to 0x0.
#define AM_REG_JEDEC_PID3_ZERO_S                     0
#define AM_REG_JEDEC_PID3_ZERO_M                     0x0000000F
#define AM_REG_JEDEC_PID3_ZERO(n)                    (((uint32_t)(n) << 0) & 0x0000000F)

//*****************************************************************************
//
// JEDEC_CID0 - Coresight ROM Table.
//
//*****************************************************************************
// Coresight ROM Table, CID0.
#define AM_REG_JEDEC_CID0_CID_S                      0
#define AM_REG_JEDEC_CID0_CID_M                      0x000000FF
#define AM_REG_JEDEC_CID0_CID(n)                     (((uint32_t)(n) << 0) & 0x000000FF)

//*****************************************************************************
//
// JEDEC_CID1 - Coresight ROM Table.
//
//*****************************************************************************
// Coresight ROM Table, CID1.
#define AM_REG_JEDEC_CID1_CID_S                      0
#define AM_REG_JEDEC_CID1_CID_M                      0x000000FF
#define AM_REG_JEDEC_CID1_CID(n)                     (((uint32_t)(n) << 0) & 0x000000FF)

//*****************************************************************************
//
// JEDEC_CID2 - Coresight ROM Table.
//
//*****************************************************************************
// Coresight ROM Table, CID2.
#define AM_REG_JEDEC_CID2_CID_S                      0
#define AM_REG_JEDEC_CID2_CID_M                      0x000000FF
#define AM_REG_JEDEC_CID2_CID(n)                     (((uint32_t)(n) << 0) & 0x000000FF)

//*****************************************************************************
//
// JEDEC_CID3 - Coresight ROM Table.
//
//*****************************************************************************
// Coresight ROM Table, CID3.
#define AM_REG_JEDEC_CID3_CID_S                      0
#define AM_REG_JEDEC_CID3_CID_M                      0x000000FF
#define AM_REG_JEDEC_CID3_CID(n)                     (((uint32_t)(n) << 0) & 0x000000FF)

#endif // AM_REG_JEDEC_H
