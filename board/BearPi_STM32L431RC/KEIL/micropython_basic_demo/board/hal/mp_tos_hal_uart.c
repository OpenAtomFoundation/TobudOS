#include "tos_k.h"
#include "mp_tos_hal_uart.h"
#include "stm32l4xx_hal.h"
#include "usart.h"

__API__ int tos_hal_uart_recv_start(hal_uart_t *uart, const uint8_t *buf, size_t size) 
{
    HAL_StatusTypeDef hal_status;
    UART_HandleTypeDef *uart_handle;

    if (!uart || !buf) {
        return -1;
    }

    if (!uart->private_uart) {
        return -1;
    }

    uart_handle = (UART_HandleTypeDef *)uart->private_uart;

    hal_status = HAL_UART_Receive_IT(uart_handle, (uint8_t *)buf, size);
    if (hal_status != HAL_OK) {
        return -1;
    }
    return 0;
}
