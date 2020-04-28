/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Atmel Corporation
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

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "board.h"
#include "board_memories.h"
#include "board_lowlevel.h"
#include <pio/pio.h>

/*----------------------------------------------------------------------------
 *        Local definitions
 *----------------------------------------------------------------------------*/
/** Define clock timeout */
#define CLOCK_TIMEOUT         5000

/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/
static void BOARD_ConfigurePmc(void)
{

#define AT91C_CKGR_MUL_SHIFT         16
#define AT91C_CKGR_PLLCOUNT_SHIFT     8
#define AT91C_CKGR_DIV_SHIFT          0

// Settings at 64 MHz for MCK

#define BOARD_OSCOUNT         (CKGR_MOR_MOSCXTST & (0x8 << 8))

// PLLA Settings 64 MHz : 12 / 3 * 32
#define BOARD_PLLAR ((1 << 29) | (0x1F << AT91C_CKGR_MUL_SHIFT) \
        | (0x1 << AT91C_CKGR_PLLCOUNT_SHIFT) | (0x3 << AT91C_CKGR_DIV_SHIFT))

// PLLB Settings 96 MHz
#define BOARD_PLLBR ((1 << 29) | (0x7 << AT91C_CKGR_MUL_SHIFT) \
        | (0x1 << AT91C_CKGR_PLLCOUNT_SHIFT) | (0x1 << AT91C_CKGR_DIV_SHIFT))

// USB on PLLB, MCK/PCK on PLLA
#define BOARD_MCKR ( PMC_MCKR_PRES_CLK_2 | PMC_MCKR_CSS_PLLA_CLK)

// Define clock timeout
#undef CLOCK_TIMEOUT
#define CLOCK_TIMEOUT           0xFFFFFFFF

    uint32_t timeout = 0;

    /* Enable NRST reset
     ************************************/
    //AT91C_BASE_RSTC->RSTC_RMR |= AT91C_RSTC_URSTEN;

    /* Initialize main oscillator
     ****************************/
    if(!(PMC->CKGR_MOR & CKGR_MOR_MOSCSEL))
    {

        PMC->CKGR_MOR = (0x37 << 16) | BOARD_OSCOUNT | CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTEN;
        timeout = 0;
        while (!(PMC->PMC_SR & PMC_SR_MOSCXTS) && (timeout++ < CLOCK_TIMEOUT));

    }

    /* Switch to 3-20MHz Xtal oscillator */
    PMC->CKGR_MOR = (0x37 << 16) | BOARD_OSCOUNT | CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTEN | CKGR_MOR_MOSCSEL;
    timeout = 0;
    while (!(PMC->PMC_SR & PMC_SR_MOSCSELS) && (timeout++ < CLOCK_TIMEOUT));
    PMC->PMC_MCKR = (PMC->PMC_MCKR & ~(uint32_t)PMC_MCKR_CSS) | PMC_MCKR_CSS_MAIN_CLK;
    timeout = 0;
    while (!(PMC->PMC_SR & PMC_SR_MCKRDY) && (timeout++ < CLOCK_TIMEOUT));

    /** Set 3 WS for Embedded Flash Access */
    EFC->EEFC_FMR = (3 << 8);

    /* Initialize PLLA */
    PMC->CKGR_PLLAR = BOARD_PLLAR;
    timeout = 0;
    while (!(PMC->PMC_SR & PMC_SR_LOCKA) && (timeout++ < CLOCK_TIMEOUT));

    /* Initialize PLLB */
    PMC->CKGR_PLLBR = BOARD_PLLBR;
    timeout = 0;
    while (!(PMC->PMC_SR & PMC_SR_LOCKB) && (timeout++ < CLOCK_TIMEOUT));

    // Set USB clock on PLLB
    REG_PMC_USB = PMC_USB_USBS | (PMC_USB_USBDIV & (1 << 8));

    /* Switch to fast clock
     **********************/
    PMC->PMC_MCKR = (BOARD_MCKR & ~PMC_MCKR_CSS) | PMC_MCKR_CSS_MAIN_CLK;
    timeout = 0;
    while (!(PMC->PMC_SR & PMC_SR_MCKRDY) && (timeout++ < CLOCK_TIMEOUT));

    PMC->PMC_MCKR = BOARD_MCKR;
    timeout = 0;
    while (!(PMC->PMC_SR & PMC_SR_MCKRDY) && (timeout++ < CLOCK_TIMEOUT));

}

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * \brief Performs the low-level initialization of the chip. This includes EFC,
 * master clock and watchdog configuration.
 */
/*----------------------------------------------------------------------------*/
void LowLevelInit (void)
{
    /** Configure PMC */
    BOARD_ConfigurePmc();
}