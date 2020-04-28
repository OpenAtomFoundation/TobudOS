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

/**
 *  \unit
 * 
 *  !Purpose
 * 
 *  Interface for configuration the Analog-to-Digital Converter (ACC) peripheral.
 * 
 *  !Usage
 * 
 *  -# Configurate the pins for ACC
 *  -# Initialize the ACC with ACC_Initialize().
 *  -# Select the active channel using ACC_EnableChannel()
 *  -# Start the conversion with ACC_StartConversion()
 *  -# Wait the end of the conversion by polling status with ACC_GetStatus()
 *  -# Finally, get the converted data using ACC_GetConvertedData()
 * 
*/
#ifndef ACC_H
#define ACC_H

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include <utility/assert.h>

/*------------------------------------------------------------------------------
 *         Definitions
 *------------------------------------------------------------------------------*/
#define ACC_SELPLUS_AD12B0 0
#define ACC_SELPLUS_AD12B1 1
#define ACC_SELPLUS_AD12B2 2
#define ACC_SELPLUS_AD12B3 3
#define ACC_SELPLUS_AD12B4 4
#define ACC_SELPLUS_AD12B5 5
#define ACC_SELPLUS_AD12B6 6
#define ACC_SELPLUS_AD12B7 7
#define ACC_SELMINUS_TS 0
#define ACC_SELMINUS_ADVREF 1
#define ACC_SELMINUS_DAC0   2
#define ACC_SELMINUS_DAC1   3
#define ACC_SELMINUS_AD12B0 4
#define ACC_SELMINUS_AD12B1 5
#define ACC_SELMINUS_AD12B2 6
#define ACC_SELMINUS_AD12B3 7


/*------------------------------------------------------------------------------
 *         Macros function of register access
 *------------------------------------------------------------------------------*/
#define ACC_CfgModeReg(pAcc, mode)  { \
            (pAcc)->ACC_MR = (mode);\
        }

#define ACC_GetModeReg(pAcc)                ((pAcc)->ACC_MR)

#define ACC_StartConversion(pAcc)           ((pAcc)->ACC_CR = ACC_CR_START)

#define ACC_SoftReset(pAcc)                 ((pAcc)->ACC_CR = ACC_CR_SWRST)

#define ACC_EnableChannel(pAcc, channel)    {\
            ASSERT(channel < 16, "ACC Channel not exist");\
            (pAcc)->ACC_CHER = (1 << (channel));\
        }

#define ACC_DisableChannel (pAcc, channel)  {\
            ASSERT((channel) < 16, "ACC Channel not exist");\
            (pAcc)->ACC_CHDR = (1 << (channel));\
        }

#define ACC_EnableIt(pAcc, mode)            {\
            ASSERT(((mode)&0xFFF00000)== 0, "ACC bad interrupt IER");\
            (pAcc)->ACC_IER = (mode);\
        }

#define ACC_DisableIt(pAcc, mode)           {\
            ASSERT(((mode)&0xFFF00000)== 0, "ACC bad interrupt IDR");\
            (pAcc)->ACC_IDR = (mode);\
        }

#define ACC_EnableDataReadyIt(pAcc)         ((pAcc)->ACC_IER = AT91C_ACC_DRDY)

#define ACC_GetStatus(pAcc)                 ((pAcc)->ACC_ISR)

#define ACC_GetChannelStatus(pAcc)          ((pAcc)->ACC_CHSR)

#define ACC_GetInterruptMaskStatus(pAcc)    ((pAcc)->ACC_IMR)

#define ACC_GetLastConvertedData(pAcc)      ((pAcc)->ACC_LCDR)

#define ACC_CfgAnalogCtrlReg(pAcc,mode)     {\
            ASSERT(((mode) & 0xFFFCFF3C)==0, "ACC bad analog control config");\
            (pAcc)->ACC_ACR = (mode);\
        }

#define ACC_CfgExtModeReg(pAcc, extmode)    {\
            ASSERT(((extmode) & 0xFF00FFFE)==0, "ACC bad extended mode config");\
            (pAcc)->ACC_EMR = (extmode);\
        }

#define ACC_GetAnalogCtrlReg(pAcc)          ((pAcc)->ACC_ACR)

/*------------------------------------------------------------------------------
 *         Exported functions
 *------------------------------------------------------------------------------*/
void ACC_Configure(Acc *pAcc,
                     uint8_t idAcc,
                     uint8_t selplus,
                     uint8_t selminus,
                     uint16_t ac_en,
                     uint16_t edge,        
                     uint16_t invert);

extern uint8_t ACC_GetComparisionResult(Acc *pAcc,uint32_t status);

#endif //#ifndef ACC_H
