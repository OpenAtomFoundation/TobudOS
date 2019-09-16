/******************************************************************************
 * Include files
 ******************************************************************************/
#include "led.h"

void led_init(void)
{
    stc_gpio_config_t pstcGpioCfg;
    
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    ///< 端口方向配置->输出
    pstcGpioCfg.enDir = GpioDirOut;
    ///< 端口驱动能力配置->高驱动能力
    pstcGpioCfg.enDrv = GpioDrvH;
    ///< 端口上下拉配置->无上下拉
    pstcGpioCfg.enPuPd = GpioNoPuPd;
    ///< 端口开漏输出配置->开漏输出关闭
    pstcGpioCfg.enOD = GpioOdDisable;
    ///< 端口输入/输出值寄存器总线控制模式配置->AHB
    pstcGpioCfg.enCtrlMode = GpioAHB;
    
    ///< GPIO IO PD05初始化（PD05在STK上外接LED）
    Gpio_Init(GpioPortD, GpioPin5, &pstcGpioCfg);
}

void led_on(void)
{
	///< 端口PD05设置为高电平（LED点亮）
	Gpio_SetIO(GpioPortD, GpioPin5);
}

void led_off(void)
{
	///< 端口PD05设置为低电平（LED关闭）
	Gpio_ClrIO(GpioPortD, GpioPin5);
}   

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


