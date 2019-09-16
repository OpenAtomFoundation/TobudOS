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
 * @brief UART模块接口实现文件。
 * @file n32g020xx_uart.c
 * @author Nations Solution Team
 * @version v1.0.1
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "n32g020xx_uart.h"

#include "n32g020xx_cgu.h"
#include "n32g020xx_common.h"
#include "n32g020xx_iom.h"
#include "n32g020xx.h"
#include "n32g020xx_boot.h"

/*
 * 每个UART通道的相关常量信息
 */
typedef struct
{
    uint64_t cgum;     // UART通道对应的CGU PCLK模块号
    UARTTypeDef* regs; // UART通道对应的寄存器组的起始地址
    IRQn_Type irqn;    // UART通道对应的中断号
} UartChannelInfo;

#define UART_CHANNEL_NUMBER 3

static const UartChannelInfo s_channel_infos[UART_CHANNEL_NUMBER] =
    {
        {
            .cgum = CGUM_UART0_PCLK,
            .regs = UART0,
            .irqn = Uart0_Exception_IrqNum,
        },
        {
            .cgum = CGUM_UART1_PCLK,
            .regs = UART1,
            .irqn = Uart1_Exception_IrqNum,
        },
        {
            .cgum = CGUM_UART2_PCLK,
            .regs = UART2,
            .irqn = Uart2_Exception_IrqNum,
        }};

// 用于保存各个通道的回调函数地址
static UART_Callback s_callback_funcs[UART_CHANNEL_NUMBER] = {0, 0, 0};

static void UART_IRQHandler(UARTTypeDef* uart_regs, UART_Callback callback)
{
    uint32_t isr_val = GET_REG_VAL(uart_regs->ISR);
    SET_REG_VAL(uart_regs->ISR, BIT0 | BIT2); // 清中断状态寄存器

    if (!callback)
    {
        return;
    }

    int32_t val;
    if (isr_val & BIT0) // 奇偶校验错误
    {
        val = UART_STATE_PARITY_ERROR;
    }
    else if (isr_val & BIT2) // FIFO溢出
    {
        val = UART_STATE_FIFO_OVERFLOW;
    }
    else if (isr_val & BIT5) // FIFO非空
    {
        val = GET_REG_VAL(uart_regs->DR);
    }
    else
    {
        return;
    }

    callback(val);
}

void UART0_IRQHandler(void)
{ 
    UART_IRQHandler(UART0, s_callback_funcs[0]);
}

void UART1_IRQHandler(void)
{
    UART_IRQHandler(UART1, s_callback_funcs[1]);
}

void UART2_IRQHandler(void)
{
    UART_IRQHandler(UART2, s_callback_funcs[2]);
}

static int32_t UART_InitIOM(UartChannel channel, UartIO io)
{
    switch (channel)
    {
    case UART_CH0:
        switch (io)
        {
        case UART_CH0_GPIO0_GPIO1:
            IOM_ConfigGpio0Uart0Tx();
            IOM_ConfigGpio1Uart0Rx();
            break;
        case UART_CH0_GPIO22_GPIO23:
            IOM_ConfigGpio22Uart0Tx();
            IOM_ConfigGpio23Uart0Rx();
            break;
        default:
            return UART_PARAM_INCORRECT;
        }
        break;
    case UART_CH1:
        switch (io)
        {
        case UART_CH1_GPIO2_GPIO3:
            IOM_ConfigGpio2Uart1Tx();
            IOM_ConfigGpio3Uart1Rx();
            break;
        case UART_CH1_GPIO18_GPIO19:
            IOM_ConfigGpio18Uart1Tx();
            IOM_ConfigGpio19Uart1Rx();
            break;
        default:
            return UART_PARAM_INCORRECT;
        }
        break;
    case UART_CH2:
        switch (io)
        {
        case UART_CH2_GPIO4_GPIO5:
            IOM_ConfigGpio4Uart2Tx();
            IOM_ConfigGpio5Uart2Rx();
            break;
        case UART_CH2_GPIO20_GPIO21:
            IOM_ConfigGpio20Uart2Tx();
            IOM_ConfigGpio21Uart2Rx();
            break;
        default:
            return UART_PARAM_INCORRECT;
        }
        break;
    default:
        return UART_PARAM_INCORRECT;
    }
    return UART_SUCCESS;
}

