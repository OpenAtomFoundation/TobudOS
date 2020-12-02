//*****************************************************************************
//
//! @file hci_apollo_config.h
//!
//! @brief This file describes the physical aspects of the HCI conection.
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

#include <stdint.h>
#include "am_bsp.h"

#ifndef HCI_APOLLO_CONFIG_H
#define HCI_APOLLO_CONFIG_H

//*****************************************************************************
//
// Pin numbers and configuration.
//
// NOTE: RTS, CTS, and RESET are implemented as GPIOs, so no "CFG" field is
// needed.
//
//*****************************************************************************
//#define HCI_APOLLO_POWER_PIN            AM_BSP_GPIO_EM9304_POWER
//#define HCI_APOLLO_POWER_CFG            AM_BSP_GPIO_CFG_EM9304_POWER

#define HCI_APOLLO_RESET_PIN            AM_BSP_GPIO_EM9304_RESET

//*****************************************************************************
//
// Other options.
//
// These options are provided in case your board setup is a little more
// unusual. Most boards shouldn't need these features. If in doubt, leave all
// of these features disabled.
//
//*****************************************************************************
#define HCI_APOLLO_CFG_OVERRIDE_ISR         1 // Override the exactle UART ISR

#endif // HCI_APOLLO_CONFIG_H
