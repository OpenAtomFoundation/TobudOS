/*
 * The Clear BSD License
 * Copyright (c) 2017, NXP Semiconductors, Inc.
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _FSL_IOCON_H_
#define _FSL_IOCON_H_

#include "fsl_common.h"

/*!
 * @addtogroup lpc_iocon
 * @{
 */

/*! @file */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.lpc_iocon_lite"
#endif


/*! @name Driver version */
/*@{*/
/*! @brief IOCON driver version 2.0.0. */
#define LPC_IOCON_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/**
 * @brief Array of IOCON pin definitions passed to IOCON_SetPinMuxing() must be in this format
 */
typedef struct _iocon_group
{
    uint32_t ionumber : 8;  /* IO number */
    uint32_t modefunc : 16; /* Function and mode */
} iocon_group_t;

/**
 * @brief IOCON function and mode selection definitions
 * @note See the User Manual for specific modes and functions supported by the various pins.
 */
#if defined(IOCON_PIO_MODE_SHIFT)
#define IOCON_MODE_INACT (0x0 << IOCON_PIO_MODE_SHIFT)    /*!< No addition pin function */
#define IOCON_MODE_PULLDOWN (0x1 << IOCON_PIO_MODE_SHIFT) /*!< Selects pull-down function */
#define IOCON_MODE_PULLUP (0x2 << IOCON_PIO_MODE_SHIFT)   /*!< Selects pull-up function */
#define IOCON_MODE_REPEATER (0x3 << IOCON_PIO_MODE_SHIFT) /*!< Selects pin repeater function */
#endif
#if defined(IOCON_PIO_HYS_SHIFT)
#define IOCON_HYS_EN (0x1 << IOCON_PIO_HYS_SHIFT) /*!< Enables hysteresis */
#endif
#if defined(IOCON_PIO_INV_SHIFT)
#define IOCON_INV_EN (0x1 << IOCON_PIO_INV_SHIFT) /*!< Enables invert function on input */
#endif
#if defined(IOCON_PIO_I2CMODE_SHIFT)
#define IOCON_STDI2C_EN (0x0 << IOCON_PIO_I2CMODE_SHIFT) /*!< I2C standard mode/fast-mode */
#define IOCON_STDGPIO_EN                                                                                           \
    (0x1 << IOCON_PIO_I2CMODE_SHIFT) /*!< Standard GPIO functionality. Requires external pull-up for GPIO output \ \
                                         function */
#define IOCON_FASTI2C_EN (0x2 << IOCON_PIO_I2CMODE_SHIFT) /*!< I2C Fast-mode Plus */
#endif
#if defined(IOCON_PIO_OD_SHIFT)
#define IOCON_OPENDRAIN_EN (0x1 << IOCON_PIO_OD_SHIFT) /*!< Enables open-drain function */
#endif
#if defined(IOCON_PIO_S_MODE_SHIFT)
#define IOCON_S_MODE_0CLK (0x0 << IOCON_PIO_S_MODE_SHIFT) /*!< Bypass input filter */
#define IOCON_S_MODE_1CLK                                                                        \
    (0x1 << IOCON_PIO_S_MODE_SHIFT) /*!< Input pulses shorter than 1 filter clock are rejected \ \
                                        */
#define IOCON_S_MODE_2CLK                                                                         \
    (0x2 << IOCON_PIO_S_MODE_SHIFT) /*!< Input pulses shorter than 2 filter clock2 are rejected \ \
                                        */
#define IOCON_S_MODE_3CLK                                                                         \
    (0x3 << IOCON_PIO_S_MODE_SHIFT) /*!< Input pulses shorter than 3 filter clock2 are rejected \ \
                                        */
#endif
#if defined(IOCON_PIO_CLK_DIV_SHIFT)
#define IOCON_CLKDIV(div) \
    ((div)                \
     << IOCON_PIO_CLK_DIV_SHIFT) /*!< Select peripheral clock divider for input filter sampling clock, 2^n, n=0-6 */
#endif
#if defined(IOCON_PIO_DACMODE_SHIFT)
#define IOCON_DAC_MODE_EN (0x1 << IOCON_PIO_DACMODE_SHIFT) /*!< DAC mode enable */
#endif
#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief   Sets I/O Control pin mux
 * @param   base        : The base of IOCON peripheral on the chip
 * @param   ionumber    : GPIO number to mux
 * @param   modefunc    : OR'ed values of type IOCON_*
 * @return  Nothing
 */
__STATIC_INLINE void IOCON_PinMuxSet(IOCON_Type *base, uint8_t ionumber, uint32_t modefunc)
{
    base->PIO[ionumber] = modefunc;
}

/**
 * @brief   Set all I/O Control pin muxing
 * @param   base        : The base of IOCON peripheral on the chip
 * @param   pinArray    : Pointer to array of pin mux selections
 * @param   arrayLength : Number of entries in pinArray
 * @return  Nothing
 */
__STATIC_INLINE void IOCON_SetPinMuxing(IOCON_Type *base, const iocon_group_t *pinArray, uint32_t arrayLength)
{
    uint32_t i;

    for (i = 0; i < arrayLength; i++)
    {
        IOCON_PinMuxSet(base, pinArray[i].ionumber, pinArray[i].modefunc);
    }
}

/* @} */

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_IOCON_H_ */
