/** ----------------------------------------------------------------------------
 *         Nationz Technology Software Support  -  NATIONZ  -
 * -----------------------------------------------------------------------------
 * Copyright (c) 2013£­2018, Nationz Corporation  All rights reserved.
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
 *        Copyright (c) 2013£­2018 All Rights Reserved 
 *******************************************************************************
 * @file     am_hal_gpio.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Register macros for the GPIO module

 * @brief Functions for accessing and configuring the GPIO module.
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup gpio GPIO
 * @ingroup hal
 * @{
 * ******************************************************************************/

#ifndef AM_HAL_GPIO_H
#define AM_HAL_GPIO_H

// DEVICE ADDRESS IS 8-bits
#define AM_HAL_GPIO_DEV_ADDR_8      (0)

// DEVICE ADDRESS IS 16-bits
#define AM_HAL_GPIO_DEV_ADDR_16     (1)

// DEVICE OFFSET IS 8-bits
#define AM_HAL_GPIO_DEV_OFFSET_8    (0x00000000)

// DEVICE OFFSET IS 16-bits
#define AM_HAL_GPIO_DEV_OFFSET_16   (0x00010000)

// Maximum number of GPIOs on this device
#define AM_HAL_GPIO_MAX_PADS        (50)

//*****************************************************************************
//
//! @name GPIO Pin defines
//! @brief GPIO Pin defines for use with interrupt functions
//!
//! These macros may be used to with \e am_hal_gpio_int_x().
//! @{
//
//*****************************************************************************
#define AM_HAL_GPIO_BIT(n)          (((uint64_t) 0x1) << n)
//! @}

//*****************************************************************************
//
// Input options.
//
//*****************************************************************************
#define AM_HAL_GPIO_INPEN           0x00000002 // Enable input transistors.
#define AM_HAL_GPIO_INPEN1           0x00000802 // Enable input transistors.
#define AM_HAL_GPIO_INCFG_RDZERO    0x00000100 // Disable input read registers.

//*****************************************************************************
//
// Output options
//
//*****************************************************************************
#define AM_HAL_GPIO_OUT_DISABLE     0x00000000
#define AM_HAL_GPIO_OUT_PUSHPULL    0x00000200
#define AM_HAL_GPIO_OUT_OPENDRAIN   0x00000400
#define AM_HAL_GPIO_OUT_3STATE      0x00000600

//*****************************************************************************
//
// Interrupt polarity
//
//*****************************************************************************
#define AM_HAL_GPIO_FALLING         0x00000001
#define AM_HAL_GPIO_RISING          0x00000000

//*****************************************************************************
//
// Pad configuration options.
//
//*****************************************************************************
#define AM_HAL_GPIO_POWER           0x00000080
#define AM_HAL_GPIO_HIGH_DRIVE      0x00000004
#define AM_HAL_GPIO_LOW_DRIVE       0x00000000
#define AM_HAL_GPIO_PULLUP          0x00000001
#define AM_HAL_GPIO_PULL1_5K        0x00000001
#define AM_HAL_GPIO_PULL6K          0x00000041
#define AM_HAL_GPIO_PULL12K         0x00000081
#define AM_HAL_GPIO_PULL24K         0x000000C1
#define AM_HAL_GPIO_FUNC(x)         ((x & 0x7) << 3)

//*****************************************************************************
//
// Common pin configurations.
//
//*****************************************************************************
#define AM_HAL_GPIO_DISABLE                                                   \
    (AM_HAL_GPIO_FUNC(3))

#define AM_HAL_GPIO_INPUT                                                     \
    (AM_HAL_GPIO_FUNC(3) | AM_HAL_GPIO_INPEN)

#define AM_HAL_GPIO_OUTPUT                                                    \
    (AM_HAL_GPIO_FUNC(3) | AM_HAL_GPIO_OUT_PUSHPULL)

#define AM_HAL_GPIO_OPENDRAIN                                                 \
    (AM_HAL_GPIO_FUNC(3) | AM_HAL_GPIO_OUT_OPENDRAIN | AM_HAL_GPIO_INPEN)

#define AM_HAL_GPIO_3STATE                                                    \
    (AM_HAL_GPIO_FUNC(3) | AM_HAL_GPIO_OUT_3STATE)

