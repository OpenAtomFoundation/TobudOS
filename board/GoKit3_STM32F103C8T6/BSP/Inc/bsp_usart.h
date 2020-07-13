#ifndef __USART_H
#define	__USART_H

#include "stm32f1xx.h"
#include <stdio.h>

//串口波特率
#define DEBUG_USART_BAUDRATE                    115200

//引脚定义
/*******************************************************/
#define DEBUG_USART                             USART1
#define DEBUG_USART_CLK_ENABLE()                __HAL_RCC_USART1_CLK_ENABLE();

#define DEBUG_USART_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_RX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USART_RX_PIN                      GPIO_PIN_10

#define DEBUG_USART_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_TX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USART_TX_PIN                      GPIO_PIN_9

#define DEBUG_USART_IRQHandler                  USART1_IRQHandler
#define DEBUG_USART_IRQ                 		USART1_IRQn
/************************************************************/


//串口波特率
#define WIFI_USART_BAUDRATE                    115200

//引脚定义
/*******************************************************/
#define WIFI_USART                             USART2
#define WIFI_USART_CLK_ENABLE()                __HAL_RCC_USART2_CLK_ENABLE();

#define WIFI_USART_RX_GPIO_PORT                GPIOA
#define WIFI_USART_RX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define WIFI_USART_RX_PIN                      GPIO_PIN_3

#define WIFI_USART_TX_GPIO_PORT                GPIOA
#define WIFI_USART_TX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define WIFI_USART_TX_PIN                      GPIO_PIN_2

#define WIFI_USART_IRQHandler                  USART2_IRQHandler
#define WIFI_USART_IRQ                 		   USART2_IRQn
/************************************************************/

void wifi_usart_init(void);
void debug_usart_init(void);
void usart_send_string(UART_HandleTypeDef *huart, uint8_t *str);
int fputc(int ch, FILE *f);
int fgetc(FILE *f);

extern UART_HandleTypeDef debug_usart;
extern UART_HandleTypeDef wifi_usart;

#endif /* __USART_H */
