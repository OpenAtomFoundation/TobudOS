/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//*****************************************************************************
//
// saph.c - Driver for the SAPH Module.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup saph_api saph
//! @{
//
//*****************************************************************************

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_SAPH__
#include "saph.h"

#include <assert.h>

void SAPH_unlock(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPHKEY) = KEY;
}

void SAPH_lock(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPHKEY) = 0;
}

void SAPH_clearInterrupt(uint16_t baseAddress, uint8_t mask)
{
    HWREG16(baseAddress + OFS_SAPHICR) |= mask;
}

void SAPH_enableInterrupt(uint16_t baseAddress, uint8_t mask)
{
    HWREG16(baseAddress + OFS_SAPHIMSC) |= mask;
}

void SAPH_disableInterrupt(uint16_t baseAddress, uint8_t mask)
{
    HWREG16(baseAddress + OFS_SAPHIMSC) &= ~mask;
}

uint8_t SAPH_getInterruptStatus(uint16_t baseAddress, uint8_t mask)
{
    return HWREG16(baseAddress + OFS_SAPHRIS) & mask;
}

void SAPH_configurePHY(uint16_t baseAddress, SAPH_configPHYParam *config)
{
    uint8_t offset = config->channel;

    HWREG16(baseAddress + OFS_SAPHOCTL0) &= ~ ((SAPH_PHY_OUTPUT_ENABLE<<offset) 
        | (SAPH_PHY_OUTPUT_HIGH<<offset));
    HWREG16(baseAddress + OFS_SAPHOCTL0) |= ((config->outputValue<<offset) 
        | (config->enableOutput<<offset));

    HWREG16(baseAddress + OFS_SAPHOCTL1) &= ~ ((SAPH_PHY_FULLPULL_ENABLE<<offset) 
        | (SAPH_PHY_TERMINATION_ENABLE<<offset));
    HWREG16(baseAddress + OFS_SAPHOCTL1) |= ((config->enableFullPull<<offset) 
        | (config->enableTermination<<offset));

    HWREG16(baseAddress + OFS_SAPHTACTL) |= UNLOCK;

    HWREG16(baseAddress + OFS_SAPHOSEL) &= ~(PCH0SEL_3<<(offset*2));
    HWREG16(baseAddress + OFS_SAPHOSEL) |= config->outputFunction<<(offset*2);

    HWREG16(baseAddress + OFS_SAPHCH0PUT + offset*6 ) &= ~CH0PUT;
    HWREG16(baseAddress + OFS_SAPHCH0PUT + offset*6 ) |= config->pullUpTrim;

    HWREG16(baseAddress + OFS_SAPHCH0PDT + offset*6 ) &= ~CH0PDT;
    HWREG16(baseAddress + OFS_SAPHCH0PDT + offset*6 ) |= config->pullDownTrim;

    HWREG16(baseAddress + OFS_SAPHCH0TT + offset*6 ) &= ~CH0TT;
    HWREG16(baseAddress + OFS_SAPHCH0TT + offset*6 ) |= config->terminationTrim;

}

void SAPH_configurePHYMultiplexer(uint16_t baseAddress,
    uint16_t dummyLoad, uint16_t source, uint16_t input)
{
    HWREG16(baseAddress + OFS_SAPHICTL0) &= ~(DUMEN | MUXCTL | MUXSEL);
    HWREG16(baseAddress + OFS_SAPHICTL0) |= dummyLoad | source | input;
}

void SAPH_configurePHYBias(uint16_t baseAddress, SAPH_configPHYBiasParam * config)
{
    uint8_t i = 0;
    for (i=0; i<SAPH_PHY_CHANNEL_COUNT; i++) {
        HWREG16(baseAddress + OFS_SAPHBCTL) &= ~(CH0EBSW<<i);
        HWREG16(baseAddress + OFS_SAPHBCTL) |= config->biasSwitch[i]<<i;
    }

    HWREG16(baseAddress + OFS_SAPHBCTL) &= ~(PGABIAS_3 | EXCBIAS_3 | CPDA | LILC
        | PGABSW | ASQBSC);
    HWREG16(baseAddress + OFS_SAPHBCTL) |= (config->biasPGA | config->biasExcitation
        | config->enableChargePump | config->enableLeakageCompensation
        | config->biasSwitchPGA | config->biasSwitchASQ);
}

void SAPH_configurePPGCount(uint16_t baseAddress, SAPH_configPPGCountParam *config)
{
    HWREG16(baseAddress + OFS_SAPHPGC) &= ~(PHIZ | PLEV | PPOL | SPULS | EPULS);
    HWREG16(baseAddress + OFS_SAPHPGC) |= (config->highImpedance 
        | config->pauseLevel | config->pausePolarity | config->stopPauseCount
        | config->excitationPulseCount);
}

void SAPH_setPPGLowPeriod(uint16_t baseAddress, uint16_t period)
{
    HWREG16(baseAddress + OFS_SAPHPGLPER) &= ~LPER;
    HWREG16(baseAddress + OFS_SAPHPGLPER) |= period;
}

