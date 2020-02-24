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

/*------------------------------------------------------------------------------
 *         Headers
 *------------------------------------------------------------------------------*/

#include "tc.h"

/*------------------------------------------------------------------------------
 *         Global functions
 *------------------------------------------------------------------------------*/

/**------------------------------------------------------------------------------
 * \brief Configures a Timer Counter Channel
 * Configures a Timer Counter to operate in the given mode. Timer is stopped
 * after configuration and must be restarted with TC_Start(). All the
 * interrupts of the timer are also disabled.
 * \param pTc  Pointer to an AT91S_TC instance.
 * \param channel Channel number.
 * \param mode  Operating mode (TC_CMR value).
 *------------------------------------------------------------------------------*/
void TC_Configure(Tc *pTc, unsigned char channel, unsigned int mode)
{
    TcChannel *pTcCh = &pTc->TC_CHANNEL[channel];

    /*  Disable TC clock */
    pTcCh->TC_CCR = TC_CCR0_CLKDIS;

    /*  Disable interrupts */
    pTcCh->TC_IDR = 0xFFFFFFFF;

    /*  Clear status register */
    pTcCh->TC_SR;

    /*  Set mode */
    pTcCh->TC_CMR = mode;
}

/**------------------------------------------------------------------------------
 * \brief Reset and Start the TC Channel
 * Enables the timer clock and performs a software reset to start the counting.
 * \param pTc  Pointer to an AT91S_TC instance.
 * \param channel Channel number.
 *------------------------------------------------------------------------------*/
void TC_Start(Tc *pTc, unsigned char channel)
{
    TcChannel *pTcCh = &pTc->TC_CHANNEL[channel];
    pTcCh->TC_CCR = TC_CCR0_CLKEN | TC_CCR0_SWTRG;
}

/**------------------------------------------------------------------------------
 * \brief Stop TC Channel
 * Disables the timer clock, stopping the counting.
 * \param pTc     Pointer to an AT91S_TC instance.
 * \param channel Channel number.
 *------------------------------------------------------------------------------*/
void TC_Stop(Tc *pTc, unsigned char channel)
{
    TcChannel *pTcCh = &pTc->TC_CHANNEL[channel];
    pTcCh->TC_CCR = TC_CCR0_CLKDIS;
}

/**------------------------------------------------------------------------------
 * \brief Find best MCK divisor
 * Finds the best MCK divisor given the timer frequency and MCK. The result
 * is guaranteed to satisfy the following equation:
 * \pre
 *   (MCK / (DIV * 65536)) <= freq <= (MCK / DIV)
 * \endpre
 * with DIV being the highest possible value.
 * \param freq  Desired timer frequency.
 * \param mck  Master clock frequency.
 * \param div  Divisor value.
 * \param tcclks  TCCLKS field value for divisor.
 * \return 1 if a proper divisor has been found; otherwise 0.
 *------------------------------------------------------------------------------*/
unsigned char TC_FindMckDivisor(
    unsigned int freq,
    unsigned int mck,
    unsigned int *div,
    unsigned int *tcclks)
{
    const unsigned int divisors[5] = {2, 8, 32, 128,BOARD_MCK / 32768};

    unsigned int index = 0;

    /*  Satisfy lower bound */
    while (freq < ((mck / divisors[index]) / 65536)) {

        index++;

        /*  If no divisor can be found, return 0 */
        if (index == 5) {

            return 0;
        }
    }

    /*  Try to maximize DIV while satisfying upper bound */
    while (index < 4) {

        if (freq > (mck / divisors[index + 1])) {

            break;
        }
        index++;
    }

    /*  Store results */
    if (div) {

        *div = divisors[index];
    }
    if (tcclks) {

        *tcclks = index;
    }

    return 1;
}

