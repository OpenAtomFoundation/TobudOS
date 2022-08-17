/*
 * Copyright 2021 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hal_tim.h"

bool TIM_Init(TIM_Type * TIMx, TIM_Init_Type * init)
{
    uint32_t cr1 = TIMx->CR1 &~ ( TIM_CR1_OPM_MASK
                                | TIM_CR1_ARPE_MASK
                                | TIM_CR1_CMS_MASK
                                | TIM_CR1_DIR_MASK
                                );
    cr1 |= TIM_CR1_OPM(init->PeriodMode);
    cr1 |= ((init->EnablePreloadPeriod) ? TIM_CR1_ARPE_MASK: 0u);
    TIMx->CR1 = cr1;

    /* Check the vadility of StepFreqHz. */
    if ( (init->StepFreqHz == 0u) || (init->StepFreqHz > init->ClockFreqHz) )
    {
        return false;
    }

    switch (init->CountMode)
    {
        case TIM_CountMode_Increasing:
            break;
        case TIM_CountMode_Decreasing:
            cr1 |= TIM_CR1_DIR_MASK;
            break;
        case TIM_CountMode_CenterAligned1:
            cr1 |= TIM_CR1_CMS(1u);
            break;
        case TIM_CountMode_CenterAligned2:
            cr1 |= TIM_CR1_CMS(2u);
            break;
        case TIM_CountMode_CenterAligned3:
            cr1 |= TIM_CR1_CMS(3u);
            break;
        default:
            break;
    }

    /* Calculate the prescaler. */
    TIMx->PSC = init->ClockFreqHz / init->StepFreqHz - 1u;
    TIMx->ARR = init->Period;
    return true;
}

void TIM_Start(TIM_Type * TIMx)
{
    TIMx->CR1 |= TIM_CR1_CEN_MASK;
}

void TIM_Stop(TIM_Type * TIMx)
{
    TIMx->CR1 &= ~TIM_CR1_CEN_MASK;
}

uint32_t TIM_GetCounterValue(TIM_Type * TIMx)
{
    return TIMx->CNT;
}

void TIM_ClearCounterValue(TIM_Type * TIMx)
{
    TIMx->CNT = 0u;
}

void TIM_EnableInterrupts(TIM_Type * TIMx, uint32_t interrupts, bool enable)
{
    if (enable)
    {
        TIMx->DIER |= interrupts;
    }
    else
    {
        TIMx->DIER &= ~interrupts;
    }
}

void TIM_EnableDMA(TIM_Type * TIMx, uint32_t dmas, bool enable)
{
    if (enable)
    {
        TIMx->DIER |= dmas;
    }
    else
    {
        TIMx->DIER &= ~dmas;
    }
}

void TIM_DoSwTrigger(TIM_Type * TIMx, uint32_t swtrgs)
{
    TIMx->EGR = swtrgs;
}

uint32_t TIM_GetInterruptStatus(TIM_Type * TIMx)
{
    return TIMx->SR;
}

void TIM_ClearInterruptStatus(TIM_Type * TIMx, uint32_t status)
{
    TIMx->SR &= ~status;
}

/*******************************/
static void _TIM_WriteChannelCtrlReg(TIM_Type * TIMx, uint32_t channel, uint32_t regval)
{
    switch (channel)
    {
    case TIM_CHN_1:
        TIMx->CCMR1 = (TIMx->CCMR1 & ~(0xFF)) | (regval & 0xFF);
        break;
    case TIM_CHN_2:
        TIMx->CCMR1 = (TIMx->CCMR1 & ~(0xFF00)) | ((regval & 0xFF) << 8u);
        break;
    case TIM_CHN_3:
        TIMx->CCMR2 = (TIMx->CCMR2 & ~(0xFF)) | (regval & 0xFF);
        break;
    case TIM_CHN_4:
        TIMx->CCMR2 = (TIMx->CCMR2 & ~(0xFF00)) | ((regval & 0xFF) << 8u);
        break;
    case TIM_CHN_5:
        TIMx->CCMR3 = (TIMx->CCMR2 & ~(0xFF)) | (regval & 0xFF);
        break;
    default:
        break;
    }
}

