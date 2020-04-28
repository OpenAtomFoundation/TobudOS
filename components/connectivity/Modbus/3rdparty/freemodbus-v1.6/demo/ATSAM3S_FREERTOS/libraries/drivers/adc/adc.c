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

/** \addtogroup adc_module Working with ADC
 * The ADC driver provides the interface to configure and use the ADC peripheral.
 * \n
 *
 * It converts the analog input to digital format. The converted result could be
 * 12bit or 10bit. The ADC supports up to 16 analog lines.
 *
 * To Enable a ADC conversion,the user has to follow these few steps:
 * <ul>
 * <li> Select an appropriate reference voltage on ADVREF   </li>
 * <li> Configure the ADC according to its requirements and special needs,which
 * could be  broken down into several parts:
 * -#   Select the resolution by setting or clearing ADC_MR_LOWRES bit in
 *      ADC_MR (Mode Register)
 * -#   Set ADC clock by setting ADC_MR_PRESCAL bits in ADC_MR, the clock is
 *      calculated with ADCClock = MCK / ( (PRESCAL+1) * 2 )
 * -#   Set Startup Time,Tracking Clock cycles and Transfer Clock respectively
 *      in ADC_MR.
   </li>
 * <li> Start conversion by setting ADC_CR_START in ADC_CR. </li>
 * </ul>
 *
 * For more accurate information, please look at the ADC section of the
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
#include <adc/adc.h>

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
  * \brief Initialize the ADC controller
  * \param pAdc Pointer to an Adc instance.
  * \param trgEn trigger mode, software or Hardware
  * \param trgSel hardware trigger selection
  * \param sleepMode sleep mode selection
  * \param resolution resolution selection 10 bits or 12 bits
  * \param mckClock value of MCK in Hz
  * \param adcClock value of the ADC clock in Hz
  * \param startupTime value of the start up time (in ADCClock) (see datasheet)
  * \param trackingt Tracking Time (in ADCClock cycle)
*/
void ADC_Initialize(Adc *pAdc, uint8_t idAdc, uint8_t trgEn, uint8_t trgSel,
        uint8_t sleepMode, uint8_t resolution, uint32_t mckClock,
        uint32_t adcClock, uint32_t startup, uint32_t tracking) {
    uint32_t prescal;
    prescal = (mckClock / (2*adcClock)) - 1;
    
    ASSERT( (prescal<0x3F), "ADC Bad PRESCAL\n\r");

    TRACE_DEBUG("adcClock:%lu MasterClock:%lu\n\r", (mckClock/((prescal+1)*2)),
            mckClock);
    
    if( adcClock != (mckClock/((prescal+1)*2)) ) {
        TRACE_WARNING("User and calculated adcClocks are different : "
                "user=%lu calc=%lu\n\r",
            adcClock, (mckClock/((prescal+1)*2)));
    }

    /* Enable peripheral clock*/    
    PMC->PMC_PCER0 = 1 << idAdc;    
    
    /*  Reset the controller */
    ADC_SoftReset(pAdc);

    /*  Write to the MR register */
    ADC_CfgModeReg( pAdc,
          ( trgEn & ADC_MR_TRGEN)
        | ( trgSel & ADC_MR_TRGSEL)
        | ( resolution & ADC_MR_LOWRES)			
        | ( sleepMode & ADC_MR_SLEEP)
        | ( (prescal<<8) & ADC_MR_PRESCAL)
        | ( (startup<<16) & ADC_MR_STARTUP) 
        | ( (tracking<<24) & ADC_MR_TRACKTIM) );
}

/**
 * Return the Channel Converted Data
 * \param pAdc Pointer to an Adc instance.
 * \param channel channel to get converted value
 */
uint32_t ADC_GetConvertedData(Adc *pAdc, uint32_t channel) {
    uint32_t data=0;
    
    if (15 >= channel) {
      data =  *((RoReg *)((uint32_t)&(pAdc->ADC_CDR0)+ channel*4));
    }
    return data;    
}
/**
 * Set compare channel
 * \param pAdc Pointer to an Adc instance.
 * \param channel channel number to be set,16 for all channels
 */
void ADC_SetCompareChannel(Adc *pAdc, uint8_t channel) {
    ASSERT(channel<=16, "Invalid channel number");
    if (channel < 16) {
        pAdc->ADC_EMR &= ~(ADC_EMR_CMPALL);
        pAdc->ADC_EMR &= ~(ADC_EMR_CMPSEL);
        pAdc->ADC_EMR |= (channel << 4);
    } else {
        pAdc->ADC_EMR |= ADC_EMR_CMPALL;
    }
}
/**
 * Set compare mode
 * \param pAdc Pointer to an Adc instance.
 * \param mode compare mode
 */
void ADC_SetCompareMode(Adc *pAdc, uint8_t mode) {
    pAdc->ADC_EMR &= ~(ADC_EMR_CMPMODE);
    pAdc->ADC_EMR |= mode;
}
/**
 * Set comparsion window,one thereshold each time
 * \param pAdc Pointer to an Adc instance.
 * \param hi_lo Comparison Window
 */
void ADC_SetComparisonWindow(Adc *pAdc, uint32_t hi_lo) {
  pAdc->ADC_CWR = hi_lo;
}

/**----------------------------------------------------------------------------
* Test if ADC Interrupt is Masked 
* \param pAdc Pointer to an Adc instance.
* \param flag flag to be tested
* \return 1 if interrupt is masked, otherwise 0
*/
uint32_t ADC_IsInterruptMasked(Adc *pAdc, uint32_t flag) {
    return (ADC_GetInterruptMaskStatus(pAdc) & flag);
}

/**----------------------------------------------------------------------------
* Test if ADC Status is Set
* \param pAdc Pointer to an Adc instance.
* \param flag flag to be tested
* \return 1 if the staus is set; 0 otherwise
*/
uint32_t ADC_IsStatusSet(Adc *pAdc, uint32_t flag) {
    return (ADC_GetStatus(pAdc) & flag);
}

/**----------------------------------------------------------------------------
* Test if ADC channel interrupt Status is Set
* \param adc_sr Value of SR register
* \param channel Channel to be tested
* \return 1 if interrupt status is set, otherwise 0
*/
uint8_t ADC_IsChannelInterruptStatusSet(uint32_t adc_sr, uint32_t channel) {
    uint8_t status;
    
    if((adc_sr & (1<<channel)) == (1<<channel)) {
        status = 1;
    } else {
        status = 0;
    }
    return status;
}

/**
  * \brief Read converted data through PDC channel
  * \param padc the pointer of adc peripheral
  * \param pBuffer the destination buffer
  * \param size the size of the buffer
*/
int8_t ADC_ReadBuffer(Adc *pADC, int16_t *pBuffer, int32_t size) {
  
    /* Check if the first PDC bank is free*/
    if ((pADC->ADC_RCR == 0) && (pADC->ADC_RNCR == 0)) {

        pADC->ADC_RPR = (uint32_t) pBuffer;
        pADC->ADC_RCR = size;
        pADC->ADC_PTCR = ADC_PTCR_RXTEN;

        return 1;
    }
    /* Check if the second PDC bank is free*/
    else if (pADC->ADC_RNCR == 0) {

        pADC->ADC_RNPR = (uint32_t) pBuffer;
        pADC->ADC_RNCR = size;

        return 1;
    } else {

        return 0;
    }
  
}