//*****************************************************************************
//
// PADREG helper macros.
//
//*****************************************************************************
#define AM_HAL_GPIO_PADREG(n)                                                 \
    (AM_REG_GPIOn(0) + AM_REG_GPIO_PADREGA_O + (n & 0xFC))

#define AM_HAL_GPIO_PADREG_S(n)                                               \
    (((uint32_t)(n) % 4) << 3)

#define AM_HAL_GPIO_PADREG_M(n)                                               \
    ((uint32_t) 0xFF << AM_HAL_GPIO_PADREG_S(n))

#define AM_HAL_GPIO_PADREG_FIELD(n, configval)                                \
    (((uint32_t)(configval) & 0xFF) << AM_HAL_GPIO_PADREG_S(n))

#define AM_HAL_GPIO_PADREG_W(n, configval)                                    \
    AM_REGVAL(AM_HAL_GPIO_PADREG(n)) =                                        \
        (AM_HAL_GPIO_PADREG_FIELD(n, configval) |                             \
         (AM_REGVAL(AM_HAL_GPIO_PADREG(n)) & ~AM_HAL_GPIO_PADREG_M(n)))

#define AM_HAL_GPIO_PADREG_R(n)                                               \
    ((AM_REGVAL(AM_HAL_GPIO_PADREG(n)) & AM_HAL_GPIO_PADREG_M(n)) >>          \
     AM_HAL_GPIO_PADREG_S(n))

//*****************************************************************************
//
// CFG helper macros.
//
//*****************************************************************************
#define AM_HAL_GPIO_CFG(n)                                                    \
    (AM_REG_GPIOn(0) + AM_REG_GPIO_CFGA_O + ((n & 0xF8) >> 1))

#define AM_HAL_GPIO_CFG_S(n)                                                  \
    (((uint32_t)(n) % 8) << 2)

#define AM_HAL_GPIO_CFG_M(n)                                                  \
    ((uint32_t) 0x7 << AM_HAL_GPIO_CFG_S(n))

#define AM_HAL_GPIO_CFG_FIELD(n, configval)                                   \
    ((((uint32_t)(configval) & 0x700) >> 8) << AM_HAL_GPIO_CFG_S(n))

#define AM_HAL_GPIO_CFG_W(n, configval)                                       \
    AM_REGVAL(AM_HAL_GPIO_CFG(n)) =                                           \
        (AM_HAL_GPIO_CFG_FIELD(n, configval) |                                \
         (AM_REGVAL(AM_HAL_GPIO_CFG(n)) & ~AM_HAL_GPIO_CFG_M(n)))

#define AM_HAL_GPIO_CFG_R(n)                                                  \
    ((AM_REGVAL(AM_HAL_GPIO_CFG(n)) & AM_HAL_GPIO_CFG_M(n)) >>                \
     AM_HAL_GPIO_CFG_S(n))

//*****************************************************************************
//
// Polarity helper macros.
//
//*****************************************************************************
#define AM_HAL_GPIO_POL_S(n)                                                  \
    ((((uint32_t)(n) % 8) << 2) + 3)

#define AM_HAL_GPIO_POL_M(n)                                                  \
    ((uint32_t) 0x1 << AM_HAL_GPIO_POL_S(n))

#define AM_HAL_GPIO_POL_FIELD(n, polarity)                                    \
    (((uint32_t)(polarity) & 0x1) << AM_HAL_GPIO_POL_S(n))

#define AM_HAL_GPIO_POL_W(n, polarity)                                        \
    AM_REGVAL(AM_HAL_GPIO_CFG(n)) =                                           \
        (AM_HAL_GPIO_POL_FIELD(n, polarity) |                                 \
         (AM_REGVAL(AM_HAL_GPIO_CFG(n)) & ~AM_HAL_GPIO_POL_M(n)))

//*****************************************************************************
//
// RD helper macros.
//
//*****************************************************************************
#define AM_HAL_GPIO_RD_REG(n)                                                 \
    (AM_REG_GPIOn(0) + AM_REG_GPIO_RDA_O + (((uint32_t)(n) & 0x20) >> 3))

#define AM_HAL_GPIO_RD_S(n)                                                   \
    ((uint32_t)(n) % 32)

