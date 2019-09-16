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
 * @brief SPI模块接口声明文件。
 * @file n32g020xx_spi.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 *
 * @defgroup SPI SPI模块
 * @{
 */

#ifndef __N32G020XX_SPI_H__
#define __N32G020XX_SPI_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 定义SPI通道号。
 */
typedef enum SpiChannel
{
    SPI_CH0 = 0, ///< 通道0
    SPI_CH1,     ///< 通道1
} SpiChannel;

typedef enum SpiMode
{
    SPI_MODE_MASTER = 0,
    SPI_MODE_SLAVE,

} SpiMode;

/**
 * @brief 定义SPI传输模式。
 *
 * 目前只支持标准模式，其它值无效。
 */
typedef enum SpiTransType
{
    SPI_TT_STANDARD = 0,   ///< 标准SPI
    SPI_TT_DUAL_ONE_LINE,  ///< DUAL传输模式一线
    SPI_TT_DUAL_TWO_LINE,  ///< DUAL传输模式二线
    SPI_TT_QUAD_ONE_LINE,  ///< QUAD传输模式一线
    SPI_TT_QUAD_FOUR_LINE, ///< QUAD传输模式四线
} SpiTransType;

/**
 * @brief 定义帧模式（极性和相位）。
 */
typedef enum SpiFrameFormat
{
    SPI_FF_CPOL0_CPHA0 = 0, ///< 极性0，相位0
    SPI_FF_CPOL0_CPHA1,     ///< 极性0，相位1
    SPI_FF_CPOL1_CPHA0,     ///< 极性1，相位0
    SPI_FF_CPOL1_CPHA1,     ///< 极性1，相位1
} SpiFrameFormat;

/**
 * @brief SPI初始化配置结构体。
 */
typedef struct SpiConfig
{
    SpiMode mode;
    /**
     * @brief SPI的SCK频率。
     *
     * SCK频率通过从20MHz按整数（1 - 256）分频得到，因此这个参数的取值为：
     *
     * clock_freq = 20000000 / N
     *
     * 使用其它的值最后会重新适配到合适的值，请注意这可能会和用户的意图不符。
     *
     * 超出上下范围的值（如配置成超过20M）会引起初始化失败。
     */
    uint32_t clock_freq;
    SpiTransType trans_type;     ///< 传输模式
    SpiFrameFormat frame_format; ///< 帧格式
    uint8_t frame_width;         ///< 每帧的位宽，可取的值为 4 - 8
} SpiConfig;

/**
 * @brief 定义SPI函数接口的返回值。
 */
typedef enum SpiRetCode
{
    SPI_SUCCESS = 0,     ///< 操作成功完成
    SPI_PARAM_INCORRECT, ///< 参数错误
    SPI_UNINITED,        ///< SPI通道未初始化
} SpiRetCode;

/**
 * @brief 初始化SPI模块。
 *
 * 对于主机模式， `SpiConfig` 的所有成员变量均有意义；对于从机模式， `SpiConfig`
 * 的成员中只有 `mode` 和 `frame_format` 有意义，其它成员的值会被忽略。
 *
 * @param channel   SPI通道号
 * @param config    初始化配置参数
 *
 * @return
 * - `SPI_SUCCESS` 表示操作成功，
 * - `SPI_PARAM_INCORRECT` 表示配置参数错误。
 *
 * @see SpiConfig
 */
int32_t SPI_Init(SpiChannel channel, const SpiConfig* config);

/**
 * @brief 发数据并收返回的数据。
 *
 * SPI在发送数据的同时也在收数据，且发送的数据和接收的数据长度相同。
 *
 * `send_buf`和`recv_buf`不能指向同一地址，也不能有部分位置重叠，否则会引起数据
 * 错误。
 *
 * 处于主机模式时，本函数会给出数据长度相应的时钟，在发送数据的同步将收到的数据
 * 放入接收缓存中。
 *
 * 处于从机模式时，本函数会等待主机的片选和时钟信号，并在收主机数据的同时发送数
 * 据，当所有数据都发送完时函数才返回。
 *
 * @warning
 * 请特别注意，本芯片的SPI从机模式在片选信号后发送的第一个字节不是有效字节，而是
 * 一个无效字节，即`send_buf`中的第一个字节实际将在片选后的第二个字节发送，假设
 * 从机要发送10个字节数据，主机需要给出11个字节的时钟信号，并且抛弃收到的第一个
 * 字节，而从机收到的主机数据会比参数`length`指定的长度大1。应用开发者在使用SPI
 * 从机模式时必须考虑这一点。
 *
 * @param channel   SPI通道号
 * @param send_buf  发送数据缓存
 * @param recv_buf  接收数据缓存
 * @param length    发送/接收数据的长度
 *
 * @return
 * - `SPI_SUCCESS` 表示操作成功，
 * - `SPI_UNINITED` 表示SPI通道未初始化，
 * - `SPI_PARAM_INCORRECT` 表示参数错误。
 */
int32_t SPI_Transmit(SpiChannel channel,
                     const uint8_t* send_buf,
                     uint8_t* recv_buf,
                     uint32_t length);

/**
 * @brief 只发送数据。
 *
 * 收到的任何数据将被忽略。
 *
 * @warning
 * 请特别注意，本芯片的SPI从机模式在片选信号后发送的第一个字节不是有效字节，而是
 * 一个无效字节，即`send_buf`中的第一个字节实际将在片选后的第二个字节发送，假设
 * 从机要发送10个字节数据，主机需要给出11个字节的时钟信号，并且抛弃收到的第一个
 * 字节。应用开发者在使用SPI从机模式时必须考虑这一点。
 *
 * @param channel   SPI通道号
 * @param send_buf  发送数据缓存
 * @param length    发送数据的长度
 *
 * @return
 * - `SPI_SUCCESS` 表示操作成功，
 * - `SPI_UNINITED` 表示SPI通道未初始化，
 * - `SPI_PARAM_INCORRECT` 表示参数错误。
 */
int32_t
SPI_SendData(SpiChannel channel, const uint8_t* send_buf, uint32_t length);

/**
 * @brief 只接收数据。
 *
 * 处于主机模式时，将发送`length`字节的无效数据，并返回收到的数据。
 *
 * 处于从机模式时，从主机接收`length`字节的数据，不发送数据。
 *
 * @param channel   SPI通道号
 * @param recv_buf  接收数据缓存
 * @param length    接收数据的长度
 *
 * @return
 * - `SPI_SUCCESS` 表示操作成功，
 * - `SPI_UNINITED` 表示SPI通道未初始化，
 * - `SPI_PARAM_INCORRECT` 表示参数错误。
 */
int32_t SPI_ReceiveData(SpiChannel channel, uint8_t* recv_buf, uint32_t length);

/**
 * @brief 关闭SPI模块。
 *
 * @param channel   SPI通道号
 *
 * @return
 * - `SPI_SUCCESS` 表示操作成功，
 * - `SPI_UNINITED` 表示SPI通道未初始化，
 * - `SPI_PARAM_INCORRECT` 表示参数错误。
 */
int32_t SPI_Close(SpiChannel channel);

#ifdef __cplusplus
}
#endif

#endif // !defined __N32G020XX_SPI_H__

/** @} */
