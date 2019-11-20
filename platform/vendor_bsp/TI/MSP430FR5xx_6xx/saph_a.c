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
// saph_a.c - Driver for the SAPH_A Module.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup saph_a_api saph_a
//! @{
//
//*****************************************************************************

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_SAPH_A__
#include "saph_a.h"

#include <assert.h>

void SAPH_A_unlock(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPH_AKEY) = KEY;
}

void SAPH_A_lock(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPH_AKEY) = 0;
}

void SAPH_A_clearInterrupt(uint16_t baseAddress, uint8_t mask)
{
    HWREG16(baseAddress + OFS_SAPH_AICR) |= mask;
}

void SAPH_A_enableInterrupt(uint16_t baseAddress, uint8_t mask)
{
    HWREG16(baseAddress + OFS_SAPH_AIMSC) |= mask;
}

void SAPH_A_disableInterrupt(uint16_t baseAddress, uint8_t mask)
{
    HWREG16(baseAddress + OFS_SAPH_AIMSC) &= ~mask;
}

uint8_t SAPH_A_getInterruptStatus(uint16_t baseAddress, uint8_t mask)
{
    return HWREG16(baseAddress + OFS_SAPH_ARIS) & mask;
}

void SAPH_A_configurePHY(uint16_t baseAddress, SAPH_A_configPHYParam *config)
{
    uint8_t offset = config->channel;

    HWREG16(baseAddress + OFS_SAPH_AOCTL0) &= ~ ((SAPH_A_PHY_OUTPUT_ENABLE<<offset) 
        | (SAPH_A_PHY_OUTPUT_HIGH<<offset));
    HWREG16(baseAddress + OFS_SAPH_AOCTL0) |= ((config->outputValue<<offset) 
        | (config->enableOutput<<offset));

    HWREG16(baseAddress + OFS_SAPH_AOCTL1) &= ~ ((SAPH_A_PHY_FULLPULL_ENABLE<<offset) 
        | (SAPH_A_PHY_TERMINATION_ENABLE<<offset));
    HWREG16(baseAddress + OFS_SAPH_AOCTL1) |= ((config->enableFullPull<<offset) 
        | (config->enableTermination<<offset));

    HWREG16(baseAddress + OFS_SAPH_ATACTL) |= UNLOCK;

    HWREG16(baseAddress + OFS_SAPH_AOSEL) &= ~(PCH0SEL_3<<(offset*2));
    HWREG16(baseAddress + OFS_SAPH_AOSEL) |= config->outputFunction<<(offset*2);

    HWREG16(baseAddress + OFS_SAPH_ACH0PUT + offset*6 ) &= ~CH0PUT;
    HWREG16(baseAddress + OFS_SAPH_ACH0PUT + offset*6 ) |= config->pullUpTrim;

    HWREG16(baseAddress + OFS_SAPH_ACH0PDT + offset*6 ) &= ~CH0PDT;
    HWREG16(baseAddress + OFS_SAPH_ACH0PDT + offset*6 ) |= config->pullDownTrim;

    HWREG16(baseAddress + OFS_SAPH_ACH0TT + offset*6 ) &= ~CH0TT;
    HWREG16(baseAddress + OFS_SAPH_ACH0TT + offset*6 ) |= config->terminationTrim;

}

void SAPH_A_configurePHYMultiplexer(uint16_t baseAddress,
    uint16_t dummyLoad, uint16_t source, uint16_t input)
{
    HWREG16(baseAddress + OFS_SAPH_AICTL0) &= ~(DUMEN | MUXCTL | MUXSEL);
    HWREG16(baseAddress + OFS_SAPH_AICTL0) |= dummyLoad | source | input;
}

void SAPH_A_configurePHYBias(uint16_t baseAddress, SAPH_A_configPHYBiasParam * config)
{
    uint8_t i = 0;
    for (i=0; i<SAPH_A_PHY_CHANNEL_COUNT; i++) {
        HWREG16(baseAddress + OFS_SAPH_ABCTL) &= ~(CH0EBSW<<i);
        HWREG16(baseAddress + OFS_SAPH_ABCTL) |= config->biasSwitch[i]<<i;
    }

    HWREG16(baseAddress + OFS_SAPH_ABCTL) &= ~(PGABIAS_3 | EXCBIAS_3 | CPDA | LILC
        | PGABSW | ASQBSC);
    HWREG16(baseAddress + OFS_SAPH_ABCTL) |= (config->biasPGA | config->biasExcitation
        | config->enableChargePump | config->enableLeakageCompensation
        | config->biasSwitchPGA | config->biasSwitchASQ);
}

void SAPH_A_configurePPGCount(uint16_t baseAddress, SAPH_A_configPPGCountParam *config)
{
    HWREG16(baseAddress + OFS_SAPH_APGC) &= ~(PHIZ | PLEV | PPOL | SPULS | EPULS);
    HWREG16(baseAddress + OFS_SAPH_APGC) |= (config->highImpedance 
        | config->pauseLevel | config->pausePolarity | config->stopPauseCount
        | config->excitationPulseCount);
}

void SAPH_A_setPPGLowPeriod(uint16_t baseAddress, uint16_t period)
{
    HWREG16(baseAddress + OFS_SAPH_APGLPER) &= ~LPER;
    HWREG16(baseAddress + OFS_SAPH_APGLPER) |= period;
}

void SAPH_A_setPPGHighPeriod(uint16_t baseAddress, uint16_t period)
{
    HWREG16(baseAddress + OFS_SAPH_APGHPER) &= ~HPER;
    HWREG16(baseAddress + OFS_SAPH_APGHPER) |= period;
}