#define AM_HAL_GPIO_RD_M(n)                                                   \
    ((uint32_t) 0x1 << AM_HAL_GPIO_RD_S(n))

#define AM_HAL_GPIO_RD(n)                                                     \
    AM_REGVAL(AM_HAL_GPIO_RD_REG(n))

//*****************************************************************************
//
// WT helper macros.
//
//*****************************************************************************
#define AM_HAL_GPIO_WT_REG(n)                                                 \
    (AM_REG_GPIOn(0) + AM_REG_GPIO_WTA_O + (((uint32_t)(n) & 0x20) >> 3))

#define AM_HAL_GPIO_WT_S(n)                                                   \
    ((uint32_t)(n) % 32)

#define AM_HAL_GPIO_WT_M(n)                                                   \
    ((uint32_t) 0x1 << AM_HAL_GPIO_WT_S(n))

#define AM_HAL_GPIO_WT(n)                                                     \
    AM_REGVAL(AM_HAL_GPIO_WT_REG(n))

//*****************************************************************************
//
// WTS helper macros.
//
//*****************************************************************************
#define AM_HAL_GPIO_WTS_REG(n)                                                \
    (AM_REG_GPIOn(0) + AM_REG_GPIO_WTSA_O + (((uint32_t)(n) & 0x20) >> 3))

#define AM_HAL_GPIO_WTS_S(n)                                                  \
    ((uint32_t)(n) % 32)

#define AM_HAL_GPIO_WTS_M(n)                                                  \
    ((uint32_t) 0x1 << AM_HAL_GPIO_WTS_S(n))

#define AM_HAL_GPIO_WTS(n)                                                    \
    AM_REGVAL(AM_HAL_GPIO_WTS_REG(n))

//*****************************************************************************
//
// WTC helper macros.
//
//*****************************************************************************
#define AM_HAL_GPIO_WTC_REG(n)                                                \
    (AM_REG_GPIOn(0) + AM_REG_GPIO_WTCA_O + (((uint32_t)(n) & 0x20) >> 3))

#define AM_HAL_GPIO_WTC_S(n)                                                  \
    ((uint32_t)(n) % 32)

#define AM_HAL_GPIO_WTC_M(n)                                                  \
    ((uint32_t) 0x1 << AM_HAL_GPIO_WTC_S(n))

#define AM_HAL_GPIO_WTC(n)                                                    \
    AM_REGVAL(AM_HAL_GPIO_WTC_REG(n))

//*****************************************************************************
//
// EN helper macros.
//
//*****************************************************************************
#define AM_HAL_GPIO_EN_REG(n)                                                 \
    (AM_REG_GPIOn(0) + AM_REG_GPIO_ENA_O + (((uint32_t)(n) & 0x20) >> 3))

#define AM_HAL_GPIO_EN_S(n)                                                   \
    ((uint32_t)(n) % 32)

#define AM_HAL_GPIO_EN_M(n)                                                   \
    ((uint32_t) 0x1 << AM_HAL_GPIO_EN_S(n))

#define AM_HAL_GPIO_EN(n)                                                     \
    AM_REGVAL(AM_HAL_GPIO_EN_REG(n))

//*****************************************************************************
//
// ENS helper macros.
//
//*****************************************************************************
#define AM_HAL_GPIO_ENS_REG(n)                                                \
    (AM_REG_GPIOn(0) + AM_REG_GPIO_ENSA_O + (((uint32_t)(n) & 0x20) >> 3))

#define AM_HAL_GPIO_ENS_S(n)                                                  \
    ((uint32_t)(n) % 32)

#define AM_HAL_GPIO_ENS_M(n)                                                  \
    ((uint32_t) 0x1 << AM_HAL_GPIO_ENS_S(n))

#define AM_HAL_GPIO_ENS(n)                                                    \
    AM_REGVAL(AM_HAL_GPIO_ENS_REG(n))

//*****************************************************************************
//
// ENC helper macros.
//
//*****************************************************************************
#define AM_HAL_GPIO_ENC_REG(n)                                                \
    (AM_REG_GPIOn(0) + AM_REG_GPIO_ENCA_O + (((uint32_t)(n) & 0x20) >> 3))

#define AM_HAL_GPIO_ENC_S(n)                                                  \
    ((uint32_t)(n) % 32)

