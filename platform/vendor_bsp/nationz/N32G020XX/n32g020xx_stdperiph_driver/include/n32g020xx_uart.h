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
 * @brief UART模块接口声明文件。
 * @file n32g020xx_uart.h
 * @author Nations Solution Team
 * @version v1.0.1
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * 
 * @defgroup UART UART模块
 * @{
 */

#ifndef __N32G020XX_UART_H__
#define __N32G020XX_UART_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // defined __cplusplus

/**
 * @brief 定义UART的校验类型。
 */
typedef enum UartParityType
{
    UART_PARITY_EVEN, ///< 偶校验
    UART_PARITY_ODD,  ///< 奇校验
    UART_PARITY_NONE  ///< 无校验
} UartParityType;

/**
 * @brief 定义UART函数接口的返回值。
 */
typedef enum UartRetCode
{
    UART_SUCCESS = 0,        ///< 操作成功完成
    UART_PARAM_INCORRECT,    ///< 参数错误
    UART_RECV_PARITY_ERROR,  ///< 校验错误
    UART_RECV_FIFO_OVERFLOW, ///< 接收FIFO溢出
    UART_TIMEOUT,            ///< 超时
} UartRetCode;

/**
 * @brief 定义UART通道号。
 *
 * 当前支持3组UART。
 */
typedef enum UartChannel
{
    UART_CH0 = 0, ///< UART通道0
    UART_CH1,     ///< UART通道1
    UART_CH2      ///< UART通道2
} UartChannel;

/**
 * @brief 定义UART各个通道使用的GPIO管脚。
 */
typedef enum UartIO
{
    UART_CH0_GPIO0_GPIO1 = 0, ///< UART0使用GPIO0作为输出，GPIO1作为输入
    UART_CH0_GPIO22_GPIO23,   ///< UART0使用GPIO22作为输出，GPIO23作为输入
    UART_CH1_GPIO2_GPIO3,     ///< UART1使用GPIO2作为输出，GPIO3作为输入
    UART_CH1_GPIO18_GPIO19,   ///< UART1使用GPIO18作为输出，GPIO19作为输入
    UART_CH2_GPIO4_GPIO5,     ///< UART2使用GPIO4作为输出，GPIO5作为输入
    UART_CH2_GPIO20_GPIO21,   ///< UART2使用GPIO20作为输出，GPIO21作为输入
} UartIO;

/** @deprecated */
#define UART_FREQ_INTERAL 0

/** @brief 使用内部时钟 */
#define UART_FREQ_INTERNAL 0

/**
 * @brief 定义接收数据时的错误码。
 */
typedef enum UartErrorCode
{
    UART_STATE_PARITY_ERROR  = -1, ///< 校验错误
    UART_STATE_FIFO_OVERFLOW = -2  ///< 接收FIFO溢出错误
} UartErrorCode;

/**
 * @fn UART_Callback
 *
 * @brief 当收到数据或发生错误时的回调函数。
 *
 * @param val 当传入值为0-255时，代表收到的1字节数据，其它值代表一个错误码
 *
 * @see UartErrorCode
 */
typedef void (*UART_Callback)(int32_t val);

/**
 * @brief UART初始化配置结构体。
 */
typedef struct UartConfig
{
    uint32_t frequence;         ///< UART的来源时钟频率
    uint32_t baudrate;          ///< 波特率，如9600、115200
    UartIO io;                  ///< UART使用哪些GPIO
    UartParityType parity_type; ///< 校验类型
    UART_Callback callback;     ///< 回调函数，用于收数据和处理错误
} UartConfig;

/**
 * @brief 初始化UART模块。
 *
 * UART模块支持使用内部和外部时钟，当使用内部时钟时，请指定 `config->frequence =
 * UART_FREQ_INTERNAL` ，当使用外部时钟时，请指定具体外部时钟的频率值。函数会根据
 * 时钟与指定波特率自动计算相关参数并配置相应的寄存器。
 *
 * 如果不想收数据， `config->callback` 可以配置为 `NULL` 。
 *
 * **如果使用外部时钟，在调用本函数前必须先配置好外部时钟并使能。**
 *
 * @param channel   UART通道号
 * @param config    初始化配置参数
 *
 * @return `UART_SUCCESS` 表示操作成功，
 * `UART_PARAM_INCORRECT`表示配置参数错误。
 *
 * @see UartConfig
 */
int32_t UART_Init(UartChannel channel, const UartConfig* config);

/**
 * @brief 发送数据。
 *
 * @param channel   UART通道号
 * @param send_buf  发送数据缓存
 * @param send_len  发送数据的长度
 *
 * @return
 * - `UART_SUCCESS` 表示操作成功
 * - `UART_TIMEOUT` 表示发送超时
 * - 其它值表示出错
 */
int32_t
UART_SendData(UartChannel channel, const uint8_t* send_buf, uint32_t send_len);

/**
 * @brief 接收数据。
 *
 * 这个函数以轮询方式接收数据，接收到`recv_len`字节数据或遇到错误才返回。
 *
 * @param channel   UART通道号
 * @param recv_buf  接收数据缓存
 * @param recv_len  需要接收的数据长度
 *
 * @return
 * - `UART_SUCCESS` 表示操作成功
 * - `UART_RECV_PARITY_ERROR` 表示接收时有奇偶校验错误
 * - `UART_RECV_FIFO_OVERFLOW` 表示接收FIFO溢出
 * - 其它值表示出错
 */
int32_t
UART_ReceiveData(UartChannel channel, uint8_t* recv_buf, uint32_t recv_len);

/**
 * @brief 关闭UART通道。
 *
 * @param channel   UART通道号
 *
 * @return `UART_SUCCESS` 表示操作成功，其它值表示出错。
 */
int32_t UART_Close(UartChannel channel);

#ifdef __cplusplus
}
#endif // defined __cplusplus

#endif // __N32G020XX_UART_H__

/** @} */
