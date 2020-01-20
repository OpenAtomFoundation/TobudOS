/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_ap3216c.c
作者	   : 左忠凯
版本	   : V1.0
描述	   : AP3216C驱动文件。
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/3/26 左忠凯创建
***************************************************************/
#include "bsp_ap3216c.h"
#include "bsp_i2c.h"
#include "bsp_delay.h"
#include "cc.h"
#include "printf.h"

/*
 * @description	: 初始化AP3216C
 * @param		: 无
 * @return 		: 0 成功，其他值 错误代码
 */
unsigned char ap3216c_init(void)
{
	unsigned char data = 0;

	/* 1、IO初始化，配置I2C IO属性	
     * I2C1_SCL -> UART4_TXD
     * I2C1_SDA -> UART4_RXD
     */
	IOMUXC_SetPinMux(IOMUXC_UART4_TX_DATA_I2C1_SCL, 1);
	IOMUXC_SetPinMux(IOMUXC_UART4_RX_DATA_I2C1_SDA, 1);

	/* 
	 *bit 16:0 HYS关闭
	 *bit [15:14]: 1 默认47K上拉
	 *bit [13]: 1 pull功能
	 *bit [12]: 1 pull/keeper使能 
	 *bit [11]: 0 关闭开路输出
	 *bit [7:6]: 10 速度100Mhz
	 *bit [5:3]: 110 驱动能力为R0/6
	 *bit [0]: 1 高转换率
	 */
	IOMUXC_SetPinConfig(IOMUXC_UART4_TX_DATA_I2C1_SCL, 0x70B0);
	IOMUXC_SetPinConfig(IOMUXC_UART4_RX_DATA_I2C1_SDA, 0X70B0);

	i2c_init(I2C1);		/* 初始化I2C1 */

	/* 2、初始化AP3216C */
	ap3216c_writeonebyte(AP3216C_ADDR, AP3216C_SYSTEMCONG, 0X04);	/* 复位AP3216C 			*/
	delayms(50);													/* AP33216C复位至少10ms */
	ap3216c_writeonebyte(AP3216C_ADDR, AP3216C_SYSTEMCONG, 0X03);	/* 开启ALS、PS+IR 		   	*/
	data = ap3216c_readonebyte(AP3216C_ADDR, AP3216C_SYSTEMCONG);	/* 读取刚刚写进去的0X03 */
	if(data == 0X03)
		return 0;	/* AP3216C正常 	*/
	else 
		return 1;	/* AP3216C失败 	*/
}

/*
 * @description	: 向AP3216C写入数据
 * @param - addr: 设备地址
 * @param - reg : 要写入的寄存器
 * @param - data: 要写入的数据
 * @return 		: 操作结果
 */
unsigned char ap3216c_writeonebyte(unsigned char addr,unsigned char reg, unsigned char data)
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

    if(i2c_master_transfer(I2C1, &masterXfer))
        status=1;
        
    return status;
}

/*
 * @description	: 从AP3216C读取一个字节的数据
 * @param - addr: 设备地址
 * @param - reg : 要读取的寄存器
 * @return 		: 读取到的数据。
 */
unsigned char ap3216c_readonebyte(unsigned char addr,unsigned char reg)
{
	unsigned char val=0;
	
	struct i2c_transfer masterXfer;	
	masterXfer.slaveAddress = addr;				/* 设备地址 				*/
    masterXfer.direction = kI2C_Read;			/* 读取数据 				*/
    masterXfer.subaddress = reg;				/* 要读取的寄存器地址 			*/
    masterXfer.subaddressSize = 1;				/* 地址长度一个字节 			*/
    masterXfer.data = &val;						/* 接收数据缓冲区 				*/
    masterXfer.dataSize = 1;					/* 读取数据长度1个字节			*/
	i2c_master_transfer(I2C1, &masterXfer);

	return val;
}

/*
 * @description	: 读取AP3216C的数据，读取原始数据，包括ALS,PS和IR, 注意！
 *				: 如果同时打开ALS,IR+PS的话两次数据读取的时间间隔要大于112.5ms
 * @param - ir	: ir数据
 * @param - ps 	: ps数据
 * @param - ps 	: als数据 
 * @return 		: 无。
 */
void ap3216c_readdata(unsigned short *ir, unsigned short *ps, unsigned short *als)
{
    unsigned char buf[6];
    unsigned char i;

	/* 循环读取所有传感器数据 */
    for(i = 0; i < 6; i++)	
    {
        buf[i] = ap3216c_readonebyte(AP3216C_ADDR, AP3216C_IRDATALOW + i);	
    }
	
    if(buf[0] & 0X80) 	/* IR_OF位为1,则数据无效 */
		*ir = 0;					
	else 				/* 读取IR传感器的数据   		*/
		*ir = ((unsigned short)buf[1] << 2) | (buf[0] & 0X03); 			
	
	*als = ((unsigned short)buf[3] << 8) | buf[2];	/* 读取ALS传感器的数据 			 */  
	
    if(buf[4] & 0x40)	/* IR_OF位为1,则数据无效 			*/
		*ps = 0;    													
	else 				/* 读取PS传感器的数据    */
		*ps = ((unsigned short)(buf[5] & 0X3F) << 4) | (buf[4] & 0X0F); 	
}

