#ifndef __XTAL_CAPBANK_TAB_H__
#define __XTAL_CAPBANK_TAB_H__

#include <stdint.h>


typedef struct {
    int16_t adc_bottom;
    int16_t adc_top;
    uint8_t capbank; 
} xtal_capbank_t;

#define XTAL_ADC_UP_HYSTERESIS     (10)  //1C = 2.8LSB
#define XTAL_ADC_DOWN_HYSTERESIS   (10)

#define TEMP_MIN_ADC_VAL      (0)
#define TEMP_0_ADC_VAL        (735)
#define TEMP_80_ADC_VAL       (905)
#define TEMP_MAX_ADC_VAL      (4095)

xtal_capbank_t g_xtal_capbank_tab[] = {
    {TEMP_MIN_ADC_VAL, TEMP_0_ADC_VAL,   8, },
    {TEMP_0_ADC_VAL,   TEMP_80_ADC_VAL,  11,},
    {TEMP_80_ADC_VAL,  TEMP_MAX_ADC_VAL, 18,},
};

typedef struct {
    uint8_t last_sect;
    uint8_t curr_sect;
    int16_t last_adc; 
} xtal_temp_cal_t;




#endif //__XTAL_CAPBANK_TAB_H__


