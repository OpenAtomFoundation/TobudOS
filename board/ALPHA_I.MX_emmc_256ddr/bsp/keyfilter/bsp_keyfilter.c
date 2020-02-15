/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_keyfilter.c
作者	   : 左忠凯
版本	   : V1.0
描述	   : 定时器按键消抖驱动。
其他	   : 按键采用中断方式，按下按键触发按键中断，在按键中断里面
  		 使能定时器定时中断。使用定时器定时中断来完成消抖延时，
  		 定时器中断周期就是延时时间。如果定时器定时中断触发，
  		 表示消抖完成(延时周期完成)，即可执行按键处理函数。
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/5 左忠凯创建
***************************************************************/
#include "bsp_key.h"
#include "bsp_gpio.h"
#include "bsp_int.h"
#include "bsp_beep.h"
#include "bsp_keyfilter.h"

/*
 * @description		: 按键初始化
 * @param			: 无
 * @return 			: 无
 */
void filterkey_init(void)
{	
	gpio_pin_config_t key_config;
	
	/* 1、初始化IO复用 */
	IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18,0);	/* 复用为GPIO1_IO18 */

	/* 2、、配置GPIO1_IO18的IO属性	
	 *bit 16:0 HYS关闭
	 *bit [15:14]: 11 默认22K上拉
	 *bit [13]: 1 pull功能
	 *bit [12]: 1 pull/keeper使能
	 *bit [11]: 0 关闭开路输出
	 *bit [7:6]: 10 速度100Mhz
	 *bit [5:3]: 000 关闭输出
	 *bit [0]: 0 低转换率
	 */
	IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18,0xF080);
	
	/* 3、初始化GPIO为中断 */
	key_config.direction = kGPIO_DigitalInput;
	key_config.interruptMode = kGPIO_IntFallingEdge;
	key_config.outputLogic = 1;
	gpio_init(GPIO1, 18, &key_config);

	GIC_EnableIRQ(GPIO1_Combined_16_31_IRQn); /* 使能GIC中对应的中断   		  */
	
	/* 注册中断服务函数 */
	system_register_irqhandler(GPIO1_Combined_16_31_IRQn, 
							   (system_irq_handler_t)gpio1_16_31_irqhandler, 
							   NULL);
	
	gpio_enableint(GPIO1, 18);		/* 使能GPIO1_IO18的中断功能 */

	filtertimer_init(66000000/100);	/* 初始化定时器,10ms */
}


/*
 * @description		: 初始化用于消抖的定时器，默认关闭定时器
 * @param - value	: 定时器EPIT计数值
 * @return 			: 无
 */
void filtertimer_init(unsigned int value)
{
	EPIT1->CR = 0;	//先清零
	
	/*
     * CR寄存器:
     * bit25:24 01 时钟源选择Peripheral clock=66MHz
     * bit15:4  0  1分频
     * bit3:	1  当计数器到0的话从LR重新加载数值
     * bit2:	1  比较中断使能
     * bit1:    1  初始计数值来源于LR寄存器值
     * bit0:    0  先关闭EPIT1
     */
	EPIT1->CR = (1<<24 | 1<<3 | 1<<2 | 1<<1);

	/* 计数值    */
	EPIT1->LR = value;
	
	/* 比较寄存器，当计数器值和此寄存器值相等的话就会产生中断 */
	EPIT1->CMPR	= 0;	
	
	GIC_EnableIRQ(EPIT1_IRQn);	/* 使能GIC中对应的中断 */
	
	/* 注册中断服务函数		    */
	system_register_irqhandler(EPIT1_IRQn, (system_irq_handler_t)filtertimer_irqhandler, NULL);	
}

/*
 * @description		: 关闭定时器
 * @param 			: 无
 * @return 			: 无
 */
void filtertimer_stop(void)
{
	EPIT1->CR &= ~(1<<0);	/* 关闭定时器 */
}

/*
 * @description		: 重启定时器
 * @param - value	: 定时器EPIT计数值
 * @return 			: 无
 */
void filtertimer_restart(unsigned int value)
{
	EPIT1->CR &= ~(1<<0);	/* 先关闭定时器 */
	EPIT1->LR = value;		/* 计数值 			*/
	EPIT1->CR |= (1<<0);	/* 打开定时器 		*/
}

/*
 * @description		: 定时器中断处理函数 
 * @param			: 无
 * @return 			: 无
 */
void filtertimer_irqhandler(void)
{ 
	static unsigned char state = OFF;

	if(EPIT1->SR & (1<<0)) 					/* 判断比较事件是否发生			*/
	{
		filtertimer_stop();					/* 关闭定时器 				*/
		if(gpio_pinread(GPIO1, 18) == 0)	/* KEY0 				*/
		{
			state = !state;
			beep_switch(state);				/* 反转蜂鸣器 				*/
		}
	}
		
	EPIT1->SR |= 1<<0; 						/* 清除中断标志位 				*/
}

/*
 * @description		: GPIO中断处理函数
 * @param			: 无
 * @return 			: 无
 */
void gpio1_16_31_irqhandler(void)
{ 
	/* 开启定时器 */
	filtertimer_restart(66000000/100);

	/* 清除中断标志位 */
	gpio_clearintflags(GPIO1, 18);
}

