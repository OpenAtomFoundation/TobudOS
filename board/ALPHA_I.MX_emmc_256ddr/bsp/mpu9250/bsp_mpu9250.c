/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_mpu9250.c
作者	   : 左忠凯
版本	   : V1.0
描述	   : MPU9250驱动文件。
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/15 左忠凯创建
***************************************************************/
#include "bsp_mpu9250.h"
#include "bsp_i2c.h"
#include "bsp_delay.h"
#include "printf.h"


/*
 * @description	: 初始化MPU9250
 * @param		: 无
 * @return 		: 0 成功，其他值 错误代码
 */
unsigned char mpu9250_init(void)
{
	unsigned char data = 0;
	
	i2c_init();		/* 初始化IIC */
    mpu_write_byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X80);//复位MPU9250
    delayms(100);  //延时100ms
    mpu_write_byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X00);//唤醒MPU9250
    mpu_set_gyro_range(3);					        	//陀螺仪传感器,±2000dps
 	mpu_set_accel_range(0);					       	 	//加速度传感器,±2g
    mpu_set_rate(50);						       	 	//设置采样率50Hz
    mpu_write_byte(MPU9250_ADDR,MPU_INT_EN_REG,0X00);   //关闭所有中断
	mpu_write_byte(MPU9250_ADDR,MPU_USER_CTRL_REG,0X00);//I2C主模式关闭
	mpu_write_byte(MPU9250_ADDR,MPU_FIFO_EN_REG,0X00);	//关闭FIFO
	mpu_write_byte(MPU9250_ADDR,MPU_INTBP_CFG_REG,0X82);//INT引脚低电平有效，开启bypass模式，可以直接读取磁力计

	data = mpu_read_byte(MPU9250_ADDR,MPU_DEVICE_ID_REG);
	if(data == MPU6500_ID)
	{
		printf("MPU9250 ID=%#X\r\n", data);
		mpu_write_byte(MPU9250_ADDR, MPU_PWR_MGMT1_REG, 0X01);  /* 设置CLKSEL,PLL X轴为参考 */
        mpu_write_byte(MPU9250_ADDR, MPU_PWR_MGMT2_REG, 0X00);  /* 加速度与陀螺仪都工作 				*/
        mpu_set_rate(50);						       			/* 设置采样率为50Hz   */
	}
	else {
		return 1;
	
	}
	
    data = mpu_read_byte(AK8963_ADDR, MAG_WIA);    				/* 读取AK8963 ID    */  
	if(data == AK8963_ID)
    {
        printf("AK8963_ID=%#X\r\n", data);
        mpu_write_byte(AK8963_ADDR, MAG_CNTL1, 0X11);			/* 设置AK8963为单次测量模式 */
    }else return 2;

	return 0;
}

/*
 * @description	: 向MPU9250写入数据
 * @param - addr: 设备地址
 * @param - reg : 要写入的寄存器
 * @param - data: 要写入的数据
 * @return 		: 操作结果
 */
unsigned char mpu_write_byte(unsigned char addr,unsigned char reg, unsigned char data)
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

    if(i2c_master_transfer(&masterXfer))
        status=1;
        
    return status;
}

/*
 * @description	: 从MPU9250读取一个字节的数据
 * @param - addr: 设备地址
 * @param - reg : 要读取的寄存器
 * @return 		: 读取到的数据。
 */
unsigned char mpu_read_byte(unsigned char addr,unsigned char reg)
{
	unsigned char val=0;
	
	struct i2c_transfer masterXfer;	
	masterXfer.slaveAddress = addr;				/* 设备地址 				*/
    masterXfer.direction = kI2C_Read;			/* 读取数据 				*/
    masterXfer.subaddress = reg;				/* 要读取的寄存器地址 			*/
    masterXfer.subaddressSize = 1;				/* 地址长度一个字节 			*/
    masterXfer.data = &val;						/* 接收数据缓冲区 				*/
    masterXfer.dataSize = 1;					/* 读取数据长度1个字节			*/
	i2c_master_transfer(&masterXfer);

	return val;
}

/*
 * @description	: 从MPU9250读取多个字节的数据
 * @param - addr: 设备地址
 * @param - reg : 要读取的开始寄存器地址
 * @param - len : 要读取的数据长度.
 * @param - buf : 读取到的数据缓冲区
 * @return 		: 无
 */
void mpu_read_len(unsigned char addr,unsigned char reg,unsigned char len,unsigned char *buf)
{	
	struct i2c_transfer masterXfer;	
	
	masterXfer.slaveAddress = addr;				/* 设备地址 				*/
    masterXfer.direction = kI2C_Read;			/* 读取数据 				*/
    masterXfer.subaddress = reg;				/* 要读取的寄存器地址 			*/
    masterXfer.subaddressSize = 1;				/* 地址长度一个字节 			*/
    masterXfer.data = buf;						/* 接收数据缓冲区 				*/
    masterXfer.dataSize = len;					/* 读取数据长度1个字节			*/
	i2c_master_transfer(&masterXfer);
} 

