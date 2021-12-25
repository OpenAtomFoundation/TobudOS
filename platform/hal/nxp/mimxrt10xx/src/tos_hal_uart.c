#include "tos_k.h"
#include "tos_hal.h"
#include "fsl_lpuart.h"

/* Definition of the clock source frequency */
#define LPUART_CLOCK_SOURCE 80000000UL

__API__ int tos_hal_uart_init(hal_uart_t *uart, hal_uart_port_t port)
{
    status_t status;
    lpuart_config_t config;
    
    if (!uart) {
        return -1;
    }
    
    if (port == HAL_UART_PORT_0) {
        // not support.
        return -1;
    } else if (port == HAL_UART_PORT_1) {
        uart->private_uart = LPUART1;
        
        LPUART_GetDefaultConfig(&config);
        config.baudRate_Bps = 115200;
        config.enableTx     = true;
        config.enableRx     = true;
        status = LPUART_Init(LPUART1, &config, LPUART_CLOCK_SOURCE);
        if (status != kStatus_Success) {
            return -1;
        }
        
         /* Enable RX interrupt. */
        LPUART_EnableInterrupts(LPUART1, kLPUART_RxDataRegFullInterruptEnable);
        EnableIRQ(LPUART1_IRQn);
    } else if (port == HAL_UART_PORT_2) {
        uart->private_uart = LPUART2;
        
        LPUART_GetDefaultConfig(&config);
        config.baudRate_Bps = 115200;
        config.enableTx     = true;
        config.enableRx     = true;
        status = LPUART_Init(LPUART2, &config, LPUART_CLOCK_SOURCE);
        if (status != kStatus_Success) {
            return -1;
        }
        
         /* Enable RX interrupt. */
        LPUART_EnableInterrupts(LPUART2, kLPUART_RxDataRegFullInterruptEnable);
        EnableIRQ(LPUART2_IRQn);
    } else if (port == HAL_UART_PORT_3) {
        uart->private_uart = LPUART3;
        
        LPUART_GetDefaultConfig(&config);
        config.baudRate_Bps = 115200;
        config.enableTx     = true;
        config.enableRx     = true;
        status = LPUART_Init(LPUART3, &config, LPUART_CLOCK_SOURCE);
        if (status != kStatus_Success) {
            return -1;
        }
        
         /* Enable RX interrupt. */
        LPUART_EnableInterrupts(LPUART3, kLPUART_RxDataRegFullInterruptEnable);
        EnableIRQ(LPUART3_IRQn);
    } else if (port == HAL_UART_PORT_4) {
        uart->private_uart = LPUART4;
        
        LPUART_GetDefaultConfig(&config);
        config.baudRate_Bps = 115200;
        config.enableTx     = true;
        config.enableRx     = true;
        status = LPUART_Init(LPUART4, &config, LPUART_CLOCK_SOURCE);
        if (status != kStatus_Success) {
            return -1;
        }
        
         /* Enable RX interrupt. */
        LPUART_EnableInterrupts(LPUART4, kLPUART_RxDataRegFullInterruptEnable);
        EnableIRQ(LPUART4_IRQn);
    }

    return 0;
}

__API__ int tos_hal_uart_write(hal_uart_t *uart, const uint8_t *buf, size_t size, uint32_t timeout)
{
    status_t status;
    LPUART_Type *base;

    if (!uart || !buf) {
        return -1;
    }

    if (!uart->private_uart) {
        return -1;
    }

    base = (LPUART_Type *)uart->private_uart;

    status = LPUART_WriteBlocking(base, (uint8_t *)buf, size);
    if (status != kStatus_Success) {
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
    LPUART_Type *base;

    if (!uart) {
        return -1;
    }

    if (!uart->private_uart) {
        return -1;
    }

    base = (LPUART_Type *)uart->private_uart;

    LPUART_Deinit(base);

    return 0;
}

