/*!
    \file  gd32f1x0_timer.c
    \brief TIMER driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#include "gd32f1x0_timer.h"

/*!
    \brief      deinit a timer
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[out] none
    \retval     none
*/
void timer_deinit(uint32_t timer_periph)
{
    switch(timer_periph){
    case TIMER0:
        /* reset TIMER0 */
        rcu_periph_reset_enable(RCU_TIMER0RST);
        rcu_periph_reset_disable(RCU_TIMER0RST);
        break;
    case TIMER1:
        /* reset TIMER1 */
        rcu_periph_reset_enable(RCU_TIMER1RST);
        rcu_periph_reset_disable(RCU_TIMER1RST);
        break;
    case TIMER2:
        /* reset TIMER2 */
        rcu_periph_reset_enable(RCU_TIMER2RST);
        rcu_periph_reset_disable(RCU_TIMER2RST);
        break;
    case TIMER5:
        /* reset TIMER5 */
        rcu_periph_reset_enable(RCU_TIMER5RST);
        rcu_periph_reset_disable(RCU_TIMER5RST);
        break;
    case TIMER13:
        /* reset TIMER13 */
        rcu_periph_reset_enable(RCU_TIMER13RST);
        rcu_periph_reset_disable(RCU_TIMER13RST);
        break;
    case TIMER14:
        /* reset TIMER14 */
        rcu_periph_reset_enable(RCU_TIMER14RST);
        rcu_periph_reset_disable(RCU_TIMER14RST);
        break;
    case TIMER15:
        /* reset TIMER15 */
        rcu_periph_reset_enable(RCU_TIMER15RST);
        rcu_periph_reset_disable(RCU_TIMER15RST);
        break;
    case TIMER16:
        /* reset TIMER16 */
        rcu_periph_reset_enable(RCU_TIMER16RST);
        rcu_periph_reset_disable(RCU_TIMER16RST);
        break;
    default:
        break;
    }
}

/*!
    \brief      initialize TIMER counter
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[in]  initpara: init parameter struct
                prescaler: prescaler value of the counter clock,0~65535
                alignedmode: TIMER_COUNTER_EDGE,TIMER_COUNTER_CENTER_DOWN,TIMER_COUNTER_CENTER_UP,TIMER_COUNTER_CENTER_BOTH
                counterdirection: TIMER_COUNTER_UP,TIMER_COUNTER_DOWN
                period: counter auto reload value
                clockdivision: TIMER_CKDIV_DIV1,TIMER_CKDIV_DIV2,TIMER_CKDIV_DIV4
                repetitioncounter: counter repetition value,0~255
    \param[out] none
    \retval     none
*/
void timer_init(uint32_t timer_periph, timer_parameter_struct* initpara)
{
    /* configure the counter prescaler value */
    TIMER_PSC(timer_periph) = (uint16_t)initpara->prescaler;

    /* configure the counter direction and aligned mode */
    if((TIMER0 == timer_periph) || (TIMER1 == timer_periph) || (TIMER2 == timer_periph)){
        TIMER_CTL0(timer_periph) &= (uint32_t)(~ TIMER_CTL0_DIR|TIMER_CTL0_CAM);
        TIMER_CTL0(timer_periph) |= (uint32_t)initpara->alignedmode;
        TIMER_CTL0(timer_periph) |= (uint32_t)initpara->counterdirection;
    }else{
        TIMER_CTL0(timer_periph) &= (uint32_t)(~ TIMER_CTL0_DIR);
        TIMER_CTL0(timer_periph) |= (uint32_t)initpara->counterdirection;
    }

    /* configure the autoreload value */
    TIMER_CAR(timer_periph) = (uint32_t)initpara->period;

    if((TIMER5 != timer_periph)){
        /* reset the CKDIV bit */
        TIMER_CTL0(timer_periph) &= (uint32_t)(~ TIMER_CTL0_CKDIV);
        TIMER_CTL0(timer_periph) |= (uint32_t)initpara->clockdivision;
    }

    if((TIMER0 == timer_periph) || (TIMER14 == timer_periph) || (TIMER15 == timer_periph)|| (TIMER16 == timer_periph)){
        /* configure the repetition counter value */
        TIMER_CREP(timer_periph) = (uint32_t)initpara->repetitioncounter;
    }

    /* generate an update event */
    TIMER_SWEVG(timer_periph) |= (uint32_t)TIMER_PSC_RELOAD_NOW;
}

/*!
    \brief      enable a timer
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[out] none
    \retval     none
*/
void timer_enable(uint32_t timer_periph)
{
    TIMER_CTL0(timer_periph) |= (uint32_t)TIMER_COUNTER_ENABLE;
}

/*!
    \brief      disable a timer
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[out] none
    \retval     none
*/
void timer_disable(uint32_t timer_periph)
{
    TIMER_CTL0(timer_periph) &= (uint32_t)TIMER_COUNTER_DISABLE;
}

/*!
    \brief      enable the auto reload shadow function
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[out] none
    \retval     none
*/
void timer_auto_reload_shadow_enable(uint32_t timer_periph)
{
    TIMER_CTL0(timer_periph) |= (uint32_t) TIMER_ARSE_ENABLE;
}

/*!
    \brief      disable the auto reload shadow function
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[out] none
    \retval     none
*/
void timer_auto_reload_shadow_disable(uint32_t timer_periph)
{
    TIMER_CTL0(timer_periph) &= (uint32_t) TIMER_ARSE_DISABLE;
}
 
/*!
    \brief      enable the update event
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[out] none
    \retval     none
*/
void timer_update_event_enable(uint32_t timer_periph)
{
    TIMER_CTL0(timer_periph) |= (uint32_t) TIMER_UPDIS_ENABLE;
}

/*!
    \brief      disable the update event
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[out] none
    \retval     none
*/
void timer_update_event_disable(uint32_t timer_periph)
{
    TIMER_CTL0(timer_periph) &= (uint32_t) TIMER_UPDIS_DISABLE;
}

/*!
    \brief      set TIMER counter alignment mode
    \param[in]  timer_periph: TIMERx(x=0,1,2)
    \param[in]  aligned: 
      \arg        TIMER_COUNTER_EDGE: edge-aligned mode
      \arg        TIMER_COUNTER_CENTER_DOWN: center-aligned and counting down assert mode
      \arg        TIMER_COUNTER_CENTER_UP: center-aligned and counting up assert mode
      \arg        TIMER_COUNTER_CENTER_BOTH: center-aligned and counting up/down assert mode
    \param[out] none
    \retval     none
*/
void timer_counter_alignment(uint32_t timer_periph, uint16_t aligned)
{
    TIMER_CTL0(timer_periph) &= (uint32_t)(~ TIMER_CTL0_CAM);
    TIMER_CTL0(timer_periph) |= (uint32_t)aligned;
}

/*!
    \brief      set TIMER counter up direction
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[out] none
    \retval     none
*/
void timer_counter_up_direction(uint32_t timer_periph)
{
    TIMER_CTL0(timer_periph) &= (uint32_t)(~ TIMER_CTL0_DIR);
    TIMER_CTL0(timer_periph) |= (uint32_t)TIMER_COUNTER_UP;
}

/*!
    \brief      set TIMER counter down direction
    \param[in]  timer_periph: TIMERx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void timer_counter_down_direction(uint32_t timer_periph)
{
    TIMER_CTL0(timer_periph) &= (uint32_t)(~ TIMER_CTL0_DIR);
    TIMER_CTL0(timer_periph) |= (uint32_t)TIMER_COUNTER_DOWN;
}

/*!
    \brief      configure TIMER prescaler
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[in]  prescaler: prescaler value
    \param[in]  pscreload: prescaler reload mode
      \arg        TIMER_PSC_RELOAD_NOW: the prescaler is loaded right now
      \arg        TIMER_PSC_RELOAD_UPDATE: the prescaler is loaded at the next update event
    \param[out] none
    \retval     none
*/
void timer_prescaler_config(uint32_t timer_periph, uint16_t prescaler, uint8_t pscreload)
{
    TIMER_PSC(timer_periph) = (uint32_t)prescaler;
    
    if(TIMER_PSC_RELOAD_NOW == pscreload){
        TIMER_SWEVG(timer_periph) |= (uint32_t)TIMER_SWEVG_UPG;
    }    
}

/*!
    \brief      configure TIMER repetition register value
    \param[in]  timer_periph: TIMERx(x=0,14,15,16)
    \param[in]  repetition: the counter repetition value
    \param[out] none
    \retval     none
*/
void timer_repetition_value_config(uint32_t timer_periph, uint16_t repetition)
{
    TIMER_CREP(timer_periph) = (uint32_t)repetition;
} 
 
/*!
    \brief      configure TIMER autoreload register value
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[in]  autoreload: the counter auto-reload value
    \param[out] none
    \retval     none
*/         
void timer_autoreload_value_config(uint32_t timer_periph, uint16_t autoreload)
{
    TIMER_CAR(timer_periph) = (uint32_t)autoreload;
}

/*!
    \brief      configure TIMER counter register value
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[in]  counter: the counter value
    \param[out] none
    \retval     none
*/         
void timer_counter_value_config(uint32_t timer_periph, uint16_t counter)
{
    TIMER_CNT(timer_periph) = (uint32_t)counter;
}

/*!
    \brief      read TIMER counter value
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[out] none
    \retval     counter value
*/         
uint32_t timer_counter_read(uint32_t timer_periph)
{
    uint32_t count_value = 0U;
    count_value = TIMER_CNT(timer_periph);
    return (count_value);
}

/*!
    \brief      read TIMER prescaler value
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[out] none
    \retval     prescaler register value
*/         
uint16_t timer_prescaler_read(uint32_t timer_periph)
{
    uint16_t prescaler_value = 0U;
    prescaler_value = (uint16_t)(TIMER_PSC(timer_periph));
    return (prescaler_value);
}

/*!
    \brief      configure TIMER single pulse mode
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[in]  spmode: 
      \arg        TIMER_SP_MODE_SINGLE: single pulse mode
      \arg        TIMER_SP_MODE_REPETITIVE: repetitive pulse mode
    \param[out] none
    \retval     none
*/
void timer_single_pulse_mode_config(uint32_t timer_periph, uint8_t spmode)
{
    if(TIMER_SP_MODE_SINGLE == spmode){
        TIMER_CTL0(timer_periph) |= (uint32_t)TIMER_CTL0_SPM;
    }else if(TIMER_SP_MODE_REPETITIVE == spmode){
        TIMER_CTL0(timer_periph) &= ~((uint32_t)TIMER_CTL0_SPM);
    }else{
    }       
}

