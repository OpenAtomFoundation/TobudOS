/**
  ******************************************************************************
  * @file    pit.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.24
  * \date    2015.10.03 FreeXc 完善了对 pit.c and pit.h 文件的注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片 PIT 模块的底层功能函数
  ******************************************************************************
  */
#include "pit.h"
#include "common.h"

//!< Internal vars
static uint32_t fac_us; /* usDelay Mut */

static PIT_CallBackType PIT_CallBackTable[4] = {NULL};
static const IRQn_Type PIT_IRQnTable[] = 
{
    PIT0_IRQn,
    PIT1_IRQn,
    PIT2_IRQn,
    PIT3_IRQn,
};

 /**
 * @brief  详细初始化 PIT 模块 推荐使用 PIT_QuickInit 函数
 * @code
 *      // 配置 PIT0 模块的 0 通道，时间周期为 1ms
 *      PIT_InitTypeDef PIT_InitStruct1; //申请一个结构变量
 *      PIT_InitStruct1.chl = 0;         //选择 0 通道
 *      PIT_InitStruct1.timeInUs = 1000  //1ms
 *      PIT_Init(&PIT_InitStruct1);
 * @endcode
 * @param[in]  PIT_InitStruct pit 模块工作配置数据
 * @retval None
 */
void PIT_Init(PIT_InitTypeDef* PIT_InitStruct)
{
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
    /* get clock */
    fac_us = GetClock(kBusClock);
    fac_us /= 1000000;
    PIT->CHANNEL[PIT_InitStruct->chl].LDVAL = fac_us * PIT_InitStruct->timeInUs;
    PIT->CHANNEL[PIT_InitStruct->chl].TCTRL |= (PIT_TCTRL_TEN_MASK);
    /* enable PIT module */
    PIT->MCR &= ~PIT_MCR_MDIS_MASK;
}

 /**
 * @brief  PIT 模块快速初始化配置
 * @code
 *      // 初始化 PIT 模块 0 通道 产生 100MS 中断 并开启中断 注册回调函数 在回调函数中打印调试信息
 *      //声明中断回调函数
 *      static void PIT0_CallBack(void);
 *      //初始化 PIT 模块的 0 通道，产生 100ms 中断
 *      PIT_QuickInit(HW_PIT_CH0, 100000);
 *      PIT_CallbackInstall(HW_PIT_CH0, PIT0_CallBack); //注册回调函数
 *      PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF,ENABLE); //开启模块 0 通道中断
 *      //中断回调函数
 *      static void PIT0_CallBack(void)
 *      {
 *          printf("Enter PIT0 INt\r\n");    
 *      }
 * @endcode
 * \attention 在中断函数中尽量不要放置会导致阻塞的函数，比如 printf 等，上述实例程序仅作
 * \attention 参考，建议实际应用中不加入 printf 函数
 * @param[in]  chl 通道号
 *              @arg HW_PIT_CH0 
 *              @arg HW_PIT_CH1
 *              @arg HW_PIT_CH2 
 *              @arg HW_PIT_CH3 
 * @param[in]  timeInUs 产生中断的周期 \单位 US
 * @retval None
 */
void PIT_QuickInit(uint8_t chl, uint32_t timeInUs)
{
    PIT_InitTypeDef PIT_InitStruct1;
    PIT_InitStruct1.chl = chl;
    PIT_InitStruct1.timeInUs = timeInUs;
    PIT_Init(&PIT_InitStruct1);
}

 /**
 * @brief  设置 PIT 模块是否开启中断功能
 * @code
 *      // 初始化 PIT 模块 0 通道 产生 100MS 中断 并开启中断 注册回调函数 在回调函数中打印调试信息
 *      //声明中断回调函数
 *      static void PIT0_CallBack(void);
 *      //初始化 PIT
 *      PIT_QuickInit(HW_PIT_CH0, 100000);
 *      PIT_CallbackInstall(HW_PIT0_CH0, PIT0_CallBack); //注册回调函数
 *      PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF,ENABLE); //开启模块 0 通道中断
 *      //中断回调函数编写
 *      static void PIT0_CallBack(void)
 *      {
 *          printf("Enter PIT0 INt\r\n");    
 *      }
 * @endcode
 * \attention 在中断函数中尽量不要放置会导致阻塞的函数，比如 printf 等，上述实例程序仅作
 * \attention 参考，建议实际应用中不加入 printf 函数
 * @param[in]  chl  通道号
 *              @arg HW_PIT0_CH0   0 通道
 *              @arg HW_PIT0_CH1   1 通道
 *              @arg HW_PIT0_CH2   2 通道
 *              @arg HW_PIT0_CH3   3 通道
 * @param[in]  config 是否打开中断
 *              @arg kPIT_IT_Disable 关闭中断
 *              @arg kPIT_IT_TOF     定时器溢出中断
 * @param[in]  flag 是否使能 PIT 中断
 *              @arg ENABLE 使能
 *              @arg DISABLE 不使能
 * @retval None
 */
