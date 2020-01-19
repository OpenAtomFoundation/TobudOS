/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_touchscreen.c
作者	   : 左忠凯
版本	   : V1.0
描述	   : 触摸屏驱动文件，触摸芯片为FT5426
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/21 左忠凯创建
***************************************************************/
#include "bsp_touchscreen.h"
#include "bsp_i2c.h"
#include "bsp_int.h"
#include "bsp_delay.h"
#include "printf.h"

struct ft5426_dev_struc ft5426_dev;

/*
 * @description	: 初始化触摸屏，其实就是初始化FT5426
 * @param		: 无
 * @return 		: 无
 */
void touchscreen_init(void)
{
	unsigned char reg_value[2];

	ft5426_dev.initfalg = FT5426_INIT_NOTFINISHED;

	/* 1、初始化IIC2 IO
     * I2C2_SCL -> UART5_TXD
     * I2C2_SDA -> UART5_RXD
     */
	IOMUXC_SetPinMux(IOMUXC_UART5_TX_DATA_I2C2_SCL,1);
	IOMUXC_SetPinMux(IOMUXC_UART5_RX_DATA_I2C2_SDA,1);

	/* 配置I2C2 IO属性	
	 *bit 16:0 HYS关闭
	 *bit [15:14]: 1 默认47K上拉
	 *bit [13]: 1 pull功能
	 *bit [12]: 1 pull/keeper使能 
	 *bit [11]: 0 关闭开路输出
	 *bit [7:6]: 10 速度100Mhz
	 *bit [5:3]: 110 驱动能力为R0/6
	 *bit [0]: 1 高转换率
	 */
	IOMUXC_SetPinConfig(IOMUXC_UART5_TX_DATA_I2C2_SCL,0x70B0);
	IOMUXC_SetPinConfig(IOMUXC_UART5_RX_DATA_I2C2_SDA,0X70B0);
	
	/* 2、初始化触摸屏中断IO和复位IO */
	gpio_pin_config_t ctintpin_config;

	IOMUXC_SetPinMux(IOMUXC_GPIO1_IO09_GPIO1_IO09,0);		/* 复用为GPIO1_IO9 */
	IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER9_GPIO5_IO09,0);/* 复用为GPIO5_IO9 */

	
	IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO09_GPIO1_IO09,0xF080);
	IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER9_GPIO5_IO09,0X10B0);

	/* 中断IO初始化 */
	ctintpin_config.direction = kGPIO_DigitalInput;
	ctintpin_config.interruptMode = kGPIO_IntRisingOrFallingEdge;
	gpio_init(GPIO1, 9, &ctintpin_config);

	GIC_EnableIRQ(GPIO1_Combined_0_15_IRQn);				/* 使能GIC中对应的中断 */
	system_register_irqhandler(GPIO1_Combined_0_15_IRQn, (system_irq_handler_t)gpio1_io9_irqhandler, NULL);	/* 注册中断服务函数 */
	gpio_enableint(GPIO1, 9);								/* 使能GPIO1_IO18的中断功能 */

	/* 复位IO初始化 */
    ctintpin_config.direction=kGPIO_DigitalOutput;	
    ctintpin_config.interruptMode=kGPIO_NoIntmode;	
    ctintpin_config.outputLogic=1;					   
    gpio_init(GPIO5, 9, &ctintpin_config); 

	/* 3、初始化I2C */
	i2c_init(I2C2);	

	/* 4、初始化FT5426 */
	gpio_pinwrite(GPIO5, 9, 0);	/* 复位FT5426 */
	delayms(20);
	gpio_pinwrite(GPIO5, 9, 1); /* 停止复位FT5426 */
	delayms(20);

	ft5426_write_byte(FT5426_ADDR, FT5426_DEVICE_MODE, 0); 	/* 进入正常模式 				*/
	ft5426_write_byte(FT5426_ADDR, FT5426_IDG_MODE, 1); 	/* FT5426中断模式 			*/

	
	ft426_read_len(FT5426_ADDR, FT5426_IDGLIB_VERSION, 2, reg_value);
	printf("Touch Frimware Version:%#X\r\n", ((unsigned short)reg_value[0] << 8) + reg_value[1]);
	
	ft5426_dev.initfalg = FT5426_INIT_FINISHED;	/* 标记FT5426初始化完成 */
	ft5426_dev.intflag = 0;
}

/*
 * @description			: GPIO1_IO9最终的中断处理函数
 * @param				: 无
 * @return 				: 无
 */
void gpio1_io9_irqhandler(void)
{ 
	if(ft5426_dev.initfalg == FT5426_INIT_FINISHED)
	{
		//ft5426_dev.intflag = 1;
		ft5426_read_tpcoord();
	}
	gpio_clearintflags(GPIO1, 9); /* 清除中断标志位 */
}


/*
 * @description	: 向FT5429写入数据
 * @param - addr: 设备地址
 * @param - reg : 要写入的寄存器
 * @param - data: 要写入的数据
 * @return 		: 操作结果
 */
