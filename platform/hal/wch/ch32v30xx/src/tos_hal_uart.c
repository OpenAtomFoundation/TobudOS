#include "tos_k.h"
#include "tos_hal.h"
#include "ch32v30x.h"
#include "usart.h"

__API__ int tos_hal_uart_init(hal_uart_t *uart, hal_uart_port_t port)
{
    if (!uart) {
        return -1;
    }

    if (port == HAL_UART_PORT_0) {
        uart->private_uart = UART7;
        uart7_init(115200);
    } else if (port == HAL_UART_PORT_2) {
        uart->private_uart = USART2;
        usart2_init(115200);
    } else if (port == HAL_UART_PORT_6) {
        uart->private_uart = UART6;
        uart6_init(115200);
    } else {
        return -1;
    }

    return 0;
}

__API__ int tos_hal_uart_write(hal_uart_t *uart, const uint8_t *buf, size_t size, uint32_t timeout)
{
    USART_TypeDef *base;

    if (!uart || !buf) {
        return -1;
    }

    if (!uart->private_uart) {
        return -1;
    }
    base = (USART_TypeDef *)uart->private_uart;

    for(size_t i=0; i<size; i++) {
        USART_SendData(base, buf[i]);
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
    //USART_TypeDef *base;

    if (!uart) {
        return -1;
    }

    if (!uart->private_uart) {
        return -1;
    }

    //base = (USART_TypeDef *)uart->private_uart;



    //usart_deinit(base);


    return 0;
}

