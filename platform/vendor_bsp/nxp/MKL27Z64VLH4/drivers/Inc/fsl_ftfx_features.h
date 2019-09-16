/*
* The Clear BSD License
* Copyright 2017-2018 NXP
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted (subject to the limitations in the
* disclaimer below) provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer in the
*   documentation and/or other materials provided with the distribution.
*
* * Neither the name of the copyright holder nor the names of its
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
* GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
* HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
* IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#ifndef _FSL_FTFX_FEATURES_H_
#define _FSL_FTFX_FEATURES_H_

#if (defined(BL_TARGET_FLASH) || defined(BL_TARGET_ROM) || defined(BL_TARGET_RAM))
#include <assert.h>
#include <string.h>
#include "fsl_device_registers.h"
#include "bootloader_common.h"
#else
#include "fsl_common.h"
#endif

#include "fsl_ftfx_adapter.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @name FTFx configuration
 * @{
 */
/*! @brief Flash driver location. */
#if !defined(FTFx_DRIVER_IS_FLASH_RESIDENT)
#if (!defined(BL_TARGET_ROM) && !defined(BL_TARGET_RAM))
#define FTFx_DRIVER_IS_FLASH_RESIDENT 1 /*!< Used for the flash resident application. */
#else
#define FTFx_DRIVER_IS_FLASH_RESIDENT 0 /*!< Used for the non-flash resident application. */
#endif
#endif

/*! @brief Flash Driver Export option */
#if !defined(FTFx_DRIVER_IS_EXPORTED)
#if defined(BL_TARGET_ROM)
#define FTFx_DRIVER_IS_EXPORTED 1 /*!< Used for the ROM bootloader. */
#else
#define FTFx_DRIVER_IS_EXPORTED 0 /*!< Used for the MCUXpresso SDK application. */
#endif
#endif
/*@}*/

/*! @brief Indicates whether the secondary flash is supported in the Flash driver */
#if defined(FSL_FEATURE_FLASH_HAS_MULTIPLE_FLASH) || defined(FSL_FEATURE_FLASH_PFLASH_1_START_ADDRESS)
#define FTFx_DRIVER_HAS_FLASH1_SUPPORT (1)
#define FTFx_FLASH_COUNT (2)
#else
#define FTFx_DRIVER_HAS_FLASH1_SUPPORT (0)
#define FTFx_FLASH_COUNT (1)
#endif

/*!
 * @name Secondary flash configuration
 * @{
 */
/*! @brief Indicates whether the secondary flash has its own protection register in flash module. */
#if defined(FSL_FEATURE_FLASH_HAS_MULTIPLE_FLASH) && defined(FTFE_FPROTS_PROTS_MASK)
#define FTFx_FLASH1_HAS_PROT_CONTROL (1)
#else
#define FTFx_FLASH1_HAS_PROT_CONTROL (0)
#endif

/*! @brief Indicates whether the secondary flash has its own Execute-Only access register in flash module. */
#if defined(FSL_FEATURE_FLASH_HAS_MULTIPLE_FLASH) && defined(FTFE_FACSSS_SGSIZE_S_MASK)
#define FTFx_FLASH1_HAS_XACC_CONTROL (1)
#else
#define FTFx_FLASH1_HAS_XACC_CONTROL (0)
#endif
/*@}*/

#if FTFx_FLASH1_HAS_XACC_CONTROL || FTFx_FLASH1_HAS_PROT_CONTROL
#define FTFx_FLASH1_IS_INDEPENDENT_BLOCK (1)
#else
#define FTFx_FLASH1_IS_INDEPENDENT_BLOCK (0)
#endif

#endif /* _FSL_FTFX_FEATURES_H_ */

