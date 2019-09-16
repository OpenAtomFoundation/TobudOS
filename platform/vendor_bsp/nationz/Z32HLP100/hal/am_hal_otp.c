/** ****************************************************************************
 * @copyright                  Nationz Co.Ltd
 *               Copyright (c) 2013гн2018 All Rights Reserved
 *******************************************************************************
 * @file     am_hal_otp.c
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Functions for handling the OTP interface.
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup 
 * @ingroup hal
 * @{
 ******************************************************************************/
#include "am_mcu_Z32HLP.h"
#include "am_hal_flash.h"

//*****************************************************************************
//
//! @brief Check if debugger is currently locked out.
//!
//! @param None.
//!
//! Determine if the debugger is already locked out.
//!
//! @return non-zero if debugger is currently locked out.
//!     Specifically:
//!     0 = debugger is not locked out.
//!     1 = debugger is locked out.
//
//*****************************************************************************
int
am_hal_otp_is_debugger_lockedout(void)
{
    uint32_t u32Val, uBF;

    //
    // Get current value of the debugger port word
    //
    u32Val = am_hal_flash_load_ui32(AM_HAL_OTP_DBGRPROT_ADDR);

    //
    // Check if already locked out.
    //
    uBF = AM_READ_SM(AM_OTP_DBGR_LOCKOUT, u32Val);
    switch ( uBF )
    {
        case 0xF:
            return 0;
        case 0xA:
            return 1;
        default:
            return 1;
    }
}

//*****************************************************************************
//
//! @brief Lock out debugger access.
//!
//! @param None.
//!
//! This function locks out access by a debugger.
//!
//! @return 0 if lockout was successful or if lockout was already enabled.
//!         Low byte=0xff, byte 1 contains current value of lockout.
//!         Else, return value from HAL programming function.
//
//*****************************************************************************
int
am_hal_otp_debugger_lockout(void)
{
    uint32_t u32Val;
    int iRet;

    iRet = am_hal_otp_is_debugger_lockedout();
    if ( iRet == 1 )
    {
        return 0;
    }
    else if ( iRet != 0 )
    {
        //
        // Uh-oh, something's weird.
        // We're locked out but not in the expected way.
        // Return as an error with the current (non-0xF) value encoded in
        // byte 1.
        //
        return (iRet << 8) | 0xff;
    }

    u32Val = am_hal_flash_load_ui32(AM_HAL_OTP_DBGRPROT_ADDR);

    //
    // Modify the appropriate bitfield for debugger lockout.
    //
    u32Val &= ~AM_OTP_DBGR_LOCKOUT_M;
    u32Val |= AM_WRITE_SM(AM_OTP_DBGR_LOCKOUT, 0xA);
    iRet = am_hal_flash_program_otp(AM_HAL_FLASH_OTP_KEY,
                                    0,
                                    &u32Val,
                                    AM_HAL_OTP_DBGR_O / 4,
                                    1);
    return iRet;
}

//*****************************************************************************
//
//! @brief Lock out SRAM access.
//!
//! @param None.
//!
//! This function locks out access by a debugger to SRAM.
//!
//! @return 0 if lockout was successful or if lockout was already enabled.
//!         Low byte=0xff, byte 1 contains current value of lockout.
//!         Else, return value from HAL programming function.
//
//*****************************************************************************
int
am_hal_otp_sram_lockout(void)
{
    uint32_t u32Val;
    int iRet;

    //
    // Get current value of the debugger port word
    //
    u32Val = am_hal_flash_load_ui32(AM_HAL_OTP_DBGRPROT_ADDR);

    //
    // Check if SRAM already locked out.
    //
    if ( AM_READ_SM(AM_OTP_SRAM_LOCKOUT, u32Val) == 0xA )
    {
        //
        // Already done, return with no error.
        //
        return 0;
    }

    //
    // Check for invalid value (which is effectively already locked out).
    //
    if ( AM_READ_SM(AM_OTP_SRAM_LOCKOUT, u32Val) != 0xF )
    {
        //
        // Uh-oh, something's wrong.
        // Return as an error with the current (non-0xF) value encoded in
        // byte 1.
        //
        return (AM_READ_SM(AM_OTP_SRAM_LOCKOUT, u32Val) << 8) | 0xff;
    }

    //
    // Modify the appropriate bitfield for SRAM access lockout.
    //
    u32Val &= ~AM_OTP_SRAM_LOCKOUT_M;
    u32Val |= AM_WRITE_SM(AM_OTP_SRAM_LOCKOUT, 0xA);
    iRet = am_hal_flash_program_otp(AM_HAL_FLASH_OTP_KEY,
                                    0,
                                    &u32Val,
                                    AM_HAL_OTP_DBGR_O / 4,
                                    1);
    return iRet;
}

