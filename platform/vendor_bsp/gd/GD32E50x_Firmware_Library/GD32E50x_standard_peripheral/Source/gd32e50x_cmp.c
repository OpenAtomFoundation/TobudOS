/*!
    \file    gd32e50x_cmp.c
    \brief   CMP driver

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

#include "gd32e50x_cmp.h"

#ifdef GD32E50X_CL
/*!
    \brief      deinitialize comparator
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_deinit(void)
{
    rcu_periph_reset_enable(RCU_CMPRST);
    rcu_periph_reset_disable(RCU_CMPRST);
}

/*!
    \brief      initialize comparator input
    \param[in]  cmp_periph: CMPx(x=1,3,5)
    \param[in]  inverting_input: CMP inverting input select
                only one parameter can be selected which is shown as below:
      \arg        CMP_1_4VREFINT: VREFINT *1/4 input
      \arg        CMP_1_2VREFINT: VREFINT *1/2 input
      \arg        CMP_3_4VREFINT: VREFINT *3/4 input
      \arg        CMP_VREFINT: VREFINT input
      \arg        CMP_PA4: PA4 input
      \arg        CMP_PA5: PA5 input
      \arg        CMP_PA2: PA2 input(only CMP1)
      \arg        CMP_PB_2_15: PB2 input(only CMP1),PB15 input(only CMP3)
    \param[out] none
    \retval     none
*/
void cmp_input_init(cmp_enum cmp_periph,inverting_input_enum inverting_input)
{
    /* initialize comparator mode */
    CMP_CS(cmp_periph) |= CS_CMPMSEL(inverting_input);
}

/*!
    \brief      initialize comparator output
    \param[in]  cmp_periph: CMPx(x=1,3,5)
    \param[in]  output_slection: CMP output select
                only one parameter can be selected which is shown as below:
      \arg        CMP_OUTPUT_NONE: output no selection
      \arg        CMP_OUTPUT_TIMER0_BKIN: TIMER0 break input(CMP1,CMP3,CMP5)
      \arg        CMP_OUTPUT_TIMER2IC2_TIMER1IC1: TIMER2 channel2 input capture(CMP3),TIMER1 channel1 input capture(CMP5) 
      \arg        CMP_OUTPUT_TIMER0IC0: TIMER0 channel0 input capture(CMP1)
      \arg        CMP_OUTPUT_TIMER1IC3: TIMER1 channel3 input capture(CMP1)
      \arg        CMP_OUTPUT_TIMER2IC0: TIMER2 channel0 input capture(CMP1)
    \param[in]  output_polarity: CMP output polarity select
                only one parameter can be selected which is shown as below:
      \arg        CMP_OUTPUT_POLARITY_INVERTED: output is inverted
      \arg        CMP_OUTPUT_POLARITY_NOINVERTED: output is not inverted
    \param[out] none
    \retval     none
*/
void cmp_output_init(cmp_enum cmp_periph,cmp_output_enum output_slection, uint32_t output_polarity)
{
    /* initialize comparator output */
    CMP_CS(cmp_periph) |= CS_CMPOSEL(output_slection);
    /* output polarity */
    if(CMP_OUTPUT_POLARITY_INVERTED == output_polarity){
         CMP_CS(cmp_periph) |= CMP_CS_CMPPL;
    }else{
         CMP_CS(cmp_periph) &= ~CMP_CS_CMPPL;
    }
}

/*!
    \brief      initialize CMP output blanking
    \param[in]  cmp_periph: CMPx(x=1,3,5)
    \param[in]  output_blank: CMP output blank select
                only one parameter can be selected which is shown as below:
      \arg        CMP_OUTPUTBLANK_NONE: no blanking
      \arg        CMP_OUTPUTBLANK_TIMER2_IC3: select TIMER2_CH3 as blanking source(CMP3)
      \arg        CMP_OUTPUTBLANK_TIMER1IC2: select TIMER1_CH2 as blanking source(CMP1)
      \arg        CMP_OUTPUTBLANK_TIMER2IC2_TIMER1IC3: Select TIMER2_CH2 as blanking source(CMP1), select TIMER1_CH3 as blanking source(CMP5)
    \param[out] none
    \retval     none
*/
void cmp_outputblank_init(cmp_enum cmp_periph,cmp_outputblank_enum output_blank)
{
   CMP_CS(cmp_periph) |= CS_CMPMBLK(output_blank);
}

/*!
    \brief      enable comparator 
    \param[in]  cmp_periph: CMPx(x=1,3,5)
    \param[out] none
    \retval     none
*/
void cmp_enable(cmp_enum cmp_periph)
{
     CMP_CS(cmp_periph) |= CMP_CS_CMPEN;
}

/*!
    \brief      disable comparator 
    \param[in]  cmp_periph: CMPx(x=1,3,5)
    \param[out] none
    \retval     none
*/
void cmp_disable(cmp_enum cmp_periph)
{
    CMP_CS(cmp_periph) &= ~CMP_CS_CMPEN;
}

/*!
    \brief      lock the comparator
    \param[in]  cmp_periph: CMPx(x=1,3,5)
    \param[out] none
    \retval     none
*/
void cmp_lock_enable(cmp_enum cmp_periph)
{
    CMP_CS(cmp_periph) |= CMP_CS_CMPLK;
}

/*!
    \brief      get output level
    \param[in]  cmp_periph: CMPx(x=1,3,5)
    \param[out] none
    \retval     the output level
*/
uint32_t cmp_output_level_get(cmp_enum cmp_periph)
{
    if(CMP_CS(cmp_periph) & CMP_CS_CMPO){
        return CMP_OUTPUTLEVEL_HIGH;
    }else{
        return CMP_OUTPUTLEVEL_LOW;
    }
}

#endif /* GD32E50x_CL */
