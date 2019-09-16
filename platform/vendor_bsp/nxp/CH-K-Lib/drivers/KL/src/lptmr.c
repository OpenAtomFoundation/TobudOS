/**
  ******************************************************************************
  * @file    lptmr.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "lptmr.h"
#include "common.h"

/* 回调函数 指针 */

/* 中断向量入口 */
static const IRQn_Type LPTMR_IRQnTable[] = 
{
    (IRQn_Type)28,
};

/**
 * @brief  初始化配置LPTM模块处于计时器模式
 * @code
 *      //设置LPTM工作在计时器模式，时间间隔是500毫秒
 *      LPTMR_TC_InitTypeDef LPTMR_TC_InitStruct1; //申请一个结构变量
 *      LPTMR_TC_InitStruct1.timeInMs = 500;       //设置计时时间是500ms
 *      LPTMR_TC_Init(&LPTMR_TC_InitStruct1);
 * @endcode
 * @param  LPTMR_TC_InitStruct: 工作配置结构体
 *         @arg timeInMs :定时时间 单位为ms
 * @retval None
 */
void LPTMR_TC_Init(LPTMR_TC_InitTypeDef* LPTMR_TC_InitStruct)
{
	/* open clock gate */
	SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK; 
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
 * @param  LPTMR_PC_InitTypeDef: 工作配置结构体
 *         @arg counterOverflowValue       :计数器计数上限，极限为0xFFFF
 *         @arg inputSource :脉冲源选择 kLPTMR_PC_InputSource_CMP0-CMP0作为脉冲计数时钟源 kLPTMR_PC_InputSource_ALT1-外部引脚LPTMR_ALT1作为外部计数时钟源 kLPTMR_PC_InputSource_ALT2-外部引脚LPTMR_ALT2作为外部计数时钟源
 *         @arg pinPolarity :脉冲计数极性选择 kLPTMR_PC_PinPolarity_RigsingEdge 上升沿计数 kLPTMR_PC_PinPolarity_FallingEdge 下降沿计数
 * @retval None
 */
void LPTMR_PC_Init(LPTMR_PC_InitTypeDef* LPTMR_PC_InitStruct)
{
	/* open clock gate */
	SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK; 
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
 * @param  LPTMR_ITDMAConfig_Type: LPTM中断类型
 *         @arg kLPTMR_IT_Disable  :关闭中断
 *         @arg kLPTMR_IT_TOF      :开启计数溢出中断
 * @retval None
 */
void LPTMR_ITDMAConfig(LPTMR_ITDMAConfig_Type config, bool status)
{
    SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK; 
    
    if(status)
    {
        NVIC_EnableIRQ(LPTMR_IRQnTable[0]);
    }

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
 * @brief  注册中断回调函数
 * @param  AppCBFun: 回调函数指针入口
 * @retval None
 * @note 对于此函数的具体应用请查阅应用实例
 */


/**
 * @brief  获取脉冲计数器的脉冲数
 * @code
 *     //获取脉冲计数的个数
 *     uint32_t counter;
 *     counter = LPTMR_PC_ReadCounter();
 * @endcode
 * @param  None
 * @retval 脉冲计数个数
 */
uint32_t LPTMR_PC_ReadCounter(void)
{
    /* must first read */
    uint32_t *p = (uint32_t *)&LPTMR0->CNR;
    *p = 0x1234;
	return (uint32_t)((LPTMR0->CNR & LPTMR_CNR_COUNTER_MASK) >> LPTMR_CNR_COUNTER_SHIFT); 
}

void LPTMR_SetTime(uint32_t ms)
{
    LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK; 
    LPTMR0->CMR = LPTMR_CMR_COMPARE(ms);
    LPTMR0->CSR |= LPTMR_CSR_TEN_MASK; 
}

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
    for(i = 0; i < pq->pin_count; i++)
    {
        SetPinMux(pq->io, pq->pin_start + i, pq->mux); 
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
 * @param  None
 * @retval None
 */
void LPTMR_ClearCounter(void)
{
    /* disable and reenable moudle to clear counter */
    LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK;
    LPTMR0->CSR |= LPTMR_CSR_TEN_MASK;
}

/**
 * @brief  中断处理函数入口
 * @param  LPTimer_IRQHandler :LPTM中断处理函数
 * @note 函数内部用于中断事件处理
 */
//void LPTimer_IRQHandler(void)
//{
//    LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;

//}


#if 0
static const QuickInit_Type LPTMR_QuickInitTable[] = 
{
    { 0, 0, 6,19, 1, 1}, //LPTMR_ALT1_PA19 6
    { 0, 2, 4, 5, 1, 2}, //LPTMR_ALT2_PC05 4
};
#endif

