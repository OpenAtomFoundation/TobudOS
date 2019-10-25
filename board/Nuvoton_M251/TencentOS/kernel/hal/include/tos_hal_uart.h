#ifndef _TOS_HAL_UART_H_
#define  _TOS_HAL_UART_H_

typedef enum hal_uart_port_en {
    HAL_UART_PORT_0 = 0,
    HAL_UART_PORT_1,
    HAL_UART_PORT_2,
    HAL_UART_PORT_3,
    HAL_UART_PORT_4,
    HAL_UART_PORT_5,
    HAL_UART_PORT_6,
} hal_uart_port_t;

typedef struct hal_uart_st {
    hal_uart_port_t port;
    void           *private_uart;
} hal_uart_t;

__API__ int tos_hal_uart_init(hal_uart_t *uart, hal_uart_port_t port);

__API__ int tos_hal_uart_write(hal_uart_t *uart, const uint8_t *buf, size_t size, uint32_t timeout);

__API__ int tos_hal_uart_read(hal_uart_t *uart, const uint8_t *buf, size_t size, uint32_t timeout);

__API__ int tos_hal_uart_deinit(hal_uart_t *uart);

#endif

