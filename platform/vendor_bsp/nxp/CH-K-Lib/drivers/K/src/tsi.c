/**
  ******************************************************************************
  * @file    tsi.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
	* \date    2015.10.08 FreeXc 完善了对 tsi 模块的相关注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片TSI模块的底层功能函数
  ******************************************************************************
  */
  
 #include "tsi.h"
 #include "common.h"
 #include "gpio.h"
 
#ifdef TSI0

static TSI_CallBackType TSI_CallBackTable[1] = {NULL};
 
static const uint16_t * TSI_ChlCNTRTable[] =
{
    (uint16_t*)&TSI0->CNTR1 + 0,
    (uint16_t*)&TSI0->CNTR1 + 1,
    (uint16_t*)&TSI0->CNTR3 + 0,
    (uint16_t*)&TSI0->CNTR3 + 1,
    (uint16_t*)&TSI0->CNTR5 + 0,
    (uint16_t*)&TSI0->CNTR5 + 1,
    (uint16_t*)&TSI0->CNTR7 + 0,
    (uint16_t*)&TSI0->CNTR7 + 1,
    (uint16_t*)&TSI0->CNTR9 + 0,
    (uint16_t*)&TSI0->CNTR9 + 1,
    (uint16_t*)&TSI0->CNTR11 + 0,
    (uint16_t*)&TSI0->CNTR11 + 1,
    (uint16_t*)&TSI0->CNTR13 + 0,
    (uint16_t*)&TSI0->CNTR13 + 1,
    (uint16_t*)&TSI0->CNTR15 + 0,
    (uint16_t*)&TSI0->CNTR15 + 1,
};

/**
 * @brief  获得指定通道的数值
 * @param[in]  chl tsi模块的通道1~15，详细请见tsi.h文件
 * @retval 该通道的数据值
 */
uint32_t TSI_GetCounter(uint32_t chl)
{
    return *TSI_ChlCNTRTable[chl];
}

/**
 * @brief  TSI通道校验函数，内部函数
 * @param[in]  chl      tsi模块的通道1~15
 * @param[in]  threshld 修正校验值
 * @retval None
 */
static void TSI_Calibration(uint32_t chl, uint32_t threshld)
{
    uint32_t cnt;
	TSI0->GENCS |= TSI_GENCS_TSIEN_MASK;
    TSI0->GENCS &= ~TSI_GENCS_STM_MASK;
	TSI0->GENCS |= TSI_GENCS_SWTS_MASK;
	while(!(TSI0->GENCS & TSI_GENCS_EOSF_MASK)){};
    cnt = TSI_GetCounter(chl);
    LIB_TRACE("TSI_Calibration cnt:%d\r\n", cnt);
    LIB_TRACE("TSI_Calibration threshld:%d\r\n", threshld);
	TSI0->THRESHOLD = TSI_THRESHOLD_HTHH(cnt + threshld) | TSI_THRESHOLD_LTHH(cnt - threshld);
    TSI0->GENCS &= ~TSI_GENCS_TSIEN_MASK;
}

/**
 * @brief  TSI初始化配置
 * @code
 *    //初始化配置TSI模块的1通道为周期触发方式，判断阀值为200
 *    TSI_InitTypeDef  TSI_InitStruct1;      //申请一个结构变量
 *    TSI_InitStruct1.chl = 1;   //选择通道
 *    TSI_InitStruct1.triggerMode = kTSI_TriggerPeriodicalScan; //硬件周期扫描
 *    TSI_InitStruct1.threshld = 200;   //设置阀值为200
 *    TSI_Init(&TSI_InitStruct1);
 * @endcode
 * @param[in]  TSI_InitStruct 指向TSI初始化结构体的指针,存储通道的工作状态
 * @retval None
 */
void TSI_Init(TSI_InitTypeDef* TSI_InitStruct)
{
	SIM->SCGC5 |= (SIM_SCGC5_TSI_MASK); 
    /* disalbe moudle */
    TSI0->GENCS = 0;
    /* Number of Consecutive Scans Per Electrode Electrode */
    TSI0->GENCS |= TSI_GENCS_NSCN(TSI_InitStruct->consecutiveScanTimes);
    /* Electrode Oscillator Prescaler */
    TSI0->GENCS |= TSI_GENCS_PS(TSI_InitStruct->electrodeOSCPrescaler);
    /* clear SCANC */
    TSI0->SCANC = 0;
    /* Ref OSC Charge Current Select */
    TSI0->SCANC |= TSI_SCANC_REFCHRG(TSI_InitStruct->refChargeCurrent);
    /* External OSC Charge Current Select */
    TSI0->SCANC |= TSI_SCANC_EXTCHRG(TSI_InitStruct->extChargeCurrent);
    /* clock source is bus clock LPOSCCLK? */
    TSI0->SCANC |= TSI_SCANC_SMOD(0)|TSI_SCANC_AMPSC(0);
    /* enable all pens */
    /* FIXME: seems if there is only one TSI pen, TSI cannot work in continues module(STM=1) */	
    TSI0->PEN |= (1<<TSI_InitStruct->chl|(1<<0));			
    TSI_Calibration(TSI_InitStruct->chl, TSI_InitStruct->threshld);
    /* config trigger mode */
    switch(TSI_InitStruct->triggerMode)
    {
        case kTSI_TriggerSoftware:
            TSI0->GENCS &= ~TSI_GENCS_STM_MASK;
            break;
        case kTSI_TriggerPeriodicalScan:
            TSI0->GENCS |= TSI_GENCS_STM_MASK;
            break;
        default:
            break;
    }
    /* clear all IT flags */
    TSI0->GENCS |= TSI_GENCS_OUTRGF_MASK;	  
	TSI0->GENCS |= TSI_GENCS_EOSF_MASK;	 
	TSI0->GENCS |= TSI_GENCS_EXTERF_MASK;
	TSI0->GENCS |= TSI_GENCS_OVRF_MASK;
//	TSI0->STATUS = 0xFFFFFFFF;
    /* enable moudle */
	TSI0->GENCS |= TSI_GENCS_TSIEN_MASK;
}

