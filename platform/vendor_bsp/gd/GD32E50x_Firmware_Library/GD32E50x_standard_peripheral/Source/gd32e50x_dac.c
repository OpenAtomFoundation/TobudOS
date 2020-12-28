/*!
    \file    gd32e50x_dac.c
    \brief   DAC driver

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32e50x_dac.h"

/* DAC register bit offset */
#define OUT1_REG_OFFSET           ((uint32_t)0x00000010U)
#define DH_12BIT_OFFSET           ((uint32_t)0x00000010U)
#define DH_8BIT_OFFSET            ((uint32_t)0x00000008U)

/*!
    \brief      deinitialize DAC
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
    \brief      enable DAC
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_enable(uint8_t dac_out)
{
    if(DAC_OUT_0 == dac_out){
        DAC_CTL0 |= DAC_CTL0_DEN0;
    }else{
        DAC_CTL0 |= DAC_CTL0_DEN1;
    }
} 

/*!
    \brief      disable DAC
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_disable(uint8_t dac_out)
{
    if(DAC_OUT_0 == dac_out){
        DAC_CTL0 &= ~DAC_CTL0_DEN0;
    }else{
        DAC_CTL0 &= ~DAC_CTL0_DEN1;
    }
}

/*!
    \brief      enable DAC DMA function
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_dma_enable(uint8_t dac_out)
{
    if(DAC_OUT_0 == dac_out){
        DAC_CTL0 |= DAC_CTL0_DDMAEN0;
    }else{
        DAC_CTL0 |= DAC_CTL0_DDMAEN1;
    }
}

/*!
    \brief      disable DAC DMA function
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_dma_disable(uint8_t dac_out)
{
    if(DAC_OUT_0 == dac_out){
        DAC_CTL0 &= ~DAC_CTL0_DDMAEN0;
    }else{
        DAC_CTL0 &= ~DAC_CTL0_DDMAEN1;
    }
}

/*!
    \brief      enable DAC output buffer
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_output_buffer_enable(uint8_t dac_out)
{
    if(DAC_OUT_0 == dac_out){
        DAC_CTL0 &= ~DAC_CTL0_DBOFF0;
    }else{
        DAC_CTL0 &= ~DAC_CTL0_DBOFF1;
    }
}

/*!
    \brief      disable DAC output buffer
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_output_buffer_disable(uint8_t dac_out)
{
    if(DAC_OUT_0 == dac_out){
        DAC_CTL0 |= DAC_CTL0_DBOFF0;
    }else{
        DAC_CTL0 |= DAC_CTL0_DBOFF1;
    }
}

/*!
    \brief      get DAC output value
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[out] none
    \retval     DAC output data: 0~4095
*/
uint16_t dac_output_value_get(uint8_t dac_out)
{
    uint16_t data = 0U;
    if(DAC_OUT_0 == dac_out){
        /* store the DAC_OUT0 output value */
        data = (uint16_t)OUT0_DO;
    }else{
        /* store the DAC_OUT1 output value */
        data = (uint16_t)OUT1_DO;
    }
    return data;
}

