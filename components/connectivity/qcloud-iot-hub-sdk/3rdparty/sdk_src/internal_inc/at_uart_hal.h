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

#ifndef __AT_UART_HAL_H__
#define __AT_UART_HAL_H__

/*
 * UART data width
 */
typedef enum {
    BAUDRATE_2400    = 2400,
    BAUDRATE_4800    = 4800,
    BAUDRATE_9600    = 9600,
    BAUDRATE_19200   = 19200,
    BAUDRATE_115200  = 115200,
    BAUDRATE_921600  = 921600,
    BAUDRATE_DEFAULT = 115200
} hal_uart_baudr_t;

/*
 * UART data width
 */
typedef enum {
    DATA_WIDTH_5BIT,
    DATA_WIDTH_6BIT,
    DATA_WIDTH_7BIT,
    DATA_WIDTH_8BIT,
    DATA_WIDTH_9BIT
} hal_uart_data_width_t;

/*
 * UART stop bits
 */
typedef enum { STOP_BITS_1, STOP_BITS_2 } hal_uart_stop_bits_t;

/*
 * UART flow control
 */
typedef enum {
    FLOW_CONTROL_DISABLED,
    FLOW_CONTROL_CTS,
    FLOW_CONTROL_RTS,
    FLOW_CONTROL_CTS_RTS
} hal_uart_flow_control_t;

/*
 * UART parity
 */
typedef enum { NO_PARITY, ODD_PARITY, EVEN_PARITY } hal_uart_parity_t;

/*
 * UART mode
 */
typedef enum { MODE_TX, MODE_RX, MODE_TX_RX } hal_uart_mode_t;

/*
 * UART state
 */
typedef enum {
    eUNUSED = 0,
    eOPENED = 1,
    eCLOSED = 2,
} hal_uart_state_t;

/*
 * UART configuration
 */
typedef struct {
    uint32_t                baud_rate;
    hal_uart_data_width_t   data_width;
    hal_uart_parity_t       parity;
    hal_uart_stop_bits_t    stop_bits;
    hal_uart_flow_control_t flow_control;
    hal_uart_mode_t         mode;
} uart_config_t;

typedef struct {
#ifdef __linux__
    int fd; /* uart fd */
#else
    void *uart_handle; /* uart handle,like stm32 UART_HandleTypeDef */
#endif
    hal_uart_state_t state;  /* uart state */
    uart_config_t    config; /* uart config */
} uart_dev_t;

#ifdef __cplusplus
}
#endif

#endif /* __AT_H__ */
