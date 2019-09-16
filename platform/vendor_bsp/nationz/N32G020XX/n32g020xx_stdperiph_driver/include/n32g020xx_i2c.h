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
 * @file n32g020xx_i2c.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @brief    I2C模块接口声明文件。
 * @defgroup I2C I2C模块
 * @{
 */

#ifndef __N32G020XX_I2C_H__
#define __N32G020XX_I2C_H__

#include <stdint.h>

typedef enum I2cRetCode
{
    I2C_SUCCESS         = 0,  ///< 成功
    I2C_FAILED          = -1, ///< 失败
    I2C_TIMEOUT         = -2, ///< 超时
    I2C_PARAM_INCORRECT = -3, ///< 参数错误
} I2cRetCode;

#define KHz (1000)

#define I2C_SPEED_STANDARD (100 * KHz)    ///< I2C速率 100KHz
#define I2C_SPEED_FAST (400 * KHz)        ///< I2C速率 400KHz
#define I2C_SPEED_FAST_PLUS (1000 * KHz)  ///< I2C速率 1MHz
#define I2C_SPEED_MAX I2C_SPEED_FAST_PLUS ///< 设备支持最大速率 1MHz

typedef enum
{
    I2C_TX, ///< I2C发送模式
    I2C_RX, ///< I2C接收模式
} I2cTransferMode;

typedef enum I2cTransferOption
{
    I2C_OPT_NONE = 0, ///< 无选项
    I2C_OPT_STOP,     ///< 停止选项
    I2C_OPT_CONTINUE, ///< 继续选项
} I2cTransferOption;

typedef enum
{
    I2C_CH0,     ///< I2C接口通道0
    I2C_CH1,     ///< I2C接口通道1
    I2C_NUM = 2, ///< I2C接口通道数
} I2cChannel;

/** @brief 从地址配置回调函数 */
typedef void (*I2CSlaveASHandler)(void);
/** @brief 从发送回调函数 */
typedef uint8_t (*I2CSlaveTxHandler)(void);
/** @brief 从接收回调函数 */
typedef void (*I2CSlaveRxHandler)(uint8_t);
/** @brief 发送完成回调函数 */
typedef void (*CompleteCallback)(void);

/**
 * @brief 初始化I2C
 *
 * @param[in] channel I2C接口通道
 * @param[in] addr I2C从模式地址
 * @param[in] speed I2C时钟速率
 * @return int32_t
 * - @ref I2C_SUCCESS : 表示成功
 * - @ref I2C_PARAM_INCORRECT : 参数错误
 */
int32_t I2C_Init(I2cChannel channel, uint8_t addr, uint32_t speed);

/**
 * @brief 反初始化I2C
 *
 * @param[in] channel I2C接口通道
 * @return int32_t
 * - @ref I2C_SUCCESS : 表示成功
 * - @ref I2C_PARAM_INCORRECT : 参数错误
 */
int32_t I2C_DeInit(I2cChannel channel);

/**
 * @brief I2C软件复位
 *
 * @param[in] channel I2C接口通道
 * @return int32_t
 * - @ref I2C_SUCCESS : 表示成功
 * - @ref I2C_PARAM_INCORRECT : 参数错误
 */
int32_t I2C_Reset(I2cChannel channel);

/**
 * @brief 发送I2C停止条件
 *
 * @param[in] channel I2C接口通道
 * @return int32_t
 * - @ref I2C_SUCCESS : 表示成功
 * - @ref I2C_PARAM_INCORRECT : 参数错误
 */
int32_t I2C_Stop(I2cChannel channel);

/**
 * @brief 禁止I2C中断
 *
 * @param[in] channel I2C接口通道
 * @return int32_t
 * - @ref I2C_SUCCESS : 表示成功
 * - @ref I2C_PARAM_INCORRECT : 参数错误
 */
int32_t I2C_InterruptDisable(I2cChannel channel);

/**
 * @brief 使能I2C中断
 *
 * @param[in] channel I2C接口通道
 * @param[in] asHandler I2C从地址匹配回调
 * @param[in] txHandler I2C从发送回调
 * @param[in] rxHandler I2C从接收回调
 * @return int32_t
 * - @ref I2C_SUCCESS : 表示成功
 * - @ref I2C_PARAM_INCORRECT : 参数错误
 */
int32_t I2C_InterruptEnable(I2cChannel channel,
                            I2CSlaveASHandler asHandler,
                            I2CSlaveTxHandler txHandler,
                            I2CSlaveRxHandler rxHandler);

/**
 * @brief I2C主模式初始化
 *
 * @param[in] channel I2C接口通道
 * @param[in] addr I2C从设备地址
 * @param[in] mode I2C传输模式
 * - @ref I2C_TX : 发送
 * - @ref I2C_RX : 接收
 * @return int32_t
 * - @ref I2C_SUCCESS : 表示成功
 * - @ref I2C_FAILED : 表示失败
 * - @ref I2C_TIMEOUT : 表示超时
 * - @ref I2C_PARAM_INCORRECT : 参数错误
 */
int32_t I2C_MasterInit(I2cChannel channel, uint8_t addr, I2cTransferMode mode);

/**
 * @brief I2C主模式数据发送
 *
 * @param[in] channel I2C接口通道
 * @param[in] data 待发送数据
 * @param[in] length 待发送数据长度（Byte）
 * @param[in] options
 * - @ref I2C_OPT_NONE : 无
 * - @ref I2C_OPT_STOP : 发送完即停止
 * - @ref I2C_OPT_CONTINUE : 发送完不停止
 * @return int32_t
 * - @ref I2C_SUCCESS : 表示成功
 * - @ref I2C_FAILED : 表示失败
 * - @ref I2C_PARAM_INCORRECT : 参数错误
 */
int32_t I2C_MasterSendData(I2cChannel channel, const uint8_t* data, uint32_t length, uint32_t options);

/**
 * @brief I2C主模式数据接收
 *
 * @param[in] channel I2C接口通道
 * @param[in,out] data 数据接收缓冲区
 * @param[in] length 数据接收缓冲区长度（Byte）
 * @param[in] options
 * - @ref I2C_OPT_NONE : 无
 * - @ref I2C_OPT_STOP : 接收完即停止
 * - @ref I2C_OPT_CONTINUE : 接收完不停止
 * @return int32_t
 * - @ref I2C_SUCCESS : 表示成功
 * - @ref I2C_PARAM_INCORRECT : 参数错误
 */
int32_t I2C_MasterRecvData(I2cChannel channel, uint8_t* data, uint32_t length, uint32_t options);

/**
 * @brief 等待I2C从模式总线呼叫
 *
 * @param[in] channel I2C接口通道
 * @return int32_t
 * - @ref I2C_SUCCESS : 表示成功
 * - @ref I2C_PARAM_INCORRECT : 参数错误
 */
int32_t I2C_SlaveBusWait(I2cChannel channel);

/**
 * @brief I2C从模式初始化
 *
 * @param[in] channel I2C接口通道
 * @return int32_t
 * - @ref I2C_SUCCESS : 表示成功
 * - @ref I2C_PARAM_INCORRECT : 参数错误
 */
int32_t I2C_SlaveInit(I2cChannel channel);

/**
 * @brief 获取I2C从模式传输模式
 *
 * @param[in] channel I2C接口通道
 * @param[out] mode
 * - @ref I2C_TX : 发送模式
 * - @ref I2C_RX : 接收模式
 * @return int32_t
 * - @ref I2C_SUCCESS : 表示成功
 * - @ref I2C_PARAM_INCORRECT : 参数错误
 */
int32_t I2C_SlaveGetTranserMode(I2cChannel channel, I2cTransferMode* mode);

/**
 * @brief I2C从模式数据发送
 *
 * @param[in] channel I2C接口通道
 * @param[in] data 待发送数据
 * @param[in] length 待发送数据长度(Byte)
 * @return int32_t
 * - @ref I2C_SUCCESS : 表示成功
 * - @ref I2C_FAILED : 表示失败
 * - @ref I2C_PARAM_INCORRECT : 参数错误
 */
int32_t I2C_SlaveSendData(I2cChannel channel, const uint8_t* data, uint32_t length);

/**
 * @brief I2C从模式数据接收
 *
 * @param[in] channel I2C接口通道
 * @param[out] data 接收缓冲区
 * @param[in] length 接收缓冲区长度(Byte)
 * @return int32_t
 * - @ref I2C_SUCCESS : 表示成功
 * - @ref I2C_FAILED : 表示失败
 * - @ref I2C_PARAM_INCORRECT : 参数错误
 */
int32_t I2C_SlaveRecvData(I2cChannel channel, uint8_t* data, uint32_t length);

/**
 * @brief I2C以DMA方式发送数据
 *
 * @param[in] channel I2C接口通道
 * @param[in] data 待发送数据
 * @param[in] length 待发送数据长度(Byte)
 * @param[in] callback 发送完成回调
 * @return int32_t
 * - @ref I2C_SUCCESS : 表示成功
 * - @ref I2C_FAILED : 表示失败
 * - @ref I2C_PARAM_INCORRECT : 参数错误
 */
int32_t I2C_DMASendData(I2cChannel channel, const uint8_t* data, uint32_t length, CompleteCallback callback);

/**
 * @brief I2C以DMA方式接收数据
 *
 * @param[in] channel I2C接口通道
 * @param[in] data 接收缓冲区
 * @param[in] length 接收缓冲区长度(Byte)
 * @param[in] callback 接收完成回调
 * @return int32_t
 * - @ref I2C_SUCCESS : 表示成功
 * - @ref I2C_FAILED : 表示失败
 * - @ref I2C_PARAM_INCORRECT : 参数错误
 */
int32_t I2C_DMARecvData(I2cChannel channel, uint8_t* data, uint32_t length, CompleteCallback callback);

#endif // __N32G020XX_I2C_H__

/** @} */