// UART内部的分频时钟源为20M
#define UART_ACTUAL_FREQ_INTERNAL 20000000

/*
 * 自动计算分频和波特率配置寄存器的值
 */
static void UART_CalcBrpAndDiv(uint32_t frequence, uint32_t baudrate, uint32_t* brp, uint32_t* div)
{
    if (frequence == UART_FREQ_INTERNAL)
    {
        // 使用40M实测时钟计算20M时钟的实际频率
        frequence = Boot_APIOSCFreqGet(1) * 500;

        if (frequence < UART_ACTUAL_FREQ_INTERNAL - 2000000
            || frequence > UART_ACTUAL_FREQ_INTERNAL + 2000000)
        {
            // 如果上下偏差超过2M，则不使用实测值
            frequence = UART_ACTUAL_FREQ_INTERNAL;
        }
    }

    /*
     * 分频支持1~128，分频值越大，功耗越低，但分频值越大会使bpr值越小，bpr值越小
     * 越容易通信错误，这里要选一个分频值尽量大，但又使bpr值尽量大于100的值
     * 计算时依赖以下等式：
     * frequence / div == baudrate * brp
     */

    uint32_t _div = 128;
    uint32_t _brp;
    uint32_t temp_brp = (frequence + (baudrate >> 1)) / baudrate;

    for (int i = 7; i >= 0; --i)
    {
        _brp = temp_brp >> i; // _brp = temp_brp / _div
        if (_brp > 100)
        {
            break;
        }
        if (_div == 1)
        {
            break;
        }
        _div >>= 1;
    }

    *brp = _brp;
    *div = _div;
}

int32_t UART_Init(UartChannel channel, const UartConfig* config)
{
    uint64_t cgum;
    UARTTypeDef* uart_regs;
    IRQn_Type irqn;
    switch (channel)
    {
    case UART_CH0:
    case UART_CH1:
    case UART_CH2:
        cgum      = s_channel_infos[(int)channel].cgum;
        uart_regs = s_channel_infos[(int)channel].regs;
        irqn      = s_channel_infos[(int)channel].irqn;
        break;
    default:
        return UART_PARAM_INCORRECT;
    }
    cgum |= CGUM_IOM_PCLK;

    uint32_t brp = 0;
    uint32_t div = 0;
    UART_CalcBrpAndDiv(config->frequence, config->baudrate, &brp, &div);
    if (brp > 0xffff)
    {
        // BPR寄存器只有16位
        return UART_PARAM_INCORRECT;
    }

    if (CGU_SUCCESS
        != CGU_ConfigUartClock(channel,
                               config->frequence == UART_FREQ_INTERNAL ? CLK_SRC_OSC_80M : CLK_SRC_EXTERNAL_12M,
                               div))
    {
        return UART_PARAM_INCORRECT;
    }

    SET_REG_BIT(CGU->CLKSRCFREQCR, BIT4); // enable CRST12MEN
    CGU_EnableModuleFunctions(cgum);
    CLR_REG_BIT(CGU->CLKSRCFREQCR, BIT4); // disable CRST12MEN

    if (UART_SUCCESS != UART_InitIOM(channel, config->io))
    {
        return UART_PARAM_INCORRECT;
    }

    // 配置波特率
    SET_REG_VAL(uart_regs->BRPH, brp >> 8);
    SET_REG_VAL(uart_regs->BRPL, brp & 0xff);

    // 保存回调函数
    s_callback_funcs[channel] = config->callback;

    if (config->callback) // 有回调
    {
        // 使能IRQ中断
        NVIC_EnableIRQ(irqn);
        // 使能UART中断，包括校验错误、FIFO溢出和FIFO非空
        SET_REG_VAL(uart_regs->IER, BIT0 | BIT2 | BIT5);
    }

    // 清中断状态寄存器
    SET_REG_VAL(uart_regs->ISR, 0xff);

    // FLUSH
    SET_REG_VAL(uart_regs->CR, BIT2);

    // 配置UART控制寄存器，配置为禁止DMA，自测模式不使能，发送数据不使能，
    // 及指定的奇偶校验配置
    uint32_t reg_cr = 0;
    switch (config->parity_type)
    {
    case UART_PARITY_EVEN:
        break;
    case UART_PARITY_ODD:
        reg_cr |= BIT0; // bit ODD_EN
        break;
    case UART_PARITY_NONE:
        reg_cr |= BIT3; // bit UART_P0
        break;
    default:
        return UART_PARAM_INCORRECT;
    }
    SET_REG_VAL(uart_regs->CR, reg_cr);

    CLR_REG_BIT(uart_regs->CR, BIT1); // 关闭发送

    return UART_SUCCESS;
}

