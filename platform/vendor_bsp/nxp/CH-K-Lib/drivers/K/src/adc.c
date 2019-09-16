/**
  ******************************************************************************
  * @file    adc.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.25
  * @date    2015.9.26 FreeXc 完善了adc.c & adc.h文件的注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片ADC模块的底层功能函数
  ******************************************************************************
  */

#include "common.h"
#include "adc.h"
#include "gpio.h"

/* leagacy support for Kineis Z Version(inital version) */
#if (!defined(ADC_BASES))
#ifdef ADC1
    #define ADC_BASES {ADC0, ADC1}
#elif ADC2
    #define ADC_BASES {ADC0, ADC1,ADC2}
#else
    #define ADC_BASES {ADC0}
#endif


#endif
/* global vars */
static ADC_Type * const ADC_InstanceTable[] = ADC_BASES;
static ADC_CallBackType ADC_CallBackTable[ARRAY_SIZE(ADC_InstanceTable)] = {NULL};

static const Reg_t SIM_ADCClockGateTable[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_ADC0_MASK},
#ifdef SIM_SCGC3_ADC1_MASK
    {(void*)&(SIM->SCGC3), SIM_SCGC3_ADC1_MASK},
#endif
#ifdef SIM_SCGC6_ADC1_MASK
    {(void*)&(SIM->SCGC6), SIM_SCGC6_ADC1_MASK},
#endif
    
#ifdef ADC2
    {(void*)&(SIM->SCGC3), SIM_SCGC6_ADC2_MASK},
#endif
};
static const IRQn_Type ADC_IRQnTable[] = 
{
    ADC0_IRQn,
#ifdef ADC1
    ADC1_IRQn,
#endif
#ifdef ADC2
    ADC2_IRQn,
#endif
};


/**
 * @brief  判断AD转换是否结束
 * @code
 *      //查看ADC0 模块的A通道的转换是否完成
 *      ADC_IsConversionCompleted(HW_ADC0, kADC_MuxA); 
 * @endcode         
 * @param[in]  instance: ADC 模块号
 *              @arg HW_ADC0  :ADC0模块
 *              @arg HW_ADC1  :ADC1模块
 *              @arg HW_ADC2  :ADC2模块
 * @param[in]  mux: ADC 转换器通道 复用 选择
 *              @arg kADC_MuxA   :A通道模式
 *              @arg kADC_MuxB   :B通道模式
 * \retval  0 转换完成
 * \retval  1 转换未完成
 */
uint8_t ADC_IsConversionCompleted(uint32_t instance, uint32_t mux)
{
    if(ADC_InstanceTable[instance]->SC1[mux] & ADC_SC1_COCO_MASK)
    {
        return 0;
    }
    return 1;
    //return (!((ADC_InstanceTable[instance]->SC1[mux]) & ADC_SC1_COCO_MASK) >> ADC_SC1_COCO_SHIFT);  
}

/**
 * @brief  AD采集校准函数，内部函数,用户无需调用
 * @param[in]  instance: ADC 模块号
 *              @arg HW_ADC0  :ADC0模块
 *              @arg HW_ADC1  :ADC1模块
 *              @arg HW_ADC2  :ADC2模块
 * \retval  0 校准完成
 * \retval  1 校准失败
 */
int32_t ADC_Calibration(uint32_t instance)
{
    uint32_t i;
    uint32_t PG, MG;
    
    // disable clock 
    
    ADC_InstanceTable[instance]->SC3 |= ADC_SC3_CALF_MASK; /* Clear the calibration's flag */
    ADC_InstanceTable[instance]->SC3 |= ADC_SC3_CAL_MASK;  /* Enable the calibration */

    for(i=0;i<100000;i++);
    //while(ADC_IsConversionCompleted(instance, 0)) {};      /* Wait conversion is competed */
    if(ADC_InstanceTable[instance]->SC3 & ADC_SC3_CALF_MASK)
    {
        /* cal failed */
        LIB_TRACE("ADC calibration failed\r\n");
        return 1;
    }
    /* Calculate plus-side calibration */
    PG = 0U;
    PG += ADC_InstanceTable[instance]->CLP0;
    PG += ADC_InstanceTable[instance]->CLP1;
    PG += ADC_InstanceTable[instance]->CLP2;
    PG += ADC_InstanceTable[instance]->CLP3;
    PG += ADC_InstanceTable[instance]->CLP4;
    PG += ADC_InstanceTable[instance]->CLPS;
    PG = 0x8000U | (PG>>1U);
    
    /* Calculate minus-side calibration */
    MG = 0U;
    MG += ADC_InstanceTable[instance]->CLM0;
    MG += ADC_InstanceTable[instance]->CLM1;
    MG += ADC_InstanceTable[instance]->CLM2;
    MG += ADC_InstanceTable[instance]->CLM3;
    MG += ADC_InstanceTable[instance]->CLM4;
    MG += ADC_InstanceTable[instance]->CLMS;
    MG = 0x8000U | (MG>>1U);
    ADC_InstanceTable[instance]->SC3 &= ~ADC_SC3_CAL_MASK;  /* Disable the calibration */
    ADC_InstanceTable[instance]->PG = PG;
    ADC_InstanceTable[instance]->MG = MG;
    return 0;
}

