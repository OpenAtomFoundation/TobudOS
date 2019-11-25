#include "tos_at.h"

__STATIC__ at_agent_t at_agent;

__STATIC__ k_stack_t at_parser_task_stack[AT_PARSER_TASK_STACK_SIZE];

__API__ int tos_at_global_lock_pend(void)
{
    if (tos_mutex_pend(&AT_AGENT->global_lock) != K_ERR_NONE) {
        return -1;
    }
    return 0;
}

__API__ int tos_at_global_lock_post(void)
{
    if (tos_mutex_post(&AT_AGENT->global_lock) != K_ERR_NONE) {
        return -1;
    }
    return 0;
}

__STATIC__ int at_uart_getchar(uint8_t *data, k_tick_t timeout)
{
    k_err_t err;

    at_delay(1);

    if (tos_sem_pend(&AT_AGENT->uart_rx_sem, timeout) != K_ERR_NONE) {
        return -1;
    }

    if (tos_mutex_pend(&AT_AGENT->uart_rx_lock) != K_ERR_NONE) {
        return -1;
    }

    err = tos_chr_fifo_pop(&AT_AGENT->uart_rx_fifo, data);

    tos_mutex_post(&AT_AGENT->uart_rx_lock);

    return err == K_ERR_NONE ? 0 : -1;
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
    at_cache_t *at_cache = K_NULL;

    at_cache = &AT_AGENT->recv_cache;

    buffer = (char *)at_cache->buffer;
    buffer_len = at_cache->recv_len;

    return at_event_do_get(buffer, buffer_len);
}

__API__ int tos_at_uart_read(uint8_t *buffer, size_t buffer_len)
{
    uint8_t data;
    size_t read_len = 0;

    while (K_TRUE) {
        if (at_uart_getchar(&data, TOS_TIME_FOREVER) != 0) {
            return read_len;
        }

        buffer[read_len++] = data;

        if (read_len == buffer_len) {
            return buffer_len;
        }
    }
}

__API__ int tos_at_uart_readline(uint8_t *buffer, size_t buffer_len)
{
    uint8_t data;
    size_t read_len = 0;

    while (K_TRUE) {
        if (at_uart_getchar(&data, TOS_TIME_FOREVER) != 0) {
            return read_len;
        }

        buffer[read_len++] = data;

        if (data == '\n') {
            return read_len;
        } else if (read_len == buffer_len) {
            return buffer_len;
        }
    }
}

__API__ int tos_at_uart_drain(uint8_t *buffer, size_t buffer_len)
{
    uint8_t data;
    size_t read_len = 0;

    while (K_TRUE) {
        if (at_uart_getchar(&data, TOS_TIME_NOWAIT) != 0) {
            return read_len;
        }

        buffer[read_len++] = data;

        if (read_len == buffer_len) {
            return buffer_len;
        }
    }
}

__STATIC__ int at_is_echo_expect(void)
{
    char *recv_buffer, *expect;
    size_t recv_buffer_len, expect_len;
    at_echo_t *at_echo = K_NULL;
    at_cache_t *at_cache = K_NULL;

    at_echo = AT_AGENT->echo;

    if (!at_echo || !at_echo->echo_expect) {
        return 0;
    }

    at_cache = &AT_AGENT->recv_cache;

    recv_buffer = (char *)at_cache->buffer;
    recv_buffer_len = at_cache->recv_len;

    expect = at_echo->echo_expect;
    expect_len = strlen(expect);

    if (recv_buffer_len < expect_len) {
        return 0;
    }

    if (strncmp(expect, recv_buffer, expect_len) == 0) {
        return 1;
    }

    return 0;
}