/*!
    \brief      configure TIMER update source 
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[in]  update: 
      \arg        TIMER_UPDATE_SRC_GLOBAL: update generate by setting of UPG bit or the counter overflow/underflow,or the slave mode controller trigger
      \arg        TIMER_UPDATE_SRC_REGULAR: update generate only by counter overflow/underflow
    \param[out] none
    \retval     none
*/
void timer_update_source_config(uint32_t timer_periph, uint8_t update)
{
    if(TIMER_UPDATE_SRC_REGULAR == update){
        TIMER_CTL0(timer_periph) |= (uint32_t)TIMER_CTL0_UPS;
    }else if(update == TIMER_UPDATE_SRC_GLOBAL){
        TIMER_CTL0(timer_periph) &= ~(uint32_t)TIMER_CTL0_UPS;
    }else{
    }
}

/*!
    \brief      enable the TIMER interrupt
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[in]  interrupt: timer interrupt enable source
      \arg        TIMER_INT_UP: update interrupt enable
      \arg        TIMER_INT_CH0: channel 0 interrupt enable
      \arg        TIMER_INT_CH1: channel 1 interrupt enable
      \arg        TIMER_INT_CH2: channel 2 interrupt enable
      \arg        TIMER_INT_CH3: channel 3 interrupt enable
      \arg        TIMER_INT_CMT: commutation interrupt enable
      \arg        TIMER_INT_TRG: trigger interrupt enable
      \arg        TIMER_INT_BRK: break interrupt enable
    \param[out] none
    \retval     none
*/
void timer_interrupt_enable(uint32_t timer_periph, uint32_t interrupt)
{
    TIMER_DMAINTEN(timer_periph) |= (uint32_t) interrupt; 
}

/*!
    \brief      disable the TIMER interrupt
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[in]  interrupt: timer interrupt source enable 
      \arg        TIMER_INT_UP: update interrupt enable
      \arg        TIMER_INT_CH0: channel 0 interrupt enable
      \arg        TIMER_INT_CH1: channel 1 interrupt enable
      \arg        TIMER_INT_CH2: channel 2 interrupt enable
      \arg        TIMER_INT_CH3: channel 3 interrupt enable
      \arg        TIMER_INT_CMT: commutation interrupt enable
      \arg        TIMER_INT_TRG: trigger interrupt enable
      \arg        TIMER_INT_BRK: break interrupt enable
    \param[out] none
    \retval     none
*/
void timer_interrupt_disable(uint32_t timer_periph, uint32_t interrupt)
{
    TIMER_DMAINTEN(timer_periph) &= (uint32_t)(~ interrupt); 
}

/*!
    \brief      get timer interrupt flag
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[in]  interrupt: the timer interrupt bits
      \arg        TIMER_INT_FLAG_UP: update interrupt flag 
      \arg        TIMER_INT_FLAG_CH0: channel 0 interrupt flag 
      \arg        TIMER_INT_FLAG_CH1: channel 1 interrupt flag
      \arg        TIMER_INT_FLAG_CH2: channel 2 interrupt flag
      \arg        TIMER_INT_FLAG_CH3: channel 3 interrupt flag
      \arg        TIMER_INT_FLAG_CMT: channel commutation interrupt flag 
      \arg        TIMER_INT_FLAG_TRG: trigger interrupt flag
      \arg        TIMER_INT_FLAG_BRK:  break interrupt flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus timer_interrupt_flag_get(uint32_t timer_periph, uint32_t interrupt)
{
    uint16_t intval = 0U, intenable = 0U;

    intval = (uint16_t)(TIMER_INTF(timer_periph) & interrupt);
    intenable = (uint16_t)(TIMER_DMAINTEN(timer_periph) & interrupt);
    
    if(((uint16_t)RESET != intval ) && (((uint16_t)RESET) != intenable)){
        return SET;
    }else{
        return RESET;
    }
}

/*!
    \brief      clear TIMER interrupt flag
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[in]  interrupt: the timer interrupt bits
      \arg        TIMER_INT_FLAG_UP: update interrupt flag 
      \arg        TIMER_INT_FLAG_CH0: channel 0 interrupt flag 
      \arg        TIMER_INT_FLAG_CH1: channel 1 interrupt flag
      \arg        TIMER_INT_FLAG_CH2: channel 2 interrupt flag
      \arg        TIMER_INT_FLAG_CH3: channel 3 interrupt flag
      \arg        TIMER_INT_FLAG_CMT: channel commutation interrupt flag 
      \arg        TIMER_INT_FLAG_TRG: trigger interrupt flag
      \arg        TIMER_INT_FLAG_BRK:  break interrupt flag
    \param[out] none
    \retval     none
*/
void timer_interrupt_flag_clear(uint32_t timer_periph, uint32_t interrupt)
{
    TIMER_INTF(timer_periph) &= (uint32_t)(~(uint32_t)interrupt);
}

/*!
    \brief      get TIMER flags
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[in]  flag: the timer interrupt flags
      \arg        TIMER_FLAG_UP: update flag
      \arg        TIMER_FLAG_CH0: channel 0 flag
      \arg        TIMER_FLAG_CH1: channel 1 flag
      \arg        TIMER_FLAG_CH2: channel 2 flag
      \arg        TIMER_FLAG_CH3: channel 3 flag
      \arg        TIMER_FLAG_CMT: channel commutation flag 
      \arg        TIMER_FLAG_TRG: trigger flag 
      \arg        TIMER_FLAG_BRK: break flag
      \arg        TIMER_FLAG_CH0O: channel 0 overcapture flag
      \arg        TIMER_FLAG_CH1O: channel 1 overcapture flag
      \arg        TIMER_FLAG_CH2O: channel 2 overcapture flag
      \arg        TIMER_FLAG_CH3O: channel 3 overcapture flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus timer_flag_get(uint32_t timer_periph, uint32_t flag)
{
    if(((uint16_t)RESET) != ((uint16_t)(TIMER_INTF(timer_periph) & flag))){
        return SET;
    }else{
        return RESET;
    }
}

/*!
    \brief      clear TIMER flags
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[in]  flag: the timer interrupt flags
      \arg        TIMER_FLAG_UP: update flag
      \arg        TIMER_FLAG_CH0: channel 0 flag
      \arg        TIMER_FLAG_CH1: channel 1 flag
      \arg        TIMER_FLAG_CH2: channel 2 flag
      \arg        TIMER_FLAG_CH3: channel 3 flag
      \arg        TIMER_FLAG_CMT: channel commutation flag 
      \arg        TIMER_FLAG_TRG: trigger flag 
      \arg        TIMER_FLAG_BRK: break flag
      \arg        TIMER_FLAG_CH0O: channel 0 overcapture flag
      \arg        TIMER_FLAG_CH1O: channel 1 overcapture flag
      \arg        TIMER_FLAG_CH2O: channel 2 overcapture flag
      \arg        TIMER_FLAG_CH3O: channel 3 overcapture flag
    \param[out] none
    \retval     none
*/
void timer_flag_clear(uint32_t timer_periph, uint32_t flag)
{
    TIMER_INTF(timer_periph) &= (uint32_t)(~(uint32_t)flag);
}

/*!
    \brief      enable the TIMER DMA
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,14,15,16)
    \param[in]  dma: timer DMA source enable 
      \arg        TIMER_DMA_UPD:  update DMA enable
      \arg        TIMER_DMA_CH0D: channel 0 DMA enable
      \arg        TIMER_DMA_CH1D: channel 1 DMA enable
      \arg        TIMER_DMA_CH2D: channel 2 DMA enable
      \arg        TIMER_DMA_CH3D: channel 3 DMA enable
      \arg        TIMER_DMA_CMTD: channel commutation DMA request enable
      \arg        TIMER_DMA_TRGD: trigger DMA enable
    \param[out] none
    \retval     none
*/
void timer_dma_enable(uint32_t timer_periph, uint16_t dma)
{
    TIMER_DMAINTEN(timer_periph) |= (uint32_t) dma; 
}

/*!
    \brief      disable the TIMER DMA
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,14,15,16)
    \param[in]  dma: timer DMA source enable 
      \arg        TIMER_DMA_UPD:  update DMA enable
      \arg        TIMER_DMA_CH0D: channel 0 DMA enable
      \arg        TIMER_DMA_CH1D: channel 1 DMA enable
      \arg        TIMER_DMA_CH2D: channel 2 DMA enable
      \arg        TIMER_DMA_CH3D: channel 3 DMA enable
      \arg        TIMER_DMA_CMTD: channel commutation DMA request enable
      \arg        TIMER_DMA_TRGD: trigger DMA enable
    \param[out] none
    \retval     none
*/
void timer_dma_disable(uint32_t timer_periph, uint16_t dma)
{
    TIMER_DMAINTEN(timer_periph) &= ~(uint32_t)dma; 
}

/*!
    \brief      channel DMA request source selection
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,14,15,16)
    \param[in]  dma_request: channel DMA request source selection
       \arg        TIMER_DMAREQUEST_CHANNELEVENT: DMA request of channel y is sent when channel y event occurs
       \arg        TIMER_DMAREQUEST_UPDATEEVENT: DMA request of channel y is sent when update event occurs 
    \param[out] none
    \retval     none
*/
void timer_channel_dma_request_source_select(uint32_t timer_periph, uint8_t dma_request)
{
    if(TIMER_DMAREQUEST_UPDATEEVENT == dma_request){
        TIMER_CTL1(timer_periph) |= (uint32_t)TIMER_CTL1_DMAS;
    }else if(TIMER_DMAREQUEST_CHANNELEVENT == dma_request){
        TIMER_CTL1(timer_periph) &= ~(uint32_t)TIMER_CTL1_DMAS;
    }else{
    }
}

