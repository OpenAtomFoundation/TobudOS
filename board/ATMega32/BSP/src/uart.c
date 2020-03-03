#include "iom32.h"
#include "uart.h"

#define F_CPU 16777216

void uart_send_byte(char data)
{
    while (!(UCSRA & (1 << UDRE))) {
        ;
    }
    UDR = data;
}

void uart_send_string(char *string)
{
    for(; *string!='\0'; string++) {
        uart_send_byte(*string);
    }
}

unsigned char uart_receive(void)
{
    while (!(UCSRA & (1 << RXC))) {
        ;
    }
    return UDR;
}

int putchar(int data)
{
    uart_send_byte(data);
}

void uart_init(unsigned int baudrate)
{
    unsigned int tmp;
    tmp = F_CPU / baudrate / 16 - 1;

    /* Set baud rate */
    UBRRH = (unsigned char)(tmp >> 8);
    UBRRL = (unsigned char)tmp;
    /* Enable receiver and transmitter, enable receive interrupt */
    UCSRB = (1 << RXCIE) | (1 << RXEN) | (1 << TXEN);
    /* 8bit, 1 stopbit */
    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1<< UCSZ0);
}

