/*!
    \file  gd32f1x0_adc.c
    \brief ADC driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#include "gd32f1x0_adc.h"

/*!
    \brief      ADC reset
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_deinit(void)
{
    rcu_periph_reset_enable(RCU_ADCRST);
    rcu_periph_reset_disable(RCU_ADCRST);
}

/*!
    \brief      enable ADC interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_enable(void)
{
    if(RESET == (ADC_CTL1 & ADC_CTL1_ADCON)){
        ADC_CTL1 |= (uint32_t)ADC_CTL1_ADCON;
    }
}

/*!
    \brief      disable ADC interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_disable(void)
{
    ADC_CTL1 &= (uint32_t)~(ADC_CTL1_ADCON);
}

/*!
    \brief      ADC calibration and reset calibration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_calibration_enable(void)
{
    /* reset the selected ADC1 calibration registers */
    ADC_CTL1 |= (uint32_t) ADC_CTL1_RSTCLB;
    /* check the RSTCLB bit state */
    while((ADC_CTL1 & ADC_CTL1_RSTCLB)){
    }
    /* enable ADC calibration process */
    ADC_CTL1 |= ADC_CTL1_CLB;
    /* check the CLB bit state */
    while((ADC_CTL1 & ADC_CTL1_CLB)){
    }
}

/*!
    \brief      enable DMA request 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_dma_mode_enable(void)
{
    ADC_CTL1 |= (uint32_t)(ADC_CTL1_DMA);
}

/*!
    \brief      disable DMA request 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_dma_mode_disable(void)
{
    ADC_CTL1 &= ~((uint32_t)ADC_CTL1_DMA);
}

/*!
    \brief      enable ADC0 temperature sensor and Vrefint channel
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_tempsensor_vrefint_enable(void)
{
    /* enable the temperature sensor and Vrefint channel */
    ADC_CTL1 |= ADC_CTL1_TSVREN;
}

/*!
    \brief      disable ADC0 temperature sensor and Vrefint channel
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_tempsensor_vrefint_disable(void)
{
    /* disable the temperature sensor and Vrefint channel */
    ADC_CTL1 &= ~ADC_CTL1_TSVREN;
}

/*!
    \brief      enable the vbat channel
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_vbat_enable(void)
{
    /* enable the vbat channel */
    ADC_CTL1 |= ADC_CTL1_VBETEN;
}

/*!
    \brief      disable the vbat channel
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_vbat_disable(void)
{
    /* disable the vbat channel */
    ADC_CTL1 &= ~ADC_CTL1_VBETEN;
}

/*!
    \brief      adc discontinuous mode config
    \param[in]  channel_group: select the channel group
      \arg        ADC_REGULAR_CHANNEL: regular channel group
      \arg        ADC_INSERTED_CHANNEL: inserted channel group
    \param[in]  length: number of conversions in discontinuous mode,the number can be 1..8
                        for regular channel ,the number is insignificant for inserted channel
    \param[out] none
    \retval     none
*/
void adc_discontinuous_mode_config(uint8_t channel_group,uint8_t length)
{
    ADC_CTL0 &= ~((uint32_t)(ADC_CTL0_DISRC | ADC_CTL0_DISIC));
    switch(channel_group){
    case ADC_REGULAR_CHANNEL:
        /* config the number of conversions in discontinuous mode  */
        ADC_CTL0 &= ~((uint32_t)ADC_CTL0_DISNUM);
        ADC_CTL0 |= (uint32_t)(((uint32_t)length - 1U) << 13U);

        ADC_CTL0 |= (uint32_t)ADC_CTL0_DISRC;
        break;
    case ADC_INSERTED_CHANNEL:
        ADC_CTL0 |= (uint32_t)ADC_CTL0_DISIC;
        break;
    default:
        break;
    }
}