/**
 * @brief  ADC模块工作初始化配置
 * @code
 *   //使用adc0模块的1通道 单端模式 8位精度 软件触发
 *   ADC_InitTypeDef ADC_InitStruct1;  //申请一个结构体
 *   ADC_InitStruct1.chl = 1;  //1通道
 *   ADC_InitStruct1.clockDiv = kADC_ClockDiv8; //ADC转换时钟为输入时钟(默认BusClock) 的8分频，和转换速度相关
 *   ADC_InitStruct1.instance = HW_ADC0;        //选择ADC0模块
 *   ADC_InitStruct1.resolutionMode = kADC_SingleDiff8or9; //单端模式下8位精度 查分模式下9位精度
 *   ADC_InitStruct1.SingleOrDifferential = kADC_Single;   //选择单端模式
 *   ADC_InitStruct1.triggerMode = kADC_TriggleSoftware;   //设置为软件触发
 *   ADC_InitStruct1.vref = kADC_VoltageVREF;              //使用外部VERFH VREFL 作为模拟电压参考
 *   //初始化ADC模块
 *   ADC_Init(&ADC_InitStruct1);
 * @endcode
 * @param[in]  ADC_InitStruct: ADC初始化结构体，内容详见注释
 * @retval None
 */
void ADC_Init(ADC_InitTypeDef* ADC_InitStruct)
{
    /* enable clock gate */
    *(uint32_t*)SIM_ADCClockGateTable[ADC_InitStruct->instance].addr |= SIM_ADCClockGateTable[ADC_InitStruct->instance].mask;
    
    /* do calibration */
    //ADC_Calibration(ADC_InitStruct->instance);
    
    /* select clock */
    //ADC_InstanceTable[ADC_InitStruct->instance]->CFG2 |= ADC_CFG2_ADACKEN_MASK;
    //ADC_InstanceTable[ADC_InitStruct->instance]->CFG1 &= ~ADC_CFG1_ADICLK_MASK;
    //ADC_InstanceTable[ADC_InitStruct->instance]->CFG1 |= ADC_CFG1_ADICLK(3);
    
	/* set clock configuration */
	ADC_InstanceTable[ADC_InitStruct->instance]->CFG1 &= ~ADC_CFG1_ADIV_MASK;
	ADC_InstanceTable[ADC_InitStruct->instance]->CFG1 |=  ADC_CFG1_ADIV(ADC_InitStruct->clockDiv); 
    
    /* long sample time */
	ADC_InstanceTable[ADC_InitStruct->instance]->CFG1 |=  ADC_CFG1_ADLPC_MASK;
    
    /* voltage reference */
    ADC_InstanceTable[ADC_InitStruct->instance]->SC2 &= ~ADC_SC2_REFSEL_MASK;
    ADC_InstanceTable[ADC_InitStruct->instance]->SC2 |= ADC_SC2_REFSEL(ADC_InitStruct->vref);
    
    /* resolutionMode */
	ADC_InstanceTable[ADC_InitStruct->instance]->CFG1 &= ~(ADC_CFG1_MODE_MASK); 
	ADC_InstanceTable[ADC_InitStruct->instance]->CFG1 |= ADC_CFG1_MODE(ADC_InitStruct->resolutionMode);
    
    /* trigger mode */
    (kADC_TriggerHardware == ADC_InitStruct->triggerMode)?(ADC_InstanceTable[ADC_InitStruct->instance]->SC2 |=  ADC_SC2_ADTRG_MASK):(ADC_InstanceTable[ADC_InitStruct->instance]->SC2 &=  ADC_SC2_ADTRG_MASK);
    
    /* if continues conversion */
    (kADC_ContinueConversionEnable == ADC_InitStruct->continueMode)?(ADC_InstanceTable[ADC_InitStruct->instance]->SC3 |= ADC_SC3_ADCO_MASK):(ADC_InstanceTable[ADC_InitStruct->instance]->SC3 &= ~ADC_SC3_ADCO_MASK);
    
    /* if hardware average enabled */
    switch(ADC_InitStruct->hardwareAveMode)
    {
        case kADC_HardwareAverageDisable:
            ADC_InstanceTable[ADC_InitStruct->instance]->SC3 &= ~ADC_SC3_AVGE_MASK;
            break;
        case kADC_HardwareAverage_4:
            ADC_InstanceTable[ADC_InitStruct->instance]->SC3 |= ADC_SC3_AVGE_MASK;
            ADC_InstanceTable[ADC_InitStruct->instance]->SC3 &= ~ADC_SC3_AVGS_MASK;
            ADC_InstanceTable[ADC_InitStruct->instance]->SC3 |= ADC_SC3_AVGS(0);
            break;
        case kADC_HardwareAverage_8:
            ADC_InstanceTable[ADC_InitStruct->instance]->SC3 |= ADC_SC3_AVGE_MASK;
            ADC_InstanceTable[ADC_InitStruct->instance]->SC3 &= ~ADC_SC3_AVGS_MASK;
            ADC_InstanceTable[ADC_InitStruct->instance]->SC3 |= ADC_SC3_AVGS(1);
            break;
        case kADC_HardwareAverage_16:
            ADC_InstanceTable[ADC_InitStruct->instance]->SC3 |= ADC_SC3_AVGE_MASK;
            ADC_InstanceTable[ADC_InitStruct->instance]->SC3 &= ~ADC_SC3_AVGS_MASK;
            ADC_InstanceTable[ADC_InitStruct->instance]->SC3 |= ADC_SC3_AVGS(2);
            break;
        case kADC_HardwareAverage_32:
            ADC_InstanceTable[ADC_InitStruct->instance]->SC3 |= ADC_SC3_AVGE_MASK;
            ADC_InstanceTable[ADC_InitStruct->instance]->SC3 &= ~ADC_SC3_AVGS_MASK;
            ADC_InstanceTable[ADC_InitStruct->instance]->SC3 |= ADC_SC3_AVGS(3);
            break;
        default:
            break;
    }
    
    ADC_Calibration(ADC_InitStruct->instance);
}

