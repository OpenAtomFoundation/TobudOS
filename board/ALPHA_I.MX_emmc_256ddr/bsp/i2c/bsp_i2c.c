/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_i2c.c
作者	   : 左忠凯
版本	   : V1.0
描述	   : IIC驱动文件。
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/15 左忠凯创建
***************************************************************/
#include "bsp_i2c.h"
#include "bsp_delay.h"
#include "printf.h"

/*
 * @description		: 初始化I2C，波特率100KHZ
 * @param - base 	: 要初始化的IIC设置
 * @return 			: 无
 */
void i2c_init(I2C_Type *base)
{
	/* 1、配置I2C */
	base->I2CR &= ~(1 << 7); /* 要访问I2C的寄存器，首先需要先关闭I2C */

    /* 设置波特率为100K
     * I2C的时钟源来源于IPG_CLK_ROOT=66Mhz
 	 * IC2 时钟 = PERCLK_ROOT/dividison(IFDR寄存器)
	 * 设置寄存器IFDR,IFDR寄存器参考IMX6UL参考手册P1260页，表29-3，
	 * 根据表29-3里面的值，挑选出一个还是的分频数，比如本例程我们
	 * 设置I2C的波特率为100K， 因此当分频值=66000000/100000=660.
	 * 在表29-3里面查找，没有660这个值，但是有640，因此就用640,
	 * 即寄存器IFDR的IC位设置为0X15
	 */
	base->IFDR = 0X15 << 0;

	/*
     * 设置寄存器I2CR，开启I2C
     * bit[7] : 1 使能I2C,I2CR寄存器其他位其作用之前，此位必须最先置1
	 */
	base->I2CR |= (1<<7);
}

/*
 * @description			: 发送重新开始信号
 * @param - base 		: 要使用的IIC
 * @param - addrss		: 设备地址
 * @param - direction	: 方向
 * @return 				: 0 正常 其他值 出错
 */
unsigned char i2c_master_repeated_start(I2C_Type *base, unsigned char address,  enum i2c_direction direction)
{
	/* I2C忙并且工作在从模式,跳出 */
	if(base->I2SR & (1 << 5) && (((base->I2CR) & (1 << 5)) == 0))		
		return 1;

	/*
     * 设置寄存器I2CR
     * bit[4]: 1 发送
     * bit[2]: 1 产生重新开始信号
	 */
	base->I2CR |=  (1 << 4) | (1 << 2);

	/*
     * 设置寄存器I2DR
     * bit[7:0] : 要发送的数据，这里写入从设备地址
     *            参考资料:IMX6UL参考手册P1249
	 */ 
	base->I2DR = ((unsigned int)address << 1) | ((direction == kI2C_Read)? 1 : 0);
	
	return 0;
}

/*
 * @description			: 发送开始信号
 * @param - base 		: 要使用的IIC
 * @param - addrss		: 设备地址
 * @param - direction	: 方向
 * @return 				: 0 正常 其他值 出错
 */
unsigned char i2c_master_start(I2C_Type *base, unsigned char address,  enum i2c_direction direction)
{
	if(base->I2SR & (1 << 5))			/* I2C忙 */
		return 1;

	/*
     * 设置寄存器I2CR
     * bit[5]: 1 主模式
     * bit[4]: 1 发送
	 */
	base->I2CR |=  (1 << 5) | (1 << 4);

	/*
     * 设置寄存器I2DR
     * bit[7:0] : 要发送的数据，这里写入从设备地址
     *            参考资料:IMX6UL参考手册P1249
	 */ 
	base->I2DR = ((unsigned int)address << 1) | ((direction == kI2C_Read)? 1 : 0);
	return 0;
}

/*
 * @description		: 检查并清除错误
 * @param - base 	: 要使用的IIC
 * @param - status	: 状态
 * @return 			: 状态结果
 */
unsigned char i2c_check_and_clear_error(I2C_Type *base, unsigned int status)
{
	/* 检查是否发生仲裁丢失错误 */
	if(status & (1<<4))
	{
		base->I2SR &= ~(1<<4);		/* 清除仲裁丢失错误位 			*/

		base->I2CR &= ~(1 << 7);	/* 先关闭I2C 				*/
		base->I2CR |= (1 << 7);		/* 重新打开I2C 				*/
		return I2C_STATUS_ARBITRATIONLOST;
	} 
	else if(status & (1 << 0))     	/* 没有接收到从机的应答信号 */
	{
		return I2C_STATUS_NAK;		/* 返回NAK(No acknowledge) */
	}
	return I2C_STATUS_OK;
}

/*
 * @description		: 停止信号
 * @param - base	: 要使用的IIC
 * @param			: 无
 * @return 			: 状态结果
 */
unsigned char i2c_master_stop(I2C_Type *base)
{
	unsigned short timeout = 0xffff;

	/*
	 * 清除I2CR的bit[5:3]这三位
	 */
	base->I2CR &= ~((1 << 5) | (1 << 4) | (1 << 3));

	/* 等待忙结束 */
	while((base->I2SR & (1 << 5)))
	{
		timeout--;
		if(timeout == 0)	/* 超时跳出 */
			return I2C_STATUS_TIMEOUT;
	}
	return I2C_STATUS_OK;
}

/*
 * @description		: 发送数据
 * @param - base 	: 要使用的IIC
 * @param - buf		: 要发送的数据
 * @param - size	: 要发送的数据大小
 * @param - flags	: 标志
 * @return 			: 无
 */
