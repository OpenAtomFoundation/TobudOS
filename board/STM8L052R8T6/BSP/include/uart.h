#ifndef __USART_H_
#define __USART_H_

void UART1_Init(uint32_t uiBaudRate);
void UART1_Send_Byte(uint8_t ucData);
void UART1_Send_String(char *Str);
void UART1_Send_Dec(unsigned int num, unsigned char ucNumCount);

#endif 

