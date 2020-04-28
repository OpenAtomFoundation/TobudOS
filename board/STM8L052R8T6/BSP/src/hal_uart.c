#include "tos_hal.h"
#include "stm8l15x.h"

int tos_hal_uart_init(hal_uart_t *uart, hal_uart_port_t port)
{

}

int tos_hal_uart_write(hal_uart_t *uart, const uint8_t *buf, size_t size, uint32_t timeout)
{
    int i = 0;

    for (i = 0; i < size; ++i) {
        UART1_Send_Byte(buf[i]);
    }
}