/*!
    \brief      enable or disable ADC special function
    \param[in]  function: the function to config
                one or more parameters can be selected below
      \arg        ADC_SCAN_MODE: scan mode select
      \arg        ADC_INSERTED_CHANNEL_AUTO: inserted channel group convert automatically
      \arg        ADC_CONTINUOUS_MODE: continuous mode select
    \param[in]  newvalue: ENABLE or DISABLE
    \param[out] none
    \retval     none
*/
void adc_special_function_config(uint32_t function, ControlStatus newvalue)
{
    if(newvalue){
        if(RESET != (function & ADC_SCAN_MODE)){
            ADC_CTL0 |= ADC_SCAN_MODE;
        }
        if(RESET != (function & ADC_INSERTED_CHANNEL_AUTO)){
            ADC_CTL0 |= ADC_INSERTED_CHANNEL_AUTO;
        } 
        if(RESET != (function & ADC_CONTINUOUS_MODE)){
            ADC_CTL1 |= ADC_CONTINUOUS_MODE;
        }        
    }else{
        if(RESET != (function & ADC_SCAN_MODE)){
            ADC_CTL0 &= ~ADC_SCAN_MODE;
        }
        if(RESET != (function & ADC_INSERTED_CHANNEL_AUTO)){
            ADC_CTL0 &= ~ADC_INSERTED_CHANNEL_AUTO;
        } 
        if(RESET != (function & ADC_CONTINUOUS_MODE)){
            ADC_CTL1 &= ~ADC_CONTINUOUS_MODE;
        }
    }
}

/*!
    \brief      adc data alignment config
    \param[in]  data_alignment: data alignment select
                only one parameter can be selected
      \arg        ADC_DATAALIGN_RIGHT: LSB alignment
      \arg        ADC_DATAALIGN_LEFT: MSB alignment
    \param[out] none
    \retval     none
*/
void adc_data_alignment_config(uint32_t data_alignment)
{
    if(data_alignment){
        ADC_CTL1 |= ADC_CTL1_DAL;
    }else{
        ADC_CTL1 &= ~((uint32_t)ADC_CTL1_DAL);
    }
}

/*!
    \brief      config the length of regular channel group or inserted channel group
    \param[in]  channel_group: select the channel group
                only one parameter can be selected
      \arg        ADC_REGULAR_CHANNEL: regular channel group
      \arg        ADC_INSERTED_CHANNEL: inserted channel group
    \param[in]  length: the length of the channel
                        regular channel 1-17
                        inserted channel 1-4
    \param[out] none
    \retval     none
*/
void adc_channel_length_config(uint8_t channel_group,uint32_t length)
{
    switch(channel_group){
    case ADC_REGULAR_CHANNEL:
        ADC_RSQ0 &= ~(((uint32_t)length - 1U) << 20U);
        ADC_RSQ0 |= (((uint32_t)length - 1U) << 20U);
        break;
    case ADC_INSERTED_CHANNEL:
        ADC_ISQ &= ~(((uint32_t)length - 1U) << 20U);
        ADC_ISQ |= (((uint32_t)length - 1U) << 20U);
        break;
    default:
        break;
    }
}

