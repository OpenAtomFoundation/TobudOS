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
 * @file n32g020xx_crc.c
 * @author Nations Solution Team
 * @version v1.0.1
 * @brief CRC16/LRC校验计算模块接口实现文件。
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "n32g020xx_crc.h"
#include "n32g020xx_common.h"
#include "n32g020xx.h"
//-----------------------------------------------------------------------------
//                        basefunc
//-----------------------------------------------------------------------------

void CRC_Init(void)
{
    //配置CGU模块中MCGR1寄存器的CRC时钟使能
    SET_REG_BIT_MSK(CGU->MCGR1,BIT0,BIT0);
    //清除CRCCR寄存器BIT2的值，不清除Crc16校验值寄存器
    SET_REG_BIT_MSK(CRC->CR, 0, BIT2);
    //设置待校验数据从低位或高位开始运算。0：从高位到低位，1：从低位到高位
    SET_REG_BIT_MSK(CRC->CR, 0, BIT1);
}

void SetCRC(uint16_t initcrc)
{
    //设置CRC->H8R寄存器，循环冗余校验码高8位值
    SET_REG_VAL(CRC->H8R, (initcrc >> 8) & 0xFF);
    //设置CRC->L8R寄存器，循环冗余校验码低8位值
    SET_REG_VAL(CRC->L8R, initcrc & 0xFF);

}

uint16_t GetCRC(void)
{
    uint16_t crc = 0;
    uint16_t tmp_h, tmp_l;
    //获取CRC->H8R寄存器，循环冗余校验码高8位的值
    tmp_h = GET_REG_VAL(CRC->H8R);
    //获取CRC->L8R寄存器，循环冗余校验码低8位的值
    tmp_l = GET_REG_VAL(CRC->L8R);

    crc = tmp_h << 8 | tmp_l;
    return crc;
}

int32_t CRC16_Calc(uint32_t init_val, const uint8_t* data, uint32_t len, uint16_t* out_val)
{
    uint32_t i;
    //CRC初始化
    CRC_Init();	
    if (init_val < 0x10000)
    {
        //配置CRC16初值
        SetCRC(init_val);
    }
    for (i = 0; i < len; i++)
    {
        //写入待校验数据
		SET_REG_VAL(CRC->DR, data[i]); 
    }
    //获取CRC16的值
    * out_val = GetCRC();
    return CRC_SUCCESS;
}

int32_t LRC_Calc(uint16_t init_val, const uint8_t* data, uint32_t len, uint8_t* out_val)
{
    uint32_t i;
    //CRC初始化
    CRC_Init();
    if (init_val < 0x100)
    {
        //配置CRC->LRC寄存器的值
        SET_REG_VAL(CRC->LRC, init_val);
    }
    for (i = 0; i < len; i++)
    {
        //写入待校验数据
		SET_REG_VAL(CRC->DR, data[i]);
    }
    //获取CRC->LRC的值
    * out_val = GET_REG_VAL(CRC->LRC);
    return CRC_SUCCESS;
}


