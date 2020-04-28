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

#include <board.h>
#include "pio.h"
#include "pio_keypad.h"


//------------------------------------------------------------------------------
//         Global Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Configures keypad controller
/// \param pPIO	Pointer to a PIO instance
/// \param config  Configuration data for given pin,see head file for detail
//------------------------------------------------------------------------------
void PIO_KeyPadConfig(AT91S_PIO *pPIO, KeyPadConfig *config)
{
    //enable/disable keypad controller 
    pPIO->PIO_KER = config->enable;

    //if enable, set keypad matrix and debouncing
    if(config->enable == TRUE) {
        //set key matrix
        pPIO->PIO_KRCR = (config->row | config->col<<8) ;

        //set debouncing
         pPIO->PIO_KDR = config->debouncing;

    }
}


//------------------------------------------------------------------------------
/// Get Key Press/Release status
/// \param pPIO  Pointer to a PIO instance
/// \param event  Pointer to a instance of KeyEvent for storing keypad status
//------------------------------------------------------------------------------
void PIO_GetKeyStatus(AT91S_PIO *pPIO, KeyEvent *event)
{
    int i,j;
    
    //get key press event
    event->kdEvent.press = (pPIO->KSR&0x1)?TRUE:FALSE;
    event->kdEvent.keyPressNum = (pPIO->KSR>>8)&0x3;
    j=event->kdEvent.keyPressNum+1;
    for(i=0; i<j; i++) {
        event->kdEvent.preKeyMatrix[i].row = ((pPIO->KKPR) >> (8*i)) & 0x7;
        event->kdEvent.preKeyMatrix[i].col = ((pPIO->KKPR) >> (8*i+4)) & 0x7;
    }

    //get key release event
    event->kuEvent.release = ((pPIO->KSR>>1) & 0x1)?TRUE:FALSE;
    event->kuEvent.keyRelNum = (pPIO->KSR>>16)&0x3;
    j=event->kdEvent.keyPressNum+1;
    for(i=0;i<j;i++) {
        event->kuEvent.relKeyMatrix[i].row = ((pPIO->KKRR) >> (8*i)) & 0x7;
        event->kuEvent.relKeyMatrix[i].col = ((pPIO->KKRR) >> (8*i+4)) & 0x7;
    }

}


//------------------------------------------------------------------------------
/// Enable keypad interrupt as Key Press Interrupt or Key Release Interrupt or both
/// \param pPIO Pointer to a PIO instance
/// \param mode  Select key interrupt mode to enable, 
///		0x1 		Key Press Interrupt
///		0x2		Key Release Interrupt
///		0x3		both of two type
//------------------------------------------------------------------------------
void PIO_KeypadEnableIt(AT91S_PIO *pPIO, unsigned int mode)
{
    switch(mode){
    case 1:PIO_KeyPadEnableKPIt(pPIO);
	   break;

    case 2:PIO_KeyPadEnableKRIt(pPIO);
	   break;

    case 3:PIO_KeyPadEnableKPIt(pPIO);
	   PIO_KeyPadEnableKRIt(pPIO);
	   break;

	default:break;
    }

}

//------------------------------------------------------------------------------
/// Disable Key Press Interrupt or Key Release Interrupt or both of them
/// \param pPIO Pointer to a PIO instance
/// \param mode  Select key interrupt mode to disable, 
///		0x1 		Key Press Interrupt
///		0x2		Key Release Interrupt
///		0x3		both of two type
//------------------------------------------------------------------------------
void PIO_KeypadDisableIt(AT91S_PIO *pPIO, unsigned int mode)
{
    switch(mode){
        case 1:PIO_KeyPadDisableKPIt(pPIO);
   	       break;

        case 2:PIO_KeyPadDisableKRIt(pPIO);
   	       break;

        case 3:PIO_KeyPadDisableKPIt(pPIO);
    	       PIO_KeyPadDisableKRIt(pPIO);
    	       break;

        default:break;
    }

}


