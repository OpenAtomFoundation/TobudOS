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
 * @file n32g020xx_dac.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @brief DAC模块的接口声明文件。
 * 
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * 
 * @defgroup DAC DAC数模转换模块
 * @{
 */

#ifndef __N32G020XX_DAC_H__
#define __N32G020XX_DAC_H__

#include "n32g020xx_common.h"
#include "n32g020xx.h"

//-----------------------------------------------------------------------------
//                        TypeDefs
//-----------------------------------------------------------------------------
/**
 * @brief DAC量化电平档位选择
 * 
 */
typedef enum VrefOptEnum
{
    VREF_OPT_1P2V = 0, ///< DAC的量化电平 保留值1.2V
    VREF_OPT_1P8V,     ///< DAC的量化电平 1.8V
    VREF_OPT_2P4V,     ///< DAC的量化电平 2.4V
    VREF_OPT_3P0V,     ///< DAC的量化电平 3.0V
} VrefOptEnum;

/**
 * @brief DAC输出管脚
 */
typedef enum DacOutPinEnum
{
    DAC_OUT0      = 0x01, ///< 输出到DAC_OUT0管脚
    DAC_OUT1      = 0x02, ///< 输出到DAC_OUT1管脚
} DacOutPinEnum;

/**
 * @brief DAC配置参数结构体
 */
typedef struct
{
    VrefOptEnum vrefOpt; ///< 量化电平
    DacOutPinEnum pin;  ///< 设置DAC输出管脚
    uint16_t dacClkDiv; ///< DAC分频值，取值1~1024
} DACConfig;

/**
 * @brief 定义DAC函数接口的返回值。
 */
typedef enum DACRetCode
{
    DAC_SUCCESS = 0,     ///< 操作成功完成
    DAC_PARAM_INCORRECT, ///< 参数错误
    DAC_FAIL,            ///< 操作失败
} DACRetCode;
//-----------------------------------------------------------------------------
//                        Exported variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                        Exported functions
//-----------------------------------------------------------------------------
//

/**
 * @brief DAC_Init  初始化dac
 * 
 * @param pDACConfig DAC配置参数结构体
 * 
 * @return 
 * - @ref DAC_SUCCESS 操作成功完成
 * - @ref DAC_PARAM_INCORRECT 配置参数错误
 * - @ref DAC_FAIL 操作失败
 * @see DACConfig
 */
int32_t DAC_Init(DACConfig* pDACConfig);

/**
 * @brief DAC_Output DAC输出模拟电平
 * 
 * @param data DAC输出值，低10位有效
 * 
 * @return DAC_SUCCESS 操作成功完成
 */
int32_t DAC_Output(uint16_t data);

/**
 * @brief DAC_Close 关闭DAC模块
 * 
 * @return DAC_SUCCESS 操作成功完成
 */
int32_t DAC_Close(void);

#endif // __N32G020XX_DAC_H__

/** @} */