/*
 * @description		: 设置MPU9250陀螺仪量程
 * @param - range	: 量程范围,0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
 * @return 			: 操作结果 0,设置成功  其他,设置失败 
 */
unsigned char mpu_set_gyro_range(unsigned char range)
{
	return mpu_write_byte(MPU9250_ADDR, MPU_GYRO_CFG_REG, range << 3);//设置陀螺仪满量程范围  
}

/*
 * @description		: 设置MPU9250加速度量程
 * @param - range	: 量程范围,0,±2g;1,±4g;2,±8g;3,±16g
 * @return 			: 操作结果 0,设置成功  其他,设置失败 
 */
unsigned char mpu_set_accel_range(unsigned char range)
{
	return mpu_write_byte(MPU9250_ADDR, MPU_ACCEL_CFG_REG,range << 3);//设置加速度传感器满量程范围  
}

/*
 * @description		: 设置MPU9250的数字低通滤波器
 * @param - range	: 数字低通滤波频率(Hz)
 * @return 			: 操作结果 0,设置成功  其他,设置失败 
 */
unsigned char mpu_set_lpf(unsigned short fre)
{
	unsigned char data = 0;
	
	if(fre >= 188) data=1;
	else if(fre >= 98) data=2;
	else if(fre >= 42) data=3;
	else if(fre >= 20) data=4;
	else if(fre >= 10) data=5;
	else data = 6; 
	return mpu_write_byte(MPU9250_ADDR,MPU_CFG_REG,data);
}

/*
 * @description	: 设置MPU9250采样率(假定Fs=1KHz)
 * @param - rate: 4~1000(Hz)
 * @return 		: 操作结果 0,设置成功  其他,设置失败 
 */
unsigned char mpu_set_rate(unsigned short rate)
{
	unsigned char data;
	if(rate > 1000) rate = 1000;
	if(rate < 4) rate = 4;
	data = 1000 / rate - 1;
	data = mpu_write_byte(MPU9250_ADDR,MPU_SAMPLE_RATE_REG,data);	/* 设置数字低通滤波器 */
 	return mpu_set_lpf(rate / 2);	/* 自动设置LPF为采样率的一半 */
}

/*
 * @description	: 得到陀螺仪值(原始值)
 * @param - gx  : 陀螺仪X轴原始数据
 * @param - gy	: 陀螺仪Y轴原始数据
 * @param - gz 	：陀螺仪Z轴原始数据
 * @return 		:  0,设置成功  其他,设置失败 
 */
void mpu_get_gyroscope(short *gx,short *gy,short *gz)
{
    unsigned char buf[6]; 
	
	mpu_read_len(MPU9250_ADDR, MPU_GYRO_XOUTH_REG, 6, buf);
	*gx=((short)buf[0] << 8) | buf[1];  
	*gy=((short)buf[2] << 8) | buf[3];  
	*gz=((short)buf[4] << 8) | buf[5];
}

/*
 * @description	: 得到加速度原始值
 * @param - ax  : 加速度X轴原始数据
 * @param - ay	: 加速度Y轴原始数据
 * @param - az 	：加速度Z轴原始数据
 * @return 		:  0,设置成功  其他,设置失败 
 */
void mpu_get_accelerometer(short *ax,short *ay,short *az)
{
    unsigned char buf[6]; 
	
	mpu_read_len(MPU9250_ADDR, MPU_ACCEL_XOUTH_REG, 6, buf);
	*ax=((short)buf[0] << 8) | buf[1];  
	*ay=((short)buf[2] << 8) | buf[3];  
	*az=((short)buf[4] << 8) | buf[5];	
}

/*
 * @description	: 得到原始磁力计值
 * @param - mx  : 磁力计X轴原始数据
 * @param - my	: 磁力计Y轴原始数据
 * @param - mz 	：磁力计Z轴原始数据
 * @return 		:  0,设置成功  其他,设置失败 
 */
void mpu_get_magnetometer(short *mx, short *my, short *mz)
{
   	unsigned char buf[6];  
	
	mpu_read_len(AK8963_ADDR, MAG_XOUT_L , 6, buf);
	*mx = ((short)buf[1] << 8) | buf[0];  
	*my = ((short)buf[3] << 8) | buf[2];  
	*mz = ((short)buf[5] << 8) | buf[4];
	
    mpu_write_byte(AK8963_ADDR, MAG_CNTL1, 0X11); /* AK8963每次读完以后都需要重新设置为单次测量模式 */
}

