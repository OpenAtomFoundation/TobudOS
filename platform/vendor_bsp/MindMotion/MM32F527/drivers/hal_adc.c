/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hal_adc.h"

void ADC_Init(ADC_Type * ADCx, ADC_Init_Type * init)
{

    if (!init)
    {
        return;
    }

    ADCx->ADCFG = (ADCx->ADCFG & ~ADC_ADCFG_RSLTCTL_MASK)| ADC_ADCFG_RSLTCTL(init->Resolution);
    /* ADC conversion mode and conversion data result align. */
    ADCx->ADCR = (ADCx->ADCR & ~( ADC_ADCR_ADMD_MASK | ADC_ADCR_ALIGN_MASK) )
               | ADC_ADCR_ADMD(init->ConvMode)
               | ADC_ADCR_ALIGN(init->Align)
               ;
    /* Set the single-end or diff conversion mode. */
    ADCx->ADCFG2  = ADCx->ADCFG2 & ~(ADC_ADCFG2_ADCSREF_MASK | ADC_ADCFG2_DC_MASK | ADC_ADCFG2_PSDC_MASK);
    if (ADC_SingleDiffConvMode_SingleEnd == init->SingleDiffMode)
    {
        ADCx->ADCFG2 |= ADC_ADCFG2_ADCSREF(init->SingleVolt);
    }
    else if (ADC_SingleDiffConvMode_Diff == init->SingleDiffMode)
    {
        ADCx->ADCFG2 |= ADC_ADCFG2_DC(init->DiffPair);
    }
    else if (ADC_SingleDiffConvMode_PseudoDiff == init->SingleDiffMode)
    {
        ADCx->ADCFG2 = ADCx->ADCFG2 | ( ADC_ADCFG2_DC(init->DiffPair) | ADC_ADCFG2_PSDC(init->DiffPair) );
    }
}

void ADC_Enable(ADC_Type * ADCx, bool enable)
{
    if (enable)
    {
        ADCx->ADCFG |= ADC_ADCFG_ADEN_MASK;
    }
    else
    {
        ADCx->ADCFG &= ~ADC_ADCFG_ADEN_MASK;
    }
}

void  ADC_EnableTempSensor(ADC_Type * ADCx, bool enable)
{
    if (enable)
    {
        ADCx->ADCFG |= ADC_ADCFG_TSEN_MASK;
    }
    else
    {
        ADCx->ADCFG &= ~ADC_ADCFG_TSEN_MASK;
    }
}

/* Use VBG 1.2V as default voltage sensor. */
void  ADC_EnableVoltSensor(ADC_Type * ADCx, bool enable)
{
    if (enable)
    {
        ADCx ->ADCFG |= ADC_ADCFG_VSEN_MASK;
    }
    else
    {
        ADCx->ADCFG &= ~ADC_ADCFG_VSEN_MASK;
    }
}

void ADC_EnableDMA(ADC_Type * ADCx, bool enable)
{
    if (enable)
    {
        ADCx->ADCR |= ADC_ADCR_DMAEN_MASK;
    }
    else
    {
        ADCx->ADCR &= ~ADC_ADCR_DMAEN_MASK;
    }
}

