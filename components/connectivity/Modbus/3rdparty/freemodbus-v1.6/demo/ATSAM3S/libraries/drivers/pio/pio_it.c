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
#include <board.h>
#include <pmc/pmc.h>
#include <utility/assert.h>
#include <utility/trace.h>
#include <cmsis/core_cm3.h>

//------------------------------------------------------------------------------
//         Local definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Local types
//------------------------------------------------------------------------------
/* Define WEAK attribute */
#if defined   ( __CC_ARM   )
    #define WEAK __attribute__ ((weak))
#elif defined ( __ICCARM__ )
    #define WEAK __weak
#elif defined (  __GNUC__  )
    #define WEAK __attribute__ ((weak))
#endif


//------------------------------------------------------------------------------
//         Local variables
//------------------------------------------------------------------------------
typedef void (*PioHandler) (unsigned char id);

/* Default PIO handlers defined as weak functions which have to be redefined by
 * the application */
WEAK void PIOA_Irq0Handler(unsigned char id) {}
WEAK void PIOA_Irq1Handler(unsigned char id) {}
WEAK void PIOA_Irq2Handler(unsigned char id) {}
WEAK void PIOA_Irq3Handler(unsigned char id) {}
WEAK void PIOA_Irq4Handler(unsigned char id) {}
WEAK void PIOA_Irq5Handler(unsigned char id) {}
WEAK void PIOA_Irq6Handler(unsigned char id) {}
WEAK void PIOA_Irq7Handler(unsigned char id) {}
WEAK void PIOA_Irq8Handler(unsigned char id) {}
WEAK void PIOA_Irq9Handler(unsigned char id) {}
WEAK void PIOA_Irq10Handler(unsigned char id) {}
WEAK void PIOA_Irq11Handler(unsigned char id) {}
WEAK void PIOA_Irq12Handler(unsigned char id) {}
WEAK void PIOA_Irq13Handler(unsigned char id) {}
WEAK void PIOA_Irq14Handler(unsigned char id) {}
WEAK void PIOA_Irq15Handler(unsigned char id) {}
WEAK void PIOA_Irq16Handler(unsigned char id) {}
WEAK void PIOA_Irq17Handler(unsigned char id) {}
WEAK void PIOA_Irq18Handler(unsigned char id) {}
WEAK void PIOA_Irq19Handler(unsigned char id) {}
WEAK void PIOA_Irq20Handler(unsigned char id) {}
WEAK void PIOA_Irq21Handler(unsigned char id) {}
WEAK void PIOA_Irq22Handler(unsigned char id) {}
WEAK void PIOA_Irq23Handler(unsigned char id) {}
WEAK void PIOA_Irq24Handler(unsigned char id) {}
WEAK void PIOA_Irq25Handler(unsigned char id) {}
WEAK void PIOA_Irq26Handler(unsigned char id) {}
WEAK void PIOA_Irq27Handler(unsigned char id) {}
WEAK void PIOA_Irq28Handler(unsigned char id) {}
WEAK void PIOA_Irq29Handler(unsigned char id) {}
WEAK void PIOA_Irq30Handler(unsigned char id) {}
WEAK void PIOA_Irq31Handler(unsigned char id) {}