/*!
    \brief      configure the TIMER DMA transfer
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,14,15,16)
    \param[in]  dma_baseaddr: 
       \arg        TIMER_DMACFG_DMATA_CTL0: DMA transfer address is TIMER_CTL0
       \arg        TIMER_DMACFG_DMATA_CTL1: DMA transfer address is TIMER_CTL1
       \arg        TIMER_DMACFG_DMATA_SMCFG: DMA transfer address is TIMER_SMCFG
       \arg        TIMER_DMACFG_DMATA_DMAINTEN: DMA transfer address is TIMER_DMAINTEN
       \arg        TIMER_DMACFG_DMATA_INTF: DMA transfer address is TIMER_INTF
       \arg        TIMER_DMACFG_DMATA_SWEVG: DMA transfer address is TIMER_SWEVG
       \arg        TIMER_DMACFG_DMATA_CHCTL0: DMA transfer address is TIMER_CHCTL0
       \arg        TIMER_DMACFG_DMATA_CHCTL1: DMA transfer address is TIMER_CHCTL1
       \arg        TIMER_DMACFG_DMATA_CHCTL2: DMA transfer address is TIMER_CHCTL2
       \arg        TIMER_DMACFG_DMATA_CNT: DMA transfer address is TIMER_CNT
       \arg        TIMER_DMACFG_DMATA_PSC: DMA transfer address is TIMER_PSC
       \arg        TIMER_DMACFG_DMATA_CAR: DMA transfer address is TIMER_CAR
       \arg        TIMER_DMACFG_DMATA_CREP: DMA transfer address is TIMER_CREP
       \arg        TIMER_DMACFG_DMATA_CH0CV: DMA transfer address is TIMER_CH0CV
       \arg        TIMER_DMACFG_DMATA_CH1CV: DMA transfer address is TIMER_CH1CV
       \arg        TIMER_DMACFG_DMATA_CH2CV: DMA transfer address is TIMER_CH2CV
       \arg        TIMER_DMACFG_DMATA_CH3CV: DMA transfer address is TIMER_CH3CV
       \arg        TIMER_DMACFG_DMATA_CCHP: DMA transfer address is TIMER_CCHP
       \arg        TIMER_DMACFG_DMATA_DMACFG: DMA transfer address is TIMER_DMACFG
       \arg        TIMER_DMACFG_DMATA_DMATB: DMA transfer address is TIMER_DMATB
    \param[in]  dma_lenth:
       \arg        TIMER_DMACFG_DMATC_xTRANSFER(x=1..18): DMA transfer x time
    \param[out] none
    \retval     none
*/
void timer_dma_transfer_config(uint32_t timer_periph, uint32_t dma_baseaddr, uint32_t dma_lenth)
{
    TIMER_DMACFG(timer_periph) &= (uint32_t)(~(TIMER_DMACFG_DMATA | TIMER_DMACFG_DMATC));
    TIMER_DMACFG(timer_periph) |= (uint32_t)(dma_baseaddr | dma_lenth);
}

/*!
    \brief      software generate events 
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[in]  event: the timer software event generation sources
      \arg        TIMER_EVENT_SRC_UPG: update event generation 
      \arg        TIMER_EVENT_SRC_CH0G: channel 0 capture or compare event generation 
      \arg        TIMER_EVENT_SRC_CH1G: channel 1 capture or compare event generation 
      \arg        TIMER_EVENT_SRC_CH2G: channel 2 capture or compare event generation 
      \arg        TIMER_EVENT_SRC_CH3G: channel 3 capture or compare event generation 
      \arg        TIMER_EVENT_SRC_CMTG: channel commutation event generation 
      \arg        TIMER_EVENT_SRC_TRGG: trigger event generation 
      \arg        TIMER_EVENT_SRC_BRKG:  break event generation 
    \param[out] none
    \retval     none
*/
void timer_event_software_generate(uint32_t timer_periph, uint16_t event)
{
    TIMER_SWEVG(timer_periph) |= (uint32_t)event;
}

/*!
    \brief      configure TIMER break function 
    \param[in]  timer_periph: TIMERx(x=0,14,15,16)
    \param[in]  breakpara: TIMER break parameter struct
                runoffstate: TIMER_ROS_STATE_ENABLE,TIMER_ROS_STATE_DISABLE
                ideloffstate: TIMER_IOS_STATE_ENABLE,TIMER_IOS_STATE_DISABLE
                deadtime: 0~255
                breakpolarity: TIMER_BREAK_POLARITY_LOW,TIMER_BREAK_POLARITY_HIGH
                outputautostate: TIMER_OUTAUTO_ENABLE,TIMER_OUTAUTO_DISABLE
                protectmode: TIMER_CCHP_PROT_OFF,TIMER_CCHP_PROT_0,TIMER_CCHP_PROT_1,TIMER_CCHP_PROT_2
                breakstate: TIMER_BREAK_ENABLE,TIMER_BREAK_DISABLE
    \param[out] none
    \retval     none
*/
void timer_break_config(uint32_t timer_periph, timer_break_parameter_struct* breakpara)
{
    TIMER_CCHP(timer_periph) = (uint32_t)(((uint32_t)breakpara->runoffstate)     |
                                           ((uint32_t)breakpara->ideloffstate)    |
                                           ((uint32_t)breakpara->deadtime)        |
                                           ((uint32_t)breakpara->breakpolarity)   |
                                           ((uint32_t)breakpara->outputautostate) |
                                           ((uint32_t)breakpara->protectmode)     |
                                           ((uint32_t)breakpara->breakstate));
}

/*!
    \brief      enable TIMER break function
    \param[in]  timer_periph: TIMERx(x=0,14,15,16)
    \param[out] none
    \retval     none
*/
void timer_break_enable(uint32_t timer_periph)
{
    TIMER_CCHP(timer_periph) |= (uint32_t)TIMER_BREAK_ENABLE;
}

/*!
    \brief      disable TIMER break function
    \param[in]  timer_periph: TIMERx(x=0,14,15,16)
    \param[out] none
    \retval     none
*/
void timer_break_disable(uint32_t timer_periph)
{
    TIMER_CCHP(timer_periph) &= (uint32_t)TIMER_BREAK_DISABLE;
}

/*!
    \brief      enable TIMER output automatic function
    \param[in]  timer_periph: TIMERx(x=0,14,15,16)
    \param[out] none
    \retval     none
*/
void timer_automatic_output_enable(uint32_t timer_periph)
{
    TIMER_CCHP(timer_periph) |= (uint32_t)TIMER_OUTAUTO_ENABLE;
}

/*!
    \brief      disable TIMER output automatic function
    \param[in]  timer_periph: TIMERx(x=0,14,15,16)
    \param[out] none
    \retval     none
*/
void timer_automatic_output_disable(uint32_t timer_periph)
{
    TIMER_CCHP(timer_periph) &= (uint32_t)TIMER_OUTAUTO_DISABLE;
}

/*!
    \brief      configure TIMER primary output function
    \param[in]  timer_periph: TIMERx(x=0,14,15,16)
    \param[in]  newvalue: ENABLE or DISABLE
    \param[out] none
    \retval     none
*/
void timer_primary_output_config(uint32_t timer_periph,ControlStatus newvalue)
{
    if(newvalue){
        TIMER_CCHP(timer_periph) |= (uint32_t)TIMER_CCHP_POEN;
    }else{
        TIMER_CCHP(timer_periph) &= (uint32_t)(~ TIMER_CCHP_POEN);
    }
}

/*!
    \brief      channel capture/compare control shadow register enable
    \param[in]  timer_periph: TIMERx(x=0,14,15,16)
    \param[in]  newvalue: ENABLE or DISABLE 
    \param[out] none
    \retval     none
*/
void timer_channel_control_shadow_config(uint32_t timer_periph, ControlStatus newvalue)
{
     if(newvalue){
        TIMER_CTL1(timer_periph) |= (uint32_t)TIMER_CTL1_CCSE;
    }else{
        TIMER_CTL1(timer_periph) &= (uint32_t)(~TIMER_CTL1_CCSE);
    }
}

/*!
    \brief      configure TIMER channel control shadow register update control
    \param[in]  timer_periph: TIMERx(x=0,1,2)
    \param[in]  ccuctl: channel control shadow register update control
      \arg        TIMER_UPDATECTL_CCU: the shadow registers update by when CMTG bit is set
      \arg        TIMER_UPDATECTL_CCUTRI: the shadow registers update by when CMTG bit is set or an rising edge of TRGI occurs 
    \param[out] none
    \retval     none
*/              
void timer_channel_control_shadow_update_config(uint32_t timer_periph, uint8_t ccuctl)
{
    if(TIMER_UPDATECTL_CCU == ccuctl){
        TIMER_CTL1(timer_periph) &= (~(uint32_t)TIMER_CTL1_CCUC);
    }else if(TIMER_UPDATECTL_CCUTRI == ccuctl){
        TIMER_CTL1(timer_periph) |= (uint32_t)TIMER_CTL1_CCUC;
    }else{
    }
}

