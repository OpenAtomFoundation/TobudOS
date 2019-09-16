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
 * @brief SPI模块接口实现文件。
 * @file n32g020xx_spi.c
 * @author Nations Solution Team
 * @version v1.0.1
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 *
 */

#include <stdbool.h>

#include "n32g020xx_spi.h"

#include "n32g020xx_cgu.h"
#include "n32g020xx_common.h"
#include "n32g020xx_iom.h"
#include "n32g020xx.h"

#define SPI_CHANNEL_NUMBER 2

// -------------------------SPIM--------------------------- //
//
/*
 * 每个SPIM通道的相关常量信息
 */
typedef struct
{
    uint64_t cgum;     // SPIM通道对应的CGU PCLK模块号
    SPIMTypeDef* regs; // SPIM通道对应的寄存器组的起始地址
    IRQn_Type irqn;    // SPIM通道对应的中断号
} SpimChannelInfo;

static const SpimChannelInfo s_spim_channel_infos[SPI_CHANNEL_NUMBER] = {
    {
        .cgum = CGUM_SPIM0_PCLK,
        .regs = SPIM0,
        .irqn = Spim0_Exception_IrqNum,
    },
    {
        .cgum = CGUM_SPIM1_PCLK,
        .regs = SPIM1,
        .irqn = Spim1_Exception_IrqNum,
    }};

// 配置SPIM的管脚
static void SPIM_InitIOM(SpiChannel channel)
{
    switch (channel)
    {
    case SPI_CH0:
        IOM_ConfigGpio8Spim0ClkOut();
        IOM_ConfigGpio9Spim0CsnOut();
        IOM_ConfigGpio10Spim0MOSI();
        IOM_ConfigGpio11Spim0MISO();
        break;
    case SPI_CH1:
        IOM_ConfigGpio14Spim1ClkOut();
        IOM_ConfigGpio15Spim1CsnOut();
        IOM_ConfigGpio16Spim1MISO();
        IOM_ConfigGpio17Spim1MOSI();
        break;
    default:
        break;
    }
}

static bool SPIM_ConfigClock(SPIMTypeDef* spim_regs, uint32_t freq)
{
    // SPI的时钟频率由两级分频产生，1级为1~128，2级为1~256，最高时钟为20M
    // 我们只支持配置2级分频，1级固定为1，这样最大256分频时，时钟为78K，应该能满
    // 足需求了
    uint32_t div;

    div = CGU_GetClkTopFrequence() / freq / 2; // 总的分频值

    if (div < 1 || div > 256)
    {
        return false;
    }

    SET_REG_VAL(spim_regs->BAUD1, div - 1); // 2级分频值
    SET_REG_VAL(spim_regs->BAUD2, 1);      // 1级分频固定为1

    return true;
}

static int32_t SPIM_Init(SpiChannel channel, const SpiConfig* config)
{
    SPIMTypeDef* spim_regs;
    uint64_t cgum = CGUM_IOM_PCLK;
    spim_regs     = s_spim_channel_infos[(int)channel].regs;
    cgum |= s_spim_channel_infos[(int)channel].cgum;

    // 一帧数据为4~8位
    if (config->frame_width < 4 || config->frame_width > 8)
    {
        return SPI_PARAM_INCORRECT;
    }

    // 目前只支持标准传输类型
    if (config->trans_type != SPI_TT_STANDARD)
    {
        return SPI_PARAM_INCORRECT;
    }
#if 0
    // Dual 和 Quad 模式下，必须配置为 8bit
    if (config->trans_type != SPI_TF_STANDARD && config->frame_width != 8)
    {
        return SPI_PARAM_INCORRECT;
    }
#endif

    if (CGU_SUCCESS != CGU_EnableModuleFunctions(cgum))
    {
        return SPI_PARAM_INCORRECT;
    }

    SPIM_InitIOM(channel);

    uint32_t spicr_val = 0;

    spicr_val |= config->trans_type << 8;            // SPIMOD
    spicr_val |= (config->frame_format & 0x01) << 7; // Bit7 SPH，相位
    spicr_val |= (config->frame_format & 0x02) << 5; // Bit6 SPO，极性

    // HSIZE FIFO操作位宽为8位，bit5~4为00，这里不用再设置

    spicr_val |= config->frame_width - 1; // DSSP

    SET_REG_VAL(spim_regs->CR, spicr_val);

    if (!SPIM_ConfigClock(spim_regs, config->clock_freq))
    {
        return SPI_PARAM_INCORRECT;
    }

    return SPI_SUCCESS;
}

