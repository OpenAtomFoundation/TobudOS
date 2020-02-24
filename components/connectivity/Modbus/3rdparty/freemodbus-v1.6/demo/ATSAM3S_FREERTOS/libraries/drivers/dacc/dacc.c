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

/** \addtogroup dacc_module Working with DACC
 * The DACC driver provides the interface to configure and use the DACC peripheral.\n
 *
 * The DACC(Digital-to-Analog Converter Controller) converts digital code to analog output.
 * The data to be converted are sent in a common register for all channels. It offers up to 2
 * analog outputs.The output voltage ranges from (1/6)ADVREF to (5/6)ADVREF.
 *
 * To Enable a DACC conversion,the user has to follow these few steps:
 * <ul>
 * <li> Select an appropriate reference voltage on ADVREF   </li>
 * <li> Configure the DACC according to its requirements and special needs,which could be
        broken down into several parts:
 * -#   Enable DACC in free running mode by clearing TRGEN in DACC_MR;
 * -#   Configure Startup Time and Refresh Period through setting STARTUP and REFRESH fields
 *      in DACC_MR; The refresh mechanism is used to protect the output analog value from
 *      decreasing.
 * -#   Enable channels and write digital code to DACC_CDR,in free running mode, the conversion
 *      is started right after at least one channel is enabled and data is written .
   </li>
 * </ul>
 *
 * For more accurate information, please look at the DACC section of the
 * Datasheet.
 *
 * Related files :\n
 * \ref DACC.c\n
 * \ref DACC.h\n
*/
/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include <board.h>
#include <dacc/dacc.h>



/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
  * \brief Initialize the DACC controller
  * \param pDACC Pointer to an DACC instance.
  * \param idDACC identifier of DAC peripheral
  * \param trgEn trigger mode, free running mode or external Hardware trigger
  * \param word transfer size,word or half word
  * \param trgSel hardware trigger selection
  * \param sleepMode sleep mode selection
  * \param mck value of MCK in Hz
  * \param refresh refresh period
  * \param user_sel user channel selection ,0 or 1
  * \param startup value of the start up time (in DACCClock) (see datasheet)
*/
void DACC_Initialize (Dacc *pDACC,
                     uint8_t idDACC,
                     uint8_t trgEn,
                     uint8_t trgSel,
                     uint8_t word,
                     uint8_t sleepMode,
                     uint32_t mck,
                     uint8_t refresh,/*refresh period*/
                     uint8_t user_sel,/*user channel selection*/
                     uint32_t startup
                     )
{
    ASSERT(1024*refresh*1000/(mck/2)<20,"Refresh preriod is too big!");


    /* Enable peripheral clock*/
    PMC->PMC_PCER0 = 1 << idDACC;

    /*  Reset the controller */
    DACC_SoftReset(pDACC);

    /*  Write to the MR register */
    DACC_CfgModeReg( pDACC,
          ( trgEn & DACC_MR_TRGEN)
        | ( trgSel & DACC_MR_TRGSEL)
        | ( word & DACC_MR_WORD)
        | ( sleepMode & DACC_MR_SLEEP)
        | ( (refresh<<8) & DACC_MR_REFRESH)
        | ( (user_sel<<16)& DACC_MR_USER_SEL)
        | ( (startup<<24) & DACC_MR_STARTUP));
}


/**
 * Set the Conversion Data
 * \param pDACC Pointer to an Dacc instance.
 * \param channel channel to get converted value
 * \return Channel converted data of the specified channel
 */
void DACC_SetConversionData(Dacc *pDACC, uint32_t data)
{
    uint32_t mr = pDACC->DACC_MR;

    if(mr & DACC_MR_WORD)
    {
    	pDACC->DACC_CDR = data;
    }
    else
    {
    	pDACC->DACC_CDR = (data&0xFFFF);
    }
}


/**
  * \brief Write converted data through PDC channel
  * \param pDACC the pointer of DACC peripheral
  * \param pBuffer the destination buffer
  * \param size the size of the buffer
*/
int8_t DACC_WriteBuffer(Dacc *pDACC,int16_t *pBuffer,int32_t size)
{

    /* Check if the first PDC bank is free*/
    if ((pDACC->DACC_TCR == 0) && (pDACC->DACC_TNCR == 0)) {

        pDACC->DACC_TPR = (uint32_t) pBuffer;
        pDACC->DACC_TCR = size;
        pDACC->DACC_PTCR = DACC_PTCR_RXTEN;

        return 1;
    }
    /* Check if the second PDC bank is free*/
    else if (pDACC->DACC_TNCR == 0) {

        pDACC->DACC_TNPR = (uint32_t) pBuffer;
        pDACC->DACC_TNCR = size;

        return 1;
    }
    else {

        return 0;
    }

}


