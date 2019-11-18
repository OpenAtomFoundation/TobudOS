/*!
    \file  gd32f1x0_dac.c
    \brief DAC driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/


#include "gd32f1x0_dac.h"

/*!
    \brief      deinit DAC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_deinit(void)
{
    rcu_periph_reset_enable(RCU_DACRST);
    rcu_periph_reset_disable(RCU_DACRST);
}

/*!
    \brief      enable DAC0
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac0_enable(void)
{
    DAC_CTL |= DAC_CTL_DEN0;
}

/*!
    \brief      disable DAC0
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac0_disable(void)
{
    DAC_CTL &= ~DAC_CTL_DEN0;
}

/*!
    \brief      enable DAC0 DMA
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac0_dma_enable(void)
{
    DAC_CTL |= DAC_CTL_DDMAEN0;
}

/*!
    \brief      disable DAC0 DMA
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac0_dma_disable(void)
{
    DAC_CTL &= ~DAC_CTL_DDMAEN0;
}

/*!
    \brief      enable DAC0 output buffer
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac0_output_buffer_enable(void)
{
    DAC_CTL &= ~DAC_CTL_DBOFF0;
}

/*!
    \brief      disable DAC0 output buffer
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac0_output_buffer_disable(void)
{
    DAC_CTL |= DAC_CTL_DBOFF0;
}

/*!
    \brief      enable DAC0 trigger
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac0_trigger_enable(void)
{
    DAC_CTL |= DAC_CTL_DTEN0;
}

/*!
    \brief      disable DAC0 trigger
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac0_trigger_disable(void)
{
    DAC_CTL &= ~DAC_CTL_DTEN0;
}

/*!
    \brief      enable DAC0 software trigger
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac0_software_trigger_enable(void)
{
    DAC_SWT |= DAC_SWT_SWTR0;
}

/*!
    \brief      disable DAC0 software trigger
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac0_software_trigger_disable(void)
{
    DAC_SWT &= ~DAC_SWT_SWTR0;
}

/*!
    \brief      enable DAC0 interrupt(DAC0 DMA underrun interrupt)
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac0_interrupt_enable(void)
{
    DAC_CTL |= DAC_CTL_DDUDRIE0;
}

/*!
    \brief      disable DAC0 interrupt(DAC0 DMA underrun interrupt)
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac0_interrupt_disable(void)
{
    DAC_CTL &= ~DAC_CTL_DDUDRIE0;
}

/*!
    \brief      set DAC0 tgigger source
    \param[in]  triggersource: external triggers of DAC
      \arg        DAC_TRIGGER_T1_TRGO: trigger source is timer1 trgo
      \arg        DAC_TRIGGER_T2_TRGO: trigger source is timer2 trgo
      \arg        DAC_TRIGGER_T5_TRGO: trigger source is timer5 trgo
      \arg        DAC_TRIGGER_T14_TRGO: trigger source is timer14 trgo
      \arg        DAC_TRIGGER_EXTI_IT9: trigger source is exti interrupt line 9
      \arg        DAC_TRIGGER_SOFTWARE: trigger source is software
    \param[out] none
    \retval     none
*/
void dac0_trigger_source_config(uint32_t triggersource)
{
    DAC_CTL &= ~DAC_CTL_DTSEL0;
    DAC_CTL |= triggersource;
}

/*!
    \brief      get the last data output value of DAC0
    \param[in]  none
    \param[out] none
    \retval     DAC output data
*/
uint16_t dac0_output_value_get(void)
{
    uint16_t data = 0U;
    data = (uint16_t)DAC0_DO;
    return data;
}

/*!
    \brief      get the specified DAC0 flag(DAC0 DMA underrun flag)
    \param[in]  none
    \param[out] none
    \retval     the state of dac bit(SET or RESET)
*/
FlagStatus dac0_flag_get(void)
{
    /* check the DMA underrun flag */
    if((uint8_t)RESET != (DAC_STAT & DAC_STAT_DDUDR0)){
        return SET;
    }else{
        return RESET;
    }
}

/*!
    \brief      clear the specified DAC0 flag(DAC0 DMA underrun flag)
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac0_flag_clear(void)
{
    DAC_STAT &= ~DAC_STAT_DDUDR0;
}

/*!
    \brief      get the specified DAC0 interrupt flag(DAC0 DMA underrun interrupt flag)
    \param[in]  none
    \param[out] none
    \retval     the state of DAC interrupt flag(SET or RESET)
*/
FlagStatus dac0_interrupt_flag_get(void)
{
    uint32_t ddudr_flag = 0U, ddudrie_flag = 0U;
    ddudr_flag = (DAC_STAT & DAC_STAT_DDUDR0);
    ddudrie_flag = DAC_CTL & DAC_CTL_DDUDRIE0;
    /* check the DMA underrun flag and DAC DMA underrun interrupt enable flag */
    if((RESET != ddudr_flag) && (RESET != ddudrie_flag)){
         return SET;
    }else{
        return RESET;
    }
}