__STATIC__ at_parse_status_t at_uart_line_parse(void)
{
    size_t curr_len = 0;
    uint8_t data, last_data = 0;
    at_cache_t *recv_cache = K_NULL;

    recv_cache = &AT_AGENT->recv_cache;

    recv_cache->recv_len = 0;
    memset(recv_cache->buffer, 0, recv_cache->buffer_size);

    while (K_TRUE) {
        if (at_uart_getchar(&data, TOS_TIME_FOREVER) != 0) {
            continue;
        }

        if (data == '\0') {
            continue;
        }

        if (curr_len < recv_cache->buffer_size) {
            recv_cache->buffer[curr_len++] = data;
            recv_cache->recv_len = curr_len;
        } else {
            recv_cache->buffer[recv_cache->buffer_size - 1] = '\0';
            return AT_PARSE_STATUS_OVERFLOW;
        }

        if (at_get_event() != K_NULL) {
            return AT_PARSE_STATUS_EVENT;
        }

        if (at_is_echo_expect()) {
            return AT_PARSE_STATUS_EXPECT;
        }

        if (data == '\n' && last_data == '\r') { // 0xd 0xa
            curr_len -= 1;
            recv_cache->buffer[curr_len - 1] = '\n';
            recv_cache->recv_len = curr_len;

            if (curr_len == 1) { // only a blank newline, ignore
                last_data = 0;
                curr_len = 0;
                recv_cache->recv_len = 0;
                continue;
            }

            return AT_PARSE_STATUS_NEWLINE;
        }

        last_data = data;
    }
}

__STATIC__ void at_echo_status_set(at_echo_t *echo)
{
    char *buffer;
    at_cache_t *at_cache;

    at_cache = &AT_AGENT->recv_cache;

    buffer = (char *)at_cache->buffer;

    if (strstr(buffer, AT_AGENT_ECHO_OK) != K_NULL) {
        echo->status = AT_ECHO_STATUS_OK;
    } else if (strstr(buffer, AT_AGENT_ECHO_ERROR) != K_NULL) {
        echo->status = AT_ECHO_STATUS_ERROR;
    } else if (strstr(buffer, AT_AGENT_ECHO_FAIL) != K_NULL) {
        echo->status = AT_ECHO_STATUS_FAIL;
    }
}

__STATIC__ void at_echo_buffer_copy(at_cache_t *at_cache, at_echo_t *echo)
{
    uint8_t *recv_buffer = K_NULL;
    size_t recv_buffer_len, copy_len, remain_len;

    recv_buffer = at_cache->buffer;
    recv_buffer_len = at_cache->recv_len;

    remain_len = echo->buffer_size - echo->__w_idx;
    if (remain_len == 0) {
        return;
    }

    copy_len = remain_len < recv_buffer_len ? remain_len : recv_buffer_len;
    memcpy(echo->buffer + echo->__w_idx, recv_buffer, copy_len);
    echo->__w_idx += copy_len;

    ++echo->line_num;
}

__STATIC__ void at_parser(void *arg)
{
    at_echo_t *at_echo = K_NULL;
    at_event_t *at_event = K_NULL;
    at_cache_t *recv_cache = K_NULL;
    at_parse_status_t at_parse_status;

    recv_cache = &AT_AGENT->recv_cache;

    while (K_TRUE) {
        at_parse_status = at_uart_line_parse();

        if (at_parse_status == AT_PARSE_STATUS_OVERFLOW) {
            // TODO: fix me
            continue;
        }

        if (at_parse_status == AT_PARSE_STATUS_EVENT) {
            at_event = at_get_event();
            if (at_event && at_event->event_callback) {
                at_event->event_callback();
            }
            continue;
        }

        at_echo = AT_AGENT->echo;
        if (!at_echo) {
            continue;
        }

        if (at_parse_status == AT_PARSE_STATUS_EXPECT) {
            at_echo->status = AT_ECHO_STATUS_EXPECT;
            if (at_echo->__is_expecting) {
                tos_sem_post(&at_echo->__expect_notify);
            }
        } else if (at_parse_status == AT_PARSE_STATUS_NEWLINE &&
                    at_echo->status == AT_ECHO_STATUS_NONE) {
            at_echo_status_set(at_echo);
        }

        if (at_echo->buffer) {
            at_echo_buffer_copy(recv_cache, at_echo);
        }

        tos_kprintln("--->%s", recv_cache->buffer);
    }
}

