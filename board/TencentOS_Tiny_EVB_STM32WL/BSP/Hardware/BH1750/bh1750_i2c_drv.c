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

#include "bh1750_i2c_drv.h"
#include "i2c.h"

static int bh1750_send_cmd(BH1750_MODE cmd)
{
    return HAL_I2C_Master_Transmit(&hi2c2, BH1750_ADDR_WRITE, (uint8_t*)&cmd, 1, 0xFFFF);
}

static int bh1750_recv_dat(uint8_t* dat)
{
	return HAL_I2C_Master_Receive(&hi2c2, BH1750_ADDR_READ, dat, 2, 0xFFFF);
}

static uint16_t bh1750_dat2lux(uint8_t* dat)
{
		uint16_t lux = 0;
		lux = dat[0];
		lux <<= 8;
		lux += dat[1];
		lux = (int)(lux / 1.2);
	
		return lux;
}

int bh1750_read_data_once(uint16_t *lux)
{
    uint8_t dat[2] = {0};
    
    if (bh1750_send_cmd(ONCE_H_MODE) != HAL_OK) {
        return -1;
    }
    
    HAL_Delay(200);
    
    if (bh1750_recv_dat(dat) != HAL_OK) {
        return -1;
    }
    
    *lux = bh1750_dat2lux(dat);
    
    return 0;
}