/*!
    \brief      set DAC data holding register value
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[in]  dac_align: DAC data alignment mode
                only one parameter can be selected which is shown as below:
      \arg        DAC_ALIGN_12B_R: 12-bit right-aligned data
      \arg        DAC_ALIGN_12B_L: 12-bit left-aligned data
      \arg        DAC_ALIGN_8B_R: 8-bit right-aligned data
    \param[in]  data: data to be loaded, 0~4095
    \param[out] none
    \retval     none
*/
void dac_data_set(uint8_t dac_out, uint32_t dac_align, uint16_t data)
{
    /* DAC_OUT0 data alignment */
    if(DAC_OUT_0 == dac_out){
        switch(dac_align){
        /* 12-bit right-aligned data */
        case DAC_ALIGN_12B_R:
            OUT0_R12DH = data;
            break;
        /* 12-bit left-aligned data */
        case DAC_ALIGN_12B_L:
            OUT0_L12DH = data;
            break;
        /* 8-bit right-aligned data */
        case DAC_ALIGN_8B_R:
            OUT0_R8DH = data;
            break;
        default:
            break;
        }
    }else{
        /* DAC_OUT1 data alignment */
        switch(dac_align){
        /* 12-bit right-aligned data */
        case DAC_ALIGN_12B_R:
            OUT1_R12DH = data;
            break;
        /* 12-bit left-aligned data */
        case DAC_ALIGN_12B_L:
            OUT1_L12DH = data;
            break;
        /* 8-bit right-aligned data */
        case DAC_ALIGN_8B_R:
            OUT1_R8DH = data;
            break;
        default:
            break;
        }
    }
}

/*!
    \brief      enable DAC output FIFO
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_output_fifo_enable(uint8_t dac_out)
{
    if(DAC_OUT_0 == dac_out){
        DAC_CTL1 |= DAC_CTL1_FIFOEN0;
    }else{
        DAC_CTL1 |= DAC_CTL1_FIFOEN1;
    }
}

/*!
    \brief      disable DAC output FIFO
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_output_fifo_disable(uint8_t dac_out)
{

    if(DAC_OUT_0 == dac_out){
        DAC_CTL1 &= ~DAC_CTL1_FIFOEN0;
    }else{
        DAC_CTL1 &= ~DAC_CTL1_FIFOEN1;
    }
}

/*!
    \brief      get DAC output FIFO number
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[out] none
    \retval     DAC output FIFO number: 0~4
*/
uint16_t dac_output_fifo_number_get(uint8_t dac_out)
{
    uint16_t number = 0U;
    
    if(DAC_OUT_0 == dac_out){
        /* get the DAC_OUT0 output FIFO number */
        number = (uint16_t)((uint16_t)DAC_STAT1 >> 4U);
    }else{
        /* get the DAC_OUT1 output FIFO number */
        number = (uint16_t)(DAC_STAT1 >> 20U);
    }
    return number;
}

/*!
    \brief      enable DAC trigger
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_trigger_enable(uint8_t dac_out)
{
    if(DAC_OUT_0 == dac_out){
        DAC_CTL0 |= DAC_CTL0_DTEN0;
    }else{
        DAC_CTL0 |= DAC_CTL0_DTEN1;
    }
}

/*!
    \brief      disable DAC trigger
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_trigger_disable(uint8_t dac_out)
{
    if(DAC_OUT_0 == dac_out){
        DAC_CTL0 &= ~DAC_CTL0_DTEN0;
    }else{
        DAC_CTL0 &= ~DAC_CTL0_DTEN1;
    }
}

/*!
    \brief      configure DAC trigger source
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[in]  triggersource: external triggers of DAC
                only one parameter can be selected which is shown as below:
      \arg        DAC_TRIGGER_T5_TRGO: TIMER5 TRGO
      \arg        DAC_TRIGGER_T7_TRGO: TIMER7 TRGO (for GD32E50X_HD and GD32E50X_XD devices)
      \arg        DAC_TRIGGER_T2_TRGO: TIMER2 TRGO (for GD32E50X_CL devices)
      \arg        DAC_TRIGGER_T6_TRGO: TIMER6 TRGO
      \arg        DAC_TRIGGER_T4_TRGO: TIMER4 TRGO
      \arg        DAC_TRIGGER_T1_TRGO: TIMER1 TRGO
      \arg        DAC_TRIGGER_T3_TRGO: TIMER3 TRGO
      \arg        DAC_TRIGGER_EXTI_9: EXTI interrupt line9 event
      \arg        DAC_TRIGGER_SOFTWARE: software trigger
      \arg        DAC_TRIGGER_SHRTIMER_DACTRIG0: SHRTIMER_DACTRIG0 trigger(for GD32E50X_HD, GD32E50X_XD and GD32E50X_CL devices)
      \arg        DAC_TRIGGER_SHRTIMER_DACTRIG1: SHRTIMER_DACTRIG1 trigger(for GD32E50X_HD, GD32E50X_XD and GD32E50X_CL devices)
      \arg        DAC_TRIGGER_SHRTIMER_DACTRIG2: SHRTIMER_DACTRIG2 trigger(for GD32E50X_HD, GD32E50X_XD and GD32E50X_CL devices)
    \param[out] none
    \retval     none
*/
void dac_trigger_source_config(uint8_t dac_out,uint32_t triggersource)
{
    if(DAC_OUT_0 == dac_out){
        /* configure DAC_OUT0 trigger source */
        DAC_CTL0 &= (uint32_t)(~(DAC_CTL0_DTSEL0 | DAC_CTL0_DTSEL0_3));
        DAC_CTL0 |= triggersource;
    }else{
        /* configure DAC_OUT1 trigger source */
        DAC_CTL0 &= (uint32_t)(~(DAC_CTL0_DTSEL1 | DAC_CTL0_DTSEL1_3));
        DAC_CTL0 |= (triggersource << OUT1_REG_OFFSET);
    }
}

