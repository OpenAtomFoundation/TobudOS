#include "tos_k.h"
#include "tos_hal.h"
#include "gd32vf103.h"
#include "usart.h"

typedef struct __UART_HandleTypeDef {
    hal_uart_port_t port;
} UART_HandleTypeDef;


UART_HandleTypeDef huart0 = { USART0 };
UART_HandleTypeDef huart1 = { USART1 };
UART_HandleTypeDef huart2 = { USART2 };

__API__ int tos_hal_uart_init(hal_uart_t *uart, hal_uart_port_t port)
{
    if (!uart) {
        return -1;
    }

    if (port == HAL_UART_PORT_0) {
        uart->private_uart = &huart0;
        usart0_init(115200);
    } else if (port == HAL_UART_PORT_1) {
        uart->private_uart = &huart1;
        usart1_init(115200);
    } else if (port == HAL_UART_PORT_2) {
        uart->private_uart = &huart2;
        usart2_init(115200);
    } else {
        return -1;
    }


    uart->port = ((UART_HandleTypeDef*)(uart->private_uart))->port;



    return 0;
}

__API__ int tos_hal_uart_write(hal_uart_t *uart, const uint8_t *buf, size_t size, uint32_t timeout)
{
    UART_HandleTypeDef *uart_handle;

    if (!uart || !buf) {
        return -1;
    }

    if (!uart->private_uart) {
        return -1;
    }

    uart_handle = (UART_HandleTypeDef *)uart->private_uart;

    for(size_t i=0; i<size; i++) {
        usart_data_transmit(uart_handle->port, buf[i]);
        tos_task_delay(1);
    }


    return 0;
}

__API__ int tos_hal_uart_read(hal_uart_t *uart, const uint8_t *buf, size_t size, uint32_t timeout)
{
    return 0;
}

__API__ int tos_hal_uart_deinit(hal_uart_t *uart)
{
    UART_HandleTypeDef *uart_handle;

    if (!uart) {
        return -1;
    }

    if (!uart->private_uart) {
        return -1;
    }

    uart_handle = (UART_HandleTypeDef *)uart->private_uart;


    usart_deinit(uart_handle->port);


    return 0;
}

