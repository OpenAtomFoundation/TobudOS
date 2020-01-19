/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_backlight.c
作者	   : 左忠凯
版本	   : V1.0
描述	   : LCD背光PWM驱动文件。
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/22 左忠凯创建
***************************************************************/
#include "bsp_backlight.h"
#include "bsp_int.h"
#include "printf.h"



/* 背光设备 */
struct backlight_dev_struc backlight_dev;

/*
 * @description	: pwm1中断处理函数
 * @param		: 无
 * @return 		: 无
 */
void pwm1_irqhandler(void)
{

	if(PWM1->PWMSR & (1 << 3)) 	/* FIFO为空中断 */
	{
		/* 将占空比信息写入到FIFO中,其实就是设置占空比 */
		pwm1_setduty(backlight_dev.pwm_duty); 
		PWM1->PWMSR |= (1 << 3); /* 写1清除中断标志位 */ 
	}
}

/*
 * @description	: 初始化背光PWM
 * @param		: 无
 * @return 		: 无
 */
void backlight_init(void)
{
	unsigned char i = 0;
	
	/* 1、背光PWM IO初始化 */
	IOMUXC_SetPinMux(IOMUXC_GPIO1_IO08_PWM1_OUT, 0); /* 复用为PWM1_OUT */

	/* 配置PWM IO属性	
	 *bit 16:0 HYS关闭
	 *bit [15:14]: 10 100K上拉
	 *bit [13]: 1 pull功能
	 *bit [12]: 1 pull/keeper使能
	 *bit [11]: 0 关闭开路输出
	 *bit [7:6]: 10 速度100Mhz
	 *bit [5:3]: 010 驱动能力为R0/2
	 *bit [0]: 0 低转换率
	 */
	IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO08_PWM1_OUT, 0XB090);
	
	/* 2、初始化PWM1		*/
	/*
   	 * 初始化寄存器PWMCR
   	 * bit[27:26]	: 01  当FIFO中空余位置大于等于2的时候FIFO空标志值位
   	 * bit[25]		:
 0  停止模式下PWM不工作
   	 * bit[24]		: 0	  休眠模式下PWM不工作
   	 * bit[23]		: 0   等待模式下PWM不工作
   	 * bit[22]		: 0   调试模式下PWM不工作
   	 * bit[21]		: 0   关闭字节交换
   	 * bit[20]		: 0	  关闭半字数据交换
   	 * bit[19:18]	: 00  PWM输出引脚在计数器重新计数的时候输出高电平
   	 *					  在计数器计数值达到比较值以后输出低电平
   	 * bit[17:16]	: 01  PWM时钟源选择IPG CLK = 66MHz
   	 * bit[15:4]	: 65  分频系数为65+1=66，PWM时钟源 = 66MHZ/66=1MHz
   	 * bit[3]		: 0	  PWM不复位
   	 * bit[2:1]		: 00  FIFO中的sample数据每个只能使用一次。
   	 * bit[0]		: 0   先关闭PWM，后面再使能
	 */
	PWM1->PWMCR = 0;	/* 寄存器先清零 */
	PWM1->PWMCR |= (1 << 26) | (1 << 16) | (65 << 4);

	/* 设置PWM周期为1000,那么PWM频率就是1M/1000 = 1KHz。 */
	pwm1_setperiod_value(1000);

	/* 设置占空比，默认50%占空比   ,写四次是因为有4个FIFO */
	backlight_dev.pwm_duty = 50;
	for(i = 0; i < 4; i++)
	{
		pwm1_setduty(backlight_dev.pwm_duty);	
	}
	
	/* 使能FIFO空中断，设置寄存器PWMIR寄存器的bit0为1 */
	PWM1->PWMIR |= 1 << 0;
	system_register_irqhandler(PWM1_IRQn, (system_irq_handler_t)pwm1_irqhandler, NULL);	/* 注册中断服务函数 */
	GIC_EnableIRQ(PWM1_IRQn);	/* 使能GIC中对应的中断 */
	PWM1->PWMSR = 0;			/* PWM中断状态寄存器清零 */
	
	pwm1_enable();				/* 使能PWM1 */

	
}

/*
 * @description	: 使能PWM
 * @param		: 无
 * @return 		: 无
 */
void pwm1_enable(void)
{
	PWM1->PWMCR |= 1 << 0;	 
}

/*
 * @description		: 设置Sample寄存器，Sample数据会写入到FIFO中，
 * 					  所谓的Sample寄存器，就相当于比较寄存器，假如PWMCR中的POUTC
 *				  	  设置为00的时候。当PWM计数器中的计数值小于Sample的时候
 *					  就会输出高电平，当PWM计数器值大于Sample的时候输出底电平,
 *					  因此可以通过设置Sample寄存器来设置占空比
 * @param -  value	: 寄存器值，范围0~0XFFFF
 * @return 			: 无
 */
void pwm1_setsample_value(unsigned int value)
{
	PWM1->PWMSAR = (value & 0XFFFF);	
}

/*
 * @description		: 设置PWM周期，就是设置寄存器PWMPR，PWM周期公式如下
 *					  PWM_FRE = PWM_CLK / (PERIOD + 2)， 比如当前PWM_CLK=1MHz
 *					  要产生1KHz的PWM，那么PERIOD = 1000000/1K - 2 = 	998
 * @param -  value	: 周期值，范围0~0XFFFF
 * @return 			: 无
 */
void pwm1_setperiod_value(unsigned int value)
{
	unsigned int regvalue = 0;

	if(value < 2)
		regvalue = 2;
	else 
		regvalue = value - 2;
	PWM1->PWMPR = (regvalue & 0XFFFF);
}

/*
 * @description		: 设置PWM占空比
 * @param -  value	: 占空比0~100，对应0%~100%
 * @return 			: 无
 */
void pwm1_setduty(unsigned char duty)
{
	unsigned short preiod;
	unsigned short sample;

	backlight_dev.pwm_duty = duty;
	preiod = PWM1->PWMPR + 2;
	sample = preiod * backlight_dev.pwm_duty / 100;
	pwm1_setsample_value(sample);
}