/*!
    \brief      enable DAC software trigger
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \retval     none
*/
void dac_software_trigger_enable(uint8_t dac_out)
{
    if(DAC_OUT_0 == dac_out){
        DAC_SWT |= DAC_SWT_SWTR0;
    }else{
        DAC_SWT |= DAC_SWT_SWTR1;
    }
}

/*!
    \brief      disable DAC software trigger
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_software_trigger_disable(uint8_t dac_out)
{
    if(DAC_OUT_0 == dac_out){
        DAC_SWT &= ~DAC_SWT_SWTR0;
    }else{
        DAC_SWT &= ~DAC_SWT_SWTR1;
    }
}

/*!
    \brief      configure DAC wave mode
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[in]  wave_mode: DAC wave mode
                only one parameter can be selected which is shown as below:
      \arg        DAC_WAVE_DISABLE: wave mode disable
      \arg        DAC_WAVE_MODE_LFSR: LFSR noise mode
      \arg        DAC_WAVE_MODE_TRIANGLE: triangle noise mode
    \param[out] none
    \retval     none
*/
void dac_wave_mode_config(uint8_t dac_out, uint32_t wave_mode)
{
    if(DAC_OUT_0 == dac_out){
        /* configure DAC_OUT0 wave mode */
        DAC_CTL0 &= ~DAC_CTL0_DWM0;
        DAC_CTL0 |= wave_mode;
    }else{
        /* configure DAC_OUT1 wave mode */
        DAC_CTL0 &= ~DAC_CTL0_DWM1;
        DAC_CTL0 |= (wave_mode << OUT1_REG_OFFSET);
    }
}

/*!
    \brief      configure DAC wave bit width
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[in]  bit_width: DAC noise wave bit width
                only one parameter can be selected which is shown as below:
      \arg        DAC_WAVE_BIT_WIDTH_1: bit width of the wave signal is 1
      \arg        DAC_WAVE_BIT_WIDTH_2: bit width of the wave signal is 2
      \arg        DAC_WAVE_BIT_WIDTH_3: bit width of the wave signal is 3
      \arg        DAC_WAVE_BIT_WIDTH_4: bit width of the wave signal is 4
      \arg        DAC_WAVE_BIT_WIDTH_5: bit width of the wave signal is 5
      \arg        DAC_WAVE_BIT_WIDTH_6: bit width of the wave signal is 6
      \arg        DAC_WAVE_BIT_WIDTH_7: bit width of the wave signal is 7
      \arg        DAC_WAVE_BIT_WIDTH_8: bit width of the wave signal is 8
      \arg        DAC_WAVE_BIT_WIDTH_9: bit width of the wave signal is 9
      \arg        DAC_WAVE_BIT_WIDTH_10: bit width of the wave signal is 10
      \arg        DAC_WAVE_BIT_WIDTH_11: bit width of the wave signal is 11
      \arg        DAC_WAVE_BIT_WIDTH_12: bit width of the wave signal is 12
    \param[out] none
    \retval     none
*/
void dac_wave_bit_width_config(uint8_t dac_out, uint32_t bit_width)
{
    if(DAC_OUT_0 == dac_out){
        /* configure DAC_OUT0 wave bit width */
        DAC_CTL0 &= ~DAC_CTL0_DWBW0;
        DAC_CTL0 |= bit_width;
    }else{
        /* configure DAC_OUT1 wave bit width */
        DAC_CTL0 &= ~DAC_CTL0_DWBW1;
        DAC_CTL0 |= (bit_width << OUT1_REG_OFFSET);
    }
}