/*!
    \brief      ADC regular channel config
    \param[in]  rank: the regular group sequencer rank,this parameter must be between 0 to 15
    \param[in]  channel: the selected ADC channel
      \arg        ADC_CHANNEL_x(x=0..18): ADC Channelx
    \param[in]  sample_time: the sample time value
      \arg        ADC_SAMPLETIME_1POINT5: 1.5 cycles
      \arg        ADC_SAMPLETIME_7POINT5: 7.5 cycles
      \arg        ADC_SAMPLETIME_13POINT5: 13.5 cycles
      \arg        ADC_SAMPLETIME_28POINT5: 28.5 cycles
      \arg        ADC_SAMPLETIME_41POINT5: 41.5 cycles
      \arg        ADC_SAMPLETIME_55POINT5: 55.5 cycles
      \arg        ADC_SAMPLETIME_71POINT5: 71.5 cycles
      \arg        ADC_SAMPLETIME_239POINT5: 239.5 cycles
    \param[out] none
    \retval     none
*/
void adc_regular_channel_config(uint8_t rank, uint8_t channel,uint32_t sample_time)
{
    uint32_t rsq,sampt;
    
#ifdef GD32F130_150
    if(ADC_CHANNEL_18 == channel){
        channel = ADC_CHANNEL_0;
    }
#endif
    
    /* configure ADC regular sequence */
    if(rank < 6U){
        rsq = ADC_RSQ2;
        rsq &=  ~((uint32_t)(ADC_RSQX_RSQN << (5U*rank)));
        rsq |= ((uint32_t)channel << (5U*rank));
        ADC_RSQ2 = rsq;
    }else if(rank < 12U){
        rsq = ADC_RSQ1;
        rsq &= ~((uint32_t)(ADC_RSQX_RSQN << (5U*(rank-6U))));
        rsq |= ((uint32_t)channel << (5U*(rank-6U)));
        ADC_RSQ1 = rsq;
    }else if(rank < 16U){
        rsq = ADC_RSQ0;
        rsq &= ~((uint32_t)(ADC_RSQX_RSQN << (5U*(rank-12U))));
        rsq |= ((uint32_t)channel << (5U*(rank-12U)));
        ADC_RSQ0 = rsq;
    }else{
        /* illegal parameters */
    }
    
    /* configure ADC sampling time */
    if(channel < 10U){
        sampt = ADC_SAMPT1;
        sampt &= ~((uint32_t)(ADC_SAMPTX_SPTN << (3U*channel)));
        sampt |= (uint32_t)(sample_time << (3U*channel));
        ADC_SAMPT1 = sampt;
    }else if(channel < 18U){
        sampt = ADC_SAMPT0;
        sampt &= ~((uint32_t)(ADC_SAMPTX_SPTN << (3U*(channel-10U))));
        sampt |= (uint32_t)(sample_time << (3U*(channel-10U)));
        ADC_SAMPT0 = sampt;
    }else{
        /* illegal parameters */ 
    }
}

/*!
    \brief      ADC inserted channel config
    \param[in]  rank: the inserted group sequencer rank,this parameter must be between 0 to 3
    \param[in]  channel: the selected ADC channel
      \arg        ADC_CHANNEL_x(x=0..18): ADC Channelx
    \param[in]  sample_time: The sample time value
      \arg        ADC_SAMPLETIME_1POINT5: 1.5 cycles
      \arg        ADC_SAMPLETIME_7POINT5: 7.5 cycles
      \arg        ADC_SAMPLETIME_13POINT5: 13.5 cycles
      \arg        ADC_SAMPLETIME_28POINT5: 28.5 cycles
      \arg        ADC_SAMPLETIME_41POINT5: 41.5 cycles
      \arg        ADC_SAMPLETIME_55POINT5: 55.5 cycles
      \arg        ADC_SAMPLETIME_71POINT5: 71.5 cycles
      \arg        ADC_SAMPLETIME_239POINT5: 239.5 cycles
    \param[out] none
    \retval     none
*/
void adc_inserted_channel_config(uint8_t rank, uint8_t channel, uint32_t sample_time)
{
    uint8_t inserted_length;
    uint32_t isq,sampt;

    inserted_length = (uint8_t)GET_BITS(ADC_ISQ, 20, 21);
    if(rank < 5U){
        isq = ADC_ISQ;
        isq &= ~((uint32_t)(ADC_ISQ_ISQN << (15U - (inserted_length - rank)*5U)));
        isq |= ((uint32_t)channel << (15U - (inserted_length - rank)*5U));
        ADC_ISQ = isq;
    }

    if(channel < 10U){
        sampt = ADC_SAMPT1;
        sampt &= ~((uint32_t)(ADC_SAMPTX_SPTN << (3U*channel)));
        sampt |= (uint32_t) sample_time << (3U*channel);
        ADC_SAMPT1 = sampt;
    }else if(channel < 19U){
        sampt = ADC_SAMPT0;
        sampt &= ~((uint32_t)(ADC_SAMPTX_SPTN << (3U*(channel-10U))));
        sampt |= ((uint32_t)sample_time << (3U*(channel-10U)));
        ADC_SAMPT0 = sampt;
    }else{
        /* illegal parameters */
    }
}