/**
 * @brief  AD采集硬件/软件触发选择
 * @param[in]  instance ADC 模块号
 *              @arg HW_ADC0  ADC0模块
 *              @arg HW_ADC1  ADC1模块
 *              @arg HW_ADC2  ADC2模块
 * \param[in]  status Conversion trigger select
 *              \arg 0 Software trigger selected
 *              \arg 1 Hardware trigger selected
 * \retval  None
 */
void ADC_EnableHardwareTrigger(uint32_t instance, bool status)
{
    (status)?
    (ADC_InstanceTable[instance]->SC2 |=  ADC_SC2_ADTRG_MASK):
    (ADC_InstanceTable[instance]->SC2 &=  ADC_SC2_ADTRG_MASK);
}

/**
 * @brief  快速完成AD初始化配置
 * @code
 *    //初始化 ADC0 通道20 引脚DM1 单端 精度 12位
 *    ADC_QuickInit(ADC0_SE20_DM1, kADC_SingleDiff12or13);
 *    //读取AD转换结果
 *    value = ADC_QuickReadValue(ADC0_SE20_DM1);
 * @endcode
 * @param[in]  MAP 快速初始化宏，详见ADC.H文件
 * @param[in]  resolutionMode  转换分辨率设置
 *              @arg kADC_SingleDiff8or9   转换精度为8/9位
 *              @arg kADC_SingleDiff10or11 转换精度为10/11位
 *              @arg kADC_SingleDiff12or13 转换精度为12/13位
 *              @arg kADC_SingleDIff16     转换精度为16位
 * @retval ADC模块号
 */
