/**
  ******************************************************************************
  * @file    lptmr.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * \date    2015.10.03 FreeXc 完善了lptmr模块的相关注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#include "common.h"
#include "lptmr.h"
#include "gpio.h"


/* 回调函数 指针 */
static LPTMR_CallBackType LPTMR_CallBackTable[1] = {NULL};
/* 中断向量入口 */
static const IRQn_Type LPTMR_IRQnTable[] = 
{
    LPTimer_IRQn,
};

#if defined(SIM_SCGC5_LPTIMER_MASK)
static const Reg_t SIM_LPTMRClockGateTable[] =
{
    {(void*)&(SIM->SCGC5), SIM_SCGC5_LPTIMER_MASK},
};

#elif defined(SIM_SCGC5_LPTMR_MASK)
static const Reg_t SIM_LPTMRClockGateTable[] =
{
    {(void*)&(SIM->SCGC5), SIM_SCGC5_LPTMR_MASK},
};
#endif

/**
 * @brief  初始化配置LPTMR模块处于计时器模式
 * @code
 *      //设置LPTM工作在计时器模式，时间间隔是500毫秒
 *      LPTMR_TC_InitTypeDef LPTMR_TC_InitStruct1; //申请一个结构变量
 *      LPTMR_TC_InitStruct1.timeInMs = 500;       //设置计时时间是500ms
 *      LPTMR_TC_Init(&LPTMR_TC_InitStruct1);
 * @endcode
 * @param[in]  LPTMR_TC_InitStruct LPTMR配置结构体
 *              @arg timeInMs 定时时间 单位为ms
 * @retval None
 */
void LPTMR_TC_Init(LPTMR_TC_InitTypeDef* LPTMR_TC_InitStruct)
{
	/* open clock gate */
    *(uint32_t*)SIM_LPTMRClockGateTable[0].addr |= SIM_LPTMRClockGateTable[0].mask;
    
    LPTMR0->CSR = 0x00; 
    LPTMR0->PSR = 0x00;
    LPTMR0->CMR = 0x00;
    
    /* disable module first */
    LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK;
    
    /* free counter will reset whenever compare register is writtened. */
    LPTMR0->CSR &= ~LPTMR_CSR_TFC_MASK;  
    
    /* timer counter mode */
    LPTMR0->CSR &= ~LPTMR_CSR_TMS_MASK; 
    
	/* bypass the prescaler, which mean we use 1KHZ LPO directly */
    LPTMR0->PSR = LPTMR_PSR_PCS(1)| LPTMR_PSR_PBYP_MASK; 
    
    /* set CMR(compare register) */
    LPTMR0->CMR = LPTMR_CMR_COMPARE(LPTMR_TC_InitStruct->timeInMs);
    
    /* enable moudle */
    LPTMR0->CSR |= LPTMR_CSR_TEN_MASK;  
}

/**
 * @brief  初始化配置LPTM模块处于脉冲计数模式
 * @code
 *      //设置LPTM工作在脉冲计数模式，计数上限是0xFFFE
 *      LPTMR_PC_InitTypeDef LPTMR_PC_InitStruct1; //申请一个结构变量
 *      LPTMR_PC_InitStruct1.timeInMs = 500;       //设置计时时间是500ms
 *      LPTMR_TC_Init(&LPTMR_TC_InitStruct1);
 * @endcode
 * @param[in]  LPTMR_PC_InitStruct LPTMR配置结构体
 *              @arg counterOverflowValue       计数器计数上限，极限为0xFFFF
 *              @arg inputSource 脉冲源选择 kLPTMR_PC_InputSource_CMP0-CMP0作为脉冲计数时钟源 kLPTMR_PC_InputSource_ALT1-外部引脚LPTMR_ALT1作为外部计数时钟源 kLPTMR_PC_InputSource_ALT2-外部引脚LPTMR_ALT2作为外部计数时钟源
 *              @arg pinPolarity 脉冲计数极性选择 kLPTMR_PC_PinPolarity_RigsingEdge 上升沿计数 kLPTMR_PC_PinPolarity_FallingEdge 下降沿计数
 * @retval None
 */
void LPTMR_PC_Init(LPTMR_PC_InitTypeDef* LPTMR_PC_InitStruct)
{
	/* open clock gate */
    *(uint32_t*)SIM_LPTMRClockGateTable[0].addr |= SIM_LPTMRClockGateTable[0].mask;
    
    LPTMR0->CSR = 0x00; 
    LPTMR0->PSR = 0x00;
    LPTMR0->CMR = 0x00;
    
    /* disable module first */
    LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK;
    
    /* free counter will reset whenever compare register is writtened. */
    LPTMR0->CSR &= ~LPTMR_CSR_TFC_MASK;  
    
    /* timer counter mode */
    LPTMR0->CSR |= LPTMR_CSR_TMS_MASK; 
    
	/* bypass the glitch filter, which mean we use 1KHZ LPO directly */
    LPTMR0->PSR = LPTMR_PSR_PCS(1)| LPTMR_PSR_PBYP_MASK;
    
    /* set CMR(compare register) */
    LPTMR0->CMR = LPTMR_CMR_COMPARE(LPTMR_PC_InitStruct->counterOverflowValue);
    
    /* input source */
    switch(LPTMR_PC_InitStruct->inputSource)
    {
        case kLPTMR_PC_InputSource_CMP0:
            LPTMR0->CSR |= LPTMR_CSR_TPS(0);
            break;
        case kLPTMR_PC_InputSource_ALT1:
            LPTMR0->CSR |= LPTMR_CSR_TPS(1);
            break;
        case kLPTMR_PC_InputSource_ALT2:
            LPTMR0->CSR |= LPTMR_CSR_TPS(2);
            break; 
        default:
            break;
    }
    
    /* pin polarity */
    switch(LPTMR_PC_InitStruct->pinPolarity)
    {
        case kLPTMR_PC_PinPolarity_RigsingEdge:
            LPTMR0->CSR &= ~LPTMR_CSR_TPP_MASK;
            break;
        case kLPTMR_PC_PinPolarity_FallingEdge:
            LPTMR0->CSR |= LPTMR_CSR_TPP_MASK;
            break;
        default:
            break;
    }
    
    /* enable moudle */
    LPTMR0->CSR |= LPTMR_CSR_TEN_MASK; 
}