void ADC_EnableInterrupts(ADC_Type * ADCx, uint32_t interrupts, bool enable)
{
    if (enable)
    {
        if ( 0u != (ADC_INT_CONV_SLOT_DONE & interrupts) )
        {
            ADCx->ADCR |= ADC_ADCR_EOCIE_MASK;
        }
        if ( 0u != (ADC_INT_CONV_SAMPLE_DONE & interrupts) )
        {
            ADCx->ADCR |= ADC_ADCR_EOSMPIE_MASK;
        }
        if ( 0u != (ADC_INT_CONV_SEQ_DONE & interrupts) )
        {
            ADCx->ADCR |= ADC_ADCR_EOSIE_MASK;
        }
        if ( 0u != (ADC_INT_CONV_COMPARE_DONE & interrupts) )
        {
            ADCx->ADCR |= ADC_ADCR_AWDIE_MASK;
        }
        if ( 0u != (ADC_INT_CONV_CALIB_DONE & interrupts) )
        {
            ADCx->ANYCR |= ADC_ANYCR_EOCALIE_MASK;
        }
        if ( 0u != (ADC_INT_JCONV_SLOT_DONE & interrupts) )
        {
            ADCx->ANYCR |= ADC_ANYCR_JEOCIE_MASK;
        }
        if ( 0u != (ADC_INT_JCONV_SAMPLE_DONE & interrupts) )
        {
            ADCx->ANYCR |= ADC_ANYCR_JEOSMPIE_MASK;
        }
        if ( 0u != (ADC_INT_JCONV_SEQ_DONE & interrupts) )
        {
            ADCx->ANYCR |= ADC_ANYCR_JEOSIE_MASK;
        }
    }
    else
    {
       if ( 0u != (ADC_INT_CONV_SLOT_DONE & interrupts) )
        {
            ADCx->ADCR &= ~ADC_ADCR_EOCIE_MASK;
        }
        if ( 0u != (ADC_INT_CONV_SAMPLE_DONE & interrupts) )
        {
            ADCx->ADCR &= ~ADC_ADCR_EOSMPIE_MASK;
        }
        if ( 0u != (ADC_INT_CONV_SEQ_DONE & interrupts) )
        {
            ADCx->ADCR &= ~ADC_ADCR_EOSIE_MASK;
        }
        if ( 0u != (ADC_INT_CONV_COMPARE_DONE & interrupts) )
        {
            ADCx->ADCR &= ~ADC_ADCR_AWDIE_MASK;
        }
        if ( 0u != (ADC_INT_CONV_CALIB_DONE & interrupts) )
        {
            ADCx->ANYCR &= ~ ADC_ANYCR_EOCALIE_MASK;
        }
        if ( 0u != (ADC_INT_JCONV_SLOT_DONE & interrupts) )
        {
            ADCx->ANYCR &= ~ ADC_ANYCR_JEOCIE_MASK;
        }
        if ( 0u != (ADC_INT_JCONV_SAMPLE_DONE & interrupts) )
        {
            ADCx->ANYCR &= ~ ADC_ANYCR_JEOSMPIE_MASK;
        }
        if ( 0u != (ADC_INT_JCONV_SEQ_DONE & interrupts) )
        {
            ADCx->ANYCR &= ~ ADC_ANYCR_JEOSIE_MASK;
        }
    }
}

uint32_t ADC_GetStatus(ADC_Type * ADCx)
{
    uint32_t flags = 0u;

    if ( 0u != (ADC_ADSTAEXT_EOCALIF_MASK & ADCx->ADSTAEXT) )
    {
        flags |= ADC_STATUS_CONV_CALIB_DONE;
    }
    if ( 0u != (ADC_ADSTAEXT_JEOCIF_MASK & ADCx->ADSTAEXT) )
    {
        flags |= ADC_STATUS_JCONV_SLOT_DONE;
    }
    if ( 0u != (ADC_ADSTAEXT_JEOSMPIF_MASK & ADCx->ADSTAEXT) )
    {
        flags |= ADC_STATUS_JCONV_SAMPLE_DONE;
    }
    if ( 0u != (ADC_ADSTAEXT_JEOSIF_MASK & ADCx->ADSTAEXT) )
    {
        flags |= ADC_STATUS_JCONV_SEQ_DONE;
    }
    if ( 0u != (ADC_ADSTAEXT_EOCIF_MASK & ADCx->ADSTAEXT) )
    {
        flags |= ADC_STATUS_CONV_SLOT_DONE;
    }
    if ( 0u != (ADC_ADSTAEXT_EOSMPIF_MASK & ADCx->ADSTAEXT) )
    {
        flags |= ADC_STATUS_CONV_SAMPLE_DONE;
    }
    if ( 0u != (ADC_ADSTA_EOSIF_MASK & ADCx->ADSTA) )
    {
        flags |= ADC_STATUS_CONV_SEQ_DONE;
    }
    if ( 0u != (ADC_ADSTA_AWDIF_MASK & ADCx->ADSTA) )
    {
        flags |= ADC_STATUS_CONV_COMPARE_DONE;
    }

     return flags;
}