void SAPH_setPPGHighPeriod(uint16_t baseAddress, uint16_t period)
{
    HWREG16(baseAddress + OFS_SAPHPGHPER) &= ~HPER;
    HWREG16(baseAddress + OFS_SAPHPGHPER) |= period;
}

void SAPH_configurePPG(uint16_t baseAddress, SAPH_configPPGParam *config)
{
    HWREG16(baseAddress + OFS_SAPHPGCTL) &= ~(PSCEN | PPGEN | TRSEL_3 | PPGCHSEL | PGSEL);
    HWREG16(baseAddress + OFS_SAPHPGCTL) |= (config->enablePrescaler | PPGEN |
        config->triggerSource | config->channelSelect | config->portSelect);

}

void SAPH_triggerPPG(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPHPPGTRIG) |= PPGTRIG;
}

void SAPH_stopPPG(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPHPGCTL) |= PPGSTOP;
}

void SAPH_configureASQ(uint16_t baseAddress, SAPH_configASQParam *config)
{
    HWREG16(baseAddress + OFS_SAPHASCTL0) &= ~(ERABRT | TRIGSEL | ASQTEN | ASQCHSEL);
    HWREG16(baseAddress + OFS_SAPHASCTL1) &= ~(CHOWN | STDBY | ESOFF | EARLYRB | CHTOG);

    HWREG16(baseAddress + OFS_SAPHASCTL1) |= config->sideOfChannel | config->standByIndication
        | config->endOfSequence | config->earlyReceiveBias | config->enableChannelToggle;
    HWREG16(baseAddress + OFS_SAPHASCTL0) |= config->abortOnError | config->triggerSource
        | config->channelSelect | ASQTEN;
}

void SAPH_triggerASQ(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPHASQTRIG) |= ASQTRIG;
}

void SAPH_stopASQ(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPHASCTL0) |= ASQSTOP;
}

uint8_t SAPH_getASQPingCounter(uint16_t baseAddress)
{
    return HWREG8(baseAddress + OFS_SAPHASCTL0) & PNGCNT;
}

void SAPH_configureASQPing(uint16_t baseAddress, SAPH_configASQPingParam *config)
{
    HWREG16(baseAddress + OFS_SAPHAPOL) &= ~PCPOL;
    HWREG16(baseAddress + OFS_SAPHAPOL) |= config->polarity;

    HWREG16(baseAddress + OFS_SAPHAPLEV) &= ~PCPLEV;
    HWREG16(baseAddress + OFS_SAPHAPLEV) |= config->pauseLevel;

    HWREG16(baseAddress + OFS_SAPHAPHIZ) &= ~PCPHIZ;
    HWREG16(baseAddress + OFS_SAPHAPHIZ) |= config->pauseHighImpedance;
}

void SAPH_configureASQTimeBase(uint16_t baseAddress, uint16_t prescalerStartValue)
{
    HWREG16(baseAddress + OFS_SAPHTBCTL) &= ~(PSSV);
    HWREG16(baseAddress + OFS_SAPHTBCTL) |= prescalerStartValue;
}

void SAPH_startASQTimer(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPHTBCTL) |= TSTART;
}

void SAPH_stopASQTimer(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPHTBCTL) |= TSTOP;
}

void SAPH_clearASQTimer(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPHTBCTL) |= TCLR;
}

void SAPH_configureMode(uint16_t baseAddress, SAPH_configModeParam *config)
{
    HWREG16(baseAddress + OFS_SAPHMCNF) &= ~LPBE;
    HWREG16(baseAddress + OFS_SAPHMCNF) |= config->lowPowerBiasMode;

    HWREG16(baseAddress + OFS_SAPHMCNF) &= ~CPEO;
    HWREG16(baseAddress + OFS_SAPHMCNF) |= config->chargePump;

    HWREG16(baseAddress + OFS_SAPHMCNF) &= ~BIMP;
    HWREG16(baseAddress + OFS_SAPHMCNF) |= config->biasImpedance;
}

void SAPH_enableLowPowerBiasMode(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPHMCNF) |= LPBE;
}

void SAPH_disableLowPowerBiasMode(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPHMCNF) &= ~LPBE;
}

void SAPH_enableChargePumpAlways(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPHMCNF) |= CPEO;
}

void SAPH_enableChargePumpOnAcquisitionOnly(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPHMCNF) &= ~CPEO;
}

void SAPH_setBiasGeneratorImpedance(uint16_t baseAddress, uint16_t biasImpedance)
{
    HWREG16(baseAddress + OFS_SAPHMCNF) &= ~BIMP;
    HWREG16(baseAddress + OFS_SAPHMCNF) |= biasImpedance;
}

#endif
//*****************************************************************************
//
//! Close the doxygen group for saph_api
//! @}
//
//*****************************************************************************

