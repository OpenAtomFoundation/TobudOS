/*!
    \file  gd32f1x0_cmp.c
    \brief CMP driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=5,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=5,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=5,9)
*/

#include "gd32f1x0_cmp.h"

/*!
    \brief      deinitialize comparator 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_deinit(void)
{
    CMP_CS = ((uint32_t)0x00000000);
}

/*!
    \brief      initialize comparator mode 
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[in]  cmp_operating_mode
      \arg        CMP_HIGHSPEED: high speed mode
      \arg        CMP_MIDDLESPEED: medium speed mode
      \arg        CMP_LOWSPEED: low speed mode
      \arg        CMP_VERYLOWSPEED: very-low speed mode
    \param[in]  cmp_inverting_input 
      \arg        CMP_1_4VREFINT: VREFINT *1/4 input
      \arg        CMP_1_2VREFINT: VREFINT *1/2 input
      \arg        CMP_3_4VREFINT: VREFINT *3/4 input
      \arg        CMP_VREFINT: VREFINT input
      \arg        CMP_DAC: PA4 (DAC) input
      \arg        CMP_PA5: PA5 input
      \arg        CMP_PA_0_2: PA0 or PA2 input
    \param[in]  cmp_hysteresis
      \arg        CMP_HYSTERESIS_NO: output no hysteresis
      \arg        CMP_HYSTERESIS_LOW: output low hysteresis
      \arg        CMP_HYSTERESIS_MIDDLE: output middle hysteresis
      \arg        CMP_HYSTERESIS_HIGH: output high hysteresis
    \param[out] none
    \retval     none
*/
void cmp_mode_init(uint32_t cmp_periph, operating_mode_enum cmp_operating_mode, inverting_input_enum cmp_inverting_input, cmp_hysteresis_enum output_hysteresis)
{
    if(CMP0 == cmp_periph){
        /* initialize comparator 0 mode */
        CMP_CS |= CS_CMP0M(cmp_operating_mode) | CS_CMP0MSEL(cmp_inverting_input) | CS_CMP0HST(output_hysteresis);
    }else{
        /* initialize comparator 1 mode */
        CMP_CS |= CS_CMP1M(cmp_operating_mode) | CS_CMP1MSEL(cmp_inverting_input) | CS_CMP1HST(output_hysteresis);
    }
}

/*!
    \brief      initialize comparator output
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[in]  cmp_output 
      \arg        CMP_OUTPUT_NONE: output no selection
      \arg        CMP_OUTPUT_TIMER0BKIN: TIMER 0 break input
      \arg        CMP_OUTPUT_TIMER0IC0: TIMER 0 channel0 input capture 
      \arg        CMP_OUTPUT_TIMER0OCPRECLR: TIMER 0 OCPRE_CLR input
      \arg        CMP_OUTPUT_TIMER1IC3: TIMER 1 channel3 input capture
      \arg        CMP_OUTPUT_TIMER1OCPRECLR: TIMER 1 OCPRE_CLR input
      \arg        CMP_OUTPUT_TIMER2IC0: TIMER 2 channel0 input capture
      \arg        CMP_OUTPUT_TIMER2OCPRECLR: TIMER 2 OCPRE_CLR input
    \param[in]  cmp_output_polarity 
      \arg        CMP_OUTPUT_POLARITY_INVERTED: output is inverted
      \arg        CMP_OUTPUT_POLARITY_NOINVERTED: output is not inverted
    \param[out] none
    \retval     none
*/
void cmp_output_init(uint32_t cmp_periph, cmp_output_enum cmp_output_slection, uint32_t cmp_output_polarity)
{
    /* initialize comparator 0 output */
    if(CMP0 == cmp_periph){
        CMP_CS |= CS_CMP0OSEL(cmp_output_slection);
        /* output polarity */
        if(CMP_OUTPUT_POLARITY_INVERTED == cmp_output_polarity){
            CMP_CS |= CMP_CS_CMP0PL;
        }else{ 
            CMP_CS &= ~CMP_CS_CMP0PL;
        }
    }else{
        CMP_CS |= CS_CMP1OSEL(cmp_output_slection);
        
        if(CMP_OUTPUT_POLARITY_INVERTED == cmp_output_polarity){
            CMP_CS |= CMP_CS_CMP1PL;
        }else{ 
            CMP_CS &= ~CMP_CS_CMP1PL;
        }
    }
}

/*!
    \brief      enable comparator 
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[out] none
    \retval     none
*/
void cmp_enable(uint32_t cmp_periph)
{
    if(CMP0 == cmp_periph){
        CMP_CS |= CMP_CS_CMP0EN;
    }else{
        CMP_CS |= CMP_CS_CMP1EN;
    }
}

/*!
    \brief      disable comparator 
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[out] none
    \retval     none
*/
void cmp_disable(uint32_t cmp_periph)
{
    if(CMP0 == cmp_periph){
        CMP_CS &= ~CMP_CS_CMP0EN;
    }else{
        CMP_CS &= ~CMP_CS_CMP1EN;
    }
}

/*!
    \brief      enable comparator switch
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_switch_enable(void)
{
    CMP_CS |= CMP_CS_CMP0SW;
}

/*!
    \brief      disable comparator switch
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_switch_disable(void)
{
    CMP_CS &= ~CMP_CS_CMP0SW;
}



/*!
    \brief      enable the window mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_window_enable(void)
{
    CMP_CS |= CMP_CS_WNDEN;
}

/*!
    \brief      disable the window mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_window_disable(void)
{
    CMP_CS &= ~CMP_CS_WNDEN;
}

/*!
    \brief      lock the comparator
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[out] none
    \retval     none
*/
void cmp_lock_enable(uint32_t cmp_periph)
{
    if(CMP0 == cmp_periph){
        CMP_CS |= CMP_CS_CMP0LK;
    }else{
        CMP_CS |= CMP_CS_CMP1LK;
    }
}

/*!
    \brief      unlock the comparator
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[out] none
    \retval     none
*/
void cmp_lock_disable(uint32_t cmp_periph)
{
    if(CMP0 == cmp_periph){
        CMP_CS &= ~CMP_CS_CMP0LK;
    }else{
        CMP_CS &= ~CMP_CS_CMP1LK;
    }
}

/*!
    \brief      get output level
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[out] none
    \retval     the output level
*/
uint32_t cmp_output_level_get(uint32_t cmp_periph)
{
    if(CMP0 == cmp_periph){
        if(CMP_CS & CMP_CS_CMP0O){
            return CMP_OUTPUTLEVEL_HIGH;
        }else{
            return CMP_OUTPUTLEVEL_LOW;
        }
    }else{
        if(CMP_CS & CMP_CS_CMP1O){
            return CMP_OUTPUTLEVEL_HIGH;
        }else{
            return CMP_OUTPUTLEVEL_LOW;
        }
    }
}
