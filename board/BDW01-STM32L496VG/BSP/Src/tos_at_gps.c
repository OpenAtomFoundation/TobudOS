/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#include "tos_at_gps.h"

__STATIC__ at_agent_t at_agent;

__STATIC__ k_stack_t at_parser_task_stack[AT_PARSER_TASK_STACK_SIZE];


__STATIC__ int at_getchar(void)
{
    uint8_t chr;
    k_err_t err;

    if (tos_sem_pend(&AT_AGENT->at_rx_sem, TOS_TIME_FOREVER) != K_ERR_NONE) {
        return -1;
    }

    err = tos_chr_fifo_pop(&AT_AGENT->at_rx_fifo, &chr);

    return err == K_ERR_NONE ? chr : -1;
}

__STATIC__ at_event_t *at_event_do_get(char *buffer, size_t buffer_len)
{
    int i = 0;
    at_event_t *event_table = K_NULL, *event = K_NULL;
    size_t event_table_size = 0, event_len;

    event_table         = AT_AGENT->event_table;
    event_table_size    = AT_AGENT->event_table_size;

    for (i = 0; i < event_table_size; ++i) {
        event = &event_table[i];
        event_len = strlen(event->event_header);

        if (buffer_len < event_len) {
            continue;
        }

        if (strncmp(event->event_header, buffer, event_len) == 0) {
            return event;
        }
    }

    return K_NULL;
}

__STATIC__ at_event_t *at_get_event(void)
{
    char *buffer;
    size_t buffer_len;

    buffer = AT_AGENT->cmd_buffer;
    buffer_len = strlen(AT_AGENT->cmd_buffer);

    return at_event_do_get(buffer, buffer_len);
}

__STATIC__ int at_readline(void)
{
    int chr, last_chr = 0;
	at_event_t *at_event = K_NULL;
    char *buf = AT_AGENT->cmd_buffer;

    while (K_TRUE) {
        if (buf - AT_AGENT->cmd_buffer >= (AT_AGENT->cmd_buffer_size - 1)) {
            return -1;
        }

        chr = at_getchar();
        if (chr < 0) {
            return -1;
        }

		if (chr == '\n' && last_chr == '\r') {
            *--buf = '\0';
//			tos_kprintln(AT_AGENT->cmd_buffer);

			at_event = at_get_event();
            if (at_event && at_event->event_callback) {
                at_event->event_callback(AT_AGENT->cmd_buffer);
            }
            return 0;
        } else if (chr == '\n') {
            *buf = '\0';
            return 0;
        }

        *buf++      = chr;
        last_chr    = chr;
    }
}


__STATIC__ void at_parser(void *arg)
{
    while (K_TRUE) {
        at_readline();
    }
}

__STATIC__ at_data_channel_t *at_channel_get(int channel_id, int is_alloc)
{
    /*
        if is_alloc is K_TRUE, means we are allocating a channel with certain id,
            data_channel[channel_id] must be free if return none K_NULL.
        otherwise if is_alloc is K_FALSE, means we are trying to get a channel with
            certain id, data_channel[channel_id] must be not free if return none K_NULL.
     */
    at_data_channel_t *data_channel = K_NULL;

    if (channel_id < 0 || channel_id >= AT_DATA_CHANNEL_NUM) {
        return K_NULL;
    }

    data_channel = &AT_AGENT->data_channel[channel_id];

    if (is_alloc && data_channel->is_free) {
        return data_channel;
    }

    if (!is_alloc && !data_channel->is_free) {
        return data_channel;
    }

    return K_NULL;
}


__API__ int tos_at_gps_channel_read(int channel_id, uint8_t *buffer, size_t buffer_len)
{
    int read_len;
    size_t total_read_len = 0;
    at_data_channel_t *data_channel = K_NULL;

//	printf("===\n");

    data_channel = at_channel_get(channel_id, K_FALSE);
    if (!data_channel || data_channel->status == AT_CHANNEL_STATUS_BROKEN) {
        return -1;
    }

//   printf("==9\n");
    if (tos_mutex_pend(&data_channel->rx_lock) != K_ERR_NONE) {
        return total_read_len;
    }
//	printf("==8\n");
    read_len = tos_chr_fifo_pop_stream(&data_channel->rx_fifo, buffer, buffer_len);

    tos_mutex_post(&data_channel->rx_lock);
		
    total_read_len += read_len;
	return total_read_len;
    
}


__API__ int tos_at_gps_channel_write(int channel_id, uint8_t *buffer, size_t buffer_len)
{
    int ret;
    at_data_channel_t *data_channel = K_NULL;
//	printf("++\n");
    data_channel = at_channel_get(channel_id, K_FALSE);
    if (!data_channel) {
//		printf("+++\n");
        return -1;
    }

    if (tos_mutex_pend(&data_channel->rx_lock) != K_ERR_NONE) {
        return -1;
    }
//	printf("++9\n");
	tos_chr_fifo_flush(&data_channel->rx_fifo);
    ret = tos_chr_fifo_push_stream(&data_channel->rx_fifo, buffer, buffer_len);

    tos_mutex_post(&data_channel->rx_lock);

    return ret;
}

__STATIC_INLINE__ int at_channel_construct(at_data_channel_t *data_channel)
{
    uint8_t *fifo_buffer = K_NULL;

    fifo_buffer = tos_mmheap_alloc(AT_DATA_CHANNEL_FIFO_BUFFER_SIZE);
    if (!fifo_buffer) {
        return -1;
    }

    if (tos_mutex_create(&data_channel->rx_lock) != K_ERR_NONE) {
        goto errout;
    }

    data_channel->rx_fifo_buffer = fifo_buffer;
    tos_chr_fifo_create(&data_channel->rx_fifo, fifo_buffer, AT_DATA_CHANNEL_FIFO_BUFFER_SIZE);

    data_channel->is_free   = K_FALSE;
    data_channel->status    = AT_CHANNEL_STATUS_WORKING;

    return 0;

errout:
    tos_mmheap_free(fifo_buffer);
    return -1;
}


