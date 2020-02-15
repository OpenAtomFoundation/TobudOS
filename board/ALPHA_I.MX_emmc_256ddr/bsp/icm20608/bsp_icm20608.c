/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_icm20608.c
作者	   : 左忠凯
版本	   : V1.0
描述	   : ICM20608驱动文件。
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/3/26 左忠凯创建
***************************************************************/
#include "bsp_icm20608.h"
#include "bsp_delay.h"
#include "bsp_spi.h"
#include "printf.h"

struct icm20608_dev_struc icm20608_dev;	/* icm20608设备 */

/*
 * @description	: 初始化ICM20608
 * @param		: 无
 * @return 		: 0 初始化成功，其他值 初始化失败
 */
unsigned char icm20608_init(void)
{	
	unsigned char regvalue;
	gpio_pin_config_t cs_config;

	/* 1、ESPI3 IO初始化 
 	 * ECSPI3_SCLK 	-> UART2_RXD
 	 * ECSPI3_MISO 	-> UART2_RTS
 	 * ECSPI3_MOSI	-> UART2_CTS
 	 */
	IOMUXC_SetPinMux(IOMUXC_UART2_RX_DATA_ECSPI3_SCLK, 0);
	IOMUXC_SetPinMux(IOMUXC_UART2_CTS_B_ECSPI3_MOSI, 0);
	IOMUXC_SetPinMux(IOMUXC_UART2_RTS_B_ECSPI3_MISO, 0);
	
	/* 配置SPI   SCLK MISO MOSI IO属性	
	 *bit 16: 0 HYS关闭
	 *bit [15:14]: 00 默认100K下拉
	 *bit [13]: 0 keeper功能
	 *bit [12]: 1 pull/keeper使能 
	 *bit [11]: 0 关闭开路输出
 	 *bit [7:6]: 10 速度100Mhz
 	 *bit [5:3]: 110 驱动能力为R0/6
	 *bit [0]: 1 高转换率
 	 */
	IOMUXC_SetPinConfig(IOMUXC_UART2_RX_DATA_ECSPI3_SCLK, 0x10B1);
	IOMUXC_SetPinConfig(IOMUXC_UART2_CTS_B_ECSPI3_MOSI, 0x10B1);
	IOMUXC_SetPinConfig(IOMUXC_UART2_RTS_B_ECSPI3_MISO, 0x10B1);

	
	IOMUXC_SetPinMux(IOMUXC_UART2_TX_DATA_GPIO1_IO20, 0);
	IOMUXC_SetPinConfig(IOMUXC_UART2_TX_DATA_GPIO1_IO20, 0X10B0);
	cs_config.direction = kGPIO_DigitalOutput;
	cs_config.outputLogic = 0;
	gpio_init(GPIO1, 20, &cs_config);
	
	/* 2、初始化SPI */
	spi_init(ECSPI3);	

	icm20608_write_reg(ICM20_PWR_MGMT_1, 0x80);		/* 复位，复位后为0x40,睡眠模式 			*/
	delayms(50);
	icm20608_write_reg(ICM20_PWR_MGMT_1, 0x01);		/* 关闭睡眠，自动选择时钟 					*/
	delayms(50);

	regvalue = icm20608_read_reg(ICM20_WHO_AM_I);
	printf("icm20608 id = %#X\r\n", regvalue);
	if(regvalue != ICM20608G_ID && regvalue != ICM20608D_ID)
		return 1;
		
	icm20608_write_reg(ICM20_SMPLRT_DIV, 0x00); 	/* 输出速率是内部采样率					*/
	icm20608_write_reg(ICM20_GYRO_CONFIG, 0x18); 	/* 陀螺仪±2000dps量程 				*/
	icm20608_write_reg(ICM20_ACCEL_CONFIG, 0x18); 	/* 加速度计±16G量程 					*/
	icm20608_write_reg(ICM20_CONFIG, 0x04); 		/* 陀螺仪低通滤波BW=20Hz 				*/
	icm20608_write_reg(ICM20_ACCEL_CONFIG2, 0x04); 	/* 加速度计低通滤波BW=21.2Hz 			*/
	icm20608_write_reg(ICM20_PWR_MGMT_2, 0x00); 	/* 打开加速度计和陀螺仪所有轴 				*/
	icm20608_write_reg(ICM20_LP_MODE_CFG, 0x00); 	/* 关闭低功耗 						*/
	icm20608_write_reg(ICM20_FIFO_EN, 0x00);		/* 关闭FIFO						*/
	return 0;
}

	
/*
 * @description  : 写ICM20608指定寄存器
 * @param - reg  : 要读取的寄存器地址
 * @param - value: 要写入的值
 * @return		 : 无
 */
void icm20608_write_reg(unsigned char reg, unsigned char value)
{
	/* ICM20608在使用SPI接口的时候寄存器地址
	 * 只有低7位有效,寄存器地址最高位是读/写标志位
	 * 读的时候要为1，写的时候要为0。
	 */
	reg &= ~0X80;	
	
	ICM20608_CSN(0);						/* 使能SPI传输			*/
	spich0_readwrite_byte(ECSPI3, reg); 	/* 发送寄存器地址		*/ 
	spich0_readwrite_byte(ECSPI3, value);	/* 发送要写入的值			*/
	ICM20608_CSN(1);						/* 禁止SPI传输			*/
}	

