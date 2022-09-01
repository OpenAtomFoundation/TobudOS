/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hal_lptim.h"

void LPTIM_Init(LPTIM_Type * LPTIMx, LPTIM_Init_Type * init)
{
    LPTIMx->CFG = (LPTIMx->CFG & ~( LPTIM_CFG_TMODE_MASK | LPTIM_CFG_DIVSEL_MASK | LPTIM_CFG_MODE_MASK) )
                | LPTIM_CFG_TMODE(init->WorkMode) /* Setup the work mode. */
                | LPTIM_CFG_DIVSEL(init->Prescaler) /* Setup the prescaler. */
                | LPTIM_CFG_MODE(init->PeriodMode); /* Setup the period mode. */

    /* Setup the period value. */
    LPTIMx->TARGET = LPTIM_TARGET_TARGETREG(init->Period);
}

void LPTIM_Start(LPTIM_Type * LPTIMx)
{
    LPTIMx->CTRL |= LPTIM_CTRL_LPTEN_MASK; /* Start the counter. */
}

void LPTIM_Stop(LPTIM_Type * LPTIMx)
{
    LPTIMx->CTRL &= ~LPTIM_CTRL_LPTEN_MASK; /* Stop the counter. */
}

void LPTIM_EnableInterrupts(LPTIM_Type * LPTIMx, uint32_t interrupts, bool enable)
{
    if (enable)
    {
        LPTIMx->IE |= interrupts;
    }
    else
    {
        LPTIMx->IE &= ~interrupts;
    }
}

uint32_t LPTIM_GetInterruptStatus(LPTIM_Type * LPTIMx)
{
    return LPTIMx->IF; /* return flags */
}

void LPTIM_ClearInterruptStatus(LPTIM_Type * LPTIMx, uint32_t status)
{
    LPTIMx->IF = status;
}

void LPTIM_EnableTriggerIn(LPTIM_Type * LPTIMx, LPTIM_TriggerInConf_Type * conf)
{
    LPTIMx->CFG = (LPTIMx->CFG & ~(LPTIM_CFG_TRIGSEL_MASK | LPTIM_CFG_FLTEN_MASK | LPTIM_CFG_TRIGCFG_MASK) )
                | LPTIM_CFG_TRIGSEL(conf->Channel) /* Setup trigger input channel. */
                | LPTIM_CFG_FLTEN(conf->EnableFilter) /* Setup the external trigger input filter. */
                | LPTIM_CFG_TRIGCFG(conf->TriggerInPolarity); /* Setup the trigger input polarity. */
}

void LPTIM_EnableOutWave(LPTIM_Type * LPTIMx, LPTIM_OutWaveConf_Type * conf)
{
    LPTIMx->CFG = (LPTIMx->CFG & ~ (LPTIM_CFG_PWM_MASK | LPTIM_CFG_POLARITY_MASK) )
                | LPTIM_CFG_PWM(conf->Wave) /* Setup the output waveform. */
                | LPTIM_CFG_POLARITY(conf->OutPolarity); /* Setup the output waveform polarity. */
}

void LPTIM_SetCompareValue(LPTIM_Type * LPTIMx, uint32_t value)
{
    LPTIMx->CMP = LPTIM_CMP_COMPAREREG(value);
}

uint32_t LPTIM_GetCounterValue(LPTIM_Type * LPTIMx)
{
    return LPTIMx->CNT; /* return current counter value. */
}

/* EOF. */
