/*!
    \file    gd32e50x_cmp.h
    \brief   definitions for the CMP

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

#ifndef GD32E50X_CMP_H
#define GD32E50X_CMP_H

#include "gd32e50x.h"

/* CMP definitions */
#define CMP                                      CMP_BASE                       /*!< CMP base address */
 
/* registers definitions */
#define CMP_CS(CMPx)                              REG32((CMP) + 0x00000020U + (CMPx) * 0x00000008U)


/* CMPx_CS bits definitions */
#define CMP_CS_CMPEN                             BIT(0)                         /*!< CMPx enable */
#define CMP_CS_CMPMSEL                           BITS(4,6)                      /*!< CMPx_IM input selection */
#define CMP_CS_CMPOSEL                           BITS(10,13)                    /*!< CMPx output selection */
#define CMP_CS_CMPPL                             BIT(15)                        /*!< polarity of CMPx output */
#define CMP_CS_CMPBLK                            BITS(18,20)                    /*!< CMPx output blanking source */
#define CMP_CS_CMPMSEL_3                         BIT(22)                        /*!< bit3 of CMPx_CS_CMP1MSEL */
#define CMP_CS_CMPO                              BIT(30)                        /*!< CMPx output */
#define CMP_CS_CMPLK                             BIT(31)                        /*!< CMPx lock */

/* CMP1_CS bits definitions */
#define CMP1_CS_CMP1EN                           BIT(0)                         /*!< CMP1 enable */
#define CMP1_CS_CMP1MSEL                         BITS(4,6)                      /*!< CMP1_IM input selection */
#define CMP1_CS_CMP1OSEL                         BITS(10,13)                    /*!< CMP1 output selection */
#define CMP1_CS_CMP1PL                           BIT(15)                        /*!< polarity of CMP1 output */
#define CMP1_CS_CMP1BLK                          BITS(18,20)                    /*!< CMP1 output blanking source */
#define CMP1_CS_CMP1MSEL_3                       BIT(22)                        /*!< bit3 of CMP1_CS_CMP1MSEL */
#define CMP1_CS_CMP1O                            BIT(30)                        /*!< CMP1 output */
#define CMP1_CS_CMP1LK                           BIT(31)                        /*!< CMP1 lock */

/* CMP3_CS bits definitions */
#define CMP3_CS_CMP3EN                           BIT(0)                         /*!< CMP3 enable */
#define CMP3_CS_CMP3MSEL                         BITS(4,6)                      /*!< CMP3_IM input selection */
#define CMP3_CS_CMP3OSEL                         BITS(10,13)                    /*!< CMP3 output selection */
#define CMP3_CS_CMP3PL                           BIT(15)                        /*!< polarity of CMP3 output */
#define CMP3_CS_CMP3BLK                          BITS(18,20)                    /*!< CMP3 output blanking source */
#define CMP3_CS_CMP3MSEL_3                       BIT(22)                        /*!< bit3 of CMP3_CS_CMP3MSEL */
#define CMP3_CS_CMP3O                            BIT(30)                        /*!< CMP3 output */
#define CMP3_CS_CMP3LK                           BIT(31)                        /*!< CMP3 lock */

/* CMP5_CS bits definitions */
#define CMP5_CS_CMP5EN                           BIT(0)                         /*!< CMP5 enable */
#define CMP5_CS_CMP5MSEL                         BITS(4,6)                      /*!< CMP5_IM input selection */
#define CMP5_CS_CMP5OSEL                         BITS(10,13)                    /*!< CMP5 output selection */
#define CMP5_CS_CMP5PL                           BIT(15)                        /*!< polarity of CMP5 output */
#define CMP5_CS_CMP5BLK                          BITS(18,20)                    /*!< CMP5 output blanking source */
#define CMP5_CS_CMP5MSEL_3                       BIT(22)                        /*!< bit3 of CMP5_CS_CMP5MSEL */
#define CMP5_CS_CMP5O                            BIT(30)                        /*!< CMP5 output */
#define CMP5_CS_CMP5LK                           BIT(31)                        /*!< CMP5 lock */

/* consts definitions */
/* CMP units */
typedef enum
{
    CMP1,                                                                       /*!< Cmoparator 1 */
    CMP3,                                                                       /*!< Cmoparator 3 */
    CMP5                                                                        /*!< Cmoparator 5 */
}cmp_enum;

/* inverting input */
typedef enum
{
    CMP_1_4VREFINT = 0,                                                         /*!< VREFINT /4 input */
    CMP_1_2VREFINT,                                                             /*!< VREFINT /2 input */
    CMP_3_4VREFINT,                                                             /*!< VREFINT *3/4 input */
    CMP_VREFINT,                                                                /*!< VREFINT input */
    CMP_PA4,                                                                    /*!< PA4 input */
    CMP_PA5,                                                                    /*!< PA5 input */
    CMP_PA2,                                                                    /*!< PA2 input */
    CMP_PB_2_15                                                                 /*!< PB2 or PB15 input */
}inverting_input_enum;

/* output */
typedef enum
{
    CMP_OUTPUT_NONE = 0x0U,                                                     /*!< output no selection */
    CMP_OUTPUT_TIMER0_BKIN = 0x1U,                                              /*!< TIMER 0 break input */
    CMP_OUTPUT_TIMER2IC2_TIMER1IC1= 0x6U,                                       /*!< TIMER 2 channel2 or TIMER 1 channel1 input */
    CMP_OUTPUT_TIMER0IC0 = 0x7U,                                                /*!< TIMER 0 channel0 input */
    CMP_OUTPUT_TIMER1IC3 = 0x08U,                                               /*!< TIMER 1 channel3 input */
    CMP_OUTPUT_TIMER2IC0 = 0xAU,                                                /*!< TIMER 2 channel0 input*/
}cmp_output_enum;

