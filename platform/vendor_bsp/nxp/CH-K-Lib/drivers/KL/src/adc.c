/**
  ******************************************************************************
  * @file    adc.c
  * @author  YANDLD
  * @version V2.6
  * @date    2015.6.21
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "adc.h"
#include "common.h"


#define kADC_ChlMuxA             (0x00)
#define kADC_ChlMuxB             (0x01)

ADC_Type * const ADC_IPTbl[] = ADC_BASES;

static const Reg_t CLKTbl[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_ADC0_MASK},
};

static const IRQn_Type ADC_IrqTbl[] = 
{
    (IRQn_Type)(ADC0_IRQn+0),
};

void ADC_SetChlMux(uint32_t instance, uint32_t mux)
{
    (kADC_ChlMuxA == mux)?
    (ADC_IPTbl[instance]->CFG2 &= ~ADC_CFG2_MUXSEL_MASK):
    (ADC_IPTbl[instance]->CFG2 |= ADC_CFG2_MUXSEL_MASK); 
}

uint32_t ADC_Init(uint32_t MAP, ADC_Speed_t speed)
{
    uint32_t i;
    map_t * pq = (map_t*)&(MAP);
    ADC_Type* ADCx = ADC_IPTbl[pq->ip];
    
    CLK_EN(CLKTbl, pq->ip);
    /* select the ADACK ,16bit Long sample time */
    ADCx->CFG2 |=  ADC_CFG2_ADACKEN_MASK;
    ADCx->CFG1 = ADC_CFG1_ADICLK(3) | ADC_CFG1_MODE(3) | ADC_CFG1_ADLPC_MASK;
    
    /* use VREH and VREFL as V reference */
    ADCx->SC2 &= ~ADC_SC2_REFSEL_MASK;
    ADCx->SC2 |= ADC_SC2_REFSEL(0);
    
    switch(speed)
    {
        case kADC_SpeedHigh:
            ADCx->CFG1 &= ~ADC_CFG1_ADLSMP_MASK;
            ADCx->CFG2 |= ADC_CFG2_ADHSC_MASK;
            break;
        case kADC_SpeedMiddle:
            ADCx->CFG1 &= ~ADC_CFG1_ADLSMP_MASK;
            ADCx->CFG2 &= ~ADC_CFG2_ADHSC_MASK;
            break;
        case kADC_SpeedLow:
            ADCx->CFG2 &= ~ADC_CFG2_ADHSC_MASK;
            ADCx->CFG1 |= ADC_CFG1_ADLSMP_MASK;
            ADCx->CFG2 |= ADC_CFG2_ADLSTS(0);
            break;
        default:
            break;
    }
    /* pinmux and ADC chl select */
    for(i = 0; i < pq->pin_count; i++)
    {
        SetPinMux(pq->io, pq->pin_start+i, 0);
        SetPinPull(pq->io, pq->pin_start+i, 0xFFFFFFFF);
    }
    ADC_SetChlMux(pq->ip, pq->reserved & 0x01);
    
    ADC_IPTbl[pq->ip]->SC1[0] &= ~(ADC_SC1_ADCH_MASK);
    (pq->reserved & 0x80)?
    (ADC_IPTbl[pq->ip]->SC1[0] |= ADC_SC1_DIFF_MASK):
    (ADC_IPTbl[pq->ip]->SC1[0] &= ~ADC_SC1_DIFF_MASK);
    ADC_IPTbl[pq->ip]->SC1[0] |= ADC_SC1_ADCH(pq->chl);
    
    return pq->ip;
}

void ADC_SetTrigMode(uint32_t instance, ADC_Trig_t trig)
{
    (trig == TrigHard)?
    (ADC_IPTbl[instance]->SC2 |=  ADC_SC2_ADTRG_MASK):
    (ADC_IPTbl[instance]->SC2 &=  ADC_SC2_ADTRG_MASK);
}