void TIM_EnableOutputCompare(TIM_Type * TIMx, uint32_t channel, TIM_OutputCompareConf_Type * conf)
{
    uint32_t regval = TIM_CCMR1_CC1S(TIM_ChannelIOMode_Out) /* output compare mode. */
            | ( (conf->EnableFastOutput) ? TIM_CCMR1_OC1FE_MASK : 0u ) /* fast output. */
            | ( (conf->EnablePreLoadChannelValue) ? TIM_CCMR1_OC1PE_MASK : 0u) /* preload of channel value. */
            | TIM_CCMR1_OC1M(conf->RefOutMode) /* output compare comparison mode. */
            | ( (conf->ClearRefOutOnExtTrigger) ? TIM_CCMR1_OC1CE_MASK : 0u) /* external trigger clear ref. */
            ;

    _TIM_WriteChannelCtrlReg(TIMx, channel, regval);

    TIM_PutChannelValue(TIMx, channel, conf->ChannelValue);

    switch (conf->PinPolarity)
    {
        case TIM_PinPolarity_Disabled:
            TIMx->CCER &= ~( ( TIM_CCER_CC1E_MASK
                             | TIM_CCER_CC1NE_MASK
                             ) << (channel<<2u) ); /* Disable both channel. */
            break;

        case TIM_PinPolarity_Rising:
            TIMx->CCER = ( ( TIMx->CCER & ~(0xF << (channel<<2u)) )
                       | ( ( TIM_CCER_CC1E_MASK ) /* Enable the pin output / input. */
                       ) << (channel<<2u));
            break;

        case TIM_PinPolarity_Falling:
            TIMx->CCER = ( TIMx->CCER & ~(0xF << (channel<<2u)) )
                       | (( TIM_CCER_CC1E_MASK /* Enable the pin output / input. */
                       |    TIM_CCER_CC1P_MASK /* Set output active polarity. */
                       ) << (channel<<2u));
            break;

        default:
            break;
    }

}

void TIM_EnableCompOutput(TIM_Type * TIMx, uint32_t channel, bool enable)
{
    if (enable)
    {
        switch (channel)
        {
            case TIM_CHN_1:
                TIMx->CCER |= TIM_CCER_CC1NE_MASK;
                break;
            case TIM_CHN_2:
                TIMx->CCER |= TIM_CCER_CC2NE_MASK;
                break;
            case TIM_CHN_3:
                TIMx->CCER |= TIM_CCER_CC3NE_MASK;
                break;
            case TIM_CHN_4:
                TIMx->CCER |= TIM_CCER_CC4NE_MASK;
                break;
            default:
                break;
        }
    }
    else
    {
        switch (channel)
        {
            case TIM_CHN_1:
                TIMx->CCER &= ~ TIM_CCER_CC1NE_MASK;
                break;
            case TIM_CHN_2:
                TIMx->CCER &= ~ TIM_CCER_CC2NE_MASK;
                break;
            case TIM_CHN_3:
                TIMx->CCER &= ~ TIM_CCER_CC3NE_MASK;
                break;
            case TIM_CHN_4:
                TIMx->CCER &= ~ TIM_CCER_CC4NE_MASK;
                break;
            default:
                break;
        }
    }
}

/* for some TIM instance, there is an additional switch to let the output signal go.
 * in this case, the output is disabled by default. then, only the switch is enabled, the output signal can go.
 */
void TIM_EnableOutputCompareSwitch(TIM_Type * TIMx, bool enable)
{
    if (enable)
    {
        TIMx->BDTR |= TIM_BDTR_MOE_MASK;
    }
    else
    {
        TIMx->BDTR &= ~ TIM_BDTR_MOE_MASK;
    }
}