#define AM_HAL_GPIO_ENC_M(n)                                                  \
    ((uint32_t) 0x1 << AM_HAL_GPIO_ENC_S(n))

#define AM_HAL_GPIO_ENC(n)                                                    \
    AM_REGVAL(AM_HAL_GPIO_ENC_REG(n))

//*****************************************************************************
//
//! @brief Configure the GPIO PAD MUX & GPIO PIN Configurations
//!  ÅäÖÃPAD¼Ä´æÆ÷£¬ÐèÒªÏÈÉèÖÃpadkey=0x73£¬ÅäÖÃÍêpadkey=0
//! @param ui32PinNumber - GPIO pin number.
//! @param ui32Config - Configuration options.
//!
//! This function applies the settings for a single GPIO. For a list of valid
//! options please see the top of this file (am_hal_gpio.h) and am_hal_pin.h.
//
//*****************************************************************************
#define am_hal_gpio_pin_config(ui32PinNumber, ui32Config)                     \
    do                                                                        \
    {                                                                         \
        if ( (int32_t)(ui32PinNumber) < 0 )                                   \
        {                                                                     \
            break;                                                            \
        }                                                                     \
                                                                              \
        AM_REGn(GPIO, 0, PADKEY) = AM_REG_GPIO_PADKEY_KEYVAL;                 \
                                                                              \
        AM_HAL_GPIO_CFG_W(ui32PinNumber, ui32Config);                         \
        AM_HAL_GPIO_PADREG_W(ui32PinNumber, ui32Config);                      \
                                                                              \
        AM_REGn(GPIO, 0, PADKEY) = 0;                                         \
    }                                                                         \
    while(0)

//*****************************************************************************
//
//! @brief Set the state of one GPIO polarity bit.
//!
//! @param ui32BitNum - GPIO number.
//! @param ui32Polarity - Desired state.
//!
//! This function sets the state of one GPIO polarity bit to a supplied value.
//! The ui32Polarity parameter should be one of the following values:
//!
//!     AM_HAL_GPIO_FALLING
//!     AM_HAL_GPIO_RISING
//!
//! @return None.
//
//*****************************************************************************
#define am_hal_gpio_int_polarity_bit_set(ui32PinNumber, ui32Polarity)         \
    do                                                                        \
    {                                                                         \
        if ( (int32_t)(ui32PinNumber) < 0 )                                   \
        {                                                                     \
            break;                                                            \
        }                                                                     \
                                                                              \
        AM_REGn(GPIO, 0, PADKEY) = AM_REG_GPIO_PADKEY_KEYVAL;                 \
        AM_HAL_GPIO_POL_W(ui32PinNumber, ui32Polarity);                       \
        AM_REGn(GPIO, 0, PADKEY) = 0;                                         \
    }                                                                         \
    while(0)

//*****************************************************************************
//
//! @brief Get the state of one GPIO from the INPUT READ REGISTER.
//!
//! @param ui32BitNum - GPIO number.
//!
//! This function retrieves the state of one GPIO from the INPUT READ
//! REGISTER.
//!
//! @return the state for the requested GPIO.
//
//*****************************************************************************
#define am_hal_gpio_input_bit_read(ui32BitNum)                                \
    ((AM_HAL_GPIO_RD(ui32BitNum) & AM_HAL_GPIO_RD_M(ui32BitNum)) != 0)

//*****************************************************************************
//
//! @brief Get the state of one GPIO in the DATA OUTPUT REGISTER
//!
//! @param ui32BitNum - GPIO number.
//!
//! This function retrieves the state of one GPIO in the DATA OUTPUT REGISTER.
//!
//! @return the state for the requested GPIO or -1 for error.
//
//*****************************************************************************
#define am_hal_gpio_out_bit_read(ui32BitNum)                                  \
    ((AM_HAL_GPIO_WT(ui32BitNum) & AM_HAL_GPIO_WT_M(ui32BitNum)) != 0)

//*****************************************************************************
//
//! @brief Set the output state high for one GPIO.
//!
//! @param ui32BitNum - GPIO number.
//!
//! This function sets the output state to high for one GPIO.
//!
//! @return None.
//
//*****************************************************************************
#define am_hal_gpio_out_bit_set(ui32BitNum)                                   \
    AM_HAL_GPIO_WTS(ui32BitNum) = AM_HAL_GPIO_WTS_M(ui32BitNum)

