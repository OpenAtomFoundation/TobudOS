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

/** \addtogroup adc_module Working with ACC
 * The ACC driver provides the interface to configure and use the ACC peripheral.\n
 *
 * It converts the analog input to digital format. The converted result could be 12bit
 * or 10bit. The ACC supports up to 16 analog lines.
 *
 * To Enable a ACC conversion,the user has to follow these few steps:
 * <ul>
 * <li> Select an appropriate reference voltage on ADVREF   </li>
 * <li> Configure the ACC according to its requirements and special needs,which could be
        broken down into several parts:
 * -#   Select the resolution by setting or clearing ACC_MR_LOWRES bit in ACC_MR (Mode Register)
 * -#   Set ACC clock by setting ACC_MR_PRESCAL bits in ACC_MR, the clock is caculated with
        ACCClock = MCK / ( (PRESCAL+1) * 2 )
 * -#   Set Startup Time,Tracking Clock cycles and Transfer Clock respeticively in ACC_MR.
   </li>
 * <li> Start conversion by setting ACC_CR_START in ACC_CR. </li>
 * </ul>
 *
 * For more accurate information, please look at the ACC section of the
 * Datasheet.
 *
 * Related files :\n
 * \ref adc.c\n
 * \ref adc.h\n
*/
/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include <board.h>
#include <acc/acc.h>



/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
  * \brief Initialize the ACC controller
  * \param pAcc Pointer to an Acc instance.
  * \param idAcc ACC identifier
  * \param selplus input connected to inp, 0~7
  * \param selminus input connected to inm,0~7
  * \param ac_en Analog comprator enabled/disabled,use pattern
  * \param edge CF flag triggering mode,use pattern
  * \param invert INVert comparator output,use pattern defined in the device header
           file
*/
void ACC_Configure(Acc *pAcc,
                     uint8_t idAcc,
                     uint8_t selplus,
                     uint8_t selminus,
                     uint16_t ac_en,
                     uint16_t edge,
                     uint16_t invert)
{

    /* Enable peripheral clock*/
    PMC->PMC_PCER1 = 1 << (idAcc-32);

    /*  Reset the controller */
    pAcc->ACC_CR |= ACC_CR_SWRST;

    /*  Write to the MR register */
    ACC_CfgModeReg( pAcc,
        ( selplus & ACC_MR_SELPLUS)
        | (( selminus<<4) & ACC_MR_SELMINUS)			
        | ( ac_en & ACC_MR_ACEN)
        | ( edge & ACC_MR_EDGETYP)
        | ( invert & ACC_MR_INV) );
    //pAcc->ACC_MR |= (ACC_MR_SELFS_OUTPUT|ACC_MR_FE_EN);

    pAcc->ACC_ACR = 0x7;

    while(pAcc->ACC_ISR & (uint32_t)ACC_ISR_MASK);
}


/**
 * Return the Channel Converted Data
 * \param pAdc Pointer to an Adc instance.
 * \param channel channel to get converted value
 * \return Channel converted data of the specified channel
 */
void ACC_SetComparisionPair(Acc *pAcc, uint8_t selplus,uint8_t selminus)
{

    uint32_t temp;

    ASSERT(selplus < 8 && selminus < 8,"The assigned channel number is invalid!");

    temp =  pAcc->ACC_MR;

    pAcc->ACC_MR = temp & (~ACC_MR_SELMINUS)&(~ACC_MR_SELPLUS);

    pAcc->ACC_MR |= ((selplus & ACC_MR_SELPLUS)|((selminus<<4) & ACC_MR_SELMINUS));

}
/**
 * Return Comparison Result
 * \param pAcc Pointer to an Acc instance.
 * \param status value of ACC_ISR
 */
uint8_t ACC_GetComparisionResult(Acc *pAcc,uint32_t status)
{
  uint32_t temp = pAcc->ACC_MR;
  if( (temp & ACC_MR_INV)== ACC_MR_INV)
  {

    if( status & ACC_ISR_SCO)
    {
      return 0; /* inn>inp*/
    }
    else return 1;/* inp>inn*/

  }
  else
  {

     if( status & ACC_ISR_SCO)
     {
      return 1; /* inp>inn*/
     }
     else return 0;/* inn>inp*/
  }

}