typedef enum
{
    CMP_OUTPUTBLANK_NONE = 0x0U,                                                /*!< output no blanking */
    CMP_OUTPUTBLANK_TIMER2_IC3 = 0x1U,                                          /*!< select TIMER2_CH3 as blanking source */
    CMP_OUTPUTBLANK_TIMER1IC2 = 0x2U,                                           /*!< select TIMER1_CH2 as blanking source */
    CMP_OUTPUTBLANK_TIMER2IC2_TIMER1IC3 = 0x3U,                                 /*!< select TIMER2_CH2 or TIMER1_CH3 as blanking source */
}cmp_outputblank_enum;

/* comparator inverting input */
#define CS_CMPMSEL(regval)                                                (BITS(4,6) & ((uint32_t)(regval) << 4))
#define CS_CMPMSEL_1_4VREFINT                                             CS_CMPMSEL(0)                   /*!< CMP inverting input 1/4 Vrefint */
#define CS_CMPMSEL_1_2VREFINT                                             CS_CMPMSEL(1)                   /*!< CMP inverting input 1/2 Vrefint */
#define CS_CMPMSEL_3_4VREFINT                                             CS_CMPMSEL(2)                   /*!< CMP inverting input 3/4 Vrefint */
#define CS_CMPMSEL_VREFINT                                                CS_CMPMSEL(3)                   /*!< CMP inverting input Vrefint */
#define CS_CMPMSEL_PA4                                                    CS_CMPMSEL(4)                   /*!< CMP inverting input PA4 */
#define CS_CMPMSEL_PA5                                                    CS_CMPMSEL(5)                   /*!< CMP inverting input PA5 */
#define CS_CMPMSEL_PA2                                                    CS_CMPMSEL(6)                   /*!< CMP inverting input PA2 */
#define CS_CMPMSEL_PB_2_15                                                CS_CMPMSEL(7)                   /*!< CMP inverting input PB2 or PB15 */

/* CMP output */
#define CS_CMPOSEL(regval)                                                (BITS(10,13) & ((uint32_t)(regval) << 10))
#define CS_CMPOSEL_OUTPUT_NONE                                            CS_CMPOSEL(0)                   /*!< CMP output none  */
#define CS_CMPOSEL_OUTPUT_TIMER0_BKIN                                     CS_CMPOSEL(1)                   /*!< CMP output TIMER 0 break input */
#define CS_CMPOSEL_OUTPUT_TIMER2IC2_TIMER1IC1                             CS_CMPOSEL(6)                   /*!< TIMER 2 channel 2 or TIMER 1 channel 1 input capture */
#define CS_CMPOSEL_OUTPUT_TIMER0IC0                                       CS_CMPOSEL(7)                   /*!< CMP output TIMER 0 channle 1 input capture */
#define CS_CMPOSEL_OUTPUT_TIMER1IC3                                       CS_CMPOSEL(8)                   /*!< CMP output TIMER 1 channle 3 input capture */
#define CS_CMPOSEL_OUTPUT_TIMER2IC0                                       CS_CMPOSEL(10)                  /*!< CMP output TIMER 2 channle 0 input capture */

/* output polarity of comparator */
#define CMP_OUTPUT_POLARITY_INVERTED                                      ((uint32_t)0x00000001)          /*!< output is inverted */
#define CMP_OUTPUT_POLARITY_NOINVERTED                                    ((uint32_t)0x00000000)          /*!< output is not inverted */

/* CMP output blanking source */
#define CS_CMPMBLK(regval)                                                (BITS(18,20) & ((uint32_t)(regval) << 18))
#define CS_CMPMBLK_OUTPUTBLANK_NONE                                       CS_CMPMBLK(0)                   /*!< output no blanking */
#define CS_CMPMBLK_OUTPUTBLANK_TIMER2_IC3                                 CS_CMPMBLK(1)                   /*!< select TIMER2_CH3 as blanking source */
#define CS_CMPMBLK_OUTPUTBLANK_TIMER1IC2                                  CS_CMPMBLK(2)                   /*!< select TIMER1_CH2 as blanking source */
#define CS_CMPMBLK_OUTPUTBLANK_TIMER2IC2_TIMER1IC3                        CS_CMPMBLK(3)                   /*!< select TIMER2_CH2 or TIMER1_CH3 as blanking source */

/* CMP output level */
#define CMP_OUTPUTLEVEL_HIGH                                              ((uint32_t)0x00000001)          /*!< comparator output high */
#define CMP_OUTPUTLEVEL_LOW                                               ((uint32_t)0x00000000)          /*!< comparator output low */

/* function declarations */
/* initialization functions */
/* CMP deinit */
void cmp_deinit(void);
/* CMP input init */
void cmp_input_init(cmp_enum cmp_periph,inverting_input_enum inverting_input);
/* CMP output init */
void cmp_output_init(cmp_enum cmp_periph,cmp_output_enum output_slection, uint32_t output_polarity);
/* CMP output blank init */
void cmp_outputblank_init(cmp_enum cmp_periph,cmp_outputblank_enum output_blank);

/* enable CMP */
void cmp_enable(cmp_enum cmp_periph);
/* disable CMP */
void cmp_disable(cmp_enum cmp_periph);
/* lock the CMP */
void cmp_lock_enable(cmp_enum cmp_periph);

/* get output level */
uint32_t cmp_output_level_get(cmp_enum cmp_periph);
#endif /* GD32E50X_CMP_H */
