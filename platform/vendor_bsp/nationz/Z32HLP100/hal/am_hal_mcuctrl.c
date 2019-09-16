/** ****************************************************************************
 * @copyright                  Nationz Co.Ltd
 *               Copyright (c) 2013－2018 All Rights Reserved
 *******************************************************************************
 * @file     am_hal_mcuctrl.c
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Functions for interfacing with the MCUCTRL.
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup mcuctrl MCU Control (MCUCTRL)
 * @ingroup hal
 * @{
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_Z32HLP.h"

//*****************************************************************************
//
// Global Variables.
//
//*****************************************************************************
const uint32_t am_hal_mcuctrl_flash_size[] =
{
    1 << 15,    /* 0x0 0x008000   32k */
    1 << 16,    /* 0x1 0x010000   64k */
    1 << 17,    /* 0x2 0x020000  128k */
    1 << 18,    /* 0x3 0x040000  256k */
    1 << 19,    /* 0x4 0x080000  512k */
    1 << 20,    /* 0x5 0x100000 1024k */
    1 << 21,    /* 0x6 0x200000 2048k */
    0,          /* 0x7 Invalid */
    0,          /* 0x8 Invalid */
    0,          /* 0x9 Invalid */
    0,          /* 0xA Invalid */
    0,          /* 0xB Invalid */
    0,          /* 0xC Invalid */
    0,          /* 0xD Invalid */
    0,          /* 0xE Invalid */
    1 << 14     /* 0xF 0x004000   16k */
};

const uint32_t am_hal_mcuctrl_sram_size[] =
{
    1 << 15,    /* 0x0 0x008000   32k */
    1 << 16,    /* 0x1 0x010000   64k */
    1 << 17,    /* 0x2 0x020000  128k */
    1 << 18,    /* 0x3 0x040000  256k */
    1 << 19,    /* 0x4 0x080000  512k */
    1 << 20,    /* 0x5 0x100000 1024k */
    1 << 21,    /* 0x6 0x200000 2048k */
    0,          /* 0x7 Invalid */
    0,          /* 0x8 Invalid */
    0,          /* 0x9 Invalid */
    0,          /* 0xA Invalid */
    0,          /* 0xB Invalid */
    0,          /* 0xC Invalid */
    0,          /* 0xD Invalid */
    0,          /* 0xE Invalid */
    1 << 14     /* 0xF 0x004000   16k */
};

//*****************************************************************************
//
//! @brief Gets all relevant device information.
//!
//! @param psDevice is a pointer to a structure that will be used to store all
//! device info.
//!
//! This function gets the device part number, chip IDs, and revision and
//! stores them in the passed structure.
//!
//! @return None
//
//*****************************************************************************
void
am_hal_mcuctrl_device_info_get(am_hal_mcuctrl_device_t *psDevice)
{
    //
    // Read the Part Number.
    //
    psDevice->ui32ChipPN = AM_REG(MCUCTRL, CHIP_INFO);

    //
    // Read the Chip ID0.
    //
    psDevice->ui32ChipID0 = AM_REG(MCUCTRL, CHIPID0);

    //
    // Read the Chip ID1.
    //
    psDevice->ui32ChipID1 = AM_REG(MCUCTRL, CHIPID1);

    //
    // Read the Chip Revision.
    //
    psDevice->ui32ChipRev = AM_REG(MCUCTRL, CHIPREV);

    //
    // Qualified from Part Number.
    //
    psDevice->ui32Qualified = AM_BFR(MCUCTRL, CHIP_INFO, QUAL);

    //
    // Flash size from Part Number.
    //
    psDevice->ui32FlashSize =
        am_hal_mcuctrl_flash_size[AM_BFR(MCUCTRL, CHIP_INFO, FLASH)];

    //
    // SRAM size from Part Number.
    //
    psDevice->ui32SRAMSize =
        am_hal_mcuctrl_sram_size[AM_BFR(MCUCTRL, CHIP_INFO, RAM)];

    //
    // Now, let's look at the JEDEC info.
    // The full partnumber is 12 bits total, but is scattered across 2 registers.
    // Bits [11:8] are 0xE.
    // Bits [7:4] are 0xE for Z32HLP100 , 0xD for Z32HLP200 .
    // Bits [3:0] are defined differently for Z32HLP100  and Z32HLP200 .
    //   For Z32HLP100 , the low nibble is 0x0.
    //   For Z32HLP200 , the low nibble indicates flash and SRAM size.
    //
    psDevice->ui32JedecPN  = (AM_BFR(JEDEC, PID0, PNL8) << 0);
    psDevice->ui32JedecPN |= (AM_BFR(JEDEC, PID1, PNH4) << 8);

    //
    // JEPID is the JEP-106 Manufacturer ID Code, which is assigned to Nationz as
    //  0x1B, with parity bit is 0x9B.  It is 8 bits located across 2 registers.
    //
    psDevice->ui32JedecJEPID  = (AM_BFR(JEDEC, PID1, JEPIDL) << 0);
    psDevice->ui32JedecJEPID |= (AM_BFR(JEDEC, PID2, JEPIDH) << 4);

    //
    // CHIPREV is 8 bits located across 2 registers.
    //
    psDevice->ui32JedecCHIPREV  = (AM_BFR(JEDEC, PID2, CHIPREVH4) << 4);
    psDevice->ui32JedecCHIPREV |= (AM_BFR(JEDEC, PID3, CHIPREVL4) << 0);

    //
    // Let's get the Coresight ID (32-bits across 4 registers)
    // For Z32HLP100  and Z32HLP200 , it's expected to be 0xB105100D.
    //
    psDevice->ui32JedecCID  = (AM_BFR(JEDEC, CID3, CID) << 24);
    psDevice->ui32JedecCID |= (AM_BFR(JEDEC, CID2, CID) << 16);
    psDevice->ui32JedecCID |= (AM_BFR(JEDEC, CID1, CID) <<  8);
    psDevice->ui32JedecCID |= (AM_BFR(JEDEC, CID0, CID) <<  0);
}

