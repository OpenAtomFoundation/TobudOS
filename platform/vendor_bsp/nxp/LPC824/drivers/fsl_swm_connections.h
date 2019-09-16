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

#ifndef _FSL_SWM_CONNECTIONS_
#define _FSL_SWM_CONNECTIONS_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.swm_connections"
#endif

/*!
 * @addtogroup swm
 * @{
 */

/*!
 * @name swm connections
 * @{
 */

/*! @brief SWM port_pin number */
typedef enum _swm_port_pin_type_t
{
    kSWM_PortPin_P0_0 = 0U,   /*!< port_pin number P0_0. */
    kSWM_PortPin_P0_1 = 1U,   /*!< port_pin number P0_1. */
    kSWM_PortPin_P0_2 = 2U,   /*!< port_pin number P0_2. */
    kSWM_PortPin_P0_3 = 3U,   /*!< port_pin number P0_3. */
    kSWM_PortPin_P0_4 = 4U,   /*!< port_pin number P0_4. */
    kSWM_PortPin_P0_5 = 5U,   /*!< port_pin number P0_5. */
    kSWM_PortPin_P0_6 = 6U,   /*!< port_pin number P0_6. */
    kSWM_PortPin_P0_7 = 7U,   /*!< port_pin number P0_7. */
    kSWM_PortPin_P0_8 = 8U,   /*!< port_pin number P0_8. */
    kSWM_PortPin_P0_9 = 9U,   /*!< port_pin number P0_9. */
    kSWM_PortPin_P0_10 = 10U, /*!< port_pin number P0_10. */
    kSWM_PortPin_P0_11 = 11U, /*!< port_pin number P0_11. */
    kSWM_PortPin_P0_12 = 12U, /*!< port_pin number P0_12. */
    kSWM_PortPin_P0_13 = 13U, /*!< port_pin number P0_13. */
    kSWM_PortPin_P0_14 = 14U, /*!< port_pin number P0_14. */
    kSWM_PortPin_P0_15 = 15U, /*!< port_pin number P0_15. */
    kSWM_PortPin_P0_16 = 16U, /*!< port_pin number P0_16. */
    kSWM_PortPin_P0_17 = 17U, /*!< port_pin number P0_17. */
    kSWM_PortPin_P0_18 = 18U, /*!< port_pin number P0_18. */
    kSWM_PortPin_P0_19 = 19U, /*!< port_pin number P0_19. */
    kSWM_PortPin_P0_20 = 20U, /*!< port_pin number P0_20. */
    kSWM_PortPin_P0_21 = 21U, /*!< port_pin number P0_21. */
    kSWM_PortPin_P0_22 = 22U, /*!< port_pin number P0_22. */
    kSWM_PortPin_P0_23 = 23U, /*!< port_pin number P0_23. */
    kSWM_PortPin_P0_24 = 24U, /*!< port_pin number P0_24. */
    kSWM_PortPin_P0_25 = 25U, /*!< port_pin number P0_25. */
    kSWM_PortPin_P0_26 = 26U, /*!< port_pin number P0_26. */
    kSWM_PortPin_P0_27 = 27U, /*!< port_pin number P0_27. */
    kSWM_PortPin_P0_28 = 28U, /*!< port_pin number P0_28. */
} swm_port_pin_type_t;

