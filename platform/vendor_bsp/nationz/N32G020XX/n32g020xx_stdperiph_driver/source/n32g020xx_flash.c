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
 * @file n32g020xx_flash.c
 * @author Nations Solution Team
 * @version v1.0.0
 * @brief FLASH模块接口实现文件。
 * 
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "n32g020xx_flash.h"

#include "n32g020xx_common.h"
#include "n32g020xx.h"

#include <stdbool.h>

#define REG8(addr) (*(volatile uint8_t*)(addr))
#define REG32(addr) (*(volatile uint32_t*)(addr))
//-----------------------------------------------------------------------------
//                        basefunc
//-----------------------------------------------------------------------------

// FLASHOPR  FLASHCADR
#define FLASH_OPR_PP (2)   // Page编程
#define FLASH_OPR_BW (5)   // Buffer写入
#define FLASH_OPR_PE (6)   // Page擦除
#define FLASH_OPR_PVRP (7) // Page编程读校验
#define FLASH_OPR_EV (9)   // Page擦除读校验
#define __FLASH_SetOpr(addr, opr)               \
    {                                           \
        SET_REG_VAL(FLASH->OPR, 0x600 | (opr)); \
        SET_REG_VAL(FLASH->OPR, 0x800 | (opr)); \
        SET_REG_VAL(FLASH->OPR, 0xB00 | (opr)); \
        SET_REG_VAL(FLASH->ADR, (addr) >> 9);   \
        REG8(addr) = 0;                         \
    }

#define FLASH_ECC2_ERR BIT7

//对FLASH进行擦写操作时的缓冲BUF
static uint8_t g_FlashBuf[FLASH_PAGE_SIZE];

static int32_t FlashOperation(uint32_t addr, uint32_t opr)
{
    // 清Flash中断寄存器
    SET_REG_VAL(FLASH->SR, 0xFFFFFFFF);
    // 清Flash报警状态寄存器
    SET_REG_VAL(FLASH->SECR, 0xFFFFFFFF);

    __FLASH_SetOpr(addr, opr);

    // 获取Flash中断寄存器和Flash报警状态寄存器的值
    while (!GET_REG_VAL(FLASH->SR) && !GET_REG_VAL(FLASH->SECR))
        ;

    // 判断Flash操作是否正常完成和Flash报警状态寄存器的值
    if (GET_REG_BIT(FLASH->SR, ~BIT0) || GET_REG_VAL(FLASH->SECR))
    {
        return FLASH_FAIL;
    }

    return FLASH_SUCCESS;
}

static bool CheckErasePage(uint32_t addr)
{
    for (int32_t i = 0; i < FLASH_PAGE_SIZE; i += 4)
    {
        // 擦完数据不为全1或有ECC状态错误
        if ((REG32(addr + i) != 0xfffffffful) || GET_REG_BIT(FLASH->SR, FLASH_ECC2_ERR))
        {
            return false;
        }
    }
    return true;
}

int32_t FLASH_ErasePage(uint32_t addr)
{
    if ((addr % FLASH_PAGE_SIZE) != 0) // Flash地址按页对齐
    {
        return FLASH_ADDR_INCORRECT;
    }

    if (GET_REG_VAL(FLASH->CR) & 0x08)
    {
        // 去掉自动擦除校验
        CLR_REG_BIT(FLASH->CR, 0x08);
    }

    // Page擦除
    if (FlashOperation(addr, FLASH_OPR_PE))
    {
        return FLASH_ERASE_FAIL;
    }

    // 校验擦除结果
    if (!CheckErasePage(addr))
    {
        return FLASH_CHECK_INCORRECT;
    }

    return FLASH_SUCCESS;
}

int32_t FLASH_WritePage(uint32_t addr, const uint8_t* src)
{
    uint32_t i;
    if ((addr % FLASH_PAGE_SIZE) != 0) // Flash地址按页对齐
    {
        return FLASH_ADDR_INCORRECT;
    }
    // 清Flash中断寄存器
    SET_REG_VAL(FLASH->SR, 0xFFFFFFFF);
    // 清Flash报警状态寄存器
    SET_REG_VAL(FLASH->SECR, 0xFFFFFFFF);
    // Buffer写入
    __FLASH_SetOpr(addr, FLASH_OPR_BW);
    for (i = 0; i < FLASH_PAGE_SIZE; i++)
    {
        ((uint8_t*)addr)[i] = ((uint8_t*)src)[i];
    }
    // Page编程
    if (FlashOperation(addr, FLASH_OPR_PP))
    {
        return FLASH_WRITE_FAIL;
    }
    // Page编程读校验
    if (FlashOperation(addr, FLASH_OPR_PVRP))
    {
        return FLASH_CHECK_INCORRECT;
    }
    return FLASH_SUCCESS;
}

