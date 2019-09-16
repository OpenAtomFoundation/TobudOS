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
 * @file     am_hal_flash.h
 * @author   
 * @date     
 * @version  v1.0
 * @brief    Functions for performing Flash operations.
 *
 * @addtogroup hal Hardware Abstraction Layer (HAL)
 * @addtogroup flash Flash
 * @ingroup hal
 * @{
 ******************************************************************************/
#ifndef AM_HAL_FLASH_H
#define AM_HAL_FLASH_H

#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
//
// Flash Program keys.
//
//*****************************************************************************
#define AM_HAL_FLASH_PROGRAM_KEY            0x12344321
#define AM_HAL_FLASH_OTP_KEY                0x87655678

//*****************************************************************************
//
// Some helpful flash values and macros.
//
//*****************************************************************************
#define AM_HAL_FLASH_ADDR                   0x00000000
#define AM_HAL_FLASH_PAGE_SIZE              ( 2 * 1024 )
#define AM_HAL_FLASH_INFO_SIZE              AM_HAL_FLASH_PAGE_SIZE
#define AM_HAL_FLASH_BLOCK_SIZE             ( 256 * 1024 )
#define AM_HAL_FLASH_BLOCK_PAGES            ( AM_HAL_FLASH_BLOCK_SIZE / AM_HAL_FLASH_PAGE_SIZE )
#define AM_HAL_FLASH_INSTANCE_SIZE          AM_HAL_FLASH_BLOCK_SIZE
#define AM_HAL_FLASH_INSTANCE_PAGES         AM_HAL_FLASH_BLOCK_PAGES
#define AM_HAL_FLASH_TOTAL_SIZE             ( AM_HAL_FLASH_BLOCK_SIZE * 2 )
#define AM_HAL_FLASH_LARGEST_VALID_ADDR     ( AM_HAL_FLASH_ADDR + AM_HAL_FLASH_TOTAL_SIZE - 1 )

// Convert an absolute flash address to a block (instance)
#define AM_HAL_FLASH_ADDR2BLOCK(addr)       ( ( addr >> 18 ) & 1 )
#define AM_HAL_FLASH_ADDR2INST(addr)        ( ( addr >> 18 ) & 1 )
// Convert an absolute flash address to a page number relative to the block
#define AM_HAL_FLASH_ADDR2PAGE(addr)        ( ( addr >> 11 ) & 0x7F )
// Convert an absolute flash address to an absolute page number
#define AM_HAL_FLASH_ADDR2ABSPAGE(addr)     ( addr >> 11 )

//*****************************************************************************
//
// Structure of function pointers to helper functions for invoking various
// flash operations.
//
//*****************************************************************************
typedef struct am_hal_flash_helper_struct
{
    int  (*am_hal_flash_mass_erase)(uint32_t, uint32_t);
    int  (*am_hal_flash_page_erase)(uint32_t, uint32_t, uint32_t);
    int  (*am_hal_flash_program_main)(uint32_t,  const uint32_t *,
                                      uint32_t*, uint32_t);
    int  (*am_hal_flash_program_otp)(uint32_t,   uint32_t,
                                     const uint32_t*,  uint32_t, uint32_t);
    void (*am_hal_flash_program_main_sram)(void);
    void (*am_hal_flash_program_otp_sram)(void);
    void (*am_hal_flash_erase_main_pages_sram)(void);
    void (*am_hal_flash_mass_erase_sram)(void);
} g_am_hal_flash_t;
extern g_am_hal_flash_t g_am_hal_flash;

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Function prototypes for the helper functions
//
//*****************************************************************************
extern void am_hal_flash_delay(uint32_t ui32Iterations);
extern int  am_hal_flash_mass_erase(uint32_t ui32Value, uint32_t ui32FlashBlk);
extern int  am_hal_flash_page_erase(uint32_t ui32Value, uint32_t ui32FlashBlk,
                                    uint32_t ui32PageNum);

extern int  am_hal_flash_program_otp(uint32_t ui32Value, uint32_t ui32FlashBlk,
                                     const uint32_t *pui32Src,
                                     uint32_t ui32Offset,
                                     uint32_t ui32NumWords);

extern int  am_hal_flash_program_main(uint32_t value, const uint32_t *pSrc,
                                      uint32_t *pDst, uint32_t  NumberOfWords);
// SRAM variants
extern void am_hal_flash_erase_main_pages_sram(void);
extern void am_hal_flash_mass_erase_sram(void);
extern void am_hal_flash_program_otp_sram(void);
extern void am_hal_flash_program_main_sram(void);

// SRAM resident reader function for OTP can't be in FLASH
uint32_t am_hal_flash_load_ui32(uint32_t ui32Address);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_FLASH_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
