/**
  ******************************************************************************
  * @file    adc.h
  * @author  YANDLD
  * @version V2.6
  * @date    2015.6.21
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_ADC_H__
#define __CH_LIB_ADC_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* ADC instance */
#define HW_ADC0  (0)
#define HW_ADC1  (1)

#define ADC0_SE1A_PE16      (0x00086020U)
#define ADC0_SE5A_PE17      (0x00286220U)
#define ADC0_SE2A_PE18      (0x00106420U)
#define ADC0_SE6A_PE19      (0x00306620U)
#define ADC0_SE0A_PE20      (0x00006820U)
#define ADC0_SE4A_PE21      (0x00206A20U)
#define ADC0_SE3A_PE22      (0x00186C20U)
#define ADC0_SE7A_PE23      (0x00386E20U)
#define ADC0_SE4B_PE29      (0x01207A20U)
#define ADC0_SE23A_PE30     (0x00B87C20U)
#define ADC0_SE8A_PB00      (0x00404008U)
#define ADC0_SE9A_PB01      (0x00484208U)
#define ADC0_SE12A_PB02     (0x00604408U)
#define ADC0_SE13A_PB03     (0x00684608U)
#define ADC0_SE14A_PC00     (0x00704010U)
#define ADC0_SE15A_PC01     (0x00784210U)
#define ADC0_SE11A_PC02     (0x00584410U)
#define ADC0_SE5B_PD01      (0x01284218U)
#define ADC0_SE6B_PD05      (0x01304A18U)
#define ADC0_SE7B_PD06      (0x01384C18U)

/* ADC speed select */
typedef enum
{
    kADC_SpeedHigh,
    kADC_SpeedMiddle,
    kADC_SpeedLow,
}ADC_Speed_t;

/* ADC trig mode */
typedef enum
{
    TrigSoft,   /* software trigger */
    TrigHard,   /* hardware trigger */
}ADC_Trig_t;


/* ADC hardware ave select */
typedef enum
{
    kADC_Ave1 = 0xFF,
    kADC_Ave4 = 0,
    kADC_Ave8 = 1,
    kADC_Ave16 = 2,
    kADC_Ave32 = 3,
}ADC_Ave_t;

//!< API functions
uint32_t ADC_Init(uint32_t MAP, ADC_Speed_t speed);
int32_t ADC_SoftRead(uint32_t instance, uint32_t chl);
void ADC_SetIntMode(uint32_t instance, bool val);
void ADC_SetTrigMode(uint32_t instance, ADC_Trig_t trig);
void ADC_SetAveMode(uint32_t instance, ADC_Ave_t mod);

#ifdef __cplusplus
}
#endif


#endif
