#ifndef	_HTS221_H_
#define	_HTS221_H_

#include <stm32l0xx_hal.h>

#define HTS221_ADDR_WR	0xBE
#define HTS221_ADDR_RD	0xBF

#define HTS221_CTRL_REG1	0x20
#define HTS221_CTRL_REG2	0x21
#define HTS221_CTRL_REG3	0x22

#define HTS221_STATUS_REG	0x27

#define HTS221_HUMIDITY_OUT_L	0x28
#define HTS221_HUMIDITY_OUT_H	0x29
#define HTS221_TEMP_OUT_L	0x2A
#define HTS221_TEMP_OUT_H	0x2B

void HTS221_Init(void);

uint8_t	HTS221_Get_Temperature(int16_t* temperature);
uint8_t	HTS221_Get_Humidity(int16_t* humidity);

#endif /*  _HTS221_H_ */
