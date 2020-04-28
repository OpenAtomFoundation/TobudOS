/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

//------------------------------------------------------------------------------
/// \file
///
/// \par Purpose
///
/// Configuration and handling of interrupts on PIO status changes. The API
/// provided here have several advantages over the traditional PIO interrupt
/// configuration approach:
///    - It is highly portable
///    - It automatically demultiplexes interrupts when multiples pins have been
///      configured on a single PIO controller
///    - It allows a group of pins to share the same interrupt
///
/// However, it also has several minor drawbacks that may prevent from using it
/// in particular applications:
///    - It enables the clocks of all PIO controllers
///    - PIO controllers all share the same interrupt handler, which does the
///      demultiplexing and can be slower than direct configuration
///    - It reserves space for a fixed number of interrupts, which can be
///      increased by modifying the appropriate constant in pio_it.c.
///
/// \par Usage
///
/// -# Initialize the PIO interrupt mechanism using PIO_InitializeInterrupts()
///    with the desired priority (0 ... 7).
/// -# Configure a status change interrupt on one or more pin(s) with
///    PIO_ConfigureIt().
/// -# Enable & disable interrupts on pins using PIO_EnableIt() and
///    PIO_DisableIt().
//------------------------------------------------------------------------------

#ifndef PIO_IT_H
#define PIO_IT_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "pio.h"

//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/// \file

/// Disable traces for this file
#undef TRACE_LEVEL
#define TRACE_LEVEL 0

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "pio_it.h"
#include "pio.h"

//------------------------------------------------------------------------------
/// \brief Parallel IO Controller A interrupt handler
/// Redefined PIOA interrupt handler for NVIC interrupt table.
//------------------------------------------------------------------------------
extern void PIOA_IrqHandler(void);
//------------------------------------------------------------------------------
/// \brief Parallel IO Controller B interrupt handler
/// Redefined PIOB interrupt handler for NVIC interrupt table.
//------------------------------------------------------------------------------
extern void PIOB_IrqHandler(void);
//------------------------------------------------------------------------------
/// \brief Parallel IO Controller C interrupt handler
/// Redefined PIOC interrupt handler for NVIC interrupt table.
//------------------------------------------------------------------------------
extern void PIOC_IrqHandler(void);
//------------------------------------------------------------------------------
/// Configures and enables the given interrupt source. The status
/// register of the corresponding PIO controller is cleared prior to enabling
/// the interrupt.
/// \param pPin  Interrupt source to enable.
//------------------------------------------------------------------------------
extern void PIO_EnableIt(const Pin *pPin);
//------------------------------------------------------------------------------
/// Disables a given interrupt source, with no added side effects.
/// \param pPin  Interrupt source to disable.
//------------------------------------------------------------------------------
extern void PIO_DisableIt(const Pin *pPin);
//------------------------------------------------------------------------------
/// \brief Initializes the PIO interrupt management logic
/// The desired priority of PIO interrupts must be provided.
/// Calling this function multiple times result in the reset of currently
/// configured interrupts.
/// \param priority  PIO controller interrupts priority.
//------------------------------------------------------------------------------
extern void PIO_InitializeInterrupts(unsigned int priority);

#endif //#ifndef PIO_IT_H