/*!
    \brief      configure TIMER channel output function
    \param[in]  timer_periph: TIMERx(x=0,1,2,13,14,15,16)
    \param[in]  channel: 
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0,1,2,13,14,15,16))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0,1,2,14))
      \arg        TIMER_CH_2: TIMER channel2(TIMERx(x=0,1,2))
      \arg        TIMER_CH_3: TIMER channel3(TIMERx(x=0,1,2))
    \param[in]  ocpara: TIMER channeln output parameter struct
                outputstate: TIMER_CCX_ENABLE,TIMER_CCX_DISABLE
                outputnstate: TIMER_CCXN_ENABLE,TIMER_CCXN_DISABLE
                ocpolarity: TIMER_OC_POLARITY_HIGH,TIMER_OC_POLARITY_LOW
                ocnpolarity: TIMER_OCN_POLARITY_HIGH,TIMER_OCN_POLARITY_LOW
                ocidlestate: TIMER_OC_IDLE_STATE_LOW,TIMER_OC_IDLE_STATE_HIGH
                ocnidlestate: TIMER_OCN_IDLE_STATE_LOW,TIMER_OCN_IDLE_STATE_HIGH
    \param[out] none
    \retval     none
*/
void timer_channel_output_config(uint32_t timer_periph, uint16_t channel, timer_oc_parameter_struct* ocpara)
{
    switch(channel){
    /* configure TIMER_CH_0 */
    case TIMER_CH_0:
        /* reset the CH0EN bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH0EN);
        /* set the CH0EN bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)ocpara->outputstate;
        /* reset the CH0P bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH0P);
        /* set the CH0P bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)ocpara->ocpolarity;

        if((TIMER0 == timer_periph) || (TIMER14 == timer_periph) || (TIMER15 == timer_periph) || (TIMER16 == timer_periph)){
            /* reset the CH0NEN bit */
            TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH0NEN);
            /* set the CH0NEN bit */
            TIMER_CHCTL2(timer_periph) |= (uint32_t)ocpara->outputnstate;
            /* reset the CH0NP bit */
            TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH0NP);
            /* set the CH0NP bit */
            TIMER_CHCTL2(timer_periph) |= (uint32_t)ocpara->ocnpolarity;
            /* reset the ISO0 bit */
            TIMER_CTL1(timer_periph) &=(uint32_t)(~TIMER_CTL1_ISO0);
            /* set the ISO0 bit */
            TIMER_CTL1(timer_periph) |= (uint32_t)ocpara->ocidlestate;
            /* reset the ISO0N bit */
            TIMER_CTL1(timer_periph) &=(uint32_t)(~TIMER_CTL1_ISO0N);
            /* set the ISO0N bit */
            TIMER_CTL1(timer_periph) |= (uint32_t)ocpara->ocnidlestate;
        }
        break;
    /* configure TIMER_CH_1 */
    case TIMER_CH_1:
        /* reset the CH1EN bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH1EN);
        /* set the CH1EN bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)ocpara->outputstate << 4);
        /* reset the CH1P bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH1P);
        /* set the CH1P bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)ocpara->ocpolarity << 4);

        if((TIMER0 == timer_periph)){
            /* reset the CH1NEN bit */
            TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH1NEN);
            /* set the CH1NEN bit */
            TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)ocpara->outputnstate << 4);
            /* reset the CH1NP bit */
            TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH1NP);
            /* set the CH1NP bit */
            TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)ocpara->ocnpolarity << 4);
            /* reset the ISO1 bit */
            TIMER_CTL1(timer_periph) &=(uint32_t)(~TIMER_CTL1_ISO1);
            /* set the ISO1 bit */
            TIMER_CTL1(timer_periph) |= (uint32_t)((uint32_t)ocpara->ocidlestate << 2);
            /* reset the ISO1N bit */
            TIMER_CTL1(timer_periph) &=(uint32_t)(~TIMER_CTL1_ISO1N);
            /* set the ISO1N bit */
            TIMER_CTL1(timer_periph) |= (uint32_t)((uint32_t)ocpara->ocnidlestate << 2);
        }
        break;
    /* configure TIMER_CH_2 */
    case TIMER_CH_2:
        /* reset the CH2EN bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH2EN);
        /* set the CH2EN bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)ocpara->outputstate << 8);
        /* reset the CH2P bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH2P);
        /* set the CH2P bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)ocpara->ocpolarity << 8);

        if((TIMER0 == timer_periph)){
            /* reset the CH2NEN bit */
            TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH2NEN);
            /* set the CH2NEN bit */
            TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)ocpara->outputnstate << 8);
            /* reset the CH2NP bit */
            TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH2NP);
            /* set the CH2NP bit */
            TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)ocpara->ocnpolarity << 8);
            /* reset the ISO2 bit */
            TIMER_CTL1(timer_periph) &=(uint32_t)(~TIMER_CTL1_ISO2);
            /* set the ISO2 bit */
            TIMER_CTL1(timer_periph) |= (uint32_t)((uint32_t)ocpara->ocidlestate << 4);
            /* reset the ISO2N bit */
            TIMER_CTL1(timer_periph) &=(uint32_t)(~TIMER_CTL1_ISO2N);
            /* set the ISO2N bit */
            TIMER_CTL1(timer_periph) |= (uint32_t)((uint32_t)ocpara->ocnidlestate << 4);
        }
        break;
    /* configure TIMER_CH_3 */
    case TIMER_CH_3:
        /* reset the CH3EN bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH3EN);
        /* set the CH3EN bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)ocpara->outputstate << 12);
        /* reset the CH3P bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH3P);
        /* set the CH3P bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)ocpara->ocpolarity << 12);

        if((TIMER0 == timer_periph)){
            /* reset the ISO3 bit */
            TIMER_CTL1(timer_periph) &=(uint32_t)(~TIMER_CTL1_ISO3);
            /* set the ISO3 bit */
            TIMER_CTL1(timer_periph) |= (uint32_t)((uint32_t)ocpara->ocidlestate << 6);
        }
        break;
    default:
        break;
    }
}

/*!
    \brief      configure TIMER channel output compare mode
    \param[in]  timer_periph: TIMERx(x=0,1,2,13,14,15,16)
    \param[in]  channel: 
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0,1,2,13,14,15,16))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0,1,2,14))
      \arg        TIMER_CH_2: TIMER channel2(TIMERx(x=0,1,2))
      \arg        TIMER_CH_3: TIMER channel3(TIMERx(x=0,1,2))
    \param[in]  ocmode: channel output compare mode
      \arg        TIMER_OC_MODE_TIMING: timing mode
      \arg        TIMER_OC_MODE_ACTIVE: active mode
      \arg        TIMER_OC_MODE_INACTIVE: inactive mode
      \arg        TIMER_OC_MODE_TOGGLE: toggle mode
      \arg        TIMER_OC_MODE_LOW: force low mode
      \arg        TIMER_OC_MODE_HIGH: force high mode
      \arg        TIMER_OC_MODE_PWM0: PWM0 mode
      \arg        TIMER_OC_MODE_PWM1: PWM1 mode
    \param[out] none
    \retval     none
*/
void timer_channel_output_mode_config(uint32_t timer_periph, uint16_t channel, uint16_t ocmode)
{
    switch(channel){
    /* configure TIMER_CH_0 */
    case TIMER_CH_0:
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH0COMCTL);
        TIMER_CHCTL0(timer_periph) |= (uint32_t)ocmode;
        break;
    /* configure TIMER_CH_1 */
    case TIMER_CH_1:
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH1COMCTL);
        TIMER_CHCTL0(timer_periph) |= (uint32_t)((uint32_t)ocmode << 8);
        break;
    /* configure TIMER_CH_2 */
    case TIMER_CH_2:
        TIMER_CHCTL1(timer_periph) &=(uint32_t)(~TIMER_CHCTL1_CH2COMCTL);
        TIMER_CHCTL1(timer_periph) |= (uint32_t)ocmode;
        break;
    /* configure TIMER_CH_3 */
    case TIMER_CH_3:
        TIMER_CHCTL1(timer_periph) &=(uint32_t)(~TIMER_CHCTL1_CH3COMCTL);
        TIMER_CHCTL1(timer_periph) |= (uint32_t)((uint32_t)ocmode << 8);
        break;
    default:
        break;
    }
}

/*!
    \brief      configure TIMER channel output pulse value
    \param[in]  timer_periph: TIMERx(x=0,1,2,13,14,15,16)
    \param[in]  channel: 
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0,1,2,13,14,15,16))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0,1,2,14))
      \arg        TIMER_CH_2: TIMER channel2(TIMERx(x=0,1,2))
      \arg        TIMER_CH_3: TIMER channel3(TIMERx(x=0,1,2))
    \param[in]  pulse: channel output pulse value
    \param[out] none
    \retval     none
*/
void timer_channel_output_pulse_value_config(uint32_t timer_periph, uint16_t channel, uint32_t pulse)
{
    switch(channel){
    /* configure TIMER_CH_0 */
    case TIMER_CH_0:
        TIMER_CH0CV(timer_periph) = (uint32_t)pulse;
        break;
    /* configure TIMER_CH_1 */
    case TIMER_CH_1:
        TIMER_CH1CV(timer_periph) = (uint32_t)pulse;
        break;
    /* configure TIMER_CH_2 */
    case TIMER_CH_2:
        TIMER_CH2CV(timer_periph) = (uint32_t)pulse;
        break;
    /* configure TIMER_CH_3 */
    case TIMER_CH_3:
         TIMER_CH3CV(timer_periph) = (uint32_t)pulse;
        break;
    default:
        break;
    }
}

/*!
    \brief      configure TIMER channel output shadow function
    \param[in]  timer_periph: TIMERx(x=0,1,2,13,14,15,16)
    \param[in]  channel: 
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0,1,2,13,14,15,16))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0,1,2,14))
      \arg        TIMER_CH_2: TIMER channel2(TIMERx(x=0,1,2))
      \arg        TIMER_CH_3: TIMER channel3(TIMERx(x=0,1,2))
    \param[in]  ocshadow: channel output shadow state
      \arg        TIMER_OC_SHADOW_ENABLE: channel output shadow state enable
      \arg        TIMER_OC_SHADOW_DISABLE: channel output shadow state disable
    \param[out] none
    \retval     none
*/
void timer_channel_output_shadow_config(uint32_t timer_periph, uint16_t channel, uint16_t ocshadow)
{
    switch(channel){
    /* configure TIMER_CH_0 */
    case TIMER_CH_0:
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH0COMSEN);
        TIMER_CHCTL0(timer_periph) |= (uint32_t)ocshadow;
        break;
    /* configure TIMER_CH_1 */
    case TIMER_CH_1:
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH1COMSEN);
        TIMER_CHCTL0(timer_periph) |= (uint32_t)((uint32_t)ocshadow << 8);
        break;
    /* configure TIMER_CH_2 */
    case TIMER_CH_2:
        TIMER_CHCTL1(timer_periph) &=(uint32_t)(~TIMER_CHCTL1_CH2COMSEN);
        TIMER_CHCTL1(timer_periph) |= (uint32_t)ocshadow;
        break;
    /* configure TIMER_CH_3 */
    case TIMER_CH_3:
        TIMER_CHCTL1(timer_periph) &=(uint32_t)(~TIMER_CHCTL1_CH3COMSEN);
        TIMER_CHCTL1(timer_periph) |= (uint32_t)((uint32_t)ocshadow << 8);
        break;
    default:
        break;
    }
}

/*!
    \brief      configure TIMER channel output fast function
    \param[in]  timer_periph: TIMERx(x=0,1,2,13,14,15,16)
    \param[in]  channel: 
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0,1,2,13,14,15,16))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0,1,2,14))
      \arg        TIMER_CH_2: TIMER channel2(TIMERx(x=0,1,2))
      \arg        TIMER_CH_3: TIMER channel3(TIMERx(x=0,1,2))
    \param[in]  ocfast: channel output fast function
      \arg        TIMER_OC_FAST_ENABLE: channel output fast function enable
      \arg        TIMER_OC_FAST_DISABLE: channel output fast function disable
    \param[out] none
    \retval     none
*/
void timer_channel_output_fast_config(uint32_t timer_periph, uint16_t channel, uint16_t ocfast)
{
    switch(channel){
    /* configure TIMER_CH_0 */
    case TIMER_CH_0:
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH0COMFEN);
        TIMER_CHCTL0(timer_periph) |= (uint32_t)ocfast;
        break;
    /* configure TIMER_CH_1 */
    case TIMER_CH_1:
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH1COMFEN);
        TIMER_CHCTL0(timer_periph) |= (uint32_t)((uint32_t)ocfast << 8);
        break;
    /* configure TIMER_CH_2 */
    case TIMER_CH_2:
        TIMER_CHCTL1(timer_periph) &=(uint32_t)(~TIMER_CHCTL1_CH2COMFEN);
        TIMER_CHCTL1(timer_periph) |= (uint32_t)ocfast;
        break;
    /* configure TIMER_CH_3 */
    case TIMER_CH_3:
        TIMER_CHCTL1(timer_periph) &=(uint32_t)(~TIMER_CHCTL1_CH3COMFEN);
        TIMER_CHCTL1(timer_periph) |= (uint32_t)((uint32_t)ocfast << 8);
        break;
    default:
        break;
    }
}