/*!
    \brief      configure DAC LFSR noise mode
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[in]  unmask_bits: LFSR noise unmask bits
                only one parameter can be selected which is shown as below:
      \arg        DAC_LFSR_BIT0: unmask the LFSR bit0
      \arg        DAC_LFSR_BITS1_0: unmask the LFSR bits[1:0]
      \arg        DAC_LFSR_BITS2_0: unmask the LFSR bits[2:0]
      \arg        DAC_LFSR_BITS3_0: unmask the LFSR bits[3:0]
      \arg        DAC_LFSR_BITS4_0: unmask the LFSR bits[4:0]
      \arg        DAC_LFSR_BITS5_0: unmask the LFSR bits[5:0]
      \arg        DAC_LFSR_BITS6_0: unmask the LFSR bits[6:0]
      \arg        DAC_LFSR_BITS7_0: unmask the LFSR bits[7:0]
      \arg        DAC_LFSR_BITS8_0: unmask the LFSR bits[8:0]
      \arg        DAC_LFSR_BITS9_0: unmask the LFSR bits[9:0]
      \arg        DAC_LFSR_BITS10_0: unmask the LFSR bits[10:0]
      \arg        DAC_LFSR_BITS11_0: unmask the LFSR bits[11:0]
    \param[out] none
    \retval     none
*/
void dac_lfsr_noise_config(uint8_t dac_out, uint32_t unmask_bits)
{
    if(DAC_OUT_0 == dac_out){
        /* configure DAC_OUT0 LFSR noise mode */
        DAC_CTL0 &= ~DAC_CTL0_DWBW0;
        DAC_CTL0 |= unmask_bits;
    }else{
        /* configure DAC_OUT1 LFSR noise mode */
        DAC_CTL0 &= ~DAC_CTL0_DWBW1;
        DAC_CTL0 |= (unmask_bits << OUT1_REG_OFFSET);
    }
}