unsigned char ft5426_write_byte(unsigned char addr,unsigned char reg, unsigned char data)
{
    unsigned char status=0;
    unsigned char writedata=data;
    struct i2c_transfer masterXfer;
	
    /* 配置I2C xfer结构体 */
   	masterXfer.slaveAddress = addr; 			/* 设备地址 				*/
    masterXfer.direction = kI2C_Write;			/* 写入数据 				*/
    masterXfer.subaddress = reg;				/* 要写入的寄存器地址 			*/
    masterXfer.subaddressSize = 1;				/* 地址长度一个字节 			*/
    masterXfer.data = &writedata;				/* 要写入的数据 				*/
    masterXfer.dataSize = 1;  					/* 写入数据长度1个字节			*/

    if(i2c_master_transfer(I2C2, &masterXfer))
        status=1;
        
    return status;
}

/*
 * @description	: 从FT5426读取一个字节的数据
 * @param - addr: 设备地址
 * @param - reg : 要读取的寄存器
 * @return 		: 读取到的数据。
 */
unsigned char ft5426_read_byte(unsigned char addr,unsigned char reg)
{
	unsigned char val=0;
	
	struct i2c_transfer masterXfer;	
	masterXfer.slaveAddress = addr;				/* 设备地址 				*/
    masterXfer.direction = kI2C_Read;			/* 读取数据 				*/
    masterXfer.subaddress = reg;				/* 要读取的寄存器地址 			*/
    masterXfer.subaddressSize = 1;				/* 地址长度一个字节 			*/
    masterXfer.data = &val;						/* 接收数据缓冲区 				*/
    masterXfer.dataSize = 1;					/* 读取数据长度1个字节			*/
	i2c_master_transfer(I2C2, &masterXfer);

	return val;
}

/*
 * @description	: 从FT5429读取多个字节的数据
 * @param - addr: 设备地址
 * @param - reg : 要读取的开始寄存器地址
 * @param - len : 要读取的数据长度.
 * @param - buf : 读取到的数据缓冲区
 * @return 		: 无
 */
void ft426_read_len(unsigned char addr,unsigned char reg,unsigned char len,unsigned char *buf)
{	
	struct i2c_transfer masterXfer;	
	
	masterXfer.slaveAddress = addr;				/* 设备地址 				*/
    masterXfer.direction = kI2C_Read;			/* 读取数据 				*/
    masterXfer.subaddress = reg;				/* 要读取的寄存器地址 			*/
    masterXfer.subaddressSize = 1;				/* 地址长度一个字节 			*/
    masterXfer.data = buf;						/* 接收数据缓冲区 				*/
    masterXfer.dataSize = len;					/* 读取数据长度1个字节			*/
	i2c_master_transfer(I2C2, &masterXfer);
} 

/*
 * @description	: 读取当前触摸点个数
 * @param 		: 无
 * @return 		: 无
 */
void ft5426_read_tpnum(void)
{
	ft5426_dev.point_num = ft5426_read_byte(FT5426_ADDR, FT5426_TD_STATUS);
}

/*
 * @description	: 读取当前所有触摸点的坐标
 * @param 		: 无
 * @return 		: 无
 */
void ft5426_read_tpcoord(void)
{
	unsigned char i = 0;
	unsigned char type = 0;
	//unsigned char id = 0;
	unsigned char pointbuf[FT5426_XYCOORDREG_NUM];
	
	ft5426_dev.point_num = ft5426_read_byte(FT5426_ADDR, FT5426_TD_STATUS);

	/*
  	 * 从寄存器FT5426_TOUCH1_XH开始，连续读取30个寄存器的值，这30个寄存器
  	 * 保存着5个点的触摸值，每个点占用6个寄存器。
	 */
	ft426_read_len(FT5426_ADDR, FT5426_TOUCH1_XH, FT5426_XYCOORDREG_NUM, pointbuf);
		
	for(i = 0; i < ft5426_dev.point_num ; i++)
	{
		unsigned char *buf = &pointbuf[i * 6];
		/* 以第一个触摸点为例，寄存器TOUCH1_XH(地址0X03),各位描述如下：
		 * bit7:6  Event flag  0:按下 1:释放 2：接触 3：没有事件
		 * bit5:4  保留
		 * bit3:0  X轴触摸点的11~8位。
		 */
		ft5426_dev.x[i] = ((buf[2] << 8) | buf[3]) & 0x0fff;
		ft5426_dev.y[i] = ((buf[0] << 8) | buf[1]) & 0x0fff;
		
		type = buf[0] >> 6;	/* 获取触摸类型 */
		

		/* 以第一个触摸点为例，寄存器TOUCH1_YH(地址0X05),各位描述如下：
		 * bit7:4  Touch ID  触摸ID，表示是哪个触摸点
		 * bit3:0  Y轴触摸点的11~8位。
		 */
		//id = (buf[2] >> 4) & 0x0f;
		
		if(type == FT5426_TOUCH_EVENT_DOWN || type == FT5426_TOUCH_EVENT_ON )/* 按下 	*/
		{
		
		} else  {	/* 释放 */	
			
		}
	}	
}