/*
 * @description	: 读取ICM20608寄存器值
 * @param - reg	: 要读取的寄存器地址
 * @return 		: 读取到的寄存器值
 */
unsigned char icm20608_read_reg(unsigned char reg)
{
	unsigned char reg_val;	   	

	/* ICM20608在使用SPI接口的时候寄存器地址
	 * 只有低7位有效,寄存器地址最高位是读/写标志位
	 * 读的时候要为1，写的时候要为0。
	 */
	reg |= 0x80; 	
	
   	ICM20608_CSN(0);               					/* 使能SPI传输	 		*/
  	spich0_readwrite_byte(ECSPI3, reg);     		/* 发送寄存器地址  		*/ 
  	reg_val = spich0_readwrite_byte(ECSPI3, 0XFF);	/* 读取寄存器的值 			*/
 	ICM20608_CSN(1);                				/* 禁止SPI传输 			*/
  	return(reg_val);               	 				/* 返回读取到的寄存器值 */
}

/*
 * @description	: 读取ICM20608连续多个寄存器
 * @param - reg	: 要读取的寄存器地址
 * @return 		: 读取到的寄存器值
 */
void icm20608_read_len(unsigned char reg, unsigned char *buf, unsigned char len)
{  
	unsigned char i;
	
	/* ICM20608在使用SPI接口的时候寄存器地址，只有低7位有效,
	 * 寄存器地址最高位是读/写标志位读的时候要为1，写的时候要为0。
	 */
	reg |= 0x80; 
		
   	ICM20608_CSN(0);               				/* 使能SPI传输	 		*/
  	spich0_readwrite_byte(ECSPI3, reg);			/* 发送寄存器地址  		*/   	   
 	for(i = 0; i < len; i++)					/* 顺序读取寄存器的值 			*/
 	{
		buf[i] = spich0_readwrite_byte(ECSPI3, 0XFF);	
	}
 	ICM20608_CSN(1);                			/* 禁止SPI传输 			*/
}

/*
 * @description : 获取陀螺仪的分辨率
 * @param		: 无
 * @return		: 获取到的分辨率
 */
float icm20608_gyro_scaleget(void)
{
	unsigned char data;
	float gyroscale;
	
	data = (icm20608_read_reg(ICM20_GYRO_CONFIG) >> 3) & 0X3;
	switch(data) {
		case 0: 
			gyroscale = 131;
			break;
		case 1:
			gyroscale = 65.5;
			break;
		case 2:
			gyroscale = 32.8;
			break;
		case 3:
			gyroscale = 16.4;
			break;
	}
	return gyroscale;
}

/*
 * @description : 获取加速度计的分辨率
 * @param		: 无
 * @return		: 获取到的分辨率
 */
unsigned short icm20608_accel_scaleget(void)
{
	unsigned char data;
	unsigned short accelscale;
	
	data = (icm20608_read_reg(ICM20_ACCEL_CONFIG) >> 3) & 0X3;
	switch(data) {
		case 0: 
			accelscale = 16384;
			break;
		case 1:
			accelscale = 8192;
			break;
		case 2:
			accelscale = 4096;
			break;
		case 3:
			accelscale = 2048;
			break;
	}
	return accelscale;
}


/*
 * @description : 读取ICM20608的加速度、陀螺仪和温度原始值
 * @param 		: 无
 * @return		: 无
 */
void icm20608_getdata(void)
{
	float gyroscale;
	unsigned short accescale;
	unsigned char data[14];
	
	icm20608_read_len(ICM20_ACCEL_XOUT_H, data, 14);
	
	gyroscale = icm20608_gyro_scaleget();
	accescale = icm20608_accel_scaleget();

	icm20608_dev.accel_x_adc = (signed short)((data[0] << 8) | data[1]); 
	icm20608_dev.accel_y_adc = (signed short)((data[2] << 8) | data[3]); 
	icm20608_dev.accel_z_adc = (signed short)((data[4] << 8) | data[5]); 
	icm20608_dev.temp_adc    = (signed short)((data[6] << 8) | data[7]); 
	icm20608_dev.gyro_x_adc  = (signed short)((data[8] << 8) | data[9]); 
	icm20608_dev.gyro_y_adc  = (signed short)((data[10] << 8) | data[11]);
	icm20608_dev.gyro_z_adc  = (signed short)((data[12] << 8) | data[13]);

	/* 计算实际值 */
	icm20608_dev.gyro_x_act = ((float)(icm20608_dev.gyro_x_adc)  / gyroscale) * 100;
	icm20608_dev.gyro_y_act = ((float)(icm20608_dev.gyro_y_adc)  / gyroscale) * 100;
	icm20608_dev.gyro_z_act = ((float)(icm20608_dev.gyro_z_adc)  / gyroscale) * 100;

	icm20608_dev.accel_x_act = ((float)(icm20608_dev.accel_x_adc) / accescale) * 100;
	icm20608_dev.accel_y_act = ((float)(icm20608_dev.accel_y_adc) / accescale) * 100;
	icm20608_dev.accel_z_act = ((float)(icm20608_dev.accel_z_adc) / accescale) * 100;

	icm20608_dev.temp_act = (((float)(icm20608_dev.temp_adc) - 25 ) / 326.8 + 25) * 100;
}