//*****************************************************************************
//
//! @brief Set copy (read) protection.
//!
//! @param @u32BegAddr The beginning address to be copy protected.
//!        @u32EndAddr The ending address to be copy protected.
//!
//! @note For Z32HLP100 , the u32BegAddr parameter should be on a 16KB boundary, and
//!       the u32EndAddr parameter should be on a (16KB-1) boundary. Otherwise
//!       both parameters will be truncated/expanded to do so.
//!       For example, if u32BegAddr=0x1000 and u32EndAddr=0xC200, the actual
//!       range that protected is: 0x0 - 0xFFFF.
//!
//! This function enables copy protection on a given flash address range.
//!
//! @return 0 if copy protection was successfully enabled.
//
//*****************************************************************************
int
am_hal_otp_set_copy_protection(uint32_t u32BegAddr, uint32_t u32EndAddr)
{
    int iRet;
    uint32_t u32BfMask, u32Val;

    //
    // Validate the parameters.
    //
    if ( (u32BegAddr > u32EndAddr)                                  ||
         (u32EndAddr > (AM_HAL_FLASH_ADDR + AM_HAL_FLASH_TOTAL_SIZE - 1)) )
    {
        //
        // Invalid arguments.
        //
        return 1;
    }

    //
    // Force given addresses to appropriate boundaries.
    //
    u32BegAddr &= ~(AM_HAL_OTP_CHUNKSIZE-1);
    u32EndAddr |= (AM_HAL_OTP_CHUNKSIZE-1);

    //
    // Create the bitmask for the protection word.
    //
    u32BfMask = AM_HAL_OTP_PROT_M(u32BegAddr, u32EndAddr);

    //
    // Now, set the mask in the copy-protection OTP.
    //
    iRet = am_hal_flash_program_otp(AM_HAL_FLASH_OTP_KEY,
                                    0,
                                    &u32BfMask,
                                    AM_HAL_OTP_COPYPROT_O / 4,
                                    1);

    //
    // Now, read it back and make sure we cleared the bits we intended to clear.
    //
    u32Val = am_hal_flash_load_ui32(AM_HAL_OTP_COPYPROT_ADDR);
    if ( (u32Val & u32BfMask) != u32BfMask )
    {
        //
        // Something went awry.  Not all the intended bits were set to 0
        //  during the programming cycle.  Return an error.
        //
        iRet = 0xff;
    }

    return iRet;
}

//*****************************************************************************
//
//! @brief Set write protection.
//!
//! @param @u32BegAddr The beginning address to be write protected.
//!        @u32EndAddr The ending address to be write protected.
//!
//! @note For Z32HLP100 , the u32BegAddr parameter should be on a 16KB boundary, and
//!       the u32EndAddr parameter should be on a (16KB-1) boundary. Otherwise
//!       both parameters will be truncated/expanded to do so.
//!       For example, if u32BegAddr=0x1000 and u32EndAddr=0xC200, the actual
//!       range that protected is: 0x0 - 0xFFFF.
//!
//! This function enables write protection on a given flash address range.
//!
//! @return 0 if write protection was successfully enabled.
//
//*****************************************************************************
int
am_hal_otp_set_write_protection(uint32_t u32BegAddr, uint32_t u32EndAddr)
{
    int iRet;
    uint32_t u32BfMask, u32Val;

    //
    // Validate the parameters.
    //
    if ( (u32BegAddr > u32EndAddr)                                  ||
         (u32EndAddr > (AM_HAL_FLASH_ADDR + AM_HAL_FLASH_TOTAL_SIZE - 1)) )
    {
        //
        // Invalid arguments.
        //
        return 1;
    }

    //
    // Force given addresses to appropriate boundaries.
    //
    u32BegAddr &= ~(AM_HAL_OTP_CHUNKSIZE-1);
    u32EndAddr |= (AM_HAL_OTP_CHUNKSIZE-1);

    //
    // Create the bitmask for the protection word.
    //
    u32BfMask = AM_HAL_OTP_PROT_M(u32BegAddr, u32EndAddr);

    //
    // Now, set the mask in the write-protection OTP.
    //
    iRet = am_hal_flash_program_otp(AM_HAL_FLASH_OTP_KEY,
                                    0,
                                    &u32BfMask,
                                    AM_HAL_OTP_WRITPROT_O / 4,
                                    1);

    //
    // Now, read it back and make sure we cleared the bits we intended to clear.
    //
    u32Val = am_hal_flash_load_ui32(AM_HAL_OTP_WRITPROT_ADDR);
    if ( (u32Val & u32BfMask) != u32BfMask )
    {
        //
        // Something went awry.  Not all the intended bits were set to 0
        //  during the programming cycle.  Return an error.
        //
        iRet = 0xff;
    }

    return iRet;
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