/*!
    \brief      configure DAC triangle noise mode
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[in]  amplitude: the amplitude of the triangle
                only one parameter can be selected which is shown as below:
      \arg        DAC_TRIANGLE_AMPLITUDE_1: triangle amplitude is 1
      \arg        DAC_TRIANGLE_AMPLITUDE_3: triangle amplitude is 3
      \arg        DAC_TRIANGLE_AMPLITUDE_7: triangle amplitude is 7
      \arg        DAC_TRIANGLE_AMPLITUDE_15: triangle amplitude is 15
      \arg        DAC_TRIANGLE_AMPLITUDE_31: triangle amplitude is 31
      \arg        DAC_TRIANGLE_AMPLITUDE_63: triangle amplitude is 63
      \arg        DAC_TRIANGLE_AMPLITUDE_127: triangle amplitude is 127
      \arg        DAC_TRIANGLE_AMPLITUDE_255: triangle amplitude is 255
      \arg        DAC_TRIANGLE_AMPLITUDE_511: triangle amplitude is 511
      \arg        DAC_TRIANGLE_AMPLITUDE_1023: triangle amplitude is 1023
      \arg        DAC_TRIANGLE_AMPLITUDE_2047: triangle amplitude is 2047
      \arg        DAC_TRIANGLE_AMPLITUDE_4095: triangle amplitude is 4095
    \param[out] none
    \retval     none
*/
void dac_triangle_noise_config(uint8_t dac_out, uint32_t amplitude)
{
    if(DAC_OUT_0 == dac_out){
        /* configure DAC_OUT0 triangle noise mode */
        DAC_CTL0 &= ~DAC_CTL0_DWBW0;
        DAC_CTL0 |= amplitude;
    }else{
        /* configure DAC_OUT1 triangle noise mode */
        DAC_CTL0 &= ~DAC_CTL0_DWBW1;
        DAC_CTL0 |= (amplitude << OUT1_REG_OFFSET);
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
    ctl = (uint32_t)(DAC_CTL0_DEN0 | DAC_CTL0_DEN1);
    DAC_CTL0 |= (uint32_t)(ctl);
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
    ctl = (uint32_t)(DAC_CTL0_DEN0 | DAC_CTL0_DEN1);
    DAC_CTL0 &= (uint32_t)(~ctl);
}

/*!
    \brief      enable DAC concurrent software trigger
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_software_trigger_enable(void)
{
    uint32_t swt = 0U;
    swt = (uint32_t)(DAC_SWT_SWTR0 | DAC_SWT_SWTR1);
    DAC_SWT |= (uint32_t)(swt); 
}

/*!
    \brief      disable DAC concurrent software trigger
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_software_trigger_disable(void)
{
    uint32_t swt = 0U;
    swt = (uint32_t)(DAC_SWT_SWTR0 | DAC_SWT_SWTR1);
    DAC_SWT &= (uint32_t)(~swt);
}

/*!
    \brief      enable DAC concurrent buffer
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_output_buffer_enable(void)
{
    uint32_t ctl = 0U;
    ctl = (uint32_t)(DAC_CTL0_DBOFF0 | DAC_CTL0_DBOFF1);
    DAC_CTL0 &= (uint32_t)(~ctl);
}

/*!
    \brief      disable DAC concurrent buffer
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_output_buffer_disable(void)
{
    uint32_t ctl = 0U;
    ctl = (uint32_t)(DAC_CTL0_DBOFF0 | DAC_CTL0_DBOFF1);
    DAC_CTL0 |= (uint32_t)(ctl);
}

/*!
    \brief      set DAC concurrent mode data holding register value
    \param[in]  dac_align: DAC data alignment mode
                only one parameter can be selected which is shown as below:
      \arg        DAC_ALIGN_12B_R: 12-bit right-aligned data
      \arg        DAC_ALIGN_12B_L: 12-bit left-aligned data
      \arg        DAC_ALIGN_8B_R: 8-bit right-aligned data
    \param[in]  data0: data to be loaded, 0~4095
    \param[in]  data1: data to be loaded, 0~4095
    \param[out] none
    \retval     none
*/
void dac_concurrent_data_set(uint32_t dac_align, uint16_t data0, uint16_t data1)
{
    uint32_t data = 0U;
    switch(dac_align){
    /* 12-bit right-aligned data */
    case DAC_ALIGN_12B_R:
        data = (uint32_t)(((uint32_t)data1 << DH_12BIT_OFFSET) | data0);
        DACC_R12DH = (uint32_t)data;
        break;
    /* 12-bit left-aligned data */
    case DAC_ALIGN_12B_L:
        data = (uint32_t)(((uint32_t)data1 << DH_12BIT_OFFSET) | data0);
        DACC_L12DH = (uint32_t)data;
        break;
    /* 8-bit right-aligned data */
    case DAC_ALIGN_8B_R:
        data = (uint32_t)(((uint32_t)data1 << DH_8BIT_OFFSET) | data0);
        DACC_R8DH = (uint32_t)data;
        break;
    default:
        break;
    }
}

/*!
    \brief      get DAC flag
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[in]  dac_flag: DAC flag
                only one parameter can be selected which is shown as below:
      \arg        DAC_FLAG_DDUDR0: DAC_OUT0 DMA underrun flag
      \arg        DAC_FLAG_FF0: DAC_OUT0 FIFO full flag
      \arg        DAC_FLAG_FE0: DAC_OUT0 FIFO empty flag
      \arg        DAC_FLAG_FIFOOVR0: DAC_OUT0 FIFO overflow flag
      \arg        DAC_FLAG_FIFOUDR0: DAC_OUT0 FIFO underflow flag
      \arg        DAC_FLAG_DDUDR1: DAC_OUT1 DMA underrun flag
      \arg        DAC_FLAG_FF1: DAC_OUT1 FIFO full flag
      \arg        DAC_FLAG_FE1: DAC_OUT1 FIFO empty flag
      \arg        DAC_FLAG_FIFOOVR1: DAC_OUT1 FIFO overflow flag
      \arg        DAC_FLAG_FIFOUDR1: DAC_OUT1 FIFO underflow flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus dac_flag_get(uint8_t dac_out, uint32_t flag)
{
    if(DAC_OUT_0 == dac_out){
        /* check DAC_OUT0 flag */
        if(DAC_FLAG_DDUDR0 == flag){
           if(RESET != (DAC_STAT0 & DAC_STAT0_DDUDR0)){
               return SET;
           }else{
               return RESET;
           }
        }else{
            if(RESET != (DAC_STAT1 & flag)){
                return SET;
            }else{
                return RESET;
            }
        }
    }else{
        /* check DAC_OUT1 flag */
        if(DAC_FLAG_DDUDR1 == flag){
            if(RESET != (DAC_STAT0 & DAC_STAT0_DDUDR1)){
                return SET;
            }else{
                return RESET;
            }
        }else{
            if(RESET != (DAC_STAT1 & flag)){
                return SET;
            }else{
                return RESET;
            }
        }
    }
}