/** pioAHandlers records ISR routines for each PIO Id for PIO controller A */
static const PioHandler pioAHandlers[32] = {
		(PioHandler) PIOA_Irq0Handler, /**< PA0 IT Handler */
		(PioHandler) PIOA_Irq1Handler, /**< PA1 IT Handler */
		(PioHandler) PIOA_Irq2Handler, /**< PA2 IT Handler */
		(PioHandler) PIOA_Irq3Handler, /**< PA3 IT Handler */
		(PioHandler) PIOA_Irq4Handler, /**< PA4 IT Handler */
		(PioHandler) PIOA_Irq5Handler, /**< PA5 IT Handler */
		(PioHandler) PIOA_Irq6Handler, /**< PA6 IT Handler */
		(PioHandler) PIOA_Irq7Handler, /**< PA7 IT Handler */
		(PioHandler) PIOA_Irq8Handler, /**< PA8 IT Handler */
		(PioHandler) PIOA_Irq9Handler, /**< PA9 IT Handler */
		(PioHandler) PIOA_Irq10Handler, /**< PA10 IT Handler */
		(PioHandler) PIOA_Irq11Handler, /**< PA11 IT Handler */
		(PioHandler) PIOA_Irq12Handler, /**< PA12 IT Handler */
		(PioHandler) PIOA_Irq13Handler, /**< PA13 IT Handler */
		(PioHandler) PIOA_Irq14Handler, /**< PA14 IT Handler */
		(PioHandler) PIOA_Irq15Handler, /**< PA15 IT Handler */
		(PioHandler) PIOA_Irq16Handler, /**< PA16 IT Handler */
		(PioHandler) PIOA_Irq17Handler, /**< PA17 IT Handler */
		(PioHandler) PIOA_Irq18Handler, /**< PA18 IT Handler */
		(PioHandler) PIOA_Irq19Handler, /**< PA19 IT Handler */
		(PioHandler) PIOA_Irq20Handler, /**< PA20 IT Handler */
		(PioHandler) PIOA_Irq21Handler, /**< PA21 IT Handler */
		(PioHandler) PIOA_Irq22Handler, /**< PA22 IT Handler */
		(PioHandler) PIOA_Irq23Handler, /**< PA23 IT Handler */
		(PioHandler) PIOA_Irq24Handler, /**< PA24 IT Handler */
		(PioHandler) PIOA_Irq25Handler, /**< PA25 IT Handler */
		(PioHandler) PIOA_Irq26Handler, /**< PA26 IT Handler */
		(PioHandler) PIOA_Irq27Handler, /**< PA27 IT Handler */
		(PioHandler) PIOA_Irq28Handler, /**< PA28 IT Handler */
		(PioHandler) PIOA_Irq29Handler, /**< PA29 IT Handler */
		(PioHandler) PIOA_Irq30Handler, /**< PA30 IT Handler */
		(PioHandler) PIOA_Irq31Handler  /**< PA31 IT Handler */
};

/* Default PIO handlers defined as weak functions which have to be redefined by
 * the application */
WEAK void PIOB_Irq0Handler(unsigned char id) {}
WEAK void PIOB_Irq1Handler(unsigned char id) {}
WEAK void PIOB_Irq2Handler(unsigned char id) {}
WEAK void PIOB_Irq3Handler(unsigned char id) {}
WEAK void PIOB_Irq4Handler(unsigned char id) {}
WEAK void PIOB_Irq5Handler(unsigned char id) {}
WEAK void PIOB_Irq6Handler(unsigned char id) {}
WEAK void PIOB_Irq7Handler(unsigned char id) {}
WEAK void PIOB_Irq8Handler(unsigned char id) {}
WEAK void PIOB_Irq9Handler(unsigned char id) {}
WEAK void PIOB_Irq10Handler(unsigned char id) {}
WEAK void PIOB_Irq11Handler(unsigned char id) {}
WEAK void PIOB_Irq12Handler(unsigned char id) {}
WEAK void PIOB_Irq13Handler(unsigned char id) {}
WEAK void PIOB_Irq14Handler(unsigned char id) {}
WEAK void PIOB_Irq15Handler(unsigned char id) {}
WEAK void PIOB_Irq16Handler(unsigned char id) {}
WEAK void PIOB_Irq17Handler(unsigned char id) {}
WEAK void PIOB_Irq18Handler(unsigned char id) {}
WEAK void PIOB_Irq19Handler(unsigned char id) {}
WEAK void PIOB_Irq20Handler(unsigned char id) {}
WEAK void PIOB_Irq21Handler(unsigned char id) {}
WEAK void PIOB_Irq22Handler(unsigned char id) {}
WEAK void PIOB_Irq23Handler(unsigned char id) {}
WEAK void PIOB_Irq24Handler(unsigned char id) {}
WEAK void PIOB_Irq25Handler(unsigned char id) {}
WEAK void PIOB_Irq26Handler(unsigned char id) {}
WEAK void PIOB_Irq27Handler(unsigned char id) {}
WEAK void PIOB_Irq28Handler(unsigned char id) {}
WEAK void PIOB_Irq29Handler(unsigned char id) {}
WEAK void PIOB_Irq30Handler(unsigned char id) {}
WEAK void PIOB_Irq31Handler(unsigned char id) {}