void ADC_ClearStatus(ADC_Type * ADCx, uint32_t flags)
{

    if ( 0u != (ADC_STATUS_CONV_CALIB_DONE & flags) )
    {
        ADCx->ADSTAEXT = ADC_ADSTAEXT_EOCALIF_MASK;
    }
    if ( 0u != (ADC_STATUS_JCONV_SLOT_DONE & flags) )
    {
        ADCx->ADSTAEXT = ADC_ADSTAEXT_JEOCIF_MASK;
    }
    if ( 0u != (ADC_STATUS_JCONV_SAMPLE_DONE & flags) )
    {
        ADCx->ADSTAEXT = ADC_ADSTAEXT_JEOSMPIF_MASK;
    }
    if ( 0u != ( ADC_STATUS_JCONV_SEQ_DONE & flags) )
    {
        ADCx->ADSTAEXT = ADC_ADSTAEXT_JEOSIF_MASK;
    }
    if ( 0u != (ADC_STATUS_CONV_SLOT_DONE & flags) )
    {
        ADCx->ADSTAEXT = ADC_ADSTAEXT_EOCIF_MASK;
    }
    if ( 0u != (ADC_STATUS_CONV_SAMPLE_DONE & flags) )
    {
        ADCx->ADSTAEXT = ADC_ADSTAEXT_EOSMPIF_MASK;
    }
    if ( 0u != (ADC_STATUS_CONV_SEQ_DONE & flags) )
    {
        ADCx->ADSTA = ADC_ADSTA_EOSIF_MASK;
    }
    if ( 0u != (ADC_STATUS_CONV_COMPARE_DONE & flags) )
    {
        ADCx->ADSTA = ADC_ADSTA_AWDIF_MASK;
    }
}

uint32_t ADC_GetChnConvResult(ADC_Type * ADCx, uint32_t * channel, uint32_t * flags)
{
    uint32_t tmp32 = ADCx->ADDATA;

    *channel = (tmp32 & (ADC_ADDATA_CHANNELSEL_MASK | ADC_ADDATA_CHANNELSELH_MASK) ) >> ADC_ADDATA_CHANNELSEL_SHIFT;
    if (flags)
    {
        *flags = (tmp32 & (ADC_ADDATA_OVERRUN_MASK | ADC_ADDATA_VALID_MASK) ) >> ADC_ADDATA_OVERRUN_SHIFT;
    }

    return (tmp32 & ADC_ADDATA_DATA_MASK ) >> ADC_ADDATA_DATA_SHIFT;
}

uint32_t ADC_GetSlotConvResult(ADC_Type * ADCx, uint32_t slot, uint32_t * flags)
{
    uint32_t tmp32 = ADCx ->ADDR[slot];

    if (flags)
    {
        *flags = (tmp32 & (ADC_ADDR_OVERRUN_MASK | ADC_ADDR_VALID_MASK) ) >> ADC_ADDR_OVERRUN_SHIFT;
    }

    return (tmp32 & ADC_ADDR_DATA_MASK ) >> ADC_ADDR_DATA_SHIFT;
}

uint32_t ADC_GetExtChnConvResult(ADC_Type * ADCx, uint32_t * channel, uint32_t * flags)
{
    uint32_t tmp32 = ADCx->JADDATA;

    *channel = (tmp32 & ADC_JADDATA_JCHANNELSEL_MASK) >> ADC_JADDATA_JCHANNELSEL_SHIFT;
    if (flags)
    {
        *flags = (tmp32 & (ADC_JADDATA_JOVERRUN_MASK | ADC_JADDATA_JVALID_MASK) ) >> ADC_JADDATA_JOVERRUN_SHIFT;
    }

    return (tmp32 & ADC_JADDATA_JDATA_MASK ) >> ADC_JADDATA_JDATA_SHIFT;
}

uint32_t ADC_GetExtSlotConvResult(ADC_Type * ADCx, uint32_t slot, uint32_t * flags)
{
    uint32_t tmp32 = ADCx ->JDR[slot];

    if (flags)
    {
        *flags = (tmp32 & (ADC_JDR_JOVERRUN_MASK | ADC_JDR_JVALID_MASK) ) >> ADC_JDR_JOVERRUN_SHIFT;
    }

    return (tmp32 & ADC_JDR_JDATA_MASK ) >> ADC_JDR_JDATA_SHIFT;
}

void  ADC_SetChnSampleTime(ADC_Type * ADCx, uint32_t channel, ADC_SampleTime_Type sample_time)
{
    if (channel < 8u)
    {
        ADCx->SMPR1 = (ADCx->SMPR1 & ~(0xF << (4u * channel))) | (sample_time << (4u * channel));
    }
    else if (channel-8 < 8u)
    {
        channel-= 8u;
        ADCx->SMPR2 = (ADCx->SMPR2 & ~(0xF << (4u * channel))) | (sample_time << (4u * channel));
    }
    else
    {
        channel-= 16u;
        ADCx->SMPR3 = (ADCx->SMPR3 & ~(0xF << (4u * channel))) | (sample_time << (4u * channel));
    }
}