void PIT_ITDMAConfig(uint8_t chl, PIT_ITDMAConfig_Type config,bool flag)
{
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
    if(flag)
    {
        NVIC_EnableIRQ(PIT_IRQnTable[chl]);
    }

    (flag)?
    (PIT->CHANNEL[chl].TCTRL |= PIT_TCTRL_TIE_MASK):
    (PIT->CHANNEL[chl].TCTRL &= ~PIT_TCTRL_TIE_MASK);
    
    /* clear IT bit */
    if(!flag)
    {
        PIT->CHANNEL[chl].TFLG |= PIT_TFLG_TIF_MASK;
    }
}

 /**
 * @brief  PIT 定时器 Counter 清到 LOAD 值 (重新开始新一次 CountDown 计时)
 * @param[in]  chl  通道号
 *              @arg HW_PIT0_CH0   0 通道
 *              @arg HW_PIT0_CH1   1 通道
 *              @arg HW_PIT0_CH2   2 通道
 *              @arg HW_PIT0_CH3   3 通道
 * @retval None
 */
void PIT_ResetCounter(uint8_t chl)
{
    PIT->CHANNEL[chl].TCTRL &= (~PIT_TCTRL_TEN_MASK);
    PIT->CHANNEL[chl].TCTRL |= (PIT_TCTRL_TEN_MASK);
}
 /**
 * @brief  读取 Counter 值
 * @param[in]  chl  通道号
 *              @arg HW_PIT0_CH0   0 通道
 *              @arg HW_PIT0_CH1   1 通道
 *              @arg HW_PIT0_CH2   2 通道
 *              @arg HW_PIT0_CH3   3 通道
 * @retval Counter 值
 */
uint32_t PIT_GetCounterValue(uint8_t chl)
{
    return PIT->CHANNEL[chl].CVAL;
}


/**
 * @brief  注册中断回调函数
 * @param[in]  chl  通道号
 *              @arg HW_PIT0_CH0   0 通道入口
 *              @arg HW_PIT0_CH1   1 通道入口
 *              @arg HW_PIT0_CH2   2 通道入口
 *              @arg HW_PIT0_CH3   3 通道入口
 * @param[in]  AppCBFun 回调函数指针入口
 * @retval None
 * @note 对于此函数的具体应用请查阅应用实例
 */
void PIT_CallbackInstall(uint8_t chl, PIT_CallBackType AppCBFun)
{
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
    if(AppCBFun != NULL)
    {
        PIT_CallBackTable[chl] = AppCBFun;
    }
}

/**
 * @brief  PIT 中断触发用户定义的回调函数，此函数用户不需要修改
 */
static void PIT_IRQHandler(uint32_t instance)
{
    PIT->CHANNEL[instance].TFLG |= PIT_TFLG_TIF_MASK;
    if(PIT_CallBackTable[instance])
    {
        PIT_CallBackTable[instance]();
    }
}

/**
 * @brief  系统 PIT0 中断函数，用户无需使用
 */
void PIT0_IRQHandler(void)
{
    PIT_IRQHandler(0);
}

/**
 * @brief  系统 PIT1 中断函数，用户无需使用
 */
void PIT1_IRQHandler(void)
{
    PIT_IRQHandler(1);
}

/**
 * @brief  系统 PIT2 中断函数，用户无需使用
 */
void PIT2_IRQHandler(void)
{
    PIT_IRQHandler(2);
}
/**
 * @brief  系统 PIT3 中断函数，用户无需使用
 */
void PIT3_IRQHandler(void)
{
    PIT_IRQHandler(3);
}