int32_t FLASH_WriteData(uint32_t addr, const uint8_t* src, uint32_t len)
{
    uint32_t i;
    uint32_t pageAddr;  // 按页对齐的地址
    uint32_t pageCount; // 写Falsh的页数
    uint32_t tmpAddr;   // 按页对齐的余地址
    uint32_t tmpLen;    // 首页需要写入Flash的长度
    uint32_t startAddr; // 起始地址是否页对齐
    uint32_t endAddr;   // 结束地址是否页对齐
    uint32_t flag = 0;  // 页不对齐时,是否跨页

    startAddr = addr % FLASH_PAGE_SIZE;
    endAddr   = (addr + len) % FLASH_PAGE_SIZE;
    if (startAddr == 0)
    {
        // 起始地址页对齐
        pageAddr = addr;
        // 计算写Flash的页数
        pageCount = len / FLASH_PAGE_SIZE;
        for (i = 0; i < pageCount; i++)
        {
            // 写Flash前先擦除
            if (FLASH_ErasePage(pageAddr) != 0)
            {
                return FLASH_ERASE_FAIL;
            }
            // 按页写Flash
            if (FLASH_WritePage(pageAddr, src + (i * FLASH_PAGE_SIZE)))
            {
                return FLASH_WRITE_FAIL;
            }
            pageAddr += FLASH_PAGE_SIZE;
        }
        if (endAddr != 0)
        {
            // 结束地址页不对齐，需要备份最后一页数据
            for (i = 0; i < FLASH_PAGE_SIZE; i++)
            {
                g_FlashBuf[i] = ((uint8_t*)pageAddr)[i];
            }
            // 计算按页对齐的余地址
            tmpAddr = len % FLASH_PAGE_SIZE;
            // 把需要写入Flash的数据保存到buf中
            for (i = 0; i < tmpAddr; i++)
            {
                g_FlashBuf[i] = ((uint8_t*)(src + (pageCount * FLASH_PAGE_SIZE)))[i];
            }
            // 写Flash前先擦除
            if (FLASH_ErasePage(pageAddr) != 0)
            {
                return FLASH_ERASE_FAIL;
            }
            // 写最后一页
            if (FLASH_WritePage(pageAddr, g_FlashBuf))
            {
                return FLASH_WRITE_FAIL;
            }
        }
    }
    else
    {
        // 起始地址页不对齐，计算首页的页对齐地址
        pageAddr = (addr / FLASH_PAGE_SIZE) * FLASH_PAGE_SIZE;
        // 计算第一页按页对齐的余地址
        tmpAddr = addr % FLASH_PAGE_SIZE;
        // 计算首页需要写入Flash的长度
        tmpLen = FLASH_PAGE_SIZE - tmpAddr;
        if (tmpLen > len)
        {
            tmpLen = len;
            flag   = 0; // 不跨页
        }
        else
        {
            flag = 1; // 跨页
        }
        // 计算写Flash的页数
        pageCount = (len - tmpLen) / FLASH_PAGE_SIZE;
        // 备份Flash数据
        for (i = 0; i < FLASH_PAGE_SIZE; i++)
        {
            g_FlashBuf[i] = ((uint8_t*)pageAddr)[i];
        }

        for (i = 0; i < tmpLen; i++)
        {
            g_FlashBuf[tmpAddr + i] = ((uint8_t*)src)[i];
        }
        // 写Flash前先擦除
        if (FLASH_ErasePage(pageAddr) != 0)
        {
            return FLASH_ERASE_FAIL;
        }
        // 写首页Flash
        if (FLASH_WritePage(pageAddr, g_FlashBuf))
        {
            return FLASH_WRITE_FAIL;
        }
        pageAddr += FLASH_PAGE_SIZE;
        for (i = 0; i < pageCount; i++)
        {
            // 写Flash前先擦除
            if (FLASH_ErasePage(pageAddr) != 0)
            {
                return FLASH_ERASE_FAIL;
            }
            // 按页写Flash
            if (FLASH_WritePage(pageAddr, src + tmpLen + (i * FLASH_PAGE_SIZE)))
            {
                return FLASH_WRITE_FAIL;
            }
            pageAddr += FLASH_PAGE_SIZE;
        }
        if ((endAddr != 0) && (flag == 1))
        {
            // 结束地址页不对齐，需要备份最后一页数据
            for (i = 0; i < FLASH_PAGE_SIZE; i++)
            {
                g_FlashBuf[i] = ((uint8_t*)pageAddr)[i];
            }
            // 计算按页对齐的余地址
            tmpAddr = (len - tmpLen) % FLASH_PAGE_SIZE;
            // 把需要写入Flash的数据保存到buf中
            for (i = 0; i < tmpAddr; i++)
            {
                g_FlashBuf[i] = ((uint8_t*)(src + tmpLen + (pageCount * FLASH_PAGE_SIZE)))[i];
            }
            // 写Flash前先擦除
            if (FLASH_ErasePage(pageAddr) != 0)
            {
                return FLASH_ERASE_FAIL;
            }
            // 写最后一页
            if (FLASH_WritePage(pageAddr, g_FlashBuf))
            {
                return FLASH_WRITE_FAIL;
            }
        }
    }
    return FLASH_SUCCESS;
}