/*!
    \brief      configure TIMER channel output clear function
    \param[in]  timer_periph: TIMERx(x=0,1,2,13,14,15,16)
    \param[in]  channel: 
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0,1,2,13,14,15,16))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0,1,2,14))
      \arg        TIMER_CH_2: TIMER channel2(TIMERx(x=0,1,2))
      \arg        TIMER_CH_3: TIMER channel3(TIMERx(x=0,1,2))
    \param[in]  occlear: channel output clear function
      \arg        TIMER_OC_CLEAR_ENABLE: channel output clear function enable
      \arg        TIMER_OC_CLEAR_DISABLE: channel output clear function disable
    \param[out] none
    \retval     none
*/
void timer_channel_output_clear_config(uint32_t timer_periph, uint16_t channel, uint16_t occlear)
{
    switch(channel){
    /* configure TIMER_CH_0 */
    case TIMER_CH_0:
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH0COMCEN);
        TIMER_CHCTL0(timer_periph) |= (uint32_t)occlear;
        break;
    /* configure TIMER_CH_1 */
    case TIMER_CH_1:
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH1COMCEN);
        TIMER_CHCTL0(timer_periph) |= (uint32_t)((uint32_t)occlear << 8);
        break;
    /* configure TIMER_CH_2 */
    case TIMER_CH_2:
        TIMER_CHCTL1(timer_periph) &=(uint32_t)(~TIMER_CHCTL1_CH2COMCEN);
        TIMER_CHCTL1(timer_periph) |= (uint32_t)occlear;
        break;
    /* configure TIMER_CH_3 */
    case TIMER_CH_3:
        TIMER_CHCTL1(timer_periph) &=(uint32_t)(~TIMER_CHCTL1_CH3COMCEN);
        TIMER_CHCTL1(timer_periph) |= (uint32_t)((uint32_t)occlear << 8);
        break;
    default:
        break;
    }
}

/*!
    \brief      configure TIMER channel output polarity 
    \param[in]  timer_periph: TIMERx(x=0,1,2,13,14,15,16)
    \param[in]  channel: 
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0,1,2,13,14,15,16))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0,1,2,14))
      \arg        TIMER_CH_2: TIMER channel2(TIMERx(x=0,1,2))
      \arg        TIMER_CH_3: TIMER channel3(TIMERx(x=0,1,2))
    \param[in]  ocpolarity: channel output polarity 
      \arg        TIMER_OC_POLARITY_HIGH: channel output polarity is high
      \arg        TIMER_OC_POLARITY_LOW: channel output polarity is low
    \param[out] none
    \retval     none
*/
void timer_channel_output_polarity_config(uint32_t timer_periph, uint16_t channel, uint16_t ocpolarity)
{
    switch(channel){
    /* configure TIMER_CH_0 */
    case TIMER_CH_0:
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH0P);
        TIMER_CHCTL2(timer_periph) |= (uint32_t)ocpolarity;
        break;
    /* configure TIMER_CH_1 */
    case TIMER_CH_1:
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH1P);
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)ocpolarity << 4);
        break;
    /* configure TIMER_CH_2 */
    case TIMER_CH_2:
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH2P);
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)ocpolarity << 8);
        break;
    /* configure TIMER_CH_3 */
    case TIMER_CH_3:
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH3P);
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)ocpolarity << 12);
        break;
    default:
        break;
    }
}

/*!
    \brief      configure TIMER channel complementary output polarity 
    \param[in]  timer_periph: TIMERx(x=0,1,2,13,14,15,16)
    \param[in]  channel: 
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0,1,2,13,14,15,16))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0,1,2,14))
      \arg        TIMER_CH_2: TIMER channel2(TIMERx(x=0,1,2))
    \param[in]  ocnpolarity: channel complementary output polarity 
      \arg        TIMER_OCN_POLARITY_HIGH: channel complementary output polarity is high
      \arg        TIMER_OCN_POLARITY_LOW: channel complementary output polarity is low
    \param[out] none
    \retval     none
*/
void timer_channel_complementary_output_polarity_config(uint32_t timer_periph, uint16_t channel, uint16_t ocnpolarity)
{
    switch(channel){
    /* configure TIMER_CH_0 */
    case TIMER_CH_0:
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH0NP);
        TIMER_CHCTL2(timer_periph) |= (uint32_t)ocnpolarity;
        break;
    /* configure TIMER_CH_1 */
    case TIMER_CH_1:
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH1NP);
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)ocnpolarity << 4);
        break;
    /* configure TIMER_CH_2 */
    case TIMER_CH_2:
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH2NP);
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)ocnpolarity << 8);
        break;
    default:
        break;
    }
}

/*!
    \brief      configure TIMER OCPRE clear source selection
    \param[in]  timer_periph: TIMERx(x=0,1,2)
    \param[in]  clear: OCPRE clear source
      \arg        TIMER_OCPRE_CLEAR_SOURCE_ETIF: OCPRE_CLR_INT is connected to ETIF
      \arg        TIMER_OCPRE_CLEAR_SOURCE_CLR: OCPRE_CLR_INT is connected to the OCPRE_CLR input
    \param[out] none
    \retval     none
*/
void timer_ocpre_clear_source_config(uint32_t timer_periph, uint8_t ocpreclear)
{
    if(TIMER_OCPRE_CLEAR_SOURCE_ETIF == ocpreclear){
        TIMER_SMCFG(timer_periph) |= (uint32_t)TIMER_SMCFG_OCRC;
    }else if(TIMER_OCPRE_CLEAR_SOURCE_CLR == ocpreclear){
        TIMER_SMCFG(timer_periph) &= ~(uint32_t)TIMER_SMCFG_OCRC;
    }else{
    }
}

/*!
    \brief      configure TIMER channel enable state
    \param[in]  timer_periph: TIMERx(x=0,1,2,13,14,15,16)
    \param[in]  channel: 
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0,1,2,13,14,15,16))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0,1,2,14))
      \arg        TIMER_CH_2: TIMER channel2(TIMERx(x=0,1,2))
      \arg        TIMER_CH_3: TIMER channel3(TIMERx(x=0,1,2))
    \param[in]  state: TIMER channel enable state
      \arg        TIMER_CCX_ENABLE: channel enable 
      \arg        TIMER_CCX_DISABLE: channel disable 
    \param[out] none
    \retval     none
*/
void timer_channel_output_state_config(uint32_t timer_periph, uint16_t channel, uint16_t state)
{
    switch(channel){
    /* configure TIMER_CH_0 */
    case TIMER_CH_0:
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH0EN);
        TIMER_CHCTL2(timer_periph) |= (uint32_t)state;
        break;
    /* configure TIMER_CH_1 */
    case TIMER_CH_1:
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH1EN);
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)state << 4);
        break;
    /* configure TIMER_CH_2 */
    case TIMER_CH_2:
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH2EN);
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)state << 8);
        break;
    /* configure TIMER_CH_3 */
    case TIMER_CH_3:
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH3EN);
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)state << 12);
        break;
    default:
        break;
    }
}

/*!
    \brief      configure TIMER channel complementary output enable state
    \param[in]  timer_periph: TIMERx(x=0,1,2,13,14,15,16)
    \param[in]  channel: 
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0,1,2,13,14,15,16))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0,1,2,14))
      \arg        TIMER_CH_2: TIMER channel2(TIMERx(x=0,1,2))
    \param[in]  ocnstate: TIMER channel complementary output enable state
      \arg        TIMER_CCXN_ENABLE: channel complementary enable 
      \arg        TIMER_CCXN_DISABLE: channel complementary disable 
    \param[out] none
    \retval     none
*/
void timer_channel_complementary_output_state_config(uint32_t timer_periph, uint16_t channel, uint16_t ocnstate)
{
    switch(channel){
    /* configure TIMER_CH_0 */
    case TIMER_CH_0:
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH0NEN);
        TIMER_CHCTL2(timer_periph) |= (uint32_t)ocnstate;
        break;
    /* configure TIMER_CH_1 */
    case TIMER_CH_1:
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH1NEN);
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)ocnstate << 4);
        break;
    /* configure TIMER_CH_2 */
    case TIMER_CH_2:
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH2NEN);
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)ocnstate << 8);
        break;
    default:
        break;
    }
}