__API__ int tos_at_gps_channel_alloc(void)
{
    int id = 0;
    at_data_channel_t *data_channel = K_NULL;

    for (id = 0; id < AT_DATA_CHANNEL_NUM; ++id) {
        data_channel = &AT_AGENT->data_channel[id];
        if (data_channel->is_free) {
            break;
        }
    }

    if (id == AT_DATA_CHANNEL_NUM || !data_channel) {
        return -1;
    }

    if (at_channel_construct(data_channel) != 0) {
        return -1;
    }

    return id;
}

__API__ int tos_at_gps_channel_free(int channel_id)
{
    at_data_channel_t *data_channel = K_NULL;

    data_channel = at_channel_get(channel_id, K_FALSE);
    if (!data_channel) {
        return -1;
    }

    tos_mutex_destroy(&data_channel->rx_lock);

    tos_mmheap_free(data_channel->rx_fifo_buffer);
    tos_chr_fifo_destroy(&data_channel->rx_fifo);

    memset(data_channel, 0, sizeof(at_data_channel_t));

    data_channel->is_free   = K_TRUE;
    data_channel->status    = AT_CHANNEL_STATUS_HANGING;

    return 0;
}


__STATIC__ void at_channel_init(void)
{
    int i = 0;

    for (i = 0; i < AT_DATA_CHANNEL_NUM; ++i) {
        memset(&AT_AGENT->data_channel[i], 0, sizeof(at_data_channel_t));
        AT_AGENT->data_channel[i].is_free   = K_TRUE;
        AT_AGENT->data_channel[i].status    = AT_CHANNEL_STATUS_HANGING;
    }

	for (i = 0; i < AT_DATA_CHANNEL_NUM; ++i) {
		tos_at_gps_channel_alloc();
	}
}

__STATIC__ void at_channel_deinit(void)
{
    int i = 0;

    for (i = 0; i < AT_DATA_CHANNEL_NUM; ++i) {
        tos_at_gps_channel_free(i);
    }
}


__STATIC__ void at_event_table_set(at_event_t *event_table, size_t event_table_size)
{
    AT_AGENT->event_table       = event_table;
    AT_AGENT->event_table_size  = event_table_size;
}


__API__ int tos_at_gps_init(hal_uart_port_t uart_port, at_event_t *event_table, size_t event_table_size)
{
    void *buffer = K_NULL;

    memset(AT_AGENT, 0, sizeof(at_agent_t));

    at_event_table_set(event_table, event_table_size);

	at_channel_init();

    buffer = tos_mmheap_alloc(AT_RX_BUFFER_SIZE);
    if (!buffer) {
        return -1;
    }

    AT_AGENT->cmd_buffer       = buffer;
    AT_AGENT->cmd_buffer_size  = AT_RX_BUFFER_SIZE;

    buffer = tos_mmheap_alloc(AT_RX_BUFFER_SIZE * 2);
    if (!buffer) {
        return -1;
    }

    AT_AGENT->at_rx_fifo_buffer = (uint8_t *)buffer;
    tos_chr_fifo_create(&AT_AGENT->at_rx_fifo, buffer, AT_RX_BUFFER_SIZE * 2);

    if (tos_sem_create(&AT_AGENT->at_rx_sem, (k_sem_cnt_t)0u) != K_ERR_NONE) {
        goto errout0;
    }

    if (tos_task_create(&AT_AGENT->parser, "at_parser", at_parser,
                        K_NULL, AT_PARSER_TASK_PRIO, at_parser_task_stack,
                        AT_PARSER_TASK_STACK_SIZE, 0) != K_ERR_NONE) {
        goto errout1;
    }

	if (tos_hal_uart_init(&AT_AGENT->uart, uart_port) != 0) {
        goto errout1;
    }

    return 0;

errout1:
    tos_sem_destroy(&AT_AGENT->at_rx_sem);

errout0:
    tos_mmheap_free(AT_AGENT->at_rx_fifo_buffer);
    AT_AGENT->at_rx_fifo_buffer = K_NULL;
    tos_chr_fifo_destroy(&AT_AGENT->at_rx_fifo);

    return -1;
}

__API__ void tos_at_gps_deinit(void)
{
    tos_task_destroy(&AT_AGENT->parser);
    tos_sem_destroy(&AT_AGENT->at_rx_sem);
	tos_hal_uart_deinit(&AT_AGENT->uart);

    tos_mmheap_free(AT_AGENT->at_rx_fifo_buffer);
    AT_AGENT->at_rx_fifo_buffer = K_NULL;
    tos_chr_fifo_destroy(&AT_AGENT->at_rx_fifo);

	at_channel_deinit();
}

__API__ void tos_at_gps_send(const char *format, ...)
{
    va_list args;
	size_t cmd_len = 0;
    static char buffer[AT_OUTPUT_MAX];

    va_start(args, format);
    cmd_len =vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    tos_hal_uart_write(&AT_AGENT->uart, buffer, cmd_len, 0xFFFF);
}

__API__ void tos_at_gps_uart_input_byte(uint8_t data)
{
    if (tos_chr_fifo_push(&AT_AGENT->at_rx_fifo, data) == K_ERR_NONE) {
        tos_sem_post(&AT_AGENT->at_rx_sem);
    }
}