/*!
    \brief      clear DAC flag
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[in]  flag: DAC flag
                only one parameter can be selected which is shown as below:
      \arg        DAC_FLAG_DDUDR0: DAC_OUT0 DMA underrun flag
      \arg        DAC_FLAG_FIFOOVR0: DAC_OUT0 FIFO overflow flag
      \arg        DAC_FLAG_FIFOUDR0: DAC_OUT0 FIFO underflow flag
      \arg        DAC_FLAG_DDUDR1: DAC_OUT1 DMA underrun flag
      \arg        DAC_FLAG_FIFOOVR1: DAC_OUT1 FIFO overflow flag
      \arg        DAC_FLAG_FIFOUDR1: DAC_OUT1 FIFO underflow flag
    \param[out] none
    \retval     none
*/
void dac_flag_clear(uint8_t dac_out, uint32_t flag)
{
    if(DAC_OUT_0 == dac_out){
        /* clear DAC_OUT0 flag */
        if(DAC_FLAG_DDUDR0 == flag){
            DAC_STAT0 |= (uint32_t)DAC_STAT0_DDUDR0;
        }else{
            DAC_STAT1 |= (uint32_t)flag;
        }
    }else{
        /* clear DAC_OUT1 flag */
        if(DAC_FLAG_DDUDR1 == flag){
            DAC_STAT0 |= (uint32_t)DAC_STAT0_DDUDR1;
        }else{
            DAC_STAT1 |= (uint32_t)flag;
        }
    }
}

