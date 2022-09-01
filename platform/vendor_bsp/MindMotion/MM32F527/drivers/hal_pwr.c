/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hal_pwr.h"

void PWR_EnableLowPowerRunMode(PWR_Type * PWRx, bool enable)
{
    if (enable)
    {
        PWRx->CR1 |= PWR_CR1_LPR_MASK;
    }
    else
    {
        PWRx->CR1 &= ~PWR_CR1_LPR_MASK;
    }
}

void PWR_EnterStopMode(PWR_Type * PWRx, bool deep_stop)
{
    PWRx->CR1 &= ~PWR_CR1_PDDS_MASK;

    if (deep_stop)
    {
        PWRx->CR1 |= PWR_CR1_LPDS_MASK;
    }
    else
    {
        PWRx->CR1 &= ~PWR_CR1_LPDS_MASK;
    }
}

void PWR_EnableStandbyWakeUpPin(PWR_Type * PWRx,  PWR_StandbyWakeUpPinConf_Type * conf)
{
    if (NULL != conf)
    {
        PWRx->CR2 |= PWR_CR2_EWUP(conf->Pins);

        if (conf->TrgIn == PWR_StandbyWakeUpPinTriggerIn_FallingEdge)
        {
            PWRx->CR3 |= PWR_CR3_WP(conf->Pins);
        }
        if (conf->TrgIn == PWR_StandbyWakeUpPinTriggerIn_RisingEdge)
        {
            PWRx->CR3 &= ~PWR_CR3_WP(conf->Pins);
        }
    }
    else
    {
        PWRx->CR2 &= ~PWR_CR2_EWUP_MASK;
    }
}

void PWR_EnableStandbyWakeUpPinFilter0(PWR_Type * PWRx,  PWR_StandbyWakeUpPinFilter0Conf_Type * conf)
{
    if (NULL != conf)
    {
        if (conf->Pin == PWR_WAKEUP_PIN_0)
        {
            PWRx->CR4 |= PWR_CR4_FILTSEL0(0u);
        }
        if (conf->Pin == PWR_WAKEUP_PIN_1)
        {
            PWRx->CR4 |= PWR_CR4_FILTSEL0(1u);
        }
        if (conf->Pin == PWR_WAKEUP_PIN_2)
        {
            PWRx->CR4 |= PWR_CR4_FILTSEL0(2u);
        }
        if ( (conf->Pin == PWR_WAKEUP_PIN_0) || (conf->Pin == PWR_WAKEUP_PIN_1) || (conf->Pin == PWR_WAKEUP_PIN_2) )
        {
            PWRx->CR4 |= PWR_CR4_FILTE0(conf->TrgIn);
            PWRx->CR4 |= PWR_CR4_FILTCNT0(conf->CountValue);
            PWRx->CR4 |= PWR_CR4_FILTF0(1u);
        }
        else
        {
            PWRx->CR4 |= PWR_CR4_FILTF0(0u);
        }
    }
    else
    {
        PWRx->CR4 |= PWR_CR4_FILTF0(0u);
    }
}

void PWR_EnableStandbyWakeUpPinFilter1(PWR_Type * PWRx,  PWR_StandbyWakeUpPinFilter1Conf_Type * conf)
{
    if (NULL != conf)
    {
        if (conf->Pin == PWR_WAKEUP_PIN_3)
        {
            PWRx->CR5 |= PWR_CR5_FILTSEL1(0u);
        }
        if (conf->Pin == PWR_WAKEUP_PIN_4)
        {
            PWRx->CR5 |= PWR_CR5_FILTSEL1(1u);
        }
        if (conf->Pin == PWR_WAKEUP_PIN_5)
        {
            PWRx->CR5 |= PWR_CR5_FILTSEL1(2u);
        }
        if ( (conf->Pin == PWR_WAKEUP_PIN_3) || (conf->Pin == PWR_WAKEUP_PIN_4) || (conf->Pin == PWR_WAKEUP_PIN_5) )
        {
            PWRx->CR5 |= PWR_CR5_FILTE1(conf->TrgIn);
            PWRx->CR5 |= PWR_CR5_FILTCNT1(conf->CountValue);
            PWRx->CR5 |= PWR_CR5_FILTF1(1u);
        }
        else
        {
            PWRx->CR5 |= PWR_CR5_FILTF1(0u);
        }
    }
    else
    {
        PWRx->CR5 |= PWR_CR5_FILTF1(0u);
    }
}

void PWR_EnterStandbyMode(PWR_Type * PWRx, PWR_StandbyWakeUpDelay_Type delay)
{
    PWRx->CR1 |= PWR_CR1_PDDS_MASK;
    PWRx->SCR |= (   PWR_WAKEUP_PIN_0
                   | PWR_WAKEUP_PIN_1
                   | PWR_WAKEUP_PIN_2
                   | PWR_WAKEUP_PIN_3
                   | PWR_WAKEUP_PIN_4
                   | PWR_WAKEUP_PIN_5
                 );
    PWRx->CR6 |= PWR_CR6_STDBYFSWK(delay);
}

uint32_t PWR_GetStatus(PWR_Type * PWRx)
{
    uint32_t flags = 0u;

    if ( 0u != (PWR_CR1_LPR_MASK & PWRx->CR1) )
    {
        flags |= PWR_STATUS_LOWPOWERRUN;
    }
    if ( 0u != (PWR_CSR_SBF_MASK & PWRx->CSR) )
    {
        flags |= PWR_STATUS_STANDBYRESET;
    }
    return flags;
}

void PWR_ClearStatus(PWR_Type * PWRx, uint32_t flags)
{
    if ( 0u != (PWR_STATUS_LOWPOWERRUN & flags) )
    {
        PWRx->CR1 &= ~PWR_CR1_LPR_MASK;
    }
    if ( 0u != (PWR_STATUS_STANDBYRESET & flags) )
    {
        PWRx->CR1 |= PWR_CR1_CSBF_MASK;
    }
}

/* EOF. */
