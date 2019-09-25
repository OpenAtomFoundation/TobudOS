/**
 * @file    wm_iouart.c
 *
 * @brief   IO uart Driver Module
 *
 * @author  dave
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */

#include <string.h>
//#include "wm_iouart.h"
#include "wm_debug.h"
#include "wm_irq.h"
#include "wm_config.h"
#include "wm_mem.h"
#include "wm_gpio.h"
#include "wm_timer.h"

#if TLS_CONFIG_IOUART
struct tls_io_uart io_uart;
#if !IO_UART_FOR_PRINT
void iouart_timer_cb(void)
{
    static int i = 0;
    int value;
    u16 bit = -1;
    static u8 ch = 0;

    value = tls_reg_read32(HR_TIMER1_CSR);
    value |= 1 << 4;
    tls_reg_write32(HR_TIMER1_CSR, value);

// if(io_uart.ifrx)
    {
        if (0 == io_uart.bitnum)    // 起始位
        {
            io_uart.bit[io_uart.bitcnt++] = tls_gpio_read(IO_UART_RX);
            if (io_uart.bit[0] != 0)
            {
                io_uart.bitcnt = 0;
                tls_gpio_write(IO_UART_TX, 1);
                return;
            }
            if (1 /* IO_UART_ONEBITE_SAMPLE_NUM == io_uart.bitcnt */ )
            {
                bit = io_uart.bit[0] /* | io_uart.bit[1] | io_uart.bit[2] */ ;
            // printf("\nbit
            // cnt=%d,bit=%d,%d,%d,%d\n",io_uart.bitcnt,io_uart.bit[0],io_uart.bit[1],io_uart.bit[2],bit);
                if (bit != 0)
                {
                    printf("\nstart bit err\n");
                // tls_timer_stop();
                // tls_gpio_int_enable(IO_UART_RX,
                // TLS_GPIO_INT_TRIG_LOW_LEVEL);
                    io_uart.bitnum = 0;
                }
                else
                {
                    io_uart.bitnum++;
                    i = 8;
                    tls_gpio_write(IO_UART_TX, 0);
                }
                io_uart.bitcnt = 0;
            }
        }
        else if (io_uart.bitnum >= 1 && io_uart.bitnum <= 8)    // 数据位
        {
            i--;
            if (i <= 0)
            {
                io_uart.bit[io_uart.bitcnt++] = tls_gpio_read(IO_UART_RX);
                if (IO_UART_ONEBITE_SAMPLE_NUM == io_uart.bitcnt)
                {
                    bit = io_uart.bit[0] | io_uart.bit[1] | io_uart.bit[2];
                // printf("\nbit[%d]=[%d]\n",io_uart.bitnum,bit);
                    if (1 == bit)
                    {
                        ch |= (1 << (io_uart.bitnum - 1));
                    }
                    io_uart.bitnum++;
                    i = 6;
                    io_uart.bitcnt = 0;
                }
            }
        }
        else if (9 == io_uart.bitnum)   // 停止位
        {
            i--;
            if (i <= 0)
            {
                io_uart.bit[io_uart.bitcnt++] = tls_gpio_read(IO_UART_RX);
                if (IO_UART_ONEBITE_SAMPLE_NUM == io_uart.bitcnt)
                {
                    bit = io_uart.bit[0] | io_uart.bit[1] | io_uart.bit[2];
                    if (1 == bit)   // 正常的停止位
                    {
#if 0
                        if (CIRC_SPACE
                            (io_uart.recv.head, io_uart.recv.tail,
                             TLS_IO_UART_RX_BUF_SIZE) <= 1)
                        {
                            printf("\nrx buf overrun\n");
                            io_uart.bitnum = 0;
                            io_uart.bitcnt = 0;
                            return;
                        }
#endif
                        tls_gpio_write(IO_UART_TX, 1);
                        io_uart.recv.buf[io_uart.recv.head] = ch;
                        io_uart.recv.head =
                            (io_uart.recv.head + 1) & (TLS_IO_UART_RX_BUF_SIZE -
                                                       1);
                    }
                    else
                    {
                        printf("\nstop bit err\n");
                    }
                    io_uart.bitnum = 0;
                    io_uart.bitcnt = 0;
                    ch = 0;
                    io_uart.ifrx = 0;
                // tls_timer_stop();
                // tls_gpio_int_enable(IO_UART_RX,
                // TLS_GPIO_INT_TRIG_LOW_LEVEL);
                }
            }
        }
    }

}


void iouart_gpio_isr_callback(void *context)
{
    u16 ret;

    ret = tls_get_gpio_int_flag(IO_UART_RX);
    if (ret)
    {
        tls_clr_gpio_int_flag(IO_UART_RX);
        if (0 == io_uart.iftx)
        {
            tls_gpio_int_disable(IO_UART_RX);
            tls_timer_start(io_uart.timercnt);
            io_uart.ifrx = 1;
        }
    }
}
#endif

void iouart_delay(int time)
{
    int value;
#ifndef WM_W600
    tls_reg_write32(HR_TIMER1_CSR, TLS_TIMER_INT_CLR);
    tls_reg_write32(HR_TIMER1_CSR,
                    time << TLS_TIMER_VALUE_S | TLS_TIMER_INT_EN | TLS_TIMER_EN
                    | TLS_TIMER_ONE_TIME);

    while (1)
    {
        value = tls_reg_read32(HR_TIMER1_CSR);
        if (value & TLS_TIMER_INT_CLR)
        {
            tls_reg_write32(HR_TIMER1_CSR, TLS_TIMER_INT_CLR);
            break;
        }
    }
#else
    tls_reg_write32(HR_TIMER0_5_CSR, TLS_TIMER_INT_CLR(1));
    tls_reg_write32(HR_TIMER1_PRD, time);
    tls_reg_write32(HR_TIMER0_5_CSR,
                    TLS_TIMER_INT_EN(1) | TLS_TIMER_EN(1) |
                    TLS_TIMER_ONE_TIME(1));
    while (1)
    {
        value = tls_reg_read32(HR_TIMER0_5_CSR);
        if (value & TLS_TIMER_INT_CLR(1))
        {
            tls_reg_write32(HR_TIMER0_5_CSR, TLS_TIMER_INT_CLR(1));
            break;
        }
    }

#endif
}

