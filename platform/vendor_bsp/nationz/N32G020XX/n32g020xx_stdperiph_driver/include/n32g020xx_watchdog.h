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
 * @brief WDT模块接口声明文件。
 * @file n32g020xx_watchdog.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * 
 * @defgroup WDT WDT(Watchdog Timer)模块
 * @{
 */

#ifndef __N32G020XX_WATCHDOG_H__
#define __N32G020XX_WATCHDOG_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 定义Watchdog的操作模式。
 */
typedef enum _WdtMode
{
    WDT_MODE_INT = 0, ///< 计时完成后触发中断
    WDT_MODE_RST,     ///< 计时完成后复位芯片
    WDT_MODE_INT_RST, ///< 计时完成后先触发中断，再复位芯片
} WdtMode;

/**
 * @brief 定义Watchdog函数接口的返回值。
 */
typedef enum _WdtRetCode
{
    WDT_SUCCESS = 0,    ///< 操作成功完成
    WDT_PARAM_INCORRECT ///< 参数错误
} WdtRetCode;

#define WDT_FREQ_INTERNAL 0 ///< 使用内部时钟

typedef void (*WDT_Callback)(void); ///< Watchdog的中断回调函数指针

/**
 * @brief Watchdog初始化配置结构体。
 */
typedef struct _WdtConfig
{
    uint32_t
        frequence; ///< Watchdog时钟频率，内部时钟时指定 `WDT_FREQ_INTERNAL`
    uint32_t time_ms; ///< 超时的毫秒数
    WdtMode mode;     ///< 超时的操作模式
    WDT_Callback callback; ///< 中断回调函数，当操作模式中有中断时有效
} WdtConfig;

/**
 * @brief 初始化Watchdog模块。
 *
 * Watchdog支持使用内部时钟和外部时钟，当使用内部时钟时请指定
 * `WDT_FREQ_INTERNAL` ，使用外部时钟时应指定具体的外部时钟频率值。函数内部会根
 * 据时钟频率和指定的超时时间计算相关的参数并配置相应的寄存器。
 * 
 * 当超时的毫秒数超过模块支持的时间范围时，函数返回`WDT_PARAM_INCORRECT`。
 *
 * **如果使用外部时钟，在调用本函数前必须先配置好外部时钟并使能。**
 * 
 * @note 注意，函数中有连续不能被中断的操作，因此调用此函数前应关闭系统中的其它
 * 中断，调用完成后再开启中断，否则操作可能失败。
 *
 * @param config 指向配置结构体的指针，用于初始化配置，见 `WdtConfig` 。
 * @return `WDT_SUCCESS` 表示操作成功， `WDT_PARAM_INCORRECT` 表示参数错误。
 * @see `WdtMode`
 * @see `WdtRetCode`
 * @see `WdtConfig`
 */
int32_t WDT_Init(const WdtConfig* config);

/**
 * @brief 关闭Watchdog模块。
 *
 * @note 注意，函数中有连续不能被中断的操作，因此调用此函数前应关闭系统中的其它
 * 中断，调用完成后再开启中断，否则操作可能失败。
 *
 * @return `WDT_SUCCESS` 表示成功。
 */
int32_t WDT_Close(void);

/**
 * @brief 复位Watchdog计数器。
 *
 * @note 注意，函数中有连续不能被中断的操作，因此调用此函数前应关闭系统中的其它
 * 中断，调用完成后再开启中断，否则操作可能失败。
 *
 * @return `WDT_SUCCESS` 表示成功。
 */
int32_t WDT_FeedDog(void);

#ifdef __cplusplus
}
#endif

#endif // __N32G020XX_WATCHDOG_H__

/** @} */
