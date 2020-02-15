#include "bsp_int.h"
/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_int.c
作者	   : 左忠凯
版本	   : V1.0
描述	   : 中断驱动文件。
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/4 左忠凯创建
***************************************************************/

/* 中断嵌套计数器 */
static unsigned int irqNesting;

/* 中断服务函数表 */
static sys_irq_handle_t irqTable[NUMBER_OF_INT_VECTORS];

/*
 * @description	: 中断初始化函数
 * @param		: 无
 * @return 		: 无
 */
void int_init(void)
{
	GIC_Init(); 						/* 初始化GIC 							*/
	system_irqtable_init();				/* 初始化中断表 							*/
	__set_VBAR((uint32_t)0x87800000); 	/* 中断向量表偏移，偏移到起始地址   				*/
}

/*
 * @description	: 初始化中断服务函数表
 * @param		: 无
 * @return 		: 无
 */
void system_irqtable_init(void)
{
	unsigned int i = 0;
	irqNesting = 0;

	/* 先将所有的中断服务函数设置为默认值 */
	for(i = 0; i < NUMBER_OF_INT_VECTORS; i++)
	{
		system_register_irqhandler((IRQn_Type)i,default_irqhandler, NULL);
	}
}

/*
 * @description			: 给指定的中断号注册中断服务函数
 * @param - irq			: 要注册的中断号
 * @param - handler		: 要注册的中断处理函数
 * @param - usrParam	: 中断服务处理函数参数
 * @return 				: 无
 */
void system_register_irqhandler(IRQn_Type irq, system_irq_handler_t handler, void *userParam)
{
	irqTable[irq].irqHandler = handler;
  	irqTable[irq].userParam = userParam;
}

/*
 * @description			: C语言中断服务函数，irq汇编中断服务函数会
 						  调用此函数，此函数通过在中断服务列表中查
 						  找指定中断号所对应的中断处理函数并执行。
 * @param - giccIar		: 中断号
 * @return 				: 无
 */
void system_irqhandler(unsigned int giccIar)
{

   uint32_t intNum = giccIar & 0x3FFUL;

   /* 检查中断号是否符合要求 */
   if ((intNum == 1023) || (intNum >= NUMBER_OF_INT_VECTORS))
   {
	 	return;
   }

   irqNesting++;	/* 中断嵌套计数器加一 */

   /* 根据传递进来的中断号，在irqTable中调用确定的中断服务函数*/
   irqTable[intNum].irqHandler(intNum, irqTable[intNum].userParam);

   irqNesting--;	/* 中断执行完成，中断嵌套寄存器减一 */

}

/*
 * @description			: 默认中断服务函数
 * @param - giccIar		: 中断号
 * @param - usrParam	: 中断服务处理函数参数
 * @return 				: 无
 */
void default_irqhandler(unsigned int giccIar, void *userParam)
{
	while(1)
  	{
   	}
}