/*!
    \brief      clear the specified DAC0 interrupt flag(DAC0 DMA underrun interrupt flag)
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac0_interrupt_flag_clear(void)
{
    DAC_CTL &= ~DAC_CTL_DDUDRIE0;
}

/*!
    \brief      set DAC0 data holding register value
    \param[in]  dac_align
      \arg        DAC_ALIGN_8B_R: data right 8b alignment
      \arg        DAC_ALIGN_12B_R: data right 12b alignment
      \arg        DAC_ALIGN_12B_L: data left 12b alignment
    \param[in]  data: data to be loaded
    \param[out] none
    \retval     none
*/
void dac0_data_set(uint32_t dac_align, uint16_t data)
{
    switch(dac_align){
        /* data right 12b alignment */
        case DAC_ALIGN_12B_R:
            DAC0_R12DH = data;
            break;
        /* data left 12b alignment */
        case DAC_ALIGN_12B_L:
            DAC0_L12DH = data;
            break;
        /* data right 8b alignment */
        case DAC_ALIGN_8B_R:
            DAC0_R8DH = data;
            break;
        default:
            break;
    }
}

#ifdef GD32F170_190
/*!
    \brief      enable DAC
    \param[in]  dac_periph
      \arg        DACx(x =0,1)
    \param[out] none
    \retval     none
*/
void dac_enable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_CTL |= DAC_CTL_DEN0;
    }else{
        DAC_CTL |= DAC_CTL_DEN1;
    }
} 

/*!
    \brief      disable DAC
    \param[in]  dac_periph
      \arg        DACx(x =0,1)
    \param[out] none
    \retval     none
*/
void dac_disable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_CTL &= ~DAC_CTL_DEN0;
    }else{
        DAC_CTL &= ~DAC_CTL_DEN1;
    }
}

/*!
    \brief      enable DAC DMA function
    \param[in]  dac_periph
      \arg        DACx(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_dma_enable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_CTL |= DAC_CTL_DDMAEN0;
    }else{
        DAC_CTL |= DAC_CTL_DDMAEN1;
    }
}

/*!
    \brief      disable DAC DMA function
    \param[in]  dac_periph
      \arg        DACx(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_dma_disable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_CTL &= ~DAC_CTL_DDMAEN0;
    }else{
        DAC_CTL &= ~DAC_CTL_DDMAEN1;
    }
}

/*!
    \brief      enable DAC output buffer
    \param[in]  dac_periph
      \arg        DACx(x =0,1)
    \param[out] none
    \retval     none
*/
void dac_output_buffer_enable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_CTL &= ~DAC_CTL_DBOFF0;
    }else{
        DAC_CTL &= ~DAC_CTL_DBOFF1;
    }
}

/*!
    \brief      disable DAC output buffer
    \param[in]  dac_periph
      \arg        DACx(x =0,1)
    \param[out] none
    \retval     none
*/
void dac_output_buffer_disable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_CTL |= DAC_CTL_DBOFF0;
    }else{
        DAC_CTL |= DAC_CTL_DBOFF1;
    }
}

/*!
    \brief      enable DAC trigger
    \param[in]  dac_periph
      \arg        DACx(x =0,1)
    \param[out] none
    \retval     none
*/
void dac_trigger_enable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_CTL |= DAC_CTL_DTEN0;
    }else{
        DAC_CTL |= DAC_CTL_DTEN1;
    }
}

/*!
    \brief      disable DAC trigger
    \param[in]  dac_periph
      \arg        DACx(x =0,1)
    \param[out] none
    \retval     none
*/
void dac_trigger_disable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_CTL &= ~DAC_CTL_DTEN0;
    }else{
        DAC_CTL &= ~DAC_CTL_DTEN1;
    }
}

/*!
    \brief      enable DAC software trigger
    \param[in]  dac_periph
      \arg        DACx(x =0,1)
    \retval     none
*/
void dac_software_trigger_enable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_SWT |= DAC_SWT_SWTR0;
    }else{
        DAC_SWT |= DAC_SWT_SWTR1;
    }
}

/*!
    \brief      disable DAC software trigger
    \param[in]  dac_periph
      \arg        DACx(x =0,1)
    \param[out] none
    \retval     none
*/
void dac_software_trigger_disable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_SWT &= ~DAC_SWT_SWTR0;
    }else{
        DAC_SWT &= ~DAC_SWT_SWTR1;
    }
}