/*!
    \brief      enable DAC interrupt
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[in]  interrupt: the DAC interrupt
                only one parameter can be selected which is shown as below:
      \arg        DAC_INT_DDUDRIE0: DAC_OUT0 DMA underrun interrupt enable
      \arg        DAC_INT_FIFOOVRIE0: DAC_OUT0 FIFO overflow interrupt enable
      \arg        DAC_INT_FIFOUDRIE0: DAC_OUT0 FIFO underflow interrupt enable
      \arg        DAC_INT_DDUDRIE1: DAC_OUT1 DMA underrun interrupt enable
      \arg        DAC_INT_FIFOOVRIE1: DAC_OUT1 FIFO overflow interrupt enable
      \arg        DAC_INT_FIFOUDRIE1: DAC_OUT1 FIFO underflow interrupt enable
    \param[out] none
    \retval     none
*/
void dac_interrupt_enable(uint8_t dac_out, uint32_t interrupt)
{
    if(DAC_OUT_0 == dac_out){
        /* enable DAC_OUT0 interrupt */
        if(DAC_INT_DDUDRIE0 == interrupt){
            DAC_CTL0 |= (uint32_t)DAC_CTL0_DDUDRIE0;
        }else{
            DAC_CTL1 |= (uint32_t)interrupt;
        }
    }else{
        /* enable DAC_OUT1 interrupt */
        if(DAC_INT_DDUDRIE1 == interrupt){
            DAC_CTL0 |= (uint32_t)DAC_CTL0_DDUDRIE1;
        }else{
            DAC_CTL1 |= (uint32_t)interrupt;
        }
    }
}

/*!
    \brief      disable DAC interrupt
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[in]  interrupt: the DAC interrupt
                only one parameter can be selected which is shown as below:
      \arg        DAC_INT_DDUDRIE0: DAC_OUT0 DMA underrun interrupt disable
      \arg        DAC_INT_FIFOOVRIE0: DAC_OUT0 FIFO overflow interrupt disable
      \arg        DAC_INT_FIFOUDRIE0: DAC_OUT0 FIFO underflow interrupt disable
      \arg        DAC_INT_DDUDRIE1: DAC_OUT1 DMA underrun interrupt disable
      \arg        DAC_INT_FIFOOVRIE1: DAC_OUT1 FIFO overflow interrupt disable
      \arg        DAC_INT_FIFOUDRIE1: DAC_OUT1 FIFO underflow interrupt disable
    \param[out] none
    \retval     none
*/
void dac_interrupt_disable(uint8_t dac_out, uint32_t interrupt)
{
    if(DAC_OUT_0 == dac_out){
        /* disable DAC_OUT0 interrupt */
        if(DAC_INT_DDUDRIE0 == interrupt){
            DAC_CTL0 &= (uint32_t)(~DAC_CTL0_DDUDRIE0);
        }else{
            DAC_CTL1 &= (uint32_t)(~interrupt);
        }
    }else{
        /* disable DAC_OUT1 interrupt */
        if(DAC_INT_DDUDRIE1 == interrupt){
            DAC_CTL0 &= (uint32_t)(~DAC_CTL0_DDUDRIE1);
        }else{
            DAC_CTL1 &= (uint32_t)(~interrupt);
        }
    }
}