/*!
    \brief      adc inserted channel offset config
    \param[in]  inserted_channel : insert channel select
      \arg        ADC_INSERTED_CHANNEL_0: inserted channel0
      \arg        ADC_INSERTED_CHANNEL_1: inserted channel1
      \arg        ADC_INSERTED_CHANNEL_2: inserted channel2
      \arg        ADC_INSERTED_CHANNEL_3: inserted channel3
    \param[in]  offset : the offset data
    \param[out] none
    \retval     the conversion value
*/
void adc_inserted_channel_offset_config(uint8_t inserted_channel,uint16_t offset)
{
    uint8_t inserted_length;
    uint32_t num = 0U;

    inserted_length = (uint8_t)GET_BITS(ADC_ISQ, 20U, 21U);
    num = 3U - (inserted_length - inserted_channel);
    
    if(num <= 3U){
        /* calculate the offset of the register */
        num = num * 4U;
        /* config the offset of the selected channels */
        REG32((ADC_BASE) + 0x14U + num) = IOFFX_IOFF((uint32_t)offset);
    }
}

/*!
    \brief      adc external trigger enable
    \param[in]  adc_channel_group: select the channel group
      \arg        ADC_REGULAR_CHANNEL: regular channel group
      \arg        ADC_INSERTED_CHANNEL: inserted channel group
    \param[in]  newvalue: ENABLE or DISABLE
    \param[out] none
    \retval     none
*/
void adc_external_trigger_config(uint8_t channel_group,ControlStatus newvalue)
{
    if(newvalue){
        if(RESET != (channel_group & ADC_REGULAR_CHANNEL)){
            ADC_CTL1 |= ADC_CTL1_ETERC;
        }
        if(RESET != (channel_group & ADC_INSERTED_CHANNEL)){
            ADC_CTL1 |= ADC_CTL1_ETEIC;
        }
    }else{
        if(RESET != (channel_group & ADC_REGULAR_CHANNEL)){
            ADC_CTL1 &= ~ADC_CTL1_ETERC;
        }
        if(RESET != (channel_group & ADC_INSERTED_CHANNEL)){
            ADC_CTL1 &= ~ADC_CTL1_ETEIC;
        }
    }
}

