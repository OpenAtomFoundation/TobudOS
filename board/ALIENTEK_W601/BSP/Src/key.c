#include "key.h"
#include "dwt.h"
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
 *	KEY驱动代码
 *	正点原子@ALIENTEK
 *	技术论坛:www.openedv.com
 *	创建日期:2019/7/10
 *	版本：V1.0
 *	版权所有，盗版必究。
 *	Copyright(C) 广州市星翼电子科技有限公司 2019-2029
 *	All rights reserved
 *	******************************************************************************
 *	初始版本
 *	******************************************************************************/

/**
 * @brief	按键初始化函数
 *
 * @param   void
 *
 * @return  void
 */
void KEY_Init(void)
{
    /*
    	KEY0 	- PA7
    	KEY1 	- PA6
    	KEY_UP 	- PA8	高电平有效
    */

    tls_gpio_cfg(WM_IO_PA_06, WM_GPIO_DIR_INPUT, WM_GPIO_ATTR_FLOATING);
    tls_gpio_cfg(WM_IO_PA_07, WM_GPIO_DIR_INPUT, WM_GPIO_ATTR_FLOATING);
    tls_gpio_cfg(WM_IO_PA_08, WM_GPIO_DIR_INPUT, WM_GPIO_ATTR_PULLLOW);
}

/**
 * @brief	按键处理函数
 *
 * @remark	注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY_UP!!
 *
 * @param   mode	0:不支持连续按,1:支持连续按
 *
 * @return  u8		返回按键值
*					0:没有任何按键按下,1:KEY0按下,2:KEY1按下,3:KEY2按下,4:KEY_UP按下
 */
u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1;   //按键松开标志

    if(mode == 1)key_up = 1; //支持连按

    if(key_up && (KEY0 == 0 || KEY1 == 0 || KEY_UP == 1))
    {
        delay_ms(10);
        key_up = 0;

        if(KEY0 == 0)       return KEY0_PRES;

        else if(KEY1 == 0)  return KEY1_PRES;

        else if(KEY_UP == 1) return WKUP_PRES;
    }

    else if(KEY0 == 1 && KEY1 == 1 && KEY_UP == 0)key_up = 1;

    return 0;   //无按键按下
}