__STATIC__ int at_uart_send(const uint8_t *buf, size_t size, uint32_t timeout)
{
    int ret;

    tos_mutex_pend(&AT_AGENT->uart_tx_lock);
    ret = tos_hal_uart_write(&AT_AGENT->uart, buf, size, timeout);
    tos_mutex_post(&AT_AGENT->uart_tx_lock);

    return ret;
}

__API__ int tos_at_echo_create(at_echo_t *echo, char *buffer, size_t buffer_size, char *echo_expect)
{
    if (!echo) {
        return -1;
    }

    if (buffer) {
        memset(buffer, 0, buffer_size);
    }

    echo->buffer            = buffer;
    echo->buffer_size       = buffer_size;
    echo->echo_expect       = echo_expect;
    echo->line_num          = 0;
    echo->status            = AT_ECHO_STATUS_NONE;
    echo->__w_idx           = 0;
    echo->__is_expecting    = K_FALSE;
    return 0;
}

__STATIC_INLINE__ void at_echo_flush(at_echo_t *echo)
{
    echo->line_num  = 0;
    echo->status    = AT_ECHO_STATUS_NONE;
    echo->__w_idx   = 0;
}

__STATIC_INLINE void at_echo_attach(at_echo_t *echo)
{
    at_echo_flush(echo);
    AT_AGENT->echo = echo;
}

__API__ int tos_at_raw_data_send(at_echo_t *echo, uint32_t timeout, const uint8_t *buf, size_t size)
{
    int ret = 0;

    if (echo) {
        at_echo_attach(echo);
    }

    ret = at_uart_send(buf, size, 0xFFFF);

    tos_task_delay(tos_millisec2tick(timeout));

    AT_AGENT->echo = K_NULL;

    return ret;
}

__API__ int tos_at_raw_data_send_until(at_echo_t *echo, uint32_t timeout, const uint8_t *buf, size_t size)
{
    int ret = 0;

    if (!echo || !echo->echo_expect) {
        return -1;
    }

    if (tos_sem_create(&echo->__expect_notify, 0) != K_ERR_NONE) {
        return -1;
    }
    echo->__is_expecting = K_TRUE;
    at_echo_attach(echo);

    ret = at_uart_send(buf, size, 0xFFFF);

    if (tos_sem_pend(&echo->__expect_notify, tos_millisec2tick(timeout)) != K_ERR_NONE) {
        ret = -1;
    }

    tos_sem_destroy(&echo->__expect_notify);

    AT_AGENT->echo = K_NULL;

    return ret;
}

__STATIC__ int at_cmd_do_exec(const char *format, va_list args)
{
    size_t cmd_len = 0;

    if (tos_mutex_pend(&AT_AGENT->cmd_buf_lock) != K_ERR_NONE) {
        return -1;
    }

    cmd_len = vsnprintf(AT_AGENT->cmd_buf, AT_CMD_BUFFER_SIZE, format, args);

    printf("AT CMD:\n%s\n", AT_AGENT->cmd_buf);

    at_uart_send((uint8_t *)AT_AGENT->cmd_buf, cmd_len, 0xFFFF);

    tos_mutex_post(&AT_AGENT->cmd_buf_lock);

    return 0;
}

__API__ int tos_at_cmd_exec(at_echo_t *echo, uint32_t timeout, const char *cmd, ...)
{
    int ret = 0;
    va_list args;

    if (echo) {
        at_echo_attach(echo);
    }

    va_start(args, cmd);
    ret = at_cmd_do_exec(cmd, args);
    va_end(args);

    if (ret != 0) {
        AT_AGENT->echo = K_NULL;
        return -1;
    }

    tos_task_delay(tos_millisec2tick(timeout));

    AT_AGENT->echo = K_NULL;

    return 0;
}

