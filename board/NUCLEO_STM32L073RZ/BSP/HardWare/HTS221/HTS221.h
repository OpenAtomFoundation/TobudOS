#ifndef	_HTS221_H_
#define	_HTS221_H_

#include <stm32l0xx_hal.h>

#define HTS221_ADDR_WR	0xBE
#define HTS221_ADDR_RD	0xBF

#define HTS221_CTRL_REG1	0x20
#define HTS221_CTRL_REG2	0x21
#define HTS221_CTRL_REG3	0x22

#define HTS221_AV_CONF          0x10
#define HTS221_STATUS_REG	0x27

#define HTS221_HUMIDITY_OUT_L	0x28
#define HTS221_HUMIDITY_OUT_H	0x29
#define HTS221_TEMP_OUT_L	0x2A
#define HTS221_TEMP_OUT_H	0x2B

#define HTS221_H0_rH_x2         0x30
#define HTS221_H1_rH_x2         0x31
#define HTS221_H0_T0_OUT_L      0x36
#define HTS221_H0_T0_OUT_H      0x37
#define HTS221_H1_T0_OUT_L      0x3A
#define HTS221_H1_T0_OUT_H      0x3B

#define HTS221_T0_degC_x8       0x32
#define HTS221_T1_degC_x8       0x33
#define HTS221_T1_T0_msb        0x35
#define HTS221_T0_OUT_L         0x3C
#define HTS221_T0_OUT_H         0x3D
#define HTS221_T1_OUT_L         0x3E
#define HTS221_T1_OUT_H         0x3F

/** 
  * @brief  temperature & humidity sensor structures definition  
  * added by jieranzhi, 2020/03/29
  */ 
typedef struct
{
  uint8_t   temp_sensitivity;
  int16_t   temperature;
  uint16_t  humi_sensitivity;
  int16_t   humidity;
}sensor_tempnhumi_t;

/** 
  * @brief  STATUS structures definition  
  */ 
typedef enum
{ 
  HTS221_T_DA  = 0x01U,  /*!< temperature data available    */
  HTS221_H_DA  = 0x02U,  /*!< humidity data available       */ 
}HTS221_StatusTypeDef;

void HTS221_Init(void);
uint8_t	HTS221_Get_Temperature(int16_t* temperature);
uint8_t	HTS221_Get_Humidity(int16_t* humidity);
uint8_t	HTS221_Get_TemperatureAndHumidity(sensor_tempnhumi_t* tempnhumi_sensor);

#endif /*  _HTS221_H_ */
