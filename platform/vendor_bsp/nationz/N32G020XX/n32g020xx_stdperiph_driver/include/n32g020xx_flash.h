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
 * @file n32g020xx_flash.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @brief FLASH模块接口声明文件。
 * 
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * 
 * @defgroup FLASH FLASH模块
 * @{
 */

#ifndef __N32G020XX_FLASH_H__
#define __N32G020XX_FLASH_H__

#include <stdint.h>

#define FLASH_PAGE_SIZE 512


/**
 * @brief 定义FLASH函数接口的返回值。
 */
typedef enum FlashRetCode
{
    FLASH_SUCCESS = 0,      ///< 操作成功完成
    FLASH_FAIL,             ///< 操作失败
    FLASH_WRITE_FAIL,       ///< 写Flash失败
    FLASH_ERASE_FAIL,       ///< 擦除Flash失败
    FLASH_CHECK_INCORRECT,  ///< 校验错误
    FLASH_ADDR_INCORRECT,   ///< Flash地址没有页对齐
} FlashRetCode;
//-----------------------------------------------------------------------------
//                        Exported functions
//-----------------------------------------------------------------------------


/** 
 * @brief 擦除flash page
 * @param addr 待擦除的Flash地址
 * 
 * @return FLASH_SUCCESS 操作成功完成  FLASH_ERASE_FAIL 擦除Flash失败  FLASH_CHECK_INCORRECT 校验错误  FLASH_ADDR_INCORRECT Flash地址没有页对齐
 * @see FLASH_PAGE_SIZE
 */
int32_t FLASH_ErasePage(uint32_t addr);

/**
 * @brief 将数据按页方式写入到flash中，写之前先执行页擦除
 * @param addr flash地址
 * @param src 待写入到flash数据的地址
 * 
 * @return FLASH_SUCCESS 操作成功完成  FLASH_WRITE_FAIL 写Flash失败  FLASH_CHECK_INCORRECT 校验错误  FLASH_ADDR_INCORRECT Flash地址没有页对齐
 * @note 页写flash，不支持多页和跨页写
 * @see FLASH_PAGE_SIZE
 */
int32_t FLASH_WritePage(uint32_t addr, const uint8_t* src);

/**
 * @brief 将数据写入到flash中。
 * @param addr flash地址
 * @param src 待写入到flash数据的地址
 * @param len 数据长度，单位为byte
 * 
 * 这个函数会自动处理页擦除和页写入，在地址没有页对齐时，函数会自动缓存相应的数
 * 据并在擦除页后重新写入。
 * 
 * @return FLASH_SUCCESS 操作成功完成  FLASH_ERASE_FAIL 擦除Flash失败  FLASH_WRITE_FAIL 写Flash失败
 * @see FLASH_PAGE_SIZE
 */
int32_t FLASH_WriteData(uint32_t addr, const uint8_t* src, uint32_t len);

#endif // __N32G020XX_FLASH_H__

/** @} */
