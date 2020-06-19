#ifndef __DRV_ADC_MEASURE_H__
#define __DRV_ADC_MEASURE_H__

#include <stdint.h>


void drv_adc_init(void);
void drv_adc_trigger(void);
uint16_t drv_adc_read(uint8_t ch);



#endif // __DRV_ADC_MEASURE_H__