uint8_t ADC_QuickInit(uint32_t MAP, ADC_ResolutionMode_Type resolutionMode)
{
    uint8_t i;
    map_t * pq = (map_t*)&(MAP);
    ADC_InitTypeDef AD_InitStruct1;
    AD_InitStruct1.instance = pq->ip;
    AD_InitStruct1.clockDiv = kADC_ClockDiv2;
    AD_InitStruct1.resolutionMode = resolutionMode;
    AD_InitStruct1.triggerMode = kADC_TriggerSoftware;
    AD_InitStruct1.singleOrDiffMode = kADC_Single;
    AD_InitStruct1.continueMode = kADC_ContinueConversionDisable;
    AD_InitStruct1.hardwareAveMode = kADC_HardwareAverageDisable;
    AD_InitStruct1.vref = kADC_VoltageVREF;
    
    /* init pinmux */
    for(i = 0; i < pq->pin_cnt; i++)
    {
        PORT_PinMuxConfig(pq->io, pq->pin_start + i, (PORT_PinMux_Type) pq->mux);
        PORT_PinPullConfig(pq->io, pq->pin_start + i, kPullDisabled); 
    }
    /* init moudle */
    ADC_Init(&AD_InitStruct1);
    
    /* init adc chlmux */
    ADC_ChlMuxConfig(pq->ip, pq->reserved);
    
    return pq->ip;
}
  
/**
 * @brief  ADC开始一次转换 
 * @note   立即返回 非阻塞式 不等待转换结果
 * @code
 *    //启动 ADC0 通道20 在A模式下数据转换
 *    ADC_StartConversion(HW_ADC0, 20, kADC_MuxA);
 * @endcode
 * @param[in]  instance: ADC 模块号
 *              @arg HW_ADC0  ADC0模块
 *              @arg HW_ADC1  ADC1模块
 *              @arg HW_ADC2  ADC2模块
 * @param[in]  chl: ADC 通道号
 * @param[in]  mux: ADC 转换器通道 复用 选择
 *              @arg kADC_MuxA   A转换器触发
 *              @arg kADC_MuxB   B转换器触发
 * @retval None
 */
void ADC_StartConversion(uint32_t instance, uint32_t chl, uint32_t mux)
{
    ADC_InstanceTable[instance]->SC1[mux] &= ~(ADC_SC1_ADCH_MASK);
    ADC_InstanceTable[instance]->SC1[mux] |= ADC_SC1_ADCH(chl);
}

/**
 * @brief  ADC Mux select 
 * @code
 *    //启动 ADC0 通道在A模式下进行数据转换
 *    ADC_ChlMuxConfig(HW_ADC0, kADC_MuxA);
 * @endcode
 * @param[in]  instance ADC模块号
 *              @arg HW_ADC0  ADC0模块
 *              @arg HW_ADC1  ADC1模块
 *              @arg HW_ADC2  ADC2模块
 * @param[in]  mux ADC转换器通道复用选择
 *              @arg kADC_MuxA   A转换器触发
 *              @arg kADC_MuxB   B转换器触发
 * @retval None
 */
void ADC_ChlMuxConfig(uint32_t instance, uint32_t mux)
{
    if(kADC_ChlMuxA ==  mux)
    {
        ADC_InstanceTable[instance]->CFG2 &= ~ADC_CFG2_MUXSEL_MASK;
    }
    if(kADC_ChlMuxB ==  mux)
    {
        ADC_InstanceTable[instance]->CFG2 |= ADC_CFG2_MUXSEL_MASK; 
    }
}

/**
 * @brief  读取ADC转换数据
 * @note   立即返回 非阻塞式 
 * @code
 *    //读取 ADC0模块下的在A模式下数据转换结果
 *    uint32_t data;   //存储转换结果
 *    data = ADC_ReadValue(HW_ADC0, kADC_MuxA);
 * @endcode
 * @param[in]  instance: ADC 模块号
 *              @arg HW_ADC0  :ADC0模块
 *              @arg HW_ADC1  :ADC1模块
 *              @arg HW_ADC2  :ADC2模块
 * @param[in]  mux: ADC 转换器通道 复用 选择
 *              @arg kADC_MuxA   :A通道模式
 *              @arg kADC_MuxB   :B通道模式
 * @retval 读取结果 如果当前还未完成转换 则返回上一次结果
 */