/*!
    \brief      configure TIMER input capture parameter 
    \param[in]  timer_periph: TIMERx(x=0,1,2,5,13,14,15,16)
    \param[in]  channel: 
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0,1,2,13,14,15,16))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0,1,2,14))
      \arg        TIMER_CH_2: TIMER channel2(TIMERx(x=0,1,2))
      \arg        TIMER_CH_3: TIMER channel3(TIMERx(x=0,1,2))
     \param[in]  icpara: TIMER channel intput parameter struct
                 icpolarity: TIMER_IC_POLARITY_RISING,TIMER_IC_POLARITY_FALLING,TIMER_IC_POLARITY_BOTH_EDGE
                 icselection: TIMER_IC_SELECTION_DIRECTTI,TIMER_IC_SELECTION_INDIRECTTI,TIMER_IC_SELECTION_ITS
                 icprescaler: TIMER_IC_PSC_DIV1,TIMER_IC_PSC_DIV2,TIMER_IC_PSC_DIV4,TIMER_IC_PSC_DIV8
                 icfilter: 0~15
    \param[out]  none
    \retval      none
*/
void timer_input_capture_config(uint32_t timer_periph,uint16_t channel,timer_ic_parameter_struct* icpara)
{
    switch(channel){
    /* configure TIMER_CH_0 */
    case TIMER_CH_0:
        /* reset the CH0EN bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH0EN);

        /* reset the CH0P and CH0NP bits */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~(TIMER_CHCTL2_CH0P|TIMER_CHCTL2_CH0NP));
        TIMER_CHCTL2(timer_periph) |= (uint32_t)(icpara->icpolarity);
        /* reset the CH0MS bit */
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH0MS);
        TIMER_CHCTL0(timer_periph) |= (uint32_t)(icpara->icselection);
        /* reset the CH0CAPFLT bit */
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH0CAPFLT);
        TIMER_CHCTL0(timer_periph) |= (uint32_t)(((uint32_t)icpara->icfilter << 4));

        /* set the CH0EN bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)TIMER_CHCTL2_CH0EN;
        break;
    /* configure TIMER_CH_1 */
    case TIMER_CH_1:
        /* reset the CH1EN bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH1EN);

        /* reset the CH1P and CH1NP bits */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~(TIMER_CHCTL2_CH1P|TIMER_CHCTL2_CH1NP));
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)icpara->icpolarity << 4);
        /* reset the CH1MS bit */
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH1MS);
        TIMER_CHCTL0(timer_periph) |= (uint32_t)((uint32_t)icpara->icselection << 8);
        /* reset the CH1CAPFLT bit */
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH1CAPFLT);
        TIMER_CHCTL0(timer_periph) |= (uint32_t)((uint32_t)icpara->icfilter << 12);

        /* set the CH1EN bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)TIMER_CHCTL2_CH1EN;
        break;
    /* configure TIMER_CH_2 */
    case TIMER_CH_2:
        /* reset the CH2EN bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH2EN);

        /* reset the CH2P and CH2NP bits */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~(TIMER_CHCTL2_CH2P|TIMER_CHCTL2_CH2NP));
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)icpara->icpolarity << 8);
        /* reset the CH2MS bit */
        TIMER_CHCTL1(timer_periph) &=(uint32_t)(~TIMER_CHCTL1_CH2MS);
        TIMER_CHCTL1(timer_periph) |= (uint32_t)(icpara->icselection);
        /* reset the CH2CAPFLT bit */
        TIMER_CHCTL1(timer_periph) &=(uint32_t)(~TIMER_CHCTL1_CH2CAPFLT);
        TIMER_CHCTL1(timer_periph) |= (uint32_t)((uint32_t)icpara->icfilter<< 4);

        /* set the CH2EN bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)TIMER_CHCTL2_CH2EN;
        break;
    /* configure TIMER_CH_3 */
    case TIMER_CH_3:
        /* reset the CH3EN bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH3EN);

        /* reset the CH3P bits */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~(TIMER_CHCTL2_CH3P));
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)icpara->icpolarity << 12);
        /* reset the CH3MS bit */
        TIMER_CHCTL1(timer_periph) &=(uint32_t)(~TIMER_CHCTL1_CH3MS);
        TIMER_CHCTL1(timer_periph) |= (uint32_t)((uint32_t)icpara->icselection << 8);
        /* reset the CH3CAPFLT bit */
        TIMER_CHCTL1(timer_periph) &=(uint32_t)(~TIMER_CHCTL1_CH3CAPFLT);
        TIMER_CHCTL1(timer_periph) |= (uint32_t)((uint32_t)icpara->icfilter << 12);

        /* set the CH3EN bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)TIMER_CHCTL2_CH3EN;
        break;
    default:
        break;
    }
    /* configure TIMER channel input capture prescaler value */
    timer_channel_input_capture_prescaler_config(timer_periph, channel,(uint32_t)icpara->icprescaler);
}

/*!
    \brief      configure TIMER channel input capture prescaler value
    \param[in]  timer_periph: TIMERx(x=0,1,2,13,14,15,16)
    \param[in]  channel: 
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0,1,2,13,14,15,16))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0,1,2,14))
      \arg        TIMER_CH_2: TIMER channel2(TIMERx(x=0,1,2))
      \arg        TIMER_CH_3: TIMER channel3(TIMERx(x=0,1,2))
    \param[in]  prescaler: channel input capture prescaler value
      \arg        TIMER_IC_PSC_DIV1: no prescaler
      \arg        TIMER_IC_PSC_DIV2: divided by 2
      \arg        TIMER_IC_PSC_DIV4: divided by 4
      \arg        TIMER_IC_PSC_DIV8: divided by 8
    \param[out] none
    \retval     none
*/
void timer_channel_input_capture_prescaler_config(uint32_t timer_periph, uint16_t channel, uint32_t prescaler)
{
    switch(channel){
    /* configure TIMER_CH_0 */
    case TIMER_CH_0:
        TIMER_CHCTL0(timer_periph) &= (uint32_t)(~ TIMER_CHCTL0_CH0CAPPSC);
        TIMER_CHCTL0(timer_periph) |= (uint32_t)prescaler;
        break;
    /* configure TIMER_CH_1 */
    case TIMER_CH_1:
        TIMER_CHCTL0(timer_periph) &= (uint32_t)(~TIMER_CHCTL0_CH1CAPPSC);
        TIMER_CHCTL0(timer_periph) |= (uint32_t)(prescaler << 8);
        break;
    /* configure TIMER_CH_2 */
    case TIMER_CH_2:
        TIMER_CHCTL1(timer_periph) &= (uint32_t)(~TIMER_CHCTL1_CH2CAPPSC);
        TIMER_CHCTL1(timer_periph) |= (uint32_t)prescaler;
        break;
    /* configure TIMER_CH_3 */
    case TIMER_CH_3:
        TIMER_CHCTL1(timer_periph) &= (uint32_t)(~TIMER_CHCTL1_CH3CAPPSC);
        TIMER_CHCTL1(timer_periph) |= (uint32_t)(prescaler << 8);
        break;
    default:
        break;
    }
}

/*!
    \brief      read TIMER channel capture compare register value
    \param[in]  timer_periph: TIMERx(x=0,1,2,13,14,15,16)
    \param[in]  channel: 
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0,1,2,13,14,15,16))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0,1,2,14))
      \arg        TIMER_CH_2: TIMER channel2(TIMERx(x=0,1,2))
      \arg        TIMER_CH_3: TIMER channel3(TIMERx(x=0,1,2))
    \param[out] none
    \retval     channel capture compare register value
*/
uint32_t timer_channel_capture_value_register_read(uint32_t timer_periph, uint16_t channel)
{
    uint32_t count_value=0U;

    switch(channel){
    case TIMER_CH_0:
        count_value = TIMER_CH0CV(timer_periph);
        break;
    case TIMER_CH_1:
        count_value = TIMER_CH1CV(timer_periph);
        break;
    case TIMER_CH_2:
        count_value = TIMER_CH2CV(timer_periph);
        break;
    case TIMER_CH_3:
        count_value = TIMER_CH3CV(timer_periph);
        break;
    default:
        break;
    }
    return (count_value);
}

/*!
    \brief      configure TIMER input pwm capture function 
    \param[in]  timer_periph: TIMERx(x=0,1,2,14)
    \param[in]  channel: 
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=0,1,2,14))
      \arg        TIMER_CH_1: TIMER channel1(TIMERx(x=0,1,2,14))
     \param[in] icpwm:TIMER channel intput pwm parameter struct
                 icpolarity: TIMER_IC_POLARITY_RISING,TIMER_IC_POLARITY_FALLING
                 icselection: TIMER_IC_SELECTION_DIRECTTI,TIMER_IC_SELECTION_INDIRECTTI
                 icprescaler: TIMER_IC_PSC_DIV1,TIMER_IC_PSC_DIV2,TIMER_IC_PSC_DIV4,TIMER_IC_PSC_DIV8
                 icfilter: 0~15
    \param[out] none
    \retval     none
*/
void timer_input_pwm_capture_config(uint32_t timer_periph, uint16_t channel, timer_ic_parameter_struct* icpwm)
{
    uint16_t icpolarity  = 0x0U;
    uint16_t icselection = 0x0U;

    if(TIMER_IC_POLARITY_RISING == icpwm->icpolarity){
        icpolarity = TIMER_IC_POLARITY_FALLING;
    }else{
        icpolarity = TIMER_IC_POLARITY_RISING;
    }

    if(TIMER_IC_SELECTION_DIRECTTI == icpwm->icselection){
        icselection = TIMER_IC_SELECTION_INDIRECTTI;
    }else{
        icselection = TIMER_IC_SELECTION_DIRECTTI;
    }

    if(TIMER_CH_0 == channel){
        /* reset the CH0EN bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH0EN);
        /* reset the CH0P and CH0NP bits */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~(TIMER_CHCTL2_CH0P|TIMER_CHCTL2_CH0NP));
        /* set the CH0P and CH0NP bits */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)(icpwm->icpolarity);
        /* reset the CH0MS bit */
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH0MS);
        /* set the CH0MS bit */
        TIMER_CHCTL0(timer_periph) |= (uint32_t)(icpwm->icselection);
        /* reset the CH0CAPFLT bit */
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH0CAPFLT);
        /* set the CH0CAPFLT bit */
        TIMER_CHCTL0(timer_periph) |= (uint32_t)(icpwm->icfilter);
        /* set the CH0EN bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)TIMER_CHCTL2_CH0EN;
        /* configure TIMER channel input capture prescaler value */
        timer_channel_input_capture_prescaler_config(timer_periph,TIMER_CH_0,(uint32_t)icpwm->icprescaler);

        /* reset the CH1EN bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH1EN);
        /* reset the CH1P and CH1NP bits */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~(TIMER_CHCTL2_CH1P|TIMER_CHCTL2_CH1NP));
        /* set the CH1P and CH1NP bits */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)icpolarity << 4);
        /* reset the CH1MS bit */
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH1MS);
        /* set the CH1MS bit */
        TIMER_CHCTL0(timer_periph) |= (uint32_t)((uint32_t)icselection << 8);
        /* reset the CH1CAPFLT bit */
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH1CAPFLT);
        /* set the CH1CAPFLT bit */
        TIMER_CHCTL0(timer_periph) |= (uint32_t)((uint32_t)icpwm->icfilter << 8);
        /* set the CH1EN bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)TIMER_CHCTL2_CH1EN;
        /* configure TIMER channel input capture prescaler value */
        timer_channel_input_capture_prescaler_config(timer_periph,TIMER_CH_1,(uint32_t)icpwm->icprescaler);
    }else{
        /* reset the CH1EN bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH1EN);
        /* reset the CH1P and CH1NP bits */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~(TIMER_CHCTL2_CH1P|TIMER_CHCTL2_CH1NP));
        /* set the CH1P and CH1NP bits */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)icpwm->icpolarity << 4);
        /* reset the CH1MS bit */
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH1MS);
        /* set the CH1MS bit */
        TIMER_CHCTL0(timer_periph) |= (uint32_t)((uint32_t)icpwm->icselection << 8);
        /* reset the CH1CAPFLT bit */
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH1CAPFLT);
        /* set the CH1CAPFLT bit */
        TIMER_CHCTL0(timer_periph) |= (uint32_t)((uint32_t)icpwm->icfilter << 8);
        /* set the CH1EN bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)TIMER_CHCTL2_CH1EN;
        /* configure TIMER channel input capture prescaler value */
        timer_channel_input_capture_prescaler_config(timer_periph,TIMER_CH_1,(uint32_t)icpwm->icprescaler);

        /* reset the CH0EN bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH0EN);
        /* reset the CH0P and CH0NP bits */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~(TIMER_CHCTL2_CH0P|TIMER_CHCTL2_CH0NP));
        /* set the CH0P and CH0NP bits */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)icpolarity;
        /* reset the CH0MS bit */
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH0MS);
        /* set the CH0MS bit */
        TIMER_CHCTL0(timer_periph) |= (uint32_t)icselection;
        /* reset the CH0CAPFLT bit */
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH0CAPFLT);
        /* set the CH0CAPFLT bit */
        TIMER_CHCTL0(timer_periph) |= (uint32_t)icpwm->icfilter;
        /* set the CH0EN bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)TIMER_CHCTL2_CH0EN;
        /* configure TIMER channel input capture prescaler value */
        timer_channel_input_capture_prescaler_config(timer_periph,TIMER_CH_0,(uint32_t)icpwm->icprescaler);
    }
}