void ADC_EnableHwTrigger(ADC_Type * ADCx, ADC_HwTriggerConf_Type * conf)
{
    if ( !conf )
    {
        ADCx->ADCR &= ~ADC_ADCR_TRGEN_MASK;
        return;
    }
    /* Enable the hardware trigger. */
    ADCx->ADCR = ( ADCx->ADCR & ~( ADC_ADCR_TRGSHIFT_MASK | ADC_ADCR_TRGEDGE_MASK) )
               | ADC_ADCR_TRGEN_MASK
               | ADC_ADCR_TRGSHIFT(conf->DelayCycle)
               | ADC_ADCR_TRGEDGE(conf->Edge)
               ;
}

void ADC_EnableSeqSlotFixed(ADC_Type * ADCx, uint32_t seq_slot, ADC_SeqFixedDirection_Type dir)
{
    ADCx->ANYCR &= ~ADC_ANYCR_CHANYMDEN_MASK;
    /* enable fixed channel. */
    ADCx->ADCHS = seq_slot;
    /* configure the scan direction. */
    ADCx->ADCR = (ADCx->ADCR & ~ADC_ADCR_SCANDIR_MASK)
               | ADC_ADCR_SCANDIR(dir)
               ;
}

void ADC_EnableSeqSlot(ADC_Type * ADCx, uint32_t slot_idx, uint32_t channel)
{

    if (slot_idx > 16u)
    {
        return; /* the available range of seq length is within 16u. */
    }

    /* enable any channel sequence mode. */
    ADCx->ANYCR |= ADC_ANYCR_CHANYMDEN_MASK;
    /* select the any slots number. */
    ADCx->ANYCFG = ADC_ANYCFG_CHANYNUM(slot_idx);
    uint32_t offset = slot_idx;
    /* fill the channel into each slot. */
    if (slot_idx < 8u)
    {
        ADCx->CHANY0 = (ADCx->CHANY0 & ~( 0xFF << (offset * 4u) )) | ( (channel & 0xF) << (offset * 4u) );
    }
    else
    {
        ADCx->CHANY1 = (ADCx->CHANY1 & ~( 0xFF << ((offset - 8u) * 4u) )) | ( (channel & 0xF) << ((offset - 8u) * 4u) );
    }
    ADCx->ANYCFG = (ADCx->ANYCFG & ~( 0x01 << (offset + 8u) )) | ( (channel & 0x10) << (offset + 8u) );

}

void ADC_EnableExtSeqSlot(ADC_Type * ADCx, uint32_t slot_idx, uint32_t channel)
{
    if (slot_idx > 4u)
    {
        return; /* the available range of seq length is within 4u. */
    }
    /* enable the inject channel sequence mode. */
    ADCx->ANYCR |= ADC_ANYCR_JCEN_MASK;
    /* select the inject slot number. */
    ADCx->JSQR = ADC_JSQR_JNUM(slot_idx);
    /* fill the channels into each slot of inject sequence. */
    uint32_t offset = slot_idx;
    ADCx->JSQR = (ADCx->JSQR &~(0x1F << (5u * offset)))| (channel & 0x1F) << (5u * offset);
}

void ADC_EnableAutoExtSeqSlot(ADC_Type * ADCx, bool enable)
{
    if (enable)
    {
        ADCx->ANYCR |= ADC_ANYCR_JAUTO_MASK;
    }
    else
    {
        ADCx->ANYCR &= ~ADC_ANYCR_JAUTO_MASK;
    }
}

void ADC_DoExtSeqSlotSwTrigger(ADC_Type * ADCx, bool enable)
{
    if (enable)
    {
        ADCx->ANYCR |= ADC_ANYCR_JADST_MASK;
    }
    else
    {
        ADCx->ANYCR &= ~ADC_ANYCR_JADST_MASK;
    }
}

void ADC_SetExtSlotCalibParam(ADC_Type * ADCx, uint32_t slot, uint32_t value)
{
    ADCx->JOFR[slot] = ADC_JOFR_JOFFSET(value);
}

void ADC_DoSwTrigger(ADC_Type * ADCx, bool enable)
{
    if (enable)
    {
        ADCx->ADCR |= ADC_ADCR_ADST_MASK;
    }
    else
    {
        ADCx->ADCR &= ~ADC_ADCR_ADST_MASK;
    }
}

