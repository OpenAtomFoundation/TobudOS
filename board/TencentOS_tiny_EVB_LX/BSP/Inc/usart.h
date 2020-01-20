#ifndef __USART_H
#define __USART_H

#define USART0_GPIO_TX_PIN  GPIO_PIN_9
#define USART0_GPIO_RX_PIN  GPIO_PIN_10
#define USART0_GPIO_PORT    GPIOA

#define USART1_GPIO_TX_PIN  GPIO_PIN_2
#define USART1_GPIO_RX_PIN  GPIO_PIN_3
#define USART1_GPIO_PORT    GPIOA

#define USART2_GPIO_TX_PIN  GPIO_PIN_10
#define USART2_GPIO_RX_PIN  GPIO_PIN_11
#define USART2_GPIO_PORT    GPIOB


void usart0_init(int baud);
void usart1_init(int baud);
void usart2_init(int baud);
#endif // __USART_H
