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
 * @file n32g020xx_crc.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @brief CRC16/LRC校验计算模块接口声明文件。
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * 
 * @defgroup CHECKSUM   CRC/LRC校验计算模块
 * @{
 */

#ifndef __N32G020XX_CRC_H__
#define __N32G020XX_CRC_H__

#include <stdint.h>

/**
 * @brief 定义CRC函数接口的返回值。
 */
typedef enum CRCRetCode
{
    CRC_SUCCESS = 0,        ///< 操作成功完成
    CRC_FAIL,               ///< 操作失败
} CRCRetCode;
//-----------------------------------------------------------------------------
//                        Exported functions
//-----------------------------------------------------------------------------


/**
 * @brief CRC16_Calc  计算CRC16的值
 * @param init_val crc初值,如果大于0x10000表示不配置初值,使用上次数据
 * @param data 待计算的数据
 * @param len 待计算的数据的长度
 * @param out_val 返回CRC结果
 * 
 * @return CRC_SUCCESS 操作成功完成
 * @note 只支持一种计算CRC16的值,其采用的生成项为X16+X15+X2+X0,输入为8位数据，输出为16位数据
 */
int32_t CRC16_Calc(uint32_t init_val, const uint8_t* data, uint32_t len, uint16_t* out_val);

/**
 * @brief LRC_Calc  计算LRC的值
 * @param init_val lrc初值,如果大于0x100表示不配置初值,使用上次数据
 * @param data 待计算的数据
 * @param len 待计算的数据的长度
 * @param out_val 返回LRC结果
 * 
 * @return CRC_SUCCESS 操作成功完成
 */
int32_t LRC_Calc(uint16_t init_val, const uint8_t* data, uint32_t len, uint8_t* out_val);

#endif // __N32G020XX_CRC_H__

/** @} */