/*!
    \brief      adc external trigger source config
    \param[in]  adc_channel_group: select the channel group
      \arg        ADC_REGULAR_CHANNEL: regular channel group
      \arg        ADC_INSERTED_CHANNEL: inserted channel group
    \param[in]  external_trigger_source: regular or inserted group trigger source
                for regular channel:
      \arg        ADC_EXTTRIG_REGULAR_T0_CH0: external trigger timer 0 CH0 event select for regular channel
      \arg        ADC_EXTTRIG_REGULAR_T0_CH1: external trigger timer 0 CH1 event select for regular channel
      \arg        ADC_EXTTRIG_REGULAR_T0_CH2: external trigger timer 0 CH2 event select for regular channel
      \arg        ADC_EXTTRIG_REGULAR_T1_CH1: external trigger timer 1 CH1 event select for regular channel
      \arg        ADC_EXTTRIG_REGULAR_T2_TRGO: external trigger timer 2 TRGO event select for regular channel
      \arg        ADC_EXTTRIG_REGULAR_T14_CH0: external trigger timer 14 CH0 event select for regular channel
      \arg        ADC_EXTTRIG_REGULAR_EXT_IT11: external trigger extiline 11 select for regular channel
      \arg        ADC_EXTTRIG_REGULAR_SWRCST: software trigger select for regular channel
                for inserted channel:
      \arg        ADC_EXTTRIG_INSERTED_T0_TRGO: external trigger timer0 TRGO event select for inserted channel
      \arg        ADC_EXTTRIG_INSERTED_T0_CH3: external trigger timer0 CH3 event select for inserted channel
      \arg        ADC_EXTTRIG_INSERTED_T1_TRGO: external trigger timer1 TRGO event select for inserted channel
      \arg        ADC_EXTTRIG_INSERTED_T1_CH0: external trigger timer1 CH0 event select for inserted channel
      \arg        ADC_EXTTRIG_INSERTED_T2_CH3: external trigger timer2 CH3 event select for inserted channel
      \arg        ADC_EXTTRIG_INSERTED_T14_TRGO: external trigger timer14 TRGO event select for inserted channel
      \arg        ADC_EXTTRIG_INSERTED_EXT_IT15: external interrupt line 15 select for inserted channel
      \arg        ADC_EXTTRIG_INSERTED_SWRCST: software trigger select for inserted channel
    \param[out] none
    \retval     none
*/
void adc_external_trigger_source_config(uint8_t channel_group,uint32_t external_trigger_source)
{   
    switch(channel_group){
    case ADC_REGULAR_CHANNEL:
        ADC_CTL1 &= ~((uint32_t)ADC_CTL1_ETSRC);
        ADC_CTL1 |= (uint32_t)external_trigger_source;
        break;
    case ADC_INSERTED_CHANNEL:
        ADC_CTL1 &= ~((uint32_t)ADC_CTL1_ETSIC);
        ADC_CTL1 |= (uint32_t)external_trigger_source;
        break;
    default:
        break;
    }
}

/*!
    \brief      adc software trigger enable
    \param[in]  adc_channel_group: select the channel group
      \arg        ADC_REGULAR_CHANNEL: regular channel group
      \arg        ADC_INSERTED_CHANNEL: inserted channel group
    \param[out] none
    \retval     none
*/
void adc_software_trigger_enable(uint8_t channel_group)
{
    if(RESET != (channel_group & ADC_REGULAR_CHANNEL)){
        ADC_CTL1 |= ADC_CTL1_SWRCST;
    }
    if(RESET != (channel_group & ADC_INSERTED_CHANNEL)){
        ADC_CTL1 |= ADC_CTL1_SWICST;
    }
}

/*!
    \brief      adc regular group data register read
    \param[in]  none
    \param[out] none
    \retval     the conversion value
*/
uint16_t adc_regular_data_read(void)
{
    return (uint16_t)(ADC_RDATA);
}

/*!
    \brief      adc inserted group data register read
    \param[in]  inserted_channel : insert channel select
      \arg        ADC_INSERTED_CHANNEL_0: inserted channel0
      \arg        ADC_INSERTED_CHANNEL_1: inserted channel1
      \arg        ADC_INSERTED_CHANNEL_2: inserted channel2
      \arg        ADC_INSERTED_CHANNEL_3: inserted channel3
    \param[out] none
    \retval     the conversion value
*/
uint16_t adc_inserted_data_read(uint8_t inserted_channel)
{
    uint32_t idata;
    /* read the data of the selected channel */
    switch(inserted_channel){
    case ADC_INSERTED_CHANNEL_0:
        idata = ADC_IDATA0;
        break;
    case ADC_INSERTED_CHANNEL_1:
        idata = ADC_IDATA1;
        break;
    case ADC_INSERTED_CHANNEL_2:
        idata = ADC_IDATA2;
        break;
    case ADC_INSERTED_CHANNEL_3:
        idata = ADC_IDATA3;
        break;
    default:
        idata = 0U;
        break;
    }
    return (uint16_t)idata;
}

