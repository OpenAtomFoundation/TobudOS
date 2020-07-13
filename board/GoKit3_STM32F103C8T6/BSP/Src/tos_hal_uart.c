#include "tos_k.h"
#include "tos_hal.h"
#include "stm32f1xx.h"
#include "bsp_usart.h"

__API__ int tos_hal_uart_init(hal_uart_t *uart, hal_uart_port_t port)
{
    if (!uart) {
        return -1;
    }
    if (port == HAL_UART_PORT_1) {
        uart->private_uart = &debug_usart;
        debug_usart_init();
    } else if (port == HAL_UART_PORT_2) {
        uart->private_uart = &wifi_usart;
        wifi_usart_init();
    } 
    
    return 0;
}

__API__ int tos_hal_uart_write(hal_uart_t *uart, const uint8_t *buf, size_t size, uint32_t timeout)
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

    hal_status = HAL_UART_Transmit(uart_handle, (uint8_t *)buf, size, timeout);
    if (hal_status != HAL_OK) {
        return -1;
    }
    return 0;
}

__API__ int tos_hal_uart_read(hal_uart_t *uart, const uint8_t *buf, size_t size, uint32_t timeout)
{
    return 0;
}

__API__ int tos_hal_uart_deinit(hal_uart_t *uart)
{
    HAL_StatusTypeDef hal_status;
    UART_HandleTypeDef *uart_handle;

    if (!uart) {
        return -1;
    }

    if (!uart->private_uart) {
        return -1;
    }

    uart_handle = (UART_HandleTypeDef *)uart->private_uart;

    hal_status = HAL_UART_DeInit(uart_handle);
    HAL_UART_MspDeInit(uart_handle);

    if (hal_status != HAL_OK) {
        return -1;
    }

    return 0;
}

