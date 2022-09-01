/*
 * Copyright 2021 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hal_comp.h"

static __IO uint32_t * COMP_GetCSR(COMP_Type * COMPx, uint32_t channel)
{
    __IO uint32_t * csr[] =
    {
        &COMPx->CSR[0],
        &COMPx->CSR[1],
        &COMPx->CSR3,
    };
    return csr[channel];
}

/* init comp. */
void COMP_Init(COMP_Type * COMPx, uint32_t channel, COMP_Init_Type * init)
{
    if ( channel < COMP_CHANNEL_NUM )
    {
        __IO uint32_t * csr = COMP_GetCSR(COMPx, channel);
        *csr = (*csr & ~(COMP_CSR_HYST_MASK | COMP_CSR_OUTSEL_MASK | COMP_CSR_INPSEL_MASK | COMP_CSR_INMSEL_MASK | COMP_CSR_MODE_MASK) )
             | COMP_CSR_HYST(init->Hysteresis)
             | COMP_CSR_OUTSEL(init->OutMux)
             | COMP_CSR_INPSEL(init->PosInMux)
             | COMP_CSR_INMSEL(init->InvInMux)
             | COMP_CSR_MODE(init->Speed)
             ;
    }
}

/* enable comp. */
void COMP_Enable(COMP_Type * COMPx, uint32_t channel, bool enable)
{
    if ( channel < COMP_CHANNEL_NUM )
    {
        __IO uint32_t * csr = COMP_GetCSR(COMPx, channel);
        if ( true == enable )
        {
            *csr |=  COMP_CSR_EN_MASK;
        }
        else
        {
            *csr &= ~COMP_CSR_EN_MASK;
        }
    }
}

/* lock. */
void COMP_Lock(COMP_Type * COMPx, uint32_t channel)
{
    if ( channel < COMP_CHANNEL_NUM )
    {
        __IO uint32_t * csr = COMP_GetCSR(COMPx, channel);
        *csr |= COMP_CSR_LOCK_MASK;
    }
}

/* get output status. */
bool COMP_GetOutputStatus(COMP_Type * COMPx, uint32_t channel)
{
    if ( channel < COMP_CHANNEL_NUM )
    {
        __IO uint32_t * csr = COMP_GetCSR(COMPx, channel);
        if ( 0u != ( COMP_CSR_OUT_MASK & (*csr) ) )
        {
            return true;
        }
        else
        {
            return false; /* normal input voltage lower than inverting input. */
        }
    }
    else
    {
        return false;
    }
}

/* enable external vref. */
void COMP_EnableExtVrefConf(COMP_Type * COMPx, COMP_ExtVrefConf_Type * conf)
{
    if ( NULL == conf) /* disable the ext vref. */
    {
        COMPx->CRV &= ~COMP_CRV_CRVEN_MASK;
    }
    else /* init & enable ext vref. */
    {
        COMPx->CRV = (COMPx->CRV & ~(COMP_CRV_CRVSRC_MASK | COMP_CRV_CRVSEL_MASK | COMP_CRV_CRVEN_MASK) )
                   | COMP_CRV_CRVSRC(conf->VrefSource)
                   | COMP_CRV_CRVSEL(conf->Volt)
                   | COMP_CRV_CRVEN_MASK
                   ;
    }
}

/* enable round robin (poll compare). */
void COMP_EnableRoundRobinConf(COMP_Type * COMPx, uint32_t channel, COMP_RoundRobinConf_Type * conf)
{
    if ( channel < COMP_CHANNEL_NUM )
    {
        if ( NULL == conf )
        {
            COMPx->POLL[channel] &= ~COMP_POLL_POLLEN_MASK;
        }
        else
        {
            COMPx->POLL[channel] = (COMPx->POLL[channel] & ~(COMP_POLL_PERIOD_MASK | COMP_POLL_FIXN_MASK | COMP_POLL_POLLCH_MASK | COMP_POLL_POLLEN_MASK) )
                                 | COMP_POLL_PERIOD(conf->Period)
                                 | COMP_POLL_FIXN(conf->EnableInvInFix)
                                 | COMP_POLL_POLLCH(conf->ChnGroup)
                                 | COMP_POLL_POLLEN_MASK;
                                 ;
        }
    }
}

/* get round robin (poll compare) status. */
bool COMP_GetRoundRobinOutStatus(COMP_Type * COMPx, uint32_t channel, COMP_InMux_Type pos_in)
{
    if ( channel < COMP_CHANNEL_NUM )
    {
        uint32_t mask = COMP_POLL_POUT(1u << ( (uint32_t)pos_in - 1u ) );
        if ( 0u != (COMPx->POLL[channel] & mask) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

/* set output filter. */
void COMP_SetOutFilter(COMP_Type * COMPx, uint32_t channel, COMP_OutFilter_Type filter)
{
    if (channel < COMP_CHANNEL_NUM)
    {
        __IO uint32_t * csr = COMP_GetCSR(COMPx, channel);
        *csr = (*csr & ~COMP_CSR_OFLT_MASK) | COMP_CSR_OFLT(filter);
    }
}

/* enable output invert. */
void COMP_EnableOutInvert(COMP_Type * COMPx, uint32_t channel, bool invert)
{
    if (channel < COMP_CHANNEL_NUM)
    {
        __IO uint32_t * csr = COMP_GetCSR(COMPx, channel);
        if(true == invert)
        {
            *csr = (*csr & ~COMP_CSR_POL_MASK) | COMP_CSR_POL(1u);
        }
        else
        {
            *csr = (*csr & ~COMP_CSR_POL_MASK) | COMP_CSR_POL(0u);
        }
    }
}

/* enable bypass output. */
void COMP_EnableBypassOutput(COMP_Type * COMPx, uint32_t channel, bool enable)
{
    if (channel < COMP_CHANNEL_NUM)
    {
        __IO uint32_t * csr = COMP_GetCSR(COMPx, channel);
        if (enable)
        {
            *csr |=  COMP_CSR_OUTANASEL_MASK;
        }
        else
        {
            *csr &= ~COMP_CSR_OUTANASEL_MASK;
        }
    }
}

void COMP_EnableWindow(COMP_Type * COMPx, uint32_t channel, bool enable)
{
    if (channel < COMP_CHANNEL_NUM)
    {
        __IO uint32_t * csr = COMP_GetCSR(COMPx, channel);
        if (enable)
        {
            *csr |=  COMP_CSR_WE_MASK;
        }
        else
        {
            *csr &= ~COMP_CSR_WE_MASK;
        }
    }
}

/* EOF. */