__API__ int tos_at_cmd_exec_until(at_echo_t *echo, uint32_t timeout, const char *cmd, ...)
{
    int ret = 0;
    va_list args;

    if (!echo || !echo->echo_expect) {
        return -1;
    }

    if (tos_sem_create(&echo->__expect_notify, 0) != K_ERR_NONE) {
        return -1;
    }
    echo->__is_expecting = K_TRUE;
    at_echo_attach(echo);

    va_start(args, cmd);
    ret = at_cmd_do_exec(cmd, args);
    va_end(args);

    if (ret != 0) {
        AT_AGENT->echo = K_NULL;
        return -1;
    }

    if (tos_sem_pend(&echo->__expect_notify, tos_millisec2tick(timeout)) != K_ERR_NONE) {
        ret = -1;
    }

    tos_sem_destroy(&echo->__expect_notify);

    AT_AGENT->echo = K_NULL;

    return ret;
}

__STATIC__ int at_recv_cache_init(void)
{
    uint8_t *buffer = K_NULL;

    buffer = tos_mmheap_alloc(AT_RECV_CACHE_SIZE);
    if (!buffer) {
        AT_AGENT->recv_cache.buffer = K_NULL;
        return - 1;
    }

    AT_AGENT->recv_cache.buffer         = buffer;
    AT_AGENT->recv_cache.buffer_size    = AT_RECV_CACHE_SIZE;
    AT_AGENT->recv_cache.recv_len       = 0;
    return 0;
}

__STATIC__ void at_recv_cache_deinit(void)
{
    uint8_t *buffer = K_NULL;

    buffer = AT_AGENT->recv_cache.buffer;
    if (buffer) {
        tos_mmheap_free(buffer);
    }

    AT_AGENT->recv_cache.buffer         = K_NULL;
    AT_AGENT->recv_cache.buffer_size    = 0;
    AT_AGENT->recv_cache.recv_len       = 0;
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

__API__ int tos_at_channel_read(int channel_id, uint8_t *buffer, size_t buffer_len)
{
    int read_len;
    size_t total_read_len = 0;
    at_data_channel_t *data_channel = K_NULL;

    data_channel = at_channel_get(channel_id, K_FALSE);
    if (!data_channel || data_channel->status == AT_CHANNEL_STATUS_BROKEN) {
        return -1;
    }

    while (K_TRUE) {
        if (tos_mutex_pend(&data_channel->rx_lock) != K_ERR_NONE) {
            return total_read_len;
        }

        read_len = tos_chr_fifo_pop_stream(&data_channel->rx_fifo, buffer, buffer_len);

        tos_mutex_post(&data_channel->rx_lock);

        total_read_len += read_len;
        if (total_read_len < buffer_len) {
            continue;
        } else {
            return buffer_len;
        }
    }
}

__API__ int tos_at_channel_read_timed(int channel_id, uint8_t *buffer, size_t buffer_len, uint32_t timeout)
{
    int read_len = 0;
    size_t total_read_len = 0;
    k_tick_t tick, remain_tick;
    at_data_channel_t *data_channel = K_NULL;

    data_channel = at_channel_get(channel_id, K_FALSE);
    if (!data_channel || data_channel->status == AT_CHANNEL_STATUS_BROKEN) {
        return -1;
    }

    tick = tos_millisec2tick(timeout);

    at_timer_countdown(&AT_AGENT->timer, tick);
    while (!at_timer_is_expired(&AT_AGENT->timer)) {
        remain_tick = at_timer_remain(&AT_AGENT->timer);
        if (remain_tick == (k_tick_t)0u) {
            return total_read_len;
        }

        if (tos_mutex_pend_timed(&data_channel->rx_lock, remain_tick) != K_ERR_NONE) {
            return total_read_len;
        }

        read_len = tos_chr_fifo_pop_stream(&data_channel->rx_fifo, buffer + read_len, buffer_len - total_read_len);

        tos_mutex_post(&data_channel->rx_lock);

        total_read_len += read_len;
        if (total_read_len < buffer_len) {
            continue;
        } else {
            return buffer_len;
        }
    }

    return total_read_len;
}

__API__ int tos_at_channel_write(int channel_id, uint8_t *buffer, size_t buffer_len)
{
    int ret;
    at_data_channel_t *data_channel = K_NULL;

    data_channel = at_channel_get(channel_id, K_FALSE);
    if (!data_channel) {
        return -1;
    }

    if (tos_mutex_pend(&data_channel->rx_lock) != K_ERR_NONE) {
        return -1;
    }

    ret = tos_chr_fifo_push_stream(&data_channel->rx_fifo, buffer, buffer_len);

    tos_mutex_post(&data_channel->rx_lock);

    return ret;
}

__STATIC_INLINE__ int at_channel_construct(at_data_channel_t *data_channel, const char *ip, const char *port)
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
    data_channel->remote_ip = ip;
    data_channel->remote_port = port;

    data_channel->is_free   = K_FALSE;
    data_channel->status    = AT_CHANNEL_STATUS_WORKING;

    return 0;

errout:
    tos_mmheap_free(fifo_buffer);
    return -1;
}

