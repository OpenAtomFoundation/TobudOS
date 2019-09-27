#include "sys.h"
#include "usart.h"

/*********************************************************************************
			  ___   _     _____  _____  _   _  _____  _____  _   __
			 / _ \ | |   |_   _||  ___|| \ | ||_   _||  ___|| | / /
			/ /_\ \| |     | |  | |__  |  \| |  | |  | |__  | |/ /
			|  _  || |     | |  |  __| | . ` |  | |  |  __| |    \
			| | | || |_____| |_ | |___ | |\  |  | |  | |___ | |\  \
			\_| |_/\_____/\___/ \____/ \_| \_/  \_/  \____/ \_| \_/

 *	******************************************************************************
 *	本程序只供学习使用，未经作者许可，不得用于其它任何用途
 *	ALIENTEK W601开发板
 *	串口0初始化
 *	正点原子@ALIENTEK
 *	技术论坛:www.openedv.com
 *	创建日期:2019/7/10
 *	版本：V1.0
 *	版权所有，盗版必究。
 *	Copyright(C) 广州市星翼电子科技有限公司 2019-2029
 *	All rights reserved
 *	******************************************************************************
 *	修改说明
 *	******************************************************************************/


//////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;
};

FILE __stdout;
/**
 * @brief	定义_sys_exit()以避免使用半主机模式
 *
 * @param	void
 *
 * @return  void
 */
void _sys_exit(int x)
{
    x = x;
}
/**
 * @brief	重定义fputc函数
 *
 * @param	ch		输出字符量
 * @param	f		文件指针
 *
 * @return  void
 */
int fputc(int ch, FILE *f)
{
    while(tls_reg_read32(HR_UART0_FIFO_STATUS) & 0x3F);	//等待发送完毕

    tls_reg_write32(HR_UART0_TX_WIN, (u8) ch);
    return ch;
}
#endif

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA = 0;     //接收状态标记


/**
 * @brief	初始化串口0函数
 *
 * @param	bound	串口波特率
 *
 * @return  void
 */
void uart_init(u32 bound)
{
    u32 bd;
    u32 apbclk;
    tls_sys_clk sysclk;

    /* 1.配置IO */
    wm_uart0_tx_config(WM_IO_PA_04);
    wm_uart0_rx_config(WM_IO_PA_05);

    /* 2.波特率设置:
    	ubdiv = (apbclk / (16 * bound) - 1)
    	ubdiv_frac = ((apbclk % (bound * 16)) / (bound)) */
    tls_sys_clk_get(&sysclk);
    apbclk = sysclk.apbclk * 1000000;
    bd = (apbclk / (16 * bound) - 1) | (((apbclk % (bound * 16)) / (bound)) << 16);
    tls_reg_write32(HR_UART0_BAUD_RATE_CTRL, bd);

    /* 2.串口参数设置：8位数据位/1位停止位/无奇偶校验位/发送使能/接收使能 */
    tls_reg_write32(HR_UART0_LINE_CTRL, ULCON_WL8 | ULCON_TX_EN | ULCON_RX_EN);
    /* 3.硬件流控关闭 */
    tls_reg_write32(HR_UART0_FLOW_CTRL, 0);
    /* 3.不使能DMA */
    tls_reg_write32(HR_UART0_DMA_CTRL, 0);
    /* 4.FIFO触发设置：1个字节 */
    tls_reg_write32(HR_UART0_FIFO_CTRL, 0);
    /* 5.开启RX中断:接收触发中断和接收超时中断*/
    tls_reg_write32(HR_UART0_INT_MASK, 0xFF & (~(UIS_RX_FIFO | UIS_RX_FIFO_TIMEOUT)));

    /* 6.串口接收中断配置 */
    NVIC_ClearPendingIRQ(UART0_IRQn);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = UART0_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 7;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

/**
 * @brief	串口0中断服务程序
 *
 * @param   void
 *
 * @return  void
 */
void UART0_IRQHandler(void)
{
    u8 res;

    if(tls_reg_read32(HR_UART0_INT_SRC) & UIS_RX_FIFO)	//接收到数据
    {
        res = (u8)tls_reg_read32(HR_UART0_RX_WIN);

        if((USART_RX_STA & 0x8000) == 0) //接收未完成
        {
            if(USART_RX_STA & 0x4000) //接收到了0x0d
            {
                if(res != 0x0a)USART_RX_STA = 0; //接收错误,重新开始

                else USART_RX_STA |= 0x8000;	//接收完成了
            }

            else   //还没收到0X0D
            {
                if(res == 0x0d)USART_RX_STA |= 0x4000;

                else
                {
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = res;
                    USART_RX_STA++;

                    if(USART_RX_STA > (USART_REC_LEN - 1))USART_RX_STA = 0; //接收数据错误,重新开始接收
                }
            }
        }

        tls_reg_write32(HR_UART0_INT_SRC, UIS_RX_FIFO);		//清除状态标志位
    }

    if(tls_reg_read32(HR_UART0_INT_SRC) & UIS_RX_FIFO_TIMEOUT)	//接收到数据
    {
        tls_reg_write32(HR_UART0_INT_SRC, UIS_RX_FIFO_TIMEOUT);		//清除状态标志位
    }
}
#endif





