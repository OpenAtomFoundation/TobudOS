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
 * @brief RTC 模块接口声明文件。
 * @file n32g020xx_rtc.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * 
 * @defgroup RTC RTC模块
 * @{
 */

#ifndef __N32G020XX_RTC_H__
#define __N32G020XX_RTC_H__

#include "n32g020xx_common.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif // defined __cplusplus

/**
 * @brief 定义PWM模块函数接口的返回值。
 */
typedef enum RtcRetCode
{
    RTC_SUCCESS = 0,  ///< 操作成功完成
    RTC_FAIL    = -1, ///< 操作失败
} RtcRetCode;

/**
 * @fn RTC_RtcCallBack
 *
 * @brief 当RTC定时时间到的回调函数
 *
 * @return RTC_SUCCESS
 * @see RTC_StartRtc
 */
typedef int32_t (*RTC_RtcCallBack)(void);

/**
 * @brief RTC启动函数
 *
 * 内部默认打开1Hz中断，如果不需要回调函数可以设置为0
 * @param WakeUp 是否作为唤醒源
 * @param pRtcCallBack 注册的中断回调函数
 *
 * @return RTC_SUCCESS
 */
int32_t RTC_StartRtc(bool WakeUp, RTC_RtcCallBack pRtcCallBack);

/**
 * @brief 停止RTC
 *
 * @return RTC_SUCCESS
 */
int32_t RTC_StopTimer(void);

/**
 * @brief 写频率调整寄存器
 *
 * @param PpmVal PPM value，范围为±1024ppm
 *
 * @return RTC_SUCCESS 表示写入成功,RTC_FAIL表示返回失败
 */
int32_t RTC_SetJudjeFreq(int16_t PpmVal);

/**
 * @brief 读RTC计数器的值
 *
 * @param *Val 传出来的RTC计数器的值
 *
 * @return RTC_SUCCESS
 */
int32_t RTC_GetDrData(int32_t* Val);

#ifdef __cplusplus
}
#endif // defined __cplusplus

#endif

/** @} */