void i2c_master_write(I2C_Type *base, const unsigned char *buf, unsigned int size)
{
	/* 等待传输完成 */
	while(!(base->I2SR & (1 << 7))); 
	
	base->I2SR &= ~(1 << 1); 	/* 清除标志位 */
	base->I2CR |= 1 << 4;		/* 发送数据 */
	
	while(size--)
	{
		base->I2DR = *buf++; 	/* 将buf中的数据写入到I2DR寄存器 */
		
		while(!(base->I2SR & (1 << 1))); 	/* 等待传输完成 */	
		base->I2SR &= ~(1 << 1);			/* 清除标志位 */

		/* 检查ACK */
		if(i2c_check_and_clear_error(base, base->I2SR))
			break;
	}
	
	base->I2SR &= ~(1 << 1);
	i2c_master_stop(base); 	/* 发送停止信号 */
}

/*
 * @description		: 读取数据
 * @param - base 	: 要使用的IIC
 * @param - buf		: 读取到数据
 * @param - size	: 要读取的数据大小
 * @return 			: 无
 */
void i2c_master_read(I2C_Type *base, unsigned char *buf, unsigned int size)
{
	volatile uint8_t dummy = 0;

	dummy++; 	/* 防止编译报错 */
	
	/* 等待传输完成 */
	while(!(base->I2SR & (1 << 7))); 
	
	base->I2SR &= ~(1 << 1); 				/* 清除中断挂起位 */
	base->I2CR &= ~((1 << 4) | (1 << 3));	/* 接收数据 */
	
	/* 如果只接收一个字节数据的话发送NACK信号 */
	if(size == 1)
        base->I2CR |= (1 << 3);

	dummy = base->I2DR; /* 假读 */
	
	while(size--)
	{
		while(!(base->I2SR & (1 << 1))); 	/* 等待传输完成 */	
		base->I2SR &= ~(1 << 1);			/* 清除标志位 */

	 	if(size == 0)
        {
        	i2c_master_stop(base); 			/* 发送停止信号 */
        }

        if(size == 1)
        {
            base->I2CR |= (1 << 3);
        }
		*buf++ = base->I2DR;
	}
}

/*
 * @description	: I2C数据传输，包括读和写
 * @param - base: 要使用的IIC
 * @param - xfer: 传输结构体
 * @return 		: 传输结果,0 成功，其他值 失败;
 */
unsigned char i2c_master_transfer(I2C_Type *base, struct i2c_transfer *xfer)
{
	unsigned char ret = 0;
	 enum i2c_direction direction = xfer->direction;	

	base->I2SR &= ~((1 << 1) | (1 << 4));			/* 清除标志位 */

	/* 等待传输完成 */
	while(!((base->I2SR >> 7) & 0X1)){}; 

	/* 如果是读的话，要先发送寄存器地址，所以要先将方向改为写 */
    if ((xfer->subaddressSize > 0) && (xfer->direction == kI2C_Read))
    {
        direction = kI2C_Write;
    }

	ret = i2c_master_start(base, xfer->slaveAddress, direction); /* 发送开始信号 */
    if(ret)
    {	
		return ret;
	}

	while(!(base->I2SR & (1 << 1))){};			/* 等待传输完成 */

    ret = i2c_check_and_clear_error(base, base->I2SR);	/* 检查是否出现传输错误 */
    if(ret)
    {
      	i2c_master_stop(base); 						/* 发送出错，发送停止信号 */
        return ret;
    }
	
    /* 发送寄存器地址 */
    if(xfer->subaddressSize)
    {
        do
        {
			base->I2SR &= ~(1 << 1);			/* 清除标志位 */
            xfer->subaddressSize--;				/* 地址长度减一 */
			
            base->I2DR =  ((xfer->subaddress) >> (8 * xfer->subaddressSize)); //向I2DR寄存器写入子地址
  
			while(!(base->I2SR & (1 << 1)));  	/* 等待传输完成 */

            /* 检查是否有错误发生 */
            ret = i2c_check_and_clear_error(base, base->I2SR);
            if(ret)
            {
             	i2c_master_stop(base); 				/* 发送停止信号 */
             	return ret;
            }  
        } while ((xfer->subaddressSize > 0) && (ret == I2C_STATUS_OK));

        if(xfer->direction == kI2C_Read) 		/* 读取数据 */
        {
            base->I2SR &= ~(1 << 1);			/* 清除中断挂起位 */
            i2c_master_repeated_start(base, xfer->slaveAddress, kI2C_Read); /* 发送重复开始信号和从机地址 */
    		while(!(base->I2SR & (1 << 1))){};/* 等待传输完成 */

            /* 检查是否有错误发生 */
			ret = i2c_check_and_clear_error(base, base->I2SR);
            if(ret)
            {
             	ret = I2C_STATUS_ADDRNAK;
                i2c_master_stop(base); 		/* 发送停止信号 */
                return ret;  
            }
           	          
        }
    }	


    /* 发送数据 */
    if ((xfer->direction == kI2C_Write) && (xfer->dataSize > 0))
    {
    	i2c_master_write(base, xfer->data, xfer->dataSize);
	}

    /* 读取数据 */
    if ((xfer->direction == kI2C_Read) && (xfer->dataSize > 0))
    {
       	i2c_master_read(base, xfer->data, xfer->dataSize);
	}
	return 0;	
}