#define SPIM_DATA32(regs) (*(volatile uint32_t*)(&(regs)->DR))
#define SPIM_DATA16(regs) (*(volatile uint16_t*)(&(regs)->DR))
#define SPIM_DATA8(regs) (*(volatile uint8_t*)(&(regs)->DR))

#define SPIM_GetTxFifoCount(regs)                                              \
    GET_REG_BIT((regs)->DSR, BIT0 | BIT1 | BIT2 | BIT3)

static void SPIM_ClearRxFifo(SPIMTypeDef* spim_regs)
{
    uint8_t tmp = 0;
    while (GET_REG_BIT(spim_regs->ISR, BIT2)) // RxFIFO非空标志为1
    {
        tmp = SPIM_DATA8(spim_regs);
        (void)tmp;/* Ignore compile warning */
    }
}

static void SPIM_ClearTxFifo(SPIMTypeDef* spim_regs)
{
#if 0
    while (GET_REG_BIT(spim_regs->DSR, BIT0|BIT1|BIT2|BIT3)) // TxFIFO数据填充数
    {

    }
#endif
}

// transmit
static int32_t SPIM_Transmit(SpiChannel channel,
                             const uint8_t* send_buf,
                             uint8_t* recv_buf,
                             uint32_t length)
{
    if (send_buf == 0 || recv_buf == 0)
    {
        // 不能为空指针
        return SPI_PARAM_INCORRECT;
    }

    SPIMTypeDef* spim_regs = s_spim_channel_infos[(int)channel].regs;

    SPIM_ClearRxFifo(spim_regs);
    SPIM_ClearTxFifo(spim_regs);

    SET_REG_BIT_MSK(
        spim_regs->EN, BIT4, BIT4 | BIT2); // SSE，传输使能，收发模式
    SET_REG_BIT(spim_regs->ISR, BIT1);     // 清传输完成标志

    CLR_REG_BIT(spim_regs->EN, BIT5); // 片选拉低

    uint32_t recv_left = length;
    while (length || recv_left)
    {
        if (length >= 4)
        {
            if (SPIM_GetTxFifoCount(spim_regs) <= 4)
            {
                SPIM_DATA8(spim_regs) = *send_buf++;
                SPIM_DATA8(spim_regs) = *send_buf++;
                SPIM_DATA8(spim_regs) = *send_buf++;
                SPIM_DATA8(spim_regs) = *send_buf++;
                length -= 4;
            }
        }
        else if (length)
        {
            if (SPIM_GetTxFifoCount(spim_regs) <= 4)
            {
                for (uint32_t i = 0; i < length; ++i)
                {
                    SPIM_DATA8(spim_regs) = *send_buf++;
                }
                length = 0;
            }
        }

        while (GET_REG_BIT(spim_regs->ISR, BIT2)) // RxFIFO非空
        {
            *recv_buf++ = SPIM_DATA8(spim_regs);
            recv_left--;
        }
    }

    while (GET_REG_BIT(spim_regs->ISR, BIT1) == 0)
        ; // 等待传输完成

    SET_REG_BIT(spim_regs->ISR, BIT1); // 清传输完成标志
    SET_REG_BIT(spim_regs->EN, BIT5);  // 片选拉高

    return SPI_SUCCESS;
}