int32_t ADC_SoftRead(uint32_t instance, uint32_t chl)
{
    ADC_IPTbl[instance]->SC1[0] &= ~(ADC_SC1_ADCH_MASK);
    ADC_IPTbl[instance]->SC1[0] |= ADC_SC1_ADCH(chl);
    if(!(ADC_IPTbl[instance]->SC1[0] & ADC_SC1_AIEN_MASK))
    {
        while((ADC_IPTbl[instance]->SC1[0] & ADC_SC1_COCO_MASK) == 0);
    }
    return ADC_IPTbl[instance]->R[0];
}

void ADC_SetIntMode(uint32_t instance, bool val)
{
    (val)?
    (ADC_IPTbl[instance]->SC1[0] |= ADC_SC1_AIEN_MASK):
    (ADC_IPTbl[instance]->SC1[0] &= ~ADC_SC1_AIEN_MASK);
    NVIC_EnableIRQ(ADC_IrqTbl[instance]); 
}

void ADC_SetAveMode(uint32_t instance, ADC_Ave_t mod)
{
    ADC_Type* ADCx = ADC_IPTbl[instance];
    
    if(mod == kADC_Ave1)
    {
        ADCx->SC3 &= ~ADC_SC3_AVGE_MASK;
    }
    else
    {
        ADCx->SC3 |= ADC_SC3_AVGE_MASK;
        ADCx->SC3 &= ~ADC_SC3_AVGS_MASK;
        ADCx->SC3 |= ADC_SC3_AVGS(mod);
    }
}

/*
    uint32_t ip_instance:3;
    uint32_t io_instance:3;
    uint32_t mux:3;
    uint32_t io_base:5;
    uint32_t io_offset:5;
    uint32_t channel:5;
    uint32_t reserved:8;
reserved: [bit.7]: 0-SE 1-DIFF
reserved: [bit.0]: 0-A  1-B
const map_t ADC_QuickInitTable[] =
{
    { 0, 4, 0,16, 1, 1, 0}, //ADC0_SE1A_PE16
    { 0, 4, 0,17, 1, 5, 0}, //ADC0_SE5A_PE17
    { 0, 4, 0,18, 1, 2, 0}, //ADC0_SE2A_PE18
    { 0, 4, 0,19, 1, 6, 0}, //ADC0_SE6A_PE19
    { 0, 4, 0,20, 1, 0, 0}, //ADC0_SE0A_PE20
    { 0, 4, 0,21, 1, 4, 0}, //ADC0_SE4A_PE21
    { 0, 4, 0,22, 1, 3, 0}, //ADC0_SE3A_PE22
    { 0, 4, 0,23, 1, 7, 0}, //ADC0_SE7A_PE23
    { 0, 4, 0,29, 1, 4, 1}, //ADC0_SE4B_PE29
    { 0, 4, 0,30, 1,23, 0}, //ADC0_SE23A_PE30
    { 0, 1, 0, 0, 1, 8, 0}, //ADC0_SE8A_PB00
    { 0, 1, 0, 1, 1, 9, 0}, //ADC0_SE9A_PB01
    { 0, 1, 0, 2, 1,12, 0}, //ADC0_SE12A_PB02
    { 0, 1, 0, 3, 1,13, 0}, //ADC0_SE13A_PB03
    { 0, 2, 0, 0, 1,14, 0}, //ADC0_SE14A_PC00
    { 0, 2, 0, 1, 1,15, 0}, //ADC0_SE15A_PC01
    { 0, 2, 0, 2, 1,11, 0}, //ADC0_SE11A_PC02
    { 0, 3, 0, 1, 1, 5, 1}, //ADC0_SE5B_PD01
    { 0, 3, 0, 5, 1, 6, 1}, //ADC0_SE6B_PD05
    { 0, 3, 0, 6, 1, 7, 1}, //ADC0_SE7B_PD06
};
*/

