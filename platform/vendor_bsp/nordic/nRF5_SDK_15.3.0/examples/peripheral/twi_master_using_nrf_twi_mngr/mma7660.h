/**
 * Copyright (c) 2015 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef MMA7660_H__
#define MMA7660_H__


#include "nrf_twi_mngr.h"

#ifdef __cplusplus
extern "C" {
#endif


// 0x98 is the MMA7660's address in the mbed Application Shield, it contains
// R/W bit and "nrf_drv_twi" (and consequently "nrf_twi_mngr") requires slave
// address without this bit, hence shifting.
#define MMA7660_ADDR        (0x98U >> 1)

#define MMA7660_REG_XOUT    0x00
#define MMA7660_REG_YOUT    0x01
#define MMA7660_REG_ZOUT    0x02
#define MMA7660_REG_TILT    0x03
#define MMA7660_REG_SRST    0x04
#define MMA7660_REG_SPCNT   0x05
#define MMA7660_REG_INTSU   0x06
#define MMA7660_REG_MODE    0x07
#define MMA7660_REG_SR      0x08
#define MMA7660_REG_PDET    0x09
#define MMA7660_REG_PD      0x0A

#define MMA7660_NUMBER_OF_REGISTERS 11

// The Alert bit (6) set signals that the register must be read again, since
// its value may be inaccurate (it was read at the same time as the device was
// attempting to update it).
// Applies to XOUT, YOUT, ZOUT and TILT registers.
#define MMA7660_DATA_IS_VALID(reg_data)  (!((reg_data) & (1U << 6)))

// Gets acceleration (g) value (6-bit 2's complement) from register data.
// [use "/ 4" instead of ">> 2", as the result of right-shifting of a signed
//  type value is implementation-defined]
#define MMA7660_GET_ACC(reg_data)  ((int8_t)((reg_data) << 2) / 4)

// Get orientation of the device (PoLa bits from the Tilt Status register).
#define MMA7660_GET_ORIENTATION(tilt_status)  (tilt_status & 0x1C)
#define MMA7660_ORIENTATION_LEFT    0x04
#define MMA7660_ORIENTATION_RIGHT   0x08
#define MMA7660_ORIENTATION_DOWN    0x14
#define MMA7660_ORIENTATION_UP      0x18

#define MMA7660_TAP_DETECTED(tilt_status)   (tilt_status & (1U << 5))
#define MMA7660_SHAKE_DETECTED(tilt_status) (tilt_status & (1U << 7))

extern uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND mma7660_xout_reg_addr;

#define MMA7660_READ(p_reg_addr, p_buffer, byte_cnt) \
    NRF_TWI_MNGR_WRITE(MMA7660_ADDR, p_reg_addr, 1,        NRF_TWI_MNGR_NO_STOP), \
    NRF_TWI_MNGR_READ (MMA7660_ADDR, p_buffer,   byte_cnt, 0)

#define MMA7660_READ_XYZ_AND_TILT(p_buffer) \
    MMA7660_READ(&mma7660_xout_reg_addr, p_buffer, 4)

#define MMA7660_INIT_TRANSFER_COUNT 1

extern nrf_twi_mngr_transfer_t const
    mma7660_init_transfers[MMA7660_INIT_TRANSFER_COUNT];

#ifdef __cplusplus
}
#endif

#endif // MMA7660_H__