__API__ int tos_at_channel_alloc_id(int channel_id, const char *ip, const char *port)
{
    at_data_channel_t *data_channel = K_NULL;

    data_channel = at_channel_get(channel_id, K_TRUE);
    if (!data_channel) {
        return -1;
    }

    if (at_channel_construct(data_channel, ip, port) != 0) {
        return -1;
    }

    return channel_id;
}

__API__ int tos_at_channel_alloc(const char *ip, const char *port)
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

    if (at_channel_construct(data_channel, ip, port) != 0) {
        return -1;
    }

    return id;
}

__API__ int tos_at_channel_free(int channel_id)
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

__API__ int tos_at_channel_set_broken(int channel_id)
{
    at_data_channel_t *data_channel = K_NULL;

    data_channel = at_channel_get(channel_id, K_FALSE);
    if (!data_channel) {
        return -1;
    }

    data_channel->status = AT_CHANNEL_STATUS_BROKEN;
    return 0;
}

__API__ int tos_at_channel_is_working(int channel_id)
{
    at_data_channel_t *data_channel = K_NULL;

    data_channel = at_channel_get(channel_id, K_FALSE);
    return data_channel && data_channel->status == AT_CHANNEL_STATUS_WORKING;
}

__STATIC__ void at_channel_init(void)
{
    int i = 0;

    for (i = 0; i < AT_DATA_CHANNEL_NUM; ++i) {
        memset(&AT_AGENT->data_channel[i], 0, sizeof(at_data_channel_t));
        AT_AGENT->data_channel[i].is_free   = K_TRUE;
        AT_AGENT->data_channel[i].status    = AT_CHANNEL_STATUS_HANGING;
    }
}

__STATIC__ void at_channel_deinit(void)
{
    int i = 0;

    for (i = 0; i < AT_DATA_CHANNEL_NUM; ++i) {
        tos_at_channel_free(i);
    }
}

__API__ const char *tos_at_channel_ip_get(int channel_id)
{
    at_data_channel_t *data_channel = K_NULL;

    data_channel = at_channel_get(channel_id, K_FALSE);
    if (!data_channel) {
        return K_NULL;
    }

    return data_channel[channel_id].remote_ip;
}

__API__ const char *tos_at_channel_port_get(int channel_id)
{
    at_data_channel_t *data_channel = K_NULL;

    data_channel = at_channel_get(channel_id, K_FALSE);
    if (!data_channel) {
        return K_NULL;
    }

    return data_channel[channel_id].remote_port;
}

__STATIC__ void at_event_table_set(at_event_t *event_table, size_t event_table_size)
{
    AT_AGENT->event_table       = event_table;
    AT_AGENT->event_table_size  = event_table_size;
}

