/** ****************************************************************************
 * @copyright                  Nationz Co.Ltd
 *               Copyright (c) 2013гн2018 All Rights Reserved
 *******************************************************************************
 * @file     am_hal_clkgen.c
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Functions for interfacing with the CLKGEN.
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup clkgen Clock Generator (CLKGEN)
 * @ingroup hal
 * @{
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_Z32HLP.h"

//*****************************************************************************
//
//! @brief Select the clock divisor for the main system clock.
//!
//! @param ui32ClockSetting - The divisor value for the system clock.
//!
//! This function can be used to select the frequency of the main system clock.
//! The \e ui32ClockSetting parameter should be set to one of the following
//! values:
//!
//!     AM_HAL_CLKGEN_SYSCLK_24MHZ
//!     AM_HAL_CLKGEN_SYSCLK_12MHZ
//!     AM_HAL_CLKGEN_SYSCLK_8MHZ
//!     AM_HAL_CLKGEN_SYSCLK_6MHZ
//!     AM_HAL_CLKGEN_SYSCLK_4_8MHZ
//!     AM_HAL_CLKGEN_SYSCLK_4MHZ
//!     AM_HAL_CLKGEN_SYSCLK_3_4MHZ
//!     AM_HAL_CLKGEN_SYSCLK_3MHZ
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_clkgen_sysclk_select(uint32_t ui32ClockSetting)
{
    //
    // Unlock the clock control register.
    //
    AM_REG(CLKGEN, CLKKEY) = AM_REG_CLKGEN_CLKKEY_KEYVAL;

    //
    // Set the HFRC divisor to the user-selected value.
    //
    AM_REG(CLKGEN, CCTRL) = ui32ClockSetting;

    //
    // Lock the clock configuration registers.
    //
    AM_REG(CLKGEN, CLKKEY) = 0;
}

//*****************************************************************************
//
//! @brief Get the current system clock frequency.
//!
//! This function can be used to determine the frequency of the main system
//! clock. The return value is the system clock frequency measured in hertz.
//!
//! @return System clock frequency in Hz
//
//*****************************************************************************
uint32_t
am_hal_clkgen_sysclk_get(void)
{
    uint32_t ui32ClockSetting;

    //
    // Read the value of the clock divider.
    //
    ui32ClockSetting = AM_BFR(CLKGEN, CCTRL, CORESEL);

    switch ( ui32ClockSetting )
    {
        case AM_HAL_CLKGEN_SYSCLK_24MHZ:
            return 24000000;
        case AM_HAL_CLKGEN_SYSCLK_12MHZ:
            return 12000000;
        case AM_HAL_CLKGEN_SYSCLK_8MHZ:
            return 8000000;
        case AM_HAL_CLKGEN_SYSCLK_6MHZ:
            return 6000000;
        case AM_HAL_CLKGEN_SYSCLK_4_8MHZ:
            return 4800000;
        case AM_HAL_CLKGEN_SYSCLK_4MHZ:
            return 4000000;
        case AM_HAL_CLKGEN_SYSCLK_3_4MHZ:
            return 3428571;
        case AM_HAL_CLKGEN_SYSCLK_3MHZ:
            return 3000000;
        default:
            return 0xFFFFFFFF;
    }
}

//*****************************************************************************
//
//! @brief Enable selected CLKGEN Interrupts.
//!
//! Use this function to enable the interrupts.
//!
//! @param ui32Interrupt - Use the macro bit fields provided in am_hal_clkgen.h
//!
//! @return None
//
//*****************************************************************************
void
am_hal_clkgen_int_enable(uint32_t ui32Interrupt)
{
    //
    // Enable the interrupts.
    //
    AM_REG(CLKGEN, INTEN) |= ui32Interrupt;
}

//*****************************************************************************
//
//! @brief Return enabled CLKGEN Interrupts.
//!
//! Use this function to get all enabled CLKGEN interrupts.
//!
//! @return enabled CLKGEN interrupts.
//
//*****************************************************************************
uint32_t
am_hal_clkgen_int_enable_get(void)
{
    //
    // Return the enabled interrupts.
    //
    return AM_REG(CLKGEN, INTEN);
}

//*****************************************************************************
//
//! @brief Disable selected CLKGEN Interrupts.
//!
//! Use this function to disable the CLKGEN interrupts.
//!
//! @param ui32Interrupt - Use the macro bit fields provided in am_hal_clkgen.h
//!
//! @return None
//
//*****************************************************************************
void
am_hal_clkgen_int_disable(uint32_t ui32Interrupt)
{
    //
    // Disable the interrupts.
    //
    AM_REG(CLKGEN, INTEN) &= ~ui32Interrupt;
}

//*****************************************************************************
//
//! @brief Sets the interrupt status.
//!
//! @param ui32IntFlags interrupts to be enabled.
//!
//! This function sets the interrupts.
//!
//! Valid values for ui32IntFlags are:
//!
//!     AM_HAL_CLKGEN_INT_RTC_ALARM
//!     AM_HAL_CLKGEN_INT_XT_FAIL
//!     AM_HAL_CLKGEN_INT_AUTOCAL_COMPLETE
//!     AM_HAL_CLKGEN_INT AUTOCAL_FAIL
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_clkgen_int_set(uint32_t ui32Interrupt)
{
    //
    // Set the interrupt status.
    //
    AM_REG(CLKGEN, INTSET) = ui32Interrupt;
}

//*****************************************************************************
//
//! @brief Gets the interrupt configuration.
//!
//! @param bEnabledOnly - return the status of only the enabled interrupts.
//!
//! This function gets the currently configured interrupts.
//!
//! @return the configured interrupts.
//!
//! Possible values for the return are:
//!
//!     AM_HAL_CLKGEN_INT_RTC_ALARM
//!     AM_HAL_CLKGEN_INT_XT_FAIL
//!     AM_HAL_CLKGEN_INT_AUTOCAL_COMPLETE
//!     AM_HAL_CLKGEN_INT AUTOCAL_FAIL
//
//*****************************************************************************
uint32_t
am_hal_clkgen_int_status_get(bool bEnabledOnly)
{
    //
    // Return the status.
    //
    if ( bEnabledOnly )
    {
        uint32_t u32RetVal = AM_REG(CLKGEN, INTSTAT);
        u32RetVal &= AM_REG(CLKGEN, INTEN);
        return u32RetVal;
    }
    else
    {
        return AM_REG(CLKGEN, INTSTAT);
    }
}

//*****************************************************************************
//
//! @brief Clears the interrupts.
//!
//! @param ui32IntFlags interrupts to be cleared.
//!
//! This function clears the interrupts.
//!
//! Valid values for ui32IntFlags are:
//!
//!     AM_HAL_CLKGEN_INT_RTC_ALARM
//!     AM_HAL_CLKGEN_INT_XT_FAIL
//!     AM_HAL_CLKGEN_INT_AUTOCAL_COMPLETE
//!     AM_HAL_CLKGEN_INT AUTOCAL_FAIL
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_clkgen_int_clear(uint32_t ui32Interrupt)
{
    //
    // Clear the interrupts.
    //
    AM_REG(CLKGEN, INTCLR) = ui32Interrupt;
}

//*****************************************************************************
//
//! @brief Starts the desired oscillator(s) (OSC).
//!
//! @param ui32OscFlags oscillator(s) to start.
//!
//! This function starts the desired oscillator(s) (OSC).
//!
//! Valid values for ui32OscFlags are:
//!
//!     AM_HAL_CLKGEN_OSC_LFRC
//!     AM_HAL_CLKGEN_OSC_XT
//!
//! @return 0 None.
//
//*****************************************************************************
void
am_hal_clkgen_osc_start(uint32_t ui32OscFlags)
{
    //
    // Start the oscillator(s).
    //
    AM_REG(CLKGEN, OCTRL) &= ~ui32OscFlags;
}

//*****************************************************************************
//
//! @brief Stops the desired oscillator(s) (OSC).
//!
//! @param ui32OscFlags oscillator(s) to stop.
//!
//! This function stops the desired oscillator(s) (OSC).
//!
//! Valid values for ui32OscFlags are:
//!
//!     AM_HAL_CLKGEN_OSC_LFRC
//!     AM_HAL_CLKGEN_OSC_XT
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_clkgen_osc_stop(uint32_t ui32OscFlags)
{
    //
    // Stop the oscillator(s).
    //
    AM_REG(CLKGEN, OCTRL) |= ui32OscFlags;
}

//*****************************************************************************
//
//! @brief Enables the clock out signal.
//!
//! @param ui32Signal desired location for the clock out signal.
//!
//! This function enables the clock out signal. See am_reg_clkgen.h for
//! available signals.
//!
//! ie. AM_REG_CLKGEN_CLKOUT_CKSEL_HFRC
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_clkgen_clkout_enable(uint32_t ui32Signal)
{
    //
    // Enable the clock out on desired signal.
    //
    AM_REG(CLKGEN, CLKOUT) = AM_REG_CLKGEN_CLKOUT_CKEN_M | ui32Signal;
}

//*****************************************************************************
//
//! @brief Disables the clock out signal.
//!
//! This function disables the clock out signal.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_clkgen_clkout_disable(void)
{
    //
    // Disable the clock out.
    //
    AM_REG(CLKGEN, CLKOUT) = 0;
}

//*****************************************************************************
//
//! @brief Enable UART system clock.
//!
//! This function enables or disables the UART system clock.
//!
//! @param ui32Module must be 0 .
//! @param ui32UartEn is one of the following.
//!     AM_HAL_CLKGEN_UARTEN_DIS
//!     AM_HAL_CLKGEN_UARTEN_EN
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_clkgen_uarten_set(uint32_t ui32Module, uint32_t ui32UartEn)
{
    uint32_t ui32Mask;

    if ( (ui32Module >= AM_REG_UART_NUM_MODULES)        ||
         (ui32UartEn > AM_HAL_CLKGEN_UARTEN_EN) )
    {
        return;
    }

    ui32UartEn <<= (ui32Module * AM_HAL_CLKGEN_UARTEN_UARTENn_S(ui32Module));
    ui32Mask = ~(AM_HAL_CLKGEN_UARTEN_UARTENn_M(ui32Module));

    //
    // Begin critical section.
    //
    AM_CRITICAL_BEGIN_ASM

    //
    // Set the UART clock
    //
    AM_REG(CLKGEN, UARTEN) &= ui32Mask;
    AM_REG(CLKGEN, UARTEN) |= ui32UartEn;

    //
    // Begin critical section.
    //
    AM_CRITICAL_END_ASM
}

//*****************************************************************************
//
//! @brief Enables HFRC auto-adjustment at the specified interval.
//!
//! @param ui32Warmup - How long to give the HFRC to stabilize during each
//! calibration attempt.
//! @param ui32Frequency - How often the auto-adjustment should happen.
//!
//! This function enables HFRC auto-adjustment from an external crystal
//! oscillator even when the crystal is not normally being used.
//!
//! ui32Warmup should be one of the following values:
//!
//!     AM_REG_CLKGEN_HFADJ_HFWARMUP_1SEC
//!     AM_REG_CLKGEN_HFADJ_HFWARMUP_2SEC
//!
//! ui32Frequency should be one of the following values:
//!
//!     AM_REG_CLKGEN_HFADJ_HFADJCK_4SEC
//!     AM_REG_CLKGEN_HFADJ_HFADJCK_16SEC
//!     AM_REG_CLKGEN_HFADJ_HFADJCK_32SEC
//!     AM_REG_CLKGEN_HFADJ_HFADJCK_64SEC
//!     AM_REG_CLKGEN_HFADJ_HFADJCK_128SEC
//!     AM_REG_CLKGEN_HFADJ_HFADJCK_256SEC
//!     AM_REG_CLKGEN_HFADJ_HFADJCK_512SEC
//!     AM_REG_CLKGEN_HFADJ_HFADJCK_1024SEC
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_clkgen_hfrc_adjust_enable(uint32_t ui32Warmup, uint32_t ui32Frequency)
{
    //
    // Set the HFRC Auto-adjust register for the user's chosen settings. Assume
    // that the HFRC should be calibrated to 24 MHz and that the crystal is
    // running at 32.768 kHz.
    //
    AM_REG(CLKGEN, HFADJ) = (ui32Warmup |
                             ui32Frequency |
                             AM_REG_CLKGEN_HFADJ_HFXTADJ(732) |
                             AM_REG_CLKGEN_HFADJ_HFADJEN_EN);
}

//*****************************************************************************
//
//! @brief Disables HFRC auto-adjustment.
//!
//! This function disables HFRC auto-adjustment.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_clkgen_hfrc_adjust_disable(void)
{
    //
    // Disable the clock out.
    //
    AM_REG(CLKGEN, HFADJ) = (AM_REG_CLKGEN_HFADJ_HFXTADJ(732) |
                             AM_REG_CLKGEN_HFADJ_HFADJEN_DIS);
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
