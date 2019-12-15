/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "syscfg/syscfg.h"

#if MYNEWT_VAL(BTTESTER_PIPE_UART)

#include "os/mynewt.h"
#include "uart/uart.h"

#include "bttester_pipe.h"

static u8_t *recv_buf;
static size_t recv_buf_len;
static bttester_pipe_recv_cb app_cb;
static size_t recv_off;

struct uart_pipe_ring {
    uint8_t head;
    uint8_t tail;
    uint16_t size;
    uint8_t *buf;
};

static struct uart_dev *uart_dev;
static struct uart_pipe_ring cr_tx;
static uint8_t cr_tx_buf[MYNEWT_VAL(BTTESTER_BTP_DATA_SIZE_MAX)];
typedef void (*console_write_char)(struct uart_dev*, uint8_t);
static console_write_char write_char_cb;

static struct uart_pipe_ring cr_rx;
static uint8_t cr_rx_buf[MYNEWT_VAL(BTTESTER_BTP_DATA_SIZE_MAX)];
static volatile bool uart_console_rx_stalled;

struct os_event rx_ev;

static inline int
inc_and_wrap(int i, int max)
{
    return (i + 1) & (max - 1);
}

static void
uart_pipe_ring_add_char(struct uart_pipe_ring *cr, char ch)
{
    cr->buf[cr->head] = ch;
    cr->head = inc_and_wrap(cr->head, cr->size);
}

static uint8_t
uart_pipe_ring_pull_char(struct uart_pipe_ring *cr)
{
    uint8_t ch;

    ch = cr->buf[cr->tail];
    cr->tail = inc_and_wrap(cr->tail, cr->size);
    return ch;
}

static bool
uart_pipe_ring_is_full(const struct uart_pipe_ring *cr)
{
    return inc_and_wrap(cr->head, cr->size) == cr->tail;
}

static bool
uart_pipe_ring_is_empty(const struct uart_pipe_ring *cr)
{
    return cr->head == cr->tail;
}

static void
uart_pipe_queue_char(struct uart_dev *uart_dev, uint8_t ch)
{
    int sr;

    if ((uart_dev->ud_dev.od_flags & OS_DEV_F_STATUS_OPEN) == 0) {
        return;
    }

    OS_ENTER_CRITICAL(sr);
    while (uart_pipe_ring_is_full(&cr_tx)) {
        /* TX needs to drain */
        uart_start_tx(uart_dev);
        OS_EXIT_CRITICAL(sr);
        if (os_started()) {
            os_time_delay(1);
        }
        OS_ENTER_CRITICAL(sr);
    }
    uart_pipe_ring_add_char(&cr_tx, ch);
    OS_EXIT_CRITICAL(sr);
}

/*
 * Interrupts disabled when console_tx_char/console_rx_char are called.
 * Characters sent only in blocking mode.
 */
static int
uart_console_tx_char(void *arg)
{
    if (uart_pipe_ring_is_empty(&cr_tx)) {
        return -1;
    }
    return uart_pipe_ring_pull_char(&cr_tx);
}

/*
 * Interrupts disabled when console_tx_char/console_rx_char are called.
 */
static int
uart_console_rx_char(void *arg, uint8_t byte)
{
    if (uart_pipe_ring_is_full(&cr_rx)) {
        uart_console_rx_stalled = true;
        return -1;
    }

    uart_pipe_ring_add_char(&cr_rx, byte);

    if (!rx_ev.ev_queued) {
        os_eventq_put(os_eventq_dflt_get(), &rx_ev);
    }

    return 0;
}

static int
uart_pipe_handle_char(int key)
{
    recv_buf[recv_off] = (u8_t) key;
    recv_off++;

    return 0;
}

static void
uart_console_rx_char_event(struct os_event *ev)
{
    static int b = -1;
    int sr;
    int ret;

    /* We may have unhandled character - try it first */
    if (b >= 0) {
        ret = uart_pipe_handle_char(b);
        if (ret < 0) {
            return;
        }
    }

    while (!uart_pipe_ring_is_empty(&cr_rx)) {
        OS_ENTER_CRITICAL(sr);
        b = uart_pipe_ring_pull_char(&cr_rx);
        OS_EXIT_CRITICAL(sr);

        /* If UART RX was stalled due to a full receive buffer, restart RX now
         * that we have removed a byte from the buffer.
         */
        if (uart_console_rx_stalled) {
            uart_console_rx_stalled = false;
            uart_start_rx(uart_dev);
        }

        ret = uart_pipe_handle_char(b);
        if (ret < 0) {
            return;
        }
    }

    /*
     * Call application callback with received data. Application
     * may provide new buffer or alter data offset.
     */
    recv_buf = app_cb(recv_buf, &recv_off);

    b = -1;
}

int
bttester_pipe_send(const u8_t *data, int len)
{
    int i;

    /* Assure that there is a write cb installed; this enables to debug
     * code that is faulting before the console was initialized.
     */
    if (!write_char_cb) {
        return -1;
    }

    for (i = 0; i < len; ++i) {
        write_char_cb(uart_dev, data[i]);
    }

    uart_start_tx(uart_dev);

    return 0;
}

int
bttester_pipe_init(void)
{
    struct uart_conf uc = {
            .uc_speed = MYNEWT_VAL(CONSOLE_UART_BAUD),
            .uc_databits = 8,
            .uc_stopbits = 1,
            .uc_parity = UART_PARITY_NONE,
            .uc_flow_ctl = MYNEWT_VAL(CONSOLE_UART_FLOW_CONTROL),
            .uc_tx_char = uart_console_tx_char,
            .uc_rx_char = uart_console_rx_char,
    };

    cr_tx.size = sizeof(cr_tx_buf);
    cr_tx.buf = cr_tx_buf;
    write_char_cb = uart_pipe_queue_char;

    cr_rx.size = sizeof(cr_rx_buf);
    cr_rx.buf = cr_rx_buf;

    rx_ev.ev_cb = uart_console_rx_char_event;

    if (!uart_dev) {
        uart_dev = (struct uart_dev *)os_dev_open(MYNEWT_VAL(CONSOLE_UART_DEV),
                                                  OS_TIMEOUT_NEVER, &uc);
        if (!uart_dev) {
            return -1;
        }
    }
    return 0;
}

void
bttester_pipe_register(u8_t *buf, size_t len, bttester_pipe_recv_cb cb)
{
	recv_buf = buf;
	recv_buf_len = len;
	app_cb = cb;
}
#endif /* MYNEWT_VAL(BTTESTER_PIPE_UART) */