// send
static int32_t
SPIM_SendData(SpiChannel channel, const uint8_t* send_buf, uint32_t length)
{
    if (send_buf == 0)
    {
        // 不能为空指针
        return SPI_PARAM_INCORRECT;
    }

    SPIMTypeDef* spim_regs = s_spim_channel_infos[(int)channel].regs;

    SPIM_ClearRxFifo(spim_regs);
    SPIM_ClearTxFifo(spim_regs);

    SET_REG_BIT_MSK(
        spim_regs->EN, BIT4, BIT4 | BIT2); // SSE，传输使能，收发模式
    SET_REG_BIT(spim_regs->ISR, BIT1);     // 清传输完成标志

    CLR_REG_BIT(spim_regs->EN, BIT5); // 片选拉低

    while (length)
    {
        if (length >= 4)
        {
            if (SPIM_GetTxFifoCount(spim_regs) <= 4)
            {
                SPIM_DATA8(spim_regs) = *send_buf++;
                SPIM_DATA8(spim_regs) = *send_buf++;
                SPIM_DATA8(spim_regs) = *send_buf++;
                SPIM_DATA8(spim_regs) = *send_buf++;
                length -= 4;
            }
        }
        else if (length)
        {
            if (SPIM_GetTxFifoCount(spim_regs) <= 4)
            {
                for (uint32_t i = 0; i < length; ++i)
                {
                    SPIM_DATA8(spim_regs) = *send_buf++;
                }
                length = 0;
            }
        }
    }

    while (GET_REG_BIT(spim_regs->ISR, BIT1) == 0)
        ; // 等待传输完成

    SET_REG_BIT(spim_regs->ISR, BIT1); // 清传输完成标志
    SET_REG_BIT(spim_regs->EN, BIT5);  // 片选拉高

    SPIM_ClearRxFifo(spim_regs);       // 忽略任何收到的数据
    SET_REG_BIT(spim_regs->ISR, BIT4); // 清RxFIFO溢出标识

    return SPI_SUCCESS;
}

// 只收不发模式
static int32_t
SPIM_ReceiveData(SpiChannel channel, uint8_t* recv_buf, uint32_t length)
{
    if (recv_buf == 0)
    {
        // 不能为空指针
        return SPI_PARAM_INCORRECT;
    }
    if (length > 1023)
    {
        // 只收不发模式最多只支持1023字节长度
        return SPI_PARAM_INCORRECT;
    }

    SPIMTypeDef* spim_regs = s_spim_channel_infos[(int)channel].regs;

    SPIM_ClearRxFifo(spim_regs);
    SPIM_ClearTxFifo(spim_regs);

    SET_REG_VAL(spim_regs->RXCNT, length);

    CLR_REG_BIT(spim_regs->EN, BIT5); // 片选拉低

    SET_REG_BIT(spim_regs->EN, BIT2 | BIT4); // SSE，传输使能，只收不发使能
    SET_REG_BIT(spim_regs->ISR, BIT1); // 清传输完成标志

    while (length)
    {
        if (length >= 4)
        {
            while (!GET_REG_BIT(spim_regs->ISR, BIT3))
                ; // 等待RxFIFO半满
            *recv_buf++ = SPIM_DATA8(spim_regs);
            *recv_buf++ = SPIM_DATA8(spim_regs);
            *recv_buf++ = SPIM_DATA8(spim_regs);
            *recv_buf++ = SPIM_DATA8(spim_regs);
            length -= 4;
        }
        else
        {
            while (!GET_REG_BIT(spim_regs->ISR, BIT3))
                ; // 等待RxFIFO非空
            *recv_buf++ = SPIM_DATA8(spim_regs);
            length--;
        }
    }

    SET_REG_BIT(spim_regs->ISR, BIT1); // 清传输完成标志
    SET_REG_BIT(spim_regs->EN, BIT5);  // 片选拉高

    return SPI_SUCCESS;
}

static int32_t SPIM_Close(SpiChannel channel)
{
    uint64_t cgum = s_spim_channel_infos[(int)channel].cgum;

    CGU_DisableModuleFunctions(cgum);
    return SPI_SUCCESS;
}

//-------------------- SPIS ---------------------------//

