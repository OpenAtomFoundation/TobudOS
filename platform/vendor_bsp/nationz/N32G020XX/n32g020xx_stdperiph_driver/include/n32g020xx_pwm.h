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
 * @brief PWM 模块接口声明文件。
 * @file n32g020xx_pwm.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * 
 * @defgroup PWM PWM模块
 * @{
 */

#ifndef __N32G020XX_PWM_H__
#define __N32G020XX_PWM_H__

#include "n32g020xx_common.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif // defined __cplusplus

/**
 * @brief 定义PWM模块函数接口的返回值。
 */
typedef enum PwmRetCode
{
    PWM_SUCCESS = 0, ///< 操作成功完成
} PwmRetCode;

/**
 * @brief PWM通道号
 */
typedef enum PwmChannel
{
    PWM_CH0 = 0,
    PWM_CH1,
    PWM_CH2,
    PWM_CH3,
    PWM_CH4,
    PWM_CH5,
    PWM_CH6,
    PWM_CH7,
} PwmChannel;

/**
 * @brief  更变相应通道占空比
 * 
 * @param  ch 取值范围为 0--7
 * @param  DutyCycle 占空比，放大了1000倍,eg:DutyCycle=10为千分之10
 * @return PWM_SUCCESS
 */
int32_t PWM_ChangePwmDutyCycle(uint8_t ch, uint16_t DutyCycle);

/**
 * @brief  PWM配置
 * 
 * @param  ch 取值范围为 0--7
 * @param  PolHigh 高电平或低电平有效
 * @param  DutyCycle 占空比，放大了1000倍,eg:DutyCycle=10为千分之10
 * @return PWM_SUCCESS
 */
int32_t PWM_ConfigContinue(uint8_t ch, bool PolHigh, uint16_t DutyCycle);

/**
 * @fn PWM_CallBack
 *
 * @brief  PWM回调函数
 * 
 * @param  ch 取值范围为 0--7
 * @param  DutyCycle 占空比，放大了1000倍,eg:DutyCycle=10为千分之10
 * @return PWM_SUCCESS
 */
typedef int32_t (*PWM_CallBack)(uint8_t ch, uint16_t* DutyCycle);

/**
 * @brief  PWM配置
 * 
 * @param  ch 取值范围为 0--7
 * @param  PolHigh 高电平或低电平有效
 * @param  DutyCycle 占空比，放大了1000倍,eg:DutyCycle=10为千分之10
 * @param  pPWM_CallBack 回调函数
 * @return PWM_SUCCESS
 */
int32_t PWM_ConfigIrq(uint8_t ch, bool PolHigh, uint16_t DutyCycle, PWM_CallBack pPWM_CallBack);

/**
 * @brief  PWM开始
 * 
 * @param  ChBitS 一次开启所有需要开启的PWM通道，相应位置1为有效
 * @return PWM_SUCCESS
 */
int32_t PWM_Start(uint8_t ChBitS);

/**
 * @brief  PWM关闭
 * 
 * @param  ChBitS 一次关闭所有需要开启的PWM通道，相应位置1为有效
 * @return PWM_SUCCESS
 */
int32_t PWM_Stop(uint8_t ChBitS);

typedef enum
{
    PWM_GPIO14 = 0,
    PWM_GPIO15,
    PWM_GPIO16,
    PWM_GPIO17,
    PWM_GPIO18,
    PWM_GPIO20,
    PWM_GPIO21,
    PWM_GPIO22,
    PWM_GPIO23,
    PWM_GPIO_NUL,
} PWM_EnumBrakIO;
/**
 * @brief  刹车配置
 * 
 * @param  ChBitS 需要配置的刹车通道，相应位置1为有效
 * @param  BreakEn 刹车使能
 * @param  BreakInLogic 刹车使能的输入逻辑电平
 * @param  BreakOutLogic 刹车发生后的输出逻辑电平
 * @return PWM_SUCCESS
 */
int32_t PWM_BreakConfig(uint8_t ChBitS, bool BreakEn, bool BreakInLogic, bool BreakOutLogic);

/**
 * @brief  刹车引脚
 * 
 * @param  Pin PWM_EnumBrakIO的枚举值
 * @return PWM_SUCCESS
 */
int32_t PWM_BreakPIn(PWM_EnumBrakIO Pin);

#ifdef __cplusplus
}
#endif // defined __cplusplus

#endif

/** @} */