/*!
    \brief      get the ADC flag bits
    \param[in]  flag: the adc flag bits
      \arg        ADC_FLAG_WDE: analog watchdog event flag
      \arg        ADC_FLAG_EOC: end of group conversion flag
      \arg        ADC_FLAG_EOIC: end of inserted group conversion flag
      \arg        ADC_FLAG_STIC: start flag of inserted channel group
      \arg        ADC_FLAG_STRC: start flag of regular channel group
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus adc_flag_get(uint32_t flag)
{
    FlagStatus reval = RESET;
    
    if(ADC_STAT & flag){
        reval = SET;
    }
    return reval;

}

/*!
    \brief      clear the ADC status flag
    \param[in]  flag: the adc flag bits
      \arg        ADC_FLAG_WDE: analog watchdog event flag
      \arg        ADC_FLAG_EOC: end of group conversion flag
      \arg        ADC_FLAG_EOIC: end of inserted group conversion flag
      \arg        ADC_FLAG_STIC: start flag of inserted channel group
      \arg        ADC_FLAG_STRC: start flag of regular channel group
    \param[out] none
    \retval     none
*/
void adc_flag_clear(uint32_t flag)
{
    ADC_STAT &= ~((uint32_t)flag);
}

/*!
    \brief      get the ADC interrupt bits
    \param[in]  flag: the adc interrupt flag
      \arg        ADC_INT_FLAG_WDE: analog watchdog interrupt flag
      \arg        ADC_INT_FLAG_EOC: end of group conversion interrupt flag
      \arg        ADC_INT_FLAG_EOIC: end of inserted group conversion interrupt flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus adc_interrupt_flag_get(uint32_t flag)
{
    FlagStatus interrupt_flag = RESET;
    uint32_t state;
    /* check the interrupt bits */
    switch(flag){
    case ADC_INT_FLAG_WDE:
        state = ADC_STAT & ADC_STAT_WDE;
        if((ADC_CTL0 & ADC_CTL0_WDEIE) && state){
          interrupt_flag = SET;
        }
        break;
    case ADC_INT_FLAG_EOC:
        state = ADC_STAT & ADC_STAT_EOC;
        if((ADC_CTL0 & ADC_CTL0_EOCIE) && state){
            interrupt_flag = SET;
        }
        break;
    case ADC_INT_FLAG_EOIC:
        state = ADC_STAT & ADC_STAT_EOIC;
        if((ADC_CTL0 & ADC_CTL0_EOICIE) && state){
            interrupt_flag = SET;
        }
        break;
    default:
        break;
    }

    return interrupt_flag;
}

/*!
    \brief      clear the ADC interrupt bits
    \param[in]  adc_interrupt: the adc interrupt bits
      \arg        ADC_INT_FLAG_WDE: analog watchdog interrupt flag
      \arg        ADC_INT_FLAG_EOC: end of group conversion interrupt flag
      \arg        ADC_INT_FLAG_EOIC: end of inserted group conversion interrupt flag
    \param[out] none
    \retval     none
*/
void adc_interrupt_flag_clear(uint32_t flag)
{
    ADC_STAT &= ~((uint32_t)flag);
}

/*!
    \brief      enable ADC interrupt 
    \param[in]  adc_periph: ADCx, x=0,1,2
                only one among these parameters can be selected
    \param[in]  interrupt: the adc interrupt
                one or more parameters can be selected
      \arg        ADC_INT_WDE: analog watchdog interrupt
      \arg        ADC_INT_EOC: end of group conversion interrupt
      \arg        ADC_INT_EOIC: end of inserted group conversion interrupt
    \param[out] none
    \retval     none
*/
void adc_interrupt_enable(uint32_t interrupt)
{
    if(RESET != (interrupt & ADC_INT_WDE)){
        ADC_CTL0 |= (uint32_t) ADC_CTL0_WDEIE;
    }

    if(RESET != (interrupt & ADC_INT_EOC)){
        ADC_CTL0 |= (uint32_t) ADC_CTL0_EOCIE;
    }

    if(RESET != (interrupt & ADC_INT_EOIC)){
        ADC_CTL0 |= (uint32_t) ADC_CTL0_EOICIE;
    }
}

