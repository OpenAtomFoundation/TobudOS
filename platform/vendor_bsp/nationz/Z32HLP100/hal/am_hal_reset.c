/** ****************************************************************************
 * @copyright                  Nationz Co.Ltd
 *               Copyright (c) 2013гн2018 All Rights Reserved
 *******************************************************************************
 * @file     am_hal_reset.c
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Hardware abstraction layer for the Reset Generator module.
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup rstgen Reset Generator (RSTGEN)
 * @ingroup hal
 * @{
 ******************************************************************************/

#include "am_mcu_Z32HLP.h"

//*****************************************************************************
//
//! @brief Configure the Reset Generator.
//!
//! @param ui32Config - Or together the supplied macros to enable
//! configurations to obtain the desired reset generator settings.
//!
//! This function will set the reset generator's configuration register based on
//! the user's desired settings listed in the supplied arugment.
//!
//! @return None.
//
//*****************************************************************************
void
am_hal_reset_init(uint32_t ui32Config)
{
    //
    // Write the configuration to the reset generator
    //
    AM_REG(RSTGEN, CFG) = ui32Config;
}

//*****************************************************************************
//
//! @brief Issue a POR (Z32HLP100 's last stage interrupt).
//!
//! This function will issue a POR reset.
//! The Z32HLP100  chip has numerous stages of reset. POR is the last and is also
//! the reset invoked by the chip's reset pin, the watchdog timer, the AIRCR
//! reset, and the SWD debugger requested interrupt.
//!
//! The Debug Access Port in the M4 is not cleared by this reset.
//!
//! @return None.
//
//*****************************************************************************
void am_hal_reset_por(void)
{
    //
    // Write the POR key to the software POR register.
    //
    AM_REG(RSTGEN, SWPOR) =
           AM_REG_RSTGEN_SWPOR_SWPORKEY(AM_REG_RSTGEN_SWPOR_SWPORKEY_KEYVALUE);
}

//*****************************************************************************
//
//! @brief Issue a POI (Z32HLP100 's second stage interrupt).
//!
//! This function will issue a POI reset.
//! The Z32HLP100  chip has numerous stages of reset. POI is the second stage.
//! A few modules are reset by POI that are not reset by POR, notably POI
//! causes the shadow registers to be reloaded from the OTP. A full power
//! cycle or POI should be used after writing new flash, debug or SRAM
//! protection bits into the OTP for these protections to take effect.
//!
//! The Debug Access Port in the M4 is not cleared by this reset.
//!
//! @return None.
//
//*****************************************************************************
void am_hal_reset_poi(void)
{
    //
    // Write the POI key to the software POI register.
    //
    AM_REG(RSTGEN, SWPOI) =
           AM_REG_RSTGEN_SWPOI_SWPOIKEY(AM_REG_RSTGEN_SWPOI_SWPOIKEY_KEYVALUE);
}

//*****************************************************************************
//
//! @brief Retrieve the status bits from the reset generator.
//!
//! This function will get the status bits from the reset generator.
//! These bits are sticky and show the accumulation of reset types that the
//! Z32HLP100  chip has experienced since power on. One should clear these out
//! after reading them.
//!
//! @return None.
//
//*****************************************************************************
uint32_t am_hal_reset_status_get(void)
{
    //
    // Retrieve the reset generator status bits
    //
    return AM_REG(RSTGEN, STAT);
}

//*****************************************************************************
//
//! @brief Clear ALL of the status bits in the reset generator.
//!
//! This function will clear all status bits in the reset generator status.
//!
//! @return None.
//
//*****************************************************************************
void am_hal_reset_status_clear(void)
{
    AM_REG(RSTGEN, CLRSTAT) = AM_REG_RSTGEN_CLRSTAT_CLRSTAT(1);
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
