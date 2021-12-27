/**
 * @Copyright 			(c) 2019,mculover666 All rights reserved	
 * @filename  			bh1750_i2c_drv.h
 * @breif				Drive BH1750 based on iic1 commucation interface
 * @version
 *            			v1.0    完成基本驱动程序              mculover666    2019/7/15
 * @note            	移植说明（非常重要）：	
 *						1. bh1750_i2c_drv.h文件中的BH1750读写地址即可；
 * 						2. 此驱动程序需要STM32CubeMX生成的I2C初始化文件i2c.h和i2c.c支持;
 * 						3. 每次写完指令之后需要延时再读取数据（大于等于该模式的测量时间）。
 */
#ifndef	_BH1750_I2C_DRV_H_
#define	_BH1750_I2C_DRV_H_

#include "stm32wlxx_hal.h"

#define	BH1750_ADDR_WRITE	0x46	//01000110
#define	BH1750_ADDR_READ	0x47	//01000111

typedef enum
{
		POWER_OFF_CMD				=	0x00,	//断电：无激活状态
		POWER_ON_CMD				=	0x01,	//通电：等待测量指令
		RESET_REGISTER				=	0x07,	//重置数字寄存器（在断电状态下不起作用）
		CONT_H_MODE					=	0x10,	//连续H分辨率模式：在11x分辨率下开始测量，测量时间120ms
		CONT_H_MODE2				=	0x11,	//连续H分辨率模式2：在0.51x分辨率下开始测量，测量时间120ms
		CONT_L_MODE					=	0x13,	//连续L分辨率模式：在411分辨率下开始测量，测量时间16ms
		ONCE_H_MODE					=	0x20,	//一次高分辨率模式：在11x分辨率下开始测量，测量时间120ms，测量后自动设置为断电模式
		ONCE_H_MODE2				=	0x21,	//一次高分辨率模式2：在0.51x分辨率下开始测量，测量时间120ms，测量后自动设置为断电模式
		ONCE_L_MODE					=	0x23	//一次低分辨率模式：在411x分辨率下开始测量，测量时间16ms，测量后自动设置为断电模式
} BH1750_MODE;

int bh1750_read_data_once(uint16_t *lux);

#endif	/* _AT24C02_I2C_DRV_H_ */