/*!
    \brief      disable ADC interrupt 
    \param[in]  adc_periph: ADCx,x=0,1,2
                only one among these parameters can be selected
    \param[in]  interrupt: the adc interrupt flag
                one or more parameters can be selected
      \arg        ADC_INT_WDE: analog watchdog interrupt flag
      \arg        ADC_INT_EOC: end of group conversion interrupt flag
      \arg        ADC_INT_EOIC: end of inserted group conversion interrupt flag
    \param[out] none
    \retval     none
*/
void adc_interrupt_disable(uint32_t interrupt)
{  
    if(RESET != (interrupt & ADC_INT_WDE)){
        ADC_CTL0 &= ~(uint32_t) ADC_CTL0_WDEIE;
    }  

    if(RESET != (interrupt & ADC_INT_EOC)){
        ADC_CTL0 &= ~(uint32_t) ADC_CTL0_EOCIE;
    }  

    if(RESET != (interrupt & ADC_INT_EOIC)){
        ADC_CTL0 &= ~(uint32_t) ADC_CTL0_EOICIE;
    }
}

/*!
    \brief      ADC analog watchdog single channel config
    \param[in]  channel: the selected ADC channel
      \arg        ADC_CHANNEL_x: ADC Channelx(x=0..18)
    \param[out] none
    \retval     none
*/
void adc_watchdog_single_channel_enable(uint8_t channel)
{
    ADC_CTL0 &= (uint32_t)~(ADC_CTL0_RWDEN | ADC_CTL0_IWDEN | ADC_CTL0_WDSC | ADC_CTL0_WDCHSEL);

    ADC_CTL0 |= (uint32_t)channel;
    ADC_CTL0 |= (uint32_t)(ADC_CTL0_RWDEN | ADC_CTL0_IWDEN | ADC_CTL0_WDSC);
}

/*!
    \brief      adc analog watchdog group channel config
    \param[in]  adc_channel_group: the channel group use analog watchdog 
      \arg        ADC_REGULAR_CHANNEL: regular channel group
      \arg        ADC_INSERTED_CHANNEL: inserted channel group
      \arg        ADC_REGULAR_INSERTED_CHANNEL: both regular and inserted group
    \param[out] none
    \retval     none
*/
void adc_watchdog_group_channel_enable(uint8_t channel_group)
{
    ADC_CTL0 &= (uint32_t)~(ADC_CTL0_RWDEN | ADC_CTL0_IWDEN | ADC_CTL0_WDSC);
    /* select the group */
    switch(channel_group){
    case ADC_REGULAR_CHANNEL:
        ADC_CTL0 |= (uint32_t) ADC_CTL0_RWDEN;
        break;
    case ADC_INSERTED_CHANNEL:
        ADC_CTL0 |= (uint32_t) ADC_CTL0_IWDEN;
        break;
    case ADC_REGULAR_INSERTED_CHANNEL:
        ADC_CTL0 |= (uint32_t)(ADC_CTL0_RWDEN | ADC_CTL0_IWDEN);
        break;
    default:
        break;
    }
}



/*!
    \brief      ADC analog watchdog disable
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_watchdog_disable(void)
{
    ADC_CTL0 &= (uint32_t)~(ADC_CTL0_RWDEN | ADC_CTL0_IWDEN | ADC_CTL0_WDSC | ADC_CTL0_WDCHSEL);
}

/*!
    \brief      ADC analog watchdog threshold config
    \param[in]  low_threshold: analog watchdog low threshold
    \param[in]  high_threshold: analog watchdog high threshold
    \param[out] none
    \retval     none
*/
void adc_watchdog_threshold_config(uint16_t low_threshold,uint16_t high_threshold)
{
    ADC_WDLT = (uint32_t)low_threshold;
    ADC_WDHT = (uint32_t)high_threshold;
}