/*!
    \brief      configure TIMER hall sensor mode
    \param[in]  timer_periph: TIMERx(x=0,1,2,14)
    \param[in]  hallmode: 
      \arg        TIMER_HALLINTERFACE_ENABLE: TIMER hall sensor mode enable
      \arg        TIMER_HALLINTERFACE_DISABLE: TIMER hall sensor mode disable
    \param[out] none
    \retval     none
*/
void timer_hall_mode_config(uint32_t timer_periph, uint8_t hallmode)
{
   if(TIMER_HALLINTERFACE_ENABLE == hallmode){
        TIMER_CTL1(timer_periph) |= (uint32_t)TIMER_CTL1_TI0S;
    }else if(TIMER_HALLINTERFACE_DISABLE == hallmode){
        TIMER_CTL1(timer_periph) &= ~(uint32_t)TIMER_CTL1_TI0S;
    }else{
    }
}

/*!
    \brief      select TIMER input trigger source 
    \param[in]  timer_periph: TIMERx(x=0,1,2,14)
    \param[in]  intrigger: 
      \arg        TIMER_SMCFG_TRGSEL_ITI0: internal trigger 0
      \arg        TIMER_SMCFG_TRGSEL_ITI1: internal trigger 1
      \arg        TIMER_SMCFG_TRGSEL_ITI2: internal trigger 2
      \arg        TIMER_SMCFG_TRGSEL_ITI3: internal trigger 3
      \arg        TIMER_SMCFG_TRGSEL_CI0F_ED: TI0 edge detector
      \arg        TIMER_SMCFG_TRGSEL_CI0FE0: filtered TIMER input 0
      \arg        TIMER_SMCFG_TRGSEL_CI1FE1: filtered TIMER input 1
      \arg        TIMER_SMCFG_TRGSEL_ETIFP: external trigger
    \param[out] none
    \retval     none
*/
void timer_input_trigger_source_select(uint32_t timer_periph,uint32_t intrigger)
{
    TIMER_SMCFG(timer_periph) &=(uint32_t)(~TIMER_SMCFG_TRGS);
    TIMER_SMCFG(timer_periph) |= (uint32_t)intrigger;
}

/*!
    \brief      select TIMER master mode output trigger source 
    \param[in]  timer_periph: TIMERx(x=0,1,2,14)
    \param[in]  outrigger: 
      \arg        TIMER_TRI_OUT_SRC_RESET: the UPG bit as trigger output
      \arg        TIMER_TRI_OUT_SRC_ENABLE: the counter enable signal TIMER_CTL0_CEN as trigger output
      \arg        TIMER_TRI_OUT_SRC_UPDATE: update event as trigger output
      \arg        TIMER_TRI_OUT_SRC_CH0: a capture or a compare match occurred in channal0 as trigger output TRGO
      \arg        TIMER_TRI_OUT_SRC_O0CPRE: O0CPRE as trigger output
      \arg        TIMER_TRI_OUT_SRC_O1CPRE: O1CPRE as trigger output
      \arg        TIMER_TRI_OUT_SRC_O2CPRE: O2CPRE as trigger output
      \arg        TIMER_TRI_OUT_SRC_O3CPRE: O3CPRE as trigger output
    \param[out] none
    \retval     none
*/
void timer_master_output_trigger_source_select(uint32_t timer_periph, uint32_t outrigger)
{
    TIMER_CTL1(timer_periph) &=(uint32_t)(~TIMER_CTL1_MMC);
    TIMER_CTL1(timer_periph) |= (uint32_t)outrigger;
}

/*!
    \brief      select TIMER slave mode 
    \param[in]  timer_periph: TIMERx(x=0,1,2,14)
    \param[in]  slavemode: 
      \arg        TIMER_SLAVE_MODE_DISABLE: slave mode disable
      \arg        TIMER_ENCODER_MODE0: encoder mode 0
      \arg        TIMER_ENCODER_MODE1: encoder mode 1
      \arg        TIMER_ENCODER_MODE2: encoder mode 2
      \arg        TIMER_SLAVE_MODE_RESTART: restart mode
      \arg        TIMER_SLAVE_MODE_PAUSE: pause mode
      \arg        TIMER_SLAVE_MODE_EVENT: event mode
      \arg        TIMER_SLAVE_MODE_EXTERNAL0: external clock mode 0.
    \param[out] none
    \retval     none
*/

void timer_slave_mode_select(uint32_t timer_periph,uint32_t slavemode)
{
    TIMER_SMCFG(timer_periph) &=(uint32_t)(~TIMER_SMCFG_SMC);

    TIMER_SMCFG(timer_periph) |= (uint32_t)slavemode;
}

/*!
    \brief      configure TIMER master slave mode 
    \param[in]  timer_periph: TIMERx(x=0,1,2,14)
    \param[in]  masterslave: 
      \arg        TIMER_MASTER_SLAVE_MODE_ENABLE: master slave mode enable
      \arg        TIMER_MASTER_SLAVE_MODE_DISABLE: master slave mode disable
    \param[out] none
    \retval     none
*/ 
void timer_master_slave_mode_config(uint32_t timer_periph, uint8_t masterslave)
{
    if(TIMER_MASTER_SLAVE_MODE_ENABLE == masterslave){
        TIMER_SMCFG(timer_periph) |= (uint32_t)TIMER_SMCFG_MSM;
    }else if(TIMER_MASTER_SLAVE_MODE_DISABLE == masterslave){
        TIMER_SMCFG(timer_periph) &= ~(uint32_t)TIMER_SMCFG_MSM;
    }else{
    }
}

/*!
    \brief      configure TIMER external trigger input
    \param[in]  timer_periph: TIMERx(x=0,1,2,14)
    \param[in]  extprescaler: 
      \arg        TIMER_EXT_TRI_PSC_OFF: no divided
      \arg        TIMER_EXT_TRI_PSC_DIV2: divided by 2
      \arg        TIMER_EXT_TRI_PSC_DIV4: divided by 4
      \arg        TIMER_EXT_TRI_PSC_DIV8: divided by 8
    \param[in]  expolarity: 
      \arg        TIMER_ETP_FALLING: active low or falling edge active
      \arg        TIMER_ETP_RISING: active high or rising edge active
    \param[in]  extfilter: a value between 0x00 and 0x0F
    \param[out] none
    \retval     none
*/
void timer_external_trigger_config(uint32_t timer_periph, uint32_t extprescaler, uint32_t expolarity, uint32_t extfilter)
{
    TIMER_SMCFG(timer_periph) &=(uint32_t)(~(TIMER_SMCFG_ETP|TIMER_SMCFG_ETPSC|TIMER_SMCFG_ETFC));
    TIMER_SMCFG(timer_periph) |= (uint32_t)(extprescaler|expolarity);
    TIMER_SMCFG(timer_periph) |= (uint32_t)(extfilter << (uint16_t)8);
}

/*!
    \brief      configure TIMER quadrature decoder mode
    \param[in]  timer_periph: TIMERx(x=0,1,2,14)
    \param[in]  decomode: 
      \arg        TIMER_ENCODER_MODE0: counter counts on CI0FE0 edge depending on CI1FE1 level
      \arg        TIMER_ENCODER_MODE1: counter counts on CI1FE1 edge depending on CI0FE0 level
      \arg        TIMER_ENCODER_MODE2: counter counts on both CI0FE0 and CI1FE1 edges depending on the level of the other input
    \param[in]  ic0polarity: 
      \arg        TIMER_IC_POLARITY_RISING: capture rising edge
      \arg        TIMER_IC_POLARITY_FALLING: capture falling edge
    \param[in]  ic1polarity: 
      \arg        TIMER_IC_POLARITY_RISING: capture rising edge
      \arg        TIMER_IC_POLARITY_FALLING: capture falling edge
    \param[out] none
    \retval     none
*/
void timer_quadrature_decoder_mode_config(uint32_t timer_periph, uint32_t decomode, uint16_t ic0polarity, uint16_t ic1polarity)
{
    TIMER_SMCFG(timer_periph) &=(uint32_t)(~TIMER_SMCFG_SMC);
    TIMER_SMCFG(timer_periph) |= (uint32_t)decomode;

    TIMER_CHCTL0(timer_periph) &= (uint32_t)(((uint32_t)(~TIMER_CHCTL0_CH0MS))&((uint32_t)(~TIMER_CHCTL0_CH1MS)));
    TIMER_CHCTL0(timer_periph) |= (uint32_t)(TIMER_IC_SELECTION_DIRECTTI|((uint32_t)TIMER_IC_SELECTION_DIRECTTI << 8));

    TIMER_CHCTL2(timer_periph) &=(uint32_t)(~(TIMER_CHCTL2_CH0P|TIMER_CHCTL2_CH0NP));
    TIMER_CHCTL2(timer_periph) &=(uint32_t)(~(TIMER_CHCTL2_CH1P|TIMER_CHCTL2_CH1NP));
    TIMER_CHCTL2(timer_periph) |= (uint32_t)(ic0polarity|((uint32_t)ic1polarity << 4U));
}