int32_t ADC_ReadValue(uint32_t instance, uint32_t mux)
{
    return ADC_InstanceTable[instance]->R[mux];
}

/**
 * @brief  读取ADC转换结果(简化版) 只需填入ADC快速初始化宏即可
 * @note   阻塞式 直到数据转换完成    
 * @code
 *    //读取AD0模块20通道DM1引脚的转换结果
 *    uint32_t value;   //存储数据转换结果
 *    value = ADC_QuickReadValue(ADC0_SE20_DM1);
 * @endcode
 * @param  MAP 快速初始化宏，详见ADC.H文件
 * @retval 转换结果
 */
int32_t ADC_QuickReadValue(uint32_t MAP)
{
    map_t * pq = (map_t*)&(MAP);
    uint32_t instance = pq->ip;
    uint32_t chl = pq->chl;
    ADC_StartConversion(instance, chl, kADC_MuxA);
    /* waiting for ADC complete */
    while((ADC_InstanceTable[instance]->SC1[kADC_MuxA] & ADC_SC1_COCO_MASK) == 0);
    return ADC_ReadValue(instance, kADC_MuxA);
}

/**
 * @brief  ADC中断及DMA功能开关函数
 * @code
 *    //配置AD0模块 转换完成中断
 *    ADC_ITDMAConfig(HW_ADC0, kADC_MuxA, kADC_IT_EOF);
 * @endcode         
 * @param[in]  instance ADC模块号
 *              @arg HW_ADC0  ADC0模块
 *              @arg HW_ADC1  ADC1模块
 *              @arg HW_ADC2  ADC2模块
 * @param[in]  mux ADC转换器通道复用选择
 *              @arg kADC_MuxA   A通道模式
 *              @arg kADC_MuxB   B通道模式
 * @param[in]  config   ADC中断及DMA配置
 *              @arg kADC_IT_Disable    关闭中断
 *              @arg kADC_DMA_Disable   关闭DMA功能 
 *              @arg kADC_IT_EOF        转换完成中断
 *              @arg kADC_DMA_EOF       DMA完成中断
 * @retval None
 */
void ADC_ITDMAConfig(uint32_t instance, uint32_t mux, ADC_ITDMAConfig_Type config)
{
    switch(config)
    {
        case kADC_IT_Disable:
            NVIC_DisableIRQ(ADC_IRQnTable[instance]);
            ADC_InstanceTable[instance]->SC1[mux] &= ~ADC_SC1_AIEN_MASK;
            break;
        case kADC_DMA_Disable:
            ADC_InstanceTable[instance]->SC2 &= ~ADC_SC2_DMAEN_MASK;
            break;
        case kADC_IT_EOF:
            ADC_InstanceTable[instance]->SC1[mux] |= ADC_SC1_AIEN_MASK;
            NVIC_EnableIRQ(ADC_IRQnTable[instance]);
            break; 
        case kADC_DMA_EOF:
            ADC_InstanceTable[instance]->SC2 |= ADC_SC2_DMAEN_MASK;
            break;
        default:
            break;
    }
}

/**
 * @brief  注册中断回调函数
 * @param[in]  instance ADC 模块号
 *              @arg HW_ADC0  ADC0模块
 *              @arg HW_ADC1  ADC1模块
 *              @arg HW_ADC2  ADC2模块
 * @param[in] AppCBFun 回调函数指针入口
 * @retval None
 * @see 对于此函数的具体应用请查阅应用实例
 */
void ADC_CallbackInstall(uint32_t instance, ADC_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        ADC_CallBackTable[instance] = AppCBFun;
    }
}

/**
 * @brief  中断处理函数入口
 * @details  ADC0_IRQHandler :芯片的ADC0模块中断函数入口
 * @note 函数内部用于中断事件处理，用户无需使用
 */
void ADC0_IRQHandler(void)
{
    if(ADC_CallBackTable[HW_ADC0] != NULL)
    {
        ADC_CallBackTable[HW_ADC0]();
    }
}

/**
 * @brief  中断处理函数入口
 * @details  ADC1_IRQHandler :芯片的ADC0模块中断函数入口
 * @note 函数内部用于中断事件处理,用户无需使用
 */
void ADC1_IRQHandler(void)
{
    if(ADC_CallBackTable[HW_ADC1] != NULL)
    {
        ADC_CallBackTable[HW_ADC1]();
    }
}

