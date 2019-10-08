/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
*
* \file ifx_i2c_config.c
*
* \brief   This file provides the ifx i2c platform specific context configurations.
*
* \addtogroup  grIFXI2C
* @{
*/

/***********************************************************************************************************************
* HEADER FILES
**********************************************************************************************************************/
// Protocol Stack Includes
#include "optiga/pal/pal_ifx_i2c_config.h"
#include "optiga/ifx_i2c/ifx_i2c_config.h"

/***********************************************************************************************************************
* MACROS
**********************************************************************************************************************/


/***********************************************************************************************************************
* ENUMS
**********************************************************************************************************************/
/***********************************************************************************************************************
* DATA STRUCTURES
***********************************************************************************************************************/

/** @brief This is IFX I2C context. Only one context is supported per slave.*/
//lint --e{785} suppress "Only required fields are initialized, the rest are handled by consumer of this structure"
ifx_i2c_context_t ifx_i2c_context_0 =
{
    /// Slave address
    0x30,
    /// i2c-master frequency
    400,
    /// IFX-I2C frame size
#if (DL_MAX_FRAME_SIZE >= 0x0115)
    0x0115,
#else
    DL_MAX_FRAME_SIZE,
#endif
    /// Vdd pin
    &optiga_vdd_0,
    /// Reset pin
    &optiga_reset_0,
    /// optiga pal i2c context
    &optiga_pal_i2c_context_0,
};

/***********************************************************************************************************************
* GLOBAL
***********************************************************************************************************************/
/***********************************************************************************************************************
* LOCAL ROUTINES
***********************************************************************************************************************/
/***********************************************************************************************************************
* API PROTOTYPES
**********************************************************************************************************************/

/**
 * @}
 **/
