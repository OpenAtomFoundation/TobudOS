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

/*
    Title: IAP implementation

    About: Purpose
        IAP feature is avalaible on ATMEL chip with internal FLASH. This allows
        the FLASH memory to be programmed even when the code is also running in
        FLASH, without writing some SRAM relocated functions.
        The IAP function entry point is written in the 3rd exception vector
        (SWI at 0x300008), and is retrieved just by reading the value at this
        address.

        ROM function algorithm :
        // Send Command
        AT91_BASE_MC->MC_FCR = FlashCmd
        // Wait Flash is ready
        while ((AT91C_BASE_MC->MC_FSR & AT91C_MC_FRDY) != AT91C_MC_FRDY)
        return
*/

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "iap.h"
#include <board.h>
#include <utility/trace.h>

//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------
/*
    Function: IAP_PerformCmd
        Perform a FLASH command through the IAP function.

    Returns:
        0 if the IAP function has executed correctly.
       -1 if the IAP feature is not implemented in the ROM code.
*/
unsigned long IAP_PerformCmd(unsigned long FlashCmd)
{   
    // Pointer on IAP function in ROM
    static void (*IAP_Function)(unsigned int) = 0;

    if (IAP_Function == 0) {

        IAP_Function = (void (*)(unsigned int)) (*((unsigned int *) IAP_FUNC_ADDR));
        TRACE_DEBUG("IAP_PerformCmd : IAP function address in ROM : 0x%08X\n\r", (unsigned int) IAP_Function);

        /* Check if IAP function is implemented (opcode in SWI != 'b' or 'ldr') */
        if ((((((unsigned long)IAP_Function >> 24) & 0xFF) == 0xEA) ||
            (((unsigned long)IAP_Function >> 24) & 0xFF) == 0xE5)) {
             TRACE_ERROR("IAP_PerformCmd : no IAP function address found in ROM\n\r");
             IAP_Function = 0;
             return -1;
        }
    }

    /* Perform FLASH command */
    IAP_Function(FlashCmd);

    return 0;
}

