#ifndef _MP_TOS_HAL_UART_H_
#define _MP_TOS_HAL_UART_H_

#include "tos_hal_uart.h"

__API__ int tos_hal_uart_recv_start(hal_uart_t *uart, const uint8_t *buf, size_t size);

#endif
