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
//         Headers
//------------------------------------------------------------------------------

#include "clock.h"
#include <board.h>
#include <utility/trace.h>

//------------------------------------------------------------------------------
//         Internal types
//------------------------------------------------------------------------------
int currentConfig = 0; // 0 have to be the default configuration

//------------------------------------------------------------------------------
/// Describes a possible clock configuration (processor clock & master clock),
/// including the necessary register values.
//------------------------------------------------------------------------------
struct ClockConfiguration {

    /// Processor clock frequency (in MHz).
    unsigned short pck;
    /// Master clock frequency (in MHz).
    unsigned short mck;
    /// CKGR_PLL reqister value.
    unsigned int pllr;
    /// PMC_MCKR register value.
    unsigned int mckr;
};

//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------

// Clock configurations for the AT91SAM9263-EK
#if defined(at91sam9263)

#define AT91C_CKGR_PLLR     AT91C_CKGR_PLLAR
#define AT91C_PMC_LOCK      AT91C_PMC_LOCKA

static const struct ClockConfiguration clockConfigurations[] = {
    // PCK = 200 MHz, MCK = 100 MHz
    {200, 100, 0x206DBF09, 0x00000102},
    // PCK = 240 MHz, MCK = 120 MHz
    {240, 120, 0x202BC003, 0x00000102},
    // PCK = 120 MHz, MCK = 120 MHz
    {120, 120, 0x20156003, 0x00000002},
    // PCK = 96 MHz, MCK = 48 MHz
    {96, 48, 0x2057400F, 0x00000102},
    // PCK = 48 MHz, MCK = 48 MHz
    {48, 48, 0x2057400F, 0x00000006}
};

// Clock configurations for the AT91SAM9G20-EK
#elif defined(at91sam9g20)

#define AT91C_CKGR_PLLR     AT91C_CKGR_PLLAR
#define AT91C_PMC_LOCK      AT91C_PMC_LOCKA

static const struct ClockConfiguration clockConfigurations[] = {

    // PCK = 400 MHz, MCK = 133 MHz
    {400, 133, 0x202A0101, 0x00001302},
    // PCK = 200 MHz, MCK = 100 MHz
    {200, 100, 0x202A3F01, 0x0000010A},
    // PCK = 96 MHz, MCK = 48 MHz
    {96, 48, 0x207C3F03, 0x0000010E},
    // PCK = 48 MHz, MCK = 48 MHz
    {48, 48, 0x207C3F03, 0x00000012}
};

// Clock configurations for the AT91SAM9XE-EK, AT91SAM9261-EK
#elif defined(at91sam9xe128) || \
      defined(at91sam9xe256) || \
      defined(at91sam9xe512) || \
      defined(at91sam9261)

#define AT91C_CKGR_PLLR     AT91C_CKGR_PLLAR
#define AT91C_PMC_LOCK      AT91C_PMC_LOCKA

static const struct ClockConfiguration clockConfigurations[] = {

    // PCK = 200 MHz, MCK = 100 MHz
    {200, 100, 0x208CBF0D, 0x00000102},
    // PCK = 100 MHz, MCK = 100 MHz
    {100, 100, 0x20273F07, 0x00000002},
    // PCK = 96 MHz, MCK = 48 MHz
    {96, 48, 0x20483F0E, 0x00000102},
    // PCK = 48 MHz, MCK = 48 MHz
    {48, 48, 0x20483F0E, 0x00000006}
};

// Clock configurations for the AT91SAM9RL64-EK, AT91CAP9-DK, AT91CAP9-STK
#elif defined(at91sam9rl64) || \
      defined(at91cap9)

#define AT91C_CKGR_PLLR     AT91C_CKGR_PLLAR
#define AT91C_PMC_LOCK      AT91C_PMC_LOCKA

static const struct ClockConfiguration clockConfigurations[] = {

    // PCK = 200 MHz, MCK = 100 MHz
    {200, 100, 0x2031BF03, 0x00000102},
    // PCK = 100 MHz, MCK = 100 MHz
    {100, 100, 0x20183F03, 0x00000002},
    // PCK = 96 MHz, MCK = 48 MHz
    {96, 48, 0x20073F01, 0x00000102},
    // PCK = 48 MHz, MCK = 48 MHz
    {48, 48, 0x20073F01, 0x00000006}
};

