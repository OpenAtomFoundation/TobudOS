/** ****************************************************************************
 * @copyright                  Nationz Co.Ltd
 *               Copyright (c) 2013гн2018 All Rights Reserved
 *******************************************************************************
 * @file     am_hal_sysctrl.c
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Functions for interfacing with the M4F system control registers
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup sysctrl System Control (SYSCTRL)
 * @ingroup hal
 * @{
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_Z32HLP.h"

//*****************************************************************************
//
//! @brief Place the core into sleep or deepsleep.
//!
//! @param bSleepDeep - False for Normal or True Deep sleep.
//!
//! This function puts the MCU to sleep or deepsleep depending on bSleepDeep.
//!
//! Valid values for bSleepDeep are:
//!
//!     AM_HAL_SYSCTRL_SLEEP_NORMAL
//!     AM_HAL_SYSCTRL_SLEEP_DEEP
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_sysctrl_sleep(bool bSleepDeep)
{
    //
    // Sleep or Deepsleep?
    //
    if (bSleepDeep)
    {
        //
        // If TPIU is not enabled, go to deepsleep.
        // else go to normal sleep.
        //
        if ((AM_REG(MCUCTRL, TPIUCTRL) & ~AM_REG_MCUCTRL_TPIUCTRL_ENABLE_EN) ==
            AM_REG_MCUCTRL_TPIUCTRL_ENABLE_DIS)
        {
            AM_REG(SYSCTRL, SCR) |= AM_REG_SYSCTRL_SCR_SLEEPDEEP_M;
        }
        else
        {
            AM_REG(SYSCTRL, SCR) &= ~AM_REG_SYSCTRL_SCR_SLEEPDEEP_M;
        }
    }
    else
    {
        AM_REG(SYSCTRL, SCR) &= ~AM_REG_SYSCTRL_SCR_SLEEPDEEP_M;
    }

    //
    // Go to sleep.
    //
#if defined(__ARMCC_VERSION)
    __wfi();            // ARM/Keil intrinsic
#else
    __asm("    wfi");   // GCC version
#endif
}

//*****************************************************************************
//
//! @brief Place the core into the deepest sleep state possible
//!
//! This function puts the MCU to sleep or deepsleep depending on which
//! peripherals are on. If the UART or either IOM module is enabled, the MCU
//! will be placed into normal sleep. Otherwise, the MCU will go to deep sleep.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_sysctrl_sleep_auto(void)
{
    //
    // If any of the HFRC peripherals are on, go to normal sleep. Otherwise go
    // to deep sleep.
    //
    if (AM_BFM(MCUCTRL, TPIUCTRL, ENABLE) || AM_BFMn(IOMSTR, 0, CFG, IFCEN) ||
        AM_BFMn(IOMSTR, 1, CFG, IFCEN) || AM_REG(CLKGEN, UARTEN))
    {
        AM_BFW(SYSCTRL, SCR, SLEEPDEEP, 1);
    }
    else
    {
        AM_BFW(SYSCTRL, SCR, SLEEPDEEP, 0);
    }

    //
    // Go to sleep.
    //
#if defined(__ARMCC_VERSION)
    __wfi();            // ARM/Keil intrinsic
#else
    __asm("    wfi");   // GCC version
#endif
}
//*****************************************************************************
//
//! @brief Enable the floating point module.
//!
//! Call this function to enable the ARM hardware floating point module.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_sysctrl_fpu_enable(void)
{
    //
    // Enable access to the FPU in both privileged and user modes.
    // NOTE: Write 0s to all reserved fields in this register.
    //
    AM_REG(SYSCTRL, CPACR) = (AM_REG_SYSCTRL_CPACR_CP11(0x3) |
                              AM_REG_SYSCTRL_CPACR_CP10(0x3));
}

//*****************************************************************************
//
//! @brief Disable the floating point module.
//!
//! Call this function to disable the ARM hardware floating point module.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_sysctrl_fpu_disable(void)
{
    //
    // Disable access to the FPU in both privileged and user modes.
    // NOTE: Write 0s to all reserved fields in this register.
    //
    AM_REG(SYSCTRL, CPACR) = 0x00000000                     &
                             ~(AM_REG_SYSCTRL_CPACR_CP11(0x3) |
                               AM_REG_SYSCTRL_CPACR_CP10(0x3));
}

//*****************************************************************************
//
//! @brief Enable stacking of FPU registers on exception entry.
//!
//! @param bLazy - Set to "true" to enable "lazy stacking".
//!
//! This function allows the core to save floating-point information to the
//! stack on exception entry. Setting the bLazy option enables "lazy stacking"
//! for interrupt handlers.  Normally, mixing floating-point code and interrupt
//! driven routines causes increased interrupt latency, because the core must
//! save extra information to the stack upon exception entry. With the lazy
//! stacking option enabled, the core will skip the saving of floating-point
//! registers when possible, reducing average interrupt latency.
//!
//! @note This function should be called before the floating-point module is
//! used in interrupt-driven code. If it is not called, the core will not have
//! any way to save context information for floating-point variables on
//! exception entry.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_sysctrl_fpu_stacking_enable(bool bLazy)
{
    if (bLazy)
    {
        //
        // Enable automatic saving of FPU registers on exception entry, using lazy
        // context saving.
        //
        AM_REG(SYSCTRL, FPCCR) |= (AM_REG_SYSCTRL_FPCCR_ASPEN(0x1) |
                                   AM_REG_SYSCTRL_FPCCR_LSPEN(0x1));
    }
    else
    {
        //
        // Enable automatic saving of FPU registers on exception entry.
        //
        AM_REG(SYSCTRL, FPCCR) |= AM_REG_SYSCTRL_FPCCR_ASPEN(0x1);
    }
}

//*****************************************************************************
//
//! @brief Disable FPU register stacking on exception entry.
//!
//! This function disables all stacking of floating point registers for
//! interrupt handlers.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_sysctrl_fpu_stacking_disable(void)
{
    //
    // Enable automatic saving of FPU registers on exception entry, using lazy
    // context saving.
    //
    AM_REG(SYSCTRL, FPCCR) &= ~(AM_REG_SYSCTRL_FPCCR_ASPEN(0x1) |
                                AM_REG_SYSCTRL_FPCCR_LSPEN(0x1));
}

//*****************************************************************************
//
//! @brief Issue a system wide reset using the AIRCR bit in the M4 system ctrl.
//!
//! This function issues a system wide reset (Z32HLP100  POR level reset).
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_sysctrl_reset(void)
{
    //
    // Set the system reset bit in the AIRCR register
    //
    AM_REG(SYSCTRL, AIRCR) = AM_REG_SYSCTRL_AIRCR_VECTKEY(0x5FA) |
                             AM_REG_SYSCTRL_AIRCR_SYSRESETREQ(1);
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