/** PIOBHandlers records ISR routines for each PIO Id for PIO controller A */
static const PioHandler pioBHandlers[32] = {
		(PioHandler) PIOB_Irq0Handler, /**< PB0 IT Handler */
		(PioHandler) PIOB_Irq1Handler, /**< PB1 IT Handler */
		(PioHandler) PIOB_Irq2Handler, /**< PB2 IT Handler */
		(PioHandler) PIOB_Irq3Handler, /**< PB3 IT Handler */
		(PioHandler) PIOB_Irq4Handler, /**< PB4 IT Handler */
		(PioHandler) PIOB_Irq5Handler, /**< PB5 IT Handler */
		(PioHandler) PIOB_Irq6Handler, /**< PB6 IT Handler */
		(PioHandler) PIOB_Irq7Handler, /**< PB7 IT Handler */
		(PioHandler) PIOB_Irq8Handler, /**< PB8 IT Handler */
		(PioHandler) PIOB_Irq9Handler, /**< PB9 IT Handler */
		(PioHandler) PIOB_Irq10Handler, /**< PB10 IT Handler */
		(PioHandler) PIOB_Irq11Handler, /**< PB11 IT Handler */
		(PioHandler) PIOB_Irq12Handler, /**< PB12 IT Handler */
		(PioHandler) PIOB_Irq13Handler, /**< PB13 IT Handler */
		(PioHandler) PIOB_Irq14Handler, /**< PB14 IT Handler */
		(PioHandler) PIOB_Irq15Handler, /**< PB15 IT Handler */
		(PioHandler) PIOB_Irq16Handler, /**< PB16 IT Handler */
		(PioHandler) PIOB_Irq17Handler, /**< PB17 IT Handler */
		(PioHandler) PIOB_Irq18Handler, /**< PB18 IT Handler */
		(PioHandler) PIOB_Irq19Handler, /**< PB19 IT Handler */
		(PioHandler) PIOB_Irq20Handler, /**< PB20 IT Handler */
		(PioHandler) PIOB_Irq21Handler, /**< PB21 IT Handler */
		(PioHandler) PIOB_Irq22Handler, /**< PB22 IT Handler */
		(PioHandler) PIOB_Irq23Handler, /**< PB23 IT Handler */
		(PioHandler) PIOB_Irq24Handler, /**< PB24 IT Handler */
		(PioHandler) PIOB_Irq25Handler, /**< PB25 IT Handler */
		(PioHandler) PIOB_Irq26Handler, /**< PB26 IT Handler */
		(PioHandler) PIOB_Irq27Handler, /**< PB27 IT Handler */
		(PioHandler) PIOB_Irq28Handler, /**< PB28 IT Handler */
		(PioHandler) PIOB_Irq29Handler, /**< PB29 IT Handler */
		(PioHandler) PIOB_Irq30Handler, /**< PB30 IT Handler */
		(PioHandler) PIOB_Irq31Handler  /**< PB31 IT Handler */
};

/* Default PIO handlers defined as weak functions which have to be redefined by
 * the application */
WEAK void PIOC_Irq0Handler(unsigned char id) {}
WEAK void PIOC_Irq1Handler(unsigned char id) {}
WEAK void PIOC_Irq2Handler(unsigned char id) {}
WEAK void PIOC_Irq3Handler(unsigned char id) {}
WEAK void PIOC_Irq4Handler(unsigned char id) {}
WEAK void PIOC_Irq5Handler(unsigned char id) {}
WEAK void PIOC_Irq6Handler(unsigned char id) {}
WEAK void PIOC_Irq7Handler(unsigned char id) {}
WEAK void PIOC_Irq8Handler(unsigned char id) {}
WEAK void PIOC_Irq9Handler(unsigned char id) {}
WEAK void PIOC_Irq10Handler(unsigned char id) {}
WEAK void PIOC_Irq11Handler(unsigned char id) {}
WEAK void PIOC_Irq12Handler(unsigned char id) {}
WEAK void PIOC_Irq13Handler(unsigned char id) {}
WEAK void PIOC_Irq14Handler(unsigned char id) {}
WEAK void PIOC_Irq15Handler(unsigned char id) {}
WEAK void PIOC_Irq16Handler(unsigned char id) {}
WEAK void PIOC_Irq17Handler(unsigned char id) {}
WEAK void PIOC_Irq18Handler(unsigned char id) {}
WEAK void PIOC_Irq19Handler(unsigned char id) {}
WEAK void PIOC_Irq20Handler(unsigned char id) {}
WEAK void PIOC_Irq21Handler(unsigned char id) {}
WEAK void PIOC_Irq22Handler(unsigned char id) {}
WEAK void PIOC_Irq23Handler(unsigned char id) {}
WEAK void PIOC_Irq24Handler(unsigned char id) {}
WEAK void PIOC_Irq25Handler(unsigned char id) {}
WEAK void PIOC_Irq26Handler(unsigned char id) {}
WEAK void PIOC_Irq27Handler(unsigned char id) {}
WEAK void PIOC_Irq28Handler(unsigned char id) {}
WEAK void PIOC_Irq29Handler(unsigned char id) {}
WEAK void PIOC_Irq30Handler(unsigned char id) {}
WEAK void PIOC_Irq31Handler(unsigned char id) {}


