#include "hal/hal_adc.h"
#include "drv_adc_measure.h"

void drv_adc_init(void)
{
    ADC_InitTypeDef adc_init_struct;
    adc_init_struct.ADC_Autoff             = FDISABLE;
    adc_init_struct.ADC_ContinuousConvMode = FENABLE;
    adc_init_struct.ADC_DataAlign          = ADC_DataAlign_Right;
    adc_init_struct.ADC_WaitMode           = FDISABLE;

    HAL_ADC_Init(ADC, &adc_init_struct);
    HAL_ADC_PrescCfg(ADC, 42);//255: 256*2
    HAL_ADC_SeqChanSelect_Cfg(ADC, ADC_CHAN_0);

    HAL_ADC_Cmd(ADC, FENABLE);
    HAL_ADC_SoftwareStartConvCmd(ADC);
}

uint16_t drv_adc_read(uint8_t ch)
{
    uint16_t read_adc = 0;

    for(volatile uint32_t t = 0; t < 40*3; t++);//delay xx us

    read_adc = HAL_ADC_GetConversionValue(ADC, ch);

    return read_adc;
}


