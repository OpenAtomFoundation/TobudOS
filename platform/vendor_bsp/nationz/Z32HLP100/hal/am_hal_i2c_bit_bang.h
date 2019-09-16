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
 * @file     am_hal_i2c_bit_bang.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief   I2C bit bang module.
 *
 * These functions implement the I2C bit bang utility
 *
 ******************************************************************************/
#ifndef AM_HAL_I2C_BIT_BANG_H
#define AM_HAL_I2C_BIT_BANG_H

//*****************************************************************************
//
// Enumerated return constants
//
//*****************************************************************************
typedef enum
{
    AM_HAL_I2C_BIT_BANG_SUCCESS = 0,
    AM_HAL_I2C_BIT_BANG_ADDRESS_NAKED,
    AM_HAL_I2C_BIT_BANG_DATA_NAKED,
    AM_HAL_I2C_BIT_BANG_CLOCK_TIMEOUT,
    AM_HAL_I2C_BIT_BANG_DATA_TIMEOUT,
}am_hal_i2c_bit_bang_enum_t;

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************
extern am_hal_i2c_bit_bang_enum_t am_hal_i2c_bit_bang_init(uint32_t sck_gpio_number,
                                     uint32_t sda_gpio_number);

extern am_hal_i2c_bit_bang_enum_t am_hal_i2c_bit_bang_send(uint8_t address,
                                                    uint32_t number_of_bytes,
                                                    uint8_t *pData,
                                                    uint8_t ui8Offset,
                                                    bool bUseOffset,
                                                    bool bNoStop);

extern am_hal_i2c_bit_bang_enum_t am_hal_i2c_bit_bang_receive(uint8_t address,
                                                    uint32_t number_of_bytes,
                                                    uint8_t *pData,
                                                    uint8_t ui8Offset,
                                                    bool bUseOffset,
                                                    bool bNoStop);
#ifdef __cplusplus
}
#endif

#endif //AM_HAL_I2C_BIT_BANG_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
