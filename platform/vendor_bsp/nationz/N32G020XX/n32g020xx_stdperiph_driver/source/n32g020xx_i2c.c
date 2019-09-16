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
/** ****************************************************************************
 * @file n32g020xx_i2c.c
 * @author Nations Solution Team
 * @date     2018-12-29 14:43:20
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @brief    I2C Driver source file
 * @addtogroup I2C
 * @{
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "n32g020xx.h"
#include "n32g020xx_common.h"
#include "n32g020xx_i2c.h"
#include "n32g020xx_cgu.h"
#include "n32g020xx_iom.h"
#include "n32g020xx_rcu.h"
#include "n32g020xx_dma.h"


/// I2C控制寄存器DMA使能位
#define I2C_CR_MDEN_M BIT7
/// I2C控制寄存器中断使能位
#define I2C_CR_MIEN_M BIT6
/// I2C控制寄存器RepeatStart位
#define I2C_CR_RSTA_M BIT5
/// I2C控制寄存器传输应答/STOP条件位
#define I2C_CR_TACK_M BIT4
/// I2C控制寄存器传输模式
#define I2C_CR_MTX_M BIT3
/// I2C控制寄存器主从模式选择位，START位
#define I2C_CR_MSTA_M BIT2
/// I2C控制寄存器模块使能位
#define I2C_CR_MEN_M BIT0

/// I2C状态寄存器传输状态位
#define I2C_SR_MTF_M BIT7
/// I2C状态寄存器地址匹配位
#define I2C_SR_MAAS_M BIT6
/// I2C状态寄存器总线忙标志位
#define I2C_SR_MBB_M BIT5
/// I2C状态寄存器仲裁丢失状态位
#define I2C_SR_MAL_M BIT4
/// I2C状态寄存器从设备读写状态位
#define I2C_SR_SRW_M BIT3
/// I2C状态寄存器中断状态位
#define I2C_SR_MINT_M BIT2
/// I2C状态寄存器应答接收状态位
#define I2C_SR_RACK_M BIT1

/// I2C寄存器通道映射宏
#define I2Cx(channel) ((I2CTypeDef*)(I2C0_BASE + (channel << 10)))

/// I2C寄存器读
#define I2C_REG_GET(channel, reg) GET_REG_VAL(I2Cx(channel)->reg)
/// I2C寄存器写
#define I2C_REG_SET(channel, reg, val) SET_REG_VAL(I2Cx(channel)->reg, (val))
/// I2C寄存器按位读
#define I2C_REG_GET_FIELD(channel, reg, field) GET_REG_BIT(I2Cx(channel)->reg, (field))
/// I2C寄存器按位写
#define I2C_REG_SET_FIELD(channel, reg, field) SET_REG_BIT(I2Cx(channel)->reg, (field))
/// I2C寄存器按位清除
#define I2C_REG_CLR_FIELD(channel, reg, field) CLR_REG_BIT(I2Cx(channel)->reg, (field))

typedef struct _I2CSlaveHandlers
{
    I2CSlaveASHandler asHandler; ///< I2C从地址匹配中断回调
    I2CSlaveTxHandler txHandler; ///< I2C从发送中断回调
    I2CSlaveRxHandler rxHandler; ///< I2C从接收中断回调
} I2CSlaveHandlers;

static I2CSlaveHandlers sI2CISRHandlers[I2C_NUM] = {0};

static void I2C_IRQService(I2cChannel channel)
{
    uint32_t status = I2C_REG_GET(channel, SR);

    I2C_REG_SET_FIELD(channel, SR, I2C_SR_MINT_M);

    if (status & I2C_SR_MAAS_M)
    {
        if (sI2CISRHandlers[channel].asHandler)
        {
            sI2CISRHandlers[channel].asHandler();
        }
        I2C_SlaveInit(channel);
    }
    else if (status & I2C_SR_MTF_M)
    {
        if (I2C_REG_GET_FIELD(channel, SR, I2C_SR_MBB_M))
        {
            if (!I2C_REG_GET_FIELD(channel, CR, I2C_CR_MTX_M))
            {
                if (sI2CISRHandlers[channel].rxHandler)
                {
                    sI2CISRHandlers[channel].rxHandler(I2C_REG_GET(channel, DR));
                }
            }
            else
            {
                if (!I2C_REG_GET_FIELD(channel, SR, I2C_SR_RACK_M))
                {
                    if (sI2CISRHandlers[channel].txHandler)
                    {
                        I2C_REG_SET(channel, DR, sI2CISRHandlers[channel].txHandler());
                    }
                }
            }
        }
    }

    if (status & I2C_SR_MAL_M)
    {
        I2C_REG_CLR_FIELD(channel, CR, I2C_CR_MTX_M);
    }
}

void I2C0_IRQHandler(void)
{
    I2C_IRQService(I2C_CH0);
}

void I2C1_IRQHandler(void)
{
    I2C_IRQService(I2C_CH1);
}

static int32_t I2C_SCKFreqSetup(I2cChannel channel, uint32_t speed)
{
    uint32_t fdr;
    uint32_t topclk;

    if (speed > I2C_SPEED_MAX)
        return I2C_PARAM_INCORRECT;

    topclk = CGU_GetClkTopFrequence();
    fdr    = topclk / (speed * 4) - 1;

    if (fdr > 0 && fdr <= 0xFF)
    {
        I2C_REG_SET(channel, FDR, fdr & 0xFF);
        return I2C_SUCCESS;
    }
    else
    {
        return I2C_PARAM_INCORRECT;
    }
}

int32_t I2C_DeInit(I2cChannel channel)
{
    switch (channel)
    {
    case I2C_CH0:
        // RCU_SetI2c0SoftRst();
        CGU_DisableModuleFunctions(CGUM_I2C0_PCLK);
        IOM_Gpio18ConfigMode(GPIO18_GPIO);
        IOM_Gpio19ConfigMode(GPIO19_GPIO);
        break;
    case I2C_CH1:
        // RCU_SetI2c1SoftRst();
        CGU_DisableModuleFunctions(CGUM_I2C1_PCLK);
        IOM_Gpio28ConfigMode(GPIO28_GPIO);
        IOM_Gpio29ConfigMode(GPIO29_GPIO);
        break;
    default:
        return I2C_PARAM_INCORRECT;
    }

    I2C_REG_CLR_FIELD(channel, CR, I2C_CR_MEN_M);

    return I2C_SUCCESS;
}

int32_t I2C_Init(I2cChannel channel, uint8_t addr, uint32_t speed)
{
    int32_t ret;

    switch (channel)
    {
    case I2C_CH0:
        // RCU_SetI2c0SoftRst();
        CGU_EnableModuleFunctions(CGUM_I2C0_PCLK);
        IOM_ConfigGpio18I2c0MSClk();
        IOM_ConfigGpio19I2c0MSDa();
        break;
    case I2C_CH1:
        // RCU_SetI2c1SoftRst();
        CGU_EnableModuleFunctions(CGUM_I2C1_PCLK);
        IOM_ConfigGpio28I2c1MSClk();
        IOM_ConfigGpio29I2c1MSDa();
        break;
    default:
        return I2C_PARAM_INCORRECT;
    }

    I2C_REG_SET(channel, DR, 0);
    I2C_REG_SET(channel, CR, 0);
    I2C_REG_SET_FIELD(channel, SR, I2C_SR_MINT_M | I2C_SR_MAL_M);

    I2C_REG_SET(channel, ADR, addr);

    ret = I2C_SCKFreqSetup(channel, speed);

    if (ret == I2C_SUCCESS)
    {
        I2C_REG_SET_FIELD(channel, CR, I2C_CR_MEN_M);
    }
    else
    {
        I2C_DeInit(channel);
    }

    return ret;
}

int32_t I2C_InterruptDisable(I2cChannel channel)
{
    IRQn_Type irq_type = I2c0_Exception_IrqNum;

    if (channel >= I2C_NUM)
    {
        return I2C_PARAM_INCORRECT;
    }
    else if (channel == I2C_CH1)
    {
        irq_type = I2c1_Exception_IrqNum;
    }

    I2C_REG_CLR_FIELD(channel, CR, I2C_CR_MIEN_M);
    NVIC_DisableIRQ(irq_type);

    return I2C_SUCCESS;
}

int32_t I2C_InterruptEnable(I2cChannel channel,
                            I2CSlaveASHandler asHandler,
                            I2CSlaveTxHandler txHandler,
                            I2CSlaveRxHandler rxHandler)
{
    IRQn_Type irq_type = I2c0_Exception_IrqNum;

    if (channel >= I2C_NUM)
    {
        return I2C_PARAM_INCORRECT;
    }
    else if (channel == I2C_CH1)
    {
        irq_type = I2c1_Exception_IrqNum;
    }

    sI2CISRHandlers[channel].asHandler = asHandler;
    sI2CISRHandlers[channel].txHandler = txHandler;
    sI2CISRHandlers[channel].rxHandler = rxHandler;
    NVIC_ClearPendingIRQ(irq_type);
    NVIC_EnableIRQ(irq_type);
    I2C_REG_SET_FIELD(channel, CR, I2C_CR_MIEN_M);

    return I2C_SUCCESS;
}

int32_t I2C_Reset(I2cChannel channel)
{
    switch (channel)
    {
    case I2C_CH0:
        RCU_SetI2c0SoftRst();
        break;
    case I2C_CH1:
        RCU_SetI2c1SoftRst();
        break;
    default:
        return I2C_PARAM_INCORRECT;
    }

    return I2C_SUCCESS;
}

int32_t I2C_Stop(I2cChannel channel)
{
    switch (channel)
    {
    case I2C_CH0:
        CGU_DisableModuleFunctions(CGUM_I2C0_PCLK);
        IOM_Gpio18ConfigMode(GPIO18);
        IOM_Gpio19ConfigMode(GPIO19);
        IOM_ConfigGpioOut(GPIO18, 1);
        IOM_ConfigGpioOut(GPIO19, 1);
        IOM_SetGpioLevel(GPIO18, 0);
        IOM_SetGpioLevel(GPIO19, 0);
        Delay_US(5);
        IOM_SetGpioLevel(GPIO18, 1);
        Delay_US(5);
        IOM_SetGpioLevel(GPIO19, 1);
        Delay_US(5);
        IOM_ConfigGpioOut(GPIO18, 0);
        IOM_ConfigGpioOut(GPIO19, 0);
        break;
    case I2C_CH1:
        CGU_DisableModuleFunctions(CGUM_I2C1_PCLK);
        IOM_Gpio28ConfigMode(GPIO28);
        IOM_Gpio29ConfigMode(GPIO29);
        IOM_ConfigGpioOut(GPIO28, 1);
        IOM_ConfigGpioOut(GPIO29, 1);
        IOM_SetGpioLevel(GPIO28, 0);
        IOM_SetGpioLevel(GPIO29, 0);
        Delay_US(5);
        IOM_SetGpioLevel(GPIO28, 1);
        Delay_US(5);
        IOM_SetGpioLevel(GPIO29, 1);
        Delay_US(5);
        IOM_ConfigGpioOut(GPIO28, 0);
        IOM_ConfigGpioOut(GPIO29, 0);
        break;
    default:
        return I2C_PARAM_INCORRECT;
    }

    I2C_REG_CLR_FIELD(channel, CR, I2C_CR_MDEN_M);
    I2C_REG_CLR_FIELD(channel, CR, I2C_CR_MEN_M);

    return I2C_SUCCESS;
}

int32_t I2C_MasterInit(I2cChannel channel, uint8_t addr, I2cTransferMode mode)
{
    uint32_t timeout;

    if (channel >= I2C_NUM)
        return I2C_PARAM_INCORRECT;

    I2C_REG_SET_FIELD(channel, CR, I2C_CR_MTX_M);

    if (!I2C_REG_GET_FIELD(channel, CR, I2C_CR_MSTA_M))
    {
        I2C_REG_CLR_FIELD(channel, CR, I2C_CR_TACK_M);
        timeout = 10 * (I2C_REG_GET(channel, FDR) + 1) * 4;
        while (I2C_REG_GET_FIELD(channel, SR, I2C_SR_MBB_M))
        {
            timeout--;
            if (timeout == 0)
            {
                return I2C_TIMEOUT;
            }
        }
    }

    I2C_REG_SET(channel, DR, (((addr >> 1) << 1) | mode));
    I2C_REG_SET_FIELD(channel, CR, I2C_CR_MSTA_M);

    timeout = 10 * (I2C_REG_GET(channel, FDR) + 1) * 4;

    while (!I2C_REG_GET_FIELD(channel, SR, I2C_SR_MTF_M))
    {
        timeout--;
        if (timeout == 0)
        {
            return I2C_TIMEOUT;
        }
    }

    I2C_REG_SET_FIELD(channel, SR, I2C_SR_MINT_M);

    if (!I2C_REG_GET_FIELD(channel, SR, I2C_SR_RACK_M))
    {
        if (mode == I2C_RX)
        {
            I2C_REG_CLR_FIELD(channel, CR, I2C_CR_MTX_M);
            volatile uint32_t mtfclr = I2C_REG_GET(channel, DR);
        }

        return I2C_SUCCESS;
    }
    else
    {
        return I2C_FAILED;
    }
}

int32_t I2C_MasterSendData(I2cChannel channel, const uint8_t* data, uint32_t length, uint32_t options)
{
    int32_t ret = I2C_SUCCESS;

    if (channel >= I2C_NUM)
        return I2C_PARAM_INCORRECT;

    while (length--)
    {
        if (0 == length)
        {
            if (options & I2C_OPT_STOP)
            {
                I2C_REG_SET_FIELD(channel, CR, I2C_CR_TACK_M);
            }
            else if (options & I2C_OPT_CONTINUE)
            {
                I2C_REG_SET_FIELD(channel, CR, I2C_CR_RSTA_M);
            }
        }

        I2C_REG_SET(channel, DR, *data);
        data++;
        while (!I2C_REG_GET_FIELD(channel, SR, I2C_SR_MTF_M))
            ;
        I2C_REG_SET_FIELD(channel, SR, I2C_SR_MINT_M);

        if (I2C_REG_GET_FIELD(channel, SR, I2C_SR_RACK_M))
        {
            ret = I2C_FAILED;
            break;
        }
    }

    I2C_REG_CLR_FIELD(channel, CR, I2C_CR_TACK_M | I2C_CR_RSTA_M);

    return ret;
}

int32_t I2C_MasterRecvData(I2cChannel channel, uint8_t* data, uint32_t length, uint32_t options)
{
    if (channel >= I2C_NUM)
        return I2C_PARAM_INCORRECT;

    while (length--)
    {
        if (0 == length)
        {
            if (options & I2C_OPT_STOP)
            {
                I2C_REG_SET_FIELD(channel, CR, I2C_CR_TACK_M);
            }
        }

        while (!I2C_REG_GET_FIELD(channel, SR, I2C_SR_MTF_M))
            ;

        *data = I2C_REG_GET(channel, DR);
        data++;
        I2C_REG_SET_FIELD(channel, SR, I2C_SR_MINT_M);
    }

    I2C_REG_CLR_FIELD(channel, CR, I2C_CR_TACK_M);

    return I2C_SUCCESS;
}

int32_t I2C_SlaveGetTranserMode(I2cChannel channel, I2cTransferMode* mode)
{
    if (channel >= I2C_NUM)
        return I2C_PARAM_INCORRECT;

    if (I2C_REG_GET_FIELD(channel, CR, I2C_CR_MTX_M))
    {
        *mode = I2C_TX;
    }
    else
    {
        *mode = I2C_RX;
    }

    return I2C_SUCCESS;
}

int32_t I2C_SlaveInit(I2cChannel channel)
{
    if (channel >= I2C_NUM)
        return I2C_PARAM_INCORRECT;

    if (I2C_REG_GET_FIELD(channel, SR, I2C_SR_SRW_M))
    {
        I2C_REG_SET_FIELD(channel, CR, I2C_CR_MTX_M);

        if (sI2CISRHandlers[channel].txHandler)
        {
            I2C_REG_SET(channel, DR, sI2CISRHandlers[channel].txHandler());
        }
    }
    else
    {
        I2C_REG_CLR_FIELD(channel, CR, I2C_CR_MTX_M);
        volatile uint32_t mtfclr = I2C_REG_GET(channel, DR);
    }

    return I2C_SUCCESS;
}

int32_t I2C_SlaveBusWait(I2cChannel channel)
{
    if (channel >= I2C_NUM)
        return I2C_PARAM_INCORRECT;

    while (!I2C_REG_GET_FIELD(channel, SR, I2C_SR_MBB_M))
        ;
    while (!I2C_REG_GET_FIELD(channel, SR, I2C_SR_MAAS_M | I2C_SR_MINT_M))
        ;
    I2C_REG_SET_FIELD(channel, SR, I2C_SR_MINT_M);
    if (I2C_REG_GET_FIELD(channel, SR, I2C_SR_SRW_M))
    {
        I2C_REG_SET_FIELD(channel, CR, I2C_CR_MTX_M);

        while (I2C_REG_GET_FIELD(channel, CR, I2C_CR_MSTA_M) || (!I2C_REG_GET_FIELD(channel, CR, I2C_CR_MTX_M)))
            ;
    }
    else
    {
        I2C_REG_CLR_FIELD(channel, CR, I2C_CR_MTX_M);
        volatile uint32_t mtfclr = I2C_REG_GET(channel, DR);
        while (I2C_REG_GET_FIELD(channel, CR, I2C_CR_MTX_M) || (!I2C_REG_GET_FIELD(channel, SR, I2C_SR_MBB_M)))
            ;
    }

    return I2C_SUCCESS;
}

int32_t I2C_SlaveSendData(I2cChannel channel, const uint8_t* data, uint32_t length)
{
    if (channel >= I2C_NUM)
        return I2C_PARAM_INCORRECT;

    if (I2C_REG_GET_FIELD(channel, CR, I2C_CR_MIEN_M))
    {
        return I2C_FAILED;
    }
    do
    {
        I2C_REG_SET(channel, DR, *data);
        data++;
        while (!I2C_REG_GET_FIELD(channel, SR, I2C_SR_MTF_M))
            ;
        I2C_REG_SET_FIELD(channel, SR, I2C_SR_MINT_M);
        length--;
    } while (I2C_REG_GET_FIELD(channel, SR, I2C_SR_MBB_M) && I2C_REG_GET_FIELD(channel, CR, I2C_CR_MTX_M)
             && !I2C_REG_GET_FIELD(channel, SR, I2C_SR_RACK_M) && length);

    I2C_REG_CLR_FIELD(channel, CR, I2C_CR_MTX_M);
    length = I2C_REG_GET(channel, DR);

    return I2C_SUCCESS;
}

int32_t I2C_SlaveRecvData(I2cChannel channel, uint8_t* data, uint32_t length)
{
    if (channel >= I2C_NUM)
        return I2C_PARAM_INCORRECT;

    if (I2C_REG_GET_FIELD(channel, CR, I2C_CR_MIEN_M))
    {
        return I2C_FAILED;
    }

    if (!I2C_REG_GET_FIELD(channel, CR, I2C_CR_MSTA_M))
    {
        do
        {
            if (I2C_REG_GET_FIELD(channel, SR, I2C_SR_MTF_M))
            {
                I2C_REG_SET_FIELD(channel, SR, I2C_SR_MINT_M);
                *data = I2C_REG_GET(channel, DR);
                data++;
                length--;
            }
        } while (I2C_REG_GET_FIELD(channel, SR, I2C_SR_MBB_M) && !I2C_REG_GET_FIELD(channel, CR, I2C_CR_MTX_M)
                 && length);
    }

    return I2C_SUCCESS;
}

int32_t I2C_DMASendData(I2cChannel channel, const uint8_t* data, uint32_t length, CompleteCallback callback)
{
    uint8_t dma_channel;
    DMAConfig config = {0};

    if (channel >= I2C_NUM)
        return I2C_PARAM_INCORRECT;

    dma_channel = DMA_GetFreeChannel();

    if (dma_channel < DMAC_MAX_CHANNEL)
    {
        config.addr_src = (uint32_t)data;
        config.addr_dst = (uint32_t)&I2Cx(channel)->DR;

        config.chctrl = (DMA_CHCTRL_PRIORITY_0 | DMA_CHCTRL_WIDTH_8BIT | DMA_CHCTRL_FLOW_M2P | DMA_CHCTRL_INC_SRC);

        if (channel == I2C_CH0)
        {
            config.chctrl |= DMA_CHCTRL_DSTPER_I2C0;
        }
        else
        {
            config.chctrl |= DMA_CHCTRL_DSTPER_I2C1;
        }
        config.chctrl |= DMA_CHCTRL_TRANSFER_SIZE(length);

        config.cmpltcb = callback;
        config.irqmask = DMA_INT_TC_CHALL|DMA_INT_BE_CHALL|DMA_INT_HF_CHALL;

        DMA_Init(&config);

        I2C_REG_SET_FIELD(channel, CR, I2C_CR_MDEN_M);
        if (DMA_SetChannelConfig(dma_channel, &config) == DMA_SUCCESS)
        {
            DMA_TransferStart(dma_channel);
            return I2C_SUCCESS;
        }
        else
        {
            return I2C_FAILED;
        }
    }
    else
    {
        return I2C_FAILED;
    }
}

int32_t I2C_DMARecvData(I2cChannel channel, uint8_t* data, uint32_t length, CompleteCallback callback)
{
    uint8_t dma_channel;
    DMAConfig config = {0};

    if (channel >= I2C_NUM)
        return I2C_PARAM_INCORRECT;

    dma_channel = DMA_GetFreeChannel();
    if (dma_channel < DMAC_MAX_CHANNEL)
    {
        config.addr_src = (uint32_t)&I2Cx(channel)->DR;
        config.addr_dst = (uint32_t)data;

        config.chctrl = (DMA_CHCTRL_PRIORITY_0 | DMA_CHCTRL_WIDTH_8BIT | DMA_CHCTRL_FLOW_P2M | DMA_CHCTRL_INC_DST);

        if (channel == I2C_CH0)
        {
            config.chctrl |= DMA_CHCTRL_SRCPER_I2C0;
        }
        else
        {
            config.chctrl |= DMA_CHCTRL_SRCPER_I2C1;
        }
        config.chctrl |= DMA_CHCTRL_TRANSFER_SIZE(length);

        config.cmpltcb = callback;

        if (I2C_REG_GET_FIELD(channel, CR, I2C_CR_MSTA_M))
        {
            while (!I2C_REG_GET_FIELD(channel, SR, I2C_SR_MTF_M))
                ;
        }

        config.irqmask = DMA_INT_TC_CHALL|DMA_INT_BE_CHALL|DMA_INT_HF_CHALL;
        DMA_Init(&config);
        if (DMA_SetChannelConfig(dma_channel, &config) == DMA_SUCCESS)
        {
            I2C_REG_SET_FIELD(channel, CR, I2C_CR_MDEN_M);
            DMA_TransferStart(dma_channel);
            return I2C_SUCCESS;
        }
        else
        {
            return I2C_FAILED;
        }
    }
    else
    {
        return I2C_FAILED;
    }
}

/** @} */