void iouart_tx_byte(u8 datatoSend)
{
    u8 i, tmp;
    u32 cpu_sr = 0;

    cpu_sr = tls_os_set_critical(); // 发送一个byte的过程中不能被打断，否则可能会有错误码
/* Start bit */
    tls_gpio_write(TLS_GPIO_TYPE_A, IO_UART_TX, 0);
#if IO_UART_FOR_PRINT
    iouart_delay(io_uart.timercnt - 3);
#else
    iouart_delay(io_uart.timercnt * IO_UART_RATE_MUL);
#endif
    for (i = 0; i < 8; i++)
    {
        tmp = (datatoSend >> i) & 0x01;

        if (tmp == 0)
        {
            tls_gpio_write(TLS_GPIO_TYPE_A, IO_UART_TX, 0);
        }
        else
        {
            tls_gpio_write(TLS_GPIO_TYPE_A, IO_UART_TX, 1);
        }
#if IO_UART_FOR_PRINT
        iouart_delay(io_uart.timercnt - 3);
#else
        iouart_delay(io_uart.timercnt * IO_UART_RATE_MUL);
#endif
    }

/* Stop bit */
    tls_gpio_write(TLS_GPIO_TYPE_A, IO_UART_TX, 1);
#if IO_UART_FOR_PRINT
    iouart_delay(io_uart.timercnt - 3);
#else
    iouart_delay(io_uart.timercnt * IO_UART_RATE_MUL);
#endif

    tls_os_release_critical(cpu_sr);
}

int tls_iouart_init(int bandrate)
{
    char *bufrx, *buftx;

    memset(&io_uart, 0, sizeof(struct tls_io_uart));
#if !IO_UART_FOR_PRINT
    bufrx = tls_mem_alloc(TLS_IO_UART_RX_BUF_SIZE);
    if (!bufrx)
        return WM_FAILED;
    memset(bufrx, 0, TLS_IO_UART_RX_BUF_SIZE);
    io_uart.recv.buf = (u8 *) bufrx;
    io_uart.recv.head = 0;
    io_uart.recv.tail = 0;
#endif

    tls_gpio_cfg(TLS_GPIO_TYPE_A, IO_UART_TX, TLS_GPIO_DIR_OUTPUT,
                 TLS_GPIO_ATTR_FLOATING);
    tls_gpio_write(TLS_GPIO_TYPE_A, IO_UART_TX, 1);

    tls_gpio_cfg(TLS_GPIO_TYPE_A, IO_UART_RX, TLS_GPIO_DIR_INPUT,
                 TLS_GPIO_ATTR_PULLLOW);

//  tls_gpio_isr_register(iouart_gpio_isr_callback,NULL);
//  tls_gpio_int_enable(IO_UART_RX, TLS_GPIO_INT_TRIG_LOW_LEVEL);
    io_uart.timercnt = 1000000 / bandrate;
#if !IO_UART_FOR_PRINT
    io_uart.timercnt = 1000000 / bandrate / IO_UART_RATE_MUL;

    tls_timer_irq_register(iouart_timer_cb);
    tls_timer_start(io_uart.timercnt);
#endif
}

#if !IO_UART_FOR_PRINT
int tls_iouart_destroy(void)
{
    tls_gpio_int_disable(IO_UART_RX);
    tls_timer_stop();
    tls_mem_free(io_uart.recv.buf);
}


int tls_iouart_read(u8 * buf, int bufsize)
{
    int data_cnt, buflen, bufcopylen;

    if (NULL == buf)
        return WM_FAILED;

    data_cnt =
        CIRC_CNT(io_uart.recv.head, io_uart.recv.tail, TLS_IO_UART_RX_BUF_SIZE);
// TLS_DBGPRT_INFO("\ndata cnt=%d\n",data_cnt);
    if (data_cnt >= bufsize)
    {
        buflen = bufsize;
    }
    else
    {
        buflen = data_cnt;
    }
    if ((io_uart.recv.tail + buflen) > TLS_IO_UART_RX_BUF_SIZE)
    {
        bufcopylen = (TLS_IO_UART_RX_BUF_SIZE - io_uart.recv.tail);
        MEMCPY(buf, io_uart.recv.buf + io_uart.recv.tail, bufcopylen);
        MEMCPY(buf + bufcopylen, io_uart.recv.buf, buflen - bufcopylen);
    }
    else
    {
        MEMCPY(buf, io_uart.recv.buf + io_uart.recv.tail, buflen);
    }
    io_uart.recv.tail =
        (io_uart.recv.tail + buflen) & (TLS_IO_UART_RX_BUF_SIZE - 1);
    return buflen;
}


int tls_iouart_write(u8 * buf, int bufsize)
{


    if (NULL == buf || bufsize <= 0 || 1 == io_uart.ifrx)
        return WM_FAILED;

    io_uart.iftx = 1;

    while (bufsize)
    {
        iouart_tx_byte(*buf);
        bufsize--;
        buf++;
    }

    io_uart.iftx = 0;
#if !IO_UART_FOR_PRINT
    tls_timer_start(io_uart.timercnt);
#endif
    return WM_SUCCESS;
}
#endif
#endif
//TLS_CONFIG_IOUART