/**
 * @brief  LPTM模块中断和DMA功能配置
 * @code
 *     //配置LPTM模块产生溢出中断
 *     LPTMR_ITDMAConfig(kLPTMR_IT_TOF, true);
 * @endcode
 * @param[in]  config LPTM中断类型
 *         @arg kLPTMR_IT_Disable  关闭中断
 *         @arg kLPTMR_IT_TOF      开启计数溢出中断
 * @param[in] status 是否使能中断
 *         @arg 0  interrupt disable
 *         @arg 1  interrupt enable
 * @retval None
 */
void LPTMR_ITDMAConfig(LPTMR_ITDMAConfig_Type config, bool status)
{
    
    /* enable clock gate */
    *(uint32_t*)SIM_LPTMRClockGateTable[0].addr |= SIM_LPTMRClockGateTable[0].mask;
    
    if(status) NVIC_EnableIRQ(LPTMR_IRQnTable[0]);
    switch (config)
    {
        case kLPTMR_IT_TOF:
            (status)?
            (LPTMR0->CSR |= LPTMR_CSR_TIE_MASK):
            (LPTMR0->CSR &= ~LPTMR_CSR_TIE_MASK);
            break;
        default:
            break;
    }
}

/**
 * @brief  注册LPTMR中断回调函数
 * @param[in]  AppCBFun 回调函数指针入口
 * @retval None
 * @note 对于此函数的具体应用请查阅应用实例
 */
void LPTMR_CallbackInstall(LPTMR_CallBackType AppCBFun)
{
	/* open clock gate */
    *(uint32_t*)SIM_LPTMRClockGateTable[0].addr |= SIM_LPTMRClockGateTable[0].mask;
    
    if(AppCBFun != NULL)
    {
        LPTMR_CallBackTable[0] = AppCBFun;
    }
}

/**
 * @brief  获取脉冲计数器的脉冲数
 * @code
 *     //获取脉冲计数的个数
 *     uint32_t counter;
 *     counter = LPTMR_PC_ReadCounter();
 * @endcode
 * @retval 脉冲计数个数
 */
uint32_t LPTMR_PC_ReadCounter(void)
{
    /* must first read */
    uint32_t *p = (uint32_t *)&LPTMR0->CNR;
    *p = 0x1234;
	return (uint32_t)((LPTMR0->CNR & LPTMR_CNR_COUNTER_MASK) >> LPTMR_CNR_COUNTER_SHIFT); 
}

/**
 * @brief  LPTMR脉冲计数快速初始化
 * @code
 *     //LPTMR脉冲计数快速初始化
 *     LPTMR_PC_QuickInit(LPTMR_ALT2_PC05);
 * @endcode
 * @param[in] MAP 单路脉冲计数模块
 * @see MAP详细的宏定义请见lptmr.h文件
 * @retval 脉冲计数的模块号
 */

uint32_t LPTMR_PC_QuickInit(uint32_t MAP)
{
    uint32_t i;
    map_t * pq = (map_t*)&(MAP);
    LPTMR_PC_InitTypeDef LPTMR_PC_InitStruct1;
    LPTMR_PC_InitStruct1.counterOverflowValue = 0xFFFF;
    switch(pq->chl)
    {
        case 1:
            LPTMR_PC_InitStruct1.inputSource = kLPTMR_PC_InputSource_ALT1;
            break;
        case 2:
            LPTMR_PC_InitStruct1.inputSource = kLPTMR_PC_InputSource_ALT2;
            break;
        default:
            break;
    }
    LPTMR_PC_InitStruct1.pinPolarity = kLPTMR_PC_PinPolarity_RigsingEdge;
    /* init pinmux */
    for(i = 0; i < pq->pin_cnt; i++)
    {
        PORT_PinMuxConfig(pq->io, pq->pin_start + i, (PORT_PinMux_Type) pq->mux); 
    }
    /* init moudle */
    LPTMR_PC_Init(&LPTMR_PC_InitStruct1);
    return pq->ip;
}

/**
 * @brief  清除脉冲计数器的脉冲数
 * @code
 *     //清除脉冲计数的个数
 *     LPTMR_ClearCounter();
 * @endcode
 * @retval None
 */
void LPTMR_ClearCounter(void)
{
    /* disable and reenable moudle to clear counter */
    LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK;
    LPTMR0->CSR |= LPTMR_CSR_TEN_MASK;
}

/**
 * @brief  中断处理函数入口，调用用户注册的回调函数
 * @note 函数内部用于中断事件处理
 */
void LPTimer_IRQHandler(void)
{
    LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;
    if(LPTMR_CallBackTable[0])
    {
        LPTMR_CallBackTable[0]();
    }
}


#if 0
static const map_t LPTMR_QuickInitTable[] = 
{
    { 0, 0, 6,19, 1, 1}, //LPTMR_ALT1_PA19 6
    { 0, 2, 4, 5, 1, 2}, //LPTMR_ALT2_PC05 4
};
#endif

