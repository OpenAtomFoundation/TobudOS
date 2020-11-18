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

#ifndef _TOS_AT_GPS_H_
#define _TOS_AT_GPS_H_

#include "tos_k.h"
#include "tos_hal.h"

#define AT_OUTPUT_MAX                        128
#define AT_RX_BUFFER_SIZE					 512
#define AT_PARSER_TASK_STACK_SIZE            2048
#define AT_PARSER_TASK_PRIO                  0

#define AT_DATA_CHANNEL_NUM                     3
#define AT_DATA_CHANNEL_FIFO_BUFFER_SIZE        (2048 + 1024)


typedef void (*at_event_callback_t)(char * str);

typedef struct at_event_st {
    const char         *event_header;
    at_event_callback_t event_callback;
} at_event_t;

typedef enum at_channel_status_en {
    AT_CHANNEL_STATUS_NONE,         /*< usually means we are try to get a channel status with invalid id */
    AT_CHANNEL_STATUS_HANGING,      /*< channel is not used */
    AT_CHANNEL_STATUS_WORKING,      /*< channel is being using */
    AT_CHANNEL_STATUS_BROKEN,       /*< channel is broken(module link to remote server is broken) */
} at_channel_status_t;

typedef struct at_data_channel_st {
    uint8_t             is_free;
    k_chr_fifo_t        rx_fifo;
    uint8_t            *rx_fifo_buffer;
    k_mutex_t           rx_lock;

    at_channel_status_t status;

} at_data_channel_t;



typedef struct at_control_st {
	at_data_channel_t   data_channel[AT_DATA_CHANNEL_NUM];
    k_task_t            parser;
	at_event_t     	*event_table;
	size_t          	event_table_size;
    char				*cmd_buffer;
    int                 cmd_buffer_size;
	hal_uart_t      	uart;
    k_sem_t             at_rx_sem;
    k_chr_fifo_t        at_rx_fifo;
    uint8_t				*at_rx_fifo_buffer;
} at_agent_t;

#define AT_AGENT   ((at_agent_t *)(&at_agent))

__API__ int tos_at_gps_init(hal_uart_port_t uart_port, at_event_t *event_table, size_t event_table_size);

__API__ void tos_at_gps_deinit(void);

__API__ void tos_at_gps_send(const char *format, ...);

__API__ void tos_at_gps_uart_input_byte(uint8_t data);

#endif /* _TOS_AT_GPS_H_ */

