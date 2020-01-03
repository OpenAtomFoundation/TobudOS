#include "tos_k.h"
#include "tos_hal.h"
#include "n32g020xx_common.h"
#include "n32g020xx.h"
#include "n32g020xx_timer.h"
#include "n32g020xx_uart.h"
#include "n32g020xx_it.h"

typedef struct __UART_HandleTypeDef
{
    UartChannel channel;
} UART_HandleTypeDef;

typedef enum {
  HAL_ERROR = -1,
  HAL_OK = 0x00U,

  HAL_BUSY = 0x02U,
  HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;

UART_HandleTypeDef huart0;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

void NBIoT_UartRcvProtocal(int32_t val) { AT_UART_DataParse((char)val); }

__API__ int tos_hal_uart_init(hal_uart_t *uart, hal_uart_port_t port)
{
    UartConfig config;

    if (!uart) {
        return -1;
    }

    if (port == HAL_UART_PORT_0) {
        huart0.channel = UART_CH0;
        uart->private_uart = &huart0;
        config.frequence   = UART_FREQ_INTERAL;
        config.baudrate    = 9600;
        config.parity_type = UART_PARITY_NONE;
        config.callback    = NBIoT_UartRcvProtocal;
        config.io          = UART_CH0_GPIO22_GPIO23;
        if (UART_SUCCESS != UART_Init(UART_CH0, &config))
        {
            printf("Init uart int failed.\r\n");
        }
    } else if (port == HAL_UART_PORT_1) {
        huart0.channel = UART_CH1;
        uart->private_uart = &huart1;
    } else if (port == HAL_UART_PORT_2) {
        uart->private_uart = &huart2;
        huart0.channel = UART_CH2;
        uart->private_uart = &huart2;
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

    hal_status = UART_SendData(uart_handle->channel, (uint8_t *)buf, size);
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

    hal_status = UART_Close(uart_handle->channel);

    if (hal_status != HAL_OK) {
        return -1;
    }

    return 0;
}