//*****************************************************************************
//
//! @brief Enables the fault capture registers.
//!
//! This function enables the DCODEFAULTADDR and ICODEFAULTADDR registers.
//!
//! @return None
//
//*****************************************************************************
void
am_hal_mcuctrl_fault_capture_enable(void)
{
    //
    // Enable the Fault Capture registers.
    //
    AM_BFW(MCUCTRL, FAULTCAPTUREEN, ENABLE, 1);
}

//*****************************************************************************
//
//! @brief Disables the fault capture registers.
//!
//! This function disables the DCODEFAULTADDR and ICODEFAULTADDR registers.
//!
//! @return None
//
//*****************************************************************************
void
am_hal_mcuctrl_fault_capture_disable(void)
{
    //
    // Disable the Fault Capture registers.
    //
    AM_BFW(MCUCTRL, FAULTCAPTUREEN, ENABLE, 0);
}

//*****************************************************************************
//
//! @brief Gets the fault status and capture registers.
//!
//! @param psFault is a pointer to a structure that will be used to store all
//! fault info.
//!
//! This function gets the status of the ICODE, DCODE, and SYS bus faults and
//! the addresses associated with the fault.
//!
//! @return None
//
//*****************************************************************************
void
am_hal_mcuctrl_fault_status(am_hal_mcuctrl_fault_t *psFault)
{
    uint32_t ui32FaultStat;

    //
    // Read the Fault Status Register.
    //
    ui32FaultStat = AM_REG(MCUCTRL, FAULTSTATUS);
    psFault->bICODE = (ui32FaultStat & AM_REG_MCUCTRL_FAULTSTATUS_ICODE_M);
    psFault->bDCODE = (ui32FaultStat & AM_REG_MCUCTRL_FAULTSTATUS_DCODE_M);
    psFault->bSYS = (ui32FaultStat & AM_REG_MCUCTRL_FAULTSTATUS_SYS_M);

    //
    // Read the DCODE fault capture address register.
    //
    psFault->ui32DCODE = AM_REG(MCUCTRL, DCODEFAULTADDR);

    //
    // Read the ICODE fault capture address register.
    //
    psFault->ui32ICODE |= AM_REG(MCUCTRL, ICODEFAULTADDR);

    //
    // Read the ICODE fault capture address register.
    //
    psFault->ui32SYS |= AM_REG(MCUCTRL, SYSFAULTADDR);
}

//*****************************************************************************
//
//! @brief Set power state of the flash.
//!
//! @param ui32FlashPower is the desired flash power configuration.
//!
//! This function sets the device power state for the flash banks.
//!
//! Valid values for ui32FlashPower are:
//!
//!     AM_HAL_MCUCTRL_FLASH_POWER_DOWN_NONE
//!     AM_HAL_MCUCTRL_FLASH_POWER_DOWN_0
//!     AM_HAL_MCUCTRL_FLASH_POWER_DOWN_1
//!     AM_HAL_MCUCTRL_FLASH_POWER_DOWN_ALL
//!
//! @return None
//
//*****************************************************************************
void
am_hal_mcuctrl_flash_power_set(uint32_t ui32FlashPower)
{
    //
    // Write desired flash power state.
    //
    AM_REG(MCUCTRL, FLASHPWRDIS) = ui32FlashPower;
}