void ADC_EnableHwComp(ADC_Type * ADCx, ADC_HwCompConf_Type * conf)
{
    if ( !conf )
    {
        /* disable the hardware compare feature for both regular & any seq. */
        ADCx->ADCFG &= ~ADC_ADCFG_AWDEN_MASK;
        return;
    }

    /* enable the hardware compare feature. */
    ADCx->ADCFG |= ADC_ADCFG_AWDEN_MASK;

    /* setup the compare channel. */

    ADCx->ADCR = (ADCx->ADCR & ~(ADC_ADCR_CMPCHL_MASK | ADC_ADCR_CMPCHH_MASK))
                                 | ADC_ADCR_CMPCHL(0xf & conf->ChnNum)
                                 | ADC_ADCR_CMPCHH(conf->ChnNum >> 4)
                                 ;
    /* setup the compare boundary. */
    ADCx->ADCMPR = ADC_ADCMPR_CMPLDATA(conf->LowLimit)
                 | ADC_ADCMPR_CMPHDATA(conf->HighLimit)
                 ;
}

void ADC_DoAutoCalib(ADC_Type * ADCx)
{
    ADCx->ANYCR |= ADC_ANYCR_ADCAL_MASK;
    while(0u == (ADCx->ADSTAEXT & ADC_ADSTAEXT_EOCALIF_MASK));
    /* clear the status. */
    ADCx->ADSTAEXT = ADC_ADSTAEXT_EOCALIF_MASK;
}

uint32_t ADC_GetCalibFactor(ADC_Type * ADCx, uint32_t * factor)
{
	for (uint32_t i = 0; i < 17; i++)
    {
        ADCx->ADDATA = ADC_ADDATA_DATA((i + 1) << 9);
        factor[i]= ADCx->ADDATA & 0x1FF;
        while ( 0u == (ADCx->ADSTAEXT & ADC_ADSTAEXT_FREOCIF_MASK))
        {}
        ADCx->ADSTAEXT |= ADC_ADSTAEXT_FREOCIF_MASK;  
    }
    return 1;
}

void ADC_SetCalibFactor(ADC_Type * ADCx, uint32_t * factor)
{
    for (uint32_t i = 0; i < 17; i++)
    {
        ADCx->ADDATA = factor[i] | ( (i + 1) << 9);
        while ( 0u == (ADCx->ADSTAEXT & ADC_ADSTAEXT_FREOCIF_MASK))
        {}
        ADCx->ADSTAEXT |= ADC_ADSTAEXT_FREOCIF_MASK;
    }
}

void ADC_SetOverSample(ADC_Type * ADCx, ADC_OverSampleConf_Type *conf)
{
    if (!conf)
    {
        return;
    }
    ADCx->ADCFG2 = ( ADCx->ADCFG2 & ~ (ADC_ADCFG2_ROVSE_MASK |ADC_ADCFG2_JOVSE_MASK | ADC_ADCFG2_OVSR_MASK | ADC_ADCFG2_OVSS_MASK | ADC_ADCFG2_TROVS_MASK) )
                    | ADC_ADCFG2_OVSR(conf->Ratio)
                    | ADC_ADCFG2_OVSS(conf->Div)
                    | ADC_ADCFG2_TROVS(conf->Mode)
                    | ADC_ADCFG2_JOVSE(conf->injectOverSampleMode)
                    ;
}

void ADC_ResetAllSlot(ADC_Type * ADCx)
{
    /* wait for the latest conversion done. */
    while( (0u != (ADCx->ADSTA & ADC_ADSTA_BUSY_MASK)) | (0u != (ADCx->ADSTAEXT & ADC_ADSTAEXT_JBUSY_MASK)) )
    {}
    /* disable the regular channel. */
    ADCx->ADCHS = 0x00;
    /* disable the any sequence. */
    ADCx->CHANY0 = 0x00;
    ADCx->CHANY1 = 0x00;
    /* reset the inject slot . */
    ADCx->JSQR = 0x00;
    /* disable inject conversion. */
    ADCx->ANYCR &= ~ADC_ANYCR_JCEN_MASK;
    /* disable sequence conversion. */
    ADCx->ANYCR &= ~ADC_ANYCR_CHANYMDEN_MASK;
    /* stop adc conversion. */
    ADCx->ADCR |= ADC_ADCR_ADST_MASK;
}
/* EOF. */
