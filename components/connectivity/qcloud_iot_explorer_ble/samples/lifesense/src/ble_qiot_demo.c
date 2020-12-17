//*****************************************************************************
//
//! @file ble_freertos_watch.c
//!
//! @brief ARM Cordio BLE - Concurrent Master/Slave Example.
//!
//! Purpose: This example demonstrates an BLE application in the Central role.
//! That is the BLE application operates as a slave to phone master and as the
//! master of subordinate slave devices running freertos_fit example in this SDK.
//!
//! Additional Information:
//! 1. Printing takes place over the ITM at 1M Baud.
//! 2. When the example powers up, 
//!    2.A. it enters advertising mode by default to wait for connection from 
//!         smart phone with Time profile, Alert Notification profile and Phone
//!         Alert Status profile supported.
//!    2.B. when BTN2 on Apollo3 EVB is short-pressed, if advertising is on, it
//!         stops advertising first and then starts scanning when advertising is
//!         stopped; if scanning is on, it stops scanning and re-start advertising
//!         when scanning stops.
//!    2.C. During scanning, the device (if discovered) running freertos_fit
//!         example in this SDK will be connected and scanning will be stopped.
//!    2.D. Repeat 2.B. and 2.C. above to connect to a new slave device running 
//!         freertos_fit example (max slaves is 3).
//! 3. when phone (iPhone is used) connects to this example, the services of Time
//!    profile, Alert Notification profile and Phone Alert Status profile will be
//     subscribed by the example.
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2020, Ambiq Micro
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision 2.4.2 of the AmbiqSuite Development Package.
//
//*****************************************************************************

//*****************************************************************************
//
// This application has a large number of common include files. For
// convenience, we'll collect them all together in a single header and include
// that everywhere.
//
//*****************************************************************************
#include "ble_qiot_demo.h"
#include "rtos.h"

#include "SEGGER_RTT.h"

#include "ble_qiot_export.h"

//*****************************************************************************
//
// Enable printing to the console.
//
//*****************************************************************************
void
enable_print_interface(void)
{
    //
    // Initialize a debug printing interface.
    //
    am_bsp_itm_printf_enable();
}

//*****************************************************************************
//
// Main Function
//
//*****************************************************************************
int
main(void)
{
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
    // SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    SEGGER_RTT_WriteString(0, ">>>> Tencent ble qiot demo start <<<<\r\n");

    //
    // Set the clock frequency
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

#ifndef NOFPU
    //
    // Enable the floating point module, and configure the core for lazy
    // stacking.
    //
    am_hal_sysctrl_fpu_enable();
    am_hal_sysctrl_fpu_stacking_enable(true);
#else
    am_hal_sysctrl_fpu_disable();
#endif

    //
    // Configure the board for low power.
    //
    am_bsp_low_power_init();

    // Turn off unused Flash & SRAM

#ifdef AM_PART_APOLLO
    //
    // SRAM bank power setting.
    // Need to match up with actual SRAM usage for the program
    // Current usage is between 32K and 40K - so disabling upper 3 banks
    //
    am_hal_mcuctrl_sram_power_set(AM_HAL_MCUCTRL_SRAM_POWER_DOWN_5 |
                                  AM_HAL_MCUCTRL_SRAM_POWER_DOWN_6 |
                                  AM_HAL_MCUCTRL_SRAM_POWER_DOWN_7,
                                  AM_HAL_MCUCTRL_SRAM_POWER_DOWN_5 |
                                  AM_HAL_MCUCTRL_SRAM_POWER_DOWN_6 |
                                  AM_HAL_MCUCTRL_SRAM_POWER_DOWN_7);

#if 0 // Not turning off the Flash as it may be needed to download the image
    //
    // Flash bank power set.
    //
    am_hal_mcuctrl_flash_power_set(AM_HAL_MCUCTRL_FLASH_POWER_DOWN_1);
#endif
#endif // AM_PART_APOLLO

#ifdef AM_PART_APOLLO2
#if 0 // Not turning off the Flash as it may be needed to download the image
    am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEMEN_FLASH512K);
#endif
    am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEMEN_SRAM64K);
#endif // AM_PART_APOLLO2

    //
    // Enable printing to the console.
    //
#ifdef AM_DEBUG_PRINTF
    enable_print_interface();
#endif

    //
    // Initialize plotting interface.
    //
    am_util_debug_printf("FreeRTOS Watch Example\n");

    // ble_services_add(service_info);
    // ble_qiot_explorer_init();
    // conn_params_init();

    //
    // Run the application.
    //
    run_tasks();

    //
    // We shouldn't ever get here.
    //
    while (1)
    {
    }

}