/*!
    \brief      configure TIMER internal clock mode
    \param[in]  timer_periph: TIMERx(x=0,1,2,14)
    \param[out] none
    \retval     none
*/
void timer_internal_clock_config(uint32_t timer_periph)
{
    TIMER_SMCFG(timer_periph) &= (uint32_t)TIMER_SLAVE_MODE_DISABLE;
}

/*!
    \brief      configure TIMER the internal trigger as external clock input
    \param[in]  timer_periph: TIMERx(x=0,1,2,14)
    \param[in]  intrigger: 
      \arg        TIMER_SMCFG_TRGSEL_ITI0: internal trigger 0
      \arg        TIMER_SMCFG_TRGSEL_ITI1: internal trigger 1
      \arg        TIMER_SMCFG_TRGSEL_ITI2: internal trigger 2
      \arg        TIMER_SMCFG_TRGSEL_ITI3: internal trigger 3
    \param[out] none
    \retval     none
*/
void timer_internal_trigger_as_external_clock_config(uint32_t timer_periph, uint32_t intrigger)
{
    timer_input_trigger_source_select(timer_periph, intrigger);
    TIMER_SMCFG(timer_periph) &= ~(uint32_t)TIMER_SMCFG_SMC;    
    TIMER_SMCFG(timer_periph) |= (uint32_t)TIMER_SLAVE_MODE_EXTERNAL0;
}

/*!
    \brief      configure TIMER the external trigger as external clock input
    \param[in]  timer_periph: TIMERx(x=0,1,2,14)
    \param[in]  extrigger: 
      \arg        TIMER_SMCFG_TRGSEL_CI0F_ED: TI0 edge detector
      \arg        TIMER_SMCFG_TRGSEL_CI0FE0: filtered TIMER input 0
      \arg        TIMER_SMCFG_TRGSEL_CI1FE1: filtered TIMER input 1
    \param[in]  expolarity: 
      \arg        TIMER_IC_POLARITY_RISING: active low or falling edge active
      \arg        TIMER_IC_POLARITY_FALLING: active high or rising edge active
    \param[in]  extfilter: a value between 0 and 15
    \param[out] none
    \retval     none
*/
void timer_external_trigger_as_external_clock_config(uint32_t timer_periph, uint32_t extrigger, uint16_t expolarity, uint32_t extfilter)
{
    if(TIMER_SMCFG_TRGSEL_CI1FE1 == extrigger){
        /* reset the CH1EN bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH1EN);
        /* reset the CH1NP bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~(TIMER_CHCTL2_CH1P|TIMER_CHCTL2_CH1NP));
        /* set the CH1NP bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)((uint32_t)expolarity << 4U);
        /* reset the CH1MS bit */
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH1MS);
        /* set the CH1MS bit */
        TIMER_CHCTL0(timer_periph) |= (uint32_t)((uint32_t)TIMER_IC_SELECTION_DIRECTTI << 8U);
        /* reset the CH1CAPFLT bit */
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH1CAPFLT);
        /* set the CH1CAPFLT bit */
        TIMER_CHCTL0(timer_periph) |= (uint32_t)(extfilter << 8U);
        /* set the CH1EN bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)TIMER_CHCTL2_CH1EN;
    }else{
        /* reset the CH0EN bit */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~TIMER_CHCTL2_CH0EN);
        /* reset the CH0P and CH0NP bits */
        TIMER_CHCTL2(timer_periph) &=(uint32_t)(~(TIMER_CHCTL2_CH0P|TIMER_CHCTL2_CH0NP));
        /* set the CH0P and CH0NP bits */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)expolarity;
        /* reset the CH0MS bit */
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH0MS);
        /* set the CH0MS bit */
        TIMER_CHCTL0(timer_periph) |= (uint32_t)TIMER_IC_SELECTION_DIRECTTI;
        /* reset the CH0CAPFLT bit */
        TIMER_CHCTL0(timer_periph) &=(uint32_t)(~TIMER_CHCTL0_CH0CAPFLT);
        /* reset the CH0CAPFLT bit */
        TIMER_CHCTL0(timer_periph) |= (uint32_t)extfilter;
        /* set the CH0EN bit */
        TIMER_CHCTL2(timer_periph) |= (uint32_t)TIMER_CHCTL2_CH0EN;
    }
    /* select TIMER input trigger source */
    timer_input_trigger_source_select(timer_periph, extrigger);
    /* reset the SMC bit */
    TIMER_SMCFG(timer_periph) &=(uint32_t)(~TIMER_SMCFG_SMC);
    /* set the SMC bit */
    TIMER_SMCFG(timer_periph) |= (uint32_t)TIMER_SLAVE_MODE_EXTERNAL0;
}

/*!
    \brief      configure TIMER the external clock mode0
    \param[in]  timer_periph: TIMERx(x=0,1,2)
    \param[in]  extprescaler: 
      \arg        TIMER_EXT_TRI_PSC_OFF: no divided
      \arg        TIMER_EXT_TRI_PSC_DIV2: divided by 2
      \arg        TIMER_EXT_TRI_PSC_DIV4: divided by 4
      \arg        TIMER_EXT_TRI_PSC_DIV8: divided by 8
    \param[in]  expolarity: 
      \arg        TIMER_ETP_FALLING: active low or falling edge active
      \arg        TIMER_ETP_RISING: active high or rising edge active
    \param[in]  extfilter: a value between 0 and 15
    \param[out] none
    \retval     none
*/
void timer_external_clock_mode0_config(uint32_t timer_periph, uint32_t extprescaler, uint32_t expolarity, uint32_t extfilter)
{
    /* configure TIMER external trigger input */
    timer_external_trigger_config(timer_periph, extprescaler, expolarity, extfilter);

    /* reset the SMC bit */
    TIMER_SMCFG(timer_periph) &=(uint32_t)(~TIMER_SMCFG_SMC);
    /* set the SMC bit */
    TIMER_SMCFG(timer_periph) |= (uint32_t)TIMER_SLAVE_MODE_EXTERNAL0;

    /* reset the TRGS bit */
    TIMER_SMCFG(timer_periph) &=(uint32_t)(~TIMER_SMCFG_TRGS);
    /* set the TRGS bit */
    TIMER_SMCFG(timer_periph) |= (uint32_t)TIMER_SMCFG_TRGSEL_ETIFP;
}

/*!
    \brief      configure TIMER the external clock mode1
    \param[in]  timer_periph: TIMERx(x=0,1,2)
    \param[in]  extprescaler: 
      \arg        TIMER_EXT_TRI_PSC_OFF: no divided
      \arg        TIMER_EXT_TRI_PSC_DIV2: divided by 2
      \arg        TIMER_EXT_TRI_PSC_DIV4: divided by 4
      \arg        TIMER_EXT_TRI_PSC_DIV8: divided by 8
    \param[in]  expolarity: 
      \arg        TIMER_ETP_FALLING: active low or falling edge active
      \arg        TIMER_ETP_RISING: active high or rising edge active
    \param[in]  extfilter: a value between 0 and 15
    \param[out] none
    \retval     none
*/
void timer_external_clock_mode1_config(uint32_t timer_periph, uint32_t extprescaler,
                                       uint32_t expolarity, uint32_t extfilter)
{
    /* configure TIMER external trigger input */
    timer_external_trigger_config(timer_periph, extprescaler, expolarity, extfilter);

    TIMER_SMCFG(timer_periph) |= (uint32_t)TIMER_SMCFG_SMC1;
}

/*!
    \brief      disable TIMER the external clock mode1
    \param[in]  timer_periph: TIMERx(x=0..4,7)
    \param[out] none
    \retval     none
*/
void timer_external_clock_mode1_disable(uint32_t timer_periph)
{
    TIMER_SMCFG(timer_periph) &= ~(uint32_t)TIMER_SMCFG_SMC1;
}

/*!
    \brief      configure TIMER13 channel0 remap function
    \param[in]  timer_periph: TIMERx(x=13)
    \param[in]  channel: 
      \arg        TIMER_CH_0: TIMER channel0(TIMERx(x=13))
    \param[in]  remap: 
      \arg        TIMER_IRMP_CI0_RMP_GPIO: channel 0 input is connected to GPIO
      \arg        TIMER_IRMP_CI0_RTCCLK: channel 0 input is connected to the RTCCLK
      \arg        TIMER_IRMP_CI0_HXTALDIV32: channel 0 input is connected to HXTAL/32 clock
      \arg        TIMER_IRMP_CI0_CKOUTSEL: channel 0 input is connected to CKOUTSEL
    \param[out] none
    \retval     none
*/
void timer_channel_remap_config(uint32_t timer_periph, uint16_t channel, uint32_t remap)
{
    if(TIMER_CH_0 == channel){
        TIMER_IRMP(timer_periph) = (uint32_t)remap;
    }
}

#ifdef GD32F170_190

/*!
    \brief      configure TIMER cc register selection
    \param[in]  timer_periph: TIMERx(x=0,1,2,13,14,15,16)
    \param[in]  ccsel: 
      \arg        TIMER_CHVSEL_DISABLE: write CHxVAL register selection disable
      \arg        TIMER_CHVSEL_ENABLE: write CHxVAL register selection enable
    \param[out] none
    \retval     none
*/
void timer_write_cc_register_config(uint32_t timer_periph, uint32_t ccsel)
{
    if(ccsel){
        TIMER_CFG(timer_periph) |= (uint32_t)ccsel;
    }else{
        TIMER_CFG(timer_periph) &= (uint32_t)ccsel;
    }
}

/*!
    \brief      configure TIMER output value selection
    \param[in]  timer_periph: TIMERx(x=0,1,2,13,14,15,16)
    \param[in]  outsel: 
      \arg        TIMER_OUTSEL_DISABLE: output value selection disable
      \arg        TIMER_OUTSEL_ENABLE: output value selection enable
    \param[out] none
    \retval     none
*/
void timer_output_value_selection_config(uint32_t timer_periph, uint32_t outsel)
{
    if(outsel){
        TIMER_CFG(timer_periph) |= (uint32_t)outsel;
    }else{
        TIMER_CFG(timer_periph) &= (uint32_t)outsel;
    }
}

#endif /* GD32F170_190 */