int32_t UART_SendData(UartChannel channel, const uint8_t* send_buf, uint32_t send_len)
{
    volatile uint32_t timeout;
    UARTTypeDef* uart_regs;
    switch (channel)
    {
    case UART_CH0:
    case UART_CH1:
    case UART_CH2:
        uart_regs = s_channel_infos[(int)channel].regs;
        break;
    default:
        return UART_PARAM_INCORRECT;
    }

    if (!send_buf)
    {
        return UART_PARAM_INCORRECT;
    }

    SET_REG_BIT(uart_regs->CR, BIT1); // 使能发送
    while (send_len)
    {
        timeout = 0xFFFF;
        SET_REG_VAL(uart_regs->DR, (uint32_t)(*send_buf));
        send_buf++;
        send_len--;
        while (!GET_REG_BIT(uart_regs->ISR, BIT1))
        {
            if (!(--timeout))
            {
                SET_REG_BIT(uart_regs->ISR, BIT1); // 清标志
                CLR_REG_BIT(uart_regs->CR, BIT1);  // 关闭发送
                return UART_TIMEOUT;
            }
        }
        SET_REG_BIT(uart_regs->ISR, BIT1); // 清标志
    }
    CLR_REG_BIT(uart_regs->CR, BIT1); // 关闭发送

    return UART_SUCCESS;
}

int32_t UART_ReceiveData(UartChannel channel, uint8_t* recv_buf, uint32_t recv_len)
{
    UARTTypeDef* uart_regs;
    switch (channel)
    {
    case UART_CH0:
    case UART_CH1:
    case UART_CH2:
        uart_regs = s_channel_infos[(int)channel].regs;
        break;
    default:
        return UART_PARAM_INCORRECT;
    }
    if (!recv_buf)
    {
        return UART_PARAM_INCORRECT;
    }

    while (recv_len)
    {
        while (!GET_REG_BIT(uart_regs->ISR, BIT5)) // 等待FIFO非空
            ;
        if (GET_REG_BIT(uart_regs->ISR, BIT0 | BIT2)) // 判断校验错误位和溢出位
        {
            if (GET_REG_BIT(uart_regs->ISR, BIT0))
            {
                // 清校验错误位
                SET_REG_BIT(uart_regs->ISR, BIT0);
                return UART_RECV_PARITY_ERROR;
            }
            if (GET_REG_BIT(uart_regs->ISR, BIT2))
            {
                //清溢出位
                SET_REG_BIT(uart_regs->ISR, BIT2);
                return UART_RECV_FIFO_OVERFLOW;
            }
        }

        *recv_buf = GET_REG_VAL(uart_regs->DR);
        recv_buf++;
        recv_len--;
    }

    return UART_SUCCESS;
}

int32_t UART_Close(UartChannel channel)
{
    uint64_t cgum;
    IRQn_Type irqn;
    switch (channel)
    {
    case UART_CH0:
    case UART_CH1:
    case UART_CH2:
        cgum = s_channel_infos[(int)channel].cgum;
        irqn = s_channel_infos[(int)channel].irqn;
        break;
    default:
        return UART_PARAM_INCORRECT;
    }

    // 清除回调函数
    s_callback_funcs[channel] = 0;
    // 关闭中断使能
    NVIC_DisableIRQ(irqn);
    // 关闭UARTx的PCLK时钟
    CGU_DisableModuleFunctions(cgum);
    return UART_SUCCESS;
}