/** PIOCHandlers records ISR routines for each PIO Id for PIO controller A */
static const PioHandler pioCHandlers[32] = {
		(PioHandler) PIOC_Irq0Handler, /**< PC0 IT Handler */
		(PioHandler) PIOC_Irq1Handler, /**< PC1 IT Handler */
		(PioHandler) PIOC_Irq2Handler, /**< PC2 IT Handler */
		(PioHandler) PIOC_Irq3Handler, /**< PC3 IT Handler */
		(PioHandler) PIOC_Irq4Handler, /**< PC4 IT Handler */
		(PioHandler) PIOC_Irq5Handler, /**< PC5 IT Handler */
		(PioHandler) PIOC_Irq6Handler, /**< PC6 IT Handler */
		(PioHandler) PIOC_Irq7Handler, /**< PC7 IT Handler */
		(PioHandler) PIOC_Irq8Handler, /**< PC8 IT Handler */
		(PioHandler) PIOC_Irq9Handler, /**< PC9 IT Handler */
		(PioHandler) PIOC_Irq10Handler, /**< PC10 IT Handler */
		(PioHandler) PIOC_Irq11Handler, /**< PC11 IT Handler */
		(PioHandler) PIOC_Irq12Handler, /**< PC12 IT Handler */
		(PioHandler) PIOC_Irq13Handler, /**< PC13 IT Handler */
		(PioHandler) PIOC_Irq14Handler, /**< PC14 IT Handler */
		(PioHandler) PIOC_Irq15Handler, /**< PC15 IT Handler */
		(PioHandler) PIOC_Irq16Handler, /**< PC16 IT Handler */
		(PioHandler) PIOC_Irq17Handler, /**< PC17 IT Handler */
		(PioHandler) PIOC_Irq18Handler, /**< PC18 IT Handler */
		(PioHandler) PIOC_Irq19Handler, /**< PC19 IT Handler */
		(PioHandler) PIOC_Irq20Handler, /**< PC20 IT Handler */
		(PioHandler) PIOC_Irq21Handler, /**< PC21 IT Handler */
		(PioHandler) PIOC_Irq22Handler, /**< PC22 IT Handler */
		(PioHandler) PIOC_Irq23Handler, /**< PC23 IT Handler */
		(PioHandler) PIOC_Irq24Handler, /**< PC24 IT Handler */
		(PioHandler) PIOC_Irq25Handler, /**< PC25 IT Handler */
		(PioHandler) PIOC_Irq26Handler, /**< PC26 IT Handler */
		(PioHandler) PIOC_Irq27Handler, /**< PC27 IT Handler */
		(PioHandler) PIOC_Irq28Handler, /**< PC28 IT Handler */
		(PioHandler) PIOC_Irq29Handler, /**< PC29 IT Handler */
		(PioHandler) PIOC_Irq30Handler, /**< PC30 IT Handler */
		(PioHandler) PIOC_Irq31Handler  /**< PC31 IT Handler */
};

