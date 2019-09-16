/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/
/**
 * @brief ADC 模块接口定义文件。
 * @file n32g020xx_adc.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 *
 * @defgroup ADC ADC模数转换模块
 * @{
 */

#ifndef __N32G020XX_ADC_H__
#define __N32G020XX_ADC_H__

#include "n32g020xx_common.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif // defined __cplusplus

/**
 * @brief 定义ADC模块函数接口的返回值。
 */
typedef enum AdcRetCode
{
    ADC_SUCCESS    = 0,  ///< 操作成功完成
    ADC_NONE_VALUE = -1, ///< 没有读到有效数据
    ADC_ONE_VALUE  = 1,  ///< 读取1个数据
} AdcRetCode;

/**
 * @brief ACD 通道号
 * 
 * @return  
 */
typedef enum AdcChannel
{
    ADC_EN_SINGLE_CH0 = 0x0001,
    ADC_EN_SINGLE_CH1 = 0x0002,
    ADC_EN_SINGLE_CH2 = 0x0004,
    ADC_EN_SINGLE_CH3 = 0x0008,
    ADC_EN_SINGLE_CH4 = 0x0010,
    ADC_EN_SINGLE_CH5 = 0x0020,
    ADC_EN_SINGLE_CH6 = 0x0040,
    ADC_EN_SINGLE_CH7 = 0x0080,
    ADC_EN_SINGLE_CH8 = 0x0100,
    ADC_EN_SINGLE_CH9 = 0x0200,
    ADC_EN_SINGLE_TEMP = 0x0400L, ///< 芯片内部温度通道
    ADC_EN_SINGLE_VCC = 0x0800L, ///< 芯片供电电压通道
} AdcChannel;
/**
 * @fn ADC_CallBack
 *
 * @brief 当ADC转换完成有时数时的回调函数。
 *
 * @param ChNum 表示通道号
 * @param AdVal 表示通道值
 *
 * @return ADC_SUCCESS
 * @see ADC_IntialConfig
 */
typedef int32_t (*ADC_CallBack)(uint8_t ChNum, uint16_t AdVal);

/**
 * @brief ADC初始化配置结构体。
 */
typedef struct
{
    uint16_t Chs;
    uint16_t AdcDiv;
    ADC_CallBack pADC_CallBack;
    bool InnerRef;
    bool LoopSampleEn;
} AdcParamInitial;

/**
 * @brief  初始化ADC模块配置
 *
 * 如果是单个采样，不需要注册中断回调函数
 * 所有通道通过Chs参数一次性设置进去
 * InnerRef=true时，内部电压应为1.2V
 * AdcDiv为ADC模块的时钟分频值，越大表示AD采样时钟越慢
 *
 * @param  pAdcParamInitial 初始化配置参数
 * @return ADC_SUCCESS
 */
int32_t ADC_IntialConfig(AdcParamInitial* pAdcParamInitial);

/**
 * @brief  ADC启动
 *
 * 在ADC_IntialConfig初始化完成后调用
 * @return Null
 */
int32_t ADC_Start(void);

/**
 * @brief  ADC停止
 * @param  ClrIrqCallBack 为true会清掉回调函数
 * @return ADC_SUCCESS
 */
int32_t ADC_Stop(bool ClrIrqCallBack);

/**
 * @brief  读ADC值
 *
 * 一般用于没有注册中断回调函数时调用
 * @param ChNum 通道号
 * @param AdVal 存放读取结果
 * @return ADC_NONE_VALUE 表示没有读到转换结果，ADC_ONE_VALUE表示读取到1个转换结果
 */
int32_t ADC_ReadValue(uint8_t* ChNum, uint16_t* AdVal);

#define ADC_DMA_REQ_EN 0x01L    ///< Dma 使能
#define ADC_DMA_REQ_BY_NE 0x02L ///< NotEmpty 触发Dma
#define ADC_DMA_REQ_BY_HF 0x04L ///< HalfFull 触发Dma请求

/**
 * @brief  ADC中断处理函数
 */
void ADAC_IRQHandler_Adc(void);


/**
 * @brief  设置设置ADC的DMA配置
 * @param  Flags 为ADC_DMA_REQ_EN，ADC_DMA_REQ_BY_NE，ADC_DMA_REQ_BY_HF的组合
 * @return ADC_SUCCESS
 */
int32_t ADC_SetDmaCfg(uint32_t Flags);

#ifdef __cplusplus
}
#endif // defined __cplusplus

#endif

/**
 * @}
 */
