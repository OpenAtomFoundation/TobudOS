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
 * @file n32g020xx_log.h
 * @author Nations Solution Team
 * @version v1.0.1
 * @brief 日志功能接口声明文件。
 *
 * 如果要开启日志功能，需在工程文件中定义宏`LOG_ENABLE`，定义`LOG_POSITION`会在
 * 日志信息前打印文件名和行号。日志信息缺省输出到UART0，可以在log.h文件中修改相
 * 应的配置来输出到其它UART通道。
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 *
 * @defgroup LOG 日志模块
 * @{
 *
 */
#ifndef __N32G020XX_LOG_H__
#define __N32G020XX_LOG_H__

#ifdef __cpluscplus
extern "C" {
#endif

#ifdef LOG_ENABLE

#include <stdio.h>

#include "n32g020xx_uart.h"

/**
 * @brief 日志输出的UART通道。
 */
//#define LOG_UART_CHANNEL UART_CH0
#define LOG_UART_CHANNEL UART_CH1

/**
 * @brief 日志输出的UART波特率。
 */
#define LOG_UART_BAUDRATE 115200

/**
 * @brief UART通道的管脚映射。
 */
//#define LOG_UART_IO_CONF UART_CH0_GPIO22_GPIO23
#define LOG_UART_IO_CONF UART_CH1_GPIO18_GPIO19

/**
 * @brief 初始化日志功能。
 */
void log_init(void);

#ifdef LOG_POSITION

/**
 * @brief 打印日志，语法与`printf`相同。
 */
#define log(...)                                  \
    {                                             \
        printf(__VA_ARGS__);                      \
    }

#define PRINT_CPU_FAULT_INFO(flag)                                                  \
    {                                                                               \
        uint32_t* stack_top = (uint32_t*)__get_MSP();                               \
        log("**************%s ERROR:REG INFO**************.\r\n", flag);            \
        log("msp = 0x%08x.\r\n", stack_top);                                        \
        log("r0  = 0x%08x,	\tr1   = 0x%08x.\r\n", stack_top[0], stack_top[1]); \
        log("r2  = 0x%08x,	\tr3   = 0x%08x.\r\n", stack_top[2], stack_top[3]); \
        log("r12 = 0x%08x,	\tlr   = 0x%08x.\r\n", stack_top[4], stack_top[5]); \
        log("pc  = 0x%08x,	\txpsr = 0x%08x.\r\n", stack_top[6], stack_top[7]); \
    }

#else // not defined LOG_POSITION
#define log(...) printf(__VA_ARGS__)
#define PRINT_CPU_FAULT_INFO(flag) \
    {                              \
    }
#endif // defined LOG_POSITION

#else // not defined LOG_ENABLE

#define log(...)
#define log_init()
#define PRINT_CPU_FAULT_INFO(flag) \
    {                              \
    }
#endif // defined LOG_ENABLE

#ifdef __cpluscplus
}
#endif

#endif // defined __N32G020XX_LOG_H__

/** @} */
