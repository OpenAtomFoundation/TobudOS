#include "sys.h"
#include "tos_config.h"
/*********************************************************************************
			  ___   _     _____  _____  _   _  _____  _____  _   __
			 / _ \ | |   |_   _||  ___|| \ | ||_   _||  ___|| | / /
			/ /_\ \| |     | |  | |__  |  \| |  | |  | |__  | |/ /
			|  _  || |     | |  |  __| | . ` |  | |  |  __| |    \
			| | | || |_____| |_ | |___ | |\  |  | |  | |___ | |\  \
			\_| |_/\_____/\___/ \____/ \_| \_/  \_/  \____/ \_| \_/

 *	******************************************************************************
 *	本程序只供学习使用，未经作者许可，不得用于其它任何用途
 *	ALIENTEK W601开发板
 *	系统时钟初始化
 *	包括时钟设置/中断管理等
 *	正点原子@ALIENTEK
 *	技术论坛:www.openedv.com
 *	创建日期:2019/7/10
 *	版本：V1.0
 *	版权所有，盗版必究。
 *	Copyright(C) 广州市星翼电子科技有限公司 2019-2029
 *	All rights reserved
 *	******************************************************************************
 *	版本修改说明
 *	无
 *	******************************************************************************/


/**
 * @brief	系统配置函数
 *
 * @remark	该函数在startup.s中调用,W601系统时钟初始化为80MHz
 *
 * @param   void
 *
 * @return  void
 */
void SystemInit(void)
{
    tls_sys_clk sysclk;
    /*系统时钟设置为80MHz*/
    tls_sys_clk_set(CPU_CLK_80M);

    /*关闭所有IO的复用功能，只开启SWD复用功能*/
    wm_gpio_af_disable();
    tls_sys_clk_get(&sysclk);
    SysTick_Config(sysclk.cpuclk * UNIT_MHZ / TOS_CFG_CPU_TICK_PER_SECOND);
    /*中断优先级组配置*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    /*配置深度睡眠低功耗模式*/
    NVIC_SystemLPConfig(NVIC_LP_SLEEPDEEP, ENABLE);

    /*选择PMU时钟*/
    tls_pmu_clk_select(0);
}


/**
 * @brief	THUMB指令不支持汇编内联、
 *			采用如下方法实现执行汇编指令WFI
 *
 * @param   void
 *
 * @return  __asm
 */
__asm void WFI_SET(void)
{
    WFI;
}
/**
 * @brief	关闭所有中断(但是不包括fault和NMI中断)
 *
 * @param   void
 *
 * @return  __asm
 */
__asm void INTX_DISABLE(void)
{
    CPSID   I
    BX      LR
}
/**
 * @brief	开启所有中断
 *
 * @param   void
 *
 * @return  __asm
 */
__asm void INTX_ENABLE(void)
{
    CPSIE   I
    BX      LR
}
/**
 * @brief	设置栈顶地址
 *
 * @param   addr	栈顶地址
 *
 * @return  __asm
 */
__asm void MSR_MSP(u32 addr)
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}
















