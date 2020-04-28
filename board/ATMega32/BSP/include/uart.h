#ifndef __USART_H_
#define __USART_H_

void uart_send_byte(char data);

void uart_send_string(char *string);

unsigned char uart_receive(void);

void uart_init(unsigned int baudrate);

#endif

