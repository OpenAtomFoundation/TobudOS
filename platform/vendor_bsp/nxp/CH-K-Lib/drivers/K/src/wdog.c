/**
  ******************************************************************************
  * @file    wdog.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.24
  * @date    2015.10.05 FreeXc 完善了wdog模块的相关API注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片看门狗模块的底层功能函数
  ******************************************************************************
  */
#include "wdog.h"
#include "common.h"

static WDOG_CallBackType WDOG_CallBackTable[1] = {NULL};

/**
 * @brief  看门狗解锁 内部函数，用户无需调用
 * @retval None
 */
static void WDOG_Unlock(void)
{
    __disable_irq();
    WDOG->UNLOCK = 0xC520u;
    WDOG->UNLOCK = 0xD928u;
    __enable_irq();
}
/**
 * @brief  看门狗快速初始化配置
 * @code
 *      //配置看门狗的定时时间为100ms
 *      WDOG_QuickInit(100); 
 * @endcode
 * @param[in]  timeInMs 看门狗触发时间，单位ms
 * @retval None
 */
void WDOG_QuickInit(uint32_t timeInMs)
{
    WDOG_InitTypeDef WDOG_InitStruct1;
    WDOG_InitStruct1.mode = kWDOG_Mode_Normal;
    WDOG_InitStruct1.timeOutInMs = timeInMs;
    WDOG_InitStruct1.windowInMs = timeInMs/2;
    WDOG_Init(&WDOG_InitStruct1);
}
/**
 * @brief  看门狗详细初始化配置
 * @code
 *      //配置看门狗在正常模式下，时间为100ms
 *    WDOG_InitTypeDef WDOG_InitStruct1;        //申请一个结构变量
 *    WDOG_InitStruct1.mode = kWDOG_Mode_Normal;//选择看门狗处于正常模式
 *    WDOG_InitStruct1.timeOutInMs = 100;       //设置间隔时间为100ms
 *    WDOG_InitStruct1.windowInMs  = 20;        //在正常模式下无意义
 *    WDOG_Init(&WDOG_InitStruct1); 
 * @endcode
 * @param[in]  WDOG_InitStruct 看门狗工作模式配置结构体   
 * @retval None
 */
void WDOG_Init(WDOG_InitTypeDef* WDOG_InitStruct)
{
    uint32_t clock;
    uint32_t time_out;
    clock = GetClock(kBusClock);
    uint32_t wdag_value = 0x01D3u;
    switch((uint32_t)WDOG_InitStruct->mode)
    {
        case kWDOG_Mode_Normal:
            wdag_value &= ~WDOG_STCTRLH_WINEN_MASK;
            break;
        case kWDOG_Mode_Window:
            wdag_value |= WDOG_STCTRLH_WINEN_MASK;
            break;		
        default:
            break;
    }
    WDOG_Unlock();
    /* set timeout value */
    time_out = ((clock/8)/1000)*(WDOG_InitStruct->timeOutInMs);
    WDOG->TOVALH = (time_out & 0xFFFF0000)>>16;
    WDOG->TOVALL = (time_out & 0x0000FFFF)>>0;
    /* set window time value :timeout must greater then window time */
    time_out = ((clock/8)/1000)*(WDOG_InitStruct->windowInMs);
    WDOG->WINH = (time_out & 0xFFFF0000)>>16;
    WDOG->WINL = (time_out & 0x0000FFFF)>>0;
    WDOG->PRESC = WDOG_PRESC_PRESCVAL(7); // perscale = 8
    /* enable wdog */
    wdag_value |= WDOG_STCTRLH_WDOGEN_MASK;
    WDOG->STCTRLH = wdag_value;
}

/**
 * @brief  看门狗中断配置
 * @code
 *      //开启看门狗中断功能    
 *      WDOG_ITDMAConfig(true);  //中断不常用
 * @endcode
 * @param[in]  status 是否开启WDOG中断
 *          \arg true  开启中断
 *          \arg false 关闭中断
 * @retval None
 */
void WDOG_ITDMAConfig(bool status)
{
    WDOG_Unlock();
    (true == status)?
    (WDOG->STCTRLH |= WDOG_STCTRLH_IRQRSTEN_MASK):
    (WDOG->STCTRLH &= ~(WDOG_STCTRLH_IRQRSTEN_MASK));

    (true == status)?
    NVIC_EnableIRQ(Watchdog_IRQn):
    NVIC_DisableIRQ(Watchdog_IRQn);
}

/**
 * @brief  WDOG注册中断回调函数
 * @param[in] AppCBFun 回调函数指针入口
 * @retval None
 * @note 对于此函数的具体应该请查阅应用实例
 */
void WDOG_CallbackInstall(WDOG_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        WDOG_CallBackTable[0] = AppCBFun;
    }
}

/**
 * @brief  读取看门狗计数器的数值
 * @code
 *      //获取当前看门狗中计时器的数值 
 *      uint32_t counter;    //申请一个变量
 *      counter = WDOG_GetResetCounter();  //获取计时器的数值，存储在counter中
 * @endcode
 * @retval 当前计数器的数值
 */
uint32_t WDOG_GetResetCounter(void)
{
    return (WDOG->RSTCNT);
}

/**
 * @brief  清除看门狗计数器的数值
 * @code
 *      //清除当前看门狗中计时器的数值 
 *      WDOG_ClearResetCounter(); 
 * @endcode
 * @retval None
 */
void WDOG_ClearResetCounter(void)
{
    WDOG->RSTCNT = WDOG_RSTCNT_RSTCNT_MASK;
}

/**
 * @brief  读取看门狗当前定时器的输出值
 * @return Watchdog Timer Output Value
 */
uint32_t WDOG_GetCurrentCounter(void)
{
    uint32_t val;
    val = (WDOG->TMROUTH << 16);
    val |= WDOG->TMROUTL;
    return val;
}

/**
 * @brief  喂狗
 * @code
 *     WDOG_Refresh();  //喂狗
 * @endcode
 * @retval None
 */
void WDOG_Refresh(void)
{
    uint32_t i;
    __disable_irq();
	WDOG->REFRESH = 0xA602u;
	WDOG->REFRESH = 0xB480u;
    __enable_irq();
    /* a gap of more then 20 bus cycle between 2 refresh sequence */
    for(i = 0; i < 20; i++)
    {
        __NOP();
    }
}

/**
 * @brief  中断处理函数入口
 * @note   用于调用用户注册的回调函数,用户无需使用
 */
void Watchdog_IRQHandler(void)
{
    WDOG->STCTRLL |= WDOG_STCTRLL_INTFLG_MASK;    
    if(WDOG_CallBackTable[0])
    {
        WDOG_CallBackTable[0]();
    }
}