/*!
    \brief      get DAC interrupt flag
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[in]  int_flag: DAC interrupt flag
                only one parameter can be selected which is shown as below:
      \arg        DAC_INT_FLAG_DDUDR0: DAC_OUT0 DMA underrun interrupt flag
      \arg        DAC_INT_FLAG_FIFOOVR0: DAC_OUT0 FIFO overflow interrupt flag
      \arg        DAC_INT_FLAG_FIFOUDR0: DAC_OUT0 FIFO underflow interrupt flag
      \arg        DAC_INT_FLAG_DDUDR1: DAC_OUT1 DMA underrun interrupt flag
      \arg        DAC_INT_FLAG_FIFOOVR1: DAC_OUT1 FIFO overflow interrupt flag
      \arg        DAC_INT_FLAG_FIFOUDR1: DAC_OUT1 FIFO underflow interrupt flag
    \param[out] none
    \retval     the state of DAC interrupt flag(SET or RESET)
*/
FlagStatus dac_interrupt_flag_get(uint8_t dac_out, uint32_t int_flag)
{
    uint32_t reg1 = 0U, reg2 = 0U;

    /* check DAC_OUT0 flag */
    if(DAC_OUT_0 == dac_out){
        /* check DAC_OUT0 interrupt flag */
        if(DAC_INT_FLAG_DDUDR0 == int_flag){
            reg1 = DAC_STAT0 & DAC_STAT0_DDUDR0;
            reg2 = DAC_CTL0 & DAC_CTL0_DDUDRIE0;
        }else if(DAC_INT_FLAG_FIFOOVR0 == int_flag){
            reg1 = DAC_STAT1 & DAC_STAT1_FIFOOVR0;
            reg2 = DAC_CTL1 & DAC_CTL1_FIFOOVRIE0;
        }else{
            reg1 = DAC_STAT1 & DAC_FLAG_FIFOUDR0;
            reg2 = DAC_CTL1 & DAC_INT_FIFOUDRIE0;
        }
    }else{
        /* check DAC_OUT1 interrupt flag */
        if(DAC_INT_FLAG_DDUDR1 == int_flag){
            reg1 = DAC_STAT0 & DAC_STAT0_DDUDR1;
            reg2 = DAC_CTL0 & DAC_CTL0_DDUDRIE1;
        }else if(DAC_INT_FLAG_FIFOOVR1 == int_flag){
            reg1 = DAC_STAT1 & DAC_STAT1_FIFOOVR1;
            reg2 = DAC_CTL1 & DAC_CTL1_FIFOOVRIE1;
        }else{
            reg1 = DAC_STAT1 & DAC_FLAG_FIFOUDR1;
            reg2 = DAC_CTL1 & DAC_INT_FIFOUDRIE1;
        }
    }

    /*get DAC interrupt flag status */
    if((RESET != reg1) && (RESET != reg2)){
        return SET;
    }else{
        return RESET;
    }
}

/*!
    \brief      clear DAC interrupt flag
    \param[in]  dac_out: DAC_OUT_x(x=0,1)
    \param[in]  int_flag: DAC interrupt flag
                only one parameter can be selected which is shown as below:
      \arg        DAC_INT_FLAG_DDUDR0: DAC_OUT0 DMA underrun interrupt flag
      \arg        DAC_INT_FLAG_FIFOOVR0: DAC_OUT0 FIFO overflow interrupt flag
      \arg        DAC_INT_FLAG_FIFOUDR0: DAC_OUT0 FIFO underflow interrupt flag
      \arg        DAC_INT_FLAG_DDUDR1: DAC_OUT1 DMA underrun interrupt flag
      \arg        DAC_INT_FLAG_FIFOOVR1: DAC_OUT1 FIFO overflow interrupt flag
      \arg        DAC_INT_FLAG_FIFOUDR1: DAC_OUT1 FIFO underflow interrupt flag
    \param[out] none
    \retval     none
*/
void dac_interrupt_flag_clear(uint8_t dac_out, uint32_t int_flag)
{
    if(DAC_OUT_0 == dac_out){
        /* clear DAC_OUT0 interrupt flag */
        if(DAC_INT_FLAG_DDUDR0 == int_flag){
            DAC_STAT0 |= (uint32_t)DAC_STAT0_DDUDR0;
        }else{
            DAC_STAT1 |= (uint32_t)int_flag;
        }
    }else{
        /* clear DAC_OUT1 interrupt flag */
        if(DAC_INT_FLAG_DDUDR1 == int_flag){
            DAC_STAT0 |= (uint32_t)DAC_STAT0_DDUDR1;
        }else{
            DAC_STAT1 |= (uint32_t)int_flag;
        }
    }
}
