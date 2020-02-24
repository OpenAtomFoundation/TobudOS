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

#ifndef PIO_KEYPAD_H
#define PIO_KEYPAD_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <board.h>

//------------------------------------------------------------------------------
//         Global Definitions
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
//         Global Macros
//------------------------------------------------------------------------------
//enable keypad press interrupt
#define PIO_KeyPadEnableKPIt(pPIO)	((pPIO)->KIER = 1<<0)

//enable keypad release interrupt
#define PIO_KeyPadEnableKRIt(pPIO)	((pPIO)->KIER = 1<<1)

//disable keypad press interrupt
#define PIO_KeyPadDisableKPIt(pPIO)	((pPIO)->KIDR = 1<<0)

//disable keypad release interrupt
#define PIO_KeyPadDisableKRIt(pPIO)	((pPIO)->KIDR = 1<<1)

//enable keypad controller interrupt
#define PIO_KeyPadEnableIt(pPIO, mode)	{switch(mode):\
						case 1:PIO_KeyPadEnableKPIt(pPIO);break;\
						case 2:PIO_KeyPadEnableKRIt(pPIO);break;\
						case 3:PIO_KeyPadEnableKPIt(pPIO);\
						       PIO_KeyPadEnableKRIt(pPIO);break;\
						default:break;\
					}

//disable keypad controller interrupt
#define PIO_KeyPadDisableIt(pPIO, mode) {switch(mode):\
						case 1:PIO_KeyPadDisableKPIt(pPIO);break;\
						case 2:PIO_KeyPadDisableKRIt(pPIO);break;\
						case 3:PIO_KeyPadDisableKPIt(pPIO);\
						       PIO_KeyPadDisableKRIt(pPIO);break;\
						default:break;\
					}

//get keypad controller interrupt mask
#define PIO_KeyPadGetItMask(pPIO)	((pPIO)->PIO_KIMR)


//------------------------------------------------------------------------------
/// Calculates the size of an array of Pin instances. The array must be defined
/// locally (i.e. not a pointer), otherwise the computation will not be correct.
/// \param pPins  Local array of Pin instances.
/// \return Number of elements in array.
//------------------------------------------------------------------------------
#define PIO_LISTSIZE(pPins)    (sizeof(pPins) / sizeof(Pin))

//------------------------------------------------------------------------------
//         Global Types
//------------------------------------------------------------------------------
typedef enum {
	FALSE,
	TRUE
} bool;

typedef struct _KeyPadConfig {
	bool enable;//keypad controller enable or disable
	unsigned char col:3;//config column size
	unsigned char row:3;//config row size
        unsigned int debouncing;//config debouncing
} KeyPadConfig;


typedef struct _KeyColRow {
      unsigned char row:3;
      unsigned char col:3;
} KeyColRow;

 
typedef struct _KeyDownEvent {
      bool press;//at least 1 pressed key detected, or 0
      unsigned char keyPressNum;//simultaneously pressed key number
      KeyColRow preKeyMatrix[4];//pressed key matrix
} KeyDownEvent;

 

typedef struct _KeyUpEvent {
      bool release;//at least 1 released key  detected, or 0
      unsigned char keyRelNum;//simultaneously released key number
      KeyColRow relKeyMatrix[4];//released key matrix
} KeyUpEvent;

      

typedef struct _KeyEvent {
      KeyDownEvent kdEvent;
      KeyUpEvent   kuEvent;
} KeyEvent;

//------------------------------------------------------------------------------
//         Global Access Macros 
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//         Global Functions
//------------------------------------------------------------------------------

void PIO_KeyPadConfig(AT91S_PIO *pPIO, KeyPadConfig *config);

void PIO_GetKeyStatus(AT91S_PIO *pPIO, KeyEvent *event);

void PIO_KeypadEnableIt(AT91S_PIO *pio, unsigned int mode);

void PIO_KeypadDisableIt(AT91S_PIO *pio, unsigned int mode);


#endif //#ifndef PIO_KEYPAD_H