//*****************************************************************************
//
//! @brief Sets the output state to low for one GPIO.
//!
//! @param ui32BitNum - GPIO number.
//!
//! This function sets the output state to low for one GPIO.
//!
//! @return None.
//
//*****************************************************************************
#define am_hal_gpio_out_bit_clear(ui32BitNum)                                 \
    AM_HAL_GPIO_WTC(ui32BitNum) = AM_HAL_GPIO_WTC_M(ui32BitNum)

//*****************************************************************************
//
//! @brief Sets the output state to ui32Value for one GPIO.
//!
//! @param ui32BitNum - GPIO number.
//! @param ui32Value - Desired output state.
//!
//! This function sets the output state to ui32Value for one GPIO.
//!
//! @return None.
//
//*****************************************************************************
#define am_hal_gpio_out_bit_replace(ui32BitNum, ui32Value)                    \
    AM_HAL_GPIO_WT(ui32BitNum) = ui32Value ?                                  \
        (AM_HAL_GPIO_WT(ui32BitNum) | AM_HAL_GPIO_WT_M(ui32BitNum)) :         \
        (AM_HAL_GPIO_WT(ui32BitNum) & ~AM_HAL_GPIO_WT_M(ui32BitNum))

//*****************************************************************************
//
//! @brief Toggle the output state of one GPIO.
//!
//! @param ui32BitNum - GPIO number.
//!
//! This function toggles the output state of one GPIO.
//!
//! @return None.
//
//*****************************************************************************
#define am_hal_gpio_out_bit_toggle(ui32BitNum)                                \
    AM_HAL_GPIO_WT(ui32BitNum) = (AM_HAL_GPIO_WT(ui32BitNum) ^                \
                                  AM_HAL_GPIO_WT_M(ui32BitNum))

//*****************************************************************************
//
//! @brief Sets the output enable for one GPIO.
//!
//! @param ui32BitNum - GPIO number.
//!
//! This function sets the output enable for one GPIO.
//!
//! @return None.
//
//*****************************************************************************
#define am_hal_gpio_out_enable_bit_set(ui32BitNum)                            \
    AM_HAL_GPIO_ENS(ui32BitNum) = AM_HAL_GPIO_ENS_M(ui32BitNum)

//*****************************************************************************
//
//! @brief Clears the output enable for one GPIO.
//!
//! @param ui32BitNum - GPIO number.
//!
//! This function clears the output enable for one GPIO.
//!
//! @return None.
//
//*****************************************************************************
#define am_hal_gpio_out_enable_bit_clear(ui32BitNum)                          \
    AM_HAL_GPIO_ENC(ui32BitNum) = AM_HAL_GPIO_ENC_M(ui32BitNum)

//*****************************************************************************
//
// Function pointer type for GPIO interrupt handlers.
//
//*****************************************************************************
typedef void (*am_hal_gpio_handler_t)(void);

//*****************************************************************************
//
// External function prototypes
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

extern uint32_t am_hal_gpio_pin_config_read(uint32_t ui32PinNumber);
extern uint64_t am_hal_gpio_input_read(void);
extern uint64_t am_hal_gpio_out_read(void);
extern uint32_t am_hal_gpio_out_enable_bit_get(uint32_t ui32BitNum);
extern uint64_t am_hal_gpio_out_enable_get(void);
extern void am_hal_gpio_int_enable(uint64_t ui64Interrupt);
extern uint64_t am_hal_gpio_int_enable_get(void);
extern void am_hal_gpio_int_disable(uint64_t ui64Interrupt);
extern void am_hal_gpio_int_clear(uint64_t ui64Interrupt);
extern void am_hal_gpio_int_set(uint64_t ui64Interrupt);
extern uint64_t am_hal_gpio_int_status_get(bool bEnabledOnly);
extern void am_hal_gpio_int_service(uint64_t ui64Status);
extern void am_hal_gpio_int_register(uint32_t ui32GPIONumber,
                                     am_hal_gpio_handler_t pfnHandler);

extern bool am_hal_gpio_int_polarity_bit_get(uint32_t ui32BitNum);

#ifdef __cplusplus
}
#endif

#endif  // AM_HAL_GPIO_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