// Clock configuration for the AT91SAM7X-EK, AT91SAM7S-EK, AT91SAM7SE-EK
#elif defined(at91sam7x128) || defined(at91sam7x256) || defined(at91sam7x512) \
      || defined(at91sam7xc128) || defined(at91sam7xc256) || defined(at91sam7xc512) \
      || defined(at91sam7s16) || defined(at91sam7s161) || defined(at91sam7s32) \
      || defined(at91sam7s321) || defined(at91sam7s64) || defined(at91sam7s128) \
      || defined(at91sam7s256) || defined(at91sam7s512) || defined(at91sam7se32) \
      || defined(at91sam7se256) || defined(at91sam7se512)

static const struct ClockConfiguration clockConfigurations[] = {

    // PCK = 55 MHz, MCK = 55 MHz
    {55, 55, 0x006A3F12, 0x00000007},
    // PCK = 48 MHz, MCK = 48 MHz
    {48, 48, 0x00483F0E, 0x00000007},
};

// Clock configuration for the AT91SAM7A3
#elif defined(at91sam7a3)

static const struct ClockConfiguration clockConfigurations[] = {

    // PCK = 60 MHz, MCK = 60 MHz
    {60, 60, 0x000C3F02, 0x00000007},
    // PCK = 48 MHz, MCK = 48 MHz
    {48, 48, 0x00483F0E, 0x00000007}
};

// No clock configuration
#else
    #error No clock configuration for this board.
#endif

/// Number of available clock configurations
#define NB_CLOCK_CONFIGURATION (sizeof(clockConfigurations)/sizeof(clockConfigurations[0]))

//------------------------------------------------------------------------------
//         Global Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Sets the specified clock configuration.
/// \param configuration  Index of the configuration to set.
//------------------------------------------------------------------------------
void CLOCK_SetConfig(unsigned char configuration)
{
    printf("Setting clock configuration #%d ... ", configuration);
    currentConfig = configuration;
    
    // Switch to main oscillator in two operations
    PMC->PMC_MCKR = (PMC->PMC_MCKR & ~AT91C_PMC_CSS) | AT91C_PMC_CSS_MAIN_CLK;
    while ((PMC->PMC_SR & AT91C_PMC_MCKRDY) == 0);

    // Configure PLL
    *AT91C_CKGR_PLLAR = clockConfigurations[configuration].pllr;
    while ((PMC->PMC_SR & AT91C_PMC_LOCKA) == 0);

    // Configure master clock in two operations
    PMC->PMC_MCKR = (clockConfigurations[configuration].mckr & ~AT91C_PMC_CSS) | AT91C_PMC_CSS_MAIN_CLK;
    while ((PMC->PMC_SR & AT91C_PMC_MCKRDY) == 0);
    PMC->PMC_MCKR = clockConfigurations[configuration].mckr;
    while ((PMC->PMC_SR & AT91C_PMC_MCKRDY) == 0);

    // DBGU reconfiguration
    DBGU_Configure(DBGU_STANDARD, 115200, clockConfigurations[configuration].mck*1000000);
    printf("done.\n\r");
}

//------------------------------------------------------------------------------
/// Display the user menu on the DBGU.
//------------------------------------------------------------------------------
void CLOCK_DisplayMenu(void)
{
    unsigned int i;

    printf("\n\rMenu Clock configuration:\n\r");
    for (i = 0; i < NB_CLOCK_CONFIGURATION; i++) {

        printf("  %d: Set PCK = %3d MHz, MCK = %3d MHz   %s\n\r",
               i,
               clockConfigurations[i].pck,
               clockConfigurations[i].mck,
               (currentConfig==i)?"(curr)":"");
    }
}

//------------------------------------------------------------------------------
/// Get the current MCK
//------------------------------------------------------------------------------
unsigned short CLOCK_GetCurrMCK(void)
{
    return clockConfigurations[currentConfig].mck;
}

//------------------------------------------------------------------------------
/// Get the current PCK
//------------------------------------------------------------------------------
unsigned short CLOCK_GetCurrPCK(void)
{
    return clockConfigurations[currentConfig].pck;
}

//------------------------------------------------------------------------------
/// Change clock configuration.
//------------------------------------------------------------------------------
void CLOCK_UserChangeConfig(void)
{
    unsigned char key = 0;
  
    while (1)
    {
        CLOCK_DisplayMenu();      
        key = DBGU_GetChar();

        if ((key >= '0') && (key <= ('0' + NB_CLOCK_CONFIGURATION - 1)))
        {
            CLOCK_SetConfig(key - '0');
            break;
        }
    }
}