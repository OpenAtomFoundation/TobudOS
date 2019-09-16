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
 * @brief TIMER 模块接口声明文件。
 * @file n32g020xx_timer.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 *
 * @defgroup TIMER  TIMER计时器模块
 * @{
 */

#ifndef __N32G020XX_TIMER_H__
#define __N32G020XX_TIMER_H__

#include "n32g020xx_common.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif // defined __cplusplus

/**
 * @brief 定义PWM模块函数接口的返回值。
 */
typedef enum TimerRetCode
{
    TIMER_SUCCESS = 0,     ///< 操作成功完成
} TimerRetCode;

typedef enum
{
    Timer0 = 0,
    Timer1,
    Timer2,
    Timer3,
    Timer4,
} TimerId;

/**
 * @fn TIMER_TimerCallBack
 *
 * @brief 当定时时间到时调用的回调函数。
 *
 * @param id 表示定时器名称，TIMER_EnumTimerId枚举值之一
 *
 * @return TIMER_SUCCESS
 * @see TIMER_StartTimerUs
 */
typedef int32_t (*TIMER_TimerCallBack)(TimerId id);

/**
 * @brief 启动定时器的定时功能
 * pTimerCallBack 填入回调函数时，为非阻塞调用,内部直接启动定时器
 * @param id 表示定时器名称，TIMER_EnumTimerId枚举值之一
 * @param us 定时us数
 * @param AutoInterOrExcFixValue
 * 为0的时候，自动选择内部高速时钟，如果外部时钟，填入非0值
 * @param pTimerCallBack  回调函数，pTimerCallBack = 0，则是阻塞调用
 * @return TIMER_SUCCESS
 */
int32_t TIMER_StartTimerUs(TimerId id,
                           int32_t us,
                           uint32_t AutoInterOrExcFixValue,
                           TIMER_TimerCallBack pTimerCallBack);

/**
 * @brief 停止定时器
 * @param id 表示定时器名称，TIMER_EnumTimerId枚举值之一
 * @return TIMER_SUCCESS
 */
int32_t TIMER_StopTimer(TimerId id);

/**
 * @fn TIMER_CaptureCallBack
 *
 * @brief 捕获完成时调用的回调函数。
 *
 * @param id 表示定时器名称，TIMER_EnumTimerId枚举值之一
 * @param CaptureValue 0:捕获错误 > 0 :捕获到的计数值
 * @return TIMER_SUCCESS
 * @see TIMER_StartCapture
 */
typedef int32_t (*TIMER_CaptureCallBack)(TimerId id,
                                         uint32_t CaptureValue);
typedef enum
{
    TimerPosedgeToPosedge = 0L,   //当前 上 升沿捕获下一次 上 升沿
    TimerPosedgeToNegedge = BIT7, //当前 上 升沿捕获下一次 下 升沿
    TimerNegedgeToNegedge = BIT8, //当前 下 升沿捕获下一次 下 升沿
    TimerNegedgeToPosedge = BIT8 | BIT7, //当前 下 升沿捕获下一次 上 升沿
    TimerPosedgeToPosNegedge = BIT9 | BIT7, //当前 上 升沿捕获下一次 上下 升沿
    TimerNegedgeToPosNegedge = BIT8 | BIT7, //当前 下 升沿捕获下一次 上下 升沿
} TIMER_EnumeCapModue;

/**
 * @brief 启动定时器的捕获功能
 *
 * @param id 表示定时器名称，TIMER_EnumTimerId枚举值之一
 * @param mode 捕获模式
 * @param pCaptureCallBack  回调函数，
 * @return TIMER_SUCCESS
 */
int32_t TIMER_StartCapture(TimerId id,
                           TIMER_EnumeCapModue mode,
                           TIMER_CaptureCallBack pCaptureCallBack);

/**
 * @brief 停止捕获功能
 * @param id 表示定时器名称
 * @return Null
 */
int32_t TIMER_StopCapture(TimerId id);

/**
 * @brief 读定时器的值
 * @param id 表示定时器名称
 * @param *Val 传出的定时器值
 * @return TIMER_SUCCESS
 */
int32_t TIMER_GetCountValue(TimerId id, uint32_t* Val);

/**
 * @brief 利用Time4唤醒的初始化调用
 * 内部采用TIM4实现
 * @return TIMER_SUCCESS
 */
int32_t TIMER_WakeUpUsIntial(void);
/**
 * @brief 唤醒使能
 * 内部采用TIM4实现
 * @param us 表示多长时间进行唤醒
 * @param StandBy true 为采用StandBy模式，false 为Idle模式
 * @param IrqStyle true 为采用中断方式，false 为事件方式
 * @return TIMER_SUCCESS
 */
int32_t TIMER_StartWakeUpUs(int32_t us,bool StandBy,bool IrqStyle);

#ifdef __cplusplus
}
#endif // defined __cplusplus

#endif // _TIMER_H_

/** @} */