void TIM_EnableInputCapture(TIM_Type * TIMx, uint32_t channel, TIM_InputCaptureConf_Type * conf)
{
    uint32_t regval = TIM_CCMR1_CC1S(TIM_ChannelIOMode_In) /* input capture mode. */
                    | TIM_CCMR1_IC1PSC(conf->InDiv)
                    | TIM_CCMR1_IC1F(conf->InFilter)
                    ;

    _TIM_WriteChannelCtrlReg(TIMx, channel, regval);

    switch (conf->PinPolarity)
    {
        case TIM_PinPolarity_Disabled:
            TIMx->CCER &= ~(TIM_CCER_CC1E_MASK << (channel<<2u));
            break;
        case TIM_PinPolarity_Rising:
            TIMx->CCER = (TIMx->CCER & ~(0xF << (channel<<2u)) )
                       | (( TIM_CCER_CC1E_MASK /* Enable the pin output / input */
                       ) << (channel<<2u) );
            break;
        case TIM_PinPolarity_Falling:
            TIMx->CCER = ( TIMx->CCER & ~(0xF << (channel<<2u)) )
                       | (( TIM_CCER_CC1E_MASK /* Enable the pin output / input */
                       |    TIM_CCER_CC1P_MASK /* Set active input edge. */
                       ) << (channel<<2u) );
            break;
        case TIM_PinPolarity_RisingOrFalling:
            TIMx->CCER = ( TIMx->CCER & ~(0xF << (channel<<2u)) )
                       | (( TIM_CCER_CC1E_MASK /* Enable the pin output / input */
                       |    TIM_CCER_CC1P_MASK /* Set active input edge. */
                       |    TIM_CCER_CC1NP_MASK
                       ) << (channel<<2u) );
            break;
        default:
            break;
    }
}

uint32_t TIM_GetChannelValue(TIM_Type * TIMx, uint32_t channel)
{
    return TIMx->CCR[channel];
}

void TIM_PutChannelValue(TIM_Type * TIMx, uint32_t channel, uint32_t value)
{
    if ( channel == TIM_CHN_5 )
    {
        TIMx->CCR5 = value;
    }
    else
    {
        TIMx->CCR[channel] = value;
    }
}

void TIM_SetClockDiv(TIM_Type * TIMx, TIM_ClockDiv_Type div)
{
    TIMx->CR1 = ( TIMx->CR1 &~ TIM_CR1_CKD_MASK )
              | ( TIM_CR1_CKD(div) ); /* set the frequncy ratio. */
}

void TIM_EnableDeadPeriod(TIM_Type * TIMx, TIM_DeadPeriodConf_Type * conf)
{
    TIMx->BDTR = ( TIMx->BDTR &~ TIM_BDTR_DTG_MASK )
               | ( TIM_BDTR_DTG(conf->DeadPeriodCoef) ); /* set the coefficient. */
}

void TIM_EnableMasterMode(TIM_Type * TIMx, TIM_MasterModeConf_Type * conf)
{
    TIMx->CR2 = ( TIMx->CR2 &~ TIM_CR2_MMS_MASK )
              | ( TIM_CR2_MMS(conf->Out) ); /* Set master mode output. */

    uint32_t smcr = TIMx->SMCR &~ TIM_SMCR_MSM_MASK;
    if (conf->EnableSync) /* synchronize with slave timers. */
    {
        smcr |= TIM_SMCR_MSM_MASK;
    }
    TIMx->SMCR = smcr;
}

void TIM_EnableSlaveMode(TIM_Type * TIMx, TIM_SlaveModeConf_Type * conf)
{
    if ( conf->Resp != TIM_SlaveResp_Disabled )
    {
        TIMx->SMCR = ( TIMx->SMCR &~ ( TIM_SMCR_TS_MASK
                                     | TIM_SMCR_SMS_MASK
                                     ) )
                   | TIM_SMCR_TS(conf->In) /* set input trigger source. */
                   | TIM_SMCR_SMS(conf->Resp); /* set response to the source */
    }
    else
    {
        TIMx->SMCR &= ~ TIM_SMCR_SMS_MASK;
    }
}

void TIM_EnableExtTriggerIn(TIM_Type * TIMx, TIM_ExtTriggerInConf_Type * conf)
{
    uint32_t smcr = TIMx->SMCR &~ ( TIM_SMCR_ETPS_MASK
                                  | TIM_SMCR_ETF_MASK
                                  | TIM_SMCR_ECE_MASK
                                  | TIM_SMCR_ETP_MASK
                                  );
    switch (conf->PinPolarity)
    {
        case TIM_PinPolarity_Disabled:
            break;
        case TIM_PinPolarity_Rising:
            smcr |= TIM_SMCR_ECE_MASK; /* enable external trigger input. */
            break;
        case TIM_PinPolarity_Falling:
            smcr |= TIM_SMCR_ETP_MASK; /* falling edge active. */
            smcr |= TIM_SMCR_ECE_MASK; /* enable external trigger input. */
            break;
        default:
            break;
    }
    smcr |= TIM_SMCR_ETPS( conf->InDiv ); /* division to the input external trigger. */
    smcr |= TIM_SMCR_ETF( conf->InFilter ); /* set filter. */
    TIMx->SMCR = smcr;
}