/*!
    \brief      get DAC output value
    \param[in]  dac_periph
      \arg        DACx(x=0,1)
    \param[out] none
    \retval     DAC output data
*/
uint16_t dac_output_value_get(uint32_t dac_periph)
{
    uint16_t data = 0U;
    if(DAC0 == dac_periph){
        data = (uint16_t)DAC0_DO;
    }else{
        data = (uint16_t)DAC1_DO;
    }
    return data;
}

/*!
    \brief      enable DAC interrupt(DAC0 DMA underrun interrupt)
    \param[in]  dac_periph
      \arg        DACx(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_interrupt_enable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_CTL |= DAC_CTL_DDUDRIE0;
    }else{
        DAC_CTL |= DAC_CTL_DDUDRIE1;
    }
}

/*!
    \brief      disable DAC interrupt(DAC0 DMA underrun interrupt)
    \param[in]  dac_periph
      \arg        DACx(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_interrupt_disable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_CTL &= ~DAC_CTL_DDUDRIE0;
    }else{
        DAC_CTL &= ~DAC_CTL_DDUDRIE1;
    }
}

/*!
    \brief      set DAC trigger source
    \param[in]  dac_periph
      \arg        DACx(x =0,1)
    \param[in]  triggersource: external triggers of DAC
      \arg        DAC_TRIGGER_T1_TRGO: trigger source is timer1 trgo
      \arg        DAC_TRIGGER_T2_TRGO: trigger source is timer2 trgo
      \arg        DAC_TRIGGER_T5_TRGO: trigger source is timer5 trgo
      \arg        DAC_TRIGGER_T14_TRGO: trigger source is timer14 trgo
      \arg        DAC_TRIGGER_EXTI_IT9: trigger source is exti interrupt line 9
      \arg        DAC_TRIGGER_SOFTWARE: trigger source is software
    \param[out] none
    \retval     none
*/
void dac_trigger_source_config(uint32_t dac_periph,uint32_t triggersource)
{
    if(DAC0 == dac_periph){
        DAC_CTL &= ~DAC_CTL_DTSEL0;
        DAC_CTL |= triggersource;
    }else{
        DAC_CTL &= ~DAC_CTL_DTSEL1;
        DAC_CTL |= (triggersource <<16);
    }
}

/*!
    \brief      get the specified DAC flag(DAC DMA underrun flag)
    \param[in]  dac_periph
      \arg        DACx(x=0,1)
    \param[out] none
    \retval     the state of dac bit(SET or RESET)
*/
FlagStatus dac_flag_get(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        /* check the DMA underrun flag */
        if(RESET != (DAC_STAT & DAC_STAT_DDUDR0)){
            return SET;
        }else{
            return RESET;
        }
    }else{
        /* check the DMA underrun flag */
        if(RESET != (DAC_STAT & DAC_STAT_DDUDR1)){
            return SET;
        }else{
            return RESET;
        }
    }
}

/*!
    \brief      clear the specified DAC flag(DAC DMA underrun flag)
    \param[in]  dac_periph
      \arg        DACx(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_flag_clear(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_STAT |= DAC_STAT_DDUDR0;
    }else{
        DAC_STAT |= DAC_STAT_DDUDR1;
    }
}

/*!
    \brief      get the specified DAC interrupt flag(DAC DMA underrun interrupt flag)
    \param[in]  dac_periph
      \arg        DACx(x=0,1)
    \param[out] none
    \retval     the state of DAC interrupt flag(SET or RESET)
*/
FlagStatus dac_interrupt_flag_get(uint32_t dac_periph)
{
    uint32_t ddudr_flag = 0U, ddudrie_flag = 0U;
    if(DAC0 == dac_periph){
        ddudr_flag = DAC_STAT & DAC_STAT_DDUDR0;
        ddudrie_flag = DAC_CTL & DAC_CTL_DDUDRIE0;
        /* check the DMA underrun flag and DAC DMA underrun interrupt enable flag */
        if((RESET != ddudr_flag) && (RESET != ddudrie_flag)){
             return SET;
        }else{
            return RESET;
        }
    }else{
        ddudr_flag = DAC_STAT & DAC_STAT_DDUDR1;
        ddudrie_flag = DAC_CTL & DAC_CTL_DDUDRIE1;
        /* check the DMA underrun flag and DAC DMA underrun interrupt enable flag */
        if((RESET != ddudr_flag) && (RESET != ddudrie_flag)){
             return SET;
        }else{
            return RESET;
        }
    }
}

