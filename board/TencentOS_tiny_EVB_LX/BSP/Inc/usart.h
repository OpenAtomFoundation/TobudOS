#ifndef __USART_H
#define __USART_H

#define USART0_GPIO_TX_PIN  GPIO_PIN_9
#define USART0_GPIO_RX_PIN  GPIO_PIN_10
#define USART0_GPIO_PORT    GPIOA

#define USART3_GPIO_TX_PIN  GPIO_PIN_10
#define USART3_GPIO_RX_PIN  GPIO_PIN_11
#define USART3_GPIO_PORT    GPIOC
void usart0_init(int baud);
void uart3_init(int baud);
#endif // __USART_H
