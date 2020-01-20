/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_exit.c
作者	   : 左忠凯
版本	   : V1.0
描述	   : 外部中断驱动。
其他	   : 配置按键对应的GPIP为中断模式
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/4 左忠凯创建
***************************************************************/
#include "bsp_exit.h"
#include "bsp_gpio.h"
#include "bsp_delay.h"
#include "bsp_beep.h"
#include "bsp_int.h"

#include "tos_k.h"

/*
 * @description			: 初始化外部中断
 * @param				: 无
 * @return 				: 无
 */
void exit_init(void)
{
    gpio_pin_config_t key_config;

    /* 1、设置IO复用 */
    IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18,0);			/* 复用为GPIO1_IO18 */
    IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18,0xF080);

    /* 2、初始化GPIO为中断模式 */
    key_config.direction = kGPIO_DigitalInput;
    key_config.interruptMode = kGPIO_IntFallingEdge;
    key_config.outputLogic = 1;
    gpio_init(GPIO1, 18, &key_config);

    GIC_EnableIRQ(GPIO1_Combined_16_31_IRQn);				/* 使能GIC中对应的中断 */
#if 0
    system_register_irqhandler(GPIO1_Combined_16_31_IRQn, (system_irq_handler_t)gpio1_io18_irqhandler, NULL);	/* 注册中断服务函数 */
#else
    tos_interrupt_handler_register(GPIO1_Combined_16_31_IRQn, (int_handler_t)gpio1_io18_irqhandler, NULL);
#endif
    gpio_enableint(GPIO1, 18);  /* 使能GPIO1_IO18的中断功能 */
}

/*
 * @description			: GPIO1_IO18最终的中断处理函数
 * @param				: 无
 * @return 				: 无
 */
void gpio1_io18_irqhandler(void *arg)
{
    static unsigned char state = 0;

    /*
    *采用延时消抖，中断服务函数中禁止使用延时函数！因为中断服务需要
    *快进快出！！这里为了演示所以采用了延时函数进行消抖，后面我们会讲解
    *定时器中断消抖法！！！
    */

    delay(10);
    if (gpio_pinread(GPIO1, 18) == 0)   /* 按键按下了  */
    {
        state = !state;
        beep_switch(state);
    }

    gpio_clearintflags(GPIO1, 18); /* 清除中断标志位 */
}

