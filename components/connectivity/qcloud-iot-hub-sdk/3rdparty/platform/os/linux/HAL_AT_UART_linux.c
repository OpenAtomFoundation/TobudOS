/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

#ifdef AT_TCP_ENABLED

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include "at_uart_hal.h"

#ifdef AT_UART_RECV_IRQ
#include "at_client.h"
#include "utils_ringbuff.h"
static void at_uart_irq_recv(void *userContex);
#endif

#define AT_UART_LINUX_DEV "/dev/ttyUSB0"

static uart_dev_t sg_at_uart = {
    .fd     = -1,
    .state  = eUNUSED,
    .config = {BAUDRATE_115200, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED, MODE_TX_RX}};

int HAL_AT_Uart_Init(void)
{
    struct termios t_opt;
    speed_t        baud;

    if (sg_at_uart.state == eOPENED) {
        Log_w("%s already opened", AT_UART_LINUX_DEV);
        return QCLOUD_ERR_FAILURE;
    }

    if ((sg_at_uart.fd = open(AT_UART_LINUX_DEV, O_RDWR | O_NOCTTY | O_NDELAY)) == -1) {
        Log_e("open at uart failed\r\n");
        return QCLOUD_ERR_FAILURE;
    }

    switch (sg_at_uart.config.baud_rate) {
        case BAUDRATE_9600:
            baud = B9600;
            break;
        case BAUDRATE_115200:
            baud = B115200;
            break;
        case BAUDRATE_921600:
            baud = B921600;
            break;
        default:
            baud = B115200;
            break;
    }

    /* set the serial port parameters */
    fcntl(sg_at_uart.fd, F_SETFL, 0);

    if (0 != tcgetattr(sg_at_uart.fd, &t_opt)) {
        return QCLOUD_ERR_FAILURE;
    }

    /* set the input and output baudrate*/
    if (0 != cfsetispeed(&t_opt, baud)) {
        return QCLOUD_ERR_FAILURE;
    }

    if (0 != cfsetospeed(&t_opt, baud)) {
        return QCLOUD_ERR_FAILURE;
    }

    /* 8N1, flow control, etc. */
    t_opt.c_cflag |= (CLOCAL | CREAD);
    if (sg_at_uart.config.parity == NO_PARITY) {
        t_opt.c_cflag &= ~PARENB;
    }
    if (sg_at_uart.config.stop_bits == STOP_BITS_1) {
        t_opt.c_cflag &= ~CSTOPB;
    } else {
        t_opt.c_cflag |= CSTOPB;
    }
    t_opt.c_cflag &= ~CSIZE;
    switch (sg_at_uart.config.data_width) {
        case DATA_WIDTH_5BIT:
            t_opt.c_cflag |= CS5;
            break;
        case DATA_WIDTH_6BIT:
            t_opt.c_cflag |= CS6;
            break;
        case DATA_WIDTH_7BIT:
            t_opt.c_cflag |= CS7;
            break;
        case DATA_WIDTH_8BIT:
            t_opt.c_cflag |= CS8;
            break;
        default:
            t_opt.c_cflag |= CS8;
            break;
    }
    t_opt.c_lflag &= ~(ECHO | ECHOE | ISIG | ICANON);

    /**
     * AT is going to use a binary protocol, so make sure to
     * turn off any CR/LF translation and the like.
     */
    t_opt.c_iflag &= ~(IXON | IXOFF | IXANY | INLCR | ICRNL);

    t_opt.c_oflag &= ~OPOST;
    t_opt.c_cc[VMIN]  = 0;
    t_opt.c_cc[VTIME] = 5;
    if (0 != tcsetattr(sg_at_uart.fd, TCSANOW, &t_opt)) {
        return QCLOUD_ERR_FAILURE;
    }

    sg_at_uart.state = eOPENED;

    /* clear uart buffer */
    // Log_e("open at uart 1\r\n");
    // read_and_discard_all_data(sg_at_uart.fd);
    // Log_e("open at uart 2\r\n");

#if defined(AT_UART_RECV_IRQ) && defined(MULTITHREAD_ENABLED)
    // START THERAD
    int          result;
    ThreadParams thread_params = {0};
    thread_params.thread_func  = at_uart_irq_recv;
    thread_params.thread_name  = "at_uart_isr_thread";
    thread_params.user_arg     = NULL;
    thread_params.stack_size   = 6144;
    thread_params.priority     = 0;

    result = HAL_ThreadCreate(&thread_params);
    if (QCLOUD_RET_SUCCESS == result) {
        Log_d("create uart_irq_recv thread success!");
    } else {
        Log_e("create uart_irq_recv thread fail!");
    }
#endif

    return QCLOUD_RET_SUCCESS;
}

int HAL_AT_Uart_Deinit(void)
{
    if (sg_at_uart.state == eOPENED) {
        close(sg_at_uart.fd);
        sg_at_uart.state = eCLOSED;
    }

    return QCLOUD_RET_SUCCESS;
}

int HAL_AT_Uart_Send(void *data, uint32_t size)
{
    uint32_t leftlen = size;
    int      ret     = QCLOUD_RET_SUCCESS;

    if (sg_at_uart.state == eOPENED) {
        while (leftlen > 0) {
            ret = write(sg_at_uart.fd, (char *)data + size - leftlen, leftlen);
            if (ret == -1) {
                Log_e("write uart fd failed on error: %d.\r\n", errno);
                ret = QCLOUD_ERR_FAILURE;
                break;
            }
            leftlen -= ret;
        }
    } else {
        Log_e("AT Uart uninitialized");
        ret = QCLOUD_ERR_FAILURE;
    }

    return (ret == QCLOUD_ERR_FAILURE) ? QCLOUD_ERR_FAILURE : size;
}

int HAL_AT_Uart_Recv(void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    int readlen;
    int ret;

    if (sg_at_uart.state == eOPENED) {
        if ((readlen = read(sg_at_uart.fd, data, expect_size)) == -1) {
            ret = QCLOUD_ERR_FAILURE;
        }

        if (recv_size) {
            *recv_size = readlen;
        }

    } else {
        Log_e("AT Uart uninitialized");
        ret = QCLOUD_ERR_FAILURE;
    }

    return ret;
}

// simulate IRQ
#ifdef AT_UART_RECV_IRQ
extern void at_client_uart_rx_isr_cb(uint8_t *pdata, uint8_t len);
static void at_uart_irq_recv(void *userContex)
{
    uint8_t data[64];
    int     readlen;
    while (1) {
        if ((readlen = read(sg_at_uart.fd, data, sizeof(data))) == -1) {
            continue;
        }
        at_client_uart_rx_isr_cb(data, readlen);  // push
    }
}
#endif

#endif