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
#ifndef LM75B_H__
#define LM75B_H__


#include "nrf_twi_mngr.h"

#ifdef __cplusplus
extern "C" {
#endif


// 0x90 is the LM75B's address in the mbed Application Shield, it contains
// R/W bit and "nrf_drv_twi" (and consequently "nrf_twi_mngr") requires slave
// address without this bit, hence shifting.
#define LM75B_ADDR          (0x90U >> 1)

#define LM75B_REG_TEMP      0x00
#define LM75B_REG_CONF      0x01
#define LM75B_REG_THYST     0x02
#define LM75B_REG_TOS       0x03

// [use "/ 32" instead of ">> 5", as the result of right-shifting of a signed
//  type value is implementation-defined]
#define LM75B_GET_TEMPERATURE_VALUE(temp_hi, temp_lo) \
    ((((int16_t)temp_hi << 8) | temp_lo) / 32)


extern uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND lm75b_conf_reg_addr;
extern uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND lm75b_temp_reg_addr;
extern uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND lm75b_tos_reg_addr;
extern uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND lm75b_thyst_reg_addr;

#define LM75B_READ(p_reg_addr, p_buffer, byte_cnt) \
    NRF_TWI_MNGR_WRITE(LM75B_ADDR, p_reg_addr, 1,        NRF_TWI_MNGR_NO_STOP), \
    NRF_TWI_MNGR_READ (LM75B_ADDR, p_buffer,   byte_cnt, 0)

#define LM75B_READ_TEMP(p_buffer) \
    LM75B_READ(&lm75b_temp_reg_addr, p_buffer, 2)

#define LM75B_INIT_TRANSFER_COUNT 1

extern nrf_twi_mngr_transfer_t const lm75b_init_transfers[LM75B_INIT_TRANSFER_COUNT];

#ifdef __cplusplus
}
#endif

#endif // LM75B_H__