#ifdef GD32F170_190
/*!
    \brief      adc resolution config
    \param[in]  resolution: ADC resolution
      \arg        ADC_RESOLUTION_12B: 12-bit ADC resolution
      \arg        ADC_RESOLUTION_10B: 10-bit ADC resolution
      \arg        ADC_RESOLUTION_8B: 8-bit ADC resolution
      \arg        ADC_RESOLUTION_6B: 6-bit ADC resolution
    \param[out] none
    \retval     none
*/
void adc_resolution_config(uint32_t resolution)
{
    ADC_CTL0 &= ~((uint32_t)ADC_CTL0_DRES);
    ADC_CTL0 |= (uint32_t)resolution;
}

/*!
    \brief      adc oversample mode config
    \param[in]  mode: ADC oversampling mode
      \arg        ADC_OVERSAMPLING_ALL_CONVERT: all oversampled conversions for a channel 
                are done consecutively after a trigger
      \arg        ADC_OVERSAMPLING_ONE_CONVERT: each oversampled conversion for a channel 
                needs a trigger
    \param[in]  shift: ADC oversampling shift
      \arg        ADC_OVERSAMPLING_SHIFT_NONE: no oversampling shift
      \arg        ADC_OVERSAMPLING_SHIFT_1B: 1-bit oversampling shift
      \arg        ADC_OVERSAMPLING_SHIFT_2B: 2-bit oversampling shift
      \arg        ADC_OVERSAMPLING_SHIFT_3B: 3-bit oversampling shift
      \arg        ADC_OVERSAMPLING_SHIFT_4B: 3-bit oversampling shift
      \arg        ADC_OVERSAMPLING_SHIFT_5B: 5-bit oversampling shift
      \arg        ADC_OVERSAMPLING_SHIFT_6B: 6-bit oversampling shift
      \arg        ADC_OVERSAMPLING_SHIFT_7B: 7-bit oversampling shift
      \arg        ADC_OVERSAMPLING_SHIFT_8B: 8-bit oversampling shift
    \param[in]  ratio: ADC oversampling ratio
      \arg        ADC_OVERSAMPLING_RATIO_MUL2: oversampling ratio X2
      \arg        ADC_OVERSAMPLING_RATIO_MUL4: oversampling ratio X4
      \arg        ADC_OVERSAMPLING_RATIO_MUL8: oversampling ratio X8
      \arg        ADC_OVERSAMPLING_RATIO_MUL16: oversampling ratio X16
      \arg        ADC_OVERSAMPLING_RATIO_MUL32: oversampling ratio X32
      \arg        ADC_OVERSAMPLING_RATIO_MUL64: oversampling ratio X64
      \arg        ADC_OVERSAMPLING_RATIO_MUL128: oversampling ratio X128
      \arg        ADC_OVERSAMPLING_RATIO_MUL256: oversampling ratio X256
    \param[out] none
    \retval     none
*/
void adc_oversample_mode_config(uint8_t mode, uint16_t shift,uint8_t ratio)
{
    if(mode){
        ADC_OVSAMPCTL |= (uint32_t)ADC_OVSAMPCTL_TOVS;
    }else{
        ADC_OVSAMPCTL &= ~((uint32_t)ADC_OVSAMPCTL_TOVS);
    }
    /* config the shift and ratio */
    ADC_OVSAMPCTL &= ~((uint32_t)(ADC_OVSAMPCTL_OVSR | ADC_OVSAMPCTL_OVSS));
    ADC_OVSAMPCTL |= ((uint32_t)shift | (uint32_t)ratio);
}

/*!
    \brief      enable ADC oversample mode 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_oversample_mode_enable(void)
{
    ADC_OVSAMPCTL |= ADC_OVSAMPCTL_OVSEN;
}

/*!
    \brief      disable ADC oversample mode 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_oversample_mode_disable(void)
{
    ADC_OVSAMPCTL &= ~((uint32_t)ADC_OVSAMPCTL_OVSEN);
}
#endif /* GD32F170_190 */