/*! @brief SWM movable selection */
typedef enum _swm_select_movable_t
{
    kSWM_USART0_TXD = 0U,  /*!< Movable function as USART0_TXD. */
    kSWM_USART0_RXD = 1U,  /*!< Movable function as USART0_RXD. */
    kSWM_USART0_RTS = 2U,  /*!< Movable function as USART0_RTS. */
    kSWM_USART0_CTS = 3U,  /*!< Movable function as USART0_CTS. */

    kSWM_USART0_SCLK = 4U, /*!< Movable function as USART0_SCLK. */
    kSWM_USART1_TXD = 5U,  /*!< Movable function as USART1_TXD. */
    kSWM_USART1_RXD = 6U,  /*!< Movable function as USART1_RXD. */
    kSWM_USART1_RTS = 7U,  /*!< Movable function as USART1_RTS. */
    
    kSWM_USART1_CTS = 8U,  /*!< Movable function as USART1_CTS. */
    kSWM_USART1_SCLK = 9U, /*!< Movable function as USART1_SCLK. */
    kSWM_USART2_TXD = 10U,  /*!< Movable function as USART2_TXD. */
    kSWM_USART2_RXD = 11U,  /*!< Movable function as USART2_RXD. */
    
    kSWM_USART2_RTS = 12U,  /*!< Movable function as USART2_RTS. */
    kSWM_USART2_CTS = 13U,  /*!< Movable function as USART2_CTS. */
    kSWM_USART2_SCLK = 14U, /*!< Movable function as USART2_SCLK. */
    kSWM_SPI0_SCK = 15U,   /*!< Movable function as SPI0_SCK. */
   
    kSWM_SPI0_MOSI = 16U,  /*!< Movable function as SPI0_MOSI. */
    kSWM_SPI0_MISO = 17U,  /*!< Movable function as SPI0_MISO. */
    kSWM_SPI0_SSEL0 = 18U, /*!< Movable function as SPI0_SSEL0. */
    kSWM_SPI0_SSEL1 = 19U, /*!< Movable function as SPI0_SSEL1. */
    
    kSWM_SPI0_SSEL2 = 20U, /*!< Movable function as SPI0_SSEL2. */
    kSWM_SPI0_SSEL3 = 21U, /*!< Movable function as SPI0_SSEL3. */
    kSWM_SPI1_SCK = 22U,   /*!< Movable function as SPI1_SCK. */
    kSWM_SPI1_MOSI = 23U,  /*!< Movable function as SPI1_MOSI. */
   
    kSWM_SPI1_MISO = 24U,  /*!< Movable function as SPI1_MISO. */
    kSWM_SPI1_SSEL0 = 25U, /*!< Movable function as SPI1_SSEL0. */
    kSWM_SPI1_SSEL1 = 26U, /*!< Movable function as SPI1_SSEL1. */
    kSWM_SCT_PIN0 = 27U, /*!< Movable function as SCT_PIN0. */
    
    kSWM_SCT_PIN1 = 28U, /*!< Movable function as SCT_PIN1. */
    kSWM_SCT_PIN2 = 29U, /*!< Movable function as SCT_PIN2. */
    kSWM_SCT_PIN3 = 30U, /*!< Movable function as SCT_PIN3. */
    kSWM_SCT_OUT0 = 31U, /*!< Movable function as SCT_OUT0. */
    
    kSWM_SCT_OUT1 = 32U, /*!< Movable function as SCT_OUT1. */
    kSWM_SCT_OUT2 = 33U, /*!< Movable function as SCT_OUT2. */
    kSWM_SCT_OUT3 = 34U, /*!< Movable function as SCT_OUT3. */
    kSWM_SCT_OUT4 = 35U, /*!< Movable function as SCT_OUT4. */
    
    kSWM_SCT_OUT5 = 36U, /*!< Movable function as SCT_OUT5. */
    kSWM_I2C1_SDA = 37U, /*!< Movable function as I2C1_SDA. */
    kSWM_I2C1_SCL = 38U, /*!< Movable function as I2C1_SCL. */
    kSWM_I2C2_SDA = 39U, /*!< Movable function as I2C2_SDA. */
   
    kSWM_I2C2_SCL = 40U, /*!< Movable function as I2C2_SCL. */
    kSWM_I2C3_SDA = 41U, /*!< Movable function as I2C3_SDA. */
    kSWM_I2C3_SCL = 42U, /*!< Movable function as I2C3_SCL. */
    kSWM_ADC_PINTRIG0 = 43U,         /*!< Movable function as PINTRIG0. */
  
    kSWM_ADC_PINTRIG1 = 44U,        /*!< Movable function as  PINTRIG1. */    
    kSWM_ACOMP = 45U,         /*!< Movable function as ACOMP. */
    kSWM_CLKOUT = 46U,        /*!< Movable function as CLKOUT. */
    kSWM_GPIO_INT_BMAT = 47U, /*!< Movable function as GPIO_INT_BMAT. */
    
    kSWM_MOVABLE_NUM_FUNCS = 48U, /*!< Movable function number. */
} swm_select_movable_t;