//------------------------------------------------------------------------------
/// \brief Generic PIO Handler.
/// The NVIC branches to the PIOx_IrqHandler() registered in exeption.c
/// PIOx_IrqHandler() invokes PIO_IrqHandler which scans which id has triggered
/// an interrupt and call the corresponding routine for the Id (<=> for the pin)
/// \param pPio  PIO controller base address.
/// \param pPioHandlers Constant table which contains IT routines for each PIO Id
//------------------------------------------------------------------------------
static void PIO_IrqHandler(Pio *pPio, const PioHandler *pPioHandlers)
{
    unsigned int status, id;

    status = pPio->PIO_ISR;
    status &= pPio->PIO_IMR;

    for (id = 0; id < 32; ++id) {
        if (status & (1 << id)) {
            pPioHandlers[id](id);
        }
    }

}

//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// \brief Parallel IO Controller A interrupt handler
/// Redefined PIOA interrupt handler for NVIC interrupt table.
//------------------------------------------------------------------------------
void PIOA_IrqHandler(void)
{
    PIO_IrqHandler(PIOA, pioAHandlers);
}

//------------------------------------------------------------------------------
/// \brief Parallel IO Controller B interrupt handler
/// Redefined PIOB interrupt handler for NVIC interrupt table.
//------------------------------------------------------------------------------
void PIOB_IrqHandler(void)
{
    PIO_IrqHandler(PIOB, pioBHandlers);
}

//------------------------------------------------------------------------------
/// \brief Parallel IO Controller C interrupt handler
/// Redefined PIOC interrupt handler for NVIC interrupt table.
//------------------------------------------------------------------------------
void PIOC_IrqHandler(void)
{
    PIO_IrqHandler(PIOC, pioCHandlers);
}

//------------------------------------------------------------------------------
/// Enables the given interrupt source. The status
/// register of the corresponding PIO controller is cleared prior to enabling
/// the interrupt.
/// \param pPin  Interrupt source to enable.
//------------------------------------------------------------------------------
void PIO_EnableIt(const Pin *pPin)
{
    Pio* pio = pPin->pio;
    TRACE_DEBUG("PIO_EnableIt()\n\r");

//    SANITY_CHECK(pPin);

    /* Enable the interrupt  in the PIO controller */
    pio->PIO_ISR;
    pio->PIO_IER = pPin->mask;


}

//------------------------------------------------------------------------------
/// Disables a given interrupt source, with no added side effects.
/// \param pPin  Interrupt source to disable.
//------------------------------------------------------------------------------
void PIO_DisableIt(const Pin *pPin)
{
    Pio* pio = pPin->pio;
    SANITY_CHECK(pPin);

    TRACE_DEBUG("PIO_DisableIt()\n\r");

    /* Disable the interrupt  in the PIO controller */
    pio->PIO_IDR = pPin->mask;
}

//------------------------------------------------------------------------------
/// \brief Initializes the PIO interrupt management logic
/// The desired priority of PIO interrupts must be provided.
/// Calling this function multiple times result in the reset of currently
/// configured interrupts.
/// \param priority  PIO controller interrupts priority.
//------------------------------------------------------------------------------
void PIO_InitializeInterrupts(unsigned int priority)
{
    TRACE_DEBUG("PIO_Initialize()\n\r");

    // Configure PIO interrupt sources
    TRACE_DEBUG("PIO_Initialize: Configuring PIOA\n\r");
    PIOA->PIO_ISR;
    PIOA->PIO_IDR = 0xFFFFFFFF;
    NVIC_DisableIRQ(PIOA_IRQn);
    NVIC_ClearPendingIRQ(PIOA_IRQn);
    NVIC_SetPriority(PIOA_IRQn, priority);
    NVIC_EnableIRQ(PIOA_IRQn);

    TRACE_DEBUG("PIO_Initialize: Configuring PIOB\n\r");
    PIOB->PIO_ISR;
    PIOB->PIO_IDR = 0xFFFFFFFF;
    NVIC_DisableIRQ(PIOB_IRQn);
    NVIC_ClearPendingIRQ(PIOB_IRQn);
    NVIC_SetPriority(PIOB_IRQn, priority);
    NVIC_EnableIRQ(PIOB_IRQn);

    TRACE_DEBUG("PIO_Initialize: Configuring PIOC\n\r");
    PIOC->PIO_ISR;
    PIOC->PIO_IDR = 0xFFFFFFFF;
    NVIC_DisableIRQ(PIOC_IRQn);
    NVIC_ClearPendingIRQ(PIOC_IRQn);
    NVIC_SetPriority(PIOC_IRQn, priority);
    NVIC_EnableIRQ(PIOC_IRQn);
}