uint32_t TIM_EnableDMABurst(TIM_Type * TIMx, TIM_DMABurstConf_Type * conf)
{
    TIMx->DCR = TIM_DCR_DBA(conf->BaseAddr) | TIM_DCR_DBL(conf->Length);
    return (uint32_t)(&(TIMx->DMAR));
}

TIM_EncoderDirection_Type TIM_GetEncoder(TIM_Type * TIMx, uint32_t * value)
{
    if (value)
    {
        * value = TIM_GetCounterValue(TIMx);
    }

    if ( (TIMx->CR1 & TIM_CR1_DIR_MASK) != 0u )
    {
        return TIM_EncoderDirection_Backward;
    }
    else
    {
        return TIM_EncoderDirection_Forward;
    }
}

void TIM_SetRepCounter(TIM_Type * TIMx, uint8_t value)
{
    TIMx->RCR = TIM_RCR_REP(value);
}

uint8_t TIM_GetRepCounterValue(TIM_Type * TIMx)
{
    return TIMx->RCR >> TIM_RCR_REPCNT_SHIFT;
}

void TIM_EnableIdleOut(TIM_Type * TIMx, uint32_t channel, TIM_IdleOut_Type * conf)
{
    uint32_t cr2 = TIMx->CR2 & ~ ( ( TIM_CR2_OIS1_MASK
                                   | TIM_CR2_OIS1N_MASK
                                   ) << ( channel << 1u )
                                 );
    if ( conf->PinPolarity == TIM_PinPolarity_Rising )
    {
        cr2 |= ( TIM_CR2_OIS1_MASK << ( channel << 1u ) );
    }
    if ( conf->CompPinPolarity == TIM_PinPolarity_Rising )
    {
        cr2 |= ( TIM_CR2_OIS1N_MASK << ( channel << 1u ) );
    }
    TIMx->CR2 = cr2;
}

void TIM_EnableLock(TIM_Type * TIMx, TIM_LockLevel_Type lock)
{
    TIMx->BDTR = ( TIMx->BDTR & ~ TIM_BDTR_LOCK_MASK )
                 | TIM_BDTR_LOCK(lock);
}

void TIM_EnableBreakIn(TIM_Type * TIMx, TIM_BreakIn_Type * conf)
{
    uint32_t bdtr = TIMx->BDTR &~ ( TIM_BDTR_BKE_MASK
                                  | TIM_BDTR_BKP_MASK
                                  | TIM_BDTR_AOE_MASK
                                  | TIM_BDTR_DOE_MASK
                                  );
    uint32_t bkinf = 0u;
    switch (conf->PinPolarity)
    {
        case TIM_PinPolarity_Disabled:
            break;
        case TIM_PinPolarity_Rising:
            bdtr |= ( TIM_BDTR_BKE_MASK
                    | TIM_BDTR_BKP_MASK
                    | ( conf->AutoSwitchOutput ? TIM_BDTR_AOE_MASK : 0u )
                    | ( conf->DirectIdleOutput ? TIM_BDTR_DOE_MASK : 0u )
                    );
            break;
        case TIM_PinPolarity_Falling:
            bdtr |= ( TIM_BDTR_BKE_MASK
                    | ( conf->AutoSwitchOutput ? TIM_BDTR_AOE_MASK : 0u )
                    | ( conf->DirectIdleOutput ? TIM_BDTR_DOE_MASK : 0u )
                    );
            break;
        default:
            break;
    }
    if ( conf->Filter != TIM_BreakInFilter_Disabled )
    {
        bkinf |= ( TIM_BKINF_BKINFE_MASK | TIM_BKINF_BKINF(conf->Filter) );
    }
    bkinf |= conf->Source;
    TIMx->BKINF = bkinf;
}

void TIM_EnablePWMShift(TIM_Type * TIMx, uint32_t channel, uint16_t value)
{
    if ( value == TIM_GetChannelValue(TIMx, channel) )
    {
        TIMx->PDER &= ~( TIM_PDER_CCR1SHIFTEN_MASK << channel );
    }
    else
    {
        TIMx->PDER |= ( TIM_PDER_CCR1SHIFTEN_MASK << channel );
        TIMx->CCRFALL[channel] = value;
    }
}

/* EOF. */