void SAPH_A_configurePPG(uint16_t baseAddress, SAPH_A_configPPGParam *config)
{
    HWREG16(baseAddress + OFS_SAPH_APGCTL) &= ~(PSCEN | PPGEN | TRSEL_3 | PPGCHSEL | PGSEL);
    HWREG16(baseAddress + OFS_SAPH_APGCTL) |= (config->enablePrescaler | PPGEN |
        config->triggerSource | config->channelSelect | config->portSelect);

}

void SAPH_A_triggerPPG(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPH_APPGTRIG) |= PPGTRIG;
}

void SAPH_A_stopPPG(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPH_APGCTL) |= PPGSTOP;
}

void SAPH_A_configureASQ(uint16_t baseAddress, SAPH_A_configASQParam *config)
{
    HWREG16(baseAddress + OFS_SAPH_AASCTL0) &= ~(ERABRT | TRIGSEL | ASQTEN | ASQCHSEL);
    HWREG16(baseAddress + OFS_SAPH_AASCTL1) &= ~(CHOWN | STDBY | ESOFF | EARLYRB | CHTOG);

    HWREG16(baseAddress + OFS_SAPH_AASCTL1) |= config->sideOfChannel | config->standByIndication
        | config->endOfSequence | config->earlyReceiveBias | config->enableChannelToggle;
    HWREG16(baseAddress + OFS_SAPH_AASCTL0) |= config->abortOnError | config->triggerSource
        | config->channelSelect | ASQTEN;
}

void SAPH_A_triggerASQ(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPH_AASQTRIG) |= ASQTRIG;
}

void SAPH_A_stopASQ(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPH_AASCTL0) |= ASQSTOP;
}

uint8_t SAPH_A_getASQPingCounter(uint16_t baseAddress)
{
    return HWREG8(baseAddress + OFS_SAPH_AASCTL0) & PNGCNT;
}

void SAPH_A_configureASQPing(uint16_t baseAddress, SAPH_A_configASQPingParam *config)
{
    HWREG16(baseAddress + OFS_SAPH_AAPOL) &= ~PCPOL;
    HWREG16(baseAddress + OFS_SAPH_AAPOL) |= config->polarity;

    HWREG16(baseAddress + OFS_SAPH_AAPLEV) &= ~PCPLEV;
    HWREG16(baseAddress + OFS_SAPH_AAPLEV) |= config->pauseLevel;

    HWREG16(baseAddress + OFS_SAPH_AAPHIZ) &= ~PCPHIZ;
    HWREG16(baseAddress + OFS_SAPH_AAPHIZ) |= config->pauseHighImpedance;
}

void SAPH_A_configureASQTimeBase(uint16_t baseAddress, uint16_t prescalerStartValue)
{
    HWREG16(baseAddress + OFS_SAPH_ATBCTL) &= ~(PSSV);
    HWREG16(baseAddress + OFS_SAPH_ATBCTL) |= prescalerStartValue;
}

void SAPH_A_startASQTimer(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPH_ATBCTL) |= TSTART;
}

void SAPH_A_stopASQTimer(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPH_ATBCTL) |= TSTOP;
}

void SAPH_A_clearASQTimer(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPH_ATBCTL) |= TCLR;
}

void SAPH_A_configureMode(uint16_t baseAddress, SAPH_A_configModeParam *config)
{
    HWREG16(baseAddress + OFS_SAPH_AMCNF) &= ~LPBE;
    HWREG16(baseAddress + OFS_SAPH_AMCNF) |= config->lowPowerBiasMode;

    HWREG16(baseAddress + OFS_SAPH_AMCNF) &= ~CPEO;
    HWREG16(baseAddress + OFS_SAPH_AMCNF) |= config->chargePump;

    HWREG16(baseAddress + OFS_SAPH_AMCNF) &= ~BIMP;
    HWREG16(baseAddress + OFS_SAPH_AMCNF) |= config->biasImpedance;
}

void SAPH_A_enableLowPowerBiasMode(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPH_AMCNF) |= LPBE;
}

void SAPH_A_disableLowPowerBiasMode(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPH_AMCNF) &= ~LPBE;
}

void SAPH_A_enableChargePumpAlways(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPH_AMCNF) |= CPEO;
}

void SAPH_A_enableChargePumpOnAcquisitionOnly(uint16_t baseAddress)
{
    HWREG16(baseAddress + OFS_SAPH_AMCNF) &= ~CPEO;
}

void SAPH_A_setBiasGeneratorImpedance(uint16_t baseAddress, uint16_t biasImpedance)
{
    HWREG16(baseAddress + OFS_SAPH_AMCNF) &= ~BIMP;
    HWREG16(baseAddress + OFS_SAPH_AMCNF) |= biasImpedance;
}

void SAPH_A_configureExtendedPPG(uint16_t baseAddress, SAPH_A_configXPGParam *config)
{
    HWREG16(baseAddress + OFS_SAPH_AXPGCTL) &= ~(ETY | XMOD | XPULS);
    HWREG16(baseAddress + OFS_SAPH_AXPGCTL) |= (config->eventType | config->extendedMode | (config->extraPulses & XPULS));
}
void SAPH_A_setXPGLowPeriod(uint16_t baseAddress, uint16_t period)
{
    HWREG16(baseAddress + OFS_SAPH_AXPGLPER) = (period & XLPER);
}
void SAPH_A_setXPGHighPeriod(uint16_t baseAddress, uint16_t period)
{
    HWREG16(baseAddress + OFS_SAPH_AXPGHPER) = (period & XHPER);
}

uint16_t SAPH_A_getPPGPhaseStatus(uint16_t baseAddress)
{
    return (HWREG16(baseAddress + OFS_SAPH_AXPGCTL) & XSTAT);
}

#endif
//*****************************************************************************
//
//! Close the doxygen group for saph_a_api
//! @}
//
//*****************************************************************************