/*
 * 每个SPIS通道的相关常量信息
 */
typedef struct
{
    uint64_t cgum;     // SPIS通道对应的CGU PCLK模块号
    SPISTypeDef* regs; // SPIS通道对应的寄存器组的起始地址
    IRQn_Type irqn;    // SPIS通道对应的中断号
} SpisChannelInfo;

static const SpisChannelInfo s_spis_channel_infos[SPI_CHANNEL_NUMBER] = {
    {
        .cgum = CGUM_SPIS0_PCLK | CGUM_SPIS0_CLK,
        .regs = SPIS0,
        .irqn = Spis0_Exception_IrqNum,
    },
    {
        .cgum = CGUM_SPIS1_PCLK | CGUM_SPIS1_CLK,
        .regs = SPIS1,
        .irqn = Spis1_Exception_IrqNum,
    }};

#define SPIS_DATA32(regs) (*(volatile uint32_t*)(&(regs)->DATA))
#define SPIS_DATA16(regs) (*(volatile uint16_t*)(&(regs)->DATA))
#define SPIS_DATA8(regs) (*(volatile uint8_t*)(&(regs)->DATA))

static void SPIS_InitIOM(SpiChannel channel)
{
    switch (channel)
    {
    case SPI_CH0:
        IOM_ConfigGpio8Spis0ClkIn();
        IOM_ConfigGpio9Spis0CsnIn();
        IOM_ConfigGpio10Spis0MISO();
        IOM_ConfigGpio11Spis0MOSI();
        break;
    case SPI_CH1:
        IOM_ConfigGpio14Spis1ClkIn();
        IOM_ConfigGpio15Spis1CsnIn();
        IOM_ConfigGpio16SPis1MOSI();
        IOM_ConfigGpio17Spis1MISO();
        break;
    default:
        break;
    }
}

static int32_t SPIS_Init(SpiChannel channel, const SpiConfig* config)
{
    // 根据SPIS通道选择相应CGU模块和寄存器基地址
    uint64_t cgum = CGUM_IOM_PCLK;
    SPISTypeDef* spis_regs;
    cgum |= s_spis_channel_infos[(int)channel].cgum;
    spis_regs = s_spis_channel_infos[(int)channel].regs;

    // 使能CGU时钟
    if (CGU_SUCCESS != CGU_EnableModuleFunctions(cgum))
    {
        return SPI_PARAM_INCORRECT;
    }

    // 配置IOM管脚
    SPIS_InitIOM(channel);

    uint32_t cr_val = 0;
    // 配置时钟极性和相位
    switch (config->frame_format)
    {
    case SPI_FF_CPOL0_CPHA0:
    case SPI_FF_CPOL0_CPHA1:
    case SPI_FF_CPOL1_CPHA0:
    case SPI_FF_CPOL1_CPHA1:
        cr_val |= (uint32_t)config->frame_format;
        break;
    default:
        return SPI_PARAM_INCORRECT;
    }

    SET_REG_VAL(spis_regs->CR, cr_val);
    SET_REG_VAL(spis_regs->DCR, 0); // 发送，接收复位，TXSIZE=1，RXSIZE=1

    return SPI_SUCCESS;
}

