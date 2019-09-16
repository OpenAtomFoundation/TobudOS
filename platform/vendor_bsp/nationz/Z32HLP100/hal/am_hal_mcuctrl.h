/** ----------------------------------------------------------------------------
 *         Nationz Technology Software Support  -  NATIONZ  -
 * -----------------------------------------------------------------------------
 * Copyright (c) 2013гн2018, Nationz Corporation  All rights reserved.
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
 *        Copyright (c) 2013гн2018 All Rights Reserved 
 *******************************************************************************
 * @file     am_hal_mcuctrl.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Functions for accessing and configuring the MCUCTRL.
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup mcuctrl MCU Control (MCUCTRL)
 * @ingroup hal
 * @{
 ******************************************************************************/
#ifndef AM_HAL_MCUCTRL_H
#define AM_HAL_MCUCTRL_H

//*****************************************************************************
//
// Z32HLP100  Number Decode.
//
//*****************************************************************************
extern const uint32_t am_hal_mcuctrl_flash_size[];
extern const uint32_t am_hal_mcuctrl_sram_size[];

//*****************************************************************************
//
// FLASH Bank Power defines.
//
//*****************************************************************************
#define AM_HAL_MCUCTRL_FLASH_POWER_DOWN_NONE    0x0
#define AM_HAL_MCUCTRL_FLASH_POWER_DOWN_0       0x1
#define AM_HAL_MCUCTRL_FLASH_POWER_DOWN_1       0x2
#define AM_HAL_MCUCTRL_FLASH_POWER_DOWN_ALL     0x3

//*****************************************************************************
//
// SRAM Bank Power defines.
//
//*****************************************************************************
#define AM_HAL_MCUCTRL_SRAM_POWER_DOWN_NONE    0x0
#define AM_HAL_MCUCTRL_SRAM_POWER_DOWN_0       0x1
#define AM_HAL_MCUCTRL_SRAM_POWER_DOWN_1       0x2
#define AM_HAL_MCUCTRL_SRAM_POWER_DOWN_2       0x4
#define AM_HAL_MCUCTRL_SRAM_POWER_DOWN_3       0x8
#define AM_HAL_MCUCTRL_SRAM_POWER_DOWN_4       0x10
#define AM_HAL_MCUCTRL_SRAM_POWER_DOWN_5       0x20
#define AM_HAL_MCUCTRL_SRAM_POWER_DOWN_6       0x40
#define AM_HAL_MCUCTRL_SRAM_POWER_DOWN_7       0x80
#define AM_HAL_MCUCTRL_SRAM_POWER_DOWN_ALL     0xFF



//*****************************************************************************
//
//! MCUCTRL device structure
//
//*****************************************************************************
typedef struct
{
    //
    //! Device part number. (BCD format)
    //
    uint32_t ui32ChipPN;

    //
    //! Unique Chip ID 0.
    //
    uint32_t ui32ChipID0;

    //
    //! Unique Chip ID 1.
    //
    uint32_t ui32ChipID1;

    //
    //! Chip Revision.
    //
    uint32_t ui32ChipRev;

    //
    //! Vendor ID.
    //
    uint32_t ui32VendorID;

    //
    //! Qualified chip.
    //
    uint32_t ui32Qualified;

    //
    //! Flash Size.
    //
    uint32_t ui32FlashSize;

    //
    //! SRAM Size.
    //
    uint32_t ui32SRAMSize;

    //
    // JEDEC chip info
    //
    uint32_t ui32JedecPN;
    uint32_t ui32JedecJEPID;
    uint32_t ui32JedecCHIPREV;
    uint32_t ui32JedecCID;
}
am_hal_mcuctrl_device_t;

//*****************************************************************************
//
//! MCUCTRL fault structure
//
//*****************************************************************************
typedef struct
{
    //
    //! ICODE bus fault occurred.
    //
    bool bICODE;

    //
    //! ICODE bus fault address.
    //
    uint32_t ui32ICODE;

    //
    //! DCODE bus fault occurred.
    //
    bool bDCODE;

    //
    //! DCODE bus fault address.
    //
    uint32_t ui32DCODE;

    //
    //! SYS bus fault occurred.
    //
    bool bSYS;

    //
    //! SYS bus fault address.
    //
    uint32_t ui32SYS;
}
am_hal_mcuctrl_fault_t;

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************
extern void am_hal_mcuctrl_device_info_get(am_hal_mcuctrl_device_t *psDevice);
extern void am_hal_mcuctrl_fault_capture_enable(void);
extern void am_hal_mcuctrl_fault_capture_disable(void);
extern void am_hal_mcuctrl_fault_status(am_hal_mcuctrl_fault_t *psFault);
extern void am_hal_mcuctrl_flash_power_set(uint32_t ui32FlashPower);
extern void am_hal_mcuctrl_sram_power_set(uint32_t ui32SRAMPower,
                                         uint32_t ui32SRAMPowerDeepSleep);
extern void am_hal_mcuctrl_bandgap_enable(void);
extern void am_hal_mcuctrl_bandgap_disable(void);
extern void am_hal_mcuctrl_bucks_enable(void);
extern void am_hal_mcuctrl_bucks_disable(void);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_MCUCTRL_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