__API__ int tos_at_init(hal_uart_port_t uart_port, at_event_t *event_table, size_t event_table_size)
{
    void *buffer = K_NULL;

    memset(AT_AGENT, 0, sizeof(at_agent_t));

    at_event_table_set(event_table, event_table_size);

    at_channel_init();

    at_timer_init(&AT_AGENT->timer);

    buffer = tos_mmheap_alloc(AT_UART_RX_FIFO_BUFFER_SIZE);
    if (!buffer) {
        return -1;
    }

    AT_AGENT->uart_rx_fifo_buffer = (uint8_t *)buffer;
    tos_chr_fifo_create(&AT_AGENT->uart_rx_fifo, buffer, AT_UART_RX_FIFO_BUFFER_SIZE);

    buffer = tos_mmheap_alloc(AT_CMD_BUFFER_SIZE);
    if (!buffer) {
        goto errout0;
    }
    AT_AGENT->cmd_buf = (char *)buffer;

    if (tos_mutex_create(&AT_AGENT->cmd_buf_lock) != K_ERR_NONE) {
        goto errout1;
    }

    if (at_recv_cache_init() != 0) {
        goto errout2;
    }

    if (tos_sem_create(&AT_AGENT->uart_rx_sem, (k_sem_cnt_t)0u) != K_ERR_NONE) {
        goto errout3;
    }

    if (tos_mutex_create(&AT_AGENT->uart_rx_lock) != K_ERR_NONE) {
        goto errout4;
    }

    if (tos_mutex_create(&AT_AGENT->uart_tx_lock) != K_ERR_NONE) {
        goto errout5;
    }

    if (tos_task_create(&AT_AGENT->parser, "at_parser", at_parser,
                        K_NULL, AT_PARSER_TASK_PRIO, at_parser_task_stack,
                        AT_PARSER_TASK_STACK_SIZE, 0) != K_ERR_NONE) {
        goto errout6;
    }

    if (tos_hal_uart_init(&AT_AGENT->uart, uart_port) != 0) {
        goto errout7;
    }

    if (tos_mutex_create(&AT_AGENT->global_lock) != K_ERR_NONE) {
        goto errout8;
    }

    return 0;

errout8:
    tos_hal_uart_deinit(&AT_AGENT->uart);

errout7:
    tos_task_destroy(&AT_AGENT->parser);

errout6:
    tos_mutex_destroy(&AT_AGENT->uart_tx_lock);

errout5:
    tos_mutex_destroy(&AT_AGENT->uart_rx_lock);

errout4:
    tos_sem_destroy(&AT_AGENT->uart_rx_sem);

errout3:
    at_recv_cache_deinit();

errout2:
    tos_mutex_destroy(&AT_AGENT->cmd_buf_lock);

errout1:
    tos_mmheap_free(AT_AGENT->cmd_buf);
    AT_AGENT->cmd_buf = K_NULL;

errout0:
    tos_mmheap_free(AT_AGENT->uart_rx_fifo_buffer);
    AT_AGENT->uart_rx_fifo_buffer = K_NULL;
    tos_chr_fifo_destroy(&AT_AGENT->uart_rx_fifo);

    return -1;
}

__API__ void tos_at_deinit(void)
{
    tos_mutex_destroy(&AT_AGENT->global_lock);

    tos_hal_uart_deinit(&AT_AGENT->uart);

    tos_task_destroy(&AT_AGENT->parser);

    tos_mutex_destroy(&AT_AGENT->uart_tx_lock);

    tos_sem_destroy(&AT_AGENT->uart_rx_sem);

    at_recv_cache_deinit();

    tos_mutex_destroy(&AT_AGENT->cmd_buf_lock);

    tos_mmheap_free(AT_AGENT->cmd_buf);
    AT_AGENT->cmd_buf = K_NULL;

    tos_mmheap_free(AT_AGENT->uart_rx_fifo_buffer);
    AT_AGENT->uart_rx_fifo_buffer = K_NULL;

    tos_chr_fifo_destroy(&AT_AGENT->uart_rx_fifo);

    at_channel_deinit();
}

/* To completely decouple the uart intterupt and at agent, we need a more powerful
   hal(driver framework), that would be a huge work, we place it in future plans. */
__API__ void tos_at_uart_write_byte(uint8_t data)
{
    if (tos_chr_fifo_push(&AT_AGENT->uart_rx_fifo, data) == K_ERR_NONE) {
        tos_sem_post(&AT_AGENT->uart_rx_sem);
    }
}

