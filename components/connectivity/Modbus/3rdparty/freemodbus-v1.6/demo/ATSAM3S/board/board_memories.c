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

/*
    Title: Memories implementation
*/
/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include <board.h>
#include "board_memories.h"

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/


/**
 * \brief Configures the EBI for NandFlash access.
 *
 */
extern void BOARD_ConfigureNandFlash( void )
{
    // Open EBI clock
    PMC->PMC_PCER0 = (1<< ID_SMC);
    // NCS0 is assigned to a NAND Flash (NANDOE and NANWE used for NCS0)
    MATRIX->CCFG_SMCNFCS = CCFG_SMCNFCS_SMC_NFCS0;

#if 0
    SMC->SMC_CS_NUMBER[0].SMC_SETUP = 0
                    | ((0 <<  0) & SMC_SETUP0_NWE_SETUP)
                    | ((1 <<  8) & SMC_SETUP0_NCS_WR_SETUP)
                    | ((0 << 16) & SMC_SETUP0_NRD_SETUP)
                    | ((1 << 24) & SMC_SETUP0_NCS_RD_SETUP);

    SMC->SMC_CS_NUMBER[0].SMC_PULSE = 0
                    | ((2 <<  0) & SMC_PULSE0_NWE_PULSE)
                    | ((3 <<  8) & SMC_PULSE0_NCS_WR_PULSE)
                    | ((4 << 16) & SMC_PULSE0_NRD_PULSE)
                    | ((4 << 24) & SMC_PULSE0_NCS_RD_PULSE);

    SMC->SMC_CS_NUMBER[0].SMC_CYCLE = 0
                  | ((4 <<  0) & SMC_CYCLE0_NWE_CYCLE)
                  | ((7 << 16) & SMC_CYCLE0_NRD_CYCLE);

    SMC->SMC_CS_NUMBER[0].SMC_MODE = SMC_MODE0_READ_MODE | SMC_MODE0_WRITE_MODE;
#else
    SMC->SMC_CS_NUMBER[0].SMC_SETUP = 0
                    | ((0 <<  0) & SMC_SETUP0_NWE_SETUP)
                    | ((0 <<  8) & SMC_SETUP0_NCS_WR_SETUP)
                    | ((0 << 16) & SMC_SETUP0_NRD_SETUP)
                    | ((0 << 24) & SMC_SETUP0_NCS_RD_SETUP);

    SMC->SMC_CS_NUMBER[0].SMC_PULSE = 0
                    | ((2 <<  0) & SMC_PULSE0_NWE_PULSE)
                    | ((2 <<  8) & SMC_PULSE0_NCS_WR_PULSE)
                    | ((2 << 16) & SMC_PULSE0_NRD_PULSE)
                    | ((2 << 24) & SMC_PULSE0_NCS_RD_PULSE);

    SMC->SMC_CS_NUMBER[0].SMC_CYCLE = 0
                  | ((3 <<  0) & SMC_CYCLE0_NWE_CYCLE)
                  | ((3 << 16) & SMC_CYCLE0_NRD_CYCLE);

    SMC->SMC_CS_NUMBER[0].SMC_MODE = SMC_MODE1_READ_MODE | SMC_MODE1_WRITE_MODE;
#endif



}