/*! @brief SWM fixed pin selection */
typedef enum _swm_select_fixed_pin_t
{
    kSWM_ACMP_INPUT1 = SWM_PINENABLE0_ACMP_I1_MASK, /*!< Fixed-pin function as ACMP_INPUT1. */
    kSWM_ACMP_INPUT2 = SWM_PINENABLE0_ACMP_I2_MASK, /*!< Fixed-pin function as ACMP_INPUT2. */
    kSWM_ACMP_INPUT3 = SWM_PINENABLE0_ACMP_I3_MASK, /*!< Fixed-pin function as ACMP_INPUT3. */
    kSWM_ACMP_INPUT4 = SWM_PINENABLE0_ACMP_I4_MASK, /*!< Fixed-pin function as ACMP_INPUT4. */
    kSWM_SWCLK = SWM_PINENABLE0_SWCLK_MASK,         /*!< Fixed-pin function as SWCLK. */
    kSWM_SWDIO = SWM_PINENABLE0_SWDIO_MASK,         /*!< Fixed-pin function as SWDIO. */
    kSWM_XTALIN = SWM_PINENABLE0_XTALIN_MASK,       /*!< Fixed-pin function as XTALIN. */
    kSWM_XTALOUT = SWM_PINENABLE0_XTALOUT_MASK,     /*!< Fixed-pin function as XTALOUT. */
    kSWM_RESETN = SWM_PINENABLE0_RESETN_MASK,       /*!< Fixed-pin function as RESETN. */
    kSWM_CLKIN = SWM_PINENABLE0_CLKIN_MASK,         /*!< Fixed-pin function as CLKIN. */
    kSWM_VDDCMP = SWM_PINENABLE0_VDDCMP_MASK,       /*!< Fixed-pin function as VDDCMP. */
    kSWM_I2C0_SDA = SWM_PINENABLE0_I2C0_SDA_MASK,   /*!< Fixed-pin function as I2C0_SDA. */
    kSWM_I2C0_SCL = SWM_PINENABLE0_I2C0_SCL_MASK,   /*!< Fixed-pin function as I2C0_SCL. */
    kSWM_ADC_CHN0 = SWM_PINENABLE0_ADC_0_MASK,      /*!< Fixed-pin function as ADC_CHN0. */
    kSWM_ADC_CHN1 = SWM_PINENABLE0_ADC_1_MASK,      /*!< Fixed-pin function as ADC_CHN1. */
    kSWM_ADC_CHN2 = SWM_PINENABLE0_ADC_2_MASK,      /*!< Fixed-pin function as ADC_CHN2. */
    kSWM_ADC_CHN3 = SWM_PINENABLE0_ADC_3_MASK,      /*!< Fixed-pin function as ADC_CHN3. */
    kSWM_ADC_CHN4 = SWM_PINENABLE0_ADC_4_MASK,      /*!< Fixed-pin function as ADC_CHN4. */
    kSWM_ADC_CHN5 = SWM_PINENABLE0_ADC_5_MASK,      /*!< Fixed-pin function as ADC_CHN5. */
    kSWM_ADC_CHN6 = SWM_PINENABLE0_ADC_6_MASK,      /*!< Fixed-pin function as ADC_CHN6. */
    kSWM_ADC_CHN7 = SWM_PINENABLE0_ADC_7_MASK,      /*!< Fixed-pin function as ADC_CHN7. */
    kSWM_ADC_CHN8 = SWM_PINENABLE0_ADC_8_MASK,      /*!< Fixed-pin function as ADC_CHN8. */
    kSWM_ADC_CHN9 = SWM_PINENABLE0_ADC_9_MASK,      /*!< Fixed-pin function as ADC_CHN9. */
    kSWM_ADC_CHN10 = SWM_PINENABLE0_ADC_10_MASK,    /*!< Fixed-pin function as ADC_CHN10. */
    kSWM_ADC_CHN11 = SWM_PINENABLE0_ADC_11_MASK,    /*!< Fixed-pin function as ADC_CHN11. */

    kSWM_FIXEDPIN_NUM_FUNCS = 0x80000041, /*!< Fixed-pin function number. */
} swm_select_fixed_pin_t;

/*@}*/

#endif /* _FSL_INPUTMUX_CONNECTIONS_ */