/*!
    \brief      clear the specified DAC interrupt flag(DAC DMA underrun interrupt flag)
    \param[in]  dac_periph
      \arg        DACx(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_interrupt_flag_clear(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_STAT |= DAC_STAT_DDUDR0;
    }else{
        DAC_STAT |= DAC_STAT_DDUDR1;
    }
}

/*!
    \brief      enable DAC concurrent mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_enable(void)
{
    uint32_t ctl = 0U;
    ctl = DAC_CTL_DEN0 | DAC_CTL_DEN1;
    DAC_CTL |= (ctl);
}

/*!
    \brief      disable DAC concurrent mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_disable(void)
{
    uint32_t ctl = 0U;
    ctl = DAC_CTL_DEN0 | DAC_CTL_DEN1;
    DAC_CTL &= (~ctl);
}

/*!
    \brief      enable DAC concurrent software trigger function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_software_trigger_enable(void)
{
    uint32_t swt = 0U;
    swt = DAC_SWT_SWTR0 | DAC_SWT_SWTR1;
    DAC_SWT |= (swt); 
}

/*!
    \brief      disable DAC concurrent software trigger function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_software_trigger_disable(void)
{
    uint32_t swt = 0U;
    swt = DAC_SWT_SWTR0 | DAC_SWT_SWTR1;
    DAC_SWT &= (~swt);
}

/*!
    \brief      enable DAC concurrent buffer funcution
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_output_buffer_enable(void)
{
    uint32_t ctl = 0U;
    ctl = DAC_CTL_DBOFF0 | DAC_CTL_DBOFF1;
    DAC_CTL &= (~ctl);
}

/*!
    \brief      disable DAC concurrent buffer funcution
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_output_buffer_disable(void)
{
    uint32_t ctl = 0U;
    ctl = DAC_CTL_DBOFF0 | DAC_CTL_DBOFF1;
    DAC_CTL |= (ctl);
}

/*!
    \brief      enable DAC concurrent interrupt funcution
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_interrupt_enable(void)
{
    DAC_CTL |= DAC_CTL_DDUDRIE0;
    DAC_CTL |= DAC_CTL_DDUDRIE1;
}

/*!
    \brief      disable DAC concurrent interrupt funcution
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_interrupt_disable(void)
{
    DAC_CTL &= ~DAC_CTL_DDUDRIE0;
    DAC_CTL &= ~DAC_CTL_DDUDRIE1;
}

/*!
    \brief      set the DAC specified data holding register value
    \param[in]  dac_periph
      \arg        DACx(x=0,1)
    \param[in]  dac_align
      \arg        DAC_ALIGN_8B_R: data right 8b alignment
      \arg        DAC_ALIGN_12B_R: data right 12b alignment
      \arg        DAC_ALIGN_12B_L: data left 12b alignment
    \param[in]  data: data to be loaded
    \param[out] none
    \retval     none
*/
void dac_data_set(uint32_t dac_periph, uint32_t dac_align, uint16_t data)
{
    if(DAC0 == dac_periph){
        switch(dac_align){
        /* data right 12b alignment */
        case DAC_ALIGN_12B_R:
            DAC0_R12DH = data;
            break;
        /* data left 12b alignment */
        case DAC_ALIGN_12B_L:
            DAC0_L12DH = data;
            break;
        /* data right 8b alignment */
        case DAC_ALIGN_8B_R:
            DAC0_R8DH = data;
            break;
        default:
            break;
        }
    }else{
        switch(dac_align){
        /* data right 12b alignment */
        case DAC_ALIGN_12B_R:
            DAC1_R12DH = data;
            break;
        /* data left 12b alignment */
        case DAC_ALIGN_12B_L:
            DAC1_L12DH = data;
            break;
        /* data right 8b alignment */
        case DAC_ALIGN_8B_R:
            DAC1_R8DH = data;
            break;
        default:
            break;
        }
    }
}

/*!
    \brief      set DAC concurrent mode data holding register value
    \param[in]  dac_align
      \arg        DAC_ALIGN_8B_R: data right 8b alignment
      \arg        DAC_ALIGN_12B_R: data right 12b alignment
      \arg        DAC_ALIGN_12B_L: data left 12b alignment
    \param[in]  data1: data to be loaded
    \param[in]  data2: data to be loaded
    \param[out] none
    \retval     none
*/
void dac_concurrent_data_set(uint32_t dac_align, uint16_t data1, uint16_t data2)
{
    uint32_t data = 0U;
    switch(dac_align){
    /* data right 12b alignment */
    case DAC_ALIGN_12B_R:
        data = ((uint32_t)data2 << 16U) | data1;
        DACC_R12DH = data;
        break;
    /* data left 12b alignment */
    case DAC_ALIGN_12B_L:
        data = ((uint32_t)data2 << 16U) | data1;
        DACC_L12DH = data;
        break;
    /* data right 8b alignment */
    case DAC_ALIGN_8B_R:
        data = ((uint32_t)data2 << 8U) | data1;
        DACC_R8DH = data;
        break;
    default:
        break;
    }
}

#endif /* GD32F170_190 */