static int32_t SPIS_Transmit(SpiChannel channel,
                             const uint8_t* send_buf,
                             uint8_t* recv_buf,
                             uint32_t length)
{
    SPISTypeDef* spis_regs = s_spis_channel_infos[(int)channel].regs;

    SET_REG_VAL(spis_regs->DCR, 0); // 发送，接收复位，TXSIZE=1，RXSIZE=1

    // 清中断标识
    SET_REG_VAL(spis_regs->IFR, 0xff);

    // 使能发送和接收
    SET_REG_BIT(spis_regs->CR, BIT6); // TXEN=1
    SET_REG_BIT(spis_regs->CR, BIT5); // RXEN=1

    while (!GET_REG_BIT(spis_regs->SR, BIT7))
        ; // 等待SPIS被SPI总线选通

    /* SPIS的第一个字节会发送一个无效数据，因此对于主机端，需要多一个字节的clock
     * 才能收完所有SPIS发的数据，因此主机数据要比从机数据多1
     */
    uint32_t recv_left = length + 1;
    while (length || recv_left)
    {
        if (length >= 4)
        {
            if (GET_REG_BIT(spis_regs->SR, BIT5)) // 发送FIFO半空
            {
                SPIS_DATA8(spis_regs) = *send_buf++;
                SPIS_DATA8(spis_regs) = *send_buf++;
                SPIS_DATA8(spis_regs) = *send_buf++;
                SPIS_DATA8(spis_regs) = *send_buf++;
                length -= 4;
            }
        }
        else if (length)
        {
            if (GET_REG_BIT(spis_regs->SR, BIT5)) // 发送FIFO半空
            {
                for (uint32_t i = 0; i < length; ++i)
                {
                    SPIS_DATA8(spis_regs) = *send_buf++;
                }
                length = 0;
            }
        }

        while (GET_REG_BIT(spis_regs->SR, BIT0)) // 接收FIFO非空
        {
            *recv_buf++ = SPIS_DATA8(spis_regs);
            recv_left--;
        }
    }
    while (!GET_REG_BIT(spis_regs->SR, BIT1))
        ; // 等待发送将完

    return SPI_SUCCESS;
}

static int32_t
SPIS_SendData(SpiChannel channel, const uint8_t* send_buf, uint32_t length)
{
    SPISTypeDef* spis_regs = s_spis_channel_infos[(int)channel].regs;

    // 清中断标识
    SET_REG_VAL(spis_regs->IFR, 0xff);

    // 使能发送
    SET_REG_BIT(spis_regs->CR, BIT6); // TXEN=1
    // 禁止接收
    CLR_REG_BIT(spis_regs->CR, BIT5); // RXEN=0

    while (!GET_REG_BIT(spis_regs->SR, BIT7))
        ; // 等待SPIS被SPI总线选通

    while (length)
    {
        if (length >= 4)
        {
            if (GET_REG_BIT(spis_regs->SR, BIT5)) // 发送FIFO半空
            {
                SPIS_DATA8(spis_regs) = *send_buf++;
                SPIS_DATA8(spis_regs) = *send_buf++;
                SPIS_DATA8(spis_regs) = *send_buf++;
                SPIS_DATA8(spis_regs) = *send_buf++;
                length -= 4;
            }
        }
        else if (length)
        {
            if (GET_REG_BIT(spis_regs->SR, BIT5)) // 发送FIFO半空
            {
                for (uint32_t i = 0; i < length; ++i)
                {
                    SPIS_DATA8(spis_regs) = *send_buf++;
                }
                length = 0;
            }
        }
    }
    while (!GET_REG_BIT(spis_regs->SR, BIT1))
        ; // 等待发送将完

    return SPI_SUCCESS;
}

static int32_t
SPIS_ReceiveData(SpiChannel channel, uint8_t* recv_buf, uint32_t length)
{
    SPISTypeDef* spis_regs = s_spis_channel_infos[(int)channel].regs;

    // 清中断标识
    SET_REG_VAL(spis_regs->IFR, 0xff);

    // 禁止发送
    CLR_REG_BIT(spis_regs->CR, BIT6); // TXEN=0
    // 使能接收
    SET_REG_BIT(spis_regs->CR, BIT5); // RXEN=1

    while (!GET_REG_BIT(spis_regs->SR, BIT7))
        ; // 等待SPIS被SPI总线选通

    while (length)
    {
        while (GET_REG_BIT(spis_regs->SR, BIT0)) // 接收FIFO非空
        {
            *recv_buf++ = SPIS_DATA8(spis_regs);
            length--;
        }
    }

    return SPI_SUCCESS;
}

static int32_t SPIS_Close(SpiChannel channel)
{
    uint64_t cgum = s_spis_channel_infos[(int)channel].cgum;

    CGU_DisableModuleFunctions(cgum);
    return SPI_SUCCESS;
}