/*
const map_t FTM_QuickInitTable[] =
{
    { 0, 0, 0, 0, 0, 0, 0}, //ADC0_SE0_DP0
    { 0, 0, 0, 0, 0, 1, 0}, //ADC0_SE1_DP1
    { 0, 0, 0, 0, 0, 3, 0}, //ADC0_SE3_DP3
    { 0, 2, 0, 2, 1, 4, 1}, //ADC0_SE4B_PC2
    { 0, 3, 0, 1, 1, 5, 1}, //ADC0_SE5B_PD1
    { 0, 3, 0, 5, 1, 6, 1}, //ADC0_SE6B_PD5
    { 0, 3, 0, 6, 1, 7, 1}, //ADC0_SE7B_PD6
    { 0, 1, 0, 0, 1, 8, 0}, //ADC0_SE8_PB0
    { 0, 1, 0, 1, 1, 9, 0}, //ADC0_SE9_PB1
    { 0, 1, 0, 2, 1,12, 0}, //ADC0_SE12_PB2
    { 0, 1, 0, 3, 1,13, 0}, //ADC0_SE13_PB3
    { 0, 2, 0, 0, 1,14, 0}, //ADC0_SE14_PC0
    { 0, 2, 0, 1, 1,15, 0}, //ADC0_SE15_PC1
    { 0, 4, 0,24, 1,17, 0}, //ADC0_SE17_E24
    { 0, 4, 0,25, 1,18, 0}, //ADC0_SE18_E25
    { 0, 0, 0, 0, 0,19, 0}, //ADC0_SE19_DM0
    { 0, 0, 0, 0, 0,20, 0}, //ADC0_SE20_DM1
    { 0, 0, 0, 0, 0,26, 0}, //ADC0_SE26_TEMP
    { 1, 0, 0, 0, 0, 0, 0}, //ADC1_SE0_DP0
    { 1, 0, 0, 0, 0, 1, 0}, //ADC1_SE1_DP1
    { 1, 0, 0, 0, 0, 3, 0}, //ADC1_SE3_DP3
    { 1, 4, 0, 0, 1, 4, 0}, //ADC1_SE4_PE0
    { 1, 4, 0, 1, 1, 5, 0}, //ADC1_SE5_PE1
    { 1, 4, 0, 2, 1, 6, 0}, //ADC1_SE6_PE2
    { 1, 4, 0, 3, 1, 7, 0}, //ADC1_SE7_PE3
    { 1, 2, 0, 8, 1, 4, 1}, //ADC1_SE4B_PC8
    { 1, 2, 0, 9, 1, 5, 1}, //ADC1_SE5B_PC9
    { 1, 2, 0,10, 1, 6, 1}, //ADC1_SE6B_PC10
    { 1, 2, 0,11, 1, 7, 1}, //ADC1_SE7B_PC11
    { 1, 1, 0, 0, 1, 8, 0}, //ADC1_SE8_PB0
    { 1, 1, 0, 1, 1, 9, 0}, //ADC1_SE9_PB1
    { 1, 1, 0,10, 1,14, 0}, //ADC1_SE14_PB10
    { 1, 1, 0,11, 1,15, 0}, //ADC1_SE15_PB11
    { 1, 0, 0,17, 1,17, 0}, //ADC1_SE17_PA17
    { 1, 0, 0, 0, 0,19, 0}, //ADC1_SE19_DM0
    { 1, 0, 0, 0, 0,20, 0}, //ADC1_SE20_DM1
    { 1, 0, 0, 0, 0,26, 0}, //ADC1_SE26_TEMP
    { 0, 0, 0, 0, 0,21, 0}, //ADC0_SE21
    { 0, 0, 0, 0, 0,22, 0}, //ADC0_SE22
    { 1, 0, 0, 0, 0,16, 0}, //ADC1_SE16
    { 1, 0, 0, 0, 0,18, 0}, //ADC1_SE18
    { 0, 0, 0, 0, 0,23, 0}, //ADC0_SE23
    { 1, 0, 0, 0, 0,23, 0}, //ADC1_SE23
    { 1, 0, 0, 0, 0,23, 0}, //ADC1_SE23
    { 1, 1, 0, 4, 1,10, 0}, //ADC1_SE10_B04
    { 1, 1, 0, 5, 1,11, 0}, //ADC1_SE11_B05
};
*/

