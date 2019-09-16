/*
 * Copyright (c) 2017, NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
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

#ifndef _FSL_RESET_H_
#define _FSL_RESET_H_

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "fsl_device_registers.h"

/*!
 * @addtogroup ksdk_common
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief reset driver version 2.0.0. */
#define FSL_RESET_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/*!
 * @brief Enumeration for peripheral reset control bits
 *
 * Defines the enumeration for peripheral reset control bits in PRESETCTRL/ASYNCPRESETCTRL registers
 */
typedef enum _SYSCON_RSTn
{
    kSPI0_RST_N_SHIFT_RSTn = 0 | 0U,     /**< SPI0 reset control. */
    kSPI1_RST_N_SHIFT_RSTn = 0 | 1U,     /**< SPI1 reset control */
    kUARTFRG_RST_N_SHIFT_RSTn = 0 | 2U,  /**< UARTFRG reset control */
    kUART0_RST_N_SHIFT_RSTn = 0 | 3U,   /**< UART0 reset control */
    kUART1_RST_N_SHIFT_RSTn = 0 | 4U,   /**< UART1 reset control */
    kUART2_RST_N_SHIFT_RSTn = 0 | 5U,   /**< UART2 reset control */
    kI2C0_RST_N_SHIFT_RSTn = 0 | 6U,    /**< I2C0 reset control */
    kMRT_RST_N_SHIFT_RSTn = 0 | 7U,     /**< Multi-rate timer(MRT) reset control */
    kSCT_RST_N_SHIFT_RSTn = 0 | 8U,      /**< SCT reset control */
    kWKT_RST_N_SHIFT_RSTn = 0 | 9U,      /**< Self-wake-up timer(WKT) reset control */
    kGPIO0_RST_N_SHIFT_RSTn = 0 | 10U,    /**< GPIO0 reset control */
    kFLASH_RST_N_SHIFT_RSTn = 0 | 11U,    /**< Flash controller reset control */
    kACMP_RST_N_SHIFT_RSTn = 0 | 12U,    /**< Analog comparator reset control */
    kI2C1_RST_N_SHIFT_RSTn = 0 | 14U,    /**< I2C1 reset control */
    kI2C2_RST_N_SHIFT_RSTn = 0 | 15U,    /**< I2C2 reset control */
    kI2C3_RST_N_SHIFT_RSTn = 0 | 16U,    /**< I2C3 reset control */    
    kADC_RST_N_SHIFT_RSTn = 0 | 24U,     /**< ADC reset control */
    kDMA_RST_N_SHIFT_RSTn = 0 | 29U,     /**< DMA reset control */

} SYSCON_RSTn_t;

/** Array initializers with peripheral reset bits **/
#define FLASH_RSTS_N            \
    {                           \
        kFLASH_RST_N_SHIFT_RSTn \
    } /* Reset bits for Flash peripheral */
#define I2C_RSTS_N                                                                                     \
    {                                                                                                  \
        kI2C0_RST_N_SHIFT_RSTn, kI2C1_RST_N_SHIFT_RSTn, kI2C2_RST_N_SHIFT_RSTn, kI2C3_RST_N_SHIFT_RSTn \
    } /* Reset bits for I2C peripheral */
#define GPIO_RSTS_N                                      \
    {                                                    \
        kGPIO0_RST_N_SHIFT_RSTn, kGPIO1_RST_N_SHIFT_RSTn \
    } /* Reset bits for GPIO peripheral */
#define SWM_RSTS_N            \
    {                         \
        kSWM_RST_N_SHIFT_RSTn \
    } /* Reset bits for SWM peripheral */
#define SCT_RSTS_N            \
    {                         \
        kSCT_RST_N_SHIFT_RSTn \
    } /* Reset bits for SCT peripheral */
#define WKT_RSTS_N            \
    {                         \
        kWKT_RST_N_SHIFT_RSTn \
    } /* Reset bits for WKT peripheral */
#define MRT_RSTS_N            \
    {                         \
        kMRT_RST_N_SHIFT_RSTn \
    } /* Reset bits for MRT peripheral */
#define SPI_RSTS_N                                     \
    {                                                  \
        kSPI0_RST_N_SHIFT_RSTn, kSPI1_RST_N_SHIFT_RSTn \
    } /* Reset bits for SPI peripheral */
#define UART_RSTS_N                                                                                         \
    {                                                                                                       \
        kUART0_RST_N_SHIFT_RSTn, kUART1_RST_N_SHIFT_RSTn, kUART2_RST_N_SHIFT_RSTn                           \
    } /* Reset bits for UART peripheral */
#define ACMP_RSTS_N            \
    {                          \
        kACMP_RST_N_SHIFT_RSTn \
    } /* Reset bits for ACMP peripheral */
#define ADC_RSTS_N            \
    {                         \
        kADC_RST_N_SHIFT_RSTn \
    } /* Reset bits for ADC peripheral */
#define DAC_RSTS_N                                     \
    {                                                  \
        kDAC0_RST_N_SHIFT_RSTn, kDAC1_RST_N_SHIFT_RSTn \
    } /* Reset bits for DAC peripheral */
#define DMA_RSTS_N            \
    {                         \
        kDMA_RST_N_SHIFT_RSTn \
    } /* Reset bits for DMA peripheral */

typedef SYSCON_RSTn_t reset_ip_name_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Assert reset to peripheral.
 *
 * Asserts reset signal to specified peripheral module.
 *
 * @param peripheral Assert reset to this peripheral. The enum argument contains encoding of reset register
 *                   and reset bit position in the reset register.
 */
void RESET_SetPeripheralReset(reset_ip_name_t peripheral);

/*!
 * @brief Clear reset to peripheral.
 *
 * Clears reset signal to specified peripheral module, allows it to operate.
 *
 * @param peripheral Clear reset to this peripheral. The enum argument contains encoding of reset register
 *                   and reset bit position in the reset register.
 */
void RESET_ClearPeripheralReset(reset_ip_name_t peripheral);

/*!
 * @brief Reset peripheral module.
 *
 * Reset peripheral module.
 *
 * @param peripheral Peripheral to reset. The enum argument contains encoding of reset register
 *                   and reset bit position in the reset register.
 */
void RESET_PeripheralReset(reset_ip_name_t peripheral);

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _FSL_RESET_H_ */