// --------------------SPI 接口-------------------------//

#define SPI_INIT_FLAG 0x80

static uint8_t s_spi_channel_modes[SPI_CHANNEL_NUMBER] = {0, 0};

#define SPI_SetMode(channel, mode)                                             \
    s_spi_channel_modes[(int32_t)(channel)] = SPI_INIT_FLAG | (mode);

#define SPI_ClearMode(channel) s_spi_channel_modes[(int32_t)(channel)] = 0;

#define SPI_IsInited(channel)                                                  \
    ((s_spi_channel_modes[(int32_t)(channel)] & SPI_INIT_FLAG) != 0)

#define SPI_GetMode(channel)                                                   \
    ((s_spi_channel_modes[(int32_t)(channel)] & (~SPI_INIT_FLAG)))

int32_t SPI_Init(SpiChannel channel, const SpiConfig* config)
{
    int32_t ret_code;
    switch (channel)
    {
    case SPI_CH0:
    case SPI_CH1:
        // 根据配置，初始化通道对应的函数结构体指针
        if (config->mode == SPI_MODE_MASTER)
        {
            ret_code = SPIM_Init(channel, config);
        }
        else
        {
            ret_code = SPIS_Init(channel, config);
        }
        if (ret_code == SPI_SUCCESS)
        {
            SPI_SetMode(channel, config->mode);
        }
        return ret_code;
    default:
        return SPI_PARAM_INCORRECT;
    }
}

int32_t SPI_Transmit(SpiChannel channel,
                     const uint8_t* send_buf,
                     uint8_t* recv_buf,
                     uint32_t length)
{
    switch (channel)
    {
    case SPI_CH0:
    case SPI_CH1:
        if (!SPI_IsInited(channel))
        {
            return SPI_UNINITED;
        }
        break;
    default:
        return SPI_PARAM_INCORRECT;
    }

    if (SPI_GetMode(channel) == SPI_MODE_MASTER)
    {
        return SPIM_Transmit(channel, send_buf, recv_buf, length);
    }
    else
    {
        return SPIS_Transmit(channel, send_buf, recv_buf, length);
    }
}

int32_t
SPI_SendData(SpiChannel channel, const uint8_t* send_buf, uint32_t length)
{
    switch (channel)
    {
    case SPI_CH0:
    case SPI_CH1:
        if (!SPI_IsInited(channel))
        {
            return SPI_UNINITED;
        }
        break;
    default:
        return SPI_PARAM_INCORRECT;
    }

    if (SPI_GetMode(channel) == SPI_MODE_MASTER)
    {
        return SPIM_SendData(channel, send_buf, length);
    }
    else
    {
        return SPIS_SendData(channel, send_buf, length);
    }
}

int32_t SPI_ReceiveData(SpiChannel channel, uint8_t* recv_buf, uint32_t length)
{
    switch (channel)
    {
    case SPI_CH0:
    case SPI_CH1:
        if (!SPI_IsInited(channel))
        {
            return SPI_UNINITED;
        }
        break;
    default:
        return SPI_PARAM_INCORRECT;
    }

    if (SPI_GetMode(channel) == SPI_MODE_MASTER)
    {
        return SPIM_ReceiveData(channel, recv_buf, length);
    }
    else
    {
        return SPIS_ReceiveData(channel, recv_buf, length);
    }
}

int32_t SPI_Close(SpiChannel channel)
{
    switch (channel)
    {
    case SPI_CH0:
    case SPI_CH1:
        if (!SPI_IsInited(channel))
        {
            return SPI_UNINITED;
        }
        break;
    default:
        return SPI_PARAM_INCORRECT;
    }

    int32_t ret_code;
    if (SPI_GetMode(channel) == SPI_MODE_MASTER)
    {
        ret_code = SPIM_Close(channel);
    }
    else
    {
        ret_code = SPIS_Close(channel);
    }

    if (ret_code == SPI_SUCCESS)
    {
        SPI_ClearMode(channel);
    }
    return ret_code;
}