//*****************************************************************************
//
//! @brief Set power state of the SRAM.
//!
//! @param ui32SRAMPower is the desired SRAM power configuration.
//! @param ui32SRAMPowerDeepSleep is the desired SRAM power configuration in
//! deep sleep.
//!
//! This function sets the device power state for the SRAM banks.
//!
//! Valid values for ui32SRAMPower and ui32SRAMPowerDeepSleep are:
//!
//!     AM_HAL_MCUCTRL_SRAM_POWER_DOWN_NONE
//!     AM_HAL_MCUCTRL_SRAM_POWER_DOWN_1
//!     AM_HAL_MCUCTRL_SRAM_POWER_DOWN_2
//!     AM_HAL_MCUCTRL_SRAM_POWER_DOWN_3
//!     AM_HAL_MCUCTRL_SRAM_POWER_DOWN_4
//!     AM_HAL_MCUCTRL_SRAM_POWER_DOWN_5
//!     AM_HAL_MCUCTRL_SRAM_POWER_DOWN_6
//!     AM_HAL_MCUCTRL_SRAM_POWER_DOWN_7
//!     AM_HAL_MCUCTRL_SRAM_POWER_DOWN_ALL
//!
//! @return None
//
//*****************************************************************************
void
am_hal_mcuctrl_sram_power_set(uint32_t ui32SRAMPower,
                              uint32_t ui32SRAMPowerDeepSleep)
{
    //
    // Write desired SRAM power state.
    //
    AM_REG(MCUCTRL, SRAMPWRDIS) = ui32SRAMPower;

    //
    // Write desired SRAM deep sleep power state.
    //
    AM_REG(MCUCTRL, SRAMPWDINSLEEP) = ui32SRAMPowerDeepSleep;
}

//*****************************************************************************
//
//! @brief Enable the Bandgap.
//!
//! This function enables the Bandgap.
//!
//! @return None
//
//*****************************************************************************
void
am_hal_mcuctrl_bandgap_enable(void)
{
    //
    // Enable the Bandgap in the MCUCTRL.
    //
    AM_REG(MCUCTRL, BANDGAPEN) = AM_REG_MCUCTRL_BANDGAPEN_BGPEN_M;
}

//*****************************************************************************
//
//! @brief Disable the Bandgap.
//!
//! This function disables the Bandgap.
//!
//! @return None
//
//*****************************************************************************
void
am_hal_mcuctrl_bandgap_disable(void)
{
    //
    // Disable the Bandgap in the MCUCTRL.
    //
    AM_REG(MCUCTRL, BANDGAPEN) = ~AM_REG_MCUCTRL_BANDGAPEN_BGPEN_M;
}

//*****************************************************************************
//
//! @brief Enable the core and memory buck converters.
//!
//! This function enables the core and memory buck converters.
//!
//! @return None
//   BUCK降压电路使能
//*****************************************************************************
void
am_hal_mcuctrl_bucks_enable(void)
{
    //
    // Enable the core buck converter in the MCUCTRL.
    //
    AM_BFW(MCUCTRL, SUPPLYSRC, COREBUCKEN, 1);

    //
    // Enable the SRAM buck converter in the MCUCTRL.
    //
    AM_BFW(MCUCTRL, SUPPLYSRC, MEMBUCKEN, 1);

    //
    // Poll until core buck is enabled.
    //
    while( !AM_BFR(MCUCTRL, SUPPLYSTATUS, COREBUCKON) );

    //
    // Poll until SRAM buck is enabled.
    //
    while( !AM_BFR(MCUCTRL, SUPPLYSTATUS, MEMBUCKON) );
}

//*****************************************************************************
//
//! @brief Disable the core and memory buck converters.
//!
//! This function disables the core and memory buck converters.
//!
//! @return None
//
//*****************************************************************************
void
am_hal_mcuctrl_bucks_disable(void)
{
    //
    // Disable the core buck converter in the MCUCTRL.
    //
    AM_BFW(MCUCTRL, SUPPLYSRC, COREBUCKEN, 0);

    //
    // Disable the SRAM buck converter in the MCUCTRL.
    //
    AM_BFW(MCUCTRL, SUPPLYSRC, MEMBUCKEN, 0);
}


//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
