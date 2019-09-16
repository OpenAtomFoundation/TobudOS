/**
  ******************************************************************************
  * @file    systick.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * \date    2015.10.03 FreeXc完善了systick模块的相关注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片ARM内核中的SysTick模块的底层功能函数
  ******************************************************************************
  */
#include "systick.h"
#include "common.h"

static uint32_t fac_us = 0;     //!< usDelay Mut
static uint32_t fac_ms = 0;     //!< msDelay Mut



 /**
 * @brief  初始化SysTick时钟
 * @code
 *   // 初始化SysTick时钟 设定中断周期为10000us(10ms)
 *   SYSTICK_Init(10000);
 * @endcode
 * @param[in]  timeInUs 中断周期,单位us
 * \note systick属于cm4内核中的模块，在RTOS中可作为其时钟节拍
 * @retval None
 */
void SYSTICK_Init(uint32_t timeInUs)
{
    /* Set clock source = core clock */
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; 
    fac_us = GetClock(kCoreClock);
    fac_us /= 1000000;
    fac_ms = fac_us * 1000;
    /* every 100ms in a int */
    SysTick->LOAD = fac_us * timeInUs;
}

 /**
 * @brief  初始化SysTick为延时应用 初始化后就可以调用 DelayMs DelayUs
 * @code
 *      //将SysTick用作延时的初始化 初始化后系统延时20ms
 *      SYSTICK_DelayInit();
 *      SYSTICK_DelayMs(20);
 * @endcode
 * @retval None
 */
void SYSTICK_DelayInit(void)
{
    SYSTICK_Init(1000);
    SYSTICK_Cmd(true);
    SYSTICK_ITConfig(false);
}

 /**
 * @brief  开启或者停止SysTick时钟
 * @code
 *   //开启时钟
 *   SYSTICK_Cmd(true);
 * @endcode
 * @param[in]  NewState 使能或者关闭
 *              @arg true 使能
 *              @arg false 停止
 * \attention  当给微控制器移植OS后，需要开启systick时钟以及中断，不然OS创建的任务无法工作
 * @retval None
 */
void SYSTICK_Cmd(bool NewState)
{
    (true == NewState)?(SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk):(SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk);
}

 /**
 * @brief  开启SysTick中断
 * @code
 *      //开启中断功能
 *      SYSTICK_ITConfig(true);
 * @endcode
 * @param[in]  NewState 使能或者关闭
 *              @arg true  使能
 *              @arg false 禁止
 * \attention  当给微控制器移植OS后，需要开启systick时钟以及中断，不然OS创建的任务无法工作
 * @retval None
 */
void SYSTICK_ITConfig(bool NewState)
{
    (true == NewState)?(SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk):(SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk);
}

 /**
 * @brief 微秒级延时函数
 * @code
 *      //延时100us
 *      SYSTICK_DelayUs(100);
 * @endcode
 * @param[in]  us 延时时间 单位us
 * @retval None
 */
void SYSTICK_DelayUs(uint32_t us)
{
    uint32_t temp;
    SysTick->LOAD = us * fac_us;
    SysTick->VAL = 0;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do
    {
        temp = SysTick->CTRL;
    }
    while((temp & SysTick_CTRL_ENABLE_Msk) && !(temp & SysTick_CTRL_COUNTFLAG_Msk));
}

 /**
 * @brief  毫秒级延时函数
 * 
 * @code
 *      //延时100ms
 *      SYSTICK_DelayMs(100);
 * @endcode
 * @param[in]  ms 延时
 * @retval None
 */
void SYSTICK_DelayMs(uint32_t ms)
{
    uint32_t temp;
    uint32_t i;
    SysTick->LOAD = fac_ms;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    for(i = 0; i < ms; i++)
	{
		SysTick->VAL = 0;
		do
		{
			temp = SysTick->CTRL;
		}
        while((temp & SysTick_CTRL_ENABLE_Msk) && !(temp & SysTick_CTRL_COUNTFLAG_Msk));
	}
}

/**
 * @brief  获得当前System tick timer的值
 * @retval 当前System tick timer的值
 */
uint32_t SYSTICK_GetVal(void)
{
    return SysTick->VAL;
}