/**
 * @brief  TSI快速初始化配置
 * @code
 *    //快速初始化配置TSI模块的1通道的PTA0引脚做默认触控引脚
 *    TSI_QuickInit(TSI0_CH1_PA00);
 * @endcode
 * @param[in]  MAP TSI初始化预定义，详见tsi.h文件
 * @retval 通道号
 */
uint32_t TSI_QuickInit(uint32_t MAP)
{
    uint32_t i;
    TSI_InitTypeDef TSI_InitStruct;
    map_t * pq = (map_t*)&MAP;
    /* config pinmux */
    for(i = 0; i < pq->pin_cnt; i++)
    {
        PORT_PinMuxConfig(pq->io, pq->pin_start + i, (PORT_PinMux_Type) pq->mux);
    }
    /* config TSI moudle */
    TSI_InitStruct.chl = pq->chl;
    TSI_InitStruct.triggerMode = kTSI_TriggerPeriodicalScan;
    TSI_InitStruct.threshld = 700;
    TSI_InitStruct.consecutiveScanTimes = 2;
    TSI_InitStruct.electrodeOSCPrescaler = kTSI_EletrodeOscDiv_2;
    TSI_InitStruct.extChargeCurrent = kTSI_ChargeCurrent_32uA;
    TSI_InitStruct.refChargeCurrent = kTSI_ChargeCurrent_32uA;
    TSI_Init(&TSI_InitStruct);
    return pq->ip;
}

/**
 * @brief  TSI模块中断类型或者DMA功能设置
 * @code
 *      //开启扫描结束触发中断模式
 *      TSI_ITDMAConfig(kTSI_IT_EndOfScan); 
 * @endcode
 * @param[in] config 中断配置模式
 *         		@arg kTSI_IT_Disable 关闭中断功能
 *         		@arg kTSI_IT_OutOfRange 超出阀值触发中断
 *         		@arg kTSI_IT_EndOfScan 扫描结束触发中断
 * @retval None
 */
void TSI_ITDMAConfig(TSI_ITDMAConfig_Type config)
{
    switch(config)
    {
        case kTSI_IT_Disable:
            TSI0->GENCS &= ~TSI_GENCS_TSIIE_MASK;
            break;
        case kTSI_IT_OutOfRange:
            TSI0->GENCS |= TSI_GENCS_TSIIE_MASK;
            TSI0->GENCS &= ~TSI_GENCS_ESOR_MASK;
            NVIC_EnableIRQ(TSI0_IRQn);
            break;
        case kTSI_IT_EndOfScan:
            TSI0->GENCS |= TSI_GENCS_TSIIE_MASK;
            TSI0->GENCS |= TSI_GENCS_ESOR_MASK;
            NVIC_EnableIRQ(TSI0_IRQn);
            break; 
        default:
            break;
    }
}

/**
 * @brief  注册中断回调函数
 * @param[in] AppCBFun 回调函数指针入口
 * @retval None
 * @note 对于此函数的具体应用请查阅应用实例
 */
void TSI_CallbackInstall(TSI_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        TSI_CallBackTable[0] = AppCBFun;
    }
}
/**
 * @brief  中断处理函数入口
 * @note 函数内部用于中断事件处理(调用用户注册的回调函数)
 */
void TSI0_IRQHandler(void)
{
    /* clear all IT pending bit */
    TSI0->GENCS |= TSI_GENCS_OUTRGF_MASK;
	TSI0->GENCS |= TSI_GENCS_EOSF_MASK;
  //  chl_array = TSI0->STATUS & 0xFFFF;
   // TSI0->STATUS = 0xFFFFFFFF;
    if(TSI_CallBackTable[0])
    {
        TSI_CallBackTable[0]();
    }  
}

#endif

/*
 static  map_t TSI_QuickInitTable[] =
{
    { 0, 1, 0, 0, 1, 0},    //TSI0_CH0_PB00
    { 0, 0, 0, 0, 1, 1},    //TSI0_CH1_PA00
    { 0, 0, 0, 1, 1, 2},    //TSI0_CH2_PA01
    { 0, 0, 0, 2, 1, 3},    //TSI0_CH3_PA02
    { 0, 0, 0, 3, 1, 4},    //TSI0_CH4_PA03
    { 0, 0, 0, 4, 1, 5},    //TSI0_CH5_PA04
    { 0, 1, 0, 1, 1, 6},    //TSI0_CH6_PB01
    { 0, 1, 0, 2, 1, 7},    //TSI0_CH7_PB02
    { 0, 1, 0, 3, 1, 8},    //TSI0_CH8_PB03
    { 0, 1, 0,16, 1, 9},    //TSI0_CH9_PB16
    { 0, 1, 0,17, 1,10},    //TSI0_CH10_PB17
    { 0, 1, 0,18, 1,11},    //TSI0_CH11_PB18
    { 0, 1, 0,19, 1,12},    //TSI0_CH12_PB19
    { 0, 2, 0, 0, 1,13},    //TSI0_CH13_PC00
    { 0, 2, 0, 1, 1,14},    //TSI0_CH14_PC01
    { 0, 2, 0, 2, 1,15},    //TSI0_CH15_PC02
};
*/
